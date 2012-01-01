/*
**	SerialIO.c
**
**	Serial read/write routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC BOOL		 ReadQueued		= FALSE,
				 WriteQueued	= FALSE;

STATIC UBYTE	*QueueReadBuffer;
STATIC LONG		 QueueReadSize;

	/* ResetSerialRead():
	 *
	 *	Reset the read status.
	 */

VOID
ResetSerialRead()
{
	ReadQueued		= FALSE;
	QueueReadBuffer	= NULL;
	QueueReadSize	= 0;
}

	/* CheckSerialRead():
	 *
	 *	Check if a read request is finished.
	 */

BOOL
CheckSerialRead()
{
	if(ReadRequest && ReadQueued)
	{
		if(CheckIO((struct IORequest *)ReadRequest))
			return(TRUE);
	}

	return(FALSE);
}

	/* WaitSerialRead():
	 *
	 *	Wait for the read request to terminate.
	 */

BYTE
WaitSerialRead()
{
	if(ReadRequest && ReadQueued)
	{
		ReadQueued		= FALSE;
		QueueReadBuffer	= NULL;
		QueueReadSize	= 0;

		return(WaitIO((struct IORequest *)ReadRequest));
	}
	else
		return(0);
}

	/* FlushSerialRead():
	 *
	 *	Forget about the current contents of the serial
	 *	read buffer.
	 */

BYTE
FlushSerialRead()
{
	if(ReadRequest)
	{
		UBYTE *WasQueueReadBuffer;
		LONG WasQueueReadSize;
		BOOL WasReading;

		WasReading			= ReadQueued;
		WasQueueReadBuffer	= QueueReadBuffer;
		WasQueueReadSize	= QueueReadSize;

		StopSerialRead();

		DoSerialCmd(CMD_CLEAR);

		if(WasReading)
			StartSerialRead(WasQueueReadBuffer,WasQueueReadSize);
	}

	return(0);
}

	/* DoSerialBreak():
	 *
	 *	Send a break signal.
	 */

BYTE
DoSerialBreak()
{
	if(WriteRequest)
	{
		UBYTE *WasQueueReadBuffer;
		LONG WasQueueReadSize;
		BOOL WasReading;

		WasReading			= ReadQueued;
		WasQueueReadBuffer	= QueueReadBuffer;
		WasQueueReadSize	= QueueReadSize;

		StopSerialRead();

		DoSerialCmd(SDCMD_BREAK);

		if(WasReading)
			StartSerialRead(WasQueueReadBuffer,WasQueueReadSize);
	}

	return(0);
}

	/* CompletelyFlushSerialRead():
	 *
	 *	Flush the serial read buffer over and over again until
	 *	no more data comes in.
	 */

VOID
CompletelyFlushSerialRead()
{
	if(ReadRequest && WriteRequest)
	{
		UBYTE *WasQueueReadBuffer;
		LONG WasQueueReadSize;
		BOOL WasReading;

		WasReading			= ReadQueued;
		WasQueueReadBuffer	= QueueReadBuffer;
		WasQueueReadSize	= QueueReadSize;

		StopSerialRead();

		do
		{
			DoSerialCmd(CMD_CLEAR);

			DelayTime(1,0);
		}
		while(GetSerialWaiting());

		if(WasReading)
			StartSerialRead(WasQueueReadBuffer,WasQueueReadSize);
	}
}

	/* StopSerialRead():
	 *
	 *	Force a read request to terminate.
	 */

VOID
StopSerialRead()
{
	if(ReadRequest && ReadQueued)
	{
		if(!CheckIO((struct IORequest *)ReadRequest))
			AbortIO((struct IORequest *)ReadRequest);

		WaitIO((struct IORequest *)ReadRequest);

		ReadQueued		= FALSE;
		QueueReadBuffer	= NULL;
		QueueReadSize	= 0;
	}
}

	/* StartSerialRead(ULONG Length,APTR Data):
	 *
	 *	Start a serial read request asynchronously.
	 */

