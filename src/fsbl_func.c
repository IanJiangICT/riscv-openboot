#include "simple_types.h"
#include "bootconf.h"
#include "serial_func.h"
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
