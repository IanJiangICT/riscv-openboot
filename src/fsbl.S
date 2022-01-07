#include "plat_def.h"
#include "plat_ops.h"
#include "storage_func.h"

/* Per socket stage */
#define FSBL_STAGE_CHIPLINK_START 		1
#define FSBL_STAGE_CHIPLINK_END			2
#define FSBL_STAGE_SHAREZONE_START 		3
#define FSBL_STAGE_SHAREZONE_END		4

/* Global stage. Recorded in the 1st uint32_t of Share-Zone */
#define FSBL_STAGE_OPENSBI_LOAD_START	5
#define FSBL_STAGE_OPENSBI_LOAD_END		6

.section .text.init;
.align 6;
.globl _start;
.globl pc_start;
.globl mc_start;

/* 
 * Common entry 
*/
_start:
	/* Check core type: MC or PC ? */
	csrr	t0,	SOC_CORE_TYPE_CSR
	li		t1,	SOC_CORE_TYPE_MC
	beq		t0,	t1,	mc_start

/* 
 * PC flow
 */
pc_start:
	/* Configure chiplink at 1 hart per socket */
	csrr	t0,	mhartid
	andi	t0,	t0,	SOC_CORE_HARTID_MASK_SOCKET
	bnez	t0,	_wait_chiplink_init

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_CHIPLINK_START
	sw		t1,	0(t0)
	fence
	fence.i

	call	plat_chiplink_init

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_CHIPLINK_END
	sw		t1,	0(t0)
	fence
	fence.i
	j		_end_chiplink_init

_wait_chiplink_init:
	la		t0,	_pc_stage
	li		t2,	FSBL_STAGE_CHIPLINK_END
1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lw		t1,	0(t0)
	blt		t1,	t2,	1b

_end_chiplink_init:
	nop

	/* Setup share-zone at 1 hart per socket */
	csrr	t0,	mhartid
	andi	t0,	t0,	SOC_CORE_HARTID_MASK_SOCKET
	bnez	t0,	_wait_sz_init

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_SHAREZONE_START
	sw		t1,	0(t0)
	fence
	fence.i

	call	plat_setup_sz

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_SHAREZONE_END
	sw		t1,	0(t0)
	fence
	fence.i
	j		_end_sz_init

_wait_sz_init:
	la		t0,	_pc_stage
	li		t2,	FSBL_STAGE_SHAREZONE_END
1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lw		t1,	0(t0)
	blt		t1,	t2,	1b

_end_sz_init:
	nop

	/* Load OpenSBI at only 1 hart globally */
	csrr	t0,	mhartid
	bnez	t0,	_wait_opensbi_load

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_OPENSBI_LOAD_START
	sw		t1,	0(t0)
	fence
	fence.i

	call	storage_load_opensbi

	la		t0,	_pc_stage
	li		t1,	FSBL_STAGE_OPENSBI_LOAD_END
	sw		t1,	0(t0)
	fence
	fence.i
	j		_end_opensbi_load

_wait_opensbi_load:
	la		t0,	_pc_stage
	li		t2,	FSBL_STAGE_OPENSBI_LOAD_END
1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lw		t1,	0(t0)
	blt		t1,	t2,	1b

_end_opensbi_load:
	nop

	/* Goto OpenSBI at all harts */
goto_opensbi:
	li		t0,	PLAT_DDR_OPENSBI
	jr		t0

fsbl_error:
	jal		fsbl_idle

/*
 * MC flow
 */
mc_start:
	call	fsbl_first_print
	call	plat_power_init
	call	plat_clock_init
	call	plat_setup_pg
	call	plat_ddrctrl_init
	call	plat_start_pc
	call	plat_mc_fsbl_final

fsbl_idle:
	wfi

.align 3;
_pc_stage:
	.word	0
	

