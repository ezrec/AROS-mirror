/*
**	Tools.c
**
**	Miscellaneous support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

#ifdef __AROS__
#include <aros/asmcall.h>
# ifdef Dispatch
#  undef Dispatch
# endif
AROS_UFP3(VOID, Dispatch,
 AROS_UFPA(struct Hook *        , UnusedHook , A0),
 AROS_UFPA(struct RastPort *    , RPort, A2),
 AROS_UFPA(LayerMsg *           , Bounds, A1));
#endif

	/* Current timer state. */

STATIC BOOL TimerRunning;

	/* ListViewStateFill(struct LVDrawMsg *Msg):
	 *
	 *	Draw the ghosting pattern over a ghosted listview line.
	 */

VOID
ListViewStateFill(struct LVDrawMsg *Msg)
{
	if(Msg->lvdm_State == LVR_SELECTEDDISABLED || Msg->lvdm_State == LVR_NORMALDISABLED)
	{
		struct RastPort	*RPort = Msg->lvdm_RastPort;
		ULONG Ghosting = 0x44441111;

		SetABPenDrMd(RPort,Msg->lvdm_DrawInfo->dri_Pens[SHADOWPEN],0,JAM1);

		SetAfPt(RPort,(UWORD *)&Ghosting,1);

		RectFill(RPort,Msg->lvdm_Bounds.MinX,Msg->lvdm_Bounds.MinY,Msg->lvdm_Bounds.MaxX,Msg->lvdm_Bounds.MaxY);

		SetAfPt(RPort,NULL,0);
	}
}

	/* EraseWindow(struct Window *Window):
	 *
	 *	Fills the interior of a window in the background colour.
	 */

VOID
EraseWindow(struct Window *Window,UWORD *Pens)
{
	LONG Left,Top,Right,Bottom;

	Left	= Window->BorderLeft;
	Top		= Window->BorderTop;
	Right	= Window->Width - (Window->BorderRight + 1);
	Bottom	= Window->Height - (Window->BorderBottom + 1);

	if(Pens)
	{
		SetPens(RPort,Pens[BACKGROUNDPEN],0,JAM1);

		RectFill(Window->RPort,Left,Top,Right,Bottom);
	}
	else
		EraseRect(Window->RPort,Left,Top,Right,Bottom);
}

	/* GetListMaxPen(UWORD *Pens):
	 *
	 *	Calculates the maximum pen number index to be
	 *	used for listview text rendering.
	 */

LONG
GetListMaxPen(UWORD *Pens)
{
	STATIC BYTE PenTable[] =
	{
		TEXTPEN,
		BACKGROUNDPEN,
		FILLTEXTPEN,
		FILLPEN,
		SHADOWPEN,
		HIGHLIGHTTEXTPEN,
	};

	LONG Max,Pen,i;

	for(i = 0, Max = -1 ; i < NUM_ELEMENTS(PenTable) ; i++)
	{
		Pen = Pens[PenTable[i]];

		if(Pen > Max)
			Max = Pen;
	}

	return(Max);
}

	/* FillBox(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height):
	 *
	 *	Fill a rectangular area with the current foregroung colour.
	 */

VOID
FillBox(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height)
{
	if(Width > 0 && Height > 0)
		RectFill(RPort,Left,Top,Left + Width - 1,Top + Height - 1);
}

VOID
FillWindowBox(struct Window *Window,LONG Left,LONG Top,LONG Width,LONG Height)
{
	if(Left + Width > WindowWidth)
		Width = WindowWidth - Left - 1;

	if(Top + Height > WindowHeight)
		Height = WindowHeight - Top - 1;

	if(Width > 0 && Height > 0)
	{
		Left	+= Window->BorderLeft;
		Top		+= Window->BorderTop;

		RectFill(Window->RPort,Left,Top,Left + Width - 1,Top + Height - 1);
	}
}

	/* PlaceText(struct RastPort *RPort,LONG Left,LONG Top,STRPTR String,LONG Len):
	 *
	 *	Print some text at a specific position.
	 */

VOID
PlaceText(struct RastPort *RPort,LONG Left,LONG Top,STRPTR String,LONG Len)
{
	Move(RPort,Left,Top + RPort->TxBaseline);
	Text(RPort,String,Len);
}

	/* SetPens(struct RastPort *RPort,ULONG APen,ULONG BPen,ULONG DrMd):
	 *
	 *	Set rendering attributes.
	 */

VOID
SetPens(struct RastPort *RPort,ULONG APen,ULONG BPen,ULONG DrMd)
{
	if(Kick30)
		SetABPenDrMd(RPort,APen,BPen,DrMd);
	else
	{
		if(APen != RPort->FgPen)
			SetAPen(RPort,APen);

		if(BPen != RPort->BgPen)
			SetBPen(RPort,BPen);

		if(DrMd != RPort->DrawMode)
			SetDrMd(RPort,DrMd);
	}
}

	/* Atol(STRPTR Buffer):
	 *
	 *	Convert ASCII representation to long word.
	 */

LONG
Atol(STRPTR Buffer)
{
	LONG Result;

	StrToLong(Buffer,&Result);

	return(Result);
}

	/* StripSpaces(STRPTR String):
	 *
	 *	Strip leading and trailing spaces from a string.
	 */

VOID
StripSpaces(STRPTR String)
{
	if(String)
	{
		STRPTR To;
		LONG Len;

		To = String;

		while(*String == ' ')
			String++;

		if(Len = strlen(String))
		{
			while(Len > 0 && String[Len - 1] == ' ')
				Len--;

			while(Len--)
				*To++ = *String++;
		}

		*To = 0;
	}
}

	/* ReplaceWindowInfo(struct WindowInfo *NewInfo):
	 *
	 *	Drop the window info into the right place.
	 */

VOID
ReplaceWindowInfo(struct WindowInfo *NewInfo)
{
	LONG i;

	ObtainSemaphore(&WindowInfoSemaphore);

	for(i = 0 ; WindowInfoTable[i].ID != -1 ; i++)
	{
		if(WindowInfoTable[i].ID == NewInfo->ID)
		{
			CopyMem(NewInfo,&WindowInfoTable[i],sizeof(struct WindowInfo));

			break;
		}
	}

	ReleaseSemaphore(&WindowInfoSemaphore);
}

	/* PutWindowInfo():
	 *
	 *	Store window size and position relative to
	 *	the main window.
	 */

VOID
PutWindowInfo(LONG ID,LONG Left,LONG Top,LONG Width,LONG Height)
{
	LONG WindowLeft,WindowTop,WindowWidth,WindowHeight;
	struct WindowInfo *Info;
	ULONG IntuiLock;
	LONG i;

	ObtainSemaphore(&WindowInfoSemaphore);

	for(i = 0 ; WindowInfoTable[i].ID != -1 ; i++)
	{
		if(WindowInfoTable[i].ID == ID)
		{
			Info = &WindowInfoTable[i];

			break;
		}
	}

	IntuiLock = LockIBase(NULL);

	WindowLeft		= Window->LeftEdge;
	WindowTop		= Window->TopEdge;
	WindowWidth		= Window->Width;
	WindowHeight	= Window->Height;

	UnlockIBase(IntuiLock);

	Info->WindowFlags = NULL;

	if(WindowWidth == Width && WindowLeft == Left)
		Info->WindowFlags |= WC_EXPANDWIDTH;
	else
	{
		if(Left == WindowLeft + WindowWidth)
			Info->WindowFlags |= WC_ALIGNSIDE;

		if(WindowLeft == Left)
			Info->WindowFlags |= WC_ALIGNLEFT;

		if(WindowLeft + WindowWidth == Left + Width)
			Info->WindowFlags |= WC_ALIGNRIGHT;
	}

	if(WindowHeight == Height && WindowTop + Window->BorderTop == Top)
		Info->WindowFlags |= WC_EXPANDHEIGHT;
	else
	{
		if(Top == WindowTop + WindowHeight)
			Info->WindowFlags |= WC_ALIGNBELOW;

		if(Top == WindowTop + Window->BorderTop)
			Info->WindowFlags |= WC_ALIGNTOP;

		if(WindowTop + WindowHeight == Top + Height)
			Info->WindowFlags |= WC_ALIGNBOTTOM;
	}

	Info->Left		= Left;
	Info->Top		= Top;
	Info->Width		= Width;
	Info->Height	= Height;

	ReleaseSemaphore(&WindowInfoSemaphore);
}

	/* GetWindowInfo():
	 *
	 *	Set the window size and position in relation to
	 *	the main window.
	 */

VOID
GetWindowInfo(LONG ID,LONG *Left,LONG *Top,LONG *Width,LONG *Height,LONG DefWidth,LONG DefHeight)
{
	LONG WindowLeft,WindowTop,WindowWidth,WindowHeight;
	struct WindowInfo *Info;
	ULONG IntuiLock;
	LONG i;

	SafeObtainSemaphoreShared(&WindowInfoSemaphore);

	Info = NULL;

	for(i = 0 ; WindowInfoTable[i].ID != -1 ; i++)
	{
		if(WindowInfoTable[i].ID == ID)
		{
			Info = &WindowInfoTable[i];

			break;
		}
	}

	if(Info && ID == WINDOW_MAIN)
	{
		*Left	= Info->Left;
		*Top	= Info->Top;
		*Width	= Info->Width;
		*Height	= Info->Height;
	}
	else
	{
		if(Window)
		{
			IntuiLock = LockIBase(NULL);

			WindowLeft		= Window->LeftEdge;
			WindowTop		= Window->TopEdge;
			WindowWidth		= Window->Width;
			WindowHeight	= Window->Height;

			UnlockIBase(IntuiLock);

			if(DefWidth && DefWidth < WindowWidth / 2)
				DefWidth = WindowWidth / 2;

			if(DefHeight && DefHeight < WindowHeight / 2)
				DefHeight = WindowHeight / 2;
		}

		if(*Width)
		{
			if(Info->Width)
				*Left = Info->Left;
			else
				*Left = WindowLeft + (WindowWidth - *Width) / 2;
		}
		else
		{
			if(DefWidth && !Info->Width)
			{
				*Width	= DefWidth;
				*Left	= WindowLeft + (WindowWidth - *Width) / 2;
			}
			else
			{
				if(Info->Width)
				{
					*Width	= Info->Width;
					*Left	= Info->Left;
				}
				else
				{
					*Width	= WindowWidth;
					*Left	= WindowLeft;
				}
			}
		}

		if(*Height)
		{
			if(Info->Height)
				*Top = Info->Top;
			else
				*Top = WindowTop + (WindowHeight - *Height) / 2;
		}
		else
		{
			if(DefHeight && !Info->Height)
			{
				*Height	= DefHeight;
				*Top	= WindowTop + (WindowHeight - *Height) / 2;
			}
			else
			{
				if(Info->Height)
				{
					*Height	= Info->Height;
					*Top	= Info->Top;
				}
				else
				{
					*Height	= WindowHeight;
					*Top	= WindowTop;
				}
			}
		}

		if(Info->WindowFlags & WC_ALIGNSIDE)
			*Left = WindowLeft + WindowWidth;

		if(Info->WindowFlags & WC_ALIGNBELOW)
			*Top = WindowTop + WindowHeight;

		if(Info->WindowFlags & WC_ALIGNLEFT)
			*Left = WindowLeft;

		if(Info->WindowFlags & WC_ALIGNTOP)
			*Top = WindowTop;

		if(Info->WindowFlags & WC_ALIGNRIGHT)
			*Left = WindowLeft + WindowWidth - *Width;

		if(Info->WindowFlags & WC_ALIGNBOTTOM)
			*Top = WindowTop + WindowHeight - *Height;

		if(Info->WindowFlags & WC_EXPANDWIDTH)
			*Width = WindowWidth;

		if(Info->WindowFlags & WC_EXPANDHEIGHT)
			*Height = WindowHeight;

		if(*Top == WindowTop && *Left == WindowLeft)
		{
			LONG TitleHeight = Window->BorderTop;

			if(*Height == WindowHeight)
				*Height -= TitleHeight;

			*Top += TitleHeight;
		}
	}

	ReleaseSemaphore(&WindowInfoSemaphore);
}

	/* GetBitMapDepth(struct BitMap *BitMap):
	 *
	 *	Return the depth of a BitMap.
	 */

LONG
GetBitMapDepth(struct BitMap *BitMap)
{
	LONG Depth;

	if(Kick30)
		Depth = (LONG)GetBitMapAttr(BitMap,BMA_DEPTH);
	else
		Depth = BitMap->Depth;

	if(Depth > 8)
		Depth = 8;

	return(Depth);
}

	/* GetDPI(ULONG Mode,ULONG *X_DPI,ULONG *Y_DPI):
	 *
	 *	Get screen DPI resolution values.
	 */

