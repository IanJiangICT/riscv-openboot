#include "simple_types.h"
#include "riscv_mmio.h"
#include "serial_func.h"

//#define CONFIG_DW_I2C_DEBUG

#ifdef CONFIG_DW_I2C_DEBUG
#define i2c_dbg(...)			serial_printf(__VA_ARGS__)
#else
#define i2c_dbg(...)			do { } while (0)
#endif
#define con_err(...)			serial_printf(__VA_ARGS__)

#define __raw_readl readl
#define __raw_writel writel

/* Register offset */
#define IC_CON							0x00
#define IC_TAR							0x04
#define IC_SAR							0x08
#define IC_HS_MADDR						0x0c
#define IC_DATA_CMD						0x10
#define IC_SS_SCL_HCNT					0x14
#define IC_UFM_SCL_HCNT					0x14
#define IC_SS_SCL_LCNT					0x18
#define IC_UFM_SCL_LCNT					0x18
#define IC_FS_SCL_HCNT					0x1c
#define IC_UFM_TBUF_CNT					0x1c
#define IC_FS_SCL_LCNT					0x20
#define IC_HS_SCL_HCNT					0x24
#define IC_HS_SCL_LCNT					0x28
#define IC_INTR_STAT					0x2c
#define IC_INTR_MASK					0x30
#define IC_RAW_INTR_STAT				0x34
#define IC_RX_TL						0x38
#define IC_TX_TL						0x3c
#define IC_CLR_INTR						0x40
#define IC_CLR_RX_UNDER					0x44
#define IC_CLR_RX_OVER					0x48
#define IC_CLR_TX_OVER					0x4c
#define IC_CLR_RD_REQ					0x50
#define IC_CLR_TX_ABRT					0x54
#define IC_CLR_RX_DONE					0x58
#define IC_CLR_ACTIVITY					0x5c
#define IC_CLR_STOP_DET					0x60
#define IC_CLR_START_DET				0x64
#define IC_CLR_GEN_CALL					0x68
#define IC_ENABLE						0x6c
#define IC_STATUS						0x70
#define IC_TXFLR						0x74
#define IC_RXFLR						0x78
#define IC_SDA_HOLD						0x7c
#define IC_TX_ABRT_SOURCE				0x80
#define IC_SLV_DATA_NACK_ONLY			0x84
#define IC_DMA_CR						0x88
#define IC_DMA_RDLR						0x90
#define IC_SDA_SETUP					0x94
#define IC_ACK_GENERAL_CALL				0x98
#define IC_ENABLE_STATUS				0x9c
#define IC_FS_SPKLEN					0xa0
#define IC_UFM_SPKLEN					0xa0
#define IC_HS_SPKLEN					0xa4
#define IC_CLR_RESTART_DET				0xa8
#define IC_SCL_STUCK_AT_LOW_TIMEOUT		0xac
#define IC_SDA_STUCK_AT_LOW_TIMEOUT		0xb0
#define IC_CLR_SCL_STUCK_DET			0xb4
#define IC_DEVICE_ID					0xb8
#define IC_SMBUS_CLK_LOW_SEXT			0xbc
#define IC_SMBUS_CLK_LOW_MEXT			0xc0
#define IC_SMBUS_THIGH_MAX_IDLE_COUNT	0xc4
#define IC_SMBUS_INTR_STAT				0xc8
#define IC_SMBUS_INTR_MASK				0xcc
#define IC_SMBUS_RAW_INTR_STAT			0xd0
#define IC_CLR_SMBUS_INTR				0xd4
#define IC_OPTIONAL_SAR					0xd8
#define IC_SMBUS_UDID_LSB				0xdc
#define IC_SMBUS_UDID_WORD0				0xdc
#define IC_SMBUS_UDID_WORD1				0xe0
#define IC_SMBUS_UDID_WORD2				0xe4
#define IC_SMBUS_UDID_WORD3				0xe8
#define REG_TIMEOUT_RST					0xf0
#define IC_COMP_PARAM_1					0xf4
#define IC_COMP_VERSION					0xf8
#define IC_COMP_TYPE					0xfc

