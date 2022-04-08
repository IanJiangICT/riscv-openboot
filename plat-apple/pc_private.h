#ifndef PC_PRIVATE_H
#define PC_PRIVATE_H

.macro	PC_PRIVATE
	csrr	t0,	marchid
.endm

.macro	PC_SET_NOC
	nop
.endm

#endif
