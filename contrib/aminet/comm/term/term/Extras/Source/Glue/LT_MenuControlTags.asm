*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_MenuControlTags ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_MenuControlTags
_LT_MenuControlTags:
		movem.l	a2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	12(sp),a2
		movem.l	(a2)+,a0/a1
		jsr	-240(a6)
		movem.l	(sp)+,a2/a6
		rts
	END
