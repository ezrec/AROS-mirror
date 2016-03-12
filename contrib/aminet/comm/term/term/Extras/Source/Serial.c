/*
**	Serial.c
**
**	Serial driver support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* In case the DSR test is enabled */

STATIC BOOL UseRTS_CTS;

	/* Serial write output routines */

STATIC SERWRITE SerWriteRoutine,SerWriteAndProcessRoutine;

STATIC VOID
WriteAndProcessUserMon(APTR Buffer,LONG Size)
{
	Size = XProtocolUserMon(XprIO,Buffer,Size,Size);

	if(Size > 0)
	{
		StartSerialWrite(Buffer,Size);

		if(Marking)
			WindowMarkerStop();

		ConProcess(Buffer,Size);

		WaitSerialWrite();

		BytesOut += Size;
	}
}

STATIC VOID
WriteUserMon(APTR Buffer,LONG Size)
{
	Size = XProtocolUserMon(XprIO,Buffer,Size,Size);

	if(Size > 0)
	{
		DoSerialWrite(Buffer,Size);

		BytesOut += Size;
	}
}

STATIC VOID
WriteAndProcessBypass(APTR Buffer,LONG Size)
{
	if((*SerWriteBypass)(Buffer,Size))
	{
		StartSerialWrite(Buffer,Size);

		ConProcess(Buffer,Size);

		WaitSerialWrite();

		BytesOut += Size;
	}
}

STATIC VOID
WriteBypass(APTR Buffer,LONG Size)
{
	if((*SerWriteBypass)(Buffer,Size))
	{
		DoSerialWrite(Buffer,Size);

		BytesOut += Size;
	}
}

STATIC VOID
WriteAndProcessBypassUserMon(APTR Buffer,LONG Size)
{
	if((*SerWriteBypass)(Buffer,Size))
	{
		Size = XProtocolUserMon(XprIO,Buffer,Size,Size);

		if(Size > 0)
		{
			StartSerialWrite(Buffer,Size);

			ConProcess(Buffer,Size);

			WaitSerialWrite();

			BytesOut += Size;
		}
	}
}

STATIC VOID
WriteBypassUserMon(APTR Buffer,LONG Size)
{
	if((*SerWriteBypass)(Buffer,Size))
	{
		Size = XProtocolUserMon(XprIO,Buffer,Size,Size);

		if(Size > 0)
		{
			DoSerialWrite(Buffer,Size);

			BytesOut += Size;
		}
	}
}

	/* WriteAndProcess(APTR Buffer,LONG Size):
	 *
	 *	At the same time write data to the serial port and
	 *	process it on the screen.
	 */

STATIC VOID
WriteAndProcess(APTR Buffer,LONG Size)
{
	StartSerialWrite(Buffer,Size);

	ConProcess(Buffer,Size);

	WaitSerialWrite();

	BytesOut += Size;
}

	/* WriteDirect(APTR Buffer,LONG Size):
	 *
	 *	Output the data and update the byte counter.
	 */

STATIC VOID
WriteDirect(APTR Buffer,LONG Size)
{
	DoSerialWrite(Buffer,Size);

	BytesOut += Size;
}

	/* SetFlags():
	 *
	 *	Set the contents of a serial device request according
	 *	to the current configuration settings.
	 */

STATIC BYTE
SetFlags(VOID)
{
	return(SetBothSerialAttributes(
		SERA_Baud,			Config->SerialConfig->BaudRate,
		SERA_BreakTime,		Config->SerialConfig->BreakLength,
		SERA_BitsPerChar,	Config->SerialConfig->BitsPerChar,
		SERA_StopBits,		Config->SerialConfig->StopBits,
		SERA_BufferSize,	Config->SerialConfig->SerialBufferSize,
		SERA_Parity,		Config->SerialConfig->Parity,
		SERA_Handshaking,	UseRTS_CTS ? Config->SerialConfig->HandshakingProtocol : HANDSHAKING_NONE,
		SERA_HighSpeed,		Config->SerialConfig->HighSpeed,
		SERA_Shared,		Config->SerialConfig->Shared,
	TAG_DONE));
}

	/* SendBreak():
	 *
	 *	Transmit a break signal.
	 */

