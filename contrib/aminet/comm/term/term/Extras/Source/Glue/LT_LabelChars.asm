*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_LabelChars ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_LabelChars
_LT_LabelChars:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-144(a6)
		move.l	(sp)+,a6
		rts
	END
