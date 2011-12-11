/*
 * Source generated with ARexxBox 1.11 (May  5 1993)
 * which is Copyright (c) 1992,1993 Michael Balzer
 */

#ifndef _filexARexx_H
#define _filexARexx_H

#define RXIF_INIT   1
#define RXIF_ACTION 2
#define RXIF_FREE   3

#define ARB_CF_ENABLED		(1L << 0)
#define ARB_CF_OWNALLOC		(1L << 1)
#define ARB_CF_CALLFREE		(1L << 2)
#define ARB_CF_CALLINIT		(1L << 3)

#define ARB_HF_CMDSHELL		(1L << 0)
#define ARB_HF_USRMSGPORT	(1L << 1)

struct RexxHost
{
	struct MsgPort *port;
	char portname[ 80 ];
	long replies;
	struct RDArgs *rdargs;
	long flags;
	LONG userdata;
	union {
		struct RexxMsg *akturexxmsg;
		BPTR outfh;
		};
};

struct rxs_command
{
	char *command, *args, *results;
	long resindex;
	void (*function)( struct RexxHost *, void **, long );
	long flags;
	long structsize;
};

#ifndef NO_GLOBALS
extern char RexxPortBaseName[80];
extern struct rxs_command rxs_commandlist[];
extern int command_cnt;
extern char *rexx_extension;
#endif

void ReplyRexxCommand( struct RexxMsg *rxmsg, long prim, long sec, char *res );
void FreeRexxCommand( struct RexxMsg *rxmsg );
struct RexxMsg *CreateRexxCommand( struct RexxHost *host, CONST_STRPTR buff, BPTR fh );
struct RexxMsg *CommandToRexx( struct RexxHost *host, struct RexxMsg *rexx_command_message );
struct RexxMsg *SendRexxCommand( struct RexxHost *host, CONST_STRPTR buff, BPTR fh );

void CloseDownARexxHost( struct RexxHost *host );
struct RexxHost *SetupARexxHost( char *basename, struct MsgPort *usrport );
struct rxs_command *FindRXCommand( char *com );
char *ExpandRXCommand( struct RexxHost *host, char *command );
void ARexxDispatch( struct RexxHost *host );

void DoShellCommand( struct RexxHost *host, char *comline, BPTR fhout );
void CommandShell( struct RexxHost *host, BPTR fhin, BPTR fhout, char *prompt );

/* rxd-Strukturen dürfen nur AM ENDE um lokale Variablen erweitert werden! */

struct rxd_clear
{
	long rc, rc2;
	struct {
		long force;
		APTR *id;
	} arg;
};

struct rxd_open
{
	long rc, rc2;
	struct {
		char *filename;
		long force;
		APTR *id;
	} arg;
};

struct rxd_quit
{
	long rc, rc2;
	struct {
		long force;
	} arg;
};

struct rxd_saveas
{
	long rc, rc2;
	struct {
		char *name;
		APTR *id;
	} arg;
};

struct rxd_save
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_print
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_nop {long rc, rc2;};
struct rxd_lockgui {long rc, rc2;};
struct rxd_unlockgui {long rc, rc2;};

struct rxd_requestnotify
{
	long rc, rc2;
	struct {
		char *prompt;
	} arg;
};

struct rxd_requestresponse
{
	long rc, rc2;
	struct {
		char *buttons;
		char *prompt;
	} arg;
};

struct rxd_requeststring
{
	long rc, rc2;
	struct {
		char *var, *stem;
		char *prompt;
		char *mydefault;
	} arg;
	struct {
		char *string;
	} res;
};

struct rxd_column
{
	long rc, rc2;
	struct {
		long *delta;
		APTR *id;
	} arg;
};

struct rxd_cursor
{
	long rc, rc2;
	struct {
		long up;
		long down;
		long left;
		long right;
		APTR *id;
	} arg;
};

struct rxd_gotobookmark
{
	long rc, rc2;
	struct {
		long *number;
		APTR *id;
	} arg;
};

