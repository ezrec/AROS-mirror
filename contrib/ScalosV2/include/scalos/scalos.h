#ifndef SCALOS_SCALOS_H
#define SCALOS_SCALOS_H
/*
**	$VER: scalos.h 39.124 (28.5.97)
**
**      Scalos.library include
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
*/

#ifndef DOS_DOS_H
#include	<dos/dos.h>
#endif

#ifndef EXEC_LISTS_H
#include	<exec/lists.h>
#endif

#ifndef EXEC_PORTS_H
#include	<exec/ports.h>
#endif

#ifndef	INTUITION_CLASSES_H
#include	<intuition/classes.h>
#endif

#ifndef SCALOS_ICONOBJECT_H
#include	<scalos/iconobject.h>
#endif


// ---------------------------------------------------------------------------

#define SCA_IconObject			0x80530001
#define SCA_Stacksize			0x80530002
#define SCA_Flags					0x80530003
#define SCA_IconNode				0x80530004
#define SCA_WindowTitle			0x80530005
#define SCA_Path					0x80530006
#define SCA_WindowRect			0x80530007
#define SCA_XOffset				0x80530008
#define SCA_YOffset				0x80530009
#define SCA_PatternNumber		0x8053000a		// see pattern.h
#define SCA_ShowAllFiles		0x8053000b
#define SCA_ViewModes			0x8053000c
#define SCA_MessagePort			0x8053000d
#define SCA_Iconify				0x8053000e

#define SCAV_WBStart_NoIcon	0x00000001
#define SCAV_WBStart_Wait		0x00000002
#define SCAV_WBStart_PathSearch	0x00000004
#define SCAV_WBStart_NoIconCheck	0x00000008

#define SCAB_OpenWindow_ScalosPort	0x00000000	// private

#define SCAV_ViewModes_Icon		IDTV_ViewModes_Icon
#define SCAV_ViewModes_Name		IDTV_ViewModes_Name
#define SCAV_ViewModes_Size		IDTV_ViewModes_Size
#define SCAV_ViewModes_Date		IDTV_ViewModes_Date

// LockWindowList():
#define SCA_LockWindowList_Shared	0
#define SCA_LockWindowList_Exclusiv	1

// FreeWBArgs():
#define SCAV_FreeLocks				1
#define SCAB_FreeLocks				0
#define SCAV_FreeNames				2
#define SCAB_FreeNames				1

#define SCA_SortType_Bubble		0
#define SCA_SortType_Selection	1
#define SCA_SortType_Insertion	2
#define SCA_SortType_Quick			3
#define SCA_SortType_Best			-1


// ---------------------- NodeTypes for AllocStdNode() ------------------------

#define NTYP_IconNode				0
#define NTYP_WindowTask				1
#define NTYP_DragNode				2
#define NTYP_PatternNode			3
#define NTYP_ScalosArg				4
#define NTYP_DeviceIcon				5
#define NTYP_MenuTree				6
#define NTYP_MenuCommand			7
#define NTYP_AppObject				8
#define NTYP_MenuInfo				9

// ------------------------ MessageTypes for AllocMsg() -----------------------

#define MTYP_CloseWindow			2
#define MTYP_Timer					4
#define MTYP_NewPattern				6
#define MTYP_Sleep					7
#define MTYP_Wakeup					8
#define MTYP_AppSleep				11
#define MTYP_AppWakeup				12
#define MTYP_Redraw					13
#define MTYP_Update					14
#define MTYP_UpdateIcon				15
#define MTYP_AddIcon					16
#define MTYP_RemIcon					17
#define MTYP_Iconify					19
#define MTYP_UnIconify				20

struct ScalosMessage {
	struct Message			sm_Message;
	ULONG						sm_Signature;		// ("IMSG")
	ULONG						sm_MessageType;	// AllocMsg() type
};

struct SM_CloseWindow {
	struct ScalosMessage	ScalosMessage;
};

struct SM_NewPattern {
	struct ScalosMessage	ScalosMessage;
	APTR						PatternNode;	// PatternNode or NULL
};

