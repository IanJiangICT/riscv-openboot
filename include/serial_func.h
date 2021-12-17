#ifndef SERIAL_FUNC_H
#define SERIAL_FUNC_H

#include "simple_types.h"

void serial_init(void);
void serial_print_str(char *str);
void serial_print_bin_u32(uint32_t v);
void serial_print_hex_u32(uint32_t v);

void serial_printf(const char *format, ...);

#endif