struct rxd_gotocolumn
{
	long rc, rc2;
	struct {
		long *column;
		APTR *id;
	} arg;
};

struct rxd_gotoline
{
	long rc, rc2;
	struct {
		long *line;
		APTR *id;
	} arg;
};

struct rxd_line
{
	long rc, rc2;
	struct {
		long *delta;
		APTR *id;
	} arg;
};

struct rxd_position
{
	long rc, rc2;
	struct {
		long sof;
		long eof;
		long sol;
		long eol;
		APTR *id;
	} arg;
};

struct rxd_setbookmark
{
	long rc, rc2;
	struct {
		long *number;
		APTR *id;
	} arg;
};

struct rxd_find
{
	long rc, rc2;
	struct {
		long prompt;
		long hex;
		long backwards;
		long quiet;
		char *text;
		APTR *id;
	} arg;
};

struct rxd_findchange
{
	long rc, rc2;
	struct {
		long prompt;
		long hex;
		long all;
		long backwards;
		long quiet;
		char *find;
		char *change;
		APTR *id;
	} arg;
};

struct rxd_findnext
{
	long rc, rc2;
	struct {
		long backwards;
		long quiet;
		APTR *id;
	} arg;
};

struct rxd_activate {long rc, rc2;};

struct rxd_deactivate {long rc, rc2;};

struct rxd_getattr
{
	long rc, rc2;
	struct {
		char *object;
		char *field;
		APTR *id;
		char *stem, *var;
	} arg;
};

struct rxd_setattr
{
	long rc, rc2;
	struct {
		char *object;
		char *field;
		APTR *id;
		char *fromstem;
		char *fromvar;
	} arg;
};

struct rxd_fault
{
	long rc, rc2;
	struct {
		char *var, *stem;
		long *number;
	} arg;
	struct {
		char *description;
	} res;
};

struct rxd_help
{
	long rc, rc2;
	struct {
		char *var, *stem;
		char *command;
	} arg;
	struct {
		char *commanddesc;
		char **commandlist;
	} res;
};

struct rxd_rx
{
	long rc, rc2;
	struct {
		char *var, *stem;
		long console;
		long async;
		char *command;
	} arg;
	struct {
		long *rc;
		char *result;
	} res;
};

struct rxd_font
{
	long rc, rc2;
	struct {
		char *name;
		long *size;
		APTR *id;
	} arg;
};

struct rxd_redo
{
	long rc, rc2;
	struct {
		long *number;
		APTR *id;
	} arg;
};

struct rxd_undo
{
	long rc, rc2;
	struct {
		long *number;
		APTR *id;
	} arg;
};

struct rxd_activateview
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};
struct rxd_activatewindow
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_adjustwindowsize
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_changewindow
{
	long rc, rc2;
	struct {
		long *leftedge;
		long *topedge;
		long *height;
		long *width;
		APTR *id;
	} arg;
};

struct rxd_movewindow
{
	long rc, rc2;
	struct {
		long *leftedge;
		long *topedge;
		APTR *id;
	} arg;
};

struct rxd_sizewindow
{
	long rc, rc2;
	struct {
		long *height;
		long *width;
		APTR *id;
	} arg;
};

struct rxd_windowtoback
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_windowtofront
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_zipwindow
{
	long rc, rc2;
	struct {
		APTR *id;
	} arg;
};

struct rxd_requestfile
{
	long rc, rc2;
	struct {
		char *var, *stem;
		char *title;
		char *path;
		char *file;
		char *pattern;
	} arg;
	struct {
		char *filename;
	} res;
};

struct rxd_printblock {long rc, rc2;};
struct rxd_setblockmark	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_cut	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_copy	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_fill	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_paste	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_insert	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_append	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_expandview	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_growview	{		long rc, rc2;		struct {			APTR *id;		} arg;	};
struct rxd_shrinkview	{		long rc, rc2;		struct {			APTR *id;		} arg;	};


struct rxd_savesettings {long rc, rc2;};
struct rxd_savecommands {long rc, rc2;};

