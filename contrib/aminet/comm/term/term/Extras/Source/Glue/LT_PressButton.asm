*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_PressButton ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_PressButton
_LT_PressButton:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-180(a6)
		move.l	(sp)+,a6
		rts
	END
