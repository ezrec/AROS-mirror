/*
 *              PATCHES.C                                                                       vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *      Updated by Thomas Richter, THOR, 5.3.2000.
 *
 *              Controls the patching of dos.library and other functions in a
 *              reliable manner.
 */

#define DEBUG_PATTERN           0

#define MONITOR_SEMAPHORE       0
#define STACK_CHECK                     0               /* Not currently used */

#ifdef __SASC
#pragma libcall SysBase RawPutChar 204 001
#endif

#include "system.h"
#include "snoopdos.h"
#include "patches.h"

/*
 *              These four pointers are imported from PATCHCODE.S, which is
 *              also where the real code size is determined.
 */
#define CODESIZE                88      /* Must equal PatchCode_End - PatchCode_Start   */
#define STACKADJUST             14      /* Must equal pc_JumpOrigFunc - pc_NormalReturn */

#define PatchCode_Size            (PatchCode_End - PatchCode_Start)
#define PatchCode_StackAdjust (PatchCode_JumpOrigFunc - PatchCode_NormalReturn)

extern char PatchCode_Start[];
extern char PatchCode_NormalReturn[];
extern char PatchCode_JumpOrigFunc[];
extern char PatchCode_End[];


#if MONITOR_SEMAPHORE
Task *LoadTask;                         /* Indicates we're inside LoadSeg() */
#endif

/*
 *              Now some packet IDs not defined by Commodore in dos/dosextens.h
 */
#define ACTION_DOUBLE             2000  /* Conman: create pipe filehandle       */
#define ACTION_FORCE              2001  /* Conman: Force input into handler     */
#define ACTION_DROP                       2004  /* Conman: Discard all queued input     */

#define ACTION_GET_DISK_FSSM  4201      /* Get disk startup message                     */
#define ACTION_FREE_DISK_FSSM 4202      /* Free disk startup message            */

/*
 *              Next, our three message types for the background process. These
 *              correspond to pattern and pathname expansion messages.
 */
#define QUIT_MSG                0
#define PATTERN_MSG             1
#define PATHEXPAND_MSG  2

/*
 *              Some miscellaneous strings
 */
char LinkPointerString[] = " --> ";

/*
 *              I needed a quick way to hook into ReleaseSemaphore() to try and
 *              figure out why ramlib would sometimes crash on calling it. What
 *              better way to do this than by re-using one of our existing
 *              patched functions (one which takes a parameter in A0). OpenDevice
 *              fits the bill nicely (we just ignore the additional parameters).
 */
#if MONITOR_SEMAPHORE
#undef LVO_OpenDevice
#define LVO_OpenDevice          -570 // ReleaseSemaphore() LVO vector
#endif

/*
 *              This structure is used to communicate between the PutMsg() patch
 *              and the background SnoopDos process when doing path expansion
 *              for ACTION_MAKE_LINK or ShowFullPaths (when we're monitoring packets,
 *              we can't guarantee that the calling processes' message port is free).
 */
typedef struct NameFromLockMsg {
        struct Message  msg;            /* Standard exec message                                */
        int                             type;           /* Type of this message                                 */
        BPTR                    lock;           /* Lock to calculate path relative to   */
        char                    *filename;      /* Filename relative to lock                    */
        char                    *buf;           /* Buffer to store result in                    */
        int                             maxlen;         /* Maximum length of buffer                             */
        char                    *newbuf;        /* On return, points to path in buffer  */
        int                             sigmask;        /* Signal number to signal when done    */
        Task                    *task;          /* Task to signal when done                             */
} NameFromLockMsg;

/*
 *              This structure is used to communicate between the patches and
 *              the background SnoopDos process when doing pattern matching.
 */
typedef struct PatternMsg {
        struct Message  msg;            /* Standard exec message                                */
        int                             type;           /* Type of this message                                 */
        char                    *name;          /* Name of task to check                                */
        int                             match;          /* Result of pattern match (1 == okay)  */
        int                             sigmask;        /* Signal number to signal when done    */
        Task                    *task;          /* Task to signal when done                             */
} PatternMsg;

/*
 *              This structure is used to cache the results of pattern comparisons
 *              for various tasks.
 */
typedef struct PatternCache {
        struct MinNode  node;           /* Used to link items together                          */
        Task                    *task;          /* ID of task stored in this entry                      */
        char                    name[PC_NAMELEN];/* Name of task stored in this entry   */
        int                             match;          /* True=monitor this task, false=don't          */
} PatternCache;

PatternCache    PCacheEntries[NUM_PCACHE_ENTRIES];      /* Entries on list              */
MsgPort                 BackgroundPort; /* Where to send pattern match requests to      */
List                    PatternList;    /* List of cached patterns                                      */
Semaphore               PatternCacheSem;/* Sem to control access to pattern cache       */
Semaphore               PatternBufSem;  /* Sem to control access to pattern buffer      */
Semaphore               TaskCacheSem;   /* Sem used to arbitrate cache access           */
Semaphore               DosDeviceSem;   /* Sem used to control access to dev list       */
Process                 *BackgroundProc;/* Process used to do pattern matching          */
ULONG                   PatternEnabled;         /* If true, pattern matching is enabled */
ULONG                   PatternWildcard;        /* If true, pattern contains a wildcard */
Task                    *SnoopTask;                     /* Pointer to our own task                              */
Task                    *RamLibTask;            /* Pointer to RamLib process                    */
Task                    *RealRamLibTask;        /* Pointer to Ramlib process (always!)  */
Task                    *InputTask;                     /* Pointer to input.device's task               */
ULONG                   NewEventSig    = -1;/* Signal bit used when new event ready     */
ULONG                   ScanDosListSig = -1;/* Signal bit used to say rescan list       */
ULONG                   TaskCacheIndex = 1; /* Index into task cache array                      */

#define PAT_MAX_LEN     (MAX_STR_LEN   + 50)
#define PAT_BUF_LEN     (PAT_MAX_LEN*2 + 3)

char                    PatternString[PAT_MAX_LEN];                     /* Holds source pattern */
char                    PatternBuf[PAT_BUF_LEN];                        /* Holds parsed pattern */
Task                    *CachedTask[NUM_CACHED_TASKS];          /* Holds cached tasks   */
Task                    *DeviceTaskList[MAX_DOS_DEVICES];       /* Holds device IDs             */

/*
 *              This structure records outstanding packets (for direct packet i/o
 *              that bypasses AmigaDOS) which we are watching out for a reply to.
 *              We keep all the additional info because it's likely that any given
 *              task will re-use a packet structure when sending packets to different
 *              DOS processes, and we want to be able to distinguish these cases.
 */
typedef struct WaitPacket {
        struct  MinNode         node;                   /* Used to link items together  */
        struct  DosPacket       *dp;                    /* Packet that was dispatched   */
        struct  Task            *sendtask;              /* Task that sent it                    */
        struct  MsgPort         *destport;              /* Port it was sent to                  */
        LONG    eventnum;                                       /* Associated event number              */
        Event   *event;                                         /* Associated event pointer             */
        ULONG   arg1;                                           /* ARG1 of the packet                   */
        ULONG   arg2;                                           /* ARG2 of the packet                   */
        ULONG   arg3;                                           /* ARG3 of the packet                   */
        char    *resmsg;                                        /* Message to use for result    */
        UWORD   flags;                                          /* Flags associated with result */
} WaitPacket;

Semaphore       PacketSem;                                                      /* Arbitrates list access       */
struct List PacketWaitList;                                             /* List of waiting packets      */
struct List     PacketFreeList;                                         /* List of free nodes           */
WaitPacket      PacketEntries[NUM_PACKET_ENTRIES];      /* The nodes on the list        */

/*
 *              Prototypes for all our replacement patched functions
 */

void BackgroundProcCode(void);
char *MyNameFromLock(BPTR lock, char *filename, char *buf, int maxlen);

typedef unsigned long (*FuncPtr)();

#define FPROTO(name)    ULONG ASMCALL New_##name(){return (0);}
#define DPROTO(name)    ULONG ASMCALL New_##name();

DPROTO(AddDosEntry)
DPROTO(CurrentDir)
DPROTO(DeleteFile)
DPROTO(Execute)
DPROTO(GetVar)
DPROTO(FindVar)
DPROTO(LoadSeg)
DPROTO(NewLoadSeg)
DPROTO(Lock)
DPROTO(CreateDir)
DPROTO(MakeLink)
DPROTO(Open)
DPROTO(Rename)
DPROTO(RunCommand)
DPROTO(SetVar)
DPROTO(DeleteVar)
DPROTO(SystemTagList)

DPROTO(FindPort)
DPROTO(FindResident)
DPROTO(FindSemaphore)
DPROTO(FindTask)
DPROTO(OpenDevice)
DPROTO(OpenLibrary)
DPROTO(OpenResource)
DPROTO(PutMsg)

DPROTO(OpenFont)
DPROTO(LockPubScreen)
DPROTO(FindToolType)
DPROTO(MatchToolValue)

/*
 *              These two defines control whether or not a patch is enabled. It's
 *              vital that PATCH_ENABLED be -1 rather than simply any non-zero
 *              value -- see the comments in patchcode.s for more details.
 */
#define PATCH_ENABLED           ((ULONG)(-1))
#define PATCH_DISABLED          0

/*
 *              MarkCallAddr
 *              CallAddr
 *
 *              These two macros allow us to determine what address we were called
 *              from in a patch function. MarkCallAddr should be the first
 *              declaration at the start of the patch function, and then CallAddr
 *              will correspond to the caller's address throughout that function.
 *              callmarker[0] is the place marker itself, callmarker[1] is the
 *              immediate return address, and callmarker[3] takes into the
 *              account the assembly language patch stub as well.
 */
#define MarkCallAddr            ULONG volatile callmarker[1]
#define CallAddr                        (callmarker[6])

/*
 *              JumpOrigFunc(retval)
 *
 *              This macro lets us return and call the original function from the
 *              resident patch code rather than from our C code. The passed
 *              parameter can be 0 in most cases, but if the function we're using
 *              this in had a formal parameter passed in reg_d0, then that
 *              formal parameter must be used as the return value.
 *
 *              We implement this by patching the stack directly to adjust the
 *              return address so that we arrive back at a second bit of code
 *              instead of the first bit.
 */
#define JumpOrigFunc(retval) \
        { callmarker[1] += STACKADJUST; \
          return (ULONG)(retval); }

/*
 *              The patch structure itself. Don't change the order of the first
 *              four fields without updating the code in PATCHCODE.S as well!
 *
 *              Note that we use a library number rather than a pointer to an
 *              actual library, for convenience.
 */
typedef struct Patch {
        UWORD   code[CODESIZE/2];               /* Assembly code to handle patch        */
        FuncPtr origfunc;                               /* Original function pointer            */
        FuncPtr newfunc;                                /* Replacement function pointer         */
        ULONG   enabled;                                /* If -1, patch currently active        */
        void    *sysbase;                               /* Points to ExecBase                           */
        ULONG   stackneeded;                    /* #bytes free stack required           */
        UWORD   usecount;                               /* No. of tasks currently in code       */
        UWORD   library;                                /* Library to patch                                     */
        WORD    offset;                                 /* Offset into library                          */
        WORD    pad;                                    /* Keep structure longword-aligned      */
} Patch;

#define PATCH_DEF(lib,func)     { lib, LVO_##func, (FuncPtr)New_##func }
#define PATCH_END                       { 0, 0, 0 }

/*
 *              This structure is used to initialise the main Patch array in memory.
 *
 *              IMPORTANT -- these patches MUST be defined in exactly the same
 *              order as the first entries in the GID_* defines in snoopdos.h
 *              since those same GID_* values are used to toggle the patches
 *              on and off.
 */
struct PatchInitTable {
        UWORD   library;                                /* Library number to patch                      */
        WORD    offset;                                 /* Offset in that library                       */
        FuncPtr newfunc;                                /* Replacement function to call         */
} PatchInit[] = {
        PATCH_END,
        PATCH_DEF(      EXEC_LIB,               FindPort                ),      /* GID_FINDPORT                 */
        PATCH_DEF(      EXEC_LIB,               FindResident    ),      /* GID_FINDRESIDENT             */
        PATCH_DEF(      EXEC_LIB,               FindSemaphore   ),      /* GID_FINDSEMAPHORE    */
        PATCH_DEF(      EXEC_LIB,               FindTask                ),      /* GID_FINDTASK                 */
        PATCH_DEF(      INTUITION_LIB,  LockPubScreen   ),      /* GID_LOCKSCREEN               */
        PATCH_DEF(      EXEC_LIB,               OpenDevice              ),      /* GID_OPENDEVICE               */
        PATCH_DEF(      GRAPHICS_LIB,   OpenFont                ),      /* GID_OPENFONT                 */
        PATCH_DEF(      EXEC_LIB,               OpenLibrary             ),      /* GID_OPENLIBRARY              */
        PATCH_DEF(      EXEC_LIB,               OpenResource    ),      /* GID_OPENRESOURCE             */
        PATCH_DEF(      ICON_LIB,               FindToolType    ),      /* GID_READTOOLTYPES    */
        PATCH_DEF(  EXEC_LIB,           PutMsg          ),      /* GID_SENDREXX         */

        PATCH_DEF(      DOS_LIB,                CurrentDir              ),      /* GID_CHANGEDIR                */
        PATCH_DEF(      DOS_LIB,                DeleteFile              ),      /* GID_DELETE                   */
        PATCH_DEF(      DOS_LIB,                Execute                 ),      /* GID_EXECUTE                  */
        PATCH_DEF(      DOS_LIB,                GetVar                  ),      /* GID_GETVAR                   */
        PATCH_DEF(      DOS_LIB,                LoadSeg                 ),      /* GID_LOADSEG                  */
        PATCH_DEF(      DOS_LIB,                Lock                    ),      /* GID_LOCKFILE                 */
        PATCH_DEF(      DOS_LIB,                CreateDir               ),      /* GID_MAKEDIR                  */
        PATCH_DEF(      DOS_LIB,                MakeLink                ),      /* GID_MAKELINK                 */
        PATCH_DEF(      DOS_LIB,                Open                    ),      /* GID_OPENFILE                 */
        PATCH_DEF(      DOS_LIB,                Rename                  ),      /* GID_RENAME                   */
        PATCH_DEF(      DOS_LIB,                RunCommand              ),      /* GID_RUNCOMMAND               */
        PATCH_DEF(      DOS_LIB,                SetVar                  ),      /* GID_SETVAR                   */
        PATCH_DEF(      DOS_LIB,                SystemTagList   ),      /* GID_SYSTEM                   */

        /*
         *              Now the paired functions that track their partner's state
         */
        PATCH_DEF(      ICON_LIB,               MatchToolValue  ),      /* GID_READTOOLTYPES2   */
        PATCH_DEF(      DOS_LIB,                NewLoadSeg              ),      /* GID_LOADSEG2                 */
        PATCH_DEF(      DOS_LIB,                FindVar                 ),      /* GID_GETVAR2                  */
        PATCH_DEF(      DOS_LIB,                DeleteVar               ),      /* GID_SETVAR2                  */
        PATCH_DEF(  DOS_LIB,            AddDosEntry         ),  /* GID_ADDDOSENTRY              */

        PATCH_END
};

#define PatchInitCount  (sizeof(PatchInit) / sizeof(PatchInit[0]))
#define PatchInitSize   (PatchInitCount * sizeof(Patch))

/*
 *              This anchor structure is used to let us locate the patches if we
 *              quit SnoopDos and then rerun it -- the patches themselves always
 *              stay in memory once loaded.
 */
typedef struct {
        Semaphore       sem;                    /* Semaphore to arbitrate access to patches     */
        char            name[30];               /* Somewhere permanent to store sem name        */
        Patch           patchdata[1];   /* This is an open-ended array                          */
} PatchAnchorData;

PatchAnchorData *PatchAnchor;   /* Points to the current patch anchor           */

/*
 *              If SAS/C allowed us to examine the value of Enum types from
 *              within the preprocessor, the following check would actually work
 */
#if SASC_ALLOWS_ENUM_CHECKS
#if (sizeof(PatchInit)/sizeof(PatchInit[0])) < (GID_NUMPATCHES+1)
#error "PatchInit[] table has too few entries for patches"
#else
#if (sizeof(PatchInit)/sizeof(PatchInit[0])) > (GID_NUMPATCHES+1)
#error "PatchInit[] table has too many entries for patches"
#endif
#endif
#endif

void *LibList[NUM_LIBS];

Patch *PatchList;

/*
 *              Now we have a big table of DOS packets that we recognise while
 *              monitoring. For each packet, we record the internal AmigaDOS ID
 *              we use to recognise it, the message ID of its name, the number
 *              of dp_Args to display, and the number of results to display.
 */

/*              The following constants define how many parameters are returned
 *              by the handler when it receives this packet. PK_0 means there is
 *              no return value, PK_1 means 1 return value, PK_2 means there are
 *              two return values (with the second being the error code if the
 *              first one indicates failure) and PK_2OK means there are two actual
 *              returned values, both of which are legitimate.
 */
#define PK_0                    0                       /* No return value from this packet             */
#define PK_1                    1                       /* One return value from this packet    */
#define PK_2                    2                       /* Two return values from this packet   */
#define PK_2OK                  3                       /* Two return values even when okay             */
#define PK_MASK                 3                       /* Used to extract PK_CODE                              */

/*
 *              How to detect when an event failed. Selecting none of these means
 *              that the packet can never fail.
 */
#define PKF_BOOL                0x08            /* True if zero result == fail                  */
#define PKF_NEG                 0x10            /* True if -1 result == fail                    */

/*
 *              We want to completely ignore some packets, namely all those sent
 *              by the handler to lower level devices like timer.device or scsi.device
 *              for its own needs. We use PK_IGNORE to identify all such packets
 *              so we can discard them.
 *
 *              PK_COMMON packets are those which are monitored when the
 *              "Monitor Packets" option is enabled. These are connected
 *              with the associated DOS functions Open(), Lock() etc. and
 *              are output as such by SnoopDos, rather than as raw packets.
 *
 *              PK_RAW is used for our sentinel packet which indicates that
 *              even the packet type should be output since it's not recognised.
 */
#define PK_COMMON               0x20            /* Common packet (Open, Lock, etc.)             */
#define PK_RAW                  0x40            /* Completely raw packet                                */
#define PK_IGNORE               0x80            /* Ignore this packet completely                */

