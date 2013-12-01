// scalos_structures.h
// $Date$
// $Revision$


#ifndef	SCALOS_STRUCTURES_H
#define	SCALOS_STRUCTURES_H


#include <graphics/gels.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include <graphics/gfx.h>
#include <dos/notify.h>
#include <libraries/gadtools.h>
#include "Semaphores.h"
#include <scalos/scalos.h>
#include <scalos/pattern.h>
#include <scalos/scalosprefs.h>
#include <scalos/scalosgfx.h>
#include <defs.h>

#include <stddef.h>

#include "FsAbstraction.h"
#include "BTree.h"
#include "int64.h"

#ifndef __AROS__
#define	BNULL		((BPTR) NULL)
#endif

#define	ICONTYPE_NONE		0

#define	MAX_ICON_UPDATE_COUNT	3		// maximum number of icons that will be updated immediately within 0.5s

#define FIBF_HIDDEN		(1<<7)		// 25.11.2004 Define Hidden protection bit

#define	MAX_FileName		120		// max. length for file names

#define	CLIpathSize		4096

#define	NO_ICON_POSITION_SHORT	((UWORD) 0x8000)

#define	NO_PEN			(-1)		// Failure code from ObtainBestPen

#define	ALPHA_OPAQUE		255

// Message Signature for AppMessage's, stored in am_Reserved[0]
#define	ID_APPM			MAKE_ID('A','P','P','M')

#define	Sizeof(array)		(sizeof(array) / sizeof((array)[0]))

//------------------------------- Rel Datas --------------------------------
#if 0
struct PatternInfo
	{
	struct	Hook ptinf_hook;		//Backfill Hook
	WORD	ptinf_width;			//Width
	WORD	ptinf_height;			//and Height of Bitmap in Pixels
	struct	BitMap *ptinf_bitmap;		//Bitmap or NULL
	APTR	ptinf_pattern;			//Pattern Array or NULL
	UWORD	ptinf_flags;			//see below
	WORD	ptinf_pad;			//padword
	struct	Message *ptinf_message;		//Message
	};
#endif

// bits in ptinf_flags ;
#define	PTINFB_FreeBitMap	0		// Flag: ptinf_bitmap has been created by CreatePictureBitMap and needs to be freed
#define	PTINFF_FreeBitMap	(1L << PTINFB_FreeBitMap)

//---------------------------------------------------------------

// Index values for iwt_WidthArray

enum WidthArrayIndex
	{
	WIDTHARRAY_NAME,
	WIDTHARRAY_SIZE,
	WIDTHARRAY_PROT,
	WIDTHARRAY_DATE,
	WIDTHARRAY_TIME,
	WIDTHARRAY_COMM,
	WIDTHARRAY_OWNER,
	WIDTHARRAY_GROUP,
	WIDTHARRAY_FILETYPE,
	WIDTHARRAY_VERSION,
	WIDTHARRAY_ICON,

	WIDTHARRAY_MAX
	};

//---------------------------------------------------------------

struct TTTextAttr
	{
	STRPTR ttta_FamilyTable[10];
	ULONG ttta_FontSize;
	ULONG ttta_FontWeight;
	ULONG ttta_FontStyle;

	char ttta_NameSpace[256];	// Room for ttta_FamilyTable entries
	};

struct TTFontFamily
	{
	APTR ttff_Normal;
	APTR ttff_Bold;
	APTR ttff_Italic;
	APTR ttff_BoldItalic;
	};

//-----------------------------------------------------------------------

struct ScalosMousePos
	{
	WORD X;
	WORD Y;
	};

//-----------------------------------------------------------------------

struct DropMarkInfo
	{
	struct Node dmi_Node;
	struct ScaIconNode *dmi_Icon;	// the icon this dropmark belongs to
	struct Rectangle dmi_Rectangle;	// dimensions of the dropmark
	ULONG dmi_Thickness;		// Thickness of dropzone frame
	struct BitMap *dmi_SaveBM[4];	// BitMaps for storage of saved area
	};

//---------------------------------------------------------------

// Icon layout
struct IconLayoutInfo
	{
	UBYTE ili_IconTypeLayoutModes[ICONTYPE_MAX];		// Layout mode per icon type
	struct Rect32 ili_LayoutArea;				// special icon layout area, or 0/0/0/0 for default
	struct Rect32 ili_Restricted[ICONLAYOUT_RESTRICTED_MAX];  // areas where no icons may be placed
	};

//-----------------------------------------------------------------------

// information collection for Lasso operation
struct LassoInfo
	{
	struct	MinList linf_IconList;			//list of all icons during lasso
	struct	MinList linf_WindowList;		//list of all Windows with locked IconSemaphores during lasso

	struct Rectangle linf_Limits;			//Limiting rectangle for Lasso
	struct	ScalosMousePos linf_OldPos;		//Old mousepos for lasso

	UBYTE	linf_SingleWindow;			//=1 for single-window Lasso mode
	};

//-----------------------------------------------------------------------

// Special vlaues in linf_OldPos
#define	LASSOX_FollowLasso	0x7000
#define	LASSOX_RemoveLasso	0x7001
#define	LASSOX_CrawlLasso	0x7002

//---------------------------------------------------------------

// Gadget IDs for scalos window gadgets

#define	GADGETID_ICONIFY	MAKE_ID(0,0,'F','Y')

#define	GADGETID_HORIZSLIDER	1
#define	GADGETID_VERTSLIDER	2
#define	GADGETID_RIGHTBUTTON	3
#define	GADGETID_LEFTBUTTON	4
#define	GADGETID_DOWNBUTTON	5
#define	GADGETID_UPBUTTON	6

#define	GADGETID_CONTROLBAR	1000
#define	GADGETID_STATUSBAR	1001

// virtual gadget IDs in iwt_MoveGadId

#define VGADGETID_IDLE		0
#define	VGADGETID_DRAGMOUSEMOVE	-1
#define	VGADGETID_LASSO		-2
#define	VGADGETID_MMBMOUSEMOVE	-3
#define	VGADGETID_WAITFORDRAG	-4		// LMB is pressed, waiting for relase or mouse move

//---------------------------------------------------------------

// Gadget IDs for control bar gadgets

#define	CBAR_GadgetID		2000

//---------------------------------------------------------------

// Gadget IDs for status bar gadgets

enum StatusBarGadgetID
	{
	SBAR_GadgetID_Text = 100,
	SBAR_GadgetID_PadLock,
	SBAR_GadgetID_Reading,
	SBAR_GadgetID_Typing,
	SBAR_GadgetID_ShowAll,
	SBAR_GadgetID_ThumbnailsAlways,
	SBAR_GadgetID_ThumbnailsAsDefault,
	SBAR_GadgetID_ThumbnailsGenerate,
	};

//-----------------------------------------------------------------------

// enum for contents of iwt_StatusBarMembers[STATUSBARGADGET_MAX]
enum StatusBarGadgetIndex
	{
	STATUSBARGADGET_Reading,                // Gadget for "reading" display
	STATUSBARGADGET_Typing,			// Gadget for "typing" display
	STATUSBARGADGET_StatusText,		// Gadget for state display
	STATUSBARGADGET_ShowAll,		// Gadget for "Show all" display
	STATUSBARGADGET_Thumbnails,		// Gadget for thumbnail mode display
	STATUSBARGADGET_ThumbnailsGenerate,	// Gadget for thumbnail generation display

	STATUSBARGADGET_MAX	// DO NOT MOVE -- MUST BE LAST ENTRY
	};

//-----------------------------------------------------------------------

