#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <diskfont/diskfont.h>
#include <graphics/text.h>
#include <exec/memory.h>
#include <dos/dos.h>
#ifndef NO_POPUPMENU
#include <clib/pm_protos.h>
#include <libraries/pm.h>
#endif
#include <devices/inputevent.h>
#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/cybergraphics_lib.h>
#include <pragma/layers_lib.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/layers.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#ifndef NO_POPUPMENU
#include <proto/pm.h>
#endif
#include <proto/wb.h>
#include <proto/icon.h>
#include <proto/keymap.h>
#endif

#include <clib/alib_protos.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "config.h"
#include "myimageclass.h"
#include "myimage.h"
#include "reqlist.h"
#include "patch.h"
#include "misc.h"
#include "asmmisc.h"
#include "mygadget.h"
#include "imageloader.h"
#include "anim.h"
#include "backfill.h"
#include "sound.h"
#include "myrequester.h"
#include "randomlogo.h"
#include "reqattack.h"
#include "reqoffexec.h"
#include "newmouse.h"

#include "myimage_protos.h"
#include "reqlist_protos.h"
#include "mygadget_protos.h"
#include "config_protos.h"

#include "intuition_extend.h"

#define POPUSER_ADD 0x800
#define RAPMUI_METHOD 	1
#define DISABLE_METHOD 	2
#define SLEEP_METHOD 	3
#define KEYPREFS_METHOD 4
#define ICONIFYIMAGE  (104)
#define POPUPIMAGE    (101)

#define ICONIFYID 1024
#define MENUID		1025

#define MENU_RAPID 		5
#define MENU_DISABLEID 	6
#define MENU_SLEEPID		7

#define TBICLASS "tbiclass"
#define BUCLASS "buttongclass"

#define RKBUFLEN 4

extern ULONG HookEntry();
void FreeUnusedData(struct ReqNode *node);
void MenuMethods(struct ReqNode *reqnode,BYTE method);

//static ULONG UserLogoCount;

/* Ugly workaround for GCC 2.95.1 */
struct AppIcon *AddAppIconB(ULONG id, ULONG userdata, STRPTR text, struct MsgPort *msgport, BPTR lock, struct DiskObject *diskobj, struct TagItem *taglist);

static void HilightGadget(struct ReqNode *reqnode,WORD id,BOOL on)
{
	struct Gadget *gad = reqnode->buttongadgets[id].gad;
	struct RastPort *rp = reqnode->win->RPort;
	WORD old_activegad = reqnode->activegad;

if (reqnode->cfg.pens[RPEN_GADGETHILIGHT].color != RCOLOR_IGNORE)
{

	if (on)
	{
	if (reqnode->rtg)
	{
		FillPixelArray(rp,gad->LeftEdge - reqnode->cfg.hithickx,
							gad->TopEdge - reqnode->cfg.hithicky,
							gad->Width + (reqnode->cfg.hithickx*2),reqnode->cfg.hithicky,PEN(RPEN_GADGETHILIGHT));
		FillPixelArray(rp,gad->LeftEdge + gad->Width,
							gad->TopEdge,
							reqnode->cfg.hithickx,gad->Height + reqnode->cfg.hithicky,PEN(RPEN_GADGETHILIGHT));
		FillPixelArray(rp,gad->LeftEdge - reqnode->cfg.hithickx,
							gad->TopEdge + gad->Height,
							gad->Width+(2*reqnode->cfg.hithickx),reqnode->cfg.hithicky,PEN(RPEN_GADGETHILIGHT));
		FillPixelArray(rp,gad->LeftEdge - reqnode->cfg.hithickx,
							gad->TopEdge,reqnode->cfg.hithickx,gad->Height,PEN(RPEN_GADGETHILIGHT));
		if (reqnode->cfg.roundbuttons)
		{
			FillPixelArray(rp,gad->LeftEdge,gad->TopEdge,1,1,PEN(RPEN_GADGETHILIGHT));
			FillPixelArray(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge,1,1,PEN(RPEN_GADGETHILIGHT));
			FillPixelArray(rp,gad->LeftEdge,gad->TopEdge+gad->Height-1,1,1,PEN(RPEN_GADGETHILIGHT));
			FillPixelArray(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge+gad->Height-1,1,1,PEN(RPEN_GADGETHILIGHT));
		}
	} else {
		SetABPenDrMd(rp,PEN(RPEN_GADGETHILIGHT),0,JAM2);

		RectFill(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						gad->TopEdge - reqnode->cfg.hithicky,
						gad->LeftEdge + gad->Width - 1 + reqnode->cfg.hithickx,
						gad->TopEdge - 1);

		RectFill(rp,gad->LeftEdge + gad->Width,
						gad->TopEdge,
						gad->LeftEdge + gad->Width -1 + reqnode->cfg.hithickx,
						gad->TopEdge + gad->Height - 1 + reqnode->cfg.hithicky);

		RectFill(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						gad->TopEdge + gad->Height,
						gad->LeftEdge + gad->Width - 1,
						gad->TopEdge + gad->Height - 1 + reqnode->cfg.hithicky);

		RectFill(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						gad->TopEdge,
						gad->LeftEdge - 1,
						gad->TopEdge + gad->Height - 1);

		if (reqnode->cfg.roundbuttons)
		{
			RectFill(rp,gad->LeftEdge,gad->TopEdge,gad->LeftEdge,gad->TopEdge);
			RectFill(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge,gad->LeftEdge+gad->Width-1,gad->TopEdge);
			RectFill(rp,gad->LeftEdge,gad->TopEdge+gad->Height-1,gad->LeftEdge,gad->TopEdge+gad->Height-1);
			RectFill(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge+gad->Height-1,gad->LeftEdge+gad->Width-1,gad->TopEdge+gad->Height-1);
		}
	} /* if rtg else*/
	} else {
		reqnode->activegad = -1;

		EraseRect(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						 gad->TopEdge - reqnode->cfg.hithicky,
						 gad->LeftEdge + gad->Width - 1 + reqnode->cfg.hithickx,
						 gad->TopEdge - 1);

		EraseRect(rp,gad->LeftEdge + gad->Width,
						 gad->TopEdge,
						 gad->LeftEdge + gad->Width -1 + reqnode->cfg.hithickx,
						 gad->TopEdge + gad->Height - 1 + reqnode->cfg.hithicky);

		EraseRect(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						 gad->TopEdge + gad->Height,
						 gad->LeftEdge + gad->Width - 1,
						 gad->TopEdge + gad->Height - 1 + reqnode->cfg.hithicky);

		EraseRect(rp,gad->LeftEdge - reqnode->cfg.hithickx,
						 gad->TopEdge,
						 gad->LeftEdge - 1,
						 gad->TopEdge + gad->Height - 1);

		if (reqnode->cfg.roundbuttons)
		{
			EraseRect(rp,gad->LeftEdge,gad->TopEdge,gad->LeftEdge,gad->TopEdge);
			EraseRect(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge,gad->LeftEdge+gad->Width-1,gad->TopEdge);
			EraseRect(rp,gad->LeftEdge,gad->TopEdge+gad->Height-1,gad->LeftEdge,gad->TopEdge+gad->Height-1);
			EraseRect(rp,gad->LeftEdge+gad->Width-1,gad->TopEdge+gad->Height-1,gad->LeftEdge+gad->Width-1,gad->TopEdge+gad->Height-1);
		}

	}

} else { if (!(on)) reqnode->activegad = -1;}//rcolor_ignore
	RefreshGList(gad,reqnode->win,0,1);
	reqnode->activegad = old_activegad;
}

static void HilightNext(struct ReqNode *reqnode)
{
	HilightGadget(reqnode,reqnode->activegad,FALSE);

	if (++reqnode->activegad == reqnode->num_buttongadgets)
	{
		reqnode->activegad = 0;
	}

	HilightGadget(reqnode,reqnode->activegad,TRUE);
}

static void HilightPrev(struct ReqNode *reqnode)
{
	HilightGadget(reqnode,reqnode->activegad,FALSE);
	if (--reqnode->activegad < 0)
	{
		reqnode->activegad = reqnode->num_buttongadgets - 1;
	}
	HilightGadget(reqnode,reqnode->activegad,TRUE);
}

