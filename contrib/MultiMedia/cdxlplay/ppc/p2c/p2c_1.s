#
#  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
#
p2c_1:
	mr		r4,r6
p2c_1_001:
	subi	r7,r7,1
p2c_1_002:
	lhzu	r18,2(r10)

	subi	r4,r4,1
	
	# pixels 0, 4, 8 and 12
	rlwinm	r26,r18,9,7,7
	rlwinm	r27,r18,13,7,7
	rlwinm	r28,r18,17,7,7
	rlwinm	r29,r18,21,7,7
	
	# pixels 1, 5, 9 and 13
	rlwimi	r26,r18,2,15,15
	rlwimi	r27,r18,6,15,15
	rlwimi	r28,r18,10,15,15
	rlwimi	r29,r18,14,15,15
	
	# pixels 2, 6, 10 and 14
	rlwimi	r26,r18,27,23,23
	rlwimi	r27,r18,31,23,23
	rlwimi	r28,r18,3,23,23
	rlwimi	r29,r18,7,23,23
	
	# pixels 3, 7, 11 and 15
	rlwimi	r26,r18,20,31,31
	rlwimi	r27,r18,24,31,31
	rlwimi	r28,r18,28,31,31
	rlwimi	r29,r18,0,31,31
	
	cmpwi	cr0,r4,0
	
	stw		r26,4(r5)
	stw		r27,8(r5)
	stw		r28,12(r5)
	stwu	r29,16(r5)
	
	bne+	p2c_1_002
	
	cmpwi	cr0,r7,0
	mr		r4,r6
	bne+	p2c_1_001

	lmw		r13,0(r1)
	addi	r1,r1,76
	blr
