/*
**	Config.c
**
**	Configuration processing routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Reset routine function pointer. */

typedef VOID (* RESET)(APTR,CONST_STRPTR);

/*****************************************************************************/

	/* Local routines. */

STATIC BOOL ReadSystemPrefs(STRPTR Name, ULONG ID, APTR Data, LONG Size, LONG Count);
STATIC VOID ResetSerialConfig(struct SerialSettings *SerialConfig);
STATIC VOID ResetModem(struct ModemSettings *ModemConfig);
STATIC VOID ResetScreen(struct ScreenSettings *ScreenConfig);
STATIC VOID ResetTerminal(struct TerminalSettings *TerminalConfig);
STATIC VOID ResetEmulation(struct EmulationSettings *EmulationConfig);
STATIC VOID ResetClip(struct ClipSettings *ClipConfig);
STATIC VOID ResetCapture(struct CaptureSettings *CaptureConfig);
STATIC VOID ResetTranslationFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetMacroFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetCursorFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetFastMacroFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetSpeechFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetSoundFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetAreaCodeFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetPhonebookFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetHotkeyFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetTrapFile(STRPTR Here, STRPTR PathBuffer);
STATIC VOID ResetFile(struct FileSettings *FileConfig, STRPTR PathBuffer);
STATIC VOID ResetPath(struct PathSettings *PathConfig, STRPTR PathBuffer);
STATIC VOID ResetMisc(struct MiscSettings *MiscConfig);
STATIC VOID ResetCommand(struct CommandSettings *UnusedCommandConfig);
STATIC VOID ResetTransfer(struct TransferSettings *TransferConfig, STRPTR DefaultLib);
STATIC VOID ResetNewConfigEntry(APTR To, LONG Type);
STATIC BOOL WriteConfigChunk(struct IFFHandle *Handle, struct Configuration *LocalConfig, LONG Type, APTR TempBuffer, STRPTR Password);
STATIC LONG IsConfigChunk(ULONG ID);
STATIC BOOL WriteConfigChunks(struct IFFHandle *Handle, struct Configuration *LocalConfig, APTR TempBuffer, STRPTR Password);
STATIC BOOL ClosePhonebookFile(struct IFFHandle *Handle);
STATIC struct IFFHandle *CreatePhonebookFile(STRPTR Name, PhonebookHandle *PhoneHandle);
STATIC BOOL DumpPhonebookRates(struct IFFHandle *Handle, struct PhoneEntry *Entry);
STATIC BOOL DumpPhonebookGroups(struct IFFHandle *Handle, struct List *GroupList);
STATIC BOOL DumpPhonebookFile(struct IFFHandle *Handle, APTR TempBuffer, PhonebookHandle *PhoneHandle);
STATIC BOOL ReadConfigChunk(struct IFFHandle *Handle, struct Configuration *LocalConfig, LONG Type, LONG Len, STRPTR Password);

/*****************************************************************************/

STATIC BOOL					FontPrefsRead		= FALSE,
							FontPrefsReadFailed	= FALSE;
STATIC struct FontPrefs		FontPrefs[3];

/*****************************************************************************/

STATIC UWORD SizeTable[] =
{
	sizeof(struct SerialSettings),
	sizeof(struct ModemSettings),
	sizeof(struct CommandSettings),
	sizeof(struct ScreenSettings),
	sizeof(struct TerminalSettings),
	sizeof(struct PathSettings),
	sizeof(struct MiscSettings),
	sizeof(struct ClipSettings),
	sizeof(struct CaptureSettings),
	sizeof(struct FileSettings),
	sizeof(struct EmulationSettings),
	sizeof(struct TransferSettings),
	MAX_FILENAME_LENGTH,				/* Translation file name */
	MAX_FILENAME_LENGTH,				/* Macro file name */
	MAX_FILENAME_LENGTH,				/* Cursor file name */
	MAX_FILENAME_LENGTH,				/* Fast macro file name */
	MAX_FILENAME_LENGTH,				/* Speech file name */
	MAX_FILENAME_LENGTH,				/* Sound file name */
	MAX_FILENAME_LENGTH,				/* Area code file name */
	MAX_FILENAME_LENGTH,				/* Phonebook file name */
	MAX_FILENAME_LENGTH,				/* Hotkey file name */
	MAX_FILENAME_LENGTH					/* Trap file name */
};

STATIC RESET ResetTable[] =
{
	(RESET)ResetSerialConfig,
	(RESET)ResetModem,
	(RESET)ResetCommand,
	(RESET)ResetScreen,
	(RESET)ResetTerminal,
	(RESET)ResetPath,
	(RESET)ResetMisc,
	(RESET)ResetClip,
	(RESET)ResetCapture,
	(RESET)ResetFile,
	(RESET)ResetEmulation,
	(RESET)ResetTransfer,
	(RESET)ResetTranslationFile,
	(RESET)ResetMacroFile,
	(RESET)ResetCursorFile,
	(RESET)ResetFastMacroFile,
	(RESET)ResetSpeechFile,
	(RESET)ResetSoundFile,
	(RESET)ResetAreaCodeFile,
	(RESET)ResetPhonebookFile,
	(RESET)ResetHotkeyFile,
	(RESET)ResetTrapFile
};

STATIC ULONG TypeTable[] =
{
	ID_SERL,
	ID_MODM,
	ID_COMD,
	ID_SCRN,
	ID_TRML,
	ID_PATH,
	ID_MISC,
	ID_CLIP,
	ID_CPTR,
	ID_FILE,
	ID_EMLN,
	ID_XFER,
	ID_XLNM,
	ID_MFNM,
	ID_CRNM,
	ID_FMNM,
	ID_SPNM,
	ID_SONM,
	ID_ACNM,
	ID_PBNM,
	ID_HKNM,
	ID_TRNM
};

#define NUM_STOPS (sizeof(Stops) / (2 * sizeof(ULONG)))

STATIC LONG Stops[] =
{
	ID_TERM,ID_CAT,
	ID_TERM,ID_VERS,
	ID_TERM,ID_DIAL,
	ID_TERM,ID_DAT2,
	ID_TERM,ID_DATE,
	ID_TERM,ID_PHON,
	ID_TERM,ID_PSWD,

	ID_TERM,ID_SERL,
	ID_TERM,ID_MODM,
	ID_TERM,ID_COMD,
	ID_TERM,ID_SCRN,
	ID_TERM,ID_TRML,
	ID_TERM,ID_PATH,
	ID_TERM,ID_MISC,
	ID_TERM,ID_CLIP,
	ID_TERM,ID_CPTR,
	ID_TERM,ID_FILE,
	ID_TERM,ID_EMLN,
	ID_TERM,ID_XFER,
	ID_TERM,ID_WINF,
	ID_TERM,ID_GRUP,
	ID_TERM,ID_XLNM,
	ID_TERM,ID_MFNM,
	ID_TERM,ID_CRNM,
	ID_TERM,ID_FMNM,
	ID_TERM,ID_SPNM,
	ID_TERM,ID_SONM,
	ID_TERM,ID_ACNM,
	ID_TERM,ID_PBNM,
	ID_TERM,ID_HKNM,
	ID_TERM,ID_TRNM
};

/*****************************************************************************/

enum	{	CR_IGNORE,CR_ASCR,CR_ASCRLF };
enum	{	LF_IGNORE,LF_ASLF,LF_ASLFCR };

/*****************************************************************************/

VOID
StripGlobals(struct Configuration *LocalConfig)
{
	if(LocalConfig)
	{
		FreeVecPooled(LocalConfig->SpeechFileName);
		FreeVecPooled(LocalConfig->SoundFileName);
		FreeVecPooled(LocalConfig->AreaCodeFileName);
		FreeVecPooled(LocalConfig->PhonebookFileName);
		FreeVecPooled(LocalConfig->HotkeyFileName);
		FreeVecPooled(LocalConfig->TrapFileName);

		LocalConfig->SpeechFileName		= NULL;
		LocalConfig->SoundFileName		= NULL;
		LocalConfig->AreaCodeFileName	= NULL;
		LocalConfig->PhonebookFileName	= NULL;
		LocalConfig->HotkeyFileName		= NULL;
		LocalConfig->TrapFileName		= NULL;
	}
}

VOID
FinalFix(struct Configuration *LocalConfig,BOOL UnusedIsPhonebook,LONG Version,LONG Revision)
{
	if(LocalConfig)
	{
		if(Version < 4 || (Version == 4 && Revision < 3))
		{
			if(LocalConfig->TransferConfig && LocalConfig->MiscConfig)
			{
				LocalConfig->TransferConfig->OverridePath	= LocalConfig->MiscConfig->OverridePath;
				LocalConfig->TransferConfig->SetArchivedBit	= LocalConfig->MiscConfig->SetArchivedBit;
				LocalConfig->TransferConfig->IdentifyFiles	= LocalConfig->MiscConfig->IdentifyFiles;
				LocalConfig->TransferConfig->TransferIcons	= LocalConfig->MiscConfig->TransferIcons;
				LocalConfig->TransferConfig->PerfMeter		= LocalConfig->MiscConfig->PerfMeter;
				LocalConfig->TransferConfig->HideUploadIcon	= LocalConfig->MiscConfig->HideUploadIcon;
			}
		}

		if(Version < 4 || (Version == 4 && Revision < 4))
		{
			if(LocalConfig->TerminalConfig && LocalConfig->MiscConfig)
				LocalConfig->MiscConfig->AlertMode = LocalConfig->TerminalConfig->AlertMode;
		}
	}
}