/* High and low times in different speed modes (in ns) */
#define MIN_SS_SCL_HIGHTIME	4000
#define MIN_SS_SCL_LOWTIME	4700
#define MIN_FS_SCL_HIGHTIME	600
#define MIN_FS_SCL_LOWTIME	1300
#define MIN_HS_SCL_HIGHTIME	60
#define MIN_HS_SCL_LOWTIME	160

/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO		(CONFIG_SYS_HZ/500)
#define I2C_STOPDET_TO		(CONFIG_SYS_HZ/500)
#define I2C_BYTE_TO_BB		(I2C_BYTE_TO * 16)

/* I2C control register definitions */
#define IC_CON_SD		0x0040
#define IC_CON_RE		0x0020
#define IC_CON_10BITADDRMASTER	0x0010
#define IC_CON_10BITADDR_SLAVE	0x0008
#define IC_CON_SPD_MSK		0x0006
#define IC_CON_SPD_SS		0x0002
#define IC_CON_SPD_FS		0x0004
#define IC_CON_SPD_HS		0x0006
#define IC_CON_MM		0x0001

/* I2C target address register definitions */
#define TAR_ADDR		0x0050
#define TAR_DEVID		0x1400

/* I2C slave address register definitions */
#define IC_SLAVE_ADDR		0x0002

/* I2C data buffer and command register definitions */
#define IC_CMD			0x0100
#define IC_STOP			0x0200

/* I2C interrupt status register definitions */
#define IC_GEN_CALL		0x0800
#define IC_START_DET		0x0400
#define IC_STOP_DET		0x0200
#define IC_ACTIVITY		0x0100
#define IC_RX_DONE		0x0080
#define IC_TX_ABRT		0x0040
#define IC_RD_REQ		0x0020
#define IC_TX_EMPTY		0x0010
#define IC_TX_OVER		0x0008
#define IC_RX_FULL		0x0004
#define IC_RX_OVER 		0x0002
#define IC_RX_UNDER		0x0001
#define IC_INTR_ALL		0x7FFF

/* FIFO threshold register definitions */
#define IC_TL0			0x00
#define IC_TL1			0x01
#define IC_TL2			0x02
#define IC_TL3			0x03
#define IC_TL4			0x04
#define IC_TL5			0x05
#define IC_TL6			0x06
#define IC_TL7			0x07

/* I2C enable register definitions */
#define IC_ENABLE_0B		0x0001
#define IC_ENABLE_ABRT		0x0002

/* I2C status register  definitions */
#define IC_STATUS_SA		0x0040
#define IC_STATUS_MA		0x0020
#define IC_STATUS_RFF		0x0010
#define IC_STATUS_RFNE		0x0008
#define IC_STATUS_TFE		0x0004
#define IC_STATUS_TFNF		0x0002
#define IC_STATUS_ACT		0x0001

/* Speed Selection */
#define IC_SPEED_MODE_STANDARD	1
#define IC_SPEED_MODE_FAST	2 /* For Fast and Fast Plus */
#define IC_SPEED_MODE_HIGH  3
#define IC_SPEED_MODE_MAX	IC_SPEED_MODE_HIGH

/* Starting speed for each Mode */
#define I2C_FAST_SPEED		400000
#define I2C_FAST_PLUS_SPEED	1000000
#define I2C_HIGH_SPEED		3400000
#define I2C_MAX_SPEED		I2C_HIGH_SPEED

#define CONFIG_DW_I2C_USE_COUNT
#define CONFIG_DW_I2C_TX_TL 0 
#define CONFIG_DW_I2C_RX_TL 0

/*
 * Initialize controller as master device
 */
