#if defined(__AROS__)
#define MUIMASTER_YES_INLINE_STDARG
#endif

#define SYSTEM_PRIVATE 1

/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>

#if !defined(__AROS__)
#include <libraries/charsets.h>
#endif

#include <libraries/locale.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <proto/alib.h>
#include <proto/utility.h>

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
#include "outlineview_class.h"
#include "documentlayout_class.h"
#include "thumbnaillist_class.h"
#include "renderer_class.h"
#include "search_class.h"
#include "annotation_class.h"
#include "toolbar_class.h"
#include "clipboard.h"

#include "system/chunky.h"
#include "system/gentexture.h"
#include "../locale.h"

struct Data
{
	void *doc;
	Object *layoutgroup;
	Object *renderer;
	Object *sldPage;
	Object *grpDisplay;
	Object *grpSearch;
	Object *grpToolbar;
	Object *grpOutline;
	int dragaction;
	int layoutmode;
	int renderpriority;
	char *filename;
	int rotation;
	struct MinList **annotationLists; // annotations are loaded on demand
	struct MUI_EventHandlerNode eh;

};

#define D(x)
#define gFalse 0
#define ANNOTLIST_INVALID 0
#define ANNOTLIST_EMPTY ((void*)-1)

#define ANNOTFLAG_ATTACHED 1

static char *outlinenames[] = {"Tree", "Thumbnails", NULL};

static Object *buildlayoutobject(int layoutmode, struct Data *data)
{
	switch(layoutmode)
	{
		case MUIV_DocumentView_Layout_Single:
			return SinglePageLayoutObject,
							MUIA_DocumentLayout_PDFDocument, data->doc,
							MUIA_DocumentLayout_Columns, 1,
							MUIA_DocumentLayout_Rotation, data->rotation,
							End;

		case MUIV_DocumentView_Layout_ContinuousSingle:
			return ContinuousLayoutObject,
							MUIA_DocumentLayout_PDFDocument, data->doc,
							MUIA_DocumentLayout_Columns, 1,
							MUIA_DocumentLayout_Rotation, data->rotation,
							End;

		case MUIV_DocumentView_Layout_Facing:
			return SinglePageLayoutObject,
							MUIA_DocumentLayout_PDFDocument, data->doc,
							MUIA_DocumentLayout_Columns, 2,
							MUIA_DocumentLayout_Rotation, data->rotation,
							End;

		case MUIV_DocumentView_Layout_ContinuousFacing:
			return ContinuousLayoutObject,
							MUIA_DocumentLayout_PDFDocument, data->doc,
							MUIA_DocumentLayout_Columns, 2,
							MUIA_DocumentLayout_Rotation, data->rotation,
							End;
			break;
	}

	return NULL; /* let it hit! */
}

