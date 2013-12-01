// PatternPrefs.c
// $Date$
// $Revision$

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/resident.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <devices/clipboard.h>
#include <devices/timer.h>
#include <utility/utility.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <prefs/prefhdr.h>
#include <prefs/wbpattern.h>
#include <scalos/scalosgfx.h>

#include <clib/alib_protos.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/locale.h>
#include <proto/iffparse.h>
#include <proto/datatypes.h>
#include <proto/scalosgfx.h>
#include <proto/timer.h>

#ifndef __AROS__
#define	NO_INLINE_STDARG
#endif
#include <proto/muimaster.h>

#include <mui/NListview_mcc.h>
#include <mui/Lamp_mcc.h>
#include <scalos/pattern.h>
#include <scalos/scalosprefsplugin.h>
#include <scalos/scalospatternprefsplugin.h>

#include <defs.h>
#include <Year.h>

#include "PatternPrefs.h"
#include "Backfill.h"
#include "BitMapMCC.h"
#include "DataTypesMCC.h"
#include "LoadDT.h"
#include "debug.h"
#include "plugin.h"

#define	ScalosPattern_NUMBERS
#define	ScalosPattern_ARRAY
#define	ScalosPattern_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#ifdef __AROS__
#define myNListObject BOOPSIOBJMACRO_START(myNListClass->mcc_Class)
#define PrecSliderObject BOOPSIOBJMACRO_START(PrecSliderClass->mcc_Class)
#define PatternSliderObject BOOPSIOBJMACRO_START(PatternSliderClass->mcc_Class)
#else
#define myNListObject NewObject(myNListClass->mcc_Class, 0
#define PrecSliderObject NewObject(PrecSliderClass->mcc_Class, 0
#define PatternSliderObject NewObject(PatternSliderClass->mcc_Class, 0
#endif

//----------------------------------------------------------------------------

#define	USE_THUMBNAILS		0

#define	THUMBNAIL_WIDTH		50
#define	THUMBNAIL_HEIGHT	50

#define	LISTFORMAT_THUMBNAILS 	"P=\33r BAR, BAR, BAR, BAR"
#define	LISTFORMAT_STANDARD	"P=\33r BAR, BAR, BAR"

//----------------------------------------------------------------------------

#define IMG(prefix1,PREFIX2) \
  BodychunkObject,\
    MUIA_FixWidth             , PREFIX2##_WIDTH ,\
    MUIA_FixHeight            , PREFIX2##_HEIGHT,\
    MUIA_Bitmap_Width         , PREFIX2##_WIDTH ,\
    MUIA_Bitmap_Height        , PREFIX2##_HEIGHT,\
    MUIA_Bodychunk_Depth      , PREFIX2##_DEPTH ,\
    MUIA_Bodychunk_Body       , (UBYTE *) prefix1##_body,\
    MUIA_Bodychunk_Compression, PREFIX2##_COMPRESSION,\
    MUIA_Bodychunk_Masking    , PREFIX2##_MASKING,\
    MUIA_Bitmap_SourceColors  , (ULONG *) prefix1##_colors,\
    MUIA_Bitmap_Transparent   , 0,\
  End

//----------------------------------------------------------------------------

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

// local data structures

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_CycleChain, TRUE, \
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp, HelpText,\
		End

#define CheckMarkHelp(selected, HelpTextID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_CycleChain       , TRUE, \
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		MUIA_ShortHelp	      , GetLocString(HelpTextID),\
		End


#define	Application_Return_EDIT	0
#define	Application_Return_USE	1001
#define	Application_Return_SAVE	1002

#define	MAX_FILENAME	512


struct RGBlong
	{
	ULONG Red;
	ULONG Green;
	ULONG Blue;
	};

struct TempRastPort
	{
	struct RastPort trp_rp;
	ULONG trp_Width;
	};
struct PatternListEntry
	{
	struct ScaExtPatternPrefs ple_PatternPrefs;
	Object *ple_MUI_ImageObject;
	ULONG	ple_ThumbnailImageNr;
	struct ScalosBitMapAndColor *ple_SAC;
	UBYTE *ple_BitMapArray;
	};

//----------------------------------------------------------------------------

// aus mempools.lib
#if !defined(__amigaos4__) && !defined(__AROS__)
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

// local functions

DISPATCHER_PROTO(PatternPrefs);
static Object *CreatePrefsGroup(struct PatternPrefsInst *inst);
static void CreateSubWindows(struct PatternPrefsInst *inst);
static ULONG DoOmSet(Class *cl, Object *o, struct opSet *ops);
static Object *CreatePrefsImage(void);
static void InitHooks(struct PatternPrefsInst *inst);

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static STRPTR GetLocString(ULONG MsgId);
static void TranslateStringArray(STRPTR *stringArray);
static void TranslateNewMenu(struct NewMenu *nm);
static SAVEDS(APTR) INTERRUPT ListConstructHookFunc(struct Hook *hook, Object *obj, struct NList_ConstructMessage *msg);
static SAVEDS(void) INTERRUPT ListDestructHookFunc(struct Hook *hook, Object *obj, struct NList_DestructMessage *msg);
static SAVEDS(ULONG) INTERRUPT ListDisplayHookFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm);
static SAVEDS(LONG) INTERRUPT ListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *msg);
static SAVEDS(APTR) INTERRUPT SettingsChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
DISPATCHER_PROTO(PatternSlider);
DISPATCHER_PROTO(PrecSlider);
static LONG ReadPrefsFile(struct PatternPrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet);
static LONG ReadPrefsBitMap(struct PatternPrefsInst *inst, struct IFFHandle *iff,
	struct PatternListEntry *scp, struct TempRastPort *trp);
static LONG WritePrefsFile(struct PatternPrefsInst *inst, CONST_STRPTR Filename);
static LONG WritePrefsColorTable(struct IFFHandle *iff, const struct PatternListEntry *scp);
static LONG WritePrefsBitMap(struct IFFHandle *iff,
	const struct PatternListEntry *scp, struct TempRastPort *trp);
static LONG SaveIcon(struct PatternPrefsInst *inst, CONST_STRPTR IconName);
static SAVEDS(void) INTERRUPT ListSelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ListChangeEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ListChangeEntry2HookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ListChangeEntry3HookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AddNewEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT RemEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT PreviewSelectHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ShowPreviewHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AutoPreviewHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ThumbnailsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AppMsgInMainListHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT DesktopPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ScreenPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IconWindowPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT TextWindowPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT ReloadThumbnailsHookFunc(struct Hook *hook, Object *o, Msg msg);

static void PreviewSelect(struct PatternPrefsInst *inst);
static void SetPrecision(struct PatternPrefsInst *inst, BYTE Precision);
static BYTE GetPrecision(struct PatternPrefsInst *inst);
static void SetChangedFlag(struct PatternPrefsInst *inst, BOOL changed);
static void ParseToolTypes(struct PatternPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt);
static Object *CreateEmptyThumbnailImage(struct PatternPrefsInst *inst);
static void RemoveThumbnailImages(struct PatternPrefsInst *inst);
static void CreateThumbnailImages(struct PatternPrefsInst *inst);
static void CreateThumbnailImage(struct PatternPrefsInst *inst, struct PatternListEntry *scp);
static void UpdateUseThumbnails(struct PatternPrefsInst *inst);
static void DisableBackgroundColorGadgets(struct PatternPrefsInst *inst, UWORD RenderType, UWORD BgType);

DISPATCHER_PROTO(myNList);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

//----------------------------------------------------------------------------

// local data items

struct Library *MUIMasterBase;
T_LOCALEBASE LocaleBase;
struct GfxBase *GfxBase;
struct Library *IconBase;
struct Library *IFFParseBase;
T_UTILITYBASE UtilityBase;
struct IntuitionBase *IntuitionBase;
struct Library *DataTypesBase;
struct Library *GuiGFXBase;
struct ScalosGfxBase *ScalosGfxBase;
struct Library *CyberGfxBase;
T_TIMERBASE TimerBase;

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* __GNUC__ && !__MORPHOS && !__amigaos4__ */

#ifdef __amigaos4__
struct Library *DOSBase;
struct DOSIFace *IDOS;
struct MUIMasterIFace *IMUIMaster;
struct LocaleIFace *ILocale;
struct GraphicsIFace *IGraphics;
struct IconIFace *IIcon;
struct IFFParseIFace *IIFFParse;
struct UtilityIFace *IUtility;
struct IntuitionIFace *IIntuition;
struct DataTypesIFace *IDataTypes;
struct GuiGFXIFace *IGuiGFX;
struct ScalosGfxIFace *IScalosGfx;
struct CyberGfxIFace *ICyberGfx;
struct Library *NewlibBase;
struct Interface *INewlib;
struct TimerIFace *ITimer;
#endif

#ifdef __AROS__
struct DosLibrary *DOSBase;
#endif

static ULONG Signature = 0x4711;

static T_TIMEREQUEST *TimerIORequest;

struct MUI_CustomClass *PrecSliderClass;
struct MUI_CustomClass *PatternSliderClass;
struct MUI_CustomClass *myNListClass;
struct MUI_CustomClass *PatternPrefsClass;
struct MUI_CustomClass *BitMapPicClass;
struct MUI_CustomClass *DataTypesImageClass;

static struct Catalog *PatternPrefsCatalog;
static struct Locale *PatternPrefsLocale;

static ULONG MajorVersion, MinorVersion;

static const struct MUIP_ScalosPrefs_MCCList RequiredMccList[] =
	{
	{ MUIC_Lamp, 11, 1 },
	{ MUIC_NListview, 18, 0 },
	{ NULL, 0, 0 }
	};
