#include "simple_types.h"
#include "bootconf.h"
#include "serial_func.h"
#include "storage_func.h"
#include "plat_ops.h"
#include "plat_def.h"

void fsbl_first_print(void)
{
	serial_print_byte('F');
#ifndef PRINT_SIMPLE
	serial_print_byte('S');
	serial_print_byte('B');
	serial_print_byte('L');
	serial_print_byte('\10');
	serial_print_byte('\13');
#endif
	return;
}

#ifdef FSBL_BIST
void fsbl_run_bist(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	if (((bc->work_mode & 0x03) != BC_WORK_MODE_BIST) || ((bc->work_mode & BC_WORK_MODE_BIST_FSBL) != 0)) return;
	serial_bist();
	storage_bist();
	return;
}
#endif
