/*
 *              SNOOPDOS.H                                                                                              vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *              This header file defines all the global variables and structures
 *              used by the various SnoopDos code modules.
 *
 *              Exactly one module should #define MAIN before including this file,
 *              to ensure that the variables are defined (as opposed to declared).
 */

#undef GLOBAL
#undef INIT

#ifdef MAIN
#  define GLOBAL
#  define INIT(x)       = x
#else
#  define GLOBAL        extern
#  define INIT(x)
#endif

#ifndef SNOOPTEXT_H
#define CATCOMP_NUMBERS
#include "snooptext.h"
#endif

#define MSG(x)          TextTable[x]

#define MAX(x,y)        ((x) > (y) ? (x) : (y))
#define MIN(x,y)        ((x) < (y) ? (x) : (y))

#define BTOC(addr)      ((void *)(((ULONG)(addr)) << 2))

#define SETTINGS_BASENAME       "SnoopDos.prefs"
#define LOGFILE_NAME            "SnoopDos.log"
#define PORT_NAME                       "SNOOPDOS"
// #define PATCHES_NAME         "SnoopDos Patches"              /* Up to beta 11                */
#define PATCHES_NAME            "SnoopDos Patchlist"    /* Beta 12 onwards      */
#define HELP_BASENAME           "SnoopDos"
#define HELP_AREXX_PORT         "SNOOPDOS_HELP"
#define APPICON_NAME            "SnoopDos"
#define BACKGROUND_NAME         "SnoopDos Support Process"

#define MINSTACKSIZE            12288   /* Minimum stack size we can run with   */

/*
 *              Global constants
 */
#define MAX_STR_LEN                     200             /* Max. string length for misc stuff    */
#define MAX_SHORT_LEN           40              /* Max. short string len for misc stuff */
#define MAX_LOCK_LEN            100             /* Max. length of a lock string                 */
#define MIN_BUF_SIZE            4096    /* Min. size of history buffer                  */
#define PC_NAMELEN                      40              /* Max. length of cached task names             */
#define MAX_SEGTRACKER_LEN      50              /* Max. len of segtracker module name   */
#define MAX_FORM_LEN            50              /* Max number of format %'s in form str */
#define NUM_PCACHE_ENTRIES      16              /* No. of task IDs to cache                             */
#define NUM_PACKET_ENTRIES      5               /* No. of outstanding packets to cache  */
#define NUM_CACHED_TASKS        10              /* No. of task entries to cache                 */
#define MAX_DOS_DEVICES         100             /* Maximum no of DOS devs we recognise  */
#define LOGBUFFER_SIZE          8192    /* Size of buffer for file logging              */
#define SAVEBUFFER_SIZE         4096    /* Size of i/o buffer for saving buffer */
#define MAX_LOAD_NESTING        3               /* Only 3 levels of nested config files */
#define MAX_FIELD_LEN           99              /* Maximum width of a format field              */

/*
 *              Constants associated with specific functions
 */
#define SETBUF_KEEPOLD          0               /* Keep old buffer when allocating new  */
#define SETBUF_FORCENEW         1               /* Force new, even if means losing old  */

                                                                        /*       --- for ShowBuffer() ---       */
#define DISPLAY_QUICK           1               /* Redraw buffer using quickest method  */
#define DISPLAY_ALL                     2               /* Force redraw of all lines in buffer  */
#define DISPLAY_NONE            4               /* Only update vars, don't refresh              */

                                                                        /*      --- for CreateEvent() ---               */
#define NO_EXPAND                       0               /* Don't expand filenames to full paths */
#define EXPAND_NAME                     1               /* Do expand filenames to full paths    */

                                                                        /*    --- for InstallNewFormat() ---    */
#define NEW_LISTVIEW            0               /* Indicates new listview format                */
#define NEW_STRING                      1               /* Indicates new string format                  */

                                                                        /*      -- for MainLoop() --                    */
#define DEFAULT_WIDTH           832             /* Default window width                                 */
#define DEFAULT_HEIGHT          300             /* Default window height                                */

                                                                        /*      -- CurrentLogType --            */
#define LT_FILE                         0               /* Currently logging to a file                  */
#define LT_DEVICE                       1               /* Currently logging to a device                */
#define LT_DEBUG                        2               /* Currently logging to debug device    */

                                                                        /*      -- MonitorMode --               */
#define MONITOR_NORMAL          0               /* Monitoring normal activity           */
#define MONITOR_PAUSED          1               /* Currently paused                                             */
#define MONITOR_DISABLED        2               /* Currently disabled                                   */

                                                                        /*         -- ShowGadget() --                   */
#define GADGET_DOWN             0               /* Show gadget in selected state                */
#define GADGET_UP               1               /* Show gadget in unselected state              */

                                                                        /*         -- SelectFont() --                   */
#define FONTSEL_WINDOW          0               /* Request new window font                              */
#define FONTSEL_BUFFER          1               /* Request new buffer font                              */

                                                                        /*         -- SelectFile() --                   */
#define FILESEL_LOADCONFIG      0               /* Request name of existing config file */
#define FILESEL_SAVECONFIG      1               /* Request name of new config file              */
#define FILESEL_DEFLOGNAME      2               /* Request name of default log file             */
#define FILESEL_NEWLOGNAME      3               /* Request name of new log file                 */
#define FILESEL_SAVEBUFFER      4               /* Request name of save buffer file             */
#define FILESEL_SAVEWINDOW      5               /* Request name of save window file             */

                                                                        /*         -- SaveConfig() --           */
#define SAVE_NOICON             0               /* Don't save icon with settings file   */
#define SAVE_ICON                       1               /* Try and save icon with settings file */

                                                                        /*        -- SetLogGadget() --          */
#define LG_NOREFRESH            0               /* Don't refresh main log mode gadgets  */
#define LG_REFRESH                      1               /* Do refresh main win log mode gadgets */

                                                                        /*      -- RecalcMainWindow() --        */
#define NOREDRAW_GADGETS        0               /* Don't redraw gadgets after recalc    */
#define REDRAW_GADGETS          1               /* Do redraw gadgets after recalc               */

                                                                        /*      -- UpdateDeviceList() --        */
#define SCANDEV_DELAY           0               /* Wait before checking for new devices */
#define SCANDEV_IMMEDIATE       1               /* Check for new devices immediately    */

                                                                        /*      -- CreateCustomImage() --               */
#define IMAGE_FONT                      0               /* Request image for font button                */
#define IMAGE_FILE                      1               /* Request image for file button                */

                                                                        /*         -- SaveBuffer() --                   */