struct WindowHistoryEntry
	{
	struct Node whe_Node;			// ln_Name points to whe_Path
	UWORD whe_ViewAll;             		// DDFLAGS_SHOWDEFAULT, DDFLAGS_SHOWICONS, or DDFLAGS_SHOWALL
	UBYTE whe_Viewmodes;			// SCAV_ViewModes_***
	WORD whe_XOffset;			// Inner window X offset
	WORD whe_YOffset;			// Inner window Y offset
	enum ScanDirResult whe_ScanDirResult;	// Result from directory scan
	struct ScaIconNode *whe_IconList;	// Pointer to the first IconNode
	WORD whe_WidthArray[WIDTHARRAY_MAX];	//Pixel width array for text mode columns
	char whe_Path[1];			// variable length
	};

//-----------------------------------------------------------------------

//Values in iwt_ObjectUnderMouse
enum ObjTypes { OUM_nothing, OUM_ScalosWindow, OUM_ForeignWindow, OUM_AppWindow, OUM_Icon };


// struct ScaWindowTask
struct internalScaWindowTask
	{
	struct ScaWindowTask iwt_WindowTask;		//public part of the structure

//----- private data -----

	struct	AppObject *iwt_AppList;			//AppObject List
	SCALOSSEMAPHORE *iwt_AppListSemaphore;		//AppObjectList Semaphore

	Object	*iwt_DevListObject;			//DeviceList object

	ULONG	iwt_IDCMPFlags;				//IDCMP-Flags

	struct	Screen *iwt_WinScreen;			//Window's Screen

	struct	DrawInfo *iwt_WinDrawInfo;		//Window's Drawinfo

	WORD	iwt_InvisibleWidth;			//Width of invisible part of Window contents
	WORD	iwt_InvisibleHeight;			//Height of invisible part of Window contents
	WORD	iwt_MinX;				//sub value for xoffset
	WORD	iwt_MinY;				//sub value for yoffset
	WORD	iwt_InnerLeft;				//inner Window offsets (ScrollArea)
	WORD	iwt_InnerTop;
	WORD	iwt_InnerRight;
	WORD	iwt_InnerBottom;
	WORD	iwt_InnerWidth;
	WORD	iwt_InnerHeight;

	WORD	iwt_GadgetBarHeightTop;			// Extra room on top for gadget bar
	WORD	iwt_GadgetBarHeightBottom;		// Extra room on bottom for gadget bar
	WORD	iwt_TextWindowGadgetHeight;		// Extra room on top for text window gadgets

	struct	MsgPort *iwt_OldFileSys;		//remember FileSysTask on window task startup

	struct	Gadget *iwt_Gadgets;			//WindowGadgets

	Object	*iwt_LastIcon;				//last clicked icon

	T_TIMEVAL iwt_LastTime;				//time of last click
	struct  ScalosMousePos iwt_LastPos;		//last icon position

	struct	DragNode *iwt_DragNodeList;		//List for Drag&Drop

	struct	ScaIconNode *iwt_DragIconList;		//List of removed icons from iconlist

	struct	ScalosMousePos iwt_OldMouse;		//Old Mouseposition

	struct	Image *iwt_GadImageRightArrow;		//ArrowGadget Images
	struct	Image *iwt_GadImageLeftArrow;
	struct	Image *iwt_GadImageDownArrow;
	struct	Image *iwt_GadImageUpArrow;
	struct	Image *iwt_IconifyImage;		//Iconify Gadget's Image

	struct	Gadget *iwt_PropBottom;			//Bottom Propgadget
	struct	Gadget *iwt_PropSide;			//Side Propgadget
	struct	Gadget *iwt_GadgetRightArrow;		//Arrow Gadgets
	struct	Gadget *iwt_GadgetLeftArrow;		//Arrow Gadgets
	struct	Gadget *iwt_GadgetDownArrow;		//Arrow Gadgets
	struct	Gadget *iwt_GadgetUpArrow;		//Arrow Gadgets
	struct	Gadget *iwt_IconifyGadget;		//Iconify Gadget

	struct	Gadget *iwt_PadLockGadget;		//Padlock Gadget for status bar

	Object	*iwt_myAppIcon;				//IconObject for iconified window AppIcon

	struct	AppObject *iwt_myAppObj;		//Iconify AppObject

	struct	Region *iwt_RemRegion;			//ClipRegion generated by SCCM_Window_InitClipRegion
	struct 	Region *iwt_DamagedRegion;		// If IDCMP_REFRESHWINDOW could not be processed, store the damaged region here

	T_TIMEVAL iwt_LockCount;			//IDCMP_IntuiTicks - Currenttime

	STRPTR	iwt_WinTitle;				//Window Title-String

	Object	*iwt_TitleObject;			//Scalos Title Object

	struct	TextFont *iwt_IconFont;			//Current Font

	struct	DragHandle *iwt_myDragHandle;		//DragHandle for Drag&Drop

	struct	ScaDeviceIcon *iwt_DevIconList;		//Current DeviceIconList

	UWORD	iwt_NotifyCounter;			//Counter for delayed update after NotifyMessages
							//every time an Notify msg is received, iwt_NotifyCounter
							//is reset to a timeout value.
							//During IconWin_Ping, iwt_NotifyCounter is decremented and
							//IconWinCheckUpdate() is called when counter reaches 0.

	WORD	iwt_MoveGadId;				//Mousemove Gadgets ID

	WORD	iwt_WidthArray[WIDTHARRAY_MAX];		//Pixel width array for text mode columns

	UBYTE	iwt_WinLockCount;			//WindowLock Counter

	UBYTE	iwt_BackDrop;				//Flag: Backdrop Window ?
	UBYTE	iwt_LassoFlag;				//Flag: Lasso on?
	UBYTE	iwt_CheckOverlappingIcons;		//Flag: always check for overlapping icons, and reposition conflicting icons.
	UBYTE	iwt_OldShowType;		
	UBYTE	iwt_LockFlag;				//is set if the layers are locked
	UBYTE	iwt_tRefreshCount;			//second - counter for the title-refresh
	UBYTE	iwt_wRefreshCount;			//second - counter for the window title-refresh
	UBYTE	iwt_dRefreshCount;			//second - counter for the diskicon-refresh
	UBYTE	iwt_NeedsTimerFlag;			//remember if you need the timer msgs

	UBYTE	iwt_Reading;				//the wintask is reading icons
							// DO NOT ACCESS DIRECTLY, USE OM_SET/OM_GET SCCA_IconWin_Reading

	UBYTE	iwt_IconifyFlag;			//currently in iconify state ?
	UBYTE	iwt_RemIconsFlag;			//icons removed while D&D
	UBYTE	iwt_TextGadFlag;			//Draw TextColumn Gadgets
	UBYTE	iwt_DragMayDrop;			//while Dragging: Icon(s) may be dropped here +jl+
	UBYTE	iwt_IconShadowVisible;			//icon shadow visible while D&D +jl+
	UBYTE	iwt_HoldUpdate;				//Flag: no visible updates (text window only) +jl+ 20010320
	UBYTE	iwt_CloseWindow;			//Flag: close window
	UBYTE	iwt_AsyncLayoutPending;			//Flag: Async window backfill is under progress.
	UBYTE	iwt_AbortScanDir;			//Flag: abort pending scandir

	UBYTE	iwt_Typing;				//Flag: currently typing, TypeRestartTime is running +jl+ 20011005
							// DO NOT ACCESS DIRECTLY, USE OM_SET/OM_GET SCCA_IconWin_Typing

	UBYTE	iwt_ReadOnly;				//Flag: drawer is Read-Only +jl+ 20011029
	UBYTE	iwt_IconListDDLocked;			//Flag: wt_IconSemaphore locked during D&D

	UWORD	iwt_CheckUpdateCount;			// Counter to protect against recursive call of IconWindowCheckUpdate

	struct	internalScaWindowTask *iwt_WinUnderPtr;	//ScaWindowTask under pointer during dragging +jl+
	struct	ScaIconNode *iwt_IconUnderPtr;		//ScaIconNode under Pointer during dragging +jl+
	struct	ScaIconNode *iwt_IconOuterUBUnderPtr;	//Pointer over outer bounds of this icon during D&D +jl+ 20010912

	struct 	LassoInfo iwt_LassoInfo;

	ULONG	iwt_DragTranspMode;			//transparency flags for DrawDrag() during dragging +jl+
	enum 	ObjTypes iwt_ObjectUnderMouse;		//Type of object under mouse during dragging +jl+
	LONG	iwt_SameLock;				//SameLock(source, dest) result while D&D +jl+
	ULONG	iwt_DragFlags;				//various dragging flags +jl+

	ULONG	iwt_IconPortOutstanding;		// count of outstanding wt_IconPort reply messages +jl+ 20010928

	WORD	iwt_StartDragMouseX;			//Mouse X position at drag start +jl+ 20010403
	WORD	iwt_StartDragMouseY;			//Mouse X position at drag start

	struct	DateStamp iwt_LastIconUpdateTime;	//Date+Time of last icon read/update +jl+ 20010323
	ULONG	iwt_IconUpdateCount;			//Counter for number of icon updates within a certain time interval

	ULONG	iwt_IconListLockedExclusive;		//Counter: incremented everytime iwt_WindowProcess exclusively locks own icon list
	ULONG	iwt_IconListLockedShared;		//Counter: incremented everytime iwt_WindowProcess locks own icon list

	SCALOSSEMAPHORE	iwt_UpdateSemaphore;		//Semaphore to prevent nesting of window update +jl+ 20010328
	SCALOSSEMAPHORE iwt_ChildProcessSemaphore;	//Semaphore to prevent closing of window until all child processes have ended

	struct	InputEvent iwt_LastInputEvent;		//last InputEvent, for RawKeyConvert +jl+ 20011005

	char	iwt_TypingBuffer[40];			//typing buffer for keyboard icon selection +jl+ 20011005
	ULONG	iwt_TypeRestartTimer;			//Counter for TypeRestartTime +jl+ 20011005

	WORD	iwt_ExtraWidth;				//amount of pixels to add to window width on OpenWindow()
	WORD	iwt_ExtraHeight;			//amount of pixels to add to window height on OpenWindow()

	struct	Screen *iwt_OrigWinScreen;		//this Screen was used to layout icons +jl+ 20011207

	struct	Gadget *iwt_StatusBar;			//Status bar object +jl+ 20011030
	struct	Gadget *iwt_ControlBar;			//Control bar object

	struct List iwt_ControlBarMemberList;		//List of ControlBar members

	struct 	ExtGadget *iwt_HighlightedControlBarGadget;	// this control bar Gadget is currently highlighted, NULL if none.

	struct	Gadget *iwt_StatusBarMembers[STATUSBARGADGET_MAX];	// Status bar member gadgets

	struct	msg_ShowPopupMenu *iwt_PopupMenuPending;	//contains message when popup menu is pending +jl+ 20011209

	struct	Process *iwt_WindowProcess;		//Scalos process for this window +jl+ 20020412

	SCALOSSEMAPHORE iwt_ScanDirSemaphore;		//Semaphore, locked during ReadIconList()

	WORD	iwt_DragScrollX;			// Value to scroll horizontally while dragging
	WORD	iwt_DragScrollY;                        // Value to scroll vertically while dragging

	LONG	iwt_BottomScrollerScale;		// shift factor for bottom scroller Top/Visible/Total values
	LONG	iwt_SideScrollerScale;			// shift factor for bottom scroller Top/Visible/Total values

	UWORD   iwt_TextWindowLineHeight;		// Height of text window line

	struct TTFontFamily iwt_IconTTFont;		//Current icon TTengine Font

	struct	Hook iwt_ColumnWidthChangeHook;		//This hook gets called when a text icon layout causes iwt_WidthArray to change

	struct List iwt_IconDropMarkInfoList;
	struct DropMarkInfo iwt_WindowDropMarkInfo;

	struct	List iwt_ThumbnailIconList;		//List of icons where thumbnail creation is pending
	SCALOSSEMAPHORE iwt_ThumbnailIconSemaphore;	//Semaphore for protection of iwt_ThumbnailIconList
	BOOL	iwt_ThumbnailGenerationPending;

	ULONG	iwt_ThumbnailMode;			//display mode for thumbnails: THUMBNAILS_Never, THUMBNAILS_Always, THUMBNAILS_AsDefault
	ULONG	iwt_ThumbnailsLifetimeDays;		//Maximum lifetime for generated thumbnails

	SCALOSSEMAPHORE iwt_ThumbGenerateSemaphore;	//Semaphore to ensure single-threaded thumbnail generation

	struct IconLayoutInfo iwt_IconLayoutInfo;	//Information about how icons are to be placed

	struct DatatypesImage *iwt_IconOverlays[ICONOVERLAY_MAX];      // DatatypesImage's for icon overlays

	SCALOSSEMAPHORE iwt_WindowHistoryListSemaphore;	// Semaphore for protection of iwt_HistoryList
	struct List iwt_HistoryList;			//List of recent window paths
	struct WindowHistoryEntry *iwt_CurrentHistoryEntry;	// Pointer to "current" entry in iwt_HistoryList

	struct FileRequester *iwt_AslFileRequest;	// ASL file requester, e.g. for SCCM_IconWin_Browse

	struct List iwt_PopChildList;			// List for ScaPopChildWindow structures;
	SCALOSSEMAPHORE iwt_PopChildListSemaphore;	// Semaphore for protection of iwt_PopChildList

	struct Rect32 iwt_IconBBox;			// Bounding Box of all icons

	WORD	iwt_LastUnCleanupInnerWidth;
	WORD	iwt_LastUnCleanupInnerHeight;

	struct EClockVal iwt_TimeStamps[20];
        };

