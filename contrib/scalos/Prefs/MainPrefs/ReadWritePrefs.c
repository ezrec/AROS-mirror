// ReadWritePrefs.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <scalos/preferences.h>
#include <scalos/scalos.h>
#include <scalos/scalospreviewplugin.h>
#include <intuition/intuition.h>
#include <devices/inputevent.h>
#include <libraries/iffparse.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/mcpgfx.h>
#include <libraries/ttengine.h>
#include <graphics/view.h>
#include <prefs/prefhdr.h>
#include <prefs/font.h>
#include <prefs/workbench.h>
#include <mui/NListview_mcc.h>
#include <mui/HotkeyString_mcc.h>
#include <mui/Popplaceholder_mcc.h>
#include <mui/TextEditor_mcc.h>

#include "MCPFrame_mcc.h"

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/scalos.h>
#include <proto/scalosplugin.h>
#include <proto/preferences.h>
#include <proto/utility.h>
#include <proto/wb.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>

#include <FontSampleMCC.h>
#include "SelectMarkSampleClass.h"
#include "ReadWritePrefs.h"
#include "HiddenDevices.h"
#include "ScalosPrefs.h"

//-----------------------------------------------------------------

#define	PS_DATA(prefsstruct)	((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

//-----------------------------------------------------------------

// defined in Scalos.c

extern struct Library *PreferencesBase;
extern struct ScalosBase *ScalosBase;
extern struct Library *WorkbenchBase;

extern struct List PluginList;
extern struct List ControlBarGadgetListNormal;
extern struct List ControlBarGadgetListBrowser;
extern CONST_STRPTR cTextWindowsColumns[];

//-----------------------------------------------------------------

// from debug.lib
extern int kprintf(const char *fmt, ...);

//-----------------------------------------------------------------

static void FillPrefsStructures(struct SCAModule *app);
static void ReadPluginList(APTR p_MyPrefsHandle, LONG lID);
static void WritePluginList(APTR p_MyPrefsHandle, LONG lID);
static void CleanupPluginList(void);
static void ReadTextModeColumns(APTR p_MyPrefsHandle, LONG lID);
static void WriteTextModeColumns(APTR p_MyPrefsHandle, LONG lID);
static void RemoveFileDisplayEntry(Object *list, ULONG Index);
static void ReadNewIconsPrecision(APTR p_MyPrefsHandle, LONG lID);
static void WriteNewIconsPrecision(APTR p_MyPrefsHandle, LONG lID);
static void UpdateFileDisplayPrefsFromGUI(struct SCAModule *app);
static size_t TranslateQualifierToString(ULONG Qualifier, STRPTR line, size_t MaxLength);
static ULONG TranslateStringToQualifier(CONST_STRPTR line);
static void AdjustBobMethod(struct SCAModule *app);
static LONG WriteIconFontPrefs(struct SCAModule *app, struct IFFHandle *iff);
static size_t GetFontPrefsSize(const struct FontPrefs *FontChunk);
static void ReadControlBarGadgetList(APTR p_MyPrefsHandle, LONG lID, struct List *CbGadgetsList,
	ULONG prefsIDGadgets, ULONG prefsIDStrings);
static void WriteControlBarGadgetList(struct SCAModule *app, APTR p_MyPrefsHandle, LONG lID,
	ULONG ObjID, ULONG prefsIDGadgets, ULONG prefsIDStrings);
static void CleanupControlBarGadgetsList(struct List *CbGadgetsList);
static void CreateDefaultControlBarGadgets(void);
static ULONG GetLog2(ULONG value);
static void SetWordPreferences(APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST UWORD *arg, ULONG struct_Size);
static void SetLongPreferences(APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST ULONG *arg, ULONG struct_Size);

//-----------------------------------------------------------------

#define	MAX_QUALIFIER_STRING	80

//-----------------------------------------------------------------

struct InputEventName
{
	CONST_STRPTR		ien_name;
	ULONG			ien_Qualifier;
};

// --- Structures To Hold Preferences
struct PathsGroup
{
	char			pg_pDefaultIcons[MAX_FILENAME];
	char			pg_pDiskCopy[MAX_FILENAME];
	char			pg_pHome[MAX_FILENAME];
	char			pg_pThemes[MAX_FILENAME];
	char			pg_pImageCache[MAX_FILENAME];
	char			pg_pThumbnailDb[MAX_FILENAME];
	char			pg_pFormat[MAX_FILENAME];
	char			pg_pWbStartup[MAX_FILENAME];
	char                    pg_SQLiteTempDir[MAX_FILENAME];
};

struct StartupGroup
{
	BYTE			sg_bShowSplash;
	UWORD			sg_bSplashCloseDelay;
	BYTE			sg_bWBStartupDelay;
	char 			sg_pWBStartup[MAX_FILENAME];
};

struct DesktopGroup
{
	char			dg_pScreen[1000];
	BYTE			dg_bTitleRefresh;
	BYTE			dg_bMemoryRefresh;
	BYTE			dg_bWinTitleRefresh;
	BYTE			dg_bWinMemoryRefresh;
	BYTE			dg_bDiskRefresh;
	BYTE			dg_bHideTitleBar;
	BYTE			dg_bPopTitleBar;
	enum ScreenTitleModes 	dg_ScreenTitleMode;
	BYTE			dg_bDropStart;
	BYTE			dg_AutoLeaveOut;
	BYTE			dg_bAllowCloseWB;
	char			dg_ConsoleName[256];
	ULONG 			dg_SingleWindowLassoQualifier;
	char			dg_SingleWindowLassoQualifierString[MAX_QUALIFIER_STRING];
};

struct IconGroup
{
	// --- General
	struct	IBox		ig_IconOffsets;
	WORD			ig_wNormFrame;
	WORD			ig_wSelFrame;


	// --- Attributes
	BYTE			ig_bClickAreaMask;
	BYTE			ig_bNewIconTransparent;
	LONG			ig_bNewIconPrecision;
	BYTE			ig_bLoadDefIconsFirst;
	BYTE			ig_bIconsSaveable;
	UWORD			ig_TransparencyDefaultIcons;
	BYTE			ig_wMultipleLines;
	BYTE			ig_HighlightUnderMouse;
	char			ig_IconFontDesc[MAX_FONTNAME];
	struct Rectangle 	ig_SizeConstraints;
	UWORD			ig_NominalSize;
	UBYTE			ig_ShowThumbnails;
	BYTE			ig_ShowThumbnailsAsDefault;
	UWORD			ig_ThumbnailSize;
	UWORD			ig_ThumbnailMaxAge;
	UWORD			ig_ThumbnailMinSizeLimit;
	ULONG			ig_ThumbnailQuality;
	BYTE			ig_bSquareThumbnails;
	WORD			ig_wNormThumbnailFrame;
	WORD			ig_wSelThumbnailFrame;
	struct	IBox		ig_ThumbnailOffsets;
	UBYTE			ig_ThumbnailsBackfill;
	UWORD			ig_ThumbnailBackgroundTransparency;
	UBYTE                   ig_SelectedTextRectangle;
	UWORD			ig_SelTextRectBorderX;
	UWORD			ig_SelTextRectBorderY;
	UWORD			ig_SelTextRectRadius;

	// --- Tooltips
	BYTE			ig_bShowTooltips;
	ULONG			ig_bTooltipDelay;
	char 			ig_ToolTipFont[MAX_FONTNAME];
	LONG			ig_lDisplayFields;
	UWORD			ig_TransparencyTooltips;

	// --- Layout
	UBYTE 			ig_IconWindowLayoutModes[ICONTYPE_MAX];
	UBYTE 			ig_DeviceWindowLayoutModes[ICONTYPE_MAX];
};

struct DragNDropGroup
{
	BYTE			ddg_bBobStyle;
	BYTE			ddg_bBobMethod;
	BYTE			ddg_bBobLook; 	//transparentmode
	BYTE			ddg_bTranspType;
	BYTE			ddg_bAutoRemoveIcons;
	BYTE			ddg_bEasyMultiSelect;
	BYTE			ddg_bEasyMultiDrag;
	BYTE			ddg_bGroupDrag;
	ULONG 			ddg_pCopyQualifier;
	char			ddg_CopyQualifierString[MAX_QUALIFIER_STRING];
	ULONG 			ddg_pMakeLinkQualifier;
	char			ddg_MakeLinkQualifierString[MAX_QUALIFIER_STRING];
	ULONG 			ddg_pMoveQualifier;
	char			ddg_MoveQualifierString[MAX_QUALIFIER_STRING];
	LONG			ddg_lTriggers;
	BYTE			ddg_bShowObjectCount;
	BYTE			ddg_bDropMarkMode;
	UWORD			ddg_TransparencyIconDrag;
	UWORD			ddg_TransparencyIconShadow;
	UBYTE			ddg_bEnableDropMenu;
	UWORD 			ddg_PopupWindowDelaySeconds;			 // Time in s before drawer window pops up during D&D
};

struct WindowsGroup
{
	char 			wg_pRootWin[600];
	char 			wg_pWindow[600];
	BYTE			wg_bType;
	BYTE			wg_bPopTitleOnly;
	BYTE			wg_bMMBMove;
	BYTE			wg_bStatusBar;
	BYTE			wg_bCleanupOnResize;
	BYTE			wg_bHideHiddenFiles;
	BYTE			wg_bHideProtectHiddenFiles; // JMC
	UBYTE			wg_ShowAllByDefault;
	UBYTE			wg_ViewByDefault;
	UBYTE			wg_CheckOverlappingIcons;
//	  UWORD			  wg_WindowTransparency;
	UWORD			wg_TransparencyActiveWindow;
	UWORD			wg_wTransparencyInactiveWindow;
	struct IBox		wg_DefaultSize;
	struct IBox		wg_CleanupSpaces;
};

struct TextWindowGroup
{
	WORD			fd_wLabelStyle;
	WORD			fd_wTextSkip; //spacing between image & text
	BYTE			fd_bShowSoftLinks;
	char			fd_TextWindowFontDescr[MAX_FONTNAME];
	BYTE			fd_lDisplayFields[12];
	BYTE			fd_bShowStripes;
	UBYTE			fd_SelectTextIconName;
	struct ARGB		fd_SelectMarkerBaseColor;
	UBYTE			fd_SelectMarkerTransparency;
	enum ScalosDrawerSortType	fd_DrawerSortMode;
};

struct TrueTypeFontsGroup
{
	BYTE			ttg_Antialias;
	WORD			ttg_Gamma;		// Gamma * 1000
	BYTE			ttg_UseScreenTTFont;
	BYTE			ttg_UseIconTTFont;
	BYTE			ttg_UseTextWindowTTFont;
	char			ttg_ScreenTTFontDesc[MAX_TTFONTDESC];
	char			ttg_IconTTFontDesc[MAX_TTFONTDESC];
	char			ttg_TextWindowTTFontDesc[MAX_TTFONTDESC];
};

struct MiscGroup
{
	BYTE			mg_bMenuCurrentDir;
	BYTE			mg_bHardEmulation;
	BYTE			mg_bUseExAll;
	BYTE			mg_bCreateSoftLinks;
	ULONG 			mg_PopupApplySelectedQualifier;
	char			mg_PopupApplySelectedQualifierString[MAX_QUALIFIER_STRING];
	BYTE			mp_PopupApplySelectedAlways;
	ULONG			mg_DefaultStackSize;
	ULONG			mg_CopyBuffSize;
	ULONG			mg_MaxUndoSteps;
};

enum DisplayFieldsIndex
{
	DFIDX_Name,
	DFIDX_Size,
	DFIDX_Access,
	DFIDX_Date,
	DFIDX_Time,
	DFIDX_Comment,
};


struct ScalosPrefsContainer
{
	struct PathsGroup		Paths;
	struct StartupGroup		Startup;
	struct DesktopGroup		Desktop;
	struct IconGroup		Icons;
	struct DragNDropGroup		DragNDrop;
	struct WindowsGroup		Windows;
	struct TextWindowGroup		TextWindows;
	struct TrueTypeFontsGroup	TrueTypeFonts;
	struct MiscGroup		Miscellaneous;
};

//-----------------------------------------------------------------

static const struct ScalosPrefsContainer defaultPrefs =
{
	{
	// Paths
#ifdef __AROS__
		"ENVARC:sys",		// pg_pDefaultIcons[MAX_FILENAME];
#else
		"ENV:sys",		// pg_pDefaultIcons[MAX_FILENAME];
#endif
		"Sys:System/DiskCopy",	// pg_pDiskCopy[MAX_FILENAME];
		"Scalos:",		// pg_pHome[MAX_FILENAME];
		"Scalos:Themes",	// pg_pThemes[MAX_FILENAME];
		"t:",			// pg_pImageCache
		"Scalos:Thumbnails.db",	// pg_pThumbnailDb[MAX_FILENAME];
		"Sys:System/Format",	// pg_pFormat[MAX_FILENAME];
		"SYS:WBStartup",	// pg_pWbStartup[MAX_FILENAME];
		"t:",			// pg_SQLiteTempDir
	},
	{
	// Startup
		TRUE,			// sg_bShowSplash;
		5,			// sg_bSplashCloseDelay;
		5,			// sg_bWBStartupDelay;
		"SYS:WBStartup", 	// sg_pWBStartup[MAX_FILENAME];
	},
	{
	// Desktop
		"Scalos V%wb %fc graphics mem %ff other mem",	// dg_pScreen[512];
		5,			// dg_bTitleRefresh;
		FALSE,			// dg_bMemoryRefresh;
		5,			// dg_bWinTitleRefresh;
		FALSE,			// dg_bWinMemoryRefresh;
		5,			// dg_bDiskRefresh;
		FALSE,			// dg_bHideTitleBar;
		FALSE,			// dg_bPopTitleBar;
		SCREENTITLE_Visible,	// dg_ScreenTitleMode
		FALSE,			// dg_bDropStart;
		FALSE,			// dg_AutoLeaveOut
		TRUE,			// dg_bAllowCloseWB;
		"CON:0/11//150/Scalos Output Window/AUTO/WAIT",	 // dg_ConsoleName
		IEQUALIFIER_CONTROL,	// dg_pSingleWindowQualifier
		"",                     // dg_SingleWindowLassoQualifierString
	},
	{
	// Icons
		{			// ig_IconOffsets;
		2, 2, 2, 2,
		},
		MF_FRAME_NONE,		// ig_wNormFrame;
		MF_FRAME_NONE | (WORD) MCP_FRAME_RECESSED,  // ig_wSelFrame;

		// --- Attributes
		TRUE,			// ig_bClickAreaMask;
		TRUE,			// ig_bNewIconTransparent;
		4,			// ig_bNewIconPrecision;
		TRUE,			// ig_bLoadDefIconsFirst;
		TRUE,			// ig_bIconsSaveable;
		70,			// ig_TransparencyDefaultIcons;
		0,			// ig_wMultipleLines;
		FALSE,			// ig_HighlightUnderMouse;
		"Topaz/8",		// ig_IconFontDesc
		{ 			// ig_SizeConstraints
                0, 0, SHRT_MAX, SHRT_MAX
		},
		100,			// ig_NominalSize
		THUMBNAILS_Never,	// ig_ShowThumbnails;
		TRUE,			// ig_ShowThumbnailsAsDefault;
		48,			// ig_ThumbnailSize;
		14,			// ig_ThumbnailMaxAge;
		128,			// ig_ThumbnailMinSizeLimit;
		SCALOSPREVIEWA_Quality_Max,	// ig_ThumbnailQuality;
		FALSE,			// ig_bSquareThumbnails
		MF_FRAME_NONE,		// ig_wNormThumbnailFrame;
		MF_FRAME_NONE | (WORD) MCP_FRAME_RECESSED, // ig_wSelThumbnailFrame;
		{			// ig_ThumbnailOffsets;
		2, 2, 2, 2,
		},
		FALSE,			// ig_ThumbnailsBackfill;
		50,			// ig_ThumbnailBackgroundTransparency;
		FALSE,			// ig_SelectedTextRectangle;
		4,			// ig_SelTextRectBorderX;
		2,			// ig_SelTextRectBorderY;
		5,			// ig_SelTextRectRadius;

		// --- Tooltips
		TRUE,			// ig_bShowTooltips;
		2,			// ig_bTooltipDelay;
		"Topaz/8",		// ig_ToolTipFont[MAX_FONTNAME];
		0,			// ig_lDisplayFields;
		100,			// ig_TransparencyTooltips

		// --- Layout
		{			// ig_IconWindowLayoutModes[ICONTYPE_MAX];
		ICONLAYOUT_Columns,
		ICONLAYOUT_Columns,     //WBDISK
		ICONLAYOUT_Columns,	//WBDRAWER
		ICONLAYOUT_Columns,	//WBTOOL
		ICONLAYOUT_Columns,	//WBPROJECT
		ICONLAYOUT_Columns,	//WBGARBAGE
		ICONLAYOUT_Columns,	//WBDEVICE
		ICONLAYOUT_Columns,	//WBKICK
		ICONLAYOUT_Columns,	//WBAPPICON
		},
		{			// ig_DeviceWindowLayoutModes[ICONTYPE_MAX];
		ICONLAYOUT_Columns,
		ICONLAYOUT_Columns,	//WBDISK
		ICONLAYOUT_Columns,	//WBDRAWER
		ICONLAYOUT_Rows,	//WBTOOL
		ICONLAYOUT_Rows,     	//WBPROJECT
		ICONLAYOUT_Columns,	//WBGARBAGE
		ICONLAYOUT_Columns,     //WBDEVICE
		ICONLAYOUT_Columns,	//WBKICK
		ICONLAYOUT_Rows,	//WBAPPICON
		},
	},
	{
	// --- Drag'n'Drop
		DRAGTYPE_ImageAndText,	// ddg_bBobStyle;
		DRAGMETHOD_Custom,	// ddg_bBobMethod;
		DRAGTRANSPMODE_SolidAndTransp,	// ddg_bBobLook;
		TRANSPTYPE_Transparent,	// ddg_bTranspType;
		TRUE,			// ddg_bAutoRemoveIcons;
		FALSE,			// ddg_bEasyMultiSelect;
		FALSE,			// ddg_bEasyMultiDrag;
		FALSE,			// ddg_bGroupDrag;
		IEQUALIFIER_CONTROL,	// ddg_pCopyQualifier
		"",			// ddg_CopyQualifierString
		IEQUALIFIER_LALT,	// ddg_pMakeLinkQualifier
		"",			// ddg_MakeLinkQualifierString
		IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT,	// ddg_pMoveQualifier
		"",			// ddg_MoveQualifierString

		0,			// ddg_lTriggers;
		TRUE,			// ddg_bShowObjectCount;
		IDTV_WinDropMark_WindowedOnly,	// ddg_bDropMarkMode;
		50,			// ddg_TransparencyIconDrag
		25,			// ddg_TransparencyIconShadow
		FALSE,			// ddg_bEnableDropMenu
		3, 			// ddg_PopupWindowDelaySeconds - Time in s before drawer window pops up during D&D
	},
	{
	// Windows
		"%pa %d( %dp%% full, %DF free, %DU in use %d)",	// wg_pRootWin[256];
		"%pa",			// wg_pWindow[256];
		FALSE,			// wg_bType;		FALSE=Simple, TRUE=Smart refresh
		FALSE,			// wg_bPopTitleOnly;
		FALSE,			// wg_bMMBMove;
		TRUE,			// wg_bStatusBar
		TRUE,			// wg_bCleanupOnResize
		FALSE,			// wg_bHideHiddenFiles;
		FALSE,			// wg_bHideProtectHiddenFiles; // JMC
		FALSE,			// wg_ShowAllByDefault
		IDTV_ViewModes_Icon,	// wg_ViewByDefault
#if defined(__MORPHOS__)
		TRUE,            	// wg_CheckOverlappingIcons
#else /* __MORPHOS__ */
		FALSE,			// wg_CheckOverlappingIcons
#endif /* __MORPHOS__ */
		100,			// wg_TransparencyActiveWindow;
		100,			// wg_wTransparencyInactiveWindow

		{			// wg_DefaultSize;
		100, 100,		// Left, Top
		200, 100,		// Width, Height
		},
		{			// wg_CleanupSpaces;
		5, 4,			// Left, Top
		5, 3,			// XSkip, YSkip
		}
	},
	{
	// TextWindows
		0,			// fd_wLabelStyle;	0=normal, 1=outline, 2=shadow
		1,			// fd_wTextSkip; //spacing between image & text
		TRUE,			// fd_bShowSoftLinks;
		"Topaz/8",		// fd_TextWindowFontDescr[MAX_FONTNAME];
		{			// fd_lDisplayFields[];
		0, 1, 2, 3, 4, 5, -1 
		},
		FALSE,			// fd_bShowStripes;
		FALSE,			// fd_SelectTextIconName
		{ (UBYTE) ~0, 133, 195, 221 },	// fd_SelectMarkerBaseColor
		128,			// fd_SelectMarkerTransparency
		DRAWER_SORT_First,	// fd_DrawerSortMode
	},
	{
	// TrueTypeFonts
		TT_Antialias_Auto,	// ttg_Antialias;
		2500,			// ttg_Gamma;		// Gamma * 1000
		FALSE,			// ttg_UseScreenTTFont;
		FALSE,			// ttg_UseIconTTFont;
		FALSE,			// ttg_UseTextWindowTTFont;
		"0/400/11/Arial",	// ttg_ScreenTTFontDesc[MAX_TTFONTDESC];
		"0/400/11/Arial",	// ttg_IconTTFontDesc[MAX_TTFONTDESC];
		"0/400/11/Arial",	// ttg_TextWindowTTFontDesc[MAX_TTFONTDESC];
	},
	{
	// Misc
		FALSE,			// mg_bMenuCurrentDir
		TRUE,			// mg_bHardEmulation
		TRUE,			// mg_bUseExAll
		FALSE,			// mg_bCreateSoftLinks
		IEQUALIFIER_LSHIFT,	// mg_PopupApplySelectedQualifier
		"",			// mg_PopupApplySelectedQualifierString
		FALSE,			// mp_PopupApplySelectedAlways
		16384,			// mg_DefaultStackSize
		262144,			// mg_CopyBuffSize
		10,			// mg_MaxUndoSteps
	}
};

