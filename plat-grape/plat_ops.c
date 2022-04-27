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
	if (val == PLAT_BOOTMODE_DEBUG) {
		bc->work_mode = BC_WORK_MODE_DEBUG;
		bc->boot_start = BC_STORAGE_ROM_ONCHIP;
		bc->storage_bc = BC_STORAGE_ROM_ONCHIP;
		bc->storage_online |= BC_STORAGE_ROM_ONCHIP;
		bc->storage_online |= BC_STORAGE_RAM_ONCHIP;
		bc->storage_online |= BC_STORAGE_DDR;
		bc->storage_online |= BC_STORAGE_FLASH_I;
		bc->storage_fsbl = BC_STORAGE_RAM_ONCHIP;
		bc->storage_opensbi = BC_STORAGE_DDR;
	} else if (val == PLAT_BOOTMODE_BIST) {
		bc->work_mode = BC_WORK_MODE_BIST | BC_WORK_MODE_BIST_ZSBL
						| BC_WORK_MODE_BIST_FSBL
						| BC_WORK_MODE_BIST_PLAT;
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

	/* Initiate SSI0 */
	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_init(ssi_base, bc->flash_freq_div0);

	return;
}

void plat_flash_identify(unsigned char *m, unsigned char *t, unsigned char *c)
{
	volatile unsigned char *ssi_base;
	ssi_base = (unsigned char *)PLAT_SSI0_BASE;
	dw_ssi_jedec_id(ssi_base, m, t, c);
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
		/* SoC */
		0x01000000 + 0x120,  500000,
		0x01000000 + 0x124,  100000,
		/* CCN */
		0x41000000 + 0x020, 1500000,
		0x41000000 + 0x024,       0,
		/* PC Cluster 0 - 3 */
		0x40000000 + 0x020, 2000000,
		0x40000000 + 0x024,       0,
		0x40100000 + 0x020, 2000000,
		0x40100000 + 0x024,       0,
		0x40200000 + 0x020, 2000000,
		0x40200000 + 0x024,       0,
		0x40300000 + 0x020, 2000000,
		0x40300000 + 0x024,       0,
		/* PC Cluster 4 - 7 */
		0x48000000 + 0x020, 2000000,
		0x48000000 + 0x024,       0,
		0x48100000 + 0x020, 2000000,
		0x48100000 + 0x024,       0,
		0x48200000 + 0x020, 2000000,
		0x48200000 + 0x024,       0,
		0x48300000 + 0x020, 2000000,
		0x48300000 + 0x024,       0,
	};
	static uint32_t start_addr_val [] = {
		/* SoC */
		0x01000000 + 0x118, 0x00000002,
		/* CCN */
		0x41000000 + 0x018, 0x00000002,
		/* PC Cluster 0 - 3 */
		0x40000000 + 0x018, 0x00000002,
		0x40100000 + 0x018, 0x00000002,
		0x40200000 + 0x018, 0x00000002,
		0x40300000 + 0x018, 0x00000002,
		/* PC Cluster 4 - 7 */
		0x48000000 + 0x018, 0x00000002,
		0x48100000 + 0x018, 0x00000002,
		0x48200000 + 0x018, 0x00000002,
		0x48300000 + 0x018, 0x00000002,
	};
	static uint32_t wait_addr_val [] = {
		/* SoC */
		0x01000000 + 0x118, 0x00010000,
		/* CCN */
		0x41000000 + 0x018, 0x00010000,
		/* PC Cluster 0 - 3 */
		0x40000000 + 0x018, 0x00010000,
		0x40100000 + 0x018, 0x00010000,
		0x40200000 + 0x018, 0x00010000,
		0x40300000 + 0x018, 0x00010000,
		/* PC Cluster 4 - 7 */
		0x48000000 + 0x018, 0x00010000,
		0x48100000 + 0x018, 0x00010000,
		0x48200000 + 0x018, 0x00010000,
		0x48300000 + 0x018, 0x00010000,
	};
	static uint32_t switch_addr_val [] = {
		/* SoC */
		0x01000000 + 0x200, 0,
		/* CCN */
		0x41000000 + 0x040, 0,
		/* PC Cluster 0 - 3 */
		0x40000000 + 0x040, 2,
		0x40100000 + 0x040, 2,
		0x40200000 + 0x040, 2,
		0x40300000 + 0x040, 2,
		/* PC Cluster 4 - 7 */
		0x48000000 + 0x040, 2,
		0x48100000 + 0x040, 2,
		0x48200000 + 0x040, 2,
		0x48300000 + 0x040, 2,
	};
	static uint32_t reset_addr_val [] = {
		0x43100000 + 0x090, 0x0B, /* DDR AXI */
		0x4A100000 + 0x090, 0x0B, /* DDR AXI */
	};
	
#ifdef CLOCK_IN_ZSBL
	/* Do NOT print as UART is not ready */
#else
	serial_print_str("clock init\n");
#endif

	uart_base = (unsigned char *)PLAT_UART0_BASE;
	ssi_base = (unsigned char *)PLAT_SSI0_BASE;

	/* Configure frequencies to each PLL */
	for (i = 0; i < sizeof(conf_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)conf_addr_val[i]));
		val = conf_addr_val[i + 1];
		writel(val, addr);
	}

	/* Start each PLL */
	for (i = 0; i < sizeof(start_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)start_addr_val[i]));
		val = start_addr_val[i + 1];
		writel(val, addr);
	}

	/* Read each PLL to wait */
	for (i = 0; i < sizeof(wait_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)wait_addr_val[i]));
		do {
			val = readl(addr);
		} while (val & wait_addr_val[i + 1]);
	}

	/* Release resets excluding PC cores */
	for (i = 0; i < sizeof(reset_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)reset_addr_val[i]));
		val = reset_addr_val[i + 1];
		writel(val, addr);
	}

	/* Switch to PLL clocks */
	for (i = 0; i < sizeof(switch_addr_val)/sizeof(uint32_t); i += 2) {
		addr = (unsigned char *)((((uint64_t)addr_h) << 32) | ((uint64_t)switch_addr_val[i]));
		val = switch_addr_val[i + 1];
		writel(val, addr);
	}

	/* Change clock parameter for devices */
	dw_uart_init(uart_base, bc->uart_freq_div);
	dw_ssi_init(ssi_base, bc->flash_freq_div);

#ifdef CLOCK_IN_ZSBL
	serial_print_byte('C');
#else
	serial_print_str("clock ok\n");
#endif
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

