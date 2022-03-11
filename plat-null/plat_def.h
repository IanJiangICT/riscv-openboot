#ifndef PLAT_NULL_H
#define PLAT_NULL_H

#define SOC_ROM_BASE			0x20000
#define SOC_ROM_SEGMENT_SIZE	0x20000
#define SOC_ROM_SEGMENT_CNT		4
#define SOC_ROM_SIZE			(SOC_ROM_SEGMENT_SIZE * SOC_ROM_SEGMENT_CNT)

#define SOC_ROM_BASE_OFFCHIP	0x20000000

#define SOC_RAM_BASE			0x40000000
#define SOC_RAM_SIZE			0x100000

#define SOC_CORE_TYPE_CSR 	marchid
#define SOC_CORE_TYPE_PC 	0x00000000
#define SOC_CORE_TYPE_MC 	0x00000003
#define SOC_CORE_HARTID_MASK_SOCKET 0x3
#define SOC_CORE_CNT_MC		1
#define SOC_CORE_CNT_PC		1

#define PLAT_ROM_BASE			SOC_ROM_BASE
#define PLAT_ROM_BASE_OFFCHIP	SOC_ROM_BASE_OFFCHIP
#define PLAT_RAM_BASE			SOC_RAM_BASE
#define PLAT_DDR_BASE			0x80000000

#define PLAT_ROM_SIZE			SOC_ROM_SIZE
#define PLAT_ROM_SIZE_OFFCHIP	0x2000000
#define PLAT_RAM_SIZE			SOC_RAM_SIZE

#define PLAT_ROM_OFFSET_MC	(SOC_ROM_SEGMENT_SIZE * 0)
#define PLAT_ROM_OFFSET_ST	(SOC_ROM_SEGMENT_SIZE * 2)
#define PLAT_ROM_OFFSET_PC	(SOC_ROM_SEGMENT_SIZE * 3)

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

#endif
