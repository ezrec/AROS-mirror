/*********************************************************************
----------------------------------------------------------------------

	MysticView
	View Class

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <libraries/gadtools.h>
#include <intuition/intuition.h>

#include <graphics/gfxmacros.h>
#include <graphics/clip.h>

#include <clib/macros.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/layers.h>

#include "mystic_subtask.h"
#include "mystic_view.h"
//#include "mystic_debug.h"
#include "mystic_global.h"

#ifdef __MORPHOS__
#define SAVEDS
#elif defined(__AROS__)
#define SAVEDS
#include <aros/asmcall.h>
#else
#define SAVEDS __saveds
#endif


//	drawing subtask

struct picdrawer
{
	APTR subtask;
	struct picview *picview;
	BYTE drawsignal;		// this signal triggers the drawtask
	BYTE stopsignal;		// this signal triggers the drawtask
	BYTE textsignal;		// this signal triggers the drawtask
};


// macros

#define RNG(b,x,t) MIN(MAX((b),(x)),(t))		/* force b <= x <= t */

#define RED_RGB32(a) ((((a) & 0xff0000)<<8) + ((a) & 0xff0000) + (((a) & 0xff0000)>>8) + (((a) & 0xff0000)>>16))
#define GREEN_RGB32(a) ((((a) & 0x00ff00)<<16) + (((a) & 0x00ff00)<<8) + ((a) & 0x00ff00) + (((a) & 0x00ff00)>>8))
#define BLUE_RGB32(a) ((((a) & 0x0000ff)<<24) + (((a) & 0x0000ff)<<16) + (((a) & 0x0000ff)<<8) + ((a) & 0x0000ff))


// prototypes

void ClearBackground(struct picview *view);
void ResetBackground(struct picview *view, BOOL clearalways);
void FillBackground(struct picview *view, int destx, int desty, int destwidth, int destheight);
//void DrawOn(struct picview *view);
//void DrawOff(struct picview *view);
void CloseDrawer(struct picdrawer *drawer);
APTR GetDrawHandle(struct picview *view);
APTR GetQuickPic(struct picview *view);
void DeletePip(struct picview *view);
void __inline StopDrawing(struct picview *view);
void __inline StartDrawing(struct picview *view);
void GetViewAspect(struct Screen *screen, int *aspectx, int *aspecty);
void LockBackground(struct picview *view);
void UnLockBackground(struct picview *view);
void ParseViewAttrs(struct picview *view, struct TagItem *tags);
struct picdrawer *CreateDrawer(struct picview *view);



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/*********************************************************************
----------------------------------------------------------------------

	picview = CreateView(screen, rastport, tags);

----------------------------------------------------------------------
*********************************************************************/

APTR SAVE_DS ASM CreateView(
	REG(a0)	struct Screen *scr,
	REG(a1)	struct RastPort *rp,
	REG(a2) TAGLIST tags)
{
	BOOL success = FALSE;
	struct picview *view;

	if ((view = Malloclear(sizeof(struct picview))))
	{
		ULONG modeID;
		
		InitSemaphore(&view->semaphore);

		view->rastport = rp;
		view->screen = scr;
		view->colormap = scr->ViewPort.ColorMap;

		view->visualinfo = GetVisualInfo(scr, NULL);



		view->truecolor = FALSE;
		modeID = GetVPModeID(&scr->ViewPort);

		if (CyberGfxBase)
		{
			if (IsCyberModeID(modeID))
			{
				view->truecolor = (GetCyberIDAttr(CYBRIDATTR_DEPTH, modeID) > 8);
			}
		}


		if ((view->rasbuffer = AllocVec(1024, MEMF_CHIP)))
		{
			view->oldtmpras = view->rastport->TmpRas;
			view->oldareainfo = view->rastport->AreaInfo;

			InitArea(&view->areainfo, view->areabuffer, 3*4);
			InitTmpRas(&view->tmpras, view->rasbuffer, 1024);
			view->rastport->AreaInfo = &view->areainfo;
			view->rastport->TmpRas = &view->tmpras;

			success = TRUE;
		}

		if (success)
		{
			view->ham = FALSE;
			if (modeID != INVALID_ID)
			{
				DisplayInfoHandle dih;
				if((dih = FindDisplayInfo(modeID)))
				{
					struct DisplayInfo di;	
		
					if(GetDisplayInfoData(dih, (UBYTE*)&di, sizeof(struct DisplayInfo), DTAG_DISP, modeID))
					{
						view->ham = !!(di.PropertyFlags & DIPF_IS_HAM);
					}
				}
			}
	
	
			//	defaults
	
			view->backpen = -1;
			view->complementpen = -1;
			view->markpen = -1;
	
			view->backcolor = 0x0c2820;
			view->markcolor = 0x44ff44;
			view->textcolor = 0xeeffdd;
			view->precision = PRECISION_ICON;
			view->zoom = 1;
			view->izoom = 0x00010000;
	
			view->viewx = -1; 
			view->viewy = -1;
			view->viewwidth = -1;
			view->viewheight = -1;
	
			view->displaymode = MVDISPMODE_KEEPASPECT_MIN;
			view->previewmode = MVPREVMODE_NONE;
	
			view->xpos = 0.5;
			view->ypos = 0.5;
			view->ixpos = 0x00008000;
			view->iypos = 0x00008000;
	
			view->staticpalette = FALSE;
			view->dither = MVDITHERMODE_AUTO;
			view->dithermode = DITHERMODE_EDD;
			view->hstype = HSTYPE_12BIT_TURBO;
			
			view->angle = 0;
	
			view->autoditherchecked = FALSE;
			
			view->drawarrows = FALSE;
			view->drawpip = FALSE;
	
			GetViewAspect(scr, &view->viewaspectx, &view->viewaspecty);
			view->viewaspect = (float) view->viewaspectx / (float) view->viewaspecty;
	
			view->readysignal = GetTagData(MVIEW_ReadySignal, -1, tags);

			view->rastlock = (struct SignalSemaphore *) GetTagData(MVIEW_RPSemaphore, NULL, tags);
	
			ParseViewAttrs(view, tags);
	
			LockBackground(view);
		}
		
	}

	if (!success)
	{
		DeleteView(view);
		view = NULL;
	}

	return (APTR) view;
}



/*********************************************************************
----------------------------------------------------------------------

	DeleteView(view)

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM DeleteView(REG(a0) APTR mview)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{
//		SetViewPicture(view, NULL);			// löscht auch PIP

		CloseDrawer(view->picdrawer);

		ObtainSemaphore(&view->semaphore);
		
		DeletePip(view);
		DeletePenShareMap(view->psm);
		DeletePicture(view->quickpic);

		UnLockBackground(view);

		if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

		ReleaseDrawHandle(view->drawhandle);
		view->drawhandle = NULL;
		_Free(view->text);
		FreeVisualInfo(view->visualinfo);
		if (view->rasbuffer)
		{
			view->rastport->AreaInfo = view->oldareainfo;
			view->rastport->TmpRas = view->oldtmpras;
			FreeVec(view->rasbuffer);
		}

		if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]

		ReleaseSemaphore(&view->semaphore);
		_Free(view);	
	}
}


/*********************************************************************
----------------------------------------------------------------------

	SetViewAttrs(picview, tags);

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM SetViewAttrs(REG(a0) APTR mview, REG(a1) TAGLIST tags)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{	
		ParseViewAttrs(view, tags);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	GetViewAttrs(picview, tags);

----------------------------------------------------------------------
*********************************************************************/

