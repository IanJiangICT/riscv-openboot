#ifndef BOOTCONF_H
#define BOOTCONF_H

#define BOOTCONF_SIZE_MAX	0x1000
#define BOOTCONF_SOCKET_MAX 8

#ifndef __ASM__
#include "simple_types.h"

enum bc_storage {
	BC_STORAGE_NONE       = 0x0000,
	BC_STORAGE_ROM_ONCHIP = 0x0001,
	BC_STORAGE_ROM_OFFCHIP= 0x0002,
	BC_STORAGE_RAM_ONCHIP = 0x0004,
	BC_STORAGE_RAM_OFFCHIP= 0x0008,
	BC_STORAGE_DDR        = 0x0010,
	BC_STORAGE_FLASH_0    = 0x0020, 	/* At the flash of socket 0 */
	BC_STORAGE_FLASH_I    = 0x0040, 	/* At each flash of every socket */
	BC_STORAGE_SD_0       = 0x0080,
	BC_STORAGE_SD_I       = 0x0100,
};

enum bc_work_mode {
	BC_WORK_MODE_PRODUCT = 0,
	BC_WORK_MODE_DIAGNOSIS,
	BC_WORK_MODE_VVCS,		/* Verification with VCS */
	BC_WORK_MODE_VZEBU,		/* Verification with Zebu */
};

enum bc_spi {
	BC_SPI_4BYTE_ADDR	= 0x0001,
	BC_SPI_FAST_READ	= 0x0002,
};

struct bootconf {
	uint32_t conf_size;			/* Size in byte, including struct bootconf + following data */
	uint32_t boot_start;
	uint32_t work_mode;
	uint32_t storage_online;	/* Bitmap of available storage types */
	uint32_t storage_bc;
	uint32_t storage_fsbl;
	uint32_t storage_opensbi;
	uint32_t enable_console;	/* Enable console output such as an UART */
	uint32_t enable_ddrctrl;	/* Enable DDR controller */
	uint32_t enable_chiplink;	/* Enable chiplink between sockets */
	uint32_t socket_id;
	uint32_t socket_cnt;
	uint32_t uart_freq_div0;	/* Frequncy dividor default */
	uint32_t uart_freq_div; 	/* Frequncy dividor after clock works */
	uint32_t flash_freq_div0;
	uint32_t flash_freq_div;
	uint32_t flash_capability;	/* Other fucntions supported beside reading 1 byte data with 3-byte address */
	uint32_t flash_step_size;	/* Data size in byte of each read (or write) when continuously
                                 * transfer is supported, which depends on the capability of the 
                                 * controller and the device. In fact, this is always supported
                                 * by actual flash chips, but might not be supported by some 
                                 * verification models */
	uint32_t pg_codes[BOOTCONF_SOCKET_MAX];		/* [Optional] Partial good codes for each socket. */
} __attribute__ ((packed));

#endif

#endif