static const struct Hook PatternPrefsHooks[] =
{
	{ { NULL, NULL }, HOOKFUNC_DEF(ListDisplayHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListConstructHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListDestructHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListCompareHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListSelectEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListChangeEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListChangeEntry2HookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ListChangeEntry3HookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddNewEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(LastSavedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RestoreHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ResetToDefaultsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AboutHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(OpenHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SaveAsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(PreviewSelectHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ContextMenuTriggerHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ShowPreviewHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AutoPreviewHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AppMsgInMainListHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SettingsChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ThumbnailsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(DesktopPatternChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ScreenPatternChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(IconWindowPatternChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(TextWindowPatternChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AslIntuiMsgHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ReloadThumbnailsHookFunc), NULL },
};


static struct NewMenu ContextMenus[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,		        NULL, 					        0,      0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_OPEN, 	        (STRPTR) MSGID_MENU_PROJECT_OPEN_SHORT,         0,      0, (APTR) HOOKNDX_Open },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_SAVEAS,         (STRPTR) MSGID_MENU_PROJECT_SAVEAS_SHORT,       0,      0, (APTR) HOOKNDX_SaveAs },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_ENABLE_THUMBNAILS,      NULL, 			        CHECKIT|MENUTOGGLE,     0, (APTR) HOOKNDX_Thumbnails },
	{  NM_ITEM, (STRPTR) MSGID_MENU_ENABLE_PREVIEW,	        NULL, 			        CHECKIT|MENUTOGGLE,     0, (APTR) HOOKNDX_ShowPreview },
	{  NM_ITEM, (STRPTR) MSGID_MENU_AUTO_PREVIEW,	        NULL, 			        CHECKIT|MENUTOGGLE,     0, (APTR) HOOKNDX_AutoPreview },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_RELOAD_THUMBNAILS,	NULL, 			        		0,     	0, (APTR) HOOKNDX_ReloadThumbnails },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,	        NULL, 					        0,      0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static STRPTR dithermodecont[] =
	{
	(STRPTR) MSGID_DITHERMODE1NAME,	        // DITHERMODE_NONE
	(STRPTR) MSGID_DITHERMODE2NAME,	        // DITHERMODE_FS
	(STRPTR) MSGID_DITHERMODE3NAME,	        // DITHERMODE_RANDOM
	NULL 
	};

//  scp_RenderType values
static STRPTR patternmode[] =
	{
	(STRPTR) MSGID_RENDERTYPE_TILED,
	(STRPTR) MSGID_RENDERTYPE_FITSIZE,
	(STRPTR) MSGID_CENTEREDNAME,
	(STRPTR) MSGID_SCALEDMINNAME,
	(STRPTR) MSGID_SCALEDMAXNAME,
	NULL
	};

// title strings for the register pages
static STRPTR RegisterTitleStrings[] =
	{
	(STRPTR) MSGID_REGISTER_PATTERNLIST,
	(STRPTR) MSGID_REGISTER_BACKGROUNDCOLORS,
	(STRPTR) MSGID_REGISTER_DEFAULTS,
	NULL
	};

static STRPTR CycleBackgroundModeStrings[] =
	{
	(STRPTR) MSGID_BGTYPE_NONE,
	(STRPTR) MSGID_BGTYPE_SINGLECOLOR,
	(STRPTR) MSGID_BGTYPE_HOR_GRADIENT,
	(STRPTR) MSGID_BGTYPE_VERT_GRADIENT,
	NULL
	};

//---------------------------------------------------------------

VOID closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf(__FUNC__ "/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (DataTypesImageClass)
		{
		CleanupDtpicClass(DataTypesImageClass);
		DataTypesImageClass = NULL;
		}
	if (BitMapPicClass)
		{
		CleanupBitMappicClass(BitMapPicClass);
		BitMapPicClass = NULL;
		}
	if (myNListClass)
		{
		MUI_DeleteCustomClass(myNListClass);
		myNListClass = NULL;
		}
	if (PrecSliderClass)
		{
		MUI_DeleteCustomClass(PrecSliderClass);
		PrecSliderClass = NULL;
		}
	if (PatternSliderClass)
		{
		MUI_DeleteCustomClass(PatternSliderClass);
		PatternSliderClass = NULL;
		}
	CleanupBackfillClass();

	if (PatternPrefsCatalog)
		{
		CloseCatalog(PatternPrefsCatalog);
		PatternPrefsCatalog = NULL;
		}
	if (PatternPrefsLocale)
		{
		CloseLocale(PatternPrefsLocale);
		PatternPrefsLocale = NULL;
		}

	if (PatternPrefsClass)
		{
		MUI_DeleteCustomClass(PatternPrefsClass);
		PatternPrefsClass = NULL;
		}

	CloseLibraries();

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif
}


LIBFUNC_P2(ULONG, LIBSCAGetPrefsInfo,
	D0, ULONG, which,
	A6, struct PluginBase *, PluginBase)
{
	ULONG result;

	(void) PluginBase;

	d1(kprintf("%s/%s/%ld: which=%ld\n", __FILE__, __FUNC__, __LINE__, which));

	switch(which)
		{
	case SCAPREFSINFO_GetClass:
		result = (ULONG) PatternPrefsClass;
		break;

	case SCAPREFSINFO_GetTitle:
		result = (ULONG) GetLocString(MSGID_PLUGIN_LIST_TITLE);
		break;

	case SCAPREFSINFO_GetTitleImage:
		result = (ULONG) CreatePrefsImage();
		break;

	default:
		result = 0;
		break;
		}

	d1(kprintf("%s/%s/%ld: ResultString=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));

	return result;
}
LIBFUNC_END

//----------------------------------------------------------------------------

DISPATCHER(PatternPrefs)
{
	struct PatternPrefsInst *inst;
	ULONG result = 0;

	switch(msg->MethodID)
		{
	case OM_NEW:
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		d1(KPrintF("%s/%s/%ld: OM_NEW obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		if (obj)
			{
			Object *prefsobject;
			struct opSet *ops = (struct opSet *) msg;

			inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);

			memset(inst, 0, sizeof(struct PatternPrefsInst));

			inst->ppb_Changed = FALSE;
			inst->ppb_fDontChange = FALSE;
			inst->ppb_patternPrecision = PRECISION_IMAGE;
			inst->ppb_fPreview = TRUE;
			inst->ppb_fAutoPreview = FALSE;
			inst->ppb_PreviewWeight = 20;
			inst->ppb_WBScreen = LockPubScreen("Workbench");
			inst->ppb_ThumbnailWidth = THUMBNAIL_WIDTH;
			inst->ppb_ThumbnailHeight = THUMBNAIL_HEIGHT;
			inst->ppb_ThumbnailImageNumber = 0;
			inst->ppb_UseSplashWindow = TRUE;

#if USE_THUMBNAILS
			inst->ppb_UseThumbNails = TRUE;
#else /* USE_THUMBNAILS */
			inst->ppb_UseThumbNails = FALSE;
#endif /* USE_THUMBNAILS */

			InitHooks(inst);

			inst->ppb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, TRUE, ops->ops_AttrList);
			inst->ppb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) "", ops->ops_AttrList);
			inst->ppb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, (ULONG) NULL, ops->ops_AttrList);
			inst->ppb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, (ULONG) NULL, ops->ops_AttrList);
			inst->ppb_fPreview = GetTagData(MUIA_ScalosPrefs_PatternPrefs_Preview, TRUE, ops->ops_AttrList);
			inst->ppb_fAutoPreview = GetTagData(MUIA_ScalosPrefs_PatternPrefs_AutoPreview, FALSE, ops->ops_AttrList);
			inst->ppb_PreviewWeight = GetTagData(MUIA_ScalosPrefs_PatternPrefs_PreviewWeight, 20, ops->ops_AttrList);
			inst->ppb_UseThumbNails = GetTagData(MUIA_ScalosPrefs_PatternPrefs_Thumbnails, inst->ppb_UseThumbNails, ops->ops_AttrList);

			prefsobject = CreatePrefsGroup(inst);
			d1(kprintf("%s/%s/%ld: prefsobject=%08lx\n", __FILE__, __FUNC__, __LINE__, prefsobject));
			if (prefsobject)
				{
				DoMethod(obj, OM_ADDMEMBER, prefsobject);

				result = (ULONG) obj;
				}
			else
				{
				CoerceMethod(cl, obj, OM_DISPOSE);
				}
			}
		break;

	case OM_DISPOSE:
		d1(KPrintF("%s/%s/%ld: OM_DISPOSE obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		if (inst->ppb_WBScreen)
			{
			UnlockPubScreen(NULL, inst->ppb_WBScreen);
			inst->ppb_WBScreen = NULL;
			}
		if (inst->ppb_LoadReq)
			{
			MUI_FreeAslRequest(inst->ppb_LoadReq);
			inst->ppb_LoadReq = NULL;
			}
		if (inst->ppb_SaveReq)
			{
			MUI_FreeAslRequest(inst->ppb_SaveReq);
			inst->ppb_SaveReq = NULL;
			}
		if (inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap])
			{
			MUI_DisposeObject(inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap]);
			inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap] = NULL;
			}
		if (inst->ppb_Objects[OBJNDX_ContextMenu])
			{
			MUI_DisposeObject(inst->ppb_Objects[OBJNDX_ContextMenu]);
			inst->ppb_Objects[OBJNDX_ContextMenu] = NULL;
			}
		return DoSuperMethodA(cl, obj, msg);
		break;

	case OM_SET:
		d1(KPrintF("%s/%s/%ld: OM_SET obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		result = DoOmSet(cl, obj, (struct opSet *) msg);
		break;

	case OM_GET:
		d1(KPrintF("%s/%s/%ld: OM_GET obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		{
		struct opGet *opg = (struct opGet *) msg;

		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		switch (opg->opg_AttrID)
			{
		case MUIA_ScalosPrefs_CreateIcons:
			*opg->opg_Storage = inst->ppb_fCreateIcons;
			result = 0;
			break;
		default:
			result = DoSuperMethodA(cl, obj, msg);
			}
		}
		break;

	case MUIM_ScalosPrefs_ParseToolTypes:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_ParseToolTypes obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf("%s/%s/%ld: before ParseToolTypes\n", __FILE__, __FUNC__, __LINE__));
		ParseToolTypes(inst, (struct MUIP_ScalosPrefs_ParseToolTypes *) msg);
		d1(kprintf("%s/%s/%ld: after ParseToolTypes\n", __FILE__, __FUNC__, __LINE__));
		break;

	case MUIM_ScalosPrefs_LoadConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_LoadConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		ReadPrefsFile(inst, "ENV:Scalos/Pattern.prefs", FALSE);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_UseConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENV:Scalos/Pattern.prefs");
		break;

	case MUIM_ScalosPrefs_UseConfigIfChanged:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_UseConfigIfChanged obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf("%s/%s/%ld: MUIM_ScalosPrefs_UseConfigIfChanged  changed=%ld\n", __FILE__, __FUNC__, __LINE__, inst->ppb_Changed));
		if (inst->ppb_Changed)
			{
			WritePrefsFile(inst, "ENV:Scalos/Pattern.prefs");

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_SaveConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_SaveConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENVARC:Scalos/Pattern.prefs");
		WritePrefsFile(inst, "ENV:Scalos/Pattern.prefs");

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_SaveConfigIfChanged:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_SaveConfigIfChanged obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf("%s/%s/%ld: MUIM_ScalosPrefs_UseConfigIfChanged  changed=%ld\n", __FILE__, __FUNC__, __LINE__, inst->ppb_Changed));
		if (inst->ppb_Changed)
			{
			WritePrefsFile(inst, "ENVARC:Scalos/Pattern.prefs");
			WritePrefsFile(inst, "ENV:Scalos/Pattern.prefs");

			d1(kprintf("%s/%s/%ld: \n"));
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_RestoreConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_RestoreConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_Restore], 0);
		break;

	case MUIM_ScalosPrefs_ResetToDefaults:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_ResetToDefaults obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ResetToDefaults], 0);
		break;

	case MUIM_ScalosPrefs_LastSavedConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_LastSavedConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_LastSaved], 0);
		break;

	case MUIM_ScalosPrefs_OpenConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_OpenConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_Open], 0);
		break;

	case MUIM_ScalosPrefs_SaveConfigAs:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_SaveConfigAs obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SaveAs], 0);
		break;

	case MUIM_ScalosPrefs_About:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_About obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_About], 0);
		break;

	case MUIM_ScalosPrefs_LoadNamedConfig:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_LoadNamedConfig obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		{
		struct MUIP_ScalosPrefs_LoadNamedConfig *lnc = (struct MUIP_ScalosPrefs_LoadNamedConfig *) msg;

		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		ReadPrefsFile(inst, lnc->ConfigFileName, FALSE);
		}
		break;

	case MUIM_ScalosPrefs_CreateSubWindows:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_CreateSubWindows obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		inst = (struct PatternPrefsInst *) INST_DATA(cl, obj);
		CreateSubWindows(inst);
		result = (ULONG) inst->ppb_SubWindows;
		break;

	case MUIM_ScalosPrefs_GetListOfMCCs:
		d1(KPrintF("%s/%s/%ld: MUIM_ScalosPrefs_GetListOfMCCs obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		result = (ULONG) RequiredMccList;
		break;

	default:
		d1(KPrintF("%s/%s/%ld: obj=%08lx MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, obj, msg->MethodID));
		result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static Object *CreatePrefsGroup(struct PatternPrefsInst *inst)
{
	CONST_STRPTR MainListFormat;

	d1(KPrintF(__FUNC__ "/%ld:  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, inst));

	inst->ppb_Objects[OBJNDX_ContextMenu] = MUI_MakeObject(MUIO_MenustripNM, ContextMenus, 0);

	if (NULL == inst->ppb_Objects[OBJNDX_ContextMenu])
		return NULL;

	inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap] = CreateEmptyThumbnailImage(inst);

	if (inst->ppb_UseThumbNails)
		MainListFormat = LISTFORMAT_THUMBNAILS;
	else
		MainListFormat = LISTFORMAT_STANDARD;

	inst->ppb_Objects[OBJNDX_Menu_ShowPreview] = (Object *) DoMethod(inst->ppb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_ShowPreview);
	inst->ppb_Objects[OBJNDX_Menu_AutoPreview] = (Object *) DoMethod(inst->ppb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_AutoPreview);
	inst->ppb_Objects[OBJNDX_Menu_Thumbnails] = (Object *) DoMethod(inst->ppb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Thumbnails);
	inst->ppb_Objects[OBJNDX_Menu_ReloadThumbnails] = (Object *) DoMethod(inst->ppb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_ReloadThumbnails);

	inst->ppb_Objects[OBJNDX_Group_Main] = VGroup,
		MUIA_Background, MUII_PageBack,

		Child,	RegisterObject,
			MUIA_Register_Titles, RegisterTitleStrings,

// Register page "Pattern List"
			Child,	VGroup,
				GroupFrame,
				Child,	inst->ppb_Objects[OBJNDX_Group_MainList] = HGroup,
					Child,	inst->ppb_Objects[OBJNDX_MainListView] = NListviewObject,
						MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
						MUIA_Listview_Input, TRUE,
						MUIA_CycleChain, TRUE,
						MUIA_Listview_List, inst->ppb_Objects[OBJNDX_MainList] = myNListObject,
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NList_Format, MainListFormat,
							MUIA_NList_PrivateData, inst,
							MUIA_NList_TitleSeparator, TRUE,
							MUIA_NList_Title, TRUE,
							MUIA_NList_DisplayHook2, &inst->ppb_Hooks[HOOKNDX_ListDisplay],
							MUIA_NList_ConstructHook2, &inst->ppb_Hooks[HOOKNDX_ListConstruct],
							MUIA_NList_DestructHook2, &inst->ppb_Hooks[HOOKNDX_ListDestruct],
							MUIA_NList_CompareHook2, &inst->ppb_Hooks[HOOKNDX_ListCompare],
							MUIA_NList_PoolPuddleSize, sizeof(struct PatternListEntry) * 128,
							MUIA_NList_PoolThreshSize, sizeof(struct PatternListEntry),
							MUIA_NList_DragSortable, TRUE,
							MUIA_NList_ShowDropMarks, TRUE,
							MUIA_NList_AutoVisible, TRUE,
							MUIA_NList_SortType, 0,
							MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
							MUIA_NList_EntryValueDependent, TRUE,
							MUIA_NList_DefaultObjectOnClick, TRUE,
							MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
							End, //NListObject
						MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_FullAuto,
						End, //NListviewObject

					Child, inst->ppb_Objects[OBJNDX_Balance] = BalanceObject,
						MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails,
						End, //BalanceObject

					Child, inst->ppb_Objects[OBJNDX_Group_Preview] = VGroupV,
						Child, HVSpace,
						Child, inst->ppb_Objects[OBJNDX_PreviewImage] = DataTypesImageObject,
							MUIA_ScaDtpic_Name, (ULONG) "",
							MUIA_ScaDtpic_Tiled, FALSE,
							MUIA_UserData, inst,
							ButtonFrame,
							MUIA_Background, MUII_ButtonBack,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							End, //DataTypesMCC
						Child, HVSpace,
						MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, GetLocString(MSGID_PREVIEWIMAGEBUBBLE),
						MUIA_Weight, inst->ppb_PreviewWeight,
						End, //VGroupV
					End, // HGroup

				Child, HGroup,
					Child, inst->ppb_Objects[OBJNDX_Lamp_Changed] = LampObject,
						MUIA_Lamp_Type, MUIV_Lamp_Type_Huge, 
						MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_LAMP_CHANGED),
						End, //LampObject

					Child,	ColGroup(2),
						Child, inst->ppb_Objects[OBJNDX_NewButton] = KeyButtonHelp(GetLocString(MSGID_NEWNAME), 'w', GetLocString(MSGID_SHORTHELP_NEWBUTTON)),
						Child, inst->ppb_Objects[OBJNDX_DelButton] = KeyButtonHelp(GetLocString(MSGID_DELNAME), 'd', GetLocString(MSGID_SHORTHELP_DELBUTTON)),
						End, //ColGroup
					End, //HGroup

				Child, VGroup,
					MUIA_Background, MUII_GroupBack,
					GroupFrame,

					Child,	inst->ppb_Objects[OBJNDX_Group_String] = ColGroup(5),
						MUIA_Disabled, TRUE,
						Child,	inst->ppb_Objects[OBJNDX_NumButton] = NumericbuttonObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 300,
							MUIA_ShortHelp, GetLocString(MSGID_NUMBUTTONBUBBLE),
							End,
						Child, inst->ppb_Objects[OBJNDX_Popasl] = PopaslObject,
							MUIA_CycleChain, TRUE,
							MUIA_Popasl_Type, ASL_FileRequest,
							MUIA_Popstring_String, inst->ppb_Objects[OBJNDX_STR_PopAsl] = MUI_MakeObject(MUIO_String, NULL, MAX_FILENAME),
							MUIA_Popstring_Button, PopButton(MUII_PopFile),
							End,
						Child, inst->ppb_Objects[OBJNDX_CyclePatternType] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, patternmode,
							MUIA_Weight, 5,
							MUIA_ShortHelp, GetLocString(MSGID_CYCLEGADBUBBLE),
							End,
						Child, Label(GetLocString(MSGID_REMAPNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkGad] = CheckMarkHelp(FALSE, MSGID_CHECKMARKGADBUBBLE),
						End,
					Child,	inst->ppb_Objects[OBJNDX_Group_Slider] = HGroup,
						MUIA_Disabled, TRUE,
						MUIA_CycleChain, TRUE,

						Child, HGroup,
							MUIA_ShowMe, (NULL != GuiGFXBase),
							Child, Label(GetLocString(MSGID_ENHANCNAME)),
							Child, inst->ppb_Objects[OBJNDX_CheckmarkEnh] = CheckMarkHelp(FALSE, MSGID_CHECKMARKENHBUBBLE),
						End, //HGroup

						Child, Label(GetLocString(MSGID_PRECNAME)),
						Child, inst->ppb_Objects[OBJNDX_PrecSlider] = PrecSliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 0,
							MUIA_Numeric_Max, 3,
							MUIA_Numeric_Value, 1,
							MUIA_ShortHelp, GetLocString(MSGID_PRECSLIDERBUBBLE),
							End,
						End,

					End, //VGroup

				Child,	inst->ppb_Objects[OBJNDX_Group_Enhanced] = VGroup,
					MUIA_Background, MUII_GroupBack,
					GroupFrame,
					MUIA_FrameTitle, GetLocString(MSGID_ENHANCOPTSNAME),
					MUIA_ShowMe, (NULL != GuiGFXBase),
					MUIA_Disabled, TRUE,

					Child,	ColGroup(4),
						Child, Label(GetLocString(MSGID_AUTODITHERNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkAutoDither] = CheckMarkHelp(FALSE, MSGID_CHECKMARKAUTODITHERBUBBLE),
						Child, Label(GetLocString(MSGID_NUMCOLSNAME)),
						Child, inst->ppb_Objects[OBJNDX_SliderColours] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 100,
							MUIA_Numeric_Format, "%ld %%",
							MUIA_Numeric_Value, 10,
							MUIA_ShortHelp, GetLocString(MSGID_COLORSSLIDERBUBBLE),
							End,
						End, //ColGroup
					Child,	HGroup,
						Child, Label(GetLocString(MSGID_DITHERMODENAME)),
						Child, inst->ppb_Objects[OBJNDX_CycleDitherMode] = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_Cycle_Entries, dithermodecont,
							MUIA_ShortHelp, GetLocString(MSGID_CYCLEDITHERMODEBUBBLE),
							End,
						Child, Label(GetLocString(MSGID_DITHERAMOUNTNAME)),
						Child, inst->ppb_Objects[OBJNDX_SliderDitherAmount] = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Slider_Min, 1,
							MUIA_Slider_Max, 20,
							MUIA_ShortHelp, GetLocString(MSGID_SLIDERDITHERAMOUNTBUBBLE),
							End,
						End, //HGroup

					End, //VGroup
				End, //VGroup

// Register page "Background Colours"
			Child,	VGroup,
				Child, VGroup,
					Child, HGroup,
						Child, Label(GetLocString(MSGID_LABEL_CYCLEBACKGROUND)),
						Child, inst->ppb_Objects[OBJNDX_CycleBackgroundType] = CycleObject,
							MUIA_Cycle_Entries, CycleBackgroundModeStrings,
							MUIA_CycleChain, TRUE,
							MUIA_Disabled, TRUE,
							End, //CycleObject
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_CYCLEBACKGROUND),
						End, //HGroup

					Child, inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1] = ColoradjustObject,
						MUIA_CycleChain, TRUE,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_BACKGROUNDCOLOR1),
						End, //ColoradjustObject

					Child, BalanceObject,
						End, //BalanceObject

					Child, inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2] = ColoradjustObject,
						MUIA_CycleChain, TRUE,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_BACKGROUNDCOLOR2),
						End, //ColoradjustObject

					End, //VGroup
				End, //VGroup


// Register page "Defaults"
			Child,	VGroup,
				Child, HVSpace,

				Child, VGroup,
					MUIA_Background, MUII_GroupBack,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_MISCELLANEOUS),

					Child, HVSpace,

					Child,	ColGroup(7),
						Child, HVSpace,
						Child, Label(GetLocString(MSGID_ASYNCNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkAsync] = CheckMarkHelp(FALSE, MSGID_CHECKMARKASYNCBUBBLE),
						Child, HVSpace,
						Child, Label(GetLocString(MSGID_FRIENDNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkFriend] = CheckMarkHelp(FALSE, MSGID_CHECKMARKFRIENDBUBBLE),
						Child, HVSpace,
						Child, HVSpace,
						Child, Label(GetLocString(MSGID_RELAYOUTNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkRelayout] = CheckMarkHelp(FALSE, MSGID_CHECKMARKRELAYOUTBUBBLE),
						Child, HVSpace,
						Child, Label(GetLocString(MSGID_RANDOMNAME)),
						Child, inst->ppb_Objects[OBJNDX_CheckmarkRandom] = CheckMarkHelp(FALSE, MSGID_CHECKMARKRANDOMBUBBLE),
						Child, HVSpace,
					End,

					Child,	ColGroup(2),
						Child, Label(GetLocString(MSGID_TASKPRINAME)),
						Child, inst->ppb_Objects[OBJNDX_SliderTaskPri] = PatternSliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Numeric_Min, -128,
							MUIA_Numeric_Max, 127,
							MUIA_Numeric_Value, -3,
							MUIA_Disabled, TRUE,
							MUIA_ShortHelp, GetLocString(MSGID_SLIDERTASKPRIBUBBLE),
							End,
					End, //ColGroup

					Child, HVSpace,

					End, //VGroup

				Child, VGroup,
					MUIA_Background, MUII_GroupBack,
					GroupFrame,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_PATTERNNUMBERS),

					Child, HVSpace,

					Child, HGroup,
						Child,	inst->ppb_Objects[OBJNDX_Group_PatternSliders] = ColGroup(2),
							Child, VSpace(0),
							Child, VSpace(0),

							Child, Label(GetLocString(MSGID_DEFWBNAME)),
							Child, inst->ppb_Objects[OBJNDX_SliderWB] = PatternSliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 300,
								MUIA_Numeric_Value, 0,
								MUIA_ShortHelp, GetLocString(MSGID_SLIDERWBBUBBLE),
								End,

							Child, VSpace(0),
							Child, VSpace(0),

							Child, Label(GetLocString(MSGID_DEFSCREENNAME)),
							Child, inst->ppb_Objects[OBJNDX_SliderScreen] = PatternSliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 300,
								MUIA_Numeric_Value, 0,
								MUIA_ShortHelp, GetLocString(MSGID_SLIDERSCREENBUBBLE),
								End,

							Child, VSpace(0),
							Child, VSpace(0),

							Child, Label(GetLocString(MSGID_DEFWINNAME)),
							Child, inst->ppb_Objects[OBJNDX_SliderWin] = PatternSliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 300,
								MUIA_Numeric_Value, 0,
								MUIA_ShortHelp, GetLocString(MSGID_SLIDERWINBUBBLE),
								End,

							Child, VSpace(0),
							Child, VSpace(0),

							Child, Label(GetLocString(MSGID_TEXTNAME)),
							Child,	inst->ppb_Objects[OBJNDX_SliderText] = PatternSliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Numeric_Min, 0,
								MUIA_Numeric_Max, 300,
								MUIA_Numeric_Value, 0,
								MUIA_ShortHelp, GetLocString(MSGID_SLIDERTEXTBUBBLE),
								End,

							Child, VSpace(0),
							Child, VSpace(0),

							End, //ColGroup

						Child, inst->ppb_Objects[OBJNDX_Group_PatternPreviews] = VGroup,
							MUIA_ShowMe, inst->ppb_UseThumbNails,

							Child,  inst->ppb_Objects[OBJNDX_Image_DesktopPattern] = BackfillObject,
								MUIA_UserData, inst,
								ImageButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								ImageButtonFrame,
								MUIA_InputMode, MUIV_InputMode_None,
								BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap],
								End, //BackfillClass
							Child,  inst->ppb_Objects[OBJNDX_Image_ScreenPattern] = BackfillObject,
								MUIA_UserData, inst,
								ImageButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								ImageButtonFrame,
								MUIA_InputMode, MUIV_InputMode_None,
								BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap],
								End, //BackfillClass
							Child,  inst->ppb_Objects[OBJNDX_Image_IconWindowPattern] = BackfillObject,
								MUIA_UserData, inst,
								ImageButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								ImageButtonFrame,
								MUIA_InputMode, MUIV_InputMode_None,
								BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap],
								End, //BackfillClass
							Child,  inst->ppb_Objects[OBJNDX_Image_TextWindowPattern] = BackfillObject,
								MUIA_UserData, inst,
								ImageButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								ImageButtonFrame,
								MUIA_InputMode, MUIV_InputMode_None,
								BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap],
								End, //BackfillClass

							End, //VGroup
						End, //HGroup

					Child, HVSpace,

					End, //VGroup

				Child, HVSpace,

				End,
			End,

			MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
		End;

	if (NULL == inst->ppb_Objects[OBJNDX_Group_Main])
		return NULL;

	d1(KPrintF("%s/%s/%ld:  fPreview=%ld  ppb_UseThumbNails=%ld\n", __FILE__, __FUNC__, __LINE__, inst->ppb_fPreview, inst->ppb_UseThumbNails));

	if (inst->ppb_UseThumbNails && inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap])
		{
		d1(kprintf("%s/%s/%ld:  BitMapObj=%08lx  Nr=%lu\n", \
			__FILE__, __FUNC__, __LINE__, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap], 1 + inst->ppb_ThumbnailImageNumber));

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_UseImage, 
			inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap], inst->ppb_ThumbnailImageNumber++, 0);
		}

	set(inst->ppb_Objects[OBJNDX_Menu_Thumbnails],  MUIA_Menuitem_Checked, inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ShowPreview], MUIA_Menuitem_Checked, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ShowPreview], MUIA_Menuitem_Enabled, !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_AutoPreview], MUIA_Menuitem_Checked, inst->ppb_fAutoPreview);
	set(inst->ppb_Objects[OBJNDX_Menu_AutoPreview], MUIA_Menuitem_Enabled, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ReloadThumbnails], MUIA_Menuitem_Enabled, inst->ppb_UseThumbNails);

	set(inst->ppb_Objects[OBJNDX_DelButton], MUIA_Disabled, TRUE);

	set(inst->ppb_Objects[OBJNDX_Balance], MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails);

	DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Group_Main], 3, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ContextMenuTrigger], MUIV_TriggerValue );

	DoMethod(inst->ppb_Objects[OBJNDX_PreviewImage], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->ppb_Objects[OBJNDX_PreviewImage], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_PreviewSelect]);

	DoMethod(inst->ppb_Objects[OBJNDX_STR_PopAsl], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry]);

	// setup sorting hooks for plugin list
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

        /* Call the AppMsgHook when an icon is dropped on a listview */
        DoMethod(inst->ppb_Objects[OBJNDX_MainListView], MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
                 inst->ppb_Objects[OBJNDX_MainListView], 3, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_AppMsgInMainList], MUIV_TriggerValue);

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListSelectEntry]);

	// Call hook whenever background type is modified
	DoMethod(inst->ppb_Objects[OBJNDX_CycleBackgroundType], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	// Call hook whenever back color 1 is modified
	DoMethod(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	// Call hook whenever back color 2 is modified
	DoMethod(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	// Call hook whenever pattern type is modified
	DoMethod(inst->ppb_Objects[OBJNDX_CyclePatternType], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkGad], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkGad], MUIM_Notify, MUIA_Selected, TRUE, 
		inst->ppb_Objects[OBJNDX_CheckmarkEnh], 3, MUIM_Set, MUIA_Selected, FALSE);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkEnh], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkEnh], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Enhanced], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkAsync], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_SliderTaskPri], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

	DoMethod(inst->ppb_Objects[OBJNDX_DelButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_RemEntry]);

	DoMethod(inst->ppb_Objects[OBJNDX_NewButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_AddNewEntry]);

	DoMethod(inst->ppb_Objects[OBJNDX_NumButton], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry3]);

	DoMethod(inst->ppb_Objects[OBJNDX_PrecSlider], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderColours], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderDitherAmount], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_CycleDitherMode], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkAutoDither], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ListChangeEntry2]);


	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_Notify, MUIA_NList_InsertPosition, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_MainList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderWB], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderScreen], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderWin], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderText], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkAsync], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkFriend], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkRelayout], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_CheckmarkRandom], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	DoMethod(inst->ppb_Objects[OBJNDX_SliderTaskPri], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->ppb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SettingsChanged]);

	// Changing the default pattern numbers for desktop, screen, icon windows,
	// and text windows updates the preview images
	DoMethod(inst->ppb_Objects[OBJNDX_SliderWB], MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Image_DesktopPattern], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_DesktopPatternChanged]);
	DoMethod(inst->ppb_Objects[OBJNDX_SliderScreen], MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Image_ScreenPattern], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ScreenPatternChanged]);
	DoMethod(inst->ppb_Objects[OBJNDX_SliderWin], MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Image_IconWindowPattern], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_IconWindowPatternChanged]);
	DoMethod(inst->ppb_Objects[OBJNDX_SliderText], MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Image_TextWindowPattern], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_TextWindowPatternChanged]);

	return inst->ppb_Objects[OBJNDX_Group_Main];
}


