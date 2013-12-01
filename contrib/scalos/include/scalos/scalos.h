#ifndef SCALOS_SCALOS_H
#define SCALOS_SCALOS_H
/*
**  $VER: scalos.h 41.9 (13. Mai 2010 15:49:13)
**
** $Date$
** $Revision$
**
**	Scalos.library include
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 2000-2010 The Scalos Team
**  All Rights Reserved
**
**
** next Tag to use :	(SCC_Dummy+226)
*/

#ifndef DOS_DOS_H
#include    <dos/dos.h>
#endif

#ifndef DOS_NOTIFY_H
#include    <dos/notify.h>
#endif

#ifndef EXEC_LISTS_H
#include    <exec/lists.h>
#endif

#ifndef EXEC_PORTS_H
#include    <exec/ports.h>
#endif

#ifndef INTUITION_CLASSES_H
#include    <intuition/classes.h>
#endif

#ifndef INTUITION_CLASSUSR_H
#include    <intuition/classusr.h>
#endif

#ifndef DATATYPES_ICONOBJECT_H
#include    <datatypes/iconobject.h>
#endif

#ifndef REXX_STORAGE_H
#include <rexx/storage.h>
#endif

#ifndef WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif

#ifndef DEVICES_TIMER_H
#include <devices/timer.h>
#endif

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif

#ifndef GRAPHICS_REGIONS_H
#include <graphics/regions.h>
#endif

#
#ifndef SCALOS_SCALOSGFX_H
#include <scalos/scalosgfx.h>
#endif

#ifndef SCALOS_UNDO_H
#include <scalos/undo.h>
#endif

#ifndef SCALOS_INT64TYPES_H
#include <scalos/int64types.h>
#endif

#define SCALOSNAME "scalos.library"

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

#if !defined(SCALOSSEMAPHORE)
#define SCALOSSEMAPHORE	       struct SignalSemaphore
#endif /* SCALOSSEMAPHORE */

#if !defined(TIMEVAL_TYPE_DEFINED)
#define TIMEVAL_TYPE_DEFINED
#if defined(__NEW_TIMEVAL_DEFINITION_USED__)
	typedef struct TimeVal  	T_TIMEVAL;
	typedef struct TimeRequest	T_TIMEREQUEST;
	#define	tr_node			Request
	#define	tr_time			Time
	#define	tv_secs			Seconds
	#define	tv_micro		Microseconds
#else //defined(__amigaos4__) && defined(__NEW_TIMEVAL_DEFINITION_USED__)
	typedef struct timeval  	T_TIMEVAL;
	typedef struct timerequest	T_TIMEREQUEST;
#endif //defined(__NEW_TIMEVAL_DEFINITION_USED__)
#endif //!defined(TIMEVAL_TYPE_DEFINED)

// ---------------------------------------------------------------------------
// forward declarations

struct internalScaWindowTask;

// ---------------------------------------------------------------------------

#define	SCA_TagBase                     0x80530000

#define SCA_IconObject                  (SCA_TagBase+1)		// Object * - IconObject to use when opening a drawer
#define SCA_Stacksize			(SCA_TagBase+2)
#define SCA_Flags			(SCA_TagBase+3)
#define SCA_IconNode			(SCA_TagBase+4)
#define SCA_WindowTitle			(SCA_TagBase+5)
#define SCA_Path			(SCA_TagBase+6)
#define SCA_WindowRect			(SCA_TagBase+7)
#define SCA_XOffset			(SCA_TagBase+8)
#define SCA_YOffset			(SCA_TagBase+9)
#define SCA_PatternNumber		(SCA_TagBase+10)	// see pattern.h
#define SCA_ShowAllFiles		(SCA_TagBase+11)	// obsolete, use SCA_ShowAllMode instead!
#define SCA_ViewModes			(SCA_TagBase+12)
#define SCA_MessagePort			(SCA_TagBase+13)
#define SCA_Iconify			(SCA_TagBase+14)
#define SCA_ClassName			(SCA_TagBase+15)
#define SCA_Class			(SCA_TagBase+16)
#define SCA_WaitTimeout			(SCA_TagBase+17)	// +jl+ 20010324
#define SCA_SemaphoreExclusive		(SCA_TagBase+18)	// +jl+ 20010408
#define SCA_SemaphoreShared		(SCA_TagBase+19)	// +jl+ 20010408
#define	SCA_NoStatusBar			(SCA_TagBase+20)	// +jl+ 20011111
#define	SCA_NoActivateWindow		(SCA_TagBase+21)	// +jl+ 20020512
#define	SCA_Priority			(SCA_TagBase+22)	// +jl+ 20020823
#define	SCA_WindowStruct		(SCA_TagBase+23)	// +jl+ 20021204
#define SCA_SemaphoreExclusiveNoNest	(SCA_TagBase+24)	// +jl+ 20040904
#define SCA_ShowAllMode			(SCA_TagBase+25)	// one of the workbench DDFLAGS_*** values
#define SCA_IconList			(SCA_TagBase+26)	// struct ScaIconNode *
#define	SCA_BrowserMode			(SCA_TagBase+27)	// ULONG
//#define SCA_HideStatusBar		(SCA_TagBase+28)	// ULONG ***obsolete, use SCA_NoStatusBar instead***
#define	SCA_CheckOverlappingIcons	(SCA_TagBase+29)	// ULONG
#define SCA_TransparencyActive		(SCA_TagBase+30)	// ULONG - percentage of opacity in active window state, 0=transparent, 100=opaque
#define SCA_TransparencyInactive	(SCA_TagBase+31)	// ULONG - percentage of opacity in inactive window state, 0=transparent, 100=opaque
#define SCA_DdPopupWindow		(SCA_TagBase+32)	// ULONG - Flag: this window has popped up during D&D operation
#define SCA_NoControlBar 		(SCA_TagBase+33)	// don't display control bar for this window

#define SCAB_WBStart_NoIcon		0
#define SCAB_WBStart_Wait		1
#define SCAB_WBStart_PathSearch		2
#define SCAB_WBStart_NoIconCheck	3

#define SCAF_WBStart_NoIcon		(1L << SCAB_WBStart_NoIcon)
#define SCAF_WBStart_Wait		(1L << SCAB_WBStart_Wait)
#define SCAF_WBStart_PathSearch		(1L << SCAB_WBStart_PathSearch)
#define SCAF_WBStart_NoIconCheck	(1L << SCAB_WBStart_NoIconCheck)

#define SCAV_WBStart_NoIcon	 	SCAF_WBStart_NoIcon
#define SCAV_WBStart_Wait	 	SCAF_WBStart_Wait
#define SCAV_WBStart_PathSearch	 	SCAF_WBStart_PathSearch
#define SCAV_WBStart_NoIconCheck 	SCAF_WBStart_NoIconCheck

#define SCAB_OpenWindow_ScalosPort	0		// private
#define SCAF_OpenWindow_ScalosPort	(1 << SCAB_OpenWindow_ScalosPort)

// LockWindowList():
#define SCA_LockWindowList_Shared   		0
#define SCA_LockWindowList_Exclusiv 		1
#define SCA_LockWindowList_AttemptShared   	2
#define SCA_LockWindowList_AttemptExclusive	3

// FreeWBArgs():
#define SCAV_FreeLocks		    1
#define SCAB_FreeLocks		0
#define SCAF_FreeLocks		(1 << SCAB_FreeLocks)
#define SCAV_FreeNames		    2
#define SCAB_FreeNames		1
#define SCAF_FreeNames		(1 << SCAB_FreeNames)

#define SCA_SortType_Bubble	    0
#define SCA_SortType_Selection	    1
#define SCA_SortType_Insertion	    2
#define SCA_SortType_Quick	    3
#define SCA_SortType_Best	    -1

// window dropmark types
enum WindowDropMarkTypes 
	{ 
	IDTV_WinDropMark_Never, 		// never show any window dropmarks
	IDTV_WinDropMark_WindowedOnly, 		// non-backdrop windows only
	IDTV_WinDropMark_Always 		// always show window dropmarks
	};

// ---------------------- NodeTypes for AllocStdNode() ------------------------

#define NTYP_IconNode		    0
#define NTYP_WindowStruct	    1
#define NTYP_DragNode		    2
#define NTYP_PatternNode	    3
#define NTYP_ScalosArg		    4
#define NTYP_DeviceIcon		    5
#define NTYP_AppObject		    8
#define NTYP_MenuInfo		    9
#define NTYP_PluginClass	    10		// +jl+
#define NTYP_PluginNode		    11		// +jl+

// ------------------------ MessageTypes for AllocMsg() -----------------------

#define MTYP_DropProc			1
#define MTYP_CloseWindow		2
#define MTYP_StartWindow		3
#define MTYP_Timer			4
#define MTYP_Requester			5
#define MTYP_NewPattern			6
#define MTYP_Sleep			7
#define MTYP_Wakeup			8
#define MTYP_AsyncRoutine		9
#define MTYP_RunProcess			10
#define MTYP_AppSleep			11
#define MTYP_AppWakeup			12
#define MTYP_Redraw			13
#define MTYP_Update			14
#define MTYP_UpdateIcon			15
#define MTYP_AddIcon			16
#define MTYP_RemIcon			17
#define MTYP_ARexxRoutine		18
#define MTYP_Iconify			19
#define MTYP_UnIconify			20
#define MTYP_AsyncBackFill		21
#define MTYP_ShowTitle			22
#define MTYP_RunMenuCmd			23	// +jl+ 20010415
#define MTYP_ShowPopupMenu		24	// +jl+ 20011209
#define MTYP_ShowStatusBar		25	// +jl+ 20040919
#define MTYP_RedrawIcon			26	// +jl+ 20050303
#define MTYP_DoPopupMenu		27	// +jl+ 20050319
#define MTYP_RedrawIconObj		28
#define MTYP_NewPreferences		29
#define MTYP_DeltaMove			30
#define MTYP_SetThumbnailImage_Remapped	31
#define MTYP_SetThumbnailImage_ARGB     32
#define MTYP_NewWindowPath		33
#define MTYP_PrefsChanged		34
#define MTYP_StartChildProcess		35
#define MTYP_RootEvent			36
#define MTYP_ShowControlBar		37

#define	MTYP_MAX			38	// must always be 1 larger than last MTYP_??5

// ---------------------------------------------------------------------------

enum ScanDirResult
	{
	SCANDIR_OK,				// everything worked, continue
	SCANDIR_FAIL_ABORT,			// fatal error, abort
	SCANDIR_FAIL_RETRY,			// maybe recoverable error, continue
	SCANDIR_EXALL_FAIL,			// ExAll failed, retry with Examine
	SCANDIR_EXALL_BADNUMBER,		// ExAll failed, retry with lower rilc_ExAllType
	SCANDIR_ABORT,				// externally aborted
	SCANDIR_FINISHED,			// dir scan finished, end
	SCANDIR_VIEWMODE_CHANGE,		// window view mode changed
	SCANDIR_WINDOW_CLOSING,			// Window is about to be closed
	};

enum ScalosSortOrder
{
	SortOrder_Default,
	SortOrder_Ascending,
	SortOrder_Descending
};

struct ScaWindowList
{
	struct ScaWindowStruct	*wl_WindowStruct;	// Pointer to the first windowstruct
	struct ScaWindowStruct	*ml_MainWindowStruct;	// Pointer into the list of windowstructs
	struct ScaWindowStruct	*ml_AppWindowStruct;	// dito - windowstruct for AppIcons
};

