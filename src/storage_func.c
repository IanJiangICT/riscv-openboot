#include "simple_types.h"
#include "bootconf.h"
#include "plat_def.h"
#include "plat_ops.h"
#include "gpt_def.h"
#include "serial_func.h"

static void rom_offchip_read(unsigned int offset, unsigned char *buf, unsigned int size)
{
	unsigned char *src = (unsigned char *)PLAT_ROM_BASE_OFFCHIP + offset;
	unsigned int i;
	for (i = 0; i < size; i++) {
		buf[i] = src[i];
	}
	return;
}

static uint32_t gpt_load_part(int part_num, uint8_t *load_to, uint32_t storage)
{
	uint32_t entry_offset;
	uint32_t entry_size;
	struct gpt_entry this_entry;
	uint32_t this_offset;
	uint32_t this_size;
	uint32_t pad_size;
	unsigned char *guid_bytes;
#ifdef FSBL_FUNC
	char print_str[] = "0 GPT...\n";
#endif
	uint32_t storage_offset;
	void (*storage_read)(unsigned int, unsigned char *, unsigned int);
	
#ifdef FSBL_FUNC
	serial_print_str("storage: ");
#endif
	if (storage == 0) {
#ifdef FSBL_FUNC
		serial_print_str(" null\n");
#endif
		return 0;
	} else if (storage == BC_STORAGE_FLASH_0 || storage == BC_STORAGE_FLASH_I) {
		storage_offset = 0;
		storage_read = &plat_flash_read;
#ifdef FSBL_FUNC
		serial_print_str(" flash ");
#endif
	} else if (storage == BC_STORAGE_SD_0 || storage == BC_STORAGE_SD_I) {
		storage_offset = 0;
		storage_read = &plat_sd_read;
#ifdef FSBL_FUNC
		serial_print_str(" sd ");
#endif
	} else if (storage == BC_STORAGE_ROM_OFFCHIP) {
		storage_offset = PLAT_ROM_SIZE;
		storage_read = &rom_offchip_read;
#ifdef FSBL_FUNC
		serial_print_str(" rom offchip ");
#endif
	} else {
#ifdef FSBL_FUNC
		serial_print_str(" not supported \n");
#endif
		return 0;
	}

#ifdef FSBL_FUNC
	print_str[0] += part_num;
	serial_print_str(print_str);
#endif

	entry_size = sizeof(this_entry);
	entry_offset = GPT_SECTOR_SIZE * GPT_PART_START_SECTOR; // Base of entry table
	entry_offset += entry_size * part_num;
	storage_read((unsigned int)(storage_offset + entry_offset), (unsigned char *)(&this_entry), (unsigned int)entry_size);

	this_offset = (uint32_t)(this_entry.lba_start << GPT_SECTOR_SIZE_SHIFT);
	this_size = (uint32_t)((this_entry.lba_end + 1 - this_entry.lba_start) << GPT_SECTOR_SIZE_SHIFT);
	guid_bytes = (unsigned char *)(&this_entry.partition_guid);
	pad_size = guid_bytes[14];
	pad_size <<= 8;
	pad_size += guid_bytes[15];

	storage_read((unsigned int)(storage_offset + this_offset), load_to, this_size);

	return (this_size - pad_size);
}

void storage_init(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	if (bc->storage_online & (BC_STORAGE_FLASH_0 | BC_STORAGE_FLASH_I)) {
#ifdef FSBL_FUNC
		serial_print_str("storage flash\n");
#endif
		plat_flash_init();
	}
	if (bc->storage_online & (BC_STORAGE_SD_0 | BC_STORAGE_SD_I)) {
#ifdef FSBL_FUNC
		serial_print_str("storage sd\n");
#endif
		plat_sd_init();
	}
	return;
}

#ifdef STORAGE_PROBE
/*
 * Read GUID type of the 1st GPT partition.
 */

/* Identify with type Linux filesystem */
static unsigned char gpt_type_guid[] = {
	0xAF, 0x3D, 0xC6, 0x0F,
	0x83, 0x84, 0x72, 0x47,
	0x8E, 0x79, 0x3D, 0x69,
	0xD8, 0x47, 0x7D, 0xE4
};

int storage_probe(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint32_t storage = bc->storage_online;
	uint32_t entry_offset;
	uint32_t entry_size;
	struct gpt_entry this_entry;
	uint32_t storage_offset;
	void (*storage_read)(unsigned int, unsigned char *, unsigned int);
	int part_num = 0;
	int i;
	unsigned char *probe_buf = (unsigned char *)(&this_entry);
#ifdef STORAGE_PROBE_QUICK
	unsigned int probe_size = sizeof(gpt_type_guid) / 4;
#else
	unsigned int probe_size = sizeof(gpt_type_guid);
#endif
	
	if ((storage & (BC_STORAGE_FLASH_0 | BC_STORAGE_FLASH_I)) != 0) {
		storage_offset = 0;
		storage_read = &plat_flash_read;
	} else { // TODO Other storage type
		return 1;
	}

	entry_size = sizeof(this_entry.type);
	entry_offset = GPT_SECTOR_SIZE * GPT_PART_START_SECTOR;
	entry_offset += entry_size * part_num;
	storage_read((unsigned int)(storage_offset + entry_offset), probe_buf, probe_size);

	for (i = 0; i < probe_size; i++) {
		if (probe_buf[i] != gpt_type_guid[i]) return -1;
	}
	return 0;
}
#else
int storage_probe(void) { return 0; }
#endif

void storage_load_bc(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;

	/* Keep using current bootconf, which is copied from ROM before (in zsbl_set_bc_rom()) */
	if (bc->storage_bc == BC_STORAGE_ROM_ONCHIP || bc->storage_bc == BC_STORAGE_ROM_OFFCHIP) {
		return;
	}

	gpt_load_part(GPT_PART_BOOTCONF, (uint8_t *)bc, bc->storage_bc);
	plat_bc_fix();
	return;
}

void storage_load_fsbl(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint8_t *dst = (uint8_t *)PLAT_RAM_FSBL;

	gpt_load_part(GPT_PART_FSBL, dst, bc->storage_fsbl);
	return;
}

void storage_load_opensbi(void)
{
	struct bootconf *bc = (struct bootconf *)PLAT_RAM_BC;
	uint8_t *dst = (uint8_t *)PLAT_DDR_OPENSBI;

	gpt_load_part(GPT_PART_OPENSBI, dst, bc->storage_opensbi);
	return;
}
