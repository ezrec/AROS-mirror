/*
**	Transfer.c
**
**	File transfer routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Prototypes for local routines. */

STATIC VOID CommonStartReceive(LONG Type, STRPTR Name, BOOL WaitForIt);
STATIC BOOL CommonStartSend(LONG Type, STRPTR TheFile, struct FileTransferInfo **TransferInfoPtr, BOOL WaitForIt);
STATIC BOOL SelectProtocol(struct Window *ParentWindow);
STATIC BOOL StartSend(BOOL IsBinary);
STATIC BOOL StartReceive(BOOL IsBinary);
STATIC VOID StripLibrary(STRPTR Protocol, STRPTR NameBuffer, LONG NameSize);
STATIC BOOL IsZModem(STRPTR String);
STATIC VOID CommonExternalSetup(STRPTR Buffer);
STATIC BOOL CommonXPRSetup(STRPTR ProtocolName, BOOL IgnoreOptions);
STATIC struct Node *CreateArgName(STRPTR String);
STATIC STRPTR BuildString(STRPTR Source, STRPTR From, STRPTR To, BYTE Type, BOOL ReceiveMode, STRPTR SingleFile, struct FileTransferInfo *Info, LONG *Error);
STATIC STRPTR ReplaceName(STRPTR Source);
STATIC VOID CommonStartSendXPR(LONG Type, BOOL IsBinary, BOOL WaitForIt);
STATIC VOID CommonSendXPRCleanup(BOOL WaitForIt);
STATIC VOID CommonStartReceiveXPR(LONG Type, BOOL IsBinary, BOOL WaitForIt);
STATIC LONG TransferCompare(struct FileTransferNode **A, struct FileTransferNode **B);

	/* CommonStartReceive():
	 *
	 *	Receive files via XPR or a different protocol.
	 */

STATIC VOID
CommonStartReceive(LONG Type,STRPTR Name,BOOL WaitForIt)
{
	STRPTR From = NULL,To = NULL,Cmd = NULL;
	BOOL IsBinary;
	LONG Mode = 0;

	if(Type == TRANSFER_ASCII && Config->TransferConfig->ASCIIDownloadType == XFER_INTERNAL)
	{
		InternalASCIIDownload(Name,WaitForIt);

		return;
	}

	IsBinary = FALSE;

	switch(Type)
	{
		case TRANSFER_BINARY:

			Cmd		= Config->TransferConfig->BinaryDownloadLibrary;
			Mode	= Config->TransferConfig->BinaryDownloadType;
			From	= Config->PathConfig->BinaryUploadPath;
			To		= Config->PathConfig->BinaryDownloadPath;

			IsBinary = TRUE;
			break;

		case TRANSFER_TEXT:

			Cmd		= Config->TransferConfig->TextDownloadLibrary;
			Mode	= Config->TransferConfig->TextDownloadType;
			From	= Config->PathConfig->TextUploadPath;
			To		= Config->PathConfig->TextDownloadPath;
			break;

		case TRANSFER_ASCII:

			Cmd		= Config->TransferConfig->ASCIIDownloadLibrary;
			Mode	= Config->TransferConfig->ASCIIDownloadType;
			From	= Config->PathConfig->ASCIIUploadPath;
			To		= Config->PathConfig->ASCIIDownloadPath;
			break;
	}

	if(Mode == XFER_DEFAULT && Config->TransferConfig->DefaultType == XFER_EXTERNALPROGRAM)
	{
		Cmd		= Config->TransferConfig->DefaultLibrary;
		Mode	= XFER_EXTERNALPROGRAM;
	}

	if(Mode == XFER_EXTERNALPROGRAM)
	{
		STRPTR	String;
		LONG	Error;

		BlockWindows();

		if(String = BuildString(Cmd,From,To,Type,TRUE,Name,NULL,&Error))
		{
			ClearSerial();

			LaunchCommand(String);

			RestartSerial();

			FreeVecPooled(String);
		}

		ReleaseWindows();
	}
	else
	{
		struct FileRequester	*FileRequest;
		UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
		BPTR					 NewDir = BNULL,
								 OldDir = BNULL;

		ClearGenericList(GenericListTable[GLIST_DOWNLOAD],FALSE);

			/* Select the download path. */

		switch(Type)
		{
			case TRANSFER_BINARY:

				DownloadPath = Config->PathConfig->BinaryDownloadPath;
				break;

			case TRANSFER_TEXT:

				DownloadPath = Config->PathConfig->TextDownloadPath;
				break;

			case TRANSFER_ASCII:

				DownloadPath = Config->PathConfig->ASCIIDownloadPath;
				break;
		}

		if(DownloadPath[0])
		{
			if(NewDir = Lock(DownloadPath,ACCESS_READ))
				OldDir = CurrentDir(NewDir);
		}

		BlockWindows();

			/* Set up the library if necessary. */

		if(!XProtocolBase)
		{
			if(SelectProtocol(Window))
			{
				if(ProtocolSetup(FALSE))
					SaveProtocolOpts();
			}
		}

		if(XProtocolBase)
		{
			SetTransferMenu(TRUE);

				/* Do we need to ask the user for
				 * the destination file name?
				 */

			if(TransferBits & XPRS_NORECREQ)
				CommonStartReceiveXPR(Type,IsBinary,WaitForIt);
			else
			{
				if(!Name)
				{
					LimitedStrcpy(sizeof(DummyBuffer),DummyBuffer,DownloadPath);

					if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + (Type - TRANSFER_BINARY)),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
					{
						FreeAslRequest(FileRequest);

							/* Save the download path. */

						strcpy(DownloadPath,DummyBuffer);
						*PathPart(DownloadPath) = 0;

						ConfigChanged = TRUE;

							/* Install the name of the file to receive. */

						XprIO->xpr_filename = DummyBuffer;

						Name = DummyBuffer;
					}
				}
				else
				{
					STRPTR Index;

					strcpy(DownloadPath,Name);

					ConfigChanged = TRUE;

					Index = PathPart(DownloadPath);

					*Index = 0;

					XprIO->xpr_filename = Name;
				}

					/* Download the file(s). */

				if(Name)
					CommonStartReceiveXPR(Type,IsBinary,WaitForIt);
			}
		}
		else
			SetTransferMenu(TRUE);

		if(NewDir)
			UnLock(CurrentDir(OldDir));

		if(SendAbort)
		{
			CancelZModem();
			SendAbort = FALSE;
		}

		ReleaseWindows();

		DownloadPath = NULL;

		DidTransfer = FALSE;

		if(WaitForIt)
		{
			if(Config->CommandConfig->DownloadMacro[0])
				SerialCommand(Config->CommandConfig->DownloadMacro);
		}
	}
}

	/* CommonStartSend():
	 *
	 *	Send files via XPR or a different protocol.
	 */

