*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorResetConsole ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorResetConsole
_XEmulatorResetConsole:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		jsr	-90(a6)
		move.l	(sp)+,a6
		rts
	END
