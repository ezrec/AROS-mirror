/*
**	XPR.c
**
**	External transfer protocol support routines
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

STATIC BOOL CheckForCarrierLoss(UWORD SerialStatus);
STATIC VOID HandleCarrierLoss(VOID);
STATIC VOID PrintBox(LONG Box, LONG Line, CONST_STRPTR String, ...);
STATIC BOOL OpenTransferWindow(VOID);
STATIC LONG CheckAbort(BOOL CheckCarrier);
STATIC STRPTR NewFileName(CONST_STRPTR Source, STRPTR Buffer, LONG BufferSize);
STATIC VOID IdentifySource(STRPTR Name, STRPTR BBSName, struct DateStamp *OpenDate);
STATIC LONG GetSeconds(STRPTR String);
STATIC STRPTR TruncateName(STRPTR FileName);
STATIC BOOL IsBlockMappedDevice(struct MsgPort *Handler);
STATIC LONG CalculateBlocks(LONG Size, LONG BlockSize);
STATIC BYTE SerialErrorReport(struct IOExtSer *Request);

	/* These variables keep the transferred bytes and transfer
	 * time in seconds.
	 */

STATIC LONG				ByteVal,ByteMax,
						TimeVal,TimeMax;

STATIC LONG				LastTimeDelta,LastPercent;

	/* Transfer statistics. */

STATIC ULONG			CPS_Minimum,
						CPS_Maximum,
						CPS_Average,
						CPS_Current,
						CPS_Count,
						CPS_Last;

	/* The file currently being transferred. */

STATIC struct Buffer	*CurrentFile;

	/* Yet some more flags. */

STATIC BOOL				UserWasNotified,			/* User was notified that the file being received will not fit. */
						CheckSpace,					/* Check the space left on the destination device? */
						FileSysTypeWasChecked,		/* Filing system type was checked? */
						ErrorMessageWasDisplayed,	/* The error message was displayed? */
						CarrierLossWasDisplayed;	/* The loss of the carrier signal was displayed? */

	/* CheckForCarrierLoss(UWORD SerialStatus):
	 *
	 *	Examine the serial status word and do whatever
	 *	may be necessary next.
	 */

