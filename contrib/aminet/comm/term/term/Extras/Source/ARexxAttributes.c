/*
**	ARexxAttributes.c
**
**	ARexx interface attribute handling routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* ToBoolean(LONG Value):
	 *
	 *	Map an index value to a boolean string.
	 */

STATIC CONST_STRPTR
ToBoolean(LONG Value)
{
	if(Value)
		return(BooleanMappings[1]);
	else
		return(BooleanMappings[0]);
}

	/* FromBoolean(STRPTR Value):
	 *
	 *	Map a boolean value to an index value.
	 */

STATIC LONG
FromBoolean(CONST_STRPTR Value)
{
	STATIC CONST_STRPTR BoolTrueMappings[] =
	{
		"TRUE",
		"ON",
		"YES",
		"ENABLED",
		"AVAILABLE",
		NULL
	};

	LONG i;

	for(i = 0 ; BoolTrueMappings[i] ; i++)
	{
		if(!Stricmp(Value,BoolTrueMappings[i]))
			return(TRUE);
	}

	return(FALSE);
}

	/* ToMap(LONG Index,STRPTR *Mappings):
	 *
	 *	Map an index to a string.
	 */

STATIC CONST_STRPTR
ToMap(LONG Index,CONST_STRPTR *Mappings)
{
	LONG i;

	for(i = 0 ; Mappings[i] ; i++)
	{
		if(Index == i)
			return(Mappings[i]);
	}

	return(Mappings[0]);
}

	/* FromMap(STRPTR String,STRPTR *Mappings):
	 *
	 *	Map a string to an index number.
	 */

STATIC LONG
FromMap(CONST_STRPTR String,CONST_STRPTR *Mappings)
{
	LONG i;

	for(i = 0 ; Mappings[i] ; i++)
	{
		if(!Stricmp(String,Mappings[i]))
			return(i);
	}

	return(-1);
}

	/* ToIndex(STRPTR Name,WORD *Count1,WORD *Count2):
	 *
	 *	Turn an index name into an index and count key.
	 */

STATIC LONG
ToIndex(STRPTR Name,WORD *Count1,WORD *Count2)
{
	BOOL FoundIt,NodeIsNumeric;
	LONG i,Level,Top,Index;
	UBYTE NodeName[40];

	Level	= 0;
	Top		= 0;
	Index	= -1;
	FoundIt	= FALSE;

		/* Reset counter variables. */

	*Count1 = *Count2 = -1;

		/* Skip leading blank spaces. */

	while(*Name == ' ' || *Name == '\t')
		Name++;

		/* Scan until end of string found. */

	while(*Name)
	{
			/* Get the node name. */

		i = 0;

		while(*Name != '.' && *Name != ' ' && *Name != '\t' && *Name)
			NodeName[i++] = *Name++;

		NodeName[i] = 0;

		DB(kprintf("node name is |%s|\n",NodeName));

			/* Is the name really a number? */

		NodeIsNumeric = IsNumeric(NodeName);

		DB(kprintf("node name numeric = %ld\n",NodeIsNumeric));

			/* Skip any trailing spaces. */

		while(*Name == ' ' || *Name == '\t')
			Name++;

			/* Is a component name to follow? */

		if(*Name == '.')
		{
				/* Skip dot and trailing spaces. */

			do
				Name++;
			while(*Name == ' ' || *Name == '\t');
		}
		else
			Name = "";	/* Last iteration. */

			/* Try to find the corresponding keyword in the tree. */

		FoundIt = FALSE;

		DB(kprintf("level is %ld\n",Level));

		for(i = Top ; i < AttributeTableSize ; i++)
		{
				/* Is it on the same tree level? Note: code will
				 * scan into the branches but will not move back up
				 * in the tree.
				 */

			if(AttributeTable[i].Level == Level)
			{
					/* Is a keyword required? */

				if(AttributeTable[i].Name)
				{
						/* Does the name match? */

					if(!Stricmp(AttributeTable[i].Name,NodeName))
					{
						DB(kprintf("matching name found\n"));

							/* Remember new anchor point. */

						Index	= i;
						Top		= i + 1;

						FoundIt = TRUE;

						break;
					}
				}
				else
				{
					DB(kprintf("no name given\n"));

						/* Is the name really a number? */

					if(NodeIsNumeric)
					{
							/* Store the counter. */

						if(*Count1 == -1)
							*Count1 = Atol(NodeName);
						else
							*Count2 = Atol(NodeName);

						DB(kprintf("using the entry; count1=%ld count2=%ld\n",*Count1,*Count2));

							/* Remember new anchor point. */

						Index	= i;
						Top	= i + 1;

						FoundIt = TRUE;

						break;
					}
				}
			}
			else
			{
					/* Don't move back up in the tree! */

				if(AttributeTable[i].Level < Level)
					break;
			}
		}

		if(FoundIt)
			Level++;
		else
			break;
	}

		/* In case the last entry was found, return the index. */

	if(FoundIt)
		return(Index);
	else
		return(-1);
}

	/* FromIndex(LONG Index,LONG Count1,LONG Count2):
	 *
	 *	Turn an index/count key into a neat index name.
	 */

STATIC STRPTR
FromIndex(LONG Index,LONG Count1,LONG Count2,BOOL IncludeRoot)
{
	STATIC UBYTE NameBuffer[160];

	LONG Levels[4],i;

		/* Reset the level indices. */

	for(i = 1 ; i < 4 ; i++)
		Levels[i] = -1;

		/* Fill in the leaf level. */

	Levels[0] = Index;

		/* Top node? */

	if(Index > 0)
	{
		LONG Count = 1,Level = AttributeTable[Index].Level;

			/* Move up in the tree. */

		for(i = Index - 1 ; i >= 0 ; i--)
		{
				/* Previous level? */

			if(AttributeTable[i].Level == Level - 1)
			{
					/* Store index. */

				Levels[Count++] = i;

				Level--;
			}
		}
	}

		/* Clear name buffer. */

	NameBuffer[0] = 0;

		/* Scan backwards. */

	for(i = 3 ; i >= 0 ; i--)
	{
			/* Valid level index? */

		if(Levels[i] != -1)
		{
				/* First name? */

			if(!NameBuffer[0])
				strcpy(NameBuffer,AttributeTable[Levels[i]].Name);
			else
			{
					/* Valid name string? */

				if(AttributeTable[Levels[i]].Name)
				{
					LimitedStrcat(sizeof(NameBuffer),NameBuffer,".");
					LimitedStrcat(sizeof(NameBuffer),NameBuffer,AttributeTable[Levels[i]].Name);
				}
				else
				{
					UBYTE Number[20];

						/* Add an index number. */

					if(Count1 != -1)
					{
						LimitedSPrintf(sizeof(Number),Number,".%ld",Count1);

						Count1 = -1;
					}
					else
						LimitedSPrintf(sizeof(Number),Number,".%ld",Count2);

					LimitedStrcat(sizeof(NameBuffer),NameBuffer,Number);
				}
			}
		}
	}

		/* Return the name string. */

	if(IncludeRoot)
		return(NameBuffer);
	else
	{
		STRPTR Name = NameBuffer;

		while(*Name++ != '.');

		return(Name);
	}
}

	/* GetSingleAttribute(LONG ID,LONG Index,LONG Count1,STRPTR Input):
	 *
	 *	Beware the MONSTER!
	 */

