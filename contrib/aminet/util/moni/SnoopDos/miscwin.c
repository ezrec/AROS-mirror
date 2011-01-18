/*
 *		MISCWIN.C											vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		This module contains miscellaneous functions associated with
 *		the SnoopDos GUI, but nevertheless not directly tied to any
 *		particular window.
 *		
 */		

#include "system.h"
#include "snoopdos.h"
#include "gui.h"

#if 0
#define DB(s)	KPrintF(s)
#else
#define DB(s)
#endif

/*
 *		Four dummy requesters for use when disabling window input
 */
struct FileRequester	*SnoopDosFR;

APTR					AG_Context;
struct NewAmigaGuide	AG_NewGuide;
char					*PendingAGuideMsg;

extern struct TextAttr	TopazFontAttr;
extern struct TextAttr	SystemFontAttr;
extern struct TextAttr	WindowFontAttr;
extern struct TextAttr	BufferFontAttr;

/*
 *		Now our busy pointer for V37 users (borrowed from ToolMaker)
 */
UWORD SECTION_CHIP WaitPointer[36] = {
	0x0000, 0x0000, 0x0400, 0x07C0, 0x0000, 0x07C0, 0x0100, 0x0380,
	0x0000, 0x07E0, 0x07C0, 0x1FF8, 0x1FF0, 0x3FEC, 0x3FF8, 0x7FDE,
	0x3FF8, 0x7FBE, 0x7FFC, 0xFF7F, 0x7EFC, 0xFFFF, 0x7FFC, 0xFFFF,
	0x3FF8, 0x7FFE, 0x3FF8, 0x7FFE, 0x1FF0, 0x3FFC, 0x07C0, 0x1FF8,
	0x0000, 0x07E0, 0x0000, 0x0000
};

ULONG SECTION_CHIP FileButtonData[] = {
	0x0FF00000, 0x0C180000, 0x0C140000, 0x0C120000, 0x0C1F0000,
	0x0C030000, 0x0C030000, 0x0C030000, 0x0C030000, 0x0FFF0000,
};
 
ULONG SECTION_CHIP FontButtonData[] = {
	0x0FE00000, 0x09200000, 0x01000000, 0x017F0000, 0x01490000,
	0x01080000, 0x01080000, 0x00080000, 0x00080000, 0x00080000,
};
 
/*
 *		This structure holds all the information we need to know about
 *		one of our custom images
 */
struct CustomBitMap {
	struct Image	image;
	struct Image	altimage;
	struct BitMap	bitmap;
	struct RastPort	rport;
};

struct CustomImage {
	struct CustomBitMap	image[2];
	int					size;
};
	
/*****************************************************************************
 *
 *		Start of functions!
 *
 *****************************************************************************/

/*
 *		DisableWindow(window, requester)
 *
 *		Disables the specified window by displaying a dummy requester in it
 *		and turning on the wait pointer. It is assumed that the window
 *		exists and has not been disabled already.
 */
void DisableWindow(struct Window *win, struct Requester *req)
{
	InitRequester(req);
	if (Request(req, win)) {
		/*
		 *		Use the new V39 system busy pointer if possible, else
		 *		drop back to our own busy pointer
		 */
		if (IntuitionBase->LibNode.lib_Version >= 39)
			SetWindowPointer(win,
							 WA_BusyPointer,	TRUE,
							 WA_PointerDelay,	TRUE,
							 TAG_DONE);
		else
			SetPointer(win, WaitPointer, 16, 16, -6, 0);
	}
}

/*
 *		EnableWindow()
 *
 *		Enables the specified window by removing the dummy requester
 *		placed inside it earlier. You must have called DisableWindow()
 *		with the same parameters first.
 */
void EnableWindow(struct Window *win, struct Requester *req)
{
	EndRequest(req, win);
	ClearPointer(win);
}

/*
 *		DisableAllWindows()
 *
 *		Disables all windows by opening a dummy requester in them and
 *		setting the window pointer to busy. Calls to this function
 *		nest, so be sure to call EnableAllWindows the correct number
 *		of times. This is intended for use when displaying modal
 *		requesters (e.g. ASL, error messages, etc.)
 */
void DisableAllWindows(void)
{
	DisableNestCount++;
	if (DisableNestCount == 1) {
		if (MainWindow) {
			/*
			 *		If we are disabling the main window, we won't be able
			 *		to respond to IDCMP_SIZEVERIFY messages, so instead, we
			 *		just stop requesting them. This lets the user resize the
			 *		window when we have a file/font requester up, even though
			 *		it won't redraw until the requester is finished.
			 */
			ModifyIDCMP(MainWindow,MainWindow->IDCMPFlags & ~IDCMP_SIZEVERIFY);
			DisableWindow(MainWindow, &MainRequester);
		}
		if (SetWindow)		DisableWindow(SetWindow,	&SetRequester);
		if (FuncWindow)		DisableWindow(FuncWindow,	&FuncRequester);
		if (FormWindow)		DisableWindow(FormWindow,	&FormRequester);
	}
}

/*
 *		EnableAllWindows()
 *
 *		Re-enables all windows disabled by calling DisableAllWindows()
 */
