
/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <proto/alib.h>
#include <proto/utility.h>

#include <proto/datatypes.h>
#include <clib/dtclass_protos.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#include <emul/emulregs.h>
#include <emul/emulinterface.h>
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "pageview_class.h"
#include "documentview_class.h"
#include "documentlayout_class.h"
#include "lay_class.h"

#include "system/chunky.h"
#include "system/functions.h"
#include "system/gentexture.h"

struct Data
{
	void *doc;
	void *output;
	Object **views;
	int prevwidth;
	struct MUI_EventHandlerNode eh;
	int clicky;
	int clickx;
	int page;
	int columns;
	int scaling;
	float zoom;
	int ispreview;
	int quiet;
	int rotation;
};

#define gFalse 0

DEFNEW
{
	obj = DoSuperNew(cl, obj,
				MUIA_Group_Horiz, FALSE,
				MUIA_Group_Columns, GetTagData(MUIA_DocumentLayout_Columns, 2, INITTAGS),
				TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		int information;

		data->doc = (void*)GetTagData(MUIA_DocumentLayout_PDFDocument, NULL, INITTAGS);
		data->output = (void*)GetTagData(MUIA_DocumentLayout_PDFOutputDevice, NULL, INITTAGS);
		data->columns = GetTagData(MUIA_DocumentLayout_Columns, 2, INITTAGS);
		data->scaling = GetTagData(MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_Zoom, INITTAGS);
		data->rotation = GetTagData(MUIA_DocumentLayout_Rotation, MUIV_DocumentLayout_Rotation_None, INITTAGS);
		data->page = (int)GetTagData(MUIA_DocumentLayout_Page, 1, INITTAGS);
		information = (int)GetTagData(MUIA_PageView_Information, MUIV_PageView_Information_None, INITTAGS);
		data->ispreview = (int)GetTagData(MUIA_PageView_IsPreview, FALSE, INITTAGS);
		data->zoom = 1.0f;

		if (data->doc != NULL)
		{
			int i;

			/* for each page add pageview object */

			data->views = malloc(sizeof(Object*) * pdfGetPagesNum(data->doc));
			#warning TODO: handle failure

			for(i=0; i<pdfGetPagesNum(data->doc); i++)
			{
				float mediawidth = pdfGetPageMediaWidth(data->doc, i + 1);
				float mediaheight = pdfGetPageMediaHeight(data->doc, i + 1);

				APTR pageview = PageViewObject,
								MUIA_PageView_PDFDocument, data->doc,
								MUIA_PageView_PDFOutputDevice, data->output,
								MUIA_PageView_Page, i + 1,
								MUIA_PageView_MediaWidth, (int)mediawidth,
								MUIA_PageView_MediaHeight, (int)mediaheight,
								MUIA_PageView_Information, information,
								MUIA_PageView_IsPreview, data->ispreview,
								//MUIA_FixWidth , 128,
								//MUIA_FixHeight , 128,
								End;

				//DoMethod(obj, OM_ADDMEMBER, HSpace(0));
				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, MUIA_FixWidth, 128, MUIA_FixHeight, 128, End);
				DoMethod(obj, OM_ADDMEMBER, pageview);
				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, MUIA_FixWidth, 50, End);
				//DoMethod(obj, OM_ADDMEMBER, HSpace(0));
				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, End);

				data->views[i] = pageview;
			}

			/* only notify about new 'focused' page if this is not previews view (outlines) */
			if (data->ispreview == FALSE)
				DoMethod(obj, MUIM_Notify, MUIA_Virtgroup_Top, MUIV_EveryTime, obj, 1, MUIM_ContinuousLayout_SetupPage);

		}
		else
		{
			/* TODO: handle error */
		}
	}

	return (ULONG)obj;
}

DEFDISP
{
	GETDATA;
	return DOSUPER;
}

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_DocumentLayout_Zoom:
			*(ULONG*)msg->opg_Storage = (int)(data->zoom * 65536.0f);
			return TRUE;
			
		case MUIA_DocumentLayout_Scaling:
			*(ULONG*)msg->opg_Storage = data->scaling;
			return TRUE;

		case MUIA_DocumentLayout_PDFDocument:
			*(ULONG*)msg->opg_Storage = data->doc;
			return TRUE;

		case MUIA_DocumentLayout_Page:
			*(ULONG*)msg->opg_Storage = (int)(data->page);
			return TRUE;

		case MUIA_PageView_NeedRefresh:
			return TRUE;	/* this is only for notification */

		case MUIA_PageView_RedirectPage:
			return TRUE;    /* this is only for notification */
	}

	return(DOSUPER);
}

