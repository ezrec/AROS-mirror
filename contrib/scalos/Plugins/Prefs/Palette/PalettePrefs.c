// PalettePrefs.c
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
#include <utility/utility.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <prefs/prefhdr.h>

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

#ifndef __AROS__
#define	NO_INLINE_STDARG
#endif
#include <proto/muimaster.h>

#include <mui/NListview_mcc.h>
#include <mui/Lamp_mcc.h>
#include <scalos/palette.h>
#include <defs.h>
#include <Year.h>

#include <scalos/scalosprefsplugin.h>

#include <defs.h>
#include <Year.h>

#include "PalettePrefs.h"
#include "DataTypesMCC.h"

#define	ScalosPalette_NUMBERS
#define	ScalosPalette_ARRAY
#define	ScalosPalette_CODE
#include STR(SCALOSLOCALE)

#include "plugin.h"

//----------------------------------------------------------------------------

#ifdef __AROS__
#define NewColorAdjustObject BOOPSIOBJMACRO_START(NewColorAdjustClass->mcc_Class)
#define NewPalettePenListObject BOOPSIOBJMACRO_START(NewPalettePenListClass->mcc_Class)
#define NewScalosPenListObject BOOPSIOBJMACRO_START(NewScalosPenListClass->mcc_Class)
#define myNListObject BOOPSIOBJMACRO_START(myNListClass->mcc_Class)
#define DataTypesImageObject BOOPSIOBJMACRO_START(DataTypesImageClass->mcc_Class)
#else
#define NewColorAdjustObject NewObject(NewColorAdjustClass->mcc_Class, 0
#define NewPalettePenListObject NewObject(NewPalettePenListClass->mcc_Class, 0
#define NewScalosPenListObject NewObject(NewScalosPenListClass->mcc_Class, 0
#define myNListObject NewObject(myNListClass->mcc_Class, 0
#define DataTypesImageObject NewObject(DataTypesImageClass->mcc_Class, 0
#endif

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

// from DoLoadDT.c
extern Object *DoLoadDT(STRPTR source, struct RastPort *rast,
	struct ColorMap *cmap,
	int x, int y, int w, int h,
	ULONG FirstTag, ...);

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

#define	ID_SPAL	MAKE_ID('S','P','A','L')
#define	ID_PENS	MAKE_ID('P','E','N','S')


#define	PEN_UNDEFINED	(-1)


struct NewScalosPenListInst
	{
	ULONG nspl_fDragging;
	};

struct PensListConstruct
	{
	ULONG splc_Red;
	ULONG splc_Green;
	ULONG splc_Blue;
	};

struct PensListEntry
	{
	// Red,Green,Blue MUST be the very first entries!
	// (used for MUIA_Coloradjust_RGB)
	ULONG sple_Red;
	ULONG sple_Green;
	ULONG sple_Blue;

	ULONG sple_Index;
	};

struct ScalosPenListEntry
	{
	WORD snle_PenIndex;
	WORD snle_PenNr;
	CONST_STRPTR snle_PenName;
	};

struct PenListInit
	{
	WORD pli_PenIndex;
	WORD pli_PenNr;
	ULONG pli_PenNameID;
	};

struct RGB
	{
	ULONG rgb_Red;
	ULONG rgb_Green;
	ULONG rgb_Blue;
	};

static const LONG StopChunkList[] =
	{
	ID_PREF, ID_SPAL,
	ID_PREF, ID_PENS,
	};

//----------------------------------------------------------------------------

// aus mempools.lib
#if !defined(__amigaos4__) && !defined(__AROS__)
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

// local functions

DISPATCHER_PROTO(PalettePrefs);
static BOOL CreatePalettePrefsObject(struct PalettePrefsInst *inst, struct opSet *ops);
static void DisposePalettePrefsObject(struct PalettePrefsInst *inst);
static Object *CreatePrefsGroup(struct PalettePrefsInst *inst);
static Object *CreatePrefsImage(void);
static void InitHooks(struct PalettePrefsInst *inst);

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static STRPTR GetLocString(ULONG MsgId);
static void TranslateNewMenu(struct NewMenu *nm);
//static void TranslateStringArray(STRPTR *stringArray);
DISPATCHER_PROTO(NewColorAdjust);
DISPATCHER_PROTO(NewPalettePenList);
DISPATCHER_PROTO(NewScalosPenList);
static APTR AllocatedPensListConstructFunc(struct Hook *hook, APTR memPool, struct PensListEntry *spleNew);
static void AllocatedPensListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry);
static ULONG AllocatedPensListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm);
static LONG AllocatedPensListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm);
static APTR WBPensListConstructFunc(struct Hook *hook, APTR memPool, struct PensListEntry *spleNew);
static void WBPensListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry);
static ULONG WBPensListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm);
static LONG WBPensListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm);
static APTR ScalosPenListConstructFunc(struct Hook *hook, APTR memPool, struct ScalosPenListEntry *msg);
static void ScalosPenListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry);
static ULONG ScalosPenListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm);
static LONG ScalosPenListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm);
static STRPTR GetLocString(ULONG MsgId);
static void GetWBColorsFunc(struct Hook *hook, Object *pop, Object *win);
static APTR AddPenFunc(struct Hook *hook, Object *o, Msg msg);
static APTR SelectPalettePenFunc(struct Hook *hook, Object *o, Msg msg);
static APTR SelectScalosPenFunc(struct Hook *hook, Object *o, Msg msg);
static APTR ChangePenColorFunc(struct Hook *hook, Object *o, ULONG **msg);
static APTR ResetToDefaultsFunc(struct Hook *hook, Object *o, Msg msg);
static APTR AboutFunc(struct Hook *hook, Object *o, Msg msg);
static APTR OpenFunc(struct Hook *hook, Object *o, Msg msg);
static APTR LastSavedFunc(struct Hook *hook, Object *o, Msg msg);
static APTR RestoreFunc(struct Hook *hook, Object *o, Msg msg);
static APTR SaveAsFunc(struct Hook *hook, Object *o, Msg msg);
static APTR DeletePenFunc(struct Hook *hook, Object *o, Msg msg);
static APTR ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg);
static void AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
static void InitPenList(struct PalettePrefsInst *inst);
static LONG ReadPrefsFile(struct PalettePrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet);
static void SearchAndSetPen(struct PalettePrefsInst *inst, const struct ScalosPen *sPen);
static LONG WritePrefsFile(struct PalettePrefsInst *inst, CONST_STRPTR Filename);
static LONG SaveIcon(struct PalettePrefsInst *inst, CONST_STRPTR IconName);
static void UpdatePalettePenCount(struct PalettePrefsInst *inst);
static ULONG GetPenReferenceCount(struct PalettePrefsInst *inst, ULONG PenNr);
static void SetChangedFlag(struct PalettePrefsInst *inst, BOOL changed);
DISPATCHER_PROTO(myNList);
static LONG ColorComp(ULONG Color1, ULONG Color2);
static LONG RGBComp(const struct PensListEntry *sple1, const struct PensListEntry *sple2);

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

#if defined(__GNUC__) && !defined(__amigaos4__)
extern T_UTILITYBASE __UtilityBase;
#endif /* __GNUC__ */

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
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

#ifdef __AROS__
struct DosLibrary *DOSBase;
#endif

struct MUI_CustomClass *NewColorAdjustClass;
struct MUI_CustomClass *NewPalettePenListClass;
struct MUI_CustomClass *NewScalosPenListClass;
struct MUI_CustomClass *myNListClass;
struct MUI_CustomClass *DataTypesImageClass;

struct MUI_CustomClass *PalettePrefsClass;

static struct Catalog *PalettePrefsCatalog;
static struct Locale *PalettePrefsLocale;

static ULONG MajorVersion, MinorVersion;

static ULONG Signature = 0x4711;

static const struct MUIP_ScalosPrefs_MCCList RequiredMccList[] =
	{
	{ MUIC_Lamp, 11, 1 },
	{ MUIC_NListview, 18, 0 },
	{ NULL, 0, 0 }
	};

