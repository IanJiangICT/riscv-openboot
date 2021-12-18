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

	addr = (unsigned char *)PLAT_SYSCSR_SOCKET_ID;
	val = readl(addr);
	bc->socket_id = (val >> 1) & 0x00000001;
	if (val & 0x00000001) {
		bc->socket_cnt = 2;
	} else {
		bc->socket_cnt = 1;
	}

	return;
}

#include "dw_ssi.h"
void plat_flash_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *tlmm_base;
	volatile unsigned char *ssi_base;
	uint32_t val;

	/* Configure TLMM for SSI0 */
	if (bc->socket_id == 0) {
		tlmm_base = (unsigned char *)PLAT_TLMM_BASE;
	} else {
		tlmm_base = (unsigned char *)PLAT_TLMM_BASE + PLAT_SOCKET_OFFSET;
	}
	val = 0x00000009;
	writel(val, tlmm_base + 192);
	writel(val, tlmm_base + 196);
	writel(val, tlmm_base + 200);
	writel(val, tlmm_base + 204);
	writel(val, tlmm_base + 208);

	/* Initiate SSI0 */
	if (bc->socket_id == 0) {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	} else {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE + PLAT_SOCKET_OFFSET;
	}
	dw_ssi_init(ssi_base);

	return;
}

void plat_flash_read_byte(unsigned int offset, unsigned char *buf)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;

	if (bc->socket_id == 0) {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	} else {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE + PLAT_SOCKET_OFFSET;
	}
	dw_ssi_read_byte(ssi_base, offset, buf);

	return;
}

void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *ssi_base;
	uint32_t i;

	if (bc->socket_id == 0) {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	} else {
		ssi_base = (unsigned char *)PLAT_SSI0_BASE + PLAT_SOCKET_OFFSET;
	}
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
	volatile unsigned char *tlmm_base;
	volatile unsigned char *uart_base;
	uint32_t val;

	/* Configure TLMM for UART0/1/2/3 */
	if (bc->socket_id == 0) {
		tlmm_base = (unsigned char *)PLAT_TLMM_BASE;
	} else {
		tlmm_base = (unsigned char *)PLAT_TLMM_BASE + PLAT_SOCKET_OFFSET;
	}
	val = 0x00000009;
	writel(val, tlmm_base + 216);
	writel(val, tlmm_base + 220);
	writel(val, tlmm_base + 248);
	writel(val, tlmm_base + 252);
	writel(val, tlmm_base + 280);
	writel(val, tlmm_base + 284);
	writel(val, tlmm_base + 312);
	writel(val, tlmm_base + 316);

	/* Initiate UART0 */
	if (bc->socket_id == 0) {
		uart_base = (unsigned char *)PLAT_UART0_BASE;
	} else {
		uart_base = (unsigned char *)PLAT_UART0_BASE + PLAT_SOCKET_OFFSET;
	}
	dw_uart_init(uart_base, bc->uart_freq_div0);

	return;
}

void plat_serial_put_byte(unsigned char data)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *uart_base;

	if (bc->socket_id == 0) {
		uart_base = (unsigned char *)PLAT_UART0_BASE;
	} else {
		uart_base = (unsigned char *)PLAT_UART0_BASE + PLAT_SOCKET_OFFSET;
	}
	dw_uart_put_byte(uart_base, data);

	return;
}

#ifdef FSBL_FUNC
void plat_clock_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	volatile unsigned char *uart_base;
	uint32_t val;
	uint32_t addr_h;
	int i;
	uint32_t conf_addr_val [] = {
		(0x60000000 + 0x18),	0x1943,
		(0x02100000 + 0x18),	0x1923,
		(0x01000000 + 0x18),	0x1913,
		(0x01100000 + 0x18),	0x1913,
		(0x01200000 + 0x18),	0x1913,
		(0x01300000 + 0x18),	0x1913,
		(0x60000040 + 0x18),	0x1953,
		(0x60000080 + 0x18),	0x1953,
		(0x600000C0 + 0x18),	0x19B3,
	};
	uint32_t enable_addr_val [] = {
		(0x02100000 + 0x40),	0x2,
		(0x01000000 + 0x40),	0x6,
		(0x01100000 + 0x40),	0x6,
		(0x01200000 + 0x40),	0x6,
		(0x01300000 + 0x40),	0x6,
		(0x60000000 + 0x150),	0x0,
	};
	uint32_t reset_addr_val [] = {
		(0x60000000 + 0x130),	0x0,
		(0x60000000 + 0x134),	0x0,
	};
	
	serial_print_str("clock init\n");

	/* Get higher address based on Socket ID */
	if (bc->socket_id == 0) {
		addr_h = (uint32_t)0xFF;
		uart_base = (unsigned char *)PLAT_UART0_BASE;
	} else {
		addr_h = (uint32_t)0x8FF;
		uart_base = (unsigned char *)PLAT_UART0_BASE + PLAT_SOCKET_OFFSET;
	}

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
#if 0
	for (i = 0; i < sizeof(reset_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)reset_addr_val[i]));
		val = readl(addr);
		val += 1;
	}
#endif
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

	serial_print_str("clock ok\n");
	return;
}

void plat_start_pc(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	uint32_t val;
	uint32_t addr_h;
	int i;
	uint32_t start_addr_val [] = {
		(0x01000000 + 0x50),	0,
		(0x01100000 + 0x50),	0,
		(0x01200000 + 0x50),	0,
		(0x01300000 + 0x50),	0,
	};

	/* Enable trace log over Zebu */
	if (bc->work_mode == BC_WORK_MODE_VZEBU && bc->socket_id == 0) {
		serial_print_str("pc trace log\n");
		addr = (unsigned char *)ZEBU_REG_PC_LOG;
		val = 1;
		writel(val, addr);
	}

	serial_print_str("start pc\n");

	/* Get higher address based on Socket ID */
	if (bc->socket_id == 0) {
		addr_h = (uint32_t)0xFF;
	} else {
		addr_h = (uint32_t)0x8FF;
	}

	/* Release resets to start PC */
	for (i = 0; i < sizeof(start_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)start_addr_val[i]));
		val = start_addr_val[i + 1];
		writel(val, addr);
	}

	return;
}

void plat_setup_pg(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	volatile unsigned char *addr;
	uint32_t val;

	val = bc->pg_codes[bc->socket_id];
	serial_print_str("pg_code ");
	serial_print_hex_u32(val);
	serial_print_str("\n");
	addr = (unsigned char *)PLAT_SYSCSR_PARTIAL_GOOD;
	writel(val, addr);

	return;
}

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

