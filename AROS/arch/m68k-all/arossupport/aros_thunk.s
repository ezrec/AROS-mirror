/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

/******************************************************************************

    NAME

	aros_thunk_relbase

    FUNCTION
	Save the current relbase, and call the function at the LVO
	specified.

    INPUTS

    RESULT
	None

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/


#include "aros/m68k/asm.h"

	.text
	.balign 4

	.globl	aros_thunk_libfunc

	// %sp(+12..) - Arguments to called function
	// %sp@(0) - Return address
	// D0 - New base address
	// D1 - LVO offset

aros_thunk_libfunc:
	movem.l	%d0-%d1/%a6,%sp@-

	// Get task's tc_SPLower
	move.l	SysBase, %a6
	sub.l	%a1,%a1
	jsr	%a6@(FindTask)
	move.l	%d0,%a6
	move.l	%a6@(tc_SPLower),%a6

	// Get relstack pointer
	clr.l	%d0
	move.b	%a6@(3),%d0
	lsl.l	#2,%d0
	lea.l	%a6@(4,%d0:w),%a0	// %a0 is now relstack pointer

	// A0 - Relstack pointer
	// A6 - RelBase pointer
	// SP - New libbase
	//      Offset of LVO
	//      Old A6
	//      Return address
	//      arguments to LVO to call
aros_thunk_common:
	movem.l	%sp@+,%d0-%d1/%a1
	addq.l	#4,%a0
	move.l	%a1,%a0@+		// Save old A6 off-stack
	move.l	%sp@+,%a0@+		// Save return address off-stack
	move.l	%d0,%a0@+		// Save new libbase
	addq.b	#3,%a6@(3)		// Adjust relstack pointer

	add.l	%d0,%d1			// Get LVO address

	move.l	%d1,%a1
	jsr 	%a1@			// Call routine
	movem.l	%d0-%d1,%sp@-		// Save returned values
	clr.l	%d0
	move.b	%a6@(3),%d0
	lsl.l	#2,%d0
	lea.l	%a6@(4,%d0:w),%a0	// %a0 is now relstack pointer
	movem.l	%a0@(-8),%d0-%d1/%a1	// Get saved values
	subq.b	#3,%a6@(3)		// Adjust relstack pointer
	move.l	%d0,%a6			// Restore A6
	move.l	%d1,%a1			// A1 is the return address
	movem.l	%sp@+,%d0-%d1		// Restore values
	jmp	%a1@			// Return