#define SAVEBUF_WINDOW          0               /* Save only visible portion of buffer  */
#define SAVEBUF_ALL                     1               /* Save entire buffer                                   */
#define SAVEBUF_CLIPBOARD       ((char *)(-1)) /* Indicates saving to clipboard */

#define SAVEBUF_PROMPT          0               /* Prompt user before overwriting file  */
#define SAVEBUF_OVERWRITE       1               /* Always verwrite existing file                */

#define KB_SHORTCUT_SIZE        256             /* Size of array for keyboard shortcuts */

/*
 *              Macro (courtesy of Doug Walker) used to allocate longword-aligned
 *              data on the stack. We can't use __aligned inside our patches
 *              because the caller may not have a longword-aligned stack.
 */
#define D_S(name, type) char c_##name[sizeof(type)+3];\
                                                type *name = (type *)((long)(c_##name+3) & ~3)

/*
 *              This is the template used to match tasks, when we exclude the
 *              Workbench and Shell tasks from the list. %s refers to the user's
 *              chosen pattern. We really want to use a pattern of (A & B) where
 *              A is the pattern that excludes Workbench/Shell and B is the
 *              user pattern. Since AmigaDOS doesn't support an & operator, we
 *              need to use boolean logic to transform this to ~(~A | ~B) which
 *              produces the string below.
 *
 *              PAT_EASY_EXCLUDE is used when there is no match string -- it's a
 *              slightly simpler (and hopefully faster) version of the exclude
 *              system tasks string.
 */
#define PAT_COMPLEX_EXCLUDE     "~((Workbench|Shell Process|Background CLI)|~(%s))"
#define PAT_EASY_EXCLUDE        "~(Workbench|Shell Process|Background CLI)"

/*
 *              Some list stuff
 */
typedef struct List                             List;
typedef struct MsgPort                  MsgPort;
typedef struct Node                             Node;
typedef struct SignalSemaphore  Semaphore;
typedef struct Task                             Task;
typedef struct Process                  Process;
typedef struct Requester                Requester;

#define HeadNode(list)          ((void *)((list)->lh_Head))
#define TailNode(list)          ((void *)((list)->lh_TailPred))
#define NextNode(node)          ((void *)((Node *)(node))->ln_Succ)
#define PrevNode(node)          ((void *)((Node *)(node))->ln_Pred)

#define FORLIST(list,p)         for ((p) = (void *)(list)->lh_Head;                     \
                                                                 ((Node *)(p))->ln_Succ;                                \
                                                                 (p) = (void *)((Node *)(p))->ln_Succ)


/*
 *              Now our gadget IDs
 *
 *              If you're changing the order of the first 20-30 boolean gadgets,
 *              then be sure to make corresponding changes to the tables contained
 *              in PATCHES.C and SETTINGS.C.
 */
typedef enum {
        GID_DUMMY,                      /* Make sure gadget ID never equals zero */

#define FIRST_BOOL_GADGET       GID_FINDPORT

#define FIRST_SYS_GADGET        GID_FINDPORT

        GID_FINDPORT,           
        GID_FINDRESIDENT,       
        GID_FINDSEMAPHORE,      
        GID_FINDTASK,           
        GID_LOCKSCREEN,         
        GID_OPENDEVICE,         
        GID_OPENFONT,           
        GID_OPENLIBRARY,        
        GID_OPENRESOURCE,       
        GID_READTOOLTYPES,      
        GID_SENDREXX,   

#define LAST_SYS_GADGET         GID_SENDREXX
#define NUM_SYS_GADGETS         (LAST_SYS_GADGET - FIRST_SYS_GADGET + 1)

#define FIRST_DOS_GADGET        GID_CHANGEDIR

        GID_CHANGEDIR,          
        GID_DELETE,                     
        GID_EXECUTE,            
        GID_GETVAR,                     
        GID_LOADSEG,            
        GID_LOCKFILE,           
        GID_MAKEDIR,            
        GID_MAKELINK,           
        GID_OPENFILE,           
        GID_RENAME,                     
        GID_RUNCOMMAND,         
        GID_SETVAR,                     
        GID_SSYSTEM,

#define LAST_DOS_GADGET         GID_SYSTEM
#define NUM_DOS_GADGETS         (LAST_DOS_GADGET - FIRST_DOS_GADGET + 1)

        GID_READTOOLTYPES2,                     /* Dummy entry to match GID_READTOOLTYPES       */
        GID_LOADSEG2,                           /* Dummy entry to match GID_LOADSEG                     */
        GID_GETVAR2,                            /* Dummy entry to match GID_GETVAR                      */
        GID_SETVAR2,                            /* Dummy entry to match GID_SETVAR                      */
        GID_ADDDOSENTRY,                        /* AddDosEntry() (to catch new devices)         */

#define GID_NUMPATCHES  (GID_SETVAR2 + 1)       /* Number of patches we have    */

        /*
         *              Make sure all gadgets corresponding to patchable DOS functions
         *              appear first in the above list, since they are used to index
         *              directly into the patch list.
         *
         *              Note that we have four additional "dummy" gadget IDs which don't
         *              corresond to actual gadgets. These are used for monitoring
         *              the following function pairs:
         *
         *                      GID_READTOOLTYPES:              FindToolType()/MatchToolValue()
         *                      GID_LOADSEG:                    LoadSeg/NewLoadSeg()
         *                      GID_GETVAR:                             GetVar()/FindVar()
         *                      GID_SETVAR:                             SetVar()/DeleteVar()
         *
         *              Whenever the main gadget ID is changed, the library patch code
         *              uses the secondary gadget ID to ensure that _both_ functions
         *              are enabled/disabled. (In fact, this is done at patch time, in
         *              UpdatePatches(), not at the time the gadget itself is clicked.)
         */

        /*
         *              Now the remaining gadgets in the function window
         */
        GID_ONLYFAILS,          
        GID_SHOWCLINUM,         
        GID_SHOWPATHS,          
        GID_USEDEVNAMES,        
        GID_MONPACKETS,
        GID_MONALLPACKETS,              
        GID_MONROMCALLS,
        GID_IGNOREWB,           

        /*
         *              Now total the number of gadgets so far. Note that these are used
         *              to index into the FuncSettings->Opts array, so don't change
         *              the order of any of the above arbitrarily.
         *
         *              If you DO need to change the order for some reason, make sure to
         *              edit the DefaultSettings array below as well to reflect the new
         *              order!
         */
#define LAST_BOOL_GADGET        GID_IGNOREWB

#define GID_NUMFUNCSETS (LAST_BOOL_GADGET+1)  /* Number of function gadgets */

        /*
         *              Now the remaining gadget IDs
         */
        GID_PAUSE,
        GID_DISABLE,
        GID_OPENLOG,
        GID_APPENDLOG,
        GID_STARTLOG,
        GID_SERIALLOG,
        GID_CLOSELOG,
        GID_SETUP,
        GID_HIDE,
        GID_QUIT,
        GID_SAVESET,
        GID_FUNCTION,
        GID_STATUS,

        GID_LEFTARROW,
        GID_RIGHTARROW,
        GID_UPARROW,
        GID_DOWNARROW,
        GID_HSCROLLER,          /* Two scrollers must come after the four arrows */
        GID_VSCROLLER,

#define GID_STARTSCROLL         GID_LEFTARROW
#define GID_ENDSCROLL           GID_VSCROLLER

        GID_MATCHNAME,
        GID_SELSYSTEM,
        GID_SELDOS,
        GID_FUNCUSE,
        GID_FUNCCANCEL,
        GID_FUNCUNDO,
        GID_SETUSE,
        GID_SETCANCEL,
        GID_SETUNDO,
        GID_FORMUSE,
        GID_FORMCANCEL,
        GID_FORMUNDO,
        GID_FORMWIDTH,

        GID_HIDEMETHOD,
        GID_OPENON,
        GID_LOGMODE,
        GID_FILEIO,
        GID_BUFFERSIZE,
        GID_BUFFORMAT,
        GID_LOGFORMAT,
        GID_HOTKEY,
        GID_SCREENNAME,
        GID_LOGFILE,
        GID_WINDOWFONT,
        GID_BUFFERFONT,
        GID_FORMATEDIT,
        GID_FORMATCOPY,
        GID_FILEPROMPT,
        GID_WFONTPROMPT,
        GID_BFONTPROMPT,

        GID_NUM_GADGETS,

        /*
         *              All the gadget IDs should appear above this point, since the
         *              symbols are used to index into the Gadget[] array.
         */

        /*
         *              Next come the internal menu IDs
         */
        MID_OPENLOG,
        MID_CLOSELOG,
        MID_PAUSE,
        MID_DISABLE,
        MID_STEP,
        MID_CHANGEPRI,
        MID_HELP,
        MID_ABOUT,
        MID_HIDE,
        MID_QUIT,

        MID_SETUP,
        MID_FUNCTION,
        MID_FORMAT,
        MID_SETWIDTH,
        MID_ROWQUAL,
        MID_SPACE_NONE,
        MID_SPACE_1P,
        MID_SPACE_2P,
        MID_REF_SIMPLE,
        MID_REF_SMART,
        MID_ALIGN_LEFT,
        MID_ALIGN_RIGHT,
        MID_STATUS,
        MID_GADGETS,
        MID_AUTO_OPEN,
        MID_DISABLE_HIDDEN,

        MID_LOAD,
        MID_SAVE,
        MID_SAVEAS,
        MID_RESET,
        MID_LASTSAVED,
        MID_RESTORE,
        MID_ICONS,

        MID_COPYWIN,
        MID_COPYBUF,
        MID_SAVEWIN,
        MID_SAVEBUF,
        MID_CLEARBUF

} GadgetID;

typedef enum {
        MODE_SETTINGS,                          /* Executing command in a settings file         */
        MODE_CMDLINE,                           /* Executing command on the CLI cmd line        */
        MODE_TOOLTYPE,                          /* Executing command in an icon tooltype        */
        MODE_REXX,                                      /* Executing command received from ARexx        */
        MODE_INTERNAL,                          /* Executing an internal command                        */
} ModeEnum;

typedef enum {
        HIDE_INVIS,                                     /* Make invisible                                                       */
        HIDE_ICON,                                      /* Create Workbench AppIcon                                     */
        HIDE_TOOLS,                                     /* Create Workbench Tools menu icon                     */
        HIDE_NONE,                                      /* No hide method                                                       */
        HIDE_MAX
} HideType;

typedef enum {
        SCREEN_DEFAULT,                         /* Open window on default public screen         */
        SCREEN_FRONT,                           /* Open window on frontmost public screen       */
        SCREEN_NAMED,                           /* Open window on named public screen           */
        SCREEN_MAX
} ScreenType;

typedef enum {
        LOGMODE_PROMPT,                         /* Prompt user for filename                                     */
        LOGMODE_APPEND,                         /* Append filename to existing file                     */
        LOGMODE_OVERWRITE,                      /* Overwrite existing file                                      */
        LOGMODE_SERIALPORT,                     /* Send output to debugger using RawPutChar     */
        LOGMODE_MAX
} LogType;

typedef enum {
        FILE_AUTOMATIC,                         /* Determine i/o type automatically                     */
        FILE_IMMEDIATE,                         /* Use immediate i/o (no buffering)                     */
        FILE_BUFFERED,                          /* Use buffered i/o                                                     */
        FILE_MAX
} FileIOType;

typedef enum {
    ROWQUAL_ANY,                                /* Ignore any qualifier when selecting rows     */
        ROWQUAL_NONE,                           /* Only select rows if no qualifier given       */
        ROWQUAL_SHIFT,                          /* Only select rows if qualifier is shift       */
        ROWQUAL_ALT,                            /* Only select rows if qualifier is alt         */
        ROWQUAL_CTRL,                           /* Only select rows if qualifier is ctrl        */
        ROWQUAL_ALL                                     /* Only select rows if any qualifier given      */
} RowQualifierType;

/*
 *              Now structures used throughout SnoopDos
 */

/*
 *              This first structure holds details of the actual events
 *
 *              Note that some of these fields are initially defined as NULL or \0
 *              when the event is created, and then filled in when they first need
 *              to be displayed by the mainline code. These are date[], time[],
 *              segname[], hunk, and offset. This is partly because it's dangerous
 *              to call the Date/Time functions via a SetFunction'd patch, and
 *              partly to save time by only decoding those quantities if they're
 *              actually required.
 *
 *              Typically, enough room will be allocated in one of the buffers to
 *              contain both the event and the strings it points to (procname,
 *              filename, segname). Segname is tricky, since it isn't always
 *              necessarily required. It's handled by allocating sufficient room
 *              if segtracking is enabled, or setting it to point to the string
 *              "<Unavailable>" otherwise. (Note that calladdr is used to
 *              determine the segtracker-related info.)
 *
 *              The sequence number is montonically increasing, from the first event
 *              recorded, and is used to allow easy traversal of the buffer.
 *              A seqnum of zero indicates that this event is currently unused.
 *              We use signed rather than unsigned because we occasionally need
 *              to subtract one from another and check if the difference is +ve
 *              or -ve.
 */
typedef struct Event {
        struct MinNode  mnode;          /* Used for linking events together                     */
        LONG                    seqnum;         /* Sequence number of this event (from 0)       */
        char                    *procname;      /* Name of calling process                                      */
        char                    *action;        /* Type of action (Open, Lock, Load, etc.)      */
        char                    *filename;      /* Name of the file being acted on                      */
        char                    *options;       /* Options associated with this function        */
        char                    *result;        /* Result (usually Okay or Fail)                */
        ULONG                   processid;      /* Address of process that called us            */
        ULONG                   calladdr;       /* Address we were called from                          */
        struct DateStamp datestamp;     /* Date and time when we were called            */
        char                    date[10];       /* String of date when we were called           */
        char                    time[10];       /* String of time when we were called           */
        char                    *segname;       /* Pointer to name of caller (segtracker)       */
        ULONG                   offset;         /* Offset we were called from (segtracker)      */
        UWORD                   hunk;           /* Hunk we were called from (segtracker)        */
        UBYTE                   status;         /* Status of this event (see ES_* enums)        */
        UBYTE                   flags;          /* See EFLG_* defines below                             */
} Event;

#define EFLG_NONE               0               /* Default: No flags set                                        */
#define EFLG_DONEDATE   0x01    /* Set if date has been converted to text       */
#define EFLG_DONESEG    0x02    /* Set if segment info has been looked up       */

/*
 *              These next two flags are used when logging events to a disk file
 */
#define EFLG_LOGPARTIAL 0x04    /* Set if partial entry for line was logged     */
#define EFLG_LOGDONE    0x08    /* Set if entire entry for line was logged      */

/*
 *              These enums define the state of the current event; they are used by
 *              the mainline code while updating the display to ensure we don't try
 *              and access garbage values.
 *
 *              ES_CREATING indicates that the event should not be accessed.
 *
 *              ES_UPDATING indicates that all fields are valid, but some may not
 *              be complete. Thus, the mainline code should continue to output the
 *              contents of this event to the screen whenever it gets a chance.
 *              This is intended to allow Result string to be set after all the
 *              others have already been set (so that the line can be output both
 *              before and after the actual DOS call is made -- that way, if the
 *              call is causing a crash, we can see which call it was.)
 *
 *              ES_READY indicates that the patch code has finished filling in the
 *              event, and the mainline code can go ahead and consider it complete.
 *
 *              ES_ACCEPTED means that the mainline code has seen the line in the
 *              ES_READY state and no longer has to worry about having to check it.
 *
 *              Note that while the patch code is responsible for setting the first
 *              three states, the mainline code sets the final ES_ACCEPTED state.
 */
typedef enum {
        ES_CREATING,                            /* This event is being created                          */
        ES_UPDATING,                            /* This event is being updated                          */
        ES_READY,                                       /* This event is ready to be displayed          */
        ES_ACCEPTED                             /* This event was successfully displayed        */
} EventStatus;

/*
 *              This next structure is used to link all the buffers together. Each
 *              buffer holds a number of variable-size data structures describing
 *              the contents of the buffer.
 */
typedef struct Buffer {
        struct MinNode  mnode;          /* Used for linking buffers together            */
        ULONG                   size;           /* Total size of this buffer in bytes           */
        ULONG                   free;           /* No. of free bytes remaining in this blk      */
        BYTE               *nextfree;   /* Pointer to next free location in buffer      */
        struct Event    firstevent[1];/* Contains first event in buffer                 */
} Buffer;

/*
 *              This next structure is used to define the layout of a display format
 *              that can be output to our window or to a disk file. The basic idea
 *              is to encode a string such as "%20f %5d %4r" into an internal form
 *              that can be efficiently scanned to produce the desired result.
 *
 *              Our chosen form is simple, yet efficient. We simply build an array
 *              of this structure, and scan it until we hit an EF_END type.
 */
typedef struct EventFormat {
        char    type;                           /* Type of event (see enum FormatID below)      */
        char    width;                          /* Width of event column in bytes                       */
        short   titlemsgid;                     /* Message ID of column title                           */
} EventFormat;

typedef enum {
        EF_END,
        EF_ACTION,
        EF_CALLADDR,
        EF_DATE,
        EF_HUNKOFFSET,
        EF_PROCID,
        EF_FILENAME,
        EF_PROCNAME,
        EF_OPTIONS,
        EF_RESULT,
        EF_SEGNAME,
        EF_TIME,
        EF_COUNT
} FormatID;

#define MAX_BUFFER_WIDTH        (((MAX_FIELD_LEN + 1) * EF_COUNT) + 1)

/*
 *              This structure is used to initialise the format fields -- it maps
 *              format types onto corresponding %x sequences and text descriptions
 */
typedef struct {
        char    type;                   /* Type of this format field                                    */
        char    idchar;                 /* Lowercase ascii char used to identify it             */
        char    defwidth;               /* Default width if none specified                              */
        short   titlemsgid;             /* Message ID of title                                                  */
} FieldInit;

/*
 *              This structure holds all the settings that will be used
 *              by SnoopDos. We collect them into a single structure to
 *              make it easy to do things like Load/Save settings,
 *              Restore an earlier set of settings, Cancel out of a
 *              settings requester, etc.
 *
 *              There are actually three types of settings: those set via
 *              the Setup... requester, those set via the Functions...
 *              requester, and everything else.
 */
typedef struct SetupSettings {
        UBYTE   HideMethod;                                     /* Icon, Tools menu or invisible        */
        UBYTE   ScreenType;                                     /* Workbench, Frontmost, or public      */
        UBYTE   LogMode;                                        /* Prompt, Append, Overwrite            */
        UBYTE   FileIOType;                                     /* Immediate, Buffered, Automatic       */
        ULONG   BufferSize;                                     /* In kilobytes                                         */
        char    HotKey[MAX_SHORT_LEN];          /* Standard commodities string          */
        char    ScreenName[MAX_SHORT_LEN];      /* Only used with Public ScreenType     */
        char    LogFile[MAX_STR_LEN];           /* Log name if Logmode not Prompt       */
        char    WindowFont[MAX_SHORT_LEN];      /* Default window font                          */
        char    BufferFont[MAX_SHORT_LEN];      /* Default buffer font                          */
        UWORD   WinFontSize;                            /* Size of window font                          */
        UWORD   BufFontSize;                            /* Size of buffer font                          */
        char    BufferFormat[MAX_STR_LEN];      /* Format string for buffer                     */
        char    LogfileFormat[MAX_STR_LEN];     /* Format string for logfile            */
} SetupSettings;

typedef struct FuncSettings {
        UBYTE   Opts[GID_NUMFUNCSETS];          /* One byte for each setting            */
        char    Pattern[MAX_STR_LEN];           /* Current pattern to match                     */
} FuncSettings;

typedef struct Settings {
        SetupSettings   Setup;
        FuncSettings    Func;
        UBYTE                   AutoOpenMain;           /* If true, auto-open win on output     */
        UBYTE                   DisableWhenHidden;      /* If true, disable when hidden         */
        UBYTE                   ShowStatus;                     /* If true, display status line         */
        UBYTE                   ShowGadgets;            /* If true, display gadgets in win      */
        UBYTE                   MakeIcons;                      /* If true, create icons for config     */
        UBYTE                   TextSpacing;            /* 0, 1 or 2: spacing between lines     */
        UBYTE                   SimpleRefresh;          /* True = Simple, False = Smart         */
        UBYTE                   RightAlign;                     /* Right-align target name field        */
        UBYTE                   RowQualifier;           /* Cur qualifier for row selection      */
        UBYTE                   pad;                            /* Keep things neatly aligned           */
        WORD                    MainWinLeft;            /* X pos of main window                         */
        WORD                    MainWinTop;                     /* Y pos of main window                         */
        WORD                    MainWinWidth;           /* Width of main window                         */
        WORD                    MainWinHeight;          /* Height of main window                        */
        WORD                    SetupWinLeft;           /* X pos of setup window                        */
        WORD                    SetupWinTop;            /* Y pos of setup window                        */
        WORD                    FuncWinLeft;            /* X pos of function window                     */
        WORD                    FuncWinTop;                     /* Y pos of function window                     */
        WORD                    FormWinLeft;            /* X pos of format window                       */
        WORD                    FormWinTop;                     /* Y pos of format window                       */
        WORD                    IconPosLeft;            /* X pos of AppIcon (-1 = default)      */
        WORD                    IconPosTop;                     /* Y pos of AppIcon (-1 = default)      */
        ULONG                   StackLimit;                     /* Stack limit on patched functions     */
} Settings;

/*
 *              These are used when selecting what settings to activate. OR them
 *              together to create combinations.
 */
#define SET_NONE                0                               /* No settings changed                          */
#define SET_FUNC                1                               /* Activate function settings           */
#define SET_SETUP               2                               /* Activate setup settings                      */
#define SET_MAIN                4                               /* Activate main window settings        */

#define SET_ALL                 (SET_FUNC | SET_SETUP | SET_MAIN)

/*
 *              These are the default settings used via "Reset to Defaults"
 */
GLOBAL Settings DefaultSettings
#ifdef MAIN
= {
        /*
         *              First our setup settings
         */
        {
                HIDE_ICON,                      /* Hide method                                          */
                SCREEN_DEFAULT,         /* Screen type                                          */
                LOGMODE_PROMPT,         /* Log method                                           */
                FILE_AUTOMATIC,         /* File I/O type                                        */
                32,                                     /* Buffer size (KB)                                     */
                "ctrl alt d",           /* Commodities hotkey                           */
                "Workbench",            /* Public screen name                           */
                "ram:SnoopDos.log",     /* Default log filename                         */
                "",                                     /* Window font filled in at startup     */
                "",                                     /* Font size filled in at startup       */
                0,                                      /* Buffer font filled in at startup     */
                0,                                      /* Font size filled in at startup       */
                "%u %p %a %n %o %r",/* Buffer format string                         */
                ""                                      /* Log format string same as buffer     */
        },
        /*
         *              Now our function settings. Make sure that these match the
         *              order of the GID_* gadget IDs corresponding to each section
         *              which are defined near the start of this file.
         */
         {
                0,                                      /* Dummy value, never used                      */      
                0,                                      /* FindPort                                                     */
                0,                                      /* FindResident                                         */
                0,                                      /* FindSemaphore                                        */
                0,                                      /* FindTask                                                     */
                1,                                      /* LockScreen                                           */
                0,                                      /* OpenDevice                                           */
                0,                                      /* OpenFont                                                     */
                0,                                      /* OpenLibrary                                          */
                0,                                      /* OpenResource                                         */
                1,                                      /* ReadToolTypes                                        */
                0,                                      /* SendRexx                         */
                1,                                      /* ChangeDir                                            */
                1,                                      /* Delete                                                       */
                1,                                      /* Execute                                                      */
                0,                                      /* GetVar                                                       */
                1,                                      /* LoadSeg                                                      */
                0,                                      /* Lock                                                         */
                1,                                      /* MakeDir                                                      */
                1,                                      /* MakeLink                                                     */
                1,                                      /* Open                                                         */
                1,                                      /* Rename                                                       */
                0,                                      /* RunCommand                                           */
                0,                                      /* SetVar                                                       */
                0,                                      /* System                                                       */
                0,                                      /* Dummy: ReadToolTypes2                        */
                0,                                      /* Dummy: LoadSeg2                                      */
                0,                                      /* Dummy: GetVar2                                       */
                0,                                      /* Dummy: SetVar2                                       */
                0,                                      /* Dummy: AddDosEntry                           */
                0,                                      /* OnlyShowFails                                        */
                1,                                      /* Show CLI number                                      */
                0,                                      /* ShowPaths                                            */
                0,                                      /* UseDevNames                                          */
                0,                                      /* MonPackets                                           */
                0,                                      /* Monitor ALL packets                          */
                0,                                      /* Monitor ROM calls                            */
                1,                                      /* IgnoreWB                                                     */

                /*
                 *              Note that by putting the pattern string at the end,
                 *              we can force a warning from the compiler if we update
                 *              the GID_* definitions and forget to update the above
                 *              list accordingly.
                 */
                ""                                      /* Default to no special pattern        */
        },
        /*
         *              Now the remaining settings
         */
        0,                                              /* No auto-open on output                       */
        0,                                              /* No disable when hidden                       */
        1,                                              /* Show status line                                     */
        1,                                              /* Show gadgets                                         */
        1,                                              /* Create icons                                         */
        0,                                              /* 0 spacing between text lines         */
        0,                                              /* No simple refresh window                     */
        0,                                              /* No right aligned target name         */
        ROWQUAL_ANY,                    /* Any row qualifier will do            */
        0,                                              /* Padding byte                                         */
        -1, -1, -1, -1,                 /* Main window size/pos (-1 = dflt)     */
        -1, -1,                                 /* Setup win   position (-1 = dflt)     */
        -1, -1,                                 /* Func window position (-1 = dflt)     */
        -1, -1,                                 /* Format window pos    (-1 = dflt) */
        -1, -1,                                 /* Icon pos (-1 = dflt)                         */
        1000                                    /* Stack limit for monitored funcs      */
}
#endif /* MAIN */
;

/*
 *              Some definitions to make it easier to access current global settings
 */
#define OnlyShowFails           CurSettings.Func.Opts[GID_ONLYFAILS]
#define ShowCLINum                      CurSettings.Func.Opts[GID_SHOWCLINUM]
#define ShowPaths                       CurSettings.Func.Opts[GID_SHOWPATHS]
#define UseDevNames                     CurSettings.Func.Opts[GID_USEDEVNAMES]
#define MonPackets                      CurSettings.Func.Opts[GID_MONPACKETS]
#define ShowAllPackets          CurSettings.Func.Opts[GID_MONALLPACKETS]
#define MonROMCalls                     CurSettings.Func.Opts[GID_MONROMCALLS]
#define IgnoreWBShell           CurSettings.Func.Opts[GID_IGNOREWB]

#define MatchName               CurSettings.Func.Pattern

#define DefaultLogMode          CurSettings.Setup.LogMode
#define DefaultLogName          CurSettings.Setup.LogFile
#define BufFormat                       CurSettings.Setup.BufferFormat
#define LogFormat                       CurSettings.Setup.LogfileFormat
#define WindowFontName          CurSettings.Setup.WindowFont
#define WindowFontSize          CurSettings.Setup.WinFontSize
#define BufferFontName          CurSettings.Setup.BufferFont
#define BufferFontSize          CurSettings.Setup.BufFontSize

#define AutoOpen                CurSettings.AutoOpenMain
#define DisableOnHide           CurSettings.DisableWhenHidden
#define StatusLine                      CurSettings.ShowStatus
#define GadgetsLine                     CurSettings.ShowGadgets
#define BoxInterGap                     CurSettings.TextSpacing
#define CreateIcons                     CurSettings.MakeIcons
#define RefreshTag                      (CurSettings.SimpleRefresh ? WA_SimpleRefresh : \
                                                                                                                 WA_SmartRefresh)
