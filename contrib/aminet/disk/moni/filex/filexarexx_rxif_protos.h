/* Prototypes for functions defined in
filexarexx_rxif.c
 */

BOOL ValidDD(struct DisplayData * );

BOOL ValidDI(struct DisplayInhalt * );

BOOL ValidFD(struct FileData * );

void rx_clear(struct RexxHost * , struct rxd_clear ** , long );

void rx_open(struct RexxHost * , struct rxd_open ** , long );

void rx_print(struct RexxHost * , struct rxd_print ** , long );

void rx_quit(struct RexxHost * , struct rxd_quit ** , long );

void rx_save(struct RexxHost * , struct rxd_save ** , long );

void rx_saveas(struct RexxHost * , struct rxd_saveas ** , long );

void rx_nop(struct RexxHost * , struct rxd_nop ** , long );

void rx_requestnotify(struct RexxHost * , struct rxd_requestnotify ** , long );

void rx_requestresponse(struct RexxHost * , struct rxd_requestresponse ** , long );

void rx_requeststring(struct RexxHost * , struct rxd_requeststring ** , long );

void rx_column(struct RexxHost * , struct rxd_column ** , long );

void rx_cursor(struct RexxHost * , struct rxd_cursor ** , long );

void rx_gotobookmark(struct RexxHost * , struct rxd_gotobookmark ** , long );

void rx_gotocolumn(struct RexxHost * , struct rxd_gotocolumn ** , long );

void rx_gotoline(struct RexxHost * , struct rxd_gotoline ** , long );

void rx_line(struct RexxHost * , struct rxd_line ** , long );

void rx_position(struct RexxHost * , struct rxd_position ** , long );

void rx_setbookmark(struct RexxHost * , struct rxd_setbookmark ** , long );

void rx_find(struct RexxHost * , struct rxd_find ** , long );

void rx_findchange(struct RexxHost * , struct rxd_findchange ** , long );

void rx_findnext(struct RexxHost * , struct rxd_findnext ** , long );

void rx_activate(struct RexxHost * , struct rxd_activate ** , long );

void rx_deactivate(struct RexxHost * , struct rxd_deactivate ** , long );

extern struct Attr applicationattributes[23];

extern struct Attr fileattributes[9];

extern struct Attr windowattributes[12];

extern struct Attr viewattributes[12];

extern struct AttrObject attributeobjects[7];

BOOL IsNumeric(STRPTR );

LONG FindAttrObject(UBYTE * );

LONG FindFieldAttr(UBYTE * , LONG );

void SetSingleAttr(struct rxd_setattr * , LONG , LONG , UBYTE * , APTR * );

void rx_setattr(struct RexxHost * , struct rxd_setattr ** , long );

UBYTE * GetSingleAttr(struct rxd_getattr * , LONG , LONG , APTR * );

LONG MySetRexxVar(struct RexxMsg * , char * , char * , ULONG , BPTR );

void rx_getattr(struct RexxHost * , struct rxd_getattr ** , long );

void rx_fault(struct RexxHost * , struct rxd_fault ** , long );

void rx_help(struct RexxHost * , struct rxd_help ** , long );

void rx_rx(struct RexxHost * , struct rxd_rx ** , long );

void rx_font(struct RexxHost * , struct rxd_font ** , long );

void rx_multiundofunction(BOOL , struct rxd_redo ** );

void rx_redo(struct RexxHost * , struct rxd_redo ** , long );

void rx_undo(struct RexxHost * , struct rxd_undo ** , long );

void rx_activatewindow(struct RexxHost * , struct rxd_activatewindow ** , long );

void rx_activateview(struct RexxHost * , struct rxd_activateview ** , long );

void rx_adjustwindowsize(struct RexxHost * , struct rxd_activatewindow ** , long );

void rx_changewindow(struct RexxHost * , struct rxd_changewindow ** , long );

void rx_movewindow(struct RexxHost * , struct rxd_movewindow ** , long );

void rx_sizewindow(struct RexxHost * , struct rxd_sizewindow ** , long );

void rx_multiwindowfunction(WORD , struct rxd_windowtoback ** );

void rx_zipwindow(struct RexxHost * , struct rxd_zipwindow ** , long );

void rx_windowtofront(struct RexxHost * , struct rxd_windowtofront ** , long );

void rx_windowtoback(struct RexxHost * , struct rxd_windowtoback ** , long );

void rx_requestfile(struct RexxHost * , struct rxd_requestfile ** , long );

void rx_grabmemory(struct RexxHost * , struct rxd_grabmemory ** , long );

void rx_printblock(struct RexxHost * , struct rxd_printblock ** , long );

void rx_multiddfunction(WORD , struct rxd_cut ** );

void rx_setblockmark(struct RexxHost * , struct rxd_setblockmark ** , long );

void rx_cut(struct RexxHost * , struct rxd_cut ** , long );

void rx_copy(struct RexxHost * , struct rxd_copy ** , long );

void rx_fill(struct RexxHost * , struct rxd_fill ** , long );

void rx_paste(struct RexxHost * , struct rxd_paste ** , long );

void rx_insert(struct RexxHost * , struct rxd_insert ** , long );

void rx_append(struct RexxHost * , struct rxd_append ** , long );

void rx_expandview(struct RexxHost * , struct rxd_expandview ** , long );

void rx_growview(struct RexxHost * , struct rxd_growview ** , long );

void rx_shrinkview(struct RexxHost * , struct rxd_shrinkview ** , long );

void rx_loadblock(struct RexxHost * , struct rxd_loadblock ** , long );

void rx_saveblock(struct RexxHost * , struct rxd_saveblock ** , long );

void rx_loadsettings(struct RexxHost * , struct rxd_loadsettings ** , long );

void rx_savesettings(struct RexxHost * , struct rxd_savesettings ** , long );

void rx_savesettingsas(struct RexxHost * , struct rxd_savesettingsas ** , long );

void rx_loaddisplaytyp(struct RexxHost * , struct rxd_loaddisplaytyp ** , long );

void rx_assigncommand(struct RexxHost * , struct rxd_assigncommand ** , long );

void rx_executecommand(struct RexxHost * , struct rxd_executecommand ** , long );

void rx_loadcommands(struct RexxHost * , struct rxd_loadcommands ** , long );

void rx_savecommands(struct RexxHost * , struct rxd_savecommands ** , long );

void rx_savecommandsas(struct RexxHost * , struct rxd_savecommandsas ** , long );

void rx_setscreen(struct RexxHost * , struct rxd_setscreen ** , long );

void rx_getblock(struct RexxHost * , struct rxd_getblock ** , long );

void rx_setblock(struct RexxHost * , struct rxd_setblock ** , long );

void rx_gotobyte(struct RexxHost * , struct rxd_gotobyte ** , long );

void rx_getbytes(struct RexxHost * , struct rxd_getbytes ** , long );

void rx_setbytes(struct RexxHost * , struct rxd_setbytes ** , long );

void rx_new(struct RexxHost * , struct rxd_new ** , long );

void rx_split(struct RexxHost * , struct rxd_split ** , long );

void rx_close(struct RexxHost * , struct rxd_close ** , long );

extern char * geburtstagstext;

void rx_next(struct RexxHost * , struct rxd_next ** , long );

void rx_previous(struct RexxHost * , struct rxd_previous ** , long );

void rx_lockgui(struct RexxHost * , struct rxd_lockgui ** , long );

void rx_unlockgui(struct RexxHost * , struct rxd_lockgui ** , long );

char * ExpandRXCommand(struct RexxHost * , char * );