static struct ScalosPrefsContainer	currentPrefs;

static struct FontPrefs IconFontPrefs;
static struct FontPrefs ScreenFontPrefs;
static struct FontPrefs SysFontPrefs;

static const struct InputEventName QualifierTable[] =
{
	{	"lshift",	IEQUALIFIER_LSHIFT				},
	{	"rshift",	IEQUALIFIER_RSHIFT				},
	{	"capslock",	IEQUALIFIER_CAPSLOCK				},
	{	"control",	IEQUALIFIER_CONTROL				},
	{	"lalt",		IEQUALIFIER_LALT				},
	{	"ralt",		IEQUALIFIER_RALT				},
	{	"lcommand",	IEQUALIFIER_LCOMMAND				},
	{	"rcommand",	IEQUALIFIER_RCOMMAND				},
	{	"shift",	IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT		},
	{	"alt",		IEQUALIFIER_LALT | IEQUALIFIER_RALT		},
	{	"command",	IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND	},
};

// default control bar gadgets
static const struct ControlBarGadgetEntry DefaultControlBarGadgets[] =
{
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_BackButton,		// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_ForwardButton,		// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_Separator,		// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_UpButton,			// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_History,			// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
	{
	{ NULL, NULL, },			// cgy_Node
	SCPGadgetType_BrowseButton,		// cgy_GadgetType;
	0,					// cgy_ImageObjectIndex
	NULL,					// cgy_Image
	"",					// cgy_ImageString
	"",					// cgy_Action[40]
	"",					// cgy_NormalImage
	"",					// cgy_SelectedImage
	"",					// cgy_DisabledImage
	"",					// cgy_HelpText
	},
};

//-----------------------------------------------------------------

BOOL ReadWritePrefsInit(void)
{
	if (!HiddenDevicesInit())
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------

void ReadWritePrefsCleanup(void)
{
	HiddenDevicesCleanup();

	CleanupPluginList();
	CleanupControlBarGadgetsList(&ControlBarGadgetListNormal);
	CleanupControlBarGadgetsList(&ControlBarGadgetListBrowser);
}

//-----------------------------------------------------------------

void UpdateGuiFromPrefs(struct SCAModule *app)
{
	ULONG n;

	ReadIconFontPrefs(app);

	// --- Paths Page

	setstring(app->Obj[STRING_DISKCOPY], (ULONG) currentPrefs.Paths.pg_pDiskCopy);
	setstring(app->Obj[STRING_SCALOSHOME], (ULONG) currentPrefs.Paths.pg_pHome);
	setstring(app->Obj[STRING_THEMES], (ULONG) currentPrefs.Paths.pg_pThemes);
	setstring(app->Obj[STRING_IMAGECACHE], (ULONG) currentPrefs.Paths.pg_pImageCache);
	setstring(app->Obj[STRING_THUMBNAILDB], (ULONG) currentPrefs.Paths.pg_pThumbnailDb);
	setstring(app->Obj[STRING_FORMAT], (ULONG) currentPrefs.Paths.pg_pFormat);
	setstring(app->Obj[STRING_SQLITE3TEMPPATH], (ULONG) currentPrefs.Paths.pg_SQLiteTempDir);

	// --- Startup Page

	setcheckmark(app->Obj[CHECK_SHOWSPLASH], currentPrefs.Startup.sg_bShowSplash);
	setslider(app->Obj[SLIDER_SPLASHCLOSE], currentPrefs.Startup.sg_bSplashCloseDelay);
	setslider(app->Obj[SLIDER_WBSDELAY], currentPrefs.Startup.sg_bWBStartupDelay);
	setstring(app->Obj[STRING_WBSTARTPATH], (ULONG) currentPrefs.Startup.sg_pWBStartup);

	// --- Desktop Page

	setstring(app->Obj[STRING_CONSOLENAME], (ULONG) currentPrefs.Desktop.dg_ConsoleName);
	set(app->Obj[POPPH_SCREENTITLE], MUIA_Popph_Contents, (ULONG) currentPrefs.Desktop.dg_pScreen);

	setslider(app->Obj[SLIDER_TITLEREFRESH], currentPrefs.Desktop.dg_bTitleRefresh);
	setcheckmark(app->Obj[CHECK_TITLEMEM], currentPrefs.Desktop.dg_bMemoryRefresh);
	set(app->Obj[SLIDER_TITLEREFRESH], MUIA_Disabled, currentPrefs.Desktop.dg_bMemoryRefresh);

	setslider(app->Obj[SLIDER_WINTITLEREFRESH], currentPrefs.Desktop.dg_bWinTitleRefresh);
	setcheckmark(app->Obj[CHECK_WINTITLEMEM], currentPrefs.Desktop.dg_bWinMemoryRefresh);
	set(app->Obj[SLIDER_WINTITLEREFRESH], MUIA_Disabled, currentPrefs.Desktop.dg_bWinMemoryRefresh);

	setslider(app->Obj[SLIDER_DISKREFRESH], currentPrefs.Desktop.dg_bDiskRefresh);

	if (currentPrefs.Desktop.dg_bHideTitleBar)
		{
		if (currentPrefs.Desktop.dg_bPopTitleBar)
			currentPrefs.Desktop.dg_ScreenTitleMode = SCREENTITLE_Popup;
		else
			currentPrefs.Desktop.dg_ScreenTitleMode = SCREENTITLE_Hidden;	// always hidden
		}
	else
		{
		currentPrefs.Desktop.dg_ScreenTitleMode = SCREENTITLE_Visible;	// always visible
		}
	setcycle(app->Obj[CYCLE_SCREENTITLEMODE], currentPrefs.Desktop.dg_ScreenTitleMode);

	setcheckmark(app->Obj[DROPSTART], currentPrefs.Desktop.dg_bDropStart);
	setcheckmark(app->Obj[CHECK_AUTOLEAVEOUT], currentPrefs.Desktop.dg_AutoLeaveOut);
	setcheckmark(app->Obj[CLOSEWB], currentPrefs.Desktop.dg_bAllowCloseWB);

	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDISK], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDISK]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDRAWER], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDRAWER]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBTOOL], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBTOOL]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBPROJECT], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBPROJECT]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBGARBAGE], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBGARBAGE]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDEVICE], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDEVICE]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBKICK], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBKICK]);
	setcycle(app->Obj[CYCLE_DESKTOP_LAYOUT_WBAPPICON], currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBAPPICON]);

	// --- Icons Page

	AdjustBobMethod(app);
	set(app->Obj[FRAME_ICONNORMAL], MUIA_MCPFrame_FrameType, currentPrefs.Icons.ig_wNormFrame);
	set(app->Obj[FRAME_ICONSELECTED], MUIA_MCPFrame_FrameType, currentPrefs.Icons.ig_wSelFrame);
	set(app->Obj[FRAME_ICON_THUMBNAIL_NORMAL], MUIA_MCPFrame_FrameType, currentPrefs.Icons.ig_wNormThumbnailFrame);
	set(app->Obj[FRAME_ICON_THUMBNAIL_SELECTED], MUIA_MCPFrame_FrameType, currentPrefs.Icons.ig_wSelThumbnailFrame);

	// make sure to set icon borders AFTER frame type since changing frame type adjusts borders
	set(app->Obj[ICONLEFT], MUIA_Numeric_Value, currentPrefs.Icons.ig_IconOffsets.Left);
	set(app->Obj[ICONRIGHT], MUIA_Numeric_Value, currentPrefs.Icons.ig_IconOffsets.Top);
	set(app->Obj[ICONTOP], MUIA_Numeric_Value, currentPrefs.Icons.ig_IconOffsets.Width);
	set(app->Obj[ICONBOTTOM], MUIA_Numeric_Value, currentPrefs.Icons.ig_IconOffsets.Height);

	// make sure to set thumbnail borders AFTER frame type since changing frame type adjusts borders
	set(app->Obj[THUMBNAILS_LEFTBORDER], MUIA_Numeric_Value, currentPrefs.Icons.ig_ThumbnailOffsets.Left);
	set(app->Obj[THUMBNAILS_RIGHTBORDER], MUIA_Numeric_Value, currentPrefs.Icons.ig_ThumbnailOffsets.Top);
	set(app->Obj[THUMBNAILS_TOPBORDER], MUIA_Numeric_Value, currentPrefs.Icons.ig_ThumbnailOffsets.Width);
	set(app->Obj[THUMBNAILS_BOTTOMBORDER], MUIA_Numeric_Value, currentPrefs.Icons.ig_ThumbnailOffsets.Height);

	setcheckmark(app->Obj[CHECK_ICONMASK], currentPrefs.Icons.ig_bClickAreaMask);
	setslider(app->Obj[SLIDER_ICONRMAP_PRECISION], currentPrefs.Icons.ig_bNewIconPrecision);
	setcheckmark(app->Obj[CHECK_NIMASK], currentPrefs.Icons.ig_bNewIconTransparent);
	setstring(app->Obj[STRING_DEFICONPATH], (ULONG) currentPrefs.Paths.pg_pDefaultIcons);
	setcheckmark(app->Obj[CHECK_DEFFIRST], currentPrefs.Icons.ig_bLoadDefIconsFirst);
	setcheckmark(app->Obj[CHECK_ICONSSAVE], currentPrefs.Icons.ig_bIconsSaveable);
	setcheckmark(app->Obj[CHECK_TOOLTIPS], currentPrefs.Icons.ig_bShowTooltips);
	setslider(app->Obj[SLIDER_TIPDELAY], currentPrefs.Icons.ig_bTooltipDelay);
	setstring(app->Obj[STRING_TOOLTIPFONT], (ULONG) currentPrefs.Icons.ig_ToolTipFont);
	setcheckmark(app->Obj[CHECK_MULTILINES], currentPrefs.Icons.ig_wMultipleLines);
	setcheckmark(app->Obj[CHECK_HILITEUNDERMOUSE], currentPrefs.Icons.ig_HighlightUnderMouse);
	setstring(app->Obj[POP_ICONFONT], (ULONG) currentPrefs.Icons.ig_IconFontDesc);
	setslider(app->Obj[SLIDER_ICONTOOLTIPS_TRANSPARENCY], currentPrefs.Icons.ig_TransparencyTooltips);

	setcycle(app->Obj[CYCLE_SHOWTHUMBNAILS], currentPrefs.Icons.ig_ShowThumbnails);

	set(app->Obj[CHECK_SHOWTHUMBNAILS_AS_DEFAULT], MUIA_Selected, currentPrefs.Icons.ig_ShowThumbnailsAsDefault);

	setslider(app->Obj[SLIDER_THUMBNAILS_MAXAGE], currentPrefs.Icons.ig_ThumbnailMaxAge);
	setslider(app->Obj[SLIDER_THUMBNAILS_MINSIZE_LIMIT], currentPrefs.Icons.ig_ThumbnailMinSizeLimit);
	setslider(app->Obj[SLIDER_THUMBNAILS_QUALITY], currentPrefs.Icons.ig_ThumbnailQuality);

	setcheckmark(app->Obj[CHECK_THUMBNAILS_SQUARE], currentPrefs.Icons.ig_bSquareThumbnails);
	setcheckmark(app->Obj[CHECK_THUMBNAILS_BACKFILL], currentPrefs.Icons.ig_ThumbnailsBackfill);
	setslider(app->Obj[SLIDER_ICONTRANSPARENCY_THUMBNAILBACK], currentPrefs.Icons.ig_ThumbnailBackgroundTransparency);

	setcheckmark(app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE], currentPrefs.Icons.ig_SelectedTextRectangle);
	setslider(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERX], currentPrefs.Icons.ig_SelTextRectBorderX);
	setslider(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], currentPrefs.Icons.ig_SelTextRectBorderY);
	setslider(app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS], currentPrefs.Icons.ig_SelTextRectRadius);
	set(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERX], MUIA_Disabled, !currentPrefs.Icons.ig_SelectedTextRectangle);
	set(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], MUIA_Disabled, !currentPrefs.Icons.ig_SelectedTextRectangle);
	set(app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS], MUIA_Disabled, !currentPrefs.Icons.ig_SelectedTextRectangle);

	set(app->Obj[GROUP_THUMBNAIL_BACKFILL_TRANSPARENCY], MUIA_Disabled, TRUE ? !currentPrefs.Icons.ig_ThumbnailsBackfill : FALSE);

	SetThumbNailSize(app, currentPrefs.Icons.ig_ThumbnailSize);
	SetIconSizeConstraints(app, &currentPrefs.Icons.ig_SizeConstraints);
	setslider(app->Obj[SLIDER_ICONSPAGE_NOMINALSIZE], currentPrefs.Icons.ig_NominalSize);

	d1(KPrintF(__FUNC__ "/%ld:  Icon offset  left=%ld  top=%ld  right=%ld  bottom=%ld\n", \
		__FUNC__, __LINE__, currentPrefs.Icons.ig_IconOffsets.Left, currentPrefs.Icons.ig_IconOffsets.Top, \
		currentPrefs.Icons.ig_IconOffsets.Width, currentPrefs.Icons.ig_IconOffsets.Height));

	setcycle(app->Obj[CYCLE_ICONS_LAYOUT_WBDRAWER], currentPrefs.Icons.ig_IconWindowLayoutModes[WBDRAWER]);
	setcycle(app->Obj[CYCLE_ICONS_LAYOUT_WBTOOL], currentPrefs.Icons.ig_IconWindowLayoutModes[WBTOOL]);
	setcycle(app->Obj[CYCLE_ICONS_LAYOUT_WBPROJECT], currentPrefs.Icons.ig_IconWindowLayoutModes[WBPROJECT]);

	// --- Drag and Drop Page

	//tooltip display field
	setcycle(app->Obj[CYCLE_STYLE], currentPrefs.DragNDrop.ddg_bBobStyle);
	setcycle(app->Obj[CYCLE_ROUTINE], currentPrefs.DragNDrop.ddg_bBobMethod);
	setmutex(app->Obj[RADIO_BOBLOOK], currentPrefs.DragNDrop.ddg_bBobLook);
	setcycle(app->Obj[CYCLE_TRANSPMODE], currentPrefs.DragNDrop.ddg_bTranspType);
	set(app->Obj[CYCLE_TRANSPMODE], MUIA_Disabled, DRAGMETHOD_GELS == currentPrefs.DragNDrop.ddg_bBobMethod);
	setcycle(app->Obj[CYCLE_DROPMARK], currentPrefs.DragNDrop.ddg_bDropMarkMode);
	set(app->Obj[SLIDER_ICONTRANSPARENCY_DRAG], MUIA_Disabled, TRANSPTYPE_Transparent != currentPrefs.DragNDrop.ddg_bTranspType);
	setslider(app->Obj[SLIDER_ICONTRANSPARENCY_DRAG], currentPrefs.DragNDrop.ddg_TransparencyIconDrag);
	setslider(app->Obj[SLIDER_ICONTRANSPARENCY_SHADOW], currentPrefs.DragNDrop.ddg_TransparencyIconShadow);
	setcheckmark(app->Obj[CHECK_ENABLE_DROPMENU], currentPrefs.DragNDrop.ddg_bEnableDropMenu);
	setslider(app->Obj[SLIDER_POPOPENWINDOW_DELAY], currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds);
	setslider(app->Obj[SLIDER_ICONTRANSPARENCY_DEFICONS], currentPrefs.Icons.ig_TransparencyDefaultIcons);
	setcheckmark(app->Obj[CHECK_AUTOREMOVE], currentPrefs.DragNDrop.ddg_bAutoRemoveIcons);
	setcheckmark(app->Obj[CHECK_EASY_MULTISELECT], currentPrefs.DragNDrop.ddg_bEasyMultiSelect);
	setcheckmark(app->Obj[CHECK_EASY_MULTIDRAG], currentPrefs.DragNDrop.ddg_bEasyMultiDrag);
	setcheckmark(app->Obj[CHECK_GROUPDRAG], !currentPrefs.DragNDrop.ddg_bGroupDrag);
	setcheckmark(app->Obj[CHECK_SHOWDRAG], currentPrefs.DragNDrop.ddg_bShowObjectCount);
	//copy qualifier
	TranslateQualifierToString(currentPrefs.DragNDrop.ddg_pCopyQualifier, 
		currentPrefs.DragNDrop.ddg_CopyQualifierString, 
		sizeof(currentPrefs.DragNDrop.ddg_CopyQualifierString));
	setstring(app->Obj[COPY_HOTKEY], (ULONG) currentPrefs.DragNDrop.ddg_CopyQualifierString);
	//MakeLink qualifier
	TranslateQualifierToString(currentPrefs.DragNDrop.ddg_pMakeLinkQualifier, 
		currentPrefs.DragNDrop.ddg_MakeLinkQualifierString, 
		sizeof(currentPrefs.DragNDrop.ddg_MakeLinkQualifierString));
	setstring(app->Obj[MAKELINK_HOTKEY], (ULONG) currentPrefs.DragNDrop.ddg_MakeLinkQualifierString);
	//Move qualifier
	TranslateQualifierToString(currentPrefs.DragNDrop.ddg_pMoveQualifier, 
		currentPrefs.DragNDrop.ddg_MoveQualifierString, 
		sizeof(currentPrefs.DragNDrop.ddg_MoveQualifierString));
	setstring(app->Obj[MOVE_HOTKEY], (ULONG) currentPrefs.DragNDrop.ddg_MoveQualifierString);

	// Single-Window Lasso qualifier
	TranslateQualifierToString(currentPrefs.Desktop.dg_SingleWindowLassoQualifier,
		currentPrefs.Desktop.dg_SingleWindowLassoQualifierString,
		sizeof(currentPrefs.Desktop.dg_SingleWindowLassoQualifierString));
	setstring(app->Obj[SINGLE_WINDOW_LASSO_HOTKEY], (ULONG) currentPrefs.Desktop.dg_SingleWindowLassoQualifierString);

	//triggers
	setcheckmark(app->Obj[CHECK_TRIG_DISK], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_DiskIcons);
	setcheckmark(app->Obj[CHECK_TRIG_DRAWER], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_DrawerIcons);
	setcheckmark(app->Obj[CHECK_TRIG_TOOL], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_ToolIcons);
	setcheckmark(app->Obj[CHECK_TRIG_PROJECT], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_ProjectIcons);
	setcheckmark(app->Obj[CHECK_TRIG_TRASHCAN], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_TrashcanIcons);
	setcheckmark(app->Obj[CHECK_TRIG_KICK], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_KickIcons);
	setcheckmark(app->Obj[CHECK_TRIG_APPICON], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_AppIcons);
	setcheckmark(app->Obj[CHECK_TRIG_APPWIN], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_AppWindows);
	setcheckmark(app->Obj[CHECK_TRIG_ICONIFIEDWIN], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_IconifiedWinIcons);
	setcheckmark(app->Obj[CHECK_TRIG_SCALOSWIN], currentPrefs.DragNDrop.ddg_lTriggers & DRAGTRANSPF_ScalosWindows);

	// --- Windows Page

	set(app->Obj[POPPH_ROOTWINDOWTITLE], MUIA_Popph_Contents, (ULONG) currentPrefs.Windows.wg_pRootWin);
	set(app->Obj[POPPH_DIRWINDOWTITLE], MUIA_Popph_Contents, (ULONG) currentPrefs.Windows.wg_pWindow);
	setcycle(app->Obj[CYCLE_WINDOWTYPE], currentPrefs.Windows.wg_bType);
	set(app->Obj[CHECK_POPTITLEONLY], MUIA_Selected, currentPrefs.Windows.wg_bPopTitleOnly);
	set(app->Obj[CHECK_MMBMOVE], MUIA_Selected, currentPrefs.Windows.wg_bMMBMove);
	set(app->Obj[WINLEFT], MUIA_String_Integer, currentPrefs.Windows.wg_DefaultSize.Left);
	set(app->Obj[WINTOP], MUIA_String_Integer, currentPrefs.Windows.wg_DefaultSize.Top);
	set(app->Obj[WINWIDTH], MUIA_String_Integer, currentPrefs.Windows.wg_DefaultSize.Width);
	set(app->Obj[WINHEIGHT], MUIA_String_Integer, currentPrefs.Windows.wg_DefaultSize.Height);
	setslider(app->Obj[SLIDER_CLEANLEFT], currentPrefs.Windows.wg_CleanupSpaces.Left);
	setslider(app->Obj[SLIDER_CLEANTOP], currentPrefs.Windows.wg_CleanupSpaces.Top);
	setslider(app->Obj[SLIDER_CLEANXSKIP], currentPrefs.Windows.wg_CleanupSpaces.Width);
	setslider(app->Obj[SLIDER_CLEANYSKIP], currentPrefs.Windows.wg_CleanupSpaces.Height);
	setcheckmark(app->Obj[CHECK_STATUSBAR], currentPrefs.Windows.wg_bStatusBar);
	setcheckmark(app->Obj[CHECK_CHECKOVERLAP], currentPrefs.Windows.wg_CheckOverlappingIcons);
	setcheckmark(app->Obj[CHECK_CLEANUP_ONRESIZE], currentPrefs.Windows.wg_bCleanupOnResize);
	setslider(app->Obj[SLIDER_TRANSPARENCY_ACTIVEWINDOW], currentPrefs.Windows.wg_TransparencyActiveWindow);
	setslider(app->Obj[SLIDER_TRANSPARENCY_INACTIVEWINDOW], currentPrefs.Windows.wg_wTransparencyInactiveWindow);

	// --- File Display Page

	setcycle(app->Obj[CYCLE_LABELSTYLE], currentPrefs.TextWindows.fd_wLabelStyle);
	setslider(app->Obj[SLIDER_LABELSPACE], currentPrefs.TextWindows.fd_wTextSkip);
	setcheckmark(app->Obj[SOFTICONSLINK], currentPrefs.TextWindows.fd_bShowSoftLinks);
	setcheckmark(app->Obj[SOFTTEXTSLINK], currentPrefs.TextWindows.fd_bShowSoftLinks);
	setstring(app->Obj[POP_TEXTMODEFONT], (ULONG) currentPrefs.TextWindows.fd_TextWindowFontDescr);
	set(app->Obj[STRING_TEXTMODEFONT_SAMPLE], MUIA_FontSample_StdFontDesc, (ULONG) currentPrefs.TextWindows.fd_TextWindowFontDescr);

	// first, put all entries into NLIST_STORAGE_FILEDISPLAY
	DoMethod(app->Obj[NLIST_STORAGE_FILEDISPLAY], MUIM_NList_Clear);

	for (n=0; cTextWindowsColumns[n]; n++)
		{
		struct FileDisplayListEntry fdle;

		fdle.fdle_Index = n;

		DoMethod(app->Obj[NLIST_STORAGE_FILEDISPLAY], MUIM_NList_InsertSingle, &fdle, MUIV_NList_Insert_Bottom);
		}

	// start with en empty "use" list
	DoMethod(app->Obj[NLIST_USE_FILEDISPLAY], MUIM_NList_Clear);

	// then, remove all used entries from NLIST_STORAGE_FILEDISPLAY and insert them in NLIST_USE_FILEDISPLAY
	for (n=0; ~0 != currentPrefs.TextWindows.fd_lDisplayFields[n] && n < Sizeof(currentPrefs.TextWindows.fd_lDisplayFields); n++)
		{
		struct FileDisplayListEntry fdle;

		fdle.fdle_Index = currentPrefs.TextWindows.fd_lDisplayFields[n];

		RemoveFileDisplayEntry(app->Obj[NLIST_STORAGE_FILEDISPLAY], fdle.fdle_Index);
		DoMethod(app->Obj[NLIST_USE_FILEDISPLAY], MUIM_NList_InsertSingle, &fdle, MUIV_NList_Insert_Bottom);
		}

	set(app->Obj[HIDEHIDDENFILES], MUIA_Selected, currentPrefs.Windows.wg_bHideHiddenFiles);
	set(app->Obj[HIDEPROTECTHIDDENFILES], MUIA_Selected, currentPrefs.Windows.wg_bHideProtectHiddenFiles); // JMC

	setcycle(app->Obj[CYCLE_SHOWALLDEFAULT], currentPrefs.Windows.wg_ShowAllByDefault);
	setcycle(app->Obj[CYCLE_VIEWBYICONSDEFAULT], currentPrefs.Windows.wg_ViewByDefault - 1);

	set(app->Obj[CHECK_STRIPED_WINDOW], MUIA_Selected, currentPrefs.TextWindows.fd_bShowStripes);
	set(app->Obj[CHECK_SELECTTEXTICONNAME], MUIA_Selected, currentPrefs.TextWindows.fd_SelectTextIconName);

	setcycle(app->Obj[CYCLE_DRAWERSORTMODE], currentPrefs.TextWindows.fd_DrawerSortMode);

	setslider(app->Obj[SLIDER_TEXTWINDOWS_SELECTFILLTRANSPARENCY], currentPrefs.TextWindows.fd_SelectMarkerTransparency);
	setslider(app->Obj[SLIDER_TEXTWINDOWS_SELECTBORDERTRANSPARENCY], currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Alpha);

	set(app->Obj[MCC_TEXTWINDOWS_SELECTMARKER_SAMPLE], TIHA_BaseColor, (ULONG) &currentPrefs.TextWindows.fd_SelectMarkerBaseColor);
	set(app->Obj[MCC_TEXTWINDOWS_SELECTMARKER_SAMPLE], TIHA_Transparency, currentPrefs.TextWindows.fd_SelectMarkerTransparency);

	SetAttrs(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK],
		MUIA_Coloradjust_Red, currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Red << 24,
		MUIA_Coloradjust_Green, currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Green << 24,
		MUIA_Coloradjust_Blue, currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Blue << 24,
		TAG_END);

	// --- TrueType Font Page

	setcycle(app->Obj[CYCLE_TTANTIALIAS], currentPrefs.TrueTypeFonts.ttg_Antialias);
	setslider(app->Obj[SLIDER_TTGAMMA], currentPrefs.TrueTypeFonts.ttg_Gamma);
	setcheckmark(app->Obj[CHECK_TTSCREENFONT_ENABLE], currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont);
	setcheckmark(app->Obj[CHECK_TTICONFONT_ENABLE], currentPrefs.TrueTypeFonts.ttg_UseIconTTFont);
	setcheckmark(app->Obj[CHECK_ICONSPAGE_TTICONFONT_ENABLE], currentPrefs.TrueTypeFonts.ttg_UseIconTTFont);
	set(app->Obj[GROUP_ICONSPAGE_ICONFONT], MUIA_Disabled, currentPrefs.TrueTypeFonts.ttg_UseIconTTFont);
	set(app->Obj[GROUP_ICONSPAGE_TTICONFONT], MUIA_Disabled, !currentPrefs.TrueTypeFonts.ttg_UseIconTTFont);
	setcheckmark(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont);
	setcheckmark(app->Obj[CHECK_TEXTWINDOW_TTTEXTWINDOWFONT_ENABLE], currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont);
	set(app->Obj[GROUP_TEXTWINDOW_FONT_SELECT], MUIA_Disabled, currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont);
	setstring(app->Obj[POPSTRING_TTSCREENFONT], (ULONG) currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc);
	setstring(app->Obj[POPSTRING_TTICONFONT], (ULONG) currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc);
	setstring(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], (ULONG) currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc);
	setstring(app->Obj[POPSTRING_TTTEXTWINDOWFONT], (ULONG) currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc);
	setstring(app->Obj[POPSTRING_TEXTWINDOW_TTTEXTWINDOWFONT], (ULONG) currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc);

	SetAttrs(app->Obj[MCC_TTSCREENFONT_SAMPLE], 
		MUIA_FontSample_Antialias, currentPrefs.TrueTypeFonts.ttg_Antialias,
		MUIA_FontSample_Gamma, currentPrefs.TrueTypeFonts.ttg_Gamma,
		MUIA_FontSample_TTFontDesc, (ULONG) currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc,
		TAG_END);
	SetAttrs(app->Obj[MCC_TTICONFONT_SAMPLE], 
		MUIA_FontSample_Antialias, currentPrefs.TrueTypeFonts.ttg_Antialias,
		MUIA_FontSample_Gamma, currentPrefs.TrueTypeFonts.ttg_Gamma,
		MUIA_FontSample_TTFontDesc, (ULONG) currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc,
		TAG_END);
	SetAttrs(app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE],
		MUIA_FontSample_Antialias, currentPrefs.TrueTypeFonts.ttg_Antialias,
		MUIA_FontSample_Gamma, currentPrefs.TrueTypeFonts.ttg_Gamma,
		MUIA_FontSample_TTFontDesc, (ULONG) currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc,
		TAG_END);
	SetAttrs(app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE], 
		MUIA_FontSample_Antialias, currentPrefs.TrueTypeFonts.ttg_Antialias,
		MUIA_FontSample_Gamma, currentPrefs.TrueTypeFonts.ttg_Gamma,
		MUIA_FontSample_TTFontDesc, (ULONG) currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc,
		TAG_END);
	SetAttrs(app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE],
		MUIA_FontSample_Antialias, currentPrefs.TrueTypeFonts.ttg_Antialias,
		MUIA_FontSample_Gamma, currentPrefs.TrueTypeFonts.ttg_Gamma,
		MUIA_FontSample_TTFontDesc, (ULONG) currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc,
		TAG_END);

	// --- Misc Page

	setcheckmark(app->Obj[CHECK_MENUCURRENTDIR], currentPrefs.Miscellaneous.mg_bMenuCurrentDir);
	setcheckmark(app->Obj[CHECK_HARDEMULATION], currentPrefs.Miscellaneous.mg_bHardEmulation);
	setcheckmark(app->Obj[CHECK_USEEXALL], currentPrefs.Miscellaneous.mg_bUseExAll);
	setcycle(app->Obj[CYCLE_CREATELINKS], currentPrefs.Miscellaneous.mg_bCreateSoftLinks);
	setslider(app->Obj[SLIDER_DEFAULTSTACKSIZE], currentPrefs.Miscellaneous.mg_DefaultStackSize / 1024);
	setslider(app->Obj[SLIDER_COPYBUFFERSIZE], GetLog2(currentPrefs.Miscellaneous.mg_CopyBuffSize));
	setslider(app->Obj[SLIDER_UNDOSTEPS], currentPrefs.Miscellaneous.mg_MaxUndoSteps);

	//Popup menu apply selected qualifier
	TranslateQualifierToString(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier, 
		currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString, 
		sizeof(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString));
	setstring(app->Obj[POPUP_SELECTED_HOTKEY], (ULONG) currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString);

	setcheckmark(app->Obj[CHECK_MISCPAGE_POPUP_SELECTED_ALWAYS], (ULONG) currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways);
	set(app->Obj[POPUP_SELECTED_HOTKEY], MUIA_Disabled, currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways);

	// --- Plugins Page

	{
	struct PluginDef *pd;

	set(app->Obj[PLUGIN_LIST], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_Clear);

	for (pd = (struct PluginDef *) PluginList.lh_Head;
		pd != (struct PluginDef *) &PluginList.lh_Tail;
		pd = (struct PluginDef *) pd->plug_Node.mln_Succ)
		{
		DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_InsertSingle,
			pd,
			MUIV_NList_Insert_Sorted);
		}

	set(app->Obj[PLUGIN_LIST], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_Redraw, MUIV_NList_Redraw_All);
	}

	{
	struct ControlBarGadgetEntry *cgy;

	set(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_Clear);

	// copy control bar gadgets to list
	for (cgy = (struct ControlBarGadgetEntry *) ControlBarGadgetListBrowser.lh_Head;
		cgy != (struct ControlBarGadgetEntry *) &ControlBarGadgetListBrowser.lh_Tail;
		cgy = (struct ControlBarGadgetEntry *) cgy->cgy_Node.ln_Succ)
		{
		DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE],
			MUIM_NList_InsertSingle,
			cgy, MUIV_NList_Insert_Bottom);
		}

	set(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_NList_Redraw, MUIV_NList_Redraw_All);
	}

	{
	struct ControlBarGadgetEntry *cgy;

	set(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_Clear);

	// copy control bar gadgets to list
	for (cgy = (struct ControlBarGadgetEntry *) ControlBarGadgetListNormal.lh_Head;
		cgy != (struct ControlBarGadgetEntry *) &ControlBarGadgetListNormal.lh_Tail;
		cgy = (struct ControlBarGadgetEntry *) cgy->cgy_Node.ln_Succ)
		{
		DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE],
			MUIM_NList_InsertSingle,
			cgy, MUIV_NList_Insert_Bottom);
		}

	set(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_NList_Redraw, MUIV_NList_Redraw_All);
	}

	FillHiddenDevicesList(app);

	DoMethod(app->Obj[APPLICATION], MUIM_CallHook, &CalculateMaxRadiusHook);
}


