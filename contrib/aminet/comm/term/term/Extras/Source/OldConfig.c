/*
**	OldConfig.c
**
**	Old configuration processing routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

struct OldSerialSettings
{
	ULONG	BaudRate;
	ULONG	BreakLength;
	ULONG	SerialBufferSize;

	UBYTE	SerialDevice[256];
	LONG	UnitNumber;

	BOOLEAN	BitsPerChar;
	BOOLEAN	Parity;
	BOOLEAN	StopBits;
	BOOLEAN	HandshakingProtocol;
	BOOLEAN	Duplex;

	BOOLEAN	xONxOFF;
	BOOLEAN	HighSpeed;
	BOOLEAN	Shared;
	BOOLEAN	StripBit8;
	BOOLEAN	CheckCarrier;
	BOOLEAN	PassThrough;
};

struct OldModemSettings
{
	UBYTE	ModemInit[80];
	UBYTE 	ModemExit[80];
	UBYTE 	ModemHangup[80];
	UBYTE	DialPrefix[80];
	UBYTE	DialSuffix[80];

	UBYTE	NoCarrier[16];
	UBYTE	NoDialTone[16];
	UBYTE	Connect[16];
	UBYTE	Voice[16];
	UBYTE	Ring[16];
	UBYTE	Busy[16];

	LONG	RedialDelay;
	LONG	DialRetries;
	LONG	DialTimeout;
	BOOLEAN	ConnectAutoBaud;
	BOOLEAN	DropDTR;
};

struct OldCommandSettings
{
	UBYTE	StartupMacro[256];
	UBYTE	LogoffMacro[256];
	UBYTE	UploadMacro[256];
	UBYTE	DownloadMacro[256];
};

struct OldScreenSettings
{
	ULONG	DisplayMode;
	WORD	ColourMode;
	UWORD	Colours[16];
	UBYTE	FontName[256];
	WORD	FontHeight;

	BOOLEAN	MakeScreenPublic;
	BOOLEAN	ShanghaiWindows;

	BOOLEAN	Blinking;
	BOOLEAN	FasterLayout;
	BOOLEAN	TitleBar;
	BOOLEAN	StatusLine;
};

struct OldTerminalSettings
{
	BOOLEAN	BellMode;
	BOOLEAN	AlertMode;
	BOOLEAN	EmulationMode;
	BOOLEAN	FontMode;

	BOOLEAN	SendCR;
	BOOLEAN	SendLF;
	BOOLEAN	ReceiveCR;
	BOOLEAN	ReceiveLF;

	UWORD	NumColumns,
			NumLines;

	UBYTE	KeyMapFileName[256];
	UBYTE	EmulationFileName[256];
	UBYTE	BeepFileName[256];

	UBYTE	TextFontName[256];
	WORD	TextFontHeight;
};

struct OldPathSettings
{
	UBYTE	ASCIIUploadPath[256];
	UBYTE	ASCIIDownloadPath[256];

	UBYTE	TextUploadPath[256];
	UBYTE	TextDownloadPath[256];

	UBYTE	BinaryUploadPath[256];
	UBYTE	BinaryDownloadPath[256];

	UBYTE	DefaultStorage[256];

	UBYTE	Editor[256];
};

struct OldMiscSettings
{
	BOOLEAN	Priority;
	BOOLEAN	BackupConfig;

	BOOLEAN	OpenFastMacroPanel;
	BOOLEAN	ReleaseDevice;

	BOOLEAN	TransferServer;
	BOOLEAN	EmulationServer;

	BOOLEAN	OverridePath;
	BOOLEAN	AutoUpload;
	BOOLEAN	SetArchivedBit;
	BOOLEAN	IdentifyFiles;
};

struct OldClipSettings
{
	WORD	ClipboardUnit;

	WORD	LineDelay,
			CharDelay;

	UBYTE	InsertPrefix[80];
	UBYTE	InsertSuffix[80];
};

struct OldCaptureSettings
{
	BOOLEAN	LogActions;
	BOOLEAN	LogCall;
	UBYTE	LogFileName[256];

	LONG	MaxBufferSize;
	BOOLEAN	BufferEnabled;

	BOOLEAN	ConnectAutoCapture;
	BOOLEAN	CaptureFilterMode;
	UBYTE	CapturePath[256];
};

struct OldFileSettings
{
	UBYTE	ProtocolFileName[256];
	UBYTE	TranslationFileName[256];
	UBYTE	MacroFileName[256];
	UBYTE	CursorFileName[256];
	UBYTE	FastMacroFileName[256];
};

struct OldEmulationSettings
{
	BOOLEAN	CursorMode;
	BOOLEAN	NumericMode;

	BOOLEAN	CursorWrap;
	BOOLEAN	LineWrap;

	BOOLEAN	InsertMode;
	BOOLEAN	NewLineMode;

	BOOLEAN	FontScale;
	BOOLEAN	ScrollMode;
	BOOLEAN	DestructiveBackspace;
	BOOLEAN	SwapBSDelete;
	BOOLEAN	PrinterEnabled;
	BOOLEAN	Pad;

	UBYTE	AnswerBack[80];
};

struct OldConfiguration
{
	struct OldSerialSettings	SerialConfig;
	struct OldModemSettings		ModemConfig;
	struct OldScreenSettings	ScreenConfig;
	struct OldTerminalSettings	TerminalConfig;
	struct OldEmulationSettings	EmulationConfig;
	struct OldClipSettings		ClipConfig;
	struct OldCaptureSettings	CaptureConfig;
	struct OldCommandSettings	CommandConfig;
	struct OldMiscSettings		MiscConfig;
	struct OldPathSettings		PathConfig;
	struct OldFileSettings		FileConfig;
};

struct OldPhoneHeader
{
	UBYTE	Name[40],
			Number[60],
			Comment[40];

	UBYTE	UserName[30],
			Password[30];
};

	/* ConvertHeader():
	 *
	 *	Convert old style phone book header to new style
	 *	phone book header.
	 */