//-----------------------------------------------------------------------

// Values in iwt_DragFlags
#define	DRAGFLAGB_WindowMarked		0		// Window Drop mark is visible
#define	DRAGFLAGF_WindowMarked		(1 << DRAGFLAGB_WindowMarked)
#define	DRAGFLAGB_WindowLocked		1		// Window update is locked
#define	DRAGFLAGF_WindowLocked		(1 << DRAGFLAGB_WindowLocked)
#define	DRAGFLAGB_UpdatePending		2		// delayed window update is pending
#define	DRAGFLAGF_UpdatePending		(1 << DRAGFLAGB_UpdatePending)

//-----------------------------------------------------------------------

struct MainTask
	{
	struct	internalScaWindowTask mwt;	// +jl+ 20010928
						// dummy, only iwt_CloseWindow is currently being used.
#if commercial
	ULONG	keykenn;			//Key kennung #$029A5956
#endif
	T_TIMEREQUEST *iorequest;		//TimerIO Request

	struct	NotifyNode *mainnotifylist;
	SCALOSSEMAPHORE	mwt_MainNotifyListSema;

	ULONG	miwt_LastGadgetUnderPtr;		//GadgetID of gadget under Pointer (for ClassWinTimerMsg())
	struct	ScaIconNode *miwt_LastIconUnderPtr;	// last icon under pointer for ClassWinTimerMsg()
	LONG	miwt_IconUnderPtrCount;			// counter for icon tooltip delay

	UBYTE	emulation;			//Emulation Mode ?
	UBYTE	screenbackfill;			//ScreenBackfill on ?
	UBYTE	restartflag;			//Set if Maintask will be restarted
	UBYTE	sleepflag;			//is the whole Scalos sleeping

	ULONG	mt_PrefsChangedFlags;		// Flags for changed preferences settings,
						// see SM_NewPreferences
	ULONG	mt_PrefsChangedTimer;

	ULONG	mt_TimerCounter;

	STRPTR	mt_ChangedMenuPrefsName;
	STRPTR	mt_ChangedPalettePrefsName;
	};

//-----------------------------------------------------------------------

struct DragNode
	{
	struct	MinNode drgn_Node;
	struct	ScaIconNode *drgn_iconnode;
	Object *drgn_icon;
	WORD	drgn_x;				// x offset
	WORD	drgn_y;				// y offset
	WORD	drgn_DeltaX;			// x offset from mouse position
	WORD	drgn_DeltaY;			// y offset from mouse position
	};