void dw_i2c_master_init(volatile unsigned char *base)
{
	uint32_t val;
	uint32_t offset;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	offset = IC_ENABLE;
	val = 0;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _ENABLE\n", offset, val);
	__raw_writel(val, base + offset);

#ifdef CONFIG_DW_I2C_DEBUG
	offset = IC_CON; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _CON\n", offset, val);
	offset = IC_RX_TL; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _RX_TL\n", offset, val);
	offset = IC_TX_TL; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _TX_TL\n", offset, val);
	offset = IC_INTR_MASK; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _INTR_MASK\n", offset, val);
	offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
	offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
	offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
	offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
	offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
	offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif

	offset = IC_CON;
	val = IC_CON_SD /* IC_SLAVE_DISABLE */
		| IC_CON_RE /* IC_RESTART_EN */
		/* IC_10BITADDR_MASTER = 0 */
		/* IC_10BITADDR_SLAVE = 0 */
		| IC_CON_SPD_SS /* SEEP = Stand */
		| IC_CON_MM /* IC_MASTER_MODE */
		;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _CON\n", offset, val);
	__raw_writel(val, base + offset);

	offset = IC_RX_TL;
	val = CONFIG_DW_I2C_RX_TL;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _RX_TL\n", offset, val);
	__raw_writel(val, base + offset);

	offset = IC_TX_TL;
	val = CONFIG_DW_I2C_TX_TL;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _TX_TL\n", offset, val);
	__raw_writel(val, base + offset);

	offset = IC_INTR_MASK;
#ifdef CONFIG_DW_I2C_TEST_IRQ
	val = IC_RX_FULL;
#else
	val = IC_INTR_ALL;
#endif
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _INTR_MASK\n", offset, val);
	__raw_writel(val, base + offset);

	offset = IC_ENABLE;
	val = 1;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _ENABLE\n", offset, val);
	__raw_writel(val, base + offset);

	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return;
}

/*
 * Set address when this controller acts as a slave device
 */
void dw_i2c_set_address(volatile unsigned char *base, unsigned char addr)
{
	i2c_dbg("dw_i2c: Debug: Enter %s. addr = 0x%x\n", __func__, addr);
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel((uint32_t)addr, base + IC_SAR);
	__raw_writel(1, base + IC_ENABLE);
	return;
}

/*
 * Set frequency by setting speed mode and relative SCL count
 */
void dw_i2c_set_frequency(volatile unsigned char *base, unsigned int apb_speed, unsigned int i2c_khz)
{
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	uint32_t ena;
	unsigned int i2c_spd;
	unsigned int speed = i2c_khz * 1000;
	unsigned int bus_speed = apb_speed;
#ifndef CONFIG_DW_I2C_USE_COUNT
	unsigned int bus_mhz = bus_speed / 1000 / 1000;
#endif

	uint32_t val;
	uint32_t offset;

	i2c_dbg("dw_i2c: Debug: Enter %s. bus speed = %ld Hz, i2c seep = %d kHz\n",
		__func__, bus_speed, i2c_khz);
	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	offset = IC_ENABLE;
	val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _ENABLE\n", offset, val);
	ena = val;

	val = 0;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _ENABLE\n", offset, val);
	__raw_writel(val, base + offset);

	offset = IC_CON;
	val = __raw_readl(base + offset);
	i2c_dbg("dw_i2c: r 0x%2x 0x%x _CON\n", offset, val);
	cntl = val;

#ifdef CONFIG_DW_I2C_USE_COUNT
	switch (i2c_spd) {
	case IC_SPEED_MODE_HIGH:
		cntl |= IC_CON_SPD_HS;
		hcnt = bus_speed / speed / (6+16) * 6 - (1+7);
		lcnt = bus_speed / speed / (6+16) * 16 - 1;
		lcnt = hcnt * 2;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_HS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_HS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
		offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = bus_speed / speed / (40+47) * 40 - (5+7);
		lcnt = bus_speed / speed / (40+47) * 47 - 1;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_SS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_SS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
		offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = bus_speed / speed / (6+13) * 6 - (5+7);
		lcnt = bus_speed / speed / (6+13) * 13 - 1;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_FS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_FS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
		offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		break;
	}
#else
	switch (i2c_spd) {
	case IC_SPEED_MODE_HIGH:
		cntl |= IC_CON_SPD_HS;
		hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / 1000;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_HS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_HS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
		offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / 1000;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_SS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_SS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
		offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / 1000;
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = hcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_FS_SCL_HCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_LCNT; val = lcnt;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_FS_SCL_LCNT);
#ifdef CONFIG_DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
		offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		break;
	}
