/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

/******************************************************************************
** Add-Routines ***************************************************************
******************************************************************************/

/* ppc

LONG	  Samples		r3     long		Samples
LONG	  ScaleLeft		r4     long		ScaleLeft
LONG	  ScaleRight		r5     long		ScaleRight
LONG	 *StartPointLeft	r6     long		StartPointLeft
LONG	 *StartPointRight	r7     long		StartPointRight
void	 *Src			r8     long		Src
void	**Dst			r9     long		Dst
LONG	  FirstOffsetI		r10    long		FirstOffsetI
Fixed64	  Add			 8(r1) long long	Add
Fixed64	 *Offset		16(r1) long		Offset
BOOL	  StopAtZero		20(r1) word		StopAtZero

*/

AddI		.equ	64 + 8
AddF		.equ	64 + 12
Offset		.equ	64 + 16
StopAtZero	.equ	64 + 20

	.text

	.globl	AddByteMono
	.globl	AddByteStereo
	.globl	AddBytesMono
	.globl	AddBytesStereo
	.globl	AddWordMono
	.globl	AddWordStereo
	.globl	AddWordsMono
	.globl	AddWordsStereo
	.globl	AddByteMonoB
	.globl	AddByteStereoB
	.globl	AddBytesMonoB
	.globl	AddBytesStereoB
	.globl	AddWordMonoB
	.globl	AddWordStereoB
	.globl	AddWordsMonoB
	.globl	AddWordsStereoB

	.type	AddByteMono,@function
	.type	AddByteStereo,@function
	.type	AddBytesMono,@function
	.type	AddBytesStereo,@function
	.type	AddWordMono,@function
	.type	AddWordStereo,@function
	.type	AddWordsMono,@function
	.type	AddWordsStereo,@function
	.type	AddByteMonoB,@function
	.type	AddByteStereoB,@function
	.type	AddBytesMonoB,@function
	.type	AddBytesStereoB,@function
	.type	AddWordMonoB,@function
	.type	AddWordStereoB,@function
	.type	AddWordsMonoB,@function
	.type	AddWordsStereoB,@function

        .align  2

AddSilenceMono:
	mullw	r14,r18,r3		# (add:high * samples) : low
	mullw	r15,r19,r3		# (add:low * samples) : low 
	add	r16,r16,r14
	addc	r17,r17,r15
	mulhwu	r14,r19,r3		# (add:low * samples) : high 
	adde	r16,r16,r14

	slwi	r15,r3,2
	add	r20,r20,r15
	li	r14,0
	mtctr	r14
	blr

        .align  2

AddSilenceStereo:
	mullw	r14,r18,r3		# (add:high * samples) : low
	mullw	r15,r19,r3		# (add:low * samples) : low 
	add	r16,r16,r14
	addc	r17,r17,r15
	mulhwu	r14,r19,r3		# (add:low * samples) : high 
	adde	r16,r16,r14

	slwi	r15,r3,3
	add	r20,r20,r15
	li	r14,0
	mtctr	r14
	blr

        .align  2

AddSilenceMonoB:
	mullw	r14,r18,r3		# (add:high * samples) : low
	mullw	r15,r19,r3		# (add:low * samples) : low 
	sub	r16,r16,r14
	subc	r17,r17,r15
	mulhwu	r14,r19,r3		# (add:low * samples) : high 
	subfe	r16,r14,r16

	slwi	r15,r3,2
	add	r20,r20,r15
	li	r14,0
	mtctr	r14
	blr

        .align  2

AddSilenceStereoB:
	mullw	r14,r18,r3		# (add:high * samples) : low
	mullw	r15,r19,r3		# (add:low * samples) : low 
	sub	r16,r16,r14
	subc	r17,r17,r15
	mulhwu	r14,r19,r3		# (add:low * samples) : high 
	subfe	r16,r14,r16

	slwi	r15,r3,3
	add	r20,r20,r15
	li	r14,0
	mtctr	r14
	blr


