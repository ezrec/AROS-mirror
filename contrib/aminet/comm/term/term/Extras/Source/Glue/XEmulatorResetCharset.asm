*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorResetCharset ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorResetCharset
_XEmulatorResetCharset:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		jsr	-102(a6)
		move.l	(sp)+,a6
		rts
	END