VOID
FixOldConfig(struct Configuration *LocalConfig,UBYTE ConfigChunkType,BOOL IsPhonebook,LONG Version,LONG Revision)
{
	if((Version == 3 && Revision < 6) || Version < 3)
	{
		if(ConfigChunkType == PREF_CLIP)
			LocalConfig->ClipConfig->ConvertLF = TRUE;

		if(ConfigChunkType == PREF_SERIAL)
			LocalConfig->SerialConfig->UseOwnDevUnit = TRUE;

		if(ConfigChunkType == PREF_MISC)
			LocalConfig->MiscConfig->ProtectiveMode = TRUE;

		if(ConfigChunkType == PREF_SCREEN)
		{
			LocalConfig->ScreenConfig->UsePens			= TRUE;
			LocalConfig->ScreenConfig->Depth			= 0;
			LocalConfig->ScreenConfig->PenColourMode	= 42;	/* Avoid trouble */
		}

		if(ConfigChunkType == PREF_TRANSFER)
		{
			LONG i;

			LocalConfig->TransferConfig->MangleFileNames = FALSE;

			if(LocalConfig->TransferConfig->ASCIIUploadLibrary[0])
				LocalConfig->TransferConfig->ASCIIUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->ASCIIDownloadLibrary[0])
				LocalConfig->TransferConfig->ASCIIDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->TextUploadLibrary[0])
				LocalConfig->TransferConfig->TextUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->TextDownloadLibrary[0])
				LocalConfig->TransferConfig->TextDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryUploadLibrary[0])
				LocalConfig->TransferConfig->BinaryUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryDownloadLibrary[0])
				LocalConfig->TransferConfig->BinaryDownloadType = XFER_XPR;

			for(i = 0 ; i < strlen(LocalConfig->TransferConfig->DefaultLibrary) - 6 ; i++)
			{
				if(!Strnicmp(&LocalConfig->TransferConfig->DefaultLibrary[i],"zmodem",6))
				{
					strcpy(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature,"*\030B01");

					LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length = strlen(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature);

					break;
				}
			}
		}

		if(ConfigChunkType == PREF_MODEM)
			LocalConfig->ModemConfig->AbortHangsUp = FALSE;

		if(ConfigChunkType == PREF_COMMAND && IsPhonebook)
		{
			strcpy(LocalConfig->CommandConfig->LoginMacro,LocalConfig->CommandConfig->StartupMacro);

			memset(LocalConfig->CommandConfig->StartupMacro,0,sizeof(LocalConfig->CommandConfig->StartupMacro));
		}

		if(ConfigChunkType == PREF_CAPTURE)
			LocalConfig->CaptureConfig->SearchHistory = 10;

		if(ConfigChunkType == PREF_EMULATION)
		{
			if(LocalConfig->EmulationConfig->DestructiveBackspace < 0 || LocalConfig->EmulationConfig->DestructiveBackspace > 2)
				LocalConfig->EmulationConfig->DestructiveBackspace = 1;
		}
	}

	if((Version == 4 && Revision < 1) || Version < 4)
	{
		if(ConfigChunkType == PREF_CLIP)
			LocalConfig->ClipConfig->ConvertLF = TRUE;

		if(ConfigChunkType == PREF_SERIAL)
		{
			LocalConfig->SerialConfig->xONxOFF		= FALSE;
			LocalConfig->SerialConfig->PassThrough	= TRUE;
		}

		if(ConfigChunkType == PREF_MISC)
			LocalConfig->MiscConfig->ProtectiveMode = TRUE;

		if(ConfigChunkType == PREF_MODEM)
		{
			LocalConfig->ModemConfig->RedialDelay	= (LocalConfig->ModemConfig->RedialDelay / 6) * 60 + (LocalConfig->ModemConfig->RedialDelay % 6) * 10;
			LocalConfig->ModemConfig->TimeToConnect	= (LocalConfig->ModemConfig->TimeToConnect / 6) * 60 + (LocalConfig->ModemConfig->TimeToConnect % 6) * 10;
			LocalConfig->ModemConfig->VerboseDialing	= FALSE;
		}

		if(ConfigChunkType == PREF_TERMINAL)
		{
			switch(LocalConfig->TerminalConfig->SendCR)
			{
				case CR_IGNORE:

					LocalConfig->TerminalConfig->SendCR = EOL_IGNORE;
					break;

				case CR_ASCR:

					LocalConfig->TerminalConfig->SendCR = EOL_CR;
					break;

				case CR_ASCRLF:

					LocalConfig->TerminalConfig->SendCR = EOL_CRLF;
					break;
			}

			switch(LocalConfig->TerminalConfig->ReceiveCR)
			{
				case CR_IGNORE:

					LocalConfig->TerminalConfig->ReceiveCR = EOL_IGNORE;
					break;

				case CR_ASCR:

					LocalConfig->TerminalConfig->ReceiveCR = EOL_CR;
					break;

				case CR_ASCRLF:

					LocalConfig->TerminalConfig->ReceiveCR = EOL_CRLF;
					break;
			}

			switch(LocalConfig->TerminalConfig->SendLF)
			{
				case LF_IGNORE:

					LocalConfig->TerminalConfig->SendLF = EOL_IGNORE;
					break;

				case LF_ASLF:

					LocalConfig->TerminalConfig->SendLF = EOL_LF;
					break;

				case LF_ASLFCR:

					LocalConfig->TerminalConfig->SendLF = EOL_LFCR;
					break;
			}

			switch(LocalConfig->TerminalConfig->ReceiveLF)
			{
				case LF_IGNORE:

					LocalConfig->TerminalConfig->ReceiveLF = EOL_IGNORE;
					break;

				case LF_ASLF:

					LocalConfig->TerminalConfig->ReceiveLF = EOL_LF;
					break;

				case LF_ASLFCR:

					LocalConfig->TerminalConfig->ReceiveLF = EOL_LFCR;
					break;
			}
		}

		if(ConfigChunkType == PREF_TRANSFER)
		{
			LONG i;

			if(LocalConfig->TransferConfig->ASCIIUploadLibrary[0])
				LocalConfig->TransferConfig->ASCIIUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->ASCIIDownloadLibrary[0])
				LocalConfig->TransferConfig->ASCIIDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->TextUploadLibrary[0])
				LocalConfig->TransferConfig->TextUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->TextDownloadLibrary[0])
				LocalConfig->TransferConfig->TextDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryUploadLibrary[0])
				LocalConfig->TransferConfig->BinaryUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryDownloadLibrary[0])
				LocalConfig->TransferConfig->BinaryDownloadType = XFER_XPR;

			for(i = 0 ; i < strlen(LocalConfig->TransferConfig->DefaultLibrary) - 6 ; i++)
			{
				if(!Strnicmp(&LocalConfig->TransferConfig->DefaultLibrary[i],"zmodem",6))
				{
					strcpy(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature,"*\030B01");

					LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length = strlen(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature);

					break;
				}
			}

			switch(LocalConfig->TransferConfig->SendCR)
			{
				case CR_IGNORE:

					LocalConfig->TransferConfig->SendCR = EOL_IGNORE;
					break;

				case CR_ASCR:

					LocalConfig->TransferConfig->SendCR = EOL_CR;
					break;

				case CR_ASCRLF:

					LocalConfig->TransferConfig->SendCR = EOL_CRLF;
					break;
			}

			switch(LocalConfig->TransferConfig->ReceiveCR)
			{
				case CR_IGNORE:

					LocalConfig->TransferConfig->ReceiveCR = EOL_IGNORE;
					break;

				case CR_ASCR:

					LocalConfig->TransferConfig->ReceiveCR = EOL_CR;
					break;

				case CR_ASCRLF:

					LocalConfig->TransferConfig->ReceiveCR = EOL_CRLF;
					break;
			}

			switch(LocalConfig->TransferConfig->SendLF)
			{
				case LF_IGNORE:

					LocalConfig->TransferConfig->SendLF = EOL_IGNORE;
					break;

				case LF_ASLF:

					LocalConfig->TransferConfig->SendLF = EOL_LF;
					break;

				case LF_ASLFCR:

					LocalConfig->TransferConfig->SendLF = EOL_LFCR;
					break;
			}

			switch(LocalConfig->TransferConfig->ReceiveLF)
			{
				case LF_IGNORE:

					LocalConfig->TransferConfig->ReceiveLF = EOL_IGNORE;
					break;

				case LF_ASLF:

					LocalConfig->TransferConfig->ReceiveLF = EOL_LF;
					break;

				case LF_ASLFCR:

					LocalConfig->TransferConfig->ReceiveLF = EOL_LFCR;
					break;
			}
		}
	}

	if(Version == 4 && Revision < 3)
	{
		if(ConfigChunkType == PREF_CLIP)
			LocalConfig->ClipConfig->ConvertLF = TRUE;

		if(ConfigChunkType == PREF_SERIAL)
		{
			LocalConfig->SerialConfig->xONxOFF		= FALSE;
			LocalConfig->SerialConfig->PassThrough	= TRUE;
		}

		if(ConfigChunkType == PREF_MISC)
			LocalConfig->MiscConfig->ProtectiveMode = TRUE;

		if(ConfigChunkType == PREF_TRANSFER)
		{
			LONG i;

			for(i = 0 ; i < strlen(LocalConfig->TransferConfig->DefaultLibrary) - 6 ; i++)
			{
				if(!Strnicmp(&LocalConfig->TransferConfig->DefaultLibrary[i],"zmodem",6))
				{
					strcpy(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature,"*\030B01");

					LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length = strlen(LocalConfig->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature);

					break;
				}
			}

			if(LocalConfig->TransferConfig->ASCIIUploadLibrary[0])
				LocalConfig->TransferConfig->ASCIIUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->ASCIIDownloadLibrary[0])
				LocalConfig->TransferConfig->ASCIIDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->InternalASCIIUpload)
				LocalConfig->TransferConfig->ASCIIUploadType = XFER_INTERNAL;

			if(LocalConfig->TransferConfig->InternalASCIIDownload)
				LocalConfig->TransferConfig->ASCIIDownloadType = XFER_INTERNAL;

			if(LocalConfig->TransferConfig->TextUploadLibrary[0])
				LocalConfig->TransferConfig->TextUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->TextDownloadLibrary[0])
				LocalConfig->TransferConfig->TextDownloadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryUploadLibrary[0])
				LocalConfig->TransferConfig->BinaryUploadType = XFER_XPR;

			if(LocalConfig->TransferConfig->BinaryDownloadLibrary[0])
				LocalConfig->TransferConfig->BinaryDownloadType = XFER_XPR;
		}
	}
}

VOID
FixScreenPens(struct ScreenSettings *ScreenConfig)
{
	LONG i,Count;

	for(Count = 0, i = DETAILPEN ; i <= BARTRIMPEN ; i++)
	{
		if(ScreenConfig->PenArray[i])
			Count++;
	}

	if(!Count)
		ScreenConfig->UsePens = TRUE;
}

/*****************************************************************************/

	/* ReadSystemPrefs():
	 *
	 *	Reads the system preferences settings.
	 */

STATIC BOOL
ReadSystemPrefs(STRPTR Name,ULONG ID,APTR Data,LONG Size,LONG Count)
{
	struct IFFHandle *Handle;
	LONG Error;

		/* Allocate an IFF handle. */

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
			/* Stop at the `body' chunk. */

		if(!(Error = StopChunk(Handle,ID_PREF,ID)))
		{
				/* Look for it... */

			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
					/* Read the data. */

				if(ReadChunkBytes(Handle,Data,Size) == Size)
				{
					if(Count)
					{
						Count--;

						Data = (APTR)((IPTR)Data + Size);
					}
					else
						break;
				}
				else
				{
					Error = IoErr();

					break;
				}
			}
		}

			/* Close the handle. */

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

		/* Return sucess/failure. */

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}

/*****************************************************************************/