VOID
StartSerialRead(APTR Data,ULONG Length)
{
	if(ReadRequest)
	{
		if(ReadQueued)
			StopSerialRead();

		ReadRequest->IOSer.io_Command	= CMD_READ;
		ReadRequest->IOSer.io_Length	= Length;
		ReadRequest->IOSer.io_Data		= Data;

		ClrSignal(1L << ReadPort->mp_SigBit);

		SendIO((struct IORequest *)ReadRequest);

		ReadQueued		= TRUE;
		QueueReadBuffer	= Data;
		QueueReadSize	= Length;
	}
}

	/* DoSerialRead(ULONG Length,APTR Data):
	 *
	 *	Perform a read request synchronously.
	 */

BYTE
DoSerialRead(APTR Data,ULONG Length)
{
	if(ReadRequest)
	{
		if(ReadQueued)
			StopSerialRead();

		ReadRequest->IOSer.io_Command	= CMD_READ;
		ReadRequest->IOSer.io_Length	= Length;
		ReadRequest->IOSer.io_Data		= Data;

		return(DoIO((struct IORequest *)ReadRequest));
	}
	else
		return(IOERR_SELFTEST);
}

	/* ResetSerialWrite():
	 *
	 *	Reset the write status.
	 */

VOID
ResetSerialWrite()
{
	WriteQueued = FALSE;
}

	/* WaitSerialWrite():
	 *
	 *	Wait for the write request to terminate.
	 */

BYTE
WaitSerialWrite()
{
	if(WriteRequest && WriteQueued)
	{
		WriteQueued = FALSE;

		return(WaitIO((struct IORequest *)WriteRequest));
	}
	else
		return(0);
}

	/* StopSerialWrite():
	 *
	 *	Force a write request to terminate.
	 */

VOID
StopSerialWrite()
{
	if(WriteRequest && WriteQueued)
	{
		if(!CheckIO((struct IORequest *)WriteRequest))
			AbortIO((struct IORequest *)WriteRequest);

		WaitIO((struct IORequest *)WriteRequest);

		WriteQueued = FALSE;
	}
}

	/* StartSerialWrite(ULONG Length,APTR Data):
	 *
	 *	Start a serial write request asynchronously.
	 */

VOID
StartSerialWrite(APTR Data,ULONG Length)
{
	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command	= CMD_WRITE;
		WriteRequest->IOSer.io_Length	= Length;
		WriteRequest->IOSer.io_Data		= Data;

		ClrSignal(1L << WriteRequest->IOSer.io_Message.mn_ReplyPort->mp_SigBit);

		SendIO((struct IORequest *)WriteRequest);

		WriteQueued = TRUE;
	}
}

	/* DoSerialWrite(ULONG Length,APTR Data):
	 *
	 *	Perform a write request synchronously.
	 */

BYTE
DoSerialWrite(APTR Data,ULONG Length)
{
	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command	= CMD_WRITE;
		WriteRequest->IOSer.io_Length	= Length;
		WriteRequest->IOSer.io_Data		= Data;

		return(DoIO((struct IORequest *)WriteRequest));
	}
	else
		return(IOERR_SELFTEST);
}

	/* DoSerialCmd(UWORD Command):
	 *
	 *	Perform single command.
	 */

BYTE
DoSerialCmd(LONG Command)
{
	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command = Command;

		return(DoIO((struct IORequest *)WriteRequest));
	}
	else
		return(IOERR_SELFTEST);
}

	/* GetSerialWaiting():
	 *
	 *	Query the number of bytes still waiting to be read
	 *	from the serial port.
	 */

ULONG
GetSerialWaiting()
{
	ULONG Waiting;

	Waiting = 0;

	if(ReadQueued)
	{
		if(!CheckSerialRead())
			return(0);
		else
			Waiting += ReadRequest->IOSer.io_Actual;
	}

	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command = SDCMD_QUERY;

		DoIO((struct IORequest *)WriteRequest);

		Waiting += WriteRequest->IOSer.io_Actual;
	}

	return(Waiting);
}

	/* GetSerialStatus():
	 *
	 *	Query the current serial port status.
	 */

LONG
GetSerialStatus()
{
	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command = SDCMD_QUERY;

		DoIO((struct IORequest *)WriteRequest);

		return(WriteRequest->io_Status);
	}
	else
