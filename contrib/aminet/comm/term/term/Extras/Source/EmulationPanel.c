/*
**	EmulationPanel.c
**
**	Editing panel for emulation configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_USE=1000, GAD_CANCEL,
			GAD_SCROLL, GAD_JUMP_SCROLL,
			GAD_EDITPENS, GAD_STDPENS,
			GAD_PAGER, GAD_PAGEGROUP, GAD_BBS_ANSI_COMPLIANT,
			GAD_TERMINALTYPE, GAD_CLS_RESETS_CURSOR, GAD_WRAP_CURSOR_MOVES,
			GAD_WRAP_CHARACTERS, GAD_DESTRUCTIVE_BS,
		};

STATIC BOOL
EmulationPenPanel(struct Window *Parent,struct Configuration *Config,LONG Depth)
{
	LayoutHandle *Handle;
	UBYTE Attribute[4];
	UBYTE Colours[16];
	UWORD Pen[16];
	BOOL Result;
	LONG i;

	Result = FALSE;

	for(i = 0 ; i < (1L << Depth) ; i++)
		Colours[i] = MappedPens[0][i];

	CopyMem(Config->EmulationConfig->Pens,Pen,sizeof(Pen));
	CopyMem(Config->EmulationConfig->Attributes,Attribute,sizeof(Attribute));

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExactClone,	TRUE,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
					LA_LabelID,	MSG_PENPANEL_DRAWING_PENS_TXT,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelText,		"_0",
						GTPA_Depth,			Depth,
						GTPA_ColorTable,	Colours,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&Pen[0],
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			PALETTE_KIND,
						LA_LabelText,		"_1",
						GTPA_Depth,			Depth,
						GTPA_ColorTable,	Colours,
						LAPA_SmallPalette,	TRUE,
						LA_UWORD,			&Pen[1],
					TAG_DONE);

					if(Depth > 1)
					{
						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_2",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[2],
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_3",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[3],
						TAG_DONE);
					}

					if(Depth > 2)
					{
						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_4",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[4],
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_5",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[5],
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_6",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[6],
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			PALETTE_KIND,
							LA_LabelText,		"_7",
							GTPA_Depth,			Depth,
							GTPA_ColorTable,	Colours,
							LAPA_SmallPalette,	TRUE,
							LA_UWORD,			&Pen[7],
						TAG_DONE);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
					LA_LabelID,	MSG_ATTRIBUTES_TXT,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_ATTRIBUTE1_GAD,
						LACY_FirstLabel,MSG_ATTRIBUTE1_GAD,
						LACY_LastLabel,	MSG_ATTRIBUTE5_GAD,
						LA_UBYTE,		&Attribute[TEXTATTR_UNDERLINE],
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_ATTRIBUTE2_GAD,
						LACY_FirstLabel,MSG_ATTRIBUTE1_GAD,
						LACY_LastLabel,	MSG_ATTRIBUTE5_GAD,
						LA_UBYTE,		&Attribute[TEXTATTR_HIGHLIGHT],
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_ATTRIBUTE3_GAD,
						LACY_FirstLabel,MSG_ATTRIBUTE1_GAD,
						LACY_LastLabel,	MSG_ATTRIBUTE5_GAD,
						LA_UBYTE,		&Attribute[TEXTATTR_BLINK],
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_ATTRIBUTE4_GAD,
						LACY_FirstLabel,MSG_ATTRIBUTE1_GAD,
						LACY_LastLabel,	MSG_ATTRIBUTE5_GAD,
						LA_UBYTE,		&Attribute[TEXTATTR_INVERSE],
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
			LAWN_TitleID,		MSG_PENPANEL_SCREEN_DRAWING_PENS_TXT,
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
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			GuideContext(CONTEXT_TEXTPEN_PANEL);

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

								CopyMem(Pen,Config->EmulationConfig->Pens,sizeof(Pen));
								CopyMem(Attribute,Config->EmulationConfig->Attributes,sizeof(Attribute));

								Result = Done = TRUE;
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

	return(Result);
}

BOOL
EmulationPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[5] =
	{
		MSG_V36_0035,
		MSG_V36_0034,
		MSG_V36_0036,
		MSG_PENPANEL_DRAWING_PENS_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	LONG Depth = 1,CurrentDepth = 1; /* COLOUR_MONO */
	LayoutHandle *Handle;
	CONST_STRPTR PageTitle[5];
	BOOL PenChoices;
	BOOL UseIt;

	UseIt = FALSE;

	LocalizeStringTable(PageTitle,PageTitleID);

	SaveConfig(LocalConfig,PrivateConfig);

	if(PrivateConfig->ScreenConfig->UseWorkbench || PrivateConfig->ScreenConfig->ShareScreen)
		PenChoices = FALSE;
	else
		PenChoices = TRUE;

	switch(PrivateConfig->ScreenConfig->ColourMode)
	{
		case COLOUR_MONO:

			Depth = 1;
			break;

		case COLOUR_AMIGA:

			Depth = 2;
			break;

		case COLOUR_EIGHT:

			Depth = 3;
			break;

		case COLOUR_SIXTEEN:

			Depth = 4;
			break;
	}

	switch(Config->ScreenConfig->ColourMode)
	{
		case COLOUR_MONO:

			CurrentDepth = 1;
			break;

		case COLOUR_AMIGA:

			CurrentDepth = 2;
			break;

		case COLOUR_EIGHT:

			CurrentDepth = 3;
			break;

		case COLOUR_SIXTEEN:

			CurrentDepth = 4;
			break;
	}

	if(Depth != CurrentDepth || !PenChoices)
	{
		PageTitle[3] = NULL;

		if(InitialPage == 4)
			InitialPage = 3;
	}
	else
		PageTitle[4] = NULL;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExactClone,	TRUE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

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
					GTCY_Labels,	PageTitle,
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
							GTCB_Checked,	PrivateConfig->EmulationConfig->CLSResetsCursor && PrivateConfig->EmulationConfig->CursorWrap && PrivateConfig->EmulationConfig->LineWrap && PrivateConfig->EmulationConfig->TerminalType == TERMINAL_VT102 && !PrivateConfig->EmulationConfig->DestructiveBackspace,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		XBAR_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	HORIZONTAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_EMULATIONPANEL_WRAP_CHARACTERS_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->LineWrap,
								LA_ID,		GAD_WRAP_CHARACTERS,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_LOCK_WRAPPING_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->LockWrapping,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_EMULATIONPANEL_INSERT_MODE_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->InsertMode,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_EMULATIONPANEL_NEW_LINE_MODE_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->NewLineMode,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_EMULATIONPANEL_CLS_RESETS_CURSOR_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->CLSResetsCursor,
								LA_ID,		GAD_CLS_RESETS_CURSOR,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_EMULATIONPANEL_PRINTER_CONTROL_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->PrinterEnabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_LOCK_STYLE_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->LockStyle,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_LOCK_COLOUR_GAD,
								LA_BYTE,	&PrivateConfig->EmulationConfig->LockColour,
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
							LA_Type,	CHECKBOX_KIND,
							LA_LabelID,	MSG_EMULATIONPANEL_WRAP_CURSOR_MOVES_GAD,
							LA_BYTE,	&PrivateConfig->EmulationConfig->CursorWrap,
							LA_ID,		GAD_WRAP_CURSOR_MOVES,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_TERMINALTYPE_TXT,
							LA_BYTE,		&PrivateConfig->EmulationConfig->TerminalType,
							LACY_FirstLabel,MSG_TERMINALTYPE_VT200_TXT,
							LACY_LastLabel,	MSG_TERMINALTYPE_VT100_TXT,
							LA_ID,			GAD_TERMINALTYPE,
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
						LA_LabelID,		MSG_EMULATIONPANEL_CURSOR_KEYS_GAD,
						LA_BYTE,		&PrivateConfig->EmulationConfig->CursorMode,
						LACY_FirstLabel,MSG_EMULATIONPANEL_STANDARD_MODE_TXT,
						LACY_LastLabel,	MSG_EMULATIONPANEL_APPLICATION_MODE_TXT,
						LA_Chars,		20,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_LOCK_CURSOR_KEY_MODE_TXT,
						LA_BYTE,		&PrivateConfig->EmulationConfig->CursorLocked,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_EMULATIONPANEL_NUMERIC_KEYPAD_GAD,
						LA_BYTE,		&PrivateConfig->EmulationConfig->NumericMode,
						LACY_FirstLabel,MSG_EMULATIONPANEL_STANDARD_MODE_TXT,
						LACY_LastLabel,	MSG_EMULATIONPANEL_APPLICATION_MODE_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_V36_1820,
						LA_BYTE,		&PrivateConfig->EmulationConfig->KeysLocked,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_EMULATIONPANEL_SWAP_BS_AND_DEL_GAD,
						LA_BYTE,		&PrivateConfig->EmulationConfig->SwapBSDelete,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,				CHECKBOX_KIND,
						LA_LabelID,				MSG_LOCK_FONT_MODE_TXT,
						LA_BYTE,				&PrivateConfig->EmulationConfig->FontLocked,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CYCLE_KIND,
						LA_LabelID,				MSG_EMULATIONPANEL_SMOOTH_SCROLLING_GAD,
						LA_BYTE,				&PrivateConfig->EmulationConfig->ScrollMode,
						LACY_FirstLabel,		MSG_EMULATIONPANEL_JUMP_TXT,
						LACY_LastLabel,			MSG_EMULATIONPANEL_SMOOTH_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CYCLE_KIND,
						LA_ID,					GAD_DESTRUCTIVE_BS,
						LA_LabelID,				MSG_EMULATIONPANEL_DESTRUCTIVE_BACKSPACE_GAD,
						LA_BYTE,				&PrivateConfig->EmulationConfig->DestructiveBackspace,
						LACY_FirstLabel,		MSG_DESTRUCTIVE_BS_OFF_TXT,
						LACY_LastLabel,			MSG_DESTRUCTIVE_BS_SHIFT_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				STRING_KIND,
						LA_LabelID,				MSG_EMULATIONPANEL_ANSWERBACK_MESSAGE_GAD,
						LA_STRPTR,				PrivateConfig->EmulationConfig->AnswerBack,
						GTST_MaxChars,			sizeof(PrivateConfig->EmulationConfig->AnswerBack) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_EMULATIONPANEL_PRESCROLL_TXT,
						LA_UBYTE,				&PrivateConfig->EmulationConfig->MaxScroll,
						LAIN_Min,				0,
						LAIN_Max,				255,
						LA_ID,					GAD_SCROLL,
						LAIN_UseIncrementers,	TRUE,
						GTIN_MaxChars,			3,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_EMULATIONPANEL_JUMP_SCROLL_TXT,
						LA_UBYTE,				&PrivateConfig->EmulationConfig->MaxJump,
						LAIN_Min,				0,
						LAIN_Max,				255,
						LA_ID,					GAD_JUMP_SCROLL,
						LAIN_UseIncrementers,	TRUE,
						GTIN_MaxChars,			3,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				if(PageTitle[3])
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
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_PENPANEL_STANDARDPENS_TXT,
								LA_ID,		GAD_STDPENS,
								LA_BYTE,	&PrivateConfig->EmulationConfig->UseStandardPens,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_LabelID,		MSG_PENPANEL_EDITPENS_TXT,
								LA_ID,			GAD_EDITPENS,
								GA_Disabled,	PrivateConfig->EmulationConfig->UseStandardPens,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}
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
			LAWN_TitleID,		MSG_EMULATIONPANEL_EMULATION_PREFERENCES_TXT,
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
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			UWORD MsgCode;
			BOOL Done;

			GuideContext(CONTEXT_EMULATION);

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
					MsgCode		= Message->Code;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_STDPENS:

								LT_SetAttributes(Handle,GAD_EDITPENS,
									GA_Disabled,	MsgCode,
								TAG_DONE);

								break;

							case GAD_EDITPENS:

								LT_LockWindow(PanelWindow);

								EmulationPenPanel(PanelWindow,PrivateConfig,Depth);

								LT_UnlockWindow(PanelWindow);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								UseIt = Done = TRUE;

								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							case GAD_BBS_ANSI_COMPLIANT:

								if(MsgCode)
								{
									LT_SetAttributes(Handle,GAD_TERMINALTYPE,
										GTCY_Active,	TERMINAL_VT102,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_CLS_RESETS_CURSOR,
										GTCB_Checked,	TRUE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_WRAP_CURSOR_MOVES,
										GTCB_Checked,	TRUE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_WRAP_CHARACTERS,
										GTCB_Checked,	TRUE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_DESTRUCTIVE_BS,
										GTCY_Active,	0,
									TAG_DONE);
								}

								break;

							default:

								LT_SetAttributes(Handle,GAD_BBS_ANSI_COMPLIANT,
									GTCB_Checked,	PrivateConfig->EmulationConfig->CLSResetsCursor && PrivateConfig->EmulationConfig->CursorWrap && PrivateConfig->EmulationConfig->LineWrap && PrivateConfig->EmulationConfig->TerminalType == TERMINAL_VT102 && !PrivateConfig->EmulationConfig->DestructiveBackspace,
								TAG_DONE);

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

	if(LocalConfig == Config && UseIt)
		ScreenSizeStuff();

	return(UseIt);
}
