/*
**	Main.c
**
**	Program main routines and event loop
**
**	Copyright � 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Argument vectors offsets. */

enum	{
			ARG_WINDOW,ARG_PUBSCREEN,ARG_STARTUP,ARG_PORTNAME,ARG_SETTINGS,ARG_UNIT,ARG_DEVICE,
			ARG_NEW,ARG_SYNC,ARG_QUIET,ARG_BEHIND,ARG_DEBUG,ARG_LANGUAGE,ARG_PHONEBOOK,ARG_AUTODIAL,
			ARG_AUTOEXIT,ARG_WINDOWTITLE,

			ARG_COUNT
		};

	/* The shell argument template and the corresponding help message. */

STATIC STRPTR 		ArgTemplate =	"WINDOW/K,PUBSCREEN/K,STARTUP/K,PORTNAME/K,"
									"SETTINGS/K,UNIT/K/N,DEVICE/K,NEW/S,SYNC/S,"
									"QUIET/S,BEHIND/S,DEBUG/S,LANGUAGE/K,PHONEBOOK/K,"
									"AUTODIAL/S,AUTOEXIT/S,WINDOWTITLE/K";

STATIC STRPTR		ArgHelp =		"\nUsage: term [WINDOW <Name>] [PUBSCREEN <Name>] [STARTUP <File name>]\n"
									"            [SETTINGS <File or path name>] [UNIT <Number>] [DEVICE <Name>]\n"
									"            [NEW] [SYNC] [QUIET] [BEHIND] [LANGUAGE <Name>] [PHONEBOOK <File name>]\n"
									"            [AUTODIAL] [AUTOEXIT] [WINDOWTITLE <Title>]\n"
									"\n"
									"      Window = Output window specifier\n"
									"   PubScreen = Name of public screen to open window upon\n"
									"     Startup = ARexx script file to run on startup\n"
									"    Settings = Main configuration file name or path name to search for it\n"
									"        Unit = Serial device driver unit number\n"
									"      Device = Serial device driver name\n"
									"         New = Spawn a new `term' process\n"
									"        Sync = Keep links to Shell environment\n"
									"       Quiet = Start iconified\n"
									"      Behind = Open screen behind all other screens, don't activate the window\n"
									"   Phonebook = Name of phonebook file to load\n"
									"    Autodial = Dial phonebook entries which are marked for autodialing\n"
									"    Autoexit = Exit `term' after dialing all autodial entries\n"
									" Windowtitle = Display this text as the terminal window title\n"
									"    Language = Language to use for the user interface\n\n";

	/* Local config path variable. */

STATIC STRPTR		ConfigPath;
STATIC UBYTE		ThePath[MAX_FILENAME_LENGTH];

	/* Startup file name. */

STATIC UBYTE		StartupFile[MAX_FILENAME_LENGTH];

	/* Console output window specifier. */

STATIC UBYTE		WindowName[256];

	/* Did we hang up the line? */

STATIC BOOL			HungUp;

	/* Go on redialing? */

STATIC BOOL			KeepRedialing;
STATIC BOOL			AutoDial;
STATIC BOOL			AutoExit;

	/* Display the connection cost? */

STATIC BOOL			DisplayHangup;
STATIC ULONG		DisplayPay;

	/* Poll OwnDevUnit.library for the device to become available again? */

STATIC BOOL			PollODU;
STATIC UWORD		PollODUCount;

	/* Check the menu for items to dial? */

STATIC BOOL			CheckDialMenu;

	/* Local routines */

STATIC BOOL HandleInput(VOID);
STATIC VOID ClearDialMenu(VOID);
STATIC BOOL EnterItemToDialList(BOOL AddedSomething,struct MenuItem *Item);
STATIC BOOL DialMenuToDialList(VOID);
STATIC VOID HandleMenu(ULONG Code,ULONG Qualifier);
STATIC VOID HandleIconify(VOID);
STATIC VOID HandleOnlineCleanup(BOOL Hangup);
STATIC VOID HandleFlowChange(VOID);
STATIC VOID FullHangup(BOOL ForceIt);
STATIC BOOL AskDial(struct Window *Parent);
STATIC VOID GoOnline(VOID);
STATIC VOID ReadyToDial(struct DataDialMsg *DialMsg);
STATIC VOID HandleUpload(UBYTE Type);
STATIC VOID LocalReleaseSerial(VOID);
STATIC VOID HandleKeyboardInput(UBYTE Char,UWORD Code,ULONG Qualifier,STRPTR InputBuffer,LONG Len);

	/* Main():
	 *
	 *	This is our main entry point.
	 */

LONG
Main()
{
	LONG Error = 0;

		/* Are we running as a child of Workbench? */

	ThisProcess = (struct Process *)FindTask(NULL);

	if(ThisProcess->pr_CLI)
		WBenchMsg = NULL;
	else
	{
		WaitPort(&ThisProcess->pr_MsgPort);

		WBenchMsg = (struct WBStartup *)GetMsg(&ThisProcess->pr_MsgPort);
	}

		/* Now try to open dos.library and utility.library and
		 * proceed to examine the startup parameters.
		 */

	DOSBase		= (struct DosLibrary *)OpenLibrary("dos.library",0);
	UtilityBase	= (struct UtilityBase *)OpenLibrary("utility.library",0);

	if(DOSBase && UtilityBase)
	{
		struct RDArgs *ArgsPtr;
		STRPTR *ArgArray;
		BOOL LaunchNew;

			/* Reset these first. */

		ArgsPtr		= NULL;
		ArgArray	= NULL;
		LaunchNew	= TRUE;

			/* We were called from Shell. */

		if(ThisProcess->pr_CLI)
		{
				/* No home directory to return to. */

			WBenchLock = BNULL;

				/* Use the ReadArgs parser, allocate the
				 * argument vectors...
				 */

			if(ArgArray = (STRPTR *)AllocVec(sizeof(STRPTR) * (ARG_COUNT),MEMF_ANY|MEMF_CLEAR))
			{
				if(ArgsPtr = (struct RDArgs *)AllocDosObject(DOS_RDARGS,TAG_DONE))
				{
					ArgsPtr->RDA_ExtHelp = ArgHelp;

						/* Parse the args (if any). */

					if(ReadArgs(ArgTemplate,(IPTR *)ArgArray,ArgsPtr))
					{
							/* Pop a running `term' to the front? */

						if(!ArgArray[ARG_NEW])
							LaunchNew = FALSE;

							/* For debugging purposes; not used yet. */

						if(ArgArray[ARG_DEBUG])
							DebugFlag = TRUE;

							/* Special language requested? */

						if(ArgArray[ARG_LANGUAGE])
							LimitedStrcpy(sizeof(Language),Language,ArgArray[ARG_LANGUAGE]);

							/* Are we to use a special settings path? */

						if(ArgArray[ARG_SETTINGS])
						{
							ConfigPath = ThePath;

							LimitedStrcpy(sizeof(ThePath),ThePath,ArgArray[ARG_SETTINGS]);
						}

							/* Are we to use a special ARexx host port name? */

						if(ArgArray[ARG_PORTNAME])
							LimitedStrcpy(sizeof(RexxPortName),RexxPortName,ArgArray[ARG_PORTNAME]);

							/* Are we to use a special output window name? */

						if(ArgArray[ARG_WINDOW])
							LimitedStrcpy(sizeof(WindowName),WindowName,ArgArray[ARG_WINDOW]);

							/* Are we to run an ARexx script on startup? */

						if(ArgArray[ARG_STARTUP])
							LimitedStrcpy(sizeof(StartupFile),StartupFile,ArgArray[ARG_STARTUP]);

							/* Load a special phonebook file */

						if(ArgArray[ARG_PHONEBOOK])
							LimitedStrcpy(sizeof(LastPhone),LastPhone,ArgArray[ARG_PHONEBOOK]);

						if(ArgArray[ARG_AUTODIAL])
							AutoDial = TRUE;

						if(ArgArray[ARG_AUTOEXIT])
							AutoExit = TRUE;

							/* Are we to open a window on a public screen? */

						if(ArgArray[ARG_PUBSCREEN])
							LimitedStrcpy(sizeof(SomePubScreenName),SomePubScreenName,ArgArray[ARG_PUBSCREEN]);

							/* Are we to use a special device? */

						if(ArgArray[ARG_DEVICE])
						{
							LimitedStrcpy(sizeof(NewDevice),NewDevice,ArgArray[ARG_DEVICE]);

							UseNewDevice = TRUE;
						}

							/* Are we to use a special unit number? */

						if(ArgArray[ARG_UNIT])
						{
							NewUnit = *(LONG *)ArgArray[ARG_UNIT];

							UseNewUnit = TRUE;
						}

							/* Are we to start up iconified? */

						if(ArgArray[ARG_QUIET])
						{
							if(!StartupFile[0])
								DoIconify = TRUE;
						}

							/* Hide the screen and don't activate the window? */

						if(ArgArray[ARG_BEHIND])
							KeepQuiet = TRUE;

							/* Use a special window title? */

						if(ArgArray[ARG_WINDOWTITLE])
						{
							CopyMem(ArgArray[ARG_WINDOWTITLE],WindowTitle,79);
							WindowTitle[79] = 0;
						}
						else
							WindowTitle[0] = 0;
					}
					else
						Error = IoErr();
				}
				else
					Error = IoErr();
			}
			else
				Error = IoErr();
		}
		else
		{
			BOOL ScrewThePath = TRUE;

				/* Change to the program's home directory. */

			WBenchLock = CurrentDir(WBenchMsg->sm_ArgList->wa_Lock);

				/* Open icon.library, we want to take a
				 * look at the icon.
				 */

			if(IconBase = OpenLibrary("icon.library",0))
			{
				struct DiskObject *Icon;

					/* Try to read the icon file. */

				if(Icon = GetProgramIcon())
				{
					STRPTR Type;

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"DEBUG"))
						DebugFlag = TRUE;

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"NOPATHFIX"))
						ScrewThePath = FALSE;

						/* Look for a `Language' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"LANGUAGE"))
						LimitedStrcpy(sizeof(Language),Language,Type);

						/* Look for a `Settings' tooltype. */

					if(ConfigPath = FindToolType((UBYTE **)Icon->do_ToolTypes,"SETTINGS"))
					{
							/* Remember the path and continue. */

						LimitedStrcpy(sizeof(ThePath),ThePath,ConfigPath);

						ConfigPath = ThePath;
					}

						/* Look for a `Phonebook' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"PHONEBOOK"))
						LimitedStrcpy(sizeof(LastPhone),LastPhone,Type);

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"AUTODIAL"))
						AutoDial = TRUE;

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"AUTOEXIT"))
						AutoExit = TRUE;

						/* Look for a `Portname' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"PORTNAME"))
						LimitedStrcpy(sizeof(RexxPortName),RexxPortName,Type);
					else
						RexxPortName[0] = 0;

						/* Look for a `Window' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"WINDOW"))
						LimitedStrcpy(sizeof(WindowName),WindowName,Type);

						/* Look for a `Windowtitle' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"WINDOWTITLE"))
						LimitedStrcpy(sizeof(WindowTitle),WindowTitle,Type);
					else
						WindowTitle[0] = 0;

						/* Look for a `Pubscreen' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"PUBSCREEN"))
						LimitedStrcpy(sizeof(SomePubScreenName),SomePubScreenName,Type);
					else
						SomePubScreenName[0] = 0;

						/* Look for a `Startup' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"STARTUP"))
						LimitedStrcpy(sizeof(StartupFile),StartupFile,Type);
					else
						StartupFile[0] = 0;

						/* Look for a `Device' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"DEVICE"))
					{
						if(Type[0])
						{
							LimitedStrcpy(sizeof(NewDevice),NewDevice,Type);

							UseNewDevice = TRUE;
						}
					}

						/* Look for a `Unit' tooltype. */

					if(Type = FindToolType((UBYTE **)Icon->do_ToolTypes,"UNIT"))
					{
						if(Type[0])
						{
							NewUnit = Atol(Type);

							UseNewUnit = TRUE;
						}
					}

						/* Look for a `Quiet' tooltype. */

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"QUIET"))
					{
						if(!StartupFile[0])
							DoIconify = TRUE;
					}

						/* Look for a `Behind' tooltype. */

					if(FindToolType((UBYTE **)Icon->do_ToolTypes,"BEHIND"))
						KeepQuiet = TRUE;

						/* Free the icon. */

					FreeDiskObject(Icon);
				}

				CloseLibrary(IconBase);
				IconBase = NULL;
			}

				/* Try to create a local CLI structure so
				 * Shell commands will receive a valid
				 * search path list.
				 */

			if(ScrewThePath)
				AttachCLI(WBenchMsg);
		}

			/* Show the error message; Error can be non-zero only if
			 * `term' was started from shell.
			 */

		if(Error)
			PrintFault(Error,"term");
		else
		{
				/* If there is already a `term' program running, pop its window to the front. */

			if(!LaunchNew)
			{
				struct TermPort *TermPort;

					/* If no other program is running, don't bother. */

				Forbid();

				if(!(TermPort = (struct TermPort *)FindPort(TERMPORTNAME)))
					LaunchNew = TRUE;
				else
				{
						/* Open intuition and pop the window to the front. */

					if(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0))
					{
						if(TermPort->TopWindow)
							BumpWindow(TermPort->TopWindow);

						CloseLibrary((struct Library *)IntuitionBase);
						IntuitionBase = NULL;
					}
				}

				Permit();
			}

				/* Do something sensible? */

			if(LaunchNew)
			{
				BYTE OldTaskPri = ThisProcess->pr_Task.tc_Node.ln_Pri;

				if(StackSize(NULL) < 16384)
				{
					LONG Success;

					if(!StackCall(&Success,16384,0,(STACKCALL)HandleInput))
						Error = ERROR_NO_FREE_STORE;
				}
				else
				{
					if(!HandleInput())
						Error = ERROR_NO_FREE_STORE;
				}

				CloseAll();

				SetTaskPri((struct Task *)ThisProcess,OldTaskPri);
			}
		}

			/* Now clean up any allocated data. */

		if(ArgsPtr)
		{
			FreeArgs(ArgsPtr);
			FreeDosObject(DOS_RDARGS,ArgsPtr);
		}

		FreeVec(ArgArray);

			/* Return to the old drawer. */

		if(WBenchMsg)
			CurrentDir(WBenchLock);
	}

		/* Close the libraries. */

	CloseLibrary((struct Library *)DOSBase);
	CloseLibrary((struct Library *)UtilityBase);

		/* If run from Workbench, return the startup message. */

	if(WBenchMsg)
	{
		Forbid();

		ReplyMsg((struct Message *)WBenchMsg);
	}

	if(Error)
		return(RETURN_ERROR);
	else
		return(RETURN_OK);
}

	/* HandleInput():
	 *
	 *	This is our main input loop (check window & serial).
	 */

