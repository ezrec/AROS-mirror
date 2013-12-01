// Scalos.c
// $Date$
// $Revision$

//   program: Scalos Main Preferences
//  coded by: Budda [mike@scalos.com]
//   started: 16/01/2001
// completed:
//   updated:

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>

#include <exec/execbase.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/ttengine.h>
#include <scalos/scalos.h>
#include <scalos/scalosprefsplugin.h>
#include <scalos/scalospreviewplugin.h>
#include <mui/BetterString_mcc.h>
#include <mui/Popplaceholder_mcc.h>
#include <mui/Urltext_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/Lamp_mcc.h>
#include <mui/NFloattext_mcc.h>
#include <mui/HotkeyString_mcc.h>
#include <mui/TextEditor_mcc.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/iffparse.h>
#include <proto/scalos.h>
#include <proto/locale.h>
#include <proto/ttengine.h>
#include <proto/utility.h>

#ifndef __AROS__
#define	NO_INLINE_STDARG
#endif
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> //+jmc+

#include "General.h"
#include "ScalosPrefs.h"
#include "ReadWritePrefs.h"
#include "HiddenDevices.h"
#include "Hooks.h"
#include "DataTypesMCC.h"
#include "McpFrameMCC.h"
#include "SelectMarkSampleClass.h"
#include <FontSampleMCC.h>
#include "Images.h"
#include "PrefsPlugins.h"
#include "UrlSubject.h" // +jmc+ define url subject for mailer.

#define	Scalos_Prefs_ARRAY
#define	Scalos_Prefs_CODE
#define	Scalos_Prefs_ARRAY
#include "locale.h"

//-----------------------------------------------------------------

#if !defined(MUIA_Text_HiCharIdx)
#define MUIA_Text_HiCharIdx   0x804214f5
#endif //!defined(MUIA_Text_HiCharIdx)

#define CheckMarkHelp(selected, HelpTextID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		MUIA_CycleChain       , TRUE,\
		MUIA_ShortHelp	      , GetLocString(HelpTextID), \
		End

#define	THEME_CONTROLBAR	"THEME:Window/ControlBar/"

#define ScanButton\
	MUI_NewObject(MUIC_Text,\
		ButtonFrame,\
		MUIA_Font,           MUIV_Font_Button,\
		MUIA_Text_HiCharIdx, '_',\
		MUIA_Text_Contents,  "Scan",\
		MUIA_Text_PreParse,  "\33c",\
		MUIA_InputMode,      MUIV_InputMode_Toggle,\
		MUIA_Background,     MUII_ButtonBack,\
		MUIA_CycleChain,     1,\
		MUIA_Weight,         0,\
		TAG_DONE)

//-----------------------------------------------------------------

struct BufferSizeSliderData
{
	char buf[20];
}
;
struct PrecisionSliderData
{
	char buf[20];
};

struct TTGammaSliderData
{
	char buf[20];
};

struct ThumbnailLifetimeSliderData
{
	char buf[20];
};

struct PageListData
{
	char ImageSpec[30];
	CONST_STRPTR TextLine;
};

struct CgyInit
	{
	enum SCPGadgetTypes cyi_GadgetType;
	CONST_STRPTR  cyi_NormalImage;	      // file name of normal state image
	CONST_STRPTR  cyi_SelectedImage;      // file name of selected state image
	CONST_STRPTR  cyi_DisabledImage;      // file name of disabled state image
	};

#if 0
#ifdef __amigaos4__
struct LocaleInfo
{
    struct LocaleIFace *li_ILocale;
    struct Catalog     *li_Catalog;
};
#endif
#endif

//-----------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

static ULONG TextWindowColumnsListDragQuery(Class *cl, Object *obj, struct MUIP_DragQuery *msg);
static ULONG TextWindowColumnsListDragDrop(Class *cl,Object *obj,struct MUIP_DragDrop *msg);
DISPATCHER_PROTO(TextWindowColumnsList);
static ULONG ControlBarGadgetsListDragQuery(Class *cl, Object *obj, struct MUIP_DragQuery *msg);
static ULONG ControlBarGadgetsListDragDrop(Class *cl,Object *obj,struct MUIP_DragDrop *msg);
DISPATCHER_PROTO(ControlBarGadgetsList);
DISPATCHER_PROTO(PrecisionSlider);
DISPATCHER_PROTO(BufferSizeSlider);
DISPATCHER_PROTO(TTGammaSlider);
DISPATCHER_PROTO(ThumbnailLifetimeSlider);
static VOID Cleanup(struct SCAModule *app);
static BOOL Init(struct SCAModule *app);
static BOOL BuildApp(LONG Action, struct SCAModule *app, struct DiskObject *icon);
static LONG MainLoop(LONG Action, struct SCAModule *app);

static SAVEDS(APTR) INTERRUPT FileDisplayConstructHookFunc(struct Hook *hook, APTR memPool, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT FileDisplayDestructHookFunc(struct Hook *hook, APTR memPool, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT FileDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);

static SAVEDS(APTR) INTERRUPT PluginListConstructHookFunc(struct Hook *hook, APTR memPool, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT PluginListDestructHookFunc(struct Hook *hook, APTR memPool, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT PluginListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);
static SAVEDS(LONG) INTERRUPT PluginListCompareHookFunc(struct Hook *hook, APTR unused, struct NList_CompareMessage *ncm);

static SAVEDS(APTR) INTERRUPT PageListConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT PageListDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT PageListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);

static SAVEDS(APTR) INTERRUPT ModulesListConstructHookFunc(struct Hook *hook, Object *o, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT ModulesListDestructHookFunc(struct Hook *hook, Object *o, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT ModulesListDisplayHookFunc(struct Hook *hook, Object *o, struct NList_DisplayMessage *nldm);

static SAVEDS(APTR) INTERRUPT HiddenDevicesConstructHookFunc(struct Hook *hook, APTR memPool, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT HiddenDevicesDestructHookFunc(struct Hook *hook, APTR memPool, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT HiddenDevicesDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm);
static SAVEDS(LONG) INTERRUPT HiddenDevicesCompareHookFunc(struct Hook *hook, APTR unused, struct NList_CompareMessage *ncm);

static SAVEDS(APTR) INTERRUPT ControlBarGadgetListConstructHookFunc(struct Hook *hook, Object *o, struct NList_ConstructMessage *nlcm);
static SAVEDS(void) INTERRUPT ControlBarGadgetListDestructHookFunc(struct Hook *hook, Object *o, struct NList_DestructMessage *nldm);
static SAVEDS(ULONG) INTERRUPT ControlBarGadgetListDisplayHookFunc(struct Hook *hook, Object *o, struct NList_DisplayMessage *nldm);

static SAVEDS(APTR) INTERRUPT CmdListConstructHookFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *msg);
static SAVEDS(void) INTERRUPT CmdListDestructHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *msg);
static SAVEDS(ULONG) INTERRUPT CmdListDisplayHookFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);
static SAVEDS(LONG) INTERRUPT CmdListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *msg);

CONST_STRPTR GetString(struct Scalos_Prefs_LocaleInfo *li, LONG stringNum);
static void TranslateStringArray(CONST_STRPTR *stringArray);
static void EnableIconFontPrefs(struct SCAModule *app);
static void SetupNewPrefsPages(struct SCAModule *app);
static void InitControlBarGadgets(struct SCAModule *app);
static void SetHotkeyNotifications(Object *hotkey, Object *scan);

static Object *GenerateAboutPage(struct SCAModule *app, CONST_STRPTR cVersion, CONST_STRPTR urlSubject);
static Object *GeneratePathsPage(struct SCAModule *app);
static Object *GenerateStartupPage(struct SCAModule *app);
static Object *GenerateDesktopPage(struct SCAModule *app);
static Object *GenerateIconsPage(struct SCAModule *app);
static Object *GenerateDragNDropPage(struct SCAModule *app);
static Object *GenerateWindowPage(struct SCAModule *app);
static Object *GenerateTextWindowPage(struct SCAModule *app);
static Object *GenerateTrueTypeFontsPage(struct SCAModule *app);
static Object *GenerateMiscPage(struct SCAModule *app);
static Object *GeneratePluginsPage(struct SCAModule *app);
static Object *GenerateModulesPage(struct SCAModule *app);

//-----------------------------------------------------------------

// defined in PrefsVersion.c
extern CONST_STRPTR BuildNr;	// Build number aka SVN revision

//-----------------------------------------------------------------

struct IntuitionBase *IntuitionBase = NULL;
struct Library *PreferencesBase = NULL;
struct Library *MUIMasterBase = NULL;
struct ScalosBase *ScalosBase = NULL;
struct Library *IFFParseBase = NULL;
struct GfxBase *GfxBase = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase = NULL;
#endif
T_LOCALEBASE LocaleBase = NULL;
struct Library *IconBase = NULL;
struct Library *AslBase = NULL;
struct Library *WorkbenchBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *TTEngineBase = NULL;
struct Library *ScalosGfxBase = NULL;
struct Library *DiskfontBase = NULL;

extern struct DosLibrary * DOSBase;

#ifdef __amigaos4__
extern struct Library *SysBase;
struct IntuitionIFace *IIntuition;
struct PreferencesIFace *IPreferences;
struct MUIMasterIFace *IMUIMaster;
struct ScalosIFace *IScalos;
struct IFFParseIFace *IIFFParse;
struct GraphicsIFace *IGraphics;
struct LocaleIFace *ILocale;
struct IconIFace *IIcon;
struct AslIFace *IAsl;
struct WorkbenchIFace *IWorkbench;
struct CyberGfxIFace *ICyberGfx;
struct ScalosGfxIFace *IScalosGfx;
struct TTEngineIFace *ITTEngine;
struct DiskfontIFace *IDiskfont;
#endif

struct List PluginList;
struct List ControlBarGadgetListNormal;
struct List ControlBarGadgetListBrowser;

static struct Catalog *ScalosPrefsCatalog;

ULONG fCreateIcons = TRUE;

static struct Screen *WBScreen = NULL;
static ULONG WBScreenDepth;

ULONG ModuleImageIndex = 15;
static ULONG ModuleEntryIndex = IMAGE_DELETE;

static ULONG ControlBarGadgetImageIndex = 0;

static ULONG HiddenDevicesImageIndex = 0;
static ULONG HiddenDevicesEntryIndex = 1;

CONST_STRPTR cTextWindowsColumns[] =
{
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_NAME,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_SIZE,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_PROT,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_DATE,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_TIME,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_COMMENT,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_OWNER,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_GROUP,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_FILETYPE,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_VERSION,
	(CONST_STRPTR) MSGID_FILEDISP_COLUMN_ICON,
	NULL
};

//-----------------------------------------------------------------

static struct MUI_CustomClass *BufferSizeSliderClass;
static struct MUI_CustomClass *PrecisionSliderClass;
static struct MUI_CustomClass *TTGammaSliderClass;
static struct MUI_CustomClass *ThumbnailLifetimeSliderClass;
struct MUI_CustomClass *DataTypesImageClass;
struct MUI_CustomClass *McpFrameClass;
struct MUI_CustomClass *FontSampleClass;
struct MUI_CustomClass *SelectMarkSampleClass;
static struct MUI_CustomClass *TextWindowColumnsListClass;
static struct MUI_CustomClass *ControlBarGadgetsListClass;

#ifdef __AROS__
#define PrecisionSliderObject BOOPSIOBJMACRO_START(PrecisionSliderClass->mcc_Class)
#define ThumbnailLifetimeSliderObject BOOPSIOBJMACRO_START(ThumbnailLifetimeSliderClass->mcc_Class)
#define ControlBarGadgetsListObject BOOPSIOBJMACRO_START(ControlBarGadgetsListClass->mcc_Class)
#define TextWindowColumnsListObject BOOPSIOBJMACRO_START(TextWindowColumnsListClass->mcc_Class)
#define SelectMarkSampleObject BOOPSIOBJMACRO_START(SelectMarkSampleClass->mcc_Class)
#define TTGammaSliderObject BOOPSIOBJMACRO_START(TTGammaSliderClass->mcc_Class)
#define BufferSizeSliderObject BOOPSIOBJMACRO_START(BufferSizeSliderClass->mcc_Class)
#else
#define PrecisionSliderObject NewObject(PrecisionSliderClass->mcc_Class, 0
#define ThumbnailLifetimeSliderObject NewObject(ThumbnailLifetimeSliderClass->mcc_Class, 0
#define ControlBarGadgetsListObject NewObject(ControlBarGadgetsListClass->mcc_Class, 0
#define TextWindowColumnsListObject NewObject(TextWindowColumnsListClass->mcc_Class, 0
#define SelectMarkSampleObject NewObject(SelectMarkSampleClass->mcc_Class, 0
#define TTGammaSliderObject NewObject(TTGammaSliderClass->mcc_Class, 0
#define BufferSizeSliderObject NewObject(BufferSizeSliderClass->mcc_Class, 0
#endif


STRPTR ProgramName = "";
static struct DiskObject *PrefsDiskObject = NULL;

static BOOL ShowSplashWindow = TRUE;
static BOOL SkipPrefsPlugins = FALSE;

static ULONG cPrefGroups[] =
{
	MSGID_PREFGROUPS_ABOUT,
	MSGID_PREFGROUPS_PATHS,
	MSGID_PREFGROUPS_STARTUP,
	MSGID_PREFGROUPS_DESKTOP,
	MSGID_PREFGROUPS_ICONS,
	MSGID_PREFGROUPS_DRAGNDROP,
	MSGID_PREFGROUPS_WINDOWS,
	MSGID_PREFGROUPS_TEXTWINDOWS,
	MSGID_PREFGROUPS_TRUETYPEFONTS,
	MSGID_PREFGROUPS_MISC,
	MSGID_PREFGROUPS_PLUGINS,
	MSGID_PREFGROUPS_MODULES,
};

static struct NewPageListEntry NewPrefsPages[Sizeof(cPrefGroups)];


static CONST_STRPTR cTitleph[] =
{
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_KICKSTART_RELEASE,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_KICKSTART_VERSION,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_SCALOS_RELEASE,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_SCALOS_VERSION,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_FREE_CHIP_MEM,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_FREE_FAST_MEM,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_FREE_MEM,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_WINDOW_PATH,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DISK_FREE,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DISK_FREE_DIVIDE,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DISK_USED,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DISK_USED_DIVIDE,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_START_NODISK,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_STOP_NODISK,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DISK_PERCENT,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_GFXCHIPS,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_CPU,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_FPU,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_TASKS,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_LIBRARIES,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_PORTS,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_DEVICES,
	(CONST_STRPTR) MSGID_TITLEPLACEHOLDER_SCREENS,
	NULL
};

static const struct CgyInit DefaultControlBarGadgets[] =
{
	{
	SCPGadgetType_BackButton,
	THEME_CONTROLBAR "ButtonBackNormal",
	THEME_CONTROLBAR "ButtonBackSelected",
	THEME_CONTROLBAR "ButtonBackDisabled",
	},
	{
	SCPGadgetType_ForwardButton,
	THEME_CONTROLBAR "ButtonForwardNormal",
	THEME_CONTROLBAR "ButtonForwardSelected",
	THEME_CONTROLBAR "ButtonForwardDisabled",
	},
	{
	SCPGadgetType_UpButton,
	THEME_CONTROLBAR "ButtonUpNormal",
	THEME_CONTROLBAR "ButtonUpSelected",
	THEME_CONTROLBAR "ButtonUpDisabled",
	},
	{
	SCPGadgetType_History,
	THEME_CONTROLBAR "HistoryNormal",
	THEME_CONTROLBAR "HistorySelected",
	THEME_CONTROLBAR "HistoryDisabled",
	},
	{
	SCPGadgetType_BrowseButton,
	THEME_CONTROLBAR "BrowseNormal",
	THEME_CONTROLBAR "BrowseSelected",
	THEME_CONTROLBAR "BrowseDisabled",
	},
	{
	SCPGadgetType_ViewBy,
	THEME_CONTROLBAR "ViewByNormal",
	THEME_CONTROLBAR "ViewBySelected",
	THEME_CONTROLBAR "ViewByDisabled",
	},
	{
	SCPGadgetType_ShowMode,
	THEME_CONTROLBAR "ShowModeNormal",
	THEME_CONTROLBAR "ShowModeSelected",
	THEME_CONTROLBAR "ShowModeDisabled",
	},
	{
	SCPGadgetType_Separator,
	THEME_CONTROLBAR "SeparatorNormal",
	THEME_CONTROLBAR "SeparatorSelected",
	THEME_CONTROLBAR "SeparatorDisabled",
	},
	{
	SCPGadgetType_ActionButton,
	THEME_CONTROLBAR "ActionButtonNormal",
	THEME_CONTROLBAR "ActionButtonSelected",
	THEME_CONTROLBAR "ActionButtonDisabled",
	},
};

static STRPTR cIconToolTips[] =
{
	"Name",
	"Type",
	"Size",
	"Last changed",
	"Protection",
	"Comment",
	NULL
};

static CONST_STRPTR cDesktopPages[] =
	{
	(CONST_STRPTR) MSGID_DESKTOPPAGES_SCREEN,
	(CONST_STRPTR) MSGID_DESKTOPPAGES_ICONS,
	(CONST_STRPTR) MSGID_DESKTOPPAGES_LAYOUT,
	(CONST_STRPTR) MSGID_ICONPAGES_MISC,
	NULL
	};

static CONST_STRPTR cIconPages[] =
{
	(CONST_STRPTR) MSGID_ICONPAGES_ATRIBUTES,
	(CONST_STRPTR) MSGID_ICONPAGES_SIZES,
	(CONST_STRPTR) MSGID_ICONPAGES_LABELS,
	(CONST_STRPTR) MSGID_ICONPAGES_BORDERS,
	(CONST_STRPTR) MSGID_ICONPAGES_TOOLTIPS,
	(CONST_STRPTR) MSGID_ICONPAGES_THUMBNAILS,
	NULL
};

static CONST_STRPTR cScreenTitleModes[] =
{
	(CONST_STRPTR) MSGID_DESKTOPPAGES_SCREENTITLE_ALWAYS,
	(CONST_STRPTR) MSGID_DESKTOPPAGES_SCREENTITLE_POP,
	(CONST_STRPTR) MSGID_DESKTOPPAGES_SCREENTITLE_NEVER,
	NULL
};

static CONST_STRPTR cCreateLinkTypes[] =
{
	(CONST_STRPTR) MSGID_CREATELINKS_HARDLINKS,
	(CONST_STRPTR) MSGID_CREATELINKS_SOFTLINKS,
	NULL
};

static CONST_STRPTR cWindowPages[] =
{
	(CONST_STRPTR) MSGID_WINDOWPAGES_GENERAL,
	(CONST_STRPTR) MSGID_WINDOWPAGES_SIZE,
	(CONST_STRPTR) MSGID_WINDOWPAGES_CONTROLBAR,
	(CONST_STRPTR) MSGID_WINDOWPAGES_LAYOUT,
	NULL
};

static CONST_STRPTR cWindowControlBar[] =
{
	(CONST_STRPTR) MSGID_WINDOWPAGES_CONTROLBAR_BROWSER,
	(CONST_STRPTR) MSGID_WINDOWPAGES_CONTROLBAR_NORMAL,
	NULL
};
static CONST_STRPTR cTextWindowPages[] =
{
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGES_FONTS,
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGES_COLUMNS,
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGES_SELECTIONMARKS,
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGES_MISC,
	NULL
};

static CONST_STRPTR cWindowRefresh[] =
{
	(CONST_STRPTR) MSGID_WINDOWREFRESH_SIMPLE,
	(CONST_STRPTR) MSGID_WINDOWREFRESH_SMART,
	NULL
};

static CONST_STRPTR cShowAllDefault[] =
{
	(CONST_STRPTR) MSGID_WINDOW_SHOWALL_DEFAULT_ICONS,
	(CONST_STRPTR) MSGID_WINDOW_SHOWALL_DEFAULT_ALL,
	NULL
};

static CONST_STRPTR cIconLayoutModes[] =
{
	(CONST_STRPTR) MSGID_ICONLAYOUT_COLUMNS,
	(CONST_STRPTR) MSGID_ICONLAYOUT_ROWS,
	NULL
};

static CONST_STRPTR cDrawerSortMode[] =
{
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGE_DRAWERSORT_TOP,
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGE_DRAWERSORT_BOTTOM,
	(CONST_STRPTR) MSGID_TEXTWINDOWPAGE_DRAWERSORT_MIXED,
	NULL
};

static CONST_STRPTR cViewByDefault[] =
{
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_ICON,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_NAME,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_SIZE,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_DATE,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_TIME,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_COMMENT,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_PROTECTION,
	(CONST_STRPTR) MGSID_WINDOW_VIEWBY_DEFAULT_OWNER,
	(CONST_STRPTR) MSGID_WINDOW_VIEWBY_DEFAULT_GROUP,
	NULL
};

static CONST_STRPTR cFileDisplayPages[] =
{
	(CONST_STRPTR) MSGID_FILEDISPLAYPAGES_TEXT,
	NULL
};

static CONST_STRPTR cIconSizesMin[] =
{
	(CONST_STRPTR) MSGID_ICONSIZES_UNLIMITED,
	(CONST_STRPTR) MSGID_ICONSIZES_16x16,
	(CONST_STRPTR) MSGID_ICONSIZES_24x24,
	(CONST_STRPTR) MSGID_ICONSIZES_32x32,
	(CONST_STRPTR) MSGID_ICONSIZES_48x48,
	(CONST_STRPTR) MSGID_ICONSIZES_64x64,
	(CONST_STRPTR) MSGID_ICONSIZES_96x96,
	(CONST_STRPTR) MSGID_ICONSIZES_128x128,
	NULL
};

static CONST_STRPTR cIconSizesMax[] =
{
	(CONST_STRPTR) MSGID_ICONSIZES_16x16,
	(CONST_STRPTR) MSGID_ICONSIZES_24x24,
	(CONST_STRPTR) MSGID_ICONSIZES_32x32,
	(CONST_STRPTR) MSGID_ICONSIZES_48x48,
	(CONST_STRPTR) MSGID_ICONSIZES_64x64,
	(CONST_STRPTR) MSGID_ICONSIZES_96x96,
	(CONST_STRPTR) MSGID_ICONSIZES_128x128,
	(CONST_STRPTR) MSGID_ICONSIZES_UNLIMITED,
	NULL
};

static CONST_STRPTR cShowThumbnails[] =
{
	(CONST_STRPTR) MSGID_SHOWTHUMBNAILS_NEVER,
	(CONST_STRPTR) MSGID_SHOWTHUMBNAILS_AS_DEFAULT,
	(CONST_STRPTR) MSGID_SHOWTHUMBNAILS_ALWAYS,
	NULL
};

static CONST_STRPTR cThumbnailSizes[] =
{
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_16x16,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_24x24,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_32x32,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_48x48,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_64x64,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_96x96,
	(CONST_STRPTR) MSGID_THUMBNAILSIZES_128x128,
	NULL
};

static CONST_STRPTR cIconLabelStyles[] =
{
	(CONST_STRPTR) MSGID_ICONLABELSTYLES_NORMAL,
	(CONST_STRPTR) MSGID_ICONLABELSTYLES_OUTLINE,
	(CONST_STRPTR) MSGID_ICONLABELSTYLES_SHADOW,
	NULL
};

static CONST_STRPTR cIconDragStyle[] =
{
	(CONST_STRPTR) MSGID_ICONDRAGSTYLES_IMAGEONLY,
	(CONST_STRPTR) MSGID_ICONDRAGSTYLES_IMAGEANDTEXT,
	NULL
};

static CONST_STRPTR cIconDragDropPages[] =
{
	(CONST_STRPTR) MSGID_ICONSPAGES_DRAGNDROP_DRAGGING,
	(CONST_STRPTR) MSGID_ICONSPAGES_DRAGNDROP_TRANSPARENCY,
	(CONST_STRPTR) MSGID_ICONSPAGES_DRAGNDROP_TRIGGERS,
	NULL
};

static CONST_STRPTR cIconDragTransparents[] =
{
	(CONST_STRPTR) MSGID_ICONDRAGTRANSPARENCY_GHOSTED,
	(CONST_STRPTR) MSGID_ICONDRAGTRANSPARENCY_TRANSPARENT,
	NULL
};

static CONST_STRPTR cIconDragRoutines[] =
{
	(CONST_STRPTR) MSGID_ICONDRAGTROUTINES_SYSTEM,
	(CONST_STRPTR) MSGID_ICONDRAGTROUTINES_CUSTOM,
	NULL
};

static CONST_STRPTR cIconDropMark[] =
{
	(CONST_STRPTR) MSGID_ICONDROPMARK_NEVER,
	(CONST_STRPTR) MSGID_ICONDROPMARK_DRAWERSONLY,
	(CONST_STRPTR) MSGID_ICONDROPMARK_ALWAYS,
	NULL
};

static CONST_STRPTR cIconDragLook[] =
{
	(CONST_STRPTR) MSGID_ICONDRAGLOOK_SOLID_TRANSP,
	(CONST_STRPTR) MSGID_ICONDRAGLOOK_ALWAYS_SOLID,
	(CONST_STRPTR) MSGID_ICONDRAGLOOK_ALWAYS_TRANSP,
	(CONST_STRPTR) MSGID_ICONDRAGLOOK_TRANSP_SOLID,
	NULL
};

static CONST_STRPTR cTTFontsAntiAliasing[] =
{
	(CONST_STRPTR) MSGID_TTFONTSPAGE_ANTIALIASING_AUTO,
	(CONST_STRPTR) MSGID_TTFONTSPAGE_ANTIALIASING_OFF,
	(CONST_STRPTR) MSGID_TTFONTSPAGE_ANTIALIASING_ON,
	NULL
};

// order must correspond to "enum SCPGadgetTypes"
static CONST_STRPTR cControlBarGadgetTypes[] =
{
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_BACKBUTTON,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_FORWARDBUTTON,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_UPBUTTON,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_HISTORY,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_BROWSEBUTTON,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_VIEWBY,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_SHOWMODE,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_SEPARATOR,
	(CONST_STRPTR) MSGID_SCPGADGETTYPE_ACTIONBUTTON,
	NULL
};

static const struct CommandTableEntry CommandsTable[] =
	{
	{ "backdrop",		MSGID_COM1NAME },
	{ "executecommand",	MSGID_COM2NAME },
	{ "redrawall",		MSGID_COM3NAME },
	{ "updateall",		MSGID_COM4NAME },
	{ "about",		MSGID_COM5NAME },
	{ "quit",		MSGID_COM6NAME },
	{ "makedir",		MSGID_COM7NAME },
	{ "parent",		MSGID_COM8NAME },
	{ "close",		MSGID_COM9NAME },
	{ "update",		MSGID_COM10NAME },
	{ "selectall",		MSGID_COM11NAME },
	{ "cleanup",		MSGID_COM12NAME },
	{ "snapshotwindow",	MSGID_COM13NAME },
	{ "snapshotall",	MSGID_COM14NAME },
	{ "showonlyicons",	MSGID_COM15NAME },
	{ "showallfiles",	MSGID_COM16NAME },
	{ "showdefault",	MSGID_COM48NAME },
	{ "open",		MSGID_COM19NAME },
	{ "openinnewwindow",    MSGID_COM_OPENNEWWINDOW },
	{ "reset",		MSGID_COM20NAME },
	{ "rename",		MSGID_COM21NAME },
	{ "iconinfo",		MSGID_COM22NAME },
	{ "iconproperties",	MSGID_COM_ICONPROPERTIES },
	{ "windowproperties",	MSGID_COM_WINDOWPROPERTIES },
	{ "snapshot",		MSGID_COM23NAME },
	{ "unsnapshot",		MSGID_COM24NAME },
	{ "leaveout",		MSGID_COM25NAME },
	{ "putaway",		MSGID_COM26NAME },
	{ "delete",		MSGID_COM27NAME },
	{ "clone",		MSGID_COM28NAME },
	{ "emptytrashcan",	MSGID_COM29NAME },
	{ "lastmsg",		MSGID_COM30NAME },
	{ "redraw",		MSGID_COM31NAME },
	{ "iconify",		MSGID_COM32NAME },
	{ "formatdisk",		MSGID_COM33NAME },
	{ "shutdown",		MSGID_COM34NAME },
	{ "sizetofit",		MSGID_COM35NAME },
	{ "thumbnailcachecleanup", MSGID_COM_THUMBNAILCACHECLEANUP },
	{ "clearselection",	MSGID_COM36NAME },
	{ "viewbyicon",		MSGID_COM17NAME },
	{ "viewbytext",		MSGID_COM18NAME },
	{ "viewbysize",		MSGID_COM37NAME },
	{ "viewbydate",		MSGID_COM38NAME },
	{ "viewbytype",		MSGID_COM42NAME },
	{ "viewbydefault",	MSGID_COM47NAME },
	{ "copy",		MSGID_COM39NAME },
	{ "cut",		MSGID_COM40NAME },
	{ "copyto",		MSGID_COM49NAME },
	{ "moveto",		MSGID_COM50NAME },
	{ "paste",		MSGID_COM41NAME },
	{ "cleanupbyname",	MSGID_COM43NAME },
	{ "cleanupbydate",	MSGID_COM44NAME },
	{ "cleanupbysize",	MSGID_COM45NAME },
	{ "cleanupbytype",	MSGID_COM46NAME },
	{ "createthumbnail",	MSGID_COM51NAME },

	};

static const struct CommandTableEntry *CommandsArray[1 + Sizeof(CommandsTable)];

static STRPTR cModulePrefs[] =
{
	"delete.module",
	"empty_trashcan.module",
	"execute_command.module",
	"format_disk.module",
	"information.module",
	"iconproperties.module",
	"newdrawer.module",
	"reboot.module",
	"rename.module",
	"systeminfo.module",
	"windowproperties.module",
	NULL
};


static struct Hook IconsSelectBobRoutineHook = { { NULL, NULL }, HOOKFUNC_DEF(IconsSelectBobRoutineHookFunc), NULL };

static struct Hook IconsMinSizeHook = { { NULL, NULL }, HOOKFUNC_DEF(IconsMinSizeHookFunc), NULL };
static struct Hook IconsMaxSizeHook = { { NULL, NULL }, HOOKFUNC_DEF(IconsMaxSizeHookFunc), NULL };

static struct Hook ModulesPrefsPopupHook = { { NULL, NULL }, HOOKFUNC_DEF(ModulesPrefsPopupHookFunc), NULL };

static struct Hook DesktopHiddenDevicesHook = { { NULL, NULL }, HOOKFUNC_DEF(DesktopHiddenDevicesHookFunc), NULL };