static void GetPens2(struct ReqNode *reqnode,WORD i)
{
	LONG color;
	color = reqnode->cfg.pens[i].color;

	if (color < 0)
	{
		reqnode->cfg.pens[i].pen = reqnode->dri->dri_Pens[-color];
	} else if ((color & RCOLOR_TYPEMASK) == RCOLOR_DIRECT)
	{
		reqnode->cfg.pens[i].pen = color & RCOLOR_COLMASK;
	} else {
D(bug("calling obtainbestpen (1) colormap %lx",reqnode->cm));
		reqnode->cfg.pens[i].pen = ObtainBestPen(reqnode->cm,
						 	  RGB32(color >> 16),
						 	  RGB32((color >> 8) & 0xFF),
						 	  RGB32(color & 0xFF),
						 	  OBP_Precision,PRECISION_EXACT,
						 	  OBP_FailIfBad,TRUE,
						 	  TAG_DONE);
D(bug("obtainbestpen returned\n"));

		if (reqnode->cfg.pens[i].pen >= 0)
		{
			reqnode->cfg.pens[i].flags |= RPENF_ALLOCED;
		} else {

D(bug("calling obtainbestpen (1) colormap %lx",reqnode->cm));
			reqnode->cfg.pens[i].pen = ObtainBestPen(reqnode->cm,
							 	  RGB32(color >> 16),
							 	  RGB32((color >> 8) & 0xFF),
							 	  RGB32(color & 0xFF),
							 	  OBP_Precision,PRECISION_GUI,
							 	  OBP_FailIfBad,TRUE,
							 	  TAG_DONE);
D(bug("obtainbestpen returned(2)\n"));
			if (reqnode->cfg.pens[i].pen >= 0)
			{
				reqnode->cfg.pens[i].flags |= RPENF_ALLOCED;
			} else {
				reqnode->cfg.pens[i].pen = FindColor(reqnode->cm,
 								 RGB32(color >> 16),
 								 RGB32((color >> 8) & 0xFF),
 								 RGB32(color & 0xFF),-1);
			}
		}

	}
}

static void GetPens(struct ReqNode *reqnode)
{
	WORD i;
	LONG color;
	ULONG triplet[3];

	for(i = 0;i < NUM_RPENS;i++)
	{
		color = reqnode->cfg.pens[i].color;
D(bug("color %d of %d\n",i,NUM_RPENS));
		if (color != RCOLOR_IGNORE)
		{

			if (reqnode->rtg)
			{
D(bug("rtgcolormode\n"));
				if (color > 0)
				{
					reqnode->cfg.pens[i].pen = (0xffffff & color);
					if (color >= 0x2000000)
					{
						color = (color & 0xFF);
						GetRGB32( reqnode->cm, color, 1L, triplet );
D(bug("getrgb32 returned\n"));
						reqnode->cfg.pens[i].pen = ((triplet[0] & 0xff0000) | (triplet[1] & 0xff00) | (triplet[2] & 0xff));

					}
				} else {
					if (color < 0)
					{
						GetRGB32( reqnode->cm, reqnode->dri->dri_Pens[-color], 1L, triplet );
D(bug("getrgb32 returned (2)\n"));
						reqnode->cfg.pens[i].pen = ((triplet[0] & 0xff0000) | (triplet[1] & 0xff00) | (triplet[2] & 0xff));
					}
				}
			} else {

D(bug("calling GetPens2\n"));
				GetPens2(reqnode,i);
			}

		} /* if (color != RCOLOR_IGNORE) */

	} /* for(i = 0;i < NUM_RPENS;i++) */

	if (reqnode->rtg)
	{
				GetPens2(reqnode,RPEN_TEXTBOXFG);
				if (reqnode->cfg.pens[RPEN_TEXTBOXSHADOW].color != RCOLOR_IGNORE)
				{
					GetPens2(reqnode,RPEN_TEXTBOXSHADOW);
				}
				GetPens2(reqnode,RPEN_BUTTONFG);
				GetPens2(reqnode,RPEN_BUTTONFGSEL);
				GetPens2(reqnode,RPEN_BUTTONFGHI);
	}

}

static void MakeLogo(struct ReqNode *reqnode)
{
	char *def,*internal,*newname,*attackname;

	if (!reqnode->cfg.nologo)
	{
		if (reqnode->num_buttongadgets == 1)
		{
			def = reqnode->cfg.infologo;
			internal = defaultconfig.infologo;
		} else if (reqnode->num_buttongadgets == 2)
		{
			def = reqnode->cfg.asklogo;
			internal = defaultconfig.asklogo;
		} else {
			def = reqnode->cfg.multiasklogo;
			internal = defaultconfig.multiasklogo;
		}

		attackname = reqnode->cfg.logo;

		if (reqnode->devattack)
		{
			if (reqnode->devattack->da_Logo)
			{
			if (reqnode->devattack->da_Flags & DA_FLAGS_LOGOFROMMEM)
			{
				//some preprocessing here...
				attackname = (char *)-1;
			} else {
				if (reqnode->devattack->da_Logo[0])
				attackname = reqnode->devattack->da_Logo;
			}
			}
		}

		if (attackname == (char *)-1)
		{
			reqnode->reqimagespec = SendImageLoaderMsg(ILM_LOGOFROMMEM,reqnode->devattack->da_Logo,0);
		} else {
			if (attackname)
			{
				newname = MiscAllocVec(256);
				if (newname)
				{
					strcpy(newname,attackname);
					if (SendRandomLogo(newname,RA_RANDOMIZE,0))
					{
						reqnode->reqimagespec = GetImageSpec(newname,def,internal);
					} else {
						reqnode->reqimagespec = GetImageSpec(attackname,def,internal);
					}

					MiscFreeVec(newname);
				} else { //out of memory? let's try it this way...
					reqnode->reqimagespec = GetImageSpec(attackname,def,internal);
				}
			} else {
				reqnode->reqimagespec = GetImageSpec(reqnode->cfg.logo,def,internal);
			}
		}

		MakeMyImage(reqnode,
						&reqnode->reqimage,
						reqnode->reqimagespec,
						PEN(RPEN_TEXTBOXBG),
						(reqnode->cfg.textbackground ? TRUE : FALSE));

		if (reqnode->reqimagespec->flags & MYIMSPF_EXTERNAL)
		{
			if (attackname == (char *)-1)
			{
				SendImageLoaderMsg(ILM_FREELOGOMEM,0,reqnode->reqimagespec);
			} else {
				SendImageLoaderMsg(ILM_FREEIMAGE,0,reqnode->reqimagespec);
			}
		}

	} /* if (!reqnode->cfg.nologo) */

}

static BOOL MakeGadgets(struct ReqNode *reqnode)//,WORD *gww,WORD *ghh,WORD *gw)
{
	struct MyGadget *mygad;
	struct MyImageSpec *imspec;
	char *name,*def;
	WORD i;

//	*gww = 0;
//	*ghh = 0;
//	*gw  = 0;

	reqnode->buttonwidth = 0;
	reqnode->buttonheight = 0;
	reqnode->maxbwidth = 0;
	reqnode->maxbheight = 0;
	reqnode->buttonextrawidth = 0;
	reqnode->buttonextraheight = 0;

	mygad = reqnode->buttongadgets;
	for(i = 0;i < reqnode->num_buttongadgets;i++)
	{
		if (!reqnode->cfg.nobuttonimages)
		{
			if (i == 0)
			{
				name = reqnode->cfg.okimage;
				def  = defaultconfig.okimage;
			} else if (i == reqnode->num_buttongadgets - 1)
			{
				name = reqnode->cfg.cancelimage;
				def  = defaultconfig.cancelimage;
			} else {
				name = reqnode->cfg.thirdimage;
				def  = defaultconfig.thirdimage;
			}

			if ((LONG)name != (LONG)IMAGE_OFF)
			{
				imspec = GetImageSpec(name,def,def);

				MakeMyImage(reqnode,
								&mygad->myim,
								imspec,
								PEN(RPEN_BUTTONBG),
								TRUE);

				if (imspec->flags & MYIMSPF_EXTERNAL)
				{
					SendImageLoaderMsg(ILM_FREEIMAGE,0,imspec);
				}
			}

		} /* if (!reqnode->cfg.nobuttonimages) */

		if (!(MakeMyGadget(reqnode,
								 mygad,
								 mygad->text,
								 0,
								 0,
								 100,
								 10,
								 (i != (reqnode->num_buttongadgets - 1)) ? i + 1 : 0)))
		{
			return FALSE;
		}

		DoMethod((Object *)mygad->im,MYIM_GETSIZE,
								 &mygad->width,
								 &mygad->height);

		if (mygad->width > reqnode->maxbwidth) reqnode->maxbwidth = mygad->width;
		if (mygad->height > reqnode->maxbheight) reqnode->maxbheight = mygad->height;

		reqnode->buttonwidth += mygad->width;

		mygad++;

	} /* for(i = 0;i < reqnode->num_buttongadgets;i++) */

	if (reqnode->cfg.samebuttonwidth) reqnode->buttonwidth = reqnode->maxbwidth * reqnode->num_buttongadgets;

	reqnode->buttonextraheight = reqnode->maxbheight + reqnode->cfg.bodygadgetspacing;

	return TRUE;
}