DEFNEW
{
	Object *sldPage, *grpOutlines, *grpOutlinesTitles, *grpToolbar;
	Object *grpDisplay, *grpOutlineTree, *grpNavigation, *balBalance, *grpSearch, *grpOutlineThumbs;

	obj = DoSuperNew(cl, obj,
				MUIA_Group_Horiz, FALSE,
				Child, HGroup,
					Child, grpDisplay = VGroup, End,
					Child, balBalance = BalanceObject, End,
					Child, grpOutlines = VGroup,
						MUIA_ShowMe, GetTagData(MUIA_DocumentView_Outline, TRUE, INITTAGS),
						MUIA_Group_PageMode, TRUE,
						MUIA_Frame, MUIV_Frame_Group,
						MUIA_Weight, 30,
						Child, grpOutlinesTitles = MUI_NewObject(MUIC_Title,
							TAG_DONE),
						End,
					End,
				Child, HGroup,
					Child, grpNavigation = HGroup,
						Child, sldPage = SliderObject,
							MUIA_Slider_Level, 1,
							MUIA_Numeric_Default, 1,
							MUIA_Slider_Min,1,
							MUIA_Slider_Max,100,
							MUIA_Weight,200,
							MUIA_Numeric_Format,  LOCSTR( MSG_SLIDER_PAGE  ),
							MUIA_CycleChain, TRUE,
							End,
						Child, HSpace(5),
						Child, grpSearch = SearchObject,
							End,
						End,
					End,
				Child, grpToolbar = ToolbarObject,
					MUIA_DocumentView_Outline, GetTagData(MUIA_DocumentView_Outline, TRUE, INITTAGS),
					End,

				TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		int outlinenum = 0;
		int i;
		
		data->dragaction = MUIV_DocumentView_DragAction_Scroll;
		data->rotation = MUIV_DocumentLayout_Rotation_None;
		data->filename = strdup((char*)GetTagData(MUIA_DocumentView_FileName, NULL, INITTAGS));
		data->renderer = (Object*)GetTagData(MUIA_DocumentView_Renderer, NULL, INITTAGS);
		data->doc = pdfNew(data->filename);
		data->layoutmode = GetTagData(MUIA_DocumentView_Layout, MUIV_DocumentView_Layout_Single, INITTAGS);
		data->renderpriority = GetTagData(MUIA_DocumentView_RenderPriority, MUIV_Renderer_Enqueue_Priority_Normal, INITTAGS);

		data->sldPage = sldPage;
		data->grpDisplay = grpDisplay;
		data->grpToolbar = grpToolbar;

		if (data->doc == NULL)
		{
			CoerceMethod(cl, obj, OM_DISPOSE);
			return 0;
		}
		
		data->annotationLists = calloc(pdfGetPagesNum(data->doc), sizeof(struct MinList*));
		for(i=pdfGetPagesNum(data->doc)-1; i>=0; i--)
			data->annotationLists[i] = ANNOTLIST_INVALID;
		

		/* setup search group */

		set(grpSearch, MUIA_Search_DocumentView, obj);

		/* setup pages slider */

		set(sldPage, MUIA_Slider_Max, pdfGetPagesNum(data->doc));
		data->layoutgroup = buildlayoutobject(data->layoutmode, data);
		if (data->layoutgroup != NULL)
		{
			int scaling = GetTagData(MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_FitPage, INITTAGS);
			if (scaling == MUIV_DocumentLayout_Scaling_None)
				scaling = MUIV_DocumentLayout_Scaling_Zoom;
				
			set(data->layoutgroup, MUIA_DocumentLayout_Scaling, scaling);
		}

		DoMethod(grpDisplay, OM_ADDMEMBER, ScrollgroupObject, MUIA_Scrollgroup_Contents, data->layoutgroup, End);

		{
			Object *outline = OutlineViewObject,
								MUIA_OutlineView_Document, data->doc,
								End;

			if (xget(outline, MUIA_OutlineView_IsEmpty) == FALSE)
			{
				DoMethod(grpOutlines, OM_ADDMEMBER, outline);
				DoMethod(grpOutlinesTitles, OM_ADDMEMBER, TextObject, MUIA_Text_Contents, "Tree", End);
				outlinenum++;
			}
			else
			{
				MUI_DisposeObject(outline);
				outline = NULL;
			}
			
			data->grpOutline = grpOutlines;
		}

		{
			Object *thumbnails = ThumbnailListObject,
								MUIA_ThumbnailList_PDFDocument, data->doc,
								MUIA_ThumbnailList_Renderer, data->renderer,
								End;

			DoMethod(grpOutlines, OM_ADDMEMBER, thumbnails);
			if (outlinenum > 0)
				DoMethod(grpOutlinesTitles, OM_ADDMEMBER, TextObject, MUIA_Text_Contents, "Thumbnails", End);
				
			outlinenum++;
		}

		if (outlinenum == 1)
		{
			DoMethod(grpOutlines, OM_REMMEMBER, grpOutlinesTitles);
			MUI_DisposeObject(grpOutlinesTitles);
		}

		/* attach toolbar to the document view */

		set(grpToolbar, MUIA_Toolbar_DocumentView, obj);

		/*	*/

		DoMethod(grpOutlines, MUIM_Notify, MUIA_OutlineView_Page, MUIV_EveryTime, sldPage, 3, MUIM_Set, MUIA_Slider_Level, MUIV_TriggerValue);

		DoMethod(sldPage, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime, obj, 3, MUIM_Set, MUIA_DocumentView_Page, MUIV_TriggerValue);
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_DocumentLayout_Page, MUIV_EveryTime, sldPage, 3, MUIM_NoNotifySet, MUIA_Slider_Level, MUIV_TriggerValue);
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_DocumentLayout_Page, MUIV_EveryTime, obj, 2, MUIM_DocumentView_UpdateAnnotations, MUIV_TriggerValue);
		
		/* NOTE: this will add notification for all child objects!! todo: rework so no time/memory is wasted! */
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_NeedRefresh, MUIV_EveryTime, obj, 2, MUIM_DocumentView_EnqueueRender, MUIV_TriggerValue);
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_RedirectPage, MUIV_EveryTime, obj, 3, MUIM_Set, MUIA_DocumentView_Page, MUIV_TriggerValue);
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_RedirectPage, MUIV_EveryTime, data->sldPage, 3, MUIM_NoNotifySet, MUIA_Slider_Level, MUIV_TriggerValue);

	}

	return (ULONG)obj;
}