STATIC BOOL
CommonStartSend(LONG Type,STRPTR TheFile,struct FileTransferInfo **TransferInfoPtr,BOOL WaitForIt)
{
	STRPTR From = NULL,To = NULL,Name = NULL;
	BOOL IsBinary;
	LONG Mode = 0;

	if(TheFile)
		TheFile = ReplaceName(TheFile);

	if(Type == TRANSFER_ASCII && Config->TransferConfig->ASCIIDownloadType == XFER_INTERNAL)
		return(InternalASCIIUpload(TheFile,WaitForIt));

	IsBinary = FALSE;

	switch(Type)
	{
		case TRANSFER_BINARY:

			Name	= Config->TransferConfig->BinaryUploadLibrary;
			Mode	= Config->TransferConfig->BinaryUploadType;
			From	= Config->PathConfig->BinaryUploadPath;
			To		= Config->PathConfig->BinaryDownloadPath;

			IsBinary = TRUE;
			break;

		case TRANSFER_TEXT:

			Name	= Config->TransferConfig->TextUploadLibrary;
			Mode	= Config->TransferConfig->TextUploadType;
			From	= Config->PathConfig->TextUploadPath;
			To		= Config->PathConfig->TextDownloadPath;
			break;

		case TRANSFER_ASCII:

			Name	= Config->TransferConfig->ASCIIUploadLibrary;
			Mode	= Config->TransferConfig->ASCIIUploadType;
			From	= Config->PathConfig->ASCIIUploadPath;
			To		= Config->PathConfig->ASCIIDownloadPath;
			break;
	}

	if(Mode == XFER_DEFAULT && Config->TransferConfig->DefaultType == XFER_EXTERNALPROGRAM)
	{
		Name	= Config->TransferConfig->DefaultLibrary;
		Mode	= XFER_EXTERNALPROGRAM;
	}

	if(Mode == XFER_EXTERNALPROGRAM)
	{
		STRPTR	String;
		LONG	Error;
		BOOL	Result;

		BlockWindows();

		if(String = BuildString(Name,From,To,Type,FALSE,TheFile,TransferInfoPtr ? *TransferInfoPtr : NULL,&Error))
		{
			ClearSerial();

			Result = LaunchCommand(String);

			RestartSerial();

			if(TransferInfoPtr)
			{
				FreeFileTransferInfo(*TransferInfoPtr);

				*TransferInfoPtr = NULL;
			}

			FreeVecPooled(String);
		}
		else
			Result = FALSE;

		ReleaseWindows();

		return(Result);
	}
	else
	{
		struct FileRequester	*FileRequest;
		UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
		STRPTR					 UploadPath = "";
		BOOL					 DidSend = TRUE;
		BPTR					 NewDir = BNULL,
								 OldDir = BNULL;

			/* We are uploading data. */

		Uploading = TRUE;

			/* Select the upload path. */

		switch(Type)
		{
			case TRANSFER_BINARY:

				UploadPath = Config->PathConfig->BinaryUploadPath;
				break;

			case TRANSFER_TEXT:

				UploadPath = Config->PathConfig->TextUploadPath;
				break;

			case TRANSFER_ASCII:

				UploadPath = Config->PathConfig->ASCIIUploadPath;
				break;
		}

		if(UploadPath[0])
		{
			if(NewDir = Lock(UploadPath,ACCESS_READ))
				OldDir = CurrentDir(NewDir);
		}

		BlockWindows();

			/* If not initialized, try to set up a new
			 * external transfer protocol.
			 */

		if(!XProtocolBase)
		{
			if(SelectProtocol(Window))
			{
				if(ProtocolSetup(FALSE))
					SaveProtocolOpts();
			}
		}

		if(XProtocolBase)
		{
			SetTransferMenu(TRUE);

				/* Do we need to use our own file requester or
				 * will xpr handle this job for us?
				 */

			if((TransferBits & XPRS_NOSNDREQ) || TheFile || TransferInfoPtr)
			{
				if(TheFile)
					XprIO->xpr_filename = TheFile;

				if(TransferInfoPtr)
					XprIO->xpr_filename = (*TransferInfoPtr)->CurrentFile->Name;

				CommonStartSendXPR(Type,IsBinary,WaitForIt);

				if(TransferInfoPtr)
				{
					FreeFileTransferInfo(*TransferInfoPtr);

					*TransferInfoPtr = NULL;
				}
			}
			else
			{
					/* We will need the file requester to find
					 * out which file(s) are to be transferred.
					 * Multiple files and wildcards are
					 * supported as well as plain file names.
					 */

				DummyBuffer[0] = 0;

				if(FileRequest = OpenSeveralFiles(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + (Type - TRANSFER_BINARY)),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
				{
					strcpy(UploadPath,FileRequest->fr_Drawer);

					ConfigChanged = TRUE;

					if(FileTransferInfo = BuildFileTransferInfo(FileRequest))
					{
							/* Make sure that at least the
							 * first file gets transferred
							 * in case the protocol does
							 * no support batch upload.
							 */

						XprIO->xpr_filename = ReplaceName(FileTransferInfo->CurrentFile->Name);

						CommonStartSendXPR(Type,IsBinary,WaitForIt);

						if(FileTransferInfo)
						{
							FreeFileTransferInfo(FileTransferInfo);

							FileTransferInfo = NULL;
						}
					}

					FreeAslRequest(FileRequest);
				}
				else
					DidSend = FALSE;
			}
		}
		else
			SetTransferMenu(TRUE);

		if(NewDir)
			UnLock(CurrentDir(OldDir));

		CommonSendXPRCleanup(WaitForIt);

		return(DidSend);
	}
}

	/* SelectProtocol(STRPTR Name,struct Window *ParentWindow):
	 *
	 *	Select a different transfer protocol library using
	 *	the asl.library file requester.
	 */

STATIC BOOL
SelectProtocol(struct Window *ParentWindow)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

	strcpy(LocalBuffer,LastXprLibrary);

	if(PickFile(ParentWindow,"Libs:","xpr#?.library",LocaleString(MSG_TERMXPR_SELECT_TRANSFER_PROTOCOL_TXT),LocalBuffer,NT_LIBRARY))
	{
		if(Stricmp(LocalBuffer,LastXprLibrary))
		{
			strcpy(LastXprLibrary,LocalBuffer);

			return(TRUE);
		}
	}

	return(FALSE);
}

	/* StartSend(BOOL IsBinary):
	 *
	 *	Start sending files with the XPR protocol.
	 */

