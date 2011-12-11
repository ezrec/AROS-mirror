/*
**	ModemPanel.c
**
**	Editing panel for modem configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIMIT_MACRO=1000,GAD_DIAL_PREFIX,GAD_DIAL_SUFFIX,
			GAD_DIAL_MODE,GAD_USE,GAD_CANCEL,
			GAD_PAGER,GAD_PAGEGROUP,GAD_INTER_DIAL_DELAY,
			GAD_PBX_PREFIX,GAD_PBX
		};

STATIC CONST_STRPTR SAVE_DS STACKARGS
ShowDialRetries(struct Gadget *UnusedGadget,LONG Level)
{
	if(Level > 1000)
		return(LocaleString(MSG_DIAL_RETRIES_UNLIMITED_TXT));
	else
	{
		STATIC UBYTE Buffer[10];

		LimitedSPrintf(sizeof(Buffer),Buffer,"%ld",Level);

		return(Buffer);
	}
}

STATIC STRPTR SAVE_DS STACKARGS
ShowHours(struct Gadget *UnusedGadget,LONG Level)
{
	STATIC UBYTE Buffer[15];

	LimitedSPrintf(sizeof(Buffer),Buffer,LocaleString(MSG_GLOBAL_SHOW_HOURS_TXT),Level / 60,Level % 60);

	return(Buffer);
}

STATIC STRPTR SAVE_DS STACKARGS
ModemShowSeconds(struct Gadget *UnusedGadget,LONG Level)
{
	STATIC UBYTE Buffer[15];

	LimitedSPrintf(sizeof(Buffer),Buffer,LocaleString(MSG_GLOBAL_SHOW_MINUTES_TXT),Level / 60,Level % 60);

	return(Buffer);
}

BOOL
ModemPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[7] =
	{
		MSG_V36_0110,
		MSG_V36_0111,
		MSG_DIALCOMMANDS_TXT,
		MSG_V36_0112,
		MSG_V36_0116,
		MSG_V36_0118,
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
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_MODEM_INIT_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->ModemInit,
						LA_Chars,		20,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->ModemInit) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_MODEM_EXIT_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->ModemExit,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->ModemExit) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_MODEM_HANGUP_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->ModemHangup,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->ModemHangup) - 1,
					TAG_DONE);

					if(LocalConfig == Config)
					{
						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_DO_NOT_SEND_MODEM_COMMANDS_TXT,
							LA_BYTE,		&PrivateConfig->ModemConfig->DoNotSendMainModemCommands,
							LA_ExtraSpace,	TRUE,
						TAG_DONE);
					}

					LT_New(Handle,
						LA_Type,	XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_COMMAND_SEND_DELAY_TXT,
						LA_LONG,				&PrivateConfig->ModemConfig->CharSendDelay,
						LAIN_Min,				0,
						LAIN_UseIncrementers,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_NO_CARRIER_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->NoCarrier,
						LA_Chars,		20,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->NoCarrier) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_NO_DIALTONE_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->NoDialTone,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->NoDialTone) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_CONNECT_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Connect,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Connect) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_VOICE_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Voice,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Voice) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_RING_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Ring,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Ring) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_BUSY_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Busy,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Busy) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_OK_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Ok,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Ok) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_ERROR_STRING_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->Error,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->Error) - 1,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					STATIC LONG DialLabels[] =
					{
						MSG_DIALMODE1_TXT,
						MSG_DIALMODE2_TXT,
						MSG_DIALMODE3_TXT,
						MSG_DIALMODE4_TXT,
						-1
					};

					BOOL NotFound;
					LONG i;

					for(i = 0, NotFound = TRUE ; NotFound && i < strlen(PrivateConfig->ModemConfig->DialPrefix) - 1 ; i++)
					{
						if(PrivateConfig->ModemConfig->DialPrefix[i] == '\\' && ToUpper(PrivateConfig->ModemConfig->DialPrefix[i + 1]) == 'W')
							NotFound = FALSE;
					}

					for(i = 0 ; NotFound && i < strlen(PrivateConfig->ModemConfig->DialSuffix) - 1 ; i++)
					{
						if(PrivateConfig->ModemConfig->DialSuffix[i] == '\\' && ToUpper(PrivateConfig->ModemConfig->DialSuffix[i + 1]) == 'W')
							NotFound = FALSE;
					}

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_DIAL_PREFIX_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->DialPrefix,
						LA_Chars,		20,
						LA_ID,			GAD_DIAL_PREFIX,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->DialPrefix) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_MODEMPANEL_DIAL_SUFFIX_GAD,
						LA_STRPTR,		PrivateConfig->ModemConfig->DialSuffix,
						LA_ID,			GAD_DIAL_SUFFIX,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->DialSuffix) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_DIALMODE2_GAD,
						LA_BYTE,		&PrivateConfig->ModemConfig->DialMode,
						LA_ID,			GAD_DIAL_MODE,
						LACY_LabelTable,DialLabels,
						GA_Disabled,	NotFound,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PBX_DIAL_PREFIX_TXT,
						LA_STRPTR,		PrivateConfig->ModemConfig->PBX_Prefix,
						LA_ID,			GAD_PBX_PREFIX,
						GTST_MaxChars,	sizeof(PrivateConfig->ModemConfig->PBX_Prefix) - 1,
						LA_ExtraSpace,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_PBX_DIAL_MODE_TXT,
						LA_BYTE,		&PrivateConfig->ModemConfig->PBX_Mode,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MODEMPANEL_REDIAL_DELAY_GAD,
						LA_LONG,			&PrivateConfig->ModemConfig->RedialDelay,
						GTSL_Min,			0,
						GTSL_Max,			60 * 60,
						GTSL_DispFunc,		ModemShowSeconds,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					if(PrivateConfig->ModemConfig->DialRetries < 0)
						PrivateConfig->ModemConfig->DialRetries = 1001;

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MODEMPANEL_DIAL_RETRIES_GAD,
						LA_LONG,			&PrivateConfig->ModemConfig->DialRetries,
						GTSL_DispFunc,		ShowDialRetries,
						GTSL_Min,			0,
						GTSL_Max,			1001,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MODEMPANEL_DIAL_TIMEOUT_GAD,
						LA_LONG,			&PrivateConfig->ModemConfig->DialTimeout,
						GTSL_Min,			0,
						GTSL_Max,			1800,
						GTSL_DispFunc,		ModemShowSeconds,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_INTER_DIAL_DELAY_TXT,
						LA_LONG,			&PrivateConfig->ModemConfig->InterDialDelay,
						GTSL_Min,			0,
						GTSL_Max,			1800,
						GTSL_DispFunc,		ModemShowSeconds,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_VERBOSE_DIALING_TXT,
						LA_BYTE,			&PrivateConfig->ModemConfig->VerboseDialing,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_MODEMPANEL_NOCARRIER_IS_BUSY_GAD,
						LA_BYTE,			&PrivateConfig->ModemConfig->NoCarrierIsBusy,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_MODEMPANEL_CONNECT_AUTO_BAUD_GAD,
						LA_BYTE,			&PrivateConfig->ModemConfig->ConnectAutoBaud,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_MODEMPANEL_ABORT_HANGS_UP_TXT,
						LA_BYTE,			&PrivateConfig->ModemConfig->AbortHangsUp,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MODEMPANEL_TIME_TO_CONNECT_GAD,
						LA_LONG,			&PrivateConfig->ModemConfig->TimeToConnect,
						LA_Chars,			8,
						GTSL_Min,			0,
						GTSL_Max,			60 * 60,
						GTSL_DispFunc,		ModemShowSeconds,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_MODEMPANEL_DROP_DTR_ON_HANGUP_GAD,
						LA_BYTE,		&PrivateConfig->ModemConfig->DropDTR,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_MODEMPANEL_REDIAL_AFTER_HANGUP_GAD,
						LA_BYTE,		&PrivateConfig->ModemConfig->RedialAfterHangup,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_MODEMPANEL_CONNECTION_LIMIT_GAD,
						LA_WORD,			&PrivateConfig->ModemConfig->ConnectLimit,
						GTSL_Min,			0,
						GTSL_Max,			1440,
						GTSL_DispFunc,		ShowHours,
						GTSL_LevelFormat,	"%s",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			STRING_KIND,
						LA_LabelID,			MSG_MODEMPANEL_LIMIT_MACRO_GAD,
						LA_STRPTR,			PrivateConfig->ModemConfig->ConnectLimitMacro,
						LA_Chars,			20,
						LA_ID,				GAD_LIMIT_MACRO,
						GTST_MaxChars,		sizeof(PrivateConfig->ModemConfig->ConnectLimitMacro) - 1,
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
			LAWN_TitleID,		MSG_MODEMPANEL_MODEM_PREFERENCES_TXT,
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
			BOOL NotFound;
			BOOL Done;
			LONG i;

			GuideContext(CONTEXT_MODEM);

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
							case GAD_DIAL_PREFIX:
							case GAD_DIAL_SUFFIX:
							case GAD_DIAL_MODE:

								for(i = 0, NotFound = TRUE ; NotFound && i < strlen(PrivateConfig->ModemConfig->DialPrefix) - 1 ; i++)
								{
									if(PrivateConfig->ModemConfig->DialPrefix[i] == '\\' && ToUpper(PrivateConfig->ModemConfig->DialPrefix[i + 1]) == 'W')
										NotFound = FALSE;
								}

								for(i = 0 ; NotFound && i < strlen(PrivateConfig->ModemConfig->DialSuffix) - 1 ; i++)
								{
									if(PrivateConfig->ModemConfig->DialSuffix[i] == '\\' && ToUpper(PrivateConfig->ModemConfig->DialSuffix[i + 1]) == 'W')
										NotFound = FALSE;
								}

								LT_SetAttributes(Handle,GAD_DIAL_MODE,
									GA_Disabled,NotFound,
								TAG_DONE);

								break;

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
	{
		if(PrivateConfig->ModemConfig->DialRetries > 1000)
			PrivateConfig->ModemConfig->DialRetries = -1;

		if(Config->MiscConfig->ProtectiveMode && PrivateConfig->SerialConfig->BaudRate >= 4800 && PrivateConfig->ModemConfig->ConnectAutoBaud)
		{
			if(ShowRequest(Window,LocaleString(MSG_AUTOBAUD_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT)))
				PrivateConfig->ModemConfig->ConnectAutoBaud = FALSE;
		}
	}

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}