VOID
GetDPI(ULONG Mode,ULONG *X_DPI,ULONG *Y_DPI)
{
	struct DisplayInfo DisplayInfo;

	*X_DPI = *Y_DPI = 72;

	if(GetDisplayInfoData(NULL,(APTR)&DisplayInfo,sizeof(struct DisplayInfo),DTAG_DISP,Mode))
	{
		if(DisplayInfo.PixelSpeed && DisplayInfo.Resolution.x && DisplayInfo.Resolution.y)
		{
			*X_DPI = (35 * 140) / DisplayInfo.PixelSpeed;
			*Y_DPI = (*X_DPI * DisplayInfo.Resolution.x) / DisplayInfo.Resolution.y;
		}
	}
}

	/* AddProtection(STRPTR FileName,ULONG Mask):
	 *
	 *	Set bits in the protection mask of a file.
	 */

VOID
AddProtection(STRPTR Name,ULONG Mask)
{
	BPTR FileLock;
	D_S(struct FileInfoBlock,FileInfo);

	if(FileLock = Lock(Name,ACCESS_READ))
	{
		if(Examine(FileLock,FileInfo))
		{
			UnLock(FileLock);

			if(Mask == FIBF_EXECUTE)
				Mask |= FileInfo->fib_Protection & ~(FIBF_OTR_EXECUTE|FIBF_GRP_EXECUTE);
			else
				Mask |= FileInfo->fib_Protection;

			SetProtection(Name,Mask);
		}
		else
			UnLock(FileLock);
	}
}

	/* GetPubScreenName(struct Screen *Screen,STRPTR Name):
	 *
	 *	Get the name of a public screen.
	 */

BOOL
GetPubScreenName(struct Screen *Screen,STRPTR Name)
{
	struct List				*PubScreenList;
	struct PubScreenNode	*ScreenNode;
	BOOL					 GotIt;

	PubScreenList = LockPubScreenList();

	for(ScreenNode = (struct PubScreenNode *)PubScreenList->lh_Head, GotIt = FALSE ; !GotIt && ScreenNode->psn_Node.ln_Succ ; ScreenNode = (struct PubScreenNode *)ScreenNode->psn_Node.ln_Succ)
	{
		if(ScreenNode->psn_Screen == Screen)
		{
			strcpy(Name,ScreenNode->psn_Node.ln_Name);
			GotIt = TRUE;
		}
	}

	UnlockPubScreenList();

	return(GotIt);
}

	/* InitSinglePort(struct MsgPort *Port):
	 *
	 *	Initialize a plain MsgPort (as created on the stack) for
	 *	usage. Don't try this at home, kids!
	 */

VOID
InitSinglePort(struct MsgPort *Port)
{
	memset(Port,0,sizeof(struct MsgPort));

	Port->mp_Flags		= PA_SIGNAL;
	Port->mp_SigBit		= SIGB_SINGLE;
	Port->mp_SigTask	= FindTask(NULL);

	NewList(&Port->mp_MsgList);
}

	/* GoodStream(BPTR Stream):
	 *
	 *	Check to see whether the current input file
	 *	is an interactive stream.
	 */

BOOL
GoodStream(BPTR Stream)
{
	if(!Stream)
		Stream = Input();

	if(Stream)
	{
		struct FileHandle *Handle = (struct FileHandle *)BADDR(Stream);

#warning Deactivated to make it compile for AROS.
#ifndef __AROS__
		if(Handle->fh_Type)
		{
			if(IsInteractive(Stream))
				return(TRUE);
		}
#endif
	}

	return(FALSE);
}

	/* GetProgramIcon():
	 *
	 *	Try to find the icon of the program.
	 */

struct DiskObject *
GetProgramIcon()
{
	struct DiskObject *Icon = NULL;

		/* Run from Workbench? */

	if(WBenchMsg)
	{
			/* Sanity check. */

		if(WBenchMsg->sm_ArgList)
		{
				/* Yet another sanity check. */

			if(WBenchMsg->sm_ArgList->wa_Name)
			{
					/* Try to find the icon in the current directory. */

				if(Icon = GetDiskObjectNew(WBenchMsg->sm_ArgList->wa_Name))
				{
					if(Icon->do_Type != WBTOOL)
					{
						FreeDiskObject(Icon);

						Icon = NULL;
					}
				}

				if(!Icon)
				{
					BPTR NewLock;

						/* Move to the directory the
						 * program was run from.
						 */

					if(NewLock = Lock("PROGDIR:",ACCESS_READ))
					{
						BPTR OldLock;

						OldLock = CurrentDir(NewLock);

							/* Try to fetch the icon, use the
							 * default name if necessary.
							 */

						if(Icon = GetDiskObjectNew(WBenchMsg->sm_ArgList->wa_Name))
						{
							if(Icon->do_Type != WBTOOL)
							{
								FreeDiskObject(Icon);

								Icon = NULL;
							}
						}

						if(!Icon)
						{
							if(Icon = GetDiskObjectNew("term"))
							{
								if(Icon->do_Type != WBTOOL)
								{
									FreeDiskObject(Icon);

									Icon = NULL;
								}
							}
						}

						CurrentDir(OldLock);

						UnLock(NewLock);
					}
				}
			}
		}
	}

		/* Still no success. */

	if(!Icon)
	{
			/* Use the default names. */

		if(Icon = GetDiskObjectNew("term"))
		{
			if(Icon->do_Type != WBTOOL)
			{
				FreeDiskObject(Icon);

				Icon = NULL;
			}
		}

		if(!Icon)
		{
			if(Icon = GetDiskObjectNew("PROGDIR:term"))
			{
				if(Icon->do_Type != WBTOOL)
				{
					FreeDiskObject(Icon);

					Icon = NULL;
				}
			}
		}
	}

	return(Icon);
}

	/* GetPenIndex(LONG Pen):
	 *
	 *	Get the table index corresponding to an on-screen
	 *	text rendering pen.
	 */

LONG
GetPenIndex(LONG Pen)
{
	LONG i,Result = 0;

	for(i = 0 ; i < 16 ; i++)
	{
		if(MappedPens[0][i] == Pen)
		{
			Result = i;
			break;
		}
	}

	return(Result);
}

	/* GetScreenWidth(struct Window *Window):
	 *
	 *	Query the current screen width.
	 */

LONG
GetScreenWidth(struct Window *Window)
{
	if(Window)
	{
		if(Window->Width > ScreenWidth || Window->LeftEdge < -Window->WScreen->LeftEdge || Window->LeftEdge + Window->WScreen->LeftEdge + Window->Width > ScreenWidth)
			return(ScreenWidth);
		else
			return(Window->Width - (Window->BorderLeft + Window->BorderRight));
	}
	else
		return(ScreenWidth);
}

	/* GetScreenHeight(struct Window *Window):
	 *
	 *	Query the current screen height.
	 */

LONG
GetScreenHeight(struct Window *Window)
{
	if(Window)
	{
		if(Window->Height > ScreenHeight || Window->TopEdge < -Window->WScreen->TopEdge || Window->TopEdge + Window->WScreen->TopEdge + Window->Height > ScreenHeight)
			return(ScreenHeight);
		else
			return(Window->Height - (Window->BorderTop + Window->BorderBottom));
	}
	else
		return(ScreenHeight);
}

	/* GetLeft(struct Screen *Screen):
	 *
	 *	Get the screen left edge.
	 */

STATIC LONG
GetLeft(struct Screen *Screen)
{
	if(Screen->LeftEdge >= 0)
		return(0);
	else
		return(-Screen->LeftEdge);
}

	/* GetScreenLeft(struct Window *Window):
	 *
	 *	Query the current screen left edge.
	 */

LONG
GetScreenLeft(struct Window *Window)
{
	if(Window)
	{
		if(Window->Width > ScreenWidth || Window->LeftEdge < -Window->WScreen->LeftEdge || Window->LeftEdge + Window->WScreen->LeftEdge + Window->Width > ScreenWidth)
			return(GetLeft(Window->WScreen));
		else
			return(Window->LeftEdge + Window->BorderLeft);
	}
	else
	{
		struct Screen *PubScreen = LockPubScreen(NULL);
		LONG Result;

		Result = GetLeft(PubScreen ? PubScreen : SharedScreen);

		UnlockPubScreen(NULL,PubScreen);

		return(Result);
	}
}

	/* OldGetAPen(struct RastPort *RPort):
	 *
	 *	Query the current primary rendering colour (old style).
	 */

ULONG
OldGetAPen(struct RastPort *RPort)
{
	return((ULONG)RPort->FgPen);
}

	/* OldGetBPen(struct RastPort *RPort):
	 *
	 *	Query the current seconary rendering colour (old style).
	 */

ULONG
OldGetBPen(struct RastPort *RPort)
{
	return((ULONG)RPort->BgPen);
}

	/* OldGetDrMd(struct RastPort *RPort):
	 *
	 *	Query the current drawing mode (old style).
	 */

ULONG
OldGetDrMd(struct RastPort *RPort)
{
	return((ULONG)RPort->DrawMode);
}

	/* OldSetWrMsk(struct RastPort *RPort,ULONG Mask):
	 *
	 *	Set the rendering plane mask (old style).
	 */

ULONG
OldSetWrMsk(struct RastPort *RPort,ULONG Mask)
{
	if(UseMasking)
		RPort->Mask = Mask;

	return((ULONG)1);
}

	/* OldGetWrMsk(struct RastPort *RPort):
	 *
	 *	Get the rendering plane mask (old style).
	 */

ULONG
OldGetWrMsk(struct RastPort *RPort)
{
	return(RPort->Mask);
}

	/* NewGetAPen(struct RastPort *RPort):
	 *
	 *	Query the current primary rendering colour (new style).
	 */

ULONG
NewGetAPen(struct RastPort *RPort)
{
	return(GetAPen(RPort));
}

	/* NewGetBPen(struct RastPort *RPort):
	 *
	 *	Query the current seconary rendering colour (new style).
	 */

ULONG
NewGetBPen(struct RastPort *RPort)
{
	return(GetBPen(RPort));
}

	/* NewGetDrMd(struct RastPort *RPort):
	 *
	 *	Query the current drawing mode (new style).
	 */

ULONG
NewGetDrMd(struct RastPort *RPort)
{
	return(GetDrMd(RPort));
}

	/* NewSetWrMsk(struct RastPort *RPort,ULONG Mask):
	 *
	 *	Set the rendering plane mask (new style).
	 */

ULONG
NewSetWrMsk(struct RastPort *RPort,ULONG Mask)
{
	if(UseMasking)
		return(SetWriteMask(RPort,Mask));
	else
		return((ULONG)1);
}

	/* NewGetWrMsk(struct RastPort *RPort):
	 *
	 *	Get the rendering plane mask (new style).
	 */

ULONG
NewGetWrMsk(struct RastPort *RPort)
{
	ULONG Mask;

	Mask = (ULONG)~0;

	GetRPAttrs(RPort,
		RPTAG_WriteMask,&Mask,
	TAG_DONE);

	return(Mask);
}

	/* SetWait(struct Window *Window):
	 *
	 *	Set the busy wait mouse pointer.
	 */

VOID
SetWait(struct Window *Window)
{
	if(GTLayoutBase)
		LT_LockWindow(Window);
}

	/* ClrWait(struct Window *Window):
	 *
	 *	Remove the busy wait mouse pointer.
	 */

VOID
ClrWait(struct Window *Window)
{
	if(GTLayoutBase)
		LT_UnlockWindow(Window);
}

	/* GetModeName(ULONG Mode):
	 *
	 *	Get the name of a display mode.
	 */

VOID
GetModeName(ULONG Mode,STRPTR Buffer,LONG BufferSize)
{
	struct NameInfo	NameInfo;

	if(GetDisplayInfoData(NULL,(APTR)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,Mode))
		LimitedStrcpy(BufferSize,Buffer,NameInfo.Name);
	else
	{
		struct DimensionInfo DimensionInfo;

		if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Mode))
		{
			STRPTR MonitorName;

			switch(Mode & MONITOR_ID_MASK)
			{
				case NTSC_MONITOR_ID:

					MonitorName = "NTSC:";
					break;

				case PAL_MONITOR_ID:

					MonitorName = "PAL:";
					break;

				case VGA_MONITOR_ID:

					MonitorName = "VGA:";
					break;

				case A2024_MONITOR_ID:

					MonitorName = "A2024:";
					break;

				default:

					MonitorName = "";
					break;
			}

			LimitedSPrintf(BufferSize,Buffer,"%s%ldx%ld",MonitorName,DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1,DimensionInfo.TxtOScan.MaxY - DimensionInfo.TxtOScan.MinY + 1);
		}
		else
			LimitedStrcpy(BufferSize,Buffer,LocaleString(MSG_SCREENPANEL_UNKNOWN_TXT));
	}
}

	/* ModeOkay(ULONG ID):
	 *
	 *	Checks whether a display mode ID will do for deep
	 *	screen bitmaps.
	 */

