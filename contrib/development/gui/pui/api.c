#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pui.h"
#include "groupclass.h"

APTR PUI_GetVisualInfoA(struct Screen *scr,struct TagItem *tags)
{
	struct PUI_RenderInfo *rc;
	
	rc = 0;
	
	if (scr)
	{
		if ((rc = AllocMem(sizeof(struct PUI_RenderInfo),MEMF_PUBLIC)))
		{
			rc->scr = scr;
			rc->cm = scr->ViewPort.ColorMap;
			rc->bm = scr->RastPort.BitMap;
			rc->li = &scr->LayerInfo;
			
			if (!(rc->dri = GetScreenDrawInfo(scr)))
			{
				FreeMem(rc,sizeof(struct PUI_RenderInfo));
				rc = 0;
			} else {
				rc->pens = rc->dri->dri_Pens;
				rc->font = rc->dri->dri_Font;
				rc->winborderl = scr->WBorLeft;
				rc->winborderr = scr->WBorRight;
				rc->winbordero = scr->WBorTop;
				rc->winborderu = scr->WBorBottom;
				rc->fontheight = rc->font->tf_YSize;
				rc->fontbaseline = rc->font->tf_Baseline;
				rc->titleheight = rc->fontheight + rc->winbordero + 1;
				rc->maxclientwidth = scr->Width - rc->winborderl - rc->winborderr;
				rc->maxclientheight = scr->Height - rc->titleheight - rc->winborderu;
				rc->scrwidth = scr->Width;
				rc->scrheight = scr->Height;

				InitRastPort(&rc->temprp);
				InitSemaphore(&rc->sem);
				
				SetFont(&rc->temprp,rc->font);
				
			}
		}
	}
	
	return rc;
}

void PUI_FreeVisualInfo(APTR vi)
{
	struct PUI_RenderInfo *ri;
	
	ri = (struct PUI_RenderInfo *)vi;
	
	if (ri)
	{
		if (ri->dri) FreeScreenDrawInfo(ri->scr,ri->dri);
		FreeMem(ri,sizeof(struct PUI_RenderInfo));
	}
}

Object *PUI_NewRootObjectA(APTR vi,struct TagItem *tags)
{
//	struct MinMax minmax;
	struct PUI_AreaData *adata;

	Object *rc;
	
	rc = 0;
	
	if (vi)
	{
		if ((rc = NewObject(groupclass,0,PUIA_Root_VisualInfo,vi,
													PUIA_Group_IsRoot,TRUE,
													GA_Immediate,TRUE,
													GA_RelVerify,TRUE,
													GA_RelSpecial,TRUE,
													TAG_MORE,tags)))
		{
			((struct ExtGadget *)rc)->MoreFlags |= GMORE_SCROLLRASTER;

			DoMethod(rc,PUIM_SETUP,
							0,
							vi,
							rc);
	
			adata = INST_DATA(areaclass,rc);

//			memset(&minmax,0,sizeof(struct MinMax));

			DoMethod(rc,PUIM_MINMAX,
							0,
							&adata->minmax);
	
		}

	} /* if (vi) */
		
	return rc;
}

Object *PUI_NewRootObject(APTR vi,Tag tag, ...)
{
	return PUI_NewRootObjectA(vi,(struct TagItem *)&tag);
}

Object *PUI_NewObjectA(LONG cl,struct TagItem *tags)
{
	return NewObjectA(classes[cl],0,tags);
}

Object *PUI_NewObject(LONG cl,Tag tag, ...)
{
	return PUI_NewObjectA(cl,(struct TagItem *)&tag);
}

