
#include <devices/rawkeycodes.h>
#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/muimaster.h>
#include <private/vapor/vapor.h>

#include "title_class.h"
#include "util.h"
#include "window.h"

struct Data
{
	struct MUI_EventHandlerNode ehnode;
};

DEFMMETHOD(Title_Close)
{
	DOSUPER;

	if (xget(obj, MUIA_Family_ChildCount) == 0)
		set(_win(obj), MUIA_Window_CloseRequest, TRUE);

	return 0;
}


DEFMMETHOD(Hide)
{
	GETDATA;
	DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);
	return DOSUPER;
}


DEFMMETHOD(Show)
{
	ULONG rc = DOSUPER;

	if (rc)
	{
		GETDATA;

		data->ehnode.ehn_Object = obj;
		data->ehnode.ehn_Class  = cl;
		data->ehnode.ehn_Priority = -10;
		data->ehnode.ehn_Flags  = MUI_EHF_GUIMODE;
		data->ehnode.ehn_Events = IDCMP_RAWKEY;

		DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);
	}

	return rc;
}


DEFMMETHOD(HandleEvent)
{
	struct IntuiMessage *imsg = msg->imsg;
	ULONG rc = 0;

	if (imsg)
	{
		APTR parent;
		ULONG qual;

		parent = (APTR)xget(obj, MUIA_Parent);
		qual = imsg->Qualifier;

		switch (imsg->Class)
		{
			case IDCMP_RAWKEY:
				switch (imsg->Code)
				{
					case RAWKEY_PAGEUP:
					case RAWKEY_PAGEDOWN:
						if (qual & (IEQUALIFIER_CONTROL|IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND))
						{
							set(parent, MUIA_Group_ActivePage, imsg->Code == RAWKEY_PAGEUP ? MUIV_Group_ActivePage_Prev : MUIV_Group_ActivePage_Next);
							rc = MUI_EventHandlerRC_Eat;
						}
						break;

					default:
						{
							struct InputEvent ie;
							TEXT buffer[4];

							ie.ie_Class        = IECLASS_RAWKEY;
							ie.ie_SubClass     = 0;
							ie.ie_Code         = imsg->Code;
							ie.ie_Qualifier    = 0;
							ie.ie_EventAddress = NULL;

							if (MapRawKey(&ie, buffer, 4, NULL) == 1)
							{
								LONG page = buffer[0] - '0';

								if (page >= 0 && page <= 9)
								{
									page--;

									if (page == -1)
										page += 10;

									if (qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
										page += 10;

									if (qual & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
										page += 10;

									if (qual & IEQUALIFIER_CONTROL)
										page += 10;

									set(parent, MUIA_Group_ActivePage, page);
									rc = MUI_EventHandlerRC_Eat;
								}
							}
						}
						break;
				}
				break;
		}
	}

	return rc;
}

DEFMMETHOD(DragDrop)
{
	GETDATA;

	/* detach document from source window and attach to ours */
		
	Object *oldwindow = _win(msg->obj);
	Object *newwindow = _win(obj);
						
	
	{
		struct MUIP_VPDFWindow_DetachView msg;
		msg.MethodID = MUIM_VPDFWindow_DetachView;
		msg.tabind = MUIV_VPDFWindow_DetachView_Active;
		
		if (DoMethodA(oldwindow, &msg))
		{
			
			DoMethod(newwindow, MUIM_VPDFWindow_AttachView, MUIV_VPDFWindow_AttachView_CurrentTabIfEmpty, msg.docview, msg.title);
		}
	}

	return DOSUPER;
}


DEFMMETHOD(DragQuery)
{
	GETDATA;
	Object *source = msg->obj != NULL ? _parent(msg->obj) : NULL;
	int v;
	return (source != obj && source != NULL && get(source, MUIA_VPDFTitle_IsTitle, &v)) ? MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse;
}

DEFGET
{
	GETDATA;

	switch(msg->opg_AttrID)
	{
		case MUIA_VPDFTitle_IsTitle:
			*(ULONG*)msg->opg_Storage = TRUE;
			return TRUE;
	};

	return DOSUPER;
}


BEGINMTABLE
DECGET
DECMMETHOD(HandleEvent)
DECMMETHOD(Hide)
DECMMETHOD(Show)
DECMMETHOD(DragDrop)
DECMMETHOD(DragQuery)
DECMMETHOD(Title_Close)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Title, TitleClass)
