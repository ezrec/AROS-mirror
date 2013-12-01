#ifndef PREFS_SCA_PATTERN_H
#define PREFS_SCA_PATTERN_H
/*
**	$VER: scalos_pattern.h 40.33 (14 Apr 2005 22:27:54)
**
** $Date$
** $Revision$
**
**	File format for scalos_pattern preferences
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 2000-2005 The Scalos Team
**	All Rights Reserved
*/

// ---------------------------------------------------------------------------

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

#include <limits.h>

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

#define ID_PATT		MAKE_ID('P','A','T','T')
#define ID_DEFS		MAKE_ID('D','E','F','S')
#define ID_TCOL		MAKE_ID('T','C','O','L')
#define	ID_TBMP		MAKE_ID('T','B','M','P')

// ---------------------------------------------------------------------------

// default pattern numbers

#define	PATTERNNR_WorkbenchDefault	(SHRT_MAX-1)
#define	PATTERNNR_IconWindowDefault	(SHRT_MAX-2)
#define	PATTERNNR_TextWindowDefault	(SHRT_MAX-3)

// ---------------------------------------------------------------------------

// TCOL structure

struct ScaPatternPrefsThumbnailColors
	{
	UWORD		stc_NumColors;		// Number of entries in color table
						// always <= 256
	UBYTE		stc_ColorTable[0];	// color table, contains <stc_NumColors> entries
						// with 3 x UBYTE each
	};

// ---------------------------------------------------------------------------

// TBMP structure

struct ScaPatternPrefsBitMap
	{
	UWORD		spb_Width;		// BitMap width
	UWORD		spb_Height;		// BitMap height

	UBYTE		spb_BitMapArray[0];	// Array for BitMap pen numbers, sized 
						// spb_Width * spb_Height bytes
						// ready to use for WritePixelArray8()
	};

// ---------------------------------------------------------------------------

// PATT Structure

struct ScaPatternPrefs 
{
	UWORD		scp_Number;		// Which pattern is it
	UWORD		scp_RenderType;		// render type (see below)
	UWORD		scp_Flags;		// see below
	UWORD		scp_NumColors;		// number of colors to use for
						// remapping (enhanced mode)
	UWORD		scp_DitherMode;		// render.library dithermode
	UWORD		scp_DitherAmount;	// render.library ditheramount
	BYTE		scp_Precision;		// Pen Precision (OBP_Precision)
	UBYTE		scp_Type;		// Type of Pattern (see below)

	UBYTE		scp_color1[3];		// rrggbb values for background color (SCP_RenderType_Centered)
	UBYTE		scp_color2[3];		// rrggbb values for second background color
};

struct ScaExtPatternPrefs 
{
	struct ScaPatternPrefs	scxp_PatternPrefs;
	char			scxp_Name[512];		// Filename or Pattern
};

//  scp_Flags values
#define SCPF_NOREMAP				(1L<<0)
#define SCPF_ENHANCED				(1L<<1)
#define	SCPF_AUTODITHER				(1L<<2)

//  scp_RenderType values
enum ScpRenderType
	{
	SCP_RenderType_Tiled = 0,
	SCP_RenderType_FitSize,
	SCP_RenderType_Centered,
	SCP_RenderType_ScaledMin,		// similar to SCP_RenderType_FitSize,
						// but keeps aspect of original image.
						// Can leave empty stripes on window top/bottom or left/right borders
	SCP_RenderType_ScaledMax,		// similar to SCP_RenderType_FitSize,
						// but keeps aspect of original image.
						// Creates no empty stripes on window borders,
						// but stripes of image may fall outside of window
	};

// scp_DitherMode values
#ifndef DITHERMODE_NONE
#define	DITHERMODE_NONE			0
#define	DITHERMODE_FS			1
#define	DITHERMODE_RANDOM		2
#endif /* DITHERMODE_NONE */

//  scp_Type
enum ScpBgType
	{
	SCP_BgType_Picture = 0,			// Picture only, no background color
	SCP_BgType_SingleColor,			// Optional Picture with single background color
	SCP_BgType_HorizontalGradient,          // Optional Picture with horizontal gradient
	SCP_BGType_VerticalGradient,		// Optional Picture with vertical gradient
	};

// ---------------------------------------------------------------------------

// DEFS Structure

struct ScaPatternDefs 
{
	UWORD		scd_Flags;			// see below
	UWORD		scd_WorkbenchPattern;		// def PatternNum for the Main window
	UWORD		scd_ScreenPattern;		// def PatternNum for the Screen
	UWORD		scd_WindowPattern;		// def PatternNum for windows
	UWORD		scd_TextModePattern;		// def PatternNum for Textmode
	BYTE		scd_TaskPriority;		// asynchronous taskpri
	UBYTE		scd_Reserved[1];
};

// scd_Flags
#define SCDB_ASYNCLAYOUT		0
#define SCDF_ASYNCLAYOUT		(1L<<SCDB_ASYNCLAYOUT)
#define SCDB_USEFRIENDBM		1
#define SCDF_USEFRIENDBM		(1L<<SCDB_USEFRIENDBM)
#define SCDB_RELAYOUT			2
#define SCDF_RELAYOUT			(1L<<SCDB_RELAYOUT)
#define SCDB_RANDOM			3
#define SCDF_RANDOM			(1L<<SCDB_RANDOM)

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* PREFS_SCAPATTERN_H */
