
	XDef	_nsd_add

;------------------------------------------------------------------------------
; a0 = ptr to 64 bit buffer
; d0 = 32bit source (UNSIGNED)
; d1 = 32bit increment (UNSIGNED)

_nsd_add
	movem.l	d0-d2/a0,-(sp)

	move.l	20(sp),a0
	move.l	24(sp),d0
	move.l	d0,d1
	move.l	28(sp),d2

	add.l	d2,d0
	cmp.l	d0,d1
	bhi	overflow

	move.l	d0,4(a0)
	movem.l	(sp)+,d0-d2/a0
	rts

overflow
	move.l	d0,4(a0)
	add.l	#1,(a0)
	movem.l	(sp)+,d0-d2/a0
	rts

	