void EnableAllWindows(void)
{
	DisableNestCount--;
	if (DisableNestCount == 0) {
		if (SetWindow)		EnableWindow(SetWindow,		&SetRequester);
		if (FuncWindow)		EnableWindow(FuncWindow,	&FuncRequester);
		if (FormWindow)		EnableWindow(FormWindow,	&FormRequester);
		if (MainWindow) {
			/*
			 *		Because the user might have resized the main window
			 *		while we were disabled, causing us to miss the resize
			 *		message (which happens when IDCMP_SIZEVERIFY is active)
			 *		we check to see if the size has changed and if it has,
			 *		cause a fake RESIZE message to be sent to ourself.
			 */
			EnableWindow(MainWindow, &MainRequester);
			ModifyIDCMP(MainWindow, MainWindow->IDCMPFlags | IDCMP_SIZEVERIFY);
			if (MainWindow->Width  != CurWindowWidth ||
							MainWindow->Height != CurWindowHeight) {
				SizeWindow(MainWindow, 0, 0);
			}
		}
	}
}

/*
 *		RecordWindowSizes()
 *
 *		Records the current size and position of all open windows on the
 *		display so that they can be re-opened in the same position next time.
 *		Usually called before a window is closed, but may also be called
 *		when (for example) settings are being saved.
 */
void RecordWindowSizes(void)
{
	if (MainWindow) {
		CurSettings.MainWinLeft   = MainWindow->LeftEdge;
		CurSettings.MainWinTop    = MainWindow->TopEdge;
		CurSettings.MainWinWidth  = MainWindow->Width;
		CurSettings.MainWinHeight = MainWindow->Height;
	}
	if (FormWindow) {
		CurSettings.FormWinLeft   = FormWindow->LeftEdge;
		CurSettings.FormWinTop    = FormWindow->TopEdge;
	}
	if (FuncWindow) {
		CurSettings.FuncWinLeft   = FuncWindow->LeftEdge;
		CurSettings.FuncWinTop    = FuncWindow->TopEdge;
	}
	if (SetWindow) {
		CurSettings.SetupWinLeft  = SetWindow->LeftEdge;
		CurSettings.SetupWinTop   = SetWindow->TopEdge;
	}
}

/*
 *		ShowError(errormsg)
 *
 *		Displays the specified error message in a requester (on the same
 *		screen as the main SnoopDos window if possible, else on the
 *		default window), waits for an OKAY click, and returns.
 */
void ShowError(char *errormsg, ...)
{
	static struct EasyStruct es = {
		sizeof(struct EasyStruct),
		0,
		"SnoopDos",
		NULL,
		NULL
	};
	int pausestate = Paused;

	Paused = 0;

	es.es_TextFormat	= errormsg;
	es.es_GadgetFormat	= MSG(MSG_ERROR_OKAY);
	DisableAllWindows();
	/*
	 *		If MainWindow is NULL, then the requester will appear on
	 *		the default public screen
	 */
	EasyRequestArgs(MainWindow, &es, 0, (&errormsg)+1);
	EnableAllWindows();
	Paused = pausestate;
}

/*
 *		GetResponse(prompt, message, params, ...)
 *
 *		Displays an easy requester with the specified prompts, waits for the
 *		user to click on an option, then returns the selected option.
 *
 *		Note that options are numbered 1,2,3....,0 with the leftmost gadget
 *		returning 1 and the rightmost gadget returning 0.
 */
int GetResponse(char *prompts, char *reqmsg, ...)
{
	static struct EasyStruct es = {
		sizeof(struct EasyStruct),
		0,
		"SnoopDos",
		NULL,
		NULL
	};
	int pausestate = Paused;
	int choice;

	Paused = 0;

	es.es_TextFormat	= reqmsg;
	es.es_GadgetFormat	= prompts;
	DisableAllWindows();
	choice = EasyRequestArgs(MainWindow, &es, 0, (&reqmsg)+1);
	EnableAllWindows();
	Paused = pausestate;
	return (choice);
}

/*
 *		MaxTextLen(font, textarray)
 *
 *		Returns the length (in pixels) of the longest string in the given
 *		zero-terminated array of message IDs, using the given font.
 */
int MaxTextLen(struct TextFont *font, int *ids)
{
	struct RastPort rp;
	int maxlen = 0;

	InitRastPort(&rp);
	SetFont(&rp, font);

	while (*ids) {
		char *msg = MSG(*ids++);
		int  len  = TextLength(&rp, msg, strlen(msg));

		if (len > maxlen)
			maxlen = len;
	}
	return (maxlen);
}

/*
 *		GetTextLen(font, msg)
 *
 *		Returns the length (in pixels) of the given message indicated by the
 *		message ID, when rendered in the given font.
 */
int GetTextLen(struct TextFont *font, char *msg)
{
	struct RastPort rp;

	InitRastPort(&rp);
	SetFont(&rp, font);
	return (TextLength(&rp, msg, strlen(msg)));
}