struct ScaWindowStruct
{
	struct MinNode		ws_Node;
	struct Process		*ws_Task;		// Window-Task address
	BPTR			ws_Lock;		// filelock to the windows directory
	struct Window		*ws_Window;		// pointer to the window or NULL
	struct MsgPort		*ws_MessagePort;	// the API-messageport
	WORD			ws_Left;
	WORD			ws_Top;
	WORD			ws_Width;
	WORD			ws_Height;		// size and position of the window
	WORD			ws_xoffset;
	WORD			ws_yoffset;		// the virtual position inside
	APTR			ws_PatternNode;
	struct ScaWindowTask 	*ws_WindowTask;		// windowtask structure
	STRPTR			ws_Name;		// windows name (is displayed in the title)
	STRPTR			ws_Title;		// title-formatstring
	STRPTR			ms_ClassName;		// scalos class name
	Class			*ms_Class;		// BOOPSI Class if ms_classname is NULL
	UWORD			ws_Flags;		// see below
	WORD			ws_PatternNumber;	// number of the pattern
	UBYTE			ws_Viewmodes;		// see below
	UBYTE			ws_WindowType;		// see below
	STRPTR			ws_WindowTaskName;	// name of window task +jl+ 20011211
	UWORD			ws_ViewAll;             // DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
	ULONG			ws_ThumbnailView;       // THUMBNAILS_Never, THUMBNAILS_Always, THUMBNAILS_AsDefault
	ULONG			ws_ThumbnailsLifetimeDays;     //Maximum lifetime for generated thumbnails
	enum ScalosSortOrder	ws_SortOrder;		// current sorting or ascending or descending
	UWORD			ws_WindowOpacityActive;	// percentage of active Scalos Window transparency - not used on all platforms!
							// (0=PREFS_TRANSPARENCY_TRANSPARENT=invisible, 100=PREFS_TRANSPARENCY_OPAQUE=opaque)
	UWORD			ws_WindowOpacityInactive;	// percentage of inactive Scalos Window transparency - not used on all platforms!
	struct Rectangle 	ws_IconSizeConstraints;	// size limits for icons, larger or smaller icons are scaled
	UWORD			ws_IconScaleFactor;	// icon scaling factor in percent
	ULONG			ws_MoreFlags;		// various flags, see below
};

// ws_Flags:
#define WSV_FlagB_RootWindow		0		// a RootDir window
#define WSV_FlagF_RootWindow		(1 << WSV_FlagB_RootWindow)
#define WSV_FlagB_NeedsTimerMsg		1		// window likes to get TimerMessages
#define WSV_FlagF_NeedsTimerMsg		(1 << WSV_FlagB_NeedsTimerMsg)
#define WSV_FlagB_Backdrop		2		// is the window in backdrop mode
#define WSV_FlagF_Backdrop		(1 << WSV_FlagB_Backdrop)
#define WSV_FlagB_TimerMsgSent		3		// timermsg already sent *PRIVATE*
#define WSV_FlagF_TimerMsgSent		(1 << WSV_FlagB_TimerMsgSent)
#define WSV_FlagB_TaskSleeps		4		// windowtask is sleeping
#define WSV_FlagF_TaskSleeps		(1 << WSV_FlagB_TaskSleeps)
#define WSV_FlagB_ShowAllFiles		5		// just what it said
#define WSV_FlagF_ShowAllFiles		(1 << WSV_FlagB_ShowAllFiles)
#define WSV_FlagB_Iconify		6		// windowtask is in iconify state
#define WSV_FlagF_Iconify		(1 << WSV_FlagB_Iconify)
#define WSV_FlagB_CheckUpdatePending	7		// IconWinCheckUpdate() is pending *PRIVATE*
#define WSV_FlagF_CheckUpdatePending	(1 << WSV_FlagB_CheckUpdatePending)
#define	WSV_FlagB_ActivatePending	8		// ActivateWindow() has been issued *PRIVATE*
#define	WSV_FlagF_ActivatePending	(1 << WSV_FlagB_ActivatePending)
#define	WSV_FlagB_Typing		9		// typing name for keyboard icon selection *PRIVATE*
#define	WSV_FlagF_Typing		(1L << WSV_FlagB_Typing)
#define	WSV_FlagB_NoStatusBar		10		// don't display status bar for this window
#define	WSV_FlagF_NoStatusBar		(1L << WSV_FlagB_NoStatusBar)
#define	WSV_FlagB_NoActivateWindow	11		// don't activate window on OpenWindow()
#define	WSV_FlagF_NoActivateWindow	(1L << WSV_FlagB_NoActivateWindow)
#define WSV_FlagB_RefreshPending	12		// IDCMP_REFRESHWINDOW could not be processed, refresh is pending *PRIVATE*
#define WSV_FlagF_RefreshPending	(1 << WSV_FlagB_RefreshPending)
#define WSV_FlagB_BrowserMode		13		// enable single-window browser mode
#define WSV_FlagF_BrowserMode		(1 << WSV_FlagB_BrowserMode)
#define WSV_FlagB_CheckOverlappingIcons	14		// prevent icons from overlapping each other
#define WSV_FlagF_CheckOverlappingIcons	(1 << WSV_FlagB_CheckOverlappingIcons)
#define WSV_FlagB_DdPopupWindow 	15		// this window has popped up during a D&D operation
#define WSV_FlagF_DdPopupWindow		(1 << WSV_FlagB_DdPopupWindow)

// ws_MoreFlags
#define	WSV_MoreFlagB_NoControlBar	0	       // don't display control bar for this window
#define	WSV_MoreFlagF_NoControlBar	(1L << WSV_MoreFlagB_NoControlBar)

// ws_WindowType:
#define WSV_Type_IconWindow	0		// Window filled with icons
#define WSV_Type_DeviceWindow	1
#define WSV_Type_DesktopWindow	WSV_Type_DeviceWindow	// synonym for WSV_Type_DeviceWindow

// ws_Viewmodes
#define SCAV_ViewModes_Default          IDTV_ViewModes_Default
#define SCAV_ViewModes_Icon		IDTV_ViewModes_Icon
#define SCAV_ViewModes_Name		IDTV_ViewModes_Name
#define SCAV_ViewModes_Size		IDTV_ViewModes_Size
#define SCAV_ViewModes_Date		IDTV_ViewModes_Date
#define SCAV_ViewModes_Time		IDTV_ViewModes_Time
#define SCAV_ViewModes_Comment		IDTV_ViewModes_Comment
#define SCAV_ViewModes_Protection	IDTV_ViewModes_Protection
#define SCAV_ViewModes_Owner		IDTV_ViewModes_Owner
#define SCAV_ViewModes_Group		IDTV_ViewModes_Group
#define SCAV_ViewModes_Type 		IDTV_ViewModes_Type
#define SCAV_ViewModes_Version		IDTV_ViewModes_Version
#define SCAV_ViewModes_MiniIcon		IDTV_ViewModes_MiniIcon

// for compatibility +jl+
#define SIV_IconWin		WSV_Type_IconWindow
#define SIV_DeviceWin		WSV_Type_DeviceWindow

// ------------------------------------------------------------------

struct PatternInfo
	{
	struct	Hook ptinf_hook;		//Backfill Hook
	WORD	ptinf_width;			//Width
	WORD	ptinf_height;			//and Height of Bitmap in Pixels
	struct	BitMap *ptinf_bitmap;		//Bitmap or NULL
	APTR	ptinf_pattern;			//Pattern Array or NULL
	UWORD	ptinf_flags;			//see below
	WORD	ptinf_pad;			//padword
	LONG	ptinf_BgPen;			//Allocated pen for background pattern
	struct	Message *ptinf_message;		//Message
	};

// ------------------------------------------------------------------

struct ScaWindowTask {
	struct PatternInfo	wt_PatternInfo;
	WORD			wt_XOffset;
	WORD			wt_YOffset;		// Innerwindow offset
	APTR			mt_MainObject;		// main ScalosClass object
	APTR			mt_WindowObject;	// "Window.sca" object
	struct Window		*wt_Window;		// pointer to the window of the task or NULL
	struct ScaIconNode	*wt_IconList;		// Pointer to the first IconNode
	struct ScaIconNode	*wt_LateIconList;	// Pointer to the first IconNode of all not yet
							// displayed icons (e.g. non-position icon while
							// loading)
	SCALOSSEMAPHORE		*wt_IconSemaphore;	// Semaphore to access the wt_IconList/wt_LateIconList
							// It *MUST* be locked before accessing the list.
							// Shared lock should be used for read-only access
	struct MsgPort		*wt_IconPort;		// MessagePort of the windowtask
	SCALOSSEMAPHORE		*wt_WindowSemaphore;	// Semaphore to avoid the task from closing
	struct ScaWindowStruct	*mt_WindowStruct;	// Pointer to the WindowStruct of this task
};							// It's only the documented end of this structure.
							// In reality it's much bigger.
// ------------------------------------------------------------------

// some structure forward declarations
struct SM_RunProcess;

// Function pointer definitions
// for some SM_  messages

typedef void (*ASYNCROUTINEFUNC)(struct ScaWindowTask *, APTR);
typedef void (*AREXXFUNC)(struct ScaWindowTask *, struct RexxMsg *);
typedef ULONG (*RUNPROCFUNC)(APTR, struct SM_RunProcess *);

// ------------------------------------------------------------------

#define	ID_IMSG		MAKE_ID('I','M','S','G')

struct ScalosMessage
	{
	struct Message		sm_Message;
	ULONG			sm_Signature;		// ID_IMSG
	ULONG			sm_MessageType;		// SCA_AllocMessage() type
	};

struct UpdateIconData
	{
	struct WBArg uid_WBArg;
	ULONG uid_IconType;
	};

struct SM_CloseWindow
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_StartWindow 
	{
	struct ScalosMessage	ScalosMessage;
	struct ScaWindowStruct  *WindowStruct;
	};

struct SM_Timer
	{
	struct ScalosMessage	ScalosMessage;
	struct ScaWindowStruct	*smtm_WindowStruct;
	T_TIMEVAL		smtm_Time;
	};

struct SM_NewPattern 
	{
	struct ScalosMessage	ScalosMessage;
	APTR			smnp_PatternNode;	// PatternNode or NULL
	};

struct SM_Sleep 
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_Wakeup 
	{
	struct ScalosMessage	ScalosMessage;
	ULONG			smwu_ReLayout;	    // BOOL
	};

struct SM_ShowTitle 
	{
	struct ScalosMessage	ScalosMessage;
	ULONG			smst_showTitle;	    // BOOL
	};

struct SM_AsyncRoutine 
	{
	struct ScalosMessage	ScalosMessage;
	ASYNCROUTINEFUNC	smar_EntryPoint;/* address of start of code to execute */
						// code will be executed with pointer to ScaWindowTask in A5
						// and pointer to smar_Args in A0
						// so, Message can easily be extended to pass arguments.
						// Result from routine will be placed in EntryPoint
	UBYTE			smar_Args[0];
	};

struct SM_RunProcess 
	{
	struct ScalosMessage	ScalosMessage;
	struct ScaWindowTask	*WindowTask;
	RUNPROCFUNC 		EntryPoint;	// address of start of code to execute
						// EntryPoint gets called with address of <Flags>
						// and pointer to SM_RunProcess message

	ULONG			Flags;		// dunno, but it's long and it's used
	// might have additional space here, depending on RunProcess() "NumLongs" parameter.
	};


struct SM_AppSleep 
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_AppWakeup 
	{
	struct ScalosMessage	ScalosMessage;
	ULONG			smaw_ReLayout;	    // BOOL
	};

struct SM_Redraw 
	{
	struct ScalosMessage	ScalosMessage;
	ULONG			smmr_Flags;	    // see below
	};

struct SM_Update 
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_UpdateIcon 
	{
	struct ScalosMessage	ScalosMessage;
	BPTR			smui_DirLock;
	CONST_STRPTR		smui_IconName;
	ULONG			smui_IconType;	// indication for icon type, if known
						// like WBDRAWER, leave at 0 if unknown
	};

struct SM_AddIcon 
	{
	struct ScalosMessage	ScalosMessage;
	UWORD			smai_x;
	UWORD			smai_y;
	BPTR			smai_DirLock;	/* +jl+ 20010217 */
	STRPTR			smai_IconName;	/* +jl+ 20010217 */
	};

struct SM_RemIcon 
	{
	struct ScalosMessage	ScalosMessage;
	BPTR			smri_DirLock;	/* +jl+ 20010217 */
	STRPTR			smri_IconName;	/* +jl+ 20010217 */
	};

struct SM_Iconify 
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_UnIconify 
	{
	struct ScalosMessage	ScalosMessage;
	};

struct SM_Requester 
	{
	struct ScalosMessage	ScalosMessage;
	union	{
		ULONG 		smrq_ReqResult;		// Result from EasyRequest()
		struct	Window 	*smrq_ParentWindow;
		} smrq_MultiPurpose;
	APTR			smrq_ArgList;		// pointer to ArgList
	struct	EasyStruct 	smrq_ez;
	UBYTE			smrq_ArgListBuffer[0];	// buffer for ArgList - allocated as large as needed
	};

struct SM_AsyncBackFill
	{
	struct ScalosMessage	ScalosMessage;
	RUNPROCFUNC		smab_BackfillFunc;	// Backfill function entry point
	struct	PatternNode 	*smab_PatternNode;
	struct	PatternInfo 	*smab_PatternInfo;
	struct	Screen 		*smab_Screen;
	struct	ScaWindowTask	*smab_WindowTask;
	UBYTE			smab_PatternInfoCopy[0];
	};

struct SM_ARexxRoutine
	{
	struct ScalosMessage	ScalosMessage;
	AREXXFUNC 		smrx_EntryPoint;
	struct	RexxMsg 	*smrx_RexxMsg;
	};

struct SM_MenuCmd
	{
	struct SM_AsyncRoutine smc_AsyncRoutine;
	ULONG			smc_Flags;
	struct ScaIconNode	*smc_IconNode;
	};

struct SM_RunMenuCmd
	{
	struct ScalosMessage    ScalosMessage;
	struct ScalosMenuTree	*smrm_MenuItem;
	struct ScaIconNode	*smrm_IconNode;
	ULONG			smrm_Flags;
	};

struct SM_RealUpdateIcon
	{
	struct SM_AsyncRoutine rui_AsyncRoutine;
	struct UpdateIconData rui_Args;
	};

struct SM_ShowPopupMenu
	{
	struct ScalosMessage    ScalosMessage;
	struct PopupMenu *smpm_PopupMenu;
	struct ScaIconNode *smpm_IconNode;
	ULONG smpm_Flags;
	struct FileTypeDef *smpm_FileType;
	UWORD smpm_Qualifier;		// the smpm_Qualifier field is a copy of the current IntuiMessage's Qualifier
	};

// values in smpm_Flags - same as in mpm_Flags

struct SM_ShowStatusBar
	{
	struct ScalosMessage    ScalosMessage;
	BOOL smsb_Visible;
	};

struct SM_ShowControlBar
	{
	struct ScalosMessage    ScalosMessage;
	BOOL smcb_Visible;
	};

struct SM_RedrawIcon
	{
	struct ScalosMessage    ScalosMessage;
	struct ScaIconNode 	*smri_Icon;
	};

struct SM_DoPopupMenu
	{
	struct ScalosMessage    ScalosMessage;
	struct InputEvent	smdpm_InputEvent;
	};

struct SM_RedrawIconObj
	{
	struct ScalosMessage    ScalosMessage;
	Object			*smrio_IconObject;
	ULONG			smrio_Flags;
	};

// values in smrio_Flags
#define SMRIOFLAGB_EraseIcon		0
#define SMRIOFLAGB_FreeLayout		1
#define SMRIOFLAGB_Highlight		2		// set this flag to indicate that there is only a change in icon highlighting state
#define SMRIOFLAGB_IconListLocked	3		// flag: icon list is currently locked
#define SMRIOFLAGB_HightlightOn		4		// flag: set ICONOBJ_USERFLAGF_DrawHighlite before drawing
#define SMRIOFLAGB_HightlightOff	5		// flag: clear ICONOBJ_USERFLAGF_DrawHighlite before drawing
#define SMRIOFLAGF_EraseIcon		(1L << SMRIOFLAGB_EraseIcon)
#define SMRIOFLAGF_FreeLayout		(1L << SMRIOFLAGB_FreeLayout)
#define SMRIOFLAGF_Highlight		(1L << SMRIOFLAGB_Highlight)
#define SMRIOFLAGF_IconListLocked	(1L << SMRIOFLAGB_IconListLocked)
#define	SMRIOFLAGF_HightlightOn		(1L << SMRIOFLAGB_HightlightOn)
#define	SMRIOFLAGF_HightlightOff	(1L << SMRIOFLAGB_HightlightOff)

struct SM_NewPreferences
	{
	struct ScalosMessage    ScalosMessage;
	ULONG			smnp_PrefsFlags;
	};

// values in smnp_PrefsFlags
// one bit is set for each changed subsystem prefs
#define SMNPFLAGB_PATTERNPREFS		0
#define SMNPFLAGB_MENUPREFS		1
#define SMNPFLAGB_FONTPREFS		2
#define SMNPFLAGB_SCALOSPREFS		3
#define SMNPFLAGB_PALETTEPREFS		4
#define SMNPFLAGB_DEFICONSPREFS		5
#define SMNPFLAGB_LOCALEPREFS		6
#define SMNPFLAGB_FILETYPESPREFS	7

#define SMNPFLAGF_PATTERNPREFS		(1L << SMNPFLAGB_PATTERNPREFS)
#define SMNPFLAGF_MENUPREFS             (1L << SMNPFLAGB_MENUPREFS)
#define SMNPFLAGF_FONTPREFS             (1L << SMNPFLAGB_FONTPREFS)
#define SMNPFLAGF_SCALOSPREFS           (1L << SMNPFLAGB_SCALOSPREFS)
#define SMNPFLAGF_PALETTEPREFS          (1L << SMNPFLAGB_PALETTEPREFS)
#define SMNPFLAGF_DEFICONSPREFS         (1L << SMNPFLAGB_DEFICONSPREFS)
#define SMNPFLAGF_LOCALEPREFS           (1L << SMNPFLAGB_LOCALEPREFS)
#define SMNPFLAGF_FILETYPESPREFS        (1L << SMNPFLAGB_FILETYPESPREFS)

struct SM_DeltaMove
	{
	struct ScalosMessage    ScalosMessage;
	LONG smdm_DeltaX;
	LONG smdm_DeltaY;
	ULONG smdm_AdjustSlider;
	};

struct SM_SetThumbnailImage_Remapped
	{
	struct ScalosMessage    ScalosMessage;
	Object	*smtir_IconObject;			// the icon to attach the new image to
	struct ScalosBitMapAndColor *smtir_NewImage;    // the new image. ScalosBitMapAndColor contents is freed by message handler
	};

struct SM_SetThumbnailImage_ARGB
	{
	struct ScalosMessage    ScalosMessage;
	Object	*smtia_IconObject;			// the icon to attach the new image to
	struct ARGBHeader smtia_NewImage;		// the new image. ARGBHeader contents is freed by message handler
	};

struct SM_NewWindowPath
	{
	struct ScalosMessage    ScalosMessage;
	STRPTR smnwp_Path;				// Path of the new drawer to display in the window
	struct TagItem *smnwp_TagList;
	};

struct SM_PrefsChanged
	{
	struct ScalosMessage    ScalosMessage;
	ULONG smpc_Flags;
	};

struct SM_StartChildProcess
	{
	struct ScalosMessage    ScalosMessage;
	struct ScaWindowTask	*smscp_WindowTask;
	};

struct SM_RootEvent
	{
	struct ScalosMessage    ScalosMessage;
	ULONG 			smre_MethodID;		// the MethodID of the event
	APTR 			smre_EventHandle;	// the handle that had been returned by SCCM_AddListener
	Class 			*smre_Class;		// Class variable of the method call
	Object 			*smre_Object;		// Object variable of the method call
	Msg 			smre_Message;		// msg variable of the method call - might no longer be valid when event is received!
	};

// ------------------------------------------------------------------

struct ScaBackdropIcon
	{
	struct MinNode		sbi_Node;
	Object			*sbi_Icon;		// IconObject from iconobject.library
	struct NotifyRequest	*sbi_NotifyReq;
	STRPTR			sbi_DrawerName;		// full name of parent directory
	};


// This structure WILL grow in the future. Do not rely on its size!
// Do NOT ALLOCATE ScaIconNode's yourself!
// Do NOT MODIFY any member on your own !
struct ScaIconNode 
{
	struct MinNode		in_Node;
	Object			*in_Icon;		// IconObject from iconobject.library
	STRPTR			in_Name;		// Filename of the icon
	struct ScaDeviceIcon	*in_DeviceIcon;		// NULL except for disk icons
	BPTR			in_Lock;		// for backdrop (left-out) icons, this lock
							// points to the original icon dir.
	struct ScaBackdropIcon	*in_IconList;		// list of linked icons (used for backdrop icons)
							// Contains a list of ad hoc created ScaBackdropIcon's where 
							// each in_Icon points to in_Icon of the corresponding
							// "left-out" icon.
	UWORD			in_Flags;		// see below
	UWORD			in_Userdata;
	struct DateStamp	in_DateStamp;		// +jl+ datestamp when the icon was read
	ULONG			in_SupportFlags;	// +jl+ Flags to determine which kind of menu 
							// operation this specific icon supports. see below
	const struct TypeNode	*in_FileType;		// +jl+ this icon's file type

	struct DateStamp	in_FileDateStamp;	// +jl+ the file's datestamp
	ULONG			in_FileSize;		// +jl+ size of the file, 0 for drawers

	WORD			in_OldLeftEdge;
	WORD			in_OldTopEdge;
};

// in_Flags:
#define INB_DefaultIcon			0		// the icon has diskobject
#define INB_File			1		// File or Drawer (TextMode only)
#define INB_TextIcon			2		// TextMode icon
#define INB_Sticky			3		// icon not moveable
#define INB_DropMarkVisible		4		// Icon drop mark current visible
#define INB_VolumeWriteProtected	5		// volume is write-protected
#define INB_FreeIconPosition		6		// Icon is stored with "no position"
#define INB_IconVisible			7		// Icon has been drawn
#define INB_Identified			8		// Icon file type has been determined

#define INF_DefaultIcon			(1L<<INB_DefaultIcon)
#define INF_File			(1L<<INB_File)
#define INF_TextIcon			(1L<<INB_TextIcon)
#define INF_Sticky			(1L<<INB_Sticky)
#define INF_DropMarkVisible		(1L<<INB_DropMarkVisible)
#define INF_VolumeWriteProtected	(1L<<INB_VolumeWriteProtected)
#define INF_FreeIconPosition		(1L<<INB_FreeIconPosition)
#define INF_IconVisible			(1L<<INB_IconVisible)
#define	INF_Identified			(1L<<INB_Identified)

// in_SupportFlags
#define INB_SupportsOpen	0
#define INB_SupportsCopy	1
#define INB_SupportsRename	2
#define INB_SupportsInformation 3
#define INB_SupportsSnapshot	4
#define INB_SupportsUnSnapshot	5
#define INB_SupportsLeaveOut	6
#define INB_SupportsPutAway	7
#define INB_SupportsDelete	8
#define INB_SupportsFormatDisk	9
#define INB_SupportsEmptyTrash	10

#define INF_SupportsOpen	(1L<<INB_SupportsOpen)
#define INF_SupportsCopy	(1L<<INB_SupportsCopy)
#define INF_SupportsRename	(1L<<INB_SupportsRename)
#define INF_SupportsInformation (1L<<INB_SupportsInformation)
#define INF_SupportsSnapshot	(1L<<INB_SupportsSnapshot)
#define INF_SupportsUnSnapshot	(1L<<INB_SupportsUnSnapshot)
#define INF_SupportsLeaveOut	(1L<<INB_SupportsLeaveOut)
#define INF_SupportsPutAway	(1L<<INB_SupportsPutAway)
#define INF_SupportsDelete	(1L<<INB_SupportsDelete)
#define INF_SupportsFormatDisk	(1L<<INB_SupportsFormatDisk)
#define INF_SupportsEmptyTrash	(1L<<INB_SupportsEmptyTrash)

// ------------------------------------------------------------------

struct ScaDeviceIcon 
{
	struct MinNode		 di_Node;
	struct MsgPort		*di_Handler;		// MessagePort of the HandlerTask
	STRPTR			 di_Volume;		// Volumename or NULL
	STRPTR			 di_Device;		// Devicename or NULL
	Object			*di_Icon;		// IconObject of this DeviceIcon struct
	struct InfoData		*di_Info;		// Infodata from dos.library/Info()
	UBYTE			 di_InfoBuf[4 + sizeof(struct InfoData)];	//InfoData
	UWORD			 di_Flags;		// see below
	struct NotifyRequest	 di_NotifyReq;		// for notification of devicewindow about icon changes
	STRPTR			 di_DiskIconName;	// this name was used to retrieve the current disk icon
};

// bits in di_Flags
#define DIBB_DontDisplay		0		// Don't display this icon
#define DIBF_DontDisplay		(1L << DIBB_DontDisplay)
#define DIBB_Remove			1		// ***internal use only*** 
#define DIBF_Remove			(1L << DIBB_Remove)
#define DIBB_InfoPending		2		// ***internal use only*** 
#define DIBF_InfoPending		(1L << DIBB_InfoPending)
#define DIBB_BackdropReadComplete 	3		// ***internal use only***
#define DIBF_BackdropReadComplete	(1L << DIBB_BackdropReadComplete)


// for compatibility's sake
#define DIB_DontDisplay		DIBB_DontDisplay

