	IFND	SCALOS_I
SCALOS_I	SET	1
**
**  $VER: scalos.h 41.9 (05 Feb 2011 20:24:29)
**
** $Date$
** $Revision$
**
**      Scalos.library include
**
**   (C) Copyright 1996-1997 ALiENDESiGN
**   (C) Copyright 1999-2010 The Scalos Team
**	All Rights Reserved
**

	include	scalos/iconobject.i
	include	graphics/regions.i
	include	workbench/startup.i

;---------------------------------------------------------------------------

SCA_TagBase                     EQU	$80530000

SCA_IconObject			EQU     (SCA_TagBase+1)
SCA_Stacksize			EQU     (SCA_TagBase+2)
SCA_Flags			EQU     (SCA_TagBase+3)
SCA_IconNode			EQU     (SCA_TagBase+4)
SCA_WindowTitle			EQU     (SCA_TagBase+5)
SCA_Path			EQU     (SCA_TagBase+6)
SCA_WindowRect			EQU     (SCA_TagBase+7)
SCA_XOffset			EQU     (SCA_TagBase+8)
SCA_YOffset			EQU     (SCA_TagBase+9)
SCA_PatternNumber		EQU     (SCA_TagBase+10)	; see pattern.i
SCA_ShowAllFiles		EQU     (SCA_TagBase+11)	; obsolete, use SCA_ShowAllMode instead!
SCA_ViewModes			EQU     (SCA_TagBase+12)
SCA_MessagePort			EQU     (SCA_TagBase+13)
SCA_Iconify			EQU     (SCA_TagBase+14)
SCA_ClassName			EQU     (SCA_TagBase+15)
SCA_Class			EQU     (SCA_TagBase+16)
SCA_WaitTimeout			equ	(SCA_TagBase+17)       	; +jl+ 20010324
SCA_SemaphoreExclusive		EQU	(SCA_TagBase+18)       	; +jl+ 20010408
SCA_SemaphoreShared		EQU	(SCA_TagBase+19)       	; +jl+ 20010408
SCA_NoStatusBar			EQU	(SCA_TagBase+20)       	; +jl+ 20011111
SCA_NoActivateWindow		EQU	(SCA_TagBase+21)       	; +jl+ 20020512
SCA_Priority			EQU	(SCA_TagBase+22)       	; +jl+ 20020823
SCA_WindowStruct		EQU	(SCA_TagBase+23)       	; +jl+ 20021204
SCA_SemaphoreExclusiveNoNest	EQU	(SCA_TagBase+24)       	; +jl+ 20040904
SCA_ShowAllMode			EQU	(SCA_TagBase+25)       	; +jl+ 20050511
SCA_IconList			EQU	(SCA_TagBase+26)       	; struct ScaIconNode *
SCA_BrowserMode			EQU	(SCA_TagBase+27)       	; ULONG
SCA_HideStatusBar		EQU	(SCA_TagBase+28)       	; ULONG
SCA_CheckOverlappingIcons	EQU	(SCA_TagBase+29)       	; ULONG
SCA_TransparencyActive		EQU	(SCA_TagBase+30)	; ULONG - percentage of opacity in active window state, 0=transparent, 100=opaque
SCA_TransparencyInactive	EQU	(SCA_TagBase+31)	; ULONG - percentage of opacity in inactive window state, 0=transparent, 100=opaque
SCA_DdPopupWindow		EQU	(SCA_TagBase+32)	; ULONG - Flag: this window has popped up during D&D operation
SCA_NoControlBar 		EQU	(SCA_TagBase+33)	; don't display control bar for this window

	BITDEF	SCA,WBStart_NoIcon,0
	BITDEF	SCA,WBStart_Wait,1
	BITDEF	SCA,WBStart_PathSearch,2
	BITDEF	SCA,WBStart_NoIconCheck,3

; for compatibility
SCAV_WBStart_NoIcon		EQU	SCAF_WBStart_NoIcon
SCAV_WBStart_Wait		EQU	SCAF_WBStart_Wait
SCAV_WBStart_PathSearch		EQU	SCAF_WBStart_PathSearch
SCAV_WBStart_NoIconCheck	EQU	SCAF_WBStart_NoIconCheck

	BITDEF	SCA,OpenWindow_ScalosPort,0		;private

; values in ws_ViewModes
SCAV_ViewModes_Default          EQU	IDTV_ViewModes_Default
SCAV_ViewModes_Icon		EQU	IDTV_ViewModes_Icon
SCAV_ViewModes_Name		EQU	IDTV_ViewModes_Name
SCAV_ViewModes_Size		EQU	IDTV_ViewModes_Size
SCAV_ViewModes_Date		EQU	IDTV_ViewModes_Date
SCAV_ViewModes_Time		EQU	IDTV_ViewModes_Time
SCAV_ViewModes_Comment		EQU	IDTV_ViewModes_Comment
SCAV_ViewModes_Protection	EQU	IDTV_ViewModes_Protection
SCAV_ViewModes_Owner		EQU	IDTV_ViewModes_Owner
SCAV_ViewModes_Group		EQU	IDTV_ViewModes_Group

;LockWindowList():
SCA_LockWindowList_Shared		EQU	0
SCA_LockWindowList_Exclusiv		EQU	1
SCA_LockWindowList_AttemptShared   	EQU	2
SCA_LockWindowList_AttemptExclusive	EQU	3

;FreeWBArgs():
	BITDEF	SCA,FreeLocks,0
	BITDEF	SCA,FreeNames,1

SCA_SortType_Bubble		EQU	0
SCA_SortType_Selection		EQU	1
SCA_SortType_Insertion		EQU	2
SCA_SortType_Quick		EQU	3
SCA_SortType_Best		EQU	-1


;---------------------- NodeTypes for AllocStdNode() ------------------------

NTYP_IconNode			EQU	0
NTYP_WindowTask			EQU	1
NTYP_DragNode			EQU	2
NTYP_PatternNode		EQU	3
NTYP_ScalosArg			EQU	4
NTYP_DeviceIcon			EQU	5
NTYP_MenuTree			EQU	6
NTYP_MenuCommand		EQU	7
NTYP_AppObject			EQU	8
NTYP_MenuInfo			EQU	9
NTYP_PluginClass		equ	10	; +jl+
NTYP_PluginNode			equ	11	; +jl+

;------------------------ MessageTypes for AllocMsg() -----------------------

MTYP_DropProc			equ	1
MTYP_CloseWindow		EQU	2
MTYP_StartWindow		equ	3
MTYP_Timer			EQU	4
MTYP_Requester			equ	5
MTYP_NewPattern			EQU	6
MTYP_Sleep			EQU	7
MTYP_Wakeup			EQU	8
MTYP_AsyncRoutine		equ	9
MTYP_RunProcess			equ	10
MTYP_AppSleep			EQU	11
MTYP_AppWakeup			EQU	12
MTYP_Redraw			EQU	13
MTYP_Update			EQU	14
MTYP_UpdateIcon			EQU	15
MTYP_AddIcon			EQU	16
MTYP_RemIcon			EQU	17
MTYP_ARexxRoutine		equ	18
MTYP_Iconify			EQU	19
MTYP_UnIconify			EQU	20
MTYP_AsyncBackFill		equ	21
MTYP_ShowTitle			equ	22
MTYP_RunMenuCmd			equ	23	; +jl+ 20010415
MTYP_ShowPopupMenu		equ	24	; +jl+ 20011209
MTYP_ShowStatusBar		equ	25	; +jl+ 20040919
MTYP_RedrawIcon			equ	26	; +jl+ 20050303
MTYP_DoPopupMenu		equ	27	; +jl+ 20050319
MTYP_RedrawIconObj		equ	28
MTYP_NewPreferences		equ	29
MTYP_DeltaMove			equ	30
MTYP_SetThumbnailImage_Remapped	equ	31
MTYP_SetThumbnailImage_ARGB     equ	32
MTYP_NewWindowPath		equ	33
MTYP_PrefsChanged		equ	34
MTYP_StartChildProcess		equ	35
MTYP_RootEvent			equ	36
MTYP_ShowControlBar		equ	37