//-----------------------------------------------------------------------

struct PatternNode
	{
	struct	MinNode ptn_Node;
	STRPTR	ptn_filename;			//Pattern path
	struct	BitMap *ptn_bitmap;		//Bitmap or NULL
	WORD	ptn_width;			//Width
	WORD	ptn_height;			//Height
	Object	*ptn_object;			//Datatypes object
	SCALOSSEMAPHORE *ptn_semaphore;		//Reading semaphore
	APTR	ptn_picture;			//guigfx picture object
	APTR	ptn_colorhandle;		//guigfx colortable
	APTR	ptn_drawhandle;			//guigfx drawhandle
	WORD	ptn_weight;			//ColorWeight
	UWORD	ptn_flags;			//Flags (see below)
	UWORD	ptn_number;			//Number of Pattern
	UWORD	ptn_type;			//Type to show - see enum ScpRenderType
	UWORD	ptn_BgMode;			//Type of background -- see enum
	UWORD	ptn_dithermode;			//Renderlib Dithermode
	UWORD	ptn_ditheramount;		//Dither amount
	UBYTE	ptn_prec;			//Precision
	UBYTE	ptn_BgColor1[3];
	UBYTE	ptn_BgColor2[3];
	};

// Values in ptn_flags :
#define	PTNB_NoRemap		0
#define	PTNB_Enhanced		1
#define	PTNB_AutoDither		2
#define	PTNB_FreeBitmap		3
#define	PTNB_Ready		4
#define	PTNF_NoRemap		(1L << PTNB_NoRemap)
#define	PTNF_Enhanced		(1L << PTNB_Enhanced)
#define	PTNF_AutoDither		(1L << PTNB_AutoDither)
#define	PTNF_FreeBitmap		(1L << PTNB_FreeBitmap)
#define	PTNF_Ready		(1L << PTNB_Ready)

//-----------------------------------------------------------------------

struct ScalosArg
	{
	struct	MinNode scarg_Node;
	BPTR	scarg_lock;			//Lock of Directory
	STRPTR	scarg_name;			//Filename or NULL for dirs
	STRPTR	scarg_ext;			//Icon Extention or NULL
	WORD	scarg_xpos;			//Icon Position
	WORD	scarg_ypos;
	WORD	scarg_DeltaX;			// x offset from mouse position
	WORD	scarg_DeltaY;			// y offset from mouse position
	WORD	scarg_icontype;			//Type of the original icon
	};

//-----------------------------------------------------------------------

struct MenuInfo
	{
	struct	MinNode minf_Node;
	UWORD	minf_type;			//Type of this item
	struct	MenuItem *minf_item;		//Address of MenuItem
	ULONG	minf_number;			//Coded MenuNumber
	struct	ScalosMenuCommand *minf_MenuCmd;	// Backpointer to ScalosMenuCommand +jl+ 20011215
	};

//types;
#define	MITYPE_NOTHING		0

#define	MITYPEB_ViewByDefault	0		// check MenuItem if "View By Default"
#define	MITYPEB_ViewByIcon	1		// check MenuItem if "View By Icon"
#define	MITYPEB_ViewByText	2		// check MenuItem if "View By Text"
#define	MITYPEB_ViewByDate	3		// check MenuItem if "View By Text"
#define	MITYPEB_ViewBySize	4		// check MenuItem if "View By Text"
#define	MITYPEB_ViewByType	5		// check MenuItem if "View By Text"
#define	MITYPEB_ShowOnlyIcons	6		// check MenuItem if "Show Only Icons"
#define	MITYPEB_ShowAllFiles	7		// check MenuItem if "Show All Files"
#define MITYPEB_ShowDefault	8               // check MenuItem if "Show Only Icons"
#define	MITYPEB_Backdrop	9

#define	MITYPEF_ViewByDefault	(1L << MITYPEB_ViewByDefault)
#define	MITYPEF_ViewByIcon	(1L << MITYPEB_ViewByIcon)
#define	MITYPEF_ViewByText	(1L << MITYPEB_ViewByText)
#define	MITYPEF_ViewByDate	(1L << MITYPEB_ViewByDate)
#define	MITYPEF_ViewBySize	(1L << MITYPEB_ViewBySize)
#define	MITYPEF_ViewByType	(1L << MITYPEB_ViewByType)
#define	MITYPEF_ShowOnlyIcons	(1L << MITYPEB_ShowOnlyIcons)
#define	MITYPEF_ShowAllFiles	(1L << MITYPEB_ShowAllFiles)
#define	MITYPEF_ShowDefault	(1L << MITYPEB_ShowDefault)
#define	MITYPEF_Backdrop	(1L << MITYPEB_Backdrop)

#define	MITYPEF_all		((UWORD) ~0)

//-----------------------------------------------------------------------

// argument structure for all those ...Prog() routines
// e.g. NewDrawerProg(), RenameProg(), DeleteProg(), ...
struct MenuCmdArg
	{
	CONST_STRPTR mca_CommandName;
	struct	ScaIconNode *mca_IconNode;
	ULONG	mca_State;
	};


// interface for all menu command routines
// see MenuCommandTable[]
typedef void (*MENUFUNC)(struct internalScaWindowTask *, const struct MenuCmdArg *);
typedef BOOL (*MENUENABLEFUNC)(struct internalScaWindowTask *, struct ScaIconNode *);

struct ScalosMenuCommand
	{
	CONST_STRPTR smcom_CommandName;
	ULONG smcom_Flags;
	ULONG smcom_ParseFlags;		// lower UWORD goes into MenuInfo minf_type
	MENUFUNC smcom_Routine;
	MENUENABLEFUNC smcom_Enable;
	};

// definitions for smcom_Flags :
#define	SMCOMFLAGB_RunFromRootWindow	0	// always run menu command from root window context
#define	SMCOMFLAGF_RunFromRootWindow	(1L << SMCOMFLAGB_RunFromRootWindow)

//---------------------------------------------------------------

#define	USERDATA_TOOLSMENU	((APTR) ~0)	// the Tools menu is identified by this UserData


#define	ID_SC	MAKE_ID(0,0,'S','C')
#define	ID_SAMI	MAKE_ID('S','A','M','I')

struct AppMenuInfo
	{
	struct Node ami_Node;
	ULONG ami_Magic;		// ID_SAMI
	struct NewMenu ami_NewMenu;

	struct AppMenuInfo *ami_Parent;

	struct List ami_ChildList;
	};

struct AppObject
	{
	struct	MinNode appo_Node;
	UWORD	appo_Kennung;				// Kennung ("SC")
	UWORD	appo_type;
	ULONG	appo_id;
	ULONG	appo_userdata;
	struct	MsgPort *appo_msgport;
	union	{
		Object *appoo_IconObject;		// for APPTYPE_AppIcon
		struct Window *appoo_Window;		// for APPTYPE_AppWindow
		struct AppMenuInfo *appoo_MenuInfo;	// for APPTYPE_AppMenuItem
		} appo_object;				// diffs with types
	APTR	appo_userdata2;				// special userdata
	struct	TagItem *appo_TagList;			// MC0001 - Taglist from AddAppIcon() (extension by jl)
	ULONG	appo_flags;				// MC0001 - flags (extension by jl)
	};

// values in appo_type :
enum AppObjTypes { APPTYPE_AppIcon, APPTYPE_AppWindow, APPTYPE_AppMenuItem };

//-----------------------------------------------------------------------

struct DoWait
	{
	struct	MinNode dwai_Node;
	struct	MsgPort *dwai_msgport;		//MessagePort
	struct	WblMessage *dwai_wblmsg;	//WBL-Message
	struct	WBStartup *dwai_wbmsg;		//WBStart Message for Program
	};

