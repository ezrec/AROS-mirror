
	xdef	_rawdo
	

_rawdo	movem.l	d2-d6/a2-a5,-(sp)
	link	a6,#-$10
	move.l	a1,-(sp)
	move.l	a0,a4
	bra	LC0DE
LC010	bsr	LC228
	move.l	(sp),d0
	unlk	a6
	movem.l	(sp)+,d2-d6/a2-a5
	rts
LC01E	moveq	#0,d0
	moveq	#0,d2
LC022	move.b	(a4)+,d2
	cmp.b	#$30,d2
	bcs.s	LC042
	cmp.b	#$39,d2
	bhi.s	LC042
	add.l	d0,d0
	move.l	d0,d1
	add.l	d0,d0
	add.l	d0,d0
	add.l	d1,d0
	sub.b	#$30,d2
	add.l	d2,d0
	bra.s	LC022
LC042	subq.l	#1,a4
	rts
LC046	tst.l	d4
	bpl.s	LC050
	move.b	#$2D,(a5)+
	neg.l	d4
LC050	moveq	#$30,d0
	lea	LL076(pc),a0
LC056	move.l	(a0)+,d1
	beq.s	LC06E
	moveq	#$2F,d2
LC05C	addq.l	#1,d2
	sub.l	d1,d4
	bcc.s	LC05C
	add.l	d1,d4
	cmp.l	d0,d2
	beq.s	LC056
	moveq	#0,d0
	move.b	d2,(a5)+
	bra.s	LC056
LC06E	moveq	#$30,d0
	add.b	d0,d4
	move.b	d4,(a5)+
	rts
LL076	dc.l	$3B9ACA00
	dc.l	$5F5E100
	dc.l	$989680
	dc.l	$F4240
	dc.l	$186A0
	dc.l	$2710
	dc.l	$3E8
	dc.l	$64
	dc.l	10
	dc.l	0
LC09E	tst.l	d4
	beq.s	LC06E
	clr.w	d1
	btst	#2,d3
	bne.s	LC0B0
	moveq	#3,d2
	swap	d4
	bra.s	LC0B2
LC0B0	moveq	#7,d2
LC0B2	rol.l	#4,d4
	move.b	d4,d0
	and.b	#15,d0
	bne.s	LC0C0
	tst.w	d1
	beq.s	LC0D4
LC0C0	moveq	#-$1,d1
	cmp.b	#9,d0
	bhi.s	LC0CE
	add.b	#$30,d0
	bra.s	LC0D2
LC0CE	add.b	#$37,d0
LC0D2	move.b	d0,(a5)+
LC0D4	dbra	d2,LC0B2
	rts
LC0DA	bsr	LC228
LC0DE	move.b	(a4)+,d0
	beq	LC010
	cmp.b	#$25,d0
	bne.s	LC0DA
	lea	-$10(a6),a5
	clr.w	d3
	cmp.b	#$2D,(a4)
	bne.s	LC0FC
	bset	#0,d3
	addq.l	#1,a4
LC0FC	cmp.b	#$30,(a4)
	bne.s	LC106
	bset	#1,d3
LC106	bsr	LC01E
	move.w	d0,d6
	moveq	#0,d5
	cmp.b	#$2E,(a4)
	bne.s	LC11C
	addq.l	#1,a4
	bsr	LC01E
	move.w	d0,d5
LC11C	cmp.b	#$6C,(a4)
	bne.s	LC128
	bset	#2,d3
	addq.l	#1,a4
LC128	move.b	(a4)+,d0
	cmp.b	#$75,d0		;!
	beq.s	LC136
	cmp.b	#$64,d0
	beq.s	LC136
	cmp.b	#$55,d0		;!
	beq.s	LC136
	cmp.b	#$44,d0
	bne.s	LC140
LC136	bsr.s	LC154
	bsr	LC046
	bra	LC1C2
LC140	cmp.b	#$78,d0
	beq.s	LC14C
	cmp.b	#$58,d0
	bne.s	LC176
LC14C	bsr.s	LC154
	bsr	LC09E
	bra.s	LC1C2
LC154	btst	#2,d3
	bne.s	LC168
	move.l	4(sp),a1
	move.w	(a1)+,d4
	move.l	a1,4(sp)
	ext.l	d4
	rts
LC168	move.l	4(sp),a1
	move.l	(a1)+,d4
	move.l	a1,4(sp)
	tst.l	d4
	rts
LC176	cmp.b	#$73,d0
	bne.s	LC184
	bsr.s	LC168
	beq.s	LC1FE
	move.l	d4,a5
	bra.s	LC1C8
LC184	cmp.b	#$62,d0
	bne.s	LC1A2
	bsr.s	LC168
	beq.s	LC1FE
	lsl.l	#2,d4
	move.l	d4,a5
	moveq	#0,d2
	move.b	(a5)+,d2
	beq.s	LC1FE
	tst.b	-1(a5,d2.w)
	bne.s	LC1D4
	subq.w	#1,d2
	bra.s	LC1D4
LC1A2	cmp.b	#$75,d0
	beq.s	LC1AE
	cmp.b	#$55,d0
	bne.s	LC1B6
LC1AE	bsr.s	LC154
	bsr	LC050
	bra.s	LC1C2
LC1B6	cmp.b	#$63,d0
	bne	LC0DA
	bsr.s	LC154
	move.b	d4,(a5)+
LC1C2	clr.b	(a5)
	lea	-$10(a6),a5
LC1C8	move.l	a5,a0
	moveq	#-$1,d2
LC1CC	tst.b	(a0)+
	dbeq	d2,LC1CC
	not.l	d2
LC1D4	tst.w	d5
	beq.s	LC1DC
	cmp.w	d5,d2
	bhi.s	LC1DE
LC1DC	move.w	d2,d5
LC1DE	sub.w	d5,d6
	bpl.s	LC1E4
	clr.w	d6
LC1E4	btst	#0,d3
	bne.s	LC1F2
	bsr.s	LC202
	bra.s	LC1F2
LC1EE	move.b	(a5)+,d0
	bsr.s	LC228
LC1F2	dbra	d5,LC1EE
	btst	#0,d3
	beq.s	LC1FE
	bsr.s	LC202
LC1FE	bra	LC0DE
LC202	move.b	#$20,d2
	btst	#1,d3
	beq.s	LC222
	cmp.b	#$2D,(a5)
	bne.s	LC218
	move.b	(a5)+,d0
	subq.w	#1,d5
	bsr.s	LC228
LC218	move.b	#$30,d2
	bra.s	LC222
LC21E	move.b	d2,d0
	bsr.s	LC228
LC222	dbra	d6,LC21E
	rts
LC228	move.b	d0,(a3)+
	rts
