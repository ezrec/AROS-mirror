/*
**	UploadPanel.c
**
**	ZModem auto-upload control panel support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

LONG
UploadPanel(LONG SendMode)
{
	LayoutHandle *Handle;
	LONG Selection;
	BOOL FromQueue;

	Selection = UPLOAD_IGNORE;

	if(SendMode)
		FromQueue = (BOOL)(GenericListCount(GenericListTable[GLIST_UPLOAD]) > 0);
	else
		FromQueue = FALSE;

	if(Handle = LT_CreateHandleTags(Window->WScreen,
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
				LA_LabelID,	MSG_UPLOADPANEL_SELECT_TRANSFER_TYPE_GAD,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	BUTTON_KIND,
					LA_LabelID,	SendMode ? MSG_UPLOADPANEL_TEXT_UPLOAD_GAD : MSG_UPLOADPANEL_TEXT_DOWNLOAD_GAD,
					LA_ID,		UPLOAD_TEXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		SendMode ? MSG_UPLOADPANEL_BINARY_UPLOAD_GAD : MSG_UPLOADPANEL_BINARY_DOWNLOAD_GAD,
					LA_ID,			UPLOAD_BINARY,
					LABT_ReturnKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			if(SendMode)
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_UPLOADPANEL_FROM_UPLOAD_QUEUE_TXT,
						LA_BOOL,		&FromQueue,
						LABT_ReturnKey,	TRUE,
						LA_ID,			UPLOAD_DUMMY,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADPANEL_IGNORE_GAD,
					LA_ID,			UPLOAD_IGNORE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_ABORT_GAD,
					LA_ID,			UPLOAD_ABORT,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_UPLOADPANEL_TRANSFER_TYPE_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			GuideContext(CONTEXT_TRANSFER);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
				{
					Selection = UPLOAD_ABORT;

					break;
				}

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case UPLOAD_TEXT:

								if(FromQueue && !GenericListCount(GenericListTable[GLIST_UPLOAD]))
									FromQueue = FALSE;

								if(FromQueue)
									Selection = UPLOAD_TEXT_FROM_LIST;
								else
									Selection = UPLOAD_TEXT;

								Done = TRUE;

								break;

							case UPLOAD_BINARY:

								if(FromQueue && !GenericListCount(GenericListTable[GLIST_UPLOAD]))
									FromQueue = FALSE;

								if(FromQueue)
									Selection = UPLOAD_BINARY_FROM_LIST;
								else
									Selection = UPLOAD_BINARY;

								Done = TRUE;

								break;

							default:

								if(MsgGadget->GadgetID != UPLOAD_DUMMY)
								{
									Selection = MsgGadget->GadgetID;

									Done = TRUE;
								}

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

	return(Selection);
}