BOOL
ModeOkay(ULONG ID)
{
	struct DimensionInfo DimensionInfo;

	if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,ID))
	{
		if(DimensionInfo.MaxDepth >= 4)
			return(TRUE);
	}

	return(FALSE);
}

	/* ExtractString():
	 *
	 *	Extracts a string from a list separated by `|' characters.
	 */

STRPTR
ExtractString(STRPTR String,STRPTR Destination,BOOL ReturnEnd)
{
	STRPTR OldString;

	if(ReturnEnd)
		OldString = NULL;
	else
		OldString = String;

	while(*String)
	{
		if(*String == '|')
		{
			*Destination = 0;

			String++;

			if(*String)
				return(String);
			else
				return(OldString);
		}
		else
			*Destination++ = *String++;
	}

	*Destination = 0;

	return(OldString);
}

	/* RemoveGetNext(struct Node *Node):
	 *
	 *	Remove a node from a list and return the `next'
	 *	node to take its place.
	 */

struct Node *
RemoveGetNext(struct Node *Node)
{
	struct Node *Next;

		/* Does the node have a successor? */

	if(Node->ln_Succ->ln_Succ)
		Next = Node->ln_Succ;
	else
	{
			/* Does it have a predecessor? */

		if(Node->ln_Pred->ln_Pred)
			Next = Node->ln_Pred;
		else
			Next = NULL;
	}

		/* Remove the old node. */

	Remove(Node);

		/* And return the node to take its place. */

	return(Next);
}

	/* GetListSize(struct List *List):
	 *
	 *	Determine the number of entries in a list.
	 */

LONG
GetListSize(struct List *List)
{
	struct Node *Node;
	LONG i;

	for(Node = List->lh_Head, i = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, i++);

	return(i);
}

	/* GetListNode(LONG Offset,struct List *List):
	 *
	 *	Return the n-th Node entry in a standard exec list.
	 */

struct Node *
GetListNode(LONG Offset,struct List *List)
{
	struct Node *Node;
	LONG Index;

	for(Node = List->lh_Head, Index = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, Index++)
	{
		if(Index == Offset)
			return(Node);
	}

	return(NULL);
}

	/* CreateNode(STRPTR Name):
	 *
	 *	Put a name string into a list node.
	 */

struct Node *
CreateNode(STRPTR Name)
{
	struct Node *Node;

	if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + strlen(Name) + 1,MEMF_ANY))
		strcpy(Node->ln_Name = (STRPTR)(Node + 1),Name);

	return(Node);
}

	/* CreateNodeArgs(STRPTR Format,...):
	 *
	 *	Put a name string into a list node, varargs version.
	 */

struct Node *
CreateNodeArgs(STRPTR Format,...)
{
	UBYTE LocalBuffer[256];
	struct Node *Node;
	va_list VarArgs;

	va_start(VarArgs,Format);
	LimitedVSPrintf(sizeof(LocalBuffer),LocalBuffer,Format,VarArgs);
	va_end(VarArgs);

	if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + strlen(LocalBuffer) + 1,MEMF_ANY))
		strcpy(Node->ln_Name = (STRPTR)(Node + 1),LocalBuffer);

	return(Node);
}

	/* FormatAppend(STRPTR String,STRPTR Format,...):
	 *
	 *	Appends a formatted string to another string.
	 */

VOID
FormatAppend(STRPTR String,STRPTR Format,...)
{
	va_list VarArgs;

	String += strlen(String);

	va_start(VarArgs,Format);
	VSPrintf(String,Format,VarArgs);
	va_end(VarArgs);
}

	/* FreeList(struct List *List):
	 *
	 *	Remove all nodes from the list
	 *	and free them on the way.
	 */

VOID
FreeList(struct List *List)
{
	if(List)
	{
		struct Node *Node;

		while(Node = RemHead(List))
			FreeVecPooled(Node);
	}
}

	/* GetNodeOffset(struct Node *Node,struct List *List):
	 *
	 *	Scan a list for a certain node and return
	 *	its position.
	 */

LONG
GetNodeOffset(struct Node *ListNode,struct List *List)
{
	if(ListNode && List)
	{
		struct Node *Node;
		LONG Offset;

		for(Node = List->lh_Head, Offset = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, Offset++)
		{
			if(Node == ListNode)
				return(Offset);
		}
	}

	return(-1);
}

	/* MoveList(struct List *From,struct List *To):
	 *
	 *	Move the contents of a list to a different list.
	 */

VOID
MoveList(struct List *From,struct List *To)
{
	struct Node *Node;

	while(Node = RemHead(From))
		AddTail(To,Node);
}

	/* CreateList():
	 *
	 *	Create a small, empty list.
	 */

struct List *
CreateList()
{
	struct List *List;

	if(List = (struct List *)AllocVecPooled(sizeof(struct MinList),MEMF_ANY))
		NewList(List);

	return(List);
}

	/* DeleteList():
	 *
	 *	Free the contents of a list and the list itself.
	 */

VOID
DeleteList(struct List *List)
{
	if(List)
	{
		struct Node *Node,*Next;

		for(Node = List->lh_Head ; Next = Node->ln_Succ ; Node = Next)
			FreeVecPooled(Node);

		FreeVecPooled(List);
	}
}

	/* MoveNode(struct List *List,struct Node *Node,LONG How):
	 *
	 *	Move a list node somewhere.
	 */

VOID
MoveNode(struct List *List,struct Node *Node,LONG How)
{
	switch(How)
	{
		case MOVE_HEAD:

			if(Node != List->lh_Head)
			{
				Remove(Node);

				AddHead(List,Node);
			}

			break;

		case MOVE_PRED:

			if(Node != List->lh_Head)
			{
				struct Node *Other;

				Other = Node->ln_Pred->ln_Pred;

				Remove(Node);

				Insert(List,Node,Other);
			}

			break;

		case MOVE_SUCC:

			if(Node != List->lh_TailPred)
			{
				struct Node *Other;

				Other = Node->ln_Succ;

				Remove(Node);

				Insert(List,Node,Other);
			}

			break;

		case MOVE_TAIL:

			if(Node != List->lh_TailPred)
			{
				Remove(Node);

				AddTail(List,Node);
			}

			break;
	}
}

	/* LogAction(STRPTR String,...):
	 *
	 *	Write an action to the default log file.
	 */

VOID
LogAction(STRPTR String,...)
{
	if(Config->CaptureConfig->LogActions && Config->CaptureConfig->LogFileName[0])
	{
		UBYTE	DummyBuffer[512];
		BPTR	File;
		BOOL	Created;

		va_list	VarArgs;

			/* Build a valid string. */

		va_start(VarArgs,String);
		LimitedVSPrintf(sizeof(DummyBuffer),DummyBuffer,String,VarArgs);
		va_end(VarArgs);

			/* Open the log file. */

		if(File = OpenToAppend(Config->CaptureConfig->LogFileName,&Created))
		{
			if(Created)
				FPrintf(File,LocaleString(MSG_TERMAUX_DATE_TIME_ACTION_TXT));
		}

			/* The file is open, build the date/time string and
			 * write the log action.
			 */

		if(File)
		{
			UBYTE			DateBuffer[40],
							TimeBuffer[40];
			struct DateTime	DateTime;

			DateStamp(&DateTime.dat_Stamp);

			DateTime.dat_Format		= FORMAT_DEF;
			DateTime.dat_Flags		= NULL;
			DateTime.dat_StrDay		= NULL;
			DateTime.dat_StrDate	= DateBuffer;
			DateTime.dat_StrTime	= TimeBuffer;

			if(DateToStr(&DateTime))
			{
				StripSpaces(TimeBuffer);

				FPrintf(File,"%-9s %8s %s\n",DateBuffer,TimeBuffer,DummyBuffer);
			}

				/* Done! */

			Close(File);
		}
	}
}

	/* GetString(STRPTR Prompt,STRPTR Buffer):
	 *
	 *	Get a string from the user, very much the same as xpr_gets.
	 */