/*

Register usage:

r3	samples
r4	scaleleft
r5	scaleright
r6	last left startpoint
r7	last right startpoint
r8	src
(r9	dst*)
r10	firstoffset
r13	&src[ offset ], temp
r14	left sample #1
r15	right sample #1
r16	int offset
r17	fract offset
r18	int add
r19	fract add
r20	dst
r21	left sample #2
r22	right sample #2
r23	left lastpoint
r24	right lastpoint
r25	temp

*/

	.macro	prelude

	stwu	r1,-64(r1)
	mflr	r0
	stw	r0,68(r1)

	stw	r13,8(r1)
	stw	r14,12(r1)
	stw	r15,16(r1)
	stw	r16,20(r1)
	stw	r17,24(r1)
	stw	r18,28(r1)
	stw	r19,32(r1)
	stw	r20,36(r1)
	stw	r21,40(r1)
	stw	r22,44(r1)
	stw	r23,48(r1)
	stw	r24,52(r1)
	stw	r25,56(r1)

	lwz	r14,Offset(r1)
	lwz	r18,AddI(r1)
	lwz	r19,AddF(r1)
	lwz	r16,0(r14)
	lwz	r17,4(r14)

	lwz	r20,0(r9)
	subi	r20,r20,4

	li	r23,0
	li	r24,0

	mtctr	r3			# Number of loop times to CTR

	.endm


	.macro	postlude

	mfctr	r4
	sub	r3,r3,r4

	lwz	r13,8(r1)
	lwz	r14,12(r1)
	lwz	r15,16(r1)
	lwz	r16,20(r1)
	lwz	r17,24(r1)
	lwz	r18,28(r1)
	lwz	r19,32(r1)
	lwz	r20,36(r1)
	lwz	r21,40(r1)
	lwz	r22,44(r1)
	lwz	r23,48(r1)
	lwz	r24,52(r1)
	lwz	r25,56(r1)

	lwz	r0,68(r1)
	mtlr	r0
	addi	r1,r1,64

	.endm

###############################################################################

        .align  2

AddByteMono:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMono
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lbz	r14,-1(r13)		# Fetch src[ offset - 1 ]
	lbz	r21,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
4:	# .got_sampleZ
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lbz	r14,-1(r13)		# Fetch src[ offset - 1 ]
	lbz	r21,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
4:	# .got_sample
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	add	r13,r8,r16		# (Calculate &src[ offset ])
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	stw	r14,0(r6)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddBytesMono:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMono
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r22,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lbz	r14,-2(r13)		# Fetch src[ offset - 2 ] (left)
	lbz	r15,-1(r13)		# Fetch src[ offset - 1 ] (right)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
4:	# .got_sampleZ
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r22,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lbz	r14,-2(r13)		# Fetch src[ offset - 2 ] (left)
	lbz	r15,-1(r13)		# Fetch src[ offset - 1 ] (right)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
4:	# .got_sample
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddByteStereo:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereo
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lbz	r14,-1(r13)		# Fetch src[ offset - 1 ]
	lbz	r21,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
4:	# .got_sampleZ
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lbz	r14,-1(r13)		# Fetch src[ offset - 1 ]
	lbz	r21,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
4:	# .got_sample
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	add	r13,r8,r16		# (Calculate &src[ offset ])
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	stw	r14,0(r6)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddBytesStereo:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereo
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r22,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lbz	r14,-2(r13)		# Fetch src[ offset - 2 ] (left)
	lbz	r15,-1(r13)		# Fetch src[ offset - 1 ] (right)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
4:	# .got_sampleZ
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r22,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lbz	r14,-2(r13)		# Fetch src[ offset - 2 ] (left)
	lbz	r15,-1(r13)		# Fetch src[ offset - 1 ] (right)
	lbz	r21,0(r13)		# Fetch src[ offset ] (left)
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
4:	# .got_sample
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordMono:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMono
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lhz	r14,-2(r13)		# Fetch src[ offset - 1 ]
	lhz	r21,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
4:	# .got_sampleZ
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lhz	r14,-2(r13)		# Fetch src[ offset - 1 ]
	lhz	r21,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
4:	# .got_sample
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
	stw	r14,0(r6)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordsMono:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMono
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r22,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lhz	r14,-4(r13)		# Fetch src[ offset - 2 ] (left)
	lhz	r15,-2(r13)		# Fetch src[ offset - 1 ] (right)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
