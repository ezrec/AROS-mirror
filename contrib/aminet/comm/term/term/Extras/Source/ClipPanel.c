/*
**	ClipPanel.c
**
**	Editing panel for clipboard configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_UNIT=1000,GAD_USE,GAD_CANCEL,
			GAD_PAGER,GAD_PAGEGROUP
		};

BOOL
ClipPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[3] =
	{
		MSG_V36_0019,
		MSG_V36_0020,
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
							LA_Type,				INTEGER_KIND,
							LA_LabelID,				MSG_CLIPPANEL_UNIT_GAD,
							LA_WORD,				&PrivateConfig->ClipConfig->ClipboardUnit,
							LA_Chars,				20,
							LAIN_Min,				0,
							LAIN_Max,				255,
							LA_ID,					GAD_UNIT,
							LAIN_UseIncrementers,	TRUE,
							GTIN_MaxChars,			3,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		STRING_KIND,
							LA_LabelID,		MSG_CLIPPANEL_PASTE_PREFIX_GAD,
							LA_STRPTR,		PrivateConfig->ClipConfig->InsertPrefix,
							GTST_MaxChars,	sizeof(PrivateConfig->ClipConfig->InsertPrefix) - 1,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		STRING_KIND,
							LA_LabelID,		MSG_CLIPPANEL_PASTE_SUFFIX_GAD,
							LA_STRPTR,		PrivateConfig->ClipConfig->InsertSuffix,
							GTST_MaxChars,	sizeof(PrivateConfig->ClipConfig->InsertSuffix) - 1,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		XBAR_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	CHECKBOX_KIND,
							LA_LabelID,	MSG_CONVERT_LF_TXT,
							LA_BYTE,	&PrivateConfig->ClipConfig->ConvertLF,
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
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		CYCLE_KIND,
							LA_LabelID,		MSG_CLIPPANEL_TEXT_PACING_GAD,
							LA_UBYTE,		&PrivateConfig->ClipConfig->PacingMode,
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
							LA_WORD,			&PrivateConfig->ClipConfig->CharDelay,
							GTSL_LevelFormat,	"%s s",
							GTSL_DispFunc,		StandardShowTime,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_CLIPPANEL_LINE_DELAY_GAD,
							GTSL_Min,			0,
							GTSL_Max,			10 * 100,
							LA_Chars,			10,
							LA_WORD,			&PrivateConfig->ClipConfig->LineDelay,
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
							LA_STRPTR,			PrivateConfig->ClipConfig->LinePrompt,
							LA_Chars,			10,
							GTST_MaxChars,		sizeof(PrivateConfig->ClipConfig->LinePrompt) - 1,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			SliderType,
							LA_LabelID,			MSG_CLIPPANEL_SEND_TIMEOUT_GAD,
							GTSL_Min,			0,
							GTSL_Max,			10 * 100,
							LA_WORD,			&PrivateConfig->ClipConfig->SendTimeout,
							GTSL_LevelFormat,	"%s s",
							GTSL_DispFunc,		StandardShowTime,
						TAG_DONE);

						LT_EndGroup(Handle);
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
			LAWN_TitleID,		MSG_CLIPPANEL_CLIP_SETTINGS_TXT,
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

			GuideContext(CONTEXT_CLIP);

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
