/*
**	Console.c
**
**	High-level terminal console routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Temporary console working buffer. */

STATIC UBYTE ConTempBuffer[512];

typedef VOID (* CALLBACK)(STRPTR,LONG);

/* How this stuff works:
 *
 *	This file holds the text output routines. The main entry point is
 *	ConTransfer (actually a function pointer). This routine feeds the
 *	input buffer into XProtocolHostMon(), a callback routine of the
 *	currently active XPR library. Control then passes to ConProcess().
 *	The incoming data is filtered through the flow scanner (FlowFilter),
 *	passed through the capture routines (CaptureData), the script recorder
 *	(RememberOutputText) and the trap scanner (TrapFilter). The data then
 *	goes through the text processing routine ConProcessData (another
 *	function pointer) which processes the embedded control sequences and
 *	eventually feeds the raw text into ConDump (yet another function
 *	pointer). This routine does the final text output.
 */

	/* GfxText(struct RastPort *RPort,STRPTR Buffer,LONG Length):
	 *
	 *	Text output, if necessary switching from gfx font
	 *	to current default font.
	 */

STATIC VOID
GfxText(struct RastPort *RPort,STRPTR Buffer,LONG Length)
{
		/* Character set modes. */

	enum { MODE_STANDARD,MODE_GFX };

	struct TextFont *Font;
	WORD TextMode;
	LONG SameMode;

		/* Determine current text rendering mode. */

	TextMode = GfxTable[Buffer[0]];

	if(TextMode == MODE_GFX)
		Font = GFX;
	else
		Font = TextFont;

	SetFont(RPort,Font);

		/* Reset number of characters in common mode. */

	SameMode = 0;

		/* Scan until all input is processed. */

	do
	{
			/* Scan for characters in the current mode. */

		while(GfxTable[Buffer[SameMode]] == TextMode && SameMode < Length)
			SameMode++;

			/* Output the text found. */

		Text(RPort,Buffer,SameMode);

			/* Decrement number of remaining bytes. */

		Length -= SameMode;

			/* Anything left? */

		if(Length > 0)
		{
				/* Skip to next character. */

			Buffer += SameMode;
			SameMode = 0;

				/* Change text output mode. */

			if(TextMode == MODE_GFX)
			{
				TextMode = MODE_STANDARD;
				Font = TextFont;
			}
			else
			{
				TextMode = MODE_GFX;
				Font = GFX;
			}

			SetFont(RPort,Font);
		}
	}
	while(Length > 0);

		/* Reset font type. */

	if(TextMode == MODE_GFX)
		SetFont(RPort,TextFont);
}

/****************************************************************************/

	/* ConTranslateSetup():
	 *
	 *	Set up for buffer translation.
	 */

STATIC VOID
ConTranslateSetup(struct TranslationHandle *Handle,STRPTR SourceBuffer,LONG SourceLen,STRPTR DestinationBuffer,LONG DestinationLen,struct TranslationEntry **Table)
{
	Handle->LocalBuffer			= NULL;
	Handle->LocalLen			= 0;

	Handle->SourceBuffer		= SourceBuffer;
	Handle->SourceLen			= SourceLen;

	Handle->DestinationBuffer	= DestinationBuffer;
	Handle->DestinationLen		= DestinationLen;

	Handle->Table				= Table;
}

	/* ConTranslateBuffer(struct TranslationHandle *Handle):
	 *
	 *	Translate buffer contents according to
	 *	translation table contents.
	 */

