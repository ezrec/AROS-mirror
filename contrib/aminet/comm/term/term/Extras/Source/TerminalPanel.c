/*
**	TerminalPanel.c
**
**	Editing panel for terminal configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_EMULATIONTYPE=1000, GAD_EMULATIONLIBRARY, GAD_COLUMNS, GAD_LINES,
			GAD_KEYMAP, GAD_TEXTFONT, GAD_FONTMODE, GAD_EMULATIONTASK, GAD_IBMFONT,
			GAD_USE, GAD_CANCEL, GAD_PAGER, GAD_PAGEGROUP, GAD_BBS_ANSI_COMPLIANT,
			GAD_AUTO_SIZE
		};

STATIC LONG LocalMaxColumns,LocalMaxLines;

STATIC STRPTR SAVE_DS STACKARGS
ShowColumn(struct Gadget *UnusedGadget,LONG Level)
{
	STATIC UBYTE Buffer[10];

	if(Level == LocalMaxColumns + 1)
		strcpy(Buffer,LocaleString(MSG_TERMINALPANEL_MAXIMUM_TXT));
	else
		LimitedSPrintf(sizeof(Buffer),Buffer,LocaleBase ? "%9lD" : "%9ld",Level);

	return(Buffer);
}

STATIC STRPTR SAVE_DS STACKARGS
ShowLine(struct Gadget *UnusedGadget,LONG Level)
{
	STATIC UBYTE Buffer[10];

	if(Level == LocalMaxLines + 1)
		strcpy(Buffer,LocaleString(MSG_TERMINALPANEL_MAXIMUM_TXT));
	else
		LimitedSPrintf(sizeof(Buffer),Buffer,LocaleBase ? "%9lD" : "%9ld",Level);

	return(Buffer);
}

	/* SelectKeymap(UBYTE *Name,struct Window *ParentWindow):
	 *
	 *	Select a new keymap file using the asl.library file requester.
	 */

STATIC BOOL
SelectKeymap(STRPTR Name,LONG NameSize,struct Window *ParentWindow)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester *FileRequester;
	BOOL NewMap;

	NewMap = FALSE;

	if(FilePart(Name) == Name)
	{
		STRPTR DrawerName;
		BPTR FileLock;
		APTR OldPtr;

		DisableDOSRequesters(&OldPtr);

		if(FileLock = Lock(DrawerName = "KEYMAPS:",ACCESS_READ))
			UnLock(FileLock);
		else
			DrawerName = "DEVS:Keymaps";

		EnableDOSRequesters(OldPtr);

		strcpy(LocalBuffer,DrawerName);
		AddPart(LocalBuffer,Name,sizeof(LocalBuffer));
	}
	else
		LimitedStrcpy(sizeof(LocalBuffer),LocalBuffer,Name);

	if(FileRequester = OpenSingleFile(ParentWindow,LocaleString(MSG_TERMINALPANEL_SELECT_KEYMAP_TXT),NULL,NULL,LocalBuffer,sizeof(LocalBuffer)))
	{
		FreeAslRequest(FileRequester);

		if(Stricmp(LocalBuffer,Name))
		{
			LimitedStrcpy(NameSize,Name,LocalBuffer);

			NewMap = TRUE;
		}
	}

	return(NewMap);
}