STATIC VOID
ResetSerialConfig(struct SerialSettings *SerialConfig)
{
	STATIC BOOL					SerialPrefsRead			= FALSE,
								SerialPrefsReadFailed	= FALSE;
	STATIC struct SerialPrefs	SerialPrefs;

		/* The program will only try to read the preferences
		 * settings once; if the first access failed, no
		 * other accesses will be made.
		 */

	if(!SerialPrefsRead && !SerialPrefsReadFailed)
	{
		if(!ReadSystemPrefs("ENV:sys/serial.prefs",ID_SERL,&SerialPrefs,sizeof(SerialPrefs),1))
			SerialPrefsReadFailed = TRUE;

		SerialPrefsRead = TRUE;
	}

		/* Did we succeed in reading the file? */

	if(SerialPrefsRead && !SerialPrefsReadFailed)
	{
			/* Fill in the common data. */

		SerialConfig->BaudRate			= SerialPrefs.sp_BaudRate;
		SerialConfig->SerialBufferSize	= SerialPrefs.sp_InputBuffer;
		SerialConfig->BitsPerChar		= SerialPrefs.sp_BitsPerChar;
		SerialConfig->StopBits			= SerialPrefs.sp_StopBits;

			/* Convert the handshaking mode. */

		switch(SerialPrefs.sp_InputHandshake)
		{
			case HSHAKE_NONE:

				SerialConfig->HandshakingProtocol = HANDSHAKING_NONE;

				break;

			case HSHAKE_RTS:

				SerialConfig->HandshakingProtocol = HANDSHAKING_RTSCTS_DSR;

				break;

			default:

				SerialConfig->HandshakingProtocol = HANDSHAKING_NONE;

				break;
		}

			/* Convert the parity settings. */

		if(SerialPrefs.sp_Parity <= PARITY_SPACE)
			SerialConfig->Parity = SerialPrefs.sp_Parity;
		else
			SerialConfig->Parity = PARITY_NONE;
	}
	else
	{
		SerialConfig->BaudRate				= 19200;
		SerialConfig->BitsPerChar			= 8;
		SerialConfig->Parity				= PARITY_NONE;
		SerialConfig->StopBits				= 1;
		SerialConfig->HandshakingProtocol	= HANDSHAKING_RTSCTS_DSR;
		SerialConfig->SerialBufferSize		= 32768;
	}

	strcpy(SerialConfig->SerialDevice,SERIALNAME);

	SerialConfig->Duplex				= DUPLEX_FULL;
	SerialConfig->BreakLength			= 250000;
	SerialConfig->UnitNumber			= 0;
	SerialConfig->PassThrough			= TRUE;

	SerialConfig->Quantum				= 256;

	SerialConfig->SatisfyODURequests	= ODU_RELEASE;
}

STATIC VOID
ResetModem(struct ModemSettings *ModemConfig)
{
	strcpy(ModemConfig->ModemInit,			"ATX3E1V1Q0\\r");
	strcpy(ModemConfig->ModemExit,			"");
	strcpy(ModemConfig->ModemHangup,		"~~~~+++~~~~ATH0\\r");
	strcpy(ModemConfig->DialPrefix,			"ATD\\w");
	strcpy(ModemConfig->DialSuffix,			"\\r");

	strcpy(ModemConfig->NoCarrier,			"NO CARRIER");
	strcpy(ModemConfig->NoDialTone,			"NO DIALTONE");
	strcpy(ModemConfig->Connect,			"CONNECT");
	strcpy(ModemConfig->Voice,				"VOICE");
	strcpy(ModemConfig->Ring,				"RING");
	strcpy(ModemConfig->Busy,				"BUSY");
	strcpy(ModemConfig->Ok,					"OK");
	strcpy(ModemConfig->Error,				"ERROR");

	ModemConfig->RedialDelay				= 20;
	ModemConfig->DialRetries				= 10;
	ModemConfig->DialTimeout				= 60;

	ModemConfig->NoCarrierIsBusy			= TRUE;

	ModemConfig->DialMode					= DIALMODE_PULSE;

	strcpy(ModemConfig->PBX_Prefix,			"0,,,");
}

STATIC VOID
ResetScreen(struct ScreenSettings *ScreenConfig)
{
	struct Screen *PubScreen;
	ULONG DisplayID;

	if(!FontPrefsRead && !FontPrefsReadFailed)
	{
		if(!ReadSystemPrefs("ENV:sys/font.prefs",ID_FONT,FontPrefs,sizeof(struct FontPrefs),3))
			FontPrefsReadFailed = TRUE;

		FontPrefsRead = TRUE;
	}

	DisplayID = INVALID_ID;

	strcpy(ScreenConfig->FontName,"topaz.font");
	ScreenConfig->FontHeight = 8;

	if(FontPrefsRead && !FontPrefsReadFailed)
	{
		LONG i;

		for(i = 0 ; i < 3 ; i++)
		{
			if(FontPrefs[i].fp_Type == FP_SCREENFONT)
			{
				strcpy(ScreenConfig->FontName,FontPrefs[i].fp_Name);
				ScreenConfig->FontHeight = FontPrefs[i].fp_TextAttr.ta_YSize;

				break;
			}
		}
	}

	if(DisplayID == INVALID_ID)
	{
		if(PubScreen = LockPubScreen(NULL))
		{
			DisplayID = GetVPModeID(&PubScreen->ViewPort);

			UnlockPubScreen(NULL,PubScreen);
		}
		else
			DisplayID = DEFAULT_MONITOR_ID | HIRESLACE_KEY;
	}

	ScreenConfig->DisplayMode		= DisplayID;
	ScreenConfig->ColourMode		= COLOUR_AMIGA;
	ScreenConfig->MakeScreenPublic	= TRUE;
	ScreenConfig->TitleBar			= TRUE;
	ScreenConfig->StatusLine		= STATUSLINE_STANDARD;
	ScreenConfig->Blinking			= TRUE;
	ScreenConfig->FasterLayout		= TRUE;

	ScreenConfig->TimeMode			= ONLINETIME_BOTH;
	ScreenConfig->UsePens			= TRUE;
	ScreenConfig->PenColourMode		= COLOUR_AMIGA;

	ScreenConfig->OverscanType		= OSCAN_TEXT;
}

STATIC VOID
ResetTerminal(struct TerminalSettings *TerminalConfig)
{
	if(!FontPrefsRead && !FontPrefsReadFailed)
	{
		if(!ReadSystemPrefs("ENV:sys/font.prefs",ID_FONT,FontPrefs,sizeof(struct FontPrefs),3))
			FontPrefsReadFailed = TRUE;

		FontPrefsRead = TRUE;
	}

	strcpy(TerminalConfig->TextFontName,"topaz.font");
	TerminalConfig->TextFontHeight = 8;

	strcpy(TerminalConfig->IBMFontName,"IBM.font");
	TerminalConfig->IBMFontHeight = 8;

	if(FontPrefsRead && !FontPrefsReadFailed)
	{
		LONG i;

		for(i = 0 ; i < 3 ; i++)
		{
			if(FontPrefs[i].fp_Type == FP_SYSFONT)
			{
				strcpy(TerminalConfig->TextFontName,FontPrefs[i].fp_Name);
				TerminalConfig->TextFontHeight = FontPrefs[i].fp_TextAttr.ta_YSize;

				if(TerminalConfig->TextFontHeight == 8 || TerminalConfig->TextFontHeight == 11)
					TerminalConfig->IBMFontHeight = TerminalConfig->TextFontHeight;

				break;
			}
		}
	}

	TerminalConfig->BellMode		= BELL_AUDIBLE;
	TerminalConfig->AlertMode		= ALERT_BEEP_SCREEN;
	TerminalConfig->EmulationMode	= EMULATION_ANSIVT100;
	TerminalConfig->FontMode		= FONT_STANDARD;

	TerminalConfig->SendCR			= EOL_CR;
	TerminalConfig->SendLF			= EOL_LF;
	TerminalConfig->ReceiveCR		= EOL_CR;
	TerminalConfig->ReceiveLF		= EOL_LF;
}

STATIC VOID
ResetEmulation(struct EmulationSettings *EmulationConfig)
{
	LONG i;

	EmulationConfig->CursorMode			= KEYMODE_STANDARD;
	EmulationConfig->NumericMode		= KEYMODE_STANDARD;

	EmulationConfig->LineWrap			= TRUE;

	EmulationConfig->FontScale			= SCALE_NORMAL;
	EmulationConfig->ScrollMode			= SCROLL_JUMP;
	EmulationConfig->PrinterEnabled		= TRUE;
	EmulationConfig->MaxJump			= 1;

	EmulationConfig->UseStandardPens	= TRUE;

	for(i = TEXTATTR_UNDERLINE ; i <= TEXTATTR_INVERSE ; i++)
		EmulationConfig->Attributes[i] = i;

	for(i = 0 ; i < 16 ; i++)
		EmulationConfig->Pens[i] = i;

	EmulationConfig->TerminalType = TERMINAL_VT200;
}

STATIC VOID
ResetClip(struct ClipSettings *ClipConfig)
{
	strcpy(ClipConfig->InsertSuffix,"\\r");

	ClipConfig->PacingMode		= PACE_DIRECT;
	ClipConfig->ConvertLF		= TRUE;
}

STATIC VOID
ResetCapture(struct CaptureSettings *CaptureConfig)
{
	CaptureConfig->BufferEnabled		= TRUE;

	CaptureConfig->CaptureFilterMode	= FILTER_BOTH;

	CaptureConfig->AutoCaptureDate		= AUTOCAPTURE_DATE_NAME;
	CaptureConfig->SearchHistory		= 10;

	CaptureConfig->OpenBufferWindow		= BUFFER_END;
	CaptureConfig->OpenBufferScreen		= BUFFER_TOP;
	CaptureConfig->BufferScreenPosition	= SCREEN_CENTRE;
	CaptureConfig->BufferWidth			= 80;

	CaptureConfig->BufferScreenMode		= INVALID_ID;

	CaptureConfig->BufferMode			= BUFFERMODE_FLOW;

	CaptureConfig->LogFileFormat		= LOGFILEFORMAT_CallInfo;

	CaptureConfig->BufferSafetyMemory	= 60 * 1024;
}