STATIC LONG
ConTranslateBuffer(struct TranslationHandle *Handle)
{
	struct TranslationEntry *Entry;
	LONG BytesWritten;
	STRPTR Data;

	Data = Handle->DestinationBuffer;
	BytesWritten = 0;

		/* Are we to return any translated data? */

	while(Handle->LocalLen && BytesWritten < Handle->DestinationLen)
	{
			/* Decrement number of bytes in buffer. */

		Handle->LocalLen--;

			/* Return next character. */

		*Data++ = *Handle->LocalBuffer++;

			/* Add another byte. */

		BytesWritten++;
	}

		/* Loop until done. */

	while(Handle->SourceLen && BytesWritten < Handle->DestinationLen)
	{
			/* Another byte eaten. */

		Handle->SourceLen--;

			/* Get table entry. */

		if(Entry = Handle->Table[*Handle->SourceBuffer++])
		{
				/* Copy to local data area. */

			Handle->LocalBuffer	= Entry->String;
			Handle->LocalLen	= Entry->Len;

				/* Translate the data. */

			while(Handle->LocalLen && BytesWritten < Handle->DestinationLen)
			{
					/* Decrement number of bytes in buffer. */

				Handle->LocalLen--;

					/* Return next character. */

				*Data++ = *Handle->LocalBuffer++;

					/* Add another byte. */

				BytesWritten++;
			}
		}
	}

	return(BytesWritten);
}

	/* ConOutputTranslate(STRPTR Buffer,LONG Size):
	 *
	 *	Frontend for ConOutput(), including character translation.
	 */

STATIC VOID
ConOutputTranslate(STRPTR Buffer,LONG Size)
{
	struct TranslationHandle Handle;
	UBYTE LocalBuffer[256];

		/* Set up for translation. */

	ConTranslateSetup(&Handle,Buffer,Size,LocalBuffer,sizeof(LocalBuffer),ReceiveTable);

		/* Process and output the data. */

	while(Size = ConTranslateBuffer(&Handle))
		(* ConOutput)(LocalBuffer,Size);
}

/****************************************************************************/

	/* ConOutputPrinter(STRPTR Buffer,LONG Size):
	 *
	 *	Raw text output routine, printer capture flavour.
	 */

STATIC VOID
ConOutputPrinter(STRPTR Buffer,LONG Size)
{
	if(Write(PrinterCapture,Buffer,Size) != Size)
	{
		BlockWindows();

		if(!ShowRequest(NULL,LocaleString(MSG_CONSOLE_ERROR_WRITING_TO_PRINTER_TXT),LocaleString(MSG_CONSOLE_IGNORE_CLOSE_PRINTER_TXT)))
			ClosePrinterCapture(TRUE);

		ReleaseWindows();
	}
}

	/* ConOutputGFX(STRPTR Buffer,LONG Size):
	 *
	 *	Raw text output routine, GFX text flavour.
	 */

STATIC VOID
ConOutputGFX(STRPTR Buffer,LONG Size)
{
	LONG Offset;

		/* Do we still have a character in the
		 * magnificient buffer?
		 */

	while(Size)
	{
			/* Cursor is positioned at
			 * the right hand side of the
			 * display. If auto-wrap is
			 * enabled, perform some
			 * kind of CR/LF, else leave
			 * the cursor where it is and
			 * quit the show.
			 */

		if(CursorX > LastPrintableColumn)
		{
				/* Wrap cursor. */

			if(Config->EmulationConfig->LineWrap)
			{
					/* Move to beginning of next line. */

				CursorX = 0;

				DownLine();

					/* Reposition cursor, don't redraw it. */

				RepositionCursor();
			}
			else
			{
					/* Stop the cursor. */

				CursorX = LastPrintableColumn;

					/* Make it reappear. */

				RepositionCursor();

				return;
			}
		}

		if((Offset = LastPrintableColumn + 1 - CursorX) > Size)
			Offset = Size;

		if(Config->EmulationConfig->InsertMode)
		{
			RasterShiftChar(Offset);

			ScrollLineShiftChar(Offset);

			ShiftChar(Offset);
		}

		RasterPutString(Buffer,Offset);

		ScrollLinePutString(Offset);

		if(FontScalingRequired)
			PrintScaled(Buffer,Offset,CurrentFontScale);
		else
			GfxText(RPort,Buffer,Offset);

		Buffer	+= Offset;

		Size	-= Offset;

		CursorX	+= Offset;
	}

	RepositionCursor();
}

	/* ConOutputNormal(STRPTR Buffer,LONG Size):
	 *
	 *	Raw text output routine, normal text flavour.
	 */