BOOL
TerminalPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[5] =
	{
		MSG_V36_1024,
		MSG_V36_1020,
		MSG_V36_1021,
		MSG_V36_1025,
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
		UBYTE FontName[MAX_FILENAME_LENGTH],IBMName[MAX_FILENAME_LENGTH];
		LONG LocalColumns,LocalLines;
		struct Window *PanelWindow;

		BuildFontName(FontName,sizeof(FontName),PrivateConfig->TerminalConfig->TextFontName,PrivateConfig->TerminalConfig->TextFontHeight);
		BuildFontName(IBMName,sizeof(IBMName),PrivateConfig->TerminalConfig->IBMFontName,PrivateConfig->TerminalConfig->IBMFontHeight);

		if(PrivateConfig != Config)
		{
			LocalMaxColumns	= 132;
			LocalMaxLines	= 64;
		}
		else
		{
			LONG Width,Height;

			if(Screen)
			{
				struct Rectangle DisplayClip;

				Width	= PrivateConfig->ScreenConfig->DisplayWidth;
				Height	= PrivateConfig->ScreenConfig->DisplayHeight;

				QueryOverscan(PrivateConfig->ScreenConfig->DisplayMode,&DisplayClip,PrivateConfig->ScreenConfig->OverscanType);

				if(!Width)
					Width = DisplayClip . MaxX - DisplayClip . MinX + 1;

				if(!Height)
					Height = DisplayClip . MaxY - DisplayClip . MinY + 1;
			}
			else
			{
				Width	= Window->WScreen->Width - Window->WScreen->WBorLeft - Window->WScreen->WBorRight;
				Height	= Window->WScreen->Height - Window->WScreen->WBorTop - Window->WScreen->WBorBottom - Window->WScreen->Font->ta_YSize - 1;

				if(!StatusWindow)
					Height -= StatusDisplayHeight;

				if(ChatMode)
					Height -= UserFontHeight + 2;
			}

			if(WindowWidth > Width)
				Width = WindowWidth;

			if(WindowHeight > Height)
				Height = WindowHeight;

			LocalMaxColumns	= Width  / TextFontWidth;
			LocalMaxLines	= Height / TextFontHeight;

			if(LocalMaxColumns < 20)
				LocalMaxColumns = 20;

			if(LocalMaxLines < 20)
				LocalMaxLines = 20;
		}

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
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_BBS_ANSI_COMPLIANT_TXT,
							LA_ID,			GAD_BBS_ANSI_COMPLIANT,
							GTCB_Checked,	PrivateConfig->TerminalConfig->FontMode != FONT_STANDARD && PrivateConfig->TerminalConfig->NumColumns == 80 && PrivateConfig->TerminalConfig->NumLines == 25,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,XBAR_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		TEXT_KIND,
							LA_LabelID,		MSG_TERMINALPANEL_TEXT_FONT_GAD,
							LA_Chars,		20,
							LA_ID,			GAD_TEXTFONT,
							GTTX_Text,		FontName,
							GTTX_Border,	TRUE,
							LATX_Picker,	TRUE,
							LATX_LockSize,	TRUE,
							GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		TEXT_KIND,
							LA_LabelID,		MSG_TERMINALPANEL_IBM_FONT_GAD,
							LA_Chars,		20,
							LA_ID,			GAD_IBMFONT,
							GTTX_Text,		IBMName,
							GTTX_Border,	TRUE,
							LATX_Picker,	TRUE,
							LATX_LockSize,	TRUE,
							GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_TERMINALPANEL_FONT_GAD,
							LA_ID,			GAD_FONTMODE,
							LA_BYTE,		&PrivateConfig->TerminalConfig->FontMode,
							LACY_FirstLabel,MSG_TERMINALPANEL_FONT_STANDARD_TXT,
							LACY_LastLabel,	MSG_TERMINALPANEL_FONT_PC_RAW,
							GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,XBAR_KIND,
						TAG_DONE);

						if(PrivateConfig->TerminalConfig->NumColumns < 20)
							LocalColumns = LocalMaxColumns;
						else
							LocalColumns = PrivateConfig->TerminalConfig->NumColumns;

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_TERMINALPANEL_NUMCOLUMNS_GAD,
							LA_ID,				GAD_COLUMNS,
							LA_LONG,			&LocalColumns,
							GTSL_Min,			20,
							GTSL_Max,			LocalMaxColumns + 1,
							GTSL_DispFunc,		ShowColumn,
							GTSL_LevelFormat,	"%s",
						TAG_DONE);

						if(PrivateConfig->TerminalConfig->NumLines < 20)
							LocalLines = LocalMaxLines;
						else
							LocalLines = PrivateConfig->TerminalConfig->NumLines;

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_TERMINALPANEL_NUMLINES_GAD,
							LA_LONG,			&LocalLines,
							LA_ID,				GAD_LINES,
							GTSL_Min,			20,
							GTSL_Max,			LocalMaxLines + 1,
							GTSL_DispFunc,		ShowLine,
							GTSL_LevelFormat,	"%s",
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					if(PrivateConfig->ScreenConfig->UseWorkbench || PrivateConfig->ScreenConfig->ShareScreen)
					{
						LT_New(Handle,
							LA_Type,VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_AUTO_ADAPT_WINDOW_SIZE_GAD,
								LA_ID,		GAD_AUTO_SIZE,
								LA_BYTE,	&PrivateConfig->TerminalConfig->AutoSize,
							TAG_DONE);

							LT_EndGroup(Handle);
						}
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_EMULATION_GAD,
						LA_ID,			GAD_EMULATIONTYPE,
						LA_BYTE,		&PrivateConfig->TerminalConfig->EmulationMode,
						LACY_FirstLabel,MSG_TERMINALPANEL_EMULATION_ANSI_VT102_TXT,
						LACY_LastLabel,	MSG_TERMINALPANEL_EMULATION_HEX_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_EMULATION_NAME_GAD,
						LA_Chars,		20,
						LA_ID,			GAD_EMULATIONLIBRARY,
						LA_STRPTR,		PrivateConfig->TerminalConfig->EmulationFileName,
						GTST_MaxChars,	sizeof(PrivateConfig->TerminalConfig->EmulationFileName) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode != EMULATION_EXTERNAL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_EMULATIONPANEL_USE_TERMINAL_TASK_TXT,
						LA_BYTE,		&PrivateConfig->TerminalConfig->UseTerminalTask,
						LA_ID,			GAD_EMULATIONTASK,
						GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_BELL_GAD,
						LA_BYTE,		&PrivateConfig->TerminalConfig->BellMode,
						LACY_FirstLabel,MSG_TERMINALPANEL_BELL_IGNORE_TXT,
						LACY_LastLabel,	MSG_TERMINALPANEL_BELL_DEFAULT_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_KEYMAP_FILE_GAD,
						LA_Chars,		20,
						LA_ID,			GAD_KEYMAP,
						LA_STRPTR,		PrivateConfig->TerminalConfig->KeyMapFileName,
						GTST_MaxChars,	sizeof(PrivateConfig->TerminalConfig->KeyMapFileName) - 1,
						LAST_Picker,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_SEND_CR_GAD,
						LA_BYTE,		&PrivateConfig->TerminalConfig->SendCR,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_SEND_LF_GAD,
						LA_BYTE,		&PrivateConfig->TerminalConfig->SendLF,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_RECEIVE_CR_GAD,
						LA_BYTE,		&PrivateConfig->TerminalConfig->ReceiveCR,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_RECEIVE_LF_GAD,
						LA_BYTE,		&PrivateConfig->TerminalConfig->ReceiveLF,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,LA_Type,XBAR_KIND,LAXB_FullSize,TRUE,TAG_DONE);

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
			LAWN_TitleID,		MSG_TERMINALPANEL_TERMINAL_PREFERENCES_TXT,
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
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			GuideContext(CONTEXT_TERMINAL);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

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
							case GAD_COLUMNS:
							case GAD_LINES:
							case GAD_FONTMODE:

								LT_SetAttributes(Handle,GAD_BBS_ANSI_COMPLIANT,
									GTCB_Checked,	PrivateConfig->TerminalConfig->FontMode != FONT_STANDARD && (LocalColumns % (LocalMaxColumns + 1)) == 80 && (LocalLines % (LocalMaxLines + 1)) == 25,
								TAG_DONE);

								break;

							case GAD_BBS_ANSI_COMPLIANT:

								LT_SetAttributes(Handle,GAD_COLUMNS,
									GTSL_Level,	80,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_LINES,
									GTSL_Level,	25,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_FONTMODE,
									GTCY_Active,	PrivateConfig->TerminalConfig->FontMode == FONT_STANDARD ? FONT_IBM : PrivateConfig->TerminalConfig->FontMode,
								TAG_DONE);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								PrivateConfig->TerminalConfig->NumColumns	= LocalColumns % (LocalMaxColumns + 1);
								PrivateConfig->TerminalConfig->NumLines		= LocalLines % (LocalMaxLines + 1);

								if(!PrivateConfig->TerminalConfig->EmulationFileName[0] && PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
									PrivateConfig->TerminalConfig->EmulationMode = EMULATION_ANSIVT100;

								UseIt = Done = TRUE;
								break;

							case GAD_EMULATIONTYPE:

								LT_SetAttributes(Handle,GAD_TEXTFONT,
									GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_IBMFONT,
									GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_FONTMODE,
									GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_EMULATIONLIBRARY,
									GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode != EMULATION_EXTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_EMULATIONTASK,
									GA_Disabled,	PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL,
								TAG_DONE);

								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_EMULATIONLIBRARY:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,GAD_EMULATIONLIBRARY));
								strcpy(LocalBuffer,DummyBuffer);

								if(PickFile(PanelWindow,"Libs:","xem#?.library",LocaleString(MSG_TERMINALPANEL_SELECT_EMULATOR_TXT),DummyBuffer,NT_LIBRARY))
								{
									if(Stricmp(LocalBuffer,DummyBuffer))
									{
										LT_SetAttributes(Handle,GAD_EMULATIONLIBRARY,
											GTST_String,DummyBuffer,
										TAG_DONE);
									}
								}

								LT_ShowWindow(Handle,TRUE);

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_KEYMAP:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LT_GetString(Handle,GAD_KEYMAP));

								if(SelectKeymap(DummyBuffer,sizeof(DummyBuffer),PanelWindow))
								{
									LT_SetAttributes(Handle,GAD_KEYMAP,
										GTST_String,	DummyBuffer,
									TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_TEXTFONT:

								if(OpenFixedFont(PanelWindow,PrivateConfig->TerminalConfig->TextFontName,&PrivateConfig->TerminalConfig->TextFontHeight))
								{
									BuildFontName(FontName,sizeof(FontName),PrivateConfig->TerminalConfig->TextFontName,PrivateConfig->TerminalConfig->TextFontHeight);

									LT_SetAttributes(Handle,GAD_TEXTFONT,
										GTTX_Text,	FontName,
									TAG_DONE);
								}

								break;

							case GAD_IBMFONT:

								if(OpenFixedFont(PanelWindow,PrivateConfig->TerminalConfig->IBMFontName,&PrivateConfig->TerminalConfig->IBMFontHeight))
								{
									BuildFontName(IBMName,sizeof(IBMName),PrivateConfig->TerminalConfig->IBMFontName,PrivateConfig->TerminalConfig->IBMFontHeight);

									LT_SetAttributes(Handle,GAD_IBMFONT,
										GTTX_Text,	IBMName,
									TAG_DONE);
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

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}