// ------------------------------------------------------------------

// UpdateIcon structure for Icon and Device windowtypes

struct ScaUpdateIcon_IW
{
	BPTR			ui_iw_Lock;	    	// Lock to the file's/dir's directory
	STRPTR			ui_iw_Name;	    	// Name of the file or dir, NULL for Disks
	// new starting with 41.8:
	ULONG			ui_IconType;		// indication for icon type, if known
							// like WBDRAWER, leave at UI_ICONTYPE_UNKNOWN if unknown
};

#define UI_ICONTYPE_UNKNOWN	0

// ------------------------------------------------------------------

struct ScalosNodeList
{
	struct MinNode		*snl_MinNode;	    // Pointer to the first node or NULL
};


// ------------------------------- Scalos Class Information --------------------------

#define SCC_Dummy		0x80580000


// ---------------------------------------------------------------------------
// -------------------- Root Class -------------------------------
// Name: "Root.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------- Attributes ------------------------------
// ---------------------------------------------------------------------------

#define SCCA_WindowTask	    			(SCC_Dummy+1)		//(ISG)

// ---------------------------------------------------------------------------
// ----------------------- Methodes -------------------------------
// ---------------------------------------------------------------------------

#define SCCM_HandleMessages			(SCC_Dummy+105)

// ---------------------------------------------------------------------------

#define SCCM_ScalosMessage			(SCC_Dummy+106)

// ---------------------------------------------------------------------------

#define SCCM_Message				(SCC_Dummy+107)

// ---------------------------------------------------------------------------

#define SCCM_ScalosMessageReply 		(SCC_Dummy+108)

// ---------------------------------------------------------------------------

#define SCCM_RunProcess				(SCC_Dummy+120)

// ---------------------------------------------------------------------------

#define SCCM_CheckForMessages			(SCC_Dummy+198)
// APTR Routine
// APTR Args
// ULONG    size_of(Args)
// APTR ReplyPort	;see values below

#define SCCV_RunProcess_NoReply		    	0
#define SCCV_RunProcess_WaitReply	    	-1

// ---------------------------------------------------------------------------

#define SCCM_Ping				(SCC_Dummy+140)	// Timer Ping

// ---------------------------------------------------------------------------

#define SCCM_AddToClipboard			(SCC_Dummy+172)
// struct ScaWindowTask *
// struct ScaIconNode *
// ULONG Opcode

#define SCCV_AddToClipboard_Copy			1
#define SCCV_AddToClipboard_Move			2

// ---------------------------------------------------------------------------

#define	SCCM_ClearClipboard			(SCC_Dummy+173)
// ./.

// ---------------------------------------------------------------------------

#define SCCM_GetLocalizedString			(SCC_Dummy+190)
// ULONG StringID

// ---------------------------------------------------------------------------

#define	SCCM_WindowStartComplete 		(SCC_Dummy+215)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_AddListener 			(SCC_Dummy+216)
// ULONG - the method we want to be informed about
// struct MsgPort * - Message port to forward the events to
// ULONG - number of times we want to receive messages

// ---------------------------------------------------------------------------

#define	SCCM_RemListener 			(SCC_Dummy+217)
// APTR - The handle returned by SCCM_AddListener

// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// -------------------- Title Class -------------------------------
// Name: "Title.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------- Attributes ------------
// ---------------------------------------------------------------------------

#define SCCA_Title_Format			(SCC_Dummy+2) //(IS.)
#define SCCA_Title_Type				(SCC_Dummy+3) //(I..)

#define SCCV_Title_Type_Screen		    	0	    // (default)
#define SCCV_Title_Type_Window		    	1

// ---------------------------------------------------------------------------
// --------------- Methods --------------
// ---------------------------------------------------------------------------

#define SCCM_Title_Generate	    		(SCC_Dummy+101)
// Result: MemPtr

// ---------------------------------------------------------------------------

#define SCCM_Title_Query	    		(SCC_Dummy+102)
// UWORD    ParseID
// Result: BOOL (Needs Refresh)

// ---------------------------------------------------------------------------

#define SCCM_Title_QueryTitle	    		(SCC_Dummy+103)
// Result: BOOL (Needs Refresh)

// ---------------------------------------------------------------------------

#define SCCM_Title_Translate	    		(SCC_Dummy+104)
// CHAR *TitleMem	gets updated after call
// CHAR *TitleFormat	gets updated after call
// UWORD    ParseID
// Result: succ BOOL

// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// ----------------- Window Class -------------------------------
// Name: "Window.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------

#define SCCM_Window_Open			(SCC_Dummy+123)

// ---------------------------------------------------------------------------

#define SCCM_Window_Close			(SCC_Dummy+124)

// ---------------------------------------------------------------------------

#define SCCM_Window_Iconify			(SCC_Dummy+125)

// ---------------------------------------------------------------------------

#define SCCM_Window_UnIconify			(SCC_Dummy+126)

// ---------------------------------------------------------------------------

#define SCCM_Window_InitClipRegion		(SCC_Dummy+127)
// Result: OldClipRegion

// ---------------------------------------------------------------------------

#define SCCM_Window_RemClipRegion		(SCC_Dummy+128)
// APTR OldClipRegion

// ---------------------------------------------------------------------------

#define SCCM_Window_SetTitle			(SCC_Dummy+129)

// ---------------------------------------------------------------------------

#define SCCM_Window_SetInnerSize		(SCC_Dummy+143)

// ---------------------------------------------------------------------------

#define SCCM_Window_LockUpdate			(SCC_Dummy+154)	
// ./.

// ---------------------------------------------------------------------------

#define SCCM_Window_UnlockUpdate		(SCC_Dummy+155)	
// ./.

// ---------------------------------------------------------------------------

#define SCCM_Window_ChangeWindow		(SCC_Dummy+189)	
// ./.

// ---------------------------------------------------------------------------

#define SCCM_Window_DynamicResize		(SCC_Dummy+195)
// ./.

// ---------------------------------------------------------------------------

#define SCCM_Window_NewPath			(SCC_Dummy+208)
// CONST_STRPTR
// ULONG Tag, Tag, ... TAG_END
// (same parameters as SCCM_IconWin_NewPath)

// ---------------------------------------------------------------------------

#define SCCM_Window_SetTransparency		(SCC_Dummy+209)
// ULONG Transparency, 0=transparent, 100=opaque

// ---------------------------------------------------------------------------

#define SCCM_Window_WBStartupFinished 		(SCC_Dummy+210)
// ./.

// -------------- Attributes ------------

#define SCCA_Window_IDCMP			(SCC_Dummy+2)	// unused
#define	SCCA_Window_Transparency		(SCC_Dummy+3)		// (ULONG) [.SG] percentage of window opacity, 0=transparent, 100=opaque


// ---------------------------------------------------------------------------
// ---------------- IconWindow Class ----------------------------
// Name: "IconWindow.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------- IconWindow Attributes ------------
// ---------------------------------------------------------------------------

#define SCCA_IconWin_Reading			(SCC_Dummy+1000)	// (BOOL)  [SG] currently  reading directory
#define	SCCA_IconWin_SelectedCount		(SCC_Dummy+1001)	// (ULONG) [SG] number of selected icons in window
#define	SCCA_IconWin_Typing			(SCC_Dummy+1002)	// (BOOL)  [SG] user is currently tying icon name
#define	SCCA_IconWin_ShowType			(SCC_Dummy+1003)	// (ULONG) [SG] DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
#define	SCCA_IconWin_InnerWidth			(SCC_Dummy+1004)	// (ULONG) [.G] width of window icon window icon area
#define	SCCA_IconWin_InnerHeight		(SCC_Dummy+1005)	// (ULONG) [.G] height of window icon window icon area
#define	SCCA_IconWin_IconFont			(SCC_Dummy+1006)	// (struct TextFont *) [.G] icon window icon font
#define	SCCA_IconWin_LayersLocked		(SCC_Dummy+1007)	// (ULONG) [.G] Flag: icon window has Layers locked
#define	SCCA_IconWin_StatusBar			(SCC_Dummy+1008)	// (BOOL)  [SG] Flag: Status bar is present
#define SCCA_IconWin_ThumbnailView		(SCC_Dummy+1009)	// (ULONG) [SG] Thumbnail display mode
#define	SCCA_IconWin_ThumbnailsGenerating	(SCC_Dummy+1010)	// (ULONG) [SG] Shows flag for thumbnail generation in progress
#define	SCCA_IconWin_InnerLeft			(SCC_Dummy+1011)	// (ULONG) [.G] left edge of window icon window icon area
#define	SCCA_IconWin_InnerTop			(SCC_Dummy+1012)	// (ULONG) [.G] top edge of window icon window icon area
#define	SCCA_IconWin_InnerRight			(SCC_Dummy+1013)	// (ULONG) [.G] right edge of window icon window icon area
#define	SCCA_IconWin_InnerBottom		(SCC_Dummy+1014)	// (ULONG) [.G] bottom
#define	SCCA_IconWin_ActiveTransparency		(SCC_Dummy+1015)	// (ULONG) [SG] degree of transparency for active window state (0=transparent, 100=opaque)
#define	SCCA_IconWin_InactiveTransparency	(SCC_Dummy+1016)	// (ULONG) [SG] degree of transparency for inactive window state (0=transparent, 100=opaque)
#define	SCCA_IconWin_ControlBar			(SCC_Dummy+1017)	// (BOOL)  [SG] Flag: Control bar is present
#define	SCCA_IconWin_IconScaleFactor            (SCC_Dummy+1018)	// (ULONG) [SG] icon scaling factor in percent
#define	SCCA_IconWin_IconSizeConstraints        (SCC_Dummy+1019)	// (struct Rectangle *) [SG] icon size constraints

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------

#define SCCM_IconWin_ReadIconList		(SCC_Dummy+100)
// ULONG    flags
// Result: BOOL CloseWindow
// Flags:
#define SCCV_IconWin_ReadIconList_ShowAll	    1

// ---------------------------------------------------------------------------

#define SCCM_IconWin_ReadIcon			(SCC_Dummy+109)
// CONST_STRPTR Name		// icon name
// struct ScaReadIconArg *	// x/y position and parent lock, may be NULL

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragBegin			(SCC_Dummy+110)	// currently unused

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragQuery			(SCC_Dummy+111)
// struct DragEnter *
// ---------- Returns : BOOL :
//	TRUE	yes, we may drop here
//	FALSE	no, no drop allowed

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragFinish			(SCC_Dummy+112)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragEnter			(SCC_Dummy+149)	// +jl+
// struct DragEnter *
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragLeave			(SCC_Dummy+150)	// +jl+
// struct DragEnter *
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DragDrop			(SCC_Dummy+113)
// APTR		dropwindow
// ULONG	mousex
// ULONG	mousey
// ULONG	Qualifier			// from IntuiMessage
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_Open			(SCC_Dummy+114)
// APTR iconnode    (IconNode to open or NULL)
// ULONG	Flags
// Result: BOOL
#define	ICONWINOPENB_IgnoreFileTypes		0
#define ICONWINOPENB_NewWindow			1
#define ICONWINOPENB_DoNotActivateWindow	2
#define ICONWINOPENB_DdPopupWindow		3
#define ICONWINOPENB_BrowserWindow		4
#define	ICONWINOPENF_IgnoreFileTypes		(1 << ICONWINOPENB_IgnoreFileTypes)
#define ICONWINOPENF_NewWindow			(1 << ICONWINOPENB_NewWindow)
#define ICONWINOPENF_DoNotActivateWindow	(1 << ICONWINOPENB_DoNotActivateWindow)
#define ICONWINOPENF_DdPopupWindow		(1 << ICONWINOPENB_DdPopupWindow)
#define ICONWINOPENF_BrowserWindow		(1 << ICONWINOPENB_BrowserWindow)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_UpdateIcon			(SCC_Dummy+115)
// BPTR Lock
// APTR Name

// ---------------------------------------------------------------------------

#define SCCM_IconWin_UpdateIconTags		(SCC_Dummy+225)
// BPTR Lock
// APTR Name
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define SCCM_IconWin_AddIcon			(SCC_Dummy+116)
// WORD x
// WORD y
// APTR Lock
// APTR Name

// auxiliary macro to combine the WORD x/y values into one longword
// for useage with DoMethod(...)
#define	SCCM_ADDICON_MAKEXY(x,y)		((((UWORD) (x)) << 16) | ((UWORD) (y)))

