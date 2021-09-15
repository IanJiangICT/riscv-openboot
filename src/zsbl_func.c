#include "simple_types.h"
#include "bootconf.h"
#include "plat_ops.h"
#include "plat_def.h"

void zsbl_set_bc_rom(void)
{
	unsigned char *ptr_src = (unsigned char *)PLAT_ROM_BC0;
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
