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
/* #define CONFIG_TEGRA_ENABLE_UARTA */
/* #define CONFIG_TEGRA_ENABLE_UARTB */
/* #define CONFIG_TEGRA_ENABLE_UARTC */

#define BOARD_EXTRA_ENV_SETTINGS \
    /* Defines for inline script */ \
    "defines=" \
        /* FDT IDS FOR VERIFICATION */ \
        "setenv odin_id               0x4F44494E; " \
        "setenv modin_id              0x4F44494F; " \
        "setenv vali_id               0x56414C49; " \
        "setenv frig_id               0x46524947; " \
        /* STAGING ADDRESSES */ \
        "setenv fdt_staging_addr      0x94000000; " \
        "setenv boot_staging_addr     0x98000000; " \
        "setenv recovery_staging_addr 0x98000000; " \
        /* BOOTARGS FOR UART TYPES */ \
        "setenv uarta_args            \"no_console_suspend console=ttyS0,115200,8n1 androidboot.console=ttyS0\"; " \
        "setenv uartb_args            \"no_console_suspend console=ttyS1,115200,8n1 androidboot.console=ttyS1\"; " \
        "setenv uartc_args            \"no_console_suspend console=ttyS2,115200,8n1 androidboot.console=ttyS2\"; " \
        "setenv usblg_args            \"console=ttyGS0,115200,8n1 androidboot.console=ttyGS0\"; " \
        "setenv no_args               \"console=null\";\0" \
    "setup_env=" \
        "setenv boot_dir ${prefix}; " \
        "test -n ${id}                   || setenv id SWR-AND; " \
        "test -n ${emmc}                 || setenv emmc 0; " \
        "test -n ${device_serial}        || mmc info device_serial; " \
        "test -n ${fbconsole}            || setenv fbconsole 9; " \
        "test -n ${uart_port}            || setenv uart_port 0; " \
        "test -n ${r2p_action}           || setenv r2p_action bootloader; " \
        "test -n ${autoboot}             || setenv autoboot 0; " \
        "test -n ${autoboot_list}        || setenv autoboot_list 0; " \
        "test -n ${usb3_enable}          || setenv usb3_enable 0; " \
        "test -n ${4k60_disable}         || setenv 4k60_disable 0; " \
        "test -n ${dvfsb}                || setenv dvfsb 0; " \
        "test -n ${odin_oc}              || setenv oc 0; " \
        "test -n ${touch_skip_tuning}    || setenv touch_skip_tuning 0; " \
        "test -n ${jc_rail_disable}      || setenv jc_rail_disable 0; " \
        "test -n ${sd_1bit}              || setenv sd_1bit 0;\0" \
    "address_parse=" \
        "host_mac_addr=0xff; " \
        /* load mac address info file from sd */ \
        "if fatload mmc 1:1 0x90000000 switchroot/joycon_mac.bin; then " \
            "if itest.b *0x90000000 == 0x01; then " \
                "echo Left JoyCon is available; " \
                "host_mac_addr=0x90000007; " \
            "else " \
                "echo -e Left JoyCon pairing info is not available!; " \
            "fi; " \
            "if itest.b *0x9000001d == 0x02; then " \
                "echo Right JoyCon is available; " \
                "host_mac_addr=0x90000024; " \
            "else " \
                "echo -e Right JoyCon pairing info is not available!; " \
            "fi; " \
        "fi; " \
        "if itest.b $host_mac_addr == 0xff; then " \
            "echo -e No JoyCons available; " \
        "else " \
            "echo Generating MAC addresses with JoyCon pairing info; " \
            "bt_mac=\"\"; " \
            "sep=\"\"; " \
            "for i in 0 1 2 3 4 5 ; do " \
                "setexpr x $host_mac_addr + $i; " \
                "setexpr.b b *$x; " \
                "if itest $b <= f; then " \
                    /* There is no way to have leading zeros, so do this hack */ \
                    "bt_mac=\"${bt_mac}${sep}0${b}\"; " \
                    "echo bt_mac (a): ${bt_mac}; " \
                "else " \
                    "bt_mac=\"${bt_mac}${sep}${b}\"; " \
                    "echo bt_mac (b): ${bt_mac}; " \
                "fi; " \
                "sep=\":\"; " \
            "done; " \
            "setexpr.b last_byte *0x90000005; " \
            "if itest $last_byte == 0xFF; then " \
                /* wrap around case */ \
                "setexpr wifi_mac gsub \"(.*:.*:.*:.*:.*:).*\" \"\\100\" $bt_mac; " \
            "else " \
                "setexpr.b wb $last_byte + 1; " \
                "if itest $wb <= f; then " \
                    "setexpr wifi_mac gsub \"(.*:.*:.*:.*:.*:).*\" \"\\10$wb\" $bt_mac; " \
                "else " \
                    "setexpr wifi_mac gsub \"(.*:.*:.*:.*:.*:).*\" \"\\1$wb\" $bt_mac; " \
                "fi; " \
            "fi; " \
        "fi;\0" \
	"preboot=" \
        "if itest.l *0xA9FBFFFC == 0x33334C42; then " \
            "env import -t 0xA9FC0000 0x20000; " \
        "fi;" \
        "run defines; " \
        "run setup_env;\0" \
	"set_variant=" \
        "if test ${t210b01} = 1; then setenv plat_info T210B01; else setenv plat_info T210; fi; " \
        /* V1 SWITCH */ \
		"if   test ${sku} = 0; then " \
			"setenv dtid ${odin_id}; " \
			"setenv dtrev 0x0; " \
			"setenv variant odin; " \
        /* V2 SWITCH */ \
		"elif test ${sku} = 1; then " \
			"setenv dtid ${modin_id}; " \
			"setenv dtrev 0xb01; " \
			"setenv variant modin;" \
        /* SWITCH LITE */ \
		"elif test ${sku} = 2; then " \
			"setenv dtid ${vali_id}; " \
			"setenv dtrev 0x0; " \
			"setenv variant vali; " \
        /* SWITCH OLED */ \
		"elif test ${sku} = 3; then " \
			"setenv dtid ${frig_id}; " \
			"setenv dtrev 0x0; " \
			"setenv variant frig; " \
		"fi; " \
        "echo SKU: ${variant} REV: ${dtrev}; " \
        "echo Serial: ${device_serial};\0" \
	"default_target=" \
		"setenv mmcdev 1; " \
		"setenv bootpart 0; " \
		"mmc dev ${mmcdev};\0" \
	"emmc_target=" \
		"setenv mmcdev 0; " \
		"mmc dev ${mmcdev};\0" \
	"emmc_overlay=" \
		"fdt set /sdhci@700b0600 status okay; " \
		"fdt set /firmware/android boot_devices sdhci-tegra.3; " \
        "echo -e using eMMC;\0" \
	"sd_overlay=" \
		"fdt set /firmware/android boot_devices sdhci-tegra.0;\0" \
    "touch_overlay=" \
        "setenv bootargs ${bootargs} \"ftm4.skip_tuning=1\";\0" \
    "usb3_overlay=" \
    	"echo -e USB3 disabled; " \
	    "fdt get value DHANDLE_USB2 /xusb_padctl@7009f000/pads/usb2/lanes/usb2-0 phandle; " \
	    "fdt set /xusb@70090000 phys <$DHANDLE_USB2>; " \
	    "fdt set /xusb@70090000 phy-names usb2-0; " \
	    "fdt set /xudc@700d0000 phys <$DHANDLE_USB2>; " \
	    "fdt set /xudc@700d0000 phy-names usb2; " \
	    "fdt set /xusb_padctl@7009f000 usb3-no-mapping-war <0x1>; " \
	    "fdt set /xusb_padctl@7009f000/ports/usb2-0 nvidia,usb3-port-fake <0xffffffff>; " \
	    "fdt set /xusb_padctl@7009f000/ports/usb3-0 status disabled;\0" \
    "4k60_overlay=" \
        "fdt set /i2c@7000c000/bm92t@18 rohm,dp-lanes <2>; " \
        "echo -e 4K60 disabled;\0" \
    "1bit_overlay=" \
        "echo -e SD Card is initialized in 1-bit mode!; " \
        "fdt set /sdhci@700b0000 bus-width <0x1>; " \
        "fdt set /sdhci@700b0000 uhs-mask <0x7F>;\0" \
    "dvfs_enable=" \
        "echo -e DVFS B-Side enabled; " \
	    "setenv bootargs ${bootargs} speedo_tegra210.cspd_id=2 speedo_tegra210.cspd_id=2 speedo_tegra210.gspd_id=2 androidboot.dvfsb=1; " \
        "if test ${sku} != 2; then; " \
            /* 2397 MHz CPU and 1075 MHz GPU hard limit */ \
            "fdt set /cpufreq/cpu-scaling-data max-frequency <0x249348>; " \
            "fdt set /dvfs nvidia,gpu-max-freq-khz <0x106800>; " \
        "else; " \
            /* 2091 MHz CPU and 844 MHz GPU hard limit. Vali */ \
            "fdt set /cpufreq/cpu-scaling-data max-frequency <0x1FE7F8>; " \
            "fdt set /dvfs nvidia,gpu-max-freq-khz <0xCE400>; " \
        "fi;\0" \
    "oc_enable=" \
        "echo -e T210 Super Overclock enabled; " \
	    "setenv bootargs ${bootargs} androidboot.oc=1; " \
        "fdt set /cpufreq/cpu-scaling-data max-frequency <0x1FE7F8>;\0" \
    "jc_rail_overlay=" \
    	"echo -e Joycon Rails disabled; " \
        "fdt set /serial@70006040 status disabled; " \
        "fdt set /serial@70006040/joyconr status disabled; " \
        "fdt set /serial@70006200 status disabled; " \
        "fdt set /serial@70006200/joyconl status disabled;\0" \
    "vali_vlim_overlay=" \
        "echo VALI: voltage limits [${VLIM}, ${SOCLIM}]; " \
        "fdt set /i2c@7000c000/battery-charger@6b/charger ti,charge-voltage-limit-millivolt <0x$VLIM>; " \
        "fdt set /i2c@7000c000/battery-charger@6b/charger ti,charge-thermal-voltage-limit <0x$VLIM 0x$VLIM 0x$VLIM 0xFF0>; " \
        "fdt set /i2c@7000c000/battery-gauge@36 maxim,kernel-maximum-soc <0x$SOCLIM>;\0" \
	"display_overlay=" \
		"if   test ${display_id} = f20;  then echo Display is INN 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-i-720p-6-2 phandle; " \
		"elif test ${display_id} = f30;  then echo Display is AUO 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-a-720p-6-2 phandle; " \
		"elif test ${display_id} = 10;   then echo Display is JDI 6.2; fdt get value DHANDLE /host1x@50000000/dsi/panel-j-720p-6-2 phandle; " \
		"elif test ${display_id} = 1020; then echo Display is INN 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-i-720p-5-5 phandle; " \
		"elif test ${display_id} = 1030; then echo Display is AUO 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-a-720p-5-5 phandle; " \
		"elif test ${display_id} = 1040; then echo Display is SHP 5.5; fdt get value DHANDLE /host1x@50000000/dsi/panel-s-720p-5-5 phandle; " \
		"else echo -e Unknown Display ID: ${display_id}!; fi; " \
		"if test -n ${DHANDLE}; then echo Setting Display panel; fdt set /host1x/dsi nvidia,active-panel <$DHANDLE>; fi\0" \
	"get_fdt=" \
		"part start mmc ${mmcdev} DTB dtb_start; " \
		"part size mmc ${mmcdev} DTB dtb_size; " \
		"mmc read ${fdt_staging_addr} ${dtb_start} ${dtb_size}; " \
		"adtimg addr ${fdt_staging_addr}; " \
		"adtimg get dt --id=${dtid} dtaddr dtsize dtidx; " \
		"cp.b ${dtaddr} ${fdt_addr_r} ${dtsize};" \
		"fdt addr ${fdt_addr_r} ${dtsize};" \
        "fdt resize 16384\0" \
	"bootcmd_common=" \
		"run set_variant; " \
		"setenv bootargs init=/init nvdec_enabled=0 pcie_aspm=off vpr_resize tegra_fbmem=0x800000@0xf5a00000 loglevel=8; " \
		"setenv bootargs ${bootargs} androidboot.selinux=permissive firmware_class.path=/vendor/firmware; " \
        "setenv bootargs ${bootargs} pmc_r2p.action=${r2p_action} pmc_r2p.enabled=1 pmc_r2p.param1=${autoboot} pmc_r2p.param2=${autoboot_list}; " \
        "setenv bootargs ${bootargs} fbcon=map:${fbconsole} consoleblank=0; " \
		"if test -n $emmc; then run emmc_target; fi; " \
		"run get_fdt; " \
		"if test -n $emmc; then run emmc_overlay; else run sd_overlay; fi; " \
        "echo uart port (debug): ${uart_port}; " \
        /* UART-A (Onboard UART Port) */ \
        "if test ${uart_port} = 1; then " \
            "setenv bootargs \"${uarta_args} ${bootargs}\"; echo -e Enabled UART-A logging; " \
            "fdt set /serial@70006000 compatible nvidia,tegra20-uart; " \
            "fdt set /serial@70006000 status okay; " \
        /* UART-B (Right JoyCon Rail) */ \
        "elif test ${uart_port} = 2; then " \
            "setenv bootargs \"${uartb_args} ${bootargs}\"; echo -e Enabled UART-B logging; " \
            "fdt set /serial@70006040 compatible nvidia,tegra20-uart; " \
            "fdt set /serial@70006040/joyconr status disabled; " \
        /* UART-C (Left JoyCon Rail) */ \
        "elif test ${uart_port} = 3; then " \
            "setenv bootargs \"${uartc_args} ${bootargs}\"; echo -e Enabled UART-C logging; " \
            "fdt set /serial@70006200 compatible nvidia,tegra20-uart; " \
            "fdt set /serial@70006200/joyconl status disabled; " \
        /* USB Serial */ \
        "elif test ${uart_port} = 4; then " \
            "setenv bootargs \"${usblg_args} ${bootargs}\"; echo -e Enabled USB Serial logging; " \
        /* No serial console */ \
        "else; " \
            "setenv bootargs \"${bootargs} ${no_args}\"; " \
        "fi; " \
        "if test ${4k60_disable} = 1; then run 4k60_overlay; fi; " \
        "if test ${sd_1bit} = 1; then run 1bit_overlay; fi; " \
		"if test ${sku} != 3; then run display_overlay; fi; " \
        "if test ${t210b01} = 1 -a ${dvfsb} = 1; then run dvfs_enable; else setenv bootargs ${bootargs} androidboot.dvfsb=0; fi; " \
        "if test ${t210b01} = 0 -a ${oc} = 1; then run oc_enable; else setenv bootargs ${bootargs} androidboot.oc=0; fi; " \
        "if test ${sku} = 2 -a -n \"${VLIM}\"; then run vali_vlim_overlay; fi; " \
        "if test ${jc_rail_disable} = 1; then run jc_rail_overlay; fi; " \
        "if test ${touch_skip_tuning} = 1; then run touch_overlay; fi; " \
        "if test ${usb3_enable} = 0; then run usb3_overlay; else echo USB3 enabled; fi; " \
        /* Set default macs, to be overridden by joycons */ \
        "setenv wifi_mac=${device_wifi_mac}; " \
        "setenv bt_mac=${device_bt_mac}; " \
        "run address_parse; " \
        "echo BT MAC: ${bt_mac}; " \
        "echo WF MAC: ${wifi_mac}; " \
        /* insert mac address dtb node */ \
        "fdt set /chosen nvidia,wifi-mac ${wifi_mac}; " \
        "fdt set /chosen nvidia,bluetooth-mac ${bt_mac}; " \
        "if test -n ${device_serial}; then fdt set / serial-number ${device_serial}; fi; " \
		"setenv bootargs ${bootargs} androidboot.bootloader=${blver} androidboot.hardware=nx androidboot.hardware.sku=${variant} androidboot.serialno=${device_serial} androidboot.modem=none androidboot.dtb_idx=${dtidx};\0" \
	"bootcmd_android=" \
		"part number mmc ${mmcdev} APP app_part_num; " \
		"part uuid mmc ${mmcdev}:${app_part_num} app_part_uuid; " \
		"part start mmc ${mmcdev} LNX lnx_start; " \
		"part size mmc ${mmcdev} LNX lnx_size; " \
		"mmc read ${boot_staging_addr} ${lnx_start} ${lnx_size}; " \
		"setenv bootargs skip_initramfs ro rootwait root=PARTUUID=${app_part_uuid} ${bootargs} bluetooth.disable_ertm=1; " \
		"bootm ${boot_staging_addr} ${boot_staging_addr} ${fdt_addr_r};\0" \
	"bootcmd_recovery=" \
		"part start mmc ${mmcdev} SOS recovery_start; " \
		"part size mmc ${mmcdev} SOS recovery_size; " \
		"mmc read ${recovery_staging_addr} ${recovery_start} ${recovery_size}; " \
		"bootm ${recovery_staging_addr} ${recovery_staging_addr} ${fdt_addr_r};\0" \
	"bootcmd=" \
		"run default_target; " \
		"if test -n ${uenvcmd}; then " \
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