VOID
SendBreak()
{
	PushStatus(STATUS_BREAKING);

	DoSerialCmd(SDCMD_BREAK);

	PopStatus();
}

	/* HangUp():
	 *
	 *	Hang up the line.
	 */

VOID
HangUp()
{
	PushStatus(STATUS_HANGUP);

	StopSerialWrite();

		/* Are we to drop the DTR line
		 * before sending the hangup
		 * string?
		 */

	if(Config->ModemConfig->DropDTR)
	{
		if(!DropDTR(Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber))
		{
			DeleteSerial();

			if(!ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_REOPEN_UNIT_TXT),LocaleString(MSG_TERMMAIN_IGNORE_QUIT_TXT),Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber))
				MainTerminated = TRUE;
		}
	}

		/* Transmit the hangup command. */

	if(Config->ModemConfig->ModemHangup[0] && !MainTerminated)
		SerialCommand(Config->ModemConfig->ModemHangup);

		/* Reset to old status. */

	PopStatus();
}

	/* CopyWriteFlags():
	 *
	 *	Update configuration with serial settings.
	 */

VOID
CopyWriteFlags()
{
	ULONG	Baud,
			BreakTime,
			BitsPerChar,
			StopBits,
			BufferSize,
			Parity,
			Handshaking,
			HighSpeed,
			Shared;

	GetSerialWriteAttributes(
		SERA_Baud,			&Baud,
		SERA_BreakTime,		&BreakTime,
		SERA_BitsPerChar,	&BitsPerChar,
		SERA_StopBits,		&StopBits,
		SERA_BufferSize,	&BufferSize,
		SERA_Parity,		&Parity,
		SERA_Handshaking,	&Handshaking,
		SERA_HighSpeed,		&HighSpeed,
		SERA_Shared,		&Shared,
	TAG_DONE);

	Config->SerialConfig->BaudRate				= Baud;
	Config->SerialConfig->BreakLength			= BreakTime;
	Config->SerialConfig->BitsPerChar			= BitsPerChar;
	Config->SerialConfig->StopBits				= StopBits;
	Config->SerialConfig->SerialBufferSize		= BufferSize;
	Config->SerialConfig->Parity				= Parity;
	Config->SerialConfig->HandshakingProtocol	= Handshaking;
	Config->SerialConfig->HighSpeed				= HighSpeed;
	Config->SerialConfig->Shared				= Shared;
}

	/* SerWriteVerbatim(APTR Buffer,LONG Size,BOOL Echo):
	 *
	 *	The `no fancy features' version of SerWrite().
	 */

VOID
SerWriteVerbatim(APTR Buffer,LONG Size,BOOL Echo)
{
	if(XProtocolBase && (TransferBits & XPRS_USERMON))
		Size = XProtocolUserMon(XprIO,Buffer,Size,Size);

	if(Size > 0)
	{
		if(Echo)
			WriteAndProcess(Buffer,Size);
		else
			DoSerialWrite(Buffer,Size);

		BytesOut += Size;
	}
}

	/* SerWriteUpdate():
	 *
	 *	Choose the right data output routine for the job.
	 */

VOID
SerWriteUpdate()
{
	if(SerWriteBypass)
	{
		if(XProtocolBase && (TransferBits & XPRS_USERMON))
		{
			SerWriteRoutine = (SERWRITE)WriteBypassUserMon;
			SerWriteAndProcessRoutine = (SERWRITE)WriteAndProcessBypassUserMon;
		}
		else
		{
			SerWriteRoutine = (SERWRITE)WriteBypass;
			SerWriteAndProcessRoutine = (SERWRITE)WriteAndProcessBypass;
		}
	}
	else
	{
		if(XProtocolBase && (TransferBits & XPRS_USERMON))
		{
			SerWriteRoutine = (SERWRITE)WriteUserMon;
			SerWriteAndProcessRoutine = (SERWRITE)WriteAndProcessUserMon;
		}
		else
		{
			SerWriteRoutine = (SERWRITE)WriteDirect;
			SerWriteAndProcessRoutine = (SERWRITE)WriteAndProcess;
		}
	}
}

	/* SerWrite(APTR Buffer,LONG Size):
	 *
	 *	Send a number of bytes across the serial line.
	 */

