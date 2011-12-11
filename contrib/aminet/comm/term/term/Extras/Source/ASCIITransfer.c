/*
**	ASCIITransfer.c
**
**	ASCII file transfer routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Local variables. */

STATIC UBYTE	ASCIISendPrompt[256];
STATIC LONG		ASCIISendPromptLen;
STATIC SENDLINE	ASCIISendLine;

STATIC LONG
MangleASCIIBuffer(BYTE TranslateCR,BYTE TranslateLF,STRPTR Source,LONG SourceLen,STRPTR Destination)
{
	LONG	CR_Len = 0,
			LF_Len = 0;
	STRPTR	CR_String = NULL,
			LF_String = NULL;
	LONG	Mask;
	LONG	Len = 0;

	switch(TranslateCR)
	{
		case EOL_IGNORE:

			CR_Len = 0;
			break;

		case EOL_CR:

			CR_Len		= 1;
			CR_String	= "\r";
			break;

		case EOL_LF:

			CR_Len		= 1;
			CR_String	= "\n";
			break;

		case EOL_CRLF:

			CR_Len		= 2;
			CR_String	= "\r\n";
			break;

		case EOL_LFCR:

			CR_Len		= 2;
			CR_String	= "\n\r";
			break;
	}

	switch(TranslateLF)
	{
		case EOL_IGNORE:

			LF_Len = 0;
			break;

		case EOL_LF:

			LF_Len		= 1;
			LF_String	= "\n";
			break;

		case EOL_CR:

			LF_Len		= 1;
			LF_String	= "\r";
			break;

		case EOL_LFCR:

			LF_Len		= 2;
			LF_String	= "\n\r";
			break;

		case EOL_CRLF:

			LF_Len		= 2;
			LF_String	= "\r\n";
			break;
	}

	if(Config->TransferConfig->StripBit8)
		Mask = 0x7F;
	else
		Mask = 0xFF;

	do
	{
		switch(*Source)
		{
			case '\r':

				if(CR_Len)
				{
					if(Len >= CR_Len && Config->TransferConfig->ExpandBlankLines)
					{
						if(!memcmp(&Destination[-CR_Len],CR_String,CR_Len))
						{
							*Destination++ = ' ';
							Len++;
						}
					}

					CopyMem(CR_String,Destination,CR_Len);

					Destination	+= CR_Len;
					Len			+= CR_Len;
				}

				break;

			case '\n':

				if(LF_Len)
				{
					if(Len >= LF_Len && Config->TransferConfig->ExpandBlankLines)
					{
						if(!memcmp(&Destination[-LF_Len],LF_String,LF_Len))
						{
							*Destination++ = ' ';
							Len++;
						}
					}

					CopyMem(LF_String,Destination,LF_Len);

					Destination	+= LF_Len;
					Len			+= LF_Len;
				}

				break;

			default:

				*Destination++ = (*Source) & Mask;

				Len++;

				break;
		}

		Source++;
	}
	while(--SourceLen);

	return(Len);
}

	/* WaitForPrompt(STRPTR Prompt,LONG PromptLen):
	 *
	 *	Scan the incoming data flow for a certain string.
	 */

STATIC BOOL
WaitForPrompt(STRPTR Prompt,LONG PromptLen)
{
	ULONG Signals;
	BOOL GotIt;

		/* Reset the match data. */

	MatchPrompt(NULL,0,NULL,0);

		/* Start the timer. */

	StartTime(Config->TransferConfig->SendTimeout / 100,(Config->TransferConfig->SendTimeout % 100) * 10000);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	GotIt = FALSE;

		/* Loop until the prompt is found or the timer elapses. */

	do
	{
		Signals = (*SerialWaitForData)(SIG_TIMER);

			/* New serial data available? */

		if(Signals & SIG_SERIAL)
		{
			ULONG Length;

				/* Check how much data is available. */

			if(Length = (*SerialGetWaiting)())
			{
					/* Don't read more than the buffer will hold. */

				if(Length > SerialBufferSize)
					Length = SerialBufferSize;

					/* Read the data. */

				if(Length = (*SerialRead)(ReadBuffer,Length))
				{
						/* Got some more data. */

					BytesIn += Length;

					if(!Config->TransferConfig->QuietTransfer)
						ConProcess(ReadBuffer,Length);

						/* Check if this is it. */

					if(MatchPrompt(ReadBuffer,Length,Prompt,PromptLen))
						GotIt = TRUE;
				}
			}
		}

			/* Stop if the bell rings. */

		if(Signals & SIG_TIMER)
			break;
	}
	while(!GotIt);

		/* Stop the timer. */

	StopTime();

		/* Unlock the xOFF state. */

	Unlock_xOFF();

	return(GotIt);
}

	/* ASCIISendLinePrompt(STRPTR Line,LONG Len):
	 *
	 *	Send text line, wait for prompt.
	 */

