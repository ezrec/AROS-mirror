;Address, RemapTable (LongWords!!!), Length (multiple of 4 (or add 3))

	XDEF	_RemapBytes
	
_RemapBytes:
	movem.l	d2/d3,-(sp)

	movem.l	8+4(sp),a0/a1
	move.l	8+12(sp),d0

	addq.l	#1,a1
	moveq		#0,d2

.loop:
	move.l	(a0),d1				;Get 4 Bytes
	
	move.b	d1,d2
	move.b	(a1,d2.w*2),d3
	ror.l		#8,d3
	
	lsr.l		#8,d1
	move.b	d1,d2
	move.b	(a1,d2.w*2),d3
	ror.l		#8,d3
	
	lsr.l		#8,d1
	move.b	d1,d2
	move.b	(a1,d2.w*2),d3
	ror.l		#8,d3
	
	lsr.l		#8,d1
	move.b	d1,d2
	move.b	(a1,d2.w*2),d3
	ror.l		#8,d3
	
	move.l	d3,(a0)+

	subq.l	#4,d0
	bpl.s		.loop

	movem.l	(sp)+,d2/d3
	rts

	END


