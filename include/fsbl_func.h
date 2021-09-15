#ifndef FSBL_FUNC_H
#define FSBL_FUNC_H

#ifdef __ASM__
.globl fsbl_first_print;
#else
void fsbl_first_print(void);
#endif

#endif
