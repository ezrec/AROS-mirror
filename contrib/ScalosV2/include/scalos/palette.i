	IFND	PREFS_SCA_PALETTE_I
PREFS_SCA_PALETTE_I	SET	1
**
**	$VER: scalos_palette.i 39.0 (6.1.97)
**
**	File format for scalos_palette preferences
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
**

;---------------------------------------------------------------------------

	IFND EXEC_TYPES_I
	INCLUDE "exec/types.i"
	ENDC

	IFND LIBRARIES_IFFPARSE_I
	INCLUDE "libraries/iffparse.i"
	ENDC

;---------------------------------------------------------------------------

ID_SPAL		equ	'SPAL'
ID_PENS		equ	'PENS'

;---------------------------------------------------------------------------

    STRUCTURE ScaPalettePrefs,0
	UWORD	scl_count			; count of the following colors
	UWORD	scl_first			; first color, normaly 0
	ULONG	scl_colors []			; a long for each RGB value
    LABEL ScaPalettePrefs_SIZEOF

;the structure is very similar to the LoadRGB32 structure

;---------------------------------------------------------------------------

    STRUCTURE ScaPensPrefs,0
	UWORD	sce_count			; count of the following colors
	STRUCT	sce_pen,sp_SIZEOF []
    LABEL ScaPensPrefs_SIZEOF

    STRUCTURE ScalosPen,0
	WORD	sp_pentype			; see below
	UWORD	sp_pen				; the selected pen
    LABEL sp_SIZEOF

; constants for ScalosPen.sp_pentype
; a standard Drawinfo Pen or:

SP_HALFSHINE		equ	-1
SP_HALFSHADOW		equ	-2
SP_OUTLINE		equ	-3
SP_DRAWERTEXT		equ	-4
SP_DRAWERTEXTSEL	equ	-5
SP_DRAWERTBACKGROUND	equ	-6
SP_FILETEXT		equ	-7
SP_FILETEXTSEL		equ	-8
SP_FILEBACKGROUND	equ	-9
SP_BACKDROPDETAIL	equ	-10
SP_BACKDROPBLOCK	equ	-11

;---------------------------------------------------------------------------

	ENDC	; PREFS_SCAPALETTE_I