VOID
SerWrite(APTR Buffer,LONG Size)
{
	if(Size < 0)
		Size = strlen(Buffer);

	if(Size > 0)
	{
		if(RememberInput)
		{
			RememberInputText(Buffer,Size);

			if(((UBYTE *)Buffer)[Size - 1] == '\r' && RecordingLine)
			{
				RememberSpill();

				RecordingLine = FALSE;

				RememberOutput = TRUE;
				RememberInput = FALSE;

				CheckItem(MEN_RECORD_LINE,FALSE);
			}
		}
		else
		{
			if(RememberOutput)
			{
				RememberInputText(Buffer,Size);

				RememberSpill();
			}
		}

		if(WriteRequest)
		{
			if(SendTable)
			{
				struct TranslationHandle Handle;
				UBYTE TranslateData[256];

					/* Set up for buffer translation. */

				TranslateSetup(&Handle,Buffer,Size,TranslateData,sizeof(TranslateData),SendTable);

					/* Full or half duplex? */

				if(Config->SerialConfig->Duplex == DUPLEX_FULL)
				{
						/* Process the data... */

					while(Size = TranslateBuffer(&Handle))
						(*SerWriteRoutine)(TranslateData,Size);
				}
				else
				{
					while(Size = TranslateBuffer(&Handle))
						(*SerWriteAndProcessRoutine)(TranslateData,Size);
				}
			}
			else
			{
				if(Config->SerialConfig->Duplex == DUPLEX_FULL)
					(*SerWriteRoutine)(Buffer,Size);
				else
					(*SerWriteAndProcessRoutine)(Buffer,Size);
			}
		}
	}
}

	/* RestartSerial():
	 *
	 *	Restart read/write activity on the serial line.
	 */

VOID
RestartSerial()
{
		/* Blow away the cache if there is anything left of it. */

	if(DataHold)
	{
		DataHold = NULL;

		UpdateSerialJob();
	}

	StartSerialRead(&ReadChar,1);
}

	/* ClearSerial():
	 *
	 *	Terminate all read/write activity on the serial line.
	 */

VOID
ClearSerial()
{
		/* Blow away the cache if there is anything left of it. */

	if(DataHold)
	{
		DataHold = NULL;

		UpdateSerialJob();
	}

	StopSerialRead();

	FlushSerialRead();
}

	/* DeleteSerial():
	 *
	 *	Close the serial device and release all associated
	 *	resources.
	 */

VOID
DeleteSerial()
{
	StopSerialRead();
	StopSerialWrite();

	if(ReadRequest)
	{
		if(ReadRequest->IOSer.io_Device)
		{
			UBYTE Name[MAX_FILENAME_LENGTH];
			struct Device *Device;

				/* Unless the device was opened in shared */
				/* access mode we'll reset it here */

			if(!Config->SerialConfig->Shared)
			{
				ReadRequest->IOSer.io_Command = CMD_RESET;

				DoIO((struct IORequest *)ReadRequest);
			}

				/* Remember the device name, we'll need it after */
				/* closing time */

			LimitedStrcpy(sizeof(Name),Name,ReadRequest->IOSer.io_Device->dd_Library.lib_Node.ln_Name);

				/* Close the device */

			CloseDevice((struct IORequest *)ReadRequest);

				/* And remove it from memory */

			Forbid();

			if(Device = (struct Device *)FindName(&SysBase->DeviceList,Name))
				RemDevice(Device);

			Permit();
		}

		DeleteIORequest(ReadRequest);
		ReadRequest = NULL;
	}

	if(WriteRequest)
	{
		DeleteIORequest(WriteRequest);
		WriteRequest = NULL;
	}

	if(WritePort)
	{
		DeleteMsgPort(WritePort);
		WritePort = NULL;
	}

	if(ReadPort)
	{
		DeleteMsgPort(ReadPort);
		ReadPort = NULL;
	}

	FreeVecPooled(ReadBuffer);
	ReadBuffer = NULL;

	FreeVecPooled(HostReadBuffer);
	HostReadBuffer = NULL;

	FreeVecPooled(StripBuffer);
	StripBuffer = NULL;

	UnlockDevice();

	UpdateSerialJob();
}

	/* GetSerialError(LONG Error,BOOL *Reset):
	 *
	 *	Return an error message for each possible serial device error.
	 */