STATIC BOOL
CheckForCarrierLoss(UWORD SerialStatus)
{
#ifdef __AROS__
	if((SerialStatus & SER_STATF_COMCD) && Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
#else
	if((SerialStatus & CIAF_COMCD) && Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
#endif
	{
		HandleCarrierLoss();

		return(TRUE);
	}
	else
		return(FALSE);
}

	/* HandleCarrierLoss():
	 *
	 *	Do whatever needs to be done in case the carrier signal
	 *	is lost during the transmission.
	 */

STATIC VOID
HandleCarrierLoss()
{
	if(!CarrierLossWasDisplayed)
	{
		CarrierLossWasDisplayed = TRUE;

		AddTransferInfo(TRUE,LocaleString(MSG_TERMXPR_CARRIER_LOST_TXT));
	}

	SetOnlineState(FALSE);

	TransferError = TRUE;
}

	/* PrintBox(LONG Box,LONG Line,STRPTR String,...):
	 *
	 *	Update the contents of a text box.
	 */

STATIC VOID
PrintBox(LONG Box,LONG Line,CONST_STRPTR String,...)
{
	UBYTE LocalBuffer[256];
	va_list VarArgs;

	va_start(VarArgs,String);
	LimitedVSPrintf(sizeof(LocalBuffer),LocalBuffer,String,VarArgs);
	va_end(VarArgs);

	LT_SetAttributes(TransferHandle,Box,
		LABX_Index,	Line,
		LABX_Text,	LocalBuffer,
	TAG_DONE);
}

	/* OpenTransferWindow():
	 *
	 *	Local routine to open the transfer window.
	 */

STATIC BOOL
OpenTransferWindow()
{
		/* Block window input. */

	BlockWindows();

		/* Try to open the transfer window panel. */

	if(TransferPanel(LocaleString(MSG_TERMXPR_RECEIVE_FILES_TXT)))
	{
			/* Supply the log text. */

		LogAction(LocaleString(MSG_TERMXPR_LOGMSG_INITIATE_BINARY_DOWNLOAD_TXT));

			/* Return success. */

		return(TRUE);
	}
	else
	{
			/* Re-enable window input. */

		ReleaseWindows();

			/* Return failure. */

		return(FALSE);
	}
}

	/* CheckAbort(BOOL CheckCarrier):
	 *
	 *	Check for transfer abort.
	 */

STATIC LONG
CheckAbort(BOOL CheckCarrier)
{
	struct IntuiMessage	*Message;
	ULONG MsgClass,MsgQualifier;
	struct Gadget *MsgGadget;
	struct Window *MsgWindow;
	UWORD MsgCode;
	LONG Result;

	Result = 0;

		/* No window available for output? */

	if(!TransferWindow)
	{
		if(!OpenTransferWindow())
			return(-1);
	}

		/* Are we to check the carrier status? */

	if(CheckCarrier)
	{
			/* Return error if carrier is lost. */

#ifdef __AROS__
		if(GetSerialStatus() & SER_STATF_COMCD)
#else
		if(GetSerialStatus() & CIAF_COMCD)
#endif
		{
			HandleCarrierLoss();

			return(-1);
		}
	}

		/* Process all incoming messages. */

	while(Message = (struct IntuiMessage *)GT_GetIMsg(TransferWindow->UserPort))
	{
		MsgClass		= Message->Class;
		MsgQualifier	= Message->Qualifier;
		MsgCode			= Message->Code;
		MsgGadget		= (struct Gadget *)Message->IAddress;
		MsgWindow		= Message->IDCMPWindow;

		GT_ReplyIMsg(Message);

		if(MsgWindow == TransferWindow)
		{
			if(!Result)
			{
				LT_HandleInput(TransferHandle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

				if(MsgClass == IDCMP_GADGETUP)
				{
					switch(MsgGadget->GadgetID)
					{
						case GAD_TRANSFER_ABORT:

							LogAction(LocaleString(MSG_TERMXPR_LOGMSG_TRANSFER_ABORTED_TXT));

							TransferAborted = TRUE;

							Result = -1;

							break;

						case GAD_TRANSFER_SKIP:

							LogAction(LocaleString(MSG_TERMXPR_LOGMSG_FILE_SKIPPED_TXT));

							Result = 1;

							break;

						case GAD_TRANSFER_ABORT_FILE:

							LogAction(LocaleString(MSG_TERMXPR_LOGMSG_FILE_ABORTED_TXT));

							Result = 2;

							break;
					}
				}
			}
		}
	}

	return(Result);
}

	/* NewFileName(STRPTR Source,STRPTR Buffer,LONG BufferSize):
	 *
	 *	Build a valid file and path name, including path
	 *	substitution, etc.
	 */

STATIC STRPTR
NewFileName(CONST_STRPTR Source,STRPTR Buffer,LONG BufferSize)
{
	if(Config->TransferConfig->OverridePath && !Uploading)
	{
		if(DownloadPath)
			LimitedStrcpy(BufferSize,Buffer,DownloadPath);
		else
			LimitedStrcpy(BufferSize,Buffer,Config->PathConfig->BinaryDownloadPath);

		if(!Buffer[0])
		{
			if(!LocalGetCurrentDirName(Buffer,BufferSize))
				Buffer[0] = 0;
		}

		if(Buffer[0])
		{
			if(AddPart(Buffer,FilePart(Source),BufferSize))
				return(Buffer);
		}
	}
	else
	{
		if(FilePart(Source) == Source)
		{
			if(LocalGetCurrentDirName(Buffer,BufferSize))
			{
				if(AddPart(Buffer,Source,BufferSize))
					return(Buffer);
			}
		}
	}

	LimitedStrcpy(BufferSize,Buffer,Source);

	return(Buffer);
}

	/* IdentifySource(STRPTR Name,STRPTR BBSName,struct DateStamp *OpenDate):
	 *
	 *	Attach source information (BBS name, date and time) to a file.
	 */

STATIC VOID
IdentifySource(STRPTR Name,STRPTR BBSName,struct DateStamp *OpenDate)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH],Time[40],Date[40];
	struct DateTime	DateTime;

	CopyMem(OpenDate,&DateTime.dat_Stamp,sizeof(struct DateStamp));

		/* Prepare for date conversion. */

	DateTime.dat_Format		= FORMAT_DEF;
	DateTime.dat_Flags		= 0;
	DateTime.dat_StrDay		= NULL;
	DateTime.dat_StrDate	= Date;
	DateTime.dat_StrTime	= Time;

		/* Convert the date. */

	if(DateToStr(&DateTime))
	{
		StripSpaces(BBSName);
		StripSpaces(Date);
		StripSpaces(Time);

		if(BBSName[0])
			LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%s %s %s",BBSName,Date,Time);
		else
			LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%s %s",Date,Time);

		SetComment(Name,LocalBuffer);
	}
}

	/* GetSeconds(STRPTR String):
	 *
	 *	Tries to turn a string of the format hh:mm:ss into
	 *	an integer number.
	 */

STATIC LONG
GetSeconds(STRPTR String)
{
	UBYTE Buffer[20];
	LONG Seconds;

	memset(Buffer,0,20);
	Seconds = 0;

	strcpy(Buffer,String);

	Seconds += Atol(&Buffer[6]);

	Buffer[5] = 0;

	Seconds += Atol(&Buffer[3]) * 60;

	Buffer[2] = 0;

	Seconds += Atol(&Buffer[0]) * 3600;

	return(Seconds);
}

	/* TruncateName(STRPTR FileName):
	 *
	 *	Truncates a file name to a maximum of 48 characters.
	 */

STATIC STRPTR
TruncateName(STRPTR FileName)
{
	LONG Len;

	Len = strlen(FileName);

	if(Len > 48)
	{
		LONG i;

		for(i = Len - 48 ; i < Len ; i++)
		{
			if(i >= Len - 44 && FileName[i] == '/')
			{
				STATIC UBYTE NameBuffer[MAX_FILENAME_LENGTH];

				strcpy(NameBuffer,".../");

				LimitedStrcat(sizeof(NameBuffer),NameBuffer,&FileName[i + 1]);

				return(NameBuffer);
			}
		}
	}

	return(FileName);
}

	/* IsBlockMappedDevice(struct MsgPort *Handler):
	 *
	 *	See if the lock given points to a block mapped filing
	 *	system.
	 */

STATIC BOOL
IsBlockMappedDevice(struct MsgPort *Handler)
{
	struct DosList *Entry;
	BOOL IsBlockMapped;

	IsBlockMapped = FALSE;

		/* Find the device the lock belongs to */

	Entry = LockDosList(LDF_DEVICES | LDF_READ);

	while(Entry = NextDosEntry(Entry,LDF_DEVICES))
	{
		if(Entry->dol_Task == Handler)
		{
			struct FileSysStartupMsg *Startup = (struct FileSysStartupMsg *)BADDR(Entry->dol_misc.dol_handler.dol_Startup);

				/* Plain filing system usually don't */
				/* use the startup entry */

			if(TypeOfMem(Startup))
			{
				struct DosEnvec	*Environ;
				STRPTR Name;

				Name	= (STRPTR)BADDR(Startup->fssm_Device);
				Environ	= (struct DosEnvec *)BADDR(Startup->fssm_Environ);

					/* Valid data in the startup entry? */

				if(TypeOfMem(Name) && TypeOfMem(Environ))
				{
					struct IOStdReq Request;

					memset(&Request,0,sizeof(Request));
					Request.io_Message.mn_Length = sizeof(Request);

						/* Last chance, try to open the device driver */

					if(!OpenDevice(Name + 1,Startup->fssm_Unit,(struct IORequest *)&Request,Startup->fssm_Flags))
					{
							/* This is a block mapped filing system */

						IsBlockMapped = TRUE;

						CloseDevice((struct IORequest *)&Request);

						break;
					}
				}
			}
		}
	}

	UnLockDosList(LDF_DEVICES | LDF_READ);

	return(IsBlockMapped);
}

	/* CalculateBlocks(LONG Size,LONG BlockSize):
	 *
	 *	Calculate the number of blocks a file will
	 *	occupy if saved to a disk.
	 */

STATIC LONG
CalculateBlocks(LONG Size,LONG BlockSize)
{
	LONG Blocks,Extension;
	BOOL HasExtension;

	Blocks			= 1;		/* One for the file header. */
	Extension		= 0;		/* How many block pointers yet. */
	HasExtension	= FALSE;	/* No extension block yet. */

		/* Round to next block. */

	Size = ((Size + BlockSize - 1) / BlockSize) * BlockSize;

	while(Size)
	{
			/* Add another block. */

		Blocks++;

			/* Subtract another block. */

		Size -= BlockSize;

			/* Add another block pointer, if 72 have been
			 * added, add another extension block.
			 */

		if((Extension++) == 72)
		{
				/* If no extension block has been generated
				 * yet, we were running on the block pointers
				 * of the file header itself.
				 */

			if(!HasExtension)
				HasExtension = TRUE;
			else
				Blocks++;

				/* Reset extension block counter. */

			Extension = 0;
		}
	}

	return(Blocks);
}

	/* SerialErrorReport(struct IOExtSer *Request):
	 *
	 *	Report a certain I/O error cause.
	 */

STATIC BYTE
SerialErrorReport(struct IOExtSer *Request)
{
	CONST_STRPTR ErrorMessage;
	BOOL IsFatal;

	switch(Request->IOSer.io_Error)
	{
		case SerErr_LineErr:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_HARDWARE_DATA_OVERRUN_TXT);

			IsFatal = FALSE;

			break;

		case SerErr_ParityErr:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_PARITY_ERROR_TXT);

			IsFatal = TRUE;

			break;

		case SerErr_TimerErr:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_TIMER_ERROR_TXT);

			IsFatal = FALSE;

			break;

		case SerErr_BufOverflow:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_READ_BUFFER_OVERFLOWED_TXT);

			IsFatal = FALSE;

			break;

		case SerErr_NoDSR:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_NO_DSR_TXT);

			IsFatal = TRUE;

			break;

		case SerErr_DetectedBreak:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_BREAK_DETECTED_TXT);

			IsFatal = TRUE;

			break;

		default:

			ErrorMessage = LocaleString(MSG_TERMXPR_ERROR_UNKNOWN_CAUSE_TXT);

			IsFatal = FALSE;

			break;
	}

	AddTransferInfo(TRUE,LocaleString(MSG_TERMXPR_ERROR_TEMPLATE_TXT),Request->IOSer.io_Error,ErrorMessage);

	if(IsFatal)
	{
		TransferError = TRUE;

		return(TRUE);
	}
	else
		return(FALSE);
}

	/* xpr_fopen(STRPTR FileName,STRPTR AccessMode):
	 *
	 *	Open a file for random access.
	 */

BPTR SAVE_DS ASM
xpr_fopen(REG(a0) const char *FileName,REG(a1) const char *AccessMode)
{
	UBYTE RealName[MAX_FILENAME_LENGTH];
	struct Buffer *File;

	DB(kprintf("xpr_fopen(\"%s\",\"%s\")\n",FileName,AccessMode));

	UserWasNotified				= FALSE;
	CarrierLossWasDisplayed		= FALSE;
	ErrorMessageWasDisplayed	= FALSE;
	FileSysTypeWasChecked		= FALSE;
	CheckSpace					= TRUE;
	DidTransfer					= TRUE;

		/* Reset transfer counters. */

	ByteVal			= 0;
	ByteMax			= 0;
	TimeVal			= 0;
	TimeMax			= 0;

	LastTimeDelta	= 0;
	LastPercent		= -1;

		/* Reset CPS statistics. */

	CPS_Minimum		= (ULONG)~0;
	CPS_Maximum		= 0;
	CPS_Average		= 0;
	CPS_Current		= 0;
	CPS_Count		= 0;
	CPS_Last		= 0;

		/* No window available for output? */

	if(!TransferWindow)
		OpenTransferWindow();

	if(OriginalName[0])
	{
		if(!Stricmp(ShrunkenName,FileName))
			FileName = OriginalName;
	}

		/* Determine the file name. */

	FileName = NewFileName(FileName,RealName,sizeof(RealName));

		/* Determine file transfer mode... */

	if(File = BufferOpen(FileName,AccessMode))
	{
		switch(AccessMode[0])
		{
			case 'r':

				LogAction(LocaleString(MSG_TERMXPR_LOGMSG_SEND_FILE_TXT),FileName);
				break;

			case 'w':

				LogAction(LocaleString(MSG_TERMXPR_LOGMSG_RECEIVE_FILE_TXT),FileName);
				break;

			case 'a':

				LogAction(LocaleString(MSG_TERMXPR_LOGMSG_UPDATE_FILE_TXT),FileName);
				break;
		}

		CurrentFile = File;
	}

	return((BPTR)File);
}

	/* xpr_fclose(struct Buffer *File):
	 *
	 *	Close a file opened by xpr_fopen.
	 */

