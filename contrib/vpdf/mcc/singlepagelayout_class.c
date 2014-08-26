
#if defined(__AROS__)
#define MUIMASTER_YES_INLINE_STDARG
#endif

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
#include <proto/dtclass.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#if defined(__MORPHOS__)
#include <emul/emulregs.h>
#include <emul/emulinterface.h>
#endif
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "pageview_class.h"
#include "documentview_class.h"
#include "documentlayout_class.h"

#include "system/chunky.h"
#include "system/functions.h"
#include "system/gentexture.h"

struct Data
{
	void *doc;
	Object **views;
	int prevwidth;
	struct MUI_EventHandlerNode eh;
	int page;
	int columns;
	int pagesnum;
	int clicky;
	int clickx;

	int scaling;
	int rotation;
	int scrollable;
	float zoom;
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
		
		data->doc = (void*)GetTagData(MUIA_DocumentLayout_PDFDocument, NULL, INITTAGS);
		data->scaling = MUIV_DocumentLayout_Scaling_FitPage;
		data->page = (int)GetTagData(MUIA_DocumentLayout_Page, 1, INITTAGS);
		data->zoom = 1.0f;
		data->rotation = GetTagData(MUIA_DocumentLayout_Rotation, MUIV_DocumentLayout_Rotation_None, INITTAGS);
		data->columns = GetTagData(MUIA_DocumentLayout_Columns, 2, INITTAGS);

		if (data->doc != NULL)
		{
			int i;
			int rangefirst = ((data->page - 1) / data->columns) * data->columns; /* first page to be displayed */

			/* for each page add pageview object */

			data->pagesnum = pdfGetPagesNum(data->doc);
			data->views = malloc(sizeof(Object*) * data->pagesnum);
			#warning TODO: handle failure

			//printf("rangefirst:%d\n", rangefirst);

			for(i=0; i<data->pagesnum; i++)
			{
				float mediawidth = pdfGetPageMediaWidth(data->doc, i + 1);
				float mediaheight = pdfGetPageMediaHeight(data->doc, i + 1);

				
				APTR pageview = PageViewObject,
								MUIA_PageView_PDFDocument, data->doc,
								MUIA_PageView_Page, i + 1,
								MUIA_PageView_MediaWidth, (int)mediawidth,
								MUIA_PageView_MediaHeight, (int)mediaheight,
								//MUIA_FixWidth , 128,
								//MUIA_FixHeight , 128,
								End;

				//DoMethod(obj, OM_ADDMEMBER, HSpace(0));
				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, MUIA_FixWidth, 128, MUIA_FixHeight, 128, End);

				if (i >= rangefirst && i < rangefirst + data->columns)
					DoMethod(obj, MUIM_Family_AddTail, pageview);

				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, MUIA_FixWidth, 50, End);
				//DoMethod(obj, OM_ADDMEMBER, HSpace(0));
				//DoMethod(obj, OM_ADDMEMBER, RectangleObject, End);
				data->views[i] = pageview;

			}

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
	int i;

	/* first remove pages displayed */

	DoMethod(obj, MUIM_Group_InitChange);

	for(i=0; i<data->columns; i++)
	{
		Object *child = (Object*)DoMethod(obj, MUIM_Group_GetChild, 0);
		if (child != NULL)
			DoMethod(obj, MUIM_Family_Remove, child);
	}

	DoMethod(obj, MUIM_Group_ExitChange);

	for(i=0; i<data->pagesnum; i++)
		MUI_DisposeObject(data->views[i]);

	free(data->views);

	/* now dispose all pages */

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
		case MUIA_PageView_RedirectPage:
			return TRUE;	/* this is only for notification */
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
			int prevpage = data->page - 1;
			int newpage = tag->ti_Data - 1;
			data->page = tag->ti_Data;
			
			/* check if there is a need to repopulate group */

			if (newpage / data->columns != prevpage / data->columns)
			{
				int i;
				int rangefirst = (newpage / data->columns) * data->columns;	/* first page to be displayed */
				//printf("setting new page for range:%d-%d\n", rangefirst, rangefirst + data->columns);
				//printf("old:0x%x new 0x%x\n", data->views[prevpage], data->views[newpage]);

				DoMethod(obj, MUIM_Group_InitChange);

				for(i=0; i<data->columns; i++)
				{
					Object *child = (Object*)DoMethod(obj, MUIM_Group_GetChild, 0);
					if (child != NULL)
						DoMethod(obj, MUIM_Family_Remove, child);
				}

				for(i=rangefirst; i<min(data->pagesnum, rangefirst + data->columns); i++)
					DoMethod(obj, MUIM_Family_AddTail, data->views[i]);

				DoMethod(obj, MUIM_Group_ExitChange);
			}
			else
			{
				MUI_Redraw(data->views[prevpage], MADF_DRAWUPDATE);
				MUI_Redraw(data->views[newpage], MADF_DRAWUPDATE);
			}
			break;
		}

		case MUIA_DocumentLayout_Zoom:
			data->zoom = tag->ti_Data / 65536.0f;
			relayout = TRUE;
			break;

		case MUIA_DocumentLayout_Scaling:
			data->scaling = tag->ti_Data;
			relayout = TRUE;
			break;
			
		case MUIA_DocumentLayout_Rotation:
			data->rotation = tag->ti_Data;
			relayout = TRUE;
			break;

	}
	NEXTTAG

	if (relayout)
		DoMethod(obj, MUIM_SinglePageLayout_Relayout);

	return DOSUPER;
}

