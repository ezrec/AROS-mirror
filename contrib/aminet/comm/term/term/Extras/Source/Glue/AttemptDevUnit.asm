*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** AttemptDevUnit ram interface
	XREF	_OwnDevUnitBase
	SECTION	OwnDevUnit,code
	XDEF	_AttemptDevUnit
_AttemptDevUnit:
		move.l	a6,-(sp)
		move.l	_OwnDevUnitBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/a1
		move.l	20(sp),d1
		jsr	-36(a6)
		move.l	(sp)+,a6
		rts
	END