DEFSET
{
	GETDATA;
	int relayout = FALSE;

	FORTAG(INITTAGS)
	{
		case MUIA_DocumentLayout_Page:
		{
			/* if the page is not in focus then make it so */

			int prevpage = data->page - 1;
			int newpage = tag->ti_Data - 1;

			Object *pageview = data->views[tag->ti_Data - 1];
			data->page = tag->ti_Data;
			if (data->quiet == FALSE)
			{
				LONG off = DoMethod(obj, MUIM_ContinuousLayout_ClippedPageOffset, data->page);

				if (off != 0)
				{
					//set(obj, MUIA_Virtgroup_Top, (LONG)xget(obj, MUIA_Virtgroup_Top) + (LONG)xget(pageview, MUIA_TopEdge) - _top(obj));
					set(obj, MUIA_Virtgroup_Top, (LONG)xget(obj, MUIA_Virtgroup_Top) - off);
				}
				//set(obj, MUIA_Virtgroup_Top, (LONG)xget(pageview, MUIA_PageView_LayoutTop));
			}

			MUI_Redraw(data->views[prevpage], MADF_DRAWOBJECT);
			MUI_Redraw(data->views[newpage], MADF_DRAWOBJECT);

			break;
		}

		case MUIA_DocumentLayout_Zoom:
			data->zoom = tag->ti_Data / 65536.0f;
			relayout = TRUE;
			break;
			
		case MUIA_DocumentLayout_Rotation:
			data->rotation = tag->ti_Data;
			relayout = TRUE;
			break;

		case MUIA_DocumentLayout_Scaling:
			data->scaling = tag->ti_Data;
			relayout = TRUE;
			break;

	}
	NEXTTAG

	if (relayout)
		DoMethod(obj, MUIM_ContinuousLayout_Relayout);

	return DOSUPER;
}

///	setup

METHOD continuouslayoutSetup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
	GETDATA;
	ULONG rc = DOSUPER;

	if (rc == 0)
		return rc;

	data->eh.ehn_Object = obj;
	data->eh.ehn_Class  = cl;
	data->eh.ehn_Events = IDCMP_MOUSEBUTTONS;
	data->eh.ehn_Priority = 100;
	data->eh.ehn_Flags = 0;//MUI_EHF_GUIMODE;
	DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->eh);

	return rc;
}

////

///	cleanup

METHOD continuouslayoutCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	GETDATA;

	if (data->eh.ehn_Object != NULL)
		DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->eh);

	return DOSUPER;
}

////

///	handleevent

METHOD continuouslayoutHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
	#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
	#define _isinobject(obj,x,y) (_between(vl,(x),vl+vw) && _between(vt,(y),vt+vh))
	#define _isinimage(obj,x,y) (_between(vl + data->offsetx, (x), vl + vw - data->offsetx) && _between(vt + data->offsety,(y), vt + vh - data->offsety))

	GETDATA;
	int vl = _left(obj);
	int vt = _top(obj);
	int vw = _width(obj);
	int vh = _height(obj);

	if (_isinobject(obj, msg->imsg->MouseX, msg->imsg->MouseY) || data->eh.ehn_Events & IDCMP_MOUSEMOVE)
	{
		if (msg->imsg != NULL)
		{
			switch (msg->imsg->Class)
			{
				case IDCMP_MOUSEBUTTONS:
					if (_isinobject(obj, msg->imsg->MouseX, msg->imsg->MouseY) && msg->imsg->Code == SELECTDOWN)
					{
						data->clickx = msg->imsg->MouseX;
						data->clicky = msg->imsg->MouseY;
						data->eh.ehn_Events |= IDCMP_MOUSEMOVE;
					}
					else
					{
						data->eh.ehn_Events &= ~IDCMP_MOUSEMOVE;
					}
					break;

				case IDCMP_MOUSEMOVE:
					{
						int newx = xget(obj, MUIA_Virtgroup_Left) - (msg->imsg->MouseX - data->clickx);
						int newy = xget(obj, MUIA_Virtgroup_Top) - (msg->imsg->MouseY - data->clicky);
						data->clickx = msg->imsg->MouseX;
						data->clicky = msg->imsg->MouseY;

						newx = clamp(newx, 0, xget(obj, MUIA_Virtgroup_Width) - _mwidth(obj));
						newy = clamp(newy, 0, xget(obj, MUIA_Virtgroup_Height) - _mheight(obj));

						SetAttrs(obj, MUIA_Virtgroup_Left, newx, MUIA_Virtgroup_Top, newy, TAG_DONE);
					}
					break;
			}
		}
	}

	return DOSUPER;
}

