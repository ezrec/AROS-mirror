*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_String2Fixed ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_String2Fixed
_LT_String2Fixed:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-126(a6)
		move.l	(sp)+,a6
		rts
	END
