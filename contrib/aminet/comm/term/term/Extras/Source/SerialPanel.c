/*
**	SerialPanel.c
**
**	Editing panel for serial configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_BAUD=1000,GAD_DATABITS,GAD_PARITY,GAD_STOPBITS,GAD_HANDSHAKING,GAD_DUPLEX,GAD_BUFFERSIZE,GAD_BREAKLENGTH,
			GAD_STRIP,GAD_HIGHSPEED,GAD_CHECK_CARRIER,GAD_SHARED,GAD_XON_XOFF,GAD_PASSTHROUGH,
			GAD_DEVICE,GAD_SELECT,GAD_UNIT,GAD_QUANTUM,GAD_SATISFY_ODU,GAD_USE_OWNDEVUNIT,GAD_RELEASE_WHEN_ONLINE,
			GAD_RELEASE_WHEN_DIALING,GAD_NO_ODU_IF_SHARED,GAD_USE,GAD_CANCEL,
			GAD_PAGER,GAD_PAGEGROUP,GAD_DIRECT_CONNECTION
		};

STATIC LONG Sizes[] =
{
	16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288
};

STATIC LONG SAVE_DS STACKARGS
ShowSerialSize(struct Gadget *UnusedGadget,LONG Level)
{
	return(Sizes[Level]);
}

#ifndef __AROS__
STATIC LONG SAVE_DS ASM
RateHookFunc(REG(a0) struct Hook *UnusedHook,REG(a2) LONG Current,REG(a1) LONG Command)
#else
AROS_UFH3(STATIC LONG, RateHookFunc,
 AROS_UFHA(struct Hook *   , UnusedHook , A0),
 AROS_UFHA(LONG            , Current, A2),
 AROS_UFHA(LONG            , Command, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	LONG Index;

	if(Current < BaudRates[0])
		Current = BaudRates[Index = 0];
	else
	{
		if(Current > BaudRates[NumBaudRates - 1])
			Current = BaudRates[Index = NumBaudRates - 1];
		else
		{
			LONG i;

			Index = -1;

			for(i = 0 ; i < NumBaudRates ; i++)
			{
				if(Current == BaudRates[i])
				{
					switch(Command)
					{
						case INCREMENTERMSG_Decrement:

							if(i > 0)
								return(BaudRates[i - 1]);
							else
								return(Current);

						case INCREMENTERMSG_Increment:

							if(i < NumBaudRates - 1)
								return(BaudRates[i + 1]);
							else
								return(Current);

						default:

							return(Current);
					}
				}
			}

			if(Index == -1)
			{
				if(Command == INCREMENTERMSG_Decrement)
				{
					for(i = 0 ; i < NumBaudRates ; i++)
					{
						if(BaudRates[i] <= Current)
							Index = i;
					}

					if(BaudRates[Index] < Current || !Index)
						return(BaudRates[Index]);
					else
						return(BaudRates[Index - 1]);
				}
				else
				{
					for(i = NumBaudRates - 1 ; i >= 0 ; i--)
					{
						if(BaudRates[i] >= Current)
							Index = i;
					}

					if(Command == INCREMENTERMSG_Initial)
					{
						if(Index == -1)
							return(Current);
						else
							return(BaudRates[Index]);
					}
					else
					{
						if(BaudRates[Index] > Current || Index == NumBaudRates - 1)
							return(BaudRates[Index]);
						else
							return(BaudRates[Index + 1]);
					}
				}
			}
		}
	}

	if(Index == -1 || (Command != INCREMENTERMSG_Decrement && Command != INCREMENTERMSG_Increment))
		return(Current);
	else
	{
		if(Command == INCREMENTERMSG_Decrement)
		{
			if(Index > 0)
				return(BaudRates[Index - 1]);
			else
				return(BaudRates[Index]);
		}
		else
		{
			if(Index < NumBaudRates - 1)
				return(BaudRates[Index + 1]);
			else
				return(BaudRates[Index]);
		}
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

BOOL
SerialPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC struct Hook IncrementerHook;

	STATIC LONG PageTitleID[5] =
	{
		MSG_V36_0171,
		MSG_V36_0179,
		MSG_V36_0176,
		MSG_ODU_SUPPORT_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	LayoutHandle *Handle;
	STRPTR PageLabels[5];
	BOOL UseIt;

	UseIt = FALSE;

	InitHook(&IncrementerHook,(HOOKFUNC)RateHookFunc,NULL);

	memset(PageLabels,0,sizeof(PageLabels));

	LocalizeStringTable(PageLabels,PageTitleID);

	if(!OwnDevUnitBase)
	{
		PageLabels[3] = NULL;

		if(InitialPage >= 3)
			InitialPage = 2;
	}

	SaveConfig(LocalConfig,PrivateConfig);

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		LONG Size,Quantum,i;

		for(i = 4 ; i < NUM_ELEMENTS(Sizes) ; i++)
		{
			if(Sizes[i] <= PrivateConfig->SerialConfig->SerialBufferSize)
				Size = i;
		}

		for(i = 0 ; i < NUM_ELEMENTS(Sizes) ; i++)
		{
			if(Sizes[i] <= PrivateConfig->SerialConfig->Quantum)
				Quantum = i;
		}

		if(PrivateConfig->SerialConfig->HighSpeed)
		{
			PrivateConfig->SerialConfig->BitsPerChar	= 8;
			PrivateConfig->SerialConfig->StopBits		= 1;
			PrivateConfig->SerialConfig->Parity			= PARITY_NONE;
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
					GTCY_Labels,	PageLabels,
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
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_SERIALPANEL_BAUD_RATE_GAD,
						LA_LONG,				&PrivateConfig->SerialConfig->BaudRate,
						LA_ID,					GAD_BAUD,
						GTIN_MaxChars,			8,
						LAIN_Min,				0,
						LAIN_UseIncrementers,	TRUE,
						LAIN_IncrementerHook,	&IncrementerHook,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				SliderType,
						LA_ID,					GAD_DATABITS,
						LA_LabelID,				MSG_SERIALPANEL_BITS_PER_CHAR_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->BitsPerChar,
						GA_Disabled,			PrivateConfig->SerialConfig->HighSpeed,
						GTSL_Min,				7,
						GTSL_Max,				8,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CYCLE_KIND,
						LA_ID,					GAD_PARITY,
						LA_LabelID,				MSG_SERIALPANEL_PARITY_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->Parity,
						GA_Disabled,			PrivateConfig->SerialConfig->HighSpeed,
						LACY_FirstLabel,		MSG_SERIALPANEL_NO_PARITY_TXT,
						LACY_LastLabel,			MSG_SERIALPANEL_SPACE_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				SliderType,
						LA_ID,					GAD_STOPBITS,
						LA_LabelID,				MSG_SERIALPANEL_STOP_BITS_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->StopBits,
						GA_Disabled,			PrivateConfig->SerialConfig->HighSpeed,
						GTSL_Min,				1,
						GTSL_Max,				2,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CYCLE_KIND,
						LA_LabelID,				MSG_SERIALPANEL_HANDSHAKING_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->HandshakingProtocol,
						LACY_FirstLabel,		MSG_SERIALPANEL_HANDSHAKING_NONE_TXT,
						LACY_LastLabel,			MSG_SERIALPANEL_HANDSHAKING_RTS_CTS_DSR_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CHECKBOX_KIND,
						LA_LabelID,				MSG_SERIALPANEL_LOCAL_ECHO_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->Duplex,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				SliderType,
						LA_LabelID,				MSG_SERIALPANEL_BUFFER_SIZE_GAD,
						LA_LONG,				&Size,
						GTSL_Min,				4,
						GTSL_Max,				NUM_ELEMENTS(Sizes) - 1,
						GTSL_DispFunc,			ShowSerialSize,
						GTSL_LevelFormat,		LocaleBase ? "%7lD" : "%7ld",
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_SERIALPANEL_BREAK_LENGTH_GAD,
						LA_LONG,				&PrivateConfig->SerialConfig->BreakLength,
						GTIN_MaxChars,			8,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				CHECKBOX_KIND,
						LA_LabelID,				MSG_SERIALPANEL_HIGH_SPEED_MODE_GAD,
						LA_BYTE,				&PrivateConfig->SerialConfig->HighSpeed,
						LA_ID,					GAD_HIGHSPEED,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,				STRING_KIND,
						LA_LabelID,				MSG_SERIALPANEL_SERIAL_DEVICE_GAD,
						LA_Chars,				20,
						LA_STRPTR,				PrivateConfig->SerialConfig->SerialDevice,
						GTST_MaxChars,			sizeof(PrivateConfig->SerialConfig->SerialDevice) - 1,
						LAST_Picker,			TRUE,
						LA_ID,					GAD_DEVICE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				INTEGER_KIND,
						LA_LabelID,				MSG_SERIALPANEL_DEVICE_UNIT_NUMBER_GAD,
						LA_LONG,				&PrivateConfig->SerialConfig->UnitNumber,
						LA_ID,					GAD_UNIT,
						LAIN_UseIncrementers,	TRUE,
						GTIN_MaxChars,			8,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_SERIALPANEL_STRIP_BIT_8_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->StripBit8,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_SERIALPANEL_SHARED_ACCESS_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->Shared,
						LA_ID,				GAD_SHARED,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_SERIALPANEL_XON_XOFF_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->xONxOFF,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_SERIALPANEL_PASS_XON_XOFF_THROUGH_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->PassThrough,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_SERIALPANEL_CHECK_CARRIER_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->CheckCarrier,
						GA_Disabled,		PrivateConfig->SerialConfig->DirectConnection,
						LA_ID,				GAD_CHECK_CARRIER,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_DIRECT_CONNECTION_GAD,
						LA_BYTE,			&PrivateConfig->SerialConfig->DirectConnection,
						LA_ID,				GAD_DIRECT_CONNECTION,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_SERIALPANEL_QUANTUM_GAD,
						LA_LONG,			&Quantum,
						LA_Chars,			6,
						GTSL_Min,			0,
						GTSL_Max,			NUM_ELEMENTS(Sizes) - 1,
						GTSL_Level,			Quantum,
						GTSL_DispFunc,		ShowSerialSize,
						GTSL_LevelFormat,	LocaleBase ? "%7lD" : "%7ld",
					TAG_DONE);

					LT_EndGroup(Handle);
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
							LA_Type,			CHECKBOX_KIND,
							LA_LabelID,			MSG_SERIALPANEL_USE_OWNDEVUNIT_TXT,
							LA_BYTE,			&PrivateConfig->SerialConfig->UseOwnDevUnit,
							LA_ID,				GAD_USE_OWNDEVUNIT,
							LA_LabelPlace,		PLACE_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			CHECKBOX_KIND,
							LA_LabelID,			MSG_DO_NOT_USE_IF_DEVICE_IS_IN_SHARED_MODE_TXT,
							LA_BYTE,			&PrivateConfig->SerialConfig->NoODUIfShared,
							LA_ID,				GAD_NO_ODU_IF_SHARED,
							GA_Disabled,		!PrivateConfig->SerialConfig->UseOwnDevUnit,
							LA_LabelPlace,		PLACE_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			CHECKBOX_KIND,
							LA_LabelID,			MSG_RELEASE_DEVICE_WHEN_ONLINE_TXT,
							LA_BYTE,			&PrivateConfig->SerialConfig->ReleaseODUWhenOnline,
							LA_ID,				GAD_RELEASE_WHEN_ONLINE,
							GA_Disabled,		!PrivateConfig->SerialConfig->UseOwnDevUnit,
							LA_ExtraSpace,		TRUE,
							LA_LabelPlace,		PLACE_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			CHECKBOX_KIND,
							LA_LabelID,			MSG_RELEASE_DEVICE_WHEN_DIALING_TXT,
							LA_BYTE,			&PrivateConfig->SerialConfig->ReleaseODUWhenDialing,
							LA_ID,				GAD_RELEASE_WHEN_DIALING,
							GA_Disabled,		!PrivateConfig->SerialConfig->UseOwnDevUnit,
							LA_LabelPlace,		PLACE_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			MX_KIND,
							LA_LabelID,			MSG_SATISFY_ODU_TXT,
							LA_BYTE,			&PrivateConfig->SerialConfig->SatisfyODURequests,
							LA_ID,				GAD_SATISFY_ODU,
							LACY_FirstLabel,	MSG_SATISFY_ODU_MODE1_TXT,
							LACY_LastLabel,		MSG_SATISFY_ODU_MODE3_TXT,
							GA_Disabled,		!PrivateConfig->SerialConfig->UseOwnDevUnit,
							LA_ExtraSpace,		TRUE,
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
			LAWN_TitleID,		MSG_SERIALPANEL_SERIAL_PREFERENCES_TXT,
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

			GuideContext(CONTEXT_SERIAL);

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
							case GAD_USE_OWNDEVUNIT:

								LT_SetAttributes(Handle,GAD_SATISFY_ODU,
									GA_Disabled,!PrivateConfig->SerialConfig->UseOwnDevUnit,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_RELEASE_WHEN_ONLINE,
									GA_Disabled,!PrivateConfig->SerialConfig->UseOwnDevUnit,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_RELEASE_WHEN_DIALING,
									GA_Disabled,!PrivateConfig->SerialConfig->UseOwnDevUnit,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_NO_ODU_IF_SHARED,
									GA_Disabled,!PrivateConfig->SerialConfig->UseOwnDevUnit,
								TAG_DONE);

								break;

							case GAD_HIGHSPEED:

								if(MsgCode)
								{
									LT_SetAttributes(Handle,GAD_PARITY,
										GA_Disabled,	TRUE,
										GTCY_Active,	PARITY_NONE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_STOPBITS,
										GA_Disabled,	TRUE,
										GTCY_Active,	0,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_DATABITS,
										GA_Disabled,	TRUE,
										GTCY_Active,	1,
									TAG_DONE);
								}
								else
								{
									LT_SetAttributes(Handle,GAD_PARITY,
										GA_Disabled,	FALSE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_STOPBITS,
										GA_Disabled,	FALSE,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_DATABITS,
										GA_Disabled,	FALSE,
									TAG_DONE);
								}

								break;

							case GAD_DIRECT_CONNECTION:

								LT_SetAttributes(Handle,GAD_CHECK_CARRIER,
									GA_Disabled,	MsgCode,
								TAG_DONE);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								PrivateConfig->SerialConfig->SerialBufferSize	= Sizes[Size];
								PrivateConfig->SerialConfig->Quantum			= Sizes[Quantum];

								UseIt = Done = TRUE;

								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							case GAD_DEVICE:

								FindLibDev(PanelWindow,LT_GetString(Handle,MsgGadget->GadgetID),NT_DEVICE,NULL);
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadget->GadgetID == GAD_DEVICE)
					{
						UBYTE LocalBuffer[MAX_FILENAME_LENGTH],DummyBuffer[MAX_FILENAME_LENGTH];

						LT_LockWindow(PanelWindow);

						strcpy(DummyBuffer,LT_GetString(Handle,GAD_DEVICE));
						strcpy(LocalBuffer,DummyBuffer);

						if(PickFile(PanelWindow,"Devs:","#?.device",LocaleString(MSG_SERIALPANEL_SELECT_DEVICE_TXT),LocalBuffer,NT_DEVICE))
						{
							if(Stricmp(DummyBuffer,LocalBuffer))
							{
								LT_SetAttributes(Handle,GAD_DEVICE,GTST_String,LocalBuffer,TAG_DONE);

								FindLibDev(PanelWindow,LocalBuffer,NT_DEVICE,NULL);
							}
						}

						LT_UnlockWindow(PanelWindow);

						LT_ShowWindow(Handle,TRUE);
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
		if(Config->MiscConfig->ProtectiveMode)
		{
			if(PrivateConfig->SerialConfig->BaudRate >= 4800 && PrivateConfig->SerialConfig->HandshakingProtocol == HANDSHAKING_NONE && !PrivateConfig->SerialConfig->DirectConnection)
			{
				if(ShowRequest(Window,LocaleString(MSG_NO_RTSCTS_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),PrivateConfig->SerialConfig->BaudRate))
					PrivateConfig->SerialConfig->HandshakingProtocol = HANDSHAKING_RTSCTS_DSR;
			}
		}

			/* Is the device driver closed up and is the main
			 * program making the call?
			 */

		if(LocalConfig == Config && !ReadRequest)
		{
				/* Is the serial configuration any different?
				 * If not, zap the serial device name in the
				 * alternate buffer to force the driver to
				 * get reopened.
				 */

			if(!CompareConfigEntries(Config->SerialConfig,PrivateConfig->SerialConfig,PREF_SERIAL))
				Config->SerialConfig->SerialDevice[0] = 0;
		}
	}

		/* The edited settings are now in PrivateConfig */

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}
