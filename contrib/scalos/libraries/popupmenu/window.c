//
// Popup Menu Window funcs
// ©1996-2002 Henrik Isaksson
//
// $Date$
// $Revision$
//

#include "pmpriv.h"

struct RGB
{
	UBYTE	Red;
	UBYTE	Green;
	UBYTE	Blue;
};

static void PM_BlurPixelArray(struct RGB *Dest, const struct RGB *Src,
	UWORD SizeX, UWORD SizeY, ULONG Blur);


//
// Allocate memory and copy a RastPort structure
//
struct RastPort *PM_CpyRPort(struct RastPort *rp)
{
#ifdef __AROS__
    	return CloneRastPort(rp);
#else
	struct RastPort *rpc;

	rpc=PM_Mem_Alloc(sizeof(struct RastPort));
	if (rpc) {
		CopyMem(rp, rpc, sizeof(struct RastPort));
	}

	return rpc;
#endif
}

//
// Copy the region beneath a window to the transparency buffer.
//
void PM_TransparencyBfr(struct PM_Window *bw)
{
	int j, bpp;
	ULONG	transparent;
	struct RGB *TempArray = NULL;

	transparent = PM_Prefs->pmp_Flags & PMP_FLAGS_TRANSPARENCY;
#if 0
#ifndef __AROS__
	GetGUIAttrs(NULL, bw->p->DrawInfo, GUIA_MenuTransparency, &transparent, TAG_DONE);
#endif
#endif

	do	{
		ULONG BytesPerRow = sizeof(struct RGB) * bw->Width;

		if (!CyberGfx)
			break;
		if (!transparent)
			break;

		if (!GetCyberMapAttr(bw->Wnd->WScreen->RastPort.BitMap, CYBRMATTR_ISCYBERGFX))
			break;

		bpp = GetCyberMapAttr(bw->Wnd->WScreen->RastPort.BitMap, CYBRMATTR_BPPIX);
		if (bpp<2)
			break;

		// If we've gotten this far, the screen is CyberGfx and > 15 bpp

		TempArray = PM_Mem_Alloc(BytesPerRow * bw->Height);
		if (NULL == TempArray)
			break;

		bw->bg.BgArray = PM_Mem_Alloc(BytesPerRow * bw->Height);
		if (NULL == bw->bg.BgArray)
			break;

		ReadPixelArray(TempArray, 0, 0,
			BytesPerRow,
			bw->RPort,
			0, 0,
			bw->Width, bw->Height,
			RECTFMT_RGB);

		PM_BlurPixelArray((struct RGB *) bw->bg.BgArray, TempArray,
			bw->Width, bw->Height, PM_Prefs->pmp_TransparencyBlur + 1);

		// Fast and simple way to blend the background to gray...
		for(j = 0; j < BytesPerRow * bw->Height; j++)
			{
			bw->bg.BgArray[j] = (bw->bg.BgArray[j]>>2) + 150;
			}
		} while (0);

	if (TempArray)
		PM_Mem_Free(TempArray);
}

