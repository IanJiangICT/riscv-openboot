#include "simple_types.h"
#include "riscv_mmio.h"

void dw_ssi_init(volatile void *ssi_base)
{
	uint32_t val;
	val = 0x00000000; writel(val, ssi_base + 0x08);	// Disable SSI
	val = 0x000703C0; writel(val, ssi_base + 0x00);	// CTRLR0: DFS_32 = 0x07 (8-bit), FRF = 0x0 (MOTOROLA_SPI)
	val = 0x00000000; writel(val, ssi_base + 0x04);	// CTRLR1: NDF = 0 (only 1 byte)
	val = 0x00000002; writel(val, ssi_base + 0x14);	// BAUDR
	val = 0x00000000; writel(val, ssi_base + 0x2C);	// IMR: Mask all interrupts (do not allow any)
	val = 0x00000001; writel(val, ssi_base + 0x08);	// Enable SSI
	return;
}


void dw_ssi_read_byte(volatile void *ssi_base, unsigned int offset, unsigned char *buf)
{
	uint32_t val;
	val = 0x00000000; writel(val, ssi_base + 0x08);	// Disable SSI
	val = 0x00000000; writel(val, ssi_base + 0x04);	// CTRLR1: NDF = 0 (only 1 byte)
	val = 0x00000000; writel(val, ssi_base + 0x1C);	// RXFTLR: RFT = 0 (only 1 entry)
	val = 0x00000000; writel(val, ssi_base + 0x10);	// SER: Disable slave
	val = 0x00000001; writel(val, ssi_base + 0x08);	// Enable SSI

	/* Send 1-byte Read CMD + 3-byte Address */
	val = 0x03;                              writel(val, ssi_base + 0x60);
	val = (uint32_t)((offset >> 16) & 0xFF); writel(val, ssi_base + 0x60);
	val = (uint32_t)((offset >>  8) & 0xFF); writel(val, ssi_base + 0x60);
	val = (uint32_t)((offset      ) & 0xFF); writel(val, ssi_base + 0x60);
	val = 0x00000001; writel(val, ssi_base + 0x10);	// Enable slave to start TX

	/* Wait until RX FIFO is full: RISR.RXFIR = 1 */
	do { val = readl(ssi_base + 0x34); } while ((val & 0x10) == 0);

	/* Get data received */
	val = readl(ssi_base + 0x60);
	*buf = val & 0xFF;

	/* Disable slave */
	val = 0x00000000; writel(val, ssi_base + 0x10);	// Disable slave

	return;
}