mtyp_max = 38

    STRUCTURE ScalosMessage,MN_SIZE
	ULONG	Signature	;("IMSG")
	ULONG	MessageType	;AllocMsg() type
	LABEL	scm_SIZEOF

    STRUCTURE SM_StartWindow,scm_SIZEOF
	APTR	smsw_WindowStruct	; struct ScaWindowStruct *
	LABEL	smsw_SIZEOF


    STRUCTURE SM_CloseWindow,scm_SIZEOF
	LABEL	smmc_SIZEOF

    STRUCTURE SM_Timer,scm_SIZEOF
	APTR	smtm_WindowStruct
	STRUCT	smtm_Time,TV_SIZE	; struct timeval
	LABEL	smtm_SIZEOF

    STRUCTURE SM_NewPattern,scm_SIZEOF
	APTR	sm_np_PatternNode		;PatternNode or NULL
	LABEL	smnp_SIZEOF

    STRUCTURE SM_Sleep,scm_SIZEOF
	LABEL	smms_SIZEOF

    STRUCTURE SM_Wakeup,scm_SIZEOF
	ULONG	sm_mw_ReLayout			;BOOL
	LABEL	smmw_SIZEOF

    STRUCTURE SM_AsyncRoutine,scm_SIZEOF
	APTR	smar_EntryPoint			; address of start of code to execute
						; code will be executed with pointer to ScaWindowTask in A5
						; and pointer to memory AFTER EntryPoint in A0
						; so, Message can easily be extended to pass arguments.
						; Result from routine will be placed in EntryPoint
	LABEL	smar_SIZEOF

    STRUCTURE SM_RunProcess,scm_SIZEOF
	APTR	smrp_EntryPoint			; address of start of code to execute
	ULONG	smrp_Flags			; ???
	LABEL	smrp_SIZEOF

    STRUCTURE SM_AppSleep,scm_SIZEOF
	LABEL	smap_SIZEOF

    STRUCTURE SM_AppWakeup,scm_SIZEOF
	ULONG	sm_aw_ReLayout			;BOOL
	LABEL	smaw_SIZEOF

    STRUCTURE SM_Redraw,scm_SIZEOF
	ULONG	smmr_Flags			;see below
	LABEL	smmr_SIZEOF

    STRUCTURE SM_Update,scm_SIZEOF
	LABEL	smmu_SIZEOF

    STRUCTURE SM_UpdateIcon,scm_SIZEOF
    	BPTR	smui_DirLock
    	APTR	smui_IconName
	LABEL	smui_SIZEOF

    STRUCTURE SM_AddIcon,scm_SIZEOF
	UWORD	smai_x
	UWORD	smai_y
	BPTR	smai_DirLock
	APTR	smai_IconName
	LABEL	smai_SIZEOF

    STRUCTURE SM_RemIcon,scm_SIZEOF
	BPTR	smri_DirLock
	APTR	smri_IconName
	LABEL	smri_SIZEOF

    STRUCTURE SM_Iconify,scm_SIZEOF
	LABEL	smic_SIZEOF

    STRUCTURE SM_UnIconify,scm_SIZEOF
	LABEL	smun_SIZEOF

    STRUCTURE SM_RunMenuCmd,scm_SIZEOF
	APTR	smrm_MenuItem
	APTR	smrm_IconNode
	ULONG	smrm_Flags
	LABEL	smrm_SIZEOF

    STRUCTURE SM_RealUpdateIcon,0
	STRUCT	rui_AsyncRoutine,smar_SIZEOF	; struct SM_AsyncRoutine
	STRUCT	rui_Args,wa_SIZEOF		; struct WBArg
	ULONG 	rui_IconType
	LABEL	rui_SIZEOF

    STRUCTURE SM_Requester,scm_SIZEOF
	ULONG 	smrq_ReqResult		; \Result from easyRequest()
	LABEL	smrq_ParentWindow	; /Parent window pointer
	APTR	smrq_ArgList		; pointer to ArgList
	STRUCT	smrq_ez,EasyStruct_SIZEOF	; struct EasyStruct
	ULONG	smrq_ArgListBuffer	; buffer for ArgList - allocated as large as needed
	LABEL	smrq_SIZEOF

    STRUCTURE SM_AsyncBackFill,scm_SIZEOF
	APTR	smab_BackfillFunc	; Backfill function entry point
	APTR	smab_PatternNode	; struct PatternNode *
	APTR	smab_PatternInfo	; struct PatternInfo *
	APTR	smab_Screen		; struct Screen *
	LABEL	smab_PatternInfoCopy
	LABEL	smab_SIZEOF

    STRUCTURE SM_ShowTitle,scm_SIZEOF
	ULONG	smst_showTitle		;    ; BOOL
	LABEL	smst_SIZEOF

    STRUCTURE SM_ARexxRoutine,scm_SIZEOF
	APTR	smrx_EntryPoint
	APTR	smrx_RexxMsg		; struct RexxMsg *
	LABEL	smrx_SIZEOF

    STRUCTURE SM_ShowPopupMenu,scm_SIZEOF
	APTR	smpm_PopupMenu;		; struct PopupMenu *
	APTR	smpm_IconNode;		; struct ScaIconNode *
	ULONG	smpm_Flags;
	LABEL	smpm_SIZEOF

; values in smpm_Flags - same as in mpm_Flags

    STRUCTURE SM_ShowStatusBar,scm_SIZEOF
	ULONG	smsb_Visible;
	LABEL	smsb_SIZEOF

   STRUCTURE SM_ShowControlBar,scm_SIZEOF
	ULONG	smcb_Visible;
	LABEL	smcb_SIZEOF

    STRUCTURE SM_RedrawIcon,scm_SIZEOF
	APTR	smrdi_Icon;	       ; struct ScaIconNode *
	LABEL	smrdi_SIZEOF

    STRUCTURE SM_DoPopupMenu,scm_SIZEOF
	STRUCT	smdpm_InputEvent,ie_SIZEOF	; struct InputEvent
	LABEL	smdpm_SIZEOF

    STRUCTURE SM_RedrawIconObj,scm_SIZEOF
	APTR	smrio_IconObject;	; Object *
	ULONG	smrio_Flags;
	LABEL	smrio_SIZEOF

; values in smrio_Flags
	BITDEF	SMRIOFLAG,EraseIcon,0
	BITDEF	SMRIOFLAG,FreeLayout,1
	BITDEF	SMRIOFLAG,Highlight,2
	BITDEF	SMRIOFLAG,IconListLocked,3
	BITDEF	SMRIOFLAG,HighlightOn,4
	BITDEF	SMRIOFLAG,HighlightOff,5

    STRUCTURE SM_NewPreferences,scm_SIZEOF
	ULONG	smnprf_PrefsFlags
	LABEL	smnprf_SIZEOF

; values in smnp_PrefsFlags
; one bit is set for each changed subsystem prefs
	BITDEF	SMNPFLAG,PATTERNPREFS,0
	BITDEF	SMNPFLAG,MENUPREFS,1
	BITDEF	SMNPFLAG,FONTPREFS,2
	BITDEF	SMNPFLAG,SCALOSPREFS,3
	BITDEF	SMNPFLAG,PALETTEPREFS,4
	BITDEF	SMNPFLAG,DEFICONSPREFS,5
	BITDEF	SMNPFLAG,LOCALEPREFS,6
	BITDEF	SMNPFLAG,FILETYPESPREFS,7

    STRUCTURE SM_DeltaMove,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	LONG 	smdm_DeltaX
	LONG 	smdm_DeltaY
	ULONG 	smdm_AdjustSlider
	LABEL	smdm_SIZEOF

    STRUCTURE SM_SetThumbnailImage_Remapped,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	APTR	mtir_IconObject			; Object * - the icon to attach the new image to
	APTR	smtir_NewImage			; struct ScalosBitMapAndColor * - the new image. ScalosBitMapAndColor contents is freed by message handler
	LABEL	smtir_SIZEOF

    STRUCTURE SM_SetThumbnailImage_ARGB,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	APTR	smtia_IconObject		; Object * -- the icon to attach the new image to
	STRUCT	smtia_NewImage,ARGBHeader_SIZEOF;	; the new image. ARGBHeader contents is freed by message handler
	LABEL	smtia_SIZEOF

    STRUCTURE SM_NewWindowPath,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	APTR 	smnwp_Path;			; STRPTR -- Path of the new drawer to display in the window
	LABEL	smnwp_SIZEOF

   STRUCTURE SM_PrefsChanged,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	ULONG	smpc_Flags;			; ULONG
	LABEL	smpc_SIZEOF

   STRUCTURE SM_StartChildProcess,scm_SIZEOF
	;struct ScalosMessage    ScalosMessage;
	APTR	smscp_WindowTask
	LABEL	smscp_SIZEOF

;---------------------------------------------------------------------------

    STRUCTURE WindowList,0
	APTR	wl_WindowStruct			;Pointer to the first windowstruct
	APTR	ml_MainWindowStruct		;Pointer into the list of windowstructs
	APTR	ml_AppWindowStruct		;dito - windowstruct for AppIcons
	LABEL	wl_SIZE

