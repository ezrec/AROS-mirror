*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XEmulatorUserMon ram interface
	XREF	_XEmulatorBase
	SECTION	xemulator,code
	XDEF	_XEmulatorUserMon
_XEmulatorUserMon:
		movem.l	a2/a6,-(sp)
		move.l	_XEmulatorBase,a6
		movem.l	12(sp),a0/a1
		movem.l	20(sp),d0/a2
		jsr	-72(a6)
		movem.l	(sp)+,a2/a6
		rts
	END
