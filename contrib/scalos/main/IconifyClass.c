// IconifyClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------
// Revision history :
//
// 20010902	jl	initial history
//----------------------------------------------------------------------------

// local data definitions

struct IconifyClassInst
	{
	ULONG ici_Dummy;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT IconifyClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG Iconify_New(Class *cl, Object *o, Msg msg);
static ULONG Iconify_Draw(Class *cl, Object *o, Msg msg);

static LONG RenderYPos(const struct Image *img, LONG MinY, LONG y);
static void DrawSquare(struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

Class * initIconifyClass(void)
{
	Class *IconifyClass;

	IconifyClass = MakeClass( NULL,
			(STRPTR) "imageclass",
			NULL,
			sizeof(struct IconifyClassInst),
			0 );

	if (IconifyClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(IconifyClass->cl_Dispatcher, IconifyClassDispatcher);
		}

	return IconifyClass;
}


static SAVEDS(ULONG) INTERRUPT IconifyClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = Iconify_New(cl, o, msg);
		break;

	case IM_DRAW:
		Result = Iconify_Draw(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG Iconify_New(Class *cl, Object *o, Msg msg)
{
	o = (Object *) DoSuperMethodA(cl, o, msg);

	if (o)
		{
		struct Image *img = (struct Image *) o;

		img->LeftEdge = -1;
		img->Width = 25;
		}

	return (ULONG) o;
}


static ULONG Iconify_Draw(Class *cl, Object *o, Msg msg)
{
	struct impDraw *imp = (struct impDraw *) msg;
	struct Image *img = (struct Image *) o;
	LONG MinX, MaxX, MinY, MaxY;
	LONG x;
	UBYTE Pen;

	MinX = MaxX = imp->imp_Offset.X + img->LeftEdge;
	MinY = MaxY = imp->imp_Offset.Y + img->TopEdge;
	MaxX += img->Width - 1;
	MaxY += img->Height - 1;

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
	Move(imp->imp_RPort, MinX - 1, MinY + 1);
	Draw(imp->imp_RPort, MinX - 1, MaxY);

	if (IDS_SELECTED == imp->imp_State)
		Pen = SHINEPEN;
	else
		Pen = SHADOWPEN;

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[Pen]);

	Move(imp->imp_RPort, MinX + 1, MaxY);
	Draw(imp->imp_RPort, MaxX, MaxY);
	Draw(imp->imp_RPort, MaxX, MinY);

	if (IDS_SELECTED == imp->imp_State)
		Pen = SHADOWPEN;
	else
		Pen = SHINEPEN;

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[Pen]);

	Draw(imp->imp_RPort, MinX + 1, MinY);
	Draw(imp->imp_RPort, MinX + 1, MaxY);

	switch (imp->imp_State)
		{
	case IDS_SELECTED:
	case IDS_NORMAL:
		Pen = FILLPEN;
		break;
	default:
		Pen = BACKGROUNDPEN;
		break;
		}

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[Pen]);
	RectFill(imp->imp_RPort, MinX + 2, MinY + 1,
		MaxX - 1, MaxY - 1);

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);

	if (IDS_SELECTED == imp->imp_State)
		x = MinX + 14;
	else
		x = MinX + 19;

	DrawSquare(imp->imp_RPort, x, RenderYPos(img, MinY, 25), MinX + 6, RenderYPos(img, MinY, 75));

	if (IDS_INACTIVENORMAL == imp->imp_State)
		Pen = BACKGROUNDPEN;
	else
		Pen = SHINEPEN;

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[Pen]);

	RectFill(imp->imp_RPort, MinX + 8, RenderYPos(img, MinY, 45), MinX + 11, RenderYPos(img, MinY, 60));

	SetAPen(imp->imp_RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);

	DrawSquare(imp->imp_RPort, MinX + 8, RenderYPos(img, MinY, 45), MinX + 11, RenderYPos(img, MinY, 60));

	return 1;
}


static LONG RenderYPos(const struct Image *img, LONG MinY, LONG y)
{
	return MinY + (y * img->Height) / 100;
}


static void DrawSquare(struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2)
{
	Move(rp, x1, y1);
	Draw(rp, x1, y2);
	Draw(rp, x2, y2);
	Draw(rp, x2, y1);
	Draw(rp, x1, y1);
}

