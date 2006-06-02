/*
 *              SUBWIN.C                                                                                        vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *              This module handles all the windows other than the main window.
 *              That is, the functions window, settings window and format
 *              window.
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
 *              These are used by all the requesters to decide what options to do
 *              when leaving a requester (make current settings permanent or
 *              restore the old settings)
 */
#define SEL_NONE                0               /* User didn't select an option yet             */
#define SEL_USE                 1               /* User selected USE in a requester             */
#define SEL_CANCEL              2               /* User selected CANCEL in a requester  */

/*
 *              These control which items are displayed in the System and DOS
 *              columns in the function window.
 */
#define COL_SELECTED    0               /* Column shows selected items                  */
#define COL_ALL                 1               /* Column shows all items                               */
#define COL_NONE                2               /* Column shows no items                                */

#define SETBUTTON_FILE  0               /* Little image of a file folder                */
#define SETBUTTON_FONT  1               /* Little image of a font folder                */
#define SETBUTTON_MAX   2               /* Anything above this is a string ID   */

/*
 *              These two are imported from SNOOPDOS.C
 */
extern char     SnoopDosTitle[];

/*
 *              Variables for the format editor
 */
int FBoxA_Left;                         /* X pos of left box in format window   */
int FBoxA_Top;                          /* Y pos of left box in format window   */
int FBoxA_Width;                        /* Width of left box in format window   */
int FBoxA_Height;                       /* Height of left box in format window  */
int FBoxA_NumChars;                     /* No. of characters across left box    */
int FBoxA_CurLines;                     /* No. of lines currently in left box   */

int FBoxB_Left;                         /* X pos of right box in format window  */
int FBoxB_Top;                          /* Y pos of right box in format window  */
int FBoxB_Width;                        /* Hidth of right box in format window  */
int FBoxB_Height;                       /* Weight of right box in format window */
int FBoxB_NumChars;                     /* No. of characters across right box   */
int FBoxB_CurLines;                     /* No. of lines currently in right box  */

int FBoxSpacing;                        /* Distance between box text baselines  */
int FBoxSelected;                       /* Line number of right box selection   */
                                                        /* (or FBOX_NOSELECT / FBOX_SELECTLEFT) */

int FBoxDeltaX;                         /* Offset from mouse to corner of drag  */
int FBoxDeltaY;                         /* Offset from mouse to corner of drag  */
int FBoxDragSelect;                     /* Currently selected drag drop area    */
int FormatCurLMB;                       /* Current state of left mouse button   */
int FormatMovingField;          /* If true, currently dragging field    */
int FormatDragBox;                      /* Box we're dragging from                              */
int FormatDragLine;                     /* Line we're dragging from                             */

char FBoxA_FormString[20];      /* sprintf format string for left box   */
char FBoxB_FormString[20];      /* sprintf format string for right box  */

char FormatSaveFormat[MAX_FORM_LEN];    /* Save area for format ed      */

/*
 *              These next two arrays are indices into the FieldTypes[] array
 *              of format initialisers.
 */
typedef struct {
        int     type;                                                   /* Type of this entry           */
        int width;                                                      /* Width of this entry          */
} EditEvent;
        
extern FieldInit FieldTypes[];                  /* Pulled in from BUFFER.C      */

EditEvent CurrentFields[EF_COUNT+1];    /* Fields in current format     */
EditEvent AvailableFields[EF_COUNT+1];  /* Fields still available       */

Settings                        SavedFuncSets;          /* Saved function settings      */
Settings                        SavedSetupSets;         /* Saved setup settings         */

struct Gadget           *SetGadList;            /* Settings window gadgets      */
struct Gadget           *FuncGadList;           /* Function window gadgets      */
struct Gadget           *FormGadList;           /* Format window gadgets        */
struct Image            *FileButtonImage;       /* Image of a file button       */
struct Image            *FontButtonImage;       /* Image of a font button       */

struct TextFont         *FormBufFont;           /* Format window listview       */
struct TextFont         *FormGadFont;           /* Format window gadgets        */

/*
 *              We don't usually free the Function window gadgets when we close
 *              the window (since it takes a long time to generate them). However,
 *              if we change window font, then we need a way to be able to change
 *              them. This variable shows you how.
 */
int             FuncSystemCol;          /* Mode of system column (all/none/sel) */
int             FuncDOSCol;                     /* Mode of DOS column (all/none/select) */

UBYTE   SavedCols[GID_NUMFUNCSETS];     /* Currently selected settings  */
UBYTE   TempCols[GID_NUMFUNCSETS];      /* Currently selected settings  */

UBYTE   SetKeyboard[KB_SHORTCUT_SIZE];  /* For keyboard equivs          */
UBYTE   FuncKeyboard[KB_SHORTCUT_SIZE]; /* For keyboard equivs          */
UBYTE   FormKeyboard[KB_SHORTCUT_SIZE]; /* For keyboard equivs          */

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
extern struct TextAttr TopazFontAttr;
extern struct TextAttr SystemFontAttr;
extern struct TextAttr WindowFontAttr;
extern struct TextAttr BufferFontAttr;

extern struct {
        struct TextAttr *gadgetfa;
        struct TextAttr *bufferfa;
} MainWindowFontList[];                         /* Imported from MAINWIN.C */

struct TextAttr *SubWindowFontList[] = {
        &WindowFontAttr,
        &SystemFontAttr,
        &TopazFontAttr,
        NULL
};

/*
 *              Next, all our gadget structures -- these are used to build the
 *              font-sensitive gadget windows at runtime.
 */
struct FuncGadgets {
        ULONG   gadgid;                 /* Gadget ID                                    */
        UWORD   stringid;               /* Name of checkmark gadget     */
        UBYTE   col;                    /* Column (0, 1 or 2)                   */
        UBYTE   row;                    /* Row (0 -- 13)                                */
};

struct FuncGadgets FuncGadgs[] = {
        /*
         *              General controls
         */
        GID_ONLYFAILS,          MSG_SHOWFAILS_GAD,              0, 0,
        GID_SHOWCLINUM,         MSG_SHOWCLI_GAD,                0, 1,
        GID_SHOWPATHS,          MSG_SHOWFULLPATHS_GAD,  0, 2,
        GID_USEDEVNAMES,        MSG_USEDEVNAMES_GAD,    0, 3,
        GID_MONPACKETS,         MSG_MONPACKETS_GAD,             0, 4,
        GID_MONALLPACKETS,      MSG_MONALLPACKETS_GAD,  0, 5,
        GID_MONROMCALLS,        MSG_MONROMCALLS_GAD,    0, 6,
        GID_IGNOREWB,           MSG_IGNOREWB_GAD,               0, 7,
        /*
         *              System functions
         */
        GID_FINDPORT,           MSG_FINDPORT_GAD,               1, 0,
        GID_FINDRESIDENT,       MSG_FINDRESIDENT_GAD,   1, 1,
        GID_FINDSEMAPHORE,      MSG_FINDSEMAPHORE_GAD,  1, 2,
        GID_FINDTASK,           MSG_FINDTASK_GAD,               1, 3,
        GID_LOCKSCREEN,         MSG_LOCKSCREEN_GAD,             1, 4,
        GID_OPENDEVICE,         MSG_OPENDEVICE_GAD,             1, 5,
        GID_OPENFONT,           MSG_OPENFONT_GAD,               1, 6,
        GID_OPENLIBRARY,        MSG_OPENLIBRARY_GAD,    1, 7,
        GID_OPENRESOURCE,       MSG_OPENRESOURCE_GAD,   1, 8,
        GID_READTOOLTYPES,      MSG_READTOOLTYPES_GAD,  1, 9,
        GID_SENDREXX,           MSG_SENDREXX_GAD,               1, 10,

        /*
         *              AmigaDOS Functions
         */
        GID_CHANGEDIR,          MSG_CHANGEDIR_GAD,              2, 0,
        GID_DELETE,                     MSG_DELETE_GAD,                 2, 1,
        GID_EXECUTE,            MSG_EXECUTE_GAD,                2, 2,
        GID_GETVAR,                     MSG_GETVAR_GAD,                 2, 3,
        GID_LOADSEG,            MSG_LOADSEG_GAD,                2, 4,
        GID_LOCKFILE,           MSG_LOCKFILE_GAD,               2, 5,
        GID_MAKEDIR,            MSG_MAKEDIR_GAD,                2, 6,
        GID_MAKELINK,           MSG_MAKELINK_GAD,               2, 7,
        GID_OPENFILE,           MSG_OPENFILE_GAD,               2, 8,
        GID_RENAME,                     MSG_RENAME_GAD,                 2, 9,
        GID_RUNCOMMAND,         MSG_RUNCOMMAND_GAD,             2, 10,
        GID_SETVAR,                     MSG_SETVAR_GAD,                 2, 11,
        GID_SYSTEMTAGLIST,              MSG_SYSTEM_GAD,                 2, 12,

        0,                                      0,                                              0, 0
};

#ifdef __amigaos4__
ULONG disabled[]={
        GID_FINDRESIDENT,     
        GID_SENDREXX,         
        GID_LOADSEG,          
        GID_SYSTEMTAGLIST,    
        0
};
#endif

/*
 *              This structure defines the gadgets in the settings window. There
 *              are three basic types of gadgets supported: cycle, string and
 *              display. For cycle gadgets, data1 is the index of the current
 *              selection, data2 is a 0-terminated array of message IDs that
 *              will hold the possible ListView choices, and data3 is a pointer
 *              to an array of string pointers of the same size (including
 *              terminating NULL) that will be filled in by the code with actual
 *              pointers to those strings.
 *              
 *              For string and display gadgets, data2 is a pointer to the string
 *              to be displayed inside the gadget and data1/data3 are unused.
 *              However, if promptid is specified, then data1 is the message
 *              number of the prompt to be used in the button instead of defaulting
 *              to a question mark (the width is automatically adjusted).
 */             
struct SetGadgets {
        UWORD   gadgid;                 /* Gadget ID                                                            */
        UWORD   stringid;               /* ID of label name                                     */
        ULONG   gtype;                  /* Gadget type (cycle, string, or display)      */
        UWORD   promptid;               /* If non-zero, ID of optional "?" button       */
        ULONG   data1;                  /* See above                                                            */
        void    *data2;                 /* See above                                                            */
        void    *data3;                 /* See above                                                            */
        UBYTE   col;                    /* Start col (0 or 1)                                           */
        UBYTE   cwidth;                 /* Column width (0, 1 or 2)                                     */
        UBYTE   row;                    /* Row (0 to 6)                                                         */
};

int SetCylHide[] = {
        MSG_CYL_HIDE_GAD, MSG_CYL_ICONIFY_GAD, MSG_CYL_TOOLSMENU_GAD,
        MSG_CYL_NONE_GAD, 0
};

int SetCylOpenOn[] = {
        MSG_CYL_DEFSCREEN_GAD, MSG_CYL_FRONTSCREEN_GAD, MSG_CYL_NAMEDSCREEN_GAD, 0
};

int SetCylLogMode[] = {
        MSG_CYL_PROMPT_USER_GAD, MSG_CYL_APPEND_GAD, MSG_CYL_OVERWRITE_GAD,
        MSG_CYL_SERIALPORT_GAD, 0
};

int SetCylFileIO[] = {
        MSG_CYL_AUTOMATIC_GAD, MSG_CYL_IMMEDIATE_GAD, MSG_CYL_BUFFERED_GAD, 0
};

char *SetCylHideText[HIDE_MAX+1];
char *SetCylOpenOnText[SCREEN_MAX+1];
char *SetCylLogModeText[LOGMODE_MAX+1];
char *SetCylFileIOText[FILE_MAX+1];


/*
 *              If you re-arrange the order of these gadget definitions,
 *              make sure you re-order the enum's that follow below as well.
 */
struct SetGadgets SetGadgs[] = {
        GID_BUFFORMAT,  MSG_BUFFORMAT_GAD,      STRING_KIND,    GID_FORMATEDIT, 
                                                                                                                MSG_FORMATEDIT_GAD,
                                        NULL,                           0,                                      0,      2,      5,

        GID_LOGFORMAT,  MSG_LOGFORMAT_GAD,      STRING_KIND,    GID_FORMATCOPY,
                                                                                                                MSG_FORMATCOPYF_GAD,
                                        NULL,                           0,                                      0,      2,      6,
        
        GID_HIDEMETHOD, MSG_HIDEMETHOD_GAD,     CYCLE_KIND,             0,      0,
                                        SetCylHide,                     SetCylHideText,         0,      0,      0,
        
        GID_OPENON,             MSG_OPENON_GAD,         CYCLE_KIND,             0,      0,
                                        SetCylOpenOn,           SetCylOpenOnText,       0,      0,      1,

        GID_LOGMODE,    MSG_LOGMODE_GAD,        CYCLE_KIND,             0,      0,
                                        SetCylLogMode,          SetCylLogModeText,      0,      0,      2,

        GID_FILEIO,             MSG_FILEIO_GAD,         CYCLE_KIND,             0,      0,
                                        SetCylFileIO,           SetCylFileIOText,       0,      0,      3,
        
        GID_HOTKEY,             MSG_HOTKEY_GAD,         STRING_KIND,    0,      0,
                                        NULL,                           0,                                      1,      1,      0,
        
        GID_SCREENNAME, MSG_SCREENNAME_GAD,     STRING_KIND,    0,      0,
                                        NULL,                           0,                                      1,      1,      1,
        
        GID_LOGFILE,    MSG_LOGFILE_GAD,        STRING_KIND,    GID_FILEPROMPT,
                                                                                                                SETBUTTON_FILE,
                                        NULL,                           0,                                      1,      1,      2,

        GID_WINDOWFONT, MSG_WINDOWFONT_GAD,     TEXT_KIND,              GID_WFONTPROMPT,
                                                                                                                SETBUTTON_FONT,
                                        NULL,                           0,                                      1,      1,      3,
        
        GID_BUFFERFONT, MSG_BUFFERFONT_GAD,     TEXT_KIND,              GID_BFONTPROMPT,
                                                                                                                SETBUTTON_FONT,
                                        NULL,                           0,                                      1,      1,      4,

        0
};

/*
 *              The following gadgets should match the _order_ that the above
 *              gadgets are laid out in, since they are used as indices into the
 *              above array.
 */
typedef enum {
        SG_BufferFormat,
        SG_LogfileFormat,
        SG_HideMethod,
        SG_ScreenType,
        SG_LogMode,
        SG_FileIOType,
        SG_HotKey,
        SG_ScreenName,
        SG_LogFile,
        SG_WindowFont,
        SG_BufferFont,
} SetGadgetEnums;

/*
 *              Arrays of gadget names for calculating minimum width
 */

/*
 *              Miscellaneous gadgets in functions window
 */
int MiscColNames[] = {
        MSG_SHOWFAILS_GAD,
        MSG_SHOWCLI_GAD,
        MSG_SHOWFULLPATHS_GAD,
        MSG_USEDEVNAMES_GAD,
        MSG_MONPACKETS_GAD,
        MSG_MONALLPACKETS_GAD,
        MSG_IGNOREWB_GAD,
        0
};

/*
 *              System gadgets in functions window
 */
int SysColNames[] = {
        MSG_FINDPORT_GAD,     
        MSG_FINDRESIDENT_GAD, 
        MSG_FINDSEMAPHORE_GAD,
        MSG_FINDTASK_GAD,     
        MSG_LOCKSCREEN_GAD,   
        MSG_OPENDEVICE_GAD,   
        MSG_OPENFONT_GAD,     
        MSG_OPENLIBRARY_GAD,  
        MSG_OPENRESOURCE_GAD, 
        MSG_READTOOLTYPES_GAD,
        MSG_SENDREXX_GAD, 
        0
};

/*
 *              AmigaDOS gadgets in functions window
 */
int DOSColNames[] = {
        MSG_CHANGEDIR_GAD, 
        MSG_DELETE_GAD,    
        MSG_EXECUTE_GAD,   
        MSG_GETVAR_GAD,    
        MSG_LOADSEG_GAD,   
        MSG_LOCKFILE_GAD,  
        MSG_MAKEDIR_GAD,   
        MSG_MAKELINK_GAD,  
        MSG_OPENFILE_GAD,  
        MSG_RENAME_GAD,    
        MSG_RUNCOMMAND_GAD,
        MSG_SETVAR_GAD,    
        MSG_SYSTEM_GAD,
        0
};

/*
 *              Aditional gadgets in function window
 */
int FuncCycleText[]             = { MSG_ALL_GAD, MSG_NONE_GAD, MSG_SELECTED_GAD, 0 };
int FuncCycleName[]             = { MSG_SELSYSTEM_GAD, MSG_SELDOS_GAD, 0 };
int UseCancelUndoText[] = { MSG_USE_GAD, MSG_CANCEL_GAD, MSG_UNDO_GAD, 0 };

/*
 *              Left column gadgets names in settings window
 */
int SettingsLeftNames[] = {
        MSG_HIDEMETHOD_GAD,
        MSG_OPENON_GAD,
        MSG_LOGMODE_GAD,
        MSG_FILEIO_GAD,
        MSG_BUFFORMAT_GAD,
        MSG_LOGFORMAT_GAD,
        0
};

/*
 *              Right column gadget names in settings window
 */
int SettingsRightNames[] = {
        MSG_HOTKEY_GAD,
        MSG_SCREENNAME_GAD,
        MSG_LOGFILE_GAD,
        MSG_WINDOWFONT_GAD,
        MSG_BUFFERFONT_GAD,
        0
};