BOOL
GetString(BOOL LoadGadget,BOOL Password,LONG MaxChars,STRPTR Prompt,STRPTR Buffer)
{
	enum	{	GAD_OK=1,GAD_CANCEL,GAD_STRING };

	struct LayoutHandle	*Handle;
	UBYTE LocalBuffer[256];
	BOOL Success = FALSE;

	if(!MaxChars)
		MaxChars = sizeof(LocalBuffer) - 1;

	if(MaxChars > sizeof(LocalBuffer) - 1)
	{
		CopyMem(Buffer,LocalBuffer,sizeof(LocalBuffer) - 1);

		LocalBuffer[sizeof(LocalBuffer) - 1] = 0;

		MaxChars = sizeof(LocalBuffer) - 1;
	}
	else
		strcpy(LocalBuffer,Buffer);

	if(!Prompt)
		Prompt = LocaleString(MSG_TERMXPR_INPUT_REQUIRED_TXT);

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelText,	Prompt,
			TAG_DONE);
			{
				if(Password)
				{
					LT_New(Handle,
						LA_Type,		PASSWORD_KIND,
						LA_STRPTR,		LocalBuffer,
						LA_ID,			GAD_STRING,
						LA_Chars,		40,
						GTST_MaxChars,	MaxChars,
					TAG_DONE);
				}
				else
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_STRPTR,		LocalBuffer,
						LA_ID,			GAD_STRING,
						LA_Chars,		60,
						LAST_Picker,	LoadGadget,
					TAG_DONE);
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
			LAWN_TitleID,		MSG_GLOBAL_ENTER_TEXT_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			BOOL				 Done = FALSE;
			ULONG				 MsgClass;
			UWORD				 MsgCode;
			struct Gadget		*MsgGadget;

			LT_Activate(Handle,GAD_STRING);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgCode		= Message->Code;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_STRING:

								if(MsgCode == '\r')
								{
									LT_UpdateStrings(Handle);

									strcpy(Buffer,LocalBuffer);

									Success = Done = TRUE;

									LT_PressButton(Handle,GAD_OK);
								}

								break;

							case GAD_OK:

								LT_UpdateStrings(Handle);

								strcpy(Buffer,LocalBuffer);

								Success = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadget->GadgetID == GAD_STRING)
					{
						UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
						struct FileRequester *FileRequest;

						strcpy(DummyBuffer,LocalBuffer);

						if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_TERMAUX_LOAD_FILE_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
						{
							FreeAslRequest(FileRequest);

							LT_SetAttributes(Handle,GAD_STRING,
								GTST_String,	DummyBuffer,
							TAG_DONE);
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

	/* WakeUp(struct Window *Window,LONG Sound):
	 *
	 *	Pop a window to the front and alert the user.
	 */

VOID
WakeUp(struct Window *Window,LONG Sound)
{
	if(Window)
	{
		if(Config->MiscConfig->AlertMode == ALERT_SCREEN || Config->MiscConfig->AlertMode == ALERT_BEEP_SCREEN)
		{
			if(Window->WScreen->LeftEdge > 0)
			{
				if(Window->WScreen->TopEdge > 0)
					MoveScreen(Window->WScreen,-Window->WScreen->LeftEdge,-Window->WScreen->TopEdge);
				else
					MoveScreen(Window->WScreen,-Window->WScreen->LeftEdge,0);
			}
			else
			{
				if(Window->WScreen->TopEdge > 0)
					MoveScreen(Window->WScreen,0,-Window->WScreen->TopEdge);
			}

			WindowToFront(Window);

			ScreenToFront(Window->WScreen);
		}
	}

	if(Sound != SOUND_BELL || Config->MiscConfig->AlertMode == ALERT_BEEP || Config->MiscConfig->AlertMode == ALERT_BEEP_SCREEN)
		SoundPlay(Sound);
}

	/* BlockWindows():
	 *
	 *	Block the main window and the status window (i.e. disable
	 *	the menu and attach a wait pointer).
	 */

VOID
BlockWindows()
{
	if(!(BlockNestCount++))
	{
		LT_LockWindow(Window);
		LT_LockWindow(StatusWindow);
		LT_LockWindow(FastWindow);
		LT_LockWindow(MatrixWindow);

		WeAreBlocking = TRUE;

		SetQueueDiscard(SpecialQueue,TRUE);

		GhostCursor();
	}
}

	/* ReleaseWindows():
	 *
	 *	Reenable the menus and clear the wait pointer.
	 */

VOID
ReleaseWindows()
{
	if(BlockNestCount == 1)
	{
		LT_UnlockWindow(Window);
		LT_UnlockWindow(StatusWindow);
		LT_UnlockWindow(FastWindow);
		LT_UnlockWindow(MatrixWindow);

		WeAreBlocking = FALSE;

		SetQueueDiscard(SpecialQueue,FALSE);

		if(Window)
		{
			Forbid();

			if(Window->Flags & WFLG_WINDOWACTIVE)
				NormalCursor();

			Permit();
		}
	}

	if(BlockNestCount)
		BlockNestCount--;

}

	/* LineRead(BPTR File,STRPTR Buffer,LONG MaxChars):
	 *
	 *	Read a few bytes from a file (à la gets).
	 */

LONG
LineRead(BPTR File,STRPTR Buffer,LONG MaxChars)
{
	STATIC UBYTE	Data[1024];
	STATIC LONG		ReadIndex,ReadLen;

	LONG BytesRead = 0;

	if(File)
	{
		LONG i;

		for(i = 0 ; i < MaxChars ; i++)
		{
			if(ReadIndex >= ReadLen)
			{
				ReadLen = Read(File,Data,sizeof(Data));

				ReadIndex = 0;

				if(ReadLen <= 0)
				{
					Buffer[i] = 0;
					break;
				}
			}

			BytesRead++;

			if((Buffer[i] = Data[ReadIndex++]) == '\n')
			{
				Buffer[i + 1] = 0;

				break;
			}
		}
	}
	else
		ReadIndex = ReadLen = 0;

	return(BytesRead);
}

	/* GetBaudRate(STRPTR Buffer):
	 *
	 *	Calculate the baud rate contained in a connect string.
	 */

LONG
GetBaudRate(STRPTR Buffer)
{
	UBYTE LocalBuffer[40];
	LONG Rate,i,j;

	for(i = j = 0 ; i < strlen(Buffer) ; i++)
	{
		if(Buffer[i] == ' ')
			continue;
		else
		{
			if(Buffer[i] >= '0' && Buffer[i] <= '9')
				LocalBuffer[j++] = Buffer[i];
			else
				break;
		}
	}

	LocalBuffer[j] = 0;

	if(StrToLong(LocalBuffer,&Rate) > 0)
	{
		if(Rate > 0)
			return(Rate);
	}

	return(0);
}

	/* GetFileSize(STRPTR Name):
	 *
	 *	Simple routine to return the size of a file in
	 *	bytes.
	 */

LONG
GetFileSize(STRPTR Name)
{
	BPTR FileLock;
	LONG FileSize = 0;
	D_S(struct FileInfoBlock,FileInfo);

	if(FileLock = Lock(Name,ACCESS_READ))
	{
		if(Examine(FileLock,FileInfo))
		{
			if(FileInfo->fib_DirEntryType < 0)
				FileSize = FileInfo->fib_Size;
		}

		UnLock(FileLock);
	}

	return(FileSize);
}

	/* PutDimensionTags(struct Window *Reference,LONG Left,LONG Top,LONG Width,LONG Height):
	 *
	 *	Put back the default values for the requesters to open.
	 */

VOID
PutDimensionTags(struct Window *Reference,LONG Left,LONG Top,LONG Width,LONG Height)
{
	if(Config->MiscConfig->RequesterMode != REQUESTERMODE_IGNORE && (!Config->MiscConfig->RequesterWidth || !Config->MiscConfig->RequesterHeight))
	{
		if(!Reference)
			Reference = Window;

		Config->MiscConfig->RequesterLeft	= Left	- Reference->LeftEdge;
		Config->MiscConfig->RequesterTop	= Top	- Reference->TopEdge;
		Config->MiscConfig->RequesterWidth	= Width;
		Config->MiscConfig->RequesterHeight	= Height;
	}
}

	/* GetDimensionTags(struct Window *Reference,struct TagItem *Tags):
	 *
	 *	Fills an array of tagitems with the dimensions of an asl requester
	 *	to be opened.
	 */

struct TagItem *
GetDimensionTags(struct Window *Reference,struct TagItem *Tags)
{
	if(Config->MiscConfig->RequesterMode == REQUESTERMODE_IGNORE || Config->MiscConfig->RequesterWidth < 1 || Config->MiscConfig->RequesterHeight < 1)
	{
		STATIC ULONG Done = TAG_DONE;

		return((struct TagItem *)&Done);
	}
	else
	{
		LONG Left,Top,Width,Height;

		if(!Reference)
			Reference = Window;

		if(Config->MiscConfig->RequesterMode == REQUESTERMODE_CENTRE)
		{
			Left	= Reference->LeftEdge	+ (Reference->Width - Config->MiscConfig->RequesterWidth) / 2;
			Top		= Reference->TopEdge	+ (Reference->Height - Config->MiscConfig->RequesterHeight) / 2;
		}
		else
		{
			Left	= Config->MiscConfig->RequesterLeft	+ Reference->LeftEdge;
			Top		= Config->MiscConfig->RequesterTop	+ Reference->TopEdge;
		}

		Width	= Config->MiscConfig->RequesterWidth;
		Height	= Config->MiscConfig->RequesterHeight;

		Tags[0].ti_Tag	= ASL_LeftEdge;
		Tags[0].ti_Data	= Left;
		Tags[1].ti_Tag	= ASL_TopEdge;
		Tags[1].ti_Data	= Top;
		Tags[2].ti_Tag	= ASL_Width;
		Tags[2].ti_Data	= Width;
		Tags[3].ti_Tag	= ASL_Height;
		Tags[3].ti_Data	= Height;
		Tags[4].ti_Tag	= TAG_DONE;

		return(Tags);
	}
}

	/* ShowRequest(struct Window *Window,STRPTR Text,STRPTR Gadgets,...):
	 *
	 *	Really quite simple varargs version of Intuition's
	 *	EasyRequest requester.
	 */

LONG
ShowRequest(struct Window *Window,STRPTR Text,STRPTR Gadgets,...)
{
	struct EasyStruct Easy;
	va_list VarArgs;
	LONG i,GadgetCount;

	for(i = GadgetCount = 0 ; i < strlen(Gadgets) ; i++)
	{
		if(Gadgets[i] == '|')
			GadgetCount++;
	}

		/* Standard data. */

	Easy.es_StructSize		= sizeof(struct EasyStruct);
	Easy.es_Flags			= NULL;
	Easy.es_Title			= LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
	Easy.es_TextFormat		= Text;
	Easy.es_GadgetFormat	= Gadgets;

	if(GadgetCount)
	{
		LONG Result;

		if(GTLayoutBase)
			LT_LockWindow(Window);

			/* Use the argument array to build the
			 * requester and display it.
			 */

		va_start(VarArgs,Gadgets);
		Result = EasyRequestArgs(Window,&Easy,NULL,(APTR)VarArgs);
		va_end(VarArgs);

		if(GTLayoutBase)
			LT_UnlockWindow(Window);

		return(Result);
	}
	else
	{
		struct Window *ReqWindow;

		if(GTLayoutBase)
			LT_LockWindow(Window);

		va_start(VarArgs,Gadgets);

		if(ReqWindow = BuildEasyRequestArgs(Window,&Easy,IDCMP_RAWKEY,(APTR)VarArgs))
		{
			ULONG	IDCMP;
			LONG	Result;

			FOREVER
			{
				WaitPort(ReqWindow->UserPort);

				IDCMP = NULL;

				Result = SysReqHandler(ReqWindow,&IDCMP,FALSE);

				if(!Result || (Result == -2 && !(IDCMP & IDCMP_RAWKEY)))
					break;
			}

			FreeSysRequest(ReqWindow);
		}

		va_end(VarArgs);

		if(GTLayoutBase)
			LT_UnlockWindow(Window);

		return(0);
	}
}

	/* CloseWindowSafely(struct Window *Window):
	 *
	 *	Close a window freeing all messages pending at
	 *	its user port (taken from example source code
	 *	published once upon a time in Amiga Mail).
	 */

VOID
CloseWindowSafely(struct Window *Window)
{
	Forbid();

	if(Window->UserPort)
	{
		struct Node *Node,*Next;

		for(Node = Window->UserPort->mp_MsgList.lh_Head ; Next = Node->ln_Succ ; Node = Next)
		{
			if(((struct IntuiMessage *)Node)->IDCMPWindow == Window)
			{
				Remove(Node);
				ReplyMsg((struct Message *)Node);
			}
		}

		Window->UserPort = NULL;
	}

	ModifyIDCMP(Window,NULL);

	Permit();

	LT_DeleteWindowLock(Window);

	CloseWindow(Window);
}

	/* DelayTime(LONG Secs,LONG Micros):
	 *
	 *	Delay for a given period of time.
	 */

VOID
DelayTime(LONG Secs,LONG Micros)
{
	StopTime();

	if(Secs || Micros)
	{
		StartTime(Secs,Micros);

		WaitTime();
	}
}

	/* WaitTime():
	 *
	 *	Wait for a pending time request to finish.
	 */

VOID
WaitTime()
{
	if(TimerRunning)
	{
		TimerRunning = FALSE;

		WaitIO((struct IORequest *)TimeRequest);
	}
}

	/* StopTime():
	 *
	 *	Stop the running timer.
	 */

VOID
StopTime()
{
	if(TimerRunning)
	{
		if(!CheckIO((struct IORequest *)TimeRequest))
			AbortIO((struct IORequest *)TimeRequest);

		WaitIO((struct IORequest *)TimeRequest);
	}
}

	/* StartTime(LONG Secs,LONG Micros):
	 *
	 *	Start the timer asynchronously.
	 */

VOID
StartTime(LONG Secs,LONG Micros)
{
	StopTime();

	if(Secs || Micros)
	{
		TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
		TimeRequest->tr_time.tv_secs	= Secs;
		TimeRequest->tr_time.tv_micro	= Micros;

		ClrSignal(SIG_TIMER);

		SendIO((struct IORequest *)TimeRequest);

		TimerRunning = TRUE;
	}
}

	/* GetEnvDOS(STRPTR Name,STRPTR Buffer):
	 *
	 *	Get the contents of a vanilla AmigaDOS environment
	 *	variable.
	 */

BOOL
GetEnvDOS(STRPTR Name,STRPTR Buffer,LONG BufferSize)
{
	if(GetVar(Name,Buffer,256,NULL) >= 0)
		return(TRUE);
	else
	{
		Buffer[0] = 0;

		return(FALSE);
	}
}

	/* SetEnvDOS(STRPTR Name,STRPTR Value):
	 *
	 *	Set the contents of a vanilla AmigaDOS environment
	 *	variable.
	 */

BOOL
SetEnvDOS(STRPTR Name,STRPTR Value)
{
	if(SetVar(Name,Value,-1,GVF_SAVE_VAR))
	{
			/* Ouch! Bug in dos.library V39! GVF_SAVE_VAR doesn't
			 * work unless GVF_GLOBAL_ONLY is set as well.
			 */

/*		if(Kick30)*/
/*			return(TRUE);*/
/*		else*/
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			BOOL Result = FALSE;

			strcpy(LocalBuffer,"ENVARC:");

			if(AddPart(LocalBuffer,FilePart(Name),sizeof(LocalBuffer)))
			{
				BPTR FileHandle;

				if(!(FileHandle = Open(LocalBuffer,MODE_NEWFILE)))
					Result = FALSE;
				else
				{
					LONG Length = strlen(Value);

					if(Write(FileHandle,Value,Length) == Length)
						Result = TRUE;

					Close(FileHandle);
				}
			}

			return(Result);
		}
	}

	return(FALSE);
}

	/* BumpWindow(struct Window *SomeWindow):
	 *
	 *	Bring a window to the front (and shift the screen
	 *	back to its initial position).
	 */

VOID
BumpWindow(struct Window *SomeWindow)
{
	if(SomeWindow)
	{
		if(SomeWindow->WScreen->LeftEdge > 0)
		{
			if(SomeWindow->WScreen->TopEdge > 0)
				MoveScreen(SomeWindow->WScreen,-SomeWindow->WScreen->LeftEdge,-SomeWindow->WScreen->TopEdge);
			else
				MoveScreen(SomeWindow->WScreen,-SomeWindow->WScreen->LeftEdge,0);
		}
		else
		{
			if(SomeWindow->WScreen->TopEdge > 0)
				MoveScreen(SomeWindow->WScreen,0,-SomeWindow->WScreen->TopEdge);
		}

		ScreenToFront(SomeWindow->WScreen);

		ActivateWindow(SomeWindow);
	}
}

	/* PushWindow(struct Window *Window):
	 *
	 *	Push/PopWindow implement a single lifo window stack
	 *	which always updates the window to activate when
	 *	LSHIFT+RSHIFT+RETURN is pressed. This routine will
	 *	push a window on the stack.
	 */

VOID
PushWindow(struct Window *Window)
{
	if(WindowStackPtr < 5)
	{
		WindowStack[WindowStackPtr++] = Window;

		TopWindow = Window;
	}
}

	/* PopWindow():
	 *
	 *	Remove topmost window from window stack.
	 */

VOID
PopWindow()
{
	if(WindowStackPtr > 0)
	{
		WindowStackPtr--;

		if(WindowStackPtr)
			TopWindow = WindowStack[WindowStackPtr - 1];
		else
			TopWindow = Window;
	}
}

	/* SplitFileName():
	 *
	 *	Split a full file name into a file and a drawer name.
	 */

VOID
SplitFileName(STRPTR FullName,STRPTR *FileName,STRPTR DrawerName)
{
	if(FilePart(FullName) == FullName)
	{
		*DrawerName	= 0;
		*FileName	= FullName;
	}
	else
	{
		STRPTR Dummy;

		strcpy(DrawerName,FullName);

		Dummy = PathPart(DrawerName);

		*Dummy = 0;

		*FileName = FilePart(FullName);
	}
}

	/* BackfillRoutine():
	 *
	 *	Window layer backfill routine.
	 */

#ifndef __AROS__
VOID SAVE_DS ASM
BackfillRoutine(REG(a0) struct Hook *UnusedHook,REG(a2) struct RastPort *RPort,REG(a1) LayerMsg *Bounds)
#else
AROS_UFH3(VOID, BackfillRoutine,
 AROS_UFHA(struct Hook *        , UnusedHook , A0),
 AROS_UFHA(struct RastPort *    , RPort, A2),
 AROS_UFHA(LayerMsg *           , Bounds, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	struct RastPort RastPort;

	CopyMem(RPort,&RastPort,sizeof(struct RastPort));
	RastPort.Layer = NULL;

	RPort = &RastPort;

	SetAPen(RPort,Pens[BACKGROUNDPEN]);
	SetDrMd(RPort,JAM2);

	RectFill(RPort,Bounds->Bounds.MinX,Bounds->Bounds.MinY,Bounds->Bounds.MaxX,Bounds->Bounds.MaxY);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

struct NameSegment
{
	UBYTE	String[63],
			Separator;
};

STATIC STRPTR
FindChar(STRPTR Template,LONG c)
{
	while(*Template)
	{
		if(*Template == c)
			return(Template);
		else
			Template++;
	}

	return(NULL);
}

STATIC UBYTE *
GetNameSegment(struct NameSegment *NameSegment,UBYTE *cp,LONG i)
{
	UBYTE *xp = NameSegment[i].String;

	while (*cp && !FindChar("._+-,@~=",*cp))
		*xp++ = *cp++;

	*xp = '\0';

	NameSegment[i].Separator = *cp;

	if (*cp)
		cp++;

	if(NameSegment[i].String[0] || NameSegment[i].Separator)
		return(cp);
	else
		return(NULL);
}

STATIC VOID
CopyNameSegment(struct NameSegment *NameSegment,UBYTE *Destination,LONG Zap)
{
	UBYTE *cp = Destination,*xp;

	for(xp = NameSegment[0].String ; *xp ; )
		*cp++ = *xp++;

	if(NameSegment[0].Separator)
	{
		LONG i;

		*cp++ = NameSegment[0].Separator;

		for(i = Zap + 1 ; ; i++)
		{
			for(xp = NameSegment[i].String ; *xp ; )
				*cp++ = *xp++;

			if(NameSegment[i].Separator)
				*cp++ = NameSegment[i].Separator;
			else
			{
				if(!NameSegment[i].String[0])
					break;
			}
		}
	}

	*cp = 0;
}

	/* ShrinkName():
	 *
	 *	Shrink a file name down to a number of characters, if
	 *	possible preserving the structure of the name. Algorithm
	 *	courtesy of the "shrink" program by Simon Brown,
	 *	Edinburgh University.
	 */

UBYTE *
ShrinkName(UBYTE *Source,UBYTE *Destination,LONG MaxLength,BOOL FixSuffix)
{
	#define MAXSEGS 10

	struct NameSegment NameSegment[MAXSEGS];

	LONG i,NumSegments,SuffixLength,Remainder,Delete,Total,Zap = 0;
	UBYTE *OldPtr = (UBYTE *)Source;

	for(i = 0 ; i < MAXSEGS && (OldPtr = GetNameSegment(NameSegment,OldPtr,i)) ; i++);

	if(i < MAXSEGS)
	{
		NumSegments = i - 1;

		if((NumSegments * 2) < MaxLength)
		{
			SuffixLength = strlen(NameSegment[NumSegments].String);

			if(SuffixLength > MaxLength - NumSegments - 1)
			{
				SuffixLength = MaxLength - (2 * NumSegments) - 1;

				NameSegment[NumSegments].String[SuffixLength - 1] = 0;
			}
			else
			{
				if (SuffixLength + NumSegments > MaxLength-NumSegments-1)
					Zap = NumSegments - (MaxLength-SuffixLength)/2;
			}

			if(NumSegments >= 1)
			{
				for(i = Zap + 1 ; i <= NumSegments ; i++)
				{
					if(NameSegment[i].Separator)
						SuffixLength++;
				}

				if(NameSegment[0].Separator)
					SuffixLength++;

				Remainder = MaxLength - SuffixLength;

				Delete = Remainder / NumSegments;

				Total = SuffixLength;

				for(i = Zap + 1 ; i < NumSegments ; i++)
				{
					NameSegment[i].String[Delete] = 0;

					Total += Delete;
				}

				NameSegment[0].String[MaxLength - Total] = 0;
			}

			CopyNameSegment(NameSegment,Destination,Zap);
		}
		else
			strcpy(Destination,Source);
	}
	else
		strcpy(Destination,Source);

	if(FixSuffix)
	{
		BOOL	GotDot	= FALSE;
		LONG	Len	= strlen(Destination),Dots;

		for(i = Dots = 0 ; i < Len ; i++)
		{
			if(Destination[i] == '.')
				Dots++;
		}

		if(!Dots)
		{
			if(Len < 4)
				strcat(Destination,".___");
			else
				Destination[Len - 4] = '.';
		}

		for(i = Len - 1 ; i >= 0 ; i--)
		{
			if(Destination[i] == '.')
			{
				if(GotDot)
					Destination[i] = '_';
				else
					GotDot = TRUE;
			}

			if(Destination[i] == '\\')
				Destination[i] = '-';
		}
	}

	return(Destination);
}

	/* BuildFontName(STRPTR Destination,STRPTR Name,LONG Size):
	 *
	 *	Build a font name and size string from given
	 *	information (raw name and size).
	 */

VOID
BuildFontName(STRPTR Destination,LONG DestinationSize,STRPTR Name,LONG Size)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	LONG Len;

	Name	= FilePart(Name);
	Len		= strlen(Name);

	if(Len > 5 && !Stricmp(&Name[Len - 5],".font"))
	{
		strcpy(LocalBuffer,Name);

		LocalBuffer[Len - 5] = 0;

		Name = LocalBuffer;
	}

	LimitedSPrintf(DestinationSize,Destination,"%s %ld",Name,Size);
}

	/* FixName(STRPTR Name):
	 *
	 *	Build a correct AmigaDOS filename.
	 */

VOID
FixName(STRPTR Name)
{
	LONG NameLen = strlen(Name),i;

		/* Replace special characters. */

	for(i = 0 ; i < NameLen ; i++)
	{
		switch(Name[i])
		{
			case ' ':

				Name[i] = '_';
				break;

			case ':':

				Name[i] = '.';
				break;

			case '/':

				Name[i] = '\\';
				break;

			case '\"':

				Name[i] = '\'';
				break;
		}
	}

		/* Truncate the name. */

	if(NameLen > 31)
		Name[31] = 0;
}

	/* ShowError(struct Window *Window,LONG Primary,LONG Secondary,STRPTR String):
	 *
	 *	Display an error message, in human readable form if possible.
	 */

VOID
ShowError(struct Window *Window,LONG Primary,LONG Secondary,STRPTR String)
{
	STATIC LONG LocalErrors[][2] =
	{
		ERR_SAVE_ERROR,			MSG_ERR_COULD_NOT_SAVE_FILE_TXT,
		ERR_LOAD_ERROR,			MSG_ERR_COULD_NOT_LOAD_FILE_TXT,
		ERR_OUTDATED,			MSG_ERR_OUTDATED_TXT,
		ERR_EXECUTE_ERROR,		MSG_ERR_COULD_NOT_EXECUTE_PROGRAM_TXT,

		ERR_FILE_NOT_FOUND,		MSG_VERIFY_NO_FILE_TXT,
		ERR_DRAWER_NOT_FOUND,	MSG_VERIFY_DRAWER_NOT_FOUND_TXT,
		ERR_PROGRAM_NOT_FOUND,	MSG_VERIFY_NO_PROGRAM_TXT,
		ERR_NOT_A_FILE,			MSG_VERIFY_DRAWER_NOT_A_FILE_TXT,
		ERR_NOT_A_DRAWER,		MSG_VERIFY_FILE_NOT_A_DRAWER_TXT,

		IFFERR_NOMEM,			MSG_IFFERR_NOMEM_TXT,
		IFFERR_READ,			MSG_IFFERR_READ_TXT,
		IFFERR_WRITE,			MSG_IFFERR_WRITE_TXT,
		IFFERR_SEEK,			MSG_IFFERR_SEEK_TXT,
		IFFERR_MANGLED,			MSG_IFFERR_MANGLED_TXT,
		IFFERR_NOTIFF,			MSG_IFFERR_NOTIFF_TXT,

		0
	};

	UBYTE LocalBuffer1[FAULT_MAX],LocalBuffer2[FAULT_MAX];
	STRPTR PrimaryError,SecondaryError;

	PrimaryError = SecondaryError = NULL;

	if(Primary)
	{
		LONG i;

		for(i = 0 ; LocalErrors[i][0] ; i++)
		{
			if(LocalErrors[i][0] == Primary)
			{
				PrimaryError = LocaleString(LocalErrors[i][1]);

				break;
			}
		}

		if(!PrimaryError)
		{
			if(Fault(Primary,NULL,LocalBuffer1,sizeof(LocalBuffer1)))
				PrimaryError = LocalBuffer1;
		}
	}

	if(Secondary)
	{
		LONG i;

		for(i = 0 ; LocalErrors[i][0] ; i++)
		{
			if(LocalErrors[i][0] == Secondary)
			{
				SecondaryError = LocaleString(LocalErrors[i][1]);

				break;
			}
		}

		if(!SecondaryError)
		{
			if(Fault(Secondary,NULL,LocalBuffer2,sizeof(LocalBuffer2)))
				SecondaryError = LocalBuffer2;
		}
	}

	if(PrimaryError)
	{
		if(String)
			ShowRequest(Window,PrimaryError,LocaleString(MSG_GLOBAL_CONTINUE_TXT),String,SecondaryError);
		else
			ShowRequest(Window,PrimaryError,LocaleString(MSG_GLOBAL_CONTINUE_TXT),SecondaryError);
	}
}

struct List *
BuildModeList(LONG *Index,ULONG DisplayMode,MODEFILTER ModeFilter,APTR UserData)
{
	struct List	*List;
	LONG		 Count = 0;

	*Index = 0;

	if(List = CreateList())
	{
		struct DisplayInfo	DisplayInfo;
		ULONG				SomeMode = INVALID_ID;

		while((SomeMode = NextDisplayInfo(SomeMode)) != INVALID_ID)
		{
			if(GetDisplayInfoData(NULL,(APTR)&DisplayInfo,sizeof(struct DisplayInfo),DTAG_DISP,SomeMode))
			{
				if((DisplayInfo.PropertyFlags & DIPF_IS_WB) && !(DisplayInfo.NotAvailable & ~DI_AVAIL_NOTWITHGENLOCK))
				{
					UBYTE ModeNameBuffer[DISPLAYNAMELEN + 1];
					struct ModeNode	*ModeNode;

					if(ModeFilter)
					{
						if(!(*ModeFilter)(SomeMode,UserData))
							continue;
					}

					GetModeName(SomeMode,ModeNameBuffer,sizeof(ModeNameBuffer));

					if(ModeNode = (struct ModeNode *)AllocVecPooled(sizeof(struct ModeNode) + strlen(ModeNameBuffer) + 1,MEMF_ANY))
					{
						strcpy(ModeNode->Node.ln_Name = (STRPTR)(ModeNode + 1),ModeNameBuffer);

						ModeNode->DisplayID = SomeMode;

						AddTail(List,(struct Node *)ModeNode);

						Count++;
					}
				}
			}
		}
	}

	if(Count)
	{
		struct ModeNode	*Node,*Next;

		for(Node = (struct ModeNode *)List->lh_Head ; Node->Node.ln_Succ ; Node = (struct ModeNode *)Node->Node.ln_Succ)
		{
			if(!(Node->DisplayID & MONITOR_ID_MASK))
			{
				for(Next = (struct ModeNode *)List->lh_Head ; Next->Node.ln_Succ ; Next = (struct ModeNode *)Next->Node.ln_Succ)
				{
					if((Node->DisplayID & ~MONITOR_ID_MASK) == (Next->DisplayID & ~MONITOR_ID_MASK) && Next != Node)
						Node->DisplayID = INVALID_ID;
				}
			}

			for(Next = (struct ModeNode *)List->lh_Head ; Next->Node.ln_Succ ; Next = (struct ModeNode *)Next->Node.ln_Succ)
			{
				if(Node->DisplayID == Next->DisplayID && Next != Node)
					Next->DisplayID = INVALID_ID;
			}
		}

		for(Node = (struct ModeNode *)List->lh_Head ; Next = (struct ModeNode *)Node->Node.ln_Succ ; Node = Next)
		{
			if(Node->DisplayID == INVALID_ID)
			{
				Count--;

				Remove((struct Node *)Node);

				FreeVecPooled(Node);
			}
		}
	}

	if(Count)
	{
		struct ModeNode	*Node,*Next;

		for(Count = 0, Node = (struct ModeNode *)List->lh_Head ; Next = (struct ModeNode *)Node->Node.ln_Succ ; Node = Next)
		{
			if(Node->DisplayID == DisplayMode)
			{
				*Index = Count;

				break;
			}
		}

		return(List);
	}
	else
	{
		DeleteList(List);

		return(NULL);
	}
}

	/* IsAssign(STRPTR Name):
	 *
	 *	Does a name refer to an assignment?
	 */

BOOL
IsAssign(STRPTR Name)
{
	LONG NameLen	= strlen(Name) - 1;
	BOOL Result		= FALSE;

		/* Does it end with a colon? */

	if(Name[NameLen] == ':')
	{
		struct DosList *DosList;
		STRPTR AssignName;

			/* Lock the list of assignments for reading. */

		DosList = LockDosList(LDF_ASSIGNS | LDF_READ);

			/* Scan the list... */

		while(DosList = NextDosEntry(DosList,LDF_ASSIGNS))
		{
				/* Convert the name from icky
				 * BCPL to `C' style string.
				 */

#warning Deactivated to make it compile for AROS.
#ifndef __AROS__
			AssignName = (STRPTR)BADDR(DosList->dol_Name);

				/* Does the name length match? */

			if(AssignName[0] == NameLen)
			{
					/* Does the name itself match? */

				if(!Strnicmp(&AssignName[1],Name,NameLen))
				{
					Result = TRUE;

					break;
				}
			}
#endif
		}

			/* Unlock the list of assignments. */

		UnLockDosList(LDF_ASSIGNS | LDF_READ);
	}

		/* Return the result. */

	return(Result);
}

	/* LockInAssign(BPTR TheLock,STRPTR TheAssignment):
	 *
	 *	Check if a file lock is part of an assignment.
	 */

BOOL
LockInAssign(BPTR TheLock,STRPTR TheAssignment)
{
	struct DevProc	*DevProc		= NULL;
	struct MsgPort	*FileSysTask	= GetFileSysTask();
	BOOL			 Result			= FALSE;


		/* Loop until all assignments are
		 * processed.
		 */

	do
	{
			/* Get the default filesystem task
			 * in case we stumble upon NULL
			 * directory locks.
			 */

		if(DevProc = GetDeviceProc(TheAssignment,DevProc))
		{
				/* Set the default filesystem task. */

			SetFileSysTask(DevProc->dvp_Port);

				/* Is the lock on the list? */

			if(SameLock(DevProc->dvp_Lock,TheLock) == LOCK_SAME)
				Result = TRUE;
		}
		else
			break;
	}
	while(DevProc && (DevProc->dvp_Flags & DVPF_ASSIGN) && !Result);

		/* Reset the default filesystem task. */

	SetFileSysTask(FileSysTask);

		/* Free device process data. */

	if(DevProc)
		FreeDeviceProc(DevProc);

	return(Result);
}

	/* DeleteBitMap(struct BitMap *BitMap):
	 *
	 *	Delete a bitmap created by CreateBitMap().
	 */

VOID
DeleteBitMap(struct BitMap *BitMap)
{
	if(BitMap)
	{
		WaitBlit();

		if(Kick30)
			FreeBitMap(BitMap);
		else
		{
			LONG i,*Sizes,Width,Height;

			Sizes = &((LONG *)BitMap)[-2];

			Width = Sizes[0];
			Height = Sizes[1];

			for(i = 0 ; i < BitMap->Depth ; i++)
				FreeRaster(BitMap->Planes[i],Width,Height);

			FreeVecPooled(Sizes);
		}
	}
}

	/* CreateBitMap(ULONG Width,ULONG Height,ULONG Depth,ULONG Flags,struct BitMap *Friend):
	 *
	 *	Create a new bitmap with given properties.
	 */

struct BitMap *
CreateBitMap(ULONG Width,ULONG Height,ULONG Depth,ULONG Flags,struct BitMap *Friend)
{
	if(Kick30)
		return(AllocBitMap(Width,Height,Depth,Flags,Friend));
	else
	{
		struct BitMap	*BitMap;
		LONG			 Plus;
		LONG			*Sizes;

		if(Depth > 8)
			Plus = (Depth - 8) * sizeof(PLANEPTR);
		else
			Plus = 0;

		if(!(Sizes = (LONG *)AllocVecPooled(sizeof(LONG) * 2 + sizeof(struct BitMap) + Plus,MEMF_CLEAR)))
			BitMap = NULL;
		else
		{
			LONG i;

			BitMap = (struct BitMap *)&Sizes[2];

			InitBitMap(BitMap,Depth,Width,Height);

			for(i = 0 ; i < BitMap->Depth ; i++)
			{
				if(!(BitMap->Planes[i] = AllocRaster(Width,Height)))
				{
					LONG j;

					for(j = 0 ; j < i ; j++)
						FreeRaster(BitMap->Planes[j],Width,Height);

					FreeVecPooled(Sizes);

					return(NULL);
				}
			}

			if(Flags & BMF_CLEAR)
				BltBitMap(BitMap,0,0,BitMap,0,0,Width,Height,0x00,(1 << Depth) - 1,NULL);

			Sizes[0] = Width;
			Sizes[1] = Height;
		}

		return(BitMap);
	}
}

	/* LaunchWorkCommon(LaunchMsg *Startup,BOOL Synchronous):
	 *
	 *	Does the final launch & cleanup work with the
	 *	startup packed provided.
	 */

STATIC BOOL
LaunchWorkCommon(LaunchMsg *Startup,BOOL Synchronous)
{
		/* Did we succeed in creating the startup message? */

	if(Startup)
	{
		LONG Result;

			/* Launch the program. */

		Result = LaunchSomething(Config->MiscConfig->SuppressOutput ? (STRPTR)"NIL:" : Config->MiscConfig->WindowName,Synchronous,Startup);

			/* And return the result. */

		return((BOOL)(Result == 0));
	}
	else
		return(FALSE);
}

	/* RexxLaunchCleanup(LaunchMsg *Startup):
	 *
	 *	Displays an error message for the ARexx script
	 *	to be launched.
	 */

STATIC VOID
LaunchRexxCleanup(LaunchMsg *Startup)
{
	if(Startup->Result != 0)
		Printf(LocaleString(MSG_TERMAUX_COMMAND_HAS_TERMINATED_TXT),Startup->Command,Startup->Result,Startup->Result2);
}

	/* LaunchRexxCommon(STRPTR Command):
	 *
	 *	Launch an ARexx command. This routine is always
	 *	asynchronous.
	 */

BOOL
LaunchRexxAsync(STRPTR Command)
{
	LaunchMsg *Startup;

		/* Skip leading blanks. */

	while(*Command == ' ' || *Command == '\t')
		Command++;

		/* Create the launch startup message. This will copy the
		 * command string.
		 */

	Startup = CreateRexxCmdLaunchMsg(Command,NULL,LaunchRexxCleanup);

		/* Do the work. */

	return((BOOL)(LaunchWorkCommon(Startup,FALSE)));
}

	/* LaunchCommandCommon(STRPTR Command, BOOL Synchronous):
	 *
	 *	Launch a command, try to figure out if it's an ARexx
	 *	command, a plain batch file or a program and act
	 *	accordingly.
	 */

STATIC BOOL
LaunchCommandCommon(STRPTR Command, BOOL Synchronous)
{
	STRPTR		 OriginalCommand;
	UBYTE		 NameBuffer[MAX_FILENAME_LENGTH];
	BOOL		 IsScript;
	BPTR		 CommandFile;
	BPTR		 FileLock;
	LONG		 i;
	LaunchMsg	*Startup;

		/* Chop off the arguments. */

	while(*Command == ' ' || *Command == '\t')
		Command++;

	OriginalCommand = Command;

	CopyMem(Command,NameBuffer,sizeof(NameBuffer) - 1);

	NameBuffer[sizeof(NameBuffer) - 1] = 0;

	for(i = 0 ; i < sizeof(NameBuffer) ; i++)
	{
		if(NameBuffer[i] == ' ' || NameBuffer[i] == '\t')
		{
			NameBuffer[i] = 0;
			break;
		}
	}

		/* Now do something useful. Check if the command
		 * could be an ARexx script.
		 */

	if(CommandFile = Open(NameBuffer,MODE_OLDFILE))
	{
		UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
		BOOL IsRexx = FALSE;
		LONG Len;

			/* 256 bytes may be not be enough, but then
			 * we're only guessing.
			 */

		if((Len = Read(CommandFile,LocalBuffer,sizeof(LocalBuffer))) > 0)
		{
			UBYTE c;

			for(i = 0 ; i < Len - 1 ; i++)
			{
				c = LocalBuffer[i];

					/* Stop on invalid characters. */

				if((c < ' ' && c != '\r' && c != '\n' && c != '\a') || (c >= 127 && c < 160))
					break;
				else
				{
						/* Looks like the typical
						 * introductory comment line.
						 */

					if(c == '/' && LocalBuffer[i + 1] == '*')
					{
						IsRexx = TRUE;
						break;
					}
				}
			}
		}

		Close(CommandFile);

			/* If it's a Rexx script, launch it. */

		if(IsRexx)
			return(LaunchRexxAsync(Command));
	}

		/* Ok, second check. Does it have the script bit set? */

	IsScript = FALSE;

	if(FileLock = Lock(NameBuffer,ACCESS_READ))
	{
		D_S(struct FileInfoBlock,FileInfo);

		if(Examine(FileLock,FileInfo))
		{
			if(FileInfo->fib_Protection & FIBF_SCRIPT)
				IsScript = TRUE;
		}

		UnLock(FileLock);
	}

		/* If it's a script command, prepend the "Execute " command. */

	if(IsScript)
	{
			/* Make room for the new command. */

		if(!(Command = AllocVecPooled(strlen("Execute ") + strlen(OriginalCommand) + 1,MEMF_ANY)))
			return(FALSE);
		else
		{
			strcpy(Command,"Execute ");
			strcat(Command,OriginalCommand);
		}
	}

		/* Create the launch startup message. This will copy the
		 * command string.
		 */

	Startup = CreateProgramLaunchMsg(Command,NULL);

		/* If we had to make a copy of the command name, release the memory. */

	if(Command != OriginalCommand)
		FreeVecPooled(Command);

		/* Do the work. */

	return((BOOL)(LaunchWorkCommon(Startup,Synchronous)));
}

	/* LaunchCommand(STRPTR Command):
	 *
	 *	Launches a command, synchronous version.
	 */

BOOL
LaunchCommand(STRPTR Command)
{
	return(LaunchCommandCommon(Command,TRUE));
}

	/* LaunchCommandAsync(STRPTR Command):
	 *
	 *	Launches a command, asynchronous version.
	 */

BOOL
LaunchCommandAsync(STRPTR Command)
{
	return(LaunchCommandCommon(Command,FALSE));
}

	/* LaunchProcess(STRPTR Name,VOID (*Entry)(VOID),BPTR Stream):
	 *
	 *	Launch a new process from given entry point, with given
	 *	name and I/O stream.
	 */

struct Process *
LaunchProcess(STRPTR Name,VOID (*Entry)(VOID),BPTR Stream)
{
	struct MsgPort *ConsoleTask;

#warning Deactivated to make it compile for AROS.
#ifndef __AROS__
	if(Stream && GoodStream(Stream))
		ConsoleTask = ((struct FileHandle *)BADDR(Stream))->fh_Type;
	else
#endif
		ConsoleTask = NULL;

	return(CreateNewProcTags(
		NP_Entry,		Entry,
		NP_StackSize,	8000,
		NP_Name,		Name,
		NP_Cli,			TRUE,
		NP_ConsoleTask,	ConsoleTask,

		ConsoleTask ? TAG_IGNORE : TAG_DONE,0,

		NP_Output,	ConsoleTask ? NULL : Stream,

		ConsoleTask ? NP_Input : TAG_IGNORE,Stream,
	TAG_DONE));
}

	/* String2Boolean(STRPTR String):
	 *
	 *	Compare string contents with table contents,
	 *	map the string to a boolean value.
	 */

BOOL
String2Boolean(STRPTR String)
{
	STATIC STRPTR TrueOptions[] =
	{
		"ON",
		"TRUE",
		"T",
		"YES",
		"Y",
		"1"
	};

	LONG i;

	for(i = 0 ; i < NUM_ELEMENTS(TrueOptions) ; i++)
	{
		if(!Stricmp(String,TrueOptions[i]))
			return(TRUE);
	}

	return(FALSE);
}

	/* SendMessageGetReply(struct MsgPort *Port,struct Message *Message):
	 *
	 *	Send a message to a given MsgPort and wait for
	 *	the reply.
	 */

VOID
SendMessageGetReply(struct MsgPort *Port,struct Message *Message)
{
	struct MsgPort LocalPort;
	struct MsgPort *ReplyPort;

	ReplyPort = Message->mn_ReplyPort;

	InitSinglePort(&LocalPort);

	Message->mn_ReplyPort = &LocalPort;

	SetSignal(0,SIGF_SINGLE);

	PutMsg(Port,Message);
	WaitPort(&LocalPort);
	GetMsg(&LocalPort);

	Message->mn_ReplyPort = ReplyPort;
}

	/* SetOnlineState(BOOL IsOnline):
	 *
	 *	Set the current online state.
	 */

VOID
SetOnlineState(BOOL IsOnline)
{
	ObtainSemaphore(&OnlineSemaphore);

	WasOnline	= Online;
	Online		= IsOnline;

	if(WasOnline && !Online)
		ActivateJob(MainJobQueue,OnlineCleanupJob);

	ReleaseSemaphore(&OnlineSemaphore);
}

	/* SwapMem(APTR FromPtr,APTR ToPtr,LONG Size):
	 *
	 *	Exchange the contents of two memory regions.
	 */

VOID
SwapMem(APTR FromPtr,APTR ToPtr,LONG Size)
{
	UBYTE *From,*To,c;

	From	= FromPtr;
	To		= ToPtr;

	while(Size--)
	{
		c		= *From;
		*From++	= *To;
		*To++	= c;
	}
}

	/* CloseIFFClip(struct IFFHandle *Handle):
	 *
	 *	Closes the clipboard opened by OpenIFFClip.
	 */

BOOL
CloseIFFClip(struct IFFHandle *Handle)
{
	if(Handle)
	{
		CloseIFF(Handle);
		CloseClipboard((struct ClipboardHandle *)Handle->iff_Stream);
		FreeIFF(Handle);
	}

	return(TRUE);
}

	/* OpenIFFClip(LONG Unit,LONG Mode):
	 *
	 *	Open the clipboard for reading or writing. This
	 *	routine handles all the initizalizations and
	 *	allocation and returns an IFFHandle ready
	 *	to be used.
	 */

struct IFFHandle *
OpenIFFClip(LONG Unit,LONG Mode)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(Handle = AllocIFF())
	{
		if(Handle->iff_Stream = (ULONG)OpenClipboard(Unit))
		{
			InitIFFasClip(Handle);

			if(!(Error = OpenIFF(Handle,(Mode == MODE_NEWFILE) ? IFFF_WRITE : IFFF_READ)))
				return(Handle);

			CloseClipboard((struct ClipboardHandle *)Handle->iff_Stream);
		}
		else
			Error = ERROR_NO_FREE_STORE;

		FreeIFF(Handle);
	}
	else
		Error = ERROR_NO_FREE_STORE;

	SetIoErr(Error);

	return(FALSE);
}

	/* CloseIFFStream(struct IFFHandle *Handle):
	 *
	 *	Closes an IFF file opened by OpenIFFStream.
	 */

BOOL
CloseIFFStream(struct IFFHandle *Handle)
{
	if(Handle)
	{
		BOOL Result;

		CloseIFF(Handle);
		Result = Close(Handle->iff_Stream);
		FreeIFF(Handle);

		return(Result);
	}
	else
		return(TRUE);
}

	/* OpenIFFStream(STRPTR Name,LONG Mode):
	 *
	 *	Open an IFF file for reading or writing. This
	 *	routine handles all the initizalizations and
	 *	allocation and returns an IFFHandle ready
	 *	to be used.
	 */

struct IFFHandle *
OpenIFFStream(STRPTR Name,LONG Mode)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(Handle = AllocIFF())
	{
		if(Handle->iff_Stream = Open(Name,Mode))
		{
			InitIFFasDOS(Handle);

			if(!(Error = OpenIFF(Handle,(Mode == MODE_NEWFILE) ? IFFF_WRITE : IFFF_READ)))
				return(Handle);

			Close(Handle->iff_Stream);

			if(Mode == MODE_NEWFILE)
				DeleteFile(Name);
		}
		else
			Error = IoErr();

		FreeIFF(Handle);
	}
	else
		Error = ERROR_NO_FREE_STORE;

	SetIoErr(Error);

	return(FALSE);
}

	/* InitHook(struct Hook *Hook,HOOKFUNC Func,APTR Data):
	 *
	 *	Set up a callback hook function.
	 */