CONST_STRPTR
GetSerialError(LONG Error, BOOL *ResetPtr)
{
	BOOL Reset;
	LONG ID;

        if (!ResetPtr)
            ResetPtr = &Reset;

	*ResetPtr	= FALSE;
	ID		= -1;

	switch(Error)
	{
		case IOERR_BADLENGTH:
		case IOERR_BADADDRESS:
		case IOERR_SELFTEST:
		case IOERR_OPENFAIL:

			ID = MSG_SERIAL_OPENFAILURE_TXT;
			break;

		case SerErr_DevBusy:

			ID = MSG_SERIAL_ERROR_DEVBUSY_TXT;
			break;

		case SerErr_BaudMismatch:

			*ResetPtr = TRUE;
			ID = MSG_SERIAL_ERROR_BAUDMISMATCH_TXT;
			break;

		case SerErr_BufErr:

			*ResetPtr = TRUE;
			ID = MSG_SERIAL_ERROR_BUFERR_TXT;
			break;

		case SerErr_InvParam:

			*ResetPtr = TRUE;
			ID = MSG_SERIAL_ERROR_INVPARAM_TXT;
			break;

		case SerErr_LineErr:

			ID = MSG_SERIAL_ERROR_LINEERR_TXT;
			break;

		case SerErr_ParityErr:

			*ResetPtr = TRUE;
			ID = MSG_SERIAL_ERROR_PARITYERR_TXT;
			break;

		case SerErr_TimerErr:

			ID = MSG_SERIAL_ERROR_TIMERERR_TXT;
			break;

		case SerErr_BufOverflow:

			*ResetPtr = TRUE;
			ID = MSG_SERIAL_ERROR_BUFOVERFLOW_TXT;
			break;

		case SerErr_NoDSR:

			ID = MSG_SERIAL_ERROR_NODSR_TXT;
			break;

	/*	case SerErr_UnitBusy:	*/
		case IOERR_UNITBUSY:
		case 16:

			ID = MSG_SERIAL_ERROR_UNIT_BUSY_TXT;
			break;
	}

	if(ID != -1)
	{
		return(LocaleString(ID));
	}
	else
		return(NULL);
}

	/* CreateSerial():
	 *
	 *	Create handles for the serial device and open it.
	 */