/*
 *              Most packets return a value in dp_Res1 and if that value is
 *              zero or -1, then the error code is in dp_Res2. We define
 *              two constants to handle these common case.
 */
#define PK_BOOLEAN              (PKF_BOOL | PK_2)
#define PK_NEGATIVE             (PKF_NEG  | PK_2)

/*
 *              Warning: this table MUST be sorted by packet ID in ascending order,
 *              since a binary search is used to locate packet types
 */
#define LAST_PACK_MSG           0

struct PacketRef {
        UWORD   packetid;                               /* The ID we watch out for                              */
        UWORD   msgid;                                  /* The name of the action to print              */
        UBYTE   numparams;                              /* Number of parameters to display              */
        UBYTE   flags;                                  /* How to interpret the result                  */
} PacketTable[] = {
        { ACTION_STARTUP,                 MSG_ACT_STARTUP,                 1, PK_1 },
        { ACTION_GET_BLOCK,               MSG_ACT_GET_BLOCK,               1, PK_IGNORE },
        { ACTION_SET_MAP,                 MSG_ACT_SET_MAP,                 1, PK_IGNORE },
        { ACTION_DIE,                             MSG_ACT_DIE,                     0, PK_BOOLEAN },
        { ACTION_EVENT,                   MSG_ACT_EVENT,                   1, PK_IGNORE },
        { ACTION_CURRENT_VOLUME,  MSG_ACT_CURRENT_VOLUME,  1, PK_2OK },
        { ACTION_LOCATE_OBJECT,   MSG_ACT_LOCATE_OBJECT,   3, PK_BOOLEAN | PK_COMMON },
        { ACTION_RENAME_DISK,             MSG_ACT_RENAME_DISK,     1, PK_BOOLEAN },
        { ACTION_FREE_LOCK,               MSG_ACT_FREE_LOCK,               1, PK_BOOLEAN },
        { ACTION_DELETE_OBJECT,   MSG_ACT_DELETE_OBJECT,   2, PK_BOOLEAN | PK_COMMON },
        { ACTION_RENAME_OBJECT,   MSG_ACT_RENAME_OBJECT,   4, PK_BOOLEAN | PK_COMMON },
        { ACTION_MORE_CACHE,              MSG_ACT_MORE_CACHE,              1, PKF_BOOL   | PK_2OK },
        { ACTION_COPY_DIR,                MSG_ACT_COPY_DIR,                1, PK_BOOLEAN },
        { ACTION_WAIT_CHAR,               MSG_ACT_WAIT_CHAR,               1, PKF_BOOL   | PK_2OK },
        { ACTION_SET_PROTECT,             MSG_ACT_SET_PROTECT,     4, PK_BOOLEAN },
        { ACTION_CREATE_DIR,              MSG_ACT_CREATE_DIR,              2, PK_BOOLEAN | PK_COMMON },
        { ACTION_EXAMINE_OBJECT,  MSG_ACT_EXAMINE_OBJECT,  2, PK_BOOLEAN },
        { ACTION_EXAMINE_NEXT,    MSG_ACT_EXAMINE_NEXT,    2, PK_BOOLEAN },
        { ACTION_DISK_INFO,               MSG_ACT_DISK_INFO,               1, PK_BOOLEAN },
        { ACTION_INFO,                    MSG_ACT_INFO,                    2, PK_BOOLEAN },
        { ACTION_FLUSH,                   MSG_ACT_FLUSH,                   0, PK_BOOLEAN },
        { ACTION_SET_COMMENT,             MSG_ACT_SET_COMMENT,     4, PK_BOOLEAN },
        { ACTION_PARENT,                  MSG_ACT_PARENT,                  1, PK_BOOLEAN },
        { ACTION_TIMER,                   MSG_ACT_TIMER,                   1, PK_IGNORE },
        { ACTION_INHIBIT,                 MSG_ACT_INHIBIT,                 1, PK_BOOLEAN },
        { ACTION_DISK_TYPE,               MSG_ACT_DISK_TYPE,               1, PK_IGNORE },
        { ACTION_DISK_CHANGE,             MSG_ACT_DISK_CHANGE,     1, PK_1 },
        { ACTION_SET_DATE,                MSG_ACT_SET_DATE,                4, PK_BOOLEAN },
        { ACTION_SAME_LOCK,               MSG_ACT_SAME_LOCK,               2, PK_BOOLEAN },
        { ACTION_READ,                    MSG_ACT_READ,                    3, PK_NEGATIVE },
        { ACTION_WRITE,                   MSG_ACT_WRITE,                   3, PK_NEGATIVE },
        { ACTION_SCREEN_MODE,             MSG_ACT_SCREEN_MODE,     1, PK_BOOLEAN },
        { ACTION_CHANGE_SIGNAL,   MSG_ACT_CHANGE_SIGNAL,   3, PKF_BOOL   | PK_2OK },
        { ACTION_READ_RETURN,             MSG_ACT_READ_RETURN,     1, PK_IGNORE },
        { ACTION_WRITE_RETURN,    MSG_ACT_WRITE_RETURN,    1, PK_IGNORE },
        { ACTION_FINDUPDATE,              MSG_ACT_FINDUPDATE,              3, PK_BOOLEAN | PK_COMMON },
        { ACTION_FINDINPUT,               MSG_ACT_FINDINPUT,               3, PK_BOOLEAN | PK_COMMON },
        { ACTION_FINDOUTPUT,              MSG_ACT_FINDOUTPUT,              3, PK_BOOLEAN | PK_COMMON },
        { ACTION_END,                             MSG_ACT_END,                     1, PK_BOOLEAN },
        { ACTION_SEEK,                    MSG_ACT_SEEK,                    3, PK_NEGATIVE },
        { ACTION_FORMAT,                  MSG_ACT_FORMAT,                  2, PK_BOOLEAN },
        { ACTION_MAKE_LINK,               MSG_ACT_MAKE_LINK,               4, PK_BOOLEAN | PK_COMMON },
        { ACTION_SET_FILE_SIZE,   MSG_ACT_SET_FILE_SIZE,   3, PK_NEGATIVE },
        { ACTION_WRITE_PROTECT,   MSG_ACT_WRITE_PROTECT,   2, PK_BOOLEAN },
        { ACTION_READ_LINK,               MSG_ACT_READ_LINK,               4, PK_BOOLEAN },
        { ACTION_FH_FROM_LOCK,    MSG_ACT_FH_FROM_LOCK,    2, PK_BOOLEAN },
        { ACTION_IS_FILESYSTEM,   MSG_ACT_IS_FILESYSTEM,   0, PK_BOOLEAN },
        { ACTION_CHANGE_MODE,             MSG_ACT_CHANGE_MODE,     3, PK_BOOLEAN },
        { ACTION_COPY_DIR_FH,             MSG_ACT_COPY_DIR_FH,     1, PK_BOOLEAN },
        { ACTION_PARENT_FH,               MSG_ACT_PARENT_FH,               1, PK_BOOLEAN },
        { ACTION_EXAMINE_ALL,             MSG_ACT_EXAMINE_ALL,     5, PK_BOOLEAN },
        { ACTION_EXAMINE_FH,              MSG_ACT_EXAMINE_FH,              2, PK_BOOLEAN },
        { ACTION_EXAMINE_ALL_END, MSG_ACT_EXAMINE_ALL_END, 5, PK_BOOLEAN },
        { ACTION_SET_OWNER,               MSG_ACT_SET_OWNER,               4, PK_BOOLEAN },
        { ACTION_DOUBLE,                  MSG_ACT_DOUBLE,                  3, PK_BOOLEAN },
        { ACTION_FORCE,                   MSG_ACT_FORCE,                   3, PK_BOOLEAN },

/*
 * Where are these supposed to be defined?
 *
 *        { ACTION_STACK,                   MSG_ACT_STACK,                   3, PK_BOOLEAN },
 *        { ACTION_QUEUE,                   MSG_ACT_QUEUE,                   3, PK_BOOLEAN },
 */
        { ACTION_DROP,                    MSG_ACT_DROP,                    1, PK_BOOLEAN },
        { ACTION_LOCK_RECORD,             MSG_ACT_LOCK_RECORD,     5, PK_BOOLEAN },
        { ACTION_FREE_RECORD,             MSG_ACT_FREE_RECORD,     3, PK_BOOLEAN },
        { ACTION_ADD_NOTIFY,              MSG_ACT_ADD_NOTIFY,              1, PK_BOOLEAN },
        { ACTION_REMOVE_NOTIFY,   MSG_ACT_REMOVE_NOTIFY,   1, PK_BOOLEAN },
        { ACTION_SERIALIZE_DISK,  MSG_ACT_SERIALIZE_DISK,  0, PK_BOOLEAN },
        { ACTION_GET_DISK_FSSM,   MSG_ACT_GET_DISK_FSSM,   0, PK_BOOLEAN },
        { ACTION_FREE_DISK_FSSM,  MSG_ACT_FREE_DISK_FSSM,  0, PK_BOOLEAN },
        /*
         *              Our final table entry is used as a sentinel -- we key off the
         *              final packet message as an indication that we're finished
         *              searching the table.
         */
        { 0,                                              LAST_PACK_MSG,                   4, PK_2OK | PK_RAW }
};

#define NUM_PACKETS             (sizeof(PacketTable) / sizeof(PacketTable[0]))

/*****************************************************************************
 *
 *              Start of functions!
 *
 *****************************************************************************/

/*
 *              InitPatches()
 *
 *              Sets up the library pointers and makes sure all the patch
 *              code vectors are initialised properly.
 *
 *              Also allocates memory for the patches (if necessary)
 *
 *              Returns TRUE for success, FALSE for failure.
 */
int InitPatches(void)
{
        struct PatchInitTable *pi;
        Patch *p;
        int i;

        /*
         *              Before doing anything else, do a quick check to ensure that
         *              the patch code in PATCHCODE.S agrees with the equivalent patch
         *              structure defined in this file.
         */
        if (CODESIZE != PatchCode_Size || STACKADJUST != PatchCode_StackAdjust) {
                Printf("In patches.c, adjust CODESIZE by %ld and STACKADJUST by %ld\n",
                                PatchCode_Size-CODESIZE, PatchCode_StackAdjust-STACKADJUST);
                return (FALSE);
        }

        LibList[DOS_LIB]                = DOSBase;
        LibList[EXEC_LIB]               = SysBase;
        LibList[INTUITION_LIB]  = IntuitionBase;
        LibList[GRAPHICS_LIB]   = GfxBase;
        LibList[ICON_LIB]               = IconBase;

        /*
         *              First, let's calculate our ROM start and end address.
         *              We do this by getting the address pointed to by the
         *              Open LVO vector in exec.library and rounding it down
         *              to the nearest 512K
         */
        RomStart = (*((ULONG *)(((char *)SysBase) + LIB_OPEN + 2))) & 0xFFF80000;
        RomEnd   = RomStart + 0x7FFFF;

        /*
         *              Now see if the patches were already installed by an earlier
         *              version of SnoopDos.
         */
        Forbid();
        PatchAnchor = (PatchAnchorData *)FindSemaphore(PATCHES_NAME);
        if (!PatchAnchor) {
                /*
                 *              Patches weren't found, so allocate a new set and initialise
                 *              it with our patch code.
                 */
                PatchAnchor = AllocMem(sizeof(PatchAnchorData) + PatchInitSize,
                                                           MEMF_ANY | MEMF_CLEAR);
                if (!PatchAnchor) {
                        Permit();
                        return (FALSE);
                }
                strcpy(PatchAnchor->name, PATCHES_NAME);
                PatchAnchor->sem.ss_Link.ln_Name = PatchAnchor->name;
                PatchAnchor->sem.ss_Link.ln_Pri  = 0;
                AddSemaphore(&PatchAnchor->sem);
        }
        /*
         *              Now PatchAnchor points to our patch list in memory -- copy over
         *              our patch code and initialised data accordingly.
         */
        PatchList = PatchAnchor->patchdata;
        for (pi = PatchInit+1, p = PatchList+1; pi->offset; p++, pi++) {
                memcpy(p->code, PatchCode_Start, CODESIZE);
                p->library              = pi->library;
                p->offset               = pi->offset;
                p->newfunc              = pi->newfunc;
                p->stackneeded  = CurSettings.StackLimit;
                p->sysbase              = SysBase;
        }
        CacheClearU();  /* Invalidate cache for code we just built */
        Permit();

        /*
         *              Now initialise other variables associated with the patch code
         */
        SnoopTask       = SysBase->ThisTask;
        InputTask       = FindTask("input.device");
        InitRamLibPatch();
        InitSemaphore(&PatternBufSem);
        InitSemaphore(&PatternCacheSem);
        InitSemaphore(&TaskCacheSem);
        InitSemaphore(&DosDeviceSem);
        InitSemaphore(&PauseSem);
        InitSemaphore(&PacketSem);

        UpdateDeviceList(SCANDEV_IMMEDIATE);

        NewList(&PatternList);
        for (i = 0; i < NUM_PCACHE_ENTRIES; i++)
                AddHead(&PatternList, (Node *)&PCacheEntries[i]);

        NewList(&PacketWaitList);
        NewList(&PacketFreeList);
        for (i = 0; i < NUM_PACKET_ENTRIES; i++)
                AddHead(&PacketFreeList, (Node *)&PacketEntries[i]);

        NewEventSig = AllocSignal(-1);
        if (NewEventSig == -1)
                return (FALSE);
        NewEventMask = 1 << NewEventSig;

        /*
         *              Signal bit used to tell main task to rescan device list because
         *              a device was added (or removed?)
         */
        ScanDosListSig = AllocSignal(-1);
        if (ScanDosListSig == -1)
                return (FALSE);
        ScanDosListMask = 1 << ScanDosListSig;

        /*
         *              Now create a background process to handle process matching.
         *              We run this at a high priority to ensure quick response.
         *
         *              We first of all initialise our pattern port, and set it
         *              to NOT signal our task when a message arrives. This ensures
         *              that if, for some reason, our new process doesn't run
         *              until after someone tries to send it a message, nothing
         *              bad will happen (e.g. signalling a task at 0x0000)
         *
         *              The background process itself will replace this with
         *              PA_SIGNAL to make things work normally again.
         */
        NewList(&BackgroundPort.mp_MsgList);
        BackgroundPort.mp_Flags = PA_IGNORE;
        BackgroundProc = CreateNewProcTags(NP_Entry,     BackgroundProcCode,
                                                                           NP_Name,      BACKGROUND_NAME,
                                                                           NP_Priority,  5,
                                                                           TAG_DONE);

        return (BackgroundProc != NULL);
}

/*
 *              UpdatePatches()
 *
 *              Scans the list of patches, updating our behaviour as follows.
 *              This means:
 *
 *                      - Any patch that's currently active and which has been flagged
 *                        as inactive should be disabled if possible (i.e. origfunc
 *                        set to NULL).
 *
 *                      - Any patch that's currently inactive and which has been flagged
 *                        as active should be enabled if possible (i.e. origfunc set
 *                        to point to the original function).
 *
 *              If we are currently Disabled, then all patches are made inactive.
 *
 */
void UpdatePatches(void)
{
        Patch *p;

        if (!PatchList)
                return;

        if (MonPackets || ShowAllPackets)
                UpdateDeviceList(SCANDEV_IMMEDIATE); /* Keep device list up to date */

        Forbid();                       /* Don't even think about letting anyone else run! */

        /*
         *              Update status of pair functions to reflect the master
         *              function's status
         */
        PatchList[GID_READTOOLTYPES2].enabled   =
                                                        PatchList[GID_READTOOLTYPES].enabled;
        PatchList[GID_LOADSEG2].enabled                 =
                                                        PatchList[GID_LOADSEG].enabled;
        PatchList[GID_GETVAR2].enabled                  =
                                                        PatchList[GID_GETVAR].enabled;
        PatchList[GID_SETVAR2].enabled                  =
                                                        PatchList[GID_SETVAR].enabled;

        for (p = PatchList+1; p->offset; p++) {
                void *lib        = LibList[p->library];
                int func_enabled = (Disabled ? PATCH_DISABLED : p->enabled);

                p->stackneeded   = CurSettings.StackLimit;      /* Update this */

                if (!lib)                       /* Skip over any libraries we couldn't open */
                        continue;

                if (p->origfunc == NULL && func_enabled != PATCH_DISABLED) {
                        /*
                         *              Okay, we want to enable this function.
                         */
                        p->origfunc = (FuncPtr)SetFunction(lib, p->offset, (FuncPtr)p);
                } else if (p->origfunc && func_enabled == PATCH_DISABLED) {
                        /*
                         *              Okay, we want to disable this function. However, we
                         *              may not be able to do this cleanly, if it's been
                         *              since patched by someone else. Thus, if our attempt
                         *              to unpatch it doesn't return a pointer to our replacement
                         *              code, we have to leave the patch installed. However, it
                         *              will be re-tried automatically the next time someone
                         *              calls this function.
                         */
                        FuncPtr curfunc = (FuncPtr)SetFunction(lib, p->offset, p->origfunc);

                        if (curfunc != (FuncPtr)p) {
                                /*
                                 *              Uhoh -- someone else patched us!
                                 */
                                SetFunction(lib, p->offset, (FuncPtr)curfunc);
                        } else {
                                /*
                                 *              We unpatched successfully, so zero pointer
                                 */
                                p->origfunc = NULL;
                        }
                }
        }
        /*
         *              We disable stack monitoring for the PutMsg() patch because
         *              we need to catch the return packet from small-stack processes
         *              like RAM: etc. Instead, that patch does its own stack checking.
         */
        PatchList[GID_SENDREXX].stackneeded = 0;

        CacheClearU();  /* Just to be safe */
        Permit();
        /*
         *              Finally, check if we've disabled packet monitoring and
         *              there are some outstanding packets that have not yet had
         *              return values assigned. Free them all as appropriate
         */
        if (!MonPackets && !ShowAllPackets)
        {
                for (;;) {
                        WaitPacket *wp;
                        Event *ev;
                        LONG  seqnum;

                        ObtainSemaphore(&PacketSem);
                        if (IsListEmpty(&PacketWaitList)) {
                                ReleaseSemaphore(&PacketSem);
                                break;
                        }
                        wp         = (WaitPacket *)RemHead(&PacketWaitList);
                        AddHead(&PacketFreeList, (Node *)wp);
                        ev         = wp->event;
                        seqnum = wp->eventnum;
                        ReleaseSemaphore(&PacketSem);

                        ObtainSemaphore(&BufSem);
                        if (seqnum >= RealFirstSeq) {
                                ev->status = ES_READY;
                                Signal(SnoopTask, NewEventMask);
                        }
                        ReleaseSemaphore(&BufSem);
                }
        }
}

