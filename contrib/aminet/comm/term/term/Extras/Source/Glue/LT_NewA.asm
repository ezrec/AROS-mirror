*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_NewA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_NewA
_LT_NewA:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-96(a6)
		move.l	(sp)+,a6
		rts
	END
