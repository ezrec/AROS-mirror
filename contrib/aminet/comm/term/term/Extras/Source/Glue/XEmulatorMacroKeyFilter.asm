*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorMacroKeyFilter ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorMacroKeyFilter
_XEmulatorMacroKeyFilter:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		movem.l	8(sp),a0/a1
		jsr	-114(a6)
		move.l	(sp)+,a6
		rts
	END