long SAVE_DS ASM
xpr_fclose(REG(a0) BPTR FileP)
{
	struct Buffer *File = (struct Buffer *)FileP;
	UBYTE RealName[MAX_FILENAME_LENGTH];
	struct DateStamp OpenDate;
	BOOL WriteAccess,Used;

	DB(kprintf("xpr_fclose(0x%08lx)\n",File));

		/* This happened only once so far, but... */

	if(!File)
	{
		CurrentFile = NULL;

		return(1);
	}

		/* Save some information. */

	OpenDate	= File->OpenDate;
	WriteAccess	= File->WriteAccess;
	Used		= File->Used;

		/* Get the name of the file. */

	if(!NameFromFH(File->FileHandle,RealName,sizeof(RealName)))
		RealName[0] = 0;

		/* Close the file and see what it brings... */

	if(BufferClose(File) && RealName[0])
	{
			/* Did any access take place at all?
			 * xprzmodem.library for example just
			 * opens and closes a file in order to
			 * see if it exists.
			 */

		if(!Used)
			LogAction(LocaleString(MSG_TERMXPR_CLOSE_FILE_TXT),RealName);
		else
		{
				/* Did we receive or send a file? */

			if(WriteAccess)
			{
				LONG Size;

					/* Did the file remain empty? */

				if(!(Size = GetFileSize(RealName)))
				{
					AddTransferInfo(TRUE,LocaleString(MSG_TERMXPR_FILE_REMOVED_TXT),FilePart(RealName));

						/* Delete empty file. */

					if(DeleteFile(RealName))
						LogAction(LocaleString(MSG_TERMXPR_CLOSE_FILE_REMOVED_TXT),RealName);
					else
						LogAction(LocaleString(MSG_TERMXPR_CLOSE_FILE_TXT),RealName);
				}
				else
				{
					struct Node *SomeNode;

					if(ByteMax)
					{
						if(Size >= ByteMax)
							AddTransferInfo(FALSE,LocaleString(MSG_TERMXPR_FILE_RECEIVED_TXT),FilePart(RealName));
						else
							AddTransferInfo(TRUE,LocaleString(MSG_TERMXPR_INCOMPLETE_FILE_RECEIVED_TXT),FilePart(RealName));
					}

						/* Try to identify the file type. */

					if(Config->TransferConfig->IdentifyCommand[0])
					{
						UBYTE LocalBuffer[2 * MAX_FILENAME_LENGTH];
						LONG i;
						BOOL GotIt;

						GotIt = FALSE;

						for(i = 0 ; i < strlen(Config->TransferConfig->IdentifyCommand) - 1 ; i++)
						{
							if(Config->TransferConfig->IdentifyCommand[i] == '%' && Config->TransferConfig->IdentifyCommand[i+1] == 's')
							{
								GotIt = TRUE;
								break;
							}
						}

						if(GotIt)
							LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,Config->TransferConfig->IdentifyCommand,RealName);
						else
						{
							LimitedStrcpy(sizeof(LocalBuffer),LocalBuffer,Config->TransferConfig->IdentifyCommand);
							LimitedStrcat(sizeof(LocalBuffer),LocalBuffer," \"");
							LimitedStrcat(sizeof(LocalBuffer),LocalBuffer,RealName);
							LimitedStrcat(sizeof(LocalBuffer),LocalBuffer,"\"");
						}

						LaunchCommandAsync(LocalBuffer);
					}
					else
					{
						switch(Config->TransferConfig->IdentifyFiles)
						{
							case IDENTIFY_IGNORE:

								if(Config->MiscConfig->CreateIcons)
									Identify(RealName,FALSE);

								break;

							case IDENTIFY_FILETYPE:

								Identify(RealName,TRUE);
								break;

							case IDENTIFY_SOURCE:

								IdentifySource(RealName,CurrentBBSName,&OpenDate);

								if(Config->MiscConfig->CreateIcons)
									Identify(RealName,FALSE);

								break;
						}
					}

					if(CPS_Minimum == (ULONG)~0)
						CPS_Minimum = 0;

					if(CPS_Count == 0)
					{
						CPS_Average	= 0;
						CPS_Count	= 1;
					}

					LogAction(LocaleString(MSG_TERMXPR_CLOSE_FILE_BYTES_TXT),RealName,Size,CPS_Minimum,CPS_Average / CPS_Count,CPS_Maximum);

					if(SomeNode = CreateGenericListNode(0,RealName))
						AddGenericListNode(GenericListTable[GLIST_DOWNLOAD],SomeNode,ADD_GLIST_BOTTOM,FALSE);
				}
			}
			else
			{
					/* Set the archived bit on files we uploaded? */

				if(Config->TransferConfig->SetArchivedBit)
					AddProtection(RealName,FIBF_ARCHIVE);

				AddTransferInfo(FALSE,LocaleString(MSG_TERMXPR_FILE_SENT_TXT),FilePart(RealName));

				LogAction(LocaleString(MSG_TERMXPR_CLOSE_FILE_TXT),RealName);

				RemoveUploadListItem(RealName);
			}
		}
	}

	CurrentFile = NULL;

	return(1);
}

	/* xpr_fread(APTR Buffer,LONG Size,LONG Count,struct Buffer *File):
	 *
	 *	Read a few bytes from a file.
	 */

long SAVE_DS ASM
xpr_fread(REG(a0) char *Buffer,REG(d0) long Size,REG(d1) long Count,REG(a1) BPTR FileP)
{
	struct Buffer *File = (struct Buffer *)FileP;
	DB(kprintf("xpr_fread(0x%08lx,%ld,%ld,0x%08lx)\n",Buffer,Size,Count,File));

	return(BufferRead(File,Buffer,Size * Count) / Size);
}

	/* xpr_fwrite(APTR Buffer,LONG Size,LONG Count,struct Buffer *File):
	 *
	 *	Write a few bytes to a file.
	 */

long SAVE_DS ASM
xpr_fwrite(REG(a0) char *Buffer,REG(d0) LONG Size,REG(d1) long Count,REG(a1) BPTR FileP)
{
	struct Buffer *File = (struct Buffer *)FileP;
	DB(kprintf("xpr_fwrite(0x%08lx,%ld,%ld,0x%08lx)\n",Buffer,Size,Count,File));

	return(BufferWrite(File,Buffer,Size * Count) / Size);
}

	/* xpr_fseek(struct Buffer *File,LONG Offset,LONG Origin):
	 *
	 *	Move the read/write pointer in a file.
	 */

long SAVE_DS ASM
xpr_fseek(REG(a0) BPTR FileP,REG(d0) long Offset,REG(d1) long Origin)
{
	struct Buffer *File = (struct Buffer *)FileP;

	DB(kprintf("xpr_fseek(0x%08lx,%ld,%ld)\n",File,Offset,Origin));

	return(BufferSeek(File,Offset,Origin) ? 0 : -1);
}

	/* xpr_sread(APTR Buffer,LONG Size,LONG Timeout):
	 *
	 *	Read a few bytes from the serial port (including
	 *	timeouts).
	 */

