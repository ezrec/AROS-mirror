#
#  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
#
p2c_3:
	add		r3,r4,r4
	add		r11,r10,r4
	add		r12,r10,r3

	mr		r4,r6
p2c_3_001:
	subi	r7,r7,1
p2c_3_002:
	lhzu	r18,2(r10)
	lhzu	r19,2(r11)
	lhzu	r20,2(r12)

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
	
	cmpwi	cr0,r4,0
	
	stw		r26,4(r5)
	stw		r27,8(r5)
	stw		r28,12(r5)
	stwu	r29,16(r5)
	
	bne+	p2c_3_002
	
	cmpwi	cr0,r7,0
	mr		r4,r6
	bne+	p2c_3_001

	lmw		r13,0(r1)
	addi	r1,r1,76
	blr
