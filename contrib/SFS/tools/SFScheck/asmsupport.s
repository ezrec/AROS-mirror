	Opt	l+,case

	XDef	_RANDOM
	XDef	_CALCCHECKSUM
	XDef	_MULU64
	XDef	_COMPRESSFROMZERO
	XDef	_COMPRESS
	XDef	_STACKSWAP

	Incdir	"INCLUDE:"

	Include	"exec/types.i"
	Include	"exec/nodes.i"
	Include	"exec/tasks.i"

Start

MuluLWQ	MACRO	;DataReg1,DataReg2
	;Reg1.L x Reg2.W = Reg1.L:Reg2.L
	Move.l	a0,-(sp)
	Move.l	\1,a0
	Mulu.w	\2,\1
	Exg	\1,a0
	Swap	\1
	Mulu.w	\1,\2
	Swap	\2
	Moveq	#0,\1
	Move.w	\2,\1
	Clr.w	\2
	Add.l	a0,\2
	Bcc.s	.ExitRoutine\@
	Addq.l	#1,\1
.ExitRoutine\@	Move.l	(sp)+,a0
	ENDM

MuluLLQ	MACRO	;DataReg1,DataReg2
	;Reg1.L x Reg2.L = Reg1.L:Reg2.L
	Movem.l	a0-a1,-(sp)
	Movem.l	\1/\2,-(sp)
	MuluLWQ	\1,\2
	Move.l	\1,a0
	Move.l	\2,a1
	Movem.l	(sp)+,\1/\2
	Swap	\2
	MuluLWQ	\1,\2
	Swap	\1
	Swap	\2
	Move.w	\2,\1
	Clr.w	\2
	Add.l	a1,\2
	Bcc.s	.NoOverFlow\@
	Addq.l	#1,\1
.NoOverFlow\@	Add.l	a0,\1
	Movem.l	(sp)+,a0-a1
	ENDM


	CNOP	0,4
_MULU64	; IN: d0.l = 32-bit number
	; IN: d1.l = 32-bit number
	; IN: a0.l = Ptr to 4 bytes for low 32 bits
	;OUT: d0.l = High 32 bits

;	Mulu.l	d0,d0:d1	68020+ instruction (not allowed on 060!)

	MuluLLQ	d0,d1
	Move.l	d1,(a0)
	Rts



	CNOP	0,4
_STACKSWAP	Move.l	(a7),d7

	Move.l	4.w,a6
	Suba.l	a1,a1
	Jsr	-294(a6)	Exec - FindTask
	Move.l	d0,a0

	Move.l	TC_SPUPPER(a0),d0
	Sub.l	TC_SPLOWER(a0),d0
	Cmp.l	#4000,d0
	Bge.s	.KeepStack

	Move.l	#StackSwapStruct_SIZEOF+4096,d0
	Moveq	#0,d1
	Jsr	-198(a6)	Exec - AllocMem
	Tst.l	d0
	Beq.s	.Exit
	Move.l	d0,a0

	Lea	StackSwapStruct_SIZEOF(a0),a1
	Move.l	a1,stk_Lower(a0)
	Add.w	#4080,a1
	Move.l	a1,stk_Pointer(a0)
	Addq.l	#8,a1
	Move.l	a1,stk_Upper(a0)
	Jsr	-732(a6)	Exec - StackSwap
	Move.l	d7,(a7)

.KeepStack	Moveq	#-1,d0
	Rts
.Exit	Moveq	#0,d0
	Rts



	CNOP	0,4
_RANDOM	; IN: d0.l = Seed
	;OUT: d0.l = New seed
	Movem.l	d1-d2,-(sp)

	Moveq	#0,d1
	Move.w	d0,d1
	Clr.w	d0
	Swap	d0

	Move.l	d0,d2
	Lsl.l	#8,d0
	Add.l	d2,d0

	Move.l	d1,d2
	Lsl.l	#8,d1
	Add.l	d2,d1

	Move.l	d0,d2
	Swap	d2
	And.l	#$7fff0000,d2
	Add.l	d2,d1
	Bvc.s	.Skip
	Addq.l	#1,d1

.Skip	Moveq	#15,d2
	Lsr.l	d2,d0
	Add.l	d1,d0
	Bclr	#31,d0

	Movem.l	(sp)+,d1-d2
	Rts


	CNOP	0,4
_CALCCHECKSUM	; IN: d0.l = Blocksize in bytes (must be a multiple of 16!)
	; IN: a0.l = ULONG *
	;OUT: d0.l = Checksum

	Move.l	d0,d1
	Lsr.l	#4,d1
	Subq.l	#1,d1
	Moveq	#1,d0

.Loop	Add.l	(a0)+,d0
	Add.l	(a0)+,d0
	Add.l	(a0)+,d0
	Add.l	(a0)+,d0
	Dbra	d1,.Loop

	Rts



	CNOP	0,4
_COMPRESSFROMZERO	; IN: a0.l UWORD *new
	; IN: a1.l UBYTE *dest
	; IN: d0.w bytes_block
	;OUT: d0.l Compressed length

	Movem.l	d2-d3/a2-a4,-(sp)

	Move.l	a1,a4
	Lea	(a0,d0.w),a2
	Moveq	#-1,d2

.Loop	Move.l	a2,d1
	Sub.l	a0,d1	d1 = 2
	Lsr.l	#1,d1	d1 = 1
	Subq.l	#1,d1	d1 = 0

	Cmp.w	#64-1,d1
	Ble.s	.Continue
	Moveq	#64-1,d1

