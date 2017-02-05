#
#  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
#
p2c_7:
	add		r3,r4,r4
	add		r11,r10,r4
	add		r12,r10,r3
	add		r13,r11,r3
	add		r14,r12,r3
	add		r15,r13,r3
	add		r16,r14,r3

	mr		r4,r6
p2c_7_001:
	subi	r7,r7,1
p2c_7_002:
	lhzu	r18,2(r10)
	lhzu	r19,2(r11)
	lhzu	r20,2(r12)
	lhzu	r21,2(r13)
	lhzu	r22,2(r14)
	lhzu	r23,2(r15)
	lhzu	r24,2(r16)

	subi	r4,r4,1
	
	# pixels 0, 4, 8 and 12
	rlwinm	r26,r18,9,7,7
	rlwinm	r27,r18,13,7,7
	rlwinm	r28,r18,17,7,7
	rlwinm	r29,r18,21,7,7
	rlwimi	r26,r19,10,6,6
	rlwimi	r27,r19,14,6,6
	rlwimi	r28,r19,18,6,6
	rlwimi	r29,r19,22,6,6
	rlwimi	r26,r20,11,5,5
	rlwimi	r27,r20,15,5,5
	rlwimi	r28,r20,19,5,5
	rlwimi	r29,r20,23,5,5
	rlwimi	r26,r21,12,4,4
	rlwimi	r27,r21,16,4,4
	rlwimi	r28,r21,20,4,4
	rlwimi	r29,r21,24,4,4
	rlwimi	r26,r22,13,3,3
	rlwimi	r27,r22,17,3,3
	rlwimi	r28,r22,21,3,3
	rlwimi	r29,r22,25,3,3
	rlwimi	r26,r23,14,2,2
	rlwimi	r27,r23,18,2,2
	rlwimi	r28,r23,22,2,2
	rlwimi	r29,r23,26,2,2
	rlwimi	r26,r24,15,1,1
	rlwimi	r27,r24,19,1,1
	rlwimi	r28,r24,23,1,1
	rlwimi	r29,r24,27,1,1
	
	# pixels 1, 5, 9 and 13
	rlwimi	r26,r18,2,15,15
	rlwimi	r27,r18,6,15,15
	rlwimi	r28,r18,10,15,15
	rlwimi	r29,r18,14,15,15
	rlwimi	r26,r19,3,14,14
	rlwimi	r27,r19,7,14,14
	rlwimi	r28,r19,11,14,14
	rlwimi	r29,r19,15,14,14
	rlwimi	r26,r20,4,13,13
	rlwimi	r27,r20,8,13,13
	rlwimi	r28,r20,12,13,13
	rlwimi	r29,r20,16,13,13
	rlwimi	r26,r21,5,12,12
	rlwimi	r27,r21,9,12,12
	rlwimi	r28,r21,13,12,12
	rlwimi	r29,r21,17,12,12
	rlwimi	r26,r22,6,11,11
	rlwimi	r27,r22,10,11,11
	rlwimi	r28,r22,14,11,11
	rlwimi	r29,r22,18,11,11
	rlwimi	r26,r23,7,10,10
	rlwimi	r27,r23,11,10,10
	rlwimi	r28,r23,15,10,10
	rlwimi	r29,r23,19,10,10
	rlwimi	r26,r24,8,9,9
	rlwimi	r27,r24,12,9,9
	rlwimi	r28,r24,16,9,9
	rlwimi	r29,r24,20,9,9
	
	# pixels 2, 6, 10 and 14
	rlwimi	r26,r18,27,23,23
	rlwimi	r27,r18,31,23,23
	rlwimi	r28,r18,3,23,23
	rlwimi	r29,r18,7,23,23
	rlwimi	r26,r19,28,22,22
	rlwimi	r27,r19,0,22,22
	rlwimi	r28,r19,4,22,22
	rlwimi	r29,r19,8,22,22
	rlwimi	r26,r20,29,21,21
	rlwimi	r27,r20,1,21,21
	rlwimi	r28,r20,5,21,21
	rlwimi	r29,r20,9,21,21
	rlwimi	r26,r21,30,20,20
	rlwimi	r27,r21,2,20,20
	rlwimi	r28,r21,6,20,20
	rlwimi	r29,r21,10,20,20
	rlwimi	r26,r22,31,19,19
	rlwimi	r27,r22,3,19,19
	rlwimi	r28,r22,7,19,19
	rlwimi	r29,r22,11,19,19
	rlwimi	r26,r23,0,18,18
	rlwimi	r27,r23,4,18,18
	rlwimi	r28,r23,8,18,18
	rlwimi	r29,r23,12,18,18
	rlwimi	r26,r24,1,17,17
	rlwimi	r27,r24,5,17,17
	rlwimi	r28,r24,9,17,17
	rlwimi	r29,r24,13,17,17
	
	# pixels 3, 7, 11 and 15
	rlwimi	r26,r18,20,31,31
	rlwimi	r27,r18,24,31,31
	rlwimi	r28,r18,28,31,31
	rlwimi	r29,r18,0,31,31
	rlwimi	r26,r19,21,30,30
	rlwimi	r27,r19,25,30,30
	rlwimi	r28,r19,29,30,30
	rlwimi	r29,r19,1,30,30
	rlwimi	r26,r20,22,29,29
	rlwimi	r27,r20,26,29,29
	rlwimi	r28,r20,30,29,29
	rlwimi	r29,r20,2,29,29
	rlwimi	r26,r21,23,28,28
	rlwimi	r27,r21,27,28,28
	rlwimi	r28,r21,31,28,28
	rlwimi	r29,r21,3,28,28
	rlwimi	r26,r22,24,27,27
	rlwimi	r27,r22,28,27,27
	rlwimi	r28,r22,0,27,27
	rlwimi	r29,r22,4,27,27
	rlwimi	r26,r23,25,26,26
	rlwimi	r27,r23,29,26,26
	rlwimi	r28,r23,1,26,26
	rlwimi	r29,r23,5,26,26
	rlwimi	r26,r24,26,25,25
	rlwimi	r27,r24,30,25,25
	rlwimi	r28,r24,2,25,25
	rlwimi	r29,r24,6,25,25
	
	cmpwi	cr0,r4,0
	
	stw		r26,4(r5)
	stw		r27,8(r5)
	stw		r28,12(r5)
	stwu	r29,16(r5)
	
	bne+	p2c_7_002
	
	cmpwi	cr0,r7,0
	mr		r4,r6
	bne+	p2c_7_001

	lmw		r13,0(r1)
	addi	r1,r1,76
	blr
