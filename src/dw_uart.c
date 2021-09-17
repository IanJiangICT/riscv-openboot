#include "simple_types.h"
#include "riscv_mmio.h"

void dw_uart_init(volatile void *uart_base, unsigned int baud)
{
	uint32_t val;
	val = 0x00000000; writel(val, uart_base + 0x10);	// MCR
	val = 0x000000A3; writel(val, uart_base + 0x0C);	// LCR
	val = baud;       writel(val, uart_base + 0x00);	// DLL
	val = 0x00000000; writel(val, uart_base + 0x04);	// DLH
	val = 0x00000023; writel(val, uart_base + 0x0C);	// LCR
	val = 0x00000000; writel(val, uart_base + 0x08);	// FCR
	val = 0x00000002; writel(val, uart_base + 0x04);	// IER
	return;
}

void dw_uart_put_byte(volatile void *uart_base, unsigned char data)
{
	uint32_t val;
	do { val = readl(uart_base + 0x7C); val &= 0x1; } while ((val & 0x1) != 0);
	val = (uint32_t)data; writel(val, uart_base + 0x00);// THR
	val = readl(uart_base + 0x08);	// Read IIR to clear interrupt
	do { val = readl(uart_base + 0x7C); val &= 0x1; } while ((val & 0x1) != 0);
	return;
}
