#ifndef PREFS_SCA_PALETTE_H
#define PREFS_SCA_PALETTE_H
/*
**	$VER: scalos_palette.h 40.33 (14 Apr 2005 18:56:51)
**
** $Date$
** $Revision$
**
**	File format for scalos_palette preferences
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

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

#define ID_SPAL		MAKE_ID('S','P','A','L')
#define ID_PENS		MAKE_ID('P','E','N','S')

// ---------------------------------------------------------------------------

struct ScaPalettePrefs {
	UWORD	scl_count;		// count of the following colors
	UWORD	scl_first;		// first color, normaly 0
	ULONG	scl_colors[1];		// a long for each RGB value
};

// the structure is very similar to the LoadRGB32 structure

// ---------------------------------------------------------------------------

struct ScalosPen {
	WORD	sp_pentype;		// see below
	UWORD	sp_pen;			// the selected pen
	};


struct ScaPensPrefs {
	UWORD			sce_count;	// count of the following colors
	struct ScalosPen	sce_pen[1];
	};


//  constants for ScalosPen.sp_pentype
//  a standard Drawinfo Pen or:

#define SP_HALFSHINE			-1
#define SP_HALFSHADOW			-2
#define SP_OUTLINE			-3
#define SP_DRAWERTEXT			-4
#define SP_DRAWERTEXTSEL		-5
#define SP_DRAWERTBACKGROUND		-6
#define SP_FILETEXT			-7
#define SP_FILETEXTSEL			-8
#define SP_FILEBACKGROUND		-9
#define SP_BACKDROPDETAIL		-10
#define SP_BACKDROPBLOCK		-11
#define SP_TOOLTIP_TEXT			-12
#define SP_TOOLTIP_BG			-13
#define SP_DRAGINFOTEXT_TEXT		-14
#define SP_DRAGINFOTEXT_BG		-15

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* PREFS_SCAPALETTE_H */