static void CreateSubWindows(struct PatternPrefsInst *inst)
{
	inst->ppb_SubWindows[0] = inst->ppb_Objects[OBJNDX_WIN_Message] = WindowObject,
//		MUIA_Window_Borderless, TRUE,
		MUIA_Window_Activate, FALSE,
		MUIA_Window_NoMenus, TRUE,
		MUIA_Window_Title, GetLocString(MSGID_WINDOW_STARTUP),
		WindowContents, VGroup,
			Child, inst->ppb_Objects[OBJNDX_MsgGauge] = GaugeObject,
				MUIA_Gauge_Horiz, TRUE,
				GaugeFrame,
				MUIA_Gauge_InfoText, " ",
				End,
			Child, inst->ppb_Objects[OBJNDX_MsgText1] = TextObject,
				MUIA_Text_PreParse, MUIX_C,
				MUIA_Text_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
				End,
			Child, inst->ppb_Objects[OBJNDX_MsgText2] = TextObject,
				MUIA_Text_PreParse, MUIX_C,
				MUIA_Text_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
				End,
			End,
		End;

	inst->ppb_SubWindows[1] = NULL;
}


static ULONG DoOmSet(Class *cl, Object *o, struct opSet *ops)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) INST_DATA(cl, o);
	struct TagItem *TagList = ops->ops_AttrList;
	struct TagItem *ti;
	ULONG result = 0;

	while ((ti = NextTagItem(&TagList)))
		{
		d1(kprintf(__FILE__ "/%s/%ld: ti=%08lx  Tag=%08lx  Data=%08lx\n", __FUNC__, __FILE__, __FUNC__, __LINE__, ti, ti->ti_Tag, ti->ti_Data));

		switch (ti->ti_Tag)
			{
		case MUIA_ScalosPrefs_CreateIcons:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_CreateIcons obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_fCreateIcons = ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_ProgramName:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_ProgramName obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_ProgramName = (CONST_STRPTR) ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_MainWindow:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_MainWindow obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_Objects[OBJNDX_WIN_Main] = (APTR) ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_Application:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_Application obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_Objects[OBJNDX_APP_Main] = (APTR) ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_PatternPrefs_Preview:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_PatternPrefs_Preview obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_fPreview = ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_PatternPrefs_AutoPreview:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_PatternPrefs_AutoPreview obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_fAutoPreview = ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_PatternPrefs_PreviewWeight:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_PatternPrefs_PreviewWeight obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_PreviewWeight = ti->ti_Data;
			break;
		case MUIA_ScalosPrefs_PatternPrefs_Thumbnails:
			d1(KPrintF("%s/%s/%ld: MUIA_ScalosPrefs_PatternPrefs_Thumbnails obj=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			inst->ppb_UseThumbNails = ti->ti_Data;
			set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Format,
				(ULONG) (inst->ppb_UseThumbNails ? LISTFORMAT_THUMBNAILS : LISTFORMAT_STANDARD));
			break;
		default:
			d1(KPrintF(__FILE__ "/%s/%ld: unknown tag o=%08lx  Tag=%08lx\n", __FUNC__, __FILE__, __FUNC__, __LINE__, o, ti->ti_Tag));
			result = DoSuperMethodA(cl, o, (Msg) ops);
			break;
			}
		}

	return result;
}


static Object *CreatePrefsImage(void)
{
#include "PatternPrefsImage.h"
	Object	*img;

	// First try to load datatypes image from THEME: tree
	img = DataTypesImageObject,
		MUIA_ScaDtpic_Name, (ULONG) "THEME:prefs/plugins/pattern",
		MUIA_ScaDtpic_FailIfUnavailable, TRUE,
		End; //DataTypesMCCObject

	if (NULL == img)
		img = IMG(PatternPrefs, PATTERNPREFS);	// use built-in fallback image

	return img;
}


static void InitHooks(struct PatternPrefsInst *inst)
{
	ULONG n;

	for (n=0; n<HOOKNDX_MAX; n++)
		{
		inst->ppb_Hooks[n] = PatternPrefsHooks[n];
		inst->ppb_Hooks[n].h_Data = inst;
		}
}


static BOOL OpenLibraries(void)
{
	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;
#endif

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *) IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	MUIMasterBase = OpenLibrary("zune.library", 0);
	if (NULL == MUIMasterBase)
		MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	IMUIMaster = (struct MUIMasterIFace *) GetInterface((struct Library *) MUIMasterBase, "main", 1, NULL);
	if (NULL == IMUIMaster)
		return FALSE;
