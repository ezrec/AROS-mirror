*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_GetMenuItem ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_GetMenuItem
_LT_GetMenuItem:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-246(a6)
		move.l	(sp)+,a6
		rts
	END