STATIC VOID
ResetTranslationFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastTranslation[0])
	{
		strcpy(LastTranslation,PathBuffer);

		AddPart(LastTranslation,"translation.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastTranslation);
}

STATIC VOID
ResetMacroFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastMacros[0])
	{
		strcpy(LastMacros,PathBuffer);

		AddPart(LastMacros,"functionkeys.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastMacros);
}

STATIC VOID
ResetCursorFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastCursorKeys[0])
	{
		strcpy(LastCursorKeys,PathBuffer);

		AddPart(LastCursorKeys,"cursorkeys.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastCursorKeys);
}

STATIC VOID
ResetFastMacroFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastFastMacros[0])
	{
		strcpy(LastFastMacros,PathBuffer);

		AddPart(LastFastMacros,"fastmacros.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastFastMacros);
}

STATIC VOID
ResetSpeechFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastSpeech[0])
	{
		strcpy(LastSpeech,PathBuffer);

		AddPart(LastSpeech,"speech.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastSpeech);
}

STATIC VOID
ResetSoundFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastSound[0])
	{
		strcpy(LastSound,PathBuffer);

		AddPart(LastSound,"sound.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastSound);
}

STATIC VOID
ResetAreaCodeFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastPattern[0])
	{
		strcpy(LastPattern,PathBuffer);

		AddPart(LastPattern,"rates.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastPattern);
}

STATIC VOID
ResetPhonebookFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastPhone[0])
	{
		strcpy(LastPhone,PathBuffer);

		AddPart(LastPhone,"phonebook.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastPhone);
}

STATIC VOID
ResetHotkeyFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastKeys[0])
	{
		strcpy(LastKeys,PathBuffer);

		AddPart(LastKeys,"hotkeys.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastKeys);
}

STATIC VOID
ResetTrapFile(STRPTR Here,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	if(!LastTraps[0])
	{
		strcpy(LastTraps,PathBuffer);

		AddPart(LastTraps,"trap.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(Here,LastTraps);
}

STATIC VOID
ResetFile(struct FileSettings *FileConfig,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	strcpy(FileConfig->ProtocolFileName,"xprzmodem.library");

	if(!LastTranslation[0])
	{
		strcpy(LastTranslation,PathBuffer);

		AddPart(LastTranslation,"translation.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(FileConfig->TranslationFileName,LastTranslation);

	if(!LastMacros[0])
	{
		strcpy(LastMacros,PathBuffer);

		AddPart(LastMacros,"functionkeys.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(FileConfig->MacroFileName,LastMacros);

	if(!LastFastMacros[0])
	{
		strcpy(LastFastMacros,PathBuffer);

		AddPart(LastFastMacros,"fastmacros.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(FileConfig->FastMacroFileName,LastFastMacros);

	if(!LastCursorKeys[0])
	{
		strcpy(LastCursorKeys,PathBuffer);

		AddPart(LastCursorKeys,"cursorkeys.prefs",MAX_FILENAME_LENGTH);
	}

	strcpy(FileConfig->CursorFileName,LastCursorKeys);
}

STATIC VOID
ResetPath(struct PathSettings *PathConfig,STRPTR PathBuffer)
{
	if(!PathBuffer)
		PathBuffer = "TERM:config";

	strcpy(PathConfig->DefaultStorage,PathBuffer);

	strcpy(PathConfig->HelpFile,"PROGDIR:term.guide");

	strcpy(PathConfig->DefaultStorage,PathBuffer);

	GetEnvDOS("EDITOR",PathConfig->Editor,sizeof(PathConfig->Editor));
}

STATIC VOID
ResetMisc(struct MiscSettings *MiscConfig)
{
	MiscConfig->Priority			= 1;

	MiscConfig->ReleaseDevice		= TRUE;

	MiscConfig->TransferServer		= TRUE;
	MiscConfig->EmulationServer		= TRUE;

	MiscConfig->OverridePath		= TRUE;
	MiscConfig->AutoUpload			= TRUE;
	MiscConfig->IdentifyFiles		= IDENTIFY_FILETYPE;

	MiscConfig->IOBufferSize		= 32768;

	MiscConfig->ProtectiveMode		= TRUE;

	MiscConfig->AlertMode			= ALERT_BEEP_SCREEN;
	MiscConfig->RequesterMode		= REQUESTERMODE_IGNORE;

	strcpy(MiscConfig->WaitString," \\b");

	MiscConfig->WaitDelay			= 1;	/* Just one second */

	if(!GetEnvDOS("TERMWINDOW",MiscConfig->WindowName,sizeof(MiscConfig->WindowName)))
		strcpy(MiscConfig->WindowName,"CON:0/11//100/term Output Window/CLOSE/SCREEN %s");
}

STATIC VOID
ResetCommand(struct CommandSettings *UnusedCommandConfig)
{
}

STATIC VOID
ResetTransfer(struct TransferSettings *TransferConfig,STRPTR DefaultLib)
{
	LONG i;

	if(!DefaultLib)
		strcpy(TransferConfig->DefaultLibrary,"xprzmodem.library");
	else
		strcpy(TransferConfig->DefaultLibrary,DefaultLib);

	for(i = 0 ; i < strlen(TransferConfig->DefaultLibrary) - 6 ; i++)
	{
		if(!Strnicmp(&TransferConfig->DefaultLibrary[i],"zmodem",6))
		{
			strcpy(TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature,"*\030B01");

			TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Length = strlen(TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD].Signature);

			break;
		}
	}

	strcpy(TransferConfig->ASCIIUploadLibrary,		"xprascii.library");
	strcpy(TransferConfig->ASCIIDownloadLibrary,	"xprascii.library");

	strcpy(TransferConfig->BinaryUploadLibrary,"run hydracom device %p speed %b line %c nocarrier rec %> send %m");
	strcpy(TransferConfig->BinaryDownloadLibrary,"run hydracom device %p speed %b line %c nocarrier rec %> get");

	TransferConfig->PacingMode				= PACE_DIRECT;
	TransferConfig->IgnoreDataPastArnold	= TRUE;
	TransferConfig->TerminatorChar			= 0x1A;
	TransferConfig->SendCR					= EOL_CR;
	TransferConfig->SendLF					= EOL_LF;
	TransferConfig->ReceiveCR				= EOL_CR;
	TransferConfig->ReceiveLF				= EOL_LF;

	TransferConfig->ErrorNotification		= 20;
	TransferConfig->TransferNotification	= XFERNOTIFY_ALWAYS;

	TransferConfig->DefaultType				= XFER_XPR;
	TransferConfig->ASCIIDownloadType		= XFER_XPR;
	TransferConfig->ASCIIUploadType			= XFER_XPR;
	TransferConfig->TextDownloadType		= XFER_DEFAULT;
	TransferConfig->TextUploadType			= XFER_DEFAULT;
	TransferConfig->BinaryDownloadType		= XFER_DEFAULT;
	TransferConfig->BinaryUploadType		= XFER_DEFAULT;

	TransferConfig->OverridePath			= TRUE;
	TransferConfig->IdentifyFiles			= IDENTIFY_FILETYPE;
}

/*****************************************************************************/

STATIC BOOL
WriteConfigChunk(struct IFFHandle *Handle,struct Configuration *LocalConfig,LONG Type,APTR TempBuffer,STRPTR Password)
{
	if(Type >= PREF_SERIAL && Type < PREF_RATES && Type != PREF_FILE)
	{
		APTR Data;
		LONG Error;

		if(Data = GetConfigEntry(LocalConfig,Type))
		{
			if(TempBuffer)
			{
				Encrypt(Data,SizeTable[Type - PREF_SERIAL],TempBuffer,Password,MAX_PASSWORD_LENGTH);

				Data = TempBuffer;
			}

			if(!(Error = PushChunk(Handle,0,TypeTable[Type - PREF_SERIAL],SizeTable[Type - PREF_SERIAL])))
			{
				if(WriteChunkRecords(Handle,Data,SizeTable[Type - PREF_SERIAL],1) != 1)
					return(FALSE);

				if(!(Error = PopChunk(Handle)))
					return(TRUE);
			}

			SetIoErr(Error);

			return(FALSE);
		}
	}

	return(TRUE);
}

STATIC LONG
IsConfigChunk(ULONG ID)
{
	LONG Type;

	for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
	{
		if(ID == TypeTable[Type - PREF_SERIAL])
			return(Type);
	}

	return(0);
}

STATIC BOOL
WriteConfigChunks(struct IFFHandle *Handle,struct Configuration *LocalConfig,APTR TempBuffer,STRPTR Password)
{
	LONG Type;

	for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
	{
		if(!WriteConfigChunk(Handle,LocalConfig,Type,TempBuffer,Password))
			return(FALSE);
	}

	return(TRUE);
}

STATIC VOID
ResetNewConfigEntry(APTR To,LONG Type)
{
	if(Type >= PREF_SERIAL && Type < PREF_RATES)
	{
		LONG Size;
		APTR From;

		From = GetConfigEntry(Config,Type);
		Size = SizeTable[Type - PREF_SERIAL];

		if(From)
			CopyMem(From,To,Size);
		else
		{
			memset(To,0,Size);

			(*ResetTable[Type - PREF_SERIAL])(To,NULL);
		}

		if(Type == PREF_SCREEN)
			FixScreenPens(To);
	}
}

STATIC BOOL
ClosePhonebookFile(struct IFFHandle *Handle)
{
	LONG Error;

	Error = PopChunk(Handle);

	if(!CloseIFFStream(Handle))
	{
		if(!Error)
			Error = IoErr();
	}

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}

STATIC struct IFFHandle *
CreatePhonebookFile(STRPTR Name,PhonebookHandle *PhoneHandle)
{
	struct IFFHandle *Handle;

	if(Handle = OpenIFFStream(Name,MODE_NEWFILE))
	{
		LONG Error;

		if(!(Error = PushChunk(Handle,ID_TERM,ID_CAT,IFFSIZE_UNKNOWN)))
		{
			if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
			{
				if(!(Error = PushChunk(Handle,0,ID_VERS,sizeof(struct TermInfo))))
				{
					struct TermInfo TermInfo;

					TermInfo.Version	= CONFIG_FILE_VERSION;
					TermInfo.Revision	= CONFIG_FILE_REVISION;

					if(WriteChunkRecords(Handle,&TermInfo,sizeof(struct TermInfo),1) != 1)
						Error = IoErr();
					else
						Error = PopChunk(Handle);
				}

				if(!Error && PhoneHandle->PhonePasswordUsed)
				{
					if(!(Error = PushChunk(Handle,0,ID_PSWD,MAX_PASSWORD_LENGTH)))
					{
						if(WriteChunkBytes(Handle,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH) != MAX_PASSWORD_LENGTH)
							Error = IoErr();
						else
							Error = PopChunk(Handle);
					}
				}

				if(!Error)
				{
					if(!(Error = PushChunk(Handle,0,ID_DIAL,sizeof(PhonebookGlobals))))
					{
						PhonebookGlobals Globals;

						Globals.Count			= PhoneHandle->NumPhoneEntries;
						Globals.ID				= PhoneHandle->PhonebookID;
						Globals.DefaultGroup	= PhoneHandle->DefaultGroup;
						Globals.AutoDial		= PhoneHandle->AutoDial;
						Globals.AutoExit		= PhoneHandle->AutoExit;

						if(WriteChunkBytes(Handle,&Globals,sizeof(Globals)) != sizeof(Globals))
							Error = IoErr();
						else
							Error = PopChunk(Handle);
					}
				}

				if(!Error)
					Error = PopChunk(Handle);
			}
		}

		if(Error)
		{
			CloseIFFStream(Handle);
			DeleteFile(Name);

			SetIoErr(Error);

			return(NULL);
		}
		else
			return(Handle);
	}
	else
		return(NULL);
}

STATIC BOOL
DumpPhonebookRates(struct IFFHandle *Handle,struct PhoneEntry *Entry)
{
	struct TimeDateNode *TimeDateNode;
	LONG Error;

	Error = 0;

	for(TimeDateNode = (struct TimeDateNode *)Entry->TimeDateList.mlh_Head ; !Error && TimeDateNode->Node.ln_Succ ; TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ)
	{
		if(!(Error = PushChunk(Handle,0,ID_DAT2,IFFSIZE_UNKNOWN)))
		{
			if(WriteChunkBytes(Handle,&TimeDateNode->Header,sizeof(struct TimeDateHeader)) != sizeof(struct TimeDateHeader))
				return(FALSE);
			else
			{
				if(WriteChunkBytes(Handle,TimeDateNode->Table,TimeDateNode->Table[0].Count * sizeof(struct TimeDate)) != TimeDateNode->Table[0].Count * sizeof(struct TimeDate))
					return(FALSE);
			}

			Error = PopChunk(Handle);
		}
	}

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}

STATIC BOOL
DumpPhonebookGroups(struct IFFHandle *Handle,struct List *GroupList)
{
	if(!IsListEmpty(GroupList))
	{
		struct PhoneGroupNode *GroupNode;
		struct PhoneNode *Node;
		PhoneGroupHeader Header;
		BOOL FormOpen;
		LONG Error;

		FormOpen = FALSE;

			/* So the extension stuff is set to zero */

		memset(&Header,0,sizeof(Header));

			/* Run down the groups */

		for(GroupNode = (PhoneGroupNode *)GroupList->lh_Head ; GroupNode->Node.ln_Succ ; GroupNode = (PhoneGroupNode *)GroupNode->Node.ln_Succ)
		{
				/* Count the members */

			Header.Count = GetListSize((struct List *)&GroupNode->GroupList);

				/* Open the group */

			if(Header.Count > 0)
			{
				if(!FormOpen)
				{
					if(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN))
					{
						SetIoErr(Error);

						return(FALSE);
					}

					FormOpen = TRUE;
				}

					/* Pick up the group name */

				strcpy(Header.FullName,GroupNode->LocalName);

				if(Error = PushChunk(Handle,0,ID_GRUP,sizeof(Header) + sizeof(LONG) * Header.Count))
				{
					SetIoErr(Error);

					return(FALSE);
				}
				else
				{
						/* First the header... */

					if(WriteChunkBytes(Handle,&Header,sizeof(Header)) == sizeof(Header))
					{
							/* ...then the IDs of the group members */

						for(Node = (struct PhoneNode *)GroupNode->GroupList.mlh_Head ; Node->Node.ln_Succ ; Node = (struct PhoneNode *)Node->Node.ln_Succ)
						{
							if(WriteChunkBytes(Handle,&Node->Entry->Header->ID,sizeof(ULONG)) != sizeof(ULONG))
								return(FALSE);
						}
					}
					else
						return(FALSE);

					if(Error = PopChunk(Handle))
					{
						SetIoErr(Error);

						return(FALSE);
					}
				}
			}
		}

		if(FormOpen)
		{
			if(Error = PopChunk(Handle))
			{
				SetIoErr(Error);

				return(FALSE);
			}
		}
	}

	return(TRUE);
}

STATIC BOOL
DumpPhonebookFile(struct IFFHandle *Handle,APTR TempBuffer,PhonebookHandle *PhoneHandle)
{
	PhoneEntry **Phonebook;
	STRPTR Password;
	APTR Whatever;
	APTR Stuff;
	LONG Error;
	LONG i;

	Error = 0;

	if(TempBuffer)
	{
		Password	= PhoneHandle->PhonePassword;
		Stuff		= TempBuffer;
	}
	else
	{
		Password	= NULL;
		Stuff		= NULL;
	}

	Phonebook = PhoneHandle->Phonebook;

	for(i = 0 ; !Error && i < PhoneHandle->NumPhoneEntries ; i++)
	{
		if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
		{
			if(!(Error = PushChunk(Handle,0,ID_PHON,sizeof(PhoneHeader))))
			{
					/* New feature for v4.7: store the dialing order in the phoneheader. */

				if(Phonebook[i]->Count < 0)
					Phonebook[i]->Header->Marked = 0;
				else
					Phonebook[i]->Header->Marked = Phonebook[i]->Count + 1;

				if(TempBuffer)
				{
					Encrypt((UBYTE *)Phonebook[i]->Header,sizeof(PhoneHeader),TempBuffer,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);

					Whatever = TempBuffer;
				}
				else
					Whatever = Phonebook[i]->Header;

				if(WriteChunkRecords(Handle,Whatever,sizeof(PhoneHeader),1) != 1)
					return(FALSE);

				Error = PopChunk(Handle);
			}

			if(!Error)
			{
				if(WriteConfigChunks(Handle,Phonebook[i]->Config,Stuff,Password))
				{
					if(!Phonebook[i]->Header->NoRates)
					{
						if(!DumpPhonebookRates(Handle,Phonebook[i]))
							return(FALSE);
					}
				}
				else
					return(FALSE);
			}

			if(!Error)
				Error = PopChunk(Handle);
		}
	}

		/* Now for the groups */

	if(!Error)
	{
		if(!DumpPhonebookGroups(Handle,(struct List *)&PhoneHandle->PhoneGroupList))
			Error = IoErr();
	}

	if(Error)
	{
		SetIoErr(Error);
		return(FALSE);
	}
	else
		return(TRUE);
}

STATIC BOOL
ReadConfigChunk(struct IFFHandle *Handle,struct Configuration *LocalConfig,LONG Type,LONG Len,STRPTR Password)
{
	if(CreateConfigEntry(LocalConfig,Type))
	{
		APTR Data;

		Data = GetConfigEntry(LocalConfig,Type);

		Len = MIN(SizeTable[Type - PREF_SERIAL],Len);

		if(ReadChunkBytes(Handle,Data,Len) == Len)
		{
			if(Password)
				Decrypt(Data,Len,Data,Password,MAX_PASSWORD_LENGTH);

			if(Type == PREF_SCREEN)
				FixScreenPens(Data);

			return(TRUE);
		}
	}

	return(FALSE);
}

STATIC APTR *
GetConfigEntryPointer(struct Configuration *LocalConfig,LONG Type)
{
	APTR *Mem;

	switch(Type)
	{
		case PREF_SERIAL:

			Mem = (APTR *)&LocalConfig->SerialConfig;
			break;

		case PREF_MODEM:

			Mem = (APTR *)&LocalConfig->ModemConfig;
			break;

		case PREF_COMMAND:

			Mem = (APTR *)&LocalConfig->CommandConfig;
			break;

		case PREF_SCREEN:

			Mem = (APTR *)&LocalConfig->ScreenConfig;
			break;

		case PREF_TERMINAL:

			Mem = (APTR *)&LocalConfig->TerminalConfig;
			break;

		case PREF_PATH:

			Mem = (APTR *)&LocalConfig->PathConfig;
			break;

		case PREF_MISC:

			Mem = (APTR *)&LocalConfig->MiscConfig;
			break;

		case PREF_CLIP:

			Mem = (APTR *)&LocalConfig->ClipConfig;
			break;

		case PREF_CAPTURE:

			Mem = (APTR *)&LocalConfig->CaptureConfig;
			break;

		case PREF_FILE:

			Mem = (APTR *)&LocalConfig->FileConfig;
			break;

		case PREF_EMULATION:

			Mem = (APTR *)&LocalConfig->EmulationConfig;
			break;

		case PREF_TRANSFER:

			Mem = (APTR *)&LocalConfig->TransferConfig;
			break;

		case PREF_TRANSLATIONFILENAME:

			Mem = (APTR *)&LocalConfig->TranslationFileName;
			break;

		case PREF_MACROFILENAME:

			Mem = (APTR *)&LocalConfig->MacroFileName;
			break;

		case PREF_CURSORFILENAME:

			Mem = (APTR *)&LocalConfig->CursorFileName;
			break;

		case PREF_FASTMACROFILENAME:

			Mem = (APTR *)&LocalConfig->FastMacroFileName;
			break;

		case PREF_SPEECHFILENAME:

			Mem = (APTR *)&LocalConfig->SpeechFileName;
			break;

		case PREF_SOUNDFILENAME:

			Mem = (APTR *)&LocalConfig->SoundFileName;
			break;

		case PREF_AREACODEFILENAME:

			Mem = (APTR *)&LocalConfig->AreaCodeFileName;
			break;

		case PREF_PHONEBOOKFILENAME:

			Mem = (APTR *)&LocalConfig->PhonebookFileName;
			break;

		case PREF_HOTKEYFILENAME:

			Mem = (APTR *)&LocalConfig->HotkeyFileName;
			break;

		case PREF_TRAPFILENAME:

			Mem = (APTR *)&LocalConfig->TrapFileName;
			break;

		default:

			Mem = NULL;
			break;
	}

	return(Mem);
}

	/* ResetConfig():
	 *
	 *	Initialize configuration with default values.
	 */

VOID
ResetConfig(struct Configuration *LocalConfig,CONST_STRPTR PathBuffer)
{
	CONST_STRPTR Arg;
	LONG Type;
	APTR Data;

	if(!PathBuffer)
		PathBuffer = "TERM:config";

	for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
	{
		if(Data = GetConfigEntry(LocalConfig,Type))
		{
			memset(Data,0,SizeTable[Type - PREF_SERIAL]);

			switch(Type)
			{
				case PREF_PATH:
				case PREF_FILE:
				case PREF_TRANSLATIONFILENAME:
				case PREF_MACROFILENAME:
				case PREF_CURSORFILENAME:
				case PREF_FASTMACROFILENAME:
				case PREF_SPEECHFILENAME:
				case PREF_SOUNDFILENAME:
				case PREF_AREACODEFILENAME:
				case PREF_PHONEBOOKFILENAME:
				case PREF_HOTKEYFILENAME:
				case PREF_TRAPFILENAME:

					Arg = PathBuffer;
					break;

				case PREF_TRANSFER:

					if(LocalConfig->FileConfig)
						Arg = LocalConfig->FileConfig->ProtocolFileName;
					else
						Arg = "xprzmodem.library";

					break;

				default:

					Arg = NULL;
					break;
			}

			(*ResetTable[Type - PREF_SERIAL])(Data,Arg);
		}
	}
}

VOID
DeleteConfigEntry(struct Configuration *LocalConfig,LONG Type)
{
	if(Type == PREF_ALL)
	{
		for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
			DeleteConfigEntry(LocalConfig,Type);
	}
	else
	{
		APTR *Mem;

		if(Mem = GetConfigEntryPointer(LocalConfig,Type))
		{
			FreeVecPooled(*Mem);

			*Mem = NULL;
		}
	}
}

VOID
ResetConfigEntry(struct Configuration *LocalConfig,LONG Type)
{
	if(LocalConfig && Type >= PREF_SERIAL && Type < PREF_RATES)
	{
		APTR To;

		if(To = GetConfigEntry(LocalConfig,Type))
			ResetNewConfigEntry(To,Type);
	}
}

APTR
GetConfigEntry(struct Configuration *LocalConfig,LONG Type)
{
	if(LocalConfig)
	{
		APTR *Mem;

		if(Mem = GetConfigEntryPointer(LocalConfig,Type))
			return(*Mem);
	}

	return(NULL);
}

LONG
CompareConfigEntries(APTR a,APTR b,LONG Type)
{
	LONG Result;

	if(Type < PREF_SERIAL || Type >= PREF_RATES || a == b)
		Result = 0;
	else
		Result = memcmp(a,b,SizeTable[Type - PREF_SERIAL]);

	return(Result);
}

VOID
PutConfigEntry(struct Configuration *LocalConfig,APTR Data,LONG Type)
{
	if(LocalConfig && Data && Type >= PREF_SERIAL && Type < PREF_RATES)
	{
		APTR Destination;

		if(Destination = GetConfigEntry(LocalConfig,Type))
		{
			if(Data != Destination)
				CopyMem(Data,Destination,SizeTable[Type - PREF_SERIAL]);
		}
	}
}

VOID
CopyConfigEntry(struct Configuration *LocalConfig,LONG Type,APTR Data)
{
	if(LocalConfig && Data && Type >= PREF_SERIAL && Type < PREF_RATES)
	{
		APTR Source;

		if(Source = GetConfigEntry(LocalConfig,Type))
		{
			if(Source != Data)
				CopyMem(Source,Data,SizeTable[Type - PREF_SERIAL]);
		}
	}
}

APTR
CreateNewConfigEntry(LONG Type)
{
	APTR Data;

	Data = NULL;

	if(Type >= PREF_SERIAL && Type < PREF_RATES)
	{
		if(Data = AllocVecPooled(SizeTable[Type - PREF_SERIAL],MEMF_ANY | MEMF_CLEAR))
			ResetNewConfigEntry(Data,Type);
	}

	return(Data);
}

BOOL
CreateConfigEntry(struct Configuration *LocalConfig,LONG Type)
{
	if(Type == PREF_ALL)
	{
		for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
		{
			if(!CreateConfigEntry(LocalConfig,Type))
			{
				DeleteConfigEntry(LocalConfig,PREF_ALL);
				return(FALSE);
			}
		}

		return(TRUE);
	}
	else
	{
		if(Type >= PREF_SERIAL && Type < PREF_RATES)
		{
			APTR *Mem;

			if(Mem = GetConfigEntryPointer(LocalConfig,Type))
			{
				if(*Mem)
					return(TRUE);
				else
				{
					if(*Mem = CreateNewConfigEntry(Type))
						return(TRUE);
				}
			}
		}

		return(FALSE);
	}
}

VOID
DeleteConfiguration(struct Configuration *LocalConfig)
{
	if(LocalConfig)
	{
		DeleteConfigEntry(LocalConfig,PREF_ALL);

		FreeVecPooled(LocalConfig);
	}
}

struct Configuration *
CreateConfiguration(BOOL Fill)
{
	struct Configuration *LocalConfig;

	if(LocalConfig = (struct Configuration *)AllocVecPooled(sizeof(struct Configuration),MEMF_ANY | MEMF_CLEAR))
	{
		if(Fill)
		{
			if(!CreateConfigEntry(LocalConfig,PREF_ALL))
			{
				FreeVecPooled(LocalConfig);

				return(NULL);
			}
		}

		return(LocalConfig);
	}
	else
		return(NULL);
}

VOID
SaveConfig(struct Configuration *Src,struct Configuration *Dst)
{
	APTR From,To;
	LONG Type;

	for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
	{
		From	= GetConfigEntry(Src,Type);
		To		= GetConfigEntry(Dst,Type);

		if(From && To)
		{
			CopyMem(From,To,SizeTable[Type - PREF_SERIAL]);

			if(Type == PREF_SCREEN)
				FixScreenPens(To);
		}
	}
}

VOID
SwapConfig(struct Configuration *Src,struct Configuration *Dst)
{
	APTR From,To;
	LONG Type;

	for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
	{
		From	= GetConfigEntry(Src,Type);
		To		= GetConfigEntry(Dst,Type);

		if(From && To)
			SwapMem(From,To,SizeTable[Type - PREF_SERIAL]);
	}
}

BOOL
SavePhonebook(STRPTR Name,PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle->Phonebook && PhoneHandle->NumPhoneEntries > 0)
	{
		struct IFFHandle *Handle;
		LONG Error;

		if(!(Handle = CreatePhonebookFile(Name,PhoneHandle)))
			Error = IoErr();
		else
		{
			APTR TempBuffer;

			TempBuffer = NULL;
			Error = 0;

			if(PhoneHandle->PhonePasswordUsed)
			{
				LONG Max = sizeof(PhoneHeader);
				LONG Type;

				for(Type = PREF_SERIAL ; Type < PREF_RATES ; Type++)
				{
					if(SizeTable[Type - PREF_SERIAL] > Max)
						Max = SizeTable[Type - PREF_SERIAL];
				}

				if(!(TempBuffer = AllocVecPooled(Max,MEMF_ANY)))
					Error = ERROR_NO_FREE_STORE;
			}

			if(!Error)
			{
				if(!DumpPhonebookFile(Handle,TempBuffer,PhoneHandle))
					Error = IoErr();
			}

			FreeVecPooled(TempBuffer);

			if(!ClosePhonebookFile(Handle))
			{
				if(!Error)
					Error = IoErr();
			}

			if(Error)
				DeleteFile(Name);
		}

		if(Error)
		{
			SetIoErr(Error);

			return(FALSE);
		}
	}

	return(TRUE);
}

	/* LoadPhonebook(STRPTR Name):
	 *
	 *	Restore a phone book from a disk file.
	 */

PhonebookHandle *
LoadPhonebook(STRPTR Name)
{
	struct PhoneEntry		**Phonebook = NULL;
	struct IFFHandle		*Handle;
	struct ContextNode		*Chunk;
	UBYTE					 ConfigChunkType;
	LONG					 Error;
	PhonebookHandle			*PhoneHandle;
	struct PhonebookGlobals	 Globals;
	struct PhoneGroupHeader	 GroupHeader;
	struct Configuration	*LocalConfig;
	LONG					 i;
	struct TermInfo			 TermInfo;
	BOOL					 FirstChunk;
	BOOL					 CheckedChunk;
	BOOL					 GotID;
	LONG					 Index;
	LONG					 Size;
	UBYTE					 PasswordBuffer[MAX_PASSWORD_LENGTH];
	BOOL					 UsePhonePassword;

	PhoneHandle 		= NULL;
	FirstChunk			= TRUE;
	CheckedChunk		= FALSE;
	GotID				= FALSE;
	Index				= 0;
	UsePhonePassword	= FALSE;

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
		if(!(Error = StopChunks(Handle,Stops,NUM_STOPS)))
		{
			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Chunk = CurrentChunk(Handle);

					/* Is this the first chunk to be read? */

				if(!CheckedChunk)
				{
					CheckedChunk = TRUE;

						/* The first chunk must be a
						 * catalog chunk, or this would
						 * not be a proper phonebook file.
						 */

					if(Chunk->cn_ID != ID_CAT)
					{
						Error = ERROR_OBJECT_WRONG_TYPE;

						break;
					}
				}

				switch(Chunk->cn_ID)
				{
					case ID_VERS:

						memset(&TermInfo,0,sizeof(TermInfo));

						if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) == sizeof(struct TermInfo))
						{
							if(TermInfo.Version != CONFIG_FILE_VERSION || (TermInfo.Version == CONFIG_FILE_VERSION && TermInfo.Revision > CONFIG_FILE_REVISION))
							{
								if(TermInfo.Version == 2 && TermInfo.Revision == 4)
								{
									CloseIFFStream(Handle);

									return(LoadOldPhonebook(Name));
								}
								else
								{
									if(TermInfo.Version != 3)
										Error = ERROR_OBJECT_WRONG_TYPE;
								}
							}
						}
						else
							Error = IoErr();

						break;

					case ID_PSWD:

						if(ReadChunkBytes(Handle,PasswordBuffer,MAX_PASSWORD_LENGTH) == MAX_PASSWORD_LENGTH)
						{
							BOOL AskPassword = TRUE;

							if(GlobalPhoneHandle->PhonePasswordUsed)
							{
								if(!memcmp(GlobalPhoneHandle->PhonePassword,PasswordBuffer,MAX_PASSWORD_LENGTH))
								{
									UsePhonePassword	= TRUE;
									AskPassword			= FALSE;
								}
							}

							if(AskPassword)
							{
								UBYTE LocalBuffer[MAX_PASSWORD_LENGTH+1];
								BOOL Ok;

								memset(LocalBuffer,0,sizeof(LocalBuffer));
								Ok = FALSE;

								if(GetString(FALSE,TRUE,sizeof(LocalBuffer),LocaleString(MSG_PHONEPANEL_PLEASE_ENTER_PASSWORD_TXT),LocalBuffer))
								{
									UBYTE AnotherBuffer[MAX_PASSWORD_LENGTH];

									Encrypt(LocalBuffer,MAX_PASSWORD_LENGTH,AnotherBuffer,LocalBuffer,strlen(LocalBuffer));

									if(!memcmp(PasswordBuffer,AnotherBuffer,MAX_PASSWORD_LENGTH))
									{
										UsePhonePassword	= TRUE;
										Ok					= TRUE;
									}
								}

								if(!Ok)
								{
									ShowRequest(Window,LocaleString(MSG_TERMPHONE_WRONG_PASSWORD_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Name);

									Error = ERROR_REQUIRED_ARG_MISSING;
								}
							}
						}
						else
							Error = IoErr();

						break;

					case ID_DIAL:

						Size = MIN(sizeof(Globals),Chunk->cn_Size);

						memset(&Globals,0,sizeof(Globals));

						if(ReadChunkBytes(Handle,&Globals,Size) == Size)
						{
							if(Globals.Count > 0)
							{
								if(!(PhoneHandle = CreatePhonebook(Globals.Count,TRUE)))
									Error = ERROR_NO_FREE_STORE;
								else
								{
									if(Size >= sizeof(PhonebookGlobals))
									{
										PhoneHandle->PhonebookID	= Globals.ID;
										PhoneHandle->DefaultGroup	= Globals.DefaultGroup;
										PhoneHandle->AutoDial		= Globals.AutoDial;
										PhoneHandle->AutoExit		= Globals.AutoExit;

										GotID = TRUE;
									}
									else
									{
										struct timeval Now;

										GetSysTime(&Now);

										PhoneHandle->PhonebookID	= Now.tv_secs;
										PhoneHandle->DefaultGroup	= 0;
										PhoneHandle->AutoDial		= FALSE;
										PhoneHandle->AutoExit		= FALSE;

										GotID = FALSE;
									}

									if(UsePhonePassword)
									{
										PhoneHandle->PhonePasswordUsed = TRUE;
										CopyMem(PasswordBuffer,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);
									}

									Phonebook = PhoneHandle->Phonebook;
								}
							}
						}
						else
							Error = IoErr();

						break;

					case ID_PHON:

						Size = MIN(sizeof(PhoneHeader),Chunk->cn_Size);

						if(FirstChunk)
							FirstChunk = FALSE;
						else
							Index++;

						memset(Phonebook[Index]->Header,0,sizeof(PhoneHeader));

						if(ReadChunkBytes(Handle,Phonebook[Index]->Header,Size) == Size)
						{
							if(PhoneHandle->PhonePasswordUsed)
								Decrypt((UBYTE *)Phonebook[Index]->Header,Size,(UBYTE *)Phonebook[Index]->Header,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);

							if(!GotID)
								Phonebook[Index]->Header->ID = PhoneHandle->PhonebookID++;
						}
						else
							Error = IoErr();

						break;

						/* Mucho importante -- must follow the phonebook entries */

					case ID_GRUP:

						if(!Phonebook[PhoneHandle->NumPhoneEntries - 1])
						{
							Error = ERR_LOAD_ERROR;
							break;
						}

						DB(kprintf("--> ID_GRUP\n"));

						if(ReadChunkBytes(Handle,&GroupHeader,sizeof(GroupHeader)) == sizeof(GroupHeader))
						{
							PhoneGroupNode *GroupNode;
							ULONG ID;

							GroupNode = NULL;

							DB(kprintf("group name |%s| entries %ld\n",GroupHeader.FullName,GroupHeader.Count));

							for(i = 0 ; !Error && i < GroupHeader.Count ; i++)
							{
								if(ReadChunkBytes(Handle,&ID,sizeof(ULONG)) == sizeof(ULONG))
								{
									LONG j;

									DB(kprintf("looking for 0x%08lx\n",ID));

									for(j = 0 ; j < PhoneHandle->NumPhoneEntries ; j++)
									{
										if(Phonebook[j]->Header->ID == ID)
										{
											DB(kprintf("found it |%s|\n",Phonebook[j]->Header->Name));

											if(!GroupNode)
												GroupNode = CreatePhoneGroup(PhoneHandle,GroupHeader.FullName);

											if(!GroupNode)
											{
												Error = ERROR_NO_FREE_STORE;

												break;
											}

											if(!AddGroupEntry(GroupNode,Phonebook[j]))
											{
												Error = ERROR_NO_FREE_STORE;

												break;
											}
										}
									}
								}
								else
								{
									Error = IoErr();

									break;
								}
							}
						}
						else
							Error = IoErr();

						break;

					/* Special treatment for obsolete "FILE" chunk */

					case ID_FILE:

						LocalConfig = Phonebook[Index]->Config;

						for(i = PREF_TRANSLATIONFILENAME ; !Error && i < PREF_RATES ; i++)
						{
							if(!CreateConfigEntry(LocalConfig,i))
								Error = ERROR_NO_FREE_STORE;
						}

						if(!Error)
						{
							strcpy(LocalConfig->TranslationFileName,	LocalConfig->FileConfig->TranslationFileName);
							strcpy(LocalConfig->MacroFileName,		LocalConfig->FileConfig->MacroFileName);
							strcpy(LocalConfig->CursorFileName,		LocalConfig->FileConfig->CursorFileName);
							strcpy(LocalConfig->FastMacroFileName,	LocalConfig->FileConfig->FastMacroFileName);

							DeleteConfigEntry(LocalConfig,PREF_FILE);
						}

						break;

					case ID_DATE:

						if(!Phonebook[Index]->Header->NoRates)
						{
							LONG Count = (Chunk->cn_Size - sizeof(struct TimeDateHeader)) / sizeof(struct TimeDateOld);
							struct TimeDateNode *TimeDateNode;

							if(TimeDateNode = CreateTimeDateNode(-1,-1,"",Count))
							{
								struct TimeDateOld *Old;

								if(Old = (struct TimeDateOld *)AllocVecPooled(sizeof(struct TimeDateOld) * Count,MEMF_ANY))
								{
									if(ReadChunkBytes(Handle,&TimeDateNode->Header,sizeof(struct TimeDateHeader)) == sizeof(struct TimeDateHeader))
									{
										if(ReadChunkRecords(Handle,Old,sizeof(struct TimeDateOld),Count) == Count)
										{
											for(i = 0 ; i < Count ; i++)
												ConvertTimeDate(&Old[i],&TimeDateNode->Table[i]);

											AdaptTimeDateNode(TimeDateNode);

											AddTail((struct List *)&Phonebook[Index]->TimeDateList,&TimeDateNode->Node);

											TimeDateNode = NULL;
										}
										else
											Error = IoErr();
									}
									else
										Error = IoErr();

									FreeVecPooled(Old);
								}
								else
									Error = ERROR_NO_FREE_STORE;

								FreeTimeDateNode(TimeDateNode);
							}
							else
								Error = ERROR_NO_FREE_STORE;
						}

						break;

					case ID_DAT2:

						if(!Phonebook[Index]->Header->NoRates)
						{
							LONG Count = (Chunk->cn_Size - sizeof(struct TimeDateHeader)) / sizeof(struct TimeDate);
							struct TimeDateNode *TimeDateNode;

							if(TimeDateNode = CreateTimeDateNode(-1,-1,"",Count))
							{
								if(ReadChunkBytes(Handle,&TimeDateNode->Header,sizeof(struct TimeDateHeader)) == sizeof(struct TimeDateHeader))
								{
									if(ReadChunkRecords(Handle,TimeDateNode->Table,sizeof(struct TimeDate),Count) == Count)
									{
										AdaptTimeDateNode(TimeDateNode);

										AddTail((struct List *)&Phonebook[Index]->TimeDateList,&TimeDateNode->Node);

										TimeDateNode = NULL;
									}
									else
										Error = IoErr();
								}
								else
									Error = IoErr();

								FreeTimeDateNode(TimeDateNode);
							}
							else
								Error = IoErr();
						}

						break;

					default:

						if(ConfigChunkType = IsConfigChunk(Chunk->cn_ID))
						{
							if(ReadConfigChunk(Handle,Phonebook[Index]->Config,ConfigChunkType,Chunk->cn_Size,PhoneHandle->PhonePasswordUsed ? PhoneHandle->PhonePassword : NULL))
								FixOldConfig(Phonebook[Index]->Config,ConfigChunkType,TRUE,TermInfo.Version,TermInfo.Revision);
							else
								Error = IoErr();
						}

						break;
				}

				if(Error)
					break;
			}

			if(!Error && PhoneHandle)
			{
				if(!Phonebook[PhoneHandle->NumPhoneEntries - 1])
					Error = ERR_LOAD_ERROR;
			}

			if(!Error)
			{
				for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
				{
					FinalFix(Phonebook[i]->Config,TRUE,TermInfo.Version,TermInfo.Revision);
					StripGlobals(Phonebook[i]->Config);
				}
			}
		}

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

	if(Error)
	{
		if(PhoneHandle)
			DeletePhonebook(PhoneHandle);

		SetIoErr(Error);

		return(NULL);
	}
	else
		return(PhoneHandle);
}

	/* WriteConfig(STRPTR Name,struct Configuration *LocalConfig):
	 *
	 *	Write the configuration to a file, very much like
	 *	WriteIFFData().
	 */

BOOL
WriteConfig(STRPTR Name,struct Configuration *LocalConfig)
{
	struct IFFHandle *Handle;
	LONG Error;

		/* Allocate a handle. */

	if(Handle = OpenIFFStream(Name,MODE_NEWFILE))
	{
			/* Push outmost chunk onto stack. */

		if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
		{
				/* Add a version identifier. */

			if(!(Error = PushChunk(Handle,0,ID_VERS,IFFSIZE_UNKNOWN)))
			{
				struct TermInfo TermInfo;

				TermInfo.Version	= CONFIG_FILE_VERSION;
				TermInfo.Revision	= CONFIG_FILE_REVISION;

					/* Update the other configuration pointer as well. */

				LocalConfig->SerialConfig->LastVersionSaved		= TermVersion;
				LocalConfig->SerialConfig->LastRevisionSaved	= TermRevision;

					/* Write the version data. */

				if(WriteChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) != sizeof(struct TermInfo))
					Error = IoErr();
				else
					Error = PopChunk(Handle);
			}

			if(!Error)
			{
				if(!WriteConfigChunks(Handle,LocalConfig,NULL,NULL))
					Error = IoErr();
			}

			if(!Error)
			{
				LONG i;

				for(i = 0 ; !Error && WindowInfoTable[i].ID != -1 ; i++)
				{
					if(!(Error = PushChunk(Handle,0,ID_WINF,sizeof(struct WindowInfo))))
					{
						if(WriteChunkBytes(Handle,&WindowInfoTable[i],sizeof(struct WindowInfo)) != sizeof(struct WindowInfo))
							Error = IoErr();
						else
							Error = PopChunk(Handle);
					}
				}
			}

				/* Seems that we're finished, now try to pop the FORM chunk
				 * and return.
				 */

			if(!Error)
				Error = PopChunk(Handle);
		}

			/* Close the handle (flush any pending data). */

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

	if(Error)
	{
		DeleteFile(Name);
		SetIoErr(Error);

		return(FALSE);
	}
	else
	{
		AddProtection(Name,FIBF_EXECUTE);

		return(TRUE);
	}
}

	/* ReadConfig(STRPTR Name,struct Configuration *LocalConfig):
	 *
	 *	Read the configuration file, very much the same as ReadIFFData().
	 */

BOOL
ReadConfig(STRPTR Name,struct Configuration *LocalConfig)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
		if(!(Error = StopChunks(Handle,Stops,NUM_STOPS)))
		{
			struct WindowInfo WindowInfo;
			struct ContextNode *Chunk;
			struct TermInfo TermInfo;
			LONG ConfigChunkType;
			LONG Type;

			memset(&TermInfo,0,sizeof(TermInfo));

			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Chunk = CurrentChunk(Handle);

				switch(Chunk->cn_ID)
				{
						/* Oops! Someone is trying to
						 * use the phone book file as
						 * a configuration file.
						 */

					case ID_CAT:

						Error = ERROR_OBJECT_WRONG_TYPE;

						break;

						/* File version information. */

					case ID_VERS:

						if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) != sizeof(struct TermInfo))
							Error = IoErr();
						else
						{
							if(TermInfo.Version != CONFIG_FILE_VERSION || (TermInfo.Version == CONFIG_FILE_VERSION && TermInfo.Revision > CONFIG_FILE_REVISION))
							{
									/* Old file format (v2.4); stop here and pass
									 * control to the compatibility code.
									 */

								if(TermInfo.Version == 2 && TermInfo.Revision == 4)
								{
									CloseIFFStream(Handle);

									return(ReadOldConfig(Name,LocalConfig));
								}
								else
								{
									if(TermInfo.Version != 3)
										Error = ERROR_OBJECT_WRONG_TYPE;
								}
							}
						}

						break;

						/* Window placement information. */

					case ID_WINF:

						if(ReadChunkBytes(Handle,&WindowInfo,sizeof(struct WindowInfo)) != sizeof(struct WindowInfo))
							Error = IoErr();
						else
							ReplaceWindowInfo(&WindowInfo);

						break;

						/* Special treatment for obsolete "FILE" chunk */

					case ID_FILE:

						for(Type = PREF_TRANSLATIONFILENAME ; !Error && Type < PREF_RATES ; Type++)
						{
							if(!CreateConfigEntry(LocalConfig,Type))
								Error = ERROR_NO_FREE_STORE;
						}

						if(!Error)
						{
							strcpy(LocalConfig->TranslationFileName,	LocalConfig->FileConfig->TranslationFileName);
							strcpy(LocalConfig->MacroFileName,			LocalConfig->FileConfig->MacroFileName);
							strcpy(LocalConfig->CursorFileName,			LocalConfig->FileConfig->CursorFileName);
							strcpy(LocalConfig->FastMacroFileName,		LocalConfig->FileConfig->FastMacroFileName);

							DeleteConfigEntry(LocalConfig,PREF_FILE);
						}

						break;

						/* Any other configuration file chunk. */

					default:

						if(ConfigChunkType = IsConfigChunk(Chunk->cn_ID))
						{
							if(!ReadConfigChunk(Handle,LocalConfig,ConfigChunkType,Chunk->cn_Size,NULL))
								Error = IoErr();
							else
								FixOldConfig(LocalConfig,ConfigChunkType,FALSE,TermInfo.Version,TermInfo.Revision);
						}

						break;
				}

					/* Stop in case of error. */

				if(Error)
					break;
			}

				/* Patch up configuration file information if necessary. */

			if(!Error)
				FinalFix(LocalConfig,FALSE,TermInfo.Version,TermInfo.Revision);
		}

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}