/*
 *              CleanupPatches()
 *
 *              Frees any resources allocated when initialising this module. Also
 *              disable any patches that are still active.
 */
void CleanupPatches(void)
{
        static int unpatchtries = 0;

        if (PatchList) {
                struct PatchInitTable *pi;
                Patch *p;

                for (p = PatchList+1; p->offset; p++)
                        p->enabled = PATCH_DISABLED;
                UpdatePatches();

                /*
                 *              Now our patches have been disabled, and it's time to
                 *              ensure nobody got left inside our code. We do this by
                 *              checking that all the usecounts are set to zero, and if
                 *              they're not, giving the user the option of continuing
                 *              to try, or exiting.
                 *
                 *              Before we do this, however, we record all the functions
                 *              which still have a usecount > 0 -- this way, if someone
                 *              else runs SnoopDos while we are checking and installs
                 *              a new set of patches in the patchtable, and enables
                 *              some additional functions, we don't have to check that
                 *              ALL usage counts are zero, only those which were non-zero
                 *              at the start of the loop.
                 *
                 *              (That is, we ignore any usage counts which are results of
                 *              functions being re-enabled by the new SnoopDos -- typically,
                 *              there will only be one or two functions at most to be
                 *              checked.)
                 *
                 *              For convenience, we re-use the PatchInit list to hold the
                 *              exit "usage" states for each function -- we won't be needing
                 *              it any more.
                 */
                for (p = (PatchList+1), pi = (PatchInit+1); p->offset; p++, pi++)
                        pi->offset = p->usecount;

                for (;;) {
                        for (p = (PatchList+1), pi = (PatchInit+1); p->offset; p++, pi++) {
                                if (pi->offset > 0) {
                                        if (p->usecount > 0)
                                                break;
                                        pi->offset = 0; /* No longer active, so mark as done */
                                }
                        }
                        if (p->offset) {
                                /*
                                 *              Couldn't unpatch, so go around the loop and try
                                 *              again. After about 8-10 seconds, we warn the user
                                 *              that we're still trying.
                                 */
                                unpatchtries++;
                                if (unpatchtries == 5) {
                                        char errorbuf[400];

                                        mysprintf(errorbuf, MSG(MSG_ERROR_UNPATCH),
                                                          GetFuncName(p - PatchList));
                                        ShowError(errorbuf);
                                }
                                Delay(2*50);            /* Wait 2 seconds before next try */
                        } else
                                break;
                }
        }
        if (BackgroundProc) {
                /*
                 *              Send a terminate message to the pattern process and wait
                 *              for it to respond
                 */
                PatternMsg pmsg;

                pmsg.type                = QUIT_MSG;
                pmsg.sigmask                     = SIGF_SINGLE;
                pmsg.task                                = SysBase->ThisTask;
                pmsg.msg.mn_Node.ln_Name = NULL;

                SetSignal(0, SIGF_SINGLE);              /* Ensure signal is clear first */
                PutMsg(&BackgroundPort, (struct Message *)&pmsg); /* Send quit message to process */
                Wait(SIGF_SINGLE);                              /* Wait for acknowledgement             */
        }
        if (NewEventSig    != -1) FreeSignal(NewEventSig),    NewEventSig    = -1;
        if (ScanDosListSig != -1) FreeSignal(ScanDosListSig), ScanDosListSig = -1;
}

/*
 *              InitRamLibPatch()
 *
 *              Attempts to patch the ramlib process so that it no longer uses
 *              a message port with the signal bit set to SIGB_SINGLE (which
 *              is asking for trouble if you call any functions that use
 *              semaphores, since a SIGB_SINGLE can be generated by a message
 *              arriving at the message port while waiting for the semaphore,
 *              causing all hell to break loose -- the semaphore appears to be
 *              obtained while in fact it's still in use by someone else).
 */
void InitRamLibPatch(void)
{
        unsigned char *ramdata;
        int i;

        RealRamLibTask = FindTask("ramlib");
        if (NoPatchRamLib || !RealRamLibTask) {
                /*
                 *              Not patching ramlib, so instead, install our bypass patch
                 *              that stops us from monitoring any ramlib tasks so that
                 *              we won't get crashed.
                 */
                RamLibTask = RealRamLibTask;
        } else {
                /*
                 *              Patch ramlib itself so that it doesn't use SIGF_SINGLE
                 *              any more.
                 */
                ramdata = (void *)SysBase->ex_RamLibPrivate;
                if (!ramdata) {
                        /*
                         *              You never know ... this field may no longer exist. If
                         *              it doesn't, then assume the bug no longer exists either
                         *              and exit.
                         */
                        return;
                }
                for (i = 0x20; i < 0x50; i += 2) {
                        struct MsgPort *ramport = (struct MsgPort *)(ramdata + i);

                        /*
                         *              Depending on the Kickstart version (V37 through V40)
                         *              the ramlib message port lies somewhere in the range
                         *              0x30 to 0x50 of the ramlib's private area. We do
                         *              some integrity checks on the port to make sure it's
                         *              not just a lucky hit. If some future SetPatch fixes
                         *              this, then we will never find a match, which suits
                         *              us fine.
                         */
                        if (ramport->mp_Flags   == 0                    &&
                                ramport->mp_SigBit  == SIGB_SINGLE      &&
                                ramport->mp_SigTask == RealRamLibTask)
                        {
                                /*
                                 *              Now patch ramlib's message port so it won't use
                                 *              SIGB_SINGLE any more. This is a little tricky, since
                                 *              ramlib process will be in the middle of WaitPort()
                                 *              and WaitPort() won't return until a message arrives at
                                 *              the port. Essentially, we can only change the port's
                                 *              message bit while we are sure that the ramlib process
                                 *              is NOT in a WaitPort(). We do this by bumping our
                                 *              process priority up to 127 temporarily, making the
                                 *              call, and then restoring it.
                                 *
                                 *              Since we're at a higher priority than ramlib, then
                                 *              as soon as ramlib responds to the message, it will
                                 *              be switched out (in the Ready state) before it has a
                                 *              chance to call WaitPort() and go back to sleep again.
                                 */
                                ULONG oldpri;

                                Forbid();
                                oldpri = SetTaskPri(SysBase->ThisTask, 127);
                                OpenLibrary("ramlib-patch.library", 0); /* Wake-up ramlib */
                                ramport->mp_SigBit = SIGBREAKB_CTRL_E;
                                SetTaskPri(SysBase->ThisTask, oldpri);
                                Permit();
                                break;
                        }
                }
        }
}

/*
 *              UpdateDeviceList(method)
 *
 *              Updates the DOS device list. This is used to keep an internal list
 *              of all the task IDs of DOS handlers so we can quickly determine
 *              if a given ID belongs to a handler or not.
 *
 *              This should be called at least once, and ideally every now and
 *              again, so that new devices added to the DOS list can be detected
 *              and old ones ignored.
 *
 *              The method can be SCANDEV_IMMEDIATE, to immediately scan the device
 *              list, or SCANDEV_DELAY to wait for a second before updating. The
 *              delay is because the patch that signals us to rescan (AddDosEntry)
 *              happens before the device is actually fully initialised, so we need
 *              to give it a little extra time to get going.
 *
 *              It's not critical if we don't give it enough time, it just means
 *              that the new device won't appear in the list after all -- a bit
 *              annoying for people using the packet debugger to help debug a
 *              device they keep mounting interactively.
 */
void UpdateDeviceList(int method)
{
        struct DosList *dlist;
        int i = 0;

        if (method == SCANDEV_DELAY)
                Delay(50);

        ObtainSemaphore(&DosDeviceSem);
        dlist = LockDosList(LDF_DEVICES | LDF_READ);
        while ((dlist = NextDosEntry(dlist, LDF_DEVICES))
                         && i < (MAX_DOS_DEVICES-1))
        {
#ifdef __AROS__
	#warning Add AROS specific code for DosList access
#else
                if (dlist->dol_Task)
                        DeviceTaskList[i++] = dlist->dol_Task->mp_SigTask;
#endif
        }
        UnLockDosList(LDF_DEVICES | LDF_READ);
        DeviceTaskList[i] = 0;
#if 0
        Printf("New device list:\n");
        for (i = 0; DeviceTaskList[i]; i++)
                Printf("    %2ld = %s\n", i, DeviceTaskList[i]->tc_Node.ln_Name);
#endif
        ReleaseSemaphore(&DosDeviceSem);
}

/*
 *              SetPattern(string, ignorewb)
 *
 *              Sets the current pattern to the given string (possibly containing
 *              wildcards). If ignorewb is true, then the pattern is modified to
 *              also exclude any processes with the names Workbench, Shell Process
 *              or Background CLI.
 */
void SetPattern(char *pattern, int ignorewb)
{
        PatternCache *pc;
        char *p;

        if (*pattern == '\0' && !ignorewb) {
                PatternEnabled = 0;
                return;
        }
        ObtainSemaphore(&PatternCacheSem);
        ObtainSemaphore(&PatternBufSem);
        PatternEnabled = 0;

        /*
         *              Now, update our pattern string. If the pattern string is empty,
         *              we disable patterns. If the pattern string contains no wildcards,
         *              we enable patterns but disable wildcard patching (this is much
         *              faster, since it can be done in the patch's task, rather than
         *              having to call back to the main routine).
         *
         *              We allocate both pattern semaphores: the Cache semaphore to
         *              ensure that no pattern code can execute when PatternEnabled
         *              is zero, and the buffer semaphore to ensure that the pattern
         *              process doesn't try to do pattern matching while we are
         *              building the new pattern string.
         *
         *              We need to be careful to get the cache semaphore BEFORE we get
         *              the pattern buffer semaphore, otherwise we could deadlock if
         *              a patch decided to check a pattern at just the wrong time.
         */
        if (ignorewb) {
                if (*pattern == '\0')
                        strcpy(PatternString, PAT_EASY_EXCLUDE);
                else
                        mysprintf(PatternString, PAT_COMPLEX_EXCLUDE, pattern);
        } else
                strcpy(PatternString, pattern);

        /*
         *              Now, due to a bug in AmigaDOS's ParsePatternNoCase, we
         *              have to convert the string to upper case first. (The bug
         *              is that character classes like [a-z] will never match anything,
         *              only [A-Z] will work. Since it's case insensitive for
         *              everything else, we simply convert everything to upper case.)
         */
        for (p = PatternString; *p; p++) {
                if (*p >= 'a' && *p <= 'z')
                        *p &= 0x5F;
        }
        PatternWildcard = ParsePatternNoCase(PatternString,
                                                                                 PatternBuf, PAT_BUF_LEN);

        if (PatternWildcard != -1)      /* Only enable patterns if successful */
                PatternEnabled = 1;

        /*
         *              Regardless of what happened, invalidate the pattern cache
         *              to force a new match for each task name.
         */
        FORLIST(&PatternList, pc)
                pc->task = NULL;

        ReleaseSemaphore(&PatternBufSem);
        ReleaseSemaphore(&PatternCacheSem);
}

/*
 *              FlushWaitingPackets(void)
 *
 *              Flushes any packets currently in the waiting packets queue
 *              (marking them as missed). Usually called after packet monitoring
 *              has been disabled, in case there were any half-completed packets
 *              in the queue.
 */
void FlushWaitingPackets(void)
{
        WaitPacket *wp;
        int flushed = 0;

        ObtainSemaphore(&PacketSem);
        wp = HeadNode(&PacketWaitList);
        while (NextNode(wp)) {
                ULONG seqnum = wp->eventnum;
                Event *ev    = wp->event;

                Remove((Node *)wp);
                AddHead(&PacketFreeList, (Node *)wp);
                ReleaseSemaphore(&PacketSem);   /* Never lock two sem's at once! */
                ObtainSemaphore(&BufSem);
                if (seqnum >= RealFirstSeq) {
                        ev->result = MSG(MSG_RES_MISSED);
                        ev->status = ES_READY;
                        flushed    = 1;
                }
                ReleaseSemaphore(&BufSem);
                ObtainSemaphore(&PacketSem);
                wp = HeadNode(&PacketWaitList);
        }
        ReleaseSemaphore(&PacketSem);

        if (flushed)
                Signal(SnoopTask, NewEventMask);
}

/*
 *              LoadFuncSettings(FuncSets)
 *
 *              Downloads the values in the passed-in FuncSettings array to the
 *              various local structures that require them.
 */
void LoadFuncSettings(FuncSettings *func)
{
        Patch *p;
        int i;

        if (!PatchList)
                return;

        Forbid();
        for (p = PatchList+1, i = 1; p->offset; p++, i++)
                p->enabled = ((func->Opts[i] && !Disabled) ?
                                                        PATCH_ENABLED : PATCH_DISABLED);
        /*
         *              A bit of a hack -- the PutMsg() function is needed to monitor
         *              three individual patches: SendRexx, Monitor Packets, and
         *              Packet Debugger. We do the check for the last two here.
         */
        if ((func->Opts[GID_MONPACKETS] || func->Opts[GID_MONALLPACKETS]) &&
             !Disabled)
        {
                PatchList[GID_SENDREXX].enabled    = PATCH_ENABLED;
                PatchList[GID_ADDDOSENTRY].enabled = PATCH_ENABLED;
        } else {
                PatchList[GID_ADDDOSENTRY].enabled = PATCH_DISABLED;
                FlushWaitingPackets();
        }
        UpdatePatches();
        Permit();
        SetPattern(func->Pattern, func->Opts[GID_IGNOREWB]);
}

/*
 *              BackgroundProcCode()
 *
 *              This function is called as a separate process. It is used
 *              to service requests for pattern matching by patched code.
 *              We do this in a separate process to ensure we have enough
 *              stack to perform the pattern matching (pattern matching can
 *              be very stack-intensive.)
 *
 *              This process is also used to do pathname expansion when we
 *              are monitoring packets -- this is needed for both the
 *              ShowAllPaths option and monitoring of the ACTION_MAKE_LINK packet.
 *
 *              We keep going until we get a QUIT_MSG from the main task,
 *              at which point we exit.
 */
void SAVEDS BackgroundProcCode(void)
{
#if DEBUG_PATTERN
        BPTR dbfile = Open("CON:100/100/400/100/Pattern/WAIT/AUTO/CLOSE",
                                           MODE_OLDFILE);
#define DBP(s)  FPrintf s
#else
#define DBP(s)
#endif

        Task  *quittask;
        ULONG  quitsig;
        int    done = 0;

        /*
         *              Our first task is to re-route the message port allocated
         *              on our behalf by the main process so that it points to us.
         *              We also re-enable signals on incoming messages to ensure
         *              we are correctly interrupted.
         */
        BackgroundPort.mp_SigTask = SysBase->ThisTask;
        BackgroundPort.mp_SigBit  = AllocSignal(-1);    /* Better not fail!             */
        BackgroundPort.mp_Flags   = PA_SIGNAL;                  /* Allow signalling now */
        DBP((dbfile, "Started pattern debug code\n"));

        while (!done) {
                /*              Now handle all pattern messages queue on the pattern port.
                 *              Unlike normal messages, we don't simply reply to these
                 *              with ReplyMsg(); instead, we signal the indicated task
                 *              using the signal bit specified in the message.
                 *
                 *              This is because the sender may not have an available
                 *              signal bit to use to receive the reply so it uses a
                 *              system bit which is guaranteed to be unused.
                 *
                 *              Note that we grab the pattern semapore while we process
                 *              the messages; this stops the mainline code from updating
                 *              the middle of the buffer while we are doing a comparison.
                 */
                PatternMsg      *pmsg;
                NameFromLockMsg *lmsg;

                WaitPort(&BackgroundPort);

                ObtainSemaphore(&PatternBufSem);
                while ((pmsg = (void *)GetMsg(&BackgroundPort)) != NULL) {
                        DBP((dbfile, "Got a message, type = %ld!\n", pmsg->type));
                        switch (pmsg->type) {

                                case QUIT_MSG:
                                        /*
                                         *              We've been told to quit by the main task, so
                                         *              set some variables appropriately. We flush
                                         *              the remaining messages in the queue first
                                         *              just to be safe (we wouldn't want to leave
                                         *              anyone hanging, now, would we?)
                                         */
                                        PatternEnabled  = 0;    /* Safety first */
                                        done                    = 1;
                                        quittask                = pmsg->task;
                                        quitsig                 = pmsg->sigmask;
                                        break;

                                case PATTERN_MSG:
                                        DBP((dbfile, "Got a pmatch from %s\n", pmsg->name));
                                        if (PatternEnabled)
                                                pmsg->match = MatchPatternNoCase(PatternBuf,
                                                                                                                 pmsg->name);
                                        else
                                                pmsg->match = 1; /* If disabled, everything matches */

                                        Signal(pmsg->task, pmsg->sigmask);
                                        break;

                                case PATHEXPAND_MSG:
                                        lmsg         = (NameFromLockMsg *)pmsg;
                                        lmsg->newbuf = MyNameFromLock(lmsg->lock, lmsg->filename,
                                                                                                  lmsg->buf,  lmsg->maxlen);
                                        Signal(lmsg->task, lmsg->sigmask);
                                        break;
                        }
                }
                ReleaseSemaphore(&PatternBufSem);
        }

#if DEBUG_PATTERN
        DBP((dbfile, "Quitting...\n"));
        Close(dbfile);
#endif

        /*
         *              Now cleanup and exit from this task. We need to Forbid()
         *              before we signal the main task that we're ready to quit,
         *              since otherwise the main task might go ahead and unload
         *              us before we have a chance to execute the last few lines
         *              in the function (e.g. if the main task is running at a
         *              higher priority than we are).
         */
        Forbid();
        FreeSignal(BackgroundPort.mp_SigBit);
        Signal(quittask, quitsig);
}

