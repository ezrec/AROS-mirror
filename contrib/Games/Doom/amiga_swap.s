
		.extern	SwapSHORT
		.extern	SwapLONG

# AB -> BA

		.align	4

SwapSHORT:	mr	r4,r3
		rlwinm	r3,r4,24,24,31		# 000A
		rlwimi	r3,r4,8,16,23		# 00BA
		blr


# ABCD -> DCBA

		.align	4

SwapLONG:	mr	r4,r3
		rlwimi	r3,r4,8,24,31		# xxxA
		rlwimi	r3,r4,24,16,23		# xxBA
		rlwimi	r3,r4,8,8,15		# xCBA
		rlwimi	r3,r4,24,0,7		# DCBA
		blr