static const struct Hook PalettePrefsHooks[] =
{
	{ { NULL, NULL }, HOOKFUNC_DEF(WBPensListConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(WBPensListDestructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(WBPensListDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(WBPensListCompareFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AllocatedPensListConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AllocatedPensListDestructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AllocatedPensListDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AllocatedPensListCompareFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ScalosPenListConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ScalosPenListDestructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ScalosPenListDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ScalosPenListCompareFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(GetWBColorsFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddPenFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectPalettePenFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectScalosPenFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangePenColorFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ResetToDefaultsFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AboutFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(OpenFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(LastSavedFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RestoreFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SaveAsFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(DeletePenFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ContextMenuTriggerHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AslIntuiMsgHookFunc), NULL },
};


static struct NewMenu ContextMenus[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_OPEN, 		(STRPTR) MSGID_MENU_PROJECT_OPEN_SHORT, 	0,	0, (APTR) HOOKNDX_Open },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_SAVEAS, 	(STRPTR) MSGID_MENU_PROJECT_SAVEAS_SHORT,	0,	0, (APTR) HOOKNDX_SaveAs },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 						0,	0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static const struct PenListInit PenListContents[] =
	{
	{ TEXTPEN,		  1, MSGID_TEXTPEN },
	{ SHINEPEN,		  2, MSGID_SHINEPEN },
	{ SHADOWPEN,		  1, MSGID_SHADOWPEN },
	{ FILLPEN,		  3, MSGID_HIFILLPEN },
	{ FILLTEXTPEN,		  1, MSGID_FILLTEXTPEN },
	{ BACKGROUNDPEN,	  0, MSGID_BGPEN },
	{ HIGHLIGHTTEXTPEN,	  2, MSGID_HITEXTPEN },
	{ BARDETAILPEN,		  1, MSGID_BARDETAILPEN },
	{ BARBLOCKPEN,		  2, MSGID_BARBLOCKPEN },
	{ BARTRIMPEN,		  1, MSGID_BARTRIMPEN },

	{  -1, 			  5, MSGID_HSHINEPEN },
	{  -2, 			  4, MSGID_HSHADOWPEN },
	{ -18, 			  1, MSGID_ICONTEXTPEN },
	{ -19, 			  1, MSGID_ICONTEXTPENSEL },
	{  -3, 			  1, MSGID_ICONTEXTOUTLINEPEN },
	{ -20, 			  1, MSGID_ICONTEXTSHADOWPEN },
	{ -23, 			  3, MSGID_ICONTEXTFILESELBG },
	{  -4, 			  3, MSGID_DRAWERTEXT },
	{  -5, 			  2, MSGID_DRAWERTEXTSEL },
	{  -6, 			  3, MSGID_DRAWERBG },
	{  -7, 			  1, MSGID_FILETEXT },
	{  -8, 			  2, MSGID_FILETEXTSEL },
	{  -9, 			  1, MSGID_FILEBG },
	{ -10, 			  0, MSGID_BACKDROPDETAIL },
	{ -11, 			  0, MSGID_BACKDROPBLOCK },
	{ -12, 			  1, MSGID_TOOLTIP_TEXT },
	{ -13, 			  2, MSGID_TOOLTIP_BG },
	{ -14, 			  1, MSGID_DRAGINFO_TEXT },
	{ -15, 			  2, MSGID_DRAGINFO_BG },
	{ -16, 			  4, MSGID_STATUSBAR_BG },
	{ -17, 			  1, MSGID_STATUSBAR_TEXT },
	{ -21, 			  4, MSGID_THUMBNAIL_BG },
	{ -22, 			  4, MSGID_THUMBNAIL_BG_SEL },
	{ 0, 			  0, 0 }
	};

static const struct PensListEntry DefaultPaletteEntries[] =
	{
	{ 0x95959595, 0x95959595, 0x95959595, 0 },
	{ 0x00000000, 0x00000000, 0x00000000, 1 },
	{ 0xffffffff, 0xffffffff, 0xffffffff, 2 },
	{ 0x56565656, 0x7e7e7e7e, 0xa2a2a2a2, 3 },
	{ 0x7b7b7b7b, 0x7b7b7b7b, 0x7b7b7b7b, 4 },
	{ 0xafafafaf, 0xafafafaf, 0xafafafaf, 5 },
	{ 0xaaaaaaaa, 0x90909090, 0x7c7c7c7c, 6 },
	{ 0xffffffff, 0xa9a9a9a9, 0x97979797, 7 },
	};

//---------------------------------------------------------------

VOID closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf(__FUNC__ "/%ld:\n", __LINE__));

	if (DataTypesImageClass)
		{
		CleanupDtpicClass(DataTypesImageClass);
		DataTypesImageClass = NULL;
		}
	if (myNListClass)
		{
		MUI_DeleteCustomClass(myNListClass);
		myNListClass = NULL;
		}

	if (NewColorAdjustClass)
		{
		MUI_DeleteCustomClass(NewColorAdjustClass);
		NewColorAdjustClass = NULL;
		}
	if (NewScalosPenListClass)
		{
		MUI_DeleteCustomClass(NewScalosPenListClass);
		NewScalosPenListClass = NULL;
		}
	if (NewPalettePenListClass)
		{
		MUI_DeleteCustomClass(NewPalettePenListClass);
		NewPalettePenListClass = NULL;
		}

	if (PalettePrefsCatalog)
		{
		CloseCatalog(PalettePrefsCatalog);
		PalettePrefsCatalog = NULL;
		}
	if (PalettePrefsLocale)
		{
		CloseLocale(PalettePrefsLocale);
		PalettePrefsLocale = NULL;
		}

	if (PalettePrefsClass)
		{
		MUI_DeleteCustomClass(PalettePrefsClass);
		PalettePrefsClass = NULL;
		}

	CloseLibraries();

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif
}


LIBFUNC_P2(ULONG, LIBSCAGetPrefsInfo,
	D0, ULONG, which,
	A6, struct PluginBase *, PluginBase);
{
	ULONG result;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: which=%ld\n", __LINE__, which));

	(void) PluginBase;

	switch(which)
		{
	case SCAPREFSINFO_GetClass:
		result = (ULONG) PalettePrefsClass;
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

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%08lx\n", __LINE__, result));

	return result;
}
LIBFUNC_END

//----------------------------------------------------------------------------

DISPATCHER(PalettePrefs)
{
	struct PalettePrefsInst *inst;
	ULONG result = 0;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	switch(msg->MethodID)
		{
	case OM_NEW:
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		d1(kprintf("%s/%s/%ld: obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		if (obj)
			{
			Object *prefsobject = NULL;

			inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);

			memset(inst, 0, sizeof(struct PalettePrefsInst));

			if (CreatePalettePrefsObject(inst, (struct opSet *) msg))
				{
				prefsobject = CreatePrefsGroup(inst);
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: prefsobject=%08lx\n", __LINE__, prefsobject));
				}
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
		DisposePalettePrefsObject((struct PalettePrefsInst *) INST_DATA(cl, obj));
		d1(kprintf("%s/%d/%ld: PalettePrefsCatalog=%08lx\n", __FILE__, __FUNC__, __LINE__, PalettePrefsCatalog));
		return DoSuperMethodA(cl, obj, msg);
		break;

	case OM_SET:
		{
		struct opSet *ops = (struct opSet *) msg;

		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		inst->ppb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, inst->ppb_fCreateIcons, ops->ops_AttrList);
		inst->ppb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) inst->ppb_ProgramName, ops->ops_AttrList);
		inst->ppb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, 
			(ULONG) inst->ppb_Objects[OBJNDX_WIN_Main], ops->ops_AttrList);
		inst->ppb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, 
			(ULONG) inst->ppb_Objects[OBJNDX_APP_Main], ops->ops_AttrList);

		return DoSuperMethodA(cl, obj, msg);
		}
		break;

	case OM_GET:
		{
		struct opGet *opg = (struct opGet *) msg;

		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
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

	case MUIM_ScalosPrefs_LoadConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		if (RETURN_OK != ReadPrefsFile(inst, "ENV:Scalos/Palette13.prefs", TRUE))
			ReadPrefsFile(inst, "ENV:Scalos/Palette13.prefs", FALSE);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENV:Scalos/Palette13.prefs");
		break;

	case MUIM_ScalosPrefs_UseConfigIfChanged:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		if (inst->ppb_Changed)
			{
			WritePrefsFile(inst, "ENV:Scalos/Palette13.prefs");
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_SaveConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENVARC:Scalos/Palette13.prefs");
		WritePrefsFile(inst, "ENV:Scalos/Palette13.prefs");
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_SaveConfigIfChanged:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		if (inst->ppb_Changed)
			{
			WritePrefsFile(inst, "ENVARC:Scalos/Palette13.prefs");
			WritePrefsFile(inst, "ENV:Scalos/Palette13.prefs");
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_RestoreConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_Restore], 0);
		break;

	case MUIM_ScalosPrefs_ResetToDefaults:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ResetToDefaults], 0);
		break;

	case MUIM_ScalosPrefs_LastSavedConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_LastSaved], 0);
		break;

	case MUIM_ScalosPrefs_OpenConfig:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_Open], 0);
		break;

	case MUIM_ScalosPrefs_SaveConfigAs:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SaveAs], 0);
		break;

	case MUIM_ScalosPrefs_About:
		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_About], 0);
		break;

	case MUIM_ScalosPrefs_LoadNamedConfig:
		{
		struct MUIP_ScalosPrefs_LoadNamedConfig *lnc = (struct MUIP_ScalosPrefs_LoadNamedConfig *) msg;

		inst = (struct PalettePrefsInst *) INST_DATA(cl, obj);
		ReadPrefsFile(inst, lnc->ConfigFileName, FALSE);
		}
		break;

	case MUIM_ScalosPrefs_CreateSubWindows:
		result = 0;
		break;

	case MUIM_ScalosPrefs_GetListOfMCCs:
		result = (ULONG) RequiredMccList;
		break;

	default:
		d1(kprintf("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));
		result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	d1(kprintf("%s/%d/%ld/%ld: END   result=%ld\n", __FILE__, __FUNC__, __LINE__, result));

	return result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static BOOL CreatePalettePrefsObject(struct PalettePrefsInst *inst, struct opSet *ops)
{
	BOOL Success = FALSE;

	do	{
		memset(inst, 0, sizeof(struct PalettePrefsInst));

		inst->ppb_Changed = FALSE;
		inst->ppb_fPreview = TRUE;
		inst->ppb_fAutoPreview = FALSE;
		inst->ppb_fDontChange = FALSE;
		inst->ppb_PalettePrecision = PRECISION_IMAGE;
		inst->ppb_PreviewWeight = 20;
		inst->ppb_AllocatedPensListImageIndex = 1;

		InitHooks(inst);

		inst->ppb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, TRUE, ops->ops_AttrList);
		inst->ppb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) "", ops->ops_AttrList);
		inst->ppb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, (ULONG) NULL, ops->ops_AttrList);
		inst->ppb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, (ULONG) NULL, ops->ops_AttrList);

		inst->ppb_WBScreen = LockPubScreen("Workbench");
		if (NULL == inst->ppb_WBScreen)
			break;

		Success = TRUE;
		} while (0);

	return Success;
}