long SAVE_DS ASM
xpr_sread(REG(a0) char *Buffer,REG(d0) long Size,REG(d1) long Timeout)
{
	DB(kprintf("xpr_sread(0x%08lx,%ld,%ld)\n",Buffer,Size,Timeout));

		/* No window available for output? */

	if(!TransferWindow)
	{
		if(!OpenTransferWindow())
			return(-1);
	}

		/* Are both IORequests available? */

	if(WriteRequest && ReadRequest)
	{
			/* Valid size parameter? */

		if(Size > 0)
		{
			ULONG SignalSet,SerialMask,WindowMask;
			ULONG Waiting;
			UWORD Status;

			GetSerialInfo(&Waiting,&Status);

				/* Return error if carrier is lost. */

			if(CheckForCarrierLoss(Status))
				return(-1);

				/* Is there data waiting to be read? */

			if(Waiting > 0)
			{
					/* No timeout specified? Read as many
					 * bytes as available.
					 */

				if(Timeout == 0)
				{
					if(Waiting > Size)
						Waiting = Size;

					if(DoSerialRead(Buffer,Waiting))
					{
						if(SerialErrorReport(ReadRequest))
							return(-1);
						else
							Waiting = ReadRequest->IOSer.io_Actual;
					}

					BytesIn += Waiting;

					return((LONG)Waiting);
				}

					/* Enough data pending to be read? */

				if(Waiting >= Size)
				{
					if(DoSerialRead(Buffer,Size))
					{
						if(SerialErrorReport(ReadRequest))
							return(-1);
						else
							Size = ReadRequest->IOSer.io_Actual;
					}

					BytesIn += Size;

					return((LONG)Size);
				}
			}
			else
			{
					/* No timeout and no data available:
					 * return immediately.
					 */

				if(Timeout == 0)
					return(0);
			}

				/* Set up the timer. */

			StartTime(Timeout / MILLION,Timeout % MILLION);

				/* Set up the read request. */

			StartSerialRead(Buffer,Size);

				/* We'll need them in a minute */

			SerialMask = PORTMASK(ReadPort);
			WindowMask = PORTMASK(TransferWindow->UserPort);

			while(TRUE)
			{
				SignalSet = Wait(SerialMask | SIG_TIMER | WindowMask);

					/* Abort the file transfer? */

				if(SignalSet & WindowMask)
				{
					LONG Result = CheckAbort(FALSE);

						/* Let's get out of here
						 * and hope that the protocol
						 * will call xpr_chkabort()
						 * in time.
						 */

					if(Result != 0)
					{
						TransferAbortState = Result;

							/* Stop whatever we were doing. */

						StopSerialRead();
						StopTime();

							/* Does the user want to cancel
							 * the transmission?
							 */

						if(Result < 0)
						{
								/* Do it the hard way */

							if(TransferAbortCount++)
								CancelZModem();

							return(Result);
						}
						else
						{
								/* So it's something else. Pick up whatever
								 * we received and return immediately.
								 */

							if(ReadRequest->IOSer.io_Actual > 0)
							{
								BytesIn += ReadRequest->IOSer.io_Actual;

								return((LONG)ReadRequest->IOSer.io_Actual);
							}
							else
							{
									/* Take a second look and query the number of
									 * bytes ready to be received, there may
									 * still be some bytes in the buffer.
									 * Note: this depends on the way the
									 * driver handles read abort.
									 */

								GetSerialInfo(&Waiting,&Status);

									/* Don't read too much. */

								if(Size > Waiting)
									Size = Waiting;

									/* Are there any bytes to be transferred? */

								if(Size > 0)
								{
										/* Read the data. */

									if(DoSerialRead(Buffer,Size))
									{
										if(SerialErrorReport(ReadRequest))
											return(-1);
										else
											Size = ReadRequest->IOSer.io_Actual;
									}

									BytesIn += Size;
								}
								else
								{
										/* Ok, so there is no data in the buffer. */
										/* Check if the carrier signal is still */
										/* present */

									if(CheckForCarrierLoss(Status))
										return(-1);
								}

								return((LONG)Size);
							}
						}
					}
				}

					/* Receive buffer filled? */

				if(SignalSet & SerialMask)
				{
						/* Abort the timer request. */

					StopTime();

						/* Did the request terminate gracefully? */

					if(WaitSerialRead())
					{
						if(SerialErrorReport(ReadRequest))
							return(-1);
						else
							Size = ReadRequest->IOSer.io_Actual;
					}

					BytesIn += Size;

					return((LONG)Size);
				}

					/* Hit by timeout? */

				if(SignalSet & SIG_TIMER)
				{
						/* Abort the read request. */

					StopSerialRead();

						/* Remove the timer request. */

					WaitTime();

						/* Did the driver receive any
						 * data?
						 */

					if(ReadRequest->IOSer.io_Actual > 0)
					{
						BytesIn += ReadRequest->IOSer.io_Actual;

						return((LONG)ReadRequest->IOSer.io_Actual);
					}
					else
					{
							/* Take a second look and query the number of
							 * bytes ready to be received, there may
							 * still be some bytes in the buffer.
							 * Note: this depends on the way the
							 * driver handles read abort.
							 */

						GetSerialInfo(&Waiting,&Status);

							/* Don't read too much. */

						if(Size > Waiting)
							Size = Waiting;

							/* Are there any bytes to be transferred? */

						if(Size > 0)
						{
								/* Read the data. */

							if(DoSerialRead(Buffer,Size))
							{
								if(SerialErrorReport(ReadRequest))
									return(-1);
								else
									Size = ReadRequest->IOSer.io_Actual;
							}

							BytesIn += Size;
						}
						else
						{
								/* Ok, so there is no data in the buffer. */
								/* Check if the carrier signal is still */
								/* present */

							if(CheckForCarrierLoss(Status))
								return(-1);
						}

						return((LONG)Size);
					}
				}
			}
		}
		else
			return(0);
	}
	else
		return(-1);
}

	/* xpr_swrite(APTR Buffer,LONG Size):
	 *
	 *	Write a few bytes to the serial port.
	 */

long SAVE_DS ASM
xpr_swrite(REG(a0) char *Buffer,REG(d0) long Size)
{
	DB(kprintf("xpr_swrite(0x%08lx,%ld)\n",Buffer,Size));

		/* Return error if carrier is lost. */

	if(WriteRequest && !CheckForCarrierLoss(GetSerialStatus()))
	{
			/* Send the data. */

		if(!DoSerialWrite(Buffer,Size))
			BytesOut += Size;
		else
		{
			if(SerialErrorReport(WriteRequest))
				return(-1);
		}

		return(0);
	}

	return(-1);
}

	/* xpr_sflush():
	 *
	 *	Release the contents of all serial buffers.
	 */

LONG SAVE_DS
xpr_sflush()
{
	DB(kprintf("xpr_sflush()\n"));

	return((LONG)FlushSerialRead());
}

	/* xpr_update(struct XPR_UPDATE *UpdateInfo):
	 *
	 *	Update the information displayed in the transfer window.
	 */