STATIC VOID
ConvertHeader(struct OldPhoneHeader *OldHeader,struct PhoneHeader *NewHeader)
{
	strcpy(NewHeader->Name,		OldHeader->Name);
	strcpy(NewHeader->Number,	OldHeader->Number);
	strcpy(NewHeader->Comment,	OldHeader->Comment);

	strcpy(NewHeader->UserName,	OldHeader->UserName);
	strcpy(NewHeader->Password,	OldHeader->Password);
}

	/* ConvertConfig():
	 *
	 *	Convert old style configuration to new style configuration.
	 */

STATIC BOOL
ConvertConfig(struct OldConfiguration *OldConfig,struct Configuration *NewConfig,BOOL IsPhonebook)
{
	if(CreateConfigEntry(NewConfig,PREF_SERIAL))
	{
		ResetConfigEntry(NewConfig,PREF_SERIAL);

		CopyMem(&OldConfig->SerialConfig,NewConfig->SerialConfig,MIN(sizeof(struct SerialSettings),sizeof(struct OldSerialSettings)));

		FixOldConfig(NewConfig,PREF_SERIAL,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_MODEM))
	{
		ResetConfigEntry(NewConfig,PREF_MODEM);

		CopyMem(&OldConfig->ModemConfig,NewConfig->ModemConfig,MIN(sizeof(struct ModemSettings),sizeof(struct OldModemSettings)));

		FixOldConfig(NewConfig,PREF_MODEM,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_COMMAND))
	{
		ResetConfigEntry(NewConfig,PREF_COMMAND);

		CopyMem(&OldConfig->CommandConfig,NewConfig->CommandConfig,MIN(sizeof(struct CommandSettings),sizeof(struct OldCommandSettings)));

		FixOldConfig(NewConfig,PREF_COMMAND,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_SCREEN))
	{
		ResetConfigEntry(NewConfig,PREF_SCREEN);

		CopyMem(&OldConfig->ScreenConfig,NewConfig->ScreenConfig,MIN(sizeof(struct ScreenSettings),sizeof(struct OldScreenSettings)));

		FixOldConfig(NewConfig,PREF_SCREEN,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_TERMINAL))
	{
		ResetConfigEntry(NewConfig,PREF_TERMINAL);

		CopyMem(&OldConfig->TerminalConfig,NewConfig->TerminalConfig,MIN(sizeof(struct TerminalSettings),sizeof(struct OldTerminalSettings)));

		FixOldConfig(NewConfig,PREF_TERMINAL,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_PATH))
	{
		ResetConfigEntry(NewConfig,PREF_PATH);

		CopyMem(&OldConfig->PathConfig,NewConfig->PathConfig,MIN(sizeof(struct PathSettings),sizeof(struct OldPathSettings)));

		FixOldConfig(NewConfig,PREF_PATH,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_MISC))
	{
		ResetConfigEntry(NewConfig,PREF_MISC);

		CopyMem(&OldConfig->MiscConfig,NewConfig->MiscConfig,MIN(sizeof(struct MiscSettings),sizeof(struct OldMiscSettings)));

		FixOldConfig(NewConfig,PREF_MISC,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_CLIP))
	{
		ResetConfigEntry(NewConfig,PREF_CLIP);

		CopyMem(&OldConfig->ClipConfig,NewConfig->ClipConfig,MIN(sizeof(struct ClipSettings),sizeof(struct OldClipSettings)));

		FixOldConfig(NewConfig,PREF_CLIP,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_CAPTURE))
	{
		ResetConfigEntry(NewConfig,PREF_CAPTURE);

		CopyMem(&OldConfig->CaptureConfig,NewConfig->CaptureConfig,MIN(sizeof(struct CaptureSettings),sizeof(struct OldCaptureSettings)));

		strcpy(NewConfig->CaptureConfig->CallLogFileName,NewConfig->CaptureConfig->LogFileName);

		AddPart(NewConfig->CaptureConfig->CallLogFileName,"term-call.log",sizeof(NewConfig->CaptureConfig->CallLogFileName));

		FixOldConfig(NewConfig,PREF_CAPTURE,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_FILE))
	{
		LONG i,Error = 0;

		ResetConfigEntry(NewConfig,PREF_FILE);

		CopyMem(&OldConfig->FileConfig,NewConfig->FileConfig,MIN(sizeof(struct FileSettings),sizeof(struct OldFileSettings)));

		FixOldConfig(NewConfig,PREF_FILE,IsPhonebook,2,4);

		for(i = PREF_TRANSLATIONFILENAME ; i <= PREF_FASTMACROFILENAME ; i++)
		{
			if(!CreateConfigEntry(NewConfig,i))
			{
				Error = ERROR_NO_FREE_STORE;
				break;
			}
		}

		if(!Error)
		{
			strcpy(NewConfig->TranslationFileName,	NewConfig->FileConfig->TranslationFileName);
			strcpy(NewConfig->MacroFileName,		NewConfig->FileConfig->MacroFileName);
			strcpy(NewConfig->CursorFileName,		NewConfig->FileConfig->CursorFileName);
			strcpy(NewConfig->FastMacroFileName,	NewConfig->FileConfig->FastMacroFileName);

			DeleteConfigEntry(NewConfig,PREF_FILE);
		}
		else
		{
			SetIoErr(Error);
			return(FALSE);
		}
	}
	else
		return(FALSE);

	if(CreateConfigEntry(NewConfig,PREF_EMULATION))
	{
		ResetConfigEntry(NewConfig,PREF_EMULATION);

		CopyMem(&OldConfig->EmulationConfig,NewConfig->EmulationConfig,MIN(sizeof(struct EmulationSettings),sizeof(struct OldEmulationSettings)));

		FixOldConfig(NewConfig,PREF_EMULATION,IsPhonebook,2,4);
	}
	else
		return(FALSE);

	FinalFix(NewConfig,IsPhonebook,2,4);

	return(TRUE);
}

	/* LoadOldPhonebook(STRPTR Name):
	 *
	 *	Restore a phone book from a disk file.
	 */