// This message is sent back to WB loader when the called program 
// replies its WBStartup (= program finished).
struct DoWaitFinished
	{
	struct	Message dwf_Message;
	struct	DoWait *dwf_DoWait;
	};

//-----------------------------------------------------------------------

struct ScalosClass
	{
	struct	MinNode sccl_Node;
	STRPTR	sccl_name;			//ClassName
	Class	*sccl_class;			//Class Pointer
	struct	PluginClass *sccl_plugin;	//Pointer to PlugIn struct
#ifdef __MORPHOS__
	struct EmulLibEntry sccl_EmulEntry;
#endif

	};

struct PluginClass
	{
	struct	MinNode plug_Node;
	WORD	plug_priority;
	UWORD	plug_instsize;
	CONST_STRPTR	plug_classname;
	CONST_STRPTR	plug_superclassname;
	HOOKFUNC	plug_classhook;		//DispFunc or NULL for diskplugins
	APTR	plug_base;			//BaseAdr from DiskPlugins
#ifdef __amigaos4__
	APTR    plug_iface;         		//Interface from DiskPlugins
#endif
	STRPTR	plug_filename;			//DiskPlugin filename
	struct	ScalosClass * (*plug_ClassInit)(const struct PluginClass *);	// Class init function or NULL
	struct	ScalosClass *plug_class;	//Scalos Class Struct
	};


struct WindowConfig
	{
	UWORD	cfig_flags;
	UBYTE	cfig_viewmodes;
	};

//-----------------------------------------------------------------------

// DragHandle1
// for use with system bob routines
struct DragHandle1
	{
	struct	GelsInfo *drgh1_oldginfo;
	struct	GelsInfo drgh1_gelsinfo;
	struct	VSprite drgh1_vshead;		//head VSprite
	struct	VSprite drgh1_vstail;		//tail VSprite
	};

// DragHandle2
// for use with custom bob routines
struct DragHandle2
	{
	struct	BitMap *drgh2_speedbitmap;	// Speedup Bitmap
	struct	BitMap *drgh2_bufferbitmap;	// Bitmap for Mask Blits
	struct	BitMap *drgh2_bufferbitmap2;	// Bitmap for big offset moving
	struct	ARGB *drgh2_transbuffer;	// Buffer for Real Transparency
	struct	ARGB *drgh2_transbuffer2;	// Buffer for Real Transparency
	struct	Window *drgh2_FgWindow;		// Foreground window
	struct	RastPort drgh2_rastport;
	};

struct DragHandle
	{
	struct	Screen *drgh_screen;
	struct	ScaBob *drgh_boblist;			// List of attached ScaBob's
	UWORD	drgh_flags;

	SCALOSSEMAPHORE drgh_BobListSemaphore;		// +jl+ 20010316 protection for drgh_boblist

							// added by +jl+ 20010224
	struct	ScaIconNode *drgh_SpecialIconList;	// List of special icons (forbidden, copy, ... indicator)
	struct	DragNode *drgh_SpecialDragList;		// DragNode's for special icons
	struct	ScaBob *drgh_SpecialBobList;		// 
	struct	ScaBob *drgh_Bob_NoDropIndicator;	// Bob for No-Drop-here indicator
	struct	ScaBob *drgh_Bob_CopyIndicator;		// Bob for Copy indicator
	struct	ScaBob *drgh_Bob_MakeLinkIndicator;	// Bob for Make-Link indicator
	struct	ScaBob *drgh_Bob_ForceMoveIndicator;	// Bob for forced move indicator

	struct	internalScaWindowTask *drgh_iwt;

	ULONG	drgh_LastDragFlags;			// +jl+ 20010515 Flags on last call to SCA_DrawDrag()

	ULONG	drgh_DrawerCount;			// +jl+ 20010813 number of drawers being dragged
	ULONG	drgh_FileCount;				// +jl+ 20010813 number of files being dragged
	ULONG	drgh_DeviceCount;			// +jl+ 20011112 number of devices being dragged

	struct 	ScaIconNode *drgh_PopOpenIcon;
	ULONG 	drgh_PopOpenTickCount;			   // IntuiTicks Counter
	struct 	internalScaWindowTask *drgh_PopOpenDestWindow;

	union
		{
		struct DragHandle1 drghs_DragHandle1;
		struct DragHandle2 drghs_DragHandle2;
		} drgh_Specific;
	};
	
//Flags: bit
#define	DRGHB_LayersLocked		0
#define	DRGHF_LayersLocked		(1 << DRGHB_LayersLocked)
#define	DRGHB_CustomBob			1
#define	DRGHF_CustomBob			(1 << DRGHB_CustomBob)
#define	DRGHB_SpeedBitMapAlloc		2
#define	DRGHF_SpeedBitMapAlloc		(1 << DRGHB_SpeedBitMapAlloc)
#define	DRGHB_BlitRealTransp		3
#define	DRGHF_BlitRealTransp		(1 << DRGHB_BlitRealTransp)
#define	DRGHB_RealTranspPossible	4
#define	DRGHF_RealTranspPossible	(1 << DRGHB_RealTranspPossible)
#define	DRGHB_BobsOptimized		5
#define	DRGHF_BobsOptimized		(1 << DRGHB_BobsOptimized)
#define	DRGHB_NoDropIndicatorVisible	6
#define	DRGHF_NoDropIndicatorVisible	(1 << DRGHB_NoDropIndicatorVisible)
#define	DRGHB_CopyIndicatorVisible	7
#define	DRGHF_CopyIndicatorVisible	(1 << DRGHB_CopyIndicatorVisible)
#define	DRGHB_MakeLinkIndicatorVisible	8
#define	DRGHF_MakeLinkIndicatorVisible	(1 << DRGHB_MakeLinkIndicatorVisible)
#define	DRGB_BobsHidden			9
#define	DRGF_BobsHidden			(1 << DRGB_BobsHidden)
#define	DRGB_DragStarted		10
#define	DRGF_DragStarted		(1 << DRGB_DragStarted)
#define	DRGHB_ForceMoveIndicatorVisible	11
#define	DRGHF_ForceMoveIndicatorVisible	(1 << DRGHB_ForceMoveIndicatorVisible)
#define	DRGHB_LockSuspended 		12
#define DRGHF_LockSuspended		(1 << DRGHB_LockSuspended)

// 0 = Layers Locked ?
// 1 = Custom Bob
// 2 = SpeedBitmap allocated ? (Custom Bob only)
// 3 = Blit in real transparents
// 4 = Is real transparents possible ?
// 5 = Bobs are optimized
// 6 = No-Drop-Here indicator visible +jl+ 20010224
// 7 = Copy indicator visible +jl+ 20010224
// 8 = Make-Link indicator visible +jl+ 20010224
// 9 = Bobs are currently hidden +jl+ 20010226

//-----------------------------------------------------------------------

// check equality of 2 ARGB variables
#define	ARGB_EQ(a, b)	(*((ULONG *) &(a)) == *((ULONG *) &(b)))

//-----------------------------------------------------------------------

struct ScaBob1
	{
	APTR	scbob1_shadow1;			//Normal Mask
	APTR	scbob1_shadow2;			//Transparent Mask

	struct	VSprite scbob1_vsprite;
	struct	Bob scbob1_bob;			//bob-structure
	};

struct ScaBob2
	{
	WORD	scbob2_xpos;			//absolute position
	WORD	scbob2_ypos;

	WORD	scbob2_lastxpos;		//last x,y position
	WORD	scbob2_lastypos;

	const UBYTE *scbob2_AlphaChannel;	//Alpha channel bytes for icon, or NULL

	struct	BitMap *scbob2_bitmap;		//Bob's Bitmap
	struct	BitMap *scbob2_backbitmap;	//Bob's Background
	struct	BitMap *scbob2_backbitmap2;	//Bob's Background Doublebuffer

	struct	BitMap *scbob2_shadow;		//Current Mask
	struct	BitMap *scbob2_shadow1;		//Normal Mask
	struct	BitMap *scbob2_shadow2;		//Transparent Mask
	};

