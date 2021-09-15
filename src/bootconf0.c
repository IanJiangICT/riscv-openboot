/*
 * The build-in one for a product
 */

#include "bootconf.h"

#define SOCKET_CNT 1

unsigned char _start[0]; /* Defined to avoid link warning */

struct bootconf bootconf0 = {
	.conf_size		= sizeof(struct bootconf) + sizeof(uint32_t) * SOCKET_CNT,
	.boot_start		= BC_BOOT_START_ROM,
	.conf_at		= BC_CONF_AT_ROM,
	.work_mode		= BC_WORK_MODE_PRODUCT,
	.load_fsbl		= 1,
	.load_opensbi	= 1,
	.enable_console	= 1,
	.enable_ddrctrl	= 1,
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