static struct Hook FileDisplayDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(FileDisplayHookFunc), NULL };
static struct Hook FileDisplayConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(FileDisplayConstructHookFunc), NULL };
static struct Hook FileDisplayDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(FileDisplayDestructHookFunc), NULL };

static struct Hook ModulesListDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(ModulesListDisplayHookFunc), NULL };
static struct Hook ModulesListConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(ModulesListConstructHookFunc), NULL };
static struct Hook ModulesListDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(ModulesListDestructHookFunc), NULL };

static struct Hook PageListDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(PageListDisplayHookFunc), NULL };
static struct Hook PageListConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(PageListConstructHookFunc), NULL };
static struct Hook PageListDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(PageListDestructHookFunc), NULL };

static struct Hook PluginListDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(PluginListDisplayHookFunc), NULL };
static struct Hook PluginListConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(PluginListConstructHookFunc), NULL };
static struct Hook PluginListDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(PluginListDestructHookFunc), NULL };
static struct Hook PluginListCompareHook = { { NULL, NULL }, HOOKFUNC_DEF(PluginListCompareHookFunc), NULL };

static struct Hook HiddenDevicesDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(HiddenDevicesDisplayHookFunc), NULL };
static struct Hook HiddenDevicesConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(HiddenDevicesConstructHookFunc), NULL };
static struct Hook HiddenDevicesDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(HiddenDevicesDestructHookFunc), NULL };
static struct Hook HiddenDevicesCompareHook = { { NULL, NULL }, HOOKFUNC_DEF(HiddenDevicesCompareHookFunc), NULL };

static struct Hook PluginListAppMsgHook = { { NULL, NULL }, HOOKFUNC_DEF(PluginListAppMsgHookFunc), NULL };

static struct Hook ControlBarGadgetListDisplayHook = { { NULL, NULL }, HOOKFUNC_DEF(ControlBarGadgetListDisplayHookFunc), NULL };
static struct Hook ControlBarGadgetListConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(ControlBarGadgetListConstructHookFunc), NULL };
static struct Hook ControlBarGadgetListDestructHook = { { NULL, NULL }, HOOKFUNC_DEF(ControlBarGadgetListDestructHookFunc), NULL };

static struct Hook CmdListConstructHook = { { NULL, NULL }, HOOKFUNC_DEF(CmdListConstructHookFunc), NULL };
static struct Hook CmdListDestructHook =  { { NULL, NULL }, HOOKFUNC_DEF(CmdListDestructHookFunc), NULL };
static struct Hook CmdListDisplayHook =	 { { NULL, NULL }, HOOKFUNC_DEF(CmdListDisplayHookFunc),  };
static struct Hook CmdListCompareHook =	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListCompareHookFunc), NULL };

static struct Hook NormalCmdSelectedHook = { { NULL, NULL }, HOOKFUNC_DEF(NormalCmdSelectedHookFunc), NULL };
static struct Hook BrowserCmdSelectedHook = { { NULL, NULL }, HOOKFUNC_DEF(BrowserCmdSelectedHookFunc), NULL };
static struct Hook ControlBarGadgetBrowserChangedHook = { { NULL, NULL }, HOOKFUNC_DEF(ControlBarGadgetBrowserChangedHookFunc), NULL };
static struct Hook ControlBarGadgetNormalChangedHook = { { NULL, NULL }, HOOKFUNC_DEF(ControlBarGadgetNormalChangedHookFunc), NULL };

static struct Hook ScreenTtfPopOpenHook = { { NULL, NULL }, HOOKFUNC_DEF(ScreenTtfPopOpenHookFunc), NULL };
static struct Hook ScreenTtfPopCloseHook = { { NULL, NULL }, HOOKFUNC_DEF(ScreenTtfPopCloseHookFunc), NULL };
static struct Hook IconTtfPopOpenHook = { { NULL, NULL }, HOOKFUNC_DEF(IconTtfPopOpenHookFunc), NULL };
static struct Hook IconTtfPopCloseHook = { { NULL, NULL }, HOOKFUNC_DEF(IconTtfPopCloseHookFunc), NULL };
static struct Hook TextWindowTtfPopOpenHook = { { NULL, NULL }, HOOKFUNC_DEF(TextWindowTtfPopOpenHookFunc), NULL };
static struct Hook TextWindowTtfPopCloseHook = { { NULL, NULL }, HOOKFUNC_DEF(TextWindowTtfPopCloseHookFunc), NULL };

static struct Hook IconDragTransparencyHook = { { NULL, NULL }, HOOKFUNC_DEF(IconDragTransparencyHookFunc), NULL };

static struct Hook ChangeThumbnailSizeHook = { { NULL, NULL }, HOOKFUNC_DEF(ChangeThumbnailSizeHookFunc), NULL };


static struct Hook OpenHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenHookFunc), NULL };
static struct Hook SaveAsHook = { { NULL, NULL }, HOOKFUNC_DEF(SaveAsFunc), NULL };
static struct Hook LastSavedHook = { { NULL, NULL }, HOOKFUNC_DEF(LastSavedFunc), NULL };
static struct Hook Pop_FontPrefs_Hook = {{NULL, NULL}, HOOKFUNC_DEF(StartFontPrefs_Func), NULL};
static struct Hook Pop_WorkbenchPrefs_Hook = {{NULL, NULL}, HOOKFUNC_DEF(StartWorkbenchPrefs_Func), NULL};
static struct Hook Add_Plugin_Hook = {{NULL, NULL}, HOOKFUNC_DEF(AddPlugin_Func), NULL};
static struct Hook PluginListActiveHook = {{NULL, NULL}, HOOKFUNC_DEF(PluginActive_Func), NULL};
static struct Hook ControlBarGadgetListNormalActiveHook = {{NULL, NULL}, HOOKFUNC_DEF(ControlBarGadgetListNormalActiveHookFunc), NULL};
static struct Hook ControlBarGadgetListBrowserActiveHook = {{NULL, NULL}, HOOKFUNC_DEF(ControlBarGadgetListBrowserActiveHookFunc), NULL};
static struct Hook ResetToDefaultsHook = { { NULL, NULL }, HOOKFUNC_DEF(ResetToDefaultsFunc), NULL };
static struct Hook RestoreHook = { { NULL, NULL }, HOOKFUNC_DEF(RestoreFunc), NULL };
static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutFunc), NULL };
static struct Hook AboutMorphosHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMorpOSFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIFunc), NULL };
static struct Hook CreateIconsHook = { { NULL, NULL }, HOOKFUNC_DEF(CreateIconsHookFunc), NULL };
static struct Hook RemovePluginHook = { { NULL, NULL }, HOOKFUNC_DEF(RemovePluginFunc), NULL };
static struct Hook PageChangeHook = { { NULL, NULL }, HOOKFUNC_DEF(PageChangeHookFunc), NULL };
static struct Hook IconFrame_NewBorder_Hook = { { NULL, NULL }, HOOKFUNC_DEF(IconFrameHookFunc), NULL };
static struct Hook ThumbnailFrame_NewBorder_Hook = { { NULL, NULL }, HOOKFUNC_DEF(ThumbnailFrameHookFunc), NULL };
static struct Hook UpdateSelectMarkerSampleHook = { { NULL, NULL }, HOOKFUNC_DEF(UpdateSelectMarkerSampleHookFunc), NULL };

struct Hook AslIntuiMsgHook = { { NULL, NULL }, HOOKFUNC_DEF(AslIntuiMsgHookFunc), NULL };
struct Hook CalculateMaxRadiusHook = { { NULL, NULL }, HOOKFUNC_DEF(CalculateMaxRadiusHookFunc), NULL };

//-----------------------------------------------------------------

// set notifications for a HotkeyString/ScanButton combo
static void SetHotkeyNotifications(Object *hotkey, Object *scan)
{
	DoMethod(scan, MUIM_Notify, MUIA_Selected, TRUE,
		MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject,
		hotkey);

	DoMethod(scan, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, hotkey,
		3, MUIM_Set, MUIA_HotkeyString_Snoop, MUIV_TriggerValue);

	//DoMethod(hotkey, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
	//	scan, 3, MUIM_Set, MUIA_Selected, FALSE);
}

//-----------------------------------------------------------------

static BOOL BuildApp(LONG Action, struct SCAModule *app, struct DiskObject *icon)
{
	static char cVersion[60] = "";
	static char urlSubject[sizeof(URLSUBJECT_SCAVERSION) + 40] = "";
	static char copyRightVersion[120] = "";
	ULONG n;
	struct TagItem *PluginTagList;
#if defined(MUIA_Application_UsedClasses)
	static STRPTR UsedClasses[] =
		{
		MUIC_BetterString,
		MUIC_Lamp,
		MUIC_NFloattext,
		MUIC_HotkeyString,
		MUIC_Popplaceholder,
		MUIC_Urltext,
		MUIC_NListview,
		MUIC_NListtree,
		MUIC_TextEditor,
		NULL
		};
#endif /* MUIA_Application_UsedClasses */

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	if (!CheckMCCVersion(MUIC_BetterString, 11, 0)
		|| !CheckMCCVersion(MUIC_Lamp, 11, 0)
		|| !CheckMCCVersion(MUIC_NFloattext, 19, 48)
		|| !CheckMCCVersion(MUIC_HotkeyString, 12, 16)
		|| !CheckMCCVersion(MUIC_Popplaceholder, 15, 4)
		|| !CheckMCCVersion(MUIC_Urltext, 16, 1)
		|| !CheckMCCVersion(MUIC_NListview, 19, 66)
		|| !CheckMCCVersion(MUIC_NListtree, 18, 18)
		|| !CheckMCCVersion(MUIC_TextEditor, 15, 22))
		{
		d1(KPrintF(__FILE__ "/%s/%ld: CheckMCCVersion failed\n", __FUNC__, __LINE__));
		return FALSE;
		}

	IconFrame_NewBorder_Hook.h_Data = app;
        ThumbnailFrame_NewBorder_Hook.h_Data = app;
	Pop_FontPrefs_Hook.h_Data = app;
	Pop_WorkbenchPrefs_Hook.h_Data = app;
	Add_Plugin_Hook.h_Data = app;
	PluginListAppMsgHook.h_Data = app;
	PluginListActiveHook.h_Data = app;
	ControlBarGadgetListBrowserActiveHook.h_Data = app;
	ControlBarGadgetListNormalActiveHook.h_Data = app;
	OpenHook.h_Data = app;
	SaveAsHook.h_Data = app;
	LastSavedHook.h_Data = app;
	RestoreHook.h_Data = app;
	ResetToDefaultsHook.h_Data = app;
	AboutHook.h_Data = app;
        AboutMorphosHook.h_Data = app;
	AboutMUIHook.h_Data = app;
	CreateIconsHook.h_Data = app;
	RemovePluginHook.h_Data = app;
	ModulesPrefsPopupHook.h_Data = app;
	DesktopHiddenDevicesHook.h_Data = app;
	IconsSelectBobRoutineHook.h_Data = app;
	PageChangeHook.h_Data = app;
	ModulesListConstructHook.h_Data = app;
	ModulesListDestructHook.h_Data = app;
	ScreenTtfPopOpenHook.h_Data = app;
	ScreenTtfPopCloseHook.h_Data = app;
	IconTtfPopOpenHook.h_Data = app;
	IconTtfPopCloseHook.h_Data = app;
	TextWindowTtfPopOpenHook.h_Data = app;
	TextWindowTtfPopCloseHook.h_Data = app;
	IconDragTransparencyHook.h_Data = app;
        CalculateMaxRadiusHook.h_Data = app;
	IconsMinSizeHook.h_Data = app;
	IconsMaxSizeHook.h_Data = app;
	HiddenDevicesDisplayHook.h_Data = app;
	HiddenDevicesConstructHook.h_Data = app;
        HiddenDevicesDestructHook.h_Data = app;
	HiddenDevicesCompareHook.h_Data = app;
        AslIntuiMsgHook.h_Data = app;
	ControlBarGadgetListConstructHook.h_Data = app;
	ControlBarGadgetListDestructHook.h_Data = app;
        CmdListConstructHook.h_Data = app;
        CmdListDestructHook.h_Data = app;
        CmdListDisplayHook.h_Data = app;
        CmdListCompareHook.h_Data = app;
	NormalCmdSelectedHook.h_Data = app;
	BrowserCmdSelectedHook.h_Data = app;
	ControlBarGadgetBrowserChangedHook.h_Data = app;
	ControlBarGadgetNormalChangedHook.h_Data = app;
        AboutMUIHook.h_Data = app;
        UpdateSelectMarkerSampleHook.h_Data = app;
	ChangeThumbnailSizeHook.h_Data = app;

	TranslateStringArray(cTextWindowsColumns);
	TranslateStringArray(cDesktopPages);
	TranslateStringArray(cIconPages);
	TranslateStringArray(cTitleph);
	TranslateStringArray(cWindowPages);
	TranslateStringArray(cWindowControlBar);
	TranslateStringArray(cTextWindowPages);
	TranslateStringArray(cScreenTitleModes);
	TranslateStringArray(cCreateLinkTypes);
	TranslateStringArray(cWindowRefresh);
	TranslateStringArray(cShowAllDefault);
	TranslateStringArray(cIconLayoutModes);
	TranslateStringArray(cDrawerSortMode);
	TranslateStringArray(cViewByDefault);
	TranslateStringArray(cFileDisplayPages);
	TranslateStringArray(cIconSizesMin);
	TranslateStringArray(cIconSizesMax);
	TranslateStringArray(cShowThumbnails);
	TranslateStringArray(cThumbnailSizes);
	TranslateStringArray(cIconLabelStyles);
	TranslateStringArray(cIconDragStyle);
	TranslateStringArray(cTTFontsAntiAliasing);
	TranslateStringArray(cControlBarGadgetTypes);
	TranslateStringArray(cIconDragTransparents);
	TranslateStringArray(cIconDragRoutines);
	TranslateStringArray(cIconDropMark);
	TranslateStringArray(cIconDragLook);
	TranslateStringArray(cIconDragDropPages);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	for (n = 0; n < Sizeof(CommandsTable); n++)
		CommandsArray[n] = &CommandsTable[n];
	CommandsArray[Sizeof(CommandsTable)] = NULL;

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// try to read V3.5+ workbench.prefs
	ReadWorkbenchPrefs("ENV:sys/workbench.prefs");

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (!SkipPrefsPlugins)
		CollectPrefsPlugins(app, "PROGDIR:", "#?.prefsplugin");
	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	CreatePrefsPlugins(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (!CheckMCCsForAllPrefsPlugins())
		{
		d1(KPrintF(__FILE__ "/%s/%ld: CheckMCCsForAllPrefsPlugins failed\n", __FUNC__, __LINE__));
		return FALSE;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	PluginTagList = GetPrefsPluginSubWindows(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// --- Get The Scalos.library Version Number Currently Running
	if (ScalosBase)
		{
		CONST_STRPTR ScalosRelease;

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		if (ScalosBase->scb_LibNode.lib_Version > 40
			|| (40 == ScalosBase->scb_LibNode.lib_Version
				&& ScalosBase->scb_LibNode.lib_Revision >= 34))
			{
			ScalosRelease = ScalosBase->scb_Revision;
			}
		else
			{
			ScalosRelease = "1.3";
			}

		snprintf(cVersion, sizeof(cVersion), GetLocString(MSGID_VERSIONSTRING),
			ScalosRelease,
			ScalosBase->scb_LibNode.lib_Version,
			ScalosBase->scb_LibNode.lib_Revision,
			BuildNr);
		snprintf(urlSubject, sizeof(urlSubject), URLSUBJECT_SCAVERSION,
			cVersion);
		}
	else
		{
		strcpy(cVersion, "");
		strcpy(urlSubject, "Scalos");
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	snprintf(copyRightVersion, sizeof(copyRightVersion), GetLocString(MSGID_COPYRIGHTSTRING), CURRENTYEAR);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	SetupNewPrefsPages(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

#if defined(__MORPHOS__)
	app->Obj[WIN_ABOUT_MORPHOS] = MUI_NewObject("Pantheon.mcc",
		MUIA_Window_ID, MAKE_ID('A','M','A','M'),
		End; //Pantheon.mcc"
#endif //defined(__MORPHOS__)

	app->Obj[APPLICATION] = ApplicationObject,

		MUIA_Application_Author,	AUTHOR,
		MUIA_Application_Base,		PROGRAM,
		MUIA_Application_Copyright,	(ULONG) copyRightVersion,
		MUIA_Application_Description,	PROGRAM,
		MUIA_Application_Title,		PROGRAM,
		MUIA_Application_Version,	"$VER: " PROGRAM " V" VERSION " (" __DATE__ ")" COMPILER_STRING,
#if defined(MUIA_Application_UsedClasses)
		MUIA_Application_UsedClasses,	UsedClasses,
#endif /* MUIA_Application_UsedClasses */
		PrefsDiskObject ? MUIA_Application_DiskObject : TAG_IGNORE,	PrefsDiskObject,
		MUIA_Application_SingleTask,	TRUE,

// -----------------------------------------------------------------------------------------------

		SubWindow, app->Obj[WINDOW_MAIN] = WindowObject,
			MUIA_Window_ID,		MAKE_ID('S', 'C', 'S', 'P'),
			MUIA_Window_Title,	PROGRAM,
			MUIA_Window_AppWindow,	TRUE,

			WindowContents, app->Obj[GROUP_MAIN] = VGroup,

				Child, HGroup,
					Child, app->Obj[LISTVIEW] = NListviewObject,
						MUIA_NListview_NList, app->Obj[LIST] = NListObject,
								InputListFrame,
								MUIA_Background, MUII_ListBack,
								MUIA_NList_DefaultObjectOnClick, TRUE,
								MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_None,
								MUIA_NList_DisplayHook2, (ULONG) &PageListDisplayHook,
								MUIA_NList_ConstructHook2, (ULONG) &PageListConstructHook,
								MUIA_NList_DestructHook2, (ULONG) &PageListDestructHook,
								MUIA_NList_Format, (ULONG) ",",
								MUIA_NList_ShowDropMarks, FALSE,
								MUIA_NList_AdjustWidth, TRUE,
								MUIA_NList_AutoVisible, TRUE,
							End,
						MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PREFGROUPS_SHORTHELP),
					End, //NListview

					Child, BalanceObject, 
					End, //BalanceObject

					Child, app->Obj[GROUP] = PageGroup,
						VirtualFrame,
						MUIA_InnerBottom, 0,
						MUIA_InnerLeft, 0,
						MUIA_InnerRight, 0,
						MUIA_InnerTop, 0,

						Child, GenerateAboutPage(app, cVersion, urlSubject),
						Child, GeneratePathsPage(app),
						Child, GenerateStartupPage(app),
						Child, GenerateDesktopPage(app),
						Child, GenerateIconsPage(app),
						Child, GenerateDragNDropPage(app),
						Child, GenerateWindowPage(app),
						Child, GenerateTextWindowPage(app),
						Child, GenerateTrueTypeFontsPage(app),
						Child, GenerateMiscPage(app),
						Child, GeneratePluginsPage(app),
						Child, GenerateModulesPage(app),

						End, //PageGroup
					End, //HGroup

// -----------------------------------------------------------------------------------------------

				Child, HGroup,
					Child, app->Obj[SAVE] = TextObject,
								ButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								MUIA_Text_HiChar, 's',
								MUIA_ControlChar, 's',
								MUIA_Text_Contents, (ULONG) GetLocString(MSGID_SAVEBUTTON),
								MUIA_CycleChain, 1,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SAVEBUTTON_SHORTHELP),
					End, //TextObject
					Child, app->Obj[USE] = TextObject,
								ButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								MUIA_Text_HiChar, 'u',
								MUIA_ControlChar, 'u',
								MUIA_Text_Contents, (ULONG) GetLocString(MSGID_USEBUTTON),
								MUIA_CycleChain, 1,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_USEBUTTON_SHORTHELP),
					End, //TextObject
					Child, app->Obj[CANCEL] = TextObject,
								ButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								MUIA_Text_HiChar, 'c',
								MUIA_ControlChar, 'c',
								MUIA_Text_Contents, (ULONG) GetLocString(MSGID_CANCELBUTTON),
								MUIA_CycleChain, 1,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_CANCELBUTTON_SHORTHELP),
					End, //TextObject
				End, //HGroup

			End, //VGroup

		End, //Window

#if defined(__MORPHOS__)
		(app->Obj[WIN_ABOUT_MORPHOS]) ? SubWindow : TAG_IGNORE, app->Obj[WIN_ABOUT_MORPHOS],
#endif //defined(__MORPHOS__)

// -----------------------------------------------------------------------------------------------
///
		// Icon frame selection window
		SubWindow, app->Obj[WINDOW_ICONFRAMES] = WindowObject,
			MUIA_Window_ID,		MAKE_ID('F', 'R', 'A', 'M'),
			MUIA_Window_Title,	(ULONG) GetLocString(MSGID_FRAMEWINDOW_TITLE),
			MUIA_Window_NoMenus, 	TRUE,

			WindowContents, RowGroup(2),

				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_NONE,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,

				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_BORDER,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_STRING,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_DROPBOX,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_XEN,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_MWB,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_THICK,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_XWIN,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, RectangleObject,
				End,

				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_BORDER | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_STRING | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_DROPBOX | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_XEN | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_MWB | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_THICK | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,
				Child, McpFrameObject,
					ButtonFrame,
					MUIA_InputMode, MUIV_InputMode_None,
					MUIA_FixWidth, 12,
					MUIA_FixHeight, 12,
					MUIA_InnerBottom, 6,
					MUIA_InnerLeft, 6,
					MUIA_InnerRight, 6,
					MUIA_InnerTop, 6,
					MUIA_Background, MUII_ButtonBack,
					MUIA_MCPFrame_FrameType, MCP_FRAME_XWIN | MCP_FRAME_RECESSED,
					MUIA_Draggable, TRUE,
					MUIA_Dropable, FALSE,
				End,

			End, //RowGroup
		End, //Window

// -----------------------------------------------------------------------------------------------

		SubWindow, app->Obj[WINDOW_SPLASH] = WindowObject,
			MUIA_Window_AppWindow,	FALSE,
			MUIA_Window_CloseGadget, FALSE,
			MUIA_Window_DepthGadget, FALSE,
			MUIA_Window_NoMenus,	TRUE,
			MUIA_Window_DragBar,	FALSE,
			MUIA_Window_LeftEdge,	MUIV_Window_LeftEdge_Centered,
			MUIA_Window_TopEdge,	MUIV_Window_TopEdge_Centered,

			WindowContents, VGroup,
				Child, app->Obj[TEXT_SPLASHWINDOW] = TextObject,
					MUIA_Weight, 0,
					MUIA_Text_Contents, (ULONG) GetLocString(MSGID_SPLASHWINDOW_LOADINGPREFS),
				End, //TextObject
			End, //VGroup

		End, //SubWindow
///
// -----------------------------------------------------------------------------------------------

		MUIA_Application_Menustrip, MenustripObject,
			Child, MenuObjectT(GetLocString(MSGID_MENU_PROJECT)),
				Child, app->Obj[MENU_OPEN] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_OPEN),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_PROJECT_OPEN_SHORT),
				End,
				Child, app->Obj[MENU_SAVE_AS] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_SAVEAS),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_PROJECT_SAVEAS_SHORT),
				End,
				Child, MenuitemObject,
					MUIA_Menuitem_Title, -1,
				End,
				Child, app->Obj[MENU_ABOUT] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_ABOUT),
				End,
#if defined(__MORPHOS__)
				Child, app->Obj[MENU_ABOUT_MORPHOS] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_ABOUTMORPHOS),
					MUIA_Menuitem_Enabled, (NULL != app->Obj[WIN_ABOUT_MORPHOS]),
				End,
#endif //defined(__MORPHOS__)
				Child, app->Obj[MENU_ABOUT_MUI] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_ABOUTMUI),
				End,
				Child, MenuitemObject,
					MUIA_Menuitem_Title, -1,
				End,
				Child, app->Obj[MENU_QUIT] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_PROJECT_QUIT),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_PROJECT_QUIT_SHORT),
				End,
					
			End,//MenuObjectT
			Child, MenuObjectT(GetLocString(MSGID_MENU_EDIT)),
				Child, app->Obj[MENU_RESET_TO_DEFAULTS] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_EDIT_RESETTODEFAULTS),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_EDIT_RESETTODEFAULTS_SHORT),
				End,
				Child, app->Obj[MENU_LAST_SAVED] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_EDIT_LASTSAVED),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_EDIT_LASTSAVED_SHORT),
				End,
				Child, app->Obj[MENU_RESTORE] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_EDIT_RESTORE),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_EDIT_RESTORE_SHORT),
				End,
			End,//MenuObjectT
			Child, MenuObjectT(GetLocString(MSGID_MENU_SETTINGS)),
				Child, app->Obj[MENU_CREATE_ICONS] = MenuitemObject,
					MUIA_Menuitem_Title, (ULONG) GetLocString(MSGID_MENU_SETTINGS_CREATEICONS),
					MUIA_Menuitem_Shortcut, (ULONG) GetLocString(MSGID_MENU_SETTINGS_CREATEICONS_SHORT),
					MUIA_Menuitem_Checkit, TRUE,
					MUIA_Menuitem_Checked, fCreateIcons,
				End,
			End,//MenuObjectT
		End,//MenuStrip

// -----------------------------------------------------------------------------------------------

		// additional taglist to add plugin subwindows
		PluginTagList ? TAG_MORE : TAG_IGNORE, PluginTagList,