/*
 *		AddKeyShortcut(shortcut, gadid, msgid)
 *
 *		Checks the string corresponding to msgid for an underlined
 *		character. If one is found, then a corresponding entry is
 *		made in the shortcut array with that gadget ID.
 *
 *		For alphabetic shortcuts, we make entries for both the lower
 *		and upper case versions of the key (since we might have
 *		strings like "_Cancel" and "U_se" and we want both kinds
 *		to be treated identically).
 */
void AddKeyShortcut(UBYTE *shortcut, int gadid, int msgid)
{
	UBYTE *p = MSG(msgid);

	while (*p) {
		if (*p++ == '_') {
			UBYTE ch = *p;

			if (ch >= 'a' && ch <= 'z')
				ch &= 0x5F;

			shortcut[ch] = gadid;
			if (ch >= 'A' && ch <= 'Z')
				shortcut[ch + 32] = gadid;
			return;
		}
	}
}

/*
 *		ShowGadget(window, gadget, type)
 *
 *		Activates or deactives the described gadget, by showing it in
 *		a GADGET_DOWN or GADGET_UP rendered state. This is mainly used
 *		to give the user direct feedback as to what's happening when
 *		they hit a keyboard shortcut.
 *
 *		Automatically ignores disabled gadgets.
 */
void ShowGadget(struct Window *win, struct Gadget *gad, int type)
{
	if ((gad->Flags & GFLG_DISABLED) == 0) {
		int gadpos = RemoveGadget(win, gad);

		if (type == GADGET_DOWN)
			gad->Flags |= GFLG_SELECTED;
		else
			gad->Flags &= ~GFLG_SELECTED;
		AddGadget(win, gad, gadpos);
		RefreshGList(gad, win, NULL, 1);
	}
}

/*
 *		MyOpenFont()
 *
 *		Tries to open the specified font. If we can't open diskfont.library,
 *		then falls back to the ROM OpenFont() in graphics.library. We do
 *		this so that we can still run, even when there is no diskfont.library
 *		around.
 *
 *		Returns a pointer to the opened font, or NULL for failure.
 */
struct TextFont *MyOpenFont(struct TextAttr *textattr)
{
	if (DiskfontBase)
		return OpenDiskFont(textattr);
	else
		return OpenFont(textattr);
}

/*
 *		CheckForASL()
 *
 *		Tries to load asl.library and displays an error requester if it
 *		can't be located. Returns TRUE if successful, or FALSE if it
 *		couldn't be loaded. All functions using asl.library functions
 *		must call this first.
 *
 *		We do it like this since it's useful to be able to run SnoopDos
 *		even when asl.library isn't around.
 */
int CheckForASL(void)
{
	if (!DiskfontBase) {
		DiskfontBase = OpenLibrary("diskfont.library", 0);
		if (!DiskfontBase) {
			ShowError(MSG(MSG_ERROR_NO_DISKFONT));
			return (FALSE);
		}
	}
	if (!AslBase) {
		AslBase = OpenLibrary("asl.library", 0);
		if (!AslBase) {
			ShowError(MSG(MSG_ERROR_NO_ASL));
			return (FALSE);
		}
	}
	return (TRUE);
}

/*
 *		SelectFont(win, fonttype)
 *
 *		Requests a font from the user of the appropriate type (either
 *		FONTSEL_WINDOW or FONTSEL_BUFFER). Returns TRUE if a font was
 *		successfully chosen, else FALSE. win is the window this
 *		requester is associated with (used to choose the correct
 *		opening screen).
 *
 *		If successful, the font requester (WindowFR or BufferFR) will
 *		hold details of the selected font.
 */
int SelectFont(struct Window *win, int fonttype)
{
	struct FontRequester **fontreq;
	struct TextAttr *fontattr;
	struct TextAttr *reqfont;
	struct TextFont *tempfont;
	int fixedwidth;
	char *title;
	int retval;
	int pausestate = Paused;

	if (!CheckForASL())
		return (FALSE);

	/*
	 *		Now a little check to make sure we can actually open
	 *		the current window font. If we can't, fall back to the
	 *		system font. (This should never arise, but if it does,
	 *		the user is in the unfortunate position of not being
	 *		able to change to a font that IS recognised, since the
	 *		font requester can't be opened!)
	 */
	reqfont = &WindowFontAttr;
	tempfont = MyOpenFont(reqfont);
	if (tempfont)
		CloseFont(tempfont);
	else
		reqfont = &SystemFontAttr;
		
	/*
	 *		Next, initialise according to the font type being selected
	 */
	if (fonttype == FONTSEL_BUFFER) {
		fontreq 	= &BufferFR;
		fontattr	= &BufferFontAttr;
		title   	= MSG(MSG_BFONT_TITLE);
		fixedwidth	= TRUE;
	} else {
		fontreq 	= &WindowFR;
		fontattr	= &WindowFontAttr;
		title   	= MSG(MSG_GFONT_TITLE);
		fixedwidth	= FALSE;
	}

	if (*fontreq == NULL) {
		*fontreq = (struct FontRequester *)
					AllocAslRequestTags(ASL_FontRequest,
							ASLFO_PrivateIDCMP,		TRUE,
							ASLFO_TitleText,		title,
							ASLFO_PositiveText,		MSG(MSG_ERROR_OKAY),
							ASLFO_NegativeText,		MSG(MSG_ERROR_CANCEL),
							ASLFO_FixedWidthOnly,	fixedwidth,
							ASLFO_InitialHeight,	SnoopScreen->Height*3/4,
							TAG_DONE);
		if (!*fontreq) {
			ShowError(MSG(MSG_ERROR_OPENFONT));
			return (FALSE);
		}
	}
	Paused = 0;
	DisableAllWindows();
	retval = AslRequestTags(*fontreq,
							ASLFO_TextAttr,			reqfont,
							ASLFO_Window,			win,
							ASLFO_InitialName,		fontattr->ta_Name,
							ASLFO_InitialSize,		fontattr->ta_YSize,
							TAG_DONE);
	EnableAllWindows();
	Paused = pausestate;

	if (retval && fonttype == FONTSEL_BUFFER) {
		/*
	 	 *		Now do an additional check to ensure we really got a
		 *		non-proportional font, since the ASL requester (as of V39)
		 *		can sometimes return a proportional font even when the
		 *		FixedWidthOnly tag is set. If we got a proportional font,
		 *		ignore the selection.
		 */
		struct TextFont *font = MyOpenFont(&BufferFR->fo_Attr);

		if (!font || (font->tf_Flags & FPF_PROPORTIONAL)) {
			ShowError(MSG(MSG_ERROR_FONT_PROPORTIONAL));
			retval = FALSE;
		}
		if (font)
			CloseFont(font);
	}
	return (retval);
}

