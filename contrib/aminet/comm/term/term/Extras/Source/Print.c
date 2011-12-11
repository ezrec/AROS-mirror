/*
**	Print.c
**
**	Printer control routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* PrintText(BPTR File,struct Window *ReqWindow,LONG *Error,STRPTR String,...):
	 *
	 *	Output a printf() style message.
	 */

BOOL
PrintText(BPTR File,struct Window *ReqWindow,LONG *Error,CONST_STRPTR String,...)
{
	UBYTE LocalBuffer[256];
	va_list	VarArgs;
	LONG Len;

	va_start(VarArgs,String);
	LimitedVSPrintf(sizeof(LocalBuffer),LocalBuffer,String,VarArgs);
	va_end(VarArgs);

	if(ReqWindow)
	{
		if(!SysReqHandler(ReqWindow,NULL,FALSE))
		{
			*Error = 0;

			return(FALSE);
		}
	}
	else
	{
		if(SetSignal(0,SIG_BREAK) & SIG_BREAK)
		{
			*Error = 0;

			return(FALSE);
		}
	}

	Len = strlen(LocalBuffer) + 1;

	SetIoErr(0);

	LimitedStrcat(sizeof(LocalBuffer),LocalBuffer,"\n");

	if(Write(File,LocalBuffer,Len) < Len)
	{
		*Error = IoErr();

		return(FALSE);
	}
	else
		return(TRUE);
}

	/* PrintHeader(BPTR File,struct Window *ReqWindow,LONG *Error,ULONG Code):
	 *
	 *	Print a line header.
	 */

STATIC BOOL
PrintHeader(BPTR File,struct Window *ReqWindow,LONG *Error,ULONG Code,BOOL Plain)
{
	UBYTE LocalBuffer[256];
	CONST_STRPTR String;
	LONG Len;

	String = LocaleString(Code);

	if(ReqWindow)
	{
		if(!SysReqHandler(ReqWindow,NULL,FALSE))
		{
			*Error = 0;

			return(FALSE);
		}
	}
	else
	{
		if(SetSignal(0,SIG_BREAK) & SIG_BREAK)
		{
			*Error = 0;

			return(FALSE);
		}
	}

	if(!Plain)
	{
		LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"\33[1m%s\33[0m",String);
		String = LocalBuffer;
	}

	SetIoErr(0);

	Len = strlen(String);

	if(Write(File,String,Len) < Len)
	{
		*Error = IoErr();

		return(FALSE);
	}

	return(TRUE);
}

	/* PrintFileInformation():
	 *
	 *	Print information on a file.
	 */

