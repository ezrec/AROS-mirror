*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_LevelWidth ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_LevelWidth
_LT_LevelWidth:
		movem.l	d2/a2/a3/a5/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	24(sp),a0/a1/a2
		movem.l	36(sp),d0/d1/a3/a5
		move.l	52(sp),d2
		jsr	-30(a6)
		movem.l	(sp)+,d2/a2/a3/a5/a6
		rts
	END
