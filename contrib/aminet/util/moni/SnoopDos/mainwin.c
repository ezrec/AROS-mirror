/*
 *              MAINWIN.C                                                                                       vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *              This module handles the main SnoopDos window, outputting new events
 *              to the window, menu options, etc.
 */             

#include "system.h"
#include "snoopdos.h"
#include "gui.h"

#if 0
#define DB(s)   KPrintF(s)
#else
#define DB(s)
#endif

/*
 *              In earlier versions, we used to lock layers whenever we rendered
 *              event output to the SnoopDos main window buffer. This avoided
 *              deadlocks with input.device. Now we use a different work around,
 *              but for the sake of documentation, we leave the original calls
 *              in (you never know when they might come in handy)
 *
 *              (The different workaround is to never monitor tasks that have
 *              locked layers on the SnoopDos screen/window).
 */
#if 0
#define LOCK_LAYERS     \
        (LockLayerInfo(&MainWindow->WScreen->LayerInfo),        \
         LockLayer(0, MainWindow->RPort->Layer))

#define UNLOCK_LAYERS \
        (UnlockLayer(MainWindow->RPort->Layer), \
         UnlockLayerInfo(&MainWindow->WScreen->LayerInfo))

#else
#define LOCK_LAYERS
#define UNLOCK_LAYERS
#endif


/*
 *              Interface to RawPutChar() in exec.library
 *              (It takes a single parameter in D0)
 */
#ifdef __SASC
#pragma libcall SysBase RawPutChar 204 001
#endif
#ifndef __AROS__
void RawPutChar(char);
#endif

/*
 *              These two are imported from SNOOPDOS.C
 */
extern char     Version[];
extern char SnoopDosTitle[];
extern char SnoopDosTitleKey[];

/*
 *              Global vars used by the main window
 */
struct Gadget           *MainGadList;           /* Main gadget list                     */
struct Menu                     *MainWinMenu;           /* Main window menu                     */
struct TextFont         *BufferFont;            /* Handle for buffer font       */
struct TextAttr         *CurMainGadgetFA;       /* Current gadget font attr     */
struct TextAttr         *CurMainBufferFA;       /* Current buffer font attr     */

struct RastPort         InvertRP;                       /* Used when sizing columns     */

int NumBufLines;                        /* Current number of lines in buffer    */
int ChangedSpacing;                     /* 1 = edited spacing since last redraw */
int ScrollDirection;            /* ID of current scroll gadget, or 0    */
int ScrollCount;                        /* Used for pacing INTUITICKS in scroll */
int RemovedGadgets;                     /* True if gadgets removed from window  */
int AwaitingResize;                     /* True if waiting for window resize    */

#define RESIZE_DONE             0       /* Resize is now complete                               */
#define RESIZE_MIDDLE   1       /* Resize happened in middle of buffer  */
#define RESIZE_BOTTOM   2       /* Resize happened at end of buffer             */

#define MOVECOL_FREE    0       /* Reposition column freely                             */
#define MOVECOL_FIXED   1       /* Reposition column fixed wrt RHS col  */

#define INVERT_HEADER   0       /* Invert only the header part of col   */
#define INVERT_FULLBOX  1       /* Invert entire column                 */

int     BoxLeft;                                /* Pixel offset of left edge of box             */
int     BoxTop;                                 /* Pixel offset of top edge of box              */
int     BoxWidth;                               /* Width of box in pixels                               */
int     BoxHeight;                              /* Height of box in pixels                              */
int     BoxHeaderLine;                  /* Baseline for box header text                 */
int     BoxInLeft;                              /* Pixel offset of inside top of box    */
int     BoxInTop;                               /* Pixel offset of inside left of box   */
int     BoxInWidth;                             /* Inside width of box                                  */
int     BoxInHeight;                    /* Inside height of box                                 */
int     BoxCharWidth;                   /* Width of a single character in box   */
int BoxCharHeight;                      /* Height of a single character in box  */
int     BoxCols;                                /* Number of columns of text in box             */
int     BoxRows;                                /* Number of rows of text in box                */
int     BoxSpacing;                             /* Height between font baselines in box */
int     BoxLowest;                              /* Bottom point of box scroll arrow             */
int     BoxBaseline;                    /* Baseline of text line 0 in box               */
int     VBorderGap;                             /* Aspect-adjusted space betw hrz lines */
int     LogButtonLeft;                  /* Left (X) Position of Logfile button  */
int LogButtonID;                        /* Gadget ID of log button on view              */

int     LeftCol;                                /* Leftmost displayed column                    */
int     RightCol;                               /* Rightmost displayed column                   */

int DraggingRow;                        /* 1 if dragging a row with mouse               */
int SelectRow;                          /* Row to draw highlighted                              */
int DraggingColumn;                     /* 1 if dragging a column with mouse    */
int ClickStartCol;                      /* Col pos where mouse first clicked    */
int ClickCurCol;                        /* Current column where mouse clicked   */
int SelectStartCol;                     /* Start position of selected column    */
int DragOrigColWidth;           /* Starting width of widening column    */
int NextOrigColWidth;           /* Starting width of following column   */
int OrigLeftCol;                        /* Starting position of left column             */
int OrigBufWidth;                       /* Starting width of buffer                             */
int MovedColumn;                        /* True if column position was altered  */

EventFormat *DragEvent;         /* Points to column being widened               */
EventFormat *SelectEvent;       /* Points to column being highlighted   */

BPTR   LogFile;                         /* Handle of output log file                    */
UBYTE *LogBuffer;                       /* Buffer for buffered i/o (or NULL)    */
ULONG  LogBufferSize;           /* Size of logfile output buffer                */
ULONG  LogBufferPos;            /* Current position in output buffer    */

UBYTE  MainKeyboard[KB_SHORTCUT_SIZE];  /* For keyboard equivs          */

/*
 *              This table is used to select the appropriate row qualifier
 *              for the row selection mechanism
 */
UWORD RowQualTable[] = {
        0,                              /* ROWQUAL_ANY   -- ignore qualifiers                           */
        0,                              /* ROWQUAL_NONE  -- don't allow any qualifiers          */
        IE_SHIFT,               /* ROWQUAL_SHIFT -- only select if SHIFT is pressed     */
        IE_ALT,                 /* ROWQUAL_ALT   -- only select if ALT is pressed       */
        IE_CTRL,                /* ROWQUAL_CTRL  -- only select if CTRL is pressed      */
        IE_ALL                  /* ROWQUAL_ALL   -- only select if any qual pressed     */
};

/*
 *              Now our font structures.
 *
 *              We do everything in terms of FontAttr structures, and whenever a
 *              window needs a font, it OpenFont's it itself. This has several
 *              advantages: it allows each window to dynamically select the
 *              best-fitting font, and it also ensures no problems if we have a
 *              window open with one font and the user than changes the font
 *              to something else -- the first window will still have a valid
 *              lock on the old font.
 */
char   BufferLine[MAX_BUFFER_WIDTH];  /* Storage area for buffer lines */

struct TextAttr TopazFontAttr  = {"topaz.font",   8, FS_NORMAL, FPB_ROMFONT };
struct TextAttr SystemFontAttr = {SystemFontName, 0, FS_NORMAL, FPB_DISKFONT};
struct TextAttr WindowFontAttr = {WindowFontName, 0, FS_NORMAL, FPB_DISKFONT};
struct TextAttr BufferFontAttr = {BufferFontName, 0, FS_NORMAL, FPB_DISKFONT};

struct {
        struct TextAttr *gadgetfa;
        struct TextAttr *bufferfa;
} MainWindowFontList[] = {
	{&WindowFontAttr,        &BufferFontAttr},
	{&SystemFontAttr,        &BufferFontAttr},
	{&WindowFontAttr,        &SystemFontAttr},
	{&SystemFontAttr,        &SystemFontAttr},
	{&TopazFontAttr,         &SystemFontAttr},
	{&TopazFontAttr,         &TopazFontAttr},
	{NULL,                           NULL}
};

/*
 *              This is used by the boopsi proportional gadgets
 */
static struct TagItem RZ_MapTags[] = {
	{PGA_Top, ICSPECIAL_CODE},
	{TAG_END}
};

/*
 *              This holds the image pointers for our BOOPSI scroll gadgets
 */
struct Image            *ScrollImage[4];

/*
 *              This structure is used to define our four scroll gadgets. We
 *              define the relative positions of each gadget in terms of the
 *              formula (Scale * Width + offset) or (Scale * Height + offset)
 *              where Width and Height are taken from the image structure.
 */
struct ScrollData {
        int             gadgetid;                       /* ID of the gadget                                             */
        int             imagetype;                      /* Which type of image we're creating   */
        LONG    widthscale;                     /* Scale applied to width                               */
        LONG    widthoffset;            /* Offset applied to width                              */
        LONG    heightscale;            /* Scale applied to height                              */
        LONG    heightoffset;           /* Offset applied to offset                             */
        LONG    borderpos;                      /* Tag for border containing gadget             */
} ScrollArrows[] = {
	{GID_LEFTARROW,  LEFTIMAGE,      -2, -17,        -1, 1,          GA_BottomBorder},
	{GID_RIGHTARROW, RIGHTIMAGE,     -1, -17,        -1, 1,          GA_BottomBorder},
	{GID_UPARROW,    UPIMAGE,        -1, 1,          -2, -9,         GA_RightBorder},
	{GID_DOWNARROW,  DOWNIMAGE,      -1, 1,          -1, -9,         GA_RightBorder}
};
        
/*
 *              Now the gadgets for the main window
 */
struct MainGadgets {
        UWORD   gadgid;                 /* Gadget ID                                    */
        UWORD   stringid;               /* ID of label name             */
        UBYTE   col4;                   /* Column in 4 x 2 layout               */
        UBYTE   row4;                   /* Row in 4 x 2 layout                  */
        UBYTE   col6;                   /* Column in 4 x 2 layout               */
        UBYTE   row6;                   /* Row in 4 x 2 layout                  */
        UBYTE   col8;                   /* Column in 4 x 2 layout               */
        UBYTE   col12;                  /* Column in 12 x 1 layout              */
        UBYTE   widthtype;              /* 0=narrow, 1=wide, 2=status   */
};


struct MainGadgets MainGadgs[] = {
	{GID_HIDE,              MSG_HIDE_GAD,           0, 2,    0,     1,      0,      4,      MAIN_NARROW},
	{GID_QUIT,              MSG_QUIT_GAD,           1, 2,    1,     1,      1,      5,      MAIN_NARROW},
	{GID_PAUSE,             MSG_PAUSE_GAD,          0, 1,    2,     1,      2,      6,      MAIN_NARROW_TOGGLE},
	{GID_DISABLE,   MSG_DISABLE_GAD,        1, 1,    3,     1,      3,      7,      MAIN_NARROW_TOGGLE},
	{GID_OPENLOG,   MSG_OPENLOG_GAD,        2, 1,    4, 0,  4,      8,  MAIN_WIDE},
	{GID_APPENDLOG, MSG_APPENDLOG_GAD,      2, 1,    4,     0,      4,      8,      MAIN_WIDE_INVIS},
	{GID_STARTLOG,  MSG_STARTLOG_GAD,       2, 1,    4,     0,      4,      8,      MAIN_WIDE_INVIS},
	{GID_SERIALLOG, MSG_SERIALLOG_GAD,      2, 1,    4, 0,  4,  8,  MAIN_WIDE_INVIS},
	{GID_CLOSELOG,  MSG_CLOSELOG_GAD,       2, 1,    4, 0,  4,      8,  MAIN_WIDE_INVIS},
	{GID_SETUP,             MSG_SETUP_GAD,          3, 1,    5,     0,      6,      10,     MAIN_WIDE},
	{GID_SAVESET,   MSG_SAVESET_GAD,        2, 2,    4,     1,      5,      9,      MAIN_WIDE},
	{GID_FUNCTION,  MSG_FUNCTION_GAD,       3, 2,    5,     1,      7,      11,     MAIN_WIDE},
	{GID_STATUS,    MSG_STATUS_GAD,         0, 0,    0,     0,      0,      0,      MAIN_STATUS},
	{0, 0, 0, 0, 0, 0, 0, 0, 0}
};


/*
 *              Names of gadgets in main window for calculating minimum width
 */
int MainWidthLeftText[] = {
        MSG_PAUSE_GAD,
        MSG_DISABLE_GAD,
        MSG_HIDE_GAD,
        MSG_QUIT_GAD,
        0
};

int MainWidthRightText[] = {
        MSG_OPENLOG_GAD,
        MSG_APPENDLOG_GAD,
        MSG_STARTLOG_GAD,
        MSG_SERIALLOG_GAD,
        MSG_CLOSELOG_GAD,
        MSG_SAVESET_GAD,
        MSG_FUNCTION_GAD,
        MSG_SETUP_GAD,
        0
};


/*
 *              SnoopDos Menus
 *
 *              -- Warning -- 
 *
 *              If you re-arrange the order of menu items or sub-items, you must
 *              make sure to update the MENU_xxxx #define's at the end to reflect
 *              the new ordering!
 *
 *              (Remember to count bars when working out item numbers, and that
 *              menu/item numbers start from zero, not one.)
 */

#define DIS                     NM_ITEMDISABLED
#define TICK            (CHECKIT | MENUTOGGLE)
#define TICKED          (CHECKIT | MENUTOGGLE | CHECKED)
#define BAR                     NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL
#define MENU_END        NM_END,  NULL, NULL, 0, 0, NULL

#define M(type,title_id,flags,mid) \
        type,   (UBYTE *)(title_id), 0, flags, 0, (APTR)(mid)

#define MX(type,title_id,flags,ex,mid) \
{type,   (UBYTE *)(title_id), 0, flags, ex,(APTR)(mid)}

struct NewMenu MainMenu[] = {

        /*
         *              Project menu
         */
        M(      NM_TITLE,       MSG_PROJECT_MENU,                       0,              0),
        M(   NM_ITEM,   MSG_PROJECT_OPENLOG,            0,              MID_OPENLOG),
        M(       NM_ITEM,       MSG_PROJECT_CLOSELOG,           DIS,    MID_CLOSELOG),
             BAR,
        MX(   NM_ITEM,  MSG_PROJECT_PAUSE,                      TICK,   ~8,             MID_PAUSE),
        MX(   NM_ITEM,  MSG_PROJECT_DISABLE,            TICK,   ~16,    MID_DISABLE),
             BAR,
        M(    NM_ITEM,  MSG_PROJECT_STEP,           0,      MID_STEP),
             BAR,
        M(    NM_ITEM,  MSG_PROJECT_PRI,            0,      0),
        MX(    NM_SUB,  MSG_PROJECT_PRI_A,                      TICK,   ~1,             MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_B,                      TICK,   ~2,             MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_C,                      TICK,   ~4,             MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_D,                      TICK,   ~8,             MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_E,                      TICK,   ~16,    MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_F,                      TICK,   ~32,    MID_CHANGEPRI),
        MX(    NM_SUB,  MSG_PROJECT_PRI_G,                      TICK,   ~64,    MID_CHANGEPRI),
                 BAR,
        M(       NM_ITEM,       MSG_PROJECT_HELP,                       0,              MID_HELP),
        M(       NM_ITEM,       MSG_PROJECT_ABOUT,                      0,              MID_ABOUT),
             BAR,
        M(       NM_ITEM,       MSG_PROJECT_HIDE,                       0,              MID_HIDE),
             BAR,
        M(       NM_ITEM,       MSG_PROJECT_QUIT,                       0,              MID_QUIT),

        /*
         *              Windows menu
         */
        M(  NM_TITLE,   MSG_WINDOWS_MENU,                       0,              0),
        M(   NM_ITEM,   MSG_WINDOWS_SETUP,                      0,              MID_SETUP),
        M(   NM_ITEM,   MSG_WINDOWS_FUNCTION,           0,              MID_FUNCTION),
        M(   NM_ITEM,   MSG_WINDOWS_FORMAT,                     0,              MID_FORMAT),
             BAR,
        M(   NM_ITEM,   MSG_WINDOWS_WIDTH,                      0,      0),
        M(        NM_SUB,       MSG_WINDOWS_WIDTH_1,            0,              MID_SETWIDTH),
        M(        NM_SUB,       MSG_WINDOWS_WIDTH_2,            0,              MID_SETWIDTH),
        M(        NM_SUB,       MSG_WINDOWS_WIDTH_3,            0,              MID_SETWIDTH),
        M(        NM_SUB,       MSG_WINDOWS_WIDTH_4,            0,              MID_SETWIDTH),
        M(        NM_SUB,       MSG_WINDOWS_WIDTH_5,            0,              MID_SETWIDTH),
             BAR,
        M(       NM_ITEM,       MSG_WINDOWS_SPACING,            0,              0),
        MX(       NM_SUB,       MSG_WINDOWS_SPACING_NONE,       TICKED, ~1,     MID_SPACE_NONE),
        MX(       NM_SUB,       MSG_WINDOWS_SPACING_1P,         TICK,   ~2,     MID_SPACE_1P),
        MX(       NM_SUB,       MSG_WINDOWS_SPACING_2P,         TICK,   ~4,     MID_SPACE_2P),

        M(       NM_ITEM,       MSG_WINDOWS_REFRESH,            0,              0),
        MX(   NM_SUB,   MSG_WINDOWS_REF_SIMPLE,         TICK,   ~1, MID_REF_SIMPLE),
        MX(   NM_SUB,   MSG_WINDOWS_REF_SMART,          TICKED, ~2, MID_REF_SMART),

        M(   NM_ITEM,   MSG_WINDOWS_ALIGN,                      0,              0),
        MX(       NM_SUB,       MSG_WINDOWS_ALIGN_LEFT,         TICKED, ~1, MID_ALIGN_LEFT),
        MX(       NM_SUB,       MSG_WINDOWS_ALIGN_RIGHT,        TICK,   ~2, MID_ALIGN_RIGHT),

        M(   NM_ITEM,   MSG_WINDOWS_ROW_QUAL,           0,      0),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_ANY,            TICKED, ~1,  MID_ROWQUAL),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_NONE,           TICK,   ~2,  MID_ROWQUAL),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_SHIFT,          TICK,   ~4,  MID_ROWQUAL),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_ALT,            TICK,   ~8,  MID_ROWQUAL),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_CTRL,           TICK,   ~16, MID_ROWQUAL),
        MX(       NM_SUB,       MSG_WINDOWS_ROW_ALL,            TICK,   ~32, MID_ROWQUAL),

             BAR,
        M(       NM_ITEM,       MSG_WINDOWS_SHOWSTATUS,         TICKED, MID_STATUS),
        M(   NM_ITEM,   MSG_WINDOWS_SHOWGADGETS,    TICKED, MID_GADGETS),
             BAR,
        M(       NM_ITEM,       MSG_WINDOWS_AUTO_OPEN,          TICK,   MID_AUTO_OPEN),
        M(   NM_ITEM,   MSG_WINDOWS_DISABLE_HIDDEN, TICK,   MID_DISABLE_HIDDEN),

        /*
         *              Settings menu
         */
        M(      NM_TITLE,       MSG_SETTINGS_MENU,                      0,              0),
        M(       NM_ITEM,       MSG_SETTINGS_LOAD,                      0,              MID_LOAD),
        M(       NM_ITEM,       MSG_SETTINGS_SAVE,                      0,              MID_SAVE),
        M(       NM_ITEM,       MSG_SETTINGS_SAVEAS,            0,              MID_SAVEAS),
             BAR,
        M(       NM_ITEM,       MSG_SETTINGS_RESETDEFAULTS,     0,              MID_RESET),
        M(       NM_ITEM,       MSG_SETTINGS_LASTSAVED,         0,              MID_LASTSAVED),
        M(       NM_ITEM,       MSG_SETTINGS_RESTORE,           0,              MID_RESTORE),
             BAR,
        M(       NM_ITEM,       MSG_SETTINGS_CREATEICONS,       TICK,   MID_ICONS),

        /*
         *              Buffer menu
         */
        M(      NM_TITLE,       MSG_BUFFER_MENU,                        0,              0),
        M(       NM_ITEM,       MSG_BUFFER_COPYWIN,                     0,              MID_COPYWIN),
        M(       NM_ITEM,       MSG_BUFFER_COPYBUF,                     0,              MID_COPYBUF),
             BAR,
        M(   NM_ITEM,   MSG_BUFFER_SAVEWIN,                     0,              MID_SAVEWIN),
        M(       NM_ITEM,       MSG_BUFFER_SAVEBUF,                     0,              MID_SAVEBUF),
             BAR,
        M(       NM_ITEM,       MSG_BUFFER_CLEARBUF,            0,              MID_CLEARBUF),

        MENU_END
};

/*
 *              These defines are used when adjusting the menu settings on the fly
 */

#define MENU_OPENLOG            FULLMENUNUM(0,  0, NOSUB)
#define MENU_CLOSELOG           FULLMENUNUM(0,  1, NOSUB)
#define MENU_PAUSE                      FULLMENUNUM(0,  3, NOSUB)
#define MENU_DISABLE            FULLMENUNUM(0,  4, NOSUB)
#define MENU_CHANGEPRI          FULLMENUNUM(0,  8, 0)
#define MENU_NUMPRI                     7                               /* Number of menu priorities */
#define MENU_HIDE                       FULLMENUNUM(0,  13, NOSUB)

#define MENU_SPACING0           FULLMENUNUM(1,  6, 0)
#define MENU_SPACING1           FULLMENUNUM(1,  6, 1)
#define MENU_SPACING2           FULLMENUNUM(1,  6, 2)
#define MENU_SIMPLE                     FULLMENUNUM(1,  7, 0)
#define MENU_SMART                      FULLMENUNUM(1,  7, 1)
#define MENU_AL_LEFT            FULLMENUNUM(1,  8, 0)
#define MENU_AL_RIGHT           FULLMENUNUM(1,  8, 1)
#define MENU_ROWQUAL_ANY        FULLMENUNUM(1,  9, 0)
#define MENU_ROWQUAL_NONE       FULLMENUNUM(1,  9, 1)
#define MENU_ROWQUAL_SHIFT      FULLMENUNUM(1,  9, 2)
#define MENU_ROWQUAL_ALT        FULLMENUNUM(1,  9, 3)
#define MENU_ROWQUAL_CTRL       FULLMENUNUM(1,  9, 4)
#define MENU_ROWQUAL_ALL        FULLMENUNUM(1,  9, 5)
#define MENU_STATUS                     FULLMENUNUM(1, 11, NOSUB)
#define MENU_GADGETS            FULLMENUNUM(1, 12, NOSUB)
#define MENU_AUTO_OPEN          FULLMENUNUM(1, 14, NOSUB)
#define MENU_DIS_HIDDEN         FULLMENUNUM(1, 15, NOSUB)

#define MENU_ICONS                      FULLMENUNUM(2,  8, NOSUB)

/*****************************************************************************
 *
 *              Start of functions!
 *
 *****************************************************************************/

/*
 *              GetTimeStr(datestamp)
 *
 *              Returns a pointer to a string giving the current time in
 *              hours, mins, and seconds. The string remains valid until
 *              the next time this function is called.
 *
 *              If the datestamp parameter is NULL, then the current date is
 *              used instead.
 */