#endif

	offset = IC_CON;
	val = cntl;
	i2c_dbg("dw_i2c: w 0x%2x 0x%x _CON\n", offset, val);
	__raw_writel(val, base + offset);

	/* Restore back i2c now speed set */
	if (ena == IC_ENABLE_0B) {
		offset = IC_ENABLE;
		val = 1;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _ENABLE\n", offset, val);
		__raw_writel(val, base + offset);
	}

	return;
}

static void i2c_setaddress(volatile unsigned char *base, unsigned int i2c_addr)
{
	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel(i2c_addr, base + IC_TAR);
	__raw_writel(1, base + IC_ENABLE);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
}

static void i2c_flush_rxfifo(volatile unsigned char *base)
{
	uint32_t val;
	uint32_t offset;
	offset  = IC_STATUS;
	do {
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
	} while (val & IC_STATUS_RFNE);
}

static int i2c_wait_for_bb(volatile unsigned char *base)
{
	uint32_t val;
	uint32_t offset;

	offset  = IC_STATUS;
	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	do {
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
	} while(val & IC_STATUS_MA || !(val & IC_STATUS_TFE));
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return 0;
}

static int i2c_xfer_init(volatile unsigned char *base, unsigned char chip, unsigned int addr,
			 int alen)
{
	uint32_t val;
	uint32_t offset;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	if (i2c_wait_for_bb(base))
		return 1;

	i2c_setaddress(base, chip);
	offset = IC_DATA_CMD;
	while (alen) {
		alen--;
		/* high byte address going out first */
		val = (addr >> (alen * 8)) & 0xff;
		i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n", offset, val);
		__raw_writel(val, base + offset);
	}
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return 0;
}

static int i2c_rx_finish(volatile unsigned char *base)
{
	uint32_t val;
	uint32_t offset;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	while (1) {
		offset = IC_RAW_INTR_STAT;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _RAW_INTR_STAT\n", offset, val);
		if (val & IC_STOP_DET) {
			offset = IC_CLR_STOP_DET;
			val = __raw_readl(base + offset);
			i2c_dbg("dw_i2c: r 0x%2x 0x%x _CLR_STOP_DET\n",
				offset, val);
			break;
		}
	}

	if (i2c_wait_for_bb(base)) {
		con_err("dw_i2c: Timed out waiting for bus\n");
		return 1;
	}

	i2c_flush_rxfifo(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);

	return 0;
}

static int i2c_tx_finish(volatile unsigned char *base)
{
	uint32_t val;
	uint32_t offset;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	while (1) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_TFE) {
			break;
		}

		offset = IC_RAW_INTR_STAT;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _RAW_INTR_STAT\n", offset, val);
		if (val & IC_STOP_DET) {
			offset = IC_CLR_STOP_DET;
			val = __raw_readl(base + offset);
			i2c_dbg("dw_i2c: r 0x%2x 0x%x _CLR_STOP_DET\n",
				offset, val);
			break;
		}
	}

	if (i2c_wait_for_bb(base)) {
		con_err("dw_i2c: Timed out waiting for bus\n");
		return 1;
	}

	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return 0;
}

