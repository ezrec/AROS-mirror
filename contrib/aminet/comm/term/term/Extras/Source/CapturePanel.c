/*
**	CapturePanel.c
**
**	Editing panel for log file, buffer and capture configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LOGACTIONS, GAD_LOGFILE, GAD_SELECT_LOGFILE, GAD_LOGCALLS, GAD_CALL_LOGFILE,GAD_SELECT_CALL_LOGFILE,
			GAD_MAX_BUFFER_SIZE,GAD_BUFFER_SAFETY_MEMORY,GAD_BUFFER_ENABLED,GAD_BUFFER_PATH,GAD_SELECT_BUFFER_PATH,
			GAD_CONNECT_AUTO_CAPTURE, GAD_AUTOCAPTURE_DATE, GAD_CAPTURE_FILTER, GAD_CONVERT_CHARS,
			GAD_CAPTURE_PATH, GAD_SELECT_CAPTURE_PATH, GAD_OPEN_BUFFER_WINDOW, GAD_BUFFER_MODE,
			GAD_REMEMBER_WINDOW_POS,GAD_OPEN_BUFFER_SCREEN,GAD_REMEMBER_SCREEN_POS,GAD_SEARCH_HISTORY,
			GAD_SCREENMODE,GAD_BUFFERSCREEN_POSITION,GAD_BUFFERWIDTH,GAD_USE, GAD_CANCEL,
			GAD_PAGER, GAD_PAGEGROUP, GAD_CALL_FILE_FORMAT
		};

BOOL
CapturePanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[] =
	{
		MSG_V36_0008,
		MSG_V36_0011,
		MSG_V36_0009,
		MSG_V36_0014,
		MSG_V36_0013,
		-1
	};

	STATIC LONG FileFormatID[] =
	{
		MSG_CALL_FILE_FORMAT_1_TXT,
		MSG_CALL_FILE_FORMAT_2_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	LayoutHandle *Handle;
	BOOL UseIt;

	UseIt = FALSE;

	SaveConfig(LocalConfig,PrivateConfig);

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		BYTE OpenBufferWindow,OpenBufferScreen;
		UBYTE ModeName[DISPLAYNAMELEN + 1];
		struct Window *PanelWindow;
		ULONG DisplayMode;

		OpenBufferScreen = PrivateConfig->CaptureConfig->OpenBufferScreen;
		OpenBufferWindow = PrivateConfig->CaptureConfig->OpenBufferWindow;

		if(OpenBufferWindow > BUFFER_END)
			OpenBufferWindow = BUFFER_END;
		else
		{
			if(OpenBufferWindow < BUFFER_TOP)
				OpenBufferWindow = BUFFER_TOP;
		}

		if(OpenBufferScreen > BUFFER_END)
			OpenBufferScreen = BUFFER_END;
		else
		{
			if(OpenBufferScreen < BUFFER_TOP)
				OpenBufferScreen = BUFFER_TOP;
		}

		DisplayMode = PrivateConfig->CaptureConfig->BufferScreenMode;

		if(ModeNotAvailable(DisplayMode))
		{
			if(PrivateConfig && PrivateConfig->ScreenConfig)
				DisplayMode = PrivateConfig->ScreenConfig->DisplayMode;
			else
				DisplayMode = INVALID_ID;
		}

		if(ModeNotAvailable(DisplayMode))
		{
			struct Screen *PubScreen;

			if(PubScreen = LockPubScreen(NULL))
			{
				DisplayMode = GetVPModeID(&PubScreen->ViewPort);

				UnlockPubScreen(NULL,PubScreen);
			}
			else
				DisplayMode = DEFAULT_MONITOR_ID | HIRES_KEY;
		}

		GetModeName(DisplayMode,ModeName,sizeof(ModeName));

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
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
						LA_LabelID,		MSG_CAPTUREPANEL_LOG_ACTIONS_GAD,
						LA_Type,		CHECKBOX_KIND,
						LA_ID,			GAD_LOGACTIONS,
						LA_BYTE,		&PrivateConfig->CaptureConfig->LogActions,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,		MSG_CAPTUREPANEL_LOGFILE_GAD,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_LOGFILE,
						LA_Chars,		20,
						LA_STRPTR,		PrivateConfig->CaptureConfig->LogFileName,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->CaptureConfig->LogFileName) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,		MSG_CAPTUREPANEL_LOG_CALLS_GAD,
						LA_Type,		CHECKBOX_KIND,
						LA_ID,			GAD_LOGCALLS,
						LA_BYTE,		&PrivateConfig->CaptureConfig->LogCall,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,		MSG_CAPTUREPANEL_CALL_LOGFILE_GAD,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_CALL_LOGFILE,
						LA_STRPTR,		PrivateConfig->CaptureConfig->CallLogFileName,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->CaptureConfig->CallLogFileName) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,		MSG_CALL_FILE_FORMAT_GAD,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_CALL_FILE_FORMAT,
						LACY_LabelTable,FileFormatID,
						LA_BYTE,		&PrivateConfig->CaptureConfig->LogFileFormat,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_LabelID,		MSG_CAPTUREPANEL_CONNECT_AUTO_CAPTURE_GAD,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_CONNECT_AUTO_CAPTURE,
							LA_BYTE,		&PrivateConfig->CaptureConfig->ConnectAutoCapture,
						TAG_DONE);

						LT_New(Handle,
							LA_LabelID,		MSG_CAPTUREPANEL_CAPTURE_FILTER_ENABLED_GAD,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_CAPTURE_FILTER,
							LA_BYTE,		&PrivateConfig->CaptureConfig->CaptureFilterMode,
						TAG_DONE);

						LT_New(Handle,
							LA_LabelID,		MSG_CONVERT_CHARACTERS_GAD,
							LA_Type,		CHECKBOX_KIND,
							LA_BYTE,		&PrivateConfig->CaptureConfig->ConvertChars,
							GA_Disabled,	!PrivateConfig->CaptureConfig->CaptureFilterMode,
							LA_ID,			GAD_CONVERT_CHARS,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_LabelID,		MSG_CAPTUREPANEL_CREATION_DATE_GAD,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_AUTOCAPTURE_DATE,
							LA_BYTE,		&PrivateConfig->CaptureConfig->AutoCaptureDate,
							LACY_FirstLabel,MSG_CAPTUREPANEL_APPEND_DATE_TXT,
							LACY_LastLabel,	MSG_CAPTUREPANEL_WRITE_DATE_TO_FILE_TXT,
						TAG_DONE);

						LT_New(Handle,
							LA_LabelID,		MSG_CAPTUREPANEL_CAPTURE_PATH_GAD,
							LA_Type,		STRING_KIND,
							LA_ID,			GAD_CAPTURE_PATH,
							LA_Chars,		20,
							LA_STRPTR,		PrivateConfig->CaptureConfig->CapturePath,
							LAST_Picker,	TRUE,
							GTST_MaxChars,	sizeof(PrivateConfig->CaptureConfig->CapturePath) - 1,
							LA_ExtraSpace,	TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_BUFFER_ENABLED_GAD,
						LA_Type,				CHECKBOX_KIND,
						LA_ID,					GAD_BUFFER_ENABLED,
						LA_BYTE,				&PrivateConfig->CaptureConfig->BufferEnabled,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_MAX_BUFFER_SIZE_GAD,
						LA_Type,				INTEGER_KIND,
						LA_ID,					GAD_MAX_BUFFER_SIZE,
						LA_LONG,				&PrivateConfig->CaptureConfig->MaxBufferSize,
						LAIN_UseIncrementers,	TRUE,
						LAIN_Min,				0,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_BUFFER_SAFETY_MEMORY_GAD,
						LA_Type,				INTEGER_KIND,
						LA_ID,					GAD_BUFFER_SAFETY_MEMORY,
						LA_LONG,				&PrivateConfig->CaptureConfig->BufferSafetyMemory,
						LAIN_UseIncrementers,	TRUE,
						LAIN_Min,				0,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_BUFFER_PATH_GAD,
						LA_Type,				STRING_KIND,
						LA_ID,					GAD_BUFFER_PATH,
						LA_STRPTR,				PrivateConfig->CaptureConfig->BufferPath,
						LAST_Picker,			TRUE,
						LA_Chars,				20,
						GTST_MaxChars,			sizeof(PrivateConfig->CaptureConfig->BufferPath) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_BUFFER_LINE_WIDTH_GAD,
						LA_Type,				SliderType,
						LA_ID,					GAD_BUFFERWIDTH,
						LA_UBYTE,				&PrivateConfig->CaptureConfig->BufferWidth,
						GTSL_Min,				80,
						GTSL_Max,				BUFFER_LINE_MAX,
						GTSL_LevelFormat,		"%3ld",
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,				MSG_BUFFER_OPERATION_MODE_TXT,
						LA_Type,				CYCLE_KIND,
						LA_ID,					GAD_BUFFER_MODE,
						LA_BYTE,				&PrivateConfig->CaptureConfig->BufferMode,
						LACY_FirstLabel,		MSG_BUFFER_MODE1_TXT,
						LACY_LastLabel,			MSG_BUFFER_MODE2_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				XBAR_KIND,
					TAG_DONE);

					if(PrivateConfig->CaptureConfig->SearchHistory < 1)
						PrivateConfig->CaptureConfig->SearchHistory = 10;

					LT_New(Handle,
						LA_LabelID,				MSG_CAPTUREPANEL_SEARCH_HISTORY_GAD,
						LA_Type,				INTEGER_KIND,
						LA_ID,					GAD_SEARCH_HISTORY,
						LA_Chars,				8,
						LAIN_UseIncrementers,	TRUE,
						LAIN_Min,				1,
						LAIN_Max,				255,
						LA_BYTE,				&PrivateConfig->CaptureConfig->SearchHistory,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_LabelID,			MSG_CAPTUREPANEL_OPEN_BUFFER_SCREEN_GAD,
							LA_Type,			MX_KIND,
							LA_ID,				GAD_OPEN_BUFFER_SCREEN,
							LA_BYTE,			&OpenBufferScreen,
							LAMX_FirstLabel,	MSG_CAPTUREPANEL_TOP_TXT,
							LAMX_LastLabel,		MSG_CAPTUREPANEL_END_TXT,
						TAG_DONE);

						LT_New(Handle,
							LA_LabelID,			MSG_CAPTUREPANEL_REMEMBER_BUFFER_SCREEN_GAD,
							LA_Type,			CHECKBOX_KIND,
							LA_ID,				GAD_REMEMBER_SCREEN_POS,
							LA_BYTE,			&PrivateConfig->CaptureConfig->RememberBufferScreen,
							LA_ExtraSpace,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_LabelID,			MSG_CAPTUREPANEL_BUFFER_SCREEN_POSITION_GAD,
							LA_Type,			MX_KIND,
							LA_ID,				GAD_BUFFERSCREEN_POSITION,
							LA_BYTE,			&PrivateConfig->CaptureConfig->BufferScreenPosition,
							LAMX_FirstLabel,	MSG_CAPTUREPANEL_LEFT_TXT,
							LAMX_LastLabel,		MSG_CAPTUREPANEL_RIGHT_TXT,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			XBAR_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		TEXT_KIND,
							LA_LabelID,		MSG_SCREENPANEL_CURRENT_DISPLAY_MODE_GAD,
							LA_Chars,		DISPLAYNAMELEN,
							LA_ID,			GAD_SCREENMODE,
							GTTX_Text,		ModeName,
							GTTX_Border,	TRUE,
							LATX_Picker,	TRUE,
							LATX_LockSize,	TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_LabelID,			MSG_CAPTUREPANEL_OPEN_BUFFER_WINDOW_GAD,
						LA_Type,			MX_KIND,
						LA_ID,				GAD_OPEN_BUFFER_WINDOW,
						LA_BYTE,			&OpenBufferWindow,
						LAMX_FirstLabel,	MSG_CAPTUREPANEL_TOP_TXT,
						LAMX_LastLabel,		MSG_CAPTUREPANEL_END_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_LabelID,			MSG_CAPTUREPANEL_REMEMBER_BUFFER_WINDOW_GAD,
						LA_Type,			CHECKBOX_KIND,
						LA_ID,				GAD_REMEMBER_WINDOW_POS,
						LA_BYTE,			&PrivateConfig->CaptureConfig->RememberBufferWindow,
						LA_ExtraSpace,		TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
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
			LAWN_TitleID,		MSG_CAPTUREPANEL_CAPTURE_SETTINGS_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester *FileRequest;
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			GuideContext(CONTEXT_CAPTURE);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_CAPTURE_FILTER:

								LT_SetAttributes(Handle,GAD_CONVERT_CHARS,
									GA_Disabled,	!PrivateConfig->CaptureConfig->CaptureFilterMode,
								TAG_DONE);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								PrivateConfig->CaptureConfig->OpenBufferWindow = OpenBufferWindow;
								PrivateConfig->CaptureConfig->OpenBufferScreen = OpenBufferScreen;
								PrivateConfig->CaptureConfig->BufferScreenMode = DisplayMode;

								UseIt = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							case GAD_LOGFILE:
							case GAD_CALL_LOGFILE:

								FindPath(PanelWindow,LT_GetString(Handle,MsgGadget->GadgetID),TRUE,NULL);
								break;

							case GAD_BUFFER_PATH:
							case GAD_CAPTURE_PATH:

								FindDrawer(PanelWindow,LT_GetString(Handle,MsgGadget->GadgetID),TRUE,NULL);
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_LOGFILE:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_CAPTUREPANEL_SELECT_LOGFILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									FindPath(PanelWindow,DummyBuffer,TRUE,NULL);

									LT_SetAttributes(Handle,MsgGadget->GadgetID,
										GTST_String,	DummyBuffer,
									TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_CALL_LOGFILE:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_CAPTUREPANEL_SELECT_CALL_LOGFILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
								{
									FindPath(PanelWindow,DummyBuffer,TRUE,NULL);

									LT_SetAttributes(Handle,MsgGadget->GadgetID,
										GTST_String,	DummyBuffer,
									TAG_DONE);

									FreeAslRequest(FileRequest);
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_BUFFER_PATH:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

								if(FileRequest = SaveDrawer(PanelWindow,LocaleString(MSG_CAPTUREPANEL_SELECT_BUFFER_PATH_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									FindDrawer(PanelWindow,DummyBuffer,TRUE,NULL);

									LT_SetAttributes(Handle,MsgGadget->GadgetID,
										GTST_String,	DummyBuffer,
									TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_CAPTURE_PATH:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

								if(FileRequest = SaveDrawer(PanelWindow,LocaleString(MSG_CAPTUREPANEL_SELECT_CAPTURE_PATH_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									FindDrawer(PanelWindow,DummyBuffer,TRUE,NULL);

									LT_SetAttributes(Handle,MsgGadget->GadgetID,
										GTST_String,	DummyBuffer,
									TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_SCREENMODE:

								LT_LockWindow(PanelWindow);

								if(PickDisplayMode(PanelWindow,&DisplayMode,NULL))
								{
									GetModeName(DisplayMode,ModeName,sizeof(ModeName));

									LT_SetAttributes(Handle,GAD_SCREENMODE,
										GTTX_Text,	ModeName,
									TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);

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
