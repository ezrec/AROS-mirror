	IFND	SCALOS_I
SCALOS_I	SET	1
**
**	$VER: scalos.i 39.124 (28.5.97)
**
**      Scalos.library include
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
**

	include	scalos/iconobject.i

;---------------------------------------------------------------------------

SCA_IconObject			EQU	$80530001
SCA_Stacksize			EQU	$80530002
SCA_Flags			EQU	$80530003
SCA_IconNode			EQU	$80530004
SCA_WindowTitle			EQU	$80530005
SCA_Path			EQU	$80530006
SCA_WindowRect			EQU	$80530007
SCA_XOffset			EQU	$80530008
SCA_YOffset			EQU	$80530009
SCA_PatternNumber		EQU	$8053000a	;see pattern.i
SCA_ShowAllFiles		EQU	$8053000b
SCA_ViewModes			EQU	$8053000c
SCA_MessagePort			EQU	$8053000d
SCA_Iconify			EQU	$8053000e

SCAV_WBStart_NoIcon		EQU	$00000001
SCAV_WBStart_Wait		EQU	$00000002
SCAV_WBStart_PathSearch		EQU	$00000004
SCAV_WBStart_NoIconCheck	EQU	$00000008

SCAB_OpenWindow_ScalosPort	EQU	$00000000	;private

SCAV_ViewModes_Icon		EQU	IDTV_ViewModes_Icon
SCAV_ViewModes_Name		EQU	IDTV_ViewModes_Name
SCAV_ViewModes_Size		EQU	IDTV_ViewModes_Size
SCAV_ViewModes_Date		EQU	IDTV_ViewModes_Date

;LockWindowList():
SCA_LockWindowList_Shared	EQU	0
SCA_LockWindowList_Exclusiv	EQU	1

;FreeWBArgs():
SCAV_FreeLocks			EQU	1
SCAB_FreeLocks			EQU	0
SCAV_FreeNames			EQU	2
SCAB_FreeNames			EQU	1

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

;------------------------ MessageTypes for AllocMsg() -----------------------

MTYP_CloseWindow		EQU	2
MTYP_Timer			EQU	4
MTYP_NewPattern			EQU	6
MTYP_Sleep			EQU	7
MTYP_Wakeup			EQU	8
MTYP_AppSleep			EQU	11
MTYP_AppWakeup			EQU	12
MTYP_Redraw			EQU	13
MTYP_Update			EQU	14
MTYP_UpdateIcon			EQU	15
MTYP_AddIcon			EQU	16
MTYP_RemIcon			EQU	17
MTYP_Iconify			EQU	19
MTYP_UnIconify			EQU	20

    STRUCTURE ScalosMessage,MN_SIZE
	ULONG	Signature	;("IMSG")
	ULONG	MessageType	;AllocMsg() type
	LABEL	sm_SIZE

    STRUCTURE Message_CloseWindow,sm_SIZE
	LABEL	sm_mc_SIZE

    STRUCTURE Message_NewPattern,sm_SIZE
	APTR	sm_np_PatternNode		;PatternNode or NULL
	LABEL	sm_np_SIZE

    STRUCTURE Message_Sleep,sm_SIZE
	LABEL	sm_ms_SIZE

    STRUCTURE Message_Wakeup,sm_SIZE
	ULONG	sm_mw_ReLayout			;BOOL
	LABEL	sm_mw_SIZE

    STRUCTURE Message_AppSleep,sm_SIZE
	LABEL	sm_ap_SIZE

    STRUCTURE Message_AppWakeup,sm_SIZE
	ULONG	sm_aw_ReLayout			;BOOL
	LABEL	sm_aw_SIZE

    STRUCTURE Message_Redraw,sm_SIZE
	ULONG	sm_mr_Flags			;see below
	LABEL	sm_mr_SIZE

    STRUCTURE Message_Update,sm_SIZE
	LABEL	sm_mu_SIZE

    STRUCTURE Message_UpdateIcon,sm_SIZE
	LABEL	sm_ui_SIZE

    STRUCTURE Message_AddIcon,sm_SIZE
	UWORD	x
	UWORD	y
	LABEL	sm_ai_SIZE

    STRUCTURE Message_RemIcon,sm_SIZE
	LABEL	sm_ri_SIZE

    STRUCTURE Message_Iconify,sm_SIZE
	LABEL	sm_ic_SIZE

    STRUCTURE Message_UnIconify,sm_SIZE
	LABEL	sm_un_SIZE


