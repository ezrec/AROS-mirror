*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorGetFreeMacroKeys ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorGetFreeMacroKeys
_XEmulatorGetFreeMacroKeys:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		move.l	12(sp),d0
		jsr	-108(a6)
		move.l	(sp)+,a6
		rts
	END