void ASM SAVE_DS GetViewAttrs(REG(a0) APTR mview, REG(a1) TAGLIST tags)
{
	struct picview *view = (struct picview *) mview;
	if (view)
	{	
		ULONG *ptr;

		if ((ptr = (ULONG *) GetTagData(MVIEW_BackColor, NULL, tags)))
		{
			*ptr = view->backcolor;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_TextColor, NULL, tags)))
		{
			*ptr = view->textcolor;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_MarkColor, NULL, tags)))
		{
			*ptr = view->markcolor;
		}
		
		if ((ptr = (ULONG *) GetTagData(MVIEW_Precision, NULL, tags)))
		{
			*ptr = view->precision;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_DestX, NULL, tags)))
		{
			*ptr = view->viewx;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_DestY, NULL, tags)))
		{
			*ptr = view->viewy;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_DestWidth, NULL, tags)))
		{
			*ptr = view->viewwidth;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_DestHeight, NULL, tags)))
		{
			*ptr = view->viewheight;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_DisplayMode, NULL, tags)))
		{
			*ptr = view->displaymode;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_PreviewMode, NULL, tags)))
		{
			*ptr = view->previewmode;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_Picture, NULL, tags)))
		{
			*ptr = (ULONG) view->showpic;
		}

		if ((ptr = (ULONG *) GetTagData(MVIEW_Zoom, NULL, tags)))
		{
			*ptr = (ULONG) (view->zoom * 65536);
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_XPos, NULL, tags)))
		{
			*ptr = (ULONG) (view->xpos * 65536);
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_YPos, NULL, tags)))
		{
			*ptr = (ULONG) (view->ypos * 65536);
		}

		if ((ptr = (ULONG *) GetTagData(MVIEW_PictureX, NULL, tags)))
		{
			*ptr = (ULONG) view->dx + view->viewx;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_PictureY, NULL, tags)))
		{
			*ptr = (ULONG) view->dy + view->viewy;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_PictureWidth, NULL, tags)))
		{
			*ptr = (ULONG) view->dw;
		}
		if ((ptr = (ULONG *) GetTagData(MVIEW_PictureHeight, NULL, tags)))
		{
			*ptr = (ULONG) view->dh;
		}
	}
}



/*********************************************************************
----------------------------------------------------------------------

	RefreshView(view)

----------------------------------------------------------------------
*********************************************************************/

