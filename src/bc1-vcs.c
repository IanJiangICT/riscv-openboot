/*
 * The build-in one for VCS simulation 
 */

#include "bootconf.h"

#define THIS_SOCKET_CNT 2
#define THIS_WORK_MODE			BC_WORK_MODE_VVCS
#define THIS_STORAGE_ONLINE ( \
	BC_STORAGE_ROM_ONCHIP \
	| BC_STORAGE_RAM_ONCHIP \
	)
#define THIS_STORAGE_BC 		BC_STORAGE_FLASH_I
#define THIS_STORAGE_FSBL 		BC_STORAGE_RAM_ONCHIP
#define THIS_STORAGE_OPENSBI	BC_STORAGE_DDR
#define THIS_BOOT_START			BC_STORAGE_ROM_ONCHIP
#define THIS_ENABLE_BITMAP 		BC_ENABLE_NONE
#define THIS_FLASH_CAP			( BC_SPI_DEFAULT \
	| BC_SPI_4BYTE_ADDR \
	| BC_SPI_FAST_READ \
	)
#define THIS_FLASH_STEP			4

struct bootconf bootconf1 = {
	.conf_size		= sizeof(struct bootconf) - sizeof(uint32_t) * (BOOTCONF_SOCKET_MAX - THIS_SOCKET_CNT),
	.boot_start		= THIS_BOOT_START,
	.work_mode		= BC_WORK_MODE_VVCS,
	.storage_online	= THIS_STORAGE_ONLINE,
	.storage_bc		= THIS_STORAGE_BC,
	.storage_fsbl	= THIS_STORAGE_FSBL,
	.storage_opensbi= THIS_STORAGE_OPENSBI,
	.enable_bitmap  = THIS_ENABLE_BITMAP,
	.socket_cnt		= THIS_SOCKET_CNT,
	.uart_freq_div0	= 1,	// Max baud under 25MHz
	.flash_freq_div0= 2,	// 12.5MHz under 25MHz
	.uart_freq_div	= 81,	// 76800 under 100MHz
	.flash_freq_div	= 4,	// 25MHz under 100MHz
	.flash_capability = THIS_FLASH_CAP,
	.flash_step_size  = THIS_FLASH_STEP,
	.pg_codes = {
		0x00000000,
		0x00000000,
	}
};
