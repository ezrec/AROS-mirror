/*
**	LibPanelPlus.c
**
**	Editing panel for transfer library configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_COMMAND=1000,
			GAD_BUTTON1,GAD_BUTTON2,GAD_BUTTON3,GAD_BUTTON4,GAD_BUTTON5,
			GAD_BUTTON6,GAD_BUTTON7,GAD_BUTTON8,GAD_BUTTON9,GAD_BUTTON10,
			GAD_PAGER,GAD_PAGEGROUP,

			GAD_USE,GAD_CANCEL
		};

VOID
ASCIITransferPanel(struct Window *Window,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[4] =
	{
		MSG_INTERNAL_ASCII_TRANSFER_SEND_TXT,
		MSG_INTERNAL_ASCII_TRANSFER_RECEIVE_TXT,
		MSG_INTERNAL_ASCII_TRANSFER_GENERAL_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	struct TransferSettings	*OldSettings;
	LayoutHandle *Handle;
	CONST_STRPTR PageTitle[4];

	if(!(OldSettings = CreateNewConfigEntry(PREF_TRANSFER)))
	{
		DisplayBeep(Window ? Window->WScreen : NULL);
		return;
	}

	CopyConfigEntry(LocalConfig,PREF_TRANSFER,OldSettings);

	LocalizeStringTable(PageTitle,PageTitleID);

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
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			TAB_KIND,
					LA_LabelID,			MSG_XFER_PAGE_TXT,
					LA_ID,				GAD_PAGER,
					GTCY_Labels,		PageTitle,
					LACY_AutoPageID,	GAD_PAGEGROUP,
					LACY_TabKey,		TRUE,
					LA_BYTE,			&InitialPage,
					LATB_FullWidth,		TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_ID,		GAD_PAGEGROUP,
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
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_CLIPPANEL_TEXT_PACING_GAD,
							LA_UBYTE,		&LocalConfig->TransferConfig->PacingMode,
							LACY_FirstLabel,MSG_CLIPPANEL_DIRECT_TXT,
							LACY_LastLabel,	MSG_CLIPPANEL_KEYDELAY_TXT,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
						LA_LabelID,	MSG_V36_0021,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_CLIPPANEL_CHAR_DELAY_GAD,
							GTSL_Min,			0,
							GTSL_Max,			10 * 100,
							LA_Chars,			10,
							LA_WORD,			&LocalConfig->TransferConfig->CharDelay,
							GTSL_LevelFormat,	"%s s",
							GTSL_DispFunc,		StandardShowTime,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_CLIPPANEL_LINE_DELAY_GAD,
							GTSL_Min,			0,
							GTSL_Max,			10 * 100,
							LA_Chars,			10,
							LA_WORD,			&LocalConfig->TransferConfig->LineDelay,
							GTSL_LevelFormat,	"%s s",
							GTSL_DispFunc,		StandardShowTime,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
						LA_LabelID,	MSG_V36_0024,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelID,			MSG_CLIPPANEL_LINE_PROMPT_GAD,
							LA_STRPTR,			LocalConfig->TransferConfig->LinePrompt,
							LA_Chars,			10,
							GTST_MaxChars,		sizeof(LocalConfig->TransferConfig->LinePrompt) - 1,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_CLIPPANEL_SEND_TIMEOUT_GAD,
							GTSL_Min,			0,
							GTSL_Max,			10 * 100,
							LA_WORD,			&LocalConfig->TransferConfig->SendTimeout,
							GTSL_LevelFormat,	"%s s",
							GTSL_DispFunc,		StandardShowTime,
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
							LA_BYTE,		&LocalConfig->TransferConfig->SendCR,
							GTCY_Labels,	EOL_Labels,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_TERMINALPANEL_SEND_LF_GAD,
							LA_BYTE,		&LocalConfig->TransferConfig->SendLF,
							GTCY_Labels,	EOL_Labels,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_EXPAND_BLANK_LINES_GAD,
							LA_BYTE,		&LocalConfig->TransferConfig->ExpandBlankLines,
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
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_RECEIVE_CR_GAD,
						LA_BYTE,		&LocalConfig->TransferConfig->ReceiveCR,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_TERMINALPANEL_RECEIVE_LF_GAD,
						LA_BYTE,		&LocalConfig->TransferConfig->ReceiveLF,
						GTCY_Labels,	EOL_Labels,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_INTERNAL_ASCII_TRANSFER_IGNORE_DATA_PAST_ARNOLD_TXT,
						LA_BYTE,		&LocalConfig->TransferConfig->IgnoreDataPastArnold,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		INTEGER_KIND,
						LA_LabelID,		MSG_INTERNAL_ASCII_TRANSFER_TERMINATOR_CHAR_TXT,
						LA_BYTE,		&LocalConfig->TransferConfig->TerminatorChar,
						LAIN_Min,		0,
						LAIN_Max,		255,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_LIBPANEL_QUIET_TRANSFER_GAD,
						LA_BYTE,	&LocalConfig->TransferConfig->QuietTransfer,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,	CHECKBOX_KIND,
						LA_LabelID,	MSG_INTERNAL_ASCII_TRANSFER_STRIP_BIT_7_TXT,
						LA_BYTE,	&LocalConfig->TransferConfig->StripBit8,
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
			LAWN_TitleID,		MSG_INTERNAL_ASCII_TRANSFER_TRANSFER_TXT,
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

			GuideContext(CONTEXT_ASCII_TRANSFER_SETTINGS);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
				{
					PutConfigEntry(LocalConfig,OldSettings,PREF_TRANSFER);
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
							case GAD_USE:

								LT_UpdateStrings(Handle);

								Done = TRUE;
								break;

							case GAD_CANCEL:

								PutConfigEntry(LocalConfig,OldSettings,PREF_TRANSFER);

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

	FreeVecPooled(OldSettings);
}

BOOL
ExternalCommandPanel(struct Window *Parent,STRPTR Command)
{
	LayoutHandle *Handle;
	BOOL Result;

	Result = FALSE;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		STATIC LONG ButtonLabels[] =
		{
			MSG_XFER_BUTTON1_TXT,MSG_XFER_BUTTON2_TXT,
			MSG_XFER_BUTTON3_TXT,MSG_XFER_BUTTON4_TXT,
			MSG_XFER_BUTTON5_TXT,MSG_XFER_BUTTON6_TXT,
			MSG_XFER_BUTTON7_TXT,MSG_XFER_BUTTON8_TXT,
			MSG_XFER_BUTTON9_TXT,MSG_XFER_BUTTON10_TXT,
		};

		UBYTE LocalCommand[MAX_FILENAME_LENGTH];
		struct Window *PanelWindow;
		LONG i,Len,Max;

		LimitedStrcpy(sizeof(LocalCommand),LocalCommand,Command);

		for(i = Max = 0 ; i < 10 ; i++)
		{
			if((Len = LT_LabelChars(Handle,LocaleString(ButtonLabels[i]))) > Max)
				Max = Len;
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
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_COMMAND,
					LA_LabelID,		MSG_XFER_COMMAND_TXT,
					LA_Chars,		50,
					LA_STRPTR,		LocalCommand,
					GTST_MaxChars,	sizeof(LocalCommand) - 1,
					LAST_Picker,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	XBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
				TAG_DONE);
				{
					for(i = 0 ; i < 5 ; i++)
					{
						LT_New(Handle,
							LA_Type,	BUTTON_KIND,
							LA_Chars,	Max,
							LA_ID,		GAD_BUTTON1 + i,
							LA_LabelID,	ButtonLabels[i],
						TAG_DONE);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
				TAG_DONE);
				{
					for(i = 5 ; i < 10 ; i++)
					{
						LT_New(Handle,
							LA_Type,	BUTTON_KIND,
							LA_Chars,	Max,
							LA_ID,		GAD_BUTTON1 + i,
							LA_LabelID,	ButtonLabels[i],
						TAG_DONE);
					}

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
			LAWN_TitleID,		MSG_EXTERNAL_PROGRAM_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			STATIC STRPTR Strings[] =
			{
				" %f",
				" %m",
				" %p",
				" %d",
				" %u",
				" %<",
				" %>",
				" %s",
				" %b",
				" %c"
			};

			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			GuideContext(CONTEXT_PARAMETERS);

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
							case GAD_USE:

								strcpy(Command,LocalCommand);

								Result = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							default:

								if(MsgGadget->GadgetID >= GAD_BUTTON1 && MsgGadget->GadgetID <= GAD_BUTTON8)
								{
									LT_LockWindow(PanelWindow);

									if(strlen(LocalCommand) + 3 < 255)
									{
										LimitedStrcat(sizeof(LocalCommand),LocalCommand,Strings[MsgGadget->GadgetID - GAD_BUTTON1]);

										LT_SetAttributes(Handle,GAD_COMMAND,
											GTST_String,			LocalCommand,
											LAST_CursorPosition,	-1,
										TAG_DONE);
									}
									else
										DisplayBeep(PanelWindow->WScreen);

									LT_UnlockWindow(PanelWindow);
								}

								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						struct FileRequester *FileRequest;
						STRPTR Index,Args;

						strcpy(LocalBuffer,LocalCommand);

						Index = FilePart(LocalBuffer);

						while(*Index && *Index != ' ')
							Index++;

						if(*Index == ' ')
						{
							*Index = 0;

							Args = &LocalCommand[(IPTR)Index - (IPTR)LocalBuffer + 1];
						}
						else
							Args = NULL;

						if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_XFER_SELECT_COMMAND_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,LocalBuffer,sizeof(LocalBuffer)))
						{
							LONG Length = strlen(LocalBuffer);

							FreeAslRequest(FileRequest);

							if(Length < sizeof(LocalBuffer) - 3 && Args)
							{
								LimitedStrcat(sizeof(LocalBuffer),LocalBuffer," ");

								Length++;

								CopyMem(Args,LocalBuffer + Length,sizeof(LocalBuffer) - 1 - Length);

								LocalBuffer[sizeof(LocalBuffer) - 1] = 0;
							}

							LT_SetAttributes(Handle,GAD_COMMAND,
								GTST_String,LocalBuffer,
							TAG_DONE);
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	return(Result);
}