char *GetTimeStr(struct DateStamp *ds)
{
        static char timestr[20];
        struct DateStamp ourds;
        struct DateTime datetime;

        if (!ds) {
                ds = &ourds;
                DateStamp(ds);
        }
        datetime.dat_Stamp       = *ds;
        datetime.dat_Format      = FORMAT_DOS;
        datetime.dat_Flags       = 0;
        datetime.dat_StrDay      = NULL;
        datetime.dat_StrDate = NULL;
        datetime.dat_StrTime = timestr;
        DateToStr(&datetime);
        return (timestr);
}

/*
 *              UpdateStatus(void)
 *
 *              Determines what the current status is (by examining various
 *              variables) and sets the status line to display an appropriate
 *              message. Call whenever the status may have changed.
 *
 *              Things which can cause a status change: pause/unpause, disable/enable,
 *              open log/close log
 */
void UpdateStatus(void)
{
        if (Paused || Disabled) {
                char *timestr;
                char *merid;
                char *msg;
                int hour;
                int min;

                if (Paused) {
                        timestr = GetTimeStr(&PauseDateStamp);
                        msg             = MSG(MSG_STATUS_PAUSED);
                } else {
                        timestr = GetTimeStr(&DisableDateStamp);
                        msg             = MSG(MSG_STATUS_DISABLED);
                }
                hour     = atoi(timestr);
                min      = atoi(timestr+3);
                merid    = "AM";
                if (hour >= 12) {
                        hour -= 12;
                        merid = "PM";
                }
                mysprintf(StatusLineText, msg, (hour ? hour : 12), min, merid);
        } else if (LogActive) {
                int msgid;

                switch (CurrentLogType) {
                        case LT_FILE:                   msgid = MSG_STATUS_LOGFILE;             break;
                        case LT_DEVICE:                 msgid = MSG_STATUS_LOGDEVICE;   break;
                        case LT_DEBUG:                  msgid = MSG_STATUS_LOGDEBUG;    break;
                }
                mysprintf(StatusLineText, MSG(msgid), CurrentLogName);
        } else
                strcpy(StatusLineText, MSG(MSG_STATUS_NORMAL));
        
        if (MainWindow) {
                GT_SetGadgetAttrs(Gadget[GID_STATUS], MainWindow, NULL,
                                                  GTTX_Text, StatusLineText,
                                                  TAG_DONE);
        }
}

/*
 *              SetLogGadget(logmode, refresh)
 *
 *              Displays the appropriate gadget in the main window according to the
 *              type of logging method chosen. If the logfile is currently open,
 *              the "close log" gadget is displayed, otherwise the correct "open"
 *              gadget is displayed according to the logmode: LOGMODE_PROMPT,
 *              LOGMODE_APPEND, LOGMODE_OVERWRITE, or LOGMODE_SERIALPORT
 *
 *              If the refresh parameter is LG_REFRESH, then refresh gadgets.
 *              LG_NOREFRESH means don't refresh gadgets (i.e. assume we're being
 *              called from the gadget creation routine).
 */
void SetLogGadget(int logmode, int refresh)
{
        static int loggadgets[] = {
                GID_OPENLOG, GID_APPENDLOG, GID_STARTLOG, GID_SERIALLOG,
                GID_CLOSELOG, 0
        };
        int gadid;
        int i;

        if (refresh == LG_REFRESH && !MainWindow)
                return;

        switch (logmode) {
                case LOGMODE_PROMPT:            gadid = GID_OPENLOG;            break;
                case LOGMODE_APPEND:            gadid = GID_APPENDLOG;          break;
                case LOGMODE_SERIALPORT:        gadid = GID_SERIALLOG;          break;
                case LOGMODE_OVERWRITE:         gadid = GID_STARTLOG;           break;
        }
        if (LogActive)
                gadid = GID_CLOSELOG;

        for (i = 0; loggadgets[i]; i++) {
                struct Gadget *gad = Gadget[loggadgets[i]];
                int gadpos;
                
                if (refresh == LG_REFRESH)
                         gadpos = RemoveGadget(MainWindow, gad);

                if (loggadgets[i] == gadid)
                        gad->LeftEdge = LogButtonLeft;
                else
                        gad->LeftEdge = INVIS_LEFT_EDGE;

                if (refresh == LG_REFRESH) {
                        AddGadget(MainWindow, gad, gadpos);
                        RefreshGList(gad, MainWindow, NULL, 1);
                }
        }
        LogButtonID = gadid;
}

/*
 *              WriteLog(string)
 *
 *              Writes the specified string to the logfile, handling buffering
 *              etc. as required.
 *
 *              If string is NULL, then flushes any buffers as necessary.
 */
void WriteLog(char *string)
{
        int len;

        if (!LogActive)
                return;
        
        if (string == NULL) {
                /*
                 *              Flush output
                 */
                if (LogFile && LogBuffer && LogBufferPos) {
                        Write(LogFile, LogBuffer, LogBufferPos);
                        LogBufferPos = 0;
                }
                return;
        }

        if (CurrentLogType == LT_DEBUG) {
                /*
                 *              Output string to serial terminal, expanding LF's to CR/LF's
                 *              as we go.
                 */
                while (*string) {
                        if (*string == '\n')
                                RawPutChar('\r');
                        RawPutChar(*string++);
                }
                return;
        }
        len = strlen(string);
        if (LogBuffer) {
                /*
                 *              Buffered i/o so add string to buffer; if necessary, flush
                 *              buffer first.
                 */
                if ((LogBufferPos + len) > LogBufferSize) {
                        Write(LogFile, LogBuffer, LogBufferPos);
                        LogBufferPos = 0;
                }
                memcpy(LogBuffer + LogBufferPos, string, len);
                LogBufferPos += len;
        } else {
                /*
                 *              Plain unbuffered i/o
                 */
                Write(LogFile, string, strlen(string));
        }
}

/*
 *              CloseLog()
 *
 *              Closes the currently open logfile (if any)
 */
void CloseLog(void)
{
        char msg[100];

        if (!LogActive)
                return;
        
        mysprintf(msg, MSG(MSG_LOG_STOP), GetTimeStr(NULL));
        WriteLog(msg);
        WriteLog(NULL); /* Flush output */
        LogActive = 0;
        SetLogGadget(DefaultLogMode, LG_REFRESH);
        UpdateStatus();
        SetMenuOptions();
        if (LogFile) {
                int oldpaused = Paused;

                Paused = 0;
                Close(LogFile);
                LogFile = NULL;
                Paused = oldpaused;
        }
        if (LogBuffer) {
                FreeMem(LogBuffer, LogBufferSize);
                LogBuffer = NULL;
        }
}

/*
 *              WriteLogHeader()
 *
 *              Writes the standard SnoopDos header line to the logfile using the
 *              current logfile format
 */
void WriteLogHeader(void)
{
        FormatEvent(LogEFormat, NULL, BufferLine+1, 0, LogWidth - 1);
        BufferLine[0]              = ' ';
        BufferLine[LogWidth+1] = '\n';
        BufferLine[LogWidth+2] = 0;
        WriteLog(BufferLine);
        UnderlineTitles(LogEFormat, BufferLine+1, '-');
        BufferLine[LogWidth+1] = '\n';
        WriteLog(BufferLine);
}

/*
 *              OpenLog(mode, filename)
 *
 *              Opens the named file for logging. If a log file is already open,
 *              then closes the existing file first.
 *
 *              Mode is set to:
 *
 *                      LOGMODE_PROMPT     - Prompt user if file exits for append/overwrite
 *                      LOGMODE_APPEND     - Automatically append to file if it exists
 *                      LOGMODE_OVERWRITE  - Automatically overwrite file it if exitsts
 *                      LOGMODE_SERIALPORT - Direct output to debugger on serial port
 *
 *              Filename is ignored if mode is set to LOGMODE_SERIALPORT.
 *
 *              Returns 1 for success, 0 for failure (couldn't open file?)
 */
int OpenLog(int logmode, char *filename)
{
        APTR oldwinptr = *TaskWindowPtr;
        char startmsg[100];
        char timestr[20];
        char datestr[20];
        char weekday[20];
        struct DateTime datetime;
        int pausestate = Paused;

        Paused = 0;
        if (LogActive)
                CloseLog();
        
        if (logmode == LOGMODE_SERIALPORT) {
                CurrentLogType = LT_DEBUG;
        } else {
                int filemode;
                int buffer = 0;

                strcpy(CurrentLogName, filename);
                *TaskWindowPtr = (APTR)-1;
                if (IsFileSystem(filename))
                        CurrentLogType = LT_FILE;
                else
                        CurrentLogType = LT_DEVICE;
                *TaskWindowPtr = oldwinptr;

                if (logmode == LOGMODE_PROMPT && CurrentLogType == LT_FILE) {
                        BPTR lock = Lock(filename, ACCESS_READ);

                        if (lock) {
                                UnLock(lock);
                                switch (GetResponse(MSG(MSG_REQ_APPEND_OVERWRITE_CANCEL),
                                                                        MSG(MSG_REQ_FILE_EXISTS), filename)) {

                                        case 1:         filemode = MODE_READWRITE;              break;
                                        case 2:         filemode = MODE_NEWFILE;                break;
                                        default:        goto open_okay;
                                }
                        } else
                                filemode = MODE_NEWFILE;
                } else if (logmode == LOGMODE_APPEND)
                        filemode = MODE_READWRITE;
                else
                        filemode = MODE_NEWFILE;

                LogFile = Open(filename, filemode);
                if (!LogFile)
                        goto open_failed;

                if (filemode == MODE_READWRITE) {
                        Seek(LogFile, 0, OFFSET_END);
                        Write(LogFile, "\n", 1); /* Leave space between multiple logs */
                }

                /*
                 *              Now select buffer type for file according to gadget and
                 *              file type.
                 */
                switch (CurSettings.Setup.FileIOType) {
                        case FILE_AUTOMATIC:
                                if (CurrentLogType == LT_FILE)
                                        buffer = 1;
                                break;

                        case FILE_IMMEDIATE:            buffer = 0;     break;
                        case FILE_BUFFERED:                     buffer = 1; break;
                }
                if (buffer) {
                        /*
                         *              Allocate a buffer for file i/o. Doesn't matter if
                         *              it fails -- we just drop back to unbuffered file
                         *              i/o instead.
                         */
                        LogBufferSize = LOGBUFFER_SIZE;
                        LogBufferPos  = 0;
                        LogBuffer     = AllocMem(LogBufferSize, MEMF_ANY);
                }
        }
        LogActive = 1;
        SetLogGadget(DefaultLogMode, LG_REFRESH);
        UpdateStatus();
        SetMenuOptions();

        DateStamp(&datetime.dat_Stamp);
        datetime.dat_Format      = FORMAT_DOS;
        datetime.dat_Flags       = 0;
        datetime.dat_StrDay      = weekday;
        datetime.dat_StrDate = datestr;
        datetime.dat_StrTime = timestr;
        DateToStr(&datetime);
        mysprintf(startmsg, MSG(MSG_LOG_START), weekday, datestr, timestr);

        /*
         *              Now select appropriate output format for log
         */
        LogWidth = ParseFormatString(LogFormat, LogEFormat, MAX_FORM_LEN);
        if (!LogWidth)
                LogWidth = ParseFormatString(BufFormat, LogEFormat, MAX_FORM_LEN);
        WriteLog(startmsg);
        WriteLogHeader();
open_okay:
        Paused = pausestate;
        return (1);

open_failed:
        Paused = pausestate;
        return (0);
}

/*
 *              SaveBuffer(savetype, savename, overwrite)
 *
 *              This function saves the contents of some or all of the current
 *              buffer to a disk file or to the clipboard.
 *
 *              savetype is SAVEBUF_WINDOW to save only that portion of the
 *              buffer that is currently displayed in the main window, or
 *              SAVEBUF_ALL to save the entire buffer.
 *
 *              savename is a pointer to a disk filename, or SAVEBUF_CLIPBOARD to
 *              save to the clipboard.device.
 *
 *              overwrite is SAVEBUF_OVERWRITE to force an existing file to be
 *              overwritten automatically or SAVEBUF_PROMPT to let the user
 *              choose whether to overwrite using a requester. If the user
 *              chooses not to overwrite, then success is returned.
 *
 *              Returns 1 for success or 0 for failure.
 */
int SaveBuffer(int savetype, char *savename, int overwrite)
{
        struct IOClipReq *clipreq;
        struct MsgPort   *clipport;
        BPTR  savefile;
        UBYTE *savebuf;
        Event *curev;
        ULONG curpos;
        int startseq;
        int curseq;
        int endseq;
        int leftcol;
        int rightcol;
        int totalsize;
        int width;
        int numlines;

        if (savetype == SAVEBUF_WINDOW) {
                /*
                 *              Save only visible portion of buffer
                 */
                startseq        = TopSeq;
                curev           = TopEvent;
                endseq          = BottomSeq;
                leftcol         = LeftCol;
                rightcol        = RightCol;
        } else {
                /*
                 *              Save entire buffer
                 */
                startseq        = RealFirstSeq;
                curev           = HeadNode(&EventList);
                endseq          = EndSeq;
                leftcol         = 0;
                rightcol        = BufferWidth - 1;
        }

        savebuf = AllocMem(SAVEBUFFER_SIZE, MEMF_ANY);
        if (!savebuf)
                return (0);

        curpos = 0;

        /*
         *              Now try and open our output device
         */
        if (savename == SAVEBUF_CLIPBOARD) {
                /*
                 *              Open clipboard
                 */
                clipport = CreateMsgPort();
                if (!clipport) {
                        FreeMem(savebuf, SAVEBUFFER_SIZE);
                        return (0);
                }
                clipreq = (struct IOClipReq *)CreateIORequest(clipport, sizeof(*clipreq));
                if (!clipreq) {
                        FreeMem(savebuf, SAVEBUFFER_SIZE);
                        DeleteMsgPort(clipport);
                        return (0);
                }
                if (OpenDevice("clipboard.device", 0, (void *)clipreq, 0)) {
                        FreeMem(savebuf, SAVEBUFFER_SIZE);
                        DeleteIORequest((struct IORequest *)clipreq);
                        DeleteMsgPort(clipport);
                        return (0);
                }
                clipreq->io_Error       = 0;
                clipreq->io_Offset      = 0;
                clipreq->io_ClipID      = 0;
        } else {
                /*
                 *              Open log file
                 */
                int filemode = MODE_NEWFILE;;

                if (overwrite == SAVEBUF_PROMPT) {
                        BPTR lk = Lock(savename, ACCESS_READ);

                        if (lk) {
                                UnLock(lk);
                                switch (GetResponse(MSG(MSG_REQ_APPEND_OVERWRITE_CANCEL),
                                                                    MSG(MSG_REQ_FILE_EXISTS), savename)) {

                                        case 1:         filemode = MODE_READWRITE;              break;
                                        case 2:         filemode = MODE_NEWFILE;                break;
                                        default:        FreeMem(savebuf, SAVEBUFFER_SIZE);
                                                                return (1);
                                }
                        }
                }
                savefile = Open(savename, filemode);
                if (!savefile) {
                        FreeMem(savebuf, SAVEBUFFER_SIZE);
                        return (0);
                }
                if (filemode == MODE_READWRITE) {
                        Seek(savefile, 0, OFFSET_END);
                        Write(savefile, "\n", 1);       /* Leave blank line between appends */
                }
        }

        /*
         *              Now calculate how many lines there are to be saved in the
         *              buffer. There will be as many lines as were selected from
         *              startseq to endseq (inclusive) plus two additional lines for
         *              the header and underline. Each line will have (rightcol-leftcol+1)
         *              characters on it, plus one additional character for the newline.
         *              We need to calculate all this in advance because the header
         *              written for the clipboard includes the number of characters of
         *              text to be written.
         */
        width = (rightcol - leftcol + 1);

        if (IsListEmpty(&EventList))
                numlines = 2;
        else
                numlines = (endseq - startseq + 1) + 2;

        totalsize = (width + 1) * numlines;

        if (savename == SAVEBUF_CLIPBOARD) {
                /*
                 *              Write the clipboard FTXT header into the buffer. We
                 *              need to write the amount of text, but we also need
                 *              to write the total size of the clip (including header)
                 *              rounded to a word boundary.
                 */
                int cliplen = (totalsize + 13) & ~1;    /* Includes header */

                memcpy(savebuf, "FORM....FTXTCHRS....", 20);
                *((ULONG *)&savebuf[4])  = cliplen;
                *((ULONG *)&savebuf[16]) = totalsize;
                curpos += 20;
        }
        /*
         *              Next, output the two header lines (the column headings plus
         *              the underline)
         */
        FormatEvent(BufferEFormat, NULL, savebuf + curpos, leftcol, rightcol);
        curpos += width;
        savebuf[curpos++] = '\n';

        UnderlineTitles(BufferEFormat, BufferLine, '-');
        memcpy(savebuf + curpos, BufferLine + leftcol, width);
        curpos += width;
        savebuf[curpos++] = '\n';

        /*
         *              Now walk through the lines in the buffer, outputting each line
         *              one at a time.
         */
        curseq     = startseq;
        totalsize -= (width + width + 2);       /* Allow for header */

        while (totalsize > 0) {
                /*
                 *              First check if we have enough room in the buffer for
                 *              the next line -- if not, then flush the buffer
                 */
                if ((curpos + width + 2) >= SAVEBUFFER_SIZE) {
                        if (savename == SAVEBUF_CLIPBOARD) {
                                clipreq->io_Command = CMD_WRITE;
                                clipreq->io_Data        = savebuf;
                                clipreq->io_Length      = curpos;
                                DoIO((struct IORequest *)clipreq);
                        } else
                                Write(savefile, savebuf, curpos);
                        curpos = 0;
                }
                /*
                 *              The next section must be completely enclosed in
                 *              ObtainSemaphore/ReleaseSemaphore to ensure the
                 *              buffer doesn't change under our feet
                 */
                ObtainSemaphore(&BufSem);
                if (curseq < RealFirstSeq) {
                        /*
                         *              Uhoh -- the list has scrolled past us while we
                         *              were outputting it; catch up as best we can.
                         */
                        curev = HeadNode(&EventList);
                        if (NextNode(curev))
                                 curseq = curev->seqnum;
                        else
                                 curseq = endseq + 1;
                }
                if (curseq > endseq) {
                        /*
                         *              We've somehow run out of lines -- this should never
                         *              happen, but you never know. For disk files, we can
                         *              just avoid outputting anything else. For the
                         *              clipboard, we need to output something, so we just
                         *              output a line of spaces.
                         */
                        if (savename != SAVEBUF_CLIPBOARD)
                                break;
                        memset(savebuf + curpos, ' ', width);
                } else {
                        FormatEvent(BufferEFormat, curev, savebuf + curpos,
                                                leftcol, rightcol);
                        curev = NextNode(curev);
                        curseq++;
                }
                curpos += width;
                savebuf[curpos++] = '\n';
                totalsize -= (width + 1);
                ReleaseSemaphore(&BufSem);
        }

        /*
         *              Now we've output the entire save region, so just flush anything
         *              left in the save buffer and we're done.
         */
        if (savename == SAVEBUF_CLIPBOARD) {
                /*
                 *              Make sure out total output is word-aligned, otherwise
                 *              Bad Things will happen
                 */
                if (totalsize & 1)
                        savebuf[curpos++] = '\0';

                if (curpos > 0) {
                        clipreq->io_Command = CMD_WRITE;
                        clipreq->io_Data        = savebuf;
                        clipreq->io_Length      = curpos;
                        DoIO((struct IORequest *)clipreq);
                }
                clipreq->io_Command = CMD_UPDATE;
                DoIO((struct IORequest *)clipreq); /* Make data publically available */

                CloseDevice((struct IORequest *)clipreq);
                DeleteIORequest((struct IORequest *)clipreq);
                DeleteMsgPort(clipport);
        } else {
                if (curpos)
                        Write(savefile, savebuf, curpos);
                Close(savefile);
        }
        FreeMem(savebuf, SAVEBUFFER_SIZE);
        return (1);
}

/*
 *              SetMonitorMode(type)
 *
 *              Sets the current monitor mode to the specified type. This will
 *              be one of the following three:
 *
 *                      MONITOR_NORMAL
 *                      MONITOR_PAUSED
 *                      MONITOR_DISABLED
 *
 *              Automatically takes care of updating the gadget imagery and
 *              menu options to reflect the mode. Also ignores attempts to
 *              Pause() if the main window isn't currently open. Note that
 *              the three modes are mutually exclusive.
 *
 *              The status line is also updated to reflect the current mode.
 *              The global MonitorType reflects the currently selected mode
 *              after this call.
 *
 *              Important: any call which may possible spawn a subtask
 *              (e.g. showing an ASL file requester etc.) must force the Paused
 *              variable to 0 before hand and then restore it to its original
 *              setting afterwards. This ensures that any other tasks that were
 *              already paused remain paused, but any new tasks that make calls
 *              during the activity will not be paused.
 *
 *              This isn't perfect, but it gives about 99% certainty of avoiding
 *              deadlocks, while still ensuring that any existing paused tasks
 *              (normally just one or two) remain paused. Thus, if someone has
 *              paused some stuff so they can open a log file, turn on the
 *              printer, or whatever, it will remain paused. (The possibility of
 *              deadlock comes about if our external file i/o somehow causes
 *              another process to make a call to one of the monitored SnoopDos
 *              functions, e.g. OpenLibrary or OpenDevice).
 *
 *              The alternative is to completely disable pausing on such calls, in
 *              which case vast quantities of output might be generated while the
 *              person is messing around in the file requester oblivious.
 *
 *              NOTE: It is impossible to enter PAUSED mode unless the SnoopDos
 *              window is open -- this is to prevent situations where you freeze
 *              the system (e.g. it's waiting for you to unpause, but you can't
 *              because you can't call up the main window due to something else.)
 */
void SetMonitorMode(int modetype)
{
        int paused   = 0;
        int disabled = 0;
        int changed  = 0;

        if (!MainWindow && modetype == MONITOR_PAUSED)
                return;

        if (modetype == MONITOR_DISABLED)
                disabled = 1;
        else if (modetype == MONITOR_PAUSED)
                paused = 1;

        /*
         *              Now update main window gadgets if necessary
         */
        if (MainWindow) {
                if (paused != Paused) {
                        ShowGadget(MainWindow, Gadget[GID_PAUSE],
                                           (paused ? GADGET_DOWN : GADGET_UP));
                }
                if (disabled != Disabled) {
                        ShowGadget(MainWindow, Gadget[GID_DISABLE],
                                           (disabled ? GADGET_DOWN : GADGET_UP));
                }
        }

        /*
         *              Next,  make the changes actually take effect.
         */
        if (paused != Paused) {
                DateStamp(&PauseDateStamp);
                if (paused)
                        ObtainSemaphore(&PauseSem);
                else
                        ReleaseSemaphore(&PauseSem);
                Paused  = paused;
                changed = 1;
        }
        if (disabled != Disabled) {
                DateStamp(&DisableDateStamp);
                Disabled = disabled;
                changed  = 1;
                LoadFuncSettings(&CurSettings.Func);    /* Update patches state */

                if (LogActive) {
                        char msg[100];

                        mysprintf(msg, MSG(Disabled ? MSG_LOG_DISABLED : MSG_LOG_ENABLED),
                                                   GetTimeStr(NULL));
                        WriteLog(msg);
                        WriteLog(NULL); /* Flush output to keep everything up-to-date */
                }
        }
        if (changed) {
                if (LogActive)          /* Flush log file when Paused */
                        WriteLog(NULL);
                UpdateStatus();
                SetMenuOptions();
        }
        MonitorType = modetype;
}

