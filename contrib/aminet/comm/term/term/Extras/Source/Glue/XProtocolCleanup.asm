*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XProtocolCleanup ram interface
	XREF	_XProtocolBase
	SECTION	xprotocol,code
	XDEF	_XProtocolCleanup
_XProtocolCleanup:
		move.l	a6,-(sp)
		move.l	_XProtocolBase,a6
		move.l	8(sp),a0
		jsr	-30(a6)
		move.l	(sp)+,a6
		rts
	END