STATIC BOOL
ASCIISendLinePrompt(STRPTR Line,LONG Len)
{
	LONG i;

	if(Len == -1)
		Len = strlen(Line);

	while(Len)
	{
		i = 0;

		while(i < Len && Line[i] != '\r')
			i++;

		if(Line[i] == '\r')
		{
			i++;

			SerWrite(Line,i);

			if(!WaitForPrompt(ASCIISendPrompt,ASCIISendPromptLen))
				return(FALSE);
		}
		else
		{
			if(i)
				SerWrite(Line,i);
		}

		Len		-= i;
		Line	+= i;
	}

	return(TRUE);
}

	/* ASCIISendLineDelay(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, include a delay where necessary.
	 */

STATIC BOOL
ASCIISendLineDelay(STRPTR Line,LONG Len)
{
	if(Len == -1)
		Len = strlen(Line);

	while(Len--)
	{
		SerWrite(Line,1);

		if(*Line == '\r')
			DelayTime(Config->TransferConfig->LineDelay / 100,(Config->TransferConfig->LineDelay % 100) * 10000);
		else
			DelayTime(Config->TransferConfig->CharDelay / 100,(Config->TransferConfig->CharDelay % 100) * 10000);

		Line++;
	}

	return(TRUE);
}

	/* ASCIISendLineEcho(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, wait for single characters to be echoed.
	 */

STATIC BOOL
ASCIISendLineEcho(STRPTR Line,LONG Len)
{
	ULONG Signals;
	BOOL GotIt;

		/* -1 means we are going to scan for a null-terminated string. */

	if(Len == -1)
		Len = strlen(Line);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	while(Len--)
	{
			/* Write the next character. */

		SerWrite(Line,1);

			/* Start the timer. */

		StartTime(Config->TransferConfig->SendTimeout / 100,(Config->TransferConfig->SendTimeout % 100) * 10000);

			/* Loop until we got an echo or the bell rings. */

		GotIt = FALSE;

		do
		{
				/* Wait for something to happen. */

			Signals = (*SerialWaitForData)(SIG_TIMER);

				/* Did we receive any data? */

			if(Signals & SIG_SERIAL)
			{
				if((*SerialGetWaiting)())
				{
					if((*SerialRead)(ReadBuffer,1))
					{
						BytesIn++;

						if(!Config->TransferConfig->QuietTransfer)
							ConProcess(ReadBuffer,1);

						if(ReadBuffer[0] == *Line)
							GotIt = TRUE;
					}
				}
			}

				/* Did the timer elapse? */

			if(Signals & SIG_TIMER)
				break;
		}
		while(!GotIt);

			/* Stop the timer. */

		StopTime();

			/* Check if we got the echo. If not, return an error. */

		if(!GotIt)
		{
			Unlock_xOFF();
			return(FALSE);
		}

			/* Skip to the next character in the line. */

		Line++;
	}

	Unlock_xOFF();

	return(TRUE);
}

	/* ASCIISendLineAnyEcho(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, wait for characters to be echoed.
	 */

