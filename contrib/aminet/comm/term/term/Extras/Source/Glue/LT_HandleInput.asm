*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_HandleInput ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_HandleInput
_LT_HandleInput:
		movem.l	a2/a3/a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	16(sp),a0
		movem.l	20(sp),d0/a1/a2/a3
		jsr	-60(a6)
		movem.l	(sp)+,a2/a3/a6
		rts
	END