BOOL
PrintFileInformation(BPTR File,struct Window *ReqWindow,LONG *Error,STRPTR Name,ULONG Flags)
{
	BOOL Continue;

		/* Any special information to print along with the name? */

	if(Flags)
	{
		BPTR FileLock;

			/* Try to grip the file. */

		if(FileLock = Lock(Name,ACCESS_READ))
		{
			D_S(struct FileInfoBlock,FileInfo);

				/* How does it look like? */

			if(Examine(FileLock,FileInfo))
			{
				UBYTE DummyBuffer[300];
				STRPTR Index;

					/* Add the size. */

				if(Flags & PRINT_SIZE)
					LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%-25s %7ld",FilePart(Name),FileInfo->fib_Size);
				else
					LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%-25s",FilePart(Name));

				Index = DummyBuffer;

					/* Find the end of the string. */

				while(*Index)
					Index++;

					/* Add the protection bits. */

				if(Flags & PRINT_BITS)
				{
					STATIC STRPTR	SetBits = "----aps",
									ClrBits = "dewr---";

					UBYTE TempString[10];

					LONG i;

					strcpy(TempString," -------");

					for(i = 0 ; i < 7 ; i++)
					{
						if(FileInfo->fib_Protection & (1L << i))
							TempString[6 - i + 1] = SetBits[i];
						else
							TempString[6 - i + 1] = ClrBits[i];
					}

					strcpy(Index,TempString);

					while(*Index)
						Index++;
				}

					/* Add the creation date. */

				if(Flags & PRINT_DATE)
				{
					UBYTE Date[20],Time[20];
					struct DateTime	DateTime;

						/* Prepare for date conversion. */

					DateTime.dat_Stamp		= FileInfo->fib_Date;
					DateTime.dat_Format		= FORMAT_DEF;
					DateTime.dat_Flags		= DTF_SUBST;
					DateTime.dat_StrDay		= NULL;
					DateTime.dat_StrDate	= Date;
					DateTime.dat_StrTime	= Time;

						/* Convert the date. */

					if(DateToStr(&DateTime))
					{
						LimitedSPrintf(sizeof(DummyBuffer) - ((IPTR)Index - (IPTR)DummyBuffer),Index," %-9s %s",Date,Time);

						while(*Index)
							Index++;
					}
				}

					/* Add the file comment. */

				if(Flags & PRINT_COMMENT)
					LimitedSPrintf(sizeof(DummyBuffer) - ((IPTR)Index - (IPTR)DummyBuffer),Index,"\n: %s",FileInfo->fib_Comment);

				Continue = PrintText(File,ReqWindow,Error,"%s\n",DummyBuffer);
			}
			else
				Continue = FALSE;

			UnLock(FileLock);
		}
		else
			Continue = FALSE;
	}
	else
		Continue = PrintText(File,ReqWindow,Error,"%s\n",Name);

	return(Continue);
}

	/* PrintEntry(BPTR File,struct Window *ReqWindow,LONG *Error,struct PhoneEntry *Entry):
	 *
	 *	Print information on the contents of a phonebook entry.
	 */

