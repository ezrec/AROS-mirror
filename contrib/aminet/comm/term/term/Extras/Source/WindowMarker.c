/*
**	WindowMarker.c
**
**	Main window text block selection routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

/****************************************************************************/

#define DB(x)	;

/**********************************************************************/

STATIC struct MarkerContext *WindowMarkerContext;
STATIC BOOL					 WindowMarkerInterrupted;

/**********************************************************************/

	/* Ask for the mouse position to be converted into a line and column index. */

STATIC VOID
Window_AskPosition(struct MarkerContext *Context,LONG *Column,LONG *Line,WORD Mode)
{
	struct TextExtent Extent;
	LONG Left,Top;

	Left	= Window->MouseX - Window->BorderLeft;
	Top		= Window->MouseY - Window->BorderTop;

	if(Mode == MARKERASK_Scroll)
	{
		if(Top < 0)
		{
			*Line	= -1;
			*Column	= 0;
		}
		else if(Top >= WindowHeight)
		{
			*Line	= Context->NumVisibleLines;
			*Column	= Context->NumVisibleColumns - 1;
		}
		else
		{
			*Line = Top / Window->RPort->TxHeight;

			if(Left < 0)
				*Column = -1;
			else if (Left >= WindowWidth)
				*Column = Context->NumVisibleColumns;
			else
				*Column = TextFit(Window->RPort,&Raster[RasterWidth * (*Line)],LastColumn + 1,&Extent,NULL,1,Left,32767);
		}
	}
	else
	{
		if(Top < 0)
			Top = 0;
		else if(Top >= WindowHeight)
			Top = WindowHeight - 1;

		if(Left < 0)
			Left = 0;
		else if (Left >= WindowWidth)
			Left = WindowWidth - 1;

		*Line	= Top / Window->RPort->TxHeight;
		*Column = TextFit(Window->RPort,&Raster[RasterWidth * (*Line)],LastColumn + 1,&Extent,NULL,1,Left,32767);
	}

	DB(kprintf("\033[0 p\033[HPosition     <%4ld,%4ld>\033[K\n",*Line,*Column));
}

	/* Scroll the visible region. */

STATIC VOID
Window_Scroll(struct MarkerContext *UnusedContext,LONG UnusedDeltaX,LONG UnusedDeltaY)
{
}

	/* Highlight a line of text between the "Left" and "Right" column (inclusive).
	 */

STATIC VOID
Window_Highlight(struct MarkerContext *UnusedContext,LONG Line,LONG Left,LONG Right)
{
	if(Line >= 0 && Line <= LastLine)
	{
		if(Left >= 0 && Right <= LastColumn && Left <= Right)
		{
			struct RastPort *RPort;
			LONG First,Length;

			RPort = Window->RPort;

			First	= TextLength(RPort,&Raster[RasterWidth * Line],Left);
			Length	= TextLength(RPort,&Raster[RasterWidth * Line + Left],Right - Left + 1);

			if(Length > 0)
			{
				ULONG OldAPen,OldBPen,OldDrMd;

				OldAPen	= ReadAPen(RPort);
				OldBPen	= ReadBPen(RPort);
				OldDrMd	= ReadDrMd(RPort);

				DB(kprintf("\033[2HMark    %4ld <%4ld,%4ld>\033[K\n",Line,Left,Right));

				SetPens(RPort,(ULONG)~0,OldBPen,JAM1 | COMPLEMENT);
				FillWindowBox(Window,First,Line * RPort->TxHeight,Length,RPort->TxHeight);
				SetPens(RPort,OldAPen,OldBPen,OldDrMd);
			}
		}
	}
}

	/* Transfer the buffer contents line by line. */

STATIC BOOL
Window_Put(APTR UserData,STRPTR Buffer,LONG Length)
{
	BOOL Result;

	if(Config->TerminalConfig->FontMode == FONT_STANDARD)
		Result = (BOOL)(WriteChunkBytes(UserData,Buffer,Length) == Length);
	else
		Result = WriteTranslatedToClip(UserData,Buffer,Length);

	return(Result);
}

STATIC BOOL
Window_PutLine(APTR UserData,STRPTR Buffer,LONG Length)
{
	BOOL Result;

	if(Config->TerminalConfig->FontMode == FONT_STANDARD)
		Result = (BOOL)(WriteChunkBytes(UserData,Buffer,Length) == Length);
	else
		Result = WriteTranslatedToClip(UserData,Buffer,Length);

	if(Result)
		Result = (BOOL)(WriteChunkBytes(UserData,"\n",1) == 1);

	return(Result);
}

STATIC BOOL
Window_Transfer(struct MarkerContext *UnusedContext,LONG Line,LONG Left,LONG Right,MARKER_Put Put,APTR UserData)
{
	if(Line >= 0 && Line <= RasterHeight)
	{
		if(Left >= 0 && Right <= RasterWidth)
			return(Put(UserData,&Raster[RasterWidth * Line + Left],Right - Left + 1));
	}

	return(TRUE);
}

