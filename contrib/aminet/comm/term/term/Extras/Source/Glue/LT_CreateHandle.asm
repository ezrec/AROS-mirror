*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_CreateHandle ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_CreateHandle
_LT_CreateHandle:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-42(a6)
		move.l	(sp)+,a6
		rts
	END
