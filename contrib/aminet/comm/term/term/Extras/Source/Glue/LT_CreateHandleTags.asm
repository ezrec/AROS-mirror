*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_CreateHandleTags ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_CreateHandleTags
_LT_CreateHandleTags:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	8(sp),a1
		move.l	(a1)+,a0
		jsr	-48(a6)
		move.l	(sp)+,a6
		rts
	END