#define NoCareRefreshBool       (CurSettings.SimpleRefresh ? FALSE : TRUE)
#define RightAligned            CurSettings.RightAlign
#define RowQual                         CurSettings.RowQualifier

/*
 *              Now, our global variables
 */

GLOBAL struct Library *AslBase;                         /* ASL library base                     */
GLOBAL struct Library *DiskfontBase;                    /* Diskfont library base        */
GLOBAL struct Library *GadToolsBase;                    /* GadTools library base        */
GLOBAL struct Library *LayersBase;                      /* Layers library base          */
GLOBAL struct Library *IconBase;                        /* Icon library base            */
GLOBAL struct GfxBase *GfxBase;                         /* Graphics library base        */
GLOBAL struct Library *RexxSysBase;                     /* REXX library base            */
GLOBAL struct UtilityBase *UtilityBase;                 /* Utility library base         */
GLOBAL struct Library *CxBase;                          /* Commodities base                     */
GLOBAL struct Library *AmigaGuideBase;                  /* AmigaGuide base                      */
GLOBAL struct Library *WorkbenchBase;                   /* Workbench base                       */
GLOBAL struct Library *KeymapBase;                              /* Keymap base                          */

GLOBAL struct IntuitionBase *IntuitionBase;             /* Intuition library base       */

