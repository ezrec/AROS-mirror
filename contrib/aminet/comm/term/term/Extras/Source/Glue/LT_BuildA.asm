*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_BuildA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_BuildA
_LT_BuildA:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-204(a6)
		move.l	(sp)+,a6
		rts
	END
