*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LT_GetCode ram interface
	XREF	_GTLayoutBase
	SECTION	gtlayout,code
	XDEF	_LT_GetCode
_LT_GetCode:
		movem.l	d2/a6,-(sp)
		move.l	_GTLayoutBase,a6
		movem.l	12(sp),d0/d1/d2/a0
		jsr	-186(a6)
		movem.l	(sp)+,d2/a6
		rts
	END
