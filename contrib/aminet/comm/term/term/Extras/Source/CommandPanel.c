/*
**	CommandPanel.c
**
**	Editing panel for command sequences
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_USE=1000,GAD_CANCEL };

BOOL
CommandPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC WORD LabelTable[] =
	{
		MSG_COMMANDPANEL_STARTUP_LOGIN_MACRO_GAD,
		MSG_COMMANDPANEL_LOGIN_MACRO_GAD,
		MSG_COMMANDPANEL_LOGOFF_MACRO_GAD,
		MSG_COMMANDPANEL_UPLOAD_MACRO_GAD,
		MSG_COMMANDPANEL_DOWNLOAD_MACRO_GAD
	};

	LONG i,Max,Len;
	LayoutHandle *Handle;
	BOOL UseIt = FALSE;

	SaveConfig(LocalConfig,PrivateConfig);

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		for(i = Max = 0 ; i < NUM_ELEMENTS(LabelTable) ; i++)
		{
			if((Len = LT_LabelChars(Handle,LocaleString(LabelTable[i]))) > Max)
				Max = Len;
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_V36_0026,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_COMMANDPANEL_STARTUP_LOGIN_MACRO_GAD,
					LA_STRPTR,		PrivateConfig->CommandConfig->StartupMacro,
					LA_Chars,		30,
					LA_LabelChars,	Max,
					GTST_MaxChars,	sizeof(PrivateConfig->CommandConfig->StartupMacro) - 1,
				TAG_DONE);

				LT_New(Handle,LA_Type,XBAR_KIND,TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_COMMANDPANEL_LOGIN_MACRO_GAD,
					LA_STRPTR,		PrivateConfig->CommandConfig->LoginMacro,
					LA_LabelChars,	Max,
					GTST_MaxChars,	sizeof(PrivateConfig->CommandConfig->LoginMacro) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_COMMANDPANEL_LOGOFF_MACRO_GAD,
					LA_STRPTR,		PrivateConfig->CommandConfig->LogoffMacro,
					LA_LabelChars,	Max,
					GTST_MaxChars,	sizeof(PrivateConfig->CommandConfig->LogoffMacro) - 1,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,				VERTICAL_KIND,
				LA_LabelID,				MSG_V36_0027,
				LAGR_LastAttributes,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_COMMANDPANEL_UPLOAD_MACRO_GAD,
					LA_STRPTR,		PrivateConfig->CommandConfig->UploadMacro,
					LA_LabelChars,	Max,
					GTST_MaxChars,	sizeof(PrivateConfig->CommandConfig->UploadMacro) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_COMMANDPANEL_DOWNLOAD_MACRO_GAD,
					LA_STRPTR,		PrivateConfig->CommandConfig->DownloadMacro,
					LA_LabelChars,	Max,
					GTST_MaxChars,	sizeof(PrivateConfig->CommandConfig->DownloadMacro) - 1,
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
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LABT_ReturnKey,	TRUE,
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
			LAWN_TitleID,		MSG_COMMANDPANEL_COMMAND_PREFERENCES_GAD,
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
			struct Gadget		*MsgGadget;

			GuideContext(CONTEXT_COMMAND);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_USE:

								LT_UpdateStrings(Handle);

								UseIt = Done = TRUE;
								break;

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

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}
