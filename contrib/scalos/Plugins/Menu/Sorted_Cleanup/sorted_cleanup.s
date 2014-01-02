; sorted_cleanup.s
; Scalos Sorted Cleanup PlugIn
; $Date$
; $Revision$

;---------------------------------------------------------------
	machine	68020

	incdir	"SC:Assembler_Headers"
	incdir	"Scalos:Src/scalos/main/std_includes/asm"

	NOLIST		; skip includes in listing

	include	"exec/types.i"
	include	"exec/macros.i"
	include	"exec/resident.i"
	include	"exec/initializers.i"
	include	"workbench/workbench.i"
	include	"scalos/iconobject.i"
	include	"scalos/scalos.i"
	include	"scalos/undo.i"
	include	"scalos_macros.i"
	include "lvo/scalos_lib.i"
	include	"asmsupp.i"

	LIST

INFO_LEVEL	equ	0

;---------------------------------------------------------------

Startlib:
	moveq	#0,d0
	rts

;---------------------------------------------------------------

RomTag:
	dc.w	RTC_MATCHWORD	;RT_MATCHWORD
	dc.l	RomTag		;RT_MATCHTAG
	dc.l	endep		;RT_ENDSKIP
	dc.b	RTF_AUTOINIT	;RT_FLAGS
	dc.b	39		;RT_VERSION
	dc.b	9		;RT_TYPE
	dc.b	0		;RT_PRI
	dc.l	libname		;RT_NAME
	dc.l	idstring	;RT_IDSTRING
	dc.l	init		;RT_INIT

	cnop	0,4

;---------------------------------------------------------------

libname:
	dc.b	'sorted_cleanup.plugin',0
	even
idstring:
	dc.b	'$VER: sorted_cleanup.plugin 41.1 (23 Oct 2009 21:16:50)',10
	dc.b	'(c) 2002-2009 The Scalos Team',0

	cnop	0,4

;---------------------------------------------------------------

init:
	dc.l	LIB_SIZE
	dc.l	functable
	dc.l	datatable
	dc.l	InitRoutine

functable:
	dc.l	Open
	dc.l	Close
	dc.l	Expunge
	dc.l	Startlib
	dc.l	menufunction

	dc.l	-1

datatable:
	INITBYTE	LN_TYPE,NT_LIBRARY
	INITLONG	LN_NAME,libname
	INITBYTE	LIB_FLAGS,LIBF_SUMUSED!LIBF_CHANGED
	INITWORD	LIB_VERSION,39
	INITWORD	LIB_REVISION,4
	INITLONG	LIB_IDSTRING,idstring
	dc.l	0

	cnop	0,4

;---------------------------------------------------------------

InitRoutine:
	movem.l	d0-a6,-(sp)
	move.l	a0,seglist
	move.l	4.w,a6
	lea	scaname(pc),a1
	moveq	#39,d0
	JSRLIB	OpenLibrary
	move.l	d0,scabase
	beq.s	.openerror
	lea	utilname(pc),a1
	moveq	#39,d0
	JSRLIB	OpenLibrary
	move.l	d0,utilbase
	lea	intname(pc),a1
	moveq	#39,d0
	JSRLIB	OpenLibrary
	move.l	d0,intbase

	movem.l	(sp)+,d0-a6
	rts
.openerror:
	movem.l	(sp)+,d0-a6
	moveq	#0,d0
	rts

;---------------------------------------------------------------

Open:					; ( libptr:a6, version:d0 )
	addq.w	#1,(LIB_OPENCNT,A6)
	bclr	#LIBB_DELEXP,(LIB_FLAGS,A6)
	move.l	a6,d0
	rts

;---------------------------------------------------------------

Close:					; ( libptr:a6 )
	subq.w	#1,(LIB_OPENCNT,A6)
	bne.s	.nooneopen
	btst	#LIBB_DELEXP,(LIB_FLAGS,A6)
	beq.s	.nooneopen
	bsr.s	Expunge
.nooneopen:
	moveq	#0,d0
	rts

;---------------------------------------------------------------

Expunge:				; ( libptr: a6 )
	tst.w	(LIB_OPENCNT,A6)
	beq.s	.nooneopen2
	bset	#LIBB_DELEXP,(LIB_FLAGS,A6)
	moveq	#0,d0
	rts

;---------------------------------------------------------------

.nooneopen2:
	movem.l	d2/a4/a5/a6,-(sp)
	move.l	seglist(pc),d2
	move.l	a6,a4
	move.l	4.w,a6
	move.l	a4,a1
	JSRLIB	Remove
	moveq	#0,d0
	move.l	a4,a1
	move.w	(LIB_NEGSIZE,A4),D0
	suba.w	D0,A1
	add.w	(LIB_POSSIZE,A4),D0
	JSRLIB	FreeMem

	move.l	scabase(pc),a1
	JSRLIB	CloseLibrary
	move.l	utilbase(pc),a1
	JSRLIB	CloseLibrary

	move.l	d2,d0
	movem.l	(sp)+,d2/a4/a5/a6
	rts

;----------------------------- GetClass Info --------------------------------

;Input: A0 = WindowTask, A1 = ScaIconNode or NULL
;Ouput: none
menufunction:
	movem.l	d1-a6,-(a7)
	move.l	a0,a5

	lea		(RedoHook,pc),a0
	DoMethod	(mt_MainObject,a5),#SCCM_IconWin_AddUndoEvent,#UNDO_Cleanup,#UNDOTAG_IconList,(wt_IconList,a5),#UNDOTAG_WindowTask,a5,#UNDOTAG_CleanupMode,#CLEANUP_Default,#UNDOTAG_RedoHook,a0,#TAG_END

