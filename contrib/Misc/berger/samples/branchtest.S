	moveq	#$1,d0
	subq	#$1,d0
	beq	label1

dead:	move.l	#$deadbeef,d0
	rts

label1:	subq	#$1,d0
	bmi	label2
	bra	dead

label2:	moveq	#$1,d0
	bpl	label3
	bra	dead

label3:	move.l	#-1,d0
	bmi	label4
	bra	dead

label4:	move.l	#$1,d0
	cmpi.l	#$1,d0
	beq	label5
	bra	dead

label5:	cmpi.l	#$0,d0
	bgt	label6
	bra	dead

label6:	cmp.l	#2,d0	; X=1 N=1 C=1
	blt	label7
	bra	dead

label7:	cmp.l	#$1,d0
	bge	label8
	bra	dead

label8:	cmp.l	#2,d0   ; X=1 N=1 C=1
	ble     label9
	bra	dead

label9:	moveq	#$2,d1	; X=1
	btst	#1,d1	; X=1
	bne	label10
	bra	dead

label10:moveq	#$1,d0	; X=1
	bchg	d0,d1	; X=1
	bne	label11 ; 
	bra	dead

label11:move.l	#$10,d0 ; X=1
	add.b	#$70,d0 ; N=1 V=1
	bvs	label12
	bra	dead

label12:move.l	#$10,d0	; all 0
	add.b	#$6f,d0 ; all 0
	bvc	label13
	bra	dead

label13:moveq	#0,d0	; Z=1
	sub.b	#127,d0	; X=1 N=1 C=1
	bvc	label14
	bra	dead

label14:moveq	#0,d0	; X=1 Z=1
	sub.b	#128,d0	; X=1 N=1 V=1 C=1
	bvs	label15
	bra	dead

label15:rts

	END
