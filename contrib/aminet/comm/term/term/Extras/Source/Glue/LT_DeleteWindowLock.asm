*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_DeleteWindowLock ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_DeleteWindowLock
_LT_DeleteWindowLock:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-162(a6)
		move.l	(sp)+,a6
		rts
	END
