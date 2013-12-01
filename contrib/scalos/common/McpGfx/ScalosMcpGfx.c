// ScalosMcpGfx.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/rastport.h>
#include <intuition/intuitionbase.h>
#include <libraries/mcpgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <ScalosMcpGfx.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __AROS__
// FIXME: temporary fix until we have figured out
// how to deal with these deprecated defines.
#define IA_ShadowPen    (IA_Dummy + 0x09)
#define IA_HighlightPen (IA_Dummy + 0x0A)
#endif

//----------------------------------------------------------------------------

extern struct TagItem *ScalosVTagList(ULONG FirstTag, va_list args);

//----------------------------------------------------------------------------

// local data structures

struct McpGfxPens
	{
	ULONG BgPen;
	ULONG ShadowPen;
	ULONG ShinePen;
	ULONG HalfShadowPen;
	ULONG HalfShinePen;
	};

//----------------------------------------------------------------------------

// local functions

static void DrawFrameButton(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameBorder(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameString(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameDropBox(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameXEN(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameMWB(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameThick(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);
static void DrawFrameXWin(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens);

//----------------------------------------------------------------------------

// local data items

static void (*DrawFrameTable[])(struct RastPort *, const struct Rectangle *, BOOL, const struct McpGfxPens *) =
	{
	NULL,			//#define MF_FRAME_NONE		  0	  /* No frame */
	DrawFrameButton,	//#define MF_FRAME_BUTTON	  1	  /* Standard 3D frame used for buttons */
	DrawFrameBorder,	//#define MF_FRAME_BORDER	  2	  /* Standard 2D frame used for */
	DrawFrameString,	//#define MF_FRAME_STRING	  3	  /* String */
	DrawFrameDropBox,	//#define MF_FRAME_DROPBOX	  4	  /* Dropbox [String (with space)?] */
	DrawFrameXEN,		//#define MF_FRAME_XEN		  5	  /* Standard XEN button */
	DrawFrameMWB,		//#define MF_FRAME_MWB		  6	  /* Standard MWB */
	DrawFrameThick,		//#define MF_FRAME_THICK	  7	  /* Standard Thick */
	DrawFrameXWin,		//#define MF_FRAME_XWIN		  8	  /* Standard XWIN */
	};

static const struct FrameSize FrameSizeTable[] =
	{
	{ { 0, 0, 0, 0, 0, 0, 0, 0 } },	//MF_FRAME_NONE
	{ { 1, 1, 1, 1, 1, 1, 1, 1 } },	//MF_FRAME_BUTTON
	{ { 1, 1, 1, 1, 1, 1, 1, 1 } },	//MF_FRAME_BORDER
	{ { 2, 2, 2, 2, 2, 2, 2, 2 } },	//MF_FRAME_STRING
	{ { 3, 3, 3, 3, 3, 3, 3, 3 } },	//MF_FRAME_DROPBOX
	{ { 2, 2, 2, 2, 3, 3, 1, 1 } },	//MF_FRAME_XEN
	{ { 2, 2, 2, 2, 2, 2, 2, 2 } },	//MF_FRAME_MWB
	{ { 2, 1, 2, 1, 2, 1, 2, 1 } },	//MF_FRAME_THICK
	{ { 2, 2, 2, 2, 2, 2, 2, 2 } },	//MF_FRAME_XWIN
	};

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

void McpGfxRectFill(struct RastPort *rp,
	UWORD MinX, WORD MinY, WORD MaxX, WORD MaxY, ULONG FirstTag, ...)
{
	va_list args;
	struct TagItem *TagList;
	struct RastPort rpClone = *rp;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);

	va_end(args);

	if (TagList)
		{
		UWORD *Pattern;
		ULONG PatSize;

		PatSize = GetTagData(IA_APatSize, 0, TagList);
		Pattern = (UWORD *) GetTagData(IA_APattern, (ULONG) NULL, TagList);

		if (PatSize && Pattern)
			{
			SetAfPt(&rpClone, Pattern, PatSize);
			}

		if (FindTagItem(IA_FGPen, TagList))
			{
			WORD FgColor;

			FgColor = GetTagData(IA_FGPen, 0, TagList);

			SetAPen(&rpClone, FgColor);
			}
		}

	RectFill(&rpClone, MinX, MinY, MaxX, MaxY);

	if (TagList)
		FreeTagItems(TagList);
}

//----------------------------------------------------------------------------

void McpGfxDrawFrame(struct RastPort *rp,
	UWORD MinX, WORD MinY, WORD MaxX, WORD MaxY, ULONG FirstTag, ...)
{
	va_list args;
	struct TagItem *TagList;
	ULONG FrameType;
	ULONG Recessed;
	struct Rectangle FrameRect;
	struct McpGfxPens Pens;
	struct RastPort rpClone = *rp;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);

	va_end(args);

	SetDrMd(&rpClone, JAM1);

	Pens.BgPen = GetTagData(IA_BGPen, 0, TagList);
	Pens.ShinePen = GetTagData(IA_ShinePen, 2, TagList);
	Pens.ShadowPen = GetTagData(IA_ShadowPen, 1, TagList);
	Pens.HalfShinePen = GetTagData(IA_HalfShinePen, 2, TagList);
	Pens.HalfShadowPen = GetTagData(IA_HalfShadowPen, 1, TagList);

	FrameType = GetTagData(IA_FrameType, MF_FRAME_NONE, TagList);
	Recessed =  GetTagData(IA_Recessed, FALSE, TagList);

	FrameRect.MinX = MinX;
	FrameRect.MaxX = MaxX;
	FrameRect.MinY = MinY;
	FrameRect.MaxY = MaxY;

	if (FrameType > 0 && FrameType < MF_FRAME_MAXIMUM)
		{
		if (DrawFrameTable[FrameType])
			{
			DrawFrameTable[FrameType](&rpClone, 
				&FrameRect,
				Recessed, 
				&Pens);
			}
		}

	if (TagList)
		FreeTagItems(TagList);
}

//----------------------------------------------------------------------------

static void DrawFrameButton(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MinX + 1, 	Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MinY + 1);

	SetAPen(rp, Recessed ? Pens->ShadowPen : Pens->ShinePen);
	Move(rp, Rect->MaxX - 1, 	Rect->MinY);
	Draw(rp, Rect->MinX, 		Rect->MinY);
	Draw(rp, Rect->MinX, 		Rect->MaxY - 1);
}

//----------------------------------------------------------------------------

static void DrawFrameBorder(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MinX, 		Rect->MinY);
	Draw(rp, Rect->MinX, 		Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MinY);
	Draw(rp, Rect->MinX, 		Rect->MinY);
}

//----------------------------------------------------------------------------

static void DrawFrameString(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MinX + 1, 	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1, 	Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MaxY);
	Draw(rp, Rect->MaxX, 		Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);

	SetAPen(rp, Recessed ? Pens->ShadowPen : Pens->ShinePen);

	Move(rp, Rect->MaxX,		Rect->MinY);
	Draw(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MinX,		Rect->MaxY);

	Move(rp, Rect->MaxX - 1,	Rect->MinY + 2);
	Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MinX + 1,	Rect->MaxY - 1);
}