static void FillPrefsStructures(struct SCAModule *app)
{
	CONST_STRPTR lp;
	ULONG x = 0;

	// --- Paths Page
	lp = (CONST_STRPTR) getv(app->Obj[STRING_DISKCOPY], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pDiskCopy, lp, sizeof(currentPrefs.Paths.pg_pDiskCopy));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_SCALOSHOME], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pHome, lp, sizeof(currentPrefs.Paths.pg_pHome));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_THEMES], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pThemes, lp, sizeof(currentPrefs.Paths.pg_pThemes));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_IMAGECACHE], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pImageCache, lp, sizeof(currentPrefs.Paths.pg_pImageCache));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_THUMBNAILDB], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pThumbnailDb, lp, sizeof(currentPrefs.Paths.pg_pThumbnailDb));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_FORMAT], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pFormat, lp, sizeof(currentPrefs.Paths.pg_pFormat));
	lp = (CONST_STRPTR) getv(app->Obj[STRING_SQLITE3TEMPPATH], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_SQLiteTempDir, lp, sizeof(currentPrefs.Paths.pg_SQLiteTempDir));

	// --- Startup Page
	currentPrefs.Startup.sg_bShowSplash       = getv(app->Obj[CHECK_SHOWSPLASH], MUIA_Selected);
	currentPrefs.Startup.sg_bSplashCloseDelay = getv(app->Obj[SLIDER_SPLASHCLOSE], MUIA_Numeric_Value);
	currentPrefs.Startup.sg_bWBStartupDelay   = getv(app->Obj[SLIDER_WBSDELAY], MUIA_Numeric_Value);
	lp = (CONST_STRPTR) getv(app->Obj[STRING_WBSTARTPATH], MUIA_String_Contents);
	stccpy(currentPrefs.Startup.sg_pWBStartup, lp, sizeof(currentPrefs.Startup.sg_pWBStartup));

	// --- Desktop Page
	lp = (CONST_STRPTR) getv(app->Obj[POPPH_SCREENTITLE], MUIA_Popph_Contents);
	stccpy(currentPrefs.Desktop.dg_pScreen, lp, sizeof(currentPrefs.Desktop.dg_pScreen));

	lp = (CONST_STRPTR) getv(app->Obj[STRING_CONSOLENAME], MUIA_String_Contents);
	stccpy(currentPrefs.Desktop.dg_ConsoleName, lp, sizeof(currentPrefs.Desktop.dg_ConsoleName));


	currentPrefs.Desktop.dg_bTitleRefresh 	= getv(app->Obj[SLIDER_TITLEREFRESH], MUIA_Numeric_Value);
	currentPrefs.Desktop.dg_bMemoryRefresh 	= getv(app->Obj[CHECK_TITLEMEM], MUIA_Selected);

	currentPrefs.Desktop.dg_bWinTitleRefresh 	= getv(app->Obj[SLIDER_WINTITLEREFRESH], MUIA_Numeric_Value);
	currentPrefs.Desktop.dg_bWinMemoryRefresh 	= getv(app->Obj[CHECK_WINTITLEMEM], MUIA_Selected);

	currentPrefs.Desktop.dg_bDiskRefresh 	= getv(app->Obj[SLIDER_DISKREFRESH], MUIA_Numeric_Value);

	currentPrefs.Desktop.dg_ScreenTitleMode	= getv(app->Obj[CYCLE_SCREENTITLEMODE], MUIA_Cycle_Active);
	switch (currentPrefs.Desktop.dg_ScreenTitleMode)
		{
	default:
	case SCREENTITLE_Visible:
		currentPrefs.Desktop.dg_bPopTitleBar = FALSE;
		currentPrefs.Desktop.dg_bHideTitleBar = FALSE;
		break;
	case SCREENTITLE_Popup:
		currentPrefs.Desktop.dg_bPopTitleBar = TRUE;
		currentPrefs.Desktop.dg_bHideTitleBar = TRUE;
		break;
	case SCREENTITLE_Hidden:
		currentPrefs.Desktop.dg_bPopTitleBar = FALSE;
		currentPrefs.Desktop.dg_bHideTitleBar = TRUE;
		break;
		}

	currentPrefs.Desktop.dg_AutoLeaveOut	= getv(app->Obj[CHECK_AUTOLEAVEOUT], MUIA_Selected);
	currentPrefs.Desktop.dg_bDropStart	= getv(app->Obj[DROPSTART], MUIA_Selected);
	currentPrefs.Desktop.dg_bAllowCloseWB	= getv(app->Obj[CLOSEWB], MUIA_Selected);

	// layout
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDISK]    = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDISK], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDRAWER]  = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDRAWER], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBTOOL]    = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBTOOL], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBPROJECT] = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBPROJECT], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBGARBAGE] = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBGARBAGE], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBDEVICE]  = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBDEVICE], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBKICK]    = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBKICK], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_DeviceWindowLayoutModes[WBAPPICON] = getv(app->Obj[CYCLE_DESKTOP_LAYOUT_WBAPPICON], MUIA_Cycle_Active);


	// --- Icons Page
	currentPrefs.Icons.ig_IconOffsets.Left		= getv(app->Obj[ICONLEFT], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_IconOffsets.Top		= getv(app->Obj[ICONRIGHT], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_IconOffsets.Width		= getv(app->Obj[ICONTOP], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_IconOffsets.Height	= getv(app->Obj[ICONBOTTOM], MUIA_Numeric_Value);

	currentPrefs.Icons.ig_wNormFrame		= getv(app->Obj[FRAME_ICONNORMAL], MUIA_MCPFrame_FrameType);
	currentPrefs.Icons.ig_wSelFrame			= getv(app->Obj[FRAME_ICONSELECTED], MUIA_MCPFrame_FrameType);
	currentPrefs.Icons.ig_wNormThumbnailFrame	= getv(app->Obj[FRAME_ICON_THUMBNAIL_NORMAL], MUIA_MCPFrame_FrameType);
	currentPrefs.Icons.ig_wSelThumbnailFrame	= getv(app->Obj[FRAME_ICON_THUMBNAIL_SELECTED], MUIA_MCPFrame_FrameType);

	currentPrefs.Icons.ig_ThumbnailOffsets.Left	     = getv(app->Obj[THUMBNAILS_LEFTBORDER], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_ThumbnailOffsets.Top	     = getv(app->Obj[THUMBNAILS_RIGHTBORDER], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_ThumbnailOffsets.Width	     = getv(app->Obj[THUMBNAILS_TOPBORDER], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_ThumbnailOffsets.Height	     = getv(app->Obj[THUMBNAILS_BOTTOMBORDER], MUIA_Numeric_Value);

	currentPrefs.Icons.ig_bClickAreaMask		= getv(app->Obj[CHECK_ICONMASK], MUIA_Selected);
	currentPrefs.Icons.ig_bNewIconPrecision		= getv(app->Obj[SLIDER_ICONRMAP_PRECISION], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_bNewIconTransparent	= getv(app->Obj[CHECK_NIMASK], MUIA_Selected);
	lp = (CONST_STRPTR) getv(app->Obj[STRING_DEFICONPATH], MUIA_String_Contents);
	stccpy(currentPrefs.Paths.pg_pDefaultIcons, lp, sizeof(currentPrefs.Paths.pg_pDefaultIcons));

	currentPrefs.Icons.ig_bLoadDefIconsFirst	= getv(app->Obj[CHECK_DEFFIRST], MUIA_Selected);
	currentPrefs.Icons.ig_bIconsSaveable		= getv(app->Obj[CHECK_ICONSSAVE], MUIA_Selected);
	currentPrefs.Icons.ig_bShowTooltips		= getv(app->Obj[CHECK_TOOLTIPS], MUIA_Selected);
	currentPrefs.Icons.ig_bTooltipDelay		= getv(app->Obj[SLIDER_TIPDELAY], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_TransparencyTooltips	= getv(app->Obj[SLIDER_ICONTOOLTIPS_TRANSPARENCY], MUIA_Numeric_Value);
	lp = (CONST_STRPTR) getv(app->Obj[STRING_TOOLTIPFONT], MUIA_String_Contents);
	stccpy(currentPrefs.Icons.ig_ToolTipFont, lp, sizeof(currentPrefs.Icons.ig_ToolTipFont));

	lp = (CONST_STRPTR) getv(app->Obj[POP_ICONFONT], MUIA_String_Contents);
	stccpy(currentPrefs.Icons.ig_IconFontDesc, lp, sizeof(currentPrefs.Icons.ig_IconFontDesc));

	currentPrefs.Icons.ig_wMultipleLines		= getv(app->Obj[CHECK_MULTILINES], MUIA_Selected);
	currentPrefs.Icons.ig_HighlightUnderMouse	= getv(app->Obj[CHECK_HILITEUNDERMOUSE], MUIA_Selected);
	currentPrefs.Icons.ig_ShowThumbnails		= getv(app->Obj[CYCLE_SHOWTHUMBNAILS], MUIA_Cycle_Active);

	currentPrefs.Icons.ig_ShowThumbnailsAsDefault	= getv(app->Obj[CHECK_SHOWTHUMBNAILS_AS_DEFAULT], MUIA_Selected);
	currentPrefs.Icons.ig_bSquareThumbnails		= getv(app->Obj[CHECK_THUMBNAILS_SQUARE], MUIA_Selected);
	currentPrefs.Icons.ig_ThumbnailsBackfill	= getv(app->Obj[CHECK_THUMBNAILS_BACKFILL], MUIA_Selected);
	currentPrefs.Icons.ig_ThumbnailBackgroundTransparency = getv(app->Obj[SLIDER_ICONTRANSPARENCY_THUMBNAILBACK], MUIA_Numeric_Value);

	currentPrefs.Icons.ig_SelectedTextRectangle     = getv(app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE], MUIA_Selected);
	currentPrefs.Icons.ig_SelTextRectBorderX        = getv(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERX], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_SelTextRectBorderY        = getv(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_SelTextRectRadius         = getv(app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS], MUIA_Numeric_Value);

	currentPrefs.Icons.ig_ThumbnailMaxAge		= getv(app->Obj[SLIDER_THUMBNAILS_MAXAGE], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_ThumbnailMinSizeLimit	= getv(app->Obj[SLIDER_THUMBNAILS_MINSIZE_LIMIT], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_ThumbnailQuality		= getv(app->Obj[SLIDER_THUMBNAILS_QUALITY], MUIA_Numeric_Value);

	currentPrefs.Icons.ig_ThumbnailSize = GetThumbNailSize(app);
	GetIconSizeConstraints(app, &currentPrefs.Icons.ig_SizeConstraints);

	currentPrefs.Icons.ig_NominalSize        	= getv(app->Obj[SLIDER_ICONSPAGE_NOMINALSIZE], MUIA_Numeric_Value);

	// layout
	currentPrefs.Icons.ig_IconWindowLayoutModes[WBDRAWER]    = getv(app->Obj[CYCLE_ICONS_LAYOUT_WBDRAWER], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_IconWindowLayoutModes[WBTOOL]      = getv(app->Obj[CYCLE_ICONS_LAYOUT_WBTOOL], MUIA_Cycle_Active);
	currentPrefs.Icons.ig_IconWindowLayoutModes[WBPROJECT]   = getv(app->Obj[CYCLE_ICONS_LAYOUT_WBPROJECT], MUIA_Cycle_Active);

	//tooltip display field
	currentPrefs.DragNDrop.ddg_bBobStyle			= getv(app->Obj[CYCLE_STYLE], MUIA_Cycle_Active);
	currentPrefs.DragNDrop.ddg_bBobMethod			= getv(app->Obj[CYCLE_ROUTINE], MUIA_Cycle_Active);
	currentPrefs.DragNDrop.ddg_bBobLook			= getv(app->Obj[RADIO_BOBLOOK], MUIA_Radio_Active);
	currentPrefs.DragNDrop.ddg_bTranspType			= getv(app->Obj[CYCLE_TRANSPMODE], MUIA_Cycle_Active);
	currentPrefs.DragNDrop.ddg_bDropMarkMode		= getv(app->Obj[CYCLE_DROPMARK], MUIA_Cycle_Active);
	currentPrefs.DragNDrop.ddg_TransparencyIconDrag		= getv(app->Obj[SLIDER_ICONTRANSPARENCY_DRAG], MUIA_Numeric_Value);
	currentPrefs.DragNDrop.ddg_TransparencyIconShadow	= getv(app->Obj[SLIDER_ICONTRANSPARENCY_SHADOW], MUIA_Numeric_Value);
	currentPrefs.Icons.ig_TransparencyDefaultIcons		= getv(app->Obj[SLIDER_ICONTRANSPARENCY_DEFICONS], MUIA_Numeric_Value);

	currentPrefs.DragNDrop.ddg_bAutoRemoveIcons		= getv(app->Obj[CHECK_AUTOREMOVE], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_bEasyMultiSelect		= getv(app->Obj[CHECK_EASY_MULTISELECT], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_bEasyMultiDrag		= getv(app->Obj[CHECK_EASY_MULTIDRAG], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_bGroupDrag			= !getv(app->Obj[CHECK_GROUPDRAG], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_bShowObjectCount		= getv(app->Obj[CHECK_SHOWDRAG], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_bEnableDropMenu		= getv(app->Obj[CHECK_ENABLE_DROPMENU], MUIA_Selected);
	currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds	= getv(app->Obj[SLIDER_POPOPENWINDOW_DELAY], MUIA_Numeric_Value);

	//copy qualifier
	lp = (CONST_STRPTR) getv(app->Obj[COPY_HOTKEY], MUIA_String_Contents);
	stccpy(currentPrefs.DragNDrop.ddg_CopyQualifierString, lp, sizeof(currentPrefs.DragNDrop.ddg_CopyQualifierString));
	currentPrefs.DragNDrop.ddg_pCopyQualifier = TranslateStringToQualifier(currentPrefs.DragNDrop.ddg_CopyQualifierString);
	//MakeLink qualifier
	lp = (CONST_STRPTR) getv(app->Obj[MAKELINK_HOTKEY], MUIA_String_Contents);
	stccpy(currentPrefs.DragNDrop.ddg_MakeLinkQualifierString, lp, sizeof(currentPrefs.DragNDrop.ddg_MakeLinkQualifierString));
	currentPrefs.DragNDrop.ddg_pMakeLinkQualifier = TranslateStringToQualifier(currentPrefs.DragNDrop.ddg_MakeLinkQualifierString);
	//Move qualifier
	lp = (CONST_STRPTR) getv(app->Obj[MOVE_HOTKEY], MUIA_String_Contents);
	stccpy(currentPrefs.DragNDrop.ddg_MoveQualifierString, lp, sizeof(currentPrefs.DragNDrop.ddg_MoveQualifierString));
	currentPrefs.DragNDrop.ddg_pMoveQualifier = TranslateStringToQualifier(currentPrefs.DragNDrop.ddg_MoveQualifierString);

	//Single-Window Lasso qualifier
	lp = (CONST_STRPTR) getv(app->Obj[SINGLE_WINDOW_LASSO_HOTKEY], MUIA_String_Contents);
	stccpy(currentPrefs.Desktop.dg_SingleWindowLassoQualifierString, lp, sizeof(currentPrefs.Desktop.dg_SingleWindowLassoQualifierString));
	currentPrefs.Desktop.dg_SingleWindowLassoQualifier = TranslateStringToQualifier(currentPrefs.Desktop.dg_SingleWindowLassoQualifierString);

	//triggers
	currentPrefs.DragNDrop.ddg_lTriggers = 0;
	get(app->Obj[CHECK_TRIG_DISK], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_DiskIcons;
	get(app->Obj[CHECK_TRIG_DRAWER], MUIA_Selected, &x); 
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_DrawerIcons;
	get(app->Obj[CHECK_TRIG_TOOL], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_ToolIcons;
	get(app->Obj[CHECK_TRIG_PROJECT], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_ProjectIcons;
	get(app->Obj[CHECK_TRIG_TRASHCAN], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_TrashcanIcons;
	get(app->Obj[CHECK_TRIG_KICK], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_KickIcons;
	get(app->Obj[CHECK_TRIG_APPICON], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_AppIcons;
	get(app->Obj[CHECK_TRIG_APPWIN], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_AppWindows;
	get(app->Obj[CHECK_TRIG_ICONIFIEDWIN], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_IconifiedWinIcons;
	get(app->Obj[CHECK_TRIG_SCALOSWIN], MUIA_Selected, &x);
	if (x)
		currentPrefs.DragNDrop.ddg_lTriggers |= DRAGTRANSPF_ScalosWindows;

	// --- Windows Page
	lp = (CONST_STRPTR) getv(app->Obj[POPPH_ROOTWINDOWTITLE], MUIA_Popph_Contents);
	stccpy(currentPrefs.Windows.wg_pRootWin, lp, sizeof(currentPrefs.Windows.wg_pRootWin));
	lp = (CONST_STRPTR) getv(app->Obj[POPPH_DIRWINDOWTITLE], MUIA_Popph_Contents);
	stccpy(currentPrefs.Windows.wg_pWindow, lp, sizeof(currentPrefs.Windows.wg_pWindow));

	currentPrefs.Windows.wg_bType                   = getv(app->Obj[CYCLE_WINDOWTYPE], MUIA_Cycle_Active);
	currentPrefs.Windows.wg_bPopTitleOnly           = getv(app->Obj[CHECK_POPTITLEONLY], MUIA_Selected);
	currentPrefs.Windows.wg_bMMBMove                = getv(app->Obj[CHECK_MMBMOVE], MUIA_Selected);
	currentPrefs.Windows.wg_DefaultSize.Left     	= getv(app->Obj[WINLEFT], MUIA_String_Integer);
	currentPrefs.Windows.wg_DefaultSize.Top      	= getv(app->Obj[WINTOP], MUIA_String_Integer);
	currentPrefs.Windows.wg_DefaultSize.Width    	= getv(app->Obj[WINWIDTH], MUIA_String_Integer);
	currentPrefs.Windows.wg_DefaultSize.Height   	= getv(app->Obj[WINHEIGHT], MUIA_String_Integer);
	currentPrefs.Windows.wg_CleanupSpaces.Left   	= getv(app->Obj[SLIDER_CLEANLEFT], MUIA_Numeric_Value);
	currentPrefs.Windows.wg_CleanupSpaces.Top    	= getv(app->Obj[SLIDER_CLEANTOP], MUIA_Numeric_Value);
	currentPrefs.Windows.wg_CleanupSpaces.Width  	= getv(app->Obj[SLIDER_CLEANXSKIP], MUIA_Numeric_Value);
	currentPrefs.Windows.wg_CleanupSpaces.Height 	= getv(app->Obj[SLIDER_CLEANYSKIP], MUIA_Numeric_Value);
	currentPrefs.Windows.wg_bStatusBar		= getv(app->Obj[CHECK_STATUSBAR], MUIA_Selected);
	currentPrefs.Windows.wg_CheckOverlappingIcons	= getv(app->Obj[CHECK_CHECKOVERLAP], MUIA_Selected);
	currentPrefs.Windows.wg_TransparencyActiveWindow	= getv(app->Obj[SLIDER_TRANSPARENCY_ACTIVEWINDOW], MUIA_Numeric_Value);
	currentPrefs.Windows.wg_wTransparencyInactiveWindow	= getv(app->Obj[SLIDER_TRANSPARENCY_INACTIVEWINDOW], MUIA_Numeric_Value);
        
	currentPrefs.Windows.wg_bCleanupOnResize	= getv(app->Obj[CHECK_CLEANUP_ONRESIZE], MUIA_Selected);
	currentPrefs.Windows.wg_bHideProtectHiddenFiles	= getv(app->Obj[HIDEPROTECTHIDDENFILES], MUIA_Selected); // JMC
	currentPrefs.Windows.wg_bHideHiddenFiles    	= getv(app->Obj[HIDEHIDDENFILES], MUIA_Selected);
	currentPrefs.Windows.wg_ShowAllByDefault	= getv(app->Obj[CYCLE_SHOWALLDEFAULT], MUIA_Cycle_Active);
	currentPrefs.Windows.wg_ViewByDefault		= 1 + getv(app->Obj[CYCLE_VIEWBYICONSDEFAULT], MUIA_Cycle_Active);


	// --- File Display Page
	currentPrefs.TextWindows.fd_wLabelStyle         = getv(app->Obj[CYCLE_LABELSTYLE], MUIA_Cycle_Active);
	currentPrefs.TextWindows.fd_wTextSkip           = getv(app->Obj[SLIDER_LABELSPACE], MUIA_Numeric_Value);
	currentPrefs.TextWindows.fd_bShowSoftLinks      = getv(app->Obj[SOFTICONSLINK], MUIA_Selected);
	lp = (CONST_STRPTR) getv(app->Obj[POP_TEXTMODEFONT], MUIA_String_Contents);
	stccpy(currentPrefs.TextWindows.fd_TextWindowFontDescr, lp, sizeof(currentPrefs.TextWindows.fd_TextWindowFontDescr));
	//currentPrefs.TextWindows.fd_lDisplayFields = getv(app->Obj[], MUIA_);
	currentPrefs.TextWindows.fd_bShowStripes	= getv(app->Obj[CHECK_STRIPED_WINDOW], MUIA_Selected);
	currentPrefs.TextWindows.fd_SelectTextIconName	= getv(app->Obj[CHECK_SELECTTEXTICONNAME], MUIA_Selected);
	currentPrefs.TextWindows.fd_DrawerSortMode    	= getv(app->Obj[CYCLE_DRAWERSORTMODE], MUIA_Cycle_Active);

	currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Red   = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Red) >> 24;
	currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Green = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Green) >> 24;
	currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Blue  = getv(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIA_Coloradjust_Blue) >> 24;
	currentPrefs.TextWindows.fd_SelectMarkerTransparency    =  getv(app->Obj[SLIDER_TEXTWINDOWS_SELECTFILLTRANSPARENCY], MUIA_Numeric_Value);
	currentPrefs.TextWindows.fd_SelectMarkerBaseColor.Alpha =  getv(app->Obj[SLIDER_TEXTWINDOWS_SELECTBORDERTRANSPARENCY], MUIA_Numeric_Value);

	UpdateFileDisplayPrefsFromGUI(app);

	// --- TrueType Font Page
	currentPrefs.TrueTypeFonts.ttg_Antialias	   = getv(app->Obj[CYCLE_TTANTIALIAS], MUIA_Cycle_Active);
	currentPrefs.TrueTypeFonts.ttg_Gamma		   = getv(app->Obj[SLIDER_TTGAMMA], MUIA_Numeric_Value);
	currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont     = getv(app->Obj[CHECK_TTSCREENFONT_ENABLE], MUIA_Selected);
	currentPrefs.TrueTypeFonts.ttg_UseIconTTFont       = getv(app->Obj[CHECK_TTICONFONT_ENABLE], MUIA_Selected);
	currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont = getv(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIA_Selected);
	lp = (CONST_STRPTR) getv(app->Obj[POPSTRING_TTSCREENFONT], MUIA_String_Contents);
	stccpy(currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc, lp, sizeof(currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc));
	lp = (CONST_STRPTR) getv(app->Obj[POPSTRING_TTICONFONT], MUIA_String_Contents);
	stccpy(currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc, lp, sizeof(currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc));
	lp = (CONST_STRPTR) getv(app->Obj[POPSTRING_TTTEXTWINDOWFONT], MUIA_String_Contents);
	stccpy(currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc, lp, sizeof(currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc));

	// --- Misc Page
	currentPrefs.Miscellaneous.mg_bMenuCurrentDir   = getv(app->Obj[CHECK_MENUCURRENTDIR], MUIA_Selected);
	currentPrefs.Miscellaneous.mg_bHardEmulation    = getv(app->Obj[CHECK_HARDEMULATION], MUIA_Selected);
	currentPrefs.Miscellaneous.mg_bUseExAll         = getv(app->Obj[CHECK_USEEXALL], MUIA_Selected);
	currentPrefs.Miscellaneous.mg_bCreateSoftLinks 	= getv(app->Obj[CYCLE_CREATELINKS], MUIA_Cycle_Active);
	currentPrefs.Miscellaneous.mg_DefaultStackSize	= 1024 * getv(app->Obj[SLIDER_DEFAULTSTACKSIZE], MUIA_Slider_Level);
	currentPrefs.Miscellaneous.mg_CopyBuffSize  	= 1 << getv(app->Obj[SLIDER_COPYBUFFERSIZE], MUIA_Slider_Level);
	currentPrefs.Miscellaneous.mg_MaxUndoSteps 	= getv(app->Obj[SLIDER_UNDOSTEPS], MUIA_Slider_Level);

	//Popup menu apply selected qualifier
	lp = (CONST_STRPTR) getv(app->Obj[POPUP_SELECTED_HOTKEY], MUIA_String_Contents);
	stccpy(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString, 
		lp, sizeof(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString));
	currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier = 
		TranslateStringToQualifier(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifierString);
	currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways = getv(app->Obj[CHECK_MISCPAGE_POPUP_SELECTED_ALWAYS], MUIA_Selected);

	// --- Plugins Page
}

LONG WriteScalosPrefs(struct SCAModule *app, CONST_STRPTR PrefsFileName)
{
	LONG lID;
	APTR p_MyPrefsHandle;
//	STRPTR PrefsFileName = "ram:scalos.prefs";

	// temp variables for big endian writes
	struct IBox ibox;
	struct Rectangle rect;

	FillPrefsStructures(app);

	p_MyPrefsHandle = AllocPrefsHandle("ScalosPrefs");
	lID = MAKE_ID('M', 'A', 'I', 'N');

	if(p_MyPrefsHandle)
		{
		ibox.Left = SCA_WORD2BE(currentPrefs.Icons.ig_IconOffsets.Left);
		ibox.Top = SCA_WORD2BE(currentPrefs.Icons.ig_IconOffsets.Top);
		ibox.Width = SCA_WORD2BE(currentPrefs.Icons.ig_IconOffsets.Width);
		ibox.Height = SCA_WORD2BE(currentPrefs.Icons.ig_IconOffsets.Height);
		SetPreferences(p_MyPrefsHandle, lID, SCP_IconOffsets, &ibox, sizeof(ibox) );

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconNormFrame, &currentPrefs.Icons.ig_wNormFrame, sizeof(currentPrefs.Icons.ig_wNormFrame) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconSelFrame, &currentPrefs.Icons.ig_wSelFrame, sizeof(currentPrefs.Icons.ig_wSelFrame) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconNormThumbnailFrame, &currentPrefs.Icons.ig_wNormThumbnailFrame, sizeof(currentPrefs.Icons.ig_wNormThumbnailFrame) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconSelThumbnailFrame, &currentPrefs.Icons.ig_wSelThumbnailFrame, sizeof(currentPrefs.Icons.ig_wSelThumbnailFrame) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconTextMode, &currentPrefs.TextWindows.fd_wLabelStyle, sizeof(currentPrefs.TextWindows.fd_wLabelStyle) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_IconSecLine, &currentPrefs.Icons.ig_wMultipleLines, sizeof(currentPrefs.Icons.ig_wMultipleLines) );
		// BYTE

		rect.MinX = SCA_WORD2BE(currentPrefs.Icons.ig_SizeConstraints.MinX);
		rect.MinY = SCA_WORD2BE(currentPrefs.Icons.ig_SizeConstraints.MinY);
		rect.MaxX = SCA_WORD2BE(currentPrefs.Icons.ig_SizeConstraints.MaxX);
		rect.MaxY = SCA_WORD2BE(currentPrefs.Icons.ig_SizeConstraints.MaxY);
		SetPreferences(p_MyPrefsHandle, lID, SCP_IconSizeConstraints, &rect, sizeof(rect) );

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconNominalSize, &currentPrefs.Icons.ig_NominalSize, sizeof(currentPrefs.Icons.ig_NominalSize) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_IconHiliteUnderMouse, &currentPrefs.Icons.ig_HighlightUnderMouse, sizeof(currentPrefs.Icons.ig_HighlightUnderMouse) );
		// BYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_IconTextSkip, &currentPrefs.TextWindows.fd_wTextSkip, sizeof(currentPrefs.TextWindows.fd_wTextSkip) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_ShowThumbnails, &currentPrefs.Icons.ig_ShowThumbnails, sizeof(currentPrefs.Icons.ig_ShowThumbnails) );
		// UBYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ShowThumbnailsAsDefault, &currentPrefs.Icons.ig_ShowThumbnailsAsDefault, sizeof(currentPrefs.Icons.ig_ShowThumbnailsAsDefault) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailsSquare, &currentPrefs.Icons.ig_bSquareThumbnails, sizeof(currentPrefs.Icons.ig_bSquareThumbnails) );
		// BYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailSize, &currentPrefs.Icons.ig_ThumbnailSize, sizeof(currentPrefs.Icons.ig_ThumbnailSize) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailMaxAge, &currentPrefs.Icons.ig_ThumbnailMaxAge, sizeof(currentPrefs.Icons.ig_ThumbnailMaxAge) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailMinSizeLimit, &currentPrefs.Icons.ig_ThumbnailMinSizeLimit, sizeof(currentPrefs.Icons.ig_ThumbnailMinSizeLimit) );

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailQuality, &currentPrefs.Icons.ig_ThumbnailQuality, sizeof(currentPrefs.Icons.ig_ThumbnailQuality) );

		ibox.Left = SCA_WORD2BE(currentPrefs.Icons.ig_ThumbnailOffsets.Left);
		ibox.Top = SCA_WORD2BE(currentPrefs.Icons.ig_ThumbnailOffsets.Top);
		ibox.Width = SCA_WORD2BE(currentPrefs.Icons.ig_ThumbnailOffsets.Width);
		ibox.Height = SCA_WORD2BE(currentPrefs.Icons.ig_ThumbnailOffsets.Height);
		SetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailOffsets, &ibox, sizeof(ibox) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailsFillBackground, &currentPrefs.Icons.ig_ThumbnailsBackfill, sizeof(currentPrefs.Icons.ig_ThumbnailsBackfill) );
		// UBYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_TransparencyThumbnailsBack, &currentPrefs.Icons.ig_ThumbnailBackgroundTransparency, sizeof(currentPrefs.Icons.ig_ThumbnailBackgroundTransparency) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_SelectedTextRectangle, &currentPrefs.Icons.ig_SelectedTextRectangle, sizeof(currentPrefs.Icons.ig_SelectedTextRectangle) );
		// UBYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectBorderX, &currentPrefs.Icons.ig_SelTextRectBorderX, sizeof(currentPrefs.Icons.ig_SelTextRectBorderX) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectBorderY, &currentPrefs.Icons.ig_SelTextRectBorderY, sizeof(currentPrefs.Icons.ig_SelTextRectBorderY) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectRadius, &currentPrefs.Icons.ig_SelTextRectRadius, sizeof(currentPrefs.Icons.ig_SelTextRectRadius) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_DeviceWinIconLayout, currentPrefs.Icons.ig_DeviceWindowLayoutModes, sizeof(currentPrefs.Icons.ig_DeviceWindowLayoutModes));
		// UBYTE[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_IconWinIconLayout, currentPrefs.Icons.ig_IconWindowLayoutModes, sizeof(currentPrefs.Icons.ig_IconWindowLayoutModes));
		// UBYTE[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_BobsType, &currentPrefs.DragNDrop.ddg_bBobStyle, sizeof(currentPrefs.DragNDrop.ddg_bBobStyle) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_BobsMethod, &currentPrefs.DragNDrop.ddg_bBobMethod, sizeof(currentPrefs.DragNDrop.ddg_bBobMethod) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_BobsTranspMode, &currentPrefs.DragNDrop.ddg_bBobLook, sizeof(currentPrefs.DragNDrop.ddg_bBobLook) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_BobsTranspType, &currentPrefs.DragNDrop.ddg_bTranspType, sizeof(currentPrefs.DragNDrop.ddg_bTranspType) );
		// BYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_BobsTransp, &currentPrefs.DragNDrop.ddg_lTriggers, sizeof(currentPrefs.DragNDrop.ddg_lTriggers) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_ScreenTitle, currentPrefs.Desktop.dg_pScreen, 1 + strlen(currentPrefs.Desktop.dg_pScreen) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_RootWinTitle, currentPrefs.Windows.wg_pRootWin, 1 + strlen(currentPrefs.Windows.wg_pRootWin) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_WindowTitle, currentPrefs.Windows.wg_pWindow, 1 + strlen(currentPrefs.Windows.wg_pWindow) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_TTfAntialiasing, &currentPrefs.TrueTypeFonts.ttg_Antialias, sizeof(currentPrefs.TrueTypeFonts.ttg_Antialias) );
		// BYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_TTfGamma, &currentPrefs.TrueTypeFonts.ttg_Gamma, sizeof(currentPrefs.TrueTypeFonts.ttg_Gamma) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_TTScreenFontDesc, currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc, 1 + strlen(currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_TTIconFontDesc, currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc, 1 + strlen(currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_TTTextWindowFontDesc, currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc, 1 + strlen(currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_UseTTScreenFont, &currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_UseTTIconFont, &currentPrefs.TrueTypeFonts.ttg_UseIconTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseIconTTFont) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_UseTTTextWindowFont, &currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont) );
		// BYTE

		//SetPreferences(p_MyPrefsHandle, lID, SCP_Seperator, , sizeof() );

		SetPreferences(p_MyPrefsHandle, lID, SCP_TitleRefresh, &currentPrefs.Desktop.dg_bTitleRefresh, sizeof(currentPrefs.Desktop.dg_bTitleRefresh) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_RefreshOnMemoryChange, &currentPrefs.Desktop.dg_bMemoryRefresh, sizeof(currentPrefs.Desktop.dg_bMemoryRefresh) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_WinTitleRefresh, &currentPrefs.Desktop.dg_bWinTitleRefresh, sizeof(currentPrefs.Desktop.dg_bWinTitleRefresh) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_WinRefreshOnMemoryChange, &currentPrefs.Desktop.dg_bWinMemoryRefresh, sizeof(currentPrefs.Desktop.dg_bWinMemoryRefresh) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ConsoleName, currentPrefs.Desktop.dg_ConsoleName, 1 + strlen(currentPrefs.Desktop.dg_ConsoleName) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsDefIcons, &currentPrefs.Paths.pg_pDefaultIcons, 1 + strlen(currentPrefs.Paths.pg_pDefaultIcons) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsDiskCopy, &currentPrefs.Paths.pg_pDiskCopy, 1 + strlen(currentPrefs.Paths.pg_pDiskCopy) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsTheme, &currentPrefs.Paths.pg_pThemes, 1 + strlen(currentPrefs.Paths.pg_pThemes) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsImagecache, &currentPrefs.Paths.pg_pImageCache, 1 + strlen(currentPrefs.Paths.pg_pImageCache) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsThumbnailDb, &currentPrefs.Paths.pg_pThumbnailDb, 1 + strlen(currentPrefs.Paths.pg_pThumbnailDb) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsWBStartup, &currentPrefs.Startup.sg_pWBStartup, 1 + strlen(currentPrefs.Startup.sg_pWBStartup) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsHome, &currentPrefs.Paths.pg_pHome, 1 + strlen(currentPrefs.Paths.pg_pHome) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_PathsSQLiteTempDir, &currentPrefs.Paths.pg_SQLiteTempDir, 1 + strlen(currentPrefs.Paths.pg_SQLiteTempDir) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscAutoRemove, &currentPrefs.DragNDrop.ddg_bAutoRemoveIcons, sizeof(currentPrefs.DragNDrop.ddg_bAutoRemoveIcons) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscClickTransp, &currentPrefs.Icons.ig_bClickAreaMask, sizeof(currentPrefs.Icons.ig_bClickAreaMask) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscHardEmulation, &currentPrefs.Miscellaneous.mg_bHardEmulation, sizeof(currentPrefs.Miscellaneous.mg_bHardEmulation) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscUseExAll, &currentPrefs.Miscellaneous.mg_bUseExAll, sizeof(currentPrefs.Miscellaneous.mg_bUseExAll) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_CreateSoftLinks, &currentPrefs.Miscellaneous.mg_bCreateSoftLinks, sizeof(currentPrefs.Miscellaneous.mg_bCreateSoftLinks) );
		// BYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_CopyBuffLen, &currentPrefs.Miscellaneous.mg_CopyBuffSize, sizeof(currentPrefs.Miscellaneous.mg_CopyBuffSize) );
		SetLongPreferences(p_MyPrefsHandle, lID, SCP_MaxUndoSteps, &currentPrefs.Miscellaneous.mg_MaxUndoSteps, sizeof(currentPrefs.Miscellaneous.mg_MaxUndoSteps) );

		if (WorkbenchBase->lib_Version < 45)
			SetLongPreferences(p_MyPrefsHandle, lID, SCP_DefaultStackSize, &currentPrefs.Miscellaneous.mg_DefaultStackSize, sizeof(currentPrefs.Miscellaneous.mg_DefaultStackSize) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscWindowType, &currentPrefs.Windows.wg_bType, sizeof(currentPrefs.Windows.wg_bType) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscDoWaitDelay, &currentPrefs.Startup.sg_bWBStartupDelay, sizeof(currentPrefs.Startup.sg_bWBStartupDelay) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscDiskiconsRefresh, &currentPrefs.Desktop.dg_bDiskRefresh, sizeof(currentPrefs.Desktop.dg_bDiskRefresh) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_MiscMenuCurrentDir, &currentPrefs.Miscellaneous.mg_bMenuCurrentDir, sizeof(currentPrefs.Miscellaneous.mg_bMenuCurrentDir) );
		// BYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_DragDropCopyQualifier, &currentPrefs.DragNDrop.ddg_pCopyQualifier, sizeof(currentPrefs.DragNDrop.ddg_pCopyQualifier) );
		SetLongPreferences(p_MyPrefsHandle, lID, SCP_PopupApplySelectedQualifier, &currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier, sizeof(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_PopupApplySelectedAlways, &currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways, sizeof(currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways) );
		// BYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_DragDropMakeLinkQualifier, &currentPrefs.DragNDrop.ddg_pMakeLinkQualifier, sizeof(currentPrefs.DragNDrop.ddg_pMakeLinkQualifier) );
		SetLongPreferences(p_MyPrefsHandle, lID, SCP_DragDropMoveQualifier, &currentPrefs.DragNDrop.ddg_pMoveQualifier, sizeof(currentPrefs.DragNDrop.ddg_pMoveQualifier) );
		SetLongPreferences(p_MyPrefsHandle, lID, SCP_SingleWindowLassoQualifier, &currentPrefs.Desktop.dg_SingleWindowLassoQualifier, sizeof(currentPrefs.Desktop.dg_SingleWindowLassoQualifier) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_NewIconsTransparent, &currentPrefs.Icons.ig_bNewIconTransparent, sizeof(currentPrefs.Icons.ig_bNewIconTransparent) );
		// BYTE

		WriteNewIconsPrecision(p_MyPrefsHandle, lID);

		//SetPreferences(p_MyPrefsHandle, lID, SCP_TextModeDateFormat, , sizeof() );
		WriteTextModeColumns(p_MyPrefsHandle, lID);

		WritePluginList(p_MyPrefsHandle, lID);

		WriteControlBarGadgetList(app, p_MyPrefsHandle, lID,
			NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE, SCP_ControlBarGadgetsStd, SCP_ControlBarGadgetStringsStd);
		WriteControlBarGadgetList(app, p_MyPrefsHandle, lID,
			NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE, SCP_ControlBarGadgetsBr, SCP_ControlBarGadgetStringsBr);

		SetPreferences(p_MyPrefsHandle, lID, SCP_MMB_Move, &currentPrefs.Windows.wg_bMMBMove, sizeof(currentPrefs.Windows.wg_bMMBMove) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_WindowPopupTitleOnly, &currentPrefs.Windows.wg_bPopTitleOnly, sizeof(currentPrefs.Windows.wg_bPopTitleOnly) );
		// BYTE

		ibox.Left = SCA_WORD2BE(currentPrefs.Windows.wg_DefaultSize.Left);
		ibox.Top = SCA_WORD2BE(currentPrefs.Windows.wg_DefaultSize.Top);
		ibox.Width = SCA_WORD2BE(currentPrefs.Windows.wg_DefaultSize.Width);
		ibox.Height = SCA_WORD2BE(currentPrefs.Windows.wg_DefaultSize.Height);
		SetPreferences(p_MyPrefsHandle, lID, SCP_DefaultWindowSize, &ibox, sizeof(ibox) );

		ibox.Left = SCA_WORD2BE(currentPrefs.Windows.wg_CleanupSpaces.Left);
		ibox.Top = SCA_WORD2BE(currentPrefs.Windows.wg_CleanupSpaces.Top);
		ibox.Width = SCA_WORD2BE(currentPrefs.Windows.wg_CleanupSpaces.Width);
		ibox.Height = SCA_WORD2BE(currentPrefs.Windows.wg_CleanupSpaces.Height);
		SetPreferences(p_MyPrefsHandle, lID, SCP_IconCleanupSpace, &ibox, sizeof(ibox) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_FullBench, &currentPrefs.Desktop.dg_bHideTitleBar, sizeof(currentPrefs.Desktop.dg_bHideTitleBar) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_LoadDefIconsFirst, &currentPrefs.Icons.ig_bLoadDefIconsFirst, sizeof(currentPrefs.Icons.ig_bLoadDefIconsFirst) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_DefaultIconsSaveable, &currentPrefs.Icons.ig_bIconsSaveable, sizeof(currentPrefs.Icons.ig_bIconsSaveable) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_EasyMultiSelect, &currentPrefs.DragNDrop.ddg_bEasyMultiSelect, sizeof(currentPrefs.DragNDrop.ddg_bEasyMultiSelect) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_EasyMultiDrag, &currentPrefs.DragNDrop.ddg_bEasyMultiDrag, sizeof(currentPrefs.DragNDrop.ddg_bEasyMultiDrag) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_DropStart, &currentPrefs.Desktop.dg_bDropStart, sizeof(currentPrefs.Desktop.dg_bDropStart) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_AutoLeaveOut, &currentPrefs.Desktop.dg_AutoLeaveOut, sizeof(currentPrefs.Desktop.dg_AutoLeaveOut));
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_HideHiddenFiles, &currentPrefs.Windows.wg_bHideHiddenFiles, sizeof(currentPrefs.Windows.wg_bHideHiddenFiles) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_HideProtectHiddenFiles, &currentPrefs.Windows.wg_bHideProtectHiddenFiles, sizeof(currentPrefs.Windows.wg_bHideProtectHiddenFiles) ); // JMC
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ShowAllDefault, &currentPrefs.Windows.wg_ShowAllByDefault, sizeof(currentPrefs.Windows.wg_ShowAllByDefault) );
		// UBYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ViewByDefault, &currentPrefs.Windows.wg_ViewByDefault, sizeof(currentPrefs.Windows.wg_ViewByDefault) );
		// UBYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_CheckOverlappingIcons, &currentPrefs.Windows.wg_CheckOverlappingIcons, sizeof(currentPrefs.Windows.wg_CheckOverlappingIcons) );
		// UBYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_ActiveWindowTransparency, &currentPrefs.Windows.wg_TransparencyActiveWindow, sizeof(currentPrefs.Windows.wg_TransparencyActiveWindow) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_InactiveWindowTransparency, &currentPrefs.Windows.wg_wTransparencyInactiveWindow, sizeof(currentPrefs.Windows.wg_wTransparencyInactiveWindow) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_TextModeFont, &currentPrefs.TextWindows.fd_TextWindowFontDescr, 1 + strlen(currentPrefs.TextWindows.fd_TextWindowFontDescr) );
		// char[]

		SetPreferences(p_MyPrefsHandle, lID, SCP_UnderSoftLinkNames, &currentPrefs.TextWindows.fd_bShowSoftLinks, sizeof(currentPrefs.TextWindows.fd_bShowSoftLinks) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_PopScreenTitle, &currentPrefs.Desktop.dg_bPopTitleBar, sizeof(currentPrefs.Desktop.dg_bPopTitleBar) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_SplashWindowEnable, &currentPrefs.Startup.sg_bShowSplash, sizeof(currentPrefs.Startup.sg_bShowSplash) );
		// BYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_SplashWindowHoldTime, &currentPrefs.Startup.sg_bSplashCloseDelay, sizeof(currentPrefs.Startup.sg_bSplashCloseDelay) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_StatusBarEnable, &currentPrefs.Windows.wg_bStatusBar, sizeof(currentPrefs.Windows.wg_bStatusBar) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_CleanupOnResize, &currentPrefs.Windows.wg_bCleanupOnResize, sizeof(currentPrefs.Windows.wg_bCleanupOnResize) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_ToolTipsEnable, &currentPrefs.Icons.ig_bShowTooltips, sizeof(currentPrefs.Icons.ig_bShowTooltips) );
		// BYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_ToolTipPopupDelay, &currentPrefs.Icons.ig_bTooltipDelay, sizeof(currentPrefs.Icons.ig_bTooltipDelay) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_ToolTipTransparency, &currentPrefs.Icons.ig_TransparencyTooltips, sizeof(currentPrefs.Icons.ig_TransparencyTooltips) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_ShowDragDropObjCount, &currentPrefs.DragNDrop.ddg_bShowObjectCount, sizeof(currentPrefs.DragNDrop.ddg_bShowObjectCount) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_DragDropIconsSingle, &currentPrefs.DragNDrop.ddg_bGroupDrag, sizeof(currentPrefs.DragNDrop.ddg_bGroupDrag) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_DropMarkMode, &currentPrefs.DragNDrop.ddg_bDropMarkMode, sizeof(currentPrefs.DragNDrop.ddg_bDropMarkMode) );
		// BYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_TransparencyIconDrag, &currentPrefs.DragNDrop.ddg_TransparencyIconDrag, sizeof(currentPrefs.DragNDrop.ddg_TransparencyIconDrag) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_TransparencyIconShadow, &currentPrefs.DragNDrop.ddg_TransparencyIconShadow, sizeof(currentPrefs.DragNDrop.ddg_TransparencyIconShadow) );
		SetWordPreferences(p_MyPrefsHandle, lID, SCP_TransparencyDefaultIcon, &currentPrefs.Icons.ig_TransparencyDefaultIcons, sizeof(currentPrefs.Icons.ig_TransparencyDefaultIcons) );
		SetPreferences(p_MyPrefsHandle, lID, SCP_EnableDropMenu, &currentPrefs.DragNDrop.ddg_bEnableDropMenu, sizeof(currentPrefs.DragNDrop.ddg_bEnableDropMenu) );
		// UBYTE

		SetWordPreferences(p_MyPrefsHandle, lID, SCP_PopupWindowsDelay, &currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds, sizeof(currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds) );

		SetPreferences(p_MyPrefsHandle, lID, SCP_TextWindowsStriped, &currentPrefs.TextWindows.fd_bShowStripes, sizeof(currentPrefs.TextWindows.fd_bShowStripes) );
		// BYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_SelectTextIconName, &currentPrefs.TextWindows.fd_SelectTextIconName, sizeof(currentPrefs.TextWindows.fd_SelectTextIconName) );
		// UBYTE

		SetPreferences(p_MyPrefsHandle, lID, SCP_SelectMarkerBaseColor, &currentPrefs.TextWindows.fd_SelectMarkerBaseColor, sizeof(currentPrefs.TextWindows.fd_SelectMarkerBaseColor) );
		// struct ARGB

		SetPreferences(p_MyPrefsHandle, lID, SCP_SelectMarkerTransparency, &currentPrefs.TextWindows.fd_SelectMarkerTransparency, sizeof(currentPrefs.TextWindows.fd_SelectMarkerTransparency) );
		// UBYTE

		SetLongPreferences(p_MyPrefsHandle, lID, SCP_DrawerSortMode, (ULONG *) &currentPrefs.TextWindows.fd_DrawerSortMode, sizeof(currentPrefs.TextWindows.fd_DrawerSortMode) );
		// enum

		WritePrefsHandle(p_MyPrefsHandle, PrefsFileName);

		FreePrefsHandle(p_MyPrefsHandle);
		}

	if (fCreateIcons)
		SaveIcon(PrefsFileName);

	return RETURN_OK;
}

LONG ReadScalosPrefs(CONST_STRPTR PrefsFileName)
{
	LONG lID;
	APTR p_MyPrefsHandle;

	CleanupPluginList();

	currentPrefs = defaultPrefs;

	p_MyPrefsHandle = AllocPrefsHandle("ScalosPrefs");
	lID = MAKE_ID('M', 'A', 'I', 'N');

	if(p_MyPrefsHandle)
		{
		ReadPrefsHandle(p_MyPrefsHandle, PrefsFileName);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconOffsets, &currentPrefs.Icons.ig_IconOffsets, sizeof(currentPrefs.Icons.ig_IconOffsets) ))
			{
			currentPrefs.Icons.ig_IconOffsets.Left = SCA_BE2WORD(currentPrefs.Icons.ig_IconOffsets.Left);
			currentPrefs.Icons.ig_IconOffsets.Top = SCA_BE2WORD(currentPrefs.Icons.ig_IconOffsets.Left);
			currentPrefs.Icons.ig_IconOffsets.Width = SCA_BE2WORD(currentPrefs.Icons.ig_IconOffsets.Left);
			currentPrefs.Icons.ig_IconOffsets.Height = SCA_BE2WORD(currentPrefs.Icons.ig_IconOffsets.Left);
			}
		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconNormFrame, &currentPrefs.Icons.ig_wNormFrame, sizeof(currentPrefs.Icons.ig_wNormFrame) ))
			currentPrefs.Icons.ig_wNormFrame = SCA_BE2WORD(currentPrefs.Icons.ig_wNormFrame);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconSelFrame, &currentPrefs.Icons.ig_wSelFrame, sizeof(currentPrefs.Icons.ig_wSelFrame) ))
			currentPrefs.Icons.ig_wSelFrame = SCA_BE2WORD(currentPrefs.Icons.ig_wSelFrame);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconNormThumbnailFrame, &currentPrefs.Icons.ig_wNormThumbnailFrame, sizeof(currentPrefs.Icons.ig_wNormThumbnailFrame) ))
			currentPrefs.Icons.ig_wNormThumbnailFrame = SCA_BE2WORD(currentPrefs.Icons.ig_wNormThumbnailFrame);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconSelThumbnailFrame, &currentPrefs.Icons.ig_wSelThumbnailFrame, sizeof(currentPrefs.Icons.ig_wSelThumbnailFrame) ))
			currentPrefs.Icons.ig_wSelThumbnailFrame = SCA_BE2WORD(currentPrefs.Icons.ig_wSelThumbnailFrame);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconTextMode, &currentPrefs.TextWindows.fd_wLabelStyle, sizeof(currentPrefs.TextWindows.fd_wLabelStyle) ))
			currentPrefs.TextWindows.fd_wLabelStyle = SCA_BE2WORD(currentPrefs.TextWindows.fd_wLabelStyle);

		d1(KPrintF(__FUNC__ "/%ld:  Icon offset  left=%ld  top=%ld  right=%ld  bottom=%ld\n", \
			__FUNC__, __LINE__, currentPrefs.Icons.ig_IconOffsets.Left, currentPrefs.Icons.ig_IconOffsets.Top, \
			currentPrefs.Icons.ig_IconOffsets.Width, currentPrefs.Icons.ig_IconOffsets.Height));

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconSecLine, &currentPrefs.Icons.ig_wMultipleLines, sizeof(currentPrefs.Icons.ig_wMultipleLines) ))
			currentPrefs.Icons.ig_wMultipleLines = SCA_BE2WORD(currentPrefs.Icons.ig_wMultipleLines);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconSizeConstraints, &currentPrefs.Icons.ig_SizeConstraints, sizeof(currentPrefs.Icons.ig_SizeConstraints) ))
			{
			currentPrefs.Icons.ig_SizeConstraints.MinX = SCA_BE2WORD(currentPrefs.Icons.ig_SizeConstraints.MinX);
			currentPrefs.Icons.ig_SizeConstraints.MinY = SCA_BE2WORD(currentPrefs.Icons.ig_SizeConstraints.MinY);
			currentPrefs.Icons.ig_SizeConstraints.MaxX = SCA_BE2WORD(currentPrefs.Icons.ig_SizeConstraints.MaxX);
			currentPrefs.Icons.ig_SizeConstraints.MaxY = SCA_BE2WORD(currentPrefs.Icons.ig_SizeConstraints.MaxY);
			}
		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconNominalSize, &currentPrefs.Icons.ig_NominalSize, sizeof(currentPrefs.Icons.ig_NominalSize) ))
			currentPrefs.Icons.ig_NominalSize = SCA_BE2WORD(currentPrefs.Icons.ig_NominalSize);

		GetPreferences(p_MyPrefsHandle, lID, SCP_IconHiliteUnderMouse, &currentPrefs.Icons.ig_HighlightUnderMouse, sizeof(currentPrefs.Icons.ig_HighlightUnderMouse) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconTextSkip, &currentPrefs.TextWindows.fd_wTextSkip, sizeof(currentPrefs.TextWindows.fd_wTextSkip) ))
			currentPrefs.TextWindows.fd_wTextSkip = SCA_BE2WORD(currentPrefs.TextWindows.fd_wTextSkip);

		GetPreferences(p_MyPrefsHandle, lID, SCP_ShowThumbnails, &currentPrefs.Icons.ig_ShowThumbnails, sizeof(currentPrefs.Icons.ig_ShowThumbnails) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailOffsets, &currentPrefs.Icons.ig_ThumbnailOffsets, sizeof(currentPrefs.Icons.ig_ThumbnailOffsets) ))
			{
			currentPrefs.Icons.ig_ThumbnailOffsets.Left = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailOffsets.Left);
			currentPrefs.Icons.ig_ThumbnailOffsets.Top = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailOffsets.Top);
			currentPrefs.Icons.ig_ThumbnailOffsets.Width = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailOffsets.Width);
			currentPrefs.Icons.ig_ThumbnailOffsets.Height = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailOffsets.Height);
			}
		GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailsFillBackground, &currentPrefs.Icons.ig_ThumbnailsBackfill, sizeof(currentPrefs.Icons.ig_ThumbnailsBackfill) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_TransparencyThumbnailsBack, &currentPrefs.Icons.ig_ThumbnailBackgroundTransparency, sizeof(currentPrefs.Icons.ig_ThumbnailBackgroundTransparency) ))
			currentPrefs.Icons.ig_ThumbnailBackgroundTransparency = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailBackgroundTransparency);

		GetPreferences(p_MyPrefsHandle, lID, SCP_SelectedTextRectangle, &currentPrefs.Icons.ig_SelectedTextRectangle, sizeof(currentPrefs.Icons.ig_SelectedTextRectangle) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectBorderX, &currentPrefs.Icons.ig_SelTextRectBorderX, sizeof(currentPrefs.Icons.ig_SelTextRectBorderX) ))
			currentPrefs.Icons.ig_SelTextRectBorderX = SCA_BE2WORD(currentPrefs.Icons.ig_SelTextRectBorderX);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectBorderY, &currentPrefs.Icons.ig_SelTextRectBorderY, sizeof(currentPrefs.Icons.ig_SelTextRectBorderY) ))
			currentPrefs.Icons.ig_SelTextRectBorderY = SCA_BE2WORD(currentPrefs.Icons.ig_SelTextRectBorderY);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_SelTextRectRadius, &currentPrefs.Icons.ig_SelTextRectRadius, sizeof(currentPrefs.Icons.ig_SelTextRectRadius) ))
			currentPrefs.Icons.ig_SelTextRectRadius = SCA_BE2WORD(currentPrefs.Icons.ig_SelTextRectRadius);

		GetPreferences(p_MyPrefsHandle, lID, SCP_ShowThumbnailsAsDefault, &currentPrefs.Icons.ig_ShowThumbnailsAsDefault, sizeof(currentPrefs.Icons.ig_ShowThumbnailsAsDefault) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailsSquare, &currentPrefs.Icons.ig_bSquareThumbnails, sizeof(currentPrefs.Icons.ig_bSquareThumbnails) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailSize, &currentPrefs.Icons.ig_ThumbnailSize, sizeof(currentPrefs.Icons.ig_ThumbnailSize) ))
			currentPrefs.Icons.ig_ThumbnailSize = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailSize);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailMaxAge, &currentPrefs.Icons.ig_ThumbnailMaxAge, sizeof(currentPrefs.Icons.ig_ThumbnailMaxAge) ))
			currentPrefs.Icons.ig_ThumbnailMaxAge = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailMaxAge);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailMinSizeLimit, &currentPrefs.Icons.ig_ThumbnailMinSizeLimit, sizeof(currentPrefs.Icons.ig_ThumbnailMinSizeLimit) ))
			currentPrefs.Icons.ig_ThumbnailMinSizeLimit = SCA_BE2WORD(currentPrefs.Icons.ig_ThumbnailMinSizeLimit);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ThumbnailQuality, &currentPrefs.Icons.ig_ThumbnailQuality, sizeof(currentPrefs.Icons.ig_ThumbnailQuality) ))
			currentPrefs.Icons.ig_ThumbnailQuality = SCA_BE2LONG(currentPrefs.Icons.ig_ThumbnailQuality);

		GetPreferences(p_MyPrefsHandle, lID, SCP_DeviceWinIconLayout, currentPrefs.Icons.ig_DeviceWindowLayoutModes, sizeof(currentPrefs.Icons.ig_DeviceWindowLayoutModes));
		// UBYTE *

		GetPreferences(p_MyPrefsHandle, lID, SCP_IconWinIconLayout, currentPrefs.Icons.ig_IconWindowLayoutModes, sizeof(currentPrefs.Icons.ig_IconWindowLayoutModes));
		// UBYTE *

		GetPreferences(p_MyPrefsHandle, lID, SCP_BobsType, &currentPrefs.DragNDrop.ddg_bBobStyle, sizeof(currentPrefs.DragNDrop.ddg_bBobStyle) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_BobsMethod, &currentPrefs.DragNDrop.ddg_bBobMethod, sizeof(currentPrefs.DragNDrop.ddg_bBobMethod) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_BobsTranspMode, &currentPrefs.DragNDrop.ddg_bBobLook, sizeof(currentPrefs.DragNDrop.ddg_bBobLook) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_BobsTranspType, &currentPrefs.DragNDrop.ddg_bTranspType, sizeof(currentPrefs.DragNDrop.ddg_bTranspType) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_BobsTransp, &currentPrefs.DragNDrop.ddg_lTriggers, sizeof(currentPrefs.DragNDrop.ddg_lTriggers) ))
			currentPrefs.DragNDrop.ddg_lTriggers = SCA_BE2LONG(currentPrefs.DragNDrop.ddg_lTriggers);

		GetPreferences(p_MyPrefsHandle, lID, SCP_ScreenTitle, currentPrefs.Desktop.dg_pScreen, sizeof(currentPrefs.Desktop.dg_pScreen) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_RootWinTitle, currentPrefs.Windows.wg_pRootWin, sizeof(currentPrefs.Windows.wg_pRootWin) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_WindowTitle, currentPrefs.Windows.wg_pWindow, sizeof(currentPrefs.Windows.wg_pWindow) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTfAntialiasing, &currentPrefs.TrueTypeFonts.ttg_Antialias, sizeof(currentPrefs.TrueTypeFonts.ttg_Antialias) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_TTfGamma, &currentPrefs.TrueTypeFonts.ttg_Gamma, sizeof(currentPrefs.TrueTypeFonts.ttg_Gamma) ))
			currentPrefs.TrueTypeFonts.ttg_Gamma = SCA_BE2WORD(currentPrefs.TrueTypeFonts.ttg_Gamma);

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTScreenFontDesc, currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc, sizeof(currentPrefs.TrueTypeFonts.ttg_ScreenTTFontDesc) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTIconFontDesc, currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc, sizeof(currentPrefs.TrueTypeFonts.ttg_IconTTFontDesc) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTTextWindowFontDesc, currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc, sizeof(currentPrefs.TrueTypeFonts.ttg_TextWindowTTFontDesc) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_UseTTScreenFont, &currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseScreenTTFont) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_UseTTIconFont, &currentPrefs.TrueTypeFonts.ttg_UseIconTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseIconTTFont) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_UseTTTextWindowFont, &currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont, sizeof(currentPrefs.TrueTypeFonts.ttg_UseTextWindowTTFont) );
		// BYTE

		//GetPreferences(p_MyPrefsHandle, lID, SCP_Seperator, , sizeof() );

		GetPreferences(p_MyPrefsHandle, lID, SCP_TitleRefresh, &currentPrefs.Desktop.dg_bTitleRefresh, sizeof(currentPrefs.Desktop.dg_bTitleRefresh) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_RefreshOnMemoryChange, &currentPrefs.Desktop.dg_bMemoryRefresh, sizeof(currentPrefs.Desktop.dg_bMemoryRefresh) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_WinTitleRefresh, &currentPrefs.Desktop.dg_bWinTitleRefresh, sizeof(currentPrefs.Desktop.dg_bWinTitleRefresh) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_WinRefreshOnMemoryChange, &currentPrefs.Desktop.dg_bWinMemoryRefresh, sizeof(currentPrefs.Desktop.dg_bWinMemoryRefresh) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_ConsoleName, currentPrefs.Desktop.dg_ConsoleName, sizeof(currentPrefs.Desktop.dg_ConsoleName) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsDefIcons, &currentPrefs.Paths.pg_pDefaultIcons, sizeof(currentPrefs.Paths.pg_pDefaultIcons) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsDiskCopy, &currentPrefs.Paths.pg_pDiskCopy, sizeof(currentPrefs.Paths.pg_pDiskCopy) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsTheme, &currentPrefs.Paths.pg_pThemes, sizeof(currentPrefs.Paths.pg_pThemes) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsImagecache, &currentPrefs.Paths.pg_pImageCache, sizeof(currentPrefs.Paths.pg_pImageCache) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsThumbnailDb, &currentPrefs.Paths.pg_pThumbnailDb, sizeof(currentPrefs.Paths.pg_pThumbnailDb) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsWBStartup, &currentPrefs.Startup.sg_pWBStartup, sizeof(currentPrefs.Startup.sg_pWBStartup) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsHome, &currentPrefs.Paths.pg_pHome, sizeof(currentPrefs.Paths.pg_pHome) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_PathsSQLiteTempDir, &currentPrefs.Paths.pg_SQLiteTempDir, sizeof(currentPrefs.Paths.pg_SQLiteTempDir) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscAutoRemove, &currentPrefs.DragNDrop.ddg_bAutoRemoveIcons, sizeof(currentPrefs.DragNDrop.ddg_bAutoRemoveIcons) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscClickTransp, &currentPrefs.Icons.ig_bClickAreaMask, sizeof(currentPrefs.Icons.ig_bClickAreaMask) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscHardEmulation, &currentPrefs.Miscellaneous.mg_bHardEmulation, sizeof(currentPrefs.Miscellaneous.mg_bHardEmulation) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscUseExAll, &currentPrefs.Miscellaneous.mg_bUseExAll, sizeof(currentPrefs.Miscellaneous.mg_bUseExAll) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_CreateSoftLinks, &currentPrefs.Miscellaneous.mg_bCreateSoftLinks, sizeof(currentPrefs.Miscellaneous.mg_bCreateSoftLinks) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_CopyBuffLen, &currentPrefs.Miscellaneous.mg_CopyBuffSize, sizeof(currentPrefs.Miscellaneous.mg_CopyBuffSize) ))
			currentPrefs.Miscellaneous.mg_CopyBuffSize = SCA_BE2LONG(currentPrefs.Miscellaneous.mg_CopyBuffSize);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_MaxUndoSteps, &currentPrefs.Miscellaneous.mg_MaxUndoSteps, sizeof(currentPrefs.Miscellaneous.mg_MaxUndoSteps) ))
			currentPrefs.Miscellaneous.mg_MaxUndoSteps = SCA_BE2LONG(currentPrefs.Miscellaneous.mg_MaxUndoSteps);

		if (WorkbenchBase->lib_Version < 45)
			if (GetPreferences(p_MyPrefsHandle, lID, SCP_DefaultStackSize, &currentPrefs.Miscellaneous.mg_DefaultStackSize, sizeof(currentPrefs.Miscellaneous.mg_DefaultStackSize) ))
				currentPrefs.Miscellaneous.mg_DefaultStackSize = SCA_BE2LONG(currentPrefs.Miscellaneous.mg_DefaultStackSize);
		else
			{
			// starting with WB 45, stack size is changed via "Workbench" prefs.
			//WorkbenchControlA()
			WorkbenchControl(NULL,
				WBCTRLA_GetDefaultStackSize, (ULONG) &currentPrefs.Miscellaneous.mg_DefaultStackSize,
				TAG_END);
			}
		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscWindowType, &currentPrefs.Windows.wg_bType, sizeof(currentPrefs.Windows.wg_bType) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscDoWaitDelay, &currentPrefs.Startup.sg_bWBStartupDelay, sizeof(currentPrefs.Startup.sg_bWBStartupDelay) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscDiskiconsRefresh, &currentPrefs.Desktop.dg_bDiskRefresh, sizeof(currentPrefs.Desktop.dg_bDiskRefresh) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_MiscMenuCurrentDir, &currentPrefs.Miscellaneous.mg_bMenuCurrentDir, sizeof(currentPrefs.Miscellaneous.mg_bMenuCurrentDir) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_DragDropCopyQualifier, &currentPrefs.DragNDrop.ddg_pCopyQualifier, sizeof(currentPrefs.DragNDrop.ddg_pCopyQualifier) ))
			currentPrefs.DragNDrop.ddg_pCopyQualifier = SCA_BE2LONG(currentPrefs.DragNDrop.ddg_pCopyQualifier);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_PopupApplySelectedQualifier, &currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier, sizeof(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier) ))
			currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier = SCA_BE2LONG(currentPrefs.Miscellaneous.mg_PopupApplySelectedQualifier);

		GetPreferences(p_MyPrefsHandle, lID, SCP_PopupApplySelectedAlways, &currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways, sizeof(currentPrefs.Miscellaneous.mp_PopupApplySelectedAlways) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_DragDropMakeLinkQualifier, &currentPrefs.DragNDrop.ddg_pMakeLinkQualifier, sizeof(currentPrefs.DragNDrop.ddg_pMakeLinkQualifier) ))
			currentPrefs.DragNDrop.ddg_pMakeLinkQualifier = SCA_BE2LONG(currentPrefs.DragNDrop.ddg_pMakeLinkQualifier);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_DragDropMoveQualifier, &currentPrefs.DragNDrop.ddg_pMoveQualifier, sizeof(currentPrefs.DragNDrop.ddg_pMoveQualifier) ))
			currentPrefs.DragNDrop.ddg_pMoveQualifier = SCA_BE2LONG(currentPrefs.DragNDrop.ddg_pMoveQualifier);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_SingleWindowLassoQualifier, &currentPrefs.Desktop.dg_SingleWindowLassoQualifier, sizeof(currentPrefs.Desktop.dg_SingleWindowLassoQualifier) ))
			currentPrefs.Desktop.dg_SingleWindowLassoQualifier = SCA_BE2LONG(currentPrefs.Desktop.dg_SingleWindowLassoQualifier);

		GetPreferences(p_MyPrefsHandle, lID, SCP_NewIconsTransparent, &currentPrefs.Icons.ig_bNewIconTransparent, sizeof(currentPrefs.Icons.ig_bNewIconTransparent) );
		// BYTE

		ReadNewIconsPrecision(p_MyPrefsHandle, lID);

		//GetPreferences(p_MyPrefsHandle, lID, SCP_TextModeDateFormat, , sizeof() );
		ReadTextModeColumns(p_MyPrefsHandle, lID);

		ReadPluginList(p_MyPrefsHandle, lID);
		ReadControlBarGadgetList(p_MyPrefsHandle, lID,
			&ControlBarGadgetListNormal, SCP_ControlBarGadgetsStd, SCP_ControlBarGadgetStringsStd);
		ReadControlBarGadgetList(p_MyPrefsHandle, lID,
			&ControlBarGadgetListBrowser, SCP_ControlBarGadgetsBr, SCP_ControlBarGadgetStringsBr);

		GetPreferences(p_MyPrefsHandle, lID, SCP_MMB_Move, &currentPrefs.Windows.wg_bMMBMove, sizeof(currentPrefs.Windows.wg_bMMBMove) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_WindowPopupTitleOnly, &currentPrefs.Windows.wg_bPopTitleOnly, sizeof(currentPrefs.Windows.wg_bPopTitleOnly) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_DefaultWindowSize, &currentPrefs.Windows.wg_DefaultSize, sizeof(currentPrefs.Windows.wg_DefaultSize) ))
			{
			currentPrefs.Windows.wg_DefaultSize.Left = SCA_BE2WORD(currentPrefs.Windows.wg_DefaultSize.Left);
			currentPrefs.Windows.wg_DefaultSize.Top = SCA_BE2WORD(currentPrefs.Windows.wg_DefaultSize.Top);
			currentPrefs.Windows.wg_DefaultSize.Width = SCA_BE2WORD(currentPrefs.Windows.wg_DefaultSize.Width);
			currentPrefs.Windows.wg_DefaultSize.Height = SCA_BE2WORD(currentPrefs.Windows.wg_DefaultSize.Height);
			}
		if (GetPreferences(p_MyPrefsHandle, lID, SCP_IconCleanupSpace, &currentPrefs.Windows.wg_CleanupSpaces, sizeof(currentPrefs.Windows.wg_CleanupSpaces) ))
			{
			currentPrefs.Windows.wg_CleanupSpaces.Left = SCA_BE2WORD(currentPrefs.Windows.wg_CleanupSpaces.Left);
			currentPrefs.Windows.wg_CleanupSpaces.Top = SCA_BE2WORD(currentPrefs.Windows.wg_CleanupSpaces.Top);
			currentPrefs.Windows.wg_CleanupSpaces.Width = SCA_BE2WORD(currentPrefs.Windows.wg_CleanupSpaces.Width);
			currentPrefs.Windows.wg_CleanupSpaces.Height = SCA_BE2WORD(currentPrefs.Windows.wg_CleanupSpaces.Height);
			}
		GetPreferences(p_MyPrefsHandle, lID, SCP_FullBench, &currentPrefs.Desktop.dg_bHideTitleBar, sizeof(currentPrefs.Desktop.dg_bHideTitleBar) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_LoadDefIconsFirst, &currentPrefs.Icons.ig_bLoadDefIconsFirst, sizeof(currentPrefs.Icons.ig_bLoadDefIconsFirst) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_DefaultIconsSaveable, &currentPrefs.Icons.ig_bIconsSaveable, sizeof(currentPrefs.Icons.ig_bIconsSaveable) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_EasyMultiSelect, &currentPrefs.DragNDrop.ddg_bEasyMultiSelect, sizeof(currentPrefs.DragNDrop.ddg_bEasyMultiSelect) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_EasyMultiDrag, &currentPrefs.DragNDrop.ddg_bEasyMultiDrag, sizeof(currentPrefs.DragNDrop.ddg_bEasyMultiDrag) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_DropStart, &currentPrefs.Desktop.dg_bDropStart, sizeof(currentPrefs.Desktop.dg_bDropStart) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_AutoLeaveOut, &currentPrefs.Desktop.dg_AutoLeaveOut, sizeof(currentPrefs.Desktop.dg_AutoLeaveOut));
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_HideHiddenFiles, &currentPrefs.Windows.wg_bHideHiddenFiles, sizeof(currentPrefs.Windows.wg_bHideHiddenFiles) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_HideProtectHiddenFiles, &currentPrefs.Windows.wg_bHideProtectHiddenFiles, sizeof(currentPrefs.Windows.wg_bHideProtectHiddenFiles) ); // JMC
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_ShowAllDefault, &currentPrefs.Windows.wg_ShowAllByDefault, sizeof(currentPrefs.Windows.wg_ShowAllByDefault) );
		// UBYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_ViewByDefault, &currentPrefs.Windows.wg_ViewByDefault, sizeof(currentPrefs.Windows.wg_ViewByDefault) );
		// UBYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_CheckOverlappingIcons, &currentPrefs.Windows.wg_CheckOverlappingIcons, sizeof(currentPrefs.Windows.wg_CheckOverlappingIcons) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ActiveWindowTransparency, &currentPrefs.Windows.wg_TransparencyActiveWindow, sizeof(currentPrefs.Windows.wg_TransparencyActiveWindow) ))
			currentPrefs.Windows.wg_TransparencyActiveWindow = SCA_BE2WORD(currentPrefs.Windows.wg_TransparencyActiveWindow);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_InactiveWindowTransparency, &currentPrefs.Windows.wg_wTransparencyInactiveWindow, sizeof(currentPrefs.Windows.wg_wTransparencyInactiveWindow) ))
			currentPrefs.Windows.wg_wTransparencyInactiveWindow = SCA_BE2WORD(currentPrefs.Windows.wg_wTransparencyInactiveWindow);

		GetPreferences(p_MyPrefsHandle, lID, SCP_TextModeFont, &currentPrefs.TextWindows.fd_TextWindowFontDescr, sizeof(currentPrefs.TextWindows.fd_TextWindowFontDescr) );
		// char[]

		GetPreferences(p_MyPrefsHandle, lID, SCP_UnderSoftLinkNames, &currentPrefs.TextWindows.fd_bShowSoftLinks, sizeof(currentPrefs.TextWindows.fd_bShowSoftLinks) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_PopScreenTitle, &currentPrefs.Desktop.dg_bPopTitleBar, sizeof(currentPrefs.Desktop.dg_bPopTitleBar) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_SplashWindowEnable, &currentPrefs.Startup.sg_bShowSplash, sizeof(currentPrefs.Startup.sg_bShowSplash) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_SplashWindowHoldTime, &currentPrefs.Startup.sg_bSplashCloseDelay, sizeof(currentPrefs.Startup.sg_bSplashCloseDelay) ))
			currentPrefs.Startup.sg_bSplashCloseDelay = SCA_BE2WORD(currentPrefs.Startup.sg_bSplashCloseDelay);

		GetPreferences(p_MyPrefsHandle, lID, SCP_StatusBarEnable, &currentPrefs.Windows.wg_bStatusBar, sizeof(currentPrefs.Windows.wg_bStatusBar) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_CleanupOnResize, &currentPrefs.Windows.wg_bCleanupOnResize, sizeof(currentPrefs.Windows.wg_bCleanupOnResize) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_ToolTipsEnable, &currentPrefs.Icons.ig_bShowTooltips, sizeof(currentPrefs.Icons.ig_bShowTooltips) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ToolTipPopupDelay, &currentPrefs.Icons.ig_bTooltipDelay, sizeof(currentPrefs.Icons.ig_bTooltipDelay) ))
			currentPrefs.Icons.ig_bTooltipDelay = SCA_BE2LONG(currentPrefs.Icons.ig_bTooltipDelay);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_ToolTipTransparency, &currentPrefs.Icons.ig_TransparencyTooltips, sizeof(currentPrefs.Icons.ig_TransparencyTooltips) ))
			currentPrefs.Icons.ig_TransparencyTooltips = SCA_BE2WORD(currentPrefs.Icons.ig_TransparencyTooltips);

		GetPreferences(p_MyPrefsHandle, lID, SCP_ShowDragDropObjCount, &currentPrefs.DragNDrop.ddg_bShowObjectCount, sizeof(currentPrefs.DragNDrop.ddg_bShowObjectCount) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_DragDropIconsSingle, &currentPrefs.DragNDrop.ddg_bGroupDrag, sizeof(currentPrefs.DragNDrop.ddg_bGroupDrag) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_DropMarkMode, &currentPrefs.DragNDrop.ddg_bDropMarkMode, sizeof(currentPrefs.DragNDrop.ddg_bDropMarkMode) );
		// BYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_TransparencyIconDrag, &currentPrefs.DragNDrop.ddg_TransparencyIconDrag, sizeof(currentPrefs.DragNDrop.ddg_TransparencyIconDrag) ))
			currentPrefs.DragNDrop.ddg_TransparencyIconDrag = SCA_BE2WORD(currentPrefs.DragNDrop.ddg_TransparencyIconDrag);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_TransparencyIconShadow, &currentPrefs.DragNDrop.ddg_TransparencyIconShadow, sizeof(currentPrefs.DragNDrop.ddg_TransparencyIconShadow) ))
			currentPrefs.DragNDrop.ddg_TransparencyIconShadow = SCA_BE2WORD(currentPrefs.DragNDrop.ddg_TransparencyIconShadow);

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_TransparencyDefaultIcon, &currentPrefs.Icons.ig_TransparencyDefaultIcons, sizeof(currentPrefs.Icons.ig_TransparencyDefaultIcons) ))
			currentPrefs.Icons.ig_TransparencyDefaultIcons = SCA_BE2WORD(currentPrefs.Icons.ig_TransparencyDefaultIcons);

		GetPreferences(p_MyPrefsHandle, lID, SCP_EnableDropMenu, &currentPrefs.DragNDrop.ddg_bEnableDropMenu, sizeof(currentPrefs.DragNDrop.ddg_bEnableDropMenu) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_PopupWindowsDelay, &currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds, sizeof(currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds) ))
			currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds = SCA_BE2WORD(currentPrefs.DragNDrop.ddg_PopupWindowDelaySeconds);

		GetPreferences(p_MyPrefsHandle, lID, SCP_TextWindowsStriped, &currentPrefs.TextWindows.fd_bShowStripes, sizeof(currentPrefs.TextWindows.fd_bShowStripes) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_SelectTextIconName, &currentPrefs.TextWindows.fd_SelectTextIconName, sizeof(currentPrefs.TextWindows.fd_SelectTextIconName) );
		// UBYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_SelectMarkerBaseColor, &currentPrefs.TextWindows.fd_SelectMarkerBaseColor, sizeof(currentPrefs.TextWindows.fd_SelectMarkerBaseColor) );
		// struct ARGB

		GetPreferences(p_MyPrefsHandle, lID, SCP_SelectMarkerTransparency, &currentPrefs.TextWindows.fd_SelectMarkerTransparency, sizeof(currentPrefs.TextWindows.fd_SelectMarkerTransparency) );
		// UBYTE

		if (GetPreferences(p_MyPrefsHandle, lID, SCP_DrawerSortMode, &currentPrefs.TextWindows.fd_DrawerSortMode, sizeof(currentPrefs.TextWindows.fd_DrawerSortMode) ))
			currentPrefs.TextWindows.fd_DrawerSortMode = SCA_BE2LONG(currentPrefs.TextWindows.fd_DrawerSortMode);

		FreePrefsHandle(p_MyPrefsHandle);
		}

	return RETURN_OK;
}


