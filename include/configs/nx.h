/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2022
 * The LineageOS Project
 */

#ifndef _NX_H
#define _NX_H

#include <linux/sizes.h>

#include "tegra210-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"Nintendo Switch"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTB

#define BOARD_EXTRA_ENV_SETTINGS \
	"preboot=if itest.l *0xA9FBFFFC == 0x33334C42; then " \
		"env import -t 0xA9FC0000 0x20000; " \
	"fi\0" \
	"set_variant=" \
		"if   test ${sku} = 0; then " \
			"setenv dtid 0x4F44494E; " \
			"setenv dtrev 0; " \
			"setenv variant odin; " \
		"elif test ${sku} = 1; then " \
			"setenv dtid 0x4F44494E; " \
			"setenv dtrev 0xb01; " \
			"setenv variant modin;" \
		"elif test ${sku} = 2; then " \
			"setenv dtid 0x56414C49; " \
			"setenv dtrev 0; " \
			"setenv variant vali; " \
		"elif test ${sku} = 3; then " \
			"setenv dtid 0x46524947; " \
			"setenv dtrev 0; " \
			"setenv variant frig; " \
		"fi\0" \
	"default_target=" \
		"setenv mmcdev 1; " \
		"setenv bootpart 0; " \
		"mmc dev ${mmcdev};\0" \
	"emmc_target=" \
		"setenv mmcdev 0; " \
		"mmc dev ${mmcdev};\0" \
	"emmc_overlay=" \
		"fdt set /sdhci@700b0600 status okay; " \
		"fdt set /firmware/android boot_devices sdhci-tegra.3;\0" \
	"sd_overlay=" \
		"fdt set /firmware/android boot_devices sdhci-tegra.0;\0" \
	"uart_overlay=" \
		"fdt set /serial@70006040 compatible nvidia,tegra20-uart; " \
		"fdt set /serial@70006040 status okay; " \
		"fdt set /serial@70006040/joyconr status disabled;\0" \
	"display_overlay=" \
		"if   test ${display_id} = f20;  then echo Display is INN 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-i-720p-6-2 phandle; " \
		"elif test ${display_id} = f30;  then echo Display is AUO 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-a-720p-6-2 phandle; " \
		"elif test ${display_id} = 10;   then echo Display is JDI 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-j-720p-6-2 phandle; " \
		"elif test ${display_id} = 1020; then echo Display is INN 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-i-720p-5-5 phandle; " \
		"elif test ${display_id} = 1030; then echo Display is AUO 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-a-720p-5-5 phandle; " \
		"elif test ${display_id} = 1040; then echo Display is SHP 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-s-720p-5-5 phandle; " \
		"else echo Unknown Display ID: ${display_id}!; fi; " \
		"if test -n ${DHANDLE}; then echo Setting Display panel; fdt set /host1x/dsi nvidia,active-panel <$DHANDLE>; fi\0" \
	"get_fdt=" \
		"setenv fdt_staging 0x94000000; " \
		"part start mmc $mmcdev DTB dtb_start; " \
		"part size mmc $mmcdev DTB dtb_size; " \
		"mmc read $fdt_staging $dtb_start $dtb_size; " \
		"adtimg addr $fdt_staging; " \
		"adtimg get dt --id=${dtid} --rev=${dtrev} dtaddr dtsize dtidx; " \
		"cp.b ${dtaddr} ${fdt_addr_r} ${dtsize};" \
		"fdt addr ${fdt_addr_r} ${dtsize};\0" \
	"bootcmd_common=" \
		"run set_variant; " \
		"setenv bootargs \"init=/init nvdec_enabled=0 tegra_fbmem=0x384000@0xf5a00000\"; " \
		"setenv bootargs \"${bootargs} firmware_class.path=/vendor/firmware pmc_r2p.reboot_action=via-payload pmc_r2p.enabled=1\"; " \
		"if test -n $useemmc; then run emmc_target; fi; " \
		"run get_fdt; " \
		"if test -n $useemmc; then run emmc_overlay; else run sd_overlay; fi; " \
		"if test $uart_port = 2; then " \
			"run uart_overlay; " \
			"setenv bootargs \"${bootargs} no_console_suspend androidboot.console=ttyS1 console=ttyS1,115200,8n1\"; " \
		"else " \
			"setenv bootargs \"${bootargs} console=null\"; " \
		"fi; " \
		"if test ${sku} != 3; then run display_overlay; fi; " \
		"mmc info serial#; " \
		"setenv bootargs \"${bootargs} androidboot.bootloader=${blver} androidboot.hardware=nx androidboot.hardware.sku=${variant} androidboot.serialno=${serial#} androidboot.modem=none androidboot.dtb_idx=${dtidx}\";\0" \
	"bootcmd_android=" \
		"setenv boot_staging 0x98000000; " \
		"part number mmc ${mmcdev} APP app_part_num; " \
		"part uuid mmc ${mmcdev}:${app_part_num} app_part_uuid; " \
		"part start mmc ${mmcdev} LNX lnx_start; " \
		"part size mmc ${mmcdev} LNX lnx_size; " \
		"mmc read $boot_staging $lnx_start $lnx_size; " \
		"setenv bootargs \"skip_initramfs ro rootwait root=PARTUUID=${app_part_uuid} ${bootargs} bluetooth.disable_ertm=1\"; " \
		"bootm $boot_staging $boot_staging $fdt_addr_r;\0" \
	"bootcmd_recovery=" \
		"setenv recovery_staging 0x98000000; " \
		"part start mmc ${mmcdev} SOS recovery_start; " \
		"part size mmc ${mmcdev} SOS recovery_size; " \
		"mmc read $recovery_staging $recovery_start $recovery_size; " \
		"bootm $recovery_staging $recovery_staging $fdt_addr_r;\0" \
	"bootcmd=" \
		"run default_target; " \
		"if test -n $uenvcmd; then " \
			"echo Running uenvcmd ...; " \
			"run uenvcmd; " \
		"else " \
			"run bootcmd_common; " \
			"if gpio input 190 || test ${recovery} = \"1\"; then " \
				"run bootcmd_recovery; " \
			"else " \
				"run bootcmd_android; " \
			"fi; " \
		"fi;\0"


#include "tegra-common-post.h"

#endif /* _NX_H */