//----------------------------------------------------------------------------

static void DrawFrameDropBox(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MaxX - 2,	Rect->MinY + 2);
	Draw(rp, Rect->MinX + 2,	Rect->MinY + 2);
	Draw(rp, Rect->MinX + 2,	Rect->MaxY - 2);

	Move(rp, Rect->MinX,		Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MinY);

	SetAPen(rp, Recessed ? Pens->ShadowPen : Pens->ShinePen);

	Draw(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MinX,		Rect->MaxY);

	Move(rp, Rect->MaxX - 2,	Rect->MinY + 3);
	Draw(rp, Rect->MaxX - 2,	Rect->MaxY - 2);
	Draw(rp, Rect->MinX + 3,	Rect->MaxY - 2);

	SetAPen(rp, Pens->BgPen);
	Move(rp, Rect->MinX + 1,	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);
}

//----------------------------------------------------------------------------

static void DrawFrameXEN(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Pens->ShadowPen);

	Move(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MaxX,		Rect->MinY);
	Draw(rp, Rect->MaxX,		Rect->MaxY);
	Draw(rp, Rect->MinX,		Rect->MaxY);
	Draw(rp, Rect->MinX,		Rect->MinY);

	if (Recessed)
		{
		SetAPen(rp, Pens->HalfShadowPen);
		Move(rp, Rect->MinX + 1,	Rect->MaxY - 1);
		Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);
		Draw(rp, Rect->MaxX - 1,	Rect->MinY + 1);

		SetAPen(rp, Pens->BgPen);
		Move(rp, Rect->MinX + 2,	Rect->MaxY - 1);
		Draw(rp, Rect->MinX + 2,	Rect->MinY + 2);
		Draw(rp, Rect->MaxX - 1,	Rect->MinY + 2);
		}
	else
		{
		SetAPen(rp, Pens->HalfShadowPen);
		Move(rp, Rect->MinX + 2,	Rect->MaxY - 1);
		Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);
		Draw(rp, Rect->MaxX - 1,	Rect->MinY + 2);

		SetAPen(rp, Pens->ShinePen);
		Move(rp, Rect->MaxX - 2,	Rect->MinY + 1);
		Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);
		Draw(rp, Rect->MinX + 1,	Rect->MaxY - 2);
		}
}

