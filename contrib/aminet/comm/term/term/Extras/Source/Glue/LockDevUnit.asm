*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** LockDevUnit ram interface
	XREF	_OwnDevUnitBase
	SECTION	OwnDevUnit,code
	XDEF	_LockDevUnit
_LockDevUnit:
		move.l	a6,-(sp)
		move.l	_OwnDevUnitBase,a6
		move.l	8(sp),a0
		movem.l	12(sp),d0/a1
		move.l	20(sp),d1
		jsr	-30(a6)
		move.l	(sp)+,a6
		rts
	END
