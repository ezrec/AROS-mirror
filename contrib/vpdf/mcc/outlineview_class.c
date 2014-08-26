
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
#include <mui/Listtree_mcc.h>

#include "util.h"
#include "poppler.h"
#include "pageview_class.h"
#include "outlineview_class.h"

struct Data
{
	void *doc;
	struct MinList *outlineitems;
};

static void buildlist(Object *obj, struct MinList *outlineitems, APTR parentnode)
{
	struct MinNode *n;
	ITERATELIST(n, outlineitems)
	{
		APTR newparentnode = (APTR)DoMethod(obj, MUIM_Listtree_Insert,
				"foo",
				n,
				parentnode,
				MUIV_Listtree_Insert_PrevNode_Tail,
				outlineHasChildren(n) ? TNF_LIST : 0
			);

		if (outlineHasChildren(n))
			buildlist(obj, outlineGetChildren(n), newparentnode);
	}
}

MUI_HOOK(outline_displayfunc, STRPTR *array, struct MUIS_Listtree_TreeNode *tn)
{
	if (tn != NULL)
	{
		struct MinNode *td = (struct MinNode*)tn->tn_User;
		char *name = outlineGetTitle(td);

		array[ 0 ] = name != NULL ? name : "---";

		if ((ULONG)array[ -1 ] % 2)
			array[-9] = (STRPTR)10;
	}
	else
	{
		array[ 0 ] = "";
	}

	return (0);
}

DEFNEW
{
	obj = DoSuperNew(cl, obj,
			MUIA_Listtree_DisplayHook, &outline_displayfunc_hook,
			MUIA_Frame, MUIV_Frame_InputList,
			TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		
		memset(data, 0, sizeof(struct Data));
		data->doc = (void*)GetTagData(MUIA_OutlineView_Document, NULL, INITTAGS);

		if (data->doc != NULL)
		{
			data->outlineitems = pdfGetOutlines(data->doc);
			buildlist(obj, data->outlineitems, MUIV_Listtree_Insert_ListNode_Root);
		}
		else
		{
		}

		/* notifications */

		DoMethod(obj, MUIM_Notify, MUIA_Listtree_Active, MUIV_EveryTime, obj, 2, MUIM_OutlineView_NewEntrySelected, MUIV_TriggerValue);

	}

	return (ULONG)obj;
}

DEFDISP
{
	return DOSUPER;
}

METHOD outlineviewNewEntrySelected(struct IClass *cl, Object *obj, struct MUIP_OutlineView_NewEntrySelected *msg)
{
	GETDATA;

	struct MUIS_Listtree_TreeNode *tn = msg->entry;
	struct MinNode *td = (struct MinNode*)tn->tn_User;
	int page = msg->entry > 0 ? outlineGetPage(td) : 0;

	if (page != 0)
		set(obj, MUIA_OutlineView_Page, page);

	return 0;
}

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_OutlineView_Page:
			return TRUE;

		case MUIA_OutlineView_IsEmpty:
			*(ULONG*)msg->opg_Storage = data->outlineitems == NULL || ISLISTEMPTY(data->outlineitems);
			return TRUE;

	}

	return(DOSUPER);
}

BEGINMTABLE
	DECNEW
	DECDISP
	DECGET
	case MUIM_OutlineView_NewEntrySelected: return outlineviewNewEntrySelected(cl,obj,(APTR)msg);
ENDMTABLE

DECSUBCLASS_NC(MUIC_Listtree, OutlineViewClass)

