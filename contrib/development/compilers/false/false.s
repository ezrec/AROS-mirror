; FALSE compiler in a 1020 bytes executable!
; Wouter van Oortmerssen, 1993, v1.1
; - fixed free twice bug in v1.0

maxsource	= 25000
maxcode		= 30000
maxmem		= maxsource+maxcode+5000

k:	clr.b	-1(a0,d0.w)
	move.l	a0,d5			; d5=temparg
	move.l	4.w,a6
	moveq	#0,d1
	moveq	#1,d0
	swap	d0			; move.l #maxmem,d0
	jsr	-684(a6)		; allocvec
	move.l	d0,d6			; d6=tempmem
	beq	nfrer
	move.l	d6,a2			; a2=mem
	move.l	d6,a4			; a4=code
	lea	maxsource(a4),a4
	move.l	a4,d7			; d7=begin_of_code
	lea	maxcode(a4),a3		; a3=lambda-stack
	lea	dosn(pc),a1
	moveq	#37,d0
	jsr	-552(a6)		; opendoslib
	tst.l	d0
	beq.w	ncler
	move.l	d0,a6			; a6=dosbase
	move.l	d5,d1
	move.l	#1005,d2
	jsr	-30(a6)			; open(arg)
	tst.l	d0
	beq.w	er
	move.l	d0,d5			; d5=temphandle
	move.l	d0,d1
	move.l	d6,d2
	move.l	#maxsource,d3
	jsr	-42(a6)			; readfile
	move.l	d0,d6
	move.l	d5,d1
	jsr	-36(a6)			; close
	cmp.w	#1,d6
	bmi.w	er
	add.l	d2,d6			; d6=end_of_source
	move.l	d2,a5			; a5=source
	lea	initc(pc),a0
	lea	inite(pc),a1
	bsr	copy

loop:	cmp.l	d6,a5			; main loop
	bpl.w	done
	moveq	#0,d0
	move.b	(a5)+,d0
	cmp.w	#"0",d0
	bpl.w	num
numb:	cmp.w	#"[",d0			; lambda
	bne.s	.1
	move.l	#$41fa0008,(a4)+	;	lea	...(pc),a0
	move.w	#$2b08,(a4)+		;	move.l	a0,-(a5)
	move.l	#$6000fff8,(a4)+	;	bra	...
	move.l	a4,(a3)+
	bra.s	loop
.1:	cmp.w	#"]",d0			; end of lambda
	bne.s	.2
	move.w	#$4e75,(a4)+
	move.l	-(a3),a0
	move.l	a4,d0
	sub.l	a0,d0
	addq.l	#2,d0
	move.w	d0,-2(a0)
	bra.s	loop
.2:	cmp.w	#"a",d0			; variable
	bpl.w	var
varb:	cmp.w	#" "+1,d0
	bmi.s	loop
	cmp.w	#"{",d0
	bne.s	.1
.l:	cmp.l	d6,a5			; collect comments
	bpl.w	done
	cmp.b	#"}",(a5)+
	bne.s	.l
	bra.s	loop
.1:	cmp.w	#34,d0
	bne.s	.2
	move.l	#$41fa0006,(a4)+	; gen code for string
	move.l	#$60000000,(a4)+
	move.l	a4,a0
.l2:	cmp.l	d6,a5
	bpl.s	.f
	cmp.b	#34,(a5)
	beq.s	.f
	move.b	(a5)+,(a4)+
	bra.s	.l2
.f:	addq.l	#1,a5
	clr.b	(a4)+
	move.l	a4,d0
	btst	#0,d0
	beq.s	.even
	clr.b	(a4)+
.even:	move.l	a4,d0
	sub.l	a0,d0
	addq.l	#2,d0
	move.w	d0,-2(a0)
	move.l	#$22084eae,(a4)+
	move.w	#$fc4c,(a4)+
	bra.w	loop
.2:	cmp.w	#"'",d0			; ascii
	bne.s	.3
	moveq	#0,d0
	move.b	(a5)+,d0
	move.w	#$2b3c,(a4)+
	move.l	d0,(a4)+
	bra.w	loop
