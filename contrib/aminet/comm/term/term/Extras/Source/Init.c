/*
**	Init.c
**
**	Program initialization and shutdown routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* This will later go into the screen and window title */

#ifdef CPU_ANY
STATIC STRPTR Machine = "";
#else
STATIC STRPTR Machine = "'020+ ";
#endif

	/* The default public screen, if it is locked */

STATIC struct Screen *DefaultPubScreen;

	/* This variable helps us to remember whether the fast!
	 * macro panel was open or not.
	 */

STATIC BOOL HadFastMacros = FALSE;

	/* Remember whether we did pen allocation or not. */

STATIC BOOL AllocatedPens = FALSE;

	/* AddExtraAssignment(STRPTR LocalDir,STRPTR Assign):
	 *
	 *	Add assignments for local directories.
	 */

STATIC VOID
AddExtraAssignment(STRPTR LocalDir,STRPTR Assign)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	BPTR FileLock;

		/* Add the colon, we'll need it later */

	LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%s:",Assign);

		/* Is the local directory present? */

	if(FileLock = Lock(LocalDir,ACCESS_READ))
	{
			/* Is the assignment present? */

		if(IsAssign(LocalBuffer))
		{
				/* Check to see if the local directory */
				/* is already on the assignment list */

			if(LockInAssign(FileLock,LocalBuffer))
			{
				UnLock(FileLock);

				FileLock = BNULL;
			}
		}
	}

		/* Can we attach the lock to the assignment list? */

	if(FileLock)
	{
		Forbid();

			/* If the assignment is already present, add the */
			/* new directory, else create a new assignment. */

		if(IsAssign(LocalBuffer))
			AssignAdd(Assign,FileLock);
		else
			AssignLock(Assign,FileLock);

		Permit();
	}
}

	/* StatusSizeSetup():
	 *
	 *	Precalculates the size of the status line display.
	 */

STATIC VOID
StatusSizeSetup(struct Screen *Screen,LONG *StatusWidth,LONG *StatusHeight)
{
	SZ_SizeSetup(Screen,(struct TextAttr *)&UserFont);

	if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0] && Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && !Config->ScreenConfig->SplitStatus)
	{
		*StatusHeight = 0;

		return;
	}

	if(Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED)
	{
		if(Config->ScreenConfig->StatusLine == STATUSLINE_COMPRESSED)
		{
			*StatusWidth	= 80 * UserFontWidth;
			*StatusHeight	= UserFontHeight;

			if(Config->ScreenConfig->SplitStatus)
			{
				*StatusWidth	+= 2;
				*StatusHeight	+= 2;
			}
		}
		else
		{
			LONG ColumnLeft[4],ColumnWidth[4];
			LONG i,Len,Max;

			*StatusWidth	= 0;
			*StatusHeight	= SZ_BoxHeight(2);

			ColumnLeft[0] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_STATUS_TXT,MSG_TERMSTATUSDISPLAY_FONT_TXT,-1);
			ColumnLeft[1] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_PROTOCOL_TXT,MSG_TERMSTATUSDISPLAY_TERMINAL_TXT,-1);
			ColumnLeft[2] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_BAUDRATE_TXT,MSG_TERMSTATUSDISPLAY_PARAMETERS_TXT,-1);
			ColumnLeft[3] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_TIME_TXT,MSG_TERMSTATUSDISPLAY_ONLINE_TXT,-1);

			Max = 0;

			for(i = MSG_TERMAUX_READY_TXT ; i <= MSG_TERMAUX_HANG_UP_TXT ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(LocaleString(i)))) > Max)
					Max = Len;
			}

			for(i = MSG_TERMSTATUSDISPLAY_FROZEN_TXT ; i <= MSG_TERMSTATUSDISPLAY_RECORDING_TXT ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(LocaleString(i)))) > Max)
					Max = Len;
			}

			ColumnWidth[0] = Max;

			Max = SZ_BoxWidth(12);

			for(i = MSG_TERMAUX_ANSI_VT102_TXT ; i <= MSG_TERMAUX_HEX_TXT ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(LocaleString(i)))) > Max)
					Max = Len;
			}

			ColumnWidth[1] = Max;

			Max = SZ_BoxWidth(10);

			for(i = MSG_TERMAUX_NONE_TXT ; i <= MSG_TERMAUX_SPACE_TXT ; i++)
			{
				if((Len = SZ_BoxWidth(4 + strlen(LocaleString(i)))) > Max)
					Max = Len;
			}

			ColumnWidth[2] = Max;

			ColumnWidth[3] = SZ_BoxWidth(8);

			for(i = 0 ; i < 4 ; i++)
				*StatusWidth += ColumnWidth[i] + ColumnLeft[i];

			*StatusWidth += 4 + 3 * InterWidth;

			if(!Config->ScreenConfig->SplitStatus)
				*StatusHeight += 4;
			else
				*StatusHeight += 2;
		}
	}
	else
		*StatusHeight = 0;
}

	/* LoadKeyMap(STRPTR Name):
	 *
	 *	Load a keymap file from disk.
	 */

STATIC struct KeyMap *
LoadKeyMap(STRPTR Name)
{
	struct KeyMapResource *KeyMapResource;
	struct KeyMap *Map = NULL;

		/* Try to get access to the list of currently loaded
		 * keymap files.
		 */

	if(KeyMapResource = (struct KeyMapResource *)OpenResource("keymap.resource"))
	{
		struct KeyMapNode *Node;

			/* Try to find the keymap in the list. */

		Forbid();

		if(Node = (struct KeyMapNode *)FindName(&KeyMapResource->kr_List,FilePart(Name)))
			Map = &Node->kn_KeyMap;

		Permit();
	}

		/* Still no keymap available? */

	if(!Map)
	{
		APTR OldPtr;

			/* Disable DOS requesters. */

		DisableDOSRequesters(&OldPtr);

			/* Unload the old keymap code. */

		if(KeySegment)
			UnLoadSeg(KeySegment);

			/* Try to load the keymap from the
			 * name the user entered.
			 */

		if(!(KeySegment = LoadSeg(Name)))
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

				/* Second try: load it from
 				 * the standard keymaps drawer.
 				 */

			strcpy(LocalBuffer,"KEYMAPS:");

			if(AddPart(LocalBuffer,FilePart(Name),sizeof(LocalBuffer)))
			{
				if(!(KeySegment = LoadSeg(LocalBuffer)))
				{
					strcpy(LocalBuffer,"Devs:Keymaps");

					if(AddPart(LocalBuffer,FilePart(Name),sizeof(LocalBuffer)))
						KeySegment = LoadSeg(LocalBuffer);
				}
			}
		}

			/* Did we get the keymap file? */

		if(KeySegment)
		{
			struct KeyMapNode *Node = (struct KeyMapNode *)&((ULONG *)BADDR(KeySegment))[1];

			Map = &Node->kn_KeyMap;
		}

			/* Enable DOS requesters again. */

		EnableDOSRequesters(OldPtr);
	}
	else
	{
		if(KeySegment)
		{
			UnLoadSeg(KeySegment);

			KeySegment = BNULL;
		}
	}

	return(Map);
}

	/* DeleteOffsetTables(VOID):
	 *
	 *	Delete the line multiplication tables.
	 */

STATIC VOID
DeleteOffsetTables(VOID)
{
	FreeVecPooled(OffsetXTable);
	OffsetXTable = NULL;

	FreeVecPooled(OffsetYTable);
	OffsetYTable = NULL;
}

	/* CreateOffsetTables(VOID):
	 *
	 *	Allocate the line multiplication tables.
	 */

STATIC BOOL
CreateOffsetTables(VOID)
{
	LONG Width,Height;

	Width	= (Window->WScreen->Width  + TextFontWidth)  * 2 / TextFontWidth;
	Height	= (Window->WScreen->Height + TextFontHeight) * 2 / TextFontHeight;

	DeleteOffsetTables();

	if(OffsetXTable = (LONG *)AllocVecPooled(Width * sizeof(LONG),MEMF_ANY))
	{
		if(OffsetYTable = (LONG *)AllocVecPooled(Height * sizeof(LONG),MEMF_ANY))
		{
			LONG i,j;

			for(i = j = 0 ; i < Width ; i++, j += TextFontWidth)
				OffsetXTable[i] = j;

			for(i = j = 0 ; i < Height ; i++, j += TextFontHeight)
				OffsetYTable[i] = j;

			return(TRUE);
		}
	}

	DeleteOffsetTables();

	return(FALSE);
}

	/* ReleaseDefaultPubScreen():
	 *
	 *	Unlocks the default public screen in case it had been locked.
	 */

STATIC VOID
ReleaseDefaultPubScreen(VOID)
{
	if(DefaultPubScreen)
	{
		UnlockPubScreen(NULL,DefaultPubScreen);
		DefaultPubScreen = NULL;
	}
}

	/* SafeOpenLibrary(STRPTR Name,LONG Version):
	 *
	 *	Try to open a library, but if there already is
	 *	a version in memory that's older than the release
	 *	we want flush it out first.
	 */

struct Library *
SafeOpenLibrary(STRPTR Name,LONG Version)
{
	struct Library *Base;

	Forbid();

		/* Is this library already in memory? */

	if(Base = (struct Library *)FindName(&SysBase->LibList,FilePart(Name)))
	{
			/* An old release? */

		if(Base->lib_Version < Version)
		{
				/* Flush it out. */

			RemLibrary(Base);
		}
	}

	Permit();

		/* Now reopen the library. */

	return(OpenLibrary(Name,Version));
}

	/* TTYResize():
	 *
	 *	Signal AmigaUW that the window size has changed.
	 */

VOID
TTYResize()
{
	if(Window && WriteRequest)
	{
		LONG Columns,Lines;

		if(XEmulatorBase && XEM_IO)
		{
			if(XEmulatorBase->lib_Version < 4)
				Columns = Lines = 0;
			else
			{
				ULONG Result = XEmulatorInfo(XEM_IO,XEMI_CONSOLE_DIMENSIONS);

				Columns	= XEMI_EXTRACT_COLUMNS(Result);
				Lines	= XEMI_EXTRACT_LINES(Result);
			}
		}
		else
		{
			Columns	= LastColumn + 1;
			Lines	= LastLine + 1;
		}

		if(Columns > 0 && Lines > 0)
		{
			WriteRequest->IOSer.io_Command	= UWCMD_TTYRESIZE;
			WriteRequest->IOSer.io_Data		= (APTR)(IPTR)((Columns << 16) | (Lines));
			WriteRequest->IOSer.io_Length	= (WindowWidth << 16) | (WindowHeight);

			DoIO((struct IORequest *)WriteRequest);
		}
	}
}

	/* UpdateTerminalLimits():
	 *
	 *	Check the current window size and extract the
	 *	size and position of the usable window rectangle.
	 */

VOID
UpdateTerminalLimits()
{
	WindowLeft		= Window->BorderLeft;
	WindowTop		= Window->BorderTop;

	WindowWidth		= Window->Width - (Window->BorderLeft + Window->BorderRight);
	WindowHeight	= Window->Height - (Window->BorderTop + Window->BorderBottom);

	if(StatusWindow)
	{
		StatusDisplayLeft	= StatusWindow->BorderLeft;
		StatusDisplayTop	= StatusWindow->BorderTop;
		StatusDisplayWidth	= StatusWindow->Width - (StatusWindow->BorderLeft + StatusWindow->BorderRight);
		StatusDisplayHeight	= StatusWindow->Height - (StatusWindow->BorderTop + StatusWindow->BorderBottom);
	}
	else
	{
		if(Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && !(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0]))
		{
			StatusDisplayLeft	= WindowLeft;
			StatusDisplayTop	= Window->Height - (Window->BorderBottom + StatusDisplayHeight);
			StatusDisplayWidth	= WindowWidth;

			WindowHeight -= StatusDisplayHeight;
		}
	}

	if(ChatMode && !(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0]))
	{
		if(CreateChatGadget())
		{
			UpdateChatGadget();

			WindowHeight -= (UserFontHeight + 2);

			ActivateChat(FALSE);
		}
	}
}

	/* Current2DefaultPalette(struct Configuration *SomeConfig):
	 *
	 *	Copy the current colour palette into the
	 *	default tables.
	 */

VOID
Current2DefaultPalette(struct Configuration *SomeConfig)
{
	ColourTable	*Table;
	UWORD *Colour12 = AtomicColours; /* COLOUR_MONO */

	Table = NULL;

	if(!SomeConfig)
		SomeConfig = Config;

	switch(SomeConfig->ScreenConfig->ColourMode)
	{
		case COLOUR_EIGHT:

			if(Kick30)
			{
				if(!ANSIColourTable)
					ANSIColourTable = CreateColourTable(8,ANSIColours,NULL);

				Table = ANSIColourTable;
			}

			Colour12 = ANSIColours;

			break;

		case COLOUR_SIXTEEN:

			if(Kick30)
			{
				if(!EGAColourTable)
					EGAColourTable = CreateColourTable(16,EGAColours,NULL);

				Table = EGAColourTable;
			}

			Colour12 = EGAColours;

			break;

		case COLOUR_AMIGA:

			if(Kick30)
			{
				if(!DefaultColourTable)
					DefaultColourTable = CreateColourTable(16,DefaultColours,NULL);

				Table = DefaultColourTable;
			}

			Colour12 = DefaultColours;

			break;

		case COLOUR_MONO:

			if(Kick30)
			{
				if(!MonoColourTable)
					MonoColourTable = CreateColourTable(2,AtomicColours,NULL);

				Table = MonoColourTable;
			}

			Colour12 = AtomicColours;
			break;
	}

	if(Table)
	{
		if(SomeConfig->ScreenConfig->UseColours96)
			Colour96xColourTable(SomeConfig->ScreenConfig->Colours96,Table,Table->NumColours);
		else
		{
			Colour12xColourTable(SomeConfig->ScreenConfig->Colours,Table,Table->NumColours);

			Colour12x96(SomeConfig->ScreenConfig->Colours,SomeConfig->ScreenConfig->Colours96,16);

			SomeConfig->ScreenConfig->UseColours96 = TRUE;
		}
	}

	CopyMem(SomeConfig->ScreenConfig->Colours,Colour12,16 * sizeof(UWORD));
}

	/* Default2CurrentPalette(struct Configuration *SomeConfig):
	 *
	 *	Copy the default palette to the current palette.
	 */

VOID
Default2CurrentPalette(struct Configuration *SomeConfig)
{
	ColourTable	*Table;
	UWORD *Colour12 = AtomicColours;

	Table = NULL;

	if(!SomeConfig)
		SomeConfig = Config;

	switch(SomeConfig->ScreenConfig->ColourMode)
	{
		case COLOUR_EIGHT:

			Table		= ANSIColourTable;
			Colour12	= ANSIColours;

			break;

		case COLOUR_SIXTEEN:

			Table		= EGAColourTable;
			Colour12	= EGAColours;

			break;

		case COLOUR_AMIGA:

			Table		= DefaultColourTable;
			Colour12	= DefaultColours;

			break;

		case COLOUR_MONO:

			Table		= MonoColourTable;
			Colour12	= AtomicColours;

			break;
	}

	CopyMem(Colour12,SomeConfig->ScreenConfig->Colours,16 * sizeof(UWORD));

	if(Table)
	{
		ColourTablex96(Table,SomeConfig->ScreenConfig->Colours96);

		SomeConfig->ScreenConfig->UseColours96 = TRUE;
	}
	else
		SomeConfig->ScreenConfig->UseColours96 = FALSE;
}

	/* PaletteSetup():
	 *
	 *	Set up colour palettes.
	 */