STATIC BOOL
HandleInput()
{
	CONST_STRPTR		 Result;
	SENDLINE		 OriginalSendLine;
	PhonebookHandle	*PhoneHandle;

	ThisProcess = (struct Process *)FindTask(NULL);

		/* Open the resources we need. */

	if(Result = OpenAll(ConfigPath))
	{
		if(IntuitionBase && Result[0])
			ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Result);

		return(FALSE);
	}

		/* Store the output window name. */

	if(WindowName[0])
		LimitedStrcpy(sizeof(Config->MiscConfig->WindowName),Config->MiscConfig->WindowName,WindowName);

		/* Tell the user what he probably doesn't know yet. */

	if(TermVersion > Config->SerialConfig->LastVersionSaved || (TermVersion == Config->SerialConfig->LastVersionSaved && TermRevision > Config->SerialConfig->LastRevisionSaved))
	{
		BlockWindows();

		ScreenToFront(Window->WScreen);

		DisplayBeep(Window->WScreen);
		DisplayBeep(Window->WScreen);

		ShowInfo(Window,LocaleString(MSG_ATTENTION_PLEASE_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocaleString(MSG_REMINDER_TXT));

		ReleaseWindows();
	}

		/* Give a hint. */

	if(Config->MiscConfig->ProtectiveMode && !FirstInvocation)
	{
		BlockWindows();

		if(Config->SerialConfig->BaudRate >= 4800 && Config->SerialConfig->HandshakingProtocol == HANDSHAKING_NONE && !Config->SerialConfig->DirectConnection)
		{
			ScreenToFront(Window->WScreen);

			if(ShowRequest(Window,LocaleString(MSG_NO_RTSCTS_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Config->SerialConfig->BaudRate))
			{
				SaveConfig(Config,PrivateConfig);

				Config->SerialConfig->HandshakingProtocol = HANDSHAKING_RTSCTS_DSR;

				ConfigSetup();
			}
		}

		if(Config->SerialConfig->BaudRate >= 4800 && Config->ModemConfig->ConnectAutoBaud)
		{
			ScreenToFront(Window->WScreen);

			if(ShowRequest(Window,LocaleString(MSG_AUTOBAUD_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT)))
			{
				SaveConfig(Config,PrivateConfig);

				Config->ModemConfig->ConnectAutoBaud = FALSE;

				ConfigSetup();
			}
		}

		ReleaseWindows();
	}

		/* Don't confuse the user yet, do it later ;-) */

	FirstInvocation = FALSE;

		/* Start the online timer if a carrier is present? */

	if(Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
	{
			/* Is the carrier signal present? */

#ifdef __AROS__
		if(!(GetSerialStatus() & SER_STATF_COMCD))
#else
		if(!(GetSerialStatus() & CIAF_COMCD))
#endif
			GoOnline();
	}

		/* Start up iconified? */

	if(DoIconify)
	{
		HandleIconify();

		DoIconify = FALSE;
	}

		/* If we came out of iconified mode, check if were told to quit. */

	if(!MainTerminated)
	{
		if(!KeepQuiet)
			BumpWindow(Window);

			/* Set up the public screen data. */

		PubScreenStuff();

			/* Change program priority. */

		SetTaskPri((struct Task *)ThisProcess,(LONG)Config->MiscConfig->Priority);

		BlockWindows();

			/* Load the phone book. */

		if(PhoneHandle = LoadPhonebook(LastPhone))
		{
			DeletePhonebook(GlobalPhoneHandle);
			GlobalPhoneHandle = PhoneHandle;

			strcpy(Config->PhonebookFileName,LastPhone);
		}

			/* Build new menu strip. */

		if(!AttachMenu(NULL))
		{
			if(IntuitionBase)
				ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_MENUS_TXT));

			return(FALSE);
		}
		else
		{
			if(Online)
				SetDialMenu(FALSE);
		}

			/* Show our business card. */

		if(!StartupFile[0] && !Config->CommandConfig->StartupMacro[0] && !KeepQuiet && !(GlobalPhoneHandle->AutoDial || AutoDial))
			ShowAbout(TRUE);

		ReleaseWindows();

			/* Don't do anything silly. */

		KeepQuiet = FALSE;

		LogAction(LocaleString(MSG_TERMMAIN_LOGMSG_PROGRAM_STARTED_TXT),TermName,TermDate);

			/* Initialize the modem. */

		OriginalSendLine = ChangeSendLine(SendLineDial);

		SerialCommand(Config->ModemConfig->ModemInit);

			/* Execute the startup macro (if any). */

		if(Config->CommandConfig->StartupMacro[0])
			SerialCommand(Config->CommandConfig->StartupMacro);

		RestoreSendLine(SendLineDial,OriginalSendLine);

			/* Check if we should dial some entries from the phonebook */
			/* right upon startup */

		if(GlobalPhoneHandle->AutoDial || AutoDial)
		{
			PhoneEntry **Phonebook;
			BOOL DidSomething;
			LONG i;

			Phonebook = GlobalPhoneHandle->Phonebook;
			DidSomething = FALSE;

			for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
			{
				if(Phonebook[i]->Header->AutoDial && GlobalPhoneHandle->Phonebook[i]->Header->Number[0])
				{
					MarkDialEntry(GlobalPhoneHandle,Phonebook[i]);

					DidSomething = TRUE;
				}
			}

			if(DidSomething)
			{
				if(AddAllDialEntries(GlobalPhoneHandle))
				{
					DoDial = DIAL_LIST;

					if(GlobalPhoneHandle->AutoExit || AutoExit)
						KeepRedialing = TRUE;
				}
			}
		}
		else
		{
			PhoneEntry **Phonebook;
			LONG HowMany;
			LONG i;

			Phonebook = GlobalPhoneHandle->Phonebook;
			HowMany = 0;

			for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
			{
				if(Phonebook[i]->Header->Marked > 0)
					HowMany++;
			}

			if(HowMany > 0)
			{
				LONG j,Counter,Which;

				for(j = 0 ; j < HowMany ; j++)
				{
					Counter	= Which = -1;

					for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
					{
						if(Phonebook[i]->Header->Marked > 0)
						{
							if(Counter == -1 || Phonebook[i]->Header->Marked <= Counter)
							{
								Counter	= Phonebook[i]->Header->Marked;
								Which	= i;
							}
						}
					}

					if(Which != -1)
					{
						MarkDialEntry(GlobalPhoneHandle,Phonebook[Which]);
						Phonebook[Which]->Header->Marked = 0;
					}
				}

				AddAllDialEntries(GlobalPhoneHandle);
			}
		}

			/* Process the startup file if any. */

		if(StartupFile[0])
			ActivateJob(MainJobQueue,StartupFileJob);

			/* Go into input loop... */

		do
		{
				/* Dial the list */

			if(DoDial != DIAL_IGNORE)
			{
				DoDial = DIAL_IGNORE;

				if(Online && !IsListEmpty(&RexxDialMsgList))
					DispatchRexxDialMsgList(FALSE);
				else
				{
					BlockWindows();

					ClearDialMenu();

					do
					{
						if(!DialPanel(GlobalPhoneHandle))
						{
							KeepRedialing = FALSE;
							break;
						}
					}
					while(PhonePanel(0));

					if(!GlobalPhoneHandle->AutoExit)
						KeepRedialing = FALSE;

					SetRedialMenu();

					ReleaseWindows();
				}
			}

				/* Process the job queue. */

			ProcessJobQueue(MainJobQueue);

				/* Is that it? */

			if(MainTerminated)
			{
				LONG Complaint;

					/* Check if there are ARexx programs running in the background. */

				if(GetLaunchCounter() > 0)
					Complaint = MSG_TERMMAIN_CANNOT_QUIT_YET_TXT;
				else
				{
						/* If we are about to quit, try to close the screen. */

					if(!DeleteDisplay())
						Complaint = MSG_TERMMAIN_CANNOT_CLOSE_SCREEN_YET_TXT;
					else
						Complaint = 0;
				}

					/* Check if we must complain. */

				if(Complaint > 0)
				{
					BlockWindows();
					ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocaleString(Complaint));
					ReleaseWindows();

					MainTerminated = FALSE;
				}
			}
			else
			{
					/* Make the user notice not too obvious events. */

				if(FlowInfo.Changed)
					HandleFlowChange();
			}
		}
		while(!MainTerminated);
	}

		/* Send the modem exit command. */

	SendLine = (SENDLINE)SendLineDial;
	SerialCommand(Config->ModemConfig->ModemExit);

		/* Stop I/O processing. */

	ClearSerial();

		/* Say goodbye. */

	LogAction(LocaleString(MSG_TERMMAIN_LOGMSG_PROGRAM_TERMINATED_TXT));
	Say(LocaleString(MSG_TERMMAIN_BYE_BYE_TXT));

	return(TRUE);
}

	/* SendInputTextBuffer(STRPTR Buffer,LONG Len,BOOL Bell,BOOL ConvertLF):
	 *
	 *	Transmit text the user typed or pasted via the
	 *	clipboard.
	 */

VOID
SendInputTextBuffer(STRPTR Buffer,LONG Len,BOOL Bell,BOOL ConvertLF)
{
	LONG Mask;
	UBYTE c;

	if(Config->SerialConfig->StripBit8)
		Mask = 0x7F;
	else
		Mask = 0xFF;

	while(Len--)
	{
		switch(CharType[c = (*Buffer++) & Mask])
		{
			case CHAR_ENTER:

				if(Get_xOFF())
				{
					if(Bell)
						BellSignal();
				}
				else
				{
					if(ConvertLF)
					{
						switch(Config->TerminalConfig->SendCR)
						{
							case EOL_LF:

								SerWrite("\n",1);
								break;

							case EOL_CR:

								SerWrite("\r",1);
								break;

							case EOL_LFCR:

								SerWrite("\n\r",2);
								break;

							case EOL_CRLF:

								SerWrite("\r\n",2);
								break;
						}
					}
					else
					{
						switch(Config->TerminalConfig->SendLF)
						{
							case EOL_LF:

								SerWrite("\n",1);
								break;

							case EOL_CR:

								SerWrite("\r",1);
								break;

							case EOL_LFCR:

								SerWrite("\n\r",2);
								break;

							case EOL_CRLF:

								SerWrite("\r\n",2);
								break;
						}
					}
				}

				break;

			case CHAR_RETURN:

				if(Get_xOFF())
				{
					if(Bell)
						BellSignal();
				}
				else
				{
					switch(Config->TerminalConfig->SendCR)
					{
						case EOL_LF:

							SerWrite("\n",1);
							break;

						case EOL_CR:

							SerWrite("\r",1);
							break;

						case EOL_LFCR:

							SerWrite("\n\r",2);
							break;

						case EOL_CRLF:

							SerWrite("\r\n",2);
							break;
					}
				}

				break;

				/* Restart in/output. */

			case CHAR_XON:

				if(Config->SerialConfig->xONxOFF)
					Clear_xOFF();

				if(Config->SerialConfig->PassThrough)
					SerWrite(&c,1);

				break;

				/* Stop in/output. */

			case CHAR_XOFF:

				if(Config->SerialConfig->xONxOFF)
					Set_xOFF();

				if(Config->SerialConfig->PassThrough)
					SerWrite(&c,1);

				break;

				/* Any other character. */

			case CHAR_VANILLA:

				if(Get_xOFF())
				{
					if(Bell)
						BellSignal();
				}
				else
				{
					if(Config->TerminalConfig->FontMode == FONT_IBM)
					{
							/* Convert special
							 * Amiga characters into
							 * alien IBM dialect.
							 */

						if(IBMConversion[c])
							SerWrite(&IBMConversion[c],1);
						else
							SerWrite(&c,1);
					}
					else
						SerWrite(&c,1);
				}

				break;
		}
	}
}

STATIC VOID
ClearDialMenu()
{
	if(Menu)
	{
		struct Menu	*ThisMenu;
		struct MenuItem	*Item;

		ClearMenuStrips();

		for(ThisMenu = Menu ; ThisMenu ; ThisMenu = ThisMenu->NextMenu)
		{
			if(Item = ThisMenu->FirstItem)
			{
				do
				{
					if((ULONG)(IPTR)GTMENUITEM_USERDATA(Item) >= DIAL_MENU_LIMIT)
					{
						Item->Flags &= ~CHECKED;

						if(Item->SubItem)
						{
							struct MenuItem *SubItem = Item->SubItem;

							do
								SubItem->Flags &= ~CHECKED;
							while(SubItem = SubItem->NextItem);
						}
					}
				}
				while(Item = Item->NextItem);
			}
		}

		ResetMenuStrips(Menu);
	}
}

STATIC BOOL
EnterItemToDialList(BOOL AddedSomething,struct MenuItem *Item)
{
	ULONG Index;

	Index = (ULONG)(IPTR)GTMENUITEM_USERDATA(Item);

	if(Index >= DIAL_MENU_LIMIT && (Item->Flags & (ITEMENABLED|CHECKIT|CHECKED)) == (ITEMENABLED|CHECKIT|CHECKED))
	{
		Index -= DIAL_MENU_LIMIT;

		if(Index < GlobalPhoneHandle->NumPhoneEntries)
		{
				/* Clear the checkmark. */

			Item->Flags &= ~CHECKED;

				/* If this is the first entry to be added, clear the old dial list. */

			if(!AddedSomething)
				DeleteDialList(GlobalPhoneHandle);

			return(AddDialEntry(GlobalPhoneHandle,GlobalPhoneHandle->Phonebook[Index],NULL));
		}
	}

	return(FALSE);
}

STATIC BOOL
DialMenuToDialList()
{
	BOOL AddedSomething = FALSE;

	if(Menu)
	{
		struct Menu	*ThisMenu;
		struct MenuItem	*Item;

		ClearMenuStrips();

		for(ThisMenu = Menu ; ThisMenu ; ThisMenu = ThisMenu->NextMenu)
		{
			if(Item = ThisMenu->FirstItem)
			{
				do
				{
					AddedSomething |= EnterItemToDialList(AddedSomething,Item);

					if(Item->SubItem)
					{
						struct MenuItem *SubItem = Item->SubItem;

						do
							AddedSomething |= EnterItemToDialList(AddedSomething,SubItem);
						while(SubItem = SubItem->NextItem);
					}
				}
				while(Item = Item->NextItem);
			}
		}

		ResetMenuStrips(Menu);
	}

	return(AddedSomething);
}

	/* HandleMenu(ULONG Code,ULONG Qualifier):
	 *
	 *	Skip along the number of selected menu items and
	 *	handle the associated functions.
	 */

STATIC VOID
HandleMenu(ULONG Code,ULONG Qualifier)
{
	struct MenuItem *MenuItem;

		/* Check until the last menuitem has been
		 * processed.
		 */

	while(Code != MENUNULL)
	{
			/* Pick up the associated menu item. */

		if(MenuItem = ItemAddress(Menu,Code))
		{
			HandleMenuCode((ULONG)(IPTR)GTMENUITEM_USERDATA(MenuItem),Qualifier);

			Code = MenuItem->NextSelect;
		}
		else
			break;
	}

		/* Enter the selected entries from the quick dial menu? */

	if(CheckDialMenu)
	{
		CheckDialMenu = FALSE;

		if(DialMenuToDialList())
			DoDial = DIAL_LIST;
	}
}

	/* HandleIconify():
	 *
	 *	Handle program iconification.
	 */

