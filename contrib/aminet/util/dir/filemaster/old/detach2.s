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



;V39 	SET	1
YEH 	SET	1


	section	_NOMERGE,code_c

	xref	LinkerDB
	xref	__BSSBAS
	xref	__BSSLEN

proge	sub.l	a3,a3
	sub.l	d5,d5
	moveq	#5,d7
	move.l	4.w,a6
	cmp.w	#3,d0
	bne.s	sleep01
	cmp.b	#'-',(a0)+
	bne.s	sleep01
	cmp.b	#'s',(a0)
	beq.s	sleep02
	cmp.b	#'S',(a0)
	bne.s	sleep01
sleep02	moveq	#4,d7
sleep01	lea	__BSSBAS,a0
	move.l	#__BSSLEN,d0
	subq.w	#1,d0
	moveq	#0,d1
clear	move.l	d1,(a0)+
	dbf	d0,clear
	sub.l	a1,a1
	jsr	-$0126(a6)
	move.l	d0,a4
	move.l	$ac(a4),d0
	bne.s	notwb1
	lea	$5c(a4),a0
	jsr	-$0180(a6)
	lea	$5c(a4),a0
	jsr	-$0174(a6)
	move.l	d0,a3

notwb1	move.l	4.w,a6

	lea	dos(pc),a1
	moveq	#0,d0
	jsr	-$0228(a6)
	move.l	d0,a5

	IFND V39
	cmp.w	#37,20(a6)
	ELSE
	cmp.w	#39,20(a6)
	ENDIF
	bcc.s	kickok
	lea	error1(pc),a0
	bsr.w	output
	bra.w	oldkick

kickok

	clr.l	-(sp)

	move.l	a3,d0
	beq.s	notwb3
	move.l	a5,a6
	move.l	36(a3),a2
	move.l	(a2),d1
	jsr	-$007e(a6)
	move.l	d0,d5
	move.l	4(a2),-(sp)
	move.l	#$80000000+1020,-(sp)	;NP_CommandName
notwb3
	moveq	#1,d0
	move.l	d7,-(sp)
	move.l	#$80000000+1013,-(sp)	;NP_Priority
	move.l	d0,-(sp)
	move.l	#$80000000+1002,-(sp)	;NP_FreeSeglist
	move.l	d0,-(sp)
	move.l	#$80000000+1018,-(sp)	;NP_Cli
	move.l	#20000,-(sp)
	move.l	#$80000000+1011,-(sp)	;NP_StackSize
	lea	proge-4(pc),a0
	move.l	(a0),d0
	clr.l	(a0)
	IFD	YEH
	move.l	d0,-(sp)
	lsl.l	#2,d0
	move.l	d0,a0		;a0=pointer to data section
	move.l	(a0),d0
	clr.l	(a0)		;clear data section next pointer
	move.l	d0,d1
	lsl.l	#2,d1		;d1=pointer to code section
	move.l	d1,a1		;put data section pointer to code section next pointer
	move.l	(sp)+,(a1)
	ENDIF
	move.l	d0,-(sp)
	move.l	#$80000000+1001,-(sp)	;NP_Seglist
	move.l	sp,d1
	move.l	a5,a6
	jsr	-$01f2(a6)
	lea	11*4(sp),sp
	move.l	a3,d0
	beq.s	notwb5
	addq.l	#2*4,sp
notwb5

endwb
oldkick	move.l	a3,d0
	beq.s	notwb4
	move.l	a5,a6
	move.l	d5,d1
	jsr	-$007e(a6)
	move.l	a5,a1
	move.l	4.w,a6
	jsr	-$019e(a6)
	jsr	-$0084(a6)
	move.l	a3,a1
	jsr	-$017a(a6)
	bra.s	notwb2
notwb4	move.l	4.w,a6
	move.l	a5,a1
	jsr	-$019e(a6)
notwb2	moveq	#0,d0
	rts

output	movem.l	d0-d7/a0-a4/a6,-(sp)
	move.l	a0,a4
	move.l	a5,a6
	lea	window(pc),a0
	move.l	a0,d1
	move.l	#1005,d2
	jsr	-$001e(a6)
	move.l	d0,d7
	beq.s	output2
	move.l	d7,d1
	move.l	a4,d2
	moveq	#-1,d3
output1	addq.l	#1,d3
	tst.b	(a4)+
	bne.s	output1
	jsr	-$0030(a6)
	move.l	#150,d1
	jsr	-$00c6(a6)
	move.l	d7,d1
	jsr	-$0024(a6)
output2	movem.l	(sp)+,d0-d7/a0-a4/a6
	rts

dos	dc.b	"dos.library",0
window	dc.b	"CON:20/100/600/50/FM Fatal error",0
	IFND V39
error1
 dc.b 10,"  Kickstart 2.0 or better required",10,0
	ELSE
error1
 dc.b 10,"  Kickstart 3.0 and CPU 68020 or better required",10,0
	ENDIF