VOID
InitHook(struct Hook *Hook,HOOKFUNC Func,APTR Data)
{
#ifdef USE_GLUE
	Hook->h_Entry		= HookEntry;
	Hook->h_SubEntry	= Func;
	Hook->h_Data		= Data;
#else
	Hook->h_Entry		= Func;
	Hook->h_Data		= Data;
#endif	/* USE_GLUE */
}

	/* FitText(struct RastPort *RPort,UWORD Width,STRPTR String,LONG Len):
	 *
	 *	Check how many character fit into Width pixels.
	 */

ULONG
FitText(struct RastPort *RPort,UWORD Width,STRPTR String,LONG Len)
{
	struct TextExtent Extent;

	if(Len == -1)
		Len = strlen(String);

	return(TextFit(RPort,String,Len,&Extent,NULL,1,Width,32767));
}

	/* DispatchRexxDialMsgList(BOOL Ok):
	 *
	 *	Dispatch all queued dial requests.
	 */

VOID
DispatchRexxDialMsgList(BOOL Ok)
{
	struct RexxMsg *DialMsg;
	LONG Result;

	if(Ok)
		Result = RC_OK;
	else
		Result = RC_WARN;

	while(DialMsg = (struct RexxMsg *)RemHead(&RexxDialMsgList))
	{
		DialMsg->rm_Result1 = Result;
		DialMsg->rm_Result2 = 0;

		ReplyMsg((struct Message *)DialMsg);
	}
}

	/* CancelZModem():
	 *
	 *	Send the zmodem cancel sequence.
	 */

