*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_LockWindow ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_LockWindow
_LT_LockWindow:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-150(a6)
		move.l	(sp)+,a6
		rts
	END