/*
 *              CheckPattern(name, namelen)
 *
 *              Checks if our task's name has been masked out by the
 *              user, using our current AmigaDOS pattern string.
 *
 *              We can't call the DOS pattern match function directly, due to
 *              stack limitations (the caller may not have a very big stack).
 *              So, we maintain a cache of recently used tasks -- the first
 *              task on the list is always the most recently monitored task.
 *
 *              If a task doesn't appear on the list, or if it appears on the
 *              list but its name has changed, then we send a message to the
 *              main SnoopDos task asking it to check the pattern. We also
 *              add the result of this check to our cache for future calls.
 *
 *              Sending a message to another task from within a patch is tricky;
 *              more specifically, receiving the acknowledgement back from the
 *              main task is tricky. This is because we don't know what signal
 *              bits the caller might be using, and so we can't just grab one.
 *              We could use AllocSignal(), but then what if the caller has no
 *              free signal bits?
 *
 *              Our solution (not necessarily the best) is to use the reserved
 *              exec signal bit SIGB_SINGLE. This is used for semaphore operations,
 *              among others, and is only ever used while waiting for a single
 *              event to happen. Thus, when our code is reached, we know this
 *              bit is not in use and thus we can use it ourselves.
 *
 *              Returns true if no pattern is set, or if the pattern matches
 *              the current task name. Returns false otherwise.
 *
 */
int CheckPattern(char *taskname, int namelen)
{
        PatternCache *pc;
        Task *thistask = SysBase->ThisTask;
        PatternMsg pmsg;

        if (!PatternEnabled)
                return (TRUE);

        if (!PatternWildcard)
                return (stricmp(taskname, PatternString) == 0);

        namelen = MIN(namelen, PC_NAMELEN-1);

        /*
         *              Got a pattern, so search our cache list
         */
        ObtainSemaphore(&PatternCacheSem);
        FORLIST(&PatternList, pc)
                if (pc->task == thistask)
                        break;

        /*
         *              Okay, now pc is either the matching task, or else a pointer
         *              to the end of our list.
         */
        if (NextNode(pc) && pc->task == thistask) {
                /*
                 *              We broke out of the loop since we matched the task.
                 *              Now check if the name matches too.
                 */
                if (strncmp(pc->name, taskname, namelen) == 0) {
                        /*
                         *              We're the same. Move this node to the start of
                         *              the list, and return the appropriate state.
                         */
                        Remove((Node *)pc);
                        AddHead(&PatternList, (Node *)pc);
                        ReleaseSemaphore(&PatternCacheSem);
                        return (pc->match);
                }
                /*
                 *              The name is different but the Task ID is the same --
                 *              most likely, someone ran a new CLI command. Thus, use
                 *              this cache entry for our new task name and fall through.
                 */
        } else {
                /*
                 *              We didn't match a task so grab one from the end of the
                 *              list and use that instead.
                 */
                pc               = TailNode(&PatternList);
                pc->task = thistask;
        }

        /*
         *              Move the node to the start of the list to stop it being flushed
         *              any time soon, and update the name to match our own name.
         */
        Remove((Node *)pc);
        AddHead(&PatternList, (Node *)pc);
        strncpy(pc->name, taskname, namelen);
        pc->name[namelen] = 0;
        ReleaseSemaphore(&PatternCacheSem);

        /*
         *              Now send a message to the main task asking it to perform a name
         *              match for us. We need to be careful to make sure our pmsg won't
         *              be confused with a DOS packet by our PutMsg() patch -- we do
         *              this by NULLing out the name field.
         */
        pmsg.type                = PATTERN_MSG;
        pmsg.name                                = taskname;
        pmsg.sigmask                     = SIGF_SINGLE;
        pmsg.task                                = thistask;
        pmsg.msg.mn_Node.ln_Name = NULL;

        SetSignal(0, SIGF_SINGLE);              /* Ensure signal is clear first         */
        PutMsg(&BackgroundPort, (struct Message *)&pmsg); /* Send request to background task      */
        Wait(SIGF_SINGLE);                              /* Wait for acknowledgement                     */
        pc->match = pmsg.match;                 /* And save result in cache                     */

        return (pmsg.match);
}

/*
 *              GetVolName(lock, buf, maxlen)
 *
 *              Copies the volume name associated with lock into the buffer,
 *              with terminating ':'. If lock is NULL, the volume address is
 *              taken directly from volume.
 *
 *              If UseDevNames is true, the device list is searched looking
 *              for the device node associated with the volume node (i.e. two
 *              nodes sharing the same task address).
 *
 *              WARNING: This function must not be called from within a DOS
 *                               device handler due to potential deadlock errors!
 *
 */
void GetVolName(BPTR lock, char *buf, int maxlen)
{
        struct DeviceList *vol;
        struct DosList *dl;
        int gotdev = 0;

        if (lock == NULL) {
                NameFromLock(lock, buf, maxlen);
                return;
        }
#ifdef __AROS__
        vol = BTOC(lock);
#else
        vol = BTOC(((struct FileLock *)BTOC(lock))->fl_Volume);
#endif

#warning Add AROS specific code for DosList access
        if (UseDevNames == 0 /* || vol->dl_Task == NULL */ ) {
                /*
                 *              Use volume name, especially if the volume isn't currently
                 *              mounted!
                 */
				UBYTE *volname;
				int len;
#ifdef __AROS__
				volname = vol->dl_DevName;
				len = MIN(maxlen-2,strlen(volname));
#else
                volname = BTOC(vol->dl_Name);
                len = MIN(maxlen-2, *volname);
#endif
                memcpy(buf, volname+1, len);
                buf[len++] = ':';
                buf[len] = '\0';
                return;
        }

        /*
         *              The user wants the device name. The only way to obtain this
         *              is to search the device list looking for the device node with
         *              the same task address as this volume.
         */
        dl = LockDosList(LDF_DEVICES | LDF_READ);
        while ((dl = NextDosEntry(dl, LDF_DEVICES))) {
#warning Add AROS specific code for DosList access
#if 0
                if (dl->dol_Task == vol->dl_Task) {
                        /*
                         *              Found our task, so now copy device name
                         */
                        UBYTE *devname = BTOC(dl->dol_Name);
                        int len = MIN(maxlen-2, *devname);

                        memcpy(buf, devname+1, len);
                        buf[len++] = ':';
                        buf[len] = '\0';
                        gotdev = 1;
                        break;
                }
#endif
        }
        UnLockDosList(LDF_DEVICES | LDF_READ);
        if (!gotdev)
                strcpy(buf, "???:");
}

/*
 *              MyNameFromLock(lock, filename, buf, maxlen)
 *
 *              This is a custom version of the DOS function NameFromLock()
 *              which expands a disk lock into a full path.
 *
 *              Our version adds the following features. The device name will be
 *              given as either the physical device (like DH0:) if UseDevNames
 *              is true, or the volume name (like System3.0:) if UseDevNames is
 *              false.
 *
 *              If filename is non-NULL, then it will be appended to the lock
 *              path. If filename contains path info, then this will be taken
 *              into account when generating the lock, so that an absolute path
 *              in filename will not have any effect, and a relative filename
 *              (like //directory) will cause the original lock to be ParentDir()'d
 *              twice before being resolved.
 *
 *              This function can't fail. In the event of an error (string too
 *              long, or something like that), the buffer will be filled accordingly.
 *              It is assumed that the buffer will always be big enough to hold short
 *              error messages or a volume name.
 *
 *              Returns a pointer to the path (which will not necessarily be at
 *              the start of the buffer, but is guaranteed null-terminated.)
 *              Note that it's even possible that the pointer returned will be
 *              to the original filename if no path expansion was required.
 *
 *              New: We now preserve the IoErr() that was present on entry, since
 *              it may have been set by the calling function if OnlyShowFails is
 *              true. Otherwise, IoErr() will be screwed up by the operations we
 *              do here (e.g. SAS/C deleting a non-existent file when OnlyShowFails
 *              is true).
 *
 *              WARNING: This function must not be called from within a DOS
 *                               device handler due to potential deadlock errors!
 */
char *MyNameFromLock(BPTR lock, char *filename, char *buf, int maxlen)
{
        Process *myproc = (Process *)SysBase->ThisTask;
        int pos = maxlen - 1;
        D_S(fib, struct FileInfoBlock);
        LONG savedioerr = IoErr();
        BPTR curlock;
        BPTR newlock;
        void *savewinptr;
        char *p;
        int len;
        int skipfirstslash = 0; /* If true, skip first slash when building name */
        int err = 0;

        /*
         *              Check for special magic filehandle
         */
        if (filename && *filename) {
                if (strcmp(filename, "*") == 0)
                        return (filename);

                /*
                 *              First determine if we have any work to do.
                 */
                if (*filename == ':') {
                        /*
                         *              Got a reference relative to the root directory. Simply
                         *              grab the volume (or device) name from the lock and go
                         *              with that.
                         */
                        int len;

                        GetVolName(lock, buf, maxlen);
                        len = strlen(buf);
                        strncat(buf+len, filename+1, maxlen-len);
                        buf[maxlen-1] = '\0';
                        SetIoErr(savedioerr);
                        return (buf);
                }
                for (p = filename; *p; p++) {
                        if ((*p == ':'))                  /* If absolute path name, leave it alone */
                                return (filename);
                }
        } else {
                /*
                 *              Filename is null, so indicate we want to skip the first
                 *              slash when building the directory path
                 */
                skipfirstslash = 1;
        }

        savewinptr = myproc->pr_WindowPtr;
        myproc->pr_WindowPtr = (APTR)-1;                /* Disable error requesters       */

        newlock = DupLock(lock);
        if (lock && !newlock) {
                GetVolName(lock, buf, 20);
                if (filename) {
                        strcat(buf, ".../");
                        strcat(buf, filename);
                }
                myproc->pr_WindowPtr = savewinptr;      /* Re-enable error requesters */
                SetIoErr(savedioerr);
                return (buf);
        }
        buf[pos] = '\0';
        curlock = newlock;
        if (filename) {
                while (newlock && *filename == '/') {
                        /*
                         *              Handle leading /'s by moving back a directory level
                         *              but nothing else
                         */
                        newlock = ParentDir(curlock);
                        if (newlock) {
                                UnLock(curlock);
                                curlock = newlock;
                                filename++;
                        }
                }
                len = strlen(filename);
                if (len > (pos-2)) {
                        memcpy(buf+2, filename+len-pos, pos-2);
                        buf[0] = buf[1] = '.';
                        pos = 0;
                        UnLock(curlock);
                } else {
                        pos -= len;
                        memcpy(buf+pos, filename, len);
                }
        }

        /*
         *              At this point, we have buf containing the filename (minus any
         *              leading /'s), starting at the index given by pos. If filename
         *              was NULL or empty, then pos indexes to a \0 terminator.
         *
         *              Next, we want to pre-pend directory names to the front of
         *              the filename (assuming there _is_ a filename) until we get
         *              to the device root.
         */
        newlock = curlock;
        while (newlock) {

                if (!Examine(curlock, fib)) {
                        err++;
                        break;
                }
                len = strlen(fib->fib_FileName);
                if (len > (pos-3)) {
                        /*
                         *              Not enough room: prefix dots at start to indicate
                         *              an overrun. We use pos-3 since we need one char
                         *              for a possible slash and two more to accomodate a
                         *              leading ".."
                         */
                        memcpy(buf+2, fib->fib_FileName+len-pos+3, pos-2);
                        buf[0] = buf[1] = '.';
                        buf[pos-1] = '/';
                        pos = 0;
                        break;
                }
                newlock = ParentDir(curlock);
                if (newlock) {
                        UnLock(curlock);
                        curlock = newlock;
                        pos -= len + 1;
                        memcpy(buf + pos, fib->fib_FileName, len);
                        if (skipfirstslash) {
                                skipfirstslash = 0;
                                buf[pos+len] = '\0';
                        } else
                                buf[pos+len] = '/';
                }
        }
        /*
         *              Now we've built the path components; add the volume node
         *              to the beginning if possible.
         */
        if (err) {
                /*
                 *              If an error occurred, the volume is probably not mounted,
                 *              so we include a ".../" component in the path to show
                 *              we couldn't get all the info
                 */
                pos -= 4;
                memcpy(buf + pos, ".../", 4);
        }
        if (pos > 0) {
                char volname[20];
                int len;
                char *p;

                GetVolName(curlock, volname, 20);
                len = strlen(volname);
                if (len > pos) {
                        p = volname + len - pos;
                        len = pos;
                } else {
                        p = volname;
                }
                pos -= len;
                memcpy(buf + pos, p, len);
        }
        if (curlock)
                UnLock(curlock);

        myproc->pr_WindowPtr = savewinptr;              /* Re-enable error requesters */
        SetIoErr(savedioerr);
        return (buf+pos);
}

/*
 *              AsyncNameFromLock(volname, lock, filename, dest, maxlen)
 *
 *              This is identical to the MyNameFromLock() function except that
 *              it gets the background SnoopDos process to do the path expansion
 *              instead of doinng it from within the caller's process. This is
 *              necessary if we need to expand paths and we are not sure if the
 *              caller's process mesage port is free.
 *
 *              Volname is the volume on which the lock resides. If the lock itself
 *              is NULL, then the root of the volume will be assumed and this
 *              string will be copied into the buffer as the volume name.
 *
 *              WARNING: This function must not be called from within a DOS
 *                               device handler due to potential deadlock errors!
 */
char *AsyncNameFromLock(char *volname, BPTR lock, char *filename,
                                                char *buf, int maxlen)
{
        NameFromLockMsg lmsg;

        if (lock == NULL) {
                /*
                 *              Do a quick scan on the filename itself to see
                 *              if it contains a colon prefixed by some text;
                 *              if it does, then we already have a full path.
                 *              If it's just a colon, prefix it with the
                 *              volume name. If it has no colon, prefix it
                 *              with the volume name and a colon.
                 */
                char *p;

                strcpy(buf, volname);
                if (filename) {
                        if (*filename == ':') {
                                strcat(buf, filename);
                        } else {
                                for (p = filename; *p && *p != ':'; p++)
                                        ;
                                if (*p)
                                        strcpy(buf, filename);
                                else
                                        mysprintf(buf, "%s:%s", volname, filename);
                        }
                }
                return (buf);
        }
        lmsg.type               = PATHEXPAND_MSG;
        lmsg.lock               = lock;
        lmsg.filename   = filename;
        lmsg.buf                = buf;
        lmsg.maxlen             = maxlen;
        lmsg.sigmask    = SIGF_SINGLE;
        lmsg.task               = SysBase->ThisTask;
        lmsg.msg.mn_Node.ln_Name = NULL;

        SetSignal(0, SIGF_SINGLE);              /* Ensure signal is clear first         */
        PutMsg(&BackgroundPort, (struct Message *)&lmsg); /* Send request to background task      */
        Wait(SIGF_SINGLE);                              /* Wait for acknowledgement                     */
        return (lmsg.newbuf);
}

#if STACK_CHECK
/*
 *              GetFreeStack()
 *
 *              Returns the amount of free space available on the calling task's
 *              stack (thanks to Ralph Babel for this).
 */
ULONG GetFreeStack(void)
{
        Process *pr = (Process *)SysBase->ThisTask;
        char *upper;
        char *lower;
        ULONG total;
        ULONG avail;

        if (pr->pr_Task.tc_Node.ln_Type == NT_PROCESS && pr->pr_CLI != NULL) {
                upper = (char *)pr->pr_ReturnAddr + sizeof(ULONG);
                total = *(ULONG *)pr->pr_ReturnAddr;
                lower = upper - total;
        } else {
                upper = (char *)pr->pr_Task.tc_SPUpper;
                lower = (char *)pr->pr_Task.tc_SPLower;
                total = upper - lower;
        }
        avail = (char *)getreg(REG_A7) - lower;
        return (avail);
}
#endif

/*
 *              CreateEvent(calladdr, seqnum, action, filename,
 *                                      options, expandname [, lock] )
 *
 *              Allocates a new event for the current task and initialises
 *              almost all the fields in it accordingly.
 *
 *              seqnum is initialised to the sequence number of the new event. This
 *              is used to allow later detection of events which have scrolled off
 *              the top of the buffer.
 *
 *              filename and options are text strings, and actionid is a message ID.
 *              The storage for these will be allocated in the event buffer.
 *
 *              If expandname is EXPAND_NAME, then 'filename' is assumed to represent
 *              a real disk filename, and it will be expanded to a full path if that
 *              option is set. If expandname is NO_EXPAND, then the name is passed
 *              unchanged.
 *
 *              If expandname is neither EXPAND_NAME or NO_EXPAND, then it is
 *              interpreted as a volume name, and one additional parameter
 *              following it will be expected -- this additional parameter will
 *              be a lock on that volume. The filename will then be expanded
 *              to a full pathname on that volume. If the lock is null, then it
 *              is assumed to represent the root of the volume. The filename
 *              expansion is done by the background process, making it safe to
 *              call from within the PutMsg() patch which monitors raw packets.
 *
 *              If you pass in a NULL value for any of the three pointers, it will
 *              be made to point to a blank string.
 *
 *              If for some reason the event could not be created, returns NULL,
 *              else returns a pointer to the initialised event.
 *
 *              Possible reasons for failure include:
 *
 *                      - Calling task is main SnoopDos task (!)
 *                      - Calling task is not included in the match pattern
 *                      - Calling task was called by a ROM function
 *                      - Disk error occurred expanding filename
 *                      - Out of memory allocating new event
 */
