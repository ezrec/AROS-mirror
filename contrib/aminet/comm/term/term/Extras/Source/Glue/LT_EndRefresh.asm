*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_EndRefresh ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_EndRefresh
_LT_EndRefresh:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-72(a6)
		move.l	(sp)+,a6
		rts
	END
