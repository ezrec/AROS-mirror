**
**	StackCall.asm
**
**	Stack swap & stack size check routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**

	include	"exec/types.i"
	include	"exec/tasks.i"
	include	"exec/memory.i"
	include	"exec/execbase.i"

	include	"dos/dos.i"
	include	"dos/dosextens.i"

*----------------------------------------------------------------------

CALL	macro
	xref	_LVO\1
	jsr	_LVO\1(a6)
	endm

*----------------------------------------------------------------------

ARG_SUCCESS	equ	4+5*4
ARG_STACKSIZE	equ	ARG_SUCCESS+4
ARG_ARGCOUNT	equ	ARG_STACKSIZE+4
ARG_ROUTINE	equ	ARG_ARGCOUNT+4
ARG_ARGS	equ	ARG_ROUTINE+4

TRUE		equ	1
FALSE		equ	0

*----------------------------------------------------------------------

	section	text,code

*----------------------------------------------------------------------

	xdef	_StackCall

	;	LONG __stdargs StackCall(	LONG *Success,
	;					LONG StackSize,
	;					LONG ArgCount,
	;					LONG (* __stdargs Function)(...),
	;					...);
	;
	;	Calls a function with parameters on a newly-allocated
	;	stack. Caution: assumes all arguments are 32 bits wide.

_StackCall:

	movem.l	d2/d3/a2/a3/a6,-(sp)		; Save registers

	move.l	ARG_STACKSIZE(sp),d0		; Get desired stack size
	add.l	#15+StackSwapStruct_SIZEOF,d0	; Round to next...
	and.l	#-16,d0				; ...quad long word
	move.l	d0,d2				; Save it for later
	move.l	#MEMF_PUBLIC,d1			; Well?
	move.l	_SysBase,a6			; Get SysBase

	CALL	AllocMem			; Allocate the memory
	tst.l	d0				; Success?
	beq.b	1$				; Exit if not

	move.l	d0,a2				; Remember base adress

	add.l	#StackSwapStruct_SIZEOF,d0	; Skip the header
	move.l	d0,stk_Lower(a2)		; Store the stack lower bound

	move.l	a2,a3				; Get the base address
	add.l	d2,a3				; Add the allocation size
	move.l	a3,stk_Upper(a2)		; Store the stack upper bound

	move.l	ARG_ARGCOUNT(sp),d0		; Get the number of arguments
	beq.b	3$				; Skip if none

	moveq	#4,d1				; One long word per argument
	mulu	d0,d1				; Nasty
	ext.l	d1				; Forget the upper word
	addq.l	#4,d1				; One for predecrement
	sub.l	d1,a3				; Pull it down

	subq.l	#1,d0				; Subtract one for DBRA
	lea.l	ARG_ARGS(sp),a0			; Get the source argument list
	move.l	a3,a1				; Get the destination argument list

2$	move.l	(a0)+,(a1)+			; Copy the parameters
	dbra	d0,2$				; Loop until all is copied

3$	move.l	a3,stk_Pointer(a2)		; Store the new stack pointer

	move.l	ARG_ROUTINE(sp),a3		; Get address of the routine to call

	move.l	a2,a0				; Swap the stack
	CALL	StackSwap

	jsr	(a3)				; Call the routine
	move.l	d0,d3				; Save the result

	move.l	a2,a0				; Swap the stack
	CALL	StackSwap

	move.l	a2,a1				; Get the base address
	move.l	d2,d0				; Get the allocation length
	CALL	FreeMem				; Free the memory

	move.l	ARG_SUCCESS(sp),a0		; Flag as success
	move.l	#TRUE,(a0)

	move.l	d3,d0				; Return the result

	movem.l	(sp)+,d2/d3/a2/a3/a6		; Restore registers
	rts

1$	move.l	ARG_SUCCESS(sp),a0		; Flag as failure
	move.l	#FALSE,(a0)

	movem.l	(sp)+,d2/d3/a2/a3/a6		; Restore registers
	rts

*----------------------------------------------------------------------

	xdef	@StackSize
	xdef	_StackSize

	;	LONG StackSize(struct Task *Task);
	;
	;	Determines the amount of stack still available to a task.
	;	Pass in NULL as the task address to return the amount
	;	of stack available to the calling task.

_StackSize:
	move.l	4(sp),a0			; Get task address

@StackSize:
	move.l	a6,-(sp)			; Save registers

	move.l	_SysBase,a6			; Get SysBase

	move.l	a0,d0				; Valid task address?
	beq.b	4$

	move.l	TC_SPREG(a0),d0			; Remember stack pointer
	bra.b	3$

4$	move.l	a0,a1
	CALL	FindTask			; Who am I?
	move.l	d0,a0				; That's me
	move.l	sp,d0				; Remember stack pointer

3$	cmp.b	#NT_PROCESS,LN_TYPE(a0)		; Is this a process?
	bne.b	1$

	tst.l	pr_CLI(a0)			; Is it running as a CLI process?
	beq.b	1$

	move.l	pr_ReturnAddr(a0),d1		; Get upper bound
	addq.l	#4,d1
	move.l	pr_ReturnAddr(a0),a0
	sub.l	(a0),d1				; Subtract total stack size

	bra.b	2$

1$	move.l	TC_SPLOWER(a0),d1		; Get lower bound

2$	sub.l	d1,d0				; Subtract from stack pointer

	move.l	(sp)+,a6			; Restore registers

	rts

*----------------------------------------------------------------------

	section	data,data

	xref	_SysBase

	end
