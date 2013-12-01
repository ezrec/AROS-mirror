#ifndef SCALOS_SCALOSGFX_H
#define SCALOS_SCALOSGFX_H
/*
**  $VER: scalosgfx.h 41.1 (21 Mar 2006 09:05:36)
**
**	scalosgfx.library include
**
**  (C) Copyright 2006 The Scalos Team
**  All Rights Reserved
*/

#ifndef	 EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef	 INTUITION_IMAGECLASS_H
#include <intuition/imageclass.h>
#endif

#ifndef	 DATATYPES_DATATYPESCLASS_H
#include <datatypes/datatypesclass.h>
#endif


//----------------------------------------------------------------------------

#define	SCALOSGFXNAME	"scalosgfx.library"

#define	gfxARGB		ARGB

// Width for TempRp.BitMap for ReadPixelLine8() and WritePixelLine8()
#define	TEMPRP_WIDTH(width)	(8 * ((((width) + 15) >> 4) << 1))

#define	PIXELARRAY8_WIDTH(width) \
	((((width) + 15) >> 4) << 4)

// buffer size for ReadPixelArray8() and WritePixelArray8()
#define	PIXELARRAY8_BUFFERSIZE(width, height) \
	(PIXELARRAY8_WIDTH(width) * ((height) + 1))

// check equality of 2 ARGB variables
#define	ARGB_EQ(a, b)	(*((ULONG *) &(a)) == *((ULONG *) &(b)))

// convert 8 bit to 32 bit R/G/B palette value
#define	RGB_8_TO_32(val)	((((UBYTE)(val)) << 24) \
				| (((UBYTE)(val)) << 16) \
				| (((UBYTE)(val)) << 8) \
				| ((UBYTE)(val)))

// get number of bytes in sac_ColortTable
#define SAC_COLORTABLESIZE(sac)	((sac)->sac_NumColors * 3 * sizeof(ULONG))

//---------------------------------------------------------------

#define	SCALOSGFX_TAGBASE               0x80537000

//---------------------------------------------------------------

// Flag values for ScalosGfxScaleBitMap()

#define SCALEFLAGB_BILINEAR    		0
#define SCALEFLAGB_AVERAGE     		1
#define SCALEFLAGB_DOUBLESIZE  	  	2
#define SCALEFLAGB_CORRECTASPECT      	3
#define SCALEFLAGB_BICUBIC		4

#define SCALEFLAGF_BICUBIC 		(1L << SCALEFLAGB_BICUBIC)
#define SCALEFLAGF_BILINEAR 		(1L << SCALEFLAGB_BILINEAR)
#define SCALEFLAGF_AVERAGE  		(1L << SCALEFLAGB_AVERAGE)
#define SCALEFLAGF_DOUBLESIZE  		(1L << SCALEFLAGB_DOUBLESIZE)
#define SCALEFLAGF_CORRECTASPECT	(1L << SCALEFLAGB_CORRECTASPECT)

/*----------------------------------------------------------------------------------*/

/* ARGB data */

struct ARGB
	{
	UBYTE	Alpha;
	UBYTE	Red;
	UBYTE	Green;
	UBYTE	Blue;
	};

struct ARGBHeader
	{
	ULONG argb_Width;
	ULONG argb_Height;
	struct ARGB *argb_ImageData;
	};

/*----------------------------------------------------------------------------------*/

/* combined BitMap and color map data
  - parameter for IDTM_NewNormalImage and IDTM_NewSelectedImage */
struct ScalosBitMapAndColor
	{
	ULONG sac_Width;
	ULONG sac_Height;
	ULONG sac_Depth;
	struct BitMap *sac_BitMap;
	ULONG sac_TransparentColor;	// number of pen for transparent color
	ULONG sac_NumColors;		// number of entries in sac_ColorTable
	ULONG *sac_ColorTable;		// color table, 3 longwords per entry
	ULONG sac_Flags;		// Flags, see below
	};

