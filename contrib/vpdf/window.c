
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
#include <memory.h>

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
#include "application.h"
#include "window.h"
#include "settings.h"
#include "system/functions.h"

#include "mcc/documentview_class.h"
#include "mcc/documentlayout_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"

#define MAXRECENT 10

struct Data
{
	unsigned int id;
	char muiid[64];
	Object *grpRoot;
	Object *grpTitles;
	Object *btnErrors;
	char title[256];
};


DEFNEW
{
	Object *grpRoot, *grpTitles;

	obj = DoSuperNew(cl, obj,
						MUIA_Window_Title,"VPDF",
						MUIA_Window_Width, MUIV_Window_Width_Screen(50),
						MUIA_Window_Height, MUIV_Window_Height_Screen(50),
						MUIA_Window_AppWindow, TRUE,
						MUIA_Window_RootObject,
						grpRoot = VGroup,
							MUIA_Background, MUII_RegisterBack,
							MUIA_Frame, MUIV_Frame_Register,
							MUIA_Group_PageMode, TRUE,
							Child, grpTitles = NewObject(getTitleClass(), NULL,
								MUIA_Title_Closable, TRUE,
								MUIA_Title_Sortable, TRUE,
								TAG_DONE),
							End,
						TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		memset(data, 0, sizeof(struct Data));
		data->id = GetTagData(MUIA_VPDFWindow_ID, 0, INITTAGS);
		data->grpRoot = grpRoot;
		data->grpTitles = grpTitles;

		/* setup window id */

		DoMethod(obj, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime, MUIV_Notify_Application, 3, MUIM_VPDF_HandleAppMessage, data->id, MUIV_TriggerValue);
		DoMethod(grpRoot, MUIM_Notify, MUIA_Group_ActivePage, MUIV_EveryTime, obj, 2, MUIM_VPDFWindow_UpdateActive, MUIV_TriggerValue);

		DoMethod(obj, MUIM_VPDFWindow_CreateTab);
	}          

	return (ULONG)obj;
}

DEFMMETHOD(VPDFWindow_CreateTab)
{
	GETDATA;

	/* place empty object into tab */

	DoMethod(data->grpRoot, MUIM_Group_InitChange);
	DoMethod(data->grpTitles, MUIM_Group_InitChange);
	DoMethod(data->grpTitles, MUIM_Family_AddTail, VPDFTitleButtonObject,
					Child, TextObject, 
						MUIA_Text_Contents, "No file loaded",
						End,
					End);

	DoMethod(data->grpRoot, MUIM_Family_AddTail, VGroup,
				MUIA_UserData, TRUE,		// move it to subclass
				Child, RectangleObject,
					End,
				End);

	DoMethod(data->grpRoot, MUIM_Group_ExitChange);
	DoMethod(data->grpTitles, MUIM_Group_ExitChange);

	/* return index of new group member */

	set(data->grpRoot, MUIA_Group_ActivePage, xget(data->grpRoot, MUIA_Family_ChildCount) - 1);
	return xget(data->grpRoot, MUIA_Family_ChildCount) - 1;
}

DEFMMETHOD(VPDFWindow_OpenFile)
{
	GETDATA;
	char *filename = msg->filename;
	int mode = msg->mode;
	int newtab;

	if (mode == MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty)
	{
		int tabind = xget(data->grpRoot, MUIA_Group_ActivePage);
		Object *tcontents = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, tabind + 1); // +1 for title object
		if (xget(tcontents, MUIA_UserData) == TRUE)	// marker for empty page. fix
			newtab = FALSE;
		else
			newtab = TRUE;
	}
	else if (mode == MUIV_VPDFWindow_OpenFile_NewTab)
	{
		newtab = TRUE;
	}
	else // if (MUIV_VPDFWindow_OpenFile_CurrentTab)
	{
		newtab = FALSE;
	}

	{
		Object *document;
		int tabind, i;
		Object *contents, *tcontents;

		if (newtab)
			tabind = DoMethod(obj, MUIM_VPDFWindow_CreateTab);
		else
			tabind = xget(data->grpRoot, MUIA_Group_ActivePage);

		/* setup new label */

		tcontents = (Object*)DoMethod(data->grpTitles, MUIM_Family_GetChild, tabind);
		set(tcontents, MUIA_Text_Contents, filename);

		/* dispose old contents and add new browser */

		contents = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, tabind + 1); /*  +1 for title object */
		DoMethod(contents, MUIM_Group_InitChange);
		tcontents = (Object*)DoMethod(contents, MUIM_Family_GetChild, 0);

		DoMethod(contents, OM_REMMEMBER, tcontents);
		MUI_DisposeObject(tcontents);

		tcontents = VGroup,
					Child, document = DocumentViewObject,
					MUIA_DocumentView_Renderer, xget(_app(obj), MUIA_VPDF_Renderer),
					MUIA_DocumentView_FileName, filename,
					MUIA_DocumentView_Layout, xget(_app(obj), MUIA_VPDFSettings_Layout),
					MUIA_DocumentLayout_Scaling, xget(_app(obj), MUIA_VPDFSettings_Scaling),
					End,
				End;

		if (tcontents != NULL)
			DoMethod(contents, MUIM_Family_AddTail, tcontents);

		set(contents, MUIA_UserData, FALSE); // mark as not empty
		DoMethod(contents, MUIM_Group_ExitChange);
		DoMethod(obj, MUIM_VPDFWindow_UpdateActive, tabind);
		return tcontents != NULL;
	}
}

