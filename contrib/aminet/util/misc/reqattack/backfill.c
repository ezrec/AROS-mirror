#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <cybergraphx/cybergraphics.h>

#ifdef __MAXON__
#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>
#else
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#endif

#include "global.h"
#include "reqlist.h"
#include "backfill.h"
#include "config.h"

#include <string.h>
#include <stdio.h>

#include "myimage_protos.h"

void MyBackfillFunc(struct Hook *hook,struct RastPort *rp,struct LayerHookMsg *msg)
{
	static UWORD bgpattern[] = {0x5555,0xAAAA,0x5555};

	struct Layer *lay,*blay;
	struct MyImage *im;
	struct Screen *scr = 0;
	struct ReqNode *reqnode = (struct ReqNode *)hook->h_Data;
	WORD x1,y1,x2,y2,px,py,pw,ph;

	lay = msg->layer;
	scr = reqnode->scr;

	x1 = msg->bounds.MinX;
	y1 = msg->bounds.MinY;
	x2 = msg->bounds.MaxX;
	y2 = msg->bounds.MaxY;

	im = reqnode->actbackfillimage;

	if (MYIMAGEOK(im))
	{
		//px = (x1 - lay->bounds.MinX) % im->framewidth;
		//in this \/ case hidden buttons are refreshed OK ;)
		x1 -= reqnode->win->LeftEdge;
		x2 -= reqnode->win->LeftEdge;
		y1 -= reqnode->win->TopEdge;
		y2 -= reqnode->win->TopEdge;
		
		
		px = x1 % im->framewidth;

		pw = im->framewidth - px;
		
		do
		{
			y1 = msg->bounds.MinY - reqnode->win->TopEdge;
			//py = (y1 - lay->bounds.MinY) % im->frameheight;
			py = y1 % im->frameheight;
			
			ph = im->frameheight - py;
			
			if (pw > (x2 - x1 + 1)) pw = x2 - x1 + 1;

			do
			{
				if (ph > (y2 - y1 + 1)) ph = y2 - y1 + 1;
				
				BltBitMap(im->bm,
							 px,
							 py,
							 rp->BitMap,
							 x1 + reqnode->win->LeftEdge,
							 y1 + reqnode->win->TopEdge,
							 pw,
							 ph,
							 192,
							 255,
							 0);
							 
				y1 += ph;

				py = 1;
				ph = im->frameheight;

			} while (y1 <= y2); /* while(y1 < y2) */

			x1 += pw;

			px = 0;
			pw = im->framewidth;

		} while (x1 <= x2); /* while (x1 < x2) */
		

	} else {
		blay = rp->Layer;rp->Layer = 0;

		if (reqnode->rtg) /*j*/
		{
			FillPixelArray(rp,x1,y1,x2-x1+1,y2-y1+1,(reqnode->cfg.pens[RPEN_PATTERN1].pen));
		} else {
			SetABPenDrMd(rp,PEN(RPEN_PATTERN1),
									PEN(RPEN_PATTERN2),
									JAM2);

			if (PEN(RPEN_PATTERN1) != PEN(RPEN_PATTERN2))
			{
				SetAfPt(rp,&bgpattern[(lay->bounds.MinX ^ lay->bounds.MinY) & 1],1);
			}
	
			RectFill(rp,x1,y1,x2,y2);
		}
		rp->Layer = blay;

	} /* if (MYIMAGEOK(im)) else ... */

}

