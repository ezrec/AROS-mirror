
	incdir	include:

	include	exec/tasks.i
	include	libraries/ahi_sub.i
	include	lvo/exec_lib.i
	include	macros.i
	
	include	void.i

	XDEF	_intAHIsub_Disable
	XDEF	_intAHIsub_Enable

	XDEF	_SlaveEntry
	XREF	_SlaveTask

;in:
* a2	struct AHI_AudioCtrl
_intAHIsub_Disable:
	push	a6
	move.l	4.w,a6
	call	Forbid				; This sucks
	pop	a6
	rts

;in:
* a2	struct AHI_AudioCtrl
_intAHIsub_Enable:
	push	a6
	move.l	4.w,a6
	call	Permit				; This sucks
	pop	a6
	rts

_SlaveEntry:
	move.l	4.w,a6
	suba.l	a1,a1
	call	FindTask
	move.l	d0,a0
	move.l	TC_Userdata(a0),a2
	move.l	ahiac_DriverData(a2),a0
	move.l	v_AHIsubBase(a0),a6
	jmp	_SlaveTask