static void MakeBodyCalcs(struct ReqNode *reqnode)//,WORD *w,WORD *h,WORD *bw,WORD *bh)
{
	WORD bw,bh;

	CalcITextSize(reqnode,reqnode->body,&bw,&bh);
	if (MYIMAGEOK(&reqnode->reqimage))
	{

		if (reqnode->cfg.logoalign==ALIGN_CENTER) {if (reqnode->cfg.logoalign2==ALIGN_CENTER){reqnode->cfg.logoalign2=ALIGN_LEFT;}}; //small protection :)

		reqnode->bodywidth = reqnode->reqimage.framewidth +
							reqnode->cfg.bodylogospacing +
							bw;

		reqnode->bodyheight = bh > reqnode->reqimage.frameheight ? bh : reqnode->reqimage.frameheight;

		if (reqnode->cfg.logoalign2==ALIGN_CENTER) //j
			{reqnode->bodyheight = bh + reqnode->cfg.bodylogospacing + reqnode->reqimage.frameheight ; reqnode->bodywidth -= reqnode->cfg.bodylogospacing;}

	} else {
		reqnode->bodywidth = bw;
		reqnode->bodyheight = bh;
	}

	reqnode->bodywidth += reqnode->cfg.innerspacingx * 2;
	reqnode->bodyheight += reqnode->cfg.innerspacingy * 2;

	reqnode->bodyx = 0; reqnode->bodyy = 0;

	reqnode->textwidth = bw; reqnode->textheight = bh;
}

static void MakeGadgetPos(struct ReqNode *reqnode,WORD *ww)//WORD w,WORD h,WORD gw,WORD gww,WORD ghh,WORD *ww)
{
	struct MyGadget *mygad;
	WORD x,y,i,sx,rh;

	x = 16 * (reqnode->scr->WBorLeft + reqnode->cfg.outerspacingx);
	y = reqnode->scr->WBorTop +
		 reqnode->scr->Font->ta_YSize + 1 +
		 reqnode->cfg.outerspacingy +
		 reqnode->bodyheight +
		 reqnode->cfg.bodygadgetspacing;

	if (reqnode->cfg.buttonyalign == BALIGN_TOP)
	{
		y -= reqnode->bodyheight + reqnode->cfg.bodygadgetspacing;
		reqnode->bodyy += reqnode->maxbheight + reqnode->cfg.bodygadgetspacing;
	}

	*ww = reqnode->buttonwidth +
		  	reqnode->cfg.buttonspacing * (reqnode->num_buttongadgets - 1);

	if ((reqnode->cfg.buttonyalign == BALIGN_TOP) | (reqnode->cfg.buttonyalign == BALIGN_BOTTOM))
	{
		sx = reqnode->cfg.buttonspacing * 16;
		if (*ww > reqnode->bodywidth)
		{
			reqnode->bodywidth = *ww;
		} else {
			if (reqnode->num_buttongadgets == 1)
			{
				switch(reqnode->cfg.buttonalign)
				{
					case ALIGN_LEFT:
						break;

					case ALIGN_CENTER:
					case ALIGN_SPREAD:
						x += (reqnode->bodywidth - *ww) * 16 / 2;
						break;

					case ALIGN_RIGHT:
						x += (reqnode->bodywidth - reqnode->maxbwidth) * 16;
						break;
				}
			} else {
				switch(reqnode->cfg.buttonalign)
				{
					case ALIGN_LEFT:
						break;

					case ALIGN_CENTER:
						x += (reqnode->bodywidth - *ww) * 16 / 2;
						break;

					case ALIGN_RIGHT:
						x += (reqnode->bodywidth - *ww) * 16;
						break;

					case ALIGN_SPREAD:
						sx = (reqnode->bodywidth - reqnode->buttonwidth) * 16 / (reqnode->num_buttongadgets - 1);
						break;
				}

			}
			*ww = reqnode->bodywidth;
			//wx = 0;
		}

		mygad = reqnode->buttongadgets;
		for(i = 0;i < reqnode->num_buttongadgets;i++)
		{
			SetAttrs(mygad->im,IA_Width,reqnode->cfg.samebuttonwidth ? reqnode->maxbwidth : mygad->width,
									 IA_Height,reqnode->maxbheight,
									 TAG_DONE);

			SetAttrs(mygad->gad,GA_Left,x / 16,
									  GA_Top,y,
									  GA_Width,mygad->im->Width,
									  GA_Height,mygad->im->Height,
									  TAG_DONE);

			x += mygad->im->Width * 16 + sx;

			mygad++;
		}
	} else {
		sx = reqnode->cfg.buttonspacing * 16;

		x = reqnode->scr->WBorLeft + reqnode->cfg.outerspacingx;
		y = 16 * (reqnode->scr->WBorTop +
				 reqnode->scr->Font->ta_YSize + 1 +
				 reqnode->cfg.outerspacingy);

		*ww = reqnode->bodywidth;
		reqnode->buttonextrawidth = reqnode->maxbwidth + reqnode->cfg.bodygadgetspacing;
		reqnode->buttonwidth = reqnode->maxbwidth;

		rh = (reqnode->maxbheight) * reqnode->num_buttongadgets;
		rh += reqnode->cfg.buttonspacing * (reqnode->num_buttongadgets -1);

		reqnode->buttonextraheight = 0;

		if (reqnode->cfg.buttonyalign == BALIGN_LEFT)
		{
			reqnode->bodyx += reqnode->maxbwidth + reqnode->cfg.bodygadgetspacing;
		} else {
			x += reqnode->bodywidth + reqnode->cfg.bodygadgetspacing;
		}

		if (rh > reqnode->bodyheight)
		{
			reqnode->bodyheight = rh;
		} else {
			if (reqnode->num_buttongadgets == 1)
			{
				switch(reqnode->cfg.buttonalign)
				{
					case ALIGN_LEFT:
						break;

					case ALIGN_CENTER:
					case ALIGN_SPREAD:
						y += (reqnode->bodyheight - rh) * 16 / 2;
						break;

					case ALIGN_RIGHT:
						y += (reqnode->bodyheight - reqnode->maxbheight) * 16;
						break;
				}
			} else {
				switch(reqnode->cfg.buttonalign)
				{
					case ALIGN_LEFT:
						break;

					case ALIGN_CENTER:
						y += (reqnode->bodyheight - rh) * 16 / 2;
						break;

					case ALIGN_RIGHT:
						y += (reqnode->bodyheight - rh) * 16;
						break;

					case ALIGN_SPREAD:
						sx = (reqnode->bodyheight - (reqnode->maxbheight * reqnode->num_buttongadgets)) * 16 / (reqnode->num_buttongadgets - 1);
						break;
				}

			}
		}

		mygad = reqnode->buttongadgets;
		for(i = 0;i < reqnode->num_buttongadgets;i++)
		{
			SetAttrs(mygad->im,IA_Width,reqnode->maxbwidth,
									 IA_Height,reqnode->maxbheight,
									 TAG_DONE);

			SetAttrs(mygad->gad,GA_Left,x,
									  GA_Top,y /16,
									  GA_Width,mygad->im->Width,
									  GA_Height,mygad->im->Height,
									  TAG_DONE);

			y += (mygad->im->Height * 16) + sx;

			mygad++;
		}
	}

}

static struct Window *MakeWindow(struct ReqNode *reqnode,WORD winw,WORD winh,ULONG idcmp)
{
	struct Window *rc;
	WORD x,y;
	struct IntuiMessage *msg;

		reqnode->backfillhook.h_Entry = (HOOKFUNC)HookEntry;
		reqnode->backfillhook.h_SubEntry = (HOOKFUNC)MyBackfillFunc;
		reqnode->backfillhook.h_Data = reqnode;

D(bug("MW: 1\n"));

	if (reqnode->cfg.backfill)
	{
		if ((reqnode->backfillimagespec = GetImageSpec(reqnode->cfg.backfill,0,0)))
		{
			MakeMyImage(reqnode,
							&reqnode->backfillimage,
							reqnode->backfillimagespec,
							-1,
							FALSE);

			if (reqnode->backfillimagespec->flags & MYIMSPF_EXTERNAL)
			{
				SendImageLoaderMsg(ILM_FREEIMAGE,0,reqnode->backfillimagespec);
			}
		}
	}

D(bug("MW:2\n"));
	if (reqnode->cfg.textbackground)
	{
		if ((reqnode->textbackfillimagespec = GetImageSpec(reqnode->cfg.textbackground,0,0)))
		{
			MakeMyImage(reqnode,
							&reqnode->textbackfillimage,
							reqnode->textbackfillimagespec,
							-1,
							FALSE);

			if (reqnode->textbackfillimagespec->flags & MYIMSPF_EXTERNAL)
			{
				SendImageLoaderMsg(ILM_FREEIMAGE,0,reqnode->textbackfillimagespec);
			}
		}
	}

D(bug("MW:3\n"));
	if (reqnode->cfg.buttonbackfill)
	{
		if ((reqnode->buttonbackfillimagespec = GetImageSpec(reqnode->cfg.buttonbackfill,0,0)))
		{
			MakeMyImage(reqnode,
							&reqnode->buttonbackfillimage,
							reqnode->buttonbackfillimagespec,
							-1,
							FALSE);

			if (reqnode->buttonbackfillimagespec->flags & MYIMSPF_EXTERNAL)
			{
				SendImageLoaderMsg(ILM_FREEIMAGE,0,reqnode->buttonbackfillimagespec);
			}
		}
	}