/*
 *		SelectFile(char *newname, char *defname, win, type)
 *
 *		Requests a file from the user of the appropriate type:
 *
 *			FILESEL_LOADCONFIG		- Loading a configuration
 *			FILESEL_SAVECONFIG		- Saving a configuration
 *			FILESEL_DEFLOGNAME		- Default log filename
 *			FILESEL_NEWLOGNAME		- Current log filename
 *
 *		Returns TRUE for success, FALSE for failure.
 *
 *		newname is the buffer to store the full pathname of the selected file.
 *		defname is the default path/filename to use in the requester.
 *
 *		defname and newname may both point to the same string -- newname is
 *		only updated if a file is actually selected.
 */
int SelectFile(char *newname, char *defname, struct Window *win, int type)
{
	char pathname[100];
	char filename[50];
	struct TextAttr *reqfont;
	struct TextFont *tempfont;
	int titleid;
	int retval;
	int pausestate = Paused;
	int savemode;
	int n;

	if (!CheckForASL())
		return (FALSE);

	switch (type) {
		case FILESEL_LOADCONFIG: titleid = MSG_ASL_LOADCONFIG;	break;
		case FILESEL_SAVECONFIG: titleid = MSG_ASL_SAVECONFIG;	break;
		case FILESEL_DEFLOGNAME: titleid = MSG_ASL_DEFLOGNAME;	break;
		case FILESEL_NEWLOGNAME: titleid = MSG_ASL_NEWLOGNAME;	break;
		case FILESEL_SAVEWINDOW: titleid = MSG_ASL_SAVEWINDOW;	break;
		case FILESEL_SAVEBUFFER: titleid = MSG_ASL_SAVEBUFFER;	break;
	}
	savemode = (type != FILESEL_LOADCONFIG);

	/*
	 *		Now a little check to make sure we can actually open
	 *		the current window font. If we can't, fall back to the
	 *		system font.
	 */
	reqfont  = &WindowFontAttr;
	tempfont = MyOpenFont(reqfont);
	if (tempfont)
		CloseFont(tempfont);
	else
		reqfont = &SystemFontAttr;
		
	strcpy(filename, FilePart(defname));
	n = PathPart(defname) - (STRPTR) defname;
	if (n)
		strncpy(pathname, defname, n);
	pathname[n] = '\0';

	if (SnoopDosFR == NULL) {
		SnoopDosFR = (struct FileRequester *)
					  AllocAslRequestTags(ASL_FileRequest,
							ASLFR_PrivateIDCMP,		TRUE,
							ASLFR_PositiveText,		MSG(MSG_ERROR_OKAY),
							ASLFR_NegativeText,		MSG(MSG_ERROR_CANCEL),
							ASLFR_RejectIcons,		TRUE,
							ASLFR_InitialHeight,	SnoopScreen->Height*3/4,
							TAG_DONE);
		if (!SnoopDosFR) {
			ShowError(MSG(MSG_ERROR_OPENFILEREQ));
			return (FALSE);
		}
	}
	/*
	 *		It's important that we disable windows while the
	 *		file requester is displayed, otherwise users might
	 *		become confused
	 */
	Paused = 0;
	DisableAllWindows();
	retval = AslRequestTags(SnoopDosFR,
							ASLFR_TitleText,		MSG(titleid),
							ASLFR_Window,			win,
							ASLFR_TextAttr,			reqfont,
							ASLFR_InitialFile,		filename,
							ASLFR_InitialDrawer,	pathname,
							ASLFR_DoSaveMode,		savemode,
							TAG_DONE);
	EnableAllWindows();
	Paused = pausestate;
	/*
	 *		Now build our return filename. If no output filename was selected
	 *		but a directory was given, then we only allow a successful if
	 *		the "directory" is actually a device -- if it's a real directory
	 *		we return failure (since you can't save or load a directory).
	 *
	 *		We even return failure if a device is specified as the directory
	 *		and we're trying to load a file, since loading from a printer
	 *		makes no sense. (Mind you, loading a config file from a CON: file
	 *		might be an interesting way to provide a built-in command line
	 *		interpreter!)
	 */
	if (retval) {
		if (*SnoopDosFR->fr_File ||
			(savemode && !IsFileSystem(SnoopDosFR->fr_Drawer)))
		{
			strcpy(newname, SnoopDosFR->fr_Drawer);
			AddPart(newname, SnoopDosFR->fr_File, 255);
		} else
			retval = FALSE;
	}
	return (retval);
}