/*
 *              SingleStep()
 *
 *              Undoes Pause just long enough for all currently waiting tasks
 *              to execute a single monitored event. Usually, this will just
 *              produce a single event in the event buffer.
 *
 *              Why would you want to do this? Because it's a real handy way
 *              of single-stepping through a program's actions at startup,
 *              especially if it happens to be crashing after doing a particular
 *              action!
 *
 *              We implement the singlestep by freeing up the Pause semaphore
 *              and then immediately trying to recapture it. We rely on the
 *              fact that Exec schedules semaphore operations on a strictly
 *              first-come first-served basis, so as soon as we free it,
 *              all the waiting tasks are guaranteed to run before we can
 *              recapture it. We move ourselves to a high priority temporarily
 *              to ensure that nobody gets a chance to run two events before
 *              we can regain control again.
 *
 *              If we weren't in pause mode before, we will be when we exit.
 *
 */
void SingleStep(void)
{
        int oldpri;

        if (!MainWindow)
                return;

        if (!Paused)
                SetMonitorMode(MONITOR_PAUSED);
        
        WriteLog(NULL);         /* Make sure logfile is up to date! */

        /*
         *              To ensure that we get the semaphore back as soon as we 
         *              possibly can (i.e. let all the waiting tasks run but nobody
         *              else), we bump our priority up temporarily
         */
        oldpri = SetTaskPri(SysBase->ThisTask, 25);
        ReleaseSemaphore(&PauseSem);
        ObtainSemaphore(&PauseSem);
        SetTaskPri(SysBase->ThisTask, oldpri);
}

/*
 *              CheckForScreen()
 *
 *              Checks if we've already got a SnoopDos screen open -- if we do,
 *              then returns TRUE. If we don't, then tries to lock the current
 *              screen and returns TRUE. If we can't lock the screen, then
 *              displays an error message and returns FALSe.
 */
int CheckForScreen(void)
{
        if (!SnoopScreen) {
                SetupScreen();
                if (!SnoopScreen) {
                        ShowError(MSG(MSG_ERROR_NO_SCREEN));
                        return (FALSE);
                }
        }
        return (TRUE);
}

/*
 *              ShowSnoopDos()
 *
 *              Attempts to open SnoopDos on the user's current preferred screen.
 *              If SnoopDos is already running, but on a different screen, then
 *              it is moved to this screen. Any open SnoopDos windows are moved,
 *              though any AmigaGuide help remains on the previous screen until
 *              the next time help is requested.
 *
 *              Fails if we can't lock the new screen for some reason (this
 *              should never happen!)
 */
int ShowSnoopDos(void)
{
        struct Screen *prevscr  = SnoopScreen;
        struct Window *prevfunc = FuncWindow;
        struct Window *prevform = FormWindow;
        struct Window *prevset  = SetWindow;
 
        if (!SetupScreen()) {
                ShowError(MSG(MSG_ERROR_NO_SCREEN));
                return (FALSE);
        }
        RemoveProgramFromWorkbench();
        ScreenToFront(SnoopScreen);
        if (prevscr == SnoopScreen) {
                /*
                 *              We were already running, and we're on the
                 *              same screen, so nothing else to do. For good
                 *              measure, we bring the main window to the
                 *              foreground, however.
                 */
                OpenMainWindow();
                return (TRUE);
        }
        if (prevscr != NULL) {
                /*
                 *              We're moving from a previous screen to this new screen,
                 *              so bring all the windows with us
                 */
                CleanupSubWindows();
                CloseMainWindow();
                OpenMainWindow();
                if (prevset)    OpenSettingsWindow();
                if (prevfunc)   OpenFunctionWindow();
                if (prevform)   OpenFormatWindow();
        } else {
                /*
                 *              We're opening on an entirely new screen -- if
                 *              DisableOnHide is true, then we need to restore
                 *              the saved state of Disable/Enable (since it will
                 *              have been disabled while in the background)
                 */
                if (DisableOnHide)
                        SetMonitorMode(LastKnownState);
                OpenMainWindow();
        }
        return (TRUE);
}

/*
 *              HideSnoopDos()
 *
 *              Removes all SnoopDos windows from the screen. If DisableOnHide
 *              is true, then SnoopDos is put into a disabled state. Otherwise,
 *              if we're currently paused, then SnoopDos is unpaused (since
 *              otherwise, it would be easy to get into a situation where
 *              everything was frozen and you could do nothing to bring
 *              SnoopDos back to life).
 *
 *              If commodities library hasn't been opened, then we don't hide
 *              since that would make it difficult to resurrect SnoopDos again.
 *              However, if commodities library is open but the user has given
 *              an invalid hotkey, we do hide -- they can always use commodities
 *              exchange to reactivate it.
 */
void HideSnoopDos(void)
{
        LastKnownState = MonitorType;

        if (CurSettings.Setup.HideMethod == HIDE_NONE)
                return;

        CleanupSubWindows();
        CloseMainWindow();
        CleanupScreen();
        if (DisableOnHide)
                SetMonitorMode(MONITOR_DISABLED);
        else if (Paused)
                SetMonitorMode(MONITOR_NORMAL);
        if (CurSettings.Setup.HideMethod != HIDE_INVIS)
                AddProgramToWorkbench(CurSettings.Setup.HideMethod);
}

/*
 *              InitMenus()
 *
 *              Should be called once as soon as the language file has been
 *              read in. Walks down the menu structure and replaces the
 *              message ID's with actual pointers to the message.
 */
void InitMenus(void)
{
        struct NewMenu *menu;

        for (menu = &MainMenu[0]; menu->nm_Type != NM_END; menu++) {
                if (menu->nm_Label && menu->nm_Label != NM_BARLABEL) {
                        char *msg = MSG((ULONG)(menu->nm_Label));

                        menu->nm_Label = msg + 2;
                        if (*msg && *msg != ' ')
                                menu->nm_CommKey = msg;
                }
        }
}

/*
 *              CalcMinMainSize(gadgetfa, bufferfa, &width, &height)
 *
 *              Fills in width and height with the minimum allowable dimensions
 *              the main window can obtain using the specified gadget and buffer
 *              fonts. Returns TRUE for success, FALSE for failure (e.g. window
 *              to big to fit on screen using current fonts).
 */
BOOL CalcMinMainSize(struct TextAttr *gadgetfa, struct TextAttr *bufferfa,
                                         int *pwidth, int *pheight)
{
        struct TextFont *gfont;
        struct TextFont *bfont;
        int widthleft;
        int widthright;
        int width;
        int height;
        int fonty;
        int bfonty;
        int bfontx;
        int bordright = SizeImage->Width;
        int bordbot   = SizeImage->Height;

        gfont = MyOpenFont(gadgetfa);
        if (!gfont)
                return (FALSE);

        bfont = MyOpenFont(bufferfa);
        if (!bfont) {
                CloseFont(gfont);
                return (FALSE);
        }
        if (bfont->tf_Flags & FPF_PROPORTIONAL) {
                CloseFont(bfont);
                CloseFont(gfont);
                return (FALSE);
        }
        fonty           = gfont->tf_YSize;
        bfonty          = bfont->tf_YSize;
        bfontx      = bfont->tf_XSize;

        widthleft   = MaxTextLen(gfont, MainWidthLeftText)  + 12;
        widthright  = MaxTextLen(gfont, MainWidthRightText) + 12;

        width  = (widthleft + widthright + MAIN_MARGIN) * 2 +
                         BorderLeft + bordright;
        height = TitlebarHeight + (bfonty + BoxInterGap) * 3 +
                         bordbot + 20 + (fonty + 10) * 3;

        CloseFont(bfont);
        CloseFont(gfont);

        if (width > ScreenWidth || height > ScreenHeight)
                return (FALSE);

        *pwidth  = width;
        *pheight = height;
        return (TRUE);
}

/*
 *              SetupBufferRastPort()
 *
 *              Sets up the main window rastport to have the correct font and
 *              colours for rendering. Should always be called before OutputBufLine
 *              if gadget operations may have occurred since the last redraw.
 */
void SetupBufferRastPort(void)
{
        struct RastPort *rport = MainWindow->RPort;

        SetAPen(rport, 1);
        SetBPen(rport, 0);
        SetDrMd(rport, JAM2);
        SetFont(rport, BufferFont);
}

/*
 *              DrawHeaderLine()
 *
 *              Redraws the current event header line in the top part of the window
 *
 *              If a column is currently selected for dragging, then draws the
 *              column heading in white to indicate it's active.
 */
void DrawHeaderLine(void)
{
        struct RastPort *rport = MainWindow->RPort;
        int numcols = RightCol - LeftCol + 1;

        if (ClearMainRHS)
                numcols = BoxCols;

        SetupBufferRastPort();
        FormatEvent(BufferEFormat, NULL, BufferLine, LeftCol, LeftCol+numcols-1);

        /*
         *              We mark the very last position on the header line with a single
         *              dot, which can then be grabbed by the user to resize the width
         *              of the last column. We don't overwrite the actual text of the
         *              final header, if it's narrow.
         *
         *              If the columns are currently being dragged, we display a | instead
         *              of a dot, to make it easier to spot, and we display it even if
         *              it would overwrite some of the final heading.
         */
        if (DraggingColumn) {
                /*
                 *              Draw the currently selected column with a white heading,
                 *              and the other stuff with a black heading
                 */
                int selectwidth = SelectEvent->width;

                if ((SelectEvent+1)->type != EF_END) {
                        /*
                         *              If we're not highlighting the last column already
                         *              then indicate the end of the last column with a
                         *              little vertical bar
                         */
                        BufferLine[BufferWidth - LeftCol - 1] = '|';
                }
                if (SelectStartCol > LeftCol) {
                        /*
                         *              Draw black portion to left of selected column
                         */
                        int width = MIN(SelectStartCol - LeftCol, numcols);

                        Move(rport, BoxInLeft, BoxHeaderLine);
                        Text(rport, BufferLine, width);
                }
                if ((SelectStartCol + selectwidth) < (LeftCol + numcols)) {
                        /*
                         *              Draw black portion to right of selected column
                         */
                        int xoffset = SelectStartCol + selectwidth - LeftCol;

                        if (xoffset < 0)
                                xoffset = 0;

                        Move(rport, BoxInLeft + BoxCharWidth * xoffset, BoxHeaderLine);
                        Text(rport, BufferLine + xoffset, numcols - MAX(xoffset,0));
                }
                if (SelectStartCol >= LeftCol && SelectStartCol < (LeftCol+numcols)) {
                        /*
                         *              Draw selected column heading in white
                         */
                        int xoffset = SelectStartCol - LeftCol;
                        int fwidth  = MIN(selectwidth, numcols - xoffset);

                        if (fwidth > 0) {
                                SetAPen(rport, 2);
                                Move(rport, BoxInLeft  + xoffset * BoxCharWidth,
                                         BoxHeaderLine);
                                Text(rport, BufferLine + xoffset, fwidth);
                                SetAPen(rport, 1);
                        }
                } else if (SelectStartCol < LeftCol &&
                                   (SelectStartCol + selectwidth) >= LeftCol) {
                        /*
                         *              Draw portion of selected column heading in white
                         */
                        int colwidth = SelectStartCol + selectwidth - LeftCol;

                        SetAPen(rport, 2);
                        Move(rport, BoxInLeft, BoxHeaderLine);
                        Text(rport, BufferLine, MIN(colwidth, numcols));
                        SetAPen(rport, 1);
                }
        } else {
                /*
                 *              Not dragging a column, so draw entire line in black
                 */
                Move(rport, BoxInLeft, BoxHeaderLine);
                Text(rport, BufferLine, numcols);
        }
}

/*
 *              RedrawMainWindow()
 *
 *              Redraws all the main parts of the window, except gadgets.
 */
void RedrawMainWindow(void)
{
        struct RastPort *rport;
        
        if (!MainWindow)
                return;

        rport = MainWindow->RPort;
        DrawBevelBox(rport, BoxLeft, MainWindow->BorderTop,
                                                BoxWidth, BoxTop - MainWindow->BorderTop,
                                                GT_VisualInfo,  MainVI,
                                                TAG_DONE);
        DrawBevelBox(rport, BoxLeft, BoxTop, BoxWidth, BoxHeight,
                                                GT_VisualInfo,  MainVI,
                                                TAG_DONE);
        /*
         *              We only draw the third bevel box if either the status line or
         *              gadget line is enabled.
         */
        if (StatusLine || GadgetsLine) {
                DrawBevelBox(rport, BoxLeft, BoxTop + BoxHeight, BoxWidth,
                                                        MainWindow->Height - MainWindow->BorderBottom -
                                                                                                 BoxTop - BoxHeight,
                                                        GT_VisualInfo,  MainVI,
                                                        TAG_DONE);
        }
        DrawHeaderLine();
        ShowBuffer(TopSeq, DISPLAY_ALL);
        if (ClearMainRHS) {
                if ((RightCol-LeftCol+1) < BoxCols) {
                        /*
                         *              Erase strip to right of our rendered text
                         */
                        SetAPen(rport, 0);
                        SetDrMd(rport, JAM1);
                        RectFill(rport, BoxInLeft + (RightCol-LeftCol+1) * BoxCharWidth,
                                                        BoxInTop,
                                                        BoxInLeft + BoxInWidth - 1,
                                                        BoxInTop + BoxInHeight - 1);
                }
                ClearMainRHS = 0;
        }
}

/*
 *              SetTextSpacing(newspacing)
 *
 *              Updates the spacing of the main window to reflect the new spacing
 *              value passed in, and adjust all the global variables accordingly.
 */
void SetTextSpacing(int newspacing)
{
        int bfonty                = BufferFont->tf_YSize;
        int bfontbaseline = BufferFont->tf_Baseline;
        int maxheight;

        if (BoxInterGap == newspacing)
                return;         /* No change needed */
        
        ChangedSpacing = 1;                     /* Flag for resize routine */
        BoxInterGap    = newspacing;

        if (!MainWindow)
                return;

        SetAPen(MainWindow->RPort, 0);
        SetDrMd(MainWindow->RPort, JAM1);
        RectFill(MainWindow->RPort, BoxInLeft, BoxInTop,
                                                                BoxInLeft + BoxInWidth - 1,
                                                                BoxInTop + BoxInHeight - 1);
        
        maxheight       = BoxHeight - VBorderGap * 2 - 2;
        BoxSpacing      = bfonty + BoxInterGap;
        BoxRows         = (maxheight + BoxInterGap) / BoxSpacing;
        BoxInHeight     = BoxRows * BoxSpacing - BoxInterGap;
        BoxInTop        = BoxTop   + VBorderGap + 1;
        BoxBaseline     = BoxInTop + bfontbaseline;

        /*
         *              We now check to see if we're currently at the end of the buffer,
         *              and if so, make sure we stay that way by repositioning after the
         *              line change.
         */
        if (BottomSeq >= EndSeq)
                ShowBuffer(EndSeq, DISPLAY_NONE);
                
        LOCK_LAYERS;
        RedrawMainWindow();
        UNLOCK_LAYERS;
        UpdateMainVScroll();
}
        
/*
 *              InitMainMargins()
 *
 *              Initialises the window margins according to the currently selected
 *              left offset and the current buffer width (as determined by the
 *              format string). Automatically ensures margins are kept within
 *              the bounds of the displayable area, adjusting them as necessary.
 *
 *              On entry, LeftCol should contain the current left margin, BoxCols
 *              should be initialised to the current width of the displayable
 *              buffer area, and BufferWidth should be the maximum width of the
 *              currently selected format string.
 */
void InitMainMargins(void)
{
        if ((LeftCol + BoxCols) > BufferWidth)
                LeftCol = BufferWidth - BoxCols;

        if (LeftCol < 0)
                LeftCol = 0;

        RightCol = LeftCol + BoxCols - 1;
        if (RightCol >= BufferWidth) {
                RightCol = BufferWidth - 1;
                if (RightCol < 0)
                        RightCol = 0;
        }
}

/*
 *              FreeScrollGadgets()
 *
 *              Frees all associated BOOPSI objects and images used by the
 *              scroll gadgets. Must only be called after the main window
 *              has closed.
 */
void FreeScrollGadgets(void)
{
        int i;

        for (i = GID_STARTSCROLL; i <= GID_ENDSCROLL; i++) {
                if (Gadget[i]) {
                        DisposeObject(Gadget[i]);
                        Gadget[i] = NULL;
                }
        }
        for (i = 0; i < 4; i++) {
                if (ScrollImage[i]) {
                        DisposeObject(ScrollImage[i]);
                        ScrollImage[i] = NULL;
                }
        }
}

/*
 *              FreeMainGadgets()
 *
 *              Frees all the gadgets allocated for the main window.
 */
void FreeMainGadgets(void)
{
        if (BufferFont) {
                CloseFont(BufferFont);
                BufferFont  = NULL;
        }
        if (MainGadList) {
                FreeGadgets(MainGadList);
                MainGadList = NULL;
        }
}

/*
 *              CreateScrollGadgets()
 *
 *              Creates the list of six gadgets that are used to give our window
 *              scroll bars and scroll arrows. These have to be created independently
 *              of the GadTools button gadgets, because since they are in the border,
 *              they must be added to the window at the time it is opened, and not
 *              afterwards.
 *
 *              The GadTools gadgets are removed and then re-added (with new
 *              size-sensitive positions) whenever the window is resized; if we
 *              try and do this with the BOOPSI border gadgets however, Intuition
 *              gets the positioning subtely wrong the second and subsequent times.
 *
 *              Hence you must call this function before opening the main window. All
 *              the gadgets are created relative to the current window dimensions, and
 *              so do not depend on specific hardcoded with/height values.
 *
 *              Returns a pointer to the gadget list, or NULL if the gadgets
 *              couldn't be created.
 */
struct Gadget *CreateScrollGadgets(void)
{
        struct Gadget *gadlist  = NULL;
        struct Gadget *gad              = (struct Gadget *)&gadlist;
        int sizew       = SizeImage->Width;
        int sizeh       = SizeImage->Height;
        int bw      = (ScreenResolution == SYSISIZE_LOWRES) ? 1 : 2;
        int i;

        /*
         *              First, create the arrows for the scroll bars. Gad starts off
         *              pointing to gadlist, which will receive the pointer to the
         *              first gadget we create.
         */
        ScrollArrows[0].widthoffset  =
        ScrollArrows[1].widthoffset  = 1 - sizew;
        ScrollArrows[2].heightoffset =
        ScrollArrows[3].heightoffset = 1 - sizeh;

        for (i = 0; i < 4; i++) {
                struct ScrollData *scd = &ScrollArrows[i];
                int gadid = scd->gadgetid;
                struct Image *img;

                img = (struct Image *)
                                NewObject(NULL, "sysiclass",
                                                  SYSIA_DrawInfo,       ScreenDI,
                                                  SYSIA_Which,          scd->imagetype,
                                                  SYSIA_Size,           ScreenResolution,
                                                  TAG_END);
                if (!img)
                        goto sgad_failed;
                ScrollImage[i] = img;

                gad = (struct Gadget *)
                                NewObject(NULL,                         "buttongclass",
                                                  GA_ID,                        gadid,
                                                  GA_Immediate,         TRUE,
                                                  GA_Image,                     img,
                                                  GA_Width,                     img->Width,
                                                  GA_Height,            img->Height,
                                                  GA_Previous,          gad,
                                                  GA_RelVerify,         TRUE,
                                                  scd->borderpos,       TRUE,
                                                  GA_RelRight,          scd->widthscale * img->Width +
                                                                                        scd->widthoffset,
                                                  GA_RelBottom,         scd->heightscale * img->Height +
                                                                                        scd->heightoffset,
                                                  ICA_TARGET,           ICTARGET_IDCMP,
                                                  TAG_END);
                if (!gad)
                        goto sgad_failed;
                Gadget[gadid] = gad;
        }
        /*
         *              Now create our two BOOPSI scroll gadgets in the window border
         */
        gad = (struct Gadget *)NewObject(NULL,                          "propgclass",
                                                                         GA_ID,                         GID_HSCROLLER,
                                                                         PGA_Freedom,           FREEHORIZ,
                                                                         PGA_NewLook,           TRUE,
                                                                         PGA_Borderless,        TRUE,
                                                                         PGA_Top,                       LeftCol,
                                                                         PGA_Visible,           BoxCols,
                                                                         PGA_Total,                     BufferWidth,
                                                                         GA_Left,                       3,
                                                                         GA_RelBottom,          3 - sizeh,
                                                                         GA_RelWidth,           -sizew - 5 -
                                                                                                                ScrollImage[0]->Width -
                                                                                                                ScrollImage[1]->Width,
                                                                         GA_Height,                     sizeh - 4,
                                                                         GA_Previous,           gad,
                                                                         GA_BottomBorder,       TRUE,
                                                                         ICA_TARGET,            ICTARGET_IDCMP,
                                                                         ICA_MAP,                       RZ_MapTags,
                                                                         TAG_END);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_HSCROLLER] = gad;

        gad = (struct Gadget *)NewObject(NULL,                          "propgclass",
                                                                         GA_ID,                         GID_VSCROLLER,
                                                                         PGA_Freedom,           FREEVERT,
                                                                         PGA_NewLook,           TRUE,
                                                                         PGA_Borderless,        TRUE,
                                                                         PGA_Top,                       TopSeq - FirstSeq,
                                                                         PGA_Visible,           BoxRows,
                                                                         PGA_Total,                     EndSeq - FirstSeq + 1,
                                                                         GA_RelRight,           bw - sizew + 3,
                                                                         GA_Top,                        TitlebarHeight + 1,
                                                                         GA_Width,                      sizew - bw - bw - 4,
                                                                         GA_RelHeight,          -TitlebarHeight -
                                                                                                                ScrollImage[2]->Height-
                                                                                                                ScrollImage[3]->Height-
                                                                                                                sizeh - 2,
                                                                         GA_RightBorder,        TRUE,
                                                                         GA_Previous,           gad,
                                                                         ICA_TARGET,            ICTARGET_IDCMP,
                                                                         ICA_MAP,                       RZ_MapTags,
                                                                         TAG_END);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_VSCROLLER] = gad;
        return (gadlist);

sgad_failed:
        FreeScrollGadgets();
        return (NULL);
}

/*
 *              CreateMainGadgets(width, height, statusline)
 *
 *              Creates the gadgets for the main SnoopDos window, assuming a window
 *              of size width x height. MainGadList will point to the list of
 *              gadgets when completed. Returns NULL if unsuccessful, or MainGadList
 *              if successful.
 *
 *              Statusline is a flag that says whether or not the status gadget is
 *              to be created.
 *
 *              There are basically seven possible gadget layouts:
 *
 *                      - 4 x 3 grid with status line on top
 *                      - 4 x 2 grid with no status line
 *                      - 6 x 2 grid with status line occupying the upper 4 left spaces
 *                      - 8 x 1 grid with no status line
 *                      - 12 x 1 grid with status line
 *          - 0 grid with no gadgets or status line
 *          - 1 line grid with status line but no gadgets
 *
 *              The first two are used when the window is narrow; the second two
 *              when the window is wider. We use colwidth[0] to hold the width
 *              of the narrow gadgets, colwidth[1] for the wide gadgets, and
 *              colwidth[2] for the status line gadget.
 *
 *              To allow us to conveniently replace one gadget definition with
 *              another, we use the concept of invisible buttons -- these are
 *              like normal buttons, but their X position is set way off to the
 *              left of the window where they won't be displayed; to reveal
 *              the buttons, we simply restore their X position and make the
 *              X position of a different button negative.
 *
 *              Finally, we have toggle buttons which flip flop between on and off.
 *              These are defined as MAIN_NARROW_TOGGLE.
 */
struct Gadget *CreateMainGadgets(struct TextAttr *gadgetfa,
                                                                 struct TextAttr *bufferfa,
                                                                 int width, int height, int statusline)
{
        struct MainGadgets *mg;
        struct NewGadget ng;
        struct TextFont *font;
        struct Gadget *gadlist;
        struct Gadget *gad;
        UWORD colwidth[MAIN_NUMWIDTHS];
        WORD  colpos[12];
        UWORD spacing;                          /* Spacing between gadget tops                          */
        UWORD yoffset;                          /* Offset of first gadget top in window         */
        int fonty;                                      /* Y height of gadget font                                      */
        int bfontx;                                     /* X height of buffer font                                      */
        int bfonty;                                     /* Y height of buffer font                                      */
        int bfontbaseline;                      /* Baseline of buffer font                                      */
        int innerwidth;                         /* Usable width of window                                       */
        int extraspace;                         /* Spare space inbetween buffer and gadgets     */
        int gheight;                            /* Height of a single gadget                            */
        int width4;                                     /* Minimum width needed for 4 x 3 gadgets       */
        int width6;                                     /* Minimum width needed for 6 x 2 gadgets       */
        int width8;                                     /* Minimum width needed for 8 x 1 gadgets       */
        int width12;                            /* Minimum width needed for 12 x 1 gadgets      */
        int gridtype;                           /* Grid type for gadget layout                          */
        int numrows;                            /* Number of rows of gadgets involved           */
        int gadg_gap;                           /* Aspect-corrected gap between gadget rows     */
        int margin = MAIN_MARGIN;       /* Gadget margin at left edge of window         */
        int boxmaxwidth;                        /* Temporary width of listview box                      */
        int boxheight;                          /* Temporary height of listview box                     */

        int bordright  = SizeImage->Width;
        int bordbot    = SizeImage->Height;

/*
 *              Quick defines for our internal grid arrangements
 */
#define FOUR_BY_THREE   0
#define SIX_BY_TWO              1
#define EIGHT_BY_ONE    2
#define TWELVE_BY_ONE   3

        if (MainGadList)
                FreeMainGadgets();

        if (!MainVI) {
                MainVI = GetVisualInfoA(SnoopScreen, NULL);
                if (!MainVI)
                        return (NULL);
        }
        font = MyOpenFont(gadgetfa);
        if (!font)
                return (NULL);

        BufferFont = MyOpenFont(bufferfa);
        if (!BufferFont) {
                CloseFont(font);
                return (NULL);
        }
        fonty                   = font->tf_YSize;
        bfontx                  = BufferFont->tf_XSize;
        bfonty                  = BufferFont->tf_YSize;
        bfontbaseline   = BufferFont->tf_Baseline;

        if (SquareAspect) {
                VBorderGap   = 2;
                gadg_gap         = 3;
        } else {
                VBorderGap   = 1;
                gadg_gap         = 2;
        }

        gheight         = fonty + GadgetHeight;
        spacing     = gheight + gadg_gap;

        colwidth[MAIN_NARROW]            = MaxTextLen(font, MainWidthLeftText)  + 12;
        colwidth[MAIN_WIDE]                      = MaxTextLen(font, MainWidthRightText) + 12;

        /*
         *              First, check if window is wide enough for gadgets. If not,
         *              then fail.
         */
        innerwidth = width - BorderLeft - bordright;

        if ((colwidth[MAIN_NARROW]+colwidth[MAIN_WIDE]+margin) * 2 > innerwidth) {
                CloseFont(font);
                CloseFont(BufferFont);
                BufferFont = NULL;
                return (NULL);
        }

        /*
         *              Now calculate how many columns our fake listview can hold using
         *              the current font, and use this to adjust our margins accordingly.
         *
         *              The '4' in boxmaxwidth comes from 2 pixels for the left and right
         *              of the box (the borders)
         */
        boxmaxwidth     = innerwidth - 2*BOX_LEFT_MARGIN - 4;
        BoxCols         = (boxmaxwidth / bfontx);

        width4          = colwidth[MAIN_NARROW] * 2 + colwidth[MAIN_WIDE] * 2;
        width6          = colwidth[MAIN_NARROW] * 4 + colwidth[MAIN_WIDE] * 2;
        width8          = colwidth[MAIN_NARROW] * 4 + colwidth[MAIN_WIDE] * 4;
        width12         = colwidth[MAIN_NARROW] * 8 + colwidth[MAIN_WIDE] * 4;
        colpos[0]       = BorderLeft + margin;
        innerwidth -= margin * 2;

        if (GadgetsLine) {
                if (statusline && width12 <= innerwidth) {
                        /*
                         *              Setup layout for 12 x 1 grid
                         */
                        int totalspace = innerwidth - width12;
                        int i;

                        for (i = 1; i < 8; i++)
                                colpos[i] = colpos[0] + colwidth[MAIN_NARROW] * i
                                                                          + (totalspace*i)/11;
                        for (i = 8; i < 12; i++)
                                colpos[i] = colpos[0] + colwidth[MAIN_NARROW] * 8
                                                                          + colwidth[MAIN_WIDE]   * (i-8)
                                                                          + (totalspace*i)/11;

                        /*
                         *              We always ensure that there is at least two pixels
                         *              between the end of the status line and the beginning
                         *              of the next gadget, otherwise we get a nasty artifact
                         *              where the identically-coloured white borders meet.
                         */
                        colwidth[MAIN_STATUS] = colpos[3] - colpos[0] +
                                                                        colwidth[MAIN_NARROW];
                        if ((colpos[0] + colwidth[MAIN_STATUS] + 2) > colpos[4])
                                colwidth[MAIN_STATUS] = colpos[4] - colpos[0] - 2;

                        gridtype = TWELVE_BY_ONE;
                        numrows  = 1;
                } else if (statusline && width6 <= innerwidth) {
                        /*
                         *              Setup layout for 6 x 2 grid.
                         */
                        int totalspace = innerwidth - width6;
                        int i;

                        for (i = 1; i < 4; i++)
                                colpos[i] = colpos[0] + colwidth[MAIN_NARROW]*i +
                                                        (totalspace*i)/5;

                        colwidth[MAIN_STATUS] = colpos[3] - colpos[0] +
                                                                        colwidth[MAIN_NARROW];

                        colpos[5]       = width - bordright - margin - colwidth[MAIN_WIDE];
                        colpos[4]       = colpos[5] - colwidth[MAIN_WIDE] - totalspace/5;

                        /*
                         *              If there isn't at least 2 pixels between the status line
                         *              and the adjacent wide gadget, reduce the size of each
                         *              wide gadget by 1 pixel (2 pixels in total) to prevent
                         *              a nasty clash between the two gadget edges.
                         */
                        if ((colpos[0] + colwidth[MAIN_STATUS] + 2) > colpos[4]) {
                                colpos[4] += 2;
                                colpos[5]++;
                                colwidth[MAIN_WIDE]--;
                        }
                        gridtype    = SIX_BY_TWO;
                        numrows         = 2;

                } else if (!statusline && width8 <= innerwidth) {
                        /*
                         *              Setup layout for 8 x 1 grid
                         */
                        int totalspace = innerwidth - width8;
                        int i;

                        for (i = 1; i < 4; i++)
                                colpos[i] = colpos[0] + colwidth[MAIN_NARROW]*i +
                                                        (totalspace*i)/7;

                        for (i = 4; i < 8; i++)
                                colpos[i] = colpos[0] + colwidth[MAIN_NARROW] * 4
                                                                          + colwidth[MAIN_WIDE]   * (i-4)
                                                                          + (totalspace*i)/7;
                        gridtype = EIGHT_BY_ONE;
                        numrows  = 1;

                } else {
                        /*
                         *              Setup layout for 4 x 3 grid
                         */
                        int totalspace = (innerwidth - width4);

                        colwidth[MAIN_STATUS] = width - bordright - margin - colpos[0];
                        colpos[3]       = width - bordright - margin - colwidth[MAIN_WIDE];
                        colpos[1]       = colpos[0] + colwidth[MAIN_NARROW] + totalspace/3;
                        colpos[2]       = colpos[3] - colwidth[MAIN_WIDE]   - totalspace/3;
                        gridtype        = FOUR_BY_THREE;
                        if (statusline)
                                numrows  = 3;
                        else
                                numrows  = 2;
                }
        } else {
                /*
                 *              No gadget line, so set all the gadget positions to a
                 *              negative value to ensure the gadgets stay hidden, and
                 *              create or don't create a status line as appropriate.
                 */
                int i;

                colwidth[MAIN_STATUS] = innerwidth;
                gridtype              = TWELVE_BY_ONE;
                numrows               = 1;
                for (i = 1; i < 12; i++)
                        colpos[i] = INVIS_LEFT_EDGE;    /* Gadget off left-hand edge */

                if (!statusline) {
                        numrows   = 0;
                        colpos[0] = INVIS_LEFT_EDGE;
                }
        }
        colwidth[MAIN_NARROW_TOGGLE] = colwidth[MAIN_NARROW];
        colwidth[MAIN_WIDE_INVIS]        = colwidth[MAIN_WIDE];

        /*
         *              Next, calculate how many lines we can fit in the main buffer
         *              display. The total vertical margin is fonty + 8 + BarHeight
         *              pixels, which is distributed as follows:
         *
         *                                      ----titlebar----
         *                  <1 pixel border>
         *                                      4 pixel gap
         *                                      Column headings
         *                                      <VBorderGap>
         *                                      2 pixels of border
         *                                      <VBorderGap>
         *                                      Lines of text
         *                                      <VBorderGap>
         *                                      (1 pixel border)
         *
         *              And in addition, if we have gadgets enabled:
         *
         *                                      (1 pixel border)
         *                                      <gadg_gap>
         *                                      ---gadget area---
         *                                      <gadg_gap>
         *                                      (1 pixel border)
         *                                      ---bottom border---
         *
         *              The minimum spacing between gadgets is also gadg_gap.
         *              Each individual gadget has a height 6 pixels higher than
         *              the font size.
         *
         *              The catch is to take the <any additional space> and distribute
         *              it equally between the various gadget spaces etc. For N rows
         *              of gadgets, this implies that there are (N+1) "gaps" that can
         *              have vertical space added to them.
         */
        BoxLeft                 = BorderLeft;
        BoxWidth                = width - bordright - BoxLeft;
        BoxInLeft               = BoxLeft + BOX_LEFT_MARGIN + 2;
        BoxInWidth              = bfontx * BoxCols;
        BoxCharWidth    = bfontx;
        BoxCharHeight   = bfonty;

        yoffset                 = height - bordbot - spacing * numrows - gadg_gap;
        BoxSpacing      = bfonty + BoxInterGap;
        if (SquareAspect) {
                BoxTop                  = TitlebarHeight + 6 + bfonty;
                BoxHeaderLine   = TitlebarHeight + 4 + bfontbaseline;
        } else {
                BoxTop                  = TitlebarHeight + 5 + bfonty;
                BoxHeaderLine   = TitlebarHeight + 3 + bfontbaseline;
        }
        if (numrows)
                boxheight   = yoffset - gadg_gap - VBorderGap*3 - BoxTop-2;
        else
                boxheight       = height - bordbot - BoxTop - 2*VBorderGap - 2;
        BoxRows = (boxheight + BoxInterGap) / BoxSpacing;
        if (numrows)
                BoxHeight       = BoxRows * BoxSpacing - BoxInterGap + 2*VBorderGap + 2;
        else
                BoxHeight       = height - bordbot - BoxTop;

        BoxLowest       = BoxTop   + BoxHeight + VBorderGap;
        BoxInTop        = BoxTop   + VBorderGap + 1;
        BoxBaseline     = BoxInTop + bfontbaseline;
        BoxInHeight     = BoxRows * BoxSpacing - BoxInterGap;

        InitMainMargins();

        /*
         *              Now get ready to create the button and status line gadgets
         *              We always create the gadgets, even if they're not currently
         *              being displayed.
         */     
        ng.ng_TextAttr          = gadgetfa;
        ng.ng_VisualInfo        = MainVI;
        ng.ng_Flags         = PLACETEXT_IN;

        ng.ng_GadgetText        = "";
        ng.ng_GadgetID          = 0;

        gadlist = NULL;
        gad = CreateContext(&gadlist);
        if (!gad)
                goto mgad_failed;

        /*
         *              Now, the buttons and status line gadget. At this point,
         *              we distribute any additional space that we have left between
         *              the lower scroll bar and the current start of the gadget
         *              grid (aka yoffset).
         */
        extraspace              = yoffset - (BoxTop + BoxHeight);
        yoffset            -= extraspace - extraspace/(numrows+1) -
                                          ((extraspace % (numrows+1)) + 1)/2;
        spacing            += extraspace / (numrows + 1);
        yoffset            += gadg_gap;
        ng.ng_Height    = gheight; 

        if (gridtype == FOUR_BY_THREE && !statusline)
                yoffset -= spacing;     /* Bias to allow for invisible status bar */

        for (mg = &MainGadgs[0]; mg->gadgid; mg++) {
                switch (gridtype) {
                        case FOUR_BY_THREE:
                                ng.ng_LeftEdge  = colpos[mg->col4];
                                ng.ng_TopEdge   = yoffset + spacing * mg->row4;
                                ng.ng_Width             = colwidth[mg->widthtype];
                                break;

                        case SIX_BY_TWO:
                                ng.ng_LeftEdge  = colpos[mg->col6];
                                ng.ng_TopEdge   = yoffset + spacing * mg->row6;
                                ng.ng_Width             = colwidth[mg->widthtype];
                                break;

                        case EIGHT_BY_ONE:
                                ng.ng_LeftEdge  = colpos[mg->col8];
                                ng.ng_TopEdge   = yoffset;
                                ng.ng_Width             = colwidth[mg->widthtype];
                                break;
                
                        case TWELVE_BY_ONE:
                                ng.ng_LeftEdge  = colpos[mg->col12];
                                ng.ng_TopEdge   = yoffset;
                                ng.ng_Width             = colwidth[mg->widthtype];
                                break;
                }
                ng.ng_GadgetText        = MSG(mg->stringid);
                ng.ng_GadgetID          = mg->gadgid;

                if (mg->gadgid == GID_STATUS) {
                        if (statusline) {
                                int len = GetTextLen(font, MSG(mg->stringid)) + 9;

                                ng.ng_LeftEdge += len;
                                ng.ng_Width        -= len;
                                ng.ng_Flags             = PLACETEXT_LEFT;
                                gad = CreateGadget(TEXT_KIND, gad, &ng,
                                                                   GT_Underscore,       '_',
                                                                   GTTX_Text,           StatusLineText,
                                                                   GTTX_Border,         TRUE,
                                                                   TAG_DONE);
                                ng.ng_Flags                = PLACETEXT_IN;
                                if (!gad)
                                        goto mgad_failed;
                                Gadget[GID_STATUS] = gad;
                        } else {
                                Gadget[GID_STATUS] = NULL;
                        }
                } else {
                        gad = CreateGadget(BUTTON_KIND, gad, &ng,
                                                           GT_Underscore,       '_',
                                                           TAG_DONE);
                        if (!gad)
                                goto mgad_failed;

                        if (mg->widthtype == MAIN_NARROW_TOGGLE)
                                gad->Activation |= GACT_TOGGLESELECT;

                        if (mg->widthtype == MAIN_WIDE_INVIS)
                                gad->LeftEdge = INVIS_LEFT_EDGE;

                        Gadget[mg->gadgid] = gad;
                }
                AddKeyShortcut(MainKeyboard, mg->gadgid, mg->stringid);
        }
        LogButtonLeft = Gadget[GID_OPENLOG]->LeftEdge;
        SetLogGadget(DefaultLogMode, LG_NOREFRESH);

        /*
         *              Now some additional initialisation for the Pause
         *              and Disable gadgets in case they're initially set
         */
        if (Paused)             Gadget[GID_PAUSE  ]->Flags |= GFLG_SELECTED;
        if (Disabled)   Gadget[GID_DISABLE]->Flags |= GFLG_SELECTED;
                
        /*
         *              Now a quick check to see if we have opened commodities.library
         *              yet -- if we haven't, then disable the hide gadget.
         */
        if (CurSettings.Setup.HideMethod == HIDE_NONE)
                Gadget[GID_HIDE]->Flags |= GFLG_DISABLED;

        return (gadlist);

mgad_failed:
        FreeMainGadgets();
        CloseFont(font);
        return (NULL);
}

/*
 *              RecalcMainWindow(width, height, refresh)
 *
 *              Updates the gadgets for the main window to reflect the new size,
 *              and renders the changes to the screen. If the refresh parameter
 *              is REDRAW_GADGETS, then the function will automatically redraw
 *              the gadgets in the window, otherwise this must be done by hand
 *              (or perhaps automatically if Intuition generates an IDCMP_REFRESH
 *              message after a resize event!)
 *
 *              Returns FALSE if something went wrong (in which case the caller
 *              is responsible for closing the main window) and displaying an
 *              error message.
 */
BOOL RecalcMainWindow(int width, int height, int dorefresh)
{
        struct RastPort *rport = MainWindow->RPort;
        int saveleft    = BoxLeft;
        int savewidth   = BoxWidth;
        int saveheight  = BoxHeight;
        int oldrows     = BoxRows;
        int startrow;

        if (!MainWindow)
                return (TRUE);

        if (MainGadList && !RemovedGadgets)
                RemoveGList(MainWindow, MainGadList, -1);

        MainGadList = CreateMainGadgets(CurMainGadgetFA, CurMainBufferFA,
                                                                        width, height, StatusLine);
        if (!MainGadList)
                return (FALSE);

        UpdateMainHScroll();
        UpdateMainVScroll();
        if (BoxWidth == savewidth && BoxLeft == saveleft && !ChangedSpacing) {
                if (BoxHeight == saveheight)
                        /*
                         *              Only erase from bottom of scroll box down since the
                         *              rest hasn't actually changed at all.
                         */
                        startrow = BoxLowest;
                else {
                        startrow = BoxTop + MIN(BoxHeight, saveheight) - 1;
                        startrow = MAX(startrow, TitlebarHeight);
                }
        } else
                startrow = TitlebarHeight;

        SetDrMd(rport, JAM1);
        SetAPen(rport, 0);
        AddGList(MainWindow, MainGadList, -1, -1, NULL);

        if (startrow == BoxLowest) {
                /*
                 *              If we are only refeshing the bottom part of the screen,
                 *              don't bother erasing the top half -- it looks much
                 *              cleaner.
                 */
                if (StatusLine || GadgetsLine) {
                        // SetAPen(rport, 4);   /* Makes gadget bground gray (nice!) */
                        RectFill(rport, BoxLeft + 2, BoxTop + BoxHeight + 1,
                                                        BoxLeft + BoxWidth - 3,
                                                        MainWindow->Height - MainWindow->BorderBottom - 1);
                        DrawBevelBox(rport, BoxLeft, BoxTop + BoxHeight, BoxWidth,
                                                                MainWindow->Height - MainWindow->BorderBottom -
                                                                                                         BoxTop - BoxHeight,
                                                                GT_VisualInfo,  MainVI,
                                                                TAG_DONE);
                }
                if (dorefresh)
                        RefreshGList(MainGadList, MainWindow, NULL, -1);
        } else {
                startrow = MAX(TitlebarHeight, startrow - BoxSpacing - 3);
                RectFill(rport, MainWindow->BorderLeft, startrow,
                                 width  - MainWindow->BorderRight  - 1,
                                 height - MainWindow->BorderBottom - 1);
                if (dorefresh)
                        RefreshGList(MainGadList, MainWindow, NULL, -1);
                /*
                 *              If we are currently at the end of the display, we want to
                 *              ensure that we scroll to the end after refreshing the window.
                 */
                if (TopSeq + oldrows > EndSeq) {
                        /*
                         *              Force redraw from buffer end after resize
                         */
                        TopSeq   = EndSeq;
                        TopEvent = EndEvent;
                }
                RedrawMainWindow();
                UpdateMainVScroll();
        }
        GT_RefreshWindow(MainWindow, NULL);
        CurWindowWidth  = MainWindow->Width;
        CurWindowHeight = MainWindow->Height;
        ChangedSpacing  = 0;
        return (TRUE);
}

/*
 *              SetMenuOptions()
 *
 *              Configures the main window's menu strip to match the currently
 *              selected options (e.g. Pause/Disable, Text Spacing, window type,
 *              icon type, etc.)
 *
 *              Call whenever settings change.
 *
 */
void SetMenuOptions(void)
{
        struct MenuItem *item;

#define SetMenuState(menunum, val) \
        item = ItemAddress(MainWinMenu, menunum); \
        item->Flags = (item->Flags & ~CHECKED) | ((val) ? CHECKED : 0)

        if (MainWindow && MainWinMenu) {
                int curpri = SysBase->ThisTask->tc_Node.ln_Pri;
                int i;

                SetMenuState(MENU_PAUSE,          Paused);
                SetMenuState(MENU_DISABLE,        Disabled);
                SetMenuState(MENU_AUTO_OPEN,  AutoOpen);
                SetMenuState(MENU_DIS_HIDDEN, DisableOnHide);
                SetMenuState(MENU_STATUS,         StatusLine);
                SetMenuState(MENU_GADGETS,        GadgetsLine);
                SetMenuState(MENU_ICONS,          CreateIcons);
                SetMenuState(MENU_SIMPLE,         CurSettings.SimpleRefresh);
                SetMenuState(MENU_SMART,          !CurSettings.SimpleRefresh);
                SetMenuState(MENU_AL_LEFT,        !RightAligned);
                SetMenuState(MENU_AL_RIGHT,       RightAligned);
                SetMenuState(MENU_SPACING0,       BoxInterGap == 0);
                SetMenuState(MENU_SPACING1,       BoxInterGap == 1);
                SetMenuState(MENU_SPACING2,       BoxInterGap == 2);

                SetMenuState(MENU_ROWQUAL_ANY,    RowQual == ROWQUAL_ANY);
                SetMenuState(MENU_ROWQUAL_NONE,   RowQual == ROWQUAL_NONE);
                SetMenuState(MENU_ROWQUAL_SHIFT,  RowQual == ROWQUAL_SHIFT);
                SetMenuState(MENU_ROWQUAL_ALT,    RowQual == ROWQUAL_ALT);
                SetMenuState(MENU_ROWQUAL_CTRL,   RowQual == ROWQUAL_CTRL);
                SetMenuState(MENU_ROWQUAL_ALL,    RowQual == ROWQUAL_ALL);

                if (CurSettings.Setup.HideMethod == HIDE_NONE)
                        OffMenu(MainWindow, MENU_HIDE);
                else
                        OnMenu(MainWindow,      MENU_HIDE);

                if (LogActive) {
                        OffMenu(MainWindow,     MENU_OPENLOG);
                        OnMenu(MainWindow,      MENU_CLOSELOG);
                } else {
                        OnMenu(MainWindow,      MENU_OPENLOG);
                        OffMenu(MainWindow,     MENU_CLOSELOG);
                }

                /*
                 *              We calculate our current priority on the fly. This
                 *              means we'll always be up-to-date, even if someone
                 *              changes our priority from outside the program.
                 */
                for (i = 0; i < MENU_NUMPRI; i++) {
                        int subnum = MENU_CHANGEPRI | SHIFTSUB(i);
                        struct MenuItem  *item  = ItemAddress(MainWinMenu, subnum);
                        struct IntuiText *itext = (void *)(item->ItemFill);

                        if (atoi(itext->IText) == curpri)
                                item->Flags |= CHECKED;
                        else
                                item->Flags &= ~CHECKED;
                }
        }
}
                
/*
 *              SetMainHideState(state)
 *
 *              Updates the current main window titlebar, hide gadget, and
 *              menu HIDE option to reflect the given hide state.
 *
 *              It works as follows. If state is HIDE_NONE, then the titlebar
 *              is set to <none> and the gadget/menu item are ghosted. If
 *              state is not HIDE_NONE, the gadget/menu item are unghosted,
 *              and the titlebar is set to <key sequence>, unless HotKeyActive
 *              is set to 0, in which case it is set to <invalid>.
 */
void SetMainHideState(int hidestate)
{
        char title[200];
        char *keyname = CurSettings.Setup.HotKey;

        if (!HotKeyActive)
                keyname = MSG(MSG_INVALID_HOTKEY);
        
        if (hidestate == HIDE_NONE)
                strcpy(title, SnoopDosTitle);
        else
                mysprintf(title, SnoopDosTitleKey, keyname);

        if (strcmp(CurrentTitle, title) != 0)
                strcpy(CurrentTitle, title);
        
        if (MainWindow) {
                struct Gadget *gad = Gadget[GID_HIDE];
                int oldstate       = gad->Flags & GFLG_DISABLED;
                int gadpos;

                SetWindowTitles(MainWindow, CurrentTitle, (UBYTE *)-1);

                gadpos = RemoveGadget(MainWindow, gad);
                if (hidestate == HIDE_NONE)
                        gad->Flags |= GFLG_DISABLED;
                else
                        gad->Flags &= ~GFLG_DISABLED;

                AddGadget(MainWindow, gad, gadpos);
                if ((gad->Flags & GFLG_DISABLED) != oldstate) {
                        /*
                         *              Only refresh the gadget if its state changed
                         */
                        RefreshGList(gad, MainWindow, NULL, 1);
                }
                if (MainWinMenu) {
                        if (CurSettings.Setup.HideMethod == HIDE_NONE)
                                OffMenu(MainWindow, MENU_HIDE);
                        else
                                OnMenu(MainWindow,      MENU_HIDE);
                }
        }
}

/*
 *              OpenMainWindow()
 *
 *              Opens the main window with the scroll buffer display, arrow gadgets,
 *              and buttons. Also creates the status line gadget if necessary.
 *
 *              Returns true for success, false for failure.
 */
BOOL OpenMainWindow(void)
{
        static WORD initzoomdims[] = { 0, 0, -1, -1 };
        struct Window *win;
        struct Gadget *scrollgadlist;
        struct TextAttr *gadgetfa;
        struct TextAttr *bufferfa;
        int minx, miny;
        int menupen;
        int i;
        int width  = CurSettings.MainWinWidth;
        int height = CurSettings.MainWinHeight;
        int left   = CurSettings.MainWinLeft;
        int top    = CurSettings.MainWinTop;

        CheckSegTracker();
        if (MainWindow) {
                WindowToFront(MainWindow);
                ActivateWindow(MainWindow);
                return (TRUE);
        }
        if (!CheckForScreen())
                goto open_main_failed;

        for (i = 0; (gadgetfa = MainWindowFontList[i].gadgetfa); i++) {
                bufferfa = MainWindowFontList[i].bufferfa;

                if (CalcMinMainSize(gadgetfa, bufferfa, &minx, &miny))
                        break;
        }
        if (!gadgetfa)
                goto open_main_failed;

        if (width  == -1)                                       width  = DEF_WINDOW_WIDTH;
        if (height == -1)                                       height = DEF_WINDOW_HEIGHT;
        if (width  == 0)                                        width  = CurSettings.MainWinWidth;
        if (height == 0)                                        height = CurSettings.MainWinHeight;
        if (width  < minx)                                      width  = minx;
        if (height < miny)                                      height = miny;
        if (width  > ScreenWidth)                       width  = ScreenWidth;
        if (height > ScreenHeight)                      height = ScreenHeight;
        if (left   == -1)                                       left   = 0;
        if (top    == -1)                                       top    = SnoopScreen->BarHeight + 2;

        initzoomdims[2] = ScreenWidth;
        initzoomdims[3] = ScreenHeight;
                
        ShowBuffer(TopSeq, DISPLAY_NONE);       /* Ensure scroll vars are up to date */

        MainGadList   = CreateMainGadgets(gadgetfa, bufferfa,
                                                                         width, height, StatusLine);
        scrollgadlist = CreateScrollGadgets();
        if (!scrollgadlist || !MainGadList)
                goto open_main_failed;

        Gadget[GID_ENDSCROLL]->NextGadget = MainGadList;

        CurMainGadgetFA = gadgetfa;
        CurMainBufferFA = bufferfa;

        SetMainHideState(CurSettings.Setup.HideMethod); /* Make title up to date */
        win = OpenWindowTags(NULL,
                                                 WA_Title,                      CurrentTitle,
                                                 WA_IDCMP,                      IDCMP_CLOSEWINDOW        |
                                                                                        IDCMP_REFRESHWINDOW      |
                                                                                        IDCMP_NEWSIZE            |
                                                                                        IDCMP_CHANGEWINDOW       |
                                                                                        IDCMP_RAWKEY             |
                                                                                        IDCMP_MENUPICK           |
                                                                                        IDCMP_IDCMPUPDATE        |
                                                                                        IDCMP_GADGETDOWN         |
                                                                                        IDCMP_MOUSEBUTTONS       |
                                                                                        IDCMP_MOUSEMOVE          |
                                                                                        IDCMP_MENUHELP           |
                                                                                        IDCMP_SIZEVERIFY         |
                                                                                        IDCMP_INACTIVEWINDOW |
                                                                                        BUTTONIDCMP,
                                                 WA_Left,                       left,
                                                 WA_Top,                        top,
                                                 WA_Width,                      width,
                                                 WA_Height,                     height,
                                                 WA_Flags,                      WFLG_CLOSEGADGET         |
                                                                                        WFLG_DRAGBAR             |
                                                                                        WFLG_DEPTHGADGET         |
                                                                                        WFLG_ACTIVATE            |
                                                                                        WFLG_SIZEGADGET          |
                                                                                        WFLG_SIZEBBOTTOM         |
                                                                                        WFLG_SIZEBRIGHT          |
                                                                                        WFLG_NEWLOOKMENUS,
                                                 WA_MenuHelp,           TRUE,
                                                 RefreshTag,            TRUE,
                                                 WA_NoCareRefresh,      NoCareRefreshBool,
                                                 WA_PubScreen,          SnoopScreen,
                                                 WA_MinWidth,           minx,
                                                 WA_MinHeight,          miny,
                                                 WA_MaxWidth,           -1,
                                                 WA_MaxHeight,          -1,
                                                 WA_Zoom,                       initzoomdims,
                                                 WA_Gadgets,            scrollgadlist,
                                                 TAG_DONE);
        if (!win)
                goto open_main_failed;

        MainWindow         = win;
        MainWindowPort = win->UserPort;
        MainWindowMask = 1 << MainWindowPort->mp_SigBit;

        if (DraggingColumn || DraggingRow) {
                /*
                 *              If we were previously closed while a column was being dragged,
                 *              reset the drag flag and signal ourselves to ensure we catch
                 *              any events that arrived while output was suspended.
                 */
                DraggingColumn = 0;
                DraggingRow    = 0;
                Signal(SysBase->ThisTask, NewEventMask);
        }
        RedrawMainWindow();
        // RefreshGList(MainGadList, MainWindow, NULL, -1);     /* Already done */
        GT_RefreshWindow(MainWindow, NULL);
        UpdateStatus();

        /*
         *              Now create menus for window
         */
        if (IntuitionBase->LibNode.lib_Version >= 39)
                menupen = 1;
        else
                menupen = 0;

        MainWinMenu = CreateMenus(MainMenu, GTMN_FrontPen, menupen, TAG_DONE);

        if (!MainWinMenu)
                ShowError(MSG(MSG_ERROR_CREATEMENUS));
        else {
                LayoutMenus(MainWinMenu, MainVI, GTMN_NewLookMenus, TRUE, TAG_DONE);
                SetMenuStrip(MainWindow, MainWinMenu);
                SetMenuOptions();
        }
        CurWindowWidth  = MainWindow->Width;
        CurWindowHeight = MainWindow->Height;
        AwaitingResize  = RESIZE_DONE;
        if ((SnoopScreen->Flags & SCREENTYPE) != WBENCHSCREEN) {
                /*
                 *              We're running on a custom screen, so redirect system
                 *              requesters to appear on this screen instead of the
                 *              Workbench screen. We don't do it unconditionally, because
                 *              our window title is so long that it makes the requesters
                 *              look very lopsided -- still, if we're on a custom screen,
                 *              it's better than nothing.
                 */
                *TaskWindowPtr = MainWindow;
        }
        if (DisableNestCount)
                DisableWindow(MainWindow, &MainRequester);

        return (TRUE);

open_main_failed:
        ShowError(MSG(MSG_ERROR_OPENMAIN));
        return (FALSE);
}

/*
 *              CloseMainWindow(void)
 *
 *              Closes the main SnoopDos window
 */
void CloseMainWindow(void)
{
        *TaskWindowPtr = SaveWindowPtr;         /* Restore previous screen address */
        if (MainWindow) {
                if (MainWinMenu) {
                        ClearMenuStrip(MainWindow);
                        FreeMenus(MainWinMenu);
                        MainWinMenu = NULL;
                }
                RecordWindowSizes();
                CloseWindow(MainWindow);
                FreeMainGadgets();
                MainWindow              = NULL;
                MainWindowMask  = 0;
        }
        FreeScrollGadgets();
        if (MainVI) {
                FreeVisualInfo(MainVI);
                MainVI = NULL;
        }
}

/*
 *              ShowStatus(msg)
 *
 *              Displays the specified message in the status line (if enabled)
 */
void ShowStatus(char *msg)
{
        strcpy(StatusLineText, msg);

        if (MainWindow) {
                GT_SetGadgetAttrs(Gadget[GID_STATUS], MainWindow, NULL,
                                                  GTTX_Text,    StatusLineText,
                                                  TAG_DONE);
        }
}

/*
 *              ReOpenMainWindow()
 *
 *              Closes and re-opens main window retaining current size but
 *              taking into account new font, spacing, refresh type, etc.
 *              If another window is specified, then that window is brought
 *              to the front after the main window is reopened.
 */
void ReOpenMainWindow(void)
{
        if (MainWindow) {
                /*
                 *              If we've changed spacing but not yet updated the window to
                 *              reflect this, then if we are going from 0-spacing to 2-spacing,
                 *              the buffer may loose a few lines. To ensure we remain at the
                 *              end of the buffer if we were there to start with, we do an
                 *              invisible scroll to the _very_ end, just to be safe.
                 */
                if (BottomSeq >= EndSeq) {
                        TopSeq   = EndSeq;
                        TopEvent = EndEvent;
                }
                CloseMainWindow();
                OpenMainWindow();
        }
}

/*
 *              CheckForDirtyMainWindow()
 *
 *              Checks to see if the main Window's rastport is dirty (i.e. we did
 *              a scrollraster which was partially obscured by another window, and
 *              now we need to refresh) and redraw the window if necessary.
 *
 *              Typically called after ShowBuffer()
 */
void CheckForDirtyMainWindow(void)
{
        if (MainWindow && MainWindow->RPort->Layer->Flags & LAYERREFRESH) {
                GT_BeginRefresh(MainWindow);
                RedrawMainWindow();
                GT_EndRefresh(MainWindow, TRUE);
        }
}

/*
 *              UpdateMainHScroll(void)
 *
 *              Updates the main horizontal scrollbar to reflect the current
 *              margin settings and buffer width.
 */
void UpdateMainHScroll(void)
{
        if (MainWindow)
                SetGadgetAttrs(Gadget[GID_HSCROLLER], MainWindow, NULL,
                                           PGA_Top,             LeftCol,
                                           PGA_Total,   BufferWidth,
                                           PGA_Visible, BoxCols,
                                           TAG_DONE);
}

/*
 *              UpdateMainVScroll(void)
 *
 *              Updates the main scrollbar to reflect the current state of play.
 *              N.b. Depends on the parameters calculated during the last call
 *              to ShowBuffer.
 */
void UpdateMainVScroll(void)
{
        if (MainWindow)
                SetGadgetAttrs(Gadget[GID_VSCROLLER], MainWindow, NULL,
                                           PGA_Top,             TopSeq - FirstSeq,
                                           PGA_Total,   EndSeq - FirstSeq + 1,
                                           PGA_Visible, BoxRows,
                                           TAG_DONE);
}

/*
 *              ScrollHorizontal(amount)
 *
 *              Scrolls the window horizontally to the left (-ve amount) or right
 *              (+ve amount) and does the minimum amount of refresh necessary to
 *              ensure the screen is correctly updated. i.e., we use ScrollRaster()
 *              to perform the bulk of the scroll.
 */
void ScrollHorizontal(int amount)
{
        struct RastPort *rport = MainWindow->RPort;
        int        oldpos = LeftCol;
        Event *firstevent;
        int x0, y0, x1, y1;
        int h0, h1;
        int dx;
        int dxwidth;
        int saveboxleft;
        int saveleftcol;
        int saverightcol;
        int saveboxcols;

        LeftCol += amount;
        InitMainMargins();
        if (!MainWindow || LeftCol == oldpos)
                return;

        UpdateMainHScroll();

        /*
         *              Okay, we're scrolling left or right, so do a ScrollRaster for
         *              part of the amount and then use ShowBuffer() to fill in the
         *              refreshed area. If there are no events or the buffer display
         *              is now out of date, do a full refresh instead.
         *
         *              We lock semaphores for the duration to ensure that the buffer
         *              doesn't change under our feet which might lead to mismatched
         *              lines of output.
         */
        LOCK_LAYERS;
        ObtainSemaphore(&BufSem);

        dx = LeftCol - oldpos;
        dxwidth = dx * BoxCharWidth;

        firstevent = HeadNode(&EventList);
        if (!firstevent || TopSeq < firstevent->seqnum || abs(dx) > (BoxCols/2)) {
                RedrawMainWindow();
                goto unlockall;
        }

        /*
         *              Calculate co-ordinates for scroll
         */
        h0 = BoxHeaderLine + BoxInTop - BoxBaseline;
        h1 = h0 + BoxCharHeight - 1;
        y0 = BoxInTop;
        y1 = BoxInTop + BoxInHeight - 1;
        x0 = BoxInLeft;
        x1 = BoxInLeft + (RightCol - LeftCol + 1) * BoxCharWidth - 1;

        if (GfxBase->LibNode.lib_Version >= 39) {
                /*
                 *              Do optimised flicker-free scroll under V39
                 */
                struct Hook *oldhook;

                oldhook = InstallLayerHook(MainWindow->WLayer, LAYERS_NOBACKFILL);
                ScrollRasterBF(rport, dxwidth, 0, x0, h0, x1, y1);
                InstallLayerHook(MainWindow->WLayer, oldhook);
        } else {
                /*
                 *              Do a somewhat flickering scroll under V37 -- unfortunately,
                 *              we have to do the scroll in two separate parts, to avoid
                 *              erasing part of the separator bar between the header and
                 *              the buffer text when it is scrolled
                 */
                ScrollRaster(rport, dxwidth, 0, x0, h0, x1, h1);
                ScrollRaster(rport, dxwidth, 0, x0, y0, x1, y1);
        }
        /*
         *              Finally, refresh the window section we just vacated by
         *              temporarily changing the margins
         */
        saveboxleft  = BoxInLeft;
        saveleftcol  = LeftCol;
        saverightcol = RightCol;
        saveboxcols  = BoxCols;

        if (dx < 0) {
                /*
                 *              Scrolling to left so refresh left side of window
                 */
                if (RightCol > -dx)
                        RightCol = LeftCol - dx - 1;
                BoxCols = RightCol - LeftCol + 1;
        } else {
                /*
                 *              Scrolling to right so refresh right side of window
                 */
                int offset = RightCol - LeftCol + 1 - dx;
                if (offset < 0)
                        goto done_scroll;

                BoxInLeft += offset * BoxCharWidth;
                LeftCol   += offset;
                BoxCols   -= offset;
        }
        if (RightCol < LeftCol)
                goto done_scroll;
        DrawHeaderLine();
        ShowBuffer(TopSeq, DISPLAY_ALL);

done_scroll:
        BoxInLeft = saveboxleft;
        LeftCol   = saveleftcol;
        RightCol  = saverightcol;
        BoxCols   = saveboxcols;
        
        /*
         *              All done, now we can release semaphores and exit
         */
unlockall:
        ReleaseSemaphore(&BufSem);
        UNLOCK_LAYERS;
        CheckForDirtyMainWindow();
}

/*
 *              DoArrowScrolling()
 *
 *              Scrolls window in the direction of the specified arrow
 *              (GID_UPARROW, GID_DOWNARROW, GID_LEFTARROW or GID_RIGHTARROW)
 *              and by the specified amount (usually 1 but sometimes more).
 */
void DoArrowScrolling(int arrowtype, int amount)
{
        int changedvert = 0;
        int horizadjust = 0;
        int newseq;

        /*
         *              If we're currently dragging columns, we ignore any attempts
         *              at scrolling since it can cause problems with the format
         *              being screwed up.
         */
        if (DraggingColumn || DraggingRow)
                return;

        switch (arrowtype) {
                case GID_UPARROW:
                        /*
                         *              Move up in a message
                         */
                        if (TopSeq > FirstSeq) {
                                changedvert = 1;
                                newseq          = TopSeq - amount;
                                if (newseq < 1)
                                        newseq = 1;
                        }
                        break;

                case GID_DOWNARROW:
                        /*              We do a clever optimisation here for power users:
                         *              if SnoopDos is Paused and is at the end of the
                         *              buffer, then clicking on the down arrow (normally
                         *              a no-op) will have the same effect as selecting
                         *              single step.
                         */
                        if (Paused && BottomSeq >= EndSeq)
                                SingleStep();

                        changedvert = 1;
                        newseq          = TopSeq + amount;
                        break;

                case GID_LEFTARROW:
                        if (LeftCol > 0)
                                horizadjust =- amount;
                        break;

                case GID_RIGHTARROW:
                        if (LeftCol + BoxCols <= BufferWidth)
                                horizadjust = amount;
                        break;
        }
        if (MainWindow) {
                if (horizadjust)
                        ScrollHorizontal(horizadjust);

                if (changedvert) {
                        InitMainMargins();
                        ShowBuffer(newseq, DISPLAY_QUICK);
                        UpdateMainVScroll();
                        CheckForDirtyMainWindow();
                }
        }
}

/*
 *              InvertColumn(pos)
 *
 *              Inverts the column drawn at the specified column position, relative
 *              to the left margin of the box (i.e. 0 ... BoxCols-1)
 *
 *              mode is INVERT_FULLBOX or INVERT_HEADER, depending on whether just
 *              the top of the column or the entire column is to be inverted. This
 *              allows for optimised updates if a column's position hasn't actually
 *              changed but the header still needs to be redrawn.
 */
void InvertColumn(int col, int mode)
{
        struct RastPort *rport = &InvertRP;
        int bfontbaseline          = BufferFont->tf_Baseline;
        int y1 = BoxHeaderLine - bfontbaseline;
        int y2 = (mode == INVERT_HEADER ? (BoxTop - 1) : (BoxInTop + BoxInHeight));

        if (col >= 0 && col < BoxCols) {
                int x = BoxInLeft + col * BoxCharWidth + BoxCharWidth / 2;

                RectFill(rport, x, y1, x+1, y2);
        }
}

/*
 *              StartDragCol(x)
 *
 *              Starts dragging a column displayed in the main window at absolute
 *              coordinate X.
 *
 *              Initialises all the variables associated with this and returns
 *              with dragging enabled.
 */
void StartDragCol(int x)
{
        EventFormat *ef;
        int clickcol;

        if (BufferEFormat[0].type == EF_END)    /* Skip empty event formats */
                return;

        /*
         *              Now scan event buffer and figure out which heading we match
         *              Note that the user must click on the event heading itself,
         *              or it won't count (i.e. clicking on space between columns
         *              doesn't work) -- this is to prevent confusion.
         */
        clickcol = LeftCol + ((x - BoxInLeft) / BoxCharWidth);
        {
                /*
                 *              Select the column to drag
                 */
                int colpos = 0;
                int len    = 0;

                for (ef = BufferEFormat; ef->type != EF_END; ef++) {
                        if (clickcol >= colpos && clickcol < (colpos + ef->width + 1))
                                break;
                        
                        colpos += ef->width + 1;
                }
                /*
                 *              Now check if the clickcol should affect the left or right
                 *              sides of the selected column. The leftmost column is a
                 *              special case (in that we always choose the righthand column
                 *              rather  the preceding lefthand, since you can't drag the
                 *              lefthand column any further to the left).
                 *
                 *              In the normal case, if there is any blank area to the right of
                 *              the column, then clicking in the blank area activates the
                 *              the right column adjustment, else the left column adjustment.
                 *              If there is no blank area, then clicking in the rightmost
                 *              column activates the right column, else the left column.
                 */
                if (ef->type != EF_END)
                        len = strlen(MSG(ef->titlemsgid));

                if (ef == BufferEFormat || ef->type == EF_END ||
                        clickcol >= (colpos + MIN(ef->width-1, len)))
                {
                        /*
                         *              Adjusting the righthand side of the selected column
                         */
                        if (ef->type == EF_END) {
                                DragEvent          = ef-1;
                                SelectEvent        = ef-1;
                                SelectStartCol = colpos - SelectEvent->width - 1;
                        } else {
                                SelectEvent    = ef;
                                SelectStartCol = colpos;
                                DragEvent                = ef;
                        }
                        DragOrigColWidth = DragEvent->width;
                        NextOrigColWidth = (ef+1)->width;
                } else {
                        /*
                         *              Adjusting the lefthand side of the selected column
                         */
                        SelectEvent      = ef;
                        SelectStartCol   = colpos;
                        DragEvent                = ef-1;
                        DragOrigColWidth = DragEvent->width;
                        NextOrigColWidth = ef->width;
                }
        }
        OrigLeftCol             = LeftCol;
        OrigBufWidth    = BufferWidth;
        ClickStartCol   = clickcol;
        ClickCurCol             = clickcol;
        DraggingColumn  = 1;
        MovedColumn             = 0;
        DrawHeaderLine();
        /*
         *              Now initialise the rastport we use for the inverted column
         *              (we use a separate rastport for speed; otherwise, we have
         *              to keep initialising it, which is rather slow.)
         */
        InvertRP = *(MainWindow->RPort);
        if (GfxBase->LibNode.lib_Version >= 39)
                SetWriteMask(&InvertRP, 1);
        else
                InvertRP.Mask = 1;
        SetAPen(&InvertRP, 1);
        SetBPen(&InvertRP, 0);
        SetDrMd(&InvertRP, COMPLEMENT);

        InvertColumn(SelectStartCol-LeftCol-1,                                  INVERT_FULLBOX);
        InvertColumn(SelectStartCol-LeftCol+SelectEvent->width, INVERT_FULLBOX);

        ReportMouse(TRUE, MainWindow);
        MainWindow->IDCMPFlags |= IDCMP_INTUITICKS;
        MainWindow->Flags      |= WFLG_RMBTRAP; /* Need to detect RMB for cancel */
}

/*
 *              SizeColumn(eventptr, newwidth, movetype)
 *
 *              Adjusts the width of the specified event entry to the new
 *              value given, performing any scrolling and updating of the
 *              main window that may be required.
 *
 *              movetype is MOVECOL_FREE to allow free movement between the
 *              columns to the left and right, or MOVECOL_FIXED to force
 *              the columns to the right to move along with the column being
 *              sized.
 */
void SizeColumn(EventFormat *ef, int newwidth, int movetype)
{
        EventFormat *nextef = ef+1;
        int oldleftcol          = LeftCol;
        int dwidth                      = newwidth - ef->width;
        int oldbufwidth         = BufferWidth;
        int oldpos1                     = SelectStartCol - LeftCol - 1;
        int oldpos2                     = SelectStartCol - LeftCol + SelectEvent->width;
        int newpos1;
        int newpos2;
        int mode1;
        int mode2;

        if (dwidth == 0)
                return;

        if (movetype == MOVECOL_FREE &&  nextef->type    != EF_END       &&
                                                                        (nextef+1)->type != EF_END) {
                /*
                 *              Bound movement so that we don't make the column to the
                 *              right of the main column exceed its bounds (in either
                 *              direction).
                 */
                int nextwidth = nextef->width - dwidth;

                if (nextwidth < 1)                              nextwidth = 1;
                if (nextwidth > MAX_FIELD_LEN)  nextwidth = MAX_FIELD_LEN;

                if (nextwidth != (nextef->width - dwidth)) {
                        dwidth   = nextef->width - nextwidth;
                        newwidth = ef->width + dwidth;
                }
                nextef->width = nextwidth;
        } else {
                /*
                 *              Dragging all the columns to the right of this one too so
                 *              update width to reflect the change
                 */
                BufferWidth   += dwidth;
        }
        ef->width      = newwidth;
        if (ef < SelectEvent)
                SelectStartCol += dwidth;
        ClickCurCol   += dwidth;
        ClearMainRHS   = 1;
        InitMainMargins();

        /*
         *              Check if the drag mouse position is off the right edge of the
         *              window; if it is, then scroll the window to keep it in view.
         */
        if (ClickCurCol >= (LeftCol + BoxCols)) {
                LeftCol = ClickCurCol - BoxCols + 1;
                InitMainMargins();
        }
        if (BufferWidth != oldbufwidth || LeftCol != oldleftcol)
                UpdateMainHScroll();

        newpos1 = SelectStartCol - LeftCol - 1;
        newpos2 = SelectStartCol - LeftCol + SelectEvent->width;

        mode1 = mode2 = INVERT_FULLBOX;

        if (newpos1 == oldpos1) mode1 = INVERT_HEADER;
        if (newpos2 == oldpos2) mode2 = INVERT_HEADER;

        InvertColumn(oldpos1, mode1);
        InvertColumn(oldpos2, mode2);
        DrawHeaderLine();
        InvertColumn(newpos1, mode1);
        InvertColumn(newpos2, mode2);

        MovedColumn = 1;        /* Show we updated a column for FinishDragCol() */
}

/*
 *              MoveColumn(xpos, movetype)
 *
 *              Mini function used when dragging columns with the mouse --
 *              changes the width of the current column to pixel position xpos,
 *              as much as possible.
 *
 *              movetype is either MOVECOL_FREE or MOVECOL_FIXED -- see the
 *              description of SizeColumn() for more details.
 */
void MoveColumn(int xpos, int movetype)
{
        int colpos = LeftCol + ((xpos - BoxInLeft) / BoxCharWidth);

        if (DragEvent) {
                int disp         = colpos - ClickStartCol;
                int newwidth = DragOrigColWidth + disp;
                int dwidth   = newwidth - DragEvent->width;

                /*
                 *              Now check to see if the change we're making will
                 *              cause us to scroll off the left edge of the window.
                 *              If it will, then reduce the change so that we scroll
                 *              at a steady 1-character rate (this works out around
                 *              10 chars a second, which should be fast enough).
                 */
                if (LeftCol > 0 && RightCol > (BufferWidth + dwidth))
                {
                        dwidth   = RightCol - BufferWidth;
                        newwidth = DragEvent->width + dwidth;
                }
                if (newwidth < 1)                               newwidth = 1;
                if (newwidth > MAX_FIELD_LEN)   newwidth = MAX_FIELD_LEN;

                SizeColumn(DragEvent, newwidth, movetype);
        }
}

/*
 *              FinishDragCol()
 *
 *              Finishes up a drag operation (just resets a few variables)
 */
void FinishDragCol(void)
{
        if (!DraggingColumn)
                return;

        ReportMouse(FALSE, MainWindow);
        MainWindow->IDCMPFlags &= ~IDCMP_INTUITICKS;
        MainWindow->Flags      &= ~WFLG_RMBTRAP;        /* Re-enable menus      */
        DraggingColumn = 0;

        /*
         *              We can optimise our update a bit by checking MovedColumn.
         *              This will only be true if the user actually resized one
         *              of the columns, so it saves us having to do a redraw
         *              if a column heading was accidentally clicked but not
         *              moved.
         */
        if (SelectEvent) {
                InvertColumn(SelectStartCol-LeftCol-1,                             INVERT_FULLBOX);
                InvertColumn(SelectStartCol-LeftCol+SelectEvent->width,INVERT_FULLBOX);
        }
        if (MovedColumn) {
                /*
                 *              Now update the settings window and format window with the
                 *              new format. This will also redraw the main window and
                 *              header line for us.
                 */
                BuildFormatString(BufferEFormat, BufFormat, MAX_FORM_LEN);
                InstallNewFormat(NEW_STRING);
        } else {
                /*
                 *              Nothing else to update, so just redraw the header line
                 *              (which will now be completely black again with no
                 *              highlighted item.)
                 */
                ClearMainRHS = 1;
                DrawHeaderLine();
        }
        /*
         *              Since we disable event scanning when columns are being dragged,
         *              we signal ourselves to check for new events in case any
         *              arrived while we were playing.
         */
        Signal(SysBase->ThisTask, NewEventMask);
}

/*
 *              StartDragRow(int y)
 *
 *              Starts dragging a highlight at y (pixel row) in the event output.
 *              This allows the user to use the mouse to highlight an entire line,
 *              making it easy to see all the information on the line related to
 *              the event. (This can sometimes be difficult to see in a normal
 *              window when there is a lot of white space between columns).
 *
 *              Initialises all the variables associated with this and returns
 *              with dragging enabled.
 */
void StartDragRow(int y)
{
        if (BufferEFormat[0].type == EF_END)    /* Skip empty event formats */
                return;

        SelectRow = 0;
        if (y > BoxInTop)
                SelectRow = (y - BoxInTop) / BoxSpacing;

        if (SelectRow >= BoxRows)
                SelectRow = BoxRows - 1;

        if ((TopSeq + SelectRow) > BottomSeq)
                SelectRow = BottomSeq - TopSeq;

        DraggingRow = 1;
        ReportMouse(TRUE, MainWindow);
        MainWindow->IDCMPFlags |= IDCMP_INTUITICKS;
        DrawSelectedLine(SelectRow, TRUE);
}

/*
 *              MoveToRow(y)
 *
 *              Moves the row highlight from its current position to a new position
 *              if necessary (doesn't render anything if the row position remains
 *              unchanged.)
 */
void MoveToRow(int y)
{
        int newrow = 0;

        if (y > BoxInTop)
                newrow = (y - BoxInTop) / BoxSpacing;

        if (newrow >= BoxRows)
                newrow = BoxRows - 1;

        if ((TopSeq + newrow) > BottomSeq)
                newrow = BottomSeq - TopSeq;

        if (newrow != SelectRow) {
                DrawSelectedLine(SelectRow, FALSE);
                DrawSelectedLine(newrow,    TRUE);
                SelectRow = newrow;
        }
}

/*
 *              FinishDragRow()
 *
 *              Stops highlighting the selected row and restores everything to normal
 */
void FinishDragRow(void)
{
        if (!DraggingRow)
                return;

        DrawSelectedLine(SelectRow, FALSE);
        ReportMouse(FALSE, MainWindow);
        MainWindow->IDCMPFlags &= ~IDCMP_INTUITICKS;
        DraggingRow = 0;

        /*
         *              It's possible that while we were playing with the row highlight,
         *              the entire buffer may have been swept away under our feet,
         *              leading to a slightly corrupt display (events out of sequence).
         *              We don't mind this while we're dragging the highlight, but we want
         *              to ensure it's sorted out properly afterwards. So, we do a quick
         *              check to see if the current display is no longer valid, and if so,
         *              then we redraw it. We need to handle new events first, else lots
         *              of important internal variables will be out of sync.
         */
        if (TopSeq < RealFirstSeq) {
                HandleNewEvents();
                ShowBuffer(TopSeq, DISPLAY_ALL);
        }
}

/*
 *              BeginResize()
 *
 *              Removes gadget list from window and does a few other things
 *              in preparation for a window resize occurring in the very
 *              near future.
 */
void BeginResize(void)
{
        if (!RemovedGadgets)
                RemoveGList(MainWindow, MainGadList, -1);
        RemovedGadgets = TRUE;
        if (BottomSeq >= EndSeq)
                AwaitingResize = RESIZE_BOTTOM;
        else
                AwaitingResize = RESIZE_MIDDLE;
}

/*
 *              EndResize(void)
 *
 *              Tiedies up after the window has been resized -- recalculates
 *              gadget list, refreshes display, etc.
 *
 *              Returns true for success, false for failure.
 */
int EndResize(void)
{
        if (MainWindow->Width  == CurWindowWidth &&
                MainWindow->Height == CurWindowHeight)
        {
                /*
                 *              Window didn't change size after all, so don't make any
                 *              changes, just restore the status quo
                 */
                if (RemovedGadgets)
                        AddGList(MainWindow, MainGadList, ~0, -1, NULL);
                AwaitingResize = RESIZE_DONE;
        } else {
                /*
                 *              Window was resized. If necessary, reposition the buffer
                 *              at the end of the new window (otherwise, if we were at
                 *              the end of the buffer and the window got smaller, there
                 *              would now be a few lines below the bottom of the window.)
                 */
                if (AwaitingResize == RESIZE_BOTTOM)
                        TopSeq = EndSeq;
                AwaitingResize = RESIZE_DONE;
                if (!RecalcMainWindow(MainWindow->Width,
                                                          MainWindow->Height,
                                                          REDRAW_GADGETS)) {
                        ShowError(MSG(MSG_ERROR_RESIZE));
                        return (0);
                }
                // RefreshWindowFrame(MainWindow);
        }
        RemovedGadgets = FALSE;

        /*
         *              Now, since we disabled monitoring of events during
         *              the resize, some may have accumulated that we missed.
         *              So we generate a fake new events signal to force a
         *              recheck in case anything did arrive.
         */
        Signal(SysBase->ThisTask, NewEventMask);
        return (1);
}

/*
 *              SetMainWindowWidth(int width)
 *
 *              Sets the width of the main window to the specified number of
 *              characters. Note that this call won't take effect immediately;
 *              it just tells Intuition to resize our window, which will generate
 *              a NEWSIZE event that we can trap.
 *
 *              Note: has no effect if main window isn't open.
 */
void SetMainWindowWidth(int colwidth)
{
        if (colwidth == 0)
                colwidth = BufferWidth;

        if (MainWindow) {
                int newleft       = MainWindow->LeftEdge;
                int newtop        = MainWindow->TopEdge;
                int newheight = MainWindow->Height;
                int newwidth  = MainWindow->Width - BoxWidth +
                                                colwidth*BoxCharWidth + 2 * BOX_LEFT_MARGIN + 5;

                if (newwidth < MainWindow->MinWidth)
                        newwidth = MainWindow->MinWidth;
                
                if ((newleft + newwidth) > ScreenWidth)
                        newleft  = ScreenWidth - newwidth;

                if (newleft < 0) {
                        newleft  = 0;
                        newwidth = ScreenWidth;
                }
                /*
                 *              Now check if we're so close to the edge of the screen that
                 *              a single additional character space would push us over the
                 *              edge .. if so, then expand the width to the full screen
                 *              width, since it looks neater when the window completely
                 *              fills the screen, instead of "almost" filling it.
                 */
                if (newleft == 0 && (newwidth + BoxCharWidth) > ScreenWidth)
                        newwidth = ScreenWidth;

                BeginResize();
                ChangeWindowBox(MainWindow, newleft, newtop, newwidth, newheight);
        }
}

/*
 *              HandleMainMsgs()
 *
 *              Processes all outstanding messages associated with the main SnoopDos
 *              window. To be called whenever we get a main window event.
 */
void HandleMainMsgs(void)
{
        static int activegad;                   /* Non-zero if gadget is depressed   */
        static int activekey;                   /* Keycode shortcut of activegad     */

        struct IntuiMessage *imsg;
        Settings *newsettings = NULL;
        int loadlastsaved = 0;
        int donemain = 0;
        int origtopseq;
        int updateslider;
        int changedhoriz;
        int changedvert;
        int amount;
        ULONG newval;
        int refreshtype         = CurSettings.SimpleRefresh;
        int newspacing          = BoxInterGap;
        int newstatus           = StatusLine;
        int newgadgets          = GadgetsLine;
        int newmode         = MonitorType;
        int newalign            = RightAligned;
        int dohide          = 0;

        if (!MainWindow)
                return;

        while ((imsg = GT_GetIMsg(MainWindowPort)) != NULL) {
                ULONG code  = imsg->Code;
                int shift   = imsg->Qualifier & IE_SHIFT;
                int altctrl = imsg->Qualifier & (IE_ALT | IE_CTRL);
                struct Gadget *gad;

                switch (imsg->Class) {
                        
                        case IDCMP_CLOSEWINDOW:
                                if (CurSettings.Setup.HideMethod == HIDE_NONE) {
                                        donemain = 1;
                                        QuitFlag = 1;
                                } else {
                                        dohide = 1;
                                }
                                break;

                        case IDCMP_REFRESHWINDOW:
                                /*
                                 *              Since the events currently displayed in our buffer
                                 *              window may have scrolled off the top of the buffer
                                 *              in the meantime, if we just blindly refresh, we
                                 *              might end up with the refreshed portions not matching
                                 *              the rest of the display. So, first try and redraw the
                                 *              buffer to ensure it contains current data. Normally,
                                 *              this will end up being a NOP.
                                 *
                                 *              See HandleNewEvents() for details as to why we need
                                 *              to lock our layers first.
                                 */
                                DB("Begin Refresh Semapore\n");
                                origtopseq = LastDrawnTopSeq;
                                LOCK_LAYERS;
                                ObtainSemaphore(&BufSem);
                                if (TopSeq != origtopseq)
                                        ShowBuffer(TopSeq, DISPLAY_ALL);
                                GT_BeginRefresh(MainWindow);
                                RedrawMainWindow();
                                GT_EndRefresh(MainWindow, TRUE);
                                DB("End refresh semaphore\n");
                                ReleaseSemaphore(&BufSem);
                                UNLOCK_LAYERS;
                                if (TopSeq != origtopseq)
                                        UpdateMainVScroll();
                                break;

                        case IDCMP_MENUHELP:
                                /*
                                 *              Display simple help for whatever menu was
                                 *              active when the HELP key was pressed. To decide
                                 *              which menu item to display help on, we simply
                                 *              build a description string from the menu name
                                 *              itself (replacing spaces with underscores and
                                 *              skipping over any dots).
                                 *
                                 *              We also strip out sub-menu items and just show
                                 *              the help for the parent item instead.
                                 */
                                if (code != MENUNULL) {
                                        static int menunames[] = {
                                                MSG_LINK_MENU_PROJECT,
                                                MSG_LINK_MENU_WINDOW,
                                                MSG_LINK_MENU_SETTINGS,
                                                MSG_LINK_MENU_BUFFER
                                        };
                                        char msgname[50];
                                        char *msg;
                                        int menunum = MENUNUM(code);
                                        
                                        if (menunum == NOMENU)
                                                msg = MSG(MSG_LINK_MAINWIN);
                                        else if (ITEMNUM(code) == NOITEM) {
                                                msg = MSG(menunames[menunum]);
                                        } else {
                                                struct MenuItem *item;

                                                item = ItemAddress(MainWinMenu,code | SHIFTSUB(NOSUB));
                                                if (!item || !(item->Flags & ITEMTEXT)) {
                                                        msg = MSG(menunames[menunum]);
                                                } else {
                                                        char *p;

                                                        msg = ((struct IntuiText *)item->ItemFill)->IText;
                                                        strcpy(msgname, "LINK Menu_");
                                                        p = msgname + strlen(msgname);

                                                        while (*msg) {
                                                                if (*msg == ' ' || *msg == '-')
                                                                        *p++ = '_';
                                                                else if (*msg != '.' && *msg != '?')
                                                                        *p++ = *msg;
                                                                msg++;
                                                        }
                                                        *p = '\0';
                                                        msg = msgname;
                                                }
                                        }
                                        ShowAGuide(msg);
                                }
                                /*
                                 *              It's possible the user may have played with some
                                 *              menu toggles while the menu was displayed ... to
                                 *              avoid us missing any items that were changed, we
                                 *              simply reset them all back to the original state.
                                 */             
                                SetMenuOptions();
                                break;

                        case IDCMP_MENUPICK:
                                while (code != MENUNULL) {
                                        struct MenuItem *item;
                                        int checked;
                                        
                                        item    = ItemAddress(MainWinMenu, code);
                                        checked = (item->Flags & CHECKED) == CHECKED;

                                        switch ((ULONG)GTMENUITEM_USERDATA(item)) {

                                                case MID_OPENLOG:
                                                        /*
                                                         *              Open a new logfile
                                                         */
                                                        if (SelectFile(ChosenLogName, ChosenLogName,
                                                                                   MainWindow, FILESEL_NEWLOGNAME))
                                                        {
                                                                if (!OpenLog(LOGMODE_PROMPT, ChosenLogName))
                                                                        ShowError(MSG(MSG_ERROR_STARTLOG),
                                                                                          DefaultLogName);
                                                        }
                                                        break;

                                                case MID_CLOSELOG:
                                                        CloseLog();
                                                        break;

                                                case MID_PAUSE:
                                                        if (checked)
                                                                newmode = MONITOR_PAUSED;
                                                        else
                                                                newmode = MONITOR_NORMAL;
                                                        break;

                                                case MID_DISABLE:
                                                        if (checked)
                                                                newmode = MONITOR_DISABLED;
                                                        else
                                                                newmode = MONITOR_NORMAL;
                                                        break;

                                                case MID_STEP:
                                                        SingleStep();
                                                        newmode = MonitorType;
                                                        break;

                                                case MID_CHANGEPRI:
                                                        /*
                                                         *              New task priority selected -- let's
                                                         *              read the priority directly from the
                                                         *              menu item itself.
                                                         */
                                                        if (checked) {
                                                                struct IntuiText *itext;
                                                                int newpri;
                                                                
                                                                itext  = (void *)item->ItemFill;
                                                                newpri = atoi(itext->IText);
                                                                SetTaskPri(SysBase->ThisTask, newpri);
                                                        }
                                                        break;

                                                case MID_HELP:
                                                        ShowAGuide(MSG(MSG_LINK_MAIN));
                                                        break;

                                                case MID_ABOUT:
                                                        ShowError(MSG(MSG_ABOUT_SNOOPDOS), Version+6,
                                                                          PORT_NAME);
                                                        break;

                                                case MID_HIDE:
                                                        dohide = 1;
                                                        break;

                                                case MID_QUIT:
                                                        donemain = 1;
                                                        QuitFlag = 1;
                                                        break;

                                                /*
                                                 *              Windows menu
                                                 */
                                                case MID_SETUP:         
                                                        if (shift && SetWindow)
                                                                CloseSettingsWindow();
                                                        else
                                                                OpenSettingsWindow();
                                                        break;

                                                case MID_FUNCTION:
                                                        if (shift && FuncWindow)
                                                                CloseFunctionWindow();
                                                        else
                                                                OpenFunctionWindow();
                                                        break;

                                                case MID_FORMAT:
                                                        if (shift && FormWindow)
                                                                CloseFormatWindow();
                                                        else
                                                                OpenFormatWindow();
                                                        break;

                                                case MID_SETWIDTH:
                                                {
                                                        struct IntuiText *itext = (void *)item->ItemFill;
                                                        int newwidth                    = atoi(itext->IText);
                                                                
                                                        SetMainWindowWidth(newwidth);
                                                        break;
                                                }
                                                case MID_ROWQUAL:
                                                {
                                                        /*
                                                         *              Changing to a new row qualifier
                                                         */
                                                        int newqual = SUBNUM(code);

                                                        if (newqual != NOSUB)
                                                                RowQual = newqual;
                                                        break;
                                                }
                                                case MID_STATUS:        newstatus  = checked;   break;
                                                case MID_GADGETS:       newgadgets = checked;   break;
                                                case MID_AUTO_OPEN:     AutoOpen   = checked;   break;
                                                case MID_DISABLE_HIDDEN: DisableOnHide = checked;break;

                                                case MID_SPACE_NONE:    if (checked) newspacing  = 0;
                                                                                                break;
                                                case MID_SPACE_1P:              if (checked) newspacing  = 1;
                                                                                                break;
                                                case MID_SPACE_2P:              if (checked) newspacing  = 2;
                                                                                                break;
                                                case MID_REF_SIMPLE:    if (checked) refreshtype = 1;
                                                                                                break;
                                                case MID_REF_SMART:             if (checked) refreshtype = 0;
                                                                                                break;
                                                case MID_ALIGN_LEFT:    if (checked) newalign    = 0;
                                                                                                break;
                                                case MID_ALIGN_RIGHT:   if (checked) newalign    = 1;
                                                                                                break;

                                                /*
                                                 *              Settings menu
                                                 */
                                                case MID_LOAD:
                                                        if (SelectFile(ConfigFileName, ConfigFileName,
                                                                                   MainWindow, FILESEL_LOADCONFIG))
                                                        {
                                                                /*
                                                                 *              This new settings file will becomoe
                                                                 *              the last saved file, so just load
                                                                 *              it in at the end instead
                                                                 */
                                                                newsettings       = 0;
                                                                loadlastsaved = 1;
                                                        }
                                                        break;
                                                        
                                                case MID_SAVE:
                                                        if (!SaveConfig(DefaultConfigName, SAVE_NOICON))
                                                                ShowError(MSG(MSG_ERROR_SAVING_SETTINGS),
                                                                                  DefaultConfigName);
                                                        break;
                                                
                                                case MID_SAVEAS:
                                                        if (SelectFile(ConfigFileName, ConfigFileName,
                                                                                   MainWindow, FILESEL_SAVECONFIG))
                                                        {
                                                                if (!SaveConfig(ConfigFileName, SAVE_ICON)) {
                                                                        ShowError(MSG(MSG_ERROR_SAVING_SETTINGS),
                                                                                          ConfigFileName);
                                                                }
                                                        }
                                                        break;
                                                        
                                                case MID_RESET:
                                                        newsettings = &DefaultSettings;
                                                        break;

                                                case MID_RESTORE:
                                                        newsettings = &RestoreSettings;
                                                        break;

                                                case MID_LASTSAVED:
                                                        /*
                                                         *              If we successfully read in the defaults
                                                         *              file _or_ saved out a settings file, then
                                                         *              go ahead and read back in the last saved
                                                         *              file; otherwise, it's the very first
                                                         *              time we've ever been run by this user,
                                                         *              so just restore the factory settings
                                                         *              instead of producing an error message.
                                                         */
                                                        if (GotLastSaved) {
                                                                newsettings   = 0;
                                                                loadlastsaved = 1;
                                                        } else {
                                                                newsettings   = &DefaultSettings;
                                                        }
                                                        break;

                                                case MID_ICONS:
                                                        CreateIcons     = checked;
                                                        break;

                                                /*
                                                 *              Buffer menu
                                                 */
                                                case MID_COPYWIN:
                                                        /*
                                                         *              Copy current window to clipboard
                                                         */
                                                        DisableAllWindows();
                                                        if (!SaveBuffer(SAVEBUF_WINDOW, SAVEBUF_CLIPBOARD,
                                                                                        SAVEBUF_OVERWRITE))
                                                                ShowError(MSG(MSG_ERROR_COPY_WIN_TO_CLIP));
                                                        EnableAllWindows();
                                                        break;

                                                case MID_COPYBUF:
                                                        /*
                                                         *              Copy current buffer to clipboard
                                                         */
                                                        DisableAllWindows();
                                                        if (!SaveBuffer(SAVEBUF_ALL, SAVEBUF_CLIPBOARD,
                                                                                        SAVEBUF_OVERWRITE))
                                                                ShowError(MSG(MSG_ERROR_COPY_ALL_TO_CLIP));
                                                        EnableAllWindows();
                                                        break;

                                                case MID_SAVEWIN:
                                                        if (SelectFile(BufferFileName, BufferFileName,
                                                                                   MainWindow, FILESEL_SAVEWINDOW))
                                                        {
                                                                DisableAllWindows();
                                                                if (!SaveBuffer(SAVEBUF_WINDOW, BufferFileName,
                                                                                                SAVEBUF_PROMPT))
                                                                {
                                                                        ShowError(MSG(MSG_ERROR_SAVING_WINDOW),
                                                                                          BufferFileName);
                                                                }
                                                                EnableAllWindows();
                                                        }
                                                        break;
                                                        
                                                case MID_SAVEBUF:
                                                        if (SelectFile(BufferFileName, BufferFileName,
                                                                                   MainWindow, FILESEL_SAVEBUFFER))
                                                        {
                                                                DisableAllWindows();
                                                                if (!SaveBuffer(SAVEBUF_ALL, BufferFileName,
                                                                                                SAVEBUF_PROMPT))
                                                                {
                                                                        ShowError(MSG(MSG_ERROR_SAVING_BUFFER),
                                                                                          BufferFileName);
                                                                }
                                                                EnableAllWindows();
                                                        }
                                                        break;

                                                case MID_CLEARBUF:
                                                        ClearWindowBuffer();
                                                        break;

                                        }
                                        code = item->NextSelect;
                                }
                                break;

                        case IDCMP_IDCMPUPDATE:
                                /*
                                 *              Handle BOOPSI slider movement. BOOPSI arrow 
                                 *              gadgets are no longer handled here but are
                                 *              treated as ordinary button gadgets since
                                 *              otherwise, we can't get the desired behaviour
                                 *              (with a delay after the first click before it
                                 *              starts repeating).
                                 */
                                changedhoriz = 0;
                                changedvert  = 0;
                                updateslider = 0;

                                GetAttr(PGA_Top, (Object *)Gadget[GID_HSCROLLER], &newval);
                                if (newval != LeftCol)
                                        ScrollHorizontal(newval - LeftCol);

                                GetAttr(PGA_Top, (Object *)Gadget[GID_VSCROLLER], &newval);
                                if (newval != (TopSeq - FirstSeq)) {
                                        ShowBuffer(FirstSeq + newval, DISPLAY_QUICK);
                                        CheckForDirtyMainWindow();
                                }
                                break;

                        case IDCMP_INTUITICKS:
                                if (ScrollDirection) {
                                        /*
                                         *              Handle possible arrow gadget scrolling
                                         */
                                        if (ScrollCount >= 3) {
                                                if (Gadget[ScrollDirection]->Flags & GFLG_SELECTED)
                                                        DoArrowScrolling(ScrollDirection, 1);
                                        } else {
                                                ScrollCount++;
                                        }
                                }
                                if (DraggingColumn) {
                                        /*
                                         *              Handle horizontal scrolling to left or right;
                                         *              we just treat it as a normal mouse move -- in
                                         *              the general case, the displacement will be
                                         *              zero, so no updating will take place.
                                         */
                                        MoveColumn(imsg->MouseX,
                                                           shift ? MOVECOL_FIXED : MOVECOL_FREE);
                                }
                                if (DraggingRow) {
                                        /*
                                         *              Dragging a highlight across rows, so we want to
                                         *              automatically scroll if we are off the top or
                                         *              bottom of the window. We also check to make sure
                                         *              we're not off the top or bottom of the buffer
                                         *              (no harm would come if we were, but it would
                                         *              lead to unnecessary flashing of the display
                                         *              highlight)
                                         */
                                        int scrolldir = 0;

                                        if (imsg->MouseY < BoxInTop && TopSeq > FirstSeq)
                                        {
                                                scrolldir = GID_UPARROW;
                                        } else if (imsg->MouseY > (BoxInTop + BoxInHeight) &&
                                                           BottomSeq    < EndSeq)
                                        {
                                                scrolldir = GID_DOWNARROW;
                                        }
                                        if (scrolldir) {
                                                FinishDragRow();
                                                DoArrowScrolling(scrolldir, 1);
                                                StartDragRow(imsg->MouseY);
                                        }
                                }
                                break;

                        case IDCMP_MOUSEBUTTONS:
                        {
                                static int lastrmb;

                                int lmb = imsg->Qualifier & IEQUALIFIER_LEFTBUTTON;
                                int rmb = imsg->Qualifier & IEQUALIFIER_RBUTTON;

                                if (lmb && !rmb && !lastrmb) {
                                        static WORD lastx, lasty;
                                        static ULONG lastsecs, lastmicros;
                                        int curx = imsg->MouseX;
                                        int cury = imsg->MouseY;

                                        if (curx >= (lastx - 1) && curx <= (lastx + 1) &&
                                                cury >= (lasty - 1) && cury <= (lasty + 1))
                                        {
                                                if (DoubleClick(lastsecs, lastmicros,
                                                                                imsg->Seconds, imsg->Micros)) {
                                                        /*
                                                         *              Got a double-click. Now check if it's
                                                         *              in the window header area. If so,
                                                         *              open or close the format window.
                                                         */
                                                        if (cury < BoxTop) {
                                                                if (shift && FormWindow)
                                                                        CloseFormatWindow();
                                                                else
                                                                        OpenFormatWindow();
                                                                break;
                                                        }
                                                }
                                        }
                                        lastx           = curx;
                                        lasty           = cury;
                                        lastsecs        = imsg->Seconds;
                                        lastmicros      = imsg->Micros;
                                }
                                if (DraggingColumn) {
                                        if (lmb == 0) {
                                                /*
                                                 *              We've finished dragging our column
                                                 */
                                                FinishDragCol();
                                        } else if (rmb) {
                                                /*
                                                 *              User clicked right button to cancel the
                                                 *              drag operation, so restore column width
                                                 *              to its original position and cancel drag.
                                                 */
cancel_dragging_column:
                                                /* Stop highlight flash when restoring orig header */


                                                if (DraggingColumn) {
                                                        EventFormat *nextev = DragEvent + 1;

                                                        /*
                                                         *              We invert the columns now and then
                                                         *              set SelectEvent to NULL to let
                                                         *              FinishDragCol() know we have already
                                                         *              unhighlighted them (this is because
                                                         *              we're hacking the window format
                                                         *              directly).
                                                         */
                                                        InvertColumn(SelectStartCol - LeftCol - 1,
                                                                                 INVERT_FULLBOX);
                                                        InvertColumn(SelectStartCol - LeftCol +
                                                                                 SelectEvent->width, INVERT_FULLBOX);
                                                        SelectEvent = NULL;
                                                        DragEvent->width = DragOrigColWidth;
                                                        if (nextev->type != EF_END)
                                                                nextev->width = NextOrigColWidth;
                                                        BufferWidth = OrigBufWidth;
                                                        if (LeftCol != OrigLeftCol) {
                                                                LeftCol = OrigLeftCol;
                                                                InitMainMargins();
                                                                UpdateMainHScroll();
                                                        }
                                                        FinishDragCol();
                                                }
                                                lastrmb = 1;
                                                break;
                                        }
                                } else if (lmb && !lastrmb) {
                                        /*
                                         *              Could be starting to drag a new column or row!
                                         *              Check bounds to see if we are. If lastrmb was
                                         *              true, then user has just released right mouse
                                         *              button but hasn't yet released left mouse button,
                                         *              so we ignore this attempt..
                                         */
                                        int x = imsg->MouseX;
                                        int y = imsg->MouseY;

                                        if (y < BoxTop && x >= BoxInLeft &&
                                                                          x < (BoxInLeft + BoxInWidth)) {
                                                StartDragCol(x);
                                        } else if (y > BoxInTop && y < (BoxInTop + BoxInHeight)) {
                                                /*
                                                 *              Now check if correct qualifier is held down
                                                 *              to activate the row selection. We need to use
                                                 *              a slighty tricky boolean equation to handle
                                                 *              ROWQUAL_NONE, wher 
                                                 */
                                                int qual = imsg->Qualifier & IE_ALL;

                                                if (RowQual == ROWQUAL_ANY                                                       ||
                                                    (RowQual == ROWQUAL_NONE && !qual)                           ||
                                                    (qual && (qual & RowQualTable[RowQual]) == qual))
                                                {
                                                        StartDragRow(y);
                                                }
                                        }
                                }
                                if (DraggingRow && !lmb)
                                        FinishDragRow();

                                lastrmb = rmb;
                                break;
                        }

                        case IDCMP_MOUSEMOVE:
                                if (DraggingColumn) {
                                        MoveColumn(imsg->MouseX,
                                                           shift ? MOVECOL_FIXED : MOVECOL_FREE);
                                }
                                if (DraggingRow)
                                        MoveToRow(imsg->MouseY);
                                break;

                        case IDCMP_GADGETUP:
                        case IDCMP_GADGETDOWN:
                                gad   = (struct Gadget *)imsg->IAddress;

handle_maingadgets:
                                switch (gad->GadgetID) {
                                        case GID_UPARROW:
                                        case GID_DOWNARROW:
                                        case GID_LEFTARROW:
                                        case GID_RIGHTARROW:
                                                if (imsg->Class == IDCMP_GADGETDOWN) {
                                                        /*
                                                         *              User clicked on a scroll button so
                                                         *              kick-off the scrolling.
                                                         */
                                                        ScrollDirection = gad->GadgetID;
                                                        DoArrowScrolling(ScrollDirection, 1);
                                                        MainWindow->IDCMPFlags |= IDCMP_INTUITICKS;
                                                        ScrollCount = 0;
                                                } else {
                                                        /*
                                                         *              User let go of a scroll button so turn
                                                         *              off scrolling.
                                                         */
                                                        ScrollDirection = 0;
                                                        MainWindow->IDCMPFlags &= ~IDCMP_INTUITICKS;
                                                }
                                                break;

                                        case GID_SETUP:
                                                if (shift && SetWindow)
                                                        CloseSettingsWindow();
                                                else
                                                        OpenSettingsWindow();
                                                break;

                                        case GID_FUNCTION:
                                                if (shift && FuncWindow)
                                                        CloseFunctionWindow();
                                                else
                                                        OpenFunctionWindow();
                                                break;

                                        case GID_SAVESET:
                                                /*
                                                 *              Save settings to configuration file
                                                 */
                                                ShowStatus(MSG(MSG_STATUS_SAVESET));
                                                DisableAllWindows();
                                                if (!SaveConfig(DefaultConfigName, SAVE_NOICON))
                                                        ShowError(MSG(MSG_ERROR_SAVING_SETTINGS),
                                                                          DefaultConfigName);
                                                EnableAllWindows();
                                                UpdateStatus();
                                                break;

                                        case GID_PAUSE:
                                                if (gad->Flags & GFLG_SELECTED)
                                                        newmode = MONITOR_PAUSED;
                                                else
                                                        newmode = MONITOR_NORMAL;
                                                break;

                                        case GID_DISABLE:
                                                if (gad->Flags & GFLG_SELECTED)
                                                        newmode = MONITOR_DISABLED;
                                                else
                                                        newmode = MONITOR_NORMAL;
                                                break;

                                        case GID_APPENDLOG:
                                        case GID_STARTLOG:
                                        case GID_SERIALLOG:
                                                if (!OpenLog(DefaultLogMode, DefaultLogName))
                                                        ShowError(MSG(MSG_ERROR_STARTLOG), DefaultLogName);
                                                break;

                                        case GID_OPENLOG:
#if 0
                                                /*
                                                 *              Quick and dirty benchmark code to scroll
                                                 *              the buffer 10 times to check speed
                                                 */
                                                { int i, l;
                                                
                                                  for (i = 0; i < 10; i++)
                                                    for (l = FirstSeq; l < EndSeq; l += BoxRows)
                                                          ShowBuffer(l, DISPLAY_QUICK);
                                                  break;
                                                }
#endif
                                                /*
                                                 *              Back to normal: open a new log file
                                                 */
                                                if (SelectFile(ChosenLogName, ChosenLogName,
                                                                           MainWindow, FILESEL_NEWLOGNAME))
                                                {
                                                        OpenLog(LOGMODE_PROMPT, ChosenLogName);
                                                }
                                                break;
                                                
                                        case GID_CLOSELOG:
                                                CloseLog();
                                                break;

                                        case GID_HIDE:
                                                dohide = 1;
                                                break;

                                        case GID_QUIT:
                                                donemain = 1;
                                                QuitFlag = 1;
                                                break;
                                }
                                break;

                        case IDCMP_SIZEVERIFY:
                                BeginResize();
                                break;

                        case IDCMP_CHANGEWINDOW:
                        case IDCMP_NEWSIZE:
                                if (!EndResize())
                                        donemain = 1;
                                break;
                        
                        case IDCMP_INACTIVEWINDOW:
                                /*
                                 *              If window becomes inactive, cancel any column
                                 *              drag currently in operation, since it could
                                 *              confuse things otherwise when the window is
                                 *              later re-activated.
                                 */
                                if (activegad) {
                                        ShowGadget(MainWindow, Gadget[activegad], GADGET_UP);
                                        activegad = 0;
                                }
                                if (DraggingRow)
                                        FinishDragRow();

                                if (DraggingColumn)
                                        goto cancel_dragging_column;
                                break;

                        case IDCMP_RAWKEY:
                        {
                                /*
                                 *              Handle all keyboard shortcuts
                                 */
                                int upstroke = imsg->Code & 0x80;
                                int keypress = ConvertIMsgToChar(imsg);
                                UBYTE gadid  = MainKeyboard[keypress];

                                if (activegad) {
                                        /*
                                         *              We're releasing a gadget that was pressed
                                         *              earlier, so handle it now
                                         */
                                        int samekey = (imsg->Code & 0x7f) == activekey;

                                        if (samekey && !upstroke)       /* Ignore repeated keys */
                                                break;

                                        ShowGadget(MainWindow, Gadget[activegad], GADGET_UP);
                                        if (samekey) {
                                                /*
                                                 *              Just released the key that was originally
                                                 *              pressed for this gadget, so now go and
                                                 *              handle the gadget action.
                                                 */
                                                gadid     = activegad;
                                                gad       = Gadget[gadid];
                                                activegad = 0;
                                                goto handle_maingadgets;
                                        }
                                        /*
                                         *              If the above check didn't kick in, it means
                                         *              we got a downpress of a different key, so
                                         *              disable the gadget for this key and continue
                                         *              to press the other key.
                                         */
                                        activegad = 0;
                                }

                                /*
                                 *              Handle simple keyboard input (i.e. vanilla keys)
                                 *
                                 *              In addition to the gadget shortcuts in MainKeyboard[]
                                 *              we also use TAB to unconditionally pause, and Space/
                                 *              Return to singlestep if we're already paused.
                                 *
                                 *              (If we made Space always single step, then if someone
                                 *              hit it by accident when the SnoopDos window was open,
                                 *              it would go into pause mode which would be annoying.)
                                 */
                                if (gadid) {
                                        /*
                                         *              Handle a gadget shortcut
                                         */
                                        gad = Gadget[gadid];
                                        if (gadid == GID_PAUSE || gadid == GID_DISABLE) {
                                                /*
                                                 *              PAUSE and DISABLE are toggle gadgets
                                                 *              which will highlight in the gadget code.
                                                 *              So, we handle them immediately, to give
                                                 *              instant feedback.
                                                 */
                                                gad->Flags ^= GFLG_SELECTED;
                                                goto handle_maingadgets;
                                        } else {
                                                /*
                                                 *              Now a slight problem ... since our logfile
                                                 *              gadget can cycle between multiple values,
                                                 *              we had better make sure that the same
                                                 *              shortcut will activate whichever one is
                                                 *              currently displayed rather than the
                                                 *              default hotkey (since all three opens
                                                 *              may share the same hotkey). If the logfile
                                                 *              is already open, we do nothing.
                                                 */
                                                if (gadid == GID_OPENLOG  || gadid == GID_APPENDLOG ||
                                                        gadid == GID_STARTLOG || gadid == GID_SERIALLOG)
                                                {
                                                        if (LogActive)
                                                                break;

                                                        gadid = LogButtonID;
                                                        gad   = Gadget[gadid];
                                                }

                                                /*
                                                 *              All these gadget are button gadgets so we
                                                 *              just highlight them now, and set the
                                                 *              activegad flag -- on the upstroke, they'll
                                                 *              be released.
                                                 */
                                                ShowGadget(MainWindow, gad, GADGET_DOWN);
                                                activegad = gadid;
                                                activekey = imsg->Code;
                                        }
                                        break;
                                }

                                /*
                                 *              Didn't match one of the gadget hotkeys,
                                 *              now check for some other special keys
                                 */
                                switch (keypress) {
                                        case 0x0D:              /* Return */
                                                DoArrowScrolling(GID_DOWNARROW, 1);
                                                break;

                                        case ' ':               /* Space */
                                                if (DraggingColumn) {
                                                        /*
                                                         *              Redraw window with current format
                                                         *              to date so user can see how it looks
                                                         */
                                                        InvertColumn(SelectStartCol - LeftCol - 1,
                                                                                 INVERT_FULLBOX);
                                                        InvertColumn(SelectStartCol - LeftCol +
                                                                                 SelectEvent->width, INVERT_FULLBOX);
                                                        BuildFormatString(BufferEFormat,
                                                                                          BufFormat, MAX_FORM_LEN);
                                                        InstallNewFormat(NEW_STRING);
                                                        InvertColumn(SelectStartCol - LeftCol - 1,
                                                                                 INVERT_FULLBOX);
                                                        InvertColumn(SelectStartCol - LeftCol +
                                                                                 SelectEvent->width, INVERT_FULLBOX);
                                                } else if (MonitorType == MONITOR_PAUSED) {
                                                        /*
                                                         *              Singlestep, but only if we were
                                                         *              already paused.
                                                         */
                                                        SingleStep();
                                                        newmode = MonitorType;
                                                }
                                                break;

                                        case 0x09:              /* Tab */
                                                SingleStep();
                                                newmode = MonitorType;
                                                break;

                                        case 0x03:              /* CTRL-C -- quit               */
                                                gadid = GID_QUIT;
                                                ShowGadget(MainWindow, Gadget[gadid], GADGET_DOWN);
                                                activegad = gadid;
                                                activekey = imsg->Code;
                                                break;
                                                
                                        case 0x04:              /* CTRL-D -- disable    */
                                                newmode = MONITOR_DISABLED;
                                                break;
                                        
                                        case 0x05:              /* CTRL-E -- enable             */
                                                newmode = MONITOR_NORMAL;
                                                break;

                                        case 0x06:              /* CTRL-F -- to front   */
                                                OpenMainWindow();
                                                break;

                                        case '\033':    /* ESC -- cancel column drag */
                                                if (DraggingColumn)
                                                        goto cancel_dragging_column;
                                                break;
                                }

                                /*
                                 *              Finally, finally, we do a quick check to see
                                 *              if the keystroke matches the format editor
                                 *              menu shortcut; if it does, we open the format
                                 *              editor. (If this key has been used by one of
                                 *              the other gadgets, e.g. due to localisation,
                                 *              then we will never get here, so not a problem.)
                                 *
                                 *              As with the settings and functions windows, if
                                 *              the keypress is shifted, then we close the
                                 *              window if it was already open.
                                 */
                                if ((keypress & 0x5F) == (*MSG(MSG_WINDOWS_FORMAT) & 0x5F)) {
                                        if (shift && FormWindow)
                                                CloseFormatWindow();
                                        else
                                                OpenFormatWindow();
                                        break;
                                }

                                /*
                                 *              Handle cursor keys etc. We allow scrolling by
                                 *              varying numbers of steps, depending on the
                                 *              qualifier key used.
                                 */
                                amount  = 1;
                                switch (imsg->Code) {
                                        case CURSORUP:
                                                if (shift)   amount = BoxRows;
                                                if (altctrl) amount = TopSeq - FirstSeq;
                                                DoArrowScrolling(GID_UPARROW, amount);
                                                break;

                                        case CURSORDOWN:
                                                if (shift)   amount = BoxRows;
                                                if (altctrl) amount = EndSeq - TopSeq;
                                                DoArrowScrolling(GID_DOWNARROW, amount);
                                                break;

                                        case CURSORLEFT:
                                                if (shift)   amount = HSCROLL_SHIFT_JUMP;
                                                if (altctrl) amount = LeftCol;
                                                DoArrowScrolling(GID_LEFTARROW, amount);
                                                break;

                                        case CURSORRIGHT:
                                                if (shift)   amount = HSCROLL_SHIFT_JUMP;
                                                if (altctrl) amount = BufferWidth - LeftCol;
                                                DoArrowScrolling(GID_RIGHTARROW, amount);
                                                break;

                                        case TABKEY:
                                                if (shift)
                                                        newmode = MONITOR_NORMAL;
                                                break;

                                        case HELPKEY:
                                                ShowAGuide(MSG(MSG_LINK_MAINWIN));
                                                break;
                                }
                                break;
                        }
                }
                GT_ReplyIMsg(imsg);
        }
        if (newmode != MonitorType)
                SetMonitorMode(newmode);

        if (dohide)
                HideSnoopDos();
        else if (donemain)
                CloseMainWindow();
        else if (refreshtype != CurSettings.SimpleRefresh) {
                BoxInterGap = newspacing;
                StatusLine  = newstatus;
                GadgetsLine = newgadgets;
                CurSettings.SimpleRefresh = refreshtype;
                ReOpenMainWindow();
        } else if (newsettings) {
                InstallSettings(newsettings, SET_ALL);
        } else if (loadlastsaved) {
                /*
                 *              We read the last-saved configuration from a file rather
                 *              than just from a memory copy so that the user can edit
                 *              the disk version by hand first if they so choose
                 */
                if (!LoadConfig(ConfigFileName, MODE_INTERNAL, NULL))
                        ShowError(MSG(MSG_ERROR_LOADING_SETTINGS), ConfigFileName);
        } else {
                if (newspacing != BoxInterGap)
                        SetTextSpacing(newspacing);
                else if (newalign != RightAligned) {
                        RightAligned = newalign;
                        ShowBuffer(TopSeq, DISPLAY_ALL);
                }
                if (newgadgets != GadgetsLine || newstatus != StatusLine) {
                        StatusLine  = newstatus;
                        GadgetsLine = newgadgets;

                        if (!RecalcMainWindow(MainWindow->Width, MainWindow->Height,
                                                                  REDRAW_GADGETS))
                                ShowError(MSG(MSG_ERROR_RESIZE));
                }
        }
}

