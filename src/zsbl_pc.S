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
do_set_noc:
	PC_SET_NOC

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
#ifdef PC_ACTS_AS_MC
acts_as_mc:
	/* TODO Support dual-socket */
	/* Only one PC (with hartid = 0) per socket acts as MC */
	li		t0, PLAT_RAM_SHARE_BASE
	sw		zero, 0(t0)
	csrr	t1, mhartid
	andi	t1, t1, SOC_CORE_HARTID_MASK_SOCKET
	bnez	t1, wait_funcs_end

	call	zsbl_set_bc_rom
#ifdef CLOCK_IN_ZSBL
	call	plat_clock_init
#endif
	call	zsbl_init_console
	call	storage_init
#ifdef STORAGE_PROBE
	call	storage_probe
#endif
	call	storage_load_bc
	call	zsbl_print_bc
	call	storage_load_fsbl

	li		t0, PLAT_RAM_SHARE_BASE
	li		t1, 1
	sw		t1, 0(t0)
	j		goto_fsbl

wait_funcs_end:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lw		t1, 0(t0)
	beqz	t1, wait_funcs_end
#endif

goto_fsbl:
	li		t0,	PLAT_RAM_FSBL
	jr		t0

zsbl_error:
	wfi

pc_trap_handler:
	HANDLE_TRAP
