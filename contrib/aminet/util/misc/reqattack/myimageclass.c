#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <utility/tagitem.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/utility_lib.h>
#else
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/dos.h>
#endif

#include <clib/alib_protos.h>

#include <string.h>
#include <ctype.h>

#include "global.h"
#include "config.h"
#include "myimage.h"
#include "myimageclass.h"
#include "reqlist.h"
#include "config.h"
#include "misc.h"

#include "myimage_protos.h"

struct IClass *MyImageClass;

extern ULONG HookEntry();


#if 0
static char ShortCut(char *text)
{
	char *sp;
	char rc=1;

	if ((sp = strchr(text,'_')))
	{
		rc = toupper(sp[1]);
	}
	return rc;
}
#endif

static LONG MyTextLength(struct RastPort *rp,char *text,LONG len)
{
	LONG rc;

	rc = TextLength(rp,text,len);
	if (strchr(text,'_')) rc -= TextLength(rp,"_",1);

	return rc;
}

static BOOL IsGadgetHi(struct MyImageData *data)
{
	BOOL rc = FALSE;

	if (data->reqnode->activegad != -1)
	{
		if (data->reqnode->buttongadgets[data->reqnode->activegad].gad == data->gad)
		{
			rc = TRUE;
		}
	}

	return rc;
}

static void DrawButtonBG(struct RastPort *rp,struct MyImageData *data,WORD x1,WORD y1,WORD x2,WORD y2,WORD state)
{
	UBYTE pen1,pen2,pen3,pen4,bgpen;

	SetDrMd(rp,JAM1);

	/* First outer edges ... */

	if (state == IDS_SELECTED)
	{
		pen1 = RPEN_OBUTTONEDGE1SEL;
		pen2 = RPEN_OBUTTONEDGE2SEL;
		pen3 = RPEN_IBUTTONEDGE1SEL;
		pen4 = RPEN_IBUTTONEDGE2SEL;

		bgpen = RPEN_BUTTONBGSEL;

	} else if (IsGadgetHi(data)) {
		pen1 = RPEN_OBUTTONEDGE1HI;
		pen2 = RPEN_OBUTTONEDGE2HI;
		pen3 = RPEN_IBUTTONEDGE1HI;
		pen4 = RPEN_IBUTTONEDGE2HI;

		bgpen = RPEN_BUTTONBGHI;

	} else {
		pen1 = RPEN_OBUTTONEDGE1;
		pen2 = RPEN_OBUTTONEDGE2;
		pen3 = RPEN_IBUTTONEDGE1;
		pen4 = RPEN_IBUTTONEDGE2;

		bgpen = RPEN_BUTTONBG;
	}

	DrawFrame(data->reqnode,rp,x1,y1,x2,y2,pen1,pen2,pen3,pen4,bgpen,TRUE);
}

static ULONG MyImage_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MyImageData *data;
	struct TagItem *ti,*tstate;
	ULONG rc = 0;

	if ((obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
	{
		data = INST_DATA(cl,obj);
		tstate = msg->ops_AttrList;

		memset(data,0,sizeof(struct MyImageData));

		while((ti = NextTagItem(&tstate)))
		{
			switch (ti->ti_Tag)
			{
				case SYSIA_DrawInfo:
					data->di = (struct DrawInfo *)ti->ti_Data;
					break;

				case IA_Data:
					data->im = (struct MyImage *)ti->ti_Data;
					break;

				case GA_Text:
					data->text = (char *)ti->ti_Data;
					if (data->text)
					{
						data->textlen = strlen(data->text);
					}
					break;

				case MYIA_ReqNode:
					data->reqnode = (struct ReqNode *)ti->ti_Data;
					break;
			}

		} /* while((ti = NextTagItem(&tstate))) */

		rc = (ULONG)obj;

	} /* if ((obj=DoSuperMethodA(cl,obj,(Msg)msg))) */

	return rc;
}

static ULONG MyImage_Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MyImageData *data;
	struct TagItem *ti;
	data = INST_DATA(cl,obj);

	if ((ti = FindTagItem(MYIA_Gadget,msg->ops_AttrList)))
	{
		ti->ti_Tag = TAG_IGNORE;
		data->gad = (struct Gadget *)ti->ti_Data;
	}

	return DoSuperMethodA(cl,obj,(Msg)msg);
}

