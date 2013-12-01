	JUMPPTR	wb

start:
	movem.l	d1-a6,-(a7)
	move.l	4.w,a6
	sub.l	a1,a1
	jsr	-294(a6)		;FindTask
	move.l	d0,a4
	tst.l	172(a4)			;Cli oder WB
	bne.w	cli
	lea	92(a4),a0
	jsr	-384(a6)		;WaitPort
	lea	92(a4),a0
	jsr	-372(a6)		;GetMessage
	move.l	d0,message
	jsr	wb
	move.l	4.w,a6
	jsr	-132(a6)		;Forbid
	move.l	message(pc),a1
	jsr	-378(a6)		;ReplyMessage
	movem.l	(a7)+,d1-a6
	moveq	#0,d0
	rts

message:	dc.l	0

	dc.b	'$VER: Test-WBMessage V39.1 (12-10-97)',10
	dc.b	'© by Stefan Sommerfeld of ALiENDESiGN!',0
	even

wb:
	movem.l	d1-a6,-(a7)
cli:
	move.l	4.w,a6
	moveq	#0,d0
	lea	dosname(pc),a1
	jsr	-552(a6)		;OpenLibrary
	move.l	d0,dosbase
	tst.l	message
	beq.w	.end
	move.l	d0,a6
	move.l	#conname,d1
	move.l	#1006,d2
	jsr	-30(a6)			;Open
	move.l	d0,d1
	move.l	d0,d4
	move.l	#format,d2
	move.l	message,a0
	move.l	32(a0),-(a7)
	move.l	28(a0),-(a7)
	move.l	24(a0),-(a7)
	move.l	20(a0),-(a7)
	move.l	a7,d3
	jsr	-354(a6)		;VFPrintf
	lea	4*4(a7),a7

	lea	-1000(a7),a7
	move.l	message,a2
	move.l	28(a2),d5
	subq.w	#1,d5
	bmi.s	.loopend
	move.l	36(a2),a2
	move.l	a7,a4
.loop:
	sf	(a4)
	move.l	(a2),d1
	move.l	a4,d2
	move.l	#1000,d3
	jsr	-402(a6)		;NameFromLock
	move.l	d4,d1
	move.l	#format2,d2
	move.l	4(a2),-(a7)
	tst.l	(a7)
	bne.s	.nostr
	move.l	#nullstr,(a7)
.nostr:
	move.l	a4,-(a7)
	move.l	(a2),-(a7)
	move.l	a7,d3
	jsr	-354(a6)		;VFPrintf
	lea	3*4(a7),a7
	addq.w	#8,a2
	dbf	d5,.loop
.loopend:
	jsr	-600(a6)		;GetProgramDir
	move.l	d0,d1
	move.l	a4,d2
	move.l	#1000,d3
	jsr	-402(a6)		;NameFromLock
	move.l	a4,-(a7)
	move.l	a7,d3
	move.l	d4,d1
	move.l	#format3,d2
	jsr	-354(a6)		;VFPrintf
	lea	4(a7),a7

	lea	1000(a7),a7
.end:
	moveq	#0,d0
	movem.l	(a7)+,d1-a6
	rts

dosbase:	dc.l	0
dosname:	dc.b	'dos.library',0

format:		dc.b	'WB-Message:',10
		dc.b	' Process: %-8lx',10
		dc.b	' Seglist: %-8lx',10
		dc.b	' NumArgs: %ld',10
		dc.b	' ToolWin: %-8lx',10
		dc.b	' ArgList:',10,0

format2:	dc.b	' - Lock: %-8lx "%s"',10
		dc.b	'   Name: "%s"',10,0
format3:	dc.b	10,'ProgramDir: "%s"',10,0

conname:	dc.b	'CON:0/0//200/WBMessage Window/AUTO',0
nullstr:	dc.b	'*** NULL POINTER ***',0