STATIC VOID
ConOutputNormal(STRPTR Buffer,LONG Size)
{
	LONG Offset;

	while(Size)
	{
		if(CursorX > LastPrintableColumn)
		{
			if(Config->EmulationConfig->LineWrap)
			{
				CursorX = 0;

				DownLine();

				RepositionCursor();
			}
			else
			{
				CursorX = LastPrintableColumn;

				RepositionCursor();

				return;
			}
		}

		if((Offset = LastPrintableColumn + 1 - CursorX) > Size)
			Offset = Size;

		if(Config->EmulationConfig->InsertMode)
		{
			RasterShiftChar(Offset);

			ScrollLineShiftChar(Offset);

			ShiftChar(Offset);
		}

		RasterPutString(Buffer,Offset);

		ScrollLinePutString(Offset);

		if(FontScalingRequired)
			PrintScaled(Buffer,Offset,CurrentFontScale);
		else
			Text(RPort,Buffer,Offset);

		Buffer	+= Offset;
		Size	-= Offset;
		CursorX	+= Offset;
	}

	RepositionCursor();
}

/****************************************************************************/

	/* ConProcessDataTransExternal(STRPTR String,LONG Size):
	 *
	 *	Process data, external emulation including translation flavour.
	 */

STATIC VOID
ConProcessDataTransExternal(STRPTR String,LONG Size)
{
	struct TranslationHandle Handle;

		/* Set up for translation. */

	ConTranslateSetup(&Handle,String,Size,ConTempBuffer,sizeof(ConTempBuffer) / 2,ReceiveTable);

	if(StripBuffer)
	{
		LONG Len;

			/* Process and output the data. */

		while(Size = ConTranslateBuffer(&Handle))
		{
			XEmulatorWrite(XEM_IO,ConTempBuffer,Size);

			XEM_HostData.Source			= ConTempBuffer;
			XEM_HostData.Destination	= StripBuffer;

			if(Len = XEmulatorHostMon(XEM_IO,&XEM_HostData,Size))
			{
				if(CaptureData)
					(*CaptureData)(StripBuffer,Len);
			}
		}
	}
	else
	{
			/* Process and output the data. */

		while(Size = ConTranslateBuffer(&Handle))
			XEmulatorWrite(XEM_IO,ConTempBuffer,Size);
	}
}

	/* ConProcessDataExternal(STRPTR String,LONG Size):
	 *
	 *	Process data, external emulation flavour.
	 */

STATIC VOID
ConProcessDataExternal(STRPTR String,LONG Size)
{
	XEmulatorWrite(XEM_IO,String,Size);

		/* Build another string to contain
		 * the pure ASCII contents, i.e.
		 * not including any ESC control
		 * sequences.
		 */

	if(StripBuffer)
	{
		LONG Len;

		XEM_HostData.Source			= String;
		XEM_HostData.Destination	= StripBuffer;

		if(Len = XEmulatorHostMon(XEM_IO,&XEM_HostData,Size))
		{
			if(CaptureData)
				(*CaptureData)(StripBuffer,Len);
		}
	}
}

	/* ConProcessDataGeneric(STRPTR String,LONG Size,Mask):
	 *
	 *	Process data, generic flavour.
	 */

