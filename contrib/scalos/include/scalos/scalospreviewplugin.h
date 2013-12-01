#ifndef SCALOS_PREVIEWPLUGIN_H
#define SCALOS_PREVIEWPLUGIN_H
/*
**	$VER: scalospreviewplugin.h 41.1 (28 Apr 2006 19:34:08)
**
** $Date$
** $Revision$
**
**	Special methods and tags for Scalos menu preferences plugin
**
**   (C) Copyright 2006 The Scalos Team
**	All Rights Reserved
*/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

#define	SCALOSPREVIEW_Tagbase			0x84722fff

// ---------------------------------------------------------------------------

/*** Methods ***/

// ---------------------------------------------------------------------------

/*** Method structs ***/

// ---------------------------------------------------------------------------

/*** Special method values ***/

// ---------------------------------------------------------------------------

/*** Attributes ***/

#define SCALOSPREVIEW_ThumbnailWidth	(SCALOSPREVIEW_Tagbase + 101)	// ULONG
#define SCALOSPREVIEW_ThumbnailHeight	(SCALOSPREVIEW_Tagbase + 102)	// ULONG
#define SCALOSPREVIEW_Screen		(SCALOSPREVIEW_Tagbase + 103)	// struct Screen *
#define SCALOSPREVIEW_SupportedColors	(SCALOSPREVIEW_Tagbase + 104)	// ULONG
#define SCALOSPREVIEW_ResultStruct	(SCALOSPREVIEW_Tagbase + 105)	// struct ScalosPreviewResult *
#define SCALOSPREVIEW_ImgBitMapHeader	(SCALOSPREVIEW_Tagbase + 106)	// struct BitMapHeader *
#define SCALOSPREVIEW_Quality		(SCALOSPREVIEW_Tagbase + 107)	// ULONG [0..15]
#define SCALOSPREVIEW_ReservedColors	(SCALOSPREVIEW_Tagbase + 108)	// ULONG

// ---------------------------------------------------------------------------

/*** Special attribute values ***/

#define SCALOSPREVIEWA_Quality_Min	0
#define SCALOSPREVIEWA_Quality_Max	15

// ---------------------------------------------------------------------------

/*** Structures, Flags & Values ***/

struct ScalosPreviewResult
	{
	struct ARGBHeader spvr_ARGBheader;	// true-color thumbnail (> 256 colors)
	struct ScalosBitMapAndColor *spvr_sac;	// remapped thumbnail (<= 256 colors)
	};

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* SCALOS_PREVIEWPLUGIN_H */