GLOBAL Settings         CurSettings;                            /* Our current settings         */
GLOBAL Settings         RestoreSettings;                        /* Settings we started with     */
GLOBAL char                     *TextTable[NUM_OF_MSGS];        /* Message table for text       */
GLOBAL char                     Language[34];                           /* Name of current locale       */

GLOBAL char                     WindowFontDesc[MAX_SHORT_LEN];  /* Font description             */
GLOBAL char                     BufferFontDesc[MAX_SHORT_LEN];  /* Font description             */

/*
 *              The name used to hold our configuration file, our ASL-selected
 *              log name, and our ASL save buffer name
 */
GLOBAL char                     DefaultConfigName[MAX_LOCK_LEN];
GLOBAL char                     ConfigFileName[MAX_LOCK_LEN];
GLOBAL char                     BufferFileName[MAX_LOCK_LEN]    INIT(LOGFILE_NAME);
GLOBAL char                     ChosenLogName[MAX_LOCK_LEN]     INIT(LOGFILE_NAME);

/*
 *              This array holds the actual gadget pointer for (almost) all the
 *              gadgets we create; we need this pointer so that we can change
 *              the state of the gadget when the window is open.
 */
GLOBAL struct Gadget *Gadget[GID_NUM_GADGETS];

GLOBAL struct DrawInfo          *ScreenDI;                      /* Screen render info           */
GLOBAL struct Screen            *SnoopScreen;           /* Screen window is on          */
GLOBAL struct Image                     *SizeImage;                     /* Size gadget image            */
GLOBAL int                                      TitlebarHeight;         /* Height of win titlebar   */
GLOBAL int                                      ScreenWidth;            /* Width of SnoopDos screen     */
GLOBAL int                                      ScreenHeight;           /* Height of    "    screen */
GLOBAL int                                      ScreenResolution;       /* SYSISIZE_(LOWRES|MEDRES)     */
GLOBAL int                                      CurWindowWidth;         /* Current window width         */
GLOBAL int                                      CurWindowHeight;        /* Current window height        */

