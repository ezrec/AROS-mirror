*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_Layout ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_Layout
_LT_Layout:
		movem.l	d2/d3/a2/a3/a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	24(sp),a3
		movem.l	(a3)+,a0/a1/a2
		movem.l	(a3)+,d0/d1/d2/d3
		jsr	-108(a6)
		movem.l	(sp)+,d2/d3/a2/a3/a6
		rts
	END