static void DisposePalettePrefsObject(struct PalettePrefsInst *inst)
{
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
	if (inst->ppb_Objects[OBJNDX_ContextMenu])
		{
		MUI_DisposeObject(inst->ppb_Objects[OBJNDX_ContextMenu]);
		inst->ppb_Objects[OBJNDX_ContextMenu] = NULL;
		}
}


static Object *CreatePrefsGroup(struct PalettePrefsInst *inst)
{
	d1(kprintf(__FUNC__ "/%ld:  inst=%08lx\n", __LINE__, inst));

	inst->ppb_Objects[OBJNDX_ContextMenu] = MUI_MakeObject(MUIO_MenustripNM, ContextMenus, 0);

	if (NULL == inst->ppb_Objects[OBJNDX_ContextMenu])
		return NULL;

	inst->ppb_Objects[OBJNDX_Group_Main] = VGroup,
		MUIA_Background, MUII_PageBack,

		Child, HGroup,
			Child, VGroup,
				Child, TextObject,
					MUIA_Font, MUIV_Font_Title,
					TextFrame,
					MUIA_Background, MUII_GroupBack,
					MUIA_Text_PreParse, "\33c",
					MUIA_Text_Contents, GetLocString(MSGID_TITLE_SCALOSPENLIST),
				End, //NListviewObject

				Child, NListviewObject,
					MUIA_CycleChain, TRUE,
					MUIA_Listview_Input, TRUE,
					MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
					MUIA_NListview_NList, inst->ppb_Objects[OBJNDX_ScalosPenList] = NewScalosPenListObject,
						MUIA_NList_PrivateData, inst,
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						MUIA_NList_Format, "BAR D=8 P=\x1br,BAR D=8 P=\x1br,,",
						MUIA_NList_DisplayHook2, &inst->ppb_Hooks[HOOKNDX_ScalosPenListDisplay],
						MUIA_NList_ConstructHook, &inst->ppb_Hooks[HOOKNDX_ScalosPenListConstruct],
						MUIA_NList_DestructHook, &inst->ppb_Hooks[HOOKNDX_ScalosPenListDestruct],
						MUIA_NList_CompareHook2, &inst->ppb_Hooks[HOOKNDX_ScalosPenListCompare],
						MUIA_NList_DragSortable, TRUE,
						MUIA_NList_ShowDropMarks, TRUE,
						MUIA_NList_AutoVisible, TRUE,
						MUIA_NList_EntryValueDependent, TRUE,
						MUIA_NList_Title, TRUE,
						MUIA_NList_TitleSeparator, TRUE,
						MUIA_NList_SortType, 2,
						MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 2,
						MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
					End,
					MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_SCALOSPENLIST),
				End, //NListviewObject
			End, //VGroup

			Child, BalanceObject, 
			End, //BalanceObject

			Child, VGroup,
				Child, inst->ppb_Objects[OBJNDX_AllocatedPensListTitleObj] = TextObject,
					MUIA_Font, MUIV_Font_Title,
					TextFrame,
					MUIA_Background, MUII_GroupBack,
					MUIA_Text_PreParse, "\33c",
					MUIA_Text_Contents, inst->ppb_PalettePenListTitle,
				End, //TextObject

				Child, NListviewObject,
					MUIA_Listview_Input, TRUE,
					MUIA_CycleChain, TRUE,
					MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
					MUIA_NListview_NList, inst->ppb_Objects[OBJNDX_AllocatedPensList] = NewPalettePenListObject,
						MUIA_NList_PrivateData, inst,
						MUIA_UserData, inst,
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						MUIA_NList_Format, "BAR D=8 P=\x1br,BAR D=8 P=\x1br,BAR P=\x1br,NOBAR D=8 P=\x1br,NOBAR D=8 P=\x1br,NOBAR D=8 P=\x1br",
						MUIA_NList_DisplayHook2, &inst->ppb_Hooks[HOOKNDX_AllocatedPensListDisplay],
						MUIA_NList_ConstructHook, &inst->ppb_Hooks[HOOKNDX_AllocatedPensListConstruct],
						MUIA_NList_DestructHook, &inst->ppb_Hooks[HOOKNDX_AllocatedPensListDestruct],
						MUIA_NList_CompareHook2, &inst->ppb_Hooks[HOOKNDX_AllocatedPensListCompare],
						MUIA_NList_DragSortable, TRUE,
						MUIA_NList_ShowDropMarks, TRUE,
						MUIA_NList_AutoVisible, TRUE,
						MUIA_NList_EntryValueDependent, TRUE,
						MUIA_NList_Title, TRUE,
						MUIA_NList_TitleSeparator, TRUE,
						MUIA_NList_SortType, 0,
						MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
						MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
					End, //NewPalettePenListClass
					MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_PALETTEPENLIST),
				End, //NListviewObject

				Child, HGroup,
					Child, inst->ppb_Objects[OBJNDX_Lamp_Changed] = LampObject,
						MUIA_Lamp_Type, MUIV_Lamp_Type_Huge, 
						MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_LAMP_CHANGED),
						End, //LampObject

					Child, HGroup,
						MUIA_Group_SameSize, TRUE,
						Child, inst->ppb_Objects[OBJNDX_NewButton] = KeyButtonHelp(GetLocString(MSGID_NEW), 'n', GetLocString(MSGID_SHORTHELP_NEWBUTTON)),
						Child, inst->ppb_Objects[OBJNDX_DeleteButton] = KeyButtonHelp(GetLocString(MSGID_DELETE), 'd', GetLocString(MSGID_SHORTHELP_DELETEBUTTON)),
					End, //HGroup
				End, //HGroup

				Child, BalanceObject,
				End, //BalanceObject

				Child, inst->ppb_Objects[OBJNDX_ColorAdjust] = NewColorAdjustObject,
					MUIA_UserData, inst,
				End, //NewColorAdjustClass

				Child, PopobjectObject,
					MUIA_Popstring_String, TextObject,
						MUIA_CycleChain, TRUE,
						MUIA_Background, MUII_GroupBack,
						TextFrame,
						MUIA_Text_Contents, GetLocString(MSGID_IMPORTNAME),
						MUIA_Text_PreParse, "\33c",
						End,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_WBPENSBUTTON),
					MUIA_Popobject_WindowHook, &inst->ppb_Hooks[HOOKNDX_GetWBColors],
					MUIA_Popobject_Object, NListviewObject,
						MUIA_Listview_Input, TRUE,
						MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
						MUIA_Listview_MultiSelect, TRUE,
						MUIA_CycleChain, TRUE,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_WBPENLIST),
						MUIA_NListview_NList, inst->ppb_Objects[OBJNDX_WBColorsList] = myNListObject,
							MUIA_NList_PrivateData, inst,
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NList_Format, "BAR D=8 P=\x1br,BAR D=8 P=\x1br,NOBAR D=8 P=\x1br,NOBAR D=8 P=\x1br,NOBAR D=8 P=\x1br",
							MUIA_NList_DisplayHook2, &inst->ppb_Hooks[HOOKNDX_WBPensListDisplay],
							MUIA_NList_ConstructHook, &inst->ppb_Hooks[HOOKNDX_WBPensListConstruct],
							MUIA_NList_DestructHook, &inst->ppb_Hooks[HOOKNDX_WBPensListDestruct],
							MUIA_NList_CompareHook2, &inst->ppb_Hooks[HOOKNDX_WBPensListCompare],
							MUIA_NList_ShowDropMarks, TRUE,
							MUIA_NList_AutoVisible, TRUE,
							MUIA_NList_Title, TRUE,
							MUIA_NList_TitleSeparator, TRUE,
							MUIA_NList_SortType, 0,
							MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 0,
							MUIA_NList_EntryValueDependent, TRUE,
							MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
						End, //inst->ppb_myNListClass
					End, //NListviewObject
				End, //PopobjectObject
			End,
		End, //HGroup

		MUIA_ContextMenu, inst->ppb_Objects[OBJNDX_ContextMenu],
	End; //VGroup

	if (NULL == inst->ppb_Objects[OBJNDX_Group_Main])
		return NULL;

	InitPenList(inst);
	UpdatePalettePenCount(inst);

	set(inst->ppb_Objects[OBJNDX_DeleteButton], MUIA_Disabled, TRUE);

	// setup sorting hooks for OBJNDX_ScalosPenList list
	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_ScalosPenList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_ScalosPenList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_ScalosPenList], 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_ScalosPenList], 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	// setup sorting hooks for OBJNDX_AllocatedPensList list
	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	// setup sorting hooks for OBJNDX_WBColorsList list
	DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList], MUIM_Notify, MUIA_NList_TitleClick, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_WBColorsList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
	DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList], MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_WBColorsList], 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
	DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList], MUIM_Notify, MUIA_NList_SortType, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_WBColorsList], 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
	DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList], MUIM_Notify, MUIA_NList_SortType2, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_WBColorsList], 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);

	// notification on Contextmenu for all places except listviews
	DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_Group_Main], 3, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ContextMenuTrigger], MUIV_TriggerValue );

	// Notification for "Add pen" button
	DoMethod(inst->ppb_Objects[OBJNDX_NewButton], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_AddPen]);

	// Notification for "Delete pen" button
	DoMethod(inst->ppb_Objects[OBJNDX_DeleteButton], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_DeletePen]);

	// Update ColorAdjust every time a new pen is selected
	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_ColorAdjust], 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SelectPalettePen]);

	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		MUIV_Notify_Window, 2, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_SelectScalosPen]);

	DoMethod(inst->ppb_Objects[OBJNDX_ColorAdjust], MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
		inst->ppb_Objects[OBJNDX_AllocatedPensList], 3, MUIM_CallHook, &inst->ppb_Hooks[HOOKNDX_ChangePenColor], MUIV_TriggerValue);

	return inst->ppb_Objects[OBJNDX_Group_Main];
}


