#ifndef BOOTCONF_H
#define BOOTCONF_H

#define BOOTCONF_SIZE_MAX	0x1000

#ifndef __ASM__
#include "simple_types.h"

enum bc_boot_start {
	BC_BOOT_START_ROM = 0,
	BC_BOOT_START_RAM,
	BC_BOOT_START_DDR,
	BC_BOOT_START_FLASH,
};

enum bc_conf_at {
	BC_CONF_AT_ROM = 0,
	BC_CONF_AT_FLASH_0, 	/* At the flash of socket 0 */
	BC_CONF_AT_FLASH_I, 	/* At each flash of every socket */
};

enum bc_work_mode {
	BC_WORK_MODE_PRODUCT = 0,
	BC_WORK_MODE_DIAGNOSIS,
	BC_WORK_MODE_VVCS,		/* Verification with VCS */
	BC_WORK_MODE_VZEBU,		/* Verification with Zebu */
};

struct bootconf {
	uint32_t conf_size;			/* Size in byte, including struct bootconf + following data */
	uint32_t boot_start;
	uint32_t conf_at;
	uint32_t work_mode;
	uint32_t load_fsbl;			/* 1: Load FSBL from external storage such as flash. Otherwise,
								 * 0: Use the build-in one, in RAM for example in the verification case */
	uint32_t load_opensbi;		/* Similarly, wheather to Load OpenSBI from external storage */
	uint32_t enable_console;	/* Enable console output such as an UART */
	uint32_t enable_ddrctrl;	/* Enable DDR controller */
	uint32_t enable_chiplink;	/* Enable chiplink between sockets */
	uint32_t socket_id;
	uint32_t socket_cnt;
	uint32_t uart_freq_div0;	/* Frequncy dividor default */
	uint32_t uart_freq_div; 	/* Frequncy dividor after clock works */
	uint32_t flash_freq_div0;
	uint32_t flash_freq_div;
	uint32_t flash_step_size;	/* Data size in byte of each read (or write) when continuously
                                 * transfer is supported, which depends on the capability of the 
                                 * controller and the device. In fact, this is always supported
                                 * by actual flash chips, but might not be supported by some 
                                 * verification models */
	uint32_t pg_codes[0];		/* [Optional] Partial good codes for each socket. */
} __attribute__ ((packed));

#endif

#endif
