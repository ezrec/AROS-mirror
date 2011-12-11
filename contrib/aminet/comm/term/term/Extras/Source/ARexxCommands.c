/*
**	ARexxCommands.c
**
**	ARexx interface command support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Cheapo shortcuts */

#define Args		Pkt->Array
#define ResultCode	Pkt->Results

	/* ScanNodeFilter(UBYTE *Data,LONG Size,struct List *List,UBYTE Mask,UBYTE **DataPtr):
	 *
	 *	Scans a data buffer for matches with the names of the list nodes. It
	 *	will return the matching node name and place the address where the
	 *	matching node name was found in the input buffer in DataPtr. If nothing
	 *	is found, NULL will be returned.
	 */

STATIC STRPTR
ScanNodeFilter(UBYTE *Data,LONG Size,struct List *List,UBYTE Mask,UBYTE **DataPtr)
{
	STRPTR Result;
	UBYTE Char;

		/* This is where the result of the search will be placed. */

	Result = NULL;

		/* Run down the input string. */

	while(Size--)
	{
			/* Get the next character, promote it to upper case
			 * and strip off the high order bit if necessary.
			 */

		if(Char = ToUpper((*Data++) & Mask))
		{
			struct WaitNode *Node;
			UBYTE MatchChar;

				/* Try to find the nodes in which the next character of the
				 * name matches the character we just picked from the string.
				 */

			for(Node = (struct WaitNode *)List->lh_Head ; Node->Node.ln_Succ ; Node = (struct WaitNode *)Node->Node.ln_Succ)
			{
					/* This is the next character to match in the node name. */

				MatchChar = ToUpper(Node->Node.ln_Name[Node->Count]) & Mask;

					/* If it doesn't match the character we just got,
					 * restart from the beginning of the node name.
					 */

				if(Char != MatchChar)
				{
					Node->Count = 0;

					MatchChar = ToUpper(Node->Node.ln_Name[0]) & Mask;
				}

					/* Does the character match? */

				if(Char == MatchChar)
				{
						/* Increase the number of matching characters. */

					Node->Count++;

						/* Does the entire name match? */

					if(!Node->Node.ln_Name[Node->Count])
					{
							/* If there is a response string
							 * attached, send it. Otherwise
							 * remember the matching string.
							 */

						if(Node->ResponseLen)
							SerWriteVerbatim(Node->Response,Node->ResponseLen,FALSE);
						else
							Result = Node->Node.ln_Name;

						Node->Count = 0;
					}
				}
			}

				/* If we got something, return now. */

			if(Result != NULL)
			{
				*DataPtr = Data;

				return(Result);
			}
		}
	}

		/* Couldn't find anything. */

	return(NULL);
}

STRPTR
RexxWait(struct RexxPkt *Pkt)
{
	enum	{	ARG_WAIT_NOECHO,ARG_TIMEOUT,ARG_WAIT_TEXT };

	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	struct List LocalList,*MatchList;
	struct WaitNode LocalWaitNode;
	ULONG Signals;
	STRPTR Result;
	LONG Timeout;
	UBYTE Mask;
	BOOL Echo;
	BOOL Done;

	if(!ReadRequest || !WriteRequest)
	{
		ResultCode[0] = RC_WARN;

		return(NULL);
	}

	if(Args[ARG_WAIT_NOECHO])
		Echo = FALSE;
	else
		Echo = TRUE;

	if(Args[ARG_WAIT_TEXT])
	{
		LONG Len;

		NewList(&LocalList);

		CopyMem(Args[ARG_WAIT_TEXT],LocalBuffer,sizeof(LocalBuffer) - 1);
		LocalBuffer[sizeof(LocalBuffer) - 1] = 0;

		Len = TranslateString(LocalBuffer,LocalBuffer);
		LocalBuffer[Len] = 0;

		memset(&LocalWaitNode,0,sizeof(struct WaitNode));

		LocalWaitNode.Node.ln_Name = LocalBuffer;

		AddTail(MatchList = &LocalList,(struct Node *)&LocalWaitNode);
	}
	else
	{
		if(!GenericListCount(GenericListTable[GLIST_WAIT]))
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = TERMERROR_LIST_IS_ALREADY_EMPTY;

			return(NULL);
		}
		else
		{
			struct WaitNode *Node;

			MatchList = (struct List *)GenericListTable[GLIST_WAIT];

			LockGenericList((struct GenericList *)MatchList);

			for(Node = (struct WaitNode *)MatchList->lh_Head ; Node->Node.ln_Succ ; Node = (struct WaitNode *)Node->Node.ln_Succ)
				Node->Count = 0;
		}
	}

	if(Config->SerialConfig->StripBit8)
		Mask = 0x7F;
	else
		Mask = 0xFF;

	if(Args[ARG_TIMEOUT])
		Timeout = *(LONG *)Args[ARG_TIMEOUT];
	else
		Timeout = RexxTimeoutVal;

	if(Timeout)
		StartTime(Timeout,0);

	Lock_xOFF();
	Clear_xOFF();

	Result = NULL;
	Done = FALSE;

	do
	{
		Signals = (*SerialWaitForData)(SIG_WINDOW | SIG_BREAK | SIG_TIMER);

		if(Signals & SIG_WINDOW)
			while(RunJob(WindowJob));

		if(Signals & SIG_SERIAL)
		{
			ULONG Length;

			if(Length = (*SerialGetWaiting)())
			{
				if(Length > SerialBufferSize / 2)
					Length = SerialBufferSize / 2;

				if(Length > Config->SerialConfig->Quantum)
					Length = Config->SerialConfig->Quantum;

				if(Length = (*SerialRead)(ReadBuffer,Length))
				{
					BytesIn += Length;

					if(Translate_CR_LF)
						Length = (*Translate_CR_LF)(ReadBuffer,Length);

					if(Length)
					{
						UBYTE *NewData;

						if(Result = ScanNodeFilter(ReadBuffer,Length,MatchList,Mask,&NewData))
						{
							ULONG NewLength;

							NewLength = (IPTR)NewData - (IPTR)ReadBuffer;

							if(NewLength + strlen(Result) != Length)
							{
								DataHold = NewData;
								DataSize = Length - NewLength;

								UpdateSerialJob();
							}

							Length = NewLength;

							Done = TRUE;
						}

						if(Echo && Length)
							ConProcess(ReadBuffer,Length);
					}
				}
			}
		}

		if(Signals & (SIG_BREAK | SIG_TIMER))
		{
			if(!Result)
				ResultCode[0] = RC_WARN;

			Done = TRUE;
		}
	}
	while(!Done);

	if(MatchList == (struct List *)GenericListTable[GLIST_WAIT])
		UnlockGenericList((struct GenericList *)MatchList);

	Unlock_xOFF();

	StopTime();

	if(Result)
		return(CreateResult(Result,ResultCode));
	else
		return(NULL);
}

STATIC BOOL
ExamineBeforeSending(STRPTR FileName,struct FileTransferInfo *Info,LONG *Results)
{
	BPTR FileLock;
	BOOL Result;

	Result = FALSE;

	if(FileLock = Lock(FileName,ACCESS_READ))
	{
		D_S(struct FileInfoBlock,FileInfo);

		if(Examine(FileLock,FileInfo))
		{
			if(FileInfo->fib_DirEntryType < 0)
			{
				UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

				if(NameFromLock(FileLock,LocalBuffer,sizeof(LocalBuffer)))
				{
					if(AddFileTransferNode(Info,LocalBuffer,FileInfo->fib_Size))
						Result = TRUE;
					else
					{
						Results[0] = RC_ERROR;
						Results[1] = ERROR_NO_FREE_STORE;
					}
				}
				else
				{
					Results[0] = RC_ERROR;
					Results[1] = IoErr();
				}
			}
			else
			{
				Results[0] = RC_ERROR;
				Results[1] = ERROR_OBJECT_WRONG_TYPE;
			}
		}
		else
		{
			Results[0] = RC_ERROR;
			Results[1] = IoErr();
		}

		UnLock(FileLock);
	}
	else
	{
		Results[0] = RC_ERROR;
		Results[1] = IoErr();
	}

	return(Result);
}

STATIC BPTR
ChangeDirBeforeSending(LONG Mode)
{
	BPTR NewDir = BNULL;

	switch(Mode)
	{
		case TRANSFER_BINARY:

			if(Config->PathConfig->BinaryUploadPath[0])
				NewDir = Lock(Config->PathConfig->BinaryUploadPath,SHARED_LOCK);

			break;

		case TRANSFER_TEXT:

			if(Config->PathConfig->TextUploadPath[0])
				NewDir = Lock(Config->PathConfig->TextUploadPath,SHARED_LOCK);

			break;

		case TRANSFER_ASCII:

			if(Config->PathConfig->ASCIIUploadPath[0])
				NewDir = Lock(Config->PathConfig->ASCIIUploadPath,SHARED_LOCK);

			break;
	}

	return(NewDir);
}

