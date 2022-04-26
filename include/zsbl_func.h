#ifndef ZSBL_FUNC_H
#define ZSBL_FUNC_H

void zsbl_set_bc_rom(void);
void zsbl_init_console(void);
void zsbl_print_bc(void);
#ifdef ZSBL_BIST
void zsbl_run_bist(void);
#endif

#endif