DEFMMETHOD(VPDFWindow_UpdateActive)
{
	GETDATA;
	
	Object *group = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, msg->active + 1); /* +1 for title object */
	group = (Object*)DoMethod(group, MUIM_Family_GetChild, 0);
	
	if (group != NULL)
	{
		void *doc = xget(group, MUIA_DocumentView_PDFDocument);
		struct pdfAttribute *attr = NULL;
		
		if (doc != NULL)
			attr = pdfGetAttr(doc, PDFATTR_TITLE);
		
		if (attr != NULL && attr->value.s != NULL)
			snprintf(data->title, sizeof(data->title), "VPDF [%s]", attr->value.s);
		else if (xget(group, MUIA_DocumentView_FileName))
			snprintf(data->title, sizeof(data->title), "VPDF [%s]", FilePart((char*)xget(group, MUIA_DocumentView_FileName)));
		else
			snprintf(data->title, sizeof(data->title), "VPDF");
			
		set(obj, MUIA_Window_Title, data->title);
		pdfFreeAttr(doc, attr);
	}

	return 0;
}

DEFMMETHOD(VPDFWindow_DetachView)
{
	GETDATA;
	
	if (msg->tabind == MUIV_VPDFWindow_DetachView_Active)
	{
		int tabind = xget(data->grpRoot, MUIA_Group_ActivePage);
		Object *tcontents = (Object*)DoMethod(data->grpTitles, MUIM_Family_GetChild, tabind);
		Object *contents = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, tabind + 1);

		DoMethod(data->grpTitles, MUIM_Group_InitChange);
		DoMethod(data->grpRoot, MUIM_Group_InitChange);
		
		DoMethod(data->grpTitles, OM_REMMEMBER, tcontents);
		DoMethod(data->grpRoot, OM_REMMEMBER, contents);
		
		DoMethod(data->grpTitles, MUIM_Group_ExitChange);
		DoMethod(data->grpRoot, MUIM_Group_ExitChange);
				
		msg->docview = contents;
		msg->title = tcontents;
		
		/* hmm, should we really close the window here? */
		
		if (xget(data->grpTitles, MUIA_Family_ChildCount) == 0)
			DoMethod(_app(data->grpTitles), MUIM_Application_PushMethod, obj, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
		
		return TRUE;
	}
	
	return FALSE;
}

DEFMMETHOD(VPDFWindow_AttachView)
{
	GETDATA;
	int tabind = msg->tabind;
	int newtab;
	Object *contents, *tcontents;
	
	if (tabind == MUIV_VPDFWindow_OpenFile_CurrentTabIfEmpty)
	{
		int tabind = xget(data->grpRoot, MUIA_Group_ActivePage);
		Object *tcontents = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, tabind + 1); // +1 for title object
		if (xget(tcontents, MUIA_UserData) == TRUE)	// marker for empty page. fix
			newtab = FALSE;
		else
			newtab = TRUE;
	}
	else if (tabind == MUIV_VPDFWindow_OpenFile_NewTab)
	{
		newtab = TRUE;
	}
	else // if (MUIV_VPDFWindow_OpenFile_CurrentTab)
	{
		newtab = FALSE;
	}
	
	if (newtab)
		tabind = DoMethod(obj, MUIM_VPDFWindow_CreateTab);
	else
		tabind = xget(data->grpRoot, MUIA_Group_ActivePage);

	/* dispose old title and put whole new object inplace */

	tcontents = (Object*)DoMethod(data->grpTitles, MUIM_Family_GetChild, tabind);
	DoMethod(data->grpTitles, MUIM_Group_InitChange);
	DoMethod(data->grpTitles, OM_REMMEMBER, tcontents);
	MUI_DisposeObject(tcontents);
	DoMethod(data->grpTitles, MUIM_Family_AddTail, msg->title);
	DoMethod(data->grpTitles, MUIM_Group_ExitChange);
	
	/* dispose old contents and add new browser */

	contents = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, tabind + 1); /*  +1 for title object */
	DoMethod(data->grpRoot, MUIM_Group_InitChange);
	DoMethod(data->grpRoot, OM_REMMEMBER, contents);
	MUI_DisposeObject(contents);
	DoMethod(data->grpRoot, MUIM_Family_AddTail, msg->docview);
	DoMethod(data->grpRoot, MUIM_Group_ExitChange);
	
	DoMethod(obj, MUIM_VPDFWindow_UpdateActive, tabind);
	return TRUE;
}

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_VPDFWindow_ID:
			*(ULONG*)msg->opg_Storage = data->id;
			return TRUE;
			
		case MUIA_VPDFWindow_PDFDocument:
		{
			void *doc = NULL;
			Object *group = (Object*)DoMethod(data->grpRoot, MUIM_Family_GetChild, xget(data->grpRoot, MUIA_Group_ActivePage) + 1); /* +1 for title object */
			group = (Object*)DoMethod(group, MUIM_Family_GetChild, 0);

			if (group != NULL)
				doc = xget(group, MUIA_DocumentView_PDFDocument);

			*(ULONG*)msg->opg_Storage = (ULONG)doc; 
			return TRUE;
		}
	}

	return DOSUPER;
}

BEGINMTABLE
	DECNEW
	DECGET
	DECMMETHOD(VPDFWindow_CreateTab)
	DECMMETHOD(VPDFWindow_OpenFile)
	DECMMETHOD(VPDFWindow_UpdateActive)
	DECMMETHOD(VPDFWindow_DetachView)
	DECMMETHOD(VPDFWindow_AttachView)

ENDMTABLE

DECSUBCLASS_NC(MUIC_Window, VPDFWindowClass)

struct VPDFTitleButton_Data
{
	Object *obj;
};

BEGINMTABLE2(VPDFTitleButtonClass)
ENDMTABLE
	
DECSUBCLASS2_NC(MUIC_Group, VPDFTitleButtonClass)