/*
 *              When ClearMainRHS is set to 1, the right hand side of the main
 *              window will be fully erased on the next redraw (normally, we only
 *              draw out as far as the rightmost entry it the current format, to
 *              save some time).
 *
 *              When PurgeFuncGadgets is 1, the function gadgets will be freed the
 *              next time the functions window is closed (usually set when the
 *              font has been changed).
 */
GLOBAL int ClearMainRHS;
GLOBAL int PurgeFuncGadgets;

GLOBAL char   SystemFontName[60];                               /* Name of system font          */
GLOBAL struct FontRequester *WindowFR;                  /* Window font requester        */
GLOBAL struct FontRequester *BufferFR;                  /* Buffer font requester        */

GLOBAL struct Window            *MainWindow;            /* Main SnoopDos window         */
GLOBAL struct Window            *FuncWindow;            /* Functions window                     */
GLOBAL struct Window            *FormWindow;            /* Format window                        */
GLOBAL struct Window            *SetWindow;                     /* Settings window                      */

GLOBAL struct VisualInfo        *MainVI;                        /* Main window visual info      */
GLOBAL struct VisualInfo        *FuncVI;                        /* Func window visual info      */
GLOBAL struct VisualInfo        *FormVI;                        /* Form window visual info      */
GLOBAL struct VisualInfo        *SetVI;                         /* Set  window visual info      */

