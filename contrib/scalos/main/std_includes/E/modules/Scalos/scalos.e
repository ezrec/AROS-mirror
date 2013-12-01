/* Emodule Path:	Scalos/scalos.e
** Emodule version:	Scalos.m 41.6 (07.01.08) by JMC
**/

OPT MODULE, PREPROCESS
OPT EXPORT

#ifndef SCALOS_SCALOS_H
#define SCALOS_SCALOS_H

MODULE 'dos/dos',
'exec/lists',
'exec/ports',
'exec/tasks',
'exec/semaphores',
'exec/libraries',
'intuition/intuition',
'intuition/classes',
'intuition/classusr',
'intuition/screens',
'datatypes/iconobject',
'utility/hooks',
'rexx/storage',
'devices/timer',
'workbench/startup',		-> Added since 40.32(1.4e)
'graphics/regions', 		-> Added since 40.32(1.4e)
'libraries/popupmenu', 		-> Added since 40.32(1.4e)
'libraries/gfx', 		-> Added since 40.32(1.4e)
'scalos/scalosgfx_struct',	-> 41.4+
'devices/inputevent',
'dos/notify',
'utility/tagitem'

#define SCALOSNAME 'scalos.library'
#define SCALOSVERSION 41
#define SCALOSREVISION 6


CONST	SCA_IconObject  		= $80530001,
	SCA_Stacksize           	= $80530002,
	SCA_Flags               	= $80530003,
	SCA_IconNode            	= $80530004,
	SCA_WindowTitle         	= $80530005,
	SCA_Path                	= $80530006,
	SCA_WindowRect          	= $80530007,
	SCA_XOffset             	= $80530008,
	SCA_YOffset             	= $80530009,
	SCA_PatternNumber       	= $8053000a,
	SCA_ShowAllFiles        	= $8053000b,
	SCA_ViewModes           	= $8053000c,
	SCA_MessagePort         	= $8053000d,
	SCA_Iconify             	= $8053000e,
	SCA_ClassName			= $8053000f,
	SCA_Class			= $80530010,
	SCA_WaitTimeout			= $80530011,	-> // +jl+ 20010324
	SCA_SemaphoreExclusive		= $80530012,	-> // +jl+ 20010408
	SCA_SemaphoreShared		= $80530013,	-> // +jl+ 20010408
	SCA_NoStatusBar			= $80530014,	-> // +jl+ 20011111
	SCA_NoActivateWindow		= $80530015,	-> // +jl+ 20020512
	SCA_Priority			= $80530016,	-> // +jl+ 20020823
	SCA_WindowStruct		= $80430017,	-> // +jl+ 20021204
	SCA_SemaphoreExclusiveNoNest	= $80530018,
	SCA_ShowAllMode			= $80530019,
	SCA_BrowserMode			= $8053001b,	-> ULONG
	SCA_HideStatusBar		= $8053001c,	-> ULONG


	SCAB_WBStart_NoIcon	= 0,
	SCAB_WBStart_Wait	= 1,
	SCAB_WBStart_PathSearch	= 2,
	SCAB_WBStart_NoIconCheck= 3

#define SCAF_WBStart_NoIcon		Shl(1L,SCAB_WBStart_NoIcon)
#define SCAF_WBStart_Wait		Shl(1L,SCAB_WBStart_Wait)
#define SCAF_WBStart_PathSearch		Shl(1L,SCAB_WBStart_PathSearch)
#define SCAF_WBStart_NoIconCheck	Shl(1L,SCAB_WBStart_NoIconCheck)

#define SCAV_WBStart_NoIcon	 SCAF_WBStart_NoIcon
#define SCAV_WBStart_Wait	 SCAF_WBStart_Wait
#define SCAV_WBStart_PathSearch	 SCAF_WBStart_PathSearch
#define SCAV_WBStart_NoIconCheck SCAF_WBStart_NoIconCheck

CONST	SCAB_OpenWindow_ScalosPort = 0	-> // private

#define SCAF_OpenWindow_ScalosPort	Shl(1,SCAB_OpenWindow_ScalosPort)

-> LockWindowList():
CONST	SCA_LockWindowList_Shared  		= 0,
	SCA_LockWindowList_Exclusiv		= 1,
	SCA_LockWindowList_AttemptShared	= 2,
	SCA_LockWindowList_AttemptExclusive	= 3


-> FreeWBArgs():
CONST	SCAV_FreeLocks         =  1,
	SCAB_FreeLocks         =  0,
	SCAV_FreeNames         =  2,
	SCAB_FreeNames         =  1,

	SCA_SortType_Bubble    =  0,
	SCA_SortType_Selection =  1,
	SCA_SortType_Insertion =  2,
	SCA_SortType_Quick     =  3,
	SCA_SortType_Best      = -1

#define SCAF_FreeLocks		Shl(1,SCAB_FreeLocks)
#define SCAF_FreeNames		Shl(1,SCAB_FreeNames)


-> window dropmark types
-> WindowDropMarkTypes

ENUM IDTV_WinDropMark_Never, 		-> never show any window dropmarks
     IDTV_WinDropMark_WindowedOnly, 	-> non-backdrop windows only
     IDTV_WinDropMark_Always 		-> always show window dropmarks
	
-> ---------------------- NodeTypes for AllocStdNode() ------------------------

CONST   NTYP_IconNode 			=  0,
	NTYP_WindowStruct		=  1,
	NTYP_DragNode			=  2,
	NTYP_PatternNode		=  3,
	NTYP_ScalosArg			=  4,
	NTYP_DeviceIcon			=  5,
	NTYP_AppObject			=  8,
	NTYP_MenuInfo			=  9,
	NTYP_PluginClass		=  10,		-> // +jl+
	NTYP_PluginNode			=  11		-> // +jl+

-> ------------------------ MessageTypes for AllocMsg() -----------------------

CONST	MTYP_DropProc			= 1,
	MTYP_CloseWindow		= 2,
	MTYP_StartWindow		= 3,
	MTYP_Timer			= 4,
	MTYP_Requester			= 5,
	MTYP_NewPattern			= 6,
	MTYP_Sleep			= 7,
	MTYP_Wakeup			= 8,
	MTYP_AsyncRoutine		= 9,
	MTYP_RunProcess			= 10,
	MTYP_AppSleep			= 11,
	MTYP_AppWakeup			= 12,
	MTYP_Redraw			= 13,
	MTYP_Update			= 14,
	MTYP_UpdateIcon			= 15,
	MTYP_AddIcon			= 16,
	MTYP_RemIcon			= 17,
	MTYP_ARexxRoutine		= 18,
	MTYP_Iconify			= 19,
	MTYP_UnIconify			= 20,
	MTYP_AsyncBackFill		= 21,
	MTYP_ShowTitle			= 22,
	MTYP_RunMenuCmd			= 23,	->  +jl+ 20010415
	MTYP_ShowPopupMenu		= 24,	->  +jl+ 20011209
	MTYP_ShowStatusBar		= 25,	->  +jl+ 20040919
	MTYP_RedrawIcon			= 26,	->  +jl+ 20050303
	MTYP_DoPopupMenu		= 27,	->  +jl+ 20050319
	MTYP_RedrawIconObj		= 28,
	MTYP_NewPreferences		= 29,
	MTYP_DeltaMove			= 30,
	MTYP_SetThumbnailImage_Remapped	= 31,
	MTYP_SetThumbnailImage_ARGB     = 32,
	MTYP_NewWindowPath		= 33,
	MTYP_MAX			= 34	-> // must always be 1 larger than last MTYP_???

-> // ------------------------------------------------------------------

-> ---------------------------------------------------------------------------

OBJECT scawindowlist
	windowstruct:PTR TO scawindowstruct       -> Pointer to the first windowstruct
	mainwindowstruct:PTR TO scawindowstruct   -> Pointer into the list of windowstructs
	appwindowstruct:PTR TO scawindowstruct    -> dito - windowstruct for AppIcons
ENDOBJECT


OBJECT scawindowstruct
	node:mn
	task:PTR TO tc                		-> Window-Task address
	lock:PTR TO LONG                	-> filelock to the windows directory
	window:PTR TO window       		-> pointer to the window or NULL
	messageport:PTR TO mp    		-> the API-messageport
	left:INT
	top:INT
	width:INT
	height:INT          			-> size and position of the window
	xoffset:INT
	yoffset:INT         			-> the virtual position inside
	patternnode:PTR TO CHAR
	windowtask:PTR TO scawindowtask        	-> windowtask structure
	name:PTR TO CHAR                	-> windows name (is displayed in the title)
	title:PTR TO CHAR           		-> title-formatstring
	classname:PTR TO CHAR       		-> scalos class name
	class:PTR TO LONG          		-> BOOPSI Class if ms_classname is NULL
	flags:INT           			-> see below
	patternnumber:INT   			-> number of the pattern
	viewmodes:CHAR      			-> see below
	windowtype:CHAR      			-> see below
	windowtaskname				-> name of window task +jl+ 20011211
	viewall             			-> DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
	thumbnailview:PTR TO LONG		-> THUMBNAILS_Never, THUMBNAILS_Always, THUMBNAILS_AsDefault
	thumbnailslifetimedays:PTR TO LONG	-> Maximum lifetime for generated thumbnails