BOOL
CreateSerial(STRPTR ErrorBuffer,LONG ErrorBufferSize)
{
	BYTE Error = 0;
	LONG ID = -1;

		/* Try to lock the serial device driver if necessary */

	if(Config->SerialConfig->UseOwnDevUnit && !(Config->SerialConfig->Shared && Config->SerialConfig->NoODUIfShared))
	{
		if(!LockDevice(Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber,ErrorBuffer,ErrorBufferSize))
			return(FALSE);
	}

		/* Propagate the serial read buffer size to the translation routines */

	Update_CR_LF_Translation();

		/* Now allocate the required resources... */

	if(ReadBuffer = AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY))
	{
		if(StripBuffer = AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY))
		{
			if(XProtocolBase && (TransferBits & XPRS_HOSTNOWAIT))
				HostReadBuffer = AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY);

			if(ReadPort = (struct MsgPort *)CreateMsgPort())
			{
				if(ReadRequest = (struct IOExtSer *)CreateIORequest(ReadPort,sizeof(struct IOExtSer)))
				{
					if(WritePort = (struct MsgPort *)CreateMsgPort())
					{
						if(WriteRequest = (struct IOExtSer *)CreateIORequest(WritePort,sizeof(struct IOExtSer)))
						{
								/* Unless the user doesn't want RTS/CTS handshaking */
								/* we'll assume it should be enabled. This also concerns */
								/* the RTS/CTS+DSR test mode which will also start */
								/* with having RTS/CTS handshaking enabled. */

							UseRTS_CTS = (BOOL)(Config->SerialConfig->HandshakingProtocol != HANDSHAKING_NONE);

								/* Set the default opening parameters */

							SetSerialReadAttributes(
								SERA_Baud,			Config->SerialConfig->BaudRate,
								SERA_BreakTime,		Config->SerialConfig->BreakLength,
								SERA_BitsPerChar,	Config->SerialConfig->BitsPerChar,
								SERA_StopBits,		Config->SerialConfig->StopBits,
								SERA_BufferSize,	Config->SerialConfig->SerialBufferSize,
								SERA_Parity,		Config->SerialConfig->Parity,
								SERA_Handshaking,	Config->SerialConfig->HandshakingProtocol,
								SERA_HighSpeed,		Config->SerialConfig->HighSpeed,
								SERA_Shared,		Config->SerialConfig->Shared,
							TAG_DONE);

								/* Open the device driver */

							if(!(Error = OpenSerialDevice(Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber)))
							{
									/* Should we perform a DSR signal test before */
									/* proceeding? */

								if(Config->SerialConfig->HandshakingProtocol == HANDSHAKING_RTSCTS_DSR)
								{
										/* Now check if the DSR signal is enabled. */

#ifdef __AROS__
									if(GetSerialStatus() & SER_STATF_COMDSR)
#else
									if(GetSerialStatus() & CIAF_COMDSR)
#endif
									{
											/* If this is the first time the driver gets initialized, */
											/* skip the notification message and just drop the RTS/CTS */
											/* handshaking bit. */

										if(FirstInvocation)
											UseRTS_CTS = FALSE;
										else
										{
											do
											{
												if(!ShowRequest(Window,LocaleString(MSG_SERIAL_NO_DSR_SIGNAL_TXT),LocaleString(MSG_SERIAL_RETRY_CANCEL_TXT)))
													UseRTS_CTS = FALSE;
											}
#ifdef __AROS__
											while(UseRTS_CTS && (GetSerialStatus() & SER_STATF_COMDSR));
#else
											while(UseRTS_CTS && (GetSerialStatus() & CIAF_COMDSR));
#endif
										}
									}
								}

									/* Update the driver data */

								ResetSerialRead();
								ResetSerialWrite();

									/* Set the parameters */

								SetFlags();

									/* And ask for a byte */

								RestartSerial();

								UpdateSerialJob();

								return(TRUE);
							}
						}
					}
					else
						ID = MSG_SERIAL_FAILED_TO_CREATE_WRITE_PORT_TXT;
				}
			}
			else
				ID = MSG_SERIAL_FAILED_TO_CREATE_READ_PORT_TXT;
		}
	}

		/* If we get here something must be wrong. */

	DeleteSerial();

		/* If we got an open error, we'll try to make it into */
		/* a readable description */

	if(Error != 0)
	{
		CONST_STRPTR String;

		if(!(String = GetSerialError(Error,NULL)))
			String = LocaleString(MSG_SERIAL_ERROR_DEVBUSY_TXT);

		LimitedSPrintf(ErrorBufferSize,ErrorBuffer,String,Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber);
	}
	else
	{
			/* If no open error has occured a problem must have */
			/* hit us before the device was ready to open. */
			/* If the error message ID is negative we'll use the */
			/* default message which indicates memory allocation */
			/* problems. */

		if(ID == -1)
			ID = MSG_SERIAL_NOT_ENOUGH_MEMORY_TXT;

		strcpy(ErrorBuffer,LocaleString(ID));
	}

	return(FALSE);
}

	/* ReconfigureSerial(struct Window *Window,struct SerialSettings *SerialConfig):
	 *
	 *	Reconfigure the serial driver according to the new
	 *	serial settings.
	 */