STATIC BOOL
Window_PickWord(struct MarkerContext *UnusedContext,LONG Left,LONG Top,LONG *WordLeft,LONG *WordRight)
{
	BOOL GotIt;

	GotIt = FALSE;

	SafeObtainSemaphoreShared(&RasterSemaphore);

	if(Left >= 0 && Left < RasterWidth && Top >= 0 && Top < RasterHeight)
	{
		if(Raster[RasterWidth * Top + Left] != ' ')
		{
			STRPTR RasterLine;
			LONG From,To;

			From = To = Left;
			RasterLine = &Raster[RasterWidth * Top];

			while(From > 0 && RasterLine[From - 1] != ' ')
				From--;

			while(To < RasterWidth - 1 && RasterLine[To + 1] != ' ')
				To++;

			GotIt = TRUE;

			*WordLeft	= From;
			*WordRight	= To;
		}
	}

	ReleaseSemaphore(&RasterSemaphore);

	return(GotIt);
}

STATIC APTR
Window_TransferStartStop(struct MarkerContext *UnusedContext,APTR UserData,ULONG Qualifier)
{
	struct IFFHandle *Handle;

	if(Handle = UserData)
	{
		if(!PopChunk(Handle))
			PopChunk(Handle);
	}
	else
	{
		APTR Buffer;
		LONG Size;

		SafeObtainSemaphoreShared(&RasterSemaphore);

		SetWait(Window);

		Buffer	= NULL;
		Size	= 0;

		if(Qualifier & SHIFT_KEY)
			GetClipContents(Config->ClipConfig->ClipboardUnit,&Buffer,&Size);

		if(Handle = OpenIFFClip(Config->ClipConfig->ClipboardUnit,MODE_NEWFILE))
		{
			if(!PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN))
			{
				if(!PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN))
				{
					if(Size > 0)
					{
						BOOL Ok;

						Ok = (BOOL)(WriteChunkBytes(Handle,Buffer,Size) == Size);

						FreeVecPooled(Buffer);

						if(Ok)
							return(Handle);
					}
					else
						return(Handle);
				}
			}
		}
	}

	CloseIFFClip(Handle);

	ReleaseSemaphore(&RasterSemaphore);

	ClrWait(Window);

	return(NULL);
}

VOID
WindowMarkerStop()
{
	if(WindowMarkerContext)
	{
		UpMarker(WindowMarkerContext);
		Marking = FALSE;

		FreeVecPooled(WindowMarkerContext);
		WindowMarkerContext = NULL;

		ReportMouse(FALSE,Window);
	}

	OffItem(MEN_COPY);
	OffItem(MEN_CLEAR);
}

VOID
WindowMarkerSelectAll()
{
	WindowMarkerStop();

	if(WindowMarkerContext = CreateMarkerContext(
		Window_AskPosition,
		Window_Scroll,
		Window_Highlight,
		Window_Highlight,
		Window_TransferStartStop,
		Window_Transfer,
		Window_Put,
		Window_PutLine,
		Window_PickWord
	))
	{
		SelectAllMarker(
			WindowMarkerContext,0,LastLine + 1,LastLine + 1,

			0,LastColumn + 1,LastColumn + 1,

			0,0,LastColumn,LastLine
		);

		Marking = TRUE;
		WindowMarkerInterrupt();
	}
}

VOID
WindowMarkWord()
{
	WindowMarkerStop();

	if(WindowMarkerContext = CreateMarkerContext(
		Window_AskPosition,
		Window_Scroll,
		Window_Highlight,
		Window_Highlight,
		Window_TransferStartStop,
		Window_Transfer,
		Window_Put,
		Window_PutLine,
		Window_PickWord
	))
	{
		if(!SetWordMarker(WindowMarkerContext,0,LastLine + 1,LastLine + 1,0,LastColumn + 1,LastColumn + 1))
			WindowMarkerStop();
		else
		{
			Marking = TRUE;
			WindowMarkerInterrupt();
		}
	}
}

VOID
WindowMarkerStart(ULONG MsgQualifier)
{
	if(WindowMarkerContext && !(MsgQualifier & SHIFT_KEY))
		WindowMarkerStop();

	WindowMarkerInterrupted = FALSE;

	if(WindowMarkerContext)
		MoveMouseMarker(WindowMarkerContext);
	else
	{
		if(WindowMarkerContext = CreateMarkerContext(
			Window_AskPosition,
			Window_Scroll,
			Window_Highlight,
			Window_Highlight,
			Window_TransferStartStop,
			Window_Transfer,
			Window_Put,
			Window_PutLine,
			Window_PickWord
		))
		{
			DownMarker(WindowMarkerContext,0,LastLine + 1,LastLine + 1,0,LastColumn + 1,LastColumn + 1);
			Marking = TRUE;
		}
	}

	if(Marking)
		ReportMouse(TRUE,Window);
}

VOID
WindowMarkerInterrupt()
{
	if(CheckMarker(WindowMarkerContext))
	{
		OnItem(MEN_COPY);
		OnItem(MEN_CLEAR);
	}
	else
	{
		OffItem(MEN_COPY);
		OffItem(MEN_CLEAR);
	}

	WindowMarkerInterrupted = TRUE;

	ReportMouse(FALSE,Window);
}

VOID
WindowMarkerMoveMouse()
{
	if(WindowMarkerContext && !WindowMarkerInterrupted)
		MoveMouseMarker(WindowMarkerContext);
}

VOID
WindowMarkerTransfer(ULONG MsgQualifier)
{
	if(WindowMarkerContext)
	{
		TransferMarker(WindowMarkerContext,MsgQualifier);

		WindowMarkerStop();
	}
}