;-----------------------------------------------------------------------

    STRUCTURE WindowStruct,MLN_SIZE
	APTR	ws_Task			;Window-Task address
	BPTR	ws_Lock			;filelock to the windows directory
	APTR	ws_Window		;pointer to the window or NULL
	APTR	ws_MessagePort		;the API-messageport
	WORD	ws_Left
	WORD	ws_Top
	WORD	ws_Width
	WORD	ws_Height		;size and position of the window
	WORD	ws_xoffset
	WORD	ws_yoffset		;the virtual position inside
	APTR	ws_PatternNode
	APTR	ws_WindowTask		;windowtask structure
	APTR	ws_Name			;windows name (is displayed in the title)
	APTR	ws_Title		;title-formatstring
	APTR	ms_ClassName		;Scalos class name
	APTR	ms_Class		;BOOPSI Class if ms_classname is NULL
	UWORD	ws_Flags		;see below
	UWORD	ws_PatternNumber	;number of the pattern
	UBYTE	ws_Viewmodes		;see below
	UBYTE	ws_WindowType		;see below
	APTR	ws_WindowTaskName	;name of window task +jl+ 20011211
	UWORD	ws_ViewAll		; DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
	ULONG	ws_ThumbnailView	; THUMBNAILS_Never, THUMBNAILS_Always, THUMBNAILS_AsDefault
	ULONG	ws_ThumbnailsLifetimeDays	; Maximum lifetime for generated thumbnails
	ULONG 	ws_SortOrder		; current sorting or ascending or descending
	UWORD	ws_WindowOpacityActive	; percentage of active Scalos Window transparency - not used on all platforms!
					; 0=PREFS_TRANSPARENCY_TRANSPARENT=invisible, 100=PREFS_TRANSPARENCY_OPAQUE=opaque)
	UWORD	ws_WindowOpacityInactive	; percentage of inactive Scalos Window transparency - not used on all platforms!
	ULONG	ws_MoreFlags		; various flags, see below
	LABEL	ws_SIZEOF

;ws_Flags:
	BITDEF	WSV,RootWindow,0	;a RootDir window
	BITDEF	WSV,NeedsTimerMsg,1	;window likes to get TimerMessages
	BITDEF	WSV,Backdrop,2		;is the window in backdrop mode
	BITDEF	WSV,TimerMsgSent,3	;timermsg already sent *PRIVATE*
	BITDEF	WSV,TaskSleeps,4	;windowtask is sleeping
	BITDEF	WSV,ShowAllFiles,5	;just what it said
	BITDEF	WSV,Iconify,6		;windowtask is in iconify state
	BITDEF	WSV,CheckUpdatePending,7 ;+jl+ 20010403 *PRIVATE*
	BITDEF	WSV,Typing,9		;+jl+ 20011005 *PRIVATE*
	BITDEF	WSV,NoStatusBar,10	; don't display status bar for this window
	BITDEF	WSV,NoActivateWindow,11	; don't activate window on OpenWindow()
	BITDEF	WSV,RefreshPending,12	; IDCMP_REFRESHWINDOW could not be processed, refresh is pending *PRIVATE*
	BITDEF	WSV,BrowserMode,13      ; enable single-window browser mode
	BITDEF	WSV,CheckOverlappingIcons,14	; prevent icons from overlapping each other
	BITDEF	WSV,DdPopupWindow,15	; this window has popped up during a D&D operation

; ws_MoreFlags
	BITDEF	WSV,NoControlBar,0	; don't display control bar for this window

;ws_WindowType:
WSV_Type_IconWindow		EQU	0	;Window filled with icons
WSV_Type_DeviceWindow		EQU	1

;-----------------------------------------------------------------------

	ifnd	MAIN_SCALOS

    STRUCTURE PatternInfo,0
	STRUCT	ptinf_hook,h_SIZEOF		;Backfill Hook
	WORD	ptinf_width;			;Width
	WORD	ptinf_height;			;and Height of Bitmap in Pixels
	APTR	ptinf_bitmap;			;struct Bitmap *, or NULL
	APTR	ptinf_pattern;			;Pattern Array or NULL
	UWORD	ptinf_flags;			;see below
	WORD	ptinf_pad;			;padword
	LONG	ptinf_BgPen;			;Allocated pen for background pattern
	APTR	ptinf_message;			;struct Message *
	LABEL	ptinf_SIZEOF

    STRUCTURE WindowTask,0
	STRUCT	wt_PatternInfo,ptinf_SIZEOF
	WORD	wt_XOffset
	WORD	wt_YOffset		;Innerwindow offset
	APTR	mt_MainObject		;main ScalosClass object
	APTR	mt_WindowObject		;"Window.sca" object
	APTR	wt_Window		;pointer to the window of the task or NULL
	APTR	wt_IconList		;Pointer to the first IconNode
	APTR	wt_LateIconList		;Pointer to the first IconNode of all not yet
					;displayed icons (e.g. non-position icon while
					;loading)
	APTR	wt_IconSemaphore	;Semaphore to access the wt_IconList/wt_LateIconList
					;It *MUST* be locked before accessing the list.
					;Shared lock should be used for read-only access
	APTR	wt_IconPort		;MessagePort of the windowtask
	APTR	wt_WindowSemaphore	;Semaphore to avoid the task from closing
	APTR	mt_WindowStruct		;Pointer to the WindowStruct of this task
	LABEL	wt_SIZEOF		;It's only the documented end of this structure.
					;In reality it's much bigger.

	ENDC
;-----------------------------------------------------------------------

    STRUCTURE ScaIconNode,MLN_SIZE
	APTR	in_Icon			;IconObject from iconobject.library
	APTR	in_Name			;Filename of the icon
	APTR	in_DeviceIcon		;NULL except for disk icons
	APTR	in_Lock			;lock to the files dir or NULL
	APTR	in_IconList		;list of linked icons (used for backdrop icons)
	UWORD	in_Flags		;see below
	UWORD	in_Userdata
	STRUCT	in_DateStamp,ds_SIZEOF	;file's datestamp
	ULONG	in_SupportFlags		;Flags to determine which kind of menu 
					;operation this specific icon supports. see below
	APTR	in_FileType		;this icon's file type
	LABEL	in_SIZEOF

;values (bits) in in_Flags:
	BITDEF	IN,DefaultIcon,0		;the icon has diskobject
	BITDEF	IN,File,1			;File or Drawer (TextMode only)
	BITDEF	IN,TextIcon,2			;TextMode icon
	BITDEF	IN,Sticky,3			;icon not moveable
	BITDEF	IN,DropMarkVisible,4		;Icon drop mark visible
	BITDEF	IN,VolumeWriteProtected,5	;volume is write-protected
	BITDEF	IN,FreeIconPosition,6		;Icon is stored with "no position
	BITDEF	IN,IconVisible,7		;Icon has been drawn
	BITDEF	IN,Identified,8			;Icon file type has been determined

;values (bits) in in_SupportFlags :
	BITDEF	IN,SupportsOpen,0
	BITDEF	IN,SupportsCopy,1
	BITDEF	IN,SupportsRename,2
	BITDEF	IN,SupportsInformation,3
	BITDEF	IN,SupportsSnapshot,4
	BITDEF	IN,SupportsUnSnapshot,5
	BITDEF	IN,SupportsLeaveOut,6
	BITDEF	IN,SupportsPutAway,7
	BITDEF	IN,SupportsDelete,8
	BITDEF	IN,SupportsFormatDisk,9
	BITDEF	IN,SupportsEmptyTrash,10

;-----------------------------------------------------------------------

     STRUCTURE ScaDeviceIcon,MLN_SIZE
	APTR	di_Handler		;MessagePort of the HandlerTask
	APTR	di_Volume		;Volumename or NULL
	APTR	di_Device		;Devicename or NULL
	APTR	di_Icon			;IconObject of this DeviceIcon struct
	APTR	di_Info			;Infodata from dos.library/Info()
	STRUCT	di_InfoBuf,id_SIZEOF+4
	UWORD	di_Flags		;see below
	LABEL	devi_SIZEOF

;di_Flags
	BITDEF	DIB,DontDisplay,0      		;Don't display this icon
	BITDEF	DIB,Remove,1    		;***internal use only***
	BITDEF	DIB,InfoPending,2   		;***internal use only***
	BITDEF	DIB,BackdropReadComplete,3	;***internal use only***

;-----------------------------------------------------------------------

