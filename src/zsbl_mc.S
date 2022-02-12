#include "bootconf.h"
#include "zsbl_common.h"
#include "mc_private.h"
#include "plat_def.h"
#include "storage_func.h"

.section .text.init;
.align	6;
.globl _start;

_start:
	nop

core_init:
	INIT_GPR
	INIT_TRAP	mc_trap_handler

do_private:
	MC_PRIVATE

zsbl_start:
	nop

stack_setup:
	li		sp,	PLAT_RAM_STACK_BASE_MC	
	li		t0,	PLAT_RAM_STACK_SIZE_MC	
	add		sp,	sp,	t0
	li		t0,	0x40
	sub		sp,	sp,	t0

call_funcs:
	call	zsbl_set_bc_rom
	call	storage_init
	call	storage_probe
	call	storage_load_bc
	call	zsbl_init_console
	call	zsbl_print_bc
	call	storage_load_fsbl

goto_fsbl:
	li		t0,	PLAT_RAM_FSBL
	jr		t0

zsbl_error:
	wfi

mc_trap_handler:
	HANDLE_TRAP