STRPTR
RexxSendFile(struct RexxPkt *Pkt)
{
	enum	{	ARG_SENDFILE_MODE,ARG_SENDFILE_NAMES };

	LONG Mode = TRANSFER_BINARY;

	if(Args[ARG_SENDFILE_MODE])
		Mode = ToMode(Args[ARG_SENDFILE_MODE]);

	if(Mode == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_ACTION_NOT_KNOWN;
	}
	else
	{
		struct FileTransferInfo	*Info;
		LONG FilesFound = 0;

		ResultCode[0] = RC_OK;

		if(Info = AllocFileTransferInfo())
		{
			struct GenericList *List = GenericListTable[GLIST_UPLOAD];
			BPTR NewDir,OldDir;

			if(Args[ARG_SENDFILE_NAMES])
			{
				STRPTR *Names;

				Names = (STRPTR *)Args[ARG_SENDFILE_NAMES];

				if(NewDir = ChangeDirBeforeSending(Mode))
					OldDir = CurrentDir(NewDir);

				while(*Names && ResultCode[0] == RC_OK)
				{
					if(ExamineBeforeSending(*Names++,Info,ResultCode))
						FilesFound++;
				}

				if(NewDir)
					UnLock(CurrentDir(OldDir));
			}

			LockGenericList(List);

			if(GenericListCount(List) > 0 && ResultCode[0] == RC_OK)
			{
				struct Node	*Node;

				if(NewDir = ChangeDirBeforeSending(Mode))
					OldDir = CurrentDir(NewDir);

				for(Node = (struct Node *)List->ListHeader.mlh_Head ; Node->ln_Succ && ResultCode[0] == RC_OK ; Node = Node->ln_Succ)
				{
					if(ExamineBeforeSending(Node->ln_Name,Info,ResultCode))
						FilesFound++;
				}

				if(NewDir)
					UnLock(CurrentDir(OldDir));
			}

			UnlockGenericList(List);

			if(ResultCode[0] == RC_OK)
			{
				BlockWindows();

				if(FilesFound)
				{
					ReadyFileTransferInfo(Info,TRUE);

					FileTransferInfo = Info;
				}

				switch(Mode)
				{
					case TRANSFER_ASCII:

						if(ChangeProtocol(Config->TransferConfig->ASCIIUploadLibrary,Config->TransferConfig->ASCIIUploadType))
						{
							if(FilesFound)
								StartSendXPR_FromList(TRANSFER_ASCII,FALSE);
							else
								StartSendXPR_AskForFile(TRANSFER_ASCII,FALSE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}

						ResetProtocol();

						break;

					case TRANSFER_TEXT:

						if(ChangeProtocol(Config->TransferConfig->TextUploadLibrary,Config->TransferConfig->TextUploadType))
						{
							if(FilesFound)
								StartSendXPR_FromList(TRANSFER_TEXT,FALSE);
							else
								StartSendXPR_AskForFile(TRANSFER_TEXT,FALSE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}

						ResetProtocol();

						break;

					case TRANSFER_BINARY:

						if(ChangeProtocol(Config->TransferConfig->BinaryUploadLibrary,Config->TransferConfig->BinaryUploadType))
						{
							if(FilesFound)
								StartSendXPR_FromList(TRANSFER_BINARY,FALSE);
							else
								StartSendXPR_AskForFile(TRANSFER_BINARY,FALSE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}

						ResetProtocol();

						break;
				}

				if(TransferFailed)
					ResultCode[0] = RC_ERROR;
				else
				{
					if(TransferAborted)
						ResultCode[0] = RC_WARN;
				}

				ReleaseWindows();
			}
			else
				FreeFileTransferInfo(Info);
		}
		else
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = ERROR_NO_FREE_STORE;
		}
	}

	return(NULL);
}

STRPTR
RexxSelect(struct RexxPkt *Pkt)
{
	enum	{	ARG_SELECT_NAME,ARG_SELECT_FROM,ARG_SELECT_NEXT,ARG_SELECT_PREVIOUS,
				ARG_SELECT_TOP,ARG_SELECT_BOTTOM
			};

	LONG Index;

	if((Index = ToList(Args[ARG_SELECT_FROM])) == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_LIST;
	}
	else
	{
		if(Args[ARG_SELECT_NAME] && Index == GLIST_DIAL)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = TERMERROR_DATA_TYPES_INCOMPATIBLE;
		}
		else
		{
			struct GenericList *List;
			STRPTR Result;

			List = GenericListTable[Index];

			if(Args[ARG_SELECT_NAME])
			{
				STRPTR Buffer;

				if(Buffer = CreateMatchBuffer(Args[ARG_SELECT_NAME]))
				{
					struct Node *Node;
					BOOL GotIt;

					LockGenericList(List);

					for(Node = (struct Node *)List->ListHeader.mlh_Head, GotIt = FALSE; !GotIt && Node->ln_Succ ; Node = Node->ln_Succ)
					{
						if(MatchBuffer(Buffer,Node->ln_Name))
						{
							List->ListNode = Node;

							GotIt = TRUE;
						}
					}

					if(!GotIt)
						ResultCode[0] = RC_WARN;

					UnlockGenericList(List);

					DeleteMatchBuffer(Buffer);
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_NO_FREE_STORE;
				}
			}
			else
			{
				if(Args[ARG_SELECT_NEXT])
				{
					if(!NextGenericListNode(List))
					{
						ResultCode[0] = RC_WARN;

						return(NULL);
					}
				}

				if(Args[ARG_SELECT_PREVIOUS])
				{
					if(!PrevGenericListNode(List))
					{
						ResultCode[0] = RC_WARN;

						return(NULL);
					}
				}

				if(Args[ARG_SELECT_TOP])
				{
					if(!FirstGenericListNode(List))
					{
						ResultCode[0] = RC_WARN;

						return(NULL);
					}
				}

				if(Args[ARG_SELECT_BOTTOM])
				{
					if(!LastGenericListNode(List))
					{
						ResultCode[0] = RC_WARN;

						return(NULL);
					}
				}
			}

			SharedLockGenericList(List);

			if(List->ListNode)
				Result = CreateResult(((struct Node *)List->ListNode)->ln_Name,ResultCode);
			else
			{
				ResultCode[0] = RC_WARN;

				Result = NULL;
			}

			UnlockGenericList(List);

			return(Result);
		}
	}

	return(NULL);
}

STRPTR
RexxSaveAs(struct RexxPkt *Pkt)
{
	enum	{	ARG_SAVEAS_NAME,ARG_SAVEAS_FROM };

	LONG Index = ToConfig(Args[ARG_SAVEAS_FROM]);

	if(Index == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_OBJECT_NOT_FOUND;
	}
	else
	{
		UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
		STRPTR FileName;

		if(Args[ARG_SAVEAS_NAME])
			FileName = Args[ARG_SAVEAS_NAME];
		else
		{
			struct FileRequester *FileRequest;
			LONG TitleID = 0;

			FileName = NULL;

			switch(Index)
			{
				case DATATYPE_TRANSLATIONS:

					TitleID = MSG_TRANSLATIONPANEL_SAVE_TRANSLATION_TABLES_TXT;
					break;

				case DATATYPE_FUNCTIONKEYS:

					TitleID = MSG_MACROPANEL_SAVE_MACRO_KEYS_TXT;
					break;

				case DATATYPE_CURSORKEYS:

					TitleID = MSG_CURSORPANEL_SAVE_CURSOR_KEYS_TXT;
					break;

				case DATATYPE_FASTMACROS:

					TitleID = MSG_FASTMACROPANEL_SAVE_FAST_MACRO_SETTINGS_TXT;
					break;

				case DATATYPE_HOTKEYS:

					TitleID = MSG_HOTKEYPANEL_SAVE_HOTKEYS_TXT;
					break;

				case DATATYPE_SPEECH:

					TitleID = MSG_SPEECHPANEL_SAVE_SPEECH_SETTINGS_TXT;
					break;

				case DATATYPE_SOUND:

					TitleID = MSG_SOUNDPANEL_SAVE_SOUNDS_TXT;
					break;

				case DATATYPE_BUFFER:

					TitleID = MSG_TERMMAIN_SAVE_BUFFER_TXT;
					break;

				case DATATYPE_CONFIGURATION:

					TitleID = MSG_TERMMAIN_SAVE_PREFERENCES_AS_TXT;
					break;

				case DATATYPE_PHONEBOOK:

					TitleID = MSG_PHONEPANEL_SAVE_PHONEBOOK_TXT;
					break;

				case DATATYPE_SCREENTEXT:

					TitleID = MSG_TERMMAIN_SAVE_SCREEN_ASCII_TXT;
					break;

				case DATATYPE_SCREENIMAGE:

					TitleID = MSG_TERMMAIN_SAVE_SCREEN_IFF_TXT;
					break;
			}

			BlockWindows();

			DummyBuffer[0] = 0;

			if(FileRequest = SaveFile(Window,LocaleString(TitleID),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				FileName = DummyBuffer;
			}

			ReleaseWindows();
		}

		if(FileName)
		{
			switch(Index)
			{
				case DATATYPE_TRANSLATIONS:

					if(SendTable && ReceiveTable)
					{
						if(!SaveTranslationTables(FileName,SendTable,ReceiveTable))
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = IoErr();
						}
						else
						{
							strcpy(LastTranslation,FileName);

							strcpy(Config->TranslationFileName,LastTranslation);

							TranslationChanged = FALSE;
						}
					}
					else
						ResultCode[0] = RC_WARN;

					break;

				case DATATYPE_FUNCTIONKEYS:

					if(!WriteIFFData(FileName,MacroKeys,sizeof(struct MacroKeys),ID_KEYS))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastMacros,FileName);

						strcpy(Config->MacroFileName,LastMacros);

						MacroChanged = FALSE;
					}

					break;

				case DATATYPE_CURSORKEYS:

					if(!WriteIFFData(FileName,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastCursorKeys,FileName);

						strcpy(Config->CursorFileName,LastCursorKeys);

						CursorKeysChanged = FALSE;
					}

					break;

				case DATATYPE_FASTMACROS:

					if(!SaveFastMacros(FileName,&FastMacroList))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastFastMacros,FileName);

						strcpy(Config->FastMacroFileName,LastFastMacros);

						FastMacrosChanged = FALSE;
					}

					break;

				case DATATYPE_HOTKEYS:

					if(!WriteIFFData(FileName,&Hotkeys,sizeof(struct Hotkeys),ID_HOTK))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastKeys,FileName);

						strcpy(Config->HotkeyFileName,LastKeys);

						HotkeysChanged = FALSE;
					}

					break;

				case DATATYPE_SPEECH:

					if(!WriteIFFData(FileName,&SpeechConfig,sizeof(struct SpeechConfig),ID_SPEK))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastSpeech,FileName);

						strcpy(Config->SpeechFileName,LastSpeech);

						SpeechChanged = FALSE;
					}

					break;

				case DATATYPE_SOUND:

					if(!WriteIFFData(FileName,&SoundConfig,sizeof(struct SoundConfig),ID_SOUN))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastSound,FileName);

						strcpy(Config->SoundFileName,LastSound);

						SoundChanged = FALSE;
					}

					break;

				case DATATYPE_BUFFER:

					if(BufferLines && Lines)
					{
						BPTR SomeFile;

						if(SomeFile = OpenToAppend(FileName,NULL))
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
									if(FWrite(SomeFile,BufferLines[i],Len,1) != 1)
									{
										ResultCode[0] = RC_ERROR;
										ResultCode[1] = IoErr();

										break;
									}
								}

								if(FPrintf(SomeFile,"\n") < 1)
								{
									ResultCode[0] = RC_ERROR;
									ResultCode[1] = IoErr();

									break;
								}
							}

							ReleaseSemaphore(&BufferSemaphore);

							Close(SomeFile);

							AddProtection(FileName,FIBF_EXECUTE);

							if(Config->MiscConfig->CreateIcons)
								AddIcon(FileName,FILETYPE_TEXT,TRUE);

							BufferChanged = FALSE;
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = IoErr();
						}
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = TERMERROR_NO_DATA_TO_PROCESS;
					}

					break;

				case DATATYPE_CONFIGURATION:

					if(!WriteConfig(FileName,Config))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastConfig,FileName);

						ConfigChanged = FALSE;
					}

					break;

				case DATATYPE_PHONEBOOK:

					if(!SavePhonebook(FileName,GlobalPhoneHandle))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastPhone,FileName);
						strcpy(Config->PhonebookFileName,LastPhone);

						PhonebookChanged = FALSE;
					}

					break;

				case DATATYPE_SCREENTEXT:

					if(RasterEnabled)
					{
						BPTR SomeFile;

						if(SomeFile = OpenToAppend(FileName,NULL))
						{
							LONG	 i,j;
							UBYTE	*Buffer;

							for(i = 0 ; i < RasterHeight ; i++)
							{
								Buffer = &Raster[i * RasterWidth];

								j = LastColumn;

								while(j >= 0 && Buffer[j] == ' ')
									j--;

								if(j >= 0)
								{
									if(!FWrite(SomeFile,Buffer,j + 1,1))
									{
										ResultCode[0] = RC_ERROR;
										ResultCode[1] = IoErr();

										break;
									}
								}

								if(!FWrite(SomeFile,"\n",1,1))
								{
									ResultCode[0] = RC_ERROR;
									ResultCode[1] = IoErr();

									break;
								}
							}

							Close(SomeFile);

							AddProtection(FileName,FIBF_EXECUTE);

							if(Config->MiscConfig->CreateIcons)
								AddIcon(FileName,FILETYPE_TEXT,TRUE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = IoErr();
						}
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = TERMERROR_NO_DATA_TO_PROCESS;
					}

					break;

				case DATATYPE_SCREENIMAGE:

					if(!SaveWindow(FileName,Window))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}

					break;
			}
		}
		else
			ResultCode[0] = RC_WARN;
	}

	return(NULL);
}

STRPTR
RexxRemove(struct RexxPkt *Pkt)
{
	enum	{	ARG_REMOVE_FROM,ARG_REMOVE_NAME };

	LONG Index;

	if((Index = ToList(Args[ARG_REMOVE_FROM])) == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_LIST;
	}
	else
	{
		if(Args[ARG_REMOVE_NAME] && Index == GLIST_DIAL)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = TERMERROR_DATA_TYPES_INCOMPATIBLE;
		}
		else
		{
			struct GenericList *List;

			List = GenericListTable[Index];

			if(Args[ARG_REMOVE_NAME])
			{
				STRPTR Buffer;

				if(Buffer = CreateMatchBuffer(Args[ARG_REMOVE_NAME]))
				{
					struct Node *Node,*Next;

					LockGenericList(List);

					for(Node = (struct Node *)List->ListHeader.mlh_Head ; Next = Node->ln_Succ ; Node = Next)
					{
						if(MatchBuffer(Buffer,Node->ln_Name))
						{
							Forbid();

							UnlockGenericList(List);

							DeleteGenericListNode(List,Node,TRUE);

							LockGenericList(List);

							Permit();
						}
					}

					UnlockGenericList(List);

					DeleteMatchBuffer(Buffer);
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_NO_FREE_STORE;
				}
			}
			else
				DeleteGenericListNode(List,NULL,TRUE);

			if(!GenericListCount(List))
				ResultCode[0] = RC_WARN;
		}
	}

	return(NULL);
}

STRPTR
RexxRequestFile(struct RexxPkt *Pkt)
{
	enum	{	ARG_REQUESTFILE_TITLE,ARG_REQUESTFILE_PATH,ARG_REQUESTFILE_FILE,
				ARG_REQUESTFILE_PATTERN,ARG_REQUESTFILE_MULTI,ARG_REQUESTFILE_NAME
			};

	if(Args[ARG_REQUESTFILE_MULTI] && !Args[ARG_REQUESTFILE_NAME])
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_RESULT_VARIABLE_REQUIRED;
	}
	else
	{
		UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
		struct FileRequester *FileRequester;

		if(Args[ARG_REQUESTFILE_PATH])
		{
			LimitedStrcpy(sizeof(DummyBuffer),DummyBuffer,Args[ARG_REQUESTFILE_PATH]);

			if(Args[ARG_REQUESTFILE_FILE])
				AddPart(DummyBuffer,Args[ARG_REQUESTFILE_FILE],sizeof(DummyBuffer));
		}
		else
		{
			if(Args[ARG_REQUESTFILE_FILE])
				LimitedStrcpy(sizeof(DummyBuffer),DummyBuffer,Args[ARG_REQUESTFILE_FILE]);
			else
				DummyBuffer[0] = 0;
		}

		BlockWindows();

		if(Args[ARG_REQUESTFILE_MULTI])
			FileRequester = OpenSeveralFiles(Window,Args[ARG_REQUESTFILE_TITLE],NULL,Args[ARG_REQUESTFILE_PATTERN],DummyBuffer,sizeof(DummyBuffer));
		else
			FileRequester = OpenSingleFile(Window,Args[ARG_REQUESTFILE_TITLE],NULL,Args[ARG_REQUESTFILE_PATTERN],DummyBuffer,sizeof(DummyBuffer));

		if(FileRequester)
		{
			if(Args[ARG_REQUESTFILE_NAME])
			{
				if(Args[ARG_REQUESTFILE_MULTI])
				{
					struct WBArg *ArgList = FileRequester->fr_ArgList;
					UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
					LONG i,Counted = 0;

					for(i = 0 ; i < FileRequester->fr_NumArgs ; i++)
					{
						if(ArgList[i].wa_Name)
						{
							if(ArgList[i].wa_Lock)
							{
								if(!NameFromLock(ArgList[i].wa_Lock,LocalBuffer,sizeof(LocalBuffer)))
								{
									ResultCode[0] = RC_ERROR;
									ResultCode[1] = IoErr();

									break;
								}
							}
							else
								strcpy(LocalBuffer,FileRequester->fr_Drawer);

							if(AddPart(LocalBuffer,ArgList[i].wa_Name,sizeof(LocalBuffer)))
							{
								if(CreateVarArgs(LocalBuffer,Pkt,"%s.%ld",Args[ARG_REQUESTFILE_NAME],i))
									Counted++;
								else
									break;
							}
							else
							{
								ResultCode[0] = RC_ERROR;
								ResultCode[1] = IoErr();

								break;
							}
						}
					}

					if(Counted)
					{
						LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%ld",Counted);

						CreateVarArgs(LocalBuffer,Pkt,"%s.COUNT",Args[ARG_REQUESTFILE_NAME]);
					}

					FreeAslRequest(FileRequester);
				}
				else
				{
					FreeAslRequest(FileRequester);

					ReleaseWindows();

					return(CreateVar(DummyBuffer,Pkt,Args[ARG_REQUESTFILE_NAME]));
				}
			}
			else
			{
				FreeAslRequest(FileRequester);

				ReleaseWindows();

				return(CreateResult(DummyBuffer,ResultCode));
			}
		}
		else
			ResultCode[0] = RC_WARN;

		ReleaseWindows();
	}

	return(NULL);
}