DEFDISP
{
	GETDATA;

	if (data->doc != NULL)
		pdfDelete(data->doc);
		
	if (data->filename != NULL)
		free(data->filename);
		
	return DOSUPER;
}

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{
		case MUIA_DocumentView_Page:
		{
			int page = tag->ti_Data;
			SetAttrs(data->layoutgroup, MUIA_Group_Forward, FALSE, MUIA_DocumentLayout_Page, page, TAG_DONE);
			break;
		}

		case MUIA_DocumentView_Layout:
			DoMethod(obj, MUIM_DocumentView_Layout, tag->ti_Data); /* this will set layoutmode */
			break;

		case MUIA_DocumentView_Outline:
			if (data->grpOutline != NULL)
				set(data->grpOutline, MUIA_ShowMe, tag->ti_Data);
			break;

		case MUIA_DocumentView_DragAction:
			data->dragaction = tag->ti_Data;
			break;


	}
	NEXTTAG

	return DOSUPER;
}

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_DocumentLayout_Zoom:
			*(ULONG*)msg->opg_Storage = xget(data->layoutgroup, msg->opg_AttrID);
			return TRUE;

		case MUIA_DocumentView_PDFDocument:
			*(ULONG*)msg->opg_Storage = (ULONG)data->doc;
			return TRUE;

		case MUIA_DocumentView_Page:
			*(ULONG*)msg->opg_Storage = (ULONG)xget(data->sldPage, MUIA_Slider_Level);
			return TRUE;

		case MUIA_DocumentView_Layout:
			*(ULONG*)msg->opg_Storage = (ULONG)data->layoutmode;
			return TRUE;
			
		case MUIA_DocumentView_FileName:
			*(ULONG*)msg->opg_Storage = (ULONG)data->filename;
			return TRUE;
			
		case MUIA_DocumentView_DragAction:
			*(ULONG*)msg->opg_Storage = (ULONG)data->dragaction;
			return TRUE;
	}

	return(DOSUPER);
}

DEFMMETHOD(Setup)
{
	GETDATA;
	
	data->eh.ehn_Object = obj;
	data->eh.ehn_Class  = cl;
	data->eh.ehn_Events = IDCMP_RAWKEY;
	data->eh.ehn_Priority = 100;
	data->eh.ehn_Flags = MUI_EHF_GUIMODE;
	DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->eh);
	
	return DOSUPER;
}

