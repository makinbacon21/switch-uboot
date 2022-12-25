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
	"fi\0"

#include "tegra-common-post.h"

#endif /* _NX_H */
