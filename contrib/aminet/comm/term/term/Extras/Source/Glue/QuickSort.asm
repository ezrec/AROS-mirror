***
*
*	A replacement for Lattice's qsort
*
*	This one is a stack miser and is (hopefully) faster.
*
*	The algorithm is taken from:
*
*	Stubbs, Webre, "Data Structures with Abstract
*	Data Types and Pascal" (Pacific Grove: Brooks/Cole Pub. Co
*	p. 256.
*
*	Modifications:
*
*	Middle element of array is used as partition element.
*	Additional logic is provided for tail recursion optimization:
*	Recursive calls sort arrays at most half the size of the array
*	sorted during the previous recursive call -> little stack use.
*
***

COMPARE	macro
	move.l	\1,a0
	move.l	\2,a1
	movem.l	a0/a1,-(sp)
	jsr	(A3)
	addq.l	#8,SP
	tst.l	d0
	endm

	section	text,code

	xdef	_qsort

_qsort:
	movem.l D5/D4/D6/D7/A3,-(SP)
	move.l	36(SP),A3
	move.l	32(SP),D7
	move.l	28(SP),D4	; number of elements in array
	move.l	24(SP),D5	; bottom of array

	; calculate size of array

	move.l	D4,D1		; low word in right
	swap	D1		; high word in D1
	mulu	D7,D4
	mulu	D7,D1		; multiply
	swap	D1		; shift high portion
	clr.w	D1		; mask high bits
	add.l	D1,D4		; add partials
	sub.l	D7,D4		; back up one
	add.l	D5,D4		; make into address

	; calculate bit position to find middle

	moveq	#-1,D6
cb0:
	addq.l	#1,D6
	btst	D6,D7
	beq	cb0

	bsr	quick2

	movem.l (SP)+,D5/D4/D6/D7/A3
	rts

	;
	; exchange contents of A0, A1
	;

exch:
	move.l	D7,D1
	bra	ex1
ex2:
	move.b	(A0),D0
	move.b	(A1),(A0)+
	move.b	D0,(A1)+
ex1:
	dbra	D1,ex2
	rts

	; in: D5, right

quick2: movem.l D3/D2,-(SP)

quick2nr:
	cmp.l	D5,D4
	bls	q1		; if left < right

	; At this point, we wish to find middle of array
	; w/o multiplying or dividing.

	; After finding size of array, in bytes,
	; we test hbit.	 This bit will be set if we
	; are halfway between elements.

	move.l	D4,D1
	sub.l	D5,D1		; calculate size of array
	btst	D6,D1
	beq	q3
	sub.l	D7,D1
q3:
	lsr.l	#1,D1		; swap(d[left], d[mid]);
	add.l	D5,D1

	move.l	D1,A0
	move.l	D5,A1
	bsr	exch

	COMPARE	d5,d4
	ble	q2		; if d[left] > d[right]

	move.l	D5,A0
	move.l	D4,A1
	bsr	exch		; swap(d[left], d[right]);
q2:
	move.l	D5,D3		; j := left
	move.l	D4,D2		; k := right
q4:				; repeat
	add.l	D7,D3		;  j := j + 1
	COMPARE	d3,d5
	blt	q4		; until d[j] >= d[left]
q5:
	sub.l	D7,D2		; k := k - 1
	COMPARE	d2,d5
	bgt	q5

	cmp.l	D3,D2
	bls	q6		; if j < k
	move.l	D2,A1
	move.l	D3,A0
	bsr	exch		; swap(d[j], d[k])
q6:
	cmp.l	D3,D2
	bcc	q4		; until j > k

	move.l	D2,A1
	move.l	D5,A0
	bsr	exch		; swap(d[left], d[k])

	; this is the recursive part.  Calculate sizes of
	; recursive calls to ensure smaller array is done
	; recursively and larger non-recursively.

	move.l	D2,D3
	sub.l	D7,D2		; k - 1
	add.l	D7,D3		; k + 1

	move.l	D2,D0
	sub.l	D5,D0		; (k - 1) - left
	move.l	D4,D1
	sub.l	D3,D1		; right - (k + 1)
	cmp.l	D0,D1
	bge	q7

	move.l	D5,-(SP)
	move.l	D3,D5
	bsr	quick2		; quick2(k + 1, right)
	move.l	(SP)+,D5
	move.l	D2,D4
	bra	quick2nr	; quick2(left, k - 1)
q7:
	move.l	D4,-(SP)
	move.l	D2,D4
	bsr	quick2		; quick2(left, k - 1)
	move.l	(SP)+,D4
	move.l	D3,D5
	bra	quick2nr	; quick2(k + 1, right)
q1:
	movem.l (SP)+,D3/D2
	rts

	end
