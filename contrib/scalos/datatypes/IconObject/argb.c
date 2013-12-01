// argb.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/iconobject.h>
#include <libraries/mcpgfx.h>

#define	__USE_SYSBASE
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/exec.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/scalosgfx.h>

#include <string.h>
#include <ctype.h>

#include <defs.h>
#include <Year.h>
#include <ScalosMcpGfx.h>

#include <datatypes/iconobject.h>
#include "iconobj.h"

//----------------------------------------------------------------------------

#define	min(a,b)	((a) < (b) ? (a) : (b))

//----------------------------------------------------------------------------

extern struct Library *CyberGfxBase;

//-----------------------------------------------------------------------------

static void WriteARGBArray(const struct ARGB *src, struct RastPort *dst,
	ULONG dstx, ULONG dsty, ULONG width, ULONG height);
static void DrawFrame(Object *o, struct InstanceData *inst, struct RastPort *rp,
	LONG x, LONG y, UWORD FrameType);
static BOOL GenMaskFromARGB(const struct IconObjectARGB *img, const struct IconObjectARGB *CloneImage,
	struct IconObjectMask *Mask, const struct IconObjectMask *CloneMask);

//-----------------------------------------------------------------------------

void ARGB_Draw(Class *cl, Object *o, struct iopDraw *opd, LONG x, LONG y)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct ARGBHeader *ImgHeader;
	struct IconObjectARGB *img;

	if ((gg->Flags & GFLG_SELECTED) && inst->iobj_SelectedARGB.iargb_ARGBimage.argb_ImageData)
		{
		img = &inst->iobj_SelectedARGB;
		}
	else
		{
		img = &inst->iobj_NormalARGB;
		}

	if (img->iargb_ScaledARGBImage.argb_ImageData)
		ImgHeader = &img->iargb_ScaledARGBImage;
	else
		ImgHeader = &img->iargb_ARGBimage;

	d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, ImgHeader->argb_Width, ImgHeader->argb_Height));
	d1(KPrintF("%s/%s/%ld:  o=%08lx  ImgData=%08lx\n", __FILE__, __FUNC__, __LINE__, o, ImgHeader->argb_ImageData));
	d1(kprintf("%s/%s/%ld:  ImgData[]=%08lx %08lx %08lx %08lx\n", __FILE__, __FUNC__, __LINE__, \
		ImgHeader->argb_ImageData[0], ImgHeader->argb_ImageData[1], ImgHeader->argb_ImageData[2], ImgHeader->argb_ImageData[3]));

	if (!(IODRAWF_NoImage & opd->iopd_DrawFlags))
		{
		if (!(IODRAWF_NoEraseBg & opd->iopd_DrawFlags))
			EraseRect(opd->iopd_RastPort, x, y, x + gg->Width, y + gg->Height);

		if (NULL == CyberGfxBase)
			{
			Move(opd->iopd_RastPort, x + inst->iobj_imgleft, y + inst->iobj_imgtop);

			Draw(opd->iopd_RastPort, x + inst->iobj_imgleft + gg->Width, y + inst->iobj_imgtop);
			Draw(opd->iopd_RastPort, x + inst->iobj_imgleft + gg->Width, y + inst->iobj_imgtop + gg->Height);
			Draw(opd->iopd_RastPort, x + inst->iobj_imgleft, y + inst->iobj_imgtop + gg->Height);
			Draw(opd->iopd_RastPort, x + inst->iobj_imgleft, y + inst->iobj_imgtop);
			}
		else
			{
			if (gg->Flags & GFLG_SELECTED)
				{
				struct ARGB K = { 0, 96, 96, 144 };

				if (IODRAWF_NoAlpha & opd->iopd_DrawFlags)
					{
					if (img == &inst->iobj_SelectedARGB)
						{
						// draw selected image
						WriteARGBArray(ImgHeader->argb_ImageData, opd->iopd_RastPort,
							x + inst->iobj_imgleft, y + inst->iobj_imgtop,
							ImgHeader->argb_Width, ImgHeader->argb_Height);
						}
					else
						{
						// draw highlighted variant of normal image
						struct IBox SrcSize;

						SrcSize.Left = SrcSize.Top = 0;
						SrcSize.Width = ImgHeader->argb_Width;
						SrcSize.Height = ImgHeader->argb_Height;

						ScalosGfxBlitARGBAlphaTags(opd->iopd_RastPort,
							ImgHeader,
							x + inst->iobj_imgleft, y + inst->iobj_imgtop,
							&SrcSize,
							SCALOSGFX_BlitARGBHilight, (ULONG) &K,
							SCALOSGFX_BlitARGBTransparency, 200,
							SCALOSGFX_BlitARGBNoAlpha, TRUE,
							TAG_END);
						}
					}
				else
					{
					if (img == &inst->iobj_SelectedARGB)
						{
						// draw selected image
						ScalosGfxBlitARGBAlpha(opd->iopd_RastPort,
							ImgHeader,
							x + inst->iobj_imgleft, y + inst->iobj_imgtop,
							0, 0,
							ImgHeader->argb_Width, ImgHeader->argb_Height);
						}
					else
						{
						// draw highlighted variant of normal image
						struct IBox SrcSize;

						SrcSize.Left = SrcSize.Top = 0;
						SrcSize.Width = ImgHeader->argb_Width;
						SrcSize.Height = ImgHeader->argb_Height;

						ScalosGfxBlitARGBAlphaTags(opd->iopd_RastPort,
							ImgHeader,
							x + inst->iobj_imgleft, y + inst->iobj_imgtop,
							&SrcSize,
							SCALOSGFX_BlitARGBHilight, (ULONG) &K,
							SCALOSGFX_BlitARGBTransparency, 200,
							TAG_END);
						}
					}
				if (!inst->iobj_Borderless && inst->iobj_imgleft > 0 && inst->iobj_imgtop > 0)
					DrawFrame(o, inst, opd->iopd_RastPort, x, y, inst->iobj_frametypesel);
				}
			else
				{
				if (IODRAWF_NoAlpha & opd->iopd_DrawFlags)
					{
					WriteARGBArray(ImgHeader->argb_ImageData, opd->iopd_RastPort,
						x + inst->iobj_imgleft, y + inst->iobj_imgtop,
						ImgHeader->argb_Width, ImgHeader->argb_Height);
					}
				else
					{
					ScalosGfxBlitARGBAlpha(opd->iopd_RastPort,
						ImgHeader,
						x + inst->iobj_imgleft, y + inst->iobj_imgtop,
						0, 0,
						ImgHeader->argb_Width, ImgHeader->argb_Height);
					}
				if (!inst->iobj_Borderless && inst->iobj_imgleft > 0 && inst->iobj_imgtop > 0)
					DrawFrame(o, inst, opd->iopd_RastPort, x, y, inst->iobj_frametype);
				}
			}
		}
}

