*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_ReplyIMsg ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_ReplyIMsg
_LT_ReplyIMsg:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		jsr	-198(a6)
		move.l	(sp)+,a6
		rts
	END