LONG
ReconfigureSerial(struct Window *Window,struct SerialSettings *SerialConfig)
{
	LONG Success = RECONFIGURE_NOCHANGE;

		/* Are new settings to be installed or have they already */
		/* been copied to the correct locations? */

	if(CompareConfigEntries(Config->SerialConfig,SerialConfig ? SerialConfig : PrivateConfig->SerialConfig,PREF_SERIAL))
	{
		BOOL SameDevice = TRUE;

			/* Any new data to swap in? */

		if(SerialConfig)
		{
				/* Store the previous settings. */

			SaveConfig(Config,PrivateConfig);

				/* Install the new settings */

			PutConfigEntry(Config,SerialConfig,PREF_SERIAL);
		}

			/* Any device name or unit number change? */

		if(strcmp(PrivateConfig->SerialConfig->SerialDevice,Config->SerialConfig->SerialDevice) || PrivateConfig->SerialConfig->UnitNumber != Config->SerialConfig->UnitNumber || PrivateConfig->SerialConfig->UseOwnDevUnit != Config->SerialConfig->UseOwnDevUnit)
			SameDevice = FALSE;

			/* Handshaking mode changed? */

		if((PrivateConfig->SerialConfig->HandshakingProtocol == HANDSHAKING_NONE && Config->SerialConfig->HandshakingProtocol != HANDSHAKING_NONE) || (PrivateConfig->SerialConfig->HandshakingProtocol != HANDSHAKING_NONE && Config->SerialConfig->HandshakingProtocol == HANDSHAKING_NONE))
			SameDevice = FALSE;

			/* Did the ODU options change? */

		if(PrivateConfig->SerialConfig->NoODUIfShared != Config->SerialConfig->NoODUIfShared && Config->SerialConfig->Shared)
			SameDevice = FALSE;

			/* Is the device currently closed, i.e. should we open it? */

		if(!ReadRequest)
			SameDevice = FALSE;

			/* Stop any IO activity. */

		ClearSerial();

			/* No dramatic changes? Simply change the parameters. */

		if(SameDevice)
		{
			LONG Error;

				/* Did the buffer size change? */

			if(PrivateConfig->SerialConfig->SerialBufferSize != Config->SerialConfig->SerialBufferSize)
			{
				STRPTR NewReadBuffer;
				STRPTR NewStripBuffer;

					/* Allocate two new buffers */

				NewReadBuffer	= AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY);
				NewStripBuffer	= AllocVecPooled(Config->SerialConfig->SerialBufferSize,MEMF_ANY);

					/* Did we get both? */

				if(NewReadBuffer && NewStripBuffer)
				{
						/* Release the old buffers */

					FreeVecPooled(ReadBuffer);
					FreeVecPooled(StripBuffer);

						/* And put the new ones in */

					ReadBuffer	= NewReadBuffer;
					StripBuffer	= NewStripBuffer;
				}
				else
				{
						/* Release whatever we had allocated */

					FreeVecPooled(NewReadBuffer);
					FreeVecPooled(NewStripBuffer);

						/* And change back to the old buffer size */

					Config->SerialConfig->SerialBufferSize = PrivateConfig->SerialConfig->SerialBufferSize;
				}
			}

				/* Propagate the buffer size to the translation routines */

			Update_CR_LF_Translation();

				/* Use new parameters */

			if(Error = SetFlags())
			{
				UBYTE LocalBuffer[256];
				CONST_STRPTR String;
				BOOL Reset;

					/* Get the error message. */

				if(!(String = GetSerialError(Error, &Reset)))
					String = LocaleString(MSG_SERIAL_ERROR_DEVBUSY_TXT);

					/* Build the device name/unit number string. */

				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,String,Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber);

					/* Display the error requester. */

				ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocalBuffer);

					/* Is a serial driver reset required? */

				if(Reset)
				{
						/* Execute the reset command. */

					DoSerialCmd(CMD_RESET);

						/* Copy the serial driver settings */
						/* to the global configuration. */

					CopyWriteFlags();

						/* Also set the read request driver flags */

					SetFlags();
				}
			}

				/* Restart read request */

			RestartSerial();
		}
		else
		{
			UBYTE LocalBuffer[256];

				/* Shut down the serial device driver */

			DeleteSerial();

				/* Reinitialize the serial device driver */

			if(!CreateSerial(LocalBuffer,sizeof(LocalBuffer)))
			{
					/* Display the error requester */

				ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocalBuffer);

					/* That didn't work */

				Success = RECONFIGURE_FAILURE;
			}
		}
	}

	return(Success);
}

	/* ReopenSerial():
	 *
	 *	Reopen the serial driver.
	 */