////

#if 0
METHOD documentviewEnqueueRender(struct IClass *cl, Object *obj, struct MUIP_DocumentView_EnqueueRender *msg)
{
	GETDATA;

	/* add page to the list of pages that have to be rendered */
	printf("enqueuerender\n");
	data->pendingrefresh = msg->page;
	DoMethod(data->proc, MUIM_Process_Signal, SIGBREAKF_CTRL_D);

	return 0;
}
#endif

METHOD continuouslayoutFindViewForPage(struct IClass *cl, Object *obj, struct MUIP_DocumentLayout_FindViewForPage *msg)
{
	GETDATA;
	return (ULONG)data->views[msg->page - 1];
}

METHOD continuouslayoutIsPageVisible(struct IClass *cl, Object *obj, struct MUIP_DocumentLayout_IsPageVisible *msg)
{
	GETDATA;
	Object *pageview = data->views[msg->page - 1];

	LONG top = xget(pageview, MUIA_TopEdge) - (LONG)_addtop(obj);
	LONG height = xget(pageview, MUIA_Height);

	/* check constraints for object and group */

	LONG nTop = top - _top(obj);
	LONG nBottom = nTop + height;
	LONG gBottom = _height(obj) + 5;    /* 5 is a margin */
	LONG gTop = -5;

	//printf("page %d borders:%d %d, borders:%d,%d\n", msg->page, nTop, nBottom, gTop, gBottom);

	if (nBottom < gTop)
	{
		/* bottom is invisible */
		return FALSE;
	}

	if (nTop > gBottom)
	{
		/* top is invisible */
		return FALSE;
	}

	return TRUE;
}

METHOD continuouslayoutClippedPageOffset(struct IClass *cl, Object *obj, struct MUIP_ContinuousLayout_ClippedPageOffset *msg)
{
	GETDATA;
	Object *pageview = data->views[msg->page - 1];

	LONG top = xget(pageview, MUIA_TopEdge) - (LONG)_addtop(obj);
	LONG height = xget(pageview, MUIA_Height);

	/* check constraints for object and group */

	LONG nTop = top - _top(obj);
	LONG nBottom = nTop + height;
	LONG gBottom = _height(obj) + 5;    /* 5 is a margin */
	LONG gTop = -5;
	LONG oldnTop = nTop;

	//printf("page %d borders:%d %d, borders:%d,%d\n", msg->page, nTop, nBottom, gTop, gBottom);

	if (nBottom < gTop)	/* completely outside top edge. scroll so the top edge is visible */
		nTop = gTop;

	if (nBottom > gBottom) /* completely or completely outside bottom edge. scroll so the page sits on the bottom frame but don't return yet */
		nTop = gBottom - height;

	if (nTop < gTop && nTop + height > gTop) /* partialy clipped outside the top edge. scroll to the bottom */
		nTop = gTop;

	return nTop - oldnTop;
}

METHOD continuouslayoutShow(struct IClass *cl, Object *obj, struct MUIP_Show *msg)
{
	GETDATA;

	/* order relayout if width changed */

	if (_mwidth(obj) != data->prevwidth)
	{
		data->prevwidth = _mwidth(obj);
		DoMethod(_app(obj), MUIM_Application_PushMethod, obj, 1, MUIM_ContinuousLayout_Relayout);
	}

	return DOSUPER;
}

void scaleCoordsWithAspect( int width , int height , int destWidth , int destHeight , int	 *newWidth , int   *newHeight );

static void applyrotation(int *width, int *height, int rotation)
{
	if (rotation == 1 || rotation == 3)
	{
		int t = *height;
		*height = *width;
		*width = t;
	}
}

