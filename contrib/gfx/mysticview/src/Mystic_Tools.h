/*********************************************************************
----------------------------------------------------------------------

	MysticView
	toolbox
	
----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_TOOLS_H
#define	MYSTIC_TOOLS_H 1

#include "Mystic_Screen.h"
#include "Mystic_Settings.h"
#include "Mystic_Window.h"


extern void WindowClear(struct mvwindow *win);
extern void WindowBackFill(struct mvwindow *win, UWORD width, UWORD height, UWORD winxoffs, UWORD winyoffs);
extern void SetAlternateWindowPosition(struct mvscreen *scr, struct mvwindow *window);
extern char **CreateFilePatternList(struct WBArg *wbargs, int numargs);
extern void DeleteFilePatternList(char **list);

extern BOOL FileRequest(struct FileRequester *fr, struct Window *win, STRPTR title, char *pathname, char *filename, char **newpathname, char **newfilename);
extern BOOL SaveRequest(struct FileRequester *fr, struct Window *win, STRPTR title, char *pathname, char *filename, char **newpathname, char **newfilename);

extern char **MultiFileRequest(struct FileRequester *fr, struct Window *win, char *title, char *pathname, char *filename, char **newpathname, char **newfilename, BOOL onlypictures, char *rejectpattern);

extern char **CreateStringList(int num);
extern void DeleteStringList(char **list);
extern char **DupStringList(char **list);
extern char *_StrDupTrim(char *string);

extern BOOL ModeRequest(struct ScreenModeRequester *sr, struct Window *win, struct mainsettings *settings, STRPTR title);

extern char *FullName(char *pathname, char *filename);
extern BOOL PathRequest(struct FileRequester *fr, struct Window *win, STRPTR title, char *pathname,	char **newpathname);


extern struct List *CreateList(void);
extern void DeleteNode(struct Node *node);
extern void DeleteList(struct List *list);
extern int CountListEntries(struct List *list);
extern char **CreateArrayFromList(struct List *list);
extern struct List *CreateListFromArray(char **array);
extern char **LoadStringList(char *filename);
extern char *DupPath(char *filename);
extern BOOL Exists(char *filename);
extern BOOL SameFile(char *f1, char *f2);
extern struct DateStamp *GetFileDate(char *filename);
extern int StrLen(char *s);
extern long FileSize(char *filename);
extern BOOL CallSubTask(LONG (* function)(APTR data), APTR data, int prio, int stack, LONG *result);


extern STRPTR STDARGS _StrDupCat(ULONG dummy, ...);


#endif
