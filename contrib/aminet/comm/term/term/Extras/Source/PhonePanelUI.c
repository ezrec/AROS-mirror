/*
**	PhonePanelUI.c
**
**	The phonebook user interface creation routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The gadget IDs are in there */

#ifndef _PHONEPANEL_H
#include "PhonePanel.h"
#endif	/* _PHONEPANEL_H */

LayoutHandle *
CreateEditorHandle(PhoneListContext *Context,BOOL Activate)
{
	STATIC LONG LabelTable1[] =
	{
		MSG_PHONEBOOK_SETTINGS_SERIAL_TXT,
		MSG_PHONEBOOK_SETTINGS_MODEM_TXT,
		MSG_PHONEBOOK_SETTINGS_SCREEN_TXT,
		MSG_PHONEBOOK_SETTINGS_TERMINAL_TXT,
		MSG_PHONEBOOK_SETTINGS_EMULATION_TXT,
		MSG_PHONEBOOK_SETTINGS_CLIPBOARD_TXT,
		MSG_PHONEBOOK_SETTINGS_CAPTURE_TXT,
		MSG_PHONEBOOK_SETTINGS_COMMANDS_TXT,
		-1
	};

	STATIC LONG LabelTable2[] =
	{
		MSG_PHONEBOOK_SETTINGS_MISCELLANEOUS_TXT,
		MSG_PHONEBOOK_SETTINGS_PATHS_TXT,
		MSG_PHONEBOOK_SETTINGS_TRANSFER_TXT,
		MSG_PHONEBOOK_SETTINGS_TRANSLATION_TXT,
		MSG_PHONEBOOK_SETTINGS_FUNCTION_KEYS_TXT,
		MSG_PHONEBOOK_SETTINGS_CURSOR_KEYS_TXT,
		MSG_PHONEBOOK_SETTINGS_FAST_MACROS_TXT,
		MSG_PHONEBOOK_SETTINGS_RATES_TXT,
		-1
	};

	PhoneNode				*Node;
	LONG					 Count;

	LayoutHandle			*Handle;
	BOOL					 IsFirst,IsLast,Disabled,RatesValid;
	LONG					 LabelLen1,LabelLen2;
	struct Configuration	*LocalConfig;
	LONG					 i,Len;

	Node = Context->SelectedNode;
	Count = Context->PhoneHandle->DialMarker;

	if(Node)
	{
		IsFirst			= (Node->Entry->Count == -1 || Node->Entry->Count == 0);
		IsLast			= (Node->Entry->Count == -1 || Node->Entry->Count == Count - 1);
		Disabled		= FALSE;
		LocalConfig		= Node->Entry->Config;
		RatesValid		= (!IsListEmpty((struct List *)&Node->Entry->TimeDateList) && !Node->Entry->Header->NoRates);
	}
	else
	{
		IsFirst			= TRUE;
		IsLast			= TRUE;
		Disabled		= TRUE;
		LocalConfig		= NULL;
		RatesValid		= FALSE;
	}

	if(Handle = LT_CreateHandleTags(Context->Window->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		for(i = LabelLen1 = 0 ; LabelTable1[i] != -1 ; i++)
		{
			Len = LT_LabelChars(Handle,LocaleString(LabelTable1[i]));

			if(Len > LabelLen1)
				LabelLen1 = Len;
		}

		for(i = LabelLen2 = 0 ; LabelTable2[i] != -1 ; i++)
		{
			Len = LT_LabelChars(Handle,LocaleString(LabelTable2[i]));

			if(Len > LabelLen2)
				LabelLen2 = Len;
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
					LA_LabelID,		MSG_PHONEBOOK_ENTRY_DATA_TXT,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PHONEBOOK_SYSTEM_NAME_TXT,
						LA_Chars,		30,
						LA_ID,			GAD_SYSTEM_NAME,
						GTST_String,	Node ? Node->Entry->Header->Name : (STRPTR)"",
						GTST_MaxChars,	sizeof(Node->Entry->Header->Name) - 1,
						GA_Disabled,	Disabled,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PHONEBOOK_COMMENT_TXT,
						LA_ID,			GAD_COMMENT,
						GTST_String,	Node ? Node->Entry->Header->Comment : (STRPTR)"",
						GTST_MaxChars,	sizeof(Node->Entry->Header->Comment) - 1,
						GA_Disabled,	Disabled,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PHONEBOOK_NUMBERS_TXT,
						LA_ID,			GAD_NUMBERS,
						GTST_String,	Node ? Node->Entry->Header->Number : (STRPTR)"",
						GTST_MaxChars,	sizeof(Node->Entry->Header->Number),
						GA_Disabled,	Disabled,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_PHONEBOOK_QUICK_MENU_TXT,
						LA_ID,			GAD_QUICK_MENU,
						GTCB_Checked,	Node ? Node->Entry->Header->QuickMenu : FALSE,
						GA_Disabled,	Disabled,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_PHONEPANEL_AUTO_DIAL_TXT,
						LA_ID,			GAD_AUTODIAL,
						GTCB_Checked,	Node ? Node->Entry->Header->AutoDial : FALSE,
						GA_Disabled,	Disabled,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_HIDE_TXT,
						LA_ID,			GAD_HIDE_USERNAME_PASSWORD,
						GTCB_Checked,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PHONEBOOK_USER_NAME_TXT,
						LA_ID,			GAD_USER_NAME,
						GTST_String,	"",
						GTST_MaxChars,	structsizeof(PhoneHeader,UserName) - 1,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PHONEBOOK_LOGIN_PASSWORD_TXT,
						LA_ID,			GAD_USER_PASSWORD,
						LAST_LastGadget,TRUE,
						GTST_String,	"",
						GTST_MaxChars,	structsizeof(PhoneHeader,Password) - 1,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LA_LabelID,		MSG_PHONEBOOK_SETTINGS_TXT,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_SERIAL_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->SerialConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_SERIAL_TXT,
								LA_ID,			GAD_SERIAL_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_MODEM_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->ModemConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_MODEM_TXT,
								LA_ID,			GAD_MODEM_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_SCREEN_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->ScreenConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_SCREEN_TXT,
								LA_ID,			GAD_SCREEN_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_TERMINAL_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->TerminalConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_TERMINAL_TXT,
								LA_ID,			GAD_TERMINAL_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_EMULATION_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->EmulationConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_EMULATION_TXT,
								LA_ID,			GAD_EMULATION_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_CLIPBOARD_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->ClipConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_CLIPBOARD_TXT,
								LA_ID,			GAD_CLIPBOARD_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_CAPTURE_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->CaptureConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_CAPTURE_TXT,
								LA_ID,			GAD_CAPTURE_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_COMMAND_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->CommandConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen1,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_COMMANDS_TXT,
								LA_ID,			GAD_COMMAND_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,		VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_MISC_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->MiscConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_MISCELLANEOUS_TXT,
								LA_ID,			GAD_MISC_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_PATH_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->PathConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_PATHS_TXT,
								LA_ID,			GAD_PATH_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_TRANSFER_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->TransferConfig != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_TRANSFER_TXT,
								LA_ID,			GAD_TRANSFER_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_TRANSLATION_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->TranslationFileName != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_TRANSLATION_TXT,
								LA_ID,			GAD_TRANSLATION_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_MACRO_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->MacroFileName != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_FUNCTION_KEYS_TXT,
								LA_ID,			GAD_MACRO_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_CURSOR_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->CursorFileName != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_CURSOR_KEYS_TXT,
								LA_ID,			GAD_CURSOR_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_FASTMACRO_STATE,
								GTCB_Checked,	LocalConfig ? (LocalConfig->FastMacroFileName != NULL) : FALSE,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_FAST_MACROS_TXT,
								LA_ID,			GAD_FASTMACRO_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_RATE_STATE,
								GTCB_Checked,	RatesValid,
								GA_Disabled,	Disabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_Chars,		LabelLen2,
								LA_LabelID,		MSG_PHONEBOOK_SETTINGS_RATES_TXT,
								LA_ID,			GAD_RATE_EDIT,
								GA_Disabled,	Disabled,
								LABT_Smaller,	TRUE,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Previous,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_FIRST_ENTRY,
					GA_Disabled,		Disabled || IsFirst,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Rewind,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_PREVIOUS_ENTRY,
					GA_Disabled,		Disabled || IsFirst,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Play,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_NEXT_ENTRY,
					GA_Disabled,		Disabled || IsLast,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Next,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_LAST_ENTRY,
					GA_Disabled,		Disabled || IsLast,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_LabelID,			MSG_GLOBAL_CLOSE_TXT,
					LABT_EscKey,		TRUE,
					LABT_ExtraFat,		TRUE,
					LA_ID,				GAD_CLOSE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_PHONEBOOK_EDIT_TXT,
			LAWN_UserPort,		Context->WindowPort,
			LAWN_HelpHook,		&GuideHook,
			WA_DepthGadget, 	TRUE,
			WA_DragBar, 		TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		Activate,
			WA_SimpleRefresh,	TRUE,
			LAWN_Parent,		Context->Window,
		TAG_DONE))
		{
			if(Activate)
			{
				LT_ShowWindow(Handle,TRUE);

				if(!Disabled)
					LT_Activate(Handle,GAD_SYSTEM_NAME);
			}

			return(Handle);
		}
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

	/* PhoneListViewRender():
	 *
	 *	This callback routine is responsible for rendering
	 *	the single listview entries.
	 */

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
PhoneListViewRender(REG(a0) struct Hook *Hook,REG(a2) struct PhoneNode *Node,REG(a1) struct LVDrawMsg *Msg)
#else
AROS_UFH3(STATIC ULONG, PhoneListViewRender,
 AROS_UFHA(struct Hook *        , Hook , A0),
 AROS_UFHA(struct PhoneNode *   , Node, A2),
 AROS_UFHA(struct LVDrawMsg *   , Msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
		/* We only know how to redraw lines. */

	if(Msg->lvdm_MethodID == LV_DRAW)
	{
		struct RastPort	*RPort	= Msg->lvdm_RastPort;
		LONG			 Count	= Node->Entry->Count + 1,
						 Left	= Msg->lvdm_Bounds.MinX,
						 Top	= Msg->lvdm_Bounds.MinY,
						 Width	= Msg->lvdm_Bounds.MaxX - Msg->lvdm_Bounds.MinX + 1,
						 Height	= Msg->lvdm_Bounds.MaxY - Msg->lvdm_Bounds.MinY + 1,
						 FgPen,BgPen,
						 Template,
						 Len,LabelLen;
		STRPTR			 String;
		UBYTE			 Num[8];
		UWORD			*Pens;

		Pens = Msg->lvdm_DrawInfo->dri_Pens;

			/* Determine the rendering pens. */

		if(Msg->lvdm_State == LVR_SELECTED)
		{
			FgPen = FILLTEXTPEN;
			BgPen = FILLPEN;
		}
		else
		{
			FgPen = TEXTPEN;
			BgPen = BACKGROUNDPEN;
		}

		FgPen = Pens[FgPen];
		BgPen = Pens[BgPen];

		if(Count > 0)
		{
			if(Pens[HIGHLIGHTTEXTPEN] != BgPen)
				FgPen = Pens[HIGHLIGHTTEXTPEN];
		}

			/* Set the rendering pens. */

		SetABPenDrMd(RPort,BgPen,BgPen,JAM2);

		RectFill(RPort,Left,Top,Left + 1,Top + Height - 1);

		Left	+= 2;
		Width	-= 2;

			/* Determine maximum index text length. */

		Template = 3 * ((ULONG)Hook->h_Data) + TextLength(RPort," - ",3);

			/* Get the vanilla name. */

		String = &Node->Node.ln_Name[6];

			/* Has this entry been selected? */

		if(Count > 0)
		{
			LONG Delta;

				/* Set up the header. */

			LimitedSPrintf(sizeof(Num),Num,"%3ld - ",Count);

				/* Determine header length. */

			Len = TextLength(RPort,Num,strlen(Num));

				/* Fill the space to precede the header. */

			if((Delta = Template - Len) > 0)
			{
				SetAPen(RPort,BgPen);
				FillBox(RPort,Left,Top,Delta,Height);
			}

				/* Render the header, right-justified. */

			SetAPen(RPort,FgPen);

			PlaceText(RPort,Left + Delta,Top,Num,strlen(Num));
		}
		else
		{
				/* Fill the blank space to precede the vanilla name. */

			SetAPen(RPort,BgPen);
			FillBox(RPort,Left,Top,Template,Height);
			SetAPen(RPort,FgPen);
		}

			/* Adjust width and area left edge. */

		Left	+= Template;
		Width	-= Template;

			/* Determine length of vanilla name. */

		LabelLen = strlen(String);

			/* Try to make it fit. */

		LabelLen = FitText(RPort,Width,String,LabelLen);

			/* Print the vanilla name if possible. */

		if(LabelLen)
		{
			Len = TextLength(RPort,String,LabelLen);

			PlaceText(RPort,Left,Top,String,LabelLen);

			Left	+= Len;
			Width	-= Len;
		}

			/* Fill the area to follow the vanilla name. */

		if(Width > 0)
		{
			SetAPen(RPort,BgPen);
			FillBox(RPort,Left,Top,Width,Height);
		}

			/* If the item happens to be disabled, draw the cross-hatch
			 * pattern across it.
			 */

		ListViewStateFill(Msg);

		return(LVCB_OK);
	}
	else
		return(LVCB_UNKNOWN);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

LayoutHandle *
CreateManagerHandle(PhoneListContext *Context,struct Window *Parent)
{
	STATIC WORD LabelTable[] =
	{
		MSG_PHONEBOOK_TAG_ALL_TXT,
		MSG_PHONEBOOK_TOGGLE_ALL_TXT,
		MSG_PHONEBOOK_UNTAG_ALL_TXT,
		MSG_PHONEBOOK_TAG_BY_PATTERN_TXT,

		MSG_PHONEBOOK_EDIT_ENTRY_TXT,
		MSG_PHONEBOOK_COPY_ENTRY_TXT,
		MSG_PHONEBOOK_ADD_ENTRY_TXT,
		MSG_PHONEBOOK_MAKE_GROUP_TXT,

		MSG_PHONEBOOK_DIAL_TXT,
		MSG_PHONEBOOK_USE_ENTRY_TXT,
		MSG_PHONEBOOK_DELETE_ENTRY_TXT,
		MSG_PHONEBOOK_SPLIT_GROUP_TXT,

		-1
	};

	struct List *PhoneList;
	ULONG Count;
	ULONG *Grouper;
	LayoutHandle *Handle;
	PhonebookHandle *PhoneHandle;

	PhoneList = Context->CurrentList;
	Count = Context->PhoneHandle->DialMarker;
	Grouper = &Context->PhoneHandle->DefaultGroup;
	PhoneHandle = Context->PhoneHandle;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		STATIC struct Hook ListViewHook;

		LONG		i,Max,Size;
		UBYTE		SmallString[2];
		struct IBox	Bounds;
		LONG		Len,LabelLen;

		InitHook(&ListViewHook,(HOOKFUNC)PhoneListViewRender,NULL);

		for(i = LabelLen = 0 ; i < NUM_ELEMENTS(LabelTable) ; i++)
		{
			Len = LT_LabelChars(Handle,LocaleString(LabelTable[i]));

			if(Len > LabelLen)
				LabelLen = Len;
		}

		SmallString[1]	= 0;
		Max				= 0;

		for(i = '0' ; i <= '9' ; i++)
		{
			SmallString[0] = i;

			if((Size = LT_LabelWidth(Handle,SmallString)) > Max)
				Max = Size;
		}

		ListViewHook.h_Data = (APTR)Max;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_NoIndent,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LONG NumEntries;
					LONG NumColumns;
					BOOL HasEntries;

					if(PhoneList && !IsListEmpty(PhoneList))
					{
						struct Node	*Node;
						LONG		 Len;

						HasEntries = TRUE;

						for(Node = PhoneList->lh_Head, NumEntries = NumColumns = 0 ; Node->ln_Succ ; Node = Node->ln_Succ)
						{
							NumEntries++;

							Len = strlen(&Node->ln_Name[6]);

							if(Len > NumColumns)
								NumColumns = Len;
						}

						if(NumEntries < 6)
							NumEntries = 6;
						else
						{
							if(NumEntries > 46)
								NumEntries = 46;
						}

						if(NumColumns < 46)
							NumColumns = 46;
					}
					else
					{
						HasEntries = FALSE;
						NumEntries = 6;
						NumColumns = 46;
					}

					LT_New(Handle,
						LA_Type,		CycleType,
						LACY_TabKey,	TRUE,
						GTCY_Labels,	Context->GroupLabels,
						LA_ID,			GAD_GROUPID,
						LA_ULONG,		Grouper,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		LISTVIEW_KIND,
						LA_ID,			GAD_NAMELIST,
						LALV_Columns,	46,
						LALV_Lines, 	6,
						LALV_CursorKey, TRUE,
						LALV_ResizeX,	TRUE,
						LALV_ResizeY,	TRUE,
						LALV_MaxGrowX,	NumColumns,
						LALV_MaxGrowY,	NumEntries,
						LALV_Link,		NIL_LINK,
						GTLV_CallBack,	&ListViewHook,
						GTLV_MaxPen,	GetListMaxPen(Handle->DrawInfo->dri_Pens),
						GTLV_Labels,	PhoneList,

						Kick30 ? TAG_IGNORE :	LALV_TextAttr,	~0,
						Kick30 ? GA_Disabled :	TAG_IGNORE,		!HasEntries,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			BOX_KIND,
						LA_ID,				GAD_COMMENTLINE,
						LABX_Rows,			1,
						LA_Chars,			50,
						LABX_DrawBox,		TRUE,
						LABX_ReserveSpace,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						LAGR_Spread,	TRUE,
						LAGR_SameSize,	TRUE,
						LAGR_Spread,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Previous,
							LATD_Smaller,		TRUE,
							LA_Chars,			LabelLen,
							LA_ID,				GAD_MOVE_FIRST,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Rewind,
							LATD_Smaller,		TRUE,
							LA_ID,				GAD_MOVE_UP,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Play,
							LATD_Smaller,		TRUE,
							LA_ID,				GAD_MOVE_DOWN,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Next,
							LATD_Smaller,		TRUE,
							LA_ID,				GAD_MOVE_LAST,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						LAGR_NoIndent,	TRUE,
						LAGR_SameSize,	TRUE,
						LAGR_Spread,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_TAG_ALL_TXT,
							LA_ID,			GAD_SELECT_ALL,
							GA_Disabled,	!PhoneHandle->NumPhoneEntries || Count == PhoneHandle->NumPhoneEntries,
							LA_Chars,		LabelLen,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_TOGGLE_ALL_TXT,
							LA_ID,			GAD_TOGGLE_ALL,
							GA_Disabled,	!PhoneHandle->NumPhoneEntries,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_UNTAG_ALL_TXT,
							LA_ID,			GAD_CLEAR_ALL,
							GA_Disabled,	!Count,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_TAG_BY_PATTERN_TXT,
							LA_ID,			GAD_PATTERN,
							GA_Disabled,	!PhoneHandle->NumPhoneEntries,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						LAGR_NoIndent,	TRUE,
						LAGR_SameSize,	TRUE,
						LAGR_Spread,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_EDIT_ENTRY_TXT,
							LA_Chars,		LabelLen,
							LA_ID,			GAD_EDIT,
							GA_Disabled,	TRUE,
							LABT_DefaultCorrection,	TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_COPY_ENTRY_TXT,
							LA_ID,			GAD_COPY,
							GA_Disabled,	TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_ADD_ENTRY_TXT,
							LA_ID,			GAD_ADD,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_MAKE_GROUP_TXT,
							LA_ID,			GAD_MAKE_GROUP,
							GA_Disabled,	!Count,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						LAGR_NoIndent,	TRUE,
						LAGR_SameSize,	TRUE,
						LAGR_Spread,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_DIAL_TXT,
							LA_ID,			GAD_DIAL,
							GA_Disabled,	!Count || (Online && !Config->MiscConfig->ProtectiveMode),
							LABT_ReturnKey, TRUE,
							LA_Chars,		LabelLen,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_USE_ENTRY_TXT,
							LA_ID,			GAD_USE,
							GA_Disabled,	TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_DELETE_ENTRY_TXT,
							LA_ID,			GAD_DELETE,
							GA_Disabled,	TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		MSG_PHONEBOOK_SPLIT_GROUP_TXT,
							LA_ID,			GAD_SPLIT_GROUP,
							GA_Disabled,	*Grouper == 0,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	YBAR_KIND,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
					LAGR_SameSize,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_PHONEPANEL_AUTO_DIAL_TXT,
						LA_ID,			GAD_MAIN_AUTODIAL,
						LA_BYTE,		&PhoneHandle->AutoDial,
						LA_LabelPlace,	PLACE_Right,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_PHONEPANEL_EXIT_WHEN_FINISHED_TXT,
						LA_ID,			GAD_MAIN_EXIT_WHEN_FINISHED,
						LA_BYTE,		&PhoneHandle->AutoExit,
						GA_Disabled,	!PhoneHandle->AutoDial,
						LA_LabelPlace,	PLACE_Right,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_PASSWORD_TXT,
						LA_ID,			GAD_PASSWORD,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_LOAD_TXT,
						LA_ID,			GAD_LOAD,
						GA_Disabled,	GetActiveEntry(PhoneHandle) != NULL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_MERGE_TXT,
						LA_ID,			GAD_MERGE,
						GA_Disabled,	!PhoneHandle->NumPhoneEntries,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_SAVE_TXT,
						LA_ID,			GAD_SAVE,
						GA_Disabled,	!PhoneHandle->NumPhoneEntries,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_PRINT_TXT,
						LA_ID,			GAD_PRINT,
						GA_Disabled,	!PhoneHandle->NumPhoneEntries,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_SORT_TXT,
						LA_ID,			GAD_SORT,
						GA_Disabled,	!PhoneHandle->NumPhoneEntries,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_PHONEBOOK_COPY_CONFIG_TXT,
						LA_ID,			GAD_COPY_CONFIG,
						GA_Disabled,	!PhoneHandle->NumPhoneEntries,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(Parent == Window)
		{
			Bounds.Left		= WindowLeft	+ Window->LeftEdge;
			Bounds.Top		= WindowTop		+ Window->TopEdge;
			Bounds.Width	= WindowWidth;
			Bounds.Height	= WindowHeight;
		}

		if(LT_Build(Handle,
			LAWN_TitleText,		Context->WindowTitle,
			LAWN_IDCMP, 		IDCMP_CLOSEWINDOW,
			LAWN_Bounds,		&Bounds,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Show,			TRUE,
			LAWN_UserPort,		Context->WindowPort,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_CloseGadget, 	TRUE,
		/*	WA_SimpleRefresh,	TRUE, */
			WA_NoCareRefresh,	TRUE,

			Parent == Window ? LAWN_Bounds : TAG_IGNORE,&Bounds,
			Parent == Window ? TAG_IGNORE : LAWN_Parent,Parent,
		TAG_DONE))
			return(Handle);
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

LayoutHandle *
CreateSelectorHandle(struct Window *Parent,STRPTR Name,STRPTR Number,STRPTR Comment,BYTE *Mode)
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		CYCLE_KIND,
					LA_ID,			GAD_SELECT_MODE,
					LA_Chars,		40,
					LA_BYTE,		Mode,
					LACY_TabKey,	TRUE,
					LACY_FirstLabel,MSG_PHONEBOOK_TAG_ENTRIES1_TXT,
					LACY_LastLabel,	MSG_PHONEBOOK_TAG_ENTRIES2_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_SELECT_NAME,
					LA_LabelID,		MSG_PHONEBOOK_SYSTEM_NAME_TXT,
					LA_STRPTR,		Name,
					GTST_MaxChars,	structsizeof(PhoneHeader,Name) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_SELECT_COMMENT,
					LA_LabelID,		MSG_PHONEBOOK_COMMENT_TXT,
					LA_STRPTR,		Comment,
					LAST_LastGadget,TRUE,
					GTST_MaxChars,	structsizeof(PhoneHeader,Comment) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_SELECT_NUMBER,
					LA_LabelID,		MSG_PHONEBOOK_NUMBERS_TXT,
					LA_STRPTR,		Number,
					GTST_MaxChars,	structsizeof(PhoneHeader,Number) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SELECT_APPLY,
					LA_LabelID,			MSG_PHONEBOOK_APPLY_TXT,
					LABT_ExtraFat,		TRUE,
					LABT_ReturnKey,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SELECT_APPLY_AND_CLOSE,
					LA_LabelID,			MSG_PHONEBOOK_APPLY_AND_CLOSE_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SELECT_CLOSE,
					LA_LabelID,			MSG_GLOBAL_CLOSE_TXT,
					LABT_EscKey,		TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_PHONEBOOK_EDIT_PATTERNS_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Show,			TRUE,
			LAWN_Parent,		Parent,
			LAWN_UserPort,		Parent->UserPort,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
			return(Handle);
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

LayoutHandle *
CreateGroupHandle(struct Window *Parent,STRPTR *Labels,ULONG Group)
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		CycleType,
					LA_ID,			GAD_GROUP_LIST,
					GTCY_Labels,	Labels,
					GTCY_Active,	Group,
					LACY_TabKey,	TRUE,
					LA_LabelID,		MSG_PHONEBOOK_PICK_GROUP_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_GROUP_NAME,
					LA_LabelID,		MSG_PHONEBOOK_OR_ENTER_NAME_TXT,
					LA_Chars,		30,
					GTST_MaxChars,	structsizeof(PhoneGroupNode,LocalName) - 1,
					GTST_String,	Labels[Group],
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_GROUP_OK,
					LA_LabelID,			MSG_GLOBAL_USE_GAD,
					LABT_ExtraFat,		TRUE,
					LABT_ReturnKey,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_GROUP_CANCEL,
					LA_LabelID,			MSG_GLOBAL_CANCEL_GAD,
					LABT_EscKey,		TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_PHONEBOOK_SELECT_GROUP_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Show,			TRUE,
			LAWN_Parent,		Parent,
			LAWN_UserPort,		Parent->UserPort,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			LT_Activate(Handle,GAD_GROUP_NAME);

			return(Handle);
		}
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

LayoutHandle *
CreateSortHandle(struct Window *Parent,WORD *Criteria,BOOL *Reverse)
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		LT_New(Handle,
			LA_Type,	HORIZONTAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		MX_KIND,
						LA_ID,			GAD_SORT_CRITERIA,
						LAMX_FirstLabel,MSG_PHONEBOOK_SORTORDER1_TXT,
						LAMX_LastLabel,	MSG_PHONEBOOK_SORTORDER4_TXT,
						LAMX_TabKey,	TRUE,
						LA_WORD,		Criteria,
						LA_LabelID,		MSG_SORT_CRITERIA_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_ID,			GAD_SORT_REVERSE,
						LA_LabelID,		MSG_REVERSE_ORDER_TXT,
						LA_BOOL,		Reverse,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		YBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SORT_IT,
					LA_LabelID,			MSG_JUST_SORT_TXT,
					LABT_ReturnKey,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SORT_AND_CLOSE,
					LA_LabelID,			MSG_SORT_AND_CLOSE_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_ID,				GAD_SORT_JUST_CLOSE,
					LA_LabelID,			MSG_GLOBAL_CLOSE_TXT,
					LABT_EscKey,		TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_SORT_PHONEBOOK_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Show,			TRUE,
			LAWN_Parent,		Parent,
			LAWN_UserPort,		Parent->UserPort,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
			return(Handle);
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

VOID
UpdatePasswordWindow(LayoutHandle *Handle)
{
	UBYTE	Password1[MAX_PASSWORD_LENGTH+1],
			Password2[MAX_PASSWORD_LENGTH+1];
	BOOL	Disabled;

	memset(Password1,0,sizeof(Password1));
	memset(Password2,0,sizeof(Password2));

	strcpy(Password1,LT_GetString(Handle,GAD_NEW_PASSWORD));
	strcpy(Password2,LT_GetString(Handle,GAD_REPEAT_PASSWORD));

	if(Password1[0] && !memcmp(Password1,Password2,MAX_PASSWORD_LENGTH))
		Disabled = FALSE;
	else
		Disabled = TRUE;

	LT_SetAttributes(Handle,GAD_USE_NEW_PASSWORD,
		GA_Disabled,Disabled,
	TAG_DONE);
}

BOOL
GetWindowPassword(LayoutHandle *Handle,STRPTR Password)
{
	UBYTE	Password1[MAX_PASSWORD_LENGTH+1],
			Password2[MAX_PASSWORD_LENGTH+1];

	memset(Password1,0,sizeof(Password1));
	memset(Password2,0,sizeof(Password2));

	strcpy(Password1,LT_GetString(Handle,GAD_NEW_PASSWORD));
	strcpy(Password2,LT_GetString(Handle,GAD_REPEAT_PASSWORD));

	if(Password1[0] && !memcmp(Password1,Password2,MAX_PASSWORD_LENGTH+1))
	{
		strcpy(Password,Password1);

		return(TRUE);
	}
	else
	{
		LONG ID;

		LT_SetAttributes(Handle,GAD_USE_NEW_PASSWORD,
			GA_Disabled,TRUE,
		TAG_DONE);

		if(!Password2[0])
			ID = GAD_REPEAT_PASSWORD;
		else
			ID = GAD_NEW_PASSWORD;

		LT_Activate(Handle,ID);

		DisplayBeep(Handle->Window->WScreen);

		return(FALSE);
	}
}

LayoutHandle *
CreatePasswordHandle(struct Window *Parent,BOOL PasswordEnabled)
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_AutoActivate,	TRUE,
	TAG_DONE))
	{
		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		PASSWORD_KIND,
					LA_LabelID,		MSG_ENTER_PASSWORD_GAD,
					LA_ID,			GAD_NEW_PASSWORD,
					LA_Chars,		20,
					GTST_MaxChars,	MAX_PASSWORD_LENGTH + 1,
					LAST_Activate,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		PASSWORD_KIND,
					LA_LabelID,		MSG_REPEAT_PASSWORD_GAD,
					LA_ID,			GAD_REPEAT_PASSWORD,
					GTST_MaxChars,	MAX_PASSWORD_LENGTH + 1,
					LAST_LastGadget,TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_USE_NEWPASSWORD_GAD,
					LA_ID,			GAD_USE_NEW_PASSWORD,
					GA_Disabled,	TRUE,
					LABT_ExtraFat,	TRUE,
					LABT_ReturnKey,	TRUE,
				TAG_DONE);

				if(PasswordEnabled)
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_CLEAR_PASSWORD_GAD,
						LA_ID,			GAD_CLEAR_PASSWORD,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);
				}

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL_PASSWORD,
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_ENTER_PASSWORD_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Show,			TRUE,
			LAWN_Parent,		Parent,
			LAWN_UserPort,		Parent->UserPort,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
			return(Handle);
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}