/*****************************************************************************/

	/* ResetHotkeys(struct Hotkeys *Hotkeys):
	 *
	 *	Reset hotkey assignments to defaults.
	 */

VOID
ResetHotkeys(struct Hotkeys *Hotkeys)
{
	strcpy(Hotkeys->termScreenToFront,		"lshift rshift return");
	strcpy(Hotkeys->BufferScreenToFront,	"control rshift return");
	strcpy(Hotkeys->SkipDialEntry,			"control lshift rshift return");
	strcpy(Hotkeys->AbortARexx,				"lshift rshift escape");

	Hotkeys->CommodityPriority	= 0;
	Hotkeys->HotkeysEnabled		= TRUE;
}

	/* ResetSpeechConfig(struct SpeechConfig *SpeechConfig):
	 *
	 *	Reset speech configuration to defaults.
	 */

VOID
ResetSpeechConfig(struct SpeechConfig *SpeechConfig)
{
	SpeechConfig->Rate		= DEFRATE;
	SpeechConfig->Pitch		= DEFPITCH;
	SpeechConfig->Frequency	= DEFFREQ;
	SpeechConfig->Volume	= DEFVOL;
	SpeechConfig->Sex		= DEFSEX;
	SpeechConfig->Enabled	= FALSE;
}

	/* ResetCursorKeys(struct CursorKeys *Keys):
	 *
	 *	Reset cursor key assignments to defaults.
	 */

