*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_Activate ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_Activate
_LT_Activate:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-174(a6)
		move.l	(sp)+,a6
		rts
	END