STRPTR
RexxReceiveFile(struct RexxPkt *Pkt)
{
	enum	{	ARG_RECEIVEFILE_MODE,ARG_RECEIVEFILE_NAME };

	LONG	Mode = TRANSFER_BINARY;
	STRPTR	Name = (STRPTR)Args[ARG_RECEIVEFILE_NAME];

	if(Args[ARG_RECEIVEFILE_MODE])
		Mode = ToMode(Args[ARG_RECEIVEFILE_MODE]);

	if(Mode == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_ACTION_NOT_KNOWN;
	}
	else
	{
		BlockWindows();

		switch(Mode)
		{
			case TRANSFER_ASCII:

				if(ChangeProtocol(Config->TransferConfig->ASCIIDownloadLibrary,Config->TransferConfig->ASCIIDownloadType))
				{
					StartReceiveXPR_File(TRANSFER_ASCII,Name,FALSE);

					SerialCommand(Config->CommandConfig->DownloadMacro);
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_NO_FREE_STORE;
				}

				ResetProtocol();

				break;

			case TRANSFER_TEXT:

				if(ChangeProtocol(Config->TransferConfig->TextDownloadLibrary,Config->TransferConfig->TextDownloadType))
				{
					StartReceiveXPR_File(TRANSFER_TEXT,Name,FALSE);

					SerialCommand(Config->CommandConfig->DownloadMacro);
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_NO_FREE_STORE;
				}

				ResetProtocol();

				break;

			case TRANSFER_BINARY:

				if(ChangeProtocol(Config->TransferConfig->BinaryDownloadLibrary,Config->TransferConfig->BinaryDownloadType))
				{
					StartReceiveXPR_File(TRANSFER_BINARY,Name,FALSE);

					SerialCommand(Config->CommandConfig->DownloadMacro);
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_NO_FREE_STORE;
				}

				ResetProtocol();

				break;
		}

		if(TransferFailed)
			ResultCode[0] = RC_ERROR;
		else
		{
			if(TransferAborted)
				ResultCode[0] = RC_WARN;
		}

		ReleaseWindows();
	}

	return(NULL);
}

STRPTR
RexxPrint(struct RexxPkt *Pkt)
{
	enum	{	ARG_PRINT_FROM,ARG_PRINT_TO,ARG_PRINT_SERIAL,ARG_PRINT_MODEM,ARG_PRINT_SCREEN,
				ARG_PRINT_TERMINAL,ARG_PRINT_USER,ARG_PRINT_COMMENT,ARG_PRINT_SIZE,
				ARG_PRINT_DATE,ARG_PRINT_BITS
			};

	LONG	Index,Mode = -1;
	ULONG	Flags = 0;

	if(Args[ARG_PRINT_SERIAL])
		Flags |= PRINT_SERIAL;

	if(Args[ARG_PRINT_MODEM])
		Flags |= PRINT_MODEM;

	if(Args[ARG_PRINT_SCREEN])
		Flags |= PRINT_SCREEN;

	if(Args[ARG_PRINT_TERMINAL])
		Flags |= PRINT_TERMINAL;

	if(Args[ARG_PRINT_USER])
		Flags |= PRINT_USERNAME;

	if(Args[ARG_PRINT_COMMENT])
		Flags |= PRINT_COMMENT;

	if(Args[ARG_PRINT_SIZE])
		Flags |= PRINT_SIZE;

	if(Args[ARG_PRINT_DATE])
		Flags |= PRINT_DATE;

	if(Args[ARG_PRINT_BITS])
		Flags |= PRINT_BITS;

	if((Index = ToList(Args[ARG_PRINT_FROM])) == -1)
	{
		if(!Stricmp(Args[ARG_PRINT_FROM],"SCREENTEXT"))
		{
			if(!RasterEnabled)
			{
				ResultCode[0] = RC_ERROR;
				ResultCode[1] = TERMERROR_NO_DATA_TO_PROCESS;

				return(NULL);
			}
			else
				Mode = 0;
		}

		if(!Stricmp(Args[ARG_PRINT_FROM],"CLIPBOARD"))
			Mode = 1;

		if(!Stricmp(Args[ARG_PRINT_FROM],"BUFFER"))
			Mode = 2;
	}

	if(Index == -1 && Mode == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_LIST;
	}
	else
	{
		BOOL Continue = TRUE;
		LONG Error = 0;
		STRPTR Name;
		BPTR File;

		if(Args[ARG_PRINT_TO])
			Name = Args[ARG_PRINT_TO];
		else
			Name = "PRT:";

		if(File = Open(Name,MODE_NEWFILE))
		{
			struct Window *ReqWindow;
			struct EasyStruct Easy;

			Easy.es_StructSize		= sizeof(struct EasyStruct);
			Easy.es_Flags			= 0;
			Easy.es_Title			= (UBYTE *)LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
			Easy.es_GadgetFormat	= (UBYTE *)LocaleString(MSG_PRINT_STOP_TXT);
			Easy.es_TextFormat		= (UBYTE *)LocaleString(MSG_GLOBAL_PRINTING_TXT);

			BlockWindows();

			if(ReqWindow = BuildEasyRequest(Window,&Easy,0))
			{
				struct GenericList *List;

				switch(Index)
				{
					case GLIST_DIAL:

						List = GenericListTable[Index];

						LockGenericList(List);

						if(GenericListCount(List) > 0)
						{
							struct GenericDialNode *Node;

							for(Node = (struct GenericDialNode *)List->ListHeader.mlh_Head ; Continue && Node->Node.ln_Succ ; Node = (struct GenericDialNode *)Node->Node.ln_Succ)
							{
								if(!Node->Index)
									Continue = PrintText(File,ReqWindow,&Error,"\n\"???\" (%s)",Node->Node.ln_Name);
							}
						}

						UnlockGenericList(List);

						break;

					case GLIST_UPLOAD:
					case GLIST_DOWNLOAD:

						List = GenericListTable[Index];

						LockGenericList(List);

						if(GenericListCount(List) > 0)
						{
							struct Node *TempNode;

							for(TempNode = (struct Node *)List->ListHeader.mlh_Head ; Continue && TempNode->ln_Succ ; TempNode = TempNode->ln_Succ)
								Continue = PrintFileInformation(File,ReqWindow,&Error,TempNode->ln_Name,Flags);
						}

						UnlockGenericList(List);

						break;

					case GLIST_WAIT:

						List = GenericListTable[Index];

						LockGenericList(List);

						if(GenericListCount(List) > 0)
						{
							struct Node *TempNode;

							for(TempNode = (struct Node *)List->ListHeader.mlh_Head ; Continue && TempNode->ln_Succ ; TempNode = TempNode->ln_Succ)
								Continue = PrintText(File,ReqWindow,&Error,"%s\n",TempNode->ln_Name);
						}

						UnlockGenericList(List);

						break;

					default:

						switch(Mode)
						{
							case 0:

								Continue = PrintScreen(File,ReqWindow,&Error);
								break;

							case 1:

								Continue = PrintClip(File,ReqWindow,&Error);
								break;

							case 2:

								Continue = PrintBuffer(File,ReqWindow,&Error);
								break;
						}

						break;
				}

				FreeSysRequest(ReqWindow);
			}

			ReleaseWindows();

			Close(File);
		}
		else
			Error = IoErr();

		if(Error)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = Error;
		}
		else
		{
			if(!Continue)
				ResultCode[0] = RC_WARN;
		}
	}

	return(NULL);
}

STRPTR
RexxOpenRequester(struct RexxPkt *Pkt)
{
	enum	{	ARG_OPENREQUESTER_REQUESTER };

	LONG Index;

	if((Index = ToRequester(Args[ARG_OPENREQUESTER_REQUESTER])) == -1)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_OBJECT_NOT_FOUND;
	}
	else
	{
		ULONG Code = 0;
		LONG i;

		switch(Index)
		{
			case REQUESTER_SERIAL:

				Code = MEN_SERIAL;
				break;

			case REQUESTER_MODEM:

				Code = MEN_MODEM;
				break;

			case REQUESTER_SCREEN:

				Code = MEN_SCREEN;
				break;

			case REQUESTER_TERMINAL:

				Code = MEN_TERMINAL;
				break;

			case REQUESTER_EMULATION:

				Code = MEN_SET_EMULATION;
				break;

			case REQUESTER_CLIPBOARD:

				Code = MEN_CLIPBOARD;
				break;

			case REQUESTER_CAPTURE:

				Code = MEN_CAPTURE;
				break;

			case REQUESTER_COMMANDS:

				Code = MEN_COMMANDS;
				break;

			case REQUESTER_MISC:

				Code = MEN_MISC;
				break;

			case REQUESTER_PATH:

				Code = MEN_PATH;
				break;

			case REQUESTER_TRANSFER:

				Code = MEN_TRANSFER;
				break;

			case REQUESTER_TRANSLATIONS:

				Code = MEN_TRANSLATION;
				break;

			case REQUESTER_FUNCTIONKEYS:

				Code = MEN_MACROS;
				break;

			case REQUESTER_CURSORKEYS:

				Code = MEN_CURSORKEYS;
				break;

			case REQUESTER_FASTMACROS:

				Code = MEN_FAST_MACROS;
				break;

			case REQUESTER_HOTKEYS:

				Code = MEN_HOTKEYS;
				break;

			case REQUESTER_SPEECH:

				Code = MEN_SPEECH;
				break;

			case REQUESTER_SOUND:

				Code = MEN_SOUND;
				break;


			case REQUESTER_PHONE:

				Code = MEN_PHONEBOOK;
				break;
		}

			/* Scan the menu list... */

		for(i = 0 ; TermMenu[i].nm_Type != NM_END ; i++)
		{
				/* Did we get a valid name string? */

			if(TermMenu[i].nm_Label != NM_BARLABEL && (TermMenu[i].nm_Type == NM_ITEM || TermMenu[i].nm_Type == NM_SUB))
			{
				if((ULONG)(IPTR)TermMenu[i].nm_UserData == Code)
				{
					struct DataMsg *Msg;

					if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
					{
						Msg->Type = DATAMSGTYPE_MENU;
						Msg->Size = (ULONG)(IPTR)TermMenu[i].nm_UserData;
						Msg->Data = NULL;

						PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);

						ResultCode[0] = RC_OK;
					}
					else
						ResultCode[0] = RC_WARN;

					break;
				}
			}
		}
	}

	return(NULL);
}