4:	# .got_sampleZ
	extsh	r21,r21
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r22,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lhz	r14,-4(r13)		# Fetch src[ offset - 2 ] (left)
	lhz	r15,-2(r13)		# Fetch src[ offset - 1 ] (right)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
4:	# .got_sample
	extsh	r21,r21
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordStereo:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereo
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lhz	r14,-2(r13)		# Fetch src[ offset - 1 ]
	lhz	r21,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
4:	# .got_sampleZ
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lhz	r14,-2(r13)		# Fetch src[ offset - 1 ]
	lhz	r21,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
4:	# .got_sample
	extsh	r21,r21

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
	stw	r14,0(r6)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordsStereo:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereo
	b	7f

0:	# .next_sampleZ
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r22,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lhz	r14,-4(r13)		# Fetch src[ offset - 2 ] (left)
	lhz	r15,-2(r13)		# Fetch src[ offset - 1 ] (right)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
4:	# .got_sampleZ
	extsh	r21,r21
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r14,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r15,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r22,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lhz	r14,-4(r13)		# Fetch src[ offset - 2 ] (left)
	lhz	r15,-2(r13)		# Fetch src[ offset - 1 ] (right)
	lhz	r21,0(r13)		# Fetch src[ offset ] (left)
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
4:	# .got_sample
	extsh	r21,r21
	extsh	r22,r22

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	addc	r17,r19,r17		# Add fraction
	adde	r16,r18,r16		# Add integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddByteMonoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMonoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lbz	r14,0(r13)
	b	4f
3:	# .not_firstZ
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ]
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
4:	# .got_sampleZ
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ]
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
4:	# .got_sample
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	add	r13,r8,r16		# (Calculate &src[ offset ])
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	stw	r14,0(r6)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr


###############################################################################

        .align  2

AddBytesMonoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMonoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lbz	r21,2(r13)		# Fetch src[ offset + 2 ] (left)
	lbz	r22,3(r13)		# Fetch src[ offset + 3 ] (right)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22
4:	# .got_sampleZ
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lbz	r21,2(r13)		# Fetch src[ offset + 2 ] (left)
	lbz	r21,3(r13)		# Fetch src[ offset + 3 ] (right)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22
4:	# .got_sample
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddByteStereoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ]
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
4:	# .got_sampleZ
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	add	r13,r8,r16		# (Calculate &src[ offset ])
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lbz	r21,1(r13)		# Fetch src[ offset + 1 ]
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
4:	# .got_sample
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	add	r13,r8,r16		# (Calculate &src[ offset ])
	lbz	r14,0(r13)		# Fetch src[ offset ]
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	stw	r14,0(r6)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddBytesStereoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lbz	r21,2(r13)		# Fetch src[ offset + 2 ] (left)
	lbz	r22,3(r13)		# Fetch src[ offset + 3 ] (right)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22
4:	# .got_sampleZ
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lbz	r21,2(r13)		# Fetch src[ offset + 2 ] (left)
	lbz	r22,3(r13)		# Fetch src[ offset + 3 ] (right)
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r21,r21,8		# Normalize...
	extsh	r21,r21
	slwi	r22,r22,8		# Normalize...
	extsh	r22,r22
4:	# .got_sample
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lbz	r14,0(r13)		# Fetch src[ offset ] (left)
	lbz	r15,1(r13)		# Fetch src[ offset + 1 ] (right)
	slwi	r14,r14,8		# Normalize...
	extsh	r14,r14
	slwi	r15,r15,8		# Normalize...
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordMonoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMonoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ]
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r21,r21
4:	# .got_sampleZ
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ]
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r21,r21
4:	# .got_sample
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r14,r14,r4		# Volume scale
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
	stw	r14,0(r6)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr


###############################################################################

        .align  2

AddWordsMonoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceMonoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lhz	r21,4(r13)		# Fetch src[ offset + 2 ] (left)
	lhz	r22,6(r13)		# Fetch src[ offset + 3 ] (right)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r21,r21
	extsh	r22,r22
4:	# .got_sampleZ
	extsh	r14,r14
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lhz	r21,4(r13)		# Fetch src[ offset + 2 ] (left)
	lhz	r21,6(r13)		# Fetch src[ offset + 3 ] (right)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r21,r21
	extsh	r22,r22