STATIC BOOL
StartSend(BOOL IsBinary)
{
	if(ReadRequest && WriteRequest)
	{
		BOOL Failed;

		BinaryTransfer = IsBinary;

		if(XProtocolSend(XprIO))
			Failed = FALSE;
		else
			Failed = TRUE;

			/* Always return to binary transfer mode */

		BinaryTransfer = TRUE;

			/* In case the transfer has been aborted,
			 * flush the input buffer of dirty data.
			 */

		if(TransferAborted)
			CompletelyFlushSerialRead();

		return(Failed);
	}
	else
		return(TRUE);
}

	/* StartReceive(BOOL IsBinary):
	 *
	 *	Start receiving files with the XPR protocol.
	 */

STATIC BOOL
StartReceive(BOOL IsBinary)
{
	if(ReadRequest && WriteRequest)
	{
		BOOL Failed;

		BinaryTransfer = IsBinary;

		if(XProtocolReceive(XprIO))
			Failed = FALSE;
		else
			Failed = TRUE;

			/* Always return to binary transfer mode */

		BinaryTransfer = TRUE;

		return(Failed);
	}
	else
		return(TRUE);
}

	/* StripLibrary(STRPTR Protocol,STRPTR NameBuffer):
	 *
	 *	Strip the .library part from a file name.
	 */

STATIC VOID
StripLibrary(STRPTR Protocol,STRPTR NameBuffer,LONG NameSize)
{
	LONG i;

	CopyMem(FilePart(Protocol),NameBuffer,NameSize - 1);
	NameBuffer[NameSize - 1] = 0;

	for(i = strlen(NameBuffer) - 1 ; i >= 0 ; i--)
	{
		if(NameBuffer[i] == '.')
		{
			NameBuffer[i] = 0;
			break;
		}
	}
}

	/* IsZModem(STRPTR String):
	 *
	 *	Checks if a file name has a "zmodem" string embedded.
	 */

STATIC BOOL
IsZModem(STRPTR String)
{
	LONG i;

	for(i = 0 ; i <= strlen(String) - 6 ; i++)
	{
		if(!Stricmp(&String[i],"zmodem"))
			return(TRUE);
	}

	return(FALSE);
}

	/* CommonExternalSetup(STRPTR Buffer):
	 *
	 *	Set up the name of an external file transfer protocol.
	 */

STATIC VOID
CommonExternalSetup(STRPTR Buffer)
{
	UBYTE NameBuffer[MAX_FILENAME_LENGTH];
	LONG i;

	while(*Buffer == ' ')
		Buffer++;

	if(!Strnicmp(Buffer,"run",3))
		Buffer += 3;

	while(*Buffer == ' ')
		Buffer++;

	CopyMem(FilePart(Buffer),NameBuffer,sizeof(NameBuffer) - 1);
	NameBuffer[sizeof(NameBuffer) - 1] = 0;

	for(i = 0 ; i < sizeof(NameBuffer) ; i++)
	{
		if(NameBuffer[i] == ' ')
		{
			NameBuffer[i] = 0;
			break;
		}
	}

	NameBuffer[0] = ToUpper(NameBuffer[0]);
	strcpy(TransferProtocolName,NameBuffer);

	UsesZModem = IsZModem(NameBuffer);

	SetTransferMenu(TRUE);
}

	/* CommonXPRSetup(STRPTR ProtocolName,BOOL IgnoreOptions):
	 *
	 *	Set up and initialize an XPR file transfer protocol.
	 */

STATIC BOOL
CommonXPRSetup(STRPTR ProtocolName,BOOL IgnoreOptions)
{
	UBYTE NameBuffer[MAX_FILENAME_LENGTH];
	BOOL Result;

		/* Clear the XPR interface buffer. */

	memset(XprIO,0,sizeof(struct XPR_IO));

		/* Copy the name of the library. */

	StripLibrary(ProtocolName,NameBuffer,sizeof(NameBuffer));

		/* Check if the transfer protocol is a sort of ZModem. */

	UsesZModem = IsZModem(NameBuffer);

		/* Reset the scanner. */

	ResetDataFlowFilter();

		/* Obtain the protocol default settings. */

	if(IgnoreOptions)
		ProtocolOptsBuffer[0] = 0;
	else
		GetEnvDOS(NameBuffer,ProtocolOptsBuffer,sizeof(ProtocolOptsBuffer));

	NameBuffer[3] = ToUpper(NameBuffer[3]);

		/* Initialize the interface structure. */

	XprIO->xpr_filename		= ProtocolOptsBuffer;
	XprIO->xpr_extension	= 4;

	#ifdef USE_GLUE
		XprIO->xpr_fopen		= xpr_fopen_glue;
		XprIO->xpr_fclose		= xpr_fclose_glue;
		XprIO->xpr_fread		= xpr_fread_glue;
		XprIO->xpr_fwrite		= xpr_fwrite_glue;
		XprIO->xpr_sread		= xpr_sread_glue;
		XprIO->xpr_swrite		= xpr_swrite_glue;
		XprIO->xpr_sflush		= xpr_sflush;
		XprIO->xpr_update		= xpr_update_glue;
		XprIO->xpr_chkabort		= xpr_chkabort;
		XprIO->xpr_gets			= xpr_gets_glue;
		XprIO->xpr_setserial	= xpr_setserial_glue;
		XprIO->xpr_ffirst		= xpr_ffirst_glue;
		XprIO->xpr_fnext		= xpr_fnext_glue;
		XprIO->xpr_finfo		= xpr_finfo_glue;
		XprIO->xpr_fseek		= xpr_fseek_glue;
		XprIO->xpr_options		= xpr_options_glue;
		XprIO->xpr_unlink		= xpr_unlink_glue;
		XprIO->xpr_squery		= xpr_squery;
		XprIO->xpr_getptr		= xpr_getptr_glue;
	#else
		XprIO->xpr_fopen		= xpr_fopen;
		XprIO->xpr_fclose		= xpr_fclose;
		XprIO->xpr_fread		= xpr_fread;
		XprIO->xpr_fwrite		= xpr_fwrite;
		XprIO->xpr_sread		= xpr_sread;
		XprIO->xpr_swrite		= xpr_swrite;
		XprIO->xpr_sflush		= xpr_sflush;
		XprIO->xpr_update		= xpr_update;
		XprIO->xpr_chkabort		= xpr_chkabort;
		XprIO->xpr_gets			= xpr_gets;
		XprIO->xpr_setserial	= xpr_setserial;
		XprIO->xpr_ffirst		= xpr_ffirst;
		XprIO->xpr_fnext		= xpr_fnext;
		XprIO->xpr_finfo		= xpr_finfo;
		XprIO->xpr_fseek		= xpr_fseek;
		XprIO->xpr_options		= xpr_options;
		XprIO->xpr_unlink		= xpr_unlink;
		XprIO->xpr_squery		= xpr_squery;
		XprIO->xpr_getptr		= xpr_getptr;
	#endif	/* USE_GLUE */

		/* No success so far */

	Result = FALSE;

		/* Try to open the library. */

	if(XProtocolBase = (struct Library *)OpenLibrary(ProtocolName,0))
	{
			/* Set up the library. */

		ClearSerial();

		TransferBits = XProtocolSetup(XprIO);

		SerWriteUpdate();

		RestartSerial();

		DeleteTransferPanel(TRUE);

			/* Successful initialization? */

		if(TransferBits & XPRS_SUCCESS)
		{
			strcpy(LastXprLibrary,ProtocolName);

			if(TransferBits & XPRS_HOSTNOWAIT)
			{
				if(!HostReadBuffer)
					HostReadBuffer = AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY);
			}
			else
			{
				FreeVecPooled(HostReadBuffer);
				HostReadBuffer = NULL;
			}

			strcpy(TransferProtocolName,&NameBuffer[3]);

			Result = TRUE;
		}
	}

	if(!Result)
	{
		ShowRequest(Window,LocaleString(MSG_GLOBAL_FAILED_TO_SET_UP_PROTOCOL_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),FilePart(ProtocolName));

		CloseLibrary(XProtocolBase);
		XProtocolBase = NULL;

		SerWriteUpdate();

		LastXprLibrary[0] = 0;
		TransferBits = 0;

		TransferProtocolName[0] = 0;
	}

	ConTransferUpdate();

	SetTransferMenu(TRUE);

	return(Result);
}

	/* CreateArgName(STRPTR String):
	 *
	 *	Create another node based upon a string given.
	 *	Encloses parameters in quotes if necessary and
	 *	`escapes' quotes.
	 */

