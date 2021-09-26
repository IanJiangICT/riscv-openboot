#include "simple_types.h"
#include "bootconf.h"
#include "serial_func.h"
#include "plat_ops.h"
#include "plat_def.h"

extern void *_start;

void zsbl_set_bc_rom(void)
{
	unsigned char *ptr_src = (unsigned char *)(_start + PLAT_ROM_OFFSET_BC0);
	unsigned char *ptr_dst = (unsigned char *)PLAT_RAM_BC;
	uint32_t data_size;
	int i;

	/* Copy bootconf0 in ROM to RAM */
	data_size = *((uint32_t *)ptr_src);
	if (data_size <= 0 || data_size > BOOTCONF_SIZE_MAX) {
		return;
	}
	for (i = 0; i < data_size; i += sizeof(uint32_t)) {
		*((uint32_t *)(ptr_dst + i)) = *((uint32_t *)(ptr_src + i));
	}

	/* Fixup based on platform */
	plat_bc_fix();	
	return;
}

void zsbl_init_console(void)
{
	serial_init();
	serial_print_str("ZSBL\n");
	return;
}

void zsbl_print_bc(void)
{
	uint32_t *bc = (uint32_t *)PLAT_RAM_BC;
	int	u32_cnt = (*bc)/sizeof(uint32_t);
	int i;

	serial_print_str("bc\n");
	for (i = 0; i < u32_cnt; i++) {
		serial_print_hex_u32(bc[i]);
		serial_print_str("\n");
	}

	return;
}