.3:	cmp.w	#"`",d0			; inline assembler :-)
	bne.s	.4
	move.l	-(a4),d0
	subq.l	#2,a4
	move.w	d0,(a4)+
	bra.w	loop
.4:
	lea	plus(pc),a0
search:	cmp.b	2(a0),d0		; generate code for symbol
	bne.s	next
	addq.l	#3,a0
	moveq	#0,d1
	move.b	(a0)+,d1
.cl:	move.w	(a0)+,(a4)+
	dbra	d1,.cl
	bra.w	loop
next:	move.w	(a0),d1
	beq.w	er
	add.w	d1,a0
	bra.s	search

done:	lea	exitc(pc),a0
	lea	exite(pc),a1
	bsr	copy
	move.l	a4,d0			; ready, now write executable.
	btst	#1,d0
	beq.s	.s
	clr.w	(a4)+
.s:	move.l	a4,d0
	sub.l	d7,d0
	sub.l	#32,d0
	lsr.l	#2,d0
	move.l	#1010,(a4)+		; hunk_end
	move.l	d7,a0
	move.l	d0,20(a0)		; hunk lenght
	move.l	d0,28(a0)
	lea	exen(pc),a0
	move.l	a0,d1			; now write exe
	move.l	#1006,d2
	jsr	-30(a6)			; open
	move.l	d0,d4
	beq	er
	move.l	d0,d1
	move.l	d7,d2
	move.l	a4,d3
	sub.l	d7,d3
	jsr	-48(a6)			; write
	move.l	d4,d1
	jsr	-36(a6)			; close

close:	move.l	a6,a1
	move.l	4.w,a6
	jsr	-414(a6)
	bsr.s	ncler
	moveq	#0,d0
	rts

er:	bsr.s	close
	bra.s	nfrer
ncler:	move.l	a2,a1
	jsr	-690(a6)
nfrer:	moveq	#10,d0			; error exit point
	rts

num:	cmp.w	#"9"+1,d0		; code gen voor num
	bpl.w	numb
	moveq	#0,d1
.l:	mulu	#10,d1
	sub.w	#"0",d0
	add.l	d0,d1
	cmp.b	#"0",(a5)
	bmi.s	.x
	cmp.b	#"9"+1,(a5)
	bpl.s	.x
	moveq	#0,d0
	move.b	(a5)+,d0
	bra.s	.l
.x:	move.w	#$2b3c,(a4)+		; move.l #x,-(a5)
	move.l	d1,(a4)+
	bra.w	loop

var:	cmp.w	#"z"+1,d0		; code gen voor vars
	bpl.w	varb
	sub.w	#"a",d0
	lsl.w	#2,d0
	move.w	#$41ec,(a4)+		; lea x(a4),a0
	move.w	d0,(a4)+
	move.w	#$2b08,(a4)+		; move.l a0,-(a5)
	bra.w	loop

copy:	move.l	a1,d0
	sub.l	a0,d0
	lsr.l	#1,d0
	subq.l	#1,d0
.l:	move.w	(a0)+,(a4)+
	dbra	d0,.l
	rts

; code-gen list: implementation for all other symbols.

plus:	dr.w	min
	dc.b	"+",1
	move.l	(a5)+,d0
	add.l	d0,(a5)
min:	dr.w	mul
	dc.b	"-",1
	move.l	(a5)+,d0
	sub.l	d0,(a5)
mul:	dr.w	div
	dc.b	"*",3
	move.l	(a5)+,d0
	muls	2(a5),d0
	move.l	d0,(a5)
div:	dr.w	printi
	dc.b	"/",4
	move.l	(a5)+,d0
	move.l	(a5),d1
	divs	d0,d1
	ext.l	d1
	move.l	d1,(a5)
printi:	dr.w	exe
	dc.b	".",9
	bra.s	.c
.form:	dc.b	"%ld",0
.c:	lea	.form(pc),a0
	move.l	a0,d1
	move.l	a5,d2
	jsr	-954(a6)
	addq.l	#4,a5
