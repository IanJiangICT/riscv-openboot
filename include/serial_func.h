#ifndef SERIAL_FUNC_H
#define SERIAL_FUNC_H

#include "simple_types.h"

void serial_init(void);
void serial_printf(const char *format, ...);
void serial_print_byte(char c);
void serial_print_str(char *str);
void serial_print_hex_u32(uint32_t v);

#if defined(ZSBL_BIST) || defined(FSBL_BIST)
void serial_bist(void);
#endif

#endif