#ifdef __AROS__
		return(SER_STATF_COMCD | SER_STATF_COMDSR);
#else
		return(CIAF_COMCD | CIAF_COMDSR);
#endif
}

	/* GetSerialInfo(ULONG *Waiting,UWORD *Status):
	 *
	 *	Query both the number of bytes waiting to be read and
	 *	the current serial status.
	 */

VOID
GetSerialInfo(ULONG *Waiting,UWORD *Status)
{
	if(WriteRequest)
	{
		if(WriteQueued)
			StopSerialWrite();

		WriteRequest->IOSer.io_Command = SDCMD_QUERY;

		DoIO((struct IORequest *)WriteRequest);

		*Waiting	= WriteRequest->IOSer.io_Actual;
		*Status		= WriteRequest->io_Status;
	}
	else
	{
		*Waiting	= 0;
#ifdef __AROS__
		*Status		= SER_STATF_COMCD | SER_STATF_COMDSR;
#else
		*Status		= CIAF_COMCD | CIAF_COMDSR;
#endif
	}
}

	/* SetSerialAttributes():
	 *
	 *	Set the serial driver attributes.
	 */

STATIC VOID
SetSerialAttributes(struct IOExtSer *Request,const struct TagItem *Tags)
{
	struct TagItem *Tag;

	while(Tag = NextTagItem((struct TagItem **)&Tags))
	{
		switch(Tag->ti_Tag)
		{
			case SERA_Baud:

				Request->io_Baud = Tag->ti_Data;
				break;

			case SERA_BreakTime:

				Request->io_BrkTime = Tag->ti_Data;
				break;

			case SERA_BitsPerChar:

				Request->io_ReadLen	= Tag->ti_Data;
				Request->io_WriteLen	= Tag->ti_Data;
				break;

			case SERA_StopBits:

				Request->io_StopBits = Tag->ti_Data;
				break;

			case SERA_BufferSize:

				Request->io_RBufLen = Tag->ti_Data;
				break;

			case SERA_Parity:

				Request->io_ExtFlags &= ~(SEXTF_MSPON | SEXTF_MARK);
				Request->io_SerFlags &= ~(SERF_PARTY_ON | SERF_PARTY_ODD);

				switch(Tag->ti_Data)
				{
					case PARITY_EVEN:

						Request->io_SerFlags |= SERF_PARTY_ON;
						break;

					case PARITY_ODD:

						Request->io_SerFlags |= SERF_PARTY_ON | SERF_PARTY_ODD;
						break;

					case PARITY_MARK:

						Request->io_SerFlags |= SERF_PARTY_ON;
						Request->io_ExtFlags |= SEXTF_MSPON | SEXTF_MARK;
						break;

					case PARITY_SPACE:

						Request->io_SerFlags |= SERF_PARTY_ON;
						Request->io_ExtFlags |= SEXTF_MSPON;
						break;
				}

				break;

			case SERA_Handshaking:

				if(Tag->ti_Data == HANDSHAKING_NONE)
					Request->io_SerFlags &= ~SERF_7WIRE;
				else
					Request->io_SerFlags |=  SERF_7WIRE;

				break;

			case SERA_HighSpeed:

				if(Tag->ti_Data)
					Request->io_SerFlags |=  SERF_RAD_BOOGIE;
				else
					Request->io_SerFlags &= ~SERF_RAD_BOOGIE;

				break;

			case SERA_Shared:

				if(Tag->ti_Data)
					Request->io_SerFlags |=  SERF_SHARED;
				else
					Request->io_SerFlags &= ~SERF_SHARED;

				break;
		}
	}

	Request->io_SerFlags |= SERF_XDISABLED;
}

	/* GetSerialAttributes():
	 *
	 *	Get the serial driver attributes.
	 */