BOOL
PrintEntry(BPTR File,struct Window *ReqWindow,BOOL Plain,LONG *Error,struct PhoneEntry *Entry,ULONG Flags)
{
	if(Plain)
	{
		if(!PrintText(File,ReqWindow,Error,"\n\"%s\" (%s)",Entry->Header->Name,Entry->Header->Number))
			return(FALSE);
	}
	else
	{
		if(!PrintText(File,ReqWindow,Error,"\n\33[4m\"%s\" (%s)\33[0m",Entry->Header->Name,Entry->Header->Number))
			return(FALSE);
	}

	if(Flags & PRINT_COMMENT)
	{
		if(Entry->Header->Comment[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_COMMENT_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,Entry->Header->Comment))
				return(FALSE);
		}
	}

	if(Flags & PRINT_USERNAME)
	{
		if(Entry->Header->UserName[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_USER_NAME_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,Entry->Header->UserName))
				return(FALSE);
		}
	}

	if((Flags & PRINT_SERIAL) && Entry->Config->SerialConfig)
	{
		STATIC UBYTE Parities[] =
		{
			'N','E','O','M','S'
		};

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_BAUD_RATE_TXT,Plain))
			return(FALSE);

		if(LocaleBase)
		{
			if(!PrintText(File,ReqWindow,Error,"%lD",Entry->Config->SerialConfig->BaudRate))
				return(FALSE);
		}
		else
		{
			if(!PrintText(File,ReqWindow,Error,"%ld",Entry->Config->SerialConfig->BaudRate))
				return(FALSE);
		}

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_PARAMETERS_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,"%ld-%lc-%ld",Entry->Config->SerialConfig->BitsPerChar,Parities[(WORD)Entry->Config->SerialConfig->Parity],Entry->Config->SerialConfig->StopBits))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_HANDSHAKING_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_SERIALPANEL_HANDSHAKING_NONE_TXT + Entry->Config->SerialConfig->HandshakingProtocol)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DUPLEX_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_SERIALPANEL_DUPLEX_FULL_TXT + Entry->Config->SerialConfig->Duplex)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_STRIP_BIT_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_DISABLED_TXT + Entry->Config->SerialConfig->StripBit8)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_FLOW_CONTROL_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_DISABLED_TXT + Entry->Config->SerialConfig->xONxOFF)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_SERIAL_DRIVER_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_NAME_UNIT_TEMPLATE_TXT),Entry->Config->SerialConfig->SerialDevice, + Entry->Config->SerialConfig->UnitNumber))
			return(FALSE);
	}

	if((Flags & PRINT_MODEM) && Entry->Config->ModemConfig)
	{
		if(Entry->Config->ModemConfig->ModemInit[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_MODEM_INIT_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->ModemConfig->ModemInit))
				return(FALSE);
		}

		if(Entry->Config->ModemConfig->ModemExit[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_MODEM_EXIT_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->ModemConfig->ModemExit))
				return(FALSE);
		}

		if(Entry->Config->ModemConfig->ModemHangup[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_HANG_UP_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->ModemConfig->ModemHangup))
				return(FALSE);
		}

		if(Entry->Config->ModemConfig->DialPrefix[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DIAL_PREFIX_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->ModemConfig->DialPrefix))
				return(FALSE);
		}

		if(Entry->Config->ModemConfig->DialSuffix[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DIAL_SUFFIX_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->ModemConfig->DialSuffix))
				return(FALSE);
		}

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_REDIAL_DELAY_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_MINUTE_SECOND_TEMPLATE_TXT),Entry->Config->ModemConfig->RedialDelay / 60,Entry->Config->ModemConfig->RedialDelay % 60))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DIAL_TIMEOUT_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_MINUTE_SECOND_TEMPLATE_TXT),Entry->Config->ModemConfig->DialTimeout / 60,Entry->Config->ModemConfig->DialTimeout % 60))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_AUTO_BAUD_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_DISABLED_TXT + Entry->Config->ModemConfig->ConnectAutoBaud)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DROP_DTR_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_DISABLED_TXT + Entry->Config->ModemConfig->DropDTR)))
			return(FALSE);
	}

	if((Flags & PRINT_SCREEN) && Entry->Config->ScreenConfig)
	{
		UBYTE ModeNameBuffer[DISPLAYNAMELEN + 1];

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_DISPLAY_MODE_TXT,Plain))
			return(FALSE);

		GetModeName(Entry->Config->ScreenConfig->DisplayMode,ModeNameBuffer,sizeof(ModeNameBuffer));

		if(!PrintText(File,ReqWindow,Error,ModeNameBuffer))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_COLOUR_MODE_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_SCREENPANEL_COLOUR_AMIGA_TXT + Entry->Config->ScreenConfig->ColourMode)))
			return(FALSE);
	}

	if((Flags & PRINT_TERMINAL) && Entry->Config->TerminalConfig)
	{
		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_TERMINAL_EMULATION_TXT,Plain))
			return(FALSE);

		if(Entry->Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		{
			if(!PrintText(File,ReqWindow,Error,"%s, \"%s\"",LocaleString(MSG_TERMINALPANEL_EMULATION_ANSI_VT102_TXT + Entry->Config->TerminalConfig->EmulationMode),Entry->Config->TerminalConfig->EmulationMode))
				return(FALSE);
		}
		else
		{
			if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_TERMINALPANEL_EMULATION_ANSI_VT102_TXT + Entry->Config->TerminalConfig->EmulationMode)))
				return(FALSE);
		}

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_FONT_TXT,Plain))
			return(FALSE);

		if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_TERMINALPANEL_FONT_STANDARD_TXT + Entry->Config->TerminalConfig->FontMode)))
			return(FALSE);

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_TEXT_COLUMNS_TXT,Plain))
			return(FALSE);

		if(Entry->Config->TerminalConfig->NumColumns < 20)
		{
			if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_MAXIMUM_TXT)))
				return(FALSE);
		}
		else
		{
			if(!PrintText(File,ReqWindow,Error,"%ld",Entry->Config->TerminalConfig->NumColumns))
				return(FALSE);
		}

		if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_TEXT_LINES_TXT,Plain))
			return(FALSE);

		if(Entry->Config->TerminalConfig->NumLines < 20)
		{
			if(!PrintText(File,ReqWindow,Error,LocaleString(MSG_PRINTPANEL_MAXIMUM_TXT)))
				return(FALSE);
		}
		else
		{
			if(!PrintText(File,ReqWindow,Error,"%ld",Entry->Config->TerminalConfig->NumLines))
				return(FALSE);
		}

		if(Entry->Config->TerminalConfig->KeyMapFileName[0])
		{
			if(!PrintHeader(File,ReqWindow,Error,MSG_PRINTPANEL_KEYMAP_FILE_TXT,Plain))
				return(FALSE);

			if(!PrintText(File,ReqWindow,Error,"\"%s\"",Entry->Config->TerminalConfig->KeyMapFileName))
				return(FALSE);
		}
	}

	return(TRUE);
}

	/* PrintScreen(BPTR File,struct Window *ReqWindow,LONG *Error):
	 *
	 *	Print the contents of the screen, requires the raster
	 *	to be available.
	 */

