*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_CreateHandleTagList ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_CreateHandleTagList
_LT_CreateHandleTagList:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	8(sp),a0/a1
		jsr	-48(a6)
		move.l	(sp)+,a6
		rts
	END
