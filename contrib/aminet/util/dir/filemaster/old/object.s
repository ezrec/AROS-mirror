;/*
;     Filemaster - Multitasking directory utility.
;     Copyright (C) 2000  Toni Wilen
;     
;     This program is free software; you can redistribute it and/or
;     modify it under the terms of the GNU General Public License
;     as published by the Free Software Foundation; either version 2
;     of the License, or (at your option) any later version.
;     
;     This program is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
;     
;     You should have received a copy of the GNU General Public License
;     along with this program; if not, write to the Free Software
;     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;*/

;----------T----------T

	xdef	_RealTimeScroll
	xdef	_unpack
	xdef	_unpacknp
	xdef	_hexconvert
	xdef	_checksum
	xdef	_bootsum
	xdef	_formathook

	xdef	_mousebutton

	xdef	_initaudio
	xdef	_endaudio
	xdef	_startaudio
	xdef	_waitaudio

	xdef	_regform
	xdef	_fmexe
	xdef	_fmexee

	xdef	_rlepack
	xdef	_rleunpack

	section	text,code

_mousebutton
	move.l	a6,-(sp)
	move.l	a0,a6
mloop	jsr	-$010e(a6)
	btst	#6,$bfe001
	bne.s	mloop
	move.l	(sp)+,a6
	rts

	;a0=src,a1=bitmap,a2=(WORD*)row,d0=totrows,d1=scrbytes,d2=width,d3=mask,a6=shows

_unpack	movem.l	d2-d7/a2-a6,-(sp)	;packed iff
	ext.l	d2

nxrow	move.l	(a0),a4
	moveq	#0,d7
	move.b	5(a1),d7
	subq.w	#1,d7
	lea	8(a1),a5
nxplane	move.l	(a5)+,a3
	moveq	#0,d6
nxbyte	moveq	#0,d5
	subq.l	#1,d1
	bmi.s	uperr
	move.b	(a4)+,d5
	bmi.s	nega
	add.w	d5,d6
	addq.w	#1,d6
ident	move.b	(a4)+,(a3)+
	subq.l	#1,d1
	bmi.s	uperr
	dbf	d5,ident
	bra.s	nxblk
nega	move.b	(a4)+,d4
	subq.l	#1,d1
	bmi.s	uperr
	neg.b	d5
	add.w	d5,d6
	addq.w	#1,d6
dupli	move.b	d4,(a3)+
	dbf	d5,dupli
nxblk	cmp.w	d2,d6
	bcs.s	nxbyte
	dbf	d7,nxplane
	cmp.b	#1,d3
	bne.s	nomask
	bsr.s	mask
	bmi.s	uperr
nomask
	bsr.s	sysput

	addq.w	#1,(a2)
	move.l	a4,(a0)
	cmp.w	(a2),d0
	bne.s	nxrow

uperr	movem.l	(sp)+,d2-d7/a2-a6
	rts

	;a6=shows,d2=width

sysput	movem.l	d0-d7/a0-a6,-(sp)
	move.l	a6,a5			;shows
	move.l	22(a5),a6		;gfxbase
	lsl.w	#3,d2

	move.l	26(a5),a0	;srcbitmap
	moveq	#0,d0		;srcx
	moveq	#0,d1		;srcy
	move.l	10(a5),a1	;dstbitmap
	move.w	d2,d4		;sizex
	subq.w	#1,d4
	moveq	#1,d5		;sizey
	moveq	#0,d2		;dstx
	move.w	(a2),d3		;dsty
	move.b	#$c0,d6		;minterm
	st	d7		;mask
	sub.l	a2,a2
	jsr	-$01e(a6)

	movem.l	(sp)+,d0-d7/a0-a6
	rts


mask	moveq	#0,d6
nxbytem	subq.l	#1,d1
	bmi.s	maskerr
	moveq	#0,d5
	move.b	(a4)+,d5
	bmi.s	negam
	addq.w	#1,d5
	add.w	d5,d6
	sub.l	d5,d1
	bmi.s	maskerr
	add.w	d5,a4
	bra.s	nxblkm
negam	addq.l	#1,a4
	subq.l	#1,d1
	bmi.s	maskerr
	neg.b	d5
	addq.w	#1,d5
	add.w	d5,d6
nxblkm	cmp.w	d2,d6
	bcs.s	nxbytem
	moveq	#1,d5
maskerr	rts

_unpacknp	movem.l	d2-d7/a2-a6,-(sp)	;unpacked iff
	ext.l	d2