LONG SAVE_DS ASM
xpr_update(REG(a0) struct XPR_UPDATE *UpdateInfo)
{
	STATIC UBYTE RealName[MAX_FILENAME_LENGTH];

	DB(kprintf("xpr_update(0x%08lx)\n",UpdateInfo));

		/* No window available for output? */

	if(!TransferWindow)
	{
		if(!OpenTransferWindow())
			return(0);
	}

	if(UpdateInfo)
	{
		BOOL NewByte,NewTime;

		NewByte	= FALSE;
		NewTime = FALSE;

		if(TransferWindow->Flags & WFLG_ZOOMED)
		{
			STATIC ULONG LastSeconds;

			struct timeval Now;
			BOOL Refresh;

			GetSysTime(&Now);

			Refresh = TRUE;

			if(!TransferZoomed)
			{
				LastSeconds = Now.tv_secs;

				TransferZoomed = TRUE;
			}
			else
			{
				if(Now.tv_secs > LastSeconds)
					LastSeconds = Now.tv_secs;
				else
					Refresh = FALSE;
			}

			if(Refresh)
			{
				STRPTR Template;

				if(LastPercent > 0)
					Template = "%s [%s %ld%%]";
				else
					Template = "%s [%s]";

				LimitedSPrintf(sizeof(TransferTitleBuffer),TransferTitleBuffer,Template,TransferWindowTitle,TruncateName(RealName),LastPercent);

				SetWindowTitles(TransferWindow,TransferTitleBuffer,(STRPTR)-1);
			}
		}
		else
		{
			if(TransferZoomed)
			{
				SetWindowTitles(TransferWindow,TransferWindowTitle,(STRPTR)-1);

				TransferZoomed = FALSE;
			}
		}

		if((UpdateInfo->xpru_updatemask & XPRU_PROTOCOL) && UpdateInfo->xpru_protocol)
			LT_SetAttributes(TransferHandle,GAD_TRANSFER_PROTOCOL,GTTX_Text,UpdateInfo->xpru_protocol,TAG_DONE);

		if((UpdateInfo->xpru_updatemask & XPRU_MSG) && UpdateInfo->xpru_msg)
			AddTransferInfo(FALSE,UpdateInfo->xpru_msg);

		if((UpdateInfo->xpru_updatemask & XPRU_ERRORMSG) && UpdateInfo->xpru_errormsg)
			AddTransferInfo(TRUE,UpdateInfo->xpru_errormsg);

		if((UpdateInfo->xpru_updatemask & XPRU_FILENAME) && UpdateInfo->xpru_filename)
		{
			STRPTR FileName = UpdateInfo->xpru_filename;

			if(OriginalName[0])
			{
				if(!Stricmp(ShrunkenName,FileName))
					FileName = OriginalName;
			}

			FileName = NewFileName(FileName,RealName,sizeof(RealName));

			LT_SetAttributes(TransferHandle,GAD_TRANSFER_FILE,LABX_Index,0,LABX_Text,TruncateName(FileName),TAG_DONE);

			if(FileTransferInfo)
			{
				LONG Percentage;

				if(FileTransferInfo->CurrentFile)
				{
					struct FileTransferNode *Node;
					STRPTR Name;

					Node = (struct FileTransferNode *)FileTransferInfo->CurrentFile->MinNode.mln_Succ;

					if(Node->MinNode.mln_Succ)
						Name = TruncateName(Node->Name);
					else
						Name = "-";

					LT_SetAttributes(TransferHandle,GAD_TRANSFER_FILE,LABX_Index,1,LABX_Text,Name,TAG_DONE);
				}

				PrintBox(GAD_TRANSFER_SIZE,2,ConvNumber,FileTransferInfo->TotalSize);

				if(FileTransferInfo->TotalFiles)
					Percentage = (100 * FileTransferInfo->DoneFiles) / FileTransferInfo->TotalFiles;
				else
					Percentage = 0;

				PrintBox(GAD_TRANSFER_SIZE,4,LocaleString(MSG_TRANSFERPANEL_NUMBER_TO_GO_PERCENT_TXT),FileTransferInfo->DoneFiles,FileTransferInfo->TotalFiles,Percentage);
			}
		}

		if((UpdateInfo->xpru_updatemask & XPRU_FILESIZE) && UpdateInfo->xpru_filesize != -1)
		{
			STRPTR FormatSpecifier;

			if(CurrentFile && CurrentFile->InfoPort)
				FormatSpecifier = ConvNumber10;
			else
				FormatSpecifier = ConvNumber;

			PrintBox(GAD_TRANSFER_SIZE,0,FormatSpecifier,UpdateInfo->xpru_filesize);

			if(ByteMax = UpdateInfo->xpru_filesize)
				NewByte = TRUE;
		}

		if((UpdateInfo->xpru_updatemask & XPRU_BYTES) && UpdateInfo->xpru_bytes != -1)
		{
			STRPTR FormatSpecifier;

			if(CurrentFile && CurrentFile->InfoPort)
				FormatSpecifier = ConvNumber10;
			else
				FormatSpecifier = ConvNumber;

			PrintBox(GAD_TRANSFER_SIZE,1,FormatSpecifier,ByteVal = UpdateInfo->xpru_bytes);

			if(FileTransferInfo)
			{
				LONG Percentage;
				ULONG Len;

				Len = FileTransferInfo->DoneSize + UpdateInfo->xpru_bytes;

				if(FileTransferInfo->TotalSize)
					Percentage = (100 * Len) / FileTransferInfo->TotalSize;
				else
					Percentage = 0;

				PrintBox(GAD_TRANSFER_SIZE,3,LocaleString(MSG_TRANSFERPANEL_NUMBER_PERCENT_TXT),Len,Percentage);
			}

			if(ByteMax)
				NewByte = TRUE;

			if(CurrentFile)
			{
				if(CurrentFile->InfoPort && !Uploading)
				{
					if(CurrentFile->InfoData.id_NumBlocks && CurrentFile->InfoData.id_BytesPerBlock)
					{
						LONG DisplaySpace,DisplayPercent;
						CONST_STRPTR DisplayMessage;

						DisplayMessage	= "";
						DisplayPercent	= (100 * CurrentFile->InfoData.id_NumBlocksUsed) / CurrentFile->InfoData.id_NumBlocks;
						DisplaySpace	= CurrentFile->InfoData.id_NumBlocks - CurrentFile->InfoData.id_NumBlocksUsed;

						if(ByteMax && (CheckSpace || !FileSysTypeWasChecked))
						{
								/* Did we take a look at the filing system yet? */

							if(!FileSysTypeWasChecked)
							{
								FileSysTypeWasChecked = TRUE;

									/* If this is not a block mapped device */
									/* we cannot guess what will happen if */
									/* it fills up (and whether it fills up at all). */

								if(!IsBlockMappedDevice(CurrentFile->InfoPort))
									CheckSpace = FALSE;
							}

								/* Should we check the space left on the destination device? */

							if(CheckSpace)
							{
									/* Important! Only use the number of bytes still */
									/* to be transferred. */

								LONG Blocks = CalculateBlocks(ByteMax - ByteVal,CurrentFile->InfoData.id_BytesPerBlock);

									/* Is there less space left than we need? */

								if(DisplaySpace < Blocks)
								{
									if(!UserWasNotified)
									{
										UserWasNotified = TRUE;

										Say(LocaleString(MSG_TERMXPR_SAY_FILE_MAY_NOT_FIT_TXT));
									}

									DisplayMessage = LocaleString(MSG_TERMXPR_FILE_MAY_NOT_FIT_TXT);
								}
							}
						}

						PrintBox(GAD_TRANSFER_FILE,2,LocaleString(MSG_TERMXPR_BYTES_FULL_TXT),DisplaySpace * CurrentFile->InfoData.id_BytesPerBlock,DisplayPercent,DisplayMessage);
					}
				}
			}
		}

		if((UpdateInfo->xpru_updatemask & XPRU_BLOCKS) && UpdateInfo->xpru_blocks != -1)
			PrintBox(GAD_TRANSFER_SIZE,5,ConvNumber,UpdateInfo->xpru_blocks);

		if((UpdateInfo->xpru_updatemask & XPRU_DATARATE) && UpdateInfo->xpru_datarate > 0)
		{
			LONG Change;

			CPS_Current = UpdateInfo->xpru_datarate;

			if(CPS_Last)
			{
				if(CPS_Last < UpdateInfo->xpru_datarate)
					Change = (100 * (UpdateInfo->xpru_datarate - CPS_Last)) / CPS_Last;
				else
					Change = (100 * (CPS_Last - UpdateInfo->xpru_datarate)) / CPS_Last;
			}
			else
				Change = 100;

			if(Change > 1)
			{
				PrintBox(GAD_TRANSFER_SECONDS,0,ConvNumber,UpdateInfo->xpru_datarate);

				CPS_Last = UpdateInfo->xpru_datarate;
			}

			if(UpdateInfo->xpru_datarate < CPS_Minimum)
				CPS_Minimum = UpdateInfo->xpru_datarate;

			if(UpdateInfo->xpru_datarate > CPS_Maximum)
				CPS_Maximum = UpdateInfo->xpru_datarate;

			if((ULONG)(CPS_Average + UpdateInfo->xpru_datarate) >= CPS_Average)
			{
				CPS_Average += UpdateInfo->xpru_datarate;

				CPS_Count++;
			}
		}

		if((UpdateInfo->xpru_updatemask & XPRU_CHARDELAY) && UpdateInfo->xpru_chardelay != -1)
			PrintBox(GAD_TRANSFER_SECONDS,1,ConvNumber,UpdateInfo->xpru_chardelay);

		if((UpdateInfo->xpru_updatemask & XPRU_PACKETDELAY) && UpdateInfo->xpru_packetdelay != -1)
			PrintBox(GAD_TRANSFER_SECONDS,2,ConvNumber,UpdateInfo->xpru_packetdelay);

		if((UpdateInfo->xpru_updatemask & XPRU_PACKETTYPE) && UpdateInfo->xpru_packettype != -1)
		{
			STRPTR FormatSpecifier;

			FormatSpecifier = ConvNumber;

			if(UpdateInfo->xpru_packettype > 32 && UpdateInfo->xpru_packettype < 256)
			{
				if(IsPrintable[UpdateInfo->xpru_packettype])
					FormatSpecifier = "`%lc'";
			}

			PrintBox(GAD_TRANSFER_SECONDS,3,FormatSpecifier,UpdateInfo->xpru_packettype);
		}

		if((UpdateInfo->xpru_updatemask & XPRU_BLOCKCHECK) && UpdateInfo->xpru_blockcheck)
			PrintBox(GAD_TRANSFER_SECONDS,4,UpdateInfo->xpru_blockcheck);

		if((UpdateInfo->xpru_updatemask & XPRU_BLOCKSIZE) && UpdateInfo->xpru_blocksize != -1)
			PrintBox(GAD_TRANSFER_SECONDS,5,ConvNumber,UpdateInfo->xpru_blocksize);


		if((UpdateInfo->xpru_updatemask & XPRU_EXPECTTIME) && UpdateInfo->xpru_expecttime)
		{
			PrintBox(GAD_TRANSFER_TOTALTIME,0,UpdateInfo->xpru_expecttime);

			if(TimeMax = GetSeconds((STRPTR)UpdateInfo->xpru_expecttime))
				NewTime = TRUE;
		}

		if((UpdateInfo->xpru_updatemask & XPRU_ELAPSEDTIME) && UpdateInfo->xpru_elapsedtime)
		{
			PrintBox(GAD_TRANSFER_TOTALTIME,1,UpdateInfo->xpru_elapsedtime);

			TimeVal = GetSeconds((STRPTR)UpdateInfo->xpru_elapsedtime);

			if(TimeMax)
				NewTime = TRUE;
		}

		if((UpdateInfo->xpru_updatemask & XPRU_ERRORS) && UpdateInfo->xpru_errors != -1)
		{
			PrintBox(GAD_TRANSFER_ERRORS,0,ConvNumber,UpdateInfo->xpru_errors);

			if(!ErrorMessageWasDisplayed && Config->TransferConfig->ErrorNotification && UpdateInfo->xpru_errors >= Config->TransferConfig->ErrorNotification)
			{
				ErrorMessageWasDisplayed = TRUE;

				WakeUp(TransferWindow,SOUND_ERROR);
			}
		}

		if((UpdateInfo->xpru_updatemask & XPRU_TIMEOUTS) && UpdateInfo->xpru_timeouts != -1)
			PrintBox(GAD_TRANSFER_ERRORS,1,ConvNumber,UpdateInfo->xpru_timeouts);

		if(NewByte)
		{
			LONG Percent;

			if(ByteMax)
				Percent = (100 * ByteVal) / ByteMax;
			else
				Percent = 0;

			if(Percent != LastPercent)
			{
				UBYTE LocalBuffer[40];

				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%ld%% %s",Percent,LocaleString(MSG_TRANSFERPANEL_DATA_TRANSFERRED_GAD));

				LastPercent = Percent;

				LT_SetAttributes(TransferHandle,GAD_TRANSFER_PERCENT,
					LAGA_Percent,	Percent,
					LAGA_InfoText,	LocalBuffer,
				TAG_DONE);
			}
		}

		if(NewTime)
		{
			LONG TimeDelta,Change;

			if(TimeVal > TimeMax)
				TimeDelta = 0;
			else
				TimeDelta = TimeMax - TimeVal;

			if(LastTimeDelta)
			{
				if(LastTimeDelta < TimeDelta)
					Change = (100 * (TimeDelta - LastTimeDelta)) / LastTimeDelta;
				else
					Change = (100 * (LastTimeDelta - TimeDelta)) / LastTimeDelta;
			}
			else
				Change = 100;

				/* Don't update the display unless the change
				 * exceeds 2% of the previous value.
				 */

			if(Change > 2)
			{
				UBYTE LocalBuffer[40];
				LONG Percent;

				LastTimeDelta = TimeDelta;

				if(TimeMax)
					Percent = (100 * TimeDelta) / TimeMax;
				else
					Percent = 0;

				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%2ld:%02ld:%02ld %s",TimeDelta / 3600,(TimeDelta / 60) % 60,TimeDelta % 60,LocaleString(MSG_TRANSFERPANEL_TIME_TO_GO_GAD));

				LT_SetAttributes(TransferHandle,GAD_TRANSFER_TIME,
					LAGA_Percent,	Percent,
					LAGA_InfoText,	LocalBuffer,
				TAG_DONE);

					/* A rapidly changing transfer time display would
					 * cause the transfer completion time display to
					 * change at the same pace. As the completion time
					 * display calculation is rather costly, a change smaller
					 * than 5% of the previous value is silently ignored.
					 */

				if(Change > 5)
				{
					UBYTE DateTimeBuffer[256];
					struct DateStamp Stamp;

						/* Obtain current time. */

					CurrentTimeToStamp(&Stamp);

						/* Add the time to go. */

					Stamp.ds_Tick	+= (TimeDelta % 60) * TICKS_PER_SECOND;
					Stamp.ds_Minute	+= TimeDelta / 60 + Stamp.ds_Tick / (60 * TICKS_PER_SECOND);
					Stamp.ds_Days	+= Stamp.ds_Minute / 1440;

					Stamp.ds_Tick	%= 60 * TICKS_PER_SECOND;
					Stamp.ds_Minute	%= 1440;

						/* Conversion succeeded? */

					if(FormatStamp(&Stamp,DateTimeBuffer,sizeof(DateTimeBuffer),TRUE))
						PrintBox(GAD_TRANSFER_FILE,3,DateTimeBuffer);
				}
			}
		}
	}

	return(1);
}

	/* xpr_chkabort():
	 *
	 *	Check if the user has aborted the transfer.
	 */

