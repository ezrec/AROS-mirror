
/* hidden.c */
VOID mySetRast(struct VT340Console *con);
VOID myScrollRaster(struct VT340Console *con, WORD dx, WORD dy, WORD xMin, WORD yMin,	WORD xMax, WORD yMax);
VOID myRectFill(struct VT340Console *con, WORD xMin, WORD yMin, WORD xMax, WORD yMax, BYTE fgPen);
VOID myMove_Text(struct VT340Console *con, UWORD xPos, UWORD yPos, STRPTR string, UWORD count);
VOID myBltBitMapRastPort(struct VT340Console *con, struct BitMap *srcBitMap, LONG xSrc, LONG ySrc, LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm);


VOID	KPrintF(UBYTE *, ...);
VOID VT340_writecon(struct VT340Console *, UBYTE *, LONG);
BOOL VT340_options(struct VT340Console *);

/* utils.a */
VOID *GetSucc(VOID *);
VOID *GetHead(VOID *);
VOID *GetTail(VOID *);
VOID *GetPred(VOID *);
VOID __stdargs BZero(VOID *, LONG);
VOID __stdargs BSet(VOID *, LONG, LONG);

/* ESC commands */
VOID	VT340_invokeESC(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_invokeCSI(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_invokeDCS(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_invokePrivate(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_hardreset(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_savecursor(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_restorecursor(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setTab(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_tabControl(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_screenaligntest(struct VT340Console *);
VOID	VT340_setG0(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG1(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG2(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG3(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG0_GL(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG1_GL(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG1_GR(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG2_GL(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG2_GR(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG3_GL(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG3_GR(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG2_GL_s(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setG3_GL_s(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setconvert(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_requestterminal(struct VT340Console *, UBYTE *, ULONG);

/* CSI commands */
VOID	VT340_ignore(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_insertchars(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_cursorup(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_cursordown(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_simplecursorup(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_simplecursordown(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_simplecursorright(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_simplecursorleft(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_cursornextline(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_cursoraboveline(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_cursorposition(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_eraseindisplay(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_eraseinline(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_sel_eraseindisplay(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_sel_eraseinline(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_erasechars(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_scrollcon(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_moveCursDown(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_moveCursForward(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setrow(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setcol(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_insertlines(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_deletelines(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_deletechars(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_tabClear(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_tabForward(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_tabBackward(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_ANSIsetmodes(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_DECsetmodes(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_ANSIresetmodes(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_DECresetmodes(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_printcontrolmode(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_autoprintmode(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_textmodes(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_devicestatreport(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setserviceclass(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setnumpad(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_reportANSIsettings(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_reportDECsettings(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setselective(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setregion(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_reportcursorstate(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_reportUPSS(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_softreset(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_selectactivedisplay(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_reportprestate(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_loadleds(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_setstatusline(struct VT340Console *, UBYTE *, ULONG);
VOID	VT340_adjustCursorColumnPosition(struct VT340Console *con);


/* VT-52 functions */
VOID VT52_EnterGrafixMode(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_ExitGrafixMode(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_Identify(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_EnterAltKeyboardMode(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_ExitAltKeyboardMode(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_EnterANSI(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_EnterAutoPrint(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_ExitAutoPrint(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_EnterControlPrint(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_ExitControlPrint(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_PrintScreen(struct VT340Console *, UBYTE *, ULONG);
VOID VT52_PrintRow(struct VT340Console *, UBYTE *, ULONG);


/* TEK functions */
BOOL TEK_open(struct VT340Console *);
BOOL TEK_parse(struct VT340Console *, UBYTE);
VOID TEK_close(struct VT340Console *);

/* support routines */
VOID	VT340_locked(struct VT340Console *, BOOL);
VOID	VT340_resetcon(struct VT340Console *, BOOL);
VOID	VT340_resetscreen(struct VT340Console *, BOOL);
VOID	VT340_resetstyles(struct VT340Console *, BOOL);
VOID	VT340_resetfonts(struct VT340Console *con);
VOID	VT340_cancel(struct VT340Console *);
VOID	VT340_invokeVT52(struct VT340Console *);
BOOL	VT340_openfonts(struct VT340Console *);
VOID	VT340_closefonts(struct VT340Console *);
VOID	VT340_setlset(struct VT340Console *, UWORD);
VOID	VT340_setrset(struct VT340Console *, UWORD);
VOID	VT340_setset(struct VT340Console *, UWORD, UWORD);
UWORD	VT340_cset(struct VT340Console *, UBYTE);
BOOL	VT340_setupconsole(struct VT340Console *, BOOL);
LONG	AtoL(UBYTE *str);
VOID	VT340_printterminator(struct VT340Console *, BPTR);
BOOL	VT340_openprinterFH(struct VT340Console *);
BOOL	VT340_openscreenFH(struct VT340Console *);
ULONG	VT340_printcontrolled(struct VT340Console *, BPTR, UBYTE *, ULONG);
VOID	VT340_configopt(struct xem_option *, UBYTE *, UBYTE *, UWORD, BOOL, BOOL);
BOOL	VT340_setopt(struct xem_option *, UWORD *, UWORD, ULONG, ULONG);
VOID	VT340_checkdims(struct VT340Console *);
VOID	VT340_cursorhome(struct VT340Console *);
VOID	VT340_getTopBot(struct VT340Console *con, WORD *top, WORD *bot);
VOID	VT340_standardTabs(struct VT340Console *con);
BOOL	VT340_isTab(struct VT340Console *con);
VOID	VT340_setActualTabPos(struct VT340Console *con);
VOID	VT340_clearActualTabPos(struct VT340Console *con);
VOID	VT340_clearAllTabs(struct VT340Console *con);
VOID	VT340_colorModes(struct VT340Console *con);
VOID	VT340_closescreenFH(struct VT340Console *con);
VOID	VT340_closeprinterFH(struct VT340Console *con);
VOID	VT340_printinfo(struct VT340Console *con, BPTR);
VOID	DeleteLockedWindow(struct VT340Console *con);
VOID	CreateLockedWindow(struct VT340Console *con);
VOID	VT340_newframe(struct VT340Console *con, struct IBox *frame);
ULONG GetTextStyle(struct VT340Console *con);
VOID	VT340_getRowsCols(struct VT340Console *con, struct IBox *border, WORD *rows, WORD *cols);


/* ParseInit.c */
BOOL VT340_ParseInit(struct VT340Console *con, STRPTR prefsFile, ULONG mode);


/* page.c */
UBYTE	PageGetLineMode(struct VT340Console *con);
VOID	RefreshPage(struct VT340Console *con);
VOID	DeletePage(struct VT340Console *con);
BOOL	CreatePage(struct VT340Console *con);
VOID	PageEraseInDisplay(struct VT340Console *, UWORD);
VOID	PageEraseInLine(struct VT340Console *, UWORD);
VOID	PageSelectiveEraseScreen(struct VT340Console *, UWORD);
VOID	PageSelectiveEraseLine(struct VT340Console *, UWORD);
VOID	PageEraseCharacters(struct VT340Console *, WORD, UWORD);
VOID	PageInsertChars(struct VT340Console *con, WORD col, UBYTE cnt);
VOID	PageDeleteChars(struct VT340Console *con, WORD col, UBYTE cnt);
VOID	PageScrollRegion(struct VT340Console *, WORD, WORD, WORD);
VOID	PageScrollDisplay(struct VT340Console *con, WORD direction, WORD sTop, WORD sBot);
/*VOID	PageShiftChars(struct VT340Console *, WORD);*/
VOID	PageGetLine(struct VT340Console *);
VOID	PagePrintDisplay(struct VT340Console *, BPTR, BOOL);
VOID	PrintScaled(struct VT340Console *con, UBYTE *buffer, LONG size, WORD col, WORD row, UBYTE scaleMode, BOOL reScale);
BOOL	CreateScale(struct VT340Console *con);
VOID	DeleteScale(struct VT340Console *con);
VOID	CreateOwnStatusLine(struct VT340Console *con);
VOID	DeleteStatusLine(struct VT340Console *con, BOOL ownOnly);
VOID	PageTextScaleMode(struct VT340Console *con, UBYTE *args, ULONG argc);
VOID	PageTextOut(struct VT340Console *);
VOID	PageEraseLines(struct VT340Console *con, WORD row, UWORD cnt);
VOID	PageCursorFlip(struct VT340Console *con);
VOID	PageCursorVisible(struct VT340Console *con);
VOID	PageCursorInvisible(struct VT340Console *con);
VOID	ASyncTimer(struct timerequest *doTime, ULONG ticks, BOOL *requestPending);
BOOL	CreateCursorServer(struct VT340Console *con);
BOOL	DeleteCursorServer(struct VT340Console *con);


/* transfer.c */
VOID VT340_transfer(struct VT340Console *con);