#endif

	IFFParseBase = OpenLibrary("iffparse.library", 36);
	if (NULL == IFFParseBase)
		return FALSE;
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *) GetInterface((struct Library *) IFFParseBase, "main", 1, NULL);
	if (NULL == IIFFParse)
		return FALSE;
#endif

	IconBase = OpenLibrary("icon.library", 0);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	IIcon = (struct IconIFace *) GetInterface((struct Library *) IconBase, "main", 1, NULL);
	if (NULL == IIcon)
		return FALSE;
#endif

	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
	if (NULL == GfxBase)
		return FALSE;
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *) GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		return FALSE;
#endif

	DataTypesBase = OpenLibrary("datatypes.library", 39);
	if (NULL == DataTypesBase)
		return FALSE;
#ifdef __amigaos4__
	IDataTypes = (struct DataTypesIFace *) GetInterface((struct Library *) DataTypesBase, "main", 1, NULL);
	if (NULL == IDataTypes)
		return FALSE;
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary(UTILITYNAME, 39);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *) GetInterface((struct Library *) UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif

	ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
	if (NULL == ScalosGfxBase)
		return FALSE;
#ifdef __amigaos4__
	IScalosGfx = (struct ScalosGfxIFace *) GetInterface((struct Library *) ScalosGfxBase, "main", 1, NULL);
	if (NULL == IScalosGfx)
		return FALSE;
#endif

	TimerIORequest = (T_TIMEREQUEST *) CreateIORequest(CreateMsgPort(), sizeof(T_TIMEREQUEST));
	if (NULL == TimerIORequest)
		return FALSE;
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, &TimerIORequest->tr_node, 0))
		{
		// OpenDevice failed
		DeleteIORequest(&TimerIORequest->tr_node);
		TimerIORequest = NULL;
		return FALSE;
		}
	TimerBase = (T_TIMERBASE) TimerIORequest->tr_node.io_Device;
	if (NULL == TimerBase)
		return FALSE;
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
	if (NULL == ITimer)
		return FALSE;
#endif /* __amigaos4__ */


	CyberGfxBase = (APTR) OpenLibrary( "cybergraphics.library", 40);
#ifdef __amigaos4__
	if (CyberGfxBase)
		ICyberGfx = (struct CyberGfxIFace *) GetInterface((struct Library *) CyberGfxBase, "main", 1, NULL);
#endif
	// CyberGfxBase may be NULL

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__)  && !defined(__amigaos4__)*/

#ifdef __amigaos4__
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	GuiGFXBase = OpenLibrary("guigfx.library", 1);
#ifdef __amigaos4__
	if (GuiGFXBase)
		IGuiGFX = (struct GuiGFXIFace *) GetInterface((struct Library *) GuiGFXBase, "main", 1, NULL);
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (LocaleBase)
		ILocale = (struct LocaleIFace *) GetInterface((struct Library *) LocaleBase, "main", 1, NULL);
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
	if (TimerIORequest)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ITimer);
#endif
		CloseDevice(&TimerIORequest->tr_node);
		DeleteIORequest(&TimerIORequest->tr_node);
		TimerIORequest = NULL;
		}
#ifdef __amigaos4__
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
	if (ICyberGfx)
		{
		DropInterface((struct Interface *)ICyberGfx);
		ICyberGfx = NULL;
		}
#endif
	if (CyberGfxBase)
		{
		CloseLibrary(CyberGfxBase);
		CyberGfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IScalosGfx)
		{
		DropInterface((struct Interface *)IScalosGfx);
		IScalosGfx = NULL;
		}
#endif
	if (ScalosGfxBase)
		{
		CloseLibrary((struct Library *) ScalosGfxBase);
		ScalosGfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IGuiGFX)
		{
		DropInterface((struct Interface *)IGuiGFX);
		IGuiGFX = NULL;
		}
#endif
	if (GuiGFXBase)
		{
		CloseLibrary(GuiGFXBase);
		GuiGFXBase = NULL;
		}
#ifdef __amigaos4__
	if (ILocale)
		{
		DropInterface((struct Interface *)ILocale);
		ILocale = NULL;
		}
#endif
	if (LocaleBase)
		{
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
		}
#ifdef __amigaos4__
	if (IDataTypes)
		{
		DropInterface((struct Interface *)IDataTypes);
		IDataTypes = NULL;
		}
#endif
	if (DataTypesBase)
		{
		CloseLibrary(DataTypesBase);
		DataTypesBase = NULL;
		}
#ifdef __amigaos4__
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
#endif
	if (GfxBase)
		{
		CloseLibrary((struct Library *) GfxBase);
		GfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif
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
#endif
	if (MUIMasterBase)
		{
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct ScalosPattern_LocaleInfo li;

	li.li_Catalog = PatternPrefsCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetScalosPatternString(&li, MsgId);
}


static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

static void TranslateNewMenu(struct NewMenu *nm)
{
	while (nm && NM_END != nm->nm_Type)
		{
		if (NM_BARLABEL != nm->nm_Label)
			nm->nm_Label = GetLocString((ULONG) nm->nm_Label);

		if (nm->nm_CommKey)
			nm->nm_CommKey = GetLocString((ULONG) nm->nm_CommKey);

		nm++;
		}
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ListConstructHookFunc(struct Hook *hook, Object *obj, struct NList_ConstructMessage *msg)
{
	struct PatternListEntry *scp = AllocPooled(msg->pool, sizeof(struct PatternListEntry));

	if (scp)
		{
		CONST_STRPTR FileName = (CONST_STRPTR) msg->entry;

		stccpy(scp->ple_PatternPrefs.scxp_Name, FileName, sizeof(scp->ple_PatternPrefs.scxp_Name));

		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number = 1;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType = SCP_RenderType_Tiled;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags = SCPF_ENHANCED | SCPF_AUTODITHER;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_NumColors = 100;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherMode = DITHERMODE_FS;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount = 1;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Precision = PRECISION_IMAGE;
		scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Type = SCP_BgType_Picture;

		scp->ple_ThumbnailImageNr = 0;
		scp->ple_SAC = NULL;
		scp->ple_MUI_ImageObject = NULL;
		scp->ple_BitMapArray = NULL;
		}

	return scp;
}

static SAVEDS(void) INTERRUPT ListDestructHookFunc(struct Hook *hook, Object *obj, struct NList_DestructMessage *msg)
{
	struct PatternListEntry *scp = (struct PatternListEntry *) msg->entry;
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (scp->ple_BitMapArray)
		{
		free(scp->ple_BitMapArray);
		scp->ple_BitMapArray = NULL;
		}
	if (scp->ple_MUI_ImageObject)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_MainList],
			MUIM_NList_UseImage, NULL, scp->ple_ThumbnailImageNr, 0);

		MUI_DisposeObject(scp->ple_MUI_ImageObject);
		scp->ple_MUI_ImageObject = NULL;
		}
	if (scp->ple_SAC)
		{
		ScalosGfxFreeSAC(scp->ple_SAC);
		scp->ple_SAC = NULL;
		}

	FreePooled(msg->pool, scp, sizeof(struct PatternListEntry));
}