static Object *CreatePrefsImage(void)
{
#include "PalettePrefsImage.h"
	Object	*img;

	// First try to load datatypes image from THEME: tree
	img = DataTypesImageObject,
		MUIA_ScaDtpic_Name, (ULONG) "THEME:prefs/plugins/palette",
		MUIA_ScaDtpic_FailIfUnavailable, TRUE,
		End; //DataTypesMCCObject

	if (NULL == img)
		img = IMG(Palette, PALETTE);	// use built-in fallback image

	return img;
}


static void InitHooks(struct PalettePrefsInst *inst)
{
	ULONG n;

	for (n=0; n<HOOKNDX_MAX; n++)
		{
		inst->ppb_Hooks[n] = PalettePrefsHooks[n];
		inst->ppb_Hooks[n].h_Data = inst;
		}
}


static BOOL OpenLibraries(void)
{
	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (!DOSBase)
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;
#endif

	d1(kprintf(__FUNC__ "/%ld:\n", __LINE__));

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	MUIMasterBase = OpenLibrary("zune.library", 0);
	if (NULL == MUIMasterBase)
		MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
	if (NULL == IMUIMaster)
		return FALSE;
#endif

	IFFParseBase = OpenLibrary("iffparse.library", 36);
	if (NULL == IFFParseBase)
		return FALSE;
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *)GetInterface((struct Library *)IFFParseBase, "main", 1, NULL);
	if (NULL == IIFFParse)
		return FALSE;
#endif

	IconBase = OpenLibrary("icon.library", 0);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
	if (NULL == IIcon)
		return FALSE;
#endif

	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
	if (NULL == GfxBase)
		return FALSE;
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		return FALSE;
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary(UTILITYNAME, 39);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* __GNUC__ && !__MORPHOS__ && ! __amigaos4__ */

	DataTypesBase = OpenLibrary("datatypes.library", 39);
	if (NULL == DataTypesBase)
		return FALSE;
#ifdef __amigaos4__
	IDataTypes = (struct DataTypesIFace *)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
	if (NULL == IDataTypes)
		return FALSE;
#endif

#ifdef __amigaos4__
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);
#ifdef __amigaos4__
	if (LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
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
	struct ScalosPalette_LocaleInfo li;

	li.li_Catalog = PalettePrefsCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetScalosPaletteString(&li, MsgId);
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

#if 0
static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}
#endif

//----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

DISPATCHER(myNList)
{
	struct PalettePrefsInst *inst = NULL;
	ULONG result;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf(__FUNC__ "/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __LINE__, MenuObj));
		d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->ppb_Hooks[MenuHookIndex];

		d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __LINE__, MenuHook));
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

DISPATCHER(NewColorAdjust)
{
	struct PalettePrefsInst *inst = NULL;
	ULONG Result;

	d1(kprintf( "%s/%s/%ld: START MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case MUIM_DragQuery:
		{
		struct MUIP_DragQuery *dq = (struct MUIP_DragQuery *) msg;

		get(obj, MUIA_UserData, &inst);

		if (inst->ppb_Objects[OBJNDX_WBColorsList] == dq->obj)
			Result = MUIV_DragQuery_Accept;
		else
			Result = MUIV_DragQuery_Refuse;
		}
		break;

	case MUIM_DragDrop:
		{
		struct MUIP_DragDrop *dd = (struct MUIP_DragDrop *) msg;
		APTR activeEntry = NULL;

		get(obj, MUIA_UserData, &inst);

		DoMethod(dd->obj, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &activeEntry);

		if (activeEntry)
			set(inst->ppb_Objects[OBJNDX_ColorAdjust], MUIA_Coloradjust_RGB, (ULONG) activeEntry);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	d1(kprintf( "%s/%s/%ld: END Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}
DISPATCHER_END


DISPATCHER(NewPalettePenList)
{
	struct PalettePrefsInst *inst = NULL;
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_DragQuery:
		{
		struct MUIP_DragQuery *dq = (struct MUIP_DragQuery *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);

		if (inst->ppb_Objects[OBJNDX_WBColorsList] == dq->obj || inst->ppb_Objects[OBJNDX_AllocatedPensList] == dq->obj)
			Result = MUIV_DragQuery_Accept;
		else
			Result = MUIV_DragQuery_Refuse;
		}
		break;

	case MUIM_DragDrop:
		{
		struct MUIP_DragDrop *dd = (struct MUIP_DragDrop *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);

		if (inst->ppb_Objects[OBJNDX_AllocatedPensList] == dd->obj)
			{
			// Drag-sorting pen list
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
			SetChangedFlag(inst, TRUE);
			Result = DoSuperMethodA(cl, obj, msg);
			}
		else
			{
			LONG DropMark = 0;
			LONG Entry;

			get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_DropMark, &DropMark);
			set(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Quiet, TRUE);

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropMark=%ld\n", __LINE__, DropMark));

			Entry = MUIV_NList_NextSelected_Start;

			do	{
				DoMethod(dd->obj, MUIM_NList_NextSelected, &Entry);

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropMark=%ld\n", __LINE__, Entry));

				if (Entry != MUIV_NList_NextSelected_End)
					{
					struct ScalosPenListEntry *snle;

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
					SetChangedFlag(inst, TRUE);

					DoMethod(dd->obj, MUIM_NList_GetEntry, Entry, &snle);
					DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList],
						MUIM_NList_InsertSingle, snle, DropMark);

					if (MUIV_NList_Insert_Bottom != DropMark)
						DropMark++;

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropMark=%ld\n", __LINE__, DropMark));
					}
				} while (Entry != MUIV_NList_NextSelected_End);

			set(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Quiet, FALSE);

			Result = 0;
			}
		}
		break;

	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf(__FUNC__ "/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __LINE__, MenuObj));
		d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->ppb_Hooks[MenuHookIndex];

		d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END


DISPATCHER(NewScalosPenList)
{
	struct PalettePrefsInst *inst = NULL;
	ULONG Result;

	switch (msg->MethodID)
		{
	case MUIM_DragQuery:
		{
		struct MUIP_DragQuery *dq = (struct MUIP_DragQuery *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);

		if (inst->ppb_Objects[OBJNDX_ScalosPenList] == dq->obj || inst->ppb_Objects[OBJNDX_AllocatedPensList] == dq->obj)
			Result = MUIV_DragQuery_Accept;
		else
			Result = MUIV_DragQuery_Refuse;
		}
		break;

	case MUIM_DragDrop:
		{
		LONG DropMark = 0;
		struct MUIP_DragDrop *dd = (struct MUIP_DragDrop *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);
		get(obj, MUIA_NList_DropMark, &DropMark);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropMark=%ld\n", __LINE__, DropMark));

		if (DropMark >= 0)
			{
			if (inst->ppb_Objects[OBJNDX_AllocatedPensList] == dd->obj)
				{
				struct ScalosPenListEntry *snle;
				LONG Entry = 0;
				ULONG OldPenNr;

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
				SetChangedFlag(inst, TRUE);

				get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Active, &Entry);
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Entry=%ld  DropMark=%ld\n", __LINE__, Entry, DropMark));

				DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, DropMark, &snle);

				OldPenNr = snle->snle_PenNr;
				snle->snle_PenNr = Entry;

				DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, DropMark);

				// Redraw "allocated pens" entry of old and new pen, since reference counts have changed
				DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Redraw, OldPenNr);
				DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Redraw, snle->snle_PenNr);
				}
			else
				{
				struct ScalosPenListEntry *snleActive = NULL;

				DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &snleActive);
				if (snleActive)
					{
					struct ScalosPenListEntry *snleDest;
					ULONG OldPenNr;

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropMark=%ld\n", __LINE__, DropMark));

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
					SetChangedFlag(inst, TRUE);

					DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, DropMark, &snleDest);

					OldPenNr = snleDest->snle_PenNr;
					snleDest->snle_PenNr = snleActive->snle_PenNr;

					DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, DropMark);

					// Redraw "allocated pens" entry of old and new pen, since reference counts have changed
					DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Redraw, OldPenNr);
					DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Redraw, snleDest->snle_PenNr);
					}
				}
			}

		Result = 0;
		}
		break;

	case MUIM_NList_DropType:
		{
		struct MUIP_NList_DropType *mdt = (struct MUIP_NList_DropType *) msg;

		Result = DoSuperMethodA(cl, obj, msg);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: DropType=%ld\n", __LINE__, *mdt->type));

		if (MUIV_NList_DropType_None != *mdt->type)
			*mdt->type = MUIV_NList_DropType_Onto;
		}
		break;

	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf(__FUNC__ "/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __LINE__, MenuObj));
		d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->ppb_Hooks[MenuHookIndex];

		d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static APTR AllocatedPensListConstructFunc(struct Hook *hook, APTR memPool,
	struct PensListEntry *spleNew)
{
	struct PensListEntry *sple = AllocPooled(memPool, sizeof(struct PensListEntry));

	if (sple)
		{
//		struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

		*sple = *spleNew;
		}

	return sple;
}

static void AllocatedPensListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry)
{
	if (Entry)
		{
		FreePooled(memPool, Entry, sizeof(struct PensListEntry));
		}
}

