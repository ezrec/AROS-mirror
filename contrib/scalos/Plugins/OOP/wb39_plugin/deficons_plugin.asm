; DefIcons_Plugin.asm
; 30 Dec 2001 16:12:14

	incdir	System:sc/Assembler_Headers

	include exec/types.i
	include exec/nodes.i
	include exec/ports.i
	include exec/libraries.i
	include	exec/initializers.i
	include	exec/resident.i
	include dos/dos.i
	include	utility/hooks.i
	include	scalos/scalos.i

;----------------------------------------------------------------------------

	xref	_LVORemove
	xref	_LVOFreeMem

	xref	_myHookFunc
	xref	_DefIconsInit
	xref	_DefIconsCleanup

;----------------------------------------------------------------------------

	STRUCTURE	DefIconsLib,LIB_SIZE
	ULONG		PL_PlugID
	LABEL		DefIcons_SIZE

;----------------------------------------------------------------------------

Startlib:
	moveq	#0,d0
	rts

;----------------------------------------------------------------------------

RomTag:
	dc.w	RTC_MATCHWORD	;RT_MATCHWORD
	dc.l	RomTag		;RT_MATCHTAG
	dc.l	endep		;RT_ENDSKIP
	dc.b	RTF_AUTOINIT	;RT_FLAGS
	dc.b	45		;RT_VERSION
	dc.b	NT_LIBRARY	;RT_TYPE
	dc.b	0		;RT_PRI
	dc.l	libname		;RT_NAME
	dc.l	idstring	;RT_IDSTRING
	dc.l	init		;RT_INIT

;----------------------------------------------------------------------------

libname:
	dc.b	'deficons.plugin',0
	even
idstring:
	dc.b	'$VER: deficons.plugin 45.6 (30 Dec 2001 16:12:22)',0

;----------------------------------------------------------------------------
	cnop	0,4
init:
	dc.l	DefIcons_SIZE	;BaseSize
	dc.l	functable
	dc.l	datatable
	dc.l	InitRoutine

;----------------------------------------------------------------------------
functable:
	dc.l	Open
	dc.l	Close
	dc.l	Expunge
	dc.l	Startlib
	dc.l	getclassinfo

	dc.l	-1

;----------------------------------------------------------------------------
datatable:
	INITBYTE	LN_TYPE,NT_LIBRARY
	INITLONG	LN_NAME,libname
	INITBYTE	LIB_FLAGS,LIBF_SUMUSED!LIBF_CHANGED
	INITWORD	LIB_VERSION,45
	INITWORD	LIB_REVISION,6
	INITLONG	LIB_IDSTRING,idstring
	dc.l	0

	cnop	0,4
;----------------------------------------------------------------------------

InitRoutine:
	movem.l	d0-a6,-(sp)

	move.l	a0,seglist

	move.l	d0,a0
	move.l	#'PLUG',PL_PlugID(a0)		;Store identifier for plugins into LibBase

	movem.l	(sp)+,d0-a6
	rts
.openerror:
	movem.l	(sp)+,d0-a6
	moveq	#0,d0
	rts

;----------------------------------------------------------------------------

Open:					; ( libptr:a6, version:d0 )
	addq.w	#1,(LIB_OPENCNT,a6)
	bclr	#LIBB_DELEXP,(LIB_FLAGS,a6)

	jsr	_DefIconsInit
	tst.l	d0			; Success ??
	beq	.openerror

	move.l	a6,d0
	rts

.openerror:
	sub.w	#1,(LIB_OPENCNT,a6)
	moveq	#0,d0
	rts
;----------------------------------------------------------------------------

Close:					; ( libptr:a6 )
	subq.w	#1,(LIB_OPENCNT,a6)
	bne.s	.nooneopen
	btst	#LIBB_DELEXP,(LIB_FLAGS,a6)
	beq.s	.nooneopen
	bsr.s	Expunge
.nooneopen:
	moveq	#0,d0
	rts

;----------------------------------------------------------------------------

Expunge:				; ( libptr: a6 )
	tst.w	(LIB_OPENCNT,a6)
	beq.s	.nooneopen2
	bset	#LIBB_DELEXP,(LIB_FLAGS,a6)
	moveq	#0,d0
	rts

.nooneopen2:
	jsr	_DefIconsCleanup

	movem.l	d2/a4/a5/a6,-(sp)
	move.l	seglist(pc),d2
	move.l	a6,a4
	move.l	4.w,a6
	move.l	a4,a1
	jsr	-252(a6)		;Remove
	moveq	#0,d0
	move.l	a4,a1
	move.w	(LIB_NEGSIZE,a4),d0
	sub.w	d0,a1
	add.w	(LIB_POSSIZE,a4),d0
	jsr	-210(a6)		;FreeMem

	move.l	d2,d0
	movem.l	(sp)+,d2/a4/a5/a6
	rts

;----------------------------- GetClass Info --------------------------------

;Input:  none
;Output: ScaClassInfo
getclassinfo:
	lea	classinfo(pc),a0
;	move.w	#2*4,ci_instsize(a0)
	move.l	a0,d0
	rts

classinfo:
	dc.l	0,0
	dc.l	_myHookFunc
	dc.l	0,0
	dc.w	-79			;Priority (just after IconWindow.sca !!!)
	dc.w	2*4			;Instance Size
	dc.w	39			;Needed Scalos Version
	dc.w	0			;Reserved
	dc.l	superclassname		;Name of the Class
	dc.l	superclassname		;Name of the SuperClass
	dc.l	name
	dc.l	description
	dc.l	makername

classname:	dc.b	'DefIcons.sca',0
superclassname:	dc.b	'IconWindow.sca',0
name:		dc.b	'Scalos DefIcons Class',0
description:	dc.b	'This class gives files lacking an icon a filetype dependent icon',0
makername:	dc.b	'Jürgen Lachmann',0
	even

;----------------------------------------------------------------------------

seglist:	dc.l	0
endep:

