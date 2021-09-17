#include "simple_types.h"
#include "bootconf.h"
#include "plat_def.h"
#include "plat_ops.h"
#include "gpt_def.h"
#include "serial_func.h"

static void flash_load_part(int part_num, uint8_t *load_to)
{
	uint32_t entry_offset;
	uint32_t entry_size;
	struct gpt_entry this_entry;
	uint32_t this_offset;
	uint32_t this_size;
	uint32_t pad_size;
	unsigned char *guid_bytes;
	char print_str[] = "0 GPT...\n";

	serial_print_str("flash: ");
	print_str[0] += part_num;
	serial_print_str(print_str);

	entry_size = sizeof(this_entry);
	entry_offset = GPT_SECTOR_SIZE * GPT_PART_START_SECTOR; // Base of entry table
	entry_offset += entry_size * part_num;
	plat_flash_read((unsigned int)entry_offset, (unsigned char *)(&this_entry), (unsigned int)entry_size);

	this_offset = (uint32_t)(this_entry.lba_start << GPT_SECTOR_SIZE_SHIFT);
	this_size = (uint32_t)((this_entry.lba_end + 1 - this_entry.lba_start) << GPT_SECTOR_SIZE_SHIFT);
	guid_bytes = (unsigned char *)(&this_entry.partition_guid);
	pad_size = guid_bytes[14];
	pad_size <<= 8;
	pad_size += guid_bytes[15];

	plat_flash_read(this_offset, load_to, this_size);

	return;
}

void flash_init(void)
{
	plat_flash_init();
	return;
}

void flash_set_bc(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint8_t *dst = (uint8_t *)PLAT_RAM_FSBL;

	/* Keep using current bootconf, which is copied from ROM before */
	if (bc->conf_at == BC_CONF_AT_ROM) {
		return;
	}

	flash_load_part(GPT_PART_BOOTCONF, (uint8_t *)bc);
	plat_bc_fix();
	return;
}

void flash_load_fsbl(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint8_t *dst = (uint8_t *)PLAT_RAM_FSBL;

	if (bc->load_fsbl) {
		flash_load_part(GPT_PART_FSBL, dst);
	}
	return;
}

void flash_load_opensbi(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint8_t *dst = (uint8_t *)PLAT_DDR_OPENSBI;

	if (bc->load_opensbi) {
		flash_load_part(GPT_PART_OPENSBI, dst);
	}
	return;
}