LONG SAVE_DS
xpr_chkabort()
{
	LONG Result;

	DB(kprintf("xpr_chkabort()\n"));

	if(TransferAbortState != 0)
	{
		Result = TransferAbortState;

		TransferAbortState = 0;
	}
	else
		Result = CheckAbort(Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection);

	return(Result);
}

	/* xpr_gets(STRPTR Prompt,STRPTR Buffer):
	 *
	 *	Prompt the user for string input.
	 */

long SAVE_DS ASM
xpr_gets(REG(a0) const char *Prompt,REG(a1) char *Buffer)
{
	enum	{	GAD_OK=1,GAD_CANCEL,GAD_STRING };

	UBYTE LocalBuffer[256];
	LayoutHandle *Handle;
	LONG Success;

	DB(kprintf("xpr_gets(\"%s\",\"%s\")\n",Prompt,Buffer));

	Success = FALSE;

	LimitedStrcpy(sizeof(LocalBuffer),LocalBuffer,Buffer);

	if(!Prompt)
		Prompt = LocaleString(MSG_TERMXPR_INPUT_REQUIRED_TXT);

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelText,	Prompt,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	STRING_KIND,
					LA_STRPTR,	LocalBuffer,
					LA_ID,		GAD_STRING,
					LA_Chars,	30,
				TAG_DONE);

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
					LA_LabelID,		MSG_TERMXPR_OKAY_GAD,
					LA_ID,			GAD_OK,
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
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleText,		LocaleString(MSG_GLOBAL_ENTER_TEXT_TXT),
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
		TAG_DONE))
		{
			struct IntuiMessage *Message;
			ULONG MsgClass,MsgQualifier;
			struct Gadget *MsgGadget;
			UWORD MsgCode;
			BOOL Done;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			LT_Activate(Handle,GAD_STRING);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)GT_GetIMsg(PanelWindow->UserPort))
				{
					MsgClass		= Message->Class;
					MsgQualifier	= Message->Qualifier;
					MsgCode			= Message->Code;
					MsgGadget		= (struct Gadget *)Message->IAddress;

					GT_ReplyIMsg(Message);

					LT_HandleInput(Handle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_STRING:

								if(MsgCode == '\r')
								{
									if(strcmp(Buffer,LocalBuffer))
										NewOptions = TRUE;

									strcpy(Buffer,LocalBuffer);

									Success = Done = TRUE;

									LT_PressButton(Handle,GAD_OK);
								}

								break;

							case GAD_OK:

								LT_UpdateStrings(Handle);

								if(strcmp(Buffer,LocalBuffer))
									NewOptions = TRUE;

								strcpy(Buffer,LocalBuffer);

								Success = Done = TRUE;

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

	return(Success);
}

	/* xpr_setserial(LONG Status):
	 *
	 *	Set/read the serial status (parameters).
	 */

LONG SAVE_DS ASM
xpr_setserial(REG(d0) LONG Status)
{
	DB(kprintf("xpr_setserial(0x%08lx)\n",Status));

	if(WriteRequest)
	{
		STATIC ULONG XprBauds[12] =
		{
			   110,
			   300,
			  1200,
			  2400,
			  4800,
			  9600,
			 19200,
			 31250,
			 38400,
			 57600,
			 76800,
			115200
		};

		LONG Return,i;

		StopSerialRead();
		StopSerialWrite();

		DoSerialCmd(SDCMD_QUERY);

		Return = WriteRequest->io_SerFlags & 0xFF;

		if(WriteRequest->io_ExtFlags & SEXTF_MSPON)
			Return |= ST_PARTYMARKON;

		if(WriteRequest->io_ExtFlags & SEXTF_MARK)
			Return |= ST_PARTYMARK;

		if(WriteRequest->io_StopBits == 2)
			Return |= ST_2BITS;

		if(WriteRequest->io_ReadLen == 7)
			Return |= ST_READ7;

		if(WriteRequest->io_WriteLen == 7)
			Return |= ST_WRITE7;

		for(i = 0 ; i < 12 ; i++)
		{
			if(XprBauds[i] >= WriteRequest->io_Baud)
			{
				Return |= (i << 16);

				break;
			}
		}

			/* Just in case no match was to be made... */

		if(!(Return & 0xFFFF0000))
			Return |= (11 << 16);

			/* Update serial parameters, don't change
			 * the baud rate however.
			 */

		if(Status != -1)
		{
			WriteRequest->io_SerFlags = Status & 0xFF;
			WriteRequest->io_ExtFlags = 0;

			if(Status & ST_PARTYMARKON)
				WriteRequest->io_ExtFlags |= SEXTF_MSPON;

			if(Status & ST_PARTYMARK)
				WriteRequest->io_ExtFlags |= SEXTF_MARK;

			if(Status & ST_2BITS)
				WriteRequest->io_StopBits = 2;
			else
				WriteRequest->io_StopBits = 1;

			if(Status & ST_READ7)
				WriteRequest->io_ReadLen = 7;
			else
				WriteRequest->io_ReadLen = 8;

			if(Status & ST_WRITE7)
				WriteRequest->io_WriteLen = 7;
			else
				WriteRequest->io_WriteLen = 8;

			WriteRequest->IOSer.io_Command = SDCMD_SETPARAMS;
			DoIO((struct IORequest *)WriteRequest);

			CopyMem(WriteRequest,ReadRequest,sizeof(struct IOExtSer));

			ReadRequest->IOSer.io_Message.mn_ReplyPort = ReadPort;

			CopyWriteFlags();

			if(WriteRequest->io_SerFlags & SERF_7WIRE)
			{
				if(Config->SerialConfig->HandshakingProtocol == HANDSHAKING_NONE)
					Config->SerialConfig->HandshakingProtocol = HANDSHAKING_RTSCTS_DSR;
			}
			else
			{
				if(Config->SerialConfig->HandshakingProtocol != HANDSHAKING_NONE)
					Config->SerialConfig->HandshakingProtocol = HANDSHAKING_NONE;
			}
		}

		return(Return);
	}
	else
		return(-1);
}

	/* xpr_ffirst(STRPTR Buffer,STRPTR Pattern):
	 *
	 *	Batch file upload: find the first matching file and return
	 *	its name.
	 */

long SAVE_DS ASM
xpr_ffirst(REG(a0) char *Buffer,REG(a1) char *UnusedPattern)
{
	DB(kprintf("xpr_ffirst(0x%08lx,\"%s\")\n",Buffer,Pattern));

	if(FileTransferInfo)
	{
		FileTransferInfo->DoneSize	= 0;
		FileTransferInfo->DoneFiles	= 0;

		FileTransferInfo->CurrentFile	= (struct FileTransferNode *)FileTransferInfo->FileList.mlh_Head;
		FileTransferInfo->CurrentSize	= FileTransferInfo->CurrentFile->Size;

		if(Config->TransferConfig->MangleFileNames)
		{
			UBYTE LocalName[MAX_FILENAME_LENGTH],*Char;

			LimitedStrcpy(sizeof(OriginalName),OriginalName,FileTransferInfo->CurrentFile->Name);

			ShrinkName(FilePart(FileTransferInfo->CurrentFile->Name),LocalName,12,TRUE);

			strcpy(Buffer,FileTransferInfo->CurrentFile->Name);

			Char = PathPart(Buffer);
			*Char = 0;

			AddPart(Buffer,LocalName,MAX_FILENAME_LENGTH);

			LimitedStrcpy(sizeof(ShrunkenName),ShrunkenName,Buffer);
		}
		else
		{
			OriginalName[0] = 0;

			strcpy(Buffer,FileTransferInfo->CurrentFile->Name);
		}

		return(1);
	}
	else
		return(0);
}

	/* xpr_fnext(LONG OldState,STRPTR Buffer,STRPTR Pattern):
	 *
	 *	Batch file upload: find the next matching file
	 *	- if any - and return its name.
	 */

long SAVE_DS ASM
xpr_fnext(REG(d0) long UnusedOldState,REG(a0) char *Buffer,REG(a1) char *UnusedPattern)
{
	DB(kprintf("xpr_fnext(%ld,0x%08lx,\"%s\")\n",OldState,Buffer,Pattern));

	if(FileTransferInfo)
	{
		if(FileTransferInfo->CurrentFile->MinNode.mln_Succ->mln_Succ)
		{
			FileTransferInfo->DoneSize	+= FileTransferInfo->CurrentSize;
			FileTransferInfo->DoneFiles	+= 1;

			FileTransferInfo->CurrentFile  = (struct FileTransferNode *)FileTransferInfo->CurrentFile->MinNode.mln_Succ;
			FileTransferInfo->CurrentSize  = FileTransferInfo->CurrentFile->Size;

			if(Config->TransferConfig->MangleFileNames)
			{
				UBYTE LocalName[MAX_FILENAME_LENGTH],*Char;

				LimitedStrcpy(sizeof(OriginalName),OriginalName,FileTransferInfo->CurrentFile->Name);

				ShrinkName(FilePart(FileTransferInfo->CurrentFile->Name),LocalName,12,TRUE);

				strcpy(Buffer,FileTransferInfo->CurrentFile->Name);

				Char = PathPart(Buffer);

				*Char = 0;

				AddPart(Buffer,LocalName,MAX_FILENAME_LENGTH);

				LimitedStrcpy(sizeof(ShrunkenName),ShrunkenName,Buffer);
			}
			else
			{
				OriginalName[0] = 0;

				strcpy(Buffer,FileTransferInfo->CurrentFile->Name);
			}

			return(1);
		}
	}

	return(0);
}

	/* xpr_finfo(STRPTR FileName,LONG InfoType):
	 *
	 *	Return information on a given file.
	 */

long SAVE_DS ASM
xpr_finfo(REG(a0) char *FileName,REG(d0) long InfoType)
{
	UBYTE RealName[MAX_FILENAME_LENGTH];

	DB(kprintf("xpr_finfo(\"%s\",%ld)\n",FileName,InfoType));

	switch(InfoType)
	{
			/* Return the file size. */

		case 1:

			if(OriginalName[0])
			{
				if(!Stricmp(ShrunkenName,FileName))
					FileName = NewFileName(OriginalName,RealName,sizeof(RealName));
				else
					FileName = NewFileName(FileName,RealName,sizeof(RealName));
			}
			else
				FileName = NewFileName(FileName,RealName,sizeof(RealName));

			return(GetFileSize(FileName));

			/* Return the file transfer mode. */

		case 2:

			return(BinaryTransfer ? 1 : 2);

			/* Ignore the rest. */

		default:

			return(0);
	}
}

	/* xpr_options(LONG NumOpts,struct xpr_option **Opts):
	 *
	 *	Provide a more polished user interface to set the
	 *	transfer protocol options.
	 */

long SAVE_DS ASM
xpr_options(REG(d0) long NumOpts,REG(a0) struct xpr_option **Opts)
{
	DB(kprintf("xpr_options(%ld,0x%08lx)\n",NumOpts,Opts));

	if(NumOpts && Opts)
	{
		enum	{	GAD_Use=1,GAD_Cancel,GAD_Special };

		LayoutHandle *Handle;
		ULONG Flags;

		Flags = 0;

			/* We only have 32 bits! */

		if(NumOpts > 32)
			NumOpts = 32;

		if(Handle = LT_CreateHandleTags(Window->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			struct Window *PanelWindow;
			LONG i,Split;

			if(NumOpts > 16)
				Split = NumOpts / 2;
			else
				Split = -1;

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						for(i = 0 ; i < NumOpts ; i++)
						{
							if(Opts[i])
							{
								switch(Opts[i]->xpro_type)
								{
									case XPRO_BOOLEAN:

										LT_New(Handle,
											LA_Type,		CHECKBOX_KIND,
											LA_LabelText,	Opts[i]->xpro_description,
											LA_ID,			GAD_Special + i,
											GTCB_Checked,	String2Boolean(Opts[i]->xpro_value),
										TAG_DONE);

										break;

									case XPRO_LONG:

										LT_New(Handle,
											LA_Type,				INTEGER_KIND,
											LA_LabelText,			Opts[i]->xpro_description,
											LA_ID,					GAD_Special + i,
											LA_Chars,				15,
											GTIN_Number,			Atol(Opts[i]->xpro_value),
											LAIN_UseIncrementers,	TRUE,
										TAG_DONE);

										break;

									case XPRO_STRING:

										LT_New(Handle,
											LA_Type,		STRING_KIND,
											LA_LabelText,	Opts[i]->xpro_description,
											LA_ID,			GAD_Special + i,
											LA_Chars,		15,
											GTST_String,	Opts[i]->xpro_value,
											GTST_MaxChars,	Opts[i]->xpro_length - 1,
										TAG_DONE);

										break;

									case XPRO_COMMPAR:

										LT_New(Handle,
											LA_Type,		STRING_KIND,
											LA_LabelText,	Opts[i]->xpro_description,
											LA_ID,			GAD_Special + i,
											LA_Chars,		15,
											LA_HighLabel,	TRUE,
											GTST_String,	Opts[i]->xpro_value,
											GTST_MaxChars,	Opts[i]->xpro_length - 1,
										TAG_DONE);

										break;

									case XPRO_HEADER:

										LT_New(Handle,
											LA_Type,		TEXT_KIND,
											LA_LabelText,	Opts[i]->xpro_description,
											LA_HighLabel,	TRUE,
											GTTX_Text,		" ",
										TAG_DONE);

										break;

									case XPRO_COMMAND:

										LT_New(Handle,
											LA_Type,		BUTTON_KIND,
											LA_LabelText,	Opts[i]->xpro_description,
											LA_ID,			GAD_Special + i,
											LA_Chars,		15,
										TAG_DONE);

										break;
								}
							}

							if(i == Split)
							{
								LT_EndGroup(Handle);

								LT_New(Handle,
									LA_Type,VERTICAL_KIND,
								TAG_DONE);
								{
									LT_New(Handle,
										LA_Type,YBAR_KIND,
									TAG_DONE);

									LT_EndGroup(Handle);
								}

								LT_New(Handle,
									LA_Type,VERTICAL_KIND,
								TAG_DONE);
							}
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
						LA_LabelID,		MSG_GLOBAL_SAVE_TXT,
						LA_ID,			GAD_Use,
						LABT_ReturnKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
						LA_ID,			GAD_Cancel,
						LABT_EscKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			if(PanelWindow = LT_Build(Handle,
				LAWN_TitleText,		OptionTitle ? OptionTitle : LocaleString(MSG_TERMXPR_TRANSFER_OPTIONS_TXT),
				LAWN_HelpHook,		&GuideHook,
				LAWN_Parent,		Window,
				WA_DepthGadget,		TRUE,
				WA_DragBar,			TRUE,
				WA_RMBTrap,			TRUE,
				WA_Activate,		TRUE,
			TAG_DONE))
			{
				struct IntuiMessage	*Message;
				ULONG MsgClass,MsgQualifier;
				struct Gadget *MsgGadget;
				BOOL CheckFlags;
				UWORD MsgCode;
				BOOL Done;

				CheckFlags = FALSE;

				PushWindow(PanelWindow);

				LT_ShowWindow(Handle,TRUE);

				Done = FALSE;

				do
				{
					if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
						break;

					while(Message = (struct IntuiMessage *)GT_GetIMsg(PanelWindow->UserPort))
					{
						MsgClass		= Message->Class;
						MsgQualifier	= Message->Qualifier;
						MsgCode			= Message->Code;
						MsgGadget		= (struct Gadget *)Message->IAddress;

						GT_ReplyIMsg(Message);

						LT_HandleInput(Handle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

						if(MsgClass == IDCMP_GADGETUP)
						{
							switch(MsgGadget->GadgetID)
							{
								case GAD_Use:

									Done = CheckFlags = TRUE;
									break;

								case GAD_Cancel:

									Done = TRUE;
									break;

								default:

									if(MsgGadget->GadgetID - GAD_Special < NumOpts)
									{
										i = MsgGadget->GadgetID - GAD_Special;

										if(Opts[i]->xpro_type == XPRO_COMMAND || (Opts[i]->xpro_type == XPRO_COMMPAR && MsgCode != '\t'))
										{
											Flags = (1L << i);

											XPRCommandSelected = Done = CheckFlags = TRUE;
										}
									}

									break;
							}
						}
					}
				}
				while(!Done);

				PopWindow();

				if(CheckFlags)
				{
					LT_LockWindow(PanelWindow);

					for(i = 0 ; i < NumOpts ; i++)
					{
						if(Opts[i])
						{
							switch(Opts[i]->xpro_type)
							{
								case XPRO_BOOLEAN:

									if(LT_GetAttributes(Handle,GAD_Special + i,TAG_DONE) != String2Boolean(Opts[i]->xpro_value))
									{
										Flags |= (1L << i);

										if(LT_GetAttributes(Handle,GAD_Special + i,TAG_DONE))
											strcpy(Opts[i]->xpro_value,"yes");
										else
											strcpy(Opts[i]->xpro_value,"no");

										NewOptions = TRUE;
									}

									break;

								case XPRO_LONG:

									if(Atol(Opts[i]->xpro_value) != LT_GetAttributes(Handle,GAD_Special + i,TAG_DONE))
									{
										Flags |= (1L << i);

										SPrintf(Opts[i]->xpro_value,"%ld",LT_GetAttributes(Handle,GAD_Special + i,TAG_DONE));

										NewOptions = TRUE;
									}

									break;

								case XPRO_STRING:

									if(strcmp(Opts[i]->xpro_value,LT_GetString(Handle,GAD_Special + i)))
									{
										Flags |= (1L << i);

										strcpy(Opts[i]->xpro_value,LT_GetString(Handle,GAD_Special + i));

										NewOptions = TRUE;
									}

									break;
							}
						}
					}

					LT_UnlockWindow(PanelWindow);
				}
				else
					Flags = 0;
			}

			LT_DeleteHandle(Handle);
		}

		return(Flags);
	}
	else
		return(0);
}

	/* xpr_unlink(STRPTR FileName):
	 *
	 *	Remove (delete) a given file.
	 */

long SAVE_DS ASM
xpr_unlink(REG(a0) char *FileName)
{
	DB(kprintf("xpr_unlink(\"%s\")\n",FileName));

		/* Sorry, but it is far too dangerous to let the protocol
		 * remove any files with path name substitution enabled.
		 * The protocol could accidentally hit the wrong file.
		 */

	if(!Config->TransferConfig->OverridePath)
	{
		if(OriginalName[0])
		{
			if(!Stricmp(ShrunkenName,FileName))
				FileName = OriginalName;
		}

		if(DeleteFile(FileName))
		{
			LogAction(LocaleString(MSG_TERMXPR_LOGMSG_DELETE_FILE_TXT),FilePart(FileName));

			return(0);
		}
	}

	return(-1);
}

	/* xpr_squery():
	 *
	 *	Check how many characters are present in the serial buffer.
	 */

LONG SAVE_DS
xpr_squery()
{
	DB(kprintf("xpr_squery()\n"));

	if(WriteRequest)
	{
		ULONG Waiting;
		UWORD Status;

		GetSerialInfo(&Waiting,&Status);

			/* Return error if carrier is lost. */

		if(!CheckForCarrierLoss(Status))
			return((LONG)Waiting);
	}

	return(-1);
}

	/* xpr_getptr(LONG InfoType):
	 *
	 *	Return a pointer to the term custom screen.
	 */

APTR SAVE_DS ASM
xpr_getptr(REG(d0) LONG InfoType)
{
	DB(kprintf("xpr_getptr(%ld)\n",InfoType));

	if(InfoType == 1)
		return((APTR)Window->WScreen);
	else
		return((APTR)-1);
}

	/* xpr_stealopts(STRPTR Prompt,STRPTR Buffer):
	 *
	 *	Steal the contents of the options buffer (replacement
	 *	for xpr_gets).
	 */

long SAVE_DS ASM
xpr_stealopts(REG(a0) const char *UnusedPrompt,REG(a1) char *Buffer)
{
	DB(kprintf("xpr_stealopts(\"%s\",\"%s\")\n",Prompt,Buffer));

	if(Buffer)
		LimitedStrcpy(sizeof(ProtocolOptsBuffer),ProtocolOptsBuffer,Buffer);

	return(1);
}
