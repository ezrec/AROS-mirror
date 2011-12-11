/*
**	Capture.c
**
**	File and printer capture support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC VOID
CaptureCallback1(APTR Buffer,LONG Size)
{
	AddLine(Buffer,Size);

	CaptureToFile(Buffer,Size);
	CaptureToFile("\n",1);
}

STATIC VOID
CaptureCallback3(APTR Buffer,LONG Size)
{
	AddLine(Buffer,Size);

	CaptureToFile(Buffer,Size);
	CaptureToFile("\n",1);

	CaptureToPrinter(Buffer,Size);
	CaptureToPrinter("\n",1);
}

STATIC VOID
CaptureCallback4(APTR Buffer,LONG Size)
{
	CaptureToFile(Buffer,Size);
	CaptureToFile("\n",1);
}

STATIC VOID
CaptureCallback5(APTR Buffer,LONG Size)
{
	CaptureToFile(Buffer,Size);
	CaptureToFile("\n",1);

	CaptureToPrinter(Buffer,Size);
	CaptureToPrinter("\n",1);
}

STATIC VOID
CaptureCallback6(APTR Buffer,LONG Size)
{
	CaptureToPrinter(Buffer,Size);
	CaptureToPrinter("\n",1);
}

STATIC VOID
CaptureCallback7(APTR Buffer,LONG Size)
{
	AddLine(Buffer,Size);

	CaptureToPrinter(Buffer,Size);
	CaptureToPrinter("\n",1);
}

VOID
CaptureFilteredTo_Buffer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)AddLine);
}

VOID
CaptureFilteredTo_Buffer_File(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback1);
}

VOID
CaptureFilteredTo_Buffer_File_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback3);
}

VOID
CaptureFilteredTo_File(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback4);
}

VOID
CaptureFilteredTo_File_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback5);
}

VOID
CaptureFilteredTo_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback6);
}

VOID
CaptureFilteredTo_Buffer_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)CaptureCallback7);
}

VOID
CaptureRawTo_Buffer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)AddLine);
}

VOID
CaptureRawTo_Buffer_File(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)AddLine);

	CaptureToFile(Buffer,Size);
}

VOID
CaptureRawTo_Buffer_File_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)AddLine);

	CaptureToFile(Buffer,Size);
	CaptureToPrinter(Buffer,Size);
}

VOID
CaptureRawTo_Buffer_Printer(APTR Buffer,LONG Size)
{
	CaptureParser(ParserStuff,Buffer,Size,(COPTR)AddLine);

	CaptureToPrinter(Buffer,Size);
}

VOID
CaptureRawTo_File(APTR Buffer,LONG Size)
{
	CaptureToFile(Buffer,Size);
}

VOID
CaptureRawTo_File_Printer(APTR Buffer,LONG Size)
{
	CaptureToFile(Buffer,Size);
	CaptureToPrinter(Buffer,Size);
}

VOID
CaptureRawTo_Printer(APTR Buffer,LONG Size)
{
	CaptureToPrinter(Buffer,Size);
}

	/* ClosePrinterCapture(BOOL Force):
	 *
	 *	Closes printer capture file.
	 */

VOID
ClosePrinterCapture(BOOL Force)
{
	if(PrinterCapture)
	{
		if(ControllerActive && StandardPrinterCapture && !Force)
			FPrintf(PrinterCapture,LocaleString(MSG_CONSOLE_TERMINAL_TRANSCRIPT_ENDING_TXT));

		if(Force)
		{
			Close(PrinterCapture);

			CheckItem(MEN_CAPTURE_TO_PRINTER,FALSE);

			PrinterCapture = BNULL;

			ConOutputUpdate();

			StandardPrinterCapture = FALSE;
		}
	}

	ControllerActive = FALSE;

	ConOutputUpdate();
}

	/* OpenPrinterCapture(BOOL Controller):
	 *
	 *	Opens printer capture file.
	 */