// set sac_TransparentColor to this value for no transparency
#define	SAC_TRANSPARENT_NONE	((ULONG) ~0)

// value in sac_Flags
#define	SACFLAGB_NO_FREE_COLORTABLE	0	// Do not attempt to free sac_ColorTable
#define	SACFLAGB_NO_FREE_BITMAP		1	// Do not attempt to free sac_BitMap

#define	SACFLAGF_NO_FREE_COLORTABLE	(1 << SACFLAGB_NO_FREE_COLORTABLE)
#define	SACFLAGF_NO_FREE_BITMAP		(1 << SACFLAGB_NO_FREE_BITMAP)

/*----------------------------------------------------------------------------------*/

struct ScaleBitMapArg
	{
	struct BitMap *sbma_SourceBM;
	ULONG sbma_SourceWidth;
	ULONG sbma_SourceHeight;
	ULONG *sbma_DestWidth;
	ULONG *sbma_DestHeight;
	ULONG sbma_NumberOfColors;
	CONST ULONG *sbma_ColorTable;
	ULONG sbma_Flags;
	struct BitMap *sbma_ScreenBM;
	};

/*----------------------------------------------------------------------------------*/

// ScalosGfxMedianCut tags

#define	SCALOSGFX_MedianCutFlags		(SCALOSGFX_TAGBASE + 1)		// ULONG
#define SCALOSGFX_MedianCutFriendBitMap         (SCALOSGFX_TAGBASE + 2)		// struct BitMap *
#define	SCALOSGFX_MedianCutReservedColors	(SCALOSGFX_TAGBASE + 3)		// ULONG

// values in SCALOSGFX_MedianCutFlags

#define	SCALOSGFXFLAGF_MedianCut_FloydSteinberg	0x00000001

/*----------------------------------------------------------------------------------*/

// ScalosGfxScaleARGBArray tags

#define	SCALOSGFX_ScaleARGBArrayFlags           (SCALOSGFX_TAGBASE + 10)	// ULONG

/*----------------------------------------------------------------------------------*/

// ScalosGfxScaleBitMap tags

// none defined yet.

/*----------------------------------------------------------------------------------*/

// LIBScalosGfxBlitIcon tags
#define	SCALOSGFX_BlitIconHilight		(SCALOSGFX_TAGBASE + 20)	// const struct ARGB *
#define	SCALOSGFX_BlitIconAlpha			(SCALOSGFX_TAGBASE + 21)	// const UBYTE *
#define	SCALOSGFX_BlitIconTransparency		(SCALOSGFX_TAGBASE + 22)	// ULONG

/*----------------------------------------------------------------------------------*/

// ScalosGfxBlitARGBAlphaTagList Tags

#define	SCALOSGFX_BlitARGBHilight		(SCALOSGFX_TAGBASE + 30)	// const struct ARGB *
#define	SCALOSGFX_BlitARGBNoAlpha		(SCALOSGFX_TAGBASE + 31)	// ULONG
#define	SCALOSGFX_BlitARGBTransparency		(SCALOSGFX_TAGBASE + 32)	// ULONG

/*----------------------------------------------------------------------------------*/

// Gradient types for ScalosGfxDrawGradient and ScalosGfxDrawGradientRastPort

#define	SCALOS_GRADIENT_VERTICAL		1
#define	SCALOS_GRADIENT_HORIZONTAL		2

/*----------------------------------------------------------------------------------*/

// Segment types for ScalosGfxDrawEllipse

#define	ELLIPSE_SEGMENT_TOPLEFT		8
#define	ELLIPSE_SEGMENT_TOPRIGHT	2
#define	ELLIPSE_SEGMENT_BOTTOMLEFT      4
#define	ELLIPSE_SEGMENT_BOTTOMRIGHT  	1

/*----------------------------------------------------------------------------------*/

#endif /* SCALOS_SCALOSGFX_H */
