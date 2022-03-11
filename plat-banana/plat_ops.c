#include "bootconf.h"
#include "simple_types.h"
#include "riscv_mmio.h"
#include "serial_func.h"
#include "plat_def.h"

void plat_bc_fix(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	uint32_t val;

	addr = (unsigned char *)PLAT_SYSCSR_BOOT_MODE;
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
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;

	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_init(ssi_base, bc->flash_freq_div0);

	return;
}

void plat_flash_read_byte(unsigned int offset, unsigned char *buf)
{
	volatile unsigned char *ssi_base;

	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_read_byte(ssi_base, offset, buf);

	return;
}

void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;
	uint32_t i;

	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	if (bc->flash_step_size <= 1) {
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
	volatile unsigned char *uart_base;

	/* Initiate UART0 */
	uart_base = (unsigned char *)PLAT_UART0_BASE;
	dw_uart_init(uart_base, bc->uart_freq_div0);

	return;
}

void plat_serial_put_byte(unsigned char data)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *uart_base;

	if (bc->enable_console == 0) return;

	uart_base = (unsigned char *)PLAT_UART0_BASE;
	dw_uart_put_byte(uart_base, data);

	return;
}

#ifdef FSBL_FUNC

void plat_power_init(void) { return; }

#define PLAT_CLK_BASE 0x4302000
#define CLK_CONFIG_BASE (PLAT_CLK_BASE + 0x100)
#define CLK_ENABLE_BASE (PLAT_CLK_BASE + 0x400)

void plat_clock_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	volatile unsigned char *uart_base;
	volatile unsigned char *ssi_base;
	uint32_t val;
	uint32_t addr_h;
	int i;
	static uint32_t conf_addr_val [] = {
		(CLK_CONFIG_BASE + 0x40 * 0 + 0x20), 0xF4240,
		(CLK_CONFIG_BASE + 0x40 * 1 + 0x20), 0x124F80,
		(CLK_CONFIG_BASE + 0x40 * 2 + 0x20), 0x124F80,
		(CLK_CONFIG_BASE + 0x40 * 3 + 0x20), 0xC3500,
		(CLK_CONFIG_BASE + 0x40 * 4 + 0x20), 0xC3500,
		(CLK_CONFIG_BASE + 0x40 * 5 + 0x20), 0xF4240,
		(CLK_CONFIG_BASE + 0x40 * 6 + 0x20), 0x10C8E0,
		(CLK_CONFIG_BASE + 0x40 * 7 + 0x20), 0x2625A,
		(CLK_CONFIG_BASE + 0x40 * 8 + 0x20), 0x2625A,

		(CLK_CONFIG_BASE + 0x40 * 0 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 1 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 2 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 3 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 4 + 0x24), 0x186A0,
		(CLK_CONFIG_BASE + 0x40 * 5 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 6 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 7 + 0x24), 0x0,
		(CLK_CONFIG_BASE + 0x40 * 8 + 0x24), 0x0,

		(CLK_CONFIG_BASE + 0x40 * 0 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 1 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 2 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 3 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 4 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 5 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 6 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 7 + 0x1C), 25000,
		(CLK_CONFIG_BASE + 0x40 * 8 + 0x1C), 25000,

		(CLK_CONFIG_BASE + 0x40 * 0 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 1 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 2 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 3 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 4 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 5 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 6 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 7 + 0x18), 0x2,
		(CLK_CONFIG_BASE + 0x40 * 8 + 0x18), 0x2,
	};
	static uint32_t check_addr [] = {
		(CLK_CONFIG_BASE + 0x40 * 0 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 1 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 2 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 3 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 4 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 5 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 6 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 7 + 0x18),
		(CLK_CONFIG_BASE + 0x40 * 8 + 0x18),
	};
	static uint32_t enable_addr_val [] = {
		(CLK_ENABLE_BASE + 0x4 * 0), 0x0,
		(CLK_ENABLE_BASE + 0x4 * 1), 0x4,
		(CLK_ENABLE_BASE + 0x4 * 2), 0x6,
		(CLK_ENABLE_BASE + 0x4 * 3), 0x0,
		(CLK_ENABLE_BASE + 0x4 * 4), 0xC,
		(CLK_ENABLE_BASE + 0x4 * 5), 0xC,
	};

	serial_print_str("clock init\n");

	addr_h = 0;
	uart_base = (unsigned char *)PLAT_UART0_BASE;
	ssi_base = (unsigned char *)PLAT_SSI0_BASE;

	/* No need to configure PLL over Zebu */
	if (bc->work_mode == BC_WORK_MODE_VZEBU) {
		goto skip_pll_config;
	}

	/* Write to each PLL to configure */
	for (i = 0; i < sizeof(conf_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)conf_addr_val[i]));
		val = conf_addr_val[i + 1];
		writel(val, addr);
	}

	/* Read each PLL to wait */
	for (i = 0; i < sizeof(check_addr)/sizeof(uint32_t); i++) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)check_addr[i]));
		do {
			val = readl(addr);
		} while (val & 0x10000);
	}

skip_pll_config:
	/* Switch to PLL clocks */
	for (i = 0; i < sizeof(enable_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)enable_addr_val[i]));
		val = enable_addr_val[i + 1];
		writel(val, addr);
	}

	/* Change clock parameter for devices */
	dw_uart_init(uart_base, bc->uart_freq_div);
	dw_ssi_init(ssi_base, bc->flash_freq_div);

	serial_print_str("clock ok\n");
	return;
}

void plat_start_pc(void) { return; }
void plat_setup_pg(void) { return; }
void plat_setup_sz(void) { return; }
void plat_ddrctrl_init(void) { return; }
void plat_chiplink_init(void) { return; }

extern void pc_start(void);
void plat_mc_fsbl_final(void)
{
	/* MC acts as PC finally on this platform */
	pc_start();
}
#endif