METHOD continuouslayoutRelayout(struct IClass *cl, Object *obj, struct MUIP_ContinuousLayout_Relayout *msg)
{
	GETDATA;
	int i;
	int width = _mwidth(obj) / data->columns - 10 * (data->columns - 1);
	int height = _mheight(obj);
	int fpwidth, fpheight;
	int scaling = data->scaling & 0xffff;

	float docwidth, docheight;
	pdfGetDocumentDimensions(data->doc, &docwidth, &docheight);

	/* calculate scaled coords for FitPage mode */

	applyrotation(&docwidth, &docheight, data->rotation);
	scaleCoordsWithAspect(docwidth, docheight, width, height, &fpwidth, &fpheight);
	
	
	/* adjust dimensions of all objects */

	DoMethod(obj, MUIM_Group_InitChange);

	for(i=0; i<pdfGetPagesNum(data->doc); i++)
	{
		int mediawidth = xget(data->views[i], MUIA_PageView_MediaWidth);
		int mediaheight = xget(data->views[i], MUIA_PageView_MediaHeight);

		applyrotation(&mediawidth, &mediaheight, data->rotation);
		
		if (scaling == MUIV_DocumentLayout_Scaling_FitWidth)
		{
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, width,
				MUIA_PageView_LayoutHeight, (int)(width * (float)fpheight / fpwidth), /* use whole document aspect to preserve consistent font size on all pages */
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);
		}
		else if (scaling == MUIV_DocumentLayout_Scaling_FixWidth(0))
		{
			width = data->scaling >> 16;
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, width,
				MUIA_PageView_LayoutHeight, (int)(width * (float)mediaheight / mediawidth),
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);
		}
		else if (scaling == MUIV_DocumentLayout_Scaling_FitPage)	/* size is adjusted to max page size in whole doc to give same text size on all pages */
		{
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, fpwidth,
				MUIA_PageView_LayoutHeight, fpheight,
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);
		}
		else if (scaling == MUIV_DocumentLayout_Scaling_Zoom)
		{
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, (int)(docwidth * data->zoom),
				MUIA_PageView_LayoutHeight, (int)(docheight * data->zoom),
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);
		}

		if (scaling != MUIV_DocumentLayout_Scaling_Zoom && i == data->page)	   // this is a bit weird, but...
			data->zoom = (float)fpwidth / mediawidth;
	}   

	DoMethod(obj, MUIM_Group_ExitChange2, TRUE);

	return 0;
}

METHOD continuouslayoutSetupPage(struct IClass *cl, Object *obj, struct MUIP_ContinuousLayout_SetupPage *msg)
{
	GETDATA;
	int i, thresh = xget(obj, MUIA_TopEdge) + xget(obj, MUIA_Height) / 2;

	if (data->quiet)
		return 0;

	for(i=0; i<pdfGetPagesNum(data->doc); i++)
	{
		int top = xget(data->views[i], MUIA_TopEdge);
		int bottom = top + xget(data->views[i], MUIA_Height);

		if (top < thresh  && bottom > thresh)
		{
			if (data->page != i + 1)
			{
				data->quiet = TRUE; /* don't scroll when page is set */
				set(obj, MUIA_DocumentLayout_Page, i + 1);
				data->quiet = FALSE;
			}
			break;
		}
	}

	return 0;
}


METHOD continuouslayoutRefresh(struct IClass *cl, Object *obj, Msg msg)
{
	DoMethod(obj, MUIM_Group_InitChange);
	DoMethod(obj, MUIM_Group_ExitChange2, TRUE);
	//MUI_Redraw(obj, MADF_DRAWOBJECT);
	return TRUE;
}


BEGINMTABLE
	DECNEW
	DECSET
	DECGET
	DECDISP
	case MUIM_Show: return continuouslayoutShow(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_FindViewForPage: return continuouslayoutFindViewForPage(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_IsPageVisible: return continuouslayoutIsPageVisible(cl,obj,(APTR)msg);
	case MUIM_ContinuousLayout_Relayout: return continuouslayoutRelayout(cl,obj,(APTR)msg);
	case MUIM_ContinuousLayout_SetupPage: return continuouslayoutSetupPage(cl,obj,(APTR)msg);
	case MUIM_ContinuousLayout_ClippedPageOffset: return continuouslayoutClippedPageOffset(cl,obj,(APTR)msg);
	case MUIM_Setup: return continuouslayoutSetup(cl,obj,(APTR)msg);
	case MUIM_Cleanup: return continuouslayoutCleanup(cl,obj,(APTR)msg);
	case MUIM_HandleEvent: return continuouslayoutHandleEvent(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_Refresh: return continuouslayoutRefresh(cl,obj,(APTR)msg);

ENDMTABLE

DECSUBCLASS_NC(MUIC_Virtgroup, ContinuousLayoutClass)
DECSUBCLASSPTR_NC(LayClass, ContinuousDynamicLayoutClass)

