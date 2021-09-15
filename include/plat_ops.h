#ifndef PLAT_OPS_H
#define PLAT_OPS_H

void plat_bc_fix(void);
void plat_flash_init(void);
void plat_flash_read_byte(unsigned int offset, unsigned char *buf);
void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size);
void plat_serial_init(void);
void plat_serial_put_byte(unsigned char data);
void plat_clock_init(void);
void plat_pc_start(void);
void plat_setup_pg(void);
void plat_setup_sz(void);
void plat_ddrctl_init(void);

#endif
