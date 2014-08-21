#define SYSTEM_PRIVATE 1

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

#include <proto/alib.h>
#include <proto/utility.h>

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
#include "outlineview_class.h"
#include "documentlayout_class.h"
#include "thumbnaillist_class.h"
#include "renderer_class.h"
#include "search_class.h"

#include "system/chunky.h"
#include "system/gentexture.h"

struct Data
{
	void *doc;
	Object *layoutgroup;
	Object *renderer;
	Object *grpDisplay;
};

#define gFalse 0

DEFNEW
{
	Object *grpDisplay;

	obj = DoSuperNew(cl, obj,
				MUIA_Group_Horiz, FALSE,
				MUIA_Frame, MUIV_Frame_InputList,   // to make it consistent with tree view
				Child, HGroup,
					Child, grpDisplay = VGroup, End,
					End,
				TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;

		data->renderer = (Object*)GetTagData(MUIA_ThumbnailList_Renderer, NULL, INITTAGS);
		data->doc = GetTagData(MUIA_ThumbnailList_PDFDocument, NULL, INITTAGS);

		data->grpDisplay = grpDisplay;

		if (data->doc == NULL)
		{
			CoerceMethod(cl, obj, OM_DISPOSE);
			return 0;
		}

		data->layoutgroup = ContinuousDynamicLayoutObject,
							MUIA_DocumentLayout_PDFDocument, data->doc,
							MUIA_PageView_Information, MUIV_PageView_Information_Number,
							MUIA_PageView_IsPreview, TRUE,
							MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_FixWidth(96),
							End;

		DoMethod(grpDisplay, OM_ADDMEMBER, ScrollgroupObject, MUIA_Scrollgroup_Contents, data->layoutgroup, End);

		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_NeedRefresh, MUIV_EveryTime, obj, 2, MUIM_DocumentView_EnqueueRender, MUIV_TriggerValue);
		DoMethod(data->layoutgroup, MUIM_Notify, MUIA_PageView_RedirectPage, MUIV_EveryTime, obj, 3, MUIM_Set, MUIA_DocumentView_Page, MUIV_TriggerValue);

	}

	return (ULONG)obj;
}

DEFDISP
{
	GETDATA;
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
            set(obj, MUIA_OutlineView_Page, page);
			nnset(data->layoutgroup, MUIA_DocumentLayout_Page, page);	/* also select the page in this view */
			break;
		}

		case MUIA_DocumentLayout_Zoom:
		case MUIA_DocumentLayout_Scaling:
			/* don't forward this one to the layout! */
			tag->ti_Tag = TAG_IGNORE;
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

		case MUIA_OutlineView_Page:
			return TRUE;
	}

	return(DOSUPER);
}

DEFMMETHOD(Cleanup)
{
	GETDATA;

	/* remove all pending pages from renderer */
	kprintf("cleanup renderer for thumbnail list %p\n", data->layoutgroup);
	DoMethod(data->renderer, MUIM_Renderer_Remove, MUIV_Renderer_Remove_All, data->layoutgroup);
	kprintf("   ..cleanup done\n");
	return DOSUPER;
}

DEFMMETHOD(DocumentView_EnqueueRender)
{
	GETDATA;
	DoMethod(data->renderer, MUIM_Renderer_Enqueue, msg->page, data->layoutgroup, MUIV_Renderer_Enqueue_Priority_Low);
	return 0;
}

DEFMMETHOD(DocumentView_FindViewForPage)
{
	GETDATA;
	return DoMethod(data->layoutgroup, MUIM_DocumentLayout_FindViewForPage, msg->page);
}

BEGINMTABLE
	DECNEW
	DECSET
	DECGET
	DECDISP
	DECMMETHOD(Cleanup)
	DECMMETHOD(DocumentView_EnqueueRender)
	DECMMETHOD(DocumentView_FindViewForPage)

ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, ThumbnailListClass)