struct SM_Sleep {
	struct ScalosMessage	ScalosMessage;
};

struct SM_Wakeup {
	struct ScalosMessage	ScalosMessage;
	ULONG						ReLayout;		// BOOL
};

struct SM_AppSleep {
	struct ScalosMessage	ScalosMessage;
};

struct SM_AppWakeup {
	struct ScalosMessage	ScalosMessage;
	ULONG						ReLayout;		// BOOL
};

struct SM_Redraw {
	struct ScalosMessage	ScalosMessage;
	ULONG						Flags;			// see below
};

struct SM_Update {
	struct ScalosMessage	ScalosMessage;
};

struct SM_UpdateIcon {
	struct ScalosMessage	ScalosMessage;
};

struct SM_AddIcon {
	struct ScalosMessage	ScalosMessage;
	UWORD						x;
	UWORD						y;
};

struct SM_RemIcon {
	struct ScalosMessage	ScalosMessage;
};

struct SM_Iconify {
	struct ScalosMessage	ScalosMessage;
};

struct SM_UnIconify {
	struct ScalosMessage	ScalosMessage;
};



// ---------------------------------------------------------------------------

struct ScaWindowList {
	struct ScaWindowStruct 	*wl_WindowStruct;			// Pointer to the first windowstruct
	struct ScaWindowStruct	*ml_MainWindowStruct;	// Pointer into the list of windowstructs
	struct ScaWindowStruct	*ml_AppWindowStruct;		// dito - windowstruct for AppIcons
};

struct ScaWindowStruct {
	struct MinNode			ws_Node;
	APTR						ws_Task;				// Window-Task address
	BPTR						ws_Lock;				// filelock to the windows directory
	struct Window			*ws_Window;			// pointer to the window or NULL
	struct MsgPort			*ws_MessagePort;	// the API-messageport
	WORD						ws_Left;
	WORD						ws_Top;
	WORD						ws_Width;
	WORD						ws_Height;			// size and position of the window
	WORD						ws_xoffset;
	WORD						ws_yoffset;			// the virtual position inside
	APTR						ws_PatternNode;
	struct ScaWindowTask *ws_WindowTask;		// windowtask structure
	STRPTR					ws_Name;				// windows name (is displayed in the title)
	STRPTR					ws_Title;			// title-formatstring
	STRPTR					ms_ClassName;		// scalos class name
	Class						*ms_Class;			// BOOPSI Class if ms_classname is NULL
	UWORD						ws_Flags;			// see below
	UWORD						ws_PatternNumber;	// number of the pattern
	UBYTE						ws_Viewmodes;		// see below
	UBYTE						ws_WindowType;		// see below
};

// ws_Flags:
#define WSV_Flags_RootWindow		1			// a RootDir window
#define WSV_Flags_NeedsTimerMsg	2			// window likes to get TimerMessages
#define WSV_Flags_Backdrop			4			// is the window in backdrop mode
#define WSV_Flags_TimerMsgSent	8			// timermsg already sent *PRIVATE*
#define WSV_Flags_TaskSleeps		16			// windowtask is sleeping
#define WSV_Flags_ShowAllFiles	32			// just what it said
#define WSV_Flags_Iconify			64			// windowtask is in iconify state

// ws_WindowType:
#define WSV_Type_IconWindow		0			// Window filled with icons
#define WSV_Type_DeviceWindow		1


