*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_RebuildTagList ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_RebuildTagList
_LT_RebuildTagList:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/a1
		jsr	-210(a6)
		move.l	(sp)+,a6
		rts
	END