STATIC ULONG
GetSerialAttributes(struct IOExtSer *Request,const struct TagItem *Tags)
{
	struct TagItem	*Tag;
	ULONG			*Data;
	ULONG			 Result;

	Result = 0;

	while(Tag = NextTagItem((struct TagItem **)&Tags))
	{
		if(!(Data = (ULONG *)Tag->ti_Data))
			Data = &Result;

		switch(Tag->ti_Tag)
		{
			case SERA_Baud:

				*Data = Request->io_Baud;
				break;

			case SERA_BreakTime:

				*Data = Request->io_BrkTime;
				break;

			case SERA_BitsPerChar:

				*Data = Request->io_ReadLen;
				break;

			case SERA_StopBits:

				*Data = Request->io_StopBits;
				break;

			case SERA_BufferSize:

				*Data = Request->io_RBufLen;
				break;

			case SERA_Parity:

				Request->io_ExtFlags &= ~(SEXTF_MSPON | SEXTF_MARK);
				Request->io_SerFlags &= ~(SERF_PARTY_ON | SERF_PARTY_ODD);

				switch(Tag->ti_Data)
				{
					case PARITY_EVEN:

						Request->io_SerFlags |= SERF_PARTY_ON;
						break;

					case PARITY_ODD:

						Request->io_SerFlags |= SERF_PARTY_ON | SERF_PARTY_ODD;
						break;

					case PARITY_MARK:

						Request->io_SerFlags |= SERF_PARTY_ON;
						Request->io_ExtFlags |= SEXTF_MSPON | SEXTF_MARK;
						break;

					case PARITY_SPACE:

						Request->io_SerFlags |= SERF_PARTY_ON;
						Request->io_ExtFlags |= SEXTF_MSPON;
						break;
				}

				switch(Request->io_ExtFlags & (SEXTF_MSPON | SEXTF_MARK))
				{
					case SEXTF_MSPON | SEXTF_MARK:

						*Data = PARITY_MARK;
						break;

					case SEXTF_MSPON:

						*Data = PARITY_SPACE;
						break;

					default:

						switch(Request->io_SerFlags & (SERF_PARTY_ON | SERF_PARTY_ODD))
						{
							case SERF_PARTY_ON | SERF_PARTY_ODD:

								*Data = PARITY_ODD;
								break;

							case SERF_PARTY_ON:

								*Data = PARITY_EVEN;
								break;

							default:

								*Data = PARITY_NONE;
								break;
						}

						break;
				}

				break;

			case SERA_Handshaking:

				if(Request->io_SerFlags & SERF_7WIRE)
					*Data = TRUE;
				else
					*Data = FALSE;

				break;

			case SERA_HighSpeed:

				if(Request->io_SerFlags & SERF_RAD_BOOGIE)
					*Data = TRUE;
				else
					*Data = FALSE;

				break;

			case SERA_Shared:

				if(Request->io_SerFlags & SERF_SHARED)
					*Data = TRUE;
				else
					*Data = FALSE;

				break;
		}
	}

	return(Result);
}

	/* SetBothSerialAttributes():
	 *
	 *	Set the serial read driver parameters.
	 */

BYTE
SetBothSerialAttributes(Tag FirstTag,...)
{
	if(ReadRequest && WriteRequest)
	{
		BYTE Result;

		if(ReadQueued)
			StopSerialRead();

		if(WriteQueued)
			StopSerialWrite();

		SetSerialAttributes(ReadRequest,(struct TagItem *)&FirstTag);

		if(ReadRequest->IOSer.io_Device)
		{
			ReadRequest->IOSer.io_Command = SDCMD_SETPARAMS;

			Result = DoIO((struct IORequest *)ReadRequest);
		}
		else
			Result = 0;

		if(!Result)
		{
			struct MsgPort *OldWritePort = WriteRequest->IOSer.io_Message.mn_ReplyPort;

			CopyMem(ReadRequest,WriteRequest,sizeof(struct IOExtSer));

			WriteRequest->IOSer.io_Message.mn_ReplyPort = OldWritePort;
		}

		return(Result);
	}
	else
		return(IOERR_SELFTEST);
}

	/* SetSerialReadAttributes():
	 *
	 *	Set the serial read driver parameters.
	 */