/*
 *              Two mini buttons attached to format gadgets in settings window
 *              Note that we have two types of possibility: one for fixed point
 *              fonts and one for proportional fonts. This is so that we can
 *              arrange for the two strings "Edit..." and "Copy" to always
 *              look properly aligned, regardless of font type. If we didn't do
 *              this, then with fixed point fonts, "Copy" looks mis-aligned
 *              with respect to "Edit..." because they are different lengths.
 */
int SetMiniButtonsP[] = {
        MSG_FORMATCOPYP_GAD,
        MSG_FORMATEDIT_GAD,
        0
};

int SetMiniButtonsF[] = {
        MSG_FORMATCOPYF_GAD,
        MSG_FORMATEDIT_GAD,
        0
};

/*
 *              Left column gadget contents in settings
 */
int SettingsLeftContents[] = {
        MSG_CYL_HIDE_GAD,
        MSG_CYL_ICONIFY_GAD,
        MSG_CYL_TOOLSMENU_GAD,
        MSG_CYL_DEFSCREEN_GAD,
        MSG_CYL_FRONTSCREEN_GAD,
        MSG_CYL_NAMEDSCREEN_GAD,
        MSG_CYL_PROMPT_USER_GAD,
        MSG_CYL_APPEND_GAD,
        MSG_CYL_OVERWRITE_GAD,
        MSG_CYL_AUTOMATIC_GAD,
        MSG_CYL_IMMEDIATE_GAD,
        MSG_CYL_BUFFERED_GAD,
        0
};

/*
 *              Mega struct that collects all the data needed to create useful BOBs.
 */
typedef struct MyGel {
        struct GelsInfo  gelinfo;                       /* GelInfo for entire structure */
        WORD                     nextline[8];           /* Nextline data                                */ 
        WORD                     *lastcolor[8];         /* Last colour data                             */
        struct RastPort  *mainrast;                     /* Rastport bob is displayed in */
        struct collTable colltable;                     /* Collision table                              */
        struct VSprite   vshead;                        /* Head sprite anchor                   */
        struct VSprite   vstail;                        /* Tail sprite anchor                   */
        struct VSprite   bobvsprite;            /* Vsprite used for bob                 */
        struct Bob               bob;                           /* Data for a single bob                */
        struct RastPort  rastport;                      /* Rastport for our BOB                 */
        struct BitMap    bitmap;                        /* Bitmap for our BOB                   */
        ULONG                    planesize;                     /* Size of one plane in bob             */
        UBYTE                   *planedata;                     /* Pointer to first plane               */
        UBYTE                   *chipdata;                      /* Pointer to all CHIP RAM data */
        ULONG                    chipsize;                      /* Total size of allocated CHIP */
} MyGel;

MyGel   *TextBob;                                               /* Info about GEL/BOBs                  */

/*
 *              Some prototypes private to this file
 */
MyGel *CreateBob(struct RastPort *rport, int width, int height, int transp);
void FreeBob(MyGel *mygel);
void UpdateBob(int x, int y);
int  PickupBob(int hit, int x, int y);
void DropBob(void);

/*****************************************************************************
 *
 *              Start of functions!
 *
 *****************************************************************************/

/*
 *              CleanupSubWindows()
 *
 *              Frees any resources associated with this module
 */
void CleanupSubWindows(void)
{
        PurgeFuncGadgets = 1;
        CloseFunctionWindow();
        CloseSettingsWindow();
        CloseFormatWindow();
}

/*
 *              GetFuncName(gadgetid)
 *
 *              Returns a pointer to a string describing the function name which
 *              matches the gadget ID passed in. This is used by PATCHES.C when
 *              it is unpatching the patched functions at exit time to identify
 *              which function can't be unpatched in the event of a problem.
 */
char *GetFuncName(int gadgetid)
{
        struct FuncGadgets *fg = FuncGadgs;

        if (gadgetid == GID_SENDREXX)   /* Special-case this multi-function one */
                return MSG(MSG_PACKET_NAME);

        for (fg = &FuncGadgs[0]; fg->gadgid; fg++) {
                if (fg->gadgid == gadgetid)
                        return MSG(fg->stringid);
        }
        return ("<Unknown Function>");
}

/*
 *              CreateFunctionGadgets(fontattr, getsize, &width, &height)
 *
 *              Creates all the gadgets for the function window. If getsize
 *              is true, then the width and height values are filled in with
 *              the dimensions of the window needed to hold the gadgets. In
 *              this case, NULL is returned if the window would be too big to
 *              fit on the current screen, or non-NULL if width and height
 *              were successfully initialised.
 *
 *              If getsize is zero, then the actual gadgets are created and
 *              a pointer to the gadget list is returned, or NULL if an
 *              error occurred (typically, out of memory).
 */
struct Gadget *CreateFunctionGadgets(struct TextAttr *fontattr,
                                                                         int getsize, int *pwidth, int *pheight)
{
        static char *cyltext[4];
        struct TextFont *font;
        struct FuncGadgets *fg;
        struct NewGadget ng;
        struct Gadget *gadlist;
        struct Gadget *gad;
        UWORD spacing[3];
        UWORD yoffset[3]; 
        UWORD colpos[3];
        int heightadjust = 0;
        int width;
        int height;
        int gwidth;
        int gheight;
        int fonty;
        int colspace = 30;
        int topline;
        int w1, w2;

        if (!FuncVI) {
                FuncVI = GetVisualInfoA(SnoopScreen, NULL);
                if (!FuncVI)
                        return (NULL);
        }
        font = MyOpenFont(fontattr);
        if (!font)
                return (NULL);

        fonty           = font->tf_YSize;
        gheight         = fonty   + 3;
        gwidth          = gheight + 15;
        
        spacing[0]  = fonty + 4;
        spacing[1]  = fonty + 4;
        spacing[2]  = fonty + 4;
        yoffset[1]      = TitlebarHeight + fonty/2;
        yoffset[2]      = yoffset[1];
        yoffset[0]      = yoffset[1] + 10 * spacing[1] - 7 * spacing[0];

        colpos[0]       = BorderLeft + FUNC_MARGIN;
        colpos[1]       = colpos[0]  + MaxTextLen(font, MiscColNames)
                                  + gwidth + colspace;
        w1                      = MaxTextLen(font, SysColNames) + gwidth;
        w2                      = GetTextLen(font, MSG(MSG_SYSFUNCS_GAD));
        colpos[2]       = colpos[1]  + MAX(w1, w2) + colspace;
        w1                      = MaxTextLen(font, DOSColNames) + gwidth;
        w2                      = GetTextLen(font, MSG(MSG_DOSFUNCS_GAD));
        width           = colpos[2]  + MAX(w1, w2) + FUNC_MARGIN + BorderRight + 8;
        height      = yoffset[2] + spacing[2] * 15 + BorderBottom;

        topline = yoffset[2];
        if (ScreenHeight < 256 && height > ScreenHeight) {
                /*
                 *              For medium and low resolution screens, shave another
                 *              few pixels from the height. Our aim is to be able to
                 *              open on a 640 x 200 screen with a 15 point screen font.
                 */
                heightadjust = -2;
                height      += heightadjust * 2;
        }
        if ((height + spacing[2] * 2) <= ScreenHeight) {
                int disp = (spacing[2] * 3) / 2;

                yoffset[0] += disp;
                yoffset[1] += disp;
                yoffset[2] += disp;
                height     += disp;
        }

        if (width > ScreenWidth || height > ScreenHeight) {
                CloseFont(font);
                return (NULL);
        }
        if (getsize) {
                CloseFont(font);
                *pwidth  = width;
                *pheight = height;
                return (struct Gadget *)(-1);
        }

        /*
         *              Now create our new gadgets
         */     
        ng.ng_Height            = gheight; 
        ng.ng_Width                     = gwidth;  
        ng.ng_TextAttr          = fontattr;
        ng.ng_VisualInfo        = FuncVI;
        ng.ng_Flags         = PLACETEXT_RIGHT;

        gadlist = NULL;
        gad = CreateContext(&gadlist);
        if (!gad)
                goto fgad_failed;

        for (fg = &FuncGadgs[0]; fg->gadgid; fg++) {
                int col = fg->col;
                int i,dis=FALSE;
#ifdef __amigaos4__
                for(i=0;disabled[i];i++)
                	if(disabled[i] == fg->gadgid) {
                		dis = TRUE;
                		break;
                	}
#endif

                ng.ng_LeftEdge          = colpos[col];
                ng.ng_TopEdge           = fg->row * spacing[col] + yoffset[col];
                ng.ng_GadgetText        = MSG(fg->stringid);
                ng.ng_GadgetID          = fg->gadgid;

                gad = CreateGadget(CHECKBOX_KIND, gad, &ng,
                                                   GT_Underscore,       '_',
                                                   GTCB_Scaled,     TRUE,
                                                   GA_Disabled,dis,
                                                   TAG_DONE);
                if (!gad)
                        goto fgad_failed;
                Gadget[fg->gadgid] = gad;
                AddKeyShortcut(FuncKeyboard, fg->gadgid, fg->stringid);
        }

        /*
         *              Now create the remaining window gadgets
         */
        ng.ng_LeftEdge          = colpos[0]
                                                  + GetTextLen(font, MSG(MSG_MATCHNAME_GAD)) + 10;
        ng.ng_TopEdge           = yoffset[1] + (23 * spacing[1]) / 2;
        ng.ng_GadgetText        = MSG(MSG_MATCHNAME_GAD);
        ng.ng_GadgetID          = GID_MATCHNAME;
        ng.ng_Width         = colpos[2] - ng.ng_LeftEdge - colspace + 8;
        ng.ng_Height            = gheight + 3;
        ng.ng_Flags         = PLACETEXT_LEFT;

        gad = CreateGadget(STRING_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           GTST_MaxChars,       MAX_STR_LEN,
                                           GTST_String,         CurSettings.Func.Pattern,
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_MATCHNAME] = gad;
        AddKeyShortcut(FuncKeyboard, GID_MATCHNAME, MSG_MATCHNAME_GAD);

        ng.ng_GadgetText        = MSG(MSG_SELSYSTEM_GAD);
        ng.ng_GadgetID          = GID_SELSYSTEM;
        ng.ng_Height            = gheight + 3;
        ng.ng_Width                     = MaxTextLen(font, FuncCycleText) + 40;
        ng.ng_LeftEdge          = colpos[0] + MaxTextLen(font, FuncCycleName) + 10;
        ng.ng_TopEdge           = yoffset[1];
        cyltext[0]                      = MSG(MSG_SELECTED_GAD);
        cyltext[1]                      = MSG(MSG_ALL_GAD);
        cyltext[2]                      = MSG(MSG_NONE_GAD);
        cyltext[3]                      = NULL;

        gad = CreateGadget(CYCLE_KIND,          gad, &ng,
                                           GT_Underscore,       '_',
                                           GTCY_Labels,         cyltext,
                                           GTCY_Active,         COL_SELECTED,
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_SELSYSTEM] = gad;
        AddKeyShortcut(FuncKeyboard, GID_SELSYSTEM, MSG_SELSYSTEM_GAD);

        ng.ng_GadgetText        = MSG(MSG_SELDOS_GAD);
        ng.ng_GadgetID          = GID_SELDOS;
        ng.ng_TopEdge           = (yoffset[0] + yoffset[1]) / 2;

        gad = CreateGadget(CYCLE_KIND,          gad, &ng,
                                           GT_Underscore,       '_',
                                           GTCY_Labels,         cyltext,
                                           GTCY_Active,         COL_SELECTED,
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_SELDOS] = gad;
        AddKeyShortcut(FuncKeyboard, GID_SELDOS, MSG_SELDOS_GAD);

        ng.ng_Width                     = MaxTextLen(font, UseCancelUndoText) + 32;
        ng.ng_Height            = fonty + GadgetHeight;
        ng.ng_GadgetText        = MSG(MSG_USE_GAD);
        ng.ng_GadgetID          = GID_FUNCUSE;
        ng.ng_LeftEdge          = colpos[0];
        ng.ng_TopEdge           = yoffset[2] + heightadjust + (27 * spacing[2]) / 2;
        ng.ng_Flags                     = PLACETEXT_IN;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_FUNCUSE] = gad;
        AddKeyShortcut(FuncKeyboard, GID_FUNCUSE, MSG_USE_GAD);
        
        ng.ng_LeftEdge          = width - BorderRight - FUNC_MARGIN - ng.ng_Width;
        ng.ng_GadgetText        = MSG(MSG_CANCEL_GAD);
        ng.ng_GadgetID          = GID_FUNCCANCEL;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_FUNCCANCEL] = gad;
        AddKeyShortcut(FuncKeyboard, GID_FUNCCANCEL, MSG_CANCEL_GAD);

        ng.ng_LeftEdge          = (ng.ng_LeftEdge + BorderLeft + FUNC_MARGIN) / 2;
        ng.ng_GadgetText        = MSG(MSG_UNDO_GAD);
        ng.ng_GadgetID          = GID_FUNCUNDO;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fgad_failed;
        Gadget[GID_FUNCUNDO] = gad;
        AddKeyShortcut(FuncKeyboard, GID_FUNCUNDO, MSG_UNDO_GAD);
        
        if (topline != yoffset[2]) {
                /*
                 *              If there's room at the top of the window, create two text
                 *              gadgets with the column headings for the System and DOS cols.
                 */
                ng.ng_TopEdge           = topline;
                ng.ng_LeftEdge          = colpos[1];
                ng.ng_GadgetText        = "";
                ng.ng_GadgetID          = 0;

                gad = CreateGadget(TEXT_KIND,   gad, &ng,
                                                   GTTX_Text,   MSG(MSG_SYSFUNCS_GAD),
                                                   TAG_DONE);
                if (!gad)
                        goto fgad_failed;

                ng.ng_LeftEdge          = colpos[2];
                gad = CreateGadget(TEXT_KIND,   gad, &ng,
                                                   GTTX_Text,   MSG(MSG_DOSFUNCS_GAD),
                                                   TAG_DONE);
                if (!gad)
                        goto fgad_failed;
        }
        PurgeFuncGadgets = 0;   /* No need to purge now */
        CloseFont(font);
        return (gadlist);

fgad_failed:
        if (gadlist)
                FreeGadgets(gadlist);
        CloseFont(font);
        return (NULL);
}

/*
 *              OpenFunctionWindow()
 *
 *              Opens the functions window with all the functions gadgets.
 *              Returns TRUE for success, FALSE for failure.
 *              The window will contain all necessary gadgets.
 *
 *              As with the Settings window, we try a variety of fonts until we
 *              find one that fits onto the screen.
 */
int OpenFunctionWindow(void)
{
        static struct TextAttr funcfontattr =
                { "empty-storage-for-func-fonts...", 0, FS_NORMAL, FPB_DISKFONT};

        static int width;               /* Maintained from call to call */
        static int height;

        int left = CurSettings.FuncWinLeft;
        int top  = CurSettings.FuncWinTop;
        struct TextAttr *fontattr = NULL;
        struct Window *win;
        int i;

        CheckSegTracker();
        if (FuncWindow) {
                WindowToFront(FuncWindow);
                ActivateWindow(FuncWindow);
                return (TRUE);
        }
        if (!CheckForScreen())
                return (FALSE);

        SavedFuncSets = CurSettings;            /* Save so we can restore later */

        if (!FuncGadList) {
                /*
                 *              Find out what dimensions our new window should be; to do
                 *              this, we calculate the size with a variety of fonts until
                 *              we find one that works.
                 */
                int i;

                for (i = 0; fontattr = SubWindowFontList[i]; i++) {
                        if (CreateFunctionGadgets(fontattr, TRUE, &width, &height))
                                break;
                }
                if (!fontattr) {
                        ShowError(MSG(MSG_ERROR_OPENFUNC));
                        return (FALSE);
                }
                /*
                 *              We need to make a copy of the fontattr now so that if
                 *              the user changes the font, all our gadgets don't suddenly
                 *              inherit the new font (at least until the window has been
                 *              closed and re-opened!)
                 */
                strcpy(funcfontattr.ta_Name, fontattr->ta_Name);
                funcfontattr.ta_YSize = fontattr->ta_YSize;
                fontattr = &funcfontattr;
        }
        if (left == -1)  left = (ScreenWidth  - width)  / 2;
        if (top  == -1)  top  = (ScreenHeight - height) / 2;

        win = OpenWindowTags(NULL,
                                                 WA_Title,                      MSG(MSG_FUNCTION_TITLE),
                                                 WA_IDCMP,                      IDCMP_CLOSEWINDOW        |
                                                                                        IDCMP_REFRESHWINDOW      |
                                                                                        IDCMP_NEWSIZE            |
                                                                                        IDCMP_INACTIVEWINDOW |
                                                                                        IDCMP_RAWKEY             |
                                                                                        BUTTONIDCMP,
                                                 WA_Left,                       left,
                                                 WA_Top,                        top,
                                                 WA_Width,                      width,
                                                 WA_Height,                     height,
                                                 WA_Flags,                      WFLG_DRAGBAR             |
                                                                                        WFLG_DEPTHGADGET         |
                                                                                        WFLG_ACTIVATE            |
                                                                                        WFLG_RMBTRAP             |
                                                                                        WFLG_NEWLOOKMENUS,
                                                 RefreshTag,            TRUE,
                                                 WA_NoCareRefresh,      NoCareRefreshBool,
                                                 WA_PubScreen,          SnoopScreen,
                                                 TAG_DONE);
        if (!win) {
                ShowError(MSG(MSG_ERROR_OPENFUNC));
                return (FALSE);
        }

        if (!FuncGadList) {
                FuncGadList = CreateFunctionGadgets(fontattr, 0, 0, 0);
                if (!FuncGadList) {
                        CloseWindow(win);
                        ShowError(MSG(MSG_ERROR_OPENFUNC));
                        return (FALSE);
                }
        }

        /*
         *              Now update function gadgets to reflect current settings
         *              for AmigaDOS functions. Under V39+, we can do this before
         *              adding them to the window. Under V37, we have to do it
         *              after adding them to the window.
         */
        if (GadToolsBase->lib_Version < 39) {
                AddGList(win, FuncGadList, -1, -1, NULL);
                RefreshGList(FuncGadList, win, NULL, -1);
                GT_RefreshWindow(win, NULL);
                for (i = FIRST_BOOL_GADGET; i <= LAST_BOOL_GADGET; i++) {
                        if (Gadget[i]) {
                                GT_SetGadgetAttrs(Gadget[i], win, NULL,
                                                                  GTCB_Checked, CurSettings.Func.Opts[i],
                                                                  TAG_DONE);
                        }
                }
        } else { /* GadToolsBase->lib_Version >= 39 */
                for (i = FIRST_BOOL_GADGET; i <= LAST_BOOL_GADGET; i++) {
                        if (Gadget[i]) {
                                GT_SetGadgetAttrs(Gadget[i], NULL, NULL,
                                                                  GTCB_Checked, CurSettings.Func.Opts[i],
                                                                  TAG_DONE);
                        }
                }
                AddGList(win, FuncGadList, -1, -1, NULL);
                RefreshGList(FuncGadList, win, NULL, -1);
                GT_RefreshWindow(win, NULL);
        }

        /*
         *              All done, so initialise some globals and return
         */
        FuncWindow              = win;
        FuncWindowPort  = win->UserPort;
        FuncWindowMask  = 1 << FuncWindowPort->mp_SigBit;
        FuncSystemCol   = COL_SELECTED;
        FuncDOSCol              = COL_SELECTED;
        if (DisableNestCount)
                DisableWindow(FuncWindow, &FuncRequester);
        return (TRUE);
}