static SAVEDS(ULONG) INTERRUPT ListDisplayHookFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG n;

	if (ndm->entry)
		{
		struct PatternListEntry *scp = (struct PatternListEntry *) ndm->entry;
		static char Buffer[MAX_FILENAME];
		STRPTR lp = Buffer;
		ULONG DesktopPattern = 0;
		ULONG ScreenPattern = 0;
		ULONG IconWindowPattern = 0;
		ULONG TextWindowPattern = 0;

		n = 0;

		// 1st column -- pattern number
		ndm->preparses[n++] = "\033r";

		// 2nd column -- select status
		ndm->preparses[n++] = "\033r";

		// 3rd column -- thumbnail
		if (inst->ppb_UseThumbNails)
			{
			ndm->preparses[n++] = "\033c";
			}

		// last column -- file name
		ndm->preparses[n] = "\033l";

		// 1st column -- pattern number
		n = 0;
		sprintf(lp, "%d", scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number);
		ndm->strings[n++] = lp;

		// 2nd column -- select status
		get(inst->ppb_Objects[OBJNDX_SliderWB], MUIA_Numeric_Value, &DesktopPattern);
		get(inst->ppb_Objects[OBJNDX_SliderScreen], MUIA_Numeric_Value, &ScreenPattern);
		get(inst->ppb_Objects[OBJNDX_SliderWin], MUIA_Numeric_Value, &IconWindowPattern);
		get(inst->ppb_Objects[OBJNDX_SliderText], MUIA_Numeric_Value, &TextWindowPattern);

		if (DesktopPattern == scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number)
			ndm->strings[n++] = GetLocString(MSGID_PATTERNTYPE_DESKTOP);
		else if (ScreenPattern == scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number)
			ndm->strings[n++] = GetLocString(MSGID_PATTERNTYPE_SCREEN);
		else if (IconWindowPattern == scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number)
			ndm->strings[n++] = GetLocString(MSGID_PATTERNTYPE_ICONWINDOW);
		else if (TextWindowPattern == scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number)
			ndm->strings[n++] = GetLocString(MSGID_PATTERNTYPE_TEXTWINDOW);
		else
			ndm->strings[n++] = GetLocString(MSGID_PATTERNTYPE_NONE);

		// 3rd column -- thumbnail
		if (inst->ppb_UseThumbNails)
			{
			lp += 1 + strlen(lp);
			sprintf(lp, "\33o[%ld]", (long)scp->ple_ThumbnailImageNr);
			ndm->strings[n++] = lp;

			d1(kprintf("%s/%s/%ld: lp=<%s>  ord=%ld  scp=%08lx  ImageObj=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, lp, ndm->entry_pos, scp, scp->ple_MUI_ImageObject));
			}

		// last column -- file name

		lp += 1 + strlen(lp);
		strcpy(lp, scp->ple_PatternPrefs.scxp_Name);
		ndm->strings[n] = lp;
		}
	else
		{
		// display titles

		n = 0;

		// 1st column -- pattern number
		ndm->preparses[n++] = "\033c";

		// 2nd column -- select status
		ndm->preparses[n++] = "\033c";

		// 3rd column -- thumbnail
		if (inst->ppb_UseThumbNails)
			{
			ndm->preparses[n++] = "\033c";
			}

		ndm->preparses[n] = "\033c";

		n = 0;

		// 1st column -- pattern number
		ndm->strings[n++] = GetLocString(MSGID_COLUMNTITLE_1);

		// 2nd column -- select status
		ndm->strings[n++] = GetLocString(MSGID_COLUMNTITLE_4);

		// 3rd column -- thumbnail
		if (inst->ppb_UseThumbNails)
			{
			ndm->strings[n++] = GetLocString(MSGID_COLUMNTITLE_3);
			}

		// last column -- file name
		ndm->strings[n] = GetLocString(MSGID_COLUMNTITLE_2);
		}

	return 0;
}


static SAVEDS(LONG) INTERRUPT ListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	const struct PatternListEntry *scp1 = (const struct PatternListEntry *) ncm->entry1;
	const struct PatternListEntry *scp2 = (const struct PatternListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by number
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = scp2->ple_PatternPrefs.scxp_PatternPrefs.scp_Number - scp1->ple_PatternPrefs.scxp_PatternPrefs.scp_Number;
			else
				Result = scp1->ple_PatternPrefs.scxp_PatternPrefs.scp_Number - scp2->ple_PatternPrefs.scxp_PatternPrefs.scp_Number;
			break;
		case 2:
			if (!inst->ppb_UseThumbNails)
				{
				if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
					Result = Stricmp(scp2->ple_PatternPrefs.scxp_Name, scp1->ple_PatternPrefs.scxp_Name);
				else
					Result = Stricmp(scp1->ple_PatternPrefs.scxp_Name, scp2->ple_PatternPrefs.scxp_Name);
				}
			break;
		case 3:		// sort by filename
			if (inst->ppb_UseThumbNails)
				{
				if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
					Result = Stricmp(scp2->ple_PatternPrefs.scxp_Name, scp1->ple_PatternPrefs.scxp_Name);
				else
					Result = Stricmp(scp1->ple_PatternPrefs.scxp_Name, scp2->ple_PatternPrefs.scxp_Name);
				}
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by number
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = scp1->ple_PatternPrefs.scxp_PatternPrefs.scp_Number - scp2->ple_PatternPrefs.scxp_PatternPrefs.scp_Number;
				else
					Result = scp2->ple_PatternPrefs.scxp_PatternPrefs.scp_Number - scp1->ple_PatternPrefs.scxp_PatternPrefs.scp_Number;
				break;
			case 2:		// sort by filename
				if (!inst->ppb_UseThumbNails)
					{
					if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
						Result = Stricmp(scp2->ple_PatternPrefs.scxp_Name, scp1->ple_PatternPrefs.scxp_Name);
					else
						Result = Stricmp(scp1->ple_PatternPrefs.scxp_Name, scp2->ple_PatternPrefs.scxp_Name);
					}
				break;
			case 3:		// sort by filename
				if (inst->ppb_UseThumbNails)
					{
					if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
						Result = Stricmp(scp2->ple_PatternPrefs.scxp_Name, scp1->ple_PatternPrefs.scxp_Name);
					else
						Result = Stricmp(scp1->ple_PatternPrefs.scxp_Name, scp2->ple_PatternPrefs.scxp_Name);
					}
				break;
			default:
				break;
				}
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

DISPATCHER(PatternSlider)
{
	struct MUIP_Numeric_Stringify *mstr = (struct MUIP_Numeric_Stringify *) msg;
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_Numeric_Stringify:
		if (0 == mstr->value)
			Result = (ULONG) GetLocString(MSGID_NONE_PRIORITY);
		else
			Result = DoSuperMethodA(cl, obj, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(PrecSlider)
{
	static ULONG PrecisionStrings[] =
		{
		MSGID_PRECEXACTNAME,
		MSGID_PRECIMAGENAME,
		MSGID_PRECICONNAME,
		MSGID_PRECGUINAME
		};
	struct MUIP_Numeric_Stringify *mstr = (struct MUIP_Numeric_Stringify *) msg;
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_Numeric_Stringify:
		Result = (ULONG) GetLocString(PrecisionStrings[mstr->value]);
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static LONG ReadPrefsFile(struct PatternPrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet)
{
	static const LONG StopChunkList[] =
		{
		ID_PREF, ID_PATT,
		ID_PREF, ID_PTRN,
		ID_PREF, ID_DEFS,
		ID_PREF, ID_TCOL,
		ID_PREF, ID_TBMP
		};
	struct TempRastPort trp;
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;

	InitRastPort(&trp.trp_rp);
	trp.trp_rp.BitMap = NULL;
	trp.trp_rp.Layer = NULL;
	trp.trp_Width = 0;

	inst->ppb_BitMapsRead = 0;
	inst->ppb_ThumbnailImageNumber = 0;
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Clear);
	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);
	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	do	{
		struct PatternListEntry *scp = NULL;

		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open(Filename, MODE_OLDFILE);
		if (0 == iff->iff_Stream)
			{
			Result = IoErr();
			break;
			}

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = StopChunks(iff, StopChunkList, 5);
		if (RETURN_OK != Result)
			break;

		while (1)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				break;

			if (ID_PTRN == cn->cn_ID)
				{
				LONG Actual;
				struct WBPatternPrefs ptrn;

				Actual = ReadChunkBytes(iff, &ptrn, sizeof(ptrn));
				if (sizeof(ptrn) != Actual)
					{
					Result = IoErr();
					break;
					}

				ptrn.wbp_Which = SCA_BE2WORD(ptrn.wbp_Which);
				ptrn.wbp_Flags = SCA_BE2WORD(ptrn.wbp_Flags);
				ptrn.wbp_DataLength = SCA_BE2WORD(ptrn.wbp_DataLength);

				if (ptrn.wbp_Flags & WBPF_PATTERN)
					{
					ULONG nEntries = 0;
					struct PatternListEntry *scp = NULL;

					DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_InsertSingle, "", MUIV_NList_Insert_Bottom);
					get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

					nEntries--;
					DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, nEntries, &scp);

					if (ptrn.wbp_Flags & WBPF_NOREMAP)
						scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags |= SCPF_NOREMAP;
					}
				}
			else if (ID_DEFS == cn->cn_ID)
				{
				LONG Actual;
				struct ScaPatternDefs pDefs;

				Actual = ReadChunkBytes(iff, &pDefs, sizeof(pDefs));
				if (sizeof(pDefs) != Actual)
					{
					Result = IoErr();
					break;
					}

				pDefs.scd_Flags = SCA_BE2WORD(pDefs.scd_Flags);
				pDefs.scd_WorkbenchPattern = SCA_BE2WORD(pDefs.scd_WorkbenchPattern);
				pDefs.scd_ScreenPattern = SCA_BE2WORD(pDefs.scd_ScreenPattern);
				pDefs.scd_WindowPattern = SCA_BE2WORD(pDefs.scd_WindowPattern);
				pDefs.scd_TextModePattern = SCA_BE2WORD(pDefs.scd_TextModePattern);

				set(inst->ppb_Objects[OBJNDX_SliderWB], MUIA_Numeric_Value, (ULONG) pDefs.scd_WorkbenchPattern);
				set(inst->ppb_Objects[OBJNDX_SliderScreen], MUIA_Numeric_Value, (ULONG) pDefs.scd_ScreenPattern);
				set(inst->ppb_Objects[OBJNDX_SliderWin], MUIA_Numeric_Value, (ULONG) pDefs.scd_WindowPattern);

				if (Actual >= offsetof(struct ScaPatternDefs, scd_TextModePattern))
					{
					set(inst->ppb_Objects[OBJNDX_SliderText], MUIA_Numeric_Value, (ULONG) pDefs.scd_TextModePattern);
					set(inst->ppb_Objects[OBJNDX_SliderTaskPri], MUIA_Numeric_Value, (ULONG) pDefs.scd_TaskPriority);
					}

				set(inst->ppb_Objects[OBJNDX_CheckmarkAsync], MUIA_Selected, (pDefs.scd_Flags & SCDF_ASYNCLAYOUT) ? TRUE : FALSE);
				set(inst->ppb_Objects[OBJNDX_CheckmarkFriend], MUIA_Selected, (pDefs.scd_Flags & SCDF_USEFRIENDBM) ? TRUE : FALSE);
				set(inst->ppb_Objects[OBJNDX_CheckmarkRelayout], MUIA_Selected, (pDefs.scd_Flags & SCDF_RELAYOUT) ? TRUE : FALSE);
				set(inst->ppb_Objects[OBJNDX_CheckmarkRandom], MUIA_Selected, (pDefs.scd_Flags & SCDF_RANDOM) ? TRUE : FALSE);
				}
			else if (ID_PATT == cn->cn_ID)
				{
				ULONG nEntries = 0;

				DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_InsertSingle, "", MUIV_NList_Insert_Bottom);
				get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

				nEntries--;
				DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, nEntries, &scp);

				if (scp)
					{
					if (0 == ReadChunkBytes(iff, &scp->ple_PatternPrefs, sizeof(struct ScaExtPatternPrefs)))
						{
						Result = IoErr();
						break;
						}
					}

				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number);
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType);
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags);
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_NumColors = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_NumColors);
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherMode = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherMode);
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount = SCA_BE2WORD(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount);

				}
			else if (ID_TCOL == cn->cn_ID)
				{
				// ID_TCOL must always be immediately preceeded by ID_PATT for the same pattern
				d1(KPrintF("%s/%s/%ld: ID_TCOL scp=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_SAC));
				if (scp)
					{
					if (NULL == scp->ple_SAC)
						{
						// create preliminary SAC with sac_ColorTable
						scp->ple_SAC = ScalosGfxCreateSAC(1, 1, 8, NULL, NULL);
						}
					d1(KPrintF("%s/%s/%ld: scp=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_SAC));
					if (scp->ple_SAC)
						{
						ULONG n;
						LONG Length;
						WORD NumColors;
						UBYTE byteColorTable[3*256];

						Length = sizeof(NumColors);

						if (Length != ReadChunkBytes(iff, &NumColors, Length))
							{
							Result = IoErr();
							break;
							}

						NumColors = SCA_BE2WORD(NumColors);

						d1(KPrintF("%s/%s/%ld: NumColors=%ld\n", __FILE__, __FUNC__, __LINE__, NumColors));
						Length = NumColors * 3;
						if (Length != ReadChunkBytes(iff, byteColorTable, Length))
							{
							Result = IoErr();
							break;
							}

						for (n=0; n < NumColors * 3; n++)
							{
							UBYTE ch = byteColorTable[n];

							scp->ple_SAC->sac_ColorTable[n] = (ch << 24) + (ch << 16) + (ch << 8) + ch;
							d1(KPrintF("%s/%s/%ld: ColorTable[%ld]=%08lx\n", __FILE__, __FUNC__, __LINE__, n, scp->ple_SAC->sac_ColorTable[n]));
							}
						}
					}
				}
			else if (ID_TBMP == cn->cn_ID)
				{
				// ID_TBMP must always be immediately preceeded by ID_PATT and ID_TCOL for the same pattern
				d1(KPrintF("%s/%s/%ld: scp=%08lx  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_SAC));
				if (scp)
					{
					Result = ReadPrefsBitMap(inst, iff, scp, &trp);
					d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					if (RETURN_OK != Result)
						break;
					}
				inst->ppb_BitMapsRead++;
				}
			}

		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (IFFERR_EOF == Result)
			Result = RETURN_OK;
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);

		FreeIFF(iff);
		}

	if (RETURN_OK != Result && !Quiet)
		{
		char Buffer[120];

		Fault(Result, "", Buffer, sizeof(Buffer) - 1);

		// MUI_RequestA()
		MUI_Request(inst->ppb_Objects[OBJNDX_APP_Main], inst->ppb_Objects[OBJNDX_WIN_Main], 0, NULL, 
			GetLocString(MSGID_ABOUTREQOK), 
			GetLocString(MSGID_REQTITLE_READERROR), 
			Filename,
			Buffer);
		}

	if (trp.trp_rp.BitMap)
		FreeBitMap(trp.trp_rp.BitMap);

	if (inst->ppb_UseThumbNails)
		{
		CreateThumbnailImages(inst);
		}

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Redraw, MUIV_NList_Redraw_All);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG ReadPrefsBitMap(struct PatternPrefsInst *inst, struct IFFHandle *iff,
	struct PatternListEntry *scp, struct TempRastPort *trp)
{
	LONG Result = RETURN_OK;
	struct ScalosBitMapAndColor *sac = NULL;

	do	{
		LONG Length;
		struct RastPort rp;
		struct ScaPatternPrefsBitMap spb;

		if (sizeof(spb) != ReadChunkBytes(iff, &spb, sizeof(spb)))
			{
			Result = IoErr();
			break;
			}

		spb.spb_Width = SCA_BE2WORD(spb.spb_Width);
		spb.spb_Height = SCA_BE2WORD(spb.spb_Height);

		d1(KPrintF("%s/%s/%ld: screen BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->ppb_WBScreen->RastPort.BitMap));

		sac = ScalosGfxCreateSAC(spb.spb_Width, spb.spb_Height,
			8, /*inst->ppb_WBScreen->RastPort.BitMap*/ NULL, NULL);
		d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, sac));
		if (NULL == sac)
			break;

		InitRastPort(&rp);
		rp.BitMap = sac->sac_BitMap;

		if (NULL == trp->trp_rp.BitMap || trp->trp_Width != TEMPRP_WIDTH(spb.spb_Width))
			{
			if (trp->trp_rp.BitMap)
				FreeBitMap(trp->trp_rp.BitMap);

			trp->trp_Width = TEMPRP_WIDTH(spb.spb_Width);
			trp->trp_rp.BitMap = AllocBitMap(trp->trp_Width, 1, 8, BMF_STANDARD, NULL);
			if (NULL == trp->trp_rp.BitMap)
				{
				Result = ERROR_NO_FREE_STORE;
				break;
				}
			}

		Length = PIXELARRAY8_BUFFERSIZE(spb.spb_Width, spb.spb_Height - 1);

		scp->ple_BitMapArray = malloc(Length);
		if (NULL == scp->ple_BitMapArray)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		if (Length != ReadChunkBytes(iff, scp->ple_BitMapArray, Length))
			{
			Result = IoErr();
			break;
			}

		// Blit image data to sac_BitMap
		WritePixelArray8(&rp, 0, 0,
			spb.spb_Width - 1, spb.spb_Height - 1,
			scp->ple_BitMapArray, &trp->trp_rp);

		if (scp->ple_SAC)
			{
			size_t Length = scp->ple_SAC->sac_NumColors * 3 * sizeof(ULONG);

			// copy ColorTable from preliminary SAC
			memcpy(sac->sac_ColorTable, scp->ple_SAC->sac_ColorTable, Length);

			// free preliminary SAC
			ScalosGfxFreeSAC(scp->ple_SAC);
			}

		scp->ple_SAC = sac;
		sac = NULL;
		} while (0);

	if (RETURN_OK != Result)
		{
		if (scp->ple_SAC)
			{
			ScalosGfxFreeSAC(scp->ple_SAC);
			scp->ple_SAC = NULL;
			}
		}
	if (sac)
		ScalosGfxFreeSAC(sac);

	return Result;
}

//----------------------------------------------------------------------------

static LONG WritePrefsFile(struct PatternPrefsInst *inst, CONST_STRPTR Filename)
{
	struct TempRastPort trp;
	struct IFFHandle *iff;
	LONG Result;
	BOOL IffOpen = FALSE;

	d1(kprintf("%s/%s/%ld: inst=%08lx  Filename=<%s>\n", __FILE__, __FUNC__, __LINE__, inst, Filename));

	InitRastPort(&trp.trp_rp);
	trp.trp_rp.BitMap = NULL;
	trp.trp_rp.Layer = NULL;
	trp.trp_Width = 0;

	do	{
		static const struct PrefHeader prefHeader = { 1, 0, 0L };
		struct ScaPatternDefs pDefs;
		ULONG n, Value = 0;

		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open(Filename, MODE_NEWFILE);
		if (0 == iff->iff_Stream)
			{
			// ... try to create missing directories here
			STRPTR FilenameCopy;

			Result = IoErr();
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

			FilenameCopy = AllocVec(1 + strlen(Filename), MEMF_PUBLIC);

			if (FilenameCopy)
				{
				STRPTR lp;

				strcpy(FilenameCopy, Filename);

				lp = PathPart(FilenameCopy);
				if (lp)
					{
					BPTR dirLock;

					*lp = '\0';
					dirLock = CreateDir(FilenameCopy);

					if (dirLock)
						UnLock(dirLock);

					iff->iff_Stream = (IPTR)Open(Filename, MODE_NEWFILE);
					if (0 == iff->iff_Stream)
						Result = IoErr();
					else
						Result = RETURN_OK;
					}

				FreeVec(FilenameCopy);
				}

			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;
			}

		Result = OpenIFF(iff, IFFF_WRITE);
		if (RETURN_OK != Result)
			break;

		IffOpen = TRUE;

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

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_DEFS, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		pDefs.scd_Flags = 0;
		memset(pDefs.scd_Reserved, 0, sizeof(pDefs.scd_Reserved));

		get(inst->ppb_Objects[OBJNDX_CheckmarkAsync], MUIA_Selected, &Value);
		if (Value)
			pDefs.scd_Flags |= SCDF_ASYNCLAYOUT;

		get(inst->ppb_Objects[OBJNDX_CheckmarkFriend], MUIA_Selected, &Value);
		if (Value)
			pDefs.scd_Flags |= SCDF_USEFRIENDBM;

		get(inst->ppb_Objects[OBJNDX_CheckmarkRelayout], MUIA_Selected, &Value);
		if (Value)
			pDefs.scd_Flags |= SCDF_RELAYOUT;

		get(inst->ppb_Objects[OBJNDX_CheckmarkRandom], MUIA_Selected, &Value);
		if (Value)
			pDefs.scd_Flags |= SCDF_RANDOM;

		pDefs.scd_Flags = SCA_WORD2BE(pDefs.scd_Flags);

		get(inst->ppb_Objects[OBJNDX_SliderWB], MUIA_Numeric_Value, &Value);
		pDefs.scd_WorkbenchPattern = SCA_WORD2BE(Value);

		get(inst->ppb_Objects[OBJNDX_SliderScreen], MUIA_Numeric_Value, &Value);
		pDefs.scd_ScreenPattern = SCA_WORD2BE(Value);

		get(inst->ppb_Objects[OBJNDX_SliderWin], MUIA_Numeric_Value, &Value);
		pDefs.scd_WindowPattern = SCA_WORD2BE(Value);

		get(inst->ppb_Objects[OBJNDX_SliderText], MUIA_Numeric_Value, &Value);
		pDefs.scd_TextModePattern = SCA_WORD2BE(Value);

		get(inst->ppb_Objects[OBJNDX_SliderTaskPri], MUIA_Numeric_Value, &Value);
		pDefs.scd_TaskPriority = Value; // BYTE

		if (WriteChunkBytes(iff, &pDefs, sizeof(pDefs)) != sizeof(pDefs))
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		n = 0;
		while (RETURN_OK == Result)
			{
			size_t Length;
			struct PatternListEntry *scp = NULL;
			struct ScaExtPatternPrefs sepp;

			DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_List_GetEntry, n, &scp);
			if (NULL == scp)
				break;

			Result = PushChunk(iff, 0, ID_PATT, IFFSIZE_UNKNOWN);
			if (RETURN_OK != Result)
				break;

			Length = offsetof(struct ScaExtPatternPrefs, scxp_Name) + strlen(scp->ple_PatternPrefs.scxp_Name) + 1;

			sepp = scp->ple_PatternPrefs;

			sepp.scxp_PatternPrefs.scp_Number = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_Number);
			sepp.scxp_PatternPrefs.scp_RenderType = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_RenderType);
			sepp.scxp_PatternPrefs.scp_Flags = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_Flags);
			sepp.scxp_PatternPrefs.scp_NumColors = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_NumColors);
			sepp.scxp_PatternPrefs.scp_DitherMode = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_DitherMode);
			sepp.scxp_PatternPrefs.scp_DitherAmount = SCA_WORD2BE(sepp.scxp_PatternPrefs.scp_DitherAmount);

			if (WriteChunkBytes(iff, &sepp, Length) != Length)
				{
				Result = IoErr();
				break;
				}

			Result = PopChunk(iff);
			if (RETURN_OK != Result)
				break;

			if (scp->ple_SAC)
				{
				if (scp->ple_SAC->sac_NumColors)
					{
					// Write Thumbnail color table
					Result = WritePrefsColorTable(iff, scp);
					if (RETURN_OK != Result)
						break;
					}
				// Write Thumbnail BitMap
				Result = WritePrefsBitMap(iff, scp, &trp);
				if (RETURN_OK != Result)
					break;
				}
			n++;
			}
		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		} while (0);

	if (iff)
		{
		if (IffOpen)
			CloseIFF(iff);

		if (iff->iff_Stream)
			{
			Close((BPTR)iff->iff_Stream);
			iff->iff_Stream = 0;
			}

		FreeIFF(iff);
		}

	if (trp.trp_rp.BitMap)
		FreeBitMap(trp.trp_rp.BitMap);

	if (RETURN_OK != Result)
		{
		char Buffer[120];

		Fault(Result, "", Buffer, sizeof(Buffer) - 1);

		// MUI_RequestA()
		MUI_Request(inst->ppb_Objects[OBJNDX_APP_Main], inst->ppb_Objects[OBJNDX_WIN_Main], 0, NULL, 
			GetLocString(MSGID_ABOUTREQOK), 
			GetLocString(MSGID_REQTITLE_SAVEERROR), 
			Filename,
			Buffer);
		}

	return Result;
}


