*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_SetAttributes ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_SetAttributes
_LT_SetAttributes:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	8(sp),a1
		move.l	(a1)+,a0
		move.l	(a1)+,d0
		jsr	-84(a6)
		move.l	(sp)+,a6
		rts
	END
