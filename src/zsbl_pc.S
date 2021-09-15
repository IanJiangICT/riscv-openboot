#include "bootconf.h"
#include "zsbl_common.h"
#include "pc_private.h"
#include "plat_def.h"

.section .text.init;
.align	6;
.globl _start;

_start:
	nop

core_init:
	INIT_GPR
	INIT_TRAP	pc_trap_handler

do_private:
	PC_PRIVATE

zsbl_start:
	nop

stack_setup:
	li		sp,	PLAT_RAM_STACK_BASE_PC	
	li		t0,	PLAT_RAM_STACK_SIZE_PC	
	csrr	t1,	mhartid
	andi	t1,	t1,	SOC_CORE_HARTID_MASK_SOCKET
	li		t2, 1
_get_stack_bottom:
	add		sp,	sp,	t0
	beqz	t1, _stack_bottom_ok
	sub		t1,	t1,	t2
	j		_get_stack_bottom
_stack_bottom_ok:
	li		t0,	0x40
	sub		sp,	sp,	t0

call_funcs:
	nop

goto_fsbl:
	li		t0,	PLAT_RAM_FSBL
	jr		t0

zsbl_error:
	wfi

pc_trap_handler:
	HANDLE_TRAP