static ULONG AllocatedPensListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm)
{
	if (ndm->entry)
		{
		struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
		struct PensListEntry *sple = (struct PensListEntry *) ndm->entry;
		static char Buffer[80];
		STRPTR lp = Buffer;

		sprintf(lp, "%ld", (long)sple->sple_Index);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: lp=<%s>\n", __LINE__, lp));
		ndm->strings[0] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "\033I[2:%08lx %08lx %08lx]",
			(long)sple->sple_Red, (long)sple->sple_Green, (long)sple->sple_Blue);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: lp=<%s>\n", __LINE__, lp));
		ndm->strings[1] = lp;
		lp += 1 + strlen(lp);

		// Ref. Count
		sprintf(lp, "%ld", (long)GetPenReferenceCount(inst, sple->sple_Index));
		ndm->strings[2] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Red >> 24));
		ndm->strings[3] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Green >> 24));
		ndm->strings[4] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Blue >> 24));
		ndm->strings[5] = lp;
		}
	else
		{
		// display titles
		ndm->strings[0] = GetLocString(MSGID_PEN);
		ndm->strings[1] = "";
		ndm->strings[2] = GetLocString(MSGID_PEN_REFERENCE_COUNT);
		ndm->strings[3] = GetLocString(MSGID_RED);
		ndm->strings[4] = GetLocString(MSGID_GREEN);
		ndm->strings[5] = GetLocString(MSGID_BLUE);
		}

	return 0;
}

static LONG AllocatedPensListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	const struct PensListEntry *sple1 = (const struct PensListEntry *) ncm->entry1;
	const struct PensListEntry *sple2 = (const struct PensListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by index
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = sple2->sple_Index - sple1->sple_Index;
			else
				Result = sple1->sple_Index - sple2->sple_Index;
			break;
		case 1:		// sort by RGB
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = RGBComp(sple2, sple1);
			else
				Result = RGBComp(sple1, sple2);
			break;
		case 2:		// sort by reference count
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = GetPenReferenceCount(inst, sple2->sple_Index) - GetPenReferenceCount(inst, sple1->sple_Index);
			else
				Result = GetPenReferenceCount(inst, sple1->sple_Index) - GetPenReferenceCount(inst, sple2->sple_Index);
			break;
		case 3:		// sort by Red
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Red, sple1->sple_Red);
			else
				Result = ColorComp(sple1->sple_Red, sple2->sple_Red);
			break;
		case 4:		// sort by Green
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Green, sple1->sple_Green);
			else
				Result = ColorComp(sple1->sple_Green, sple2->sple_Green);
			break;
		case 5:		// sort by Blue
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Blue, sple1->sple_Blue);
			else
				Result = ColorComp(sple1->sple_Blue, sple2->sple_Blue);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by index
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = sple2->sple_Index - sple1->sple_Index;
				else
					Result = sple1->sple_Index - sple2->sple_Index;
				break;
			case 1:		// sort by RGB
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = RGBComp(sple2, sple1);
				else
					Result = RGBComp(sple1, sple2);
				break;
			case 2:		// sort by reference count
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = GetPenReferenceCount(inst, sple2->sple_Index) - GetPenReferenceCount(inst, sple1->sple_Index);
				else
					Result = GetPenReferenceCount(inst, sple1->sple_Index) - GetPenReferenceCount(inst, sple2->sple_Index);
				break;
			case 3:		// sort by Red
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Red, sple1->sple_Red);
				else
					Result = ColorComp(sple1->sple_Red, sple2->sple_Red);
				break;
			case 4:		// sort by Green
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Green, sple1->sple_Green);
				else
					Result = ColorComp(sple1->sple_Green, sple2->sple_Green);
				break;
			case 5:		// sort by Blue
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Blue, sple1->sple_Blue);
				else
					Result = ColorComp(sple1->sple_Blue, sple2->sple_Blue);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static APTR WBPensListConstructFunc(struct Hook *hook, APTR memPool,
	struct PensListEntry *spleNew)
{
	struct PensListEntry *sple = AllocPooled(memPool, sizeof(struct PensListEntry));

	if (sple)
		{
		*sple = *spleNew;
		}

	return sple;
}

static void WBPensListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry)
{
	FreePooled(memPool, Entry, sizeof(struct PensListEntry));
}

static ULONG WBPensListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm)
{
	if (ndm->entry)
		{
		struct PensListEntry *sple = (struct PensListEntry *) ndm->entry;
		static char Buffer[80];
		STRPTR lp = Buffer;

		sprintf(lp, "%ld", (long)sple->sple_Index);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: lp=<%s>\n", __LINE__, lp));
		ndm->strings[0] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "\x1bI[2:%08lx %08lx %08lx]",
			(long)sple->sple_Red, (long)sple->sple_Green, (long)sple->sple_Blue);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: lp=<%s>\n", __LINE__, lp));
		ndm->strings[1] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Red >> 24));
		ndm->strings[2] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Green >> 24));
		ndm->strings[3] = lp;
		lp += 1 + strlen(lp);

		sprintf(lp, "%ld", (long)(sple->sple_Blue >> 24));
		ndm->strings[4] = lp;
		}
	else
		{
		// display titles
		ndm->strings[0] = GetLocString(MSGID_PEN);
		ndm->strings[1] = "";
		ndm->strings[2] = GetLocString(MSGID_RED);
		ndm->strings[3] = GetLocString(MSGID_GREEN);
		ndm->strings[4] = GetLocString(MSGID_BLUE);
		}

	return 0;
}