STATIC LONG
GetSingleAttribute(LONG ID,LONG Index,LONG Count1, CONST_STRPTR *Result)
{
	STATIC UBYTE ResultBuffer[MAXPUBSCREENNAME + 1];

	LONG Number,Error;
	STRPTR String;

	Error	= 0;
	String	= NULL;
	Number	= 0;

	switch(ID)
	{
		case ATTR_APPLICATION_VERSION:

			LimitedSPrintf(sizeof(ResultBuffer),ResultBuffer,"%ld.%ld",TermVersion,TermRevision);

			String = ResultBuffer;

			break;

		case ATTR_APPLICATION_SCREEN:

			if(GetPubScreenName(Window->WScreen,ResultBuffer))
				String = ResultBuffer;
			else
				Error = ERROR_OBJECT_NOT_FOUND;

			break;

		case ATTR_APPLICATION_SESSION_ONLINE:

			Number = Online;
			break;

		case ATTR_APPLICATION_SESSION_SESSIONSTART:

			if(!FormatStamp(&SessionStart,ResultBuffer,sizeof(ResultBuffer),FALSE))
				strcpy(ResultBuffer,"-");

			String = ResultBuffer;

			break;

		case ATTR_APPLICATION_SESSION_BYTESSENT:

			Number = BytesOut;
			break;

		case ATTR_APPLICATION_SESSION_BYTESRECEIVED:

			Number = BytesIn;
			break;

		case ATTR_APPLICATION_SESSION_CONNECTMESSAGE:

			String = BaudBuffer;
			break;

		case ATTR_APPLICATION_SESSION_BBSNAME:

			String = CurrentBBSName;
			break;

		case ATTR_APPLICATION_SESSION_BBSNUMBER:

			String = CurrentBBSNumber;
			break;

		case ATTR_APPLICATION_SESSION_BBSCOMMENT:

			String = CurrentBBSComment;
			break;

		case ATTR_APPLICATION_SESSION_USERNAME:

			String = UserName;
			break;

		case ATTR_APPLICATION_SESSION_ONLINEMINUTES:

			Number = OnlineMinutes;
			break;

		case ATTR_APPLICATION_SESSION_ONLINECOST:

			Number = (QueryAccountantCost() + 5000) / 10000;
			break;

		case ATTR_APPLICATION_AREXX:

			String = RexxPortName;
			break;

		case ATTR_APPLICATION_LASTERROR:

			Number = LastRexxError;
			break;

		case ATTR_APPLICATION_TERMINAL_ROWS:

			if(XEmulatorBase && XEM_IO)
			{
				if(XEmulatorBase->lib_Version >= 4)
				{
					ULONG LocalResult;

					LocalResult = XEmulatorInfo(XEM_IO,XEMI_CONSOLE_DIMENSIONS);

					Number = XEMI_EXTRACT_LINES(LocalResult);
				}
				else
					Number = 0;
			}
			else
				Number = LastLine + 1;

			break;

		case ATTR_APPLICATION_TERMINAL_COLUMNS:

			if(XEmulatorBase && XEM_IO)
			{
				if(XEmulatorBase->lib_Version >= 4)
				{
					ULONG LocalResult;

					LocalResult = XEmulatorInfo(XEM_IO,XEMI_CONSOLE_DIMENSIONS);

					Number = XEMI_EXTRACT_COLUMNS(LocalResult);
				}
				else
					Number = 0;
			}
			else
				Number = LastColumn + 1;

			break;

		case ATTR_APPLICATION_BUFFER_SIZE:

			Number = BufferSpace;
			break;

		case ATTR_PREFS_SERIAL_BAUDRATE:

			Number = Config->SerialConfig->BaudRate;
			break;

		case ATTR_PREFS_SERIAL_USE_OWNDEVUNIT:

			Number = Config->SerialConfig->UseOwnDevUnit;
			break;

		case ATTR_PREFS_SERIAL_DIRECT_CONNECTION:

			Number = Config->SerialConfig->DirectConnection;
			break;

		case ATTR_PREFS_SERIAL_BREAKLENGTH:

			Number = Config->SerialConfig->BreakLength;
			break;

		case ATTR_PREFS_SERIAL_BUFFERSIZE:

			Number = Config->SerialConfig->SerialBufferSize;
			break;

		case ATTR_PREFS_SERIAL_QUANTUM:

			Number = Config->SerialConfig->Quantum;
			break;

		case ATTR_PREFS_SERIAL_DEVICENAME:

			String = Config->SerialConfig->SerialDevice;
			break;

		case ATTR_PREFS_SERIAL_UNIT:

			Number = Config->SerialConfig->UnitNumber;
			break;

		case ATTR_PREFS_SERIAL_BITSPERCHAR:

			Number = Config->SerialConfig->BitsPerChar;
			break;

		case ATTR_PREFS_SERIAL_PARITYMODE:

			Number = Config->SerialConfig->Parity;
			break;

		case ATTR_PREFS_SERIAL_STOPBITS:

			Number = Config->SerialConfig->StopBits;
			break;

		case ATTR_PREFS_SERIAL_HANDSHAKINGMODE:

			Number = Config->SerialConfig->HandshakingProtocol;
			break;

		case ATTR_PREFS_SERIAL_DUPLEXMODE:

			Number = Config->SerialConfig->Duplex;
			break;

		case ATTR_PREFS_SERIAL_XONXOFF:

			Number = Config->SerialConfig->xONxOFF;
			break;

		case ATTR_PREFS_SERIAL_HIGHSPEED:

			Number = Config->SerialConfig->HighSpeed;
			break;

		case ATTR_PREFS_SERIAL_SHARED:

			Number = Config->SerialConfig->Shared;
			break;

		case ATTR_PREFS_SERIAL_STRIPBIT8:

			Number = Config->SerialConfig->StripBit8;
			break;

		case ATTR_PREFS_SERIAL_CARRIERCHECK:

			Number = Config->SerialConfig->CheckCarrier;
			break;

		case ATTR_PREFS_SERIAL_PASSXONXOFFTHROUGH:

			Number = Config->SerialConfig->PassThrough;
			break;

		case ATTR_PREFS_SERIAL_OWNDEVUNIT_REQUEST:

			Number = Config->SerialConfig->SatisfyODURequests;
			break;

		case ATTR_PREFS_MODEM_DIAL_MODE:

			Number = Config->ModemConfig->DialMode;
			break;

		case ATTR_PREFS_MODEM_PBX_MODE:

			Number = Config->ModemConfig->PBX_Mode;
			break;

		case ATTR_PREFS_MODEM_DO_NOT_SEND_COMMANDS:

			Number = Config->ModemConfig->DoNotSendMainModemCommands;
			break;

		case ATTR_PREFS_MODEM_CHAR_SEND_DELAY:

			Number = Config->ModemConfig->CharSendDelay;
			break;

		case ATTR_PREFS_MODEM_MODEMINITTEXT:

			String = Config->ModemConfig->ModemInit;
			break;

		case ATTR_PREFS_MODEM_PBX_PREFIX:

			String = Config->ModemConfig->PBX_Prefix;
			break;

		case ATTR_PREFS_MODEM_MODEMEXITTEXT:

			String = Config->ModemConfig->ModemExit;
			break;

		case ATTR_PREFS_MODEM_MODEMHANGUPTEXT:

			String = Config->ModemConfig->ModemHangup;
			break;

		case ATTR_PREFS_MODEM_DIALPREFIXTEXT:

			String = Config->ModemConfig->DialPrefix;
			break;

		case ATTR_PREFS_MODEM_DIALSUFFIXTEXT:

			String = Config->ModemConfig->DialSuffix;
			break;

		case ATTR_PREFS_MODEM_NOCARRIERTEXT:

			String = Config->ModemConfig->NoCarrier;
			break;

		case ATTR_PREFS_MODEM_NODIALTONETEXT:

			String = Config->ModemConfig->NoDialTone;
			break;

		case ATTR_PREFS_MODEM_CONNECTTEXT:

			String = Config->ModemConfig->Connect;
			break;

		case ATTR_PREFS_MODEM_VOICETEXT:

			String = Config->ModemConfig->Voice;
			break;

		case ATTR_PREFS_MODEM_RINGTEXT:

			String = Config->ModemConfig->Ring;
			break;

		case ATTR_PREFS_MODEM_BUSYTEXT:

			String = Config->ModemConfig->Busy;
			break;

		case ATTR_PREFS_MODEM_OKTEXT:

			String = Config->ModemConfig->Ok;
			break;

		case ATTR_PREFS_MODEM_ERRORTEXT:

			String = Config->ModemConfig->Error;
			break;

		case ATTR_PREFS_MODEM_REDIALDELAY:

			Number = Config->ModemConfig->RedialDelay;
			break;

		case ATTR_PREFS_MODEM_DIALRETRIES:

			Number = Config->ModemConfig->DialRetries;
			break;

		case ATTR_PREFS_MODEM_DIALTIMEOUT:

			Number = Config->ModemConfig->DialTimeout;
			break;

		case ATTR_PREFS_MODEM_INTER_DIAL_DELAY:

			Number = Config->ModemConfig->InterDialDelay;
			break;

		case ATTR_PREFS_MODEM_CONNECTAUTOBAUD:

			Number = Config->ModemConfig->ConnectAutoBaud;
			break;

		case ATTR_PREFS_MODEM_HANGUPDROPSDTR:

			Number = Config->ModemConfig->DropDTR;
			break;

		case ATTR_PREFS_MODEM_REDIALAFTERHANGUP:

			Number = Config->ModemConfig->RedialAfterHangup;
			break;

		case ATTR_PREFS_MODEM_VERBOSEDIALING:

			Number = Config->ModemConfig->VerboseDialing;
			break;

		case ATTR_PREFS_MODEM_NOCARRIERISBUSY:

			Number = Config->ModemConfig->NoCarrierIsBusy;
			break;

		case ATTR_PREFS_MODEM_CONNECTLIMIT:

			Number = Config->ModemConfig->ConnectLimit;
			break;

		case ATTR_PREFS_MODEM_CONNECTLIMITMACRO:

			String = Config->ModemConfig->ConnectLimitMacro;
			break;

		case ATTR_PREFS_SERIAL_RELEASE_WHEN_ONLINE:

			Number = Config->SerialConfig->ReleaseODUWhenOnline;
			break;

		case ATTR_PREFS_SERIAL_RELEASE_WHEN_DIALING:

			Number = Config->SerialConfig->ReleaseODUWhenDialing;
			break;

		case ATTR_PREFS_SERIAL_NO_ODU_IF_SHARED:

			Number = Config->SerialConfig->NoODUIfShared;
			break;

		case ATTR_PREFS_SERIAL_LOCAL_ECHO:

			Number = Config->SerialConfig->Duplex;
			break;

		case ATTR_PREFS_MODEM_TIME_TO_CONNECT:

			Number = Config->ModemConfig->TimeToConnect;
			break;

		case ATTR_PREFS_COMMANDS_STARTUPMACROTEXT:

			String = Config->CommandConfig->StartupMacro;
			break;

		case ATTR_PREFS_COMMANDS_LOGINMACROTEXT:

			String = Config->CommandConfig->LoginMacro;
			break;

		case ATTR_PREFS_COMMANDS_LOGOFFMACROTEXT:

			String = Config->CommandConfig->LogoffMacro;
			break;

		case ATTR_PREFS_COMMANDS_UPLOADMACROTEXT:

			String = Config->CommandConfig->UploadMacro;
			break;

		case ATTR_PREFS_COMMANDS_DOWNLOADMACROTEXT:

			String = Config->CommandConfig->DownloadMacro;
			break;

		case ATTR_PREFS_SCREEN_COLOURMODE:

			Number = Config->ScreenConfig->ColourMode;
			break;

		case ATTR_PREFS_SCREEN_USEPENS:

			Number = Config->ScreenConfig->UsePens;
			break;

		case ATTR_PREFS_SCREEN_WINDOW_BORDER:

			Number = Config->ScreenConfig->ShareScreen;
			break;

		case ATTR_PREFS_SCREEN_SPLIT_STATUS:

			Number = Config->ScreenConfig->SplitStatus;
			break;

		case ATTR_PREFS_SCREEN_FONTNAME:

			String = Config->ScreenConfig->FontName;
			break;

		case ATTR_PREFS_SCREEN_FONTSIZE:

			Number = Config->ScreenConfig->FontHeight;
			break;

		case ATTR_PREFS_SCREEN_MAKESCREENPUBLIC:

			Number = Config->ScreenConfig->MakeScreenPublic;
			break;

		case ATTR_PREFS_SCREEN_SHANGHAIWINDOWS:

			break;

		case ATTR_PREFS_SCREEN_BLINKING:

			Number = Config->ScreenConfig->Blinking;
			break;

		case ATTR_PREFS_SCREEN_FASTERLAYOUT:

			Number = Config->ScreenConfig->FasterLayout;
			break;

		case ATTR_PREFS_SCREEN_TITLEBAR:

			Number = Config->ScreenConfig->TitleBar;
			break;

		case ATTR_PREFS_SCREEN_STATUSLINEMODE:

			Number = Config->ScreenConfig->StatusLine;
			break;

		case ATTR_PREFS_SCREEN_USEWORKBENCH:

			Number = Config->ScreenConfig->UseWorkbench;
			break;

		case ATTR_PREFS_SCREEN_PUBSCREENNAME:

			String = Config->ScreenConfig->PubScreenName;
			break;

		case ATTR_PREFS_SCREEN_ONLINEDISPLAY:

			Number = Config->ScreenConfig->TimeMode;
			break;

		case ATTR_PREFS_TERMINAL_BELLMODE:

			Number = Config->TerminalConfig->BellMode;
			break;

		case ATTR_PREFS_TERMINAL_USETERMINALPROCESS:

			Number = Config->TerminalConfig->UseTerminalTask;
			break;

		case ATTR_PREFS_TERMINAL_AUTOSIZE:

			Number = Config->TerminalConfig->AutoSize;
			break;

		case ATTR_PREFS_TERMINAL_EMULATIONMODE:

			Number = Config->TerminalConfig->EmulationMode;
			break;

		case ATTR_PREFS_TERMINAL_FONTMODE:

			Number = Config->TerminalConfig->FontMode;
			break;

		case ATTR_PREFS_TERMINAL_SENDCRMODE:

			Number = Config->TerminalConfig->SendCR;
			break;

		case ATTR_PREFS_TERMINAL_SENDLFMODE:

			Number = Config->TerminalConfig->SendLF;
			break;

		case ATTR_PREFS_TERMINAL_RECEIVECRMODE:

			Number = Config->TerminalConfig->ReceiveCR;
			break;

		case ATTR_PREFS_TERMINAL_RECEIVELFMODE:

			Number = Config->TerminalConfig->ReceiveLF;
			break;

		case ATTR_PREFS_TERMINAL_NUMCOLUMNS:

			Number = Config->TerminalConfig->NumColumns;
			break;

		case ATTR_PREFS_TERMINAL_NUMLINES:

			Number = Config->TerminalConfig->NumLines;
			break;

		case ATTR_PREFS_TERMINAL_KEYMAPNAME:

			String = Config->TerminalConfig->KeyMapFileName;
			break;

		case ATTR_PREFS_TERMINAL_EMULATIONNAME:

			String = Config->TerminalConfig->EmulationFileName;
			break;

		case ATTR_PREFS_TERMINAL_FONTNAME:

			String = Config->TerminalConfig->TextFontName;
			break;

		case ATTR_PREFS_TERMINAL_FONTSIZE:

			Number = Config->TerminalConfig->TextFontHeight;
			break;

		case ATTR_PREFS_PATHS_ASCIIUPLOADPATH:

			String = Config->PathConfig->ASCIIUploadPath;
			break;

		case ATTR_PREFS_PATHS_ASCIIDOWNLOADPATH:

			String = Config->PathConfig->ASCIIDownloadPath;
			break;

		case ATTR_PREFS_PATHS_TEXTUPLOADPATH:

			String = Config->PathConfig->TextUploadPath;
			break;

		case ATTR_PREFS_PATHS_TEXTDOWNLOADPATH:

			String = Config->PathConfig->TextDownloadPath;
			break;

		case ATTR_PREFS_PATHS_BINARYUPLOADPATH:

			String = Config->PathConfig->BinaryUploadPath;
			break;

		case ATTR_PREFS_PATHS_BINARYDOWNLOADPATH:

			String = Config->PathConfig->BinaryDownloadPath;
			break;

		case ATTR_PREFS_PATHS_CONFIGPATH:

			String = Config->PathConfig->DefaultStorage;
			break;

		case ATTR_PREFS_PATHS_EDITORNAME:

			String = Config->PathConfig->Editor;
			break;

		case ATTR_PREFS_PATHS_HELPFILENAME:

			String = Config->PathConfig->HelpFile;
			break;

		case ATTR_PREFS_MISC_ALERTMODE:

			Number = Config->MiscConfig->AlertMode;
			break;

		case ATTR_PREFS_MISC_WAITSTRING:

			String = Config->MiscConfig->WaitString;
			break;

		case ATTR_PREFS_MISC_WAITDELAY:

			Number = Config->MiscConfig->WaitDelay;
			break;

		case ATTR_PREFS_MISC_REQUESTERMODE:

			Number = Config->MiscConfig->RequesterMode;
			break;

		case ATTR_PREFS_MISC_REQUESTERWIDTH:

			Number = Config->MiscConfig->RequesterWidth;
			break;

		case ATTR_PREFS_MISC_REQUESTERHEIGHT:

			Number = Config->MiscConfig->RequesterHeight;
			break;

		case ATTR_PREFS_MISC_IOBUFFERSIZE:

			Number = Config->MiscConfig->IOBufferSize;
			break;

		case ATTR_PREFS_MISC_PRIORITY:

			Number = Config->MiscConfig->Priority;
			break;

		case ATTR_PREFS_MISC_BACKUPCONFIG:

			Number = Config->MiscConfig->BackupConfig;
			break;

		case ATTR_PREFS_MISC_OVERWRITE_WARNING:

			Number = Config->MiscConfig->ProtectiveMode;
			break;

		case ATTR_PREFS_MISC_SUPPRESSOUTPUT:

			Number = Config->MiscConfig->SuppressOutput;
			break;

		case ATTR_PREFS_MISC_OPENFASTMACROPANEL:

			Number = Config->MiscConfig->OpenFastMacroPanel;
			break;

		case ATTR_PREFS_MISC_RELEASEDEVICE:

			Number = Config->MiscConfig->ReleaseDevice;
			break;

		case ATTR_PREFS_MISC_OVERRIDEPATH:
		case ATTR_PREFS_TRANSFER_OVERRIDEPATH:

			Number = Config->TransferConfig->OverridePath;
			break;

		case ATTR_PREFS_TRANSFER_EXPAND_BLANK_LINES:

			Number = Config->TransferConfig->ExpandBlankLines;
			break;

		case ATTR_PREFS_MISC_AUTOUPLOAD:

			Number = Config->MiscConfig->AutoUpload;
			break;

		case ATTR_PREFS_MISC_SETARCHIVEDBIT:
		case ATTR_PREFS_TRANSFER_SETARCHIVEDBIT:

			Number = Config->TransferConfig->SetArchivedBit;
			break;

		case ATTR_PREFS_TRANSFER_COMMENTMODE:
		case ATTR_PREFS_MISC_COMMENTMODE:

			Number = Config->TransferConfig->IdentifyFiles;
			break;

		case ATTR_PREFS_MISC_TRANSFERICONS:
		case ATTR_PREFS_TRANSFER_TRANSFERICONS:

			Number = Config->TransferConfig->TransferIcons;
			break;

		case ATTR_PREFS_MISC_CREATEICONS:

			Number = Config->MiscConfig->CreateIcons;
			break;

		case ATTR_PREFS_MISC_SIMPLEIO:

			Number = Config->MiscConfig->SimpleIO;
			break;

		case ATTR_PREFS_MISC_HIDE_UPLOAD_ICON:
		case ATTR_PREFS_TRANSFER_HIDE_UPLOAD_ICON:

			Number = Config->TransferConfig->HideUploadIcon;
			break;

		case ATTR_PREFS_MISC_TRANSFER_PERFMETER:
		case ATTR_PREFS_TRANSFER_TRANSFER_PERFMETER:

			Number = Config->TransferConfig->PerfMeter;
			break;

		case ATTR_PREFS_CLIPBOARD_UNIT:

			Number = Config->ClipConfig->ClipboardUnit;
			break;

		case ATTR_PREFS_CLIPBOARD_LINEDELAY:

			Number = Config->ClipConfig->LineDelay;
			break;

		case ATTR_PREFS_CLIPBOARD_CHARDELAY:

			Number = Config->ClipConfig->CharDelay;
			break;

		case ATTR_PREFS_CLIPBOARD_INSERTPREFIXTEXT:

			String = Config->ClipConfig->InsertPrefix;
			break;

		case ATTR_PREFS_CLIPBOARD_INSERTSUFFIXTEXT:

			String = Config->ClipConfig->InsertSuffix;
			break;

		case ATTR_PREFS_CLIPBOARD_LINEPROMPT:

			String = Config->ClipConfig->LinePrompt;
			break;

		case ATTR_PREFS_CLIPBOARD_TEXTPACING:

			Number = Config->ClipConfig->PacingMode;
			break;

		case ATTR_PREFS_CLIPBOARD_SENDTIMEOUT:

			Number = Config->ClipConfig->SendTimeout;
			break;

		case ATTR_PREFS_CLIPBOARD_CONVERTLF:

			Number = Config->ClipConfig->ConvertLF;
			break;

		case ATTR_PREFS_CAPTURE_CONVERTCHARACTERS:

			Number = Config->CaptureConfig->ConvertChars;
			break;

		case ATTR_PREFS_CAPTURE_BUFFERMODE:

			Number = Config->CaptureConfig->BufferMode;
			break;

		case ATTR_PREFS_CAPTURE_BUFFERSAFETYMEMORY:

			Number = Config->CaptureConfig->BufferSafetyMemory;
			break;

		case ATTR_PREFS_CAPTURE_LOGACTIONS:

			Number = Config->CaptureConfig->LogActions;
			break;

		case ATTR_PREFS_CAPTURE_LOGCALLS:

			Number = Config->CaptureConfig->LogCall;
			break;

		case ATTR_PREFS_CAPTURE_LOGFILENAME:

			String = Config->CaptureConfig->LogFileName;
			break;

		case ATTR_PREFS_CAPTURE_MAXBUFFERSIZE:

			Number = Config->CaptureConfig->MaxBufferSize;
			break;

		case ATTR_PREFS_CAPTURE_BUFFER:

			Number = Config->CaptureConfig->BufferEnabled;
			break;

		case ATTR_PREFS_CAPTURE_CONNECTAUTOCAPTURE:

			Number = Config->CaptureConfig->ConnectAutoCapture;
			break;

		case ATTR_PREFS_CAPTURE_AUTOCAPTUREDATE:

			Number = Config->CaptureConfig->AutoCaptureDate;
			break;

		case ATTR_PREFS_CAPTURE_CAPTUREFILTER:

			Number = Config->CaptureConfig->CaptureFilterMode;
			break;

		case ATTR_PREFS_CAPTURE_CAPTUREPATH:

			String = Config->CaptureConfig->CapturePath;
			break;

		case ATTR_PREFS_CAPTURE_CALLLOGFILENAME:

			String = Config->CaptureConfig->CallLogFileName;
			break;

		case ATTR_PREFS_CAPTURE_BUFFERSAVEPATH:

			String = Config->CaptureConfig->BufferPath;
			break;

		case ATTR_PREFS_CAPTURE_OPENBUFFERWINDOW:

			Number = Config->CaptureConfig->OpenBufferWindow;
			break;

		case ATTR_PREFS_CAPTURE_OPENBUFFERSCREEN:

			Number = Config->CaptureConfig->OpenBufferScreen;
			break;

		case ATTR_PREFS_CAPTURE_BUFFERSCREENPOSITION:

			Number = Config->CaptureConfig->BufferScreenPosition;
			break;

		case ATTR_PREFS_CAPTURE_BUFFERWIDTH:

			Number = Config->CaptureConfig->BufferWidth;
			break;

		case ATTR_PREFS_CAPTURE_SEARCH_HISTORY:

			Number = Config->CaptureConfig->SearchHistory;
			break;

		case ATTR_PREFS_CAPTURE_REMEMBER_BUFFERWINDOW:

			Number = Config->CaptureConfig->RememberBufferWindow;
			break;

		case ATTR_PREFS_CAPTURE_REMEMBER_BUFFERSCREEN:

			Number = Config->CaptureConfig->RememberBufferScreen;
			break;

		case ATTR_PREFS_FILE_TRANSFERPROTOCOLNAME:

			String = Config->TransferConfig->DefaultLibrary;
			break;

		case ATTR_PREFS_FILE_TRANSLATIONFILENAME:

			String = Config->TranslationFileName;
			break;

		case ATTR_PREFS_FILE_MACROFILENAME:

			String = Config->MacroFileName;
			break;

		case ATTR_PREFS_FILE_CURSORFILENAME:

			String = Config->CursorFileName;
			break;

		case ATTR_PREFS_FILE_FASTMACROFILENAME:

			String = Config->FastMacroFileName;
			break;

		case ATTR_PREFS_EMULATION_IDENTIFICATION:

			Number = Config->EmulationConfig->TerminalType;
			break;

		case ATTR_PREFS_EMULATION_STYLELOCKED:

			Number = Config->EmulationConfig->LockStyle;
			break;

		case ATTR_PREFS_EMULATION_USEPENS:

			Number = Config->EmulationConfig->UseStandardPens;
			break;

		case ATTR_PREFS_EMULATION_COLOURLOCKED:

			Number = Config->EmulationConfig->LockColour;
			break;

		case ATTR_PREFS_EMULATION_CURSORMODE:

			Number = Config->EmulationConfig->CursorMode;
			break;

		case ATTR_PREFS_EMULATION_CURSORLOCKED:

			Number = Config->EmulationConfig->CursorLocked;
			break;

		case ATTR_PREFS_EMULATION_FONTLOCKED:

			Number = Config->EmulationConfig->FontLocked;
			break;

		case ATTR_PREFS_EMULATION_NUMPADLOCKED:

			Number = Config->EmulationConfig->KeysLocked;
			break;

		case ATTR_PREFS_EMULATION_WRAPLOCKED:

			Number = Config->EmulationConfig->LockWrapping;
			break;

		case ATTR_PREFS_EMULATION_NUMERICMODE:

			Number = Config->EmulationConfig->NumericMode;
			break;

		case ATTR_PREFS_EMULATION_CURSORWRAP:

			Number = Config->EmulationConfig->CursorWrap;
			break;

		case ATTR_PREFS_EMULATION_LINEWRAP:

			Number = Config->EmulationConfig->LineWrap;
			break;

		case ATTR_PREFS_EMULATION_INSERTMODE:

			Number = Config->EmulationConfig->InsertMode;
			break;

		case ATTR_PREFS_EMULATION_NEWLINEMODE:

			Number = Config->EmulationConfig->NewLineMode;
			break;

		case ATTR_PREFS_EMULATION_SCROLLMODE:

			Number = Config->EmulationConfig->ScrollMode;
			break;

		case ATTR_PREFS_EMULATION_DESTRUCTIVEBACKSPACE:

			Number = Config->EmulationConfig->DestructiveBackspace;
			break;

		case ATTR_PREFS_EMULATION_SWAPBSDELETE:

			Number = Config->EmulationConfig->SwapBSDelete;
			break;

		case ATTR_PREFS_EMULATION_PRINTERENABLED:

			Number = Config->EmulationConfig->PrinterEnabled;
			break;

		case ATTR_PREFS_EMULATION_ANSWERBACKTEXT:

			String = Config->EmulationConfig->AnswerBack;
			break;

		case ATTR_PREFS_EMULATION_CLS_RESETS_CURSOR:

			Number = Config->EmulationConfig->CLSResetsCursor;
			break;

		case ATTR_PREFS_EMULATION_MAXSCROLL:

			Number = Config->EmulationConfig->MaxScroll;
			break;

		case ATTR_PREFS_EMULATION_MAXJUMP:

			Number = Config->EmulationConfig->MaxJump;
			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_LIBRARY:

			String = Config->TransferConfig->DefaultLibrary;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_UPLOAD_LIBRARY:

			String = Config->TransferConfig->ASCIIUploadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_LIBRARY:

			String = Config->TransferConfig->ASCIIDownloadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_LIBRARY:

			String = Config->TransferConfig->TextUploadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_LIBRARY:

			String = Config->TransferConfig->TextDownloadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_LIBRARY:

			String = Config->TransferConfig->BinaryUploadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_LIBRARY:

			String = Config->TransferConfig->BinaryDownloadLibrary;
			break;

		case ATTR_PREFS_TRANSFER_INTERNAL_ASCII_DOWNLOAD:

			Number = Config->TransferConfig->InternalASCIIDownload;
			break;

		case ATTR_PREFS_TRANSFER_INTERNAL_ASCII_UPLOAD:

			Number = Config->TransferConfig->InternalASCIIUpload;
			break;

		case ATTR_PREFS_TRANSFER_QUIET_TRANSFER:

			Number = Config->TransferConfig->QuietTransfer;
			break;

		case ATTR_PREFS_TRANSFER_MANGLE_FILE_NAMES:

			Number = Config->TransferConfig->MangleFileNames;
			break;

		case ATTR_PREFS_TRANSFER_STRIP_BIT_8:

			Number = Config->TransferConfig->StripBit8;
			break;

		case ATTR_PREFS_TRANSFER_IGNORE_DATA_PAST_ARNOLD:

			Number = Config->TransferConfig->IgnoreDataPastArnold;
			break;

		case ATTR_PREFS_TRANSFER_TERMINATOR_CHAR:

			Number = Config->TransferConfig->TerminatorChar;
			break;

		case ATTR_PREFS_TRANSFER_LINEDELAY:

			Number = Config->TransferConfig->LineDelay;
			break;

		case ATTR_PREFS_TRANSFER_CHARDELAY:

			Number = Config->TransferConfig->CharDelay;
			break;

		case ATTR_PREFS_TRANSFER_LINEPROMPT:

			String = Config->TransferConfig->LinePrompt;
			break;

		case ATTR_PREFS_TRANSFER_TEXTPACING:

			Number = Config->TransferConfig->PacingMode;
			break;

		case ATTR_PREFS_TRANSFER_SENDTIMEOUT:

			Number = Config->TransferConfig->SendTimeout;
			break;

		case ATTR_PREFS_TRANSFER_ERROR_NOTIFY_COUNT:

			Number = Config->TransferConfig->ErrorNotification;
			break;

		case ATTR_PREFS_TRANSFER_ERROR_NOTIFY_WHEN:

			Number = Config->TransferConfig->TransferNotification;
			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_TYPE:

			Number = Config->TransferConfig->DefaultType;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_UPLOAD_TYPE:

			Number = Config->TransferConfig->ASCIIUploadType;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_TYPE:

			Number = Config->TransferConfig->ASCIIDownloadType;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_TYPE:

			Number = Config->TransferConfig->TextUploadType;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_TYPE:

			Number = Config->TransferConfig->TextDownloadType;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_TYPE:

			Number = Config->TransferConfig->BinaryUploadType;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_TYPE:

			Number = Config->TransferConfig->BinaryDownloadType;
			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_SEND_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_RECEIVE_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTDOWNLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTDOWNLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_UPLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_ASCIIUPLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_ASCIIUPLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_ASCIIDOWNLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_ASCIIDOWNLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_TEXTUPLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_TEXTUPLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_TEXTDOWNLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_TEXTDOWNLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_BINARYUPLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_BINARYUPLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_SIGNATURE:

			TranslateBack(Config->TransferConfig->Signatures[TRANSFERSIG_BINARYDOWNLOAD].Signature,Config->TransferConfig->Signatures[TRANSFERSIG_BINARYDOWNLOAD].Length,ResultBuffer,sizeof(ResultBuffer));

			String = ResultBuffer;
			break;

		case ATTR_PREFS_TRANSFER_IDENTIFY_COMMAND:

			String = Config->TransferConfig->IdentifyCommand;
			break;

		case ATTR_PREFS_SPEECH_RATE:

			Number = SpeechConfig.Rate;
			break;

		case ATTR_PREFS_SPEECH_PITCH:

			Number = SpeechConfig.Pitch;
			break;

		case ATTR_PREFS_SPEECH_FREQUENCY:

			Number = SpeechConfig.Frequency;
			break;

		case ATTR_PREFS_SPEECH_SEXMODE:

			Number = SpeechConfig.Sex;
			break;

		case ATTR_PREFS_SPEECH_VOLUME:

			Number = (100 * SpeechConfig.Volume) / 64;
			break;

		case ATTR_PREFS_SPEECH_SPEECH:

			Number = SpeechConfig.Enabled;
			break;

		case ATTR_PREFS_TRANSLATIONS_X_SEND:

			if(Count1 < 0 || Count1 > 255)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
			{
				STRPTR Str;
				LONG Len;
				UBYTE c;

				c = Count1;
				Len = 1;
				Str = &c;

				if(SendTable && SendTable[Count1])
				{
					Str = SendTable[Count1]->String;
					Len = SendTable[Count1]->Len;
				}

				TranslateBack(Str,Len,ResultBuffer,sizeof(ResultBuffer));

				String = ResultBuffer;
			}

			break;

		case ATTR_PREFS_TRANSLATIONS_X_RECEIVE:

			if(Count1 < 0 || Count1 > 255)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
			{
				STRPTR Str;
				LONG Len;
				UBYTE c;

				c = Count1;
				Len = 1;
				Str = &c;

				if(ReceiveTable)
				{
					if(ReceiveTable[Count1])
					{
						Str = ReceiveTable[Count1]->String;
						Len = ReceiveTable[Count1]->Len;
					}
				}

				TranslateBack(Str,Len,ResultBuffer,sizeof(ResultBuffer));
			}

			String = ResultBuffer;

			break;

		case ATTR_PREFS_FASTMACROS_COUNT:

			Number = FastMacroCount;
			break;

		case ATTR_PREFS_FASTMACROS_X_NAME:

			if(Count1 < FastMacroCount && Count1 >= 0)
			{
				struct MacroNode *Node;

				Node = (struct MacroNode *)GetListNode(Count1,&FastMacroList);

				String = Node->Macro;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PREFS_FASTMACROS_X_CODE:

			if(Count1 < FastMacroCount && Count1 >= 0)
			{
				struct MacroNode *Node;

				Node = (struct MacroNode *)GetListNode(Count1,&FastMacroList);

				String = Node->Code;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PREFS_HOTKEYS_TERMSCREENTOFRONTTEXT:

			String = Hotkeys.termScreenToFront;
			break;

		case ATTR_PREFS_HOTKEYS_BUFFERSCREENTOFRONTTEXT:

			String = Hotkeys.BufferScreenToFront;
			break;

		case ATTR_PREFS_HOTKEYS_SKIPDIALENTRYTEXT:

			String = Hotkeys.SkipDialEntry;
			break;

		case ATTR_PREFS_HOTKEYS_ABORTAREXX:

			String = Hotkeys.AbortARexx;
			break;

		case ATTR_PREFS_HOTKEYS_COMMODITYPRIORITY:

			Number = Hotkeys.CommodityPriority;
			break;

		case ATTR_PREFS_HOTKEYS_HOTKEYSENABLED:

			Number = Hotkeys.HotkeysEnabled;
			break;

		case ATTR_PREFS_CURSORKEYS_UPTEXT:

			String = CursorKeys->Keys[0][0];
			break;

		case ATTR_PREFS_CURSORKEYS_RIGHTTEXT:

			String = CursorKeys->Keys[0][1];
			break;

		case ATTR_PREFS_CURSORKEYS_DOWNTEXT:

			String = CursorKeys->Keys[0][2];
			break;

		case ATTR_PREFS_CURSORKEYS_LEFTTEXT:

			String = CursorKeys->Keys[0][3];
			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_UPTEXT:

			String = CursorKeys->Keys[1][0];
			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_RIGHTTEXT:

			String = CursorKeys->Keys[1][1];
			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_DOWNTEXT:

			String = CursorKeys->Keys[1][2];
			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_LEFTTEXT:

			String = CursorKeys->Keys[1][3];
			break;

		case ATTR_PREFS_CURSORKEYS_ALT_UPTEXT:

			String = CursorKeys->Keys[2][0];
			break;

		case ATTR_PREFS_CURSORKEYS_ALT_RIGHTTEXT:

			String = CursorKeys->Keys[2][1];
			break;

		case ATTR_PREFS_CURSORKEYS_ALT_DOWNTEXT:

			String = CursorKeys->Keys[2][2];
			break;

		case ATTR_PREFS_CURSORKEYS_ALT_LEFTTEXT:

			String = CursorKeys->Keys[2][3];
			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_UPTEXT:

			String = CursorKeys->Keys[3][0];
			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_RIGHTTEXT:

			String = CursorKeys->Keys[3][1];
			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_DOWNTEXT:

			String = CursorKeys->Keys[3][2];
			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_LEFTTEXT:

			String = CursorKeys->Keys[3][3];
			break;

		case ATTR_PREFS_FUNCTIONKEYS_X:

			if(Count1 < 1 || Count1 > 10)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
				String = MacroKeys->Keys[0][Count1 - 1];

			break;

		case ATTR_PREFS_FUNCTIONKEYS_SHIFT_X:

			if(Count1 < 1 || Count1 > 10)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
				String = MacroKeys->Keys[1][Count1 - 1];

			break;

		case ATTR_PREFS_FUNCTIONKEYS_ALT_X:

			if(Count1 < 1 || Count1 > 10)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
				String = MacroKeys->Keys[2][Count1 - 1];

			break;

		case ATTR_PREFS_FUNCTIONKEYS_CONTROL_X:

			if(Count1 < 1 || Count1 > 10)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
				String = MacroKeys->Keys[3][Count1 - 1];

			break;

		case ATTR_PREFS_PROTOCOL:

			String = ProtocolOptsBuffer;
			break;

		case ATTR_PREFS_MISC_CONSOLEWINDOW:
		case ATTR_PREFS_CONSOLE:

			String = Config->MiscConfig->WindowName;
			break;

		case ATTR_PHONEBOOK_COUNT:

			Number = GlobalPhoneHandle->NumPhoneEntries;
			break;

		case ATTR_PHONEBOOK_X_NAME:

			if(Count1 < GlobalPhoneHandle->NumPhoneEntries && Count1 >= 0)
				String = GlobalPhoneHandle->Phonebook[Count1]->Header->Name;
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PHONEBOOK_X_NUMBER:

			if(Count1 < GlobalPhoneHandle->NumPhoneEntries && Count1 >= 0)
				String = GlobalPhoneHandle->Phonebook[Count1]->Header->Number;
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PHONEBOOK_X_COMMENTTEXT:

			if(Count1 < GlobalPhoneHandle->NumPhoneEntries && Count1 >= 0)
				String = GlobalPhoneHandle->Phonebook[Count1]->Header->Comment;
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PHONEBOOK_X_USERNAME:

			if(Count1 < GlobalPhoneHandle->NumPhoneEntries && Count1 >= 0)
				String = GlobalPhoneHandle->Phonebook[Count1]->Header->UserName;
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PHONEBOOK_X_PASSWORDTEXT:

			if(Count1 < GlobalPhoneHandle->NumPhoneEntries && Count1 >= 0)
				String = GlobalPhoneHandle->Phonebook[Count1]->Header->Password;
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			break;

		case ATTR_PREFS_SOUND_BELLNAME:

			String = SoundConfig.BellFile;
			break;

		case ATTR_PREFS_SOUND_ERROR:

			String = SoundConfig.ErrorNotifyFile;
			break;

		case ATTR_PREFS_SOUND_CONNECTNAME:

			String = SoundConfig.ConnectFile;
			break;

		case ATTR_PREFS_SOUND_DISCONNECTNAME:

			String = SoundConfig.DisconnectFile;
			break;

		case ATTR_PREFS_SOUND_GOODTRANSFERNAME:

			String = SoundConfig.GoodTransferFile;
			break;

		case ATTR_PREFS_SOUND_BADTRANSFERNAME:

			String = SoundConfig.BadTransferFile;
			break;

		case ATTR_PREFS_SOUND_RINGNAME:

			String = SoundConfig.RingFile;
			break;

		case ATTR_PREFS_SOUND_VOICENAME:

			String = SoundConfig.VoiceFile;
			break;

		case ATTR_PREFS_SOUND_PRELOAD:

			Number = SoundConfig.Preload;
			break;

		case ATTR_PREFS_SOUND_VOLUME:

			Number = SoundConfig.Volume;
			break;
	}

	if(!Error)
	{
		switch(AttributeTable[Index].Type)
		{
			case INFO_TEXT:

				*Result = String;

				break;

			case INFO_NUMERIC:

				LimitedSPrintf(sizeof(ResultBuffer),ResultBuffer,"%ld",Number);

				*Result = ResultBuffer;

				break;

			case INFO_BOOLEAN:

				*Result = ToBoolean(Number);

				break;

			case INFO_MAPPED:

				*Result = ToMap(Number,AttributeTable[Index].Mappings);

				break;
		}
	}

	return(Error);
}

	/* SetSingleAttribute(LONG ID,LONG Index,LONG Count1,STRPTR Input):
	 *
	 *	Beware the MONSTER!
	 */

STATIC LONG
SetSingleAttribute(LONG ID,LONG Index,LONG Count1,CONST_STRPTR Input)
{
	LONG Number,Error,i;
	CONST_STRPTR String;

	Error	= 0;
	String	= NULL;
	Number	= 0;

	switch(AttributeTable[Index].Type)
	{
		case INFO_TEXT:

			String = Input;
			break;

		case INFO_NUMERIC:

			Number = Atol(Input);
			break;

		case INFO_BOOLEAN:

			Number = FromBoolean(Input);
			break;

		case INFO_MAPPED:

			if((Number = FromMap(Input,AttributeTable[Index].Mappings)) == -1)
				return(ERROR_TOO_MANY_ARGS);
			else
				break;
	}

	UpdateRequired			= TRUE;
	TransferUpdateRequired	= FALSE;

	switch(ID)
	{
		case ATTR_PREFS_SERIAL_BAUDRATE:
		{
			ULONG Rate;

			Rate = 0;

			for(i = 0 ; i < NumBaudRates ; i++)
			{
				if(BaudRates[i] >= Number)
				{
					Rate = BaudRates[i];

					break;
				}
			}

			if(Rate && Rate != Config->SerialConfig->BaudRate)
			{
				Config->SerialConfig->BaudRate = Rate;

				ConfigChanged = TRUE;

				UpdateRequired = TRUE;
			}

			break;
		}

		case ATTR_PREFS_SERIAL_OWNDEVUNIT_REQUEST:

			Config->SerialConfig->SatisfyODURequests = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_DIRECT_CONNECTION:

			Config->SerialConfig->DirectConnection = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_BREAKLENGTH:

			Config->SerialConfig->BreakLength = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_BUFFERSIZE:

			for(i = 9 ; i <= 19 ; i++)
			{
				if(1L << i >= Number)
				{
					Config->SerialConfig->SerialBufferSize = 1L << i;

					Update_CR_LF_Translation();

					UpdateRequired = ConfigChanged = TRUE;

					break;
				}
			}

			break;

		case ATTR_PREFS_SERIAL_QUANTUM:

			for(i = 8 ; i <= 19 ; i++)
			{
				if(1L << i >= Number)
				{
					Config->SerialConfig->Quantum = 1L << i;

					UpdateRequired = ConfigChanged = TRUE;

					break;
				}
			}

			break;

		case ATTR_PREFS_SERIAL_DEVICENAME:

			strcpy(Config->SerialConfig->SerialDevice,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_UNIT:

			if(Number >= 0)
			{
				Config->SerialConfig->UnitNumber = Number;

				UpdateRequired = ConfigChanged = TRUE;
			}
			else
				Error = TERMERROR_UNIT_NOT_AVAILABLE;

			break;

		case ATTR_PREFS_SERIAL_BITSPERCHAR:

			if(Number < 7 || Number > 8)
			{
				Error = ERROR_BAD_NUMBER;

				UpdateRequired = ConfigChanged = TRUE;
			}
			else
				Config->SerialConfig->BitsPerChar = Number;

			break;

		case ATTR_PREFS_SERIAL_PARITYMODE:

			Config->SerialConfig->Parity = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_STOPBITS:

			if(Number < 0 || Number > 1)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->SerialConfig->StopBits = Number;

				UpdateRequired = ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_SERIAL_HANDSHAKINGMODE:

			Config->SerialConfig->HandshakingProtocol = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_DUPLEXMODE:

			Config->SerialConfig->Duplex = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_XONXOFF:

			Config->SerialConfig->xONxOFF = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_HIGHSPEED:

			Config->SerialConfig->HighSpeed = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_SHARED:

			Config->SerialConfig->Shared = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_STRIPBIT8:

			Config->SerialConfig->StripBit8 = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_CARRIERCHECK:

			Config->SerialConfig->CheckCarrier = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_PASSXONXOFFTHROUGH:

			Config->SerialConfig->PassThrough = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_USE_OWNDEVUNIT:

			Config->SerialConfig->UseOwnDevUnit = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_RELEASE_WHEN_ONLINE:

			Config->SerialConfig->ReleaseODUWhenOnline = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_RELEASE_WHEN_DIALING:

			Config->SerialConfig->ReleaseODUWhenDialing = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_NO_ODU_IF_SHARED:

			Config->SerialConfig->NoODUIfShared = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SERIAL_LOCAL_ECHO:

			Config->SerialConfig->Duplex = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_DIAL_MODE:

			Config->ModemConfig->DialMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_PBX_MODE:

			Config->ModemConfig->PBX_Mode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_PBX_PREFIX:

			strcpy(Config->ModemConfig->PBX_Prefix,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_DO_NOT_SEND_COMMANDS:

			Config->ModemConfig->DoNotSendMainModemCommands = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_CHAR_SEND_DELAY:

			Config->ModemConfig->CharSendDelay = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_MODEMINITTEXT:

			strcpy(Config->ModemConfig->ModemInit,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_MODEMEXITTEXT:

			strcpy(Config->ModemConfig->ModemExit,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_MODEMHANGUPTEXT:

			strcpy(Config->ModemConfig->ModemHangup,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_DIALPREFIXTEXT:

			strcpy(Config->ModemConfig->DialPrefix,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_DIALSUFFIXTEXT:

			strcpy(Config->ModemConfig->DialSuffix,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_NOCARRIERTEXT:

			strcpy(Config->ModemConfig->NoCarrier,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_NODIALTONETEXT:

			strcpy(Config->ModemConfig->NoDialTone,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_CONNECTTEXT:

			strcpy(Config->ModemConfig->Connect,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_VOICETEXT:

			strcpy(Config->ModemConfig->Voice,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_RINGTEXT:

			strcpy(Config->ModemConfig->Ring,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_BUSYTEXT:

			strcpy(Config->ModemConfig->Busy,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_OKTEXT:

			strcpy(Config->ModemConfig->Ok,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_ERRORTEXT:

			strcpy(Config->ModemConfig->Error,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_REDIALDELAY:

			if(Number >= 0)
			{
				Config->ModemConfig->RedialDelay = Number;

				ConfigChanged = TRUE;
			}
			else
				Error = ERROR_BAD_NUMBER;

			break;

		case ATTR_PREFS_MODEM_DIALRETRIES:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ModemConfig->DialRetries = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_MODEM_DIALTIMEOUT:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ModemConfig->DialTimeout = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_MODEM_INTER_DIAL_DELAY:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ModemConfig->InterDialDelay = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_MODEM_CONNECTAUTOBAUD:

			Config->ModemConfig->ConnectAutoBaud = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_HANGUPDROPSDTR:

			Config->ModemConfig->DropDTR = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_REDIALAFTERHANGUP:

			Config->ModemConfig->RedialAfterHangup = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_VERBOSEDIALING:

			Config->ModemConfig->VerboseDialing = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_NOCARRIERISBUSY:

			Config->ModemConfig->NoCarrierIsBusy = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_CONNECTLIMIT:

			if(Number > 1440)
				Number = 1440;

			Config->ModemConfig->ConnectLimit = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_CONNECTLIMITMACRO:

			strcpy(Config->ModemConfig->ConnectLimitMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MODEM_TIME_TO_CONNECT:

			if(Number >= 0)
			{
				Config->ModemConfig->TimeToConnect = Number;

				ConfigChanged = TRUE;
			}
			else
				Error = ERROR_BAD_NUMBER;

			break;

		case ATTR_PREFS_COMMANDS_STARTUPMACROTEXT:

			strcpy(Config->CommandConfig->StartupMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_COMMANDS_LOGOFFMACROTEXT:

			strcpy(Config->CommandConfig->LogoffMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_COMMANDS_UPLOADMACROTEXT:

			strcpy(Config->CommandConfig->UploadMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_COMMANDS_DOWNLOADMACROTEXT:

			strcpy(Config->CommandConfig->DownloadMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_COMMANDS_LOGINMACROTEXT:

			strcpy(Config->CommandConfig->LoginMacro,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_COLOURMODE:

			Config->ScreenConfig->ColourMode = Number;

			UpdateRequired = ConfigChanged = TRUE;

			Default2CurrentPalette(Config);

			break;

		case ATTR_PREFS_SCREEN_FONTNAME:

			strcpy(Config->ScreenConfig->FontName,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_FONTSIZE:

			if(Number < 1)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ScreenConfig->FontHeight = Number;

				UpdateRequired = ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_SCREEN_MAKESCREENPUBLIC:

			Config->ScreenConfig->MakeScreenPublic = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_SHANGHAIWINDOWS:

			break;

		case ATTR_PREFS_SCREEN_BLINKING:

			Config->ScreenConfig->Blinking = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_FASTERLAYOUT:

			Config->ScreenConfig->FasterLayout = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_TITLEBAR:

			Config->ScreenConfig->TitleBar = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_STATUSLINEMODE:

			Config->ScreenConfig->StatusLine = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_USEWORKBENCH:

			Config->ScreenConfig->UseWorkbench = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_PUBSCREENNAME:

			strcpy(Config->ScreenConfig->PubScreenName,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_ONLINEDISPLAY:

			Config->ScreenConfig->TimeMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_USEPENS:

			Config->ScreenConfig->UsePens = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_WINDOW_BORDER:

			Config->ScreenConfig->ShareScreen = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SCREEN_SPLIT_STATUS:

			Config->ScreenConfig->SplitStatus = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_BELLMODE:

			Config->TerminalConfig->BellMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_EMULATIONMODE:

			Config->TerminalConfig->EmulationMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_FONTMODE:

			Config->TerminalConfig->FontMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_SENDCRMODE:

			Config->TerminalConfig->SendCR = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_SENDLFMODE:

			Config->TerminalConfig->SendLF = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_RECEIVECRMODE:

			Config->TerminalConfig->ReceiveCR = Number;

			Update_CR_LF_Translation();

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_RECEIVELFMODE:

			Config->TerminalConfig->ReceiveLF = Number;

			Update_CR_LF_Translation();

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_NUMCOLUMNS:

			if(Number < 0)
				Number = 0;

			Config->TerminalConfig->NumColumns = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_NUMLINES:

			if(Number < 0)
				Number = 0;

			Config->TerminalConfig->NumLines = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_KEYMAPNAME:

			strcpy(Config->TerminalConfig->KeyMapFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_EMULATIONNAME:

			strcpy(Config->TerminalConfig->EmulationFileName,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_FONTNAME:

			strcpy(Config->TerminalConfig->TextFontName,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_USETERMINALPROCESS:

			Config->TerminalConfig->UseTerminalTask = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_AUTOSIZE:

			Config->TerminalConfig->AutoSize = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TERMINAL_FONTSIZE:

			if(Number < 1)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->TerminalConfig->TextFontHeight = Number;

				UpdateRequired = ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_PATHS_ASCIIUPLOADPATH:

			strcpy(Config->PathConfig->ASCIIUploadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_ASCIIDOWNLOADPATH:

			strcpy(Config->PathConfig->ASCIIDownloadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_TEXTUPLOADPATH:

			strcpy(Config->PathConfig->TextUploadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_TEXTDOWNLOADPATH:

			strcpy(Config->PathConfig->TextDownloadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_BINARYUPLOADPATH:

			strcpy(Config->PathConfig->BinaryUploadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_BINARYDOWNLOADPATH:

			strcpy(Config->PathConfig->BinaryDownloadPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_CONFIGPATH:

			strcpy(Config->PathConfig->DefaultStorage,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_EDITORNAME:

			strcpy(Config->PathConfig->Editor,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_PATHS_HELPFILENAME:

			strcpy(Config->PathConfig->HelpFile,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_WAITSTRING:

			strcpy(Config->MiscConfig->WaitString,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_WAITDELAY:

			if(Number < 1)
				Number = 1;
			else
			{
				if(Number > 60 * 60)
					Number = 60 * 60;
			}

			Config->MiscConfig->WaitDelay = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_ALERTMODE:

			Config->MiscConfig->AlertMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_REQUESTERMODE:

			Config->MiscConfig->RequesterMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_REQUESTERWIDTH:

			if(Number < 80)
				Number = 80;

			Config->MiscConfig->RequesterWidth = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_REQUESTERHEIGHT:

			if(Number < 80)
				Number = 80;

			Config->MiscConfig->RequesterHeight = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_PRIORITY:

			if(Number < -128)
				Number = -128;

			if(Number > 127)
				Number = 127;

			Config->MiscConfig->Priority = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_BACKUPCONFIG:

			Config->MiscConfig->BackupConfig = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_OVERWRITE_WARNING:

			Config->MiscConfig->ProtectiveMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_SUPPRESSOUTPUT:

			Config->MiscConfig->SuppressOutput = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_OPENFASTMACROPANEL:

			Config->MiscConfig->OpenFastMacroPanel = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_RELEASEDEVICE:

			Config->MiscConfig->ReleaseDevice = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_OVERRIDEPATH:
		case ATTR_PREFS_MISC_OVERRIDEPATH:

			Config->TransferConfig->OverridePath = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_AUTOUPLOAD:

			Config->MiscConfig->AutoUpload = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_EXPAND_BLANK_LINES:

			Config->TransferConfig->ExpandBlankLines = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_SETARCHIVEDBIT:
		case ATTR_PREFS_MISC_SETARCHIVEDBIT:

			Config->TransferConfig->SetArchivedBit = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_COMMENTMODE:
		case ATTR_PREFS_MISC_COMMENTMODE:

			Config->TransferConfig->IdentifyFiles = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_TRANSFERICONS:
		case ATTR_PREFS_TRANSFER_TRANSFERICONS:

			Config->TransferConfig->TransferIcons = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_CREATEICONS:

			Config->MiscConfig->CreateIcons = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_SIMPLEIO:

			Config->MiscConfig->SimpleIO = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_HIDE_UPLOAD_ICON:
		case ATTR_PREFS_TRANSFER_HIDE_UPLOAD_ICON:

			Config->TransferConfig->HideUploadIcon = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_TRANSFER_PERFMETER:
		case ATTR_PREFS_TRANSFER_TRANSFER_PERFMETER:

			Config->TransferConfig->PerfMeter = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_MISC_IOBUFFERSIZE:

			if(Number < 1)
				Number = 1;
			else
			{
				if(Number > (1L << 20))
					Number = (1L << 20);
			}

			for(i = 12 ; i <= 20 ; i++)
			{
				if(Number >= (1L << i) && Number <= (1L << (i + 1)))
				{
					Number = (1L << i);

					break;
				}
			}

			Config->MiscConfig->IOBufferSize = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_UNIT:

			if(Number < 0)
				Error = TERMERROR_UNIT_NOT_AVAILABLE;
			else
			{
				Config->ClipConfig->ClipboardUnit = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_CLIPBOARD_LINEDELAY:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ClipConfig->LineDelay = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_CLIPBOARD_CHARDELAY:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->ClipConfig->CharDelay = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_CLIPBOARD_INSERTPREFIXTEXT:

			strcpy(Config->ClipConfig->InsertPrefix,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_INSERTSUFFIXTEXT:

			strcpy(Config->ClipConfig->InsertSuffix,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_LINEPROMPT:

			strcpy(Config->ClipConfig->LinePrompt,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_TEXTPACING:

			Config->ClipConfig->PacingMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_SENDTIMEOUT:

			Config->ClipConfig->SendTimeout = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CLIPBOARD_CONVERTLF:

			Config->ClipConfig->ConvertLF = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_CONVERTCHARACTERS:

			Config->CaptureConfig->ConvertChars = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFERMODE:

			Config->CaptureConfig->BufferMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFERSAFETYMEMORY:

			Config->CaptureConfig->BufferSafetyMemory = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_LOGACTIONS:

			Config->CaptureConfig->LogActions = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_LOGCALLS:

			Config->CaptureConfig->LogCall = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_LOGFILENAME:

			strcpy(Config->CaptureConfig->LogFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_MAXBUFFERSIZE:

			if(Number < 0)
				Number = 0;

			Config->CaptureConfig->MaxBufferSize = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFER:

			Config->CaptureConfig->BufferEnabled = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_CONNECTAUTOCAPTURE:

			Config->CaptureConfig->ConnectAutoCapture = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_AUTOCAPTUREDATE:

			Config->CaptureConfig->AutoCaptureDate = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_CAPTUREFILTER:

			Config->CaptureConfig->CaptureFilterMode = Number;

			ConfigChanged = TRUE;

			ConOutputUpdate();

			break;

		case ATTR_PREFS_CAPTURE_CAPTUREPATH:

			strcpy(Config->CaptureConfig->CapturePath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_CALLLOGFILENAME:

			strcpy(Config->CaptureConfig->CallLogFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFERSAVEPATH:

			strcpy(Config->CaptureConfig->BufferPath,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_OPENBUFFERWINDOW:

			Config->CaptureConfig->OpenBufferWindow = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_OPENBUFFERSCREEN:

			Config->CaptureConfig->OpenBufferScreen = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFERSCREENPOSITION:

			Config->CaptureConfig->BufferScreenPosition = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_BUFFERWIDTH:

			if(Number < 80)
				Number = 80;

			if(Number > 255)
				Number = 255;

			Config->CaptureConfig->BufferWidth = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_SEARCH_HISTORY:

			if(Number < 1)
				Number = 1;

			if(Number > 255)
				Number = 255;

			Config->CaptureConfig->SearchHistory = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_REMEMBER_BUFFERWINDOW:

			Config->CaptureConfig->RememberBufferWindow = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_CAPTURE_REMEMBER_BUFFERSCREEN:

			Config->CaptureConfig->RememberBufferScreen = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_FILE_TRANSFERPROTOCOLNAME:

			strcpy(Config->TransferConfig->DefaultLibrary,String);

			ConfigChanged = TRUE;

			TransferUpdateRequired = TRUE;

			break;

		case ATTR_PREFS_FILE_TRANSLATIONFILENAME:

			strcpy(Config->TranslationFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_FILE_MACROFILENAME:

			strcpy(Config->MacroFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_FILE_CURSORFILENAME:

			strcpy(Config->CursorFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_FILE_FASTMACROFILENAME:

			strcpy(Config->FastMacroFileName,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_IDENTIFICATION:

			Config->EmulationConfig->TerminalType = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_CURSORMODE:

			Config->EmulationConfig->CursorMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_MAXSCROLL:

			if(Number < 0)
				Number = 0;
			else
			{
				if(Number > 255)
					Number = 255;
			}

			Config->EmulationConfig->MaxScroll = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_MAXJUMP:

			if(Number < 0)
				Number = 0;
			else
			{
				if(Number > 255)
					Number = 255;
			}

			Config->EmulationConfig->MaxJump = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_NUMERICMODE:

			Config->EmulationConfig->NumericMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_CURSORWRAP:

			Config->EmulationConfig->CursorWrap = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_LINEWRAP:

			Config->EmulationConfig->LineWrap = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_INSERTMODE:

			Config->EmulationConfig->InsertMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_NEWLINEMODE:

			Config->EmulationConfig->NewLineMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_SCROLLMODE:

			Config->EmulationConfig->ScrollMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_DESTRUCTIVEBACKSPACE:

			Config->EmulationConfig->DestructiveBackspace = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_SWAPBSDELETE:

			Config->EmulationConfig->SwapBSDelete = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_PRINTERENABLED:

			Config->EmulationConfig->PrinterEnabled = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_ANSWERBACKTEXT:

			strcpy(Config->EmulationConfig->AnswerBack,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_CLS_RESETS_CURSOR:

			Config->EmulationConfig->CLSResetsCursor = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_NUMPADLOCKED:

			Config->EmulationConfig->KeysLocked = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_WRAPLOCKED:

			Config->EmulationConfig->LockWrapping = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_FONTLOCKED:

			Config->EmulationConfig->FontLocked = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_CURSORLOCKED:

			Config->EmulationConfig->CursorLocked = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_STYLELOCKED:

			Config->EmulationConfig->LockStyle = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_COLOURLOCKED:

			Config->EmulationConfig->LockColour = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_EMULATION_USEPENS:

			Config->EmulationConfig->UseStandardPens = Number;

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_LIBRARY:

			strcpy(Config->TransferConfig->DefaultLibrary,String);

			ConfigChanged = TRUE;

			TransferUpdateRequired = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ASCII_UPLOAD_LIBRARY:

			strcpy(Config->TransferConfig->ASCIIUploadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_LIBRARY:

			strcpy(Config->TransferConfig->ASCIIDownloadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_LIBRARY:

			strcpy(Config->TransferConfig->TextUploadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_LIBRARY:

			strcpy(Config->TransferConfig->TextDownloadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_LIBRARY:

			strcpy(Config->TransferConfig->BinaryUploadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_LIBRARY:

			strcpy(Config->TransferConfig->BinaryDownloadLibrary,String);

			UpdateRequired = ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_QUIET_TRANSFER:

			Config->TransferConfig->QuietTransfer = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_MANGLE_FILE_NAMES:

			Config->TransferConfig->MangleFileNames = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_STRIP_BIT_8:

			Config->TransferConfig->StripBit8 = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_IGNORE_DATA_PAST_ARNOLD:

			Config->TransferConfig->IgnoreDataPastArnold = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TERMINATOR_CHAR:

			if(Number < 0)
				Number = 0;
			else
			{
				if(Number > 255)
					Number = 255;
			}

			Config->TransferConfig->TerminatorChar = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_LINEDELAY:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->TransferConfig->LineDelay = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_TRANSFER_CHARDELAY:

			if(Number < 0)
				Error = ERROR_BAD_NUMBER;
			else
			{
				Config->TransferConfig->CharDelay = Number;

				ConfigChanged = TRUE;
			}

			break;

		case ATTR_PREFS_TRANSFER_LINEPROMPT:

			strcpy(Config->TransferConfig->LinePrompt,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXTPACING:

			Config->TransferConfig->PacingMode = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_SENDTIMEOUT:

			Config->TransferConfig->SendTimeout = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ERROR_NOTIFY_COUNT:

			Config->TransferConfig->ErrorNotification = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ERROR_NOTIFY_WHEN:

			Config->TransferConfig->TransferNotification = Number;

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_TYPE:

			Config->TransferConfig->DefaultType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ASCII_UPLOAD_TYPE:

			Config->TransferConfig->ASCIIUploadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_TYPE:

			Config->TransferConfig->ASCIIDownloadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_TYPE:

			Config->TransferConfig->TextUploadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_TYPE:

			Config->TransferConfig->TextDownloadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_TYPE:

			Config->TransferConfig->BinaryUploadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_TYPE:

			Config->TransferConfig->BinaryDownloadType = Number;

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_SEND_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_DEFAULT_RECEIVE_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTDOWNLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTDOWNLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_UPLOAD_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_TEXTUPLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_TEXTUPLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_TEXTDOWNLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_TEXTDOWNLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_UPLOAD_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_BINARYUPLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_BINARYUPLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_SIGNATURE:

			Config->TransferConfig->Signatures[TRANSFERSIG_BINARYDOWNLOAD].Length = TranslateString(String,Config->TransferConfig->Signatures[TRANSFERSIG_BINARYDOWNLOAD].Signature);

			ConfigChanged	= TRUE;
			UpdateRequired	= TRUE;

			break;

		case ATTR_PREFS_TRANSFER_IDENTIFY_COMMAND:

			LimitedStrcpy(sizeof(Config->TransferConfig->IdentifyCommand),Config->TransferConfig->IdentifyCommand,String);

			ConfigChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_RATE:

			if(Number < MINRATE)
				Number = MINRATE;

			if(Number > MAXRATE)
				Number = MAXRATE;

			SpeechConfig.Rate = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_PITCH:

			if(Number < MINPITCH)
				Number = MINPITCH;

			if(Number > MAXPITCH)
				Number = MAXPITCH;

			SpeechConfig.Pitch = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_FREQUENCY:

			if(Number < MINFREQ)
				Number = MINFREQ;

			if(Number > MAXFREQ)
				Number = MAXFREQ;

			SpeechConfig.Frequency = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_SEXMODE:

			SpeechConfig.Sex = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_VOLUME:

			if(Number < 0)
				Number = 0;

			Number = (64 * Number) / 100;

			if(Number > 64)
				Number = 64;

			SpeechConfig.Volume = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_SPEECH_SPEECH:

			SpeechConfig.Enabled = Number;

			SpeechChanged = TRUE;

			break;

		case ATTR_PREFS_TRANSLATIONS_X_SEND:

			if(Count1 < 0 || Count1 > 255)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
			{
				if(SendTable && *String)
				{
					struct TranslationEntry *Entry = AllocTranslationEntry(String);

					if(Entry)
					{
						if(SendTable[Count1])
							FreeTranslationEntry(SendTable[Count1]);

						SendTable[Count1] = Entry;

						TranslationChanged = TRUE;
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}
				else
					Error = TERMERROR_NO_DATA_TO_PROCESS;
			}

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_TRANSLATIONS_X_RECEIVE:

			if(Count1 < 0 || Count1 > 255)
				Error = TERMERROR_INDEX_OUT_OF_RANGE;
			else
			{
				if(ReceiveTable && *String)
				{
					struct TranslationEntry *Entry;

					if(Entry = AllocTranslationEntry(String))
					{
						if(ReceiveTable[Count1])
							FreeTranslationEntry(ReceiveTable[Count1]);

						ReceiveTable[Count1] = Entry;

						TranslationChanged = TRUE;
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}
				else
					Error = TERMERROR_NO_DATA_TO_PROCESS;
			}

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FASTMACROS_X_NAME:

			if(Count1 < FastMacroCount && Count1 >= 0)
			{
				struct MacroNode *Node;

				Node = (struct MacroNode *)GetListNode(Count1,&FastMacroList);

				strcpy(Node->Macro,String);

				FastMacrosChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FASTMACROS_X_CODE:

			if(Count1 < FastMacroCount && Count1 >= 0)
			{
				struct MacroNode *Node;

				Node = (struct MacroNode *)GetListNode(Count1,&FastMacroList);

				strcpy(Node->Code,String);

				FastMacrosChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_HOTKEYS_TERMSCREENTOFRONTTEXT:

			strcpy(Hotkeys.termScreenToFront,String);

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_HOTKEYS_BUFFERSCREENTOFRONTTEXT:

			strcpy(Hotkeys.BufferScreenToFront,String);

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_HOTKEYS_SKIPDIALENTRYTEXT:

			strcpy(Hotkeys.SkipDialEntry,String);

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_HOTKEYS_ABORTAREXX:

			strcpy(Hotkeys.AbortARexx,String);

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_HOTKEYS_COMMODITYPRIORITY:

			if(Number < -128)
				Number = -128;

			if(Number > 127)
				Number = 127;

			Hotkeys.CommodityPriority = Number;

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_HOTKEYS_HOTKEYSENABLED:

			Hotkeys.HotkeysEnabled = Number;

			HotkeysChanged = TRUE;

			break;

		case ATTR_PREFS_CURSORKEYS_UPTEXT:

			strcpy(CursorKeys->Keys[0][0],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_RIGHTTEXT:

			strcpy(CursorKeys->Keys[0][1],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_DOWNTEXT:

			strcpy(CursorKeys->Keys[0][2],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_LEFTTEXT:

			strcpy(CursorKeys->Keys[0][3],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_UPTEXT:

			strcpy(CursorKeys->Keys[1][0],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_RIGHTTEXT:

			strcpy(CursorKeys->Keys[1][1],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_DOWNTEXT:

			strcpy(CursorKeys->Keys[1][2],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_SHIFT_LEFTTEXT:

			strcpy(CursorKeys->Keys[1][3],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_ALT_UPTEXT:

			strcpy(CursorKeys->Keys[2][0],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_ALT_RIGHTTEXT:

			strcpy(CursorKeys->Keys[2][1],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_ALT_DOWNTEXT:

			strcpy(CursorKeys->Keys[2][2],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_ALT_LEFTTEXT:

			strcpy(CursorKeys->Keys[2][3],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_UPTEXT:

			strcpy(CursorKeys->Keys[3][0],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_RIGHTTEXT:

			strcpy(CursorKeys->Keys[3][1],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_DOWNTEXT:

			strcpy(CursorKeys->Keys[3][2],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_CURSORKEYS_CONTROL_LEFTTEXT:

			strcpy(CursorKeys->Keys[3][3],String);

			CursorKeysChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FUNCTIONKEYS_X:

			if(Count1 >= 1 && Count1 <= 10)
			{
				strcpy(MacroKeys->Keys[0][Count1 - 1],String);

				MacroChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FUNCTIONKEYS_SHIFT_X:

			if(Count1 >= 1 && Count1 <= 10)
			{
				strcpy(MacroKeys->Keys[1][Count1 - 1],String);

				MacroChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FUNCTIONKEYS_ALT_X:

			if(Count1 >= 1 && Count1 <= 10)
			{
				strcpy(MacroKeys->Keys[2][Count1 - 1],String);

				MacroChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_FUNCTIONKEYS_CONTROL_X:

			if(Count1 >= 1 && Count1 <= 10)
			{
				strcpy(MacroKeys->Keys[3][Count1 - 1],String);

				MacroChanged = TRUE;
			}
			else
				Error = TERMERROR_INDEX_OUT_OF_RANGE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_PROTOCOL:

			strcpy(ProtocolOptsBuffer,String);

			TransferUpdateRequired = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_MISC_CONSOLEWINDOW:
		case ATTR_PREFS_CONSOLE:

			strcpy(Config->MiscConfig->WindowName,String);

			ConfigChanged = TRUE;

			UpdateRequired = FALSE;

			break;

		case ATTR_PREFS_SOUND_BELLNAME:

			strcpy(SoundConfig.BellFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_ERROR:

			strcpy(SoundConfig.ErrorNotifyFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_CONNECTNAME:

			strcpy(SoundConfig.ConnectFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_DISCONNECTNAME:

			strcpy(SoundConfig.DisconnectFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_GOODTRANSFERNAME:

			strcpy(SoundConfig.GoodTransferFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_BADTRANSFERNAME:

			strcpy(SoundConfig.BadTransferFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_RINGNAME:

			strcpy(SoundConfig.RingFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_VOICENAME:

			strcpy(SoundConfig.VoiceFile,String);

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_PRELOAD:

			SoundConfig.Preload = Number;

			SoundChanged = TRUE;

			break;

		case ATTR_PREFS_SOUND_VOLUME:

			if(Number < 0)
				Number = 0;
			else
			{
				if(Number > 100)
					Number = 100;
			}

			SoundConfig.Volume = Number;

			SoundChanged = TRUE;

			break;
	}

	if(Error)
		UpdateRequired = FALSE;

	return(Error);
}

	/* Cheapo shortcut :-) */

#define Args		Pkt->Array
#define ResultCode	Pkt->Results

	/* RexxGetAttr(struct RexxPkt *Pkt):
	 *
	 *	Get some global variable(s). Care to rewrite this beastie to
	 *	work recursively?
	 */

STRPTR
RexxGetAttr(struct RexxPkt *Pkt)
{
	enum	{	ARG_GETATTR_OBJECT,ARG_GETATTR_FIELD,ARG_GETATTR_STEM,
				ARG_GETATTR_VAR };

	WORD Index,Count1,Count2;
	UBYTE SrcName[160];

	if(Args[ARG_GETATTR_FIELD])
		LimitedSPrintf(sizeof(SrcName),SrcName,"%s.%s",Args[ARG_GETATTR_OBJECT],Args[ARG_GETATTR_FIELD]);
	else
		strcpy(SrcName,Args[ARG_GETATTR_OBJECT]);

	DB(kprintf("srcname=|%s|\n",SrcName));

	if((Index = ToIndex(SrcName,&Count1,&Count2)) == -1)
	{
		DB(kprintf("->unknown object\n"));

		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_OBJECT;
	}
	else
	{
		UBYTE DstName[160];

		DB(kprintf("item is index=%ld count1=%ld count2=%ld name |%s|\n",Index,Count1,Count2,AttributeTable[Index].Name ? AttributeTable[Index].Name : (STRPTR)"«NULL»"));

		if(AttributeTable[Index].Type == INFO_STEM)
		{
			DB(kprintf("this is a stem variable\n"));

			if(!Args[ARG_GETATTR_STEM])
			{
				DB(kprintf("->no stem var provided\n"));

				ResultCode[0] = RC_ERROR;
				ResultCode[1] = TERMERROR_RESULT_VARIABLE_REQUIRED;
			}
			else
			{
				LONG Level,Max,Min;
				CONST_STRPTR Result;

				Max = 0;
				Min = 0;

				strcpy(DstName,Args[ARG_GETATTR_STEM]);

				DB(kprintf("dstname=|%s|\n",DstName));

				Level = AttributeTable[Index].Level;

				DB(kprintf("list level %ld\n",Level));

				do
				{
					if(AttributeTable[Index].Name)
					{
						if(AttributeTable[Index].Type != INFO_STEM)
						{
							Forbid();

							DB(kprintf("reading item |%s|\n",AttributeTable[Index].Name));

							if(ResultCode[1] = GetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,&Result))
							{
								DB(kprintf("->error reading it\n"));

								ResultCode[0] = RC_ERROR;

								Permit();

								break;
							}
							else
							{
								if(!AttributeTable[Index + 1].Name)
									Max = Atol(Result);

								DB(kprintf("storing in |%s.%s|\n",DstName,FromIndex(Index,Count1,0,FALSE)));

								if(!CreateVarArgs(Result,Pkt,"%s.%s",DstName,FromIndex(Index,Count1,0,FALSE)))
								{
									DB(kprintf("->could not create var\n"));

									Permit();

									break;
								}
							}

							Permit();
						}
						else
						{
							if(!AttributeTable[Index + 1].Name)
							{
									/* Special cases! */

								switch(AttributeTable[Index].NodeID)
								{
									case ATTR_PREFS_TRANSLATIONS:

										Max = 256;
										break;

									case ATTR_PREFS_FUNCTIONKEYS:
									case ATTR_PREFS_FUNCTIONKEYS_SHIFT:
									case ATTR_PREFS_FUNCTIONKEYS_ALT:
									case ATTR_PREFS_FUNCTIONKEYS_CONTROL:

										Max = 11;
										Min = 1;
										break;

									default:

										Forbid();

										if(ResultCode[1] = GetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,&Result))
										{
											Permit();

											ResultCode[0] = RC_ERROR;

											break;
										}
										else
											Max = Atol(Result);

										Permit();

										break;
								}
							}
						}
					}
					else
					{
						LONG i,j;

						DB(kprintf("reading items %ld through %ld\n",Min,Max - 1));

						for(i = Min ; i < Max ; i++)
						{
							j = 0;

							do
							{
								if(AttributeTable[Index + j].Type != INFO_STEM)
								{
									Forbid();

									DB(kprintf("reading item %ld\n",AttributeTable[Index + j].NodeID));

									if(ResultCode[1] = GetSingleAttribute(AttributeTable[Index + j].NodeID,Index + j,i,&Result))
									{
										DB(kprintf("->error reading it\n"));

										Permit();

										ResultCode[0] = RC_ERROR;

										break;
									}
									else
									{
										DB(kprintf("storing it in |%s.%s|\n",DstName,FromIndex(Index + j,i,0,FALSE)));

										if(!CreateVarArgs(Result,Pkt,"%s.%s",DstName,FromIndex(Index + j,i,0,FALSE)))
										{
											Permit();

											break;
										}
									}

									Permit();
								}

								j++;
							}
							while(AttributeTable[Index + j].Level > Level && !ResultCode[0]);
						}

						while(AttributeTable[Index + 1].Level > Level)
							Index++;

						Min = 0;

						if(ResultCode[0])
							break;
					}

					Index++;
				}
				while(Index < AttributeTableSize && AttributeTable[Index].Level > Level);
			}
		}
		else
		{
			CONST_STRPTR Result;

			if(Args[ARG_GETATTR_STEM])
				LimitedSPrintf(sizeof(DstName),DstName,"%s.%s",Args[ARG_GETATTR_STEM],SrcName);
			else
			{
				if(Args[ARG_GETATTR_VAR])
					strcpy(DstName,Args[ARG_GETATTR_VAR]);
				else
					DstName[0] = 0;
			}

			DB(kprintf("dstname=|%s|\n",DstName));

			Forbid();

			if(ResultCode[1] = GetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,&Result))
			{
				ResultCode[0] = RC_ERROR;
				DB(kprintf("->could not read attr\n"));
			}
			else
			{
				if(DstName[0])
				{
					DB(kprintf("storing in variable |%s|\n",DstName));
					CreateVar(Result,Pkt,DstName);
				}
				else
				{
					Permit();

					DB(kprintf("returning in |RESULT|\n"));

					return(CreateResult(Result,ResultCode));
				}
			}

			Permit();
		}
	}

	return(NULL);
}

	/* RexxSetAttr(struct RexxPkt *Pkt):
	 *
	 *	Set some global variable(s). Care to rewrite this beastie to
	 *	work recursively?
	 */

STRPTR
RexxSetAttr(struct RexxPkt *Pkt)
{
	enum	{	ARG_SETATTR_OBJECT,ARG_SETATTR_FIELD,ARG_SETATTR_STEM,
				ARG_SETATTR_VAR };

	WORD Index,Count1,Count2;
	UBYTE SrcName[160];

	if(Args[ARG_SETATTR_FIELD])
		LimitedSPrintf(sizeof(SrcName),SrcName,"%s.%s",Args[ARG_SETATTR_OBJECT],Args[ARG_SETATTR_FIELD]);
	else
		strcpy(SrcName,Args[ARG_SETATTR_OBJECT]);

	if((Index = ToIndex(SrcName,&Count1,&Count2)) == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_OBJECT;
	}
	else
	{
		if(AttributeTable[Index].ReadOnly)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = TERMERROR_INFORMATION_IS_READ_ONLY;
		}
		else
		{
			UBYTE DstName[160];

			if(AttributeTable[Index].Type == INFO_STEM)
			{
				if(!Args[ARG_SETATTR_STEM])
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = TERMERROR_RESULT_VARIABLE_REQUIRED;
				}
				else
				{
					UBYTE FullName[160];
					LONG Level,Max,Min;
					CONST_STRPTR Result;

					Max = 0;
					Min = 0;

					strcpy(DstName,Args[ARG_SETATTR_STEM]);

					Level = AttributeTable[Index].Level;

					do
					{
						if(AttributeTable[Index].Name)
						{
							if(!AttributeTable[Index + 1].Name)
							{
								if(AttributeTable[Index].Type == INFO_STEM)
								{
										/* Special cases! */

									switch(AttributeTable[Index].NodeID)
									{
										case ATTR_PREFS_TRANSLATIONS:

											Max = 256;
											break;

										case ATTR_PREFS_FUNCTIONKEYS:
										case ATTR_PREFS_FUNCTIONKEYS_SHIFT:
										case ATTR_PREFS_FUNCTIONKEYS_ALT:
										case ATTR_PREFS_FUNCTIONKEYS_CONTROL:

											Max = 11;
											Min = 1;
											break;

										default:

											Max = 0;
											break;
									}
								}
								else
									Max = 0;

								if(!Max)
								{
									Forbid();

									if(ResultCode[1] = GetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,&Result))
									{
										Permit();

										ResultCode[0] = RC_ERROR;

										break;
									}
									else
										Max = Atol(Result);

									Permit();
								}
							}

							if(AttributeTable[Index].Type != INFO_STEM)
							{
								LimitedSPrintf(sizeof(FullName),FullName,"%s.%s",DstName,FromIndex(Index,Count1,0,FALSE));

								if(!GetRexxVar(Pkt->RexxMsg,FullName,(char **)&Result))
								{
									if(ResultCode[1] = SetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,Result))
									{
										ResultCode[0] = RC_ERROR;

										break;
									}
								}
								else
									ResultCode[0] = RC_ERROR;
							}
						}
						else
						{
							LONG i,j;

							for(i = Min ; i < Max ; i++)
							{
								j = 0;

								do
								{
									if(AttributeTable[Index + j].Type != INFO_STEM)
									{
										LimitedSPrintf(sizeof(FullName),FullName,"%s.%s",DstName,FromIndex(Index + j,i,0,FALSE));

										if(!GetRexxVar(Pkt->RexxMsg,FullName,(char **)&Result))
										{
											if(ResultCode[1] = SetSingleAttribute(AttributeTable[Index + j].NodeID,Index + j,i,Result))
											{
												ResultCode[0] = RC_ERROR;

												break;
											}
										}
										else
											ResultCode[0] = RC_ERROR;
									}

									j++;
								}
								while(AttributeTable[Index + j].Level > Level && !ResultCode[0]);
							}

							while(AttributeTable[Index + 1].Level > Level)
								Index++;

							Min = 0;

							if(ResultCode[0])
								break;
						}

						Index++;
					}
					while(Index < AttributeTableSize && AttributeTable[Index].Level > Level);
				}
			}
			else
			{
				CONST_STRPTR Result;

				if(Args[ARG_SETATTR_STEM])
				{
					LimitedSPrintf(sizeof(DstName),DstName,"%s.%s",Args[ARG_SETATTR_STEM],SrcName);

					if(!GetRexxVar(Pkt->RexxMsg,DstName,(char **)&Result))
					{
						if(ResultCode[1] = SetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,Result))
							ResultCode[0] = RC_ERROR;
					}
					else
						ResultCode[0] = RC_ERROR;
				}
				else
				{
					if(ResultCode[1] = SetSingleAttribute(AttributeTable[Index].NodeID,Index,Count1,Args[ARG_SETATTR_VAR]))
						ResultCode[0] = RC_ERROR;
				}
			}
		}
	}

	return(NULL);
}