Object *PUI_MakeObjectA(LONG type,LONG *params)
{
	Object *rc;
	
	rc = 0;
	switch(type)
	{
		case PUIO_TextButton:
			rc = ButtonObject,
						ButtonFrame,
						RelVerify,
						GA_ID,params[0],
						PUIA_Button_Text,params[1],
						End;
			break;
			
		case PUIO_ImageButton:
			rc = ButtonObject,
						ImageButtonFrame,
						RelVerify,
						GA_ID,params[0],
						PUIA_Button_Image,params[1],
						End;
			break;
			
		case PUIO_TextImageButton:
			rc = ButtonObject,
						TextImageButtonFrame,
						RelVerify,
						GA_ID,params[0],
						PUIA_Button_Text,params[1],
						PUIA_Button_Image,params[2],
						End;
			break;

		case PUIO_CheckMark:
			rc = ButtonObject,
						ButtonFrame,
						Immediate,
						GA_ID,params[0],
						GA_Selected,params[1],
						PUIA_ShowSelState,FALSE,
						PUIA_Button_Toggle,TRUE,
						PUIA_Button_Image,SysImageObject,
							PUIA_SysImage_Which,PUIV_SysImage_CheckMark,
							End,
						End;
						
			break;

		case PUIO_CoolImage:
			rc = CoolImageObject,
						PUIA_CoolImage_Which,params[0],
						End;
			break;


		case PUIO_Label:
			rc = LabelObject,
						PUIA_Label_Text,params[0],
						PUIA_Label_TextAlign,params[1],
						PUIA_Frame,params[2],
						PUIA_FramePhantomHoriz,TRUE,
						End;
			break;
						
	} /* switch(type) */
	
	return rc;
}

Object *PUI_MakeObject(LONG type,...)
{
	return PUI_MakeObjectA(type,&type + 1);
}

LONG PUI_MayRender(Object *obj)
{
	struct PUI_AreaData *adata;
	LONG rc;

	adata = INST_DATA(areaclass,obj);
	
	if ((adata->renderlock != 0) || (adata->flags & PUIF_Area_Hidden))
	{
		rc = FALSE;
	} else {
		rc = TRUE;
	}
	
	return rc;
}

static LONG GetWindowSize(Object *obj,BOOL width,BOOL inner)
{
	struct PUI_MinMax *minmax;
	struct PUI_RenderInfo *ri;
	LONG rc;

	rc = -1;
	
	if (obj)
	{
		GetAttr(PUIA_Root_Dimensions,obj,(ULONG *)&minmax);
		GetAttr(PUIA_Area_RenderInfo,obj,(ULONG *)&ri);
		
		if (width)
		{
			rc = minmax->minwidth +
			     window_defaultspacingl +
			     window_defaultspacingr;

			if (!inner) rc += ri->winborderl + ri->winborderr;

		} else {
			rc = minmax->minheight +
				  window_defaultspacingo +
				  window_defaultspacingu;
				  
			if (!inner) rc += ri->titleheight + ri->winborderu;
				  
		}		
	}
	
	return rc;
}

LONG PUI_WindowWidth(Object *obj)
{
	return GetWindowSize(obj,TRUE,FALSE);
}

LONG PUI_WindowHeight(Object *obj)
{
	return GetWindowSize(obj,FALSE,FALSE);
}

LONG PUI_InnerWidth(Object *obj)
{
	return GetWindowSize(obj,TRUE,TRUE);
}

LONG PUI_InnerHeight(Object *obj)
{
	return GetWindowSize(obj,FALSE,TRUE);
}

void PUI_SetWindowLimits(struct Window *win,Object *obj)
{
	struct PUI_AreaData *adata;
	WORD 				 	  x,y;

	adata = INST_DATA(areaclass,obj);
	
	x = win->BorderLeft + win->BorderRight +
		 window_defaultspacingl + window_defaultspacingr;

	y = win->BorderTop + win->BorderBottom +
		 window_defaultspacingo + window_defaultspacingu;

	WindowLimits(win,x + adata->minmax.minwidth,
						  y + adata->minmax.minheight,
						  x + (adata->minmax.maxwidth  > 16384 ? 16384 : adata->minmax.maxwidth),
						  y + (adata->minmax.maxheight > 16384 ? 16384 : adata->minmax.maxheight));
}

struct IntuiMessage *PUI_GetIMsg(struct MsgPort *msgport)
{
	return (struct IntuiMessage *)GetMsg(msgport);
}

void PUI_ReplyIMsg(struct IntuiMessage *intuimsg)
{
	ReplyMsg((struct Message *)intuimsg);
}

