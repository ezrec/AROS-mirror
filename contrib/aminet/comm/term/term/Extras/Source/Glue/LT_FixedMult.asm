*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_FixedMult ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_FixedMult
_LT_FixedMult:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),d0/d1
		jsr	-132(a6)
		move.l	(sp)+,a6
		rts
	END