IPTR mui_getv(APTR obj, ULONG attr)
{
	IPTR v;
	GetAttr(attr, obj, &v);
	return (v);
}


static void ReadPluginList(APTR p_MyPrefsHandle, LONG lID)
{
	WORD PluginArray[304];
	LONG Entry;

	Entry = 0;

	while (GetEntry(p_MyPrefsHandle, lID, SCP_PlugInList, PluginArray, sizeof(PluginArray), Entry) > 0)
		{
		STRPTR lp;
		struct PluginDef *pd = (struct PluginDef *) calloc(1, sizeof(struct PluginDef));
		struct SCP_PluginEntry *pey = (struct SCP_PluginEntry *) PluginArray;

		d1(KPrintF(__FILE__ "/%s/%ld: MinVersion=%ld\n", __FUNC__, __LINE__, pey->pey_NeededVersion));

		if (pd)
			{
			struct ScaOOPPluginBase *ScalosPluginBase;
#ifdef __amigaos4__
			struct ScalosPluginIFace *IScalosPlugin;
#endif

			pd->plug_priority = SCA_BE2WORD(pey->pey_Priority);
			pd->plug_instsize = SCA_BE2WORD(pey->pey_InstSize);
			pd->plug_NeededVersion = SCA_BE2WORD(pey->pey_NeededVersion);

			d1(KPrintF(__FILE__ "/%s/%ld: priority=%ld  instsize=%ld\n", __FUNC__, __LINE__, pd->plug_priority, pd->plug_instsize));

			lp = pey->pey_Path;

			stccpy(pd->plug_filename, lp, sizeof(pd->plug_filename));
			d1(KPrintF(__FILE__ "/%s/%ld: filename=<%s>\n", __FUNC__, __LINE__, pd->plug_filename));

			lp += 1 + strlen(lp);
			stccpy(pd->plug_classname, lp, sizeof(pd->plug_classname));
			d1(KPrintF(__FILE__ "/%s/%ld: classname=<%s>\n", __FUNC__, __LINE__, pd->plug_classname));

			lp += 1 + strlen(lp);
			stccpy(pd->plug_superclassname, lp, sizeof(pd->plug_superclassname));

			d1(KPrintF(__FILE__ "/%s/%ld: superclassname=<%s>\n", __FUNC__, __LINE__, pd->plug_superclassname));

			if (NULL == ScalosBase || pd->plug_NeededVersion <= ScalosBase->scb_LibNode.lib_Version)
				{
				ScalosPluginBase = (struct ScaOOPPluginBase *) OpenLibrary(pd->plug_filename, 0);

				d1(KPrintF(__FILE__ "/%s/%ld: ScalosPluginBase=%08lx\n", __FUNC__, __LINE__, ScalosPluginBase));
				if (ScalosPluginBase)
					{
					const struct ScaClassInfo *ci;

#ifdef __amigaos4__
					IScalosPlugin = (struct ScalosPluginIFace *)GetInterface(
						(struct Library *)ScalosPluginBase, "main", 1, NULL
					);
#endif

					do	{
						d1(kprintf(__FILE__ "/%s/%ld:  signature=%08lx\n", __FUNC__, __LINE__, ScalosPluginBase->soob_Signature));

						if (MAKE_ID('P','L','U','G') != ScalosPluginBase->soob_Signature)
							break;

						pd->plug_libVersion = ScalosPluginBase->soob_Library.lib_Version;
						pd->plug_libRevision = ScalosPluginBase->soob_Library.lib_Revision;

						sprintf(pd->plug_VersionString, "V%ld.%ld", (long) pd->plug_libVersion, (long) pd->plug_libRevision);

						d1(KPrintF(__FILE__ "/%s/%ld:  VersionString=<%s>\n", __FUNC__, __LINE__, pd->plug_VersionString));

						ci = SCAGetClassInfo();
						d1(kprintf(__FILE__ "/%s/%ld: ClassInfo=%08lx\n", __FUNC__, __LINE__, ci));
						if (NULL == ci)
							break;

						d1(kprintf(__FILE__ "/%s/%ld: ClassInfo=%08lx  NeededVersion=%ld\n", __FUNC__, __LINE__, ci, ci->ci_neededversion));

						if (ScalosBase && ci->ci_neededversion > ScalosBase->scb_LibNode.lib_Version)
							break;

						pd->plug_priority = ci->ci_priority;
						pd->plug_instsize = ci->ci_instsize;

						stccpy(pd->plug_classname, ci->ci_classname, sizeof(pd->plug_classname));
						stccpy(pd->plug_superclassname, ci->ci_superclassname, sizeof(pd->plug_superclassname));
						stccpy(pd->plug_RealName, ci->ci_name, sizeof(pd->plug_RealName));
						stccpy(pd->plug_Description, ci->ci_description, sizeof(pd->plug_Description));
						stccpy(pd->plug_Creator, ci->ci_makername, sizeof(pd->plug_Creator));

						d1(kprintf(__FILE__ "/%s/%ld: InstSize=%ld\n", __FUNC__, __LINE__, pd->plug_instsize));
						d1(kprintf(__FILE__ "/%s/%ld: Priority=%ld\n", __FUNC__, __LINE__, pd->plug_priority));
						d1(kprintf(__FILE__ "/%s/%ld: ClassName=<%s>\n", __FUNC__, __LINE__, pd->plug_classname));
						d1(kprintf(__FILE__ "/%s/%ld: SuperClassName=<%s>\n", __FUNC__, __LINE__, pd->plug_superclassname));
						d1(kprintf(__FILE__ "/%s/%ld: RealName=<%s>\n", __FUNC__, __LINE__, pd->plug_RealName));
						d1(kprintf(__FILE__ "/%s/%ld: Description=<%s>\n", __FUNC__, __LINE__, pd->plug_Description));
						d1(kprintf(__FILE__ "/%s/%ld: Creator=<%s>\n", __FUNC__, __LINE__, pd->plug_Creator));

						pd->plug_SuccessfullyLoaded = TRUE;
						} while (0);

#ifdef __amigaos4__
					if (IScalosPlugin)
						DropInterface((struct Interface *)IScalosPlugin);
#endif
					CloseLibrary((struct Library *) ScalosPluginBase);
					}
				else if (NULL == ScalosBase)
					{
					// assume the plugin refused to open due to Scalos not running
					pd->plug_SuccessfullyLoaded = TRUE;
					}
				}

			AddTail(&PluginList, (struct Node *) &pd->plug_Node);
			}
		Entry++;
		}
}