struct ScaBob
	{
	struct	MinNode scbob_Node;
	struct	DragHandle *scbob_draghandle;	//backlink to draghandle

	WORD	scbob_x;
	WORD	scbob_y;			//distance from move-point

	WORD	scbob_width;
	WORD	scbob_height;			//bob dimensions

	WORD	scbob_BoundsWidth;
	WORD	scbob_BoundsHeight;		//maximum bob dimensions (including icon text!)

	WORD	scbob_LeftBorder;		//Distance between left and boundsleft

	ULONG	scbob_Flags;			// +jl+ 20010314

	union	{
		struct ScaBob1 scbob_Bob1;
		struct ScaBob2 scbob_Bob2;
		} scbob_Bob;
	};

// Values in scbob_Flags and scbob2_Flags
#define	BOBFLAGB_NeverTransparent	0
#define	BOBFLAGF_NeverTransparent	(1 << BOBFLAGB_NeverTransparent)
#define	BOBFLAGB_NoOptimize		1
#define	BOBFLAGF_NoOptimize		(1 << BOBFLAGB_NoOptimize)
#define	BOBFLAGB_FreeAlphaChannel	2
#define	BOBFLAGF_FreeAlphaChannel	(1 << BOBFLAGB_FreeAlphaChannel)

//-----------------------------------------------------------------------
// Drag/Drop support
//-----------------------------------------------------------------------

struct DropOps
	{
	int drop_Copy:1;
	int drop_Move:1;
	int drop_CreateLink:1;
	int drop_PutAway:1;
	int drop_LeaveOut:1;
	};

enum DropMenuResult
	{
	DROPMENURESULT_Unknown = 0,
	DROPMENURESULT_Copy = 1,
	DROPMENURESULT_Move,
	DROPMENURESULT_CreateLink,
	DROPMENURESULT_Cancel = 99,
	};

struct DropInfo
	{
	struct ScalosArg *drin_Arg;
	BPTR drin_DestLock;
	Object *drin_FileTransObj;
	LONG drin_x;
	LONG drin_y;
	struct DropOps drin_Drops;
	enum DropMenuResult drin_LastDropMenuResult;
	struct DropOps drin_LastAllowedDrops;
	};

// Drop Function called by DropTask()
typedef void (*DROPFUNC)(struct ScalosArg **ArgList, 
	struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest, 
	struct DropInfo *DrInfo);

struct SM_DropProc
	{
	struct ScalosMessage sdpm_ScalosMessage;
	DROPFUNC sdpm_DropFunc;
	struct ScaWindowStruct *sdpm_SourceWindowStruct;
	struct ScaWindowStruct *sdpm_DestWindowStruct;
	struct ScalosArg *sdpm_ArgList;
	WORD sdpm_MouseX;
	WORD sdpm_MouseY;
	struct DropOps sdpm_Drops;
	};

//---------------------------------------------------------------

// ID for PopupMenu titles +jl+
#define	PM_TITLE_ID	9999


// +jl+ 20010428
struct IconMenuSupports
	{
	CONST_STRPTR ims_CommandName;
	ULONG ims_FlagMask;
	};

// +jl+ 20010621 moved from Scalos.c
struct ChainedLine
	{
	struct Node cnl_Node;
	STRPTR cnl_Line;
	LONG cnl_PosX;
	LONG cnl_PosY;
	};

struct BackDropList
	{
	struct List bdl_LinesList;
	ULONG bdl_Changed;
	ULONG bdl_Loaded;
	ULONG bdl_Filtered;
	ULONG bdl_NotPresent;
	BOOL bdl_Initialized;
	};

// +jl+ 20010807
struct ScaPluginPrefs
	{
	WORD spp_Priority;		// Plugin class priority
	UWORD spp_InstSize;		// Plugin class instance data size
	UWORD spp_MinVersion;		// Minimum Scalos version for plugin
	UWORD spp_Dummy;
	char spp_Names[1];		// variable length, null-terminated strings :
					// class file name, Class name, superclass name, 
	};

//---------------------------------------------------------------

// return codes for copy/move function when user aborted
#define	RESULT_UserAborted	RETURN_WARN

//---------------------------------------------------------------

#define	NOT_A_LOCK		((BPTR) ~0)
#define	IS_VALID_LOCK(x)	((x) != NOT_A_LOCK)

//---------------------------------------------------------------

// Flags for SetBackFill()

#define	SETBACKFILLB_NOASYNC	0
#define	SETBACKFILLF_NOASYNC	(1L << SETBACKFILLB_NOASYNC)

//---------------------------------------------------------------

struct ReadIconListData
	{
	ULONG rild_IconType;			// Workbench icon type (e.g. WBDISK)
	const struct TypeNode *rild_TypeNode;	// TypeNode * or Workbench icon type (e.g. WBDISK)
	LONG rild_Type;				// same as fib_DirEntryType
	LONG rild_Protection;			// same as fib_Protection
	ULONG64 rild_Size64;
	ULONG rild_OwnerUID;                    // Owner info
	ULONG rild_OwnerGID;			// Group info
	struct DateStamp rild_DateStamp;
	BOOL rild_DiskWriteProtected;		// Flag: disk/volume is not writeable
	BOOL rild_CheckOverlap;			// Flag: check for overlapping icons
	BOOL rild_FetchIconType;		// TRUE if icon type sould be read (requires extra reading via NewIconObject())
	BOOL rild_SoloIcon;			// this is an icon w/o associated object
	char rild_Name[MAX_FileName];
	char rild_Comment[80];			// size equal to fib_Comment
	};

struct ReadIconListControl
	{
	struct MsgPort *rilc_replyPort;		// Reply Port for packets
	struct MsgPort *rilc_FileSysPort;	// file system packet message port
	struct DosPacket *rilc_pkt;		// DosPacket
	struct ExAllControl *rilc_exallControl;
	APTR rilc_ExAllBuffer;
	BPTR rilc_dirLock;
	ULONG rilc_ExNextOk;
	BOOL rilc_FlagFinished;			// Flag: ExAllData contains last entries
	BOOL rilc_FlagLargeFileSupported;	// Flag: file system supports large files (> 32 bit)
	struct ExAllData *rilc_edNext;
	T_ExamineData *rilc_exd;
	T_ExamineDirHandle rilc_DirHandle;
	struct FileInfoBlock *rilc_Fib;		// for fallback ACTION_EXAMINE_OBJECT / ACTION_EXAMINE_NEXT
	ULONG rilc_CleanupCount;
	ULONG rilc_ExAllType;
	BOOL rilc_PacketPending;		// Packet has been sent, need to wait for reply 
	BOOL rilc_DiskWritable;			// Disk/Volume is writable
	struct ReadIconListData rilc_rd;
	struct List rilc_UnlinkedIconScanList;	// List of all icon files found
	struct List rilc_IconScanList;		// List of all icon files treated by LinkIconScanList
	struct List rilc_NonIconScanList;	// List of all non-icon files found, after LinkIconScanList()
	struct BTree *rilc_StdFilesTree;	// BTree of all non-icon files found during GetFileList()
	struct BTree *rilc_IconTree;		// BTree of all icons in wt_IconList and wt_LateIconList
	ULONG rilc_TotalFound;			// number of entries found
	UBYTE rilc_OrigViewByType;		// Initial window view type 
	struct internalScaWindowTask *rilc_WindowTask;
	enum ScanDirResult (*rilc_Check)(struct ReadIconListControl *rilc);
	struct BackDropList rilc_BackdropList;
	};

struct IconScanEntry
	{
	struct Node ise_Node;
	struct IconScanEntry *ise_IconPtr;	// Pointer from object to icon
	struct IconScanEntry *ise_ObjPtr;	// Pointer from icon to object
	Object *ise_IconObj;			// Iconobject generated during scanning
	ULONG ise_IconType;			// Workbench icon type (e.g. WBDISK)
	ULONG ise_Flags;
	ULONG ise_Pos;				// Result from IsIconName()
	char ise_PosChar;
	struct FileInfoBlock ise_Fib;
	ULONG64 ise_Size64;
	};