static ULONG MyImage_Draw(struct IClass *cl,struct Image *im,struct impDraw *msg)
{
	struct MyImageData *data;
	struct ReqNode *reqnode;
	struct RastPort *rp;
	char *sp;
	WORD x,y,x1,y1,x2,y2,frameno=0;
	BOOL IsSelected;

	data = INST_DATA(cl,(Object *)im);
	reqnode = data->reqnode;

	rp = msg->imp_RPort;
	x1 = msg->imp_Offset.X;
	y1 = msg->imp_Offset.Y;
	x2 = x1 + im->Width - 1;
	y2 = y1 + im->Height - 1;
	IsSelected = (msg->imp_State == IDS_SELECTED) ? TRUE : FALSE;

	DrawButtonBG(rp,data,x1,y1,x2,y2,(WORD)msg->imp_State);

	if (data->im)
	{
		y = (y1 + y2 + 1 - data->im->frameheight) / 2;
		if (IsSelected)
		{
			if (data->im->frames > 1) frameno = 1; //draw the selected button image
		}
			DrawMyImage(rp,
							data->im,
							x1 + reqnode->cfg.buttonispacingx + (IsSelected & reqnode->cfg.shiftbuttontext),
							y + (IsSelected & reqnode->cfg.shiftbuttontext),
							frameno);
	}

	if (data->text)
	{
		x = IsSelected ? PEN(RPEN_BUTTONFGSEL) :
							  IsGadgetHi(data) ? PEN(RPEN_BUTTONFGHI) :
											  		   PEN(RPEN_BUTTONFG);

		SetAPen(rp,x);

		x = IsSelected ? reqnode->cfg.buttontextselstyle :
							  IsGadgetHi(data) ? reqnode->cfg.buttontexthistyle :
											 		   reqnode->cfg.buttontextstyle;

		SetFont(rp,reqnode->buttonfont);
		SetSoftStyle(rp,x,AskSoftStyle(rp));

		x = (x1 +
		     reqnode->cfg.buttonispacingx +
		     (data->im ? (data->im->framewidth + reqnode->cfg.bimagetextspacing) : 0 ) +
		     (x2 - reqnode->cfg.buttonispacingx) -
		     MyTextLength(rp,data->text,data->textlen)
		    ) / 2;

/*		y = (y1 + y2) / 2  - (data->di->dri_Font->tf_Baseline + 1) / 2 + data->di->dri_Font->tf_Baseline;*/
/*newer is down!*/
/*		y = (y1 + y2 - data->di->dri_Font->tf_YSize + 1) / 2 + data->di->dri_Font->tf_Baseline; */
		y = (y1 + y2 - reqnode->buttonfont->tf_YSize + 1) / 2 + reqnode->buttonfont->tf_Baseline;

		Move(rp,x + (IsSelected & reqnode->cfg.shiftbuttontext),
				  y + (IsSelected & reqnode->cfg.shiftbuttontext));

		if ((sp = strchr(data->text,'_')))
		{
			if ((x1 = ((ULONG)sp - (ULONG)data->text)))
			{
				Text(rp,data->text,x1);
				x += TextLength(rp,data->text,x1);
				Move(rp,x + IsSelected,y + IsSelected);
			}
			if ((x2 = (data->textlen - x1 - 1)) > 0)
			{
				Text(rp,data->text + x1 + 1,x2);
			}

				RectFill(rp,x + IsSelected,y + IsSelected + 2,x + IsSelected + TextLength(rp,data->text + x1 + 1,1) - 1,y + IsSelected + 2);

		} else {
			Text(rp,data->text,data->textlen);
		}

	} /* if (data->text) */

	return 0;
}

static ULONG MyImage_GetSize(struct IClass *cl,struct Image *im,struct myimpGetSize *msg)
{
	struct MyImageData *data;
	struct config *cfg;
	WORD w,h,hh;

	data = INST_DATA(cl,im);
	cfg = &data->reqnode->cfg;

	h = cfg->buttonispacingy * 2 + data->reqnode->buttonfont->tf_YSize;
		 /*data->reqnode->font->tf_YSize;*/

	if (data->im)
	{
		hh = cfg->buttonispacingy2 * 2 +
			  data->im->frameheight;

		if (hh > h) h = hh;
	}

	ObtainSemaphore(&temprpsem);
	SetFont(&temprp,data->reqnode->buttonfont);	/*node->font*/
	w = TextLength(&temprp,data->text,data->textlen);
	ReleaseSemaphore(&temprpsem);

	if (data->im)
	{
		w += data->im->framewidth +
			  cfg->bimagetextspacing;
	}

	w += cfg->buttonispacingx * 2 +
	     cfg->buttonextrawidth;

	*msg->width = w;
	*msg->height = h;

	return 0;
}

static ULONG MyImageDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	ULONG rc;

	switch(msg->MethodID)
	{
		case OM_NEW:
			rc = MyImage_New(cl,obj,(struct opSet *)msg);

			break;

		case OM_SET:
			rc = MyImage_Set(cl,obj,(struct opSet *)msg);
			break;

		case IM_DRAW:
			rc = MyImage_Draw(cl,(struct Image *)obj,(struct impDraw *)msg);
			break;

		case MYIM_GETSIZE:
			rc = MyImage_GetSize(cl,(struct Image *)obj,(struct myimpGetSize *)msg);
			break;

		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;
	}

	return rc;
}

BOOL InitMyImageClass(void)
{
	BOOL rc=FALSE;

	if ((MyImageClass = MakeClass(0,IMAGECLASS,0,sizeof(struct MyImageData),0)))
	{
		MyImageClass->cl_Dispatcher.h_Entry=(HOOKFUNC)HookEntry;
		MyImageClass->cl_Dispatcher.h_SubEntry=(HOOKFUNC)MyImageDispatcher;
	}
	return rc;
}

void CleanupMyImageClass(void)
{
	if (MyImageClass) FreeClass(MyImageClass);
}