static LONG WBPensListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
//	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	const struct PensListEntry *sple1 = (const struct PensListEntry *) ncm->entry1;
	const struct PensListEntry *sple2 = (const struct PensListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		// primary sorting
		switch (col1)
			{
		case 0:		// sort by index
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = sple2->sple_Index - sple1->sple_Index;
			else
				Result = sple1->sple_Index - sple2->sple_Index;
			break;
		case 1:		// sort by RGB
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = RGBComp(sple2, sple1);
			else
				Result = RGBComp(sple1, sple2);
			break;
		case 2:		// sort by Red
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Red, sple1->sple_Red);
			else
				Result = ColorComp(sple1->sple_Red, sple2->sple_Red);
			break;
		case 3:		// sort by Green
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Green, sple1->sple_Green);
			else
				Result = ColorComp(sple1->sple_Green, sple2->sple_Green);
			break;
		case 4:		// sort by Blue
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = ColorComp(sple2->sple_Blue, sple1->sple_Blue);
			else
				Result = ColorComp(sple1->sple_Blue, sple2->sple_Blue);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by index
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = sple2->sple_Index - sple1->sple_Index;
				else
					Result = sple1->sple_Index - sple2->sple_Index;
				break;
			case 1:		// sort by RGB
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = RGBComp(sple2, sple1);
				else
					Result = RGBComp(sple1, sple2);
				break;
			case 2:		// sort by Red
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Red, sple1->sple_Red);
				else
					Result = ColorComp(sple1->sple_Red, sple2->sple_Red);
				break;
			case 3:		// sort by Green
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Green, sple1->sple_Green);
				else
					Result = ColorComp(sple1->sple_Green, sple2->sple_Green);
				break;
			case 4:		// sort by Blue
				if (ncm->sort_type2 & MUIV_NList_TitleMark_TypeMask)
					Result = ColorComp(sple2->sple_Blue, sple1->sple_Blue);
				else
					Result = ColorComp(sple1->sple_Blue, sple2->sple_Blue);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static APTR ScalosPenListConstructFunc(struct Hook *hook, APTR memPool, struct ScalosPenListEntry *msg)
{
	struct ScalosPenListEntry *snle = AllocPooled(memPool, sizeof(struct ScalosPenListEntry));

	if (snle)
		{
		*snle = *msg;
		}

	return snle;
}

static void ScalosPenListDestructFunc(struct Hook *hook, APTR memPool, APTR Entry)
{
	FreePooled(memPool, Entry, sizeof(struct ScalosPenListEntry));
}

static ULONG ScalosPenListDisplayFunc(struct Hook *hook, Object *obj, struct NList_DisplayMessage *ndm)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	if (ndm->entry)
		{
		struct PensListEntry *sple = NULL;
		struct ScalosPenListEntry *snle = (struct ScalosPenListEntry *) ndm->entry;
		static char Buffer[80];
		STRPTR lp = Buffer;


		if (PEN_UNDEFINED != snle->snle_PenNr)
			sprintf(lp, "%d", snle->snle_PenNr);
		else
			strcpy(lp, "??");
		ndm->strings[0] = lp;
		lp += 1 + strlen(lp);

		if (PEN_UNDEFINED != snle->snle_PenNr)
			DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_GetEntry, snle->snle_PenNr, &sple);

		if (sple)
			{
			sprintf(lp, "\x1bI[2:%08lx %08lx %08lx]", 
				(long)sple->sple_Red, (long)sple->sple_Green, (long)sple->sple_Blue);
			}
		else
			{
			strcpy(lp, "");
			}
		ndm->strings[1] = lp;

		ndm->strings[2] = (STRPTR) snle->snle_PenName;
		}
	else
		{
		// display titles
		ndm->strings[0] = GetLocString(MSGID_PEN);
		ndm->strings[1] = "";
		ndm->strings[2] = GetLocString(MSGID_PENNAMENAME);
		}

	return 0;
}

static LONG ScalosPenListCompareFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *ncm)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	const struct ScalosPenListEntry *snle1 = (const struct ScalosPenListEntry *) ncm->entry1;
	const struct ScalosPenListEntry *snle2 = (const struct ScalosPenListEntry *) ncm->entry2;
	LONG col1 = ncm->sort_type & MUIV_NList_TitleMark_ColMask;
	LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
	LONG Result = 0;

	if (ncm->sort_type != MUIV_NList_SortType_None)
		{
		struct PensListEntry *sple1 = NULL;
		struct PensListEntry *sple2 = NULL;

		if (PEN_UNDEFINED != snle1->snle_PenNr)
			DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_GetEntry, snle1->snle_PenNr, &sple1);
		if (PEN_UNDEFINED != snle2->snle_PenNr)
			DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_GetEntry, snle2->snle_PenNr, &sple2);

		// primary sorting
		switch (col1)
			{
		case 0:		// sort by pen number
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = snle2->snle_PenNr - snle1->snle_PenNr;
			else
				Result = snle1->snle_PenNr - snle2->snle_PenNr;
			break;
		case 1:		// sort by RGB
			if (sple2 && sple1)
				{
				if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
					Result = RGBComp(sple2, sple1);
				else
					Result = RGBComp(sple1, sple2);
				}
			break;
		case 2:		// sort by name
			if (ncm->sort_type & MUIV_NList_TitleMark_TypeMask)
				Result = Stricmp(snle2->snle_PenName, snle1->snle_PenName);
			else
				Result = Stricmp(snle1->snle_PenName, snle2->snle_PenName);
			break;
		default:
			break;
			}

		if (0 == Result && col1 != col2)
			{
			// Secondary sorting
			switch (col2)
				{
			case 0:		// sort by pen number
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = snle1->snle_PenNr - snle2->snle_PenNr;
				else
					Result = snle2->snle_PenNr - snle1->snle_PenNr;
				break;
			case 1:		// sort by RGB
				if (sple1 && sple2)
					{
					if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
						Result = RGBComp(sple2, sple1);
					else
						Result = RGBComp(sple1, sple2);
					}
				break;
			case 2:		// sort by name
				if (ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask)
					Result = Stricmp(snle2->snle_PenName, snle1->snle_PenName);
				else
					Result = Stricmp(snle1->snle_PenName, snle2->snle_PenName);
				break;
			default:
				break;
				}
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static void GetWBColorsFunc(struct Hook *hook, Object *pop, Object *win)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	if (inst->ppb_WBScreen)
		{
		ULONG nColors = inst->ppb_WBScreen->BitMap.Depth;
		struct ColorMap *cm = inst->ppb_WBScreen->ViewPort.ColorMap;
		struct RGB *RGBTable;

		if (nColors > 8)
			nColors = cm->Count;
		else
			nColors = 1L << nColors;

		DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList], MUIM_NList_Clear);

		RGBTable = AllocVec(nColors * sizeof(struct RGB), MEMF_PUBLIC);
		if (RGBTable)
			{
			ULONG n;

			GetRGB32(cm, 0, nColors, (ULONG *) RGBTable);

			for (n=0; n<nColors; n++)
				{
				struct PensListEntry sple;

				sple.sple_Index = n;
				sple.sple_Red   = RGBTable[n].rgb_Red;
				sple.sple_Green = RGBTable[n].rgb_Green;
				sple.sple_Blue  = RGBTable[n].rgb_Blue;

				DoMethod(inst->ppb_Objects[OBJNDX_WBColorsList],
					MUIM_NList_InsertSingle, &sple, MUIV_NList_Insert_Sorted);
				}

			FreeVec(RGBTable);
			}
		}
}

//----------------------------------------------------------------------------

static APTR AddPenFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	ULONG *rgb = NULL;
	struct PensListEntry sple;

	sple.sple_Index = 0;

	get(inst->ppb_Objects[OBJNDX_ColorAdjust], MUIA_Coloradjust_RGB, &rgb);
	get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Entries, &sple.sple_Index );

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: rgb=%08lx  %08lx  %08lx\n", __LINE__, rgb[0], rgb[1], rgb[2]));

	sple.sple_Red   = rgb[0];
	sple.sple_Green = rgb[1];
	sple.sple_Blue  = rgb[2];

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
	SetChangedFlag(inst, TRUE);

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList],
		MUIM_NList_InsertSingle, &sple, MUIV_NList_Insert_Bottom);
	set(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Active, MUIV_NList_Active_Bottom);

	UpdatePalettePenCount(inst);

	return 0;
}

static APTR SelectPalettePenFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	struct PensListEntry *sple;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &sple);

	if (sple)
		set(inst->ppb_Objects[OBJNDX_ColorAdjust], MUIA_Coloradjust_RGB, (ULONG) sple);

	set(inst->ppb_Objects[OBJNDX_DeleteButton], MUIA_Disabled, NULL == sple);
	set(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIA_NList_Active, MUIV_NList_Active_Off);

	return 0;
}

static APTR SelectScalosPenFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	struct ScalosPenListEntry *snle = NULL;

	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &snle);

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: snle=%08lx\n", __LINE__, snle));

	if (snle)
		{
		set(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Active, snle->snle_PenNr);
		}

	return 0;
}

static APTR ChangePenColorFunc(struct Hook *hook, Object *o, ULONG **msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	struct PensListEntry *sple = NULL;
	ULONG *rgb = *msg;
	LONG ActiveEntryNr = 0;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &sple);
	get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Active, &ActiveEntryNr);

	if (sple)
		{
		if (sple->sple_Red != rgb[0] || sple->sple_Green != rgb[1] || sple->sple_Blue != rgb[2])
			{
			ULONG n, nEntries = 0;

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
			SetChangedFlag(inst, TRUE);

			sple->sple_Red = rgb[0];
			sple->sple_Green = rgb[1];
			sple->sple_Blue = rgb[2];

			DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Redraw, MUIV_NList_Redraw_Active);

			get(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIA_NList_Entries, &nEntries);

			for (n=0; n<nEntries; n++)
				{
				struct ScalosPenListEntry *snle;

				DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, n, &snle);

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: snle_PenNr=%ld ActiveEntryNr=%ld\n", __LINE__, snle->snle_PenNr, ActiveEntryNr));

				if (snle && snle->snle_PenNr == ActiveEntryNr)
					DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, n);
				}
			}
		}

	return 0;
}

