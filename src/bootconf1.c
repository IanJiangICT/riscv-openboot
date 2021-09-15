/*
 * Example for single-socket over Zebu
 */

#include "bootconf.h"

#define SOCKET_CNT 1

unsigned char _start[0]; /* Defined to avoid link warning */

struct bootconf bootconf1 = {
	.conf_size		= sizeof(struct bootconf) + sizeof(uint32_t) * SOCKET_CNT,
	.boot_start		= BC_BOOT_START_ROM,
	.conf_at		= BC_CONF_AT_ROM,
	.work_mode		= BC_WORK_MODE_VZEBU,
	.load_fsbl		= 0,
	.load_opensbi	= 0,
	.enable_console	= 1,
	.enable_ddrctrl	= 0,
	.socket_cnt		= 1,
	.uart_freq_div0	= 20,	// 76800 under 25MHz
	.uart_freq_div	= 1,	// 6250000 under 100MHz
	.flash_freq_div0= 2,	// 12.5MHz under 25MHz
	.flash_freq_div	= 2,	// 50MHz under 100MHz
	.flash_step_size= 1,	// Continuously transfer not supported by current flash device model
};

uint32_t pg_code_list[] = {
	0x01010100,		/* 4 harts enabled */
};
