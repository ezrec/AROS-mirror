*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_Build ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_Build
_LT_Build:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	8(sp),a1
		move.l	(a1)+,a0
		jsr	-204(a6)
		move.l	(sp)+,a6
		rts
	END