STRPTR
RexxOpen(struct RexxPkt *Pkt)
{
	enum	{	ARG_OPEN_NAME,ARG_OPEN_TO };

	LONG Index = ToConfig(Args[ARG_OPEN_TO]);

	if(Index == -1 || Index > DATATYPE_PHONEBOOK)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_OBJECT_NOT_FOUND;
	}
	else
	{
		UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
		STRPTR FileName;

		if(Args[ARG_OPEN_NAME])
			FileName = Args[ARG_OPEN_NAME];
		else
		{
			struct FileRequester *FileRequest;
			LONG TitleID = 0;

			FileName = NULL;

			switch(Index)
			{
				case DATATYPE_TRANSLATIONS:

					TitleID = MSG_PHONEPANEL_SELECT_TRANSLATION_TXT;
					break;

				case DATATYPE_FUNCTIONKEYS:

					TitleID = MSG_PHONEPANEL_SELECT_KEYBOARD_MACROS_TXT;
					break;

				case DATATYPE_CURSORKEYS:

					TitleID = MSG_PHONEPANEL_SELECT_CURSOR_KEYS_TXT;
					break;

				case DATATYPE_FASTMACROS:

					TitleID = MSG_PHONEPANEL_SELECT_FAST_MACROS_TXT;
					break;

				case DATATYPE_HOTKEYS:

					TitleID = MSG_HOTKEYPANEL_LOAD_HOTKEYS_TXT;
					break;

				case DATATYPE_SPEECH:

					TitleID = MSG_SPEECHPANEL_LOAD_SPEECH_SETTINGS_TXT;
					break;

				case DATATYPE_SOUND:

					TitleID = MSG_SOUNDPANEL_LOAD_SOUNDS_TXT;
					break;

				case DATATYPE_BUFFER:

					TitleID = MSG_TERMMAIN_LOAD_BUFFER_TXT;
					break;

				case DATATYPE_CONFIGURATION:

					TitleID = MSG_TERMMAIN_OPEN_PREFERENCES_TXT;
					break;

				case DATATYPE_PHONEBOOK:

					TitleID = MSG_PHONEPANEL_LOAD_PHONEBOOK_TXT;
					break;
			}

			BlockWindows();

			DummyBuffer[0] = 0;

			if(FileRequest = OpenSingleFile(Window,LocaleString(TitleID),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				FreeAslRequest(FileRequest);

				FileName = DummyBuffer;
			}
			else
				ResultCode[0] = RC_WARN;

			ReleaseWindows();
		}

		if(FileName)
		{
			struct TranslationEntry	**Send,**Receive;
			BPTR SomeFile;
			BOOL Success;

			if(!GetFileSize(FileName))
			{
				ResultCode[0] = RC_ERROR;
				ResultCode[1] = ERROR_OBJECT_NOT_FOUND;

				return(NULL);
			}

			BlockWindows();

			switch(Index)
			{
				case DATATYPE_TRANSLATIONS:

					Receive = NULL;
					Success = FALSE;

					if(Send = AllocTranslationTable())
					{
						if(Receive = AllocTranslationTable())
						{
							if(!(Success = LoadTranslationTables(FileName,Send,Receive)))
							{
								ResultCode[0] = RC_ERROR;
								ResultCode[1] = IoErr();
							}
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_NO_FREE_STORE;
					}

					if(!Success)
					{
						if(Send)
							FreeTranslationTable(Send);

						if(Receive)
							FreeTranslationTable(Receive);
					}
					else
					{
						strcpy(Config->TranslationFileName,FileName);

						strcpy(LastTranslation,FileName);

						FreeTranslationTable(SendTable);
						FreeTranslationTable(ReceiveTable);

						SendTable		= Send;
						ReceiveTable	= Receive;
					}

					break;

				case DATATYPE_FUNCTIONKEYS:

					if(!LoadMacros(FileName,MacroKeys))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						MacroChanged = FALSE;

						strcpy(LastMacros,FileName);

						strcpy(Config->MacroFileName,LastMacros);
					}

					break;

				case DATATYPE_CURSORKEYS:

					if(!ReadIFFData(FileName,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						CursorKeysChanged = FALSE;

						strcpy(LastCursorKeys,FileName);

						strcpy(Config->CursorFileName,LastCursorKeys);
					}

					break;

				case DATATYPE_FASTMACROS:

					if(!LoadFastMacros(FileName,&FastMacroList))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(Config->FastMacroFileName,FileName);

						strcpy(LastFastMacros,FileName);

						FastMacrosChanged = FALSE;

						FastMacroCount = GetListSize(&FastMacroList);
					}

					RefreshFastWindow();

					break;

				case DATATYPE_HOTKEYS:

					if(!LoadHotkeys(FileName,&Hotkeys))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastKeys,FileName);
						strcpy(Config->HotkeyFileName,LastKeys);

						HotkeysChanged = FALSE;

						SetupCx();
					}

					break;

				case DATATYPE_SPEECH:

					if(!ReadIFFData(FileName,&SpeechConfig,sizeof(struct SpeechConfig),ID_SPEK))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastSpeech,FileName);
						strcpy(Config->SpeechFileName,LastSpeech);

						SpeechSetup();

						SpeechChanged = FALSE;
					}

					break;

				case DATATYPE_SOUND:

					if(!ReadIFFData(FileName,&SoundConfig,sizeof(struct SoundConfig),ID_SOUN))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
					else
					{
						strcpy(LastSound,FileName);
						strcpy(Config->SoundFileName,LastSound);

						SoundInit();

						SoundChanged = FALSE;
					}

					break;

				case DATATYPE_BUFFER:

					if(SomeFile = Open(FileName,MODE_OLDFILE))
					{
						LONG Len;

						LineRead(BNULL,NULL,0);

						while((Len = LineRead(SomeFile,FileName,80)) > 0)
							CaptureParser(ParserStuff,FileName,Len,(COPTR)AddLine);

						Close(SomeFile);

						BufferChanged = TRUE;
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}

					break;

				case DATATYPE_CONFIGURATION:

					if(ReadConfig(FileName,PrivateConfig))
					{
						SwapConfig(PrivateConfig,Config);

						strcpy(LastConfig,FileName);

						ConfigSetup();

						ConfigChanged = FALSE;
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}

					break;

				case DATATYPE_PHONEBOOK:

					if(GetActiveEntry(GlobalPhoneHandle))
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_OBJECT_IN_USE;
					}
					else
					{
						PhonebookHandle *PhoneHandle;

						if(PhoneHandle = LoadPhonebook(FileName))
						{
							strcpy(LastPhone,FileName);
							strcpy(Config->PhonebookFileName,LastPhone);

							PhonebookChanged = FALSE;
							RebuildMenu = TRUE;
							ActivateJob(MainJobQueue,RebuildMenuJob);

							DeletePhonebook(GlobalPhoneHandle);
							GlobalPhoneHandle = PhoneHandle;
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = IoErr();
						}
					}

					break;
			}

			ReleaseWindows();
		}
		else
			ResultCode[0] = RC_WARN;
	}

	return(NULL);
}

STRPTR
RexxDelay(struct RexxPkt *Pkt)
{
	enum	{	ARG_DELAY_MICROSECONDS,ARG_DELAY_SECONDS,ARG_DELAY_MINUTES,ARG_DELAY_QUIET };

	LONG Seconds,Micros;
	ULONG Signals;
	BOOL QuietPlease;

	Seconds = Micros = 0;

	if(Args[ARG_DELAY_QUIET] || !ReadRequest || !WriteRequest)
		QuietPlease = TRUE;
	else
		QuietPlease = FALSE;

	if(Args[ARG_DELAY_MINUTES])
		Seconds += 60 * (*(LONG *)Args[ARG_DELAY_MINUTES]);

	if(Args[ARG_DELAY_SECONDS])
		Seconds += *(LONG *)Args[ARG_DELAY_SECONDS];

	if(Args[ARG_DELAY_MICROSECONDS])
		Micros = *(LONG *)Args[ARG_DELAY_MICROSECONDS];

	if(Seconds || Micros)
	{
		BOOL Done;

		StartTime(Seconds,Micros);

		BlockWindows();

			/* Lock the current xOFF state and clear the xOFF flag. */

		Lock_xOFF();
		Clear_xOFF();

			/* Loop until the timer has elapsed. */

		Done = FALSE;

		do
		{
				/* Wait for something to happen. */

			Signals = (*SerialWaitForData)(SIG_TIMER | SIG_BREAK);

			if(Signals & SIG_SERIAL)
			{
				ULONG Length;

					/* Check how much data is available. */

				if(Length = (*SerialGetWaiting)())
				{
						/* Don't read more than the buffer will hold. */

					if(Length > SerialBufferSize / 2)
						Length = SerialBufferSize / 2;

						/* Read the data. */

					if(Length = (*SerialRead)(ReadBuffer,Length))
					{
							/* Got some more data. */

						BytesIn += Length;

						if(Translate_CR_LF)
							Length = (*Translate_CR_LF)(ReadBuffer,Length);

						if(!QuietPlease && Length)
							ConProcess(ReadBuffer,Length);
					}
				}
			}

			if(Signals & SIG_BREAK)
			{
				ResultCode[0] = RC_WARN;

				Done = TRUE;
			}

			if(Signals & SIG_TIMER)
				Done = TRUE;
		}
		while(!Done);

			/* Stop the timer. */

		StopTime();

			/* Unlock the xOFF state. */

		Unlock_xOFF();

		ReleaseWindows();
	}

	return(NULL);
}

STRPTR
RexxCapture(struct RexxPkt *Pkt)
{
	enum	{	ARG_CAPTURE_TO,ARG_CAPTURE_NAME,ARG_OVERWRITE,ARG_APPEND,ARG_SKIP };

	if(!Stricmp(Args[ARG_CAPTURE_TO],"PRINTER"))
	{
		if(!PrinterCapture)
			OpenPrinterCapture(FALSE);
	}
	else
	{
		if(!Stricmp(Args[ARG_CAPTURE_TO],"FILE"))
		{
			if(FileCapture)
			{
				ResultCode[0] = RC_ERROR;
				ResultCode[1] = ERROR_OBJECT_IN_USE;
			}
			else
			{
				if(Args[ARG_CAPTURE_NAME])
				{
					if(FileCapture = BufferOpen(Args[ARG_CAPTURE_NAME],"a"))
						strcpy(CaptureName,Args[ARG_CAPTURE_NAME]);
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = IoErr();
					}
				}
				else
				{
					UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
					struct FileRequester *FileRequest;

					if(!CaptureName[0])
					{
						strcpy(CaptureName,Config->CaptureConfig->CapturePath);

						if(!AddPart(CaptureName,LocaleString(MSG_DIALPANEL_CAPTURE_NAME_TXT),sizeof(DummyBuffer)))
							CaptureName[0] = 0;
					}

					strcpy(DummyBuffer,CaptureName);

					BlockWindows();

					if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_CAPTURE_TO_DISK_TXT),LocaleString(MSG_GLOBAL_OPEN_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
					{
						BOOL Continue;

						if(GetFileSize(DummyBuffer))
						{
							Continue = TRUE;

							if(!Args[ARG_OVERWRITE] && ! Args[ARG_APPEND] && ! Args[ARG_SKIP])
							{
								switch(ShowRequest(Window,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_TXT),LocaleString(MSG_GLOBAL_CREATE_APPEND_CANCEL_TXT),DummyBuffer))
								{
									case 0:

										ResultCode[0] = RC_WARN;
										Continue = FALSE;
										break;

									case 1:

										FileCapture = BufferOpen(DummyBuffer,"w");
										break;

									case 2:

										FileCapture = BufferOpen(DummyBuffer,"a");
										break;
								}
							}
							else
							{
								STRPTR Mode;

								if(Args[ARG_OVERWRITE])
									Mode = "w";
								else if(Args[ARG_APPEND])
									Mode = "a";
								else
									Mode = NULL;

								if(Mode)
									FileCapture = BufferOpen(DummyBuffer,Mode);
								else
								{
									ResultCode[0] = RC_WARN;
									Continue = FALSE;
								}
							}
						}
						else
						{
							Continue = TRUE;

							FileCapture = BufferOpen(DummyBuffer,"w");
						}

						if(Continue)
						{
							if(!FileCapture)
							{
								ShowRequest(Window,LocaleString(MSG_GLOBAL_ERROR_OPENING_FILE_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),DummyBuffer);

								ResultCode[0] = RC_ERROR;
								ResultCode[1] = IoErr();
							}
							else
							{
								strcpy(CaptureName,DummyBuffer);

								CheckItem(MEN_CAPTURE_TO_FILE,TRUE);
							}
						}

						FreeAslRequest(FileRequest);
					}

					ReleaseWindows();
				}
			}
		}
		else
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = ERROR_REQUIRED_ARG_MISSING;
		}
	}

	ConOutputUpdate();

	CheckItem(MEN_CAPTURE_TO_FILE,		FileCapture != NULL);
	CheckItem(MEN_CAPTURE_TO_PRINTER,	PrinterCapture != BNULL);

	return(NULL);
}

