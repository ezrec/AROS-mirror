	IFND	PREFS_SCA_PATTERN_I
PREFS_SCA_PATTERN_I	SET	1
**
**	$VER: scalos_pattern.i 39.2 (03.06.97)
**
**	File format for scalos_pattern preferences
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

ID_PATT		EQU	'PATT'
ID_DEFS		EQU	'DEFS'

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

; scp_Type
SCP_Type_Picture		EQU	0

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
	LABEL	ScaPatternDefs_SIZEOF

;scd_Flags
    BITDEF SCD,ASYNCLAYOUT,0
    BITDEF SCD,USEFRIENDBM,1
    BITDEF SCD,RELAYOUT,2
    BITDEF SCD,RANDOM,3

;---------------------------------------------------------------------------

	ENDC	; PREFS_SCAPATTERN_I
