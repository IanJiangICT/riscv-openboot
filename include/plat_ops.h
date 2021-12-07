#ifndef PLAT_OPS_H
#define PLAT_OPS_H

#ifdef __ASM__
.globl plat_bc_fix;
.globl plat_flash_init;
.globl plat_flash_read_byte;
.globl plat_flash_read;
.globl plat_sd_init;
.globl plat_sd_read_byte;
.globl plat_sd_read;
.globl plat_serial_init;
.globl plat_serial_put_byte;
.globl plat_clock_init;
.globl plat_start_pc;
.globl plat_setup_pg;
.globl plat_setup_sz;
.globl plat_ddrctrl_init;
.globl plat_chiplink_init;
.globl plat_mc_fsbl_final;
#else
void plat_bc_fix(void);
void plat_flash_init(void);
void plat_flash_read_byte(unsigned int offset, unsigned char *buf);
void plat_flash_read(unsigned int offset, unsigned char *buf, unsigned int size);
void plat_sd_init(void);
void plat_sd_read_byte(unsigned int offset, unsigned char *buf);
void plat_sd_read(unsigned int offset, unsigned char *buf, unsigned int size);
void plat_serial_init(void);
void plat_serial_put_byte(unsigned char data);
void plat_clock_init(void);
void plat_start_pc(void);
void plat_setup_pg(void);
void plat_setup_sz(void);
void plat_ddrctrl_init(void);
void plat_chiplink_init(void);
void plat_mc_fsbl_final(void);
#endif

#endif
