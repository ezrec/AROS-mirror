
	XDEF _UnpackBytes

_UnpackBytes:
	movem.l	d2/a2,-(sp)
	
	movem.l	8+4(sp),a0/a1		;// a0 = source  a1 = dest
	movem.l	8+12(sp),d0/a2		;// d0 = size    a2 = nextdata

.loop:
	moveq		#0,d1
	move.b	(a0)+,d1
	bpl.s		.normalcopy
	
.repeatcopy:
	neg.b		d1
	bmi.s		.loop

	move.b	(a0)+,d2

.repeatloop:
	move.b	d2,(a1)+
	subq.l	#1,d0
	ble.s		.done
	
	dbf		d1,.repeatloop
	bra.s		.loop

.normalcopy:
	move.b	(a0)+,(a1)+
	subq.l	#1,d0
	ble.s		.done
	dbf		d1,.normalcopy
	bra.s		.loop
	
.done:
	move.l	a0,(a2)
	
	movem.l	(sp)+,d2/a2
	rts


	END