static void PM_BlurPixelArray(struct RGB *Dest, const struct RGB *Src,
	UWORD SizeX, UWORD SizeY, ULONG Blur)
{
	ULONG y;
	ULONG Divisor = Blur + 24;

	for (y = 0; y < SizeY; y++)
		{
		ULONG x;
		const struct RGB *SrcPtr = Src;
		const struct RGB *Upper, *Lower;
		const struct RGB *Upper2, *Lower2;
		struct RGB *DestPtr = Dest;

		Upper = Upper2 = Lower = Lower2 = SrcPtr;
		if (y > 0)
			Upper -= SizeX;
		if (y > 1)
			Upper2 -= 2 * SizeX;
		if (y < (SizeY - 1))
			Lower += SizeX;
		if (y < (SizeY - 2))
			Lower2 += SizeX;

		for (x = 0; x < SizeX; x++)
			{
			ULONG Left, Right;
			ULONG Left2, Right2;

			Left   = (x > 0)           ? -1 : 0;
			Left2  = (x > 1)           ? -2 : 0;
			Right  = (x < (SizeX - 1)) ?  1 : 0;
			Right2 = (x < (SizeX - 2)) ?  2 : 0;

			DestPtr->Red   = (Upper2[Left2].Red   + Upper2[Left].Red   +        Upper2[0].Red   + Upper2[Right].Red   + Upper2[Right2].Red   +
		  			   Upper[Left2].Red   +  Upper[Left].Red   +         Upper[0].Red   +  Upper[Right].Red   +  Upper[Right2].Red   +
		  			  SrcPtr[Left2].Red   +	SrcPtr[Left].Red   + Blur * SrcPtr[0].Red   + SrcPtr[Right].Red   + SrcPtr[Right2].Red   +
		  			   Lower[Left2].Red   +  Lower[Left].Red   +         Lower[0].Red   +  Lower[Right].Red   +  Lower[Right2].Red   +
		  			  Lower2[Left2].Red   +	Lower2[Left].Red   +        Lower2[0].Red   + Lower2[Right].Red   + Lower2[Right2].Red   ) / Divisor;

			DestPtr->Green = (Upper2[Left2].Green + Upper2[Left].Green +        Upper2[0].Green + Upper2[Right].Green + Upper2[Right2].Green +
		  			   Upper[Left2].Green +  Upper[Left].Green +         Upper[0].Green +  Upper[Right].Green +  Upper[Right2].Green +
		  			  SrcPtr[Left2].Green +	SrcPtr[Left].Green + Blur * SrcPtr[0].Green + SrcPtr[Right].Green + SrcPtr[Right2].Green +
		  			   Lower[Left2].Green +  Lower[Left].Green +         Lower[0].Green +  Lower[Right].Green +  Lower[Right2].Green +
		  			  Lower2[Left2].Green +	Lower2[Left].Green +        Lower2[0].Green + Lower2[Right].Green + Lower2[Right2].Green ) / Divisor;

			DestPtr->Blue  = (Upper2[Left2].Blue  + Upper2[Left].Blue  +        Upper2[0].Blue  + Upper2[Right].Blue  + Upper2[Right2].Blue  +
		  			   Upper[Left2].Blue  +  Upper[Left].Blue  +         Upper[0].Blue  +  Upper[Right].Blue  +  Upper[Right2].Blue  +
		  			  SrcPtr[Left2].Blue  +	SrcPtr[Left].Blue  + Blur * SrcPtr[0].Blue  + SrcPtr[Right].Blue  + SrcPtr[Right2].Blue  +
		  			   Lower[Left2].Blue  +  Lower[Left].Blue  +         Lower[0].Blue  +  Lower[Right].Blue  +  Lower[Right2].Blue  +
		  			  Lower2[Left2].Blue  +	Lower2[Left].Blue  +        Lower2[0].Blue  + Lower2[Right].Blue  + Lower2[Right2].Blue  ) / Divisor;

			SrcPtr++;
			Upper++;
			Upper2++;
			Lower++;
			Lower2++;
			DestPtr++;
			}

		Src += SizeX;
		Dest += SizeX;
		}
}


//
// Create an offscreen buffer for rendering animation and transition
// effects.
//
void PM_OffScreenBfr(struct PM_Window *bw)
{
	if (PM_Prefs->pmp_Animation) {
	        bw->te.BMap=AllocBitMap(bw->Width, bw->Height, bw->Wnd->WScreen->RastPort.BitMap->Depth, BMF_MINPLANES, bw->Wnd->WScreen->RastPort.BitMap);
        	if (bw->te.BMap) {
			bw->te.RPort=PM_CpyRPort(&bw->Wnd->WScreen->RastPort);
			if (bw->te.RPort) {
				bw->te.RPort->Layer = NULL; /* huuu! */
				bw->te.RPort->BitMap = bw->te.BMap;
			}
		}
	}
}

