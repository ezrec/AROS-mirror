
		.text

		.global	CyberGfxBase
		.global	PPCCallOS

		.global	BestCModeIDTags
		.global	LockBitMapTags

		.align	3

BestCModeIDTags:
		stwu	1,-128(1)
		mflr	11
		stw	11,100(1)
		lwz	11,128(1)
		stw	11,96(1)
		stw	3,104(1)
		stw	4,108(1)
		stw	5,112(1)
		stw	6,116(1)
		stw	7,120(1)
		stw	8,124(1)
		stw	9,128(1)
		stw	10,132(1)
		addi	11,1,104
		stw	11,68(1)
		li	11,-60
		stw	11,8(1)
		li	11,1
		stw	11,12(1)
		stw	11,24(1)
		lis	11,CyberGfxBase@ha
		lwz	11,CyberGfxBase@l(11)
		stw	11,92(1)
		addi	3,1,8
		bl	PPCCallOS
		lwz	11,96(1)
		stw	11,128(1)
		lwz	11,100(1)
		mtlr	11
		addi	1,1,128
		blr

		.align	3

LockBitMapTags:
		stwu	1,-128(1)
		mflr	11
		stw	11,100(1)
		lwz	11,128(1)
		stw	11,96(1)
		stw	4,108(1)
		stw	5,112(1)
		stw	6,116(1)
		stw	7,120(1)
		stw	8,124(1)
		stw	9,128(1)
		stw	10,132(1)
		stw	3,68(1)
		addi	11,1,108
		stw	11,72(1)
		li	11,-168
		stw	11,8(1)
		li	11,1
		stw	11,12(1)
		stw	11,24(1)
		lis	11,CyberGfxBase@ha
		lwz	11,CyberGfxBase@l(11)
		stw	11,92(1)
		addi	3,1,8
		bl	PPCCallOS
		lwz	11,96(1)
		stw	11,128(1)
		lwz	11,100(1)
		mtlr	11
		addi	1,1,128
		blr