STATIC struct Node *
CreateArgName(STRPTR String)
{
	struct Node	*Node;
	STRPTR		 Index = String;
	LONG		 Len,NewLen;
	BOOL 		 Quotes = FALSE;

	Len = NewLen = strlen(String);

	while(*Index)
	{
		if(*Index == '\"')
			NewLen++;

		if(*Index == ' ' && !Quotes)
		{
			NewLen += 2;

			Quotes = TRUE;
		}

		Index++;
	}

	if(!Len)
	{
		NewLen += 2;

		Quotes = TRUE;
	}

	if(NewLen > Len)
	{
		if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + NewLen + 1,MEMF_ANY))
		{
			STRPTR Dest;

			Node->ln_Name = (STRPTR)(Node + 1);

			Dest	= Node->ln_Name;
			Index	= String;

			if(Quotes)
				*Dest++ = '\"';

			while(*Index)
			{
				if(*Index == '\"')
					*Dest++ = '*';

				*Dest++ = *Index++;
			}

			if(Quotes)
				*Dest++ = '\"';

			*Dest = 0;
		}
	}
	else
	{
		if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + Len + 1,MEMF_ANY))
			strcpy(Node->ln_Name = (STRPTR)(Node + 1),String);
	}

	return(Node);
}

	/* BuildString():
	 *
	 *	Build a command line based upon a template and
	 *	existing file transfer info.
	 */

