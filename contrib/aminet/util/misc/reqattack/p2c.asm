;// breite in pixel

Quelle = (6*4)+4+0
Ziel	 = (6*4)+4+4
Breite = (6*4)+4+8

 IFD GCC
Planes = (6*4)+4+12
 ELSE
Planes = (6*4)+4+10
 ENDC
	
	XDEF _Planar2Chunky

_Planar2Chunky:
	movem.l	d2-d7,-(sp)
	
 IFD GCC
	move.l	Breite(sp),d0
 ELSE
	move		Breite(sp),d0
 ENDC

	add		#15,d0
	and		#$FFF0,d0
	moveq		#0,d1
	moveq		#0,d2
	move		d0,d1
	move		d0,d2
	lsr		#3,d1				;// d1=Modulo in Bytes
	lsr		#4,d0				;// d0=Counter in Words
	move.l	Quelle(sp),a0
	move.l	Ziel(sp),a1
	
 IFD GCC
	move.l	Planes(sp),d7
 ELSE
 	move		Planes(sp),d7
 ENDC

	lsr		#1,d2
	subq		#1,d2
	moveq		#0,d3				;// d3 = immer 0

.ClrLoop:
	clr		(a1)+
	dbf		d2,.ClrLoop

	move.l	Ziel(sp),a1

 IFD GCC
	move.l	Planes(sp),d2
 ELSE
 	move		Planes(sp),d2
 ENDC

	subq		#1,d2
	mulu		d1,d2
	lea		0(a0,d2.l),a0
;//	move		d0,d5
	subq		#1,d0
	subq		#1,d7

.PlaneLoop:
	move		d0,d6
	move.l	Ziel(sp),a1

.ZeilenLoop:
	move		(a0)+,d2

	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	move.b	(a1),d4		;//(**)
	add.b		d4,d4
	add		d2,d2
	addx.b	d3,d4
	move.b	d4,(a1)+
	dbf		d6,.ZeilenLoop

	sub.w		d1,a0
	sub.w		d1,a0
	dbf		d7,.PlaneLoop

	movem.l	(sp)+,d2-d7
	rts
	
	END