DEFMMETHOD(Cleanup)
{
	GETDATA;

	/* remove all pending pages from renderer */
	D(kprintf("cleanup renderer for view %p\n", data->layoutgroup));
	DoMethod(data->renderer, MUIM_Renderer_Remove, MUIV_Renderer_Remove_All, data->layoutgroup);
	D(kprintf("   ..cleanup done\n"));
	
	if (data->eh.ehn_Object != NULL)
	{
		DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->eh);
		data->eh.ehn_Object = NULL;
	}
	
	return DOSUPER;
}



DEFMMETHOD(DocumentView_EnqueueRender)
{
	GETDATA;
	//kprintf("enqueue with high priority:%d\n", msg->page);
	DoMethod(data->renderer, MUIM_Renderer_Enqueue, msg->page, data->layoutgroup, data->renderpriority);
	
	// rerendered so probably annotations have to be reprocessed. optimize this pass..
	DoMethod(_app(obj), MUIM_Application_PushMethod, obj,2, MUIM_DocumentView_UpdateAnnotations, msg->page);
	
	
	return 0;
}

DEFMMETHOD(DocumentView_Layout)
{
	GETDATA;
	Object *grpDisplayChild;
	ULONG zoom = xget(data->layoutgroup, MUIA_DocumentLayout_Zoom);
	ULONG scaling = xget(data->layoutgroup, MUIA_DocumentLayout_Scaling);
	LONG page = xget(data->layoutgroup, MUIA_DocumentLayout_Page);
	LONG pagenum;

	D(kprintf("cleanup renderer for view %p\n", data->layoutgroup));
	DoMethod(data->renderer, MUIM_Renderer_Remove, MUIV_Renderer_Remove_All, data->layoutgroup);
	D(kprintf("   ..cleanup done\n"));

	grpDisplayChild = (Object*)DoMethod(data->grpDisplay, MUIM_Family_GetChild, MUIV_Family_GetChild_First);
	DoMethod(data->grpDisplay, MUIM_Group_InitChange);
	DoMethod(data->grpDisplay, MUIM_Group_Remove, grpDisplayChild);
	MUI_DisposeObject(grpDisplayChild);

	/* disposing layout object disposed all annotation objects which are currently attached to pages. mark them as invalid */

	for(pagenum=1; pagenum<=pdfGetPagesNum(data->doc); pagenum++)
	{
		struct MinList *l = data->annotationLists[pagenum - 1];
		if (l != ANNOTLIST_INVALID && l != ANNOTLIST_EMPTY)
		{
            struct pdfAnnotation *an;
			ITERATELIST(an, l)
			{
				an->obj = NULL;
			}
		}
	}

	data->layoutmode = msg->layout;
	data->layoutgroup = buildlayoutobject(msg->layout, data);
	SetAttrs(data->layoutgroup, MUIA_DocumentLayout_Zoom, zoom, MUIA_DocumentLayout_Scaling, scaling, TAG_DONE);

	DoMethod(data->grpDisplay, OM_ADDMEMBER, ScrollgroupObject, MUIA_Scrollgroup_Contents, data->layoutgroup, End);
	DoMethod(data->grpDisplay, MUIM_Group_ExitChange);

	/* NOTE: using pushmethod ensures that object will be properly layed out and focusing on page will work properly. investigate in mui! */
	#warning investigate!
	DoMethod(_app(obj), MUIM_Application_PushMethod, data->layoutgroup, 3, MUIM_Set, MUIA_DocumentLayout_Page, page);
	//set(data->layoutgroup, MUIA_DocumentLayout_Page, page);

	DoMethod(data->layoutgroup, MUIM_Notify, MUIA_DocumentLayout_Page, MUIV_EveryTime, data->sldPage, 3, MUIM_NoNotifySet, MUIA_Slider_Level, MUIV_TriggerValue);
	/* NOTE: this will add notification for all child objects!! todo: rework so no time/memory is wasted! */
	DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_NeedRefresh, MUIV_EveryTime, obj, 2, MUIM_DocumentView_EnqueueRender, MUIV_TriggerValue);
	DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_RedirectPage, MUIV_EveryTime, obj, 3, MUIM_Set, MUIA_DocumentView_Page, MUIV_TriggerValue);
	DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_RedirectPage, MUIV_EveryTime, data->sldPage, 3, MUIM_NoNotifySet, MUIA_Slider_Level, MUIV_TriggerValue);

	return 0;
}