// ---------------------------------------------------------------------------

#define SCCM_IconWin_RemIcon			(SCC_Dummy+117)
// BPTR Lock
// CONST_STRPTR Name

// ---------------------------------------------------------------------------

#define SCCM_IconWin_MakeWBArg			(SCC_Dummy+118)
// struct ScaIconNode *
// APTR Buffer
// ---------- Returns : Number of WbArg's put into Buffer

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CountWBArg			(SCC_Dummy+197)
// struct ScaIconNode *
// ---------- Returns : Number of selected items

// ---------------------------------------------------------------------------

#define SCCM_IconWin_UnCleanUp			(SCC_Dummy+193)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CleanUp			(SCC_Dummy+119)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CleanUpByName		(SCC_Dummy+180)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CleanUpByDate		(SCC_Dummy+181)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CleanUpBySize		(SCC_Dummy+182)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_CleanUpByType		(SCC_Dummy+183)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_SetVirtSize		(SCC_Dummy+121)
// UWORD    Flags
// Flags:
//  bit 0 = set Bottom-Slider
//  bit 1 = set Right-Slider
#define	SETVIRTB_AdjustBottomSlider		0
#define	SETVIRTB_AdjustRightSlider		1
#define	SETVIRTF_AdjustBottomSlider		(1 << SETVIRTB_AdjustBottomSlider)
#define	SETVIRTF_AdjustRightSlider		(1 << SETVIRTB_AdjustRightSlider)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_Redraw			(SCC_Dummy+122)
// ULONG Flags

// Values in Flags :
#define	REDRAWB_DontEraseWindow			0
#define	REDRAWB_ReLayoutIcons			1
#define REDRAWB_DontRefreshWindowFrame		2
#define	REDRAWF_DontEraseWindow			(1L << REDRAWB_DontEraseWindow)
#define	REDRAWF_ReLayoutIcons			(1L << REDRAWB_ReLayoutIcons)
#define REDRAWF_DontRefreshWindowFrame		(1L << REDRAWB_DontRefreshWindowFrame)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_Update			(SCC_Dummy+130)

// ---------------------------------------------------------------------------

// was SCCM_IconWin_CheckUpdate
#define SCCM_IconWin_ScheduleUpdate		(SCC_Dummy+151)	// +jl+
// ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_Sleep			(SCC_Dummy+131)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_WakeUp			(SCC_Dummy+132)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_MenuCommand		(SCC_Dummy+138)
// APTR CommandName
// APTR IconNode    ;(or NULL)
// BOOL State	    ;(checkmark state if it's a checkmark)

// ---------------------------------------------------------------------------

#define SCCM_IconWin_NewViewMode		(SCC_Dummy+139)
// ULONG    ViewMode
// Result: IconWindow.sca subclass object

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DrawIcon			(SCC_Dummy+141)
// APTR Icon

// ---------------------------------------------------------------------------

#define SCCM_IconWin_LayoutIcon			(SCC_Dummy+142)
// APTR Icon
// ULONG	LayoutFlags

// +jl+ Layout Flags see iconobject.h
#ifndef IOLAYOUTB_NormalImage
#define IOLAYOUTB_NormalImage			0	// Layout normal Image
#define IOLAYOUTF_NormalImage			(1 << SCCLAYOUTB_NormalImage)
#define IOLAYOUTB_SelectedImage			1	// Layout selected Image
#define IOLAYOUTF_SelectedImage			(1 << SCCLAYOUTB_NormalImage)
#endif

// ---------------------------------------------------------------------------

#define SCCM_IconWin_DeltaMove			(SCC_Dummy+144)
// LONG deltaX
// LONG deltaY

// ---------------------------------------------------------------------------

#define SCCM_IconWin_GetDefIcon			(SCC_Dummy+146)
// char*	filename
// LONG filetype
// ULONG	protectionbits

// ---------------------------------------------------------------------------

#define SCCM_IconWin_ShowIconToolTip		(SCC_Dummy+156)	// +jl+ 20010519
// struct ScaIconNode *
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_RawKey			(SCC_Dummy+158)	// +jl+ 20010716
// struct IntuiMessage *

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_AddToStatusBar		(SCC_Dummy+159)	// +jl+ 20011204
// struct Gadget *
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_RemFromStatusBar		(SCC_Dummy+160)	// +jl+ 20011204
// struct Gadget *

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_UpdateStatusBar		(SCC_Dummy+161)	// +jl+ 20011204
// struct Gadget *
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ShowPopupMenu		(SCC_Dummy+166)	// +jl+ 20011209
// struct PopupMenu *mpm_PopupMenu;
// struct ScaIconNode *mpm_IconNode;		(may be NULL for window popup menus)
// ULONG mpm_Flags;

// Values in mpm_Flags
#define	SHOWPOPUPFLGB_IconSemaLocked		0
#define	SHOWPOPUPFLGB_WinListLocked		1
#define	SHOWPOPUPFLGB_FreePopupMenu		2
#define	SHOWPOPUPFLGF_IconSemaLocked		(1L << SHOWPOPUPFLGB_IconSemaLocked)
#define	SHOWPOPUPFLGF_WinListLocked		(1L << SHOWPOPUPFLGB_WinListLocked)
#define	SHOWPOPUPFLGF_FreePopupMenu		(1L << SHOWPOPUPFLGB_FreePopupMenu)

#define	SCCM_IconWin_ShowGadgetToolTip		(SCC_Dummy+171)	// +jl+ 20021001
// ULONG sgtt_GadgetID

// Values for sgtt_GadgetID
enum sgttGadgetIDs
{
	SGTT_GADGETID_RightScroller = 1001,
	SGTT_GADGETID_BottomScroller,
	SGTT_GADGETID_UpArrow,
	SGTT_GADGETID_DownArrow,
	SGTT_GADGETID_RightArrow,
	SGTT_GADGETID_LeftArrow,
	SGTT_GADGETID_Iconify,
	SGTT_GADGETID_StatusBar_Text,
	SGTT_GADGETID_StatusBar_ReadOnly,
	SGTT_GADGETID_StatusBar_Reading,
	SGTT_GADGETID_StatusBar_Typing,
	SGTT_GADGETID_StatusBar_ShowAll,
	SGTT_GADGETID_StatusBar_ThumbnailsAlways,
	SGTT_GADGETID_StatusBar_ThumbnailsAsDefault,
	SGTT_GADGETID_StatusBar_ThumbnailsGenerate,
	SGTT_GADGETID_ControlBar,	// any control bar gadget
	SGTT_GADGETID_unknown,
};

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconLeft		(SCC_Dummy+174)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconRight		(SCC_Dummy+175)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconUp		(SCC_Dummy+176)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconDown		(SCC_Dummy+177)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconNext		(SCC_Dummy+178)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ActivateIconPrevious	(SCC_Dummy+179)	// +jl+ 20021201
// ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_AddGadget			(SCC_Dummy+184)	// +jl+ 20030112
// struct Gadget *

// ---------------------------------------------------------------------------

#define SCCM_IconWin_ImmediateCheckUpdate	(SCC_Dummy+187)	// +jl+
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_GetIconFileType		(SCC_Dummy+168)	// +jl+ 20020615
// struct ScaIconNode *gift_IconNode;

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_ClearIconFileTypes		(SCC_Dummy+169)	// +jl+ 20020615
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_NewPatternNumber		(SCC_Dummy+192)	// +jl+ 20040912
// struct msg_NewPatternNumber *

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_AddToControlBar		(SCC_Dummy+199)
// struct Gadget *
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_RemFromControlBar		(SCC_Dummy+200)
// struct Gadget *

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_UpdateControlBar		(SCC_Dummy+201)
// struct Gadget *
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_NewPath			(SCC_Dummy+202)
// CONST_STRPTR
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_History_Back		(SCC_Dummy+204)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_History_Forward		(SCC_Dummy+205)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_StartNotify		(SCC_Dummy+206)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_Browse			(SCC_Dummy+207)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_WBStartupFinished		(SCC_Dummy+208)
// ./.

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_StartPopOpenTimer		(SCC_Dummy+213)
// struct ScaWindowTask *spot_DestWindow;
// struct DragHandle *spot_DragHandle;
// struct ScaIconNode *spot_IconNode;

// ---------------------------------------------------------------------------

#define SCCM_IconWin_StopPopOpenTimer		(SCC_Dummy+214)
// struct DragHandle *stop_DragHandle;

// ---------------------------------------------------------------------------

#define	SCCM_IconWin_AddUndoEvent               (SCC_Dummy+218)
// enum ScalosUndoType aue_Type;
// ULONG Tag, Tag, ... TAG_END

// ---------------------------------------------------------------------------

#define SCCM_IconWin_BeginUndoStep		(SCC_Dummy+219)
// ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_EndUndoStep		(SCC_Dummy+220)
// APTR UndoStep

// ---------------------------------------------------------------------------

#define SCCM_IconWin_RandomizePatternNumber	(SCC_Dummy+222)
// ./.

// ---------------------------------------------------------------------------

#define SCCM_IconWin_UnCleanUpRegion		(SCC_Dummy+224)
// struct Region *UnCleanupRegion;

// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// ---------------- DeviceWindow Class ----------------------------
// Name: "DeviceWindow.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------

#define SCCM_DeviceWin_ReadIcon			(SCC_Dummy+136)
// APTR DeviceNode

// ---------------------------------------------------------------------------

#define SCCM_DeviceWin_RemIcon			(SCC_Dummy+188)
// struct ScaIconNode *


// ---------------------------------------------------------------------------
// ---------------- TextWindow Class ----------------------------
// Name: "TextWindow.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------

#define SCCM_TextWin_ReadIcon			(SCC_Dummy+137)	// currently unimplemented

// ---------------------------------------------------------------------------

#define SCCM_TextWin_ReAdjust			(SCC_Dummy+145)

// ---------------------------------------------------------------------------

// hold all visible updates until SCCM_TextWin_EndUpdate
#define SCCM_TextWin_BeginUpdate		(SCC_Dummy+152)
// ULONG RefreshFlag
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

// unlock window updates and do SCCM_TextWin_ReAdjust
#define SCCM_TextWin_EndUpdate			(SCC_Dummy+153)
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define SCCM_TextWin_InsertIcon			(SCC_Dummy+186)
// struct ScaIconNode *
// ---------- Returns : ./.

// ---------------------------------------------------------------------------

#define	SCCM_TextWin_DrawColumnHeaders		(SCC_Dummy+194)
// ./.
// ---------- Returns : ./.

// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// --------------- DeviceList Class ----------------------------
// Name: "DeviceList.sca"
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------

#define SCCM_DeviceList_Generate		(SCC_Dummy+133)
// APTR ScalosNodeList

// ---------------------------------------------------------------------------

#define SCCM_DeviceList_FreeDevNode		(SCC_Dummy+134)
// APTR Node

// ---------------------------------------------------------------------------

#define SCCM_DeviceList_Filter			(SCC_Dummy+135)
// APTR ScalosNodeList

// ------------------------------------------------------------------


// ---------------------------------------------------------------------------
// ------------- FileTransfer Class ---------------------------
// Name: "FileTransfer.sca"
// ---------------------------------------------------------------------------

enum ftOpCodes { FTOPCODE_Copy=1, FTOPCODE_Move, FTOPCODE_CreateLink, FTOPCODE_Delete };

// -------------- Attributes -----------

#define SCCA_FileTrans_Number			(SCC_Dummy+5)	// [I..]
#define SCCA_FileTrans_Screen			(SCC_Dummy+6)	// [I..]
#define	SCCA_FileTrans_ReplaceMode		(SCC_Dummy+157)	// [I..] +jl+ 20010713
#define SCCA_FileTrans_WriteProtectedMode	(SCC_Dummy+195)	// [I..]
#define SCCA_FileTrans_LinksNotSupportedMode	(SCC_Dummy+212)	// [I..]

// Values for SCCA_FileTrans_ReplaceMode
#define	SCCV_ReplaceMode_Ask			0
#define	SCCV_ReplaceMode_Never			1
#define	SCCV_ReplaceMode_Always			2
#define	SCCV_ReplaceMode_Abort			3

// Values for SCCA_FileTrans_WriteProtectedMode
#define	SCCV_WriteProtectedMode_Ask		0
#define	SCCV_WriteProtectedMode_Never		1
#define	SCCV_WriteProtectedMode_Always		2
#define	SCCV_WriteProtectedMode_Abort		3

