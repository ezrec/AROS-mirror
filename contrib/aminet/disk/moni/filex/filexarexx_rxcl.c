#include <exec/types.h>
#include <dos/dos.h>
#include <rexx/storage.h>

#include "filexarexx.h"

#define RESINDEX(stype) (((long) &((struct stype *)0)->res) / sizeof(long))

char	RexxPortBaseName[80] = "FILEX";
char	*rexx_extension = "FILEX";

struct rxs_command rxs_commandlist[] =
{
#ifdef AREXX
	{ "ACTIVATE", NULL, NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_activate, ARB_CF_ENABLED ,sizeof(struct rxd_activate)},
	{ "ACTIVATEVIEW", "VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_activateview, ARB_CF_ENABLED ,sizeof(struct rxd_activateview)},
	{ "ACTIVATEWINDOW", "WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_activatewindow, ARB_CF_ENABLED ,sizeof(struct rxd_activatewindow)},
	{ "ADJUSTWINDOWSIZE", "WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_adjustwindowsize, ARB_CF_ENABLED ,sizeof(struct rxd_adjustwindowsize)},
	{ "APPEND","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_append, ARB_CF_ENABLED ,sizeof(struct rxd_append)},
	{ "ASSIGNCOMMAND", "NUMBER/K/N,NAME/K", 0,0,(void(*)(struct RexxHost*,void **,long)) rx_assigncommand, ARB_CF_ENABLED ,sizeof(struct rxd_assigncommand)},
	{ "CHANGEWINDOW", "LEFTEDGE/K/N,TOPEDGE/K/N,HEIGHT/K/N,WIDTH/K/N,WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_changewindow, ARB_CF_ENABLED ,sizeof(struct rxd_changewindow)},
	{ "CLEAR", "FORCE/S,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_clear, ARB_CF_ENABLED ,sizeof(struct rxd_clear)},
	{ "CLOSE", "VIEW/S,WINDOW/S,FORCE/S,VIEWID=VID=WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_close, ARB_CF_ENABLED ,sizeof(struct rxd_close)},
	{ "COLUMN", "DELTA/N/A,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_column, ARB_CF_ENABLED ,sizeof(struct rxd_column)},
	{ "COPY","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_copy, ARB_CF_ENABLED ,sizeof(struct rxd_copy)},
	{ "CURSOR", "UP/S,DOWN/S,LEFT/S,RIGHT/S,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_cursor, ARB_CF_ENABLED ,sizeof(struct rxd_cursor)},
	{ "CUT","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_cut, ARB_CF_ENABLED ,sizeof(struct rxd_cut)},
	{ "DEACTIVATE", NULL, NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_deactivate, ARB_CF_ENABLED ,sizeof(struct rxd_deactivate)},
	{ "EXECUTECOMMAND", "NUMBER/K/N,NAME/K", 0, 0,(void(*)(struct RexxHost*,void **,long)) rx_executecommand, ARB_CF_ENABLED ,sizeof(struct rxd_executecommand)},
	{ "EXPANDVIEW", "VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_expandview, ARB_CF_ENABLED ,sizeof(struct rxd_expandview)},
	{ "FAULT", "NUMBER/N/A", "DESCRIPTION", RESINDEX(rxd_fault),(void(*)(struct RexxHost*,void **,long)) rx_fault, ARB_CF_ENABLED|ARB_CF_CALLFREE ,sizeof(struct rxd_fault)},
	{ "FILL","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_fill, ARB_CF_ENABLED ,sizeof(struct rxd_fill)},
	{ "FIND", "PROMPT/S,HEX/S,BACKWARDS/S,QUIET/S,TEXT/F,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_find, ARB_CF_ENABLED ,sizeof(struct rxd_find)},
	{ "FINDCHANGE", "PROMPT/S,HEX/S,ALL/S,BACKWARDS/S,QUIET/S,FIND,CHANGE,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_findchange, ARB_CF_ENABLED ,sizeof(struct rxd_findchange)},
	{ "FINDNEXT", "BACKWARDS/S,QUIET/S,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_findnext, ARB_CF_ENABLED ,sizeof(struct rxd_findnext)},
	{ "FONT", "NAME/A,SIZE/N/A,WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_font, ARB_CF_ENABLED ,sizeof(struct rxd_font)},
	{ "GETATTR", "OBJECT/A,FIELD,ID/K/N,STEM/K,VAR/K",0,0,(void(*)(struct RexxHost*,void **,long)) rx_getattr, ARB_CF_ENABLED ,sizeof(struct rxd_getattr)},
	{ "GETBLOCK", NULL, "BLOCK", RESINDEX(rxd_getblock),(void(*)(struct RexxHost*,void **,long)) rx_getblock, ARB_CF_ENABLED|ARB_CF_CALLFREE ,sizeof(struct rxd_getblock)},
	{ "GETBYTES", "POS/N/A,NUMBER/N/A,FILEID=FID/K/N", "HEXSTRING", RESINDEX(rxd_getbytes),(void(*)(struct RexxHost*,void **,long)) rx_getbytes, ARB_CF_ENABLED|ARB_CF_CALLFREE ,sizeof(struct rxd_getbytes)},
	{ "GOTOBOOKMARK", "NUMBER/N,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_gotobookmark, ARB_CF_ENABLED ,sizeof(struct rxd_gotobookmark)},
	{ "GOTOBYTE", "POS/N/A,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_gotobyte, ARB_CF_ENABLED ,sizeof(struct rxd_gotobyte)},
	{ "GOTOCOLUMN", "COLUMN/N/A,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_gotocolumn, ARB_CF_ENABLED ,sizeof(struct rxd_gotocolumn)},
	{ "GOTOLINE", "LINE/N/A,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_gotoline, ARB_CF_ENABLED ,sizeof(struct rxd_gotoline)},
	{ "GRABMEMORY","START/N/A,END/N/A,FORCE/S,VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_grabmemory, ARB_CF_ENABLED ,sizeof(struct rxd_grabmemory)},
	{ "GROWVIEW", "VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_growview, ARB_CF_ENABLED ,sizeof(struct rxd_growview)},
	{ "HELP", "COMMAND", "COMMANDDESC,COMMANDLIST/M", RESINDEX(rxd_help),(void(*)(struct RexxHost*,void **,long)) rx_help, ARB_CF_ENABLED|ARB_CF_CALLFREE ,sizeof(struct rxd_help)},
	{ "INSERT","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_insert, ARB_CF_ENABLED ,sizeof(struct rxd_insert)},
	{ "LINE", "DELTA/N/A,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_line, ARB_CF_ENABLED ,sizeof(struct rxd_line)},
	{ "LOADBLOCK", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_loadblock, ARB_CF_ENABLED ,sizeof(struct rxd_loadblock)},
	{ "LOADCOMMANDS", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_loadcommands, ARB_CF_ENABLED ,sizeof(struct rxd_loadcommands)},
	{ "LOADDISPLAYTYP", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_loaddisplaytyp, ARB_CF_ENABLED ,sizeof(struct rxd_loaddisplaytyp)},
	{ "LOADSETTINGS", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_loadsettings, ARB_CF_ENABLED ,sizeof(struct rxd_loadsettings)},
	{ "LOCKGUI", NULL, NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_lockgui, ARB_CF_ENABLED ,sizeof(struct rxd_lockgui)},
	{ "MOVEWINDOW", "LEFTEDGE/N,TOPEDGE/N,WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_movewindow, ARB_CF_ENABLED ,sizeof(struct rxd_movewindow)},
	{ "NEW", "VIEW/S,WINDOW/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_new, ARB_CF_ENABLED ,sizeof(struct rxd_new)},
	{ "NEXT", "VIEW/S,WINDOW/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_next, ARB_CF_ENABLED ,sizeof(struct rxd_next)},
	{ "NOP", NULL, NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_nop, ARB_CF_ENABLED ,sizeof(struct rxd_nop)},
	{ "OPEN", "FILENAME,FORCE/S,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_open, ARB_CF_ENABLED ,sizeof(struct rxd_open)},
	{ "PASTE","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_paste, ARB_CF_ENABLED ,sizeof(struct rxd_paste)},
	{ "POSITION", "SOF/S,EOF/S,SOL/S,EOL/S,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_position, ARB_CF_ENABLED ,sizeof(struct rxd_position)},
	{ "PREVIOUS", "VIEW/S,WINDOW/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_previous, ARB_CF_ENABLED ,sizeof(struct rxd_previous)},
	{ "PRINT", "FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_print, ARB_CF_ENABLED ,sizeof(struct rxd_print)},
	{ "PRINTBLOCK",NULL,NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_printblock, ARB_CF_ENABLED ,sizeof(struct rxd_printblock)},
	{ "QUIT", "FORCE/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_quit, ARB_CF_ENABLED ,sizeof(struct rxd_quit)},
	{ "REDO", "NUMBER/N,FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_redo, ARB_CF_ENABLED ,sizeof(struct rxd_redo)},
	{ "REQUESTFILE", "TITLE/K,PATH/K,FILE/K,PATTERN/K", "FILENAME", RESINDEX(rxd_requestfile),(void(*)(struct RexxHost*,void **,long)) rx_requestfile, ARB_CF_ENABLED|ARB_CF_CALLINIT|ARB_CF_CALLFREE ,sizeof(struct rxd_requestfile)},
	{ "REQUESTNOTIFY", "PROMPT", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_requestnotify, ARB_CF_ENABLED ,sizeof(struct rxd_requestnotify)},
	{ "REQUESTRESPONSE", "BUTTONS/K,PROMPT/K", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_requestresponse, ARB_CF_ENABLED ,sizeof(struct rxd_requestresponse)},
	{ "REQUESTSTRING", "PROMPT/K,DEFAULT/K", "STRING", RESINDEX(rxd_requeststring),(void(*)(struct RexxHost*,void **,long)) rx_requeststring, ARB_CF_ENABLED|ARB_CF_CALLINIT|ARB_CF_CALLFREE ,sizeof(struct rxd_requeststring)},
	{ "RX", "CONSOLE/S,ASYNC/S,COMMAND/F", "RC/N,RESULT", RESINDEX(rxd_rx),(void(*)(struct RexxHost*,void **,long)) rx_rx, ARB_CF_ENABLED|ARB_CF_OWNALLOC|ARB_CF_CALLFREE|ARB_CF_CALLINIT,sizeof(struct rxd_rx)},
	{ "SAVE", "FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_save, ARB_CF_ENABLED ,sizeof(struct rxd_save)},
	{ "SAVEAS", "NAME,FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_saveas, ARB_CF_ENABLED ,sizeof(struct rxd_saveas)},
	{ "SAVEBLOCK", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_saveblock, ARB_CF_ENABLED ,sizeof(struct rxd_saveblock)},
	{ "SAVECOMMANDS", 0, NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_savecommands, ARB_CF_ENABLED ,sizeof(struct rxd_savecommands)},
	{ "SAVECOMMANDSAS", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_savecommandsas, ARB_CF_ENABLED ,sizeof(struct rxd_savecommandsas)},
	{ "SAVESETTINGS", 0, NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_savesettings, ARB_CF_ENABLED ,sizeof(struct rxd_savesettings)},
	{ "SAVESETTINGSAS", "NAME", NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_savesettingsas, ARB_CF_ENABLED ,sizeof(struct rxd_savesettingsas)},
	{ "SETATTR", "OBJECT/A,FIELD,ID/K/N,FROMSTEM/K,FROMVAR/K", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_setattr, ARB_CF_ENABLED ,sizeof(struct rxd_setattr)},
	{ "SETBLOCK", "BLOCK/A", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_setblock, ARB_CF_ENABLED ,sizeof(struct rxd_setblock)},
	{ "SETBLOCKMARK","VIEWID=VID/K/N",NULL,0,(void(*)(struct RexxHost*,void **,long)) rx_setblockmark, ARB_CF_ENABLED ,sizeof(struct rxd_setblockmark)},
	{ "SETBOOKMARK", "NUMBER/N,VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_setbookmark, ARB_CF_ENABLED ,sizeof(struct rxd_setbookmark)},
	{ "SETBYTES", "POS/N/A,HEXSTRING/A,FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_setbytes, ARB_CF_ENABLED ,sizeof(struct rxd_setbytes)},
	{ "SETSCREEN", "NAME/K,OWN/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_setscreen, ARB_CF_ENABLED ,sizeof(struct rxd_setscreen)},
	{ "SHRINKVIEW", "VIEWID=VID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_shrinkview, ARB_CF_ENABLED ,sizeof(struct rxd_shrinkview)},
	{ "SIZEWINDOW", "HEIGHT/N,WIDTH/N,WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_sizewindow, ARB_CF_ENABLED ,sizeof(struct rxd_sizewindow)},
	{ "SPLIT", "VIEW/S,WINDOW/S", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_split, ARB_CF_ENABLED ,sizeof(struct rxd_split)},
	{ "UNDO", "NUMBER/N,FILEID=FID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_undo, ARB_CF_ENABLED ,sizeof(struct rxd_undo)},
	{ "UNLOCKGUI", NULL, NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_unlockgui, ARB_CF_ENABLED ,sizeof(struct rxd_unlockgui)},
	{ "WINDOWTOBACK", "WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_windowtoback, ARB_CF_ENABLED ,sizeof(struct rxd_windowtoback)},
	{ "WINDOWTOFRONT", "WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_windowtofront, ARB_CF_ENABLED ,sizeof(struct rxd_windowtofront)},
	{ "ZIPWINDOW", "WINDOWID=WID/K/N", NULL, 0,(void(*)(struct RexxHost*,void **,long)) rx_zipwindow, ARB_CF_ENABLED ,sizeof(struct rxd_zipwindow)},
#endif
	{ NULL, NULL, NULL, NULL, NULL }
};

int		command_cnt = 65 + 12;

/* Neue Kommandos:
 * NEW VIEW/WINDOW
 * SPLIT VIEW/WINDOW
 * NEXT VIEW/WINDOW
 * PREVIOUS VIEW/WINDOW
 * CLOSE VIEW/WINDOW
 * EXPANDVIEW
 * GROWIEW
 * SHRINKVIEW

 * ADJUSTWINDOWSIZE ID
 * ACTIVATEVIEW ID

 * LOCKGUI
 * UNLOCKGUI
 */