STATIC VOID
HandleIconify()
{
	BOOL Released = FALSE;

		/* Set the wait mouse pointer... */

	BlockWindows();

		/* Open workbench.library. */

	if(WorkbenchBase)
	{
			/* Open icon.library. */

		if(IconBase)
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			struct DiskObject *Icon;

			strcpy(LocalBuffer,Config->PathConfig->DefaultStorage);

			if(AddPart(LocalBuffer,"term_SleepIcon",sizeof(LocalBuffer)))
				Icon = GetDiskObject(LocalBuffer);
			else
				Icon = NULL;

			if(!Icon)
				Icon = GetDiskObject("PROGDIR:term_SleepIcon");

			if(!Icon)
			{
				if(!(Icon = GetProgramIcon()))
					Icon = GetDefDiskObject(WBTOOL);
			}

				/* Did we get an icon? */

			if(Icon)
			{
				struct MsgPort *IconPort;

					/* Reset the icon type. */

				Icon->do_Type = 0;

					/* Default icon position. */

				Icon->do_CurrentX = NO_ICON_POSITION;
				Icon->do_CurrentY = NO_ICON_POSITION;

					/* Create the Workbench reply port. */

				if(IconPort = CreateMsgPort())
				{
					struct AppIcon *AppIcon;

						/* Add the application icon. */

					if(AppIcon = AddAppIconA(0,0,TermIDString,IconPort,BNULL,Icon,NULL))
					{
						struct AppMessage *AppMessage;

							/* Release the window. */

						Released = TRUE;
						ReleaseWindows();

							/* Close the display. full stop. */

						if(DeleteDisplay())
						{
							ULONG Signals;

								/* Reset and release the serial driver. */

							if(Config->MiscConfig->ReleaseDevice)
							{
								ClearSerial();
								DeleteSerial();
							}

								/* Wait for double-click. */

							IconTerminated = FALSE;

							do
							{
								Signals = Wait(PORTMASK(IconPort) | SIG_REXX | SIGBREAKF_CTRL_F);

									/* Received a double-click? */

								if(Signals & PORTMASK(IconPort))
								{
										/* Pick up application messages. */

									while(AppMessage = (struct AppMessage *)GetMsg(IconPort))
									{
										IconTerminated = TRUE;

										ReplyMsg((struct Message *)AppMessage);
									}
								}

									/* Wake up if ARexx command received. */

								if(Signals & SIG_REXX)
									while(RunJob(RexxJob));

									/* Generic wakeup signal. */

								if(Signals & SIGBREAKF_CTRL_F)
									IconTerminated = TRUE;

									/* Ready to leave? */

								if(IconTerminated)
								{
										/* Open the serial driver. */

									if(Config->MiscConfig->ReleaseDevice)
									{
										if(!CreateSerial(LocalBuffer,sizeof(LocalBuffer)))
										{
											switch(ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_TERMMAIN_ICONIFY_IGNORE_QUIT_TXT),LocalBuffer))
											{
												case 0:

													MainTerminated = TRUE;
													break;

												case 1:

													IconTerminated = FALSE;
													break;
											}
										}
									}
								}

									/* Still ready to leave? */

								if(IconTerminated && !MainTerminated)
								{
									CONST_STRPTR String;

										/* Create the display. */

									if(String = CreateDisplay(TRUE,FALSE))
									{
										switch(ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_TERMMAIN_ICONIFY_QUIT_TXT),String))
										{
											case 0:

												MainTerminated = TRUE;
												break;

											case 1:

													/* Reset and release the serial driver. */

												if(Config->MiscConfig->ReleaseDevice)
												{
													ClearSerial();
													DeleteSerial();
												}

												IconTerminated = FALSE;
												break;
										}
									}
									else
									{
										BumpWindow(Window);

										PubScreenStuff();
									}
								}
							}
							while(!IconTerminated);
						}
						else
						{
							BlockWindows();

							ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocaleString(MSG_TERMMAIN_CANNOT_CLOSE_SCREEN_YET_TXT));

							ReleaseWindows();
						}

							/* Remove the application icon. */

						RemoveAppIcon(AppIcon);

							/* Reply pending messages. */

						while(AppMessage = (struct AppMessage *)GetMsg(IconPort))
							ReplyMsg((struct Message *)AppMessage);
					}
					else
						ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_ADD_APPLICATION_ICON_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

					DeleteMsgPort(IconPort);
				}
				else
					ShowRequest(Window,LocaleString(MSG_GLOBAL_FAILED_TO_CREATE_MSGPORT_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

				FreeDiskObject(Icon);
			}
			else
				ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_OPEN_TOOL_ICON_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
		}
		else
			ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_OPEN_ICON_LIBRARY_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
	}
	else
		ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_OPEN_WORKBENCH_LIBRARY_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

	if(!Released)
		ReleaseWindows();

		/* Finished! */

	DoIconify = FALSE;
}

	/* HandleOnlineCleanup():
	 *
	 *	Perform offline cleanup tasks.
	 */

STATIC VOID
HandleOnlineCleanup(BOOL Hangup)
{
	SoundPlay(SOUND_DISCONNECT);

		/* Execute logoff macro. */

	if(Config->CommandConfig->LogoffMacro[0] && WasOnline)
		SerialCommand(Config->CommandConfig->LogoffMacro);

	StopCall(FALSE);

		/* Keep the current connection costs */

	DisplayPay = StopAccountant();

	SetDialMenu(TRUE);

	if(!Hangup)
		Say(LocaleString(MSG_TERMAUX_CARRIER_LOST_TXT));

		/* Clear the password. */

	Password[0] = 0;
	UserName[0]	= 0;

	CurrentBBSName[0]		= 0;
	CurrentBBSComment[0]	= 0;
	CurrentBBSNumber[0]		= 0;

	if(!Config->SerialConfig->CheckCarrier || Config->SerialConfig->DirectConnection)
	{
		ObtainSemaphore(&OnlineSemaphore);
		Online = WasOnline = FALSE;
		ReleaseSemaphore(&OnlineSemaphore);
	}

	SetActivePattern(NULL);
	SetActiveEntry(GlobalPhoneHandle,NULL);

	DisarmLimit();

		/* Previous configuration available? */

	if(BackupConfig)
	{
			/* Remember old configuration. */

		SaveConfig(Config,PrivateConfig);

			/* Copy configuration. */

		SaveConfig(BackupConfig,Config);

			/* Set up new configuration. */

		ConfigSetup();

			/* Free old configuration. */

		DeleteConfiguration(BackupConfig);

		BackupConfig = NULL;
	}

		/* Display the connection cost next time control */
		/* passes through the main loop. */

	ActivateJob(MainJobQueue,DisplayCostJob);

	DisplayHangup = Hangup;

	if(Config->ModemConfig->RedialAfterHangup || KeepRedialing)
	{
		if(GlobalPhoneHandle->DialList)
		{
			if(!IsListEmpty(GlobalPhoneHandle->DialList))
			{
				ObtainSemaphore(&OnlineSemaphore);
				Online = WasOnline = FALSE;
				ReleaseSemaphore(&OnlineSemaphore);

				DoDial = DIAL_LIST;
			}
			else
			{
				if(KeepRedialing)
				{
					KeepRedialing = FALSE;

					if(GlobalPhoneHandle->AutoExit)
						MainTerminated = TRUE;
				}
			}
		}
		else
			KeepRedialing = FALSE;
	}
}

	/* HandleFlowChange():
	 *
	 *	Handle data flow scanner information.
	 */