// Values for SCCA_FileTrans_LinksNotSupportedMode
#define	SCCV_LinksNotSupportedMode_Ask		0
#define	SCCV_LinksNotSupportedMode_Ignore	1
#define	SCCV_LinksNotSupportedMode_IgnoreAll	2
#define	SCCV_LinksNotSupportedMode_Copy		3
#define	SCCV_LinksNotSupportedMode_CopyAll	4
#define	SCCV_LinksNotSupportedMode_Abort	5

// ---------------------------------------------------------------------------
// --------------- Methods -------------
// ---------------------------------------------------------------------------
#define SCCM_FileTrans_Copy			(SCC_Dummy+147)
// BPTR SourceLock
// BPTR DestLock
// CONST_STRPTR	SrcName
// CONST_STRPTR	DestName
// LONG MouseX
// LONG MouseY

// ---------------------------------------------------------------------------

#define SCCM_FileTrans_Move			(SCC_Dummy+148)
// BPTR SourceLock
// BPTR DestLock
// BPTR Name
// LONG MouseX
// LONG MouseY

// ---------------------------------------------------------------------------

#define SCCM_FileTrans_CheckAbort		(SCC_Dummy+162)
// input -
// return RETURN_OK to continue 
//	or non-zero to abort moving/copying

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_OpenWindow		(SCC_Dummy+163)
// input -
// returns struct Window *  - NULL if window could not be opened

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_OpenWindowDelayed 	(SCC_Dummy+164)	
// input -
// returns struct Window *  - NULL if window could not be opened

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_UpdateWindow		(SCC_Dummy+165)	
// ULONG UpdateMode
// BPTR SrcDirLock
// BPTR DestDirLock
// CONST_STRPTR Name
// returns -

enum FILETRANS_UpdateMode	{ FTUPDATE_Initial, FTUPDATE_EnterNewDir, FTUPDATE_Entry, FTUPDATE_SameFile, FTUPDATE_Final };

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_CountTimeout		(SCC_Dummy+167)
// input -
// returns non-zero value if CountTimeout is exceeded and counting should be aborted.

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_OverwriteRequest		(SCC_Dummy+185)	// +jl+ 20011214
// enum OverWriteReqType requestType
// BPTR srcLock
// CONST_STRPTR srcName
// BPTR destLock
// CONST_STRPTR destName
// struct Window *parentWindow
// ULONG suggestedBodyTextId
// ULONG suggestedGadgetTextId
// returns ExistsReqResult

enum OverWriteReqType { OVERWRITEREQ_Copy, OVERWRITEREQ_Move, OVERWRITEREQ_CopyIcon };
enum ExistsReqResult { EXISTREQ_Replace = 1, EXISTREQ_Skip, EXISTREQ_ReplaceAll, EXISTREQ_SkipAll, EXISTREQ_Abort = 0 };

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_CreateLink		(SCC_Dummy+191)
// BPTR SourceLock
// BPTR DestLock
// CONST_STRPTR	SrcName
// CONST_STRPTR	DestName
// LONG MouseX
// LONG MouseY

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_WriteProtectedRequest	(SCC_Dummy+196)
// enum WriteProtectedReqType requestType
// BPTR destLock
// CONST_STRPTR destName
// struct Window *parentWindow
// LONG ErrorCode
// ULONG suggestedBodyTextId
// ULONG suggestedGadgetTextId
// returns WriteProtectedReqResult

enum WriteProtectedReqType { WRITEPROTREQ_Copy, WRITEPROTREQ_Move, WRITEPROTREQ_CopyIcon };
enum WriteProtectedReqResult { WRITEPROTREQ_Replace = 1, WRITEPROTREQ_Skip, WRITEPROTREQ_ReplaceAll, WRITEPROTREQ_SkipAll, WRITEPROTREQ_Abort = 0 };

// ---------------------------------------------------------------------------

#define SCCM_FileTrans_ErrorRequest		(SCC_Dummy+210)
// ULONG mer_SuggestedBodyTextId;
// ULONG mer_SuggestedGadgetTextId;
// returns ErrorReqResult

enum ErrorReqResult { ERRORREQ_Retry = 1, ERRORREQ_Skip, ERRORREQ_Abort };
enum FileTransTypeAction
	{
	ftta_Copy,
	ftta_Move,
	ftta_Delete,
	ftta_DeleteFile,
	ftta_DeleteDir,
	ftta_CopyAndDelete,
	ftta_CopyFile,
	ftta_CopyDir,
	ftta_CopyVolume,
	ftta_CopyLink,
	ftta_CreateSoftLink,
	ftta_CreateHardLink,
	ftta_Count,
	ftta_CountDir,
	};

enum FileTransOperation
	{
	fto_Lock,
	fto_AddPart,
	fto_MakeLink,
	fto_AllocPathBuffer,
	fto_NameFromLock,
	fto_Rename,
	fto_AllocDosObject,
	fto_Examine,
	fto_Open,
	fto_Read,
	fto_Write,
	fto_GetDeviceProc,
	fto_ReadLink,
	fto_DeleteFile,
	fto_CreateDir,
	fto_ExNext,
	fto_DupLock,
	fto_SetProtection,
	};

// ---------------------------------------------------------------------------

#define SCCM_FileTrans_LinksNotSupportedRequest	(SCC_Dummy+211)
//	  BPTR  mlns_SrcDirLock;
//	  CONST_STRPTR mlns_SrcName;
//	  BPTR  mlns_DestDirLock;
//	  CONST_STRPTR mlns_DestName;
//	  ULONG mlns_SuggestedBodyTextId;
//	  ULONG mlns_SuggestedGadgetTextId;

enum LinksNotSupportedReqResult { LINKSNOTSUPPORTEDREQ_Ignore = 1, LINKSNOTSUPPORTEDREQ_IgnoreAll, LINKSNOTSUPPORTEDREQ_Copy, LINKSNOTSUPPORTEDREQ_CopyAll, LINKSNOTSUPPORTEDREQ_Abort };

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_InsufficientSpaceRequest	(SCC_Dummy+223)
//	  struct Window *mlns_ParentWindow;
//	  BPTR  miss_SrcDirLock;
//	  CONST_STRPTR miss_SrcName;
//	  BPTR  miss_DestDirLock;
//	  CONST_STRPTR miss_DestName;
//	  const ULONG64 *miss_RequiredSpace;
//	  const ULONG64 *miss_AvailableSpace;
//	  ULONG miss_SuggestedBodyTextId;
//	  ULONG miss_SuggestedGadgetTextId;

enum InsufficientSpaceReqResult { INSUFFICIENTSPACE_Ignore = 1, INSUFFICIENTSPACE_Abort = 0 };

// ---------------------------------------------------------------------------

#define	SCCM_FileTrans_Delete			(SCC_Dummy+221)
// BPTR DirLock
// CONST_STRPTR	Name

// ---------------------------------------------------------------------------
// ------------------------------------------------------------------
// ---------------------------------------------------------------------------

// this is the struct of every Scalos
// Root.sca subclass
struct ScaRootList 
{
	struct ScaWindowTask	*rl_WindowTask;			// Pointer to a WindowTask structure
	struct ScaInternInfos 	*rl_internInfos;		// Pointer to a Internal Infos struct
};								// !! This is not the end of this struct


struct ScaInternInfos
{
	struct MsgPort		*ii_MainMsgPort;		// Main message port
	struct ScaWindowStruct	*ii_MainWindowStruct;		// Pointer into the windowlist
								// from Desktop Window
	struct ScaWindowStruct	*ii_AppWindowStruct;		// Pointer into the windowlist
								// from Window for AppIcons
	struct Screen		*ii_Screen;			// the Scalos Screen
	struct DrawInfo		*ii_DrawInfo;			// DrawInfo from this screen
	APTR			ii_visualinfo;			// Gadtools VisualInfo
};


/* extended Library base for Scalos OOP plugins */
struct ScaOOPPluginBase
	{
	struct Library soob_Library;	// Standard Library base
	ULONG	soob_Signature;		// Must be 'PLUG'
	};

struct ScaClassInfo 
{
	struct Hook		ci_Hook;			// Dispatcher Hook
	WORD			ci_priority;			// Where to insert the class (-128 to 127)
	UWORD			ci_instsize;			// Instance size
	UWORD			ci_neededversion;		// needed Scalos Version (normally this is 39)
	UWORD			ci_reserved;			// should be NULL
	STRPTR			ci_classname;			// name of your class (max 50 chars)
	STRPTR			ci_superclassname;		// name of the superclass (max 50 chars)
	STRPTR			ci_name;			// real name (max 100 chars)
	STRPTR			ci_description;			// short description (max 200 chars)
	STRPTR			ci_makername;			// name of the programmer (max 50 chars)
};


// This is the parameter structure for Scalos tooltip info string plugin functions
struct ScaToolTipInfoHookData
	{
	const struct ScaIconNode *ttshd_IconNode;	// icon node of the file/drawer/disk to identify
	BPTR ttshd_FileLock;				// READ lock on the file/drawer/disk to identify

	ULONG64 ttshd_FSize;
	LONG ttshd_DirEntryType;
	ULONG ttshd_Protection;
	CONST_STRPTR ttshd_FileName;
	CONST_STRPTR ttshd_Comment;
	struct DateStamp ttshd_Date;

	STRPTR ttshd_Buffer;				// Buffer to fill result string in
	ULONG ttshd_BuffLen;				// Length of ttshd_Buffer
	};

// Parameter structure for SCCM_IconWin_ReadIcon
struct ScaReadIconArg
	{
	WORD	ria_x;
	WORD	ria_y;
	BPTR	ria_Lock;
	ULONG	ria_IconType;
	};

/* ----------------------- Scalos Class Hierarchy -------------------------

				(priority)

	-+- Root.sca 		(-128)
	 |
	 +-- Window.sca 	(-90)
	 |
	 +--- Title.sca 	(-80)
	 |
	 +--- DeviceList.sca 	(-80)
	 |
	 +--- FileTransfer.sca 	(-80)
	 |
	 +-+- IconWindow.sca 	(-80)
	   |
	   +-- DeviceWindow.sca (-60)
	   |
	   +-- TextWindow.sca 	(-60)

----------------------------------------------------------------------- */


struct ScaAppObjNode
{
	struct MinNode	an_Node;		/* Pretty much all the structures with two longs 
						   at the start are actually a MinNode (think 
						   about it, you need a list of AppObjects) */
	WORD			an_Kennug;	/* for want of a better name, holds "SC" as identifier */
	WORD			an_type;
	LONG			an_id;
	LONG			an_userdata;
	struct MsgPort	*an_msgport;
	LONG			an_object;
	LONG			an_userdata2;
};


/*
 types:
  0 = AppIcons		object: IconObject
  1 = AppWindow		object: Window
  2 = AppMenuItem		object: MenuItem
*/


struct DragEnter
	{
	struct ScaWindowStruct *drage_Window;	// Window from wich dragging started
	struct ScaIconNode *drage_Icon;		// Icon under mouse pointer or NULL
	ULONG	drage_MouseX;
	ULONG	drage_MouseY;
	};


// Flag Values for SCA_DrawDrag()
#define SCAB_Drag_Transparent		0
#define SCAF_Drag_Transparent		(1 << SCAB_Drag_Transparent)
#define SCAB_Drag_IndicateCopy		1		// +jl+ draw indicator for copying
#define SCAF_Drag_IndicateCopy		(1 << SCAB_Drag_IndicateCopy)
#define SCAB_Drag_NoDropHere		2		// +jl+ nothing may be dropped here
#define SCAF_Drag_NoDropHere		(1 << SCAB_Drag_NoDropHere)
#define SCAB_Drag_Hide			3		// +jl+ hide Bobs
#define SCAF_Drag_Hide			(1 << SCAB_Drag_Hide)
#define SCAB_Drag_IndicateMakeLink	4		// +jl+ draw indicator for Make-Link
#define SCAF_Drag_IndicateMakeLink	(1 << SCAB_Drag_IndicateMakeLink)
#define	SCAB_Drag_NoDrawDrag		5		// +jl+ 20010802 internal use only
#define	SCAF_Drag_NoDrawDrag		(1 << SCAB_Drag_NoDrawDrag)
#define SCAB_Drag_ForceMove		6		// +jl+ draw indicator for forced move
#define SCAF_Drag_ForceMove		(1 << SCAB_Drag_ForceMove)


/****************************************************************************/
/*	Tags for ScalosControlA() library call                 		    */
/****************************************************************************/

