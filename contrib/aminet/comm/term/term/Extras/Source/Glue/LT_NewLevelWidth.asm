*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_NewLevelWidth ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_NewLevelWidth
_LT_NewLevelWidth:
		movem.l	d2/d3/a2/a3/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	24(sp),a0/a1/a2
		movem.l	36(sp),d0/d1/a3
		move.l	48(sp),d3
		move.l	52(sp),d2
		jsr	-258(a6)
		movem.l	(sp)+,d2/d3/a2/a3/a6
		rts
	END