/*
 *              CreateSettingsGadgets(fontattr, getsize, &pwidth, &pheight)
 *
 *              Creates the gadgets list for the settings window using the specified
 *              window font. If the window would be to big for the current screen
 *              using this font, or if there is a problem creating the gadgets,
 *              returns NULL.
 *
 *              The returned gadget list should be passed to FreeGadgets() when
 *              it is no longer required.
 *
 *              If getsize is true, then pwidth and pheight are filled in with the
 *              dimensions of the window size needed to hold the gadgets, but
 *              no actual gadget creation is carried out. In this case, NULL is
 *              returned for a failure (couldn't open font, or window size would
 *              exceed current screen size) and non-NULL for success.
 */
struct Gadget *CreateSettingsGadgets(struct TextAttr *fontattr,
                                                                         int getsize, int *pwidth, int *pheight)
{
        struct SetGadgets *sg;
        struct NewGadget ng;
        struct NewGadget ngbut;
        struct TextFont *font;
        struct Gadget *gadlist;
        struct Gadget *gad;
        UWORD spacing;
        UWORD yoffset;
        UWORD colpos[2];
        UWORD colwidth[3];
        int miniwidth;
        int width;
        int height;
        int gheight;
        int fonty;
        int fontx;

        if (!SetVI) {
                SetVI = GetVisualInfoA(SnoopScreen, NULL);
                if (!SetVI)
                        return (NULL);
        }
        font = MyOpenFont(fontattr);
        if (!font)
                return (NULL);
        
        fonty           = font->tf_YSize;
        fontx           = font->tf_XSize;

        gheight         = fonty + GadgetHeight;
        spacing     = fonty + GadgetSpacing;
        yoffset         = TitlebarHeight + fonty/2;
        colpos[0]       = BorderLeft + SET_MARGIN
                                  + MaxTextLen(font, SettingsLeftNames) + 7;
        colwidth[0] = MaxTextLen(font, SettingsLeftContents) + 40;
        colpos[1]       = colpos[0] + colwidth[0] + SET_MARGIN*2
                                                          + MaxTextLen(font, SettingsRightNames);
        colwidth[1] = fontx * 24; /* Room for about 24 chars in string gadgets */
        colwidth[2] = colwidth[1] + colpos[1] - colpos[0];      /* Dual width */

        /*
         *              See SetMiniButtons[] definitions for explanation of this bit
         */
        if (font->tf_Flags & FPF_PROPORTIONAL) {
                miniwidth = MaxTextLen(font, SetMiniButtonsP) + 20;
                SetGadgs[1].data1 = MSG_FORMATCOPYP_GAD;
        } else {
                miniwidth = MaxTextLen(font, SetMiniButtonsF) + 20;
                SetGadgs[1].data1 = MSG_FORMATCOPYF_GAD;
        }

        width           = colpos[1] + colwidth[1] + SET_MARGIN + BorderRight;
        height      = yoffset + spacing * 8 + BorderBottom;

        if (width > ScreenWidth || height >ScreenHeight) {
                CloseFont(font);
                return (NULL);
        }
        if (getsize) {
                *pwidth  = width;
                *pheight = height;
                CloseFont(font);
                return (struct Gadget *)(-1);
        }

        /*
         *              Okay, looks like our size is okay so now prime our gadget
         *              table with the current settings values to use when the
         *              gadget is created.
         *
         */
#define DEF_CYCLE(x)    SetGadgs[SG_##x].data1
#define DEF_STRING(x)   SetGadgs[SG_##x].data2

        DEF_CYCLE(HideMethod)     = CurSettings.Setup.HideMethod;
        DEF_CYCLE(ScreenType)     = CurSettings.Setup.ScreenType;
        DEF_CYCLE(LogMode)                = CurSettings.Setup.LogMode;
        DEF_CYCLE(FileIOType)     = CurSettings.Setup.FileIOType;
        DEF_STRING(HotKey)                = CurSettings.Setup.HotKey;
        DEF_STRING(ScreenName)    = CurSettings.Setup.ScreenName;
        DEF_STRING(LogFile)               = CurSettings.Setup.LogFile;
        DEF_STRING(WindowFont)    = GetFontDesc(WindowFontDesc,
                                                                                    WindowFontName, WindowFontSize);
        DEF_STRING(BufferFont)    = GetFontDesc(BufferFontDesc,
                                                                                BufferFontName, BufferFontSize);
        DEF_STRING(BufferFormat)  = BufFormat;
        DEF_STRING(LogfileFormat) = LogFormat;

        /*
         *              Next, onto the gadget creation itself
         */
        ng.ng_Height            = gheight; 
        ng.ng_TextAttr          = fontattr;
        ng.ng_VisualInfo        = SetVI;
        ng.ng_Flags         = PLACETEXT_LEFT;

        ngbut.ng_Height         = gheight; 
        ngbut.ng_TextAttr       = fontattr;
        ngbut.ng_VisualInfo     = SetVI;
        ngbut.ng_Flags          = PLACETEXT_IN;

        FileButtonImage = CreateCustomImage(IMAGE_FILE, gheight+2-2*SquareAspect);
        FontButtonImage = CreateCustomImage(IMAGE_FONT, gheight);
        if (!FileButtonImage || !FontButtonImage) {
                CloseFont(font);
                return (NULL);
        }
        gadlist = NULL;
        gad = CreateContext(&gadlist);
        if (!gad) {
                CloseFont(font);
                return (NULL);
        }

        for (sg = &SetGadgs[0]; sg->gadgid; sg++) {
                ng.ng_LeftEdge          = colpos[sg->col];
                ng.ng_Width                     = colwidth[sg->cwidth];
                ng.ng_TopEdge           = yoffset + sg->row * spacing;
                ng.ng_GadgetText        = MSG(sg->stringid);
                ng.ng_GadgetID          = sg->gadgid;
                ng.ng_Height            = gheight;

                if (sg->promptid) {
                        /*
                         *              Create optional mini gadget to right of main gadget.
                         *              If data1 == 0 or data1 == 1 then use a file or font
                         *              BOOPSI gadget image, else use a Gadtools text button
                         *              containing the label corresponding to MSG(data1) else
                         *              use string ID indicated by data1
                         */
                        ngbut.ng_GadgetID  = sg->promptid;
                        ngbut.ng_TopEdge   = ng.ng_TopEdge;
                        ngbut.ng_Height    = gheight;
                        if (sg->gtype == STRING_KIND && !SquareAspect) {
                                ngbut.ng_Height  += 2;
                                ngbut.ng_TopEdge -= 1;
                        }
                        if (sg->data1 > SETBUTTON_MAX) {
                                /*
                                 *              Creating a text gadget
                                 */
                                ngbut.ng_LeftEdge   = ng.ng_LeftEdge + ng.ng_Width - miniwidth;
                                ngbut.ng_Width          = miniwidth;
                                ngbut.ng_GadgetText = MSG(sg->data1);
                                gad = CreateGadget(BUTTON_KIND, gad, &ngbut,
                                                                   GT_Underscore, '_',
                                                                   TAG_DONE);
                                if (!gad)
                                        goto sgad_failed;
                                AddKeyShortcut(SetKeyboard, sg->promptid, sg->data1);
                        } else {
                                /*
                                 *              Creating an image gadget
                                 */
                                struct Image *img = FileButtonImage;

                                if (sg->data1 == SETBUTTON_FONT)
                                        img = FontButtonImage;

                                ngbut.ng_Width          = img->Width;
                                ngbut.ng_GadgetText = NULL;
                                ngbut.ng_LeftEdge   = ng.ng_LeftEdge + ng.ng_Width -img->Width;
                                gad = CreateGadget(GENERIC_KIND, gad, &ngbut, TAG_DONE);
                                if (!gad)
                                        goto sgad_failed;

                                /*
                                 *              Now fill in the details of our gadget
                                 */
                                gad->Flags                      |= GFLG_GADGIMAGE | GFLG_GADGHIMAGE;
                                gad->Activation         |= GACT_IMMEDIATE | GACT_RELVERIFY;
                                gad->GadgetType         |= GTYP_BOOLGADGET;
                                gad->GadgetRender       = img;
                                gad->SelectRender       = img + 1;
                        }
                        Gadget[sg->promptid] = gad;
                        ng.ng_Width     -= ngbut.ng_Width + 2;
                }
                switch (sg->gtype) {
                        case STRING_KIND:
                                if (!SquareAspect) {
                                        ng.ng_Height  += 2;
                                        ng.ng_TopEdge -= 1;
                                }
                                gad = CreateGadget(STRING_KIND, gad, &ng,
                                                                   GT_Underscore,       '_',
                                                                   GTST_String,         sg->data2,
                                                                   GTST_MaxChars,       MAX_STR_LEN,
                                                                   TAG_DONE);
                                break;

                        case TEXT_KIND:
                                ng.ng_Width-= 2;
                                ng.ng_LeftEdge++;
                                gad = CreateGadget(TEXT_KIND, gad, &ng,
                                                                   GT_Underscore,       '_',
                                                                   GTTX_Text,           sg->data2,
                                                                   GTTX_Border,         TRUE,
                                                                   TAG_DONE);
                                break;

                        case CYCLE_KIND:
                        {
                                int *msgid = sg->data2;
                                char **msg = sg->data3;
                                int i;

                                for (i = 0; msgid[i]; i++)
                                        msg[i] = MSG(msgid[i]);
                                msg[i] = NULL;
                                gad = CreateGadget(CYCLE_KIND, gad, &ng,
                                                                   GT_Underscore,       '_',
                                                                   GTCY_Active,         sg->data1,
                                                                   GTCY_Labels,         msg,
                                                                   TAG_DONE);
                                break;
                        }
                }
                if (!gad)
                        goto sgad_failed;
                Gadget[sg->gadgid] = gad;
                AddKeyShortcut(SetKeyboard, sg->gadgid, sg->stringid);
        }

        ng.ng_GadgetText        = MSG(MSG_BUFFERSIZE_GAD);
        ng.ng_GadgetID          = GID_BUFFERSIZE;
        ng.ng_TopEdge           = yoffset + spacing * 4 + (1 - SquareAspect);
        ng.ng_Height            = gheight + (2 - 2 * SquareAspect);
        ng.ng_Width                     = GetTextLen(font, "10000") + 14;
        ng.ng_LeftEdge          = colpos[0] + colwidth[0] - ng.ng_Width;

        gad = CreateGadget(INTEGER_KIND,        gad, &ng,
                                           GT_Underscore,       '_',
                                           GTIN_Number,         CurSettings.Setup.BufferSize,
                                           TAG_DONE);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_BUFFERSIZE] = gad;
        AddKeyShortcut(SetKeyboard, GID_BUFFERSIZE, MSG_BUFFERSIZE_GAD);

        ng.ng_Width                     = MaxTextLen(font, UseCancelUndoText) + 32;
        ng.ng_Height            = fonty + GadgetHeight;
        ng.ng_GadgetText        = MSG(MSG_USE_GAD);
        ng.ng_GadgetID          = GID_SETUSE;
        ng.ng_LeftEdge          = BorderLeft + SET_MARGIN;
        ng.ng_TopEdge           = yoffset + 7 * spacing;
        ng.ng_Flags                     = PLACETEXT_IN;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_SETUSE] = gad;
        AddKeyShortcut(SetKeyboard, GID_SETUSE, MSG_USE_GAD);
        
        ng.ng_LeftEdge          = width - BorderRight - SET_MARGIN - ng.ng_Width;
        ng.ng_GadgetText        = MSG(MSG_CANCEL_GAD);
        ng.ng_GadgetID          = GID_SETCANCEL;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_SETCANCEL] = gad;
        AddKeyShortcut(SetKeyboard, GID_SETCANCEL, MSG_CANCEL_GAD);
        
        ng.ng_LeftEdge          = (ng.ng_LeftEdge + BorderLeft + SET_MARGIN) / 2;
        ng.ng_GadgetText        = MSG(MSG_UNDO_GAD);
        ng.ng_GadgetID          = GID_SETUNDO;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto sgad_failed;
        Gadget[GID_SETUNDO] = gad;
        AddKeyShortcut(SetKeyboard, GID_SETUNDO, MSG_UNDO_GAD);

        CloseFont(font);
        return (gadlist);

sgad_failed:
        if (gadlist)
                FreeGadgets(gadlist);
        CloseFont(font);
        return (FALSE);
}

/*
 *              OpenSettingsWindow()
 *
 *              Opens the settings window with all the settings gadgets.
 *              The window will contain all necessary gadgets. Note that
 *              since gadgets can take some time to create on 68000 systems,
 *              we cache the gadget list so that on subsequent re-opens of
 *              the window, we don't need to recreate them all.
 *
 *              Returns TRUE for success, false for failure.
 */
int OpenSettingsWindow(void)
{
        static struct TextAttr setfontattr =
                { "empty-storage-for-setup-fonts..", 0, FS_NORMAL, FPB_DISKFONT};

        static int width;               /* Maintained from call to call */
        static int height;

        int left = CurSettings.SetupWinLeft;
        int top  = CurSettings.SetupWinTop;
        struct TextAttr *fontattr = NULL;
        struct Window *win;

        CheckSegTracker();
        if (SetWindow) {
                WindowToFront(SetWindow);
                ActivateWindow(SetWindow);
                return (TRUE);
        }
        if (!CheckForScreen())
                return (FALSE);

        SavedSetupSets = CurSettings;           /* Save so we can restore later */

        if (!SetGadList) {
                /*
                 *              Find out what dimensions our new window should be; to do
                 *              this, we calculate the size with a variety of fonts until
                 *              we find one that works.
                 */
                int i;

                for (i = 0; fontattr = SubWindowFontList[i]; i++) {
                        if (CreateSettingsGadgets(fontattr, TRUE, &width, &height))
                                break;
                }
                if (!fontattr) {
                        ShowError(MSG(MSG_ERROR_OPENSET));
                        return (FALSE);
                }
                /*
                 *              We need to make a copy of the fontattr now so that if
                 *              the user changes the font, all our gadgets don't suddenly
                 *              inherit the new font (at least until the window has been
                 *              closed and re-opened!)
                 */
                strcpy(setfontattr.ta_Name, fontattr->ta_Name);
                setfontattr.ta_YSize = fontattr->ta_YSize;
                fontattr = &setfontattr;
        }
        if (left == -1)  left = (ScreenWidth  - width)  / 2;
        if (top  == -1)  top  = (ScreenHeight - height) / 2;

        win = OpenWindowTags(NULL,
                                                 WA_Title,                      MSG(MSG_SETTINGS_TITLE),
                                                 WA_IDCMP,                      IDCMP_CLOSEWINDOW        |
                                                                                        IDCMP_REFRESHWINDOW      |
                                                                                        IDCMP_NEWSIZE            |
                                                                                        IDCMP_RAWKEY             |
                                                                                        IDCMP_INACTIVEWINDOW |
                                                                                        BUTTONIDCMP,
                                                 WA_Left,                       left,
                                                 WA_Top,                        top,
                                                 WA_Width,                      width,
                                                 WA_Height,                     height,
                                                 WA_Flags,                      WFLG_DRAGBAR             |
                                                                                        WFLG_DEPTHGADGET         |
                                                                                        WFLG_ACTIVATE            |
                                                                                        WFLG_RMBTRAP             |
                                                                                        WFLG_NEWLOOKMENUS,
                                                 RefreshTag,            TRUE,
                                                 WA_NoCareRefresh,      NoCareRefreshBool,
                                                 WA_PubScreen,          SnoopScreen,
                                                 TAG_DONE);
        if (!win) {
                ShowError(MSG(MSG_ERROR_OPENSET));
                return (FALSE);
        }
        if (!SetGadList) {
                SetGadList = CreateSettingsGadgets(fontattr, 0, 0, 0);
                if (!SetGadList) {
                        CloseWindow(win);
                        ShowError(MSG(MSG_ERROR_OPENSET));
                        return (FALSE);
                }
        }
        AddGList(win, SetGadList, -1, -1, NULL);
        RefreshGList(SetGadList, win, NULL, -1);
        GT_RefreshWindow(win, NULL);

        SetWindow               = win;
        SetWindowPort   = win->UserPort;
        SetWindowMask   = 1 << SetWindowPort->mp_SigBit;
        if (DisableNestCount)
                DisableWindow(SetWindow, &SetRequester);
        return (TRUE);
}

