/*
**	ImportPanel.c
**
**	Editing panel for phone unit settings
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST=1,GAD_REPLACE,GAD_APPEND,GAD_CANCEL };

BOOL
ImportPanel(struct Window *Parent,PhonebookHandle *PhoneHandle,PhoneEntry *Entry,struct List *TimeDateList)
{
	BOOL			 MadeChanges = FALSE;
	struct List		*PhoneList;
	LayoutHandle	*Handle;

	if(!(PhoneList = CreateRegularPhoneList(PhoneHandle)))
	{
		DisplayBeep(Parent->WScreen);
		return(FALSE);
	}

	if(!TimeDateList)
		TimeDateList = (struct List *)&Entry->TimeDateList;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LONG Index,MaxHeight,NumEntries;
				PhoneNode *Node;

				for(Index = -1, NumEntries = 0, Node = (PhoneNode *)PhoneList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ, NumEntries++)
				{
					if(Node->Entry == Entry)
						Index = NumEntries;
				}

				if(NumEntries < 10)
					MaxHeight = 10;
				else
				{
					if(NumEntries > 20)
						MaxHeight = 20;
					else
						MaxHeight = NumEntries;
				}

				LT_New(Handle,
					LA_Type,		LISTVIEW_KIND,
					LA_LabelID,		MSG_V36_0088,
					LA_ID,			GAD_LIST,
					LA_Chars,		50,
					LALV_Lines,		10,
					LALV_Link,		NIL_LINK,
					LALV_CursorKey,	TRUE,
					LALV_MaxGrowY,	MaxHeight,
					LALV_ResizeY,	TRUE,
					GTLV_Selected,	Index,
					GTLV_Labels,	PhoneList,
					GTLV_MaxPen,	GetListMaxPen(Handle->DrawInfo->dri_Pens),
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_IMPORTPANEL_REPLACE_RATES_GAD,
					LA_ID,			GAD_REPLACE,
					GA_Disabled,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_IMPORTPANEL_APPEND_RATES_GAD,
					LA_ID,			GAD_APPEND,
					GA_Disabled,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_IMPORTPANEL_IMPORT_RATES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			BOOL				 Done = FALSE;
			ULONG				 MsgClass;
			UWORD				 MsgCode;
			struct Gadget		*MsgGadget;
			PhoneNode			*Node = NULL;

			GuideContext(CONTEXT_IMPORT);

			LT_ShowWindow(Handle,TRUE);

			PushWindow(PanelWindow);

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgCode		= Message->Code;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_LIST:

								Node = (PhoneNode *)GetListNode(MsgCode,PhoneList);

								LT_SetAttributes(Handle,GAD_REPLACE,
									GA_Disabled,	Node->Entry == Entry,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_APPEND,
									GA_Disabled,	Node->Entry == Entry,
								TAG_DONE);

								break;

							case GAD_REPLACE:

								FreeTimeDateList(TimeDateList);

								/* Falls through to... */

							case GAD_APPEND:

								CopyTimeDateList((struct List *)&Node->Entry->TimeDateList,TimeDateList,TRUE);

								MadeChanges = TRUE;

								/* Falls through to... */

							case GAD_CANCEL:

								Done = TRUE;
								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	DeleteList(PhoneList);

	return(MadeChanges);
}