STATIC VOID
HandleFlowChange()
{
	if(Online)
	{
		if(FlowInfo.NoCarrier)
		{
			if(Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
			{
					/* Is the carrier still present? */

#ifdef __AROS__
				if(!(GetSerialStatus() & SER_STATF_COMCD))
#else
				if(!(GetSerialStatus() & CIAF_COMCD))
#endif
					FlowInfo.NoCarrier = FALSE;
			}

			if(FlowInfo.NoCarrier)
				SetOnlineState(FALSE);
		}
	}
	else
	{
		if(FlowInfo.Voice)
		{
			UBYTE DateTimeBuffer[256];

			FormatStamp(NULL,DateTimeBuffer,sizeof(DateTimeBuffer),FALSE);

			WakeUp(Window,SOUND_VOICE);

			ConPrintf(LocaleString(MSG_TERMMAIN_INCOMING_VOIC_CALL_TXT),DateTimeBuffer);

			Say(LocaleString(MSG_TERMMAIN_SAY_INCOMING_VOICE_CALL_TXT));
		}

		if(FlowInfo.Ring)
		{
			UBYTE DateTimeBuffer[256];

			FormatStamp(NULL,DateTimeBuffer,sizeof(DateTimeBuffer),FALSE);

			WakeUp(Window,SOUND_RING);

			ConPrintf(LocaleString(MSG_TERMMAIN_INCOMING_CALL_TXT),DateTimeBuffer);

			Say(LocaleString(MSG_GLOBAL_INCOMING_CALL_TXT));
		}

		if(FlowInfo.Connect)
		{
				/* Are we to check the carrier signal? */

			if(Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
			{
					/* No carrier signal present? */

#ifdef __AROS__
				if(GetSerialStatus() & SER_STATF_COMCD)
#else
				if(GetSerialStatus() & CIAF_COMCD)
#endif
					FlowInfo.Connect = FALSE;
			}

			if(FlowInfo.Connect)
			{
				WakeUp(Window,SOUND_CONNECT);

				SetOnlineState(TRUE);

				BaudPending = FALSE;

				SetDialMenu(FALSE);
			}
		}
	}

		/* Check if we are to prompt the user for
		 * file transfer.
		 */

	if(FlowInfo.Signature)
	{
		LONG Type = FlowInfo.Signature - SCAN_SIGDEFAULTUPLOAD + TRANSFERSIG_DEFAULTUPLOAD;

		BlockWindows();

		switch(Type)
		{
			case TRANSFERSIG_DEFAULTUPLOAD:

				switch(UploadPanel(TRUE))
				{
					case UPLOAD_TEXT:

#ifdef BUILTIN_ZMODEM
						if(UseInternalZModem)
						{
							InternalZModemTextUpload();
							break;
						}
#endif	/* BUILTIN_ZMODEM */

						StartSendXPR_AskForFile(TRANSFER_TEXT,TRUE);
						break;

					case UPLOAD_BINARY:

#ifdef BUILTIN_ZMODEM
						if(UseInternalZModem)
						{
							InternalZModemBinaryUpload();
							break;
						}
#endif	/* BUILTIN_ZMODEM */

						StartSendXPR_AskForFile(TRANSFER_BINARY,TRUE);
						break;

					case UPLOAD_ABORT:

						CancelZModem();
						break;

					case UPLOAD_BINARY_FROM_LIST:

						HandleUpload(UPLOAD_BINARY);
						break;

					case UPLOAD_TEXT_FROM_LIST:

						HandleUpload(UPLOAD_TEXT);
						break;
				}

				break;

			case TRANSFERSIG_DEFAULTDOWNLOAD:

#ifdef BUILTIN_ZMODEM
				if(UseInternalZModem)
				{
					ZReceive();
					break;
				}
#endif	/* BUILTIN_ZMODEM */

				switch(UploadPanel(FALSE))
				{
					case UPLOAD_TEXT:

						StartReceiveXPR_AskForFile(TRANSFER_TEXT,TRUE);
						break;

					case UPLOAD_BINARY:

						StartReceiveXPR_AskForFile(TRANSFER_BINARY,TRUE);
						break;
				}

				break;

			case TRANSFERSIG_ASCIIUPLOAD:

				if(ChangeProtocol(Config->TransferConfig->ASCIIUploadLibrary,Config->TransferConfig->ASCIIUploadType))
					StartSendXPR_AskForFile(TRANSFER_ASCII,TRUE);

				ResetProtocol();

				break;

			case TRANSFERSIG_ASCIIDOWNLOAD:

				if(ChangeProtocol(Config->TransferConfig->ASCIIDownloadLibrary,Config->TransferConfig->ASCIIDownloadType))
					StartReceiveXPR_AskForFile(TRANSFER_ASCII,TRUE);

				ResetProtocol();

				break;

			case TRANSFERSIG_TEXTUPLOAD:

#ifdef BUILTIN_ZMODEM
				if(UseInternalZModem)
				{
					InternalZModemTextUpload();
					break;
				}
#endif	/* BUILTIN_ZMODEM */

				if(ChangeProtocol(Config->TransferConfig->TextUploadLibrary,Config->TransferConfig->TextUploadType))
					StartSendXPR_AskForFile(TRANSFER_TEXT,TRUE);

				ResetProtocol();

				break;

			case TRANSFERSIG_TEXTDOWNLOAD:

#ifdef BUILTIN_ZMODEM
				if(UseInternalZModem)
				{
					ZReceive();
					break;
				}
#endif	/* BUILTIN_ZMODEM */

				if(ChangeProtocol(Config->TransferConfig->TextDownloadLibrary,Config->TransferConfig->TextDownloadType))
					StartReceiveXPR_AskForFile(TRANSFER_TEXT,TRUE);

				ResetProtocol();

				break;

			case TRANSFERSIG_BINARYUPLOAD:

#ifdef BUILTIN_ZMODEM
				if(UseInternalZModem)
				{
					InternalZModemBinaryUpload();
					break;
				}
#endif	/* BUILTIN_ZMODEM */

				if(ChangeProtocol(Config->TransferConfig->BinaryUploadLibrary,Config->TransferConfig->BinaryUploadType))
					StartSendXPR_AskForFile(TRANSFER_BINARY,TRUE);

				ResetProtocol();

				break;

			case TRANSFERSIG_BINARYDOWNLOAD:

#ifdef BUILTIN_ZMODEM
				if(UseInternalZModem)
				{
					ZReceive();
					break;
				}
#endif	/* BUILTIN_ZMODEM */

				if(ChangeProtocol(Config->TransferConfig->BinaryDownloadLibrary,Config->TransferConfig->BinaryDownloadType))
					StartReceiveXPR_AskForFile(TRANSFER_BINARY,TRUE);

				ResetProtocol();

				break;
		}

		ReleaseWindows();
	}

	ResetDataFlowFilter();
}

	/* FullHangup():
	 *
	 *	In a nutshell, do the full work required to hang up the line.
	 */

STATIC VOID
FullHangup(BOOL ForceIt)
{
	BlockWindows();

	DispatchRexxDialMsgList(FALSE);

	HangUp();

	ReleaseWindows();

	if(Config->SerialConfig->CheckCarrier && !ForceIt && !Config->SerialConfig->DirectConnection)
		HungUp = TRUE;
	else
	{
		SetOnlineState(FALSE);

		HandleOnlineCleanup(TRUE);
	}
}

	/* AskDial(struct Window *Parent):
	 *
	 *	This is called when the user is about to start dialing. If the
	 *	dial button/menu item is available with the protective mode
	 *	enabled, a message will be displayed, asking if the line
	 *	should be hung up before proceeding.
	 */

STATIC BOOL
AskDial(struct Window *Parent)
{
	if(!DialItemsAvailable)
	{
		if(ShowRequest(Parent,LocaleString(MSG_CANNOT_DIAL_BECAUSE_TXT),LocaleString(MSG_DIAL_CANCEL_TXT)))
			FullHangup(TRUE);
		else
			return(FALSE);
	}

	return(TRUE);
}

	/* GoOnline():
	 *
	 *	Go into online state.
	 */

STATIC VOID
GoOnline()
{
	SetOnlineState(TRUE);

	BaudCount		= 0;
	BaudBuffer[0]	= 0;
	BaudPending		= FALSE;

	Password[0]		= 0;
	UserName[0]		= 0;

	SendStartup		= FALSE;

	DisarmLimit();

	CurrentBBSName[0]		= 0;
	CurrentBBSComment[0]	= 0;
	CurrentBBSNumber[0]		= 0;

	SetActivePattern(NULL);
	SetActiveEntry(GlobalPhoneHandle,NULL);

	SetDialMenu(FALSE);
}

STATIC VOID
ReadyToDial(struct DataDialMsg *DialMsg)
{
	struct GenericDialNode *Node;
	PhoneEntry **Phonebook;
	BOOL DidSomething;

	DidSomething = FALSE;

	AddTail(&RexxDialMsgList,(struct Node *)DialMsg->DialMsg);

	Phonebook = GlobalPhoneHandle->Phonebook;

	while(Node = (struct GenericDialNode *)RemHead(DialMsg->DialList))
	{
		if(Node->Index != -1)
		{
			if(Node->Index >= 0 && Node->Index < GlobalPhoneHandle->NumPhoneEntries)
				DidSomething |= AddDialEntry(GlobalPhoneHandle,Phonebook[Node->Index],NULL);
		}
		else
		{
			if(Node->Number != NULL)
				DidSomething |= AddDialEntry(GlobalPhoneHandle,NULL,Node->Number);
			else
			{
				LONG i;

				for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
				{
					if(!Stricmp(Phonebook[i]->Header->Name,Node->Node.ln_Name))
					{
						if(Phonebook[i]->Header->Number[0])
							DidSomething |= AddDialEntry(GlobalPhoneHandle,Phonebook[i],NULL);
					}
				}
			}
		}

		FreeVecPooled(Node);
	}

	if(DidSomething)
		DoDial = DIAL_LIST;
}

STATIC VOID
HandleUpload(UBYTE Type)
{
	LockGenericList(GenericListTable[GLIST_UPLOAD]);

	if(GenericListCount(GenericListTable[GLIST_UPLOAD]) > 0)
	{
		struct FileTransferInfo *Info;

		if(Info = AllocFileTransferInfo())
		{
			LONG FilesFound;
			APTR OldPtr;
			struct Node *Node;
			BPTR FileLock;
			D_S(struct FileInfoBlock,FileInfo);

			FilesFound = 0;

			DisableDOSRequesters(&OldPtr);

			for(Node = ((struct List *)GenericListTable[GLIST_UPLOAD])->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
			{
				if(FileLock = Lock(Node->ln_Name,ACCESS_READ))
				{
					if(Examine(FileLock,FileInfo))
					{
						if(FileInfo->fib_DirEntryType < 0)
						{
							if(AddFileTransferNode(Info,Node->ln_Name,FileInfo->fib_Size))
								FilesFound++;

							if(Config->TransferConfig->TransferIcons)
							{
								UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
								BPTR InfoLock;

								LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%s.info",Node->ln_Name);

								if(InfoLock = Lock(LocalBuffer,ACCESS_READ))
								{
									if(Examine(InfoLock,FileInfo))
									{
										if(FileInfo->fib_DirEntryType < 0)
										{
											if(AddFileTransferNode(Info,LocalBuffer,FileInfo->fib_Size))
												FilesFound++;
										}
									}

									UnLock(InfoLock);
								}
							}
						}
					}

					UnLock(FileLock);
				}
			}

			ClearGenericList(GenericListTable[GLIST_UPLOAD],TRUE);

			EnableDOSRequesters(OldPtr);

			if(FilesFound)
			{
				BlockWindows();

				ReadyFileTransferInfo(Info,FALSE);

				FileTransferInfo = Info;

				StartSendXPR_FromList(Type == UPLOAD_BINARY ? TRANSFER_BINARY : TRANSFER_TEXT,TRUE);

				ReleaseWindows();
			}
			else
				FreeFileTransferInfo(Info);
		}
	}
	else
		StartSendXPR_AskForFile(Type == UPLOAD_BINARY ? TRANSFER_BINARY : TRANSFER_TEXT,TRUE);

	UnlockGenericList(GenericListTable[GLIST_UPLOAD]);
}

	/* LocalReleaseSerial():
	 *
	 *	Release the serial device driver, then reopen it again.
	 */

STATIC VOID
LocalReleaseSerial()
{
	BOOL Continue,SerialClosed;
	APTR OldPtr;

	DB(kprintf("in: LocalReleaseSerial()\n"));

	ChangeWindowPtr(&OldPtr,(APTR)Window);

		/* This might happen if an ARexx user
		 * released the serial device and
		 * failed to reopen it.
		 */

	if(ReadPort)
		SerialClosed = FALSE;
	else
		SerialClosed = TRUE;

	DB(kprintf("blockwindows\n"));

	BlockWindows();

		/* Prevent catastrophes! */

	if(!Config->MiscConfig->ProtectiveMode)
		Continue = TRUE;
	else
	{
		if(Online && !SerialClosed)
		{
			if(!ShowRequest(Window,LocaleString(MSG_TERMMAIN_YOU_ARE_STILL_ONLINE_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT)))
				Continue = FALSE;
			else
				Continue = TRUE;
		}
		else
			Continue = TRUE;
	}

	DB(kprintf("continue=%ld serialclosed=%ld\n",Continue,SerialClosed));

	if(Continue)
	{
		if(SerialClosed)
		{
			DB(kprintf("reopenserial\n"));

			ReopenSerial();
		}
		else
		{
			DB(kprintf("clearserial\n"));
			ClearSerial();

			DB(kprintf("deleteserial\n"));
			DeleteSerial();

			switch(ShowRequest(Window,LocaleString(MSG_TERMMAIN_UNIT_RESET_AND_RELEASED_TXT),LocaleString(MSG_TERMMAIN_RETURN_QUIT_TXT),Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber))
			{
				case 0:

					MainTerminated = TRUE;
					break;

				case 1:

					ReopenSerial();
					break;

				case 2:

					ActivateJob(MainJobQueue,IconifyJob);
					break;
			}
		}
	}

	RestoreWindowPtr(OldPtr);

	ReleaseWindows();

	DB(kprintf("out: LocalReleaseSerial()\n"));
}

	/* HandleKeyboardInput():
	 *
	 *	Process the keyboard input.
	 */

STATIC VOID
HandleKeyboardInput(UBYTE Char,UWORD Code,ULONG Qualifier,STRPTR InputBuffer,LONG Len)
{
		/* Take care of the numeric keypad. */

	if((Qualifier & IEQUALIFIER_NUMERICPAD) && (Config->EmulationConfig->NumericMode == KEYMODE_APPLICATION))
	{
		STATIC STRPTR StringTable[22][2] =
		{
			{ "0",	"\033Op", },
			{ "1",	"\033Oq", },
			{ "2",	"\033Or", },
			{ "3",	"\033Os", },
			{ "4",	"\033Ot", },
			{ "5",	"\033Ou", },
			{ "6",	"\033Ov", },
			{ "7",	"\033Ow", },
			{ "8",	"\033Ox", },
			{ "9",	"\033Oy", },
			{ "-",	"\033Om", },
			{ "+",	"\033Ol",	/* This should really be a comma */ },
			{ ".",	"\033On", },

			{ "(",	"\033OP", },
			{ "[",	"\033OP", },
			{ "{",	"\033OP", },
			{ "]",	"\033OQ", },
			{ ")",	"\033OQ", },
			{ "}",	"\033OQ", },
			{ "/",	"\033OR", },
			{ "*",	"\033OS", },

			{ "\r",	"\033OM" },
		};

		STATIC struct { UBYTE Code; STRPTR String; } CodeTable[18] =
		{
			{ 0x0F,	"\033Op",	/* "0" */ },
			{ 0x1D,	"\033Oq",	/* "1" */ },
			{ 0x1E,	"\033Or",	/* "2" */ },
			{ 0x1F,	"\033Os",	/* "3" */ },
			{ 0x2D,	"\033Ot",	/* "4" */ },
			{ 0x2E,	"\033Ou",	/* "5" */ },
			{ 0x2F,	"\033Ov",	/* "6" */ },
			{ 0x3D,	"\033Ow",	/* "7" */ },
			{ 0x3E,	"\033Ox",	/* "8" */ },
			{ 0x3F,	"\033Oy",	/* "9" */ },
			{ 0x4A,	"\033Om",	/* "-" */ },
			{ 0x5E,	"\033Ol",	/* "+", but this should really be a comma */ },
			{ 0x3C,	"\033On",	/* "." */ },

			{ 0x5A,	"\033OP",	/* "[" */ },
			{ 0x5B,	"\033OQ",	/* "]" */ },
			{ 0x5C,	"\033OR",	/* "/" */ },
			{ 0x5D,	"\033OS",	/* "*" */ },

			{ 0x43,	"\033OM"	/* <cr> */ },
		};

		STRPTR	String = NULL;
		LONG	i;

		for(i = 0 ; i < 22 ; i++)
		{
			if(Char == StringTable[i][0][0])
			{
				String = StringTable[i][1];

				break;
			}
		}

		if(!String)
		{
			for(i = 0 ; i < 18 ; i++)
			{
				if(Code == CodeTable[i].Code)
				{
					String = CodeTable[i].String;

					break;
				}
			}
		}

		if(String)
		{
			if(ClipInput)
			{
				CloseClip();

				ClipInput = ClipXerox = ClipPrefix = FALSE;
			}

			SerWrite(String,strlen(String));

			Len = 0;
		}
	}

		/* This looks like a raw, or better, now cooked key. */

	if(Len)
	{
		switch(CharType[Char])
		{
			case CHAR_HELP:

				GuideDisplay(CONTEXT_MAIN);

				Len = 0;

				break;

			case CHAR_CURSOR:

				if(ClipInput)
				{
					CloseClip();

					ClipInput = ClipXerox = ClipPrefix = FALSE;
				}

					/* If in cursor key applications mode,
					 * send the corresponding string.
					 */

				if(Config->EmulationConfig->CursorMode == KEYMODE_APPLICATION)
				{
					STATIC STRPTR CursorTable[4] =
					{
						"\033OA",
						"\033OB",
						"\033OC",
						"\033OD"
					};

					SerWrite(CursorTable[Char - CUP],3);
				}
				else
				{
					LONG QualType;

						/* Find the appropriate qualifier. */

					if(Qualifier & SHIFT_KEY)
						QualType = 1;
					else
					{
						if(Qualifier & ALT_KEY)
							QualType = 2;
						else
						{
							if(Qualifier & CONTROL_KEY)
								QualType = 3;
							else
								QualType = 0;
						}
					}

						/* Send the corresponding string. */

					SerialCommand(CursorKeys->Keys[QualType][Char - CUP]);
				}

				Len = 0;

				break;

				/* Any function key pressed? */

			case CHAR_FUNCTION:

				if(ClipInput)
				{
					CloseClip();

					ClipInput = ClipXerox = ClipPrefix = FALSE;
				}

				if(Qualifier & CONTROL_KEY)
					SerialCommand(MacroKeys->Keys[3][Char - FN1]);
				else
				{
					if(Qualifier & ALT_KEY)
						SerialCommand(MacroKeys->Keys[2][Char - FN1]);
					else
					{
						if(Qualifier & SHIFT_KEY)
							SerialCommand(MacroKeys->Keys[1][Char - FN1]);
						else
							SerialCommand(MacroKeys->Keys[0][Char - FN1]);
					}
				}

				Len = 0;

				break;

				/* Anything else? */

			default:

				if(Len == 1 && Char == '\r' && Recording && !RecordingLine && (Qualifier & SHIFT_KEY))
				{
					RememberInputText("\r",1);

					Len = 0;

					PopStatus();
					PushStatus(STATUS_RECORDING_LINE);

					RecordingLine = TRUE;

					RememberResetInput();

					RememberOutput = FALSE;
					RememberInput = TRUE;

					CheckItem(MEN_RECORD_LINE,TRUE);
				}

				break;
		}

			/* Any characters to send? */

		if(Len)
			SendInputTextBuffer(InputBuffer,Len,TRUE,FALSE);
	}
}

	/* HandleSerialCheckJob():
	 *
	 *	Handle routine checkup actions.
	 */

BOOL
HandleSerialCheckJob(JobNode *UnusedJob)
{
		/* Attempt to lock the serial device? */

	if(PollODU)
	{
			/* We don't want to poll too often */

		if(PollODUCount++ >= (5 + ROUTINE_CHECK_INTERVAL - 1) / ROUTINE_CHECK_INTERVAL)
		{
			PollODUCount = 0;

				/* Still supporting the locking protocol? */

			if(Config->SerialConfig->UseOwnDevUnit)
			{
				BlockWindows();

					/* Give it a try */

				if(LockDevice(Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber,NULL,0))
				{
						/* Check */

					ReopenSerial();

					PollODU = FALSE;
				}

				ReleaseWindows();
			}
			else
				PollODU = FALSE;
		}
	}

		/* Take a look at the carrier signal. */

	if(Config->SerialConfig->CheckCarrier && WriteRequest && !Config->SerialConfig->DirectConnection)
	{
		UWORD Status = GetSerialStatus();

			/* Still online? */

		if(Online)
		{
				/* Carrier detect signal lost? */
#ifdef __AROS__
			if(Status & SER_STATF_COMCD) 
#else
			if(Status & CIAF_COMCD)
#endif
				SetOnlineState(FALSE);
		}
		else
		{
				/* Is the carrier detect signal
				 * present?
				 */
#ifdef __AROS__
			if(!(Status & SER_STATF_COMCD))
#else
			if(!(Status & CIAF_COMCD))
#endif
				GoOnline();
		}
	}

		/* Check online time limit. */

	if(CheckLimit())
		LaunchRexxAsync(LimitMacro);

		/* Flush capture file contents to disk,
		 * this routine is executed each minute
		 * in order to store the captured data.
		 */

	if(BufferFlushCount++ >= (60 + ROUTINE_CHECK_INTERVAL - 1) / ROUTINE_CHECK_INTERVAL)
	{
		BufferFlushCount = 0;

			/* Flush the capture file. */

		if(FileCapture)
			BufferFlush(FileCapture);
	}

	return(FALSE);
}

	/* HandleQueueJob(JobNode *UnusedJob):
	 *
	 *	Process the special message queue.
	 */

BOOL
HandleQueueJob(JobNode *UnusedJob)
{
	struct DataMsg *Item;

	if(Item = GetMsgItem(SpecialQueue))
	{
		WORD OldEcho,Type;

		switch(Item->Type)
		{
				/* Output data. */

			case DATAMSGTYPE_WRITE:

				SerWrite(Item->Data,Item->Size);
				break;

				/* Execute a command. */

			case DATAMSGTYPE_SERIALCOMMAND:

				SerialCommand(Item->Data);
				break;

				/* Execute a command, but don't echo it. */

			case DATAMSGTYPE_SERIALCOMMANDNOECHO:

				OldEcho = Config->SerialConfig->Duplex;

				Config->SerialConfig->Duplex = DUPLEX_FULL;

				SerialCommand(Item->Data);

				Config->SerialConfig->Duplex = OldEcho;
				break;

				/* Output contents of clipboard */

			case DATAMSGTYPE_WRITECLIP:

				if(!ClipInput)
				{
					if(!OpenClip(Item->Size))
						ClipInput = ClipXerox = TRUE;
					else
						ClipInput = ClipXerox = FALSE;
				}

					/* Are we reading input from the clipboard? */

				if(ClipInput)
				{
					UBYTE InputBuffer[256];
					LONG Len;

					if((Len = GetClip(InputBuffer,sizeof(InputBuffer) - 1)) < 0)
					{
						CloseClip();

						ClipInput = FALSE;

						if(ClipXerox)
						{
							if(Config->ClipConfig->InsertSuffix[0])
								SerialCommand(Config->ClipConfig->InsertSuffix);

							ClipXerox = FALSE;
						}

						ClipPrefix = FALSE;
					}
					else
					{
						if(!ClipPrefix && ClipXerox)
						{
							if(Config->ClipConfig->InsertPrefix[0])
								SerialCommand(Config->ClipConfig->InsertPrefix);

							ClipPrefix = TRUE;
						}

						if(Len > 0)
							SendInputTextBuffer(InputBuffer,Len,FALSE,Config->ClipConfig->ConvertLF);
					}
				}

				break;

				/* Start an upload */

			case DATAMSGTYPE_UPLOAD:

				Type = Item->Size;

				DeleteMsgItem((struct MsgItem *)Item);
				Item = NULL;

				HandleUpload(Type);
				break;

				/* Call a menu item */

			case DATAMSGTYPE_MENU:

				HandleMenuCode((ULONG)Item->Size,(IPTR)Item->Data);

				break;

				/* Rendezvous with external process */

			case DATAMSGTYPE_RENDEZVOUS:

				if(ReadRequest && WriteRequest)
				{
						/* Abort serial I/O processing */

					ClearSerial();

					BlockWindows();

						/* Return the message, caution, we're not ready yet */

					Forbid();

					DeleteMsgItem((struct MsgItem *)Item);

					Item = NULL;

						/* Prepare to wait... */

					WaitForHandshake();

						/* Pick up the queue */

					RestartSerial();

					Permit();

					ReleaseWindows();
				}

				break;

				/* Discard current buffer contents */

			case DATAMSGTYPE_CLEARBUFFER:

				FreeBuffer();
				break;

				/* Redial current dial list */

			case DATAMSGTYPE_REDIAL:

				if(!Online)
					DoDial = DIAL_LIST;

				break;

				/* Hang up the line */

			case DATAMSGTYPE_HANGUP:

				FullHangup(FALSE);
				break;

				/* Go to online state */

			case DATAMSGTYPE_GOONLINE:

				if(!Online)
					GoOnline();

				break;

				/* Dial a list of numbers */

			case DATAMSGTYPE_DIAL:

				ReadyToDial((struct DataDialMsg *)Item);
				break;

				/* Output data as if it had come from the main window. */

			case DATAMSGTYPE_WRITESTRING:

				SendInputTextBuffer(Item->Data,Item->Size,TRUE,TRUE);
				break;

				/* Output data as if it had come from the main window. */

			case DATAMSGTYPE_WRITESTRING_NOECHO:

				OldEcho = Config->SerialConfig->Duplex;

				Config->SerialConfig->Duplex = DUPLEX_FULL;

				SendInputTextBuffer(Item->Data,Item->Size,TRUE,TRUE);

				Config->SerialConfig->Duplex = OldEcho;
				break;
		}

		if(Item)
			DeleteMsgItem((struct MsgItem *)Item);

		return(TRUE);
	}
	else
		return(FALSE);
}

	/* HandleOwnDevUnitJob():
	 *
	 *	Deal with the OwnDevUnit signal notification.
	 */

BOOL
HandleOwnDevUnitJob(JobNode *UnusedJob)
{
	DB(kprintf("handle ODU\n"));

	if(!Online || !ReadPort || (Online && Config->SerialConfig->ReleaseODUWhenOnline))
	{
		if(Config->SerialConfig->SatisfyODURequests == ODU_RELEASE)
		{
			DB(kprintf("ODU release\n"));
			LocalReleaseSerial();
		}

		if(Config->SerialConfig->SatisfyODURequests == ODU_WAIT)
		{
			BOOL Continue,SerialClosed;
			APTR OldPtr;

			DB(kprintf("ODU wait\n"));

			ChangeWindowPtr(&OldPtr,(APTR)Window);

				/* This might happen if an ARexx user
				 * released the serial device and
				 * failed to reopen it.
				 */

			if(ReadPort)
				SerialClosed = FALSE;
			else
				SerialClosed = TRUE;

			BlockWindows();

				/* Prevent catastrophes! */

			if(Online && !SerialClosed && !Config->SerialConfig->ReleaseODUWhenOnline)
			{
				if(!ShowRequest(Window,LocaleString(MSG_TERMMAIN_YOU_ARE_STILL_ONLINE_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT)))
					Continue = FALSE;
				else
					Continue = TRUE;
			}
			else
				Continue = TRUE;

			if(Continue)
			{
				if(!SerialClosed)
				{
					ClearSerial();

					DeleteSerial();
				}
			}

			RestoreWindowPtr(OldPtr);

			ReleaseWindows();

				/* Start polling for the device to become */
				/* available again */

			PollODU = TRUE;
		}
	}

	DB(kprintf("ODU done\n"));

	return(FALSE);
}

	/* HandleWindowJob(JobNode *UnusedJob):
	 *
	 *	Just makes a call to HandleWindow().
	 */

BOOL
HandleWindowJob(JobNode *UnusedJob)
{
	return(HandleWindow());
}

	/* HandleWindowAndXEMJob(JobNode *UnusedJob):
	 *
	 *	Calls both HandleXEM() and HandleWindow().
	 */

BOOL
HandleWindowAndXEMJob(JobNode *UnusedJob)
{
	HandleXEM();

	return(HandleWindow());
}

	/* HandleWindow():
	 *
	 *	This funny part checks the window(s) for incoming
	 *	user input. Menus are handled elsewhere.
	 */

BOOL
HandleWindow()
{
	STATIC ULONG	ClickSeconds,ClickMicros;
	STATIC ULONG	LastSeconds,LastMicros;
	STATIC BOOL		TestingDoubleClick;

	struct IntuiMessage	*Message;
	ULONG				 MsgClass,Code,Qualifier,Seconds,Micros;
	LONG				 MouseX,MouseY,Len,GadgetID;
	struct Gadget		*Gadget;
	UBYTE				 Char,InputBuffer[256];
	struct Window		*IDCMPWindow;
	BOOL				 Result,WindowJustActivated;

	Result				= FALSE;
	WindowJustActivated	= FALSE;

		/* Are we reading input from the clipboard? */

	if(ClipInput)
	{
		LONG Length = GetClip(InputBuffer,sizeof(InputBuffer) - 1);

		if(Length < 0)
		{
			CloseClip();

			ClipInput = FALSE;

			if(ClipXerox)
			{
				if(Config->ClipConfig->InsertSuffix[0])
					SerialCommand(Config->ClipConfig->InsertSuffix);

				ClipXerox = FALSE;
			}

			ClipPrefix = FALSE;
		}
		else
		{
			if(!ClipPrefix && ClipXerox)
			{
				if(Config->ClipConfig->InsertPrefix[0])
					SerialCommand(Config->ClipConfig->InsertPrefix);

				ClipPrefix = TRUE;
			}

			if(Length > 0)
				SendInputTextBuffer(InputBuffer,Length,FALSE,Config->ClipConfig->ConvertLF);

			Result = TRUE;
		}
	}

		/* Any news in the mail? */

	if(Message = (struct IntuiMessage *)GetMsg(Window->UserPort))
	{
			/* A click into the window should activate it, but
			 * we don't want to have the character snapping activated
			 * under these conditions. In this case we rely upon
			 * Intuition sending the IDCMP_ACTIVEWINDOW and
			 * IDCMP_MOUSEBUTTONS event marked with the same
			 * creation time stamp. Even if the Intuition
			 * implementation should change no harm should
			 * be done.
			 */

		Seconds	= Message->Seconds;
		Micros	= Message->Micros;

		if(Seconds == LastSeconds && Micros == LastMicros && Message->IDCMPWindow == Window)
		{
			if(Message->Class == IDCMP_ACTIVEWINDOW || Message->Class == IDCMP_MOUSEBUTTONS)
				WindowJustActivated = TRUE;
		}

		LastSeconds	= Seconds;
		LastMicros	= Micros;

			/* Pick up the pieces. */

		MsgClass	= Message->Class;
		Code		= Message->Code;
		Qualifier	= Message->Qualifier;
		Gadget		= (struct Gadget *)Message->IAddress;

		MouseX		= Message->MouseX;
		MouseY		= Message->MouseY;

		IDCMPWindow	= Message->IDCMPWindow;

		Len		= 0;
		Char	= 0;

		switch(MsgClass)
		{
			case IDCMP_IDCMPUPDATE:

				GadgetID = GetTagData(GA_ID,0,(struct TagItem *)Gadget);
				break;

			case IDCMP_GADGETUP:
			case IDCMP_GADGETDOWN:

				GadgetID = Gadget->GadgetID;
				break;

			case IDCMP_RAWKEY:

					/* Perform key conversion. */

				if(XEmulatorBase)
				{
					if(Len = XEmulatorUserMon(XEM_IO,InputBuffer,sizeof(InputBuffer) - 1,Message))
						Char = InputBuffer[0];
				}
				else
					Char = KeyConvert(Message,InputBuffer,&Len);

				/* Falls through to... */

			default:

				GadgetID = 0;
				break;
		}

		ReplyMsg((struct Message *)Message);

			/* The following messages originated from
			 * the fast! macro panel.
			 */

		if(IDCMPWindow == FastWindow)
		{
			switch(MsgClass)
			{
					/* Close the window. */

				case IDCMP_CLOSEWINDOW:

					CloseFastWindow();
					return(TRUE);

					/* Gadget or window size change event. */

				case IDCMP_GADGETUP:
				case IDCMP_NEWSIZE:

					HandleFastWindowGadget(MsgClass,Code);
					return(TRUE);
			}
		}

			/* Status window activated? */

		if(IDCMPWindow == StatusWindow)
		{
			switch(MsgClass)
			{
				case IDCMP_ACTIVEWINDOW:

					if(!Config->ScreenConfig->SplitStatus)
						NormalCursor();

					break;

				case IDCMP_CLOSEWINDOW:

					ShakeHands((struct Task *)StatusProcess,SIG_CLOSEWINDOW);

					ClearMenuStrip(StatusWindow);
					CloseWindowSafely(StatusWindow);

					StatusWindow = NULL;
					break;
			}
		}

			/* What about the matrix window? */

		if(IDCMPWindow == MatrixWindow)
		{
			if(DispatchMatrixWindow(&MsgClass,Code,Qualifier,Char))
				CloseMatrixWindow();
		}

			/* Main window message? */

		if(IDCMPWindow == Window)
		{
			switch(MsgClass)
			{
				case IDCMP_GADGETUP:

					if(GadgetID == CHAT_GadgetID)
						HandleChatGadget(Code);

					break;

				case IDCMP_GADGETDOWN:

					if(GadgetID == CHAT_GadgetID)
						MarkChatGadgetAsActive();

					break;

				case IDCMP_INACTIVEWINDOW:

					WindowMarkerInterrupt();

					GhostCursor();

					break;

				case IDCMP_ACTIVEWINDOW:

						/* Take care of the chat gadget if necessary */

					ActivateChat(TRUE);

					NormalCursor();

					break;

				case IDCMP_NEWSIZE:

						/* Is a window clipping region installed? */

					if(ClipRegion)
					{
						struct Rectangle RegionRectangle;

							/* Install old region. */

						InstallClipRegion(Window->WLayer,OldRegion);

							/* Fill in the clipping rectangle. */

						RegionRectangle.MinX = Window->BorderLeft;
						RegionRectangle.MinY = Window->BorderTop;
						RegionRectangle.MaxX = Window->Width - (Window->BorderRight + 1);
						RegionRectangle.MaxY = Window->Height - (Window->BorderBottom + 1);

							/* Clear previous clipping region. */

						ClearRegion(ClipRegion);

							/* Set new clipping region. */

						OrRectRegion(ClipRegion,&RegionRectangle);

							/* Install new clipping region. */

						OldRegion = InstallClipRegion(Window->WLayer,ClipRegion);
					}

					ForceStatusUpdate();

					HandleMenuCode(MEN_RESET_TERMINAL,Qualifier);

						/* Take care of the chat gadget if necessary */

					ActivateChat(TRUE);

					TTYResize();

					break;

				case IDCMP_CLOSEWINDOW:

					HandleMenuCode(MEN_QUIT,Qualifier);
					break;

				case IDCMP_MOUSEMOVE:

					WindowMarkerMoveMouse();
					break;

				case IDCMP_MOUSEBUTTONS:

					if(Code == SELECTUP)
						ActivateChat(TRUE);

					if((!WindowJustActivated || Code != SELECTDOWN) && (!XEmulatorBase || Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL))
					{
						switch(Code)
						{
							case SELECTUP:

								WindowMarkerInterrupt();
								break;

							case SELECTDOWN:

								MouseX -= WindowLeft;

								if(MouseX < 0)
									MouseX = 0;

								if(MouseX > WindowWidth - 1)
									MouseX = WindowWidth - 1;

								MouseY -= WindowTop;

								if(MouseY < 0)
									MouseY = 0;

								if(MouseY > WindowHeight - 1)
									MouseY = WindowHeight - 1;

								if(Qualifier & CONTROL_KEY)
								{
									LONG FirstX,FirstY;

									FirstX = (MouseX * CharCellDenominator) / (TextFontWidth * CharCellNominator);
									FirstY = MouseY / TextFontHeight;

									if(FirstX < RasterWidth && FirstY < RasterHeight)
									{
										UBYTE LocalChar[2];

										SafeObtainSemaphoreShared(&RasterSemaphore);

										LocalChar[0] = Raster[FirstY * RasterWidth + FirstX];

										ReleaseSemaphore(&RasterSemaphore);

										if(LocalChar[0])
										{
											LONG Len;

											if(Qualifier & SHIFT_KEY)
											{
												LocalChar[1] = '\r';
												Len = 2;
											}
											else
												Len = 1;

											SendInputTextBuffer(LocalChar,Len,TRUE,TRUE);
										}
									}

									return(TRUE);
								}

								if((Qualifier & ALT_KEY) && (Qualifier & IEQUALIFIER_LEFTBUTTON))
								{
									LONG DeltaX,DeltaY;

									ObtainSemaphore(&TerminalSemaphore);

									DeltaX = (MouseX * CharCellDenominator) / (TextFontWidth * CharCellNominator)  - CursorX;
									DeltaY = MouseY / TextFontHeight - CursorY;

									ReleaseSemaphore(&TerminalSemaphore);

									if(DeltaX || DeltaY)
									{
										if(DeltaX > 0)
										{
											DeltaX++;

											while(DeltaX--)
												SerWrite("\33[C",3);
										}

										if(DeltaX < 0)
										{
											while(DeltaX++)
												SerWrite("\33[D",3);
										}

										if(DeltaY > 0)
										{
											DeltaY++;

											while(DeltaY--)
												SerWrite("\33[B",3);
										}

										if(DeltaY < 0)
										{
											while(DeltaY++)
												SerWrite("\33[A",3);
										}
									}

									return(TRUE);
								}

								if(TestingDoubleClick)
								{
									ULONG CurrentSeconds,CurrentMicros;

									TestingDoubleClick = FALSE;

									CurrentTime(&CurrentSeconds,&CurrentMicros);

									if(DoubleClick(ClickSeconds,ClickMicros,CurrentSeconds,CurrentMicros))
									{
										WindowMarkWord();

										return(TRUE);
									}
								}
								else
								{
									TestingDoubleClick = TRUE;

									CurrentTime(&ClickSeconds,&ClickMicros);
								}

								WindowMarkerStart(Qualifier);
								break;
						}
					}

					break;
			}
		}

			/* Now for general information. */

		switch(MsgClass)
		{
			case IDCMP_REFRESHWINDOW:

				BeginRefresh(IDCMPWindow);
				EndRefresh(IDCMPWindow,TRUE);
				break;

			case IDCMP_RAWKEY:

				HandleKeyboardInput(Char,Code,Qualifier,InputBuffer,Len);
				break;

				/* A menu item was selected. */

			case IDCMP_MENUPICK:

				HandleMenu(Code,Qualifier);

					/* Take care of the chat gadget if necessary */

				ActivateChat(TRUE);

				break;

				/* Menu help is required. */

			case IDCMP_MENUHELP:

				if(MENUNUM(Code) == NOMENU || MENUNUM(Code) > 9 || ITEMNUM(Code) == NOITEM)
					GuideDisplay(CONTEXT_MAIN_MENU);
				else
					GuideDisplay(CONTEXT_PROJECT_MEN + MENUNUM(Code));

				break;
		}

		return(TRUE);
	}

	return(Result);
}

	/* HandleMenuCode(ULONG Code,ULONG Qualifier):
	 *
	 *	Handle each function associated with a menu code.
	 */

VOID
HandleMenuCode(ULONG Code,ULONG Qualifier)
{
	struct FileRequester	*FileRequest;
	UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];

	struct Window			*ReqWindow;
	struct EasyStruct		 Easy;

	BPTR					 SomeFile;
	APTR					 OldPtr;

	BOOL					 Checked;

	switch(Code)
	{
			/* Save screen as IFF-ILBM file. */

		case MEN_SAVE_AS_PICTURE:

			BlockWindows();

			DummyBuffer[0] = 0;

			if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_SAVE_SCREEN_IFF_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				if(!SaveWindow(DummyBuffer,Window))
					ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
			}

			ReleaseWindows();

			break;

			/* Save screen as ASCII file. */

		case MEN_SAVE_AS_TEXT:

			BlockWindows();

			DummyBuffer[0] = 0;

			if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_SAVE_SCREEN_ASCII_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				LONG Error = 0;

				FreeAslRequest(FileRequest);

				SomeFile = BNULL;

				if(GetFileSize(DummyBuffer))
				{
					switch(ShowRequest(Window,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_TXT),LocaleString(MSG_GLOBAL_CREATE_APPEND_CANCEL_TXT),DummyBuffer))
					{
						case 0:

							SetIoErr(0);
							break;

						case 1:

							SomeFile = Open(DummyBuffer,MODE_NEWFILE);
							break;

						case 2:

							SomeFile = OpenToAppend(DummyBuffer,NULL);
							break;
					}
				}
				else
					SomeFile = Open(DummyBuffer,MODE_NEWFILE);

				if(SomeFile)
				{
					UBYTE *Buffer;
					LONG i,j;

					for(i = 0 ; i < RasterHeight ; i++)
					{
						Buffer = &Raster[i * RasterWidth];

						j = RasterWidth - 1;

						while(j >= 0 && Buffer[j] == ' ')
							j--;

						if(j >= 0)
						{
							SetIoErr(0);

							if(FWrite(SomeFile,Buffer,j + 1,1) < 1)
							{
								Error = IoErr();

								break;
							}
						}

						SetIoErr(0);

						if(FWrite(SomeFile,"\n",1,1) < 1)
						{
							Error = IoErr();

							break;
						}
					}

					Close(SomeFile);

					AddProtection(DummyBuffer,FIBF_EXECUTE);

					if(Config->MiscConfig->CreateIcons)
						AddIcon(DummyBuffer,FILETYPE_TEXT,TRUE);
				}
				else
					Error = IoErr();

				if(Error)
					ShowError(Window,ERR_SAVE_ERROR,Error,DummyBuffer);
			}

			ReleaseWindows();

			break;

			/* Print the screen (pure ASCII). */

		case MEN_PRINT_SCREEN:

			BlockWindows();

			if(RasterEnabled)
				PrintSomething(PRINT_SCREEN);
			else
				ShowRequest(Window,LocaleString(MSG_TERMMAIN_NO_DATA_TO_PRINT_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

			ReleaseWindows();

			break;

			/* Print the screen (graphics). */

		case MEN_PRINT_SCREEN_AS_GFX:

			BlockWindows();

			PrintScreenGfx();

			ReleaseWindows();

			break;

			/* Print the clipboard contents. */

		case MEN_PRINT_CLIP:

			BlockWindows();

			PrintSomething(PRINT_CLIP);

			ReleaseWindows();

			break;

			/* Open/close the terminal capture file. */

		case MEN_CAPTURE_TO_FILE:

			if(FileCapture)
				CloseFileCapture();
			else
				OpenFileCapture(FALSE);

			break;

			/* Open/close the terminal capture file. */

		case MEN_CAPTURE_TO_RAW_FILE:

			if(FileCapture)
				CloseFileCapture();
			else
				OpenFileCapture(TRUE);

			break;

			/* Start/terminate the printer
			 * capture.
			 */

		case MEN_CAPTURE_TO_PRINTER:

			if(PrinterCapture)
				ClosePrinterCapture(TRUE);
			else
				OpenPrinterCapture(FALSE);

			break;

			/* Iconify the program. */

		case MEN_ICONIFY:

			if(Config->MiscConfig->ProtectiveMode)
			{
				if(Online && Config->MiscConfig->ReleaseDevice && !(Qualifier & SHIFT_KEY))
				{
					BlockWindows();

					if(!ShowRequest(Window,LocaleString(MSG_TERMMAIN_YOU_ARE_STILL_ONLINE_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT)))
					{
						ReleaseWindows();

						break;
					}

					ReleaseWindows();
				}
			}

			ActivateJob(MainJobQueue,IconifyJob);

			break;

			/* Say who we are. */

		case MEN_ABOUT:

			if(Qualifier & IEQUALIFIER_LSHIFT)
				StartTerminalTest(Qualifier);
			else
			{
				BlockWindows();
				ShowAbout(FALSE);
				ReleaseWindows();
			}

			break;

			/* Terminate the program. */

		case MEN_QUIT:

			if((Qualifier & SHIFT_KEY) || !Config->MiscConfig->ProtectiveMode)
				MainTerminated = TRUE;
			else
			{
				LONG OldLen,Len;
				STRPTR Buffer;

				OldLen = Len = strlen(LocaleString(MSG_TERMMAIN_REALLY_QUIT_TXT)) + 3;

				if(Online)
					Len += strlen(LocaleString(MSG_WAIT_PROGRAM_IS_STILL_ONLINE_TXT)) + 4;

				if(BufferChanged)
					Len += strlen(LocaleString(MSG_WAIT_REVIEW_BUFFER_NOT_SAVED_TXT)) + 4;

				if(ConfigChanged)
					Len += strlen(LocaleString(MSG_WAIT_CONFIGURATION_HAS_BEEN_CHANGED_TXT)) + 4;

				if(PhonebookChanged)
					Len += strlen(LocaleString(MSG_WAIT_PHONEBOOK_NOT_SAVED_TXT)) + 4;

				if(TranslationChanged)
					Len += strlen(LocaleString(MSG_WAIT_TRANSLATION_TABLES_CHANGED_TXT)) + 4;

				if(MacroChanged)
					Len += strlen(LocaleString(MSG_WAIT_MACRO_KEYS_CHANGED_TXT)) + 4;

				if(CursorKeysChanged)
					Len += strlen(LocaleString(MSG_WAIT_CURSOR_KEYS_CHANGED_TXT)) + 4;

				if(FastMacrosChanged)
					Len += strlen(LocaleString(MSG_WAIT_FAST_MACROS_CHANGED_TXT)) + 4;

				if(HotkeysChanged)
					Len += strlen(LocaleString(MSG_WAIT_HOTKEYS_CHANGED_TXT)) + 4;

				if(SpeechChanged)
					Len += strlen(LocaleString(MSG_WAIT_SPEECH_SETTINGS_CHANGED_TXT)) + 4;

				if(SoundChanged)
					Len += strlen(LocaleString(MSG_WAIT_SOUND_SETTINGS_CHANGED_TXT)) + 4;

				BlockWindows();

				ChangeWindowPtr(&OldPtr,(APTR)Window);

				if(OldLen != Len)
				{
					if(Buffer = (STRPTR)AllocVecPooled(Len,MEMF_ANY))
					{
						LimitedSPrintf(Len,Buffer,"%s\n\n",LocaleString(MSG_TERMMAIN_REALLY_QUIT_TXT));

						if(Online)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_PROGRAM_IS_STILL_ONLINE_TXT));

						if(BufferChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_REVIEW_BUFFER_NOT_SAVED_TXT));

						if(ConfigChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_CONFIGURATION_HAS_BEEN_CHANGED_TXT));

						if(PhonebookChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_PHONEBOOK_NOT_SAVED_TXT));

						if(TranslationChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_TRANSLATION_TABLES_CHANGED_TXT));

						if(MacroChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_MACRO_KEYS_CHANGED_TXT));

						if(CursorKeysChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_CURSOR_KEYS_CHANGED_TXT));

						if(FastMacrosChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_FAST_MACROS_CHANGED_TXT));

						if(HotkeysChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_HOTKEYS_CHANGED_TXT));

						if(SpeechChanged)
							FormatAppend(Buffer," � %s\n",LocaleString(MSG_WAIT_SPEECH_SETTINGS_CHANGED_TXT));

						if(ShowRequest(Window,Buffer,LocaleString(MSG_GLOBAL_YES_NO_TXT)))
							MainTerminated = TRUE;

						FreeVecPooled(Buffer);
					}
					else
						MainTerminated = TRUE;
				}
				else
					MainTerminated = TRUE;

				RestoreWindowPtr(OldPtr);

				ReleaseWindows();
			}

			break;

			/* Feed the contents of the clipboard
			 * into the input stream.
			 */

		case MEN_PASTE:

			if(!OpenClip(Config->ClipConfig->ClipboardUnit))
			{
				ClipInput = TRUE;

				if(Qualifier & SHIFT_KEY)
					ClipXerox = TRUE;
			}
			else
				ClipInput = FALSE;

			break;

		case MEN_COPY:

			WindowMarkerTransfer(Qualifier);
			break;

		case MEN_CLEAR:

			WindowMarkerStop();
			break;

		case MEN_SELECT_ALL:

			WindowMarkerSelectAll();
			break;

			/* Execute an AmigaDOS command. */

		case MEN_EXECUTE_DOS_COMMAND:

			BlockWindows();

				/* Enter the name of the command. */

			if(GetString(TRUE,FALSE,0,LocaleString(MSG_TERMMAIN_ENTER_AMIGADOS_COMMAND_TXT),AmigaDOSCommandBuffer))
			{
				if(AmigaDOSCommandBuffer[0])
					LaunchCommandAsync(AmigaDOSCommandBuffer);
			}

			ReleaseWindows();

			break;

			/* Execute an ARexx script command. */

		case MEN_EXECUTE_REXX_COMMAND:

			BlockWindows();

				/* Get the rexx file name/program. */

			if(GetString(TRUE,FALSE,0,LocaleString(MSG_TERMMAIN_ENTER_AREXX_COMMAND_TXT),ARexxCommandBuffer))
			{
				if(ARexxCommandBuffer[0])
					LaunchRexxAsync(ARexxCommandBuffer);
			}

			ReleaseWindows();

			break;

			/* Turn recording on/off. */

		case MEN_RECORD:

			if(GetItem(MEN_RECORD))
			{
				if(!Recording)
				{
					if(CreateRecord(CurrentBBSName[0] ? CurrentBBSName : LocaleString(MSG_SCREENPANEL_UNKNOWN_TXT)))
					{
						RememberResetOutput();
						RememberResetInput();

						RememberOutput = TRUE;

						Recording = TRUE;
						RecordingLine = FALSE;

						PushStatus(STATUS_RECORDING);

						OnItem(MEN_RECORD_LINE);
					}
				}
			}
			else
			{
				if(Recording)
				{
					FinishRecord();

					RememberOutput = FALSE;
					RememberInput = FALSE;

					Recording = FALSE;
					RecordingLine = FALSE;

					PopStatus();

					OffItem(MEN_RECORD_LINE);
				}
			}

			break;

		case MEN_RECORD_LINE:

			if(Recording)
			{
				if(GetItem(MEN_RECORD))
				{
					if(!RecordingLine)
					{
						PopStatus();
						PushStatus(STATUS_RECORDING_LINE);

						RecordingLine = TRUE;

						RememberResetInput();

						RememberOutput = FALSE;
						RememberInput = TRUE;
					}
				}
				else
				{
					if(RecordingLine)
					{
						PopStatus();
						PushStatus(STATUS_RECORDING);

						RememberSpill();

						RecordingLine = FALSE;

						RememberOutput = TRUE;
						RememberInput = FALSE;
					}
				}
			}

			break;

		case MEN_DISABLE_TRAPS:

			Checked = GetItem(MEN_DISABLE_TRAPS);

			if(Checked || GenericListCount(GenericListTable[GLIST_TRAP]) == 0)
				WatchTraps = FALSE;
			else
				WatchTraps = TRUE;

			break;

			/* Edit the trap settings? */

		case MEN_EDIT_TRAPS:

			BlockWindows();

			TrapPanelConfig(Config,&TrapsChanged);

			CheckItem(MEN_DISABLE_TRAPS,!WatchTraps);

			ReleaseWindows();

			break;

			/* Open the phonebook and dial the
			 * list of entries the user will select.
			 */

		case MEN_PHONEBOOK:

			BlockWindows();

			if(PhonePanel(Qualifier))
			{
				if(AskDial(Window))
					DoDial = DIAL_LIST;
			}

			ReleaseWindows();

			break;

			/* Redial those dial list entries which
			 * we were unable to connect.
			 */

		case MEN_REDIAL:

			BlockWindows();

				/* If the modem is still online, provide help. */

			if(AskDial(Window))
				DoDial = DIAL_LIST;

			ReleaseWindows();

			break;

			/* Dial a single number. */

		case MEN_DIAL_NUMBER:

			BlockWindows();

				/* If the modem is still online, provide help. */

			if(AskDial(Window))
			{
				if(GetString(FALSE,FALSE,0,LocaleString(MSG_TERMMAIN_ENTER_PHONE_NUMBER_TXT),DialNumberBuffer))
				{
					if(DialNumberBuffer[0])
					{
							/* Remove all entries from the list. */

						DeleteDialList(GlobalPhoneHandle);

						if(AddDialEntry(GlobalPhoneHandle,NULL,DialNumberBuffer))
							DoDial = DIAL_LIST;
					}
				}
			}

			ReleaseWindows();

			break;

			/* Send a break across the serial line. */

		case MEN_SEND_BREAK:

			SendBreak();
			break;

			/* Hang up the phone line. */

		case MEN_HANG_UP:

			FullHangup(FALSE);

			break;

			/* Wait a bit... */

		case MEN_WAIT:

			Easy.es_StructSize		= sizeof(struct EasyStruct);
			Easy.es_Flags			= 0;
			Easy.es_Title			= LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
			Easy.es_GadgetFormat	= LocaleString(MSG_GLOBAL_CONTINUE_TXT);
			Easy.es_TextFormat		= LocaleString(MSG_TERMMAIN_WAITING_TXT);

			BlockWindows();

			if(ReqWindow = BuildEasyRequest(Window,&Easy,IDCMP_RAWKEY,NULL))
			{
				ULONG Signals,Mask,Which;
				LONG Seconds;
				BOOL More;

					/* Don't echo serial output. */

			/*	Quiet = TRUE; */

				if(!(Seconds = Config->MiscConfig->WaitDelay))
					Seconds = 1;

				SerialCommand(Config->MiscConfig->WaitString);
				RunJob(SerialJob);

				StartTime(Seconds,0);

				if(ReadPort && !Get_xOFF() && ProcessIO)
					Mask = SIG_SERIAL;
				else
					Mask = 0;

				More = FALSE;

				FOREVER
				{
					Which = Mask | SIG_REXX | SIG_TIMER | PORTMASK(ReqWindow->UserPort);

					if(More)
					{
						Signals = SetSignal(0,Which) & Which;
						More = FALSE;
					}
					else
						Signals = Wait(Which);

					if(Signals & PORTMASK(ReqWindow->UserPort))
					{
						ULONG IDCMP = 0;
						LONG Result;

						Result = SysReqHandler(ReqWindow,&IDCMP,FALSE);

						if(Result == 0 || (Result == -2 && !(IDCMP & IDCMP_RAWKEY)))
						{
							StopTime();

							SerialCommand(Config->MiscConfig->WaitString);

							break;
						}
					}

					if(Signals & SIG_TIMER)
					{
						WaitTime();

						SerialCommand(Config->MiscConfig->WaitString);

						StartTime(Seconds,0);
					}

					if(Signals & Mask)
					{
						More = RunJob(SerialJob);

						if(Get_xOFF())
						{
							Mask = 0;
							More = FALSE;
						}
					}

					if(Signals & SIG_REXX)
					{
						struct RexxMsg *Msg;

						if(Msg = (struct RexxMsg *)GetMsg(TermRexxPort))
						{
							Msg->rm_Result1 = RC_ERROR;
							Msg->rm_Result2 = ERR10_001;

							ReplyMsg((struct Message *)Msg);

							More = TRUE;
						}
					}
				}

				RunJob(SerialJob);

			/*	Quiet = FALSE; */

				FreeSysRequest(ReqWindow);
			}

			ReleaseWindows();

			break;

			/* Flush the serial buffers. */

		case MEN_FLUSH_BUFFER:

			ClearSerial();

			RestartSerial();

			break;

			/* Release the serial device for other
			 * applications.
			 */

		case MEN_RELEASE_DEVICE:

			ActivateJob(MainJobQueue,ReleaseSerialJob);
			break;

		case MEN_UPLOAD_ASCII:

			BlockWindows();

			if(ChangeProtocol(Config->TransferConfig->ASCIIUploadLibrary,Config->TransferConfig->ASCIIUploadType))
				StartSendXPR_AskForFile(TRANSFER_ASCII,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

		case MEN_DOWNLOAD_ASCII:

			BlockWindows();

			if(ChangeProtocol(Config->TransferConfig->ASCIIDownloadLibrary,Config->TransferConfig->ASCIIDownloadType))
				StartReceiveXPR_AskForFile(TRANSFER_ASCII,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

		case MEN_UPLOAD_TEXT:

			BlockWindows();

#ifdef BUILTIN_ZMODEM
			if(UseInternalZModem)
			{
				InternalZModemTextUpload();

				ReleaseWindows();
				break;
			}
#endif	/* BUILTIN_ZMODEM */

			if(ChangeProtocol(Config->TransferConfig->TextUploadLibrary,Config->TransferConfig->TextUploadType))
				StartSendXPR_AskForFile(TRANSFER_TEXT,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

		case MEN_DOWNLOAD_TEXT:

			BlockWindows();

#ifdef BUILTIN_ZMODEM
			if(UseInternalZModem)
			{
				ZReceive();

				ReleaseWindows();
				break;
			}
#endif	/* BUILTIN_ZMODEM */

			if(ChangeProtocol(Config->TransferConfig->TextDownloadLibrary,Config->TransferConfig->TextDownloadType))
				StartReceiveXPR_AskForFile(TRANSFER_TEXT,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

			/* Edit and transfer a file. */

		case MEN_EDIT_AND_UPLOAD_TEXT:

			BlockWindows();

			if(!Config->PathConfig->Editor[0])
				GetString(TRUE,FALSE,0,LocaleString(MSG_TERMMAIN_ENTER_NAME_OF_EDITOR_TO_USE_TXT),Config->PathConfig->Editor);

			if(Config->PathConfig->Editor[0])
			{
				DummyBuffer[0] = 0;

				if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_TERMMAIN_EDIT_AND_TRANSFER_FILE_TXT),LocaleString(MSG_TERMMAIN_EDIT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
				{
					UBYTE CompoundName[2 * MAX_FILENAME_LENGTH];

					FreeAslRequest(FileRequest);

					LimitedStrcpy(sizeof(CompoundName),CompoundName,Config->PathConfig->Editor);
					LimitedStrcat(sizeof(CompoundName),CompoundName," \"");
					LimitedStrcat(sizeof(CompoundName),CompoundName,DummyBuffer);
					LimitedStrcat(sizeof(CompoundName),CompoundName,"\"");

					LaunchCommand(CompoundName);

					BumpWindow(Window);

					if(GetFileSize(DummyBuffer))
					{
						switch(ShowRequest(Window,LocaleString(MSG_TERMMAIN_TRANSFER_FILE_AS_TXT),LocaleString(MSG_TERMMAIN_ASCII_UPLOAD_CANCEL_TXT),FilePart(DummyBuffer)))
						{
							case 1:

								if(ChangeProtocol(Config->TransferConfig->ASCIIUploadLibrary,Config->TransferConfig->ASCIIUploadType))
									StartSendXPR_File(TRANSFER_ASCII,DummyBuffer,TRUE);

								ResetProtocol();

								break;

							case 2:

								if(ChangeProtocol(Config->TransferConfig->TextUploadLibrary,Config->TransferConfig->TextUploadType))
									StartSendXPR_File(TRANSFER_TEXT,DummyBuffer,TRUE);

								ResetProtocol();

								break;
						}
					}
				}
			}

			ReleaseWindows();
			break;

		case MEN_UPLOAD_BINARY:

			BlockWindows();

#ifdef BUILTIN_ZMODEM
			if(UseInternalZModem)
			{
				InternalZModemBinaryUpload();

				ReleaseWindows();
				break;
			}
#endif	/* BUILTIN_ZMODEM */

			if(ChangeProtocol(Config->TransferConfig->BinaryUploadLibrary,Config->TransferConfig->BinaryUploadType))
				StartSendXPR_AskForFile(TRANSFER_BINARY,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

			/* Download some files. */

		case MEN_DOWNLOAD_BINARY:

			BlockWindows();

#ifdef BUILTIN_ZMODEM
			if(UseInternalZModem)
			{
				ZReceive();

				ReleaseWindows();
				break;
			}
#endif	/* BUILTIN_ZMODEM */

			if(ChangeProtocol(Config->TransferConfig->BinaryDownloadLibrary,Config->TransferConfig->BinaryDownloadType))
				StartReceiveXPR_AskForFile(TRANSFER_BINARY,TRUE);

			ResetProtocol();

			ReleaseWindows();

			break;

			/* Clear the contents of the scrollback
			 * buffer.
			 */

		case MEN_CLEAR_BUFFER:

			if(Lines)
			{
				BlockWindows();

				if((Qualifier & SHIFT_KEY) || !Config->MiscConfig->ProtectiveMode)
				{
					FreeBuffer();

					TerminateBuffer();
				}
				else
				{
					if(ShowRequest(Window,LocaleString(MSG_TERMMAIN_BUFFER_STILL_HOLDS_LINES_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Lines))
					{
						FreeBuffer();

						TerminateBuffer();
					}
				}

				ReleaseWindows();
			}

			break;

			/* Display the scrollback buffer.
			 * Notify the scrollback task or
			 * fire it off if appropriate.
			 */

		case MEN_DISPLAY_BUFFER:

			if(!LaunchBuffer())
			{
				BlockWindows();

				ShowRequest(Window,LocaleString(MSG_TERMMAIN_UNABLE_TO_CREATE_BUFFER_TASK_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

				ReleaseWindows();
			}

			break;

			/* Close the buffer display. */

		case MEN_CLOSE_BUFFER:

			TerminateBuffer();
			break;

			/* Is the buffer to be frozen? */

		case MEN_FREEZE_BUFFER:

			BufferFrozen = GetItem(MEN_FREEZE_BUFFER);
			ConOutputUpdate();

			break;

			/* Load the buffer contents from a file. */

		case MEN_OPEN_BUFFER:

			BlockWindows();

			DummyBuffer[0] = 0;

			if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_TERMMAIN_LOAD_BUFFER_TXT),LocaleString(MSG_GLOBAL_LOAD_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				if(GetFileSize(DummyBuffer))
				{
					if(SomeFile = Open(DummyBuffer,MODE_OLDFILE))
					{
						if(Lines)
						{
							switch(ShowRequest(Window,LocaleString(MSG_TERMMAIN_BUFFER_STILL_HOLDS_LINES_TXT),LocaleString(MSG_TERMMAIN_DISCARD_APPPEND_CANCEL_TXT),Lines))
							{
								case 0:

									Close(SomeFile);
									SomeFile = BNULL;
									break;

								case 1:

									FreeBuffer();
									break;
							}
						}

						if(SomeFile)
						{
							LONG Len;

							LineRead(BNULL,NULL,0);

							while((Len = LineRead(SomeFile,DummyBuffer,80)) > 0)
								CaptureParser(ParserStuff,DummyBuffer,Len,(COPTR)AddLine);

							Close(SomeFile);

							BufferChanged = TRUE;
						}
					}
					else
						ShowError(Window,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
				}
			}

			ReleaseWindows();
			break;

			/* Save the contents of the scrollback
			 * buffer to a file (line by line).
			 */

		case MEN_SAVE_BUFFER_AS:

			BlockWindows();

			if(!Lines || !BufferLines)
				ShowRequest(Window,LocaleString(MSG_GLOBAL_NOTHING_IN_THE_BUFFER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
			else
			{
				DummyBuffer[0] = 0;

				if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_SAVE_BUFFER_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
				{
					LONG Error = 0;

					FreeAslRequest(FileRequest);

					SomeFile = BNULL;

						/* If the file we are about
						 * to create already exists,
						 * ask the user whether we are
						 * to create, append or skip
						 * the file.
						 */

					if(GetFileSize(DummyBuffer))
					{
						switch(ShowRequest(Window,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_TXT),LocaleString(MSG_GLOBAL_CREATE_APPEND_CANCEL_TXT),DummyBuffer))
						{
							case 1:

								SomeFile = Open(DummyBuffer,MODE_NEWFILE);
								break;

							case 2:

								SomeFile = OpenToAppend(DummyBuffer,NULL);
								break;
						}
					}
					else
						SomeFile = Open(DummyBuffer,MODE_NEWFILE);

					if(SomeFile)
					{
						LONG i,Len;

							/* Obtain the semaphore required
							 * to gain access to the line buffer
							 */

						SafeObtainSemaphoreShared(&BufferSemaphore);

						for(i = 0 ; i < Lines ; i++)
						{
							Len = BufferLines[i][-1];

							if(Len)
							{
								SetIoErr(0);

								if(FWrite(SomeFile,BufferLines[i],Len,1) < 1)
								{
									Error = IoErr();

									break;
								}
							}

							SetIoErr(0);

							if(FPrintf(SomeFile,"\n") < 1)
							{
								Error = IoErr();

								break;
							}
						}

						ReleaseSemaphore(&BufferSemaphore);

						Close(SomeFile);

						AddProtection(DummyBuffer,FIBF_EXECUTE);

						if(Config->MiscConfig->CreateIcons)
							AddIcon(DummyBuffer,FILETYPE_TEXT,TRUE);

						BufferChanged = FALSE;
					}
					else
						Error = IoErr();

					if(Error)
						ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
				}
			}

			ReleaseWindows();

			break;

			/* Simply clear the screen and move the
			 * cursor to its home position.
			 */

		case MEN_CLEAR_SCREEN:

			ConClear();
			break;

			/* Reset the current text rendering font. */

		case MEN_RESET_FONT:

			ConResetFont();
			break;

			/* Reset the display styles and restore
			 * the colours.
			 */

		case MEN_RESET_STYLES:

			ConResetStyles();
			break;

			/* Reset the whole terminal. */

		case MEN_RESET_TERMINAL:

			ConResetTerminal();
			break;

		case MEN_SET_EMULATION:

			BlockWindows();

			if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
			{
				OptionTitle = LocaleString(MSG_TERMMAIN_EMULATION_PREFERENCES_TXT);

				NewOptions = FALSE;

				XEmulatorOptions(XEM_IO);

				if(NewOptions)
				{
					SetEmulatorOptions(XEM_PREFS_SAVE);

					NewOptions = FALSE;
				}

				OptionTitle = NULL;
			}
			else
			{
				if(EmulationPanel(Window,Config))
				{
					ConfigSetup();

					ConfigChanged = TRUE;
				}
			}

			ReleaseWindows();

			break;

			/* Set the serial preferences. */

		case MEN_SERIAL:

			BlockWindows();

			if(SerialPanel(Window,Config))
			{
				ConfigSetup();

				ConfigChanged = TRUE;
			}

			ReleaseWindows();

			break;

			/* Set the modem preferences. */

		case MEN_MODEM:

			BlockWindows();

			if(ModemPanel(Window,Config))
			{
				ResetDataFlowFilter();

				ConfigChanged = TRUE;
			}

			ReleaseWindows();

			break;

			/* Set the screen preferences. */

		case MEN_SCREEN:

			BlockWindows();

			if(ScreenPanel(Window,Config))
			{
				if(memcmp(PrivateConfig->ScreenConfig->Colours,Config->ScreenConfig->Colours,sizeof(UWORD) * 16))
				{
					switch(Config->ScreenConfig->ColourMode)
					{
						case COLOUR_EIGHT:

							CopyMem(Config->ScreenConfig->Colours,ANSIColours,16 * sizeof(UWORD));
							break;

						case COLOUR_SIXTEEN:

							CopyMem(Config->ScreenConfig->Colours,EGAColours,16 * sizeof(UWORD));
							break;

						case COLOUR_AMIGA:

							CopyMem(Config->ScreenConfig->Colours,DefaultColours,16 * sizeof(UWORD));
							break;

						case COLOUR_MONO:

							CopyMem(Config->ScreenConfig->Colours,AtomicColours,16 * sizeof(UWORD));
							break;
					}
				}

				ConfigSetup();

				ConfigChanged = TRUE;
			}
			else
			{
				if(memcmp(PrivateConfig->ScreenConfig->Colours,Config->ScreenConfig->Colours,sizeof(UWORD) * 16))
				{
					switch(Config->ScreenConfig->ColourMode)
					{
						case COLOUR_EIGHT:

							CopyMem(Config->ScreenConfig->Colours,ANSIColours,16 * sizeof(UWORD));
							break;

						case COLOUR_SIXTEEN:

							CopyMem(Config->ScreenConfig->Colours,EGAColours,16 * sizeof(UWORD));
							break;

						case COLOUR_AMIGA:

							CopyMem(Config->ScreenConfig->Colours,DefaultColours,16 * sizeof(UWORD));
							break;

						case COLOUR_MONO:

							CopyMem(Config->ScreenConfig->Colours,AtomicColours,16 * sizeof(UWORD));
							break;
					}

					ConfigChanged = TRUE;
				}
			}

			ReleaseWindows();

			break;

			/* Set the terminal preferences. */

		case MEN_TERMINAL:

			BlockWindows();

			if(!TerminalPanel(Window,Config))
				ReleaseWindows();
			else
			{
				ReleaseWindows();

				Update_CR_LF_Translation();

				ConfigSetup();

				ConfigChanged = TRUE;

					/* Make the changes now */

				if(FixScreenSize && !ResetDisplay)
				{
					ScreenSizeStuff();

					ForceStatusUpdate();

					HandleMenuCode(MEN_RESET_TERMINAL,0);
				}
			}

			break;

			/* Set the clipboard preferences. */

		case MEN_CLIPBOARD:

			BlockWindows();

			if(ClipPanel(Window,Config))
			{
				ConfigSetup();

				ConfigChanged = TRUE;
			}

			ReleaseWindows();

			break;

			/* Set the capture preferences. */

		case MEN_CAPTURE:

			BlockWindows();

			if(CapturePanel(Window,Config))
			{
				ConOutputUpdate();

				ConfigSetup();

				ConfigChanged = TRUE;
			}

			ReleaseWindows();

			break;

			/* Set the command preferences. */

		case MEN_COMMANDS:

			BlockWindows();

			if(CommandPanel(Window,Config))
				ConfigChanged = TRUE;

			ReleaseWindows();

			break;

			/* Set the miscellaneous preferences. */

		case MEN_MISC:

			BlockWindows();

			if(MiscPanel(Window,Config))
			{
				ConfigSetup();

				ConfigChanged = TRUE;
			}

			ReleaseWindows();

			break;

			/* Set the path settings. */

		case MEN_PATH:

			BlockWindows();

			if(PathPanel(Window,Config))
				ConfigChanged = TRUE;

			ReleaseWindows();

			break;

			/* Set the file transfer options. */

		case MEN_TRANSFER:

			BlockWindows();

#ifdef BUILTIN_ZMODEM
			if(UseInternalZModem)
			{
				ZSettings();

				ReleaseWindows();

				break;
			}
#endif	/* BUILTIN_ZMODEM */

			XprIO->xpr_filename = NULL;

				/* Set up the library options. */

			if(XProtocolBase)
			{
				XPRCommandSelected = FALSE;

				ClearSerial();

				NewOptions = FALSE;

				TransferBits = XProtocolSetup(XprIO);

				RestartSerial();

				DeleteTransferPanel(TRUE);

					/* Successful? */

				if(!(TransferBits & XPRS_SUCCESS))
				{
					ShowRequest(Window,LocaleString(MSG_GLOBAL_FAILED_TO_SET_UP_PROTOCOL_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LastXprLibrary);

					CloseLibrary(XProtocolBase);

					XProtocolBase = NULL;

					LastXprLibrary[0] = 0;

					TransferBits = 0;

					SetTransferMenu(TRUE);
				}
				else
					SaveProtocolOpts();
			}

			ReleaseWindows();

			break;

			/* Set the file transfer procol settings. */

		case MEN_TRANSFER_PROTOCOL:

			BlockWindows();

			if(LibPanel(Window,Config))
			{
				ConfigSetup();

				ConfigChanged = TRUE;

				ResetDataFlowFilter();
			}

			ReleaseWindows();

			break;

			/* Set the translation tables. */

		case MEN_TRANSLATION:

			BlockWindows();

			TranslationPanelConfig(Config,&SendTable,&ReceiveTable,LastTranslation,Window,&TranslationChanged);

				/* Choose the right console write routine. */

			ConOutputUpdate();

			ReleaseWindows();

			break;

			/* Set the keyboard macros. */

		case MEN_MACROS:

			BlockWindows();

			if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
			{
				XEmulatorMacroKeyFilter(XEM_IO,NULL);

				MacroPanelConfig(Config,MacroKeys,LastMacros,Window,&MacroChanged);

				SetupXEM_MacroKeys(MacroKeys);
			}
			else
				MacroPanelConfig(Config,MacroKeys,LastMacros,Window,&MacroChanged);

			ReleaseWindows();

			break;

			/* Set the cursor keys. */

		case MEN_CURSORKEYS:

			BlockWindows();

			CursorPanelConfig(Config,CursorKeys,LastCursorKeys,Window,&CursorKeysChanged);

			ReleaseWindows();

			break;

			/* Set the fast macros. */

		case MEN_FAST_MACROS:

			BlockWindows();

			if(FastMacroPanelConfig(Config,&FastMacroList,LastFastMacros,Window,&FastMacrosChanged))
			{
				FastMacroCount = GetListSize(&FastMacroList);
				RefreshFastWindow();
			}

			ReleaseWindows();

			break;

			/* Set the hotkey preferences. */

		case MEN_HOTKEYS:

			BlockWindows();

			if(HotkeyPanelConfig(Config,&Hotkeys,LastKeys,&HotkeysChanged))
			{
				if(LastKeys[0] && !SetupCx())
					ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_SET_UP_HOTKEYS_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
			}

			ReleaseWindows();

			break;

			/* Set the speech preferences. */

		case MEN_SPEECH:

			BlockWindows();

			SpeechPanelConfig(&SpeechConfig,LastSpeech,&SpeechChanged);

			ReleaseWindows();

			break;

			/* Set the sound preferences. */

		case MEN_SOUND:

			BlockWindows();

			if(SoundPanelConfig(&SoundConfig,LastSound,&SoundChanged))
			{
				if(LastSound[0])
					SoundInit();
			}

			ReleaseWindows();

			break;

			/* Edit the phone number patterns and rates. */

		case MEN_RATES:

			BlockWindows();

			PatternPanelConfig(PatternList,LastPattern,&PatternsChanged);

			ReleaseWindows();

			break;

			/* Open the preferences settings. */

		case MEN_OPEN_SETTINGS:

			BlockWindows();

			strcpy(DummyBuffer,LastConfig);

			if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_TERMMAIN_OPEN_PREFERENCES_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				if(ReadConfig(DummyBuffer,PrivateConfig))
				{
					SwapConfig(PrivateConfig,Config);

					strcpy(LastConfig,DummyBuffer);

					ConfigSetup();

					ConfigChanged = FALSE;
				}
				else
					ShowRequest(Window,LocaleString(MSG_GLOBAL_ERROR_OPENING_FILE_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),DummyBuffer);
			}

			ReleaseWindows();

			break;

			/* Save the terminal preferences. */

		case MEN_SAVE_SETTINGS:

			if(LastConfig[0])
			{
				BlockWindows();

				if(!Screen)
					PutWindowInfo(WINDOW_MAIN,Window->LeftEdge,Window->TopEdge,Window->Width,Window->Height);

				if(!WriteConfig(LastConfig,Config))
					ShowError(Window,ERR_SAVE_ERROR,IoErr(),LastConfig);
				else
					ConfigChanged = FALSE;

				ReleaseWindows();

				break;
			}

			/* Save the terminal preferences to a
			 * given file name.
			 */

		case MEN_SAVE_SETTINGS_AS:

			BlockWindows();

			strcpy(DummyBuffer,LastConfig);

			if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_SAVE_PREFERENCES_AS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				if(!Screen)
					PutWindowInfo(WINDOW_MAIN,Window->LeftEdge,Window->TopEdge,Window->Width,Window->Height);

				if(WriteConfig(DummyBuffer,Config))
				{
					strcpy(LastConfig,DummyBuffer);

					ConfigChanged = FALSE;
				}
				else
					ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
			}

			ReleaseWindows();

			break;

			/* Show terminal information window. */

		case MEN_STATUS_WINDOW:

			if(InfoWindow)
				CloseInfoWindow();
			else
				OpenInfoWindow();

			break;

		case MEN_REVIEW_WINDOW:

			if(ReviewWindow)
				DeleteReview();
			else
				CreateReview();

			break;

		case MEN_MATRIX_WINDOW:

			if(MatrixWindow)
				CloseMatrixWindow();
			else
				OpenMatrixWindow(Window);

			break;

			/* Open the packet window if necessary, else
			 * just activate it.
			 */

		case MEN_PACKET_WINDOW:

			CreatePacketWindow();
			break;

			/* Enable or disable the packet window */

		case MEN_CHAT_LINE:

			Checked = GetItem(MEN_CHAT_LINE);

			if(Checked != ChatMode)
			{
				ChatMode = Checked;

				ResetDisplay = TRUE;
				ActivateJob(MainJobQueue,ResetDisplayJob);
			}

			break;

			/* Toggle the presence of the fast! macro panel. */

		case MEN_FAST_MACROS_WINDOW:

			if(FastWindow)
				CloseFastWindow();
			else
				OpenFastWindow();

			break;

			/* Open the upload queue window. */

		case MEN_UPLOAD_QUEUE_WINDOW:

			CreateQueueProcess();
			break;

			/* Check if we should go for the quick dial menu */

		default:

			if(Code >= DIAL_MENU_LIMIT)
				CheckDialMenu = TRUE;

			break;
	}
}

	/* HandleReleaseSerial():
	 *
	 *	Release the serial device driver, then reopen it again.
	 */

BOOL
HandleReleaseSerial(JobNode *UnusedJob)
{
	if(!MainTerminated)
		LocalReleaseSerial();

	return(FALSE);
}

	/* HandleStartupFile(JobNode *Job):
	 *
	 *	Launches the startup ARexx script.
	 */

BOOL
HandleStartupFile(JobNode *UnusedJob)
{
	if(!MainTerminated)
	{
		LaunchRexxAsync(StartupFile);

		StartupFile[0] = 0;
	}

	return(FALSE);
}

BOOL
HandleIconifyJob(JobNode *UnusedJob)
{
	if(!MainTerminated)
		HandleIconify();

	return(FALSE);
}

BOOL
HandleRebuildMenuJob(JobNode *UnusedJob)
{
	if(!ResetDisplay && !MainTerminated)
	{
		struct Menu *Menu;

		if(Menu = BuildMenu())
			AttachMenu(Menu);
		else
			DisconnectDialMenu();

		RebuildMenu = FALSE;
	}

	return(FALSE);
}

BOOL
HandleResetDisplayJob(JobNode *UnusedJob)
{
	if(!MainTerminated)
	{
		if(!DisplayReset())
			MainTerminated = TRUE;
	}

	return(FALSE);
}

BOOL
HandleOnlineCleanupJob(JobNode *UnusedJob)
{
	if(!MainTerminated)
	{
		HandleOnlineCleanup(HungUp);

		WasOnline	= FALSE;
		HungUp		= FALSE;
	}

	return(FALSE);
}

BOOL
HandleDisplayCostJob(JobNode *Job)
{
	BOOL TurnOff = TRUE;

	if(!MainTerminated)
	{
		if(ResetDisplay)
			TurnOff = FALSE;
		else
		{
			UBYTE Sum[20];
			LONG ID;

				/* Reset the text rendering styles, font, etc. in
				 * order to keep the following text from getting
				 * illegible.
				 */

			SoftReset();

			if(DisplayPay)
			{
				if(DisplayHangup)
					ID = MSG_TERMMAIN_LOGMSG_HANG_UP_COST_TXT;
				else
					ID = MSG_TERMAUX_CARRIER_LOST_COST_TXT;

				CreateSum((DisplayPay + 5000) / 10000,TRUE,Sum,sizeof(Sum));

					/* Display how much we expect
					 * the user will have to pay for
					 * this call.
					 */

				ConPrintf(LocaleString(MSG_TERMMAIN_THIS_CALL_WILL_COST_YOU_TXT),Sum);
			}
			else
			{
				if(DisplayHangup)
					ID = MSG_TERMMAIN_LOGMSG_HANG_UP_TXT;
				else
					ID = MSG_TERMAUX_CARRIER_LOST_TXT;

				Sum[0] = 0;
			}

			LogAction(LocaleString(ID),Sum);

			DisplayHangup	= FALSE;
			DisplayPay		= 0;
		}
	}

	if(TurnOff)
		SuspendJob(MainJobQueue,Job);

	return(FALSE);
}
