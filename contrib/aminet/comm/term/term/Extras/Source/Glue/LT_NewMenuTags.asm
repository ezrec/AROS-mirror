*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_NewMenuTags ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_NewMenuTags
_LT_NewMenuTags:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		lea	8(sp),a0
		jsr	-234(a6)
		move.l	(sp)+,a6
		rts
	END
