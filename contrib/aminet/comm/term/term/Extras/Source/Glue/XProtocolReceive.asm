*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XProtocolReceive ram interface
	XREF	_XProtocolBase
	SECTION	xprotocol,code
	XDEF	_XProtocolReceive
_XProtocolReceive:
		move.l	a6,-(sp)
		move.l	_XProtocolBase,a6
		move.l	8(sp),a0
		jsr	-48(a6)
		move.l	(sp)+,a6
		rts
	END