struct ScaWindowTask {
	char						wt_PatternInfo[40];
	WORD						wt_XOffset;
	WORD						wt_YOffset;			// Innerwindow offset
	APTR						mt_MainObject;		// main ScalosClass object
	APTR						mt_WindowObject;	// "Window.sca" object
	struct Window			*wt_Window;			// pointer to the window of the task or NULL
	struct ScaIconNode	*wt_IconList;		// Pointer to the first IconNode
	struct ScaIconNode	*wt_LateIconList;	// Pointer to the first IconNode of all not yet
														// displayed icons (e.g. non-position icon while
														// loading)
	struct SignalSemaphore	*wt_IconSemaphore;// Semaphore to access the wt_IconList/wt_LateIconList
														// It *MUST* be locked before accessing the list.
														// Shared lock should be used for read-only access
	struct MsgPort			*wt_IconPort;		// MessagePort of the windowtask
	struct SignalSemaphore	*wt_WindowSemaphore;// Semaphore to avoid the task from closing
	struct ScaWindowStruct *mt_WindowStruct;// Pointer to the WindowStruct of this task
};														// It's only the documented end of this structure.
														// In reality it's much bigger.

struct ScaIconNode {
	struct MinNode			in_Node;
	APTR						in_Icon;				// IconObject from iconobject.library
	STRPTR					in_Name;				// Filename of the icon
	struct ScaDeviceIcon *in_DeviceIcon;	// NULL except for disk icons
	BPTR						in_Lock;				// lock to the files dir or NULL
	APTR						in_IconList;		// list of linked icons (used for backdrop icons)
	UWORD						in_Flags;			// see below
	UWORD						in_Userdata;
};

// in_Flags:
#define INB_DefaultIcon		0		// the icon has diskobject
#define INB_File				1		// File or Drawer (TextMode only)
#define INB_TextIcon			2		// TextMode icon
#define INB_Sticky			3		// icon not moveable
#define INF_DefaultIcon		(1L<<INB_DefaultIcon)
#define INF_File				(1L<<INB_File)
#define INF_TextIcon			(1L<<INB_TextIcon)
#define INF_Sticky			(1L<<INB_Sticky)

struct ScaDeviceIcon {
	struct MinNode			di_Node;
	struct MsgPort			*di_Handler;		// MessagePort of the HandlerTask
	STRPTR					di_Volume;			// Volumename or NULL
	STRPTR					di_Device;			// Devicename or NULL
	APTR						di_Icon;				// IconObject of this DeviceIcon struct
	APTR						di_Info;				// Infodata from dos.library/Info()
	UWORD						di_Flags;			// see below
};

// di_Flags
#define DIB_DontDisplay			0				// Don't display this icon


// UpdateIcon structure for Icon and Device windowtypes

struct ScaUpdateIcon_IW {
	BPTR						ui_iw_Lock;			// Lock to the file's/dir's directory
	STRPTR					ui_iw_Name;			// Name of the file or dir, NULL for Disks
};


struct ScalosNodeList {
	struct MinNode			*snl_MinNode;		// Pointer to the first node or NULL
};


// ------------------------------- Scalos Class Information --------------------------

#define SCC_Dummy				0x80580000


// -------------------- Root Class -------------------------------
// Name: "Root.sca"

// ---------------------- Attributes ------------------------------

#define SCCA_WindowTask				SCC_Dummy+1		// (ISG)

// ----------------------- Methodes -------------------------------

#define SCCM_HandleMessages		SCC_Dummy+105
#define SCCM_ScalosMessage			SCC_Dummy+106		// !
#define SCCM_Message					SCC_Dummy+107
struct SCCP_Message { ULONG MethodID; struct IntuiMessage *IntuiMsg; };
#define SCCM_ScalosMessageReply	SCC_Dummy+108		// !
#define SCCM_RunProcess				SCC_Dummy+120
struct SCCP_RunProcess { ULONG MethodID; APTR Routine; APTR Args;
									ULONG argsSize; APTR ReplyPort; /*see values below*/ };

#define SCCV_RunProcess_NoReply				0
#define SCCV_RunProcess_WaitReply			-1

#define SCCM_Ping						SCC_Dummy+140	// Timer Ping

// -------------------- Title Class -------------------------------
// Name: "Title.sca"

// -------------- Attributes ------------

#define SCCA_Title_Format				SCC_Dummy+2	// (IS.)
#define SCCA_Title_Type					SCC_Dummy+3	// (I..)

#define SCCV_Title_Type_Screen				0		// (default)
#define SCCV_Title_Type_Window				1