void ASM SAVE_DS RefreshView(REG(a0) APTR mview)
{
	struct picview *view = (struct picview *) mview;
	if (view)
	{
		StopDrawing(view);

		ObtainSemaphore(&view->semaphore);

		DeletePicture(view->quickpic);
		view->quickpic = NULL;

		DeletePip(view);

		view->autoditherchecked = FALSE;

		ReleaseSemaphore(&view->semaphore);

		StartDrawing(view);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	SetViewPicture(view, picture);

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM SetViewPicture(REG(a0) APTR mview, REG(a1) PICTURE *pic)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{
		StopDrawing(view);

		ObtainSemaphore(&view->semaphore);		//[

		ResetBackground(view, pic == NULL ? TRUE : FALSE);

		DeletePip(view);

		if (view->rastlock) ObtainSemaphore(view->rastlock);	//[
		
		if (view->drawhandle && !(view->staticpalette && !view->psm))
		{
			ReleaseDrawHandle(view->drawhandle);
			view->drawhandle = NULL;
		}

		if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
		
		if (view->psm)
		{
			DeletePenShareMap(view->psm);
			view->psm = NULL;
		}

		if (view->quickpic)
		{
			DeletePicture(view->quickpic);
			view->quickpic = NULL;
		}	
		
		if ((view->showpic = pic))
		{
			GetPictureAttrs(pic,
					PICATTR_Width, (IPTR)&view->picwidth,
					PICATTR_Height, (IPTR)&view->picheight, 
					PICATTR_AspectX, (IPTR)&view->picaspectx,
					PICATTR_AspectY, (IPTR)&view->picaspecty,
					TAG_DONE);
		}

		view->picaspect = (float) view->picaspecty / (float) view->picaspectx;

		view->autoditherchecked = FALSE;
			
		ReleaseSemaphore(&view->semaphore);		//]

		StartDrawing(view);
	}
}



/*********************************************************************
----------------------------------------------------------------------

	DrawOn(view);

----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM DrawOn(REG(a0) APTR mview)
{
	struct picview *view = (struct picview *) mview;
	BOOL success = FALSE;

	if (view)
	{
		ObtainSemaphore(&view->semaphore);
	
		if (!view->picdrawer)
		{
			view->picdrawer = CreateDrawer(view);
			
			if (view->picdrawer)
			{
				success = TRUE;
			}
		}
	
		ReleaseSemaphore(&view->semaphore);
	
		StartDrawing(view);
	}
	
	return success;
}


/*********************************************************************
----------------------------------------------------------------------

	DrawOff(view);

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM DrawOff(REG(a0) APTR mview)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{
		StopDrawing(view);

		ObtainSemaphore(&view->semaphore);

		if (view->picdrawer)
		{
			DB(kprintf("closing drawtask\n"));
			CloseDrawer(view->picdrawer);
			view->picdrawer = NULL;
		}

		ReleaseSemaphore(&view->semaphore);
	}
}



/*********************************************************************
----------------------------------------------------------------------

	SetViewStart(view, startx, starty)

----------------------------------------------------------------------
*********************************************************************/

void ASM SAVE_DS SetViewStart(REG(a0) APTR mview, REG(d0) LONG startx, REG(d1) LONG starty)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{
		//StopDrawing(view);

		//ObtainSemaphore(&view->semaphore);

		view->startxpos = view->xpos;
		view->startypos = view->ypos;
		view->startx = startx;
		view->starty = starty;	

		//ReleaseSemaphore(&view->semaphore);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	SetViewRelative(view, x, y, *xpos, *ypos)

----------------------------------------------------------------------
*********************************************************************/

void ASM SAVE_DS SetViewRelative(REG(a0) APTR mview, REG(d0) LONG x, REG(d1) LONG y)
{
	struct picview *view = (struct picview *) mview;

	if (view)
	{
		float va = view->viewaspect;
		float pa = view->picaspect;
		float vpa;
		float f, f1, f2;
		float sw, sh, dw, dh;
		float newxpos, newypos;
		
		float deltax = (view->startx - x);
		float deltay = (view->starty - y);


		
		f1 = (float)view->viewwidth / (float)view->picwidth * va * pa;
		f2 = (float)view->viewheight / (float)view->picheight;

		switch (view->displaymode)
		{
			case MVDISPMODE_KEEPASPECT_MIN:
				f = MIN(f1, f2);
				break;
		
			case MVDISPMODE_KEEPASPECT_MAX:
				f = MAX(f1, f2);
				break;
		
			case MVDISPMODE_ONEPIXEL:
				f = 1;
				pa = 1;
				break;

			case MVDISPMODE_IGNOREASPECT:
				f = 1;
				pa = 1;
				va = 1;
				break;

			default:
				f = 1;
				break;
		}
		
		vpa = va * pa;

		if (view->angle)
		{	
			float angle = (float) view->angle / (float) 65536;
			float sina = sin(6.283056 * -angle);
			float cosa = cos(6.283056 * -angle);
			float deltat;

			deltat = deltax;
			deltax = (deltat * cosa - deltay * sina);
			deltay = deltat * sina + deltay * cosa;
		}

		dw = RNG(2, (float)view->picwidth * f * view->zoom / vpa, view->viewwidth);
		dh = RNG(2, (float)view->picheight * f * view->zoom, view->viewheight);

		sw = (float)dw * vpa / (f * view->zoom);
		sh = (float)dh / (f * view->zoom);

		newxpos = RNG(0, view->startxpos + sw * deltax / ((view->picwidth - sw) * dw), 1);
		newypos = RNG(0, view->startypos + sh * deltay / ((view->picheight - sh) * dh), 1);

		if (newxpos != view->xpos || newypos != view->ypos)
		{
			StopDrawing(view);
			
			view->xpos = newxpos;
			view->ypos = newypos;

			StartDrawing(view);
		}

	}
}


/*********************************************************************
----------------------------------------------------------------------

	LockView(view, lock)

----------------------------------------------------------------------
*********************************************************************/

void ASM SAVE_DS LockView(REG(a0) APTR mview, REG(d0) BOOL lock)
{
#if 0
	struct picview *view = (struct picview *) mview;
	if (view)
	{
		if (lock)
		{
			view->lockcount++;
			if (view->lockcount == 1)
			{
				StopDrawing(view);
				ObtainSemaphore(&view->semaphore);
			}
		}
		else
		{
			view->lockcount--;
			if (view->lockcount == 0)
			{
				ReleaseSemaphore(&view->semaphore);
			//	StartDrawing(view);
			}
		}
	}
#endif
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void ReleasePip(struct picview *view)
{
	if (view->pip.active)
	{
		if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

		LockLayer(0, view->rastport->Layer);
		InstallClipRegion(view->rastport->Layer, NULL);
		UnlockLayer(view->rastport->Layer);

		if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]

		DisposeRegion(view->pip.region);
	}
}


void DeletePip(struct picview *view)
{
	FreeBitMap(view->pip.quickbm);
	view->pip.quickbm = NULL;
	view->pip.active = FALSE;
}


/*--------------------------------------------------------------------

	pip = CreatePip(view, sx, sy, sw, sh, dx, dy, dw, dh)

--------------------------------------------------------------------*/

BOOL CreatePip(struct picview *view, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, struct Hook *aborthook)
{
	BOOL success = FALSE;
	
	APTR drawhandle = GetDrawHandle(view);

	PICTURE *pic = view->showpic;
	
	if (!view->staticpalette && view->quickpic)
	{
		pic = view->quickpic;
	}
	
	if (drawhandle && pic)
	{
		if ((view->pip.region = NewRegion()))
		{
			int txheight = view->text ? view->rastport->TxHeight : 0;
			float pwf, phf;
			int psw, psh, psx, psy;
			int x, y;
			int pdw, pdh;
			int maxpdw = view->viewwidth / 4;
			int maxpdh = view->viewheight / 4;
			int pdxoffs = 0;
			int pdyoffs = 0;
			struct Rectangle r;
			float aspect = (float) view->viewaspecty / (float) view->viewaspectx;
	
			pwf = (float) maxpdw / (float) view->picwidth;
			phf = (float) maxpdh / (float) view->picheight;
	
			if (pwf /** aspect*/ < phf * aspect)
			{
				pdw = (int) (0.5 + aspect * pwf * (float) view->picwidth);
				pdh = (int) (0.5 + pwf * (float) view->picheight);
			}
			else
			{
				pdw = (int) (0.5 + aspect * phf * (float) view->picwidth);
				pdh = (int) (0.5 + phf * (float) view->picheight);
			}

			x = view->viewx + RNG(1, pdxoffs, view->viewwidth - pdw - 1);
			y = view->viewy + RNG(1, pdyoffs, view->viewheight - txheight - pdh - 1);
				
			view->pip.psw = psw = sw * pdw / view->picwidth;
			view->pip.psh = psh = sh * pdh / view->picheight;
			view->pip.psx = psx = sx * pdw / view->picwidth;
			view->pip.psy = psy = sy * pdh / view->picheight;

			view->pip.pdx = x - 1;
			view->pip.pdy = y - 1;

			if (pdw + 2 != view->pip.pdw || pdh + 2 != view->pip.pdh)
			{
				view->pip.pdw = pdw + 2;
				view->pip.pdh = pdh + 2;
				FreeBitMap(view->pip.quickbm);
				view->pip.quickbm = NULL;
			}
			
			if (view->pip.quickbm == NULL)
			{
				view->pip.quickbm = CreatePictureBitMap(drawhandle, pic,
					GGFX_DestWidth, pdw, GGFX_DestHeight, pdh,
					GGFX_DitherMode, DITHERMODE_EDD, 
					GGFX_CallBackHook, (IPTR)aborthook,
					TAG_DONE);
			}
			
			if (view->pip.quickbm)
			{
				if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

				BltBitMapRastPort(view->pip.quickbm, 0,0,
					view->rastport, x, y, pdw, pdh, 0xc0);

				SetAPen(view->rastport, view->backpen);
				Move(view->rastport, x - 1, y - 1);
				Draw(view->rastport, x - 1 + pdw + 1, y - 1);
				Draw(view->rastport, x - 1 + pdw + 1, y - 1 + pdh + 1);
				Draw(view->rastport, x - 1, y - 1 + pdh + 1);
				Draw(view->rastport, x - 1, y - 1);
	
				SetAPen(view->rastport, view->markpen);
				Move(view->rastport, x + psx, y + psy);
				Draw(view->rastport, x + psx + psw - 1, y + psy);
				Draw(view->rastport, x + psx + psw - 1, y + psy + psh - 1);
				Draw(view->rastport, x + psx, y + psy + psh - 1);
				Draw(view->rastport, x + psx, y + psy);
				
				SetAPen(view->rastport, view->backpen);
				Move(view->rastport, x + psx - 1, y + psy - 1);
				Draw(view->rastport, x + psx - 1 + psw + 1, y + psy - 1);
				Draw(view->rastport, x + psx - 1 + psw + 1, y + psy - 1 + psh + 1);
				Draw(view->rastport, x + psx - 1, y + psy - 1 + psh + 1);
				Draw(view->rastport, x + psx - 1, y + psy - 1);
	
				r.MinX = view->viewx;
				r.MinY = view->viewy;
				r.MaxX = view->viewx + view->viewwidth - 1;
				r.MaxY = view->viewy + view->viewheight - 1;
	
				if (OrRectRegion(view->pip.region, &r))
				{
					r.MinX = x - 1;
					r.MinY = y - 1;
					r.MaxX = x + pdw;
					r.MaxY = y + pdh;
					if (XorRectRegion(view->pip.region, &r))
					{
						LockLayer(0, view->rastport->Layer);
						InstallClipRegion(view->rastport->Layer, view->pip.region);
						UnlockLayer(view->rastport->Layer);
						success = TRUE;
					}
				}

				if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]

			}
		
			view->pip.active = success;
			
			if (!success)
			{
				DisposeRegion(view->pip.region);
			}

		}
	}

	return success;
}





/*--------------------------------------------------------------------

	DrawArrows(view, sx, sy, dx, dy, dw, dh)

--------------------------------------------------------------------*/

void DrawArrows(struct picview *view, int sx, int sy, int sw, int sh, int dw, int dh)
{
	int txheight = view->text ? view->rastport->TxHeight : 0;
	int hamwidth = view->ham ? 1 : 0;

	int ax,ay, bx,by, cx,cy, t;
	int ax2,ay2, bx2,by2, cx2,cy2;
	int x, y;
	float aspect = (float) view->viewaspecty / (float) view->viewaspectx;

	#define ARROWWIDTH1		25
	#define ARROWHEIGHT1	12
	#define ARROWWIDTH2		13
	#define ARROWHEIGHT2	6
	#define ARROWOFFS		4

//	int xsize1 = (int)((float) ARROWWIDTH1 * (float) view->viewaspecty / (float) view->viewaspectx);
//	int ysize1 = ARROWHEIGHT1;
//	int xsize2 = (int)((float) ARROWWIDTH2 * (float) view->viewaspecty / (float) view->viewaspectx);
//	int ysize2 = ARROWHEIGHT2;

	int xarrowoffs = (int)((float) ARROWOFFS * (float) view->viewaspecty / (float) view->viewaspectx);
	int yarrowoffs = ARROWOFFS;

	ax = ARROWWIDTH1 - ARROWWIDTH1 / 2; ay = 0 - ARROWHEIGHT1 / 2;
	bx = ARROWWIDTH1 / 2 - ARROWWIDTH1 / 2; by = ARROWHEIGHT1 / 2 - ARROWHEIGHT1 / 2;
	cx = ARROWWIDTH1 - ARROWWIDTH1 / 2; cy = ARROWHEIGHT1 - ARROWHEIGHT1 / 2;

	ax2 = ARROWWIDTH2 - ARROWWIDTH2 / 2; ay2 = 0 - ARROWHEIGHT2 / 2;
	bx2 = ARROWWIDTH2 / 2 - ARROWWIDTH2 / 2; by2 = ARROWHEIGHT2 / 2 - ARROWHEIGHT2 / 2;
	cx2 = ARROWWIDTH2 - ARROWWIDTH2 / 2; cy2 = ARROWHEIGHT2 - ARROWHEIGHT2 / 2;


	if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

	if (dw + hamwidth == view->viewwidth && sx > 1)	/* left */
	{
		x = view->viewx + 2;
		y = view->viewy + (view->viewheight - txheight) / 2;
		SetAPen(view->rastport, view->backpen);
		AreaMove(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaDraw(view->rastport, (int)(x + bx * aspect), y + by);
		AreaDraw(view->rastport, (int)(x + cx * aspect), y + cy);
		AreaDraw(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaEnd(view->rastport);
		SetAPen(view->rastport, view->markpen);
		AreaMove(view->rastport, (int)(x + ax2 * aspect + xarrowoffs), y + ay2);
		AreaDraw(view->rastport, (int)(x + bx2 * aspect + xarrowoffs), y + by2);
		AreaDraw(view->rastport, (int)(x + cx2 * aspect + xarrowoffs), y + cy2);
		AreaDraw(view->rastport, (int)(x + ax2 * aspect + xarrowoffs), y + ay2);
		AreaEnd(view->rastport);
	}

	t = ay; ay = -ax; ax = t;	/* turn left */
	t = cy; cy = -cx; cx = t;
	t = ay2; ay2 = -ax2; ax2 = t;	/* turn left */
	t = cy2; cy2 = -cx2; cx2 = t;

	if (dh == view->viewheight - txheight && sy + sh < view->picheight - 1)	/* down */
	{
		x = view->viewx + view->viewwidth / 2;
		y = view->viewy + view->viewheight - 1 - txheight - 2;
		SetAPen(view->rastport, view->backpen);
		AreaMove(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaDraw(view->rastport, (int)(x + bx * aspect), y + by);
		AreaDraw(view->rastport, (int)(x + cx * aspect), y + cy);
		AreaDraw(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaEnd(view->rastport);
		SetAPen(view->rastport, view->markpen);
		AreaMove(view->rastport, (int)(x + ax2 * aspect), y + ay2 - yarrowoffs);
		AreaDraw(view->rastport, (int)(x + bx2 * aspect), y + by2 - yarrowoffs);
		AreaDraw(view->rastport, (int)(x + cx2 * aspect), y + cy2 - yarrowoffs);
		AreaDraw(view->rastport, (int)(x + ax2 * aspect), y + ay2 - yarrowoffs);
		AreaEnd(view->rastport);
	}

	t = ay; ay = -ax; ax = t;	/* turn left */
	t = cy; cy = -cx; cx = t;
	t = ay2; ay2 = -ax2; ax2 = t;	/* turn left */
	t = cy2; cy2 = -cx2; cx2 = t;

	if (dw + hamwidth == view->viewwidth && sx + sw < view->picwidth - 1)	/* right */
	{
		x = view->viewx + view->viewwidth - 1 - 2;
		y = view->viewy + (view->viewheight - txheight) / 2;
		SetAPen(view->rastport, view->backpen);
		AreaMove(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaDraw(view->rastport, (int)(x + bx * aspect), y + by);
		AreaDraw(view->rastport, (int)(x + cx * aspect), y + cy);
		AreaDraw(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaEnd(view->rastport);
		SetAPen(view->rastport, view->markpen);
		AreaMove(view->rastport, (int)(x + ax2 * aspect - xarrowoffs), y + ay2);
		AreaDraw(view->rastport, (int)(x + bx2 * aspect - xarrowoffs), y + by2);
		AreaDraw(view->rastport, (int)(x + cx2 * aspect - xarrowoffs), y + cy2);
		AreaDraw(view->rastport, (int)(x + ax2 * aspect - xarrowoffs), y + ay2);
		AreaEnd(view->rastport);
	}

	t = ay; ay = -ax; ax = t;	/* turn left */
	t = cy; cy = -cx; cx = t;
	t = ay2; ay2 = -ax2; ax2 = t;	/* turn left */
	t = cy2; cy2 = -cx2; cx2 = t;

	if (dh == view->viewheight - txheight && sy > 1)	/* up */
	{
		x = view->viewx + view->viewwidth / 2;
		y = view->viewy + 2;
		SetAPen(view->rastport, view->backpen);
		AreaMove(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaDraw(view->rastport, (int)(x + bx * aspect), y + by);
		AreaDraw(view->rastport, (int)(x + cx * aspect), y + cy);
		AreaDraw(view->rastport, (int)(x + ax * aspect), y + ay);
		AreaEnd(view->rastport);
		SetAPen(view->rastport, view->markpen);
		AreaMove(view->rastport, (int)(x + ax2 * aspect), y + ay2 + yarrowoffs);
		AreaDraw(view->rastport, (int)(x + bx2 * aspect), y + by2 + yarrowoffs);
		AreaDraw(view->rastport, (int)(x + cx2 * aspect), y + cy2 + yarrowoffs);
		AreaDraw(view->rastport, (int)(x + ax2 * aspect), y + ay2 + yarrowoffs);
		AreaEnd(view->rastport);
	}

	if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]

}



/*--------------------------------------------------------------------

	DrawViewTextSimple(view, text, textlen, clearbg)
	
	no semaphore locking

--------------------------------------------------------------------*/

void DrawViewTextSimple(struct picview *view, char *text, int textlen, BOOL clearbg)
{
	//
	//		display text line
	//
	
	struct TextExtent te;
	int chars, width;


	if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

	if ((chars = TextFit(view->rastport, text, textlen,
		&te, NULL, 1, view->viewwidth, view->rastport->TxHeight)))
	{
		width = TextLength(view->rastport, text, chars);

		if (clearbg)
		{
			SetAPen(view->rastport, view->backpen);
			RectFill(view->rastport, view->viewx, view->viewy + view->viewheight - view->rastport->TxHeight,
				view->viewx + view->viewwidth - 1,
				view->viewy + view->viewheight - 1);
		}

		SetABPenDrMd(view->rastport, view->complementpen, view->backpen, JAM2);

		Move(view->rastport, view->viewx + view->viewwidth/2 - width/2,
			view->viewy + view->viewheight - view->rastport->TxHeight + view->rastport->TxBaseline);

		Text(view->rastport, text, chars);
	}

	if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
}


/*--------------------------------------------------------------------

	DrawViewText(view, clearbg)

--------------------------------------------------------------------*/

void DrawViewText(struct picview *view, BOOL clearbg)
{
//	ObtainSemaphore(&view->semaphore);

	if (view->text)
	{
		DrawViewTextSimple(view, view->text, view->textlen, clearbg);
	}

//	ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	StopDrawing(view)

--------------------------------------------------------------------*/

void __inline StopDrawing(struct picview *view)
{
	//ObtainSemaphore(&view->semaphore);

	//Forbid();

	if (view->picdrawer)
	{
		SignalSubTask(view->picdrawer->subtask, 1 << (view->picdrawer->stopsignal));
	}

	//Permit();

	//ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	StartDrawing(view)

--------------------------------------------------------------------*/

void __inline StartDrawing(struct picview *view)
{
	//ObtainSemaphore(&view->semaphore);

	//Forbid();

	if (view->picdrawer)
	{
		SignalSubTask(view->picdrawer->subtask, 1 << (view->picdrawer->drawsignal));
	}

	//Permit();

	//ReleaseSemaphore(&view->semaphore);
}



/*********************************************************************
----------------------------------------------------------------------

	ResetView(view)

----------------------------------------------------------------------
*********************************************************************/

void ResetView(struct picview *view)
{
	if (view)
	{
		StopDrawing(view);

		ObtainSemaphore(&view->semaphore);

		DeletePip(view);

		//DrawOff(view);

	//	ResetBackground(view);
		ClearBackground(view);

		ReleaseDrawHandle(view->drawhandle);
		view->drawhandle = NULL;

		DeletePicture(view->quickpic);
		view->quickpic = NULL;

		DeletePenShareMap(view->psm);
		view->psm = NULL;

		//DrawOn(view);

		view->autoditherchecked = FALSE;

		ReleaseSemaphore(&view->semaphore);
	}
}



/*--------------------------------------------------------------------

	CalcViewParameters(view, pic, sx,sy,sw,sh,dx,dy,dw,dh,cx,cy)
	
	layout calculator

--------------------------------------------------------------------*/

BOOL CalcViewParameters(struct picview *view, 
	int *sourcex, int *sourcey, int *sourcewidth, int *sourceheight,
	int *destx, int *desty, int *destwidth, int *destheight, int *centerx, int *centery)
{

	BOOL success = FALSE;

	int sx, sy, sw, sh, dx, dy, dw, dh, cx = 0, cy = 0;
	PICTURE *pic;

	if ((pic = view->showpic) && view)
	{
		int viewwidth, viewheight;
		
	//	ObtainSemaphore(&view->semaphore);

		viewwidth = view->viewwidth - (view->ham ? 1 : 0);
		viewheight = view->viewheight - (view->text ? view->rastport->TxHeight : 0);

		if (viewwidth > 0 && viewheight > 0)
		{
			int picwidth = view->picwidth, picheight = view->picheight;
			float f1, f2, f;
	
			if (picwidth > 0 && picheight > 0)
			{
				if (view->displaymode == MVDISPMODE_FIT)
				{
					dw = viewwidth;
					dh = viewheight;
					dx = 0;
					dy = 0;
					sw = picwidth;
					sh = picheight;
					sx = 0;
					sy = 0;
					cx = viewwidth/2;
					cy = viewheight/2;
				}
				else
				{
					float va = view->viewaspect;
					float pa = view->picaspect;
					float vpa;

					f1 = (float)viewwidth / (float)picwidth * va * pa;
					f2 = (float)viewheight / (float)picheight;

					switch (view->displaymode)
					{
						case MVDISPMODE_KEEPASPECT_MIN:
							f = MIN(f1, f2);
							break;
					
						case MVDISPMODE_KEEPASPECT_MAX:
							f = MAX(f1, f2);
							break;
				
						case MVDISPMODE_ONEPIXEL:
							f = 1;
							pa = 1;
							break;

						case MVDISPMODE_IGNOREASPECT:
							f = 1;
							pa = 1;
							va = 1;
							break;

						default:
							f = 1;
							break;
					}
	
					vpa = va * pa;

					if (view->angle && (view->displaymode != MVDISPMODE_FIT))
					{	
						float angle = (float) view->angle / (float) 65536;
						float sina = sin(6.283056 * angle);
						float cosa = cos(6.283056 * angle);
						WORD minx = 32767, miny = 32767, maxx = -32767, maxy = -32767;
						WORD x, y;
						float fx1, fy1, fx2, fy2;
						float fxc, fyc;
						float vw, vh;

						dw = (float)picwidth * f * view->zoom / pa;
						dh = picheight * f * view->zoom;

						vw = ((view->xpos-0.5) * MAX(0, dw - viewwidth*va) / dw) + 0.5;
						vh = ((view->ypos-0.5) * MAX(0, dh - viewheight) / dh) + 0.5;

						fx1 = -vw * (float)dw;
						fy1 = -vh *(float)dh;
						fx2 = (1-vw) * (float)dw;
						fy2 = (1-vh) * (float)dh;
						
						fxc = (-vw + view->xpos) * (float) dw;
						fyc = (-vh + view->ypos) * (float) dh;

						cx = (WORD) ((fxc * cosa - fyc * sina) / va);
						cy = (WORD) (fxc * sina + fyc * cosa);
		
						view->coordinates[0] = x = (WORD) ((fx1 * cosa - fy1 * sina) / va);
						minx = MIN(minx, x); maxx = MAX(maxx, x);
						view->coordinates[1] = y = (WORD) (fx1 * sina + fy1 * cosa);
						miny = MIN(miny, y); maxy = MAX(maxy, y);

						view->coordinates[2] = x = (WORD) ((fx2 * cosa - fy1 * sina) / va);
						minx = MIN(minx, x); maxx = MAX(maxx, x);
						view->coordinates[3] = y = (WORD) (fx2 * sina + fy1 * cosa);
						miny = MIN(miny, y); maxy = MAX(maxy, y);

						view->coordinates[4] = x = (WORD) ((fx2 * cosa - fy2 * sina) / va);
						minx = MIN(minx, x); maxx = MAX(maxx, x);
						view->coordinates[5] = y = (WORD) (fx2 * sina + fy2 * cosa);
						miny = MIN(miny, y); maxy = MAX(maxy, y);

						view->coordinates[6] = x = (WORD) ((fx1 * cosa - fy2 * sina) / va);
						minx = MIN(minx, x); maxx = MAX(maxx, x);
						view->coordinates[7] = y = (WORD) (fx1 * sina + fy2 * cosa);
						miny = MIN(miny, y); maxy = MAX(maxy, y);

						dw = RNG(2, maxx-minx, viewwidth);
						dh = RNG(2, maxy-miny, viewheight);

						dx = (viewwidth - dw) / 2 + (view->ham ? 1 : 0);
						dy = (viewheight - dh) / 2;
				
						view->coordinates[0] += dw/2;
						view->coordinates[1] += dh/2;
						view->coordinates[2] += dw/2;
						view->coordinates[3] += dh/2;
						view->coordinates[4] += dw/2;
						view->coordinates[5] += dh/2;
						view->coordinates[6] += dw/2;
						view->coordinates[7] += dh/2;
						cx += dw/2 + dx;
						cy += dh/2 + dy;
					
						sw = picwidth;
						sh = picheight;
						sx = 0;
						sy = 0;
					}
					else
					{
						dw = RNG(2, (float)picwidth * f * view->zoom / vpa, viewwidth);
						dh = RNG(2, (float)picheight * f * view->zoom, viewheight);
	
						sw = (float)dw * vpa / (f * view->zoom);
						sh = dh / (f * view->zoom);
	
						sx = view->xpos * (float)(picwidth - sw);
						sy = view->ypos * (float)(picheight - sh);
	
						dx = (viewwidth - dw) / 2 + (view->ham ? 1 : 0);
						dy = (viewheight - dh) / 2;

						if (centerx || centery)
						{
							cx = dx + (view->xpos * dw);
							cy = dy + (view->ypos * dh);
						}
					}
				}	

				view->cx = cx;
				view->cy = cy;
				view->dx = dx;
				view->dy = dy;
				view->dw = dw;
				view->dh = dh;

				if (destwidth)			*destwidth = dw;
				if (destheight)			*destheight = dh;
				if (destx)				*destx = dx;
				if (desty)				*desty = dy;
				if (sourcewidth)		*sourcewidth = sw;
				if (sourceheight)		*sourceheight = sh;
				if (sourcex)			*sourcex = sx;
				if (sourcey)			*sourcey = sy;
				if (centerx)			*centerx = cx;
				if (centery)			*centery = cy;

				success = TRUE;
			}
		}

	//	ReleaseSemaphore(&view->semaphore);
	}
	
	return success;
}



/*--------------------------------------------------------------------

	GetViewAspect(screen, *aspectx, *aspecty)

--------------------------------------------------------------------*/

void GetViewAspect(struct Screen *screen, int *aspectx, int *aspecty)
{
	ULONG modeID;
	*aspectx = 1;
	*aspecty = 1;

	if ((modeID = GetVPModeID(&screen->ViewPort)) != INVALID_ID)
	{
		DisplayInfoHandle dih;

		if((dih = FindDisplayInfo(modeID)))
		{
			struct DisplayInfo di;	

			if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(struct DisplayInfo), DTAG_DISP, modeID))
			{
				*aspectx = di.Resolution.x;
				*aspecty = di.Resolution.y;
			}
		}
	}
}


/*--------------------------------------------------------------------

	psm = GetPenShareMap(picview)

--------------------------------------------------------------------*/

APTR GetPenShareMap(struct picview *view)
{
//	ObtainSemaphore(&view->semaphore);

	if (view->staticpalette || view->truecolor)
	{
		DeletePenShareMap(view->psm);
		view->psm = NULL;
	}
	else
	{
		if (!view->psm)
		{
			if (!view->truecolor)
			{
				APTR pic;
			
				if ((pic = view->showpic))
				{
					if ((view->psm = CreatePenShareMap(GGFX_HSType, view->hstype, TAG_DONE)))
					{
						if (!AddPicture(view->psm, pic, NULL))
						{
							DeletePenShareMap(view->psm);
							view->psm = NULL;
						}
					}
				}
			}
		}
	}

//	ReleaseSemaphore(&view->semaphore);
	
	return view->psm;
}


/*--------------------------------------------------------------------

	dh = GetDrawHandle(picview)

--------------------------------------------------------------------*/

APTR GetDrawHandle(struct picview *view)
{
//	ObtainSemaphore(&view->semaphore);

	if (!view->drawhandle)
	{

		if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

		view->drawhandle = ObtainDrawHandle(GetPenShareMap(view),
				view->rastport, view->colormap, 
				GGFX_BGColor, view->backcolor,
				GGFX_ModeID, GetVPModeID(&view->screen->ViewPort),

				GGFX_AutoDither, FALSE,
				GGFX_DitherMode, DITHERMODE_NONE,

				OBP_Precision, view->precision,
				TAG_DONE);

		if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
	}

//	ReleaseSemaphore(&view->semaphore);
	
	return view->drawhandle;
}


/*--------------------------------------------------------------------

	int GetBackPen(view)

--------------------------------------------------------------------*/

int __inline GetBackPen(struct picview *view)
{
	int pen;	// = 0;
	
//	ObtainSemaphore(&view->semaphore);

//	if (view->backpen != -1)
//	{
		pen = view->backpen;
//	}

//	ReleaseSemaphore(&view->semaphore);

	return pen;
}


/*--------------------------------------------------------------------

	getquickpic

--------------------------------------------------------------------*/

APTR __inline GetQuickPic(struct picview *view)
{
//	ObtainSemaphore(&view->semaphore);

//	if (view->truecolor)
//	{
//		ReleaseSemaphore(&view->semaphore);
//		return NULL;
//	}

	if (!view->quickpic)
	{
		if (view->showpic)
		{
			if (view->truecolor)
			{
				ULONG pixelformat;
				GetPictureAttrs(view->showpic, PICATTR_PixelFormat, (IPTR)&pixelformat, TAG_DONE);
			
				if (pixelformat == PIXFMT_0RGB_32)
				{
				//	ReleaseSemaphore(&view->semaphore);
					return view->showpic;
				}
			}
		
			if (!view->quickpic)
			{
				// NORMAL OPAQUE
	
				if ((view->quickpic = ClonePicture(view->showpic, NULL)))
				{
					ULONG pixfmt;

					DoPictureMethod(view->quickpic, PICMTHD_MAPDRAWHANDLE, (IPTR)view->drawhandle, NULL);
					GetPictureAttrs(view->showpic, PICATTR_PixelFormat, (IPTR)&pixfmt, TAG_DONE);
				
					if (pixfmt != PIXFMT_CHUNKY_CLUT)
					{
						PICTURE *finalquickpic;
						if ((finalquickpic = ClonePicture(view->quickpic, NULL)))
						{
							DeletePicture(view->quickpic);
							view->quickpic = finalquickpic;
						}
					}
				}
			}
		}
	}

//	ReleaseSemaphore(&view->semaphore);

	return view->quickpic;
}


void DrawGrid(struct picview *view, int sourcex, int sourcey, int sourcewidth, int sourceheight, int destx, int desty, int destwidth, int destheight)
{
	#define		GRIDS		10

	float deststepx, deststepy;
	float deststartx, deststarty;
	int dx, dy;
	
	int i;

	deststepx = (float) view->picwidth / GRIDS * (float) destwidth / (float) sourcewidth;
	deststepy = (float) view->picheight / GRIDS * (float) destheight / (float) sourceheight;

	deststartx = (float) sourcex * (float) destwidth / (float) sourcewidth;
	deststarty = (float) sourcey * (float) destheight / (float) sourceheight;

	if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

	SetABPenDrMd(view->rastport, view->complementpen, view->backpen, JAM2);

	for (i = 0; i <= GRIDS; ++i)
	{
		dx = (int)(i * deststepx - deststartx);
	
		if (dx >= 0 && dx <= destwidth)
		{
			dx = MIN(dx, destwidth-1);
			dx += destx + view->viewx;
			Move(view->rastport, dx, view->viewy + desty);
			Draw(view->rastport, dx, view->viewy + desty + destheight - 1);
		}

		dy = (int)(i * deststepy - deststarty);
		if (dy == destheight) dy--;

		if (dy >= 0 && dy <= destheight)
		{
			dy = MIN(dy, destheight-1);
			dy += desty + view->viewy;
			Move(view->rastport, view->viewx + destx, dy);
			Draw(view->rastport, view->viewx + destx + destwidth - 1, dy);
		}
	}

	if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]

	#undef	GRIDS
}


void MarkCenter(struct picview *view, int cx, int cy, int dx, int dy, int dw, int dh)
{
	#define	CURSORSIZE	4

	int x1, y1, x2, y2, x, y;
	int viewheight = view->viewheight - (view->text ? view->rastport->TxHeight : 0);
	int xsize = (int)((float) CURSORSIZE * (float) view->viewaspecty / (float) view->viewaspectx);
	int ysize = CURSORSIZE;


	x = RNG(view->viewx + dx, view->viewx + cx, view->viewx + view->viewwidth - 1);
	y = RNG(view->viewy + dy, view->viewy + cy, view->viewy + viewheight - 1);
	x1 = RNG(view->viewx + dx, cx - xsize + view->viewx, view->viewx + view->viewwidth - 1);
	y1 = RNG(view->viewy + dy, cy - ysize + view->viewy, view->viewy + viewheight - 1);
	x2 = RNG(view->viewx + dx, cx + xsize + view->viewx, view->viewx + view->viewwidth - 1);
	y2 = RNG(view->viewy + dy, cy + ysize + view->viewy, view->viewy + viewheight - 1);

	if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

	SetAPen(view->rastport, view->backpen);

	RectFill(view->rastport, x1,y1,x2,y2);
	
	SetAPen(view->rastport, view->markpen);

	Move(view->rastport, x, y1);
	Draw(view->rastport, x, y2);
	Move(view->rastport, x1, y);
	Draw(view->rastport, x2, y);

	if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
}




/*--------------------------------------------------------------------

	picture draw function

--------------------------------------------------------------------*/

struct aborthookdata
{
	struct picview *view;
	ULONG abortsignals;
};


#ifdef __MORPHOS__
FUNC_HOOK(ULONG, abortdraw, struct Hook *, hook)
#elif defined(__AROS__)
ULONG abortdrawfunc(struct Hook *hook, APTR _obj, APTR _msg)
#else
ULONG SAVEDS ASM abortdrawfunc(REG(a0) struct Hook *hook)
#endif
{
	struct aborthookdata *data = (struct aborthookdata *) hook->h_Data;
	ULONG signals;

	signals = SetSignal(0, 0);

	if (signals & data->abortsignals)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


BOOL SAVEDS picdrawinitfunc(APTR data)
{
	struct picdrawer *drawer = (struct picdrawer *) data;

	drawer->drawsignal = AllocSignal(-1);
	drawer->stopsignal = AllocSignal(-1);
	drawer->textsignal = AllocSignal(-1);

	if (drawer->drawsignal >= 0 && drawer->stopsignal >= 0 && drawer->textsignal >= 0)
	{
		return TRUE;
	}
	else
	{
		FreeSignal(drawer->textsignal);
		FreeSignal(drawer->stopsignal);
		FreeSignal(drawer->drawsignal);
		return FALSE;
	}
}

LONG SAVEDS picdrawfunc(APTR subtask, BYTE abortsignal)
{
	ULONG signals;
	ULONG checksignals, stopsignals, abortsignals, drawsignals, textsignals, hooksignals;

	struct Hook drawsubhook;

	struct picdrawer *drawer;
	struct picview *view;
	struct aborthookdata hookdata;
	
	BOOL picturevalid = TRUE;
	
	int sx, sy, sw, sh, dx, dy, dw, dh, cx, cy;

	drawer = (struct picdrawer *) ObtainData(subtask);
	view = drawer->picview;

	textsignals = (1 << drawer->textsignal);
	drawsignals = (1 << drawer->drawsignal);
	abortsignals = (1 << abortsignal);
	stopsignals = (1 << drawer->stopsignal);

	checksignals = abortsignals + drawsignals + textsignals;
	hooksignals = abortsignals + drawsignals + stopsignals;

	hookdata.view = view;
	hookdata.abortsignals = hooksignals;

	drawsubhook.h_Data = (APTR) &hookdata;
#ifdef __MORPHOS__
	drawsubhook.h_Entry = (HOOKFUNC) &abortdraw;
#elif defined(__AROS__)
	drawsubhook.h_Entry = HookEntry;
	drawsubhook.h_SubEntry = (HOOKFUNC) abortdrawfunc; 
#else
	drawsubhook.h_Entry = (HOOKFUNC) abortdrawfunc;
#endif


	for(;;)
	{
		if (picturevalid)
		{
			signals = Wait(checksignals);
		}
		else
		{
			signals = SetSignal(0, checksignals);
		}
		
		if (signals & abortsignals)
		{
			DB(kprintf("picdrawtask received abortsignal\n"));
			break;
		}

		if (SetSignal(0, stopsignals))
		{
			DB(kprintf("picdrawtask received stopsignal\n"));
		}
		
		if (signals & drawsignals)
		{
			DB(kprintf("picdrawtask received drawsignal\n"));
			picturevalid = FALSE;
		}

		if (!picturevalid)
		{
			APTR drawhandle;
			BOOL displayed = FALSE;

			ObtainSemaphore(&view->semaphore);		//[
			
			if ((drawhandle = GetDrawHandle(view)))
			{
				if (view->showpic)
				{
					if (CalcViewParameters(view, &sx,&sy,&sw,&sh,&dx,&dy,&dw,&dh,&cx,&cy))
					{
						BOOL abort = FALSE;
					
						APTR quickpic = NULL;
						BOOL drawmainpic = FALSE;

						//
						//		get quick picture
						//

						if (view->previewmode & MVPREVMODE_OPAQUE)
						{
							quickpic = GetQuickPic(view);
						}


						if (!(quickpic && view->truecolor))
						{
							if (view->dither != MVDITHERMODE_OFF || view->ham || !quickpic)
							{
								drawmainpic = TRUE;
							}
						}


						//		check autodither

						switch (view->dither)
						{
							case MVDITHERMODE_AUTO:
								if (!view->autoditherchecked)
								{
									view->autodither = (BOOL) DoPictureMethod(view->showpic, PICMTHD_CHECKAUTODITHER, (IPTR)drawhandle, NULL);
									view->autoditherchecked = TRUE;
								}
								break;
							case MVDITHERMODE_OFF:
								view->autodither = FALSE;
								break;
							default:
								view->autodither = TRUE;
								break;
						}
						
						if (quickpic && drawmainpic)
						{
							drawmainpic = view->autodither;
						}


						if (view->drawpip)
						{
							abort = !CreatePip(view, sx, sy, sw, sh, dx, dy, dw, dh, &drawsubhook);
							if (abort) DB(kprintf("%ld pip aborted\n", ++count));
						}


						if (!abort)
						{
							if (quickpic)
							{
								if (DrawPicture(drawhandle, quickpic, view->viewx + dx, view->viewy + dy,
									RND_DestCoordinates, view->angle && (view->displaymode != MVDISPMODE_FIT) ? (IPTR)view->coordinates : NULL,
									GGFX_AutoDither, FALSE,
									GGFX_DitherMode, DITHERMODE_NONE,
									GGFX_DestWidth, dw, GGFX_DestHeight, dh,
									GGFX_RastLock, (IPTR)view->rastlock,
									GGFX_SourceWidth, sw, GGFX_SourceHeight, sh,
									GGFX_SourceX, sx, GGFX_SourceY, sy, TAG_DONE))
								{
	
									if (!drawmainpic)
									{
										picturevalid = TRUE;
										
										if (view->readysignal != -1)
										{
											SignalParentTask(view->picdrawer->subtask, 1 << view->readysignal);
										}
									}

									FillBackground(view, dx, dy, dw, dh);
								}
								else
								{
									abort = TRUE;
								}
							}
							else
							{
								if (view->previewmode == MVPREVMODE_GRID)
								{
									ClearBackground(view);
									DrawGrid(view, sx, sy, sw, sh, dx, dy, dw, dh); 
								}
								else
								{
									FillBackground(view, dx, dy, dw, dh);
								}
							}
						}


						if (!abort)
						{
							if (view->drawarrows)
							{
								DrawArrows(view, sx, sy, sw, sh, dw, dh);
							}
							if (view->markcenter)
							{
								MarkCenter(view, cx,cy,dx,dy,dw,dh);
							}
							DrawViewText(view, TRUE);
						}


						//
						//		draw high quality picture
						//
		
						if (!abort)
						{
							if (drawmainpic) 
							{
								if (DrawPicture(drawhandle, view->showpic, view->viewx + dx, view->viewy + dy,
										RND_DestCoordinates, view->angle && (view->displaymode != MVDISPMODE_FIT) ? (IPTR)view->coordinates : NULL,
										GGFX_DestWidth, dw, GGFX_DestHeight, dh,
										GGFX_SourceWidth, sw, GGFX_SourceHeight, sh,
										GGFX_SourceX, sx, GGFX_SourceY, sy,
										GGFX_RastLock, (IPTR)view->rastlock,

			//	GGFX_AutoDither, (view->dither == MVDITHERMODE_AUTO),
				GGFX_AutoDither, FALSE,
				GGFX_DitherMode, view->autodither ? view->dithermode : DITHERMODE_NONE,

									//	GGFX_DitherMode, view->dither == MVDITHERMODE_OFF ? DITHERMODE_NONE : view->dithermode,
										GGFX_CallBackHook, (IPTR)&drawsubhook,
										TAG_DONE))
								{
									picturevalid = TRUE;
									
									if (view->readysignal != -1)
									{
										SignalParentTask(view->picdrawer->subtask, 1 << view->readysignal);
									}
								}
								else
								{
									abort = TRUE;
								}
							}
						}

						if (!abort)
						{
							if (view->drawarrows)
							{
								DrawArrows(view, sx, sy, sw, sh, dw, dh);
							}
							if (view->markcenter)
							{
								MarkCenter(view, cx,cy,dx,dy,dw,dh);
							}
						}
						
						if (view->drawpip)
						{
							ReleasePip(view);
						}
					
						displayed = !abort;
					}
				}
				else
				{
					ResetBackground(view, TRUE);
					picturevalid = TRUE;
				}
			}
			
			ReleaseSemaphore(&view->semaphore);		//]
		}
		
		if (signals & textsignals)
		{
			ObtainSemaphore(&view->semaphore);
			DrawViewText(view, TRUE);
			ReleaseSemaphore(&view->semaphore);
		}

		signals = SetSignal(0, stopsignals);
	}

	DB(kprintf("***drawtask: aborted\n"));

	FreeSignal(drawer->textsignal);
	FreeSignal(drawer->stopsignal);
	FreeSignal(drawer->drawsignal);

	ReleaseData(subtask);

	DB(kprintf("***drawtask: returning\n"));

	return NULL;
}



/*--------------------------------------------------------------------

	LockBackground(view)

--------------------------------------------------------------------*/

void LockBackground(struct picview *view)
{
	ObtainSemaphore(&view->semaphore);

	if (view->backpen == -1)
	{
		view->backpen = ObtainBestPen(view->colormap,
				RED_RGB32(view->backcolor),
				GREEN_RGB32(view->backcolor),
				BLUE_RGB32(view->backcolor),
				OBP_Precision, view->precision, TAG_DONE);
	}

	if (view->complementpen == -1)
	{
		view->complementpen = ObtainBestPen(view->colormap,
				RED_RGB32(view->textcolor),
				GREEN_RGB32(view->textcolor),
				BLUE_RGB32(view->textcolor),
				OBP_Precision, PRECISION_ICON, TAG_DONE);
	}

	if (view->markpen == -1)
	{
		view->markpen = ObtainBestPen(view->colormap,
				RED_RGB32(view->markcolor),
				GREEN_RGB32(view->markcolor),
				BLUE_RGB32(view->markcolor),
				OBP_Precision, PRECISION_ICON, TAG_DONE);
	}

	ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	UnLockBackground(view)

--------------------------------------------------------------------*/

void UnLockBackground(struct picview *view)
{
	ObtainSemaphore(&view->semaphore);

	ReleasePen(view->colormap, view->markpen);
	view->markpen = -1;

	ReleasePen(view->colormap, view->backpen);
	view->backpen = -1;

	ReleasePen(view->colormap, view->complementpen);
	view->complementpen = -1;
	
	ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	ClearBackground(view)

--------------------------------------------------------------------*/

void ClearBackground(struct picview *view)
{
	int pen;
	
//	ObtainSemaphore(&view->semaphore);

	pen = GetBackPen(view);

	if (pen != -1)
	{
		if (view->viewwidth > 0 && view->viewheight > 0)
		{
			if (view->viewx >= 0 && view->viewy >= 0)
			{
				if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

				SetABPenDrMd(view->rastport, pen,pen, JAM1);
				
				RectFill(view->rastport, view->viewx, view->viewy,
					view->viewx + view->viewwidth -1,
					view->viewy + view->viewheight -1);

		//		DrawViewText(view, FALSE);

				if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
			}
		}
	}

//	ReleaseSemaphore(&view->semaphore);
}



/*--------------------------------------------------------------------

	ResetBackground(view, clearalways)

--------------------------------------------------------------------*/

void ResetBackground(struct picview *view, BOOL clearbg)
{
//	ObtainSemaphore(&view->semaphore);
	
	if (clearbg || (!view->truecolor && !view->staticpalette))
	{
		ClearBackground(view);
	}
	else
	{
		DrawViewText(view, TRUE);
	}

//	ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	FillBackground(view, dx, dy, dw, dh)

--------------------------------------------------------------------*/

void FillBackground(struct picview *view, int dx, int dy, int dw, int dh)
{
	int viewheight;

//	ObtainSemaphore(&view->semaphore);

	viewheight = view->viewheight - (view->text ? view->rastport->TxHeight : 0);

	if (view->viewwidth > 0 && view->viewheight > 0)
	{
		if (view->viewx >= 0 && view->viewy >= 0)
		{
			struct RastPort *rp;
			int pen;
			
			pen = GetBackPen(view);
			
			if (pen != -1)
			{
				if (view->rastlock) ObtainSemaphore(view->rastlock);	//[

				rp = view->rastport;
			
				SetABPenDrMd(rp, pen,pen, JAM1);
			
				if (dy)
				{
					RectFill(rp, view->viewx, view->viewy,
						view->viewx + view->viewwidth - 1,
						view->viewy + dy - 1);
				}
			
				if (dy + dh < viewheight)
				{
					RectFill(rp, view->viewx, view->viewy + dh + dy,
						view->viewx + view->viewwidth,
						view->viewy + viewheight - 1);
				}

				if (dx + dw < view->viewwidth)
				{
					RectFill(rp, view->viewx + dw + dx,
						view->viewy + dy,
						view->viewx + view->viewwidth - 1,
						view->viewy + viewheight - 1);
				}
			
				if (dx)
				{
					RectFill(rp, view->viewx, view->viewy + dy,
						view->viewx + dx - 1 - (view->ham ? 1 : 0),
						view->viewy + viewheight - 1);

					if (view->ham)
					{
						SetABPenDrMd(view->rastport, 0,0, JAM1);
						Move(view->rastport, view->viewx + dx - 1, view->viewy + dy);
						Draw(view->rastport, view->viewx + dx - 1, view->viewy + dy + dh - 1);
					}
				}
			

				if (view->rastlock) ReleaseSemaphore(view->rastlock);	//]
	
		//		DrawViewText(view, TRUE);
			}

		}
	}

//	ReleaseSemaphore(&view->semaphore);
}


/*--------------------------------------------------------------------

	ParseViewAttrs(view, struct TagItem *tags)

--------------------------------------------------------------------*/

void ParseViewAttrs(struct picview *view, struct TagItem *tags)
{
	if (view)
	{
		int oldstaticpalette;
		ULONG oldprecision;
		ULONG oldhstype;
		BOOL resetview = FALSE;
		int newviewdata = 0;
		ULONG newcolor;
		ULONG t;




		if ((t = GetTagData(MVIEW_Zoom, view->izoom, tags)) != view->izoom)
		{
			float zoom = (t >> 16) + ((float) (t & 0xffff)) / (float) 65536;
			view->zoom = RNG(0.1, zoom, 10);
			view->izoom = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_XPos, view->ixpos, tags)) != view->ixpos)
		{
			view->xpos = RNG(0, (float) t / 65536, 1);
			view->ixpos = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_YPos, view->iypos, tags)) != view->iypos)
		{
			view->ypos = RNG(0, (float) t / 65536, 1);
			view->iypos = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_Rotation, view->angle, tags) & 0xffff) != view->angle)
		{
			view->angle = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DestX, ~0, tags)) != ~0)
//		if ((t = GetTagData(MVIEW_DestX, view->viewx, tags)) != view->viewx)
		{
			view->viewx = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DestY, ~0, tags)) != ~0)
//		if ((t = GetTagData(MVIEW_DestY, view->viewy, tags)) != view->viewy)
		{
			view->viewy = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DestWidth, ~0, tags)) != ~0)