struct rxd_grabmemory
{
	long rc, rc2;
	struct {
		long *start,*end;
		long force;
		APTR *id;
	} arg;
};

struct rxd_assigncommand
{
	long rc, rc2;
	struct {
		long *number;
		char *name;
	} arg;
};

struct rxd_executecommand
{
	long rc, rc2;
	struct {
		long *number;
		char *name;
	} arg;
};

struct rxd_loadblock
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_saveblock
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_loadsettings
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_savesettingsas
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_loaddisplaytyp
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_loadcommands
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_savecommandsas
{
	long rc, rc2;
	struct {
		char *name;
	} arg;
};

struct rxd_setscreen
{
	long rc, rc2;
	struct {
		char *name;
		long own;
	} arg;
};

struct rxd_getblock
{
	long rc, rc2;
	struct {
		char *var, *stem;
	} arg;
	struct {
		char *block;
	} res;
};

struct rxd_setblock
{
	long rc, rc2;
	struct {
		char *block;
	} arg;
};

struct rxd_gotobyte
{
	long rc, rc2;
	struct {
		long *pos;
		APTR *id;
	} arg;
};

struct rxd_getbytes
{
	long rc, rc2;
	struct {
		char *var, *stem;
		long *pos,*number;
		APTR *id;
	} arg;
	struct {
		char *hexstring;
	} res;
};

struct rxd_setbytes
{
	long rc, rc2;
	struct {
		long *pos;
		char *hexstring;
		APTR *id;
	} arg;
};

struct rxd_close{
	long rc, rc2;
	struct {
		long view, window, force;
		APTR *id;
	} arg;
};


struct rxd_new{	long rc, rc2;	struct {		long view, window;	} arg;};
struct rxd_split{	long rc, rc2;	struct {		long view, window;	} arg;};
struct rxd_next{	long rc, rc2;	struct {		long view, window;	} arg;};
struct rxd_previous{	long rc, rc2;	struct {		long view, window;	} arg;};

void rx_clear( struct RexxHost *, struct rxd_clear **, long );
void rx_open( struct RexxHost *, struct rxd_open **, long );
void rx_print( struct RexxHost *, struct rxd_print **, long );
void rx_quit( struct RexxHost *, struct rxd_quit **, long );
void rx_save( struct RexxHost *, struct rxd_save **, long );
void rx_saveas( struct RexxHost *, struct rxd_saveas **, long );
void rx_nop( struct RexxHost *, struct rxd_nop **, long );
void rx_requestnotify( struct RexxHost *, struct rxd_requestnotify **, long );
void rx_requestresponse( struct RexxHost *, struct rxd_requestresponse **, long );
void rx_requeststring( struct RexxHost *, struct rxd_requeststring **, long );
void rx_column( struct RexxHost *, struct rxd_column **, long );
void rx_cursor( struct RexxHost *, struct rxd_cursor **, long );
void rx_gotobookmark( struct RexxHost *, struct rxd_gotobookmark **, long );
void rx_gotocolumn( struct RexxHost *, struct rxd_gotocolumn **, long );
void rx_gotoline( struct RexxHost *, struct rxd_gotoline **, long );
void rx_line( struct RexxHost *, struct rxd_line **, long );
void rx_position( struct RexxHost *, struct rxd_position **, long );
void rx_setbookmark( struct RexxHost *, struct rxd_setbookmark **, long );
void rx_find( struct RexxHost *, struct rxd_find **, long );
void rx_findchange( struct RexxHost *, struct rxd_findchange **, long );
void rx_findnext( struct RexxHost *, struct rxd_findnext **, long );
void rx_activate( struct RexxHost *, struct rxd_activate **, long );
void rx_deactivate( struct RexxHost *, struct rxd_deactivate **, long );
void rx_getattr( struct RexxHost *, struct rxd_getattr **, long );
void rx_setattr( struct RexxHost *, struct rxd_setattr **, long );
void rx_fault( struct RexxHost *, struct rxd_fault **, long );
void rx_help( struct RexxHost *, struct rxd_help **, long );
void rx_rx( struct RexxHost *, struct rxd_rx **, long );
void rx_font( struct RexxHost *, struct rxd_font **, long );
void rx_redo( struct RexxHost *, struct rxd_redo **, long );
void rx_undo( struct RexxHost *, struct rxd_undo **, long );
void rx_activatewindow( struct RexxHost *, struct rxd_activatewindow **, long );
void rx_activateview( struct RexxHost *, struct rxd_activateview **, long );
void rx_changewindow( struct RexxHost *, struct rxd_changewindow **, long );
void rx_movewindow( struct RexxHost *, struct rxd_movewindow **, long );
void rx_sizewindow( struct RexxHost *, struct rxd_sizewindow **, long );
void rx_windowtoback( struct RexxHost *, struct rxd_windowtoback **, long );
void rx_windowtofront( struct RexxHost *, struct rxd_windowtofront **, long );
void rx_requestfile( struct RexxHost *, struct rxd_requestfile **, long );
void rx_zipwindow( struct RexxHost *, struct rxd_zipwindow **, long );

