#include "plat_def.h"

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
	. = (PLAT_ROM_BASE + PLAT_ROM_OFFSET_PC);
	.text.init : { *(.text.init) }
	. = ALIGN(0x1000);
	.text : { *(.text) }
	. = ALIGN(0x1000);
	.data : { *(.data) }
	.data.string : { *(.data.string) }
	.bss : { *(.bss) }
	_end = .;
}