// --------------- Methodes -------------

#define SCCM_Title_Generate			SCC_Dummy+101	// Result: MemPtr

#define SCCM_Title_Query				SCC_Dummy+102
struct SCCP_Title_Query { ULONG MethodID; UWORD ParseID; };	// Result: BOOL (Needs Refresh)

#define SCCM_Title_QueryTitle			SCC_Dummy+103	// Result: BOOL (Needs Refresh)

#define SCCM_Title_Translate			SCC_Dummy+104
struct SCCP_Title_Translate { ULONG MethodID; char *TitleMem; char *TitleFormat;
										UWORD ParseID; };	// Result: succ BOOL


// ----------------- Window Class -------------------------------
// Name: "Window.sca"

// --------------- Methodes -------------

#define SCCA_Window_IDCMP				SCC_Dummy+2

// -------------- Attributes ------------

#define SCCM_Window_Open				SCC_Dummy+123
#define SCCM_Window_Close				SCC_Dummy+124
#define SCCM_Window_Iconify			SCC_Dummy+125
#define SCCM_Window_UnIconify			SCC_Dummy+126
#define SCCM_Window_InitClipRegion	SCC_Dummy+127	// Result: OldClipRegion

#define SCCM_Window_RemClipRegion	SCC_Dummy+128
struct SCCP_Window_RemClipRegion { ULONG MethodID; APTR OldClipRegion; };

#define SCCM_Window_SetTitle			SCC_Dummy+129
#define SCCM_Window_SetInnerSize		SCC_Dummy+143

// ---------------- IconWindow Class ----------------------------
// Name: "IconWindow.sca"

// --------------- Methodes -------------
#define SCCM_IconWin_ReadIconList	SCC_Dummy+100
struct SCCP_IconWin_ReadIconList { ULONG MethodID;
												ULONG flags; };		// Result: BOOL CloseWindow
#define SCCV_IconWin_ReadIconList_ShowAll			1

#define SCCM_IconWin_ReadIcon			SCC_Dummy+109
#define SCCM_IconWin_DragBegin		SCC_Dummy+110
#define SCCM_IconWin_DragQuery		SCC_Dummy+111
#define SCCM_IconWin_DragFinish		SCC_Dummy+112
#define SCCM_IconWin_DragDrop			SCC_Dummy+113
struct SCCP_IconWin_DragDrop { ULONG MethodID;
										APTR dropwindow;
										ULONG	mousex, mousey; };
#define SCCM_IconWin_Open				SCC_Dummy+114
struct SCCP_IconWin_Open { ULONG MethodID;
									APTR iconnode; /* IconNode to open or NULL */ };	// Result: BOOL

#define SCCM_IconWin_UpdateIcon		SCC_Dummy+115
#define SCCM_IconWin_AddIcon			SCC_Dummy+116
#define SCCM_IconWin_RemIcon			SCC_Dummy+117
#define SCCM_IconWin_MakeWBArg		SCC_Dummy+118
#define SCCM_IconWin_CleanUp			SCC_Dummy+119
#define SCCM_IconWin_SetVirtSize		SCC_Dummy+121
struct SCCP_IconWin_SetVirtSize { ULONG MethodID;
											UWORD Flags; };
// UWORD	Flags
// Flags:
//  bit 0 = set Right-SLider
//  bit 1 = set Bottom-Slider

#define SCCM_IconWin_Redraw			SCC_Dummy+122
#define SCCM_IconWin_Update			SCC_Dummy+130
#define SCCM_IconWin_Sleep				SCC_Dummy+131
#define SCCM_IconWin_WakeUp			SCC_Dummy+132
#define SCCM_IconWin_MenuCommand		SCC_Dummy+138
struct SCCP_IconWin_MenuCommand { ULONG MethodID;
											APTR CommandName;
											APTR IconNode; /*NULL is legal*/
											BOOL State; /*checkmark state if it's a checkmark*/ };