STATIC VOID
ConProcessDataGeneric(STRPTR String,LONG Size,UBYTE Mask)
{
	LONG	Len = 0;
	UBYTE	c;

		/* If still parsing a sequence,
		 * continue with it.
		 */

	if(InSequence)
	{
		while(Size--)
		{
			c = *String++ & Mask;

			if(!(*AbortTable[c])(c))
			{
				InSequence = FALSE;

				break;
			}
		}
	}

		/* Check which font we are in, if other than Topaz
		 * the only invalid char is a Null (0) which will
		 * display as a space if let to continue.
		 */

	if(Config->TerminalConfig->FontMode == FONT_STANDARD)
	{
		while(Size-- > 0)
		{
			if(IsPrintable[c = *String++ & Mask])
			{
					/* This character is associated with a
					 * special function (bell, xon, xoff, etc.).
					 */

				if(SpecialTable[c])
				{
					if(Len)
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}

						/* Does this character start
						 * a control sequence?
						 */

					if(InSequence = (*SpecialTable[c])(c))
					{
						while(Size-- > 0)
						{
							c = *String++ & Mask;

							if(!(*AbortTable[c])(c))
							{
								InSequence = FALSE;

								break;
							}
						}
					}
				}
				else
				{
						/* Put the character into the buffer
						 * and flush it if necessary.
						 */

					ConTempBuffer[Len] = c;

					if(Len++ == sizeof(ConTempBuffer))
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}
				}
			}
		}
	}
	else
	{
		while(Size-- > 0)
		{
			if(c = (*String++ & Mask))
			{
					/* This character is associated with a
					 * special function (bell, xon, xoff, etc.).
					 */

				if(SpecialTable[c])
				{
					if(Len)
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}

					if(InSequence = (*SpecialTable[c])(c))
					{
						while(Size-- > 0)
						{
							c = *String++ & Mask;

							if(!(*AbortTable[c])(c))
							{
								InSequence = FALSE;

								break;
							}
						}
					}
				}
				else
				{
						/* Put the character into the buffer
						 * and flush it if necessary.
						 */

					ConTempBuffer[Len] = c;

					if(Len++ == sizeof(ConTempBuffer))
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}
				}
			}
		}
	}

	if(Len)
		(*ConDump)(ConTempBuffer,Len);
}

	/* ConProcessData7(STRPTR String,LONG Size):
	 *
	 *	Process data, 7 bit flavour.
	 */

STATIC VOID
ConProcessData7(STRPTR String,LONG Size)
{
	ConProcessDataGeneric(String,Size,0x7F);
}

	/* ConProcessData8(STRPTR String,LONG Size):
	 *
	 *	Process data, 8 bit flavour.
	 */

STATIC VOID
ConProcessData8(STRPTR String,LONG Size)
{
	LONG	Len = 0;
	UBYTE	c;

	if(InSequence)
	{
		while(Size--)
		{
			c = *String++;

			if(!(*AbortTable[c])(c))
			{
				InSequence = FALSE;

				break;
			}
		}
	}

	if(Config->TerminalConfig->FontMode == FONT_STANDARD)
	{
		while(Size-- > 0)
		{
			if(IsPrintable[c = *String++])
			{
				if(SpecialTable[c])
				{
					if(Len)
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}

					if(InSequence = (*SpecialTable[c])(c))
					{
						while(Size-- > 0)
						{
							c = *String++;

							if(!(*AbortTable[c])(c))
							{
								InSequence = FALSE;

								break;
							}
						}
					}
				}
				else
				{
					ConTempBuffer[Len] = c;

					if(Len++ == sizeof(ConTempBuffer))
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}
				}
			}
		}
	}
	else
	{
		while(Size-- > 0)
		{
			if(c = *String++)
			{
				if(SpecialTable[c])
				{
					if(Len)
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}

					if(InSequence = (*SpecialTable[c])(c))
					{
						while(Size-- > 0)
						{
							c = *String++;

							if(!(*AbortTable[c])(c))
							{
								InSequence = FALSE;

								break;
							}
						}
					}
				}
				else
				{
					ConTempBuffer[Len] = c;

					if(Len++ == sizeof(ConTempBuffer))
					{
						(*ConDump)(ConTempBuffer,Len);

						Len = 0;
					}
				}
			}
		}
	}

	if(Len)
		(*ConDump)(ConTempBuffer,Len);
}

	/* ConProcessDataHexGeneric(STRPTR String,LONG Size,UBYTE Mask):
	 *
	 *	Process data, hex mode flavour, generic version.
	 */