STATIC BOOL
ASCIISendLineAnyEcho(STRPTR Line,LONG Len)
{
	ULONG Signals;
	BOOL GotIt;

		/* -1 means we are going to scan for a null-terminated string. */

	if(Len == -1)
		Len = strlen(Line);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	while(Len--)
	{
			/* Write the next character. */

		SerWrite(Line,1);

			/* Start the timer. */

		StartTime(Config->TransferConfig->SendTimeout / 100,(Config->TransferConfig->SendTimeout % 100) * 10000);

			/* Loop until we got an echo or the bell rings. */

		GotIt = FALSE;

		do
		{
				/* Wait for something to happen. */

			Signals = (*SerialWaitForData)(SIG_TIMER);

				/* Did we receive any data? */

			if(Signals & SIG_SERIAL)
			{
				if((*SerialGetWaiting)())
				{
					if((*SerialRead)(ReadBuffer,1))
					{
						BytesIn++;

						if(!Config->TransferConfig->QuietTransfer)
							ConProcess(ReadBuffer,1);

						GotIt = TRUE;
					}
				}
			}

				/* Did the timer elapse? */

			if(Signals & SIG_TIMER)
				break;
		}
		while(!GotIt);

			/* Stop the timer. */

		StopTime();

			/* Check if we got the echo. If not, return an error. */

		if(!GotIt)
		{
			Unlock_xOFF();
			return(FALSE);
		}

			/* Skip to the next character in the line. */

		Line++;
	}

	Unlock_xOFF();

	return(TRUE);
}

	/* ASCIISendSetup():
	 *
	 *	Choose the right routine for the text line output job.
	 */

VOID
ASCIISendSetup()
{
		/* Pick the line send routine. */

	switch(Config->TransferConfig->PacingMode)
	{
		case PACE_DIRECT:

			ASCIISendLine = (SENDLINE)SendLineSimple;
			break;

		case PACE_ECHO:

			if(Config->TransferConfig->SendTimeout)
				ASCIISendLine = (SENDLINE)ASCIISendLineEcho;
			else
				ASCIISendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_ANYECHO:

			if(Config->TransferConfig->SendTimeout)
				ASCIISendLine = (SENDLINE)ASCIISendLineAnyEcho;
			else
				ASCIISendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_PROMPT:

			if(Config->TransferConfig->SendTimeout)
			{
					/* Prepare the prompt string. */

				if(Config->TransferConfig->LinePrompt[0])
					ASCIISendPromptLen = TranslateString(Config->TransferConfig->LinePrompt,ASCIISendPrompt);
				else
				{
					ASCIISendPrompt[0] = 0;
					ASCIISendPromptLen = 0;
				}

				ASCIISendLine = (SENDLINE)ASCIISendLinePrompt;
			}
			else
				ASCIISendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_DELAY:

			if(Config->TransferConfig->LineDelay || Config->TransferConfig->CharDelay)
				ASCIISendLine = (SENDLINE)ASCIISendLineDelay;
			else
				ASCIISendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_KEYBOARD:

			ASCIISendLine = (SENDLINE)SendLineKeyDelay;
			break;
	}
}