static void WritePluginList(APTR p_MyPrefsHandle, LONG lID)
{
	WORD PluginArray[304];
	LONG Entry;
	struct PluginDef *pd;

	Entry = 0;

	for (pd = (struct PluginDef *) PluginList.lh_Head;
		pd != (struct PluginDef *) &PluginList.lh_Tail;
		pd = (struct PluginDef *) pd->plug_Node.mln_Succ)
		{
		size_t Length = sizeof(struct SCP_PluginEntry);
		STRPTR lp;
		struct SCP_PluginEntry *pey = (struct SCP_PluginEntry *) PluginArray;

		memset(PluginArray, 0, sizeof(PluginArray));

		pey->pey_Priority = SCA_WORD2BE(pd->plug_priority);
		pey->pey_InstSize = SCA_WORD2BE(pd->plug_instsize);
		pey->pey_NeededVersion = SCA_WORD2BE(pd->plug_NeededVersion);
		pey->pey_unused = 0;

		d1(kprintf(__FILE__ "/%s/%ld: priority=%ld  instsize=%ld\n", __FUNC__, __LINE__, pl->plug_priority, pl->plug_instsize));

		lp = pey->pey_Path;

		strcpy(lp, pd->plug_filename);
		d1(kprintf(__FILE__ "/%s/%ld: filename=<%s>\n", __FUNC__, __LINE__, pd->plug_filename));

		Length += strlen(lp);
		lp += 1 + strlen(lp);

		strcpy(lp, pd->plug_classname);
		d1(kprintf(__FILE__ "/%s/%ld: classname=<%s>\n", __FUNC__, __LINE__, pd->plug_classname));

		Length += 1 + strlen(lp);
		lp += 1 + strlen(lp);

		strcpy(lp, pd->plug_superclassname);

		Length += 1 + strlen(lp);

		SetEntry(p_MyPrefsHandle, lID, SCP_PlugInList, PluginArray, Length, Entry++);
		}
}