nnxrow	move.l	(a0),a4
	moveq	#0,d7
	move.b	5(a1),d7
	subq.w	#1,d7
	lea	8(a1),a5
nnxplane
	move.l	(a5)+,a3
nnxbyte	move.w	d2,d6
nnxbytec
	move.b	(a4)+,(a3)+
	subq.l	#1,d1
	bmi.s	nuperr;
	subq.w	#1,d6
	bne.s	nnxbytec
	dbf	d7,nnxplane
	cmp.b	#1,d3
	bne.s	nnomask

	move.w	d2,d6
nnxbytem
	addq.l	#1,a4
	subq.l	#1,d1
	bmi.s	nuperr;
	subq.w	#1,d6
	bne.s	nnxbytem

nnomask
	bsr.w	sysput

	addq.w	#1,(a2)
	move.l	a4,(a0)
	cmp.w	(a2),d0
	bne.s	nnxrow

nuperr	movem.l	(sp)+,d2-d7/a2-a6
	rts

	;a0=reqscroll

_RealTimeScroll
l3C00	MOVEM.L	D0-D7/A0-A6,-(SP)
	MOVE.L	A0,A3
	MOVE.L	(A3),D0
	MOVE.W	8(A3),D1
	EXT.L	D1
	BSR.W	l3B94
	MOVE.L	D0,D7
l3C14	MOVE.L	D7,D6
	MOVE.L	4(A3),D0
	SUB.L	10(A3),D0
	BLE.W	l3D42
	MOVE.L	14(A3),A0
	MOVE.L	$22(A0),A0
	MOVEQ	#0,D1
	move.w	2(a0),d1
	tst.w	48(a3)
	bne.s	jaska
	MOVE.W	4(A0),D1
jaska	BSR.W	l3BCE
	MOVE.L	D0,D2
	MOVE.W	8(A3),D1
	EXT.L	D1
	BSR.W	l3B94
	MOVE.L	D7,D1
	SUB.L	D0,D1
	MOVE.L	D1,D3
	BEQ.W	l3D42
	NEG.L	D1
	BMI.S	l3C50
	ADDQ.L	#8,D1
l3C50	SUBQ.L	#4,D1
	MOVE.L	D1,D0
	BGE.S	l3C58
	NEG.L	D0
l3C58	MOVE.L	D1,D5
	MOVE.W	#5,D1
	BSR.W	l3BE0
	MOVE.L	D0,D1
	TST.L	D5
	BGE.S	l3C6A
	NEG.L	D1
l3C6A	MOVE.W	8(A3),D4
	MULU	12(A3),D4
	MOVE.L	D4,D5
	LSR.L	#2,D5
	SUB.L	D5,D4
	MOVE.W	8(A3),D5
	EXT.L	D5
	SUB.L	D5,D4
	CMP.L	D4,D1
	BGT.S	l3C8A
	NEG.L	D1
	CMP.L	D4,D1
	BLE.S	l3CA6
l3C8A	MOVE.L	D2,(A3)
	SUB.L	D3,D7
	MOVE.L	$12(A3),A0
	MOVEM.L	D0-D7/A0-A6,-(SP)
	EXG	A0,A3
	MOVE.L	A0,-(SP)
	JSR	(A3)
	ADDQ.W	#4,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
	BRA.W	l3C14

l3CA6	NEG.L	D1
	ADD.L	D1,D7
	MOVE.L	D1,-(SP)
	MOVE.L	D7,D2
	MOVE.L	D6,D3
	TST.L	(SP)
	BPL.S	l3CC0
	MOVE.W	8(A3),D1
	SUBQ.W	#1,D1
	EXT.L	D1
	ADD.L	D1,D2
	ADD.L	D1,D3
l3CC0	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,D1
	MOVE.L	D0,-(SP)
	MOVE.W	8(A3),D0
	EXT.L	D0
	BSR.W	l3B94
	MOVE.L	D0,D1
	MOVE.L	(SP)+,D0
	SUB.L	D7,D1
	MOVEM.L	D0/D1,-(SP)
	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,D2
	MOVE.L	D3,D0
	BSR.W	l3BE0
	MOVE.L	D0,D3
	MOVEM.L	(SP)+,D0/D1
	SUB.L	D2,D3
	BGE.S	l3D20
	NEG.L	D3
	MOVE.L	10(A3),D2
	SUB.L	D3,D2
	ADD.L	D2,D0
	MOVEM.L	D0/D1,-(SP)
	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	EXT.L	D1
	BSR.B	l3B94
	MOVE.L	D0,D2
	MOVEM.L	(SP)+,D0/D1
	ADD.L	D2,D1