/*
 *              CloseFunctionWindow(void)
 *
 *              Closes the functions window. Note that we do NOT free the gadget
 *              list in this case; since it takes so long to build, we leave it
 *              alone for the benefit of future opens.
 */
void CloseFunctionWindow(void)
{
        if (FuncWindow) {
                RecordWindowSizes();
                CloseWindow(FuncWindow);
                FuncWindow     = NULL;
                FuncWindowMask = 0;
        }
        if (PurgeFuncGadgets && FuncGadList) {
                FreeGadgets(FuncGadList);
                FuncGadList        = NULL;
        }
        if (PurgeFuncGadgets && FuncVI) {
                FreeVisualInfo(FuncVI);
                FuncVI = NULL;
        }
        PurgeFuncGadgets = 0;
}

/*
 *              CloseSettingsWindow(void)
 *
 *              Closes the settings window, and free gadgets associated with window.
 */
void CloseSettingsWindow(void)
{
        if (SetWindow) {
                RecordWindowSizes();
                CloseWindow(SetWindow);
                SetWindow     = NULL;
                SetWindowMask = 0;
        }
        if (SetGadList) {
                FreeGadgets(SetGadList);
                SetGadList = NULL;
        }
        if (FileButtonImage) {
                FreeCustomImage(FileButtonImage);
                FileButtonImage = NULL;
        }
        if (FontButtonImage) {
                FreeCustomImage(FontButtonImage);
                FontButtonImage = NULL;
        }
        if (SetVI) {
                FreeVisualInfo(SetVI);
                SetVI = NULL;
        }
}
 
/*
 *              HandleSettingsMsgs()
 *
 *              Processes all outstanding messages associated with the Settings
 *              gadget window. To be called whenever we get a settings event.
 */

#define STRVAL(gid)     (((struct StringInfo *)(Gadget[gid]->SpecialInfo))->Buffer)

void HandleSettingsMsgs(void)
{
        static int activegad;                   /* Non-zero if gadget is depressed   */
        static int activekey;                   /* Keycode shortcut of activegad     */

        struct IntuiMessage *imsg;
        int doneset = SEL_NONE;
        char *str;
        int val;

        if (!SetWindow)
                return;

        while ((imsg = GT_GetIMsg(SetWindowPort)) != NULL) {
                struct Gadget *gad;
                int gadid;
                int shift  = (imsg->Qualifier & IE_SHIFT);
                int newval;

                switch (imsg->Class) {
                        
                        case IDCMP_CLOSEWINDOW:
                                doneset = SEL_USE;
                                break;

                        case IDCMP_REFRESHWINDOW:
                                GT_BeginRefresh(SetWindow);
                                GT_EndRefresh(SetWindow, TRUE);
                                break;

                        case IDCMP_MOUSEMOVE: 
                        case IDCMP_GADGETDOWN:
                        case IDCMP_GADGETUP:
                                gad    = (struct Gadget *)imsg->IAddress;
                                gadid  = gad->GadgetID;
                                newval = imsg->Code;

handle_set_gads:
                                switch (gadid) {

                                        case GID_FILEPROMPT:
                                        {
                                                char *defname = "";

                                                /*
                                                 *              Choose a default name for the logfile
                                                 */
                                                if (IsFileSystem(DefaultLogName))
                                                        defname = DefaultLogName;

                                                if (SelectFile(DefaultLogName, defname, SetWindow,
                                                                           FILESEL_DEFLOGNAME))
                                                {
                                                        /*
                                                         *              Got a new default filename, so update
                                                         *              the logfile gadget accordingly
                                                         */
                                                        GT_SetGadgetAttrs(Gadget[GID_LOGFILE],
                                                                                          SetWindow, NULL,
                                                                                          GTST_String, DefaultLogName,
                                                                                          TAG_DONE);
                                                }
                                                break;
                                        }

                                        case GID_WFONTPROMPT:
                                                /*
                                                 *              Choose a new gadget (window) font
                                                 */
                                                if (SelectFont(SetWindow, FONTSEL_WINDOW)) {
                                                        strcpy(WindowFontName, WindowFR->fo_Attr.ta_Name);
                                                        WindowFontSize = WindowFR->fo_Attr.ta_YSize;
                                                        WindowFontAttr.ta_YSize = WindowFontSize;

                                                        GT_SetGadgetAttrs(
                                                                Gadget[GID_WINDOWFONT], SetWindow, NULL,
                                                                GTTX_Text, GetFontDesc(WindowFontDesc,
                                                                                                           WindowFontName,
                                                                                                           WindowFontSize),
                                                                TAG_DONE);
                                                        ReOpenMainWindow();
                                                        WindowToFront(SetWindow);
                                                        /*
                                                         *              Since we allow the function gadgets
                                                         *              to hang around even when the function
                                                         *              window closes, we need to free the
                                                         *              gadgets when we change fonts to force
                                                         *              them to be regenerated in the new
                                                         *              font.
                                                         */             
                                                        if (!FuncWindow) {
                                                                FreeGadgets(FuncGadList);
                                                                FuncGadList              = NULL;
                                                        } else {
                                                                PurgeFuncGadgets = TRUE;
                                                        }
                                                }
                                                break;

                                        case GID_BFONTPROMPT:
                                                /*
                                                 *              Choose a new buffer font
                                                 */
                                                if (SelectFont(SetWindow, FONTSEL_BUFFER)) {
                                                        strcpy(BufferFontName, BufferFR->fo_Attr.ta_Name);
                                                        BufferFontSize = BufferFR->fo_Attr.ta_YSize;
                                                        BufferFontAttr.ta_YSize = BufferFontSize;

                                                        GT_SetGadgetAttrs(
                                                                Gadget[GID_BUFFERFONT],
                                                                SetWindow, NULL,
                                                                GTTX_Text, GetFontDesc(BufferFontDesc,
                                                                                                           BufferFontName,
                                                                                                           BufferFontSize),
                                                                TAG_DONE);
                                                        ReOpenMainWindow();
                                                        WindowToFront(SetWindow);
                                                }
                                                break;

                                        case GID_FORMATEDIT:
                                                OpenFormatWindow();
                                                break;

                                        case GID_FORMATCOPY:
                                                if (shift)
                                                        *LogFormat = '\0';
                                                else
                                                        strcpy(LogFormat, BufFormat);
                                                GT_SetGadgetAttrs(Gadget[GID_LOGFORMAT],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, LogFormat,
                                                                                  TAG_DONE);
                                                break;

                                        case GID_FILEIO:
                                                CurSettings.Setup.FileIOType = newval;
                                                break;

                                        case GID_OPENON:
                                                CurSettings.Setup.ScreenType = newval;
                                                break;

                                        case GID_LOGMODE:
                                                CurSettings.Setup.LogMode        = newval;
                                                SetLogGadget(newval, LG_REFRESH);
                                                break;

                                        case GID_HIDEMETHOD:
                                                if (!CxBase)
                                                        newval = 0;

                                                if (newval != CurSettings.Setup.HideMethod) {
                                                        int oldval = CurSettings.Setup.HideMethod;

                                                        CurSettings.Setup.HideMethod = newval;
                                                        if (newval == HIDE_NONE)
                                                                CleanupHotKey();
                                                        else if (oldval == HIDE_NONE) {
                                                                /*
                                                                 *              Switching out of hide state so
                                                                 *              re-activate hotkey
                                                                 */
                                                                InstallHotKey(CurSettings.Setup.HotKey);
                                                        }
                                                        SetMainHideState(newval);
                                                }
                                                break;

                                        case GID_HOTKEY:
                                        {
                                                char *msg = CurSettings.Setup.HotKey;

                                                strcpy(msg, STRVAL(GID_HOTKEY));
                                                InstallHotKey(msg);
                                                SetMainHideState(CurSettings.Setup.HideMethod);
                                                break;
                                        }

                                        case GID_SCREENNAME:
                                                strcpy(CurSettings.Setup.ScreenName,
                                                           STRVAL(GID_SCREENNAME));
                                                break;

                                        case GID_LOGFILE:
                                                strcpy(CurSettings.Setup.LogFile, STRVAL(GID_LOGFILE));
                                                break;

                                        case GID_LOGFORMAT:
                                                strcpy(CurSettings.Setup.LogfileFormat,
                                                           STRVAL(GID_LOGFORMAT));
                                                break;

                                        case GID_BUFFORMAT:
                                                str = STRVAL(GID_BUFFORMAT);
                                                if (strcmp(BufFormat, str) != 0) {
                                                        strcpy(BufFormat, str);
                                                        InstallNewFormat(NEW_STRING);
                                                }
                                                break;

                                        case GID_BUFFERSIZE:
                                                /*
                                                 *              We don't do anything here at all ... instead,
                                                 *              the update is handled when the user clicks
                                                 *              on USE.
                                                 */
                                                break;

                                        case GID_SETUSE:
                                                doneset = SEL_USE;
                                                break;

                                        case GID_SETCANCEL:
                                                doneset = SEL_CANCEL;
                                                break;

                                        case GID_SETUNDO:
                                                /*
                                                 *              Major bummer -- we need to manually update
                                                 *              ALL the string gadgets, since if the user
                                                 *              modified one of them and didn't press RETURN
                                                 *              we won't have picked it up and so we won't
                                                 *              know to undo it :-(
                                                 */
                                                InstallSettings(&SavedSetupSets, SET_SETUP);

#define UndoSet SavedSetupSets.Setup

                                                GT_SetGadgetAttrs(Gadget[GID_HOTKEY],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, UndoSet.HotKey,
                                                                                  TAG_DONE);
                                                GT_SetGadgetAttrs(Gadget[GID_SCREENNAME],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, UndoSet.ScreenName,
                                                                                  TAG_DONE);
                                                GT_SetGadgetAttrs(Gadget[GID_LOGFILE],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, UndoSet.LogFile,
                                                                                  TAG_DONE);
                                                GT_SetGadgetAttrs(Gadget[GID_BUFFORMAT],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, UndoSet.BufferFormat,
                                                                                  TAG_DONE);
                                                GT_SetGadgetAttrs(Gadget[GID_LOGFORMAT],
                                                                                  SetWindow, NULL,
                                                                                  GTST_String, UndoSet.LogfileFormat,
                                                                                  TAG_DONE);
                                                GT_SetGadgetAttrs(Gadget[GID_BUFFERSIZE],
                                                                                  SetWindow, NULL,
                                                                                  GTIN_Number, UndoSet.BufferSize,
                                                                                  TAG_DONE);
                                                break;
                                }
                                break;

                        case IDCMP_INACTIVEWINDOW:
                                if (activegad) {
                                        ShowGadget(SetWindow, Gadget[activegad], GADGET_UP);
                                        activegad = 0;
                                }
                                break;

                        case IDCMP_RAWKEY:
                        {
                                int upstroke = imsg->Code & 0x80;
                                int keypress = ConvertIMsgToChar(imsg);

                                gadid  = SetKeyboard[keypress];
                                if (activegad) {
                                        /*
                                         *              We're releasing a gadget that was pressed
                                         *              earlier, so handle it now
                                         */
                                        int samekey = (imsg->Code & 0x7f) == activekey;

                                        if (samekey && !upstroke)       /* Ignore repeated keys */
                                                break;

                                        ShowGadget(SetWindow, Gadget[activegad], GADGET_UP);
                                        if (samekey) {
                                                /*
                                                 *              Just released the key that was originally
                                                 *              pressed for this gadget, so now go and
                                                 *              handle the gadget action.
                                                 */
                                                gadid     = activegad;
                                                gad       = Gadget[gadid];
                                                activegad = 0;
                                                goto handle_set_gads;
                                        }

                                        /*
                                         *              If the above check didn't kick in, it means
                                         *              we got a downpress of a different key, so
                                         *              disable the gadget for this key and continue
                                         *              to press the other key.
                                         */
                                        activegad = 0;
                                }

                                if (imsg->Code == HELPKEY) {
                                        ShowAGuide(MSG(MSG_LINK_SETUPWIN));
                                        break;
                                }

                                /*
                                 *              Handle keyboard equivalent keypresses for gadgets
                                 */
                                if (gadid) {
                                        int numopts;

                                        gad = Gadget[gadid];

                                        switch (gadid) {
                                                case GID_HIDEMETHOD:
                                                        newval  = CurSettings.Setup.HideMethod;
                                                        numopts = HIDE_MAX;
                                                        goto handle_set_cycle;

                                                case GID_OPENON:
                                                        newval  = CurSettings.Setup.ScreenType;
                                                        numopts = SCREEN_MAX;
                                                        goto handle_set_cycle;

                                                case GID_LOGMODE:
                                                        newval  = CurSettings.Setup.LogMode;
                                                        numopts = LOGMODE_MAX;
                                                        goto handle_set_cycle;

                                                case GID_FILEIO:
                                                        newval  = CurSettings.Setup.FileIOType;
                                                        numopts = FILE_MAX;
handle_set_cycle:
                                                        newval++;
                                                        if (shift)
                                                                newval -= 2;
                                                        if (newval < 0)                 newval += numopts;
                                                        if (newval >= numopts)  newval -= numopts;
                                                        GT_SetGadgetAttrs(gad, SetWindow, NULL,
                                                                                          GTCY_Active, newval,
                                                                                          TAG_DONE);
                                                        goto handle_set_gads;

                                                case GID_BUFFERSIZE:
                                                case GID_BUFFORMAT:
                                                case GID_LOGFORMAT:
                                                case GID_HOTKEY:
                                                case GID_SCREENNAME:
                                                case GID_LOGFILE:
                                                        if (gadid != GID_LOGFILE || !shift) {
                                                                ActivateGadget(gad, SetWindow, NULL);
                                                                break;
                                                        }
                                                        /*
                                                         *              User pressed GID_LOGFILE with SHIFT
                                                         *              pressed, so change gadget type to
                                                         *              GID_FILEPROMPT and fall through to
                                                         *              button code.
                                                         */
                                                        gadid = GID_FILEPROMPT;
                                                        /* Fall through */

                                                case GID_SETUSE:
                                                case GID_SETUNDO:
                                                case GID_SETCANCEL:
                                                case GID_FORMATCOPY:
                                                case GID_FORMATEDIT:
                                                case GID_WINDOWFONT:
                                                case GID_BUFFERFONT:
                                                case GID_WFONTPROMPT:
                                                case GID_BFONTPROMPT:
                                                        if (gadid == GID_WINDOWFONT)
                                                                gadid = GID_WFONTPROMPT;
                                                        if (gadid == GID_BUFFERFONT)
                                                                gadid = GID_BFONTPROMPT;
                                                        gad = Gadget[gadid];
                                                        ShowGadget(SetWindow, gad, GADGET_DOWN);
                                                        activegad = gadid;
                                                        activekey = imsg->Code;
                                                        break;
                                        }
                                }
                                break;
                        }
                }
                GT_ReplyIMsg(imsg);
        }
        /*
         *              Handled all messages. Now check for possible USE/CANCEL
         */
        if (doneset == SEL_USE) {
                /*
                 *              Use the current settings. To be safe, we update all
                 *              our string gadgets in case they were modified but
                 *              we didn't catch the modification (i.e. user didn't press
                 *              Return).
                 */
                str = STRVAL(GID_BUFFORMAT);
                if (strcmp(BufFormat, str) != 0) {
                        strcpy(BufFormat, str);
                        InstallNewFormat(NEW_STRING);
                }
                strcpy(CurSettings.Setup.LogFile,                STRVAL(GID_LOGFILE));
                strcpy(CurSettings.Setup.ScreenName,     STRVAL(GID_SCREENNAME));
                strcpy(CurSettings.Setup.HotKey,                 STRVAL(GID_HOTKEY));
                strcpy(CurSettings.Setup.LogfileFormat,  STRVAL(GID_LOGFORMAT));
                strcpy(CurSettings.Setup.LogfileFormat,  STRVAL(GID_LOGFORMAT));
                val = atoi(STRVAL(GID_BUFFERSIZE));
                CloseSettingsWindow();
                /*
                 *              Now check if we've got a new buffer size --
                 *              if so, install it.
                 */
                if (val < 1)
                        val = 1;

                if (val != SavedSetupSets.Setup.BufferSize) {
                        CurSettings.Setup.BufferSize = val;
                        ClearWindowBuffer();
                        if (!SetTotalBufferSize(val * 1024, SETBUF_FORCENEW)) {
                                ShowError(MSG(MSG_ERROR_NOBUFMEM), val);
                                Cleanup(20);
                        }
                }
        } else if (doneset == SEL_CANCEL) {
                /*
                 *              Cancel the current settings, and restore those that
                 *              were in effect before we started this. We close
                 *              the settings window first in case the main window
                 *              needs to close and re-open (change of font) since
                 *              this looks cleaner.
                 *
                 *              Note that even though we saved the entire set of settings,
                 *              we only restore the SETUP settings, not all of them.
                 */
                CloseSettingsWindow();
                InstallSettings(&SavedSetupSets, SET_SETUP);
        }
}

