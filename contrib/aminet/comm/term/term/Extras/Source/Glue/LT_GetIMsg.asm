*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_GetIMsg ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_GetIMsg
_LT_GetIMsg:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-192(a6)
		move.l	(sp)+,a6
		rts
	END