VOID
ReopenSerial()
{
	BOOL SerialClosed = TRUE;
	UBYTE LocalBuffer[256];

	do
	{
		if(CreateSerial(LocalBuffer,sizeof(LocalBuffer)))
			SerialClosed = FALSE;
		else
		{
			switch(ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_TERMMAIN_RETRY_IGNORE_QUIT_TXT),LocalBuffer))
			{
				case 0:

					MainTerminated = TRUE;
					break;

				case 2:

					SerialClosed = FALSE;
					break;
			}
		}
	}
	while(SerialClosed && !MainTerminated);
}

	/* HandleSerialJob():
	 *
	 *	Handle the data coming in from the serial line.
	 */

BOOL
HandleSerialJob(JobNode *UnusedJob)
{
	BOOL MoreData;

	if(HostReadBuffer)
	{
		LONG Length = XProtocolHostMon(XprIO,HostReadBuffer,0,SerialBufferSize);

		if(Translate_CR_LF && Length > 0)
			Length = (*Translate_CR_LF)(HostReadBuffer,Length);

		if(Length > 0)
		{
			ConProcess(HostReadBuffer,Length);

			if(Get_xOFF())
				return(FALSE);
		}

		MoreData = TRUE;
	}
	else
		MoreData = FALSE;

		/* Any news? */

	if(CheckSerialRead())
	{
			/* Terminate the read request properly. */

		if(!WaitSerialRead())
		{
			ULONG Length;

				/* We got another byte. */

			ReadBuffer[0] = ReadChar;

				/* Check if there is anything waiting to be read. */

			if(Length = GetSerialWaiting())
			{
				ULONG MaxSize;

					/* This is how many characters fill still fit into
					 * the read buffer.
					 */

				MaxSize = SerialBufferSize - 1;

					/* Check if there is more data waiting
					 * than will fit into the buffer.
					 */

				if(Length > MaxSize)
					Length = MaxSize;

					/* Check if there is more data waiting than
					 * the serial quantum allows.
					 */

				if(Length > Config->SerialConfig->Quantum)
					Length = Config->SerialConfig->Quantum;

					/* Read the data still waiting. */

				if(DoSerialRead(&ReadBuffer[1],Length))
					Length = 0;
			}

				/* One byte was added earlier. */

			Length++;

				/* Process the data. */

			BytesIn += Length;

			(*ConTransfer)(ReadBuffer,Length);
		}

			/* If something is in the cache, don't overwrite
			 * it by requesting another byte. Serial I/O
			 * processing will be restarted when the cache
			 * gets drained.
			 */

		if(!DataHold && ProcessIO)
		{
				/* Yes, there is more to come. */

			MoreData = TRUE;

			StartSerialRead(&ReadChar,1);
		}
	}

	return(MoreData);
}

	/* UpdateSerialJob():
	 *
	 *	Either activate or suspend the serial I/O job.
	 */

VOID
UpdateSerialJob()
{
	ULONG Mask;

	if(!Get_xOFF() && ReadPort && ProcessIO)
		Mask = SIG_SERIAL;
	else
		Mask = 0;

	if(Mask)
	{
		if(DataHold)
		{
			Mask = 0;

			ActivateJob(MainJobQueue,SerialCacheJob);
		}
		else
			SuspendJob(MainJobQueue,SerialCacheJob);
	}

	UpdateJob(MainJobQueue,SerialJob,Mask);

		/* Now pick the right simplified access routines
		 * for the job.
		 */

	if(DataHold)
	{
		SerialWaitForData	= SerialCacheWaitForData;
		SerialGetWaiting	= SerialCacheGetWaiting;
		SerialRead			= SerialCacheRead;
	}
	else
	{
		SerialWaitForData	= SerialStdWaitForData;
		SerialGetWaiting	= SerialStdGetWaiting;
		SerialRead			= SerialStdRead;
	}
}

	/* HandleSerialCacheJob(JobNode *UnusedJob):
	 *
	 *	Does what HandleSerialJob() does, but picks the
	 *	data from the cache.
	 */

