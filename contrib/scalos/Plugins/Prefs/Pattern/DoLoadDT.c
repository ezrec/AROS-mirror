// DoLoadDT.c  
// datatype loading, scaling, remapping and dithering routine.
// $Date$
// $Revision$

#include <exec/memory.h>
#include <exec/types.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <graphics/rastport.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/scalosgfx.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include <defs.h>

#include "LoadDT.h"
#include "debug.h"

//--------------------------------------------------------------------------------

static BOOL IsPictureDT43(Object *o);

//--------------------------------------------------------------------------------

struct ScalosBitMapAndColor *DoLoadDT(STRPTR source, struct RastPort *rast,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *screen)
{
	Object *obj;
	struct BitMap *bm = NULL;
	struct BitMapHeader *bmhd = NULL;
	struct ScalosBitMapAndColor *sac = NULL;
	ULONG Success = FALSE;
	struct ARGBHeader argbSrc = { 0, 0, NULL };
	struct ARGBHeader argbDest = { 0, 0, NULL };

//  	printf('\n Start AVAILMEM-\d\n',AvailMem(MEMF_ANY))

	do	{
		PLANEPTR MaskPlane = NULL;
		LONG Result;
		ULONG ScreenDepth;

		ScreenDepth = GetBitMapAttr(screen->RastPort.BitMap, BMA_DEPTH);

		// NewDTObjectA(
		obj = NewDTObject(source,
				DTA_SourceType, DTST_FILE,
				PDTA_DestMode, PMODE_V43,
				DTA_GroupID, GID_PICTURE,
				PDTA_Remap, FALSE,
				OBP_Precision, PRECISION_EXACT,
				TAG_END);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: obj=%08lx\n", __LINE__, obj));
		if (NULL == obj)
			{
			ShowError("NewDTObject(%s) failed\n", source);
			break;
			}

		if (IsPictureDT43(obj))
			{
			d1(KPrintF(__FILE__ "/%s/%ld:  IsPictureDT43\n", __FUNC__, __LINE__));
			SetDTAttrs(obj, NULL, NULL,
				PDTA_Screen, (ULONG) screen,
				PDTA_UseFriendBitMap, ScreenDepth <= 8,
				TAG_END);
			}
		else
			{
			SetDTAttrs(obj, NULL, NULL,
				PDTA_UseFriendBitMap, FALSE,
				TAG_END);
			}

		if (!DoDTMethod(obj, 
			NULL,
			NULL,
			DTM_PROCLAYOUT, 
			NULL, 
			TRUE))
			{
			ShowError("%s: DTM_PROCLAYOUT failed\n", source);
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: DTM_PROCLAYOUT failed\n", __LINE__));
			break;
			}

		sac = ScalosGfxCreateEmptySAC();
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: sac=%08lx\n", __LINE__, sac));
		if (NULL == sac)
			{
			ShowError("%s: ScalosGfxCreateEmptySAC failed\n", source);
			break;
			}

		sac->sac_Flags |= SACFLAGF_NO_FREE_COLORTABLE;

		Result = GetDTAttrs(obj,
			PDTA_NumColors, (ULONG) &sac->sac_NumColors,
			PDTA_CRegs, (ULONG) &sac->sac_ColorTable,
			PDTA_BitMapHeader, (ULONG) &bmhd,
			PDTA_DestBitMap, (ULONG) &bm,
			TAG_END);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));
		if (Result < 4)
			{
			ShowError("%s: GetDTAttrs failed\n", source);
			break;
			}

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: sac_NumColors=%lu\n", __LINE__, sac->sac_NumColors));
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: bm=%08lx\n", __LINE__, bm));
		if (NULL == bm)
			{
			ShowError("%s: GetDTAttrs() bm == NULL\n", source);
			break;
			}

		ScalosGfxCalculateScaleAspect(bmhd->bmh_Width, bmhd->bmh_Height,
			&ScaledWidth, &ScaledHeight);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: bmh_Depth=%lu\n", __LINE__, bmhd->bmh_Depth));

		(void) GetDTAttrs(obj,
			PDTA_MaskPlane, (ULONG) &MaskPlane,
			TAG_END);

		argbSrc.argb_Width = bmhd->bmh_Width;
		argbSrc.argb_Height = bmhd->bmh_Height;

		// Create ARGB from BitMap
		argbSrc.argb_ImageData = ScalosGfxCreateARGBFromBitMap(bm,
			bmhd->bmh_Width,
			bmhd->bmh_Height,
			sac->sac_NumColors,
			sac->sac_ColorTable,
			MaskPlane);
		d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbSrc.argb_ImageData));
		if (NULL == argbSrc.argb_ImageData)
			break;

		argbDest.argb_Width = ScaledWidth;
		argbDest.argb_Height = ScaledHeight;

		// Scale image
		argbDest.argb_ImageData = ScalosGfxScaleARGBArrayTags(&argbSrc,
			&argbDest.argb_Width,
			&argbDest.argb_Height,
			SCALOSGFX_ScaleARGBArrayFlags, SCALEFLAGF_BILINEAR | SCALEFLAGF_AVERAGE | SCALEFLAGF_DOUBLESIZE,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  argbDest.argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbDest.argb_ImageData));
		if (NULL == argbDest.argb_ImageData)
			break;

		// Free preliminary SAC previously created
		ScalosGfxFreeSAC(sac);

		// Create dithered image from scaled image
		sac = ScalosGfxMedianCutTags(&argbDest, 8,
			SCALOSGFX_MedianCutFlags, SCALOSGFXFLAGF_MedianCut_FloydSteinberg,
			SCALOSGFX_MedianCutFriendBitMap, NULL,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  sac=%08lx\n", __FUNC__, __LINE__, sac));
		if (NULL == sac)
			break;

		Success = TRUE;
		} while (0);

	ScalosGfxFreeARGB(&argbSrc.argb_ImageData);
	ScalosGfxFreeARGB(&argbDest.argb_ImageData);

	if (obj)
		DisposeDTObject(obj);
	if (!Success)
		{
		ScalosGfxFreeSAC(sac);
		sac = NULL;
		}

	return sac;
}

// ----------------------------------------------------------

// Work-around for OS3.0/3.1 and Picasso96 picture.datatype V43.41
// This datatype seems to understand PMODE_V43, but in combination with
// PDTA_UseFriendBitMap and PDTA_Screen, returns a very weird BitMap
// that I was unable to deal with: Depth=16, but no CyberGfx BitMap
// Looks like an extended standard BitMap with 16 plane pointers.

static BOOL IsPictureDT43(Object *o)
{
	const ULONG *MethodArray = NULL;
	BOOL IsV43 = FALSE;

	GetDTAttrs(o,
		DTA_Methods, (ULONG) &MethodArray,
		TAG_END);

	while (MethodArray && ~0 != *MethodArray)
		{
		if (PDTM_SCALE == *MethodArray)
			{
			IsV43 = TRUE;
			break;
			}
		MethodArray++;
		}

	return IsV43;
}

//-----------------------------------------------------------------------------