// -----------------------------------------------------------------------------------------------

	End //Application
	;

	d1(KPrintF(__FILE__ "/%s/%ld: app=%08lx\n", __FUNC__, __LINE__, app->Obj[APPLICATION]));
	if (NULL == app->Obj[APPLICATION])
		return FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// add pages for prefs plugins
	InsertPrefsPlugins(app, app->Obj[GROUP], app->Obj[LIST]);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// open Splash window
	if (ShowSplashWindow)
        	set(app->Obj[WINDOW_SPLASH], MUIA_Window_Open, TRUE);

	if (icon)
		DoMethodForAllPrefsPlugins(2, MUIM_ScalosPrefs_ParseToolTypes, icon->do_ToolTypes);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_LoadConfig);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	InitControlBarGadgets(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	UpdateGuiFromPrefs(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

#if defined(__MORPHOS__)
	set(app->Obj[GROUP2_DEFAULTSTACKSIZE], MUIA_ShowMe, FALSE);
	set(app->Obj[GROUP_DEFAULTSTACKSIZE], MUIA_Disabled, FALSE);
#else /* __MORPHOS__ */
	set(app->Obj[GROUP2_DEFAULTSTACKSIZE], MUIA_ShowMe, WorkbenchBase->lib_Version >= 45);
	set(app->Obj[GROUP_DEFAULTSTACKSIZE], MUIA_Disabled, WorkbenchBase->lib_Version >= 45);
#endif /* __MORPHOS__ */

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	EnableIconFontPrefs(app);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	DoMethod(app->Obj[GROUP], MUIM_Notify, MUIA_Group_ActivePage, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &PageChangeHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Startup Page
	DoMethod(app->Obj[CHECK_SHOWSPLASH], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[SLIDER_SPLASHCLOSE], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Desktop Page
	DoMethod(app->Obj[CHECK_TITLEMEM], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[SLIDER_TITLEREFRESH], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_WINTITLEMEM], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[SLIDER_WINTITLEREFRESH], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
	DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_Notify, MUIA_NList_ButtonClick, MUIV_EveryTime,
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &DesktopHiddenDevicesHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Icons Page

	// Disable BorderX/BorderY/Radius sliders when CHECK_SEL_ICONTEXT_RECTANGLE is deselected
	DoMethod(app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERX], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
	DoMethod(app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
	DoMethod(app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// Click icon frame gadget opens frame selection window
	DoMethod(app->Obj[FRAME_ICONNORMAL], MUIM_Notify,MUIA_Pressed, FALSE,
		app->Obj[WINDOW_ICONFRAMES], 3, MUIM_Set, MUIA_Window_Open, TRUE);
	DoMethod(app->Obj[FRAME_ICONSELECTED], MUIM_Notify,MUIA_Pressed, FALSE,
		app->Obj[WINDOW_ICONFRAMES], 3, MUIM_Set, MUIA_Window_Open, TRUE);
	DoMethod(app->Obj[FRAME_ICON_THUMBNAIL_NORMAL], MUIM_Notify,MUIA_Pressed, FALSE,
		app->Obj[WINDOW_ICONFRAMES], 3, MUIM_Set, MUIA_Window_Open, TRUE);
	DoMethod(app->Obj[FRAME_ICON_THUMBNAIL_SELECTED], MUIM_Notify,MUIA_Pressed, FALSE,
		app->Obj[WINDOW_ICONFRAMES], 3, MUIM_Set, MUIA_Window_Open, TRUE);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// Disable thumbnail background transparency slider if thumbnail background is not filled
	DoMethod(app->Obj[CHECK_THUMBNAILS_BACKFILL], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_THUMBNAIL_BACKFILL_TRANSPARENCY], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// dropping a new icon frame updates icon borders top/left/right/bottom
	DoMethod(app->Obj[FRAME_ICONNORMAL], MUIM_Notify, MUIA_MCPFrame_FrameType, MUIV_EveryTime,
		app->Obj[WINDOW_ICONFRAMES], 2, MUIM_CallHook, &IconFrame_NewBorder_Hook);
	DoMethod(app->Obj[FRAME_ICONSELECTED], MUIM_Notify, MUIA_MCPFrame_FrameType, MUIV_EveryTime,
		app->Obj[WINDOW_ICONFRAMES], 2, MUIM_CallHook, &IconFrame_NewBorder_Hook);

	// dropping a new thumbnail frame updates thumbnail borders top/left/right/bottom
	DoMethod(app->Obj[FRAME_ICON_THUMBNAIL_NORMAL], MUIM_Notify, MUIA_MCPFrame_FrameType, MUIV_EveryTime,
		app->Obj[WINDOW_ICONFRAMES], 2, MUIM_CallHook, &ThumbnailFrame_NewBorder_Hook);
	DoMethod(app->Obj[FRAME_ICON_THUMBNAIL_SELECTED], MUIM_Notify, MUIA_MCPFrame_FrameType, MUIV_EveryTime,
		app->Obj[WINDOW_ICONFRAMES], 2, MUIM_CallHook, &ThumbnailFrame_NewBorder_Hook);

	DoMethod(app->Obj[SOFTICONSLINK], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[SOFTTEXTSLINK], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
	DoMethod(app->Obj[SOFTTEXTSLINK], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[SOFTICONSLINK], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

	DoMethod(app->Obj[CHECK_TOOLTIPS], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		app->Obj[TOOLTIP_SETTINGSGROUP], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
	DoMethod(app->Obj[POP_FONTPREFS], MUIM_Notify, MUIA_Selected, FALSE, 
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &Pop_FontPrefs_Hook);

	DoMethod(app->Obj[CYCLE_ROUTINE], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &IconsSelectBobRoutineHook);

	DoMethod(app->Obj[CYCLE_ICONMINSIZE], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &IconsMinSizeHook);
	DoMethod(app->Obj[CYCLE_ICONMAXSIZE], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &IconsMaxSizeHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// Selecting a new font updates font sample
	DoMethod(app->Obj[POP_ICONFONT], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[MCC_ICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_StdFontDesc, MUIV_TriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- TrueType fonts page
	// disable icon font selection if icon TT font enabled
	DoMethod(app->Obj[CHECK_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_ICONSPAGE_ICONFONT], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
	// update TT icon font enable buttons
	DoMethod(app->Obj[CHECK_ICONSPAGE_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[CHECK_TTICONFONT_ENABLE], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[CHECK_ICONSPAGE_TTICONFONT_ENABLE], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_ICONSPAGE_TTICONFONT], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
	// disable text window font selection if text window TT font enabled
	DoMethod(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_TEXTWINDOW_FONT_SELECT], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_TEXTWINDOW_FONT_SELECT], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- DragnDrop Page
	DoMethod(app->Obj[CYCLE_TRANSPMODE], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &IconDragTransparencyHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- TextWindows Page
	// Selecting a new font updates font sample
	DoMethod(app->Obj[POP_TEXTMODEFONT], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, 
		app->Obj[STRING_TEXTMODEFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_StdFontDesc, MUIV_TriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Miscellaneous page
	DoMethod(app->Obj[POP_WORKBENCHPREFS], MUIM_Notify, MUIA_Selected, FALSE, 
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &Pop_WorkbenchPrefs_Hook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Modules Page
	DoMethod(app->Obj[NLIST_MODULES], MUIM_Notify, MUIA_NList_ButtonClick, MUIV_EveryTime,
		app->Obj[WINDOW_MAIN], 2, MUIM_CallHook, &ModulesPrefsPopupHook);
	DoMethod(app->Obj[NLIST_MODULES], MUIM_NList_Insert, cModulePrefs, -1, MUIV_NList_Insert_Bottom);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	{ STRPTR xxx = strdup("TestTestTest"); if (xxx) free(xxx); }

	// --- Plugins Page
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		MUIV_Notify_Window, 2, MUIM_CallHook, &PluginListActiveHook);
	// call hook if "add plugin" button is clicked
	DoMethod(app->Obj[ADD_PLUGIN], MUIM_Notify, MUIA_Selected, FALSE, 
		app->Obj[PLUGIN_LIST], 2, MUIM_CallHook, &Add_Plugin_Hook);
	// call hook if "remove plugin" button is clicked
	DoMethod(app->Obj[REMOVE_PLUGIN], MUIM_Notify, MUIA_Pressed, FALSE, 
		app->Obj[PLUGIN_LIST], 2, MUIM_CallHook, &RemovePluginHook);
        /* Call the AppMsgHook when an icon is dropped on plugin listview */
        DoMethod(app->Obj[PLUGIN_LISTVIEW], MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
                 app->Obj[PLUGIN_LISTVIEW], 3, MUIM_CallHook, &PluginListAppMsgHook, MUIV_TriggerValue);
	// setup sorting hooks for plugin list
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		app->Obj[PLUGIN_LIST], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		app->Obj[PLUGIN_LIST], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		app->Obj[PLUGIN_LIST], 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(app->Obj[PLUGIN_LIST], MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		app->Obj[PLUGIN_LIST], 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// --- General Prefs GUI
	DoMethod(app->Obj[WINDOW_MAIN], MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
		MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(app->Obj[LIST], MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, 
		app->Obj[GROUP], 3, MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue);
	DoMethod(app->Obj[CANCEL], MUIM_Notify, MUIA_Pressed, FALSE, 
		MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(app->Obj[SAVE], MUIM_Notify, MUIA_Pressed, FALSE, 
		MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_SAVE);
	DoMethod(app->Obj[USE], MUIM_Notify, MUIA_Pressed, FALSE, 
		MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_USE);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// --- List Maintainance
	DoMethod(app->Obj[STORAGE_TIPS], MUIM_NList_Insert, cIconToolTips, -1, MUIV_NList_Insert_Bottom);

	SetAttrs(app->Obj[LIST], MUIA_NList_Active, MUIV_NList_Active_Top, TAG_DONE);

	// set allowed drag/drop partners for text window columns
	SetAttrs(app->Obj[NLIST_STORAGE_FILEDISPLAY], MUIA_UserData, (ULONG) app->Obj[NLIST_USE_FILEDISPLAY], TAG_DONE);
	SetAttrs(app->Obj[NLIST_USE_FILEDISPLAY], MUIA_UserData, (ULONG) app->Obj[NLIST_STORAGE_FILEDISPLAY], TAG_DONE);

	// set allowed drag/drop partners for control bar gadgets
	SetAttrs(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_AVAILABLE], MUIA_UserData, (ULONG) app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], TAG_DONE);
	SetAttrs(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIA_UserData, (ULONG) app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_AVAILABLE], TAG_DONE);

	// set allowed drag/drop partners for control bar gadgets
	SetAttrs(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_AVAILABLE], MUIA_UserData, (ULONG) app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], TAG_DONE);
	SetAttrs(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIA_UserData, (ULONG) app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_AVAILABLE], TAG_DONE);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// call hook if an active Control bar gadget is selected
	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE], 2, MUIM_CallHook, &ControlBarGadgetListBrowserActiveHook);

	// call hook if an active Control bar gadget is selected
	DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE], 2, MUIM_CallHook, &ControlBarGadgetListNormalActiveHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// Now disable several controls that are only allowed for screen depth > 8
	if ((NULL == CyberGfxBase) || (WBScreenDepth <= 8))
		{
		SetAttrs(app->Obj[CHECK_STRIPED_WINDOW],
			MUIA_Disabled, TRUE,
			MUIA_Selected, FALSE,
			TAG_END);
		SetAttrs(app->Obj[SLIDER_ICONTRANSPARENCY_DRAG], 
			MUIA_Disabled, TRUE,
			MUIA_Numeric_Value, 100,
			TAG_END);
		SetAttrs(app->Obj[SLIDER_ICONTRANSPARENCY_SHADOW], 
			MUIA_Disabled, TRUE,
			MUIA_Numeric_Value, 100,
			TAG_END);
		SetAttrs(app->Obj[SLIDER_ICONTRANSPARENCY_DEFICONS], 
			MUIA_Disabled, TRUE,
			MUIA_Numeric_Value, 100,
			TAG_END);
		SetAttrs(app->Obj[SLIDER_ICONTOOLTIPS_TRANSPARENCY],
			MUIA_Disabled, TRUE,
			MUIA_Numeric_Value, 100,
			TAG_END);
		SetAttrs(app->Obj[CYCLE_ROUTINE], 
			MUIA_Disabled, TRUE,
			MUIA_Cycle_Active, TRANSPTYPE_Ghosted,
			TAG_END);
		SetAttrs(app->Obj[SLIDER_ICONTRANSPARENCY_THUMBNAILBACK],
			MUIA_Disabled, TRUE,
			MUIA_Cycle_Active, TRANSPTYPE_Ghosted,
			TAG_END);
		}

	if ((NULL == CyberGfxBase) || (WBScreenDepth <= 8) || (NULL == ScalosGfxBase))
		{
		set(app->Obj[GROUP_TEXTWINDOWS_SELECTIONMARK], MUIA_Disabled, TRUE);
		}

	// Enable TrueType controls if ttengine.library is available
	if (TTEngineBase)
		{
		SetAttrs(app->Obj[GROUP_TTGLOBALS], 
			MUIA_Disabled, FALSE,
			TAG_END);
		SetAttrs(app->Obj[GROUP_TTSCREENFONT], 
			MUIA_Disabled, FALSE,
			TAG_END);
		SetAttrs(app->Obj[GROUP_TTICONFONT], 
			MUIA_Disabled, FALSE,
			TAG_END);
		SetAttrs(app->Obj[GROUP_ICONSPAGE_TTICONFONT],
			MUIA_Disabled, FALSE,
			TAG_END);
		SetAttrs(app->Obj[GROUP_TTTEXTWINDOWFONT], 
			MUIA_Disabled, FALSE,
			TAG_END);
		SetAttrs(app->Obj[GROUP_TEXTWINDOW_TTTEXTWINDOWFONT],
			MUIA_Disabled, FALSE,
			TAG_END);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// After icon font change, recalculate maximum radius for selected icon text rectangle
	DoMethod(app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);
	DoMethod(app->Obj[CHECK_ICONSPAGE_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);
	DoMethod(app->Obj[CHECK_TTICONFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);
	DoMethod(app->Obj[POP_ICONFONT], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);
	DoMethod(app->Obj[POPSTRING_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);
	DoMethod(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[GROUP], 2, MUIM_CallHook, &CalculateMaxRadiusHook);

	DoMethod(app->Obj[POPSTRING_TTSCREENFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
		app->Obj[MCC_TTSCREENFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);
	// update TT icon font samples
	DoMethod(app->Obj[POPSTRING_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[MCC_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);
	DoMethod(app->Obj[POPSTRING_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);

	DoMethod(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[MCC_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);
	DoMethod(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);

	DoMethod(app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[POPSTRING_TTICONFONT], 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);
	DoMethod(app->Obj[POPSTRING_TTICONFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[POPSTRING_ICONSPAGE_TTICONFONT], 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);

	DoMethod(app->Obj[POPSTRING_TTTEXTWINDOWFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
		app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);
	DoMethod(app->Obj[POPSTRING_TTTEXTWINDOWFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_TTFontDesc, MUIV_TriggerValue);

	DoMethod(app->Obj[POPSTRING_TEXTWINDOW_TTTEXTWINDOWFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[POPSTRING_TTTEXTWINDOWFONT], 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);
	DoMethod(app->Obj[POPSTRING_TTTEXTWINDOWFONT], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		app->Obj[POPSTRING_TEXTWINDOW_TTTEXTWINDOWFONT], 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);

	// update TT icon font enable buttons
	DoMethod(app->Obj[CHECK_TEXTWINDOW_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[CHECK_TEXTWINDOW_TTTEXTWINDOWFONT_ENABLE], 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
	DoMethod(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_TEXTWINDOW_TTTEXTWINDOWFONT], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
	// disable text window font selection if text window TT font enabled
	DoMethod(app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[GROUP_TEXTWINDOW_FONT_SELECT], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

	// Update font samples if antialias is changed
	DoMethod(app->Obj[CYCLE_TTANTIALIAS], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		app->Obj[MCC_TTSCREENFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Antialias, MUIV_TriggerValue);
	DoMethod(app->Obj[CYCLE_TTANTIALIAS], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		app->Obj[MCC_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Antialias, MUIV_TriggerValue);
	DoMethod(app->Obj[CYCLE_TTANTIALIAS], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Antialias, MUIV_TriggerValue);
	DoMethod(app->Obj[CYCLE_TTANTIALIAS], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Antialias, MUIV_TriggerValue);
	DoMethod(app->Obj[CYCLE_TTANTIALIAS], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Antialias, MUIV_TriggerValue);

	// Update font samples if gamma is changed
	DoMethod(app->Obj[SLIDER_TTGAMMA], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		app->Obj[MCC_TTSCREENFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Gamma, MUIV_TriggerValue);
	DoMethod(app->Obj[SLIDER_TTGAMMA], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		app->Obj[MCC_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Gamma, MUIV_TriggerValue);
	DoMethod(app->Obj[SLIDER_TTGAMMA], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Gamma, MUIV_TriggerValue);
	DoMethod(app->Obj[SLIDER_TTGAMMA], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Gamma, MUIV_TriggerValue);
	DoMethod(app->Obj[SLIDER_TTGAMMA], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE], 3, MUIM_Set, MUIA_FontSample_Gamma, MUIV_TriggerValue);

	// Disable POPUP_SELECTED_HOTKEY if CHECK_MISCPAGE_POPUP_SELECTED_ALWAYS is enabled
	DoMethod(app->Obj[CHECK_MISCPAGE_POPUP_SELECTED_ALWAYS], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		app->Obj[POPUP_SELECTED_HOTKEY], 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// sort Action Button command list
	DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTION], MUIM_NList_Sort);

	// use selected action command on double click into listview
	DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTION], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTION], 2, MUIM_CallHook, &BrowserCmdSelectedHook);

	// use selected action command on double click into listview
	DoMethod(app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTION], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTION], 2, MUIM_CallHook, &NormalCmdSelectedHook);

	// call ControlBarGadgetBrowserChangedHook to update the attributes of the selected control bar Gadget
	//  everytime any of the strings below is modified
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE], 2, MUIM_CallHook, &ControlBarGadgetBrowserChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE], 2, MUIM_CallHook, &ControlBarGadgetBrowserChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE], 2, MUIM_CallHook, &ControlBarGadgetBrowserChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION], 2, MUIM_CallHook, &ControlBarGadgetBrowserChangedHook);
	DoMethod(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], MUIM_Notify, MUIA_TextEditor_HasChanged, TRUE,
		app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT], 2, MUIM_CallHook, &ControlBarGadgetBrowserChangedHook);

	// call ControlBarGadgetNormalChangedHook to update the attributes of the selected control bar Gadget
	//  everytime any of the strings below is modified
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE], 2, MUIM_CallHook, &ControlBarGadgetNormalChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE], 2, MUIM_CallHook, &ControlBarGadgetNormalChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE], 2, MUIM_CallHook, &ControlBarGadgetNormalChangedHook);
	DoMethod(app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION], 2, MUIM_CallHook, &ControlBarGadgetNormalChangedHook);
	DoMethod(app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], MUIM_Notify, MUIA_TextEditor_HasChanged, TRUE,
		app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT], 2, MUIM_CallHook, &ControlBarGadgetNormalChangedHook);

	// Update sample icon whenever thumbnail size is changed
	DoMethod(app->Obj[CYCLE_THUMBNAILSIZE], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		app->Obj[CYCLE_THUMBNAILSIZE], 2, MUIM_CallHook, &ChangeThumbnailSizeHook);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// Menu
	DoMethod(app->Obj[MENU_QUIT], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(app->Obj[MENU_OPEN], MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &OpenHook);

	DoMethod(app->Obj[MENU_SAVE_AS], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &SaveAsHook);

	DoMethod(app->Obj[MENU_LAST_SAVED], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &LastSavedHook);

	DoMethod(app->Obj[MENU_RESET_TO_DEFAULTS], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &ResetToDefaultsHook);

	DoMethod(app->Obj[MENU_RESTORE], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &RestoreHook);

	DoMethod(app->Obj[MENU_ABOUT], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &AboutHook);

#if defined(__MORPHOS__)
	if (app->Obj[WIN_ABOUT_MORPHOS])
		{
		DoMethod(app->Obj[MENU_ABOUT_MORPHOS], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			MUIV_Notify_Application, 2, MUIM_CallHook, &AboutMorphosHook);

		DoMethod(app->Obj[WIN_ABOUT_MORPHOS], MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			app->Obj[WIN_ABOUT_MORPHOS], 2, MUIA_Window_Open, FALSE);
		}
#endif //defined(__MORPHOS__)

	DoMethod(app->Obj[MENU_ABOUT_MUI], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &AboutMUIHook);

	DoMethod(app->Obj[MENU_CREATE_ICONS], MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &CreateIconsHook);

	//Icon border selection window
	DoMethod(app->Obj[WINDOW_ICONFRAMES], MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
		app->Obj[WINDOW_ICONFRAMES], 3, MUIM_Set, MUIA_Window_Open, FALSE);

	// Update select marker sample
	DoMethod(app->Obj[SLIDER_TEXTWINDOWS_SELECTFILLTRANSPARENCY], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		app->Obj[MCC_TEXTWINDOWS_SELECTMARKER_SAMPLE], 3, MUIM_Set, TIHA_Transparency, MUIV_TriggerValue);
	DoMethod(app->Obj[SLIDER_TEXTWINDOWS_SELECTBORDERTRANSPARENCY], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &UpdateSelectMarkerSampleHook);
	DoMethod(app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK], MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
		MUIV_Notify_Application, 2, MUIM_CallHook, &UpdateSelectMarkerSampleHook);

	// Set notifications for HotkeyStrings
	SetHotkeyNotifications(app->Obj[SINGLE_WINDOW_LASSO_HOTKEY], app->Obj[SINGLE_WINDOW_LASSO_HOTKEY_SCAN]);
	SetHotkeyNotifications(app->Obj[POPUP_SELECTED_HOTKEY], app->Obj[POPUP_SELECTED_HOTKEY_SCAN]);
	SetHotkeyNotifications(app->Obj[COPY_HOTKEY], app->Obj[COPY_HOTKEY_SCAN]);
	SetHotkeyNotifications(app->Obj[MAKELINK_HOTKEY], app->Obj[MAKELINK_HOTKEY_SCAN]);
	SetHotkeyNotifications(app->Obj[MOVE_HOTKEY], app->Obj[MOVE_HOTKEY_SCAN]);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (MUIV_Application_ReturnID_EDIT == Action)
		{
		// open main window
		SetAttrs(app->Obj[WINDOW_MAIN],
			MUIA_Window_DefaultObject, (ULONG) app->Obj[LISTVIEW],
			MUIA_Window_Open, TRUE,
			TAG_DONE);

		if (!getv(app->Obj[WINDOW_MAIN], MUIA_Window_Open))
			{
	        	set(app->Obj[WINDOW_SPLASH], MUIA_Window_Open, FALSE);
			return FALSE;
			}
		}

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	// close Splash window
	set(app->Obj[WINDOW_SPLASH], MUIA_Window_Open, FALSE);

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

#if 0
	{
	struct List *ChildList = NULL;

	GetAttr(MUIA_Group_ChildList, app->Obj[GROUP], (ULONG *) &ChildList);
	if (ChildList)
		{
		Object *obj = (Object *) ChildList->lh_Head;
		Object *child;

		while ((child = NextObject(&obj)))
			{
			d1(KPrintF(__FILE__ "/%s/%ld: child=%08lx  MinWidth=%-4ld  MinHeight=%-4ld  MaxWidth=%-4ld  MaxHeight=%-4ld  DefWidth=%-4ld  DefHeight=%-4ld\n", \
				__FUNC__, __LINE__, child,
				_minwidth(child), _minheight(child), \
				_maxwidth(child), _maxheight(child), \
				_defwidth(child), _defheight(child)));
			}
		}
	}
#endif

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	return (TRUE);
}


static LONG MainLoop(LONG Action, struct SCAModule *app)
{
	BOOL Running = TRUE;

	while (Running)
		{
		ULONG sigs = 0;

		Action = DoMethod(app->Obj[APPLICATION], MUIM_Application_NewInput, &sigs);

		switch(Action)
			{
		case MUIV_Application_ReturnID_Quit:
		case MUIV_Application_ReturnID_SAVE:
		case MUIV_Application_ReturnID_USE:
			Running = FALSE;
			break;
			}

		if(sigs && (sigs = Wait(sigs | SIGBREAKF_CTRL_C)) & SIGBREAKF_CTRL_C)
			{
			Action = MUIV_Application_ReturnID_Quit;
			Running = FALSE;
			}
		}

	return Action;
}


int main(int argc, char *argv[])
{
	LONG Action = MUIV_Application_ReturnID_EDIT;
	// CONST_STRPTR GivenFileName = NULL; // not used
	struct SCAModule app;
	BPTR oldDir = (BPTR)NULL;
	struct RDArgs *rdArgs = NULL;
	struct DiskObject *icon = NULL;
	APTR prWindowPtr;
	struct Process *MyProcess = (struct Process *) FindTask(NULL);
	int Result = RETURN_FAIL;
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	d1(KPrintF(__FILE__ "/%s/%ld: starting up\n", __FUNC__, __LINE__));

	prWindowPtr = MyProcess->pr_WindowPtr;
	MyProcess->pr_WindowPtr = (APTR) ~0;	// suppress error requesters

	// --- Startup The Business
	if(Init(&app))
	{
		d1(KPrintF(__FILE__ "/%s/%ld: init done\n", __FUNC__, __LINE__));

		if (WBenchMsg && WBenchMsg->sm_ArgList)
			{
			static char PrgNamePath[512];
			struct WBArg *arg;

			d1(KPrintF(__FILE__ "/%s/%ld: WBenchMsg\n", __FUNC__, __LINE__));

			if (WBenchMsg->sm_NumArgs > 1)
				{
				arg = &WBenchMsg->sm_ArgList[1];
				// GivenFileName = arg->wa_Name;
				}
			else
				{
				arg = &WBenchMsg->sm_ArgList[0];
				}

			ProgramName = PrgNamePath;

			NameFromLock(WBenchMsg->sm_ArgList[0].wa_Lock, PrgNamePath, sizeof(PrgNamePath));
			AddPart(PrgNamePath, WBenchMsg->sm_ArgList[0].wa_Name, sizeof(PrgNamePath));

			oldDir = CurrentDir(arg->wa_Lock);

			icon = GetDiskObject(arg->wa_Name);
			if (icon)
				{
				STRPTR tt;

				tt = FindToolType(icon->do_ToolTypes, "NOPREFSPLUGINS");
				if (tt)
					SkipPrefsPlugins = TRUE;

				tt = FindToolType(icon->do_ToolTypes, "CREATEICONS");
				if (tt)
					{
					if (MatchToolValue(tt, "NO"))
						fCreateIcons = FALSE;
					}

				tt = FindToolType(icon->do_ToolTypes, "ACTION");
				if (tt)
					{
					if (MatchToolValue(tt, "EDIT"))
						Action = MUIV_Application_ReturnID_EDIT;
					if (MatchToolValue(tt, "USE"))
						Action = MUIV_Application_ReturnID_USE;
					if (MatchToolValue(tt, "SAVE"))
						Action = MUIV_Application_ReturnID_SAVE;
					}

				tt = FindToolType(icon->do_ToolTypes, "NOSPLASHWINDOW");
				if (tt)
					ShowSplashWindow = FALSE;
				}
			}
		else
			{
			IPTR ArgArray[4];

			d1(KPrintF(__FILE__ "/%s/%ld: CLI\n", __FUNC__, __LINE__));

			ProgramName = argv[0];

			memset(ArgArray, 0, sizeof(ArgArray));

			rdArgs = ReadArgs("FROM,EDIT/S,USE/S,SAVE/S", ArgArray, NULL);

			if (ArgArray[0])
				{
				// GivenFileName = (CONST_STRPTR) ArgArray[0];
				}
			if (ArgArray[1])
				Action = MUIV_Application_ReturnID_EDIT;
			if (ArgArray[2])
				Action = MUIV_Application_ReturnID_USE;
			if (ArgArray[3])
				Action = MUIV_Application_ReturnID_SAVE;
			}

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		ReadScalosPrefs("ENV:scalos/scalos.prefs");

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		PrefsDiskObject = GetDiskObject(ProgramName);

		d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

		if (BuildApp(Action, &app, icon))
			{
			d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

			// --- Display The GUI
			if (MUIV_Application_ReturnID_EDIT == Action)
				Action = MainLoop(Action, &app);

			switch (Action)
				{
			case MUIV_Application_ReturnID_SAVE:
				set(app.Obj[TEXT_SPLASHWINDOW], MUIA_Text_Contents, (ULONG) GetLocString(MSGID_SPLASHWINDOW_SAVINGPREFS));
				if (ShowSplashWindow)
					set(app.Obj[WINDOW_SPLASH], MUIA_Window_Open, TRUE);

				DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_SaveConfig);
				WriteScalosPrefs(&app, "ENV:scalos/scalos.prefs");
				WriteScalosPrefs(&app, "ENVARC:scalos/scalos.prefs");
				WriteFontPrefs(&app, "ENVARC:sys/font.prefs");
				WriteFontPrefs(&app, "ENV:sys/font.prefs");
				WriteWorkbenchPrefs("ENVARC:sys/workbench.prefs");
				WriteWorkbenchPrefs("ENV:sys/workbench.prefs");
				break;
			case MUIV_Application_ReturnID_USE:
				set(app.Obj[TEXT_SPLASHWINDOW], MUIA_Text_Contents, (ULONG) GetLocString(MSGID_SPLASHWINDOW_APPLYINGPREFS));
				if (ShowSplashWindow)
					set(app.Obj[WINDOW_SPLASH], MUIA_Window_Open, TRUE);

				DoMethodForAllPrefsPlugins(1, MUIM_ScalosPrefs_UseConfig);
				WriteScalosPrefs(&app, "ENV:scalos/scalos.prefs");
				WriteFontPrefs(&app, "ENV:sys/font.prefs");
				WriteWorkbenchPrefs("ENV:sys/workbench.prefs");
				break;
				}
			}

		MUI_DisposeObject(app.Obj[APPLICATION]);

		memset(app.Obj, 0, sizeof(app.Obj));

		d1(KPrintF(__FILE__ "/%s/%ld: just about to cleanup\n", __FUNC__, __LINE__));

		// restore pr_WindowPtr
		MyProcess->pr_WindowPtr = prWindowPtr;
	}

	if (icon)
		FreeDiskObject(icon);
	if (oldDir)
		CurrentDir(oldDir);
	if (rdArgs)
		FreeArgs(rdArgs);

	Cleanup(&app);

	// restore pr_WindowPtr
	MyProcess->pr_WindowPtr = prWindowPtr;

	d1(KPrintF(__FILE__ "/%s/%ld: END Result=%ld\n", __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT PluginListConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct PluginDef *dp = AllocPooled(nlcm->pool, sizeof(struct PluginDef));

	(void) unused;

	if (dp)
		{
		*dp = *((struct PluginDef *) nlcm->entry);
		}

	return (APTR) dp;
}


static SAVEDS(void) INTERRUPT PluginListDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	(void) unused;

	FreePooled(nldm->pool, nldm->entry, sizeof(struct PluginDef));
}


static SAVEDS(ULONG) INTERRUPT PluginListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	if (nldm->entry)
		{
		struct PluginDef *pd = (struct PluginDef *) nldm->entry;

		if (!pd->plug_SuccessfullyLoaded)
			{
			// plugin could not be opened
			// display it italic in list!
			nldm->preparses[0] = MUIX_I;
			nldm->preparses[1] = MUIX_I;
			}

		nldm->strings[0] = pd->plug_RealName;
		nldm->strings[1] = pd->plug_filename;
		}
	else
		{
		// display titles
		nldm->strings[0] = (STRPTR) GetLocString(MSGID_PLUGINLIST_NAME);
		nldm->strings[1] = (STRPTR) GetLocString(MSGID_PLUGINLIST_PATH);
		}

	return 0;
}

static SAVEDS(LONG) INTERRUPT PluginListCompareHookFunc(struct Hook *hook, APTR unused, struct NList_CompareMessage *ncm)
{
	const struct PluginDef *pd1 = (struct PluginDef *) ncm->entry1;
	const struct PluginDef *pd2 = (struct PluginDef *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	d1(KPrintF(__FUNC__ "/%ld:  sort_type=%ld  sort_type2=%ld\n", __LINE__, ncm->sort_type, ncm->sort_type2));

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by real name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(pd2->plug_RealName, pd1->plug_RealName);
			else
				Result = Stricmp(pd1->plug_RealName, pd2->plug_RealName);
			break;
		case 1:		// sort by filename
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(pd2->plug_filename, pd1->plug_filename);
			else
				Result = Stricmp(pd1->plug_filename, pd2->plug_filename);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by realname
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(pd2->plug_RealName, pd1->plug_RealName);
				else
					Result = Stricmp(pd1->plug_RealName, pd2->plug_RealName);
				break;
			case 1:		// sort by filename
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(pd2->plug_filename, pd1->plug_filename);
				else
					Result = Stricmp(pd1->plug_filename, pd2->plug_filename);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}


//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT PageListConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct PageListData *pld = AllocPooled(nlcm->pool, sizeof(struct PageListData));

	(void) unused;

	if (pld)
		{
		const struct NewPageListEntry *nple = (const struct NewPageListEntry *) nlcm->entry;

		d1(KPrintF(__FILE__ "/%s/%ld: nple_ImageIndex=%lu  nple_TitleString=<%s>  nple_TitleImage=%08lx\n", \
			__FUNC__, __LINE__, nple->nple_ImageIndex, nple->nple_TitleString, nple->nple_TitleImage));

		snprintf(pld->ImageSpec, sizeof(pld->ImageSpec), "\33o[%ld]", (long) nple->nple_ImageIndex);
		pld->TextLine = nple->nple_TitleString;
		}

	return (APTR) pld;
}


static SAVEDS(void) INTERRUPT PageListDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	(void) unused;

	FreePooled(nldm->pool, nldm->entry, sizeof(struct PageListData));
}

static SAVEDS(ULONG) INTERRUPT PageListDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	if (nldm->entry)
		{
		struct PageListData *pld = (struct PageListData *) nldm->entry;

		nldm->strings[0] = pld->ImageSpec;
		nldm->strings[1] = (STRPTR) pld->TextLine;
		nldm->preparses[0] = MUIX_C;
		}
	else
		{
		// display titles
		nldm->strings[0] = "";
		nldm->strings[1] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT FileDisplayConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct FileDisplayListEntry *fdle = AllocPooled(nlcm->pool, sizeof(struct FileDisplayListEntry));

	(void) unused;

	if (fdle)
		{
		struct FileDisplayListEntry *newFdle = (struct FileDisplayListEntry *) nlcm->entry;

		fdle->fdle_Index = newFdle->fdle_Index;
		}

	return (APTR) fdle;
}


static SAVEDS(void) INTERRUPT FileDisplayDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	(void) unused;

	FreePooled(nldm->pool, nldm->entry, sizeof(struct FileDisplayListEntry));
}


static SAVEDS(ULONG) INTERRUPT FileDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	if (nldm->entry)
		{
		struct FileDisplayListEntry *fdle = (struct FileDisplayListEntry *) nldm->entry;

		nldm->strings[0] = (STRPTR) cTextWindowsColumns[fdle->fdle_Index];
		}
	else
		{
		// display titles
		nldm->strings[0] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ControlBarGadgetListConstructHookFunc(struct Hook *hook, Object *o, struct NList_ConstructMessage *nlcm)
{
	struct ControlBarGadgetEntry *cgy = AllocPooled(nlcm->pool, sizeof(struct ControlBarGadgetEntry));

	if (cgy)
		{
		const struct ControlBarGadgetEntry *cgyOrig = (struct ControlBarGadgetEntry *) nlcm->entry;

		*cgy = *cgyOrig;

		cgy->cgy_ImageObjectIndex = ControlBarGadgetImageIndex++;

		if (SCPGadgetType_ActionButton == cgy->cgy_GadgetType)
			{
			cgy->cgy_NormalImage = cgyOrig->cgy_NormalImage ? strdup(cgyOrig->cgy_NormalImage) : NULL;
			cgy->cgy_SelectedImage = cgyOrig->cgy_SelectedImage ? strdup(cgyOrig->cgy_SelectedImage) : NULL;
			cgy->cgy_DisabledImage = cgyOrig->cgy_DisabledImage ? strdup(cgyOrig->cgy_DisabledImage) : NULL;
			cgy->cgy_HelpText = cgyOrig->cgy_HelpText ? strdup(cgyOrig->cgy_HelpText) : NULL;
			}
		else
			{
			// replace empty image names by strings from DefaultControlBarGadgets[]
			ULONG n;

			for (n = 0; n < Sizeof(DefaultControlBarGadgets); n++)
				{
				if (DefaultControlBarGadgets[n].cyi_GadgetType == cgy->cgy_GadgetType)
					{
					if (cgyOrig->cgy_NormalImage && strlen(cgyOrig->cgy_NormalImage) > 0)
						cgy->cgy_NormalImage = strdup(cgyOrig->cgy_NormalImage);
					else
						cgy->cgy_NormalImage   = strdup(DefaultControlBarGadgets[n].cyi_NormalImage);

					if (cgyOrig->cgy_SelectedImage && strlen(cgyOrig->cgy_SelectedImage) > 0)
						cgy->cgy_SelectedImage = strdup(cgyOrig->cgy_SelectedImage);
					else
						cgy->cgy_SelectedImage = strdup(DefaultControlBarGadgets[n].cyi_SelectedImage);

					if (cgyOrig->cgy_DisabledImage && strlen(cgyOrig->cgy_DisabledImage) > 0)
						cgy->cgy_DisabledImage = strdup(cgyOrig->cgy_DisabledImage);
					else
						cgy->cgy_DisabledImage = strdup(DefaultControlBarGadgets[n].cyi_DisabledImage);
					break;
					}
				}	
			}

		if (ExistsObject(cgy->cgy_NormalImage))
			{
			d1(KPrintF("%s/%s/%ld:  cgy_NormalImage=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage));
			d1(KPrintF("%s/%s/%ld:  cgy_NormalImage=<%s>\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage));

			cgy->cgy_Image = DataTypesImageObject,
				MUIA_ScaDtpic_Name, (ULONG) cgy->cgy_NormalImage,
				End; //DataTypesMCC

			d1(KPrintF("%s/%s/%ld:  <%s>  cgy_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage, cgy->cgy_Image));
			}
		else
			{
			cgy->cgy_Image = NULL;
			}

		d1(KPrintF("%s/%s/%ld:  <%s>  cgy_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage, cgy->cgy_Image));

		DoMethod(o, MUIM_NList_UseImage, cgy->cgy_Image, cgy->cgy_ImageObjectIndex, 0);

		d1(KPrintF("%s/%s/%ld:  <%s>  cgy_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_NormalImage, cgy->cgy_Image));
		}

	d1(KPrintF("%s/%s/%ld:  END cgy=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy));

	return (APTR) cgy;
}


static SAVEDS(void) INTERRUPT ControlBarGadgetListDestructHookFunc(struct Hook *hook, Object *o, struct NList_DestructMessage *nldm)
{
	struct ControlBarGadgetEntry *cgy = (struct ControlBarGadgetEntry *) nldm->entry;

	if (cgy)
		{
		if (cgy->cgy_Image)
			{
			DoMethod(o, MUIM_NList_UseImage, NULL, cgy->cgy_ImageObjectIndex, 0);

			MUI_DisposeObject(cgy->cgy_Image);
			cgy->cgy_Image = NULL;
			}
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
		if (cgy->cgy_HelpText)
			{
			free(cgy->cgy_HelpText);
			cgy->cgy_HelpText = NULL;
			}

		FreePooled(nldm->pool, cgy, sizeof(struct ControlBarGadgetEntry));
		}
}


static SAVEDS(ULONG) INTERRUPT ControlBarGadgetListDisplayHookFunc(struct Hook *hook, Object *o, struct NList_DisplayMessage *nldm)
{
	(void) o;

	if (nldm->entry)
		{
		struct ControlBarGadgetEntry *cgy = (struct ControlBarGadgetEntry *) nldm->entry;

		if (cgy->cgy_GadgetType < Sizeof(cControlBarGadgetTypes) - 1)
			nldm->strings[0] = (STRPTR) cControlBarGadgetTypes[cgy->cgy_GadgetType];
		else
			nldm->strings[0] = (STRPTR) GetLocString(MSGID_SCPGADGETTYPE_UNKNOWN);

		snprintf(cgy->cgy_ImageString, sizeof(cgy->cgy_ImageString), "\033o[%lu]", (unsigned long) cgy->cgy_ImageObjectIndex);
		nldm->strings[1] = cgy->cgy_ImageString;
		nldm->strings[2] = cgy->cgy_Action;
		}
	else
		{
		// display titles
		nldm->strings[0] = (STRPTR) GetLocString(MSGID_CONTROLBARGADGETLIST_TYPE);
//		nldm->strings[1] = (STRPTR) GetLocString(MSGID_CONTROLBARGADGETLIST_PATH);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CmdListConstructHookFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *msg)
{
	(void) hook;
	(void) obj;

	return msg->entry;
}

static SAVEDS(void) INTERRUPT CmdListDestructHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *msg)
{
	(void) hook;
	(void) obj;
	(void) msg;
}


static SAVEDS(ULONG) INTERRUPT CmdListDisplayHookFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct CommandTableEntry *cte = (struct CommandTableEntry *) ltdm->entry;

	d1(KPrintF(__FILE__ "/%s/%ld: cte=%08lx\n", __FUNC__, __LINE__, cte));

	if (cte)
		{
		ltdm->preparses[0] = "";

		ltdm->strings[0] = (STRPTR) GetLocString(cte->cte_NameMsgId);
		ltdm->strings[1] = (STRPTR) cte->cte_Command;
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "";
		ltdm->preparses[1] = "";

		ltdm->strings[0] = "";
		ltdm->strings[1] = "";
		}

	return 0;
}


static SAVEDS(LONG) INTERRUPT CmdListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	const struct CommandTableEntry *cte1 = (const struct CommandTableEntry *) ncm->entry1;
	const struct CommandTableEntry *cte2 = (const struct CommandTableEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by full name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(GetLocString(cte2->cte_NameMsgId), GetLocString(cte1->cte_NameMsgId));
			else
				Result = Stricmp(GetLocString(cte1->cte_NameMsgId), GetLocString(cte2->cte_NameMsgId));
			break;
		case 1:		// sort by internal name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(cte2->cte_Command, cte1->cte_Command);
			else
				Result = Stricmp(cte1->cte_Command, cte2->cte_Command);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by full name
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(GetLocString(cte2->cte_NameMsgId), GetLocString(cte1->cte_NameMsgId));
				else
					Result = Stricmp(GetLocString(cte1->cte_NameMsgId), GetLocString(cte2->cte_NameMsgId));
				break;
			case 1:		// sort by internal name
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(cte2->cte_Command, cte1->cte_Command);
				else
					Result = Stricmp(cte1->cte_Command, cte2->cte_Command);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

CONST_STRPTR GetLocString(LONG MsgId)
{
	struct Scalos_Prefs_LocaleInfo li;

	li.li_Catalog = ScalosPrefsCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetScalos_PrefsString(&li, MsgId);
}

static void TranslateStringArray(CONST_STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

//----------------------------------------------------------------------------

LONG SaveIcon(CONST_STRPTR IconName)
{
	struct DiskObject *icon, *allocIcon;

	static UWORD ImageData[] =
		{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x07, 0xFF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x18, 0x00, 0x60, 0x00, 0x10, 0x00,
		0x00, 0x00, 0x20, 0xFC, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00, 0x41, 0x02, 0x08, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x40, 0x82, 0x08, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x40, 0x82, 0x08, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x21, 0x04, 0x08, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x1E, 0x18, 0x10, 0x00, 0x0C, 0x00,
		0x00, 0x00, 0x00, 0x60, 0x20, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x02, 0x1C, 0x00, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 
		0x40, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 
		0xD5, 0x55, 0x55, 0x55, 0x55, 0x56, 0x00, 0x00, 0xD5, 0x55, 0x50, 0x00, 0x55, 0x55, 0x80, 0x00, 
		0xD5, 0x55, 0x47, 0xFF, 0x95, 0x55, 0x60, 0x00, 0xD5, 0x55, 0x5F, 0x03, 0xE5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x3E, 0x55, 0xF5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x3F, 0x55, 0xF5, 0x55, 0x50, 0x00,
		0xD5, 0x55, 0x3F, 0x55, 0xF5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x5E, 0x53, 0xF5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x41, 0x47, 0xE5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x55, 0x1F, 0xD5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0x7F, 0x15, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xFC, 0x55, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0xE1, 0x55, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0x05, 0x55, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00,
		0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00, 0x35, 0x55, 0x55, 0x05, 0x55, 0x55, 0x50, 0x00, 
		0x0D, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x00, 0x03, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		};

	static struct Image NormalImage =
		{
		0, 0, 54, 23,
		2,
		ImageData,
		3, 0,
		NULL
		};

	static STRPTR defToolTypes[] =
		{
		"ACTION=USE",
		NULL
		};

	static struct DiskObject DefaultIcon =
		{
		WB_DISKMAGIC, WB_DISKVERSION,

		{
		NULL, 
		0, 0, 54, 24,
		GFLG_GADGIMAGE | GFLG_GADGHBOX,
		GACT_RELVERIFY | GACT_IMMEDIATE,
		GTYP_BOOLGADGET,
		&NormalImage, NULL,
		NULL,
		0,
		NULL,
		0,
		NULL
		},

		WBPROJECT,
		NULL,
		defToolTypes,
		NO_ICON_POSITION, NO_ICON_POSITION,
		NULL,
		NULL,
		8192
		};

	icon = allocIcon = GetDiskObject("ENV:sys/def_ScaMainPrefs");
	if (NULL == icon)
		icon = allocIcon = GetDiskObject("ENV:sys/def_pref");
	if (NULL == icon)
		icon = &DefaultIcon;

	if (icon)
		{
		STRPTR oldDefaultTool = icon->do_DefaultTool;

		icon->do_DefaultTool = ProgramName;

		PutDiskObject((STRPTR) IconName, icon);

		icon->do_DefaultTool = oldDefaultTool;

		if (allocIcon)
			FreeDiskObject(allocIcon);
		}

	return 0;
}

//----------------------------------------------------------------------------

//static INLINE Object *PrecisionSliderMCCObject(Tag tags, ...)
//{
//	  return NewObjectA(PrecisionSliderClass->mcc_Class, 0, (struct TagItem *) &tags);
//}

//----------------------------------------------------------------------------

DISPATCHER(PrecisionSlider)
{
	if (msg->MethodID==MUIM_Numeric_Stringify)
	{
		struct PrecisionSliderData *data = INST_DATA(cl,obj);
		struct MUIP_Numeric_Stringify *m = (APTR)msg;

		switch (m->value)
			{
		case 1:
			strcpy(data->buf, GetLocString(MSGID_PRECISION_EXACT));
			break;
		case 2:
			strcpy(data->buf, GetLocString(MSGID_PRECISION_IMAGE));
			break;
		case 3:
			strcpy(data->buf, GetLocString(MSGID_PRECISION_ICON));
			break;
		case 4:
		default:
			strcpy(data->buf, GetLocString(MSGID_PRECISION_GUI));
			break;
			}

		return (ULONG) data->buf;
	}
	return(DoSuperMethodA(cl,obj,msg));
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(BufferSizeSlider)
{
	if (msg->MethodID == MUIM_Numeric_Stringify)
		{
		struct PrecisionSliderData *data = INST_DATA(cl,obj);
		struct MUIP_Numeric_Stringify *m = (APTR)msg;
		ULONG size;

		if (m->value < 20)
			{
                        size = 1 << (m->value - 10);	// Convert to KBytes
			snprintf(data->buf, sizeof(data->buf), "%ldK", (long) size);
			}
		else
			{
			size = 1 << (m->value - 20);	// Convert to MBytes
                        snprintf(data->buf, sizeof(data->buf), "%ldM", (long) size);
			}

		return (ULONG) data->buf;
		}

	return DoSuperMethodA(cl, obj, msg);
}
DISPATCHER_END

//----------------------------------------------------------------------------

//static INLINE Object *TTGammaSliderMCCObject(Tag tags, ...)
//{
//	  return NewObjectA(TTGammaSliderClass->mcc_Class, 0, (struct TagItem *) &tags);
//}

//----------------------------------------------------------------------------

DISPATCHER(TTGammaSlider)
{
	if (msg->MethodID==MUIM_Numeric_Stringify)
	{
		struct TTGammaSliderData *data = INST_DATA(cl,obj);
		struct MUIP_Numeric_Stringify *m = (APTR)msg;

		snprintf(data->buf, sizeof(data->buf), "%ld.%03ld", (long) (m->value / 1000), (long) (m->value % 1000));

		return (ULONG) data->buf;
	}
	return(DoSuperMethodA(cl,obj,msg));
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(ThumbnailLifetimeSlider)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_Numeric_Stringify:
		{
		struct ThumbnailLifetimeSliderData *data = INST_DATA(cl,obj);
		struct MUIP_Numeric_Stringify *mstr = (struct MUIP_Numeric_Stringify *) msg;

		if (0 == mstr->value)
			Result = (ULONG) GetLocString(MSGID_THUMBNAILS_LIFETIME_FOREVER);
		else
			{
			snprintf(data->buf, sizeof(data->buf), MUIX_C "%lu %s",
				(unsigned long) mstr->value,
				GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE_DAYS));
			Result = (ULONG) data->buf;
			}
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//-----------------------------------------------------------------

static ULONG TextWindowColumnsListDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
	d1(kprintf(__FUNC__ "/%ld:  src=%08lx  obj=%08lx  userdata=%08lx\n", __LINE__, msg->obj, obj, muiUserData(obj)));

	if (msg->obj == obj)
	{
		/*
		** If somebody tried to drag ourselves onto ourselves, we let our superclass
		** (the list class) handle the necessary actions. Depending on the state of
		** its MUIA_List_DragSortable attribute, it will either accept or refuse to become
		** the destination object.
		*/

		return DoSuperMethodA(cl,obj, (Msg) msg);
	}
	else if (msg->obj == (Object *) muiUserData(obj))
	{
		/*
		** If our predefined source object wants us to become active,
		** we politely accept it.
		*/

		return MUIV_DragQuery_Accept;
	}
	else
	{
		/*
		** Otherwise, someone tried to feed us with something we don't like
		** very much. Just refuse it.
		*/

		return MUIV_DragQuery_Refuse;
	}

}


static ULONG TextWindowColumnsListDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
	if (msg->obj == obj)
		{
		return DoSuperMethodA(cl, obj, (Msg) msg);
		}
	else
	{
		/*
		** we can be sure that msg->obj is our predefined source object
		** since we wouldn't have accepted the MUIM_DragQuery and wouldn't
		** have become an active destination object otherwise.
		*/

		LONG idArray[Sizeof(cTextWindowsColumns)];
		LONG dropmark = 0;
		LONG sortable = 0;
		LONG id = MUIV_NList_NextSelected_Start;
		LONG ArrayIndex;

		get(obj, MUIA_NList_DragSortable, &sortable);
		get(obj, MUIA_NList_DropMark, &dropmark);

		ArrayIndex = 0;
		while (1)
			{
			APTR entry;

			DoMethod(msg->obj, MUIM_NList_NextSelected, &id);

			if (MUIV_NList_NextSelected_End == id)
				break;

			idArray[ArrayIndex] = id;

			/* get source entry */
			DoMethod(msg->obj, MUIM_NList_GetEntry, id, &entry);

			if (sortable)
				{
				DoMethod(obj, MUIM_NList_InsertSingle, entry, dropmark);
				dropmark++;
				}
			else
				{
				/*
				** we are about to return something to the available fields
				** listview which is always sorted.
				*/

				DoMethod(obj, MUIM_NList_InsertSingle, entry, MUIV_NList_Insert_Sorted);
				}
			ArrayIndex++;
			}

		while (ArrayIndex--)
			{
			set(msg->obj, MUIA_NList_Active, MUIV_NList_Active_Off);

			/* remove source entry */
			DoMethod(msg->obj, MUIM_NList_Remove, idArray[ArrayIndex]);
			}

		/*
		** make the inserted object the active and make the source listviews
		** active object inactive to give some more visual feedback to the user.
		*/

		get(obj, MUIA_NList_InsertPosition, &dropmark);
		set(obj, MUIA_NList_Active, dropmark);
		set(msg->obj, MUIA_NList_Active, MUIV_NList_Active_Off);

		return(0);
	}
}


DISPATCHER(TextWindowColumnsList)
{
	switch (msg->MethodID)
	{
	case MUIM_DragQuery: 
		return TextWindowColumnsListDragQuery(cl,obj,(APTR)msg);
		break;
	case MUIM_DragDrop:
		return TextWindowColumnsListDragDrop(cl,obj,(APTR)msg);
		break;
	}
	return DoSuperMethodA(cl,obj,msg);
}
DISPATCHER_END

//-----------------------------------------------------------------

static ULONG ControlBarGadgetsListDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
	d1(kprintf(__FUNC__ "/%ld:  src=%08lx  obj=%08lx  userdata=%08lx\n", __LINE__, msg->obj, obj, muiUserData(obj)));

	if (msg->obj == obj)
	{
		/*
		** If somebody tried to drag ourselves onto ourselves, we let our superclass
		** (the list class) handle the necessary actions. Depending on the state of
		** its MUIA_List_DragSortable attribute, it will either accept or refuse to become
		** the destination object.
		*/

		return DoSuperMethodA(cl,obj, (Msg) msg);
	}
	else if (msg->obj == (Object *) muiUserData(obj))
	{
		/*
		** If our predefined source object wants us to become active,
		** we politely accept it.
		*/

		return MUIV_DragQuery_Accept;
	}
	else
	{
		/*
		** Otherwise, someone tried to feed us with something we don't like
		** very much. Just refuse it.
		*/

		return MUIV_DragQuery_Refuse;
	}

}


static ULONG ControlBarGadgetsListDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	if (msg->obj == obj)
		{
		return DoSuperMethodA(cl, obj, (Msg) msg);
		}
	else
	{
		/*
		** we can be sure that msg->obj is our predefined source object
		** since we wouldn't have accepted the MUIM_DragQuery and wouldn't
		** have become an active destination object otherwise.
		*/

		LONG dropmark = 0;
		LONG id = MUIV_NList_NextSelected_Start;
		ULONG SourceIsDragSortable = FALSE;
		ULONG DestIsDragSortable = FALSE;

		get(obj, MUIA_NList_DragSortable, &DestIsDragSortable);
		get(msg->obj, MUIA_NList_DragSortable, &SourceIsDragSortable);
		get(obj, MUIA_NList_DropMark, &dropmark);

		while (1)
			{
			APTR entry;

			DoMethod(msg->obj, MUIM_NList_NextSelected, &id);

			d1(KPrintF(__FILE__ "/%s/%ld: id=%ld\n", __FUNC__, __LINE__, id));

			if (MUIV_NList_NextSelected_End == id)
				break;

			/* get source entry */
			DoMethod(msg->obj, MUIM_NList_GetEntry, id, &entry);

			// add to destination -- only to active list
			if (DestIsDragSortable)
				{
				DoMethod(obj, MUIM_NList_InsertSingle, entry, dropmark);
				dropmark++;
				}
			}

		/* remove source entries -- only from active list */
		if (SourceIsDragSortable)
			{
			DoMethod(msg->obj, MUIM_NList_Remove, MUIV_NList_Remove_Selected);
			}

		/*
		** make the inserted object the active and make the source listviews
		** active object inactive to give some more visual feedback to the user.
		*/

		get(obj, MUIA_NList_InsertPosition, &dropmark);
		set(obj, MUIA_NList_Active, dropmark);

		// deselect everything on source object
		set(msg->obj, MUIA_NList_Active, MUIV_NList_Active_Off);
		DoMethod(msg->obj,MUIM_NList_Select, MUIV_NList_Select_All, MUIV_NList_Select_Off, NULL);

		d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));

		return(0);
	}
}


DISPATCHER(ControlBarGadgetsList)
{
	switch (msg->MethodID)
	{
	case MUIM_DragQuery:
		return ControlBarGadgetsListDragQuery(cl,obj,(APTR)msg);
		break;
	case MUIM_DragDrop:
		return ControlBarGadgetsListDragDrop(cl,obj,(APTR)msg);
		break;
	}
	return DoSuperMethodA(cl,obj,msg);
}
DISPATCHER_END

//-----------------------------------------------------------------

static VOID Cleanup(struct SCAModule *app)
{
	ReadWritePrefsCleanup();
	PrefsPluginsCleanup();

	if (app->Obj[WIN_ABOUT_MORPHOS])
		{
		MUI_DisposeObject(app->Obj[WIN_ABOUT_MORPHOS]);
		app->Obj[WIN_ABOUT_MORPHOS] = NULL;
		}
	if (WBScreen)
		{
		UnlockPubScreen(NULL, WBScreen);
		WBScreen = NULL;
		}

	if (PrefsDiskObject)
		{
		FreeDiskObject(PrefsDiskObject);
		PrefsDiskObject = NULL;
		}

	if (ScalosPrefsCatalog)
		{
		CloseCatalog(ScalosPrefsCatalog);
		ScalosPrefsCatalog = NULL;
		}

	if (LocaleBase)
		{
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
		}
	if (ControlBarGadgetsListClass)
		{
		MUI_DeleteCustomClass(ControlBarGadgetsListClass);
		ControlBarGadgetsListClass = NULL;
		}
	if (TextWindowColumnsListClass)
		{
		MUI_DeleteCustomClass(TextWindowColumnsListClass);
		TextWindowColumnsListClass = NULL;
		}
	if (SelectMarkSampleClass)
		{
		CleanupSelectMarkSampleClass(SelectMarkSampleClass);
		SelectMarkSampleClass = NULL;
		}
	if (McpFrameClass)
		{
		CleanupMcpFrameClass(McpFrameClass);
		McpFrameClass = NULL;
		}
	if (DataTypesImageClass)
		{
		CleanupDtpicClass(DataTypesImageClass);
		DataTypesImageClass = NULL;
		}
	if (FontSampleClass)
		{
		CleanupFontSampleClass(FontSampleClass);
		FontSampleClass = NULL;
		}
	if (PrecisionSliderClass)
		{
		MUI_DeleteCustomClass(PrecisionSliderClass);
		PrecisionSliderClass = NULL;
		}
	if (BufferSizeSliderClass)
		{
		MUI_DeleteCustomClass(BufferSizeSliderClass);
		BufferSizeSliderClass = NULL;
		}
	if (ThumbnailLifetimeSliderClass)
		{
		MUI_DeleteCustomClass(ThumbnailLifetimeSliderClass);
		ThumbnailLifetimeSliderClass = NULL;
		}
	if (TTGammaSliderClass)
		{
		MUI_DeleteCustomClass(TTGammaSliderClass);
		TTGammaSliderClass = NULL;
		}

	// --- Close Libraries

#ifdef __amigaos4__
	if (IPreferences)
		{
		DropInterface((struct Interface *)IPreferences);
		IPreferences = NULL;
		}
#endif //__amigaos4__
	if (PreferencesBase)
		{
		CloseLibrary(PreferencesBase);
		PreferencesBase = NULL;
		}
#ifdef __amigaos4__
	if (IWorkbench)
		{
		DropInterface((struct Interface *)IWorkbench);
		IWorkbench = NULL;
		}
#endif //__amigaos4__
	if (WorkbenchBase)
		{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
		}
#ifdef __amigaos4__
	if (ICyberGfx)
		{
		DropInterface((struct Interface *)ICyberGfx);
		ICyberGfx = NULL;
		}
#endif //__amigaos4__
	if (CyberGfxBase)
		{
		CloseLibrary(CyberGfxBase);
		CyberGfxBase = NULL;
		}
#ifdef __amigaos4__
	if (ITTEngine)
		{
		DropInterface((struct Interface *)ITTEngine);
		ITTEngine = NULL;
		}
#endif //__amigaos4__
	if (TTEngineBase)
		{
		CloseLibrary(TTEngineBase);
		TTEngineBase = NULL;
		}
#ifdef __amigaos4__
	if (IScalosGfx)
		{
		DropInterface((struct Interface *)IScalosGfx);
		IScalosGfx = NULL;
		}
#endif //__amigaos4__
	if (ScalosGfxBase)
		{
		CloseLibrary(ScalosGfxBase);
		ScalosGfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif //__amigaos4__
	if (ScalosBase)
		{
		CloseLibrary(&ScalosBase->scb_LibNode);
		ScalosBase = NULL;
		}
#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif //__amigaos4__
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
		}
#ifdef __amigaos4__
	if (IMUIMaster)
		{
		DropInterface((struct Interface *)IMUIMaster);
		IMUIMaster = NULL;
		}
#endif //__amigaos4__
	if (MUIMasterBase)
		{
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif //__amigaos4__
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
#ifdef __amigaos4__
	if (IAsl)
		{
		DropInterface((struct Interface *)IAsl);
		IAsl = NULL;
		}
#endif //__amigaos4__
	if (AslBase)
		{
		CloseLibrary(AslBase);
		AslBase = NULL;
		}
#ifndef __amigaos4__
	if (UtilityBase)
		{
		CloseLibrary((struct Library *)UtilityBase);
		UtilityBase = NULL;
		}
#endif //__amigaos4__
#ifdef __amigaos4__
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
#endif //__amigaos4__
	if (GfxBase)
		{
		CloseLibrary((struct Library *)GfxBase);
		GfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif //__amigaos4__
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
		}
#ifdef __amigaos4__
	if (IDiskfont)
		{
		DropInterface((struct Interface *)IDiskfont);
		IDiskfont = NULL;
		}
#endif //__amigaos4__
	if (DiskfontBase)
		{
		CloseLibrary(DiskfontBase);
		DiskfontBase = NULL;
		}
}


static BOOL Init(struct SCAModule *app)
{
	BOOL Success = FALSE;

	memset(app, 0, sizeof(struct SCAModule));

	NewList(&PluginList);
	NewList(&ControlBarGadgetListNormal);
	NewList(&ControlBarGadgetListBrowser);

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	do	{
		DiskfontBase = OpenLibrary("diskfont.library", 39);
		if (NULL == DiskfontBase)
			{
			Printf("Error: Unable to open 'diskfont.library' v39\n");
			break;
			}
#ifdef __amigaos4__
		IDiskfont = (struct DiskfontIFace *)GetInterface((struct Library *)DiskfontBase, "main", 1, NULL);
		if (NULL == IDiskfont)
			{
			Printf("Error: Could not get 'diskfont.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
		if(! (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39) ))
			{
			Printf("Error: Unable to open 'Intuition.library' v39\n");
			break;
			}
#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			{
			Printf("Error: Could not get 'intuition.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
		if(! (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",39) ))
			{
			Printf("Error: Unable to open 'graphics.library' v39\n");
			break;
			}
#ifdef __amigaos4__
		IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			{
			Printf("Error: Could not get 'graphics.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
#ifndef __amigaos4__
		if(! (UtilityBase = OpenLibrary("utility.library",39) ))
			{
			MsgBox("Unable to open 'utility.library v39", "Error", NULL);
			break;
			}
#endif //__amigaos4__
		IconBase = OpenLibrary("icon.library", 39);
		if (NULL == IconBase)
			{
			MsgBox("Unable to open 'icon.library v39", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			{
			Printf("Error: Could not get 'icon.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
		AslBase = OpenLibrary("asl.library", 39);
		if (NULL == AslBase)
			{
			MsgBox("Unable to open 'asl.library v39", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IAsl = (struct AslIFace *)GetInterface((struct Library *)AslBase, "main", 1, NULL);
		if (NULL == IAsl)
			{
			Printf("Error: Could not get 'asl.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
		IFFParseBase = OpenLibrary("iffparse.library", 39);
		if (NULL == IFFParseBase)
			{
			MsgBox("Unable to open 'iffparse.library v39", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IIFFParse = (struct IFFParseIFace *)GetInterface((struct Library *)IFFParseBase, "main", 1, NULL);
		if (NULL == IIFFParse)
			{
			Printf("Error: Could not get 'iffparse.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__
		MUIMasterBase = OpenLibrary("zune.library", 0);
		if (NULL == MUIMasterBase)
			MUIMasterBase = OpenLibrary(MUIMASTER_NAME, 19);
		if (NULL == MUIMasterBase)
			{
			MsgBox("Unable to open 'muimaster.library v19", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		if (NULL == IMUIMaster)
			{
			Printf("Error: Could not get 'muimaster.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__

		PreferencesBase = OpenLibrary("preferences.library", 39);
		if (NULL == PreferencesBase)
			{
			MsgBox("Unable to open 'preferences.library' v39", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			{
			Printf("Error: Could not get 'preferences.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__

		WorkbenchBase = OpenLibrary("workbench.library", 39);
		if (NULL == WorkbenchBase)
			{
			MsgBox("Unable to open 'workbench.library' v39", "Error", NULL);
			break;
			}
#ifdef __amigaos4__
		IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
		if (NULL == IWorkbench)
			{
			Printf("Error: Could not get 'diskfont.library' OS4 interface\n");
			break;
			}
#endif //__amigaos4__

		// TTEngineBase is optional
		TTEngineBase = OpenLibrary(TTENGINENAME, 6);
#ifdef __amigaos4__
		if (TTEngineBase)
			ITTEngine = (struct TTEngineIFace *)GetInterface((struct Library *)TTEngineBase, "main", 1, NULL);
#endif //__amigaos4__

		// CyberGfxBase is optional
		CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
#ifdef __amigaos4__
		if (CyberGfxBase)
			ICyberGfx = (struct CyberGfxIFace *)GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
#endif //__amigaos4__

		// ScalosBase is optional
		ScalosBase = (struct ScalosBase *) OpenLibrary(SCALOSNAME, 39);
#ifdef __amigaos4__
		if (ScalosBase)
			IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
#endif //__amigaos4__

		// ScalosGfxBase is optional
		ScalosGfxBase = OpenLibrary(SCALOSGFXNAME, 42);
#ifdef __amigaos4__
		if (ScalosGfxBase)
			IScalosGfx = (struct ScalosGfxIFace *)GetInterface(ScalosGfxBase, "main", 1, NULL);
#endif //__amigaos4__

		// LocaleBase is optional
		LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
		if (LocaleBase)
			ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif //__amigaos4__
		if (LocaleBase)
			ScalosPrefsCatalog = OpenCatalogA(NULL, "Scalos/Scalos_Prefs.catalog", NULL);

		PrecisionSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct PrecisionSliderData), DISPATCHER_REF(PrecisionSlider));
		if (NULL == PrecisionSliderClass)
			{
			MsgBox("Unable to create PrecisionSliderClass", "Error", NULL);
			break;
			}

		BufferSizeSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct PrecisionSliderData), DISPATCHER_REF(BufferSizeSlider));
		if (NULL == BufferSizeSliderClass)
			{
			MsgBox("Unable to create BufferSizeSliderClass", "Error", NULL);
			break;
			}

		ThumbnailLifetimeSliderClass = MUI_CreateCustomClass(NULL, MUIC_Numericbutton,
			NULL, sizeof(struct ThumbnailLifetimeSliderData), DISPATCHER_REF(ThumbnailLifetimeSlider));
		if (NULL == ThumbnailLifetimeSliderClass)
			{
			MsgBox("Unable to create ThumbnailLifetimeSliderClass", "Error", NULL);
			break;
			}

		TTGammaSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct TTGammaSliderData), DISPATCHER_REF(TTGammaSlider));
		if (NULL == TTGammaSliderClass)
			{
			MsgBox("Unable to create TTGammaSliderClass", "Error", NULL);
			break;
			}

		TextWindowColumnsListClass = MUI_CreateCustomClass(NULL, MUIC_NList, NULL, 0, DISPATCHER_REF(TextWindowColumnsList));
		if (NULL == TextWindowColumnsListClass)
			{
			MsgBox("Unable to create TextWindowColumnsListClass", "Error", NULL);
			break;
			}

		ControlBarGadgetsListClass = MUI_CreateCustomClass(NULL, MUIC_NList, NULL, 0, DISPATCHER_REF(ControlBarGadgetsList));
		if (NULL == ControlBarGadgetsListClass)
			{
			MsgBox("Unable to create ControlBarGadgetsListClass", "Error", NULL);
			break;
			}

		FontSampleClass = InitFontSampleClass();
		if (NULL == FontSampleClass)
			{
			MsgBox("Unable to create FontSampleClass", "Error", NULL);
			break;
			}

		if (!PrefsPluginsInit())
			{
			MsgBox("PrefsPluginsInit() failed", "Error", NULL);
			break;
			}

		WBScreen = LockPubScreen("Workbench");
		if (NULL == WBScreen)
			{
			MsgBox("Unable to lock workbench screen", "Error", NULL);
			break;
			}

		WBScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

		DataTypesImageClass = InitDtpicClass();
		if (NULL == DataTypesImageClass)
			{
			MsgBox("Unable to create DataTypesImageClass", "Error", NULL);
			break;
			}

		McpFrameClass = InitMcpFrameClass();
		if (NULL == McpFrameClass)
			{
			MsgBox("Unable to create McpFrameClass", "Error", NULL);
			break;
			}

		SelectMarkSampleClass = InitSelectMarkSampleClass();
		if (NULL == SelectMarkSampleClass)
			{
			MsgBox("Unable to create SelectMarkSampleClass", "Error", NULL);
			break;
			}

		if (!ReadWritePrefsInit())
			{
			MsgBox("ReadWritePrefsInit() failed.", "Error", NULL);
			break;
			}

		// --- Everything opened fine
		Success = TRUE;
		} while (0);

	d1(KPrintF(__FILE__ "/%s/%ld: END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}


// --- A Simple Message Requester
VOID MsgBox(char *message, char *title, struct SCAModule *app)
{
	if(MUIMasterBase && app->Obj[APPLICATION])
	{
		MUI_Request(app->Obj[APPLICATION],
			NULL,
			0,
			title,
			"Ok",
			message
			);
	}
	else if (IntuitionBase)
	{
		struct EasyStruct ER =
		{
			sizeof(struct EasyStruct),
			0,
			"",
			"",
			"Ok",
		};

	ER.es_Title = title;
	ER.es_TextFormat = message;

		EasyRequest(NULL, &ER, NULL, NULL);
	}

}

BOOL ExistsObject(CONST_STRPTR Name)
{
	BOOL Exists = FALSE;
	BPTR fLock;

	d1(KPrintF("%s/%s/%ld:  START Name=%08lx\n", __FILE__, __FUNC__, __LINE__, Name));

	if (Name)
		{
		d1(KPrintF("%s/%s/%ld:  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

		fLock = Lock(Name, ACCESS_READ);
		if (fLock)
		        {
		        Exists = TRUE;
		        UnLock(fLock);
		        }
		else
		        {
		        LONG Err = IoErr();

		        switch (Err)
			       {
		        case ERROR_OBJECT_IN_USE:
			       Exists = TRUE;
			       break;
			       }
		        }
		}

	d1(KPrintF("%s/%s/%ld:  END Exists=%ld\n", __FILE__, __FUNC__, __LINE__, Exists));

	return Exists;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ModulesListConstructHookFunc(struct Hook *hook, Object *o, struct NList_ConstructMessage *nlcm)
{
	struct ModuleListEntry *mdle = (struct ModuleListEntry *) AllocPooled(nlcm->pool, sizeof(struct ModuleListEntry));

	d1(KPrintF("%s/%s/%ld:  START mdle=%08lx\n", __FILE__, __FUNC__, __LINE__, mdle));

	if (mdle)
		{
		CONST_STRPTR ModuleName = (CONST_STRPTR) nlcm->entry;
		STRPTR dlp;
		CONST_STRPTR lp;
		STRPTR tempLine;
		char tempName[108];
		BOOL EnablePrefs;
		size_t len;

		d1(KPrintF("%s/%s/%ld:  ModuleName=<%s>\n", __FILE__, __FUNC__, __LINE__, ModuleName));

		stccpy(mdle->mdle_Modulename, ModuleName, sizeof(mdle->mdle_Modulename));
		snprintf(mdle->mdle_Filename, sizeof(mdle->mdle_Filename), "Scalos:Prefs/Modules/%s.prefs", ModuleName);

		for (dlp=tempName, lp=ModuleName; *lp && '.' != *lp; )
			*dlp++ = *lp++;
		*dlp = '\0';

		d1(KPrintF("%s/%s/%ld:  mdle_Filename=<%s>\n", __FILE__, __FUNC__, __LINE__, mdle->mdle_Filename));

		// --- Is the prefs executable accessible?
#if 1
		EnablePrefs = ExistsObject(mdle->mdle_Filename);
#else
		EnablePrefs = TRUE;
#endif

		mdle->mdle_ImageObjectIndex = ModuleImageIndex++;
		mdle->mdle_PopupObjectIndex = ModuleImageIndex++;
		mdle->mdle_EntryIndex = ModuleEntryIndex++;

		d1(KPrintF("%s/%s/%ld:  ModuleName=<%s>  tempName=<%s>\n", __FILE__, __FUNC__, __LINE__, ModuleName, tempName));

		len = 20 + strlen(ModuleName) + strlen(tempName) ;
		tempLine = malloc(len);
		if (tempLine)
			{
			snprintf(tempLine, len, "THEME:Modules/%s/%s", ModuleName, tempName);
			d1(KPrintF("%s/%s/%ld:  tempLine=<%s>\n", __FILE__, __FUNC__, __LINE__, tempLine));

			if (ExistsObject(tempLine))
				{
				d1(KPrintF("%s/%s/%ld:  tempLine=%08lx\n", __FILE__, __FUNC__, __LINE__, tempLine));
				d1(KPrintF("%s/%s/%ld:  tempLine=<%s>\n", __FILE__, __FUNC__, __LINE__, tempLine));

				mdle->mdle_Image = DataTypesImageObject,
					MUIA_ScaDtpic_Name, (ULONG) tempLine,
					End; //DataTypesMCC

				d1(kprintf(__FUNC__ "/%ld:  <%s>  mdle_Image=%08lx\n", __LINE__, tempLine, mdle->mdle_Image));

				DoMethod(o, MUIM_NList_UseImage,
					mdle->mdle_Image,
					mdle->mdle_ImageObjectIndex,
                                        0);
				}
			else
				{
				struct SCAModule *app = (struct SCAModule *) hook->h_Data;

				d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

				mdle->mdle_Image = NULL;

				DoMethod(o, MUIM_NList_UseImage,
					app->Obj[mdle->mdle_EntryIndex],
					mdle->mdle_ImageObjectIndex,
                                        0);
				}

			free(tempLine);
			}

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		if (EnablePrefs)
			{
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

			mdle->mdle_Popup = ImageObject,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_Image_Spec, "6:18", //MUII_PopUp,
				End; //Image

			DoMethod(o, MUIM_NList_UseImage,
				mdle->mdle_Popup, mdle->mdle_PopupObjectIndex, ~0);
			}
		else
			{
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			mdle->mdle_Popup = NULL;
			}
		}

	d1(KPrintF("%s/%s/%ld:  END  mdle=%08lx\n", __FILE__, __FUNC__, __LINE__, mdle));

	return (APTR) mdle;
}


static SAVEDS(void) INTERRUPT ModulesListDestructHookFunc(struct Hook *hook, Object *o, struct NList_DestructMessage *nldm)
{
	struct ModuleListEntry *mdle = (struct ModuleListEntry *) nldm->entry;

	if (mdle)
		{
		if (mdle->mdle_Image)
			{
			DoMethod(o, MUIM_NList_UseImage, NULL, mdle->mdle_ImageObjectIndex, 0);

			MUI_DisposeObject(mdle->mdle_Image);
			mdle->mdle_Image = NULL;
			}
		if (mdle->mdle_Popup)
			{
			DoMethod(o, MUIM_NList_UseImage, NULL, mdle->mdle_PopupObjectIndex, 0);

			MUI_DisposeObject(mdle->mdle_Popup);
			mdle->mdle_Popup = NULL;
			}

		FreePooled(nldm->pool, mdle, sizeof(struct ModuleListEntry));
		}
}


static SAVEDS(ULONG) INTERRUPT ModulesListDisplayHookFunc(struct Hook *hook, Object *o, struct NList_DisplayMessage *nldm)
{
	(void) o;

	if (nldm->entry)
		{
		struct ModuleListEntry *mdle = (struct ModuleListEntry *) nldm->entry;

		snprintf(mdle->mdle_ImageString, sizeof(mdle->mdle_ImageString), "\033o[%lu]", (unsigned long) mdle->mdle_ImageObjectIndex);

		if (mdle->mdle_Popup)
			snprintf(mdle->mdle_PopupObjectString, sizeof(mdle->mdle_PopupObjectString), "\033o[%lu@%lu]", (unsigned long) mdle->mdle_PopupObjectIndex, (unsigned long) mdle->mdle_EntryIndex);
		else
			strcpy(mdle->mdle_PopupObjectString, "");

		d1(kprintf(__FUNC__ "/%ld:  PopupObj=<%s>\n", __LINE__, mdle->mdle_PopupObjectString));

		nldm->strings[0] = mdle->mdle_ImageString;
		nldm->strings[1] = mdle->mdle_Modulename;
		nldm->strings[2] = mdle->mdle_PopupObjectString;

		nldm->preparses[0] = MUIX_C;
		nldm->preparses[2] = MUIX_C;
		}
	else
		{
		// display titles
		nldm->strings[0] = "";
		nldm->strings[1] = "";
		nldm->strings[2] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT HiddenDevicesConstructHookFunc(struct Hook *hook, APTR unused, struct NList_ConstructMessage *nlcm)
{
	struct HiddenDeviceListEntry *hde = (struct HiddenDeviceListEntry *) AllocPooled(nlcm->pool, sizeof(struct HiddenDeviceListEntry));
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	(void) unused;

	if (hde)
		{
		const struct NewHiddenDevice *nhd = (const struct NewHiddenDevice *) nlcm->entry;

		stccpy(hde->hde_DeviceName, nhd->nhd_DeviceName, sizeof(hde->hde_DeviceName) - 1);
		stccpy(hde->hde_VolumeName, nhd->nhd_VolumeName, sizeof(hde->hde_VolumeName) - 1);

		hde->hde_CheckboxObjectIndex = HiddenDevicesImageIndex++;
		hde->hde_EntryIndex = HiddenDevicesEntryIndex++;

		hde->hde_Hidden = nhd->nhd_Hidden;

		hde->hde_CheckboxImage = ImageObject,
			ImageButtonFrame,
//			MUIA_InputMode, MUIV_InputMode_Toggle,
			MUIA_InputMode, MUIV_InputMode_Immediate,
			MUIA_Image_Spec, MUII_CheckMark,
			MUIA_Image_FreeVert, TRUE,
			MUIA_Selected, hde->hde_Hidden,
			MUIA_Background, MUII_ButtonBack,
			MUIA_ShowSelState, FALSE,
			MUIA_CycleChain, TRUE,
			End;

		d1(KPrintF(__FUNC__ "/%ld:  <%s>  hde_CheckboxImage=%08lx\n", __LINE__, hde->hde_DeviceName, hde->hde_CheckboxImage));

		DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_UseImage,
			hde->hde_CheckboxImage, hde->hde_CheckboxObjectIndex, 0);
		}

	return (APTR) hde;
}


static SAVEDS(void) INTERRUPT HiddenDevicesDestructHookFunc(struct Hook *hook, APTR unused, struct NList_DestructMessage *nldm)
{
	struct HiddenDeviceListEntry *hde = (struct HiddenDeviceListEntry *) nldm->entry;
	struct SCAModule *app = (struct SCAModule *) hook->h_Data;

	(void) unused;

	if (hde)
		{
		if (hde->hde_CheckboxImage)
			{
			DoMethod(app->Obj[NLIST_HIDDENDEVICES], MUIM_NList_UseImage, NULL, hde->hde_CheckboxObjectIndex, 0);

			MUI_DisposeObject(hde->hde_CheckboxImage);
			hde->hde_CheckboxImage = NULL;
			}

		FreePooled(nldm->pool, hde, sizeof(struct HiddenDeviceListEntry));
		}
}


static SAVEDS(ULONG) INTERRUPT HiddenDevicesDisplayHookFunc(struct Hook *hook, APTR unused, struct NList_DisplayMessage *nldm)
{
	(void) unused;

	if (nldm->entry)
		{
		struct HiddenDeviceListEntry *hde = (struct HiddenDeviceListEntry *) nldm->entry;

		snprintf(hde->hde_CheckboxString, sizeof(hde->hde_CheckboxString), "\033o[%lu@%lu]",
			(unsigned long) hde->hde_CheckboxObjectIndex,
			(unsigned long) nldm->entry_pos);

		nldm->strings[0] = hde->hde_CheckboxString;
		if (hde->hde_VolumeName[0])
			nldm->strings[1] = hde->hde_VolumeName;
		else
			nldm->strings[1] = hde->hde_DeviceName;

		nldm->preparses[0] = MUIX_C;
		nldm->preparses[2] = MUIX_C;
		}
	else
		{
		// display titles
		nldm->strings[0] = "";
		nldm->strings[1] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) INTERRUPT HiddenDevicesCompareHookFunc(struct Hook *hook, APTR unused, struct NList_CompareMessage *ncm)
{
//	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	const struct HiddenDeviceListEntry *hde1 = (const struct HiddenDeviceListEntry *) ncm->entry1;
	const struct HiddenDeviceListEntry *hde2 = (const struct HiddenDeviceListEntry *) ncm->entry2;

	return Stricmp(hde1->hde_DeviceName, hde2->hde_DeviceName);
}

//----------------------------------------------------------------------------

void SetThumbNailSize(struct SCAModule *app, UWORD ThumbnailSize)
{
	ULONG SizeIndex;

	if (ThumbnailSize <= 16)
		SizeIndex = THUMBNAILSIZE_16;
	else if (ThumbnailSize <= 24)
		SizeIndex = THUMBNAILSIZE_24;
	else if (ThumbnailSize <= 32)
		SizeIndex = THUMBNAILSIZE_32;
	else if (ThumbnailSize <= 48)
		SizeIndex = THUMBNAILSIZE_48;
	else if (ThumbnailSize <= 64)
		SizeIndex = THUMBNAILSIZE_64;
	else if (ThumbnailSize <= 96)
		SizeIndex = THUMBNAILSIZE_96;
	else
		SizeIndex = THUMBNAILSIZE_128;

	set(app->Obj[CYCLE_THUMBNAILSIZE], MUIA_Cycle_Active, SizeIndex);

	CallHookPkt(&ChangeThumbnailSizeHook, app->Obj[CYCLE_THUMBNAILSIZE], NULL);
}

//----------------------------------------------------------------------------

UWORD GetThumbNailSize(struct SCAModule *app)
{
	ULONG SizeIndex;
	UWORD Size;

	SizeIndex = getv(app->Obj[CYCLE_THUMBNAILSIZE], MUIA_Cycle_Active);

	switch (SizeIndex)
	{
	case THUMBNAILSIZE_16:
		Size = 16;
		break;
	case THUMBNAILSIZE_24:
		Size = 24;
		break;
	case THUMBNAILSIZE_32:
		Size = 32;
		break;
	case THUMBNAILSIZE_48:
		Size = 48;
		break;
	case THUMBNAILSIZE_64:
		Size = 64;
		break;
	case THUMBNAILSIZE_96:
		Size = 96;
		break;
	case THUMBNAILSIZE_128:
	default:
		Size = 128;
	}

	return Size;
}

//----------------------------------------------------------------------------

void SetIconSizeConstraints(struct SCAModule *app, const struct Rectangle *SizeConstraints)
{
	ULONG MinSize, MaxSize;

	if (SizeConstraints->MinX < 16)
		MinSize = ICONSIZEMIN_Unlimited;
	else if (SizeConstraints->MinX < 24)
		MinSize = ICONSIZEMIN_16;
	else if (SizeConstraints->MinX < 32)
		MinSize = ICONSIZEMIN_24;
	else if (SizeConstraints->MinX < 48)
		MinSize = ICONSIZEMIN_32;
	else if (SizeConstraints->MinX < 64)
		MinSize = ICONSIZEMIN_48;
	else if (SizeConstraints->MinX < 96)
		MinSize = ICONSIZEMIN_64;
	else if (SizeConstraints->MinX < 128)
		MinSize = ICONSIZEMIN_96;
	else
		MinSize = ICONSIZEMIN_128;

	if (SizeConstraints->MaxX <= 16)
		MaxSize = ICONSIZEMAX_16;
	else if (SizeConstraints->MaxX <= 24)
		MaxSize = ICONSIZEMAX_24;
	else if (SizeConstraints->MaxX <= 32)
		MaxSize = ICONSIZEMAX_32;
	else if (SizeConstraints->MaxX <= 48)
		MaxSize = ICONSIZEMAX_48;
	else if (SizeConstraints->MaxX <= 64)
		MaxSize = ICONSIZEMAX_64;
	else if (SizeConstraints->MaxX <= 96)
		MaxSize = ICONSIZEMAX_96;
	else if (SizeConstraints->MaxX <= 128)
		MaxSize = ICONSIZEMAX_128;
	else
		MaxSize = ICONSIZEMAX_Unlimited;

	set(app->Obj[CYCLE_ICONMINSIZE], MUIA_Cycle_Active, MinSize);
	set(app->Obj[CYCLE_ICONMAXSIZE], MUIA_Cycle_Active, MaxSize);

	AdjustIconSizeSample(app, SizeConstraints);
}

//----------------------------------------------------------------------------

void GetIconSizeConstraints(struct SCAModule *app, struct Rectangle *SizeConstraints)
{
	ULONG MinSize, MaxSize;

	MinSize	= getv(app->Obj[CYCLE_ICONMINSIZE], MUIA_Cycle_Active);
	MaxSize	= getv(app->Obj[CYCLE_ICONMAXSIZE], MUIA_Cycle_Active);

	switch (MinSize)
		{
	case ICONSIZEMIN_16:
		SizeConstraints->MinX = SizeConstraints->MinY = 16;
		break;
	case ICONSIZEMIN_24:
		SizeConstraints->MinX = SizeConstraints->MinY = 24;
		break;
	case ICONSIZEMIN_32:
		SizeConstraints->MinX = SizeConstraints->MinY = 32;
		break;
	case ICONSIZEMIN_48:
		SizeConstraints->MinX = SizeConstraints->MinY = 48;
		break;
	case ICONSIZEMIN_64:
		SizeConstraints->MinX = SizeConstraints->MinY = 64;
		break;
	case ICONSIZEMIN_96:
		SizeConstraints->MinX = SizeConstraints->MinY = 96;
		break;
	case ICONSIZEMIN_128:
		SizeConstraints->MinX = SizeConstraints->MinY = 128;
		break;
	case ICONSIZEMIN_Unlimited:
	default:
		SizeConstraints->MinX = SizeConstraints->MinY = 0;
		break;
		}

	switch (MaxSize)
		{
	case ICONSIZEMAX_16:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 16;
		break;
	case ICONSIZEMAX_24:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 24;
		break;
	case ICONSIZEMAX_32:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 32;
		break;
	case ICONSIZEMAX_48:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 48;
		break;
	case ICONSIZEMAX_64:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 64;
		break;
	case ICONSIZEMAX_96:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 96;
		break;
	case ICONSIZEMAX_128:
		SizeConstraints->MaxX = SizeConstraints->MaxY = 128;
		break;
	case ICONSIZEMAX_Unlimited:
	default:
		SizeConstraints->MaxX = SizeConstraints->MaxY = SHRT_MAX;
		break;
		}
}

//----------------------------------------------------------------------------

void AdjustIconSizeSample(struct SCAModule *app, const struct Rectangle *SizeConstraints)
{
	ULONG MinSampleIndex, MaxSampleIndex;

	if (SizeConstraints->MinX < 16)
		MinSampleIndex = 0;
	else if (SizeConstraints->MinX < 24)
		MinSampleIndex = 1;
	else if (SizeConstraints->MinX < 32)
		MinSampleIndex = 2;
	else if (SizeConstraints->MinX < 48)
		MinSampleIndex = 3;
	else if (SizeConstraints->MinX < 64)
		MinSampleIndex = 4;
	else if (SizeConstraints->MinX < 96)
		MinSampleIndex = 5;
	else if (SizeConstraints->MinX < 128)
		MinSampleIndex = 6;
	else
		MinSampleIndex = 7;

	if (SizeConstraints->MaxX <= 16)
		MaxSampleIndex = 1;
	else if (SizeConstraints->MaxX <= 24)
		MaxSampleIndex = 2;
	else if (SizeConstraints->MaxX <= 32)
		MaxSampleIndex = 3;
	else if (SizeConstraints->MaxX <= 48)
		MaxSampleIndex = 4;
	else if (SizeConstraints->MaxX <= 64)
		MaxSampleIndex = 5;
	else if (SizeConstraints->MaxX <= 96)
		MaxSampleIndex = 6;
	else if (SizeConstraints->MaxX <= 128)
		MaxSampleIndex = 7;
	else
		MaxSampleIndex = 0;

	set(app->Obj[GOUP_ICONSIZE_MIN_SAMPLE], MUIA_Group_ActivePage, MinSampleIndex);
	set(app->Obj[GOUP_ICONSIZE_MAX_SAMPLE], MUIA_Group_ActivePage, MaxSampleIndex);

	d1(KPrintF("%s/%s/%ld: MinSampleIndex=%lu  MaxSampleIndex=%lu", __FILE__, __FUNC__, __LINE__, MinSampleIndex, MaxSampleIndex);)
}

//----------------------------------------------------------------------------

static void EnableIconFontPrefs(struct SCAModule *app)
{
	ULONG EnableFontSet;
	BPTR fLock;

	// check if font prefs available
	fLock = Lock(SYS_FONTPREFS_NAME, ACCESS_READ);
	if (fLock)
		{
		EnableFontSet = FALSE;
		UnLock(fLock);
		}
	else
		{
		EnableFontSet = TRUE;
		}
	set(app->Obj[GROUP_ICONFONT_NOTICE], MUIA_ShowMe, !EnableFontSet);
	set(app->Obj[GROUP_ICONFONT_SELECT], MUIA_ShowMe, EnableFontSet);
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: %s ", __LINE__, name);)

	while (1)
		{
		ULONG ver = 0;
		ULONG rev = 0;
		struct Library *base;
		char libname[256];

		// First we attempt to acquire the version and revision through MUI
		Object *obj = MUI_NewObject((STRPTR) name, TAG_DONE);
		if (obj)
			{
			get(obj, MUIA_Version, &ver);
			get(obj, MUIA_Revision, &rev);

			MUI_DisposeObject(obj);

			if(ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __LINE__, ver, rev);)
				return TRUE;
				}
			}

		// If we did't get the version we wanted, let's try to open the
		// libraries ourselves and see what happens...
		stccpy(libname, "PROGDIR:mui", sizeof(libname));
		AddPart(libname, name, sizeof(libname));

		if ((base = OpenLibrary(&libname[8], 0)) || (base = OpenLibrary(&libname[0], 0)))
			{
			UWORD OpenCnt = base->lib_OpenCnt;

			ver = base->lib_Version;
			rev = base->lib_Revision;

			CloseLibrary(base);

			// we add some additional check here so that eventual broken .mcc also have
			// a chance to pass this test (i.e. Toolbar.mcc is broken)
			if (ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through OpenLibrary()\n", __LINE__, ver, rev);)
				return TRUE;
				}

			if (OpenCnt > 1)
				{
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_MCC_IN_USE),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}

			// Attempt to flush the library if open count is 0 or because the
			// user wants to retry (meaning there's a chance that it's 0 now)

			if (flush)
				{
				struct Library *result;
			
				Forbid();
				if ((result = (struct Library *) FindName(&((struct ExecBase *)SysBase)->LibList, name)))
					RemLibrary(result);
				Permit();
				flush = FALSE;
				}
			else
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: couldn`t find minimum required version.\n", __LINE__);)

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_OLD_MCC),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}
			}
		else
			{
			// No MCC at all - no need to attempt flush
			flush = FALSE;
			if (!MUI_Request(NULL, NULL, 0L,
				(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
				(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                (STRPTR) GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
}

//-----------------------------------------------------------------------------

struct TagItem *ScalosVTagList(ULONG FirstTag, va_list args)
{
	struct TagItem *ClonedTagList;
	ULONG AllocatedSize = 10;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: FirstTag=%08lx  args=%08lx\n", __LINE__, FirstTag, args));

	do	{
		ULONG NumberOfTags = 1;
		BOOL Finished = FALSE;
		struct TagItem *ti;

		ClonedTagList = ti = AllocateTagItems(AllocatedSize);
		if (NULL == ClonedTagList)
			break;

		ClonedTagList->ti_Tag = FirstTag;
		while (!Finished)
			{
			switch (ti->ti_Tag)
				{
			case TAG_MORE:
				ti->ti_Data = va_arg(args, ULONG);
				Finished = TRUE;
				break;
			case TAG_END:
				Finished = TRUE;
				break;
			default:
				ti->ti_Data = va_arg(args, ULONG);
				break;
				}

			d1(KPrintF(__FUNC__ "/%ld  ti=%08lx  Tag=%08lx  Data=%08lx\n", __LINE__, ti, ti->ti_Tag, ti->ti_Data));

			if (!Finished)
				{
				if (++NumberOfTags >= AllocatedSize)
					{
					// we need to extend our allocated taglist
					struct TagItem *ExtendedTagList;
					ULONG ExtendedSize = 2 * AllocatedSize;

					ExtendedTagList	= AllocateTagItems(ExtendedSize);
					if (NULL == ExtendedTagList)
						{
						FreeTagItems(ClonedTagList);
						ClonedTagList = NULL;
						break;
						}

					d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: ExtendedTagList=%08lx  ClonedTagList=%08lx\n", __LINE__, ExtendedTagList, ClonedTagList));

					// copy contents from old to extended taglist
					memcpy(ExtendedTagList, ClonedTagList, sizeof(struct TagItem) * AllocatedSize);

					// free old taglist
					FreeTagItems(ClonedTagList);
					ClonedTagList = ExtendedTagList;

					ti = ClonedTagList + NumberOfTags - 1;
					AllocatedSize = ExtendedSize;
					}
				else
					{
					ti++;
					}

				ti->ti_Tag = va_arg(args, ULONG);
				}
			}

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: NumberOfTags=%lu  AllocatedSize=%lu\n", __LINE__, NumberOfTags, AllocatedSize));
		} while (0);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: ClonedTagList=%08lx\n", __LINE__, ClonedTagList));

	d1({ ULONG n; \
		for (n = 0; ClonedTagList; n++) \
			{ \
			KPrintF(__FILE__ "/" __FUNC__ "/%ld: TagList[%ld]: Tag=%08lx  Data=%08lx\n", \
				__LINE__, n, ClonedTagList[n].ti_Tag, ClonedTagList[n].ti_Data); \
			if (TAG_END == ClonedTagList[n].ti_Tag || TAG_MORE == ClonedTagList[n].ti_Tag)
				break;
			} \
		})

	return ClonedTagList;
}

//----------------------------------------------------------------------------

static void InitControlBarGadgets(struct SCAModule *app)
{
	ULONG n;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	// Initialize list of available control bar gadgets
	for (n = 0; n < Sizeof(DefaultControlBarGadgets); n++)
		{
		struct ControlBarGadgetEntry cgy;

		memset(&cgy, 0, sizeof(cgy));

		cgy.cgy_GadgetType    = DefaultControlBarGadgets[n].cyi_GadgetType;
		cgy.cgy_NormalImage   = (STRPTR) DefaultControlBarGadgets[n].cyi_NormalImage;
		cgy.cgy_SelectedImage = (STRPTR) DefaultControlBarGadgets[n].cyi_SelectedImage;
		cgy.cgy_DisabledImage = (STRPTR) DefaultControlBarGadgets[n].cyi_DisabledImage;

		d1(KPrintF(__FILE__ "/%s/%ld:  add image  cgy_NormalImage=<%s>\n", __FUNC__, __LINE__, cgy.cgy_NormalImage));

		DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_AVAILABLE], MUIM_NList_InsertSingle, &cgy, MUIV_NList_Insert_Bottom);
		DoMethod(app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_AVAILABLE], MUIM_NList_InsertSingle, &cgy, MUIV_NList_Insert_Bottom);
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void SetupNewPrefsPages(struct SCAModule *app)
{
	struct NewPageListEntry *nple = NewPrefsPages;
	ULONG n;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	for (n = 0; n < Sizeof(cPrefGroups); n++)
		{
		nple->nple_TitleString = GetLocString(cPrefGroups[n]);
		nple->nple_TitleImage = app->Obj[n + IMAGE_ABOUT];
		nple->nple_ImageIndex = n;
		nple->nple_CreateTitleImage = CreatePrefsImage;
		nple->nple_UserData = (void *) (n + IMAGE_ABOUT);

		d1(KPrintF(__FILE__ "/%s/%ld: nple=%08lx  nple_ImageIndex=%lu  nple_TitleString=<%s>  nple_TitleImage=%08lx\n", \
			__FUNC__, __LINE__, nple, nple->nple_ImageIndex, \
                        nple->nple_TitleString, nple->nple_TitleImage));
		
		nple++;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

Object *CreatePrefsPage(struct SCAModule *app, Object *Page, struct NewPageListEntry *nple)
{
	Object *result = NULL;

	if (Page)
		{
		Object *nlistview;

#if defined(MUIA_Scrollgroup_AutoBars)
		result = ScrollgroupObject,
			MUIA_Scrollgroup_AutoBars, TRUE,
			MUIA_Scrollgroup_Contents, VGroupV,
#else //MUIA_Scrollgroup_AutoBars
		result = VGroupV,
#endif //MUIA_Scrollgroup_AutoBars
				MUIA_Group_VertSpacing, 0,
				Child, nlistview = NListviewObject,
						MUIA_VertWeight, 0,
						MUIA_InputMode, MUIV_InputMode_None,
						MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
						MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_None,
						MUIA_NListview_NList, NListObject,
								ReadListFrame,
								MUIA_VertWeight, 0,
								MUIA_Font, MUIV_Font_Big,
								MUIA_Background, MUII_ListBack,
								MUIA_NList_DefaultObjectOnClick, FALSE,
								MUIA_NList_DisplayHook2, (ULONG) &PageListDisplayHook,
								MUIA_NList_ConstructHook2, (ULONG) &PageListConstructHook,
								MUIA_NList_DestructHook2, (ULONG) &PageListDestructHook,
								MUIA_NList_Format, (ULONG) ",",
								MUIA_NList_ShowDropMarks, FALSE,
								MUIA_NList_Input, FALSE,
								MUIA_NList_AdjustHeight, TRUE,
								MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_None,
							End, //NListObject
						MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
//						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PREFGROUPS_SHORTHELP),
					End, //NListviewObject
				Child, Page,
#if defined(MUIA_Scrollgroup_AutoBars)
				End, //VGroupV
			End; //ScrollgroupObject
#else //defined(MUIA_Scrollgroup_AutoBars)
			End; //VGroupV
#endif //defined(MUIA_Scrollgroup_AutoBars)


		if (result)
			{
			DoMethod(app->Obj[LISTVIEW], MUIM_NList_InsertSingle,
				nple,
				MUIV_NList_Insert_Bottom);
			DoMethod(nlistview, MUIM_NList_InsertSingle,
				nple,
				MUIV_NList_Insert_Bottom);

			if (nple->nple_CreateTitleImage)
				{
				DoMethod(app->Obj[LISTVIEW], MUIM_NList_UseImage,
					nple->nple_CreateTitleImage(nple->nple_UserData),
                                        nple->nple_ImageIndex, 0);
				DoMethod(nlistview, MUIM_NList_UseImage,
					nple->nple_CreateTitleImage(nple->nple_UserData),
                                        nple->nple_ImageIndex, 0);
				}
			}
		}

	return result;
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__)
// Replacement for SAS/C library functions

size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

#endif /* !__SASC && !__amigaos4__ */

//-----------------------------------------------------------------

static Object *GenerateAboutPage(struct SCAModule *app, CONST_STRPTR cVersion, CONST_STRPTR urlSubject)
{
///
	static char prefsVersion[60];
	static char aboutPageCopyRight[120] = "";

	snprintf(prefsVersion, sizeof(prefsVersion), GetLocString(MSGID_ABOUTPAGE_PREFSVERSION), VERSION_MAJOR, VERSION_MINOR);
	snprintf(aboutPageCopyRight, sizeof(aboutPageCopyRight), GetLocString(MSGID_ABOUTPAGE_COPYRIGHT), CURRENTYEAR);

	return CreatePrefsPage(app,
		VGroup,
//		MUIA_Background, MUII_PageBack,
		MUIA_Background, "5:THEME:SplashBackground",

		Child, HVSpace,

		Child, HGroup,
			Child, HVSpace,

			Child, VGroup,
				Child, DataTypesImageObject,
					MUIA_ScaDtpic_Name,  (ULONG) "THEME:ScalosSplashLogo",
					End, //DataTypesMCCObject

				Child, TextObject,
					MUIA_Font, MUIV_Font_Big,
					MUIA_Text_PreParse, MUIX_C,
					MUIA_Text_Contents, (ULONG) cVersion,
				End, //Text

				Child, HVSpace,

				Child, TextObject,
					MUIA_Font, MUIV_Font_Title,
					MUIA_Text_PreParse, MUIX_C,
					MUIA_Text_Contents, (ULONG) prefsVersion,
				End, //Text

				Child, TextObject,
					MUIA_Font, MUIV_Font_Title,
					MUIA_Text_PreParse, MUIX_C,
					MUIA_Text_Contents, COMPILER_STRING,
				End, //Text

			End, //VGroup

			Child, HVSpace,
		End, //HGroup


		Child, HVSpace,

		Child, VGroup,
			TextFrame,
			MUIA_InputMode, MUIV_InputMode_None,
			MUIA_Background, MUII_TextBack,

			Child, TextObject,
				MUIA_Text_SetMax, FALSE,
				MUIA_Text_Contents, (ULONG) aboutPageCopyRight,
			End, //Text

			Child, HGroup,
				MUIA_Group_HorizSpacing, 0,

				Child, HVSpace,

				Child, HGroup,
					MUIA_Group_HorizSpacing, 0,

					Child, TextObject,
						MUIA_Text_Contents, (ULONG) GetLocString(MSGID_ABOUTPAGE_CONTACT1),
					End, //Text

					Child, UrltextObject,
						MUIA_Urltext_Text, "Scalos@VFEmail.net",
						MUIA_Urltext_Url,  (ULONG) urlSubject,	// +jmc+
						MUIA_Urltext_SetMax, FALSE,
					End, //URLText

					Child, TextObject,
						MUIA_Text_Contents, (ULONG) GetLocString(MSGID_ABOUTPAGE_CONTACT2),
					End, //Text

					Child, UrltextObject,
						MUIA_Urltext_Text, "scalos.noname.fr",
						MUIA_Urltext_Url, "http://scalos.noname.fr",
						MUIA_Urltext_SetMax, FALSE,
					End, //URLText

					Child, TextObject,
						MUIA_Text_Contents, (ULONG) GetLocString(MSGID_ABOUTPAGE_CONTACT3),
					End, //Text
				End, //HGroup

				Child, HVSpace,
			End, //HGroup

		End, //VGroup

	End, //VGroup
	&NewPrefsPages[0]);
///
}

//-----------------------------------------------------------------

static Object *GeneratePathsPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
			MUIA_Background, MUII_PageBack,

			Child, HVSpace,

			Child, VGroup,
			MUIA_Background, MUII_GroupBack,
			GroupFrame,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_PATHSPAGE_DEFAULTPATHS),

			Child, HVSpace,

			Child, ColGroup(2),
				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_SCALOS_HOME)),
				Child, app->Obj[STRING_SCALOSHOME] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "SYS:",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_SCALOS_HOME_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_SCALOS_HOME_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_THEMES)),
				Child, app->Obj[STRING_THEMES] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "SCALOS:Themes/",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_THEMES_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_THEMES_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_DEFAULTICONS)),
				Child, app->Obj[STRING_DEFICONPATH] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "ENV:Sys/",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_DEFAULTICONS_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_DEFAULTICONS_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_WBSTARTUP)),
				Child, app->Obj[STRING_WBSTARTPATH] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "SYS:WBStartup/",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_WBSTARTUP_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_WBSTARTUP_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_IMAGECACHE)),
				Child, app->Obj[STRING_IMAGECACHE] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "t:",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_IMAGECACHE_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_IMAGECACHE_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_SQLITE3TEMPPATH)),
				Child, app->Obj[STRING_SQLITE3TEMPPATH] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "t:",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_SQLITE3TEMPPATH_ASLTITLE),
					ASLFR_DrawersOnly, TRUE,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_SQLITE3TEMPPATH_SHORTHELP),
				End, //Pop

				Child, HVSpace,
				Child, HVSpace,

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_THUMBNAILDB)),
				Child, app->Obj[STRING_THUMBNAILDB] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopFile),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "Scalos:Thumbnails.db",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_THUMBNAILDB_ASLTITLE),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_THUMBNAILDB_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_DISKCOPY)),
				Child, app->Obj[STRING_DISKCOPY] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopFile),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "SYS:System/DiskCopy",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_DISKCOPY_ASLTITLE),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_DISKCOPY_SHORTHELP),
				End, //Pop

				Child, Label2((ULONG) GetLocString(MSGID_PATHSPAGE_FORMAT)),
				Child, app->Obj[STRING_FORMAT] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopFile),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, "SYS:System/Format",
					End, //BetterString
					ASLFR_TitleText, (ULONG) GetLocString(MSGID_PATHSPAGE_FORMAT_ASLTITLE),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PATHSPAGE_FORMAT_SHORTHELP),
				End, //Pop
			End, //ColumnGroup

			Child, HVSpace,
		End, //VGroup
		Child, HVSpace,
	End, //VGroup
	&NewPrefsPages[1]);
///
}

//-----------------------------------------------------------------

static Object *GenerateStartupPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,
		Child, HVSpace,

		Child, VGroup,
			GroupFrame,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_STARTUPPAGE_SPLASHWINDOW),
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, HGroup,
				Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_SHOWSPLASHWINDOW)),
				Child, app->Obj[CHECK_SHOWSPLASH] = CheckMarkHelp(TRUE, MSGID_STARTUPPAGE_SHOWSPLASHWINDOW_SHORTHELP),
				Child, HVSpace,
			End, //HGroup

			Child, HGroup,
				Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_SPLASHCLOSEDELAY)),
				Child, app->Obj[SLIDER_SPLASHCLOSE] = SliderObject,
					MUIA_CycleChain, 1,
					MUIA_Numeric_Min, 1,
					MUIA_Numeric_Max, 10,
					MUIA_Slider_Horiz, TRUE,
					MUIA_Numeric_Value, 5,
				End, //Slider
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_STARTUPPAGE_SPLASHCLOSEDELAY_SHORTHELP),
				Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_DOWAITDELAY_SECONDS)),
			End, //HGroup

			Child, HVSpace,

		End, //VGroup

		Child, VGroup,
			GroupFrame,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_STARTUPPAGE_WBSTARTUP),
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, HGroup,
				Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_DOWAITDELAY)),
				Child, app->Obj[SLIDER_WBSDELAY] = SliderObject,
					MUIA_CycleChain, 1,
					MUIA_Weight, 500,
					MUIA_Numeric_Min, 1,
					MUIA_Numeric_Max, 10,
					MUIA_Slider_Horiz, TRUE,
					MUIA_Numeric_Value, 2,
				End, //Slider
				Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_DOWAITDELAY_SECONDS)),
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_STARTUPPAGE_DOWAITDELAY_SHORTHELP),
			End, //HGroup

			Child, HVSpace,
		End, //VGroup

		Child, HVSpace,

	End, //VGroup
	&NewPrefsPages[2]);
///
}

//-----------------------------------------------------------------

static Object *GenerateDesktopPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, RegisterObject,
			MUIA_Register_Titles, cDesktopPages,
			MUIA_CycleChain, TRUE,

			// --- Desktop-Screen
			Child, app->Obj[GROUP_SCRTITLE] = VGroup,
				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_SCREENTITLEBAR),
					MUIA_Background, MUII_GroupBack,

					Child, HGroup,
						Child, Label2((ULONG) GetLocString(MSGID_DESKTOPPAGE_SCREEN)),
						Child, app->Obj[POPPH_SCREENTITLE] = PopplaceholderObject,
							MUIA_Popph_Title, (ULONG) GetLocString(MSGID_DESKTOPPAGE_SCREEN_DESCRIPTION),
							MUIA_Popph_Array, cTitleph,
							MUIA_Popph_Avoid, MUIV_Popph_Avoid_Textinput,
							MUIA_Popph_ReplaceMode, FALSE,
							MUIA_Popph_Contents, "",
							MUIA_Popph_StringMaxLen, 1000,
						End, //Popplaceholder
					End, //HGroup


					Child, HGroup,
						Child, Label2((ULONG) GetLocString(MSGID_DESKTOPPAGE_TITLEBAR_REFRESH)),
						Child, app->Obj[SLIDER_TITLEREFRESH] = SliderObject,
							MUIA_CycleChain, 1,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 5,
						End, //Slider
						Child, Label((ULONG) GetLocString(MSGID_DESKTOPPAGE_TITLEBAR_REFRESH_SECONDS)),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_TITLEBAR_REFRESH_SHORTHELP),
					End, //HGroup

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_TITLEBAR_REFRESH_MEMCHANGE)),
						Child, app->Obj[CHECK_TITLEMEM] = CheckMarkHelp(FALSE, MSGID_DESKTOPPAGE_TITLEBAR_REFRESH_MEMCHANGE_SHORTHELP),
						Child, HVSpace,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_TITLEBAR_REFRESH_MEMCHANGE_SHORTHELP),
					End, //HGroup

					Child, HVSpace,

					Child, HGroup,
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_SCREENTITLE_MODE)),
						Child, app->Obj[CYCLE_SCREENTITLEMODE] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cScreenTitleModes,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_SCREENTITLE_MODE_SHORTHELP),
						End, //Cycle
					End, //HGroup
				End, //VGroup

				Child, HVSpace,
			End, //VGroup

			// Desktop -- Icons
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_DESKTOP_REFRESH),
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, Label2((ULONG) GetLocString(MSGID_DESKTOPPAGE_DISK_ICON_RATE)),
						Child, app->Obj[SLIDER_DISKREFRESH] = SliderObject,
							MUIA_CycleChain, 1,
							MUIA_Weight, 500,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 2,
						End, //Slider
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_DISK_ICON_RATE_SHORTHELP),
						Child, Label((ULONG) GetLocString(MSGID_DESKTOPPAGE_DISK_ICON_RATE_SECONDS)),
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_ICONS_HIDDENDEVICES),
					MUIA_Background, MUII_GroupBack,

					// Child, HVSpace,

					Child, app->Obj[NLISTVIEW_HIDDENDEVICES] = NListviewObject,
						MUIA_CycleChain, TRUE,
	//										  MUIA_Listview_Input, TRUE,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
						MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
						MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_FullAuto,
						MUIA_NListview_NList, app->Obj[NLIST_HIDDENDEVICES] = NListObject,
							MUIA_NList_DefaultObjectOnClick, TRUE,
							MUIA_NList_DragSortable, FALSE,
							MUIA_Background, MUII_ListBack,
							InputListFrame,
							MUIA_NList_DisplayHook2, (ULONG) &HiddenDevicesDisplayHook,
							MUIA_NList_ConstructHook2, (ULONG) &HiddenDevicesConstructHook,
							MUIA_NList_DestructHook2, (ULONG) &HiddenDevicesDestructHook,
							MUIA_NList_CompareHook2, &HiddenDevicesCompareHook,
							MUIA_NList_SortType, 0,
							MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
							MUIA_NList_Format, (ULONG) ",",
							MUIA_NList_ShowDropMarks, FALSE,
							MUIA_NList_AdjustHeight, TRUE,
							End, //NListObject
						End, //NListviewObject
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_HIDDENDEVICES_SHORTHELP),
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

			// Desktop -- Layout
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUTPREFERENCES),

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDISK)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBDISK] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDISK_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDRAWER)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBDRAWER] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDRAWER_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBTOOL)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBTOOL] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBTOOL_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBPROJECT)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBPROJECT] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBPROJECT_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBGARBAGE)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBGARBAGE] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBGARBAGE_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDEVICE)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBDEVICE] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBDEVICE_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBKICK)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBKICK] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBKICK_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBAPPICON)),
						Child, app->Obj[CYCLE_DESKTOP_LAYOUT_WBAPPICON] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LAYOUT_WBAPPICON_SHORTHELP),
						End, //Cycle
					End, //ColGroup

					Child, HVSpace,

				End, //VGroup
				Child, HVSpace,
			End, //VGroup

			// Desktop -- misc
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_CONSOLENAME),
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, app->Obj[STRING_CONSOLENAME] = BetterStringObject,
						MUIA_CycleChain, TRUE,
						StringFrame,
						MUIA_String_Contents, "",
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_CONSOLENAME_SHORTHELP),
					End, //BetterString

					Child, HVSpace,
				End, //VGroup

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_MISCELLANEOUS),
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_ALLOW_CLOSEWB)),
							Child, app->Obj[CLOSEWB] = CheckMarkHelp(TRUE, MSGID_DESKTOPPAGE_ALLOW_CLOSEWB_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_DROPSTART)),
							Child, app->Obj[DROPSTART] = CheckMarkHelp(TRUE, MSGID_DESKTOPPAGE_DROPSTART_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_AUTOLEAVEOUT)),
							Child, app->Obj[CHECK_AUTOLEAVEOUT] = CheckMarkHelp(TRUE, MSGID_DESKTOPPAGE_AUTOLEAVEOUT_SHORTHELP),
							End, //ColGroup

						Child, HVSpace,
						End, //HGroup

					Child, HVSpace,
					End, //VGroup

				Child, VGroup,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DESKTOPPAGE_LASSO),
					MUIA_Background, MUII_GroupBack,

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_SINGLEWINDOW_LASSO)),
						Child, HGroup,
							Child, app->Obj[SINGLE_WINDOW_LASSO_HOTKEY] = HotkeyStringObject,
								StringFrame,
								MUIA_String_Contents, "",
								MUIA_HotkeyString_Snoop, FALSE,
							End, //HotkeyString
							Child, app->Obj[SINGLE_WINDOW_LASSO_HOTKEY_SCAN] = ScanButton,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_SINGLEWINDOW_LASSO_SHORTHELP),
							End, //HGroup
						End, //ColGroup
					End, //VGroup

			Child, HVSpace,
			End, //VGroup
		End, //RegisterObject
	End, //VGroup
	&NewPrefsPages[3]);
///
}

//-----------------------------------------------------------------

static Object *GenerateIconsPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, RegisterObject,
			MUIA_Register_Titles, cIconPages,
			MUIA_CycleChain, TRUE,

			// --- Icons-Attributes
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_GENERAL),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_MASKED_CLICKAREA)),
							Child, app->Obj[CHECK_ICONMASK] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_MASKED_CLICKAREA_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_HILITE_UNDER_MOUSE)),
							Child, app->Obj[CHECK_HILITEUNDERMOUSE] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_HILITE_UNDER_MOUSE_SHORTHELP),
#if 0
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_MULTISELECTION)),
								Child, app->Obj[CHECK_MULTISELECT] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_MULTISELECTION_SHORTHELP),
#endif
						End, //ColGroup
						Child, HVSpace,
					End, //HGroup
					Child, HVSpace,
				End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_NEWICONS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,

					Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_NEWICONS_REMAP_PRECISION)),
						Child, app->Obj[SLIDER_ICONRMAP_PRECISION] = PrecisionSliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 4,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 1,
							End, //Slider
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_NEWICONS_REMAP_PRECISION_SHORTHELP),
					End, //HGroup

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_NEWICONS_TRANSPARENTBG)),
						Child, app->Obj[CHECK_NIMASK] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_NEWICONS_TRANSPARENTBG_SHORTHELP),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_NEWICONS_TRANSPARENTBG_SHORTHELP),
						Child, HVSpace,

					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_DEFICONS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_DEFICONS_LOADFIRST)),
						Child, app->Obj[CHECK_DEFFIRST] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_DEFICONS_LOADFIRST_SHORTHELP),
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_DEFICONS_SAVEABLE)),
						Child, app->Obj[CHECK_ICONSSAVE] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_DEFICONS_SAVEABLE_SHORTHELP),
						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,

					Child, HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_ATTRIBUTES_TRANSPARENCY_DEFICONS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

						Child, app->Obj[SLIDER_ICONTRANSPARENCY_DEFICONS] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 100,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 50,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ATTRIBUTES_TRANSPARENCY_DEFICONS_SHORTHELP),
						End, //Slider

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, HVSpace,

			End, //VGroup

			// --- Icons-Sizes
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONSCALING),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_NOMINALSIZE_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_NOMINALSIZE)),

						Child, app->Obj[SLIDER_ICONSPAGE_NOMINALSIZE] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, IDTA_ScalePercentage_MIN,
							MUIA_Numeric_Max, IDTA_ScalePercentage_MAX,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 100,
						End, //Slider

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_PERCENT)),
					End, //HGroup

					Child, HVSpace,

					Child, ColGroup(5),
						Child, HVSpace,

						Child, HGroup,
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_MINSIZE)),
							Child, app->Obj[CYCLE_ICONMINSIZE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconSizesMin,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_MINSIZE_SHORTHELP),
								End, //Cycle
							End, //HGroup

						Child, HVSpace,

						Child, HGroup,
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_MAXSIZE)),
							Child, app->Obj[CYCLE_ICONMAXSIZE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconSizesMax,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SCALING_MAXSIZE_SHORTHELP),
								End, //Cycle
							End, //HGroup

						Child, HVSpace,

						Child, HVSpace,

						Child, app->Obj[GOUP_ICONSIZE_MIN_SAMPLE] = PageGroup,
							MUIA_Frame, MUIV_Frame_Gauge,
							Child, HVSpace, // empty
							Child,  app->Obj[IMAGE_ICON_SAMPLE_16] = CreatePrefsImage((void *) IMAGE_ICON16),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_24] = CreatePrefsImage((void *) IMAGE_ICON24),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_32] = CreatePrefsImage((void *) IMAGE_ICON32),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_48] = CreatePrefsImage((void *) IMAGE_ICON48),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_64] = CreatePrefsImage((void *) IMAGE_ICON64),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_96] = CreatePrefsImage((void *) IMAGE_ICON96),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_128] = CreatePrefsImage((void *) IMAGE_ICON128),
							End, //PageGroup

						Child, HVSpace,

						Child, app->Obj[GOUP_ICONSIZE_MAX_SAMPLE] = PageGroup,
							MUIA_Frame, MUIV_Frame_Gauge,
							Child, HVSpace, // empty
							Child,  app->Obj[IMAGE_ICON_SAMPLE_16] = CreatePrefsImage((void *) IMAGE_ICON16),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_24] = CreatePrefsImage((void *) IMAGE_ICON24),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_32] = CreatePrefsImage((void *) IMAGE_ICON32),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_48] = CreatePrefsImage((void *) IMAGE_ICON48),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_64] = CreatePrefsImage((void *) IMAGE_ICON64),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_96] = CreatePrefsImage((void *) IMAGE_ICON96),
							Child,  app->Obj[IMAGE_ICON_SAMPLE_128] = CreatePrefsImage((void *) IMAGE_ICON128),
							End, //PageGroup

						Child, HVSpace,
					End, //ColGroup

					Child, HVSpace,
				End, //VGroup
				Child, HVSpace,

			End, //VGroup

			// --- Icons-Labels
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_TEXT)),
						Child, app->Obj[CYCLE_LABELSTYLE] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cIconLabelStyles,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_TEXT_SHORTHELP),
						End, //Cycle
					End, //HGroup

					Child, HGroup,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_TEXTSPACE)),
						Child, app->Obj[SLIDER_LABELSPACE] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 0,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_TEXTSPACE_SHORTHELP),
						End, //Slider
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_TEXTSPACE_PIXELS)),
					End, //HGroup

					Child, HGroup,
						Child, HVSpace,
						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_SPLIT)),
							Child, app->Obj[CHECK_MULTILINES] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_LABELS_SPLIT_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_UNDERLINE_LINKS)),
							Child, app->Obj[SOFTICONSLINK] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_LABELS_UNDERLINE_LINKS_SHORTHELP),

						End, //ColGroup
						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,
				End, //VGroup

			Child, HVSpace,

			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_RECT_GROUP),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_RECT)),
						Child, app->Obj[CHECK_SEL_ICONTEXT_RECTANGLE] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_SEL_ICONTEXT_RECT_SHORTHELP),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_RECT_SHORTHELP),
						Child, HVSpace,

					End, //HGroup

					Child, HVSpace,

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_XBORDER)),
						Child, app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERX] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 4,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_XBORDER_SHORTHELP),
						End, //Cycle

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_YBORDER)),
						Child, app->Obj[SLIDER_SEL_ICONTEXT_RECT_BORDERY] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 2,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_YBORDER_SHORTHELP),
						End, //Cycle

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_RADIUS)),
						Child, app->Obj[SLIDER_SEL_ICONTEXT_RECT_RADIUS] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 20,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 5,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SEL_ICONTEXT_RADIUS_SHORTHELP),
						End, //Cycle
					End, //ColGroup

				Child, HVSpace,
				End, //VGroup

			End, //VGroup

			Child, HVSpace,

			Child, VGroup,
				MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_FONT),
				GroupFrame,
				MUIA_Background, MUII_GroupBack,

				Child, HVSpace,

				Child, HGroup,
					Child, HVSpace,
					Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_ENABLE)),
					Child, app->Obj[CHECK_ICONSPAGE_TTICONFONT_ENABLE] = CheckMarkHelp(FALSE, MSGID_FONTSPAGE_TTFICONFONT_ENABLE_SHORTHELP),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FONTSPAGE_TTFICONFONT_ENABLE_SHORTHELP),
					End, //HGroup

				Child, HVSpace,

				Child, app->Obj[GROUP_ICONSPAGE_ICONFONT] = VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_GROUP_ICONFONT),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,
					Child, app->Obj[GROUP_ICONFONT_NOTICE] = HGroup,
						Child, FloattextObject,
							TextFrame,
							MUIA_Floattext_Justify, FALSE,
							MUIA_Floattext_Text, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_FONT_NOTICE),
							End, //Floattext
						Child, app->Obj[POP_FONTPREFS] = ImageObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							MUIA_Image_Spec, "6:18", //MUII_PopUp,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_FONT_SHORTHELP),
							End, //Image
						End, //HGroup

					Child, app->Obj[GROUP_ICONFONT_SELECT] = HGroup,
						Child, app->Obj[POP_ICONFONT] = PopaslObject,
							MUIA_CycleChain, TRUE,
							MUIA_Popstring_Button, PopButton(MUII_PopUp),
							MUIA_Popstring_String, BetterStringObject,
								StringFrame,
								MUIA_String_Contents, "Xen/8",
								End, //BetterString
							ASLFR_TitleText, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONFONT_ASLTITLE),
							MUIA_Popasl_Type , ASL_FontRequest,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_ICONFONT_SHORTHELP),
							End, //Pop
						End, //HGroup

					Child, app->Obj[MCC_ICONFONT_SAMPLE] = FontSampleObject,
						TextFrame,
						MUIA_Background, MUII_TextBack,
						MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LABELS_FONT_SAMPLETEXT_SHORTHELP),
						End, //FontSampleMCCObject
					End, //VGroup

				Child, app->Obj[GROUP_ICONSPAGE_TTICONFONT] = HGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,
					MUIA_Disabled, TRUE,

					Child, VGroup,
						Child, app->Obj[POPSTRING_ICONSPAGE_TTICONFONT] = PopstringObject,
							MUIA_CycleChain, TRUE,
							MUIA_Popstring_Button, PopButton(MUII_PopUp),
							MUIA_Popstring_String, BetterStringObject,
								StringFrame,
								MUIA_String_Contents, " ",
								End, //BetterString
							MUIA_Popstring_OpenHook, &IconTtfPopOpenHook,
							MUIA_Popstring_CloseHook, &IconTtfPopCloseHook,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_SHORTHELP),
							End, //PopstringObject

						Child, app->Obj[MCC_ICONSPAGE_TTICONFONT_SAMPLE] = FontSampleObject,
							TextFrame,
							MUIA_Background, MUII_TextBack,
							MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_SAMPLETEXT_SHORTHELP),
							End, //FontSampleMCCObject
						End, //VGroup
					End, //HGroup
				End, //VGroup


			Child, HVSpace,

			End, //VGroup

			// --- Icons-Borders
			Child, VGroup,
				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, ColGroup(2),
						Child, app->Obj[FRAME_ICONNORMAL] = McpFrameObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_InnerBottom, 4,
							MUIA_InnerLeft, 4,
							MUIA_InnerRight, 4,
							MUIA_InnerTop, 4,
							MUIA_CycleChain, 1,
							MUIA_Background, MUII_ButtonBack,
							MUIA_CycleChain, 1,
							MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON,
							MUIA_Draggable, FALSE,
							MUIA_Dropable, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_NORMAL_SHORTHELP),
						End,
						Child, app->Obj[FRAME_ICONSELECTED] = McpFrameObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_InnerBottom, 4,
							MUIA_InnerLeft, 4,
							MUIA_InnerRight, 4,
							MUIA_InnerTop, 4,
							MUIA_CycleChain, 1,
							MUIA_Background, MUII_ButtonBack,
							MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON | MCP_FRAME_RECESSED,
							MUIA_Draggable, FALSE,
							MUIA_Dropable, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_SELECTED_SHORTHELP),
						End,

						Child, CLabel((ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_NORMAL)),
						Child, CLabel((ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_SELECTED)),
					End, //ColGroup
					Child, ColGroup(4),
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_LEFT)),
						Child, app->Obj[ICONLEFT] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 4,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_LEFT_SHORTHELP),
							End, // Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_TOP)),
						Child, app->Obj[ICONTOP] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 3,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_TOP_SHORTHELP),
							End, //Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_RIGHT)),
						Child, app->Obj[ICONRIGHT] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 4,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_RIGHT_SHORTHELP),
							End, //Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_BOTTOM)),
						Child, app->Obj[ICONBOTTOM] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 3,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_BOTTOM_SHORTHELP),
							End, //Slider
						End, //ColGroup
					End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_ICONFRAME),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, ColGroup(2),
						Child, app->Obj[FRAME_ICON_THUMBNAIL_NORMAL] = McpFrameObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_InnerBottom, 4,
							MUIA_InnerLeft, 4,
							MUIA_InnerRight, 4,
							MUIA_InnerTop, 4,
							MUIA_CycleChain, 1,
							MUIA_Background, MUII_ButtonBack,
							MUIA_CycleChain, 1,
							MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON,
							MUIA_Draggable, FALSE,
							MUIA_Dropable, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_ICONFRAME_NORMAL_SHORTHELP),
							End, //McpFrameClass

						Child, app->Obj[FRAME_ICON_THUMBNAIL_SELECTED] = McpFrameObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_InnerBottom, 4,
							MUIA_InnerLeft, 4,
							MUIA_InnerRight, 4,
							MUIA_InnerTop, 4,
							MUIA_CycleChain, 1,
							MUIA_Background, MUII_ButtonBack,
							MUIA_CycleChain, 1,
							MUIA_MCPFrame_FrameType, MCP_FRAME_BUTTON | MCP_FRAME_RECESSED,
							MUIA_Draggable, FALSE,
							MUIA_Dropable, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_ICONFRAME_SELECTED_SHORTHELP),
							End, //McpFrameClass

						Child, CLabel((ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_NORMAL)),
						Child, CLabel((ULONG) GetLocString(MSGID_ICONSPAGE_ICONFRAME_SELECTED)),

						End, //ColGroup
					Child, ColGroup(4),
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_LEFT)),
						Child, app->Obj[THUMBNAILS_LEFTBORDER] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 4,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_LEFT_SHORTHELP),
							End, // Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_TOP)),
						Child, app->Obj[THUMBNAILS_TOPBORDER] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 3,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_TOP_SHORTHELP),
							End, //Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_RIGHT)),
						Child, app->Obj[THUMBNAILS_RIGHTBORDER] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 4,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_RIGHT_SHORTHELP),
							End, //Slider
						Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_BOTTOM)),
						Child, app->Obj[THUMBNAILS_BOTTOMBORDER] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 10,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 3,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_ICONBORDER_BOTTOM_SHORTHELP),
							End, //Slider
						End, //ColGroup
					End, //VGroup

			End, //VGroup


			// --- Icons--Tooltips
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_GENERAL),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_SHOW)),
						Child, app->Obj[CHECK_TOOLTIPS] = CheckMarkHelp(TRUE, MSGID_ICONSPAGE_TOOLTIPS_SHOW_SHORTHELP),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_SHOW_SHORTHELP),
						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, app->Obj[TOOLTIP_SETTINGSGROUP] = VGroup,

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_SETTINGS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, HVSpace,

						Child, HGroup,
							Child, Label2((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_DELAY)),
							Child, app->Obj[SLIDER_TIPDELAY] = SliderObject,
									MUIA_CycleChain, TRUE,
									MUIA_Numeric_Min, 1,
									MUIA_Numeric_Max, 10,
									MUIA_Slider_Horiz, TRUE,
									MUIA_Numeric_Value, 5,
							End, //Slider
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_DELAY_SHORTHELP),
							Child, Label((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_DELAY_SECONDS)),
						End, //HGroup

						Child, HGroup,
/* currently unused */					MUIA_ShowMe, FALSE,
							Child, Label2((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_FONT)),
							Child, app->Obj[STRING_TOOLTIPFONT] = PopaslObject,
								MUIA_CycleChain, TRUE,
								MUIA_Popasl_Type, ASL_FontRequest,
								MUIA_Popstring_Button, PopButton(MUII_PopUp),
								MUIA_Popstring_String, BetterStringObject,
									StringFrame,
									MUIA_String_Contents, "Xen/8",
								End, //BetterString

								ASLFO_TitleText, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_FONT_ASLTITLE),
								ASLFO_FixedWidthOnly, FALSE,
							End, //Pop
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_FONT_SHORTHELP),
						End, //HGroup

						Child, HVSpace,

						Child, HGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_TRANSPARENCY),
							GroupFrame,
							MUIA_Background, MUII_GroupBack,

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

							Child, app->Obj[SLIDER_ICONTOOLTIPS_TRANSPARENCY] = SliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 100,
								MUIA_Slider_Horiz, TRUE,
								MUIA_Numeric_Value, 50,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_TRANSPARENCY_SHORTHELP),
							End, //Slider

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
						End, //HGroup
						Child, HVSpace,

					End, //VGroup

					Child, HGroup,
/* currently unused */				MUIA_ShowMe, FALSE,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_DISPLAYFIELDS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, VGroup,
							Child, CLabel2((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_AVAILABLEFIELDS)),
							Child, NListviewObject,
/* currently unused */						MUIA_Disabled, TRUE,
								MUIA_NListview_NList, app->Obj[STORAGE_TIPS] = NListObject,
									InputListFrame,
									MUIA_Background, MUII_ListBack,
									//MUIA_List_Format, "",
									MUIA_NList_ShowDropMarks, TRUE,
									MUIA_NList_AdjustWidth, FALSE,
									MUIA_NList_AutoVisible, TRUE,
									MUIA_NList_DragSortable, FALSE,
								End, //List
								MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
								MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_AVAILABLEFIELDS_SHORTHELP),
							End, //Listview
						End, //VGroup

						Child, VGroup,
							Child, CLabel2((ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_FIELDSINUSE)),
							Child, NListviewObject,
/* currently unused */						MUIA_Disabled, TRUE,
								MUIA_NListview_NList, app->Obj[USED_TIPS] = NListObject,
									InputListFrame,
									MUIA_Background, MUII_ListBack,
									//MUIA_List_Format, "",
									MUIA_NList_ShowDropMarks, TRUE,
									MUIA_NList_AdjustWidth, FALSE,
									MUIA_NList_AutoVisible, TRUE,
									MUIA_NList_DragSortable, TRUE,
								End, //List
								MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
								MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_TOOLTIPS_FIELDSINUSE_SHORTHELP),
							End, //Listview
						End, //VGroup
					End, //HGroup

				End, //VGroup

				Child, HVSpace,

				End, //VGroup

			// --- Icons-Thumbnails
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_SETTINGS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SHOWTHUMBNAILS)),
							Child, app->Obj[CYCLE_SHOWTHUMBNAILS] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cShowThumbnails,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_SHOWTHUMBNAILS_SHORTHELP),
							End, //Cycle

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_SIZE)),
							Child, app->Obj[CYCLE_THUMBNAILSIZE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cThumbnailSizes,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_SIZE_SHORTHELP),
							End, //Cycle

							Child, HVSpace,	// always 2 HVSpace due to ColGroup(2)
							Child, HVSpace,

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_SHOWTHUMBNAILS_AS_DEFAULT)),
							Child, app->Obj[CHECK_SHOWTHUMBNAILS_AS_DEFAULT] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_SHOWTHUMBNAILS_AS_DEFAULT_SHORTHELP),

							Child, HVSpace,	// always 2 HVSpace due to ColGroup(2)
							Child, HVSpace,

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_SQUARE)),
							Child, app->Obj[CHECK_THUMBNAILS_SQUARE] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_THUMBNAILS_SQUARE_SHORTHELP),

							Child, HVSpace,	// always 2 HVSpace due to ColGroup(2)
							Child, HVSpace,

							Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_BACKFILL)),
							Child, app->Obj[CHECK_THUMBNAILS_BACKFILL] = CheckMarkHelp(FALSE, MSGID_ICONSPAGE_THUMBNAILS_BACKFILL_SHORTHELP),

							End, // ColGroup

						Child, HVSpace,

						Child, VGroup,
							Child, app->Obj[GROUP_THUMBNAILS_ICON_SAMPLE] = PageGroup,
								MUIA_Frame, MUIV_Frame_Gauge,
								Child,  app->Obj[IMAGE_ICON_SAMPLE_16] = CreatePrefsImage((void *) IMAGE_ICON16),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_24] = CreatePrefsImage((void *) IMAGE_ICON24),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_32] = CreatePrefsImage((void *) IMAGE_ICON32),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_48] = CreatePrefsImage((void *) IMAGE_ICON48),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_64] = CreatePrefsImage((void *) IMAGE_ICON64),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_96] = CreatePrefsImage((void *) IMAGE_ICON96),
								Child,  app->Obj[IMAGE_ICON_SAMPLE_128] = CreatePrefsImage((void *) IMAGE_ICON128),
								End, //PageGroup
							End, //VGroup

						Child, HVSpace,

						End, //HGroup

					Child, HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_QUALITY),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_QUALITY_WORST)),

						Child, app->Obj[SLIDER_THUMBNAILS_QUALITY] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, SCALOSPREVIEWA_Quality_Min,
							MUIA_Numeric_Max, SCALOSPREVIEWA_Quality_Max,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, SCALOSPREVIEWA_Quality_Max,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_QUALITY_SHORTHELP),
						End, //Slider

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_QUALITY_BEST)),
					End, //HGroup

					Child, HVSpace,
					End, //HGroup

				Child, app->Obj[GROUP_THUMBNAIL_BACKFILL_TRANSPARENCY] = HGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAIL_BACKFILL_TRANSPARENCY),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

					Child, app->Obj[SLIDER_ICONTRANSPARENCY_THUMBNAILBACK] = SliderObject,
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 0,
						MUIA_Numeric_Max, 100,
						MUIA_Slider_Horiz, TRUE,
						MUIA_Numeric_Value, 50,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAIL_BACKFILL_TRANSPARENCY_SHORTHELP),
						End, //Slider

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
					End, //HGroup

				Child, HGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_CACHE),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE)),
						Child, app->Obj[SLIDER_THUMBNAILS_MAXAGE] = ThumbnailLifetimeSliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 365,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 14,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MAXAGE_SHORTHELP),
						End, //ThumbnailLifetimeSliderClass

						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MINSIZE_LIMIT)),
						Child, app->Obj[SLIDER_THUMBNAILS_MINSIZE_LIMIT] = NumericbuttonObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 8,
							MUIA_Numeric_Max, 256,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 128,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_THUMBNAILS_MINSIZE_LIMIT_SHORTHELP),
						End, //Slider

						End, //ColGroup

					Child, HVSpace,
					End, // HGroup

				Child, HVSpace,

				End, //VGroup


		End, //Register

	End, //VGroup
	&NewPrefsPages[4]);
///
}

//-----------------------------------------------------------------

static Object *GenerateDragNDropPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, RegisterObject,
			MUIA_Register_Titles, cIconDragDropPages,
			MUIA_CycleChain, TRUE,

			// Drag and drop
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_BOBS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_ROUTINES)),
							Child, app->Obj[CYCLE_ROUTINE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconDragRoutines,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_ROUTINES_SHORTHELP),
							End, //Cycle

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY)),
							Child, app->Obj[CYCLE_TRANSPMODE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconDragTransparents,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_SHORTHELP),
							End, //Cycle

						End, //ColGroup

						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,
				End, //VGroup

				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(2),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_STYLE)),
							Child, app->Obj[CYCLE_STYLE] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconDragStyle,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_STYLE_SHORTHELP),
							End, //Cycle

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_DROPMARKMODE)),
							Child, app->Obj[CYCLE_DROPMARK] = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_Cycle_Entries, cIconDropMark,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_DROPMARKMODE_SHORTHELP),
							End, //Cycle

						End, //ColGroup

						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,
				End, //VGroup

				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, ColGroup(4),
						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_AUTOREMOVE)),
						Child, app->Obj[CHECK_AUTOREMOVE] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_AUTOREMOVE_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_EASY_MULTISELECT)),
						Child, app->Obj[CHECK_EASY_MULTISELECT] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_EASY_MULTISELECT_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_GROUP_MULTIPLE)),
						Child, app->Obj[CHECK_GROUPDRAG] = CheckMarkHelp(FALSE,MSGID_DRAGNDROPPAGE_GROUP_MULTIPLE_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_EASY_MULTIDRAG)),
						Child, app->Obj[CHECK_EASY_MULTIDRAG] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_EASY_MULTIDRAG_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_DRAGGINGLABEL)),
						Child, app->Obj[CHECK_SHOWDRAG] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_DRAGGINGLABEL_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_DROPMENU)),
						Child, app->Obj[CHECK_ENABLE_DROPMENU] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_DROPMENU_SHORTHELP),

						End, //ColGroup

						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,
				End, //VGroup

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_QUALIFIERS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCECOPY)),
						Child, HGroup,
							Child, app->Obj[COPY_HOTKEY] = HotkeyStringObject,
								StringFrame,
								MUIA_String_Contents, "",
								MUIA_HotkeyString_Snoop, FALSE,
							End, //HotkeyString
							Child, app->Obj[COPY_HOTKEY_SCAN] = ScanButton,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCECOPY_SHORTHELP),
						End, //HGroup

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCEMAKELINK)),
						Child, HGroup,
							Child, app->Obj[MAKELINK_HOTKEY] = HotkeyStringObject,
								StringFrame,
								MUIA_String_Contents, "",
								MUIA_HotkeyString_Snoop, FALSE,
							End, //HotkeyString
							Child, app->Obj[MAKELINK_HOTKEY_SCAN] = ScanButton,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCEMAKELINK_SHORTHELP),
						End, //HGroup

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCEMOVE)),
						Child, HGroup,
							Child, app->Obj[MOVE_HOTKEY] = HotkeyStringObject,
								StringFrame,
								MUIA_String_Contents, "",
								MUIA_HotkeyString_Snoop, FALSE,
							End, //HotkeyString
							Child, app->Obj[MOVE_HOTKEY_SCAN] = ScanButton,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_FORCEMOVE_SHORTHELP),
						End, //HGroup
					End, //ColGroup

					Child, HVSpace,
				End, //VGroup

				Child, HVSpace,
			End, //VGroup

			// Transparency
			Child, VGroup,
				Child, HVSpace,

				Child, HGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_ICONDRAG),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

					Child, app->Obj[SLIDER_ICONTRANSPARENCY_DRAG] = SliderObject,
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 0,
						MUIA_Numeric_Max, 100,
						MUIA_Slider_Horiz, TRUE,
						MUIA_Numeric_Value, 50,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_ICONDRAG_SHORTHELP),
					End, //Slider

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
				End, //HGroup

				Child, HGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_ICONSHADOW),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

					Child, app->Obj[SLIDER_ICONTRANSPARENCY_SHADOW] = SliderObject,
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 0,
						MUIA_Numeric_Max, 100,
						MUIA_Slider_Horiz, TRUE,
						MUIA_Numeric_Value, 50,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_ICONSHADOW_SHORTHELP),
					End, //Slider

					Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
				End, //HGroup

				Child, HVSpace,
			End, //VGroup

			// Triggers
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					//MUIA_FrameTitle, "",
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_LOOK)),
						Child, app->Obj[RADIO_BOBLOOK] = RadioObject,
							MUIA_CycleChain, TRUE,
							MUIA_Radio_Entries, cIconDragLook,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_LOOK_SHORTHELP),
						End, //Radio

						Child, HVSpace,
					End, //HGroup

					Child, HVSpace,
				End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),

						Child, HVSpace,

						Child, ColGroup(4),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_DISKICONS)),
							Child, app->Obj[CHECK_TRIG_DISK] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_DRAWERICONS)),
							Child, app->Obj[CHECK_TRIG_DRAWER] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_TOOLICONS)),
							Child, app->Obj[CHECK_TRIG_TOOL] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_PROJECTICONS)),
							Child, app->Obj[CHECK_TRIG_PROJECT] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_TRASHCANICONS)),
							Child, app->Obj[CHECK_TRIG_TRASHCAN] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_KICKICONS)),
							Child, app->Obj[CHECK_TRIG_KICK] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_APPICONS)),
							Child, app->Obj[CHECK_TRIG_APPICON] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_APPWINDOWS)),
							Child, app->Obj[CHECK_TRIG_APPWIN] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_ICONIFIED_WINDOWS)),
							Child, app->Obj[CHECK_TRIG_ICONIFIEDWIN] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRIGGERS_SCALOS_WINDOWS)),
							Child, app->Obj[CHECK_TRIG_SCALOSWIN] = CheckMarkHelp(FALSE, MSGID_DRAGNDROPPAGE_TRIGGERS_SHORTHELP),
						End, //ColGroup

						Child, HVSpace,
					End, //VGroup

				Child, HVSpace,

				End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_POPOPENWINDOWS),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, HGroup,
						Child, Label((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_SLIDER_POPOPWINDOW_DELAY)),
						Child, app->Obj[SLIDER_POPOPENWINDOW_DELAY] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 20,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 3,
						End, //Slider
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DRAGNDROPPAGE_SLIDER_POPOPWINDOW_DELAY_SHORTHELP),
						Child, Label((ULONG) GetLocString(MSGID_STARTUPPAGE_DOWAITDELAY_SECONDS)),
					End, //HGroup

					Child, HVSpace,

					End, //VGroup

				Child, HVSpace,

			End, //VGroup,
		End, //Register

	End, //VGroup
	&NewPrefsPages[5]);
///
}

//-----------------------------------------------------------------

static Object *GenerateWindowPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, RegisterObject,
			MUIA_Register_Titles, cWindowPages,
			MUIA_CycleChain, TRUE,

			// --- Windows-General
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),
						Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_ROOTWINDOW)),
						Child, app->Obj[POPPH_ROOTWINDOWTITLE] = PopplaceholderObject,
							MUIA_Popph_Title, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_ROOTWINDOW_DESCRIPTION),
							MUIA_Popph_Array, cTitleph,
							MUIA_Popph_Avoid, MUIV_Popph_Avoid_Textinput,
							MUIA_Popph_ReplaceMode, FALSE,
							MUIA_Popph_Contents, "",
							MUIA_Popph_StringMaxLen, 600,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_ROOTWINDOW_SHORTHELP),
						End, //Popplaceholder

						Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_DIRECTORYWINDOW)),
						Child, app->Obj[POPPH_DIRWINDOWTITLE] = PopplaceholderObject,
							MUIA_Popph_Title, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_DIRECTORYWINDOW_DESCRIPTION),
							MUIA_Popph_Array, cTitleph,
							MUIA_Popph_Avoid, MUIV_Popph_Avoid_Textinput,
							MUIA_Popph_ReplaceMode, FALSE,
							MUIA_Popph_Contents, "",
							MUIA_Popph_StringMaxLen, 600,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTITLES_DIRECTORYWINDOW_SHORTHELP),
						End, //Popplaceholder

						Child, Label2((ULONG) GetLocString(MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH)),
						Child, HGroup,
							Child, app->Obj[SLIDER_WINTITLEREFRESH] = SliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 1,
								MUIA_Numeric_Max, 10,
								MUIA_Slider_Horiz, TRUE,
								MUIA_Numeric_Value, 5,
							End, //Slider
							Child, Label((ULONG) GetLocString(MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH_SECONDS)),
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH_SHORTHELP),
						End, //HGroup
					End, //ColGroup

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH_MEMCHANGE)),
						Child, app->Obj[CHECK_WINTITLEMEM] = CheckMarkHelp(FALSE, MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH_MEMCHANGE_SHORTHELP),
						Child, HVSpace,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_DESKTOPPAGE_WINTITLEBAR_REFRESH_MEMCHANGE_SHORTHELP),
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, VGroup,
					//MUIA_FrameTitle, "",
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(4),
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTYPE)),
						Child, app->Obj[CYCLE_WINDOWTYPE] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cWindowRefresh,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTYPE_SHORTHELP),
						End, //Cycle
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_SHOWALL_DEFAULT)),
						Child, app->Obj[CYCLE_SHOWALLDEFAULT] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cShowAllDefault,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_SHOWALL_DEFAULT_SHORTHELP),
						End, //Cycle
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_VIEWBY_DEFAULT)),
						Child, app->Obj[CYCLE_VIEWBYICONSDEFAULT] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cViewByDefault,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_WINDOWTYPE_VIEWBY_DEFAULT_SHORTHELP),
						End, //Cycle
						Child, HVSpace,
					End, //ColGroup

					Child, HVSpace,

				End, //VGroup

				Child, VGroup,
					//MUIA_FrameTitle, "",
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(4),
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTEXTMENU_DRAGBAR)),
						Child, app->Obj[CHECK_POPTITLEONLY] = CheckMarkHelp(FALSE, MSGID_WINDOWPAGE_CONTEXTMENU_DRAGBAR_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_MMB_MOVE)),
						Child, app->Obj[CHECK_MMBMOVE] = CheckMarkHelp(FALSE, MSGID_WINDOWPAGE_MMB_MOVE_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_SHOW_STATUSBAR)),
						Child, app->Obj[CHECK_STATUSBAR] = CheckMarkHelp(FALSE, MSGID_WINDOWPAGE_SHOW_STATUSBAR_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_CHECK_OVERLAP)),
						Child, app->Obj[CHECK_CHECKOVERLAP] = CheckMarkHelp(FALSE, MSGID_WINDOWPAGE_CHECK_OVERLAP_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUP_ONRESIZE)),
						Child, app->Obj[CHECK_CLEANUP_ONRESIZE] = CheckMarkHelp(FALSE, MSGID_WINDOWPAGE_CLEANUP_ONRESIZE_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_HIDE_HIDDENFILES)),
						Child, app->Obj[HIDEHIDDENFILES] = CheckMarkHelp(FALSE, MSGID_FILEDISPLAYPAGE_HIDE_HIDDENFILES_SHORTHELP),
						Child, HVSpace,

						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_HIDE_PROTECTHIDDENFILES)),
						Child, app->Obj[HIDEPROTECTHIDDENFILES] = CheckMarkHelp(FALSE, MSGID_FILEDISPLAYPAGE_HIDE_PROTECTHIDDENFILES_SHORTHELP),
						Child, HVSpace,

					End, //ColGroup

					Child, HVSpace,

				End, //VGroup

				Child, HVSpace,

			End, //VGroup

			// --- Window-Sizes
			Child, VGroup,

				Child, HVSpace,

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(4),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE_SHORTHELP),

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE_LEFT)),
						Child, app->Obj[WINLEFT] = BetterStringObject,
							StringFrame,
							MUIA_CycleChain, TRUE,
							MUIA_String_Integer, 220,
							MUIA_String_Accept, "0123456789",
							MUIA_String_AdvanceOnCR, TRUE,
							End, //BetterString

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE_TOP)),
						Child, app->Obj[WINTOP] = BetterStringObject,
							StringFrame,
							MUIA_CycleChain, TRUE,
							MUIA_String_Integer, 100,
							MUIA_String_Accept, "0123456789",
							MUIA_String_AdvanceOnCR, TRUE,
							End, //BetterString

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE_WIDTH)),
						Child, app->Obj[WINWIDTH] = BetterStringObject,
							StringFrame,
							MUIA_CycleChain, TRUE,
							MUIA_String_Integer, 200,
							MUIA_String_Accept, "0123456789",
							MUIA_String_AdvanceOnCR, TRUE,
							End, //BetterString

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_DEFAULT_WINDOWSIZE_HEIGHT)),
						Child, app->Obj[WINHEIGHT] = BetterStringObject,
							StringFrame,
							MUIA_CycleChain, TRUE,
							MUIA_String_Integer, 300,
							MUIA_String_Accept, "0123456789",
							MUIA_String_AdvanceOnCR, TRUE,
							End, //BetterString
					End, //ColGroup

					Child, HVSpace,
				End, //VGroup

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE_SHORTHELP),

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE_LEFT)),
						Child, app->Obj[SLIDER_CLEANLEFT] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Numeric_Value, 5,
							MUIA_Slider_Horiz, TRUE,
							End, //SliderObject

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE_TOP)),
						Child, app->Obj[SLIDER_CLEANTOP] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Numeric_Value, 5,
							MUIA_Slider_Horiz, TRUE,
							End, //SliderObject

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE_XSKIP)),
						Child, app->Obj[SLIDER_CLEANXSKIP] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Numeric_Value, 5,
							MUIA_Slider_Horiz, TRUE,
							End, //SliderObject

						Child, Label((ULONG) GetLocString(MSGID_WINDOWPAGE_CLEANUPSPACE_YSKIP)),
						Child, app->Obj[SLIDER_CLEANYSKIP] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 20,
							MUIA_Numeric_Value, 5,
							MUIA_Slider_Horiz, TRUE,
							End, //SliderObject
					End, //ColGroup

					Child, HVSpace,

				End, //VGroup

				Child, HVSpace,

			End, //VGroup

			// --- Window-Control Bar Browser
			Child, RegisterObject,
				MUIA_Register_Titles, cWindowControlBar,
				MUIA_CycleChain, TRUE,

				// --- Browser Window Control Bar
				Child, VGroup,

					Child, VSpace(1),

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_BROWSER_GADGETS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,
						MUIA_Weight, 500,

						Child, HGroup,
							Child, VGroup,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_AVAILABLE_GADGETS_SHORTHELP),
								Child, CLabel2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_AVAILABLE_GADGETS)),
								Child, app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_AVAILABLE] = NListviewObject,
									MUIA_Weight, 500,
									MUIA_Listview_Input, TRUE,
									MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
									MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
									MUIA_NListview_NList, app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_AVAILABLE] = ControlBarGadgetsListObject,
										InputListFrame,
										MUIA_Background, MUII_ListBack,
										MUIA_NList_Format, "W=-1, W=-1 MICW=3, W=-1",
										MUIA_NList_ShowDropMarks, FALSE,
										MUIA_NList_AutoVisible, TRUE,
										MUIA_NList_DisplayHook2, &ControlBarGadgetListDisplayHook,
										MUIA_NList_ConstructHook2, &ControlBarGadgetListConstructHook,
										MUIA_NList_DestructHook2, &ControlBarGadgetListDestructHook,
									End,
								End, //Listview
							End, //VGroup

							Child, VGroup,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTIVE_GADGETS_SHORTHELP),
								Child, CLabel2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTIVE_GADGETS)),
								Child, app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTIVE] = NListviewObject,
									MUIA_Weight, 500,
									MUIA_Listview_Input, TRUE,
									MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
									MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
									MUIA_NListview_NList, app->Obj[NLIST_CONTROLBARGADGETS_BROWSER_ACTIVE] = ControlBarGadgetsListObject,
										InputListFrame,
										MUIA_Background, MUII_ListBack,
										MUIA_NList_Format, "W=-1, W=25 MICW=3, W=-1",
										MUIA_NList_DragSortable, TRUE,
										MUIA_NList_ShowDropMarks, TRUE,
										MUIA_NList_AutoVisible, TRUE,
										MUIA_NList_DisplayHook2, &ControlBarGadgetListDisplayHook,
										MUIA_NList_ConstructHook2, &ControlBarGadgetListConstructHook,
										MUIA_NList_DestructHook2, &ControlBarGadgetListDestructHook,
									End,
								End, //Listview
							End, //VGroup
						End, //HGroup

						Child, VSpace(1),

						Child, ColGroup(2),

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_BROWSER_NORMALIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_NORMALIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_SELECTEDIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_BROWSER_DISABLEDIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, VGroup,
								Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_GADGETHELPTEXT)),
								Child, HVSpace,
								End, //VGroup
							Child, app->Obj[TEXTEDITOR_CONTROLBARGADGETS_BROWSER_HELPTEXT] = TextEditorObject,
								TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_CycleChain, TRUE,
								MUIA_Disabled, TRUE,
								MUIA_TextEditor_Contents, "",
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_GADGETHELPTEXT_SHORTHELP),
								End, //TextEditor

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTION)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_BROWSER_ACTION] = StringObject,
									MUIA_Disabled, TRUE,
									StringFrame,
									MUIA_String_Contents, "",
								End, //String
								Child, app->Obj[POP_CONTROLBARGADGETS_BROWSER_ACTION] = PopobjectObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopUp),
									MUIA_Popobject_Object, app->Obj[NLISTVIEW_CONTROLBARGADGETS_BROWSER_ACTION] = NListviewObject,
										MUIA_NListview_NList, NListObject,
											InputListFrame,
											MUIA_Background, MUII_ListBack,
											MUIA_NList_Format, ",",
											MUIA_NList_ConstructHook2, &CmdListConstructHook,
											MUIA_NList_DestructHook2, &CmdListDestructHook,
											MUIA_NList_DisplayHook2, &CmdListDisplayHook,
											MUIA_NList_CompareHook2, &CmdListCompareHook,
											MUIA_NList_AdjustWidth, TRUE,
											MUIA_NList_SortType, 1,
											MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 1,
											MUIA_NList_SourceArray, CommandsArray,
										End, //NListObject
									End, //NListviewObject
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTION_SHORTHELP),
								End, //PopobjectObject
								Child, HVSpace,
							End, //HGroup
						End, //ColGroup
					End, //VGroup

					Child, VSpace(1),

				End, //VGroup

				// --- Standard Window Control Bar
				Child, VGroup,

					Child, VSpace(1),

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMAL_GADGETS),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,
						MUIA_Weight, 500,

						Child, HGroup,
							Child, VGroup,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_AVAILABLE_GADGETS_SHORTHELP),
								Child, CLabel2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_AVAILABLE_GADGETS)),
								Child, app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_AVAILABLE] = NListviewObject,
									MUIA_Weight, 500,
									MUIA_Listview_Input, TRUE,
									MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
									MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
									MUIA_NListview_NList, app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_AVAILABLE] = ControlBarGadgetsListObject,
										InputListFrame,
										MUIA_Background, MUII_ListBack,
										MUIA_NList_Format, "W=-1, W=-1 MICW=3, W=-1",
										MUIA_NList_ShowDropMarks, FALSE,
										MUIA_NList_AutoVisible, TRUE,
										MUIA_NList_DisplayHook2, &ControlBarGadgetListDisplayHook,
										MUIA_NList_ConstructHook2, &ControlBarGadgetListConstructHook,
										MUIA_NList_DestructHook2, &ControlBarGadgetListDestructHook,
									End,
								End, //Listview
							End, //VGroup

							Child, VGroup,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTIVE_GADGETS_SHORTHELP),
								Child, CLabel2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTIVE_GADGETS)),
								Child, app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTIVE] = NListviewObject,
									MUIA_Weight, 500,
									MUIA_Listview_Input, TRUE,
									MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
									MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
									MUIA_NListview_NList, app->Obj[NLIST_CONTROLBARGADGETS_NORMAL_ACTIVE] = ControlBarGadgetsListObject,
										InputListFrame,
										MUIA_Background, MUII_ListBack,
										MUIA_NList_Format, "W=-1, W=25 MICW=3, W=-1",
										MUIA_NList_DragSortable, TRUE,
										MUIA_NList_ShowDropMarks, TRUE,
										MUIA_NList_AutoVisible, TRUE,
										MUIA_NList_DisplayHook2, &ControlBarGadgetListDisplayHook,
										MUIA_NList_ConstructHook2, &ControlBarGadgetListConstructHook,
										MUIA_NList_DestructHook2, &ControlBarGadgetListDestructHook,
									End,
								End, //Listview
							End, //VGroup
						End, //HGroup

						Child, VSpace(1),

						Child, ColGroup(2),

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_NORMAL_NORMALIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_NORMALIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_NORMALIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_SELECTEDIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_SELECTEDIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE] = PopaslObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopFile),
									MUIA_Popstring_String, StringObject,
										StringFrame,
									End, //String
									ASLFR_TitleText, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE_ASLTITLE),
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_DISABLEDIMAGE_SHORTHELP),
								End, //PopAsl
								Child, app->Obj[DTIMG_CONTROLBARGADGETS_NORMAL_DISABLEDIMAGE] = DataTypesImageObject,
									MUIA_ScaDtpic_Name, (ULONG) "",
									MUIA_ScaDtpic_FailIfUnavailable, FALSE,
								End, //DataTypesMCC
							End, //HGroup

							Child, VGroup,
								Child, Label1((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_GADGETHELPTEXT)),
								Child, HVSpace,
								End, //VGroup
							Child, app->Obj[TEXTEDITOR_CONTROLBARGADGETS_NORMAL_HELPTEXT] = TextEditorObject,
								TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_CycleChain, TRUE,
								MUIA_Disabled, TRUE,
								MUIA_TextEditor_Contents, "",
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_GADGETHELPTEXT_SHORTHELP),
								End, //TextEditor

							Child, Label2((ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTION)),
							Child, HGroup,
								Child, app->Obj[STRING_CONTROLBARGADGETS_NORMAL_ACTION] = StringObject,
									MUIA_Disabled, TRUE,
									StringFrame,
									MUIA_String_Contents, "",
								End, //String
								Child, app->Obj[POP_CONTROLBARGADGETS_NORMAL_ACTION] = PopobjectObject,
									MUIA_CycleChain, TRUE,
									MUIA_Disabled, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopUp),
									MUIA_Popobject_Object, app->Obj[NLISTVIEW_CONTROLBARGADGETS_NORMAL_ACTION] = NListviewObject,
										MUIA_NListview_NList, NListObject,
											InputListFrame,
											MUIA_Background, MUII_ListBack,
											MUIA_NList_Format, ",",
											MUIA_NList_ConstructHook2, &CmdListConstructHook,
											MUIA_NList_DestructHook2, &CmdListDestructHook,
											MUIA_NList_DisplayHook2, &CmdListDisplayHook,
											MUIA_NList_CompareHook2, &CmdListCompareHook,
											MUIA_NList_AdjustWidth, TRUE,
											MUIA_NList_SortType, 1,
											MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 1,
											MUIA_NList_SourceArray, CommandsArray,
										End, //NListObject
									End, //NListviewObject
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_CONTROLBAR_ACTION_SHORTHELP),
								End, //PopobjectObject
								Child, HVSpace,
							End, //HGroup
						End, //ColGroup
					End, //VGroup

					Child, VSpace(1),

				End, //VGroup

			End, //Register

			// Windows -- Layout
			Child, VGroup,
				Child, HVSpace,

				Child, VGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUTPREFERENCES),

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBDRAWER)),
						Child, app->Obj[CYCLE_ICONS_LAYOUT_WBDRAWER] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBDRAWER_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBTOOL)),
						Child, app->Obj[CYCLE_ICONS_LAYOUT_WBTOOL] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBTOOL_SHORTHELP),
						End, //Cycle
						Child, Label1((ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBPROJECT)),
						Child, app->Obj[CYCLE_ICONS_LAYOUT_WBPROJECT] = CycleObject,
							MUIA_Cycle_Entries, cIconLayoutModes,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_ICONSPAGE_LAYOUT_WBPROJECT_SHORTHELP),
						End, //Cycle
					End, //ColGroup

				End, //VGroup

				Child, HVSpace,

				Child, VGroup,
					MUIA_ShowMe, ((struct Library *) DOSBase)->lib_Version >= 51,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_TRANSPARENCY_WINDOW),
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_TRANSPARENCY_ACTIVEWINDOW),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_TRANSPARENCY_ACTIVEWINDOW_SHORTHELP),

						Child, ColGroup(3),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

							Child, app->Obj[SLIDER_TRANSPARENCY_ACTIVEWINDOW] = SliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 100,
								MUIA_Slider_Horiz, TRUE,
								MUIA_Numeric_Value, 50,
							End, //Slider

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),

							End, //ColGroup

						End, //VGroup

					Child, VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_WINDOWPAGE_TRANSPARENCY_INACTIVEWINDOW),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_WINDOWPAGE_TRANSPARENCY_INACTIVEWINDOW_SHORTHELP),

						Child, ColGroup(3),
							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

							Child, app->Obj[SLIDER_TRANSPARENCY_INACTIVEWINDOW] = SliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 100,
								MUIA_Slider_Horiz, TRUE,
								MUIA_Numeric_Value, 50,
							End, //Slider

							Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),

							End, //ColGroup

						End, //VGroup

					End, //VGroup

				Child, HVSpace,
			End, //VGroup

		End, //Register
	End, //VGroup
	&NewPrefsPages[6]);
///
}

//-----------------------------------------------------------------

static Object *GenerateTextWindowPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, RegisterObject,
			MUIA_Register_Titles, cTextWindowPages,
			MUIA_CycleChain, TRUE,

			// Fonts
			Child, VGroup,
				MUIA_Background, MUII_RegisterBack,

				Child, HVSpace,

				Child, VGroup,

					Child, HVSpace,

					Child, HGroup,
						Child, HVSpace,
						Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE)),
						Child, app->Obj[CHECK_TEXTWINDOW_TTTEXTWINDOWFONT_ENABLE] = CheckMarkHelp(FALSE, MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE_SHORTHELP),
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE_SHORTHELP),
					End, //HGroup

					Child, app->Obj[GROUP_TEXTWINDOW_FONT_SELECT] = VGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_ICONSPAGE_GROUP_ICONFONT),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,
						Child, HGroup,
//							  Child, Label2((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_FONT)),
							Child, app->Obj[POP_TEXTMODEFONT] = PopaslObject,
								MUIA_CycleChain, TRUE,
								MUIA_Popstring_Button, PopButton(MUII_PopUp),
								MUIA_Popstring_String, BetterStringObject,
									StringFrame,
									MUIA_String_Contents, "Xen/8",
								End, //BetterString
								ASLFR_TitleText, (ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_FONT_ASLTITLE),
								MUIA_Popasl_Type , ASL_FontRequest,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_FONT_SHORTHELP),
							End, //Pop
						End, //HGroup

						Child, app->Obj[STRING_TEXTMODEFONT_SAMPLE] = FontSampleObject,
							TextFrame,
							MUIA_Background, MUII_TextBack,
							MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_FONT_SAMPLETEXT),
							End, //FontSampleMCCObject
					End, //VGroup

					Child, app->Obj[GROUP_TEXTWINDOW_TTTEXTWINDOWFONT] = HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,
						MUIA_Disabled, TRUE,

						Child, VGroup,
							Child, app->Obj[POPSTRING_TEXTWINDOW_TTTEXTWINDOWFONT] = PopstringObject,
								MUIA_CycleChain, TRUE,
								MUIA_Popstring_Button, PopButton(MUII_PopUp),
								MUIA_Popstring_String, BetterStringObject,
									StringFrame,
									MUIA_String_Contents, " ",
									End, //BetterString
								MUIA_Popstring_OpenHook, &TextWindowTtfPopOpenHook,
								MUIA_Popstring_CloseHook, &TextWindowTtfPopCloseHook,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT_SHORTHELP),
							End, //PopstringObject

							Child, app->Obj[MCC_TEXTWINDOW_TTTEXTWINDOWFONT_SAMPLE] = FontSampleObject,
								TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT_SAMPLETEXT_SHORTHELP),
								End, //FontSampleMCCObject
						End, //VGroup
					End, //HGroup
				End, //VGroup

				Child, HVSpace,

			End, //VGroup

			// Columns
			Child, VGroup,
				MUIA_Background, MUII_RegisterBack,

				Child, HVSpace,

				Child, HGroup,
					MUIA_Weight, 500,

					Child, VGroup,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_AVAILABLE_COLUMNS_SHORTHELP),
						Child, CLabel2((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_AVAILABLE_COLUMNS)),
						Child, app->Obj[NLISTVIEW_STORAGE_FILEDISPLAY] = NListviewObject,
							MUIA_Listview_Input, TRUE,
							MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
							MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
							MUIA_NListview_NList, app->Obj[NLIST_STORAGE_FILEDISPLAY] = TextWindowColumnsListObject,
								InputListFrame,
								MUIA_Background, MUII_ListBack,
								//MUIA_NList_Format, "",
								MUIA_NList_DragSortable, FALSE,
								MUIA_NList_ShowDropMarks, FALSE,
								MUIA_NList_AutoVisible, TRUE,
								MUIA_NList_DisplayHook2, &FileDisplayDisplayHook,
								MUIA_NList_ConstructHook2, &FileDisplayConstructHook,
								MUIA_NList_DestructHook2, &FileDisplayDestructHook,
							End,
						End, //Listview
					End, //VGroup

					Child, VGroup,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_COLUMNS_INUSE_SHORTHELP),
						Child, CLabel2((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_COLUMNS_INUSE)),
						Child, app->Obj[NLISTVIEW_USE_FILEDISPLAY] = NListviewObject,
							MUIA_Listview_Input, TRUE,
							MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Shifted,
							MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
							MUIA_NListview_NList, app->Obj[NLIST_USE_FILEDISPLAY] = TextWindowColumnsListObject,
								InputListFrame,
								MUIA_Background, MUII_ListBack,
								//MUIA_NList_Format, "",
								MUIA_NList_DragSortable, TRUE,
								MUIA_NList_ShowDropMarks, TRUE,
								MUIA_NList_AutoVisible, TRUE,
								MUIA_NList_DisplayHook2, &FileDisplayDisplayHook,
								MUIA_NList_ConstructHook2, &FileDisplayConstructHook,
								MUIA_NList_DestructHook2, &FileDisplayDestructHook,
							End,
						End, //Listview
					End, //VGroup
				End, //HGroup

				Child, HVSpace,
			End, //VGroup

			// Selection marks
			Child, VGroup,
				MUIA_Background, MUII_RegisterBack,

				Child, app->Obj[GROUP_TEXTWINDOWS_SELECTIONMARK] = VGroup,

//				  	Child, HVSpace,

					Child, HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_GROUP_BASECOLOR),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, app->Obj[COLORADJUST_TEXTWINDOWS_SELECTIONMARK] = ColoradjustObject,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_SELECTIONMARK_BASECOLOR_SHORTHELP),
							End, //ColoradjustObject
					End, //HGroup

//				  	Child, HVSpace,

					Child, HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_SELECTION_BORDER_TRANSPARENCY),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

						Child, app->Obj[SLIDER_TEXTWINDOWS_SELECTBORDERTRANSPARENCY] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 255,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 255,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_SELECTIONMARK_BORDER_TRANSPARENCY_SHORTHELP),
						End, //Slider

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
					End, //HGroup

					Child, HGroup,
						MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_SELECTION_FILL_TRANSPARENCY),
						GroupFrame,
						MUIA_Background, MUII_GroupBack,

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_TRANSPARENT)),

						Child, app->Obj[SLIDER_TEXTWINDOWS_SELECTFILLTRANSPARENCY] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 255,
							MUIA_Slider_Horiz, TRUE,
							MUIA_Numeric_Value, 128,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_SELECTIONMARK_FILL_TRANSPARENCY_SHORTHELP),
						End, //Slider

						Child, Label1((ULONG) GetLocString(MSGID_DRAGNDROPPAGE_TRANSPARENCY_OPAQUE)),
					End, //HGroup

					Child, HVSpace,

				End, //VGroup

				Child, HGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, app->Obj[MCC_TEXTWINDOWS_SELECTMARKER_SAMPLE] = SelectMarkSampleObject,
						TextFrame,
						MUIA_Background, MUII_TextBack,
						TIHA_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					End, //
				End, //HGroup

			End, //VGroup

			// Miscellaneous
			Child, VGroup,
				MUIA_Background, MUII_RegisterBack,

				Child, HVSpace,

				Child, HGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),
						Child, Label1((ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_DRAWERSORT)),
						Child, app->Obj[CYCLE_DRAWERSORTMODE] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, cDrawerSortMode,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TEXTWINDOWSPAGE_DRAWERSORT_SHORTHELP),
							End, //CycleObject
					End, //ColGroup
					Child, HVSpace,

				End, //HGroup

				Child, HGroup,
					GroupFrame,
					MUIA_Background, MUII_GroupBack,

					Child, HVSpace,

					Child, ColGroup(2),

						Child, Label1((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_SOFTLINKS_UNDERLINED)),
						Child, app->Obj[SOFTTEXTSLINK] = CheckMarkHelp(FALSE, MSGID_FILEDISPLAYPAGE_SOFTLINKS_UNDERLINED_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_STRIPED)),
						Child, app->Obj[CHECK_STRIPED_WINDOW] = CheckMarkHelp(FALSE, MSGID_FILEDISPLAYPAGE_STRIPED_SHORTHELP),

						Child, Label1((ULONG) GetLocString(MSGID_FILEDISPLAYPAGE_SELECTTEXTICONNAME)),
						Child, app->Obj[CHECK_SELECTTEXTICONNAME] = CheckMarkHelp(FALSE, MSGID_FILEDISPLAYPAGE_SELECTTEXTICONNAME_SHORTHELP),
					End, //ColGroup
					Child, HVSpace,

				End, //HGroup
				Child, HVSpace,

			End, //VGroup
		End, //Register
	End, //VGroup
	&NewPrefsPages[7]);
///
}

//-----------------------------------------------------------------

static Object *GenerateTrueTypeFontsPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, HVSpace,

		Child, app->Obj[GROUP_TTGLOBALS] = HGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_GLOBALS),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,
			MUIA_Disabled, TRUE,

			Child, VGroup,
				Child, HVSpace,

				Child,	ColGroup(2),
					Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_ANTIALIASING)),
					Child, app->Obj[CYCLE_TTANTIALIAS] = CycleObject,
						MUIA_CycleChain, TRUE,
						MUIA_Cycle_Entries, cTTFontsAntiAliasing,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ANTIALIASING_SHORTHELP),
					End, //Radio
				End, //ColGroup

				Child, HVSpace,

				Child,	ColGroup(2),
					Child, Label(GetLocString(MSGID_TTFONTSPAGE_GAMMA)),
					Child, app->Obj[SLIDER_TTGAMMA] = TTGammaSliderObject,
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 100,
						MUIA_Numeric_Max, 5000,
						MUIA_Numeric_Value, 2500,
						MUIA_ShortHelp, GetLocString(MSGID_TTFONTSPAGE_GAMMA_SHORTHELP),
						End,
				End, //ColGroup

				Child, HVSpace,
			End, //HGroup
		End, //HGroup

		Child, app->Obj[GROUP_TTSCREENFONT] = HGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,
			MUIA_Disabled, TRUE,

			Child, VGroup,
				Child, HGroup,
					Child, HVSpace,
					Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_ENABLE)),
					Child, app->Obj[CHECK_TTSCREENFONT_ENABLE] = CheckMarkHelp(FALSE, MSGID_TTFONTSPAGE_SCREENFONT_ENABLE_SHORTHELP),

					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_ENABLE_SHORTHELP),
				End, //HGroup

				Child, app->Obj[POPSTRING_TTSCREENFONT] = PopstringObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, " ",
					End, //BetterString
					MUIA_Popstring_OpenHook, &ScreenTtfPopOpenHook,
					MUIA_Popstring_CloseHook, &ScreenTtfPopCloseHook,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_SHORTHELP),
				End, //PopstringObject

				Child, app->Obj[MCC_TTSCREENFONT_SAMPLE] = FontSampleObject,
					TextFrame,
					MUIA_Background, MUII_TextBack,
					MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_SAMPLETEXT_SHORTHELP),
					End, //FontSampleMCCObject

			End, //VGroup

		End, //HGroup

		Child, HVSpace,

		Child, app->Obj[GROUP_TTICONFONT] = HGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,
			MUIA_Disabled, TRUE,

			Child, VGroup,
				Child, HGroup,
					Child, HVSpace,
					Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_ENABLE)),
					Child, app->Obj[CHECK_TTICONFONT_ENABLE] = CheckMarkHelp(FALSE, MSGID_FONTSPAGE_TTFICONFONT_ENABLE_SHORTHELP),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_FONTSPAGE_TTFICONFONT_ENABLE_SHORTHELP),
				End, //HGroup

				Child, app->Obj[POPSTRING_TTICONFONT] = PopstringObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, " ",
						End, //BetterString
					MUIA_Popstring_OpenHook, &IconTtfPopOpenHook,
					MUIA_Popstring_CloseHook, &IconTtfPopCloseHook,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_SHORTHELP),
				End, //PopstringObject

				Child, app->Obj[MCC_TTICONFONT_SAMPLE] = FontSampleObject,
					TextFrame,
					MUIA_Background, MUII_TextBack,
					MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_ICONFONT_SAMPLETEXT_SHORTHELP),
					End, //FontSampleMCCObject
			End, //VGroup

		End, //HGroup

		Child, HVSpace,

		Child, app->Obj[GROUP_TTTEXTWINDOWFONT] = HGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,
			MUIA_Disabled, TRUE,

			Child, VGroup,
				Child, HGroup,
					Child, HVSpace,
					Child, Label1((ULONG) GetLocString(MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE)),
					Child, app->Obj[CHECK_TTTEXTWINDOWFONT_ENABLE] = CheckMarkHelp(FALSE, MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE_SHORTHELP),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE_SHORTHELP),
				End, //HGroup

				Child, app->Obj[POPSTRING_TTTEXTWINDOWFONT] = PopstringObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_Popstring_String, BetterStringObject,
						StringFrame,
						MUIA_String_Contents, " ",
						End, //BetterString
					MUIA_Popstring_OpenHook, &TextWindowTtfPopOpenHook,
					MUIA_Popstring_CloseHook, &TextWindowTtfPopCloseHook,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT_SHORTHELP),
				End, //PopstringObject

				Child, app->Obj[MCC_TTTEXTWINDOWFONT_SAMPLE] = FontSampleObject,
					TextFrame,
					MUIA_Background, MUII_TextBack,
					MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_TEXTWINDOWFONT_SAMPLETEXT_SHORTHELP),
					End, //FontSampleMCCObject
			End, //VGroup

		End, //HGroup

		Child, HVSpace,

	End, //VGroup
	&NewPrefsPages[8]);
///
}

//-----------------------------------------------------------------

static Object *GenerateMiscPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, HVSpace,

		Child, VGroup,
			MUIA_FrameTitle, GetLocString(MSGID_MISCPAGE_POPUPMENUS),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, ColGroup(2),
				Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_POPUP_SELECTED_ALWAYS)),
				Child, HGroup,
					Child, app->Obj[CHECK_MISCPAGE_POPUP_SELECTED_ALWAYS] = CheckMarkHelp(FALSE, MSGID_TTFONTSPAGE_TTFTEXTWINDOWFONT_ENABLE_SHORTHELP),
					Child, HVSpace,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_POPUP_SELECTED_ALWAYS_SHORTHELP),
					End, //HGroup

				Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_POPUP_SELECTED_HOTKEY)),
				Child, HGroup,
					Child, app->Obj[POPUP_SELECTED_HOTKEY] = HotkeyStringObject,
						StringFrame,
						MUIA_String_Contents, "",
						MUIA_HotkeyString_Snoop, FALSE,
					End, //HotkeyString
					Child, app->Obj[POPUP_SELECTED_HOTKEY_SCAN] = ScanButton,
				End, //HGroup
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_POPUP_SELECTED_HOTKEY_SHORTHELP),
			End, //ColGroup

			Child, HVSpace,
		End, //VGroup

		Child, VGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_MISCPAGE_LABELS_STACK),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, app->Obj[GROUP2_DEFAULTSTACKSIZE] = HGroup,
				MUIA_Background, MUII_GroupBack,

				Child, FloattextObject,
					TextFrame,
					MUIA_Floattext_Justify, FALSE,
					MUIA_Floattext_Text, (ULONG) GetLocString(MSGID_MISCPAGE_DEFAULTSTACKSIZE_OS35),
				End, //Floattext
				Child, app->Obj[POP_WORKBENCHPREFS] = ImageObject,
					MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_Image_Spec, "6:18", //MUII_PopUp,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_POP_WORKBENCHPREFS_SHORTHELP),
				End, //Image
			End, //HGroup

			Child, app->Obj[GROUP_DEFAULTSTACKSIZE] = ColGroup(2),
				Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_DEFAULTSTACKSIZE)),
				Child, app->Obj[SLIDER_DEFAULTSTACKSIZE] = SliderObject,
					MUIA_CycleChain, TRUE,
					MUIA_Numeric_Format, "%luK",
					MUIA_Slider_Horiz, TRUE,
					MUIA_Slider_Level, 16,
					MUIA_Slider_Min, 8,
					MUIA_Slider_Max, 2048,
				End, //Slider
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_DEFAULTSTACKSIZE_SHORTHELP),
			End, //ColGroup

			Child, HVSpace,
		End, //VGroup

		Child, VGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_MISCPAGE_LABELS_UNDOSTEPS),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, ColGroup(2),
				Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_UNDOSTEPS)),
				Child, app->Obj[SLIDER_UNDOSTEPS] = SliderObject,
					MUIA_CycleChain, TRUE,
					MUIA_Slider_Horiz, TRUE,
					MUIA_Slider_Level, 10,
					MUIA_Slider_Min, 1,
					MUIA_Slider_Max, 100,
				End, //Slider
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_UNDOSTEPS_SHORTHELP),
			End, //ColGroup

			Child, HVSpace,
		End, //VGroup

		Child, VGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_MISCPAGE_LABELS_FILEOPERATIONS),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, app->Obj[GROUP_COPYBUFFERSIZE] = ColGroup(2),
				Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_COPYBUFFERSIZE)),
				Child, app->Obj[SLIDER_COPYBUFFERSIZE] = BufferSizeSliderObject,
					MUIA_CycleChain, TRUE,
					MUIA_Slider_Horiz, TRUE,
					MUIA_Slider_Level, 28 - 15,
					MUIA_Slider_Min, 15,
					MUIA_Slider_Max, 28,
				End, //Slider
				MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_COPYBUFFERSIZE_SHORTHELP),
			End, //ColGroup

			Child, HVSpace,
		End, //VGroup


		Child, VGroup,
			//MUIA_FrameTitle, "",
			GroupFrame,
			MUIA_Background, MUII_GroupBack,

			Child, HVSpace,

			Child, HGroup,
				Child, HVSpace,

				Child, ColGroup(2),
					Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_MENU_CURRENTDIR)),
					Child, app->Obj[CHECK_MENUCURRENTDIR] = CheckMarkHelp(FALSE, MSGID_MISCPAGE_MENU_CURRENTDIR_SHORTHELP),

					Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_HARD_EMULATION)),
					Child, app->Obj[CHECK_HARDEMULATION] = CheckMarkHelp(FALSE, MSGID_MISCPAGE_HARD_EMULATION_SHORTHELP),

					Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_USE_EXALL)),
					Child, app->Obj[CHECK_USEEXALL] = CheckMarkHelp(FALSE, MSGID_MISCPAGE_USE_EXALL_SHORTHELP),

					Child, Label1((ULONG) GetLocString(MSGID_MISCPAGE_CREATE_LINKS)),
					Child, app->Obj[CYCLE_CREATELINKS] = CycleObject,
						MUIA_CycleChain, TRUE,
						MUIA_Cycle_Entries, cCreateLinkTypes,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_MISCPAGE_CREATE_LINKS_SHORTHELP),
					End, //Image


				End, //ColGroup

				Child, HVSpace,
			End, //HGroup

			Child, HVSpace,

		End, //VGroup

		Child, HVSpace,

	End, //VGroup
	&NewPrefsPages[9]);
///
}

//-----------------------------------------------------------------

static Object *GeneratePluginsPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, VSpace(2),

		Child, VGroup,
			Child, HGroup,
				//Child, HVSpace,
				Child, VGroup,
					Child, Label((ULONG) GetLocString(MSGID_PLUGINSPAGE_INSTALLEDPLUGINS)),
					Child, app->Obj[PLUGIN_LISTVIEW] = NListviewObject,
						MUIA_NListview_NList, app->Obj[PLUGIN_LIST] = NListObject,
							InputListFrame,
							MUIA_CycleChain, TRUE,
							MUIA_Background, MUII_ListBack,
							MUIA_NList_Format, "W=-1, W=-1",
							MUIA_NList_DefaultObjectOnClick, TRUE,
							MUIA_NList_ShowDropMarks, FALSE,
							//MUIA_NList_AdjustWidth, TRUE,
							MUIA_NList_AutoVisible, TRUE,
							MUIA_NList_Title, TRUE,
							MUIA_NList_TitleSeparator, TRUE,
							MUIA_NList_SortType, 0,
							MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
							MUIA_NList_MinColSortable, 0,
							MUIA_NList_DisplayHook2, &PluginListDisplayHook,
							MUIA_NList_ConstructHook2, &PluginListConstructHook,
							MUIA_NList_DestructHook2, &PluginListDestructHook,
							MUIA_NList_CompareHook2, &PluginListCompareHook,
							End, //NList
						MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_INSTALLEDPLUGINS_SHORTHELP),
						End, //Listview

					Child, BalanceObject,
						End, //Balance

					Child, HGroup,
						Child, HVSpace,
						Child, app->Obj[ADD_PLUGIN] = TextObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_CycleChain, TRUE,
							MUIA_Background, MUII_ButtonBack,
							MUIA_Text_Contents, (ULONG) GetLocString(MSGID_PLUGINSPAGE_ADD_NEW),
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_ADD_NEW_SHORTHELP),
							End, //TextObject

						Child, HVSpace,

						Child, app->Obj[REMOVE_PLUGIN] = TextObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							ButtonFrame,
							MUIA_CycleChain, TRUE,
							MUIA_Background, MUII_ButtonBack,
							MUIA_Text_Contents, (ULONG) GetLocString(MSGID_PLUGINSPAGE_REMOVE_SELECTED),
							MUIA_Disabled, TRUE,
							MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_REMOVE_SELECTED_SHORTHELP),
							End, //TextObject

						Child, HVSpace,

						End, //HGroup

					End, //VGroup

				//Child, HVSpace,
				End, //HGroup

			Child, ColGroup(2),
				Child, VGroup,
					Child, Label((ULONG) GetLocString(MSGID_PLUGINSPAGE_NAME)),
					Child, app->Obj[PLUGIN_NAME] = TextObject,
						TextFrame,
						MUIA_Background, MUII_ReadListBack,
						MUIA_Text_Contents,"",
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_NAME_SHORTHELP),
					End,

					Child, Label1((ULONG) GetLocString(MSGID_PLUGINSPAGE_FILENAME)),
					Child, app->Obj[PLUGIN_FILENAME] = TextObject,
						TextFrame,
						MUIA_Background, MUII_ReadListBack,
						MUIA_Text_Contents, "",
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_FILENAME_SHORTHELP),
					End,

					Child, Label1((ULONG) GetLocString(MSGID_PLUGINSPAGE_VERSION)),
					Child, app->Obj[PLUGIN_VERSION] = TextObject,
						TextFrame,
						MUIA_Background, MUII_ReadListBack,
						MUIA_Text_Contents, "",
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_VERSION_SHORTHELP),
					End,

					Child, HGroup,
						MUIA_InputMode, MUIV_InputMode_RelVerify,
						ButtonFrame,
						MUIA_Background, MUII_ButtonBack,
/* currently unused */				MUIA_Disabled, TRUE,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_EDIT_PREFERENCES_SHORTHELP),

						Child, TextObject,
							MUIA_Text_Contents, (ULONG) GetLocString(MSGID_PLUGINSPAGE_EDIT_PREFERENCES),
						End,

						Child, ImageObject,
							MUIA_Image_Spec, "6:18", //MUII_PopUp,
						End, //Image
					End, //HGroup
				End, //VGroup

				Child, VGroup,
					Child, Label1((ULONG) GetLocString(MSGID_PLUGINSPAGE_DESCRIPTION)),
					Child, app->Obj[PLUGIN_DESCRIPTION] = FloattextObject,
						TextFrame,
						MUIA_Background, MUII_ReadListBack,
						MUIA_Floattext_Justify, FALSE,
						MUIA_Floattext_Text, "",
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_DESCRIPTION_SHORTHELP),
					End, //FloattextObject

					Child, Label1((ULONG) GetLocString(MSGID_PLUGINSPAGE_CREATOR)),
					Child, app->Obj[PLUGIN_CREATOR] = FloattextObject,
						TextFrame,
						MUIA_Background, MUII_ReadListBack,
						MUIA_Floattext_Justify, FALSE,
						MUIA_Floattext_Text, "",
						MUIA_VertWeight, 20,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_PLUGINSPAGE_CREATOR_SHORTHELP),
						End, //FloattextObject

					End, //VGroup
				End, //ColGroup
			End, //VGroup
		Child, VSpace(2),

	End, //VGroup
	&NewPrefsPages[10]);
///
}

//-----------------------------------------------------------------

static Object *GenerateModulesPage(struct SCAModule *app)
{
///
	return CreatePrefsPage(app,
		VGroup,
		MUIA_Background, MUII_PageBack,

		Child, HVSpace,

		Child, VGroup,
			MUIA_FrameTitle, (ULONG) GetLocString(MSGID_MODULESPAGE_MODULE_PREFERENCES),
			GroupFrame,
			MUIA_Background, MUII_GroupBack,
			MUIA_Weight, 500,

			Child, HGroup,

			Child, app->Obj[NLISTVIEW_MODULES] = NListviewObject,
				MUIA_Listview_Input, FALSE,
				MUIA_CycleChain, 1,
				MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
				MUIA_Listview_DragType, MUIV_Listview_DragType_None,
				MUIA_NListview_NList, app->Obj[NLIST_MODULES] = NListObject,
					MUIA_NList_DefaultObjectOnClick, TRUE,
					MUIA_Background, MUII_ListBack,
					InputListFrame,
					MUIA_NList_Format, ",,",
					MUIA_NList_DragSortable, FALSE,
					MUIA_NList_ShowDropMarks, FALSE,
					MUIA_NList_AutoVisible, TRUE,
					MUIA_NList_DisplayHook2, &ModulesListDisplayHook,
					MUIA_NList_ConstructHook2, &ModulesListConstructHook,
					MUIA_NList_DestructHook2, &ModulesListDestructHook,
				End,
			End, //Listview

			End, //HGroup
		End, //VGroup

		Child, HVSpace,

		End, //VGroup
		&NewPrefsPages[11]);
///
}

//-----------------------------------------------------------------


