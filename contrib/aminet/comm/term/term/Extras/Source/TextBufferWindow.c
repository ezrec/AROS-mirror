/*
**	TextBufferWindow.c
**
**	Support routines for the text buffer window and screen.
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

STATIC VOID Buffer_AskPosition(struct MarkerContext *Context, LONG *Column, LONG *Line, WORD Mode);
STATIC VOID Buffer_Scroll(struct MarkerContext *Context, LONG DeltaX, LONG DeltaY);
STATIC VOID Buffer_Highlight(struct MarkerContext *Context, LONG Line, LONG Left, LONG Right);
STATIC BOOL Buffer_Put(APTR UserData, STRPTR Buffer, LONG Length);
STATIC BOOL Buffer_PutLine(APTR UserData, STRPTR Buffer, LONG Length);
STATIC BOOL Buffer_Transfer(struct MarkerContext *Context, LONG Line, LONG Left, LONG Right, MARKER_Put Put, APTR UserData);
STATIC BOOL Buffer_PickWord(struct MarkerContext *Context, LONG Left, LONG Top, LONG *WordLeft, LONG *WordRight);
STATIC APTR Buffer_TransferStartStop(struct MarkerContext *Context, APTR UserData, ULONG Qualifier);
STATIC VOID BufferMarkerStop(TextBufferInfo *BufferInfo);
STATIC VOID BufferMarkWord(TextBufferInfo *BufferInfo);
STATIC VOID BufferMarkerStart(TextBufferInfo *BufferInfo, UWORD MsgQualifier);
STATIC VOID BufferMarkerInterrupt(TextBufferInfo *BufferInfo);
STATIC VOID BufferMarkerMoveMouse(TextBufferInfo *BufferInfo);
STATIC VOID BufferMarkerTransfer(TextBufferInfo *BufferInfo, UWORD MsgQualifier);
STATIC VOID FlushMsg(struct Window *Window);
STATIC VOID BufferClear(TextBufferInfo *BufferInfo, LONG Left, LONG Top, LONG Right, LONG Bottom);
STATIC VOID BufferScroll(TextBufferInfo *BufferInfo, LONG Lines);
STATIC VOID BufferComplement(TextBufferInfo *BufferInfo, LONG SrcX, LONG SrcY, LONG Width, LONG Height);
STATIC VOID DeleteScroller(TextBufferInfo *BufferInfo);
STATIC BOOL CreateScroller(TextBufferInfo *BufferInfo, LONG Height);
STATIC VOID PrintLine(TextBufferInfo *BufferInfo, STRPTR Buffer, LONG LineNumber);
STATIC VOID PrintLineWithMarker(TextBufferInfo *BufferInfo, STRPTR Buffer, LONG LineNumber, LONG AbsoluteLine);
STATIC LONG RedrawScreen(TextBufferInfo *BufferInfo, LONG FirstLine);
STATIC VOID BufferDestructor(struct DataMsg *Item);
STATIC VOID BufferSerWrite(TextBufferInfo *BufferInfo, APTR Data, LONG Size);
STATIC VOID StartSearch(TextBufferInfo *BufferInfo, struct SearchInfo *SearchInfo, STRPTR SearchBuffer);
STATIC BOOL HandleBuffer(struct SignalSemaphore *Access, TextBufferInfo **Data);
STATIC VOID DeleteBufferInfo(TextBufferInfo *BufferInfo);
STATIC TextBufferInfo *CreateBufferInfo(struct Screen *Parent, BOOL *pSearchForward, BOOL *pIgnoreCase, BOOL *pWholeWords, LONG *pTopLine);
STATIC VOID SAVE_DS BufferServer(VOID);
STATIC VOID SAVE_DS ReviewServer(VOID);

/****************************************************************************/

	/* Gadget ID codes. */

enum	{	GAD_SCROLLER,
			GAD_UP,
			GAD_DOWN
		};

	/* Menu ID codes. */

enum	{	MEN_SEARCH,
			MEN_REPEAT,
			MEN_GOTO,
			MEN_CLEARBUF_CONTENTS,
			MEN_QUITBUF,
			MEN_COPYCLIP,
			MEN_PASTECLIP,
			MEN_CLEARCLIP,
			MEN_SELECT_ALL_CLIP
		};

/****************************************************************************/

STATIC VOID
Buffer_AskPosition(struct MarkerContext *Context,LONG *Column,LONG *Line,WORD Mode)
{
	TextBufferInfo *BufferInfo;
	struct TextExtent Extent;
	LONG Left,Top;

	BufferInfo = Context->UserData;

	Left	= BufferInfo->Window->MouseX - BufferInfo->Left;
	Top		= BufferInfo->Window->MouseY - BufferInfo->Top;

	SafeObtainSemaphoreShared(&BufferSemaphore);

	if(Mode == MARKERASK_Scroll)
	{
		if(Top < 0)
		{
			*Column	= 0;
			*Line	= -1;
		}
		else if(Top >= BufferInfo->Height)
		{
			*Column	= Context->NumVisibleColumns - 1;
			*Line	= Context->NumVisibleLines;
		}
		else
		{
			LONG ThisLine;

			ThisLine = Top / BufferInfo->LocalTextFontHeight;

			if(BufferInfo->TopLine + ThisLine >= Lines)
			{
				*Column	= Context->NumVisibleColumns - 1;
				*Line	= Context->NumVisibleLines;
			}
			else
			{
				if(Left < 0)
					*Column = -1;
				else if (Left >= BufferInfo->Width)
					*Column = Context->NumVisibleColumns;
				else
				{
					STRPTR TheLine = BufferLines[BufferInfo->TopLine + ThisLine];

					if(TheLine[-1])
						*Column = TextFit(BufferInfo->RPort,TheLine,TheLine[-1],&Extent,NULL,1,Left,32767);
					else
						*Column = 0;
				}

				*Line = ThisLine;
			}
		}
	}
	else
	{
		LONG ThisLine;

		if(Top < 0)
			Top = 0;
		else if(Top >= BufferInfo->Height)
			Top = BufferInfo->Height - 1;

		if(Left < 0)
			Left = 0;
		else if (Left >= BufferInfo->Width)
			Left = BufferInfo->Width - 1;

		ThisLine = Top / BufferInfo->LocalTextFontHeight;

		if(BufferInfo->TopLine + ThisLine >= Lines)
		{
			STRPTR TheLine;

			ThisLine = Lines - BufferInfo->TopLine - 1;

			TheLine = BufferLines[BufferInfo->TopLine + ThisLine];

			if(TheLine[-1] > 0)
				*Column = TextFit(BufferInfo->RPort,TheLine,TheLine[-1],&Extent,NULL,1,Left,32767);
			else
				*Column = 0;

			*Line = ThisLine;
		}
		else
		{
			if(Left < 0)
				*Column = 0;
			else
			{
				STRPTR TheLine;

				TheLine = BufferLines[BufferInfo->TopLine + ThisLine];

				if(TheLine[-1] > 0)
					*Column = TextFit(BufferInfo->RPort,TheLine,TheLine[-1],&Extent,NULL,1,Left,32767);
				else
					*Column = 0;
			}

			*Line = ThisLine;
		}
	}

	ReleaseSemaphore(&BufferSemaphore);
}

	/* Scroll the visible region. */

STATIC VOID
Buffer_Scroll(struct MarkerContext *Context,LONG UnusedDeltaX,LONG DeltaY)
{
	TextBufferInfo *BufferInfo;
	SIPTR NewTopLine;

	BufferInfo = Context->UserData;

	NewTopLine = BufferInfo->TopLine + DeltaY;

	if(NewTopLine < 0)
		NewTopLine = 0;
	else if (NewTopLine + BufferInfo->NumBufferLines >= Lines)
		NewTopLine = Lines - BufferInfo->NumBufferLines;

	if(NewTopLine >= 0 && NewTopLine != BufferInfo->TopLine)
	{
		BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,NewTopLine);

		BufferInfo->TopLine = NewTopLine;

		SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
			PGA_Top,BufferInfo->TopLine,
		TAG_DONE);
	}
}

	/* Highlight a line of text between the "Left" and "Right" column (inclusive).
	 */

STATIC VOID
Buffer_Highlight(struct MarkerContext *Context,LONG Line,LONG Left,LONG Right)
{
	TextBufferInfo *BufferInfo;

	BufferInfo = Context->UserData;

	if(Line >= BufferInfo->TopLine && Line < BufferInfo->TopLine + BufferInfo->NumBufferLines && Line < Lines)
	{
		if(Left >= BufferInfo->NumBufferColumns)
			return;

		if(Right >= BufferInfo->NumBufferColumns)
			Right = BufferInfo->NumBufferColumns - 1;

		if(Left <= Right)
			BufferComplement(BufferInfo,BufferInfo->BufferColumnOffsets[Left],BufferInfo->BufferLineOffsets[Line - BufferInfo->TopLine],BufferInfo->BufferColumnOffsets[Right - Left + 1],BufferInfo->BufferLineOffsets[1]);
	}
}

	/* Transfer the buffer contents line by line. */

STATIC BOOL
Buffer_Put(APTR UserData,STRPTR Buffer,LONG Length)
{
	if(Length <= 0)
		return(TRUE);
	else
	{
		BOOL Result;

		if(Config->TerminalConfig->FontMode == FONT_STANDARD)
			Result = (BOOL)(WriteChunkBytes(UserData,Buffer,Length) == Length);
		else
			Result = WriteTranslatedToClip(UserData,Buffer,Length);

		return(Result);
	}
}

STATIC BOOL
Buffer_PutLine(APTR UserData,STRPTR Buffer,LONG Length)
{
	BOOL Result;

	if(Length <= 0)
		Result = TRUE;
	else
	{
		if(Config->TerminalConfig->FontMode == FONT_STANDARD)
			Result = (BOOL)(WriteChunkBytes(UserData,Buffer,Length) == Length);
		else
			Result = WriteTranslatedToClip(UserData,Buffer,Length);
	}

	if(Result)
		Result = (BOOL)(WriteChunkBytes(UserData,"\n",1) == 1);

	return(Result);
}

STATIC BOOL
Buffer_Transfer(struct MarkerContext *Context,LONG Line,LONG Left,LONG Right,MARKER_Put Put,APTR UserData)
{
	BOOL Result;

	Result = TRUE;

	SafeObtainSemaphoreShared(&RasterSemaphore);

	if(Line >= 0 && Line < Lines)
	{
		STRPTR TheLine;

		TheLine = BufferLines[Line];

		if(Left < 0)
			Left = 0;

		if(Right >= TheLine[-1])
			Right = TheLine[-1] - 1;

		if(Left <= Right)
			Result = Put(UserData,&TheLine[Left],Right - Left + 1);
		else if (TheLine[-1] == 0)
			Result = Put(UserData,&TheLine[Left],0);
	}

	ReleaseSemaphore(&RasterSemaphore);

	return(Result);
}

