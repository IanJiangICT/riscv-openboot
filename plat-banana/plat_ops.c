#include "bootconf.h"
#include "simple_types.h"
#include "riscv_mmio.h"
#include "serial_func.h"
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
		bc->storage_bc = BC_STORAGE_FLASH_0;
	} else { /* Debug mode for Simulation */
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_ROM_ONCHIP;
		bc->storage_fsbl = 0; // FSBL in RAM already
		bc->enable_console = 0;
	}

	/* Always 1 socket */
	bc->socket_cnt = 1;

	return;
}

#include "dw_ssi.h"
void plat_flash_init(void)
{
	volatile void *ssi_base;

	ssi_base = (void *)PLAT_SSI0_BASE;
	dw_ssi_init(ssi_base);

	return;
}

void plat_flash_read_byte(unsigned int offset, unsigned char *buf)
{
	volatile void *ssi_base;

	ssi_base = (void *)PLAT_SSI0_BASE;
	dw_ssi_read_byte(ssi_base, offset, buf);

	return;
}

void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile void *ssi_base;
	uint32_t i;

	ssi_base = (void *)PLAT_SSI0_BASE;
	if (bc->flash_step_size == 0) {
		for (i = 0; i < size; i++) {
			dw_ssi_read_byte(ssi_base, offset + i, buf + i);
		}
	} else { /* TODO: Optimize with continuously read */
		uint32_t rx_size = 0;
		while ((size - rx_size) >= bc->flash_step_size) {
			//dw_ssi_read(ssi_base, offset + rx_size, buf + rx_size, rx_size);
			rx_size += bc->flash_step_size;
		}
		for (i = 0; i < (size - rx_size); i++) {
			dw_ssi_read_byte(ssi_base, offset + rx_size + i, buf + rx_size + i);
		}
	}

	return;
}

void plat_sd_init(void) { return; }
void plat_sd_read_byte(unsigned int offset, unsigned char *buf) { return; }
void plat_sd_read(unsigned int offset, unsigned char *buf, unsigned int size) { return; }

#include "dw_uart.h"
void plat_serial_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile void *uart_base;

	/* Initiate UART0 */
	uart_base = (void *)PLAT_UART0_BASE;
	dw_uart_init(uart_base, bc->uart_freq_div0);

	return;
}

void plat_serial_put_byte(unsigned char data)
{
	volatile void *uart_base;

	uart_base = (void *)PLAT_UART0_BASE;
	dw_uart_put_byte(uart_base, data);

	return;
}

#ifdef FSBL_FUNC
void plat_clock_init(void) { return; }
void plat_start_pc(void) { return; }
void plat_setup_pg(void) { return; }
void plat_setup_sz(void) { return; }
void plat_ddrctrl_init(void) { return; }
void plat_chiplink_init(void) { return; }
#endif

