**
**	Glue.asm
**
**	Stub code to invoke routines that don't accept assembly
**	language parameters
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**

	section	text,code

*-----------------------------------------------------------------------------

	xdef	_CustomStuffTextGlue
	xref	_CustomStuffText

_CustomStuffTextGlue:

	move.l	d0,-(sp)
	move.l	a3,-(sp)
	jsr	_CustomStuffText
	addq.l	#8,sp
	rts

	xdef	_CustomCountCharGlue
	xref	_CustomCountChar

_CustomCountCharGlue:

	move.l	d0,-(sp)
	move.l	a3,-(sp)
	jsr	_CustomCountChar
	addq.l	#8,sp
	rts

*-----------------------------------------------------------------------------

	xdef	_StuffCharGlue
	xref	_StuffChar

_StuffCharGlue:

	move.l	d0,-(sp)
	move.l	a3,-(sp)
	jsr	_StuffChar
	addq.l	#8,sp
	rts

*-----------------------------------------------------------------------------

	xdef	_CountCharGlue
	xref	_CountChar

_CountCharGlue:

	move.l	a3,-(sp)
	jsr	_CountChar
	addq.l	#4,sp
	rts

*-----------------------------------------------------------------------------

	xdef	_RendezvousLoginGlue
	xref	_RendezvousLogin

_RendezvousLoginGlue:

	movem.l	a0/a1/a2,-(sp)
	jsr	_RendezvousLogin
	add.l	#12,sp
	rts

	xdef	_RendezvousLogoffGlue
	xref	_RendezvousLogoff

_RendezvousLogoffGlue:

	move.l	a0,-(sp)
	jsr	_RendezvousLogoff
	addq.l	#4,sp
	rts

	xdef	_RendezvousNewNodeGlue
	xref	_RendezvousNewNode

_RendezvousNewNodeGlue:

	move.l	a0,-(sp)
	jsr	_RendezvousNewNode
	addq.l	#4,sp
	rts

*-----------------------------------------------------------------------------

	xdef	_xem_sread_glue
	xref	_xem_sread

_xem_sread_glue:

	movem.l	d0/d1,-(sp)
	move.l	a0,-(sp)
	jsr	_xem_sread
	add.l	#12,sp
	rts

	xdef	_xem_toptions_glue
	xref	_xem_toptions

_xem_toptions_glue:

	movem.l	d0/a0,-(sp)
	jsr	_xem_toptions
	addq.l	#8,sp
	rts

	xdef	_xem_swrite_glue
	xref	_xem_swrite

_xem_swrite_glue:

	move.l	d0,-(sp)
	move.l	a0,-(sp)
	jsr	_xem_swrite
	addq.l	#8,sp
	rts

	xdef	_xem_tgets_glue
	xref	_xem_tgets

_xem_tgets_glue:

	move.l	d0,-(sp)
	movem.l	a0/a1,-(sp)
	jsr	_xem_tgets
	add.l	#12,sp
	rts

	xdef	_xem_tbeep_glue
	xref	_xem_tbeep

_xem_tbeep_glue:

	movem.l	d0/d1,-(sp)
	jsr	_xem_tbeep
	addq.l	#8,sp
	rts

	xdef	_xem_macrodispatch_glue
	xref	_xem_macrodispatch

_xem_macrodispatch_glue:

	move.l	a0,-(sp)
	jsr	_xem_macrodispatch
	addq.l	#4,sp
	rts

*-----------------------------------------------------------------------------

	xdef	_xpr_fopen_glue
	xref	_xpr_fopen

_xpr_fopen_glue:

	movem.l	a0/a1,-(sp)
	jsr	_xpr_fopen
	addq.l	#8,sp
	rts

	xdef	_xpr_fclose_glue
	xref	_xpr_fclose

_xpr_fclose_glue:

	move.l	a0,-(sp)
	jsr	_xpr_fclose
	addq.l	#4,sp
	rts

	xdef	_xpr_fread_glue
	xref	_xpr_fread

_xpr_fread_glue:

	move.l	a1,-(sp)
	movem.l	d0/d1,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_fread
	add.l	#16,sp
	rts

	xdef	_xpr_fwrite_glue
	xref	_xpr_fwrite

_xpr_fwrite_glue:

	move.l	a1,-(sp)
	movem.l	d0/d1,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_fwrite
	add.l	#16,sp
	rts

	xdef	_xpr_fseek_glue
	xref	_xpr_fseek

_xpr_fseek_glue:

	movem.l	d0/d1,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_fseek
	add.l	#12,sp
	rts

	xdef	_xpr_sread_glue
	xref	_xpr_sread

_xpr_sread_glue:

	movem.l	d0/d1,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_sread
	add.l	#12,sp
	rts

	xdef	_xpr_swrite_glue
	xref	_xpr_swrite

_xpr_swrite_glue:

	move.l	d0,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_swrite
	addq.l	#8,sp
	rts

	xdef	_xpr_update_glue
	xref	_xpr_update

_xpr_update_glue:

	move.l	a0,-(sp)
	jsr	_xpr_update
	addq.l	#4,sp
	rts

	xdef	_xpr_gets_glue
	xref	_xpr_gets

_xpr_gets_glue:

	movem.l	a0/a1,-(sp)
	jsr	_xpr_gets
	addq.l	#8,sp
	rts

	xdef	_xpr_setserial_glue
	xref	_xpr_setserial

_xpr_setserial_glue:

	move.l	d0,-(sp)
	jsr	_xpr_setserial
	addq.l	#4,sp
	rts

	xdef	_xpr_ffirst_glue
	xref	_xpr_ffirst

_xpr_ffirst_glue:

	movem.l	a0/a1,-(sp)
	jsr	_xpr_ffirst
	addq.l	#8,sp
	rts

	xdef	_xpr_fnext_glue
	xref	_xpr_fnext

_xpr_fnext_glue:

	movem.l	d0/a0/a1,-(sp)
	jsr	_xpr_fnext
	add.l	#12,sp
	rts

	xdef	_xpr_finfo_glue
	xref	_xpr_finfo

_xpr_finfo_glue:

	move.l	d0,-(sp)
	move.l	a0,-(sp)
	jsr	_xpr_finfo
	addq.l	#8,sp
	rts

	xdef	_xpr_options_glue
	xref	_xpr_options

_xpr_options_glue:

	movem.l	d0/a0,-(sp)
	jsr	_xpr_options
	addq.l	#8,sp
	rts

	xdef	_xpr_unlink_glue
	xref	_xpr_unlink

_xpr_unlink_glue:

	move.l	a0,-(sp)
	jsr	_xpr_unlink
	addq.l	#4,sp
	rts

	xdef	_xpr_getptr_glue
	xref	_xpr_getptr

_xpr_getptr_glue:

	movem.l	d0,-(sp)
	jsr	_xpr_getptr
	addq.l	#4,sp
	rts

	xdef	_xpr_stealopts_glue
	xref	_xpr_stealopts

_xpr_stealopts_glue:

	movem.l	a0/a1,-(sp)
	jsr	_xpr_stealopts
	addq.l	#8,sp
	rts

*-----------------------------------------------------------------------------

	end