STATIC VOID
ConProcessDataHexGeneric(STRPTR String,LONG Size,UBYTE Mask)
{
	UBYTE	DummyBuffer[40],c;
	LONG	Fit,Spaces,Current;

		/* How many characters will fit into the line? */

	Fit	= (LastColumn + 1) / 4;

		/* Which position are we currently in? */

	Current	= CursorX / 3;

		/* Weird cursor position? */

	if(Current >= Fit || (CursorX % 3))
	{
		ConProcessDataGeneric("\r\n",2,Mask);

		Current = 0;
	}

		/* Check the font type. */

	if(Config->TerminalConfig->FontMode == FONT_STANDARD)
	{
		while(Size-- > 0)
		{
			c = (*String++) & Mask;

			Spaces = (Fit - Current) * 3 + Current - 3;

			if(c > ' ' && c < 127)
				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%02lx \33[%ldC%lc\33[%ldD",c,Spaces,c,Spaces + 1);
			else
				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%02lx \33[%ldC.\33[%ldD",c,Spaces,Spaces + 1);

			ConProcessDataGeneric(DummyBuffer,strlen(DummyBuffer),Mask);

			if(Current++ == Fit - 1)
			{
				Current = 0;

				ConProcessDataGeneric("\r\n",2,Mask);
			}
		}
	}
	else
	{
		while(Size-- > 0)
		{
			c = (*String++) & Mask;

			Spaces = (Fit - Current) * 3 + Current - 3;

			if(c && c != ' ')
				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%02lx \33[%ldC%lc\33[%ldD",c,Spaces,c,Spaces + 1);
			else
				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%02lx \33[%ldC.\33[%ldD",c,Spaces,Spaces + 1);

			ConProcessDataGeneric(DummyBuffer,strlen(DummyBuffer),Mask);

			if(Current++ == Fit - 1)
			{
				Current = 0;

				ConProcessDataGeneric("\r\n",2,Mask);
			}
		}
	}
}

	/* ConProcessDataHex7(STRPTR String,LONG Size):
	 *
	 *	Process data, hex mode flavour, seven bits.
	 */

STATIC VOID
ConProcessDataHex7(STRPTR String,LONG Size)
{
	ConProcessDataHexGeneric(String,Size,0x7F);
}

	/* ConProcessDataHex8(STRPTR String,LONG Size):
	 *
	 *	Process data, hex mode flavour, eight bits.
	 */

STATIC VOID
ConProcessDataHex8(STRPTR String,LONG Size)
{
	ConProcessDataHexGeneric(String,Size,0xFF);
}

/****************************************************************************/

	/* ConTransferHost(STRPTR Buffer,LONG Len):
	 *
	 *	Process data read from the serial line,
	 *	special XPR flavour.
	 */

STATIC VOID
ConTransferHost(STRPTR Buffer,LONG Len)
{
	LONG MaxSize;

		/* How much room is left? */

	if(Buffer != ReadBuffer)
		MaxSize = SerialBufferSize - ((IPTR)Buffer - (IPTR)ReadBuffer);
	else
		MaxSize = SerialBufferSize;

		/* Let the XPR library take its share. */

	Len = XProtocolHostMon(XprIO,Buffer,Len,MaxSize);

		/* Did a file transfer start? */

	if(TransferWindow)
		TransferCleanup();

		/* If the file transfer was aborted, flush the receive buffer. */

	if(TransferAborted)
	{
		BlockWindows();

		CompletelyFlushSerialRead();

		ReleaseWindows();

		TransferAborted = FALSE;
	}

		/* Reset these two. */

	TransferFailed = TransferError = FALSE;

	if(Len)
		ConProcess(Buffer,Len);
}

	/* ConRawOutput(STRPTR String):
	 *
	 *	Bypass the `normal' ConOutput() data processing.
	 */

STATIC VOID
ConRawOutput(STRPTR String)
{
	CALLBACK ProcessData;

	if(Config->SerialConfig->StripBit8)
		ProcessData = (CALLBACK)ConProcessData7;
	else
		ProcessData = (CALLBACK)ConProcessData8;

	ObtainSemaphore(&TerminalSemaphore);

	if(Marking)
		WindowMarkerStop();

	ClearCursor();

	(*ProcessData)(String,strlen(String));

	DrawCursor();

	ReleaseSemaphore(&TerminalSemaphore);
}

STATIC VOID
ConTransferHost_Translate_CR_LF(STRPTR String,LONG Size)
{
	LONG MaxSize;

	if(String != ReadBuffer)
		MaxSize = SerialBufferSize - ((IPTR)String - (IPTR)ReadBuffer);
	else
		MaxSize = SerialBufferSize;

	Size = XProtocolHostMon(XprIO,String,Size,MaxSize);

	if(TransferWindow)
		TransferCleanup();

	if(Size > 0)
	{
		if(Size = (*Translate_CR_LF)(String,Size))
			ConProcess(String,Size);
	}
}

STATIC VOID
ConProcess_Translate_CR_LF(STRPTR String,LONG Size)
{
	if(Size = (*Translate_CR_LF)(String,Size))
		ConProcess(String,Size);
}

	/* ConProcess(STRPTR String,LONG Size):
	 *
	 *	This is the central entry point to the terminal window
	 *	output routines. The incoming data is filtered through
	 *	the flow scanner (FlowFilter), passed through the capture
	 *	routines (CaptureData), the script recorder (RememberOutputText)
	 *	and the trap scanner (TrapFilter).
	 */

VOID
ConProcess(STRPTR String,LONG Size)
{
	if(Size > 0)
	{
			/* Feed the flow filter. */

		if(UseFlow)
			Size = FlowFilter(String,Size,Config->SerialConfig->StripBit8 ? 0x7F : 0xFF);

			/* In quiet mode no characters are echoed to the
			 * console window, they are just passed through
			 * the data flow filter. Usually, this mode is
			 * enabled by the dial panel.
			 */

		if(!ConsoleQuiet && Size > 0)
		{
				/* Capture the data. */

			if(CaptureData && !XEmulatorBase)
			{
				if(Config->CaptureConfig->ConvertChars && Config->TerminalConfig->FontMode != FONT_STANDARD)
				{
					UBYTE	 LocalBuffer[BUFFER_LINE_MAX];
					UBYTE	*Src = String,*Dst,c;
					ULONG	 Count,Len = Size;

					while(Len > 0)
					{
						Count = MIN(BUFFER_LINE_MAX,Len);
						Len -= Count;
						Dst = LocalBuffer;

						do
						{
							if(c = ISOConversion[*Src++])
								*Dst++ = c;
							else
								*Dst++ = (UBYTE)'·';
						}
						while(--Count > 0);

						if((Count = (IPTR)Dst - (IPTR)LocalBuffer) > 0)
							(*CaptureData)(LocalBuffer,Count);
					}
				}
				else
					(*CaptureData)(String,Size);
			}

				/* Remember the data. */

			if(RememberOutput)
				RememberOutputText(String,Size);

				/* Check the traps if necessary. */

			if(WatchTraps)
				TrapFilter(String,Size);

			if(Marking)
				WindowMarkerStop();

			if(TerminalQueue)
			{
				struct DataMsg *Msg;

				if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg) + Size))
				{
					Msg->Data = (APTR)(Msg + 1);
					Msg->Size = Size;

					CopyMem(String,Msg->Data,Msg->Size);

					PutMsgItem(TerminalQueue,(struct MsgItem *)Msg);

					return;
				}
			}

			ClearCursor();

			(*ConProcessData)(String,Size);

			DrawCursor();
		}
	}
}

	/* ConPrintf(STRPTR String,...):
	 *
	 *	Output a string to the console.
	 */

VOID
ConPrintf(CONST_STRPTR String,...)
{
	UBYTE LocalBuffer[256];
	va_list	VarArgs;

	if(Marking)
		WindowMarkerStop();

	va_start(VarArgs,String);
	LimitedVSPrintf(sizeof(LocalBuffer),LocalBuffer,String,VarArgs);
	va_end(VarArgs);

	ConProcess(LocalBuffer,strlen(LocalBuffer));
}

/****************************************************************************/

	/* ConProcessUpdate():
	 *
	 *	Choose the right console data processing routine.
	 */

VOID
ConProcessUpdate()
{
	Forbid();

	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
	{
		if(ReceiveTable)
			ConProcessData = ConProcessDataTransExternal;
		else
			ConProcessData = ConProcessDataExternal;
	}
	else
	{
		if(Config->SerialConfig->StripBit8)
		{
			if(Config->TerminalConfig->EmulationMode == EMULATION_HEX)
				ConProcessData = ConProcessDataHex7;
			else
				ConProcessData = ConProcessData7;
		}
		else
		{
			if(Config->TerminalConfig->EmulationMode == EMULATION_HEX)
				ConProcessData = ConProcessDataHex8;
			else
				ConProcessData = ConProcessData8;
		}
	}

	Permit();
}

	/* ConOutputUpdate():
	 *
	 *	Choose the right raw text output routine for the job.
	 */

VOID
ConOutputUpdate()
{
	Forbid();

	if(Config->CaptureConfig->BufferMode == BUFFERMODE_FLOW)
		SaveRaster = SaveRasterDummy;
	else
		SaveRaster = SaveRasterReal;

	if(BufferFrozen || Config->CaptureConfig->BufferMode != BUFFERMODE_FLOW)
	{
		if(FileCapture)
		{
			if(PrinterCapture)
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_File_Printer;
				else
					CaptureData = CaptureRawTo_File_Printer;
			}
			else
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_File;
				else
					CaptureData = CaptureRawTo_File;
			}
		}
		else
		{
			if(PrinterCapture)
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_Printer;
				else
					CaptureData = CaptureRawTo_Printer;
			}
			else
				CaptureData = NULL;
		}
	}
	else
	{
		if(FileCapture)
		{
			if(PrinterCapture)
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_Buffer_File_Printer;
				else
					CaptureData = CaptureRawTo_Buffer_File_Printer;
			}
			else
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_Buffer_File;
				else
					CaptureData = CaptureRawTo_Buffer_File;
			}
		}
		else
		{
			if(PrinterCapture)
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_Buffer_Printer;
				else
					CaptureData = CaptureRawTo_Buffer_Printer;
			}
			else
			{
				if(Config->CaptureConfig->CaptureFilterMode && !RawCapture)
					CaptureData = CaptureFilteredTo_Buffer;
				else
					CaptureData = CaptureRawTo_Buffer;
			}
		}
	}

	if(ControllerActive)
		ConOutput = ConOutputPrinter;
	else
	{
		if(CurrentFont == GFX)
			ConOutput = ConOutputGFX;
		else
			ConOutput = ConOutputNormal;
	}

	if(ReceiveTable)
		ConDump = ConOutputTranslate;
	else
		ConDump = ConOutput;

	Permit();
}

	/* ConFontScaleUpdate():
	 *
	 *	Choose the right font scale for the job.
	 */