STRPTR
RexxAdd(struct RexxPkt *Pkt)
{
	enum	{	ARG_ADD_FROM,ARG_ADD_BEFORE,ARG_ADD_AFTER,ARG_ADD_RESPONSE,
				ARG_ADD_COMMAND,ARG_ADD_PHONEENTRY,ARG_ADD_NAME
			};

	LONG ListIndex;

	if((ListIndex = ToList(Args[ARG_ADD_FROM])) != -1)
	{
		struct GenericList *List = GenericListTable[ListIndex];
		LONG AddMode;

		if(Args[ARG_ADD_BEFORE])
			AddMode = ADD_GLIST_BEFORE;
		else
		{
			if(Args[ARG_ADD_AFTER])
				AddMode = ADD_GLIST_BEHIND;
			else
				AddMode = ADD_GLIST_BOTTOM;
		}

		if(ListIndex == GLIST_DIAL)
		{
			if(Args[ARG_ADD_PHONEENTRY])
			{
				if(!IsNumeric(Args[ARG_ADD_PHONEENTRY]))
				{
					STRPTR Buffer;

					if(Buffer = CreateMatchBuffer(Args[ARG_ADD_PHONEENTRY]))
					{
						struct GenericDialNode *Node;
						BOOL GotIt = FALSE;
						LONG i;

						for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
						{
							if(MatchBuffer(Buffer,GlobalPhoneHandle->Phonebook[i]->Header->Name))
							{
								if(Node = (struct GenericDialNode *)CreateGenericListNode(sizeof(struct GenericDialNode),GlobalPhoneHandle->Phonebook[i]->Header->Name))
								{
									Node->Number = NULL;
									Node->Index = -1;

									AddGenericListNode(List,(struct Node *)Node,AddMode,TRUE);

									GotIt = TRUE;
								}
								else
								{
									ResultCode[0] = RC_ERROR;
									ResultCode[1] = ERROR_NO_FREE_STORE;

									break;
								}
							}
						}

						if(!GotIt && ResultCode[0] != RC_ERROR)
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_OBJECT_NOT_FOUND;
						}

						DeleteMatchBuffer(Buffer);
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_NO_FREE_STORE;
					}
				}
				else
				{
					LONG Index = Atol(Args[ARG_ADD_PHONEENTRY]);

					if(Index < 0 || Index > GlobalPhoneHandle->NumPhoneEntries)
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = TERMERROR_INDEX_OUT_OF_RANGE;
					}
					else
					{
						struct GenericDialNode *Node;

						if(Node = (struct GenericDialNode *)CreateGenericListNode(sizeof(struct GenericDialNode),GlobalPhoneHandle->Phonebook[Index]->Header->Name))
						{
							Node->Number = NULL;
							Node->Index = Index;

							AddGenericListNode(List,(struct Node *)Node,AddMode,TRUE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}
					}
				}
			}
			else
			{
				if(Args[ARG_ADD_NAME])
				{
					struct GenericDialNode *Node;

					if(Node = (struct GenericDialNode *)CreateGenericListNode(sizeof(struct GenericDialNode),Args[ARG_ADD_NAME]))
					{
						Node->Number = Node->Node.ln_Name;
						Node->Index = -1;

						AddGenericListNode(List,(struct Node *)Node,AddMode,TRUE);
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_NO_FREE_STORE;
					}
				}
			}
		}
		else
		{
			if(ListIndex == GLIST_WAIT)
			{
				STRPTR	Response;
				LONG	ResponseLen;

				if(Args[ARG_ADD_RESPONSE])
				{
					Response	= Args[ARG_ADD_RESPONSE];
					ResponseLen	= strlen(Response);
				}
				else
				{
					Response	= NULL;
					ResponseLen	= 0;
				}

				if(Args[ARG_ADD_NAME])
				{
					struct WaitNode *Node;

					if(Node = (struct WaitNode *)CreateGenericListNode(sizeof(struct WaitNode) + ResponseLen + 1,Args[ARG_ADD_NAME]))
					{
						LONG i,Len;

						Len = TranslateString(Node->Node.ln_Name,Node->Node.ln_Name);

						Node->Node.ln_Name[Len] = 0;

						for(i = 0 ; i < Len ; i++)
							Node->Node.ln_Name[i] = ToUpper(Node->Node.ln_Name[i]);

						if(Response && ResponseLen)
						{
							Node->Response = &Node->Node.ln_Name[Len + 1];

							strcpy(Node->Response,Response);

							Node->ResponseLen = TranslateString(Node->Response,Node->Response);
						}
						else
							Node->ResponseLen = 0;

						AddGenericListNode(List,(struct Node *)Node,AddMode,TRUE);
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_NO_FREE_STORE;
					}
				}
				else
				{
					ResultCode[0] = RC_ERROR;
					ResultCode[1] = ERROR_REQUIRED_ARG_MISSING;
				}
			}
			else
			{
				if(ListIndex == GLIST_TRAP)
				{
					if(!Args[ARG_ADD_NAME] || !Args[ARG_ADD_COMMAND])
					{
						struct TrapNode	*Node;
						UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
						LONG NameLen;

						if(strlen(Args[ARG_ADD_NAME]) > sizeof(LocalBuffer) - 1)
							Args[ARG_ADD_NAME][sizeof(LocalBuffer) - 1] = 0;

						NameLen = TranslateString(Args[ARG_ADD_NAME],LocalBuffer);

						if(Node = (struct TrapNode *)AllocVecPooled(sizeof(struct TrapNode) + strlen(Args[ARG_ADD_NAME]) + 1 + NameLen + strlen(Args[ARG_ADD_COMMAND]) + 1,MEMF_ANY))
						{
							STRPTR String = Node->Node.ln_Name = (STRPTR)(Node + 1);

							strcpy(String,Args[ARG_ADD_NAME]);

							String += strlen(String) + 1;

							Node->Sequence = String;

							CopyMem(LocalBuffer,String,NameLen);

							String += NameLen;

							Node->Command = String;

							strcpy(String,Args[ARG_ADD_COMMAND]);

							Node->SequenceLen = NameLen;
							Node->Count = 0;

							AddGenericListNode(List,(struct Node *)Node,AddMode,TRUE);
						}
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = ERROR_REQUIRED_ARG_MISSING;
					}
				}
				else
				{
					if(Args[ARG_ADD_NAME])
					{
						struct Node *Node;

						if(Node = CreateGenericListNode(sizeof(struct DialNode),Args[ARG_ADD_NAME]))
							AddGenericListNode(List,Node,AddMode,TRUE);
						else
						{
							ResultCode[0] = RC_ERROR;
							ResultCode[1] = ERROR_NO_FREE_STORE;
						}
					}
					else
					{
						ResultCode[0] = RC_ERROR;
						ResultCode[1] = TERMERROR_WRONG_LIST;
					}
				}
			}
		}
	}
	else
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_UNKNOWN_LIST;
	}

	return(NULL);
}

STRPTR
RexxActivate(struct RexxPkt *UnusedPkt)
{
	if(Window)
		BumpWindow(Window);
	else
		Signal((struct Task *)ThisProcess,SIGBREAKF_CTRL_F);

	return(NULL);
}

STRPTR
RexxBaud(struct RexxPkt *Pkt)
{
	enum	{	ARG_BAUD_RATE };

	LONG Rate = *(LONG *)Args[ARG_BAUD_RATE],Min = MILLION,Diff,Index = 0;
	UBYTE Number[10];
	LONG i;

	for(i = 0 ; i < NumBaudRates ; i++)
	{
		Diff = Rate - BaudRates[i];

		if(Diff >= 0 && Diff < Min)
		{
			Min		= Diff;
			Index	= i;
		}
	}

	LimitedSPrintf(sizeof(Number),Number,"%ld");

	if(BaudRates[Index] != Config->SerialConfig->BaudRate)
	{
		Config->SerialConfig->BaudRate = BaudRates[Index];

		ConfigChanged = TRUE;

		UpdateRequired = TRUE;
	}

	return(CreateArgstring(Number,strlen(Number)));
}

STRPTR
RexxBeepScreen(struct RexxPkt *UnusedPkt)
{
	BellSignal();

	return(NULL);
}

STRPTR
RexxCallMenu(struct RexxPkt *Pkt)
{
	enum	{	ARG_CALLMENU_TITLE };

	STRPTR Buffer;

	if(Buffer = CreateMatchBuffer(Args[ARG_CALLMENU_TITLE]))
	{
		LONG i;

		ResultCode[0] = RC_WARN;

			/* Scan the menu list... */

		for(i = 0 ; TermMenu[i].nm_Type != NM_END ; i++)
		{
				/* Did we get a valid name string? */

			if(TermMenu[i].nm_Label != NM_BARLABEL && (TermMenu[i].nm_Type == NM_ITEM || TermMenu[i].nm_Type == NM_SUB))
			{
					/* Does the name match our template? */

				if(MatchBuffer(Buffer,(STRPTR)TermMenu[i].nm_Label))
				{
					struct DataMsg *Msg;

					if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
					{
						Msg->Type = DATAMSGTYPE_MENU;
						Msg->Size = (ULONG)(IPTR)TermMenu[i].nm_UserData;
						Msg->Data = NULL;

						PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);

						ResultCode[0] = RC_OK;
					}
					else
						ResultCode[0] = RC_WARN;

					break;
				}
			}
		}

		DeleteMatchBuffer(Buffer);
	}
	else
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_NO_FREE_STORE;
	}

	return(NULL);
}

STRPTR
RexxClear(struct RexxPkt *Pkt)
{
	enum	{	ARG_CLEAR_FROM,ARG_CLEAR_FORCE };

	if(!Stricmp(Args[ARG_CLEAR_FROM],"BUFFER"))
	{
		if(Lines)
		{
			if(Args[ARG_CLEAR_FORCE])
			{
				FreeBuffer();

				TerminateBuffer();
			}
			else
			{
				BlockWindows();

				if(ShowRequest(Window,LocaleString(MSG_TERMMAIN_BUFFER_STILL_HOLDS_LINES_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Lines))
				{
					FreeBuffer();

					TerminateBuffer();
				}
				else
					ResultCode[0] = RC_WARN;

				ReleaseWindows();
			}
		}
	}
	else
	{
		LONG ListIndex;

		if((ListIndex = ToList(Args[ARG_CLEAR_FROM])) != -1)
			ClearGenericList(GenericListTable[ListIndex],TRUE);
		else
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = TERMERROR_UNKNOWN_LIST;
		}
	}

	return(NULL);
}

STRPTR
RexxClearScreen(struct RexxPkt *UnusedPkt)
{
	ConClear();

	return(NULL);
}

STRPTR
RexxClose(struct RexxPkt *Pkt)
{
	enum	{	ARG_CLOSE_FROM };

	STATIC STRPTR ValidArgs[3] =
	{
		"PRINTER",
		"FILE",
		"ALL"
	};

	LONG i;

	for(i = 0 ; i < 3 ; i++)
	{
		if(!Stricmp(Args[ARG_CLOSE_FROM],ValidArgs[i]))
		{
			if(i == 0 || i == 2)
			{
				if(PrinterCapture)
					ClosePrinterCapture(TRUE);
			}

			if(i == 1 || i == 2)
			{
				if(FileCapture)
				{
					BufferClose(FileCapture);

					CheckItem(MEN_CAPTURE_TO_FILE,FALSE);

					FileCapture = NULL;

					if(!GetFileSize(CaptureName))
						DeleteFile(CaptureName);
					else
					{
						AddProtection(CaptureName,FIBF_EXECUTE);

						if(Config->MiscConfig->CreateIcons)
							AddIcon(CaptureName,FILETYPE_TEXT,TRUE);
					}

					ConOutputUpdate();
				}
			}

			return(NULL);
		}
	}

	ResultCode[0] = RC_ERROR;
	ResultCode[1] = ERROR_TOO_MANY_ARGS;

	return(NULL);
}

STRPTR
RexxCloseDevice(struct RexxPkt *UnusedPkt)
{
	ClearSerial();

	DeleteSerial();

	return(NULL);
}

