; June 19, 1995
; This program is a pair of subroutines I use to generate random
; number is assembly langauge.  The first subroutine is called at
; the start of the program to initialize the seeds to unique values.
; This is done using the currenttime function of intuition.  Thus,
; S2 which is set to the microseconds value has a 1 in 1,000,000
; chance of starting at the same value as any given previous run.
; S1 should always be unique.
; The second subroutine is based on the algorithm of Pierre L'Ecuyer
; [L'Ecuyer, P. Efficient and Portable Combined Random Number Generators.
; _Commun._ASM_31_, 6 (June, 1988) 742].  Please see the original
; article for a detailed explanation.
;
; These programs were written to be used by any 68000 type processor
; and can be assembled using the a68k assembler.  They can be incorp-
; orated into programs directly or by linking the object code if
; setseed, randnum, intubase, and mathbase are xdef'ed in this routine
; and xref'ed in the main program.
; Distribution:  These routines are intended as freeware and their
; distribution is restricted as follows:
;
; Source code:  The source code must be accompanied by all comments
;		include the introductory comments and the program comments.
;		No fee may be charged for distribution of the code.
; Executable code:  The executable version of these routines may be used if:
;		1) The documentation of the main program includes the
;		   following text:
;		   "This program uses the L'Ecuyer random number generator
;		    as written for the amiga by Scott McMahan."
;		2) The main program using these routines is either freeware
;		   or the author(s) have the express, written consent of me,
;		   Scott McMahan.
;		   (No big deal here, I just don't want to pay for shareware
;		   that uses my routine.  Permission will be easy to get.)
;
;To contact the author and/or make bug reports, mail me at
;       Scott McMahan
;       2538 Fairfield Pl #1
;       Madison, WI 53704
; Sorry, no Email because I expect my email address to be changing in
; the next 6 months and then again in the next 2 years.

CurrentTime	equ	-$0054
IEEEDPFlt	equ	-$0024
IEEEDPDiv	equ	-$0054

	;Routine to set up S1 and S2 (seeds for random number generator)
	;based on currenttime function
	;parameters:			none
	;presets:			intubase=	base address of
	;						intuition.library
	;				CurrentTime=	offset for currenttime
	;						function of intuition
	;returns:				none
	;changed regs/mem locations:	none/S1,S2
	section text,code
	xdef	@setseed
@setseed:
;	movem.l A0/A1,-(A7)		;save registers	******
	lea	S1(A4),A0		;get the current seconds and
	lea	S2(A4),A1		;microseconds as seeds
	move.l	_IntuitionBase(A4),A6
	jsr	CurrentTime(A6)		;in S1 and S2
	addq.l	#1,S1(A4)		;add one to S1 and S2 to make sure greater than 0
	addq.l	#1,S2(A4)
	cmpi.l	#$7FFFFFAA,S1(A4)	;make sure S1<=2147483562
	ble.S	20$
	subi.l	#$7FFFFFAA,S1(A4)
20$	cmpi.l	#$7FFFFF06,S2(A4)	;make sure S2<=2147483398
	ble.S	30$
	subi.l	#$7FFFFF06,S2(A4)
30$ ;	movem.l (A7)+,A0/A1		******
	rts

	;Routine to generate a random number from 0 to 1
	;using S1 and S2 as seeds
	;Parameters:			S1 and S2	Seeds
	;presets			mathbase =	base address of
	;						mathieeedoubbas.library
	;				IEEEDPFlt=	Offset of int to IEEE
	;						routine
	;				IEEEDPDiv=	Offset of IEEE division
	;						routine
	;Returns:			D0/D1 =	Random number from 0 to 1
	;					in IEEE double precision
	;Changed regs/mem locations	D0,D1/S1,S2

	xdef	@random
;	section rand,code		******
@random	movem.l D2/D3/A6,-(A7)
	move.l	S1(A4),D1
	divu	#$D1A4,D1		;D1=S1 DIV 53668 (k in article)
	move.l	D1,D0			;D0 also equals k
	swap	D1
	mulu	#$9C4E,D1		;D1=40014*(S1-k*53668)
	mulu	#$2FB3,D0		;D0=12211*k
	sub.l	D0,D1			;D1=40014*(S1-k*53668)-12211*k
;	tst.l	D1			******
	bgt.S	20$		;if D1<1
	add.l	#$7FFFFFAB,D1		;then D1=D1+2147483563
20$	move.l	D1,S1(A4)		;store D2 in S1 as new seed
	move.l	S2(A4),D1
	divu	#$CE26,D1		;D1=S2 DIV 52774 (k in article)
	move.l	D1,D0			;D0 also equals k
	swap	D1
	mulu	#$9EF4,D1		;D1=40692*(S1-k*52774)
	mulu	#$0ECF,D0		;D0=3791*k
	sub.l	D0,D1			;D1=40692*(S1-k*52774)-3791*k
;	tst.l	D1
	bgt.S	30$		;if D1<1
	add.l	#$7FFFFF07,D1		;then D1=D1+2147483399
30$	move.l	D1,S2(A4)		;store D2 in S2 as new seed
	move.l	S1(A4),D0
	sub.l	S2(A4),D0		;D0=S1-S2
;	tst.l	D0			******
	bge.S	40$		;if D0<0
	add.l	#$7FFFFFA9,D0		;then D0=D0+2147483562
40$	move.l	_MathIeeeDoubBasBase(A4),A6
	jsr	IEEEDPFlt(A6)		;convert it to IEEE Double Precision
	move.l	#$41DFFFFF,D2		;
	move.l	#$EA800000,D3		;
	jsr	IEEEDPDiv(A6)		;Divide it by 2147483562
	movem.l (A7)+,D2/D3/A6
	rts

	section __MERGED,BSS
;	even				******
	xref	_IntuitionBase
	xref	_MathIeeeDoubBasBase

S1:	ds.l	1			;first seed for generator, set to seconds by
					;currenttime
S2:	ds.l	1			;second seed for generator, set to microsecs
					;by currenttime
	end
