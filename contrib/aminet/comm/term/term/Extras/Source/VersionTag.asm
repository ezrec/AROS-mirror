**
**	VersionTag.asm
**
**	Version tag identification
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**

	include	"term_rev.i"

*--------------------------------------------------------------------------

	section	text,code

*--------------------------------------------------------------------------

	xref	_Start

	jmp	_Start

*--------------------------------------------------------------------------

	cnop	0,4

	xdef	_VersTag

_VersTag:

	dc.b	0,'$VER: '
	VERS
	dc.b	' ('
	DATE
	dc.b	')'

	IFD	CPU_ANY
	dc.b	' Generic 68k version'
	ELSE
	dc.b	' 68020/030/040/060 version'
	ENDC

	dc.b	13,10,0

*--------------------------------------------------------------------------

	section	data,data

*--------------------------------------------------------------------------

	xdef	_TermVersion

_TermVersion:

	dc.l	VERSION

	xdef	_TermRevision

_TermRevision:

	dc.l	REVISION

	xdef	_TermName

_TermName:

	VERS
	dc.b	0

	cnop	0,4

	xdef	_TermDate

_TermDate:

	DATE
	dc.b	0

	end