GLOBAL struct MsgPort           *MainWindowPort;        /* Msg port for main window     */
GLOBAL struct MsgPort           *FuncWindowPort;        /* Msg port for func window     */
GLOBAL struct MsgPort           *FormWindowPort;        /* Msg port for form window     */
GLOBAL struct MsgPort           *SetWindowPort;         /* Msg port for set window      */
GLOBAL struct MsgPort           *SnoopPort;                     /* ARexx message port           */

GLOBAL char             StatusLineText[150];            /* Cur. msg in status line      */      
GLOBAL char                     CurrentTitle[200];                      /* Cur. main window title   */

GLOBAL ULONG            MainWindowMask;                         /* Sig mask for main window     */
GLOBAL ULONG            FuncWindowMask;                         /* Sig mask for func window     */
GLOBAL ULONG            FormWindowMask;                         /* Sig mask for format win      */
GLOBAL ULONG            SetWindowMask;                          /* Sig mask for set window      */
GLOBAL ULONG            NewEventMask;                           /* Sig mask for new DOS evt */
GLOBAL ULONG            WorkbenchMask;                          /* Sig mask for WBench msgs     */
GLOBAL ULONG            CommodityMask;                          /* Sig mask for CX msgs         */
GLOBAL ULONG            ScanDosListMask;                        /* Sig mask to rescan devs      */
GLOBAL ULONG            AmigaGuideMask;                         /* Sig mask for AmigaGuide      */
GLOBAL ULONG            RexxPortMask;                           /* Sig mask for Rexx msgs       */