VOID
ConFontScaleUpdate()
{
	CurrentFontScale	= RasterAttr[CursorY];
	FontScalingRequired	= FALSE;
	CharCellNominator	= 1;

	if(CurrentCharWidth == SCALE_HALF)
		CharCellDenominator = 2;
	else
		CharCellDenominator = 1;

	if(CurrentFontScale == SCALE_ATTR_NORMAL)
	{
		if(CurrentCharWidth == SCALE_HALF)
		{
				/* Half of normal width */

			LastPrintableColumn	= (LastColumn + 1) * 2 - 1;
			LastPrintablePixel 	= (LastPixel + 1) * 2 - 1;
			FontScalingRequired	= TRUE;
		}
		else
		{
				/* Normal width */

			LastPrintableColumn	= LastColumn;
			LastPrintablePixel	= LastPixel;
		}
	}
	else
	{
		if(CurrentCharWidth == SCALE_HALF)
		{
				/* Half of double width */

			LastPrintableColumn	= LastColumn;
			LastPrintablePixel	= LastPixel;
		}
		else
		{
				/* Double width */

			LastPrintableColumn	= ((LastColumn + 1) / 2) - 1;
			LastPrintablePixel	= ((LastPixel + 1) / 2) - 1;
			FontScalingRequired	= TRUE;
		}
	}
}

	/* ConTransferUpdate():
	 *
	 *	Choose the right processing routine for the job.
	 */