STATIC BOOL
Buffer_PickWord(struct MarkerContext *UnusedContext,LONG Left,LONG Top,LONG *WordLeft,LONG *WordRight)
{
	BOOL GotIt;

	GotIt = FALSE;

	SafeObtainSemaphoreShared(&BufferSemaphore);

	if(Top >= 0 && Top < Lines)
	{
		STRPTR TheLine;

		TheLine = BufferLines[Top];

		if(Left >= 0 && Left < TheLine[-1] && TheLine[Left] != ' ')
		{
			LONG From,To;

			From = To = Left;

			while(From > 0 && TheLine[From - 1] != ' ')
				From--;

			while(To < TheLine[-1] - 1 && TheLine[To + 1] != ' ')
				To++;

			GotIt = TRUE;

			*WordLeft	= From;
			*WordRight	= To;
		}
	}

	ReleaseSemaphore(&BufferSemaphore);

	return(GotIt);
}

STATIC APTR
Buffer_TransferStartStop(struct MarkerContext *Context,APTR UserData,ULONG Qualifier)
{
	struct IFFHandle *Handle;
	TextBufferInfo *BufferInfo;

	BufferInfo = Context->UserData;

	if(Handle = UserData)
	{
		if(!PopChunk(Handle))
			PopChunk(Handle);
	}
	else
	{
		APTR Buffer;
		LONG Size;

		SafeObtainSemaphoreShared(&BufferSemaphore);

		SetWait(BufferInfo->Window);

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

	ReleaseSemaphore(&BufferSemaphore);

	ClrWait(BufferInfo->Window);

	return(NULL);
}

STATIC VOID
BufferMarkerStop(TextBufferInfo *BufferInfo)
{
	if(BufferInfo->Marker)
	{
		UpMarker(BufferInfo->Marker);

		FreeVecPooled(BufferInfo->Marker);
		BufferInfo->Marker = NULL;

		ReportMouse(FALSE,BufferInfo->Window);

		ModifyIDCMP(BufferInfo->Window,BufferInfo->Window->IDCMPFlags & ~IDCMP_INTUITICKS);
	}

	OffMenu(BufferInfo->Window,FULLMENUNUM(1,0,NOSUB));
	OffMenu(BufferInfo->Window,FULLMENUNUM(1,2,NOSUB));
}

STATIC VOID
BufferSetMarker(TextBufferInfo *BufferInfo,LONG Left,LONG Top,LONG Width)
{
	BufferMarkerStop(BufferInfo);

	if(BufferInfo->Marker = CreateMarkerContext(
		Buffer_AskPosition,
		Buffer_Scroll,
		Buffer_Highlight,
		Buffer_Highlight,
		Buffer_TransferStartStop,
		Buffer_Transfer,
		Buffer_Put,
		Buffer_PutLine,
		Buffer_PickWord
	))
	{
		BufferInfo->Marker->UserData = BufferInfo;

		SetMarker(BufferInfo->Marker,BufferInfo->TopLine,BufferInfo->DisplayedLines,Lines,0,BufferInfo->NumBufferColumns,BufferInfo->NumBufferColumns,Top,Left,Left + Width - 1);

		BufferMarkerInterrupt(BufferInfo);
	}
}

STATIC VOID
BufferMarkerSelectAll(TextBufferInfo *BufferInfo)
{
	SafeObtainSemaphoreShared(&BufferSemaphore);

	if(Lines > 0)
	{
		BufferMarkerStop(BufferInfo);

		if(BufferInfo->Marker = CreateMarkerContext(
			Buffer_AskPosition,
			Buffer_Scroll,
			Buffer_Highlight,
			Buffer_Highlight,
			Buffer_TransferStartStop,
			Buffer_Transfer,
			Buffer_Put,
			Buffer_PutLine,
			Buffer_PickWord
		))
		{
			BufferInfo->Marker->UserData = BufferInfo;

			SelectAllMarker(BufferInfo->Marker,BufferInfo->TopLine,BufferInfo->DisplayedLines,Lines,0,BufferInfo->NumBufferColumns,BufferInfo->NumBufferColumns,
				0,0,
				BufferInfo->NumBufferColumns - 1,Lines - 1
			);

			BufferMarkerInterrupt(BufferInfo);
		}
	}

	ReleaseSemaphore(&BufferSemaphore);
}

STATIC VOID
BufferMarkWord(TextBufferInfo *BufferInfo)
{
	BufferMarkerStop(BufferInfo);

	if(BufferInfo->Marker = CreateMarkerContext(
		Buffer_AskPosition,
		Buffer_Scroll,
		Buffer_Highlight,
		Buffer_Highlight,
		Buffer_TransferStartStop,
		Buffer_Transfer,
		Buffer_Put,
		Buffer_PutLine,
		Buffer_PickWord
	))
	{
		BufferInfo->Marker->UserData = BufferInfo;

		SafeObtainSemaphoreShared(&BufferSemaphore);

		if(!SetWordMarker(BufferInfo->Marker,BufferInfo->TopLine,BufferInfo->DisplayedLines,Lines,0,BufferInfo->NumBufferColumns,BufferInfo->NumBufferColumns))
			BufferMarkerStop(BufferInfo);
		else
			BufferMarkerInterrupt(BufferInfo);

		ReleaseSemaphore(&BufferSemaphore);
	}
}

STATIC VOID
BufferMarkerStart(TextBufferInfo *BufferInfo,UWORD MsgQualifier)
{
	if(BufferInfo->Marker && !(MsgQualifier & SHIFT_KEY))
		BufferMarkerStop(BufferInfo);

	BufferInfo->Interrupted = FALSE;

	if(BufferInfo->Marker)
		MoveMouseMarker(BufferInfo->Marker);
	else
	{
		if(BufferInfo->Marker = CreateMarkerContext(
			Buffer_AskPosition,
			Buffer_Scroll,
			Buffer_Highlight,
			Buffer_Highlight,
			Buffer_TransferStartStop,
			Buffer_Transfer,
			Buffer_Put,
			Buffer_PutLine,
			Buffer_PickWord
		))
		{
			BufferInfo->Marker->UserData = BufferInfo;

			SafeObtainSemaphoreShared(&BufferSemaphore);

			DownMarker(BufferInfo->Marker,BufferInfo->TopLine,BufferInfo->DisplayedLines,Lines,0,BufferInfo->NumBufferColumns,BufferInfo->NumBufferColumns);

			ReleaseSemaphore(&BufferSemaphore);
		}
	}

	if(BufferInfo->Marker)
	{
		ReportMouse(TRUE,BufferInfo->Window);
		ModifyIDCMP(BufferInfo->Window,BufferInfo->Window->IDCMPFlags | IDCMP_INTUITICKS);
	}
}

STATIC VOID
BufferMarkerInterrupt(TextBufferInfo *BufferInfo)
{
	if(CheckMarker(BufferInfo->Marker))
	{
		OnMenu(BufferInfo->Window,FULLMENUNUM(1,0,NOSUB));
		OnMenu(BufferInfo->Window,FULLMENUNUM(1,2,NOSUB));
	}
	else
	{
		OffMenu(BufferInfo->Window,FULLMENUNUM(1,0,NOSUB));
		OffMenu(BufferInfo->Window,FULLMENUNUM(1,2,NOSUB));
	}

	BufferInfo->Interrupted = TRUE;

	ReportMouse(FALSE,BufferInfo->Window);
	ModifyIDCMP(BufferInfo->Window,BufferInfo->Window->IDCMPFlags & ~IDCMP_INTUITICKS);
}

STATIC VOID
BufferMarkerMoveMouse(TextBufferInfo *BufferInfo)
{
	if(BufferInfo->Marker && !BufferInfo->Interrupted)
		MoveMouseMarker(BufferInfo->Marker);
}

STATIC VOID
BufferMarkerTransfer(TextBufferInfo *BufferInfo,UWORD MsgQualifier)
{
	if(BufferInfo->Marker)
	{
		TransferMarker(BufferInfo->Marker,MsgQualifier);

		BufferMarkerStop(BufferInfo);
	}
}

/****************************************************************************/

	/* FlushMsg(struct Window *Window):
	 *
	 *	Cancel all pending messages of a window.
	 */

STATIC VOID
FlushMsg(struct Window *Window)
{
	struct Message *Message;

	while(Message = GetMsg(Window->UserPort))
		ReplyMsg(Message);
}

STATIC VOID
BufferClear(TextBufferInfo *BufferInfo,LONG Left,LONG Top,LONG Right,LONG Bottom)
{
	struct RastPort	*RPort = BufferInfo->RPort;

	Left	+= BufferInfo->Left;
	Top		+= BufferInfo->Top;
	Right	+= BufferInfo->Left;
	Bottom	+= BufferInfo->Top;

	if(Right >= BufferInfo->Left + BufferInfo->Width)
		Right = BufferInfo->Left + BufferInfo->Width - 1;

	if(Bottom >= BufferInfo->Top + BufferInfo->Height)
		Bottom = BufferInfo->Top + BufferInfo->Height - 1;

	if(Left <= Right && Top <= Bottom)
		EraseRect(RPort,Left,Top,Right,Bottom);
}

STATIC VOID
BufferScroll(TextBufferInfo *BufferInfo,LONG Lines)
{
	struct RastPort *RPort;

	if(Lines < 0)
		Lines = -BufferInfo->BufferLineOffsets[-Lines];
	else
		Lines = BufferInfo->BufferLineOffsets[Lines];

	RPort = BufferInfo->RPort;

	ScrollRaster(RPort,0,Lines,BufferInfo->Left,BufferInfo->Top,BufferInfo->Left + BufferInfo->Width - 1,BufferInfo->Top + BufferInfo->Height - 1);

	BeginUpdate(RPort->Layer);
	EndUpdate(RPort->Layer,TRUE);
}

STATIC VOID
BufferComplement(TextBufferInfo *BufferInfo,LONG SrcX,LONG SrcY,LONG Width,LONG Height)
{
	struct RastPort *RPort = BufferInfo->RPort;

	SetPens(RPort,(ULONG)~0,0,JAM1 | COMPLEMENT);

	SrcX += BufferInfo->Left;
	SrcY += BufferInfo->Top;

	FillBox(RPort,SrcX,SrcY,Width,Height);

	SetPens(RPort,BufferInfo->TextFrontPen,BufferInfo->TextBackPen,JAM2);
}

	/* DeleteScroller():
	 *
	 *	Delete scroller and arrow objects.
	 */

STATIC VOID
DeleteScroller(TextBufferInfo *BufferInfo)
{
	DisposeObject(BufferInfo->Scroller);
	DisposeObject(BufferInfo->UpArrow);
	DisposeObject(BufferInfo->DownArrow);
	DisposeObject(BufferInfo->UpImage);
	DisposeObject(BufferInfo->DownImage);
}

	/* CreateScroller(LONG Height):
	 *
	 *	Create scroller and arrow objects.
	 */

STATIC BOOL
CreateScroller(TextBufferInfo *BufferInfo,LONG Height)
{
	STATIC struct TagItem ArrowMappings[] =
	{
            { GA_ID,	GA_ID, },

            { TAG_END }
	};

	IPTR ArrowWidth,ArrowHeight;
	struct Screen *Screen;
	LONG SizeType;
	BOOL Result;

	Result = FALSE;

	if(BufferInfo->Parent)
		Screen = BufferInfo->Parent;
	else
		Screen = BufferInfo->Screen;

	if(Screen->Flags & SCREENHIRES)
		SizeType = SYSISIZE_MEDRES;
	else
		SizeType = SYSISIZE_LOWRES;

	if(!BufferInfo->Parent)
	{
		if(BufferInfo->UpImage = NewObject(NULL,SYSICLASS,
			SYSIA_Size,		SizeType,
			SYSIA_Which,	UPIMAGE,
			SYSIA_DrawInfo,	BufferInfo->BufferDrawInfo,
		TAG_DONE))
		{
			if(BufferInfo->DownImage = NewObject(NULL,SYSICLASS,
				SYSIA_Size,		SizeType,
				SYSIA_Which,	DOWNIMAGE,
				SYSIA_DrawInfo,	BufferInfo->BufferDrawInfo,
			TAG_DONE))
			{
				LONG ScrollerHeight,LeftEdge;

				GetAttr(IA_Height,	BufferInfo->UpImage,&ArrowHeight);
				GetAttr(IA_Width,	BufferInfo->UpImage,&ArrowWidth);

				ScrollerHeight = Height - 2 * ArrowHeight;

				LeftEdge = BufferInfo->Screen->Width - ArrowWidth;

				if(BufferInfo->Scroller = NewObject(NULL,PROPGCLASS,
					GA_ID,			GAD_SCROLLER,

					GA_Top,			0,
					GA_Left,		LeftEdge,
					GA_Width,		ArrowWidth,
					GA_Height,		ScrollerHeight,
					GA_Immediate,	TRUE,
					GA_FollowMouse,	TRUE,
					GA_RelVerify,	TRUE,

					PGA_Freedom,	FREEVERT,
					PGA_NewLook,	TRUE,

					PGA_Visible,	1,
					PGA_Total,		1,
				TAG_DONE))
				{
					if(BufferInfo->UpArrow = NewObject(NULL,BUTTONGCLASS,
						GA_ID,			GAD_UP,
						GA_Image,		BufferInfo->UpImage,
						GA_Left,		LeftEdge,
						GA_Top,			ScrollerHeight,
						GA_Height,		ArrowHeight,
						GA_Width,		ArrowWidth,
						GA_Previous,	BufferInfo->Scroller,

						ICA_TARGET,		ICTARGET_IDCMP,
						ICA_MAP,		ArrowMappings,
					TAG_DONE))
					{
						if(BufferInfo->DownArrow = NewObject(NULL,BUTTONGCLASS,
							GA_ID,			GAD_DOWN,
							GA_Image,		BufferInfo->DownImage,
							GA_Left,		LeftEdge,
							GA_Top,			ScrollerHeight + ArrowHeight,
							GA_Height,		ArrowHeight,
							GA_Width,		ArrowWidth,
							GA_Previous,	BufferInfo->UpArrow,

							ICA_TARGET,		ICTARGET_IDCMP,
							ICA_MAP,		ArrowMappings,
						TAG_DONE))
							Result = TRUE;
					}
				}
			}
		}
	}
	else
	{
		Object *SizeImage;

		if(SizeImage = NewObject(NULL,SYSICLASS,
			SYSIA_Size,		SizeType,
			SYSIA_Which,	SIZEIMAGE,
			SYSIA_DrawInfo,	BufferInfo->BufferDrawInfo,
		TAG_DONE))
		{
			IPTR SizeWidth,SizeHeight;

			GetAttr(IA_Width,	SizeImage,&SizeWidth);
			GetAttr(IA_Height,	SizeImage,&SizeHeight);

			DisposeObject(SizeImage);

			BufferInfo->RightBorderWidth = SizeWidth;

			if(BufferInfo->UpImage = NewObject(NULL,SYSICLASS,
				SYSIA_Size,		SizeType,
				SYSIA_Which,	UPIMAGE,
				SYSIA_DrawInfo,	BufferInfo->BufferDrawInfo,
			TAG_DONE))
			{
				GetAttr(IA_Height,BufferInfo->UpImage,&ArrowHeight);

				if(BufferInfo->DownImage = NewObject(NULL,SYSICLASS,
					SYSIA_Size,		SizeType,
					SYSIA_Which,	DOWNIMAGE,
					SYSIA_DrawInfo,	BufferInfo->BufferDrawInfo,
				TAG_DONE))
				{
					if(BufferInfo->Scroller = NewObject(NULL,PROPGCLASS,
						GA_ID,			GAD_SCROLLER,

						GA_Top,			Screen->WBorTop + Screen->Font->ta_YSize + 2,
						GA_RelHeight,	-(Screen->WBorTop + Screen->Font->ta_YSize + 2 + SizeHeight + 1 + 2 * ArrowHeight),
						GA_Width,		SizeWidth - 8,
						GA_RelRight,	-(SizeWidth - 5),

						GA_Immediate,	TRUE,
						GA_FollowMouse,	TRUE,
						GA_RelVerify,	TRUE,
						GA_RightBorder,	TRUE,

						PGA_Freedom,	FREEVERT,
						PGA_NewLook,	TRUE,
						PGA_Borderless,	TRUE,

						PGA_Visible,	1,
						PGA_Total,		1,
					TAG_DONE))
					{
						if(BufferInfo->UpArrow = NewObject(NULL,BUTTONGCLASS,
							GA_ID,			GAD_UP,

							GA_Image,		BufferInfo->UpImage,
							GA_RelRight,	-(SizeWidth - 1),
							GA_RelBottom,	-(SizeHeight - 1 + 2 * ArrowHeight),
							GA_Height,		ArrowHeight,
							GA_Width,		SizeWidth,
							GA_Previous,	BufferInfo->Scroller,
							GA_RightBorder,	TRUE,

							ICA_TARGET,		ICTARGET_IDCMP,
							ICA_MAP,		ArrowMappings,
						TAG_DONE))
						{
							if(BufferInfo->DownArrow = NewObject(NULL,BUTTONGCLASS,
								GA_ID,			GAD_DOWN,

								GA_Image,		BufferInfo->DownImage,
								GA_RelRight,	-(SizeWidth - 1),
								GA_RelBottom,	-(SizeHeight - 1 + ArrowHeight),
								GA_Height,		ArrowHeight,
								GA_Width,		SizeWidth,
								GA_Previous,	BufferInfo->UpArrow,
								GA_RightBorder,	TRUE,

								ICA_TARGET,		ICTARGET_IDCMP,
								ICA_MAP,		ArrowMappings,
							TAG_DONE))
								Result = TRUE;
						}
					}
				}
			}
		}
	}

	if(!Result)
		DeleteScroller(BufferInfo);
	else
		BufferInfo->ArrowWidth = ArrowWidth;

	return(Result);
}

	/* PrintLine(STRPTR Buffer,LONG LineNumber):
	 *
	 *	Print a line at a given line number in the displayed area.
	 */

STATIC VOID
PrintLine(TextBufferInfo *BufferInfo,STRPTR Buffer,LONG LineNumber)
{
	LONG Length;

	Length = Buffer[-1];

		/* Print the text. */

	if(Length > 0)
	{
		struct TextExtent Extent;

		Length = TextFit(BufferInfo->RPort,Buffer,Length,&Extent,NULL,1,BufferInfo->Width,32767);

		if(Length > 0)
			PlaceText(BufferInfo->RPort,BufferInfo->Left,BufferInfo->Top + BufferInfo->BufferLineOffsets[LineNumber],Buffer,Length);
	}

		/* The line doesn't exactly fill the displayed line,
		 * so erase the remaining columns.
		 */

	if(Length < BufferInfo->NumBufferColumns)
		BufferClear(BufferInfo,BufferInfo->BufferColumnOffsets[Length],BufferInfo->BufferLineOffsets[LineNumber],BufferInfo->BufferColumnOffsets[BufferInfo->NumBufferColumns] - 1,BufferInfo->BufferLineOffsets[LineNumber + 1] - 1);
}

STATIC VOID
PrintLineWithMarker(TextBufferInfo *BufferInfo,STRPTR Buffer,LONG LineNumber,LONG AbsoluteLine)
{
	LONG Length,Left,Right;

	Length = Buffer[-1];

		/* Print the text. */

	if(Length > 0)
	{
		struct TextExtent Extent;

		Length = TextFit(BufferInfo->RPort,Buffer,Length,&Extent,NULL,1,BufferInfo->Width,32767);

		if(Length > 0)
			PlaceText(BufferInfo->RPort,BufferInfo->Left,BufferInfo->Top + BufferInfo->BufferLineOffsets[LineNumber],Buffer,Length);
	}

		/* The line doesn't exactly fill the displayed line,
		 * so erase the remaining columns.
		 */

	if(Length < BufferInfo->NumBufferColumns)
		BufferClear(BufferInfo,BufferInfo->BufferColumnOffsets[Length],BufferInfo->BufferLineOffsets[LineNumber],BufferInfo->BufferColumnOffsets[BufferInfo->NumBufferColumns] - 1,BufferInfo->BufferLineOffsets[LineNumber + 1] - 1);

	if(CheckMarkerHighlighting(BufferInfo->Marker,AbsoluteLine,&Left,&Right))
	{
		if(Left < BufferInfo->NumBufferColumns)
		{
			if(Right >= BufferInfo->NumBufferColumns)
				Right = BufferInfo->NumBufferColumns - 1;

			if(Left <= Right)
				BufferComplement(BufferInfo,BufferInfo->BufferColumnOffsets[Left],BufferInfo->BufferLineOffsets[LineNumber],BufferInfo->BufferColumnOffsets[Right - Left + 1],BufferInfo->BufferLineOffsets[1]);
		}
	}
}

	/* RedrawScreen(LONG FirstLine):
	 *
	 *	Redraw the contents of the entire screen and return the
	 *	number of lines actually drawn.
	 */

STATIC LONG
RedrawScreen(TextBufferInfo *BufferInfo,LONG FirstLine)
{
	LONG i,Last,Line = 0,Result;

	SafeObtainSemaphoreShared(&BufferSemaphore);

		/* Determine last line to display. */

	if((Last = FirstLine + BufferInfo->NumBufferLines) > Lines)
	{
		Last = Lines;

		if((FirstLine = Last - BufferInfo->NumBufferLines) < 0)
			FirstLine = 0;

		BufferInfo->TopLine = FirstLine;
	}

	if(Last > FirstLine + BufferInfo->NumBufferLines)
		Last = FirstLine + BufferInfo->NumBufferLines;

	Result = Last - FirstLine;

	if(Lines)
	{
		if(BufferInfo->LastTopLine == -1)
			BufferInfo->LastTopLine = FirstLine;
		else
		{
			LONG Delta = FirstLine - BufferInfo->LastTopLine;

			if(ABS(Delta) >= BufferInfo->NumBufferLines)
				BufferInfo->LastTopLine = FirstLine;
			else
			{
					/* No change? */

				if(!Delta)
				{
					ReleaseSemaphore(&BufferSemaphore);

					return(Result);
				}
				else
				{
					BufferInfo->LastTopLine = FirstLine;

						/* Scrolled up? */

					if(Delta < 0)
					{
						BufferScroll(BufferInfo,Delta);

						Last = FirstLine - Delta;
					}
					else
					{
							/* Scrolled down. */

						BufferScroll(BufferInfo,Delta);

						FirstLine += BufferInfo->NumBufferLines - Delta;

						Line = BufferInfo->NumBufferLines - Delta;
					}
				}
			}
		}

		if(BufferLines)
		{
			if(BufferInfo->Marker)
			{
				for(i = FirstLine ; i < Last ; i++)
					PrintLineWithMarker(BufferInfo,BufferLines[i],Line++,i);
			}
			else
			{
				for(i = FirstLine ; i < Last ; i++)
					PrintLine(BufferInfo,BufferLines[i],Line++);
			}
		}
	}

		/* We didn't fill the whole screen, so clear the rest. */

	if(Result < BufferInfo->NumBufferLines)
		BufferClear(BufferInfo,0,BufferInfo->BufferLineOffsets[Result],BufferInfo->BufferColumnOffsets[BufferInfo->NumBufferColumns] - 1,BufferInfo->BufferLineOffsets[BufferInfo->NumBufferLines] - 1);

	ReleaseSemaphore(&BufferSemaphore);

	return(Result);
}

STATIC VOID
BufferDestructor(struct DataMsg *Item)
{
	Signal(Item->Client,Item->Mask);
}

STATIC VOID
BufferSerWrite(TextBufferInfo *BufferInfo,APTR Data,LONG Size)
{
	struct DataMsg Msg;

	InitMsgItem(&Msg,(DESTRUCTOR)BufferDestructor);

	Msg.Type	= DATAMSGTYPE_WRITE;
	Msg.Data	= Data;
	Msg.Size	= Size;
	Msg.Client	= FindTask(NULL);
	Msg.Mask	= 1UL << BufferInfo->BufferSignal;

	Forbid();

	ClrSignal(Msg.Mask);

	PutMsgItem(SpecialQueue,(struct MsgItem *)&Msg);

	Wait(Msg.Mask);

	Permit();
}

STATIC VOID
StartSearch(TextBufferInfo *BufferInfo,struct SearchInfo *SearchInfo,STRPTR SearchBuffer)
{
	SafeObtainSemaphoreShared(&BufferSemaphore);

	LT_LockWindow(BufferInfo->Window);

	if(Lines)
	{
		LONG LineNumber;

		LineNumber = SearchTextBuffer(SearchInfo);

		BufferMarkerStop(BufferInfo);

		if(LineNumber == -1)
		{
			ShowRequest(BufferInfo->Window,LocaleString(MSG_TERMBUFFER_DID_NOT_FIND_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),SearchBuffer);

			FlushMsg(BufferInfo->Window);

			SearchInfo->FoundY = -1;
		}
		else
		{
			if(LineNumber < BufferInfo->TopLine)
			{
				BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine = LineNumber);

				SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
					PGA_Top,BufferInfo->TopLine,
				TAG_DONE);
			}
			else
			{
				if(LineNumber > BufferInfo->TopLine + BufferInfo->DisplayedLines - 1)
				{
					if(LineNumber >= Lines - BufferInfo->NumBufferLines)
					{
						LONG NewCurrentLine;

						if((NewCurrentLine = Lines - BufferInfo->NumBufferLines) < 0)
							NewCurrentLine = 0;

						if(BufferInfo->TopLine != NewCurrentLine)
							BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine = NewCurrentLine);
					}
					else
						BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine = LineNumber);

					SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
						PGA_Top,BufferInfo->TopLine,
					TAG_DONE);
				}
			}

			BufferSetMarker(BufferInfo,SearchInfo->FoundX,LineNumber,SearchInfo->PatternWidth);
		}
	}
	else
		ShowRequest(BufferInfo->Window,LocaleString(MSG_GLOBAL_NOTHING_IN_THE_BUFFER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));

	LT_UnlockWindow(BufferInfo->Window);

	ReleaseSemaphore(&BufferSemaphore);
}