GLOBAL int                      BorderLeft;                     /* Width of left border in window       */
GLOBAL int                      BorderRight;            /* Width of right border in window      */
GLOBAL int                      BorderTop;                      /* Height of top border in window       */
GLOBAL int                      BorderBottom;           /* Height of bottom border in win   */
GLOBAL int                      SquareAspect;           /* True if aspect ratio is 1:1          */
GLOBAL int                      GadgetHeight;           /* According to aspect ratio            */
GLOBAL int                      GadgetSpacing;          /* According to aspect ratio            */

GLOBAL int                      QuitFlag;                       /* If true, user said quit                      */
GLOBAL int                      HideOnStartup;          /* True if HIDE found in config         */
GLOBAL int                      CommodityPriority;      /* Pri to install Commodity at          */
GLOBAL int                      NoPatchRamLib;          /* If true, skip patching ramlib        */
GLOBAL ULONG            SegTrackerActive;       /* Add ev space for ST info             */
GLOBAL ULONG            RomStart;                       /* Start ROM address                            */
GLOBAL ULONG            RomEnd;                         /* End ROM address                                      */

/*
 *              These variables all deal with the list of monitored events.
 *              All events are numbered in a monotonically increasing manner.
 *              We keep track of the sequence number associated with each
 *              pointer, so that we can easily tell if a pointer is valid
 *              or not by checking if it falls within the range of the
 *              sequence number associated with the first and last events
 *              on the list. (In practice, we only need to check the first
 *              sequence number.)
 *
 *              LastDrawnTopSeq is similar to TopSeq, but differs in the case
 *              where what's displayed in the window has scrolled off the
 *              top of the buffer since the window was last refreshed. In
 *              this case, we can detect that LastDrawnTopSeq is now < FirstSeq
 *              and refresh the window accordingly (when we get an IDCMP_REFRESH
 *              request), rather than refreshing part of the window one way and
 *              part the other.
 *
 *              The EndComplete* variables give the highest event that is
 *              "complete", i.e. that has been set to type ES_ACCEPTED, and
 *              which no longer needs to be refreshed.
 */
GLOBAL List                     EventList;                                      /* List of captured events      */
GLOBAL Semaphore        BufSem;                                         /* Controls  access to list     */
GLOBAL Semaphore        PauseSem;                                       /* Controls pausing                     */
GLOBAL LONG                     NextSeq;                                        /* # of newest event            */
GLOBAL LONG                     BaseSeq;                                        /* All others are > this        */
GLOBAL LONG                     FirstSeq;                                       /* # of event at window top     */
GLOBAL LONG                     RealFirstSeq;                           /* as FSeq but ALWAYS right     */
GLOBAL LONG                     TopSeq;                                         /* # of event at window top     */
GLOBAL LONG                     LastDrawnTopSeq;                        /* # of event disp'd at top     */
GLOBAL LONG                     BottomSeq;                                      /* # of event at window bot     */
GLOBAL LONG                     MaxScannedSeq;                          /* # of highest ev scanned      */
GLOBAL LONG                     EndSeq;                                         /* # of highest ev scanned      */
GLOBAL LONG                     EndCompleteSeq;                         /* # of h. complete ev scnd     */
GLOBAL LONG                     BufferWidth;                            /* Max width of current buf     */
GLOBAL LONG                     LogWidth;                                       /* Max width of log output      */

GLOBAL Event            *TopEvent;                                      /* Event at top of screen       */
GLOBAL Event            *BottomEvent;                           /* Event at bot of screen       */
GLOBAL Event            *EndEvent;                              /* Highest event scanned        */
GLOBAL Event            *EndCompleteEvent;                      /* Max complete event scand */

GLOBAL int                      HotKeyActive;                           /* True if valid hotkey         */
GLOBAL APTR                     *TaskWindowPtr;                         /* Points to pr_WindowPtr       */
GLOBAL APTR                     SaveWindowPtr;                          /* Saved value for DOS reqs     */

GLOBAL int                      MonitorType;                            /* See MONITOR_??? defines      */
GLOBAL int                      Paused;                                         /* If true, we're paused        */
GLOBAL int                      Disabled;                                       /* If true, we're disabled      */
GLOBAL int                      LastKnownState;                         /* State when we did HIDE       */
GLOBAL int                      LogActive;                                      /* If true, we're logging       */
GLOBAL int                      GotLastSaved;                           /* 1=Last Saved is valid        */

GLOBAL int                      DisableNestCount;                       /* #times DisableWin called     */
GLOBAL Requester        MainRequester;                          /* For disabling main win       */
GLOBAL Requester        SetRequester;                           /* For disableing setup win     */
GLOBAL Requester        FuncRequester;                          /* For disabling func win       */
GLOBAL Requester        FormRequester;                          /* For disabling form win       */

