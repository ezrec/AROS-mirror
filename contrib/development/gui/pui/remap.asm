;/* Remap.s */
;
;a0 = Address  a1=RemapTable (LongWords!!!)  d0=Length

	XDEF	RemapBytes

RemapBytes:
	move.l	a2,-(sp)

	moveq		#0,d1
	bra.s		.loopentry
	
	CNOP		0,4
	
.remaploop:
	move.b	(a0),d1
	move.l	a1,a2
	add.w		d1,a2
	move.b	(a2),(a0)+

.loopentry:
	dbf		d0,.remaploop
	
	move.l	(sp)+,a2
	rts

	END

