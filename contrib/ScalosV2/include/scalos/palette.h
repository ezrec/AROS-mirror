#ifndef PREFS_SCA_PALETTE_H
#define PREFS_SCA_PALETTE_H
/*
**	$VER: scalos_palette.h 39.0 (6.1.97)
**
**	File format for scalos_palette preferences
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

#define ID_SPAL		MAKE_ID('S','P','A','L')
#define ID_PENS		MAKE_ID('P','E','N','S')

// ---------------------------------------------------------------------------

struct ScaPalettePrefs {
	UWORD	scl_count;			// count of the following colors
	UWORD	scl_first;			// first color, normaly 0
	ULONG	scl_colors[];		// a long for each RGB value
};

// the structure is very similar to the LoadRGB32 structure

// ---------------------------------------------------------------------------

struct ScaPensPrefs {
	UWORD					sce_count;	// count of the following colors
	struct ScalosPen	sce_pen[];
}

struct ScalosPen {
	WORD	sp_pentype;					// see below
	UWORD	sp_pen;						// the selected pen
}

//  constants for ScalosPen.sp_pentype
//  a standard Drawinfo Pen or:

#define SP_HALFSHINE				-1
#define SP_HALFSHADOW			-2
#define SP_OUTLINE				-3
#define SP_DRAWERTEXT			-4
#define SP_DRAWERTEXTSEL		-5
#define SP_DRAWERTBACKGROUND	-6
#define SP_FILETEXT				-7
#define SP_FILETEXTSEL			-8
#define SP_FILEBACKGROUND		-9
#define SP_BACKDROPDETAIL		-10
#define SP_BACKDROPBLOCK		-11

// ---------------------------------------------------------------------------

#endif /* PREFS_SCAPALETTE_H */