/*
 *              DrawSelectedLine(row, highlight)
 *
 *              Calculates what event is currently positioned at the specified row
 *              in the main window and redraws that row either highlighted or
 *              unhighlighted. If the event is now complete, its state is updated
 *              accordingly. If no event can be found at that line, then the nearest
 *              row is highlighted instead.
 *
 *              If highlight is true, the row is highlighted, else it is unhighlighted.
 */
void DrawSelectedLine(int row, int highlight)
{
        struct RastPort *rport = MainWindow->RPort;
        Event *ev;
        Event *firstevent;
        int   firstseq;
        int   fillpen = 0;
        int   textpen = 1;
        int   i;

        if (IsListEmpty(&EventList) || BufferEFormat[0].type == EF_END)
                return;

        /*
         *              First, find the event
         */
        LOCK_LAYERS;
        ObtainSemaphore(&BufSem);
        firstevent = HeadNode(&EventList);
        firstseq   = firstevent->seqnum;
        ev         = TopEvent;
        if (TopSeq < firstseq)
                ev = firstevent;
        
        for (i = 0; i < row && ev != TailNode(&EventList); i++, ev = NextNode(ev))
                ;
        if (ev->status == ES_READY)
                ev->status = ES_ACCEPTED;

        FormatEvent(BufferEFormat, ev, BufferLine, LeftCol, LeftCol + BoxCols - 1);
        /*
         *              Now setup rastport according to whether or not we want it
         *              highlighted
         */
        if (highlight) {
                fillpen = ScreenDI->dri_Pens[FILLPEN];
                textpen = ScreenDI->dri_Pens[FILLTEXTPEN];
        }
        SetAPen(rport, textpen);
        SetBPen(rport, fillpen);
        SetDrMd(rport, JAM2);
        Move(rport, BoxInLeft, BoxBaseline + i * BoxSpacing);
        Text(rport, BufferLine, BoxCols);
        ReleaseSemaphore(&BufSem);
        UNLOCK_LAYERS;
}

/*
 *              OutputBufLine(event, row)
 *
 *              Outputs the specified event to the buffer display at the given
 *              row, using the current format, left and right col indicators.
 *              See also DrawSelectedLine() above.
 */
void OutputBufLine(Event *event, int row)
{
        struct RastPort *rport = MainWindow->RPort;

        if (event->status == ES_READY)
                event->status = ES_ACCEPTED;

        FormatEvent(BufferEFormat, event, BufferLine, LeftCol, RightCol);
        Move(rport, BoxInLeft, BoxBaseline + row*BoxSpacing);
        Text(rport, BufferLine, RightCol - LeftCol + 1);
}

/*
 *              ClearWindowBuffer()
 *
 *              Clears the buffer area on the screen, and empties the internal
 *              buffer as well (called by the Clear Buffer menu option).
 */
void ClearWindowBuffer(void)
{
        ClearBuffer();
        if (MainWindow) {
                struct RastPort *rport = MainWindow->RPort;

                /*
                 *              Erase strip to right of our rendered text
                 */
                SetAPen(rport, 0);
                SetDrMd(rport, JAM1);
                RectFill(rport, BoxInLeft, BoxInTop,
                                                BoxInLeft+BoxInWidth-1, BoxInTop+BoxInHeight-1);

                UpdateMainVScroll();
        }
        ClearMainRHS = 0;
}

/*
 *              ShowBuffer(seqnum, displaytype)
 *
 *              Adjusts the buffer display so that it begins at event numbered
 *              seqnum. If more than half of the needed events are currently
 *              on display, the buffer is scrolled to put them in the correct
 *              position. Any lines not currently on display are redrawn.
 *
 *              Updates TopSeq and BottomSeq accordingly.
 *
 *              Also does validity checking to ensure that if our current set of
 *              events has scrolled off the top of the buffer, we will still be
 *              displayed.
 *
 *              Displaytype is DISPLAY_ALL or DISPLAY_QUICK, to force either
 *              the entire buffer to be redrawn dumbly at the new position, or
 *              to optimise it by scrolling, only drawing changed items, etc.
 *
 *              If displaytype is DISPLAY_NONE, then all the Seq variables are
 *              recalculated to take account of events scrolling off the top
 *              etc. but no new output is displayed. This allows us to update
 *              the scrollbar without interrupting the current screen display
 *              while the user is reading it.
 *
 *              Note: it is save to call ShowBuffer with DISPLAY_NONE when the
 *              main window is closed, but for all the others, it should be open.
 */
void ShowBuffer(LONG seqnum, int displaytype)
{
        struct RastPort *rport;
        LONG  oldtopseq = TopSeq;               /* Saved old top position                               */
        LONG  firstseq;                                 /* First sequence number on list                */
        Event *firstevent;                              /* First event on list                                  */
        Event *topev;                                   /* New top event displayed on screen    */
        int   row = 0;                                  /* Current output row in buffer                 */
        int   scrollthresh;                             /* If moving less than this, use scroll */

        /*
         *              If there is a lot of activity, then sometimes we can get
         *              output messages in between an IDCMP_VERIFY and an IDCMP_NEWSIZE
         *              which can lead to screen artefacts appearing in the border.
         *              One way to avoid these is to always refresh the borders when
         *              we get an IDCMP_NEWSIZE, but a neater way is to suspend output
         *              while we're between the two types of message -- this is less
         *              visually jarring on the user.
         */
        if (AwaitingResize != RESIZE_DONE)
                displaytype = DISPLAY_NONE;

        /*
         *              Important that we lock our layers -- see HandleNewEvents()
         *              for a more detailed explanation of why.
         */
        DB("ShowBuffer-Semaphore\n");
        LOCK_LAYERS;
        ObtainSemaphore(&BufSem);

        if (IsListEmpty(&EventList) ||
                                        ((Event *)HeadNode(&EventList))->status == ES_CREATING)
                goto done_show_buffer;

        firstevent = HeadNode(&EventList);
        firstseq   = firstevent->seqnum;

        if (TopSeq < firstseq) {
                TopEvent    = firstevent;
                TopSeq      = firstseq;
                BottomSeq   = firstseq;
                /*
                 *              The events currently on display in the window must have
                 *              scrolled off the top of the buffer, so we can't assume
                 *              any of them still exist. So, force a full redraw instead.
                 */
                if (displaytype == DISPLAY_QUICK)
                        displaytype = DISPLAY_ALL;
        }
        if (EndSeq < firstseq) {
                EndSeq          = firstseq;
                EndEvent        = firstevent;
        }
        if (seqnum < firstseq)  seqnum = firstseq;
        if (seqnum > EndSeq)    seqnum = EndSeq;
        
        /*
         *              Work out if we need to scan forwards or backwards from the
         *              current event at the top of the screen
         */
        if (seqnum <= TopSeq) {
                /*
                 *              Scanning backwards from current top of screen. Now see if
                 *              it would be faster to scan forward from the start of the
                 *              list.
                 */
                if ((TopSeq - seqnum) < (seqnum - firstseq)) {
                        for (topev = TopEvent;
                                        topev != HeadNode(&EventList) && topev->seqnum > seqnum;
                                topev = PrevNode(topev))
                                ;
                } else {
                        for (topev = firstevent;
                                        topev != TailNode(&EventList) && topev->seqnum < seqnum;
                                topev = NextNode(topev))
                                ;
                }
        } else { /* seqnum > TopSeq */
                /*
                 *              Scanning forwards from the current top of the screen. Now
                 *              see if it would be faster to scan backwards from the end of
                 *              the event list.
                 */
                if ((EndSeq - seqnum) < (seqnum - TopSeq)) {
                        for (topev = EndEvent; topev != TopEvent && topev->seqnum > seqnum;
                                                                   topev = PrevNode(topev))
                                ;
                } else {
                        for (topev = TopEvent; topev != EndEvent && topev->seqnum < seqnum;
                                                                   topev = NextNode(topev))
                                ;
                }
        }
        
        /*
         *              Now topev is the new top event to display in the window.
         *              But wait! What if this topev is close to the bottom of our
         *              list? We want to always show a full window of text if
         *              possible. So, adjust it to ensure as many events as possible
         *              can fit in the screen.
         */
        while (topev != firstevent && seqnum > (EndSeq - BoxRows + 1)) {
                topev = PrevNode(topev);
                seqnum--;
        }

        /*
         *              Okay! Now we finally have topev pointing to our new top of
         *              screen node. Now update our display accordingly. We also
         *              take this opportunity to refresh our scroll bar, since it's
         *              relative position may well now be different.
         */
        FirstSeq  = firstseq;
        TopEvent  = topev;
        TopSeq    = seqnum;

        if ((displaytype & DISPLAY_NONE) || !MainWindow)
                goto done_show_buffer;

        SetupBufferRastPort();
        rport = MainWindow->RPort;

        /*
         *              We record the top line that was last drawn into the buffer. This
         *              is useful when ShowBuffer(..., DISPLAY_NONE) is called since it
         *              let's us detect when the info inside the window is no longer valid
         *              due to having scrolled off the top of the buffer.
         */
        if (seqnum != oldtopseq)
                LastDrawnTopSeq = TopSeq;

        if (displaytype & DISPLAY_ALL)
                goto redrawall;
        
        /*
         *              Setup so we only render to bitplane 0 -- this greatly
         *              speeds up text output and rendering speed, especially
         *              on deep Workbenches.
         */
        if (GfxBase->LibNode.lib_Version >= 39)
                SetWriteMask(rport, 1);         /* Speed up scrolling and rendering! */
        else
                rport->Mask = 1;

        /* 
         *              Now setup our scroll threshold. If running on V37 (where
         *              when we scroll, we get backfilled with colour 0 before
         *              we can redraw), then our threshold is 1/2 the displayed
         *              number of rows. If running on V39 or above, we increase
         *              the threshold to 3/4 of the displayed rows.
         */
        scrollthresh = BoxRows / 2;
        if (GfxBase->LibNode.lib_Version >= 39)
                scrollthresh = (BoxRows * 3) / 4;

        if (seqnum == oldtopseq) {
                /*
                 *              We're already at the correct position in the buffer, so
                 *              just scan looking for new or updated entries (i.e. any
                 *              which are not yet ES_ACCEPTED)
                 */
                while (row < BoxRows && seqnum <= EndSeq) {
                        if (topev->status != ES_ACCEPTED)
                                OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
        } else if (seqnum > oldtopseq && (seqnum - oldtopseq) <= scrollthresh) {
                /*
                 *              Scrolling forward, so move buffer up.
                 *
                 *              If running on V39 or above, we disable background fills
                 *              for our layer while we ScrollRaster() since we're going
                 *              to be redrawing the area anyway -- it speeds things up,
                 *              and is also less visually obtrusive.
                 */
                int dy = seqnum - oldtopseq;
                int scrollwidth = (RightCol - LeftCol + 1) * BoxCharWidth;

                DB("ScrollRaster start\n");
                if (GfxBase->LibNode.lib_Version >= 39) {
                        struct Hook *oldhook;

                        oldhook = InstallLayerHook(MainWindow->WLayer, LAYERS_NOBACKFILL);
                        ScrollRasterBF(rport, 0, dy * BoxSpacing,
                                                   BoxInLeft, BoxInTop,
                                                   BoxInLeft + scrollwidth-1, BoxInTop + BoxInHeight-1);
                        InstallLayerHook(MainWindow->WLayer, oldhook);
                } else {
                        ScrollRaster(rport, 0, dy * BoxSpacing,
                                                 BoxInLeft, BoxInTop,
                                                 BoxInLeft + scrollwidth-1, BoxInTop + BoxInHeight-1);
                }
                DB("ScrollRaster end\n");
                                         
                while (seqnum <= EndSeq && row < (BoxRows - dy)) {
                        /*
                         *              Even though these lines are already present,
                         *              we still have to re-output any lines that are
                         *              not ES_ACCEPTED since they may have been
                         *              updated since the last time they were displayed.
                         */
                        if (topev->status != ES_ACCEPTED)
                                OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
                while (seqnum <= EndSeq && row < BoxRows) {
                        OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
        } else if (oldtopseq > seqnum && (oldtopseq - seqnum) <= scrollthresh) {
                /*
                 *              Scrolling backward, so move buffer down.
                 */
                int dy = oldtopseq - seqnum;
                int scrollwidth = (RightCol - LeftCol + 1) * BoxCharWidth;

                DB("ScrollRaster start\n");
                if (GfxBase->LibNode.lib_Version >= 39) {
                        struct Hook *oldhook;

                        oldhook = InstallLayerHook(MainWindow->WLayer, LAYERS_NOBACKFILL);
                        ScrollRasterBF(rport, 0, -dy * BoxSpacing,
                                                   BoxInLeft, BoxInTop,
                                                   BoxInLeft + scrollwidth-1, BoxInTop+BoxInHeight-1);
                        InstallLayerHook(MainWindow->WLayer, oldhook);
                } else {
                        ScrollRaster(rport, 0, -dy * BoxSpacing,
                                                 BoxInLeft, BoxInTop,
                                                 BoxInLeft + scrollwidth-1, BoxInTop + BoxInHeight-1);
                }
                DB("ScrollRaster end\n");
                /*
                 *              Update the new (blank) areas first
                 */
                while (seqnum <= oldtopseq) {
                        OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
                /*
                 *              Now can the remaining lines in the buffer (which are
                 *              already on the screen) and re-output any which are
                 *              not ES_ACCEPTED, since they may have changed since the
                 *              last update.
                 */
                while (row < BoxRows && seqnum <= EndSeq) {
                        if (topev->status != ES_ACCEPTED)
                                OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
        } else {
                /*
                 *              The move is too big to handle by scrolling, so just
                 *              redraw the entire buffer instead.
                 */
redrawall:
                while (row < BoxRows && seqnum <= EndSeq) {
                        OutputBufLine(topev, row);
                        topev = NextNode(topev);
                        seqnum++;
                        row++;
                }
        }

        if ((displaytype & DISPLAY_ALL) == 0) {
                /*
                 *              Undo the bitplane optimisation we did earlier, but only
                 *              if we're doing an optimised refresh.
                 */
                if (GfxBase->LibNode.lib_Version >= 39)
                        SetWriteMask(rport, 0xFF);
                else
                        rport->Mask = 0xff;
        }
        BottomSeq = seqnum - 1; /* Bottom most sequence number in window */

done_show_buffer:
        DB("ShowBuffer-Semaphore-done\n");
        ReleaseSemaphore(&BufSem);
        UNLOCK_LAYERS;
}

/*
 *              HandleNewEvents()
 *
 *              Called whenever we are signalled that a new event has arrived
 *              in the input queue.
 *
 *              We simply scan the queue from our last known position to our
 *              current position and, if necessary, update our output window
 *              to reflect any new events. We also output any new _complete_
 *              events to the logfile.
 */
void HandleNewEvents(void)
{
        Event *firstevent;
        Event *newev;
        Event *ev;
        int   curendcompleteseq = EndCompleteSeq;
        int   curendseq         = EndSeq;
        LONG  newseq;
        int   refresh = 0;

        DB("HandleNewEvent-Semaphore\n");

        if (IsListEmpty(&EventList) || AwaitingResize != RESIZE_DONE
                                                                || DraggingColumn || DraggingRow)
                return;
        
        /*
         *              It is vital that for the duration of the screen update,
         *              we retain complete access to our output rastport. Otherwise,
         *              we can run into a deadlock where a higher priority task
         *              (like input.device) locks the screen layer's while we
         *              are in the middle of our refresh, and then calls one of
         *              our patched functions (like OpenFont) -- instant hang
         *              (because we own the buffer semaphore and the patched
         *              function will sit waiting for us to free it).
         *
         *              By locking our layers, we ensure that nobody else can
         *              ever get in once we start our refresh. The downside is
         *              that this locks out menu rendering etc, but since we'll
         *              be quick at refreshing, that's okay.
         *
         *              Note that we lock the layers _before_ getting the buffer
         *              semaphore -- if we do it afterwards, there is a window of
         *              opportunity where we task switch to a higher priority task
         *              between the two calls which then proceeds to lock layers
         *              and try and lock the buffer semaphore -- deadlock again!
         *
         *              News Flash! We don't lock layers any more, since it lead
         *              to jerky performance when dragging the scrollbar etc.
         *              Instead, our patch code checks to see if our layers are
         *              locked by the calling task and if they are, bypasses
         *              outputting the event completely. Since the only things
         *              that ever call our patches when layers are locked are
         *              low-level Intuition functions that are re-rendering
         *              gadgets etc (and calling OpenFont()) this is actually
         *              a win-win since we're typically not interested in those
         *              events anyway.
         */             
        if (MainWindow) {
                LOCK_LAYERS;            /* This is now a no-op */
        }
        ObtainSemaphore(&BufSem);

        if (EndSeq == BottomSeq)                        /* If at end of buf, show new o/p       */
                refresh = 1;

        firstevent = HeadNode(&EventList);
        if (firstevent->seqnum > EndCompleteSeq)
                newev = firstevent;
        else if (EndCompleteEvent == TailNode(&EventList)) {
                newev = EndCompleteEvent;
        } else {
                newev = NextNode(EndCompleteEvent);
        }
        newseq = newev->seqnum;
        
        /*
         *              First we scan all the new incoming entries to figure out
         *              our new EndEvent/EndSeq (for ShowBuffer)
         */
        for (ev = newev; NextNode(ev) != NULL && ev->status != ES_CREATING;
                                         ev = NextNode(ev))
                ;
        EndEvent = PrevNode(ev);
        EndSeq   = EndEvent->seqnum;

        if (MainWindow) {
                if (refresh) {
                        /*
                         *              We were already displaying the end of the buffer and we
                         *              got new info in, so move the display to the new end of
                         *              buffer.
                         */
                        ShowBuffer(EndSeq, DISPLAY_QUICK);
                } else if (BottomSeq > EndCompleteSeq) {
                        /*
                         *              We weren't at the very end of the buffer, but there was an
                         *              unfinished event displayed somewhere on the screen, so
                         *              rescan the currently displayed bufferlines in case it
                         *              needs to be updated.
                         */
                        ShowBuffer(TopSeq, DISPLAY_QUICK);              
                } else {
                        /*
                         *              We're too far up in the buffer to show any of the
                         *              new data, but update our internal variables anyway
                         *              so we can keep the scroll bar up to date.
                         */
                        ShowBuffer(TopSeq, DISPLAY_NONE);
                }
        } else {
                /*
                 *              Simply update TopSeq accordingly so it will be correct the
                 *              next time we open the window
                 */
                TopSeq   = EndSeq;
                TopEvent = EndEvent;
        }

        /*
         *              Now mark all new events which are finished as complete, so that
         *              we won't rescan them
         */
        for (ev = newev; NextNode(ev) != NULL && ev->status >= ES_READY;
                         ev = NextNode(ev)) {
                EndCompleteEvent = ev;
                EndCompleteSeq   = ev->seqnum;
        }
        DB("HandleNewEvent-Semaphore-done\n");
        ReleaseSemaphore(&BufSem);

        if (MainWindow) {
                CheckForDirtyMainWindow();
                UNLOCK_LAYERS;
                UpdateMainVScroll();
        }

        if (LogActive && !Disabled) {
                /*
                 *              Now output the new events to our logfile. We have to be
                 *              very careful doing this since we must not keep the buffer
                 *              semaphore locked while doing file output (it could lead
                 *              to deadlocks if the file output causes a requester to
                 *              appear on the screen, for example). Thus, we must lock
                 *              and unlock the semaphore as we scan each event. Sigh.
                 */
                LONG latestready;               /* seqnum of highest event that's ES_READY */

                /*
                 *              Output the new events to the logfile. We only output
                 *              events that are complete, or events that are incomplete
                 *              but which have a complete event after them. Any complete
                 *              events that have been output earlier are not output
                 *              again, unless they were only partially output the first
                 *              time.
                 */
                ObtainSemaphore(&BufSem);
                ev = HeadNode(&EventList);
                if (ev->seqnum > newseq) {
                        newev  = ev;
                        newseq = ev->seqnum;
                } else if (newseq == curendcompleteseq) {
                        /*
                         *              We couldn't advance past the endcompleteseq earlier on
                         *              because there was nothing beyond it, so step past now.
                         */
                        newev = NextNode(newev);
                        newseq++;
                }
                for (ev = TailNode(&EventList);
                        ev->seqnum > newseq && ev->status < ES_READY; ev = PrevNode(ev))
                        ;
                latestready = ev->seqnum;
                if (ev->status < ES_READY)
                        latestready--;          /* Just in case ev->seqnum == newseq */

                for (ev = newev; NextNode(ev) != NULL && ev->seqnum <= latestready;
                                                 ev = NextNode(ev)) {
                        int firstchar = 0;

                        /*
                         *              For each event that is either complete, or partially
                         *              complete but followed by a complete event, output it
                         *              to the logfile (unless already output).
                         */
                        if (ev->status >= ES_READY) {
                                if ((ev->flags & EFLG_LOGDONE) == 0) {
                                        if (ev->flags & EFLG_LOGPARTIAL)
                                                firstchar = '\\';
                                        else
                                                firstchar = ' ';
                                        ev->flags |= EFLG_LOGDONE;
                                }
                        } else if ((ev->flags & EFLG_LOGPARTIAL) == 0) {
                                /*
                                 *              Output partial event
                                 */
                                ev->flags |= EFLG_LOGPARTIAL;
                                firstchar = '/';
                        }

                        if (firstchar) {
                                /*
                                 *              Output the line
                                 */
                                FormatEvent(LogEFormat, ev, BufferLine+1, 0, LogWidth - 1);
                                BufferLine[0]                     = firstchar;
                                BufferLine[LogWidth+1] = '\n';
                                BufferLine[LogWidth+2] = 0;
                                ReleaseSemaphore(&BufSem);
                                WriteLog(BufferLine);
                                ObtainSemaphore(&BufSem);
                                /*
                                 *              While we had the semaphore released, it's possible
                                 *              that the event we're on may have scrolled off the
                                 *              buffer. If it has, then give up right now (we'll
                                 *              catch the new ones the next time around).
                                 */
                                if (((Event *)HeadNode(&EventList))->seqnum > latestready)
                                        break;
                        }
                }
                ReleaseSemaphore(&BufSem);
                if (Paused)
                        WriteLog(NULL); /* Flush log if we're single-stepping */
        }
        if (AutoOpen && !MainWindow && curendseq < EndSeq)
                ShowSnoopDos();
}

/*
 *              CleanupMainWindow()
 *
 *              Frees any resources associated with this module
 */
void CleanupMainWindow(void)
{
        CloseMainWindow();
        if (MainGadList)        FreeGadgets(MainGadList),       MainGadList             = NULL;
        if (BufferFont)         CloseFont(BufferFont),          BufferFont              = NULL;
        if (LogActive)          CloseLog();
}
