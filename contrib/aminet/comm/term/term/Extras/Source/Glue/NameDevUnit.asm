*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** NameDevUnit ram interface
	XREF	_OwnDevUnitBase
	SECTION	OwnDevUnit,code
	XDEF	_NameDevUnit
_NameDevUnit:
		move.l	a6,-(sp)
		move.l	_OwnDevUnitBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/a1
		jsr	-48(a6)
		move.l	(sp)+,a6
		rts
	END