STRPTR
RexxCloseRequester(struct RexxPkt *Pkt)
{
	if(ThisProcess)
		Signal((struct Task *)ThisProcess,SIG_BREAK);
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxDeactivate(struct RexxPkt *UnusedPkt)
{
	Forbid();

	if(Window)
		ActivateJob(MainJobQueue,IconifyJob);

	Permit();

	return(NULL);
}

STATIC VOID
RexxDialDestructor(struct MsgItem *Item)
{
	if(((struct DataDialMsg *)Item)->DialMsg)
	{
		((struct DataDialMsg *)Item)->DialMsg->rm_Result1 = RC_WARN;
		((struct DataDialMsg *)Item)->DialMsg->rm_Result2 = 0;

		ReplyMsg((struct Message *)((struct DataDialMsg *)Item)->DialMsg);
	}

	DeleteList(((struct DataDialMsg *)Item)->DialList);

	FreeVecPooled(Item);
}

STRPTR
RexxDial(struct RexxPkt *Pkt)
{
	enum	{	ARG_SYNC,ARG_DIAL_NUM };

	struct DataDialMsg *Msg;
	struct List *DialList;

	if(!(Msg = (struct DataDialMsg *)CreateMsgItem(sizeof(struct DataDialMsg))))
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_NO_FREE_STORE;

		return(NULL);
	}

	if(Args[ARG_DIAL_NUM])
	{
		if(DialList = CreateList())
		{
			struct GenericDialNode *Node;
			LONG Len;

			Len = strlen(Args[ARG_DIAL_NUM]);

			if(Node = (struct GenericDialNode *)AllocVecPooled(sizeof(struct DialNode) + Len + 1,MEMF_ANY | MEMF_CLEAR))
			{
				Node->Node.ln_Name = (STRPTR)(Node + 1);

				strcpy(Node->Node.ln_Name,Args[ARG_DIAL_NUM]);

				Node->Number = Node->Node.ln_Name;
				Node->Index = -1;

				AddTail(DialList,(struct Node *)Node);
			}
			else
			{
				DeleteMsgItem((struct MsgItem *)Msg);
				FreeVecPooled(DialList);

				ResultCode[0] = RC_ERROR;
				ResultCode[1] = ERROR_NO_FREE_STORE;

				return(NULL);
			}
		}
		else
		{
			DeleteMsgItem((struct MsgItem *)Msg);
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = ERROR_NO_FREE_STORE;

			return(NULL);
		}
	}
	else
	{
		if(GenericListCount(GenericListTable[GLIST_DIAL]) > 0)
		{
			if(DialList = CreateList())
			{
				struct Node *Node;

				while(Node = RemoveFirstGenericListNode(GenericListTable[GLIST_DIAL]))
					AddTail(DialList,Node);
			}
			else
			{
				DeleteMsgItem((struct MsgItem *)Msg);

				ResultCode[0] = RC_ERROR;
				ResultCode[1] = ERROR_NO_FREE_STORE;

				return(NULL);
			}
		}
		else
			DialList = NULL;
	}

	if(DialList)
	{
		if(Args[ARG_SYNC])
		{
			Msg->DialMsg = Pkt->RexxMsg;

			Pkt->RexxMsg = NULL;
		}
		else
			Msg->DialMsg = NULL;

		InitMsgItem(Msg,RexxDialDestructor);

		Msg->Data.Type = DATAMSGTYPE_DIAL;
		Msg->DialList = DialList;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
		DeleteMsgItem((struct MsgItem *)Msg);

	return(NULL);
}

STRPTR
RexxDuplex(struct RexxPkt *Pkt)
{
	enum	{	ARG_DUPLEX_FULL,ARG_DUPLEX_HALF };

	LONG OldMode;
	LONG Mode;

	if(Args[ARG_DUPLEX_FULL])
		Mode = DUPLEX_FULL;

	if(Args[ARG_DUPLEX_HALF])
		Mode = DUPLEX_HALF;

	OldMode = Config->SerialConfig->Duplex;

	if(Config->SerialConfig->Duplex != Mode)
	{
		Config->SerialConfig->Duplex = Mode;

		UpdateRequired = TRUE;

		ConfigChanged = TRUE;
	}

	return(CreateArgstring(DuplexMappings[OldMode],strlen(DuplexMappings[OldMode])));
}

STRPTR
RexxFault(struct RexxPkt *Pkt)
{
	enum	{	ARG_FAULT_CODE };

	LONG Code = *(LONG *)Args[ARG_FAULT_CODE];
	UBYTE RexxResultString[MAX_FILENAME_LENGTH];
	CONST_STRPTR Result;

	if(Code >= ERR10_001 && Code <= ERR10_048)
		Result = LocaleString(MSG_AREXX_SYSERR10_001_TXT + Code - ERR10_001);
	else
	{
		if(Code >= TERMERROR_NO_DATA_TO_PROCESS && Code <= TERMERROR_WRONG_LIST)
			Result = LocaleString(MSG_AREXX_HOSTERR_000_TXT + Code - TERMERROR_NO_DATA_TO_PROCESS);
		else
		{
			Fault(Code,NULL,RexxResultString,sizeof(RexxResultString));

			Result = RexxResultString;
		}
	}

	return(CreateResult(Result,ResultCode));
}

STRPTR
RexxGetClip(struct RexxPkt *Pkt)
{
	enum	{	ARG_GETCLIP_UNIT };

	struct IFFHandle *Handle;
	STRPTR ResultBuffer;
	LONG Unit;
	LONG Error;

	if(Args[ARG_GETCLIP_UNIT])
		Unit = *(LONG *)Args[ARG_GETCLIP_UNIT];
	else
		Unit = Config->ClipConfig->ClipboardUnit;

	ResultBuffer = NULL;

	if(Handle = OpenIFFClip(Unit,MODE_OLDFILE))
	{
		if(!(Error = StopChunk(Handle,ID_FTXT,ID_CHRS)))
		{
			if(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				struct ContextNode *ContextNode;

				ContextNode = CurrentChunk(Handle);

				if(ContextNode->cn_Type == ID_FTXT && ContextNode->cn_Size > 0)
				{
					STRPTR Result;

					if(Result = (STRPTR)AllocVecPooled(ContextNode->cn_Size,MEMF_ANY))
					{
						if(ReadChunkBytes(Handle,Result,ContextNode->cn_Size) == ContextNode->cn_Size)
							ResultBuffer = CreateArgstring(Result,ContextNode->cn_Size);

						FreeVecPooled(Result);
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}
				else
					ResultCode[0] = RC_WARN;
			}
			else
				Error = ERROR_OBJECT_NOT_FOUND;
		}

		CloseIFFClip(Handle);
	}
	else
		Error = IoErr();

	if(Error)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = Error;
	}

	return(ResultBuffer);
}

STRPTR
RexxGoOnline(struct RexxPkt *Pkt)
{
	struct DataMsg *Msg;

	if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
	{
		Msg->Type = DATAMSGTYPE_GOONLINE;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxHangup(struct RexxPkt *Pkt)
{
	struct DataMsg *Msg;

	if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
	{
		Msg->Type = DATAMSGTYPE_HANGUP;
		Msg->Data = (UBYTE *)Pkt->RexxMsg;

		Pkt->RexxMsg = NULL;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxHelp(struct RexxPkt *Pkt)
{
	enum	{	ARG_HELP_COMMAND,ARG_HELP_PROMPT };

	if(Args[ARG_HELP_PROMPT])
		GuideSetup();
	else
	{
		LONG i;

		for(i = 0 ; i < CommandTableSize ; i++)
		{
			if(!Stricmp(Args[ARG_HELP_COMMAND],CommandTable[i].Name))
			{
				if(CommandTable[i].Arguments)
					return(CreateResult(CommandTable[i].Arguments,ResultCode));
				else
					return(CreateResult(",",ResultCode));
			}
		}

		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_OBJECT_NOT_FOUND;
	}

	return(NULL);
}

STRPTR
RexxOpenDevice(struct RexxPkt *Pkt)
{
	enum	{	ARG_OPENDEVICE_NAME,ARG_OPENDEVICE_UNIT };

	if(ReadRequest)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = TERMERROR_DEVICE_DRIVER_STILL_OPEN;

		return(NULL);
	}
	else
	{
		UBYTE Result[MAX_FILENAME_LENGTH+10];

		LimitedSPrintf(sizeof(Result),"\"%s\" %ld",Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber);

		if(Args[ARG_OPENDEVICE_NAME])
		{
			strcpy(Config->SerialConfig->SerialDevice,Args[ARG_OPENDEVICE_NAME]);

			ConfigChanged = TRUE;
		}

		if(Args[ARG_OPENDEVICE_UNIT])
		{
			Config->SerialConfig->UnitNumber = *(LONG *)Args[ARG_OPENDEVICE_UNIT];

			ConfigChanged = TRUE;
		}

		BlockWindows();

		ReopenSerial();

		ReleaseWindows();

		return(CreateArgstring(Result,strlen(Result)));
	}
}

STRPTR
RexxParity(struct RexxPkt *Pkt)
{
	enum	{	ARG_PARITY_EVEN,ARG_PARITY_ODD,ARG_PARITY_NONE,ARG_PARITY_MARK,ARG_PARITY_SPACE };

	LONG OldMode;
	LONG Mode;

	if(Args[ARG_PARITY_EVEN])
		Mode = PARITY_EVEN;

	if(Args[ARG_PARITY_ODD])
		Mode = PARITY_ODD;

	if(Args[ARG_PARITY_NONE])
		Mode = PARITY_NONE;

	if(Args[ARG_PARITY_MARK])
		Mode = PARITY_MARK;

	if(Args[ARG_PARITY_SPACE])
		Mode = PARITY_SPACE;

	OldMode = Config->SerialConfig->Parity;

	if(Config->SerialConfig->Parity != Mode)
	{
		Config->SerialConfig->Parity = Mode;

		UpdateRequired = TRUE;

		ConfigChanged = TRUE;
	}

	return(CreateArgstring(ParityMappings[OldMode],strlen(ParityMappings[OldMode])));
}

STRPTR
RexxPasteClip(struct RexxPkt *Pkt)
{
	enum	{	ARG_PASTECLIP_UNIT };

	struct DataMsg *Msg;
	LONG Unit;

	if(Args[ARG_PASTECLIP_UNIT])
		Unit = *(LONG *)Args[ARG_PASTECLIP_UNIT];
	else
		Unit = Config->ClipConfig->ClipboardUnit;

	if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
	{
		Msg->Type = DATAMSGTYPE_WRITECLIP;
		Msg->Size = Unit;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_NO_FREE_STORE;
	}

	return(NULL);
}

STRPTR
RexxProcessIO(struct RexxPkt *Pkt)
{
	enum	{	ARG_PROCESSIO_ON,ARG_PROCESSIO_OFF };

	BOOL OldState;

	OldState = ProcessIO;

	if(Args[ARG_PROCESSIO_ON])
		ProcessIO = TRUE;

	if(Args[ARG_PROCESSIO_OFF])
		ProcessIO = FALSE;

	if(OldState != ProcessIO)
	{
		if(ProcessIO)
			RestartSerial();

		UpdateSerialJob();

		if(!ProcessIO)
			ClearSerial();
	}

	return(CreateArgstring(BooleanMappings[OldState],strlen(BooleanMappings[OldState])));
}

STRPTR
RexxProtocol(struct RexxPkt *Pkt)
{
	enum	{	ARG_PROTOCOL_NONE,ARG_PROTOCOL_RTSCTS,ARG_PROTOCOL_RTSCTSDTR };

	LONG OldMode;
	LONG Mode;

	if(Args[ARG_PROTOCOL_NONE])
		Mode = HANDSHAKING_NONE;

	if(Args[ARG_PROTOCOL_RTSCTS])
		Mode = HANDSHAKING_RTSCTS;

	if(Args[ARG_PROTOCOL_RTSCTSDTR])
		Mode = HANDSHAKING_RTSCTS_DSR;

	OldMode = Config->SerialConfig->HandshakingProtocol;

	if(Config->SerialConfig->HandshakingProtocol != Mode)
	{
		Config->SerialConfig->HandshakingProtocol = Mode;

		UpdateRequired = TRUE;

		ConfigChanged = TRUE;
	}

	return(CreateArgstring(HandshakingMappings[OldMode],strlen(HandshakingMappings[OldMode])));
}

STRPTR
RexxPutClip(struct RexxPkt *Pkt)
{
	enum	{	ARG_PUTCLIP_UNIT,ARG_PUTCLIP_TEXT };

	struct IFFHandle *Handle;
	LONG Unit,Error;

	if(Args[ARG_PUTCLIP_UNIT])
		Unit = *(LONG *)Args[ARG_PUTCLIP_UNIT];
	else
		Unit = Config->ClipConfig->ClipboardUnit;

	if(Handle = OpenIFFClip(Unit,MODE_NEWFILE))
	{
		if(!(Error = PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN)))
		{
			if(!(Error = PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN)))
			{
				LONG Len = strlen(Args[ARG_PUTCLIP_TEXT]);

				if(WriteChunkBytes(Handle,Args[ARG_PUTCLIP_TEXT],Len) == Len)
					Error = PopChunk(Handle);
				else
					Error = IoErr();
			}
		}

		if(!Error)
			Error = PopChunk(Handle);

		CloseIFFClip(Handle);
	}
	else
		Error = IoErr();

	if(Error)
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = Error;
	}

	return(NULL);
}

STRPTR
RexxQuit(struct RexxPkt *Pkt)
{
	enum	{	ARG_QUIT_FORCE };

	struct DataMsg *Msg;

	if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
	{
		Pkt->RexxMsg->rm_Result1 = RC_WARN;
		Pkt->RexxMsg->rm_Result2 = 0;

		ReplyMsg((struct Message *)Pkt->RexxMsg);

		Pkt->RexxMsg = NULL;

		Msg->Type = DATAMSGTYPE_MENU;
		Msg->Size = MEN_QUIT;

		if(Args[ARG_QUIT_FORCE])
			Msg->Data = (APTR)SHIFT_KEY;
		else
			Msg->Data = NULL;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_NO_FREE_STORE;
	}

	return(NULL);
}

STRPTR
RexxRedial(struct RexxPkt *Pkt)
{
	struct DataMsg *Msg;

	if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
	{
		Msg->Type = DATAMSGTYPE_REDIAL;

		PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
	}
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxRequestNotify(struct RexxPkt *Pkt)
{
	enum	{	ARG_REQUESTNOTIFY_TITLE,ARG_REQUESTNOTIFY_PROMPT };

	struct EasyStruct Easy;

	Easy.es_StructSize		= sizeof(struct EasyStruct);
	Easy.es_Flags			= 0;
	Easy.es_TextFormat		= Args[ARG_REQUESTNOTIFY_PROMPT];
	Easy.es_GadgetFormat	= LocaleString(MSG_GLOBAL_CONTINUE_TXT);

	if(Args[ARG_REQUESTNOTIFY_TITLE])
		Easy.es_Title = Args[ARG_REQUESTNOTIFY_TITLE];
	else
		Easy.es_Title = LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);

	BlockWindows();

	EasyRequestArgs(Window,&Easy,NULL,NULL);

	ReleaseWindows();

	return(NULL);
}

STRPTR
RexxRequestNumber(struct RexxPkt *Pkt)
{
	enum	{	ARG_REQUESTNUMBER_DEFAULT,ARG_REQUESTNUMBER_PROMPT };

	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];

	if(Args[ARG_REQUESTNUMBER_DEFAULT])
		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%ld",*(LONG *)Args[ARG_REQUESTNUMBER_DEFAULT]);
	else
		DummyBuffer[0] = 0;

	BlockWindows();

	if(GetString(FALSE,FALSE,sizeof(DummyBuffer)-1,Args[ARG_REQUESTNUMBER_PROMPT],DummyBuffer))
	{
		STRPTR Index = DummyBuffer;

		while(*Index == ' ' || *Index == '\t')
			Index++;

		if(*Index)
		{
			LONG Value;

			if(StrToLong(DummyBuffer,&Value) == -1)
			{
				ResultCode[0] = RC_ERROR;
				ResultCode[1] = ERROR_BAD_NUMBER;
			}
			else
			{
				ReleaseWindows();

				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%ld",Value);

				return(CreateResult(DummyBuffer,ResultCode));
			}
		}
		else
			ResultCode[0] = RC_WARN;
	}
	else
		ResultCode[0] = RC_WARN;

	ReleaseWindows();

	return(NULL);
}

