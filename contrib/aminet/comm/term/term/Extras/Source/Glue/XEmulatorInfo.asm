*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorInfo ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorInfo
_XEmulatorInfo:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-120(a6)
		move.l	(sp)+,a6
		rts
	END