VOID
PaletteSetup(struct Configuration *SomeConfig)
{
	LONG i;

	if(!SomeConfig)
		SomeConfig = Config;

	if(SomeConfig->ScreenConfig->UseColours96)
	{
		Colour96x12(SomeConfig->ScreenConfig->Colours96,NormalColours,16);
		Colour96x12(SomeConfig->ScreenConfig->Colours96,SomeConfig->ScreenConfig->Colours,16);
	}
	else
	{
		CopyMem(SomeConfig->ScreenConfig->Colours,NormalColours,16 * sizeof(UWORD));

		Colour12x96(NormalColours,SomeConfig->ScreenConfig->Colours96,16);

		SomeConfig->ScreenConfig->UseColours96 = TRUE;
	}

	CopyMem(NormalColours,&NormalColours[16],16 * sizeof(UWORD));
	CopyMem(NormalColours,BlinkColours,32 * sizeof(UWORD));

	switch(SomeConfig->ScreenConfig->ColourMode)
	{
		case COLOUR_EIGHT:

			if(SomeConfig->ScreenConfig->Blinking)
			{
				for(i = 0 ; i < 8 ; i++)
					BlinkColours[8 + i] = BlinkColours[0];

				PaletteSize = 16;
			}
			else
				PaletteSize = 8;

			break;

		case COLOUR_SIXTEEN:

			if(GetBitMapDepth(Window->WScreen->RastPort.BitMap) >= 5 && SomeConfig->ScreenConfig->Blinking)
			{
				for(i = 0 ; i < 16 ; i++)
					BlinkColours[16 + i] = BlinkColours[0];

				PaletteSize = 32;
			}
			else
				PaletteSize = 16;

			break;

		case COLOUR_AMIGA:

			BlinkColours[3] = BlinkColours[0];

			PaletteSize = 4;

			break;

		case COLOUR_MONO:

			PaletteSize = 2;
			break;
	}

	if(Kick30)
	{
		if(NormalColourTable)
			DeleteColourTable(NormalColourTable);

		if(BlinkColourTable)
			DeleteColourTable(BlinkColourTable);

		if(NormalColourTable = CreateColourTable(PaletteSize,NULL,SomeConfig->ScreenConfig->Colours96))
		{
			if(PaletteSize == 2 || !SomeConfig->ScreenConfig->Blinking)
				BlinkColourTable = NULL;
			else
			{
				if(BlinkColourTable = CreateColourTable(PaletteSize,NULL,NULL))
				{
					switch(SomeConfig->ScreenConfig->ColourMode)
					{
						case COLOUR_EIGHT:

							for(i = 0 ; i < 8 ; i++)
							{
								CopyColourEntry(NormalColourTable,NormalColourTable,i,8 + i);
								CopyColourEntry(NormalColourTable,BlinkColourTable,i,i);
								CopyColourEntry(NormalColourTable,BlinkColourTable,0,8 + i);
							}

							break;

						case COLOUR_SIXTEEN:

							if(GetBitMapDepth(Window->WScreen->RastPort.BitMap) >= 5)
							{
								for(i = 0 ; i < 16 ; i++)
								{
									CopyColourEntry(NormalColourTable,NormalColourTable,i,16 + i);
									CopyColourEntry(NormalColourTable,BlinkColourTable,i,i);
									CopyColourEntry(NormalColourTable,BlinkColourTable,0,16 + i);
								}
							}
							else
							{
								DeleteColourTable(BlinkColourTable);

								BlinkColourTable = NULL;
							}

							break;

						case COLOUR_AMIGA:

							for(i = 0 ; i < 4 ; i++)
								CopyColourEntry(NormalColourTable,BlinkColourTable,i,i);

							CopyColourEntry(BlinkColourTable,BlinkColourTable,0,3);

							break;
					}
				}
				else
				{
					DeleteColourTable(NormalColourTable);

					NormalColourTable = NULL;
				}
			}
		}
	}
}

	/* ScreenSizeStuff():
	 *
	 *	Set up the terminal screen size.
	 */

VOID
ScreenSizeStuff()
{
	ObtainSemaphore(&TerminalSemaphore);

		/* Is this really the built-in emulation? */

	if(Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL)
	{
		LONG MaxColumns,MaxLines,Columns,Lines;

		MaxColumns	= WindowWidth / TextFontWidth;
		MaxLines	= WindowHeight / TextFontHeight;

			/* Drop the text area marker. */

		if(Marking)
			WindowMarkerStop();

			/* Turn off the cursor. */

		ClearCursor();

			/* Set up the new screen width. */

		if(Config->TerminalConfig->NumColumns < 20)
			Columns = MaxColumns;
		else
			Columns = Config->TerminalConfig->NumColumns;

			/* Set up the new screen height. */

		if(Config->TerminalConfig->NumLines < 20)
			Lines = MaxLines;
		else
			Lines = Config->TerminalConfig->NumLines;

			/* More columns than we will be able to display? */

		if(Columns > MaxColumns)
			Columns = MaxColumns;

			/* More lines than we will be able to display? */

		if(Lines > MaxLines)
			Lines = MaxLines;

			/* Set up the central data. */

		LastColumn	= Columns - 1;
		LastLine	= Lines - 1;
		LastPixel	= MUL_X(Columns) - 1;

			/* Are we to clear the margin? */

		if(Columns < MaxColumns || Lines < MaxLines)
		{
				/* Save the rendering attributes. */

			BackupRender();

				/* Set the defaults. */

			SetAPen(RPort,BgPen = MappedPens[0][PenTable[0]]);

			SetMask(RPort,DepthMask);

				/* Clear remaining columns. */

			if(Columns < MaxColumns)
				ScrollLineRectFill(RPort,MUL_X(LastColumn + 1),0,WindowWidth - 1,WindowHeight - 1);

				/* Clear remaining lines. */

			if(Lines < MaxLines)
				ScrollLineRectFill(RPort,0,MUL_Y(LastLine + 1),WindowWidth - 1,WindowHeight - 1);

				/* Restore rendering attributes. */

			BackupRender();
		}

			/* Truncate illegal cursor position. */

		if(CursorY > LastLine)
			CursorY = LastLine;

		ConFontScaleUpdate();

			/* Truncate illegal cursor position. */

		if(CursorX > LastPrintableColumn)
			CursorX = LastPrintableColumn;

			/* Fix scroll region button. */

		if(!RegionSet)
			Bottom = LastLine;

			/* Turn the cursor back on. */

		DrawCursor();
	}

	FixScreenSize = FALSE;

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* PubScreenStuff():
	 *
	 *	This part handles the public screen setup stuff.
	 */

VOID
PubScreenStuff()
{
	if(Screen)
	{
			/* Are we to make our screen public? */

		if(Config->ScreenConfig->MakeScreenPublic)
			PubScreenStatus(Screen,0);
		else
			PubScreenStatus(Screen,PSNF_PRIVATE);
	}
}

	/* ConfigSetup():
	 *
	 *	Compare the current configuration with the
	 *	last backup and reset the serial device, terminal,
	 *	etc. if necessary.
	 */

VOID
ConfigSetup()
{
	BOOL RasterWasEnabled,WindowSizeUpdate,WantCustomScreen;

	RasterWasEnabled = RasterEnabled;
	WindowSizeUpdate = FALSE;

		/* See if we want a custom screen */

	if(Config->ScreenConfig->UseWorkbench || Config->ScreenConfig->ShareScreen)
		WantCustomScreen = FALSE;
	else
		WantCustomScreen = TRUE;

		/* Hide or show the upload queue icon. */

	ToggleUploadQueueIcon(Config->TransferConfig->HideUploadIcon);

		/* Take care of the end-of-line translation. */

	Update_CR_LF_Translation();

		/* First we will take a look at the configuration
		 * and try to find those parts which have changed
		 * and require the main screen display to be
		 * reopened.
		 */

	if(PrivateConfig->ScreenConfig->FontHeight != Config->ScreenConfig->FontHeight)
		ResetDisplay = TRUE;

	if((PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL) || (PrivateConfig->TerminalConfig->EmulationMode != EMULATION_EXTERNAL && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL))
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->SplitStatus != Config->ScreenConfig->SplitStatus)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->ShareScreen != Config->ScreenConfig->ShareScreen)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->Depth != Config->ScreenConfig->Depth || PrivateConfig->ScreenConfig->OverscanType != Config->ScreenConfig->OverscanType)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->DisplayWidth != Config->ScreenConfig->DisplayWidth || PrivateConfig->ScreenConfig->DisplayHeight != Config->ScreenConfig->DisplayHeight)
		ResetDisplay = TRUE;

	if(Stricmp(PrivateConfig->ScreenConfig->FontName,Config->ScreenConfig->FontName))
		ResetDisplay = TRUE;

	if(PrivateConfig->TerminalConfig->FontMode != Config->TerminalConfig->FontMode)
		ResetDisplay = TRUE;

	if(PrivateConfig->TerminalConfig->UseTerminalTask != Config->TerminalConfig->UseTerminalTask)
		ResetDisplay = TRUE;

	if(PrivateConfig->TerminalConfig->AutoSize != Config->TerminalConfig->AutoSize && !WantCustomScreen)
	{
		if(Config->TerminalConfig->AutoSize)
			FixScreenSize = TRUE;
		else
			WindowSizeUpdate = TRUE;
	}

	if(PrivateConfig->TerminalConfig->TextFontHeight != Config->TerminalConfig->TextFontHeight)
		ResetDisplay = TRUE;

	if(Stricmp(PrivateConfig->TerminalConfig->TextFontName,Config->TerminalConfig->TextFontName))
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->DisplayMode != Config->ScreenConfig->DisplayMode || PrivateConfig->ScreenConfig->ColourMode != Config->ScreenConfig->ColourMode)
		ResetDisplay = TRUE;

	if(PrivateConfig->TerminalConfig->IBMFontHeight != Config->TerminalConfig->IBMFontHeight)
		ResetDisplay = TRUE;

	if(Stricmp(PrivateConfig->TerminalConfig->IBMFontName,Config->TerminalConfig->IBMFontName))
		ResetDisplay = TRUE;

	if((PrivateConfig->ScreenConfig->ColourMode == Config->ScreenConfig->ColourMode) && (PrivateConfig->ScreenConfig->ColourMode == COLOUR_EIGHT || PrivateConfig->ScreenConfig->ColourMode == COLOUR_SIXTEEN))
	{
		if(PrivateConfig->ScreenConfig->Blinking != Config->ScreenConfig->Blinking)
			ResetDisplay = TRUE;
	}

	if(Kick30)
	{
		if(Config->ScreenConfig->UsePens != PrivateConfig->ScreenConfig->UsePens || memcmp(Config->ScreenConfig->PenArray,PrivateConfig->ScreenConfig->PenArray,sizeof(UWORD) * 12))
			ResetDisplay = TRUE;
	}

	if(Config->ScreenConfig->Depth != PrivateConfig->ScreenConfig->Depth)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->FasterLayout != Config->ScreenConfig->FasterLayout)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->StatusLine != Config->ScreenConfig->StatusLine)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->TitleBar != Config->ScreenConfig->TitleBar)
		ResetDisplay = TRUE;

	if(PrivateConfig->ScreenConfig->UseWorkbench != Config->ScreenConfig->UseWorkbench)
		ResetDisplay = TRUE;

	if(strcmp(PrivateConfig->ScreenConfig->PubScreenName,Config->ScreenConfig->PubScreenName) && Config->ScreenConfig->UseWorkbench)
		ResetDisplay = TRUE;

	if(PrivateConfig->EmulationConfig->UseStandardPens != Config->EmulationConfig->UseStandardPens)
		ResetDisplay = TRUE;

	if(!Config->EmulationConfig->UseStandardPens && (memcmp(Config->EmulationConfig->Pens,PrivateConfig->EmulationConfig->Pens,sizeof(Config->EmulationConfig->Pens)) || memcmp(Config->EmulationConfig->Attributes,PrivateConfig->EmulationConfig->Attributes,sizeof(Config->EmulationConfig->Attributes))))
		ResetDisplay = TRUE;

		/* Now for the `harmless' actions which do not
		 * require to change the screen or other
		 * rendering data.
		 */

	if(!ResetDisplay)
	{
		if(PrivateConfig->TerminalConfig->NumColumns != Config->TerminalConfig->NumColumns || PrivateConfig->TerminalConfig->NumLines != Config->TerminalConfig->NumLines)
		{
			if(!WantCustomScreen)
				WindowSizeUpdate = TRUE;
			else
				ResetDisplay = TRUE;
		}
	}

	if(!ResetDisplay && WindowSizeUpdate)
	{
		LONG	Width,
			Height;

		if(Config->TerminalConfig->NumColumns < 20)
			Width = ScreenWidth;
		else
			Width = Window->BorderLeft + TextFontWidth * Config->TerminalConfig->NumColumns + Window->BorderRight;

		if(Config->TerminalConfig->NumLines < 20)
			Height = ScreenHeight;
		else
			Height = Window->BorderTop + TextFontHeight * Config->TerminalConfig->NumLines + Window->BorderBottom;

			/* Add the status line size if necessary */

		if(!StatusWindow && Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && !(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0]))
			Height += StatusDisplayHeight;

		ChangeWindowBox(Window,Window->LeftEdge,Window->TopEdge,Width,Height);

		FixScreenSize = TRUE;
	}

	if(PrivateConfig->ScreenConfig->MakeScreenPublic != Config->ScreenConfig->MakeScreenPublic)
		PubScreenStuff();

	if(PrivateConfig->ScreenConfig->ColourMode == Config->ScreenConfig->ColourMode && (memcmp(PrivateConfig->ScreenConfig->Colours,Config->ScreenConfig->Colours,sizeof(UWORD) * 16) || memcmp(PrivateConfig->ScreenConfig->Colours96,Config->ScreenConfig->Colours96,sizeof(ColourEntry) * 16)))
		Current2DefaultPalette(Config);

		/* Are we to load a new transfer library? */

	if(Config->TransferConfig->DefaultType == XFER_XPR)
	{
		if(Config->TransferConfig->DefaultLibrary[0])
		{
			if(strcmp(PrivateConfig->TransferConfig->DefaultLibrary,Config->TransferConfig->DefaultLibrary))
			{
				strcpy(LastXprLibrary,Config->TransferConfig->DefaultLibrary);

				ProtocolSetup(FALSE);
			}
		}
		else
			CloseXPR();
	}
	else
	{
		CloseXPR();

		ProtocolSetup(FALSE);
	}

		/* No custom keymap this time? */

	if(!Config->TerminalConfig->KeyMapFileName[0])
	{
		KeyMap = NULL;

		if(KeySegment)
		{
			UnLoadSeg(KeySegment);

			KeySegment = BNULL;
		}
	}
	else
	{
			/* Check whether the keymap name has changed. */

		if(strcmp(PrivateConfig->TerminalConfig->KeyMapFileName,Config->TerminalConfig->KeyMapFileName))
			KeyMap = LoadKeyMap(Config->TerminalConfig->KeyMapFileName);
	}

		/* Are we to load the keyboard macro settings? */

	if(Config->MacroFileName[0] && Stricmp(PrivateConfig->MacroFileName,Config->MacroFileName))
	{
		if(!LoadMacros(Config->MacroFileName,MacroKeys))
			ResetMacroKeys(MacroKeys);
		else
			strcpy(LastMacros,Config->MacroFileName);
	}

		/* Are we to load the cursor key settings? */

	if(Config->CursorFileName[0] && Stricmp(PrivateConfig->CursorFileName,Config->CursorFileName))
	{
		if(!ReadIFFData(Config->CursorFileName,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
			ResetCursorKeys(CursorKeys);
		else
			strcpy(LastCursorKeys,Config->CursorFileName);
	}

		/* Are we to load the translation tables? */

	if(Config->TranslationFileName[0] && Stricmp(PrivateConfig->TranslationFileName,Config->TranslationFileName))
	{
		if(SendTable)
		{
			FreeTranslationTable(SendTable);

			SendTable = NULL;
		}

		if(ReceiveTable)
		{
			FreeTranslationTable(ReceiveTable);

			ReceiveTable = NULL;
		}

		if(SendTable = AllocTranslationTable())
		{
			if(ReceiveTable = AllocTranslationTable())
			{
				if(LoadTranslationTables(Config->TranslationFileName,SendTable,ReceiveTable))
				{
					strcpy(LastTranslation,Config->TranslationFileName);

					if(IsStandardTable(SendTable) && IsStandardTable(ReceiveTable))
					{
						FreeTranslationTable(SendTable);

						SendTable = NULL;

						FreeTranslationTable(ReceiveTable);

						ReceiveTable = NULL;
					}
				}
				else
				{
					FreeTranslationTable(SendTable);

					SendTable = NULL;

					FreeTranslationTable(ReceiveTable);

					ReceiveTable = NULL;
				}
			}
			else
			{
				FreeTranslationTable(SendTable);

				SendTable = NULL;
			}
		}
	}

		/* Update the text sending functions. */

	SendSetup();

		/* Are we to load the fast macro settings? */

	if(Config->FastMacroFileName[0] && Stricmp(PrivateConfig->FastMacroFileName,Config->FastMacroFileName))
	{
		if(FastWindow)
			LT_LockWindow(FastWindow);

		FreeList(&FastMacroList);

		if(LoadFastMacros(Config->FastMacroFileName,&FastMacroList))
		{
			strcpy(LastFastMacros,Config->FastMacroFileName);
			FastMacroCount = GetListSize(&FastMacroList);
		}

		if(FastWindow)
		{
			RefreshFastWindow();

			LT_UnlockWindow(FastWindow);
		}
	}

		/* Serial configuration needs updating? */

	ReconfigureSerial(Window,NULL);

		/* Are we to open the fast macro panel? */

	if(Config->MiscConfig->OpenFastMacroPanel)
		HadFastMacros = TRUE;

		/* Are we to freeze the text buffer? */

	if(!Config->CaptureConfig->BufferEnabled)
		BufferFrozen = TRUE;

		/* Now for the actions which require that the
		 * screen stays open.
		 */

	if(!ResetDisplay)
	{
		if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
		{
			if(PrivateConfig->TerminalConfig->EmulationMode != EMULATION_EXTERNAL || (Config->TerminalConfig->EmulationFileName[0] && strcmp(PrivateConfig->TerminalConfig->EmulationFileName,Config->TerminalConfig->EmulationFileName)))
			{
				if(!OpenEmulator(Config->TerminalConfig->EmulationFileName))
				{
					Config->TerminalConfig->EmulationMode = EMULATION_ANSIVT100;

					ResetDisplay = TRUE;

					RasterEnabled = TRUE;

					ShowRequest(Window,LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_EMULATION_LIBRARY_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Config->TerminalConfig->EmulationFileName);
				}
				else
					RasterEnabled = FALSE;
			}
		}
		else
		{
			if(XEmulatorBase && PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
			{
				XEmulatorClearConsole(XEM_IO);

				CloseEmulator(FALSE);
			}
			else
				RasterEnabled = TRUE;
		}

		if(RasterEnabled != RasterWasEnabled)
			RasterEraseScreen(2);

		if(!Config->ScreenConfig->UseWorkbench && !SharedScreen)
		{
			PaletteSetup(Config);

			LoadColourTable(VPort,NormalColourTable,NormalColours,PaletteSize);
		}

		if(Config->MiscConfig->OpenFastMacroPanel && !FastWindow)
			OpenFastWindow();

		PubScreenStuff();

		if(Menu)
		{
			CheckItem(MEN_FREEZE_BUFFER,BufferFrozen);

			if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0])
				OffItem(MEN_CHAT_LINE);
			else
				CheckItem(MEN_CHAT_LINE,ChatMode);

			SetTransferMenu(TRUE);

			SetRasterMenu(RasterEnabled);

			if(MatrixWindow)
				CheckItem(MEN_MATRIX_WINDOW,TRUE);

			if(InfoWindow)
				CheckItem(MEN_STATUS_WINDOW,TRUE);

			if(PacketWindow)
				CheckItem(MEN_PACKET_WINDOW,TRUE);

			if(ChatMode)
				CheckItem(MEN_CHAT_LINE,TRUE);

			if(FastWindow)
				CheckItem(MEN_FAST_MACROS_WINDOW,TRUE);

				/* Disable the dialing functions if online. */

			if(Online)
				SetDialMenu(FALSE);
			else
				SetDialMenu(TRUE);
		}

		Blocking = FALSE;
	}
	else
	{
			/* Are we no longer to use the external emulator? */

		if(Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL)
		{
			if(XEmulatorBase && PrivateConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
			{
				XEmulatorClearConsole(XEM_IO);

				CloseEmulator(FALSE);
			}
		}

		RasterEnabled = TRUE;
		FixScreenSize = FALSE;
	}

		/* Change the task priority. */

	SetTaskPri((struct Task *)ThisProcess,(LONG)Config->MiscConfig->Priority);

	ConOutputUpdate();

	ConFontScaleUpdate();

	ConProcessUpdate();

		/* Reset the scanner. */

	ResetDataFlowFilter();

	if(ResetDisplay)
		ActivateJob(MainJobQueue,ResetDisplayJob);
}

	/* DisplayReset():
	 *
	 *	Reset the entire display if necessary.
	 */

BOOL
DisplayReset()
{
	BOOL Success = TRUE;
	CONST_STRPTR Result;

		/* Delete the display (if possible).
		 * This will go wrong if there
		 * are any visitor windows on our
		 * screen.
		 */

	if(DeleteDisplay())
	{
		if(Result = CreateDisplay(FALSE,FALSE))
		{
			DeleteDisplay();

			ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Result);

			Success = FALSE;
		}
		else
		{
			BumpWindow(Window);

			PubScreenStuff();
		}
	}
	else
	{
		SaveConfig(PrivateConfig,Config);

		BlockWindows();
		ShowRequest(Window,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocaleString(MSG_TERMMAIN_CANNOT_CLOSE_SCREEN_YET_TXT));
		ReleaseWindows();
	}

	ResetDisplay = FALSE;
	SuspendJob(MainJobQueue,ResetDisplayJob);

		/* Prepare for the worst case... */

	if(!Success)
		MainTerminated = TRUE;

	return(Success);
}

	/* DeleteDisplay():
	 *
	 *	Free all resources associated with the terminal
	 *	display (tasks, interrupts, screen, window, etc.).
	 */

