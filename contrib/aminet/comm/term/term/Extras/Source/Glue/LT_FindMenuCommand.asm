*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_FindMenuCommand ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_FindMenuCommand
_LT_FindMenuCommand:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/d1/a1
		jsr	-252(a6)
		move.l	(sp)+,a6
		rts
	END
