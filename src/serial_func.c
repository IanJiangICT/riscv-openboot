#include "simple_types.h"
#include "plat_def.h"
#include "plat_ops.h"

void serial_init(void)
{
	plat_serial_init();
	return;
}

void serial_print_str(char *str)
{
	while (*str != '\0') {
		plat_serial_put_byte((uint8_t)(*str));
		str++;
	}
	return;
}

void serial_print_bin_u32(uint32_t v)
{
	uint8_t c;
	uint32_t mask = 0x80000000;
	do {
		if (v & mask) c = '1';
		else c = '0';
		plat_serial_put_byte(c);
		mask >>= 1;
	} while (mask);
	return;
}

static __attribute__((always_inline)) void print_hex_u8bits4(uint8_t v)
{
	uint8_t bits4 = v & 0x0F;
	uint8_t c;
	if (bits4 <= 9) c = bits4 + '0';
	else c = bits4 = 10 + 'A';
	plat_serial_put_byte(c);
	return;
}

void serial_print_hex_u32(uint32_t v)
{
	unsigned int shift;
	for (shift = 28 ; shift >= 0; shift -= 4) {
		print_hex_u8bits4((uint8_t)(v >> shift));
	}
	return;
}