BOOL
DeleteDisplay()
{
	struct Screen *WhichScreen;

	if(SharedScreen)
		WhichScreen = SharedScreen;
	else
		WhichScreen = Screen;

	if(WhichScreen)
	{
		if(!(PubScreenStatus(WhichScreen,PSNF_PRIVATE) & PSNF_PRIVATE))
			return(FALSE);
	}

	StopTerminalTest();

	GuideCleanup();

	CloseQueueWindow();

	ShakeHands((struct Task *)StatusProcess,SIG_KILL);

	WindowMarkerStop();

	CloseInfoWindow();

	DeleteReview();

	DeleteEmulationProcess();

	CloseEmulator(FALSE);

	DeleteRaster();

	DeleteScale();

	FreeVecPooled(TabStops);
	TabStops = NULL;

	FreeVecPooled(ScrollLines);
	ScrollLines = NULL;

	if(Screen)
		ScreenToBack(Screen);

	if(FastWindow)
	{
		HadFastMacros = TRUE;

		CloseFastWindow();
	}
	else
		HadFastMacros = FALSE;

	if(MatrixWindow)
		CloseMatrixWindow();

	if(StatusWindow)
	{
		ClearMenuStrip(StatusWindow);
		CloseWindowSafely(StatusWindow);

		StatusWindow = NULL;
	}

	if(DrawInfo)
	{
			/* Release the rendering pens. */

		FreeScreenDrawInfo(Window->WScreen,DrawInfo);

		DrawInfo = NULL;
	}

	HideChatGadget();

	DeletePacketWindow(FALSE);

	if(Window)
	{
		if(!Screen)
			PutWindowInfo(WINDOW_MAIN,Window->LeftEdge,Window->TopEdge,Window->Width,Window->Height);

		if(AllocatedPens && Kick30)
		{
			LONG i;

			ObtainSemaphore(&TerminalSemaphore);

				/* Erase the window contents. We will
				 * want to release any pens we have
				 * allocated and want to avoid nasty
				 * flashing and flickering.
				 */

			SetAPen(RPort,0);

			FillBox(RPort,WindowLeft,WindowTop,WindowWidth,WindowHeight);

				/* Release any pens we have allocated. */

			for(i = 0 ; i < 32 ; i++)
			{
				if(MappedPens[1][i])
				{
					ReleasePen(VPort->ColorMap,MappedPens[0][i]);

					MappedPens[0][i] = i;
					MappedPens[1][i] = FALSE;
				}
			}

			AllocatedPens = FALSE;

			ReleaseSemaphore(&TerminalSemaphore);
		}

		if(ClipRegion)
		{
			InstallClipRegion(Window->WLayer,OldRegion);

			DisposeRegion(ClipRegion);

			ClipRegion = NULL;
		}

		ClearMenuStrip(Window);

		RestoreWindowPtr(OldWindowPtr);

		PopWindow();

		if(TermPort)
			TermPort->TopWindow = NULL;

		LT_DeleteWindowLock(Window);

		CloseWindow(Window);

		Window = NULL;
	}

	if(Menu)
	{
		LT_DisposeMenu(Menu);
		Menu = NULL;
	}

	FreeVisualInfo(VisualInfo);
	VisualInfo = NULL;

		/* Clean up the menu glyphs. */

	DisposeObject(AmigaGlyph);

	AmigaGlyph = NULL;

	DisposeObject(CheckGlyph);

	CheckGlyph = NULL;

	if(Screen)
	{
		CloseScreen(Screen);

		Screen = NULL;
	}

	if(SharedScreen)
	{
		CloseScreen(SharedScreen);

		SharedScreen = NULL;
	}

	ReleaseDefaultPubScreen();

	if(GFX)
	{
		CloseFont(GFX);

		GFX = NULL;
	}

	if(TextFont)
	{
		CloseFont(TextFont);

		TextFont = NULL;
	}

	if(UserTextFont)
	{
		CloseFont(UserTextFont);

		UserTextFont = NULL;
	}

	return(TRUE);
}

	/* CreateDisplay(BOOL UsePresetSize):
	 *
	 *	Open the display and allocate associated data.
	 */