ENDOBJECT

-> ws_Flags:
CONST 	WSV_FlagB_RootWindow		= 0,	-> a RootDir window
	WSV_FlagB_NeedsTimerMsg		= 1,	-> window likes to get TimerMessages
	WSV_FlagB_Backdrop		= 2,	-> is the window in backdrop mode
	WSV_FlagB_TimerMsgSent		= 3,	-> timermsg already sent *PRIVATE*
	WSV_FlagB_TaskSleeps		= 4,	-> windowtask is sleeping
	WSV_FlagB_ShowAllFiles		= 5,	-> just what it said
	WSV_FlagB_Iconify		= 6,	-> windowtask is in iconify state
	WSV_FlagB_CheckUpdatePending	= 7,	-> IconWinCheckUpdate() is pending *PRIVATE*
	WSV_FlagB_ActivatePending	= 8,	-> ActivateWindow() has been issued *PRIVATE*
	WSV_FlagB_Typing		= 9,	-> typing name for keyboard icon selection *PRIVATE*
	WSV_FlagB_NoStatusBar		= 10,	-> don't display status bar for this window
	WSV_FlagB_NoActivateWindow	= 11,	-> don't activate window on OpenWindow()
	WSV_FlagB_RefreshPending	= 12,	-> IDCMP_REFRESHWINDOW could not be processed, refresh is pending *PRIVATE*
	WSV_FlagB_BrowserMode		= 13	-> enable single-window browser mode

#define WSV_FlagF_RootWindow		Shl(1,WSV_FlagB_RootWindow)
#define WSV_FlagF_NeedsTimerMsg		Shl(1,WSV_FlagB_NeedsTimerMsg)
#define WSV_FlagF_Backdrop		Shl(1,WSV_FlagB_Backdrop)
#define WSV_FlagF_TimerMsgSent		Shl(1,WSV_FlagB_TimerMsgSent)
#define WSV_FlagF_TaskSleeps		Shl(1,WSV_FlagB_TaskSleeps)
#define WSV_FlagF_ShowAllFiles		Shl(1,WSV_FlagB_ShowAllFiles)
#define WSV_FlagF_Iconify		Shl(1,WSV_FlagB_Iconify)
#define WSV_FlagF_CheckUpdatePending	Shl(1,WSV_FlagB_CheckUpdatePending)
#define	WSV_FlagF_ActivatePending	Shl(1,WSV_FlagB_ActivatePending)
#define	WSV_FlagF_Typing		Shl(1,WSV_FlagB_Typing)
#define	WSV_FlagF_NoStatusBar		Shl(1,WSV_FlagB_NoStatusBar)
#define	WSV_FlagF_NoActivateWindow	Shl(1,WSV_FlagB_NoActivateWindow)
#define WSV_FlagF_RefreshPending	Shl(1,WSV_FlagB_RefreshPending)
#define WSV_FlagF_BrowserMode		Shl(1,WSV_FlagB_BrowserMode)

-> ws_WindowType:
CONST	WSV_Type_IconWindow	= 0,	    -> Window filled with icons
	WSV_Type_DeviceWindow	= 1

#define WSV_Type_DesktopWindow	WSV_Type_DesktopWindow -> synonym for WSV_Type_DeviceWindow

-> ws_Viewmodes:
#define SCAV_ViewModes_Default    IDTV_ViewModes_Default
#define SCAV_ViewModes_Icon	  IDTV_ViewModes_Icon
#define SCAV_ViewModes_Name	  IDTV_ViewModes_Name
#define SCAV_ViewModes_Size	  IDTV_ViewModes_Size
#define SCAV_ViewModes_Date	  IDTV_ViewModes_Date
#define SCAV_ViewModes_Time       IDTV_ViewModes_Time
#define SCAV_ViewModes_Comment    IDTV_ViewModes_Comment
#define SCAV_ViewModes_Protection IDTV_ViewModes_Protection
#define SCAV_ViewModes_Owner      IDTV_ViewModes_Owner
#define SCAV_ViewModes_Group      IDTV_ViewModes_Group
#define SCAV_ViewModes_Type	  IDTV_ViewModes_Type
#define SCAV_ViewModes_Version	  IDTV_ViewModes_Version
#define SCAV_ViewModes_MiniIcon	  IDTV_ViewModes_MiniIcon

-> for compatibility +jl+
#define SIV_IconWin		WSV_Type_IconWindow
#define SIV_DeviceWin		WSV_Type_DeviceWindow

-> ------------------------------------------------------------------

OBJECT patterninfo
	hook:hook
	width			-> Width
	height			-> and Height of Bitmap in Pixels
	bitmap:PTR TO bitmap	-> Bitmap or NULL
	pattern:PTR TO CHAR	-> Pattern Array or NULL
	flags:INT		-> see below
	pad			-> padword
	pen			-> Allocated pen for background pattern
	message:mn		-> Message
ENDOBJECT

-> ------------------------------------------------------------------

OBJECT scawindowtask
	patterninfo:PTR TO patterninfo		-> was: patterninfo[40]:ARRAY OF CHAR
	xoffset:INT
	yoffset:INT         			-> Innerwindow offset
	mainobject:PTR TO CHAR      		-> main ScalosClass object
	windowobject:PTR TO CHAR    		-> "Window.sca" object
	window:PTR TO window         		-> pointer to the window of the task or NULL
	iconlist:PTR TO scaiconnode       	-> Pointer to the first IconNode
	lateiconlist:PTR TO scaiconnode   	-> Pointer to the first IconNode of all not yet
						-> displayed icons (e.g. non-position icon while
						-> loading)
	iconsemaphore:PTR TO ss 		-> Semaphore to access the wt_IconList/wt_LateIconList
						-> It *MUST* be locked before accessing the list.
						-> Shared lock should be used for read-only access
	iconport:PTR TO mp       		-> MessagePort of the windowtask
	windowsemaphore:PTR TO ss 		-> Semaphore to avoid the task from closing
	windowstruct:PTR TO scawindowstruct 	-> Pointer to the WindowStruct of this task
ENDOBJECT

-> // some structure forward declarations
->struct SM_RunProcess;

-> // Function pointer definitions
-> // for some SM_  messages

OBJECT asyncroutinefunc
	scawindowtask:PTR TO LONG
ENDOBJECT

OBJECT arexxfunc
	scawindowtask:PTR TO LONG
	rexxmsg
ENDOBJECT

OBJECT runprocfunc
	sm_runprocess:PTR TO LONG
ENDOBJECT

-> typedef void (*ASYNCROUTINEFUNC)(struct internalScaWindowTask *, APTR);
-> typedef void (*AREXXFUNC)(struct internalScaWindowTask *, struct RexxMsg *);
-> typedef ULONG (*RUNPROCFUNC)(APTR, struct SM_RunProcess *);

-> ------------------------------------------------------------------

CONST	ID_IMSG=$494d5347	-> #define	ID_IMSG		MAKE_ID('I','M','S','G')

OBJECT scalosmessage
	message:mn
	signature       -> ("IMSG")
	messagetype     -> AllocMsg() type
ENDOBJECT

OBJECT sm_closewindow
	sm:scalosmessage
ENDOBJECT

OBJECT sm_startwindow
	sm:scalosmessage
	windowstruct:PTR TO scawindowstruct
ENDOBJECT

OBJECT sm_timer
	sm:scalosmessage
	windowstruct:PTR TO scawindowstruct
	time:timeval
ENDOBJECT

OBJECT sm_newpattern
	sm:scalosmessage
	patternnode:PTR TO CHAR    -> PatternNode or NULL
ENDOBJECT

OBJECT sm_sleep
	sm:scalosmessage
ENDOBJECT

OBJECT sm_wakeup
	sm:scalosmessage
	relayout       -> BOOL
ENDOBJECT


OBJECT sm_showtitle
	sm:scalosmessage
	showtitle	 -> BOOL
ENDOBJECT

OBJECT sm_asyncroutine
	sm:scalosmessage
	entrypoint:asyncroutinefunc     -> address of start of code to execute
	args:CHAR			-> code will be executed with pointer to ScaWindowTask in A5
					-> and pointer to smar_Args in A0
					-> so, Message can easily be extended to pass arguments.
ENDOBJECT				-> Result from routine will be placed in EntryPoint

OBJECT sm_runprocess
	sm:scalosmessage
	windowtask:PTR TO scawindowtask
	entrypoint:runprocfunc    	-> address of start of code to execute as new process
	flags          			-> dunno, but it's long and it's used
ENDOBJECT

OBJECT sm_appsleep
	sm:scalosmessage
ENDOBJECT

OBJECT sm_appwakeup
	sm:scalosmessage
	relayout       -> BOOL
ENDOBJECT

OBJECT sm_redraw
	sm:scalosmessage
	flags          -> see below
