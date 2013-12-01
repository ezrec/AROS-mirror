// Variables.h
// $Date$
// $Revision$


#ifndef	VARIABLES_H
#define	VARIABLES_H


#include "defs.h"
#include <exec/types.h>

#include "Prefs.h"

/* ------------------------------------------------- */

// variables from AppMenu.c

extern SCALOSSEMAPHORE MenuSemaphore;
extern SCALOSSEMAPHORE AppMenuListSemaphore;	// Semaphore to protect ToolsMenuItemList and AppMenuList

/* ------------------------------------------------- */

// Array Index for PopupMenus[]
enum PopupMenusArrayIndex
	{
	SCPOPUPMENU_Disk=0,
	SCPOPUPMENU_Drawer,
	SCPOPUPMENU_Tool,
	SCPOPUPMENU_Trashcan,
	SCPOPUPMENU_Window,
	SCPOPUPMENU_AppIcon,
	SCPOPUPMENU_Desktop,

	SCPOPUPMENU_Last
	};

/* ------------------------------------------------- */

// defined in DefIcons.c
///
extern SCALOSSEMAPHORE DefIconsSemaphore;
extern SCALOSSEMAPHORE DefIconsCacheSemaphore;
///
/* ------------------------------------------------- */

// defined in filetypes.c
///
extern SCALOSSEMAPHORE FileTypeListSema;
///
/* ------------------------------------------------- */

// defined in FileCommands.c

extern ULONG prefCopyBuffLen;			// size of file copy buffer

/* ------------------------------------------------- */

// defined in FontUtil.c

extern struct Hook ScalosFontHook;
extern SCALOSSEMAPHORE tteSema;

/* ------------------------------------------------- */

// defined in IconImageClass.c

extern Class *IconImageClass;

/* ------------------------------------------------- */

// defined in idcmp.c

extern const struct IDCMPTableEntry IDCMPFuncTable[];	// +jl+ 20010929 IDCMP message table
extern const size_t IDCMPFuncTableSize;			// number of entries in IDCMPFuncTable[]

/* ------------------------------------------------- */

// defined in Memory.c

extern SCALOSSEMAPHORE MemPoolSemaphore;		// +jl+ 20020210 memory pool semaphore
extern void *MemPool;					// +jl+ 20020210 memory pool

/* ------------------------------------------------- */

// defined in Menu.c
///
extern struct IconMenuSupports IconSupportTable[];
extern struct IconMenuSupports WindowMenuTable[];
extern struct Menu *MainMenu;				// +jl+ 20010710 window menu
extern struct NewMenu *MainNewMenu;			// NewMenu array for main menu
extern struct MenuInfo *ParseMenuList;			// List of MenuInfo's
extern SCALOSSEMAPHORE ParseMenuListSemaphore;		// Semaphore to protect ParseMenuList
///
/* ------------------------------------------------- */

// defined in MenuCommand.c

extern struct ScalosMenuCommand MenuCommandTable[];
extern struct List globalCopyClipBoard;			// Clipboard for files/directories via copy/cut/paste
extern SCALOSSEMAPHORE ClipboardSemaphore;		// Semaphore to protect globalCopyClipBoard

/* ------------------------------------------------- */

// defined in Messages.c

extern struct MsgTableEntry iMsgTable[];	// +jl+ 20010928 Root_HandleMessages message table

/* ------------------------------------------------- */

// defined in PopupMenus.c

extern struct Hook PMGetStringHook;		// PM_LocaleHook getstring hook
extern struct PopupMenu *PopupMenus[];		// Array of Popup Menus

/* ------------------------------------------------- */

// defined in Prefs.c
///
extern struct Hook *globalCopyHook;
extern struct Hook *globalDeleteHook;
extern struct Hook *globalTextInputHook;
extern struct List globalCloseWBHookList;	// Hooks in this list will be called on AppSleep/AppWakeup
extern struct List ControlBarGadgetListNormal;	// List of control bar gadgets for standard windows
extern struct List ControlBarGadgetListBrowser;	// List of control bar gadgets for browser-mode windows
extern SCALOSSEMAPHORE CopyHookSemaphore;
extern SCALOSSEMAPHORE DeleteHookSemaphore;
extern SCALOSSEMAPHORE TextInputHookSemaphore;
extern SCALOSSEMAPHORE CloseWBHookListSemaphore;
extern struct PatternNode *PatternNodes;	// +jl+ 20010826
extern struct ScalosPrefs CurrentPrefs;
extern struct ScalosPalettePrefs PalettePrefs;
extern struct ScalosPatternPrefs PatternPrefs;
extern struct ScalosFontPrefs FontPrefs;
///
/* ------------------------------------------------- */

// defined in RootClass.c
///
extern SCALOSSEMAPHORE ListenerSema;
///
/* ------------------------------------------------- */

// defined in ScaLibrary.c

extern APTR ScalosLibFunctions[];

/* ------------------------------------------------- */

// defined in Scalos.c
///
extern const int Max_PathLen;			// length of buffer allocated by AllocPathBuffer()
extern struct MsgPort *FBlitPort;		// Result defined in FindPort("FBlit")
extern SCALOSSEMAPHORE QuitSemaphore;		// +jl+ 20011130 semaphore to prevent main process from premature quitting
extern SCALOSSEMAPHORE MenuSema;		// +jl+ 20010825 semaphore to protect menu access defined in changing windows
extern SCALOSSEMAPHORE LayersSema;		// +jl+ 20010806 semaphore to protect OpenWindow/CloseWindow/Layers operations
extern SCALOSSEMAPHORE WBStartListSema;		// semaphore to protect _WBStartList
extern SCALOSSEMAPHORE PatternSema; 		// +jl+ 20010826 Pattern semaphore
extern SCALOSSEMAPHORE ClassListSemaphore; 	// +jl+ 20010929 Class list semaphore
extern SCALOSSEMAPHORE DoWaitSemaphore;		// +jl+ 20011001 Semaphore for DOWAIT-List