STATIC BOOL
HandleBuffer(struct SignalSemaphore *Access,TextBufferInfo **Data)
{
	ULONG LastSeconds,LastMicros,Seconds,Micros;
	BOOL WindowJustActivated,UpdatePercent,RingBack;
	ULONG SignalSet;
	struct IntuiMessage *Message;
	ULONG MsgClass;
	UWORD MsgCode,MsgQualifier,MsgGadgetID;
	UBYTE Char,FullChar = 0;
	UBYTE PercentBuffer[80],SearchBuffer[256];
	STRPTR PercentTemplate;
	struct SearchContext *Context;
	struct SearchInfo *SearchInfo;
	struct Hook HistoryHook;
	TextBufferInfo *BufferInfo;
	BOOL DisplayDirty,DoNotUpdateScroller;
	LONG NewTopLine;
	ULONG ClickSeconds,ClickMicros;
	BOOL TestingDoubleClick;

	TestingDoubleClick	= FALSE;
	ClickSeconds		= 0;
	ClickMicros			= 0;

	LastSeconds			= 0;
	LastMicros			= 0;
	UpdatePercent		= TRUE;
	RingBack 			= FALSE;

	Context				= NULL;
	SearchInfo			= NULL;

	DoNotUpdateScroller	= FALSE;
	DisplayDirty		= FALSE;
	NewTopLine			= 0;

	BufferInfo = *Data;

	if(LocaleBase)
		PercentTemplate = "%lD/%lD (%ld%%)";
	else
		PercentTemplate = "%ld/%ld (%ld%%)";

	while(TRUE)
	{
		if(DisplayDirty)
		{
			DisplayDirty = FALSE;

			if(NewTopLine < 0)
				NewTopLine = 0;
			else if (NewTopLine + BufferInfo->NumBufferLines >= Lines)
				NewTopLine = Lines - BufferInfo->NumBufferLines;

			if(NewTopLine >= 0 && NewTopLine != BufferInfo->TopLine)
			{
				BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,NewTopLine);

				BufferInfo->TopLine = NewTopLine;

				if(DoNotUpdateScroller)
					DoNotUpdateScroller	= FALSE;
				else
				{
					SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
						PGA_Top,BufferInfo->TopLine,
					TAG_DONE);
				}

				UpdatePercent = TRUE;
			}
		}

		if(BufferInfo->TopLine < 0)
			BufferInfo->TopLine = 0;

			/* Show where we are. */

		if(UpdatePercent)
		{
			STRPTR ScreenTitle,WindowTitle;

			SafeObtainSemaphoreShared(&BufferSemaphore);

			if(Lines > 0)
				LimitedSPrintf(sizeof(PercentBuffer),PercentBuffer,PercentTemplate,BufferInfo->TopLine,MAX(0,Lines - BufferInfo->NumBufferLines),(100 * (BufferInfo->TopLine + BufferInfo->DisplayedLines)) / Lines);
			else
				PercentBuffer[0] = 0;

			LimitedSPrintf(sizeof(BufferInfo->TitleBuffer),BufferInfo->TitleBuffer,"%s  %s",LocaleString(MSG_TERMBUFFER_TERM_BUFFER_TXT),PercentBuffer);

			if(BufferInfo->Parent)
			{
				WindowTitle	= BufferInfo->TitleBuffer;
				ScreenTitle = (STRPTR)~0;
			}
			else
				ScreenTitle = WindowTitle = BufferInfo->TitleBuffer;

			SetWindowTitles(BufferInfo->Window,WindowTitle,ScreenTitle);

			UpdatePercent = FALSE;

			ReleaseSemaphore(&BufferSemaphore);
		}

		if(!BufferInfo->BufferTerminated)
			SignalSet = Wait(SIG_KILL | SIG_TOFRONT | SIG_UPDATE | SIG_MOVEUP | PORTMASK(BufferInfo->Window->UserPort) | BufferInfo->QueueMask);
		else
		{
			ObtainSemaphore(Access);

			SignalSet = SIG_KILL | SIG_TOFRONT | SIG_UPDATE | SIG_MOVEUP | PORTMASK(BufferInfo->Window->UserPort) | BufferInfo->QueueMask;
			SignalSet = SetSignal(0,SignalSet) & SignalSet;

				/* Disconnect */

			if(!SignalSet)
			{
				BufferInfo->Buddy = NULL;

				*Data = NULL;

				ReleaseSemaphore(Access);

				break;
			}
			else
				ReleaseSemaphore(Access);
		}

			/* Leave this town? */

		if(SignalSet & SIG_KILL)
			BufferInfo->BufferTerminated = RingBack = TRUE;

			/* Bring our window to the front. */

		if(SignalSet & SIG_TOFRONT)
		{
			if(Context)
				LT_ShowWindow(Context->SearchHandle,TRUE);

			BumpWindow(BufferInfo->Window);
		}

			/* Redraw the entire screen. */

		if(SignalSet & SIG_MOVEUP)
		{
			BufferMarkerStop(BufferInfo);

			BufferInfo->LastTopLine = -1;

			SafeObtainSemaphoreShared(&BufferSemaphore);

			BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine);

			SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
				PGA_Total,Lines,
			TAG_DONE);

			ReleaseSemaphore(&BufferSemaphore);

			UpdatePercent = TRUE;

			Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
		}

			/* Redraw the entire screen unless it is already drawn. */

		if(SignalSet & SIG_UPDATE)
		{
			SafeObtainSemaphoreShared(&BufferSemaphore);

			if(BufferInfo->DisplayedLines < BufferInfo->NumBufferLines || !Lines)
			{
				BufferMarkerStop(BufferInfo);

				BufferInfo->LastTopLine = -1;

				BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine);
			}

			SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
				PGA_Total,Lines,
			TAG_DONE);

			ReleaseSemaphore(&BufferSemaphore);

			UpdatePercent = TRUE;

			Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
		}

			/* A request from the queue? */

		if(SignalSet & BufferInfo->QueueMask)
		{
			struct DataMsg *Msg;

			while(Msg = (struct DataMsg *)GetMsgItem(BufferInfo->Queue))
			{
				switch(Msg->Size)
				{
					case REVIEW_MOVE_TOP:

						DisplayDirty	= TRUE;
						NewTopLine		= 0;

						break;

					case REVIEW_MOVE_BOTTOM:

						DisplayDirty	= TRUE;
						NewTopLine		= Lines - BufferInfo->NumBufferLines;

						break;

					case REVIEW_MOVE_UP:

						DisplayDirty	= TRUE;
						NewTopLine		= BufferInfo->TopLine - (BufferInfo->NumBufferLines - 1);

						break;

					case REVIEW_MOVE_DOWN:

						DisplayDirty	= TRUE;
						NewTopLine		= BufferInfo->TopLine + (BufferInfo->NumBufferLines - 1);

						break;
				}

				DeleteMsgItem((struct MsgItem *)Msg);
			}
		}

			/* Process the incoming window
			 * input.
			 */

		while(Message = (struct IntuiMessage *)GetMsg(BufferInfo->Window->UserPort))
		{
			if(Context && Context->SearchWindow == Message->IDCMPWindow)
			{
				MsgClass = 0;

				if(HandleSearchMessage(Context,&Message))
				{
					BOOL Ok = Context->Ok;

					DeleteSearchContext(Context);

					Context = NULL;

					if(Ok)
					{
						if(SearchInfo)
							DeleteSearchInfo(SearchInfo);

						if(SearchInfo = CreateSearchInfo(SearchBuffer,BufferInfo->SearchForward,BufferInfo->IgnoreCase,BufferInfo->WholeWords))
							StartSearch(BufferInfo,SearchInfo,SearchBuffer);
					}
					else
					{
						if(SearchInfo)
							DeleteSearchInfo(SearchInfo);

						SearchInfo = NULL;
					}
				}
			}
			else
			{
				MsgClass		= Message->Class;
				MsgCode			= Message->Code;
				MsgQualifier	= Message->Qualifier;
				Seconds			= Message->Seconds;
				Micros			= Message->Micros;

				WindowJustActivated	= FALSE;
				MsgGadgetID			= 0;

				switch(MsgClass)
				{
					case IDCMP_SIZEVERIFY:

						BufferMarkerStop(BufferInfo);
						break;

					case IDCMP_IDCMPUPDATE:

						MsgGadgetID = (UWORD)GetTagData(GA_ID,0,(struct TagItem *)Message->IAddress);
						break;

					case IDCMP_GADGETUP:
					case IDCMP_GADGETDOWN:

						MsgGadgetID = ((struct Gadget *)Message->IAddress)->GadgetID;
						break;

					case IDCMP_RAWKEY:

						FullChar = ConvertTheKey(NULL,0,MsgCode,MsgQualifier,*(ULONG *)Message->IAddress);

						if(!(MsgQualifier & IEQUALIFIER_NUMERICPAD))
							Char = FullChar;
						else
							Char = ConvertTheKey(NULL,0,MsgCode,MsgQualifier & ~(CONTROL_KEY | ALT_KEY | SHIFT_KEY),*(ULONG *)Message->IAddress);

						break;

					case IDCMP_ACTIVEWINDOW:
					case IDCMP_MOUSEBUTTONS:

						WindowJustActivated = (BOOL)(Seconds == LastSeconds && Micros == LastMicros);
						break;
				}

				LastSeconds	= Seconds;
				LastMicros	= Micros;

				ReplyMsg((struct Message *)Message);
			}

			if(MsgClass == IDCMP_INACTIVEWINDOW)
				BufferMarkerInterrupt(BufferInfo);

			if(MsgClass == IDCMP_IDCMPUPDATE)
			{
				switch(MsgGadgetID)
				{
					case GAD_UP:

						DisplayDirty	= TRUE;
						NewTopLine		= BufferInfo->TopLine - 1;

						break;

					case GAD_DOWN:

						DisplayDirty	= TRUE;
						NewTopLine		= BufferInfo->TopLine + 1;

						break;
				}
			}

			if(MsgClass == IDCMP_MOUSEMOVE || MsgClass == IDCMP_INTUITICKS)
				BufferMarkerMoveMouse(BufferInfo);

			if(MsgClass == IDCMP_MOUSEMOVE || ((MsgClass == IDCMP_GADGETDOWN || MsgClass == IDCMP_GADGETUP) && MsgGadgetID == GAD_SCROLLER))
			{
				GetAttr(PGA_Top,BufferInfo->Scroller,(IPTR *)&NewTopLine);

				DisplayDirty		= TRUE;
				DoNotUpdateScroller	= TRUE;
			}

			if(MsgClass == IDCMP_MOUSEBUTTONS)
			{
				if(!WindowJustActivated || MsgCode != SELECTDOWN)
				{
					switch(MsgCode)
					{
						case SELECTUP:

							BufferMarkerInterrupt(BufferInfo);
							break;

						case SELECTDOWN:

							if(TestingDoubleClick)
							{
								ULONG CurrentSeconds,CurrentMicros;

								TestingDoubleClick = FALSE;

								CurrentTime(&CurrentSeconds,&CurrentMicros);

								if(DoubleClick(ClickSeconds,ClickMicros,CurrentSeconds,CurrentMicros))
								{
									BufferMarkWord(BufferInfo);

									break;
								}
							}
							else
							{
								TestingDoubleClick = TRUE;

								CurrentTime(&ClickSeconds,&ClickMicros);
							}

							BufferMarkerStart(BufferInfo,MsgQualifier);
							break;
					}
				}
			}

			if(MsgClass == IDCMP_RAWKEY)
			{
				switch(MsgCode)
				{
					case HELP_CODE:

						GuideDisplay(CONTEXT_TEXTBUFFER);
						break;

						/* Scroll the buffer up. */

					case CURSOR_UP_CODE:

						DisplayDirty = TRUE;

						if(MsgQualifier & (CONTROL_KEY | ALT_KEY))
						{
							NewTopLine = 0;

							break;
						}

						if(MsgQualifier & SHIFT_KEY)
						{
							NewTopLine = BufferInfo->TopLine - (BufferInfo->NumBufferLines - 1);

							break;
						}

						NewTopLine = BufferInfo->TopLine - 1;
						break;

						/* Scroll the buffer down. */

					case CURSOR_DOWN_CODE:

						DisplayDirty = TRUE;

						if(MsgQualifier & (CONTROL_KEY | ALT_KEY))
						{
							NewTopLine = Lines - BufferInfo->NumBufferLines;

							break;
						}

						if(MsgQualifier & SHIFT_KEY)
						{
							NewTopLine = BufferInfo->TopLine + (BufferInfo->NumBufferLines - 1);

							break;
						}

						NewTopLine = BufferInfo->TopLine + 1;
						break;

					default:

							/* Use the numeric keypad keys to
							 * move through the buffer.
							 */

						if(!(MsgQualifier & IEQUALIFIER_NUMERICPAD))
							Char = FullChar;
						else
						{
							switch(Char - '0')
							{
									/* Jump to bottom. */

								case 1:

									DisplayDirty	= TRUE;
									NewTopLine		= Lines - BufferInfo->NumBufferLines;

									break;

									/* Move one line down. */

								case 2:

									DisplayDirty	= TRUE;
									NewTopLine		= BufferInfo->TopLine + 1;

									break;

									/* Move one page down. */

								case 3:

									DisplayDirty	= TRUE;
									NewTopLine		= BufferInfo->TopLine + (BufferInfo->NumBufferLines - 1);

									break;

									/* Jump to top. */

								case 7:

									DisplayDirty	= TRUE;
									NewTopLine		= 0;

									break;

									/* Move one line up. */

								case 8:

									DisplayDirty	= TRUE;
									NewTopLine		= BufferInfo->TopLine - 1;

									break;

									/* Move one page up. */

								case 9:

									DisplayDirty	= TRUE;
									NewTopLine		= BufferInfo->TopLine - (BufferInfo->NumBufferLines - 1);

									break;
							}

							if(Char >= '0' && Char <= '9')
								Char = 0;
							else
								Char = FullChar;
						}

						if(Char > 0 && BufferInfo->Parent)
						{
							if(Config->SerialConfig->StripBit8)
								Char &= 0x7F;

							if(Get_xOFF())
							{
								if(Char == XON)
									Clear_xOFF();
							}
							else
							{
								STRPTR String;
								LONG Length;

								String = &Char;
								Length = 1;

									/* Convert chars as appropriate */

								switch(Char)
								{
									case '\n':

										switch(Config->TerminalConfig->SendLF)
										{
											case EOL_LF:

												break;

											case EOL_LFCR:

												String = "\n\r";
												Length = 2;
												break;

											case EOL_CRLF:

												String = "\r\n";
												Length = 2;
												break;

											case EOL_CR:

												String = "\r";
												break;
										}

										break;

									case '\r':

										switch(Config->TerminalConfig->SendCR)
										{
											case EOL_CR:

												break;

											case EOL_LFCR:

												String = "\n\r";
												Length = 2;
												break;

											case EOL_CRLF:

												String = "\r\n";
												Length = 2;
												break;

											case EOL_LF:

												String = "\n";
												break;
										}

										break;

										/* Restart in/output. */

									case XON:

										if(Config->SerialConfig->xONxOFF)
											Clear_xOFF();

										if(!Config->SerialConfig->PassThrough)
											Length = 0;

										break;

										/* Stop in/output. */

									case XOF:

										if(Config->SerialConfig->xONxOFF)
											Set_xOFF();

										if(!Config->SerialConfig->PassThrough)
											Length = 0;

										break;

										/* Convert special Amiga characters into
										 * alien IBM dialect.
										 */

									default:

										if(Config->TerminalConfig->FontMode == FONT_IBM)
										{
											if(IBMConversion[Char])
												Char = IBMConversion[Char];
											else
												Length = 0;
										}

										break;
								}

								if(Length > 0)
									BufferSerWrite(BufferInfo,String,Length);
							}
						}

						break;
				}
			}

			if(MsgClass == IDCMP_MENUHELP)
				GuideDisplay(CONTEXT_BUFFER_MENU);

			if(MsgClass == IDCMP_CLOSEWINDOW)
				BufferInfo->BufferTerminated = TRUE;

			if(MsgClass == IDCMP_NEWSIZE)
			{
				LONG Width,Height;

				BufferInfo->Width				= BufferInfo->Window->Width		- (BufferInfo->Window->BorderLeft	+ BufferInfo->Window->BorderRight);
				BufferInfo->Height				= BufferInfo->Window->Height	- (BufferInfo->Window->BorderTop	+ BufferInfo->Window->BorderBottom);

				Width							= BufferInfo->Width		- (BufferInfo->Width	% BufferInfo->LocalTextFontWidth);
				Height							= BufferInfo->Height	- (BufferInfo->Height	% BufferInfo->LocalTextFontHeight);

				BufferInfo->NumBufferColumns	= Width / BufferInfo->LocalTextFontWidth;
				BufferInfo->NumBufferLines		= Height / BufferInfo->LocalTextFontHeight;

				if(Width < BufferInfo->Width)
					BufferClear(BufferInfo,Width,0,BufferInfo->Width - 1,BufferInfo->Height - 1);

				if(Height < BufferInfo->Height)
					BufferClear(BufferInfo,0,Height,BufferInfo->Width - 1,BufferInfo->Height - 1);

				BufferInfo->Width	= Width;
				BufferInfo->Height	= Height;

				BufferInfo->LastTopLine = -1;
				BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine);

				SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
					PGA_Total,		Lines,
					PGA_Top,		BufferInfo->TopLine,
					PGA_Visible,	BufferInfo->DisplayedLines,
				TAG_DONE);

				UpdatePercent = TRUE;
			}

			if(MsgClass == IDCMP_MENUPICK)
			{
				struct MenuItem	*MenuItem;
				struct DataMsg	 Msg;

				while(MsgCode != MENUNULL)
				{
					if(!(MenuItem = ItemAddress(BufferInfo->BufferMenuStrip,MsgCode)))
						break;
					else
					{
						switch((ULONG)(IPTR)GTMENUITEM_USERDATA(MenuItem))
						{
							case MEN_COPYCLIP:

								BufferMarkerTransfer(BufferInfo,MsgQualifier);
								break;

							case MEN_CLEARCLIP:

								BufferMarkerStop(BufferInfo);
								break;

							case MEN_SELECT_ALL_CLIP:

								BufferMarkerSelectAll(BufferInfo);
								break;

							case MEN_PASTECLIP:

								InitMsgItem(&Msg,(DESTRUCTOR)BufferDestructor);

								Msg.Type	= DATAMSGTYPE_WRITECLIP;
								Msg.Size	= Config->ClipConfig->ClipboardUnit;
								Msg.Client	= FindTask(NULL);
								Msg.Mask	= 1UL << BufferInfo->BufferSignal;

								Forbid();

								ClrSignal(Msg.Mask);

								PutMsgItem(SpecialQueue,(struct MsgItem *)&Msg);

								Wait(Msg.Mask);

								Permit();
								break;

							case MEN_SEARCH:

								if(Context)
									LT_ShowWindow(Context->SearchHandle,TRUE);
								else
									Context = CreateSearchContext(BufferInfo->Window,SearchBuffer,&HistoryHook,(struct List *)&TextBufferHistory,&BufferInfo->SearchForward,&BufferInfo->IgnoreCase,&BufferInfo->WholeWords);

								UpdatePercent = TRUE;

								break;

							case MEN_REPEAT:

								if(Context)
									LT_ShowWindow(Context->SearchHandle,TRUE);
								else
								{
									if(SearchInfo)
										StartSearch(BufferInfo,SearchInfo,SearchBuffer);
									else
										Context = CreateSearchContext(BufferInfo->Window,SearchBuffer,&HistoryHook,(struct List *)&TextBufferHistory,&BufferInfo->SearchForward,&BufferInfo->IgnoreCase,&BufferInfo->WholeWords);
								}

								UpdatePercent = TRUE;

								break;

							case MEN_GOTO:

								if(Window)
									BumpWindow(Window);

								break;

							case MEN_QUITBUF:

								BufferInfo->BufferTerminated = TRUE;
								break;

							case MEN_CLEARBUF_CONTENTS:

								if(Lines)
								{
									struct DataMsg *Msg;

									if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg)))
									{
										Msg->Type = DATAMSGTYPE_CLEARBUFFER;

										UpdatePercent = TRUE;

										LT_LockWindow(BufferInfo->Window);

										if(MsgQualifier & SHIFT_KEY)
										{
											PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
											Msg = NULL;
										}
										else
										{
											if(Config->MiscConfig->ProtectiveMode)
											{
												if(!ShowRequest(BufferInfo->Window,LocaleString(MSG_TERMBUFFER_BUFFER_STILL_HOLDS_LINES_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Lines))
												{
													LT_UnlockWindow(BufferInfo->Window);

													break;
												}
											}

											PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
											Msg = NULL;
										}

										DeleteMsgItem((struct MsgItem *)Msg);
									}

									LT_UnlockWindow(BufferInfo->Window);

									FlushMsg(BufferInfo->Window);
								}

								break;
						}

						MsgCode = MenuItem->NextSelect;
					}
				}
			}

			if(DisplayDirty)
			{
				DisplayDirty = FALSE;

				if(NewTopLine < 0)
					NewTopLine = 0;
				else if (NewTopLine + BufferInfo->NumBufferLines >= Lines)
					NewTopLine = Lines - BufferInfo->NumBufferLines;

				if(NewTopLine >= 0 && NewTopLine != BufferInfo->TopLine)
				{
					BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,NewTopLine);

					BufferInfo->TopLine = NewTopLine;

					if(DoNotUpdateScroller)
						DoNotUpdateScroller = FALSE;
					else
					{
						SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
							PGA_Top,BufferInfo->TopLine,
						TAG_DONE);
					}

					UpdatePercent = TRUE;
				}
			}
		}
	}

	if(Context)
		DeleteSearchContext(Context);

	if(SearchInfo)
		DeleteSearchInfo(SearchInfo);

		/* Put the stuff back */

	*BufferInfo->pTopLine		= BufferInfo->TopLine;
	*BufferInfo->pSearchForward	= BufferInfo->SearchForward;
	*BufferInfo->pIgnoreCase	= BufferInfo->IgnoreCase;
	*BufferInfo->pWholeWords	= BufferInfo->WholeWords;

	return(RingBack);
}