VOID
ResetCursorKeys(struct CursorKeys *Keys)
{
	STATIC STRPTR Defaults[4] =
	{
		"\\e[A",
		"\\e[B",
		"\\e[C",
		"\\e[D"
	};

	LONG i,j;

	for(i = 0 ; i < 4 ; i++)
	{
		for(j = 0 ; j < 4 ; j++)
			strcpy(Keys->Keys[j][i],Defaults[i]);
	}
}

	/* ResetSound(struct SoundConfig *SoundConfig):
	 *
	 *	Reset the sound settings to defaults.
	 */

VOID
ResetSound(struct SoundConfig *SoundConfig)
{
	memset(SoundConfig,0,sizeof(struct SoundConfig));

	SoundConfig->Volume = 100;

	strcpy(SoundConfig->BellFile,Config->TerminalConfig->BeepFileName);
}

	/* ResetMacroKeys(struct MacroKeys *Keys):
	 *
	 *	Reset the macro key assignments to defaults.
	 */

VOID
ResetMacroKeys(struct MacroKeys *Keys)
{
	STATIC STRPTR FunctionKeyCodes[4] =
	{
		"\\eOP",
		"\\eOQ",
		"\\eOR",
		"\\eOS"
	};

	LONG i;

	memset(Keys,0,sizeof(struct MacroKeys));

	for(i = 0 ; i < 4 ; i++)
		strcpy(Keys->Keys[1][i],FunctionKeyCodes[i]);
}

	/* LoadMacros(STRPTR Name,struct MacroKeys *Keys):
	 *
	 *	Load the keyboard macros from a file.
	 */

