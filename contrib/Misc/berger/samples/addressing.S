	clr.l	d0
	clr.l	d1
	clr.l	d2
	clr.l	d3
	clr.l	d4
	clr.l	d5
	clr.l	d6
	clr.l	d7

	lea	array1,a0
	move.b	(a0),d0		; d0 = 0; Z=1
	move.b	(a0)+,d1	; d1 = 0; Z=1
	move.b	-(a0),d2	; d2 = 0; Z=1
	move.b	1(a0),d3	; d3 = 1
	moveq	#5,d0		; d5 = 5;
	move.b	(a0,d0),d4	; d4 = 5
	move.b	1(a0,d0),d5	; d5 = 6
	move.l	#$10005,d0	; d0 = $10005
	move.b	array1(pc,d0.w),d6; d6 = 5
	moveq	#$5,d0		; d0 = $5
	move.b	array1(pc,d0.l),d7; d7 = 5

	lea	array1_2(pc),a0	; 
	move.b	(a0),d0		; d0 = $40
	move.b	(a0)+,d1	; d1 = $40
	move.b	(a0)+,d2	; d2 = $41
	move.b	-(a0),d3	; d3 = $41

	lea	array2(pc),a0
	move.w	1(a0),d0	; d0 = $101
	move.w	#2,a1		; a1 = $2
	move.w	(a0,a1),d2	; d2 = $102
	add	#1,a1
	move.w	array2(pc,a1),d3; d3 = $101

	rts


array1:	dc.b	0,1,2,3,4,5,6,7,8,9
array1_2:	
	dc.b	$40,$41,$42,$43,$44,$45,$46,$47,$48,$49
array2:	dc.w	$100,$101,$102,$103,$104,$105,$106,$107,$108,$109
array3:	dc.l	$10000,$10001,$10002,$10003,$10004
	dc.l	$10005,$10006,$10007,$10008,$10009

	END