#define	SCALOSCONTROL_BASE		0x80530300

/* Set the default stack size for launching programs with (ULONG). */
#define	SCALOSCTRLA_GetDefaultStackSize		(SCALOSCONTROL_BASE+1)

/* Set the default stack size for launching programs with (ULONG). */
#define	SCALOSCTRLA_SetDefaultStackSize		(SCALOSCONTROL_BASE+2)

/* Get a list of currently running Workbench programs (struct List **). */
#define	SCALOSCTRLA_GetProgramList		(SCALOSCONTROL_BASE+3)

/* Release the list of currently running Workbench programs (struct List *). */
#define	SCALOSCTRLA_FreeProgramList		(SCALOSCONTROL_BASE+4)

/* Get the current state of the "Disable CloseWorkbench()" flag */
#define	SCALOSCTRLA_GetCloseWBDisabled		(SCALOSCONTROL_BASE+5)

/* Set the state of the "Disable CloseWorkbench()" flag */
#define	SCALOSCTRLA_SetCloseWBDisabled		(SCALOSCONTROL_BASE+6)

/* Get the current state of the boot-up splash window flag */
#define	SCALOSCTRLA_GetSplashEnable		(SCALOSCONTROL_BASE+7)

/* Turn off/on boot-up splash window */
#define	SCALOSCTRLA_SetSplashEnable		(SCALOSCONTROL_BASE+8)

/* Get the current icon tool tip enabled state */
#define	SCALOSCTRLA_GetToolTipEnable		(SCALOSCONTROL_BASE+9)

/* Turn off/on icon tool tips */
#define	SCALOSCTRLA_SetToolTipEnable		(SCALOSCONTROL_BASE+10)

/* Get the delay for icon tool tips */
#define	SCALOSCTRLA_GetToolTipDelay		(SCALOSCONTROL_BASE+11)

/* Set the delay for icon tool tips */
#define	SCALOSCTRLA_SetToolTipDelay		(SCALOSCONTROL_BASE+12)

/* Get the current icon tool tip enabled state */
#define	SCALOSCTRLA_GetOldDragIconMode		(SCALOSCONTROL_BASE+13)

/* Turn off/on icon tool tips */
#define	SCALOSCTRLA_SetOldDragIconMode		(SCALOSCONTROL_BASE+14)

/* Get the number of seconds to pass before typing next 
 * character in a drawer window will restart
 * with new file name (ULONG *).
 */
#define	SCALOSCTRLA_GetTypeRestartTime		(SCALOSCONTROL_BASE+15)

/* Set number of seconds to pass before typing
 * next character in a drawer window will restart
 * with new file name (ULONG).
 */
#define	SCALOSCTRLA_SetTypeRestartTime		(SCALOSCONTROL_BASE+16)

/* get a BOOL variable telling whether Scalos
 * is running in emulation mode (TRUE)
 * or in preview mode (FALSE)
 */
#define	SCALOSCTRLA_GetEmulationMode		(SCALOSCONTROL_BASE+17)

/* get a BOOL variable telling whether Scalos
 * displays a status bar in every non-backdrop window
 */
#define	SCALOSCTRLA_GetStatusBarEnable		(SCALOSCONTROL_BASE+18)

/* Tell whether Scalos to display a status 
 * bar in every non-backdrop window (any non-zero value)
 * or to display no status bars (0 = FALSE)
 * Changes only apply to windows opened thereafter.
 */
#define	SCALOSCTRLA_SetStatusBarEnable		(SCALOSCONTROL_BASE+19)

/* Obtain the current hook that will be invoked when Scalos
 * starts copying files and data (struct Hook **) (V40)
 */
#define	SCALOSCTRLA_GetCopyHook			(SCALOSCONTROL_BASE+20)

/* Install a new hook that will be invoked when Scalos
 * starts copying files and data (struct Hook *) (V40)
 */
#define	SCALOSCTRLA_SetCopyHook			(SCALOSCONTROL_BASE+21)

/* Obtain the current hook that will be invoked when Scalos deletes
 * files and drawers or empties the trashcan (struct Hook **)
 * (V40).
 */
#define	SCALOSCTRLA_GetDeleteHook		(SCALOSCONTROL_BASE+22)

/* Install a new hook that will be invoked when Scalos deletes
 * files and drawers or empties the trashcan (struct Hook *)
 * (V40).
 */
#define	SCALOSCTRLA_SetDeleteHook		(SCALOSCONTROL_BASE+23)

/* Obtain the current hook that will be invoked when Scalos requests
 * the user to enter text, i.e. when a file is to be renamed
 * or a new drawer is to be created (struct Hook **) (V40)
 */
#define	SCALOSCTRLA_GetTextInputHook		(SCALOSCONTROL_BASE+24)

/* Install a new hook that will be invoked when Scalos requests
 * the user to enter text, i.e. when a file is to be renamed
 * or a new drawer is to be created (struct Hook *) (V40)
 */
#define	SCALOSCTRLA_SetTextInputHook		(SCALOSCONTROL_BASE+25)

/* Add a hook that will be invoked when Scalos is about 
 * to close (cleanup), and when Scalos has opened 
 * again (setup) (struct Hook *); (V40)
 */
#define	SCALOSCTRLA_AddCloseWBHook		(SCALOSCONTROL_BASE+26)

/* Remove a hook that has been installed with the
 * SCALOSCTRLA_AddCloseWBHook tag (struct Hook *); (V40)
 */
#define	SCALOSCTRLA_RemCloseWBHook		(SCALOSCONTROL_BASE+27)

/* get a BOOL variable telling whether Scalos
 * displays horizontally striped lines in text windows
 */
#define	SCALOSCTRLA_GetStripedTextWindows	(SCALOSCONTROL_BASE+28)

/* Tell whether Scalos to display  horizontally 
 * striped lines in text windows
 * Changes only apply to windows opened thereafter.
 */
#define	SCALOSCTRLA_SetStripedTextWindows	(SCALOSCONTROL_BASE+29)

/* get a BOOL variable telling whether Scalos
 * shows the number of dragged objects during D&D
 */
#define	SCALOSCTRLA_GetDisplayDragCount		(SCALOSCONTROL_BASE+30)

/* Tell whether Scalos to display  
 * the number of dragged objects during D&D
 */
#define	SCALOSCTRLA_SetDisplayDragCount		(SCALOSCONTROL_BASE+31)

/* get a ULONG variable telling when Scalos
 * shows window drop marks during D&D 
 * 0 - never, 1 - only on non-backdrop windows, 2 - always
 */
#define	SCALOSCTRLA_GetWindowDropMarkMode	(SCALOSCONTROL_BASE+32)

/* Set when Scalos will show window drop marks during D&D 
 * 0 - never, 1 - only on non-backdrop windows, 2 - always
 */
#define	SCALOSCTRLA_SetWindowDropMarkMode	(SCALOSCONTROL_BASE+33)

/* Get bit mask of all currently enabled icon types
 * IDTV_IconType_NewIcon - display NewIcons
 * IDTV_IconType_ColorIcon - display OS3.5 color icons
 */
#define	SCALOSCTRLA_GetSupportedIconTypes	(SCALOSCONTROL_BASE+34)


/* Select which icon types are enabled :
 * IDTV_IconType_NewIcon - display NewIcons
 * IDTV_IconType_ColorIcon - display OS3.5 color icons
 */
#define	SCALOSCTRLA_SetSupportedIconTypes	(SCALOSCONTROL_BASE+35)


/* Get a list of all supported menu command names (struct List **). */
#define	SCALOSCTRLA_GetMenuCommandList		(SCALOSCONTROL_BASE+36)

/* Release the list of all supported menu command names (struct List *). */
#define	SCALOSCTRLA_FreeMenuCommandList		(SCALOSCONTROL_BASE+37)


/* Get the size of the buffer used for moving and 
 * copying files (ULONG *).
 */
#define	SCALOSCTRLA_GetCopyBuffSize		(SCALOSCONTROL_BASE+38)

/* Set the size of the buffer used for moving and 
 * copying files (ULONG).
 * Min = 4 KBytes, Max = 4 MBytes
 */
#define	SCALOSCTRLA_SetCopyBuffSize		(SCALOSCONTROL_BASE+39)


/* for internal use only
   intentionally undocumented */
#define	SCALOSCTRLA_GetSemaphoreList		(SCALOSCONTROL_BASE+40)


/****************************************************************************/
/*	Tags for SCCM_IconWin_UpdateIconTags                   		    */
/****************************************************************************/
#define	SCALOSUPDATEICONTAGS_BASE		0x80530301

#define UPDATEICON_IconType                     (SCALOSUPDATEICONTAGS_BASE+1)


/****************************************************************************/
/*	Method structures for all Scalos SCCM_*** class methods		    */
/****************************************************************************/

// --- RootClass methods ------------------------------------------------

// SCCM_RunProcess
struct msg_RunProcess
	{
	ULONG mrp_MethodID;
	RUNPROCFUNC mrp_EntryPoint;
	APTR mrp_Args;
	ULONG mrp_ArgSize;
	struct MsgPort *mrp_ReplyPort;
	};

// SCCM_Message
struct msg_Message
	{
	ULONG msm_MethodID;
	struct IntuiMessage *msm_iMsg;
	};

// SCCM_AddToClipboard
struct msg_AddToClipboard
	{
	ULONG acb_MethodID;
	struct internalScaWindowTask *acb_iwt;
	struct ScaIconNode *acb_in;
	enum ftOpCodes acb_Opcode;
	};

// SCCM_GetLocalizedString
struct msg_GetLocString
	{
	ULONG mgl_MethodID;
	ULONG mgl_StringID;
	};

// SCCM_AddListener
struct msg_AddListener
	{
	ULONG mal_MethodID;
	ULONG mal_Method;		// the method we want to be informed about
	struct MsgPort *mal_Port;	// Message port to forward the events to
	ULONG mal_Count;		// number of times we want to receive messages
	};

// SCCM_RemoveListener
struct msg_RemoveListener
	{
	ULONG mrl_MethodID;
	APTR mrl_EventHandle;		// The handle returned by SCCM_AddListener
	};

// --- DeviceWindowClass methods ----------------------------------------

// SCCM_DeviceWin_ReadIcon
struct msg_DevWinReadIcon
	{
	ULONG dri_MethodID;
	struct ScaDeviceIcon *dri_DeviceNode;
	};

// SCCM_DeviceWin_RemIcon
struct msg_DevWinRemIcon
	{
	ULONG dri_MethodID;
	struct ScaIconNode *dri_IconNode;
	};

// --- DevListClass methods ------------------------------------------

// SCCM_DeviceList_Generate
struct msg_Generate
	{
	ULONG mge_MethodID;
	struct ScaDeviceIcon **mge_DevIconList;
	};

// SCCM_DeviceList_FreeDevNode
struct msg_FreeDevNode
	{
	ULONG mfd_MethodID;
	struct ScaDeviceIcon *mfd_DevIcon;
	};

// SCCM_DeviceList_Filter
struct msg_Filter
	{
	ULONG mfi_MethodID;
	struct ScaDeviceIcon **mfi_DevIconList;
	};

// --- IconWindowClass methods ------------------------------------------

// SCCM_IconWin_ReadIconList
struct msg_ReadIconList
	{
	ULONG mrl_MethodID;
	ULONG mrl_Flags;
	};

// SCCM_IconWin_ShowIconToolTip
struct msg_ShowIconToolTip
	{
	ULONG mtt_MethodID;
	struct ScaIconNode *mtt_IconNode;
	};

// SCCM_IconWin_ShowGadgetToolTip
struct msg_ShowGadgetToolTip
	{
	ULONG sgtt_MethodID;
	ULONG sgtt_GadgetID;
	};

// SCCM_IconWin_RawKey
struct msg_RawKey
	{
	ULONG mrk_MethodID;
	struct IntuiMessage *mrk_iMsg;
	};

// SCCM_IconWin_DragBegin
struct msg_DragBegin
	{
	ULONG mdb_MethodID;
	struct DragEnter mdb_DragEnter;
	};

// SCCM_IconWin_DragDrop
struct msg_DragDrop
	{
	ULONG mdd_MethodID;
	struct Window *mdd_DropWindow;
	ULONG mdd_MouseX;
	ULONG mdd_MouseY;
	ULONG mdd_Qualifier;
	};

// SCCM_IconWin_Open
struct msg_Open
	{
	ULONG mop_MethodID;
	struct ScaIconNode *mop_IconNode;
	ULONG mop_Flags;
	};

