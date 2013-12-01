; Scalos_Macros.i
; 30 Aug 2001 20:47:16

tsta   MACRO
	IFNE	mode020
	tst.l	\1
	ELSE
	cmp.l	#0,'\1'
	ENDC
	ENDM

tstpc:	MACRO
	IFNE	mode020
	tst.\0	\1(pc)
	ELSE
	tst.\0	\1
	ENDC
	ENDM

bral:	MACRO
	IFNE	mode020
	bra.l	\1
	ELSE
	jmp \1
	ENDC
	ENDM

bsrl:	MACRO
	IFNE	mode020
	bsr.l	\1
	ELSE
	jsr \1
	ENDC
	ENDM

bnel:	MACRO
	IFNE	mode020
	bne.l	\1
	ELSE
	beq.s	skip\@ 
	jmp \1
skip\@:
	ENDC
	ENDM

DoMethod    MACRO
	movem.l d1/a0-a1/a6,-(a7)
	move.l	a7,a6
	clr.l	-(a7)
	ifge	NARG-15
	move.l	\f,-(a7)
	endc
	ifge	NARG-14
	move.l	\e,-(a7)
	endc
	ifge	NARG-13
	move.l	\d,-(a7)
	endc
	ifge	NARG-12
	move.l	\c,-(a7)
	endc
	ifge	NARG-11
	move.l	\b,-(a7)
	endc
	ifge	NARG-10
	move.l	\a,-(a7)
	endc
	ifge	NARG-9
	move.l	\9,-(a7)
	endc
	ifge	NARG-8
	move.l	\8,-(a7)
	endc
	ifge	NARG-7
	move.l	\7,-(a7)
	endc
	ifge	NARG-6
	move.l	\6,-(a7)
	endc
	ifge	NARG-5
	move.l	\5,-(a7)
	endc
	ifge	NARG-4
	move.l	\4,-(a7)
	endc
	ifge	NARG-3
	move.l	\3,-(a7)
	endc
	ifge	NARG-2
	move.l	\2,-(a7)
	endc
	ifge	NARG-1
	move.l	\1,-(a7)
	endc
	move.l	a7,a1
	jsr _DoMethod
	move.l	a6,a7
	movem.l (a7)+,d1/a0-a1/a6
	ENDM

cmv3	MACRO
	ifge	NARG-1
	move.l	'\1',-(a7)
	ENDC
	ENDM

;+++;30 = mu_sizeof
;+++GTMENU_USERDATA MACRO
;+++	move.l	30(\1),\2
;+++	ENDM
;+++
;+++;34 = mi_sizeof
;+++GTMENUITEM_USERDATA MACRO
;+++	move.l	34(\1),\2
;+++	ENDM

seti:	MACRO
	move.l	\2,d0
	move.l	\3,d1
	move.l	\1,a0
	bsr.s	setattr
	ENDM

geti:	MACRO
	move.l	\1,a0
	move.l	\2,d0
	bsr.s	getattr
	ENDM

TRUE	EQU 1
FALSE	EQU 0


call	MACRO
;	xref	_LVO\1
	jsr	_LVO\1(a6)
	ENDM

RETURNSUPER MACRO
	movem.l (a7)+,d1-a6
	move.l	(cl_Super,a0),a0
	move.l	(cl_Dispatcher+h_Entry,a0),-(a7)
	rts
	ENDM

CALLSUPER   MACRO
	movem.l a0-a3,-(a7)
	move.l	(cl_Super,a0),a0
	move.l	(cl_Dispatcher+h_Entry,a0),a3
	jsr	(a3)
	movem.l (a7)+,a0-a3
	ENDM

STARTCLASS  MACRO
	movem.l d1-a6,-(a7)
	lea	\1,a3
	move.l	(a1),d1
.loop:
	move.l	(a3)+,d2
	beq.s	.loopend
	cmp.l	d2,d1
	beq.s	.found
	addq.w	#4,a3
	bra.s	.loop
.found:
	move.l	(a3)+,a3
	jsr	(a3)		; +jl+ 20010830
	movem.l (a7)+,d1-a6
	rts

.loopend:
	movem.l (a7)+,d1-a6
	move.l	24(a0),a0
	move.l	8(a0),-(a7)
	rts
	ENDM

STARTMETHOD MACRO
	movem.l d1-a6,-(sp)	; +jl+ 20010830
	move.w	(cl_InstOffset,a0),d6
	lea	(a2,d6.w),a4	; instance data
	move.l	(a2),a5		; ScaWindowTask
	lea	4(a1),a3	; skip MethodID
	ENDM

STARTMETHODNEW	MACRO
	movem.l d1-a6,-(sp)	; +jl+ 20010830
	ENDM

ENDMETHOD   MACRO
	movem.l (a7)+,d1-a6
	rts
	ENDM

DBSTRING    MACRO
	movem.l a0/a1,-(a7)
	move.l	\1,-(a7)
	move.l	a7,a1
	lea debugstring,a0
	jsr dboutput
	addq.w	#4,a7
	movem.l (a7)+,a0/a1
	ENDM

DBNUMBER    MACRO
	movem.l a0/a1,-(a7)
	move.l	\1,-(a7)
	move.l	a7,a1
	lea debugnumber,a0
	jsr dboutput
	addq.w	#4,a7
	movem.l (a7)+,a0/a1
	ENDM