;UpdateIcon structure for Icon and Device windowtypes

    STRUCTURE UpdateIcon_IW,0
	BPTR	ui_iw_Lock		;Lock to the file's/dir's directory
	APTR	ui_iw_Name		;Name of the file or dir, NULL for Disks
	LABEL	ui_iw_SIZE

;-----------------------------------------------------------------------

    STRUCTURE ScalosNodeList,0
	APTR	MinNode			;Pointer to the first node or NULL
	LABEL	snl_SIZE

;------------------------------- Scalos Class Information --------------------------

SCC_Dummy		EQU	$80580000


;-------------------- Root Class -------------------------------
;Name: "Root.sca"

;---------------------- Attributes ------------------------------

SCCA_WindowTask			EQU	SCC_Dummy+1	;(ISG)

;----------------------- Methodes -------------------------------

SCCM_HandleMessages		EQU	SCC_Dummy+105
SCCM_ScalosMessage		EQU	SCC_Dummy+106		;!
SCCM_Message			EQU	SCC_Dummy+107
SCCM_ScalosMessageReply		EQU	SCC_Dummy+108		;!
SCCM_RunProcess			EQU	SCC_Dummy+120
;APTR	Routine
;APTR	Args
;ULONG	size_of(Args)
;APTR	ReplyPort		;see values below

SCCV_RunProcess_NoReply		EQU	0
SCCV_RunProcess_WaitReply	EQU	-1
SCCM_Ping               	equ	SCC_Dummy+140	; Timer Ping
SCCM_AddToClipboard		equ	SCC_Dummy+172
SCCM_ClearClipboard		equ	SCC_Dummy+173
SCCM_GetLocalizedString		equ	SCC_Dummy+190
SCCM_WindowStartComplete 	equ	SCC_Dummy+215

;-------------------- Title Class -------------------------------
;Name: "Title.sca"

;-------------- Attributes ------------

SCCA_Title_Format		EQU	SCC_Dummy+2	;(IS.)
SCCA_Title_Type			EQU	SCC_Dummy+3	;(I..)

SCCV_Title_Type_Screen		EQU	0	;(default)
SCCV_Title_Type_Window		EQU	1

;--------------- Methodes -------------

SCCM_Title_Generate		EQU	SCC_Dummy+101
;Result: MemPtr

SCCM_Title_Query		EQU	SCC_Dummy+102
;UWORD	ParseID
;Result: BOOL (Needs Refresh)

SCCM_Title_QueryTitle		EQU	SCC_Dummy+103
;Result: BOOL (Needs Refresh)

SCCM_Title_Translate		EQU	SCC_Dummy+104
;CHAR	*TitleMem
;CHAR	*TitleFormat
;UWORD	ParseID
;Result: succ BOOL


;----------------- Window Class -------------------------------
;Name: "Window.sca"

;--------------- Methods -------------

SCCM_Window_Open		EQU	SCC_Dummy+123
SCCM_Window_Close		EQU	SCC_Dummy+124
SCCM_Window_Iconify		EQU	SCC_Dummy+125
SCCM_Window_UnIconify		EQU	SCC_Dummy+126
SCCM_Window_InitClipRegion	EQU	SCC_Dummy+127
;Result: OldClipRegion

SCCM_Window_RemClipRegion	EQU	SCC_Dummy+128
;APTR	OldClipRegion

SCCM_Window_SetTitle		EQU	SCC_Dummy+129
SCCM_Window_SetInnerSize        equ	SCC_Dummy+143

SCCM_Window_LockUpdate		equ	SCC_Dummy+154	; +jl+ 20010420
; ./.
SCCM_Window_UnlockUpdate	equ	SCC_Dummy+155	; +jl+ 20010420
; ./.

SCCM_Window_ChangeWindow	equ	SCC_Dummy+189	; +jl+ 20040309
; ./.

SCCM_Window_DynamicResize	equ	SCC_Dummy+195
; ./.

SCCM_Window_NewPath		equ	SCC_Dummy+208
; CONST_STRPTR
; ULONG Tag, Tag, ... TAG_END
; (same parameters as SCCM_IconWin_NewPath)

SCCM_Window_SetTransparency	equ	SCC_Dummy+209
; ULONG Transparency, 0=transparent, 100=opaque

SCCM_Window_WBStartupFinished 	equ	SCC_Dummy+210

;-------------- Attributes ------------

SCCA_Window_IDCMP		EQU	SCC_Dummy+2	; unused
SCCA_Window_Transparency	EQU	(SCC_Dummy+3)	; (ULONG) [.SG] percentage of window opacity, 0=transparent, 100=opaque

;---------------- IconWindow Class ----------------------------
;Name: "IconWindow.sca"

; -------------- IconWindow Attributes ------------

SCCA_IconWin_Reading		EQU	SCC_Dummy+1000	; (BOOL)  [SG] currently  reading directory
SCCA_IconWin_SelectedCount	EQU	SCC_Dummy+1001	; (ULONG) [SG] number of selected icons in window
SCCA_IconWin_Typing		EQU	SCC_Dummy+1002	; (BOOL)  [SG] user is currently tying icon name
SCCA_IconWin_ShowType		EQU	SCC_Dummy+1003	; (ULONG) [SG] DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
SCCA_IconWin_InnerWidth		EQU	SCC_Dummy+1004	; (ULONG) [.G] icon window inner width
SCCA_IconWin_InnerHeight	EQU	SCC_Dummy+1005	; (ULONG) [.G] icon window inner height
SCCA_IconWin_IconFont		EQU	SCC_Dummy+1006	; (struct TextFont *) [.G] icon window icon font
SCCA_IconWin_LayersLocked	EQU	SCC_Dummy+1007	; (ULONG) [.G] Flag: icon window has Layers locked
SCCA_IconWin_StatusBar		EQU	SCC_Dummy+1008	; (BOOL)  [SG] Flag: Status bar is present
SCCA_IconWin_ThumbnailView	EQU	SCC_Dummy+1009	; (ULONG) [SG] Thumbnail display mode
SCCA_IconWin_InnerLeft		EQU	SCC_Dummy+1011	; (ULONG) [.G] left edge of window icon window icon area
SCCA_IconWin_InnerTop		EQU	SCC_Dummy+1012	; (ULONG) [.G] top edge of window icon window icon area
SCCA_IconWin_InnerRight		EQU	SCC_Dummy+1013	; (ULONG) [.G] right edge of window icon window icon area
SCCA_IconWin_InnerBottom	EQU	SCC_Dummy+1014	; (ULONG) [.G] bottom
SCCA_IconWin_ActiveTransparency	EQU	SCC_Dummy+1015	; (ULONG) [SG] degree of transparency for active window state (0=transparent, 100=opaque)
SCCA_IconWin_InactiveTransparency EQU	SCC_Dummy+1016	; (ULONG) [SG] degree of transparency for inactive window state (0=transparent, 100=opaque)
SCCA_IconWin_ControlBar		EQU	SCC_Dummy+1017	; (BOOL)  [SG] Flag: Control bar is present

;--------------- Methods -------------
SCCM_IconWin_ReadIconList	EQU	SCC_Dummy+100
;ULONG	flags
;Result: BOOL CloseWindow
;Flags:
SCCV_IconWin_ReadIconList_ShowAll	EQU	1

SCCM_IconWin_ReadIcon		EQU	SCC_Dummy+109
SCCM_IconWin_DragBegin		EQU	SCC_Dummy+110
SCCM_IconWin_DragQuery		EQU	SCC_Dummy+111
SCCM_IconWin_DragFinish		EQU	SCC_Dummy+112
SCCM_IconWin_DragEnter		equ	SCC_Dummy+149
SCCM_IconWin_DragLeave		equ	SCC_Dummy+150
SCCM_IconWin_DragDrop		EQU	SCC_Dummy+113
SCCM_IconWin_Open		EQU	SCC_Dummy+114
;APTR	iconnode	(IconNode to open or NULL)
;ULONG	      Flags
	BITDEF	ICONWINOPEN,IgnoreFileTypes,0
	BITDEF	ICONWINOPEN,NewWindow,1
	BITDEF	ICONWINOPEN,DoNotActivateWindow,2
;Result: BOOL

SCCM_IconWin_UpdateIcon		EQU	SCC_Dummy+115

SCCM_IconWin_UpdateIconTags	EQU	SCC_Dummy+225
; BPTR Lock
; APTR Name
; ULONG Tag, Tag, ... TAG_END