exe:	dr.w	store
	dc.b	"!",1
	move.l	(a5)+,a0
	jsr	(a0)
store:	dr.w	get
	dc.b	":",1
	move.l	(a5)+,a0
	move.l	(a5)+,(a0)
get:	dr.w	equal
	dc.b	";",1
	move.l	(a5)+,a0
	move.l	(a0),-(a5)
equal:	dr.w	if
	dc.b	"=",5
	move.l	(a5)+,d0
	cmp.l	(a5),d0
	seq	d0
	ext.w	d0
	ext.l	d0
	move.l	d0,(a5)
if:	dr.w	dup
	dc.b	"?",3
	move.l	(a5)+,a0
	move.l	(a5)+,d0
	beq.s	.f
	jsr	(a0)
.f:
dup:	dr.w	drop
	dc.b	"$",0
	move.l	(a5),-(a5)
drop:	dr.w	swap
	dc.b	"%",0
	addq.l	#4,a5
swap:	dr.w	rot
	dc.b	"\",4
	move.l	(a5),d0
	move.l	4(a5),(a5)
	move.l	d0,4(a5)
rot:	dr.w	neg
	dc.b	"@",7
	move.l	8(a5),d0
	move.l	4(a5),8(a5)
	move.l	(a5),4(a5)
	move.l	d0,(a5)
neg:	dr.w	and
	dc.b	"_",0
	neg.l	(a5)
and:	dr.w	or
	dc.b	"&",1
	move.l	(a5)+,d0
	and.l	d0,(a5)
or:	dr.w	not
	dc.b	"|",1
	move.l	(a5)+,d0
	or.l	d0,(a5)
not:	dr.w	big
	dc.b	"~",0
	not.l	(a5)
big:	dr.w	while
	dc.b	">",5
	move.l	(a5)+,d0
	cmp.l	(a5),d0
	smi	d0
	ext.w	d0
	ext.l	d0
	move.l	d0,(a5)
while:	dr.w	put
	dc.b	"#",12
	movem.l	(a5)+,a0/a1		; fun,boolf
	movem.l	a0/a1,-(a7)
.s:	move.l	4(a7),a0
	jsr	(a0)
	tst.l	(a5)+
	beq.s	.e
	move.l	(a7),a0
	jsr	(a0)
	bra.s	.s
.e:	addq.l	#8,a7
put:	dr.w	getc
	dc.b	",",3
	move.l	d6,d1
	move.l	(a5)+,d2
	jsr	-312(a6)
getc:	dr.w	pick
	dc.b	"^",3
	move.l	d5,d1
	jsr	-306(a6)
	move.l	d0,-(a5)
pick:	dr.w	flush
	dc.b	"ø",3
	move.l	(a5)+,d0
	lsl.l	#2,d0
	move.l	0(a5,d0.l),-(a5)
flush:	dc.w	0
	dc.b	"ß",5
	move.l	d5,d1
	jsr	-360(a6)
	move.l	d6,d1
	jsr	-360(a6)

exen:	dc.b	"a.out",0
	even

; initialisation code.

initc:	dc.l	1011			; hunkheader
	dc.l	0,1,0,0 		; endofnames,1hunk,firsthunkno,lasthn
	dc.l	0			; codesize/4
	dc.l	1001			; hunkcode
	dc.l	0			; codesize/4
	lea	-$900(a7),a5		; a5=calcstack
	lea	-$800(a7),a4		; a4=variables
	move.l	a0,(a4)			; var_a=arg
	move.l	4.w,a6
	lea	dosn(pc),a1
	moveq	#37,d0
	jsr	-552(a6)		; opendoslib
	tst.l	d0
	bne.s	.o
	rts
.o:	move.l	d0,a6
	jsr	-60(a6)
	move.l	d0,d6			; d6=stdout
	jsr	-54(a6)
	move.l	d0,d5			; d5=stdin
	bra.s	inite
dosn:	dc.b	"dos.library",0
inite:
exitc:	move.l	a6,a1
	move.l	4.w,a6
	jsr	-414(a6)
	moveq	#0,d0
	rts
exite:
