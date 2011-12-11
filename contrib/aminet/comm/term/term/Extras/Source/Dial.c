/*
**	Dial.c
**
**	The dialing routine as called by the phonebook
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Panel gadget IDs. */

enum	{	GAD_CALLING=1,GAD_TIME,GAD_NOTE,
			GAD_SKIP,GAD_REMOVE,GAD_ONLINE,GAD_ABORT
		};

	/* Local copy of the global serial and modem settings. */

STATIC struct SerialSettings	*OriginalSerialConfig;
STATIC struct ModemSettings		*OriginalModemConfig;

	/* Local routines. */

STATIC VOID QuickHangup(struct DialNode *DialNode);
STATIC VOID DialPrintBox(LayoutHandle *Handle, LONG Box, LONG Line, CONST_STRPTR String, ...);
STATIC BOOL SendLocalModemCommand(LayoutHandle *Handle, CONST_STRPTR Command, CONST_STRPTR Message, CONST_STRPTR ErrorMessage);
STATIC VOID BuildName(STRPTR Name, LONG MaxNameLen, STRPTR Date);
STATIC VOID OpenAutoCaptureFile(CONST_STRPTR SomeName);
STATIC VOID Connect(PhonebookHandle *PhoneHandle,struct DialNode *DialNode);
STATIC LayoutHandle *OpenDialPanel(BOOL *Record,BOOL *SkipLoginMacro);

	/* GetCurrentSettings():
	 *
	 *	Get the settings corresponding to a dial node.*/

STATIC VOID
GetCurrentSettings(struct DialNode *DialNode,struct SerialSettings **SerialPtr,struct ModemSettings **ModemPtr)
{
	*SerialPtr	= OriginalSerialConfig;
	*ModemPtr	= OriginalModemConfig;

	if(DialNode && DialNode->Entry && DialNode->Entry->Config)
	{
		if(DialNode->Entry->Config->SerialConfig)
			*SerialPtr = DialNode->Entry->Config->SerialConfig;

		if(DialNode->Entry->Config->ModemConfig)
			*ModemPtr = DialNode->Entry->Config->ModemConfig;
	}
}

	/* QuickHangup(struct DialNode *DialNode):
	 *
	 *	Stop dialing by hanging up or sending a return character to the modem.
	 */

STATIC VOID
QuickHangup(struct DialNode *DialNode)
{
	BOOL UseHangUp;

	if(DialNode->Entry && DialNode->Entry->Config->ModemConfig)
		UseHangUp = DialNode->Entry->Config->ModemConfig->AbortHangsUp;
	else
		UseHangUp = OriginalModemConfig->AbortHangsUp;

	if(UseHangUp)
		HangUp();
	else
	{
		SerWrite("\r",1);

		StartTime(1,0);

			/* Wait for the modem to respond or the timer to elapse. */

		Wait(GetJobMask(SerialJob) | SIG_TIMER);

		StopTime();
	}

		/* Ignore the response of the modem. */

	while(RunJob(SerialJob));

	ResetDataFlowFilter();
}

	/* DialPrintBox():
	 *
	 *	Render text into one of the display box lines.
	 */

STATIC VOID
DialPrintBox(LayoutHandle *Handle,LONG Box,LONG Line,CONST_STRPTR String,...)
{
	UBYTE LocalBuffer[256];
	va_list VarArgs;

	va_start(VarArgs,String);
	LimitedVSPrintf(sizeof(LocalBuffer),LocalBuffer,String,VarArgs);
	va_end(VarArgs);

	LT_SetAttributes(Handle,Box,
		LABX_Index,	Line,
		LABX_Text,	LocalBuffer,
	TAG_DONE);
}

	/* SendModemCommandGetResponse(LayoutHandle *Handle,STRPTR Command):
	 *
	 *	Send a modem command and wait for a response.
	 */

STATIC BOOL
SendModemCommandGetResponse(LayoutHandle *Handle,CONST_STRPTR Command)
{
	ULONG Signals,SerialMask,WindowMask;
	ULONG LastBytesOut;
	BOOL Done;

		/* Reset the data flow scanner. */

	ResetDataFlowFilter();

		/* Send the modem command, but first remember how many
		 * bytes were sent yet.
		 */

	LastBytesOut = BytesOut;

	SerialCommand(Command);

		/* If no serial output took place, we'll return
		 * immediately.
		 */

	if(BytesOut == LastBytesOut)
		return(TRUE);

		/* Get the two wait masks. */

	SerialMask = GetJobMask(SerialJob);
	WindowMask = PORTMASK(Handle->Window->UserPort);

		/* Start the timer. We will wait up to ten seconds for
		 * the modem to respond.
		 */

	StartTime(10,0);

	Done = FALSE;

	do
	{
			/* Wait for something to happen. */

		Signals = Wait(SerialMask | WindowMask | SIG_TIMER);

			/* Any window signal? */

		if(Signals & WindowMask)
		{
			struct IntuiMessage	*Message;
			ULONG MsgClass,MsgQualifier;
			struct Gadget *MsgGadget;
			UWORD MsgCode;

				/* Check if we are to skip or abort. */

			while(Message = LT_GetIMsg(Handle))
			{
				MsgClass		= Message->Class;
				MsgCode			= Message->Code;
				MsgQualifier	= Message->Qualifier;
				MsgGadget		= Message->IAddress;

				LT_ReplyIMsg(Message);

					/* Convert the space keypress into a
					 * skip command.
					 */

				if(MsgClass == IDCMP_RAWKEY)
				{
					if(LT_GetCode(MsgQualifier,MsgClass,MsgCode,MsgGadget) == ' ')
						Done = TRUE;
				}

					/* So a button was pressed. */

				if(MsgClass == IDCMP_GADGETUP)
				{
					switch(MsgGadget->GadgetID)
					{
						case GAD_SKIP:
						case GAD_ABORT:

							Done = TRUE;
							break;
					}
				}
			}
		}

			/* Any modem signal? */

		if(Signals & SerialMask)
		{
				/* Handle the serial input. */

			while(RunJob(SerialJob));

				/* Exit if the modem said something sensible. */

			if(FlowInfo.Changed)
				Done = TRUE;
		}

			/* Did the bell ring? */

		if(Signals & SIG_TIMER)
			Done = TRUE;
	}
	while(!Done);

		/* Stop the timer. */

	StopTime();

		/* Did the modem respond? */

	return(FlowInfo.Changed);
}

	/* SendLocalModemCommand():
	 *
	 *	Send a command to the modem and display an error message if necessary.
	 */

STATIC BOOL
SendLocalModemCommand(LayoutHandle *Handle,CONST_STRPTR Command,CONST_STRPTR Message,CONST_STRPTR ErrorMessage)
{
		/* Show the message. */

	DialPrintBox(Handle,GAD_NOTE,0,Message);

		/* Send the command and check if the modem did respond. */

	if(SendModemCommandGetResponse(Handle,Command))
	{
			/* No news is good news. */

		if(!FlowInfo.Error)
			return(TRUE);
		else
			DialPrintBox(Handle,GAD_NOTE,0,ErrorMessage);
	}
	else
		DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_MODEM_NOT_RESPONDING_TXT));

	WakeUp(Handle->Window,SOUND_ERROR);
	return(FALSE);
}

	/* BuildName(STRPTR Name):
	 *
	 *	Build a file name from a BBS name and the current date.
	 */

STATIC VOID
BuildName(STRPTR Name,LONG MaxNameLen,STRPTR Date)
{
	if(Date[0])
	{
		LONG NameLen,DateLen,Delta;

		NameLen = strlen(Name);
		DateLen = strlen(Date);

		if((Delta = NameLen + 1 + DateLen - 32) > 0)
			Name[NameLen - Delta] = 0;

		LimitedStrcat(MaxNameLen,Name,"_");
		LimitedStrcat(MaxNameLen,Name,Date);
	}
}

	/* OpenAutoCaptureFile(STRPTR SomeName):
	 *
	 *	Open a capture file.
	 */

STATIC VOID
OpenAutoCaptureFile(CONST_STRPTR SomeName)
{
	struct DateTime	DateTime;
	UBYTE Date[20],Time[20];

		/* Get the current time and date. */

	DateStamp(&DateTime.dat_Stamp);

		/* Prepare for date conversion. */

	DateTime.dat_Format		= FORMAT_DEF;
	DateTime.dat_Flags		= 0;
	DateTime.dat_StrDay		= NULL;
	DateTime.dat_StrDate	= Date;
	DateTime.dat_StrTime	= Time;

		/* Convert the date. */

	if(DateToStr(&DateTime))
	{
		UBYTE LocalBuffer[MAX_FILENAME_LENGTH],Name[MAX_FILENAME_LENGTH];

			/* Remember the BBS name. */

		LimitedStrcpy(sizeof(Name),Name,SomeName);

			/* Append the creation date if necessary. */

		if(Config->CaptureConfig->AutoCaptureDate == AUTOCAPTURE_DATE_NAME)
			BuildName(Name,sizeof(Name),Date);

			/* Make it a reasonable name. */

		FixName(Name);

			/* Get the capture file path. */

		LimitedStrcpy(sizeof(LocalBuffer),LocalBuffer,Config->CaptureConfig->CapturePath);

			/* Try to build a valid file and path name. */

		if(AddPart(LocalBuffer,Name,sizeof(LocalBuffer)))
		{
				/* Is the capture file still open? */

			if(FileCapture)
			{
					/* Close the file. */

				BufferClose(FileCapture);

				FileCapture = NULL;

					/* Any data written? */

				if(!GetFileSize(CaptureName))
					DeleteFile(CaptureName);
				else
				{
					AddProtection(CaptureName,FIBF_EXECUTE);

					if(Config->MiscConfig->CreateIcons)
						AddIcon(CaptureName,FILETYPE_TEXT,TRUE);
				}
			}

				/* Try to append the new data. */

			if(FileCapture = BufferOpen(LocalBuffer,"a"))
			{
					/* Set the menu checkmark. */

				CheckItem(MEN_CAPTURE_TO_FILE,TRUE);

					/* Remember the current capture file name. */

				strcpy(CaptureName,LocalBuffer);

					/* Add the creation date if necessary. */

				if(Config->CaptureConfig->AutoCaptureDate == AUTOCAPTURE_DATE_INCLUDE)
				{
					UBYTE DateTimeBuffer[256];

					if(FormatStamp(&DateTime.dat_Stamp,DateTimeBuffer,sizeof(DateTimeBuffer),FALSE))
						BPrintf(FileCapture,LocaleString(MSG_DIALPANEL_FILE_CREATED_TXT),DateTimeBuffer);
				}
			}
			else
				CheckItem(MEN_CAPTURE_TO_FILE,FALSE);

			ConOutputUpdate();
		}
	}
}

	/* Connect(PhonebookHandle *PhoneHandle,struct DialNode *DialNode):
	 *
	 *	Perform connect action(s).
	 */

STATIC VOID
Connect(PhonebookHandle *PhoneHandle,struct DialNode *DialNode)
{
	struct ModemSettings *ModemConfig;
	ULONG RateIs;
	CONST_STRPTR Name;

		/* Check which modem configuration to use later. */

	if(DialNode->Entry && DialNode->Entry->Config->ModemConfig)
		ModemConfig = DialNode->Entry->Config->ModemConfig;
	else
		ModemConfig = OriginalModemConfig;

		/* Get the current area code settings. */

	SetActivePattern(FindTimeDate(PatternList,DialNode->Number));

		/* Mark the active phonebook entry, if any. */

	SetActiveEntry(PhoneHandle,DialNode->Entry);

		/* Get the DTE rate ready. */

	if(DTERate > 0)
		RateIs = DTERate;
	else
		RateIs = Config->SerialConfig->BaudRate;

		/* Does the node have a phonebook entry attached? */

	if(DialNode->Entry)
	{
		SaveConfig(DialNode->Entry->Config,Config);
		ConfigChanged = FALSE;

		Name = DialNode->Entry->Header->Name;

		strcpy(Password,DialNode->Entry->Header->Password);
		strcpy(UserName,DialNode->Entry->Header->UserName);

		SendStartup = TRUE;

		strcpy(CurrentBBSName,DialNode->Entry->Header->Name);
		strcpy(CurrentBBSComment,DialNode->Entry->Header->Comment);

		LogAction(LocaleString(MSG_DIALPANEL_CONNECTED_TO_1_TXT),Name,DialNode->Number,RateIs);
	}
	else
	{
		Name = LocaleString(MSG_GLOBAL_UNKNOWN_TXT);

		Password[0]	= 0;
		UserName[0]	= 0;

		SendStartup	= FALSE;

		CurrentBBSName[0]		= 0;
		CurrentBBSComment[0]	= 0;

		LogAction(LocaleString(MSG_DIALPANEL_CONNECTED_TO_2_TXT),DialNode->Number,RateIs);
	}

		/* Register the call. */

	MakeCall(Name,DialNode->Number);

		/* Store the current phone number. */

	strcpy(CurrentBBSNumber,DialNode->Number);

		/* Get the online time limit and the macro to execute when the limit is reached. */

	if(ModemConfig->ConnectLimit > 0 && ModemConfig->ConnectLimitMacro[0])
	{
		ArmLimit(ModemConfig->ConnectLimit);

		strcpy(LimitMacro,ModemConfig->ConnectLimitMacro);
	}
	else
		DisarmLimit();

		/* We are now online. */

	SetOnlineState(TRUE);

		/* Start the accounting process */

	StartAccountant(ModemConfig->TimeToConnect);

		/* Open auto-capture file. */

	if(Config->CaptureConfig->ConnectAutoCapture && Config->CaptureConfig->CapturePath[0])
	{
		CONST_STRPTR FileName;

		if(DialNode->Entry)
			FileName = DialNode->Entry->Header->Name;
		else
			FileName = LocaleString(MSG_DIALPANEL_CAPTURE_NAME_TXT);

		OpenAutoCaptureFile(FileName);
	}

		/* Remove node from dialing list and perform system setup. */

	RemoveAndDeleteRelatedDialNodes(PhoneHandle,DialNode);

		/* Make a backup of the old settings. */

	if(PrivateConfig->MiscConfig->BackupConfig && !BackupConfig)
	{
		if(BackupConfig = CreateConfiguration(TRUE))
			SaveConfig(PrivateConfig,BackupConfig);
	}

		/* Make sure that the following
		 * setup/initialization will not
		 * touch the serial configuration.
		 */

	PutConfigEntry(PrivateConfig,Config->SerialConfig,PREF_SERIAL);

	ConfigSetup();

		/* Reset the scanner. */

	ResetDataFlowFilter();
}

	/* OpenDialPanel(BOOL *Record):
	 *
	 *	Open the dialing panel.
	 */

STATIC LayoutHandle *
OpenDialPanel(BOOL *Record,BOOL *SkipLoginMacro)
{
	LayoutHandle *Handle;

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
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
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			BOX_KIND,
						LA_ID,				GAD_CALLING,
						LA_Chars,			40,
						LABX_Rows,			4,
						LABX_ReserveSpace,	TRUE,
						LABX_FirstLabel,	MSG_DIALPANEL_CALLING_TXT,
						LABX_LastLabel,		MSG_DIALPANEL_NEXT_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			BOX_KIND,
						LA_ID,				GAD_TIME,
						LABX_Rows,			2,
						LABX_ReserveSpace,	TRUE,
						LABX_FirstLabel,	MSG_DIALPANEL_TIMEOUT_TXT,
						LABX_LastLabel,		MSG_DIALPANEL_ATTEMPT_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			BOX_KIND,
						LA_ID,				GAD_NOTE,
						LABX_Rows,			1,
						LABX_ReserveSpace,	TRUE,
						LABX_FirstLabel,	MSG_DIALPANEL_MESSAGE_TXT,
						LABX_LastLabel,		MSG_DIALPANEL_MESSAGE_TXT,
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
						LA_LabelID,		MSG_DIALPANEL_RECORD_ON_CONNECTION_TXT,
						LA_BOOL,		Record,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_SKIP_LOGIN_MACRO_GAD,
						LA_BOOL,		SkipLoginMacro,
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
				LAGR_Spread,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_DIALPANEL_SKIP_GAD,
					LA_ID,			GAD_SKIP,
					LABT_ExtraFat,	TRUE,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_REMOVE_GAD,
					LA_ID,			GAD_REMOVE,
					LABT_ExtraFat,	TRUE,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_DIALPANEL_GO_TO_ONLINE_GAD,
					LA_ID,			GAD_ONLINE,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_ABORT_GAD,
					LA_ID,			GAD_ABORT,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_DIALPANEL_DIALING_TXT,
			LAWN_IDCMP,			IDCMP_CLOSEWINDOW,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
		/*	WA_SimpleRefresh,	TRUE, */
		TAG_DONE))
			return(Handle);
		else
			LT_DeleteHandle(Handle);
	}

	return(NULL);
}

	/* DialPanel():
	 *
	 *	This routine opens a small window in the middle of the
	 *	console window and walks down the list of numbers to
	 *	dial.
	 */

BOOL
DialPanel(PhonebookHandle *PhoneHandle)
{
	enum { STOP_Not,STOP_Abort,STOP_Skip };
	enum { START_Dial,START_Call,START_Wait };

	BOOL ReturnToPhonebook,RecordOnConnection,ReleaseSerialDevice,SkipLoginMacro;
	LayoutHandle *Handle;

			/* Remember the original modem and serial settings, so we
			 * can return to them later.
			 */

	OriginalSerialConfig	= CreateNewConfigEntry(PREF_SERIAL);
	OriginalModemConfig		= CreateNewConfigEntry(PREF_MODEM);

	if(!OriginalSerialConfig || !OriginalModemConfig)
	{
		FreeVecPooled(OriginalSerialConfig);
		OriginalSerialConfig = NULL;

		FreeVecPooled(OriginalModemConfig);
		OriginalModemConfig = NULL;

		DisplayBeep(NULL);

		return(FALSE);
	}

	ReturnToPhonebook = RecordOnConnection = ReleaseSerialDevice = SkipLoginMacro = FALSE;

	if(Handle = OpenDialPanel(&RecordOnConnection,&SkipLoginMacro))
	{
		struct SerialSettings *	CurrentSerialConfig;
		struct ModemSettings *	CurrentModemConfig;

		struct Window *			PanelWindow;

		UBYTE					NoteText[80];

		UBYTE					ExitCommand[80];

		UBYTE					ExitBuffer[80],
						 		InitBuffer[80];

		UBYTE			 		PrefixBuffer[80],
						 		SuffixBuffer[80];

		UBYTE					DialBuffer[300];

		LONG					DialTimeout = 0,DialRetries,DialAttempt;

		struct DialNode *		DialNode;

		LONG					RedialTimeout = 0;

		WORD					Start,Stop;

		BOOL					DoNotHangUp,ErrorHasOccured,ModemNotResponding,Done;

		BOOL					OldQuietSettings;

		ULONG					ODUMask,WindowMask,Signals;

		SENDLINE				OriginalSendLine;

		struct timeval			LastTimeCheck;

			/* Start from scratch. */

		PanelWindow 		= Handle->Window;

		DialRetries			= 0;
		DialAttempt			= 0;
		DialNode			= NULL;

		ErrorHasOccured		= FALSE;
		ModemNotResponding	= FALSE;
		DoNotHangUp 		= FALSE;

		Done				= FALSE;

		Start				= START_Call;
		Stop				= STOP_Not;

		GetCurrentSettings(NULL,&CurrentSerialConfig,&CurrentModemConfig);

		GetSysTime(&LastTimeCheck);

			/* Don't mix up the line send stuff with the default sending routine */

		OriginalSendLine = ChangeSendLine(SendLineDial);

			/* Set up the ODU stuff */

		if(OwnDevUnitBase && Config->SerialConfig->ReleaseODUWhenDialing && Config->SerialConfig->SatisfyODURequests != ODU_KEEP && !(Config->SerialConfig->Shared && Config->SerialConfig->NoODUIfShared))
			ODUMask = SIG_OWNDEVUNIT;
		else
			ODUMask = 0;

			/* And get the window port mask ready */

		WindowMask = PORTMASK(Handle->Window->UserPort);

			/* Reset the area code scanner data. */

		SetActivePattern(NULL);
		SetActiveEntry(PhoneHandle,NULL);

			/* We are now dialing. */

		PushStatus(STATUS_DIALING);

			/* Set up the AmigaGuide help context. */

		GuideContext(CONTEXT_DIAL);

			/* No exit command is defined yet. */

		ExitCommand[0] = 0;

			/* Make the current one the active one. */

		LT_ShowWindow(Handle,TRUE);

		PushWindow(PanelWindow);

			/* Make a backup of the current configuration. */

		SaveConfig(Config,PrivateConfig);

			/* Don't echo serial output unless requested by the user. */

		OldQuietSettings = SetConsoleQuiet((BOOL)(Config->ModemConfig->VerboseDialing == FALSE));

			/* Reset the scanner. */

		ResetDataFlowFilter();

			/* Get the entry signals ready and clear those we do not yet care about. */

		Signals = SetSignal(0,SIG_BREAK | SIG_SKIP | ODUMask | WindowMask) & (ODUMask | WindowMask);

			/* The big dialing loop. */

		do
		{
				/* Abort the process? */

			if(!Done && (Signals & SIG_BREAK))
			{
				Stop = STOP_Abort;
				Done = TRUE;
			}

				/* Check if we should let go of the device */

			if(!Done && (Signals & ODUMask))
			{
				Stop = STOP_Abort;
				Done = TRUE;

				ReleaseSerialDevice = TRUE;
			}

				/* Any window input? */

			if(!Done && (Signals & WindowMask))
			{
				struct IntuiMessage	*Message;
				ULONG MsgClass,MsgQualifier;
				struct Gadget *MsgGadget;
				UWORD MsgCode;

				while(Message = LT_GetIMsg(Handle))
				{
					MsgClass		= Message->Class;
					MsgCode			= Message->Code;
					MsgQualifier	= Message->Qualifier;
					MsgGadget		= Message->IAddress;

					LT_ReplyIMsg(Message);

						/* Convert the space keypress into a
						 * skip command.
						 */

					if(MsgClass == IDCMP_RAWKEY)
					{
						if(LT_GetCode(MsgQualifier,MsgClass,MsgCode,MsgGadget) == ' ')
						{
							if(Stop == STOP_Not && Start != START_Call)
							{
								LT_PressButton(Handle,GAD_SKIP);

								Stop = STOP_Skip;
							}
						}
					}

						/* Close the window, hang up the line,
						 * return to the phone book.
						 */

					if(MsgClass == IDCMP_CLOSEWINDOW)
					{
						Stop = STOP_Abort;
						Done = TRUE;
						ReturnToPhonebook = TRUE;
					}

						/* So a button was pressed. */

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
								/* Remove the currently active dialing
								 * list entry.
								 */

							case GAD_REMOVE:

									/* This makes sense only while
									 * we are dialing.
									 */

								if(Start == START_Dial)
								{
									struct DialNode	*NextNode;

									NextNode = NULL;

										/* If still dialing, hang up first. */

									if(DoNotHangUp)
										DoNotHangUp = FALSE;
									else
										QuickHangup(DialNode);

										/* Is there another entry in the list? */

									if(DialNode->Node.ln_Succ->ln_Succ)
										NextNode = (struct DialNode *)DialNode->Node.ln_Succ;
									else
									{
											/* No, there isn't; do we have a list with
											 * at least two entries in it?
											 */

										if(PhoneHandle->DialList->lh_Head->ln_Succ->ln_Succ)
										{
												/* Yet another dialing attempt coming up... */

											DialAttempt++;

												/* Check for dial retry limit. */

											if(DialRetries >= 0 && DialAttempt >= DialRetries)
											{
												DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_MAXIMUM_NUMBER_OF_DIAL_RETRIES_TXT));

												WakeUp(PanelWindow,SOUND_BELL);

												DelayTime(2,0);

												Say(LocaleString(MSG_DIALPANEL_MAXIMUM_NUMBER_OF_DIAL_RETRIES_TXT));

												Done = TRUE;
											}
											else
											{
													/* Grab first list entry and continue. */

												NextNode = (struct DialNode *)PhoneHandle->DialList->lh_Head;
											}
										}
										else
										{
												/* That's all, folks! */

											DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_DIALING_LIST_IS_EMPTY_TXT));

											DelayTime(2,0);

											Done = TRUE;
										}
									}

										/* Remove dial entry from list. */

									RemoveDialNode(PhoneHandle,DialNode);
									DeleteDialNode(DialNode);

										/* Is there an entry to proceed with? */

									if(DialNode = NextNode)
										Start = START_Call;

									GetCurrentSettings(DialNode,&CurrentSerialConfig,&CurrentModemConfig);
								}

								break;

							case GAD_SKIP:

								if(Stop == STOP_Not && Start != START_Call)
									Stop = STOP_Skip;

								break;

							case GAD_ONLINE:

									/* Go online so soon? */

								if(!DialNode)
								{
									if(!IsListEmpty(PhoneHandle->DialList))
										DialNode = (struct DialNode *)PhoneHandle->DialList->lh_Head;
								}

								if(DialNode)
									Connect(PhoneHandle,DialNode);

								GetCurrentSettings(DialNode,&CurrentSerialConfig,&CurrentModemConfig);

								Done = TRUE;

								break;

								/* Abort the dialing process. */

							case GAD_ABORT:

								Stop = STOP_Abort;
								Done = TRUE;

								break;
						}
					}

					if(Done)
						break;
				}
			}

				/* Skip the current action? */

			if(!Done && (Signals & SIG_SKIP))
			{
				if(Stop == STOP_Not && Start != START_Call)
					Stop = STOP_Skip;
			}

				/* Are we to abort? */

			if(Stop == STOP_Abort)
			{
				Stop = STOP_Not;

					/* Hang up if necessary. */

				if(Start == START_Dial)
				{
					DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_ABORTING_TXT));

						/* Hang up if necessary. */

					if(DoNotHangUp)
						DoNotHangUp = FALSE;
					else
						QuickHangup(DialNode);
				}
			}

				/* Start at the beginning. */

			if(!Done && !DialNode)
			{
				GetCurrentSettings(DialNode = (struct DialNode *)PhoneHandle->DialList->lh_Head,&CurrentSerialConfig,&CurrentModemConfig);

				LT_SetAttributes(Handle,GAD_SKIP,
					GA_Disabled,	FALSE,
				TAG_DONE);

				LT_SetAttributes(Handle,GAD_REMOVE,
					GA_Disabled,	FALSE,
				TAG_DONE);

				Start = START_Call;
			}

				/* Are we to start a call? */

			if(!Done && Start == START_Call)
			{
					/* Reset the sequence scanner, the user may have skipped
					 * the previous dial attempt causing the modem to return
					 * `NO CARRIER'. To prevent the dialer from skipping the
					 * next dial entry as well as the previous we have to
					 * flush any data pending on the serial line.
					 */

				ResetDataFlowFilter();

					/* Does this entry have a configuration attached? */

				if(DialNode->Entry == NULL && Config->ModemConfig->DoNotSendMainModemCommands)
					InitBuffer[0] = ExitBuffer[0] = 0;
				else
				{
					LimitedStrcpy(sizeof(InitBuffer),InitBuffer,DialNode->InitCommand ? DialNode->InitCommand : CurrentModemConfig->ModemInit);
					LimitedStrcpy(sizeof(ExitBuffer),ExitBuffer,DialNode->ExitCommand ? DialNode->ExitCommand : CurrentModemConfig->ModemExit);
				}

				LimitedStrcpy(sizeof(PrefixBuffer),PrefixBuffer,DialNode->DialPrefix ? DialNode->DialPrefix : CurrentModemConfig->DialPrefix);
				LimitedStrcpy(sizeof(SuffixBuffer),SuffixBuffer,DialNode->DialSuffix ? DialNode->DialSuffix : CurrentModemConfig->DialSuffix);

					/* Send the modem exit string before we
					 * will need to reconfigure the serial
					 * device driver.
					 */

				if(ExitCommand[0])
				{
					if(!SendLocalModemCommand(Handle,ExitCommand,LocaleString(MSG_DIALPANEL_SENDING_MODEM_EXIT_COMMAND_TXT),LocaleString(MSG_DIALPANEL_ERROR_SENDING_MODEM_COMMAND_TXT)))
					{
						if(!FlowInfo.Changed)
							ModemNotResponding = TRUE;

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}

					ExitCommand[0] = 0;
				}

					/* We will need to change the serial and modem parameters
					 * in order to establish a connection.
					 */

				if(!Done)
				{
					PutConfigEntry(Config,CurrentModemConfig,PREF_MODEM);

					if(ReconfigureSerial(PanelWindow,CurrentSerialConfig) == RECONFIGURE_FAILURE)
					{
						WakeUp(PanelWindow,SOUND_ERROR);

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}
				}

					/* Update the ODU bits and send the modem init command, if any. */

				if(!Done)
				{
						/* Update the ODU stuff */

					if(OwnDevUnitBase && CurrentSerialConfig->ReleaseODUWhenDialing && CurrentSerialConfig->SatisfyODURequests != ODU_KEEP && !(CurrentSerialConfig->Shared && CurrentSerialConfig->NoODUIfShared))
						ODUMask = SIG_OWNDEVUNIT;
					else
						ODUMask = 0;

						/* Send the modem init command. */

					if(InitBuffer[0])
					{
						if(!SendLocalModemCommand(Handle,InitBuffer,LocaleString(MSG_DIALPANEL_SENDING_MODEM_INIT_COMMAND_TXT),LocaleString(MSG_DIALPANEL_ERROR_SENDING_MODEM_COMMAND_TXT)))
						{
							if(!FlowInfo.Changed)
								ModemNotResponding = TRUE;

							ErrorHasOccured = TRUE;
							Done = TRUE;
						}

						InitBuffer[0] = 0;
					}
				}

					/* Get on with the rest. */

				if(!Done)
				{
					STRPTR Comment;

						/* Remember the new exit command. */

					LimitedStrcpy(sizeof(ExitCommand),ExitCommand,ExitBuffer);

					DialPrintBox(Handle,GAD_CALLING,0,DialNode->Node.ln_Name);

						/* Show the comment, if any. */

					if(DialNode->Entry && DialNode->Entry->Header->Comment[0])
						Comment = DialNode->Entry->Header->Comment;
					else
						Comment = "-";

					DialPrintBox(Handle,GAD_CALLING,1,Comment);

						/* Display the number being called. */

					DialPrintBox(Handle,GAD_CALLING,2,DialNode->Number);

						/* Display the name of the service to call next. */

					DialPrintBox(Handle,GAD_CALLING,3,DialNode->Node.ln_Succ->ln_Succ ? DialNode->Node.ln_Succ->ln_Name : "-");

						/* Right now we're dialing. */

					if(DialNode->Node.ln_Succ->ln_Succ)
					{
						LimitedSPrintf(sizeof(NoteText),NoteText,LocaleString(MSG_DIALER_NOTE_TXT),LocaleString(MSG_DIALPANEL_DIALING_TXT),GetListSize((struct List *)DialNode));

						DialPrintBox(Handle,GAD_NOTE,0,NoteText);
					}
					else
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_DIALING_TXT));

						/* Say what we are dialing. */

					Say(LocaleString(MSG_DIALPANEL_NOW_CALLING_TXT),DialNode->Node.ln_Name);

						/* Pick up dial timeout and dial retries. */

					DialTimeout	= CurrentModemConfig->DialTimeout;
					DialRetries	= CurrentModemConfig->DialRetries;

						/* Build the dialing command. */

					LimitedStrcpy(sizeof(DialBuffer),DialBuffer,PrefixBuffer);

					if(CurrentModemConfig->PBX_Mode && CurrentModemConfig->PBX_Prefix[0])
						LimitedStrcat(sizeof(DialBuffer),DialBuffer,CurrentModemConfig->PBX_Prefix);

					LimitedStrcat(sizeof(DialBuffer),DialBuffer,DialNode->Number);
					LimitedStrcat(sizeof(DialBuffer),DialBuffer,SuffixBuffer);

						/* Dial the number. */

					SerialCommand(DialBuffer);

						/* Now we should be dialing. */

					Start = START_Dial;

						/* For the sake of precision. */

					GetSysTime(&LastTimeCheck);
				}
			}

				/* Are we to skip the current assignment? */

			if(!Done && Stop == STOP_Skip)
			{
				Stop = STOP_Not;

				switch(Start)
				{
						/* Are we currently dialing? */

					case START_Dial:

							/* Hang up if necessary. */

						if(DoNotHangUp)
							DoNotHangUp = FALSE;
						else
							QuickHangup(DialNode);

							/* Is this one the last entry? */

						if(DialNode->Node.ln_Succ->ln_Succ)
						{
								/* There is still another entry. */

							GetCurrentSettings(DialNode = (struct DialNode *)DialNode->Node.ln_Succ,&CurrentSerialConfig,&CurrentModemConfig);

								/* Check if we should wait before we fire off another dial command. */

							if(CurrentModemConfig->InterDialDelay)
							{
									/* Wait until the inter-dial delay has elapsed. */

								RedialTimeout = CurrentModemConfig->InterDialDelay;

								DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_WAITING_TO_CALL_TXT));

								Say(LocaleString(MSG_DIALPANEL_WAITING_TXT));

									/* No entry is currently being called. */

								DialPrintBox(Handle,GAD_CALLING,0,"-");
								DialPrintBox(Handle,GAD_CALLING,1,"-");
								DialPrintBox(Handle,GAD_CALLING,2,"-");

								LT_SetAttributes(Handle,GAD_REMOVE,
									GA_Disabled,	TRUE,
								TAG_DONE);

									/* Display name of entry to call next. */

								DialPrintBox(Handle,GAD_CALLING,3,DialNode->Node.ln_Name);

								Start = START_Wait;

									/* For the sake of precision. */

								GetSysTime(&LastTimeCheck);
							}
							else
								Start = START_Call;
						}
						else
						{
								/* Yet another dial attempt coming up. */

							DialAttempt++;

								/* Is this one the last dial
								 * attempt to be made?
								 */

							if(DialAttempt >= DialRetries && DialRetries >= 0)
							{
								DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_MAXIMUM_NUMBER_OF_DIAL_RETRIES_TXT));

								WakeUp(PanelWindow,SOUND_BELL);

								DelayTime(2,0);

								Say(LocaleString(MSG_DIALPANEL_MAXIMUM_NUMBER_OF_DIAL_RETRIES_TXT));

								Done = TRUE;
							}
							else
							{
								LONG InterDialDelay;

									/* Get the first list entry. */

								GetCurrentSettings(DialNode = (struct DialNode *)PhoneHandle->DialList->lh_Head,&CurrentSerialConfig,&CurrentModemConfig);

									/* Get the redial delay. */

								RedialTimeout	= CurrentModemConfig->RedialDelay;
								InterDialDelay	= CurrentModemConfig->InterDialDelay;

									/* Check if the inter-dial delay is larger than
									 * the redial delay. If so, use the inter-dial delay.
									 */

								if(InterDialDelay > RedialTimeout)
									RedialTimeout = InterDialDelay;

									/* No redial delay? Restart dialing... */

								if(!RedialTimeout)
								{
									Start = START_Call;

									DelayTime(1,0);
								}
								else
								{
										/* Go into redial delay. */

									DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_REDIAL_DELAY_TXT));

									Say(LocaleString(MSG_DIALPANEL_WAITING_TXT));

										/* No entry is currently being called. */

									DialPrintBox(Handle,GAD_CALLING,0,"-");
									DialPrintBox(Handle,GAD_CALLING,1,"-");
									DialPrintBox(Handle,GAD_CALLING,2,"-");

									LT_SetAttributes(Handle,GAD_REMOVE,
										GA_Disabled,	TRUE,
									TAG_DONE);

										/* Display name of entry to call next. */

									DialPrintBox(Handle,GAD_CALLING,3,DialNode->Node.ln_Name);

									Start = START_Wait;

										/* For the sake of precision. */

									GetSysTime(&LastTimeCheck);
								}
							}
						}

						break;

						/* Stop waiting. */

					case START_Wait:

						if(Start == START_Wait)
						{
							LT_SetAttributes(Handle,GAD_REMOVE,
								GA_Disabled,	FALSE,
							TAG_DONE);

							Start = START_Call;
						}

						break;
				}
			}

				/* Take care of serial input. */

			if(!Done)
				RunJob(SerialJob);

				/* Any news from the modem? */

			if(!Done && FlowInfo.Changed)
			{
					/* Current number is busy. */

				if(FlowInfo.Busy || (FlowInfo.NoCarrier && CurrentModemConfig->NoCarrierIsBusy))
				{
					if(!Done && Start == START_Dial)
					{
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_LINE_IS_BUSY_TXT));

						Say(LocaleString(MSG_DIALPANEL_LINE_IS_BUSY_TXT));

						DelayTime(1,0);

						Stop = STOP_Skip;

						DoNotHangUp = TRUE;
					}

					ResetDataFlowFilter();
				}

					/* Line does not feature a dialtone. */

				if(FlowInfo.NoDialTone)
				{
					if(!Done && Start == START_Dial)
					{
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_NO_DIALTONE_TXT));

						WakeUp(PanelWindow,SOUND_ERROR);

						Say(LocaleString(MSG_DIALPANEL_NO_DIALTONE_TXT));

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}

					ResetDataFlowFilter();
				}

					/* Somebody tries to call us. */

				if(FlowInfo.Ring)
				{
					if(!Done)
					{
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_GLOBAL_INCOMING_CALL_TXT));

						WakeUp(PanelWindow,SOUND_RING);

						Say(LocaleString(MSG_GLOBAL_INCOMING_CALL_TXT));

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}

					ResetDataFlowFilter();
				}

					/* Somebody's talking. */

				if(FlowInfo.Voice)
				{
					if(!Done)
					{
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_INCOMING_VOICE_CALL_TXT));

						WakeUp(PanelWindow,SOUND_VOICE);

						Say(LocaleString(MSG_DIALPANEL_INCOMING_VOICE_CALL_TXT));

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}

					ResetDataFlowFilter();
				}

					/* We got a connect. */

				if(FlowInfo.Connect)
				{
					if(!Done && Start == START_Dial)
					{
							/* Make the connection. */

						Connect(PhoneHandle,DialNode);

						Done = TRUE;

							/* Wake the user up. */

						if(BaudBuffer[0])
						{
							DialPrintBox(Handle,GAD_NOTE,0,"CONNECT %s",BaudBuffer);

							WakeUp(PanelWindow,SOUND_CONNECT);

							DelayTime(2,0);

								/* Install new baud rate if desired. */

							if(Config->ModemConfig->ConnectAutoBaud && DTERate > 110)
							{
								Config->SerialConfig->BaudRate = DTERate;

								ReconfigureSerial(PanelWindow,NULL);
							}
						}
						else
						{
							DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_CONNECTION_ESTABLISHED_TXT));

							WakeUp(PanelWindow,SOUND_CONNECT);
						}

						Say(LocaleString(MSG_DIALPANEL_CONNECTION_ESTABLISHED_TXT));
					}

					ResetDataFlowFilter();
				}

					/* Looks like an error. */

				if(FlowInfo.Error)
				{
					if(!Done && Start == START_Dial)
					{
						DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_ERROR_SENDING_MODEM_COMMAND_TXT));

						WakeUp(PanelWindow,SOUND_ERROR);

						ErrorHasOccured = TRUE;
						Done = TRUE;
					}

					ResetDataFlowFilter();
				}

					/* In any other case, reset the scanner. */

				ResetDataFlowFilter();
			}

				/* If nothing special's up... */

			if(!Done && Stop == STOP_Not && Start != START_Call)
			{
				struct timeval Now,Then;

					/* Give a brief report. */

				switch(Start)
				{
					case START_Dial:

						DialPrintBox(Handle,GAD_TIME,0,"%2ld:%02ld",DialTimeout / 60,DialTimeout % 60);
						DialPrintBox(Handle,GAD_TIME,1,LocaleString(DialRetries < 0 ? MSG_DIAL_RETRIES_UNLIMITED_TXT : MSG_DIALPANEL_ATTEMPT_OF_TXT),DialAttempt + 1,DialRetries);

						break;

					case START_Wait:

						DialPrintBox(Handle,GAD_TIME,0,"%2ld:%02ld",RedialTimeout / 60,RedialTimeout % 60);
						break;
				}

					/* Wait a second or until something interesting happens. */

				StartTime(1,0);

				Signals = Wait(SIG_TIMER | SIG_BREAK | SIG_SKIP | ODUMask | WindowMask);

				StopTime();

					/* Get the current time. */

				GetSysTime(&Now);

					/* Check if the current time is "later" than the last time check.
					 * This is to avoid embarrassing situations such as could take
					 * place if the the user resets the operating system time.
					 */

				if(-CmpTime(&Now,&LastTimeCheck) >= 0)
				{
					CopyMem(&Now,&Then,sizeof(struct timeval));
					SubTime(&Now,&LastTimeCheck);
				}
				else
				{
					CopyMem(&Now,&LastTimeCheck,sizeof(struct timeval));
					Now.tv_secs = 0;
				}

					/* The following instructions are executed as time passes by. */

				if(Now.tv_secs > 0)
				{
					LONG Seconds = Now.tv_secs;

						/* Remember the last time we went through. */

					CopyMem(&Then,&LastTimeCheck,sizeof(struct timeval));

					switch(Start)
					{
							/* Are we dialing? */

						case START_Dial:

							if(DialTimeout > Seconds)
								DialTimeout -= Seconds;
							else
							{
								DialTimeout = 0;

									/* The dial timeout has elapsed without
									 * a connection being made.
									 */

								Stop = STOP_Skip;

								DialPrintBox(Handle,GAD_NOTE,0,LocaleString(MSG_DIALPANEL_DIAL_ATTEMPT_TIMEOUT_TXT));
							}

							break;

							/* Are we waiting? */

						case START_Wait:

							if(RedialTimeout > Seconds)
								RedialTimeout -= Seconds;
							else
							{
								RedialTimeout = 0;

									/* The redial delay has elapsed,
									 * start dialing again.
									 */

								Stop = STOP_Skip;
							}

							break;
					}
				}
			}
		}
		while(!Done);

			/* Reset the status */

		PopStatus();

			/* Are we online or not? */

		if(!Online)
		{
			BOOL SendCommands;

				/* If the modem is not responding anyway, don't bother
				 * sending any commands.
				 */

			SendCommands = (BOOL)(ModemNotResponding == FALSE);

				/* Is the serial setup different? */

			if(CompareConfigEntries(Config->SerialConfig,OriginalSerialConfig,PREF_SERIAL))
			{
					/* Set up the old serial configuration. */

				if(ReconfigureSerial(PanelWindow,OriginalSerialConfig) == RECONFIGURE_FAILURE)
					SendCommands = FALSE;
			}

				/* Put back the original modem settings. */

			PutConfigEntry(Config,OriginalModemConfig,PREF_MODEM);

			if(SendCommands)
			{
					/* Send the exit command if necessary. */

				if(ExitCommand[0])
				{
					if(!SendLocalModemCommand(Handle,ExitCommand,LocaleString(MSG_DIALPANEL_SENDING_MODEM_EXIT_COMMAND_TXT),LocaleString(MSG_DIALPANEL_ERROR_SENDING_MODEM_COMMAND_TXT)))
						ErrorHasOccured = TRUE;
				}

					/* Take care of the init command if necessary. */

				if(Config->ModemConfig->ModemInit[0] && !ErrorHasOccured)
				{
					if(!SendLocalModemCommand(Handle,Config->ModemConfig->ModemInit,LocaleString(MSG_DIALPANEL_SENDING_MODEM_INIT_COMMAND_TXT),LocaleString(MSG_DIALPANEL_ERROR_SENDING_MODEM_COMMAND_TXT)))
						ErrorHasOccured = TRUE;
				}
			}
		}

			/* Make sure the error gets displayed. */

		if(ErrorHasOccured && !ReleaseSerialDevice)
		{
			LONG i;

			for(i = GAD_SKIP ; i <= GAD_REMOVE ; i++)
				LT_SetAttributes(Handle,i,GA_Disabled,TRUE,TAG_DONE);

			Done = FALSE;

			do
			{
				if(Wait(WindowMask | SIG_BREAK) & SIG_BREAK)
					break;
				else
				{
					struct IntuiMessage	*Message;

					while(Message = LT_GetIMsg(Handle))
					{
						if(Message->Class == IDCMP_CLOSEWINDOW || Message->Class == IDCMP_GADGETUP)
							Done = TRUE;

						LT_ReplyIMsg(Message);
					}
				}
			}
			while(!Done);
		}

			/* Put back the old routine if necessary */

		RestoreSendLine(SendLineDial,OriginalSendLine);

		PopWindow();

		LT_DeleteHandle(Handle);

			/* Reset the scanner. */

		ResetDataFlowFilter();

			/* We are done now, restart echoing serial input. */

		SetConsoleQuiet(OldQuietSettings);

			/* Reset the display if necessary. */

		if(ResetDisplay)
		{
			if(!DisplayReset())
			{
				FreeVecPooled(OriginalSerialConfig);
				OriginalSerialConfig = NULL;

				FreeVecPooled(OriginalModemConfig);
				OriginalModemConfig = NULL;

				return(FALSE);
			}
		}

			/* Handle online jobs. */

		SetDialMenu(Online == FALSE);

		if(Online)
		{
				/* Send the startup macro if necessary. */

			if(SendStartup && !SkipLoginMacro)
			{
				if(Config->CommandConfig->LoginMacro[0])
					SerialCommand(Config->CommandConfig->LoginMacro);

				if(Config->CommandConfig->StartupMacro[0])
					SerialCommand(Config->CommandConfig->StartupMacro);

				SendStartup = FALSE;
			}

				/* Take care of the recording feature. */

			if(RecordOnConnection)
			{
				if(CreateRecord(CurrentBBSName[0] ? CurrentBBSName : CurrentBBSNumber))
				{
					RememberResetOutput();
					RememberResetInput();

					RememberOutput	= TRUE;

					Recording		= TRUE;
					RecordingLine	= FALSE;

					PushStatus(STATUS_RECORDING);

					OnItem(MEN_RECORD_LINE);

					CheckItem(MEN_RECORD,TRUE);
					CheckItem(MEN_RECORD_LINE,FALSE);
				}
			}
		}
	}

		/* Reply to the messages that started the
		 * dialing process.
		 */

	DispatchRexxDialMsgList(Online);

		/* Release the serial device driver if necessary. */

	if(ReleaseSerialDevice)
		RunJob(OwnDevUnitJob);

		/* Release the backup data. */

	FreeVecPooled(OriginalSerialConfig);
	OriginalSerialConfig = NULL;

	FreeVecPooled(OriginalModemConfig);
	OriginalModemConfig = NULL;

		/* Exit and tell the caller if control should pass
		 * back to the phonebook control panel.
		 */

	return(ReturnToPhonebook);
}
