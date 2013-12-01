// dtimage.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dostags.h>
#ifdef __amigaos4__
#include <dos/anchorpath.h>
#endif
#include <graphics/gfx.h>
#if defined(__amigaos4__)
#include <graphics/blitattr.h>
#endif //defined(__amigaos4__)
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <datatypes/pictureclass.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define __USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/layers.h>
#include <proto/utility.h>
#include <proto/timer.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include <scalos/scalos.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

#define PDTA_AlphaChannel	(DTA_Dummy + 256)	/* Alphachannel input */

//----------------------------------------------------------------------------

// local functions

static void FreeDatatypesImage(struct DatatypesImage *dti);
static struct DatatypesImage *NewDatatypesImage(CONST_STRPTR ImageName, ULONG Flags);
static struct DatatypesImage *FindDatatypesImage(CONST_STRPTR ImageName, ULONG Flags);
static LONG CreateTempFile(CONST_STRPTR TempFileName, CONST_STRPTR DtFileName);
static void DtImageNotify(struct internalScaWindowTask *iwt, struct NotifyMessage *msg);
static void CleanupTempFiles(void);
#if defined(__amigaos4__)
static BOOL DtImageCreateAlpha(struct DatatypesImage *dti);
#endif //defined(__amigaos4__)

//----------------------------------------------------------------------------

// local data items :

static struct List DataTypesImageList;
static BOOL Initialized = FALSE;

//----------------------------------------------------------------------------

BOOL InitDataTypesImage(void)
{
	NewList(&DataTypesImageList);

	CleanupTempFiles();

	Initialized = TRUE;

	return TRUE;
}


void CleanupDataTypesImage(void)
{
	if (Initialized)
		{
		struct DatatypesImage *dti;

		while ((dti = (struct DatatypesImage *) RemHead(&DataTypesImageList)))
			{
			d1(kprintf("%s/%s/%ld: Freeing dti(%lx)\n", __FILE__, __FUNC__, __LINE__, dti));
			FreeDatatypesImage(dti);
			}
		}
}


void DisposeDatatypesImage(struct DatatypesImage **dti)
{
	if (*dti)
		{
		if (0 == --(*dti)->dti_UseCount)
			{
#if 0
			Remove(&(*dti)->dti_Node);
			FreeDatatypesImage(*dti);
#endif
			*dti = NULL;
	                }
                }
}


struct DatatypesImage *CreateDatatypesImage(CONST_STRPTR ImageName, ULONG Flags)
{
	struct DatatypesImage *dti;

	dti = FindDatatypesImage(ImageName, Flags);
	if (dti)
		dti->dti_UseCount++;
	else
		dti = NewDatatypesImage(ImageName, Flags);

	d1(kprintf("%s/%s/%ld: Create image %s dti(%lx)\n", __FILE__, __FUNC__, __LINE__,
		ImageName, dti));

	return dti;
}


void FillBackground(struct RastPort *rp, struct DatatypesImage *dtImage,
	WORD MinX, WORD MinY, WORD MaxX, WORD MaxY, 
	ULONG XStart, ULONG YStart)
{
	if (dtImage && dtImage->dti_BitMap)
		{
		LONG x, y;
		LONG h;
		LONG SrcX, SrcY;
		LONG bmHeight = dtImage->dti_BitMapHeader->bmh_Height;
		LONG bmWidth = dtImage->dti_BitMapHeader->bmh_Width;

		d1(KPrintF("%s/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld  XStart=%ld YStart=%ld\n", \
			__FILE__, __FUNC__, __LINE__, MinX, MinY, MaxX, MaxY, XStart, YStart));

		d1(KPrintF("%s/%s/%ld: Image w=%ld  h=%ld\n", \
			__FILE__, __FUNC__, __LINE__, bmWidth, bmHeight));

		XStart %= bmWidth;
		YStart %= bmHeight;

		if (YStart < 0)
			SrcY = bmHeight - (-YStart % bmHeight);
		else
			SrcY = YStart;

		SrcY %= bmHeight;
		h = bmHeight - SrcY;

		for (y = MinY; y <= MaxY; )
			{
			LONG w;

			if (XStart < 0)
				SrcX = bmWidth - (-XStart % bmWidth);
			else
				SrcX = XStart;

			SrcX %= bmWidth;

			w = bmWidth - SrcX;

			if ((y + h) > MaxY)
				h = 1 + MaxY - y;

			d1(kprintf("%s/%s/%ld: SrcX=%ld  y=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, SrcX, y, h));

			for (x = MinX; x <= MaxX; )
				{
				if (x + w > MaxX)
					w = 1 + MaxX - x;

				d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, w, h));

				BltBitMapRastPort(dtImage->dti_BitMap, 
					SrcX, SrcY, 
					rp,
					x, y,
					w, h,
					0xC0);

				x += w;
				w = bmWidth;
				SrcX = 0;
				}

			y += h;
			h = bmHeight;
			SrcY = 0;
			}
		}
	else
		{
		SetAPen(rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN]);
		SetDrMd(rp, JAM1);

		RectFill(rp, MinX, MinY, MaxX, MaxY);
		}
}