STATIC STRPTR
BuildString(STRPTR Source,STRPTR From,STRPTR To,BYTE Type,BOOL ReceiveMode,STRPTR SingleFile,struct FileTransferInfo *Info,LONG *Error)
{
	struct List	*List;
	STRPTR Result = NULL;

	if(Info)
	{
		if(!IsListEmpty((struct List *)Info))
			Info->CurrentFile = (struct FileTransferNode *)Info->FileList.mlh_Head;
		else
			Info = NULL;
	}

	*Error = 0;

	if(List = CreateList())
	{
		UBYTE LocalBuffer[MAX_FILENAME_LENGTH],TempBuffer[MAX_FILENAME_LENGTH];
		struct FileRequester *FileRequest;
		STRPTR Index,Dest;
		struct Node *Node = NULL;

		Index = Source;
		Dest = LocalBuffer;

		while(*Index && !(*Error))
		{
			if(*Index == '%')
			{
				struct Node	*InsertHere = List->lh_TailPred;
				BOOL InsertBefore = TRUE;

				Index++;

				if(!(*Index))
				{
					*Dest = 0;

					if(LocalBuffer[0])
					{
						if(Node = CreateNode(LocalBuffer))
						{
							AddTail(List,Node);

							LocalBuffer[0] = 0;
						}
						else
							*Error = ERROR_NO_FREE_STORE;
					}

					break;
				}

				switch(*Index)
				{
					case 'f':

						if(SingleFile)
						{
							if(Node = CreateArgName(SingleFile))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							SingleFile = NULL;

							break;
						}

						if(Info)
						{
							if(Node = CreateArgName(Info->CurrentFile->Name))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							if(Info->CurrentFile->MinNode.mln_Succ)
								Info->CurrentFile = (struct FileTransferNode *)Info->CurrentFile->MinNode.mln_Succ;
							else
								Info = NULL;

							break;
						}

						LT_LockWindow(Window);

						if(ReceiveMode)
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,To);

							FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}
						else
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,From);

							FileRequest = OpenSingleFile(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}

						if(FileRequest)
						{
							if(Node = CreateArgName(TempBuffer))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							FreeAslRequest(FileRequest);
						}
						else
							*Error = ERR_ABORTED;

						LT_UnlockWindow(Window);

						break;

					case 'F':

						if(SingleFile)
						{
							if(Node = CreateArgName(FilePart(SingleFile)))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							SingleFile = NULL;

							break;
						}

						if(Info)
						{
							if(Node = CreateArgName(FilePart(Info->CurrentFile->Name)))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							if(Info->CurrentFile->MinNode.mln_Succ)
								Info->CurrentFile = (struct FileTransferNode *)Info->CurrentFile->MinNode.mln_Succ;
							else
								Info = NULL;

							break;
						}

						LT_LockWindow(Window);

						if(ReceiveMode)
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,To);

							FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}
						else
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,From);

							FileRequest = OpenSingleFile(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}

						if(FileRequest)
						{
							if(Node = CreateArgName(FilePart(TempBuffer)))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							FreeAslRequest(FileRequest);
						}
						else
							*Error = ERR_ABORTED;

						LT_UnlockWindow(Window);

						break;

					case 'm':

						if(SingleFile)
						{
							if(Node = CreateArgName(SingleFile))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							SingleFile = NULL;

							break;
						}

						if(Info)
						{
							struct FileTransferNode *SomeNode;

							for(SomeNode = Info->CurrentFile ; SomeNode->MinNode.mln_Succ && !(*Error) ; SomeNode = (struct FileTransferNode *)SomeNode->MinNode.mln_Succ)
							{
								if(Node = CreateArgName(SomeNode->Name))
									AddTail(List,Node);
								else
									*Error = ERROR_NO_FREE_STORE;
							}

							Info = NULL;

							break;
						}

						LT_LockWindow(Window);

						if(ReceiveMode)
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,To);

							FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}
						else
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,From);

							FileRequest = OpenSeveralFiles(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}

						LT_UnlockWindow(Window);

						if(FileRequest)
						{
							LONG i;

							if(TempBuffer[0] && FileRequest->fr_NumArgs == 1)
							{
								if(Node = CreateArgName(TempBuffer))
								{
									AddTail(List,Node);

									break;
								}
								else
									*Error = ERROR_NO_FREE_STORE;
							}

							for(i = 0 ; i < FileRequest->fr_NumArgs && !(*Error) ; i++)
							{
								if(FileRequest->fr_ArgList[i].wa_Lock)
								{
									if(NameFromLock(FileRequest->fr_ArgList[i].wa_Lock,TempBuffer,sizeof(TempBuffer)))
									{
										if(AddPart(TempBuffer,FileRequest->fr_ArgList[i].wa_Name,sizeof(TempBuffer)))
										{
											if(Node = CreateArgName(TempBuffer))
												AddTail(List,Node);
											else
												*Error = ERROR_NO_FREE_STORE;
										}
										else
											*Error = IoErr();
									}
									else
										*Error = IoErr();
								}
								else
								{
									LimitedStrcpy(sizeof(TempBuffer),TempBuffer,FileRequest->fr_Drawer);

									if(AddPart(TempBuffer,FileRequest->fr_ArgList[i].wa_Name,sizeof(TempBuffer)))
									{
										if(Node = CreateArgName(TempBuffer))
											AddTail(List,Node);
										else
											*Error = ERROR_NO_FREE_STORE;
									}
									else
										*Error = IoErr();
								}
							}

							FreeAslRequest(FileRequest);
						}
						else
							*Error = ERR_ABORTED;

						break;

					case 'M':

						if(SingleFile)
						{
							if(Node = CreateArgName(FilePart(SingleFile)))
								AddTail(List,Node);
							else
								*Error = ERROR_NO_FREE_STORE;

							SingleFile = NULL;

							break;
						}

						if(Info)
						{
							struct FileTransferNode *SomeNode;

							for(SomeNode = Info->CurrentFile ; SomeNode->MinNode.mln_Succ && !(*Error) ; SomeNode = (struct FileTransferNode *)SomeNode->MinNode.mln_Succ)
							{
								if(Node = CreateArgName(FilePart(SomeNode->Name)))
									AddTail(List,Node);
								else
									*Error = ERROR_NO_FREE_STORE;
							}

							Info = NULL;

							break;
						}

						LT_LockWindow(Window);

						if(ReceiveMode)
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,To);

							FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}
						else
						{
							LimitedStrcpy(sizeof(TempBuffer),TempBuffer,From);

							FileRequest = OpenSeveralFiles(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + Type),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,TempBuffer,sizeof(TempBuffer));
						}

						LT_UnlockWindow(Window);

						if(FileRequest)
						{
							LONG i;

							if(TempBuffer[0] && FileRequest->fr_NumArgs == 1)
							{
								if(Node = CreateArgName(FilePart(TempBuffer)))
								{
									AddTail(List,Node);

									break;
								}
								else
									*Error = ERROR_NO_FREE_STORE;
							}

							for(i = 0 ; i < FileRequest->fr_NumArgs && !(*Error) ; i++)
							{
								if(FileRequest->fr_ArgList[i].wa_Lock)
								{
									if(AddPart(TempBuffer,FileRequest->fr_ArgList[i].wa_Name,sizeof(TempBuffer)))
									{
										if(Node = CreateArgName(TempBuffer))
											AddTail(List,Node);
										else
											*Error = ERROR_NO_FREE_STORE;
									}
									else
										*Error = IoErr();
								}
								else
								{
									if(Node = CreateArgName(FileRequest->fr_ArgList[i].wa_Name))
										AddTail(List,Node);
									else
										*Error = ERROR_NO_FREE_STORE;
								}
							}

							FreeAslRequest(FileRequest);
						}
						else
							*Error = ERR_ABORTED;

						break;

					case 'b':
					case 'B':

						if(Node = CreateNodeArgs("%ld",Config->SerialConfig->BaudRate))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case 'c':
					case 'C':

						if(Node = CreateNodeArgs("%ld",DTERate ? DTERate : Config->SerialConfig->BaudRate))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case 'p':
					case 'P':

						if(Node = CreateArgName(RexxPortName))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case 'd':
					case 'D':

						if(Node = CreateArgName(Config->SerialConfig->SerialDevice))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case 'u':
					case 'U':

						if(Node = CreateNodeArgs("%ld",Config->SerialConfig->UnitNumber))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case '<':

						if(Node = CreateArgName(From))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case '>':

						if(Node = CreateArgName(To))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					case 's':
					case 'S':

						if(!GetPubScreenName(Window->WScreen,TempBuffer))
							TempBuffer[0] = 0;

						if(Node = CreateArgName(TempBuffer))
							AddTail(List,Node);
						else
							*Error = ERROR_NO_FREE_STORE;

						break;

					default:

						*Dest = *Index;

						InsertBefore = FALSE;

						break;
				}

				if(InsertBefore && Node)
				{
					struct Node *OtherNode;

					*Dest = 0;

					if(LocalBuffer[0])
					{
						if(OtherNode = CreateNode(LocalBuffer))
						{
							Insert(List,OtherNode,InsertHere);

							LocalBuffer[0] = 0;
						}
						else
							*Error = ERROR_NO_FREE_STORE;
					}

					Dest = LocalBuffer;
				}
			}
			else
				*Dest++ = *Index;

			Index++;
		}

		*Dest = 0;

		if(LocalBuffer[0] && !(*Error))
		{
			if(Node = CreateNode(LocalBuffer))
				AddTail(List,Node);
			else
				*Error = ERROR_NO_FREE_STORE;
		}

		if(!(*Error))
		{
			if(!IsListEmpty(List))
			{
				LONG Total = 0,Len;

				for(Node = List->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
					Total += strlen(Node->ln_Name) + 1;

				if(Result = (STRPTR)AllocVecPooled(Total,MEMF_ANY))
				{
					STRPTR Buffer = Result;

					for(Node = List->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
					{
						Len = strlen(Node->ln_Name);

						CopyMem(Node->ln_Name,Buffer,Len);

						Buffer += Len;

						*Buffer++ = ' ';
					}

					Buffer[-1] = 0;
				}
			}
			else
				Result = "";
		}

		DeleteList(List);
	}
	else
		*Error = ERROR_NO_FREE_STORE;

	return(Result);
}

	/* ReplaceName(STRPTR Original):
	 *
	 *	Replace a filename with a mangled version for
	 *	file uploads.
	 */