GLOBAL int                      CurrentLogType;                         /* See LT_??? defines above     */
GLOBAL char                     CurrentLogName[120];            /* Name of current logfile      */

GLOBAL struct DateStamp  PauseDateStamp;                /* Time we paused at            */
GLOBAL struct DateStamp  DisableDateStamp;              /* Time we disabled at          */

GLOBAL EventFormat      BufferEFormat[MAX_FORM_LEN];/* Format specifier for buf */
GLOBAL EventFormat      LogEFormat[MAX_FORM_LEN];       /* Format specifier for log     */

/*
 *              Prototypes for LANGUAGE.C
 */
void InitTextTable(void);
void InitLocale(char *catalogname);
void CleanupLocale(void);

/*
 *              Prototypes for BUFFER.C
 */
void    InitBuffers(void);
int     SetTotalBufferSize(ULONG newsize, int alwaystry);
void    ClearBuffer(void);
void    CleanupBuffers(void);
Event  *GetNewEvent(int stringsize);
void    CopyEvents(UBYTE *newbuf, ULONG newsize, UBYTE *oldbuf, ULONG oldsize);
int             ParseFormatString(char *formatstr, EventFormat *evformat,
                                                                                   int maxfields);
void    BuildFormatString(EventFormat *evformat, char *formatstr, int maxlen);
char   *UnderlineTitles(EventFormat *ef, char *outstr, char underchar);
void    CheckSegTracker(void);
void FormatEvent(EventFormat *eventformat, Event *event,
                                 char *outstr, int start, int end);

/*
 *              Prototypes for PATCHES.C
 */
void LoadFuncSettings(FuncSettings *func);
void HandlePatternMsgs(void);
void UpdateDeviceList(int method);
void InitRamLibPatch(void);
void CleanupPatches(void);
int InitPatches(void);
void SetPattern(char *pattern, int ignorewb);

/*
 *              Prototypes for SNOOPDOS.C
 */
void InstallSettings(Settings *set, int which);
void mysprintf(char *outstr, char *fmtstr, ...);
char *GetFontDesc(char *fontdesc, char *fontname, int size);
void Cleanup(int errcode);

/*
 *              Prototypes for MAINWIN.C
 */
void CleanupMainWindow(void);
void InitMenus(void);
void ClearWindowBuffer(void);
void ShowBuffer(LONG seqnum, int displaytype);
void DrawSelectedLine(int row, int highlight);
void HandleNewEvents(void);
void UpdateMainVScroll(void);
void SetMenuOptions(void);
void InstallNewFormat(int type);
void ReOpenMainWindow(void);
void RecordWindowSizes(void);
void SetMonitorMode(int modetype);
void ScrollHorizontal(int amount);
void SetLogGadget(int logmode, int refresh);
int  SaveBuffer(int savetype, char *savename, int overwrite);
int  OpenLog(int logmode, char *filename);
void CloseLog();
void WriteLog(char *string);
void ShowStatus(char *msg);
void UpdateStatus(void);
int  ShowSnoopDos(void);
void HideSnoopDos(void);
BOOL OpenMainWindow(void);
void CloseMainWindow(void);
int  CheckForScreen(void);
void UpdateMainHScroll(void);
void UpdateMainVScroll(void);
void SetMainHideState(int hidestate);
int  IsDiskFile(char *filename);
void HandleMainMsgs(void);
BOOL RecalcMainWindow(int width, int height, int dorefresh);
void SetTextSpacing(int newspacing);
void DoArrowScrolling(int arrowtype, int amount);
void SetMainWindowWidth(int colwidth);
void CloseLog(void);
void SingleStep(void);
void InitMainMargins(void);
void RedrawMainWindow(void);

/*
 *              Prototypes for SUBWIN.C
 */
void CleanupSubWindow(void);
void HandleFormatMsgs(void);
void HandleSettingsMsgs(void);
void HandleFunctionMsgs(void);
void HandleFuncMsgs(void);
char *GetFuncName(int gadgetid);
int  OpenFormatWindow(void);
void CloseFormatWindow(void);
void CloseFunctionWindow(void);
void CloseSettingsWindow(void);
void ShowFuncOpts(UBYTE newopts[], int firstid, int lastid);
void ResetFuncToSelected(void);
int OpenSettingsWindow(void);
int OpenFunctionWindow(void);
void CleanupSubWindows(void);


/*
 *              Prototypes for MISCWIN.C
 */
void InitFonts(void);
int  SetupScreen(void);
void CleanupScreen(void);
void ShowError(char *errormsg, ...);
int  GetResponse(char *prompts, char *reqmsg, ...);
int  SelectFont(struct Window *win, int fonttype);
int  SelectFile(char *newname, char *defname, struct Window *win, int type);
void AddKeyShortcut(UBYTE *shortcut, int gadid, int msgid);
struct TextFont *MyOpenFont(struct TextAttr *textattr);
int  ShowAGuide(char *cmdstring);
void HandleAGuideMsgs(void);
void CleanupAGuide(void);
struct Image *CreateCustomImage(int imagetype, int height);
void FreeCustomImage(struct Image *image);
int  ConvertIMsgToChar(struct IntuiMessage *imsg);
void ShowGadget(struct Window *win, struct Gadget *gad, int type);
void CleanupWindows(void);
void EnableAllWindows(void);
void DisableAllWindows(void);
void DisableWindow(struct Window *win, struct Requester *req);
int GetTextLen(struct TextFont *font, char *msg);
int MaxTextLen(struct TextFont *font, int *ids);


/*
 *              Prototypes for HOTKEY.C
 */
int InstallHotKey(char *hotkey);
void HandleHotKeyMsgs(void);
void CleanupHotKey(void);
void CleanupIcons(void);
void WriteIcon(char *filename);
void RemoveProgramFromWorkbench(void);
int  AddProgramToWorkbench(int hidetype);
void HandleWorkbenchMsgs(void);
struct DiskObject *GetProgramIcon(void);

/*
 *              Prototypes for SETTINGS.C
 */
int  LoadConfig(char *filename, int mode, Settings *set);
int  SaveConfig(char *filename, int saveicon);
int  ExecCommand(char *cmdline, int mode, Settings *set);
int  InitRexxPort(void);
void HandleRexxMsgs(void);
void CleanupRexxPort(void);
void InitSettings(void);
int  ParseStartupOpts(int argc, char **argv);
void ShowCommands(BPTR file);