l3D20	MOVE.L	(SP)+,D2
	MOVEM.L	D0-D7/A0-A6,-(SP)
	;MOVE.L	$1A(A3),-(SP)
	move.l	a3,-(sp)
	MOVEM.L	D0-D3,-(SP)
	MOVE.L	$1A(A3),A0
	EXG	A0,A3
	JSR	(A3)
	ADD.W	#$14,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
	BRA.W	l3C14

l3D42	MOVE.L	D7,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,(A3)
	MOVE.L	$16(A3),A0
	MOVE.L	A0,D2
	BEQ.S	l3D66
	MOVEM.L	D0-D7/A0-A6,-(SP)
	EXG	A0,A3
	MOVE.L	A0,-(SP)
	JSR	(A3)
	ADDQ.W	#4,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
l3D66	MOVE.L	A6,-(SP)
	move.l	40(a3),a6
	JSR	-$10E(A6)
	MOVE.L	(SP)+,A6
	MOVE.L	14(A3),A0
	TST.B	13(A0)
	BMI.W	l3C14
	MOVEM.L	(SP)+,D0-D7/A0-A6
	RTS

l3B94	MOVEM.L	D2-D7/A0-A6,-(SP)
	MOVE.W	D1,D2
	MULU	D0,D2
	MOVE.L	D0,D3
	SWAP	D3
	MOVE.L	D1,D4
	SWAP	D4
	MULU	D4,D0
	MULU	D3,D1
	MULU	D4,D3
	SWAP	D0
	SWAP	D1
	MOVE.L	D3,A0
	ADD.W	D0,A0
	ADD.W	D1,A0
	CLR.W	D0
	CLR.W	D1
	ADD.L	D0,D2
	BCC.S	l3BBE
	ADDQ.L	#1,A0
l3BBE	ADD.L	D1,D2
	BCC.S	l3BC4
	ADDQ.L	#1,A0
l3BC4	MOVE.L	A0,D1
	MOVE.L	D2,D0
	MOVEM.L	(SP)+,D2-D7/A0-A6
	RTS

l3BCE	BSR.B	l3B94
	MOVE.W	D0,-(SP)
	MOVE.W	D1,D0
	SWAP	D0
	TST.W	(SP)+
	BPL.S	l3BDE
	ADDQ.L	#1,D0
l3BDE	RTS

l3BE0	MOVEM.L	D1-D7/A0-A6,-(SP)
	MOVE.L	D0,D2
	CLR.W	D2
	SWAP	D2
	DIVU	D1,D2
	SWAP	D0
	SWAP	D2
	MOVE.W	D2,D0
	SWAP	D0
	DIVU	D1,D0
	MOVE.W	D0,D2
	MOVE.L	D2,D0
	MOVEM.L	(SP)+,D1-D7/A0-A6
	RTS

	;a0=src,a1=dst,d0=len,d1=width

_hexconvert
	movem.l	d2-d3/a2,-(sp)

	move.l	a1,a2
	move.w	d1,d3
	subq.w	#1,d3
hc5	move.b	#32,(a2)+
	move.b	#32,(a2)+
	move.b	#32,(a2)+
	dbf	d3,hc5
	move.b	#32,(a2)

	move.w	d1,d3
	add.w	d1,d3
	add.w	d1,d3
	addq.w	#1,d3

	subq.w	#1,d0
	add.w	d1,d1
	lea	1(a1,d1.w),a2
hc3	move.b	(a0)+,d1
	move.b	d1,d2
	cmp.b	#32,d1
	bcc.s	hc4
	moveq	#'.',d2
hc4	move.b	d2,(a2)+
	move.b	d1,d2
	lsr.b	#4,d1
	and.b	#$0f,d1
	cmp.b	#10,d1
	bcs.s	hc1
	addq.b	#7,d1
hc1	add.b	#'0',d1
	move.b	d1,(a1)+
	and.b	#$0f,d2
	cmp.b	#10,d2
	bcs.s	hc2
	addq.b	#7,d2
hc2	add.b	#'0',d2
	move.b	d2,(a1)+
	dbf	d0,hc3
	move.w	d3,d0
	movem.l	(sp)+,d2-d3/a2
	rts

_checksum
	move.l	d0,d1
	lsr.l	#2,d1
	moveq	#0,d0
checks2	add.l	(a0)+,d0
checks1	dbf	d1,checks2
	rts

_bootsum
	moveq	#0,d0
	move.l	a0,a1
	move.w	#1024/4-1,d1
