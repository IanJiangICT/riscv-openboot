#ifndef ZSBL_COMMON_H
#define ZSBL_COMMON_H

#ifdef __ASM__
.macro INIT_GPR
	addi	x1,		x0,		0
	addi	x2,		x0,		0
	addi	x3,		x0,		0
	addi	x4,		x0,		0
	addi	x5,		x0,		0
	addi	x6,		x0,		0
	addi	x7,		x0,		0
	addi	x8,		x0,		0
	addi	x9,		x0,		0
	addi	x10,	x0,		0
	addi	x11,	x0,		0
	addi	x12,	x0,		0
	addi	x13,	x0,		0
	addi	x14,	x0,		0
	addi	x15,	x0,		0
	addi	x16,	x0,		0
	addi	x17,	x0,		0
	addi	x18,	x0,		0
	addi	x19,	x0,		0
	addi	x20,	x0,		0
	addi	x21,	x0,		0
	addi	x22,	x0,		0
	addi	x23,	x0,		0
	addi	x24,	x0,		0
	addi	x25,	x0,		0
	addi	x26,	x0,		0
	addi	x27,	x0,		0
	addi	x28,	x0,		0
	addi	x29,	x0,		0
	addi	x30,	x0,		0
	addi	x31,	x0,		0
.endm

.macro INIT_TRAP trap_handler
	li		t0,			-1
	csrw	mie,		t0
	csrci	mstatus,	0x8	// .MIE
	csrr	t0,			mstatus
	csrw	mscratch,	x0
	csrw	mepc,		x0
	csrw	mcause,		x0
	csrw	mip,		x0
	la		t0,			\trap_handler
	csrw	mepc,		t0
.endm

.macro	HANDLE_TRAP
	csrci	mstatus,	0x8	// .MIE
	csrr	t0,			mcause
	csrr	t0,			mstatus
	csrr	t0,			mepc
	csrr	t0,			mtval
	wfi
.endm
	
#endif

#endif