/*
 *              ShowFuncOpts(options, firstid, lastid)
 *
 *              Updates the function window gadgets to reflect the new values
 *              in the specified options array.
 *
 *              The new options are compared with the existing options to see
 *              if they've changed, so you should make sure to call this function
 *              _before_ copying the new options onto the current settings.
 *
 *              Only gadgets from the first to the last ID given are affected.
 */
void ShowFuncOpts(UBYTE newopts[], int firstid, int lastid)
{
        int i;

        for (i = firstid; i <= lastid; i++) {
                if (Gadget[i] != NULL && newopts[i] != CurSettings.Func.Opts[i]) {
                        GT_SetGadgetAttrs(Gadget[i], FuncWindow, NULL,
                                                          GTCB_Checked, newopts[i],
                                                          TAG_DONE);
                }
        }
}

/*
 *              ResetFuncToSelected()
 *
 *              Changes the two cycle gadgets in the function window back to the
 *              SELECTED state, and updates the variables accordingly. Note
 *              that doesn't change the actual boolean settings themselves; in
 *              effect, it makes the current settings permanent by stopping you
 *              switching back to the SELECTED state to restore an older set
 *              by clicking on the cycle gadget yourself.
 */
void ResetFuncToSelected(void)
{
        if (FuncSystemCol != COL_SELECTED) {
                GT_SetGadgetAttrs(Gadget[GID_SELSYSTEM],
                                                  FuncWindow, NULL,
                                                  GTCY_Active, COL_SELECTED,
                                                  TAG_DONE);
        }
        if (FuncDOSCol != COL_SELECTED) {
                GT_SetGadgetAttrs(Gadget[GID_SELDOS],
                                                  FuncWindow, NULL,
                                                  GTCY_Active, COL_SELECTED,
                                                  TAG_DONE);
        }
        FuncSystemCol = COL_SELECTED;
        FuncDOSCol    = COL_SELECTED;
}

/*
 *              HandleFuncMsgs()
 *
 *              Processes all outstanding messages associated with the Functions
 *              gadget window. To be called whenever we get a functions event.
 *
 *              The only slightly complicated bit here concerns the two cycle
 *              gadgets which can be used to select all or none of either of
 *              the DOS and System columns, or to restore a previous setting
 *              installed before.
 */
void HandleFuncMsgs(void)
{
        static int activegad;                   /* Non-zero if gadget is depressed   */
        static int activekey;                   /* Keycode shortcut of activegad     */

        struct IntuiMessage *imsg;
        int donefunc = SEL_NONE;

        if (!FuncWindow)
                return;

        while ((imsg = GT_GetIMsg(FuncWindowPort)) != NULL) {
                struct Gadget *gad;
                int shift = (imsg->Qualifier & IE_SHIFT);
                LONG gadid;
                int newval;

                switch (imsg->Class) {
                        
                        case IDCMP_REFRESHWINDOW:
                                GT_BeginRefresh(FuncWindow);
                                GT_EndRefresh(FuncWindow, TRUE);
                                break;

                        case IDCMP_CLOSEWINDOW:
                                donefunc = SEL_USE;
                                break;

                        case IDCMP_MOUSEMOVE: 
                        case IDCMP_GADGETDOWN:
                        case IDCMP_GADGETUP:
                                gad    = (struct Gadget *)imsg->IAddress;
                                gadid  = gad->GadgetID;
                                newval = imsg->Code;

handle_func_gads:

                                switch (gadid) {

                                        case GID_MATCHNAME:
                                                strcpy(MatchName, STRVAL(GID_MATCHNAME));
                                                SetPattern(MatchName, IgnoreWBShell);
                                                break;

                                        case GID_SELSYSTEM:
                                        case GID_SELDOS:
                                                /*
                                                 *              Changing system column cycle gadget.
                                                 *              Update to show ALL, NONE or SELECTED
                                                 *              system gadgets.
                                                 */
                                        {
                                                int first    = FIRST_SYS_GADGET;
                                                int last     = LAST_SYS_GADGET;
                                                int numgads  = NUM_SYS_GADGETS;
                                                int *pcurval = &FuncSystemCol;

                                                if (gadid == GID_SELDOS) {
                                                        /*
                                                         *              Override above defaults with new
                                                         *              defaults for DOS cycle gadget
                                                         */
                                                        first    = FIRST_DOS_GADGET;
                                                        last     = LAST_DOS_GADGET;
                                                        numgads  = NUM_DOS_GADGETS;
                                                        pcurval  = &FuncDOSCol;
                                                }
                                                if (newval == *pcurval)
                                                        break;          /* Ignore no-op events */

                                                if (newval == COL_SELECTED) {
                                                        /*
                                                         *              Restore previously saved system col values
                                                         */
                                                        ShowFuncOpts(SavedCols, first, last);
                                                        memcpy(&CurSettings.Func.Opts[first],
                                                                   &SavedCols[first], numgads);
                                                } else {
                                                        /*
                                                         *              Select either ALL or NONE of the settings
                                                         *              in the system column.
                                                         */
                                                        int fillval = (newval == COL_ALL);

                                                        memset(&TempCols[first], fillval, numgads);
                                                        ShowFuncOpts(TempCols, first, last);
                                                                        
                                                        if (*pcurval == COL_SELECTED) {
                                                           /*
                                                            *           If we are moving out of the
                                                                *               selected state, then save the
                                                                *               existing settings for later.
                                                                */
                                                           memcpy(&SavedCols[first],
                                                                          &CurSettings.Func.Opts[first], numgads);
                                                        }
                                                        memset(&CurSettings.Func.Opts[first],
                                                                   fillval, numgads);
                                                }
                                                LoadFuncSettings(&CurSettings.Func);
                                                *pcurval = newval;
                                                break;
                                        }

                                        case GID_FUNCUSE:
                                                /*
                                                 *              Retain existing settings, and exit
                                                 */
                                                donefunc = SEL_USE;
                                                break;

                                        case GID_FUNCCANCEL:
                                                /*
                                                 *              Restore original settings and exit
                                                 */
                                                donefunc = SEL_CANCEL;
                                                break;

                                        case GID_FUNCUNDO:
                                                /*
                                                 *              Restore original settings but don't exit
                                                 */
                                                InstallSettings(&SavedFuncSets, SET_FUNC);
                                                break;

                                        default:
                                                if (gadid >= FIRST_BOOL_GADGET &&
                                                                                                gadid <= LAST_BOOL_GADGET) {
                                                        /*
                                                         *              Getting a new boolean gadget so update
                                                         *              CurSettings to reflect this.
                                                         */
                                                        CurSettings.Func.Opts[gadid] =
                                                                                (gad->Flags & GFLG_SELECTED) != 0;
                                                        LoadFuncSettings(&CurSettings.Func);
                                                        /*
                                                         *              Now check to see if we need to put
                                                         *              either column back into the selected
                                                         *              state again.
                                                         */
                                                        if (FuncSystemCol != COL_SELECTED &&
                                                                gadid >= FIRST_SYS_GADGET         &&
                                                            gadid <= LAST_SYS_GADGET)
                                                        {
                                                                FuncSystemCol = COL_SELECTED;
                                                                GT_SetGadgetAttrs(Gadget[GID_SELSYSTEM],
                                                                                                  FuncWindow, NULL,
                                                                                                  GTCY_Active, COL_SELECTED,
                                                                                                  TAG_DONE);
                                                        }
                                                        if (FuncDOSCol != COL_SELECTED  &&
                                                                gadid >= FIRST_DOS_GADGET       &&
                                                            gadid <= LAST_DOS_GADGET)
                                                        {
                                                                FuncDOSCol = COL_SELECTED;
                                                                GT_SetGadgetAttrs(Gadget[GID_SELDOS],
                                                                                                  FuncWindow, NULL,
                                                                                                  GTCY_Active, COL_SELECTED,
                                                                                                  TAG_DONE);
                                                        }
                                                }
                                                break;
                                }
                                break;

                        case IDCMP_INACTIVEWINDOW:
                                if (activegad) {
                                        ShowGadget(FuncWindow, Gadget[activegad], GADGET_UP);
                                        activegad = 0;
                                }
                                break;

                        case IDCMP_RAWKEY:
                        {
                                int upstroke = imsg->Code & 0x80;
                                int keypress = ConvertIMsgToChar(imsg);

                                gadid  = FuncKeyboard[keypress];
                                if (activegad) {
                                        /*
                                         *              We're releasing a gadget that was pressed
                                         *              earlier, so handle it now
                                         */
                                        int samekey = (imsg->Code & 0x7f) == activekey;

                                        if (samekey && !upstroke)       /* Ignore repeated keys */
                                                break;

                                        ShowGadget(FuncWindow, Gadget[activegad], GADGET_UP);
                                        if (samekey) {
                                                /*
                                                 *              Just released the key that was originally
                                                 *              pressed for this gadget, so now go and
                                                 *              handle the gadget action.
                                                 */
                                                gadid     = activegad;
                                                gad       = Gadget[gadid];
                                                activegad = 0;
                                                goto handle_func_gads;
                                        }
                                        /*
                                         *              If the above check didn't kick in, it means
                                         *              we got a downpress of a different key, so
                                         *              disable the gadget for this key and continue
                                         *              to press the other key.
                                         */
                                        activegad = 0;
                                }

                                if (imsg->Code == HELPKEY) {
                                        ShowAGuide(MSG(MSG_LINK_FUNCWIN));
                                        break;
                                }

                                /*
                                 *              Handle keyboard equivalents for gadgets
                                 */
                                gadid = FuncKeyboard[keypress];
                                if (gadid) {
                                        gad = Gadget[gadid];

                                        if (gadid >= FIRST_BOOL_GADGET &&
                                                                                gadid <= LAST_BOOL_GADGET) {
                                                /*
                                                 *              It's a boolean gadget so just invert its
                                                 *              current setting
                                                 */
                                                newval = !CurSettings.Func.Opts[gadid];
                                                GT_SetGadgetAttrs(Gadget[gadid], FuncWindow, NULL,
                                                                                  GTCB_Checked, newval, TAG_DONE);
                                                goto handle_func_gads;
                                        } else if (gadid == GID_SELSYSTEM || gadid == GID_SELDOS) {
                                                /*
                                                 *              One of the two cycle gadgets. Cycle forward
                                                 *              if the unshifted key was pressed or back
                                                 *              if the shifted version was pressed
                                                 */
                                                newval = FuncSystemCol;
                                                if (gadid == GID_SELDOS)
                                                        newval = FuncDOSCol;
                                                newval++;
                                                if (shift)
                                                        newval -= 2;
                                                if (newval < 0) newval = 2;
                                                if (newval > 2) newval = 0;
                                                GT_SetGadgetAttrs(gad, FuncWindow, NULL,
                                                                                  GTCY_Active, newval, TAG_DONE);
                                                goto handle_func_gads;
                                        } else if (gadid == GID_MATCHNAME) {
                                                /* 
                                                 *              The string gadget, so simply activate it
                                                 */
                                                ActivateGadget(gad, FuncWindow, NULL);
                                        } else {
                                                /*
                                                 *              It must be a button gadget so show it
                                                 *              selected first before processing it
                                                 */
                                                ShowGadget(FuncWindow, gad, GADGET_DOWN);
                                                activegad = gadid;
                                                activekey = imsg->Code;
                                        }
                                }
                                break;
                        }
                }
                GT_ReplyIMsg(imsg);
        }
        if (donefunc != SEL_NONE) {
                /*
                 *              Since we cache the function gadgets between window opens,
                 *              we need to ensure the two cycle gadgets are reset to
                 *              SELECTED mode ready for the next time we open the window.
                 *              (Ideally, we would remember the cycle state and use the
                 *              same state next time, but that's too much work!)
                 */
                ResetFuncToSelected();
        }
        if (donefunc == SEL_USE) {
                strcpy(MatchName, STRVAL(GID_MATCHNAME));
                SetPattern(MatchName, IgnoreWBShell);
                CloseFunctionWindow();
        } else if (donefunc == SEL_CANCEL) {
                CloseFunctionWindow();
                /*
                 *              Note that though we saved all the settings, we only
                 *              re-install the original function settings, not all
                 *              of them.
                 */
                InstallSettings(&SavedFuncSets, SET_FUNC);
        }
}

/*
 *              CreateFormatGadgets(gadgetfa, bufferfa, getsize, &width, &height)
 *
 *              Creates all the gadgets for the format window. If getsize
 *              is true, then the width and height values are filled in with
 *              the dimensions of the window needed to hold the gadgets. In
 *              this case, NULL is returned if the window would be too big to
 *              fit on the current screen, or non-NULL if width and height
 *              were successfully initialised.
 *
 *              gadgetfa gives the gadget font to use, bufferfa gives the buffer
 *              font (for the format listings).
 *
 *              If getsize is zero, then the actual gadgets are created and
 *              a pointer to the gadget list is returned, or NULL if an
 *              error occurred (typically, out of memory).
 */
struct Gadget *CreateFormatGadgets(struct TextAttr *gadgetfa,
                                                                   struct TextAttr *bufferfa,
                                                                   int getsize, int *pwidth, int *pheight)
{
        struct TextFont *gfont;
        struct TextFont *bfont;
        struct NewGadget ng;
        struct Gadget *gadlist;
        struct Gadget *gad;
        FieldInit *fi;
        int width;
        int height;
        int gwidth;
        int gheight;
        int gfonty;
        int bfontx;
        int bfonty;
        int topline;
        int maxtitlelen;
        int buttonwidth;
        int buttonspace;
        int headingspace;
        int outmargin;

        if (!FormVI) {
                FormVI = GetVisualInfoA(SnoopScreen, NULL);
                if (!FormVI)
                        return (NULL);
        }
        gfont = MyOpenFont(gadgetfa);
        if (!gfont)
                return (NULL);

        bfont = MyOpenFont(bufferfa);
        if (!bfont) {
                CloseFont(gfont);
                return (NULL);
        }
        if (bfont->tf_Flags & FPF_PROPORTIONAL) {
                CloseFont(bfont);
                CloseFont(gfont);
                return (NULL);
        }
        bfontx          = bfont->tf_XSize;
        bfonty          = bfont->tf_YSize;
        gfonty          = gfont->tf_YSize;
        gheight         = gfonty  + 3;
        gwidth          = gheight + 15;
        topline     = TitlebarHeight + gfonty/2;

        /*
         *              Now calculate the width of the window boxes which will contain
         *              the format sequences being displayed. The defaults box must be
         *              wide enough to hold "titlemsg..  %x" whereas the current format
         *              box must be wide enough to hold "titlemsg..  %20x".
         *
         *              Since the message titles may be localised, we calculate the
         *              longest string and work from that.
         */
        maxtitlelen = 0;
        for (fi = FieldTypes; fi->type != EF_END; fi++) {
                int len = strlen(MSG(fi->titlemsgid));

                if (len > maxtitlelen)
                        maxtitlelen = len;
        }
        /*
         *              The next two vars give the number of characters that will fit
         *              across each box. These is used when producing output.
         *
         *              For the left box, this is the length of the longest heading title,
         *              plus 2 for spacing after the heading name, another 2 for the
         *              format ID (%x) and a final 2 for the spacing on either side.
         *
         *              The right box is similar, but uses an additional 2 characters to
         *              display the field width of the selected item.
         */
        FBoxA_NumChars  = maxtitlelen   + 6;
        FBoxB_NumChars  = FBoxA_NumChars + 2;
        FBoxA_Width     = FBoxA_NumChars * bfontx + 4;
        FBoxB_Width     = FBoxB_NumChars * bfontx + 4;

        /*
         *              Create format strings for sprintf formatting later on. These
         *              look like this:
         *
         *                      FBoxA_FormString:       " %-??s  %%%lc "                (2 parameters)
         *                      FBoxB_FormString:       " %-??s  %%%02ld%lc "   (3 parameters)
         *
         *              and when strings are output using these templates, they look like:
         *
         *                      FBoxA_FormString:       " PaddedTitleName....  %x "
         *                      FBoxB_FormString:   " PaddedTitleName....  %12x "
         */
        mysprintf(FBoxA_FormString, " %%-%lds  %%%%%%lc ",               maxtitlelen);
        mysprintf(FBoxB_FormString, " %%-%lds  %%%%%%02ld%%lc ", maxtitlelen);

        /*
         *              Make sure we never have overlapping button gadgets (this might
         *              happen if we had a big gadget font and small buffer font).
         */
        buttonwidth = MaxTextLen(gfont, UseCancelUndoText) + 32;
        buttonspace = buttonwidth * 3 + FORM_MARGIN * 4 + BorderLeft + BorderRight;

        width = FBoxA_Width + FBoxB_Width + FORM_MARGIN * 4 +
                                                  BorderLeft  + BorderRight;

        outmargin = FORM_MARGIN;
        if (buttonspace > width)
                width = buttonspace;

        headingspace =  GetTextLen(gfont, MSG(MSG_AVAILABLE_GAD)) +
                                        GetTextLen(gfont, MSG(MSG_CURFORMAT_GAD)) +
                                        BorderLeft + BorderRight + FORM_MARGIN*3;

        if (headingspace > width)
                width = headingspace;

        /*
         *              Now calculate the dimensions of our two windows boxes. The left
         *              box displays the available format headings, and the right box
         *              displays the currently selected format headings.
         */
        FBoxSpacing             = bfonty + 2;
        FBoxA_Top               = topline + gfonty + 4;
        FBoxA_Height    = EF_COUNT * FBoxSpacing + 4;
        FBoxA_Left      = BorderLeft + outmargin;

        FBoxB_Top               = FBoxA_Top;
        FBoxB_Height    = FBoxA_Height;
        FBoxB_Left      = width - BorderRight - outmargin - FBoxB_Width;
         
        height = FBoxA_Top + FBoxA_Height + BorderBottom +
                         (gfonty + GadgetHeight) * 3;

        if (width > ScreenWidth || height > ScreenHeight) {
                CloseFont(gfont);
                CloseFont(bfont);
                return (NULL);
        }
        if (getsize) {
                CloseFont(gfont);
                CloseFont(bfont);
                *pwidth  = width;
                *pheight = height;
                return (struct Gadget *)(-1);
        }

        /*
         *              Now create our window gadgets. We have a total of 4 -- the
         *              Use, Undo, Cancel buttons, and the field width slider.
         */     
        gadlist = NULL;
        gad = CreateContext(&gadlist);
        if (!gad)
                goto fogad_failed;

        ng.ng_TextAttr          = gadgetfa;
        ng.ng_VisualInfo        = FormVI;
        ng.ng_Flags                     = PLACETEXT_IN;
        ng.ng_TopEdge           = height - BorderBottom - (gfonty+GadgetHeight) * 3/2;
        ng.ng_Height            = gfonty + GadgetHeight;
        ng.ng_Width                     = buttonwidth;

        ng.ng_LeftEdge          = FORM_MARGIN + BorderLeft;
        ng.ng_GadgetText        = MSG(MSG_USE_GAD);
        ng.ng_GadgetID          = GID_FORMUSE;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fogad_failed;
        Gadget[GID_FORMUSE] = gad;
        AddKeyShortcut(FormKeyboard, GID_FORMUSE, MSG_USE_GAD);
        
        ng.ng_LeftEdge          = width - BorderRight - FORM_MARGIN - ng.ng_Width;
        ng.ng_GadgetText        = MSG(MSG_CANCEL_GAD);
        ng.ng_GadgetID          = GID_FORMCANCEL;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fogad_failed;
        Gadget[GID_FORMCANCEL] = gad;
        AddKeyShortcut(FormKeyboard, GID_FORMCANCEL, MSG_CANCEL_GAD);
        
        ng.ng_LeftEdge          = (ng.ng_LeftEdge + BorderLeft + FORM_MARGIN) / 2;
        ng.ng_GadgetText        = MSG(MSG_UNDO_GAD);
        ng.ng_GadgetID          = GID_FORMUNDO;

        gad = CreateGadget(BUTTON_KIND,         gad, &ng,
                                           GT_Underscore,       '_',
                                           TAG_DONE);
        if (!gad)
                goto fogad_failed;
        Gadget[GID_FORMUNDO] = gad;
        AddKeyShortcut(FormKeyboard, GID_FORMUNDO, MSG_UNDO_GAD);

        /*
         *              Create the slider gadget. We need to leave room on the left for
         *              the title, and on the right for the current value.
         */
        ng.ng_LeftEdge          = FORM_MARGIN + BorderLeft + 8 +
                                                  GetTextLen(gfont, MSG(MSG_FIELDWIDTH_GAD));
        ng.ng_Width                     = width - FORM_MARGIN - ng.ng_LeftEdge -
                                                  BorderLeft - BorderRight - GetTextLen(gfont, "99 ");
        ng.ng_Height            = gfonty + 3;
        ng.ng_TopEdge           = (ng.ng_TopEdge + FBoxA_Top + FBoxA_Height -
                                                   ng.ng_Height) / 2;
        ng.ng_GadgetText        = MSG(MSG_FIELDWIDTH_GAD);
        ng.ng_GadgetID          = GID_FORMWIDTH;
        ng.ng_Flags                     = PLACETEXT_LEFT;

        gad = CreateGadget(SLIDER_KIND,                 gad, &ng,
                                           GTSL_LevelFormat,    "%2ld ",
                                           GTSL_LevelPlace,             PLACETEXT_RIGHT,
                                           GTSL_Level,                  1,
                                           GTSL_Min,                    1,
                                           GTSL_Max,                    MAX_FIELD_LEN,
                                           GTSL_MaxLevelLen,    2,
                                           GT_Underscore,               '_',
                                           GA_Disabled,                 TRUE,
                                           GA_RelVerify,                TRUE,
                                           TAG_DONE);
        if (!gad)
                goto fogad_failed;
        Gadget[GID_FORMWIDTH] = gad;
        AddKeyShortcut(FormKeyboard, GID_FORMWIDTH, MSG_FIELDWIDTH_GAD);

        FormGadFont = gfont;
        FormBufFont = bfont;
        return (gadlist);

fogad_failed:
        if (gadlist)
                FreeGadgets(gadlist);
        CloseFont(gfont);
        CloseFont(bfont);
        return (NULL);
}