menufunction2:
	move.l	4.w,a6
	move.l	(wt_IconSemaphore,a5),a0
	JSRLIB	ObtainSemaphore

	move.l	(wt_LateIconList,a5),a2
	move.l	scabase(pc),a6
.loop:
	tst.l	a2
	beq.s	.loopend

	lea	(wt_LateIconList,a5),a0
	lea	(wt_IconList,a5),a1
	move.l	a2,d0
	JSRLIB2	SCA_MoveNode
	move.l	(LN_SUCC,a2),a2
	bra.s	.loop
.loopend:
	move.l	(wt_IconList,a5),(wt_LateIconList,a5)
	clr.l	(wt_IconList,a5)
	lea	(wt_LateIconList,a5),a0
	lea	CompareNameHook(pc),a1
	moveq	#SCA_SortType_Best,d0
	JSRLIB2	SCA_SortNodes

	move.l	4.w,a6
	move.l	(wt_IconSemaphore,a5),a0
	JSRLIB	ReleaseSemaphore

	clr.l	(wt_XOffset,a5)
	move.l	(mt_WindowStruct,a5),a0
	move.l	(wt_XOffset,a5),(ws_xoffset,a0)

	DoMethod	(mt_MainObject,a5),#SCCM_IconWin_CleanUp
	DoMethod	(mt_MainObject,a5),#SCCM_IconWin_SetVirtSize,#SETVIRTF_AdjustRightSlider|SETVIRTF_AdjustBottomSlider
	DoMethod	(mt_MainObject,a5),#SCCM_IconWin_Redraw,#0

	movem.l	(a7)+,d1-a6
	rts

;---------------------------------------------------------------

; special entry for menufunction, without executing SCCM_IconWin_AddUndoEvent
InternalRedoFunc:
	movem.l	d1-a6,-(a7)
	move.l	a0,a5
	bra     menufunction2

;---------------------------------------------------------------
	
CompareNameHook:
	dc.l	0,0
	dc.l	comparenamefunc
	dc.l	0,0

;---------------------------------------------------------------

RedoHook:
	dc.l	0,0
	dc.l    RedoFunc
	dc.l	0,0

;---------------------------------------------------------------

; a0 : hook	(unused)
; a2 : object	(unused)
; a1 : message	(UndoEvent)
RedoFunc:
	movem.l	d1-a6,-(a7)


	IFGE	INFO_LEVEL-15
		move.l	a1,-(SP)
		move.l	a2,-(SP)
		move.l	a0,-(SP)
		PUTMSG	15,<'%s/RedoFunc  hook=%08lx  obj=%08lx  msg=%08lx'>
		add.l	#3*4,sp
	ENDC

	move.l	a1,a3		; UndoEvent
	move.l  uev_Data+ucd_WindowTask(a3),a0	;WindowTask
	CLEARA	a1

	IFGE	INFO_LEVEL-15
		move.l	a0,-(SP)
		move.l	a3,-(SP)
		PUTMSG	15,<'%s/RedoFunc  WindowTask=%08lx  UndoEvent=%08lx'>
		add.l	#3*4,sp
	ENDC

	bsr     InternalRedoFunc

	movem.l	(a7)+,d1-a6
	rts

;---------------------------------------------------------------

; a0 : hook
; a2 : object
; a1 : message
comparenamefunc:
	move.l	a6,-(a7)
	tst.l	intbase
	beq.s	.noint
	move.l	a1,-(a7)
	clr.l	-(a7)
	move.l	#IDTA_Type,d0
	move.l	(in_Icon,a1),a0
	move.l	a7,a1
	move.l	intbase(pc),a6
	JSRLIB	GetAttr
	clr.l	-(a7)
	move.l	#IDTA_Type,d0
	move.l	(in_Icon,a2),a0
	move.l	a7,a1
	JSRLIB	GetAttr
	move.l	(a7)+,d0
	move.l	(a7)+,d1
	cmp.l	#WBDRAWER,d0			;Drawer ?
	beq.s	.firstdir
	cmp.l	#WBDRAWER,d1			;Drawer ?
	bne.s	.files
	bra.s	.greater
.firstdir:
	cmp.l	#WBDRAWER,d1			;Drawer ?
	bne.s	.smaller
.files:	move.l	(a7)+,a1
.noint:	move.l	(in_Name,a2),a0
	move.l	(in_Name,a1),a1
	tst.l	a0
	beq	.NullName
	tst.l	a1
	beq	.NullName
	move.l	utilbase(pc),a6
	JSRLIB	Stricmp
	move.l	(a7)+,a6
	rts
.NullName:
	moveq	#1,d0
	move.l	(a7)+,a6
	rts
.greater:
	moveq	#1,d0
	bra.s	.end
.smaller:
	moveq	#-1,d0
.end:	addq.w	#4,a7
	move.l	(a7)+,a6
	rts


;--------------------------- DoMethod --------------------------------------

_DoMethod:			;Standard-Function !! DON'T REMOVE !!
	movem.l	d1/a0-a2/a6,-(a7)
	move.l	(a1)+,a2
	cmp.w	#0,a2
	beq.s	.do
	move.l	-4(a2),a0
	move.l	8(a0),a6
	jsr	(a6)
	movem.l	(a7)+,d1/a0-a2/a6
	rts
.do:	moveq	#0,d0
	movem.l	(a7)+,d1/a0-a2/a6
	rts

;----------------------------------------------------------------------------

seglist:	dc.l	0
scabase:	dc.l	0
utilbase:	dc.l	0
intbase:	dc.l	0

scaname:	dc.b	'scalos.library',0
utilname:	dc.b	'utility.library',0
intname:	dc.b	'intuition.library',0

;	this is the name that the device will have
subSysName:
		IFGE	INFO_LEVEL-1
		dc.b	'.',0		; Dummy, für PUTMSG
		ENDC

endep:

