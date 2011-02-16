#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <proto/alib.h>
#include <proto/asl.h>
#include <proto/bgui.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <proto/utility.h>
#include <proto/keymap.h>

#ifdef __AROS__
#warning No textfield.gadget in AROS yet
#else
#include <clib/textfield_protos.h>
#endif

#include <exec/exec.h>

#ifdef __AROS__
#warning No textfield.gadget in AROS yet
#else
#include <gadgets/textfield.h>
#endif

#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/videocontrol.h>
#include <diskfont/diskfont.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuitionbase.h>
#include <libraries/bgui_macros.h>

#ifdef __AROS__
#define CATCOMP_NUMBERS
#include "locale.h"

STRPTR GetString(ULONG id);
void InitLocale(STRPTR catname, ULONG version);
void CleanupLocale(void);

#endif

#include "compilerspecific.h"

#define MenuTitle(i)\
  { NM_TITLE, NULL, NULL, 0, 0, (APTR)i }

#define PROJECT_MENU \
  MenuTitle(ID_PROJECTMENU), \
    Item(NULL,"O",ID_OPEN), \
    Item(NULL,"S",ID_SAVE), \
    Item(NULL,"D",ID_CHANGEDIR), \
    Item(NULL,"I",ID_PREVIEW), \
    ItemBar, \
    Item(NULL,"P",ID_PREFS), \
    Item(NULL,"?",ID_ABOUT), \
    ItemBar, \
    Item(NULL,"Q",ID_QUIT)
#define FONT_MENU \
  MenuTitle(ID_FONTMENU), \
    Item(NULL,"E",ID_EDITFONT), \
    Item(NULL,NULL,ID_EDITCHAR), \
    ItemBar, \
    Item(NULL,NULL,ID_MOVEFL), \
    Item(NULL,NULL,ID_MOVEFR), \
    Item(NULL,NULL,ID_MOVEFU), \
    Item(NULL,NULL,ID_MOVEFD), \
    ItemBar, \
    Item(NULL,"R",ID_RESETKERN), \
    ItemBar, \
    Item(NULL,"A",ID_CASCADE)
#define CHARACTER_MENU \
  MenuTitle(ID_CHARMENU), \
    Item(NULL,"Z",ID_UNDO), \
    Item(NULL,"X",ID_CUT), \
    Item(NULL,"C",ID_COPY), \
    Item(NULL,"V",ID_PASTE), \
    Item(NULL,NULL,ID_ERASE), \
    ItemBar, \
    Item(NULL,NULL,ID_MOVEL), \
    Item(NULL,NULL,ID_MOVER), \
    Item(NULL,NULL,ID_MOVEU), \
    Item(NULL,NULL,ID_MOVED), \
    ItemBar, \
    Item(NULL,NULL,ID_FLIPH), \
    Item(NULL,NULL,ID_FLIPV), \
    ItemBar, \
    Item(NULL,NULL,ID_ZOOMIN), \
    Item(NULL,NULL,ID_ZOOMOUT), \
    ItemBar, \
    Item(NULL,"W",ID_WIDTHC), \
    Item(NULL,"K",ID_KERNING)
#define PREVIEW_MENU \
  MenuTitle(ID_PREVIEWMENU), \
    Item(NULL,"T",ID_UPDATE), \
    Item(NULL,"H",ID_ALL), \
    Item(NULL,"L",ID_CLEAR)

#define NAME "Typeface"
#define VERSION "1.24"
#define CURRENT_PREFS 2

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

struct Character
{
  UWORD chr_Width,chr_Height;
  UBYTE *chr_Data;
};

struct TFPreferences
{
  UWORD Version;
  ULONG Flags, ScrModeID, ScrDepth;
  ULONG VecHeight, PixelX, PixelY;
  ULONG SaveTo;
  UBYTE ScrFontName[MAXFONTNAME];
  UWORD ScrFontHeight;
  UBYTE ThisPubName[MAXPUBSCREENNAME], DefPubName[MAXPUBSCREENNAME];
  UWORD ToolBar;
  UBYTE CharSize,CharWidth;
  UWORD CharHeight;
  UBYTE FixedFontName[MAXFONTNAME];
  UWORD FixedFontHeight;
};

#define PREFS_CUSTOMSCREEN	(1<<0)
#define PREFS_PIXELBORDER	(1<<1)
#define PREFS_PIXELGRID		(1<<2)
#define PREFS_MAXWINDOW		(1<<3)
#define PREFS_CONFIRMQUIT	(1<<4)
#define PREFS_SHOWBASELINE	(1<<5)
#define PREFS_AUTOTBARHEIGHT	(1<<6)
#define PREFS_INVERTSHIFT	(1<<7)
#define PREFS_NOTOGGLE		(1<<8)
#define PREFS_QUERYWIDTH	(1<<9)

#define TBAR_WIDTHL		0
#define FTBAR_WIDTHL		(1<<(TBAR_WIDTHL))
#define TBAR_WIDTHR		1
#define FTBAR_WIDTHR		(1<<(TBAR_WIDTHR))
#define TBAR_MOVEL		2
#define FTBAR_MOVEL		(1<<(TBAR_MOVEL))
#define TBAR_MOVER		3
#define FTBAR_MOVER		(1<<(TBAR_MOVER))
#define TBAR_MOVEU		4
#define FTBAR_MOVEU		(1<<(TBAR_MOVEU))
#define TBAR_MOVED		5
#define FTBAR_MOVED		(1<<(TBAR_MOVED))
#define TBAR_KERN		6
#define FTBAR_KERN		(1<<(TBAR_KERN))
#define TBAR_WIDTH		7
#define FTBAR_WIDTH		(1<<(TBAR_WIDTH))
#define TBAR_ZOOMIN		8
#define FTBAR_ZOOMIN		(1<<(TBAR_ZOOMIN))
#define TBAR_ZOOMOUT		9
#define FTBAR_ZOOMOUT		(1<<(TBAR_ZOOMOUT))
#define TBAR_FLIPH		10
#define FTBAR_FLIPH		(1<<(TBAR_FLIPH))
#define TBAR_FLIPV		11
#define FTBAR_FLIPV		(1<<(TBAR_FLIPV))

#define DEFAULT_TBAR \
  FTBAR_WIDTHL|FTBAR_WIDTHR|FTBAR_MOVEL| \
  FTBAR_MOVER|FTBAR_MOVEU|FTBAR_MOVED|FTBAR_KERN

/* typeface.c */

enum id { ID_OPEN = 1,		ID_SAVE,		ID_CHANGEDIR,
	  ID_EDITFONT,		ID_PREFS,		ID_ABOUT,
	  ID_QUIT,		ID_CHARS,		ID_CHARSCROLL,
	  ID_EDIT,		ID_WIDTHL,		ID_WIDTHR,
	  ID_WIDTHC,		ID_MOVEL,		ID_MOVER,
	  ID_MOVEU,		ID_MOVED,		ID_KERNING,
	  ID_COPY,		ID_PASTE,		ID_ACCEPT,
	  ID_CANCEL,		ID_PROPORTIONAL,	ID_HEIGHT,
	  ID_WIDTH,		ID_BASELINE,		ID_SMEAR,
	  ID_NORMAL,		ID_BOLD,		ID_ITALIC,
	  ID_ULINE,		ID_EXTEND,		ID_REVERSE,
	  ID_ASPECT,		ID_VECHEIGHT,		ID_PIXELWIDTH,
	  ID_PIXELHEIGHT,	ID_PIXELBORDER,		ID_ASSIGNSTR,
	  ID_ASSIGNPATH,	ID_ASSIGN,		ID_ASSIGNADD,
	  ID_CHARWIDTH,		ID_WIDTHPOS,		ID_SPACE,
	  ID_KERN,		ID_CUSTOM,		ID_CHOOSEMODE,
	  ID_CHOOSEFONT,	ID_SAVETO,		ID_USE,
	  ID_FONTNAME,		ID_GETSAVEDIR,		ID_FIRST,
	  ID_LAST,		ID_KERNTABLE,		ID_SAVEDIR,
	  ID_BLANK,		ID_PUBNAME,		ID_RESETKERN,
	  ID_MOVEFL,		ID_MOVEFR,		ID_MOVEFU,
	  ID_MOVEFD,		ID_CHARSIZE,		ID_CHARWIDE,
	  ID_CHARHIGH,		ID_CHOOSEFIXFONT,	ID_MAXWIN,
	  ID_CONFIRMQUIT,	ID_SHOWBASELINE,	ID_AUTOHEIGHT,
	  ID_INVERTSHIFT,	ID_PREVIEW,		ID_ALL,
	  ID_PREVIEWSTR,	ID_CLEAR,		ID_UPDATE,
	  ID_PREVIEWSCROLL,	ID_ZOOMIN,		ID_ZOOMOUT,
	  ID_PROJECTMENU,	ID_FONTMENU,		ID_CHARMENU,
	  ID_PREVIEWMENU,	ID_UNDO,		ID_CUT,
	  ID_ERASE,		ID_FLIPH,		ID_FLIPV,
	  ID_TOGGLE,		ID_CASCADE,		ID_EDITCHAR,
	  ID_EDITCHARNUM,	ID_QUERYWIDTH,		ID_QUERYWIDTHNUM };

