*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_DisposeMenu ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_DisposeMenu
_LT_DisposeMenu:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-222(a6)
		move.l	(sp)+,a6
		rts
	END