SCCM_IconWin_AddIcon		EQU	SCC_Dummy+116
SCCM_IconWin_RemIcon		EQU	SCC_Dummy+117
SCCM_IconWin_MakeWBArg		EQU	SCC_Dummy+118
SCCM_IconWin_CountWBArg		EQU	SCC_Dummy+197
SCCM_IconWin_UnCleanUp		EQU	SCC_Dummy+193
SCCM_IconWin_CleanUp		EQU	SCC_Dummy+119
SCCM_IconWin_SetVirtSize	EQU	SCC_Dummy+121
;UWORD	Flags
;Flags:
; bit 0 = set Bottom-Slider
; bit 1 = set Right-Slider
	BITDEF	SETVIRT,AdjustBottomSlider,0
	BITDEF	SETVIRT,AdjustRightSlider,1

SCCM_IconWin_Redraw		EQU	SCC_Dummy+122
SCCM_IconWin_Update		EQU	SCC_Dummy+130
SCCM_IconWin_CheckUpdate	equ	SCC_Dummy+151	; +jl+
SCCM_IconWin_Sleep		EQU	SCC_Dummy+131
SCCM_IconWin_WakeUp		EQU	SCC_Dummy+132
SCCM_IconWin_MenuCommand	EQU	SCC_Dummy+138
;APTR	CommandName
;APTR	IconNode	;(or NULL)
;BOOL	State		;(checkmark state if it's a checkmark)

SCCM_IconWin_NewViewMode	EQU	SCC_Dummy+139
;ULONG	ViewMode
;Result: IconWindow.sca subclass object

SCCM_IconWin_DrawIcon           equ	SCC_Dummy+141
; APTR	Icon

SCCM_IconWin_LayoutIcon         equ	SCC_Dummy+142
; APTR	Icon
; ULONG	LayoutFlags

SCCM_IconWin_DeltaMove          equ	SCC_Dummy+144
; LONG	deltaX
; LONG	deltaY

SCCM_IconWin_GetDefIcon         equ	SCC_Dummy+146
; char*	filename
; LONG	filetype
; ULONG	protectionbits

SCCM_IconWin_ShowIconToolTip	equ	SCC_Dummy+156	; +jl+ 20010519
; struct ScaIconNode *
; ---------- Returns : ./.

SCCM_IconWin_RawKey		equ	SCC_Dummy+158	; +jl+ 20010716
; struct IntuiMessage *

SCCM_IconWin_AddToStatusBar	equ	SCC_Dummy+159	; +jl+ 20011204
; struct Gadget *
; ULONG Tag, Tag, ... TAG_END

SCCM_IconWin_RemFromStatusBar	equ	SCC_Dummy+160	; +jl+ 20011204
; struct Gadget *

SCCM_IconWin_UpdateStatusBar	equ	SCC_Dummy+161	; +jl+ 20011204
; struct Gadget *
; ULONG Tag, Tag, ... TAG_END

SCCM_IconWin_ShowPopupMenu	equ	SCC_Dummy+166	; +jl+ 20011209
; struct PopupMenu *mpm_PopupMenu;
; struct ScaIconNode *mpm_IconNode;		(may be NULL for window popup menus)
; ULONG mpm_Flags;

; Values in mpm_Flags
	BITDEF	SHOWPOPUPFLG,IconSemaLocked,0
	BITDEF	SHOWPOPUPFLG,WinListLocked,1
	BITDEF	SHOWPOPUPFLG,FreePopupMenu,2

SCCM_IconWin_ShowGadgetToolTip	equ	SCC_Dummy+171	; +jl+ 20021001
; ULONG sgtt_GadgetID

; Values for sgtt_GadgetID
SGTT_GADGETID_RightScroller		equ	1001
SGTT_GADGETID_BottomScroller		equ	1002
SGTT_GADGETID_UpArrow			equ	1003
SGTT_GADGETID_DownArrow			equ	1004
SGTT_GADGETID_RightArrow		equ	1005
SGTT_GADGETID_LeftArrow			equ	1006
SGTT_GADGETID_Iconify			equ	1007
SGTT_GADGETID_StatusBar_Text		equ	1008
SGTT_GADGETID_StatusBar_ReadOnly	equ	1009
SGTT_GADGETID_StatusBar_Reading		equ	1010
SGTT_GADGETID_StatusBar_Typing		equ	1011
SGTT_GADGETID_StatusBar_ShowAll		equ	1012
SGTT_GADGETID_unknown			equ	1013


SCCM_IconWin_ActivateIconLeft	equ	SCC_Dummy+174	; +jl+ 20021201
; ./.

SCCM_IconWin_ActivateIconRight	equ	SCC_Dummy+175	; +jl+ 20021201
; ./.

SCCM_IconWin_ActivateIconUp	equ	SCC_Dummy+176	; +jl+ 20021201
; ./.

SCCM_IconWin_ActivateIconDown	equ	SCC_Dummy+177	; +jl+ 20021201
; ./.

SCCM_IconWin_ActivateIconNext	equ	SCC_Dummy+178	; +jl+ 20021201
; ./.

SCCM_IconWin_ActivateIconPrevious	equ	SCC_Dummy+179	; +jl+ 20021201
; ./.

SCCM_IconWin_AddGadget		equ	SCC_Dummy+184	; +jl+ 20030112
; struct Gadget *

SCCM_IconWin_ImmediateCheckUpdate	equ	SCC_Dummy+187	; +jl+
; ./.

SCCM_IconWin_GetIconFileType	equ	SCC_Dummy+168	; +jl+ 20020615
; struct ScaIconNode *gift_IconNode;

SCCM_IconWin_ClearIconFileTypes	equ	SCC_Dummy+169	; +jl+ 20020615
; ./.

SCCM_IconWin_NewPatternNumber	equ	SCC_Dummy+192	; +jl+ 20040912
; struct msg_NewPatternNumber *

SCCM_IconWin_AddToControlBar	equ	SCC_Dummy+199
; struct Gadget *
; ULONG Tag, Tag, ... TAG_END

SCCM_IconWin_RemFromControlBar	equ	SCC_Dummy+200
; struct Gadget *

SCCM_IconWin_UpdateControlBar	equ	SCC_Dummy+201
; struct Gadget *
; ULONG Tag, Tag, ... TAG_END

SCCM_IconWin_NewPath		equ	SCC_Dummy+202
; CONST_STRPTR

SCCM_IconWin_History_Back	equ	SCC_Dummy+204
; ./.

SCCM_IconWin_History_Forward	equ	SCC_Dummy+205
; ./.

SCCM_IconWin_StartNotify	equ	SCC_Dummy+206
; ./.

SCCM_IconWin_Browse		equ	SCC_Dummy+207
; ./.

SCCM_IconWin_WBStartupFinished	equ	SCC_Dummy+208
; ./.

SCCM_IconWin_StartPopOpenTimer	equ	SCC_Dummy+213
; struct ScaIconNode *spot_IconNode;

SCCM_IconWin_StopPopOpenTimer	equ	SCC_Dummy+214
; ./.

SCCM_IconWin_AddUndoEvent   	equ	SCC_Dummy+218
; enum ScalosUndoType aue_Type;
; ULONG Tag, Tag, ... TAG_END

SCCM_IconWin_BeginUndoStep	equ	SCC_Dummy+219
; ./.

SCCM_IconWin_EndUndoStep	equ	SCC_Dummy+220
; APTR UndoStep

SCCM_IconWin_RandomizePatternNumber equ	SCC_Dummy+222
; ./.

SCCM_IconWin_UnCleanUpRegion	equ	SCC_Dummy+224
; struct Region *UnCleanupRegion;


;---------------- DeviceWindow Class ----------------------------
;Name: "DeviceWindow.sca"

;--------------- Methodes -------------
SCCM_DeviceWin_ReadIcon		EQU	SCC_Dummy+136
;APTR DeviceNode

;---------------- TextWindow Class ----------------------------
;Name: "TextWindow.sca"

;--------------- Methodes -------------
SCCM_TextWin_ReadIcon		EQU	SCC_Dummy+137
SCCM_TextWin_ReAdjust           equ	SCC_Dummy+145

; +jl+ 20010320
; hold all visible updates until SCCM_TextWin_EndUpdate
SCCM_TextWin_BeginUpdate	equ	SCC_Dummy+152

; unlock window updates and do SCCM_TextWin_ReAdjust
SCCM_TextWin_EndUpdate		equ	SCC_Dummy+153

SCCM_TextWin_InsertIcon		equ	SCC_Dummy+186
; struct ScaIconNode *
; ---------- Returns : ./.


SCCM_TextWin_DrawColumnHeaders	equ	SCC_Dummy+194
; ./.
; ---------- Returns : ./.

;--------------- DeviceList Class ----------------------------
;Name: "DeviceList.sca"

;--------------- Methodes -------------
SCCM_DeviceList_Generate	EQU	SCC_Dummy+133
;APTR ScalosNodeList

SCCM_DeviceList_FreeDevNode	EQU	SCC_Dummy+134
;APTR Node

SCCM_DeviceList_Filter		EQU	SCC_Dummy+135
;APTR ScalosNodeList


; ------------- FileTransfer Class ---------------------------
; Name: TBC

FTOPCODE_Copy			EQU	1
FTOPCODE_Move			EQU	2
FTOPCODE_CreateLink		EQU	3
FTOPCODE_Delete			EQU	4

; -------------- Attributes -----------

SCCA_FileTrans_Number           equ	SCC_Dummy+5
SCCA_FileTrans_Screen           equ	SCC_Dummy+6
SCCA_FileTrans_ReplaceMode	EQU	SCC_Dummy+157	; [ISG] +jl+ 20010713
SCCA_FileTrans_WriteProtectedMode equ	SCC_Dummy+195	; [I..]

; Values for SCCA_FileTrans_ReplaceMode
SCCV_ReplaceMode_Ask		EQU	0
SCCV_ReplaceMode_Never		EQU	1
SCCV_ReplaceMode_Always		EQU	2

; Values for SCCA_FileTrans_WriteProtectedMode
SCCV_WriteProtectedMode_Ask	equ	0
SCCV_WriteProtectedMode_Never	equ	1
SCCV_WriteProtectedMode_Always	equ	2
SCCV_WriteProtectedMode_Abort	equ	3

; --------------- Methods -------------
SCCM_FileTrans_Copy             equ	SCC_Dummy+147
; BPTR	SourceLock
; BPTR	DestLock
; BPTR	Name

SCCM_FileTrans_Move             equ	SCC_Dummy+148
; BPTR	SourceLock
; BPTR	DestLock
; BPTR	Name


SCCM_FileTrans_CheckAbort	equ	SCC_Dummy+162
; input -
; return RETURN_OK to continue 
;	or non-zero to abort moving/copying

SCCM_FileTrans_OpenWindow	equ	SCC_Dummy+163
; input -
; returns struct Window *  - NULL if window could not be opened


SCCM_FileTrans_OpenWindowDelayed equ	SCC_Dummy+164
; input -
; returns struct Window *  - NULL if window could not be opened


SCCM_FileTrans_UpdateWindow	equ	SCC_Dummy+165
; ULONG UpdateMode
; BPTR SrcDirLock
; BPTR DestDirLock
; CONST_STRPTR Name
; returns -


SCCM_FileTrans_CountTimeout	equ	SCC_Dummy+167
; input -
; returns non-zero value if CountTimeout is exceeded and counting should be aborted.


SCCM_FileTrans_OverwriteRequest	equ	SCC_Dummy+185
; enum OverWriteReqType requestType
; BPTR srcLock
; CONST_STRPTR srcName
; BPTR destLock
; CONST_STRPTR destName
; struct Window *parentWindow
; ULONG suggestedBodyTextId
; ULONG suggestedGadgetTextId
; returns ExistsReqResult

OVERWRITEREQ_Copy		equ	0
OVERWRITEREQ_Move		equ	1
OVERWRITEREQ_CopyIcon		equ	2

EXISTREQ_Replace		equ	1
EXISTREQ_Skip			equ	2
EXISTREQ_ReplaceAll		equ	3
EXISTREQ_SkipAll		equ	4
EXISTREQ_Abort			equ	0


SCCM_FileTrans_CreateLink	equ	SCC_Dummy+191
; BPTR SourceLock
; BPTR DestLock
; CONST_STRPTR	SrcName
; CONST_STRPTR	DestName
; LONG MouseX
; LONG MouseY

SCCM_FileTrans_WriteProtectedRequest	equ    SCC_Dummy+196
; enum WriteProtectedReqType requestType
; BPTR destLock
; CONST_STRPTR destName
; struct Window *parentWindow
; LONG ErrorCode
; ULONG suggestedBodyTextId
; ULONG suggestedGadgetTextId
; returns WriteProtectedReqResult

WRITEPROTREQ_Copy		equ	0
WRITEPROTREQ_Move		equ	1
WRITEPROTREQ_CopyIcon		equ	2

WRITEPROTREQ_Replace		equ	1
WRITEPROTREQ_Skip		equ	2
WRITEPROTREQ_ReplaceAll		equ	3
WRITEPROTREQ_SkipAll		equ	4
WRITEPROTREQ_Abort		equ	0

SCCM_FileTrans_ErrorRequest		equ	SCC_Dummy+210
; ULONG mer_SuggestedBodyTextId;
; ULONG mer_SuggestedGadgetTextId;
; returns ErrorReqResult

ERRORREQ_Retry 			equ	1
ERRORREQ_Skip			equ	2
ERRORREQ_Abort			equ	3


ftta_Copy			equ	0
ftta_Move			equ	1
ftta_Delete			equ	2
ftta_DeleteFile			equ	3
ftta_DeleteDir			equ	4
ftta_CopyAndDelete		equ	5
ftta_CopyFile			equ	6
ftta_CopyDir			equ	7
ftta_CopyVolume			equ	8
ftta_CopyLink			equ	9
ftta_CreateSoftLink		equ	10
ftta_CreateHardLink		equ	11
ftta_Count			equ	12
ftta_CountDir			equ	13

fto_Lock			equ	0
fto_AddPart			equ	1
fto_MakeLink                    equ	2
fto_AllocPathBuffer             equ	3
fto_NameFromLock                equ	4
fto_Rename                      equ	5
fto_AllocDosObject              equ	6
fto_Examine                     equ	7
fto_Open                        equ	8
fto_Read                        equ	9
fto_Write                       equ	10
fto_GetDeviceProc               equ	11
fto_ReadLink                    equ	12
fto_DeleteFile                  equ	13
fto_CreateDir                   equ	14
fto_ExNext                      equ	15
fto_DupLock                     equ	16
fto_SetProtection              	equ	17

; ---------------------------------------------------------------------------

SCCM_FileTrans_LinksNotSupportedRequest	equ	SCC_Dummy+211
;        BPTR  mlns_SrcDirLock;
;        CONST_STRPTR mlns_SrcName;
;        BPTR  mlns_DestDirLock;
;        CONST_STRPTR mlns_DestName;
;        ULONG mlns_SuggestedBodyTextId;
;        ULONG mlns_SuggestedGadgetTextId;

LINKSNOTSUPPORTEDREQ_Ignore	equ	1
LINKSNOTSUPPORTEDREQ_IgnoreAll	equ	2
LINKSNOTSUPPORTEDREQ_Copy	equ	3
LINKSNOTSUPPORTEDREQ_CopyAll	equ	4
LINKSNOTSUPPORTEDREQ_Abort	equ	5

; ---------------------------------------------------------------------------

SCCM_FileTrans_InsufficientSpaceRequest	equ	SCC_Dummy+223
;        struct Window *mlns_ParentWindow;
;        BPTR  miss_SrcDirLock;
;        CONST_STRPTR miss_SrcName;
;        BPTR  miss_DestDirLock;
;        CONST_STRPTR miss_DestName;
;        const ULONG64 *miss_RequiredSpace;
;        const ULONG64 *miss_AvailableSpace;
;        ULONG miss_SuggestedBodyTextId;
;        ULONG miss_SuggestedGadgetTextId;

INSUFFICIENTSPACE_Ignore	equ	1
INSUFFICIENTSPACE_Abort		equ	2

; ---------------------------------------------------------------------------

SCCM_FileTrans_Delete			equ	SCC_Dummy+221
;	BPTR DirLock
;	CONST_STRPTR Name

;------------------------------------------------------------------
	IF	0
; added by +jl+ from "scalos.h"

; texticon.datatype (scalos intern)

TIDTA_FileType          	equ DTA_Dummy+1151      ; (ISG)
TIDTA_FileSize          	equ DTA_Dummy+1152      ; (ISG)
TIDTA_Protection        	equ DTA_Dummy+1153      ; (ISG)
TIDTA_Days              	equ DTA_Dummy+1154      ; (ISG)
TIDTA_Mins              	equ DTA_Dummy+1155      ; (ISG)
TIDTA_Ticks             	equ DTA_Dummy+1156      ; (ISG)
TIDTA_Comment           	equ DTA_Dummy+1157      ; (ISG)
TIDTA_Owner_UID         	equ DTA_Dummy+1158      ; (ISG) UWORD
TIDTA_Owner_GID         	equ DTA_Dummy+1159      ; (ISG) UWORD
TIDTA_Name_Width        	equ DTA_Dummy+1160      ; (ISG) Pixel width
TIDTA_FileSize_Width    	equ DTA_Dummy+1161      ; (ISG) Pixel width
TIDTA_Protection_Width  	equ DTA_Dummy+1162      ; (ISG)
TIDTA_Date_Width        	equ DTA_Dummy+1163      ; (ISG)
TIDTA_Time_Width        	equ DTA_Dummy+1164      ; (ISG)
TIDTA_Comment_Width     	equ DTA_Dummy+1165      ; (ISG)
TIDTA_Owner_Width       	equ DTA_Dummy+1166      ; (ISG)
	ENDIF
;------------------------------------------------------------------

    STRUCTURE RootList,0	;this is the struct of every Scalos
				;Root.sca subclass
	APTR	rl_WindowTask	;Pointer to a WindowTask structure
	APTR	rl_internInfos	;Pointer to a Internal Infos struct
	LABEL	rl_SIZE		;!! This is not the end of this struct


    STRUCTURE internInfos,0
	APTR	ii_MainMsgPort		;Main message port
	APTR	ii_MainWindowStruct	;Pointer into the windowlist
					;from Desktop Window
	APTR	ii_AppWindowStruct	;Pointer into the windowlist
					;from Window for AppIcons
	APTR	ii_Screen		;the Scalos Screen
	APTR	ii_DrawInfo		;DrawInfo from this screen
	APTR	ii_visualinfo		;Gadtools VisualInfo
	LABEL	ii_SIZE

    STRUCTURE ScaClassInfo,0
	STRUCT	clsi_Hook,h_SIZEOF	;Dispatcher Hook
	UWORD	clsi_Priority		;Where to insert the class (-128 to 127)
	UWORD	clsi_InstSize		;Instance size
	UWORD	clsi_NeededVersion	;needed Scalos Version (normaly
					; this is 39)
	UWORD	clsi_Reserved		;should be NULL
	APTR	clsi_ClassName		;name of your class (max 50 chars)
	APTR	clsi_SuperClassName	;name of the superclass (max 50 chars)
	APTR	clsi_Name			;real name (max 100 chars)
	APTR	clsi_Description		;short description (max 200 chars)
	APTR	clsi_MakerName		;name of the programmer
	LABEL	clsi_SIZEOF


;----------------------- Scalos Class Hierachy -------------------------

;				(priority)

;-+- Root.sca			(-128)
; |
; +-- Window.sca		(-90)
; |
; +--- Title.sca		(-80)
; |
; +--- DeviceList.sca		(-80)
; |
; +--- FileTransfer.sca		(-80)
; |
; +-+- IconWindow.sca		(-80)
;   |
;   +-- DeviceWindow.sca	(-60)
;   |
;   +-- TextWindow.sca		(-60)


;-----------------------------------------------------------------------

;***************************************************************************
;*	Method structures for all Scalos SCCM_*** class methods		    
;***************************************************************************

; --- RootClass methods ------------------------------------------------

; SCCM_RunProcess
    STRUCTURE msg_RunProcess,0
	ULONG	mrp_MethodID
	APTR	mrp_EntryPoint
	APTR 	mrp_Args
	ULONG	mrp_ArgSize
	APTR	mrp_ReplyPort
	LABEL	mrp_SIZEOF

; SCCM_Message
    STRUCTURE msg_Message,0
	ULONG	msm_MethodID
	APTR	msm_iMsg
	LABEL	mscm_SIZEOF

; SCCM_AddToClipboard
    STRUCTURE msg_AddToClipboard,0
	ULONG	acb_MethodID
	APTR	acb_iwt
	APTR	acb_in
	ULONG	acb_Opcode
	LABEL	acb_SIZEOF

; SCCM_GetLocalizedString
    STRUCTURE msg_GetLocString,0
	ULONG	mgl_MethodID
	ULONG	mgl_StringID
	LABEL	mgl_SIZEOF

; ---------------------------------------------------------------------------
; --- DeviceWindowClass methods ----------------------------------------

; SCCM_DeviceWin_ReadIcon
    STRUCTURE msg_DevWinReadIcon,0
	ULONG	dri_MethodID
	APTR	dri_DeviceNode
	LABEL	dri_SIZEOF

; SCCM_DeviceWin_RemIcon
    STRUCTURE msg_DevWinRemIcon,0
	ULONG	dremi_MethodID
	APTR	dremi_IconNode
	LABEL	dremi_SIZEOF

; --- IconWindowClass methods ------------------------------------------

; SCCM_IconWin_ReadIconList
    STRUCTURE msg_ReadIconList,0
	ULONG	mrl_MethodID
	ULONG	mrl_Flags
	LABEL	mrl_SIZEOF

; SCCM_IconWin_ShowIconToolTip
    STRUCTURE msg_ShowIconToolTip,0
	ULONG	mtt_MethodID
	APTR	mtt_IconNode
	LABEL	mtt_SIZEOF

; SCCM_IconWin_ShowGadgetToolTip
    STRUCTURE msg_ShowGadgetToolTip,0
	ULONG	sgtt_MethodID
	ULONG	sgtt_GadgetID
	LABEL	sgtt_SIZEOF

; SCCM_IconWin_RawKey
    STRUCTURE msg_RawKey,0
	ULONG	mrk_MethodID
	APTR	mrk_iMsg
	LABEL	mrk_SIZEOF

; SCCM_IconWin_DragBegin
    STRUCTURE msg_DragBegin,0
	ULONG	mdb_MethodID
	APTR	mdb_DragEnter
	LABEL	mdb_SIZEOF

; SCCM_IconWin_DragDrop
    STRUCTURE msg_DragDrop,0
	ULONG	mdd_MethodID
	APTR	mdd_DropWindow
	ULONG	mdd_MouseX
	ULONG	mdd_MouseY
	ULONG	mdd_Qualifier
	LABEL	mdd_SIZEOF

; SCCM_IconWin_Open
    STRUCTURE msg_Open,0
	ULONG	mop_MethodID
	APTR	mop_IconNode
	ULONG	mop_Flags
	LABEL	mop_SIZEOF

; SCCM_IconWin_AddIcon
    STRUCTURE msg_AddIcon,0
	ULONG	mai_MethodID
	WORD	mai_x
	WORD	mai_y
	BPTR	mai_Lock
	APTR	mai_Name
	LABEL	mai_SIZEOF

; SCCM_IconWin_RemIcon
    STRUCTURE msg_RemIcon,0
	ULONG	mri_MethodID
	BPTR	mri_Lock
	APTR	mri_Name
	LABEL	mri_SIZEOF

; SCCM_IconWin_UpdateIconTags
    STRUCTURE msg_UpdateIconTags,0
	ULONG 	muit_MethodID;
	BPTR 	muit_Lock;
	APTR 	muit_Name;
	ULONG	muit_TagList		 ; may contain more tags added
	LABEL	muit_SIZEOF

; SCCM_IconWin_MakeWBArg
    STRUCTURE msg_MakeWbArg,0
	ULONG	mwa_MethodID
	APTR	mwa_Icon
	APTR	mwa_Buffer
	LABEL	mwa_SIZEOF

; SCCM_IconWin_CountWBArg
    STRUCTURE msg_CountWbArg,0
	ULONG	mca_MethodID
	APTR	mca_Icon
	LABEL	mca_SIZEOF

; SCCM_IconWin_SetVirtSize
    STRUCTURE msg_SetVirtSize,0
	ULONG	msv_MethodID
	ULONG	msv_Flags
	LABEL	msv_SIZEOF

; SCCM_IconWin_Redraw
    STRUCTURE msg_Redraw,0
	ULONG	mrd_MethodID
	ULONG	mrd_Flags
	LABEL	mrd_SIZEOF

; SCCM_IconWin_WakeUp
    STRUCTURE msg_Wakeup,0
	ULONG	mwu_MethodID
	ULONG	mwu_ReLayout
	LABEL	mwu_SIZEOF

; SCCM_IconWin_DrawIcon
    STRUCTURE msg_DrawIcon,0
	ULONG	mdi_MethodID
	APTR	mdi_IconObject
	LABEL	mdi_SIZEOF

; SCCM_IconWin_LayoutIcon
    STRUCTURE msg_LayoutIcon,0
	ULONG	mli_MethodID
	APTR	mli_IconObject
	ULONG	mli_LayoutFlags
	LABEL	mli_SIZEOF

; SCCM_IconWin_GetDefIcon
    STRUCTURE msg_GetDefIcon,0
	ULONG	mgd_MethodID
	APTR	mgd_Name
	LONG	mgd_Type;	; fib_DirEntryType from FileInfoBlock
	ULONG	mgd_Protection	; fib_Protection from FileInfoBlock
	LABEL	mgd_SIZEOF

; SCCM_IconWin_MenuCommand
    STRUCTURE msg_MenuCommand,0
	ULONG	mcm_MethodID
	APTR	mcm_CmdName
	APTR	mcm_IconNode
	ULONG	mcm_State
	LABEL	mcm_SIZEOF

; SCCM_IconWin_NewViewMode
    STRUCTURE msg_NewViewMode,0
	ULONG	mnv_MethodID
	ULONG	mnv_NewMode
	LABEL	mnv_SIZEOF

; SCCM_IconWin_DeltaMove
    STRUCTURE msg_DeltaMove,0
	ULONG	mdm_MethodID
	LONG	mdm_DeltaX
	LONG	mdm_DeltaY
	LABEL	mdm_SIZEOF

; SCCM_IconWin_AddToStatusBar
    STRUCTURE msg_AddToStatusBar,0
	ULONG	mab_MethodID
	APTR	mab_NewMember
	ULONG	mab_TagList		; may contain more tags added
	LABEL	mab_SIZEOF

; SCCM_IconWin_RemFromStatusBar
    STRUCTURE msg_RemFromStatusBar,0
	ULONG	mrb_MethodID
	APTR	mrb_OldMember
	LABEL	mrb_SIZEOF

; SCCM_IconWin_UpdateStatusBar
    STRUCTURE msg_UpdateStatusBar,0
	ULONG	mub_MethodID
	APTR	mub_Member
	ULONG	mub_TagList		; may contain more tags added
	LABEL	mub_SIZEOF

; SCCM_IconWin_AddGadget
    STRUCTURE msg_AddGadget,0
	ULONG	mag_MethodID
	APTR	mag_NewGadget
	LABEL	mag_SIZEOF

; SCCM_IconWin_ReadIcon
    STRUCTURE msg_ReadIcon,0
	ULONG	mrdi_MethodID
	APTR	mrdi_Name;		; icon name
	APTR	mrdi_ria
	LABEL	mrdi_SIZEOF

; SCCM_IconWin_ShowPopupMenu
    STRUCTURE msg_ShowPopupMenu,0
	ULONG	mpm_MethodID
	APTR	mpm_PopupMenu
	APTR	mpm_IconNode
	ULONG	mpm_Flags
	ULONG	mpm_Qualifier
	APTR	mpm_FileType
	LABEL	mpm_SIZEOF

; SCCM_IconWin_NewPatternNumber
    STRUCTURE  msg_NewPatternNumber,0
	ULONG 	npn_MethodID
	ULONG 	npn_PatternNumber
	LABEL	npn_SIZEOF

; SCCM_IconWin_NewPath
    STRUCTURE msg_NewPath,0
	ULONG	npa_MethodID
	APTR 	npa_Path
	LABEL	npa_SIZEOF

; SCCM_IconWin_StartPopOpenTimer
    STRUCTURE msg_StartPopOpenTimer,0
	ULONG 	spot_MethodID
	APTR	spot_DestWindow
	APTR	spot_DragHandle
	APTR	spot_IconNode
	LABEL	spot_SIZEOF

; SCCM_IconWin_StopPopOpenTimer
    STRUCTURE msg_StopPopOpenTimer,0
	ULONG 	stop_MethodID
	APTR	stop_DragHandle
	LABEL	stop_SIZEOF

; SCCM_IconWin_AddUndoEvent
    STRUCTURE msg_AddUndoEvent,0
	ULONG 	aueb_MethodID
	ULONG	aue_Type;
	ULONG	aue_TagList		; may contain more tags added
	LABEL	aue_SIZEOF

; SCCM_IconWin_EndUndoStep
    STRUCTURE msg_EndUndoStep,0
	ULONG 	eus_MethodID		
	APTR 	eus_UndoStep            ; Result from SCCM_IconWin_BeginUndoStep
	LABEL	eus_SIZEOF

; SCCM_IconWin_UnCleanUpRegion
    STRUCTURE msg_UnCleanUpRegion,0
	ULONG 	ucr_MethodID
	STRUCT	ucr_UnCleanUpRegion,rg_SIZEOF ; struct Region
	LABEL	ucr_SIZEOF

; --- TextWindowClass methods ------------------------------------------

; SCCM_TextWin_EndUpdate
    STRUCTURE msg_EndUpdate,0
	ULONG	meu_MethodID
	ULONG	meu_RefreshFlag
	LABEL	meu_SIZEOF

; SCCM_TextWin_InsertIcon
    STRUCTURE msg_InsertIcon,0
	ULONG	iic_MethodID
	APTR	iic_IconNode
	LABEL	iic_SIZEOF

; --- WindowClass methods ------------------------------------------

    STRUCTURE msg_RemClipRegion,0
	ULONG	mrc_MethodID
	APTR	mrc_oldClipRegion
	LABEL	mrc_SIZEOF

    STRUCTURE msg_SetTitle,0
	ULONG	mst_MethodID
	APTR	mst_Title
	LABEL	mst_SIZEOF

    STRUCTURE msg_GetIconFileType,0
	ULONG	mft_MethodID
	APTR	mft_IconNode
	LABEL	mft_SIZEOF

; --- FileTransClass methods ------------------------------------------

    STRUCTURE msg_Move,0
	ULONG	mmv_MethodID
	BPTR	mmv_SrcDirLock
	BPTR	mmv_DestDirLock
	APTR	mmv_Name
	LONG	mmv_MouseX
	LONG	mmv_MouseY
	LABEL	mmv_SIZEOF

    STRUCTURE msg_Copy,0
	ULONG	mcp_MethodID
	BPTR	mcp_SrcDirLock
	BPTR	mcp_DestDirLock
	APTR	mcp_SrcName
	APTR	mcp_DestName
	LONG	mcp_MouseX
	LONG	mcp_MouseY
	LABEL	mcp_SIZEOF

    STRUCTURE msg_CreateLink,0
	ULONG	mcl_MethodID
	BPTR	mcl_SrcDirLock
	BPTR	mcl_DestDirLock
	APTR	mcl_SrcName
	APTR	mcl_DestName
	LONG	mcl_MouseX
	LONG	mcl_MouseY
	LABEL	mcl_SIZEOF

    STRUCTURE msg_UpdateWindow,0
	ULONG	muw_MethodID
	ULONG	muw_UpdateMode
	BPTR	muw_SrcDirLock
	BPTR	muw_DestDirLock
	APTR	muw_SrcName
	LABEL	muw_SIZEOF

    STRUCTURE msg_OverwriteRequest,0
	ULONG	mov_MethodID
	ULONG	mov_RequestType
	BPTR	mov_SrcLock
	APTR	mov_SrcName
	BPTR	mov_DestLock
	APTR	mov_DestName
	APTR	mov_ParentWindow
	ULONG	mov_SuggestedBodyTextId
	ULONG	mov_SuggestedGadgetTextId
	LABEL	mov_SIZEOF

    STRUCTURE msg_Delete,0
	ULONG 	mmd_MethodID
	BPTR 	mmd_DirLock
	APTR	mmd_Name	; CONST_STRPTR
	LABEL	mmd_SIZEOF

;-----------------------------------------------------------------------

; Text icon types
WB_TEXTICON_TOOL	EQU	10
WB_TEXTICON_DRAWER	EQU	9

;-----------------------------------------------------------------------

; possible selections for thumbnail display

THUMBNAILS_Never	EQU	0
THUMBNAILS_AsDefault	EQU	1
THUMBNAILS_Always	EQU	2

;-----------------------------------------------------------------------

; Icon layout modes

ICONTYPE_MAX		EQU	9	; (1 + WBAPPICON)
ICONLAYOUT_RESTRICTED_MAX EQU	4

ICONLAYOUT_Columns	EQU	0	; Layout top-down in columns (default)
ICONLAYOUT_Rows		EQU	1	; Layout left-to-right in rows

;-----------------------------------------------------------------------

; Scalos library base

    STRUCTURE ScalosBase,LIB_SIZE
	;struct Library scb_LibNode;
	STRUCT 	scb_Pad,2		; pad to next longword border
	APTR 	scb_Revision		; "1.4f" or "1.5" etc.
	LABEL	scb_SIZEOF

;-----------------------------------------------------------------------

	ENDC	; SCALOS_I
