*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_MenuControlTagList ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_MenuControlTagList
_LT_MenuControlTagList:
		movem.l	a2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	12(sp),a0/a1/a2
		jsr	-240(a6)
		movem.l	(sp)+,a2/a6
		rts
	END