BOOL
OpenPrinterCapture(BOOL Controller)
{
	if(PrinterCapture)
	{
		if(Controller && !ControllerActive)
		{
			ControllerActive = TRUE;

			ConOutputUpdate();

			FPrintf(PrinterCapture,LocaleString(MSG_CONSOLE_TERMINAL_TRANSCRIPT_FOLLOWS_TXT));
		}

		return(TRUE);
	}
	else
	{
		if(PrinterCapture = Open("PRT:",MODE_NEWFILE))
			CheckItem(MEN_CAPTURE_TO_PRINTER,TRUE);
		else
		{
			CheckItem(MEN_CAPTURE_TO_PRINTER,FALSE);

			BlockWindows();

			ShowRequest(Window,LocaleString(MSG_TERMMAIN_ERROR_OPENING_PRINTER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),"PRT:");

			ReleaseWindows();
		}

		if(Controller)
		{
			ControllerActive		= TRUE;
			StandardPrinterCapture	= FALSE;
		}
		else
		{
			StandardPrinterCapture = FALSE;

			if(ControllerActive)
				FPrintf(PrinterCapture,LocaleString(MSG_CONSOLE_USER_TERMINAL_TRANSCRIPT_FOLLOWS_TXT));
		}

		ConOutputUpdate();

		if(PrinterCapture)
			return(TRUE);
		else
			return(FALSE);
	}
}

	/* CloseFileCapture():
	 *
	 *	Close the capture file.
	 */

VOID
CloseFileCapture()
{
	if(FileCapture)
	{
		BufferClose(FileCapture);

		FileCapture = NULL;

		if(!GetFileSize(CaptureName))
			DeleteFile(CaptureName);
		else
		{
			AddProtection(CaptureName,FIBF_EXECUTE);

			if(Config->MiscConfig->CreateIcons)
				AddIcon(CaptureName,FILETYPE_TEXT,FALSE);
		}
	}

	CheckItem(MEN_CAPTURE_TO_FILE,FALSE);
	CheckItem(MEN_CAPTURE_TO_RAW_FILE,FALSE);

	RawCapture = FALSE;

	ConOutputUpdate();
}

	/* OpenFileCapture():
	 *
	 *	Open a capture file.
	 */

