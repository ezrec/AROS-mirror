*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_Rebuild ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_Rebuild
_LT_Rebuild:
		movem.l	a2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	12(sp),a0/a1/a2
		movem.l	24(sp),d0/d1
		jsr	-54(a6)
		movem.l	(sp)+,a2/a6
		rts
	END