VOID
ConTransferUpdate()
{
	if(Translate_CR_LF)
	{
		if(XProtocolBase && (TransferBits & XPRS_HOSTMON))
			ConTransfer = (CONTRANSFER)ConTransferHost_Translate_CR_LF;
		else
			ConTransfer = (CONTRANSFER)ConProcess_Translate_CR_LF;
	}
	else
	{
		if(XProtocolBase && (TransferBits & XPRS_HOSTMON))
			ConTransfer = (CONTRANSFER)ConTransferHost;
		else
			ConTransfer = (CONTRANSFER)ConProcess;
	}
}

/****************************************************************************/

	/* ConClear():
	 *
	 *	Clears the console screen, no matter what emulation is
	 *	currently running.
	 */

VOID
ConClear()
{
	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		XEmulatorClearConsole(XEM_IO);
	else
	{
		if(Marking)
			WindowMarkerStop();

		ConRawOutput("\033[2J\033[H");
	}
}

	/* ConResetFont():
	 *
	 *	Resets the console font, no matter what emulation is
	 *	currently running.
	 */

VOID
ConResetFont()
{
	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		XEmulatorResetCharset(XEM_IO);
	else
	{
		if(Marking)
			WindowMarkerStop();

		CurrentFont = TextFont;

		SetFont(RPort,CurrentFont);

		ConOutputUpdate();
	}
}

	/* ConResetStyles():
	 *
	 *	Resets the console text rendering style, no matter what emulation is
	 *	currently running.
	 */

