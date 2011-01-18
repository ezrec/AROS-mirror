/*
*	wintext.h
*
*	Routines for font-independent window-text system, which allows
*	writing of text based on character positions.
*	Still being tweaked.
*
*	MWS 3/92.
*/

#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <intuition/intuition.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <string.h>

#include "wintext.h"
#include "icon.h"

extern struct Library * DiskfontBase;

BOOL InitWinTextInfo(WINTEXTINFO *wti)	/* for Workbench screen at moment */
{
	struct Screen screen;

	if (GetScreenData(&screen, sizeof(screen), WBENCHSCREEN, NULL))
	{
		wti->tattr.ta_Name = TTString("FONTNAME", "topaz.font");
		wti->tattr.ta_YSize = TTInt("FONTHEIGHT", 8);

		if (wti->tf = OpenDiskFont(&wti->tattr))
		{
			wti->font_x = wti->tf->tf_XSize;
			wti->font_y = wti->tf->tf_YSize;
			wti->font_baseline = wti->tf->tf_Baseline;
			wti->toffset = screen.WBorTop;
			wti->loffset = screen.WBorLeft+1;
			wti->roffset = screen.WBorRight+1;
			wti->boffset = screen.WBorBottom;

			return TRUE;
		}
	}
	return FALSE;
}	


void RenderWinTexts(WINTEXTINFO *info, WINTEXT *wt)
{
	struct RastPort *rp;

	while (wt)
	{
		rp = info->window->RPort;
		SetAPen(rp, wt->pen);
		SetBPen(rp, wt->bg);
		SetDrMd(rp, wt->mode);
		Move(rp, info->loffset + wt->lpos*info->font_x, 
		     info->toffset + wt->tpos*info->font_y + info->font_baseline);
		Text(rp, wt->text, strlen(wt->text));
		wt = wt->next;
	}
}

/****** UNUSED AT MOMENT 

void WinText(WINTEXTINFO *info, char *text,
	     UWORD lpos, UWORD tpos, UWORD pen, UWORD mode)
{
	struct RastPort *rp;

	rp = info->window->RPort;
	SetAPen(rp, pen);
	SetDrMd(rp, mode);
	Move(rp, info->loffset + lpos*info->font_x, info->toffset + (tpos+1)*info->font_y);
	Text(rp, text, strlen(text));
}

******/
