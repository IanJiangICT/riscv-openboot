#include "simple_types.h"
#include "bootconf.h"
#include "serial_func.h"
#include "storage_func.h"
#include "plat_ops.h"
#include "plat_def.h"

extern struct bootconf *bootconf0;

void zsbl_set_bc_rom(void)
{
	unsigned char *ptr_src = (unsigned char *)(&bootconf0);
	unsigned char *ptr_dst = (unsigned char *)PLAT_RAM_BC;
	uint32_t data_size;
	int i;

	/* Copy bootconf0 in ROM to RAM */
	data_size = *((uint32_t *)ptr_src);
	if (data_size <= 0 || data_size > BOOTCONF_SIZE_MAX) {
		data_size = BOOTCONF_SIZE_MAX;
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
	serial_print_byte('Z');
#ifndef PRINT_SIMPLE
	serial_print_byte('S');
	serial_print_byte('B');
	serial_print_byte('L');
	serial_print_byte('\10');
	serial_print_byte('\13');
#endif
	return;
}

void zsbl_print_bc(void)
{
	serial_print_byte('B');
#ifndef PRINT_SIMPLE
	serial_print_byte('C');
	serial_print_byte('\10');
	serial_print_byte('\13');
#endif
	return;
}

#ifdef ZSBL_BIST
void zsbl_run_bist(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	if (((bc->work_mode & 0x03) != BC_WORK_MODE_BIST) || ((bc->work_mode & BC_WORK_MODE_BIST_ZSBL) != 0)) return;
	serial_bist();
	storage_bist();
	return;
}
#endif