DEFMMETHOD(DocumentView_FindViewForPage)
{
	GETDATA;
	return DoMethod(data->layoutgroup, MUIM_DocumentLayout_FindViewForPage, msg->page);
}

DEFMMETHOD(HandleEvent)
{

	GETDATA;
	int rc = 0;

	if (msg->imsg != NULL)
	{
		switch (msg->imsg->Class)
		{
			case IDCMP_RAWKEY:
				if (msg->imsg->Code == RAWKEY_SPACE)
				{
					set(data->sldPage, MUIA_Slider_Level, xget(data->sldPage, MUIA_Slider_Level) + 1);
					rc = MUI_EventHandlerRC_Eat;
				}
				else if (msg->imsg->Code == RAWKEY_BACKSPACE)
				{
					set(data->sldPage, MUIA_Slider_Level, xget(data->sldPage, MUIA_Slider_Level) - 1);
					rc = MUI_EventHandlerRC_Eat;
				}
				break;
		}
		
	}
	
	return rc;
	
}

DEFMMETHOD(DocumentView_RotateLeft)
{
	GETDATA;
	data->rotation = data->rotation == MUIV_PageView_Rotation_None ? MUIV_PageView_Rotation_270 : data->rotation - 1;
	SetAttrs(data->layoutgroup, MUIA_DocumentLayout_Rotation, data->rotation, TAG_DONE);
	return TRUE;
}

DEFMMETHOD(DocumentView_RotateRight)
{
	GETDATA;
	data->rotation = data->rotation == MUIV_PageView_Rotation_270 ? MUIV_PageView_Rotation_None : data->rotation + 1;
	SetAttrs(data->layoutgroup, MUIA_DocumentLayout_Rotation, data->rotation, TAG_DONE);
	return TRUE;
}

