*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** FreeDevUnit ram interface
	XREF	_OwnDevUnitBase
	SECTION	OwnDevUnit,code
	XDEF	_FreeDevUnit
_FreeDevUnit:
		move.l	a6,-(sp)
		move.l	_OwnDevUnitBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-42(a6)
		move.l	(sp)+,a6
		rts
	END