BOOL
PrintScreen(BPTR File,struct Window *ReqWindow,LONG *Error)
{
	UBYTE *Buffer;
	LONG i,j;

		/* Run down the lines... */

	for(i = 0 ; i <= LastLine ; i++)
	{
			/* Grab the line. */

		Buffer = &Raster[i * RasterWidth];

		j = LastColumn;

			/* Strip trailing spaces. */

		while(j >= 0 && Buffer[j] == ' ')
			j--;

			/* Blank line? */

		if(j >= 0)
		{
			SetIoErr(0);

			if(Write(File,Buffer,j + 1) < j + 1)
			{
				*Error = IoErr();

				return(FALSE);
			}
		}

			/* Is printing to be aborted? */

		if(!SysReqHandler(ReqWindow,NULL,FALSE))
		{
			*Error = 0;

			return(FALSE);
		}

			/* Add line terminator. */

		SetIoErr(0);

		if(Write(File,"\n",1) < 1)
		{
			*Error = IoErr();

			return(FALSE);
		}

			/* Is printing to be aborted? */

		if(!SysReqHandler(ReqWindow,NULL,FALSE))
		{
			*Error = 0;

			return(FALSE);
		}
	}

	return(TRUE);
}

	/* PrintClip(BPTR File,struct Window *ReqWindow,LONG *Error):
	 *
	 *	Print the contents of the clipboard.
	 */

BOOL
PrintClip(BPTR File,struct Window *ReqWindow,LONG *Error)
{
		/* Are we currently reading input from the
		 * clipboard? If so, close it.
		 */

	if(ClipInput)
	{
		CloseClip();

		ClipInput = ClipXerox = FALSE;
	}

		/* Open the clipboard for reading. */

	if(*Error = OpenClip(Config->ClipConfig->ClipboardUnit))
		return(FALSE);
	else
	{
		UBYTE InputBuffer[256];
		LONG Len;

			/* Read clipboard contents. */

		while((Len = GetClip(InputBuffer,sizeof(InputBuffer) - 1)) > 0)
		{
				/* Are we to stop printing? */

			if(!SysReqHandler(ReqWindow,NULL,FALSE))
			{
				*Error = 0;

				CloseClip();

				return(FALSE);
			}
			else
			{
				SetIoErr(0);

				if(Write(File,InputBuffer,Len) < Len)
				{
					*Error = IoErr();

					CloseClip();

					return(FALSE);
				}
			}
		}

		if(Len < 0)
		{
			if(SysReqHandler(ReqWindow,NULL,FALSE) == -2)
			{
				SetIoErr(0);

				if(Write(File,"\n",1) < 1)
					*Error = IoErr();

				CloseClip();

				return(FALSE);
			}
		}

		CloseClip();
	}

	return(TRUE);
}

	/* PrintBuffer(BPTR File,struct Window *ReqWindow,LONG *Error):
	 *
	 *	Print the contents of the text buffer.
	 */

