
	XDEF	_Copy_PutChFunc
	XDEF	_GetLength_PutChFunc
	XDEF	_MakeMask

_Copy_PutChFunc:
	move.b	d0,(a3)+
	rts

_GetLength_PutChFunc:
	addq.l	#1,(a3)
	rts

; (APTR mask,APTR chunky,LONG bwidth,LONG modulo,LONG height,LONG maskplanes)
_MakeMask:
	movem.l	d2-d6/a2,-(sp)

	movem.l	6*4+4(sp),a0/a1		;// a0 = mask  a1 = chunky
	addq.l	#8,a1
	addq.l	#8,a1						;// chunky += 16 pixels
	movem.l	6*4+12(sp),d0-d3		;// d0 = bwidth  d1 = modulo
											;// d2 = height  d3 = maskplanes

	move.l	d0,d4				;// skip 1 mask line:
	lsr.l		#3,d4
	mulu		d3,d4
	add.l		d4,a0				;// mask += bwidth / 8

	subq		#1,d3
	mulu		d0,d3				;// (maskplanes - 1) * bwidth =
	lsr.l		#4,d3				;// d3 = "planemodulo" in words

	sub.l		d1,d0
	lsr.l		#3,d0				;// d0 = (bwidth - modulo) in BYTEs

	lsr.l		#4,d1				;// d1 = modulo in WORDs

	bra		.yloop_entry

.yloop:

	move.l	d1,d5
	move.l	a0,a2				;// save mask linestart addr
	bra		.xloop_entry

.xloop:
	moveq		#0,d4				;// mask will be in high word!

;--
	move.l	-(a1),d6			;// Pixels 13 .. 16
	tst.b		d6					;// 16
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 15
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 14
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 13
	tst.b		d6
	sne		d4
	ror.l		#1,d4
;--
	move.l	-(a1),d6			;// Pixels 9 .. 12
	tst.b		d6					;// 12
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 11
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 10
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 9
	tst.b		d6
	sne		d4
	ror.l		#1,d4
;--
	move.l	-(a1),d6			;// Pixels 5 .. 8
	tst.b		d6					;// 8
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 7
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 6
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 5
	tst.b		d6
	sne		d4
	ror.l		#1,d4
;--
	move.l	-(a1),d6			;// Pixels 1 .. 4
	tst.b		d6					;// 4
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 3
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 2
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	lsr.l		#8,d6				;// 1
	tst.b		d6
	sne		d4
	ror.l		#1,d4

	swap		d4
	move		d4,(a0)+			;// 16 mask bits created

	moveq		#32,d4
	add.l		d4,a1				;// chunky += 32 (not 16, because of 4x -(a1) !!!

.xloop_entry:
	dbf		d5,.xloop

	add.l		d0,a0				;// skip bwidth - modulo

	move		d3,d4
	bra.s		.planeloop_entry

.planeloop:
	move		(a2)+,(a0)+
.planeloop_entry:
	dbf		d4,.planeloop

.yloop_entry:
	dbf		d2,.yloop

	movem.l	(sp)+,d2-d6/a2
	rts
;
; OBSOLETE !
;
; IFD GCC
; 	XDEF	_WriteLUTPixelArray
;	XREF	_CyberGfxBase
;
;_WriteLUTPixelArray:
;    movem.l	d2-d7/a2/a6,-(sp)
;    move.l	_CyberGfxBase,a6
;
;    move.l	8*4+4(sp),a0
;    movem.l	8*4+8(sp),d0-d2
;    movem.l	8*4+20(sp),a1-a2
;    movem.l	8*4+28(sp),d3-d7

;    jsr		-198(a6)

;    movem.l	(sp)+,d2-d7/a2/a6
;    rts
;
; ENDC

	END