#define SCCM_IconWin_NewViewMode		SCC_Dummy+139
struct SCCP_IconWin_NewViewMode { ULONG MethodID; ULONG ViewMode; };		// Result: IconWindow.sca subclass object

#define SCCM_IconWin_DrawIcon			SCC_Dummy+141
struct SCCP_IconWin_DrawIcon { ULONG MethodID; APTR Icon; };

#define SCCM_IconWin_LayoutIcon		SCC_Dummy+142
struct SCCP_IconWin_LayoutIcon { ULONG MethodID; APTR Icon; ULONG LayoutFlags; };

#define SCCM_IconWin_DeltaMove		SCC_Dummy+144
struct SCCP_IconWin_DeltaMove { ULONG MethodID; LONG deltaX; LONG deltaY; };

#define SCCM_IconWin_GetDefIcon		SCC_Dummy+146
struct SCCP_IconWin_GetDefIcon { ULONG MethodID; char *filename; LONG fileType; ULONG protectionBits; };	// Result: Icon

// ---------------- DeviceWindow Class ----------------------------
// Name: "DeviceWindow.sca"

// --------------- Methodes -------------
#define SCCM_DeviceWin_ReadIcon		SCC_Dummy+136
struct SCCP_DeviceWin_ReadIcon { ULONG MethodID; APTR DeviceNode; };

// ---------------- TextWindow Class ----------------------------
// Name: "TextWindow.sca"

// --------------- Methodes -------------
#define SCCM_TextWin_ReadIcon			SCC_Dummy+137
#define SCCM_TextWin_ReAdjust			SCC_Dummy+145


// --------------- DeviceList Class ----------------------------
// Name: "DeviceList.sca"

// --------------- Methodes -------------
#define SCCM_DeviceList_Generate		SCC_Dummy+133
struct SCCP_DeviceList_Generate { ULONG MethodID; struct ScalosNodeList *ScalosNodeList; };

#define SCCM_DeviceList_FreeDevNode	SCC_Dummy+134
struct SCCP_DeviceList_FreeDevNode { ULONG MethodID; APTR Node; };

#define SCCM_DeviceList_Filter		SCC_Dummy+135
struct SCCP_DeviceList_Filter { ULONG MethodID; struct ScalosNodeList *ScalosNodeList; };

// ------------------------------------------------------------------

struct ScaRootList {										// this is the struct of every Scalos
																// Root.sca subclass
	struct ScaWindowTask		*rl_WindowTask;		// Pointer to a WindowTask structure
	struct ScaInternInfos	*rl_internInfos;		// Pointer to a Internal Infos struct
};																// !! This is not the end of this struct


struct ScaInternInfos {
	struct MsgPort				*ii_MainMsgPort;		// Main message port
	struct ScaWindowStruct	*ii_MainWindowStruct;// Pointer into the windowlist
																// from Desktop Window
	struct ScaWindowStruct	*ii_AppWindowStruct;	// Pointer into the windowlist
																// from Window for AppIcons
	struct Screen				*ii_Screen;				// the Scalos Screen
	struct DrawInfo			*ii_DrawInfo;			// DrawInfo from this screen
	APTR							ii_visualinfo;			// Gadtools VisualInfo
};

struct ScaClassInfo {
	struct Hook					ci_Hook;					// Dispatcher Hook
	UWORD							ci_priority;			// Where to insert the class (-128 to 127)
	UWORD							ci_instsize;			// Instance size
	UWORD							ci_neededversion;		// needed Scalos Version (normaly
																	// this is 39)
	UWORD							ci_reserved;			// should be NULL
	STRPTR						ci_classname;			// name of your class (max 50 chars)
	STRPTR						ci_superclassname;	// name of the superclass (max 50 chars)
	STRPTR						ci_name;					// real name (max 100 chars)
	STRPTR						ci_description;		// short description (max 200 chars)
	STRPTR						ci_makername;			// name of the programmer
};

/* ----------------------- Scalos Class Hirachie -------------------------
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


----------------------------------------------------------------------- */


#endif	/* SCALOS_SCALOS_H */