BOOL
PrintBuffer(BPTR File,struct Window *ReqWindow,LONG *Error)
{
	BOOL Continue;
	LONG i,Len;

	SafeObtainSemaphoreShared(&BufferSemaphore);

	Continue = TRUE;

	if(BufferLines)
	{
		for(i = 0 ; i < Lines ; i++)
		{
			Len = BufferLines[i][-1];

			if(!SysReqHandler(ReqWindow,NULL,FALSE))
			{
				*Error = 0;

				Continue = FALSE;

				break;
			}

			if(Len)
			{
				SetIoErr(0);

				if(Write(File,BufferLines[i],Len) < Len)
				{
					*Error = IoErr();

					Continue = FALSE;

					break;
				}
			}

			if(!SysReqHandler(ReqWindow,NULL,FALSE))
			{
				*Error = 0;

				Continue = FALSE;

				break;
			}

			SetIoErr(0);

			if(Write(File,"\n",1) < 1)
			{
				*Error = IoErr();

				Continue = FALSE;

				break;
			}
		}
	}
	else
		Continue = FALSE;

	ReleaseSemaphore(&BufferSemaphore);

	return(Continue);
}

	/* PrintSomething(BYTE Source):
	 *
	 *	Print the screen or the current contents of the clipboard.
	 */

VOID
PrintSomething(LONG Source)
{
	struct Window *ReqWindow;
	struct EasyStruct Easy;
	LONG Error;

		/* Fill in the easy requester structure. */

	Easy.es_StructSize		= sizeof(struct EasyStruct);
	Easy.es_Flags			= 0;
	Easy.es_Title			= (STRPTR)LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
	Easy.es_GadgetFormat	= (STRPTR)LocaleString(MSG_PRINT_STOP_TXT);

	if(Source == PRINT_CLIP)
		Easy.es_TextFormat = (STRPTR)LocaleString(MSG_PRINT_PRINTING_CLIP_TXT);
	else
		Easy.es_TextFormat = (STRPTR)LocaleString(MSG_PRINT_PRINTING_SCREEN_TXT);

		/* The requester is to be displayed while printing. */

	if(ReqWindow = BuildEasyRequest(Window,&Easy,0))
	{
		BPTR SomeFile;

			/* Add header information if printer channel is already open. */

		if(PrinterCapture)
		{
			LONG Len = strlen(LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_FOLLOWS_TXT));

			if(Write(PrinterCapture,LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_FOLLOWS_TXT),Len) < Len)
			{
				FreeSysRequest(ReqWindow);

				ReqWindow = NULL;

				Error = IoErr();

				SomeFile = BNULL;
			}
			else
				SomeFile = PrinterCapture;
		}
		else
		{
				/* Open printer channel. */

			if(!(SomeFile = Open("PRT:",MODE_NEWFILE)))
			{
				FreeSysRequest(ReqWindow);

				ReqWindow = NULL;

				Error = IoErr();
			}
		}

			/* Everything fine so far? */

		if(!Error && SomeFile)
		{
			BOOL Stopped;

				/* Are we to print the screen? */

			if(Source == PRINT_SCREEN)
				Stopped = !PrintScreen(SomeFile,ReqWindow,&Error);
			else
				Stopped = !PrintClip(SomeFile,ReqWindow,&Error);

				/* Add a trailer if necessary. */

			if(PrinterCapture)
			{
				if(!Error && !Stopped)
				{
					LONG Len;

					SetIoErr(0);

					Len = strlen(LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_ENDING_TXT));

					if(Write(PrinterCapture,LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_ENDING_TXT),Len) < Len)
						Error = IoErr();
				}
			}
			else
			{
					/* Close the printer stream. */

				if(!Close(SomeFile))
					Error = IoErr();
			}
		}

			/* Release the system requster. */

		if(ReqWindow)
			FreeSysRequest(ReqWindow);

			/* Display the error code if necessary. */

		if(Error)
		{
			UBYTE LocalBuffer[256];
			STRPTR ErrorString;

			if(Fault(Error,NULL,LocalBuffer,sizeof(LocalBuffer)))
				ErrorString = LocalBuffer;
			else
				ErrorString = "???";

			ShowRequest(Window,LocaleString(MSG_PRINT_ERROR_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Error,ErrorString);
		}
	}
}

	/* PrintRegion(LONG Top,LONG Bottom):
	 *
	 *	Print the contents of a screen region.
	 */