BOOL
HandleSerialCacheJob(JobNode *UnusedJob)
{
	STRPTR	Data = DataHold;
	LONG	Size = DataSize;

		/* These two are no longer valid. */

	DataHold = NULL;
	DataSize = 0;

		/* If there was anything in the buffer,
		 * process it.
		 */

	if(Size > 0)
		(*ConTransfer)(Data,Size);

	RestartSerial();

	UpdateSerialJob();

	return(TRUE);
}


/**********************************************************************/


	/* SerialStdWaitForData(ULONG OtherBits):
	 *
	 *	Wait for new data to arrive or other events to
	 *	take place.
	 */

ULONG
SerialStdWaitForData(ULONG OtherBits)
{
	return(Wait(SIG_SERIAL | OtherBits));
}

	/* SerialCacheWaitForData(ULONG OtherBits):
	 *
	 *	Checks if other events took place and always flag
	 *	new serial data to be available.
	 */

ULONG
SerialCacheWaitForData(ULONG OtherBits)
{
	return((SetSignal(0,OtherBits) & OtherBits) | SIG_SERIAL);
}

	/* SerialStdGetWaiting():
	 *
	 *	Get the number of bytes still waiting to be read.
	 */

ULONG
SerialStdGetWaiting()
{
	return(GetSerialWaiting());
}

	/* SerialCacheGetWaiting():
	 *
	 *	Return the number of bytes still in the cache and
	 *	the number of bytes waiting in the serial read
	 *	buffer.
	 */

ULONG
SerialCacheGetWaiting()
{
	return(DataSize + SerialStdGetWaiting());
}

	/* SerialStdRead(UBYTE *Buffer,LONG Len):
	 *
	 *	Reads a number of bytes from the serial line and
	 *	puts them into the buffer. This routine assumes a
	 *	read request had been queued earlier. It waits
	 *	for the queued request to terminate, grabs the
	 *	character that was read and fills the remaining
	 *	buffer with the data to follow. Eventually, the
	 *	read request gets queued again.
	 */

LONG
SerialStdRead(UBYTE *Buffer,LONG Len)
{
	LONG Total;

		/* Wait for the read request to terminate. */

	WaitSerialRead();

		/* Put the byte into the buffer. */

	*Buffer++ = ReadChar;
	Len--;

		/* So far, one byte was read. */

	Total = 1;

		/* Any more data requested? */

	if(Len > 0)
	{
			/* Read the rest. */

		if(!DoSerialRead(Buffer,Len))
			Total += Len;
	}

		/* Restart the read request. */

	StartSerialRead(&ReadChar,1);

		/* Return the number of bytes read so far. */

	return(Total);
}

	/* SerialCacheRead(UBYTE *Buffer,LONG Size):
	 *
	 *	Read a number of bytes from the cache and the
	 *	serial buffer. Switch back to normal operation
	 *	if the cache is exhausted.
	 */

LONG
SerialCacheRead(UBYTE *Buffer,LONG Size)
{
	LONG Total;

		/* Nothing has been read yet. */

	Total = 0;

		/* Check if there is anything left in the cache. */

	if(DataSize > 0)
	{
		LONG Count;

			/* If there is more data in the cache than
			 * requested, just read the requested number
			 * of bytes from it.
			 */

		if(DataSize > Size)
			Count = Size;
		else
			Count = DataSize;

			/* Copy the data from the cache into the buffer. */

		CopyMem(DataHold,Buffer,Count);

			/* This is how many bytes went into the buffer so far. */

		Total	+= Count;

			/* Update the buffer data. */

		Buffer	+= Count;
		Size	-= Count;

			/* Update the cache data. */

		DataHold += Count;
		DataSize -= Count;
	}

		/* If no data is left in the cache, switch the read routines
		 * back to the standard versions. This is done inside
		 * RestartSerial().
		 */

	if(DataSize == 0)
		RestartSerial();

		/* If there is still data to be read, read it from the standard
		 * channel, otherwise return the number of bytes already read.
		 */

	if(Size > 0)
		return(Total + SerialStdRead(Buffer,Size));
	else
		return(Total);
}
