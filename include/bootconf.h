#ifndef BOOTCONF_H
#define BOOTCONF_H

/* XXX Make sure that sizeof(struct bootconf) <= BOOTCONF_SOCKET_MAX */
#define BOOTCONF_SIZE_MAX	0x40
#define BOOTCONF_SOCKET_MAX 2

#ifndef __ASM__
#include "simple_types.h"

enum bc_storage {
	BC_STORAGE_NONE       = 0x00,
	BC_STORAGE_ROM_ONCHIP = 0x01,
	BC_STORAGE_ROM_OFFCHIP= 0x02,
	BC_STORAGE_RAM_ONCHIP = 0x04,
	BC_STORAGE_DDR        = 0x08,
	BC_STORAGE_FLASH_0    = 0x10, 	/* At the flash of socket 0 */
	BC_STORAGE_FLASH_I    = 0x20, 	/* At each flash of every socket */
	BC_STORAGE_SD_0       = 0x40,
	BC_STORAGE_SD_I       = 0x80,
};

enum bc_work_mode {
	BC_WORK_MODE_PRODUCT = 0,
	BC_WORK_MODE_DIAGNOSIS,
	BC_WORK_MODE_VVCS,		/* Verification with VCS */
	BC_WORK_MODE_VZEBU,		/* Verification with Zebu */
};

enum bc_spi {
	BC_SPI_DEFAULT		= 0x00,
	BC_SPI_4BYTE_ADDR	= 0x01,
	BC_SPI_FAST_READ	= 0x02,
};

enum bc_enable {
	BC_ENABLE_NONE		= 0x00,
	BC_ENABLE_CONSOLE	= 0x01, /* Enable console output such as an UART */
	BC_ENABLE_DDRCTRL	= 0x02, /* Enable DDR controller */
	BC_ENABLE_CHIPLINK	= 0x04,	/* Enable chiplink between sockets */
	BC_ENABLE_ALL		= 0xFF,
};

struct bootconf {
	uint8_t conf_size;			/* Size in byte, including struct bootconf + following data */
	uint8_t boot_start;
	uint8_t work_mode;
	uint8_t storage_online;		/* Bitmap of available storage types */
	uint8_t storage_bc;
	uint8_t storage_fsbl;
	uint8_t storage_opensbi;
	uint8_t enable_bitmap;
	uint8_t socket_id;
	uint8_t socket_cnt;
	uint8_t uart_freq_div0;		/* Frequncy dividor default */
	uint8_t uart_freq_div; 		/* Frequncy dividor after clock works */
	uint8_t flash_freq_div0;
	uint8_t flash_freq_div;
/*
 * About reading a flash:
 * 1) Do the 1st reading (ZSBL in on-chip ROM reading bootconf1 with bootconf0)
 *    under the lowest capability:
 *    - This is indicated in bootconf0:
 *        flash_step_size == 1, or
 *        flash_capability == BC_SPI_DEFAULT
 *    - Read 1 byte data each time with 3-byte-address only in the lower 16MB
 *      memory space.
 * 2) Do the 2nd reading (ZSBL reading FSBL and FSBL reading OpenSBI with
 *    bootconf1) under the highest capability as far as possible:
 *    - This is indicated in bootconf1:
 *        flash_step_size > 1, and
 *        flash_capability with BC_SPI_4BYTE_ADDR and BC_SPI_FAST_READ
 *    - Fast read multiple bytes each time with 4-byte-address in the whole
 *      memory space.
 * 3) Keep under the lowest capability if either fast-read or 4-byte-address is
 *    supported in the actural flash memory, which should be indicated in
 *    bootconf1.
 */
	uint8_t flash_capability;	/* Combination of enum bc_spi */
	uint8_t flash_step_size;	/* Data size in byte of each read (or write) when continuously
                                 * transfer is supported, which depends on the capability of the 
                                 * controller and the device. In fact, this is always supported
                                 * by actual flash chips, but might not be supported by some 
                                 * verification models */
	uint32_t pg_codes[BOOTCONF_SOCKET_MAX];		/* [Optional] Partial good codes for each socket. */
} __attribute__ ((packed));

#endif

#endif
