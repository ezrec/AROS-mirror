*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_LayoutMenusA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_LayoutMenusA
_LT_LayoutMenusA:
		movem.l	a2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	12(sp),a0/a1/a2
		jsr	-114(a6)
		movem.l	(sp)+,a2/a6
		rts
	END
