*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_LayoutA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_LayoutA
_LT_LayoutA:
		movem.l	d2/d3/a2/a3/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	24(sp),a0/a1/a2
		movem.l	36(sp),d0/d1/d2/d3/a3
		jsr	-108(a6)
		movem.l	(sp)+,d2/d3/a2/a3/a6
		rts
	END
