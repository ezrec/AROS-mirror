*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorClearConsole ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorClearConsole
_XEmulatorClearConsole:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		jsr	-84(a6)
		move.l	(sp)+,a6
		rts
	END