CONST_STRPTR
CreateDisplay(BOOL UsePresetSize,BOOL Activate)
{
	LONG ErrorCode,Top,Height,Count,RealDepth = 1,i;
	LONG StatusWidth,StatusHeight;
	struct Rectangle DisplayClip;
	UWORD PenArray[16];
	ULONG X_DPI,Y_DPI;
	IPTR TagArray[9];
	BOOL OpenFailed;
	BOOL RethinkPens;

	OpenFailed = FALSE;
	Count = 0;

	ResetDisplay = FALSE;
	SuspendJob(MainJobQueue,ResetDisplayJob);

	BlockNestCount = 0;

	WeAreBlocking = FALSE;

	SetQueueDiscard(SpecialQueue,FALSE);

	TagDPI[0].ti_Tag = TAG_DONE;

		/* Don't permit weird settings. */

	if(!Config->ScreenConfig->StatusLine || (!Config->ScreenConfig->ShareScreen && !Config->ScreenConfig->UseWorkbench))
		Config->ScreenConfig->SplitStatus = FALSE;

	if(Config->ScreenConfig->UseWorkbench || Config->ScreenConfig->ShareScreen)
	{
		STRPTR ScreenName = NULL;

		if(Config->ScreenConfig->PubScreenName[0])
		{
			struct Screen *SomeScreen;

			if(SomeScreen = LockPubScreen(Config->ScreenConfig->PubScreenName))
			{
				UnlockPubScreen(NULL,SomeScreen);

				ScreenName = Config->ScreenConfig->PubScreenName;
			}
		}

		if(!(DefaultPubScreen = LockPubScreen(ScreenName)))
			return(LocaleString(MSG_TERMINIT_FAILED_TO_GET_DEFAULT_PUBLIC_SCREEN_TXT));
		else
		{
			GetDPI(GetVPModeID(&DefaultPubScreen->ViewPort),&X_DPI,&Y_DPI);

			strcpy(UserFontName,DefaultPubScreen->Font->ta_Name);

			UserFont.tta_Name	= UserFontName;
			UserFont.tta_YSize	= DefaultPubScreen->Font->ta_YSize;
			UserFont.tta_Style	= DefaultPubScreen->Font->ta_Style;
			UserFont.tta_Flags	= DefaultPubScreen->Font->ta_Flags;
		}
	}

	if(!Config->ScreenConfig->UseWorkbench)
	{
		GetDPI(Config->ScreenConfig->DisplayMode,&X_DPI,&Y_DPI);

		strcpy(UserFontName,Config->ScreenConfig->FontName);

		UserFont.tta_Name	= UserFontName;
		UserFont.tta_YSize	= Config->ScreenConfig->FontHeight;
		UserFont.tta_Style	= FS_NORMAL | FSF_TAGGED;
		UserFont.tta_Flags	= FPF_DESIGNED;
		UserFont.tta_Tags	= TagDPI;

		TagDPI[0].ti_Tag	 = TA_DeviceDPI;
		TagDPI[0].ti_Data	 = (X_DPI << 16) | Y_DPI;
		TagDPI[1].ti_Tag	 = TAG_DONE;
	}

	if(!(UserTextFont = SmartOpenDiskFont((struct TextAttr *)&UserFont)))
	{
		if(Config->ScreenConfig->UseWorkbench)
		{
			ReleaseDefaultPubScreen();

			return(LocaleString(MSG_TERMINIT_UNABLE_TO_OPEN_FONT_TXT));
		}
		else
		{
			strcpy(Config->ScreenConfig->FontName,	"topaz.font");
			strcpy(UserFontName,				"topaz.font");

			Config->ScreenConfig->FontHeight = 8;

			UserFont.tta_YSize	= Config->ScreenConfig->FontHeight;
			UserFont.tta_Style	= FS_NORMAL;
			UserFont.tta_Flags	= FPF_DESIGNED | FPF_ROMFONT;

			if(!(UserTextFont = OpenFont((struct TextAttr *)&UserFont)))
			{
				ReleaseDefaultPubScreen();

				return(LocaleString(MSG_TERMINIT_UNABLE_TO_OPEN_FONT_TXT));
			}
		}
	}

		/* Open the default text rendering font. */

	do
	{
			/* If it's not the standard font, use the IBM PC style font. */

		if(Config->TerminalConfig->FontMode != FONT_STANDARD)
		{
			strcpy(TextFontName,Config->TerminalConfig->IBMFontName);

			TextAttr.tta_YSize = Config->TerminalConfig->IBMFontHeight;
		}
		else
		{
			strcpy(TextFontName,Config->TerminalConfig->TextFontName);

			TextAttr.tta_YSize = Config->TerminalConfig->TextFontHeight;
		}

		TextAttr.tta_Name	= TextFontName;
		TextAttr.tta_Style	= FS_NORMAL | FSF_TAGGED;
		TextAttr.tta_Flags	= FPF_DESIGNED;
		TextAttr.tta_Tags	= TagDPI;

			/* Does it open? */

		if(!(TextFont = SmartOpenDiskFont((struct TextAttr *)&TextAttr)))
		{
				/* So it didn't open. Revert to the standard text font. */

			if(Config->TerminalConfig->FontMode != FONT_STANDARD)
				Config->TerminalConfig->FontMode = FONT_STANDARD;
			else
			{
					/* So this is the standard font. Is it the ROM default font? */

				if(!Stricmp(Config->TerminalConfig->TextFontName,"topaz.font") && Config->TerminalConfig->TextFontHeight == 8)
				{
						/* Looks like you lose. */

					ReleaseDefaultPubScreen();

					return(LocaleString(MSG_TERMINIT_UNABLE_TO_OPEN_TEXT_TXT));
				}
				else
				{
						/* Use the ROM default font. */

					strcpy(Config->TerminalConfig->TextFontName,"topaz.font");
					Config->TerminalConfig->TextFontHeight = 8;
				}
			}
		}
	}
	while(TextFont == NULL);

	TextFontHeight	= TextFont->tf_YSize;
	TextFontWidth	= TextFont->tf_XSize;
	TextFontBase	= TextFont->tf_Baseline;

		/* Determine extra font box width for slanted/boldface glyphs. */

	FontRightExtend	= MAX(TextFont->tf_XSize / 2,TextFont->tf_BoldSmear);

	CurrentFont = TextFont;

	GFXFont.ta_YSize = TextFontHeight;

	if(GFX = SmartOpenDiskFont(&GFXFont))
	{
		if(GFX->tf_XSize != TextFont->tf_XSize || GFX->tf_YSize != TextFont->tf_YSize)
		{
			CloseFont(GFX);

			GFX = NULL;
		}
	}

	UserFontHeight	= UserTextFont->tf_YSize;
	UserFontWidth	= UserTextFont->tf_XSize;
	UserFontBase	= UserTextFont->tf_Baseline;

		/* We will need this one later */

	OpenWindowTag = WA_CustomScreen;

	if(Config->ScreenConfig->UseWorkbench || Config->ScreenConfig->ShareScreen)
	{
		LONG WindowLeft,WindowTop,DummyWidth,DummyHeight;
		LONG FullWidth,Height,Width,Index;
		struct ViewPortExtra *Extra;
		struct TagItem SomeTags[7];
		struct Screen *LocalScreen;

		Index = 0;
		LocalScreen = DefaultPubScreen;
		WindowLeft = WindowTop = DummyWidth = DummyHeight = -1;

		if(Config->ScreenConfig->ShareScreen && !Config->ScreenConfig->UseWorkbench)
		{
			struct DimensionInfo DimensionInfo;

			if(ModeNotAvailable(Config->ScreenConfig->DisplayMode))
			{
				Config->ScreenConfig->DisplayMode = GetVPModeID(&DefaultPubScreen->ViewPort);

				if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Config->ScreenConfig->DisplayMode))
				{
					LONG Width,Height;

					Width	= DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1;
					Height	= DimensionInfo.TxtOScan.MaxY - DimensionInfo.TxtOScan.MinY + 1;

					if(Width != Config->ScreenConfig->DisplayWidth && Config->ScreenConfig->DisplayWidth)
						Config->ScreenConfig->DisplayWidth = Width;

					if(Height != Config->ScreenConfig->DisplayHeight && Config->ScreenConfig->DisplayHeight)
						Config->ScreenConfig->DisplayHeight = Height;
				}
			}

			if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Config->ScreenConfig->DisplayMode))
			{
				LONG Depth,ScreenWidth,ScreenHeight;
				UWORD Pens;

				Pens = (UWORD)~0;

				if(Config->ScreenConfig->DisplayWidth && Config->ScreenConfig->DisplayHeight && AslBase->lib_Version >= 38)
				{
					ScreenWidth		= Config->ScreenConfig->DisplayWidth;
					ScreenHeight	= Config->ScreenConfig->DisplayHeight;
				}
				else
				{
					ScreenWidth		= 0;
					ScreenHeight	= 0;
				}

				switch(Config->ScreenConfig->ColourMode)
				{
					case COLOUR_EIGHT:

						Depth = 3 + 1;
						break;

					case COLOUR_SIXTEEN:

						Depth = 4 + 1;
						break;

					case COLOUR_AMIGA:

						Depth = 2;
						break;

					default:

						Depth = 1;
						break;
				}

				if(Depth > DimensionInfo.MaxDepth)
					Depth = DimensionInfo.MaxDepth;

				if(!Kick30 && Depth > 2)
					Depth = 2;

				if(Config->ScreenConfig->Depth && Config->ScreenConfig->Depth <= DimensionInfo.MaxDepth)
					Depth = Config->ScreenConfig->Depth;

				switch(Config->ScreenConfig->ColourMode)
				{
					case COLOUR_EIGHT:

						if(Depth < 3 + 1)
							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

						break;

					case COLOUR_SIXTEEN:

						if(Depth < 4 + 1)
						{
							if(Depth < 3 + 1)
								Config->ScreenConfig->ColourMode = COLOUR_AMIGA;
							else
								Config->ScreenConfig->ColourMode = COLOUR_EIGHT;
						}

						break;
				}

				LimitedSPrintf(sizeof(ScreenTitle),ScreenTitle,LocaleString(MSG_TERMINIT_SCREENTITLE_TXT),TermName,Machine,TermDate,TermIDString);

				StatusSizeSetup(NULL,&StatusWidth,&StatusHeight);

				if(StatusHeight && StatusWidth > ScreenWidth)
					ScreenWidth = StatusWidth;

				if(SharedScreen = OpenScreenTags(NULL,
					ScreenWidth  ? SA_Width  : TAG_IGNORE,	ScreenWidth,
					ScreenHeight ? SA_Height : TAG_IGNORE,	ScreenHeight,

					SA_Title,		ScreenTitle,
					SA_Depth,		Depth,
					SA_Pens,		&Pens,
					SA_Overscan,	AslBase->lib_Version >= 38 ? Config->ScreenConfig->OverscanType : OSCAN_TEXT,
					SA_DisplayID,	Config->ScreenConfig->DisplayMode,
					SA_Font,		&UserFont,
					SA_AutoScroll,	TRUE,
					SA_ShowTitle,	Config->ScreenConfig->TitleBar,
					SA_PubName,		TermIDString,
					SA_Interleaved,	Config->ScreenConfig->FasterLayout,
					SA_SharePens,	TRUE,
				TAG_DONE))
				{
					LocalScreen = SharedScreen;

					if(Config->ScreenConfig->MakeScreenPublic)
						PubScreenStatus(LocalScreen,0);
					else
						PubScreenStatus(LocalScreen,PSNF_PRIVATE);
				}
			}
		}

		if(!SharedScreen)
			LimitedSPrintf(sizeof(ScreenTitle),ScreenTitle,"%s %s(%s)",TermName,Machine,TermDate);

		StatusSizeSetup(LocalScreen,&StatusWidth,&StatusHeight);

		if(GetBitMapDepth(LocalScreen->RastPort.BitMap) == 1 || Config->ScreenConfig->FasterLayout)
			UseMasking = FALSE;
		else
		{
			if(Kick30)
			{
				if(GetBitMapAttr(LocalScreen->RastPort.BitMap,BMA_FLAGS) & BMF_INTERLEAVED)
					UseMasking = FALSE;
				else
					UseMasking = TRUE;
			}
			else
				UseMasking = TRUE;
		}

		VPort = &LocalScreen->ViewPort;

			/* Get the current display dimensions. */

		if(Extra = GetViewPortExtra(VPort))
		{
			ScreenWidth		= Extra->DisplayClip.MaxX - Extra->DisplayClip.MinX + 1;
			ScreenHeight	= Extra->DisplayClip.MaxY - Extra->DisplayClip.MinY + 1;
		}
		else
		{
			ScreenWidth		= LocalScreen->Width;
			ScreenHeight	= LocalScreen->Height;
		}

		DepthMask = (1L << GetBitMapDepth(LocalScreen->RastPort.BitMap)) - 1;

		switch(Config->ScreenConfig->ColourMode)
		{
			case COLOUR_SIXTEEN:

				if(DepthMask < 15)
				{
					if(DepthMask >= 7)
						Config->ScreenConfig->ColourMode = COLOUR_EIGHT;
					else
					{
						if(DepthMask >= 3)
							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;
						else
							Config->ScreenConfig->ColourMode = COLOUR_MONO;
					}
				}

				break;

			case COLOUR_EIGHT:

				if(DepthMask < 7)
				{
					if(DepthMask >= 3)
						Config->ScreenConfig->ColourMode = COLOUR_AMIGA;
					else
						Config->ScreenConfig->ColourMode = COLOUR_MONO;
				}

				break;

			case COLOUR_AMIGA:

				if(DepthMask < 3)
					Config->ScreenConfig->ColourMode = COLOUR_MONO;

				break;
		}

		if(!(DrawInfo = GetScreenDrawInfo(LocalScreen)))
		{
			ReleaseDefaultPubScreen();

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OBTAIN_SCREEN_DRAWINFO_TXT));
		}
		else
			Pens = DrawInfo->dri_Pens;

		CreateMenuGlyphs(LocalScreen,DrawInfo,&AmigaGlyph,&CheckGlyph);

		SZ_SizeSetup(LocalScreen,(struct TextAttr *)&UserFont);

			/* Obtain visual info (whatever that may be). */

		if(!(VisualInfo = GetVisualInfo(LocalScreen,TAG_DONE)))
		{
				/* Delete the DrawInfo now, or it won't
				 * get freed during shutdown.
				 */

			FreeScreenDrawInfo(LocalScreen,DrawInfo);
			DrawInfo = NULL;

			ReleaseDefaultPubScreen();

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OBTAIN_VISUAL_INFO_TXT));
		}

		if(Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && !Config->ScreenConfig->SplitStatus)
			FullWidth = StatusWidth;
		else
			FullWidth = 0;

		GetWindowInfo(WINDOW_MAIN,&WindowLeft,&WindowTop,&DummyWidth,&DummyHeight,0,0);

		if(UsePresetSize)
		{
			SomeTags[Index  ].ti_Tag	= WA_Width;
			SomeTags[Index++].ti_Data	= DummyWidth;

			SomeTags[Index  ].ti_Tag	= WA_Height;
			SomeTags[Index++].ti_Data	= DummyHeight;
		}
		else
		{
			if(Config->TerminalConfig->NumColumns < 20)
			{
				LONG Width = GetScreenWidth(NULL);

				if(FullWidth && Width < FullWidth)
				{
					SomeTags[Index  ].ti_Tag	= WA_InnerWidth;
					SomeTags[Index++].ti_Data	= FullWidth;
				}
				else
				{
					SomeTags[Index  ].ti_Tag	= WA_Width;
					SomeTags[Index++].ti_Data	= Width;
				}
			}
			else
			{
				SomeTags[Index  ].ti_Tag	= WA_InnerWidth;
				SomeTags[Index++].ti_Data	= Config->TerminalConfig->NumColumns * TextFontWidth;
			}

			if(Config->TerminalConfig->NumLines < 20)
			{
				SomeTags[Index  ].ti_Tag	= WA_Height;
				SomeTags[Index++].ti_Data	= GetScreenHeight(NULL) - (LocalScreen->BarHeight + 1);
			}
			else
			{
				SomeTags[Index  ].ti_Tag	= WA_InnerHeight;
				SomeTags[Index++].ti_Data	= Config->TerminalConfig->NumLines * TextFontHeight + StatusHeight;
			}
		}

		if(WindowLeft != -1)
		{
			SomeTags[Index  ].ti_Tag	= WA_Left;
			SomeTags[Index++].ti_Data	= WindowLeft;
		}
		else
		{
			SomeTags[Index  ].ti_Tag	= WA_Left;
			SomeTags[Index++].ti_Data	= GetScreenLeft(NULL);
		}

		if(WindowTop != -1)
		{
			SomeTags[Index  ].ti_Tag	= WA_Top;
			SomeTags[Index++].ti_Data	= WindowTop;
		}

		SomeTags[Index].ti_Tag = TAG_DONE;

			/* If we're using a public screen, take care of it. */

		if(!SharedScreen)
			OpenWindowTag = WA_PubScreen;

			/* Open the main window. */

		if(!(Window = OpenWindowTags(NULL,
			WA_MaxHeight,		LocalScreen->Height,
			WA_MaxWidth,		LocalScreen->Width,
			WA_SmartRefresh,	TRUE,
			WA_NewLookMenus,	TRUE,
			WA_RMBTrap,			TRUE,
			WA_IDCMP,			DEFAULT_IDCMP | IDCMP_SIZEVERIFY,
			WA_DragBar,			TRUE,
			WA_DepthGadget,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		Config->ScreenConfig->StatusLine == STATUSLINE_DISABLED || Config->ScreenConfig->SplitStatus,
			WA_NoCareRefresh,	TRUE,
			WA_Title,			WindowTitle[0] ? WindowTitle : ScreenTitle,
			WA_MenuHelp,		TRUE,
			WA_Activate,		Activate,
			OpenWindowTag,		LocalScreen,

			AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
			CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,

		TAG_MORE,SomeTags)))
		{
				/* Delete the DrawInfo now, or it won't
				 * get freed during shutdown.
				 */

			FreeScreenDrawInfo(LocalScreen,DrawInfo);
			DrawInfo = NULL;

			ReleaseDefaultPubScreen();

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_WINDOW_TXT));
		}

		if(WindowTitle[0])
			SetWindowTitles(Window,(STRPTR)-1,ScreenTitle);

		if(StatusHeight)
		{
			StatusDisplayHeight = StatusHeight;

			CopyMem(Window->RPort,StatusRPort = &StatusRastPort,sizeof(struct RastPort));
		}
		else
			StatusRPort = NULL;

			/* Create a user clip region to keep text from
			 * leaking into the window borders.
			 */

		if(!(ClipRegion = NewRegion()))
		{
			ReleaseDefaultPubScreen();

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_WINDOW_TXT));
		}
		else
		{
			struct Rectangle RegionRectangle;

				/* Adjust the region to match the inner window area. */

			RegionRectangle.MinX = Window->BorderLeft;
			RegionRectangle.MinY = Window->BorderTop;
			RegionRectangle.MaxX = Window->Width - (Window->BorderRight + 1);
			RegionRectangle.MaxY = Window->Height - (Window->BorderBottom + 1);

				/* Establish the region. */

			OrRectRegion(ClipRegion,&RegionRectangle);

				/* Install the region. */

			OldRegion = InstallClipRegion(Window->WLayer,ClipRegion);
		}

		if(FullWidth < 40 * TextFontWidth)
			FullWidth = 40 * TextFontWidth;

		Width	= Window->BorderLeft + FullWidth + Window->BorderRight;
		Height	= Window->BorderTop + 20 * TextFontHeight + Window->BorderBottom + StatusHeight;

		if(ChatMode && !(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0]))
			Height += UserFontHeight + 2;

		WindowLimits(Window,Width,Height,0,0);

		ReleaseDefaultPubScreen();
	}
	else
	{
		struct DimensionInfo DimensionInfo;
		LONG MaxDepth,ScreenDepth;

		if(ModeNotAvailable(Config->ScreenConfig->DisplayMode))
		{
			struct Screen *PubScreen;

			if(PubScreen = LockPubScreen(NULL))
			{
				Config->ScreenConfig->DisplayMode = GetVPModeID(&PubScreen->ViewPort);

				if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Config->ScreenConfig->DisplayMode))
				{
					LONG Width,Height;

					Width	= DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1;
					Height	= DimensionInfo.TxtOScan.MaxY - DimensionInfo.TxtOScan.MinY + 1;

					if(Width != Config->ScreenConfig->DisplayWidth && Config->ScreenConfig->DisplayWidth)
						Config->ScreenConfig->DisplayWidth = Width;

					if(Height != Config->ScreenConfig->DisplayHeight && Config->ScreenConfig->DisplayHeight)
						Config->ScreenConfig->DisplayHeight = Height;
				}

				UnlockPubScreen(NULL,PubScreen);
			}
		}

			/* Query the display size. If it fails, back out immediately. */

		if(!QueryOverscan(Config->ScreenConfig->DisplayMode,&DisplayClip,Config->ScreenConfig->OverscanType))
			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_SCREEN_TXT));

		if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Config->ScreenConfig->DisplayMode))
		{
			LONG MaxWidth,MaxHeight,Width,Height;

			MaxWidth	= DisplayClip.MaxX - DisplayClip.MinX + 1;
			MaxHeight	= DisplayClip.MaxY - DisplayClip.MinY + 1;

			if(Config->ScreenConfig->DisplayWidth && Config->ScreenConfig->DisplayHeight && AslBase->lib_Version >= 38)
			{
				ScreenWidth		= Config->ScreenConfig->DisplayWidth;
				ScreenHeight	= Config->ScreenConfig->DisplayHeight;
			}
			else
			{
				ScreenWidth		= MaxWidth;
				ScreenHeight	= MaxHeight;
			}

			if(Config->TerminalConfig->NumColumns < 20)
				Width = MaxWidth = ScreenWidth;
			else
			{
				Width = TextFontWidth * Config->TerminalConfig->NumColumns;

				ScreenWidth = 0;
			}

			if(Config->TerminalConfig->NumLines < 20)
				Height = MaxHeight = ScreenHeight;
			else
			{
				Height = TextFontHeight * Config->TerminalConfig->NumLines;

				if(Config->ScreenConfig->TitleBar)
					Height += UserFontHeight + 3;

				if(Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED)
				{
					if(Config->ScreenConfig->StatusLine == STATUSLINE_COMPRESSED)
						Height += UserFontHeight;
					else
						Height += 4 + (2 + 2 * UserFontHeight + 2);
				}

				ScreenHeight = 0;
			}

			if(Height > MaxHeight)
				Height = MaxHeight;

			if(Width > MaxWidth)
				Width = MaxWidth;

			if(DimensionInfo.MinRasterWidth <= Width && Width <= DimensionInfo.MaxRasterWidth && Width < MaxWidth)
			{
				LONG Half;

				Width = MaxWidth - Width;

				Half = Width / 2;

				DisplayClip.MinX += Half;
				DisplayClip.MaxX -= Width - Half;
			}

			if(DimensionInfo.MinRasterHeight <= Height && Height <= DimensionInfo.MaxRasterHeight)
				DisplayClip.MaxY = DisplayClip.MinY + Height - 1;

			if(!ScreenWidth)
				ScreenWidth = DisplayClip.MaxX - DisplayClip.MinX + 1;

			if(!ScreenHeight)
				ScreenHeight = DisplayClip.MaxY - DisplayClip.MinY + 1;

			MaxDepth = DimensionInfo.MaxDepth;
		}
		else
		{
			ScreenWidth = ScreenHeight = 0;
			MaxDepth = 4;
		}

			/* We'll configure the screen parameters at
			 * run time, at first we'll set up the screen
			 * depth.
			 */

		do
		{
			RethinkPens = FALSE;

			if(!Config->ScreenConfig->UsePens && Kick30)
			{
				for(i = DETAILPEN ; i <= BARTRIMPEN ; i++)
					PenArray[i] = Config->ScreenConfig->PenArray[i];

				PenArray[i] = (UWORD)~0;
			}
			else
			{
				UWORD *Data;

				switch(Config->ScreenConfig->ColourMode)
				{
					case COLOUR_EIGHT:

						Data = ANSIPens;
						break;

					case COLOUR_SIXTEEN:

						if(Kick30)
							Data = NewEGAPens;
						else
							Data = EGAPens;

						break;

					case COLOUR_AMIGA:

						Data = StandardPens;
						break;

					default:

						Data = NULL;
						break;
				}

				if(Data)
				{
					for(i = DETAILPEN ; i <= BARTRIMPEN ; i++)
						PenArray[i] = Data[i];

					PenArray[i] = (UWORD)~0;
				}
			}

			switch(Config->ScreenConfig->ColourMode)
			{
				case COLOUR_EIGHT:

						/* Special screen depth requested? */

					if(Config->ScreenConfig->Depth)
					{
							/* The minimum number of colours required */

						if(Config->ScreenConfig->Blinking)
							ScreenDepth = 4;
						else
							ScreenDepth = 3;

							/* This is what the user wanted */

						RealDepth = Config->ScreenConfig->Depth;

							/* Too deep for this display mode? */

						if(RealDepth > MaxDepth)
							RealDepth = MaxDepth;

							/* Less colours than required? */

						if(RealDepth < ScreenDepth && ScreenDepth <= MaxDepth)
							RealDepth = ScreenDepth;

							/* Not enough colours to display it? */

						if(RealDepth < ScreenDepth)
						{
								/* Return to standard mode */

							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

							ConfigChanged = TRUE;

							RethinkPens = TRUE;

							break;
						}
					}
					else
					{
							/* The minimum number of colours */

						if(Config->ScreenConfig->Blinking)
							ScreenDepth = 4;
						else
							ScreenDepth = 3;

							/* Too many for this mode? */

						if(ScreenDepth > MaxDepth)
						{
								/* Return to standard mode */

							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

							ConfigChanged = TRUE;
							RethinkPens = TRUE;

							break;
						}

						RealDepth = ScreenDepth;
					}

					TagArray[Count++] = SA_Pens;
					TagArray[Count++] = (IPTR)PenArray;

					TagArray[Count++] = SA_BlockPen;
					TagArray[Count++] = PenArray[SHADOWPEN];

					TagArray[Count++] = SA_DetailPen;
					TagArray[Count++] = PenArray[BACKGROUNDPEN];

					break;

				case COLOUR_SIXTEEN:

					if(Config->ScreenConfig->Depth)
					{
						if(Config->ScreenConfig->Blinking && MaxDepth > 4)
							ScreenDepth = 5;
						else
							ScreenDepth = 4;

						RealDepth = Config->ScreenConfig->Depth;

						if(RealDepth > MaxDepth)
							RealDepth = MaxDepth;

						if(RealDepth < ScreenDepth && ScreenDepth <= MaxDepth)
							RealDepth = ScreenDepth;

						if(RealDepth < ScreenDepth)
						{
							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

							ConfigChanged = TRUE;
							RethinkPens = TRUE;

							break;
						}
					}
					else
					{
						if(Config->ScreenConfig->Blinking && MaxDepth > 4)
							ScreenDepth = 5;
						else
							ScreenDepth = 4;

						if(ScreenDepth > MaxDepth)
						{
							Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

							ConfigChanged = TRUE;
							RethinkPens = TRUE;

							break;
						}

						RealDepth = ScreenDepth;
					}

					TagArray[Count++] = SA_Pens;
					TagArray[Count++] = (IPTR)PenArray;

					TagArray[Count++] = SA_BlockPen;
					TagArray[Count++] = PenArray[SHADOWPEN];

					TagArray[Count++] = SA_DetailPen;
					TagArray[Count++] = PenArray[BACKGROUNDPEN];

					break;

				case COLOUR_MONO:

					if(Config->ScreenConfig->Depth)
						RealDepth = Config->ScreenConfig->Depth;
					else
						RealDepth = 1;

					if(RealDepth > MaxDepth)
						RealDepth = MaxDepth;

					break;

				case COLOUR_AMIGA:

					if(Config->ScreenConfig->Depth)
						RealDepth = Config->ScreenConfig->Depth;
					else
						RealDepth = 2;

					if(RealDepth > MaxDepth)
						RealDepth = MaxDepth;

					TagArray[Count++] = SA_Pens;
					TagArray[Count++] = (IPTR)PenArray;

					TagArray[Count++] = SA_BlockPen;
					TagArray[Count++] = PenArray[SHADOWPEN];

					TagArray[Count++] = SA_DetailPen;
					TagArray[Count++] = PenArray[BACKGROUNDPEN];

					break;
			}
		}
		while(RethinkPens);

			/* Add the depth value. */

		TagArray[Count++] = SA_Depth;
		TagArray[Count++] = RealDepth;

			/* Terminate the tag array. */

		TagArray[Count] = TAG_END;

			/* Set the plane mask. */

		DepthMask = (1L << RealDepth) - 1;

		LimitedSPrintf(sizeof(ScreenTitle),ScreenTitle,LocaleString(MSG_TERMINIT_SCREENTITLE_TXT),TermName,Machine,TermDate,TermIDString);

		StatusSizeSetup(NULL,&StatusWidth,&StatusHeight);

		if(StatusHeight && StatusWidth > ScreenWidth)
			ScreenWidth = StatusWidth;

		do
		{
			if(Screen = (struct Screen *)OpenScreenTags(NULL,
				ScreenWidth  ? SA_Width  : TAG_IGNORE,	ScreenWidth,
				ScreenHeight ? SA_Height : TAG_IGNORE,	ScreenHeight,

				SA_Title,		ScreenTitle,
				SA_Left,		DisplayClip.MinX,
				SA_DClip,		&DisplayClip,
				SA_DisplayID,	Config->ScreenConfig->DisplayMode,
				SA_Font,		&UserFont,
				SA_Behind,		!Activate,
				SA_AutoScroll,	TRUE,
				SA_ShowTitle,	Config->ScreenConfig->TitleBar,
				SA_PubName,		TermIDString,
				SA_ErrorCode,	&ErrorCode,
				SA_Interleaved,	Config->ScreenConfig->FasterLayout && Kick30 && RealDepth > 1,
				SA_BackFill,	&BackfillHook,
			TAG_MORE,TagArray))
			{
				if(RealDepth > 1 && !Config->ScreenConfig->FasterLayout)
				{
					UseMasking = TRUE;

					if(Kick30)
					{
						if(GetBitMapAttr(Screen->RastPort.BitMap,BMA_FLAGS) & BMF_INTERLEAVED)
							UseMasking = FALSE;
					}
				}
				else
					UseMasking = FALSE;
			}

				/* So we've got an error. */

			if(!Screen)
			{
				if(OpenFailed)
					return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_SCREEN_TXT));
				else
				{
					struct Screen *PubScreen;

					switch(ErrorCode)
					{
							/* Can't open screen with these display
							 * modes.
							 */

						case OSERR_NOMONITOR:
						case OSERR_NOCHIPS:
						case OSERR_UNKNOWNMODE:
						case OSERR_NOTAVAILABLE:

							if(PubScreen = LockPubScreen(NULL))
							{
								struct DimensionInfo DimensionInfo;

								Config->ScreenConfig->DisplayMode = GetVPModeID(&PubScreen->ViewPort);

								if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,Config->ScreenConfig->DisplayMode))
								{
									LONG Width,Height;

									Width	= DimensionInfo.TxtOScan.MaxX - DimensionInfo.TxtOScan.MinX + 1;
									Height	= DimensionInfo.TxtOScan.MaxY - DimensionInfo.TxtOScan.MinY + 1;

									if(Width != Config->ScreenConfig->DisplayWidth && Config->ScreenConfig->DisplayWidth)
										Config->ScreenConfig->DisplayWidth = Width;

									if(Height != Config->ScreenConfig->DisplayHeight && Config->ScreenConfig->DisplayHeight)
										Config->ScreenConfig->DisplayHeight = Height;
								}

								UnlockPubScreen(NULL,PubScreen);
							}
							else
								Config->ScreenConfig->DisplayMode = DEFAULT_MONITOR_ID | HIRESLACE_KEY;

							OpenFailed = TRUE;

							break;

						case OSERR_PUBNOTUNIQUE:

							return(LocaleString(MSG_TERMINIT_SCREEN_ID_ALREADY_IN_USE_TXT));

							/* Some different error, probably out of
							 * memory.
							 */

						default:

							return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_SCREEN_TXT));
					}
				}
			}
		}
		while(Screen == NULL);

		if(!(DrawInfo = GetScreenDrawInfo(Screen)))
			return(LocaleString(MSG_TERMINIT_FAILED_TO_OBTAIN_SCREEN_DRAWINFO_TXT));
		else
			Pens = DrawInfo->dri_Pens;

		CreateMenuGlyphs(Screen,DrawInfo,&AmigaGlyph,&CheckGlyph);

		VPort = &Screen->ViewPort;

		ScreenWidth		= Screen->Width;
		ScreenHeight	= Screen->Height;

		StatusSizeSetup(Screen,&StatusWidth,&StatusHeight);

			/* Obtain visual info (whatever that may be). */

		if(!(VisualInfo = GetVisualInfo(Screen,TAG_DONE)))
		{
				/* Delete the DrawInfo now, or it won't
				 * get freed during shutdown.
				 */

			FreeScreenDrawInfo(Screen,DrawInfo);

			DrawInfo = NULL;

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OBTAIN_VISUAL_INFO_TXT));
		}

		if(Config->ScreenConfig->TitleBar)
		{
			Top = Screen->BarHeight + 1;

			Height = Screen->Height - (Screen->BarHeight + 1);
		}
		else
		{
			Top = 0;

			Height = Screen->Height;
		}

			/* Open the main window. */

		if(!(Window = OpenWindowTags(NULL,
			WA_Top,				Top,
			WA_Left,			0,
			WA_Width,			Screen->Width,
			WA_Height,			Height,
			WA_Backdrop,		TRUE,
			WA_Borderless,		TRUE,
			WA_CustomScreen,	Screen,
			WA_NewLookMenus,	TRUE,
			WA_RMBTrap,			TRUE,
			WA_IDCMP,			DEFAULT_IDCMP & ~IDCMP_CLOSEWINDOW,
			WA_MenuHelp,		TRUE,
			WA_NoCareRefresh,	TRUE,
			WA_Activate,		Activate,

			AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
			CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,
		TAG_DONE)))
		{
				/* Delete the DrawInfo now, or it won't
				 * get freed during shutdown.
				 */

			FreeScreenDrawInfo(Screen,DrawInfo);

			DrawInfo = NULL;

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_WINDOW_TXT));
		}

		if(StatusHeight)
		{
			StatusDisplayHeight = StatusHeight;

			CopyMem(Window->RPort,StatusRPort = &StatusRastPort,sizeof(struct RastPort));
		}
		else
			StatusRPort = NULL;
	}

		/* Now check if a nonzero background colour should be used. */

	if(Pens[BACKGROUNDPEN])
		BackfillTag = WA_BackFill;
	else
		BackfillTag = TAG_IGNORE;

		/* Fill the `default' colour with current values. */

	if(!Config->ScreenConfig->UseWorkbench && Initializing && !SharedScreen)
	{
		if(Kick30)
		{
			ULONG RGB[3 * 16];

			GetRGB32(VPort->ColorMap,0,16,RGB);

			ANSIColourTable		= CreateColourTable(8,	ANSIColours,	NULL);
			EGAColourTable		= CreateColourTable(16,	EGAColours,	NULL);
			DefaultColourTable	= CreateColourTable(16,	NULL,		RGB);
			MonoColourTable		= CreateColourTable(2,	AtomicColours,	NULL);
		}

		for(i = 0 ; i < 16 ; i++)
			DefaultColours[i] = GetRGB4(VPort->ColorMap,i);

		Initializing = FALSE;
	}

		/* Load the approriate colours. */

	if(LoadColours)
	{
		Default2CurrentPalette(Config);

		LoadColours = FALSE;
	}

		/* Reset the current colours and the blinking equivalents. */

	PaletteSetup(Config);

	if(!Config->ScreenConfig->UseWorkbench && !SharedScreen)
		LoadColourTable(VPort,NormalColourTable,NormalColours,PaletteSize);

		/* Get the vanilla rendering pens. */

	RenderPens[0] = Pens[BACKGROUNDPEN];
	RenderPens[1] = Pens[TEXTPEN];
	RenderPens[2] = Pens[SHINEPEN];
	RenderPens[3] = Pens[FILLPEN];

		/* Are we to use the Workbench screen for text output? */

	if(Config->ScreenConfig->UseWorkbench || (SharedScreen && Kick30))
	{
		BOOL UseDefaults;

		if(Kick30 && (Config->ScreenConfig->ColourMode == COLOUR_EIGHT || Config->ScreenConfig->ColourMode == COLOUR_SIXTEEN))
		{
			LONG NumPens;
			ULONG R,G,B;
			BOOL GotAll;

			UseDefaults = FALSE;

			do
			{
				if(Config->ScreenConfig->ColourMode == COLOUR_EIGHT)
					NumPens = 8;
				else
					NumPens = 16;

				for(i = 0 ; i < 32 ; i++)
					MappedPens[1][i] = FALSE;

				GotAll = TRUE;

					/* Allocate the text rendering pens, note that
					 * we will use the currently installed palette
					 * to obtain those pens which match them best.
					 * The user will be unable to change these
					 * colours.
					 */

				if(NormalColourTable)
				{
					for(i = 0 ; i < NumPens ; i++)
					{
							/* Try to obtain a matching pen. */

						if((MappedPens[0][i] = ObtainBestPen(VPort->ColorMap,NormalColourTable->Entry[i].Red,NormalColourTable->Entry[i].Green,NormalColourTable->Entry[i].Blue,
							OBP_FailIfBad,TRUE,
						TAG_DONE)) == -1)
						{
							MappedPens[1][i] = FALSE;

							GotAll = FALSE;

							break;
						}
						else
							MappedPens[1][i] = TRUE;
					}
				}
				else
				{
					for(i = 0 ; i < NumPens ; i++)
					{
							/* Split the 12 bit colour palette entry. */

						R = (NormalColours[i] >> 8) & 0xF;
						G = (NormalColours[i] >> 4) & 0xF;
						B =  NormalColours[i]       & 0xF;

							/* Try to obtain a matching pen. */

						if((MappedPens[0][i] = ObtainBestPen(VPort->ColorMap,SPREAD((R << 4) | R),SPREAD((G << 4) | G),SPREAD((B << 4) | B),
							OBP_FailIfBad,TRUE,
						TAG_DONE)) == -1)
						{
							MappedPens[1][i] = FALSE;

							GotAll = FALSE;

							break;
						}
						else
							MappedPens[1][i] = TRUE;
					}
				}

					/* Did we get what we wanted? */

				if(!GotAll)
				{
						/* Release all the pens we succeeded
						 * in allocating.
						 */

					for(i = 0 ; i < NumPens ; i++)
					{
						if(MappedPens[1][i])
							ReleasePen(VPort->ColorMap,MappedPens[0][i]);
					}

						/* Check if we could step back */

					if(Config->ScreenConfig->ColourMode = COLOUR_SIXTEEN)
						Config->ScreenConfig->ColourMode = COLOUR_EIGHT;
					else
					{
						Config->ScreenConfig->ColourMode = COLOUR_AMIGA;

						UseDefaults = TRUE;
					}
				}
				else
					AllocatedPens = TRUE;
			}
			while(!AllocatedPens && !UseDefaults);
		}
		else
			UseDefaults = TRUE;

			/* Use the default rendering pens? */

		if(UseDefaults)
		{
			if(Config->ScreenConfig->ColourMode == COLOUR_AMIGA)
			{
				for(i = 0 ; i < 4 ; i++)
				{
					MappedPens[0][i] = RenderPens[i];
					MappedPens[1][i] = FALSE;
				}

					/* Set the remaining pens to defaults. */

				for(i = 4 ; i < 16 ; i++)
				{
					MappedPens[0][i] = RenderPens[1];
					MappedPens[1][i] = FALSE;
				}
			}
			else
			{
				for(i = 0 ; i < 16 ; i++)
				{
					MappedPens[0][i] = RenderPens[i & 1];
					MappedPens[1][i] = FALSE;
				}
			}
		}

		for(i = 0 ; i < 16 ; i++)
		{
			MappedPens[0][i + 16] = MappedPens[0][i];
			MappedPens[1][i + 16] = FALSE;
		}
	}
	else
	{
			/* Reset the colour translation table. */

		for(i = 0 ; i < 32 ; i++)
		{
			MappedPens[0][i] = i;
			MappedPens[1][i] = FALSE;
		}
	}

		/* Determine default text rendering colour. */

	switch(Config->ScreenConfig->ColourMode)
	{
		case COLOUR_SIXTEEN:

			SafeTextPen = MappedPens[0][7];
			break;

		case COLOUR_EIGHT:

			SafeTextPen = MappedPens[0][7];
			break;

		case COLOUR_AMIGA:
		case COLOUR_MONO:

			SafeTextPen = MappedPens[0][1];
			break;
	}

		/* Take care of pen and attribute mapping. */

	if(Config->EmulationConfig->UseStandardPens)
	{
		for(i = 0 ; i < 16 ; i++)
		{
			PenTable[i]		= i;
			TextAttributeTable[i]	= i;
		}
	}
	else
	{
		UBYTE Value;

		CopyMem(Config->EmulationConfig->Pens,PenTable,sizeof(Config->EmulationConfig->Pens));

		for(i = 0 ; i < 16 ; i++)
		{
			Value = 0;

			if(i & ATTR_UNDERLINE)
			{
				switch(Config->EmulationConfig->Attributes[TEXTATTR_UNDERLINE])
				{
					case TEXTATTR_UNDERLINE:

						Value |= ATTR_UNDERLINE;
						break;

					case TEXTATTR_HIGHLIGHT:

						Value |= ATTR_HIGHLIGHT;
						break;

					case TEXTATTR_BLINK:

						Value |= ATTR_BLINK;
						break;

					case TEXTATTR_INVERSE:

						Value |= ATTR_INVERSE;
						break;
				}
			}

			if(i & ATTR_HIGHLIGHT)
			{
				switch(Config->EmulationConfig->Attributes[TEXTATTR_HIGHLIGHT])
				{
					case TEXTATTR_UNDERLINE:

						Value |= ATTR_UNDERLINE;
						break;

					case TEXTATTR_HIGHLIGHT:

						Value |= ATTR_HIGHLIGHT;
						break;

					case TEXTATTR_BLINK:

						Value |= ATTR_BLINK;
						break;

					case TEXTATTR_INVERSE:

						Value |= ATTR_INVERSE;
						break;
				}
			}

			if(i & ATTR_BLINK)
			{
				switch(Config->EmulationConfig->Attributes[TEXTATTR_BLINK])
				{
					case TEXTATTR_UNDERLINE:

						Value |= ATTR_UNDERLINE;
						break;

					case TEXTATTR_HIGHLIGHT:

						Value |= ATTR_HIGHLIGHT;
						break;

					case TEXTATTR_BLINK:

						Value |= ATTR_BLINK;
						break;

					case TEXTATTR_INVERSE:

						Value |= ATTR_INVERSE;
						break;
				}
			}

			if(i & ATTR_INVERSE)
			{
				switch(Config->EmulationConfig->Attributes[TEXTATTR_INVERSE])
				{
					case TEXTATTR_UNDERLINE:

						Value |= ATTR_UNDERLINE;
						break;

					case TEXTATTR_HIGHLIGHT:

						Value |= ATTR_HIGHLIGHT;
						break;

					case TEXTATTR_BLINK:

						Value |= ATTR_BLINK;
						break;

					case TEXTATTR_INVERSE:

						Value |= ATTR_INVERSE;
						break;
				}
			}

			TextAttributeTable[i] = Value;
		}
	}

		/* Determine window inner dimensions and top/left edge offsets. */

	UpdateTerminalLimits();

		/* Set up scaling data (bitmaps & rastports). */

	if(!CreateScale(Window))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_FONT_SCALING_INFO_TXT));

	if(!CreateOffsetTables())
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_OFFSET_TABLES_TXT));

	TabStopMax = Window->WScreen->Width / TextFontWidth;

		/* Allocate the tab stop line. */

	if(!(TabStops = (BYTE *)AllocVecPooled(TabStopMax,MEMF_ANY | MEMF_CLEAR)))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

		/* Reset the Window job handler to the default routine. */

	WindowJob = SingleWindowJob;

		/* Push it on the window stack (should become bottommost
		 * entry).
		 */

	PushWindow(Window);

	if(TermPort)
		TermPort->TopWindow = Window;

	if(Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && Config->ScreenConfig->SplitStatus)
	{
		if(!(StatusWindow = OpenWindowTags(NULL,
			WA_Top,				Window->TopEdge + Window->Height,
			WA_Left,			Window->LeftEdge,
			WA_InnerWidth,		StatusWidth,
			WA_InnerHeight,		StatusHeight,
			WA_DragBar,			TRUE,
			WA_DepthGadget,		TRUE,
			WA_NewLookMenus,	TRUE,
			WA_Title,			ScreenTitle,
			OpenWindowTag,		Window->WScreen,
			WA_RMBTrap,			TRUE,
			WA_CloseGadget,		TRUE,
			WA_MenuHelp,		TRUE,
			WA_NoCareRefresh,	TRUE,

			AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
			CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,
		TAG_DONE)))
			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_STATUS_WINDOW_TXT));
	}
	else
		StatusWindow = NULL;

	if(StatusWindow)
	{
		StatusWindow->UserPort = Window->UserPort;

		if(!ModifyIDCMP(StatusWindow,DEFAULT_IDCMP))
		{
			StatusWindow->UserPort = NULL;

			CloseWindow(StatusWindow);
			StatusWindow = NULL;

			return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_STATUS_WINDOW_TXT));
		}
	}

	UpdateTerminalLimits();

	RPort = Window->RPort;

		/* Default console setup. */

	CursorX = 0;
	CursorY	= 0;

	SetDrMd(RPort,JAM2);

		/* Set the font. */

	SetFont(RPort,CurrentFont);

		/* Redirect AmigaDOS requesters. */

	ChangeWindowPtr(&OldWindowPtr,(APTR)Window);

		/* Create the character raster. */

	if(!CreateRaster())
		return(LocaleString(MSG_TERMINIT_UNABLE_TO_CREATE_SCREEN_RASTER_TXT));

	ConOutputUpdate();

	ConFontScaleUpdate();

		/* Set up the scrolling info. */

	ScrollLineCount = Window->WScreen->Height / TextFontHeight;

	if(!(ScrollLines = (struct ScrollLineInfo *)AllocVecPooled(sizeof(struct ScrollLineInfo) * ScrollLineCount,MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_SCROLLING_SUPPORT_INFO_TXT));

		/* Create the menu strip. */

	if(!AttachMenu(NULL))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_MENUS_TXT));

		/* Disable the `Execute ARexx Command' menu item if
		 * the rexx server is not available.
		 */

	if(!RexxSysBase)
		OffItem(MEN_EXECUTE_REXX_COMMAND);

	if(Recording)
	{
		OnItem(MEN_RECORD_LINE);

		CheckItem(MEN_RECORD,TRUE);
		CheckItem(MEN_RECORD_LINE,RecordingLine);
	}
	else
	{
		OffItem(MEN_RECORD_LINE);

		CheckItem(MEN_RECORD,FALSE);
		CheckItem(MEN_RECORD_LINE,FALSE);
	}

	CheckItem(MEN_DISABLE_TRAPS,!(WatchTraps && !IsListEmpty((struct List *)GenericListTable[GLIST_TRAP])));

	if(StatusWindow)
	{
		SetMenuStrip(StatusWindow,Menu);

		StatusWindow->Flags &= ~WFLG_RMBTRAP;

		SetDrMd(StatusWindow->RPort,JAM2);
	}

		/* Add a tick if file capture is active. */

	if(RawCapture)
	{
		if(FileCapture)
			CheckItem(MEN_CAPTURE_TO_RAW_FILE,TRUE);
		else
			CheckItem(MEN_CAPTURE_TO_RAW_FILE,FALSE);

		CheckItem(MEN_CAPTURE_TO_FILE,FALSE);
	}
	else
	{
		if(FileCapture)
			CheckItem(MEN_CAPTURE_TO_FILE,TRUE);
		else
			CheckItem(MEN_CAPTURE_TO_FILE,FALSE);
	}

		/* Add a tick if printer capture is active. */

	CheckItem(MEN_CAPTURE_TO_PRINTER,PrinterCapture != BNULL);

		/* Add a tick if the buffer is frozen. */

	CheckItem(MEN_FREEZE_BUFFER,BufferFrozen);

		/* Disable the dialing functions if online. */

	if(Online)
		SetDialMenu(FALSE);
	else
		SetDialMenu(TRUE);

		/* Take care of the chat line. */

	if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0])
		OffItem(MEN_CHAT_LINE);
	else
		CheckItem(MEN_CHAT_LINE,ChatMode);

		/* Update the clipboard menus. */

	SetClipMenu(FALSE);

	SetTransferMenu(TRUE);

		/* Disable the `Print Screen' and `Save ASCII' functions
		 * if raster is not enabled.
		 */

	SetRasterMenu(RasterEnabled);

		/* Take care of the remaining windows */

	if(MatrixWindow)
		CheckItem(MEN_MATRIX_WINDOW,TRUE);

	if(InfoWindow)
		CheckItem(MEN_STATUS_WINDOW,TRUE);

	if(PacketWindow)
		CheckItem(MEN_PACKET_WINDOW,TRUE);

	if(ChatMode)
		CheckItem(MEN_CHAT_LINE,TRUE);

	if(FastWindow)
		CheckItem(MEN_FAST_MACROS_WINDOW,TRUE);

		/* Enable the menu. */

	Window->Flags &= ~WFLG_RMBTRAP;

	strcpy(EmulationName,LocaleString(MSG_TERMXEM_NO_EMULATION_TXT));

		/* Create the status server. */

	Forbid();

	if(StatusProcess = CreateNewProcTags(
		NP_Entry,		StatusServer,
		NP_Name,		"term Status Process",
		NP_WindowPtr,	-1,
		NP_Priority,	5,
	TAG_DONE))
		WaitForHandshake();

	Permit();

	if(!StatusProcess)
		return(LocaleString(MSG_TERMINIT_UNABLE_TO_CREATE_STATUS_TASK_TXT));

		/* Obtain the default public screen name just in case
		 * we'll need it later.
		 */

	GetDefaultPubScreen(DefaultPubScreenName);

		/* Set up the window size. */

	ScreenSizeStuff();

		/* Select the default console data processing routine. */

	ConProcessUpdate();

		/* Handle the remaining terminal setup. */

	if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Config->TerminalConfig->EmulationFileName[0])
	{
		if(!OpenEmulator(Config->TerminalConfig->EmulationFileName))
		{
			Config->TerminalConfig->EmulationMode = EMULATION_ANSIVT100;

			ResetDisplay = TRUE;
			ActivateJob(MainJobQueue,ResetDisplayJob);

			RasterEnabled = TRUE;

			ShowRequest(Window,LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_EMULATION_LIBRARY_TXT),Config->TerminalConfig->EmulationFileName);
		}
		else
		{
			if(RasterEnabled)
				RasterEnabled = FALSE;

			SetRasterMenu(RasterEnabled);
		}
	}

		/* Choose the right console data processing routine. */

	ConProcessUpdate();

		/* Reset terminal emulation. */

	if(Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL)
	{
		ClearCursor();

		ForegroundPen = GetPenIndex(SafeTextPen);
		BackgroundPen = 0;

		UpdatePens();

		Reset();

		DrawCursor();
	}
	else
	{
		if(XEmulatorBase)
			XEmulatorResetConsole(XEM_IO);
	}

		/* Restart the fast! macro panel. */

	if(HadFastMacros || Config->MiscConfig->OpenFastMacroPanel)
		OpenFastWindow();

	if(Config->TerminalConfig->UseTerminalTask && Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL)
		CreateEmulationProcess();
	else
		DeleteEmulationProcess();

	TTYResize();

	UpdateJob(MainJobQueue,WindowJob,SIG_WINDOW);

	return(NULL);
}

	/* CloseAll():
	 *
	 *	Free all resources and leave the program.
	 */