BOOL
OpenFileCapture(BOOL Raw)
{
	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester *FileRequest;
	BOOL Aborted;

	Aborted = FALSE;

	CloseFileCapture();

	BlockWindows();

	if(!CaptureName[0])
	{
		strcpy(CaptureName,Config->CaptureConfig->CapturePath);

		if(!AddPart(CaptureName,LocaleString(MSG_DIALPANEL_CAPTURE_NAME_TXT),sizeof(CaptureName)))
			CaptureName[0] = 0;
	}

	strcpy(DummyBuffer,CaptureName);

	if(FileRequest = SaveFile(Window,LocaleString(MSG_TERMMAIN_CAPTURE_TO_DISK_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
	{
		FreeAslRequest(FileRequest);

		if(GetFileSize(DummyBuffer))
		{
			switch(ShowRequest(Window,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_TXT),LocaleString(MSG_GLOBAL_CREATE_APPEND_CANCEL_TXT),DummyBuffer))
			{
				case 0:

					FileCapture = NULL;

					Aborted = TRUE;

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
			FileCapture = BufferOpen(DummyBuffer,"w");

		if(!Aborted)
		{
			if(!FileCapture)
				ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
			else
				strcpy(CaptureName,DummyBuffer);
		}
	}

	if(Raw)
	{
		if(FileCapture)
			CheckItem(MEN_CAPTURE_TO_RAW_FILE,TRUE);
		else
			CheckItem(MEN_CAPTURE_TO_RAW_FILE,FALSE);

		RawCapture = TRUE;
	}
	else
	{
		if(FileCapture)
			CheckItem(MEN_CAPTURE_TO_FILE,TRUE);
		else
			CheckItem(MEN_CAPTURE_TO_FILE,FALSE);

		RawCapture = FALSE;
	}

	ConOutputUpdate();

	ReleaseWindows();

	if(FileCapture)
		return(TRUE);
	else
		return(FALSE);
}

	/* CaptureToFile(APTR Buffer,LONG Size):
	 *
	 *	Send data to the capture file.
	 */

VOID
CaptureToFile(APTR Buffer,LONG Size)
{
	if(Size)
	{
		if(BufferWrite(FileCapture,Buffer,Size) != Size)
		{
			BlockWindows();

				/* We had an error writing to the file. */

			switch(ShowRequest(NULL,LocaleString(MSG_CONSOLE_ERROR_WRITING_TO_CAPTURE_FILE_TXT),LocaleString(MSG_CONSOLE_IGNORE_DISCARD_CLOSE_TXT),CaptureName))
			{
				case 1:

					BufferClose(FileCapture);

					DeleteFile(CaptureName);

					CheckItem(MEN_CAPTURE_TO_FILE,FALSE);

					FileCapture = NULL;

					ConOutputUpdate();

					break;

				case 2:

					BufferClose(FileCapture);

					CheckItem(MEN_CAPTURE_TO_FILE,FALSE);

					FileCapture = NULL;

					if(!GetFileSize(CaptureName))
						DeleteFile(CaptureName);
					else
					{
						AddProtection(CaptureName,FIBF_EXECUTE);

						if(Config->MiscConfig->CreateIcons)
							AddIcon(CaptureName,FILETYPE_TEXT,FALSE);
					}

					ConOutputUpdate();

					break;
			}

			ReleaseWindows();
		}
	}
}

	/* CaptureToPrinter(APTR Buffer,LONG Size):
	 *
	 *	Send capture data to the printer, convert the
	 *	characters as necessary to avoid shocking the
	 *	printer.
	 */

VOID
CaptureToPrinter(APTR Buffer,LONG Size)
{
	if(!Config->CaptureConfig->ConvertChars && Config->TerminalConfig->FontMode != FONT_STANDARD)
	{
		UBYTE  LocalBuffer[BUFFER_LINE_MAX];
		UBYTE *Src,*Dst,c;
		ULONG Count;

		Src = Buffer;

		while(Size > 0)
		{
			Count = MIN(Size,BUFFER_LINE_MAX);

			Size -= Count;

			Dst = LocalBuffer;

			do
			{
				if(c = ISOConversion[*Src++])
					*Dst++ = c;
				else
					*Dst++ = ' ';
			}
			while(--Count > 0);

			Count = (IPTR)Dst - (IPTR)&LocalBuffer[0];

			while(Count > 0 && LocalBuffer[Count - 1] == ' ')
				Count--;

			if(Count > 0)
			{
				if(Write(PrinterCapture,LocalBuffer,Count) != Count)
				{
					BlockWindows();

					if(!ShowRequest(Window,LocaleString(MSG_CONSOLE_ERROR_WRITING_TO_PRINTER_TXT),LocaleString(MSG_CONSOLE_IGNORE_CLOSE_PRINTER_TXT)))
					{
						Close(PrinterCapture);

						CheckItem(MEN_CAPTURE_TO_PRINTER,FALSE);

						PrinterCapture = BNULL;

						ConOutputUpdate();

						Size = 0;
					}

					ReleaseWindows();
				}
			}
		}
	}
	else
	{
		if(Size > 0)
		{
				/* Send the buffer to the printer. */

			if(Write(PrinterCapture,Buffer,Size) != Size)
			{
				BlockWindows();

				if(!ShowRequest(Window,LocaleString(MSG_CONSOLE_ERROR_WRITING_TO_PRINTER_TXT),LocaleString(MSG_CONSOLE_IGNORE_CLOSE_PRINTER_TXT)))
				{
					Close(PrinterCapture);

					CheckItem(MEN_CAPTURE_TO_PRINTER,FALSE);

					PrinterCapture = BNULL;

					ConOutputUpdate();
				}

				ReleaseWindows();
			}
		}
	}
}