BOOL
LoadMacros(STRPTR Name,struct MacroKeys *Keys)
{
	struct StoredProperty *Prop;
	struct TermInfo *TermInfo;
	struct IFFHandle *Handle;
	LONG Error;

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
			/* Collect version number ID if
			 * available.
			 */

		if(!(Error = PropChunks(Handle,(LONG *)VersionProps,1)))
		{
				/* The following line tells iffparse to stop at the
				 * very beginning of a `Type' chunk contained in a
				 * `TERM' FORM chunk.
				 */

			if(!(Error = StopChunk(Handle,ID_TERM,ID_KEYS)))
			{
					/* Parse the file... */

				if(!ParseIFF(Handle,IFFPARSE_SCAN))
				{
						/* Did we get a version ID? */

					if(Prop = FindProp(Handle,ID_TERM,ID_VERS))
					{
						TermInfo = (struct TermInfo *)Prop->sp_Data;

							/* Is it the file format we are able
							 * to read?
							 */

						if((TermInfo->Version > CONFIG_FILE_VERSION) || (TermInfo->Version == CONFIG_FILE_VERSION && TermInfo->Revision > CONFIG_FILE_REVISION) || (TermInfo->Version == 1 && TermInfo->Revision < 6))
						{
								/* Probably an older revision. */

							if(TermInfo->Version == 1 && TermInfo->Revision < 6)
							{
								memset(Keys,0,sizeof(struct MacroKeys));

								if(ReadChunkBytes(Handle,Keys,10 * 256) != 10 * 256)
									Error = IoErr();
							}
							else
								Error = ERR_OUTDATED;
						}
						else
						{
							/* The file read pointer is positioned
							 * just in front of the first data
							 * to be read, so let's don't disappoint
							 * iffparse and read it.
							 */

							if(ReadChunkBytes(Handle,Keys,sizeof(struct MacroKeys)) != sizeof(struct MacroKeys))
								Error = IoErr();
						}
					}
					else
					{
							/* File was created by WriteIFFData previous
							 * to revision 1.4.
							 */

						memset(Keys,0,sizeof(struct MacroKeys));

						if(ReadChunkBytes(Handle,Keys,10 * 256) != 10 * 256)
							Error = IoErr();
					}
				}
			}
		}

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

	if(!Error)
		return(TRUE);
	else
	{
		SetIoErr(Error);

		return(FALSE);
	}
}