static LONG WritePrefsColorTable(struct IFFHandle *iff, const struct PatternListEntry *scp)
{
	LONG Result;

	do	{
		UBYTE byteColorTable[3*256];
		LONG n, Length;
		UWORD Value;

		Result = PushChunk(iff, 0, ID_TCOL, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		Value = SCA_WORD2BE(scp->ple_SAC->sac_NumColors);
		if (WriteChunkBytes(iff, &Value, sizeof(Value)) != sizeof(Value))
			{
			Result = IoErr();
			break;
			}

		for (n=0; n < 3 * scp->ple_SAC->sac_NumColors; n++)
			{
			byteColorTable[n] = (UBYTE) (scp->ple_SAC->sac_ColorTable[n] >> 24);
			}

		Length = scp->ple_SAC->sac_NumColors * 3;

		if (WriteChunkBytes(iff, byteColorTable, Length) != Length)
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


static LONG WritePrefsBitMap(struct IFFHandle *iff,
	const struct PatternListEntry *scp, struct TempRastPort *trp)
{
	LONG Result;
	UBYTE *BitMapArray = NULL;

	do	{
		struct RastPort rp;
		struct ScaPatternPrefsBitMap spb;
		LONG Length;
		size_t BitMapArrayLength;

		spb.spb_Width = scp->ple_SAC->sac_Width;
		spb.spb_Height = scp->ple_SAC->sac_Height;

		InitRastPort(&rp);
		rp.BitMap = scp->ple_SAC->sac_BitMap;

		if (NULL == trp->trp_rp.BitMap || trp->trp_Width != TEMPRP_WIDTH(spb.spb_Width))
			{
			if (trp->trp_rp.BitMap)
				FreeBitMap(trp->trp_rp.BitMap);

			trp->trp_Width = TEMPRP_WIDTH(spb.spb_Width);
			trp->trp_rp.BitMap = AllocBitMap(trp->trp_Width, 1, 8, BMF_STANDARD, NULL);
			if (NULL == trp->trp_rp.BitMap)
				{
				Result = ERROR_NO_FREE_STORE;
				break;
				}
			}

		BitMapArrayLength = PIXELARRAY8_BUFFERSIZE(spb.spb_Width, spb.spb_Height - 1);

		BitMapArray = malloc(BitMapArrayLength);
		if (NULL == BitMapArray)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		ReadPixelArray8(&rp, 0, 0,
			spb.spb_Width - 1, spb.spb_Height - 1,
			BitMapArray, &trp->trp_rp);

		Result = PushChunk(iff, 0, ID_TBMP, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		spb.spb_Width = SCA_WORD2BE(spb.spb_Width);
		spb.spb_Height = SCA_WORD2BE(spb.spb_Height);

		Length = sizeof(struct ScaPatternPrefsBitMap);

		if (WriteChunkBytes(iff, &spb, Length) != Length)
			{
			Result = IoErr();
			break;
			}

		if (WriteChunkBytes(iff, BitMapArray, BitMapArrayLength) != BitMapArrayLength)
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;
		} while (0);

	if (BitMapArray)
		free(BitMapArray);

	return Result;
}

//----------------------------------------------------------------------------

static LONG SaveIcon(struct PatternPrefsInst *inst, CONST_STRPTR IconName)
{
	struct DiskObject *icon, *allocIcon;

	static UBYTE ImageData[] =
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
		(UWORD *)ImageData,
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
		32768
		};

	icon = allocIcon = GetDiskObject("ENV:sys/def_ScaPatternPrefs");
	if (NULL == icon)
		icon = allocIcon = GetDiskObject("ENV:sys/def_pref");
	if (NULL == icon)
		icon = &DefaultIcon;

	if (icon)
		{
		STRPTR oldDefaultTool = icon->do_DefaultTool;

		icon->do_DefaultTool = (STRPTR) inst->ppb_ProgramName;

		PutDiskObject((STRPTR) IconName, icon);

		icon->do_DefaultTool = oldDefaultTool;

		if (allocIcon)
			FreeDiskObject(allocIcon);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT ListSelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	struct PatternListEntry *scp = NULL;

	inst->ppb_fDontChange = TRUE;

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &scp);
	if (scp)
		{
		set(inst->ppb_Objects[OBJNDX_DelButton], MUIA_Disabled, FALSE);
		set(inst->ppb_Objects[OBJNDX_Group_String], MUIA_Disabled, FALSE);

		d1(kprintf("%s/%s/%ld: scp=%08lx  type=%ld\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType));

		setcheckmark(inst->ppb_Objects[OBJNDX_CheckmarkGad], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_NOREMAP);
		setcheckmark(inst->ppb_Objects[OBJNDX_CheckmarkEnh], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_ENHANCED);
		setcheckmark(inst->ppb_Objects[OBJNDX_CheckmarkAutoDither], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_AUTODITHER);

		setcycle(inst->ppb_Objects[OBJNDX_CyclePatternType], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType);
		set(inst->ppb_Objects[OBJNDX_NumButton], MUIA_Numeric_Value, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number);

		SetPrecision(inst, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Precision);

		setslider(inst->ppb_Objects[OBJNDX_SliderColours], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_NumColors);
		setcycle(inst->ppb_Objects[OBJNDX_CycleDitherMode], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherMode);
		set(inst->ppb_Objects[OBJNDX_SliderDitherAmount], MUIA_Numeric_Value, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount);

		set(inst->ppb_Objects[OBJNDX_Group_Slider], MUIA_Disabled, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_NOREMAP);
		set(inst->ppb_Objects[OBJNDX_Group_Enhanced], MUIA_Disabled, !(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_ENHANCED));
		set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_Disabled, FALSE);

		set(inst->ppb_Objects[OBJNDX_STR_PopAsl], MUIA_String_Contents, (ULONG) scp->ple_PatternPrefs.scxp_Name);

		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Red,   RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[0]));
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Green, RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[1]));
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Blue,  RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[2]));

		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Red,   RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[0]));
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Green, RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[1]));
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Blue,  RGB_8_TO_32(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[2]));

		setcycle(inst->ppb_Objects[OBJNDX_CycleBackgroundType], scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Type);

		DisableBackgroundColorGadgets(inst,
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType,
                        scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Type);

		if (inst->ppb_fPreview)
			{
			set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_Disabled, FALSE);

			if (inst->ppb_fAutoPreview)
				PreviewSelect(inst);
			else
				{
				SetAttrs(inst->ppb_Objects[OBJNDX_PreviewImage],
					MUIA_ScaDtpic_Name, "",
					MUIA_ScaDtpic_Tiled, FALSE,
					TAG_END);
				set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_Background, MUII_ButtonBack);
				}
			}

		}
	else
		{
		set(inst->ppb_Objects[OBJNDX_DelButton], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_Group_String], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_Group_Slider], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_Group_Enhanced], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_Disabled, TRUE);
                set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_CycleBackgroundType], MUIA_Disabled, TRUE);
		}

	inst->ppb_fDontChange = FALSE;
}

//----------------------------------------------------------------------------

static void PreviewSelect(struct PatternPrefsInst *inst)
{
	struct PatternListEntry *scp = NULL;

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &scp);
	if (scp)
		{
//		ULONG Value;
//		ULONG DefsFlags = 0;
//
//		get(inst->ppb_Objects[OBJNDX_CheckmarkFriend], MUIA_Selected, &Value);
//		if (Value)
//			DefsFlags |= SCDF_USEFRIENDBM;

		SetAttrs(inst->ppb_Objects[OBJNDX_PreviewImage],
			MUIA_ScaDtpic_Name, scp->ple_PatternPrefs.scxp_Name,
			MUIA_ScaDtpic_Tiled, SCP_RenderType_Tiled == scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType,
			TAG_END);
		}

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
}

//----------------------------------------------------------------------------

static BYTE PrecisionLookupTable[] = 
	{
	PRECISION_EXACT,
	PRECISION_IMAGE,
	PRECISION_ICON,
	PRECISION_GUI,
	};

static void SetPrecision(struct PatternPrefsInst *inst, BYTE Precision)
{
	ULONG Value;

	inst->ppb_patternPrecision = Precision;

	for (Value=0; 
		PrecisionLookupTable[Value] != Precision 
		&& Value < sizeof(PrecisionLookupTable)/sizeof(PrecisionLookupTable[0]);
		Value++)
		;

	set(inst->ppb_Objects[OBJNDX_PrecSlider], MUIA_Numeric_Value, Value);
}


static BYTE GetPrecision(struct PatternPrefsInst *inst)
{
	ULONG Value = 0;

	get(inst->ppb_Objects[OBJNDX_PrecSlider], MUIA_Numeric_Value, &Value);

	return PrecisionLookupTable[Value];
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT ListChangeEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (!inst->ppb_fDontChange)
		{
		struct PatternListEntry *scp = NULL;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SetChangedFlag(inst, TRUE);

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &scp);
		if (scp)
			{
			STRPTR String = NULL;

			get(inst->ppb_Objects[OBJNDX_STR_PopAsl], MUIA_String_Contents, &String);
			if (NULL != String)
				{
				stccpy(scp->ple_PatternPrefs.scxp_Name, String, sizeof(scp->ple_PatternPrefs.scxp_Name));

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				if (inst->ppb_UseThumbNails)
					{
					set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

					// detach old image from nlist
					DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_UseImage,
						inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap], scp->ple_ThumbnailImageNr, 0);

					d1(KPrintF("%s/%s/%ld: ImageNr=%ld\n", __FILE__, __FUNC__, __LINE__, scp->ple_ThumbnailImageNr));

					// free old image
					if (scp->ple_MUI_ImageObject)
						{
						MUI_DisposeObject(scp->ple_MUI_ImageObject);
						scp->ple_MUI_ImageObject = NULL;
						}
					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					if (scp->ple_SAC)
						{
						ScalosGfxFreeSAC(scp->ple_SAC);
						scp->ple_SAC = NULL;
						}

					// create new image
					CreateThumbnailImage(inst, scp);
					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
					}

				DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Redraw, MUIV_NList_Redraw_Active);

				if (inst->ppb_fAutoPreview)
					PreviewSelect(inst);
				}
			}
		}
}

static SAVEDS(void) INTERRUPT ListChangeEntry2HookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (!inst->ppb_fDontChange)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &scp);
		if (scp)
			{
			ULONG selected = 0;
			ULONG Value = 0;

			d1(kprintf("%s/%s/%ld: \n"));
			SetChangedFlag(inst, TRUE);

			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags = 0;

			get(inst->ppb_Objects[OBJNDX_CheckmarkGad], MUIA_Selected, &selected);
			if (selected)
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags |= SCPF_NOREMAP;

			get(inst->ppb_Objects[OBJNDX_CheckmarkEnh], MUIA_Selected, &selected);
			if (selected)
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags |= SCPF_ENHANCED;

			get(inst->ppb_Objects[OBJNDX_CheckmarkAutoDither], MUIA_Selected, &selected);
			if (selected)
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags |= SCPF_AUTODITHER;

			get(inst->ppb_Objects[OBJNDX_CyclePatternType], MUIA_Cycle_Active, &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType = Value;

			d1(kprintf("%s/%s/%ld: scp=%08lx  type=%ld\n", __FILE__, __FUNC__, __LINE__, scp, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType));

			get(inst->ppb_Objects[OBJNDX_CycleDitherMode], MUIA_Cycle_Active, &selected);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherMode = selected;

			get(inst->ppb_Objects[OBJNDX_SliderColours], MUIA_Numeric_Value, &selected);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_NumColors = selected;

			get(inst->ppb_Objects[OBJNDX_SliderDitherAmount], MUIA_Numeric_Value, &selected);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_DitherAmount = selected;

			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Precision = GetPrecision(inst);

			set(inst->ppb_Objects[OBJNDX_Group_Slider], MUIA_Disabled, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_NOREMAP);
			set(inst->ppb_Objects[OBJNDX_Group_Enhanced], MUIA_Disabled, !(scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Flags & SCPF_ENHANCED));

			get(inst->ppb_Objects[OBJNDX_CycleBackgroundType], MUIA_Cycle_Active, &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Type = Value;

			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Red,   &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[0] = Value >> 24;
			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Green, &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[1] = Value >> 24;
			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Coloradjust_Blue,  &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color1[2] = Value >> 24;

			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Red,   &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[0] = Value >> 24;
			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Green, &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[1] = Value >> 24;
			get(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Coloradjust_Blue,  &Value);
			scp->ple_PatternPrefs.scxp_PatternPrefs.scp_color2[2] = Value >> 24;

			DisableBackgroundColorGadgets(inst,
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_RenderType,
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Type);
			}
		}
}

static SAVEDS(void) INTERRUPT ListChangeEntry3HookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (!inst->ppb_fDontChange)
		{
		struct PatternListEntry *scp = NULL;

		d1(kprintf("%s/%s/%ld: \n"));
		SetChangedFlag(inst, TRUE);

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &scp);
		if (scp)
			{
			ULONG Value = 0;

			get(inst->ppb_Objects[OBJNDX_NumButton], MUIA_Numeric_Value, &Value);

			if (0 != Value)
				{
				scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number = Value;
				DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Redraw, MUIV_NList_Redraw_Active);
				}
			}
		}
}


static SAVEDS(void) INTERRUPT AddNewEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	d1(kprintf("%s/%s/%ld: \n"));
	SetChangedFlag(inst, TRUE);

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_InsertSingle, "SYS:Prefs/Patterns", MUIV_NList_Insert_Bottom);
	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Active, MUIV_NList_Active_Bottom);

	DoMethod(inst->ppb_Objects[OBJNDX_Popasl], MUIM_Popstring_Open);
}


static SAVEDS(void) INTERRUPT RemEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	d1(kprintf("%s/%s/%ld: \n"));
	SetChangedFlag(inst, TRUE);

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Remove, MUIV_NList_Remove_Active);
}


static SAVEDS(void) INTERRUPT AppMsgInMainListHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	struct AppMessage *AppMsg = *((struct AppMessage **) msg);
	ULONG n;

	for (n=0; n<AppMsg->am_NumArgs; n++)
		{
		char Path[MAX_FILENAME];

		if (NameFromLock(AppMsg->am_ArgList[n].wa_Lock, Path, sizeof(Path))
			&& AddPart(Path, AppMsg->am_ArgList[n].wa_Name, sizeof(Path)))
			{
			DoMethod(inst->ppb_Objects[OBJNDX_MainList],
				MUIM_NList_InsertSingle, Path, MUIV_NList_Insert_Bottom);

			set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Active, MUIV_NList_Active_Bottom);

			// create thumbnail for the new entry
			CallHookPkt(&inst->ppb_Hooks[HOOKNDX_ListChangeEntry], NULL, NULL);
			}
		}

	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Sort);

	d1(kprintf("%s/%s/%ld: \n"));
	SetChangedFlag(inst, TRUE);
}


//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	inst->ppb_ThumbnailImageNumber = 0;
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Clear);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ReadPrefsFile(inst, "ENVARC:Scalos/Pattern.prefs", FALSE);
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetChangedFlag(inst, TRUE);
}