STRPTR
RexxRequestResponse(struct RexxPkt *Pkt)
{
	enum	{	ARG_REQUESTRESPONSE_TITLE,ARG_REQUESTRESPONSE_OPTIONS,
				ARG_REQUESTRESPONSE_PROMPT
			};

	struct EasyStruct Easy;
	LONG Result;

	Easy.es_StructSize	= sizeof(struct EasyStruct);
	Easy.es_Flags		= 0;
	Easy.es_TextFormat	= Args[ARG_REQUESTRESPONSE_PROMPT];

	if(Args[ARG_REQUESTRESPONSE_OPTIONS])
		Easy.es_GadgetFormat = Args[ARG_REQUESTRESPONSE_OPTIONS];
	else
		Easy.es_GadgetFormat = LocaleString(MSG_GLOBAL_YES_NO_TXT);

	if(Args[ARG_REQUESTRESPONSE_TITLE])
		Easy.es_Title = Args[ARG_REQUESTRESPONSE_TITLE];
	else
		Easy.es_Title = LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);

	BlockWindows();

	Result = EasyRequestArgs(Window,&Easy,NULL,NULL);

	ReleaseWindows();

	if(Result)
	{
		UBYTE DummyBuffer[20];

		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%ld",Result);

		return(CreateResult(DummyBuffer,ResultCode));
	}
	else
	{
		ResultCode[0] = RC_WARN;

		return(NULL);
	}
}

STRPTR
RexxRequestString(struct RexxPkt *Pkt)
{
	enum	{	ARG_REQUESTSTRING_SECRET,ARG_REQUESTSTRING_DEFAULT,ARG_REQUESTSTRING_PROMPT };

	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];

	if(Args[ARG_REQUESTSTRING_DEFAULT])
		strcpy(DummyBuffer,Args[ARG_REQUESTSTRING_DEFAULT]);
	else
		DummyBuffer[0] = 0;

	BlockWindows();

	if(GetString(FALSE,Args[ARG_REQUESTSTRING_SECRET] != NULL,sizeof(DummyBuffer) - 1,Args[ARG_REQUESTSTRING_PROMPT],DummyBuffer))
	{
		ReleaseWindows();

		return(CreateResult(DummyBuffer,ResultCode));
	}
	else
		ResultCode[0] = RC_WARN;

	ReleaseWindows();

	return(NULL);
}

STRPTR
RexxReset(struct RexxPkt *Pkt)
{
	enum	{	ARG_CLEAR,ARG_STYLE,ARG_TEXT,ARG_TIMER };

	if(Args[ARG_CLEAR])
		RexxResetScreen(Pkt);

	if(Args[ARG_STYLE])
		RexxResetStyles(Pkt);

	if(Args[ARG_TEXT])
		RexxResetText(Pkt);

	if(Args[ARG_TIMER])
		RexxResetTimer(Pkt);

	return(NULL);
}

STRPTR
RexxResetScreen(struct RexxPkt *UnusedPkt)
{
	ConResetTerminal();

	return(NULL);
}

STRPTR
RexxResetStyles(struct RexxPkt *UnusedPkt)
{
	ConResetStyles();

	return(NULL);
}

STRPTR
RexxResetText(struct RexxPkt *UnusedPkt)
{
	ConResetFont();

	return(NULL);
}

STRPTR
RexxResetTimer(struct RexxPkt *UnusedPkt)
{
	Forbid();

	if(StatusProcess)
		Signal((struct Task *)StatusProcess,SIG_RESETTIME);

	Permit();

	return(NULL);
}

STRPTR
RexxSave(struct RexxPkt *Pkt)
{
	enum	{	ARG_SAVE_FROM };

	Args[1] = Args[ARG_SAVE_FROM];
	Args[0] = NULL;

	return(RexxSaveAs(Pkt));
}

STRPTR
RexxSend(struct RexxPkt *Pkt)
{
	enum	{	ARG_SEND_NOECHO,ARG_SEND_LOCAL,ARG_SEND_LITERAL,ARG_SEND_BYTE,ARG_SEND_TEXT };

	if(Args[ARG_SEND_LOCAL])
	{
		if(!Args[ARG_SEND_NOECHO])
		{
			if(Marking)
				WindowMarkerStop();

			if(Args[ARG_SEND_TEXT])
			{
				if(Args[ARG_SEND_LITERAL])
					ConProcess(Args[ARG_SEND_TEXT],strlen(Args[ARG_SEND_TEXT]));
				else
					ConsoleCommand(Args[ARG_SEND_TEXT]);
			}
			else
			{
				UBYTE Byte = *(LONG *)Args[ARG_SEND_BYTE];

				ConProcess(&Byte,1);
			}
		}
	}
	else
	{
		BOOL OldSettings;

		OldSettings = SetConsoleQuiet((BOOL)(Args[ARG_SEND_NOECHO] != NULL));

		if(Args[ARG_SEND_TEXT])
		{
			if(Args[ARG_SEND_LITERAL])
				SerWrite(Args[ARG_SEND_TEXT],strlen(Args[ARG_SEND_TEXT]));
			else
				SerialCommand(Args[ARG_SEND_TEXT]);
		}
		else
		{
			UBYTE Byte = *(LONG *)Args[ARG_SEND_BYTE];

			SerWrite(&Byte,1);
		}

		SetConsoleQuiet(OldSettings);
	}

	return(NULL);
}

STRPTR
RexxSendBreak(struct RexxPkt *Pkt)
{
	if(WriteRequest)
		SendBreak();
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxSpeak(struct RexxPkt *Pkt)
{
	enum	{	ARG_SPEAK_TEXT };

	if(SpeechConfig.Enabled && English)
		Say(Args[ARG_SPEAK_TEXT]);
	else
		ResultCode[0] = RC_WARN;

	return(NULL);
}

STRPTR
RexxStopBits(struct RexxPkt *Pkt)
{
	enum	{	ARG_STOPBITS_0,ARG_STOPBITS_1 };

	LONG Bits;
	UBYTE Number[10];

	LimitedSPrintf(sizeof(Number),Number,"%ld",Config->SerialConfig->StopBits);

	if(Args[ARG_STOPBITS_0])
		Bits = 0;

	if(Args[ARG_STOPBITS_1])
		Bits = 1;

	if(Config->SerialConfig->StopBits != Bits)
	{
		Config->SerialConfig->StopBits = Bits;

		UpdateRequired = TRUE;

		ConfigChanged = TRUE;
	}

	return(CreateArgstring(Number,strlen(Number)));
}

STRPTR
RexxTextBuffer(struct RexxPkt *Pkt)
{
	enum	{	ARG_TEXTBUFFER_LOCK,ARG_TEXTBUFFER_UNLOCK };

	Forbid();

	if(Args[ARG_TEXTBUFFER_LOCK])
		BufferFrozen = TRUE;

	if(Args[ARG_TEXTBUFFER_UNLOCK])
		BufferFrozen = FALSE;

	ConOutputUpdate();

	Permit();

	CheckItem(MEN_FREEZE_BUFFER,BufferFrozen);

	return(NULL);
}

STRPTR
RexxTimeout(struct RexxPkt *Pkt)
{
	enum	{	ARG_TIMEOUT_SECONDS,ARG_TIMEOUT_OFF };

	if(Args[ARG_TIMEOUT_OFF])
		RexxTimeoutVal = 0;
	else
		RexxTimeoutVal = *(LONG *)Args[ARG_TIMEOUT_SECONDS];

	return(NULL);

}

STRPTR
RexxTrap(struct RexxPkt *Pkt)
{
	enum	{	ARG_TRAP_ON,ARG_TRAP_OFF };

	BOOL Enabled;

	if(Args[ARG_TRAP_ON])
		Enabled = TRUE;

	if(Args[ARG_TRAP_OFF])
		Enabled = FALSE;

	CheckItem(MEN_DISABLE_TRAPS,Enabled != TRUE);

	if(GenericListCount(GenericListTable[GLIST_TRAP]) != 0 && Enabled)
		WatchTraps = TRUE;
	else
		WatchTraps = FALSE;

	return(NULL);
}

STRPTR
RexxWindow(struct RexxPkt *Pkt)
{
	enum	{	ARG_WINDOW_NAMES,ARG_WINDOW_OPEN,ARG_WINDOW_CLOSE,ARG_WINDOW_ACTIVATE,
				ARG_WINDOW_MIN,ARG_WINDOW_MAX,ARG_WINDOW_FRONT,ARG_WINDOW_BACK,
				ARG_WINDOW_TOP,ARG_WINDOW_BOTTOM,ARG_WINDOW_UP,ARG_WINDOW_DOWN
			};

	STRPTR	*Names = (STRPTR *)Args[ARG_WINDOW_NAMES];
	LONG	 Index;

	while(*Names)
	{
		if((Index = ToWindow(*Names++)) != -1)
		{
			if(Args[ARG_WINDOW_OPEN])
			{
				Forbid();

				switch(Index)
				{
					case WINDOWID_BUFFER:

						LaunchBuffer();
						break;

					case WINDOWID_UPLOAD_QUEUE:

						CreateQueueProcess();

						break;

					case WINDOWID_REVIEW:

						if(!ReviewWindow)
							CreateReview();

						break;

					case WINDOWID_PACKET:

						if(!PacketWindow)
							CreatePacketWindow();

						break;

					case WINDOWID_FASTMACROS:

						if(!FastWindow)
							OpenFastWindow();

						break;

					case WINDOWID_STATUS:

						if(!InfoWindow)
							OpenInfoWindow();

						break;

					case WINDOWID_MAIN:

						if(!IconTerminated)
							IconTerminated = TRUE;

						break;

					case WINDOWID_SINGLE_CHAR_ENTRY:

						if(Window)
							OpenMatrixWindow(Window);

						break;
				}

				Permit();
			}

			if(Args[ARG_WINDOW_CLOSE])
			{
				Forbid();

				switch(Index)
				{
					case WINDOWID_UPLOAD_QUEUE:

						CloseQueueWindow();
						break;

					case WINDOWID_BUFFER:

						TerminateBuffer();
						break;

					case WINDOWID_REVIEW:

						if(ReviewWindow)
							DeleteReview();

						break;

					case WINDOWID_PACKET:

						if(PacketWindow)
							DeletePacketWindow(FALSE);

						break;

					case WINDOWID_FASTMACROS:

						if(FastWindow)
							CloseFastWindow();

						break;

					case WINDOWID_STATUS:

						if(InfoWindow)
							CloseInfoWindow();

						break;

					case WINDOWID_MAIN:

						if(Window)
							ActivateJob(MainJobQueue,IconifyJob);

						break;

					case WINDOWID_SINGLE_CHAR_ENTRY:

						CloseMatrixWindow();
						break;
				}

				Permit();
			}

			if(Args[ARG_WINDOW_ACTIVATE])
			{
				Forbid();

				switch(Index)
				{
					case WINDOWID_UPLOAD_QUEUE:

						CreateQueueProcess();

						break;

					case WINDOWID_BUFFER:

						LaunchBuffer();
						break;

					case WINDOWID_REVIEW:

						if(ReviewWindow)
							BumpWindow(ReviewWindow);

						break;

					case WINDOWID_PACKET:

						if(PacketWindow)
							BumpWindow(PacketWindow);

						break;

					case WINDOWID_FASTMACROS:

						if(FastWindow)
							BumpWindow(FastWindow);

						break;

					case WINDOWID_STATUS:

						if(InfoWindow)
							BumpWindow(InfoWindow);

						break;

					case WINDOWID_MAIN:

						if(Window)
							BumpWindow(Window);

						break;

					case WINDOWID_SINGLE_CHAR_ENTRY:

						if(MatrixWindow)
							BumpWindow(MatrixWindow);

						break;
				}

				Permit();
			}

			if(Args[ARG_WINDOW_MIN])
			{
				struct Window *SomeWindow = NULL;

				Forbid();

				switch(Index)
				{
					case WINDOWID_REVIEW:

						SomeWindow = ReviewWindow;
						break;

					case WINDOWID_PACKET:

						SomeWindow = PacketWindow;
						break;

					case WINDOWID_FASTMACROS:

						SomeWindow = FastWindow;
						break;

					case WINDOWID_STATUS:

						SomeWindow = InfoWindow;
						break;
				}

				if(SomeWindow)
					ChangeWindowBox(SomeWindow,SomeWindow->LeftEdge,SomeWindow->TopEdge,SomeWindow->MinWidth,SomeWindow->MinHeight);

				Permit();
			}

			if(Args[ARG_WINDOW_MAX])
			{
				struct Window *SomeWindow = NULL;

				Forbid();

				switch(Index)
				{
					case WINDOWID_REVIEW:

						SomeWindow = ReviewWindow;
						break;

					case WINDOWID_PACKET:

						SomeWindow = PacketWindow;
						break;

					case WINDOWID_FASTMACROS:

						SomeWindow = FastWindow;
						break;

					case WINDOWID_STATUS:

						SomeWindow = InfoWindow;
						break;
				}

				if(SomeWindow)
					ChangeWindowBox(SomeWindow,0,0,SomeWindow->MaxWidth,SomeWindow->MaxHeight);

				Permit();
			}

			if(Args[ARG_WINDOW_FRONT])
			{
				Forbid();

				switch(Index)
				{
					case WINDOWID_UPLOAD_QUEUE:

						CreateQueueProcess();

						break;

					case WINDOWID_BUFFER:

						LaunchBuffer();
						break;

					case WINDOWID_REVIEW:

						if(ReviewWindow)
							WindowToFront(ReviewWindow);

						break;

					case WINDOWID_PACKET:

						if(PacketWindow)
							WindowToFront(PacketWindow);

						break;

					case WINDOWID_FASTMACROS:

						if(FastWindow)
							WindowToFront(FastWindow);

						break;

					case WINDOWID_STATUS:

						if(InfoWindow)
							WindowToFront(InfoWindow);

						break;

					case WINDOWID_MAIN:

						if(Window)
							WindowToFront(Window);

						break;

					case WINDOWID_SINGLE_CHAR_ENTRY:

						if(MatrixWindow)
							WindowToFront(MatrixWindow);

						break;
				}

				Permit();
			}

			if(Args[ARG_WINDOW_BACK])
			{
				Forbid();

				switch(Index)
				{
					case WINDOWID_REVIEW:

						if(ReviewWindow)
							WindowToBack(ReviewWindow);

						break;

					case WINDOWID_PACKET:

						if(PacketWindow)
							WindowToBack(PacketWindow);

						break;

					case WINDOWID_FASTMACROS:

						if(FastWindow)
							WindowToBack(FastWindow);

						break;

					case WINDOWID_STATUS:

						if(InfoWindow)
							WindowToBack(InfoWindow);

						break;

					case WINDOWID_MAIN:

						if(Window)
							WindowToBack(Window);

						break;

					case WINDOWID_SINGLE_CHAR_ENTRY:

						if(MatrixWindow)
							WindowToBack(MatrixWindow);

						break;
				}

				Permit();
			}

			Forbid();

			if(Index == WINDOW_REVIEW && ReviewWindow)
			{
				if(Args[ARG_WINDOW_TOP])
					MoveReview(REVIEW_MOVE_TOP);

				if(Args[ARG_WINDOW_BOTTOM])
					MoveReview(REVIEW_MOVE_BOTTOM);

				if(Args[ARG_WINDOW_UP])
					MoveReview(REVIEW_MOVE_UP);

				if(Args[ARG_WINDOW_DOWN])
					MoveReview(REVIEW_MOVE_DOWN);
			}

			Permit();
		}
	}

	return(NULL);
}

STATIC VOID
RexxLaunchCleanup(LaunchMsg *Startup)
{
	struct RexxPkt *Pkt = Startup->RexxPkt;

	ResultCode[0] = Startup->Result;
	ResultCode[1] = Startup->Result2;

	RexxPktCleanup(Pkt,NULL);
}

STRPTR
RexxRX(struct RexxPkt *Pkt)
{
	enum	{	ARG_RX_CONSOLE,ARG_RX_ASYNC,ARG_RX_COMMAND };

	LaunchMsg	*Startup;
	BOOL		 Eaten;

	if(!(Startup = CreateRexxCmdLaunchMsg(Args[ARG_RX_COMMAND],Pkt,Args[ARG_RX_ASYNC] ? NULL : RexxLaunchCleanup)))
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERR10_013;

		RexxPktCleanup(Pkt,NULL);

		return(NULL);
	}

	if(Args[ARG_RX_ASYNC])
	{
		RexxPktCleanup(Pkt,NULL);

		Eaten = TRUE;
	}
	else
		Eaten = FALSE;

	if(LaunchSomething(Args[ARG_RX_CONSOLE],!Eaten,Startup))
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = IoErr();
	}
	else
	{
		if(Eaten)
			DeleteLaunchMsg(Startup);
	}

	if(!Eaten)
		RexxPktCleanup(Pkt,NULL);

	return(NULL);
}

