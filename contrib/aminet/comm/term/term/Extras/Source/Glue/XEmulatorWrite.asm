*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorWrite ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorWrite
_XEmulatorWrite:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		movem.l	8(sp),a0/a1
		move.l	16(sp),d0
		jsr	-54(a6)
		move.l	(sp)+,a6
		rts
	END