	if (!MYIMAGEOK(&reqnode->textbackfillimage))
	{
		FreeMyImageMask(&reqnode->reqimage);
	} else {
		if (MYIMAGEOK(&reqnode->reqimage) &&
			 (reqnode->reqimage.frames > 1) &&
			 (reqnode->reqimage.mask))
		{
			AllocMyImageHelpBM(&reqnode->reqimage,
									 reqnode->scr->RastPort.BitMap);
		}
	}

	switch(reqnode->cfg.reqpos)
	{
		case RPOS_NORMAL:
			x = -reqnode->scr->LeftEdge;
			y = -reqnode->scr->TopEdge;
			break;

		case RPOS_CENTER:
			VisibleScreenSize(reqnode->scr,&x,&y);
			x = (x - winw) / 2 - reqnode->scr->LeftEdge;
			y = (y - winh) / 2 - reqnode->scr->TopEdge;
			break;

		case RPOS_MOUSE:
			x = reqnode->scr->MouseX - (winw / 2);
			y = reqnode->scr->MouseY - (winh / 2);
			break;
	}

	if (reqnode->win)
	{
		ChangeWindowBox(reqnode->win,x,y,winw,winh);
		WindowToFront(reqnode->win);
		ActivateWindow(reqnode->win);
		x=2;
		while (x)
		{
			WaitPort(reqnode->win->UserPort);
			msg = (struct IntuiMessage *)GetMsg(reqnode->win->UserPort);
			if (msg->Class == IDCMP_CHANGEWINDOW) x--;
			if (msg->Class == IDCMP_NEWSIZE) x--;
			ReplyMsg((struct Message *)msg);
		}
		rc = reqnode->win;
	} else {
D(bug("MW:4\n"));
	rc = OpenWindowTags(0,WA_CustomScreen,(LONG)reqnode->scr,
			 WA_Title,(LONG)reqnode->title,
			 WA_Left,x,
			 WA_Top,y,
			 WA_Width,winw,
			 WA_Height,winh,
			 WA_IDCMP,IDCMP_GADGETUP |
			 			 IDCMP_RAWKEY |
			 			 IDCMP_ACTIVEWINDOW |
			 			 IDCMP_INACTIVEWINDOW |
			 			 IDCMP_CHANGEWINDOW |
			 			 IDCMP_MOUSEBUTTONS |
			 			 IDCMP_NEWSIZE |
			 			 (idcmp & ~IDCMP_VANILLAKEY),
			 WA_Flags,WFLG_DRAGBAR |
			 			 WFLG_DEPTHGADGET |
			 			 WFLG_ACTIVATE |
			 			 WFLG_RMBTRAP,
			 WA_AutoAdjust,TRUE,
 			WA_ExtraGadget_Iconify,TRUE,
 			WA_ExtraGadget_PopUp,TRUE,
			 TAG_DONE);
	}

	if (rc)
	{
D(bug("MW:5\n"));
		reqnode->win = rc;

		reqnode->bodyx += rc->BorderLeft + reqnode->cfg.outerspacingx;
		reqnode->bodyy += rc->BorderTop + reqnode->cfg.outerspacingy;

		InstallLayerHook(rc->WLayer,&reqnode->backfillhook);

		reqnode->actbackfillimage = &reqnode->backfillimage;

		EraseRect(rc->RPort,rc->BorderLeft,
							rc->BorderTop,
							rc->Width - rc->BorderRight - 1,
							reqnode->bodyy);
		EraseRect(rc->RPort,rc->BorderLeft,
							reqnode->bodyy,
							reqnode->bodyx,
							rc->Height - rc->BorderBottom - 1);
		EraseRect(rc->RPort,reqnode->bodyx + reqnode->bodywidth,
							reqnode->bodyy,
							rc->Width - rc->BorderRight - 1,
							rc->Height - rc->BorderBottom - 1);
		EraseRect(rc->RPort,rc->BorderLeft,
							reqnode->bodyheight + reqnode->bodyy,
							rc->Width - rc->BorderRight - 1,
							rc->Height - rc->BorderBottom - 1);

		if (MYIMAGEOK(&reqnode->textbackfillimage))
		{
			reqnode->actbackfillimage = &reqnode->textbackfillimage;
			EraseRect(rc->RPort,reqnode->bodyx,reqnode->bodyy,reqnode->bodyx + reqnode->bodywidth - 1, reqnode->bodyy + reqnode->bodyheight - 1);
			reqnode->actbackfillimage = &reqnode->backfillimage;

			FreeMyImage(&reqnode->textbackfillimage);

		} else {
			if (PEN(RPEN_TEXTBOXBG) != 0)
			{
				if (reqnode->rtg)
				{
					FillPixelArray(rc->RPort,reqnode->bodyx,reqnode->bodyy,reqnode->bodywidth,reqnode->bodyheight,PEN(RPEN_TEXTBOXBG));
				} else {
					SetAPen(rc->RPort,PEN(RPEN_TEXTBOXBG));
					RectFill(rc->RPort,reqnode->bodyx,reqnode->bodyy,reqnode->bodyx + reqnode->bodywidth - 1, reqnode->bodyy + reqnode->bodyheight - 1);
				}
			}
		}
		DrawFrame(reqnode,
					 rc->RPort,
					 reqnode->bodyx,
					 reqnode->bodyy,
					 reqnode->bodyx + reqnode->bodywidth - 1,
					 reqnode->bodyy + reqnode->bodyheight - 1,
					 RPEN_OTEXTBOXEDGE1,
					 RPEN_OTEXTBOXEDGE2,
					 RPEN_ITEXTBOXEDGE1,
					 RPEN_ITEXTBOXEDGE2,
					 -1,FALSE);

D(bug("MW:6\n"));
		SetFont(rc->RPort,reqnode->font);

	} /* if (rc) */

	return rc;
}

struct Window *MakeMyRequester(struct ReqNode *reqnode,struct Window *win,ULONG idcmp)
{
	struct MyGadget *mygad;
	//struct Gadget *string;
	struct Screen *scr;
	WORD i,x,y,ww,realdepth,
		  winw,winh,logox = 0,logoy = 0; //,lx;

	// WORD bshiftx=0,bshifty=0;

	struct Window *rc = 0;
	struct TextFont *txfont;
	struct TextAttr *ta;
	UWORD	ysize;
	struct IntuiText *it;

	BOOL doconfig=TRUE;

	D(bug("MMR: Init\n"));

	if (reqnode->win) doconfig=FALSE;

		InitExecuteConfig();

		D(bug("MMR: ExecuteConfig\n"));
		ExecuteConfig(reqnode);

	if (doconfig)
	{
		if (reqnode->cfg.reqoff > -1)
		{
			if (reqnode->cfg.reqoffexec)
			{
				ReqOFFExecute(reqnode);
			}
			if (reqnode->cfg.reqoff > -1)
			{
				reqnode->win = OpenWindowTags(0,WA_Left,0,WA_Top,0,WA_Width,1,WA_Height,1,WA_Flags,WFLG_BACKDROP|WFLG_BORDERLESS,TAG_DONE);
				rc = reqnode->win;
				goto done;
			}
		}

		if (reqnode->cfg.reqoff == -2)
		{
			reqnode->cfg.reqoff = -1;
			reqnode->win = OpenWindowTags(0,WA_CustomScreen,(LONG)reqnode->scr,
				 			WA_Title,(LONG)reqnode->title,
							WA_Left,0,
							WA_Top,0,
							WA_Width,1,
							WA_Height,1,
							WA_IDCMP,IDCMP_GADGETUP |
				 			IDCMP_RAWKEY |
				 			IDCMP_ACTIVEWINDOW |
				 			IDCMP_INACTIVEWINDOW |
				 			IDCMP_CHANGEWINDOW |
				 			IDCMP_MOUSEBUTTONS |
				 			IDCMP_NEWSIZE |
				 			(idcmp & ~IDCMP_VANILLAKEY),
				 			WA_Flags,WFLG_DRAGBAR |
				 			WFLG_DEPTHGADGET |
				 			WFLG_ACTIVATE |
				 			WFLG_RMBTRAP,
				 			WA_AutoAdjust,TRUE,
				 			WA_ExtraGadget_Iconify,TRUE,
				 			WA_ExtraGadget_PopUp,TRUE,
							TAG_DONE); //vanilla key NOT allowed!

			if (reqnode->win)
			{
				if (reqnode->bcfg.info)
				{
					if (reqnode->bcfg.info[0]) reqnode->appicondiskobject = (struct DiskObject *)SendRandomLogo(reqnode->bcfg.info,RA_APPICON,0);
				}

				if (!(reqnode->appicondiskobject)) reqnode->appicondiskobject = GetDiskObject("ENV:ReqAttack");

				if (reqnode->appicondiskobject)
				{
					// reset icon position
					reqnode->appicondiskobject->do_CurrentX = 0;
					reqnode->appicondiskobject->do_CurrentY = 0;

					reqnode->appicon = AddAppIconB(1,(ULONG)reqnode,
												reqnode->title,
												reqnode->win->UserPort,
												NULL,reqnode->appicondiskobject,
												NULL);
					if (reqnode->appicon)
					{
						//now free the resources :)
						FreeUnusedData(reqnode);
						WindowToBack(reqnode->win);
					} else {
						FreeDiskObject(reqnode->appicondiskobject);
						reqnode->appicondiskobject = 0;
						CloseWindow(reqnode->win);
						reqnode->win = 0;
					}
				} //if diskobject
			}

			rc = reqnode->win;
			goto done;
		} // if reqoff == -2
	} // if doconfig