/*
 *		InitFonts()
 *
 *		Initialises our default fonts (system, topaz) and
 *		updates the current font settings to whichever font is
 *		appropriate, if they have not already been set to
 *		a particular font.
 */
void InitFonts(void)
{
	/*
	 *		We have four fonts in total; the current gadget and buffer fonts,
	 *		the system font, and finally topaz 8 point. If we fail to open
	 *		a window using either of the first two, we will try again using
	 *		the third and finally using the fourth before giving up.
	 *
	 *		We default to having the gadget font be the same as the screen
	 *		font, and having the buffer font the same as the system font.
	 */
	strcpy(SystemFontName, GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name);
	SystemFontAttr.ta_YSize = GfxBase->DefaultFont->tf_YSize;
	BufferFontAttr.ta_YSize = BufferFontSize;
	WindowFontAttr.ta_YSize = WindowFontSize;

	if (BufferFontSize == 0) {
		strcpy(BufferFontName, SystemFontName);
		BufferFontAttr.ta_YSize = SystemFontAttr.ta_YSize;
		BufferFontSize = BufferFontAttr.ta_YSize;
	}

	if (SnoopScreen && WindowFontSize == 0) {
		strcpy(WindowFontName, SnoopScreen->Font->ta_Name);
		WindowFontAttr.ta_YSize = SnoopScreen->Font->ta_YSize;
		WindowFontSize = WindowFontAttr.ta_YSize;
	}
}

/*
 *		SetupScreen()
 *
 *		Initialises the font and other variables which depend on the
 *		user's chosen screen. This is called prior to opening any
 *		windows on a screen.
 *
 *		If we can't open on the user's desired screen (maybe it doesn't
 *		exist) then we fall back to Workbench. If we can't even open on
 *		Workbench, then we're really in trouble and should probably quit.
 *
 *		We maintain the screen info until CleanupScreen() is called (usually
 *		because SnoopDos is going into a hidden state).
 *
 *		Returns TRUE for success, false for failure. SnoopScreen can also
 *		be checked at any time to see if we have a valid screen or not.
 *
 */
int SetupScreen(void)
{
	struct ColorMap *cm;
	struct Screen *scr;
	char *scrname = NULL;			/* Default is default public screen */
	struct List *psl;
	struct PubScreenNode *psn;

	/*
	 *		First, let's locate the screen we want, according to the
	 *		user's preference. If we can't open the preferred screen,
	 *		we try again with the default public screen. If we can't
	 *		open the default public screen, we try Workbench. If we
	 *		can't open THAT, then we're really screwed.
	 */
	if (SnoopScreen)
		CleanupScreen();

	switch (CurSettings.Setup.ScreenType) {
		
		case SCREEN_FRONT:
			/*
			 *		We want to open on the front public screen if possible.
			 *		The only way to find out if the current screen is a
			 *		public screen is to read IntuitionBase->FirstScreen and
			 *		then try and find a match for it on the public screen list.
			 *		If we do, then the public screen structure will give the
			 *		public name of the screen, which we can then use to
			 *		lock it.
			 */
			psl = (void *)LockPubScreenList();
			if (psl) {
				scr = IntuitionBase->FirstScreen;
				FORLIST(psl, psn) {
					if (psn->psn_Screen == scr) {
						scrname = psn->psn_Node.ln_Name;
						break;
					}
				}
				UnlockPubScreenList();
			}
			break;

		case SCREEN_NAMED:
			scrname = CurSettings.Setup.ScreenName;
			break;
	}
	scr = LockPubScreen(scrname);
	if (!scr && scrname)
		scr = LockPubScreen(NULL);

	if (!scr) {
		scr = LockPubScreen("Workbench");
		if (!scr)
			return (FALSE);
	}
	BorderLeft		= scr->WBorLeft;
	BorderRight		= scr->WBorRight;
	BorderTop       = scr->WBorTop;
	BorderBottom	= scr->WBorBottom;
	ScreenWidth		= scr->Width;
	ScreenHeight	= scr->Height;
	TitlebarHeight	= BorderTop + scr->Font->ta_YSize + 1;

	/*
	 *		Now calculate aspect ratio of screen, so we can scale the
	 *		horizontal scroll bar accordingly.
	 */
	SquareAspect  = 1;	/* Assume square aspect ratio */
	GadgetHeight  = HI_GADGET_HEIGHT;
	GadgetSpacing = HI_GADGET_SPACING;
	cm = scr->ViewPort.ColorMap;
	if (cm) {
		struct TagItem ti[] = {
			{VTAG_VIEWPORTEXTRA_GET,	NULL},
			{VTAG_END_CM,			NULL}
		};
		if (VideoControl(cm, ti) == 0) {
			struct ViewPortExtra *vpe = (struct ViewPortExtra *)ti[0].ti_Data;
			struct Rectangle *rect = &vpe->DisplayClip;

			if ((rect->MaxX - rect->MinX)/2 > (rect->MaxY - rect->MinY)) {
				SquareAspect  = 0;
				GadgetHeight  = LO_GADGET_HEIGHT;
				GadgetSpacing = LO_GADGET_SPACING;
			}
		}
	}
	SnoopScreen = scr;
	ScreenDI	= GetScreenDrawInfo(SnoopScreen);
	/*
	 *		Get the sizing image used by windows. We need this info
	 *		so that we can align our scroll arrows and scroll bars
	 *		correctly.
	 */
	ScreenResolution = (SnoopScreen->Flags & SCREENHIRES) ? SYSISIZE_MEDRES :
															SYSISIZE_LOWRES;
	SizeImage = (struct Image *)NewObject(NULL, "sysiclass",
										  SYSIA_DrawInfo,	ScreenDI,
										  SYSIA_Which,		SIZEIMAGE,
										  SYSIA_Size,		ScreenResolution,
										  TAG_END);
	if (!SizeImage) {
		CleanupScreen();
		return (NULL);
	}
	InitFonts();
	return (TRUE);
}

