*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_GetAttributesA ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_GetAttributesA
_LT_GetAttributesA:
		move.l	a6,-(sp)
		move.l	_GTLayoutBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/a1
		jsr	-78(a6)
		move.l	(sp)+,a6
		rts
	END