static void FreeDatatypesImage(struct DatatypesImage *dti)
{
	if (dti->dti_ImageObj)
		{
		Object *ImageObject = dti->dti_ImageObj;
		struct ExtGadget *gg = (struct ExtGadget *) ImageObject;
		struct DTSpecialInfo *si = (struct DTSpecialInfo *) gg->SpecialInfo;

		dti->dti_ImageObj = NULL;
		dti->dti_BitMap = NULL;

		if (dti->dti_NotifyNode)
			{
			RemFromMainNotifyList(dti->dti_NotifyNode);
			dti->dti_NotifyNode = NULL;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// wait for pending PROCLAYOUT to complete
		while (si && si->si_Flags & DTSIF_LAYOUTPROC)
			Delay(10);

		DisposeDTObject(ImageObject);

		// now temporary image file can be deleted
		DeleteFile(dti->dti_TempFilename);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (dti->dti_ARGB)
			{
			ScalosFree(dti->dti_ARGB);
			dti->dti_ARGB = NULL;
			}
		if (dti->dti_Filename)
			{
			d1(KPrintF("%s/%s/%ld: FileName=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, dti->dti_Filename, dti->dti_Filename));
			FreeCopyString(dti->dti_Filename);
			dti->dti_Filename = NULL;
			}
		if (dti->dti_TempFilename)
			{
			FreePathBuffer(dti->dti_TempFilename);
			dti->dti_TempFilename = NULL;
			}
		}

	ScalosFree(dti);
}


static struct DatatypesImage *NewDatatypesImage(CONST_STRPTR ImageName, ULONG Flags)
{
	struct DatatypesImage *dti;
	BOOL Success = FALSE;

	do	{
		ULONG Result;
		ULONG UseFriendBM;
		ULONG DoRemap;

		d1(KPrintF("%s/%s/%ld: START  ImageName=<%s>\n", __FILE__, __FUNC__, __LINE__, ImageName));

		dti = ScalosAlloc(sizeof(struct DatatypesImage));
		if (NULL == dti)
			{
			d1(KPrintF("%s/%s/%ld: dti=%08lx\n", __FILE__, __FUNC__, __LINE__, dti));
			break;
			}

		d1(KPrintF("%s/%s/%ld: dti=%08lx\n", __FILE__, __FUNC__, __LINE__, dti));

		memset(dti, 0, sizeof(struct DatatypesImage));

		dti->dti_Filename = AllocCopyString(ImageName);
		if (NULL == dti->dti_Filename)
			{
			d1(KPrintF("%s/%s/%ld: dti_Filename=%08lx\n", __FILE__, __FUNC__, __LINE__, dti->dti_Filename));
			break;
			}

		dti->dti_TempFilename = AllocPathBuffer();
		if (NULL == dti->dti_TempFilename)
			{
			d1(KPrintF("%s/%s/%ld: dti_TempFilename=%08lx\n", __FILE__, __FUNC__, __LINE__, dti->dti_TempFilename));
			break;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// Create pseudo-random name for temp. file
		if (!TempName(dti->dti_TempFilename, Max_PathLen))
			{
			d1(KPrintF("%s/%s/%ld: TempName() failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// create a temporary copy of the datatypes image
		// work around datatypes keeping a lock on the image file
		if (RETURN_OK != CreateTempFile(dti->dti_TempFilename, dti->dti_Filename))
			{
			d1(KPrintF("%s/%s/%ld: CreateTempFile() failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		dti->dti_MaskPlane = NULL;
		dti->dti_BitMap = NULL;
		dti->dti_BitMapHeader = NULL;
		dti->dti_UseCount = 0;
		dti->dti_Flags = Flags;

		d1(KPrintF("%s/%s/%ld: dti=%08lx  Temp=<%s>  Orig=<%s>\n", \
			__FILE__, __FUNC__, __LINE__, dti, dti->dti_TempFilename, dti->dti_Filename));

		dti->dti_NotifyTab.nft_FileName = dti->dti_Filename;
		dti->dti_NotifyTab.nft_Entry = DtImageNotify;

		/* +dm+ Do not use a friend bitmap for 8bit or less screens */
		UseFriendBM = !(Flags & DTI_NoFriendBitMap) && (GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH) > 8);
		DoRemap = !(Flags & DTIFLAG_NoRemap);

		// NewDTObjectA()
		dti->dti_ImageObj = NewDTObject(dti->dti_TempFilename,
				DTA_SourceType, DTST_FILE,
				DTA_GroupID, GID_PICTURE,
				PDTA_DestMode, PMODE_V43,
				PDTA_Remap, DoRemap,
				DoRemap ? PDTA_Screen : TAG_IGNORE, (ULONG) iInfos.xii_iinfos.ii_Screen,
				PDTA_UseFriendBitMap, UseFriendBM,
				TAG_END);
		d1(KPrintF("%s/%s/%ld: dti_ImageObj=%08lx\n", __FILE__, __FUNC__, __LINE__, dti->dti_ImageObj));
		if (NULL == dti->dti_ImageObj)
			{
			d1(KPrintF("%s/%s/%ld: dti_ImageObj=%08lx\n", __FILE__, __FUNC__, __LINE__, dti->dti_ImageObj));
			break;
			}

		// DoDTMethodA()
		if (!DoDTMethod(dti->dti_ImageObj, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE))
			{
			d1(KPrintF("%s/%s/%ld: DTM_PROCLAYOUT failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		Result = GetDTAttrs(dti->dti_ImageObj,
			PDTA_DestBitMap, (ULONG) &dti->dti_BitMap,
			PDTA_BitMapHeader, (ULONG) &dti->dti_BitMapHeader,
			TAG_END);

		d1(KPrintF("%s/%s/%ld: Result=%ld  dti_BitMap=%08lx  dti_BitMapHeader=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, Result, dti->dti_BitMap, dti->dti_BitMapHeader));
		if (Result < 2)
			{
			d1(KPrintF("%s/%s/%ld: GetDTAttrs failed\n", __FILE__, __FUNC__, __LINE__));
			d1(KPrintF("%s/%s/%ld: Result=%ld  dti_BitMap=%08lx  dti_BitMapHeader=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, Result, dti->dti_BitMap, dti->dti_BitMapHeader));
			break;
			}

		if (NULL == dti->dti_BitMap || NULL == dti->dti_BitMapHeader)
			break;

		switch (dti->dti_BitMapHeader->bmh_Masking)
			{
		case mskHasAlpha:
#if defined(__amigaos4__)
			DtImageCreateAlpha(dti);
#endif //defined(__amigaos4__)
		case mskHasMask:
		case mskHasTransparentColor:
			GetDTAttrs(dti->dti_ImageObj,
				PDTA_MaskPlane, (ULONG) &dti->dti_MaskPlane,
				TAG_END);
			break;
		default:
			dti->dti_MaskPlane = NULL;
			break;
			}

		d1(kprintf("%s/%s/%ld: <%s> Masking=%ld  MaskPlane=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			ImageName, dti->dti_BitMapHeader->bmh_Masking, dti->dti_MaskPlane));

		dti->dti_NotifyNode = AddToMainNotifyList(&dti->dti_NotifyTab, 0);

		Success = TRUE;
		} while (0);

	if (Success)
		{
		AddTail(&DataTypesImageList, &dti->dti_Node);
		dti->dti_UseCount++;
		}
	else if (dti)
		{
		FreeDatatypesImage(dti);
		dti = NULL;
		}

	d1(KPrintF("%s/%s/%ld: END dti=%08lx\n", __FILE__, __FUNC__, __LINE__, dti));

	return dti;
}


static struct DatatypesImage *FindDatatypesImage(CONST_STRPTR ImageName, ULONG Flags)
{
	struct DatatypesImage *dti;

	for (dti = (struct DatatypesImage *) DataTypesImageList.lh_Head;
		dti != (struct DatatypesImage *) &DataTypesImageList.lh_Tail;
		dti = (struct DatatypesImage *) dti->dti_Node.ln_Succ)
		{
		if ((dti->dti_Flags == Flags) && (0 == Stricmp(ImageName, dti->dti_Filename)))
			return dti;
		}

	return NULL;
}


BOOL TempName(STRPTR Buffer, size_t MaxLen)
{
	T_TIMEVAL tv;
	char TimeBuffer[80];

	GetSysTime(&tv);

	if (MaxLen <= strlen(CurrentPrefs.pref_ImageCacheDir))
		return FALSE;

	stccpy(Buffer, CurrentPrefs.pref_ImageCacheDir, MaxLen);

	snprintf(TimeBuffer, sizeof(TimeBuffer), "Scalos%08lx%08lx", (unsigned long) tv.tv_secs, (unsigned long) tv.tv_micro);

	return AddPart(Buffer, TimeBuffer, MaxLen);
}


static LONG CreateTempFile(CONST_STRPTR TempFileName, CONST_STRPTR DtFileName)
{
	LONG Result = RETURN_OK;
	BPTR fdOrig;
	BPTR fdCopy = BNULL;
	STRPTR CopyBuffer = NULL;
	const size_t BuffSize = 8192;

	do	{
		fdOrig = Open(DtFileName, MODE_OLDFILE);
		if (BNULL == fdOrig)
			{
			Result = IoErr();
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}
		fdCopy = Open(TempFileName, MODE_NEWFILE);
		if (BNULL == fdCopy)
			{
			Result = IoErr();
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}
		CopyBuffer = ScalosAlloc(BuffSize);
		if (NULL == CopyBuffer)
			{
			Result = ERROR_NO_FREE_STORE;
			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		while (1)
			{
			LONG actualLength;

			actualLength = Read(fdOrig, CopyBuffer, BuffSize);
			if (actualLength <= 0)
				{
				if (actualLength < 0)
					{
					Result = IoErr();
					d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					}
				break;
				}

			if (actualLength != Write(fdCopy, CopyBuffer, actualLength))
				{
				Result = IoErr();
				d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}
			}
		} while (0);

	if (CopyBuffer)
		ScalosFree(CopyBuffer);
	if (fdOrig)
		Close(fdOrig);
	if (fdCopy)
		Close(fdCopy);

	if (RETURN_OK != Result)
		DeleteFile(TempFileName);

	return Result;
}


static void DtImageNotify(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct DatatypesImage *dti;
	struct DatatypesImage *dtiNew;

	// original file of <dti> has been modified
	// try to create Datatypes object from new image file
	// if we succeed, replace old datatypes image by new one
	// if we fail, continue using old datatypes object

	dti = (struct DatatypesImage *) (msg->nm_NReq->nr_UserData - offsetof(struct DatatypesImage, dti_NotifyTab));

	d1(KPrintF("%s/%s/%ld: nr_Name=<%s>  nr_UserData=%08lx  dti=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, msg->nm_NReq->nr_Name, msg->nm_NReq->nr_UserData, dti));

	do	{
		ULONG Result;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		dtiNew = ScalosAlloc(sizeof(struct DatatypesImage));
		if (NULL == dtiNew)
			break;

		d1(KPrintF("%s/%s/%ld: dtiNew=%08lx\n", __FILE__, __FUNC__, __LINE__, dtiNew));

		memset(dtiNew, 0, sizeof(struct DatatypesImage));

		dtiNew->dti_Filename = AllocCopyString(dti->dti_Filename);
		if (NULL == dtiNew->dti_Filename)
			break;

		dtiNew->dti_TempFilename = AllocPathBuffer();
		if (NULL == dtiNew->dti_TempFilename)
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// Create pseudo-random name for temp. file
		if (!TempName(dtiNew->dti_TempFilename, Max_PathLen))
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// create a temporary copy of the datatypes image
		// work around datatypes keeping a lock on the image file
		if (RETURN_OK != CreateTempFile(dtiNew->dti_TempFilename, dtiNew->dti_Filename))
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// Try to create datatypes image from new file
		// NewDTObjectA()
		dtiNew->dti_ImageObj = NewDTObject(dtiNew->dti_TempFilename,
				DTA_SourceType, DTST_FILE,
				DTA_GroupID, GID_PICTURE,
				PDTA_DestMode, PMODE_V43,
				PDTA_Remap, TRUE,
				PDTA_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				PDTA_UseFriendBitMap, (GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH) <= 8 ? FALSE : TRUE ),	 /* +dm+ Do not use a friend bitmap for 8bit or less screens */
				TAG_END);
		d1(KPrintF("%s/%s/%ld: dti_ImageObj=%08lx\n", __FILE__, __FUNC__, __LINE__, dtiNew->dti_ImageObj));
		if (NULL == dtiNew->dti_ImageObj)
			break;

		// dispose old datatypes object
		DisposeDTObject(dti->dti_ImageObj);

		// remove old temp image file
		DeleteFile(dti->dti_TempFilename);
		strcpy(dti->dti_TempFilename, dtiNew->dti_TempFilename);
		strcpy(dtiNew->dti_TempFilename, "");

		// use new Datatypes object
		dti->dti_ImageObj = dtiNew->dti_ImageObj;
		dtiNew->dti_ImageObj = NULL;

		// Update information from new Datatypes object

		// DoDTMethodA()
		if (!DoDTMethod(dti->dti_ImageObj, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE))
			break;

		Result = GetDTAttrs(dti->dti_ImageObj,
			PDTA_DestBitMap, (ULONG) &dti->dti_BitMap,
			PDTA_BitMapHeader, (ULONG) &dti->dti_BitMapHeader,
			TAG_END);

		if (Result < 2)
			break;

		if (NULL == dti->dti_BitMap || NULL == dti->dti_BitMapHeader)
			break;

		switch (dti->dti_BitMapHeader->bmh_Masking)
			{
		case mskHasMask:
		case mskHasAlpha:
		case mskHasTransparentColor:
			GetDTAttrs(dti->dti_ImageObj,
				PDTA_MaskPlane, (ULONG) &dti->dti_MaskPlane,
				TAG_END);
			break;
		default:
			dti->dti_MaskPlane = NULL;
			break;
			}
		} while (0);

	FreeDatatypesImage(dtiNew);
}


static void CleanupTempFiles(void)
{
	STRPTR Path = NULL;
	struct AnchorPath *ap;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	
	do	{
		LONG rc;

		ap = ScalosAllocAnchorPath(APF_DOWILD | APF_ITSWILD, Max_PathLen);
		d1(KPrintF("%s/%s/%ld: ap=%08lx\n", __FILE__, __FUNC__, __LINE__, ap));
		if (NULL == ap)
			break;

		Path = AllocPathBuffer();
		d1(KPrintF("%s/%s/%ld: Path=%08lx\n", __FILE__, __FUNC__, __LINE__, Path));
		if (NULL == Path)
			break;

		stccpy(Path, CurrentPrefs.pref_ImageCacheDir, Max_PathLen);

		if (!AddPart(Path, "Scalos#?", Max_PathLen))
			break;

		d1(KPrintF("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		rc = MatchFirst(Path, ap);
		d1(KPrintF("%s/%s/%ld: MatchFirst rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		while (RETURN_OK == rc)
			{
#ifndef __amigaos4__
			d1(KPrintF("%s/%s/%ld: ap_Buf=<%s>\n", __FILE__, __FUNC__, __LINE__, ap->ap_Buf));
			(void) DeleteFile(ap->ap_Buf);
#else
			d1(KPrintF("%s/%s/%ld: ap_Buf=<%s>\n", __FILE__, __FUNC__, __LINE__, ap->ap_Buffer));
			(void) DeleteFile(ap->ap_Buffer);
#endif

			rc = MatchNext(ap);
			d1(KPrintF("%s/%s/%ld: MatchNext rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
			}

		MatchEnd(ap);
		} while (0);

	if (ap)
		ScalosFreeAnchorPath(ap);

	if (Path)
		FreePathBuffer(Path);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


#if defined(__amigaos4__)
static BOOL DtImageCreateAlpha(struct DatatypesImage *dti)
{
	BOOL success = FALSE;

	do	{
		size_t BytesPerPixel = dti->dti_BitMapHeader->bmh_Width * sizeof(ULONG);

		if (mskHasAlpha != dti->dti_BitMapHeader->bmh_Masking)
			break;

		dti->dti_ARGB = ScalosAlloc(BytesPerPixel * dti->dti_BitMapHeader->bmh_Height);
		if (NULL == dti->dti_ARGB)
			break;

		DoMethod(dti->dti_ImageObj,
			PDTM_READPIXELARRAY,
			dti->dti_ARGB,
			PBPAFMT_ARGB,
			BytesPerPixel,
			0,
			0,
			dti->dti_BitMapHeader->bmh_Width,
			dti->dti_BitMapHeader->bmh_Height
			);

		success = TRUE;
		} while (0);

	return success;
}
#endif //defined(__amigaos4__)


void DtImageDraw(struct DatatypesImage *dti, struct RastPort *rp,
	LONG Left, LONG Top, LONG Width, LONG Height)
{
	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
#if defined(__amigaos4__)
	if (dti->dti_ARGB)
		{
		LONG rc;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		// BltBitMapTagList
		rc = BltBitMapTags(
			BLITA_Source, dti->dti_ARGB,
			BLITA_Dest, rp,
			BLITA_SrcX, 0,
			BLITA_SrcY, 0,
			BLITA_DestX, Left,
			BLITA_DestY, Top,
			BLITA_Width, Width,
			BLITA_Height, Height,
			BLITA_SrcType, BLITT_ARGB32,
			BLITA_DestType, BLITT_RASTPORT,
			BLITA_SrcBytesPerRow, dti->dti_BitMapHeader->bmh_Width * sizeof(ULONG),
			BLITA_UseSrcAlpha, TRUE,
			TAG_END);

		d1(kprintf("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		}
	else
#endif //defined(__amigaos4__)
		{
		ULONG rc;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		// returns 1 if successful, 0 on error
		rc = DoMethod(dti->dti_ImageObj,
			DTM_DRAW,
			rp,
			Left,
			Top,
			Width,
			Height,
			0, 0,
			NULL
			);
		d1(kprintf("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

		if (!rc)
			{
			if (dti->dti_MaskPlane)
				{
				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				BltMaskBitMapRastPort(dti->dti_BitMap,
					0, 0,
					rp,
					Left, Top,
					Width, Height,
					ABC | ABNC | ANBC,
					dti->dti_MaskPlane);
				}
			else
				{
				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				BltBitMapRastPort(dti->dti_BitMap,
					0, 0,
					rp,
					Left, Top,
					Width, Height,
					ABC | ABNC);
				}
			}
		}
	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