///	setup

METHOD singlepagelayoutSetup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
	GETDATA;
	ULONG rc = DOSUPER;

	if (rc == 0)
		return rc;

	data->eh.ehn_Object = obj;
	data->eh.ehn_Class  = cl;
	data->eh.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY;
	data->eh.ehn_Priority = 100;
	data->eh.ehn_Flags = MUI_EHF_GUIMODE;
	DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->eh);

	return rc;
}

////

///	cleanup

METHOD singlepagelayoutCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	GETDATA;

	if (data->eh.ehn_Object != NULL)
		DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->eh);

	return DOSUPER;
}

////

///	handleevent

METHOD singlepagelayoutHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
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
						// TODO: this is not optimal as it assumes about a parent but let it be for now...
						if (xget(objFindContainerByAttribute(obj, MUIA_DocumentView_DragAction), MUIA_DocumentView_DragAction) == MUIV_DocumentView_DragAction_Scroll)
						{
							data->clickx = msg->imsg->MouseX;
							data->clicky = msg->imsg->MouseY;
							data->eh.ehn_Events |= IDCMP_MOUSEMOVE;
						}
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

				case IDCMP_RAWKEY:
					/* first scroll then turn the page */
					if (msg->imsg->Code == RAWKEY_NM_WHEEL_DOWN)
					{
						if (xget(obj, MUIA_Virtgroup_Top) + _mheight(obj) ==  xget(obj, MUIA_Virtgroup_Height) && data->page < data->pagesnum)
						{
							SetAttrs(obj, MUIA_DocumentLayout_Page, data->page + 1,
											MUIA_Virtgroup_Top, 0, TAG_DONE);
							return MUI_EventHandlerRC_Eat;
						}
					}
					else if (msg->imsg->Code == RAWKEY_NM_WHEEL_UP)
					{
						if (xget(obj, MUIA_Virtgroup_Top) == 0 && data->page > 1)
						{
							SetAttrs(obj, MUIA_DocumentLayout_Page, data->page - 1,
											MUIA_Virtgroup_Top, xget(obj, MUIA_Virtgroup_Height), TAG_DONE);
							return MUI_EventHandlerRC_Eat;
						}
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

METHOD singlepagelayoutFindViewForPage(struct IClass *cl, Object *obj, struct MUIP_DocumentLayout_FindViewForPage *msg)
{
	GETDATA;
	return (ULONG)data->views[msg->page - 1];
}

METHOD singlepagelayoutIsPageVisible(struct IClass *cl, Object *obj, struct MUIP_DocumentLayout_IsPageVisible *msg)
{
	GETDATA;
	int rangefirst = ((data->page - 1) / data->columns) * data->columns; /* first page to be displayed */

	return (msg->page - 1) >= rangefirst && (msg->page - 1) < rangefirst + data->columns;
}

METHOD singlepagelayoutShow(struct IClass *cl, Object *obj, struct MUIP_Show *msg)
{
	GETDATA;

	/* order relayout if width changed */

	if (_mwidth(obj) != data->prevwidth)
	{
		data->prevwidth = _mwidth(obj);
		DoMethod(_app(obj), MUIM_Application_PushMethod, obj, 1, MUIM_SinglePageLayout_Relayout);
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

METHOD singlepagelayoutRelayout(struct IClass *cl, Object *obj, struct MUIP_ContinuousLayout_Relayout *msg)
{
	GETDATA;
	int i;
	int width = _mwidth(obj) / data->columns - 10 * (data->columns - 1);
	int height = _mheight(obj);
	int fpwidth, fpheight;
	float docwidth, docheight;
	pdfGetDocumentDimensions(data->doc, &docwidth, &docheight);

	/* adjust dimensions of all objects */

	DoMethod(obj, MUIM_Group_InitChange);

	for(i=0; i<data->pagesnum; i++)
	{
		int mediawidth = xget(data->views[i], MUIA_PageView_MediaWidth);
		int mediaheight = xget(data->views[i], MUIA_PageView_MediaHeight);
		
		applyrotation(&mediawidth, &mediaheight, data->rotation);
		//kprintf("relayout view:0x%x\n", data->views[i]);

		//kprintf("relayout to:%d, %d pages:%d\n", fpwidth, fpheight, data->pagesnum);

		if (data->scaling == MUIV_DocumentLayout_Scaling_FitWidth)
		{
			/* here we differentiate depending on number of columns. with >1 we render all of pages using document metrics to preserve font size */
			if (data->columns == 1)
			{
				scaleCoordsWithAspect(mediawidth, mediaheight, width, height * 100.0f, &fpwidth, &fpheight);
				SetAttrs(data->views[i],
					MUIA_PageView_LayoutWidth, fpwidth,
					MUIA_PageView_LayoutHeight, fpheight,
					MUIA_PageView_Rotation, data->rotation,
					TAG_DONE);
			}
			else
			{
				scaleCoordsWithAspect(docwidth, docheight, width, height * 100.0f, &fpwidth, &fpheight);
				SetAttrs(data->views[i],
					MUIA_PageView_LayoutWidth, fpwidth,
					MUIA_PageView_LayoutHeight, fpheight,
					MUIA_PageView_Rotation, data->rotation,
					TAG_DONE);
			}

			if (i == data->page)	// this is a bit weird, but...
				data->zoom = (float)fpwidth / mediawidth;
		}
		else if (data->scaling == MUIV_DocumentLayout_Scaling_FitPage)
		{
			scaleCoordsWithAspect(mediawidth, mediaheight, width, height, &fpwidth, &fpheight);
			
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, fpwidth,
				MUIA_PageView_LayoutHeight, fpheight,
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);

			if (i == data->page)	// this is a bit weird, but...
				data->zoom = (float)fpwidth / mediawidth;
		}
		else if (data->scaling == MUIV_DocumentLayout_Scaling_Zoom)
		{
			SetAttrs(data->views[i],
				MUIA_PageView_LayoutWidth, (int)(mediawidth * data->zoom),
				MUIA_PageView_LayoutHeight, (int)(mediaheight * data->zoom),
				MUIA_PageView_Rotation, data->rotation,
				TAG_DONE);
		}
	}

	DoMethod(obj, MUIM_Group_ExitChange2, TRUE);
	//kprintf("relayout done:%d\n", (int)(data->zoom * 100));
	return 0;
}

METHOD singlepagelayoutRefresh(struct IClass *cl, Object *obj, Msg msg)
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
	case MUIM_Show: return singlepagelayoutShow(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_FindViewForPage: return singlepagelayoutFindViewForPage(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_IsPageVisible: return singlepagelayoutIsPageVisible(cl,obj,(APTR)msg);
	case MUIM_Setup: return singlepagelayoutSetup(cl,obj,(APTR)msg);
	case MUIM_Cleanup: return singlepagelayoutCleanup(cl,obj,(APTR)msg);
	case MUIM_HandleEvent: return singlepagelayoutHandleEvent(cl,obj,(APTR)msg);
	case MUIM_SinglePageLayout_Relayout: return singlepagelayoutRelayout(cl,obj,(APTR)msg);
	case MUIM_DocumentLayout_Refresh: return singlepagelayoutRefresh(cl,obj,(APTR)msg);

ENDMTABLE

DECSUBCLASS_NC(MUIC_Virtgroup, SinglePageLayoutClass)