//-----------------------------------------------------------------------------

static void WriteARGBArray(const struct ARGB *src, struct RastPort *dst,
	ULONG dstx, ULONG dsty, ULONG width, ULONG height)
{
	WritePixelArray((APTR) src, 0, 0,
		width * sizeof(struct ARGB),
		dst, dstx, dsty,
                width, height, RECTFMT_ARGB);
}

//-----------------------------------------------------------------------------

static void DrawFrame(Object *o, struct InstanceData *inst, struct RastPort *rp,
	LONG x, LONG y, UWORD FrameType)
{
	struct RastPort rpCopy;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	rpCopy = *rp;

	d1(kprintf("%s/%s/%ld:  xMin=%ld  yMin=%ld  xMax=%ld  yMax=%ld\n", \
		__FILE__, __FUNC__, __LINE__, x, y, x + gg->Width - 1, y + gg->Height - 1));

	McpGfxDrawFrame(&rpCopy, x, y, x + gg->Width - 1, y + gg->Height - 1,
		IA_HalfShinePen, inst->iobj_HalfShinePen,
		IA_HalfShadowPen, inst->iobj_HalfShadowPen,
		IA_FrameType, FrameType & 0x7fff,
		IA_Recessed, (FrameType & 0x8000) ? 1 : 0,
		TAG_END);
}

//-----------------------------------------------------------------------------

BOOL GenMasksFromARGB(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	if (!GenMaskFromARGB(&inst->iobj_NormalARGB, NULL,
		&inst->iobj_NormalMask, NULL))
		return FALSE;

	d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return GenMaskFromARGB(&inst->iobj_SelectedARGB, &inst->iobj_NormalARGB,
		&inst->iobj_SelectedMask, &inst->iobj_NormalMask);
}

//-----------------------------------------------------------------------------