STATIC VOID
DeleteBufferInfo(TextBufferInfo *BufferInfo)
{
	if(BufferInfo)
	{
		FreeVecPooled(BufferInfo->BufferLineOffsets);

		if(BufferInfo->Window)
		{
			ClearMenuStrip(BufferInfo->Window);

			LT_DeleteWindowLock(BufferInfo->Window);

			CloseWindow(BufferInfo->Window);
		}

		FreeVecPooled(BufferInfo->Marker);

		DeleteScroller(BufferInfo);

		DisposeObject(BufferInfo->BufferAmigaGlyph);
		DisposeObject(BufferInfo->BufferCheckGlyph);

		FreeScreenDrawInfo(BufferInfo->Screen,BufferInfo->BufferDrawInfo);

		LT_DisposeMenu(BufferInfo->BufferMenuStrip);

		if(BufferInfo->Screen && !BufferInfo->Parent)
			CloseScreen(BufferInfo->Screen);

		if(BufferInfo->LocalFont)
			CloseFont(BufferInfo->LocalFont);

		if(BufferInfo->BufferSignal != -1)
			FreeSignal(BufferInfo->BufferSignal);

		DeleteMsgQueue(BufferInfo->Queue);

		FreeVecPooled(BufferInfo);
	}
}

STATIC TextBufferInfo *
CreateBufferInfo(struct Screen *Parent,BOOL *pSearchForward,BOOL *pIgnoreCase,BOOL *pWholeWords,LONG *pTopLine)
{
	TextBufferInfo *BufferInfo;

	if(BufferInfo = (TextBufferInfo *)AllocVecPooled(sizeof(TextBufferInfo),MEMF_ANY | MEMF_CLEAR))
	{
		LONG						 Width,Height;
		ULONG						 DisplayMode;

		struct Rectangle			 DisplayClip;
		struct DimensionInfo		 DimensionInfo;

		BufferInfo->pSearchForward	= pSearchForward;
		BufferInfo->pIgnoreCase		= pIgnoreCase;
		BufferInfo->pWholeWords		= pWholeWords;
		BufferInfo->pTopLine		= pTopLine;

		BufferInfo->TopLine			= *pTopLine;
		BufferInfo->LastTopLine		= -1;

		BufferInfo->SearchForward	= *pSearchForward;
		BufferInfo->IgnoreCase		= *pIgnoreCase;
		BufferInfo->WholeWords		= *pWholeWords;

		BufferInfo->OldColumn		= -1;
		BufferInfo->OldLine			= -1;
		BufferInfo->OldLength		= -1;

		BufferInfo->Buddy			= (struct Process *)FindTask(NULL);

		CopyMem(&TextFont,&BufferInfo->LocalTextFont,sizeof(struct TTextAttr));

		BufferInfo->LocalTextFont.tta_Name	= BufferInfo->LocalTextFontName;
		BufferInfo->LocalTextFont.tta_YSize	= Config->TerminalConfig->TextFontHeight;

		strcpy(BufferInfo->LocalTextFontName,Config->TerminalConfig->TextFontName);

		CopyMem(&UserFont,&BufferInfo->LocalUserFont,sizeof(struct TTextAttr));

		BufferInfo->LocalUserFont.tta_Name = BufferInfo->LocalUserFontName;

		strcpy(BufferInfo->LocalUserFontName,UserFont.tta_Name);

		if(!Config->CaptureConfig->ConvertChars && Config->TerminalConfig->FontMode != FONT_STANDARD)
		{
			strcpy(BufferInfo->LocalTextFontName,Config->TerminalConfig->IBMFontName);

			BufferInfo->LocalTextFont.tta_YSize = Config->TerminalConfig->IBMFontHeight;

			BufferInfo->NeedClipConversion = TRUE;
		}
		else
			BufferInfo->NeedClipConversion = FALSE;

		if(BufferInfo->LocalFont = SmartOpenDiskFont((struct TextAttr *)&BufferInfo->LocalTextFont))
		{
			BufferInfo->LocalTextFontWidth	= BufferInfo->LocalFont->tf_XSize;
			BufferInfo->LocalTextFontHeight	= BufferInfo->LocalFont->tf_YSize;

			DisplayMode = Config->CaptureConfig->BufferScreenMode;

			if(DisplayMode == INVALID_ID && Window)
				DisplayMode = GetVPModeID(&Window->WScreen->ViewPort);

			if(ModeNotAvailable(DisplayMode))
				DisplayMode = Config->ScreenConfig->DisplayMode;

			if(ModeNotAvailable(DisplayMode))
			{
				struct Screen *PubScreen = LockPubScreen(NULL);

				if(PubScreen)
				{
					DisplayMode = GetVPModeID(&PubScreen->ViewPort);

					UnlockPubScreen(NULL,PubScreen);
				}
				else
					DisplayMode = DEFAULT_MONITOR_ID | HIRES_KEY;
			}

				/* Set up the actual width of the screen we want. */

			Width = Config->CaptureConfig->BufferWidth * BufferInfo->LocalTextFontWidth + BufferInfo->ArrowWidth + 1;

			if((BufferInfo->BufferSignal = AllocSignal(-1)) != -1)
			{
					/* Get the mode dimension info. */

				if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,DisplayMode))
				{
						/* Determine maximum text overscan width. */

					LONG TextWidth = DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1;

						/* Too small? */

					if(Width < DimensionInfo.MinRasterWidth)
						Width = DimensionInfo.MinRasterWidth;

						/* Far too large? */

					if(Width > DimensionInfo.MaxRasterWidth)
						Width = DimensionInfo.MaxRasterWidth;

						/* A bit too large? */

					if(Width > TextWidth)
						Width = TextWidth;

						/* Inquire the text overscan dimensions. */

					if(QueryOverscan(DisplayMode,&DisplayClip,OSCAN_TEXT))
					{
							/* Centre the buffer screen. */

						if(DisplayClip.MaxX - DisplayClip.MinX + 1 > Width)
						{
							LONG Differ = (DisplayClip.MaxX - DisplayClip.MinX + 1 - Width) / 2;

							switch(Config->CaptureConfig->BufferScreenPosition)
							{
								case SCREEN_LEFT:

									DisplayClip.MaxX = DisplayClip.MinX + Width;
									break;

								case SCREEN_RIGHT:

									DisplayClip.MinX = DisplayClip.MaxX - Width;
									break;

								case SCREEN_CENTRE:

									DisplayClip.MinX += Differ;
									DisplayClip.MaxX -= Differ;

									break;
							}
						}

							/* Open a two bitplane clone of the main screen. */

						if(Parent)
						{
							BufferInfo->Parent = Parent;
							BufferInfo->Screen = Parent;

							if(BufferInfo->Queue = CreateMsgQueue(0,0))
								BufferInfo->QueueMask = BufferInfo->Queue->SigMask;
							else
								BufferInfo->Screen = NULL;
						}
						else
						{
							UWORD Pens = (UWORD)~0;

							BufferInfo->Screen = OpenScreenTags(NULL,
								SA_Title,		LocaleString(MSG_TERMBUFFER_TERM_BUFFER_TXT),
								SA_Depth,		2,
								SA_Left,		DisplayClip.MinX,
								SA_DClip,		&DisplayClip,
								SA_DisplayID,	DisplayMode,
								SA_Font,		&BufferInfo->LocalUserFont,
								SA_AutoScroll,	TRUE,
								SA_Pens,		&Pens,
							TAG_END);
						}

						if(BufferInfo->Screen)
						{
							if(BufferInfo->BufferDrawInfo = GetScreenDrawInfo(BufferInfo->Screen))
							{
								IPTR *MenuTags;

								CreateMenuGlyphs(BufferInfo->Screen,BufferInfo->BufferDrawInfo,&BufferInfo->BufferAmigaGlyph,&BufferInfo->BufferCheckGlyph);

								if(Parent)
								{
									char STR_COLON[] = ":";
									STATIC IPTR ReviewTags[] =
									{
										LAMN_TitleID,			MSG_TERMREVIEW_PROJECT_MEN,
											LAMN_ItemID,		MSG_TERMREVIEW_SEARCH_MEN,
												LAMN_UserData,	MEN_SEARCH,
											LAMN_ItemID,		MSG_TERMREVIEW_REPEAT_SEARCH_MEN,
												LAMN_UserData,	MEN_REPEAT,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_TERMREVIEW_CLEAR_BUFFER_MEN,
												LAMN_UserData,	MEN_CLEARBUF_CONTENTS,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_TERMREVIEW_CLOSE_BUFFER_MEN,
												LAMN_UserData,	MEN_QUITBUF,

											LAMN_ItemID,		MSG_CLOSE_WINDOW_TXT,
												LAMN_KeyText,	0L,
												LAMN_UserData,	MEN_QUITBUF,

										LAMN_TitleID,			MSG_TERMBUFFER_EDIT_MEN,
											LAMN_ItemID,		MSG_TERMDATA_COPY_MEN,
												LAMN_UserData,	MEN_COPYCLIP,
											LAMN_ItemID,		MSG_TERMDATA_PASTE_MEN,
												LAMN_UserData,	MEN_PASTECLIP,
											LAMN_ItemID,		MSG_TERMDATA_CLEAR_MEN,
												LAMN_UserData,	MEN_CLEARCLIP,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_SELECT_ALL_MEN,
												LAMN_UserData,	MEN_SELECT_ALL_CLIP,

										TAG_DONE
									};
									ReviewTags[25] = (IPTR)STR_COLON;

									MenuTags = ReviewTags;
								}
								else
								{
									STATIC IPTR BufferTags[] =
									{
										LAMN_TitleID,			MSG_TERMBUFFER_PROJECT_MEN,
											LAMN_ItemID,		MSG_TERMBUFFER_SEARCH_MEN,
												LAMN_UserData,	MEN_SEARCH,
											LAMN_ItemID,		MSG_TERMBUFFER_REPEAT_SEARCH_MEN,
												LAMN_UserData,	MEN_REPEAT,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_TERMBUFFER_GO_TO_MAIN_SCREEN_MEN,
												LAMN_UserData,	MEN_GOTO,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_TERMBUFFER_CLEAR_BUFFER_MEN,
												LAMN_UserData,	MEN_CLEARBUF_CONTENTS,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_TERMBUFFER_CLOSE_BUFFER_MEN,
												LAMN_UserData,	MEN_QUITBUF,

										LAMN_TitleID,			MSG_TERMBUFFER_EDIT_MEN,
											LAMN_ItemID,		MSG_TERMDATA_COPY_MEN,
												LAMN_UserData,	MEN_COPYCLIP,
											LAMN_ItemID,		MSG_TERMDATA_PASTE_MEN,
												LAMN_UserData,	MEN_PASTECLIP,
											LAMN_ItemID,		MSG_TERMDATA_CLEAR_MEN,
												LAMN_UserData,	MEN_CLEARCLIP,

											LAMN_ItemText,		(IPTR)NM_BARLABEL,

											LAMN_ItemID,		MSG_SELECT_ALL_MEN,
												LAMN_UserData,	MEN_SELECT_ALL_CLIP,

										TAG_DONE
									};

									MenuTags = BufferTags;
								}

								if(BufferInfo->BufferMenuStrip = LT_NewMenuTags(
									LAHN_LocaleHook,		&LocaleHook,
									LAMN_Screen,			BufferInfo->Screen,
									LAMN_TextAttr,			&BufferInfo->LocalUserFont,
									LAMN_AmigaGlyph,		BufferInfo->BufferAmigaGlyph,
									LAMN_CheckmarkGlyph,	BufferInfo->BufferCheckGlyph,
								TAG_MORE,MenuTags))
								{
									UWORD *Pens = BufferInfo->BufferDrawInfo->dri_Pens;

									BufferInfo->TextFrontPen	= Pens[TEXTPEN];
									BufferInfo->TextBackPen		= Pens[BACKGROUNDPEN];

									BufferInfo->MaxPen = MAX(BufferInfo->TextFrontPen,BufferInfo->TextBackPen);

									Height = BufferInfo->Screen->Height - (BufferInfo->Screen->BarHeight + 2);

									if(CreateScroller(BufferInfo,Height))
									{
										LONG Left,Top;

										if(Parent)
										{
											Left	= Window->LeftEdge;
											Top		= Window->TopEdge + Window->BorderTop;
											Width	= Window->Width;
											Height	= Window->Height - Window->BorderTop;

											GetWindowInfo(WINDOW_REVIEW,&Left,&Top,&Width,&Height,Width,Height);
										}
										else
										{
											Left	= 0;
											Top		= BufferInfo->Screen->BarHeight + 2;
											Width	= BufferInfo->Screen->Width;
											Height	= BufferInfo->Screen->Height - Top;
										}

										if(BufferInfo->Window = OpenWindowTags(NULL,
											WA_Left,			Left,
											WA_Top,				Top,
											WA_Width,			Width,
											WA_Height,			Height,
											WA_Backdrop,		Parent == NULL,
											WA_Borderless,		Parent == NULL,
											WA_DepthGadget,		Parent != NULL,
											WA_DragBar,			Parent != NULL,
											WA_CloseGadget,		Parent != NULL,
											WA_SizeGadget,		Parent != NULL,
											WA_NoCareRefresh,	TRUE,
											WA_NewLookMenus,	TRUE,
											WA_IDCMP,			IDCMP_IDCMPUPDATE | IDCMP_RAWKEY | IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE | IDCMP_MENUHELP | IDCMP_SIZEVERIFY | IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW,
											WA_MenuHelp,		TRUE,
											WA_Gadgets,			BufferInfo->Scroller,
											WA_CustomScreen,	BufferInfo->Screen,
											WA_Activate,		TRUE,
											BackfillTag,		Parent ? &BackfillHook : NULL,

											BufferInfo->BufferAmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, BufferInfo->BufferAmigaGlyph,
											BufferInfo->BufferCheckGlyph ? WA_Checkmark : TAG_IGNORE, BufferInfo->BufferCheckGlyph,
										TAG_DONE))
										{
											LONG MaxLines,MaxColumns;

											SetMenuStrip(BufferInfo->Window,BufferInfo->BufferMenuStrip);

											OffMenu(BufferInfo->Window,FULLMENUNUM(1,0,NOSUB));
											OffMenu(BufferInfo->Window,FULLMENUNUM(1,2,NOSUB));

											if(!Parent)
												OffMenu(BufferInfo->Window,FULLMENUNUM(1,1,NOSUB));

											BufferInfo->Left	= BufferInfo->Window->BorderLeft;
											BufferInfo->Top		= BufferInfo->Window->BorderTop;
											BufferInfo->Width	= BufferInfo->Window->Width		- (BufferInfo->Window->BorderLeft	+ BufferInfo->Window->BorderRight);
											BufferInfo->Height	= BufferInfo->Window->Height	- (BufferInfo->Window->BorderTop	+ BufferInfo->Window->BorderBottom);

											if(!Parent)
												BufferInfo->Width -= BufferInfo->ArrowWidth;

											BufferInfo->Width	-= BufferInfo->Width	% BufferInfo->LocalTextFontWidth;
											BufferInfo->Height	-= BufferInfo->Height	% BufferInfo->LocalTextFontHeight;

											if(Parent)
												WindowLimits(BufferInfo->Window,BufferInfo->Window->BorderLeft + 20 * BufferInfo->LocalTextFontWidth + BufferInfo->Window->BorderRight,BufferInfo->Window->BorderTop + BufferInfo->LocalTextFontHeight + BufferInfo->Window->BorderBottom,BufferInfo->Screen->Width,BufferInfo->Screen->Height);

											MaxColumns	= BufferInfo->Window->WScreen->Width / BufferInfo->LocalTextFontWidth + 1;
											MaxLines	= BufferInfo->Window->WScreen->Height / BufferInfo->LocalTextFontHeight + 1;

											if(BufferInfo->BufferLineOffsets = (UWORD *)AllocVecPooled(sizeof(UWORD) * (MaxLines + MaxColumns),MEMF_ANY | MEMF_CLEAR))
											{
												LONG Index;
												LONG i;

												BufferInfo->BufferColumnOffsets	= &BufferInfo->BufferLineOffsets[MaxLines];

												for(i = Index = 0 ; i < MaxLines ; i++)
												{
													BufferInfo->BufferLineOffsets[i] = Index;

													Index += BufferInfo->LocalTextFontHeight;
												}

												for(i = Index = 0 ; i < MaxColumns ; i++)
												{
													BufferInfo->BufferColumnOffsets[i] = Index;

													Index += BufferInfo->LocalTextFontWidth;
												}

													/* Determine maximum dimensions of
													 * the buffer screen (in rows and
													 * columns).
													 */

												if(Parent)
													BufferInfo->NumBufferColumns = BufferInfo->Width / BufferInfo->LocalTextFontWidth;
												else
													BufferInfo->NumBufferColumns = (BufferInfo->Window->Width - (BufferInfo->ArrowWidth + 1)) / BufferInfo->LocalTextFontWidth;

												BufferInfo->NumBufferLines = BufferInfo->Height / BufferInfo->LocalTextFontHeight;

												if(BufferInfo->TopLine == -1 || !Config->CaptureConfig->RememberBufferScreen)
												{
													switch(Config->CaptureConfig->OpenBufferScreen)
													{
														case BUFFER_TOP:

															BufferInfo->TopLine = 0;
															break;

														case BUFFER_END:

															if((BufferInfo->TopLine = Lines - BufferInfo->NumBufferLines) < 0)
																BufferInfo->TopLine = 0;

															break;

														default:

															BufferInfo->TopLine = 0;
															break;
													}
												}

												if(BufferInfo->TopLine > Lines - BufferInfo->NumBufferLines || BufferInfo->TopLine < 0)
													BufferInfo->TopLine = 0;

												BufferInfo->RPort = BufferInfo->Window->RPort;

												SetFont(BufferInfo->RPort,BufferInfo->LocalFont);

													/* Bring the screen to the front. */

												BumpWindow(BufferInfo->Window);

													/* Set the drawing pens for the window. */

												if(Kick30)
												{
														/* Don't slow down display updates when using an
														 * interleaved screen.
														 */

													if(!(GetBitMapAttr(BufferInfo->RPort->BitMap,BMA_FLAGS) & BMF_INTERLEAVED))
														SetMaxPen(BufferInfo->RPort,BufferInfo->MaxPen);
												}

												SetPens(BufferInfo->RPort,BufferInfo->TextFrontPen,BufferInfo->TextBackPen,JAM2);

													/* Initial creation of the buffer display. */

												BufferInfo->DisplayedLines = RedrawScreen(BufferInfo,BufferInfo->TopLine);

												SetGadgetAttrs((struct Gadget *)BufferInfo->Scroller,BufferInfo->Window,NULL,
													PGA_Top,		BufferInfo->TopLine,
													PGA_Total,		Lines,
													PGA_Visible,	BufferInfo->NumBufferLines,
												TAG_DONE);

												if(!Parent)
												{
													struct RastPort *RPort = BufferInfo->Screen->BarLayer->rp;

													BufferInfo->TitleOffset = TextLength(RPort,LocaleString(MSG_TERMBUFFER_TERM_BUFFER_TXT),strlen(LocaleString(MSG_TERMBUFFER_TERM_BUFFER_TXT))) + TextLength(RPort,"  ",1) + 4;
												}

												return(BufferInfo);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	DeleteBufferInfo(BufferInfo);

	return(NULL);
}

	/* BufferServer():
	 *
	 *	Asynchronous task to display the data stored in the
	 *	scrollback display buffer.
	 */

STATIC VOID SAVE_DS
BufferServer()
{
	STATIC BOOL		SearchForward	= TRUE,
					IgnoreCase		= TRUE,
					WholeWords		= FALSE;
	STATIC LONG		TopLine			= -1;

	TextBufferInfo	*BufferInfo;
	struct Task		*Father,*Me;
	BOOL			 RingBack = TRUE;

		/* Wake this guy up */

	Me = FindTask(NULL);

	Father = (struct Task *)Me->tc_UserData;

		/* Snap our fingers... */

	if(BufferInfo = CreateBufferInfo(NULL,&SearchForward,&IgnoreCase,&WholeWords,&TopLine))
	{
			/* Open wide, here I come! */

		ObtainSemaphore(&BufferTaskSemaphore);

		BufferInfoData = BufferInfo;

		ReleaseSemaphore(&BufferTaskSemaphore);

			/* Up and running */

		Signal(Father,SIG_HANDSHAKE);

		Father = NULL;

			/* Swish the tinsel */

		RingBack = HandleBuffer(&BufferTaskSemaphore,&BufferInfoData);

			/* Activate the main window again */

		if(Window)
			BumpWindow(Window);

			/* The wrecking crew */

		DeleteBufferInfo(BufferInfo);
	}

		/* Shutdown in an orderly fashion */

	Forbid();

	if(RingBack)
	{
		if(Father)
			Signal(Father,SIG_HANDSHAKE);
		else
			Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
	}
}

	/* LaunchBuffer():
	 *
	 *	Launch the buffer process.
	 */

BOOL
LaunchBuffer()
{
	ObtainSemaphore(&BufferTaskSemaphore);

		/* Is the buffer process already running? */

	if(BufferInfoData)
	{
			/* Tell it to bring its screen to the front. */

		Signal((struct Task *)BufferInfoData->Buddy,SIG_TOFRONT);

		ReleaseSemaphore(&BufferTaskSemaphore);

			/* Return success. */

		return(TRUE);
	}
	else
	{
		struct Task	*Child;
		BOOL		 Result = FALSE;

		ReleaseSemaphore(&BufferTaskSemaphore);

		Forbid();

			/* Launch the buffer process. */

		if(Child = (struct Task *)CreateNewProcTags(
			NP_Entry,		BufferServer,
			NP_Name,		"term Buffer Process",
			NP_WindowPtr,	-1,
		TAG_END))
		{
			Child->tc_UserData = FindTask(NULL);

			WaitForHandshake();

			ObtainSemaphore(&BufferTaskSemaphore);

			if(BufferInfoData)
				Result = TRUE;

			ReleaseSemaphore(&BufferTaskSemaphore);
		}

		Permit();

			/* Return the result. */

		return(Result);
	}
}

	/* TerminateBuffer():
	 *
	 *	Terminate the buffer process.
	 */

VOID
TerminateBuffer()
{
	ObtainSemaphore(&BufferTaskSemaphore);

	if(BufferInfoData)
	{
		Forbid();

		ReleaseSemaphore(&BufferTaskSemaphore);
		ShakeHands((struct Task *)BufferInfoData->Buddy,SIG_KILL);

		Permit();
	}
	else
		ReleaseSemaphore(&BufferTaskSemaphore);
}

	/* MoveBuffer(TextBufferInfo *BufferInfo,BYTE Mode):
	 *
	 *	Move the currently displayed buffer area somewhere.
	 */

VOID
MoveBuffer(struct SignalSemaphore *Access,struct TextBufferInfo **Data,LONG Mode)
{
	struct TextBufferInfo *BufferInfo;

	ObtainSemaphore(Access);

	BufferInfo = (TextBufferInfo *)*Data;

	if(BufferInfo && BufferInfo->Queue)
	{
		LONG SigBit;

		if((SigBit = AllocSignal(-1)) != -1)
		{
			struct DataMsg Msg;

			InitMsgItem(&Msg,(DESTRUCTOR)BufferDestructor);

			Msg.Type	= DATAMSGTYPE_MOVEREVIEW;
			Msg.Size	= Mode;
			Msg.Client	= FindTask(NULL);
			Msg.Mask	= 1L << SigBit;

			ClrSignal(Msg.Mask);

			PutMsgItem(BufferInfo->Queue,(struct MsgItem *)&Msg);

			ReleaseSemaphore(Access);

			Wait(Msg.Mask);

			FreeSignal(SigBit);

			return;
		}
	}

	ReleaseSemaphore(Access);
}

	/* NotifyBuffer(struct SignalSemaphore *Access,struct TextBufferInfo **Data,ULONG Signals):
	 *
	 *	Send a signal to the buffer process.
	 */

VOID
NotifyBuffer(struct SignalSemaphore *Access,struct TextBufferInfo **Data,ULONG Signals)
{
	struct TextBufferInfo *BufferInfo;

	ObtainSemaphore(Access);

	BufferInfo = (TextBufferInfo *)*Data;

	if(BufferInfo && BufferInfo->Buddy)
	{
		Forbid();

		ReleaseSemaphore(Access);
		ShakeHands((struct Task *)BufferInfo->Buddy,Signals);

		Permit();
	}
	else
		ReleaseSemaphore(Access);
}


/*****************************************************************************/


STATIC VOID SAVE_DS
ReviewServer()
{
	STATIC BOOL		SearchForward	= TRUE,
					IgnoreCase		= TRUE,
					WholeWords		= FALSE;
	STATIC LONG		TopLine			= -1;

	TextBufferInfo	*BufferInfo;
	struct Task		*Father,*Me;
	BOOL			 RingBack = TRUE;

		/* Wake this guy up */

	Me = FindTask(NULL);

	Father = (struct Task *)Me->tc_UserData;

		/* Snap our fingers... */

	if(BufferInfo = CreateBufferInfo(Window->WScreen,&SearchForward,&IgnoreCase,&WholeWords,&TopLine))
	{
			/* Open wide, here I come! */

		ObtainSemaphore(&ReviewTaskSemaphore);

		ReviewInfoData = BufferInfo;

		ReleaseSemaphore(&ReviewTaskSemaphore);

			/* Up and running */

		Signal(Father,SIG_HANDSHAKE);

		Father = NULL;

			/* Swish the tinsel */

		RingBack = HandleBuffer(&ReviewTaskSemaphore,&ReviewInfoData);

		if(Config->CaptureConfig->RememberBufferWindow)
			PutWindowInfo(WINDOW_REVIEW,BufferInfo->Window->LeftEdge,BufferInfo->Window->TopEdge,BufferInfo->Window->Width,BufferInfo->Window->Height);

			/* Activate the main window again */

		if(Window)
			BumpWindow(Window);

			/* The wrecking crew */

		DeleteBufferInfo(BufferInfo);

		CheckItem(MEN_REVIEW_WINDOW,FALSE);
	}

		/* Shutdown in an orderly fashion */

	Forbid();

	if(RingBack)
	{
		if(Father)
			Signal(Father,SIG_HANDSHAKE);
		else
			Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
	}
}

VOID
MoveReview(LONG Mode)
{
	MoveBuffer(&ReviewTaskSemaphore,&ReviewInfoData,Mode);
}

VOID
DeleteReview()
{
	CheckItem(MEN_REVIEW_WINDOW,FALSE);

	ObtainSemaphore(&ReviewTaskSemaphore);

	if(ReviewInfoData)
	{
		Forbid();

		ReleaseSemaphore(&ReviewTaskSemaphore);
		ShakeHands((struct Task *)ReviewInfoData->Buddy,SIG_KILL);

		Permit();
	}
	else
		ReleaseSemaphore(&ReviewTaskSemaphore);
}

BOOL
CreateReview()
{
	BOOL Result = FALSE;

	ObtainSemaphore(&ReviewTaskSemaphore);

		/* Is the Review process already running? */

	if(ReviewInfoData)
	{
			/* Tell it to bring its screen to the front. */

		Signal((struct Task *)ReviewInfoData->Buddy,SIG_TOFRONT);

		ReleaseSemaphore(&ReviewTaskSemaphore);

			/* Return success. */

		Result = TRUE;
	}
	else
	{
		struct Task	*Child;

		ReleaseSemaphore(&ReviewTaskSemaphore);

		Forbid();

			/* Launch the Review process. */

		if(Child = (struct Task *)CreateNewProcTags(
			NP_Entry,		ReviewServer,
			NP_Name,		"term Review Process",
			NP_WindowPtr,	-1,
		TAG_END))
		{
			Child->tc_UserData = FindTask(NULL);

			WaitForHandshake();

			ObtainSemaphore(&ReviewTaskSemaphore);

			if(ReviewInfoData)
				Result = TRUE;

			ReleaseSemaphore(&ReviewTaskSemaphore);
		}

		Permit();
	}

	if(Result)
		CheckItem(MEN_REVIEW_WINDOW,TRUE);

	return(Result);
}