/*
 *              ShowFormatLine(line, select)
 *
 *              Outputs the numbered line of text in the right-hand box. If the
 *              number given is too high, then a blank line is output instead.
 *              The text will be of the form "HeadingName.... %02x" according
 *              to the current contents of the line.
 *
 *              If select is FORMAT_SELECTED, the line will be highlighted, else
 *              it will be output normally.
 */
void ShowFormatLine(int line, int select)
{
        int bfontbaseline          = FormBufFont->tf_Baseline;
        int boxwidth               = FormBufFont->tf_XSize * FBoxB_NumChars;
        struct RastPort *rport = FormWindow->RPort;
        char viewbuf[60];
        int fillpen = 0;
        int textpen = 1;
        int textrow;
        
        if (select == FORMAT_SELECTED) {
                fillpen = ScreenDI->dri_Pens[FILLPEN];
                textpen = ScreenDI->dri_Pens[FILLTEXTPEN];
        }
        SetFont(rport, FormBufFont);
        SetAPen(rport, textpen);
        SetBPen(rport, fillpen);
        SetDrMd(rport, JAM2);

        if (line < 0 || line >= EF_COUNT)
                return;

        if (line < FBoxB_CurLines) {
                FieldInit *fi = &FieldTypes[CurrentFields[line].type];

                mysprintf(viewbuf, FBoxB_FormString, MSG(fi->titlemsgid),
                                                   CurrentFields[line].width, fi->idchar);
        } else {
                memset(viewbuf, ' ', FBoxB_NumChars);
        }
        textrow = FBoxB_Top + line*FBoxSpacing + 3;
        Move(rport, FBoxB_Left + 2, textrow + bfontbaseline);
        Text(rport, viewbuf, FBoxB_NumChars);

        /*
         *              Now draw highlight on top and bottom of selected line
         *              (or erase, as the case may be). This fills out the
         *              selection area and makes it look much better.
         */
        SetAPen(rport, fillpen);
        Move(rport, FBoxB_Left + 2, textrow - 1);
        Draw(rport, FBoxB_Left + 1 + boxwidth, textrow - 1);
        Move(rport, FBoxB_Left + 2, textrow + FormBufFont->tf_YSize);
        Draw(rport, FBoxB_Left + 1 + boxwidth, textrow + FormBufFont->tf_YSize);
}

/*
 *              ShowDragSelect(pos, select)
 *
 *              Highlights the specified region to indicate where the currently
 *              dragged field will end up if it is released here.
 *
 *              If pos is FBOX_NOSELECT, no drop area is active and no output is
 *              produced. If pos is FBOX_SELECTLEFT, then the left box is active
 *              and it is given an inverse highlight around the interior of the
 *              box giving a bezel effect.
 *
 *              If pos is in the range 0 ... EF_COUNT, then it corresponds to an
 *              insertion point between two lines in the right-most box. This is
 *              show by a two-pixel high highlight line drawn between those two
 *              lines.
 *
 *              If select is FORMAT_SELECTED, the feature is drawn; if select is
 *              FORMAT_UNSELECTED, the feature is instead erased. Note that after
 *              calling this function, it may be necessary to redisplay any
 *              current highlight bar in the right box (see ShowFormatLine) since
 *              there is a one-pixel overlap between the highlight bar and the
 *              rendering done here.
 */
void ShowDragSelect(int pos, int select)
{
        struct RastPort *rport = FormWindow->RPort;
        int black = 1;
        int white = 2; 

        if (pos == FBOX_NOSELECT)
                return;
        
        if (select == FORMAT_UNSELECTED) {
                black = 0;
                white = 0;
        }
        SetDrMd(rport, JAM2);
        if (pos == FBOX_SELECTLEFT) {
                /*
                 *              Draw bezel inside left box
                 */
                int boxleft  = FBoxA_Left + 2;
                int boxtop       = FBoxA_Top  + 1;
                int boxright = FBoxA_Left + FBoxA_Width  - 3;
                int boxbot   = FBoxA_Top  + FBoxA_Height - 2;

                SetAPen(rport, black);
                Move(rport, boxleft,    boxbot);
                Draw(rport, boxleft,    boxtop);
                Move(rport, boxleft+1,  boxbot-1);
                Draw(rport, boxleft+1,  boxtop);
                Draw(rport, boxright-1, boxtop);

                SetAPen(rport, white);
                Move(rport, boxright,   boxtop);
                Draw(rport, boxright,   boxbot);
                Move(rport, boxright-1, boxtop);
                Draw(rport, boxright-1, boxbot);
                Draw(rport, boxleft-1,  boxbot);
        } else if (pos >= 0 && pos <= EF_COUNT) {
                /*
                 *              Draw thin selection line in right box
                 */
                int line = FBoxB_Top + 1 + pos * FBoxSpacing;

                SetAPen(rport, white);
                Move(rport, FBoxB_Left+2,                               line);
                Draw(rport, FBoxB_Left+FBoxB_Width-3,   line);
                SetAPen(rport, black);
                Move(rport, FBoxB_Left+2,                               line+1);
                Draw(rport, FBoxB_Left+FBoxB_Width-3,   line+1);
        }
}

/*
 *              RedrawFormatWindow()
 *
 *              Redraws all the parts of the format window that are generated by
 *              hand: this includes the two bevelled boxes containing the available
 *              format fields, the headings for those boxes, and the current contens
 *              of those boxes, including highlighting where appropriate.
 */
void RedrawFormatWindow(void)
{
        char viewbuf[60];
        struct RastPort *rport = FormWindow->RPort;
        int gfonty                = FormGadFont->tf_YSize;
        int gfontbaseline = FormGadFont->tf_Baseline;
        int bfontbaseline = FormBufFont->tf_Baseline;
        int line;
        int headpos;
        EditEvent *ee;

        DrawBevelBox(rport, FBoxA_Left, FBoxA_Top, FBoxA_Width, FBoxA_Height,
                                                GT_VisualInfo, FormVI,
                                                TAG_DONE);

        DrawBevelBox(rport, FBoxB_Left, FBoxB_Top, FBoxB_Width, FBoxB_Height,
                                                GT_VisualInfo, FormVI,
                                                TAG_DONE);

        SetAPen(rport, 1);
        SetBPen(rport, 0);
        SetDrMd(rport, JAM2);
        SetFont(rport, FormGadFont);

        /*
         *              When outputting the box headings, we adjust the centering
         *              to ensure that headings wider than the boxes don't get
         *              pushed into the window borders.
         */
#define CENTRE(left,width,msgid)        \
                ((width - GetTextLen(FormGadFont, MSG(msgid)))/2 + left)

        headpos = CENTRE(FBoxA_Left, FBoxA_Width, MSG_AVAILABLE_GAD);
        if (headpos < (BorderLeft + 4))
                headpos = FBoxA_Left; /* was: BorderLeft + 4; */

        Move(rport, headpos, FBoxA_Top - gfonty + gfontbaseline - 4);
        Text(rport, MSG(MSG_AVAILABLE_GAD), strlen(MSG(MSG_AVAILABLE_GAD)));

        headpos = CENTRE(FBoxB_Left, FBoxB_Width, MSG_CURFORMAT_GAD);
        if (headpos < (FBoxB_Left - 4))
                headpos = FBoxB_Left + FBoxB_Width -
                                                           GetTextLen(FormGadFont, MSG(MSG_CURFORMAT_GAD));

        Move(rport, headpos, FBoxB_Top - gfonty + gfontbaseline - 4);
        Text(rport, MSG(MSG_CURFORMAT_GAD), strlen(MSG(MSG_CURFORMAT_GAD)));

        /*
         *              Now redraw the text inside the left box (available headings)
         */
        SetFont(rport, FormBufFont);
        ee = &AvailableFields[0];
        for (line = 0; line < (FBoxB_Height - 4); line += FBoxSpacing) {
                if (ee->type != END_EDITLIST) {
                        FieldInit *fi = &FieldTypes[ee->type];
                        mysprintf(viewbuf, FBoxA_FormString, MSG(fi->titlemsgid),
                                                                                                 fi->idchar);
                        ee++;
                } else {
                        memset(viewbuf, ' ', FBoxA_NumChars);
                }
                Move(rport, FBoxA_Left + 2, FBoxA_Top + line + bfontbaseline + 3);
                Text(rport, viewbuf, FBoxA_NumChars);
        }

        /*
         *              Redraw the text inside the right box (currently selected headings)
         */
        ee = &CurrentFields[0];
        for (line = 0; line < EF_COUNT; line++) {
                if (line == FBoxSelected)
                        ShowFormatLine(line, FORMAT_SELECTED);
                else
                        ShowFormatLine(line, FORMAT_UNSELECTED);
        }
}

/*
 *              ConvertListToFormat(list, format)
 *
 *              Converts the passed in EditList to a format list which can be
 *              used for updating the main SnoopDos display. Returns the
 *              total width (in characters) occupied by the format.
 */
int ConvertListToFormat(EditEvent *ee, EventFormat *evformat)
{
        int width = 0;

        while (ee->type != END_EDITLIST) {
                width               += ee->width + 1; /* Include space between cols */
                evformat->type           = FieldTypes[ee->type].type;
                evformat->width          = ee->width;
                evformat->titlemsgid = FieldTypes[ee->type].titlemsgid;
                evformat++;
                ee++;
        }
        evformat->type = EF_END;
        if (width)
                width--;                /* Don't count extra space at end */
        return (width);
}

/*
 *              CreateFormatLists(format)
 *
 *              Parses the passed-in format string and creates the two lists
 *              used by the format editor window to allow the user to re-arrange
 *              the format.
 *
 *              Should be called before RedrawFormatWindow() is called for the
 *              first time.
 *
 *              Also initialises some format globals related to the list boxes.
 */
void CreateFormatLists(EventFormat *evformat)
{
        EditEvent *ee;
        FieldInit *fi;
        EventFormat *ef;

        /*
         *              Now build the lists for the two boxes. The right box contains
         *              all items in the format string (and in the same order that they
         *              appear in the format string). The left box contains all the
         *              items that are NOT in the format string, in the presorted
         *              order that they appear in the FieldTypes[] initialiser array.
         *
         *              Note that the two edit lists contain indexes directly into
         *              the FieldTypes array, rather than specific event types.
         *              Thus, we store the offset into the array when we find a match
         *              rather than fi->type.
         */
        ee = &CurrentFields[0];
        for (ef = evformat; ef->type != EF_END; ef++) {
                for (fi = FieldTypes; fi->type != ef->type && fi->type != EF_END; fi++)
                        ;
                ee->type  = fi - FieldTypes;
                ee->width = ef->width;
                ee++;
        }
        ee->type = END_EDITLIST;
        FBoxB_CurLines = ee - &CurrentFields[0];

        ee = &AvailableFields[0];
        for (fi = FieldTypes; fi->type != EF_END; fi++) {
                for (ef = BufferEFormat; ef->type != fi->type && ef->type != EF_END;
                                                                ef++)
                        ;
                if (ef->type != fi->type) {     /* Not in current format so add it here */
                        ee->type = fi - &FieldTypes[0];
                        ee++;
                }
        }
        ee->type = END_EDITLIST;

        FBoxA_CurLines          = ee - &AvailableFields[0];
        FBoxSelected            = FBOX_NOSELECT;
        FBoxDragSelect          = FBOX_NOSELECT;
        FormatCurLMB            = 0;    /* Left mouse button not selected at start */
        FormatMovingField       = 0;
        /*
         *              Ensure slider gadget is disabled (it might have been left
         *              enabled from an earlier time, which can be confusing).
         */
        if (FormWindow) {
                GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH], FormWindow,  NULL,
                                                  GA_Disabled, TRUE,
                                                  TAG_DONE);
        }
}

/*
 *              OpenFormatWindow()
 *
 *              Opens the format editor window with all the format gadgets.
 *              Returns TRUE for success, FALSE for failure.
 *              The window will contain all necessary gadgets.
 *
 *              As with the Settings window, we try a variety of fonts until we
 *              find one that fits onto the screen.
 */