BYTE
SetSerialReadAttributes(Tag FirstTag,...)
{
#ifdef __AROS__
        AROS_SLOWSTACKTAGS_PRE_AS(FirstTag,BYTE)
#else
        BYTE retval;
#endif
	if(ReadRequest)
	{
		if(ReadQueued)
			StopSerialRead();

		SetSerialAttributes(ReadRequest,AROS_SLOWSTACKTAGS_ARG(FirstTag));

		if(ReadRequest->IOSer.io_Device)
		{
			ReadRequest->IOSer.io_Command = SDCMD_SETPARAMS;

			retval = DoIO((struct IORequest *)ReadRequest);
		}
		else
			retval = 0;
	}
	else
		retval = IOERR_SELFTEST;
#ifdef __AROS__
        AROS_SLOWSTACKTAGS_POST
#endif
}

	/* GetSerialWriteAttributes():
	 *
	 *	Get the serial write driver parameters.
	 */

ULONG
GetSerialWriteAttributes(Tag FirstTag,...)
{
	if(WriteRequest)
		return(GetSerialAttributes(WriteRequest,(struct TagItem *)&FirstTag));
	else
		return(0);
}

	/* DropDTR(STRPTR Device,LONG Unit):
	 *
	 *	Drops the DTR signal on the currently open device.
	 */

BOOL
DropDTR(STRPTR Device,LONG Unit)
{
	UBYTE *WasQueueReadBuffer;
	LONG WasQueueReadSize;
	BOOL WasReading;

	WasReading			= ReadQueued;
	WasQueueReadBuffer	= QueueReadBuffer;
	WasQueueReadSize	= QueueReadSize;

	StopSerialRead();

		/* Do we have a valid environment? */

	if(WriteRequest)
	{
			/* We'll start with the special line signal control commands */
			/* as supported by the ASDG dual serial board. */

		WriteRequest->IOSer.io_Command	= SIOCMD_SETCTRLLINES;
		WriteRequest->IOSer.io_Offset	= SIOB_DTRF;
		WriteRequest->IOSer.io_Length	= 0;

		if(!DoIO((struct IORequest *)WriteRequest))
		{
				/* Ok, so that did work. Now we wait a little... */

			DelayTime(1,0);

				/* And we raise the DTR signal again */

			WriteRequest->IOSer.io_Command	= SIOCMD_SETCTRLLINES;
			WriteRequest->IOSer.io_Offset	= SIOB_DTRF;
			WriteRequest->IOSer.io_Length	= SIOB_DTRF;

			DoIO((struct IORequest *)WriteRequest);
		}
		else
		{
				/* So that didn't work; we'll have to open and close */
				/* the device in order to make the driver drop the */
				/* DTR signal. */

			CloseDevice((struct IORequest *)ReadRequest);

				/* Wait a little... */

			DelayTime(1,0);

				/* And reopen the driver again; note that since we closed */
				/* it nothing should have modified the default settings */

			if(OpenSerialDevice(Device,Unit))
			{
					/* Who knows what happened, the device didn't open */
					/* and we will have to clean up now */

				ReadRequest->IOSer.io_Device = WriteRequest->IOSer.io_Device = NULL;

				return(FALSE);
			}
		}
	}

	if(WasReading)
		StartSerialRead(WasQueueReadBuffer,WasQueueReadSize);

	return(TRUE);
}

	/* OpenSerialDevice(STRPTR Device,LONG Unit):
	 *
	 *	Open the serial device driver and update the write
	 *	request as well.
	 */

BYTE
OpenSerialDevice(STRPTR Device,LONG Unit)
{
	BYTE Result;

	Result = OpenDevice(Device,Unit,(struct IORequest *)ReadRequest,0);

	if(Result == 0)
	{
			/* Was the write request initialized? */

		if(WriteRequest->IOSer.io_Device)
		{
			CopyMem(&WriteRequest->io_CtlChar,&ReadRequest->io_CtlChar,ReadRequest->IOSer.io_Message.mn_Length - offsetof(struct IOExtSer,io_CtlChar));

			ReadRequest->IOSer.io_Command = SDCMD_SETPARAMS;
			DoIO((struct IORequest *)ReadRequest);
		}

			/* Update the write request */

		CopyMem(ReadRequest,WriteRequest,sizeof(struct IOExtSer));

		WriteRequest->IOSer.io_Message.mn_ReplyPort = WritePort;
	}

	return(Result);
}