DEFMMETHOD(DocumentView_UpdateAnnotations)
{
	GETDATA;
	int refpagenum = msg->page;
	int pagenum;
	int first = -1;
	int last;
	int modified = FALSE;
	
	/* for each visible page around the reference one we display annotations objects */
	
	for(first = refpagenum; first > 1 && DoMethod(data->layoutgroup, MUIM_DocumentLayout_IsPageVisible, first - 1); first--) {}
	for(last = first; last < pdfGetPagesNum(data->doc) && DoMethod(data->layoutgroup, MUIM_DocumentLayout_IsPageVisible, last + 1); last++) {}
	
	//printf("process annotations...:%d:%d-%d\n", refpagenum, first, last);

	for(pagenum=first; pagenum<=last; pagenum++)
	{
		struct MinList *l = data->annotationLists[pagenum - 1];
		if (l == ANNOTLIST_INVALID)
		{
			struct MinList *annotList;
			
			//printf("fetch annotations for page %d\n", pagenum);
			annotList = pdfGetAnnotations(data->doc, pagenum);
			if (annotList == NULL)
				annotList = ANNOTLIST_EMPTY;
				
			data->annotationLists[pagenum - 1] = annotList;
		}
	}

	
	for(pagenum = pdfGetPagesNum(data->doc); pagenum >= 1; pagenum--)
	{
		struct MinList *l = data->annotationLists[pagenum-1];
		if (l != ANNOTLIST_EMPTY && l != ANNOTLIST_INVALID)
		{
			struct pdfAnnotation *an;
			Object *pageview = (Object*)DoMethod(data->layoutgroup, MUIM_DocumentLayout_FindViewForPage, pagenum);
			
			if (pagenum < first || pagenum> last)
			{
				ITERATELIST(an, l)
				{	
					if (an->obj != NULL)
					{
						//printf("  remove annotation from page %d\n", pagenum);
						if (!modified)
							DoMethod(data->layoutgroup, MUIM_Group_InitChange);
						modified = TRUE;
							
						DoMethod(data->layoutgroup, MUIM_Family_Remove, an->obj);
						DoMethod(pageview, MUIM_PageView_RemoveAnnotation, an->obj);
						MUI_DisposeObject(an->obj);
						an->obj = NULL;
					}		
				}
			}
			else
			{
				ITERATELIST(an, l)
				{	
					if (an->obj == NULL)
					{
						float coords[4];
						//printf("   new annotation for page %d\n", pagenum);
						if (!modified)
							DoMethod(data->layoutgroup, MUIM_Group_InitChange);
						modified = TRUE;
							
						an->obj = AnnotationObject,
							MUIA_Annotation_Contents, an->contents,
							MUIA_Annotation_RefObject, pageview,
							End;
						DoMethod(data->layoutgroup, MUIM_Family_AddTail, an->obj);
						coords[0] = an->x1;
						coords[1] = an->y1;
						coords[2] = an->x2;
						coords[3] = an->y2;
						DoMethod(pageview, MUIM_PageView_AddAnnotation, an->obj, coords);
					}		
				}
			}
		}
	}
	
	if (modified)
		DoMethod(data->layoutgroup, MUIM_Group_ExitChange);

	return TRUE;
}

DEFMMETHOD(DocumentView_ClearSelection)
{
	GETDATA;
	int pagenum;
	
	for(pagenum=1; pagenum<=pdfGetPagesNum(data->doc); pagenum++)
	{
		Object *pageview = (Object*)DoMethod(data->layoutgroup, MUIM_DocumentLayout_FindViewForPage, pagenum);
		if (pageview != NULL)
		{
			DoMethod(pageview, MUIM_PageView_ClearSelection);
		}
	}
	

	return TRUE;
}

DEFMMETHOD(DocumentView_SelectionCopy)
{
	GETDATA;
	int pagenum;
	
	for(pagenum=1; pagenum<=pdfGetPagesNum(data->doc); pagenum++)
	{
	
		Object *pageview = (Object*)DoMethod(data->layoutgroup, MUIM_DocumentLayout_FindViewForPage, pagenum);
		if (pageview != NULL)
		{
			struct MUIP_PageView_GetSelection msg;
			msg.MethodID = MUIM_PageView_GetSelection;
			if (DoMethodA(pageview, &msg))
			{
				struct pdfSelectionText *selection =  pdfBuildTextForSelection(data->doc, pagenum, msg.region.x1, msg.region.y1, msg.region.x2, msg.region.y2);
				if (selection != NULL)
				{
					clipboard_write_text(selection->utf8, CODESET_UTF8);
					pdfDisposeTextForSelection(data->doc, selection);
				}
				return TRUE;
			}
		}
	}
	

	return FALSE;
}

BEGINMTABLE
	DECNEW
	DECSET
	DECGET
	DECDISP
	DECMMETHOD(Cleanup)
	DECMMETHOD(Setup)
	DECMMETHOD(HandleEvent)
	DECMMETHOD(DocumentView_EnqueueRender)
	DECMMETHOD(DocumentView_Layout)
	DECMMETHOD(DocumentView_FindViewForPage)
	DECMMETHOD(DocumentView_RotateRight)
	DECMMETHOD(DocumentView_RotateLeft)
	DECMMETHOD(DocumentView_UpdateAnnotations)
	DECMMETHOD(DocumentView_ClearSelection)
	DECMMETHOD(DocumentView_SelectionCopy)

ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, DocumentViewClass)