static SAVEDS(void) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	inst->ppb_ThumbnailImageNumber = 0;
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Clear);

	ReadPrefsFile(inst, "ENV:Scalos/Pattern.prefs", FALSE);
	d1(kprintf("%s/%s/%ld: \n"));
	SetChangedFlag(inst, TRUE);
}


static SAVEDS(void) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG n;
	static const struct ScaExtPatternPrefs DefaultPrefs[] =
		{
		{
			{
			1,
                        SCP_RenderType_Tiled,
                        SCPF_ENHANCED,
			100,
			DITHERMODE_FS,
			1,
			PRECISION_IMAGE,
			SCP_BgType_Picture,
			},
			"THEME:Desktop/Background",
		},
		{
			{
			3,
                        SCP_RenderType_Tiled,
                        SCPF_ENHANCED,
			100,
			DITHERMODE_FS,
			1,
			PRECISION_IMAGE,
			SCP_BgType_Picture,
			},
			"THEME:Window/Background"
		},
		{
			{
			4,
                        SCP_RenderType_Tiled,
                        SCPF_ENHANCED,
			100,
			DITHERMODE_FS,
			1,
			PRECISION_IMAGE,
			SCP_BgType_Picture,
			},
			"THEME:Window/TextBackground"
		},
		};

	inst->ppb_ThumbnailImageNumber = 0;
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Clear);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	set(inst->ppb_Objects[OBJNDX_CheckmarkAsync], MUIA_Selected, TRUE);
	set(inst->ppb_Objects[OBJNDX_CheckmarkFriend], MUIA_Selected, TRUE);
	set(inst->ppb_Objects[OBJNDX_CheckmarkRelayout], MUIA_Selected, TRUE);
	set(inst->ppb_Objects[OBJNDX_CheckmarkRandom], MUIA_Selected, TRUE);
	set(inst->ppb_Objects[OBJNDX_SliderWB], MUIA_Numeric_Value, 1);
	set(inst->ppb_Objects[OBJNDX_SliderScreen], MUIA_Numeric_Value, 0);
	set(inst->ppb_Objects[OBJNDX_SliderWin], MUIA_Numeric_Value, 3);
	set(inst->ppb_Objects[OBJNDX_SliderText], MUIA_Numeric_Value, 4);
	set(inst->ppb_Objects[OBJNDX_SliderTaskPri], MUIA_Numeric_Value, -1);

	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	for (n = 0; n < Sizeof(DefaultPrefs); n++)
		{
		struct PatternListEntry *scp;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList],
			MUIM_NList_InsertSingle, DefaultPrefs[n].scxp_Name, MUIV_NList_Insert_Bottom);
		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		scp->ple_PatternPrefs.scxp_PatternPrefs = DefaultPrefs[n].scxp_PatternPrefs;
		}

	// Entries have been added unsorted.
	// now sort listview according to current sorting type
	DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Sort);

	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, FALSE);

	if (inst->ppb_UseThumbNails)
		CreateThumbnailImages(inst);
	
        SetChangedFlag(inst, TRUE);
}


static SAVEDS(APTR) INTERRUPT AboutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	MUI_Request(inst->ppb_Objects[OBJNDX_APP_Main], inst->ppb_Objects[OBJNDX_WIN_Main], 0, NULL, 
		GetLocString(MSGID_ABOUTREQOK), 
		GetLocString(MSGID_ABOUTREQFORMAT), 
		MajorVersion, MinorVersion, COMPILER_STRING, CURRENTYEAR);

	return 0;
}


static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (NULL == inst->ppb_LoadReq)
		{
		inst->ppb_LoadReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Pattern.pre",
			ASLFR_Flags1, FRF_DOPATTERNS,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_InitialPattern, "#?.(pre|prefs)",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->ppb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}

	if (inst->ppb_LoadReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->ppb_LoadReq,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_OPEN),
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->ppb_LoadReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				inst->ppb_ThumbnailImageNumber = 0;
				DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_Clear);

				ReadPrefsFile(inst, inst->ppb_LoadReq->fr_File, FALSE);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	if (NULL == inst->ppb_SaveReq)
		{
		inst->ppb_SaveReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Pattern.pre",
			ASLFR_DoSaveMode, TRUE,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->ppb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}

	if (inst->ppb_SaveReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->ppb_SaveReq,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_SAVEAS),
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->ppb_SaveReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				LONG Result;
				BPTR oldDir = CurrentDir(dirLock);

				Result = WritePrefsFile(inst, inst->ppb_SaveReq->fr_File);

				if (RETURN_OK == Result)
					{
					if (inst->ppb_fCreateIcons)
						SaveIcon(inst, inst->ppb_SaveReq->fr_File);
					}

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT PreviewSelectHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	PreviewSelect(inst);

	return 0;
}

static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG MenuHookIndex = 0;
	struct Hook *MenuHook = NULL;

	d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuObj));
	d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, *((ULONG *) msg)));

	get(MenuObj, MUIA_UserData, &MenuHookIndex);

	if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
		MenuHook = &inst->ppb_Hooks[MenuHookIndex];

	d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuHook));
	if (MenuHook)
		DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

	return NULL;
}


static SAVEDS(APTR) INTERRUPT ShowPreviewHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG Checked = FALSE;

	get(MenuObj, MUIA_Menuitem_Checked, &Checked);
	inst->ppb_fPreview = Checked;

	set(inst->ppb_Objects[OBJNDX_Balance], MUIA_ShowMe, inst->ppb_fPreview);
	set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_ShowMe, inst->ppb_fPreview);

	if (!inst->ppb_fPreview)
		{
		inst->ppb_fAutoPreview = FALSE;
		set(inst->ppb_Objects[OBJNDX_Menu_AutoPreview], MUIA_Menuitem_Checked, inst->ppb_fAutoPreview);
		}

	set(inst->ppb_Objects[OBJNDX_Menu_AutoPreview], MUIA_Menuitem_Enabled, inst->ppb_fPreview);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT AutoPreviewHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG Checked = FALSE;

	get(MenuObj, MUIA_Menuitem_Checked, &Checked);
	inst->ppb_fAutoPreview = Checked;

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ThumbnailsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG Checked = FALSE;

	get(MenuObj, MUIA_Menuitem_Checked, &Checked);
	inst->ppb_UseThumbNails = Checked;

	UpdateUseThumbnails(inst);

	if (inst->ppb_UseThumbNails)
		{
		CreateThumbnailImages(inst);
		}
	else
		{
		RemoveThumbnailImages(inst);
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SettingsChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetChangedFlag(inst, TRUE);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT DesktopPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG Pattern = 0;
	ULONG n, nEntries = 0;
	BOOL Found;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	get(inst->ppb_Objects[OBJNDX_SliderWB], MUIA_Numeric_Value, &Pattern);
	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

	d1(KPrintF("%s/%s/%ld: Pattern=%lu  Entries=%lu\n", __FILE__, __FUNC__, __LINE__, Pattern, nEntries));

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp && scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number == Pattern
			&& scp->ple_MUI_ImageObject)
			{
			d1(KPrintF("%s/%s/%ld: BitMapObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			Found = TRUE;
			set(inst->ppb_Objects[OBJNDX_Image_DesktopPattern],
				BFA_BitmapObject, scp->ple_MUI_ImageObject);
			}
		}

	if (!Found)
		{
		set(inst->ppb_Objects[OBJNDX_Image_DesktopPattern],
			BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap]);
		}
}

static SAVEDS(void) INTERRUPT ScreenPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG Pattern = 0;
	ULONG n, nEntries = 0;
	BOOL Found;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	get(inst->ppb_Objects[OBJNDX_SliderScreen], MUIA_Numeric_Value, &Pattern);
	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

	d1(KPrintF("%s/%s/%ld: Pattern=%lu  Entries=%lu\n", __FILE__, __FUNC__, __LINE__, Pattern, nEntries));

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp && scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number == Pattern
			&& scp->ple_MUI_ImageObject)
			{
			d1(KPrintF("%s/%s/%ld: BitMapObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			Found = TRUE;
			set(inst->ppb_Objects[OBJNDX_Image_ScreenPattern],
				BFA_BitmapObject, scp->ple_MUI_ImageObject);
			}
		}

	if (!Found)
		{
		set(inst->ppb_Objects[OBJNDX_Image_ScreenPattern],
			BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap]);
		}
}

static SAVEDS(void) INTERRUPT IconWindowPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG Pattern = 0;
	ULONG n, nEntries = 0;
	BOOL Found;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	get(inst->ppb_Objects[OBJNDX_SliderWin], MUIA_Numeric_Value, &Pattern);
	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

	d1(KPrintF("%s/%s/%ld: Pattern=%lu  Entries=%lu\n", __FILE__, __FUNC__, __LINE__, Pattern, nEntries));

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp && scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number == Pattern
			&& scp->ple_MUI_ImageObject)
			{
			d1(KPrintF("%s/%s/%ld: BitMapObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			Found = TRUE;
			set(inst->ppb_Objects[OBJNDX_Image_IconWindowPattern],
				BFA_BitmapObject, scp->ple_MUI_ImageObject);
			}
		}

	if (!Found)
		{
		set(inst->ppb_Objects[OBJNDX_Image_IconWindowPattern],
			BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap]);
		}
}

static SAVEDS(void) INTERRUPT TextWindowPatternChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG Pattern = 0;
	ULONG n, nEntries = 0;
	BOOL Found;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	get(inst->ppb_Objects[OBJNDX_SliderText], MUIA_Numeric_Value, &Pattern);
	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

	d1(KPrintF("%s/%s/%ld: Pattern=%lu  Entries=%lu\n", __FILE__, __FUNC__, __LINE__, Pattern, nEntries));

	for (Found = FALSE, n = 0; !Found && n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp && scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number == Pattern
			&& scp->ple_MUI_ImageObject)
			{
			d1(KPrintF("%s/%s/%ld: BitMapObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			Found = TRUE;
			set(inst->ppb_Objects[OBJNDX_Image_TextWindowPattern],
				BFA_BitmapObject, scp->ple_MUI_ImageObject);
			}
		}

	if (!Found)
		{
		set(inst->ppb_Objects[OBJNDX_Image_TextWindowPattern],
			BFA_BitmapObject, inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap]);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == iMsg->Class)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_APP_Main], MUIM_Application_CheckRefresh);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT ReloadThumbnailsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PatternPrefsInst *inst = (struct PatternPrefsInst *) hook->h_Data;
	ULONG n, nEntries = 0;

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);
	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);

	SetChangedFlag(inst, TRUE);

	for (n = 0; n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp)
			{
			d1(KPrintF("%s/%s/%ld: scp=%08lx  ple_SAC=%08lx  scp_Number=%lu\n", __FILE__, __FUNC__, __LINE__, \
				scp, scp->ple_SAC, scp->ple_PatternPrefs.scxp_PatternPrefs.scp_Number));
			if (scp->ple_SAC)
				{
				// delete thumbnail information
				ScalosGfxFreeSAC(scp->ple_SAC);
				scp->ple_SAC = NULL;
				}
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}
		}

	CreateThumbnailImages(inst);

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
}

//----------------------------------------------------------------------------

static Object *CreateEmptyThumbnailImage(struct PatternPrefsInst *inst)
{
	static const ULONG ThumbnailImageColors[] =
	{
	   0xaaaaaaaa,0xaaaaaaaa,0xa0a0a0a0,
	   0x00000000,0x00000000,0x00000000,
	   0xffffffff,0xffffffff,0xffffffff,
	   0x66666666,0x88888888,0xbbbbbbbb,
	   0x99999999,0x99999999,0x99999999,
	   0xbbbbbbbb,0xbbbbbbbb,0xbbbbbbbb,
	   0xbbbbbbbb,0xaaaaaaaa,0x99999999,
	   0xffffffff,0xbbbbbbbb,0xaaaaaaaa
	};
	struct RastPort rp;
	struct BitMap *bm;
	Object *BitMapObj = NULL;

	rp.BitMap = NULL;
	InitRastPort(&rp);

	do	{
		bm = rp.BitMap = AllocBitMap(inst->ppb_ThumbnailWidth, inst->ppb_ThumbnailHeight, 2, BMF_CLEAR, NULL);
		d1(kprintf("%s/%s/%ld: bm=%08lx\n", __FILE__, __FUNC__, __LINE__, bm));
		if (NULL == bm)
			break;

		SetRast(&rp, 0);
		SetAPen(&rp, 1);

		Move(&rp, 0, 0);
		Draw(&rp, 0, inst->ppb_ThumbnailHeight - 1);
		Draw(&rp, inst->ppb_ThumbnailWidth - 1, inst->ppb_ThumbnailHeight - 1);
		Draw(&rp, inst->ppb_ThumbnailWidth - 1, 0);
		Draw(&rp, 0, 0);

		Move(&rp, 0, 0);
		Draw(&rp, inst->ppb_ThumbnailWidth - 1, inst->ppb_ThumbnailHeight - 1);

		Move(&rp, 0, inst->ppb_ThumbnailHeight - 1);
		Draw(&rp, inst->ppb_ThumbnailWidth - 1, 0);

		BitMapObj = BitmapObject,
			MUIA_Bitmap_Width, inst->ppb_ThumbnailWidth,
			MUIA_Bitmap_Height, inst->ppb_ThumbnailHeight,
			MUIA_Bitmap_Bitmap, (IPTR)bm,
			MUIA_Bitmap_Transparent, 0,
			MUIA_Bitmap_UseFriend, TRUE,
			MUIA_Bitmap_Precision, PRECISION_ICON,
			MUIA_Bitmap_SourceColors, (IPTR)ThumbnailImageColors,
			End;

		d1(kprintf("%s/%s/%ld: BitMapObj=%08lx\n", __FILE__, __FUNC__, __LINE__, BitMapObj));

		if (BitMapObj)
			bm = NULL;	// don't free bm
		} while (0);

	if (bm)
		FreeBitMap(bm);

	return BitMapObj;
}

//----------------------------------------------------------------------------

static void RemoveThumbnailImages(struct PatternPrefsInst *inst)
{
	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);
	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(inst->ppb_Objects[OBJNDX_MainList],
		MUIM_NList_UseImage, NULL, MUIV_NList_UseImage_All, 0);

	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
}

//----------------------------------------------------------------------------

