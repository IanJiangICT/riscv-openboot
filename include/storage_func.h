#ifndef STORAGE_FUNC_H
#define STORAGE_FUNC_H

#ifdef __ASM__

.globl storage_init;
.globl storage_load_bc;
.globl storage_load_fsbl;
.globl storage_load_opensbi;
#ifdef STORAGE_PROBE
.globl storage_probe;
#endif

#else

void storage_init(void);
void storage_load_bc(void);
void storage_load_fsbl(void);
void storage_load_opensbi(void);
#ifdef STORAGE_PROBE
int storage_probe(void);
#endif

#endif

#endif
