*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XProtocolSetup ram interface
	XREF	_XProtocolBase
	SECTION	xprotocol,code
	XDEF	_XProtocolSetup
_XProtocolSetup:
		move.l	a6,-(sp)
		move.l	_XProtocolBase,a6
		move.l	8(sp),a0
		jsr	-36(a6)
		move.l	(sp)+,a6
		rts
	END
