#ifndef PREFS_SCA_PATTERN_H
#define PREFS_SCA_PATTERN_H
/*
**	$VER: scalos_pattern.h 39.2 (03.06.97)
**
**	File format for scalos_pattern preferences
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
*/

// ---------------------------------------------------------------------------

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

// ---------------------------------------------------------------------------

#define ID_PATT		MAKE_ID('P','A','T','T')
#define ID_DEFS		MAKE_ID('D','E','F','S')

// ---------------------------------------------------------------------------

// PATT Structure

struct ScaPatternPrefs {
	UWORD		scp_Number;				// Which pattern is it
	UWORD		scp_RenderType;		// render tyoe (see below)
	UWORD		scp_Flags;				// see below
	UWORD		scp_NumColors;			// number of colors to use for
											// remapping (enhanced mode)
	UWORD		scp_DitherMode;		// render.library dithermode
	UWORD		scp_DitherAmount;		// render.library ditheramount
	BYTE		scp_Precision;			// Pen Precision (OBP_Precision)
	UBYTE		scp_Type;				// Type of Pattern (see below)
	UWORD		scp_Reserved1;
	ULONG		scp_Reserved2;
	char		scp_Name[512];			// Filename or Pattern
};

//  scp_Flags values
#define SCPF_NOREMAP					(1L<<0)
#define SCPF_ENHANCED				(1L<<1)

//  scp_RenderType values
#define SCP_RenderType_Tiled		0
#define SCP_RenderType_FitSize	1
#define SCP_RenderType_Centered	2

//  scp_Type
#define SCP_Type_Picture			0

// ---------------------------------------------------------------------------

// DEFS Structure

struct ScaPatternDefs {
	UWORD		scd_Flags;					// see below
	UWORD		scd_WorkbenchPattern		// def PatternNum for the Main-
												// window
	UWORD		scd_ScreenPattern;		// def PatternNum for the Screen
	UWORD		scd_WindowPattern;		// def PatternNum for windows
	UWORD		scd_TextModePattern;		// def PatternNum for Textmode
	UBYTE		scd_TaskPriority;			// asyncron taskpri
};

// scd_Flags
#define SCDF_ASYNCLAYOUT		(1L<<0)
#define SCDF_USEFRIENDBM		(1L<<1)
#define SCDF_RELAYOUT			(1L<<2)
#define SCDF_RANDOM				(1L<<3)

// ---------------------------------------------------------------------------

#endif /* PREFS_SCAPATTERN_H */