enum scroll { GADG_HORIZ,
	      GADG_VERT,
	      GADG_LEFT,
	      GADG_RIGHT,
	      GADG_UP,
	      GADG_DOWN,
	      GADG_NONE };

#define GETBIT(buffer,offset) \
  ((((*((UBYTE *)(buffer)+((offset)>>3)))&(128>>((offset)&7))) != 0) \
  ? 1 : 0)

#ifdef __AROS__
struct charDef
{
#if AROS_BIG_ENDIAN
  WORD charOffset,charBitWidth;
#else
  WORD charBitWidth,charOffset;
#endif
};

#else
struct charDef
{
  WORD charOffset,charBitWidth;
};
#endif

struct CharNode;

void Setup(void);
void SetupScreen(void);
SAVEDS ASM ULONG WindowHook(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct IntuiMessage *, msg));
void OpenFontWnd(void);
void CloseFontWnd(void);
void Quit(void);
void QuitScreen(void);
void MsgLoop(void);
ULONG CheckObjMsg(struct Window *wnd, Object *obj);
void CheckOpenCharWin(ULONG pressed, ULONG shift);
void SharedMsgs(ULONG code,struct CharNode *node);
void LoadChosenFont(void);
void MyError(char *message);
LONG ShowReq(char *text,char *gadgets,...);
void ClrDisposeObject(Object **obj);
void ClrWindowClose(Object **obj,struct Window **wnd);
void SleepWindows(void);
void WakeWindows(void);
BOOL UnpackChar(struct Character *chr,struct TextFont *font,ULONG i);
void KernTables(ULONG to, ULONG from);
void ClearCurrentFont(void);
void OpenEditFontWnd(void);
void CloseEditFontWnd(BOOL obj);
SAVEDS ASM LONG TBCompareFunc(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct lvCompare *, lvc));
SAVEDS ASM LONG TBResourceFunc(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct lvResource *, lvr));
SAVEDS char *TBDisplayFunc(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct lvRender *, lvr));
void OpenPrefsWnd(void);
void ClosePrefsWnd(BOOL obj);
void OpenAssignWnd(void);
void CloseAssignWnd(BOOL obj);
void OpenEditCharWnd(void);
void CloseEditCharWnd(BOOL obj);
void OpenWidthWnd(struct CharNode *node);
void CloseWidthWnd(BOOL obj);
void OpenKernWnd(struct CharNode *node);
void CloseKernWnd(BOOL obj);
void OpenSaveWnd(void);
void CloseSaveWnd(void);
void OpenAboutWnd(void);
void PutPositive(struct Window *wnd,char *name,LONG value,LONG min,
  LONG *dest);
void NewHeights(void);
void NewWidths(void);
void ChangeCharWidth(struct Character *chr, ULONG width, ULONG newoffset,
  ULONG oldoffset);
void CheckWidthKern(struct Character *chr);
void SavePrefs(char *filename);
void LoadPrefs(void);
void FlushAllFonts(void);
void GetScreenRatio(struct Screen *scr);
void WriteFont(char *buffer, char *name, UWORD height);
char *GetPubName(BOOL screen);
void CopyPubName(void);
void WarnVisitors(void);
LONG SizeX(LONG x);
LONG SizeY(LONG y);
void CheckQuit(void);
void AddToolBarList(UWORD entry);
void CascadeWindows(void);

/* char.c */

#define DATA(i,j) *(chr->chr_Data+((j)*chr->chr_Width)+(i))

struct CharNode
{
  struct Node chln_Node;
  Object *chln_Object;
  struct Window *chln_Window;
  char *chln_Title;
  Object *chln_HorizGadg, *chln_VertGadg;
  Object *chln_LeftGadg, *chln_RightGadg;
  Object *chln_UpGadg, *chln_DownGadg;
  struct Hook *chln_Hook;
  Object *chln_EditGadg;
  struct Character *chln_Character;
  ULONG chln_Number;
  ULONG chln_MinPixelX, chln_MinPixelY;
  ULONG chln_CurPixelX, chln_CurPixelY;
  ULONG chln_ToolBarWidth, chln_ToolBarHeight;
  struct Character chln_UndoChar;
  Object *chln_ToolBar, *chln_WinGroup;
};

struct CharNode *OpenCharWin(ULONG charnum,struct IBox size);
void CloseCharWin(struct CharNode *node);
Object *CreateSysImage(ULONG which,struct DrawInfo *dri);
Object *CreatePropGadg(ULONG freedom,Tag tag1,...);
Object *CreateButtonGadg(Object *image,Tag tag1,...);
#ifdef __AROS__
AROS_UFP3(void, CharHook,
    AROS_UFPA(struct Hook *, hook, A0),
    AROS_UFPA(Object *, o, A2),
    AROS_UFPA(struct IntuiMessage *, msg, A1));