VOID
PrintRegion(LONG Top,LONG Bottom,BOOL FormFeed)
{
	BPTR SomeFile;
	LONG i,j;
	UBYTE *Buffer;

	BlockWindows();

	if(PrinterCapture)
	{
		LONG Len = strlen(LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_FOLLOWS_TXT));

		if(Write(PrinterCapture,LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_FOLLOWS_TXT),Len) < Len)
		{
			ShowRequest(Window,LocaleString(MSG_CONSOLE_ERROR_WRITING_TO_PRINTER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

			ReleaseWindows();

			return;
		}

		SomeFile = PrinterCapture;
	}
	else
	{
		if(!(SomeFile = Open("PRT:",MODE_NEWFILE)))
		{
			ShowRequest(Window,LocaleString(MSG_TERMMAIN_FAILED_TO_OPEN_PRINTER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

			ReleaseWindows();

			return;
		}
	}

	for(i = Top ; i < Bottom ; i++)
	{
		Buffer = &Raster[i * RasterWidth];

		j = LastColumn;

		while(j >= 0 && Buffer[j] == ' ')
			j--;

		if(j >= 0)
		{
			SetIoErr(0);

			if(Write(SomeFile,Buffer,j + 1) < j + 1)
			{
				FormFeed = FALSE;

				break;
			}
		}

		SetIoErr(0);

		if(Write(SomeFile,"\n",1) < 1)
		{
			FormFeed = FALSE;

			break;
		}
	}

	if(PrinterCapture)
	{
		LONG Len = strlen(LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_ENDING_TXT));

		Write(PrinterCapture,LocaleString(MSG_CONSOLE_SCREEN_PRINTOUT_ENDING_TXT),Len);
	}
	else
	{
		if(FormFeed)
			Write(SomeFile,"\f",1);

		Close(SomeFile);
	}

	ReleaseWindows();

}

	/* PrintScreenGfx():
	 *
	 *	Print the window contents as graphics.
	 */

BOOL
PrintScreenGfx()
{
	struct MsgPort *PrintPort;
	LONG Error;

		/* Create the printer port */

	if(PrintPort = CreateMsgPort())
	{
		struct IODRPReq *PrintRequest;

			/* Create the rastport dump request */

		if(PrintRequest = (struct IODRPReq *)CreateIORequest(PrintPort,sizeof(struct IODRPReq)))
		{
				/* Open the printer driver */

			if(!OpenDevice("printer.device",0,(struct IORequest *)PrintRequest,0))
			{
				struct RastPort *RPort;

					/* Create a new rastport */

				if(RPort = (struct RastPort *)AllocVecPooled(sizeof(struct RastPort),MEMF_ANY))
				{
					struct BitMap *BitMap;
					LONG Width,Height;

						/* Initialize the rastport */

					InitRastPort(RPort);

						/* Keep these handy */

					Width	= Window->Width		- (Window->BorderLeft + Window->BorderRight);
					Height	= Window->Height	- (Window->BorderTop + Window->BorderBottom);

					if(StatusWindow)
						Height -= StatusDisplayHeight;

						/* Allocate offscreen buffer to hold the window contents */

					if(BitMap = CreateBitMap(Width,Height,GetBitMapDepth(Window->RPort->BitMap),0,Window->RPort->BitMap))
					{
						struct EasyStruct Easy;
						struct Window *ReqWindow;

							/* Put the bitmap into the rastport */

						RPort->BitMap = BitMap;

							/* Clear the bitmap */

						SetRast(RPort,0);

							/* Copy the window contents to the offscreen buffer */

						ClipBlit(Window->RPort,Window->BorderLeft,Window->BorderTop,RPort,0,0,Width,Height,MINTERM_COPY);

							/* Wait for the bitmap to be transferred */

						WaitBlit();

							/* Set up the print request */

						PrintRequest->io_Command	= PRD_DUMPRPORT;
						PrintRequest->io_RastPort	= RPort;
						PrintRequest->io_ColorMap	= Window->WScreen->ViewPort.ColorMap;
						PrintRequest->io_Modes		= GetVPModeID(&Window->WScreen->ViewPort);
						PrintRequest->io_SrcWidth	= Width;
						PrintRequest->io_SrcHeight	= Height;

							/* Set up the abort requester */

						Easy.es_StructSize		= sizeof(Easy);
						Easy.es_Flags			= 0;
						Easy.es_Title			= LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
						Easy.es_GadgetFormat	= LocaleString(MSG_GLOBAL_ABORT_GAD);
						Easy.es_TextFormat		= LocaleString(MSG_PRINTING_SCREEN_TXT);

							/* Create the abort requester */

						if(ReqWindow = BuildEasyRequest(Window,&Easy,0))
						{
							ULONG Signals;

								/* Everything is fine so far */

							Error = 0;

								/* Start printing */

							BeginIO((struct IORequest *)PrintRequest);

								/* Run until everything is done */

							while(TRUE)
							{
									/* Wait for an event */

								Signals = Wait(PORTMASK(ReqWindow->UserPort) | PORTMASK(PrintPort));

									/* Is the printer finished? */

								if(Signals & PORTMASK(PrintPort))
								{
										/* Wait for the request to return and check for error */

									switch(WaitIO((struct IORequest *)PrintRequest))
									{
										case PDERR_NOTGRAPHICS:

											Error = ERR_NO_GFX_OUTPUT;
											break;

										case PDERR_BADDIMENSION:

											Error = ERR_BAD_DIMENSION;
											break;

										case IOERR_OPENFAIL:

											Error = ERR_NO_PRINTER;
											break;

										case PDERR_INTERNALMEMORY:
										case PDERR_BUFFERMEMORY:

											Error = ERROR_NO_FREE_STORE;
											break;
									}

									break;
								}

									/* Did the user press the abort button? */

								if(Signals & PORTMASK(ReqWindow->UserPort))
								{
									if(!SysReqHandler(ReqWindow,NULL,FALSE))
									{
										AbortIO((struct IORequest *)PrintRequest);

										WaitIO((struct IORequest *)PrintRequest);

										break;
									}
								}
							}

								/* Close the requester */

							FreeSysRequest(ReqWindow);
						}
						else
							Error = ERROR_NO_FREE_STORE;

							/* Free the memory allocated for the bitmap */

						DeleteBitMap(BitMap);
					}
					else
						Error = ERROR_NO_FREE_STORE;

						/* Free the rastport */

					FreeVecPooled(RPort);
				}
				else
					Error = ERROR_NO_FREE_STORE;

					/* Close the printer driver */

				CloseDevice((struct IORequest *)PrintRequest);
			}
			else
				Error = ERR_NO_PRINTER;

				/* Free the rastport dump request */

			DeleteIORequest((struct IORequest *)PrintRequest);
		}
		else
			Error = ERROR_NO_FREE_STORE;

			/* Free the printer port */

		DeleteMsgPort(PrintPort);
	}
	else
		Error = ERROR_NO_FREE_STORE;

		/* Return the result */

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}