// SCCM_IconWin_AddIcon
struct msg_AddIcon
	{
	ULONG mai_MethodID;
	WORD mai_x;
	WORD mai_y;
	BPTR mai_Lock;
	CONST_STRPTR mai_Name;
	};

// SCCM_IconWin_RemIcon
struct msg_RemIcon
	{
	ULONG mri_MethodID;
	BPTR mri_Lock;
	CONST_STRPTR mri_Name;
	};

// SCCM_IconWin_UpdateIconTags
struct msg_UpdateIconTags
	{
	ULONG muit_MethodID;
	BPTR muit_Lock;
	CONST_STRPTR muit_Name;
	ULONG muit_TagList[0];
	};

// SCCM_IconWin_MakeWBArg
struct msg_MakeWbArg
	{
	ULONG mwa_MethodID;
	struct ScaIconNode *mwa_Icon;
	struct WBArg *mwa_Buffer;
	};

// SCCM_IconWin_CountWBArg
struct msg_CountWbArg
	{
	ULONG mca_MethodID;
	struct ScaIconNode *mca_Icon;
	};

// SCCM_IconWin_SetVirtSize
struct msg_SetVirtSize
	{
	ULONG msv_MethodID;
	ULONG msv_Flags;
	};

// SCCM_IconWin_Redraw
struct msg_Redraw
	{
	ULONG mrd_MethodID;
	ULONG mrd_Flags;
	};

// SCCM_IconWin_WakeUp
struct msg_Wakeup
	{
	ULONG mwu_MethodID;
	ULONG mwu_ReLayout;
	};

// SCCM_IconWin_DrawIcon
struct msg_DrawIcon
	{
	ULONG mdi_MethodID;
	Object *mdi_IconObject;
	};

// SCCM_IconWin_LayoutIcon
struct msg_LayoutIcon
	{
	ULONG mli_MethodID;
	Object *mli_IconObject;
	ULONG mli_LayoutFlags;
	};

// SCCM_IconWin_GetDefIcon
struct msg_GetDefIcon
	{
	ULONG mgd_MethodID;
	CONST_STRPTR mgd_Name;
	LONG mgd_Type;		// fib_DirEntryType from FileInfoBlock
	ULONG mgd_Protection;	// fib_Protection from FileInfoBlock
	ULONG mgd_IconType;	// WB icon type as fallback if object cannot be locked
	};

// SCCM_IconWin_MenuCommand
struct msg_MenuCommand
	{
	ULONG mcm_MethodID;
	CONST_STRPTR mcm_CmdName;
	struct ScaIconNode *mcm_IconNode;
	ULONG mcm_State;
	};

// SCCM_IconWin_NewViewMode
struct msg_NewViewMode
	{
	ULONG mnv_MethodID;
	ULONG mnv_NewMode;
	};

// SCCM_IconWin_DeltaMove
struct msg_DeltaMove
	{
	ULONG mdm_MethodID;
	LONG mdm_DeltaX;
	LONG mdm_DeltaY;
	};

// SCCM_IconWin_AddToStatusBar
// SCCM_IconWin_AddToControlBar
struct msg_AddToStatusBar
	{
	ULONG mab_MethodID;
	struct Gadget *mab_NewMember;
	ULONG mab_TagList[0];
	};

// SCCM_IconWin_RemFromStatusBar
// SCCM_IconWin_RemFromControlBar
struct msg_RemFromStatusBar
	{
	ULONG mrb_MethodID;
	struct Gadget *mrb_OldMember;
	};

// SCCM_IconWin_UpdateStatusBar
// SCCM_IconWin_UpdateControlBarBar
struct msg_UpdateStatusBar
	{
	ULONG mub_MethodID;
	struct Gadget *mub_Member;
	ULONG mub_TagList[0];
	};

// SCCM_IconWin_AddGadget
struct msg_AddGadget
	{
	ULONG mag_MethodID;
	struct Gadget *mag_NewGadget;
	};

// SCCM_IconWin_ReadIcon
struct msg_ReadIcon
	{
	ULONG mri_MethodID;
	CONST_STRPTR mri_Name;		// icon name
	struct ScaReadIconArg *mri_ria;
	};

// SCCM_IconWin_ShowPopupMenu
struct msg_ShowPopupMenu
	{
	ULONG mpm_MethodID;
	struct PopupMenu *mpm_PopupMenu;
	struct ScaIconNode *mpm_IconNode;
	ULONG mpm_Flags;
	ULONG mpm_Qualifier;
	struct FileTypeDef *mpm_FileType;
	};

// SCCM_IconWin_NewPatternNumber
struct msg_NewPatternNumber
	{
	ULONG npn_MethodID;
	ULONG npn_PatternNumber;
	};

// SCCM_IconWin_NewPath
struct msg_NewPath
	{
	ULONG npa_MethodID;
	CONST_STRPTR npa_Path;
	ULONG npa_TagList[0];
	};

// SCCM_IconWin_StartPopOpenTimer
struct msg_StartPopOpenTimer
	{
	ULONG spot_MethodID;
	struct ScaWindowTask *spot_DestWindow;
	struct DragHandle *spot_DragHandle;
	struct ScaIconNode *spot_IconNode;
	};

// SCCM_IconWin_StopPopOpenTimer
struct msg_StopPopOpenTimer
	{
	ULONG stop_MethodID;
	struct DragHandle *stop_DragHandle;
	};

// SCCM_IconWin_AddUndoEvent
struct msg_AddUndoEvent
	{
	ULONG aue_MethodID;
	enum ScalosUndoType aue_Type;
	ULONG aue_TagList[0];
	};

// SCCM_IconWin_EndUndoStep
struct msg_EndUndoStep
	{
	ULONG eus_MethodID;
	APTR eus_UndoStep;      // Result from SCCM_IconWin_BeginUndoStep
	};

// SCCM_IconWin_UnCleanUpRegion
struct msg_UnCleanUpRegion
	{
	ULONG ucr_MethodID;
	struct Region *ucr_UnCleanUpRegion;
	};

// --- TextWindowClass methods ------------------------------------------

// SCCM_TextWin_EndUpdate
struct msg_EndUpdate
	{
	ULONG meu_MethodID;
	ULONG meu_RefreshFlag;
	};

// SCCM_TextWin_InsertIcon
struct msg_InsertIcon
	{
	ULONG iic_MethodID;
	struct ScaIconNode *iic_IconNode;
	};

// --- TitleClass methods ------------------------------------------

struct msg_Translate
	{
	ULONG mxl_MethodID;
	STRPTR mxl_Buffer;		// Buffer being written to, gets updated after call
	CONST_STRPTR mxl_TitleFormat;	// current title format pointer, gets updated after call
	UWORD mxl_ParseID;		// ID containing the two-character variable name, e.g. "os"
	UWORD mxl_BuffLen;		// Length of mxl_Buffer
	};

struct msg_Query
	{
	ULONG mqy_MethodID;
	UWORD mqy_ParseID;		// ID containing the two-character variable name, e.g. "os"
	};

// --- WindowClass methods ------------------------------------------

struct msg_RemClipRegion
	{
	ULONG mrc_MethodID;
	struct Region *mrc_oldClipRegion;
	};

struct msg_SetTitle
	{
	ULONG mst_MethodID;
	CONST_STRPTR mst_Title;
	};

struct msg_GetIconFileType
	{
	ULONG mft_MethodID;
	struct ScaIconNode *mft_IconNode;
	};

// --- FileTransClass methods ------------------------------------------

struct msg_Move
	{
	ULONG mmv_MethodID;
	BPTR mmv_SrcDirLock;
	BPTR mmv_DestDirLock;
	CONST_STRPTR mmv_Name;
	LONG mmv_MouseX;
	LONG mmv_MouseY;
	};

struct msg_Copy
	{
	ULONG mcp_MethodID;
	BPTR mcp_SrcDirLock;
	BPTR mcp_DestDirLock;
	CONST_STRPTR mcp_SrcName;
	CONST_STRPTR mcp_DestName;
	LONG mcp_MouseX;
	LONG mcp_MouseY;
	};

struct msg_CreateLink
	{
	ULONG mcl_MethodID;
	BPTR mcl_SrcDirLock;
	BPTR mcl_DestDirLock;
	CONST_STRPTR mcl_SrcName;
	CONST_STRPTR mcl_DestName;
	LONG mcl_MouseX;
	LONG mcl_MouseY;
	};

struct msg_UpdateWindow
	{
	ULONG muw_MethodID;
	ULONG muw_UpdateMode;
	BPTR muw_SrcDirLock;
	BPTR muw_DestDirLock;
	CONST_STRPTR muw_SrcName;
	};

struct msg_OverwriteRequest
	{
	ULONG mov_MethodID;
	enum OverWriteReqType mov_RequestType;
	BPTR mov_SrcLock;
	CONST_STRPTR mov_SrcName;
	BPTR mov_DestLock;
	CONST_STRPTR mov_DestName;
	struct Window *mov_ParentWindow;
	ULONG mov_SuggestedBodyTextId;
	ULONG mov_SuggestedGadgetTextId;
	};

// SCCM_FileTrans_WriteProtectedRequest
struct msg_WriteProtectedRequest
	{
	ULONG mwp_MethodID;
	enum WriteProtectedReqType mwp_RequestType;
	BPTR mwp_DestLock;
	CONST_STRPTR mwp_DestName;
	struct Window *mwp_ParentWindow;
	LONG mwp_ErrorCode;
	ULONG mwp_SuggestedBodyTextId;
	ULONG mwp_SuggestedGadgetTextId;
	};

// SCCM_FileTrans_ErrorRequest
struct msg_ErrorRequest
	{
	ULONG mer_MethodID;
	struct Window *mer_ParentWindow;
	enum FileTransTypeAction mer_Action;	// Action performed when the error occurred
	enum FileTransOperation mer_Op;		// Operation performed (Lock, Examine etc.) when error occurred
	LONG	mer_ErrorCode;			// code of error
	STRPTR	mer_ErrorFileName;		// name of object on which error occured
	ULONG mer_SuggestedBodyTextId;
	ULONG mer_SuggestedGadgetTextId;
	};

// SCCM_FileTrans_LinksNotSupportedRequest
struct msg_LinksNotSupportedRequest
	{
	ULONG mlns_MethodID;
	struct Window *mlns_ParentWindow;
	BPTR  mlns_SrcDirLock;
	CONST_STRPTR mlns_SrcName;
	BPTR  mlns_DestDirLock;
	CONST_STRPTR mlns_DestName;
	ULONG mlns_SuggestedBodyTextId;
	ULONG mlns_SuggestedGadgetTextId;
	};

// SCCM_FileTrans_InsufficientSpaceRequest
struct msg_InsufficientSpaceRequest
	{
	ULONG miss_MethodID;
	struct Window *mlns_ParentWindow;
	BPTR  miss_SrcDirLock;
	CONST_STRPTR miss_SrcName;
	BPTR  miss_DestDirLock;
	CONST_STRPTR miss_DestName;
	const ULONG64 *miss_RequiredSpace;
	const ULONG64 *miss_AvailableSpace;
	ULONG miss_SuggestedBodyTextId;
	ULONG miss_SuggestedGadgetTextId;
	};

// SCCM_FileTrans_Delete
struct msg_Delete
	{
	ULONG mmd_MethodID;
	BPTR mmd_DirLock;
	CONST_STRPTR mmd_Name;
	};

/****************************************************************************/

// Text icon types
#define	WB_TEXTICON_TOOL	10
#define	WB_TEXTICON_DRAWER	9

/****************************************************************************/

// possible selections for thumbnail display

enum DisplayThumbnails
	{
	THUMBNAILS_Never,
	THUMBNAILS_AsDefault,
	THUMBNAILS_Always
	};

/****************************************************************************/

// Icon layout modes

#define	ICONTYPE_MAX			(1 + WBAPPICON)
#define ICONLAYOUT_RESTRICTED_MAX	4

enum IconLayoutMode
	{
	ICONLAYOUT_Columns,		// Layout top-down in columns (default)
	ICONLAYOUT_Rows,		// Layout left-to-right in rows
	};

/****************************************************************************/

// Scalos library base

struct ScalosBase
	{
	struct Library scb_LibNode;
	UBYTE scb_Pad[2];		// pad to next longword border
	CONST_STRPTR scb_Revision;	// "1.4f" or "1.5" etc.
	};

/****************************************************************************/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif	/* SCALOS_SCALOS_H */