#else
SAVEDS ASM void CharHook(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct IntuiMessage *, msg));
#endif
void ForceResizeChar(struct CharNode *node);
void SetScrollers(struct CharNode *node);
void RedrawEdit(struct CharNode *node);
void MoveLeft(struct CharNode *node,struct Character *chr);
void MoveRight(struct CharNode *node,struct Character *chr);
void MoveUp(struct CharNode *node,struct Character *chr);
void MoveDown(struct CharNode *node,struct Character *chr);
void MoveAll(void (*MoveFunction)());
void RedrawAll(void);
void ZoomIn(struct CharNode *node);
void ZoomOut(struct CharNode *node);
void SetMinWindowSize(struct CharNode *node, ULONG oldx);
void CopyChar(struct CharNode *node,struct Character *copyto);
void PasteChar(struct CharNode *node,struct Character *pasteto);
void EraseChar(struct CharNode *node);
void SwapBytes(UBYTE *ptr,int i,int j);
void FlipHoriz(struct CharNode *node);
void FlipVert(struct CharNode *node);
void OpenResetKernWnd(void);
void CloseResetKernWnd(BOOL obj);
void ResetKerning(WORD space,WORD kern);

/* fontio.c */

void OpenPreviewWnd(void);
void ClosePreviewWnd(BOOL all);
void SetPreviewFont();
void PreviewMsgs(ULONG code);
void SetPreviewScroller();
void PreviewAll(void);
UBYTE *SaveFont(BOOL tables,BOOL preview);
ULONG SaveShowReq(char *text,char *gadgets,...);
void WriteCharData(struct charDef *cd, UBYTE *fontdataptr, ULONG dest,
  ULONG src, ULONG *off, ULONG mod);
void OpenQueryWidthWnd(void);
void CloseQueryWidthWnd(void);
BOOL LoadFont(struct TextAttr *font, ULONG width);

/* text.c */

void SetLabelKey(Object *win, Object *gadg, ULONG text);
void SetupMenus(struct NewMenu *menus);

/* node.c */

struct List *GetNewList(struct List **list);
struct Node *CreateNode(ULONG size,struct List *list);
void RemoveList(struct List **list,BOOL all,HOOKFUNC hook);

/* gadget.c */

#define CG_TAGBASE	(TAG_USER|0x100000)
#define CG_Pos		(CG_TAGBASE+0)
#define CG_Pressed	(CG_TAGBASE+1)
#define CG_SizeX	(CG_TAGBASE+2)
#define CG_SizeY	(CG_TAGBASE+3)
#define CG_Redraw	(CG_TAGBASE+4)
#define CG_Font		(CG_TAGBASE+5)
#define CG_ShiftDown	(CG_TAGBASE+6)

Class *InitCharGadgClass(void);
ULONG GetCharGadgHeight(UWORD height,struct TextFont *font);

#define EG_XOFFSET 4
#define EG_YOFFSET 2

#define EG_TAGBASE	(TAG_USER|0x100000)
#define EG_PixelX	(EG_TAGBASE+0)
#define EG_PixelY	(EG_TAGBASE+1)
#define EG_CharStruct	(EG_TAGBASE+2)
#define EG_Update	(EG_TAGBASE+3)
#define EG_Width	(EG_TAGBASE+4)
#define EG_Height	(EG_TAGBASE+5)
#define EG_XOffset	(EG_TAGBASE+6)
#define EG_YOffset	(EG_TAGBASE+7)
#define EG_CharNode	(EG_TAGBASE+8)
#define EG_PixelBorder	(EG_TAGBASE+9)
#define EG_ShowBaseline	(EG_TAGBASE+10)
#define EG_Baseline	(EG_TAGBASE+11)
#define EG_ChangePtr	(EG_TAGBASE+12)
#define EG_Toggle	(EG_TAGBASE+13)

Class *InitEditGadgClass(void);

#define SCRL_TAGBASE	(TAG_USER|0x100000)
#define SCRL_Right	(SCRL_TAGBASE+0)

Class *InitSlideGadgClass(void);

#define FL_TAGBASE	(TAG_USER|0x100000)
#define FL_AcceptDrop	(FL_TAGBASE+0)
#define FL_SortDrops	(FL_TAGBASE+1)

Class *InitFLClass(void);

/* error.c */

enum errors { ASLALLOC,
	      CREATEPORT,
	      LOCKSCREEN,
	      OPENSCREEN,
	      ALLOCVEC,
	      NEWWINDOW,
	      NEWFILE,
	      OPENWINDOW,
	      NEWPROP,
	      NEWBUTTON };

void ErrorCode(ULONG code);
char *ErrorText(ULONG code);