VOID
CancelZModem()
{
	if(UsesZModem)
		DoSerialWrite(ZModemCancel,20);
}

	/* WaitForHandshake():
	 *
	 *	Clears the handshaking signal, then waits for it.
	 *	Must be called while in Forbid().
	 */

VOID
WaitForHandshake()
{
	SetSignal(0,SIG_HANDSHAKE);
	Wait(SIG_HANDSHAKE);
}

	/* ShakeHands(struct Task *Notify,ULONG NotifyMask):
	 *
	 *	Send a signal to a Task and wait for it to
	 *	reply with the handshake signal.
	 */

VOID
ShakeHands(struct Task *Notify,ULONG NotifyMask)
{
	if(Notify)
	{
		Forbid();

		Signal(Notify,NotifyMask);

		WaitForHandshake();

		Permit();
	}
}

	/* GetViewPortExtra(struct ViewPort *ViewPort):
	 *
	 *	Gets the extra information associated with a viewport.
	 */

struct ViewPortExtra *
GetViewPortExtra(struct ViewPort *ViewPort)
{
	struct TagItem Tags[2] = { VTAG_VIEWPORTEXTRA_GET, NULL, TAG_DONE };

	if(!VideoControl(ViewPort->ColorMap,Tags))
		return((struct ViewPortExtra *)Tags[0].ti_Data);
	else
		return(NULL);
}

	/* OpenToAppend(STRPTR Name,BOOL *Created):
	 *
	 *	Open a plain AmigaDOS file to append data to it. If a new file
	 *	is created this routine will indicate this by setting the
	 *	boolean variable pointed to by "Created" to TRUE.
	 */