static void CleanupPluginList(void)
{
	struct PluginDef *pd;

	while ((pd  = (struct PluginDef *) RemHead(&PluginList)))
		{
		free(pd);
		}
}


struct PluginDef *FindPlugin(struct SCAModule *app, BPTR fLock)
{
	ULONG entries;
	ULONG n;
	struct PluginDef *pd;

	entries = getv(app->Obj[PLUGIN_LIST], MUIA_NList_Entries);

	for (n=0; n<entries; n++)
		{
		BPTR eLock;

		DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_GetEntry, n, &pd);

		eLock = Lock(pd->plug_filename, ACCESS_READ);
		if (eLock)
			{
			LONG same = SameLock(eLock, fLock);
			UnLock(eLock);

			if (LOCK_SAME == same)
				return pd;
			}
		}

	return NULL;
}


void RemovePlugin(struct SCAModule *app, const struct PluginDef *pdRemove)
{
	ULONG entries;
	ULONG n;
	struct PluginDef *pd;

	entries = getv(app->Obj[PLUGIN_LIST], MUIA_NList_Entries);

	// Remove <pdRemove> from NList
	for (n=0; n<entries; n++)
		{
		DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_GetEntry, n, &pd);

		if (0 == strcmp(pdRemove->plug_RealName, pd->plug_RealName))
			{
			DoMethod(app->Obj[PLUGIN_LIST], MUIM_NList_Remove, n);
			break;
			}
		}

	// Remove <pdRemove> from PluginList
	for (pd = (struct PluginDef *) PluginList.lh_Head;
		pd != (struct PluginDef *) &PluginList.lh_Tail;
		pd = (struct PluginDef *) pd->plug_Node.mln_Succ)
		{
		if (0 == strcmp(pdRemove->plug_RealName, pd->plug_RealName))
			{
			Remove((struct Node *) pd);
			free(pd);
			break;
			}
		}
}


