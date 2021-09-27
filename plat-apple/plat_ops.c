#include "bootconf.h"
#include "simple_types.h"
#include "riscv_mmio.h"
#include "plat_def.h"

void plat_bc_fix(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile void *addr;
	uint32_t val;

	addr = (void *)PLAT_SYSCSR_BOOT_MODE;
	val = readl(addr);
	val &= 0x0000000F;
	if (val == 0x0) {
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_SD_0;
	} else if (val == 0x1) {
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_FLASH_I;
	} else if (val == 0x2) {
		bc->boot_start = BC_STORAGE_ROM_OFFCHIP;
		bc->storage_bc = BC_STORAGE_ROM_OFFCHIP;
	} else {
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_ROM_ONCHIP;
	}

	addr = (void *)PLAT_SYSCSR_SOCKET_ID;
	val = readl(addr);
	bc->socket_id = (val >> 1) & 0x00000001;
	if (val & 0x00000001) {
		bc->socket_cnt = 2;
	} else {
		bc->socket_cnt = 1;
	}

	return;
}

void plat_flash_init(void) { return; }
void plat_flash_read_byte(unsigned int offset, unsigned char *buf) { return; }
void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size) { return; }
void plat_sd_init(void) { return; }
void plat_sd_read_byte(unsigned int offset, unsigned char *buf) { return; }
void plat_sd_read(unsigned int offset, unsigned char *buf, unsigned int size) { return; }
void plat_serial_init(void) { return; }
void plat_serial_put_byte(unsigned char data) { return; }
void plat_clock_init(void) { return; }
void plat_start_pc(void) { return; }
void plat_setup_pg(void) { return; }
void plat_setup_sz(void) { return; }
void plat_ddrctrl_init(void) { return; }
void plat_chiplink_init(void) { return; }