/*
 * Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int dw_i2c_read_mem(volatile unsigned char *base, unsigned char dev, unsigned int addr,
		    unsigned int alen, unsigned char *buffer, unsigned int len)
{
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	int ret;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	ret = i2c_xfer_init(base, dev, addr, alen);
	if (ret != 0) {
		con_err("dw_i2c: Error: Failed i2c_xfer_init. ret = %d\n",
			ret);
		return 1;
	}

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1)
				val |= IC_STOP;
#endif
			i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			__raw_writel(val, base + offset);
			active = 1;
		}

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
			i2c_dbg("dw_i2c: r 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
	}

	ret = i2c_rx_finish(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return ret;
}

/*
 * Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
int dw_i2c_write_mem(volatile unsigned char *base, unsigned char dev, unsigned int addr,
		     unsigned int alen, unsigned char *buffer, unsigned int len)
{
	uint32_t val;
	uint32_t offset;
	int ret;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	ret = i2c_xfer_init(base, dev, addr, alen);
	if (ret != 0) {
		con_err("dw_i2c: Error: Failed i2c_xfer_init. ret = %d\n",
			ret);
		return 1;
	}

	while (len) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_TFNF) {
			offset = IC_DATA_CMD;
			val = *buffer;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1) {
				val = *buffer | IC_STOP;
			}
#endif
			i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			__raw_writel(val, base + offset);
			buffer++;
			len--;
		}
	}

	ret = i2c_tx_finish(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return ret;
}

/*
 * Simple read and write for some bytes.
 */
int dw_i2c_read_bytes(volatile unsigned char *base, unsigned char dev, unsigned char *buffer,
		      unsigned int len, unsigned int stop)
{
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	unsigned int orig_len = len;
	int ret;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	if (i2c_wait_for_bb(base))
		return -1;
	i2c_setaddress(base, dev);

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0)
				val |= IC_STOP;
#endif
			i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			__raw_writel(val, base + offset);
			active = 1;
		}

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
			i2c_dbg("dw_i2c: r 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
		/* TODO Timeout. */
	}

	ret = i2c_rx_finish(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	if (ret != 0) {
		return -1;
	}

	return (orig_len - len);
}

int dw_i2c_write_bytes(volatile unsigned char *base, unsigned char dev, unsigned char *buffer,
		       unsigned int len, unsigned int stop)
{
	uint32_t val;
	uint32_t offset;
	unsigned int orig_len = len;
	int ret;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	if (i2c_wait_for_bb(base))
		return -1;
	i2c_setaddress(base, dev);

	while (len) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_TFNF) {
			offset = IC_DATA_CMD;
			val = *buffer;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0) {
				val = *buffer | IC_STOP;
			}
#endif
			i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			__raw_writel(val, base + offset);
			buffer++;
			len--;
		}
		/* TODO Timeout. */
	}

	ret = i2c_tx_finish(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	if (ret != 0) {
		return -1;
	}

	return (orig_len - len);
}

/*
 * DW I2C VIP operations:
 * - write: 1 byte I2C CMD + 1 byte address + n bytes data
 * - read:  1 byte I2C CMD + n bytes data (from slave)
 */
int dw_i2c_write_vip(volatile unsigned char *base, unsigned char dev, unsigned int addr,
		     unsigned char *buffer, unsigned int len)
{
	return dw_i2c_write_mem(base, dev, addr, 1, buffer, len);
}

int dw_i2c_read_vip(volatile unsigned char *base, unsigned char dev, unsigned char *buffer, unsigned len)
{
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	int ret;

	i2c_dbg("dw_i2c: Debug: Enter %s\n", __func__);
	if (i2c_wait_for_bb(base))
		return 1;

	i2c_setaddress(base, dev);

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1)
				val |= IC_STOP;
#endif
			i2c_dbg("dw_i2c: w 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			__raw_writel(val, base + offset);
			active = 1;
		}

#ifdef CONFIG_DW_I2C_TEST_IRQ
		while (irq_test_flag == 0) {
			irq_local_save(irq_flags);
			irq_local_enable();
			irq_local_disable();
			irq_local_restore(irq_flags);
		}
		irq_test_flag = 0;
		irqc_ack_irq(IRQ_I2C0 + irq_test_master_num);
		irqc_unmask_irq(IRQ_I2C0 + irq_test_master_num);
#endif

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
		i2c_dbg("dw_i2c: r 0x%2x 0x%x _STATUS\n", offset, val);
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
			i2c_dbg("dw_i2c: r 0x%2x 0x%x _DATA_CMD\n",
				offset, val);
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
	}

	ret = i2c_rx_finish(base);
	i2c_dbg("dw_i2c: Debug: Exit %s\n", __func__);
	return ret;
}