	if (!(reqnode->cfg.donotpatch))
	{
		if (doconfig)
		{
			reqnode->customidcmp = (idcmp & ~IDCMP_VANILLAKEY);

			if (CyberGfxBase)
			{
				if (IsCyberModeID(GetVPModeID(reqnode->vp)))
				{
					realdepth = GetBitMapAttr( reqnode->scr->RastPort.BitMap, BMA_DEPTH );
					if (realdepth > 8)
					{
						reqnode->rtg = TRUE;
						reqnode->cfg.bitmapflags = BITMAPF_MINPLANES|BITMAPF_FRIEND;
					}
				}
			}
		} //if doconfig

		reqnode->fontok = FALSE;
		reqnode->buttonfontok = FALSE;
		reqnode->buttonfont = reqnode->font;

	if (reqnode->cfg.textfont)
	{
		if ((ta = MiscAllocVec(sizeof(*ta))))
		{
			ta->ta_Name = reqnode->cfg.textfont;
			ta->ta_YSize = reqnode->cfg.textfontsize;
			ta->ta_Style = 0;
			ta->ta_Flags = 0;
			txfont = OpenDiskFont(ta);
			MiscFreeVec(ta);
			if (!(txfont == 0))
			{
				reqnode->font = txfont;
				reqnode->fontok = TRUE;
				/*recalculate text positions*/
				it = reqnode->body;
				it->TopEdge = 0;/*it->TopEdge - ysize + reqnode->font->tf_YSize;*/
				ysize = reqnode->font->tf_YSize - reqnode->font->tf_Baseline;
				y = 1;
				if (ysize < 1) y = 1;
				y += reqnode->font->tf_YSize -1;
				while (it)
				{
					if (it->NextText)
					{
						it->NextText->TopEdge = it->TopEdge + y;/*it->NextText->TopEdge - ysize + reqnode->font->tf_YSize;*/
						it = it->NextText;
					} else
					{
						it = 0;
					}
				}
			} else {
				reqnode->font = reqnode->dri->dri_Font;
				reqnode->fontok = FALSE;
			}
		}
	}

	if (reqnode->cfg.buttonfont)
	{
		if ((ta = MiscAllocVec(sizeof(*ta))))
		{
			ta->ta_Name = reqnode->cfg.buttonfont;
			ta->ta_YSize = reqnode->cfg.buttonfontsize;
			ta->ta_Style = 0;
			ta->ta_Flags = 0;
			txfont = OpenDiskFont(ta);
			MiscFreeVec(ta);
			if (!(txfont == 0))
			{
				reqnode->buttonfont = txfont;
				reqnode->buttonfontok = TRUE;
			}
		}
	}

		if (reqnode->devattack)
		{
			reqnode->activegad = reqnode->devattack->da_StartButton;
		}

		D(bug("MMR: Gfx stuff init\n"));
		GetPens(reqnode);
		D(bug("MMR: Gfx stuff: pens allocated\n"));
		MakeLogo(reqnode);
		D(bug("MMR: Gfx stuff: logo loaded\n"));

		if (!MakeGadgets(reqnode)) goto done;
		D(bug("MMR: Gfx stuff gadgets\n"));
		MakeBodyCalcs(reqnode);
		MakeGadgetPos(reqnode,&ww);
		D(bug("MMR: Gfx stuff calcs&positions\n"));

		winw = ww + reqnode->buttonextrawidth +
				 reqnode->cfg.outerspacingx * 2;

		winh = reqnode->bodyheight +
				 reqnode->cfg.outerspacingy * 2 +
				 reqnode->buttonextraheight;

		winw += reqnode->scr->WBorLeft +
				  reqnode->scr->WBorRight;

		winh += reqnode->scr->WBorTop +
				  reqnode->scr->Font->ta_YSize + 1 +
			 	  reqnode->scr->WBorBottom;

		if (reqnode->cfg.cooldragging)
		{
			reqnode->draggad = NewObject(0,BUTTONGCLASS,
													 GA_Left,0,
													 GA_Top,reqnode->scr->WBorTop +
															  reqnode->scr->Font->ta_YSize,
													 GA_RelWidth,0,
													 GA_RelHeight,0,
													 GA_SysGType,GTYP_WDRAGGING,
													 TAG_DONE);
		}

		scr = IntuitionBase->FirstScreen;

		if ((rc= MakeWindow(reqnode,winw,winh,idcmp)))
		{

			D(bug("Window opened %lx %lx\n",rc,rc->IDCMPFlags));

			if (MYIMAGEOK(&reqnode->reqimage))
			{

				//setup logox and x position
				if (reqnode->cfg.logoalign == ALIGN_CENTER)
				{
					x = reqnode->bodyx + reqnode->cfg.innerspacingx;
					logox = reqnode->cfg.logoalign2 == ALIGN_LEFT ? x : x + reqnode->bodywidth - reqnode->reqimage.framewidth - reqnode->cfg.bodylogospacing - reqnode->cfg.innerspacingx;
					if (reqnode->cfg.logoalign2 == ALIGN_LEFT) x += reqnode->reqimage.framewidth + reqnode->cfg.bodylogospacing;
				} else { // TOP or BOTTOM have the same x pos
					x = reqnode->cfg.innerspacingx + reqnode->bodyx;

					if (reqnode->cfg.logoalign2 == ALIGN_CENTER)
					{
						logox = reqnode->bodyx + ((reqnode->bodywidth - reqnode->reqimage.framewidth)/2);
					}

					if (reqnode->cfg.logoalign2 == ALIGN_RIGHT)
					{
						logox = x + reqnode->bodywidth - reqnode->reqimage.framewidth - (reqnode->cfg.innerspacingx * 2);
					}

					if (reqnode->cfg.logoalign2 == ALIGN_LEFT)
					{
						logox = x;
						x += reqnode->reqimage.framewidth + reqnode->cfg.bodylogospacing;
					}
				}

				//setup logoy and y position
				if (reqnode->cfg.logoalign2 ==	ALIGN_CENTER)
				{
					y = reqnode->bodyy + reqnode->cfg.innerspacingy;
					logoy = reqnode->cfg.logoalign == ALIGN_TOP ? y : y + reqnode->bodyheight - reqnode->reqimage.frameheight - (reqnode->cfg.innerspacingy*2);
					if (reqnode->cfg.logoalign == ALIGN_TOP) y += reqnode->reqimage.frameheight;
				} else { //LEFT and RIGHT have the same y positon...
					y = reqnode->bodyy + reqnode->cfg.innerspacingy;
					logoy = reqnode->cfg.logoalign == ALIGN_CENTER ? reqnode->bodyy + (reqnode->bodyheight - reqnode->reqimage.frameheight)/2 :
									reqnode->cfg.logoalign == ALIGN_TOP ? reqnode->bodyy + reqnode->cfg.innerspacingy :
											reqnode->bodyy + reqnode->bodyheight - reqnode->reqimage.frameheight - reqnode->cfg.innerspacingy;
				}

				SetupMyImageHelpBM(&reqnode->reqimage,
										 rc->RPort,
										 logox,
										 logoy);

				DrawMyImage(rc->RPort,&reqnode->reqimage,logox,logoy,0);

				if (reqnode->reqimage.frames < 2)
				{
					FreeMyImage(&reqnode->reqimage);
				}
			} else {//if MYIMAGEOK
				x = reqnode->bodyx + reqnode->cfg.innerspacingx;
				y = reqnode->bodyy + reqnode->cfg.innerspacingy;
			}

			if (reqnode->cfg.logoalign2 != ALIGN_CENTER)
			{
				y = reqnode->cfg.bodyyalign == ALIGN_TOP ? y :
							 reqnode->cfg.bodyyalign == ALIGN_BOTTOM ? y + reqnode->bodyheight - reqnode->textheight - reqnode->cfg.innerspacingy*2:
											  y + ((reqnode->bodyheight - reqnode->textheight)/2) - reqnode->cfg.innerspacingy;
			}

			if (reqnode->buttonwidth > reqnode->textwidth + reqnode->reqimage.framewidth + reqnode->cfg.bodylogospacing)
			{
				if (reqnode->cfg.bodyxalign == ALIGN_RIGHT)
				{
					x = reqnode->bodyx + reqnode->bodywidth - reqnode->textwidth - reqnode->cfg.innerspacingx;
				}

				if (reqnode->cfg.bodyxalign == ALIGN_CENTER)
				{
					x = reqnode->bodyx + ((reqnode->bodywidth - reqnode->textwidth - reqnode->reqimage.framewidth - reqnode->cfg.bodylogospacing)/2) - reqnode->cfg.innerspacingx + reqnode->reqimage.framewidth + reqnode->cfg.bodylogospacing;
				}
			}

			if (PENOK(RPEN_TEXTBOXSHADOW))
			{
				SetAPen(rc->RPort,PEN(RPEN_TEXTBOXSHADOW));
				MyPrintIText(reqnode,
								 rc->RPort,
								 reqnode->body,
								 x + 1,
								 y + 1,
								 reqnode->textwidth);
			}

			SetAPen(rc->RPort,PEN(RPEN_TEXTBOXFG));
			MyPrintIText(reqnode,
							 rc->RPort,
							 reqnode->body,
							 x,
							 y,
							 reqnode->textwidth);

			mygad = reqnode->buttongadgets;
			for(i = 0;i < reqnode->num_buttongadgets;i++)
			{
				AddGadget(rc,mygad->gad,-1);
				RefreshGList(mygad->gad,rc,0,1);
				mygad++;
			}

			if (reqnode->draggad) AddGadget(rc,reqnode->draggad,-1);

			//iconifygadget (VisualPrefs only!)
			if (!(FindResident("MorphOS")))
			reqnode->iconifyimage = NewObject(NULL,TBICLASS,SYSIA_Which,ICONIFYIMAGE,
							  SYSIA_DrawInfo,(ULONG)reqnode->dri,
							  TAG_END);

			if (reqnode->iconifyimage)
			{
				reqnode->iconifygadget = NewObject(NULL,BUCLASS,
								 GA_RelRight,1 - (2 * (reqnode->iconifyimage->Width - 1)),
								 GA_Top,0,
								 GA_Width,reqnode->iconifyimage->Width - 1,
               					                 GA_Height,reqnode->iconifyimage->Height,
                              					 GA_TopBorder,TRUE,
					                         GA_Image,(ULONG)reqnode->iconifyimage,
					                         GA_ID,ICONIFYID,GA_RelVerify,TRUE,
               					                 TAG_END);
				if (reqnode->iconifygadget)
				{
					AddGList(rc,reqnode->iconifygadget,0,1,NULL);
					RefreshGList(reqnode->iconifygadget,rc,NULL,1);
				}
			}

			//menu open gadget (VisualPrefs only!)
#ifndef NO_POPUPMENU
			if (PopupMenuBase)
			{
			if (!(FindResident("MorphOS")))
			reqnode->menuimage = NewObject(NULL,TBICLASS,SYSIA_Which,POPUPIMAGE,
							  SYSIA_DrawInfo,reqnode->dri,
							  TAG_END);

			if (reqnode->menuimage)
			{
				reqnode->menugadget = NewObject(NULL,BUCLASS,
																	 GA_RelRight,1 - (3 * (reqnode->menuimage->Width - 1)),
																	 GA_Top,0,
					                                     GA_Width,reqnode->menuimage->Width - 1,
               					                      GA_Height,reqnode->menuimage->Height,
                              					       GA_TopBorder,TRUE,
					                                     GA_Image,reqnode->menuimage,
					                                     GA_ID,MENUID,GA_RelVerify,TRUE,
               					                      TAG_END);
				if (reqnode->menugadget)
				{
					AddGList(rc,reqnode->menugadget,0,1,NULL);
					RefreshGList(reqnode->menugadget,rc,NULL,1);
				}
			}
			} // if PopupMenuBase
#endif
			/*string = (struct Gadget *)NewObject(NULL,"strgclass",
						GA_ID,1,GA_Immediate,TRUE,GA_RelVerify,TRUE,
						GA_Top,20,GA_Left,10,GA_Width,50,GA_Height,20,
						GA_RightBorder,TRUE,GA_LeftBorder,TRUE,
						GA_TopBorder,TRUE,GA_BottomBorder,TRUE,
						STRINGA_MaxChars,10,
						STRINGA_Justification,STRINGRIGHT,TAG_END);
			if (string)
			{
				AddGList(rc,string,0,1,NULL);RefreshGList(string,rc,NULL,1);
				Delay(350);
				RemoveGList(rc,string,1);
				DisposeObject(string);

			}*/

			if (doconfig) if (scr != rc->WScreen) reqnode->flags |= REQF_POPBACKSCREEN;

			if (reqnode->devattack)
			{
				HilightGadget(reqnode,reqnode->devattack->da_StartButton,TRUE);
			} else {
				HilightGadget(reqnode,0,TRUE);
			}

			if (MYIMAGEOK(&reqnode->reqimage) &&
			    (reqnode->reqimage.frames > 1))
			{
				if (SendAnimMsg(ANIMM_START,
									 rc->RPort,
									 &reqnode->reqimage,
									 logox,
									 logoy) == ANIMMR_OK)
				{
					reqnode->flags |= REQF_LOGOANIMATED;
				}
			}


#ifndef NO_POPUPMENU
			// here we'll create da popup menu
			if (PopupMenuBase)
			{
				reqnode->popupmenu = PMMenu("ReqAttack"),
												PMItem("Edit requester"),PM_UserData,MENU_RAPID,PMEnd,
												PMItem("Sleep"),PM_UserData,MENU_SLEEPID,PMEnd,
												PMItem("Disable sounds"),PM_UserData,MENU_DISABLEID,PMEnd,
												PMBar,PM_ID,10,PMEnd,
												PMEnd;

				//now insert the button items...
				if(reqnode->popupmenu)
				{
					mygad = reqnode->buttongadgets;
					for(i = 0;i < reqnode->num_buttongadgets;i++)
					{
						lx=i+1+POPUSER_ADD;
						if (i==0)lx=1+POPUSER_ADD;
						if (i==reqnode->num_buttongadgets-1)lx=POPUSER_ADD;
						PM_InsertMenuItem(reqnode->popupmenu,
											PM_Insert_AfterID,10+i,
											PM_Insert_Item,
											PM_MakeItem(PM_Title,mygad->text,PM_ID,10+i+1,PM_UserData,lx,TAG_DONE),
											TAG_DONE);
						mygad++;
					}
				}
			}
#endif

#ifndef NO_SOUND
			if (!(reqnode->dissound))
			{
				if (reqnode->cfg.sound)
				{
					if (reqnode->cfg.sound[0])
					{
						reqnode->soundhandle = SendSoundMsg(reqnode->cfg.soundport,
																	   SOUNDCMD_PLAY,
																	   reqnode->cfg.sound,
																	   0,reqnode);
					}
				}
			}
#endif
			ScreenToFront(rc->WScreen);

		} /* if (rc) */

	} /* if (!(reqnode->cfg.donotpatch)) */

done:

	DoneExecuteConfig();

	D(bug("all set up\n"));

	return rc;
}

void FreeUnusedData(struct ReqNode *node)
{
	struct MyGadget *mygad; WORD i;

	InstallLayerHook(node->win->WLayer,LAYERS_NOBACKFILL);

	if (node->buttongadgets)
	{
		mygad = node->buttongadgets;
		for (i = 0;i < node->num_buttongadgets;i++)
		{
			FreeMyGadget(node->win,mygad);
			FreeMyImage(&mygad->myim);
			mygad++;
		}
	}

	if (node->draggad)
	{
		if (node->win) RemoveGadget(node->win,node->draggad);
		DisposeObject(node->draggad);
		node->draggad=0;
	}

#ifndef NO_POPUPMENU
	if (PopupMenuBase)
	{
		if (node->popupmenu)
		{
			PM_FreePopupMenu(node->popupmenu);
			node->popupmenu = 0;
		}
	}
#endif

	if (node->flags & REQF_LOGOANIMATED)
	{
		SendAnimMsg(ANIMM_STOP,0,&node->reqimage,0,0);
	}

	if (node->menugadget)
	{
		RemoveGList(node->win,node->menugadget,1);
		DisposeObject(node->menugadget);
		node->menugadget = 0;
	}

	if (node->menuimage)
	{
		DisposeObject(node->menuimage);
		node->menuimage = 0;
	}

	if (node->iconifygadget)
	{
		RemoveGList(node->win,node->iconifygadget,1);
		DisposeObject(node->iconifygadget);
		node->iconifygadget = 0;
	}

	if (node->iconifyimage)
	{
		DisposeObject(node->iconifyimage);
		node->iconifyimage = 0;
	}

	FreeMyImage(&node->backfillimage);
	FreeMyImage(&node->textbackfillimage);
	FreeMyImage(&node->buttonbackfillimage);
	FreeMyImage(&node->reqimage);

	FreeMyRequesterPens(node);

#ifndef NO_SOUND
	if (node->soundhandle)
	{
		SendSoundMsg(node->cfg.soundport,
					    SOUNDCMD_STOP,
						 0,
						 node->soundhandle,node);
		node->soundhandle = 0;
	}
#endif

	if (node->fontok)
	{
		CloseFont(node->font);
		node->fontok=0;
	}

	if (node->buttonfontok)
	{
		CloseFont(node->buttonfont);
		node->buttonfontok=0;
	}

}