/*
 *		CleanupScreen()
 *
 *		Frees resources associated with the screen. Called when
 *		SnoopDos is about to go into HIDE mode.
 */
void CleanupScreen(void)
{
	if (SizeImage) {
		DisposeObject(SizeImage);
		SizeImage = NULL;
	}
	if (SnoopScreen) {
		if (ScreenDI) {
			FreeScreenDrawInfo(SnoopScreen, ScreenDI);
			ScreenDI = NULL;
		}
		UnlockPubScreen(NULL, SnoopScreen);
		SnoopScreen = NULL;
	}
}

/*
 *		CleanupWindows()
 *
 *		Frees all resources allocated in windows module
 */
void CleanupWindows(void)
{
	CleanupMainWindow();
	CleanupSubWindows();
	CleanupScreen();
	if (WindowFR)		FreeAslRequest(WindowFR),	WindowFR	= NULL;
	if (BufferFR)		FreeAslRequest(BufferFR),	BufferFR	= NULL;
	if (SnoopDosFR)		FreeAslRequest(SnoopDosFR),	SnoopDosFR	= NULL;
}

/*
 *		ShowAGuide(cmdstring)
 *
 *		Attempts to open AmigaGuide and display help on the specified
 *		topic. (The cmd string should be an actual AmigaGuide command
 *		in the form "LINK keyword".)
 *
 *		After calling this function, you should call CleanupHelp()
 *		before exiting the program, to ensure that the AmigaGuide
 *		system is safely closed down.
 *
 *		If SnoopDos is running on a separate screen to the one currently
 *		in use by AmigaGuide, AmigaGuide will be shut down and re-opened
 *		on the current screen.
 *
 *		Returns true for success, false for failure. However, it puts
 *		up its own error messages, so you can pretty much ignore the
 *		return value. (Calling CleanupAGuide() is always safe.)
 *
 *		Note: cmdstring should be a permanent string, not a temporary
 *		(at least until the next time this function is called!)
 */
int ShowAGuide(char *cmdstring)
{
	if (!AmigaGuideBase) {
		AmigaGuideBase = OpenLibrary("amigaguide.library", 34);
		if (!AmigaGuideBase) {
			ShowError(MSG(MSG_ERROR_NO_AMIGAGUIDE));
			return (FALSE);
		}
	}
	if (!CheckForScreen())
		return (FALSE);
	
	if (AG_Context && AG_NewGuide.nag_Screen != SnoopScreen) {
		CloseAmigaGuide(AG_Context);
		AG_Context      = NULL;
		AmigaGuideMask = 0;
	}
	if (!AG_Context) {
		/*
		 *		AmigaGuide wasn't already running so open it up on
		 *		our current screen
		 *
		 *		The docs say that we must ensure the screen pointer
		 *		AmigaGuide gets remains valid for the duration of
		 *		AmigaGuide use. What happens if someone closes the
		 *		main SnoopDos window causing us to remove our lock on
		 *		the public screen?
		 *
		 *		Well, since earlier, we ensured that we always force
		 *		AmigaGuide to open on our current screen, and since a
		 *		public screen can't close as long as AmigaGuide has some
		 *		windows on it, the only potential for a problem would be
		 *		if AmigaGuide tried to open a new window on its screen
		 *		on its own -- and it will never do that since we always
		 *		control it via this function.
		 */
		static char copycmd[80];

		AG_NewGuide.nag_Lock		= NULL;
		AG_NewGuide.nag_Name		= MSG(MSG_HELPFILE_NAME);
		AG_NewGuide.nag_Screen		= SnoopScreen;
		AG_NewGuide.nag_ClientPort	= HELP_AREXX_PORT;
		AG_NewGuide.nag_BaseName	= HELP_BASENAME;
		AG_NewGuide.nag_Flags		= HTF_CACHE_NODE;

		AG_Context = OpenAmigaGuideAsync(&AG_NewGuide, TAG_DONE);
		if (!AG_Context) {
			ShowError(MSG(MSG_ERROR_CREATE_AMIGAGUIDE));
			return (FALSE);
		}
		AmigaGuideMask = AmigaGuideSignal(AG_Context);
		/*
		 *		Now since AmigaGuide won't initialise immediately, it's
		 *		no use sending this help request to it -- we need to wait
		 *		until it tells us that it's ready for action. So, we
		 *		remember the command that was to be sent, and when we
		 *		get the okay from AmigaGuide, we can send it (see
		 *		HandleAGuideMsgs() below.)
		 *
		 *		Also, since sometimes the string that we are sent is only
		 *		temporary (e.g. from an ARexx message) we need to make a
		 *		copy of it.
		 */		
		strcpy(copycmd, cmdstring);
		PendingAGuideMsg = copycmd;
		return (TRUE);
	}

	/*
	 *		Usual case: just send the command directly to AmigaGuide
	 */
	SendAmigaGuideCmd(AG_Context, cmdstring, NULL);
	return (TRUE);
}

