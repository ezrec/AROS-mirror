/*
**	MiscPanel.c
**
**	Editing panel for miscellaneous parts of the configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_USE=1000,GAD_CANCEL,
			GAD_PAGER,GAD_PAGEGROUP,
			GAD_REQUESTERMODE,GAD_SET_REQUESTERSIZE,
			GAD_WAITSTRING,GAD_CONSOLE_OUTPUT_WINDOW
		};

STATIC STRPTR SAVE_DS STACKARGS
MiscShowSeconds(struct Gadget *UnusedGadget,LONG Level)
{
	STATIC UBYTE Buffer[15];

	LimitedSPrintf(sizeof(Buffer),Buffer,LocaleString(MSG_GLOBAL_SHOW_MINUTES_TXT),Level / 60,Level % 60);

	return(Buffer);
}

STATIC LONG SAVE_DS STACKARGS
ShowSize(struct Gadget *UnusedGadget,LONG Level)
{
	return(1L << Level);
}

BOOL
MiscPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[] =
	{
		MSG_V36_0107,
		MSG_V36_0108,
		MSG_USER_INTERFACE_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	LayoutHandle *Handle;
	BOOL UseIt;

	UseIt = FALSE;

	SaveConfig(LocalConfig,PrivateConfig);

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,	FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		LONG BufferSize,i;

		for(i = 0 ; i < 32 ; i++)
		{
			if((1L << i) == PrivateConfig->MiscConfig->IOBufferSize)
			{
				BufferSize = i;

				break;
			}
		}

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		TAB_KIND,
					LA_ID,			GAD_PAGER,
					LA_LabelID,		MSG_XFER_PAGE_TXT,
					LACY_LabelTable,PageTitleID,
					LACY_AutoPageID,GAD_PAGEGROUP,
					LACY_TabKey,	TRUE,
					LA_BYTE,		&InitialPage,
					LATB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_ID,			GAD_PAGEGROUP,
				LAGR_ActivePage,InitialPage,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_MISCPANEL_BACKUP_CONFIGURATION_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->BackupConfig,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_MISCPANEL_SHOW_FAST_MACROS_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->OpenFastMacroPanel,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_MISCPANEL_RELEASE_SERIAL_DEVICE_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->ReleaseDevice,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_MISCPANEL_SIMPLE_IO_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->SimpleIO,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_OVERWRITE_WARNING_TXT,
						LA_BYTE,	&PrivateConfig->MiscConfig->ProtectiveMode,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_MISCPANEL_CREATE_ICONS_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->CreateIcons,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_WAITSTRING,
						LA_LabelID,		MSG_WAIT_STRING,
						LA_STRPTR,		PrivateConfig->MiscConfig->WaitString,
						LA_Chars,		20,
						GTST_MaxChars,	sizeof(PrivateConfig->MiscConfig->WaitString) - 1,
					TAG_DONE);

					if(PrivateConfig->MiscConfig->WaitDelay < 1)
						PrivateConfig->MiscConfig->WaitDelay = 1;

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_WAIT_DELAY_GAD,
						LA_LONG,			&PrivateConfig->MiscConfig->WaitDelay,
						GTSL_Min,			1,
						GTSL_Max,			60 * 60,
						GTSL_DispFunc,		MiscShowSeconds,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MISCPANEL_PROGRAM_PRIORITY_GAD,
						GTSL_Min,			-128,
						GTSL_Max,			 127,
						GTSL_LevelFormat,	"%4ld",
						LA_BYTE,			&PrivateConfig->MiscConfig->Priority,
						LA_Chars,			12,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_V36_1810,
						LA_LONG,			&BufferSize,
						GTSL_Min,			12,
						GTSL_Max,			20,
						GTSL_DispFunc,		ShowSize,
						GTSL_LevelFormat,	LocaleBase ? "%7lD" : "%7ld",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_CONSOLE_OUTPUT_WINDOW,
						LA_LabelID,		MSG_CONSOLE_OUTPUT_WINDOW_TXT,
						LA_STRPTR,		PrivateConfig->MiscConfig->WindowName,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(PrivateConfig->MiscConfig->WindowName) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_SUPPRESS_OUTPUT_GAD,
						LA_BYTE,	&PrivateConfig->MiscConfig->SuppressOutput,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_ALERT_GAD,
						LA_BYTE,		&PrivateConfig->MiscConfig->AlertMode,
						LACY_FirstLabel,MSG_TERMINALPANEL_ALERT_NONE_TXT,
						LACY_LastLabel,	MSG_TERMINALPANEL_ALERT_SIGNAL_SCREEN_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_REQUESTER_DIMENSIONS_TXT,
						LA_BYTE,		&PrivateConfig->MiscConfig->RequesterMode,
						LACY_FirstLabel,MSG_LABEL_CENTRE_TXT,
						LACY_LastLabel,	MSG_LABEL_IGNORE_TXT,
						LA_ID,			GAD_REQUESTERMODE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_REQUESTER_EDIT_SIZE_TXT,
						LA_ID,			GAD_SET_REQUESTERSIZE,
						GA_Disabled,	PrivateConfig->MiscConfig->RequesterMode == REQUESTERMODE_IGNORE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

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
			LAWN_TitleID,		MSG_MISCPANEL_MISCELLANEOUS_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct FileRequester *FileRequester;
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			BOOL JustCreated;
			ULONG MsgClass;
			BOOL Done;

			FileRequester = NULL;

			GuideContext(CONTEXT_MISC);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			JustCreated = FALSE;
			Done = FALSE;

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
							case GAD_REQUESTERMODE:

								LT_SetAttributes(Handle,GAD_SET_REQUESTERSIZE,
									GA_Disabled,PrivateConfig->MiscConfig->RequesterMode == REQUESTERMODE_IGNORE,
								TAG_DONE);

								break;

							case GAD_SET_REQUESTERSIZE:

								LT_LockWindow(PanelWindow);

								if(!FileRequester)
								{
									if(FileRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
										ASLFR_Window,		PanelWindow,
										ASLFR_TitleText,	LocaleString(MSG_MOVE_RESIZE_THIS_WINDOW_TXT),
										ASLFR_PositiveText,	LocaleString(MSG_GLOBAL_USE_GAD),
										ASLFR_Flags1,		FILF_NEWIDCMP,
									TAG_DONE))
										JustCreated = TRUE;
								}

								if(FileRequester)
								{
									BOOL Result;

									if(JustCreated && (!PrivateConfig->MiscConfig->RequesterWidth || !PrivateConfig->MiscConfig->RequesterHeight))
										JustCreated = FALSE;

									Result = AslRequestTags(FileRequester,
										ASLFR_InitialLeftEdge,				Window->LeftEdge + PrivateConfig->MiscConfig->RequesterLeft,
										ASLFR_InitialTopEdge,				Window->TopEdge + PrivateConfig->MiscConfig->RequesterTop,
										JustCreated ? ASLFR_InitialWidth : TAG_IGNORE,	PrivateConfig->MiscConfig->RequesterWidth,
										JustCreated ? ASLFR_InitialHeight : TAG_IGNORE,	PrivateConfig->MiscConfig->RequesterHeight,
									TAG_DONE);

									if(!Result && IoErr())
										Result = AslRequest(FileRequester,NULL);

									if(Result)
									{
										PrivateConfig->MiscConfig->RequesterLeft	= FileRequester->fr_LeftEdge - Window->LeftEdge;
										PrivateConfig->MiscConfig->RequesterTop		= FileRequester->fr_TopEdge - Window->TopEdge;
										PrivateConfig->MiscConfig->RequesterWidth	= FileRequester->fr_Width;
										PrivateConfig->MiscConfig->RequesterHeight	= FileRequester->fr_Height;
									}

									JustCreated = FALSE;
								}

								LT_UnlockWindow(PanelWindow);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								PrivateConfig->MiscConfig->IOBufferSize = 1L << BufferSize;

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

			if(FileRequester)
				FreeAslRequest(FileRequester);

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
