/*
*/

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <exec/libraries.h>
#include <graphics/gfx.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <graphics/rpattr.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <constructor.h>
#include <intuition/pointerclass.h>

#define	 SYSTEM_PRIVATE
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#undef	 SYSTEM_PRIVATE

#include <private/vapor/vapor.h>
#include "pageview_class.h"
#include "documentview_class.h"
#include "search_class.h"

#include "system/chunky.h"
#include "system/memory.h"
#include "system/functions.h"

#include "util.h"
#include "poppler.h"

struct Data
{
	Object *objDocumentView;
	Object *strPhrase;
	Object *btnNext;
	Object *btnPrev;
	Object *txtInfo;
	int prevpage;
};


/*
 * MUI Standard methods
 */

///	dispose

DEFDISP
{
	GETDATA;
	return(DOSUPER);
}

////

///	new

DEFNEW
{
	Object *strPhrase, *btnNext, *btnPrev, *txtInfo;

	obj = DoSuperNew(cl, obj,
						MUIA_Group_Horiz, TRUE,
						Child, strPhrase = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_Textinput_RemainActive, TRUE,
							MUIA_Weight, 500,
							End,
						Child, btnPrev = SimpleButton("_Prev"),
						Child, btnNext = SimpleButton("_Next"),
						Child, txtInfo = TextObject,
							MUIA_Text_Contents, "",
							End,
						TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;

		memset(data, 0, sizeof(struct Data));

		data->strPhrase = strPhrase;
		data->btnPrev = btnPrev;
		data->btnNext = btnNext;
		data->txtInfo = txtInfo;

		DoMethod(strPhrase, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, obj, 3, MUIM_Search_Search, NULL, 1);
		DoMethod(btnNext, MUIM_Notify, MUIA_Selected, FALSE, obj, 3, MUIM_Search_Search, NULL, 1);
		DoMethod(btnPrev, MUIM_Notify, MUIA_Selected, FALSE, obj, 3, MUIM_Search_Search, NULL, -1);
	}

	return (ULONG)obj;

}

////

///	set

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{
		case MUIA_Search_DocumentView:
			data->objDocumentView = (Object*)tag->ti_Data;
			break;
	}
	NEXTTAG;

	return(DOSUPER);
}

////

///	get

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		//case MUIA_PageView_Width:
		//	  *(msg->opg_Storage) = (ULONG)(data->image ? data->image->width : 0);
		//	  return TRUE;

	}

	return(DOSUPER);
}

////

DEFMMETHOD(Search_Search)
{
	GETDATA;
	char *phrase = msg->phrase;
	void *doc = (void*)xget(data->objDocumentView, MUIA_DocumentView_PDFDocument);
	int page = xget(data->objDocumentView, MUIA_DocumentView_Page);
	double x1, y1, x2, y2;
	float v[4];

	if (phrase == NULL)
		phrase = (char*)xget(data->strPhrase, MUIA_String_Contents);

	if (*phrase == '\0')
		return FALSE;

	if (pdfSearch(doc, &page, phrase, msg->direction, &x1, &y1, &x2, &y2))
	{
		Object *pageview = DoMethod(data->objDocumentView, MUIM_DocumentView_FindViewForPage, page);

		v[0] = x1;
		v[1] = y1;
		v[2] = x2;
		v[3] = y2;

		if (data->prevpage != page && data->prevpage != 0)
		{
			Object *pageview = DoMethod(data->objDocumentView, MUIM_DocumentView_FindViewForPage, data->prevpage);
			DoMethod(pageview, MUIM_PageView_RemoveMarker, MUIV_PageView_RemoveMarker_All);
		}
		else
		{
			DoMethod(pageview, MUIM_PageView_RemoveMarker, MUIV_PageView_RemoveMarker_All);
		}

		set(data->objDocumentView, MUIA_DocumentView_Page, page);
		DoMethod(pageview, MUIM_PageView_AddMarker, MUIV_PageView_AddMarker_New, v, 0xc0c0c0c0);
		DoMethod(pageview, MUIM_PageView_Update, -1, -1, -1, -1);
		data->prevpage = page;
	}

	return 0;
}

BEGINMTABLE
	DECSET
	DECGET
	DECNEW
	DECDISP
	DECMMETHOD(Search_Search)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, SearchClass)


