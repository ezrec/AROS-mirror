
		.extern	SetFPMode
		.extern	FixedMul
		.extern	FixedDiv

# set rounding mode to "truncate towards minus infinity"
# otherwise demos get out of step and play wrong

SetFPMode:	mtfsb1	30
		mtfsb1	31
		blr

# fixed_t FixedMul (fixed_t a, fixed_t b)

FixedMul:	mullw	r5,r3,r4
		mulhw	r6,r3,r4
		rlwinm	r3,r5,16,16,31
		rlwimi	r3,r6,16,0,15
		blr

# fixed_t FixedDiv (fixed_t a, fixed_t b)

FixedDiv:	cmpwi	r4,0
		beq	over

		subi	r1,r1,8		# make space on stack for FP

# convert a to double

		cmpwi	r3,0
		lis	r5,0x4330
		li	r6,0
		bge	cont1
		neg	r3,r3
cont1:		stw	r3,4(r1)
		stw	r5,0(r1)
		lfd	f0,0(r1)
		stw	r6,4(r1)
		lfd	f1,0(r1)
		fsub	f0,f0,f1
		bge	conv2
		fneg	f0,f0

# convert b to double

conv2:		cmpwi	r4,0
		bge	cont2
		neg	r4,r4
cont2:		stw	r4,4(r1)
		lfd	f1,0(r1)
		stw	r6,4(r1)
		lfd	f2,0(r1)
		fsub	f1,f1,f2
		bge	ldcons
		fneg	f1,f1

# load 1/65536

ldcons:		lis	r5,0x40F0
		stw	r5,0(r1)
		stw	r6,4(r1)
		lfd	f2,0(r1)

# do fixed pt division

		fdiv	f0,f0,f1
		fmul	f0,f0,f2

		fctiw	f0,f0
		stfd	f0,(r1)
		lwz	r3,4(r1)

		addi	r1,r1,8
		blr

# divide by 0, overflow

over:		cmpwi	r3,0
		lis	r3,-0x8000
		blt	return
		not	r3,r3
return:		blr