struct Event *CreateEvent(ULONG calladdr, LONG *seqnum, ULONG actionid,
                                                  char *filename, char *options, int expandname, ...)
{
        Task *thistask = SysBase->ThisTask;
        struct CommandLineInterface *cli;
        Event *ev;
        char *procname;
        char *strptr;
        char *action;
        char namebuf[MAX_SHORT_LEN+1];
        char pathbuf[MAX_STR_LEN+1];
        int plen = 0;
        int flen = 0;
        int olen = 0;
        int alen = 0;
        int slen = 0;
        int clinum;

        /*
         *              We ignore all calls from the main SnoopDOS process and
         *              background process, and also any nested calls made while
         *              we have the semaphore locked (which might otherwise lead
         *              to nasty trashing of the list!)
         *
         *              Also, any calls from input.device are ignored since these
         *              typically call us with layers locked, and the slightest
         *              delay could be fatal.
         */
        if (thistask == SnoopTask || thistask == (Task *)BackgroundProc
                                                          || thistask == BufSem.ss_Owner
                                                          || thistask == RamLibTask
                                                          || thistask == InputTask)
                return (NULL);

        /*
         *              We also ignore any calls made from ROM addresses (if this
         *              option is enabled), since this cuts down the amount of
         *              superfluos output considerably.
         *
         *              We specifically DO allow calls from within ramlib, since
         *              it's very easy to disable ramlib directly if need be (by
         *              putting ~ramlib in the match name) and ramlib does
         *              many useful operations that are convenient to monitor.
         */
        if (thistask != RealRamLibTask && !MonROMCalls &&
                                                          calladdr >= RomStart && calladdr <= RomEnd)
                return (NULL);

        /*
         *              Work out the correct name for our task. This will either be
         *              the task name, the process name, or the current CLI command
         *              (if the process is a CLI, _and_ is currently running a command.)
         *
         *              In addition, for CLIs, we set clinum to the CLI number. For
         *              tasks and non-CLI processes, clinum becomes zero.
         */
        procname = thistask->tc_Node.ln_Name;
        plen     = strlen(procname);
        cli              = BTOC(((Process *)thistask)->pr_CLI);
        clinum   = 0;

        if (thistask->tc_Node.ln_Type == NT_PROCESS && cli) {
                /*
                 *              If we have a CLI command, we have to convert the name
                 *              from a BSTR into a C string
                 */
                clinum = ((Process *)thistask)->pr_TaskNum;
                if (cli->cli_Module) {
                        UBYTE *cliname = BTOC(cli->cli_CommandName);
                        plen = *cliname;

                        if (plen) {
                                if (plen > MAX_SHORT_LEN)
                                        plen = MAX_SHORT_LEN;
                                memcpy(namebuf, cliname+1, plen);
                                namebuf[plen] = 0;
                        } else {
                                strcpy(namebuf, "<Unknown>");
                                plen = strlen(namebuf);
                        }
                        procname = namebuf;
                }
        }

#if 0
        if (stricmp(procname, "ramlib") == 0) {
                KPrintF("Ramlib: Stacksize is %ld\n", GetFreeStack());
        }
#endif

        if (!CheckPattern(procname, plen))              /* Perform pattern check */
                return (FALSE);

        /*
         *              Now deal with our three strings
         */
        if (ShowPaths && filename != NULL) {
                if (expandname == EXPAND_NAME) {
                        /*
                         *              Expand filename to full path
                         */
                        filename = MyNameFromLock(((Process *)thistask)->pr_CurrentDir,
                                                                          filename, pathbuf, MAX_STR_LEN);
                } else if (expandname != NO_EXPAND) {
                        /*
                         *              Expand filename to full path using background process
                         *              and the lock specified in expandname.
                         */
                        BPTR lock = (BPTR)(*(ULONG *)(&expandname+1));

                        filename = AsyncNameFromLock((char *)expandname, lock, filename,
                                                                             pathbuf, MAX_STR_LEN);
                }
        }
        plen++;
        if (ShowCLINum && clinum)
                plen += 8;                                              /* Leave room for CLI display */

        action = MSG(actionid);
        alen   = strlen(action) + 1;

        if (filename)   flen = strlen(filename) + 1;
        if (options)    olen = strlen(options)  + 1;

        if (SegTrackerActive)
                slen = MAX_SEGTRACKER_LEN;

        /*
         *              If the calling task has locked the layers of the SnoopDos
         *              screen, then we ignore all calls by that task. This prevents
         *              us deadlocking when we wait for the semaphore (while meanwhile,
         *              the SnoopDos mainline code could own the semaphore, and be
         *              waiting for the layers to be unlocked by our caller so it can
         *              render some output).
         *
         *              We Forbid() to make sure the window doesn't get closed from
         *              under our feet while we're checking it. We have to try and
         *              allocate the event while still Forbid'd since otherwise, there
         *              is a small window where the main window could open, lock the
         *              layers, and lock the semaphore, in between our check. It's
         *              unlikely, but not _that_ unlikely (especially since the main
         *              window always does an immediate redraw when it opens)
         *
         *              THOR: RPort->Layer is set to NULL by some rendering calls
         *              and hence the code might find a NULL here. Handle this as if
         *              the layer is locked.
         */
        Forbid();
        if (MainWindow &&
                        (MainWindow->RPort->Layer                     == NULL     ||
                         MainWindow->RPort->Layer->Lock.ss_Owner      == thistask ||
                         MainWindow->WScreen->LayerInfo.Lock.ss_Owner == thistask)) {
                Permit();
                return (NULL);
        }
        ev = GetNewEvent(plen + alen + flen + olen + slen);
        Permit();

        if (!ev)
                return (NULL);

        ev->action = ev->filename = ev->options = ev->result = "";

        strptr = (char *)(ev+1);

        if (ShowCLINum && clinum)
                mysprintf(strptr, "[%ld] %s", clinum, procname);
        else
                strcpy(strptr, procname);
        ev->procname  = strptr;
        strptr       += plen;

//      KPrintF("New event: %ld, %s, %s\n", ev->seqnum, procname, action); /* DB */
        if (alen) {
                strcpy(strptr, action);
                ev->action   = strptr;
                strptr          += alen;
        }
        if (flen) {
                strcpy(strptr, filename);
                ev->filename  = strptr;
                strptr           += flen;
        }
        if (olen) {
                strcpy(strptr, options);
                ev->options   = strptr;
                strptr       += olen;
        }
        if (slen) {
                ev->segname       = strptr;
                strptr       += slen;
        } else {
                ev->segname   = NULL;   /* Say no segtracker storage allocated */
        }
        // DateStamp(&ev->datestamp);
        ev->calladdr    = calladdr;
        ev->processid   = (ULONG)thistask;
        // ev->processid        = GetFreeStack();

        if (seqnum)
                *seqnum = ev->seqnum;
        return (ev);
}

/*
 *              CheckTaskRecursion()
 *
 *              Checks if the current task is already listed as being on the
 *              recursion list. If so, returns FALSE, indicating that the
 *              caller should immediately pass control back to the original
 *              function and not try and do anything else.
 *
 *              Otherwise, records the task's address in a recursion list
 *              and returns a handle. This handle should be passed back to
 *              EndTaskRecursion() when the function is complete, which will
 *              cause it to be removed from the list.
 *
 *              OpenLibrary() needs to surround its call to CreateEvent() with
 *              CheckTaskRecursion()/EndTaskRecursion. Also, GetMsg()/PutMsg() need
 *              this to check if any of the DOS packets it monitors have been
 *              generated by AmigaDOS functions we monitor.
 */
ULONG CheckTaskRecursion(void)
{
        Task *taskid = SysBase->ThisTask;
        ULONG taskhandle;
        int i;

        for (i = 1; i < NUM_CACHED_TASKS; i++)
                if (CachedTask[i] == taskid)
                        return (0);

        ObtainSemaphore(&TaskCacheSem);
        taskhandle = TaskCacheIndex++;
        if (TaskCacheIndex >= NUM_CACHED_TASKS)
                TaskCacheIndex = 1;
        CachedTask[taskhandle] = taskid;
        ReleaseSemaphore(&TaskCacheSem);

        return (taskhandle);
}

/*
 *              EndTaskRecursion(taskhandle)
 *
 *              Called to remove a task from the recursion list.
 *              See CheckTaskRecursion() above.
 */
void EndTaskRecursion(ULONG taskhandle)
{
        CachedTask[taskhandle] = 0;
}

#if 0
/*
 *              TestPatch()
 *
 *              Patches a single function to try it out
 */
void TestPatch(void)
{
        extern List                     EventList;                      /* List of events               */
        static EventFormat evformat[50];
        static char outstr[500];
        char *formatstr = "%d %t %c %n %s %h %a %40f %o %r";
        int formatlen;

        OnlyShowFails    = 0;
        SegTrackerActive = 1;
        formatlen = ParseFormatString(formatstr, evformat, 50);

        if (!InitPatches()) {
                printf("Patch initialisation failed!\n");
                return;
        }

        PatchList[GID_OPENFILE].enabled = 1;
        UpdatePatches();
        printf("Patch now enabled!\n\n");

        FormatEvent(evformat, NULL, outstr, 0, formatlen-1);
        printf("%s\n", outstr);
        printf("%s\n", UnderlineTitles(evformat, outstr, '-'));

        for (;;) {
                ULONG mask;

                mask = Wait(SIGBREAKF_CTRL_C | NewEventMask);
                if (mask & SIGBREAKF_CTRL_C)
                        break;

                if (mask & NewEventMask) {
                        Event *ev;

                        SetSignal(0, NewEventMask);     /* Clear signal for next time */
                        FORLIST(&EventList, ev) {
                                int stat = ev->status;

                                if (stat == ES_UPDATING || stat == ES_READY) {
                                        FormatEvent(evformat, ev, outstr, 0, formatlen-1);
                                        printf("%s\n", outstr);
                                        if (stat == ES_READY)
                                                ev->status = ES_ACCEPTED;
                                }
                        }
                }
        }
        PatchList[GID_OPENFILE].enabled = 0;
        UpdatePatches();
        printf("Patch disabled\n");
        CleanupPatches();
}
#endif

/*
 *              HandlePaused(event, seqnum)
 *
 *              Should be called when pausing is enabled -- it sets the
 *              event Result code to "...." to indicate the event is about
 *              to be executed, then waits for the pausing to be turned off.
 *              If you pass in NULL as the event code, then this isn't done.
 *
 *              seqnum is the actual event number remembered by the caller,
 *              not (necessarily) the event number stored in the event. This
 *              is so that the code can check if the event has rolled off the
 *              top of the buffer or not before modifying it.
 *
 *              There are some circumstances where it is dangerous for
 *              us to pause because we are being called by a system task
 *              that is needed by the main SnoopDos program.
 *
 *              We work around this as follows. Firstly, if the caller happens
 *              to own the semaphore associated with the layer info for the
 *              window or screen containing the SnoopDos window, then we
 *              don't try and pause it since otherwise we might deadlock
 *              (the user wouldn't be able to unpause SnoopDos because that
 *              requires clicking on a button.)
 *
 *              Secondly, whenever SnoopDos itself does something that might
 *              stop it processing the main messages (e.g. show a requester,
 *              open a file, check for ASL, etc.) then pausing is temporarily
 *              turned off since we have no way of knowing whether a third
 *              party patch might be spinning off a background task to do it
 *              and the background task might try and wait on this. Bummer, eh?
 */
void HandlePaused(Event *ev, LONG seqnum)
{
        if (Paused) {
                /*
                 *              Need to forbid when checking layers since otherwise the
                 *              main window might close while we're reading its pointers.
                 */
                int gotlock = 0;
                char *oldresmsg;

                Forbid();
                if (MainWindow) {
                        struct Layer *layer;
                        struct Layer_Info *li;
                        Task *thistask = SysBase->ThisTask;

                        layer = MainWindow->RPort->Layer;
                        li    = &MainWindow->WScreen->LayerInfo;

                        /*
                         * THOR: Check whether the layer is available. Some Gfx rendering
                         * calls clear the layer temporarely.
                         * In that case, do not proceed since we do not really
                         * know who owns the layer.
                         */

                        if (layer) {
                                gotlock = (li->Lock.ss_Owner    == thistask) ||
                                          (layer->Lock.ss_Owner == thistask);
                        } else  gotlock = TRUE;
                }
                Permit();
                if (gotlock)
                        return;

                /*
                 *              Okay, safe to proceed with pausing. We simply set the
                 *              result message to indicate we're paused, signal
                 *              the main task, wait for the pausing to finish, restore
                 *              the result message, and return.
                 */
                if (ev) {
                        ObtainSemaphore(&BufSem);
                        if (seqnum >= RealFirstSeq) {
                                oldresmsg  = ev->result;
                                ev->result = MSG(MSG_RES_PAUSE);
                        }
                        ReleaseSemaphore(&BufSem);
                }
                Signal(SnoopTask, NewEventMask);        /* Make sure main task updates */
                ObtainSemaphore(&PauseSem);
                ReleaseSemaphore(&PauseSem);
                if (ev) {
                        ObtainSemaphore(&BufSem);
                        if (seqnum >= RealFirstSeq)
                                ev->result = oldresmsg;
                        ReleaseSemaphore(&BufSem);
                }
        }
}

#define CheckForPause(ev,seqnum)        if (Paused)     { HandlePaused(ev, seqnum); }

/*****************************************************************************
 *
 *              Now here are all the patches for the 25 or so functions that we
 *              patch into. Most of the patches follow a similar template, and
 *              share similar lower-level functions, but are different enough to
 *              prevent us from using the same patch for several functions.
 *
 *              We always signal both before and after a particular call, so that
 *              in the event of a call causing a crash, it will still show up in
 *              the SnoopDos window.
 *
 *              The OnlyShowFails flag is used to say whether we want to monitor
 *              all calls to a function, or only those which return a failure code.
 *              If the latter, we need to call the function first before deciding
 *              whether or not to create an event to record it -- in this case, the
 *              result code remains cached for the remainder of the function. The
 *              rest of the time, we create the event and signal the main task,
 *              then call the function to get the result, and finally update the
 *              event with the result and signal the main task again.
 *
 *              Each patch that calls a function and then updates the buffer on its
 *              return must ensure that the buffer entry is has a pointer to is still
 *              valid before it updates it. It does this by remembering the sequence
 *              number of the buffer entry, and then checking that the sequence
 *              number of the first buffer entry is still <= that (i.e. the entry
 *              is still somewhere in the buffer). This is vital to ensure that
 *              innocent buffer memory isn't trashed by old events.
 *
 *****************************************************************************/

/*
 *              New_FindPort(name)
 *
 *              Searches the system port list looking for a port name
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_FindPort,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindPort(reg_a1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindPort origfunc = (FP_FindPort)PatchList[GID_FINDPORT].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDPORT,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_FindResident(name)
 *
 *              Searches the system resident list looking for a resident module
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_FindResident,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindResident(reg_a1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindResident origfunc = (FP_FindResident)
                                                                PatchList[GID_FINDRESIDENT].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (SysBase->ThisTask == RealRamLibTask)
                JumpOrigFunc(0);

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDRESIDENT,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_FindSemaphore(name)
 *
 *              Searches the system semaphore list looking for the named semaphore
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_FindSemaphore,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindSemaphore(reg_a1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindSemaphore origfunc = (FP_FindSemaphore)
                                                                 PatchList[GID_FINDSEMAPHORE].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDSEM,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_FindTask(name)
 *
 *              Searches the system task list looking for the named task.
 *              We ignore calls to FindTask(0) since this is just
 *              looking for a pointer to the current task.
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_FindTask,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindTask(reg_a1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindTask origfunc = (FP_FindTask)PatchList[GID_FINDTASK].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (name == NULL)
                return (origfunc(name, libbase));

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDTASK,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_LockPubScreen(name)
 *
 *              Obtains a lock on the named public screen. If the name is NULL,
 *              then the caller is looking for a Lock() on the default screen,
 *              and we ignore it (otherwise we'd get loads of output).
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_LockPubScreen,
    AROS_UFHA (char *, name, A0),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_LockPubScreen(reg_a0 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_LockPubScreen origfunc = (FP_LockPubScreen)
                                                                PatchList[GID_LOCKSCREEN].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (!name)
                JumpOrigFunc(0);

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_LOCKSCREEN,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                Signal(SnoopTask, NewEventMask);
                CheckForPause(ev, seqnum);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_OpenDevice(name, unit, ioreq, flags)
 *
 *              Opens a new device. We need to be careful with the error return
 *              for this function, since success is indicated by a return of 0!
 *
 *              N.b. Kickstart 3.1 has a space-saving hack which allows a NULL
 *              device name to be interpreted as a magic cookie indicating a
 *              particular ROM devices. We ignore such calls (especially from CON,
 *              which does it a lot!) but flag other occurrances of NULL device
 *              names as an error.
 */
