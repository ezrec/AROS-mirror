*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorSetup ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorSetup
_XEmulatorSetup:
		move.l	a6,-(sp)
		move.l	_XEmulatorBase,a6
		move.l	8(sp),a0
		jsr	-30(a6)
		move.l	(sp)+,a6
		rts
	END
