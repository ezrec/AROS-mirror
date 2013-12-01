; putmsga.asm
; 04 Oct 2001 17:30:30

	incdir	"sc:Assembler_Headers"

	include	"exec/types.i"
	include	"exec/tasks.i"
	include	"exec/ports.i"
	include	"exec/nodes.i"
	include	"asmsupp.i"

	xdef	_origPutMsg
	xdef	_newPutMsg


	machine	68020

	section	putmsga,code

INFO_LEVEL	equ	1000


_origPutMsg:	dc.l	0

; void PutMsg(struct MsgPort *, struct Message *)
;	      A0                A1
_newPutMsg:
	move.l	a2,-(sp)
	tst.l	(MP_SIGTASK,a0)
	beq	.old
	move.l	(LN_NAME,[MP_SIGTASK,a0]),a2
	cmp.l	#'Scal',(a2)
	beq	.ok
	cmp.l	#'Work',(a2)
	beq	.ok
	bra	.old
.ok:
	move.l	(4,sp),d0
	move.l	d0,-(sp)
	move.l	a1,-(sp)
	move.l	a0,-(sp)
	PUTMSG	0,'%s:  Port=%08lx  Msg=%08lx  callAddr=%08lx'
	lea	(3*4,sp),sp

.old:	move.l	(sp)+,a2
	move.l	_origPutMsg,-(sp)
	rts

subSysName:	dc.b	'PutMsg',0

	end
