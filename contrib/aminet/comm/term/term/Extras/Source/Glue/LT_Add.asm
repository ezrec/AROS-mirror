*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_Add ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_Add
_LT_Add:
		movem.l	d2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	12(sp),a1
		move.l	(a1)+,a0
		movem.l	(a1)+,d0/d1/d2
		jsr	-90(a6)
		movem.l	(sp)+,d2/a6
		rts
	END
