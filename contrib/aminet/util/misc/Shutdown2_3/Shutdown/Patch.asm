	section	text,code

	xref	_LinkerDB

PATCH	macro

	xdef	_New\1
	xref	_My\1

_New\1:	movem.l	d2-d7/a2-a6,-(sp)
	lea	_LinkerDB,a4
	jsr	_My\1
	move.l	d0,d1
	movem.l	(sp)+,d2-d7/a2-a6
	rts

	endm

	PATCH	Open
	PATCH	Close
	PATCH	Write
	PATCH	FPutC
	PATCH	FPuts
	PATCH	FWrite
	PATCH	VFPrintf
	PATCH	VFWritef
	PATCH	PutStr
	PATCH	WriteChars
	PATCH	VPrintf

	xdef	_BlockBeginIO
	xref	_BlockRoutine

_BlockBeginIO:
	movem.l	d0-d7/a0-a6,-(sp)
	lea	_LinkerDB,a4
	jsr	_BlockRoutine
	movem.l	(sp)+,d0-d7/a0-a6
	rts

	xdef	_NewDevExpunge
	xref	_DevExpunge

_NewDevExpunge:
	movem.l	d2-d7/a2-a6,-(sp)
	lea	_LinkerDB,a4
	jsr	_DevExpunge
	movem.l	(sp)+,d2-d7/a2-a6
	rts

	end