//
// Open a window
//
BOOL PM_OpenWindow(struct PM_Window *pw, int left, int top, int width, int height, struct Screen *scr)
{
	pw->bg.BgArray = NULL;
	pw->te.RPort = NULL;
	pw->te.BMap = NULL;
	pw->Wnd = OpenWindowTags(NULL,
		WA_Borderless,          TRUE,
                WA_RMBTrap,             TRUE,
                WA_Left,                left,
                WA_Top,                 top,
                WA_Width,               width,
                WA_Height,              height,
                //WA_ReportMouse,         TRUE,
                WA_CustomScreen,        scr,
                //WA_IDCMP,               IDCMP_CLOSEWINDOW,	// Kommer aldrig att inträffa - anv. för resize
                WA_SmartRefresh,        TRUE,
                WA_BackFill,            LAYERS_NOBACKFILL,
#if defined(WA_FrontWindow)
		WA_FrontWindow, TRUE,
#endif /* WA_FrontWindow */
                TAG_DONE);

	if (pw->Wnd)
		{
		pw->RPort = pw->Wnd->RPort;
		d1(KPrintF("%s/%s/%ld: pw=%08lx  RPort=%08lx\n", __FILE__, __FUNC__, __LINE__, pw, pw->RPort));

		/* Transparency/background image */
		PM_TransparencyBfr(pw);

		/* Transition effects */
		PM_OffScreenBfr(pw);

		return TRUE;
		}
	else 	
		{
        	DisplayBeep(NULL);
		return FALSE;
		}
}

//
// Close a window
//
void PM_CloseWindow(struct PM_Window *bw)
{
	if (bw->bg.BgArray) PM_Mem_Free(bw->bg.BgArray);
#ifdef __AROS__
	if (bw->te.RPort) FreeRastPort(bw->te.RPort);
#else	
	if (bw->te.RPort) PM_Mem_Free(bw->te.RPort);
#endif
	if (bw->te.BMap) FreeBitMap(bw->te.BMap);
	if (bw->Wnd) CloseWindow(bw->Wnd);

	bw->bg.BgArray = NULL;
	bw->te.RPort = NULL;
	bw->te.BMap = NULL;
	bw->Wnd = NULL;
}

//
// Resize a window
//
void PM_ResizeWindow(struct PM_Window *bw, int l, int t, int w, int h)
{
	struct Message *msg;

	if (l==bw->Wnd->LeftEdge && t==bw->Wnd->TopEdge && w==bw->Width && h==bw->Height)
		return;		// If no change

	ModifyIDCMP(bw->Wnd, IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE);
	ChangeWindowBox(bw->Wnd, l, t, w, h);
	WaitPort(bw->Wnd->UserPort);
	while ((msg=GetMsg(bw->Wnd->UserPort))) ReplyMsg(msg);
	ModifyIDCMP(bw->Wnd, IDCMP_CLOSEWINDOW);

  //     	bw->Width=bw->wnd->Width;
  //     	bw->Height=bw->wnd->Height;
}

//
// Find out if we should close our submenu
//

//
// PM_InsideWindows(px, py, wnd)
//
// px, py - Screen coords
// wnd    - PM_Window
//

BOOL PM_InsideWindows(int px, int py, struct PM_Window *wnd)
{
        int x=px;
        int y=py;
        struct PM_Window *w;

        w = wnd->Prev;

	if (w) {
		if (w->Selected) {
			if (px > w->Wnd->LeftEdge + w->Selected->Left - 2 &&
			   px < w->Wnd->LeftEdge + w->Selected->Left + w->Selected->Width + 2 &&
			   py > w->Wnd->TopEdge + w->Selected->Top - 2 &&
			   py < w->Wnd->TopEdge + w->Selected->Top + w->Selected->Height + 2)
				return FALSE;
		}
	}

        while (w) {
                if (x > w->Wnd->LeftEdge &&
		   y >= w->Wnd->TopEdge &&
		   x < w->Wnd->LeftEdge + w->Width &&
		   y < w->Wnd->TopEdge + w->Height) {
			return TRUE;
		}
                w=w->Prev;
        }

        return FALSE;
}