static void CreateThumbnailImages(struct PatternPrefsInst *inst)
{
	T_TIMEVAL tvNow;
	T_TIMEVAL tvStart;
	ULONG n, nEntries = 0;
	APTR prWindowPtr;
	struct Process *MyProcess = (struct Process *) FindTask(NULL);

	prWindowPtr = MyProcess->pr_WindowPtr;
	MyProcess->pr_WindowPtr = (APTR) ~0;	// suppress error requesters

	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

	get(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Entries, &nEntries);
	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	for (n = 0; n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp)
			{
			// replace image by empty thumbnail
			DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_UseImage,
				inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap],
				scp->ple_ThumbnailImageNr, 0);

			if (scp->ple_MUI_ImageObject)
				{
				MUI_DisposeObject(scp->ple_MUI_ImageObject);
				scp->ple_MUI_ImageObject = NULL;
				}
			}
		}

	inst->ppb_ThumbnailImageNumber = 0;

	// for performance reasons, GaugeProgress and TextMsgProgress
	// are updated no faster than every 100ms
	GetSysTime(&tvStart);
	tvNow.tv_secs = 0;
	tvNow.tv_micro = 100000;
	AddTime(&tvStart, &tvNow);	// +100ms

	set(inst->ppb_Objects[OBJNDX_MsgGauge], MUIA_Gauge_Max, nEntries);

	for (n = 0; n < nEntries; n++)
		{
		struct PatternListEntry *scp = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_APP_Main], MUIM_Application_InputBuffered);

		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_GetEntry, n, &scp);

		if (scp)
			{
			if (inst->ppb_UseSplashWindow)
				{
				GetSysTime(&tvNow);

				if (CmpTime(&tvStart, &tvNow) > 0)
					{
					// it's more than 100ms since initial start
					ULONG WindowIsOpen = 0;
					char TextLine[50];

					tvNow.tv_secs = 0;
					tvNow.tv_micro = 100000;
					AddTime(&tvStart, &tvNow);	// +100ms

					// make sure the progress display window is open
					get(inst->ppb_Objects[OBJNDX_WIN_Message], MUIA_Window_Open, &WindowIsOpen);
					if (!WindowIsOpen)
						{
						set(inst->ppb_Objects[OBJNDX_WIN_Message], MUIA_Window_Open, TRUE);
						}

					set(inst->ppb_Objects[OBJNDX_MsgText1], MUIA_Text_Contents,
						(ULONG) GetLocString(scp->ple_SAC ? MSGID_LOADING_THUMBNAIL : MSGID_CREATING_THUMBNAIL));
					set(inst->ppb_Objects[OBJNDX_MsgText2], MUIA_Text_Contents, (ULONG) scp->ple_PatternPrefs.scxp_Name);

					sprintf(TextLine, GetLocString(MSGID_PROGRESS_THUMBNAILS), n, nEntries);
					set(inst->ppb_Objects[OBJNDX_MsgGauge], MUIA_Gauge_Current, n);
					set(inst->ppb_Objects[OBJNDX_MsgGauge], MUIA_Gauge_InfoText, (ULONG) TextLine);
					}
				}

			CreateThumbnailImage(inst, scp);
			}
		}

	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	// Update pattern preview images
	CallHookPkt(&inst->ppb_Hooks[HOOKNDX_DesktopPatternChanged],
		inst->ppb_Objects[OBJNDX_Image_DesktopPattern],
                NULL);
	CallHookPkt(&inst->ppb_Hooks[HOOKNDX_ScreenPatternChanged],
		inst->ppb_Objects[OBJNDX_Image_ScreenPattern],
                NULL);
	CallHookPkt(&inst->ppb_Hooks[HOOKNDX_IconWindowPatternChanged],
		inst->ppb_Objects[OBJNDX_Image_IconWindowPattern],
                NULL);
	CallHookPkt(&inst->ppb_Hooks[HOOKNDX_TextWindowPatternChanged],
		inst->ppb_Objects[OBJNDX_Image_TextWindowPattern],
                NULL);

	set(inst->ppb_Objects[OBJNDX_WIN_Message], MUIA_Window_Open, FALSE);
	set(inst->ppb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);

	// restore pr_WindowPtr
	MyProcess->pr_WindowPtr = prWindowPtr;
}

//----------------------------------------------------------------------------

static void CreateThumbnailImage(struct PatternPrefsInst *inst, struct PatternListEntry *scp)
{
	Object *obj = NULL;
	struct RastPort rp;

	rp.BitMap = NULL;
	InitRastPort(&rp);

	do	{
		d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
		d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, scp->ple_PatternPrefs.scxp_Name));

		if (scp->ple_SAC)
		{
			// check if named image exists
			BPTR fLock;

			fLock = Lock(scp->ple_PatternPrefs.scxp_Name, ACCESS_READ);
			d1(KPrintF("%s/%s/%ld: fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
			if (fLock)
				{
				UnLock(fLock);
				}
			else
				{
				if (scp->ple_MUI_ImageObject)
					{
					MUI_DisposeObject(scp->ple_MUI_ImageObject);
					scp->ple_MUI_ImageObject = NULL;
					}
				if (scp->ple_SAC)
					{
					ScalosGfxFreeSAC(scp->ple_SAC);
					scp->ple_SAC = NULL;
					}
				}
		}

		if (NULL == scp->ple_MUI_ImageObject)
			{
			UBYTE *BitMapArray;
			ULONG ScreenDepth;

			ScreenDepth = GetBitMapAttr(inst->ppb_WBScreen->RastPort.BitMap, BMA_DEPTH);

			d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_SAC));
			if (NULL == scp->ple_SAC)
				{
				BitMapArray = NULL;
				scp->ple_SAC = DoLoadDT((STRPTR) scp->ple_PatternPrefs.scxp_Name, &rp,
					inst->ppb_ThumbnailWidth, inst->ppb_ThumbnailHeight,
					inst->ppb_WBScreen);

				d1(KPrintF("%s/%s/%ld: sac=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_SAC));
				if (NULL == scp->ple_SAC)
					break;
				}
			else
				{
				BitMapArray = scp->ple_BitMapArray;
				}

			if (CyberGfxBase && ScreenDepth > 8)
				{
				scp->ple_MUI_ImageObject = BitMapPicObject,
					MUIA_ScaBitMappic_BitMap, (IPTR)scp->ple_SAC->sac_BitMap,
					MUIA_ScaBitMappic_ColorTable, (IPTR)scp->ple_SAC->sac_ColorTable,
					MUIA_ScaBitMappic_Width, scp->ple_SAC->sac_Width,
					MUIA_ScaBitMappic_Height, scp->ple_SAC->sac_Height,
					MUIA_ScaBitMappic_Screen, (IPTR)inst->ppb_WBScreen,
					MUIA_ScaBitmappic_BitMapArray, (IPTR)BitMapArray,
					End;
				}
			else
				{
				scp->ple_MUI_ImageObject = BitmapObject,
					MUIA_Bitmap_Width, scp->ple_SAC->sac_Width,
					MUIA_Bitmap_Height, scp->ple_SAC->sac_Height,
					MUIA_Bitmap_Bitmap, (IPTR)scp->ple_SAC->sac_BitMap,
					MUIA_Bitmap_SourceColors, (IPTR)scp->ple_SAC->sac_ColorTable,
					MUIA_Bitmap_UseFriend, TRUE,
					MUIA_Bitmap_Precision, PRECISION_ICON,
					End;
				}

			d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));
			if (NULL == scp->ple_MUI_ImageObject)
				break;

			// generate unique image number
			scp->ple_ThumbnailImageNr = ++inst->ppb_ThumbnailImageNumber;
			}

		rp.BitMap = NULL;       // don't free rp.BitMap
		} while (0);

	d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (obj)
		DisposeDTObject(obj);
	if (rp.BitMap)
		FreeBitMap(rp.BitMap);

	d1(KPrintF("%s/%s/%ld: ImageNr=%ld\n", __FILE__, __FUNC__, __LINE__, scp->ple_ThumbnailImageNr));
	d1(KPrintF("%s/%s/%ld: ple_MUI_ImageObject=%08lx\n", __FILE__, __FUNC__, __LINE__, scp->ple_MUI_ImageObject));

	if (scp->ple_MUI_ImageObject)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_UseImage,
			scp->ple_MUI_ImageObject, scp->ple_ThumbnailImageNr, 0);
		}
	else
		{
		DoMethod(inst->ppb_Objects[OBJNDX_MainList], MUIM_NList_UseImage,
			inst->ppb_Objects[OBJNDX_EmptyThumbnailBitmap], scp->ple_ThumbnailImageNr, 0);
		}

	d1(KPrintF("%s/%s/%ld: end\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

void ShowError(const char *str, ...)
{
	(void) str;
}

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

DISPATCHER(myNList)
{
	struct PatternPrefsInst *inst = NULL;
	ULONG result;

	d1(kprintf("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf(__FUNC__ "/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FILE__, __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuObj));
		d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->ppb_Hooks[MenuHookIndex];

		d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

		result = 0;
		}
		break;

	default:
		result = DoSuperMethodA(cl, obj, msg);
		}

	return result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static void SetChangedFlag(struct PatternPrefsInst *inst, BOOL changed)
{
	if (changed != inst->ppb_Changed)
		{
		set(inst->ppb_Objects[OBJNDX_Lamp_Changed], 
			MUIA_Lamp_Color, changed ? MUIV_Lamp_Color_Ok : MUIV_Lamp_Color_Off);
		inst->ppb_Changed = changed;
		}
}

//-----------------------------------------------------------------

static void ParseToolTypes(struct PatternPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt)
{
	STRPTR tt;

	d1(kprintf(__FUNC__ "/%ld: start  ptt=%08lx  tooltypes=%08lx\n", __FILE__, __FUNC__, __LINE__, ptt, ptt->ToolTypes));
	d1(kprintf(__FUNC__ "/%ld: start  tooltypes=%08lx %08lx %08lx\n", __FILE__, __FUNC__, __LINE__, ptt, ptt->ToolTypes[0], ptt->ToolTypes[1], ptt->ToolTypes[2]));

	tt = FindToolType(ptt->ToolTypes, "NOSPLASHWINDOW");
	inst->ppb_UseSplashWindow = (NULL == tt);

	tt = FindToolType(ptt->ToolTypes, "THUMBNAILS");
	d1(kprintf(__FUNC__ "/%ld: tt=%08lx\n", __FILE__, __FUNC__, __LINE__, tt));
	if (tt)
		{
		inst->ppb_UseThumbNails = TRUE;
		inst->ppb_fPreview = FALSE;
		inst->ppb_fAutoPreview = FALSE;
		}
	else
		{
		tt = FindToolType(ptt->ToolTypes, "NOPREVIEW");
		d1(KPrintF(__FUNC__ "/%ld: tt=%08lx\n", __FILE__, __FUNC__, __LINE__, tt));
		if (tt)
			{
			inst->ppb_fPreview = FALSE;
			}
		else
			{
			tt = FindToolType(ptt->ToolTypes, "AUTOPREVIEW");
			d1(KPrintF(__FUNC__ "/%ld: tt=%08lx\n", __FILE__, __FUNC__, __LINE__, tt));
			if (tt)
				inst->ppb_fAutoPreview = TRUE;
			}
		}

	if (inst->ppb_Objects[OBJNDX_MainList])
		UpdateUseThumbnails(inst);

	d1(kprintf(__FUNC__ "/%ld: end\n", __FILE__, __FUNC__, __LINE__));
}

//---------------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	MajorVersion = PluginBase->pl_LibNode.lib_Version;
	MinorVersion = PluginBase->pl_LibNode.lib_Revision;

	d1(kprintf(__FUNC__ "/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (0x4711 == Signature++)
		{
		d1(kprintf(__FUNC__ "/%ld:   PatternPrefsBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
			PatternPrefsBase, FindTask(NULL)->tc_Node.ln_Name));

		d1(kprintf(__FUNC__ "/%ld:\n", __FILE__, __FUNC__, __LINE__));

		if (!OpenLibraries())
			return FALSE;

#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			return FALSE;
#endif

		PatternPrefsClass = MUI_CreateCustomClass(&PluginBase->pl_LibNode, MUIC_Group,
				NULL, sizeof(struct PatternPrefsInst), DISPATCHER_REF(PatternPrefs));

		d1(kprintf(__FUNC__ "/%ld:  PatternPrefsClass=%08lx\n", __FILE__, __FUNC__, __LINE__, PatternPrefsClass));
		if (NULL == PatternPrefsClass)
			return FALSE;

		if (LocaleBase)
			{
			if (NULL == PatternPrefsLocale)
				PatternPrefsLocale = OpenLocale(NULL);

			if (PatternPrefsLocale)
				{
				if (NULL == PatternPrefsCatalog)
					{
					PatternPrefsCatalog = OpenCatalog(PatternPrefsLocale,
						(STRPTR) "Scalos/ScalosPattern.catalog", NULL);
					}
				}
			}

		TranslateStringArray(dithermodecont);
		TranslateStringArray(patternmode);
		TranslateStringArray(RegisterTitleStrings);
		TranslateStringArray(CycleBackgroundModeStrings);

		TranslateNewMenu(ContextMenus);

		DataTypesImageClass = InitDtpicClass();
		if (NULL == DataTypesImageClass)
			return FALSE;	// Failure

		BackfillClass = InitBackfillClass();
		if (NULL == BackfillClass)
			return FALSE;	// Failure

		PatternSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL,
			0, DISPATCHER_REF(PatternSlider));
		if (NULL == PatternSliderClass)
			return FALSE;	// Failure

		BitMapPicClass = InitBitMappicClass();
		if (NULL == BitMapPicClass)
			return FALSE;	// Failure

		PrecSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL,
			0, DISPATCHER_REF(PrecSlider));
		if (NULL == PrecSliderClass)
			return FALSE;	// Failure

		myNListClass = MUI_CreateCustomClass(NULL, MUIC_NList,
			NULL, 0, DISPATCHER_REF(myNList));
		if (NULL == myNListClass)
			return FALSE;	// Failure
		}

	return TRUE;	// Success
}

//----------------------------------------------------------------------------

static void UpdateUseThumbnails(struct PatternPrefsInst *inst)
{
	set(inst->ppb_Objects[OBJNDX_Balance], MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Group_Preview], MUIA_ShowMe, inst->ppb_fPreview && !inst->ppb_UseThumbNails);

	set(inst->ppb_Objects[OBJNDX_Menu_Thumbnails],  MUIA_Menuitem_Checked, inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ShowPreview], MUIA_Menuitem_Checked, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ShowPreview], MUIA_Menuitem_Enabled, !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_AutoPreview], MUIA_Menuitem_Enabled, inst->ppb_fPreview && !inst->ppb_UseThumbNails);
	set(inst->ppb_Objects[OBJNDX_Menu_ReloadThumbnails], MUIA_Menuitem_Enabled, inst->ppb_UseThumbNails);

	set(inst->ppb_Objects[OBJNDX_MainList], MUIA_NList_Format,
		(ULONG) (inst->ppb_UseThumbNails ? LISTFORMAT_THUMBNAILS : LISTFORMAT_STANDARD));

	// pattern previews are only enabled if thumbnails are enabled
	set(inst->ppb_Objects[OBJNDX_Group_PatternPreviews], MUIA_ShowMe, inst->ppb_UseThumbNails);
}

//----------------------------------------------------------------------------

static void DisableBackgroundColorGadgets(struct PatternPrefsInst *inst, UWORD RenderType, UWORD BgType)
{
	// Background type only changeable with SCP_RenderType_Centered and SCP_RenderType_ScaledMin
	switch (RenderType)
		{
	case SCP_RenderType_FitSize:
	case SCP_RenderType_Tiled:
		set(inst->ppb_Objects[OBJNDX_CycleBackgroundType], MUIA_Disabled, TRUE);
		setcycle(inst->ppb_Objects[OBJNDX_CycleBackgroundType], SCP_BgType_Picture);
		BgType = SCP_BgType_Picture;
		break;
	case SCP_RenderType_Centered:
	case SCP_RenderType_ScaledMin:
	default:
		set(inst->ppb_Objects[OBJNDX_CycleBackgroundType], MUIA_Disabled, FALSE);
		break;
		}

	switch (BgType)
		{
	case SCP_BgType_SingleColor:
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Disabled, FALSE);
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Disabled, TRUE);
		break;
	case SCP_BgType_HorizontalGradient:
	case SCP_BGType_VerticalGradient:
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Disabled, FALSE);
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Disabled, FALSE);
		break;
	case SCP_BgType_Picture:
	default:
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor1], MUIA_Disabled, TRUE);
		set(inst->ppb_Objects[OBJNDX_ColorAdjustBgColor2], MUIA_Disabled, TRUE);
		break;
		}
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__) && !defined(__AROS__)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__)

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
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
#endif /* __MORPHOS__ */

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif /* __SASC */

//----------------------------------------------------------------------------