int OpenFormatWindow(void)
{
        static struct TextAttr formgfontattr =
                { "empty-storage-for-format1-fonts", 0, FS_NORMAL, FPB_DISKFONT};
        static struct TextAttr formbfontattr =
                { "empty-storage-for-format2-fonts", 0, FS_NORMAL, FPB_DISKFONT};

        static int width;               /* Maintained from call to call */
        static int height;

        int left = CurSettings.FormWinLeft;
        int top  = CurSettings.FormWinTop;
        struct TextAttr *gadgetfa;
        struct TextAttr *bufferfa;
        struct Window *win;
        int i;

        CheckSegTracker();
        if (FormWindow) {
                WindowToFront(FormWindow);
                ActivateWindow(FormWindow);
                return (TRUE);
        }
        if (!CheckForScreen())
                return (FALSE);

        strcpy(FormatSaveFormat, BufFormat);    /* Save for possible Undo */

        /*
         *              Now try all combinations of gadget and buffer font until we
         *              find a font combination that fits on our screen
         */
        for (i = 0; gadgetfa = MainWindowFontList[i].gadgetfa; i++) {
                bufferfa = MainWindowFontList[i].bufferfa;
                if (CreateFormatGadgets(gadgetfa, bufferfa, TRUE, &width, &height))
                        break;
        }
        if (!gadgetfa) {
                ShowError(MSG(MSG_ERROR_OPENFORMAT));
                return (FALSE);
        }
        /*
         *              Now copy our font attributes so that our gadgets won't strangely
         *              get changed behind the scenes if we update our fonts
         */
        strcpy(formgfontattr.ta_Name, gadgetfa->ta_Name);
        formgfontattr.ta_YSize = gadgetfa->ta_YSize;
        gadgetfa = &formgfontattr;

        strcpy(formbfontattr.ta_Name, bufferfa->ta_Name);
        formbfontattr.ta_YSize = bufferfa->ta_YSize;
        bufferfa = &formbfontattr;

        if (left == -1)  left = (ScreenWidth  - width)  / 2;
        if (top  == -1)  top  = (ScreenHeight - height) / 2;

        win = OpenWindowTags(NULL,
                                                 WA_Title,                      MSG(MSG_FORMAT_TITLE),
                                                 WA_IDCMP,                      IDCMP_CLOSEWINDOW        |
                                                                                        IDCMP_REFRESHWINDOW      |
                                                                                        IDCMP_NEWSIZE            |
                                                                                        IDCMP_RAWKEY             |
                                                                                        IDCMP_MOUSEBUTTONS       |
                                                                                        IDCMP_INACTIVEWINDOW |
                                                                                        BUTTONIDCMP                      |
                                                                                        SLIDERIDCMP,
                                                 WA_Left,                       left,
                                                 WA_Top,                        top,
                                                 WA_Width,                      width,
                                                 WA_Height,                     height,
                                                 WA_Flags,                      WFLG_DRAGBAR             |
                                                                                        WFLG_DEPTHGADGET         |
                                                                                        WFLG_ACTIVATE            |
                                                                                        WFLG_RMBTRAP             |
                                                                                        WFLG_NEWLOOKMENUS,
                                                 RefreshTag,            TRUE,
                                                 WA_NoCareRefresh,      NoCareRefreshBool,
                                                 WA_PubScreen,          SnoopScreen,
                                                 TAG_DONE);
        if (!win) {
                ShowError(MSG(MSG_ERROR_OPENFORMAT));
                return (FALSE);
        }

        if (!FormGadList) {
                FormGadList = CreateFormatGadgets(gadgetfa, bufferfa, 0, 0, 0);
                if (!FormGadList) {
                        CloseWindow(win);
                        ShowError(MSG(MSG_ERROR_OPENFORMAT));
                        return (FALSE);
                }
        }

        /*
         *              Now try and create our BOBs for the window
         */
        TextBob = CreateBob(win->RPort, FBoxB_Width + 8,
                                                                    FormBufFont->tf_YSize + 4, 1);
        if (!TextBob) {
                ShowError(MSG(MSG_ERROR_OPENFORMAT));
                FreeGadgets(FormGadList);
                FormGadList     = NULL;
                CloseWindow(win);
                return (FALSE);
        }
        AddBob(&TextBob->bob, win->RPort);

        /*
         *              Now add gadgets to screen window, and then setup the current
         *              format string for editing.
         */
        AddGList(win, FormGadList, -1, -1, NULL);
        RefreshGList(FormGadList, win, NULL, -1);
        GT_RefreshWindow(win, NULL);

        /*
         *              All done, so initialise some globals and return
         */
        FormWindow              = win;
        FormWindowPort  = win->UserPort;
        FormWindowMask  = 1 << FormWindowPort->mp_SigBit;

        CreateFormatLists(BufferEFormat);
        /*
         *              Finally, draw our window to reflect this
         */
        RedrawFormatWindow();
        if (DisableNestCount)
                DisableWindow(FormWindow, &FormRequester);
        return (TRUE);
}

/*
 *              CloseFormatWindow(void)
 *
 *              Close the format window and free gadgets associated with the window.
 */
void CloseFormatWindow(void)
{
        if (FormWindow) {
                if (TextBob) {
                        FreeBob(TextBob);
                        TextBob = NULL;
                }
                RecordWindowSizes();
                CloseWindow(FormWindow);
                CloseFont(FormGadFont);
                CloseFont(FormBufFont);
                FormWindow     = NULL;
                FormWindowMask = 0;
                FormGadFont        = NULL;
                FormBufFont        = NULL;
        }
        if (FormGadList) {
                FreeGadgets(FormGadList);
                FormGadList = NULL;
        }
        if (FormVI) {
                FreeVisualInfo(FormVI);
                FormVI = NULL;
        }
}
 
/*
 *              FormatHitDetect(x, y)
 *
 *              Does a bounds check for the specified X/Y co-ordinates against the
 *              format editor window. A return value in the range 0..EF_COUNT-1
 *              indicates that line number in the right hand window was hit.
 *
 *              A return of FBOX_SELECTLEFT indicates that the left box was selected
 *              (not sure which line; doesn't really matter).
 *
 *              A return of FBOX_NOSELECT indicates neither box was selected.
 */
int FormatHitDetect(int x, int y)
{
        if (x >= FBoxA_Left && x <= (FBoxA_Left + FBoxA_Width  - 1) &&
                y >= FBoxA_Top  && y <= (FBoxA_Top  + FBoxA_Height - 1))
        {
                return (FBOX_SELECTLEFT);
        }
        
        if (x >= FBoxB_Left && x <= (FBoxB_Left + FBoxB_Width  - 1) &&
                y >= FBoxB_Top  && y <= (FBoxB_Top  + FBoxB_Height - 1))
        {
                if (y <= FBoxB_Top + FBoxSpacing + 2)
                        return (0);
                else
                        return (y - FBoxB_Top - 2) / FBoxSpacing;
        }
        return (FBOX_NOSELECT);
}

/*
 *              InstallNewFormat(type)
 *
 *              Installs the currently defined format into the main window for
 *              the user to see immediately.
 *
 *              If type is NEW_LISTVIEW, then the new format is assumed to be
 *              from the format editor window's CurrentFields list. The
 *              setup window, if open, will have its own format string updated
 *              accordingly.
 *
 *              If type is NEW_STRING, then the new format is assumed to be
 *              stored in the BufFormat string. The format window, if open,
 *              will have its own format listview updated accordingly.
 *
 *              If the buffer string is completely invalid (i.e. evaluates to
 *              an empty format) then it will be replaced with an empty string.
 */
void InstallNewFormat(int type)
{
        if (type == NEW_LISTVIEW) {
                BufferWidth = ConvertListToFormat(CurrentFields, BufferEFormat);
        } else /* type == NEW_STRING */ {
                BufferWidth = ParseFormatString(BufFormat, BufferEFormat,
                                                                                                   MAX_FORM_LEN);
                if (BufferWidth == 0) {
                        BufferWidth = ParseFormatString(DefaultSettings.Setup.BufferFormat,
                                                                                        BufferEFormat, MAX_FORM_LEN);
                }
                if (FormWindow) {
                        CreateFormatLists(BufferEFormat);
                        RedrawFormatWindow();
                }
        }
        BuildFormatString(BufferEFormat, BufFormat, MAX_FORM_LEN);
        if (SetWindow) {
                GT_SetGadgetAttrs(Gadget[GID_BUFFORMAT], SetWindow, NULL,
                                                  GTST_String,  BufFormat,
                                                  TAG_DONE);
        }
        ClearMainRHS = 1;
        InitMainMargins();
        UpdateMainHScroll();
        RedrawMainWindow();
}

/*
 *              HandleFormatMsgs()
 *
 *              Processes all outstanding messages associated with the Format
 *              editor gadget window. To be called whenever we get a format event.
 */
void HandleFormatMsgs(void)
{
        static int activegad;                   /* Non-zero if gadget is depressed   */
        static int activekey;                   /* Keycode shortcut of activegad     */

        struct IntuiMessage *imsg;
        int doneform = SEL_NONE;
        int hit;
        int lmb;
        int rmb;

        if (!FormWindow)
                return;

        while ((imsg = GT_GetIMsg(FormWindowPort)) != NULL) {
                struct Gadget *gad;
                int dxwidth = 0;                                /* Amount to adjust width by */
                int newrow  = FBoxSelected;             /* Currently selected row        */
                int gadid;

                switch (imsg->Class) {
                        
                        case IDCMP_CLOSEWINDOW:
                                doneform = SEL_USE;
                                break;

                        case IDCMP_REFRESHWINDOW:
                                GT_BeginRefresh(FormWindow);
                                RedrawFormatWindow();
                                GT_EndRefresh(FormWindow, TRUE);
                                break;

                        case IDCMP_MOUSEBUTTONS:
                                lmb = imsg->Qualifier & IEQUALIFIER_LEFTBUTTON;
                                rmb = imsg->Qualifier & IEQUALIFIER_RBUTTON;
                                /*
                                 *              If we're already dragging something, then the
                                 *              right mouse button allows it to be cancelled.
                                 */
                                if (FormatMovingField && rmb)
                                        goto cancel_moving_field;

                                if (lmb == FormatCurLMB)        /* Ignore spurious events */
                                        break;

                                hit = FormatHitDetect(imsg->MouseX, imsg->MouseY);
                                if (lmb) {
                                        /*
                                         *              User is trying to select or pick-up a new
                                         *              event.
                                         */
                                        if (hit >= 0 && hit < FBoxB_CurLines &&
                                                                                                hit != FBoxSelected) {
                                                if (FBoxSelected >= 0 && FBoxSelected < FBoxB_CurLines)
                                                        ShowFormatLine(FBoxSelected, FORMAT_UNSELECTED);
                                                FBoxSelected = hit;
                                                ShowFormatLine(hit, FORMAT_SELECTED);
                                                GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH],
                                                                                  FormWindow,  NULL,
                                                                                  GA_Disabled, FALSE,
                                                                                  GTSL_Level, CurrentFields[hit].width,
                                                                                  TAG_DONE);
                                        }
                                        if (hit != FBOX_NOSELECT) {
                                                /*
                                                 *              Pick-up the icon under mouse
                                                 */
                                                if (PickupBob(hit, imsg->MouseX, imsg->MouseY)) {
                                                        FormatMovingField = 1;
                                                        ReportMouse(1, FormWindow);     /* Enable mouse moves */
                                                }
                                        }
                                } else {
                                        /*
                                         *              We might be finished doing a drag of a field
                                         *              from one position to another -- if so, drop the
                                         *              field in it's new place.
                                         */
                                        if (FormatMovingField) {
                                                int curdragselect = FBoxDragSelect;

                                                FormatMovingField = 0;
                                                ReportMouse(0, FormWindow);     /* Disable mouse moves  */
                                                DropBob();
                                                if (curdragselect != FBOX_NOSELECT)
                                                        InstallNewFormat(NEW_LISTVIEW);
                                        }
                                }
                                FormatCurLMB = lmb;
                                break;
                                                
                        case IDCMP_MOUSEMOVE: 
                                if (FormatMovingField) {
                                        UpdateBob(imsg->MouseX, imsg->MouseY);
                                        break;
                                }

                                /*
                                 *              Fall through to handle MOUSEMOVE for slider gadget
                                 */

                        case IDCMP_GADGETDOWN:
                        case IDCMP_GADGETUP:
                                gad   = (struct Gadget *)imsg->IAddress;
                                gadid = gad->GadgetID;

handle_form_gads:
                                switch (gadid) {

                                        case GID_FORMUSE:
                                                doneform = SEL_USE;
                                                break;

                                        case GID_FORMCANCEL:
                                                doneform = SEL_CANCEL;
                                                break;

                                        case GID_FORMUNDO:
                                                strcpy(BufFormat, FormatSaveFormat);
                                                InstallNewFormat(NEW_STRING);
                                                break;

                                        case GID_FORMWIDTH:
                                                /*
                                                 *              The slider has been moved, so update the
                                                 *              currently selected item accordingly.
                                                 */
                                                if (FBoxSelected >= 0 &&
                                                                                        FBoxSelected < FBoxB_CurLines) {
                                                        CurrentFields[FBoxSelected].width = imsg->Code;
                                                        ShowFormatLine(FBoxSelected, FORMAT_SELECTED);
                                                        if (imsg->Class == IDCMP_GADGETUP)
                                                                InstallNewFormat(NEW_LISTVIEW);
                                                }
                                                break;
                                }
                                break;

                        case IDCMP_INACTIVEWINDOW:
                                /*
                                 *              Cancel any currently pressed button
                                 */
                                if (activegad) {
                                        ShowGadget(FormWindow, Gadget[activegad], GADGET_UP);
                                        activegad = 0;
                                }
                                /*
                                 *              If we're dragging something and the window
                                 *              goes inactive, drop it. Otherwise, when the
                                 *              window is reactivated, it will still be attached
                                 *              to the mouse pointer!
                                 */
                                if (FormatMovingField) {
                                        FormatCurLMB      = 0;
cancel_moving_field:
                                        ReportMouse(0, FormWindow);
                                        UpdateBob(0, -9999);            /* Make position invalid */
                                        DropBob();
                                        FormatMovingField = 0;
                                        if (imsg->Class == IDCMP_INACTIVEWINDOW) {
                                                /*
                                                 *              If we went inactive, then the act of
                                                 *              erasing the BOB has probably resulted
                                                 *              in a portion of the window frame being
                                                 *              restored in its active colours, even
                                                 *              though the window is now inactive -- so
                                                 *              we refresh it to get around this.
                                                 */
                                                RefreshWindowFrame(FormWindow);
                                        }
                                }
                                break;
                                        
                        case IDCMP_RAWKEY:
                        {
                                int shift    = imsg->Qualifier & IE_SHIFT;
                                int altctrl  = imsg->Qualifier & (IE_ALT | IE_CTRL);
                                int upstroke = imsg->Code & 0x80;
                                int keypress = ConvertIMsgToChar(imsg);

                                /*
                                 *              Handle keyboard shortcuts
                                 */
                                if (FormatMovingField && keypress == '\033') {
                                        /*
                                         *              ESC cancels any drag currently in operation
                                         */
                                        goto cancel_moving_field;
                                }

                                gadid  = FormKeyboard[keypress];
                                if (activegad) {
                                        /*
                                         *              We're releasing a gadget that was pressed
                                         *              earlier, so handle it now
                                         */
                                        int samekey = (imsg->Code & 0x7f) == activekey;

                                        if (samekey && !upstroke)       /* Ignore repeated keys */
                                                break;

                                        ShowGadget(FormWindow, Gadget[activegad], GADGET_UP);
                                        if (samekey) {
                                                /*
                                                 *              Just released the key that was originally
                                                 *              pressed for this gadget, so now go and
                                                 *              handle the gadget action.
                                                 */
                                                gadid     = activegad;
                                                gad       = Gadget[gadid];
                                                activegad = 0;
                                                goto handle_form_gads;
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
                                 *              Check for normal gadget keyboard shortcuts
                                 */
                                if (gadid) {
                                        gad = Gadget[gadid];
                                        if (gadid == GID_FORMWIDTH) {
                                                /*
                                                 *              Got a slider adjustment request
                                                 */
                                                if (imsg->Qualifier & IE_SHIFT)
                                                        dxwidth = -1;
                                                else
                                                        dxwidth = 1;

                                                if (newrow == FBOX_NOSELECT)
                                                        newrow = 0;     /* If no row active, pick first row */

                                                if (FBoxB_CurLines == 0)  /* Ignore if box is empty */
                                                        break;

                                                goto format_move_slider;
                                        }
                                        /*
                                         *              Must be a simple button press so highlight
                                         *              the button and handle accordingly
                                         */
                                        ShowGadget(FormWindow, gad, GADGET_DOWN);
                                        activegad = gadid;
                                        activekey = imsg->Code;
                                        break;
                                }

                                /*
                                 *              Handle cursor movement. This lets us choose a
                                 *              new item in the format box, or adjust the width
                                 *              of an existing item.
                                 */
                                if (!FBoxB_CurLines)    /* A format must exist */
                                        break;
                                
                                if (newrow == FBOX_NOSELECT)
                                        newrow = -1;    /* Ensure cursor down selects item 0 */

                                switch (imsg->Code) {
                                        case CURSORUP:
                                                if (newrow > 0 && !(shift | altctrl))
                                                        newrow--;
                                                else
                                                        newrow = 0;
                                                break;

                                        case CURSORDOWN:
                                                if (shift | altctrl)
                                                        newrow = FBoxB_CurLines - 1;
                                                else if (newrow < (FBoxB_CurLines-1))
                                                        newrow++;
                                                break;

                                        case CURSORLEFT:
                                                dxwidth = -1;
                                                if (shift)   dxwidth  = -5;
                                                if (altctrl) dxwidth  = -MAX_FIELD_LEN;
                                                break;

                                        case CURSORRIGHT:
                                                dxwidth = 1;
                                                if (shift)   dxwidth = 5;
                                                if (altctrl) dxwidth = MAX_FIELD_LEN;
                                                break;

                                        case HELPKEY:
                                                ShowAGuide(MSG(MSG_LINK_FORMATWIN));
                                                goto done_keys;

                                        default:
                                                goto done_keys;         /* Ignore all other keys */
                                }

format_move_slider:
                                /*
                                 *              First check if the row has changed. (As well as
                                 *              changing via cursor up/down, this can also happen
                                 *              with left/right if there was no item selected to
                                 *              begin with
                                 */
                                if (newrow < 0)
                                        newrow = 0;

                                if (newrow != FBoxSelected && FBoxB_CurLines > 0) {
                                        /*
                                         *              Unactivate old row, activate a new row
                                         */
                                        if (FBoxSelected >= 0 && FBoxSelected < FBoxB_CurLines)
                                                ShowFormatLine(FBoxSelected, FORMAT_UNSELECTED);
                                        FBoxSelected = newrow;
                                        ShowFormatLine(newrow, FORMAT_SELECTED);
                                        GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH],
                                                                          FormWindow,  NULL,
                                                                          GA_Disabled, FALSE,
                                                                          GTSL_Level, CurrentFields[newrow].width,
                                                                          TAG_DONE);
                                }

                                /*
                                 *              Now check if the horizontal position of the
                                 *              slider has changed.
                                 */
                                if (dxwidth) {
                                        int newwidth = CurrentFields[FBoxSelected].width + dxwidth;

                                        if (newwidth < 1)                               newwidth = 1;
                                        if (newwidth > MAX_FIELD_LEN)   newwidth = MAX_FIELD_LEN;
                                        if (FBoxSelected != FBOX_NOSELECT) {
                                                if (newwidth != CurrentFields[FBoxSelected].width) {
                                                        GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH],
                                                                                          FormWindow,  NULL,
                                                                                          GTSL_Level,  newwidth,
                                                                                          TAG_DONE);
                                                        CurrentFields[FBoxSelected].width = newwidth;
                                                        ShowFormatLine(FBoxSelected, FORMAT_SELECTED);
                                                        InstallNewFormat(NEW_LISTVIEW);
                                                }
                                        }
                                }