boots2	add.l	(a0)+,d0
	bcc.s	boots1
	addq.l	#1,d0
boots1	dbf	d1,boots2
	move.l	d0,4(a1)
	rts

custom	dc.l	$dff000

_initaudio	move.l	custom(pc),a1
	move.w	#%0000000000001111,$96(a1)
	move.w	#%0000011110000000,$9a(a1)
	move.w	#%0000011110000000,$9c(a1)
	move.w	#%0000000011111111,$9e(a1)
	lsr.l	#1,d0
	move.l	a0,$a0(a1)
	move.w	d0,$a4(a1)
	move.w	#64,$a8(a1)
	move.w	d1,$a6(a1)
	move.l	a0,$b0(a1)
	move.w	d0,$b4(a1)
	move.w	#64,$b8(a1)
	move.w	d1,$b6(a1)
	move.w	#%1000000000000011,$96(a1)
	rts
_endaudio	move.l	custom(pc),a1
	move.w	#%0000000000001111,$96(a1)
	move.w	#%0000000110000000,$9c(a1)
	rts
_startaudio
	move.l	custom(pc),a1
	move.w	#%0000000110000000,$9c(a1)
	lsr.l	#1,d0
	move.l	a0,$a0(a1)
	move.w	d0,$a4(a1)
	move.l	a0,$b0(a1)
	move.w	d0,$b4(a1)
	rts
_waitaudio	move.l	a6,-(sp)
	move.l	a0,a6
wa2	move.l	custom(pc),a1
	move.w	$1e(a1),d0
	and.w	#%0000000110000000,d0
	bne.s	wa1
	moveq	#10,d1
	jsr	-$00c6(a6)
	bra.s	wa2
wa1	move.l	(sp)+,a6
	rts


_formathook
	move.l	a1,d0
	move.l	16(a0),a1
	move.b	d0,(a1)
	addq.l	#1,16(a0)
	rts


	;a0=source,a1=dest,d0=len
_rlepack
	move.l	a2,-(sp)
	link	a5,#-12
	ext.l	d0
	move.l	d0,-(sp)
	lea	-4(a5),a2
	move.l	a1,(a2)
	move.l	a2,-(sp)
	lea	-8(a5),a2
	move.l	a0,(a2)
	move.l	a2,-(sp)
	bsr.w	packrow
	lea	12(sp),sp
	unlk	a5
	move.l	(sp)+,a2
	rts

PT    equr  a0                ;-> beginning of replicate run (if any) 
IX    equr  a1                ;-> end+1 of input line 
IP    equr  a2                ;-> beginning of literal run (if any) 
IQ    equr  a3                ;-> end+1 of lit and/or rep run (if any) 
OP    equr  a4                ;-> end+1 of output line current pos 
FP    equr  a6                ;frame pointer 
;SP    equr  a7               ; stack pointer 
RT    equr  d0                ;return value 
MX    equr  d1                ;check for maximum run = MAX 
AM    equr  d2                ;amount 
CH    equr  d3                ;character 
REGS  reg   AM/CH/IP/IQ/OP 
FRM   equ   8                 ;input line address 
TOO   equ   12                ;output line address 
AMT   equ   16                ;length of input line 
MAX   equ   128               ;maximum encodable output run 
CHECK equ   1                 ;turns on maximum row checking 
 
 
packrow 
 
 
CAS0	link     FP,#0 
	movem.l  REGS,-(SP) 
	movea.l  FRM(FP),IP 
	movea.l  (IP),IP	  ;IP = *from 
	movea.l  IP,IQ	  ;IQ = IP 
	movea.l  IQ,IX 
	adda.l   AMT(FP),IX    ;IX = IP + amt 
	movea.l  TOO(FP),OP 
	movea.l  (OP),OP	  ;OP = *too 
CAS1	movea.l  IQ,PT	  ;adjust PT (no replicates yet!) 
	move.b   (IQ)+,CH	  ;grab character 
	cmpa.l   IQ,IX	  ;if input is finished 
	beq.s    CAS5	  ;branch to case 5 
	ifd	CHECK 
	move.l   IQ,MX 
	sub.l    IP,MX 
	cmpi     #MAX,MX	  ;if run has reached MAX 
	beq.s    CAS6	  ;branch to case 6 
	endc 
	cmp.b    (IQ),CH	  ;if next character != CH 
	bne.s    CAS1	  ;stay in case 1 