/*
 *		HandleAGuideMsgs()
 *
 *		Handles any AmigaGuide messages (as indicate by a signal
 *		arriving that matches AmigaGuideMask)
 */
void HandleAGuideMsgs(void)
{
	struct AmigaGuideMsg *agm;
	int unloadhelp = 0;

	if (!AmigaGuideBase || !AG_Context)
		return;

	while ((agm = GetAmigaGuideMsg(AG_Context)) != NULL) {
		switch (agm->agm_Type) {

			case ActiveToolID:
				/*
				 *		The first time we try and display something,
				 *		AmigaGuide has to load first. We get this
				 *		message when it's finished loading, to let
				 *		us know we can now display the help.
				 */
				if (PendingAGuideMsg) {
					SendAmigaGuideCmd(AG_Context, PendingAGuideMsg, NULL);
					PendingAGuideMsg = NULL;
				}
				break;

			case ToolCmdReplyID:
			case ToolStatusID:
			case ShutdownMsgID:
				if (agm->agm_Pri_Ret){
					if (agm->agm_Sec_Ret == HTERR_CANT_OPEN_DATABASE) {
						ShowError(MSG(MSG_ERROR_AGUIDE_CANT_OPEN),
								  MSG(MSG_HELPFILE_NAME));
						unloadhelp = 1;
					} else {
						ShowError(MSG(MSG_ERROR_AGUIDE_GENERIC),
								  GetAmigaGuideString(agm->agm_Sec_Ret));
					}
					PendingAGuideMsg = NULL;	/* Cancel any pending cmd */
				}
				break;
		}
		ReplyAmigaGuideMsg(agm);
	}
	if (unloadhelp)
		CleanupAGuide();
}

/*
 *		CleanupAGuide()
 *
 *		Frees all resources allocated by ShowAGuide(). Should only be
 *		called at the end of the program.
 */
void CleanupAGuide(void)
{
	if (AG_Context)		CloseAmigaGuide(AG_Context),	AG_Context = NULL;
	if (AmigaGuideBase)	CloseLibrary(AmigaGuideBase),	AmigaGuideBase = NULL;
	AmigaGuideMask = 0;
}

/*
 *		CreateCustomImage(imagetype, height)
 *
 *		Creates an image of the specified type (IMAGE_FILE or IMAGE_FONT)
 *		and returns a pointer to an image structure with details about
 *		the image. The return value + 1 points to the same image, but
 *		selected.
 *
 *		height is the height of the image -- the width is fixed at 16 pixels.
 *
 *		N.b. ScreenScreen should be valid when this function is called. You
 *		should call FreeCustomImage(imageptr) to free up any memory allocated
 *		when you're done.
 */
struct Image *CreateCustomImage(int imagetype, int height)
{
#define NUM_ELS(x)	(sizeof(x) / sizeof(x[0]))

	ULONG *imagedata  = FileButtonData;
	int imbytesperrow = 4;	/* Must be even */
	int imwidth		  = 20;
	int imheight      = NUM_ELS(FileButtonData);
	int depth		  = SnoopScreen->RastPort.BitMap->Depth;

	struct CustomImage  *ci;
	struct CustomBitMap *cbm;
	UBYTE *bitplane;
	int size;

	if (imagetype == IMAGE_FONT) {
		imagedata = FontButtonData;
		imheight  = NUM_ELS(FontButtonData);
		if (height <= 12)	/* Strip last line if gadget is tiny */
			imheight--;
	}