done_keys:
                                break;
                        }
                }
                GT_ReplyIMsg(imsg);
        }
        if (doneform != SEL_NONE) {
                CloseFormatWindow();
                if (doneform == SEL_CANCEL) {
                        strcpy(BufFormat, FormatSaveFormat);
                        InstallNewFormat(NEW_STRING);
                }
        }
}

/*
 *              -----------------------------------------------------------------------
 *              Now our BOB/GEL routines for the drag-and-drop format editor.
 *              These should really be in their own module, but they're being
 *              put here for now for convenience.
 *              -----------------------------------------------------------------------
 */

/*
 *              InitGel(rastport, width, height, transp)
 *
 *              This function does all the initialisation necessary to allow us
 *              to use a single BOB in the given rastport. A new bitmap is allocated
 *              of dimensions width x height into which data can be rendered.
 *
 *              If successful, this function returns a pointer to a MyGel structure
 *              which contains, among other things, the rastport and bob structure
 *              you will use when calling system routines to manipulate the bob.
 *
 *              Note that the bob must be removed from the Gel list before rendering
 *              into it.
 *
 *              If transp is true, then the BOB will be transparent on colour zero.
 *              In this case, you must call InitMasks(mygel->vsprite) whenever
 *              you make a change to the image, to recalculate the mask.
 */
MyGel *CreateBob(struct RastPort *rport, int width, int height, int transp)
{
        struct GelsInfo *gInfo;
        struct Bob              *bob;
        struct VSprite  *vsprite;
        MyGel                   *mygel;
        ULONG                   linesize;
        ULONG                   planesize;
        ULONG                   imagesize;
        ULONG                   chipsize;
        UBYTE                   *chipdata;
        int                             wordwidth;
        int                             depth;
        int                             i;

        wordwidth       = (width + 15) >> 4;
        depth           = rport->BitMap->Depth;

        mygel = (MyGel *)AllocMem(sizeof(MyGel), MEMF_CLEAR);
        if (!mygel)
                return (NULL);
                
        mygel->mainrast         = rport;
        gInfo                           = &mygel->gelinfo;
        gInfo->nextLine         = mygel->nextline;
        gInfo->lastColor        = mygel->lastcolor;
        gInfo->collHandler      = &mygel->colltable;
        gInfo->sprRsrvd         = 0x03;

        /*
         *              Set left-most and top-most to 1 to better keep items
         *              inside the display boundaries.
         */
        gInfo->leftmost   = 1;
        gInfo->topmost    = 1;
        gInfo->rightmost  = (rport->BitMap->BytesPerRow << 3) - 1;
        gInfo->bottommost = rport->BitMap->Rows - 1;
        rport->GelsInfo   = gInfo;
        InitGels(&mygel->vshead, &mygel->vstail, gInfo);

        /*
         *              Now allocate a new BOB to be linked into this structure.
         *              We also allocate a bitmap associated with the bob, and
         *              give it its own RastPort so that we can render into it.
         */
        linesize        = sizeof(WORD)  * wordwidth;
        planesize       = linesize      * height;
        imagesize       = planesize             * depth;
        chipsize        = imagesize*2 + linesize + planesize;
        bob                     = &mygel->bob;
        vsprite         = &mygel->bobvsprite;

        if (!(chipdata = AllocMem(chipsize, MEMF_CHIP | MEMF_CLEAR))) {
                rport->GelsInfo = NULL;
                FreeMem(mygel, sizeof(MyGel));
                return (NULL);
        }
        mygel->chipdata         = (void *)(chipdata);
        mygel->chipsize         = chipsize;

        mygel->planedata        = (void *)(chipdata);
        bob->SaveBuffer         = (void *)(chipdata + imagesize);
        vsprite->BorderLine = (void *)(chipdata + imagesize*2);
        vsprite->CollMask       = (void *)(chipdata + imagesize*2 + linesize);

        vsprite->Y                      = -9999;
        vsprite->X                      = -9999;
        vsprite->Flags      = SAVEBACK | (transp ? OVERLAY : 0);
        vsprite->Width      = wordwidth;
        vsprite->Depth      = depth;
        vsprite->Height     = height;
        vsprite->MeMask     = 0;
        vsprite->HitMask    = 0;
        vsprite->ImageData  = (void *)mygel->planedata;
        vsprite->SprColors  = NULL;
        vsprite->PlanePick  = -1;
        vsprite->PlaneOnOff = 0x00;
        InitMasks(vsprite);

        vsprite->VSBob      = bob;
        bob->BobVSprite     = vsprite;
        bob->ImageShadow    = vsprite->CollMask;
        bob->Flags          = 0;
        bob->Before         = NULL;
        bob->After          = NULL;
        bob->BobComp        = NULL;
        bob->DBuffer            = NULL;

        /*
         *              Now setup the Rastport and Bitmap so we can render into the BOB
         */
        InitRastPort(&mygel->rastport);
        InitBitMap(&mygel->bitmap, depth, width, height);
        mygel->rastport.BitMap = &mygel->bitmap;

        for (i = 0; i < depth; i++)
                mygel->bitmap.Planes[i] = mygel->planedata + i * planesize;

        return (mygel);
}

/*
 *              FreeBob(mygel)
 *
 *              Frees the bob allocated earlier, along with all other associated info 
 *              Also removes all GELs from the system rastport.
 */
void FreeBob(MyGel *mygel)
{
        mygel->mainrast->GelsInfo = NULL;
        FreeMem(mygel->chipdata, mygel->chipsize);
        FreeMem(mygel, sizeof(MyGel));
}

/*
 *              PickupBob(hit, x, y)
 *
 *              Picks up the line and copies it into our bob's bitmap, then adds
 *              the bob to the screen at the current mouse location so the
 *              user can drag it around.
 *
 *              If the hit is correct, we can ignore Y since the field being
 *              dragged is in the right box and we already know which line.
 *              Otherwise, Y is a pixel co-ordinate that we use to calculate
 *              the line of the box that's being chosen.
 *
 *              If hit is FBOX_SELECTLEFT, then we need to calculate which line
 *              we are dealing with.
 *
 *              We also record the start line we use.
 *
 *              Returns true for success, false if we didn't have a hit after all.
 */
int PickupBob(int hit, int x, int y)
{
        struct RastPort *rport    = FormWindow->RPort;
        struct RastPort *bobrport = &TextBob->rastport;
        char viewbuf[50];
        int line;
        int box;
        int xpos;
        int ypos;
        int len;

        if (hit == FBOX_NOSELECT) {
                return (FALSE);
        } else if (hit == FBOX_SELECTLEFT) {
                box  = FORMAT_LEFTBOX;
                line = (y - FBoxA_Top - 2);
                if (line < FBoxSpacing)
                        line = 0;
                else
                        line /= FBoxSpacing;
                if (line >= FBoxA_CurLines)
                        return (FALSE);
        } else {
                /* Right box */
                box  = FORMAT_RIGHTBOX;
                line = hit;
                if (line >= FBoxB_CurLines)
                        return (FALSE);
        }

        /*
         *              Okay, got a definite hit so go ahead and start the drag
         */
        FormatDragBox   = box;
        FormatDragLine  = line;
        
        SetRast(bobrport, 0);                           /* Erase rast bitmap    */
        SetAPen(bobrport, 2);                           /* White foreground             */
        SetBPen(bobrport, 1);                           /* Black background             */
        SetDrMd(bobrport, JAM2);                        /* Solid text                   */
        SetFont(bobrport, FormBufFont);

        if (box == FORMAT_LEFTBOX) {
                FieldInit *fi = &FieldTypes[AvailableFields[line].type];

                mysprintf(viewbuf, FBoxA_FormString, MSG(fi->titlemsgid), fi->idchar);
                len  = FBoxA_NumChars;
                xpos = FBoxA_Left + 2;
                ypos = FBoxA_Top  + 2 + line * FBoxSpacing;
        } else {
                FieldInit *fi = &FieldTypes[CurrentFields[line].type];

                mysprintf(viewbuf, FBoxB_FormString, MSG(fi->titlemsgid),
                                                   CurrentFields[line].width, fi->idchar);
                len  = FBoxB_NumChars;
                xpos = FBoxB_Left + 2;
                ypos = FBoxB_Top  + 2 + line * FBoxSpacing;
        }
        TextBob->bob.BobVSprite->X = xpos;
        TextBob->bob.BobVSprite->Y = ypos;
        FBoxDeltaX = xpos - x;
        FBoxDeltaY = ypos - y;

        Move(bobrport, 0, FormBufFont->tf_Baseline+1);
        Text(bobrport, viewbuf, len);

        /*
         *              Now draw highlight above and below the text
         */
        SetAPen(bobrport, 1);
        Move(bobrport, 0, 0);
        Draw(bobrport, len * FormBufFont->tf_XSize - 1, 0);
        Move(bobrport, 0, FormBufFont->tf_YSize + 1);
        Draw(bobrport, len * FormBufFont->tf_XSize - 1, FormBufFont->tf_YSize + 1);

        InitMasks(TextBob->bob.BobVSprite);
        SortGList(rport);
        DrawGList(rport, ViewPortAddress(FormWindow));
        return (1);
}

/*
 *              DropBob()
 *
 *              Releases the current bob at the target, and updates the two
 *              lists/displays accordingly.
 */
void DropBob(void)
{
        EditEvent savedev;
        EditEvent *ee;
        int destpos;
        int seltype;

        destpos = FBoxDragSelect;                       /* Save destination drop selection      */
        UpdateBob(0, -9999);                            /* Kill any highlighted region          */

        /*
         *              Now re-arrange the edit lists to reflect the new position of
         *              the dragged field. There are three cases to consider:
         *
         *                      Left box        -> Right box
         *                      Right box       -> Right box
         *                      Right box       -> Left box
         */
        if (destpos == FBOX_NOSELECT)
                return;
        
        ShowFormatLine(FBoxSelected, FORMAT_UNSELECTED);
        if (FBoxSelected >= 0 && FBoxSelected < EF_COUNT)
                seltype = CurrentFields[FBoxSelected].type;
        else
                seltype = -1;

        /*
         *              First, delete item from its current list
         */
        if (FormatDragBox == FORMAT_LEFTBOX) {
                /*
                 *              Deleting from left box
                 */
                ee              = &AvailableFields[FormatDragLine];
                savedev.type  = ee->type;
                savedev.width = FieldTypes[ee->type].defwidth;

                while (ee->type != END_EDITLIST) {
                        ee[0] = ee[1];
                        ee++;
                }
                FBoxA_CurLines--;
                seltype = savedev.type; /* New selection is item we just dragged */
        } else {
                /*
                 *              Deleting from right box. If the drop position is after
                 *              the point where we're deleting, we adjust it to reflect
                 *              the deleted line.
                 */

                if (destpos > FormatDragLine && destpos <= EF_COUNT)
                        destpos--;

                ee              = &CurrentFields[FormatDragLine];
                savedev = *ee;
                while (ee->type != END_EDITLIST) {
                        ee[0] = ee[1];
                        ee++;
                }
                FBoxB_CurLines--;
        }
        /*
         *              Now add the item (savedev) to the new list in the appropriate
         *              place
         */
        if (destpos == FBOX_SELECTLEFT) {
                /*
                 *              Search the existing left box list looking for the best place
                 *              to insert the item -- we do this in strict alphabetical
                 *              order (based on the % identifer)
                 */
                int newid = FieldTypes[savedev.type].idchar;
                EditEvent tempev;

                for (ee = &AvailableFields[0]; ee->type != END_EDITLIST; ee++)
                        if (FieldTypes[ee->type].idchar > newid)
                                break;

                do {
                        tempev  = *ee;
                        *ee     = savedev;
                        savedev = tempev;
                        ee++;
                } while (ee[-1].type != END_EDITLIST);
                FBoxA_CurLines++;
        } else {
                /* 
                 *              Adding line to the right box. Simply insert it at the
                 *              specified location.
                 */
                EditEvent tempev;

                ee = &CurrentFields[destpos];
                do {
                        tempev  = *ee;
                        *ee     = savedev;
                        savedev = tempev;
                        ee++;
                } while (ee[-1].type != END_EDITLIST);
                FBoxB_CurLines++;
        }
        /*
         *              Now search our right box list to see can we match the item
         *              that was currently highlighted when we entered this routine.
         */
        FBoxSelected = 0;
        for (ee = &CurrentFields[0]; ee->type != END_EDITLIST; ee++) {
                if (ee->type == seltype)
                        break;
                FBoxSelected++;
        }
        if (ee->type == END_EDITLIST) {
                FBoxSelected = FBOX_NOSELECT;
                GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH], FormWindow,  NULL,
                                                  GA_Disabled, TRUE,
                                                  TAG_DONE);
        } else {
                GT_SetGadgetAttrs(Gadget[GID_FORMWIDTH], FormWindow,  NULL,
                                                  GA_Disabled, FALSE,
                                                  GTSL_Level,  ee->width,
                                                  TAG_DONE);
        }

        /*
         *              Now, finally, update our window to show the new arrangement.
         */
        RedrawFormatWindow();
}

/*
 *              UpdateBob(x, y)
 *
 *              Updates the current position of the bob on the screen, and
 *              draws any highlights etc. that are deemed necessary.
 */
void UpdateBob(int x, int y)
{
        int bfonty = FormBufFont->tf_YSize;
        int pos;

        /*
         *              Now see if we are overlapping any of the drop regions, and if
         *              so, highlight the region appropriately.
         *
         *              When dragging into the left box, we highlight the entire box
         *              since the order of items is pre-determined and it doesn't
         *              make any sense to drop a field into one particular place.
         *
         *              When dragging into the right box, we highlight a line in
         *              between two other lines, which shows where the item will
         *              be inserted.
         *
         *              For neatness, we don't show highlighting which would have
         *              no effect on the current settings (i.e. moving a field to
         *              the same position it's already in).
         */
        pos = FBOX_NOSELECT;
        if (x >= FBoxA_Left && x <= (FBoxA_Left + FBoxA_Width  - 1) &&
            y >= FBoxA_Top  && y <= (FBoxA_Top  + FBoxA_Height - 1) &&
                FormatDragBox != FORMAT_LEFTBOX) {
                /*
                 *              Got a hit in left-hand box
                 */
                pos = FBOX_SELECTLEFT;
        } else if (x >= FBoxB_Left && x <= (FBoxB_Left + FBoxB_Width - 1) &&
                           y >= (FBoxB_Top - 10) && y <= (FBoxB_Top + FBoxB_Height + 10)) {
                /*
                 *              Got a hit in the right-hand box, so now calculate which line
                 */
                pos = (y - FBoxB_Top - 2 + bfonty/2) / FBoxSpacing;

                if (pos < 0)                            pos = 0;
                if (pos > FBoxB_CurLines)       pos = FBoxB_CurLines;
                if (FormatDragBox == FORMAT_RIGHTBOX &&
                                                (pos == FormatDragLine || pos == (FormatDragLine+1))) {
                        pos = FBOX_NOSELECT;    /* Don't highlight line we dragged from */
                }
        }

        if (pos != FBoxDragSelect) {
                /*
                 *              Currently selected item has changed, so erase current one,
                 *              then draw new one. We also refresh the currently selected
                 *              line, in case it got trashed by the highlighting.
                 *
                 *              Note that we must first move the bob off-screen while we
                 *              render the changes, otherwise it gets confused and tends
                 *              to obscure the output.
                 */
                TextBob->bob.BobVSprite->X = 0;
                TextBob->bob.BobVSprite->Y = -9999;
                SortGList(FormWindow->RPort);
                DrawGList(FormWindow->RPort, ViewPortAddress(FormWindow));
                ShowDragSelect(FBoxDragSelect, FORMAT_UNSELECTED);
                ShowFormatLine(FBoxSelected, FORMAT_SELECTED);
                ShowDragSelect(pos, FORMAT_SELECTED);
                FBoxDragSelect = pos;
        }

        TextBob->bob.BobVSprite->X = x + FBoxDeltaX;
        TextBob->bob.BobVSprite->Y = y + FBoxDeltaY;
        SortGList(FormWindow->RPort);
        DrawGList(FormWindow->RPort, ViewPortAddress(FormWindow));
}