.Continue	Move.w	d1,d3
	Subq.w	#1,d3
	Move.w	(a0),d0
	Bne.s	.NotClear

.LoopClr	Tst.w	(a0)+
	Dbne	d1,.LoopClr

	; Above loop will be left when (a3) wasn't zero or when d1 reaches -1.  If the
	; loop is left because (a3) wasn't zero then d1 won't be decreased by one.

	; d1 = 62 -> 1 zero word found.
	; d1 = 0  -> 63 zero words found.
	; d1 = -1 -> 64 zero words found.

	Tst.b	d1
	Blt.s	.Skip
	Subq.l	#2,a0

.Skip	Sub.b	d1,d3
	Move.b	d3,(a1)+
	Bra.s	.DoLoop

.NotClear	Cmp.w	d2,d0
	Bne.s	.NotSet

.LoopSet	Cmp.w	(a0)+,d2
	Dbne	d1,.LoopSet

	Tst.b	d1
	Blt.s	.Skip2
	Subq.l	#2,a0

.Skip2	Sub.b	d1,d3
	Or.b	#$80,d3
	Move.b	d3,(a1)+
	Bra.s	.DoLoop

.NotSet	Move.l	a1,a3
	Addq.l	#1,a1
;	Move.w	d0,(a1)+

.LoopAny	Move.w	(a0)+,d0
	Beq.s	.Stop
	Cmp.w	d2,d0
	Beq.s	.Stop
	Move.w	d0,(a1)+
	Dbra	d1,.LoopAny

	Bra.s	.Skip3

.Stop	Subq.l	#2,a0
.Skip3	Sub.b	d1,d3
	Or.b	#$C0,d3
	Move.b	d3,(a3)

.DoLoop	Cmp.l	a2,a0
	Blt.s	.Loop

	Move.l	a1,d0
	Sub.l	a4,d0

	Movem.l	(sp)+,d2-d3/a2-a4
	Rts




	CNOP	0,4
_COMPRESS	; IN: a0.l UWORD *new
	; IN: a1.l UWORD *old
	; IN: a6.l UBYTE *dest
	; IN: d0.w bytes_block
	;OUT: d0.l Compressed length

	Movem.l	d2-d4/a2-a4/a6,-(sp)

	Lea	(a0,d0.w),a2
	Moveq	#-1,d2

.Loop	Move.l	a2,d1
	Sub.l	a0,d1	d1 = 2
	Lsr.l	#1,d1	d1 = 1
	Subq.l	#1,d1	d1 = 0

	Cmp.w	#64-1,d1
	Ble.s	.Continue
	Moveq	#64-1,d1

.Continue	Move.w	d1,d3
	Subq.b	#1,d3
	Move.w	d1,d4
	Move.l	a0,a3
	Move.l	a1,a4

.LoopUnchanged	Cmpm.w	(a3)+,(a4)+
	Dbne	d4,.LoopUnchanged

	Move.w	(a0),d0
	Bne.s	.NotClear

.LoopClr	Tst.w	(a0)+
	Dbne	d1,.LoopClr

	; Above loop will be left when (a3) wasn't zero or when d1 reaches -1.  If the
	; loop is left because (a3) wasn't zero then d1 won't be decreased by one.

	; d1 = 62 -> 1 zero word found.
	; d1 = 0  -> 63 zero words found.
	; d1 = -1 -> 64 zero words found.

	Cmp.w	d1,d4	0,3 -> 3-0 = 3
	Blt.s	.Unchanged

	Tst.b	d1
	Blt.s	.Skip
	Subq.l	#2,a0

.Skip	Sub.b	d1,d3
	Lea	2(a1,d3.w*2),a1
	Move.b	d3,(a6)+
	Bra.s	.DoLoop


.NotClear	Cmp.w	d2,d0
	Bne.s	.NotSet

.LoopSet	Cmp.w	(a0)+,d2
	Dbne	d1,.LoopSet

	Cmp.w	d1,d4
	Blt.s	.Unchanged

	Tst.b	d1
	Blt.s	.Skip2
	Subq.l	#2,a0

.Skip2	Sub.b	d1,d3
	Lea	2(a1,d3.w*2),a1
	Or.b	#$80,d3
	Move.b	d3,(a6)+
	Bra.s	.DoLoop


.Unchanged	Tst.b	d4
	Blt.s	.Skip4
	Subq.l	#2,a3
	Subq.l	#2,a4

.Skip4	Sub.b	d4,d3
	Or.b	#$40,d3
	Move.b	d3,(a6)+
	Move.l	a3,a0
	Move.l	a4,a1
	Bra.s	.DoLoop


.NotSet	Cmp.b	d4,d3
	Bge.s	.Unchanged

	Move.l	a6,a3
	Addq.l	#1,a6

.LoopAny	Move.w	(a0)+,d0
	Beq.s	.Stop
	Cmp.w	d2,d0
	Beq.s	.Stop
	Cmp.w	(a1)+,d0
	Beq.s	.Stop2
	Move.w	d0,(a6)+
	Dbra	d1,.LoopAny

	Bra.s	.Skip3

.Stop2	Subq.l	#2,a1
.Stop	Subq.l	#2,a0
.Skip3	Sub.b	d1,d3
	;Lea	2(a1,d3.w*2),a1
	Or.b	#$C0,d3
	Move.b	d3,(a3)

.DoLoop	Cmp.l	a2,a0
	Blt	.Loop

	Move.l	a6,d0

	Movem.l	(sp)+,d2-d4/a2-a4/a6
	Sub.l	a6,d0
	Rts