	/*
	 *		For the gadget, we need to store two full bitmaps, one for
	 *		the selected image and one for the unselected image. We
	 *		allocate a single structure in CHIP ram that holds all the
	 *		details we need.
	 */
	size = sizeof(struct CustomImage) + imbytesperrow * height * depth * 2;
	ci   = AllocMem(size, MEMF_CHIP | MEMF_CLEAR);
	if (!ci)
		return (NULL);
	
	ci->size = size;

	/*
	 *		Now initialise our two images -- the steps are almost identical
	 *		for each, except that one is filled and one is not.
	 */
	bitplane = (UBYTE *)(ci+1);
	for (cbm = ci->image; cbm < &ci->image[2]; cbm++) {
		struct RastPort *rport  = &cbm->rport;
		struct BitMap   *bitmap = &cbm->bitmap;
		int fillpen;
		int textpen;
		int shinepen;
		int shadowpen;
		int planemask;
		int i;

		InitRastPort(rport);
		InitBitMap(bitmap, depth, imwidth, height);

		/*
		 *		Now initialise the plane pointers accordingly
		 */
		for (i = 0; i < depth; i++) {
			bitmap->Planes[i] = bitplane;
			bitplane += (imbytesperrow * height);
		}
		rport->BitMap = bitmap;

		/*
		 *		Now we're ready to render our chosen image.
		 *		First let's draw the box and box highlight.
		 *		If we're drawing the selected state, we
		 *		invert the highlight colours to make the box
		 *		look indented.
		 */
		shinepen	= ScreenDI->dri_Pens[SHINEPEN];
		shadowpen	= ScreenDI->dri_Pens[SHADOWPEN];
		if (cbm != ci->image) {
			int swappen = shinepen;
			shinepen  	= shadowpen;
			shadowpen 	= swappen;
		}
		SetDrMd(rport, JAM1);
		SetAPen(rport, shinepen);
		Move(rport, 0, height-1);
		Draw(rport, 0, 0);
		Draw(rport, imwidth-2, 0);
		Move(rport, 1, height-2);
		Draw(rport, 1, 1);
		SetAPen(rport, shadowpen);
		Move(rport, imwidth-1, 0);
		Draw(rport, imwidth-1, height-1);
		Draw(rport, 1, height-1);
		Move(rport, imwidth-2, 1);
		Draw(rport, imwidth-2, height-2);

		/*
		 *		Now, if we're on the second round, fill in the box
		 *		in the select colour
		 */
		fillpen = 0;
		textpen = 1;
		if (cbm != ci->image) {
			fillpen = ScreenDI->dri_Pens[FILLPEN];
			textpen = ScreenDI->dri_Pens[FILLTEXTPEN];
			SetAPen(rport, fillpen);
			RectFill(rport, 2, 1, imwidth-3, height-2);
		}
		/*
		 *		Now the tricky bit -- we need to copy our image data
		 *		into the middle of the box icon we've just created.
		 *		Luckily, the data is already word-aligned for us so
		 *		we just need to worry about vertically centering it.
		 *		We use a little XOR trick to figure out which planes
		 *		need to be updated, and also to do the updating.
		 */
		planemask = fillpen ^ textpen;
		for (i = 0; i < depth; i++) {
			if (planemask & (1 << i)) {
				ULONG *src  = imagedata;
				ULONG *dest = ((ULONG *)(cbm->bitmap.Planes[i])) +
							  (height - imheight) / 2;
				int j;

				for (j = 0; j < imheight; j++)
					*dest++ ^= *src++;
			}
		}
		/*
		 *		All done, now initialise the image accordingly
		 */
		cbm->image.Width     = imwidth;
		cbm->image.Height    = height;
		cbm->image.Depth     = depth;
		cbm->image.ImageData = (UWORD *)(cbm->bitmap.Planes[0]);
		cbm->image.PlanePick = (1 << depth) - 1;
	}
	/*
	 *		To ensure that our two image structures our consecutive,
	 *		we copy the second bitmap's image back into the first
	 *		image's bitmap's alternate image
	 */
	ci->image[0].altimage = ci->image[1].image;
	return ((struct Image *)ci);
}

/*
 *		FreeCustomImage(image)
 *
 *		Release a custom image allocated earlier by CreateCustomImage()
 */
void FreeCustomImage(struct Image *image)
{
	if (image)
		FreeMem(image, ((struct CustomImage *)image)->size);
}

/*
 *		ConvertIMsgToChar()
 *
 *		Takes an IntuiMessage of IDCMP_RAWKEY and returns the single
 *		character representation it corresponds to. If this isn't
 *		possible (e.g. a HELP key or cursor key) then returns 0
 *		instead.
 */
int ConvertIMsgToChar(struct IntuiMessage *imsg)
{
	static struct InputEvent ev;		/* Ensure initalised to 0 */
	char buffer[9];
	int len;

	if (imsg->Class != IDCMP_RAWKEY)
		return (0);

	ev.ie_Class 	 	= IECLASS_RAWKEY;
	ev.ie_Code  	 	= imsg->Code;
	ev.ie_Qualifier 	= imsg->Qualifier;
	ev.ie_EventAddress	= *(APTR *)imsg->IAddress;
	len = MapRawKey(&ev, buffer, 8, NULL);
	if (len != 1)
		return (0);
	return (buffer[0]);
}