BPTR
OpenToAppend(STRPTR Name,BOOL *Created)
{
	BPTR FileHandle;
	BPTR FileLock;

	if(FileLock = Lock(Name,EXCLUSIVE_LOCK))
	{
		if(!(FileHandle = OpenFromLock(FileLock)))
		{
			UnLock(FileLock);

			FileHandle = Open(Name,MODE_READWRITE);
		}

		if(FileHandle)
		{
			if(Seek(FileHandle,0,OFFSET_END) == -1)
			{
				LONG Error;

				Error = IoErr();

				Close(FileHandle);

				SetIoErr(Error);

				return(NULL);
			}
		}

		if(Created)
			*Created = FALSE;
	}
	else
	{
		if(FileHandle = Open(Name,MODE_NEWFILE))
		{
			if(Created)
				*Created = TRUE;
		}
	}

	return(FileHandle);
}

	/* PushStatus(WORD NewStatus):
	 *
	 *	Remember the current status and set the new one.
	 */

VOID
PushStatus(WORD NewStatus)
{
	ObtainSemaphore(&StatusSemaphore);

	if(StatusStackPointer < 10)
		StatusStack[StatusStackPointer++] = NewStatus;

	ReleaseSemaphore(&StatusSemaphore);
}

	/* PopStatus():
	 *
	 *	Return to the old status value.
	 */

VOID
PopStatus()
{
	ObtainSemaphore(&StatusSemaphore);

	if(StatusStackPointer > 0)
		StatusStackPointer--;

	ReleaseSemaphore(&StatusSemaphore);
}

	/* GetStatus():
	 *
	 *	Returns the current status.
	 */

WORD
GetStatus()
{
	WORD Status;

	SafeObtainSemaphoreShared(&StatusSemaphore);

	if(StatusStackPointer > 0)
		Status = StatusStack[StatusStackPointer - 1];
	else
		Status = STATUS_READY;

	ReleaseSemaphore(&StatusSemaphore);

	if(Status == STATUS_READY && Get_xOFF())
		Status = STATUS_HOLDING;

	return(Status);
}

	/* Clear_xOFF():
	 *
	 *	Clears the xOFF stop marker.
	 */

VOID
Clear_xOFF()
{
	ObtainSemaphore(&xONxOFF_Semaphore);

	xOFF_Enabled = FALSE;

	UpdateSerialJob();

	ReleaseSemaphore(&xONxOFF_Semaphore);
}

	/* Set_xOFF():
	 *
	 *	Sets the xOFF stop marker unless access to the
	 *	marker is forbidden.
	 */

