
		.extern	ppc_c2p_line

# convert chunky line to planar
# r3 = line #
# r4 = src ptr
# r5 = dest bitmap
# r6 = (SCREENWIDTH+31)>>5 (# cells to convert per line per plane)

		.align	4

ppc_c2p_line:	addi	r1,r1,-36
		stswi	r13,r1,0		# save registers
		stw	r21,32(r1)

		mtctr	r6			# ctr = number of loops

		lhz	r6,0(r5)		# bm_BytesPerRow
		mullw	r3,r3,r6		# offset in BitPlane of line

		addi	r5,r5,12		# bm_Planes[1]
		lswi	r6,r5,28		# BP2-8
		lwz	r5,-4(r5)		# BP1

c2p_loop:	lswi	r13,r4,0		# next 8 longs

		rlwimi	r0,r13,0,0,0		# A7
		rlwimi	r0,r13,7,1,1		# B7
		rlwimi	r0,r13,14,2,2		# C7
		rlwimi	r0,r13,21,3,3		# D7
		rlwimi	r0,r14,28,4,4		# E7
		rlwimi	r0,r14,3,5,5		# F7
		rlwimi	r0,r14,10,6,6		# G7
		rlwimi	r0,r14,17,7,7		# H7
		rlwimi	r0,r15,24,8,8		# I7
		rlwimi	r0,r15,31,9,9		# J7
		rlwimi	r0,r15,6,10,10		# K7
		rlwimi	r0,r15,13,11,11		# L7
		rlwimi	r0,r16,20,12,12 	# M7
		rlwimi	r0,r16,27,13,13		# N7
		rlwimi	r0,r16,2,14,14		# O7
		rlwimi	r0,r16,9,15,15		# P7
		rlwimi	r0,r17,16,16,16		# Q7
		rlwimi	r0,r17,23,17,17		# R7
		rlwimi	r0,r17,30,18,18		# S7
		rlwimi	r0,r17,5,19,19		# T7
		rlwimi	r0,r18,12,20,20		# U7
		rlwimi	r0,r18,19,21,21		# V7
		rlwimi	r0,r18,26,22,22		# W7
		rlwimi	r0,r18,1,23,23		# X7
		rlwimi	r0,r19,8,24,24		# Y7
		rlwimi	r0,r19,15,25,25		# Z7
		rlwimi	r0,r19,22,26,26		# a7
		rlwimi	r0,r19,29,27,27		# b7
		rlwimi	r0,r20,4,28,28		# c7
		rlwimi	r0,r20,11,29,29		# d7
		rlwimi	r0,r20,18,30,30		# e7
		rlwimi	r0,r20,25,31,31		# f7
		stwx	r0,r12,r3		# store to BP8+offset

		rlwimi	r21,r13,1,0,0		# A6
		rlwimi	r21,r13,8,1,1		# B6
		rlwimi	r21,r13,15,2,2		# C6
		rlwimi	r21,r13,22,3,3		# D6
		rlwimi	r21,r14,29,4,4		# E6
		rlwimi	r21,r14,4,5,5		# F6
		rlwimi	r21,r14,11,6,6		# G6
		rlwimi	r21,r14,18,7,7		# H6
		rlwimi	r21,r15,25,8,8		# I6
		rlwimi	r21,r15,0,9,9		# J6
		rlwimi	r21,r15,7,10,10		# K6
		rlwimi	r21,r15,14,11,11	# L6
		rlwimi	r21,r16,21,12,12	# M6
		rlwimi	r21,r16,28,13,13	# N6
		rlwimi	r21,r16,3,14,14		# O6
		rlwimi	r21,r16,10,15,15	# P6
		rlwimi	r21,r17,17,16,16	# Q6
		rlwimi	r21,r17,24,17,17	# R6
		rlwimi	r21,r17,31,18,18	# S6
		rlwimi	r21,r17,6,19,19		# T6
		rlwimi	r21,r18,13,20,20	# U6
		rlwimi	r21,r18,20,21,21	# V6
		rlwimi	r21,r18,27,22,22	# W6
		rlwimi	r21,r18,2,23,23		# X6
		rlwimi	r21,r19,9,24,24		# Y6
		rlwimi	r21,r19,16,25,25	# Z6
		rlwimi	r21,r19,23,26,26	# a6
		rlwimi	r21,r19,30,27,27	# b6
		rlwimi	r21,r20,5,28,28		# c6
		rlwimi	r21,r20,12,29,29	# d6
		rlwimi	r21,r20,19,30,30	# e6
		rlwimi	r21,r20,26,31,31	# f6
		stwx	r21,r11,r3		# store to BP7+offset

		rlwimi	r0,r13,2,0,0		# A5
		rlwimi	r0,r13,9,1,1		# B5
		rlwimi	r0,r13,16,2,2		# C5
		rlwimi	r0,r13,23,3,3		# D5
		rlwimi	r0,r14,30,4,4		# E5
		rlwimi	r0,r14,5,5,5		# F5
		rlwimi	r0,r14,12,6,6		# G5
		rlwimi	r0,r14,19,7,7		# H5
		rlwimi	r0,r15,26,8,8		# I5
		rlwimi	r0,r15,1,9,9		# J5
		rlwimi	r0,r15,8,10,10		# K5
		rlwimi	r0,r15,15,11,11		# L5
		rlwimi	r0,r16,22,12,12		# M5
		rlwimi	r0,r16,29,13,13		# N5
		rlwimi	r0,r16,4,14,14		# O5
		rlwimi	r0,r16,11,15,15		# P5
		rlwimi	r0,r17,18,16,16		# Q5
		rlwimi	r0,r17,25,17,17		# R5
		rlwimi	r0,r17,0,18,18		# S5
		rlwimi	r0,r17,7,19,19		# T5
		rlwimi	r0,r18,14,20,20		# U5
		rlwimi	r0,r18,21,21,21		# V5
		rlwimi	r0,r18,28,22,22		# W5
		rlwimi	r0,r18,3,23,23		# X5
		rlwimi	r0,r19,10,24,24		# Y5
		rlwimi	r0,r19,17,25,25		# Z5
		rlwimi	r0,r19,24,26,26		# a5
		rlwimi	r0,r19,31,27,27		# b5
		rlwimi	r0,r20,6,28,28		# c5
		rlwimi	r0,r20,13,29,29		# d5
		rlwimi	r0,r20,20,30,30		# e5
		rlwimi	r0,r20,27,31,31		# f5
		stwx	r0,r10,r3		# store to BP6+offset

		rlwimi	r21,r13,3,0,0		# A4
		rlwimi	r21,r13,10,1,1		# B4
		rlwimi	r21,r13,17,2,2		# C4
		rlwimi	r21,r13,24,3,3		# D4
		rlwimi	r21,r14,31,4,4		# E4
		rlwimi	r21,r14,6,5,5		# F4
		rlwimi	r21,r14,13,6,6		# G4
		rlwimi	r21,r14,20,7,7		# H4
		rlwimi	r21,r15,27,8,8		# I4
		rlwimi	r21,r15,2,9,9		# J4
		rlwimi	r21,r15,9,10,10		# K4
		rlwimi	r21,r15,16,11,11	# L4
		rlwimi	r21,r16,23,12,12	# M4
		rlwimi	r21,r16,30,13,13	# N4
		rlwimi	r21,r16,5,14,14		# O4
		rlwimi	r21,r16,12,15,15	# P4
		rlwimi	r21,r17,19,16,16	# Q4
		rlwimi	r21,r17,26,17,17	# R4
		rlwimi	r21,r17,1,18,18		# S4
		rlwimi	r21,r17,8,19,19		# T4
		rlwimi	r21,r18,15,20,20	# U4
		rlwimi	r21,r18,22,21,21	# V4
		rlwimi	r21,r18,29,22,22	# W4
		rlwimi	r21,r18,4,23,23		# X4
		rlwimi	r21,r19,11,24,24	# Y4
		rlwimi	r21,r19,18,25,25	# Z4
		rlwimi	r21,r19,25,26,26	# a4
		rlwimi	r21,r19,0,27,27		# b4
		rlwimi	r21,r20,7,28,28		# c4
		rlwimi	r21,r20,14,29,29	# d4
		rlwimi	r21,r20,21,30,30	# e4
		rlwimi	r21,r20,28,31,31	# f4
		stwx	r21,r9,r3		# store to BP5+offset

		rlwimi	r0,r13,4,0,0		# A3
		rlwimi	r0,r13,11,1,1		# B3
		rlwimi	r0,r13,18,2,2		# C3
		rlwimi	r0,r13,25,3,3		# D3
		rlwimi	r0,r14,0,4,4		# E3
		rlwimi	r0,r14,7,5,5		# F3
		rlwimi	r0,r14,14,6,6		# G3
		rlwimi	r0,r14,21,7,7		# H3
		rlwimi	r0,r15,28,8,8		# I3
		rlwimi	r0,r15,3,9,9		# J3
		rlwimi	r0,r15,10,10,10		# K3
		rlwimi	r0,r15,17,11,11		# L3
		rlwimi	r0,r16,24,12,12		# M3
		rlwimi	r0,r16,31,13,13		# N3
		rlwimi	r0,r16,6,14,14		# O3
		rlwimi	r0,r16,13,15,15		# P3
		rlwimi	r0,r17,20,16,16		# Q3
		rlwimi	r0,r17,27,17,17		# R3
		rlwimi	r0,r17,2,18,18		# S3
		rlwimi	r0,r17,9,19,19		# T3
		rlwimi	r0,r18,16,20,20		# U3
		rlwimi	r0,r18,23,21,21		# V3
		rlwimi	r0,r18,30,22,22		# W3
		rlwimi	r0,r18,5,23,23		# X3
		rlwimi	r0,r19,12,24,24		# Y3
		rlwimi	r0,r19,19,25,25		# Z3
		rlwimi	r0,r19,26,26,26		# a3
		rlwimi	r0,r19,1,27,27		# b3
		rlwimi	r0,r20,8,28,28		# c3
		rlwimi	r0,r20,15,29,29		# d3
		rlwimi	r0,r20,22,30,30		# e3
		rlwimi	r0,r20,29,31,31		# f3
		stwx	r0,r8,r3		# store to BP4+offset

		rlwimi	r21,r13,5,0,0		# A2
		rlwimi	r21,r13,12,1,1		# B2
		rlwimi	r21,r13,19,2,2		# C2
		rlwimi	r21,r13,26,3,3		# D2
		rlwimi	r21,r14,1,4,4		# E2
		rlwimi	r21,r14,8,5,5		# F2
		rlwimi	r21,r14,15,6,6		# G2
		rlwimi	r21,r14,22,7,7		# H2
		rlwimi	r21,r15,29,8,8		# I2
		rlwimi	r21,r15,4,9,9		# J2
		rlwimi	r21,r15,11,10,10	# K2
		rlwimi	r21,r15,18,11,11	# L2
		rlwimi	r21,r16,25,12,12	# M2
		rlwimi	r21,r16,0,13,13		# N2
		rlwimi	r21,r16,7,14,14		# O2
		rlwimi	r21,r16,14,15,15	# P2
		rlwimi	r21,r17,21,16,16	# Q2
		rlwimi	r21,r17,28,17,17	# R2
		rlwimi	r21,r17,3,18,18		# S2
		rlwimi	r21,r17,10,19,19	# T2
		rlwimi	r21,r18,17,20,20	# U2
		rlwimi	r21,r18,24,21,21	# V2
		rlwimi	r21,r18,31,22,22	# W2
		rlwimi	r21,r18,6,23,23		# X2
		rlwimi	r21,r19,13,24,24	# Y2
		rlwimi	r21,r19,20,25,25	# Z2
		rlwimi	r21,r19,27,26,26	# a2
		rlwimi	r21,r19,2,27,27		# b2
		rlwimi	r21,r20,9,28,28		# c2
		rlwimi	r21,r20,16,29,29	# d2
		rlwimi	r21,r20,23,30,30	# e2
		rlwimi	r21,r20,30,31,31	# f2
		stwx	r21,r7,r3		# store to BP3+offset

		rlwimi	r0,r13,6,0,0		# A1
		rlwimi	r0,r13,13,1,1		# B1
		rlwimi	r0,r13,20,2,2		# C1
		rlwimi	r0,r13,27,3,3		# D1
		rlwimi	r0,r14,2,4,4		# E1
		rlwimi	r0,r14,9,5,5		# F1
		rlwimi	r0,r14,16,6,6		# G1
		rlwimi	r0,r14,23,7,7		# H1
		rlwimi	r0,r15,30,8,8		# I1
		rlwimi	r0,r15,5,9,9		# J1
		rlwimi	r0,r15,12,10,10		# K1
		rlwimi	r0,r15,19,11,11		# L1
		rlwimi	r0,r16,26,12,12 	# M1
		rlwimi	r0,r16,1,13,13		# N1
		rlwimi	r0,r16,8,14,14		# O1
		rlwimi	r0,r16,15,15,15		# P1
		rlwimi	r0,r17,22,16,16		# Q1
		rlwimi	r0,r17,29,17,17		# R1
		rlwimi	r0,r17,4,18,18		# S1
		rlwimi	r0,r17,11,19,19		# T1
		rlwimi	r0,r18,18,20,20		# U1
		rlwimi	r0,r18,25,21,21		# V1
		rlwimi	r0,r18,0,22,22		# W1
		rlwimi	r0,r18,7,23,23		# X1
		rlwimi	r0,r19,14,24,24		# Y1
		rlwimi	r0,r19,21,25,25		# Z1
		rlwimi	r0,r19,28,26,26		# a1
		rlwimi	r0,r19,3,27,27		# b1
		rlwimi	r0,r20,10,28,28		# c1
		rlwimi	r0,r20,17,29,29		# d1
		rlwimi	r0,r20,24,30,30		# e1
		rlwimi	r0,r20,31,31,31		# f1
		stwx	r0,r6,r3		# store to BP2+offset

		rlwimi	r21,r13,7,0,0		# A0
		rlwimi	r21,r13,14,1,1		# B0
		rlwimi	r21,r13,21,2,2		# C0
		rlwimi	r21,r13,28,3,3		# D0
		rlwimi	r21,r14,3,4,4		# E0
		rlwimi	r21,r14,10,5,5		# F0
		rlwimi	r21,r14,17,6,6		# G0
		rlwimi	r21,r14,24,7,7		# H0
		rlwimi	r21,r15,31,8,8		# I0
		rlwimi	r21,r15,6,9,9		# J0
		rlwimi	r21,r15,13,10,10	# K0
		rlwimi	r21,r15,20,11,11	# L0
		rlwimi	r21,r16,27,12,12	# M0
		rlwimi	r21,r16,2,13,13		# N0
		rlwimi	r21,r16,9,14,14		# O0
		rlwimi	r21,r16,16,15,15	# P0
		rlwimi	r21,r17,23,16,16	# Q0
		rlwimi	r21,r17,30,17,17	# r21
		rlwimi	r21,r17,5,18,18		# S0
		rlwimi	r21,r17,12,19,19	# T0
		rlwimi	r21,r18,19,20,20	# U0
		rlwimi	r21,r18,26,21,21	# V0
		rlwimi	r21,r18,1,22,22		# W0
		rlwimi	r21,r18,8,23,23		# X0
		rlwimi	r21,r19,15,24,24	# Y0
		rlwimi	r21,r19,22,25,25	# Z0
		rlwimi	r21,r19,29,26,26	# a0
		rlwimi	r21,r19,4,27,27		# b0
		rlwimi	r21,r20,11,28,28	# c0
		rlwimi	r21,r20,18,29,29	# d0
		rlwimi	r21,r20,25,30,30	# e0
		rlwimi	r21,r20,0,31,31		# f0
		stwx	r21,r5,r3		# store to BP1+offset

		addi	r4,r4,32
		addi	r3,r3,4
		bdnz	c2p_loop		# decrement ctr, branch non-zero

		lswi	r13,r1,0		# restore registers
		lwz	r21,32(r1)
		addi	r1,r1,36
		blr