BOOL
InternalASCIIUpload(STRPTR SingleFile,BOOL WaitForIt)
{
	BOOL DidSend = FALSE;
	BPTR NewDir,OldDir;

		/* We are uploading data. */

	Uploading = TRUE;

		/* Change to the drawer uploads are going to start from. */

	if(NewDir = Lock(Config->PathConfig->ASCIIUploadPath,SHARED_LOCK))
		OldDir = CurrentDir(NewDir);
	else
		OldDir = BNULL;

	BlockWindows();

		/* Check if we must collect the names of the files to send. */

	if(!FileTransferInfo)
	{
		if(SingleFile)
		{
			ULONG Size;

			if(Size = GetFileSize(SingleFile))
			{
				if(FileTransferInfo = AllocFileTransferInfo())
				{
					if(!AddFileTransferNode(FileTransferInfo,SingleFile,Size))
					{
						FreeFileTransferInfo(FileTransferInfo);

						FileTransferInfo = NULL;
					}
				}
			}
		}
		else
		{
			struct FileRequester *FileRequest;
			UBYTE DummyBuffer[MAX_FILENAME_LENGTH];

			strcpy(DummyBuffer,Config->PathConfig->ASCIIUploadPath);

			if(FileRequest = OpenSeveralFiles(Window,LocaleString(MSG_TERMTRANSFER_UPLOAD_FILE_TXT + TRANSFER_ASCII),LocaleString(MSG_TERMTRANSFER_SEND_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				*PathPart(DummyBuffer) = 0;

				strcpy(Config->PathConfig->ASCIIUploadPath,DummyBuffer);

				ConfigChanged = TRUE;

				FileTransferInfo = BuildFileTransferInfo(FileRequest);

				FreeAslRequest(FileRequest);
			}
		}
	}
	else
	{
		if(SingleFile)
		{
			ULONG Size;

			if(Size = GetFileSize(SingleFile))
			{
				if(!AddFileTransferNode(FileTransferInfo,SingleFile,Size))
				{
					FreeFileTransferInfo(FileTransferInfo);

					FileTransferInfo = NULL;
				}
			}
		}
	}

	TransferAborted	= FALSE;
	TransferFailed	= TRUE;
	TransferError	= FALSE;

	if(FileTransferInfo)
	{
		struct Window *ThisWindow;

		if(ThisWindow = CreateASCIIWindow(TRUE))
		{
			LONG Chars = 0,Lines = 0,Len,i;
			BOOL Terminated = FALSE;
			struct Buffer *File;

				/* Final preparations. */

			FileTransferInfo->DoneSize	= 0;
			FileTransferInfo->DoneFiles	= 0;

			FileTransferInfo->CurrentFile	= (struct FileTransferNode *)FileTransferInfo->FileList.mlh_Head;
			FileTransferInfo->CurrentSize	= FileTransferInfo->CurrentFile->Size;

			PushStatus(STATUS_UPLOAD);

			ASCIISendSetup();

				/* Tell everyone what we are doing. */

			PushStatus(STATUS_UPLOAD);

				/* Lock the current xOFF state and clear the xOFF flag. */

			Lock_xOFF();
			Clear_xOFF();

			while(!Terminated && FileTransferInfo->CurrentFile->MinNode.mln_Succ)
			{
				if(File = BufferOpen(FileTransferInfo->CurrentFile->Name,"r"))
				{
					UBYTE	OtherBuffer[256],
							DummyBuffer[2 * sizeof(OtherBuffer)];
					ULONG	Signals;
					BOOL	LastCR = FALSE,Skipped = FALSE;
					LONG	Length;

					AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_OPENING_FILE_TXT),FileTransferInfo->CurrentFile->Name);

					while(!Skipped && !Terminated && (Len = BufferRead(File,OtherBuffer,sizeof(OtherBuffer))) > 0)
					{
						if(Len = MangleASCIIBuffer(Config->TransferConfig->SendCR,Config->TransferConfig->SendLF,OtherBuffer,Len,DummyBuffer))
						{
							for(i = 0 ; i < Len ; i++)
							{
								if(DummyBuffer[i] == '\r')
								{
									Lines++;

									LastCR = TRUE;
								}
								else
								{
									if(DummyBuffer[i] == '\n' && !LastCR)
										Lines++;

									LastCR = FALSE;
								}
							}

							Chars += Len;

							DidSend = TRUE;

							TransferFailed = FALSE;

							(*ASCIISendLine)(DummyBuffer,Len);
						}

						Signals = SetSignal(0,SIG_BREAK | PORTMASK(ThisWindow->UserPort)) & (SIG_BREAK | PORTMASK(ThisWindow->UserPort));

						if(Signals & SIG_BREAK)
							Terminated = TransferAborted = TRUE;

						if(Signals & PORTMASK(ThisWindow->UserPort))
						{
							switch(HandleASCIIWindow())
							{
								case 1:

									Terminated = TransferAborted = TRUE;
									break;

								case 2:

									Skipped = TRUE;
									break;
							}

							if(Terminated || Skipped)
								break;
						}

						if(Length = (*SerialGetWaiting)())
						{
							if(Length > SerialBufferSize)
								Length = SerialBufferSize;

							if(Length > Config->SerialConfig->Quantum)
								Length = Config->SerialConfig->Quantum;

							if(Length = (*SerialRead)(ReadBuffer,Length))
							{
								BytesIn += Length;

									/* Send the data to the console. */

								if(!Config->TransferConfig->QuietTransfer)
									ConProcess(ReadBuffer,Length);
							}
						}

						UpdateASCIIWindow(Chars,FileTransferInfo->CurrentFile->Size,Lines);
					}

					if(Len < 0)
					{
						AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_ERROR_READING_FILE_TXT),FileTransferInfo->CurrentFile->Name);

						TransferFailed = TRUE;

						break;
					}

					BufferClose(File);

					RemoveUploadListItem(FileTransferInfo->CurrentFile->Name);
				}
				else
				{
					AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_ERROR_OPENING_FILE_TXT),FileTransferInfo->CurrentFile->Name);

					TransferFailed = TRUE;

					break;
				}

				FileTransferInfo->DoneSize	+= FileTransferInfo->CurrentSize;
				FileTransferInfo->DoneFiles	+= 1;

				FileTransferInfo->CurrentFile  = (struct FileTransferNode *)FileTransferInfo->CurrentFile->MinNode.mln_Succ;
				FileTransferInfo->CurrentSize  = FileTransferInfo->CurrentFile->Size;
			}

			Unlock_xOFF();

			PopStatus();

			if(TransferFailed || TransferError)
				WakeUp(ThisWindow,SOUND_BADTRANSFER);
			else
			{
				WakeUp(ThisWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteASCIIWindow(ThisWindow,TransferFailed && WaitForIt);
		}

		FreeFileTransferInfo(FileTransferInfo);

		FileTransferInfo = NULL;
	}

	if(TransferFailed || TransferAborted)
		Say(LocaleString(MSG_GLOBAL_TRANSFER_FAILED_OR_ABORTED_TXT));
	else
		Say(LocaleString(MSG_GLOBAL_TRANSFER_COMPLETED_TXT));

	if(NewDir)
		UnLock(CurrentDir(OldDir));

	SendAbort = FALSE;

	ReleaseWindows();

	DidTransfer = FALSE;

	if(WaitForIt)
	{
		if(Config->CommandConfig->DownloadMacro[0])
		{
			SerialCommand(Config->CommandConfig->UploadMacro);
			DidTransfer = FALSE;
		}
	}

	return(DidSend);
}