PhonebookHandle *
LoadOldPhonebook(STRPTR Name)
{
	STATIC ULONG Stops[6 * 2] =
	{
		ID_TERM,ID_VERS,
		ID_TERM,ID_DIAL,
		ID_TERM,ID_PREF,
		ID_TERM,ID_DATE,
		ID_TERM,ID_PHON,
		ID_TERM,ID_PSWD
	};

	PhonebookHandle			*PhoneHandle = NULL;
	struct IFFHandle		*Handle;
	struct ContextNode		*Chunk;
	struct TimeDateNode		*TimeDateNode;
	struct OldConfiguration	*OldConfig;
	struct OldPhoneHeader	*OldHeader;
	LONG					 Error;

	if(OldConfig = (struct OldConfiguration *)AllocVecPooled(sizeof(struct OldConfiguration) + sizeof(struct OldPhoneHeader),MEMF_ANY))
	{
		OldHeader = (struct OldPhoneHeader *)(OldConfig + 1);

		if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
		{
			if(!(Error = StopChunks(Handle,(LONG *)Stops,6)))
			{
				LONG Size = 0,Count = 0,Last = -1;

				while(!ParseIFF(Handle,IFFPARSE_SCAN))
				{
					Chunk = CurrentChunk(Handle);

					if(Chunk->cn_ID == ID_VERS)
					{
						struct TermInfo TermInfo;

						if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) == sizeof(struct TermInfo))
						{
							if(TermInfo.Version != 2 || TermInfo.Revision != 4)
							{
								Error = ERR_OUTDATED;

								break;
							}
						}
						else
						{
							Error = IoErr();

							break;
						}
					}

					if(Chunk->cn_ID == ID_PSWD)
					{
						UBYTE PasswordBuffer[MAX_PASSWORD_LENGTH];

						if(ReadChunkBytes(Handle,PasswordBuffer,MAX_PASSWORD_LENGTH) == MAX_PASSWORD_LENGTH)
						{
							BOOL AskPassword = TRUE;

							if(GlobalPhoneHandle->PhonePasswordUsed)
							{
								if(!memcmp(GlobalPhoneHandle->PhonePassword,PasswordBuffer,MAX_PASSWORD_LENGTH))
								{
									AskPassword = FALSE;

									CopyMem(GlobalPhoneHandle->PhonePassword,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);
									PhoneHandle->PhonePasswordUsed = TRUE;
								}
							}

							if(AskPassword)
							{
								UBYTE LocalBuffer[MAX_PASSWORD_LENGTH+1];
								BOOL Ok = FALSE;

								memset(LocalBuffer,0,sizeof(LocalBuffer));

								if(GetString(FALSE,TRUE,MAX_PASSWORD_LENGTH+1,LocaleString(MSG_PHONEPANEL_PLEASE_ENTER_PASSWORD_TXT),LocalBuffer))
								{
									UBYTE AnotherBuffer[MAX_PASSWORD_LENGTH];

									Encrypt(LocalBuffer,MAX_PASSWORD_LENGTH,AnotherBuffer,LocalBuffer,strlen(LocalBuffer));

									if(!memcmp(PasswordBuffer,AnotherBuffer,MAX_PASSWORD_LENGTH))
									{
										CopyMem(PasswordBuffer,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);

										PhoneHandle->PhonePasswordUsed = TRUE;

										Ok = TRUE;
									}
								}

								if(!Ok)
								{
									ShowRequest(Window,LocaleString(MSG_TERMPHONE_WRONG_PASSWORD_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Name);

									break;
								}
							}
						}
						else
						{
							Error = IoErr();

							break;
						}
					}

					if(Chunk->cn_ID == ID_DIAL)
					{
						if(ReadChunkBytes(Handle,&Size,sizeof(LONG)) == sizeof(LONG))
						{
							if(!(PhoneHandle = CreatePhonebook(Size,TRUE)))
								break;
						}
						else
						{
							Error = IoErr();

							break;
						}
					}

					if(Chunk->cn_ID == ID_PHON)
					{
						LONG LocalSize = MIN(sizeof(struct OldPhoneHeader),Chunk->cn_Size);

						if(ReadChunkBytes(Handle,OldHeader,LocalSize) == LocalSize)
						{
							if(PhoneHandle->PhonePasswordUsed)
								Decrypt((UBYTE *)OldHeader,LocalSize,(UBYTE *)OldHeader,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);
						}
						else
						{
							Error = IoErr();

							break;
						}
					}

					if(Chunk->cn_ID == ID_PREF)
					{
						if(Count < Size)
						{
							LONG LastSize = MIN(Chunk->cn_Size,sizeof(struct OldConfiguration));

							if(ReadChunkBytes(Handle,OldConfig,LastSize) == LastSize)
							{
								if(PhoneHandle->PhonePasswordUsed)
									Decrypt((UBYTE *)OldConfig,LastSize,(UBYTE *)OldConfig,PhoneHandle->PhonePassword,MAX_PASSWORD_LENGTH);

								if(ConvertConfig(OldConfig,PhoneHandle->Phonebook[Count]->Config,TRUE))
								{
									memset(PhoneHandle->Phonebook[Count]->Header,0,sizeof(PhoneHeader));

									ConvertHeader(OldHeader,PhoneHandle->Phonebook[Count]->Header);

									Last = Count;

									PhoneHandle->Phonebook[Count]->Count = -1;

									Count++;
								}
								else
								{
									if(Count)
										Size = Count - 1;
									else
										Size = 0;

									break;
								}
							}
							else
							{
								Error = IoErr();

								break;
							}
						}
					}

					if(Chunk->cn_ID == ID_DATE)
					{
						if(Last != -1)
						{
							LONG LocalCount = (Chunk->cn_Size - sizeof(struct TimeDateHeader)) / sizeof(struct TimeDateOld);

							if(TimeDateNode = CreateTimeDateNode(-1,-1,"",LocalCount))
							{
								struct TimeDateOld *Old;

								if(Old = (struct TimeDateOld *)AllocVecPooled(sizeof(struct TimeDateOld) * LocalCount,MEMF_ANY))
								{
									if(ReadChunkBytes(Handle,&TimeDateNode->Header,sizeof(struct TimeDateHeader)) == sizeof(struct TimeDateHeader))
									{
										if(ReadChunkBytes(Handle,Old,sizeof(struct TimeDateOld) * LocalCount) == sizeof(struct TimeDateOld) * LocalCount)
										{
											LONG i;

											for(i = 0 ; i < LocalCount ; i++)
												ConvertTimeDate(&Old[i],&TimeDateNode->Table[i]);

											AdaptTimeDateNode(TimeDateNode);

											AddTail((struct List *)&PhoneHandle->Phonebook[Last]->TimeDateList,&TimeDateNode->Node);

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
						}
					}
				}

				if(!PhoneHandle->Phonebook[PhoneHandle->NumPhoneEntries - 1])
					Error = ERR_LOAD_ERROR;

				if(!Error)
				{
					PhoneEntry **Phonebook;
					struct timeval Now;
					LONG i;

					GetSysTime(&Now);

					PhoneHandle->PhonebookID		= Now.tv_secs;
					PhoneHandle->NumPhoneEntries	= Size;

					Phonebook = PhoneHandle->Phonebook;

					for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
					{
						Phonebook[i]->Header->ID = PhoneHandle->PhonebookID++;
						StripGlobals(Phonebook[i]->Config);
					}
				}
			}

			CloseIFFStream(Handle);
		}
		else
			Error = IoErr();

		FreeVecPooled(OldConfig);
	}
	else
		Error = ERROR_NO_FREE_STORE;

	if(Error)
	{
		DeletePhonebook(PhoneHandle);

		SetIoErr(Error);

		return(NULL);
	}
	else
		return(PhoneHandle);
}

	/* ReadConfig(STRPTR Name,struct Configuration *Config):
	 *
	 *	Read the configuration file, very much the same as ReadIFFData().
	 */

BOOL
ReadOldConfig(STRPTR Name,struct Configuration *LocalConfig)
{
	struct IFFHandle		*Handle;
	struct StoredProperty	*Prop;
	struct TermInfo			*TermInfo;
	struct OldConfiguration	*OldConfig;
	LONG					 Error;

	if(OldConfig = (struct OldConfiguration *)AllocVecPooled(sizeof(struct OldConfiguration),MEMF_ANY))
	{
		if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
		{
			if(!(Error = PropChunks(Handle,(LONG *)VersionProps,1)))
			{
				if(!(Error = StopChunk(Handle,ID_TERM,ID_PREF)))
				{
					if(!ParseIFF(Handle,IFFPARSE_SCAN))
					{
						if(Prop = FindProp(Handle,ID_TERM,ID_VERS))
						{
							TermInfo = (struct TermInfo *)Prop->sp_Data;

							if(TermInfo->Version == 2 && TermInfo->Revision == 4)
							{
								struct ContextNode *Chunk = CurrentChunk(Handle);

								if(ReadChunkRecords(Handle,OldConfig,MIN(Chunk->cn_Size,sizeof(struct OldConfiguration)),1) == 1)
								{
									if(!ConvertConfig(OldConfig,LocalConfig,FALSE))
										Error = IoErr();
								}
								else
									Error = IoErr();
							}
							else
								Error = ERR_OUTDATED;
						}
					}
				}
			}

			CloseIFFStream(Handle);
		}
		else
			Error = IoErr();

		FreeVecPooled(OldConfig);
	}
	else
		Error = ERROR_NO_FREE_STORE;

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}
