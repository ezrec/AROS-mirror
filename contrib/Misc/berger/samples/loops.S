	moveq	#0,d0
	move.l  #$80,d1
	bsr	l1
	nop     	; d1 = $0000ff80 ; N=1 V=1 C=1

	moveq	#1,d0
	move	#$80,d1
	bsr	l1
	nop      	; d1 = $0000ff81 ; N=1 V=1 C=1

	move	#25,d0
	move	#$80,d1
	bsr	l1
	nop            	; d1 = $0000ff99 ; N=1 V=1 C=1

	moveq	#0,d0
	move	#$7f,d1
	bsr	l2
	nop       	; d1 = $00000081 ; C=1

	moveq	#1,d0
	move	#$7f,d1
	bsr	l2
	nop      	; d1 = 000000082 ; C=1 	

	move	#25,d0
	move	#$7f,d1
	bsr	l2
	nop             ; d1 = $0000009a ; C=1

	move	#10,d0
	move	#$7f,d1
	bsr	l3
	nop		; d1 = $0000000a ; Z=1

	move	#0,d0
	move	#$7f,d1
	bsr	l4
	nop		; d1 = $00000100 ; Z=1

	move	#10,d0
	move	#$7f,d1
	bsr	l5
	nop		; d1 = $00000009
	
	move	#10,d0
	move	#$7f,d1
	bsr	l6
	nop		; d1 = $0000010b ; N=1 C=1

	rts		



l1:	subq	#1,d1
	cmp.b	d1,d0
	bvc	l1
	rts
	
l2:	addq	#1,d1
	cmp.b	d1,d0
	bvs	l2
	rts

l3:	subq	#1,d1
	cmp.b	d1,d0
	blt	l3
	rts

l4:	addq	#1,d1
	cmp.b	d1,d0
	bgt	l4
	rts

l5:	subq	#1,d1
	cmp.b	d1,d0
	ble	l5
	rts

l6:	addq	#1,d1
	cmp.b	d1,d0
	bge	l6
	rts

	END