VOID
CloseAll()
{
	RemoveRendezvousSemaphore();

	DispatchRexxDialMsgList(FALSE);

	DeleteRecord();

	DeleteQueueProcess();

	SoundExit();

	SZ_SizeCleanup();

	if(IntuitionBase && Window)
		BlockWindows();

	if(RexxPort)
		RemPort(RexxPort);

	DeleteChatGadget();

	if(TermRexxPort)
	{
		if(RexxSysBase)
		{
			struct Message *Msg;

			while(Msg = GetMsg(TermRexxPort))
				ReplyMsg(Msg);
		}

		DeleteMsgPort(TermRexxPort);
	}

	ShakeHands((struct Task *)RexxProcess,SIG_KILL);

	CloseLibrary((struct Library *)RexxSysBase);

	if(XprIO && XProtocolBase)
		XProtocolCleanup(XprIO);

	CloseLibrary(XProtocolBase);

	FreeVec(XprIO);

	TerminateBuffer();

	DeleteSpeech();

	Forbid();

	BufferClosed = TRUE;

	DeleteBuffer();

	Permit();

	if(FileCapture)
	{
		BufferClose(FileCapture);

		if(!GetFileSize(CaptureName))
			DeleteFile(CaptureName);
		else
		{
			AddProtection(CaptureName,FIBF_EXECUTE);

			if(Config->MiscConfig->CreateIcons)
				AddIcon(CaptureName,FILETYPE_TEXT,FALSE);
		}
	}

	if(PrinterCapture)
		Close(PrinterCapture);

		/* Close the external emulator. */

	CloseEmulator(TRUE);

	if(IntuitionBase && GfxBase)
		DeleteDisplay();

	CaptureParserExit();

	UnLoadSeg(KeySegment);

	StopCall(TRUE);

	DeleteAccountant();

	FreeSignal(CheckBit);

	ClearSerial();

	DeleteSerial();

	StopTime();

	if(TimeRequest)
	{
		if(TimeRequest->tr_node.io_Device)
			CloseDevice((struct IORequest *)TimeRequest);

		DeleteIORequest((struct IORequest *)TimeRequest);
	}

	DeleteMsgPort(TimePort);

	ShutdownCx();

	if(TermPort)
	{
		if(TermID != -1)
		{
			ObtainSemaphore(&TermPort->OpenSemaphore);

			TermPort->OpenCount--;

			if(TermPort->OpenCount <= 0 && !TermPort->HoldIt)
			{
				RemPort(&TermPort->MsgPort);

				ReleaseSemaphore(&TermPort->OpenSemaphore);

				FreeVec(TermPort);
			}
			else
				ReleaseSemaphore(&TermPort->OpenSemaphore);
		}
	}

	FreeSignal(OwnDevBit);
	CloseLibrary(OwnDevUnitBase);

	CloseClip();

	LocaleClose();

	DeleteMsgQueue(SpecialQueue);

	if(ConsoleDevice)
		CloseDevice((struct IORequest *)ConsoleRequest);

	CloseLibrary(GTLayoutBase);

	CloseLibrary(IconBase);

	CloseLibrary(DataTypesBase);

	CloseLibrary(WorkbenchBase);

	CloseLibrary(CxBase);

	CloseLibrary(IFFParseBase);

	CloseLibrary(AslBase);

	CloseLibrary(GadToolsBase);

	CloseLibrary(LayersBase);

	CloseLibrary((struct Library *)GfxBase);

	CloseLibrary((struct Library *)IntuitionBase);

	MemoryCleanup();
}

	/* OpenAll():
	 *
	 *	Open all required resources or return an error message
	 *	if anything went wrong.
	 */