;---------------------------------------------------------------------------

    STRUCTURE WindowList,0
	APTR	wl_WindowStruct			;Pointer to the first windowstruct
	APTR	ml_MainWindowStruct		;Pointer into the list of windowstructs
	APTR	ml_AppWindowStruct		;dito - windowstruct for AppIcons
	LABEL	wl_SIZE

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
	LABEL	ws_SIZE

;ws_Flags:
WSV_Flags_RootWindow		EQU	1	;a RootDir window
WSV_Flags_NeedsTimerMsg		EQU	2	;window likes to get TimerMessages
WSV_Flags_Backdrop		EQU	4	;is the window in backdrop mode
WSV_Flags_TimerMsgSent		EQU	8	;timermsg already sent *PRIVATE*
WSV_Flags_TaskSleeps		EQU	16	;windowtask is sleeping
WSV_Flags_ShowAllFiles		EQU	32	;just what it said
WSV_Flags_Iconify		EQU	64	;windowtask is in iconify state

;ws_WindowType:
WSV_Type_IconWindow		EQU	0	;Window filled with icons
WSV_Type_DeviceWindow		EQU	1


    STRUCTURE WindowTask,0
	STRUCT	wt_PatternInfo
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
	LABEL	wt_SIZE		;It's only the documented end of this structure.
					;In reality it's much bigger.

    STRUCTURE IconNode,MLN_SIZE
	APTR	in_Icon			;IconObject from iconobject.library
	APTR	in_Name			;Filename of the icon
	APTR	in_DeviceIcon		;NULL except for disk icons
	APTR	in_Lock			;lock to the files dir or NULL
	APTR	in_IconList		;list of linked icons (used for backdrop icons)
	UWORD	in_Flags		;see below
	UWORD	in_Userdata
	LABEL	in_SIZE

;in_Flags:
INB_DefaultIcon			EQU	0	;the icon has diskobject
INB_File			EQU	1	;File or Drawer (TextMode only)
INB_TextIcon			EQU	2	;TextMode icon
INB_Sticky			EQU	3	;icon not moveable

     STRUCTURE DeviceIcon,MLN_SIZE
	APTR	di_Handler		;MessagePort of the HandlerTask
	APTR	di_Volume		;Volumename or NULL
	APTR	di_Device		;Devicename or NULL
	APTR	di_Icon			;IconObject of this DeviceIcon struct
	APTR	di_Info			;Infodata from dos.library/Info()
	UWORD	di_Flags		;see below
	LABEL	di_SIZE

;di_Flags
DIB_DontDisplay			EQU	0	;Don't display this icon


;UpdateIcon structure for Icon and Device windowtypes

    STRUCTURE UpdateIcon_IW,0
	BPTR	ui_iw_Lock		;Lock to the file's/dir's directory
	APTR	ui_iw_Name		;Name of the file or dir, NULL for Disks
	LABEL	ui_iw_SIZE


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

;--------------- Methodes -------------

SCCA_Window_IDCMP		EQU	SCC_Dummy+2

;-------------- Attributes ------------

SCCM_Window_Open		EQU	SCC_Dummy+123
SCCM_Window_Close		EQU	SCC_Dummy+124
SCCM_Window_Iconify		EQU	SCC_Dummy+125
SCCM_Window_UnIconify		EQU	SCC_Dummy+126
SCCM_Window_InitClipRegion	EQU	SCC_Dummy+127
;Result: OldClipRegion

SCCM_Window_RemClipRegion	EQU	SCC_Dummy+128
;APTR	OldClipRegion

SCCM_Window_SetTitle		EQU	SCC_Dummy+129

;---------------- IconWindow Class ----------------------------
;Name: "IconWindow.sca"

