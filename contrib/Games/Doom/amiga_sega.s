
		.extern	Sega6
		.extern	Sega3

# returns - xxSAxxxxxxCBMXYZxxSAxxDUxxCBRLDU

		.align	4

Sega6:		mflr	r8
		lis	r7,0x00DF
		lis	r6,0x00BF
		ori	r7,r7,0xF000
		ori	r6,r6,0xE001

		bl	GetSega		# xxSAxxDUxxCBRLDU
		rlwinm	r3,r3,16,0,15	# save
		bl	GetSega		# xxSAxxDUxxCBRLDU
		bl	GetSega		# xxSAxxxxxxCBRLDU
		bl	GetSega		# xxSAxxxxxxCBMXYZ
		rlwinm	r3,r3,16,0,31	# xxSAxxxxxxCBMXYZxxSAxxDUxxCBRLDU

		li	r4,-0x0100	# SEL=1
		mtlr	r8
		sth	r4,0x34(r7)
		blr

# returns - 0000000000000000xxSAxxDUxxCBRLDU

		.align	4

Sega3:		mflr	r8
		lis	r7,0x00DF
		lis	r6,0x00BF
		ori	r7,r7,0xF000
		ori	r6,r6,0xE001

		li	r3,0
		bl	GetSega		# xxSAxxDUxxCBRLDU

		li	r4,-0x0100	# SEL=1
		mtlr	r8
		sth	r4,0x34(r7)
		blr

# read Sega controller

		.align	4

GetSega:	rlwinm	r3,r3,0,0,15
		li	r4,-0x00FF	# SEL=1, dump caps
		sth	r4,0x34(r7)

		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)

		rlwinm.	r4,r4,0,24,24
		bne	shl1
		ori	r3,r3,0x10	# B/0

shl1:		lhz	r4,0x16(r7)
		rlwinm.	r4,r4,0,17,17
		bne	shl2
		ori	r3,r3,0x20	# C/0

shl2:		lhz	r4,0xC(r7)
		rlwinm.	r5,r4,0,30,30
		beq	shl3
		ori	r3,r3,8		# R/M

shl3:		rlwinm.	r5,r4,0,22,22
		beq	shl4
		ori	r3,r3,4		# L/X

shl4:		rlwinm	r5,r4,1,0,31
		xor	r4,r4,r5
		rlwinm.	r5,r4,0,30,30
		beq	shl5
		ori	r3,r3,2		# D/Y

shl5:		rlwinm.	r5,r4,0,22,22
		beq	shl6
		ori	r3,r3,1		# U/Z

shl6:		li	r4,-0x10FF	# SEL=0, dump caps
		sth	r4,0x34(r7)

		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)
		lbz	r4,0(r6)

		rlwinm.	r4,r4,0,24,24
		bne	shl11
		ori	r3,r3,0x1000	# A/0

shl11:		lhz	r4,0x16(r7)
		rlwinm.	r4,r4,0,17,17
		bne	shl12
		ori	r3,r3,0x2000	# S/0

shl12:		lhz	r4,0xC(r7)
		rlwinm.	r5,r4,0,30,30
		beq	shl13
		ori	r3,r3,0x800	# 0/1

shl13:		rlwinm.	r5,r4,0,22,22
		beq	shl14
		ori	r3,r3,0x400	# 0/1

shl14:		rlwinm	r5,r4,1,0,31
		xor	r4,r4,r5
		rlwinm.	r5,r4,0,30,30
		beq	shl15
		ori	r3,r3,0x200	# D/1

shl15:		rlwinm.	r5,r4,0,22,22
		beq	shl16
		ori	r3,r3,0x100	# U/1

shl16:		blr

