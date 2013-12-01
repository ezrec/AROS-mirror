	IFND	PREFS_SCA_PATTERN_I
PREFS_SCA_PATTERN_I	SET	1
**
**	$VER: scalos_pattern.i 40.32 (12 Sep 2004 22:31:48)
**
**	File format for scalos_pattern preferences
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	(C) Copyright 2000-2004 The Scalos Team
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

ID_PATT		EQU	'PATT'
ID_DEFS		EQU	'DEFS'
ID_TCOL		equ	'TCOL'
ID_TBMP		equ	'TBMP'

// ---------------------------------------------------------------------------

// default pattern numbers

SHRT_MAX			set	32767

PATTERNNR_WorkbenchDefault	equ	SHRT_MAX-1
PATTERNNR_IconWindowDefault	equ	SHRT_MAX-2
PATTERNNR_TextWindowDefault	equ	SHRT_MAX-3

// ---------------------------------------------------------------------------

// TCOL structure

    STRUCTURE ScaPatternPrefsThumbnailColors,0
	UWORD		stc_NumColors		; Number of entries in color table
						; always <= 256
	UBYTE		stc_ColorTable;		; color table, contains <stc_NumColors> entries
						; with 3 x UBYTE each
    LABEL ScaPatternPrefsThumbnailColors_SIZEOF

; ---------------------------------------------------------------------------

; TBMP structure

    STRUCTURE ScaPatternPrefsBitMap,0
	UWORD		spb_Width;		; BitMap width
	UWORD		spb_Height;		; BitMap height

	UBYTE		spb_BitMapArray;	; Array for BitMap pen numbers, sized 
						; spb_Width * spb_Height bytes
						; ready to use for WritePixelArray8()
    LABEL ScaPatternPrefsBitMap_SIZEOF

;---------------------------------------------------------------------------

;PATT Structure

    STRUCTURE ScaPatternPrefs,0
	UWORD	scp_Number			; Which pattern is it
	UWORD	scp_RenderType			; render tyoe (see below)
	UWORD	scp_Flags			; see below
	UWORD	scp_NumColors			; number of colors to use for
						; remapping (enhanced mode)
	UWORD	scp_DitherMode			; render.library dithermode
	UWORD	scp_DitherAmount		; render.library ditheramount
	BYTE	scp_Precision			; Pen Precision (OBP_Precision)
	UBYTE	scp_Type			; Type of Pattern (see below)
	UWORD	scp_Reserved1
	ULONG	scp_Reserved2
	STRUCT	scp_Name,512			; Filename or Pattern
    LABEL ScaPatternPrefs_SIZEOF


; scp_Flags values
    BITDEF SCP,NOREMAP,0
    BITDEF SCP,ENHANCED,1

; scp_RenderType values
SCP_RenderType_Tiled		EQU	0
SCP_RenderType_FitSize		EQU	1
SCP_RenderType_Centered		EQU	2
SCP_RenderType_ScaledMin	EQU	3       ; similar to SCP_RenderType_FitSize,
						; but keeps aspect of original image.
						; Can leave empty stripes on window top/bottom or left/right borders
SCP_RenderType_ScaledMax	EQU	4       ; similar to SCP_RenderType_FitSize,
						; but keeps aspect of original image.
						; Creates no empty stripes on window borders,
						; but stripes of image may fall outside of window

; scp_Type values
SCP_BgType_Picture		EQU	0	; Picture only, no background color
SCP_BgType_SingleColor		EQU	1	; Optional Picture with single background color
SCP_BgType_HorizontalGradient	EQU	2	; Optional Picture with horizontal gradient
SCP_BgType_VerticalGradient	EQU	3	; Optional Picture with vertical gradient

;---------------------------------------------------------------------------

;DEFS Structure

    STRUCTURE ScaPatternDefs,0
	UWORD	scd_Flags			;see below
	UWORD	scd_WorkbenchPattern		;def PatternNum for the Main-
						;window
	UWORD	scd_ScreenPattern		;def PatternNum for the Screen
	UWORD	scd_WindowPattern		;def PatternNum for windows
	UWORD	scd_TextModePattern		;def PatternNum for Textmode
	UBYTE	scd_TaskPriority		;asyncron taskpri
	UBYTE	scd_Reserved
	LABEL	ScaPatternDefs_SIZEOF

;scd_Flags
    BITDEF SCD,ASYNCLAYOUT,0
    BITDEF SCD,USEFRIENDBM,1
    BITDEF SCD,RELAYOUT,2
    BITDEF SCD,RANDOM,3

;---------------------------------------------------------------------------

	ENDC	; PREFS_SCAPATTERN_I