#ifdef __AROS__
AROS_UFH5 (ULONG, New_OpenDevice,
    AROS_UFHA (char *, name, A0),
    AROS_UFHA (long, unit, D0),
    AROS_UFHA (struct IORequest *, ioreq, A1),
    AROS_UFHA (long, flags, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_OpenDevice(reg_a0 char *name, reg_d0 long unit,
                                             reg_a1 struct IORequest *ioreq,
                                                 reg_d1 long flags, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_OpenDevice origfunc = (FP_OpenDevice)PatchList[GID_OPENDEVICE].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        char unitstr[20];
        char *devname = name;
        LONG  seqnum;

/*
 *              Release Semaphore replacement code for debugging patch
 */
#if MONITOR_SEMAPHORE
        Semaphore *sem = (Semaphore *)name;

        if (LoadTask == SysBase->ThisTask)
                KPrintF("ReleaseSemaphore in LoadSeg: retaddr = %08lx\n", CallAddr);
        return origfunc(name, unit, ioreq, flags, libbase);
#endif

        if (onlyfails) {
                result = origfunc(name, unit, ioreq, flags, libbase);
                if (!result)
                        return (result);
        }
        /*
         *              Now a workaround for a bug in CON that makes it call
         *              OpenDevice("timer.device" | NULL) very frequently.
         *              (The NULL is a space-saving shorthand which the 3.1 Kickstart
         *              interprets as a magic cookie for "timer.device" to save some
         *              ROM space).
         */
        if ((CallAddr >= RomStart && CallAddr <= RomEnd) ||
                                        strcmp(SysBase->ThisTask->tc_Node.ln_Name, "CON") == 0) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(unit);
        }
        if (!devname)
                devname = MSG(MSG_NULLSTR);

        mysprintf(unitstr, MSG(MSG_OPT_DEVUNIT), unit);
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_OPENDEV,
                                         devname, unitstr, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(unit);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, unit, ioreq, flags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (!result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_OpenFont(textattr)
 *
 *              Opens a new font that's currently in memory. OpenDiskFont will
 *              call this function first before checking the disk, so patching
 *              this function catches all accesses.
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_OpenFont,
    AROS_UFHA (struct TextAttr *, textattr, A0),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_OpenFont(reg_a0 struct TextAttr *textattr, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_OpenFont origfunc = (FP_OpenFont)PatchList[GID_OPENFONT].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        char *name;
        char sizestr[20];
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(textattr, libbase);
                if (result)
                        return (result);
        }
        if (textattr) {
                mysprintf(sizestr, MSG(MSG_OPT_FONTSIZE), textattr->ta_YSize);
                name = textattr->ta_Name;
        } else {
                *sizestr = '\0';
                name = MSG(MSG_NULLSTR);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_OPENFONT,
                                         name, sizestr, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(textattr, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_OpenLibrary(name, version)
 *
 *              Opens the named library with the specified version number.
 *
 *              We have a nasty potential for deadlock here -- CreateEvent()
 *              which we call, in turns calls DateStamp(), and DateStamp()
 *              itself calls OpenLibrary(....). So, we need to protect
 *              ourselves against a recursive call.
 *
 *              We do this by ignoring any call to open dos.library made
 *              by a call originating in ROM.
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_OpenLibrary,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (long, version, D0),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_OpenLibrary(reg_a1 char *name, reg_d0 long version,
                                                  reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_OpenLibrary origfunc = (FP_OpenLibrary)
                                                           PatchList[GID_OPENLIBRARY].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        char verstr[20];
        ULONG taskhandle;
        LONG  seqnum;

        /*
         *              Check for re-entrant calling (usually caused by the
         *              DateStamp call in the GetNewEvent() code).
         *
         *              In theory, we should only need to check if the semaphore
         *              is owned by the current task. In practise, we need
         *              the additional checks to be really sure (otherwise
         *              we seem to get unexplained crashes very frequently --
         *              very mysterious and worrying).
         *
         *              We also ignore calls to open dos.library -- this is partly
         *              because it gets called by DateStamp() inside dos.library
         *              itself (so things that call DateStamp once a second, like
         *              Enforcer(), will generate loads of calls to it) and partly
         *              to avoid problems with our own CreateEvent() code calling it.
         */
        if (BufSem.ss_Owner == SysBase->ThisTask ||
                strcmp(name, "dos.library") == 0)
        {
                JumpOrigFunc(version);
        }

        if (onlyfails) {
                result = origfunc(name, version, libbase);
                if (result)
                        return (result);
        }
        taskhandle = CheckTaskRecursion();
        if (!taskhandle) {
                if (!onlyfails)
                        result = origfunc(name, version, libbase);
                return (result);
        }
        mysprintf(verstr, MSG(MSG_OPT_LIBVER), version);
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_OPENLIB,
                                         name, verstr, NO_EXPAND);
        EndTaskRecursion(taskhandle);

        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(version);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, version, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_OpenResource(name)
 *
 *              Opens the named resource
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_OpenResource,
    AROS_UFHA (char *, name, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_OpenResource(reg_a1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_OpenResource origfunc = (FP_OpenResource)
                                                            PatchList[GID_OPENRESOURCE].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_OPENRES,
                                         name, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_FindToolType(array, tooltype)
 *
 *              Searches the tooltype array for a particular tooltype
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_FindToolType,
    AROS_UFHA (char **, array, A0),
    AROS_UFHA (char *, tooltype, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindToolType(reg_a0 char **array, reg_a1 char *tooltype,
                                                   reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindToolType origfunc = (FP_FindToolType)
                                                            PatchList[GID_READTOOLTYPES].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        char *toolname = tooltype;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(array, tooltype, libbase);
                if (result)
                        return (result);
        }
        if (toolname == NULL)
                toolname = MSG(MSG_NULLSTR);

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDTOOL,
                                         toolname, NULL, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(array, tooltype, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_MatchToolValue(tooltype, value)
 *
 *              Checks if a specified tooltype contains a given value
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_MatchToolValue,
    AROS_UFHA (char *, tooltype, A0),
    AROS_UFHA (char *, value, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_MatchToolValue(reg_a0 char *tooltype, reg_a1 char *value,
                                                     reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_MatchToolValue origfunc = (FP_MatchToolValue)
                                                              PatchList[GID_READTOOLTYPES2].origfunc;
        int onlyfails = OnlyShowFails;
        ULONG result;
        Event *ev;
        char *toolname = tooltype;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(tooltype, value, libbase);
                if (result)
                        return (result);
        }
        if (toolname == NULL)
                toolname = MSG(MSG_NULLSTR);

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_MATCHTOOL,
                                         toolname, value, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(tooltype, value, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_CurrentDir()
 *
 *              Changes current directory to somewhere else
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_CurrentDir,
    AROS_UFHA (BPTR, lock, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_CurrentDir(reg_d1 BPTR lock, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_CurrentDir origfunc = (FP_CurrentDir)PatchList[GID_CHANGEDIR].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        char lockbuf[MAX_LOCK_LEN+1];
        char *lockpath;
        LONG  seqnum;

        if (onlyfails)
                return origfunc(lock, libbase);         /* CurrentDir never fails */

        lockpath = MyNameFromLock(lock, NULL, lockbuf, MAX_LOCK_LEN);
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_CHANGEDIR,
                                         lockpath, NULL, NO_EXPAND);
        if (!ev)
                JumpOrigFunc(0);

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        ev->status = ES_UPDATING;
        CheckForPause(ev, seqnum);
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return origfunc(lock, libbase);
}

/*
 *              New_DeleteFile()
 *
 *              Deletes a file from disk
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_DeleteFile,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_DeleteFile(reg_d1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_DeleteFile origfunc = (FP_DeleteFile)PatchList[GID_DELETE].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_DELETE,
                                         name, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_Execute()
 *
 *              Executes a command from disk. Now superceded by System and RunCommand
 */
#ifdef __AROS__
AROS_UFH4 (ULONG, New_Execute,
    AROS_UFHA (char *, cmdline, D1),
    AROS_UFHA (BPTR, fin, D2),
    AROS_UFHA (BPTR, fout, D3),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_Execute(reg_d1 char *cmdline, reg_d2 BPTR fin, reg_d3 BPTR fout,
                                          reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_Execute origfunc = (FP_Execute)PatchList[GID_EXECUTE].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char *optstr;
        LONG  seqnum;

        /*
         *              For functions that may potentially take a long time to complete,
         *              we check if they're being called from ROM straight away, so
         *              that we can exit and not hang around waiting for them if they
         *              are, even in the OnlyShowFails case.
         */
        if (!MonROMCalls && CallAddr >= RomStart && CallAddr <= RomEnd)
                JumpOrigFunc(0);

        if (onlyfails) {
                result = origfunc(cmdline, fin, fout, libbase);
                if (result)
                        return (result);
        }
        if (fin == NULL)
                optstr = MSG(MSG_OPT_EXECSINGLE);
        else
                optstr = MSG(MSG_OPT_EXECBATCH);

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_EXECUTE,
                                         cmdline, optstr, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(cmdline, fin, fout, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_GetVar()
 *
 *              Inquires about the value of an environment variable
 */
#ifdef __AROS__
AROS_UFH5 (ULONG, New_GetVar,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (char *, buffer, D2),
    AROS_UFHA (LONG, size, D3),
    AROS_UFHA (LONG, flags, D4),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_GetVar(reg_d1 char *name, reg_d2 char *buffer,
                                         reg_d3 size, reg_d4 flags, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_GetVar origfunc = (FP_GetVar)PatchList[GID_GETVAR].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char optstr[20];
        ULONG optid;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, buffer, size, flags, libbase);
                if (result != -1)
                        return (result);
        }
        /*
         *              Now determine our flags. We recognise global, local
         *              or either types of variable. Binary variables are
         *              flagged with a trailing asterisk.
         */
        if              (flags & GVF_GLOBAL_ONLY)       optid = MSG_OPT_GLOBAL;
        else if ((flags & 7) == LV_ALIAS)       optid = MSG_OPT_ALIAS;
        else if (flags & GVF_LOCAL_ONLY)        optid = MSG_OPT_LOCAL;
        else                                                            optid = MSG_OPT_ANY;
        strcpy(optstr, MSG(optid));

        if (flags & GVF_BINARY_VAR)                     strcat(optstr, "*");

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_GETVAR,
                                         name, optstr, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, buffer, size, flags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result != -1)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_FindVar()
 *
 *              Inquires about the value of a local environment variable
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_FindVar,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (ULONG, type, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_FindVar(reg_d1 char *name, reg_d2 ULONG type,
                                          reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_FindVar origfunc = (FP_FindVar)PatchList[GID_GETVAR2].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        ULONG optid;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, type, libbase);
                if (result)
                        return (result);
        }

        if              ((type & 7) == LV_VAR)          optid = MSG_OPT_LOCAL;
        else if ((type & 7) == LV_ALIAS)        optid = MSG_OPT_ALIAS;
        else                                                            optid = MSG_OPT_UNKNOWN;

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_FINDVAR,
                                         name, MSG(optid), NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                Signal(SnoopTask, NewEventMask);
                CheckForPause(ev, seqnum);
                result = origfunc(name, type, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_SetVar()
 *
 *              Sets a (possibly new) variable to a particular value
 */
#ifdef __AROS__
AROS_UFH5 (ULONG, New_SetVar,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (char *, buffer, D2),
    AROS_UFHA (LONG, size, D3),
    AROS_UFHA (LONG, flags, D4),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_SetVar(reg_d1 char *name, reg_d2 char *buffer,
                                         reg_d3 size, reg_d4 flags, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_SetVar origfunc = (FP_SetVar)PatchList[GID_SETVAR].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char varstr[MAX_STR_LEN+1];
        int vlen;
        ULONG optid;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, buffer, size, flags, libbase);
                if (result)
                        return (result);
        }
        /*
         *              Now determine our flags. We recognise global, local
         *              or alias variables.
         */
        if              (flags & GVF_GLOBAL_ONLY)       optid = MSG_OPT_GLOBAL;
        else if ((flags & 7) == LV_VAR)         optid = MSG_OPT_LOCAL;
        else if ((flags & 7) == LV_ALIAS)       optid = MSG_OPT_ALIAS;
        else                                                            optid = MSG_OPT_UNKNOWN;

        /*
         *              Now create a string that looks like "Variable=Value"
         *
         *              We go to some pains to ensure we don't overwrite our
         *              string length
         */
        vlen = strlen(name);
        if (vlen > (MAX_STR_LEN-1)) {
                strncpy(varstr, name, MAX_STR_LEN);
                varstr[MAX_STR_LEN] = 0;
        } else {
                strcpy(varstr, name);
                strcat(varstr, "=");
                vlen = 98 - vlen;
                if (size != -1)
                        vlen = MIN(vlen, size);

                strncat(varstr, buffer, vlen);
                varstr[MAX_STR_LEN] = 0;
        }

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_SETVAR,
                                         varstr, MSG(optid), NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, buffer, size, flags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_DeleteVar()
 *
 *              Deletes an environment variable from the environment
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_DeleteVar,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (ULONG, flags, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_DeleteVar(reg_d1 char *name, reg_d2 ULONG flags,
                                                reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_DeleteVar origfunc = (FP_DeleteVar)PatchList[GID_SETVAR2].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        ULONG optid;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, flags, libbase);
                if (result)
                        return (result);
        }
        /*
         *              Now determine our flags. We recognise global, local
         *              or alias variables.
         */
        if              (flags & GVF_GLOBAL_ONLY)       optid = MSG_OPT_GLOBAL;
        else if ((flags & 7) == LV_VAR)         optid = MSG_OPT_LOCAL;
        else if ((flags & 7) == LV_ALIAS)       optid = MSG_OPT_ALIAS;
        else                                                            optid = MSG_OPT_UNKNOWN;

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_KILLVAR,
                                         name, MSG(optid), NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, flags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_LoadSeg()
 *
 *              Tries to load in a module from disk
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_LoadSeg,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_LoadSeg(reg_d1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_LoadSeg origfunc = (FP_LoadSeg)PatchList[GID_LOADSEG].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_LOADSEG,
                                         name, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);

#if MONITOR_SEMAPHORE
                LoadTask = SysBase->ThisTask;
                result = origfunc(name, libbase);
                LoadTask = 0;
#else
                result = origfunc(name, libbase);
#endif
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_NewLoadSeg()
 *
 *              Tries to load in a module from disk
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_NewLoadSeg,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (TAGPTR, tags, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_NewLoadSeg(reg_d1 char *name, reg_d2 TAGPTR tags,
                                                 reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_NewLoadSeg origfunc = (FP_NewLoadSeg)PatchList[GID_LOADSEG2].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, tags, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_NEWLOADSEG,
                                         name, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, tags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_NewLoadSeg()
 *
 *              Tries to load in a module from disk
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_Lock,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (LONG, mode, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_Lock(reg_d1 char *name, reg_d2 LONG mode, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_Lock origfunc = (FP_Lock)PatchList[GID_LOCKFILE].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG optid;
        char *curname;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, mode, libbase);
                if (result)
                        return (result);
        }
        if              (mode == ACCESS_READ)           optid = MSG_OPT_READ;
        else if (mode == ACCESS_WRITE)          optid = MSG_OPT_WRITE;
        else                                                            optid = MSG_OPT_READBAD;

        curname = name;
        if (!ShowPaths && *curname == '\0')
                curname = "\"\"";

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_LOCK,
                                         curname, MSG(optid), EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, mode, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_CreateDir()
 *
 *              Creates a new directory on disk
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_CreateDir,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_CreateDir(reg_d1 char *name, reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_CreateDir origfunc = (FP_CreateDir)PatchList[GID_MAKEDIR].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(name, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_MAKEDIR,
                                         name, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_MakeLink()
 *
 *              Creates a new hard or soft link
 */
#ifdef __AROS__
AROS_UFH4 (ULONG, New_MakeLink,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (LONG, dest, D2),
    AROS_UFHA (LONG, soft, D3),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_MakeLink(reg_d1 char *name, reg_d2 LONG dest, reg_d3 LONG soft,
                                           reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        struct Process *myproc = (struct Process *)SysBase->ThisTask;
        FP_MakeLink origfunc   = (FP_MakeLink)PatchList[GID_MAKELINK].origfunc;
        int onlyfails          = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG optid;
        char namestr[MAX_STR_LEN+1];
        LONG seqnum;
        int  len;

        if (onlyfails) {
                result = origfunc(name, dest, soft, libbase);
                if (result)
                        return (result);
        }
        if (soft)       optid = MSG_OPT_SOFTLINK;
        else            optid = MSG_OPT_HARDLINK;

        /*
         *              Now build a string that looks like "name --> <dest>" to display
         *              as our link. For soft links, we just concatenate the two
         *              strings. For hard links, we have to generate a filename
         *              from the lock also.
         */
        len = strlen(name);
        if (len >= MAX_STR_LEN) {
                strncpy(namestr, name, MAX_STR_LEN);
                namestr[MAX_STR_LEN] = 0;
        } else {
                if (ShowPaths) {
                        strcpy(namestr, MyNameFromLock(myproc->pr_CurrentDir,
                                                                                   name, namestr, MAX_STR_LEN-2));
                        len = strlen(namestr);
                } else
                        strcpy(namestr, name);

                strcat(namestr, LinkPointerString);             /* Usually ' --> ' */
                if (soft) {
                        strncat(namestr, (char *)dest, MAX_STR_LEN - len - 1);
                        namestr[MAX_STR_LEN] = 0;
                } else {
                        strcat(namestr, MyNameFromLock((BPTR)dest, NULL, namestr+len+1,
                                                                                                 MAX_STR_LEN-len-1));
                }
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_MAKELINK,
                                         namestr, MSG(optid), NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, dest, soft, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_Open()
 *
 *              Opens a new file on disk
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_Open,
    AROS_UFHA (char *, name, D1),
    AROS_UFHA (LONG, accessMode, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_Open(reg_d1 char *name, reg_d2 LONG accessMode,
                                  reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_Open origfunc = (FP_Open)PatchList[GID_OPENFILE].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char *optstr;
        LONG  seqnum;

        if              (accessMode == MODE_OLDFILE)    optstr = MSG(MSG_OPT_READ);
        else if (accessMode == MODE_NEWFILE)    optstr = MSG(MSG_OPT_WRITE);
        else if (accessMode == MODE_READWRITE)  optstr = MSG(MSG_OPT_MODIFY);
        else                                                                    optstr = MSG(MSG_OPT_UNKNOWN);

        if (onlyfails) {
                result = origfunc(name, accessMode, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_OPEN,
                                         name, optstr, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(name, accessMode, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_Rename()
 *
 *              Renames a file on disk. This is a little tricky, since we have
 *              to generate two events: one containing the source name and
 *              one containing the destination name.
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_Rename,
    AROS_UFHA (char *, oldname, D1),
    AROS_UFHA (char *, newname, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_Rename(reg_d1 char *oldname, reg_d2 char *newname,
                                     reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_Rename origfunc = (FP_Rename)PatchList[GID_RENAME].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        LONG  seqnum;

        if (onlyfails) {
                result = origfunc(oldname, newname, libbase);
                if (result)
                        return (result);
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_RENAME,
                                         oldname, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }
        ev->status = ES_READY;
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_RENAME2,
                                         newname, NULL, EXPAND_NAME);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(oldname, newname, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result)
                        ev->result = MSG(MSG_RES_OKAY);
                else
                        ev->result = MSG(MSG_RES_FAIL);
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_RunCommand()
 *
 *              Runs a command from disk. Like Execute() only with more control.
 */
#ifdef __AROS__
AROS_UFH5 (ULONG, New_RunCommand,
    AROS_UFHA (BPTR, seglist, D1),
    AROS_UFHA (ULONG, stack, D2),
    AROS_UFHA (char *, cmdline, D3),
    AROS_UFHA (LONG, cmdlen, D4),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_RunCommand(reg_d1 BPTR seglist,  reg_d2 ULONG stack,
                                                 reg_d3 char *cmdline, reg_d4 cmdlen,
                                         reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_RunCommand origfunc = (FP_RunCommand)PatchList[GID_RUNCOMMAND].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char  stacksize[40];    /* Reserve additional space for return code! */
        LONG  seqnum;
        int       stklen;
        char  *p;

        /*
         *              For functions that may potentially take a long time to complete,
         *              we check if they're being called from ROM straight away, so
         *              that we can exit and not hang around waiting for them if they
         *              are, even in the OnlyShowFails case.
         */
        if (!MonROMCalls && CallAddr >= RomStart && CallAddr <= RomEnd)
                JumpOrigFunc(0);

        if (onlyfails) {
                result = origfunc(seglist, stack, cmdline, cmdlen, libbase);
                if (result != -1)
                        return (result);
        }

        /*
         *              This is kind of nasty but it works. We need somewhere to store
         *              the return code string, but when we call CreateEvent(), we
         *              don't know what it will be yet. So, we allocate additional
         *              space at the end of our stacksize to hold this info, and
         *              overwrite it later on.
         */
        mysprintf(stacksize, "%ld", stack);
        stklen = strlen(stacksize);
        strcat(stacksize, "................");  /* Storage for return code */

        /*
         *              Next, a quick hack to get rid of that nasty CR at the end
         *              of the string (purely for visual purposes -- we put it back
         *              afterwards, of course!)
         */
        for (p = cmdline; *p; p++) {
                if (*p == '\n') {
                        *p = ' ';
                        break;
                }
        }
        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_RUNCOMMAND,
                                         cmdline, stacksize, NO_EXPAND);
        if (*p)
                *p = '\n';

        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }
        ev->options[stklen] = 0;

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->result = "----";
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(seglist, stack, cmdline, cmdlen, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result != -1) {
                        ev->result = ev->options + stklen + 1;
                        mysprintf(ev->result, "%ld", result);
                } else {
                        ev->result = MSG(MSG_RES_FAIL);
                }
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*
 *              New_SystemTagList()
 *
 *              Executes a command line. Like Execute() only more powerful.
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_SystemTagList,
    AROS_UFHA (char *, cmdline, D1),
    AROS_UFHA (TAGPTR, tags, D2),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_SystemTagList(reg_d1 char *cmdline, reg_d2 TAGPTR tags,
                                                reg_a6 void *libbase)
#endif
{
        MarkCallAddr;
        FP_SystemTagList origfunc = (FP_SystemTagList)
                                                                 PatchList[GID_SYSTEM].origfunc;
        int onlyfails = OnlyShowFails;
        Event *ev;
        ULONG result;
        char  optstr[20];       /* Reserve additional space for return code! */
        LONG  seqnum;

        /*
         *              For functions that may potentially take a long time to complete,
         *              we check if they're being called from ROM straight away, so
         *              that we can exit and not hang around waiting for them if they
         *              are, even in the OnlyShowFails case.
         */
        if (!MonROMCalls && CallAddr >= RomStart && CallAddr <= RomEnd)
                JumpOrigFunc(0);

        if (onlyfails) {
                result = origfunc(cmdline, tags, libbase);
                if (result != -1)
                        return (result);
        }
        /*
         *              This is kind of nasty but it works. We need somewhere to store
         *              the return code string, but when we call CreateEvent(), we
         *              don't know what it will be yet. So, we allocate space for our
         *              option string instead and then replace it with our result
         *              string later on.
         */
        strcpy(optstr, "............"); /* Storage for result string */

        ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_SYSTEM, cmdline,
                                         optstr, NO_EXPAND);
        if (!ev) {
                if (onlyfails)
                        return (result);
                JumpOrigFunc(0);
        }
        *ev->options = '\0';

        /*
         *              Allocated event safely, now tell SnoopDos task to wake up
         */
        if (onlyfails) {
                CheckForPause(NULL, 0);
        } else {
                ev->result = "----";
                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
                result = origfunc(cmdline, tags, libbase);
        }
        ObtainSemaphore(&BufSem);
        if (seqnum >= RealFirstSeq) {
                if (result != -1) {
                        ev->result  = ev->options;
                        ev->options = "";
                        mysprintf(ev->result, "%ld", result);
                } else {
                        ev->result = MSG(MSG_RES_FAIL);
                }
                ev->status = ES_READY;
                Signal(SnoopTask, NewEventMask);
        }
        ReleaseSemaphore(&BufSem);
        return (result);
}

/*****************************************************************************
 *
 *        Now the Mother of all patches -- the PutMsg() packet monitor/Rexx patch
 *
 *        (There are quite a few sub-functions associated with this.)
 *
 *****************************************************************************/

/*
 *              New_AddDosEntry(dlist)
 *
 *              This is a very simple patch which is only enabled when we are
 *              monitoring DOS packets. It simply signals the main task to update
 *              the device list whenever a new device is added to the DOS device
 *              list.
 */
#ifdef __AROS__
AROS_UFH2 (ULONG, New_AddDosEntry,
    AROS_UFHA (struct DosList *, dlist, D1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_AddDosEntry(reg_d1 struct DosList *dlist, reg_a6 void *libbase)
#endif
{
        FP_AddDosEntry origfunc = (FP_AddDosEntry)
                                                           PatchList[GID_ADDDOSENTRY].origfunc;
        ULONG result;

        /*
         *              Note that we better remember to actually add the new device
         *              before telling SnoopDos to update its list, otherwise it
         *              might miss it if it's too quick (e.g. priority > 0).
         *
         *              New: Even though we DO add it before signalling SnoopDos, the
         *              device itself won't be fully initialised for a little bit longer.
         *              Therefore, the main SnoopDos code
         */
        result = origfunc(dlist, libbase);

        if (dlist && dlist->dol_Type == DLT_DEVICE)
                Signal(SnoopTask, ScanDosListMask);

        return (result);
}

/*
 *              AddWaitingPacket(ev, seqnum, failmsg, dp, port, pt)
 *
 *              Adds a new entry to the list of waiting packets containing enough
 *              information to allow us to identify the reply to the packet when
 *              it is processed later on (in the main PutMsg() patch).
 *
 *              If there isn't a spare waiting packet, we will steal one and mark
 *              the event it represented as "Miss" to indicate the return code
 *              was missed.
 */
void AddWaitingPacket(Event *ev, LONG seqnum, char *failmsg,
                                          struct DosPacket *dp, struct MsgPort *port,
                                          struct PacketRef *pt)
{
        WaitPacket *wp;
        Event *oldev;
        ULONG oldseq;

        /*
         *              Add details about this packet to our list of waiting
         *              packets, so that when we spot the packet being returned
         *              by PutMsg() later on, we can fill in the result code.
         *
         *              Note that we must not obtain both the buffer and packet
         *              semaphores simultaneously, or a deadlock could occur.
         */
        ObtainSemaphore(&PacketSem);
        if (IsListEmpty(&PacketFreeList)) {
                /*
                 *              Removing an existing node which is already in use. We need
                 *              to signal our main task to tell it not to continue waiting
                 *              for this event to complete. We have to wait until we unlock
                 *              the packetsem so we just note the details for now.
                 */
                wp     = (WaitPacket *)RemTail(&PacketWaitList);
                oldseq = wp->eventnum;
                oldev  = wp->event;
                //      if (!wp)
                //              KPrintF("Warning!! No packet available on WaitList!\n");
        } else {
                wp     = (WaitPacket *)RemHead(&PacketFreeList);
                oldseq = 0;
        }
        wp->dp           = dp;
        wp->sendtask = SysBase->ThisTask;
        wp->destport = port;
        wp->event    = ev;
        wp->eventnum = seqnum;
        wp->arg1         = dp->dp_Arg1;
        wp->arg2         = dp->dp_Arg2;
        wp->arg3         = dp->dp_Arg3;
        wp->resmsg       = failmsg;
        wp->flags    = pt->flags;
        AddHead(&PacketWaitList, (Node *)wp);
        ReleaseSemaphore(&PacketSem);

        /*
         *              Finally, check if we had to flush out an old packet from
         *              our list of waiting packets when we allocated the new
         *              packet. If so, mark the packet as missed (if it hasn't
         *              already scrolled off the top of the buffer)
         */
        if (oldseq) {
                ObtainSemaphore(&BufSem);
                if (oldseq >= RealFirstSeq) {
                        oldev->result = MSG(MSG_RES_MISSED);
                        oldev->status = ES_READY;
                        Signal(SnoopTask, NewEventMask);
                }
                ReleaseSemaphore(&BufSem);
        }
}

/*
 *              HandleSimplePacket(calladdr, packet, port, packetref)
 *
 *              Handles the given packet by creating an event that matches the
 *              packet type (if we support it) and creating a new entry on the
 *              waiting packet list so we can match it up later.
 *
 *              This function handles only simple packets (i.e. those with the
 *              PK_COMMON flag set in the packet table). If the corresponding
 *              AmigaDOS option (Open, Lock, MakeDir etc.) is not currently
 *              enabled, then no entry is made for this packet.
 */
void HandleSimplePacket(ULONG calladdr, struct DosPacket *dp,
                                        struct MsgPort *port, struct PacketRef *pt)
{
        /*
         *              Now create our new simple event to represent the packet.
         *              The contents of this will vary depending on the packet
         *              itself, but in general, is an exact match with the format
         *              of the AmigaDOS function associated with the packet.
         *
         *              (If that function is not being monitored, then we do nothing.)
         */
        Event *ev;
        LONG  seqnum;
        ULONG gadid;
        ULONG actid = 0;
        char  *volname = ((Task *)(port->mp_SigTask))->tc_Node.ln_Name;
        BPTR  lock = NULL;
        BSTR  name;
        char  *optstr;
        char  namebuf[MAX_STR_LEN+1];
        char  optbuf[20];

        namebuf[0] = '\0';

        /*
         *              First, determine the format to use, based on the packet type
         */
        switch (dp->dp_Type) {
                case ACTION_FINDINPUT:
                        gadid   = GID_OPENFILE;
                        actid   = MSG_ACT_POPEN;
                        lock    = (BPTR)dp->dp_Arg2;
                        name    = (BSTR)dp->dp_Arg3;
                        optstr  = MSG(MSG_OPT_READ);
                        break;

                case ACTION_FINDOUTPUT:
                        gadid   = GID_OPENFILE;
                        actid   = MSG_ACT_POPEN;
                        lock    = (BPTR)dp->dp_Arg2;
                        name    = (BSTR)dp->dp_Arg3;
                        optstr  = MSG(MSG_OPT_WRITE);
                        break;

                case ACTION_FINDUPDATE:
                        gadid   = GID_OPENFILE;
                        actid   = MSG_ACT_POPEN;
                        lock    = (BPTR)dp->dp_Arg2;
                        name    = (BSTR)dp->dp_Arg3;
                        optstr  = MSG(MSG_OPT_MODIFY);
                        break;

                case ACTION_DELETE_OBJECT:
                        gadid   = GID_DELETE;
                        actid   = MSG_ACT_PDELETE;
                        lock    = (BPTR)dp->dp_Arg1;
                        name    = (BSTR)dp->dp_Arg2;
                        optstr  = NULL;
                        break;

                case ACTION_CREATE_DIR:
                        gadid   = GID_MAKEDIR;
                        actid   = MSG_ACT_PMAKEDIR;
                        lock    = (BPTR)dp->dp_Arg1;
                        name    = (BSTR)dp->dp_Arg2;
                        optstr  = NULL;
                        break;

                case ACTION_LOCATE_OBJECT:
                        gadid   = GID_LOCKFILE;
                        actid   = MSG_ACT_PLOCK;
                        lock    = (BPTR)dp->dp_Arg1;
                        name    = (BSTR)dp->dp_Arg2;
                        switch (dp->dp_Arg3) {
                                case ACCESS_READ:       optstr = MSG(MSG_OPT_READ);             break;
                                case ACCESS_WRITE:      optstr = MSG(MSG_OPT_WRITE);    break;
                                default:                        optstr = MSG(MSG_OPT_READBAD);  break;
                        }
                        break;

                case ACTION_MAKE_LINK:
                {
                        int len;
                        char *cname;

                        gadid   = GID_MAKELINK;
                        actid   = MSG_ACT_PMAKELINK;
                        lock    = (BPTR)dp->dp_Arg1;
                        cname   = BTOC(dp->dp_Arg2);

                        /*
                         *              Now build a string that looks like "name --> <dest>" to
                         *              display as our link. For soft links, we just concatenate
                         *              the two strings. For hard links, we have to generate a
                         *              filename from the lock also.
                         */
                        len = *cname;
                        if (len >= MAX_STR_LEN) {
                                strncpy(namebuf, cname+1, MAX_STR_LEN);
                                namebuf[MAX_STR_LEN] = 0;
                        } else {
                                if (ShowPaths) {
                                        char *tempbuf = namebuf + MAX_STR_LEN-9 - len;

                                        memcpy(tempbuf, cname+1, len);
                                        tempbuf[len] = '\0';
                                        strcpy(namebuf, AsyncNameFromLock(volname, lock, tempbuf,
                                                                                                  namebuf, MAX_STR_LEN-8));
                                        len = strlen(namebuf);
                                } else {
                                        memcpy(namebuf, cname+1, len);
                                        namebuf[len] = '\0';
                                }
                                strcat(namebuf, LinkPointerString);             /* Usually ' --> ' */
                                len += strlen(LinkPointerString);

                                if (dp->dp_Arg4 == LINK_HARD) {
                                        /*
                                         *              Hard link: interpret dp->dp_Arg3 as the lock
                                         *              being linked to
                                         */
                                        strcat(namebuf, AsyncNameFromLock(volname, (BPTR)dp->dp_Arg3,
                                                                                                          NULL, namebuf+len+1,
                                                                                                          MAX_STR_LEN-len-1));
                                        optstr = MSG(MSG_OPT_HARDLINK);
                                } else {
                                        /*
                                         *              Soft link: interpret dp->dp_Arg3 as the name of
                                         *              the path to the link
                                         */
                                        strncat(namebuf, (char *)dp->dp_Arg3, MAX_STR_LEN-len-1);
                                        namebuf[MAX_STR_LEN] = 0;
                                        optstr = MSG(MSG_OPT_SOFTLINK);
                                }
                        }
                        volname = (char *)NO_EXPAND; /* Don't try and expand filename */
                        name    = NULL;
                        break;
                }

                case ACTION_RENAME_OBJECT:
                        lock    = (BPTR)dp->dp_Arg1;
                        name    = (BSTR)dp->dp_Arg2;
                        optstr  = NULL;

                        /*
                         *              This is a bit cheeky. The Rename action comes in
                         *              two parts, the first line with the original name and
                         *              the second part with the new name. We create the
                         *              first part here, and then fall through to let the
                         *              rest of the code handle the second part.
                         */
                        if (CurSettings.Func.Opts[GID_RENAME]) {
                                char *cname = BTOC(name);

                                lock            = (BPTR)dp->dp_Arg3;
                                name            = (BSTR)dp->dp_Arg4;
                                if (*cname) {
                                        memcpy(namebuf, cname+1, *cname);
                                        namebuf[(int)*cname] = '\0';
                                } else {
                                        /*
                                         *              If we have an empty string, we replace it with ""
                                         *              since we may be renaming the current directory (?)
                                         */
                                        strcpy(namebuf, "\"\"");
                                }
                                ev = CreateEvent(calladdr, &seqnum, MSG_ACT_PRENAME,
                                                                 namebuf, NULL, (int)volname, lock);
                                if (!ev)
                                        return;

                                ev->status      = ES_READY;
                                gadid           = GID_RENAME;
                                actid           = MSG_ACT_PRENAME2;
                        }
                        break;
        }

        /*
         *              Now, if we recognised the packet type, create a new event
         *              with the info stored accordingly. We ignore any packets
         *              for which monitoring is disabled.
         */
        if (actid && CurSettings.Func.Opts[gadid]) {
                /*
                 *              We reserve 8 bytes at the start of the options string to
                 *              hold the result message (Ok or Fail) that is filled in
                 *              by the PutMsg() code when the packet is returned.
                 */
#define RES_LEN         8

                strcpy(optbuf, "........");
                if (optstr)
                        strcat(optbuf, optstr);

                if (name) {
                        char *cname = BTOC(name);

                        namebuf[0] = '\0';
                        if (*cname) {
                                memcpy(namebuf, cname+1, *cname);
                                namebuf[(int)*cname] = '\0';
                        } else {
                                /*
                                 *              If we have an empty string, we replace it with ""
                                 *              since it will typically be a Lock("").
                                 */
                                strcpy(namebuf, "\"\"");
                        }
                }
                ev = CreateEvent(calladdr, &seqnum, actid, namebuf, optbuf,
                                                 (int)volname, lock);
                if (!ev)
                        return;

                ev->result    = ev->options;
                ev->options  += RES_LEN;
                *ev->result   = '\0';
                ev->status    = ES_UPDATING;
                /*
                 *              We add the event to the waiting list BEFORE checking for Pause
                 *              so that if while paused, the user decides to disable packet
                 *              monitoring, the packet will be in the wait queue already and
                 *              thus will be safely flushed; otherwise, it would hang around
                 *              for ever (not doing any damage but wasting resources).
                 */
                AddWaitingPacket(ev, seqnum, "%s", dp, port, pt);
                CheckForPause(ev, seqnum);
                Signal(SnoopTask, NewEventMask);
        }
#undef RES_LEN
}

/*
 *              HandleRawPacket(calladdr, packet, port, packetref)
 *
 *              Handles the given packet by creating an event that matches the
 *              packet type (if we support it) and creating a new entry on the
 *              waiting packet list so we can match it up later.
 *
 *              This function handles all packets and produces a raw hex dump
 *              (with the name of the packet) rather than the more english-like
 *              "Open", "Lock", etc.
 */
void HandleRawPacket(ULONG calladdr, struct DosPacket *dp,
                                     struct MsgPort *port, struct PacketRef *pt)
{
        /*
         *              Building this event is a little tricky, since the result
         *              string will not be a static string but will contain the
         *              actual return value. Thus, we allocate enough room in the
         *              filename portion to hold the result string as well, and
         *              adjust our variables accordingly after the event has
         *              been created.
         *
         *              The same is true for raw packet types (i.e. those that we
         *              don't recognise at all).
         */
#define RAW_LEN         20              /* Length of possible raw packet type   */
#define RES_LEN         26              /* Length of longest result code                */
#define ARG_LEN         50              /* Length of longest argument list              */

        static char *fmt[] = {
                "(None)", "%8lx", "%8lx, %8lx", "%8lx, %8lx, %8lx",
                "%8lx, %8lx, %8lx, %8lx", "%8lx, %8lx, %8lx, %8lx, %8lx"
        };
        static char *failmsg[] = {
                "%s", "%-4s %8lx", "%-4s %8lx", "%-4s %8lx, %8lx"
        };
        char args[RAW_LEN+RES_LEN+ARG_LEN];
        char *tname = args;
        Event *ev;
        LONG  seqnum;

        if (pt->flags & PK_RAW)
                tname += RAW_LEN;

        tname += RES_LEN;
        memset(args, ' ', tname - args);
        mysprintf(tname, fmt[pt->numparams], dp->dp_Arg1, dp->dp_Arg2,
                                                        dp->dp_Arg3, dp->dp_Arg4, dp->dp_Arg5);

        ev = CreateEvent(calladdr, &seqnum, pt->msgid, args,
                                         ((Task *)(port->mp_SigTask))->tc_Node.ln_Name,
                                         NO_EXPAND);
        if (!ev)
                return;

        if (pt->flags & PK_RAW) {
                /*
                 *              Now fix up action name and target name
                 */
                ev->action    = ev->filename;
                ev->filename += RAW_LEN;
                mysprintf(ev->action, MSG(MSG_ACT_RAWPACKET), dp->dp_Type);
        }
        ev->result    = ev->filename;   /* Point to storage for later result */
        ev->filename += RES_LEN;
        *ev->result   = 0;                              /* Make result field empty initially */
        ev->status        = ES_UPDATING;
        /*
         *              We add the event to the waiting list BEFORE checking for Pause
         *              so that if while paused, the user decides to disable packet
         *              monitoring, the packet will be in the wait queue already and
         *              thus will be safely flushed; otherwise, it would hang around
         *              for ever (not doing any damage but wasting resources).
         */
        AddWaitingPacket(ev, seqnum, failmsg[pt->flags & PK_MASK], dp, port, pt);
        CheckForPause(ev, seqnum);
        Signal(SnoopTask, NewEventMask);
}

/*
 *              New_PutMsg()
 *
 *              This function is used frequently by the system to send a message
 *              from one task to another. It is also used by applications wishing
 *              to send ARexx messages, which is what we want to monitor.
 *
 *              The problem is: how do we distinguish system messages (of which
 *              there are lots!) from ARexx messages (which are relatively
 *              infrequent).
 *
 *              It's almost impossible to get a perfect solution to this, but we
 *              can get fairly close by checking the following:
 *
 *                - The destination port must have a name
 *                - The host name associated with the supposed Rexx message must
 *                      point to valid memory
 *        - The action type for the Rexx message is RXCOMM
 *        - The Rexx system library must be up and running
 *        - The library base must be word aligned (else we fault on a 68000)
 *        - The Close() vector in the Rexx private system library must
 *                      match the Close() vector in the Rexx library base that we
 *                      opened ourselves (the library bases themselves can be different).
 *
 *              If all these hold true, then we can assume it is a valid ARexx
 *              message and display it accordingly.
 *
 *              For DOS packets, things are a little simpler since DOS packets contain
 *              cross-links that can be easily checked for validity. To keep things
 *              moving at a reasonable speed, we only consider StandardPackets, where
 *              the DosPacket structure immediately follows the exec message structure.
 *              This is not universally true, but it covers all the cases we want to
 *              monitor.
 *
 *              We need to intercept DOS packets twice -- once on the initial PutMsg
 *              to the target device, and once when the target device PutMsg's the
 *              reply. Thus, we maintain a list of outstanding packets so we can
 *              detect which is which.
 *
 *              To identify DOS packets, we use the following criterion:
 *
 *                      - Task is a process
 *                      - Message node's name points to valid longword-aligned memory
 *                      - DOS packet (from name ptr) points back to message header
 *                      - Either current task is on the list of devices we're monitoring
 *                        or the destination message port's owner is on the list of
 *                        devices we're monitoring.
 *
 *              If any of these conditions fail, we ignore the message.
 *
 */
#ifdef __AROS__
AROS_UFH3 (ULONG, New_PutMsg,
    AROS_UFHA (struct MsgPort *, port, A0),
    AROS_UFHA (struct Message *, msg, A1),
    AROS_UFHA (void *, libbase, A6)
)
#else
ULONG ASMCALL New_PutMsg(reg_a0 struct MsgPort *port, reg_a1 struct Message *msg,
                                         reg_a6 void *libbase)
#endif
{
        MarkCallAddr;

        FP_PutMsg origfunc   = (FP_PutMsg)PatchList[GID_SENDREXX].origfunc;
        Task *thistask           = SysBase->ThisTask;

/*
 *              Save a little stack space and time by making these non-local
 */
#define rmsg                    ((struct RexxMsg *)msg)
#define rexxclosevect   (ULONG *)(((ULONG)RexxSysBase) - 10)
#define thisclosevect   (ULONG *)(((ULONG)rmsg->rm_LibBase) - 10)

        struct DosPacket *dp;
        Task  *desttask;
        Event *ev;
        LONG  seqnum;
        ULONG taskhandle;

        /*
         *              Before doing anything else, check if we're being called from
         *              within an interrupt. If so, get the heck out of here since
         *              trying to obtain a semaphore from within an interrupt is
         *              definitely a bad idea!
         *
         *              To quickly check if we're within an interrupt, we simply see
         *              if our stack pointer is within the range of the supervisor stack.
         *              If it is, then we're probably in an interrupt, or at least in
         *              Supervisor mode, and either way we don't want to continue.
         *
         *              We also ignore ramlib, for the usual reasons, and our own
         *              background task (since it is used to expand locks from within
         *              of this patch and an infinite loop would be easy!)
         */
#warning Disabled getreg() calls
#if 0
        if (thistask == RamLibTask || thistask == (Task *)BackgroundProc ||
                (getreg(REG_A7) >= (ULONG)SysBase->SysStkLower &&
                 getreg(REG_A7) <= (ULONG)SysBase->SysStkUpper))
        {
                JumpOrigFunc(0);
        }
#endif

        /*
         *              See if we have a valid ARexx message. See the function
         *              header for an explanation of all the checks
         */
        if (CurSettings.Func.Opts[GID_SENDREXX]                 &&
                port->mp_Node.ln_Name                                           &&
                (!TypeOfMem(port->mp_Node.ln_Name) || (*port->mp_Node.ln_Name)) &&
                rmsg->rm_CommAddr                                                       &&
                TypeOfMem(rmsg->rm_CommAddr)                            &&
                (rmsg->rm_Action & RXCODEMASK) == RXCOMM        &&
                RexxSysBase                                                                     &&
                TypeOfMem(thisclosevect)                                        &&
                ((ULONG)thisclosevect & 1) == 0             &&
                *thisclosevect == *rexxclosevect)
        {
                /*
                 *              Okay, we know we've got a valid Rexx message so now enter
                 *              it into the event buffer. First, however, check that we
                 *              are within our stack bounds (since the normal check done
                 *              in the assembly patch code is bypassed for this function
                 *              to accomodate RAM returning DOS packets).
                 */
#warning Disabled getreg() calls
#if 0
                if (getreg(REG_A7) >= (ULONG)thistask->tc_SPLower &&
                        getreg(REG_A7) <= (ULONG)thistask->tc_SPUpper &&
                        getreg(REG_A7) <=
                                                ((ULONG)thistask->tc_SPLower+CurSettings.StackLimit))
                {
                        JumpOrigFunc(0);
                }
#endif
                ev = CreateEvent(CallAddr, &seqnum, MSG_ACT_SENDREXX, rmsg->rm_Args[0],
                                                 port->mp_Node.ln_Name, NO_EXPAND);
                if (!ev)
                        return origfunc(port, msg, libbase);

                ev->status = ES_UPDATING;
                CheckForPause(ev, seqnum);
                ObtainSemaphore(&BufSem);
                if (seqnum >= RealFirstSeq) {
                        ev->status = ES_READY;
                        Signal(SnoopTask, NewEventMask);
                }
                ReleaseSemaphore(&BufSem);
                return origfunc(port, msg, libbase);
        }

        /*
         *              Not an ARexx message, now check if it's a DOS packet.
         *              First though, we must check to see if we're already in
         *              the middle of a SnoopDos patch -- if so, we could end up
         *              recursing forever unless we don't try and monitor this one.
         *
         *              See above for the criteria we use to spot packet messages.
         */
        taskhandle = CheckTaskRecursion();
        if (!taskhandle)
                return origfunc(port, msg, libbase);

        dp               = (struct DosPacket *)(msg->mn_Node.ln_Name);
        desttask = port->mp_SigTask;

        if ((ShowAllPackets | MonPackets)                                                       &&
                thistask->tc_Node.ln_Type == NT_PROCESS                                 &&
                ((ULONG)dp & 1) == 0                                                                    &&
                TypeOfMem(dp)                                                                                   &&
                dp->dp_Link == msg                                                                              &&
                ((ULONG)(dp->dp_Port) & 3) == 0                                                 &&
                TypeOfMem(dp->dp_Port))
        {
                /*
                 *              Looks like we're going to monitor the packet.
                 *
                 *              First, check if the packet is already on our list of
                 *              outstanding packets. If so, we can handle it immediately.
                 */
                struct PacketRef *pt;
                WaitPacket *wp;
                Task **ptask;
                char *p = NULL;

                /*
                 *              Now some slightly tricky stuff. We can have up to TWO
                 *              entries on the waiting packet list which match the current
                 *              packet (one for the Packet Debugger and one for the
                 *              Monitor Packets option) and we need to acknowledge them
                 *              both. However, we also need to ensure we never try and
                 *              lock both the PacketSem and BufferSem at the same time
                 *              (since otherwise we could deadlock).
                 *
                 *              This effectively means we have to restart our list scan
                 *              after each packet has been acknowledged, until we have
                 *              no more left -- a small speed hit, but actually not too
                 *              bad since most times, we will be the only packet on the
                 *              wait list anyway. (We use 'p' as a flag to indicate
                 *              whether or not we found a packet, so we know to return
                 *              at the end of it all.)
                 */
                ObtainSemaphore(&PacketSem);
                wp = HeadNode(&PacketWaitList);
                while (NextNode(wp)) {
                        if (wp->dp               == dp                                  &&
                                wp->sendtask != thistask                        &&
                                wp->destport != port                            &&
                                wp->arg1         == dp->dp_Arg1                 &&
                                wp->arg2         == dp->dp_Arg2                 &&
                                wp->arg3         == dp->dp_Arg3)
                        {
                                /*
                                 *              Found the packet on our list, so go ahead and fill in
                                 *              the result code for the associated event (assuming it
                                 *              hasn't scrolled off the buffer, of course!)
                                 */
                                Event *ev         = wp->event;
                                LONG   seqnum = wp->eventnum;
                                char  *fmsg   = wp->resmsg;
                                char   flags  = wp->flags;

                                Remove((Node *)wp);
                                AddHead(&PacketFreeList, (Node *)wp);
                                ReleaseSemaphore(&PacketSem);
                                ObtainSemaphore(&BufSem);
                                if (seqnum >= RealFirstSeq) {
                                        /*
                                         *              Okay, now fill in the result string. The space for
                                         *              this was allocated when the event was defined.
                                         *
                                         *              For events that succeed, we can print "Okay",
                                         *              "Okay <result>", and "Okay <result1> <result2>".
                                         *
                                         *              For events which fail, we can print
                                         *              "Fail <result2>" or "Fail <result1> <result2>"
                                         */
                                        UWORD fail;
                                        ULONG param;
                                        char  *q, *r;

#define                                   evresmsg      MSG(fail ? MSG_RES_FAIL : MSG_RES_OKAY)

                                        fail = ((flags & PKF_BOOL) && dp->dp_Res1 == 0)  ||
                                                   ((flags & PKF_NEG)  && dp->dp_Res1 == (ULONG)-1);

#if NOT_WORRIED_ABOUT_STACK_SIZE
                                        /*
                                         *              Unfortunately, we can't use this nice simple
                                         *              code because calling mysprintf() uses too much
                                         *              stack space if we're being called from certain
                                         *              handlers with minimal stack space (e.g. RAM).
                                         *              Instead, we must do the same effect ourselves.
                                         *              This code is left here so you can see what the
                                         *              original intention was.
                                         */
                                        if ((flags & PK_MASK) == PK_2OK) {
                                                mysprintf(ev->result, fmsg, evresmsg,
                                                                  dp->dp_Res1, dp->dp_Res2);
                                        } else {
                                                mysprintf(ev->result, fmsg, evresmsg,
                                                                  (fail ? dp->dp_Res2 : dp->dp_Res1));
                                        }
                                        p = (char *)-1; /* Non-zero means we did something */

#else /* Extremely worried about stack size */

                                        /*
                                         *              Here we roll our own sprintf() (inline!) to keep
                                         *              stack usage to an absolute minimum. It's a rather
                                         *              small subset and somewhat hacked, you'll notice.
                                         *
                                         *              We make P non-null to indicate we did some work
                                         */
                                        p = fmsg;
                                        q = ev->result;

                                        /*
                                         *              First, skip over string formatter at start which
                                         *              always holds either 'Ok' or 'Fail'
                                         */
                                        strcpy(q, evresmsg);
                                        q += strlen(q);
                                        while (q < (ev->result + 4))
                                                *q++ = ' ';
                                        while (*p && *p != 's')
                                                p++;
                                        if (*p)
                                                p++;

                                        /*
                                         *              Now parse remainder of string. The first parameter
                                         *              encountered gets replaced with either dp_Res1 or
                                         *              dp_Res2, depending on circumstances. The second
                                         *              parameter always gets replaced with dp_Res2.
                                         */
                                        if ((flags & PK_MASK) == PK_2OK || !fail)
                                                param = dp->dp_Res1;
                                        else
                                                param = dp->dp_Res2;

                                        while (*p) {
                                                if (*p != '%') {
                                                        *q++ = *p++;
                                                        continue;
                                                }
                                                while (*p && *p != 'x' && *p != 'X')
                                                        p++;
                                                p++;
                                                if (param == 0) {
                                                        strcpy(q, "       0");
                                                } else if (param == (ULONG)-1) {
                                                        strcpy(q, "      -1");
                                                } else {
                                                        /*
                                                         *              Expand param to 8 hex digits, right-aligned
                                                         */
                                                        r = q + 7;
                                                        while (param > 0) {
                                                                *r-- = "0123456789ABCDEF"[param & 15];
                                                                param >>= 4;
                                                        }
                                                        while (r >= q)
                                                                *r-- = ' ';
                                                }
                                                q += 8;
                                                param = dp->dp_Res2; /* Second parm always dp_Res2 */
                                        }
                                        *q = '\0';
#endif
                                        /*
                                         *              Unlike most times, we don't signal SnoopDos that
                                         *              an event has been updated (yet) since there may
                                         *              be another event to update too, and it's best
                                         *              to do a single signal at the end (fewer context
                                         *              switches are required.) So, we just mark the
                                         *              event as ready.
                                         */
                                        ev->status = ES_READY;
                                }
                                ReleaseSemaphore(&BufSem);
                                ObtainSemaphore(&PacketSem);
                                /*
                                 *              Now restart the search from the beginning of the
                                 *              list (since wp is no longer valid)
                                 */
                                wp = HeadNode(&PacketWaitList);
                        } else {
                                /*
                                 *              Just advance to next element on list
                                 */
                                wp = NextNode(wp);
                        }
                }
                ReleaseSemaphore(&PacketSem);
                if (p) {
                        /*
                         *              Found a matching packet, so wake up the main task
                         *              and finish up. (We only signal the main snoop task
                         *              now, so that both events get printed simultaneously,
                         *              rather than one after another, which would be a little
                         *              messy). This also avoids us getting pre-empted inside
                         *              our loop above if SnoopDos is running at a higher priority
                         *              than this task, which is convenient (though not essential).
                         */
                        Signal(SnoopTask, NewEventMask);
                        goto done_putmsg;
                }

                /*
                 *              If we didn't find a match on our list of waiting packets,
                 *              then check to see if we're ignoring ROM calls. If so,
                 *              and if we were called from ROM, then immediately abort
                 *              (this would be done later anyway, but by checking now,
                 *              we save quite a bit of time.)
                 *
                 *              (We can't do this checking any earlier, or we'd miss
                 *              returned packets from ROM filesystems like RAM: and FFS).
                 *
                 *              We also exit if we're under the stack limit. (We can't
                 *              check this earlier because otherwise we'd miss the
                 *              return packet from RAM which only has a tiny stack.)
                 */
#warning Disabled getreg() calls
#if 0
                if (((!MonROMCalls && CallAddr >= RomStart && CallAddr <= RomEnd)) ||
                        (getreg(REG_A7) >= (ULONG)thistask->tc_SPLower &&
                         getreg(REG_A7) <= (ULONG)thistask->tc_SPUpper &&
                         getreg(REG_A7) <=
                                                ((ULONG)thistask->tc_SPLower+CurSettings.StackLimit)))
                {
                        goto done_putmsg;
                }
#endif
                /*
                 *              Okay, didn't match the packet with a previous outgoing one.
                 *              Now search the device list to see if we can match either
                 *              the owner of the port (a new outgoing packet) or the task ID
                 *              of the sender (a returning packet).
                 *
                 *              In the case of a returning packet, we just ignore it since
                 *              we missed it on the way out.
                 */
                for (ptask = DeviceTaskList; *ptask && *ptask != desttask; ptask++) {
                        if (*ptask == thistask)
                                goto done_putmsg;       /* Sender was a DOS device so ignore */
                }
                if (!*ptask) {
                        /*
                         *              Couldn't find dest task on device list so it must
                         *              be a message to somewhere else -- hence, ignore it.
                         */
                        goto done_putmsg;
                }

                /*
                 *              Okay, got ourselves a bona fida packet. Next, look it up
                 *              in our packet table to decide what to do with it.
                 *              Unrecognised packets come out as LAST_PACK_MSG which is
                 *              specifically initialised to handle them properly.
                 */
                for (pt = &PacketTable[0]; pt->msgid != LAST_PACK_MSG; pt++)
                        if (pt->packetid == dp->dp_Type)
                                break;

                if (pt->flags & PK_IGNORE)      /* Certain packet types are ignored */
                        goto done_putmsg;

                /*
                 *              Okay, we finally (finally!) get to create a new entry
                 *              in our event buffer. Actually, we get to create two
                 *              entries, since if we have both the Packet Debugger
                 *              AND the Monitor Packets option turned on, we want to
                 *              see the output from both of them (even if they're both
                 *              the same event!).
                 *
                 *              Why? Well, if you're debugging a device driver and you
                 *              have Open packets etc. coming back and forth, then
                 *              as well as the raw packet data, it's also kind of nice
                 *              to see actual filenames etc. in english instead of hex.
                 */
                if (MonPackets && (pt->flags & PK_COMMON))
                        HandleSimplePacket(CallAddr, dp, port, pt);

                if (ShowAllPackets)
                        HandleRawPacket(CallAddr, dp, port, pt);
        }

done_putmsg:
        EndTaskRecursion(taskhandle);
        return origfunc(port, msg, libbase);
}

