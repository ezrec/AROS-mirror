/* hidden.c */
VOID mySetRast(struct BBSConsole *con);
VOID myScrollRaster(struct BBSConsole *con, WORD dx, WORD dy, WORD xMin, WORD yMin,	WORD xMax, WORD yMax);
VOID myRectFill(struct BBSConsole *con, WORD xMin, WORD yMin, WORD xMax, WORD yMax, UBYTE fgPen);
VOID myMove_Text(struct BBSConsole *con, UWORD xPos, UWORD yPos, STRPTR string, UWORD count);


/* ESC commands */
VOID	BBS_invokeESC(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_invokeCSI(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_hardreset(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_savecursor(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_restorecursor(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_huntST(struct BBSConsole *, UBYTE *, ULONG);

/* CSI commands */
VOID	BBS_ignore(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_insertchars(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursorup(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursordown(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursorright(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursorleft(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursornextline(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursoraboveline(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_cursorposition(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_eraseindisplay(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_eraseinline(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_insertlines(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_deletelines(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_deletechars(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_scrollup(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_scrolldown(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_ANSIsetmodes(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_ANSIresetmodes(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_LRsetmodes(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_LRresetmodes(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_textmodes(struct BBSConsole *, UBYTE *, ULONG);
VOID	BBS_devicestatreport(struct BBSConsole *, UBYTE *, ULONG);

/* support routines */
VOID	BBS_reset(struct BBSConsole *);
VOID	BBS_cancel(struct BBSConsole *);
VOID	BBS_textout(struct BBSConsole *);
VOID	BBS_erasechars(struct BBSConsole *, UWORD);
VOID	BBS_eraselines(struct BBSConsole *, UWORD);
VOID	BBS_eraselinebeg(struct BBSConsole *);
VOID	BBS_erasescrbeg(struct BBSConsole *);
VOID	BBS_erasescreen(struct BBSConsole *);
VOID	BBS_cursorVisible(struct BBSConsole *);
VOID	BBS_cursorInvisible(struct BBSConsole *);
VOID	BBS_cursorFlip(struct BBSConsole *);
VOID	BBS_setupconsole(struct BBSConsole *);
BOOL	BBS_readsetup(struct BBSConsole *);
VOID	BBS_internalsettings(struct BBSConsole *con);
BOOL	BBS_parseoptions(struct BBSConsole *con, STRPTR optionsBuffer);
VOID	ASyncTimer(struct timerequest *doTime, ULONG ticks, BOOL *requestPending);
BOOL	CreateCursorServer(struct BBSConsole *);
BOOL	DeleteCursorServer(struct BBSConsole *);
VOID	BBS_newframe(struct BBSConsole *con, struct IBox *frame);
VOID	BBS_checkframe(struct BBSConsole *con);
VOID	BBS_clearoldframe(struct BBSConsole *con);
VOID	BBS_clearwholeframe(struct BBSConsole *con);
VOID	BBS_resetstyles(struct BBSConsole *con, BOOL colorsOnly);



VOID BBS_writecon(struct BBSConsole *, UBYTE *, ULONG);