//----------------------------------------------------------------------------

static void DrawFrameMWB(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MinX + 1,	Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MinY + 1);

	SetAPen(rp, Recessed ? Pens->ShadowPen : Pens->ShinePen);

	Move(rp, Rect->MaxX - 1,	Rect->MinY);
	Draw(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MinX,		Rect->MaxY - 1);

	SetAPen(rp, Recessed ? Pens->HalfShinePen : Pens->HalfShadowPen);

	Move(rp, Rect->MinX + 2,	Rect->MaxY - 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MinY + 2);

	SetAPen(rp, Recessed ? Pens->HalfShadowPen : Pens->HalfShinePen);

	Move(rp, Rect->MaxX - 2,	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MaxY - 2);

	SetAPen(rp, Pens->BgPen);
	WritePixel(rp, Rect->MaxX - 1, 	Rect->MinY + 1);
	WritePixel(rp, Rect->MinX + 1, 	Rect->MaxY - 1);

	SetAPen(rp, Pens->HalfShadowPen);
	WritePixel(rp, Rect->MaxX, 	Rect->MinY);
	WritePixel(rp, Rect->MinX, 	Rect->MaxY);
}

//----------------------------------------------------------------------------

static void DrawFrameThick(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->ShadowPen);

	Move(rp, Rect->MinX + 1,	Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MinY);

	Move(rp, Rect->MaxX - 1,	Rect->MinY + 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);

	SetAPen(rp, Recessed ? Pens->ShadowPen : Pens->ShinePen);

	Move(rp, Rect->MinX,		Rect->MaxY);
	Draw(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MaxX - 1,	Rect->MinY);

	Move(rp, Rect->MinX + 1,	Rect->MinY + 1);
	Draw(rp, Rect->MinX + 1,	Rect->MaxY - 1);
}

//----------------------------------------------------------------------------

static void DrawFrameXWin(struct RastPort *rp, const struct Rectangle *Rect,
	BOOL Recessed, const struct McpGfxPens *Pens)
{
	SetAPen(rp, Recessed ? Pens->ShinePen : Pens->HalfShadowPen);

	Move(rp, Rect->MinX + 1,	Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MaxY);
	Draw(rp, Rect->MaxX,		Rect->MinY);

	Move(rp, Rect->MaxX - 1,	Rect->MinY + 1);
	Draw(rp, Rect->MaxX - 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MinX + 2,	Rect->MaxY - 1);

	SetAPen(rp, Recessed ? Pens->HalfShadowPen : Pens->ShinePen);

	Move(rp, Rect->MinX,		Rect->MaxY);
	Draw(rp, Rect->MinX,		Rect->MinY);
	Draw(rp, Rect->MaxX - 1,	Rect->MinY);

	Move(rp, Rect->MinX + 1,	Rect->MaxY - 1);
	Draw(rp, Rect->MinX + 1,	Rect->MinY + 1);
	Draw(rp, Rect->MaxX - 2,	Rect->MinY + 1);
}

//----------------------------------------------------------------------------

const struct FrameSize *McpGetFrameSize(UWORD frameType)
{
	if (frameType < MF_FRAME_MAXIMUM)
		return &FrameSizeTable[frameType];

	return NULL;
}