LONG HandleMyRequester(struct ReqNode *reqnode,ULONG *idcmpflags,BOOL wait)
{
	struct IntuiMessage *msg;
	struct AppMessage *amsg;
	BYTE method = FALSE, runlock = FALSE;
	UBYTE rawbuffer[RKBUFLEN];
	struct InputEvent ie;

	LONG rc = -2;
	//ULONG mrc;

	D(bug("handler called for: %s \n",reqnode->title));

	if (wait) WaitPort(reqnode->win->UserPort);

	if (reqnode->appicon)
	{
		D(bug("iconified\n"));

		amsg = (struct AppMessage *)GetMsg(reqnode->win->UserPort);
		while (amsg)
		{
			if (amsg->am_UserData == (ULONG) reqnode)
			{
				if (amsg)
				{
					RemoveAppIcon(reqnode->appicon);reqnode->appicon=0;
					FreeDiskObject(reqnode->appicondiskobject);reqnode->appicondiskobject=0;
					MakeMyRequester(reqnode,0,0);
				}
			} else {
				//react for idcmp flags!
				msg = (struct IntuiMessage *)amsg;
				if (msg->Class & reqnode->customidcmp)
				{
					rc = -1;
					if (idcmpflags) *idcmpflags = msg->Class;
				}
			}
			ReplyMsg((struct Message *)amsg);
			amsg = (struct AppMessage *)GetMsg(reqnode->win->UserPort);
		}
	} else {

	while ((msg = (struct IntuiMessage *)GetMsg(reqnode->win->UserPort)))
	{

		switch(msg->Class)
		{
			case IDCMP_GADGETUP:
				D(bug("idcmp_gadgetup\n"));
				switch(((struct Gadget *)msg->IAddress)->GadgetID)
				{
#ifndef NO_POPUPMENU
					case MENUID:
					if (reqnode->popupmenu)
					{
						mrc = PM_OpenPopupMenu(reqnode->win,PM_Top,reqnode->win->BorderTop,PM_Menu,reqnode->popupmenu,TAG_DONE);
						switch(mrc)
						{
							case MENU_RAPID://call RAPrefsMUI...
								if (!(reqnode->cfg.previewmode))
								{
									method = RAPMUI_METHOD;
								}
							break;

							case MENU_DISABLEID:
								method = DISABLE_METHOD;
							break;

							case MENU_SLEEPID:
								method = SLEEP_METHOD;
							break;

							default:
								if (mrc != 0) rc = mrc - POPUSER_ADD;
							break;
						}
					}
					break;

					case ETI_PopUp:
					if (reqnode->popupmenu)
					{
						mrc = PM_OpenPopupMenu(reqnode->win,PM_Top,reqnode->win->BorderTop,PM_Menu,reqnode->popupmenu,TAG_DONE);
						switch(mrc)
						{
							case MENU_RAPID://call RAPrefsMUI...
								if (!(reqnode->cfg.previewmode))
								{
									method = RAPMUI_METHOD;
								}
							break;

							case MENU_DISABLEID:
								method = DISABLE_METHOD;
							break;

							case MENU_SLEEPID:
								method = SLEEP_METHOD;
							break;

							default:
								if (mrc != 0) rc = mrc - POPUSER_ADD;
							break;
						}
					}
					break;
#endif
					case ICONIFYID:
						method = SLEEP_METHOD;
					break;

					case ETI_Iconify:
						method = SLEEP_METHOD;
					break;

					default:
						rc = ((struct Gadget *)msg->IAddress)->GadgetID;
					break;
				}
				break;

			case IDCMP_RAWKEY:
				D(bug("rawkey\n"));
				/* SHIFT + TAB does not send a IDCMP_VANILLAKEY message :( */

				if (reqnode->num_buttongadgets > 1)
				{
					if (((msg->Code == 66) && (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))) ||
						 (msg->Code == 79) || (msg->Code == NM_WHEEL_DOWN) || (msg->Code == NM_WHEEL_LEFT))
					{
						HilightPrev(reqnode);break;
					} else if ((msg->Code == 78) || (msg->Code == NM_WHEEL_UP) || (msg->Code == NM_WHEEL_RIGHT))
					{
						HilightNext(reqnode);break;
					}
				}

				if (reqnode->cfg.functionkeys &&
					 (!(msg->Qualifier & IEQUALIFIER_REPEAT)))
				{
					if ((msg->Code >= 80) &&
						 (msg->Code <= 89) &&
						 (msg->Code <= (79 + reqnode->num_buttongadgets)))
					{
						if (KeyGadget(reqnode->win,
										  reqnode->buttongadgets[msg->Code - 80].gad,
										  msg->Code,
										  TRUE))
						{
							rc = reqnode->buttongadgets[msg->Code - 80].gad->GadgetID;break;
						}
					}
				}

				//here's the RAWKEY to VANILLAKEY conversion...
				ie.ie_Class = IECLASS_RAWKEY;
				ie.ie_SubClass = 0;
				ie.ie_Code = msg->Code;
				ie.ie_Qualifier = NULL;//msg->Qualifier;
				ie.ie_EventAddress = (APTR *) *((ULONG *)msg->IAddress);
				if (MapRawKey(&ie,rawbuffer,RKBUFLEN,0))
				{

					if (!(runlock))
					{
						if  (((toupper(rawbuffer[0]) == toupper(reqnode->cfg.runkey))
									  							  && ((reqnode->cfg.keyqualifier == 0)
									  							  											|| (msg->Qualifier & reqnode->cfg.keyqualifier))))
						{
							method = KEYPREFS_METHOD;
							runlock = TRUE;
						}
						if  (((toupper(rawbuffer[0]) == toupper(reqnode->cfg.editkey))
									  							  && ((reqnode->cfg.keyqualifier == 0)
									  							  											|| (msg->Qualifier & reqnode->cfg.keyqualifier))))
						{
							method = RAPMUI_METHOD;
							runlock = TRUE;
						}
					} //if (!(runlock))

					if (!(reqnode->appicondiskobject)) if  (((toupper(rawbuffer[0]) == toupper(reqnode->cfg.iconifykey))
								  							  && ((reqnode->cfg.keyqualifier == 0)
								  							  											|| (msg->Qualifier & reqnode->cfg.keyqualifier))))
																	{
																		method = SLEEP_METHOD;
																	};

					if ((reqnode->num_buttongadgets > 1) && (rawbuffer[0] == 9))
					{
						HilightNext(reqnode);
					} else if ((rawbuffer[0] == 13) ||
								  ((toupper(rawbuffer[0]) == toupper(reqnode->cfg.okkey))
								  							  && ((reqnode->cfg.keyqualifier == 0)
								  							  											|| (msg->Qualifier & reqnode->cfg.keyqualifier))))
					{
						if (KeyGadget(reqnode->win,
										  reqnode->buttongadgets[reqnode->activegad].gad,
										  rawbuffer[0],
										  FALSE))
						{
							rc = reqnode->buttongadgets[reqnode->activegad].gad->GadgetID;
						}

					} else if ((rawbuffer[0] == 27) ||
								  ((toupper(rawbuffer[0]) == toupper(reqnode->cfg.cancelkey))
								  							  && ((reqnode->cfg.keyqualifier == 0)
								  							  											|| (msg->Qualifier & reqnode->cfg.keyqualifier))))
					{
						if (KeyGadget(reqnode->win,
										  reqnode->buttongadgets[reqnode->num_buttongadgets - 1].gad,
										  rawbuffer[0],
										  FALSE))
						{
							rc = 0;
						}
					}
				}

				break;

			/* J ADDS*/
			/*This reports window movements to the RAAHISound :)*/
			case IDCMP_CHANGEWINDOW:
				D(bug("chwindow\n"));
#ifndef NO_SOUND
				if (reqnode->soundhandle)
				{
					SendSoundMsg(reqnode->bcfg.soundport,
								    SOUNDCMD_MOVE,
									 0,
									 reqnode->soundhandle,reqnode);
				}
#endif
				break;

			/*This reports window activation to the RAAHISound :)*/
			case IDCMP_ACTIVEWINDOW:
				D(bug("actwindow\n"));
#ifndef NO_SOUND
				if (reqnode->soundhandle)
				{
					D(bug("sending message... "));
					SendSoundMsg(reqnode->bcfg.soundport,
								    SOUNDCMD_ACTIVE,
									 0,
									 reqnode->soundhandle,reqnode);
					D(bug("ending act part\n"));
				}
#endif
				break;

			/*This reports window disactivation to the RAAHISound :)*/
			case IDCMP_INACTIVEWINDOW:
				D(bug("inactwindow\n"));
#ifndef NO_SOUND
				if (reqnode->soundhandle)
				{
					SendSoundMsg(reqnode->bcfg.soundport,
								    SOUNDCMD_INACTIVE,
									 0,
									 reqnode->soundhandle,reqnode);
				}
#endif
				break;

			case IDCMP_MOUSEBUTTONS:
				D(bug("mousebuttons\n"));
			/*This opens the popup menu - NEW in 1.6*/
				if (msg->Qualifier & IEQUALIFIER_RBUTTON)
				{
#ifndef NO_POPUPMENU
					if (reqnode->popupmenu)
					{
						mrc = PM_OpenPopupMenu(reqnode->win,PM_Menu,reqnode->popupmenu,TAG_DONE);
						switch(mrc)
						{
							case MENU_RAPID://call RAPrefsMUI...
								if (!(reqnode->cfg.previewmode))
								{
									method = RAPMUI_METHOD;
								}
							break;

							case MENU_DISABLEID:
								method = DISABLE_METHOD;
							break;

							case MENU_SLEEPID:
								method = SLEEP_METHOD;
							break;

							default:
								if (mrc != 0) rc = mrc - POPUSER_ADD;
							break;
						}
					}
#endif					
				// This works like an immediate pressing of highlighted button!
/*				} else if (msg->Qualifier & IEQUALIFIER_MIDBUTTON)
				{
					if (msg->Code & ~IECODE_UP_PREFIX)
					if (KeyGadget(reqnode->win,
									  reqnode->buttongadgets[reqnode->activegad].gad,
									  rawbuffer[0],
									  FALSE))
					{
						rc = reqnode->buttongadgets[reqnode->activegad].gad->GadgetID;
					}*/
				}
				break;

		}; /* switch(msg->Class) */

	D(bug("ending switch...\n"));

		if (rc == -2)
		{
			if (msg->Class & reqnode->customidcmp)
			{
				rc = -1;
				if (idcmpflags) *idcmpflags = msg->Class;
			}
		}

		ReplyMsg(&msg->ExecMessage);

		if (rc < 0) if (method) MenuMethods(reqnode,method);

	} /* while ((msg = (struct IntuiMessage *)GetMsg(reqnode->win->UserPort))) */

	} /* if appicon*/

