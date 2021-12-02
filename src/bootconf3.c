/*
 * The build-in one for VCS simulation 
 */

#include "bootconf.h"

#define SOCKET_CNT 1

#define THIS_STORAGE_ONLINE ( \
	BC_STORAGE_ROM_ONCHIP \
	| BC_STORAGE_RAM_ONCHIP \
	)
#define THIS_STORAGE_BC 		BC_STORAGE_ROM_ONCHIP
#define THIS_STORAGE_FSBL 		BC_STORAGE_RAM_ONCHIP
#define THIS_STORAGE_OPENSBI	BC_STORAGE_DDR
#define THIS_BOOT_START			BC_STORAGE_ROM_ONCHIP

unsigned char _start[0]; /* Defined to avoid link warning */

struct bootconf bootconf0 = {
	.conf_size		= sizeof(struct bootconf) + sizeof(uint32_t) * SOCKET_CNT,
	.boot_start		= THIS_BOOT_START,
	.work_mode		= BC_WORK_MODE_VVCS,
	.storage_online	= THIS_STORAGE_ONLINE,
	.storage_bc		= THIS_STORAGE_BC,
	.storage_fsbl	= THIS_STORAGE_FSBL,
	.storage_opensbi= THIS_STORAGE_OPENSBI,
	.enable_console	= 1,
	.enable_ddrctrl	= 0,
	.socket_cnt		= 1,
	.uart_freq_div0	= 20,	// 76800 under 25MHz
	.uart_freq_div	= 81,	// 76800 under 100MHz
	.flash_freq_div0= 2,	// 12.5MHz under 25MHz
	.flash_freq_div	= 2,	// 50MHz under 100MHz
	.flash_step_size= 256,
};

uint32_t pg_code_list[] = {
	0x00000000,
};