BOOL AddPlugin(struct SCAModule *app, CONST_STRPTR FileName)
{
	BOOL Success = FALSE;
	struct PluginDef *pd;
	BPTR libLock = (BPTR)NULL;
	struct ScaOOPPluginBase *ScalosPluginBase = NULL;
#ifdef __amigaos4__
	struct ScalosPluginIFace *IScalosPlugin = NULL;
#endif

	do	{
		const struct ScaClassInfo *ci;

		pd = (struct PluginDef *) calloc(1, sizeof(struct PluginDef));
		if (NULL == pd)
			break;

		libLock = Lock(FileName, ACCESS_READ);
		if ((BPTR)NULL == libLock)
			break;

		NameFromLock(libLock, pd->plug_filename, sizeof(pd->plug_filename));

		ScalosPluginBase = (struct ScaOOPPluginBase *) OpenLibrary(pd->plug_filename, 0);

		d1(KPrintF(__FILE__ "/%s/%ld: ScalosPluginBase=%08lx\n", __FUNC__, __LINE__, ScalosPluginBase));
		if (NULL == ScalosPluginBase)
			break;
#ifdef __amigaos4__
		IScalosPlugin = (struct ScalosPluginIFace *)GetInterface(
			(struct Library *)ScalosPluginBase, "main", 1, NULL
		);
#endif
		if (MAKE_ID('P','L','U','G') != ScalosPluginBase->soob_Signature)
			break;

		pd->plug_libVersion = ScalosPluginBase->soob_Library.lib_Version;
		pd->plug_libRevision = ScalosPluginBase->soob_Library.lib_Revision;

		sprintf(pd->plug_VersionString, "V%ld.%ld", (long) pd->plug_libVersion, (long) pd->plug_libRevision);

		d1(KPrintF(__FILE__ "/%s/%ld:  VersionString=<%s>\n", __FUNC__, __LINE__, pd->plug_VersionString));

		ci = SCAGetClassInfo();
		if (NULL == ci)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld: ClassInfo=%08lx  NeededVersion=%ld\n", __FUNC__, __LINE__, ci, ci->ci_neededversion));

		if (ScalosBase && ci->ci_neededversion > ScalosBase->scb_LibNode.lib_Version)
			break;

		pd->plug_priority = ci->ci_priority;
		pd->plug_instsize = ci->ci_instsize;
		pd->plug_NeededVersion = ci->ci_neededversion;

		stccpy(pd->plug_classname, ci->ci_classname, sizeof(pd->plug_classname));
		stccpy(pd->plug_superclassname, ci->ci_superclassname, sizeof(pd->plug_superclassname));
		stccpy(pd->plug_RealName, ci->ci_name, sizeof(pd->plug_RealName));
		stccpy(pd->plug_Description, ci->ci_description, sizeof(pd->plug_Description));
		stccpy(pd->plug_Creator, ci->ci_makername, sizeof(pd->plug_Creator));

		d1(KPrintF(__FILE__ "/%s/%ld: InstSize=%ld\n", __FUNC__, __LINE__, pd->plug_instsize));
		d1(KPrintF(__FILE__ "/%s/%ld: Priority=%ld\n", __FUNC__, __LINE__, pd->plug_priority));
		d1(KPrintF(__FILE__ "/%s/%ld: ClassName=<%s>\n", __FUNC__, __LINE__, pd->plug_classname));
		d1(KPrintF(__FILE__ "/%s/%ld: SuperClassName=<%s>\n", __FUNC__, __LINE__, pd->plug_superclassname));
		d1(KPrintF(__FILE__ "/%s/%ld: RealName=<%s>\n", __FUNC__, __LINE__, pd->plug_RealName));
		d1(KPrintF(__FILE__ "/%s/%ld: Description=<%s>\n", __FUNC__, __LINE__, pd->plug_Description));
		d1(KPrintF(__FILE__ "/%s/%ld: Creator=<%s>\n", __FUNC__, __LINE__, pd->plug_Creator));

		// make sure there are no duplicate plugin entries
		RemovePlugin(app, pd);

		AddTail(&PluginList, (struct Node *) &pd->plug_Node);

		DoMethod(app->Obj[PLUGIN_LIST],
			MUIM_NList_InsertSingle,
			pd,
			MUIV_NList_Insert_Sorted);
		set(app->Obj[PLUGIN_LIST], MUIA_NList_Active, MUIV_NList_Active_Bottom);

		Success = TRUE;

		// do not free pd now!
		pd = NULL;
		} while (0);

#ifdef __amigaos4__
	if (IScalosPlugin)
		DropInterface((struct Interface *)IScalosPlugin);
#endif
	if (ScalosPluginBase)
		CloseLibrary((struct Library *) ScalosPluginBase);
	if (libLock)
		UnLock(libLock);
	if (pd)
		free(pd);

	return Success;
}


static void ReadTextModeColumns(APTR p_MyPrefsHandle, LONG lID)
{
	struct PrefsStruct *ps;

	ps = FindPreferences(p_MyPrefsHandle, lID, SCP_TextMode_ListColumns);
	if (ps)
		{
		BYTE *pu = (BYTE *) PS_DATA(ps);
		short n = 0;

		while (~0 != *pu && n < Sizeof(currentPrefs.TextWindows.fd_lDisplayFields))
			{
			BYTE col = *pu++;

			d1(kprintf(__FILE__ "/%s/%ld: Column=%ld\n", __FUNC__, __LINE__, col));

			if (!(col & 0x80))
				{
				currentPrefs.TextWindows.fd_lDisplayFields[n++] = col;
				d1(kprintf(__FILE__ "/%s/%ld: ColumnsArray[%ld]=%ld\n", __FUNC__, __LINE__, n-1, ColumnsArray[n-1]));
				}
			}

		currentPrefs.TextWindows.fd_lDisplayFields[n] = ~0;
		}
}


static void WriteTextModeColumns(APTR p_MyPrefsHandle, LONG lID)
{
	SetPreferences(p_MyPrefsHandle, lID, SCP_TextMode_ListColumns, 
		currentPrefs.TextWindows.fd_lDisplayFields, sizeof(currentPrefs.TextWindows.fd_lDisplayFields));
}


static void RemoveFileDisplayEntry(Object *list, ULONG Index)
{
	ULONG nEntries = 0;
	ULONG n;

	get(list, MUIA_NList_Entries, &nEntries);

	for (n = 0; n < nEntries; n++)
		{
		struct FileDisplayListEntry *entry = NULL;

		DoMethod(list, MUIM_NList_GetEntry, n, &entry);

		if (entry)
			{
			if (entry->fdle_Index == Index)
				{
				DoMethod(list, MUIM_NList_Remove, n);
				break;
				}
			}
		}
}


void ResetToDefaults(struct SCAModule *app)
{
	CleanupPluginList();
	currentPrefs = defaultPrefs;
	CreateDefaultControlBarGadgets();
	UpdateGuiFromPrefs(app);
}


static void ReadNewIconsPrecision(APTR p_MyPrefsHandle, LONG lID)
{
	LONG x = 0;

	if (GetPreferences(p_MyPrefsHandle, lID, SCP_NewIconsPrecision, &x, sizeof(x) ))
		{
		x = SCA_BE2LONG(x);
		}

	switch (x)
		{
	case PRECISION_EXACT:
		currentPrefs.Icons.ig_bNewIconPrecision = 1;
		break;
	case PRECISION_IMAGE:
		currentPrefs.Icons.ig_bNewIconPrecision = 2;
		break;
	case PRECISION_ICON:
		currentPrefs.Icons.ig_bNewIconPrecision = 3;
		break;
	case PRECISION_GUI:
	default:
		currentPrefs.Icons.ig_bNewIconPrecision = 4;
		break;
		}
}


static void WriteNewIconsPrecision(APTR p_MyPrefsHandle, LONG lID)
{
	LONG x;

	switch (currentPrefs.Icons.ig_bNewIconPrecision)
		{
	case 1:
		x = PRECISION_EXACT;
		break;
	case 2:
		x = PRECISION_IMAGE;
		break;
	case 3:
		x = PRECISION_ICON;
		break;
	case 4:
	default:
		x = PRECISION_GUI;
		break;
		}

	x = SCA_LONG2BE(x);

	SetPreferences(p_MyPrefsHandle, lID, SCP_NewIconsPrecision, &x, sizeof(x) );
}


static void UpdateFileDisplayPrefsFromGUI(struct SCAModule *app)
{
	ULONG nEntries = 0;
	ULONG n;

	get(app->Obj[NLIST_USE_FILEDISPLAY], MUIA_NList_Entries, &nEntries);

	for (n = 0; n < nEntries; n++)
		{
		struct FileDisplayListEntry *entry = NULL;

		DoMethod(app->Obj[NLIST_USE_FILEDISPLAY], MUIM_NList_GetEntry, n, &entry);

		if (entry)
			{
			currentPrefs.TextWindows.fd_lDisplayFields[n] = entry->fdle_Index;
			}
		}

	currentPrefs.TextWindows.fd_lDisplayFields[n] = ~0;
}


