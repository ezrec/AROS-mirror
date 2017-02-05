#
#  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
#
	.data
	.align 2
p2c_funcs:
	.long p2c_1,p2c_2,p2c_3,p2c_4,p2c_5,p2c_6,p2c_7,p2c_8

	.text
	.align 2
	.global planar2chunky
planar2chunky:
	subi	r1,r1,76
	stmw	r13,0(r1)
	subi	r8,r8,1
	lis		r11,p2c_funcs@ha
	addi	r11,r11,p2c_funcs@l
	slwi	r8,r8,2
	srwi	r6,r6,4
	lwzx	r11,r11,r8
	subi	r10,r3,2
	mtctr	r11
	subi	r5,r5,4
	bctr

	.include "p2c/p2c_1.s"
	.include "p2c/p2c_2.s"
	.include "p2c/p2c_3.s"
	.include "p2c/p2c_4.s"
	.include "p2c/p2c_5.s"
	.include "p2c/p2c_6.s"
	.include "p2c/p2c_7.s"
	.include "p2c/p2c_8.s"