VOID
ConResetStyles()
{
	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		XEmulatorResetTextStyles(XEM_IO);
	else
	{
		if(Marking)
			WindowMarkerStop();

		ConRawOutput("\033[0m");

		ObtainSemaphore(&TerminalSemaphore);

		ClearCursor();

		CurrentCharWidth = SCALE_NORMAL;

		if(!Config->EmulationConfig->LockColour)
		{
			ForegroundPen = GetPenIndex(SafeTextPen);
			BackgroundPen = 0;
		}

		UpdatePens();

		ConFontScaleUpdate();

		DrawCursor();

		ReleaseSemaphore(&TerminalSemaphore);
	}
}

	/* ConResetTerminal():
	 *
	 *	Resets the console, no matter what emulation is
	 *	currently running.
	 */

VOID
ConResetTerminal()
{
	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		XEmulatorResetConsole(XEM_IO);
	else
	{
		if(Marking)
			WindowMarkerStop();

		ConRawOutput("\033c");
	}
}

	/* SetConsoleQuiet(BOOL NewSettings):
	 *
	 *	Tell the console to be quiet.
	 */

BOOL
SetConsoleQuiet(BOOL NewSettings)
{
	BOOL OldSettings;

	OldSettings		= ConsoleQuiet;
	ConsoleQuiet	= NewSettings;

	return(OldSettings);
}