static size_t TranslateQualifierToString(ULONG Qualifier, STRPTR line, size_t MaxLength)
{
	ULONG n;

	for (n=0; n<Sizeof(QualifierTable); n++)
		{
		if ((Qualifier & QualifierTable[n].ien_Qualifier) == QualifierTable[n].ien_Qualifier)
			{
			if (MaxLength > 2 + strlen(QualifierTable[n].ien_name))
				{
				strcat(line, " ");
				strcat(line, QualifierTable[n].ien_name);
				MaxLength -= strlen(QualifierTable[n].ien_name) + 1;
				}
			}
		}

	return strlen(line);
}


static ULONG TranslateStringToQualifier(CONST_STRPTR line)
{
	ULONG Qualifier = 0L;

	while (*line)
		{
		size_t Length = 0;
		ULONG n;
		char QualString[20];
		STRPTR dlp = QualString;

		while (*line && !isspace(*line) && Length < sizeof(QualString) - 1)
			*dlp++ = *line++;
		*dlp = '\0';

		// skip separating " "
		while (*line && isspace(*line))
			line++;

		for (n=0; n<Sizeof(QualifierTable); n++)
			{
			if (0 == Stricmp(QualifierTable[n].ien_name, QualString))
				{
				Qualifier |= QualifierTable[n].ien_Qualifier;
				break;
				}
			}
		}

	return Qualifier;
}


// Check if prerequisites for custom Bob method are fulfilled
static void AdjustBobMethod(struct SCAModule *app)
{
	struct Library *CyberGfxBase;

	CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
	if (NULL == CyberGfxBase)
		{
		// CyberGfxBase is required for custom Bobs!
		currentPrefs.DragNDrop.ddg_bBobMethod = DRAGMETHOD_GELS;
		}
	else
		{
		struct Screen *WbScreen;

		CloseLibrary(CyberGfxBase);

		WbScreen = LockPubScreen("Workbench");
		if (WbScreen)
			{
			ULONG ScreenDepth;

			ScreenDepth = GetBitMapAttr(WbScreen->RastPort.BitMap, BMA_DEPTH);

			// custom Bobs are only useable for Screen depths > 8 bit !
			if (ScreenDepth <= 8)
				currentPrefs.DragNDrop.ddg_bBobMethod = DRAGMETHOD_GELS;

			UnlockPubScreen(NULL, WbScreen);
			}
		}
}


void ReadIconFontPrefs(struct SCAModule *app)
{
	struct IFFHandle *iff;
	struct FontPrefs *FontChunk = NULL;
	BOOL IffOpened = FALSE;

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		iff->iff_Stream = (ULONG) Open("ENV:sys/font.prefs", MODE_OLDFILE);

		d1(kprintf(__FILE__ "/%s/%ld: iff_Stream=%08lx\n", __FUNC__, __LINE__, iff->iff_Stream));
		if (0 == iff->iff_Stream)
			break;

		if (RETURN_OK != OpenIFF(iff, IFFF_READ))
			break;

		d1(kprintf(__FILE__ "/%s/%ld: OpenIFF() OK\n", __FUNC__, __LINE__));
		IffOpened = TRUE;

		if (RETURN_OK != StopChunk(iff, ID_PREF, ID_FONT))
			break;

		d1(kprintf(__FILE__ "/%s/%ld: StopChunk() OK\n", __FUNC__, __LINE__));

		while (1)
			{
			struct ContextNode *cn;

			if (RETURN_OK != ParseIFF(iff, IFFPARSE_SCAN))
				break;

			d1(kprintf(__FILE__ "/%s/%ld: ParseIFF() OK\n", __FUNC__, __LINE__));

			cn = CurrentChunk(iff);

			d1(kprintf(__FILE__ "/%s/%ld: CurrentChunk=%08lx\n", __FUNC__, __LINE__, cn));
			if (NULL == cn)
				continue;

			d1(kprintf(__FILE__ "/%s/%ld: CurrentChunk=%08lx  Size=%lu\n", __FUNC__, __LINE__, cn, cn->cn_Size));

			FontChunk = malloc(cn->cn_Size);
			d1(kprintf(__FILE__ "/%s/%ld: FontChunk=%08lx\n", __FUNC__, __LINE__, FontChunk));
			if (NULL == FontChunk)
				break;

			if (ReadChunkBytes(iff, FontChunk, cn->cn_Size) < 0)
				break;

			d1(kprintf(__FILE__ "/%s/%ld: ReadChunkBytes OK\n", __FUNC__, __LINE__));

			FontChunk->fp_Type = SCA_BE2WORD(FontChunk->fp_Type);
			FontChunk->fp_TextAttr.ta_YSize = SCA_BE2WORD(FontChunk->fp_TextAttr.ta_YSize);

			switch (FontChunk->fp_Type)
				{
			case FP_WBFONT:
				sprintf(currentPrefs.Icons.ig_IconFontDesc, "%s/%d",
					FontChunk->fp_Name, FontChunk->fp_TextAttr.ta_YSize);
				set(app->Obj[MCC_ICONFONT_SAMPLE],
					MUIA_FontSample_StdFontDesc, (ULONG) currentPrefs.Icons.ig_IconFontDesc);
				set(app->Obj[POP_ICONFONT],
					MUIA_String_Contents, (ULONG) currentPrefs.Icons.ig_IconFontDesc);
				memset(&IconFontPrefs, 0, sizeof(IconFontPrefs));
				IconFontPrefs = *FontChunk;
				break;
			case FP_SCREENFONT:
				memset(&ScreenFontPrefs, 0, sizeof(ScreenFontPrefs));
				ScreenFontPrefs = *FontChunk;
				break;
			case FP_SYSFONT:
				memset(&SysFontPrefs, 0, sizeof(SysFontPrefs));
				SysFontPrefs = *FontChunk;
				break;
			default:
				break;
				}

			d1(KPrintF(__FILE__ "/%s/%ld: FontAttr.ta_Name=<%s>  SIze=%ld\n", \
				__FUNC__, __LINE__, FontChunk->fp_Name, FontChunk->fp_TextAttr.ta_YSize));

			free(FontChunk);
			FontChunk = NULL;
			}
		} while (0);

	if (FontChunk)
		free(FontChunk);
	if (iff)
		{
		if (IffOpened)
			CloseIFF(iff);
		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);
		FreeIFF(iff);
		}
}


void WriteFontPrefs(struct SCAModule *app, CONST_STRPTR FontPrefsName)
{
	struct IFFHandle *iff;
	BOOL IffOpened = FALSE;
	struct FontPrefs FontChunk;

	do	{
		size_t Length;
		LONG Result;
		static const struct PrefHeader prefHeader = { 0, 0, 0L };

		iff = AllocIFF();
		if (NULL == iff)
			break;

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR) Open(FontPrefsName, MODE_NEWFILE);

		d1(kprintf(__FILE__ "/%s/%ld: iff_Stream=%08lx\n", __FUNC__, __LINE__, iff->iff_Stream));
		if (0 == iff->iff_Stream)
			break;

		if (RETURN_OK != OpenIFF(iff, IFFF_WRITE))
			break;

		d1(kprintf(__FILE__ "/%s/%ld: OpenIFF() OK\n", __FUNC__, __LINE__));
		IffOpened = TRUE;

		Result = PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_PRHD, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		if (WriteChunkBytes(iff, (APTR) &prefHeader, sizeof(prefHeader)) < 0)
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);		// PRHD
		if (RETURN_OK != Result)
			break;


		Result = WriteIconFontPrefs(app, iff);
		if (RETURN_OK != Result)
			break;

		// Write Sys Font Prefs
		Result = PushChunk(iff, 0, ID_FONT, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Length = GetFontPrefsSize(&SysFontPrefs);
		FontChunk = SysFontPrefs;
		FontChunk.fp_Type = SCA_WORD2BE(FontChunk.fp_Type);
		FontChunk.fp_TextAttr.ta_YSize = SCA_WORD2BE(FontChunk.fp_TextAttr.ta_YSize);
		if (Length != WriteChunkBytes(iff, (APTR) &FontChunk, Length))
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		// Write Screen font prefs
		Result = PushChunk(iff, 0, ID_FONT, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Length = GetFontPrefsSize(&ScreenFontPrefs);
		FontChunk = ScreenFontPrefs;
		FontChunk.fp_Type = SCA_WORD2BE(FontChunk.fp_Type);
		FontChunk.fp_TextAttr.ta_YSize = SCA_WORD2BE(FontChunk.fp_TextAttr.ta_YSize);
		if (Length != WriteChunkBytes(iff, (APTR) &FontChunk, Length))
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;
		} while (0);

	if (iff)
		{
		if (IffOpened)
			CloseIFF(iff);
		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);
		FreeIFF(iff);
		}
}



static LONG WriteIconFontPrefs(struct SCAModule *app, struct IFFHandle *iff)
{
	LONG Result;

	do	{
		struct FontPrefs FontChunk;
		CONST_STRPTR Separator;
		size_t Length;

		Separator = strchr(currentPrefs.Icons.ig_IconFontDesc, '/');
		if (NULL == Separator)
			{
			Result =  ERROR_BAD_TEMPLATE;
			break;
			}

		Length = Separator - (STRPTR) currentPrefs.Icons.ig_IconFontDesc;

		FontChunk = IconFontPrefs;
		memset(&FontChunk.fp_Name, 0, sizeof(FontChunk.fp_Name));
//		FontChunk.fp_TextAttr.ta_Name = NULL;
		stccpy(FontChunk.fp_Name, currentPrefs.Icons.ig_IconFontDesc, 1 + Length);

		// append ".font" if not already present
		if (0 != strcmp(FontChunk.fp_Name + Length - 5, ".font"))
			strcat(FontChunk.fp_Name, ".font");

		sscanf(Separator, "/%hd", &FontChunk.fp_TextAttr.ta_YSize);

		Result = PushChunk(iff, 0, ID_FONT, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Length = GetFontPrefsSize(&FontChunk);
		FontChunk.fp_Type = SCA_WORD2BE(FontChunk.fp_Type);
		FontChunk.fp_TextAttr.ta_YSize = SCA_WORD2BE(FontChunk.fp_TextAttr.ta_YSize);
		if (Length != WriteChunkBytes(iff, (APTR) &FontChunk, Length))
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;
		} while (0);

	return Result;
}


static size_t GetFontPrefsSize(const struct FontPrefs *FontChunk)
{
	size_t Length;

	Length = sizeof(struct FontPrefs);
//	  Length = offsetof(struct FontPrefs, fp_Name)
//		  + strlen(FontChunk->fp_Name)
//		  + 1;

	return Length;
}


static void ReadControlBarGadgetList(APTR p_MyPrefsHandle, LONG lID, struct List *CbGadgetsList,
	ULONG prefsIDGadgets, ULONG prefsIDStrings)
{
	struct SCP_GadgetStringEntry *gse = NULL;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct SCP_GadgetEntry sgy;
		size_t size;

		size = GetEntry(p_MyPrefsHandle, lID, prefsIDGadgets, &sgy, sizeof(sgy), 0);
		if (size > 0)
			{
			struct SCP_GadgetStringEntry gseTemp;
	                LONG Entry;

			CleanupControlBarGadgetsList(CbGadgetsList);

			memset(&gseTemp, 0, sizeof(gseTemp));
			Entry = 0;

			// first determine required size of prefsIDStrings
			if (GetPreferences(p_MyPrefsHandle, lID, prefsIDStrings, &gseTemp, sizeof(gseTemp) ))
				{
				gseTemp.gse_Length = SCA_BE2LONG(gseTemp.gse_Length);
				}
			size = sizeof(struct SCP_GadgetStringEntry) + gseTemp.gse_Length;
			d1(KPrintF("%s/%s/%ld: gse_Length=%lu  size=%lu\n", __FILE__, __FUNC__, __LINE__, gseTemp.gse_Length, size));

			gse = malloc(size);
			d1(KPrintF("%s/%s/%ld: gse=%08lx\n", __FILE__, __FUNC__, __LINE__, gse));
			if (NULL == gse)
				break;

			// now read complete prefsIDStrings
			if (GetPreferences(p_MyPrefsHandle, lID, prefsIDStrings, gse, size ))
				{
				gse->gse_Length = SCA_BE2LONG(gse->gse_Length);
				}

			// read array of SCP_ControlBarGadgets
			while ((size = GetEntry(p_MyPrefsHandle, lID, prefsIDGadgets, &sgy, sizeof(sgy), Entry)) > 0)
				{
				struct ControlBarGadgetEntry *cgy;

				d1(KPrintF("%s/%s/%ld: size=%lu\n", __FILE__, __FUNC__, __LINE__, size));

				d1(KPrintF("%s/%s/%ld: sgy_Action=<%s>\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_Action));
				d1(KPrintF("%s/%s/%ld: sgy_NormalImageIndex=%lu  sgy_SelectedImageIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_NormalImageIndex, sgy.sgy_SelectedImageIndex));
				d1(KPrintF("%s/%s/%ld: sgy_DisabledImageIndex=%lu  sgy_HelpTextIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_DisabledImageIndex, sgy.sgy_HelpTextIndex));

				cgy = calloc(sizeof(struct ControlBarGadgetEntry), 1);
				d1(KPrintF("%s/%s/%ld: cgy=%08lx  Entry=%ld  sgy_GadgetType=%ld\n", __FILE__, __FUNC__, __LINE__, cgy, Entry, sgy.sgy_GadgetType));
				if (NULL == cgy)
					break;

				// create ControlBarGadgetEntry from SCP_GadgetEntry
				cgy->cgy_GadgetType = SCA_BE2WORD(sgy.sgy_GadgetType);
				stccpy(cgy->cgy_Action, sgy.sgy_Action, sizeof(cgy->cgy_Action));
				cgy->cgy_NormalImage = strdup(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_NormalImageIndex)]);
				cgy->cgy_SelectedImage = strdup(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_SelectedImageIndex)]);
				cgy->cgy_DisabledImage = strdup(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_DisabledImageIndex)]);
				cgy->cgy_HelpText = strdup(&gse->gse_Strings[SCA_BE2LONG(sgy.sgy_HelpTextIndex)]);

				// add new ControlBarGadgetEntry to CbGadgetsList
				AddTail(CbGadgetsList, &cgy->cgy_Node);

				Entry++;
				}
			}
		else
			{
			// no stored record for control bar gadgets
			// create internal default setup
			CreateDefaultControlBarGadgets();
			}
		} while (0);

	if (gse)
		free(gse);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void WriteControlBarGadgetList(struct SCAModule *app, APTR p_MyPrefsHandle, LONG lID,
	ULONG ObjID, ULONG prefsIDGadgets, ULONG prefsIDStrings)
{
	struct SCP_GadgetStringEntry *gse = NULL;

	do	{
		struct ControlBarGadgetEntry *cgy;
		size_t StringLength = 0;
		size_t gseLength;
		ULONG StringIndex = 0;
		ULONG entries, n;
		LONG Entry;

		if (0 == ObjID)
			break;

		entries = getv(app->Obj[ObjID], MUIA_NList_Entries);

		// count required length for SCP_ControlBarGadgetStrings
		for (n=0; n<entries; n++)
			{
			DoMethod(app->Obj[ObjID], MUIM_NList_GetEntry, n, &cgy);

			if (SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
				{
				StringLength += 1 + strlen(cgy->cgy_NormalImage);
				StringLength += 1 + strlen(cgy->cgy_SelectedImage);
				StringLength += 1 + strlen(cgy->cgy_DisabledImage);
				StringLength += 1 + strlen(cgy->cgy_HelpText);
				}
			}

		// allocate SCP_ControlBarGadgetStrings
		gseLength = sizeof(struct SCP_GadgetStringEntry) + StringLength;
		gse = calloc(gseLength, 1);
		if (NULL == gse)
			break;

		gse->gse_Length = SCA_LONG2BE(StringLength);
		Entry = 0;

		// start with empty string
		strcpy(gse->gse_Strings,  "");
		StringIndex++;

		for (n=0; n<entries; n++)
			{
			struct SCP_GadgetEntry sgy;

			DoMethod(app->Obj[ObjID], MUIM_NList_GetEntry, n, &cgy);

			memset(&sgy, 0, sizeof(sgy));

			sgy.sgy_GadgetType = SCA_WORD2BE(cgy->cgy_GadgetType);
			stccpy(sgy.sgy_Action, cgy->cgy_Action, sizeof(sgy.sgy_Action));

			if (SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
				{
				sgy.sgy_NormalImageIndex = SCA_LONG2BE(StringIndex);
				strcpy(gse->gse_Strings + StringIndex,  cgy->cgy_NormalImage);
				StringIndex += 1 + strlen(cgy->cgy_NormalImage);

				sgy.sgy_SelectedImageIndex = SCA_LONG2BE(StringIndex);
				strcpy(gse->gse_Strings + StringIndex,  cgy->cgy_SelectedImage);
				StringIndex += 1 + strlen(cgy->cgy_SelectedImage);

				sgy.sgy_DisabledImageIndex = SCA_LONG2BE(StringIndex);
				strcpy(gse->gse_Strings + StringIndex,  cgy->cgy_DisabledImage);
				StringIndex += 1 + strlen(cgy->cgy_DisabledImage);

				sgy.sgy_HelpTextIndex = SCA_LONG2BE(StringIndex);
				strcpy(gse->gse_Strings + StringIndex,  cgy->cgy_HelpText);
				StringIndex += 1 + strlen(cgy->cgy_HelpText);
				}

			d1(KPrintF("%s/%s/%ld: Entry=%ld  sgy_GadgetType=%ld\n", __FILE__, __FUNC__, __LINE__, Entry, sgy.sgy_GadgetType));
			d1(KPrintF("%s/%s/%ld: sgy_Action=<%s>\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_Action));
			d1(KPrintF("%s/%s/%ld: sgy_NormalImageIndex=%lu  sgy_SelectedImageIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_NormalImageIndex, sgy.sgy_SelectedImageIndex));
			d1(KPrintF("%s/%s/%ld: sgy_DisabledImageIndex=%lu  sgy_HelpTextIndex=%lu\n", __FILE__, __FUNC__, __LINE__, sgy.sgy_DisabledImageIndex, sgy.sgy_HelpTextIndex));

			SetEntry(p_MyPrefsHandle, lID, prefsIDGadgets, &sgy, sizeof(sgy), Entry++);
			}

		// now write complete SCP_ControlBarGadgetStrings
		SetPreferences(p_MyPrefsHandle, lID, prefsIDStrings, gse, gseLength);
		} while (0);

	if (gse)
		free(gse);
}


static void CleanupControlBarGadgetsList(struct List *CbGadgetsList)
{
	struct ControlBarGadgetEntry *cgy;

	while ((cgy  = (struct ControlBarGadgetEntry *) RemHead(CbGadgetsList)))
		{
		if (cgy->cgy_NormalImage)
			{
			free(cgy->cgy_NormalImage);
			cgy->cgy_NormalImage = NULL;
			}
		if (cgy->cgy_SelectedImage)
			{
			free(cgy->cgy_SelectedImage);
			cgy->cgy_SelectedImage = NULL;
			}
		if (cgy->cgy_DisabledImage)
			{
			free(cgy->cgy_DisabledImage);
			cgy->cgy_DisabledImage = NULL;
			}
		free(cgy);
		}
}


static void CreateDefaultControlBarGadgets(void)
{
	ULONG n;

	CleanupControlBarGadgetsList(&ControlBarGadgetListBrowser);

	for (n = 0; n < Sizeof(DefaultControlBarGadgets); n++)
		{
		struct ControlBarGadgetEntry *cgy;

		cgy = calloc(sizeof(struct ControlBarGadgetEntry), 1);
		d1(KPrintF("%s/%s/%ld: cgy=%08lx  sgy_GadgetType=%ld\n", __FILE__, __FUNC__, __LINE__, cgy, DefaultControlBarGadgets[n].cgy_GadgetType));
		if (NULL == cgy)
			break;

		// create ControlBarGadgetEntry from SCP_GadgetEntry
		cgy->cgy_GadgetType = DefaultControlBarGadgets[n].cgy_GadgetType;
		stccpy(cgy->cgy_Action, DefaultControlBarGadgets[n].cgy_Action, sizeof(cgy->cgy_Action));
		cgy->cgy_NormalImage = strdup(DefaultControlBarGadgets[n].cgy_NormalImage);
		cgy->cgy_SelectedImage = strdup(DefaultControlBarGadgets[n].cgy_SelectedImage);
		cgy->cgy_DisabledImage = strdup(DefaultControlBarGadgets[n].cgy_DisabledImage);
		cgy->cgy_HelpText = strdup(DefaultControlBarGadgets[n].cgy_HelpText);

		// add new ControlBarGadgetEntry to ControlBarGadgetList
		AddTail(&ControlBarGadgetListBrowser, &cgy->cgy_Node);
		}
}


static ULONG GetLog2(ULONG value)
{
	ULONG Result = 0;

	while (value > 1)
		{
		value = (value >> 1) & 0x7fffffff;
		Result++;
		}

	return Result;
}


// write WORD/UWORD in big endian
static void SetWordPreferences(APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST UWORD *arg, ULONG struct_Size)
{
	if (struct_Size == 2)
		{
		UWORD value = SCA_WORD2BE(*arg);
		SetPreferences(prefsHandle, iD, prefsTag, &value, struct_Size );
		}
}


// write LONG/ULONG in big endian
static void SetLongPreferences(APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST ULONG *arg, ULONG struct_Size)
{
	if (struct_Size == 4)
		{
		ULONG value = SCA_LONG2BE(*arg);
		SetPreferences(prefsHandle, iD, prefsTag, &value, struct_Size );
		}
}