#ifndef NO_SOUND
if(!(reqnode->dissound))
{
	if (rc == 0) /*J adds!!!*/
	{
		if (reqnode->num_buttongadgets == 1)
		{
				if (reqnode->bcfg.oksound[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.oksound,
									   0,reqnode);
				} else {
					if (reqnode->bcfg.cancelsound[0])
					{
						reqnode->soundhandle = 0;
						SendSoundMsg(reqnode->bcfg.soundport,
										   SOUNDCMD_PLAY,
										   reqnode->bcfg.cancelsound,
										   0,reqnode);
					}
				}
		} else {
				if (reqnode->bcfg.cancelsound[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.cancelsound,
									   0,reqnode);
				}
		}
	}
	if (rc == 1)
	{
				if (reqnode->bcfg.oksound[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.oksound,
									   0,reqnode);
				}
	}
	if (rc == 2)
	{
				if (reqnode->bcfg.othersound1[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.othersound1,
									   0,reqnode);
				}
	}
	if (rc == 3)
	{
				if (reqnode->bcfg.othersound2[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.othersound2,
									   0,reqnode);
				}
	}
	if (rc > 3)
	{
				if (reqnode->bcfg.othersound3[0])
				{
					reqnode->soundhandle = 0;
					SendSoundMsg(reqnode->bcfg.soundport,
									   SOUNDCMD_PLAY,
									   reqnode->bcfg.othersound3,
									   0,reqnode);
				}
	}
}//dissounds
#endif

D(bug("exiting...\n"));
	return rc;
}


void FreeMyRequesterPens(struct ReqNode *reqnode)
{
	WORD i;

	for(i = 0; i < NUM_RPENS;i++)
	{
		if (reqnode->cfg.pens[i].flags & RPENF_ALLOCED)
		{
			ReleasePen(reqnode->cm,reqnode->cfg.pens[i].pen);
			reqnode->cfg.pens[i].flags &= ~RPENF_ALLOCED;
		}
	}
}

void MenuMethods(struct ReqNode *reqnode,BYTE method)
{
	struct MsgPort *p;
	struct RAPrefsMsg ramsg;
	struct MsgPort replyport;
	char *infoname;

	switch(method)
	{
		case KEYPREFS_METHOD:
					SendRandomLogo((char *)reqnode,RA_KEYPREFS,NULL);
			break;
		case RAPMUI_METHOD:
							Forbid();
							p = FindPort(RAPREFS);
							if (!(p))
							{
								Permit();
								SendRandomLogo((char *)reqnode,RA_RUNRAPREFS,(ULONG *)GetProgName2(reqnode));
							} else {
								Permit();
								//send the message to RA
								ObtainSemaphore(&rapsem);

								replyport.mp_Node.ln_Type = NT_MSGPORT;
								replyport.mp_Flags = PA_SIGNAL;
								replyport.mp_SigBit = SIGB_SINGLE;
								replyport.mp_SigTask = FindTask(0);
								NewList(&replyport.mp_MsgList);

								SetSignal(0,SIGF_SINGLE);

								ramsg.msg.mn_ReplyPort = &replyport;
								ramsg.msg.mn_Length = sizeof(ramsg);

								ramsg.version = RAPREFSMUI_VERSION;
								ramsg.reqnode = reqnode;
								ramsg.proc = GetProgName2(reqnode);

								Forbid();
								if ((p = FindPort(RAPREFS)))
								{
									PutMsg(p,&ramsg.msg);
									WaitPort(&replyport);
								}
								Permit();

								ReleaseSemaphore(&rapsem);

							}
							Permit();
		break;

		case SLEEP_METHOD:
							infoname = reqnode->bcfg.info;
							if (reqnode->devattack)
							{
								if (reqnode->devattack->da_AppIcon)
								{
									if (reqnode->devattack->da_AppIcon[0])
									infoname = reqnode->devattack->da_AppIcon;
								}
							}

							if (!(reqnode->appicondiskobject))
							{
								reqnode->appicondiskobject = (struct DiskObject *)SendRandomLogo(infoname,RA_APPICON,0);
							}

							if (reqnode->appicondiskobject)
							{
								// reset icon position
								reqnode->appicondiskobject->do_CurrentX = 0;
								reqnode->appicondiskobject->do_CurrentY = 0;

								if (!(reqnode->appicon)) reqnode->appicon = AddAppIconB(1,(ULONG)reqnode,
																		reqnode->title,
																		reqnode->win->UserPort,
																		NULL,reqnode->appicondiskobject,
																		NULL);
								if (reqnode->appicon)
								{
									//now free the resources :)
									FreeUnusedData(reqnode);
									ChangeWindowBox(reqnode->win,0,0,1,1);
									WindowToBack(reqnode->win);
								} else {
									FreeDiskObject(reqnode->appicondiskobject);
									reqnode->appicondiskobject = 0;
								}
							} //if diskobject
		break;

		case DISABLE_METHOD:
							reqnode->dissound=TRUE;
#ifndef NO_SOUND
							if (reqnode->soundhandle)
							{
								SendSoundMsg(reqnode->bcfg.soundport,
											    SOUNDCMD_STOP,
												 0,
												 reqnode->soundhandle,reqnode);
								reqnode->soundhandle=0;
							}
#endif
		break;
	}
}