CONST_STRPTR
OpenAll(CONST_STRPTR ConfigPath)
{
	UBYTE PathBuffer[MAX_FILENAME_LENGTH];
	CONST_STRPTR Result,ConfigFileName;
	UBYTE LocalBuffer[256];
	LONG ErrorCode,i;
	BOOL ZapPhonebook;

	ConfigFileName = NULL;

	Kick30 = (SysBase->LibNode.lib_Version >= 39);

		/* These are here in order to avoid trouble later */

	ConTransferUpdate();
	SerWriteUpdate();

		/* Remember the start of this session. */

	DateStamp(&SessionStart);

		/* Set up the hooks in Data.c */

	InitHook(&LocaleHook,(HOOKFUNC)LocaleHookFunc,NULL);
	InitHook(&GuideHook,(HOOKFUNC)GuideSetupHook,NULL);
	InitHook(&BackfillHook,(HOOKFUNC)BackfillRoutine,NULL);

		/* Set up font DPI tag list. */

	TagDPI[0].ti_Tag = TAG_DONE;

		/* Initialize all the semaphores */

	for(i = 0 ; StartupSemaphoreTable[i] != NULL ; i++)
		InitSemaphore(StartupSemaphoreTable[i]);

		/* Initialize all the lists */

	for(i = 0 ; StartupListTable[i] != NULL ; i++)
		NewList(StartupListTable[i]);

	if(!MemorySetup())
		return("Cannot create memory pool");

		/* Rendezvous setup. */

	RendezvousSetup();

		/* Set up the program launcher. */

	LaunchSetup();

		/* Open the translation tables. */

	LocaleOpen("term.catalog","english",20);

		/* Fill in the menu configuration. */

	PrepareLocalizedMenu(&TermMenu,&NumMenuEntries);

		/* Open intuition.library, any version. */

	if(!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_INTUITION_LIBRARY_TXT));

		/* Check if we should use the old style sliders. */

	if(GetVar("termoldsliders",PathBuffer,sizeof(PathBuffer),0) >= 0)
		SliderType = SLIDER_KIND;
	else
		SliderType = LEVEL_KIND;

		/* Open some more libraries. */

	if(!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_GRAPHICS_LIBRARY_TXT));

	if(!(LayersBase = OpenLibrary("layers.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_LAYERS_LIBRARY_TXT));

		/* Install the correct routines to query
		 * the rendering colours and drawing mode.
		 */

	if(!Kick30)
	{
		ReadAPen = OldGetAPen;
		ReadBPen = OldGetBPen;
		ReadDrMd = OldGetDrMd;
		SetMask = OldSetWrMsk;
		GetMask = OldGetWrMsk;
	}
	else
	{
		ReadAPen = NewGetAPen;
		ReadBPen = NewGetBPen;
		ReadDrMd = NewGetDrMd;
		SetMask = NewSetWrMsk;
		GetMask = NewGetWrMsk;
	}

		/* Check if locale.library has already installed the operating system
		 * patches required for localization.
		 */

	LanguageCheck();

		/* Open the remaining libraries. */

	if(!(GadToolsBase = OpenLibrary("gadtools.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_GADTOOLS_LIBRARY_TXT));

	if(!(AslBase = OpenLibrary("asl.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_ASL_LIBRARY_TXT));

	if(!(IFFParseBase = OpenLibrary("iffparse.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_IFFPARSE_LIBRARY_TXT));

	if(!(CxBase = OpenLibrary("commodities.library",0)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_COMMODITIES_LIBRARY_TXT));

	if(!(TimePort = (struct MsgPort *)CreateMsgPort()))
		return(LocaleString(MSG_GLOBAL_FAILED_TO_CREATE_MSGPORT_TXT));

	if(!(TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(struct timerequest))))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_IOREQUEST_TXT));

	if(OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *)TimeRequest,0))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_TIMER_DEVICE_TXT));

	TimerBase = TimeRequest->tr_node.io_Device;

		/* User interface. */

	if(!(GTLayoutBase = SafeOpenLibrary("PROGDIR:gtlayout.library",36)))
	{
		if(!(GTLayoutBase = SafeOpenLibrary("gtlayout.library",36)))
		{
			STATIC UBYTE LocalBuffer[256];

			CONST_STRPTR String;
			BOOL GotIt;
			LONG i;

			String = LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_GTLAYOUT_LIBRARY_TXT);

			GotIt = FALSE;

			for(i = 0 ; i < strlen(String) ; i++)
			{
				if(String[i] == '%')
				{
					GotIt = TRUE;
					break;
				}
			}

			if(GotIt)
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,String,36);
			else
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%s (V%ld)",String,36);

			return(LocalBuffer);
		}
	}

	if(GetVar("termoldcycle",PathBuffer,sizeof(PathBuffer),0) >= 0 || GetVar("RussLeBar",PathBuffer,sizeof(PathBuffer),0) >= 0)
	{
		LONG Value;

		CycleType = CYCLE_KIND;

		if(StrToLong(PathBuffer,&Value) > 0)
		{
			if(!Value)
				CycleType = POPUP_KIND;
		}
	}
	else
		CycleType = POPUP_KIND;

#ifdef BUILTIN_ZMODEM
	if(GetVar("terminternalzmodem",PathBuffer,sizeof(PathBuffer),NULL) >= 0)
	{
		LONG Value;

		UseInternalZModem = TRUE;

		if(StrToLong(PathBuffer,&Value) > 0)
		{
			if(!Value)
				UseInternalZModem = FALSE;
		}
	}
	else
		UseInternalZModem = FALSE;
#endif

		/* Open OwnDevUnit.library, and allocate a signal bit if it opens. */

	if(OwnDevUnitBase = OpenLibrary(ODU_NAME,0))
	{
			/* If we can't get the signal, ignore the library */

		if((OwnDevBit = AllocSignal(-1)) == -1)
		{
			CloseLibrary(OwnDevUnitBase);
			OwnDevUnitBase = NULL;
		}
	}

		/* Open workbench.library, don't complain if it fails. */

	WorkbenchBase = OpenLibrary("workbench.library",0);

		/* Open icon.library as well, don't complain if it fails either. */

	IconBase = OpenLibrary("icon.library",0);

		/* Try to open datatypes.library, just for the fun of it. */

	DataTypesBase = OpenLibrary("datatypes.library",39);

	if(!(ConsoleRequest = (struct IOStdReq *)AllocVecPooled(sizeof(struct IOStdReq),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_CONSOLE_REQUEST_TXT));

	ConsoleRequest->io_Message.mn_Length = sizeof(struct IOStdReq);

	if(OpenDevice("console.device",CONU_LIBRARY,(struct IORequest *)ConsoleRequest,0))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_OPEN_CONSOLE_DEVICE_TXT));

	ConsoleDevice = ConsoleRequest->io_Device;

	if(!(FakeInputEvent = (struct InputEvent *)AllocVecPooled(sizeof(struct InputEvent),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_INPUTEVENT_TXT));

	FakeInputEvent->ie_Class = IECLASS_RAWKEY;

	if(!(MacroKeys = (struct MacroKeys *)AllocVecPooled(sizeof(struct MacroKeys),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_MACROKEYS_TXT));

	if(!(CursorKeys = (struct CursorKeys *)AllocVecPooled(sizeof(struct CursorKeys),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_CURSORKEYS_TXT));

		/* Create the job queue. */

	if(!(MainJobQueue = CreateJobQueue()))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

		/* Create the job nodes. */

	for(i = 0 ; JobInitTable[i].Name ; i++)
	{
		if(!(*JobInitTable[i].Node = CreateJob(JobInitTable[i].Name,JobInitTable[i].Type,JobInitTable[i].Function,0)))
			return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));
	}

		/* Create all generic lists. */

	for(i = GLIST_UPLOAD ; i < GLIST_COUNT ; i++)
	{
		if(!(GenericListTable[i] = CreateGenericList()))
			return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));
	}

	GenericListTable[GLIST_UPLOAD]->Notify = NotifyUploadQueue;

		/* Add extra assignments. */

	AddExtraAssignment("PROGDIR:Fonts","Fonts");
	AddExtraAssignment("Fonts","Fonts");
	AddExtraAssignment("PROGDIR:Libs","Libs");
	AddExtraAssignment("Libs","Libs");

		/* Set up the attention buffers. */

	if(!(AttentionBuffers[0] = (STRPTR)AllocVecPooled(SCAN_COUNT * ATTENTION_BUFFER_SIZE,MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_SEQUENCE_ATTENTION_INFO_TXT));

	for(i = 1 ; i < SCAN_COUNT ; i++)
		AttentionBuffers[i] = &AttentionBuffers[i - 1][ATTENTION_BUFFER_SIZE];

		/* Obtain the default environment storage
		 * path.
		 */

	if(!ConfigPath)
	{
		ConfigPath = PathBuffer;

		if(!GetEnvDOS("TERMCONFIGPATH",PathBuffer,sizeof(PathBuffer)))
		{
			if(!GetEnvDOS("TERMPATH",PathBuffer,sizeof(PathBuffer)))
			{
				APTR LastPtr;
				BPTR FileLock;

				strcpy(PathBuffer,"TERM:config");

				DisableDOSRequesters(&LastPtr);

				if(FileLock = Lock("TERM:",ACCESS_READ))
					UnLock(FileLock);
				else
				{
					FileLock = DupLock(ThisProcess->pr_HomeDir);

						/* Create TERM: assignment referring to
						 * the directory `term' was loaded from.
						 */

					if(!AssignLock("TERM",FileLock))
						UnLock(FileLock);
				}

				if(!(FileLock = Lock(PathBuffer,ACCESS_READ)))
					FileLock = CreateDir(PathBuffer);

				if(FileLock)
					UnLock(FileLock);

				EnableDOSRequesters(LastPtr);
			}
		}
	}
	else
	{
		if(GetFileSize(ConfigPath))
		{
			STRPTR Index;

			strcpy(PathBuffer,ConfigPath);

			Index = PathPart(PathBuffer);

			*Index = 0;

			ConfigFileName = ConfigPath;

			ConfigPath = PathBuffer;
		}
	}

		/* Check for proper assignment path if necessary. */

	if(!Strnicmp(ConfigPath,"TERM:",5))
	{
		APTR OldPtr;
		BPTR DirLock;

			/* Block dos requesters. */

		DisableDOSRequesters(&OldPtr);

			/* Try to get a lock on `TERM:' assignment. */

		if(DirLock = Lock("TERM:",ACCESS_READ))
			UnLock(DirLock);
		else
		{
				/* Clone current directory lock. */

			DirLock = DupLock(ThisProcess->pr_CurrentDir);

				/* Create TERM: assignment referring to
				 * current directory.
				 */

			if(!AssignLock("TERM",DirLock))
				UnLock(DirLock);
		}

		EnableDOSRequesters(OldPtr);
	}

		/* Create proper path names. */

	if(ConfigFileName)
	{
		if(!GetFileSize(ConfigFileName))
			ConfigFileName = NULL;
	}

	if(!ConfigFileName)
	{
		strcpy(LastConfig,ConfigPath);

		AddPart(LastConfig,"term.prefs",MAX_FILENAME_LENGTH);
	}
	else
		strcpy(LastConfig,ConfigFileName);

	strcpy(DefaultPubScreenName,"Workbench");

		/* Special measure */

	if(!LastPhone[0])
		ZapPhonebook = TRUE;
	else
		ZapPhonebook = FALSE;

		/* Create both configuration buffers. */

	if(!(Config = CreateConfiguration(TRUE)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_PRIMARY_CONFIG_TXT));

	if(!(PrivateConfig = CreateConfiguration(TRUE)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_SECONDARY_CONFIG_TXT));

	ResetConfig(Config,ConfigPath);

	if(ZapPhonebook)
		LastPhone[0] = 0;

		/* Read some more environment variables. */

	GetEnvDOS("EDITOR",Config->PathConfig->Editor,sizeof(Config->PathConfig->Editor));

		/* Look for the default configuration file. */

	if(!ReadConfig(LastConfig,Config))
	{
		ResetConfig(Config,ConfigPath);

		Initializing = TRUE;

		LoadColours = TRUE;

			/* Now we can safely assume that this is the */
			/* first invocation of this program on the */
			/* current setup */

		FirstInvocation = TRUE;
	}
	else
	{
		Current2DefaultPalette(Config);

		if(Config->ScreenConfig->ColourMode == COLOUR_AMIGA)
			Initializing = FALSE;
		else
			Initializing = TRUE;
	}

	if(UseNewDevice)
		strcpy(Config->SerialConfig->SerialDevice,NewDevice);

	if(UseNewUnit)
		Config->SerialConfig->UnitNumber = NewUnit;

	if(Config->MiscConfig->OpenFastMacroPanel)
		HadFastMacros = TRUE;

		/* Set up the phonebook file name */

	if(!LastPhone[0])
	{
		if(Config->PhonebookFileName[0])
			strcpy(LastPhone,Config->PhonebookFileName);
		else
		{
			strcpy(LastPhone,	Config->PathConfig->DefaultStorage);
			AddPart(LastPhone,	"phonebook.prefs",MAX_FILENAME_LENGTH);
		}
	}

		/* Load the keyboard macros. */

	strcpy(LastMacros,	Config->PathConfig->DefaultStorage);
	AddPart(LastMacros,	"functionkeys.prefs",MAX_FILENAME_LENGTH);

	if(Config->MacroFileName[0])
	{
		if(LoadMacros(Config->MacroFileName,MacroKeys))
			strcpy(LastMacros,Config->MacroFileName);
		else
		{
			if(!LoadMacros(LastMacros,MacroKeys))
				ResetMacroKeys(MacroKeys);
		}
	}
	else
	{
		if(!LoadMacros(LastMacros,MacroKeys))
			ResetMacroKeys(MacroKeys);
	}

		/* Load the cursor keys. */

	strcpy(LastCursorKeys,	Config->PathConfig->DefaultStorage);
	AddPart(LastCursorKeys,	"cursorkeys.prefs",MAX_FILENAME_LENGTH);

	if(Config->CursorFileName[0])
	{
		if(ReadIFFData(Config->CursorFileName,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
			strcpy(LastCursorKeys,Config->CursorFileName);
		else
		{
			if(!ReadIFFData(LastCursorKeys,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
				ResetCursorKeys(CursorKeys);
		}
	}
	else
	{
		if(!ReadIFFData(LastCursorKeys,CursorKeys,sizeof(struct CursorKeys),ID_KEYS))
			ResetCursorKeys(CursorKeys);
	}

		/* Load the sound settings. */

	strcpy(LastSound,	Config->PathConfig->DefaultStorage);
	AddPart(LastSound,	"sound.prefs",MAX_FILENAME_LENGTH);

	if(Config->SoundFileName[0])
	{
		if(ReadIFFData(Config->SoundFileName,&SoundConfig,sizeof(struct SoundConfig),ID_SOUN))
			strcpy(LastSound,Config->SoundFileName);
		else
		{
			if(!ReadIFFData(LastSound,&SoundConfig,sizeof(struct SoundConfig),ID_SOUN))
				ResetSound(&SoundConfig);
		}
	}
	else
	{
		if(!ReadIFFData(LastSound,&SoundConfig,sizeof(struct SoundConfig),ID_SOUN))
			ResetSound(&SoundConfig);
	}

		/* Initialize the sound support routines. */

	SoundInit();

		/* Load the phone number pattern / rates settings. */

	strcpy(LastPattern,	Config->PathConfig->DefaultStorage);
	AddPart(LastPattern,	"rates.prefs",MAX_FILENAME_LENGTH);

	if(Config->AreaCodeFileName[0])
	{
		if(PatternList = LoadTimeDateList(Config->AreaCodeFileName,&ErrorCode))
			strcpy(LastPattern,Config->AreaCodeFileName);
		else
			PatternList = LoadTimeDateList(LastPattern,&ErrorCode);
	}
	else
		PatternList = LoadTimeDateList(LastPattern,&ErrorCode);

	if(!(GlobalPhoneHandle = CreatePhonebook(0,FALSE)))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

	if(!PatternList)
	{
		if(!(PatternList = CreateList()))
			return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));
	}

		/* Load the translation tables. */

	strcpy(LastTranslation,		Config->PathConfig->DefaultStorage);
	AddPart(LastTranslation,	"translation.prefs",MAX_FILENAME_LENGTH);

	if(Config->TranslationFileName[0])
	{
		if(LoadDefaultTranslationTables(Config->TranslationFileName))
			strcpy(LastTranslation,Config->TranslationFileName);
		else
			LoadDefaultTranslationTables(LastTranslation);
	}
	else
		LoadDefaultTranslationTables(LastTranslation);

		/* Load the fast! macro settings. */

	strcpy(LastFastMacros,		Config->PathConfig->DefaultStorage);
	AddPart(LastFastMacros,		"fastmacros.prefs",MAX_FILENAME_LENGTH);

	if(Config->FastMacroFileName[0])
	{
		if(LoadFastMacros(Config->FastMacroFileName,&FastMacroList))
			strcpy(LastFastMacros,Config->FastMacroFileName);
		else
			LoadFastMacros(LastFastMacros,&FastMacroList);
	}
	else
		LoadFastMacros(LastFastMacros,&FastMacroList);

	FastMacroCount = GetListSize(&FastMacroList);

		/* Load the speech settings. */

	strcpy(LastSpeech,	Config->PathConfig->DefaultStorage);
	AddPart(LastSpeech,	"speech.prefs",MAX_FILENAME_LENGTH);

	if(Config->SpeechFileName[0])
	{
		if(ReadIFFData(Config->SpeechFileName,&SpeechConfig,sizeof(struct SpeechConfig),ID_SPEK))
			strcpy(LastSpeech,Config->SpeechFileName);
		else
		{
			if(!ReadIFFData(LastSpeech,&SpeechConfig,sizeof(struct SpeechConfig),ID_SPEK))
				ResetSpeechConfig(&SpeechConfig);
		}
	}
	else
	{
		if(!ReadIFFData(LastSpeech,&SpeechConfig,sizeof(struct SpeechConfig),ID_SPEK))
			ResetSpeechConfig(&SpeechConfig);
	}

		/* Load the hotkey settings. */

	strcpy(LastKeys,	Config->PathConfig->DefaultStorage);
	AddPart(LastKeys,	"hotkeys.prefs",MAX_FILENAME_LENGTH);

	if(Config->HotkeyFileName[0])
	{
		if(LoadHotkeys(Config->HotkeyFileName,&Hotkeys))
			strcpy(LastKeys,Config->HotkeyFileName);
		else
		{
			if(!LoadHotkeys(LastKeys,&Hotkeys))
				ResetHotkeys(&Hotkeys);
		}
	}
	else
	{
		if(!LoadHotkeys(LastKeys,&Hotkeys))
			ResetHotkeys(&Hotkeys);
	}

		/* Load the trap settings. */

	strcpy(LastTraps,	Config->PathConfig->DefaultStorage);
	AddPart(LastTraps,	"trap.prefs",MAX_FILENAME_LENGTH);

	WatchTraps = TRUE;

	if(Config->TrapFileName[0])
	{
		if(LoadTraps(Config->TrapFileName,GenericListTable[GLIST_TRAP]))
			strcpy(LastTraps,Config->TrapFileName);
		else
		{
			if(!LoadTraps(LastTraps,GenericListTable[GLIST_TRAP]))
				WatchTraps = FALSE;
		}
	}
	else
	{
		if(!LoadTraps(LastTraps,GenericListTable[GLIST_TRAP]))
			WatchTraps = FALSE;
	}

		/* Set up the text pacing controls. */

	SendSetup();

		/* Set up the capture parser. */

	if(!(ParserStuff = CreateParseContext()))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

	if(!CaptureParserInit())
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

		/* Are we to freeze the text buffer? */

	if(!Config->CaptureConfig->BufferEnabled)
		BufferFrozen = TRUE;

	ConOutputUpdate();

		/* Initialize the data flow parser. */

	ResetDataFlowFilter();

		/* Set up parsing jump tables. */

	if(!(SpecialTable = (JUMP *)AllocVecPooled(256 * sizeof(JUMP),MEMF_CLEAR | MEMF_ANY)))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

	for(i = 0 ; i < sizeof(SpecialKeys) / sizeof(struct SpecialKey) ; i++)
		SpecialTable[SpecialKeys[i].Key] = (JUMP)SpecialKeys[i].Routine;

	if(!(AbortTable = (JUMP *)AllocVecPooled(256 * sizeof(JUMP),MEMF_ANY)))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

	for(i = 0 ; i < 256 ; i++)
	{
		switch(AbortMap[i])
		{
			case 0:

				AbortTable[i] = (JUMP)ParseCode;
				break;

			case 1:

				AbortTable[i] = (JUMP)DoCancel;
				break;

			case 2:

				AbortTable[i] = (JUMP)DoNewEsc;
				break;

			case 3:

				AbortTable[i] = (JUMP)DoNewCsi;
				break;
		}
	}

	if(!(TrapStuff = CreateParseContext()))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

		/* Create the special event queue. */

	if(!(SpecialQueue = CreateMsgQueue(0,0)))
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));

		/* Set up the serial driver. */

	if(!CreateSerial(LocalBuffer,sizeof(LocalBuffer)))
	{
		ShowRequest(NULL,LocaleString(MSG_GLOBAL_TERM_HAS_A_PROBLEM_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LocalBuffer);

		DeleteSerial();
	}

		/* Get a signal bit. */

	if((CheckBit = AllocSignal(-1)) == -1)
		return(LocaleString(MSG_TERMINIT_FAILED_TO_GET_CHECK_SIGNAL_TXT));

		/* Add the global term port. */

	Forbid();

	if(!(TermPort = (struct TermPort *)FindPort(TERMPORTNAME)))
	{
		if(!(TermPort = (struct TermPort *)AllocVec(sizeof(struct TermPort) + sizeof(TERMPORTNAME),MEMF_PUBLIC|MEMF_CLEAR)))
			return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_GLOBAL_PORT_TXT));
		else
		{
			NewList(&TermPort->MsgPort.mp_MsgList);
			InitSemaphore(&TermPort->OpenSemaphore);

			TermPort->MsgPort.mp_Flags			= PA_IGNORE;
			TermPort->MsgPort.mp_Node.ln_Name	= (char *)(TermPort + 1);

			strcpy(TermPort->MsgPort.mp_Node.ln_Name,TERMPORTNAME);

			AddPort(&TermPort->MsgPort);
		}
	}

	Permit();

		/* Keep another term task from removing the port. */

	TermPort->HoldIt = TRUE;

		/* Install a new term process. */

	ObtainSemaphore(&TermPort->OpenSemaphore);

	TermPort->OpenCount++;

	TermPort->HoldIt = FALSE;

	TermID = TermPort->ID++;

	ReleaseSemaphore(&TermPort->OpenSemaphore);

		/* Set up the ID string. */

	if(TermID)
		LimitedSPrintf(sizeof(TermIDString),TermIDString,"TERM.%ld",TermID);
	else
		strcpy(TermIDString,"TERM");

	if(RexxPortName[0])
	{
		for(i = 0 ; i < strlen(RexxPortName) ; i++)
			RexxPortName[i] = ToUpper(RexxPortName[i]);

		if(FindPort(RexxPortName))
			RexxPortName[0] = 0;
	}

	if(!RexxPortName[0])
		strcpy(RexxPortName,TermIDString);

		/* Install the hotkey handler. */

	SetupCx();

		/* Allocate the first few lines for the display buffer. */

	if(!CreateBuffer())
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_VIEW_BUFFER_TXT));

	if(!(XprIO = (struct XPR_IO *)AllocVec(sizeof(struct XPR_IO),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_CREATE_PROTOCOL_BUFFER_TXT));

		/* Set up the external emulation macro data. */

	if(!(XEM_MacroKeys = (struct XEmulatorMacroKey *)AllocVecPooled((2 + 10 * 4) * sizeof(struct XEmulatorMacroKey),MEMF_ANY|MEMF_CLEAR)))
		return(LocaleString(MSG_TERMINIT_FAILED_TO_ALLOCATE_MACRO_KEY_DATA_TXT));

		/* Set up the default transfer protocol */

	if(Config->TransferConfig->DefaultType == XFER_XPR)
		strcpy(LastXprLibrary,Config->TransferConfig->DefaultLibrary);

	ProtocolSetup(FALSE);

		/* Load a keymap file if required. */

	if(Config->TerminalConfig->KeyMapFileName[0])
		KeyMap = LoadKeyMap(Config->TerminalConfig->KeyMapFileName);

	if(!(TermRexxPort = (struct MsgPort *)CreateMsgPort()))
		return(LocaleString(MSG_GLOBAL_FAILED_TO_CREATE_MSGPORT_TXT));

		/* If rexxsyslib.library opens cleanly it's time for
		 * us to create the background term Rexx server.
		 */

	if(RexxSysBase = (struct RxsLib *)OpenLibrary(RXSNAME,0))
	{
			/* Create a background process handling the
			 * rexx messages asynchronously.
			 */

#ifdef AROS_BUG_FIXED
// Blocks everything if started.
		if(!(RexxProcess = StartProcessWaitForHandshake("term Rexx Process",(TASKENTRY)RexxServer,
			NP_Priority,	5,
			NP_StackSize,	8000,
			NP_WindowPtr,	-1,
		TAG_END)))
			return(LocaleString(MSG_TERMINIT_UNABLE_TO_CREATE_AREXX_PROCESS_TXT));
#endif
	}

		/* Install the public screen name, assumes that the user
		 * wants the window to be opened on the screen, rather than
		 * opening a custom screen.
		 */

	if(SomePubScreenName[0])
	{
		strcpy(Config->ScreenConfig->PubScreenName,SomePubScreenName);

		Config->ScreenConfig->Blinking		= FALSE;
		Config->ScreenConfig->FasterLayout	= FALSE;
		Config->ScreenConfig->UseWorkbench	= TRUE;

		SomePubScreenName[0] = 0;
	}

	CreateQueueProcess();

	AddRendezvousSemaphore(RexxPortName);

		/* Start the rates accounting */

#ifdef AROS_BUG_FIXED
// Blocks everything if started
	if(!CreateAccountant())
		return(LocaleString(MSG_GLOBAL_NO_AUX_BUFFERS_TXT));
#endif

	if(DoIconify)
		return(NULL);
	else
	{
			/* Create the whole display. */

		if(Result = CreateDisplay(FALSE,!KeepQuiet))
			return(Result);
		else
		{
			PubScreenStuff();

			UpdateJob(MainJobQueue,SerialCheckJob,SIG_CHECK);
			UpdateJob(MainJobQueue,QueueJob,SIG_QUEUE);

			if(OwnDevUnitBase)
				UpdateJob(MainJobQueue,OwnDevUnitJob,SIG_OWNDEVUNIT);

			if(TermRexxPort)
				UpdateJob(MainJobQueue,RexxJob,SIG_REXX);

			return(NULL);
		}
	}
}
