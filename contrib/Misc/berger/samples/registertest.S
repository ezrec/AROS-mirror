	moveq	#$0,d0	; d0 = 0  Z=1
	moveq	#$1,d1	; d1 = 1  Z=0
	moveq	#$2,d2	; d2 = 2  Z=0
	moveq	#$3,d3	; d3 = 3  Z=0
	moveq	#$4,d4	; d4 = 4  Z=0
	moveq	#$5,d5	; d5 = 5  Z=0
	moveq	#$6,d6	; d6 = 6  Z=0
	moveq	#$7,d7	; d7 = 7  Z=0

	nop

	move.l	#-1,d0	; d0 = #$ffffffff  N=1
	clr.b	d0	; d0 = #$ffffff00  N=0 Z=1
	clr.w	d0	; d0 = #$ffff0000  Z=1
	clr.l	d0	; d0 = #$00000000  Z=1

	nop

	move	#$80,d0	; d0 = #$00000080  Z=0 N=0
	add.b	#$ff,d0 ; d0 = #$0000007f  X=1 V=1 C=1

	nop

	move	#-1,d0  ; d0 = #$0000ffff  X=1 N=1
	and.b	#$9,d0  ; d0 = #$0000ff09  X=1
	and.w	#$99,d0 ; d0 = #$00000009  X=1
	and.l	#$99999999,d0	; d0 = #$00000009  X=1
	nop

	move	#-1,d0	; d0 = #$0000ffff X=1 N=1
	eor.b	#$9,d0	; d0 = #$0000fff6 X=1 N=1
	eor.w	#$99,d0	; d0 = #$0000ff6f X=1 N=1
	eor.l	#$99999999,d0	; d0 = #$999966f6 X=1 N=1
	nop

	move	#0,d0	; d0 = #$99990000 X=1 Z=1
	or.b	#$9,d0	; d0 = #$99990009 X=1
	or.w	#$99,d0	; d0 = #$99990099 X=1
	or.l	#$99999999,d0	; d0 = #$99999999 X=1 N=1

	nop

	move.b	#-1,d0	; d0 = #$999999ff X=1 N=1
	ext.w	d0	; d0 = #$9999ffff X=1 N=1
	ext.l	d0	; d0 = #$ffffffff X=1 N=1

	moveq	#$0,d0	; d0 = #$00000000 X=1 Z=1
	moveq	#-1,d0	; d0 = #$00000000 X=1 N=1

        moveq	#$0,d0	; d0 = #$00000000 X=1 Z=1
	neg.b	d0                        Z=1

        moveq	#$0,d0	; d0 = #$00000000 Z=1
	neg.w	d0                        Z=1

        moveq	#$0,d0	; d0 = #$00000000 Z=1
	neg.l	d0                        Z=1

        moveq	#$1,d0	; d0 = #$00000001
	neg.b	d0      ; d0 = #$000000ff X=1 N=1 C=1

        move.l	#$80,d0	; d0 = #$00000080 X=1
	neg.b	d0      ; d0 = #$00000080 X=1 N=1 V=1 C=1

        move.l	#$7f,d0	; d0 = #$0000007f X=1
	neg.b	d0      ; d0 = #$00000081 X=1 N=1 C=1


        moveq	#$0,d0	; d0 = #$00000000 X=1 Z=1
	not.b	d0      ; d0 = #$000000ff X=1 N=1

        moveq	#$0,d0	; d0 = #$00000000 X=1 Z=1
	not.w	d0      ; d0 = #$0000ffff X=1 N=1

        moveq	#$0,d0	; d0 = #$00000000 X=1 Z=1
	not.l	d0      ; d0 = #$ffffffff X=1 N=1

        moveq	#$1,d0	; d0 = #$00000001 X=1
	not.b	d0      ; d0 = #$000000fe X=1 N=1

        move.l	#$80,d0	; d0 = #$00000080 X=1
	not.b	d0      ; d0 = #$0000007f X=1

        move.l	#$7f,d0	; d0 = #$0000007f X=1
	not.b	d0      ; d0 = #$00000080 X=1 N=1

        moveq	#$40,d0	; d0 = #$00000040 X=1
	add.b	#$80,d0	; d0 = #$000000c0 N=1
	add.b	#$80,d0	; d0 = #$00000040 X=1 V=1 C=1

	rts	

	END