VOID
Set_xOFF()
{
	ObtainSemaphore(&xONxOFF_Semaphore);

	if(xONxOFF_Lock == 0)
		xOFF_Enabled = TRUE;

	UpdateSerialJob();

	ReleaseSemaphore(&xONxOFF_Semaphore);
}

	/* Get_xOFF():
	 *
	 *	Queries the current state of the xOFF marker.
	 */

BOOL
Get_xOFF()
{
	BOOL Result;

	SafeObtainSemaphoreShared(&xONxOFF_Semaphore);

	Result = xOFF_Enabled;

	ReleaseSemaphore(&xONxOFF_Semaphore);

	return(Result);
}

	/* Lock_xOFF():
	 *
	 *	Locks the current xOFF marker state.
	 */

VOID
Lock_xOFF()
{
	ObtainSemaphore(&xONxOFF_Semaphore);

	xONxOFF_Lock++;

	ReleaseSemaphore(&xONxOFF_Semaphore);
}

	/* Unlock_xOFF():
	 *
	 *	Unlocks the current xOFF marker state.
	 */

VOID
Unlock_xOFF()
{
	ObtainSemaphore(&xONxOFF_Semaphore);

	if(xONxOFF_Lock > 0)
		xONxOFF_Lock--;

	ReleaseSemaphore(&xONxOFF_Semaphore);
}

	/* MoveListViewNode():
	 *
	 *	Move the node of a list view somewhere.
	 */

VOID
MoveListViewNode(LayoutHandle *Handle,struct List *List,LONG ListID,struct Node *Node,LONG *Offset,LONG How)
{
	LT_SetAttributes(Handle,ListID,
		GTLV_Labels,	~0,
	TAG_DONE);

	MoveNode(List,Node,How);

	*Offset = GetNodeOffset(Node,List);

	LT_SetAttributes(Handle,ListID,
		GTLV_Labels,	List,
		LALV_Selected,	*Offset,
	TAG_DONE);
}

	/* StartProcessWaitForHandshake():
	 *
	 *	Create a new process and wait for the handshake
	 *	signal.
	 */

struct Process *
StartProcessWaitForHandshake(STRPTR Name,TASKENTRY Entry,...)
{
	struct Process *Process;
	va_list Args;

	va_start(Args,Entry);

	Forbid();

	if(Process = CreateNewProcTags(
		NP_Name,	Name,
		NP_Entry,	Entry,
	TAG_MORE,Args))
		WaitForHandshake();

	Permit();

	va_end(Args);

	return(Process);
}

	/* LocalGetCurrentDirName(STRPTR Buffer,LONG BufferSize):
	 *
	 *	Obtains the name of the "current" directory of the
	 *	calling process, regardless of the current directory
	 *	of the Shell environment the process may be running in.
	 */

BOOL
LocalGetCurrentDirName(STRPTR Buffer,LONG BufferSize)
{
	return((BOOL)NameFromLock(((struct Process *)FindTask(NULL))->pr_CurrentDir,Buffer,BufferSize));
}

	/* SafeObtainSemaphoreShared():
	 *
	 *	This is for backwards compatibility with Kickstart 2.04 and
	 *	avoids a deadlock when trying to get a shared lock on
	 *	a semaphore already held in exclusive mode by the same Task.
	 */

VOID
SafeObtainSemaphoreShared(struct SignalSemaphore *Semaphore)
{
		/* Do it right with Kickstart 3.x. */

	if(Kick30)
		ObtainSemaphoreShared(Semaphore);
	else
	{
			/* Try to get the shared semaphore */

		if(!AttemptSemaphoreShared(Semaphore))
		{
				/* Check if we can get the exclusive version */

			if(!AttemptSemaphore(Semaphore))
			{
					/* Oh well, wait for the shared lock */

				ObtainSemaphoreShared(Semaphore);
			}
		}
	}
}

	/* LocalCreateTask(STRPTR Name,BYTE Priority,TASKENTRY Entry,ULONG StackSize,LONG NumArgs,...):
	 *
	 *	Special version of CreateTask() that allows for arguments to be passed to
	 *	the Task to be created.
	 */

struct Task *
LocalCreateTask(STRPTR Name,LONG Priority,TASKENTRY Entry,ULONG StackSize,LONG NumArgs,...)
{
		/* A MemList with an additional MemEntry following it. */

	struct FatMemList
	{
		struct MemList	MemList;
		struct MemEntry	MemEntry;
	};
	struct MemList *LocalMemList,*MemList;
	struct FatMemList FatMemList;
	struct Task *Task;


		/* Limit the Task priority to legal values. */

	if(Priority < -128)
		Priority = -128;
	else
	{
		if(Priority > 127)
			Priority = 127;
	}

		/* Clear the result. */

	Task = NULL;

		/* Round the stack size to a multiple of four. */

	StackSize = (StackSize + 3) & ~3;
#ifdef __AROS__
    	if (StackSize < AROS_STACKSIZE) StackSize = AROS_STACKSIZE;
#endif

		/* Reset the contents of the local memory allocation request. */

	memset(&FatMemList,0,sizeof(FatMemList));
	LocalMemList = &FatMemList.MemList;

		/* Set up the memory allocation request; we want room for
		 * the Task structure and the requested stack size plus
		 * any arguments to pass to the new Task.
		 */

	LocalMemList->ml_NumEntries			= 2;
	LocalMemList->ml_ME[0].me_Reqs		= MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR;
	LocalMemList->ml_ME[0].me_Length	= sizeof(struct Task);
	LocalMemList->ml_ME[1].me_Reqs		= MEMF_ANY|MEMF_CLEAR;
	LocalMemList->ml_ME[1].me_Length	= StackSize + NumArgs * sizeof(LONG);

		/* Try the allocation. */

#ifdef __AROS__
    	if(NewAllocEntry(LocalMemList, &MemList, NULL))
#else
	if(MemList = AllocEntry(LocalMemList))
#endif
	{
	#ifndef __AROS__
			/* Check if the allocation did work. */
    	
		if((ULONG)MemList & 0x80000000)
		{
				/* Strip the failure bit. */

			MemList = (struct MemList *)((ULONG)MemList & ~0x80000000);
		}
		else
	#endif
		{
				/* So we got the memory for the Task. */

			Task = (struct Task *)MemList->ml_ME[0].me_Addr;

				/* Initialize the Task data structure. We will not
				 * copy the name to a separate buffer, it is assumed
				 * that the name pointer will remain valid for the
				 * entire life time of this Task.
				 */

			Task->tc_Node.ln_Pri	= Priority;
			Task->tc_Node.ln_Type	= NT_TASK;
			Task->tc_Node.ln_Name	= Name;

			Task->tc_SPLower		= MemList->ml_ME[1].me_Addr;
			Task->tc_SPUpper		= (APTR)((ULONG)Task->tc_SPLower + MemList->ml_ME[1].me_Length);
			Task->tc_SPReg			= (APTR)((ULONG)Task->tc_SPUpper - NumArgs * sizeof(LONG));

				/* Add the memory to be released when the Task exits. */

			NewList(&Task->tc_MemEntry);
			AddTail(&Task->tc_MemEntry,(struct Node *)MemList);

				/* If there are any arguments to pass to the Task,
				 * copy them onto the stack.
				 */

			if(NumArgs > 0)
			{
				va_list VarArgs;

				va_start(VarArgs,NumArgs);
				CopyMem(VarArgs,Task->tc_SPReg,NumArgs * sizeof(LONG));
				va_end(VarArgs);
			}

				/* Try to launch the Task... */
#ifdef AROS_BUG_FIXED
			Task = AddTask(Task,(APTR)Entry,NULL);
#else
			if (0 == NumArgs) {
				kprintf("AROS-term: Launched task with name '%s'!\n",Name);
				Task = AddTask(Task,(APTR)Entry,NULL);
			} else {
				kprintf("AROS-term: Supposed to launch task with name '%s', but cannot do it!\n",Name);
				Task = NULL;
			}
#endif
		}

			/* Check if the Task could be launched and clean
			 * up if it couldn't.
			 */

		if(Task == NULL && MemList != NULL)
		    FreeEntry(MemList);
	}

	return(Task);
}

	/* ChangeWindowPtr(APTR *Old,APTR New):
	 *
	 *	Change the pr_WindowPtr entry and return the old value.
	 */

VOID
ChangeWindowPtr(APTR *Old,APTR New)
{
	struct Process *CurrentProcess;

	CurrentProcess = (struct Process *)FindTask(NULL);

	*Old = CurrentProcess->pr_WindowPtr;

	CurrentProcess->pr_WindowPtr = New;
}

	/* RestoreWindowPtr(APTR Old):
	 *
	 *	Restore the pr_WindowPtr to whatever it was poiting to.
	 */

VOID
RestoreWindowPtr(APTR Old)
{
	((struct Process *)FindTask(NULL))->pr_WindowPtr = Old;
}

	/* DisableDOSRequesters(APTR *WindowPtr):
	 *
	 *	Turn off the AmigaDOS requesters.
	 */

VOID
DisableDOSRequesters(APTR *WindowPtr)
{
	ChangeWindowPtr(WindowPtr,(APTR)-1);
}

	/* EnableDOSRequesters(APTR WindowPtr):
	 *
	 *	Turn the AmigaDOS requesters back on.
	 */

VOID
EnableDOSRequesters(APTR WindowPtr)
{
	RestoreWindowPtr(WindowPtr);
}

	/* SpeechSynthesizerAvailable():
	 *
	 *	Checks if the speech synthesizer is available for use.
	 */

BOOL
SpeechSynthesizerAvailable()
{
	struct Library *LocalTranslatorBase;
	BOOL SpeechAvailable;

	SpeechAvailable = FALSE;

	if(LocalTranslatorBase = OpenLibrary("translator.library",0))
	{
#warning Deactivated to make it compile for AROS.
#ifndef __AROS__
		struct narrator_rb NarratorRequest;

		memset(&NarratorRequest,0,sizeof(NarratorRequest));
		NarratorRequest.message.io_Message.mn_Length = sizeof(NarratorRequest);

		if(!OpenDevice("narrator.device",0,(struct IORequest *)&NarratorRequest,NULL))
		{
			SpeechAvailable = TRUE;

			CloseDevice((struct IORequest *)&NarratorRequest);
		}

		CloseLibrary(LocalTranslatorBase);
#endif
	}

	return(SpeechAvailable);
}

	/* CurrentTimeToStamp(struct DateStamp *Stamp):
	 *
	 *	A replacement for DateStamp().
	 */

VOID
CurrentTimeToStamp(struct DateStamp *Stamp)
{
	struct timeval Now;

	GetSysTime(&Now);

	Stamp->ds_Days		= Now.tv_secs / (24 * 60 * 60);
	Stamp->ds_Minute	= (Now.tv_secs % (24 * 60 * 60)) / 60;
	Stamp->ds_Tick		= (Now.tv_secs % 60) * TICKS_PER_SECOND + (TICKS_PER_SECOND * Now.tv_micro) / MILLION;
}

	/* SmartOpenDiskFont(struct TextAttr *TextAttr):
	 *
	 *	Opens a disk-resident font. Does not require diskfont.library
	 *	to be open all the time while the program is running.
	 */

struct TextFont *
SmartOpenDiskFont(struct TextAttr *TextAttr)
{
	struct TextFont *Result;

	Result = NULL;

	if(DiskfontBase = OpenLibrary("diskfont.library",0))
	{
		Result = OpenDiskFont(TextAttr);

		CloseLibrary(DiskfontBase);
	}

	if(!Result)
		Result = OpenFont(TextAttr);

	return(Result);
}

	/* ArmLimit(ULONG SecondsToGo):
	 *
	 *	Arm the limit counter. We remember the time when the limit
	 *	will be exceeded.
	 */

VOID
ArmLimit(ULONG SecondsToGo)
{
	GetSysTime(&LimitTime);
	LimitTime.tv_secs += SecondsToGo;
}

	/* DisarmLimit():
	 *
	 *	Clear the limit counter.
	 */

VOID
DisarmLimit()
{
	memset(&LimitTime,0,sizeof(LimitTime));
}

	/* CheckLimit():
	 *
	 *	Check if the limit counter has been triggered.
	 *	If triggered, clear the limit counter.
	 */

BOOL
CheckLimit()
{
	BOOL Triggered;

	Triggered = FALSE;

	if(LimitTime.tv_secs > 0 || LimitTime.tv_micro > 0)
	{
		struct timeval Now;

		GetSysTime(&Now);

		if(-CmpTime(&Now,&LimitTime) >= 0)
			Triggered = TRUE;
	}

	if(Triggered)
		DisarmLimit();

	return(Triggered);
}