extern struct List WBStartList;			// list of programs launched by scalos

extern struct ScalosClass *ScalosClassList;	// +jl+ 20010929 Scalos Class list
extern struct DoWait *DoWaitList;		// +jl+ 20011001 List of Msgs for DOWAIT

extern struct IClass *DtImageClass;		// Image Class wrapper for Scalos Datatypes images
extern struct IClass *TextIconClass;		// Class for Text mode icons
extern struct IClass *TextIconHighlightClass;	// Class for highlighting of text mode icons
extern struct IClass *IconifyImageClass;	// +jl+ 20010831 Imageclass for the iconify gadget

extern UWORD WBLaunchCount;			// number of workbench launched programs
extern ULONG RandomSeed;			// +jl+ 20011130 RandomSeed to generate good random numbers

extern APTR PenShareMap;			// +jl+ 20011118 PenShareMap defined in guigfx

extern ULONG OldMemSize;			// +jl+ 20010831 Old total memorysize

extern struct PluginClass *ScalosPluginList;	// +jl+ 20011009 List of plugin classes
extern Object *ScreenTitleObject;		// +jl+ 20010831 Scalos Title Object

extern struct Locale *ScalosLocale;		// +jl+ 20010803
extern struct Catalog *ScalosCatalog;		// +jl+ 20010829

extern struct TextFont *IconWindowFont;		// +jl+ 20010710 Font for icon window icons
extern struct TextFont *TextWindowFont;		// +jl+ 20010710 Font for text window icons

extern struct TTFontFamily IconWindowTTFont;	// +jl+ 20030110 TTengine Font for icon window icons
extern struct TTFontFamily TextWindowTTFont;	// +jl+ 20030110 TTengine Font for text window icons

extern struct TTFontFamily ScreenTTFont;	// TTengine screen font

extern STRPTR LastScreenTitleBuffer;		// +jl+ 20011226 Scalos ScreenTitle (Last Msg)
extern STRPTR ScreenTitleBuffer;		// +jl+ 20010831 Scalos ScreenTitle

extern UBYTE IconActive;
extern UBYTE PopupMenuFlag;			// Flag: Are popupmenus available +jl+ 20010303

extern struct MsgPort *wbPort;			// Workbench Message port

extern struct MainTask *MainWindowTask;		// +jl+ 20011020 MainTask
extern struct Process *MainTask;		// Scalos main task +jl+ 20010504

extern struct extendedScaInternInfos iInfos;
///
/* ------------------------------------------------- */

// defined in ScalosInit.c
///
extern struct Process *ScalosMainTask;

extern SCALOSSEMAPHORE WinListSemaphore;
extern struct ScaWindowList winlist;

extern UWORD ScaLibPluginOpenCount;		// +jl+ 20010710 Scalos library open count by plugins
extern UBYTE fInitializingPlugins;		// Flag: set during OOP plugin initialization
extern ULONG HelpGroupID;			// common ID for window help group

extern T_CONSOLEDEVICE ConsoleDevice;
extern struct Library *AslBase;
extern struct Library *CxBase;
extern struct Library *CyberGfxBase;
extern struct Library *DataTypesBase;
extern struct Library *DiskfontBase;
extern struct DosLibrary *DOSBase;
extern struct GfxBase *GfxBase;
extern struct Library *GuiGFXBase;
extern struct Library *GadToolsBase;
extern struct Library *IconBase;
extern struct Library *IconobjectBase;
extern struct Library *IFFParseBase;
extern struct IntuitionBase *IntuitionBase;
extern struct Library *LayersBase;
extern T_LOCALEBASE LocaleBase;
extern struct PopupMenuBase *PopupMenuBase;
extern struct Library *PreferencesBase;
extern T_REXXSYSBASE RexxSysBase;
extern struct ScalosBase *ScalosBase;
extern struct Library *WorkbenchBase;
extern T_UTILITYBASE UtilityBase;
extern struct Library *TitlebarImageBase;
extern struct Library *TTEngineBase;
extern struct Library *SQLite3Base;
extern struct ScalosGfxBase *ScalosGfxBase;
///
/* ------------------------------------------------- */

// defined in ScalosVersion.c
///
extern CONST_STRPTR versTag;			// $VER version string
extern const char ScalosRevision[];		// Revision string
extern CONST_STRPTR ScalosBuildNr;		// Build number aka SVN revision
///
/* ------------------------------------------------- */

// defined in Semaphores.c
///
extern struct ScalosSemaphoreList GlobalSemaphoreList[];
///
/* ------------------------------------------------- */

// defined in Thumbnails.c
///
extern SCALOSSEMAPHORE ThumbnailsCleanupSemaphore;      // Semaphore to ensure only a single cleanup process is running
///
/* ------------------------------------------------- */

// defined in Undo.c

extern struct List globalUndoList;			// global Undo list for all file-related actions
extern SCALOSSEMAPHORE UndoListListSemaphore;		// Semaphore to protect globalUndoList
extern struct List globalRedoList;			// global Redo list for all file-related actions
extern SCALOSSEMAPHORE RedoListListSemaphore;		// Semaphore to protect globalRedoList

/* ------------------------------------------------- */

#endif /* VARIABLES_H */
