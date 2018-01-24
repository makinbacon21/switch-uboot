/*
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <env.h>
#include <i2c.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/tegra.h>
#include <asm/arch-tegra/ap.h>
#include <asm/arch-tegra/gp_padctrl.h>
#include <asm/arch-tegra/pmc.h>
#include "../../nvidia/p2571/max77620_init.h"

void pin_mux_mmc(void)
{
	struct pmc_ctlr *const pmc = (struct pmc_ctlr *)NV_PA_PMC_BASE;
	struct udevice *dev;
	u32 reg_val;
	uchar val;
	int ret;

	/* Make sure the SDMMC1 controller is powered */
	reg_val = readl(&pmc->pmc_no_iopower);
	reg_val |= BIT(12);
	writel(reg_val, &pmc->pmc_no_iopower);
	(void)readl(&pmc->pmc_no_iopower);
	udelay(1000);
	reg_val &= ~(BIT(12));
	writel(reg_val, &pmc->pmc_no_iopower);
	(void)readl(&pmc->pmc_no_iopower);

	/* Inform IO pads that voltage is gonna be 3.3V */
	reg_val = readl(&pmc->pmc_pwr_det_val);
	reg_val |= BIT(12);
	writel(reg_val, &pmc->pmc_pwr_det_val);
	(void)readl(&pmc->pmc_pwr_det_val);

	/* Turn on MAX77620 LDO2 to 3.3V for SD card power */
	debug("%s: Set LDO2 for VDDIO_SDMMC_AP power to 3.3V\n", __func__);
	ret = i2c_get_chip_for_busnum(5, MAX77620_I2C_ADDR_7BIT, 1, &dev);
	if (ret) {
		printf("%s: Cannot find MAX77620 I2C chip\n", __func__);
		return;
	}
	/* 0xF2 for 3.3v, enabled: bit7:6 = 11 = enable, bit5:0 = voltage */
	val = 0xF2;
	ret = dm_i2c_write(dev, MAX77620_CNFG1_L2_REG, &val, 1);
	if (ret)
		printf("Failed to enable 3.3V LDO for SD Card IO: %d\n", ret);

	/* Disable LDO4 discharge for RTC power */
	ret = dm_i2c_read(dev, MAX77620_CNFG2_L4_REG, &val, 1);
	if (ret) {
		printf("Failed to read LDO4 register: %d\n", ret);
	} else {
		val &= ~BIT(1); /* ADE */
		ret = dm_i2c_write(dev, MAX77620_CNFG2_L4_REG, &val, 1);
		if (ret)
			printf("Failed to disable ADE in LDO4: %d\n", ret);
	}
}

enum {
	NX_HW_TYPE_ODIN,
	NX_HW_TYPE_MODIN,
	NX_HW_TYPE_VALI,
	NX_HW_TYPE_FRIG
};

#define FUSE_RESERVED_ODMX(x) (0x1C8 + 4 * (x))
static int get_sku(bool t210b01)
{
	const volatile void __iomem *odm4 = (void *)(NV_PA_FUSE_BASE + FUSE_RESERVED_ODMX(4));

	if (t210b01)
	{
		switch ((readl(odm4) & 0xF0000) >> 16)
		{
			case 2:
				return NX_HW_TYPE_VALI;

			case 4:
				return NX_HW_TYPE_FRIG;

			case 1:
			default:
				return NX_HW_TYPE_MODIN;
		}
	}

	return NX_HW_TYPE_ODIN;
}

#define SECURE_SCRATCH112_SETUP_DONE		0x34313254
#define PMC_SCRATCH0_RECOVERY_MODE		(1 << 31)
#define PMC_SCRATCH0_FASTBOOT_MODE		(1 << 30)
#define PMC_SCRATCH0_MASK (PMC_SCRATCH0_RECOVERY_MODE | PMC_SCRATCH0_FASTBOOT_MODE)
static void board_env_setup(void)
{
	u32 scratch0   = tegra_pmc_readl(offsetof(struct pmc_ctlr, pmc_scratch0));
	u32 scratch113 = tegra_pmc_readl(offsetof(struct pmc_ctlr, pmc_secure_scratch113));
	bool t210b01 = tegra_get_chip_rev() == MAJORPREV_TEGRA210B01;
	char sku[2] = { get_sku(t210b01) + '0', 0 };

	// If the mariko modchip is not initialized in a specific manner, things will hang.
	// Scratch112 gets set to this specific value on known good modchip payloads.
	if (t210b01 &&
	    tegra_pmc_readl(offsetof(struct pmc_ctlr, pmc_secure_scratch112))
			!= SECURE_SCRATCH112_SETUP_DONE) {
		puts("Board was not initialized properly! Hang prevented.\n"
			"Board will reboot in 10s..\n");
		mdelay(10000);

		psci_system_reset();

		return;
	}

	// Set soc type
	env_set("t210b01", t210b01 ? "1" : "0");

	// Set nx sku
	env_set("sku", sku);

	// Set Display ID
	env_set_hex("display_id", scratch113 & 0xFFFF);

	// Set charging limits for lite
	if (get_sku(t210b01) == NX_HW_TYPE_VALI) {
		//env_set_hex("VLIM", scratch113 >> 16);

		// TODO: Set soc limit
	}

	// If kernel set reboot to recovery, then set a var for scripts to action on
	env_set("recovery", scratch0 & PMC_SCRATCH0_RECOVERY_MODE ? "1" : "0");

	// Clear out scratch0 mode select bits
	tegra_pmc_writel(scratch0 & ~PMC_SCRATCH0_MASK, offsetof(struct pmc_ctlr, pmc_scratch0));
}

// This has nothing to do with a fan, but it gets called at the right time
void start_cpu_fan(void)
{
	board_env_setup();
}