static APTR ResetToDefaultsFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	ULONG n;
	LONG EntryNr;
	struct ScalosPenListEntry *snle;
	const struct PenListInit *pli;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Clear);

	for (n=0; n<sizeof(DefaultPaletteEntries)/sizeof(DefaultPaletteEntries[0]); n++)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList],
			MUIM_NList_InsertSingle, &DefaultPaletteEntries[n], MUIV_NList_Insert_Sorted);
		}

	for (pli=PenListContents, EntryNr=0; pli->pli_PenIndex; pli++, EntryNr++)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, EntryNr, &snle);

		if (NULL == snle)
			break;

		snle->snle_PenIndex = pli->pli_PenIndex;
		snle->snle_PenNr = pli->pli_PenNr;
		snle->snle_PenName = GetLocString(pli->pli_PenNameID);
		}

	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, MUIV_NList_Redraw_All);

	SetChangedFlag(inst, TRUE);

	return NULL;
}

static APTR AboutFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	MUI_Request(inst->ppb_Objects[OBJNDX_APP_Main], inst->ppb_Objects[OBJNDX_WIN_Main], 0, NULL, 
		GetLocString(MSGID_ABOUTREQOK), 
		GetLocString(MSGID_ABOUTREQFORMAT), 
		MajorVersion, MinorVersion, COMPILER_STRING, CURRENTYEAR);

	return 0;
}

static APTR OpenFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	if (NULL == inst->ppb_LoadReq)
		{
		inst->ppb_LoadReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Palette.pre",
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

				DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Clear);

				ReadPrefsFile(inst, inst->ppb_LoadReq->fr_File, FALSE);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}

static APTR LastSavedFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Clear);

	ReadPrefsFile(inst, "ENVARC:Scalos/Palette13.prefs", FALSE);

	SetChangedFlag(inst, TRUE);

	return 0;
}

static APTR RestoreFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Clear);

	ReadPrefsFile(inst, "ENV:Scalos/Palette13.prefs", FALSE);

	SetChangedFlag(inst, TRUE);

	return 0;
}

static APTR SaveAsFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;

	if (NULL == inst->ppb_SaveReq)
		{
		inst->ppb_SaveReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Palette.pre",
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

static APTR DeletePenFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	LONG ActiveEntryNr = MUIV_NList_Active_Off;
	LONG PenCount = 0;

	get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Active, &ActiveEntryNr);
	get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_NList_Entries, &PenCount);

	if (MUIV_NList_Active_Off != ActiveEntryNr)
		{
		LONG n;
		LONG Entries = 0;

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));
		SetChangedFlag(inst, TRUE);

		get(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIA_NList_Entries, &Entries);

		// adjust all pen numbers in inst->ppb_Objects[OBJNDX_ScalosPenList]

		for (n=0; n<Entries; n++)
			{
			struct ScalosPenListEntry *snle;

			DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, n, &snle);

			if (snle->snle_PenNr == ActiveEntryNr)
				snle->snle_PenNr = PEN_UNDEFINED;
			if (snle->snle_PenNr > ActiveEntryNr)
				snle->snle_PenNr--;
			}

		DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Remove, MUIV_NList_Remove_Active);
		DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, MUIV_NList_Redraw_All);
		}

	UpdatePalettePenCount(inst);

	return 0;
}


static APTR ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG MenuHookIndex = 0;
	struct Hook *MenuHook = NULL;

	d1(kprintf(__FUNC__ "/%ld: MenuObj=%08lx\n", __LINE__, MenuObj));
	d1(kprintf(__FUNC__ "/%ld: msg=%08lx *msg=%08lx\n", __LINE__, msg, *((ULONG *) msg)));

	get(MenuObj, MUIA_UserData, &MenuHookIndex);

	if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
		MenuHook = &inst->ppb_Hooks[MenuHookIndex];

	d1(kprintf(__FUNC__ "/%ld: MenuHook=%08lx\n", __LINE__, MenuHook));
	if (MenuHook)
		DoMethod(inst->ppb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

	return NULL;
}

//----------------------------------------------------------------------------

static void AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct PalettePrefsInst *inst = (struct PalettePrefsInst *) hook->h_Data;
	struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == iMsg->Class)
		{
		DoMethod(inst->ppb_Objects[OBJNDX_APP_Main], MUIM_Application_CheckRefresh);
		}
}

//----------------------------------------------------------------------------

static void InitPenList(struct PalettePrefsInst *inst)
{
	const struct PenListInit *pli;

	for (pli=PenListContents; pli->pli_PenIndex; pli++)
		{
		struct ScalosPenListEntry snle;

		snle.snle_PenIndex = pli->pli_PenIndex;
		snle.snle_PenNr = pli->pli_PenNr;
		snle.snle_PenName = GetLocString(pli->pli_PenNameID);

		DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList],
			MUIM_NList_InsertSingle, &snle, MUIV_NList_Insert_Sorted);
		}
}


static LONG ReadPrefsFile(struct PalettePrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet)
{
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;

	DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_NList_Clear);

	do	{
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

		Result = StopChunks(iff, StopChunkList, 2);
		if (RETURN_OK != Result)
			break;

		while (RETURN_OK == Result)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%ld\n", __LINE__, Result));
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				break;

			if (ID_PENS == cn->cn_ID)
				{
				LONG Actual;
				WORD Pen, PenCount;

				Actual = ReadChunkBytes(iff, &PenCount, sizeof(PenCount));
				if (Actual < 0)
					break;

				PenCount = SCA_BE2WORD(PenCount);

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: PenCount=%ld\n", __LINE__, PenCount));

				if (PenCount < 2)
					break;

				for (Pen=0; Pen<PenCount; Pen++)
					{
					struct ScalosPen sPen;

					if (ReadChunkBytes(iff, &sPen, sizeof(sPen)) >= 0)
						{
						sPen.sp_pentype = SCA_BE2WORD(sPen.sp_pentype);
						sPen.sp_pen = SCA_BE2WORD(sPen.sp_pen);
						SearchAndSetPen(inst, &sPen);
						}
					}
				}
			else if (ID_SPAL == cn->cn_ID)
				{
				LONG n, ScreenColorListEntries;
				LONG Actual;

				Actual = ReadChunkBytes(iff, &ScreenColorListEntries, sizeof(ScreenColorListEntries));
				if (Actual < 0)
					break;
				ScreenColorListEntries = SCA_BE2LONG(ScreenColorListEntries);
				ScreenColorListEntries >>= 16;

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ScreenColorListEntries=%ld\n", __LINE__, ScreenColorListEntries));

				for (n=0; n<ScreenColorListEntries; n++)
					{
					struct PensListConstruct splc;
					struct PensListEntry sple;

					Actual = ReadChunkBytes(iff, &splc, sizeof(splc));

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Read Palette Size=%ld  Actual=%ld\n", __LINE__, cn->cn_Size, Actual));
					if (Actual < 0)
						{
						Result = IoErr();
						break;
						}

					sple.sple_Index = n;
					sple.sple_Red = SCA_BE2LONG(splc.splc_Red);
					sple.sple_Green = SCA_BE2LONG(splc.splc_Green);
					sple.sple_Blue = SCA_BE2LONG(splc.splc_Blue);

					DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList],
						MUIM_NList_InsertSingle, &sple, MUIV_NList_Insert_Sorted);
					}
				}
			}

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%ld\n", __LINE__, Result));
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

	DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_Redraw, MUIV_NList_Redraw_All);
	UpdatePalettePenCount(inst);

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%ld\n", __LINE__, Result));

	return Result;
}


static void SearchAndSetPen(struct PalettePrefsInst *inst, const struct ScalosPen *sPen)
{
	ULONG n = 0;

	while (1)
		{
		struct ScalosPenListEntry *snle = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, n, &snle);
		if (NULL == snle)
			break;

		if (snle->snle_PenIndex == sPen->sp_pentype)
			{
			snle->snle_PenNr = sPen->sp_pen;
			}

		n++;
		}
}


