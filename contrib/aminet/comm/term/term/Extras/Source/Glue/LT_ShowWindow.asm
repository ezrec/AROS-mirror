*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_ShowWindow ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_ShowWindow
_LT_ShowWindow:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-168(a6)
		move.l	(sp)+,a6
		rts
	END
