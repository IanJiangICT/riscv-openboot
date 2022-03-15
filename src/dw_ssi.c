#include "simple_types.h"
#include "riscv_mmio.h"
#include "storage_def.h"

void dw_ssi_init(volatile unsigned char *ssi_base, unsigned int freq_div)
{
	uint32_t val;
	val = 0x00000000; writel(val, ssi_base + 0x08);	// Disable SSI
	val = 0x000703C0; writel(val, ssi_base + 0x00);	// CTRLR0: DFS_32 = 0x07 (8-bit), FRF = 0x0 (MOTOROLA_SPI)
	val = 0x00000000; writel(val, ssi_base + 0x04);	// CTRLR1: NDF = 0 (only 1 byte)
	val = (uint32_t)freq_div;						// BAUDR: SCKDV is any even value between 2 and 65534
	val = (val + 1) & 0x0000FFFE;
					  writel(val, ssi_base + 0x14);
	val = 0x00000000; writel(val, ssi_base + 0x2C);	// IMR: Mask all interrupts (do not allow any)
	val = 0x00000001; writel(val, ssi_base + 0x08);	// Enable SSI
	return;
}

void dw_ssi_jedec_id(volatile unsigned char *ssi_base, unsigned char *manufacturer, unsigned char *mem_type, unsigned char *capacity)
{
	uint32_t val;
	val = 0x00000000; writel(val, ssi_base + 0x08);	// Disable SSI
	val = 0x000703C0; writel(val, ssi_base + 0x00);	// CTRLR0: DFS_32 = 0x07 (8-bit), FRF = 0x0 (MOTOROLA_SPI)
	val = 0x00000002; writel(val, ssi_base + 0x04);	// CTRLR1: NDF = 2
	val = 0x00000002; writel(val, ssi_base + 0x1C);	// RXFTLR: RFT = 2
	val = 0x00000000; writel(val, ssi_base + 0x10);	// SER: Disable slave
	val = 0x00000001; writel(val, ssi_base + 0x08);	// Enable SSI

	/* Send 1-byte CMD */
	val = SPI_INST_JEDEC_ID;                 writel(val, ssi_base + 0x60);
	val = 0x00000001; writel(val, ssi_base + 0x10);	// Enable slave to start TX

	/* Wait until RX FIFO is full: RISR.RXFIR = 1 */
	do { val = readl(ssi_base + 0x34); } while ((val & 0x10) == 0);

	/* Get data received */
	val = readl(ssi_base + 0x60); *manufacturer = val & 0xFF;
	val = readl(ssi_base + 0x60); *mem_type     = val & 0xFF;
	val = readl(ssi_base + 0x60); *capacity     = val & 0xFF;

	/* Disable slave */
	val = 0x00000000; writel(val, ssi_base + 0x10);	// Disable slave

	return;
}

void dw_ssi_read_byte(volatile unsigned char *ssi_base, unsigned int offset, unsigned char *buf, unsigned char addr_4bytes)
{
	uint32_t val;
	val = 0x00000000; writel(val, ssi_base + 0x08);	// Disable SSI
	val = 0x000703C0; writel(val, ssi_base + 0x00);	// CTRLR0: DFS_32 = 0x07 (8-bit), FRF = 0x0 (MOTOROLA_SPI)
	val = 0x00000000; writel(val, ssi_base + 0x04);	// CTRLR1: NDF = 0 (only 1 byte)
	val = 0x00000000; writel(val, ssi_base + 0x1C);	// RXFTLR: RFT = 0 (only 1 entry)
	val = 0x00000000; writel(val, ssi_base + 0x10);	// SER: Disable slave
	val = 0x00000001; writel(val, ssi_base + 0x08);	// Enable SSI

	/* Send 1-byte Read CMD + 3/4 byte Address */
	if (addr_4bytes) {
		val = SPI_INST_READ_4BA;                 writel(val, ssi_base + 0x60);
		val = (uint32_t)((offset >> 24) & 0xFF); writel(val, ssi_base + 0x60);
	} else {
		val = SPI_INST_READ;                     writel(val, ssi_base + 0x60);
	}
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
