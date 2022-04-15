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
	if (val == PLAT_BOOTMODE_SIM_SSI || val == PLAT_BOOTMODE_SIM_SD) {
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_ROM_ONCHIP;
		bc->storage_online |= BC_STORAGE_ROM_ONCHIP;
		bc->storage_online |= BC_STORAGE_RAM_ONCHIP;
		if (val == PLAT_BOOTMODE_SIM_SSI) {
			bc->storage_online |= BC_STORAGE_FLASH_I;
		} else {
			bc->storage_online |= BC_STORAGE_SD_I;
		}
		bc->storage_fsbl = BC_STORAGE_RAM_ONCHIP;
		bc->storage_opensbi = BC_STORAGE_DDR;
	}

	/* TODO Get socket count from system register */
	bc->socket_cnt = 1;

	return;
}

#include "dw_ssi.h"
void plat_flash_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;
	uint32_t val;
	unsigned char m, t, c;

	/* Initiate SSI0 */
	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_init(ssi_base, bc->flash_freq_div0);
#ifdef STORAGE_READ_ID
	dw_ssi_jedec_id(ssi_base, &m, &t, &c);
#endif

	return;
}

void plat_flash_read_byte(unsigned int offset, unsigned char *buf)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;
	unsigned char flag = bc->flash_capability & BC_SPI_4BYTE_ADDR;

	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_read_byte(ssi_base, offset, buf, flag);

	return;
}

void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;
	unsigned char cap = bc->flash_capability;
	uint32_t i;

	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	if ((bc->flash_step_size > 1) && (cap & BC_SPI_4BYTE_ADDR) && (cap & BC_SPI_FAST_READ)) {
		uint32_t rx_size = 0;
		while ((size - rx_size) >= bc->flash_step_size) {
			dw_ssi_fast_read4ba(ssi_base, offset + rx_size, buf + rx_size, bc->flash_step_size);
			rx_size += bc->flash_step_size;
		}
		for (i = 0; i < (size - rx_size); i++) {
			dw_ssi_read_byte(ssi_base, offset + rx_size + i, buf + rx_size + i, cap & BC_SPI_4BYTE_ADDR);
		}
	} else {
		for (i = 0; i < size; i++) {
			dw_ssi_read_byte(ssi_base, offset + i, buf + i, cap & BC_SPI_4BYTE_ADDR);
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
	uint32_t val;

	/* Initiate UART0 */
	uart_base = (unsigned char *)PLAT_UART0_BASE;
	dw_uart_init(uart_base, bc->uart_freq_div0);

	return;
}

void plat_serial_put_byte(unsigned char data)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *uart_base;

	if ((bc->enable_bitmap & BC_ENABLE_CONSOLE) == 0) return;

	uart_base = (unsigned char *)PLAT_UART0_BASE;
	dw_uart_put_byte(uart_base, data);

	return;
}

#if (!defined(CLOCK_IN_ZSBL) && !defined(FSBL_FUNC)) || (defined(CLOCK_IN_ZSBL) && defined (FSBL_FUNC))
void plat_clock_init(void) { return; } /* Null opration */
#else
void plat_clock_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	volatile unsigned char *uart_base;
	volatile unsigned char *ssi_base;
	uint32_t val;
	uint32_t addr_h = 0;
	int i;
	static uint32_t conf_addr_val [] = {
	};
	static uint32_t enable_addr_val [] = {
	};
	static uint32_t reset_addr_val [] = {
	};
	
	serial_print_str("clock init\n");

	/* Get higher address based on Socket ID */
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
	for (i = 0; i < sizeof(conf_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)conf_addr_val[i]));
		do {
			val = readl(addr);
		} while (val & 0x10000);
	}

skip_pll_config:
	/* Release resets excluding PC cores */
	for (i = 0; i < sizeof(reset_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)reset_addr_val[i]));
		val = reset_addr_val[i + 1];
		writel(val, addr);
	}

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
#endif

#ifdef FSBL_FUNC

void plat_power_init(void) { return; }

#if (!defined(PC_ACTS_AS_MC)) && (!defined(MC_ACTS_AS_PC))
void plat_start_pc(void) { return; }
#endif
void plat_setup_pg(void) { return; }

void plat_setup_sz(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint32_t val;
	uint32_t *sharezone = (uint32_t *)PLAT_DDR_SHAREZONE;

	val = bc->pg_codes[bc->socket_id];
	sharezone[0] = 0;
	sharezone[1] = bc->socket_cnt;
	sharezone[2 + bc->socket_id] = val;
	serial_print_str("sz ");
	serial_print_hex_u32(sharezone[0]);
	serial_print_str(" ");
	serial_print_hex_u32(sharezone[1]);
	serial_print_str(" ");
	serial_print_hex_u32(sharezone[2]);
	serial_print_str(" ");
	serial_print_hex_u32(sharezone[3]);
	serial_print_str("\n");

	__asm__ __volatile__("fence; fence.i;");

	return;
}

void plat_ddrctrl_init(void) { return; }
void plat_chiplink_init(void) { return; }

void plat_mc_fsbl_final(void)
{
	/* MC does nothing finally on this platform */
	while(1) { asm("wfi"); }
}
#endif

