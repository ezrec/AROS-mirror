*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_NewMenuTemplate ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_NewMenuTemplate
_LT_NewMenuTemplate:
		movem.l	a2/a3/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	16(sp),a0/a1/a2/a3
		movem.l	32(sp),d0/d1
		jsr	-228(a6)
		movem.l	(sp)+,a2/a3/a6
		rts
	END
