*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** XProtocolHostMon ram interface
	XREF	_XProtocolBase
	SECTION	xprotocol,code
	XDEF	_XProtocolHostMon
_XProtocolHostMon:
		move.l	a6,-(sp)
		move.l	_XProtocolBase,a6
		movem.l	8(sp),a0/a1
		movem.l	16(sp),d0/d1
		jsr	-54(a6)
		move.l	(sp)+,a6
		rts
	END