4:	# .got_sample
	extsh	r14,r14
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	add	r14,r15,r14
	stwu	r14,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordStereoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_firstZ
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ]
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r21,r21
4:	# .got_sampleZ
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ]
	b	4f
3:	# .not_first
	lhz	r21,2(r13)		# Fetch src[ offset + 1 ]
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r21,r21
4:	# .got_sample
	extsh	r14,r14

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	srawi	r21,r21,15
	add	r14,r14,r21

	mullw	r13,r14,r4		# Volume scale (left)
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++
	mullw	r13,r14,r5		# Volume scale (right)
	lwz	r25,4(r20)		# Fetsh *dst
	add	r13,r13,r25
	stwu	r13,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,1		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ]
	extsh	r14,r14
	stw	r14,0(r6)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

###############################################################################

        .align  2

AddWordsStereoB:
	prelude

	lhz	r13,StopAtZero(r1)	# Test StopAtZero
	cmpwi	cr0,r13,0
	bne+	1f
	cmpwi	cr0,r4,0		# Test if volume == 0
	bne+	2f
	bl	AddSilenceStereoB
	b	7f

0:	# .next_sampleZ
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
1:	# .first_sampleZ
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_firstZ
	lwz	r21,4(r13)		# Fetch src[ offset + 2 ] (left)
	lwz	r22,6(r13)		# Fetch src[ offset + 3 ] (right)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r21,r21
	extsh	r22,r22
4:	# .got_sampleZ
	extsh	r14,r14
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	cmpwi	cr0,r23,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r14,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r14,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r23,r14			# Update lastsample

	cmpwi	cr0,r24,0
	bgt	5f
	beq	6f
	cmpwi	cr0,r15,0
	bge	7f
	b	6f
5:	# .lastpoint_gtZ
	cmpwi	cr0,r15,0
	ble	7f
6:	# .lastpoint_checkedZ
	mr	r24,r15			# Update lastsample

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

0:	# .next_sample
	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer
2:	# .first_sample
	cmp	cr0,1,r16,r10		# Offset == FirstOffset?
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	bne+	3f
	lwz	r21,0(r6)		# Fetch left lastpoint (normalized)
	lwz	r22,0(r7)		# Fetch right lastpoint (normalized)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	b	4f
3:	# .not_first
	lhz	r21,4(r13)		# Fetch src[ offset + 2 ] (left)
	lhz	r22,6(r13)		# Fetch src[ offset + 3 ] (right)
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r21,r21
	extsh	r22,r22
4:	# .got_sample
	extsh	r14,r14
	extsh	r15,r15

	srwi	r13,r17,17		# Get linear high word / 2
	sub	r21,r21,r14
	sub	r22,r22,r15
	mullw	r21,r13,r21		# Linear interpolation
	lwz	r25,4(r20)		# Fetsh *dst
	mullw	r22,r13,r22		# Linear interpolation
	srawi	r21,r21,15
	add	r14,r14,r21
	srawi	r22,r22,15
	add	r15,r15,r22

	mullw	r14,r14,r4		# Volume scale (left)
	mullw	r15,r15,r5		# Volume scale (right)
	add	r14,r14,r25
	stwu	r14,4(r20)		# Store to *dst, dst++
	lwz	r25,4(r20)		# Fetsh *dst
	add	r15,r15,r25
	stwu	r15,4(r20)		# Store to *dst, dst++

	bdnz	0b
	b	8f

7:	# .abort
	li	r18,0
	li	r19,0
8:	# .exit
	slwi	r13,r16,2		# (Calculate &src[ offset ])
	add	r13,r8,r13
	lhz	r14,0(r13)		# Fetch src[ offset ] (left)
	lhz	r15,2(r13)		# Fetch src[ offset + 1 ] (right)
	extsh	r14,r14
	extsh	r15,r15
	stw	r14,0(r6)
	stw	r15,0(r7)

	subfc	r17,r19,r17		# Subtract fraction
	subfe	r16,r18,r16		# Subtract integer

	lwz	r13,Offset(r1)
	stw	r16,0(r13)
	stw	r17,4(r13)

	addi	r20,r20,4
	stw	r20,0(r9)

	postlude
	blr

	.end
