*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XProtocolSend ram interface
	XREF	_XProtocolBase
	SECTION	xprotocol,code
	XDEF	_XProtocolSend
_XProtocolSend:
		move.l	a6,-(sp)
		move.l	_XProtocolBase,a6
		move.l	8(sp),a0
		jsr	-42(a6)
		move.l	(sp)+,a6
		rts
	END
