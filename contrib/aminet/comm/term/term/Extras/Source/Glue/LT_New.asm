*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_New ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_New
_LT_New:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	8(sp),a1
		move.l	(a1)+,a0
		jsr	-96(a6)
		move.l	(sp)+,a6
		rts
	END
