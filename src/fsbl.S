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

#if defined(MC_ACTS_AS_PC) && defined(PC_ACTS_AS_MC)
	#error "Invalid configuration. MC_ACTS_AS_PC and PC_ACTS_AS_MC are true at the same time."
#endif

/* 
 * Common entry 
*/
_start:
	/* Check core type: MC or PC ? */
	csrr	t0,	SOC_CORE_TYPE_CSR
	li		t1,	SOC_CORE_TYPE_MC
	beq		t0,	t1,	mc_start

#ifdef PC_ACTS_AS_MC
pc_acts_as_mc:
	/* Only one PC (with hartid = 0) per socket acts as MC */
	/* No need to set up partial-good */
	csrr	t0,	mhartid
	andi	t0,	t0,	SOC_CORE_HARTID_MASK_SOCKET
	bnez	t0,	pc_start
	call	fsbl_first_print
	call	plat_power_init
	call	plat_clock_init
	call	plat_ddrctrl_init
#endif

#ifdef FSBL_BIST
	call	fsbl_run_bist
#endif

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
#ifdef PC_ACTS_AS_MC
	j		fsbl_idle
#else
	call	fsbl_first_print
	call	plat_power_init
	call	plat_clock_init
#ifdef STORAGE_PROBE
	call	storage_probe
#endif
	call	plat_setup_pg
	call	plat_ddrctrl_init
#ifdef MC_ACTS_AS_PC
mc_acts_as_pc:
	nop
#else
	call	plat_start_pc
#endif
	call	plat_mc_fsbl_final
#endif

fsbl_idle:
	wfi

.align 3;
_pc_stage:
	.word	0
	