// Values in ise_Flags
#define	ISEFLG_IsIcon	0x00000001		// this entry is an icon
#define	ISEFLG_IsLink	0x00000002		// this is a link
#define	ISEFLG_Used	0x00000004		// this entry has already been processed

//---------------------------------------------------------------

// definitions for 
typedef ULONG (*MSGFUNC)(struct internalScaWindowTask *, struct Message *, APTR);

struct MsgTableEntry
	{
	MSGFUNC MsgFunc;
	MSGFUNC ReplyFunc;
	};

//---------------------------------------------------------------

// definitions for IDCMPFuncTable

typedef ULONG (*IDCMPMSGFUNC)(struct internalScaWindowTask *, struct IntuiMessage *);

// IntuiMessage's will NOT be ReplyMSG()'ed when the MsgFunc returns a non-zero value !!

struct IDCMPTableEntry
	{
	ULONG ite_IDCMP;
	IDCMPMSGFUNC ite_MsgFunc;
	};

//---------------------------------------------------------------

// arguments for WblTask()

struct WblInput
	{
	ULONG	wbli_NumArgs;
	ULONG	wbli_StackSize;
	ULONG	wbli_dummy;
	UWORD	wbli_Flags;
	UWORD	wbli_Priority;
	struct	WBArg wbli_ArgList[1];	// !! variable size !!
	};

struct WblReturn
	{
	struct WBStartup *wblr_WBStartup;
	};

struct WblMessage
	{
	struct	Message wbl_Message;
	ULONG	wbl_Magic;		// ID_WBLM
	union	{
		struct WblReturn wbl_Return;
		struct WblInput  wbl_Input;
		} wbl_IO;
	};

#define	ID_WBLM	MAKE_ID('W','B','L','M')

struct WBNode
	{
	struct Node wbn_Node;
	struct WBStartup *wbn_wbStart;
	};

// Values in wbl_Flags
// same Flags as in SCA_WBStart()
// SCAF_WBStart_xxxxx

//---------------------------------------------------------------

// message to window backfill hook

struct BackFillMsg
	{
	struct Layer *bfm_Layer;
	struct Rectangle bfm_Rect;
	LONG bfm_OffsetX;
	LONG bfm_OffsetY;
	};

//---------------------------------------------------------------

struct ARexxCmdEntry
	{
	CONST_STRPTR ace_CmdName;
	ULONG ace_Flags;
	AREXXFUNC ace_EntryPoint;
	};

// values in ace_Flags
#define	ACEFLAGB_RunFromMainTask	0
#define	ACEFLAGF_RunFromMainTask	(1L << ACEFLAGB_RunFromMainTask)

//---------------------------------------------------------------

// data types for "globalCopyClipBoard"
// i.e. file copy/cut/paste operations

struct FileTransArg
	{
	struct	Node ftarg_Node;
	ULONG	ftarg_Opcode;		// enum ftOpCodes
	struct	WBArg ftarg_Arg;

	struct internalScaWindowTask *ftarg_iwt;	// Scalos window this entry belongs to (may not be valid!)
	struct ScaIconNode *ftarg_in;			// IconNode inside ftarg_iwt (may not be valid!)
	};

//---------------------------------------------------------------

// Message functions in NotifyTable
typedef void (*NOTIFYFUNC)(struct internalScaWindowTask *, struct NotifyMessage *);

struct NotifyNode
	{
	struct MinNode non_Node;
	struct NotifyRequest non_NotifyRequest;
	};

struct NotifyTab
	{
	CONST_STRPTR nft_FileName;	// file name to start notification on
	NOTIFYFUNC nft_Entry;		// notification function
	};

//---------------------------------------------------------------

struct DatatypesImage
	{
	struct Node dti_Node;

	STRPTR dti_Filename;		// Copy of original datatypes file name,
					// created w/ AllocCopyString()
	STRPTR dti_TempFilename;	// name of temp. file
					// created w/ AllocPathBuffer
	Object *dti_ImageObj;		// datatypes image object
	struct BitMapHeader *dti_BitMapHeader;
	struct BitMap *dti_BitMap;
	PLANEPTR dti_MaskPlane;
	ULONG *dti_ARGB;

	LONG dti_UseCount;
	
	ULONG dti_Flags;

	struct NotifyTab dti_NotifyTab;
	struct NotifyNode *dti_NotifyNode;	 // notification if original image is modified
	};

// Flag bits in dti_Flags
#define	DTIFLAG_NoRemap		0x00000001	// No Remap
#define	DTI_NoFriendBitMap	0x00000002	// don't use Friend BitMap

//---------------------------------------------------------------

struct ScalosTagList
	{
	struct TagItem *stl_TagList;
	ULONG stl_MaxEntries;
	ULONG stl_Index;
	};

#define	SCALOSTAGLIST_STEP	20

//---------------------------------------------------------------

// Include nesting limit for filetype descriptions
#define	MAX_INCLUDE_NESTING	10

struct FileTypeFileDesc
	{
	ULONG ftfd_LineNumber;
	BPTR ftfd_FileHandle;
	};

struct FileTypeFileInfo
	{
	struct Node ffi_Node;

	struct NotifyTab ffi_NotifyTab;
	struct NotifyNode *ffi_NotifyNode;
	STRPTR ffi_Path;
	STRPTR ffi_Name;
	};

struct FileTypeDef
	{
	struct Node ftd_Node;

	STRPTR ftd_Name;				// Name of filetype (in ftd_Node.ln_Name)

	SCALOSSEMAPHORE ftd_Sema;			 // Semaphore prevents FileTypeDef from being deleted while in use
	SCALOSSEMAPHORE	ftd_ReadSema;			 // Semaphore is held exclusively while ftd is read
	SCALOSSEMAPHORE	ftd_DisposeSema;		 // Semaphore is held exclusively while ftd is deleted
	SCALOSSEMAPHORE	ftd_FileInfoSema;		// Semaphore protects ftd_FileInfoList

	STRPTR ftd_Description;				// textual description of file type

	struct ScalosMenuTree *ftd_DefaultAction;	// default action on double click

	BPTR ftd_FileTypesDirLock;			// Lock on filetype descriptions' directory

	struct ttDef *ftd_ToolTipDef;
	struct PopupMenu *ftd_PopupMenu;

	struct List ftd_FileInfoList;

	BOOL ftd_ReadFinished;				// TRUE when filetype has been read

	STRPTR ftd_PreviewPluginName;			// name of filetype-specific thumbnail plugin
	struct Library *ftd_PreviewPluginBase; 		// library base for filetype-specific thumbnail plugin
#ifdef __amigaos4__
	struct ScalosPreviewPluginIFace *ftd_IPreviewPlugin;
#endif

	// ---- temporary, only used during Popup menu and Tooltip creation --------------

	struct List ftd_MenuList;			// Root List of FileTypeMenuItem

	struct List *ftd_CurrentMenuList;

	struct ScalosTagList ftd_TagList;
	struct ScalosTagList *ftd_CurrentTagList;

	struct FileTypeMenuItem *ftd_CurrentMenuItem;
	struct FileTypeMenuItem *ftd_ParentMenuItem;

	struct FileTypeTTItem *ftd_CurrentTTItem;

	struct FileTypeFileDesc ftd_FileHandles[MAX_INCLUDE_NESTING];
	ULONG ftd_IncludeNesting;			// index into ftd_FileHandle[]

	ULONG ftd_RequestedSection;
	};

//---------------------------------------------------------------