ENDOBJECT

OBJECT sm_update
	sm:scalosmessage
ENDOBJECT

OBJECT sm_updateicon
	sm:scalosmessage
    	dirlock
    	iconname
ENDOBJECT

OBJECT sm_addicon
	sm:scalosmessage
	x:INT
	y:INT
    	dirlock
    	iconname
ENDOBJECT

OBJECT sm_remicon
	sm:scalosmessage
    	dirlock		/* +jl+ 20010217 */
    	iconname	/* +jl+ 20010217 */
ENDOBJECT

OBJECT sm_iconify
	sm:scalosmessage
ENDOBJECT

OBJECT sm_uniconify
	sm:scalosmessage
ENDOBJECT

OBJECT sm_requester 
	sm:scalosmessage
	smrq_ReqResult:PTR TO LONG	-> Result from EasyRequest()
	smrq_ParentWindow:PTR TO window
	smrq_MultiPurpose
	smrq_ArgList:PTR TO CHAR	-> pointer to ArgList
	smrq_ez:easystruct
	smrq_ArgListBuffer:CHAR		-> buffer for ArgList - allocated as large as needed
ENDOBJECT

OBJECT sm_asyncbackfill
	sm:scalosmessage
	smab_backfillfunc:runprocfunc		-> Backfill function entry point
	patternnode:PTR TO LONG
	smab_patterninfo:PTR TO patterninfo
	smab_screenstruct:PTR TO screen
	smab_windowtaskstruct:PTR TO scawindowtask
	smab_patterninfocopy:CHAR
ENDOBJECT

OBJECT sm_arexxroutine
	sm:scalosmessage
	smrx_entrypoint:arexxfunc
	smrx_RexxMsg:PTR TO rexxmsg
ENDOBJECT

OBJECT sm_menucmd
	smc_asyncroutine:sm_asyncroutine
	smc_flags:PTR TO LONG
	smc_iconNode:PTR TO  scaiconnode
ENDOBJECT

OBJECT sm_runmenucmd
	sm:scalosmessage
	scalosmenutree:PTR TO LONG
	iconnode:PTR TO  scaiconnode
	flags
ENDOBJECT

OBJECT sm_realupdateicon
	smc_asyncroutine:sm_asyncroutine
	arg:PTR TO wbarg
ENDOBJECT

OBJECT sm_showpopupmenu
	sm:scalosmessage
	smpm_popupmenu:PTR TO popupmenu
	iconnode:PTR TO scaiconnode
	flags
	filetypedef:PTR TO LONG		-> should be: filetypedef:PTR TO filetypedef 
	qualifier:INT			-> the smpm_Qualifier field is a copy of the current IntuiMessage's Qualifier
ENDOBJECT

-> ------------------------------------------------------------------

-> values in smpm_Flags - same as in mpm_Flags

OBJECT sm_showstatusbar
	sm:scalosmessage
	visible:LONG
ENDOBJECT

OBJECT sm_redrawicon
	sm:scalosmessage
	smri_icon:PTR TO scaiconnode
ENDOBJECT

OBJECT sm_dopopupmenu
	sm:scalosmessage
	ie:inputevent
ENDOBJECT

OBJECT sm_redrawiconobj
	sm:scalosmessage
	smrio_iconobject:PTR TO LONG -> pointer to object
	smrio_flags:PTR TO LONG
ENDOBJECT

-> values in smrio_Flags
CONST	SMRIOFLAGB_EraseIcon		= 0,
	SMRIOFLAGB_FreeLayout		= 1,
	SMRIOFLAGB_Highlight		= 2,	-> set this flag to indicate that there is only a change in icon highlighting state
	SMRIOFLAGB_IconListLocked	= 3,	-> flag: icon list is currently locked
	SMRIOFLAGB_HightlightOn		= 4,	-> flag: set ICONOBJ_USERFLAGF_DrawHighlite before drawing
	SMRIOFLAGB_HightlightOff	= 5	-> flag: clear ICONOBJ_USERFLAGF_DrawHighlite before drawing

#define SMRIOFLAGF_EraseIcon		Shl(1,SMRIOFLAGB_EraseIcon)
#define SMRIOFLAGF_FreeLayout		Shl(1,SMRIOFLAGB_FreeLayout)
#define SMRIOFLAGF_Highlight		Shl(1,SMRIOFLAGB_Highlight)
#define SMRIOFLAGF_IconListLocked	Shl(1,SMRIOFLAGB_IconListLocked)
#define	SMRIOFLAGF_HightlightOn		Shl(1,SMRIOFLAGB_HightlightOn)
#define	SMRIOFLAGF_HightlightOff	Shl(1,SMRIOFLAGB_HightlightOff)

OBJECT sm_newpreferences
	sm:scalosmessage
	prefsflags:PTR TO LONG
ENDOBJECT

-> values in smnp_PrefsFlags
-> one bit is set for each changed subsystem prefs
CONST	SMNPFLAGB_PATTERNPREFS		= 0,
	SMNPFLAGB_MENUPREFS		= 1,
	SMNPFLAGB_FONTPREFS		= 2,
	SMNPFLAGB_SCALOSPREFS		= 3,
	SMNPFLAGB_PALETTEPREFS		= 4,
	SMNPFLAGB_DEFICONSPREFS		= 5,
	SMNPFLAGB_LOCALEPREFS		= 6,
	SMNPFLAGB_FILETYPESPREFS	= 7

#define SMNPFLAGF_PATTERNPREFS		Shl(1,SMNPFLAGB_PATTERNPREFS)
#define SMNPFLAGF_MENUPREFS             Shl(1,SMNPFLAGB_MENUPREFS)
#define SMNPFLAGF_FONTPREFS             Shl(1,SMNPFLAGB_FONTPREFS)
#define SMNPFLAGF_SCALOSPREFS           Shl(1,SMNPFLAGB_SCALOSPREFS)
#define SMNPFLAGF_PALETTEPREFS          Shl(1,SMNPFLAGB_PALETTEPREFS)
#define SMNPFLAGF_DEFICONSPREFS         Shl(1,SMNPFLAGB_DEFICONSPREFS)
#define SMNPFLAGF_LOCALEPREFS           Shl(1,SMNPFLAGB_LOCALEPREFS)
#define SMNPFLAGF_FILETYPESPREFS        Shl(1,SMNPFLAGB_FILETYPESPREFS)

OBJECT sm_deltamove
	sm:scalosmessage
	deltax:LONG
	deltay:LONG
	adjustslider:PTR TO LONG
ENDOBJECT

OBJECT sm_setthumbnailimage_remapped
	sm:scalosmessage
	smtir_iconobject				-> the icon to attach the new image to
	smtir_newimage:PTR TO scalosbitmapandcolor	-> the new image. ScalosBitMapAndColor contents is freed by message handler
ENDOBJECT

OBJECT sm_setthumbnailimage_argb
	sm:scalosmessage
	smtia_iconobject			-> the icon to attach the new image to
	smtia_newimage:PTR TO argbheader	-> the new image. ARGBHeader contents is freed by message handler
ENDOBJECT

OBJECT sm_newwindowpath
	sm:scalosmessage
	path			-> Path of the new drawer to display in the window
	taglist:tagitem
ENDOBJECT

-> ------------------------------------------------------------------

OBJECT scabackdropicon
	node:mn
	sbi_icon:PTR TO LONG		 	-> IconObject from iconobject.library
	notifyreq:PTR TO LONG
	sbi_drawername
ENDOBJECT

-> This structure WILL grow in the future. Do not rely on its size!
-> Do NOT ALLOCATE ScaIconNode's yourself!
-> Do NOT MODIFY any member on your own !

OBJECT scaiconnode
	node:mn
	icon:PTR TO LONG                -> IconObject from iconobject.library
	name:PTR TO CHAR                -> Filename of the icon
	deviceicon:PTR TO scadeviceicon -> NULL except for disk icons
	lock:PTR TO LONG                -> lock to the files dir or NULL
	iconlist:PTR TO scabackdropicon -> list of linked icons (used for backdrop icons)
	flags:INT           		-> see below
	userdata:INT
	datestamp:datestamp
	typenode:PTR TO LONG
	filedatestamp:datestamp
	filesize:PTR TO LONG
	oldleftedge:INT
	oldtopedge:INT
ENDOBJECT

-> in_Flags:
CONST	INB_DefaultIcon			= 0,		-> the icon has diskobject
	INB_File			= 1,		-> File or Drawer (TextMode only)
	INB_TextIcon			= 2,		-> TextMode icon
	INB_Sticky			= 3,		-> icon not moveable
	INB_DropMarkVisible		= 4,		-> Icon drop mark current visible
	INB_VolumeWriteProtected	= 5,		-> volume is write-protected
	INB_FreeIconPosition		= 6,		-> Icon is stored with "no position"
	INB_IconVisible			= 7,		-> Icon has been drawn
	INB_Identified			= 8		-> Icon file type has been determined

