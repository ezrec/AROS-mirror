Prototype const UBYTE * version;
Prototype ED * uninit_init (ED * ep);
Prototype int setpen (Line line, Column column);
Prototype WORD inversemode (int n);
Prototype int text_init (ED * oldep, WIN * win, struct NewWindow * nw);
Prototype BOOL text_switch (WIN * win);
Prototype BOOL text_sync (void);
Prototype BOOL text_adjust (BOOL force);
Prototype void text_load (void);
Prototype int text_colno (void);
Prototype int text_lineno (void);
Prototype int text_lines (void);
Prototype int text_cols (void);
Prototype int text_imode (void);
Prototype int text_tabsize (void);
Prototype UBYTE * text_name (void);
Prototype void text_uninit (void);
Prototype void text_cursor (int n);
Prototype void text_position (int col, int row);
Prototype void text_displayseg (int start, int lines);
Prototype void text_redraw_cmdline (void);
Prototype void text_redisplay (void);
Prototype void text_redisplaycurrline (void);
Prototype void text_write (UBYTE * str);
Prototype void text_redrawblock (BOOL on);
Prototype void redraw_text (Line start_line, Line end_line);
Prototype void redraw_textlineseg (Line line, Column start_column, Column end_column);
Prototype void redraw_lineseg (UBYTE * text, UWORD y, Column start_column, Column end_column);
Prototype ULONG RexxMask;
Prototype int foundcmd; /* control for implicit ARexx macro invocation	 */
Prototype int cmderr;	/* global command error flag for do_rexx()'s use */
Prototype long do_rexx (const char * port, const char *fmt,...);
Prototype char * get_rexx_result (void);
Prototype void openrexx (void);
Prototype void closerexx (void);
Prototype long do_rxImplied (char * cmd,char * args);
Prototype void extern_rexx_command (void);
Prototype struct FileRequester	 * FReq;
Prototype struct ReqToolsBase	 * ReqToolsBase;
Prototype struct rtFileRequester * RFReq;
Prototype int aslsave (void);
Prototype void fixdirname (char * dir);
Prototype void splitpath (char * name, char * file, char * dir);
Prototype int reqsave (void);
Prototype void check_stack (ED * ep);
Prototype int tas_flag (char * what, int old, char * prefix);
Prototype void loadconfig (ED * ep);
Prototype LIST	    DBase;
Prototype ED	  * Ep;
Prototype struct Config default_config;
Prototype struct GlobalFlags globalflags;
Prototype long	    Nsu;
Prototype UBYTE     CtlC;
Prototype UBYTE     Current[MAXLINELEN];
Prototype UBYTE     Deline[MAXLINELEN];
Prototype UBYTE     Space[32];
Prototype Column    Clen;
Prototype UBYTE   * Partial;
Prototype UBYTE   * esc_partial;
Prototype UBYTE   * String;
Prototype UWORD     ColumnPos[MAXLINELEN];
Prototype UWORD     RowPos[MAXROWS];
Prototype UWORD     Xsize;
Prototype UWORD     Ysize;
Prototype UWORD     XTbase;
Prototype UWORD     YTbase;
Prototype UWORD     Lines;
Prototype UWORD     Columns;
Prototype UWORD     Xbase;
Prototype UWORD     Ybase;
Prototype UWORD     Xpixs;
Prototype UWORD     Ypixs;
Prototype UWORD     LineDistance;
Prototype UBYTE   * av[];
Prototype WORD	    PageJump;
Prototype UBYTE     RexxPortName[8];
Prototype UBYTE     tmp_buffer[MAXLINELEN];
Prototype UWORD     NumClicks;
Prototype ULONG     LoopCont;
Prototype ULONG     LoopBreak;
Prototype ULONG     MacroRecord;
Prototype struct IntuitionBase * IntuitionBase;
Prototype struct GfxBase       * GfxBase;
Prototype struct Library       * IconBase;
Prototype struct Library       * AslBase;
Prototype struct DosLibrary    * DOSBase;
Prototype struct WBStartup     * Wbs;
Prototype void swapmem (void *, void *, ULONG);
Prototype void makemygadget (struct Gadget *gad);
Prototype int firstns (char * str);
Prototype int lastns (char * str);
Prototype int wordlen (char * str);
Prototype BOOL getpathto (BPTR lock, char * filename, char * buf);
Prototype LINE allocline (long size);
Prototype void freeline (LINE line);
Prototype int detab (char * ibuf, char * obuf, int maxlen);
Prototype int xefgets (FILE * fi, char * buf, int max);
Prototype ED * finded (char * str, int doff);
Prototype void mountrequest (int bool);
Prototype FONT * GetFont (char * name, WORD  size);
Prototype void movetocursor (void);
Prototype int extend (ED * ep, int lines);
Prototype int makeroom (int n);
Prototype void freelist (LINE * list, int n);
Prototype long lineflags (int line);
Prototype void scroll_display (WORD dx, WORD dy, Column lc, Line tl, Column rc, Line bl);
Prototype void ScrollAndUpdate (int dx, int dy);
Prototype char * skip_whitespace (char * ptr);
Prototype char is_number (char * str);
Prototype char * getnextcomline (FILE* fi, int * lineno);
Prototype char * fname (char * fullpath);
Prototype BOOL switch_ed (ED * newed);
Prototype void MakeRectVisible (WIN * win, UWORD minx, UWORD miny, UWORD maxx, UWORD maxy);
Prototype int LINELEN (ED * ep, Line nr);
Prototype void iconify (void);
Prototype WIN * TOpenWindow (struct NewWindow * nw);
Prototype WIN * opensharedwindow (struct NewWindow *nw);
Prototype void closesharedwindow (WIN * win);
Prototype int getyn (char * title, char * text, char * gads, ...);
Prototype void show_title (char *);
Prototype void title (char * fmt, ...);
Prototype void format_string (char * dest, const char * fmt);
Prototype void window_title (void);
Prototype void set_window_params (void);
Prototype char * geoskip (char * ptr, int * pval, int * psgn);
Prototype void GeometryToNW (char * geo, struct NewWindow *nw);
Prototype void rest_prop (ED * ep);
Prototype void rem_prop (ED * ep);
Prototype struct PropGadget * add_prop (struct Window * win);
Prototype void free_prop (struct PropGadget * pg);
Prototype void prop_adj (void);
Prototype ULONG new_top (void);
Prototype void error (char * fmt, ...);
Prototype void warn (char * fmt, ...);
Prototype struct MsgPort  * Sharedport;
Prototype char		    MShowTitle;
Prototype char		    noadj;
Prototype struct GfxBase  * GfxBase;
Prototype struct __XDMEArgs XDMEArgs;
Prototype void PrintCWD (void);
Prototype int main (int mac, char ** mav);
Prototype void OptimizedRefresh (ED *);
Prototype void exiterr (char * str);
Prototype int breakcheck (void);
Prototype void breakreset (void);
Prototype void ops (char ** toolarray);
Prototype BOOL loadfile (BPTR lock, char * name);
Prototype LIST PBase;
Prototype void path_init (void);
Prototype NOT_BLOCK_C_CONST Block     ActualBlock;
Prototype Block * get_block (Block * buf);
Prototype Block * set_block (Block *buf);
Prototype Block * set_block_nud (Block *buf);
Prototype ED * set_block_ep (ED * new_ed);
Prototype UWORD set_block_type (UWORD type);
Prototype UWORD set_block_flags (UWORD newflags, UWORD flagmask);
Prototype void set_block_start (Line line, Column column);
Prototype void set_block_end (Line line, Column column);
Prototype void unblock (void);
Prototype Column block_leftcolumn (Line);
Prototype Column block_rightcolumn (Line);
Prototype void block_normalize (void);
Prototype UWORD is_inblock (Line line, Column col);
Prototype BOOL IsBlockVisible (void);
Prototype void displayblock (BOOL on);
Prototype BOOL block_ok (void);
Prototype char * block_to_string (void);
Prototype char	* HistoBuff[];
Prototype WORD	  NumHistoLines;
Prototype WORD	  HistoLine;
Prototype int get_char (ED * ep, struct pos * curr, int dir);
Prototype int get_pong (int num);
Prototype void escapecomlinemode (void);
Prototype void markerkill (ED * ep);
Prototype int do_command (char * str);
Prototype char * breakout (char ** ptr, char * quoted, char ** paux);
Prototype int getappname (char * buffer, int bsize);
Prototype int saveit (const char * om);
Prototype APTR AllocFunc (long siz, long typ);
Prototype void FreeFunc (APTR mem, long siz);
Prototype void DeallocFunc (char * str);
Prototype char * DupFunc (const char * str, long typ);
Prototype ULONG 	       OpenRexx 	    (char *, const char *, __stkargs void (*do_res)(struct RexxMsg *), BPTR, BPTR);
Prototype BOOL		       CloseRexx	    (void);
Prototype long		       LastRexxError	    (void);
Prototype struct RexxMsg     * BuildRexxMsg	    (const char *, const long, char *, ...);
Prototype BOOL		       RemoveRexxMsg	    (struct RexxMsg *);
Prototype BOOL		       PutRexxMsg	    (char *, struct RexxMsg *);
Prototype struct RexxMsg     * GetRexxMsg	    (void);
Prototype struct RexxMsg     * FetchRexxMsg	    (void);
Prototype BOOL		       ReplyRexxMsg	    (struct RexxMsg *, long, char *);
Prototype long		       SyncRexxCommand	    (const char *, const char *, char *, long, char *, ...);
Prototype __stkargs void       RexxMessageHandler   (struct RexxMsg *);
Prototype void		     * ChangeProcessMessage (__stkargs void (*new_func)(struct RexxMsg *));
Prototype struct RexxCommand * SetRexxCommandTable  (struct RexxCommand *);
Prototype long		       ModifyRepCount	    (long);
Prototype BOOL		       AttempCloseRexx	    (void);
Prototype void redraw_block (BOOL force, Line start_line, Column start_column, Line end_line, Column end_column);
Prototype void	    init_variables  (void);
Prototype char *    GetTypedVar     (const char *, int *);
Prototype int	    SetTypedVar     (const char *, const char *, int);
Prototype void	    do_set	    (void);
Prototype void	    do_unset	    (void);
Prototype void	    do_setenv	    (void);
Prototype void	    do_unsetenv     (void);
Prototype void	    do_settvar	    (void);
Prototype void	    do_unsettvar    (void);
Prototype int	    is_tflagset     (int);              /* Proto'd for access from control.c */
Prototype int	    is_gflagset     (int);              /* Proto'd for access from control.c */
Prototype void	    do_toggleflag   (void);
Prototype char *    getvar	    (const char *);
Prototype void	 DelAllVarsFromTree ( VBASE * tree );
Prototype void	 DelVarFromTree     ( VBASE * tree, char * name );
Prototype char * GetVarFromTree     ( VBASE * tree, char * name );
Prototype void	 SetVarIntoTree     ( VBASE * tree, char * name, char * value );
Prototype char* names2string   ( MLIST* list, char* buffer, char delimiter, void* filter );  /* assumes buffer is 1/4k * BUFFERS chgars long */
Prototype int	numnodes       ( MLIST* list );
Prototype NODE* findnode       ( MLIST* list, char* name, short len );
Prototype void	DelAllVarsFromList ( MLIST* list );
Prototype void	DelVarFromList ( MLIST* list, char* name );
Prototype char* GetVarFromList ( MLIST* list, char* name );
Prototype void	SetVarIntoList ( MLIST* list, char* name, char* value );
Prototype int	 test_arg  (char*, int);
Prototype char * GetFlag   (char*, char*, int, char*);
Prototype char	 SetFlag   (char*, char*, char*, int, char*);
Prototype int	 IsFlagSet (char*, int, int);
Prototype char	SPC_set (const char *name, const char *value);
Prototype char *SPC_get (const char *name);
Prototype UBYTE *ltostr (long l);
Prototype char *strrep (char *old, const char *new);
Prototype char * current_word (void);
Prototype void do_setspecialvar(void);
Prototype char ReqString[MAXLINELEN];
Prototype UBYTE 	    *APIC_IconTitle;
Prototype UBYTE 	    *APIC_IconFile;
Prototype UBYTE 	    *APIC_ActionDblClick;
Prototype UBYTE 	    *APIC_ActionDrop;
Prototype ULONG 	     APIC_SigMask;
Prototype void APIC_Terminate (void);
Prototype void APIC_Initialize (void);
Prototype void APIC_Control (void);
Prototype int do_apicshow (void);
Prototype int do_apichide (void);
Prototype ULONG CMDSH_SigMask;
Prototype UBYTE*CMDSH_FileName;
Prototype UBYTE*CMDSH_Prompt;
Prototype void	CMDSH_Terminate  (void);
Prototype int	CMDSH_Initialize (void);
Prototype void	CMDSH_Control	 (void);
Prototype int	CMDSH_Print	 (const char *, ULONG *);
Prototype void	  do_quiet    (void);
Prototype void	  do_eval     (void);
Prototype void	  do_force    (void);
Prototype void	  do_abort    (void);
Prototype void	  do_unabort  (void);
Prototype void	  do_break    (void);
Prototype void	  do_continue (void);
Prototype void	  clearcont   (void);
Prototype void	  clearbreak  (void);
Prototype void	  clearbreaks (void);
Prototype void	  do_insertwrite (void);
Prototype void	  do_printf	 (void);
Prototype void	  m_write	 (char *);
Prototype void	  do_viewmode	 (void);
Prototype void	  do_simpletabs  (void);
Prototype void	  do_sourcebreak (void);
Prototype void	  do_fwovr	 (void);
Prototype void	  do_longline	 (void);
Prototype void	  do_unnamedmathfunc(void);
Prototype void	  do_namedmathfunc  (void);
Prototype void	  do_infixmode	    (void);
Prototype void	  do_swapV	    (void);
Prototype UBYTE * GetArg      (int);
Prototype UBYTE * CommandName (void);
Prototype void clear_record	(void);
Prototype void start_recording	(void);
Prototype void end_recording	(void);
Prototype void replay_record	(void);
Prototype void do_saverecord	(void);
Prototype void add_record	(char * string);
Prototype void do_simptr	(void);
Prototype void	do_setmacro	(void);
Prototype void	do_unsetmacro	(void);
Prototype void	do_setmacrovar	(void);
Prototype void	do_unsetmacrovar(void);
Prototype void	do_retmacro	(void);
Prototype void	do_macrosave	(void);
Prototype void	do_macroload	(void);
Prototype void	SetMacroVar	(char* name, char* value);
Prototype MACRO*getmacro	(char* name);
Prototype int	callmacro	(MACRO*);
Prototype int	nummacroargs	(MACRO*);
Prototype char* getmacroarg	(char* name);
Prototype char* getmacrovar	(char* name);
Prototype void	DelMacros	(void);
Prototype int	SetMacro	(char* name, char* body, int nargs);
Prototype int	 savemacros	 (FILE *);          /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */
Prototype int	 loadmacros	 (FILE *, int *);   /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */
Prototype int	 GetDefMacros	 (void);            /* PATCH_NULL [25 Jan 1993] : proto'd for pack.c */
Prototype int init_macros (void);
Prototype int do_readtplt (void);
Prototype void do_delmenu   (void);
Prototype void do_delitem   (void);
Prototype void do_delitembar(void);
Prototype void do_delsub    (void);
Prototype void do_delsubbar (void);
Prototype void do_itembar   (void);
Prototype void do_itemcheck (void);
Prototype void do_itemadd   (void);
Prototype void do_subadd    (void);
Prototype void do_subcheck  (void);
Prototype void do_subbar    (void);
Prototype void do_menuoff   (void);
Prototype void do_menuon    (void);
Prototype void do_menuload  (void);
Prototype void do_menusave  (void);
Prototype char*menutomacro  (char*);
Prototype void do_menuclear (void);
Prototype void do_getcheckitem (void);
Prototype void do_getchecksub  (void);
Prototype void do_setcheckitem (void);
Prototype void do_setchecksub  (void);
Prototype void do_new_menustrip (void);
Prototype void do_del_menustrip (void);
Prototype void do_use_menustrip (void);
Prototype APTR menufind (char *name);
Prototype char *menugethelp(APTR handle);
Prototype char *menugetcommand(APTR handle);
Prototype char *menusetcommand(APTR handle, char *val);
Prototype char *menusethelp (APTR handle, char *val);
Prototype int menucall (APTR handle);
Prototype void		menuclear   (MENUSTRIP *ms);
Prototype int		menudel     (MENUSTRIP *ms, char *tname);
Prototype int		itemdel     (MENUSTRIP *ms, char *tname, char *iname);
Prototype int		subdel	    (MENUSTRIP *ms, char *tname, char *iname, char *sname);
Prototype XITEM *	itemadd     (MENUSTRIP *ms, char *tname, char *iname,              char *macro, char *help, int check, char *scut);
Prototype XITEM *	subadd	    (MENUSTRIP *ms, char *tname, char *iname, char *sname, char *macro, char *help, int check, char *scut);
Prototype int		loadmenus   (MENUSTRIP *ms, FILE *fi,     int *lineno);
Prototype int		savemenus   (MENUSTRIP *ms, FILE *fo);
Prototype int		chkitemcheck(MENUSTRIP *ms, char *tname, char *iname);
Prototype int		chksubcheck (MENUSTRIP *ms, char *tname, char *iname, char *sname);
Prototype int		setitemcheck(MENUSTRIP *ms, char *tname, char *iname,              int status);
Prototype int		setsubcheck (MENUSTRIP *ms, char *tname, char *iname, char *sname, int status);
Prototype APTR		menu2macro  (MENUSTRIP *ms, char *str);
Prototype MENUSTRIP *get_menustrip    (char * name);
Prototype MENUSTRIP *new_menustrip    (char * name, int defaults);
Prototype void	     delete_menustrip (MENUSTRIP * kt, int force);
Prototype void	     exit_menustrips (void);
Prototype APTR menu_cmd (struct IntuiMessage * im);
Prototype void *active_menu;
Prototype void	menuon	     (MENUSTRIP * ms, struct Window * win);
Prototype void	menuoff      (MENUSTRIP * ms, struct Window * win);
Prototype void	menu_strip   (MENUSTRIP * ms, struct Window * Win);
Prototype void	menuload     (MENUSTRIP * ms,  char * fname);
Prototype void	menusave     (MENUSTRIP * ms,  char * fname);
Prototype void	fixmenu      (MENUSTRIP * ms, struct Window * win);
Prototype void* currentmenu  (void);
Prototype void* currentwindow(void);
Prototype char*keyspectomacro(char*);
Prototype void*currenthash  (void);
Prototype APTR keyfind (char *name);
Prototype char *keygethelp(APTR handle);
Prototype char *keygetcommand(APTR handle);
Prototype char *keysetcommand(APTR handle, char *val);
Prototype char *keysethelp (APTR handle, char *val);
Prototype int keycall (APTR handle);
Prototype void	 keyload	(KEYTABLE * kt, char * name);
Prototype void	 keysave	(KEYTABLE * kt, char * name);
Prototype int	 loadkeys	(KEYTABLE * kt, FILE * fi, int * lineno);
Prototype int	 savekeys	(KEYTABLE * kt, FILE * fo);
Prototype HASH * findhash	(KEYTABLE * kt, KEYSPEC *ks,       UBYTE *);
Prototype APTR	 keyspec2macro	(KEYTABLE * kt,              const UBYTE *);
Prototype int	 resethash	(KEYTABLE * kt);
Prototype int	 unmapkey	(KEYTABLE * kt, const UBYTE *key);
Prototype int	 mapkey 	(KEYTABLE * kt, const UBYTE *key, const UBYTE *value, const UBYTE *help);
Prototype int	 remhash	(KEYTABLE * kt, KEYSPEC *ks);
Prototype int	 addhash	(KEYTABLE * kt, KEYSPEC *ks, const UBYTE *comm, const UBYTE *help);
Prototype void	 dealloc_hash	(KEYTABLE * kt);
Prototype KEYTABLE * get_keytable    (char * name);
Prototype KEYTABLE * new_keytable    (char * name, int defaults);
Prototype void	     delete_keytable (KEYTABLE * kt, int force);
Prototype void	     exit_keytables (void);
Prototype void keyctl (struct IntuiMessage *, int, ULONG);  /* PATCH_NULL [14 Feb 1993] : changed to void */
Prototype struct Library * ConsoleDevice;
Prototype void keyboard_init (void);
Prototype UBYTE* cqtoa (KEYSPEC *ks);
Prototype BOOL get_codequal (const UBYTE *str, KEYSPEC *ks);
Prototype TQUAL iqual2qual (ULONG qual, int blen, char * buf, int code);
Prototype ULONG qual2iqual (TQUAL qual);
Prototype BOOL a2iqual (const UBYTE *str, ULONG *piqual);
Prototype UBYTE *iqual2a (ULONG iqual);
Prototype void init_kb (void);
Prototype UBYTE  CtoA (TCODE c);
Prototype TQUAL CIgnoreQ (TCODE c);
Prototype BOOL IsRawC (KEYSPEC *ks);
Prototype int DeadKeyConvert (struct IntuiMessage * msg, UBYTE * buf, int bufsize, struct KeyMap * keymap);
Prototype ULONG ext_qualifiers;
Prototype int  qualifier (char *);
DEFCMD(Line, SetLine, (ED * ep, Line line, int mode))
DEFCMD(Line, AddToLine, (ED * ep, int distance))
DEFCMD (void, UpdateEd, (ED * ep))
DEFUSERCMD("about", 0, CF_VWM|CF_ICO,void, do_about, (void),)
DEFUSERCMD("redisplay", 0, CF_VWM, void, do_redisplay, (void),)
DEFUSERCMD("rxresult", 1, CF_VWM|CF_ICO, void, put_rexx_result, (void),)
DEFUSERCMD("rx",  1, CF_VWM|CF_ICO, void , do_rx, (void),;)
DEFUSERCMD("rx1", 2, CF_VWM|CF_ICO, void , do_rx, (void),;)
DEFUSERCMD("rx2", 3, CF_VWM|CF_ICO, void , do_rx, (void),)
DEFUSERCMD("port", 2, CF_VWM|CF_ICO, void, to_port, (void),)
DEFUSERCMD("select", 1, CF_VWM|CF_ICO, void, select_window, (void),)
DEFUSERCMD("projectinfo", 0, CF_VWM|CF_ICO, void, project_info, (void),)
DEFUSERCMD("projectsave", 0, CF_VWM|CF_ICO, void, save_project, (void),)
DEFUSERCMD("projectload", 0, CF_VWM|CF_ICO, int, load_project, (void),)
DEFUSERCMD("pattern", 1, CF_VWM|CF_COK|CF_ICO, void, do_pattern, (void),)
DEFUSERCMD("arpload", 0, CF_VWM, void, do_aslload, (void),)
DEFUSERCMD("arpinsfile", 0, 0, void, do_aslinsfile, (void),)
DEFUSERCMD("arpfont", 0, CF_VWM, void, do_aslfont, (void),)
DEFUSERCMD("reqload", 0, CF_VWM, void, do_reqload, (void),)
DEFUSERCMD("reqinsfile", 0, 0, void, do_reqinsfile, (void),)
DEFUSERCMD("reqfont", 0, CF_VWM, void, do_reqfont, (void),)
DEFUSERCMD("if",     2, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),;)
DEFUSERCMD("ifelse", 3, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),;)
DEFUSERCMD("while",  2, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),)
DEFUSERCMD("repeat", 2, CF_VWM|CF_COK|CF_ICO, void, do_repeat, (void),)
DEFUSERCMD("scanf", 1, CF_VWM|CF_COK|CF_ICO, void, do_scanf, (void),)
DEFUSERCMD("push", 1, CF_VWM|CF_COK|CF_ICO, void, do_push, (void),)
DEFUSERCMD("pick", 1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),;)
DEFUSERCMD("peek", 1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),;)
DEFUSERCMD("pop",  1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),)
DEFUSERCMD("swap", 1, CF_VWM|CF_COK|CF_ICO, void, do_swap, (void),)
DEFUSERCMD("margin", 1, CF_VWM|CF_ICO|CF_COK, void, do_margin, (void),)
DEFUSERCMD("wordwrap", 1, CF_VWM|CF_COK|CF_ICO, void, do_wordwrap, (void),)
DEFUSERCMD("global", 1, CF_VWM|CF_COK|CF_ICO, void, do_global, (void),)
DEFUSERCMD("setparcol", 1, CF_VWM|CF_COK|CF_ICO, void, do_setparcol, (void),)
DEFUSERCMD("tabstop", 1, CF_VWM|CF_COK|CF_ICO, void, do_tabstop, (void),)
DEFUSERCMD("insertmode", 1, CF_VWM|CF_COK|CF_ICO, void, do_insertmode, (void),)
DEFUSERCMD("chfilename", 1, CF_VWM|CF_ICO, void, do_chfilename, (void),)
DEFUSERCMD("savetabs", 1, CF_VWM|CF_COK|CF_ICO, void, do_savetabs, (void),)
DEFUSERCMD("setfont", 2, CF_VWM, void, do_setfont, (void),)
DEFUSERCMD("ignorecase", 1, CF_VWM|CF_COK|CF_ICO, void, do_ignorecase, (void),)
DEFUSERCMD("saveconfig", 0, CF_VWM|CF_COK|CF_ICO, void, do_saveconfig, (void),)
DEFUSERCMD("fgpen", 1, CF_VWM, void, do_fgpen, (void),)
DEFUSERCMD("tfpen", 1, CF_VWM, void, do_tfpen, (void),)
DEFUSERCMD("tbpen", 1, CF_VWM, void, do_tbpen, (void),)
DEFUSERCMD("bgpen", 1, CF_VWM, void, do_bgpen, (void),)
DEFUSERCMD("hgpen", 1, CF_VWM, void, do_hgpen, (void),)
DEFUSERCMD("bbpen", 1, CF_VWM, void, do_bbpen, (void),)
DEFUSERCMD("modified", 1, CF_ICO, void, do_modified, (void),)
DEFUSERCMD("autoindent", 1, CF_VWM|CF_ICO, void, do_autoindent, (void),)
DEFUSERCMD("sizewindow", 1, CF_VWM|CF_COK, void, do_sizewindow, (void),)
DEFUSERCMD("autosplit", 1, CF_VWM|CF_ICO, void, do_autosplit, (void),)
DEFUSERCMD("taskpri", 1, CF_VWM|CF_COK|CF_ICO, void, do_taskpri, (void),)
DEFUSERCMD("debug", 1, CF_VWM|CF_ICO, void, do_debug, (void),)
DEFUSERCMD("dobackup", 1, CF_VWM|CF_ICO, void, do_dobackup, (void),)
DEFUSERCMD("autounblock", 1, CF_VWM|CF_ICO, void, do_autounblock, (void),)
DEFUSERCMD("followcursor", 1, CF_VWM|CF_COK|CF_ICO, void, do_followcursor, (void),)
DEFUSERCMD("nicepaging", 1, CF_VWM|CF_COK|CF_ICO, void, do_nicepaging, (void),)
DEFUSERCMD("iconactive", 1, CF_VWM|CF_COK|CF_ICO, void, do_iconactive, (void),)
DEFUSERCMD("blockendsfloat", 1, CF_VWM|CF_COK|CF_ICO, void, do_blockendsfloat, (void),)
DEFUSERCMD("sline", 1, CF_VWM|CF_COK|CF_ICO, void, do_sline, (void),)
DEFUSERCMD("spacing", 1, CF_VWM, void, do_setlinedistance, (void),)
DEFUSERCMD("bs",   0, CF_COK, void, do_bs, (void),;)
DEFUSERCMD("del",  0, CF_COK, void, do_bs, (void),;)
DEFUSERCMD("back", 0, CF_COK, void, do_bs, (void),)
DEFUSERCMD("remeol", 0, CF_VWM|CF_COK, void, do_remeol, (void),)
DEFUSERCMD("split", 0, 0, void, do_split, (void),)
DEFUSERCMD("join", 0, 0, int, do_join, (void),)
DEFUSERCMD("reformat", 0, 0, void, do_reformat, (int n),)
DEFUSERCMD("insline", 0, 0, void, do_insline, (void),)
DEFUSERCMD("deline", 0, 0, void, do_deline, (void),)
DEFUSERCMD("tlate", 1, CF_COK, void, do_tlate, (void),)
DEFUSERCMD("justify", 1, 0, void, do_justify, (void),)
DEFUSERCMD("unjustify", 0, 0, void, do_unjustify, (void),)
DEFUSERCMD("indent", 2, 0, void, do_indent, (void),)
DEFUSERCMD("inslines", 1, 0, void, do_inslines, (void),)
DEFUSERCMD("delete", 0, CF_COK, void, do_delete, (void),)
DEFUSERCMD("delines", 1, 0, void, do_delines, (void),)
DEFUSERCMD("undeline", 0, 0, void, do_undeline, (void),)
DEFUSERCMD("clipins", 0, CF_COK, void, do_clipins, (void),)
DEFUSERCMD("space", 0, CF_COK, void, do_space, (void),)
DEFUSERCMD("return", 0, CF_COK, void, do_return, (void),)
DEFUSERCMD("undo", 0, 0, void, do_undo, (void),)
DEFUSERCMD("makecursorvisible", 0, CF_VWM|CF_COK, void, do_makecursorvisible, (void),)
DEFUSERCMD("iconify", 0, CF_VWM|CF_ICO, void, do_iconify, (void),)
DEFUSERCMD("setdeficontitle", 1, CF_VWM|CF_ICO|CF_COK, void, do_setdeficontitle, (void),)
DEFUSERCMD("uniconify", 0, CF_VWM|CF_ICO, void, uniconify, (void),)
DEFUSERCMD("newwindow", 0, CF_VWM|CF_ICO, void, do_newwindow, (void),)
DEFUSERCMD("openwindow", 1, CF_VWM|CF_ICO, void, do_openwindow, (void),)
DEFUSERCMD("setdeftitle", 1, CF_VWM|CF_ICO|CF_COK, void, do_setdeftitle, (void),)
DEFUSERCMD("resize", 2, CF_VWM, void, do_resize, (void),)
DEFUSERCMD("title", 1, CF_VWM|CF_ICO, void, do_title, (void),)
DEFUSERCMD("showlog", 0, CF_VWM|CF_ICO|CF_COK, void, do_showlog, (void),)
DEFUSERCMD("setgeometry", 4, CF_VWM, void, do_setgeometry, (void),)
DEFUSERCMD("toback", 0, CF_VWM|CF_COK|CF_ICO, void, do_toback, (void),)
DEFUSERCMD("tofront", 0, CF_VWM|CF_COK|CF_ICO, void, do_tofront, (void),)
DEFUSERCMD("tomouse", 0, CF_VWM, void, do_tomouse, (void),)
DEFUSERCMD("pubscreen", 1, CF_VWM|CF_ICO,void, set_pubscreen, (void),)
DEFUSERCMD("addpath", 1, CF_VWM|CF_ICO, void, do_addpath, (void),)
DEFUSERCMD("rempath", 1, CF_VWM|CF_COK|CF_ICO, void, do_rempath, (void),)
DEFUSERCMD("vrefctags", 1, CF_VWM|CF_ICO, void, do_refctags, (void),;)
DEFUSERCMD("refctags",  0, CF_VWM|CF_ICO, void, do_refctags, (void),)
DEFUSERCMD("vctags", 1, CF_VWM, void, do_ctags, (void),;)
DEFUSERCMD("ctags",  0, CF_VWM, void, do_ctags, (void),)
DEFUSERCMD("vref", 1, CF_VWM|CF_ICO, void, do_refs, (void),;)
DEFUSERCMD("ref",  0, CF_VWM|CF_ICO, void, do_refs, (void),)
DEFUSERCMD("repstr",  1, CF_VWM, void, do_findstr, (void),;)
DEFUSERCMD("findstr", 1, CF_VWM, void, do_findstr, (void),)
DEFUSERCMD("findr", 2, 0, void, do_findr, (void),;)
DEFUSERCMD("nextr", 0, 0, void, do_findr, (void),;)
DEFUSERCMD("prevr", 0, 0, void, do_findr, (void),)
DEFUSERCMD("find", 1, CF_VWM, void, do_find, (void),;)
DEFUSERCMD("next", 0, CF_VWM, void, do_find, (void),;)
DEFUSERCMD("prev", 0, CF_VWM, void, do_find, (void),)
DEFUSERCMD("replace", 0, 0, void, do_replace, (void),)
DEFUSERCMD("blocktype", 1, CF_VWM, void, do_blocktype, (void),)
DEFUSERCMD("block",     0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("bstart",    0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("bend",      0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("unblock",   0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("lineblock", 0, CF_VWM, void, do_block, (void),)
DEFUSERCMD("copy", 0, CF_VWM|CF_ICO|CF_COK, void, do_copy, (void),)
DEFUSERCMD("bdelete", 0, 0, void, do_bdelete, (void),)
DEFUSERCMD("bcopy", 0, 0, void, do_bcopy, (void),)
DEFUSERCMD("bmove", 0, 0, void, do_bmove, (void),)
DEFUSERCMD("bsource", 0, CF_VWM|CF_ICO, void, do_bsource, (void),)
DEFUSERCMD("up", 0, CF_VWM|CF_COK, void, do_up, (void),)
DEFUSERCMD("scrolldown", 0, CF_VWM, void, do_scrolldown, (void),)
DEFUSERCMD("scrollup", 0, CF_VWM, void, do_scrollup, (void),)
DEFUSERCMD("down", 0, CF_VWM|CF_COK, void, do_down, (void),)
DEFUSERCMD("pagedown",  0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageleft",  0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageright", 0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageup",    0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageset",   1, CF_VWM, void, do_page, (void),)
DEFUSERCMD("downadd", 0, 0, void, do_downadd, (void),)
DEFUSERCMD("left", 0, CF_VWM|CF_COK, void, do_left, (void),)
DEFUSERCMD("right", 0, CF_VWM|CF_COK, void, do_right, (void),)
DEFUSERCMD("tab", 0, CF_VWM|CF_COK, void, do_tab, (void),)
DEFUSERCMD("backtab", 0, CF_VWM|CF_COK, void, do_backtab, (void),)
DEFUSERCMD("top", 0, CF_VWM, void, do_top, (void),)
DEFUSERCMD("bottom", 0, CF_VWM, void, do_bottom, (void),)
DEFUSERCMD("first", 0, CF_VWM|CF_COK, void, do_firstcolumn, (void),)
DEFUSERCMD("firstnb", 0, CF_VWM|CF_COK, void, do_firstnb, (void),)
DEFUSERCMD("last", 0, CF_VWM|CF_COK, void, do_lastcolumn, (void),)
DEFUSERCMD("goto", 1, CF_VWM, void, do_goto, (void),)
DEFUSERCMD("screentop", 0, CF_VWM, void, do_screentop, (void),)
DEFUSERCMD("screenbottom", 0, CF_VWM, void, do_screenbottom, (void),)
DEFUSERCMD("wleft", 0, CF_VWM|CF_COK, void, do_wleft, (void),)
DEFUSERCMD("wright", 0, CF_VWM|CF_COK, void, do_wright, (void),)
DEFUSERCMD("col", 1, CF_VWM|CF_COK, void, do_col, (void),)
DEFUSERCMD("scrollleft",  0, CF_VWM, void, do_scroll, (int dir),;)
DEFUSERCMD("scrollright", 0, CF_VWM, void, do_scroll, (int dir),)
DEFUSERCMD("match", 0, CF_VWM|CF_COK, void, do_match, (void),)
DEFUSERCMD("ping", 1, CF_VWM|CF_ICO, void, do_ping, (void),)
DEFUSERCMD("pong", 1, CF_VWM, void, do_pong, (void),)
DEFUSERCMD("recall", 0, CF_VWM, void, do_recall, (void),)
DEFUSERCMD("escimm", 1, CF_VWM|CF_PAR, void, do_esc, (void),;)
DEFUSERCMD("esc",    0, CF_VWM|CF_COK, void, do_esc, (void),)
DEFUSERCMD("nop",  0, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),;)
DEFUSERCMD("rem",  1, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),;)
DEFUSERCMD("null", 0, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),)
DEFUSERCMD("source", 1, CF_VWM|CF_COK|CF_ICO, void, do_source, (long do_err),)
DEFUSERCMD("quit", 0, CF_VWM|CF_ICO, void, do_quit, (void),)
DEFUSERCMD("quitall", 0, CF_VWM|CF_ICO, void, do_quitall, (void),)
DEFUSERCMD("execute", 1, CF_VWM|CF_ICO, void, do_execute, (void),)
DEFUSERCMD("newfile", 1, CF_VWM, int, do_edit, (void),;)
DEFUSERCMD("insfile", 1,      0, int, do_edit, (void),)
DEFUSERCMD("appendsave", 1, CF_VWM|CF_COK|CF_ICO, int, do_appendsave, (void),)
DEFUSERCMD("bappendsave", 1, CF_VWM|CF_COK|CF_ICO, int, do_bappendsave, (void),)
DEFUSERCMD("bsave", 1, CF_VWM|CF_ICO, int, do_bsave, (void),)
DEFUSERCMD("barpsave", 0, CF_VWM|CF_ICO, int, do_barpsave, (void),)
DEFUSERCMD("saveold", 0, CF_VWM|CF_COK|CF_ICO, int, do_save, (void),)
DEFUSERCMD("saveas", 1, CF_VWM|CF_COK|CF_ICO, int, do_saveas, (void),)
DEFUSERCMD("writeto", 1, CF_VWM|CF_COK|CF_ICO, int, do_writeto, (void),)
DEFUSERCMD("arpsave", 0, CF_VWM|CF_ICO, int, do_aslsave, (void),)
DEFUSERCMD("reqsave", 0, CF_VWM|CF_ICO, int, do_reqsave, (void),)
DEFUSERCMD("cd", 1, CF_VWM|CF_ICO, void, do_cd, (void),)
DEFUSERCMD("print", 1, CF_VWM|CF_ICO,void,do_print,(void),)
DEFUSERCMD("flag", 2, CF_VWM|CF_COK|CF_ICO, void, do_flag, (void),)
DEFUSERCMD("resettoggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("settoggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("settvar", 2, CF_VWM|CF_ICO|CF_COK, void, do_settvar, (void),)
DEFUSERCMD("toggle", 1, CF_VWM|CF_COK|CF_ICO, void, do_toggleflag, (void),)
DEFUSERCMD("unset", 1, CF_VWM|CF_COK|CF_ICO, void, do_unset, (void),)
DEFUSERCMD("unsetenv", 1, CF_VWM|CF_COK|CF_ICO, void, do_unsetenv, (void),)
DEFUSERCMD("unsettvar", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsettvar, (void),)
DEFUSERCMD("set", 2, CF_VWM|CF_COK|CF_ICO, void, do_set, (void),)
DEFUSERCMD("setenv", 2, CF_VWM|CF_COK|CF_ICO, void, do_setenv, (void),)
DEFUSERCMD("spc", 2, CF_VWM|CF_COK|CF_ICO, void, do_setspecialvar, (void),)
DEFUSERCMD("reqstring", 4, CF_COK|CF_VWM|CF_ICO, int, do_reqstring, (void),)
DEFUSERCMD("reqnumber",     6, CF_COK|CF_VWM|CF_ICO, int, do_reqnumber, (void),)
DEFUSERCMD("reqpalette", 2, CF_COK|CF_VWM|CF_ICO, int, do_reqpalette, (void),)
DEFUSERCMD("request", 3, CF_COK|CF_VWM|CF_ICO, int, do_request, (void),)
DEFUSERCMD("reqfile", 3, CF_COK|CF_VWM|CF_ICO, int, do_reqfile, (void),)
DEFUSERCMD("closecmdshell", 0, CF_COK|CF_ICO|CF_VWM, int, do_closecmdshell, (void),)
DEFUSERCMD("opencmdshell", 0, CF_COK|CF_ICO|CF_VWM, int, do_opencmdshell, (void),)
DEFUSERCMD("cmdshellout", 1, CF_COK|CF_ICO|CF_VWM, int, do_cmdshelloutput, (void),)
DEFUSERCMD("viewmode", 1, CF_VWM|CF_COK|CF_ICO, void,do_viewmode, (void),)
DEFUSERCMD("activatefront", 1, CF_VWM|CF_COK|CF_ICO, void, do_activefront, (void),)
DEFUSERCMD("windowcycling", 1, CF_VWM|CF_COK|CF_ICO, void, do_windowcyc, (void),)
DEFUSERCMD("abort", 0, CF_VWM|CF_ICO|CF_COK, void,do_abort, (void),)
DEFUSERCMD("unabort", 0, CF_VWM|CF_ICO|CF_COK, void,do_unabort, (void),)
DEFUSERCMD("continue", 0, CF_VWM|CF_ICO|CF_COK, void,do_continue, (void),)
DEFUSERCMD("break", 0, CF_VWM|CF_ICO|CF_COK, void,do_break, (void),)
DEFUSERCMD("rem", 1, CF_VWM|CF_ICO|CF_COK, void,do_null, (void),)
DEFUSERCMD("eval", 1, CF_VWM|CF_COK, void,do_eval, (void),)
DEFUSERCMD("force", 2, CF_VWM|CF_COK, void,do_force, (void),)
DEFUSERCMD("quiet", 1, CF_VWM|CF_ICO|CF_COK, void,do_quiet, (void),)
DEFUSERCMD("overwrite", 1, CF_COK, void,do_insertwrite, (void),)
DEFUSERCMD("insert", 1, CF_COK, void,do_insertwrite, (void),)
DEFUSERCMD("printf", 2, CF_COK, void,do_printf, (void),)
DEFUSERCMD("sourcebreaks", 1, CF_VWM|CF_COK|CF_ICO, void,do_sourcebreak, (void),)
DEFUSERCMD("math1", 2, CF_VWM|CF_ICO|CF_COK, void, do_unnamedmathfunc, (void),)
DEFUSERCMD("math2", 3, CF_VWM|CF_ICO|CF_COK, void, do_unnamedmathfunc, (void),)
DEFUSERCMD("infixmode", 1, CF_VWM|CF_ICO|CF_COK, void, do_infixmode, (void),)
DEFUSERCMD("inc", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("sub", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("add", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("dec", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("div", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("mod", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("mul", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("not", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("neg", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("swapv", 2, CF_VWM|CF_ICO|CF_COK, void, do_swapV, (void),)
DEFUSERCMD("recstart", 0, CF_VWM|CF_ICO|CF_COK, void, start_recording, (void),)
DEFUSERCMD("recend", 0, CF_VWM|CF_ICO|CF_COK, void, end_recording, (void),)
DEFUSERCMD("recplay", 0, CF_VWM|CF_ICO|CF_COK, void, replay_record, (void),)
DEFUSERCMD("recsave", 1, CF_VWM|CF_ICO|CF_COK, void, do_saverecord, (void),)
DEFUSERCMD("simptr", 2, CF_VWM|CF_COK, void, do_simptr, (void),)
DEFUSERCMD("smv",         2, CF_VWM|CF_ICO|CF_COK, void, do_setmacrovar, (void),;)
DEFUSERCMD("setmacrovar", 2, CF_VWM|CF_ICO|CF_COK, void, do_setmacrovar, (void),)
DEFUSERCMD("unsetmacrovar", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsetmacrovar, (void),)
DEFUSERCMD("ret", 0, CF_VWM|CF_ICO|CF_COK, void, do_retmacro, (void),)
DEFUSERCMD("setmacro", 3, CF_VWM|CF_ICO|CF_COK, void, do_setmacro, (void),)
DEFUSERCMD("unsetmacro", 1, CF_VWM|CF_ICO|CF_COK, void, do_unsetmacro, (void),)
DEFUSERCMD("macrosave", 1, CF_VWM|CF_ICO, void, do_macrosave, (void),)
DEFUSERCMD("macroload", 1, CF_VWM|CF_ICO, void, do_macroload, (void),)
DEFUSERCMD("readtemplate", 1, CF_ICO, int, do_readtplt, (void),)
DEFUSERCMD("pushvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_pushVAR, (void),)
DEFUSERCMD("dropvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_dropVAR, (void),)
DEFUSERCMD("pickvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_pickVAR, (void),)
DEFUSERCMD("purgevar", 1, CF_VWM|CF_COK|CF_ICO, int, do_purgeVAR, (void),)
DEFUSERCMD("popvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_popVAR, (void),)
DEFUSERCMD("swapvar", 1, CF_VWM|CF_COK|CF_ICO, int, do_swapVAR, (void),)
DEFUSERCMD("menuadd", 3, CF_VWM|CF_ICO, void, do_itemadd, (void),;)
DEFUSERCMD("menubar", 1, CF_VWM|CF_ICO, void, do_itembar, (void),;)
DEFUSERCMD("menucheck", 3, CF_VWM|CF_ICO, void, do_itemcheck, (void),;)
DEFUSERCMD("menuclear", 0, CF_VWM|CF_ICO, void, do_menuclear, (void),;)
DEFUSERCMD("menudel", 2, CF_VWM|CF_ICO, void, do_delitem, (void),;)
DEFUSERCMD("menudelbar", 1, CF_VWM|CF_ICO, void, do_delitembar, (void),;)
DEFUSERCMD("menudelhdr", 1, CF_VWM|CF_ICO, void, do_delmenu, (void),;)
DEFUSERCMD("menudelsub", 3, CF_VWM|CF_ICO, void, do_delsub, (void),;)
DEFUSERCMD("menudelsubar", 2, CF_VWM|CF_ICO, void, do_delsubbar, (void),;)
DEFUSERCMD("menugetcheck", 3, CF_VWM|CF_ICO, void, do_getcheckitem, (void),;)
DEFUSERCMD("menugetscheck", 4, CF_VWM|CF_ICO, void, do_getchecksub, (void),;)
DEFUSERCMD("menuload", 1, CF_VWM|CF_ICO, void, do_menuload, (void),;)
DEFUSERCMD("menuoff", 0, CF_VWM|CF_ICO, void, do_menuoff, (void),;)
DEFUSERCMD("menuon", 0, CF_VWM|CF_ICO, void, do_menuon, (void),;)
DEFUSERCMD("menusave", 1, CF_VWM|CF_ICO, void, do_menusave, (void),;)
DEFUSERCMD("menusetcheck", 3, CF_VWM|CF_ICO, void, do_setcheckitem, (void),;)
DEFUSERCMD("menusetscheck", 4, CF_VWM|CF_ICO, void, do_setchecksub, (void),;)
DEFUSERCMD("menusubadd", 4, CF_VWM|CF_ICO, void, do_subadd, (void),;)
DEFUSERCMD("menusubbar", 2, CF_VWM|CF_ICO, void, do_subbar, (void),;)
DEFUSERCMD("menusubcheck", 4, CF_VWM|CF_ICO, void, do_subcheck, (void),;)
DEFUSERCMD("newmenustrip", 1, CF_VWM|CF_ICO|CF_COK, void, do_new_menustrip, (void),;)
DEFUSERCMD("usemenustrip", 1, CF_VWM|CF_ICO|CF_COK, void, do_use_menustrip, (void),;)
DEFUSERCMD("map", 2, CF_VWM|CF_ICO, void, do_map, (void),)
DEFUSERCMD("unmap", 1, CF_VWM|CF_COK|CF_ICO, void, do_unmap, (void),)
DEFUSERCMD("keysave", 1, CF_VWM|CF_ICO, void, do_keysave, (void),)
DEFUSERCMD("keyload", 1, CF_VWM|CF_ICO, void, do_keyload, (void),)
DEFUSERCMD("qualifier", 1, CF_VWM|CF_ICO|CF_COK, void, do_qualifier, (void),)
DEFUSERCMD("newkeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_new_keytable, (void),)
DEFUSERCMD("remkeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_del_keytable, (void),)
DEFUSERCMD("usekeytable", 1, CF_VWM|CF_ICO|CF_COK, void, do_use_keytable, (void),)