struct GlobalGadgetDef
	{
	struct internalScaWindowTask *ggd_iwt;	// ScalosWindow the gadget is located in
	struct Gadget *ggd_Gadget;		// the Gadget itself
	struct ControlBarGadgetEntry *ggd_cgy;	// for control bar gadgets, this is the corresponding ControlBarGadgetEntry,
						// only valid if ggd_GadgetID == SGTT_GADGETID_ControlBar
	struct Hook *ggd_GadgetTextHook;	// Hook for custom gadget help texts
	enum sgttGadgetIDs ggd_GadgetID;	// ID of the gadget
	};

//---------------------------------------------------------------

struct ScalosSemaphoreList
	{
	SCALOSSEMAPHORE	*sspl_Semaphore;
	CONST_STRPTR sspl_Name;
	};

//---------------------------------------------------------------

struct ScalosTextAttr
	{
	struct TextAttr sta_TextAttr;
	STRPTR sta_AllocName;			// Allocated string for ta_Name
	};

//---------------------------------------------------------------

// private variant of ScaInternInfos
struct extendedScaInternInfos
{
	struct ScaInternInfos xii_iinfos;		// public part of ScaInternInfos
	struct GlobalGadgetDef xii_GlobalGadgetUnderPointer;
	struct DragHandle *xii_GlobalDragHandle;
#if defined(__MORPHOS__)
	BOOL xii_Layers3D;
#endif //defined(__MORPHOS__)
};

//---------------------------------------------------------------

// Memory allocation

#define	SCALOS_MEM_START_MAGIC	0x5343414d
#define	SCALOS_MEM_END_MAGIC	0xe34341cd

#define	SCALOS_MEM_TRAILER	20

struct AllocatedMemFromPoolDebug
	{
	size_t		amp_Size;			// Size of allocated memory

	ULONG		amp_Line;			// Line number of allocator
	CONST_STRPTR	amp_File;			// File name of allocator
	CONST_STRPTR	amp_Function;			// Name of allocating function

	ULONG		amp_Magic;

	UBYTE		amp_UserData[0];		// Start of user-visible memory
	};

struct AllocatedMemFromPool
	{
	size_t		amp_Size;			// Size of allocated memory
	UBYTE		amp_UserData[0];		// Start of user-visible memory
	};

//---------------------------------------------------------------

struct ImageBorders
	{
	UWORD Left;
	UWORD Top;
	UWORD Right;
	UWORD Bottom;
	};

struct IconCleanSpace
	{
	WORD Left;              // left/right cleanup spaces around icons
	WORD Top;               // top/bottom cleanup spaces around icons
	WORD XSkip;             // horizontal space left between icons on cleanu
	WORD YSkip;		// vertical space left between icons on cleanu
	};

//---------------------------------------------------------------

enum TextIColNames
	{
	TXICOL_Name,
	TXICOL_Size,
	TXICOL_Prot,
	TXICOL_Date,
	TXICOL_Time,
	TXICOL_Comment,
	TXICOL_Owner,
	TXICOL_Group,
	TXICOL_FileType,
	TXICOL_Version,
	TXICOL_MiniIcon,

	TXICOL_MAX
	};

//--------------------------------------------------------------------

struct ControlBarGadgetEntry
	{
	struct	Node cgy_Node;

	enum SCPGadgetTypes cgy_GadgetType;

	char 	cgy_Action[40];		// Name of Menu Command
	STRPTR 	cgy_NormalImage;	// file name of normal state image
	STRPTR	cgy_SelectedImage;	// file name of selected state image
	STRPTR	cgy_DisabledImage;	// file name of disabled state image
	STRPTR  cgy_HelpText;		// Bubble help text
	};

//--------------------------------------------------------------------

struct IconWindowProperties
	{
	WORD iwp_XOffset;
	WORD iwp_YOffset;
	WORD iwp_WidthArray[WIDTHARRAY_MAX];    //Pixel width array for text mode columns
	UBYTE iwp_Viewmodes;
	UBYTE iwp_CheckOverlap;
	UWORD iwp_ShowAll;
	UWORD iwp_PatternNumber;
	ULONG iwp_ThumbnailView;
	ULONG iwp_ThumbnailsLifetimeDays;
	BOOL iwp_NoStatusBar;
	BOOL iwp_NoControlBar;
	BOOL iwp_BrowserMode;
	UWORD iwp_OpacityActive;
	UWORD iwp_OpacityInactive;
	enum ScalosSortOrder iwp_SortOrder;
	struct Rectangle iwp_IconSizeConstraints; // size limits for icons, larger or smaller icons are scaled
	UWORD iwp_IconScaleFactor;     // standard icon scaling factor in percent
	};

//---------------------------------------------------------------

struct ScaPopChildWindow
	{
	struct Node spcw_Node;
	BOOL spcw_WasLocked;		// Result from initial SuspendDrag
	APTR spcw_EventHandle;		// Result from SCCM_AddListener
	struct DragHandle *spcw_DragHandle;
	struct internalScaWindowTask *spcw_DestWindowTask;
	};

//---------------------------------------------------------------

// some MorphOS compatibility

#ifndef INTUITION_EXTENSIONS_H

#define WA_ExtraGadget_Iconify	(WA_Dummy + 153)
#define ETI_Dummy		(0xFFD0)
#define ETI_Iconify		(ETI_Dummy + 0)

#endif /* INTUITION_EXTENSIONS_H */

//---------------------------------------------------------------

// Flag values for ScaleBitMap()

#define SCALEFLAGB_BILINEAR    		0
#define SCALEFLAGB_AVERAGE     		1
#define SCALEFLAGB_DOUBLESIZE  	  	2
#define SCALEFLAGB_CORRECTASPECT      	3

#define SCALEFLAGF_BILINEAR 		(1L << SCALEFLAGB_BILINEAR)
#define SCALEFLAGF_AVERAGE  		(1L << SCALEFLAGB_AVERAGE)
#define SCALEFLAGF_DOUBLESIZE  		(1L << SCALEFLAGB_DOUBLESIZE)
#define SCALEFLAGF_CORRECTASPECT	(1L << SCALEFLAGB_CORRECTASPECT)

//---------------------------------------------------------------

// Flag values for AddThumbnailIcon()

#define THUMBNAILICONB_SAVEICON		0
#define THUMBNAILICONB_NOICONPOSITION	1

#define THUMBNAILICONF_SAVEICON	       	(1L << THUMBNAILICONB_SAVEICON)
#define THUMBNAILICONF_NOICONPOSITION	(1L << THUMBNAILICONB_NOICONPOSITION)

//----------------------------------------------------------------------------

// Constants used for preferences transparency settings.

#define PREFS_TRANSPARENCY_TRANSPARENT	0
#define PREFS_TRANSPARENCY_OPAQUE	100

//---------------------------------------------------------------

/* definitions for dd_Flags */
#if !defined(DDFLAGS_SHOWMASK)
#define DDFLAGS_SHOWMASK    0x00000003 /* show mode for drawers */

#define DDFLAGS_SORTMASK    0x00000300 /* sort order for text mode drawers (V52) */
#define DDFLAGS_SORTDEFAULT 0x00000000 /* default (inherit parent's sort order) */
#define DDFLAGS_SORTASC     0x00000100 /* sort files in ascending order */
#define DDFLAGS_SORTDESC    0x00000200 /* sort files in descending order */

#define DDFLAGS_INFOMASK    0x00000C00 /* show .info files in drawer? (V52) */
#define DDFLAGS_INFODEFAULT 0x00000000 /* default (inherit parent's .info mode) */
#define DDFLAGS_INFOHIDE    0x00000400 /* don't show .info files */
#define DDFLAGS_INFOSHOW    0x00000800 /* show .info files */
#endif //!defined(DDFLAGS_SHOWMASK)

//---------------------------------------------------------------

#ifdef __AROS__
// FIXME: temporary fix until we have figured out
// how to deal with these deprecated defines.
#define IA_ShadowPen    (IA_Dummy + 0x09)
#define IA_HighlightPen (IA_Dummy + 0x0A)
#endif

#endif	// SCALOS_STRUCTURES_H