#define INF_DefaultIcon			Shl(1,INB_DefaultIcon)
#define INF_File			Shl(1,INB_File)
#define INF_TextIcon			Shl(1,INB_TextIcon)
#define INF_Sticky			Shl(1,INB_Sticky)
#define INF_DropMarkVisible		Shl(1,INB_DropMarkVisible)
#define	INF_VolumeWriteProtected	Shl(1,INB_VolumeWriteProtected)
#define INF_FreeIconPosition		Shl(1,INB_FreeIconPosition)
#define INF_IconVisible			Shl(1,INB_IconVisible)
#define	INF_Identified			Shl(1,INB_Identified)

-> in_SupportFlags
CONST	INB_SupportsOpen	= 0,
	INB_SupportsCopy	= 1,
	INB_SupportsRename	= 2,
	INB_SupportsInformation = 3,
	INB_SupportsSnapshot	= 4,
	INB_SupportsUnSnapshot	= 5,
	INB_SupportsLeaveOut	= 6,
	INB_SupportsPutAway	= 7,
	INB_SupportsDelete	= 8,
	INB_SupportsFormatDisk	= 9,
	INB_SupportsEmptyTrash	= 10

#define INF_SupportsOpen	Shl(1,INB_SupportsOpen)
#define INF_SupportsCopy	Shl(1,INB_SupportsCopy)
#define INF_SupportsRename	Shl(1,INB_SupportsRename)
#define INF_SupportsInformation Shl(1,INB_SupportsInformation)
#define INF_SupportsSnapshot	Shl(1,INB_SupportsSnapshot)
#define INF_SupportsUnSnapshot	Shl(1,INB_SupportsUnSnapshot)
#define INF_SupportsLeaveOut	Shl(1,INB_SupportsLeaveOut)
#define INF_SupportsPutAway	Shl(1,INB_SupportsPutAway)
#define INF_SupportsDelete	Shl(1,INB_SupportsDelete)
#define INF_SupportsFormatDisk	Shl(1,INB_SupportsFormatDisk)
#define INF_SupportsEmptyTrash	Shl(1,INB_SupportsEmptyTrash)

-> ------------------------------------------------------------------
OBJECT scadeviceicon
	node:mn
	handler:PTR TO mp        	-> MessagePort of the HandlerTask
	volume:PTR TO CHAR          	-> Volumename or NULL
	device:PTR TO CHAR          	-> Devicename or NULL
	icon:PTR TO LONG                -> IconObject of this DeviceIcon struct
	info:PTR TO infodata           	-> Infodata from dos.library/Info()
	infobuf[4]:ARRAY OF infodata 	-> infodata
	flags:INT           		-> see below
	notifyreq:notifyrequest		-> for notification of devicewindow about icon changes
	diskiconname			-> this name was used to retrieve the current disk icon
ENDOBJECT

-> bits in di_Flags
CONST	DIBB_DontDisplay		= 0,		-> Don't display this icon
	DIBB_Remove			= 1,		-> ***internal use only***
	DIBB_InfoPending		= 2		-> ***internal use only***

#define DIBF_DontDisplay		Shl(1L,DIBB_DontDisplay)
#define DIBF_Remove			Shl(1L,DIBB_Remove)
#define DIBF_InfoPending		Shl(1L,DIBB_InfoPending)

-> for compatibility's sake
#define DIB_DontDisplay		DIBB_DontDisplay

-> ------------------------------------------------------------------

-> UpdateIcon structure for Icon and Device windowtypes

OBJECT scaupdateicon_iw
	ui_iw_lock         	-> Lock to the file's/dir's directory
	ui_iw_name:PTR TO CHAR  -> Name of the file or dir, NULL for Disks
ENDOBJECT

-> ------------------------------------------------------------------

OBJECT scalosnodelist
	minnode:PTR TO mn       -> Pointer to the first node or NULL
ENDOBJECT

-> ------------------------------- Scalos Class Information --------------------------

CONST SCC_Dummy		= $80580000

/*********************************************************
**			Root Class			**
**			Name: "Root.sca"		**
**********************************************************/

-> ---------------------- Attributes ------------------------------

CONST SCCA_WindowTask	= SCC_Dummy+1 -> (ISG)

-> ----------------------- Methodes -------------------------------

CONST   SCCM_HandleMessages	= SCC_Dummy+105,
	SCCM_ScalosMessage	= SCC_Dummy+106,
	SCCM_Message		= SCC_Dummy+107,
	SCCM_ScalosMessageReply = SCC_Dummy+108,
	SCCM_RunProcess		= SCC_Dummy+120,

-> APTR Routine
-> APTR Args
-> ULONG    size_of(Args)
-> APTR ReplyPort	see values below

	SCCV_RunProcess_NoReply	  =  0,
	SCCV_RunProcess_WaitReply = -1,

	SCCM_Ping		  = SCC_Dummy+140, -> Timer Ping

	SCCM_AddToClipboard	  = SCC_Dummy+172,
-> struct ScaWindowTask *
-> struct ScaIconNode *
-> ULONG Opcode

	SCCV_AddToClipboard_Copy  = 1,
	SCCV_AddToClipboard_Move  = 2,
	SCCM_ClearClipboard	  = SCC_Dummy+173,

	SCCM_GetLocalizedString	  = SCC_Dummy+190,
-> ULONG StringID

/*********************************************************
**			Title Class			**
**			Name: "Title.sca"		**
**********************************************************/

-> -------------- Attributes ------------

	SCCA_Title_Format	= SCC_Dummy+2, -> (IS.)
	SCCA_Title_Type		= SCC_Dummy+3, -> (I..)
	SCCV_Title_Type_Screen	= 0,	       -> (default)
	SCCV_Title_Type_Window	= 1,

-> --------------- Methods --------------

	SCCM_Title_Generate	= SCC_Dummy+101,
-> Result: MemPtr

	SCCM_Title_Query	= SCC_Dummy+102,
-> UWORD    ParseID
-> Result: BOOL (Needs Refresh)

	SCCM_Title_QueryTitle	= SCC_Dummy+103,
-> Result: BOOL (Needs Refresh)

	SCCM_Title_Translate	= SCC_Dummy+104,
-> CHAR *TitleMem	gets updated after call
-> CHAR *TitleFormat	gets updated after call
-> UWORD    ParseID
-> Result: succ BOOL

/*********************************************************
**			Window Class			**
**			Name: "Window.sca"		**
**********************************************************/

-> --------------- Methods -------------

	SCCM_Window_Open		= SCC_Dummy+123,
	SCCM_Window_Close		= SCC_Dummy+124,
	SCCM_Window_Iconify		= SCC_Dummy+125,
	SCCM_Window_UnIconify		= SCC_Dummy+126,
	SCCM_Window_InitClipRegion	= SCC_Dummy+127,
-> Result: OldClipRegion

	SCCM_Window_RemClipRegion	= SCC_Dummy+128,
-> APTR OldClipRegion

	SCCM_Window_SetTitle		= SCC_Dummy+129,
	SCCM_Window_SetInnerSize	= SCC_Dummy+143,
	SCCM_Window_LockUpdate		= SCC_Dummy+154,	-> +jl+ 20010420
	SCCM_Window_UnlockUpdate	= SCC_Dummy+155,	-> +jl+ 20010420

	SCCM_Window_ChangeWindow	= SCC_Dummy+189,	-> +jl+ 20040309

-> -------------- Attributes ------------

	SCCA_Window_IDCMP		= SCC_Dummy+2,	-> unused

/*********************************************************
**			IconWindow Class		**
**			Name: "IconWindow.sca"		**
**********************************************************/

-> -------------- IconWindow Attributes ------------

	SCCA_IconWin_Reading			= SCC_Dummy+1000,	-> (BOOL)  [SG] currently  reading directory
	SCCA_IconWin_SelectedCount		= SCC_Dummy+1001,	-> (ULONG) [SG] number of selected icons in window
	SCCA_IconWin_Typing			= SCC_Dummy+1002,	-> (BOOL)  [SG] user is currently tying icon name
	SCCA_IconWin_ShowType			= SCC_Dummy+1003,	-> (ULONG) [SG] Show All / Show only icons
	SCCA_IconWin_InnerWidth			= SCC_Dummy+1004,	-> (ULONG) [.G] icon window inner width
	SCCA_IconWin_InnerHeight		= SCC_Dummy+1005,	-> (ULONG) [.G] icon window inner height
	SCCA_IconWin_IconFont			= SCC_Dummy+1006,	-> (struct TextFont *) [.G] icon window icon font
	SCCA_IconWin_LayersLocked		= SCC_Dummy+1007,	-> (ULONG) [.G] Flag: icon window has Layers locked
	SCCA_IconWin_StatusBar			= SCC_Dummy+1008,	-> (BOOL)  [SG] Flag: Status bar is present
	SCCA_IconWin_ThumbnailView		= SCC_Dummy+1009,	-> (ULONG) [SG] Thumbnail display mode
	SCCA_IconWin_ThumbnailsGenerating	= SCC_Dummy+1010	-> (ULONG) [SG] Shows flag for thumbnail generation in progress