/*****************************************************************************/

	/* WriteIFFData(STRPTR Name,APTR Data,LONG Size,ULONG Type):
	 *
	 *	Write data to an IFF file (via iffparse.library).
	 */

BOOL
WriteIFFData(STRPTR Name,APTR Data,LONG Size,ULONG Type)
{
	struct IFFHandle *Handle;
	LONG Error;

		/* Allocate a handle. */

	if(!(Handle = OpenIFFStream(Name,MODE_NEWFILE)))
		Error = IoErr();
	else
	{
			/* Push outmost chunk onto stack. */

		if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
		{
				/* Add a version identifier. */

			if(!(Error = PushChunk(Handle,0,ID_VERS,IFFSIZE_UNKNOWN)))
			{
				struct TermInfo TermInfo;

				TermInfo.Version	= CONFIG_FILE_VERSION;
				TermInfo.Revision	= CONFIG_FILE_REVISION;

					/* Write the version data. */

				if(WriteChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) != sizeof(struct TermInfo))
					Error = IoErr();
				else
					Error = PopChunk(Handle);
			}

				/* Push the real data chunk on the stack. */

			if(!Error)
			{
				if(!(Error = PushChunk(Handle,0,Type,Size)))
				{
						/* Write the data. */

					if(WriteChunkBytes(Handle,Data,Size) != Size)
						Error = IoErr();
					else
						Error = PopChunk(Handle);
				}
			}

				/* Seems that we're done, now try to pop the FORM chunk
				 * and return.
				 */

			if(!Error)
				Error = PopChunk(Handle);
		}

		CloseIFFStream(Handle);
	}

	if(!Error)
	{
		AddProtection(Name,FIBF_EXECUTE);

		return(TRUE);
	}
	else
	{
		DeleteFile(Name);
		SetIoErr(Error);

		return(FALSE);
	}
}

	/* ReadIFFData(STRPTR Name,APTR Data,LONG Size,ULONG Type):
	 *
	 *	Read data from a `TERM' FORM chunk contained in an IFF file.
	 */

BOOL
ReadIFFData(STRPTR Name,APTR Data,LONG Size,ULONG Type)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(!(Handle = OpenIFFStream(Name,MODE_OLDFILE)))
		Error = IoErr();
	else
	{
			/* Collect version number ID if
			 * available.
			 */

		if(!(Error = PropChunks(Handle,(LONG *)VersionProps,1)))
		{
				/* The following line tells iffparse to stop at the
				 * very beginning of a `Type' chunk contained in a
				 * `TERM' FORM chunk.
				 */

			if(!(Error = StopChunk(Handle,ID_TERM,Type)))
			{
					/* Parse the file... */

				if(!ParseIFF(Handle,IFFPARSE_SCAN))
				{
					struct StoredProperty *Prop;

						/* Did we get a version ID? */

					if(!(Prop = FindProp(Handle,ID_TERM,ID_VERS)))
						Error = ERR_OUTDATED;
					else
					{
						struct TermInfo *TermInfo;

						TermInfo = (struct TermInfo *)Prop->sp_Data;

							/* Is it the file format we are able
							 * to read?
							 */

						if((TermInfo->Version > CONFIG_FILE_VERSION) || (TermInfo->Version == CONFIG_FILE_VERSION && TermInfo->Revision > CONFIG_FILE_REVISION) || (TermInfo->Version == 1 && TermInfo->Revision < 6))
							Error = ERR_OUTDATED;
						else
						{
							struct ContextNode *Chunk = CurrentChunk(Handle);

							if(Chunk->cn_Size < Size)
								Size = Chunk->cn_Size;

								/* The file read pointer is positioned
								 * just in front of the first data
								 * to be read, so let's don't disappoint
								 * iffparse and read it.
								 */

							if(ReadChunkBytes(Handle,Data,Size) != Size)
								Error = IoErr();
						}
					}
				}
			}
		}

		CloseIFFStream(Handle);
	}

	if(!Error)
		return(TRUE);
	else
	{
		SetIoErr(Error);
		return(FALSE);
	}
}