static LONG WritePrefsFile(struct PalettePrefsInst *inst, CONST_STRPTR Filename)
{
	struct IFFHandle *iff;
	LONG Result;
	BOOL IffOpen = FALSE;

	do	{
		static const struct PrefHeader prefHeader = { 1, 0, 0L };
		ULONG n, Entries;
		UWORD wEntries;

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
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%ld\n", __LINE__, Result));

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

			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: Result=%ld\n", __LINE__, Result));
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

		Result = PushChunk(iff, 0, ID_SPAL, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_List_Entries, &Entries);

		Entries <<= 16;
		Entries = SCA_LONG2BE(Entries);

		if (WriteChunkBytes(iff, &Entries, sizeof(Entries)) < 0)
			{
			Result = IoErr();
			break;
			}

		n = 0;
		while (RETURN_OK == Result)
			{
			struct PensListConstruct splc;
			struct PensListEntry *sple = NULL;

			DoMethod(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIM_List_GetEntry, n, &sple);
			if (NULL == sple)
				break;

			splc.splc_Red = SCA_LONG2BE(sple->sple_Red);
			splc.splc_Green = SCA_LONG2BE(sple->sple_Green);
			splc.splc_Blue = SCA_LONG2BE(sple->sple_Blue);

			if (WriteChunkBytes(iff, &splc, sizeof(struct PensListConstruct)) < 0)
				{
				Result = IoErr();
				break;
				}
			n++;
			}
		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, 0, ID_PENS, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		get(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIA_List_Entries, &Entries);

		wEntries = Entries;
		wEntries = SCA_WORD2BE(wEntries);

		if (WriteChunkBytes(iff, &wEntries, sizeof(wEntries)) < 0)
			{
			Result = IoErr();
			break;
			}

		n = 0;
		while (RETURN_OK == Result)
			{
			struct ScalosPen sPen;
			struct ScalosPenListEntry *snle = NULL;

			DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_List_GetEntry, n, &snle);
			if (NULL == snle)
				break;

			sPen.sp_pentype = SCA_WORD2BE(snle->snle_PenIndex);
			sPen.sp_pen = SCA_WORD2BE(snle->snle_PenNr);

			if (WriteChunkBytes(iff, &sPen, sizeof(sPen)) < 0)
				{
				Result = IoErr();
				break;
				}
			n++;
			}
		if (RETURN_OK != Result)
			break;

		Result = PopChunk(iff);
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


static LONG SaveIcon(struct PalettePrefsInst *inst, CONST_STRPTR IconName)
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
		8192
		};

	icon = allocIcon = GetDiskObject("ENV:sys/def_ScaPalettePrefs");
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


static void UpdatePalettePenCount(struct PalettePrefsInst *inst)
{
	ULONG allocatedPens = 0;
	UWORD availablePens;

	availablePens = 1 + inst->ppb_WBScreen->ViewPort.ColorMap->PalExtra->pe_SharableColors;

	if (inst->ppb_Objects[OBJNDX_AllocatedPensList])
		get(inst->ppb_Objects[OBJNDX_AllocatedPensList], MUIA_List_Entries, &allocatedPens);
	else
		allocatedPens = 0;

	sprintf(inst->ppb_PalettePenListTitle, GetLocString(MSGID_TITLE_PALETTEPENLIST), 
		allocatedPens, availablePens);

	if (inst->ppb_Objects[OBJNDX_AllocatedPensListTitleObj])
		set(inst->ppb_Objects[OBJNDX_AllocatedPensListTitleObj], MUIA_Text_Contents, (ULONG) inst->ppb_PalettePenListTitle);

	set(inst->ppb_Objects[OBJNDX_NewButton], MUIA_Disabled, allocatedPens >= availablePens);
}

//----------------------------------------------------------------------------

static ULONG GetPenReferenceCount(struct PalettePrefsInst *inst, ULONG PenNr)
{
	ULONG Entries = 0;
	ULONG References = 0;
	ULONG n;

	get(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIA_NList_Entries, &Entries);

	for (n=0; n<Entries; n++)
		{
		const struct ScalosPenListEntry *snle = NULL;

		DoMethod(inst->ppb_Objects[OBJNDX_ScalosPenList], MUIM_NList_GetEntry, n, &snle);
		if (snle)
			{
			if (snle->snle_PenNr == PenNr)
				References++;
			}
		}

	return References;
}

//----------------------------------------------------------------------------

static void SetChangedFlag(struct PalettePrefsInst *inst, BOOL changed)
{
	if (changed != inst->ppb_Changed)
		{
		set(inst->ppb_Objects[OBJNDX_Lamp_Changed], 
			MUIA_Lamp_Color, changed ? MUIV_Lamp_Color_Ok : MUIV_Lamp_Color_Off);
		inst->ppb_Changed = changed;
		}
}

//---------------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	MajorVersion = PluginBase->pl_LibNode.lib_Version;
	MinorVersion = PluginBase->pl_LibNode.lib_Revision;

	d1(kprintf("%s/%s/%ld:  Signature=%08lx\n", __FILE__, __FUNC__, __LINE__, Signature));

	if (0x4711 == Signature++)
		{
		d1(kprintf(__FUNC__ "/%ld:   PluginBase=%08lx  procName=<%s>\n", __LINE__, \
			PluginPrefsBase, FindTask(NULL)->tc_Node.ln_Name));

		if (!OpenLibraries())
			return FALSE;

#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			return FALSE;
#endif

		PalettePrefsClass = MUI_CreateCustomClass(&PluginBase->pl_LibNode, MUIC_Group,
				NULL, sizeof(struct PalettePrefsInst), DISPATCHER_REF(PalettePrefs));

		d1(kprintf(__FUNC__ "/%ld:  PalettePrefsClass=%08lx\n", __LINE__, PalettePrefsClass));
		if (NULL == PalettePrefsClass)
			return FALSE;

		d1(kprintf(__FUNC__ "/%ld: PalettePrefsCatalog=%08lx\n", __LINE__, PalettePrefsCatalog));
		d1(kprintf(__FUNC__ "/%ld: LocaleBase=%08lx\n", __LINE__, LocaleBase));

		if (LocaleBase)
			{
			d1(kprintf(__FUNC__ "/%ld: PalettePrefsLocale=%08lx\n", __LINE__, PalettePrefsLocale));

			if (NULL == PalettePrefsLocale)
				PalettePrefsLocale = OpenLocale(NULL);

			d1(kprintf(__FUNC__ "/%ld: PalettePrefsLocale=%08lx\n", __LINE__, PalettePrefsLocale));

			if (PalettePrefsLocale)
				{
				d1(kprintf(__FUNC__ "/%ld: PalettePrefsCatalog=%08lx\n", __LINE__, PalettePrefsCatalog));

				if (NULL == PalettePrefsCatalog)
					{
					PalettePrefsCatalog = OpenCatalog(PalettePrefsLocale,
						(STRPTR) "Scalos/ScalosPalette.catalog", NULL);

					d1(kprintf(__FUNC__ "/%ld: \n", __LINE__));
					}
				}
			}

		TranslateNewMenu(ContextMenus);

		NewColorAdjustClass = MUI_CreateCustomClass(NULL, MUIC_Coloradjust, NULL,
			4, DISPATCHER_REF(NewColorAdjust));
		d1(kprintf( "%s/%s/%ld: NewColorAdjustClass=%08lx\n", __FILE__, __FUNC__, __LINE__, NewColorAdjustClass));
		if (NULL == NewColorAdjustClass)
			return FALSE;	// Failure

		NewPalettePenListClass = MUI_CreateCustomClass(NULL, MUIC_NList, NULL,
			4, DISPATCHER_REF(NewPalettePenList));
		if (NULL == NewPalettePenListClass)
			return FALSE;	// Failure

		NewScalosPenListClass = MUI_CreateCustomClass(NULL, MUIC_NList, NULL,
			sizeof(struct NewScalosPenListInst), DISPATCHER_REF(NewScalosPenList));
		if (NULL == NewScalosPenListClass)
			return FALSE;	// Failure

		myNListClass = MUI_CreateCustomClass(NULL, MUIC_NList,
			NULL, 4, DISPATCHER_REF(myNList));
		if (NULL == myNListClass)
			return FALSE;	// Failure

		DataTypesImageClass = InitDtpicClass();
		d1(KPrintF(__FUNC__ "/%ld: DataTypesImageClass=%08lx\n", __LINE__, DataTypesImageClass));
		if (NULL == DataTypesImageClass)
			return FALSE;	// Failure
		}

	d1(kprintf(__FUNC__ "/%ld: Success\n", __LINE__));

	return TRUE;	// Success
}

//----------------------------------------------------------------------------

static LONG ColorComp(ULONG Color1, ULONG Color2)
{
	if (Color1 == Color2)
		return 0;
	if (Color1 > Color2)
		return 1;

	return -1;
}

static LONG RGBComp(const struct PensListEntry *sple1, const struct PensListEntry *sple2)
{
	LONG Result;

	Result = ColorComp(sple1->sple_Red, sple2->sple_Red);
	if (0 == Result)
		Result = ColorComp(sple1->sple_Green, sple2->sple_Green);
	if (0 == Result)
		Result = ColorComp(sple1->sple_Blue, sple2->sple_Blue);

	return Result;
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && ! defined(__amigaos4__)

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif /* !__SASC && !__amigaos4__ */

//----------------------------------------------------------------------------