-> --------------- Methods -------------

CONST	SCCM_IconWin_ReadIconList	= SCC_Dummy+100,

-> ULONG    flags
-> Result: BOOL CloseWindow
-> Flags:

	SCCV_IconWin_ReadIconList_ShowAll 	= 1,

	SCCM_IconWin_ReadIcon	= SCC_Dummy+109,

-> CONST_STRPTR Name	icon name
-> struct ScaReadIconArg *	x/y position and parent lock, may be NULL

	SCCM_IconWin_DragBegin	= SCC_Dummy+110,	-> currently unused
	SCCM_IconWin_DragQuery	= SCC_Dummy+111,

-> struct DragEnter *
-> ---------- Returns : BOOL :
->	TRUE	yes, we may drop here
->	FALSE	no, no drop allowed


	SCCM_IconWin_DragFinish	= SCC_Dummy+112,

	SCCM_IconWin_DragEnter	= SCC_Dummy+149,	-> +jl+

-> struct DragEnter *
-> ---------- Returns : ./.

	SCCM_IconWin_DragLeave	= SCC_Dummy+150,	-> +jl+

-> struct DragEnter *
-> ---------- Returns : ./.

	SCCM_IconWin_DragDrop	= SCC_Dummy+113,

-> APTR		dropwindow
-> ULONG	mousex
-> ULONG	mousey
-> ULONG	Qualifier from IntuiMessage
-> ---------- Returns : ./.

	SCCM_IconWin_Open	=  SCC_Dummy+114

-> APTR iconnode    (IconNode to open or NULL)
-> ULONG	Flags
-> Result: BOOL

#define	ICONWINOPENB_IgnoreFileTypes	0
#define	ICONWINOPENF_IgnoreFileTypes	Shl(1,ICONWINOPENB_IgnoreFileTypes)

CONST	SCCM_IconWin_UpdateIcon = SCC_Dummy+115,
-> BPTR Lock
-> APTR Name

	SCCM_IconWin_AddIcon	= SCC_Dummy+116,
-> WORD x
-> WORD y
-> APTR Lock
-> APTR Name

	SCCM_IconWin_RemIcon	= SCC_Dummy+117,
-> BPTR Lock
-> CONST_STRPTR Name

	SCCM_IconWin_MakeWBArg	= SCC_Dummy+118,
-> struct ScaIconNode *
-> APTR Buffer
-> ---------- Returns : 1 = Success, 0 = Failure

	SCCM_IconWin_CleanUp		= SCC_Dummy+119,
	SCCM_IconWin_CleanUpByName	= SCC_Dummy+180,
	SCCM_IconWin_CleanUpByDate	= SCC_Dummy+181,
	SCCM_IconWin_CleanUpBySize	= SCC_Dummy+182,
	SCCM_IconWin_CleanUpByType	= SCC_Dummy+183,
	SCCM_IconWin_SetVirtSize	= SCC_Dummy+121
-> UWORD    Flags
-> Flags:
->  bit 0 = set Bottom-Slider
->  bit 1 = set Right-Slider
#define	SETVIRTB_AdjustBottomSlider	0
#define	SETVIRTB_AdjustRightSlider	1
#define	SETVIRTF_AdjustBottomSlider	Shl(1,SETVIRTB_AdjustBottomSlider)
#define	SETVIRTF_AdjustRightSlider	Shl(1,SETVIRTB_AdjustRightSlider)

CONST	SCCM_IconWin_Redraw		= SCC_Dummy+122
-> ULONG Flags

-> Values in Flags :
#define	REDRAWB_DontEraseWindow		0
#define	REDRAWB_ReLayoutIcons		1
#define	REDRAWF_DontEraseWindow		Shl(1L,REDRAWB_DontEraseWindow)
#define	REDRAWF_ReLayoutIcons		Shl(1L,REDRAWB_ReLayoutIcons)

CONST 	SCCM_IconWin_Update		= SCC_Dummy+130,

-> was SCCM_IconWin_CheckUpdate
	SCCM_IconWin_ScheduleUpdate	= SCC_Dummy+151,	-> +jl+
-> ./.

	SCCM_IconWin_Sleep		= SCC_Dummy+131,
	SCCM_IconWin_WakeUp		= SCC_Dummy+132,
	SCCM_IconWin_MenuCommand	= SCC_Dummy+138,
