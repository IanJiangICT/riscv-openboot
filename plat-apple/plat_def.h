#ifndef PLAT_APPLE_H
#define PLAT_APPLE_H

#define SOC_ROM_BASE			0xFF8F000000
#define SOC_ROM_SEGMENT_SIZE	0x20000
#define SOC_ROM_SEGMENT_CNT		8
#define SOC_ROM_SIZE			(SOC_ROM_SEGMENT_SIZE * SOC_ROM_SEGMENT_CNT)

#define SOC_ROM_BASE_OFFCHIP	0xFF88000000

#define SOC_RAM_BASE			0xFF80000000
#define SOC_RAM_SIZE			0x0000100000

#define SOC_CORE_TYPE_CSR 	marchid
#define SOC_CORE_TYPE_PC 	0x00000000
#define SOC_CORE_TYPE_MC 	0x00000003
#define SOC_CORE_HARTID_MASK_SOCKET 0xF
#define SOC_CORE_CNT_MC		1
#define SOC_CORE_CNT_PC		16

#define PLAT_ROM_BASE			SOC_ROM_BASE
#define PLAT_ROM_BASE_OFFCHIP	SOC_ROM_BASE_OFFCHIP
#define PLAT_RAM_BASE			SOC_RAM_BASE
#define PLAT_DDR_BASE			0x0

#define PLAT_ROM_SIZE			SOC_ROM_SIZE
#define PLAT_ROM_SIZE_OFFCHIP	0x2000000
#define PLAT_RAM_SIZE			SOC_RAM_SIZE

#define PLAT_ROM_OFFSET_MC	(SOC_ROM_SEGMENT_SIZE * 0)
#define PLAT_ROM_OFFSET_ST	(SOC_ROM_SEGMENT_SIZE * 2)
#define PLAT_ROM_OFFSET_PC	(SOC_ROM_SEGMENT_SIZE * 3)

#define PLAT_BOOTMODE_BIST	4
#define PLAT_BOOTMODE_DEBUG	5
#define PLAT_BOOTMODE_SIM_MC_OFFCHIP 2
#define PLAT_BOOTMODE_SIM_PC_OFFCHIP 6

#define PLAT_RAM_STACK_SIZE_MC	0x2000
#define PLAT_RAM_STACK_SIZE_PC	0x1000
#define PLAT_RAM_STACK_BASE_PC	(PLAT_RAM_BASE + PLAT_RAM_SIZE - PLAT_RAM_STACK_SIZE_PC * SOC_CORE_CNT_PC)
#define PLAT_RAM_STACK_BASE_MC	(PLAT_RAM_STACK_BASE_PC - PLAT_RAM_STACK_SIZE_MC * SOC_CORE_CNT_MC)
#define PLAT_RAM_STACK_BASE		PLAT_RAM_STACK_BASE_MC
#define PLAT_RAM_BC				(PLAT_RAM_STACK_BASE - BOOTCONF_SIZE_MAX)
#define PLAT_RAM_SHARE_SIZE		0x100
#define PLAT_RAM_SHARE_BASE		(PLAT_RAM_BC - PLAT_RAM_SHARE_SIZE)

#define PLAT_RAM_FSBL		PLAT_RAM_BASE
#define PLAT_DDR_SHAREZONE	(PLAT_DDR_BASE + 0x100000)
#define PLAT_DDR_OPENSBI	(PLAT_DDR_BASE + 0x200000)

#define PLAT_SYSCSR_BASE			0xFF91000000
#define	PLAT_SYSCSR_BOOT_MODE		(PLAT_SYSCSR_BASE + 0x00)
#define	PLAT_SYSCSR_SOCKET_ID		(PLAT_SYSCSR_BASE + 0x04)
#define	PLAT_SYSCSR_PARTIAL_GOOD	(PLAT_SYSCSR_BASE + 0x50)

#define PLAT_SOCKET_OFFSET	0x80000000000
#define	PLAT_CRCNTL_BASE	 0xFF60000000
#define	PLAT_TLMM_BASE		 0xFF60200000
#define	PLAT_SSI0_BASE		 0xFF62000000
#define	PLAT_SSI1_BASE		 0xFF62100000
#define	PLAT_PLIC0_BASE		 0xFF70000000
#define	PLAT_PLIC1_BASE		0x8FF70000000
#define	PLAT_UART0_BASE		 0xFF63000000
#define	PLAT_UART1_BASE		 0xFF63100000
#define	PLAT_UART2_BASE		 0xFF63200000
#define	PLAT_UART3_BASE		 0xFF63300000

#define PLAT_SSI_RXFIFO_DEPTH	8
#define PLAT_SSI_TXFIFO_DEPTH	8

#define	PLAT_UART0_HWIRQ	84
#define	PLAT_UART1_HWIRQ	(PLAT_UART0_HWIRQ + 1)
#define	PLAT_UART2_HWIRQ	(PLAT_UART0_HWIRQ + 2)
#define	PLAT_UART3_HWIRQ	(PLAT_UART0_HWIRQ + 3)

#define PLAT_CLINT0_BASE	 0xFF010F0000
#define CLINT_REG(offset)		(PLAT_CLINT0_BASE + (offset))
#define CLINT_MSIP(hart)		CLINT_REG((hart) << 2)
#define CLINT_MTIMECMP(hart)	CLINT_REG(0x4000 + ((hart) << 3))
#define CLINT_MTIME				CLINT_REG(0xBFF8)

#ifndef PLAT_CLINT_FREQ
#define PLAT_CLINT_FREQ 	25000000
#endif

#ifndef PLAT_DDR_DFI_FREQ
#define PLAT_DDR_DFI_FREQ	600000000
#endif

#define	ZEBU_REG_PC_LOG		0xFF601FFFF0

#endif