;--------------- Methodes -------------
SCCM_IconWin_ReadIconList	EQU	SCC_Dummy+100
;ULONG	flags
;Result: BOOL CloseWindow
;Flags:
SCCV_IconWin_ReadIconList_ShowAll	EQU	1

SCCM_IconWin_ReadIcon		EQU	SCC_Dummy+109
SCCM_IconWin_DragBegin		EQU	SCC_Dummy+110
SCCM_IconWin_DragQuery		EQU	SCC_Dummy+111
SCCM_IconWin_DragFinish		EQU	SCC_Dummy+112
SCCM_IconWin_DragDrop		EQU	SCC_Dummy+113
SCCM_IconWin_Open		EQU	SCC_Dummy+114
;APTR	iconnode	(IconNode to open or NULL)
;Result: BOOL

SCCM_IconWin_UpdateIcon		EQU	SCC_Dummy+115
SCCM_IconWin_AddIcon		EQU	SCC_Dummy+116
SCCM_IconWin_RemIcon		EQU	SCC_Dummy+117
SCCM_IconWin_MakeWBArg		EQU	SCC_Dummy+118
SCCM_IconWin_CleanUp		EQU	SCC_Dummy+119
SCCM_IconWin_SetVirtSize	EQU	SCC_Dummy+121
;UWORD	Flags
;Flags:
; bit 0 = set Right-SLider
; bit 1 = set Bottom-Slider
SCCM_IconWin_Redraw		EQU	SCC_Dummy+122
SCCM_IconWin_Update		EQU	SCC_Dummy+130
SCCM_IconWin_Sleep		EQU	SCC_Dummy+131
SCCM_IconWin_WakeUp		EQU	SCC_Dummy+132
SCCM_IconWin_MenuCommand	EQU	SCC_Dummy+138
;APTR	CommandName
;APTR	IconNode	;(or NULL)
;BOOL	State		;(checkmark state if it's a checkmark)

SCCM_IconWin_NewViewMode	EQU	SCC_Dummy+139
;ULONG	ViewMode
;Result: IconWindow.sca subclass object


;---------------- DeviceWindow Class ----------------------------
;Name: "DeviceWindow.sca"

;--------------- Methodes -------------
SCCM_DeviceWin_ReadIcon		EQU	SCC_Dummy+136
;APTR DeviceNode

;---------------- TextWindow Class ----------------------------
;Name: "TextWindow.sca"

;--------------- Methodes -------------
SCCM_TextWin_ReadIcon		EQU	SCC_Dummy+137


;--------------- DeviceList Class ----------------------------
;Name: "DeviceList.sca"

;--------------- Methodes -------------
SCCM_DeviceList_Generate	EQU	SCC_Dummy+133
;APTR ScalosNodeList

SCCM_DeviceList_FreeDevNode	EQU	SCC_Dummy+134
;APTR Node

SCCM_DeviceList_Filter		EQU	SCC_Dummy+135
;APTR ScalosNodeList

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

    STRUCTURE ClassInfo,0
	STRUCT	ci_Hook,HOOK		;Dispatcher Hook
	UWORD	ci_priority		;Where to insert the class (-128 to 127)
	UWORD	ci_instsize		;Instance size
	UWORD	ci_neededversion	;needed Scalos Version (normaly
					; this is 39)
	UWORD	ci_reserved		;should be NULL
	APTR	ci_classname		;name of your class (max 50 chars)
	APTR	ci_superclassname	;name of the superclass (max 50 chars)
	APTR	ci_name			;real name (max 100 chars)
	APTR	ci_description		;short description (max 200 chars)
	APTR	ci_makername		;name of the programmer
	LABEL	ci_SIZE


;----------------------- Scalos Class Hirachie -------------------------
;(priority)

;-+- Root.sca (-128)
; |
; +-- Window.sca (-90)
; |
; +--- Title.sca (-80)
; |
; +--- DeviceList.sca (-80)
; |
; +-+- IconWindow.sca (-80)
;   |
;   +-- DeviceWindow.sca (-60)
;   |
;   +-- TextWindow.sca (-60)


;-----------------------------------------------------------------------

	ENDC	; SCALOS_I