-> APTR CommandName
-> APTR IconNode(or NULL)
-> BOOL State(checkmark state if it's a checkmark)

	SCCM_IconWin_NewViewMode	= SCC_Dummy+139,
-> ULONG    ViewMode
-> Result: IconWindow.sca subclass object

	SCCM_IconWin_DrawIcon		= SCC_Dummy+141,
-> APTR Icon

	SCCM_IconWin_LayoutIcon		= SCC_Dummy+142
-> APTR  Icon
-> ULONG LayoutFlags

-> +jl+ Layout Flags see iconobject.h
#ifndef IOLAYOUTB_NormalImage
#define IOLAYOUTB_NormalImage			0	-> Layout normal Image
#define IOLAYOUTF_NormalImage			Shl(1,SCCLAYOUTB_NormalImage)
#define IOLAYOUTB_SelectedImage			1	-> Layout selected Image
#define IOLAYOUTF_SelectedImage			Shl(1,SCCLAYOUTB_NormalImage)
#endif


CONST	SCCM_IconWin_DeltaMove		= SCC_Dummy+144,
-> LONG deltaX
-> LONG deltaY

	SCCM_IconWin_GetDefIcon		= SCC_Dummy+146,
-> char*	filename
-> LONG 	filetype
-> ULONG	protectionbits

	SCCM_IconWin_ShowIconToolTip	= SCC_Dummy+156,	-> +jl+ 20010519
-> struct ScaIconNode *
-> ---------- Returns : ./.

	SCCM_IconWin_RawKey		= SCC_Dummy+158,	-> +jl+ 20010716
-> struct IntuiMessage *

	SCCM_IconWin_AddToStatusBar	= SCC_Dummy+159,	-> +jl+ 20011204
-> struct Gadget *
-> ULONG Tag, Tag, ... TAG_END

	SCCM_IconWin_RemFromStatusBar	= SCC_Dummy+160,	-> +jl+ 20011204
-> struct Gadget *

	SCCM_IconWin_UpdateStatusBar	= SCC_Dummy+161,	-> +jl+ 20011204
-> struct Gadget *
-> ULONG Tag, Tag, ... TAG_END

	SCCM_IconWin_ShowPopupMenu	= SCC_Dummy+166		-> +jl+ 20011209
-> struct PopupMenu *mpm_PopupMenu
-> struct ScaIconNode *mpm_IconNode(may be NULL for window popup menus)
-> ULONG mpm_Flags

-> Values in mpm_Flags
#define	SHOWPOPUPFLGB_IconSemaLocked		0
#define	SHOWPOPUPFLGB_WinListLocked		1
#define	SHOWPOPUPFLGB_FreePopupMenu		2
#define	SHOWPOPUPFLGF_IconSemaLocked		Shl(1L,SHOWPOPUPFLGB_IconSemaLocked)
#define	SHOWPOPUPFLGF_WinListLocked		Shl(1L,SHOWPOPUPFLGB_WinListLocked)
#define	SHOWPOPUPFLGF_FreePopupMenu		Shl(1L,SHOWPOPUPFLGB_FreePopupMenu)

CONST	SCCM_IconWin_ShowGadgetToolTip	= SCC_Dummy+171		-> +jl+ 20021001
-> ULONG sgtt_GadgetID

-> Values for sgtt_GadgetID
ENUM 						->sgttGadgetIDs
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
	SGTT_GADGETID_unknown

CONST	SCCM_IconWin_ActivateIconLeft		= SCC_Dummy+174,	-> +jl+ 20021201
	SCCM_IconWin_ActivateIconRight		= SCC_Dummy+175,	-> +jl+ 20021201
	SCCM_IconWin_ActivateIconUp		= SCC_Dummy+176,	-> +jl+ 20021201
	SCCM_IconWin_ActivateIconDown		= SCC_Dummy+177,	-> +jl+ 20021201
	SCCM_IconWin_ActivateIconNext		= SCC_Dummy+178,	-> +jl+ 20021201
	SCCM_IconWin_ActivateIconPrevious	= SCC_Dummy+179,	-> +jl+ 20021201
-> ./.

	SCCM_IconWin_AddGadget			= SCC_Dummy+184,	-> +jl+ 20030112
-> struct Gadget *

	SCCM_IconWin_ImmediateCheckUpdate	= SCC_Dummy+187,	-> +jl+
-> ./.

	SCCM_Window_GetIconFileType		= SCC_Dummy+168,	-> +jl+ 20020615
-> struct ScaIconNode *gift_IconNode

	SCCM_Window_ClearIconFileTypes		= SCC_Dummy+169,	-> +jl+ 20020615

/*********************************************************
**			DeviceWindow Class		**
**			Name: "DeviceWindow.sca"	**
**********************************************************/

-> --------------- Methods -------------

	SCCM_DeviceWin_ReadIcon			= SCC_Dummy+136,
-> APTR DeviceNode

	SCCM_DeviceWin_RemIcon			= SCC_Dummy+188,
-> struct ScaIconNode *

/*********************************************************
**			TextWindow Class		**
**			Name: "TextWindow.sca"		**
**********************************************************/

-> --------------- Methods -------------
	SCCM_TextWin_ReadIcon			= SCC_Dummy+137,	-> currently unimplemented
	SCCM_TextWin_ReAdjust			= SCC_Dummy+145,

-> +jl+ 20010320
-> hold all visible updates until SCCM_TextWin_EndUpdate
	SCCM_TextWin_BeginUpdate		= SCC_Dummy+152,
-> ULONG RefreshFlag
-> ---------- Returns : ./.

-> unlock window updates and do SCCM_TextWin_ReAdjust
	SCCM_TextWin_EndUpdate			= SCC_Dummy+153,
-> ---------- Returns : ./.

	SCCM_TextWin_InsertIcon			= SCC_Dummy+186,
-> struct ScaIconNode *
-> ---------- Returns : ./.

/*********************************************************
**			DeviceList Class		**
**			Name: "DeviceList.sca"		**
**********************************************************/

-> --------------- Methods -------------

	SCCM_DeviceList_Generate		= SCC_Dummy+133,
-> APTR ScalosNodeList

	SCCM_DeviceList_FreeDevNode		= SCC_Dummy+134,
-> APTR Node

	SCCM_DeviceList_Filter			= SCC_Dummy+135
-> APTR ScalosNodeList

/*********************************************************
**			FileTransfer Class		**
**			Name: "FileTransfer.sca"	**
**********************************************************/

ENUM FTOPCODE_Copy=1, FTOPCODE_Move	-> enum ftOpCodes { FTOPCODE_Copy=1, FTOPCODE_Move };

-> -------------- Attributes -----------

CONST	SCCA_FileTrans_Number			= SCC_Dummy+5,	 -> [I..]
	SCCA_FileTrans_Screen			= SCC_Dummy+6,	 -> [I..]
	SCCA_FileTrans_ReplaceMode		= SCC_Dummy+157, -> [I..] +jl+ 20010713

-> Values for SCCA_FileTrans_ReplaceMode
	SCCV_ReplaceMode_Ask			= 0,
	SCCV_ReplaceMode_Never			= 1,
	SCCV_ReplaceMode_Always			= 2,
	SCCV_ReplaceMode_Abort			= 3,

-> --------------- Methods -------------
	SCCM_FileTrans_Copy			= SCC_Dummy+147,
-> BPTR SourceLock
-> BPTR DestLock
-> CONST_STRPTR	SrcName
-> CONST_STRPTR	DestName
-> LONG MouseX
-> LONG MouseY

	SCCM_FileTrans_Move			= SCC_Dummy+148,	->new
-> BPTR SourceLock
-> BPTR DestLock
-> BPTR Name
-> LONG MouseX
-> LONG MouseY

	SCCM_FileTrans_CheckAbort		= SCC_Dummy+162,	->new +jl+ 20011207
-> input -
-> return RETURN_OK to continue 
-> or non-zero to abort moving/copying

	SCCM_FileTrans_OpenWindow		= SCC_Dummy+163,	->+jl+ 20011208
-> input -
-> returns struct Window *  - NULL if window could not be opened

	SCCM_FileTrans_OpenWindowDelayed 	= SCC_Dummy+164,	-> +jl+ 20011208
-> input -
-> returns struct Window *  - NULL if window could not be opened

	SCCM_FileTrans_UpdateWindow		= SCC_Dummy+165		-> +jl+ 20011209
-> ULONG UpdateMode
-> BPTR SrcDirLock
-> BPTR DestDirLock
-> CONST_STRPTR Name
-> returns -

ENUM	FTUPDATE_Initial, FTUPDATE_EnterNewDir, FTUPDATE_Entry, FTUPDATE_SameFile, FTUPDATE_Final
 -> FILETRANS_UpdateMode

	
CONST	SCCM_FileTrans_CountTimeout		= SCC_Dummy+167,	-> +jl+ 20011214
-> input -
-> returns non-zero value if CountTimeout is exceeded and counting should be aborted.

	SCCM_FileTrans_OverwriteRequest		= SCC_Dummy+185	-> +jl+ 20011214
-> enum OverWriteReqType requestType
-> BPTR srcLock
-> CONST_STRPTR srcName
-> BPTR destLock
-> CONST_STRPTR destName
-> struct Window *parentWindow
-> ULONG suggestedBodyTextId
-> ULONG suggestedGadgetTextId
-> returns ExistsReqResult

ENUM 	OVERWRITEREQ_Copy,OVERWRITEREQ_Move,OVERWRITEREQ_CopyIcon -> OverWriteReqType

ENUM  	EXISTREQ_Replace = 1,EXISTREQ_Skip,EXISTREQ_ReplaceAll,EXISTREQ_SkipAll,EXISTREQ_Abort = 0 -> ExistsReqResult

/*****************************************************************
**		this is the struct of every Scalos		**
**			Root.sca subclass			**
******************************************************************/

OBJECT scarootlist
	windowtask:PTR TO scawindowtask         -> Pointer to a WindowTask structure
	interninfos:PTR TO scainterninfos       -> Pointer to a Internal Infos struct
ENDOBJECT                                       -> !! This is not the end of this struct

OBJECT scainterninfos
	mainmsgport:PTR TO mp                	-> Main message port
	mainwindowstruct:PTR TO scawindowstruct -> Pointer into the windowlist
						-> from Desktop Window
	appwindowstruct:PTR TO scawindowstruct  -> Pointer into the windowlist 
						-> from Window for AppIcons
	screen:PTR TO screen                    -> the Scalos Screen
	drawinfo:PTR TO drawinfo                -> DrawInfo from this screen
	visualinfo:PTR TO CHAR          	-> Gadtools VisualInfo
ENDOBJECT


/** extended Library base for Scalos OOP plugins **/
OBJECT scaooppluginbase
	soob_library	-> Standard Library base
	soob_signature		-> Must be 'PLUG'
ENDOBJECT

OBJECT scaclassinfo
	hook:hook           		-> Dispatcher Hook
	priority:LONG          		-> Where to insert the class (-128 to 127)
	instsize:INT           		-> Instance size
	neededversion:INT      		-> needed Scalos Version (normaly -> this is 39)
	reserved:INT           		-> should be NULL
	classname:PTR TO CHAR       	-> name of your class (max 50 chars)
	superclassname:PTR TO CHAR  	-> name of the superclass (max 50 chars)
	name:PTR TO CHAR            	-> real name (max 100 chars)
	description:PTR TO CHAR     	-> short description (max 200 chars)
	makername:PTR TO CHAR       	-> name of the programmer
ENDOBJECT



/** This is the parameter structure for Scalos tooltip info string plugin functions **/
OBJECT scatooltipinfohookdata
	iconnode:PTR TO scaiconnode	-> icon node of the file/drawer/disk to identify
	filelock			-> READ lock on the file/drawer/disk to identify
	fib:PTR TO fileinfoblock	-> FileInfoBlock, valid if ttshd_FileLock != NULL
	buffer				-> Buffer to fill result string in
	bufflen				-> Length of ttshd_Buffer
ENDOBJECT

/** Parameter structure for SCCM_IconWin_ReadIcon **/
OBJECT scareadiconarg
	x
	y
	lock
ENDOBJECT

/** ----------------------- Scalos Class Hierarchy -------------------------
	(priority)

	-+- Root.sca (-128)
	 |
	 +-- Window.sca (-90)
	 |
	 +--- Title.sca (-80)
	 |
	 +--- DeviceList.sca (-80)
	 |
	 +-+- IconWindow.sca (-80)
	   |
	   +-- DeviceWindow.sca (-60)
	   |
	   +-- TextWindow.sca (-60)


------------------------------------------------------------------------ **/


OBJECT scaappobjnode
	minnode:PTR TO mn	/* Pretty much all the structures with two longs
			 	  at the start are actually a MinNode (think 
				  about it, you need a list of AppObjects) */
	kennug			/* for want of a better name, holds "SC" as identifier */
	type
	id
	userdata
	msg:PTR TO mp
	object
	userdata2
ENDOBJECT

/**
 types:
  0 = AppIcons		object: IconObject
  1 = AppWindow		object: Window
  2 = AppMenuItem	object: MenuItem
**/

OBJECT dragenter
	window:PTR TO scawindowstruct	-> Window from wich dragging started
	icon:PTR TO scaiconnode		-> Icon under mouse pointer or NULL
	mousex
	mousey
ENDOBJECT

-> Flag Values for SCA_DrawDrag()
#define SCAB_Drag_Transparent		0
#define SCAF_Drag_Transparent		Shl(1,SCAB_Drag_Transparent)
#define SCAB_Drag_IndicateCopy		1		-> +jl+ draw indicator for copying
#define SCAF_Drag_IndicateCopy		Shl(1,SCAB_Drag_IndicateCopy)
#define SCAB_Drag_NoDropHere		2		-> +jl+ nothing may be dropped here
#define SCAF_Drag_NoDropHere		Shl(1,SCAB_Drag_NoDropHere)
#define SCAB_Drag_Hide			3		-> +jl+ hide Bobs
#define SCAF_Drag_Hide			Shl(1,SCAB_Drag_Hide)
#define SCAB_Drag_IndicateMakeLink	4		-> +jl+ draw indicator for Make-Link
#define SCAF_Drag_IndicateMakeLink	Shl(1,SCAB_Drag_IndicateMakeLink)
#define	SCAB_Drag_NoDrawDrag		5		-> +jl+ 20010802 internal use only
#define	SCAF_Drag_NoDrawDrag		Shl(1,SCAB_Drag_NoDrawDrag)

/****************************************************************************
*****************************************************************************/
-> +jl+ 20010623
-> Tags for ScalosControlA() library call

CONST	SCALOSCONTROL_BASE		= $80530300

/*
** Set the default stack size for launching programs with (ULONG).
*/
	#define	SCALOSCTRLA_GetDefaultStackSize	SCALOSCONTROL_BASE+1

/*
** Set the default stack size for launching programs with (ULONG).
*/
	#define	SCALOSCTRLA_SetDefaultStackSize	SCALOSCONTROL_BASE+2

/*
** Get a list of currently running Workbench programs (struct List **).
*/
	#define	SCALOSCTRLA_GetProgramList	SCALOSCONTROL_BASE+3

/*
** Release the list of currently running Workbench programs (struct List *).
*/
	#define	SCALOSCTRLA_FreeProgramList	SCALOSCONTROL_BASE+4

/*
** Get the current state of the "Disable CloseWorkbench()" flag
*/
	#define	SCALOSCTRLA_GetCloseWBDisabled	SCALOSCONTROL_BASE+5

/*
** Set the state of the "Disable CloseWorkbench()" flag
*/
	#define	SCALOSCTRLA_SetCloseWBDisabled	SCALOSCONTROL_BASE+6

/*
** Get the current state of the boot-up splash window flag
*/
	#define	SCALOSCTRLA_GetSplashEnable	SCALOSCONTROL_BASE+7

/*
** Turn off/on boot-up splash window
*/
	#define	SCALOSCTRLA_SetSplashEnable	SCALOSCONTROL_BASE+8

/*
** Get the current icon tool tip enabled state
*/
	#define	SCALOSCTRLA_GetToolTipEnable	SCALOSCONTROL_BASE+9

/*
** Turn off/on icon tool tips
*/
	#define	SCALOSCTRLA_SetToolTipEnable	SCALOSCONTROL_BASE+10

/*
** Get the delay for icon tool tips
*/
	#define SCALOSCTRLA_GetToolTipDelay	SCALOSCONTROL_BASE+11

/*
** Set the delay for icon tool tips
*/
	#define SCALOSCTRLA_SetToolTipDelay	SCALOSCONTROL_BASE+12

/*
** Get the current icon tool tip enabled state
*/
	#define SCALOSCTRLA_GetOldDragIconMode	SCALOSCONTROL_BASE+13

/*
** Turn off/on icon tool tips
*/
	#define SCALOSCTRLA_SetOldDragIconMode	SCALOSCONTROL_BASE+14

/*
** Get the number of seconds to pass before typing next 
** character in a drawer window will restart
** with new file name (ULONG *).
*/
	#define SCALOSCTRLA_GetTypeRestartTime	SCALOSCONTROL_BASE+15

/*
** Set number of seconds to pass before typing
** next character in a drawer window will restart
** with new file name (ULONG).
*/
	#define SCALOSCTRLA_SetTypeRestartTime	SCALOSCONTROL_BASE+16

/*
** Get a BOOL variable telling whether Scalos
** is running in emulation mode (TRUE)
** or in preview mode (FALSE)
*/
	#define SCALOSCTRLA_GetEmulationMode	SCALOSCONTROL_BASE+17

/*
** Get a BOOL variable telling whether Scalos
** displays a status bar in every non-backdrop window
*/
	#define SCALOSCTRLA_GetStatusBarEnable	SCALOSCONTROL_BASE+18

/*
** Tell whether Scalos to display a status 
** bar in every non-backdrop window (any non-zero value)
** or to display no status bars (0 = FALSE)
** Changes only apply to windows opened thereafter.
*/
	#define SCALOSCTRLA_SetStatusBarEnable	SCALOSCONTROL_BASE+19

/*
** Obtain the current hook that will be invoked when Scalos
** starts copying files and data (struct Hook **) (V40)
*/
	#define SCALOSCTRLA_GetCopyHook		SCALOSCONTROL_BASE+20

/*
** Install a new hook that will be invoked when Scalos
** starts copying files and data (struct Hook *) (V40)
*/
	#define SCALOSCTRLA_SetCopyHook		SCALOSCONTROL_BASE+21

/*
** Obtain the current hook that will be invoked when Scalos deletes
** files and drawers or empties the trashcan (struct Hook **)
** (V40).
*/
	#define SCALOSCTRLA_GetDeleteHook	SCALOSCONTROL_BASE+22

/*
** Install a new hook that will be invoked when Scalos deletes
** files and drawers or empties the trashcan (struct Hook *)
** (V40).
*/
	#define SCALOSCTRLA_SetDeleteHook	SCALOSCONTROL_BASE+23

/*
** Obtain the current hook that will be invoked when Scalos requests
** the user to enter text, i.e. when a file is to be renamed
** or a new drawer is to be created (struct Hook **) (V40)
*/
	#define SCALOSCTRLA_GetTextInputHook	SCALOSCONTROL_BASE+24

/*
** Install a new hook that will be invoked when Scalos requests
** the user to enter text, i.e. when a file is to be renamed
** or a new drawer is to be created (struct Hook *) (V40)
*/
	#define SCALOSCTRLA_SetTextInputHook	SCALOSCONTROL_BASE+25

/*
** Add a hook that will be invoked when Scalos is about 
** to close (cleanup), and when Scalos has opened 
** again (setup) (struct Hook *); (V40)
*/
	#define SCALOSCTRLA_AddCloseWBHook	SCALOSCONTROL_BASE+26

/*
** Remove a hook that has been installed with the
** SCALOSCTRLA_AddCloseWBHook tag (struct Hook *); (V40)
*/
	#define SCALOSCTRLA_RemCloseWBHook	SCALOSCONTROL_BASE+27

/*
** Get a BOOL variable telling whether Scalos
** displays horizontally striped lines in text windows
*/
	#define SCALOSCTRLA_GetStripedTextWindows SCALOSCONTROL_BASE+28

/*
** Tell whether Scalos to display  horizontally 
** striped lines in text windows
** Changes only apply to windows opened thereafter.
*/
	#define SCALOSCTRLA_SetStripedTextWindows SCALOSCONTROL_BASE+29

/*
** Get a BOOL variable telling whether Scalos
** shows the number of dragged objects during D&D
*/
	#define SCALOSCTRLA_GetDisplayDragCount	  SCALOSCONTROL_BASE+30

/*
** Tell whether Scalos to display  
** the number of dragged objects during D&D
*/
	#define SCALOSCTRLA_SetDisplayDragCount	  SCALOSCONTROL_BASE+31

/*
** Get a ULONG variable telling when Scalos
** shows window drop marks during D&D 
** 0 - never, 1 - only on non-backdrop windows, 2 - always
*/
	#define SCALOSCTRLA_GetWindowDropMarkMode SCALOSCONTROL_BASE+32

/*
** Set when Scalos will show window drop marks during D&D 
** 0 - never, 1 - only on non-backdrop windows, 2 - always
*/
	#define SCALOSCTRLA_SetWindowDropMarkMode SCALOSCONTROL_BASE+33

/*
** Get bit mask of all currently enabled icon types
** IDTV_IconType_NewIcon - display NewIcons
** IDTV_IconType_ColorIcon - display OS3.5 color icons
**/
	#define SCALOSCTRLA_GetSupportedIconTypes SCALOSCONTROL_BASE+34

/*
** Select which icon types are enabled :
** IDTV_IconType_NewIcon - display NewIcons
** IDTV_IconType_ColorIcon - display OS3.5 color icons
*/
	#define SCALOSCTRLA_SetSupportedIconTypes  SCALOSCONTROL_BASE+35


/*
** Get a list of all supported menu command names (struct List **).
*/
	#define SCALOSCTRLA_GetMenuCommandList	SCALOSCONTROL_BASE+36

/*
** Release the list of all supported menu command names (struct List *).
*/
	#define SCALOSCTRLA_FreeMenuCommandList	SCALOSCONTROL_BASE+37


/*
** Get the size of the buffer used for moving and 
** copying files (ULONG *).
*/
	#define SCALOSCTRLA_GetCopyBuffSize	SCALOSCONTROL_BASE+38

/*
** Set the size of the buffer used for moving and 
** copying files (ULONG).
** Min = 4 KBytes, Max = 4 MBytes
*/
	#define SCALOSCTRLA_SetCopyBuffSize	SCALOSCONTROL_BASE+39


/****************************************************************************/
/*	Method structures for all Scalos SCCM_*** class methods		    */
/****************************************************************************/

-> --- RootClass methods ------------------------------------------------

-> SCCM_RunProcess

OBJECT runprocess
	methodid:PTR TO LONG
	entrypoint -> RUNPROCFUNC
	args
	argsize
	replyport:PTR TO mp
ENDOBJECT

-> SCCM_Message

OBJECT message
	methodid:PTR TO LONG
	msg:PTR TO intuimessage
ENDOBJECT 

-> SCCM_AddToClipboard

OBJECT addtoclipboard
	methodid:PTR TO LONG
	wt:PTR TO scawindowtask 
	in:PTR TO scaiconnode
	ftopcodes
	opcode
ENDOBJECT 

-> SCCM_GetLocalizedString

OBJECT getlocstring
	methodid:PTR TO LONG
	stringid:PTR TO LONG
ENDOBJECT 

-> --- DeviceWindowClass methods ----------------------------------------

-> SCCM_DeviceWin_ReadIcon

OBJECT devwinreadicon
	methodid:PTR TO LONG
	devicenode:PTR TO scadeviceicon
ENDOBJECT 

-> SCCM_DeviceWin_RemIcon

OBJECT devwinremicon
	methodid:PTR TO LONG
	iconnode:PTR TO scaiconnode
ENDOBJECT 

-> --- IconWindowClass methods ------------------------------------------

-> SCCM_IconWin_ReadIconList

OBJECT readiconlist
	methodid:PTR TO LONG
	flags
ENDOBJECT 

-> SCCM_IconWin_ShowIconToolTip

OBJECT showicontooltip
	methodid:PTR TO LONG
	iconnode:PTR TO scaiconnode
ENDOBJECT 

-> SCCM_IconWin_ShowGadgetToolTip

OBJECT showgadgettooltip
	methodid:PTR TO LONG
	gadgetid:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_RawKey

OBJECT rawkey
	methodid:PTR TO LONG
	msg:PTR TO intuimessage
ENDOBJECT 

-> SCCM_IconWin_DragBegin

OBJECT dragbegin
	methodid:PTR TO LONG
	dragenter:dragenter
ENDOBJECT 

-> SCCM_IconWin_DragDrop

OBJECT dragdrop
	methodid:PTR TO LONG
	dropwindow:PTR TO window
	mousex:PTR TO LONG
	mousey:PTR TO LONG
	qualifier:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_Open

OBJECT open
	methodid:PTR TO LONG
	iconnode:PTR TO scaiconnode
	flags:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_AddIcon

OBJECT addicon
	methodid:PTR TO LONG
	x
	y
	lock
	name
ENDOBJECT 

-> SCCM_IconWin_RemIcon

OBJECT remicon
	methodid:PTR TO LONG
	lock
	name
ENDOBJECT 

-> SCCM_IconWin_MakeWBArg

OBJECT makewbarg
	methodid:PTR TO LONG
	icon:PTR TO scaiconnode
	buffer:PTR TO wbarg
ENDOBJECT 

-> SCCM_IconWin_SetVirtSize

OBJECT setvirtsize
	methodid:PTR TO LONG
	flags:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_Redraw

OBJECT redraw
	methodid:PTR TO LONG
	flags:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_WakeUp

OBJECT wakeup
	methodid:PTR TO LONG
	relayout:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_DrawIcon

OBJECT drawicon
	methodid:PTR TO LONG
	iconobject:PTR TO object
ENDOBJECT 

-> SCCM_IconWin_LayoutIcon

OBJECT layouticon
	methodid:PTR TO LONG
	iconobject:PTR TO object
	layoutflags:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_GetDefIcon

OBJECT getdeficon
	methodid:PTR TO LONG
	name
	type			-> fib_DirEntryType from FileInfoBlock
	protection:PTR TO LONG	-> fib_Protection from FileInfoBlock
ENDOBJECT 

-> SCCM_IconWin_MenuCommand

OBJECT menucommand
	methodid:PTR TO LONG
	cmdname
	iconnode:PTR TO scaiconnode
	state:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_NewViewMode

OBJECT newviewmode
	methodid:PTR TO LONG
	newmode:PTR TO LONG
ENDOBJECT 

-> SCCM_IconWin_DeltaMove

OBJECT deltamove
	methodid:PTR TO LONG
	deltax
	deltay
ENDOBJECT 

-> SCCM_IconWin_AddToStatusBar

OBJECT addtostatusbar
	methodid:PTR TO LONG
	newmember:PTR TO gadget
	taglist:PTR TO LONG 	-> ULONG mab_TagList[0]
ENDOBJECT 

-> SCCM_IconWin_RemFromStatusBar

OBJECT remfromstatusbar
	methodid:PTR TO LONG
	oldmember:PTR TO gadget
ENDOBJECT 

-> SCCM_IconWin_UpdateStatusBar

OBJECT updatestatusbar
	methodid:PTR TO LONG
	member:PTR TO gadget
	taglist:PTR TO LONG 	-> ULONG mub_TagList[0]
ENDOBJECT 

-> SCCM_IconWin_AddGadget

OBJECT addgadget
	methodid:PTR TO LONG
	newgadget:PTR TO gadget
ENDOBJECT 

-> SCCM_IconWin_ReadIcon

OBJECT readicon
	methodid:PTR TO LONG
	name			-> icon name
	ria:PTR TO scareadiconarg
ENDOBJECT 

-> SCCM_IconWin_ShowPopupMenu

OBJECT showpopupmenu
	methodid:PTR TO LONG
	popupmenu:PTR TO popupmenu
	iconnode:PTR TO scaiconnode
	flags:PTR TO LONG
	qualifier:PTR TO LONG
ENDOBJECT 

-> --- TextWindowClass methods ------------------------------------------

-> SCCM_TextWin_EndUpdate

OBJECT endupdate
	methodid:PTR TO LONG
	refreshflag:PTR TO LONG
ENDOBJECT 

-> SCCM_TextWin_InsertIcon

OBJECT inserticon
	methodid:PTR TO LONG
	iconnode:PTR TO scaiconnode
ENDOBJECT 

-> --- TextWindowClass methods ------------------------------------------

OBJECT translate
    methodid:PTR TO LONG
    buffer:PTR TO LONG      -> Buffer being written to, gets updated after call
    titleformat    		-> current title format pointer, gets updated after call
    parseid        		-> ID containing the two-character variable name, e.g. "os"
    bufflen        		-> Length of mxl_Buffer
ENDOBJECT

-> ---  MSG_QUERY -------------------------------------------------------

OBJECT query
    methodid:PTR TO LONG
    parseid        -> ID containing the two-character variable name, e.g. "os"
ENDOBJECT

-> --- WindowClass methods ------------------------------------------

OBJECT remclipregion
	methodid:PTR TO LONG
	oldclipregion:PTR TO region
ENDOBJECT 

OBJECT settitle
	methodid:PTR TO LONG
	title
ENDOBJECT 

OBJECT geticonfiletype
	methodid:PTR TO LONG
	iconnode:PTR TO scaiconnode
ENDOBJECT 

-> --- FileTransClass methods ------------------------------------------

OBJECT move
	methodid:PTR TO LONG
	srcdirlock
	destdirlock
	name
	mousex
	mousey
ENDOBJECT 

OBJECT copy
	methodid:PTR TO LONG
	srcdirlock
	destdirlock
	srcname
	destname
	mousex
	mousey
ENDOBJECT 

OBJECT updatewindow
	methodid:PTR TO LONG
	updatemode:PTR TO LONG
	srcdirlock
	destdirlock
	srcname
ENDOBJECT 

OBJECT overwriterequest
	methodid:PTR TO LONG
	overwritereqtype
	requesttype
	srclock
	srcname
	destlock
	destname
	parentwindow:PTR TO window
	suggestedbodytextid:PTR TO LONG
	suggestedgadgettextid:PTR TO LONG
ENDOBJECT 

-> possible selections for thumbnail display
ENUM 	THUMBNAILS_Never,
	THUMBNAILS_AsDefault,
	THUMBNAILS_Always

-> Icon layout modes

#define	ICONTYPE_MAX			(1 + WBAPPICON)
#define ICONLAYOUT_RESTRICTED_MAX	4

ENUM	ICONLAYOUT_Columns,		-> Layout top-down in columns (default)
	ICONLAYOUT_Rows			-> Layout left-to-right in rows

/****************************************************************************/

-> Scalos library base

OBJECT scalosbase
	scb_LibNode:PTR TO lib
	scb_Pad[2]:ARRAY OF LONG	-> pad to next longword border
	scb_Revision			-> "1.4f" or "1.5" etc.
ENDOBJECT

/****************************************************************************/

#endif	/* SCALOS_SCALOS_H */