CAS2	move.b   (IQ)+,CH	  ;grab character 
	cmpa.l   IQ,IX	  ;if input is finished 
	beq.s    CAS7	  ;branch to case 7 
	ifd	CHECK 
	move.l   IQ,MX 
	sub.l    IP,MX 
	cmpi     #MAX,MX	  ;if run has reached MAX 
	beq.s    CAS6	  ;branch to case 6 
	endc 
	cmp.b    (IQ),CH	  ;if next character != CH 
	bne.s    CAS1	  ;branch to case 1 
CAS3	move.b   (IQ)+,CH	  ;grab character 
	cmpa.l   IQ,IX	   ; if input is finished 
	beq.s    CAS7		;  branch to case 7 
	ifd	CHECK 
	move.l   IQ,MX 
	sub.l    PT,MX 
	cmpi     #MAX,MX	  ;if run has reached MAX 
	beq.s    CAS4		;  branch to case 4 
	endc 
	cmp.b    (IQ),CH	  ;if next character = CH 
	beq.s    CAS3		;  stay in case 3 
CAS4	move.l   PT,AM 
	sub.l    IP,AM	    ;AM = PT - IP 
	beq.s    C41		   ;branch to replicate run 
	subq     #1,AM	    ;AM = AM - 1 
	move.b   AM,(OP)+	 ;output literal control byte 
C40	move.b   (IP)+,(OP)+    ;output literal run 
	dbra     AM,C40 
C41	move.l   PT,AM 
	sub.l    IQ,AM	    ;AM = PT - IQ (negative result!) 
	addq     #1,AM	    ;AM = AM + 1 
	move.b   AM,(OP)+	 ;output replicate control byte 
	move.b   CH,(OP)+	 ;output repeated character 
	movea.l  IQ,IP	  ;  reset IP 
	bra.s    CAS1	   ;  branch to case 1 (not done) 
CAS5	move.l   IQ,AM 
	sub.l    IP,AM	    ;AM = IQ - IP (positive result > 0) 
	subq     #1,AM	    ;AM = AM - 1 
	move.b   AM,(OP)+	 ;output literal control byte 
C50	move.b   (IP)+,(OP)+   ; output literal run 
	dbra     AM,C50 
	bra.s    CAS8	   ;  branch to case 8 (done) 
	ifd	CHECK 
CAS6	move.l   IQ,AM 
	sub.l    IP,AM	    ;AM = IQ - IP (positive result > 0) 
	subq     #1,AM	    ;AM = AM - 1 
	move.b   AM,(OP)+	 ;output literal control byte 
C60	move.b   (IP)+,(OP)+   ; output literal run 
	dbra     AM,C60 
	bra	CAS1	   ;  branch to case 1 (not done) 
	endc 
CAS7	move.l   PT,AM 
	sub.l    IP,AM	   ; AM = PT - IP (positive result > 0) 
	beq.s    C71		   ;branch to replicate run 
	subq     #1,AM	    ;AM = AM - 1 
	move.b   AM,(OP)+	 ;output literal control byte 
C70	move.b   (IP)+,(OP)+   ; output literal run 
	dbra     AM,C70 
C71	move.l   PT,AM 
	sub.l    IQ,AM	   ; AM = PT - IQ (negative result) 
	addq     #1,AM	   ; AM = AM + 1 
	move.b   AM,(OP)+	 ;output replicate control byte 
	move.b   CH,(OP)+	 ;output repeated character 
CAS8	movea.l  FRM(FP),PT   ;  PT = **from 
	move.l   IQ,(PT)	 ; *from = *from + amt 
	movea.l  TOO(FP),PT   ;  PT = **too 
	move.l   OP,RT 
	sub.l    (PT),RT	 ;return = OP - *too  
	move.l   OP,(PT)	 ;*too = *too + return 
	movem.l  (SP)+,REGS 
	UNLK     FP 
	rts 

	;a0-source,a1-dest,d0=len
 
_rleunpack
unpack	move.l	d2,-(sp)
ccnext	moveq	#0,d2
	move.b	(a0)+,d2
	bmi.s	ccnega
ccident	move.b	(a0)+,(a1)+
	subq.w	#1,d0
	beq.s	ccend
	dbf	d2,ccident
	bra.s	ccnext
ccnega	move.b	(a0)+,d1
	neg.b	d2
ccdupl	move.b	d1,(a1)+
	subq.w	#1,d0
	beq.s	ccend
	dbf	d2,ccdupl
	bra.s	ccnext
ccend	move.l	a0,d0
	move.l	(sp)+,d2
	rts


	section __MERGED,data

_regform	incbin	"reg.form"
_fmexe		incbin	"fmexe"
_fmexee
