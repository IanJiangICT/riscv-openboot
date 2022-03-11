/*
 * The build-in one for a product
 */

#include "bootconf.h"

#define SOCKET_CNT 1

#define THIS_STORAGE_ONLINE ( \
	BC_STORAGE_ROM_ONCHIP \
	| BC_STORAGE_ROM_OFFCHIP \
	| BC_STORAGE_RAM_ONCHIP \
	| BC_STORAGE_DDR \
	| BC_STORAGE_FLASH_I \
	)
#define THIS_STORAGE_BC 		BC_STORAGE_FLASH_I
#define THIS_STORAGE_FSBL 		BC_STORAGE_FLASH_I
#define THIS_STORAGE_OPENSBI	BC_STORAGE_FLASH_I
#define THIS_BOOT_START			BC_STORAGE_ROM_ONCHIP

struct bootconf bootconf0 = {
	.conf_size		= sizeof(struct bootconf) - sizeof(uint32_t) * (BOOTCONF_SOCKET_MAX - SOCKET_CNT),
	.boot_start		= THIS_BOOT_START,
	.work_mode		= BC_WORK_MODE_PRODUCT,
	.storage_online	= THIS_STORAGE_ONLINE,
	.storage_bc		= THIS_STORAGE_BC,
	.storage_fsbl	= THIS_STORAGE_FSBL,
	.storage_opensbi= THIS_STORAGE_OPENSBI,
	.enable_console	= 1,
	.enable_ddrctrl	= 1,
	.socket_cnt		= SOCKET_CNT,
#ifdef ZSBL_ONCHIP
	/* For on-chip ZSBL, bus frequency originates from external clock (25MHz for example)
	 * in the 1st initiating of these devices. */
	.uart_freq_div0	= 20,	// 76800 under 25MHz
	.flash_freq_div0= 2,	// 12.5MHz under 25MHz
#else
	/* For off-chip ZSBL, bus frequency is switched to PLL clock (100MHz for example)
	 * before the 1st initiating of these devices. */
	.uart_freq_div0	= 81,	// 76800 under 25MHz
	.flash_freq_div0= 4,	// 25MHz under 100MHz
#endif
	.uart_freq_div	= 81,	// 76800 under 100MHz
	.flash_freq_div	= 4,	// 25MHz under 100MHz
	.flash_step_size= 1,
	.pg_codes = {
		0x00000000,
	}
};