//		if ((t = GetTagData(MVIEW_DestWidth, view->viewwidth, tags)) != view->viewwidth)
		{
			view->viewwidth = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DestHeight, ~0, tags)) != ~0)
//		if ((t = GetTagData(MVIEW_DestHeight, view->viewheight, tags)) != view->viewheight)
		{
			view->viewheight = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DisplayMode, view->displaymode, tags)) != view->displaymode)
		{
			view->displaymode = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_PreviewMode, view->previewmode, tags)) != view->previewmode)
		{
			view->previewmode = t;
			newviewdata++;
			
			StopDrawing(view);
			ObtainSemaphore(&view->semaphore);
			DeletePicture(view->quickpic);
			view->quickpic = NULL;
			ReleaseSemaphore(&view->semaphore);
		}

		if ((t = GetTagData(MVIEW_ShowCursor, view->markcenter, tags)) != view->markcenter)
		{
			view->markcenter = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_ScreenAspectX, 0, tags)) != 0)
		{
			view->viewaspectx = t;
			view->viewaspect = (float) view->viewaspectx / (float) view->viewaspecty;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_ScreenAspectY, 0, tags)) != 0)
		{
			view->viewaspecty = t;
			view->viewaspect = (float) view->viewaspectx / (float) view->viewaspecty;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DitherMode, view->dithermode, tags)) != view->dithermode)
		{
			view->dithermode = t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_Dither, view->dither, tags)) != view->dither)
		{
			StopDrawing(view);
			ObtainSemaphore(&view->semaphore);
			view->dither = t;
			view->autoditherchecked = FALSE;
			ReleaseSemaphore(&view->semaphore);
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_DrawArrows, view->drawarrows, tags)) != view->drawarrows)
		{
			view->drawarrows = (BOOL) t;
			newviewdata++;
		}

		if ((t = GetTagData(MVIEW_ShowPip, view->drawpip, tags)) != view->drawpip)
		{
			StopDrawing(view);
			ObtainSemaphore(&view->semaphore);
			view->drawpip = (BOOL) t;
			ReleaseSemaphore(&view->semaphore);
			newviewdata++;
		}


		if ((t = GetTagData(MVIEW_Picture, ~0, tags)) != ~0)
		{
			StopDrawing(view);

		//	ObtainSemaphore(&view->semaphore);

			SetViewPicture(view, (APTR) t);

		//	ReleaseSemaphore(&view->semaphore);
		}

		if ((t = GetTagData(MVIEW_Text, ~0, tags)) != ~0)
		{
			char *oldt = view->text;
			BOOL changed = FALSE;

			Forbid();
			
			if (_Strcmp((char *) t, view->text) != 0)
			{
				_Free(view->text);
				view->text = NULL;
	
				if (t)
				{			
					view->text = _StrDup((char *) t);
					view->textlen = strlen(view->text);
				}
				
				changed = TRUE;
			}

			Permit();

			if (changed)
			{
				Forbid();
				if (view->picdrawer)
				{
					SignalSubTask(view->picdrawer->subtask, 1 << (view->picdrawer->textsignal));
				}
				Permit();

				if (!oldt != !t)
				{
					newviewdata++;
				}
			}
		}





		//	essential changes

		oldhstype = view->hstype;
		if ((view->hstype = (ULONG) GetTagData(MVIEW_HSType, oldhstype, tags)) != oldhstype)
		{
			resetview = TRUE;
		}

		oldstaticpalette = view->staticpalette;
		if ((view->staticpalette = GetTagData(MVIEW_StaticPalette, oldstaticpalette, tags)) != oldstaticpalette)
		{
			resetview = TRUE;
		}

		oldprecision = view->precision;
		if ((view->precision = GetTagData(MVIEW_Precision, oldprecision, tags)) != oldprecision)
		{
			resetview = TRUE;
		}

		if ((newcolor = GetTagData(MVIEW_BackColor, ~0, tags)) != ~0)
		{
			view->backcolor = newcolor;
			resetview = TRUE;
		}

		if ((newcolor = GetTagData(MVIEW_TextColor, ~0, tags)) != ~0)
		{
			view->textcolor = newcolor;
			resetview = TRUE;
		}

		if ((newcolor = GetTagData(MVIEW_MarkColor, ~0, tags)) != ~0)
		{
			view->markcolor = newcolor;
			resetview = TRUE;
		}


		//	apply changes


		if (resetview)
		{
			ResetView(view);
			StartDrawing(view);
		}
		else if (newviewdata)
		{
			StopDrawing(view);
			StartDrawing(view);
		}

	}
}