void rx_grabmemory(struct RexxHost *,struct rxd_grabmemory **, long );
void rx_printblock(struct RexxHost *,struct rxd_printblock **, long );
void rx_setblockmark(struct RexxHost *,struct rxd_setblockmark **, long );
void rx_cut(struct RexxHost *,struct rxd_cut **, long );
void rx_copy(struct RexxHost *,struct rxd_copy **, long );
void rx_fill(struct RexxHost *,struct rxd_fill **, long );
void rx_paste(struct RexxHost *,struct rxd_paste **, long );
void rx_insert(struct RexxHost *,struct rxd_insert **, long );
void rx_append(struct RexxHost *,struct rxd_append **, long );
void rx_loadblock(struct RexxHost *,struct rxd_loadblock **, long );
void rx_saveblock(struct RexxHost *,struct rxd_saveblock **, long );
void rx_loadsettings(struct RexxHost *,struct rxd_loadsettings **, long );
void rx_savesettings(struct RexxHost *,struct rxd_savesettings **, long );
void rx_savesettingsas(struct RexxHost *,struct rxd_savesettingsas **, long );
void rx_loaddisplaytyp(struct RexxHost *,struct rxd_loaddisplaytyp **, long );
void rx_assigncommand(struct RexxHost *,struct rxd_assigncommand **, long );
void rx_executecommand(struct RexxHost *,struct rxd_executecommand **, long );
void rx_loadcommands(struct RexxHost *,struct rxd_loadcommands **, long );
void rx_savecommands(struct RexxHost *,struct rxd_savecommands **, long );
void rx_savecommandsas (struct RexxHost *,struct rxd_savecommandsas **, long );

void rx_setscreen (struct RexxHost *,struct rxd_setscreen **, long );
void rx_getblock (struct RexxHost *,struct rxd_getblock **, long );
void rx_setblock (struct RexxHost *,struct rxd_setblock **, long );
void rx_gotobyte (struct RexxHost *,struct rxd_gotobyte **, long );
void rx_getbytes (struct RexxHost *,struct rxd_getbytes **, long );
void rx_setbytes (struct RexxHost *,struct rxd_setbytes **, long );

void rx_new(struct RexxHost *,struct rxd_new **, long );
void rx_close(struct RexxHost *,struct rxd_close **, long );
void rx_next(struct RexxHost *,struct rxd_next **, long );
void rx_previous(struct RexxHost *,struct rxd_previous **, long );
void rx_split(struct RexxHost *,struct rxd_split **, long );
void rx_expandview(struct RexxHost *,struct rxd_expandview **, long );
void rx_growview(struct RexxHost *,struct rxd_growview **, long );
void rx_shrinkview(struct RexxHost *,struct rxd_shrinkview **, long );

void rx_adjustwindowsize( struct RexxHost *, struct rxd_adjustwindowsize **, long );
void rx_lockgui( struct RexxHost *, struct rxd_lockgui **, long );
void rx_unlockgui( struct RexxHost *, struct rxd_unlockgui **, long );

#endif