STATIC STRPTR
ReplaceName(STRPTR Source)
{
	if(Config->TransferConfig->MangleFileNames)
	{
		UBYTE LocalName[MAX_FILENAME_LENGTH],*Char;

		LimitedStrcpy(sizeof(OriginalName),OriginalName,Source);

		ShrinkName(FilePart(Source),LocalName,12,TRUE);

		LimitedStrcpy(sizeof(ShrunkenName),ShrunkenName,Source);

		Char = PathPart(ShrunkenName);

		*Char = 0;

		if(AddPart(ShrunkenName,LocalName,sizeof(ShrunkenName)))
			return(ShrunkenName);
	}

	OriginalName[0] = 0;
	return(Source);
}

	/* CommonStartSendXPR():
	 *
	 *	Start sending files using the XPR protocol.
	 */

STATIC VOID
CommonStartSendXPR(LONG Type,BOOL IsBinary,BOOL WaitForIt)
{
	PushStatus(STATUS_UPLOAD);

	if(TransferPanel(LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + (Type - TRANSFER_BINARY))))
	{
		ClearSerial();

		LogAction(LocaleString(MSG_TERMTRANSFER_LOGMSG_INITIATE_UPLOAD_TXT),LocaleString(MSG_TERMTRANSFER_BINARY_TXT + (Type - TRANSFER_BINARY)));

		TransferFailed = StartSend(IsBinary);

			/* Tell the user what happened */

		if(TransferFailed || TransferAborted)
			Say(LocaleString(MSG_GLOBAL_TRANSFER_FAILED_OR_ABORTED_TXT));
		else
			Say(LocaleString(MSG_GLOBAL_TRANSFER_COMPLETED_TXT));

		if(TransferFailed || TransferError)
		{
			if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
				WakeUp(TransferWindow,SOUND_BADTRANSFER);

			DeleteTransferPanel(WaitForIt);
		}
		else
		{
			if(TransferWindow)
			{
				if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
					WakeUp(TransferWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteTransferPanel(FALSE);
		}

		RestartSerial();
	}

	PopStatus();
}

	/* CommonSendXPRCleanup(BOOL WaitForIt):
	 *
	 *	Do the cleanup work after a file transfer has finished.
	 */

STATIC VOID
CommonSendXPRCleanup(BOOL WaitForIt)
{
	if(SendAbort)
	{
		CancelZModem();
		SendAbort = FALSE;
	}

	ReleaseWindows();

	Uploading = FALSE;

	if(WaitForIt)
	{
		if(Config->CommandConfig->UploadMacro[0])
			SerialCommand(Config->CommandConfig->UploadMacro);
	}

	DidTransfer = FALSE;
}

	/* CommonStartReceiveXPR():
	 *
	 *	Start receiving files using the XPR protocol.
	 */

STATIC VOID
CommonStartReceiveXPR(LONG Type,BOOL IsBinary,BOOL WaitForIt)
{
	PushStatus(STATUS_DOWNLOAD);

		/* Open the transfer panel. */

	if(TransferPanel(LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + (Type - TRANSFER_BINARY))))
	{
		ClearSerial();

		LogAction(LocaleString(MSG_TERMTRANSFER_LOGMSG_INITIATE_DOWNLOAD_TXT),LocaleString(MSG_TERMTRANSFER_BINARY_TXT + (Type - TRANSFER_BINARY)));

			/* Receive the file. */

		TransferFailed = StartReceive(IsBinary);

		if(TransferAborted || TransferFailed)
			Say(LocaleString(MSG_GLOBAL_TRANSFER_FAILED_OR_ABORTED_TXT));
		else
			Say(LocaleString(MSG_GLOBAL_TRANSFER_COMPLETED_TXT));

		if(TransferFailed || TransferError)
		{
			if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
				WakeUp(TransferWindow,SOUND_BADTRANSFER);

			DeleteTransferPanel(WaitForIt);
		}
		else
		{
			if(TransferWindow)
			{
				if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
					WakeUp(TransferWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteTransferPanel(FALSE);
		}

			/* Queue another read
			 * request.
			 */

		RestartSerial();
	}

	PopStatus();
}

	/* TransferCompare(struct FileTransferNode **A,struct FileTransferNode **B):
	 *
	 *	Local subroutine required by QuickSort().
	 */

STATIC LONG
TransferCompare(struct FileTransferNode **A,struct FileTransferNode **B)
{
	if((*A)->Size > (*B)->Size)
		return(-1);
	else
	{
		if((*A)->Size < (*B)->Size)
			return(1);
		else
			return(Stricmp(FilePart((*A)->Name),FilePart((*B)->Name)));
	}
}

	/* FreeFileTransferInfo(struct FileTransferInfo *Info):
	 *
	 *	Free a file transfer info list as allocated
	 *	by AllocFileTransferInfo() and AddFileTransferNode().
	 */

VOID
FreeFileTransferInfo(struct FileTransferInfo *Info)
{
	if(Info)
	{
		struct FileTransferNode *Node,*Next;

		for(Node = (struct FileTransferNode *)Info->FileList.mlh_Head ; Next = (struct FileTransferNode *)Node->MinNode.mln_Succ ; Node = Next)
			FreeVecPooled(Node);

		FreeVecPooled(Info);
	}
}

	/* AllocFileTransferInfo():
	 *
	 *	Allocate a FileTransferInfo structure for use with
	 *	AddFileTransferNode().
	 */

struct FileTransferInfo *
AllocFileTransferInfo()
{
	struct FileTransferInfo *Info;

	if(Info = (struct FileTransferInfo *)AllocVecPooled(sizeof(struct FileTransferInfo),MEMF_ANY | MEMF_CLEAR))
	{
		NewList((struct List *)&Info->FileList);

		return(Info);
	}

	return(NULL);
}

	/* AddFileTransferNode(struct FileTransferInfo *Info,STRPTR Name,ULONG Size):
	 *
	 *	Allocate a file transfer information node.
	 */

BOOL
AddFileTransferNode(struct FileTransferInfo *Info,STRPTR Name,ULONG Size)
{
	struct FileTransferNode *Node;

	if(Node = (struct FileTransferNode *)AllocVecPooled(sizeof(struct FileTransferNode) + strlen(Name) + 1,MEMF_ANY))
	{
		Node->Size	= Size;
		Node->Name	= (STRPTR)(Node + 1);

		strcpy(Node->Name,Name);

		AddTail((struct List *)&Info->FileList,(struct Node *)Node);

		Info->TotalSize += Size;

		Info->TotalFiles++;

		return(TRUE);
	}
	else
		return(FALSE);
}

	/* ReadyFileTransferInfo(struct FileTransferInfo *Info,BOOL SortIt):
	 *
	 *	Sorts the file transfer information list in ascending
	 *	order, but makes sure that the files are sorted
	 *	in descending order (i.e. the larger files come
	 *	first and files of equal size are sorted
	 *	lexically).
	 */

VOID
ReadyFileTransferInfo(struct FileTransferInfo *Info,BOOL SortIt)
{
	if(Info->TotalFiles > 1 && SortIt)
	{
		struct FileTransferNode **NodeTable;

		if(NodeTable = (struct FileTransferNode **)AllocVecPooled(sizeof(struct FileTransferNode *) * Info->TotalFiles,MEMF_ANY))
		{
			struct FileTransferNode *Node;
			LONG i;

			for(Node = (struct FileTransferNode *)Info->FileList.mlh_Head, i = 0 ; Node->MinNode.mln_Succ ; Node = (struct FileTransferNode *)Node->MinNode.mln_Succ, i++)
				NodeTable[i] = Node;

			qsort(NodeTable,Info->TotalFiles,sizeof(struct FileTransferNode *),(SORTFUNC)TransferCompare);

			NewList((struct List *)&Info->FileList);

			for(i = 0 ; i < Info->TotalFiles ; i++)
				AddTail((struct List *)&Info->FileList,(struct Node *)NodeTable[i]);

			FreeVecPooled(NodeTable);
		}
	}

	Info->DoneSize	= 0;
	Info->DoneFiles	= 0;

	Info->CurrentFile	= (struct FileTransferNode *)Info->FileList.mlh_Head;
	Info->CurrentSize	= Info->CurrentFile->Size;
}

	/* BuildFileTransferInfo(struct FileRequester *FileRequester):
	 *
	 *	Build a file transfer information list from
	 *	information provided by a FileRequester structure.
	 */

struct FileTransferInfo *
BuildFileTransferInfo(struct FileRequester *FileRequester)
{
	struct FileTransferInfo *Info;
	LONG FilesFound;

	FilesFound = 0;

	if(Info = AllocFileTransferInfo())
	{
		BOOL Success = FALSE;
		BPTR NewLock,OldLock;

		if(NewLock = Lock(FileRequester->fr_Drawer,ACCESS_READ))
		{
			OldLock = CurrentDir(NewLock);

			if(FileRequester->fr_NumArgs)
			{
				struct WBArg *ArgList;
				BPTR FileLock;
				LONG i;
				D_S(struct FileInfoBlock,FileInfo);

				ArgList = FileRequester->fr_ArgList;
				Success = TRUE;

				for(i = 0 ; Success && i < FileRequester->fr_NumArgs ; i++)
				{
					if(ArgList[i].wa_Name)
					{
						if(ArgList[i].wa_Lock)
						{
							CurrentDir(ArgList[i].wa_Lock);

							if(FileLock = Lock(ArgList[i].wa_Name,ACCESS_READ))
							{
								if(Examine(FileLock,FileInfo))
								{
									if(FileInfo->fib_DirEntryType < 0)
									{
										UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

										if(NameFromLock(FileLock,LocalBuffer,sizeof(LocalBuffer)))
										{
											if(!AddFileTransferNode(Info,LocalBuffer,FileInfo->fib_Size))
												Success = FALSE;
											else
												FilesFound++;
										}
									}
								}

								UnLock(FileLock);
							}

							CurrentDir(NewLock);
						}
						else
						{
							if(FileLock = Lock(ArgList[i].wa_Name,ACCESS_READ))
							{
								if(Examine(FileLock,FileInfo))
								{
									if(FileInfo->fib_DirEntryType < 0)
									{
										UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

										if(NameFromLock(FileLock,LocalBuffer,sizeof(LocalBuffer)))
										{
											if(!AddFileTransferNode(Info,LocalBuffer,FileInfo->fib_Size))
												Success = FALSE;
											else
												FilesFound++;
										}
									}
								}

								UnLock(FileLock);
							}
						}
					}
				}
			}
			else
			{
				struct AnchorPath *Anchor;
				STRPTR FileName;

				if(FileRequester->fr_NumArgs > 1 && FileRequester->fr_ArgList)
					FileName = FileRequester->fr_ArgList->wa_Name;
				else
					FileName = FileRequester->fr_File;

				if(Anchor = (struct AnchorPath *)AllocVecPooled(sizeof(struct AnchorPath),MEMF_ANY | MEMF_CLEAR))
				{
					if(!MatchFirst(FileName,Anchor))
					{
						Success = TRUE;

						if(Anchor->ap_Info.fib_DirEntryType < 0)
						{
							UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

							if(NameFromLock(NewLock,LocalBuffer,sizeof(LocalBuffer)))
							{
								if(AddPart(LocalBuffer,Anchor->ap_Info.fib_FileName,sizeof(LocalBuffer)))
								{
									if(!AddFileTransferNode(Info,LocalBuffer,Anchor->ap_Info.fib_Size))
										Success = FALSE;
									else
										FilesFound++;
								}
							}
						}

						if(Success)
						{
							while(!MatchNext(Anchor))
							{
								UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

								if(NameFromLock(NewLock,LocalBuffer,sizeof(LocalBuffer)))
								{
									if(AddPart(LocalBuffer,Anchor->ap_Info.fib_FileName,sizeof(LocalBuffer)))
									{
										if(!AddFileTransferNode(Info,LocalBuffer,Anchor->ap_Info.fib_Size))
										{
											Success = FALSE;

											break;
										}
										else
											FilesFound++;
									}
								}
							}
						}

						if(IoErr() != ERROR_NO_MORE_ENTRIES)
							Success = FALSE;
					}

					MatchEnd(Anchor);

					FreeVecPooled(Anchor);
				}
			}

			UnLock(CurrentDir(OldLock));
		}

		if(Success && FilesFound)
		{
			ReadyFileTransferInfo(Info,TRUE);

			return(Info);
		}
		else
			FreeFileTransferInfo(Info);
	}

	return(NULL);
}

VOID
StartReceiveXPR_File(LONG Type,STRPTR Name,BOOL WaitForIt)
{
	CommonStartReceive(Type,Name,WaitForIt);
}

VOID
StartReceiveXPR_AskForFile(LONG Type,BOOL WaitForIt)
{
	CommonStartReceive(Type,NULL,WaitForIt);
}

BOOL
StartSendXPR_File(LONG Type,STRPTR TheFile,BOOL WaitForIt)
{
	return(CommonStartSend(Type,TheFile,NULL,WaitForIt));
}

BOOL
StartSendXPR_AskForFile(LONG Type,BOOL WaitForIt)
{
	return(CommonStartSend(Type,NULL,NULL,WaitForIt));
}

BOOL
StartSendXPR_FromList(LONG Type,BOOL WaitForIt)
{
	return(CommonStartSend(Type,NULL,&FileTransferInfo,WaitForIt));
}

	/* ResetProtocol():
	 *
	 *	Return to the previously selected file
	 *	transfer protocol.
	 */

VOID
ResetProtocol()
{
	ChangeProtocol(NULL,XFER_DEFAULT);
}

	/* SaveProtocolOpts():
	 *
	 *	Save the current protocol settings to an environment variable.
	 */

VOID
SaveProtocolOpts()
{
		/* It's time to save the altered options. */

	if(NewOptions && XProtocolBase)
	{
		UBYTE NameBuffer[MAX_FILENAME_LENGTH];

			/* Strip the `.library' part. */

		StripLibrary(LastXprLibrary,NameBuffer,sizeof(NameBuffer));

			/* Cause the xpr.library to prompt for
			 * input. We expect the library to fill
			 * the prompt string with the default
			 * settings. The resulting string is
			 * intercepted by xpr_stealopts, saved
			 * to an environment variable and will
			 * serve as a reinitialization string
			 * later.
			 */

		XprIO->xpr_filename		= NULL;
		XprIO->xpr_extension	= 0;

		#ifdef USE_GLUE
			XprIO->xpr_gets			= xpr_stealopts_glue;
			XprIO->xpr_options		= NULL;
		#else
			XprIO->xpr_gets			= xpr_stealopts;
			XprIO->xpr_options		= NULL;
		#endif	/* USE_GLUE */

		ClearSerial();

		XProtocolSetup(XprIO);

		DeleteTransferPanel(FALSE);

			/* Save the options in case anything goes
			 * wrong.
			 */

		NewOptions = FALSE;

		SetEnvDOS(NameBuffer,ProtocolOptsBuffer);

			/* Reinitialize the library. */

		XprIO->xpr_filename		= ProtocolOptsBuffer;
		XprIO->xpr_extension	= 4;

		#ifdef USE_GLUE
			XprIO->xpr_gets			= xpr_gets_glue;
			XprIO->xpr_options		= xpr_options_glue;
		#else
			XprIO->xpr_gets			= (APTR)xpr_gets;
			XprIO->xpr_options		= xpr_options;
		#endif	/* USE_GLUE */

		XProtocolSetup(XprIO);

		RestartSerial();

		DeleteTransferPanel(FALSE);
	}
}

	/* TransferCleanup():
	 *
	 *	We did a file transfer (auto-download?) and
	 *	will need to close the transfer window.
	 */

VOID
TransferCleanup()
{
	if(DidTransfer)
	{
		if(TransferFailed || TransferError)
		{
			if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
				WakeUp(TransferWindow,SOUND_BADTRANSFER);

			DeleteTransferPanel(TRUE);
		}
		else
		{
			if(TransferWindow)
			{
				if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
					WakeUp(TransferWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteTransferPanel(FALSE);
		}

		if(SendAbort)
		{
			CancelZModem();
			SendAbort = FALSE;
		}

		if(Config->CommandConfig->DownloadMacro[0])
			SerialCommand(Config->CommandConfig->DownloadMacro);

		Say(LocaleString(MSG_GLOBAL_TRANSFER_COMPLETED_TXT));

		DidTransfer = FALSE;
	}
	else
	{
		if(TransferFailed || TransferError)
		{
			if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
				WakeUp(TransferWindow,SOUND_BADTRANSFER);

			DeleteTransferPanel(TRUE);
		}
		else
		{
			if(TransferWindow)
			{
				if(Config->TransferConfig->TransferNotification == XFERNOTIFY_ALWAYS || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
					WakeUp(TransferWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteTransferPanel(FALSE);
		}
	}

	ReleaseWindows();
}

	/* RemoveUploadListItem(STRPTR Name):
	 *
	 *	Remove a named item from the upload list.
	 */

VOID
RemoveUploadListItem(STRPTR Name)
{
	BPTR NameLock;

	if(NameLock = Lock(Name,ACCESS_READ))
	{
		struct GenericList *List;
		struct Node *Node;
		STRPTR Base;
		BOOL Done;

		List = GenericListTable[GLIST_UPLOAD];
		Base = FilePart(Name);

		LockGenericList(List);

		for(Node = (struct Node *)List->ListHeader.mlh_Head, Done = FALSE ; !Done && Node->ln_Succ ; Node = Node->ln_Succ)
		{
			if(!Stricmp(Base,FilePart(Node->ln_Name)))
			{
				BPTR ListLock;

				if(ListLock = Lock(Node->ln_Name,ACCESS_READ))
				{
					if(SameLock(ListLock,NameLock) == LOCK_SAME)
					{
						DeleteGenericListNode(List,Node,TRUE);

						Done = TRUE;
					}

					UnLock(ListLock);
				}
			}
		}

		UnlockGenericList(List);

		UnLock(NameLock);
	}
}

	/* CloseXPR():
	 *
	 *	Close the XPR library if it is still open.
	 */

VOID
CloseXPR()
{
	if(XProtocolBase)
	{
		XProtocolCleanup(XprIO);

		CloseLibrary(XProtocolBase);
		XProtocolBase = NULL;

		SerWriteUpdate();
		ConTransferUpdate();

		SetTransferMenu(TRUE);
	}
}

	/* ChangeProtocol(STRPTR ProtocolName):
	 *
	 *	Select a different file transfer protocol.
	 */

BOOL
ChangeProtocol(STRPTR ProtocolName,LONG Type)
{
	if(Type == XFER_DEFAULT)
	{
		ProtocolName = Config->TransferConfig->DefaultLibrary;
		Type = Config->TransferConfig->DefaultType;
	}

	if(Type == XFER_XPR)
	{
		if(ProtocolName && !ProtocolName[0])
			ProtocolName = NULL;

		if(!ProtocolName)
			ProtocolName = Config->TransferConfig->DefaultLibrary;

		if(!Stricmp(ProtocolName,LastXprLibrary) && XProtocolBase)
			return(TRUE);
	}

		/* Close the old library if still open. */

	CloseXPR();

	if(Type == XFER_INTERNAL)
	{
		UsesZModem = FALSE;

		strcpy(TransferProtocolName,"ASCII");

		SetTransferMenu(TRUE);

		return(TRUE);
	}

	if(Type == XFER_EXTERNALPROGRAM)
	{
		CommonExternalSetup(ProtocolName);

		return(TRUE);
	}

	return(CommonXPRSetup(ProtocolName,FALSE));
}

	/* ProtocolSetup(BOOL IgnoreOptions):
	 *
	 *	Set up the library and options for the external protocol.
	 */

BOOL
ProtocolSetup(BOOL IgnoreOptions)
{
		/* Close the old library if still open. */

	CloseXPR();

	if(Config->TransferConfig->DefaultType == XFER_EXTERNALPROGRAM)
	{
		if(Config->TransferConfig->DefaultLibrary[0])
		{
			CommonExternalSetup(Config->TransferConfig->DefaultLibrary);

			return(TRUE);
		}
		else
			return(FALSE);
	}

	return(CommonXPRSetup(LastXprLibrary,IgnoreOptions));
}