static BOOL GenMaskFromARGB(const struct IconObjectARGB *img, const struct IconObjectARGB *CloneImage,
	struct IconObjectMask *Mask, const struct IconObjectMask *CloneMask)
{
	const struct ARGBHeader *ImgHeader;
	const	struct ARGB *PicPtr;
	ULONG	BytesPerRow;
	const struct IconObjectARGB *imgUsed;
	size_t Size;

	d1(KPrintF( "%s/%s/%ld:  img=%08lx  CloneImage=%08lx\n", __FILE__, __FUNC__, __LINE__, img, CloneImage));
	d1(KPrintF( "%s/%s/%ld:  Mask=%08lx  CloneMask=%08lx\n", __FILE__, __FUNC__, __LINE__, Mask, CloneMask));

	imgUsed = img->iargb_ARGBimage.argb_ImageData ? img : CloneImage;

	if (NULL == imgUsed)
		return FALSE;

	d1(KPrintF( "%s/%s/%ld:  iargb_ScaledARGBImage=%08lx  w=%ld  h=%ld  ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		&imgUsed->iargb_ScaledARGBImage, \
		imgUsed->iargb_ScaledARGBImage.argb_Width, \
		imgUsed->iargb_ScaledARGBImage.argb_Height, \
		imgUsed->iargb_ScaledARGBImage.argb_ImageData));
	d1(KPrintF( "%s/%s/%ld:  iargb_ARGBimage=%08lx  w=%ld  h=%ld  ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		&imgUsed->iargb_ARGBimage, \
		imgUsed->iargb_ARGBimage.argb_Width, \
		imgUsed->iargb_ARGBimage.argb_Height, \
		imgUsed->iargb_ARGBimage.argb_ImageData));

	if (imgUsed->iargb_ScaledARGBImage.argb_Width > 512
		|| imgUsed->iargb_ScaledARGBImage.argb_Height > 512)
		return FALSE;

	d1(KPrintF( "%s/%s/%ld:  imgUsed=%08lx\n", __FILE__, __FUNC__, __LINE__, imgUsed));

	if (imgUsed->iargb_ScaledARGBImage.argb_ImageData)
		ImgHeader = &imgUsed->iargb_ScaledARGBImage;
	else
		ImgHeader = &imgUsed->iargb_ARGBimage;

	d1(KPrintF( "%s/%s/%ld:  Width=%lu  Height=%ld\n", __FILE__, __FUNC__, __LINE__, ImgHeader->argb_Width, ImgHeader->argb_Height));

	Mask->iom_Width = ImgHeader->argb_Width;
	Mask->iom_Height = ImgHeader->argb_Height;

	BytesPerRow = BYTESPERROW(ImgHeader->argb_Width);
	Size = BytesPerRow * ImgHeader->argb_Height;
	Size = ALIGN_LONG(Size);

	d1(KPrintF( "%s/%s/%ld:  BytesPerRow=%ld\n", __FILE__, __FUNC__, __LINE__, BytesPerRow));

	MyFreeVecPooled(ChipMemPool, (APTR *) &Mask->iom_Mask);

	Mask->iom_Mask = MyAllocVecPooled(ChipMemPool, Size);
	if (NULL == Mask->iom_Mask)
		return FALSE;

	memset(Mask->iom_Mask, 0, Size);

	PicPtr = ImgHeader->argb_ImageData;

	d1(KPrintF( "%s/%s/%ld:  PicPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, PicPtr));

	if (PicPtr)
		{
		UBYTE	*MaskPtr = Mask->iom_Mask;
		LONG	y;
		ULONG	Threshold = 0;

		d1(KPrintF( "%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		for (y = 0; y < ImgHeader->argb_Height; y++)
			{
			LONG	x;
			UWORD	BitMask = 0x0080;
			UBYTE	*MaskPtr2 = MaskPtr;

			for (x = 0; x < ImgHeader->argb_Width; x++)
				{
				UBYTE	alpha;

				alpha = PicPtr->Alpha;
				PicPtr++;

				if (alpha > Threshold)
					*MaskPtr2 |= BitMask;

				BitMask >>= 1;
				if (0 == BitMask)
					{
					BitMask = 0x0080;
					MaskPtr2++;
					d1(if (MaskPtr2 > Mask->iom_Mask + Size)\
						{ \
						kprintf("%s/%s/%ld:  iobj_normmask overflow\n", __FILE__, __FUNC__, __LINE__); \
						break; \
						} );
					}
				}

			MaskPtr += BytesPerRow;
			}
		}
	else if (CloneImage && CloneMask)
		{
		d1(KPrintF( "%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		Mask->iom_Width = CloneMask->iom_Width;
		Mask->iom_Height = CloneMask->iom_Height;

		CopyMem(Mask->iom_Mask, CloneMask->iom_Mask, Size);
		}

	return TRUE;
}

//-----------------------------------------------------------------------------

BOOL GenAlphaFromARGB(Class *cl, Object *o, struct IconObjectARGB *img, struct ARGBHeader *ImgHeader)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	UBYTE	*AlphaPtr;
	const	struct ARGB *PicPtr;
	UWORD	Left, BoundsLeft;
	ULONG	IconWidth, IconHeight;
	WORD	LeftOffset;
	ULONG	y;
	size_t	Size;


	d1(KPrintF("%s/%s/%ld:  START o=%08lx  w=%lu h=%lu  bw=%lu bh=%lu\n", \
		__FILE__, __FUNC__, __LINE__, o, gg->Width, gg->Height, gg->BoundsWidth, gg->BoundsHeight));

	Left = (UWORD) gg->LeftEdge;
	BoundsLeft = (UWORD) gg->BoundsLeftEdge;

	d1(KPrintF("%s/%s/%ld:  Left=%lu  BoundsLeft=%lu\n", __FILE__, __FUNC__, __LINE__, Left, BoundsLeft));

	IconWidth = gg->BoundsWidth;
	IconHeight = gg->BoundsHeight;

	d1(KPrintF("%s/%s/%ld:  argb_Width=%lu  BoundsWidth=%lu\n", __FILE__, __FUNC__, __LINE__, ImgHeader->argb_Width, gg->BoundsWidth));
	d1(KPrintF("%s/%s/%ld:  argb_Height=%lu  BoundsHeight=%lu\n", __FILE__, __FUNC__, __LINE__, ImgHeader->argb_Height, gg->BoundsHeight));

	LeftOffset = inst->iobj_imgleft + (Left - BoundsLeft);
	d1(KPrintF("%s/%s/%ld:  iobj_imgleft=%lu  iobj_imgtop=%lu  LeftOffset=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_imgleft, inst->iobj_imgtop, LeftOffset));

	if ((LeftOffset + ImgHeader->argb_Width) > IconWidth)
		{
		IconWidth = LeftOffset + ImgHeader->argb_Width;
		d1(KPrintF("%s/%s/%ld:  Image does not fit - BoundsWidth too small or iobj_imgleft too large!!\n", __FILE__, __FUNC__, __LINE__));
		}

	Size = IconWidth * IconHeight;

	MyFreeVecPooled(PubMemPool, (APTR *) &img->iargb_AlphaChannel);

	img->iargb_AlphaChannel = MyAllocVecPooled(PubMemPool, Size);
	d1(KPrintF("%s/%s/%ld: iobj_AlphaChannel=%08lx Size=%lu\n", __FILE__, __FUNC__, __LINE__, img->iargb_AlphaChannel, Size));
	if (NULL == img->iargb_AlphaChannel)
		return FALSE;

	// make sure all parts outside the image area are visible (border + text)
	memset(img->iargb_AlphaChannel, 0xff, Size);

	PicPtr = ImgHeader->argb_ImageData;
	AlphaPtr = img->iargb_AlphaChannel + inst->iobj_imgtop * IconWidth;

	d1(KPrintF("%s/%s/%ld:  argb_Width=%lu  argb_Height=%lu\n", __FILE__, __FUNC__, __LINE__, \
		ImgHeader->argb_Width, ImgHeader->argb_Height));

	for (y = 0; y < ImgHeader->argb_Height; y++)
		{
		ULONG	x;
		UBYTE	*XAlphaPtr;

		for (x = 0, XAlphaPtr = AlphaPtr + LeftOffset; x < ImgHeader->argb_Width; x++)
			{
			d1(if (XAlphaPtr > img->iargb_AlphaChannel + Size)\
				{\
				kprintf("%s/%s/%ld:  (x=%ld,y=%ld) XAlphaPtr overflow\n", __FILE__, __FUNC__, __LINE__, x, y);\
				break; \
				} );
			*XAlphaPtr++ = PicPtr->Alpha;
			PicPtr++;
			}

		AlphaPtr += IconWidth;
		}

	d1(KPrintF("%s/%s/%ld: END o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	return TRUE;
}

//-----------------------------------------------------------------------------

