*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_AddA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_AddA
_LT_AddA:
		movem.l	d2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	12(sp),a0
		movem.l	16(sp),d0/d1/d2/a1
		jsr	-90(a6)
		movem.l	(sp)+,d2/a6
		rts
	END