BOOL
InternalASCIIDownload(STRPTR Name,BOOL WaitForIt)
{
	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
	BOOL DidSend = FALSE;
	BPTR NewDir,OldDir;

		/* Clear the download list. */

	ClearGenericList(GenericListTable[GLIST_DOWNLOAD],FALSE);

		/* This is where the files should go into. */

	DownloadPath = Config->PathConfig->ASCIIDownloadPath;

		/* Is the name empty? */

	NewDir = OldDir = BNULL;

	if(DownloadPath[0])
	{
		if(NewDir = Lock(DownloadPath,ACCESS_READ))
			OldDir = CurrentDir(NewDir);
	}

	BlockWindows();

		/* Should we ask for a file name? */

	if(!Name)
	{
		struct FileRequester *FileRequest;

		strcpy(DummyBuffer,DownloadPath);

		if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMTRANSFER_DOWNLOAD_FILE_TXT + TRANSFER_ASCII),LocaleString(MSG_TERMTRANSFER_RECEIVE_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
		{
			FreeAslRequest(FileRequest);

				/* Save the download path. */

			strcpy(DownloadPath,DummyBuffer);

			ConfigChanged = TRUE;

			Name = DummyBuffer;
		}
	}
	else
	{
		STRPTR Index;

		strcpy(DownloadPath,Name);

		Index = PathPart(DownloadPath);
		*Index = 0;

		ConfigChanged = TRUE;
	}

		/* Final preparations. */

	TransferAborted	= FALSE;
	TransferFailed	= TRUE;
	TransferError	= FALSE;

		/* Did we get a file name? */

	if(Name)
	{
		struct Window *ThisWindow;

		if(ThisWindow = CreateASCIIWindow(FALSE))
		{
			struct Buffer *File;

			if(File = BufferOpen(Name,"w"))
			{
				UBYTE OtherBuffer[512];
				LONG Chars = 0,Lines = 0,i;
				BOOL Terminated = FALSE;
				ULONG Signals;

				AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_OPENING_FILE_TXT),Name);

				TransferFailed = FALSE;

					/* We are now downloading. */

				PushStatus(STATUS_DOWNLOAD);

				StartTime(5,0);

					/* Lock the current xOFF state and clear the xOFF flag. */

				Lock_xOFF();
				Clear_xOFF();

				do
				{
					Signals = (*SerialWaitForData)(SIG_BREAK | SIG_TIMER | PORTMASK(ThisWindow->UserPort));

					if(Signals & PORTMASK(ThisWindow->UserPort))
					{
						if(HandleASCIIWindow())
							break;
					}

					if(Signals & SIG_SERIAL)
					{
						LONG Length;

							/* Check how many characters are still waiting to be read. */

						if(Length = (*SerialGetWaiting)())
						{
								/* Stop the timer. */

							StopTime();

								/* Now determine how many of the waiting data
								 * we will accept.
								 */

							if(Length > SerialBufferSize)
								Length = SerialBufferSize;

							if(Length > Config->SerialConfig->Quantum)
								Length = Config->SerialConfig->Quantum;

							if(Length > sizeof(OtherBuffer) / 2)
								Length = sizeof(OtherBuffer) / 2;

								/* Read the waiting data. */

							if(Length = (*SerialRead)(ReadBuffer,Length))
							{
								BytesIn += Length;

								if(Length = MangleASCIIBuffer(Config->TransferConfig->ReceiveCR,Config->TransferConfig->ReceiveLF,ReadBuffer,Length,OtherBuffer))
								{
										/* Send the data to the console. */

									if(!Config->TransferConfig->QuietTransfer)
										ConProcess(ReadBuffer,Length);

									if(Config->TransferConfig->IgnoreDataPastArnold)
									{
										for(i = 0 ; i < Length ; i++)
										{
											if(OtherBuffer[i] == '\r')
												Lines++;

											if(OtherBuffer[i] == Config->TransferConfig->TerminatorChar)
											{
												Length = i;

												Terminated = TRUE;

												break;
											}
										}
									}
									else
									{
										for(i = 0 ; i < Length ; i++)
										{
											if(OtherBuffer[i] == '\r')
												Lines++;
										}
									}

									if(Length > 0)
									{
										if(BufferWrite(File,OtherBuffer,Length) == Length)
											Chars += Length;
										else
										{
											AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_ERROR_WRITING_FILE_TXT),Name);

											TransferFailed = Terminated = TRUE;
										}
									}
								}

								UpdateASCIIWindow(Chars,0,Lines);
							}

								/* Restart the timer and clear the
								 * timer wait mask in case we grabbed
								 * the new data right at the moment
								 * the timeout was about to strike.
								 */

							StartTime(5,0);
							Signals &= ~SIG_TIMER;
						}
					}

					if(Signals & SIG_TIMER)
						Terminated = TRUE;

					if(Signals & SIG_BREAK)
						TransferAborted = Terminated = TRUE;
				}
				while(!Terminated);

				Unlock_xOFF();

				PopStatus();

				StopTime();

				BufferClose(File);
			}
			else
			{
				AddASCIIMessage(LocaleString(MSG_ASCIIPANEL_ERROR_OPENING_FILE_TXT),Name);

				TransferFailed = TRUE;
			}

			if(TransferFailed || TransferError)
				WakeUp(ThisWindow,SOUND_BADTRANSFER);
			else
			{
				WakeUp(ThisWindow,SOUND_GOODTRANSFER);

				DelayTime(2,0);
			}

			DeleteASCIIWindow(ThisWindow,TransferFailed && WaitForIt);
		}
	}

	if(TransferFailed || TransferAborted)
		Say(LocaleString(MSG_GLOBAL_TRANSFER_FAILED_OR_ABORTED_TXT));
	else
		Say(LocaleString(MSG_GLOBAL_TRANSFER_COMPLETED_TXT));

	if(NewDir)
		UnLock(CurrentDir(OldDir));

	SendAbort = FALSE;

	ReleaseWindows();

	DownloadPath = NULL;

	DidTransfer = FALSE;

	if(WaitForIt)
	{
		if(Config->CommandConfig->DownloadMacro[0])
		{
			SerialCommand(Config->CommandConfig->DownloadMacro);
			DidTransfer = FALSE;
		}
	}

	return(DidSend);
}