/*--------------------------------------------------------------------

	DeleteDrawer(drawer)

	free a drawer

--------------------------------------------------------------------*/

void __inline DeleteDrawer(struct picdrawer *drawer)
{
	_Free(drawer);
}


/*--------------------------------------------------------------------

	CloseDrawer(drawer)

	abort and free a drawer.

--------------------------------------------------------------------*/

void CloseDrawer(struct picdrawer *drawer)
{
	if (drawer)
	{
		AbortSubTask(drawer->subtask);
		CloseSubTask(drawer->subtask);
		DeleteDrawer(drawer);
	}
}



/*--------------------------------------------------------------------

	drawer = CreateDrawer(view)

	create a picture drawer

--------------------------------------------------------------------*/

struct picdrawer *CreateDrawer(struct picview *view)
{
	struct picdrawer *drawer;
	BOOL success = FALSE;

	if ((drawer = Malloc(sizeof(struct picdrawer))))
	{
		drawer->picview = view;
	
		if ((drawer->subtask = SubTask((APTR)picdrawfunc, drawer, 0, "MysticView ViewTask (%x)", (APTR)picdrawinitfunc, FALSE)))
		{
			success = TRUE;			
		}
	}

	if (!success)
	{
		DeleteDrawer(drawer);
		drawer = NULL;
	}

	return drawer;
}


