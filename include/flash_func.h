#ifndef FLASH_FUNC_H
#define FLASH_FUNC_H

#ifdef __ASM__
.globl flash_init;
.globl flash_set_bc;
.globl flash_load_fsbl;
.globl flash_load_opensbi;
#else
void flash_init(void);
void flash_set_bc(void);
void flash_load_fsbl(void);
void flash_load_opensbi(void);
#endif

#endif