STRPTR
RexxExecTool(struct RexxPkt *Pkt)
{
	enum	{	ARG_EXECTOOL_CONSOLE,ARG_EXECTOOL_ASYNC,ARG_EXECTOOL_PORT,ARG_EXECTOOL_COMMAND };

	UBYTE		 CommandName[MAX_FILENAME_LENGTH];
	UBYTE		 CommandArgs[MAX_FILENAME_LENGTH];
	STRPTR		 Index = Args[ARG_EXECTOOL_COMMAND];
	LONG		 i;
	BOOL		 Eaten;
	LaunchMsg	*Startup;

	while(*Index == ' ' || *Index == '\t')
		Index++;

	for(i = 0 ; Index[i] != ' ' && Index[i] != '\t' ; i++)
		CommandName[i] = Index[i];

	CommandName[i] = 0;

	Index += i;

	while(*Index == ' ' || *Index == '\t')
		Index++;

	if(Args[ARG_EXECTOOL_PORT])
		LimitedSPrintf(sizeof(CommandArgs),CommandArgs,"%s %s\n",Index,RexxPortName);
	else
		LimitedSPrintf(sizeof(CommandArgs),CommandArgs,"%s\n",Index);

	LimitedStrcat(sizeof(CommandName),CommandName," ");
	LimitedStrcat(sizeof(CommandName),CommandName,CommandArgs);

	if(!(Startup = CreateProgramLaunchMsg(CommandName,NULL)))
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERR10_013;

		RexxPktCleanup(Pkt,NULL);

		return(NULL);
	}

	if(Args[ARG_EXECTOOL_ASYNC])
	{
		RexxPktCleanup(Pkt,NULL);

		Eaten = TRUE;
	}
	else
		Eaten = FALSE;

	if(LaunchSomething(Args[ARG_EXECTOOL_CONSOLE],FALSE,Startup))
	{
		if(!Eaten)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = IoErr();
		}
	}
	else
		DeleteLaunchMsg(Startup);

	if(!Eaten)
		RexxPktCleanup(Pkt,NULL);

	return(NULL);
}

STRPTR
RexxRead(struct RexxPkt *Pkt)
{
	enum	{	ARG_READ_NUM,ARG_READ_CR,ARG_READ_NOECHO,ARG_READ_VERBATIM,
				ARG_TIMEOUT,ARG_TERMINATOR,ARG_READ_PROMPT
			};

	LONG MaxBytesToRead;
	LONG BytesRead;
	ULONG Signals;
	STRPTR Buffer;
	STRPTR Result;
	LONG Timeout;
	BOOL Done;
	BOOL Echo;
	WORD Terminator;

	if(!ReadRequest || !WriteRequest)
	{
		ResultCode[0] = RC_WARN;

		return(NULL);
	}

	if(Args[ARG_READ_NUM])
	{
		MaxBytesToRead = *(LONG *)Args[ARG_READ_NUM] + 1;

		if(MaxBytesToRead < 1)
		{
			ResultCode[0] = RC_ERROR;
			ResultCode[1] = ERROR_BAD_NUMBER;

			return(NULL);
		}

		if(MaxBytesToRead > MAX_RESULT_LEN + 1)
			MaxBytesToRead = MAX_RESULT_LEN + 1;
	}
	else
		MaxBytesToRead = MAX_RESULT_LEN + 1;

	if(!(Buffer = (STRPTR)AllocVecPooled(MaxBytesToRead,MEMF_ANY)))
	{
		ResultCode[0] = RC_ERROR;
		ResultCode[1] = ERROR_NO_FREE_STORE;

		return(NULL);
	}

	if(Args[ARG_READ_PROMPT])
		SerialCommand(Args[ARG_READ_PROMPT]);

	if(Args[ARG_READ_NOECHO])
		Echo = FALSE;
	else
		Echo = TRUE;

	if(Args[ARG_TIMEOUT])
		Timeout = *(LONG *)Args[ARG_TIMEOUT];
	else
		Timeout = RexxTimeoutVal;

	Terminator = -1;

	if(Args[ARG_TERMINATOR])
	{
		UBYTE LocalBuffer[256];

		if(TranslateString(Args[ARG_TERMINATOR],LocalBuffer) > 0)
			Terminator = LocalBuffer[0];
	}

	if(Timeout)
		StartTime(Timeout,0);

	Done = FALSE;
	BytesRead = 0;

	if(Args[ARG_READ_VERBATIM] || !ReceiveTable)
	{
		do
		{
			Signals = (*SerialWaitForData)(SIG_WINDOW | SIG_BREAK | SIG_TIMER);

			if(Signals & SIG_WINDOW)
				while(RunJob(WindowJob));

			if(Signals & SIG_SERIAL)
			{
				ULONG Length;

				if(Length = (*SerialGetWaiting)())
				{
					if(Length > SerialBufferSize / 2)
						Length = SerialBufferSize / 2;

					if(Length > Config->SerialConfig->Quantum)
						Length = Config->SerialConfig->Quantum;

					if(Length = (*SerialRead)(ReadBuffer,Length))
					{
						BytesIn += Length;

						if(Translate_CR_LF)
							Length = (*Translate_CR_LF)(ReadBuffer,Length);

						if(Length)
						{
							UBYTE *BufferPtr,c;

							BufferPtr = ReadBuffer;

							while(!Done && Length > 0)
							{
								Length--;

								c = *BufferPtr++;

								if(c == Terminator)
								{
									if(Echo)
										ConProcess(&c,1);

									Done = TRUE;
									break;
								}

								switch(c)
								{
									case '\n':

										if(Echo)
											ConProcess(&c,1);

										break;

									case '\r':

										if(Args[ARG_READ_CR])
											Done = TRUE;

										if(Echo)
											ConProcess(&c,1);

										break;

									case '\b':

										if(BytesRead > 0)
										{
											BytesRead--;

											if(Echo)
												ConProcess(&c,1);
										}

										break;

									case CONTROL_('X'):

										if(Echo)
										{
											while(BytesRead > 0)
											{
												BytesRead--;

												ConProcess("\b",1);
											}
										}
										else
											BytesRead = 0;

										break;

									default:

										if(BytesRead < MaxBytesToRead && ((c >= ' ' && c < 127) || c >= 160 || Config->TerminalConfig->FontMode == FONT_IBM))
										{
											Buffer[BytesRead++] = c;

											if(Echo)
												ConProcess(&c,1);
										}

										break;
								}
							}
						}
					}
				}
			}

			if(Signals & (SIG_BREAK | SIG_TIMER))
			{
				ResultCode[0] = RC_WARN;

				Done = TRUE;
			}
		}
		while(!Done);
	}
	else
	{
		do
		{
			Signals = (*SerialWaitForData)(SIG_WINDOW | SIG_BREAK | SIG_TIMER);

			if(Signals & SIG_WINDOW)
				while(RunJob(WindowJob));

			if(Signals & SIG_SERIAL)
			{
				ULONG Length;

				if(Length = (*SerialGetWaiting)())
				{
					if(Length > SerialBufferSize / 2)
						Length = SerialBufferSize / 2;

					if(Length > Config->SerialConfig->Quantum)
						Length = Config->SerialConfig->Quantum;

					if(Length = (*SerialRead)(ReadBuffer,Length))
					{
						BytesIn += Length;

						if(Translate_CR_LF)
							Length = (*Translate_CR_LF)(ReadBuffer,Length);

						if(Length)
						{
							struct TranslationHandle Handle;
							UBYTE c;

							TranslateSetup(&Handle,ReadBuffer,Length,&c,1,ReceiveTable);

							while(!Done && TranslateBuffer(&Handle))
							{
								if(c == Terminator)
								{
									if(Echo)
										ConProcess(&c,1);

									Done = TRUE;
									break;
								}

								switch(c)
								{
									case '\n':

										if(Echo)
											ConProcess(&c,1);

										break;

									case '\r':

										if(Args[ARG_READ_CR])
											Done = TRUE;

										if(Echo)
											ConProcess(&c,1);

										break;

									case '\b':

										if(BytesRead > 0)
										{
											BytesRead--;

											if(Echo)
												ConProcess(&c,1);
										}

										break;

									case CONTROL_('X'):

										if(Echo)
										{
											while(BytesRead > 0)
											{
												BytesRead--;

												ConProcess("\b",1);
											}
										}
										else
											BytesRead = 0;

										break;

									default:

										if(BytesRead < MaxBytesToRead && ((c >= ' ' && c < 127) || c >= 160 || Config->TerminalConfig->FontMode == FONT_IBM))
										{
											Buffer[BytesRead++] = c;

											if(Echo)
												ConProcess(&c,1);
										}

										break;
								}
							}
						}
					}
				}
			}

			if(Signals & (SIG_BREAK | SIG_TIMER))
			{
				ResultCode[0] = RC_WARN;

				Done = TRUE;
			}
		}
		while(!Done);
	}

	StopTime();

	if(BytesRead > 0)
	{
		Buffer[BytesRead] = 0;

		Result = CreateResult(Buffer,ResultCode);
	}
	else
	{
		ResultCode[0] = RC_WARN;
		Result = NULL;
	}

	FreeVecPooled(Buffer);

	return(Result);
}
