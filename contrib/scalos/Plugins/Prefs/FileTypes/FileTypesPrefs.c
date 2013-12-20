// FileTypesPrefs.c
// $Date$
// $Revision$

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
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
#include <libraries/ttengine.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <prefs/prefhdr.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/locale.h>
#include <proto/preferences.h>
#include <proto/iconobject.h>
#define USE_INLINE_STDARG
#include <proto/ttengine.h>
#undef	USE_INLINE_STDARG

#if !defined(__AROS__)
#define	NO_INLINE_STDARG
#endif
#include <proto/muimaster.h>

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/Lamp_mcc.h>

#include <scalos/scalosprefsplugin.h>
#include <scalos/preferences.h>
#include <DefIcons.h>

#include "defs.h"
#include <Year.h> // +jmc+

#include <FontSampleMCC.h>
#include "IconobjectMCC.h"
#include "DataTypesMCC.h"
#include "debug.h"

#include "FileTypesPrefs.h"
#include "FileTypesPrefs_proto.h"

#define	ScalosFileTypes_NUMBERS
#define	ScalosFileTypes_ARRAY
#define	ScalosFileTypes_CODE
#include STR(SCALOSLOCALE)

#include "plugin.h"

//----------------------------------------------------------------------------

#define	ID_MAIN		MAKE_ID('M','A','I','N')
#define	PS_DATA(prefsstruct)	((STRPTR) (prefsstruct)) + sizeof((prefsstruct)->ps_Size)

#define	ENV_FILETYPES_DIR     	"ENV:Scalos/FileTypes"
#define	ENVARC_FILETYPES_DIR    "ENVARC:Scalos/FileTypes"
#define	ENV_DEFICONS_PREFS	"ENV:deficons.prefs"
#define	ENVARC_DEFICONS_PREFS	"ENVARC:deficons.prefs"

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

// local data structures

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_CycleChain, TRUE, \
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, (ULONG) GetLocString(name),\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp	  , (ULONG) GetLocString(HelpText),\
		End

#define KeyButtonHelp2(name,key,HelpText,Weight)\
	TextObject,\
		ButtonFrame,\
		MUIA_CycleChain, TRUE, \
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, (ULONG) GetLocString(name),\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , (ULONG) *GetLocString(key),\
		MUIA_ControlChar  , (ULONG) *GetLocString(key),\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp	  , (ULONG) GetLocString(HelpText),\
		MUIA_Weight	  , (Weight), \
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

struct AddEntryListEntry
	{
	enum FtEntryType ael_EntryType;
	};

struct AddAttrListEntry
	{
	enum ftAttributeType aal_EntryType;
	};

struct EditAttrListEntry
	{
	char eal_ValueString[MAX_ATTRVALUE];
	struct FtAttribute eal_fta;
	};

enum AttrDefaultType
	{
	ATTRDEFTYPE_None,
	ATTRDEFTYPE_ULong,
	ATTRDEFTYPE_LocString,
	ATTRDEFTYPE_String,
	ATTRDEFTYPE_String2,
	ATTRDEFTYPE_PathName,
	ATTRDEFTYPE_FileName,
	ATTRDEFTYPE_FontString,
	ATTRDEFTYPE_TTFontString,
	};

struct AttrDefaultValue
	{
	enum AttrDefaultType avd_Type;
	ULONG avd_ULongValue;

	char *avd_StringValue;

	ULONG (*avd_ConvertValueFromString)(CONST_STRPTR DisplayString);
	};

struct AttributeDef
	{
	enum ftAttributeType atd_Type;

	BOOL atd_Required;

	ULONG atd_ExcludeCount;
	enum ftAttributeType atd_Exclude[10];

	ULONG atd_MinValue;		// Minimum ULONG value or minimal string length
	ULONG atd_MaxValue;		// Maximum ULONG value or maximal string length

	struct AttrDefaultValue atd_DefaultContents;

	struct AttrDefaultValue *atd_PossibleContents;
	ULONG atd_NumberOfValues;
	};

struct EntryAttributes
	{
	enum FtEntryType eat_EntryType;
	const struct AttributeDef *eat_AttrArray;
	ULONG eat_AttrCount;		// number of entries in eat_AttrArray
	};

enum FindDir
	{
	FINDDIR_First = 0,
	FINDDIR_Next,
	FINDDIR_Prev
	};

struct FoundNode
	{
	struct Node fdn_Node;
	Object *fdn_ListTree;
	struct MUI_NListtree_TreeNode *fdn_TreeNode;
	ULONG fdn_Index;
	};

//----------------------------------------------------------------------------

// aus mempools.lib
#if !defined(__amigaos4__) && !defined(__AROS__)
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

// local functions

DISPATCHER_PROTO(FileTypesPrefs);
static Object *CreatePrefsGroup(struct FileTypesPrefsInst *inst);
static Object **CreateSubWindows(Class *cl, Object *o);
static Object *CreatePrefsImage(void);
static void InitHooks(struct FileTypesPrefsInst *inst);

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static void TranslateNewMenu(struct NewMenu *nm);
static void TranslateStringArray(STRPTR *stringArray);

static SAVEDS(APTR) INTERRUPT TreeConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT TreeDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT TreeDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm);

static SAVEDS(APTR) INTERRUPT AttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT AttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT AttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);

static SAVEDS(APTR) INTERRUPT AddEntryConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT AddEntryDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT AddEntryDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);

static SAVEDS(APTR) INTERRUPT AddAttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT AddAttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT AddAttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);

static SAVEDS(APTR) INTERRUPT EditAttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT EditAttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT EditAttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);

static SAVEDS(ULONG) INTERRUPT EditAttrTTFontOpenHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT EditAttrTTFontCloseHookFunc(struct Hook *hook, Object *obj, Msg msg);

static SAVEDS(APTR) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT HideEmptyEntriesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseAllHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandAllHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CopyHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT PasteHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CopyAttrHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CutAttrHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT PasteAttrHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SelectAttributeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT EditAttributeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AboutFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AddEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AddAttributeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT RemoveEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT RemoveAttributeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ChangeAttributeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SelectAttributeValueHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT EditAttributePopAslFileStartHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT EditAttributePopAslPathStartHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
static BOOL IncrementalSearchFileType(struct FileTypesPrefsInst *inst,
	CONST_STRPTR SearchString, enum FindDir dir);
static void BuildFindTree(struct FileTypesPrefsInst *inst,
	Object *ListTree,
	struct MUI_NListtree_TreeNode *list, CONST_STRPTR pattern,
	struct MUI_NListtree_TreeNode *startnode, ULONG *dosearch, BOOL CaseSensitive);
static SAVEDS(void) INTERRUPT ShowFindGroupHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT HideFindGroupHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IncrementalFindFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IncrementalFindNextFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT IncrementalFindPrevFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg);

static LONG ReadPrefs(struct FileTypesPrefsInst *inst,
	CONST_STRPTR LoadName, CONST_STRPTR DefIconPrefsName);
static LONG WritePrefs(struct FileTypesPrefsInst *inst,
	CONST_STRPTR SaveName, CONST_STRPTR DefIconPrefsName, ULONG Flags);

DISPATCHER_PROTO(myNListTree);
DISPATCHER_PROTO(myFileTypesNListTree);
DISPATCHER_PROTO(myNList);
DISPATCHER_PROTO(myFileTypesActionsNList);


static void DisposeAttribute(struct FtAttribute *fta);
static void CleanupAttributes(struct List *AttributesList);
static const struct FtAttribute *FindAttribute(const struct FileTypesListEntry *fte, enum FtEntryType AttrType);
static void RemoveAttribute(struct FileTypesListEntry *fte, enum ftAttributeType AttrType);
static struct FileTypesListEntry *FindChildByType(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tn, enum FtEntryType RequestedType);
static BOOL HasChildren(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn);
static void AddDefaultAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte);
static void SetAddableAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte);
static void UpdateAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte);
static void AddNewAttribute(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte, enum ftAttributeType AttrType);
static void BeginSetMenuDefaultAction(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn);
static void SetMenuDefaultAction(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnMenu, struct MUI_NListtree_TreeNode *tnNewDefault);
static const struct AttributeDef *GetAttributeDef(enum ftAttributeType AttrType, enum FtEntryType fteEntryType);
static void FillListOfAttributeValues(struct FileTypesPrefsInst *inst, 
	const struct FileTypesListEntry *fte, const struct AttributeDef *atd, 
	const struct AttrListEntry *Attr);
static ULONG ConvertGroupOrientationFromString(CONST_STRPTR DisplayString);
static struct MUI_NListtree_TreeNode *CopyFileTypesEntry(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnToListNode, struct MUI_NListtree_TreeNode *tnToPrevNode, 
	struct MUI_NListtree_TreeNode *tnFrom, ULONG destList, ULONG srcList);
static BOOL MayPasteOnto(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom);
static struct MUI_NListtree_TreeNode *MayPasteBelow(struct FileTypesPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom);
static void EnablePasteMenuEntry(struct FileTypesPrefsInst *inst);
static BOOL MayPasteAttr(struct FileTypesPrefsInst *inst, const struct FileTypesListEntry *fte, const struct AttrListEntry *Attr);
static void EnablePasteAttrMenuEntry(struct FileTypesPrefsInst *inst);
static BOOL IsAttrRequired(struct FileTypesListEntry *fte, const struct AttributeDef *atd);
static void EntryHasChanged(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn);
static void SetChangedFlag(struct FileTypesPrefsInst *inst, BOOL changed);
static void BuildTTDescFromAttrList(char *buffer, size_t length, struct TagItem *AttrList);
static BOOL ParseTTFontFromDesc(CONST_STRPTR FontDesc, 
	ULONG *FontStyle, ULONG *FontWeight, ULONG *FontSize,
	STRPTR FontName, size_t FontNameSize);
static void ShowHiddenNodes(struct FileTypesPrefsInst *inst);
static void ParseToolTypes(struct FileTypesPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt);
static LONG ReadScalosPrefs(struct FileTypesPrefsInst *inst, CONST_STRPTR PrefsFileName);
static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString);
static ULONG DoDragDrop(Class *cl, Object *obj, Msg msg);
static struct FoundNode *AddFoundNode(struct FileTypesPrefsInst *inst, Object *ListTree, struct MUI_NListtree_TreeNode *tn);
static CONST_STRPTR FindString(CONST_STRPTR string, CONST_STRPTR pattern, BOOL CaseSensitive);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

//----------------------------------------------------------------------------

// local data items

#ifndef __AROS__
struct Library *MUIMasterBase;
T_LOCALEBASE LocaleBase;
struct GfxBase *GfxBase;
struct Library *IconBase;
T_UTILITYBASE UtilityBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *TTEngineBase;
struct Library *IconobjectBase;
struct Library *WorkbenchBase;
struct Library *IFFParseBase;
struct Library *DiskfontBase;
struct Library *PreferencesBase;
struct Library *DataTypesBase;

#ifdef __amigaos4__
struct Library *DOSBase;
struct DOSIFace *IDOS;
struct MUIMasterIFace *IMUIMaster;
struct LocaleIFace *ILocale;
struct GraphicsIFace *IGraphics;
struct WorkbenchIFace *IWorkbench;
struct IconIFace *IIcon;
struct IFFParseIFace *IIFFParse;
struct IconobjectIFace *IIconobject;
struct UtilityIFace *IUtility;
struct IntuitionIFace *IIntuition;
struct TTEngineIFace *ITTEngine;
struct DiskfontIFace *IDiskfont;
struct PreferencesIFace *IPreferences;
struct DataTypesIFace *IDataTypes;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

#ifdef __AROS__
struct DosLibrary *DOSBase;
#endif

struct MUI_CustomClass *FileTypesPrefsClass;
struct MUI_CustomClass *myNListClass;
struct MUI_CustomClass *myFileTypesActionsNListClass;
struct MUI_CustomClass *myNListTreeClass;
struct MUI_CustomClass *myFileTypesNListTreeClass;
struct MUI_CustomClass *FontSampleClass;
struct MUI_CustomClass *IconobjectClass;
struct MUI_CustomClass *DataTypesImageClass;

#ifdef __AROS__
#define myNListTreeObject BOOPSIOBJMACRO_START(myNListTreeClass->mcc_Class)
#define myNListObject BOOPSIOBJMACRO_START(myNListClass->mcc_Class)
#define myFileTypesNListTreeObject BOOPSIOBJMACRO_START(myFileTypesNListTreeClass->mcc_Class)
#define myFileTypesActionsNListObject BOOPSIOBJMACRO_START(myFileTypesActionsNListClass->mcc_Class)
#else
#define myNListTreeObject NewObject(myNListTreeClass->mcc_Class, 0
#define myNListObject NewObject(myNListClass->mcc_Class, 0
#define myFileTypesNListTreeObject NewObject(myFileTypesNListTreeClass->mcc_Class, 0
#define myFileTypesActionsNListObject NewObject(myFileTypesActionsNListClass->mcc_Class, 0
#endif

static struct Catalog *FileTypesPrefsCatalog;
static struct Locale *FileTypesPrefsLocale;

static BOOL StaticsTranslated;

static const struct MUIP_ScalosPrefs_MCCList RequiredMccList[] =
	{
	{ MUIC_NListtree, 18, 18 },
	{ MUIC_NList, 18, 0 },
	{ NULL, 0, 0 }
	};

static ULONG MajorVersion, MinorVersion;

static const struct Hook FileTypesPrefsHooks[] =
{
	{ { NULL, NULL }, NULL, NULL, NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(OpenHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SaveAsHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(TreeDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(TreeConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(TreeDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AttrDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AttrConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AttrDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddEntryDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddEntryConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddEntryDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddAttrDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddAttrConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddAttrDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttrDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttrConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttrDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesActionDisplayFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesActionConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesActionDestructFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(SelectEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectAttributeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttributeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AboutFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ContextMenuTriggerHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseAllHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandAllHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseFileTypesHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandFileTypesHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseAllFileTypesHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandAllFileTypesHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CopyHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CutHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(PasteHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CopyAttrHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CutAttrHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(PasteAttrHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddAttributeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemoveEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemoveAttributeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ChangeAttributeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectAttributeValueHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttributePopAslFileStartHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttributePopAslPathStartHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttrTTFontOpenHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditAttrTTFontCloseHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ResetToDefaultsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(LastSavedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RestoreHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(HideEmptyEntriesHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(SelectFileTypesEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectFileTypesActionEntryHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(RenameFileTypeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddFileTypeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemoveFileTypeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AddFileTypesMethodHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemoveFileTypesActionHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionMatchMatchHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionMatchCaseHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionMatchOffsetHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionSearchSearchHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionSearchCaseHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionSearchSkipSpacesHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionFilesizeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionPatternHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionProtectionHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionDosDeviceHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionDeviceNameHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionContentsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionDosTypeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangedFileTypesActionMinSizeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(FileTypesActionDragDropSortHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ShowFindGroupHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(HideFindGroupHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(IncrementalFindFileTypeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(IncrementalFindNextFileTypeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(IncrementalFindPrevFileTypeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(LearnFileTypeHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CreateNewFileTypesIconHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(EditFileTypesIconHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(AslIntuiMsgHookFunc), NULL },
};


// Context menu in "filetypes" list
static struct NewMenu ContextMenuFileTypes[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSE,	 	NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_CollapseFileTypes },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPAND, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_ExpandFileTypes },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSEALL, 	NULL, 	0,			0, (APTR) HOOKNDX_CollapseAllFileTypes },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPANDALL, 	NULL, 	0,			0, (APTR) HOOKNDX_ExpandAllFileTypes },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_FIND, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_ShowFindGroup },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

// Context menu on "methods" list
static struct NewMenu ContextMenuFileTypesActions[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_OPEN, 		(STRPTR) MSGID_MENU_PROJECT_OPEN_SHORT, 	0,	0, (APTR) HOOKNDX_Open },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_SAVEAS, 	(STRPTR) MSGID_MENU_PROJECT_SAVEAS_SHORT,	0,	0, (APTR) HOOKNDX_SaveAs },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static struct NewMenu ContextMenus[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSE,	 	NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_Collapse },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPAND, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_Expand },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_HIDE_EMPTY_ENTRIES,	NULL, 				CHECKIT|MENUTOGGLE,	0, (APTR) HOOKNDX_HideEmptyEntries },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COPY,	 	NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_Copy },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_CUT, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_Cut },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_PASTE, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_Paste },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSEALL, 	NULL, 	0,			0, (APTR) HOOKNDX_CollapseAll },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPANDALL, 	NULL, 	0,			0, (APTR) HOOKNDX_ExpandAll },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static struct NewMenu ContextMenusAttrList[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 	0,			0, NULL },
	{  NM_ITEM, NM_BARLABEL,				NULL, 	0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COPY,	 	NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_CopyAttr },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_CUT, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_CutAttr },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_PASTE, 		NULL, 	NM_ITEMDISABLED,	0, (APTR) HOOKNDX_PasteAttr },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 	0,			0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static const BOOL MayPasteAfterMatrix[ENTRYTYPE_MAX][ENTRYTYPE_MAX] =
	{
	//    0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17
	/* 0 ENTRYTYPE_FileType */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 1 ENTRYTYPE_PopupMenu */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 2 ENTRYTYPE_PopupMenu_SubMenu */
	{ FALSE, FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 3 ENTRYTYPE_PopupMenu_MenuItem */
	{ FALSE, FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 4 ENTRYTYPE_PopupMenu_InternalCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 5 ENTRYTYPE_PopupMenu_WbCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 6 ENTRYTYPE_PopupMenu_ARexxCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 7 ENTRYTYPE_PopupMenu_CliCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 8 ENTRYTYPE_PopupMenu_PluginCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 9 ENTRYTYPE_PopupMenu_IconWindowCmd */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 10 ENTRYTYPE_PopupMenu_MenuSeparator */
	{ FALSE, FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 11 ENTRYTYPE_ToolTip */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 12 ENTRYTYPE_ToolTip_Group */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 13 ENTRYTYPE_ToolTip_Member */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE},
	/* 14 ENTRYTYPE_ToolTip_HBar */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 15 ENTRYTYPE_ToolTip_String */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 16 ENTRYTYPE_ToolTip_Space */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 17 ENTRYTYPE_ToolTip_DtImage */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	//    0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17
	};
static const BOOL MayPasteIntoMatrix[ENTRYTYPE_MAX][ENTRYTYPE_MAX] =
	{
	//    0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17
	/* 0 ENTRYTYPE_FileType */
	{ FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE,  FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 1 ENTRYTYPE_PopupMenu */
	{ FALSE, FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 2 ENTRYTYPE_PopupMenu_SubMenu */
	{ FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 3 ENTRYTYPE_PopupMenu_MenuItem */
	{ FALSE, FALSE, FALSE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 4 ENTRYTYPE_PopupMenu_InternalCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 5 ENTRYTYPE_PopupMenu_WbCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 6 ENTRYTYPE_PopupMenu_ARexxCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 7 ENTRYTYPE_PopupMenu_CliCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 8 ENTRYTYPE_PopupMenu_PluginCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 9 ENTRYTYPE_PopupMenu_IconWindowCmd */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 10 ENTRYTYPE_PopupMenu_MenuSeparator */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},

	/* 11 ENTRYTYPE_ToolTip */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 12 ENTRYTYPE_ToolTip_Group */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE},
	/* 13 ENTRYTYPE_ToolTip_Member */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE},
	/* 14 ENTRYTYPE_ToolTip_HBar */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 15 ENTRYTYPE_ToolTip_String */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 16 ENTRYTYPE_ToolTip_Space */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	/* 17 ENTRYTYPE_ToolTip_DtImage */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
	//    0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17
	};

static const ULONG EntryTypeNames[] =
	{
	MSGID_ENTRYTYPE_FILETYPE,

	MSGID_ENTRYTYPE_POPUPMENU,
	MSGID_ENTRYTYPE_SUBMENU,
	MSGID_ENTRYTYPE_MENUITEM,
	MSGID_ENTRYTYPE_INTERNALCMD,
	MSGID_ENTRYTYPE_WBCMD,
	MSGID_ENTRYTYPE_AREXXCMD,
	MSGID_ENTRYTYPE_CLICMD,
	MSGID_ENTRYTYPE_PLUGINCMD,
	MSGID_ENTRYTYPE_ICONWINDOWCMD,
	MSGID_ENTRYTYPE_MENUSEPARATOR,

	MSGID_ENTRYTYPE_TOOLTIP,
	MSGID_ENTRYTYPE_GROUP,
	MSGID_ENTRYTYPE_MEMBER,
	MSGID_ENTRYTYPE_HBAR,
	MSGID_ENTRYTYPE_STRING,
	MSGID_ENTRYTYPE_SPACE,
	MSGID_ENTRYTYPE_DTIMAGE,
	};

static const ULONG AddFiletypeStringsPM[] =
	{
	ENTRYTYPE_PopupMenu,
	0
	};
static const ULONG AddFiletypeStringsTT[] =
	{
	ENTRYTYPE_ToolTip,
	0
	};
static const ULONG AddFiletypeStringsPMTT[] =
	{
	ENTRYTYPE_PopupMenu,
	ENTRYTYPE_ToolTip,
	0
	};
static const ULONG AddPopupMenuStrings[] =
	{
	ENTRYTYPE_PopupMenu_SubMenu,
	ENTRYTYPE_PopupMenu_MenuItem,
	ENTRYTYPE_PopupMenu_MenuSeparator,
	0
	};
static const ULONG AddSubMenuStrings[] =
	{
	ENTRYTYPE_PopupMenu_MenuItem,
	ENTRYTYPE_PopupMenu_MenuSeparator,
	0
	};
static const ULONG AddMenuItemStrings[] =
	{
	ENTRYTYPE_PopupMenu_InternalCmd,
	ENTRYTYPE_PopupMenu_WbCmd,
	ENTRYTYPE_PopupMenu_ARexxCmd,
	ENTRYTYPE_PopupMenu_CliCmd,
	ENTRYTYPE_PopupMenu_PluginCmd,
	ENTRYTYPE_PopupMenu_IconWindowCmd,
	0
	};
static const ULONG AddToolTipStrings[] =
	{
	ENTRYTYPE_ToolTip_Group,
	0
	};
static const ULONG AddGroupStrings[] =
	{
	ENTRYTYPE_ToolTip_Member,
	0
	};
static const ULONG AddMemberStrings[] =
	{
	ENTRYTYPE_ToolTip_Group,
	ENTRYTYPE_ToolTip_HBar,
	ENTRYTYPE_ToolTip_String,
	ENTRYTYPE_ToolTip_Space,
	ENTRYTYPE_ToolTip_DtImage,
	0
	};

static STRPTR RegisterTitles[] =
	{
	(STRPTR) MSGID_REGISTERTITLE_RECOGNITION,
	(STRPTR) MSGID_REGISTERTITLE_ACTIONS,
	NULL
	};

STRPTR AddFileTypeActionStrings[] =
	{
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MATCH,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_SEARCH,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_FILESIZE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_PATTERN,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_PROTECTION,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_OR,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_ISASCII,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DOSDEVICE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DEVICENAME,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_CONTENTS,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DOSTYPE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MINSIZEMB,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MACROCLASS,
	NULL
	};

STRPTR AddFileTypeActionStringsProject[] =
	{
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MATCH,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_SEARCH,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_FILESIZE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_PATTERN,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_PROTECTION,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_OR,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_ISASCII,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MACROCLASS,
	NULL
	};

STRPTR AddFileTypeActionStringsDisk[] =
	{
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_OR,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_ISASCII,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DOSDEVICE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DEVICENAME,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_CONTENTS,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_DOSTYPE,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MINSIZEMB,
	(STRPTR) MSGID_FILETYPE_ADD_ACTION_MACROCLASS,
	NULL
	};

static STRPTR FileTypeActionProtectionStrings[] =
	{
	(STRPTR) MSGID_FILETYPE_PROTECTION_SET,
	(STRPTR) MSGID_FILETYPE_PROTECTION_UNSET,
	(STRPTR) MSGID_FILETYPE_PROTECTION_IGNORE,
	NULL,
	};

static struct AttrDefaultValue AttrDefStringStyleContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "normal" },
	{ ATTRDEFTYPE_String, 0, "bold" },
	{ ATTRDEFTYPE_String, 0, "italic" },
	{ ATTRDEFTYPE_String, MSGID_CHANGEME, "bolditalic" },
	};
static struct AttrDefaultValue AttrDefStringHAlignContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "left" },
	{ ATTRDEFTYPE_String, 0, "center" },
	{ ATTRDEFTYPE_String, 0, "right" },
	};
static struct AttrDefaultValue AttrDefStringVAlignContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "top" },
	{ ATTRDEFTYPE_String, 0, "center" },
	{ ATTRDEFTYPE_String, 0, "bottom" },
	};
static struct AttrDefaultValue AttrDefStringPenContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "PENIDX_HSHINEPEN"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_HSHADOWPEN"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_ICONTEXTOUTLINEPEN",	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_DRAWERTEXT"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_DRAWERTEXTSEL"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_DRAWERBG",		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_FILETEXT"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_FILETEXTSEL"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_FILEBG"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_BACKDROPDETAIL"	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_BACKDROPBLOCK"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_TOOLTIP_TEXT"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_TOOLTIP_BG"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_DRAGINFOTEXT_TEXT"	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_DRAGINFOTEXT_BG"	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_STATUSBAR_BG"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_STATUSBAR_TEXT"	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_ICONTEXTPEN"		},
	{ ATTRDEFTYPE_String, 0, "PENIDX_ICONTEXTPENSEL"	},
	{ ATTRDEFTYPE_String, 0, "PENIDX_ICONTEXTSHADOWPEN"	},
	};
static struct AttrDefaultValue AttrDefStringIDContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "MSGID_BYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_KBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_GBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_HBYTENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_REQTITLE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_OPENERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_OPENERRORGADGETS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_QUITERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_GADGETSNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_WBLOADNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_WBLOAD_ASLTITLE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_REPLACENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_REPLACEGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_DOWAITERRNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_SCAERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_CLOSEWBNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUTNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_OKNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_UPDATENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_SYSINFONAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FLUSHNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_REBOOTNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_EXISTSNAME_COPY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_EXISTSGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_SUREREBOOTNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_OKCANCELNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_LIBERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_SCALIBERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLNAMENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLSIZENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLACCESSNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLDATENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLTIMENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLCOMNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_USEDLIBSNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_SCARUNNINGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PREVIEWNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_EMULATIONNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSTITLE_COPY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COPYINGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FROMNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TONAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE1NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE2NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE3NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE4NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE5NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMTITLE6NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COPYERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COPYERRORGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MOVEERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_DELETEERRORNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUT_TESTTEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUT_UNREGTEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUT_REGTOTEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUT_REGTEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ABOUT_COMREGINFO"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_SETUPSCREEN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_SETUPSCRPATTERN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_INITMENU"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_INITTIMER"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_STARTWINDOWPROC"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_INITNOTIFY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_WBSTARTUP"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_WBSTARTUP_RUNPRG"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_READDEVICEICONS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_INITDEVICEWINDOW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_WBSTARTUPFINISHED"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_SCALOSVERSION"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_ROOTDIR"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USERDIR"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_SOFTLINK"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_HARDLINKDIR"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_HARDLINKFILE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_PIPEFILE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_UNKNOWNTYPE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_LASTCHANGE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_SIZE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_PROTECTION"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_STATE_WRITEPROTECT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_STATE_VALIDATING"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_STATE_READWRITE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_STATE_UNKNOWN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_DISK"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USEDCOUNTFMT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_CREATED"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_STATE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USED"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILESIZEFMT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_APPICON"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ICON_WITHOUT_FILE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ICONIFIED_WINDOW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_OPEN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_COPY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_RENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_INFO"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_SNAPSHOT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_UNSNAPSHOT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_LEAVEOUT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_PUTAWAY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_DELETE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_FORMATDISK"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PMMENU_APPICON_EMPTYTRASHCAN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_EXISTSNAME_MOVE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_DRAWER_1"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_DRAWER_2"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_FILE_1"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_FILE_2"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TEXTICON_DRAWER"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TEXTICON_TRASHCAN"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_ENDREADDEVICEICONS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_STATUSBARTEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_DEVICE_1"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MULTIDRAG_DEVICE_2"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_CANCELBUTTON"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PREPARING_COPY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PREPARING_MOVE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSFER_REMAININGTIME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSFER_MINUTES"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSFER_SECONDS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSFER_SECOND"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_FILETRANSTITLE_MOVE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_MOVINGNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_READINGPREFS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_DISK_UNREADABLE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_PROGRESS_MAINWINDOW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_NEW_DRAWER_NAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_CANNOT_OPEN_DRAWER"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ALLOCPENSREQ_CONTENTS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ALLOCPENSREQ_GADGETSNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBGARBAGE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBPROJECT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBDEVICE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBKICK"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBTOOL"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILETYPE_WBDISK"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_ERROR_NO_DEFAULTTOOL"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_RIGHTSCROLLER"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_BOTTOMSCROLLER"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_UPARROW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_DOWNARROW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_RIGHTARROW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_LEFTARROW"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_ICONIFY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_STATUSBAR_TEXT"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_STATUSBAR_READONLY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_STATUSBAR_READING"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_STATUSBAR_TYPING"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_GADGET_STATUSBAR_SHOWALL"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_EXISTSICON_COPY"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USEDCOUNTFMT_FULL"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USEDCOUNTFMT_FREE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_USEDCOUNTFMT_INUSE"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_DRAWERS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_FILES"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_LINKTO"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TOOLTIP_KICKSTART"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_NODEFAULTTOOL_REQ"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_NODEFAULTTOOL_REQ_GADGETS"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_REQ_SELECTDEFTOOL"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_TEXTICON_SOLOICON"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLOWNERNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLGROUPNAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLFILETYPENAME"	},
	{ ATTRDEFTYPE_String, 0, "MSGID_COLVERSIONNAME"	},
	};
static struct AttrDefaultValue AttrDefGroupOrientationContents[] =
	{
	{ ATTRDEFTYPE_ULong, TTL_Horizontal, "", ConvertGroupOrientationFromString	},
	{ ATTRDEFTYPE_ULong, TTL_Vertical, "", ConvertGroupOrientationFromString	},
	};
static struct AttrDefaultValue AttrDefGroupStringSrcContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "diskstate",		},
	{ ATTRDEFTYPE_String, 0, "diskusage",		},
	{ ATTRDEFTYPE_String, 0, "diskusagefree",	},
	{ ATTRDEFTYPE_String, 0, "diskusageinuse",	},
	{ ATTRDEFTYPE_String, 0, "diskusagepercent",	},
	{ ATTRDEFTYPE_String, 0, "fibfilename",		},
	{ ATTRDEFTYPE_String, 0, "filecomment",		},
	{ ATTRDEFTYPE_String, 0, "filedate",		},
	{ ATTRDEFTYPE_String, 0, "fileprotection",	},
	{ ATTRDEFTYPE_String, 0, "filesize",		},
	{ ATTRDEFTYPE_String, 0, "filetime",		},
	{ ATTRDEFTYPE_String, 0, "filetypestring",	},
	{ ATTRDEFTYPE_String, 0, "iconname",		},
	{ ATTRDEFTYPE_String, 0, "linktarget",		},
	{ ATTRDEFTYPE_String, 0, "plugin",		},
	{ ATTRDEFTYPE_String, 0, "versionstring",	},
	{ ATTRDEFTYPE_String, 0, "volumecreateddate",	},
	{ ATTRDEFTYPE_String, 0, "volumecreatedtime",	},
	{ ATTRDEFTYPE_String, 0, "volumeordevicename",	},
	};
static struct AttrDefaultValue AttrDefPopupMenuInternalCmds[] =
	{
	{ ATTRDEFTYPE_String2, MSGID_COM5NAME, 			"about",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM1NAME, 			"backdrop",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM12NAME, 		"cleanup",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM43NAME, 		"cleanupbyname",       },
	{ ATTRDEFTYPE_String2, MSGID_COM44NAME, 		"cleanupbydate",       },
	{ ATTRDEFTYPE_String2, MSGID_COM45NAME, 		"cleanupbysize",       },
	{ ATTRDEFTYPE_String2, MSGID_COM46NAME, 		"cleanupbytype",       },
	{ ATTRDEFTYPE_String2, MSGID_COM36NAME, 		"clearselection",      },
	{ ATTRDEFTYPE_String2, MSGID_COM28NAME, 		"clone",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM9NAME, 			"close",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM39NAME, 		"copy",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM51NAME, 		"createthumbnail",     },
	{ ATTRDEFTYPE_String2, MSGID_COM40NAME, 		"cut",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM49NAME, 		"copyto",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM27NAME, 		"delete",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM29NAME, 		"emptytrashcan",       },
	{ ATTRDEFTYPE_String2, MSGID_COM2NAME, 			"executecommand",      },
	{ ATTRDEFTYPE_String2, MSGID_COM_FIND, 			"find",	         	},
	{ ATTRDEFTYPE_String2, MSGID_COM33NAME, 		"formatdisk",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM32NAME, 		"iconify",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM22NAME, 		"iconinfo",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM_ICONPROPERTIES,	"iconproperties",      },
	{ ATTRDEFTYPE_String2, MSGID_COM30NAME, 		"lastmsg",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM25NAME, 		"leaveout",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM7NAME, 			"makedir",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM50NAME, 		"moveto",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM19NAME, 		"open",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM_OPENBROWSERWINDOW, 	"openinbrowserwindow", },
	{ ATTRDEFTYPE_String2, MSGID_COM_OPENNEWWINDOW, 	"openinnewwindow",     },
	{ ATTRDEFTYPE_String2, MSGID_COM8NAME, 			"parent",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM41NAME, 		"paste",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM26NAME, 		"putaway",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM6NAME, 			"quit",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM_REDO,			"redo",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM31NAME, 		"redraw",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM3NAME, 			"redrawall",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM21NAME, 		"rename",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM20NAME, 		"reset",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM11NAME, 		"selectall",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM16NAME, 		"showallfiles",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM15NAME, 		"showonlyicons",       },
	{ ATTRDEFTYPE_String2, MSGID_COM48NAME, 		"showdefault",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM34NAME, 		"shutdown",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM35NAME, 		"sizetofit",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM23NAME, 		"snapshot",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM14NAME, 		"snapshotall",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM13NAME, 		"snapshotwindow",      },
	{ ATTRDEFTYPE_String2, MSGID_COM_THUMBNAILCACHECLEANUP,	"thumbnailcachecleanup", },
	{ ATTRDEFTYPE_String2, MSGID_COM24NAME, 		"unsnapshot",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM_UNDO,			"undo",		       },
	{ ATTRDEFTYPE_String2, MSGID_COM10NAME, 		"update",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM4NAME, 			"updateall",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM47NAME, 		"viewbydefault",       },
	{ ATTRDEFTYPE_String2, MSGID_COM38NAME, 		"viewbydate",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM17NAME, 		"viewbyicon",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM37NAME, 		"viewbysize",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM42NAME, 		"viewbytype",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM18NAME, 		"viewbytext",	       },
	{ ATTRDEFTYPE_String2, MSGID_COM_WINDOWPROPERTIES, 	"windowproperties",    },
	};
static struct AttrDefaultValue AttrDefGroupMemberHideContents[] =
	{
	{ ATTRDEFTYPE_String, 0, "novolumenode",	},
	{ ATTRDEFTYPE_String, 0, "isempty",		},
	};

static struct AttributeDef AttrDefFileType[] =
	{
		{ 
		ATTRTYPE_FtDescription, 
		FALSE,
		0, { ATTRTYPE_FtDescription },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "" },
		NULL, 0,
		},
		{
		ATTRTYPE_PvPluginName,
		FALSE,
		0, { ATTRTYPE_PvPluginName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "Scalos:Plugins/Preview/" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefDtImage[] =
	{
		{ 
		ATTRTYPE_DtImageName, 
		TRUE,
		0, { ATTRTYPE_DtImageName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "SYS:" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefSubMenu[] =
	{
		{ 
		ATTRTYPE_MenuItemName, 
		TRUE,
		0, { ATTRTYPE_MenuItemName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		NULL, 0,
		},
		{
		ATTRTYPE_UnselIconName,
		FALSE,
		0, { ATTRTYPE_UnselIconName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "THEME:Filetypes/" },
		NULL, 0,
		},
		{
		ATTRTYPE_SelIconName,
		FALSE,
		0, { ATTRTYPE_SelIconName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "THEME:Filetypes/" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefMenuItem[] =
	{
		{ 
		ATTRTYPE_MenuItemName, 
		TRUE,
		0, { ATTRTYPE_MenuItemName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		NULL, 0,
		},
		{ 
		ATTRTYPE_MenuCommKey, 
		FALSE,
		0, { ATTRTYPE_MenuCommKey },
		1, 1,
		{ ATTRDEFTYPE_String, 0, "" },
		NULL, 0,
		},
		{ 
		ATTRTYPE_MenuDefaultAction, 
		FALSE,
		0, { ATTRTYPE_MenuDefaultAction },
		0, 0,
		{ ATTRDEFTYPE_None, 0, "" },
		NULL, 0,
		},
		{
		ATTRTYPE_UnselIconName,
		FALSE,
		0, { ATTRTYPE_UnselIconName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "THEME:Filetypes/" },
		NULL, 0,
		},
		{
		ATTRTYPE_SelIconName,
		FALSE,
		0, { ATTRTYPE_SelIconName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "THEME:Filetypes/" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefWBCmd[] =
	{
		{ 
		ATTRTYPE_CommandName, 
		TRUE,
		0, { ATTRDEFTYPE_FileName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "SYS:" },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandStacksize, 
		FALSE,
		0, { ATTRTYPE_CommandStacksize },
		4096, 4194304,
		{ ATTRDEFTYPE_ULong, 16384, "", NULL },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandPriority, 
		FALSE,
		0, { ATTRTYPE_CommandPriority },
		SCHAR_MIN, SCHAR_MAX,
		{ ATTRDEFTYPE_ULong, 0, "", NULL },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandWbArgs, 
		FALSE,
		0, { ATTRTYPE_CommandWbArgs },
		0, 0,
		{ ATTRDEFTYPE_None, 0, "" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefCmd[] =
	{
		{ 
		ATTRTYPE_CommandName, 
		TRUE,
		0, { ATTRTYPE_CommandName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FileName, 0, "SYS:" },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandStacksize, 
		FALSE,
		0, { ATTRTYPE_CommandStacksize },
		4096, 4194304,
		{ ATTRDEFTYPE_ULong, 16384, "", NULL },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandPriority, 
		FALSE,
		0, { ATTRTYPE_CommandPriority },
		SCHAR_MIN, SCHAR_MAX,
		{ ATTRDEFTYPE_ULong, 0, "", NULL },
		NULL, 0,
		},
		{ 
		ATTRTYPE_CommandWbArgs, 
		FALSE,
		0, { ATTRTYPE_CommandWbArgs },
		0, 0,
		{ ATTRDEFTYPE_None, 0, "" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefInternalCmd[] =
	{
		{ 
		ATTRTYPE_CommandName, 
		TRUE,
		0, { ATTRTYPE_CommandName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		AttrDefPopupMenuInternalCmds, Sizeof(AttrDefPopupMenuInternalCmds),
		},
	};
static struct AttributeDef AttrDefPluginCmd[] =
	{
		{ 
		ATTRTYPE_CommandName, 
		TRUE,
		0, { ATTRTYPE_CommandName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefIconWindowCmd[] =
	{
		{ 
		ATTRTYPE_CommandName, 
		TRUE,
		0, { ATTRTYPE_CommandName },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_PathName, 0, "SYS:" },
		NULL, 0,
		},
	};
static struct AttributeDef AttrDefGroup[] =
	{
		{ 
		ATTRTYPE_GroupOrientation, 
		TRUE,
		0, { ATTRTYPE_GroupOrientation },
		TTL_Horizontal, TTL_Vertical,
		{ ATTRDEFTYPE_ULong, TTL_Vertical, "", ConvertGroupOrientationFromString },
		AttrDefGroupOrientationContents, Sizeof(AttrDefGroupOrientationContents),
		},
	};
static struct AttributeDef AttrDefMember[] =
	{
		{ 
		ATTRTYPE_MemberHideString, 
		FALSE,
		0, { ATTRTYPE_MemberHideString },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		AttrDefGroupMemberHideContents, Sizeof(AttrDefGroupMemberHideContents),
		},
	};
static struct AttributeDef AttrDefString[] =
	{
		{ 
		ATTRTYPE_StringText, 
		TRUE,
		2, { ATTRTYPE_StringSrc, ATTRTYPE_StringId },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_LocString, MSGID_CHANGEME, "" },
		NULL, 0,
		},
		{ 
		ATTRTYPE_StringSrc, 
		TRUE,
		2, { ATTRTYPE_StringText, ATTRTYPE_StringId },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "diskstate" },
		AttrDefGroupStringSrcContents, Sizeof(AttrDefGroupStringSrcContents),
		},
		{ 
		ATTRTYPE_StringId, 
		TRUE,
		2, { ATTRTYPE_StringText, ATTRTYPE_StringSrc },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "MSGID_BYTENAME" },
		AttrDefStringIDContents, Sizeof(AttrDefStringIDContents),
		},
		{ 
		ATTRTYPE_StringPen, 
		FALSE,
		0, { ATTRTYPE_StringPen },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "PENIDX_TOOLTIP_TEXT" },
		AttrDefStringPenContents, Sizeof(AttrDefStringPenContents),
		},
		{ 
		ATTRTYPE_StringFont, 
		FALSE,
		0, { ATTRTYPE_StringFont },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_FontString, 0, "" },
		NULL,
		},
		{ 
		ATTRTYPE_StringTTFont, 
		FALSE,
		0, { ATTRTYPE_StringTTFont },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_TTFontString, 0, "" },
		NULL,
		},
		{ 
		ATTRTYPE_StringStyle, 
		FALSE,
		0, { ATTRTYPE_StringStyle },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "normal" },
		AttrDefStringStyleContents, Sizeof(AttrDefStringStyleContents),
		},
		{ 
		ATTRTYPE_StringVAlign, 
		FALSE,
		0, { ATTRTYPE_StringVAlign },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "center" },
		AttrDefStringVAlignContents, Sizeof(AttrDefStringVAlignContents),
		},
		{ 
		ATTRTYPE_StringHAlign, 
		FALSE,
		0, { ATTRTYPE_StringHAlign },
		0, MAX_ATTRVALUE,
		{ ATTRDEFTYPE_String, 0, "center" },
		AttrDefStringHAlignContents, Sizeof(AttrDefStringHAlignContents),
		},
	};
static struct AttributeDef AttrDefSpace[] =
	{
		{ 
		ATTRTYPE_SpaceSize, 
		TRUE,
		0, { ATTRTYPE_SpaceSize },
		1, 255,
		{ ATTRDEFTYPE_ULong, 1, "", NULL },
		NULL, 0,
		},
	};

static const struct EntryAttributes AllEntryAttributes[] =
	{
		{
		ENTRYTYPE_FileType,
		AttrDefFileType, Sizeof(AttrDefFileType),
		},
		{
		ENTRYTYPE_PopupMenu,
		NULL, 0,
		},
		{
		ENTRYTYPE_PopupMenu_SubMenu,
		AttrDefSubMenu, Sizeof(AttrDefSubMenu),
		},
		{
		ENTRYTYPE_PopupMenu_MenuItem,
		AttrDefMenuItem, Sizeof(AttrDefMenuItem),
		},
		{
		ENTRYTYPE_PopupMenu_InternalCmd,
		AttrDefInternalCmd, Sizeof(AttrDefInternalCmd),
		},
		{
		ENTRYTYPE_PopupMenu_WbCmd,
		AttrDefWBCmd, Sizeof(AttrDefWBCmd),
		},
		{
		ENTRYTYPE_PopupMenu_ARexxCmd,
		AttrDefCmd, Sizeof(AttrDefCmd),
		},
		{
		ENTRYTYPE_PopupMenu_CliCmd,
		AttrDefCmd, Sizeof(AttrDefCmd),
		},
		{
		ENTRYTYPE_PopupMenu_PluginCmd,
		AttrDefPluginCmd, Sizeof(AttrDefPluginCmd),
		},
		{
		ENTRYTYPE_PopupMenu_IconWindowCmd,
		AttrDefIconWindowCmd, Sizeof(AttrDefIconWindowCmd),
		},
		{
		ENTRYTYPE_PopupMenu_MenuSeparator,
		NULL, 0,
		},
		{
		ENTRYTYPE_ToolTip,
		NULL, 0,
		},
		{
		ENTRYTYPE_ToolTip_Group,
		AttrDefGroup, Sizeof(AttrDefGroup),
		},
		{
		ENTRYTYPE_ToolTip_Member,
		AttrDefMember, Sizeof(AttrDefMember),
		},
		{
		ENTRYTYPE_ToolTip_HBar,
		NULL, 0,
		},
		{
		ENTRYTYPE_ToolTip_String,
		AttrDefString, Sizeof(AttrDefString),
		},
		{
		ENTRYTYPE_ToolTip_Space,
		AttrDefSpace, Sizeof(AttrDefSpace),
		},
		{
		ENTRYTYPE_ToolTip_DtImage,
		AttrDefDtImage, Sizeof(AttrDefDtImage),
		}
	};

//---------------------------------------------------------------

BOOL closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

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
	if (IconobjectClass)
		{
		CleanupIconobjectClass(IconobjectClass);
		IconobjectClass = NULL;
		}
	if (myNListClass)
		{
		MUI_DeleteCustomClass(myNListClass);
		myNListClass = NULL;
		}
	if (myFileTypesActionsNListClass)
		{
		MUI_DeleteCustomClass(myFileTypesActionsNListClass);
		myFileTypesActionsNListClass = NULL;
		}
	if (myNListTreeClass)
		{
		MUI_DeleteCustomClass(myNListTreeClass);
		myNListTreeClass = NULL;
		}
	if (myFileTypesNListTreeClass)
		{
		MUI_DeleteCustomClass(myFileTypesNListTreeClass);
		myFileTypesNListTreeClass = NULL;
		}

	if (FileTypesPrefsCatalog)
		{
		CloseCatalog(FileTypesPrefsCatalog);
		FileTypesPrefsCatalog = NULL;
		}
	if (FileTypesPrefsLocale)
		{
		CloseLocale(FileTypesPrefsLocale);
		FileTypesPrefsLocale = NULL;
		}

	if (FileTypesPrefsClass)
		{
		MUI_DeleteCustomClass(FileTypesPrefsClass);
		FileTypesPrefsClass = NULL;
		}

	CloseLibraries();

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif

	return TRUE;
}


LIBFUNC_P2(ULONG, LIBSCAGetPrefsInfo,
	D0, ULONG, which,
	A6, struct PluginBase *, PluginBase, 5)
{
	ULONG result;

	(void) PluginBase;

	d1(kprintf(__FILE__ "/%s/%ld: which=%ld\n", __FUNC__, __LINE__, which));

	switch(which)
		{
	case SCAPREFSINFO_GetClass:
		result = (ULONG) FileTypesPrefsClass;
		break;

	case SCAPREFSINFO_GetTitle:
		result = (ULONG) GetLocString(MSGID_PLUGIN_LIST_TITLE);
		break;

	case SCAPREFSINFO_GetTitleImage:
		result = (ULONG) CreatePrefsImage();
		break;

	default:
		result = (ULONG) NULL;
		break;
		}

	d1(kprintf(__FILE__ "/%s/%ld: Result=%08lx\n", __FUNC__, __LINE__, result));

	return result;
}
LIBFUNC_END

//----------------------------------------------------------------------------

DISPATCHER(FileTypesPrefs)
{
	struct FileTypesPrefsInst *inst;
	ULONG n;
	ULONG result = 0;

	d1(kprintf("%s/%ld:  START obj=%08lx\n", __FUNC__, __LINE__, obj));

	switch(msg->MethodID)
		{
	case OM_NEW:
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		d1(kprintf(__FILE__ "/%s/%ld: OM_NEW obj=%08lx\n", __FUNC__, __LINE__, obj));
		if (obj)
			{
			Object *prefsobject;
			struct opSet *ops = (struct opSet *) msg;

			inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);

			memset(inst, 0, sizeof(struct FileTypesPrefsInst));

			inst->fpb_SaveFlags = FTWRITEFLAG_ONLY_SAVE_CHANGED | FTWRITEFLAG_CLEAR_CHANGE_FLAG;
			inst->fpb_Changed = FALSE;
			inst->fpb_HideEmptyNodes = FALSE;
			inst->fpb_FileTypesDirLock = (BPTR) NULL;
			inst->fpb_IncludeNesting = 0;
			inst->fpb_WBScreen = LockPubScreen("Workbench");
			inst->fpb_MenuImageIndex = 0;

			inst->fpb_TTfAntialias = TT_Antialias_Auto;
			inst->fpb_TTfGamma = 2500;

			NewList(&inst->fpb_FoundList);
			inst->fpb_CurrentFound = (struct FoundNode *) &inst->fpb_FoundList.lh_Tail;

			InitHooks(inst);

			ReadScalosPrefs(inst, "ENV:scalos/scalos.prefs");

			inst->fpb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, TRUE, ops->ops_AttrList);
			inst->fpb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) "", ops->ops_AttrList);
			inst->fpb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, (ULONG) NULL, ops->ops_AttrList);
			inst->fpb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, (ULONG) NULL, ops->ops_AttrList);

			prefsobject = CreatePrefsGroup(inst);
			d1(kprintf(__FILE__ "/%s/%ld: prefsobject=%08lx\n", __FUNC__, __LINE__, prefsobject));
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
		d1(kprintf(__FILE__ "/%s/%ld: OM_DISPOSE obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);

		CleanupFoundNodes(inst);

		if (inst->fpb_WBScreen)
			{
			UnlockPubScreen(NULL, inst->fpb_WBScreen);
			inst->fpb_WBScreen = NULL;
			}
		for (n=0; n<Sizeof(inst->fpb_FileHandles); n++)
			{
			if (inst->fpb_FileHandles[n].ftfd_FileHandle)
				{
				Close(inst->fpb_FileHandles[n].ftfd_FileHandle);
				inst->fpb_FileHandles[n].ftfd_FileHandle = (BPTR) NULL;
				}
			}
		if (inst->fpb_LearnReq)
			{
			MUI_FreeAslRequest(inst->fpb_LearnReq);
			inst->fpb_LearnReq = NULL;
			}
		if (inst->fpb_LoadReq)
			{
			MUI_FreeAslRequest(inst->fpb_LoadReq);
			inst->fpb_LoadReq = NULL;
			}
		if (inst->fpb_SaveReq)
			{
			MUI_FreeAslRequest(inst->fpb_SaveReq);
			inst->fpb_SaveReq = NULL;
			}
		if (inst->fpb_FileTypesDirLock)
			{
			UnLock(inst->fpb_FileTypesDirLock);
			inst->fpb_FileTypesDirLock = (BPTR) NULL;
			}
		if (inst->fpb_Objects[OBJNDX_ContextMenuFileTypes])
			{
			MUI_DisposeObject(inst->fpb_Objects[OBJNDX_ContextMenuFileTypes]);
			inst->fpb_Objects[OBJNDX_ContextMenuFileTypes] = NULL;
			}
		if (inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions])
			{
			MUI_DisposeObject(inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions]);
			inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions] = NULL;
			}
		if (inst->fpb_Objects[OBJNDX_ContextMenu])
			{
			MUI_DisposeObject(inst->fpb_Objects[OBJNDX_ContextMenu]);
			inst->fpb_Objects[OBJNDX_ContextMenu] = NULL;
			}
		if (inst->fpb_Objects[OBJNDX_ContextMenuAttrList])
			{
			MUI_DisposeObject(inst->fpb_Objects[OBJNDX_ContextMenuAttrList]);
			inst->fpb_Objects[OBJNDX_ContextMenuAttrList] = NULL;
			}
		if (inst->fpb_IconObject)
			{
			DisposeIconObject(inst->fpb_IconObject);
			inst->fpb_IconObject = NULL;
			}

		CleanupDefIcons(inst);

		d1(kprintf("%s/%ld: FileTypesPrefsCatalog=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsCatalog));
		return DoSuperMethodA(cl, obj, msg);
		break;

	case OM_SET:
		{
		struct opSet *ops = (struct opSet *) msg;

		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf("%s/%ld:   OM_SET  createIcons=%ld\n", __FUNC__, __LINE__, inst->fpb_fCreateIcons));

		inst->fpb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, inst->fpb_fCreateIcons, ops->ops_AttrList);
		inst->fpb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) inst->fpb_ProgramName, ops->ops_AttrList);
		inst->fpb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, 
			(ULONG) inst->fpb_Objects[OBJNDX_WIN_Main], ops->ops_AttrList);
		inst->fpb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, 
			(ULONG) inst->fpb_Objects[OBJNDX_APP_Main], ops->ops_AttrList);

		d1(kprintf("%s/%ld:   OM_SET  createIcons=%ld\n", __FUNC__, __LINE__, inst->fpb_fCreateIcons));

		return DoSuperMethodA(cl, obj, msg);
		}
		break;

	case OM_GET:
		{
		struct opGet *opg = (struct opGet *) msg;

		d1(kprintf(__FILE__ "/%s/%ld: OM_GET obj=%08lx\n", __FUNC__, __LINE__, obj));

		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		switch (opg->opg_AttrID)
			{
		case MUIA_ScalosPrefs_CreateIcons:
			*opg->opg_Storage = inst->fpb_fCreateIcons;
			result = 0;
			break;
		default:
			result = DoSuperMethodA(cl, obj, msg);
			}
		}
		break;

	case MUIM_ScalosPrefs_ParseToolTypes:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_ParseToolTypes obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf(__FILE__ "/%s/%ld: before ParseToolTypes\n", __FUNC__, __LINE__));
		ParseToolTypes(inst, (struct MUIP_ScalosPrefs_ParseToolTypes *) msg);
		d1(kprintf(__FILE__ "/%s/%ld: after ParseToolTypes\n", __FUNC__, __LINE__));
		 break;

	case MUIM_ScalosPrefs_LoadConfig:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_LoadConfig obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		ReadPrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS);
		inst->fpb_SaveFlags = FTWRITEFLAG_ONLY_SAVE_CHANGED | FTWRITEFLAG_CLEAR_CHANGE_FLAG;
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfig:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_UseConfig obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		WritePrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS, inst->fpb_SaveFlags);
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfigIfChanged:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_UseConfigIfChanged obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		if (inst->fpb_Changed)
			{
			WritePrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS, inst->fpb_SaveFlags);
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_SaveConfig:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_SaveConfig obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		WritePrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS, inst->fpb_SaveFlags & ~FTWRITEFLAG_CLEAR_CHANGE_FLAG);
		WritePrefs(inst, ENVARC_FILETYPES_DIR, ENVARC_DEFICONS_PREFS, inst->fpb_SaveFlags);
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_SaveConfigIfChanged:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_SaveConfigIfChanged obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		if (inst->fpb_Changed)
			{
			WritePrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS, inst->fpb_SaveFlags & ~FTWRITEFLAG_CLEAR_CHANGE_FLAG);
			WritePrefs(inst, ENVARC_FILETYPES_DIR, ENVARC_DEFICONS_PREFS, inst->fpb_SaveFlags);
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_RestoreConfig:
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		d1(KPrintF(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_RestoreConfig\n", __FUNC__, __LINE__));
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_Restore], 0);
		break;

	case MUIM_ScalosPrefs_ResetToDefaults:
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		d1(KPrintF(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_ResetToDefaults", __FUNC__, __LINE__));
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ResetToDefaults], 0);
		break;

	case MUIM_ScalosPrefs_LastSavedConfig:
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		d1(KPrintF(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_LastSavedConfig", __FUNC__, __LINE__));
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_LastSaved], 0);
		break;

	case MUIM_ScalosPrefs_OpenConfig:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_OpenConfig obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_Open], 0);
		break;

	case MUIM_ScalosPrefs_SaveConfigAs:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_SaveConfigAs obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SaveAs], 0);
		break;

	case MUIM_ScalosPrefs_About:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_About obj=%08lx\n", __FUNC__, __LINE__, obj));
		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_About], 0);
		break;

	case MUIM_ScalosPrefs_LoadNamedConfig:
		{
		struct MUIP_ScalosPrefs_LoadNamedConfig *lnc = (struct MUIP_ScalosPrefs_LoadNamedConfig *) msg;

		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_LoadNamedConfig obj=%08lx\n", __FUNC__, __LINE__, obj));

		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		ReadPrefs(inst, lnc->ConfigFileName, ENV_DEFICONS_PREFS);
		inst->fpb_SaveFlags = FTWRITEFLAG_ONLY_SAVE_CHANGED | FTWRITEFLAG_CLEAR_CHANGE_FLAG;
		}
		break;

	case MUIM_ScalosPrefs_CreateSubWindows:
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_CreateSubWindows obj=%08lx\n", __FUNC__, __LINE__, obj));
		result = (ULONG) CreateSubWindows(cl, obj);
		break;

	case MUIM_ScalosPrefs_PageActive:
		{
		struct MUIP_ScalosPrefs_PageActive *spa = (struct MUIP_ScalosPrefs_PageActive *) msg;

		inst = (struct FileTypesPrefsInst *) INST_DATA(cl, obj);
		inst->fpb_PageIsActive = spa->isActive;
		d1(kprintf(__FILE__ "/%s/%ld: MUIM_ScalosPrefs_PageActive  isActive=%08lx  inst=%08lx\n", __FUNC__, __LINE__, spa->isActive, inst));
		}
		break;

	case MUIM_ScalosPrefs_GetListOfMCCs:
		result = (ULONG) RequiredMccList;
		break;

	default:
		d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));
		result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	d1(kprintf("%s/%ld:  END inst=%08lx\n", __FUNC__, __LINE__, inst));
	return result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static Object *CreatePrefsGroup(struct FileTypesPrefsInst *inst)
{
	d1(kprintf("%s/%ld:  inst=%08lx\n", __FUNC__, __LINE__, inst));

	inst->fpb_Objects[OBJNDX_ContextMenuFileTypes] = MUI_MakeObject(MUIO_MenustripNM, ContextMenuFileTypes, 0);
	if (NULL == inst->fpb_Objects[OBJNDX_ContextMenuFileTypes])
		return NULL;

	inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions] = MUI_MakeObject(MUIO_MenustripNM, ContextMenuFileTypesActions, 0);
	if (NULL == inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions])
		return NULL;

	inst->fpb_Objects[OBJNDX_Menu_ExpandFileTypes] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuFileTypes],
			MUIM_FindUData, HOOKNDX_ExpandFileTypes);
	inst->fpb_Objects[OBJNDX_Menu_CollapseFileTypes] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuFileTypes],
			MUIM_FindUData, HOOKNDX_CollapseFileTypes);

	inst->fpb_Objects[OBJNDX_Menu_Find] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuFileTypes],
			MUIM_FindUData, HOOKNDX_ShowFindGroup);

	inst->fpb_Objects[OBJNDX_ContextMenu] = MUI_MakeObject(MUIO_MenustripNM, ContextMenus, 0);
	if (NULL == inst->fpb_Objects[OBJNDX_ContextMenu])
		return NULL;

	inst->fpb_Objects[OBJNDX_Menu_Expand] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Expand);
	inst->fpb_Objects[OBJNDX_Menu_Collapse] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Collapse);

	inst->fpb_Objects[OBJNDX_Menu_Copy] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Copy);
	inst->fpb_Objects[OBJNDX_Menu_Cut] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Cut);
	inst->fpb_Objects[OBJNDX_Menu_Paste] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_Paste);

	inst->fpb_Objects[OBJNDX_Menu_HideEmptyEntries] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_HideEmptyEntries);


	inst->fpb_Objects[OBJNDX_ContextMenuAttrList] = MUI_MakeObject(MUIO_MenustripNM, ContextMenusAttrList, 0);
	if (NULL == inst->fpb_Objects[OBJNDX_ContextMenuAttrList])
		return NULL;

	inst->fpb_Objects[OBJNDX_Menu_CopyAttr] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuAttrList],
			MUIM_FindUData, HOOKNDX_CopyAttr);
	inst->fpb_Objects[OBJNDX_Menu_CutAttr] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuAttrList],
			MUIM_FindUData, HOOKNDX_CutAttr);
	inst->fpb_Objects[OBJNDX_Menu_PasteAttr] = (Object *) DoMethod(inst->fpb_Objects[OBJNDX_ContextMenuAttrList],
			MUIM_FindUData, HOOKNDX_PasteAttr);


	d1(kprintf("%s/%ld:  inst=%08lx\n", __FUNC__, __LINE__, inst));

	inst->fpb_Objects[OBJNDX_Group_Main] = VGroup,
		MUIA_Background, MUII_PageBack,

		// OBJNDX_ShadowListView is used as invisible storage
		// for copy and paste operations
		Child, inst->fpb_Objects[OBJNDX_ShadowListView] = NListviewObject,
			MUIA_ShowMe, FALSE,
			MUIA_CycleChain, TRUE,
			MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_ShadowListTree] = myNListTreeObject,
				MUIA_NList_PrivateData, inst,
				MUIA_NList_Format, "",
				MUIA_NListtree_DisplayHook, &inst->fpb_Hooks[HOOKNDX_TreeDisplay],
				MUIA_NListtree_ConstructHook, &inst->fpb_Hooks[HOOKNDX_TreeConstruct],
				MUIA_NListtree_DestructHook, &inst->fpb_Hooks[HOOKNDX_TreeDestruct],
				End, // myNListTreeClass
			End, // NListviewObject

		Child, inst->fpb_Objects[OBJNDX_ShadowAttrListView] = NListviewObject,
			MUIA_ShowMe, FALSE,
			MUIA_CycleChain, TRUE,
			MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_ShadowAttrList] = myNListObject,
				MUIA_NList_PrivateData, inst,
				MUIA_NList_Format, ",",
				InputListFrame,
				MUIA_Background, MUII_ListBack,
				MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_AttrDisplay],
				MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_AttrConstruct],
				MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_AttrDestruct],
				MUIA_NList_DragSortable, FALSE,
				MUIA_NList_ShowDropMarks, FALSE,
				MUIA_NList_Title, TRUE,
				MUIA_NList_TitleSeparator, TRUE,
				End, // myNListTreeClass

			MUIA_Listview_DragType, MUIV_Listview_DragType_None,
			MUIA_Weight, 50,
			End, // NListviewObject

		Child, RegisterObject,
			MUIA_Register_Titles, RegisterTitles,
			MUIA_CycleChain, TRUE,

			// --- filetypes
			Child, VGroup,
				Child, HGroup,
					MUIA_Weight, 200,
					Child, VGroup,
						Child, VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPES_FILETYPES),
							GroupFrame,

							Child, inst->fpb_Objects[OBJNDX_Group_FindFiletype] = HGroup,
								GroupFrame,
								Child, inst->fpb_Objects[OBJNDX_Button_HideFind] = TextObject,
									MUIA_Weight, 10,
									ButtonFrame,
									MUIA_CycleChain, TRUE,
#if defined(MUII_Close)
									MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_Close) "]",
#else
									MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_TapeStop) "]",
#endif
									MUIA_InputMode, MUIV_InputMode_RelVerify,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShortHelp, GetLocString(MSGID_BUTTON_FIND_HIDE_SHORTHELP),
									End,

								Child, inst->fpb_Objects[OBJNDX_String_FindFileType] = StringObject,
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_ShortHelp, GetLocString(MSGID_STRING_FIND_SHORTHELP),
									End, //StringObject
								Child, inst->fpb_Objects[OBJNDX_Button_FindPrevFileType] = TextObject,
									MUIA_Weight, 10,
									ButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_TapePlayBack) "]",
									MUIA_InputMode, MUIV_InputMode_RelVerify,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShortHelp, GetLocString(MSGID_BUTTON_FIND_PREV_SHORTHELP),
									End,
								Child, inst->fpb_Objects[OBJNDX_Button_FindNextFileType] = TextObject,
									MUIA_Weight, 10,
									ButtonFrame,
									MUIA_CycleChain, TRUE,
									MUIA_Text_Contents, MUIX_C "\33I[6:" STR(MUII_TapePlay) "]",
									MUIA_InputMode, MUIV_InputMode_RelVerify,
									MUIA_Background, MUII_ButtonBack,
									MUIA_ShortHelp, GetLocString(MSGID_BUTTON_FIND_NEXT_SHORTHELP),
									End,
								Child, inst->fpb_Objects[OBJNDX_Button_FindHitCount] = TextObject,
									MUIA_Weight, 10,
									MUIA_Text_Contents, "",
									End, //TextObject

								End, //HGroup

							Child, inst->fpb_Objects[OBJNDX_NListview_FileTypes] = NListviewObject,
								MUIA_CycleChain, TRUE,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_LISTVIEW_FILETYPES),
								MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_NListtree_FileTypes] = myFileTypesNListTreeObject,
									MUIA_Background, MUII_ListBack,
									MUIA_NList_PrivateData, inst,
									MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenuFileTypes],
									MUIA_NListtree_DisplayHook, &inst->fpb_Hooks[HOOKNDX_FileTypesDisplay],
									MUIA_NListtree_ConstructHook, &inst->fpb_Hooks[HOOKNDX_FileTypesConstruct],
									MUIA_NListtree_DestructHook, &inst->fpb_Hooks[HOOKNDX_FileTypesDestruct],
									MUIA_NListtree_DragDropSort, TRUE,
									MUIA_NList_ShowDropMarks, TRUE,
									MUIA_NListtree_EmptyNodes, TRUE,
									MUIA_NListtree_AutoVisible, MUIV_NListtree_AutoVisible_Expand,
								End, //myFileTypesNListTreeClass
							End, //NListtreeObject

							Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Name] = StringObject,
								MUIA_CycleChain, TRUE,
								MUIA_Disabled, TRUE,
								MUIA_String_AdvanceOnCR, TRUE,
								MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPES),
								StringFrame,
								End, //StringObject

							Child, ColGroup(2),
								Child, inst->fpb_Objects[OBJNDX_Button_FileTypes_Add] = KeyButtonHelp(MSGID_BUTTON_ADD_FILETYPE,
									'a',
									MSGID_SHORTHELP_BUTTON_ADD_FILETYPE),
								Child, inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove] = KeyButtonHelp(MSGID_BUTTON_DELETE_FILETYPE,
									'r',
									MSGID_SHORTHELP_BUTTON_DELETE_FILETYPE),
								End, //ColGroup
							End, //VGroup
						End, //VGroup

					Child, VGroup,
						Child, HVSpace,
						MUIA_Weight, 10,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_DEFAULTICON),

						Child, inst->fpb_Objects[OBJNDX_Text_FileTypes_FileTypesName] = TextObject,
							MUIA_ShowMe, FALSE,
							MUIA_Text_Contents, "",
							MUIA_Text_PreParse, MUIX_B "\33c",
							End, //TextObject

						Child, HVSpace,

						Child, inst->fpb_Objects[OBJNDX_Text_FileTypes_DefIconName] = TextObject,
							MUIA_Text_Contents, "",
							MUIA_Text_PreParse, "\33c",
							MUIA_Text_SetMin, TRUE,
							End, //TextObject
						Child, inst->fpb_Objects[OBJNDX_IconObjectMCC_FileTypes_Icon] = IconobjectMCCObject,
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							NoFrame,
							MUIA_ShowSelState, FALSE,
							End, //IconobjectMCCObject

						Child, HVSpace,

						Child, inst->fpb_Objects[OBJNDX_Button_FileTypes_CreateIcon] = KeyButtonHelp(MSGID_BUTTON_CREATE_ICON,
							'i',
							MSGID_SHORTHELP_BUTTON_CREATE_ICON),
						
                                                Child, HVSpace,
						End, //VGroup
					End, //HGroup

				Child, BalanceObject,
					End, // BalanceObject

				Child, VGroup,
					MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPES_METHODS),
					GroupFrame,
					MUIA_Weight, 50,

					Child, inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods] = NListviewObject,
						MUIA_CycleChain, TRUE,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_LISTVIEW_FILETYPE_ACTION),
						MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_NList_FileTypes_Actions] = myFileTypesActionsNListObject,
							MUIA_NList_PrivateData, inst,
							MUIA_NList_Format, "BAR,",
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenuFileTypesActions],
							MUIA_NList_DragSortable, TRUE,
							MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_FileTypesActionDisplay],
							MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_FileTypesActionConstruct],
							MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_FileTypesActionDestruct],
						End, //myFileTypesActionsNListClass
					End, //NListviewObject

					Child, ColGroup(3),
						Child, inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add] = PopobjectObject,
							MUIA_Disabled, TRUE,
							MUIA_UserData, inst,
							MUIA_CycleChain, TRUE,
							MUIA_Popstring_Button, KeyButtonHelp(MSGID_BUTTON_ADD_FILETYPEACTION,
								'a',
								MSGID_SHORTHELP_BUTTON_ADD_FILETYPEACTION),
							MUIA_Popobject_Object, inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add] = NListviewObject,
								MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_Nlist_FileTypes_Methods_Add] = NListObject,
									InputListFrame,
									MUIA_Background, MUII_ListBack,
									MUIA_NList_AdjustWidth, TRUE,
									End, //NListObject
								End, //NListviewObject
							MUIA_Popstring_CloseHook, &inst->fpb_Hooks[HOOKNDX_AddFileTypesMethod],
							End, //PopobjectObject
						Child, inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove] = KeyButtonHelp(MSGID_BUTTON_REMOVE_FILETYPEACTION,
							'r',
							MSGID_SHORTHELP_BUTTON_REMOVE_FILETYPEACTION),
						Child, inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn] = KeyButtonHelp(MSGID_BUTTON_LEARN_FILETYPEACTION,
							'l',
							MSGID_SHORTHELP_BUTTON_LEARN_FILETYPEACTION),
						End, //ColGroup
					End, //VGroup

				Child, BalanceObject,
					End, // BalanceObject

				Child, ScrollgroupObject,
					MUIA_Weight, 25,
					MUIA_Scrollgroup_Contents, VirtgroupObject,
						MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_VIRTGROUP_FILETYPE_ACTION_ATTRIBUTES),

						// empty placeholder
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Empty] = VGroup,
							MUIA_ShowMe, TRUE,
							Child, HVSpace,
							End, //VGroup

						// for MATCH
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Match] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_MATCH),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_MATCH_OFFSET)),
								Child, inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Offset] = StringObject,
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_Integer, 0,
									MUIA_String_AdvanceOnCR, TRUE,
									MUIA_String_Accept , "0123456879",
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_MATCH_OFFSET),
									End, //StringObject
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_MATCH_CASE_SENSITIVE)),
								Child, inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Match_Case] = KeyCheckMark(FALSE, "v"),
								End, //HGroup

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_MATCH_MATCH)),
								Child, inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_MATCH),
									MUIA_CycleChain, TRUE,
									MUIA_String_AdvanceOnCR, TRUE,
									StringFrame,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for SEARCH
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Search] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_SEARCH),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, HVSpace,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_SEARCH_SKIP_SPACES)),
								Child, inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces] = KeyCheckMark(FALSE, "v"),
								Child, HVSpace,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_SEARCH_CASE_SENSITIVE)),
								Child, inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case] = KeyCheckMark(FALSE, "v"),
								Child, HVSpace,
								End, //HGroup

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_SEARCH_SEARCH)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_SEARCH),
									MUIA_CycleChain, TRUE,
									MUIA_String_AdvanceOnCR, TRUE,
									StringFrame,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for FILESIZE
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Filesize] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_FILESIZE),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_FILESIZE_FILESIZE)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Filesize_Filesize] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_FILESIZE),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_Integer, 0,
									MUIA_String_Accept , "0123456879",
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for MINSIZEMB
						Child, inst->fpb_Objects[OBJNDX_Group_FileTypes_Action_MinSizeMB] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_MINSIZEMB),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_MINSIZEMB_SIZE)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_MINSIZEMB),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_Integer, 0,
									MUIA_String_Accept , "0123456879",
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup
						// for PATTERN
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Pattern] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_PATTERN),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PATTERN_PATTERN)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Pattern_Pattern] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PATTERN),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for DOSDEVICE
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_DosDevice] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_DOSDEVICE),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_DOSDEVICE_PATTERN)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosDevice_Pattern] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_DOSDEVICE),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for DEVICENAME
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_DeviceName] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_DEVICENAME),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_DEVICENAME_PATTERN)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DeviceName_Pattern] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_DEVICENAME),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for CONTENTS
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Contents] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_CONTENTS),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_CONTENTS_PATTERN)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Contents_Pattern] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_CONTENTS),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for DOSTYPE
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_DosType] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_DOSTYPE),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, HGroup,
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_DOSTYPE_PATTERN)),
								Child, inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosType_Pattern] = StringObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_DOSTYPE),
									MUIA_CycleChain, TRUE,
									StringFrame,
									MUIA_String_AdvanceOnCR, TRUE,
									End, //StringObject
								End, //HGroup
							Child, HVSpace,
							End, //VGroup

						// for PROTECTION
						Child, inst->fpb_Objects[OBJNDX_Group_Filetypes_Actions_Protection] = VGroup,
							MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_FILETYPESACTION_PROTECTION),
							GroupFrame,
							MUIA_ShowMe, FALSE,

							Child, ColGroup(4),
								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_R)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_R),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_W)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_W),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_E)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_E),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_D)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_D),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_S)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_S),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_P)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_P),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_A)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_A),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								Child, Label2((ULONG) GetLocString(MSGID_FILETYPEACTION_PROTECTION_H)),
								Child, inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H] = CycleObject,
									MUIA_ShortHelp, (ULONG) GetLocString(MSGID_SHORTHELP_STRING_FILETYPEACTION_PROTECTION_H),
									MUIA_CycleChain, TRUE,
		                                                        MUIA_Font, MUIV_Font_Button,
									MUIA_Cycle_Entries, FileTypeActionProtectionStrings,
									End, //CycleObject

								End, //ColGroup
							Child, HVSpace,
							End, //VGroup
						End, //VirtualGroup
					End, //Scrollgroup

				End, //VGroup

			// --- filetype actions
			Child, VGroup,
				Child, inst->fpb_Objects[OBJNDX_MainListView] = NListviewObject,
					MUIA_CycleChain, TRUE,
					MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_MainListTree] = myNListTreeObject,
						MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenu],
						MUIA_NList_PrivateData, inst,
						MUIA_NList_Format, "",
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						MUIA_NListtree_DisplayHook, &inst->fpb_Hooks[HOOKNDX_TreeDisplay],
						MUIA_NListtree_ConstructHook, &inst->fpb_Hooks[HOOKNDX_TreeConstruct],
						MUIA_NListtree_DestructHook, &inst->fpb_Hooks[HOOKNDX_TreeDestruct],
						MUIA_NListtree_DragDropSort, TRUE,
						MUIA_NList_ShowDropMarks, TRUE,
						MUIA_NListtree_EmptyNodes, TRUE,
						MUIA_NListtree_AutoVisible, MUIV_NListtree_AutoVisible_Expand,
						End, // myNListTreeClass

					MUIA_Listview_DragType, MUIV_Listview_DragType_None,
					End, // NListviewObject

				Child, inst->fpb_Objects[OBJNDX_HiddenListView] = NListviewObject,
					MUIA_ShowMe, FALSE,
					MUIA_CycleChain, TRUE,
					MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_HiddenListTree] = myNListTreeObject,
						MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenu],
						MUIA_NList_PrivateData, inst,
						MUIA_NList_Format, "",
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						MUIA_NListtree_DisplayHook, &inst->fpb_Hooks[HOOKNDX_TreeDisplay],
						MUIA_NListtree_ConstructHook, &inst->fpb_Hooks[HOOKNDX_TreeConstruct],
						MUIA_NListtree_DestructHook, &inst->fpb_Hooks[HOOKNDX_TreeDestruct],
						MUIA_NListtree_DragDropSort, TRUE,
						MUIA_NList_ShowDropMarks, TRUE,
						MUIA_NListtree_EmptyNodes, TRUE,
						MUIA_NListtree_AutoVisible, MUIV_NListtree_AutoVisible_Expand,
						End, // myNListTreeClass

					MUIA_Listview_DragType, MUIV_Listview_DragType_None,
					End, // NListviewObject

				Child, HGroup,
					Child, inst->fpb_Objects[OBJNDX_Lamp_Changed] = LampObject,
						MUIA_Lamp_Type, MUIV_Lamp_Type_Huge, 
						MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_LAMP_CHANGED),
						End, //LampObject

					Child, ColGroup(2),
						Child, inst->fpb_Objects[OBJNDX_AddEntryPopObject] = PopobjectObject,
							MUIA_Disabled, TRUE,
							MUIA_CycleChain, TRUE,
							MUIA_Popstring_Button, inst->fpb_Objects[OBJNDX_ButtonAddEntry] = KeyButtonHelp(MSGID_BUTTON_ADD_ENTRY,
								'a',
								MSGID_SHORTHELP_BUTTON_ADD_ENTRY),
							MUIA_Popobject_Object, inst->fpb_Objects[OBJNDX_AddEntryListView] = NListviewObject,
								MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_AddEntryList] = NListObject,
									InputListFrame,
									MUIA_Background, MUII_ListBack,
									MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_AddEntryListConstruct],
									MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_AddEntryListDestruct],
									MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_AddEntryListDisplay],
									MUIA_NList_AdjustWidth, TRUE,
									End, //NListObject
								End, //NListviewObject
							End, //PopobjectObject

						Child, inst->fpb_Objects[OBJNDX_ButtonRemoveEntry] = KeyButtonHelp(MSGID_BUTTON_DELETE_ENTRY,
								'd',
								MSGID_SHORTHELP_BUTTON_DELETE_ENTRY),
					End, //ColGroup
				End, //HGroup

				Child, BalanceObject,
					End, // BalanceObject

				Child, inst->fpb_Objects[OBJNDX_AttrListView] = NListviewObject,
					MUIA_CycleChain, TRUE,
					MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_AttrList] = myNListObject,
						MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenuAttrList],
						MUIA_NList_PrivateData, inst,
						MUIA_NList_Format, ",",
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_AttrDisplay],
						MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_AttrConstruct],
						MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_AttrDestruct],
						MUIA_NList_DragSortable, FALSE,
						MUIA_NList_ShowDropMarks, FALSE,
						MUIA_NList_Title, TRUE,
						MUIA_NList_TitleSeparator, TRUE,
						End, // myNListTreeClass

					MUIA_Listview_DragType, MUIV_Listview_DragType_None,
					MUIA_Weight, 50,
					End, // NListviewObject

				Child, ColGroup(3),
					Child, inst->fpb_Objects[OBJNDX_AddAttrPopObject] = PopobjectObject,
						MUIA_Disabled, TRUE,
						MUIA_CycleChain, TRUE,
						MUIA_Popstring_Button, inst->fpb_Objects[OBJNDX_ButtonAddAttribute] = KeyButtonHelp(MSGID_BUTTON_ADD_ATTRIBUTE,
							' ',
							MSGID_SHORTHELP_BUTTON_ADD_ATTRIBUTE),
						MUIA_Popobject_Object, inst->fpb_Objects[OBJNDX_AddAttrListView] = NListviewObject,
							MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_AddAttrList] = NListObject,
								InputListFrame,
								MUIA_Background, MUII_ListBack,
								MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_AddAttrListConstruct],
								MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_AddAttrListDestruct],
								MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_AddAttrListDisplay],
								MUIA_NList_AdjustWidth, TRUE,
								End, //NListObject
							End, //NListviewObject
						End, //PopobjectObject
					Child, inst->fpb_Objects[OBJNDX_ButtonEditAttribute] = KeyButtonHelp(MSGID_BUTTON_EDIT_ATTRIBUTE,
							'e',
							MSGID_SHORTHELP_BUTTON_EDIT_ATTRIBUTE),
					Child, inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute] = KeyButtonHelp(MSGID_BUTTON_DELETE_ATTRIBUTE,
							'r',
							MSGID_SHORTHELP_BUTTON_DELETE_ATTRIBUTE),
					End, //HGroup

				MUIA_ContextMenu, inst->fpb_Objects[OBJNDX_ContextMenu],

				End,	//VGroup

			End, //RegisterObject

		End; //VGroup

	if (NULL == inst->fpb_Objects[OBJNDX_Group_Main])
		return NULL;

	d1(kprintf("%s/%ld:  inst=%08lx\n", __FUNC__, __LINE__, inst));

	set(inst->fpb_Objects[OBJNDX_Menu_HideEmptyEntries], MUIA_Menuitem_Checked, inst->fpb_HideEmptyNodes);
	set(inst->fpb_Objects[OBJNDX_ButtonRemoveEntry], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Menu_CopyAttr], MUIA_Menuitem_Enabled, FALSE);
	set(inst->fpb_Objects[OBJNDX_Menu_CutAttr], MUIA_Menuitem_Enabled, FALSE);
	set(inst->fpb_Objects[OBJNDX_HiddenListView], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled, TRUE);
        
	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add], MUIA_Disabled, TRUE);

	DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Group_Main], 3, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ContextMenuTrigger], MUIV_TriggerValue );

	DoMethod(inst->fpb_Objects[OBJNDX_AddEntryListView], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->fpb_Objects[OBJNDX_AddEntryPopObject], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_AddEntry]);
	DoMethod(inst->fpb_Objects[OBJNDX_AddAttrListView], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->fpb_Objects[OBJNDX_AddAttrPopObject], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_AddAttribute]);

	// Call hook when Find Filetype Hide button is clicked
	DoMethod(inst->fpb_Objects[OBJNDX_Button_HideFind], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_HideFind], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_HideFindGroup]);

	// Call hook everytime the contents of StringFindFileType changes
	DoMethod(inst->fpb_Objects[OBJNDX_String_FindFileType], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FindFileType], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_IncrementalFindFileType]);

	// Call FindNextFileTypeHook when "Next" button is clicked
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FindNextFileType], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_IncrementalFindNextFileType]);

	// Call FindPrevFileTypeHook when "Prev" button is clicked
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_IncrementalFindPrevFileType]);

	// call hook on double click into "Add Method" popup listview
	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_AddFileTypesMethod]);

	// call hook when the "Learn" button is pressed
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_Learn_FileType] );

	// call hook everytime the filetype actions list is drag-drop sorted
	DoMethod(inst->fpb_Objects[OBJNDX_NList_FileTypes_Actions], MUIM_Notify, MUIA_NList_DragSortInsert, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_NList_FileTypes_Actions], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_DragDropSort_FileTypesAction] );

	// call hook everytime a new filetype is added
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FileTypes_Add], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FileTypes_Add], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_AddFileType] );

	// call hook everytime a new filetype is removed
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_RemoveFileType] );

	// call hook everytime a new filetype is selected
	DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_NListtree_FileTypes], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SelectFileTypesEntry] );

	// call hook everytime a new filetype is renamed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Name], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Name], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_RenameFileType] );

	// call hook everytime a new filetypes action is removed
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_RemoveFileTypeAction] );

	// setup hook if "create new" button is presseed for filetypes icon
	DoMethod(inst->fpb_Objects[OBJNDX_Button_FileTypes_CreateIcon], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_Button_FileTypes_CreateIcon], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_CreateNewFileTypesIcon] );

	// setup hook if filetypes icon is clicked
	DoMethod(inst->fpb_Objects[OBJNDX_IconObjectMCC_FileTypes_Icon], MUIM_Notify, MUIA_Pressed, FALSE,
		inst->fpb_Objects[OBJNDX_IconObjectMCC_FileTypes_Icon], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_EditFileTypesIcon] );

	// call hook if filetypes action MATCH attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Offset], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionMatchOffset] );
	DoMethod(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Match_Case], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Match_Case], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionMatchCase] );
	DoMethod(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionMatchMatch] );

	// call hook if filetypes action SEARCH attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionSearchSearch] );
	DoMethod(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionSearchCase] );
	DoMethod(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionSearchSkipSpaces] );

	// call hook if filetypes action FILESIZE attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Filesize_Filesize], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Filesize_Filesize], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionFilesize] );

	// call hook if filetypes action PATTERN attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Pattern_Pattern], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Pattern_Pattern], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionPattern] );

	// call hook if filetypes action DOSDEVICE attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosDevice_Pattern], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosDevice_Pattern], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionDosDevice] );

	// call hook if filetypes action DEVICENAME attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DeviceName_Pattern], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DeviceName_Pattern], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionDeviceName] );

	// call hook if filetypes action CONTENTS attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Contents_Pattern], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Contents_Pattern], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionContents] );

	// call hook if filetypes action DOSTYPE attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosType_Pattern], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosType_Pattern], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionDosType] );

	// call hook if filetypes action MINSIZEMB attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionMinSize] );

	// call hook if filetypes action PROTECTION attributes are changed
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );
	DoMethod(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangedFileTypesActionProtection] );

	// call hook everytime a filetypes action entry is selected
	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SelectFileTypesActionEntry] );

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SelectEntry] );

	DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SelectAttribute] );

	DoMethod(inst->fpb_Objects[OBJNDX_ButtonRemoveEntry], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_RemoveEntry]);

	DoMethod(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_RemoveAttribute]);

	// Edit Attribute by pressing "Edit" button of double-clicking attribute in list

	DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_EditAttribute] );

	DoMethod(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_EditAttribute]);

	// initially display icon for "project" filetype
	// "Create new" icon is disabled if "def_project" icon is present
	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_CreateIcon],
		MUIA_Disabled, DisplayFileTypesIcon(inst, "project"));

	return inst->fpb_Objects[OBJNDX_Group_Main];
}


static Object **CreateSubWindows(Class *cl, Object *o)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) INST_DATA(cl, o);

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	inst->fpb_SubWindows[0] = inst->fpb_Objects[OBJNDX_WIN_EditAttribute] = WindowObject,
		MUIA_Window_Title, GetLocString(MSGID_EDITATTRIBUTE_WINDOWTITLE),
		MUIA_Window_ID,	MAKE_ID('F','T','P','R'),
		WindowContents, VGroup,

			Child, TextObject,
				MUIA_Text_PreParse, MUIX_C,
				MUIA_Text_Contents, GetLocString(MSGID_EDITATTRIBUTE_TITLE),
				End, // TextObject

			Child, HGroup,
				TextFrame,
				Child, HVSpace,
				Child, inst->fpb_Objects[OBJNDX_Text_AttributeName] = TextObject,
					MUIA_HorizWeight, 200,
					MUIA_Text_PreParse, MUIX_C,
					MUIA_Text_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxx",
					MUIA_Text_SetMin, TRUE,
					End, // TextObject
				Child, HVSpace,
			End, //HGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeValue] = VGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_String_AttributeValue] = StringObject,
					MUIA_CycleChain, TRUE,
					StringFrame,
					MUIA_String_Contents, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_Text_SetMin, TRUE,
					End, // StringObject
			End, //VGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeAslPath] = VGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslPath] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popasl_StartHook, &inst->fpb_Hooks[HOOKNDX_EditAttributePopAslPathStartHook],
					MUIA_Popasl_Type, ASL_FileRequest,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_Popstring_String, KeyString("", MAX_ATTRVALUE, '\0'),
					MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				End, //PopaslObject
			End, //VGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeAslFile] = HGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_DtPic_AttributeSelectAslFile] = DataTypesImageObject,
					MUIA_ScaDtpic_Name, (ULONG) "",
					MUIA_ScaDtpic_FailIfUnavailable, FALSE,
					End, //DataTypesMCCObject

				Child, inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFile] = PopaslObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popasl_StartHook, &inst->fpb_Hooks[HOOKNDX_EditAttributePopAslFileStartHook],
					MUIA_Popasl_Type, ASL_FileRequest,
					MUIA_Popstring_String, KeyString("", MAX_ATTRVALUE, '\0'),
					MUIA_Popstring_Button, PopButton(MUII_PopFile),
				End, //PopaslObject
			End, //VGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeAslFont] = VGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFont] = PopaslObject,
					MUIA_CycleChain, TRUE,
					TAG_IGNORE, 993,
					MUIA_Popasl_Type, ASL_FontRequest,
					MUIA_Popstring_String, KeyString("", MAX_ATTRVALUE, '\0'),
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
				End, //PopaslObject

				Child, inst->fpb_Objects[OBJNDX_AslFont_Sample] = FontSampleObject,
					TextFrame,
					MUIA_Background, MUII_TextBack,
					MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					MUIA_FontSample_Antialias, inst->fpb_TTfAntialias,
					MUIA_FontSample_Gamma, inst->fpb_TTfGamma,
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_SAMPLETEXT_SHORTHELP),
				End, //FontSampleMCCObject
			End, //VGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeTTFont] = VGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_Pop_AttributeSelectTTFont] = PopstringObject,
					MUIA_CycleChain, TRUE,
					TAG_IGNORE, 994,
					MUIA_Popstring_String, KeyString("", MAX_ATTRVALUE, '\0'),
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_Popstring_OpenHook, &inst->fpb_Hooks[HOOKNDX_EditAttributeTTFontOpen],
					MUIA_Popstring_CloseHook, &inst->fpb_Hooks[HOOKNDX_EditAttributeTTFontClose],
				End, //PopstringObject

				Child, inst->fpb_Objects[OBJNDX_TTFont_Sample] = FontSampleObject,
					TextFrame,
					MUIA_Background, MUII_TextBack,
					MUIA_FontSample_Antialias, inst->fpb_TTfAntialias,
					MUIA_FontSample_Gamma, inst->fpb_TTfGamma,
					MUIA_FontSample_DemoString, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SAMPLETEXT),
					MUIA_ShortHelp, (ULONG) GetLocString(MSGID_TTFONTSPAGE_SCREENFONT_SAMPLETEXT_SHORTHELP),
				End, //FontSampleMCCObject
			End, //VGroup

			Child, inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue] = VGroup,
				MUIA_ShowMe, FALSE,

				Child, inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue] = PopobjectObject,
					MUIA_CycleChain, TRUE,
					MUIA_Popstring_Button, PopButton(MUII_PopUp),
					MUIA_Popstring_String, KeyString("", MAX_ATTRVALUE, '\0'),
					MUIA_Popobject_Object, inst->fpb_Objects[OBJNDX_Listview_AttributeSelectValue] = NListviewObject,
						MUIA_NListview_NList, inst->fpb_Objects[OBJNDX_List_AttributeSelectValue] = NListObject,
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NList_ConstructHook2, &inst->fpb_Hooks[HOOKNDX_EditAttributeListConstruct],
							MUIA_NList_DestructHook2, &inst->fpb_Hooks[HOOKNDX_EditAttributeListDestruct],
							MUIA_NList_DisplayHook2, &inst->fpb_Hooks[HOOKNDX_EditAttributeListDisplay],
							MUIA_NList_AdjustWidth, TRUE,
						End, //NListObject
					End, //NListviewObject
				End, //PopobjectObject
			End, //VGroup

		Child, ColGroup(2),
				Child, inst->fpb_Objects[OBJNDX_ButtonChangeAttribute] = KeyButtonHelp(MSGID_BUTTON_CHANGE_ATTRIBUTE,
					' ',
					MSGID_SHORTHELP_BUTTON_CHANGE_ATTRIBUTE),
				Child, inst->fpb_Objects[OBJNDX_ButtonKeepAttribute] = KeyButtonHelp(MSGID_BUTTON_KEEP_ATTRIBUTE,
					' ',
					MSGID_SHORTHELP_BUTTON_KEEP_ATTRIBUTE),
				End, //ColGroup
			End, //VGroup
		End;

	d1(KPrintF(__FILE__ "/%s/%ld: fpb_SubWindows[0]=%08lx\n", __FUNC__, __LINE__, inst->fpb_SubWindows[0]));

	inst->fpb_SubWindows[1] = NULL;

	set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont], MUIA_Disabled, NULL == TTEngineBase);

	// Selecting a new ASL font updates Asl font sample
	DoMethod(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFont], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, 
		inst->fpb_Objects[OBJNDX_AslFont_Sample], 3, MUIM_Set, MUIA_FontSample_StdFontDesc, MUIV_TriggerValue);

	// Pressing "change" button in "edit attribute" window calls hook
	DoMethod(inst->fpb_Objects[OBJNDX_ButtonChangeAttribute], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->fpb_Objects[OBJNDX_Group_Main], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_ChangeAttribute]);

	// Pressing "cancel" button in "edit attribute" window closes window
	DoMethod(inst->fpb_Objects[OBJNDX_ButtonKeepAttribute], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->fpb_Objects[OBJNDX_WIN_EditAttribute], 3, MUIM_Set, MUIA_Window_Open, FALSE);

	// Doubleclick in "edit attribute" selection listview calls hook
	DoMethod(inst->fpb_Objects[OBJNDX_Listview_AttributeSelectValue], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue], 2, MUIM_CallHook, &inst->fpb_Hooks[HOOKNDX_SelectAttributeValue]);

	// Update sample image everytime the image file string changes
	DoMethod(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFile], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->fpb_Objects[OBJNDX_DtPic_AttributeSelectAslFile], 3, MUIM_Set, MUIA_ScaDtpic_Name, MUIV_TriggerValue);

	return inst->fpb_SubWindows;
}


static Object *CreatePrefsImage(void)
{
#include "FileTypesPrefsImage.h"
	Object	*img;

	// First try to load datatypes image from THEME: tree
	img = DataTypesImageObject,
		MUIA_ScaDtpic_Name, (ULONG) "THEME:prefs/plugins/filetypes",
		MUIA_ScaDtpic_FailIfUnavailable, TRUE,
		End; //DataTypesMCCObject

	if (NULL == img)
		img = IMG(FileTypes, FILETYPES);        // use built-in fallback image

	return img;
}


static void InitHooks(struct FileTypesPrefsInst *inst)
{
	ULONG n;

	for (n=0; n<HOOKNDX_MAX; n++)
		{
		inst->fpb_Hooks[n] = FileTypesPrefsHooks[n];
		inst->fpb_Hooks[n].h_Data = inst;
		}
}


static BOOL OpenLibraries(void)
{
	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;
#endif

	DiskfontBase = OpenLibrary("diskfont.library", 39);
	if (NULL == DiskfontBase)
		return FALSE;
#ifdef __amigaos4__
	IDiskfont = (struct DiskfontIFace *)GetInterface((struct Library *)DiskfontBase, "main", 1, NULL);
	if (NULL == IDiskfont)
		return FALSE;
#endif

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

	IconobjectBase = OpenLibrary("iconobject.library", 0);
	if (NULL == IconobjectBase)
		return FALSE;
#ifdef __amigaos4__
	IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL);
	if (NULL == IIconobject)
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

	WorkbenchBase = OpenLibrary("workbench.library", 39);
	if (NULL == WorkbenchBase)
		return FALSE;
#ifdef __amigaos4__
	IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
	if (NULL == IWorkbench)
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

	PreferencesBase = OpenLibrary("preferences.library", 39);
	if (NULL == PreferencesBase)
		return FALSE;
#ifdef __amigaos4__
	IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
	if (NULL == IPreferences)
		return FALSE;
#endif

	DataTypesBase = OpenLibrary("datatypes.library", 39);
	if (NULL == DataTypesBase)
		return FALSE;
#ifdef __amigaos4__
	IDataTypes = (struct DataTypesIFace *)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
	if (NULL == IDataTypes)
		return FALSE;
#endif

	// TTEngineBase is optional
	TTEngineBase = OpenLibrary(TTENGINENAME, 6);
#ifdef __amigaos4__
	if (TTEngineBase)
		ITTEngine = (struct TTEngineIFace *)GetInterface(TTEngineBase, "main", 1, NULL);
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
	if (ITTEngine)
		{
		DropInterface((struct Interface *)ITTEngine);
		ITTEngine = NULL;
		}
#endif
	if (TTEngineBase)
		{
		CloseLibrary(TTEngineBase);
		TTEngineBase = NULL;
		}
#ifdef __amigaos4__
	if (IPreferences)
		{
		DropInterface((struct Interface *)IPreferences);
		IPreferences = NULL;
		}
#endif
	if (PreferencesBase)
		{
		CloseLibrary(PreferencesBase);
		PreferencesBase = NULL;
		}
#ifdef __amigaos4__
	if (IIconobject)
		{
		DropInterface((struct Interface *)IIconobject);
		IIconobject = NULL;
		}
#endif
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
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
	if (IWorkbench)
		{
		DropInterface((struct Interface *)IWorkbench);
		IWorkbench = NULL;
		}
#endif
	if (WorkbenchBase)
		{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
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
	if (IDiskfont)
		{
		DropInterface((struct Interface *)IDiskfont);
		IDiskfont = NULL;
		}
#endif
	if (DiskfontBase)
		{
		CloseLibrary(DiskfontBase);
		DiskfontBase = NULL;
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

static SAVEDS(APTR) INTERRUPT TreeConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm)
{
	struct FileTypesListEntry *fte = AllocPooled(ltcm->MemPool, sizeof(struct FileTypesListEntry));

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->MemPool, ltcm->UserData));
	d1(kprintf(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

	if (fte)
		{
		struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
		char UnSelIconName[MAX_ATTRVALUE];

		NewList(&fte->ftle_AttributesList);
		fte->ftle_FileFound = FALSE;
		fte->ftle_Changed = FALSE;

		GetAttributeValueString(FindAttribute(fte, ATTRTYPE_UnselIconName), UnSelIconName, sizeof(UnSelIconName));
		if (strlen(UnSelIconName) > 0)
			{
			fte->fte_ImageObject = DataTypesImageObject, 0,
				MUIA_ScaDtpic_Name, (ULONG) UnSelIconName,
				MUIA_ScaDtpic_FailIfUnavailable, TRUE,
				End; //DataTypesMCCObject

			if (fte->fte_ImageObject)
				{
				fte->fte_ImageIndex = ++inst->fpb_MenuImageIndex;
				DoMethod(obj, MUIM_NList_UseImage, fte->fte_ImageObject, fte->fte_ImageIndex, 0);
				}
			else
				{
				fte->fte_ImageIndex = 0;
				}
			}
		}

	return fte;
}

static SAVEDS(void) INTERRUPT TreeDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) ltdm->UserData;

	d1(kprintf(__FILE__ "/%s/%ld: mle=%08lx\n", __FUNC__, __LINE__, ltdm->UserData));

	if (fte)
		{
		CleanupAttributes(&fte->ftle_AttributesList);

		if (fte->fte_ImageObject)
			{
			DoMethod(obj, MUIM_NList_UseImage, NULL, fte->fte_ImageIndex, 0);

			MUI_DisposeObject(fte->fte_ImageObject);
			fte->fte_ImageObject = NULL;
			}
		}

	FreePooled(ltdm->MemPool, ltdm->UserData, sizeof(struct FileTypesListEntry));
	ltdm->UserData = NULL;
}

static SAVEDS(ULONG) INTERRUPT TreeDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) ltdm->TreeNode->tn_User;

	d1(kprintf(__FILE__ "/%s/%ld: <%s> fte=%08lx\n", __FUNC__, __LINE__, ltdm->TreeNode->tn_Name, fte));

	if (fte)
		{
		const struct FtAttribute *fta;
		char Line[MAX_ATTRVALUE];

		// indicate changed entries
		if (fte->ftle_Changed)
			ltdm->Preparse[0] = MUIX_PH;
		else
			ltdm->Preparse[0] = MUIX_PT;

		switch (fte->ftle_EntryType)
			{
		case ENTRYTYPE_ToolTip_Member:
			fta = FindAttribute(fte, ATTRTYPE_MemberHideString);
			if (fta)
				{
				GetAttributeValueString(fta, Line, sizeof(Line));
				snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
					"%s" MUIX_I " %s %s" MUIX_N,
					ltdm->TreeNode->tn_Name, GetAttributeName(fta->fta_Type), Line);
				}
			else
				{
				snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
					"%s", ltdm->TreeNode->tn_Name);
				}

			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_ToolTip_String:
			fta = FindAttribute(fte, ATTRTYPE_StringId);
			if (NULL == fta)
				fta = FindAttribute(fte, ATTRTYPE_StringText);
			if (NULL == fta)
				fta = FindAttribute(fte, ATTRTYPE_StringSrc);

			GetAttributeValueString(fta, Line, sizeof(Line));
			snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
				"%s" MUIX_I " %s %.*s" MUIX_N,
				ltdm->TreeNode->tn_Name, GetAttributeName(fta->fta_Type), MAX_FTE_NAME, Line);
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_ToolTip_DtImage:
			GetAttributeValueString(FindAttribute(fte, ATTRTYPE_DtImageName), Line, sizeof(Line));
			snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
				"%s" MUIX_I " FILENAME %.*s" MUIX_N,
				ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_PopupMenu_InternalCmd:
		case ENTRYTYPE_PopupMenu_WbCmd:
		case ENTRYTYPE_PopupMenu_ARexxCmd:
		case ENTRYTYPE_PopupMenu_CliCmd:
		case ENTRYTYPE_PopupMenu_PluginCmd:
		case ENTRYTYPE_PopupMenu_IconWindowCmd:
			GetAttributeValueString(FindAttribute(fte, ATTRTYPE_CommandName), Line, sizeof(Line));
			snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
				"%s" MUIX_I " NAME %.*s" MUIX_N,
				ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_PopupMenu_SubMenu:
			GetAttributeValueString(FindAttribute(fte, ATTRTYPE_MenuItemName), Line, sizeof(Line));
			if (fte->fte_ImageObject)
				{
				snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
					 "\33o[%ld]" "%s" MUIX_I " NAME %.*s" MUIX_N,
					(long) fte->fte_ImageIndex, ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
				}
			else
				{
				snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
					"%s" MUIX_I " NAME %.*s" MUIX_N,
					ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
				}
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_ToolTip_Group:
			GetAttributeValueString(FindAttribute(fte, ATTRTYPE_GroupOrientation), Line, sizeof(Line));
			snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
				"%s" MUIX_I " ORIENTATION %.*s" MUIX_N,
				ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		case ENTRYTYPE_PopupMenu_MenuItem:
			GetAttributeValueString(FindAttribute(fte, ATTRTYPE_MenuItemName), Line, sizeof(Line));

			if (FindAttribute(fte, ATTRTYPE_MenuDefaultAction))
				{
				if (fte->fte_ImageObject)
					{
					snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
						"\33o[%ld]" MUIX_B "%s" MUIX_I " NAME %.*s" MUIX_N,
						fte->fte_ImageIndex, ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
					}
				else
					{
					snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
						MUIX_B "%s" MUIX_I " NAME %.*s" MUIX_N,
						ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
					}
				}
			else
				{
				if (fte->fte_ImageObject)
					{
					snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
						"\33o[%ld]%s" MUIX_I " NAME %.*s" MUIX_N,
						fte->fte_ImageIndex, ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
					}
				else
					{
					snprintf(fte->ftle_NameBuffer, sizeof(fte->ftle_NameBuffer),
						"%s" MUIX_I " NAME %.*s" MUIX_N,
						ltdm->TreeNode->tn_Name, MAX_FTE_NAME, Line);
					}
				}
			ltdm->Array[0] = fte->ftle_NameBuffer;
			break;

		default:
			ltdm->Array[0] = ltdm->TreeNode->tn_Name;
			break;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT AttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm)
{
	const struct FtAttribute *fta = (const struct FtAttribute *) ltcm->entry;
	struct AttrListEntry *ale = (struct AttrListEntry *) AllocPooled(ltcm->pool, sizeof(struct AttrListEntry) + fta->fta_Length);

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->MemPool, ltcm->entry));
	d1(kprintf(__FILE__ "/%s/%ld: fta=%08lx\n", __FUNC__, __LINE__, ale));

	if (ale)
		{
		memcpy(&ale->ale_Attribute, fta, sizeof(struct FtAttribute) + fta->fta_Length);
		}

	return ale;
}

static SAVEDS(void) INTERRUPT AttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct AttrListEntry *ale = (struct AttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: START fta=%08lx\n", __FUNC__, __LINE__, ale));

	if (ale)
		{
		FreePooled(ltdm->pool, ale, sizeof(struct AttrListEntry) + ale->ale_Attribute.fta_Length);
		ltdm->entry = NULL;
		}

	d1(kprintf(__FILE__ "/%s/%ld: END fta=%08lx\n", __FUNC__, __LINE__, ale));
}

static SAVEDS(ULONG) INTERRUPT AttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct AttrListEntry *ale = (struct AttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: <%s> fta=%08lx\n", __FUNC__, __LINE__, ltdm->TreeNode->tn_Name, fta));

	if (ale)
		{
		ltdm->preparses[0] = "";
		ltdm->preparses[1] = "";

		ltdm->strings[0] = (STRPTR) GetAttributeName(ale->ale_Attribute.fta_Type);
		ltdm->strings[1] = (STRPTR) GetAttributeValueString(&ale->ale_Attribute, 
			ale->ale_ValueString, sizeof(ale->ale_ValueString));
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "\033c";
		ltdm->preparses[1] = "\033c";

		ltdm->strings[0] = GetLocString(MSGID_ATTRCOLUMNTITLE_NAME);
		ltdm->strings[1] = GetLocString(MSGID_ATTRCOLUMNTITLE_VALUE);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT AddEntryConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm)
{
	struct AddEntryListEntry *ael = (struct AddEntryListEntry *) AllocPooled(ltcm->pool, sizeof(struct AddEntryListEntry));

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->pool, ltcm->entry));
	d1(kprintf(__FILE__ "/%s/%ld: ael=%08lx\n", __FUNC__, __LINE__, ael));

	if (ael)
		{		
		ael->ael_EntryType = (enum FtEntryType) ltcm->entry;
		d1(kprintf(__FILE__ "/%s/%ld: ael_EntryType=%lu\n", __FUNC__, __LINE__, ael->ael_EntryType));
		}

	return ael;
}

static SAVEDS(void) INTERRUPT AddEntryDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct AddEntryListEntry *ael = (struct AddEntryListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: ael=%08lx\n", __FUNC__, __LINE__, ael));

	if (ael)
		{
		FreePooled(ltdm->pool, ael, sizeof(struct AddEntryListEntry));
		ltdm->entry = NULL;
		}
}

static SAVEDS(ULONG) INTERRUPT AddEntryDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct AddEntryListEntry *ael = (struct AddEntryListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: ael=%08lx\n", __FUNC__, __LINE__, ael));

	if (ael)
		{
		ltdm->preparses[0] = "";

		ltdm->strings[0] = (STRPTR) GetNameFromEntryType(ael->ael_EntryType);
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "";

		ltdm->strings[0] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT AddAttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm)
{
	struct AddAttrListEntry *aal = (struct AddAttrListEntry *) AllocPooled(ltcm->pool, sizeof(struct AddAttrListEntry));

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->pool, ltcm->entry));
	d1(kprintf(__FILE__ "/%s/%ld: aal=%08lx\n", __FUNC__, __LINE__, aal));

	if (aal)
		{		
		aal->aal_EntryType = (enum FtEntryType) ltcm->entry;
		d1(kprintf(__FILE__ "/%s/%ld: aal_EntryType=%lu\n", __FUNC__, __LINE__, aal->aal_EntryType));
		}

	return aal;
}

static SAVEDS(void) INTERRUPT AddAttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct AddAttrListEntry *aal = (struct AddAttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: aal=%08lx\n", __FUNC__, __LINE__, aal));

	if (aal)
		{
		FreePooled(ltdm->pool, aal, sizeof(struct AddAttrListEntry));
		ltdm->entry = NULL;
		}
}

static SAVEDS(ULONG) INTERRUPT AddAttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct AddAttrListEntry *aal = (struct AddAttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: aal=%08lx\n", __FUNC__, __LINE__, aal));

	if (aal)
		{
		ltdm->preparses[0] = "";

		ltdm->strings[0] = (STRPTR) GetAttributeName(aal->aal_EntryType);
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "";

		ltdm->strings[0] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT EditAttrConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm)
{
	const struct EditAttrListEntry *newEntry = ltcm->entry;
	struct EditAttrListEntry *eal = (struct EditAttrListEntry *) AllocPooled(ltcm->pool, sizeof(struct EditAttrListEntry) + newEntry->eal_fta.fta_Length);

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->pool, ltcm->entry));
	d1(kprintf(__FILE__ "/%s/%ld: eal=%08lx\n", __FUNC__, __LINE__, eal));

	if (eal)
		{
		memcpy(eal, newEntry, sizeof(struct EditAttrListEntry) + newEntry->eal_fta.fta_Length);
		}

	return eal;
}

static SAVEDS(void) INTERRUPT EditAttrDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct EditAttrListEntry *eal = (struct EditAttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: START aal=%08lx\n", __FUNC__, __LINE__, eal));

	if (eal)
		{
		FreePooled(ltdm->pool, eal, sizeof(struct EditAttrListEntry) + eal->eal_fta.fta_Length);
		ltdm->entry = NULL;
		}

	d1(kprintf(__FILE__ "/%s/%ld: END aal=%08lx\n", __FUNC__, __LINE__, eal));
}

static SAVEDS(ULONG) INTERRUPT EditAttrDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct EditAttrListEntry *eal = (struct EditAttrListEntry *) ltdm->entry;

	d1(kprintf(__FILE__ "/%s/%ld: eal=%08lx\n", __FUNC__, __LINE__, eal));

	if (eal)
		{
		ltdm->preparses[0] = "";

		GetAttributeValueString(&eal->eal_fta, eal->eal_ValueString, sizeof(eal->eal_ValueString));

		if (eal->eal_fta.fta_Value2)
			{
			d1(KPrintF(__FILE__ "/%s/%ld: Type=%ld  <%s>\n", __FUNC__, __LINE__, \
				eal->eal_fta.fta_Type, eal->eal_ValueString));

			ltdm->strings[0] = GetLocString(eal->eal_fta.fta_Value2);
			ltdm->strings[1] = eal->eal_ValueString;
			}
		else
			{
			d1(KPrintF(__FILE__ "/%s/%ld: Type=%ld  <%s>\n", __FUNC__, __LINE__, \
				eal->eal_fta.fta_Type, eal->eal_ValueString));

			ltdm->strings[0] = eal->eal_ValueString;
			}
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "";
		ltdm->strings[0] = "";

		if (eal->eal_fta.fta_Value2)
			{
			ltdm->preparses[1] = "";
			ltdm->strings[1] = "";
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

STRPTR GetLocString(ULONG MsgId)
{
	struct ScalosFileTypes_LocaleInfo li;

	li.li_Catalog = FileTypesPrefsCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR) GetScalosFileTypesString(&li, MsgId);
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

static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}

//----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT AboutFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	MUI_Request(inst->fpb_Objects[OBJNDX_APP_Main], inst->fpb_Objects[OBJNDX_WIN_Main], 0, NULL, 
		GetLocString(MSGID_ABOUTREQOK), 
		GetLocString(MSGID_ABOUTREQFORMAT), 
		MajorVersion, MinorVersion, COMPILER_STRING, CURRENTYEAR);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Clear, NULL, 0);
	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	inst->fpb_SaveFlags = FTWRITEFLAG_CLEAR_CHANGE_FLAG;

	InitDefIcons(inst, NULL);

	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);

	return 0;
}


static SAVEDS(APTR) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

	ReadPrefs(inst, ENVARC_FILETYPES_DIR, ENVARC_DEFICONS_PREFS);

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
	inst->fpb_SaveFlags = FTWRITEFLAG_CLEAR_CHANGE_FLAG;

	return 0;
}

static SAVEDS(APTR) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);

	ReadPrefs(inst, ENV_FILETYPES_DIR, ENV_DEFICONS_PREFS);

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
	inst->fpb_SaveFlags = FTWRITEFLAG_CLEAR_CHANGE_FLAG;

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT HideEmptyEntriesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	ULONG Checked = FALSE;

	get(inst->fpb_Objects[OBJNDX_Menu_HideEmptyEntries], MUIA_Menuitem_Checked, &Checked);
	inst->fpb_HideEmptyNodes = Checked;

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);
	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	if (inst->fpb_HideEmptyNodes)
		HideEmptyNodes(inst);
	else
		ShowHiddenNodes(inst);

	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CollapseHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Close,
				MUIV_NListtree_Close_ListNode_Active,
				MUIV_NListtree_Close_TreeNode_All,
				0);
			}

		set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tn);
		}

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Active,
		tn,
		0);

	set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Open, 
		MUIV_NListtree_Open_ListNode_Active,
		MUIV_NListtree_Open_TreeNode_Active,
		0);

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
				MUIM_NListtree_Open, 
				MUIV_NListtree_Open_ListNode_Active,
				MUIV_NListtree_Open_TreeNode_All,
				0);
			}

		set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tn);
		}

	set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT CollapseAllHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Close, 
		MUIV_NListtree_Close_ListNode_Root,
		MUIV_NListtree_Close_TreeNode_All,
		0);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandAllHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Open, 
		MUIV_NListtree_Open_ListNode_Root,
		MUIV_NListtree_Open_TreeNode_All,
		0);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CollapseFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Active,
		MUIV_NListtree_Close_TreeNode_All,
		0);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_Open,
		MUIV_NListtree_Open_ListNode_Active,
		MUIV_NListtree_Open_TreeNode_Active,
		0);

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIM_NListtree_Open,
				MUIV_NListtree_Open_ListNode_Active,
				MUIV_NListtree_Open_TreeNode_All,
				0);
			}

		set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, (ULONG) tn);
		}

	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT CollapseAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Root,
		MUIV_NListtree_Close_TreeNode_All,
		0);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT ExpandAllFileTypesHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIM_NListtree_Open,
		MUIV_NListtree_Open_ListNode_Root,
		MUIV_NListtree_Open_TreeNode_All,
		0);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CopyHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	DoMethod(inst->fpb_Objects[OBJNDX_ShadowListTree], MUIM_NListtree_Clear);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		CopyFileTypesEntry(inst, 
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root, 
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
			tn, 
			OBJNDX_ShadowListTree, OBJNDX_MainListTree);
		}

	EnablePasteMenuEntry(inst);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT CutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	DoMethod(inst->fpb_Objects[OBJNDX_ShadowListTree], MUIM_NListtree_Clear);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		CopyFileTypesEntry(inst, 
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_GetEntry_ListNode_Root, 
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
			tn, 
			OBJNDX_ShadowListTree, OBJNDX_MainListTree);

		EntryHasChanged(inst, tn);

		DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Remove, 
			MUIV_NListtree_Remove_ListNode_Active,
			MUIV_NListtree_Remove_TreeNode_Active,
			0);
		}

	EnablePasteMenuEntry(inst);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT PasteHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnParent;
		struct MUI_NListtree_TreeNode *tnShadow;

		tnParent = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);

		tnShadow = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_ShadowListTree],
			MUIM_NListtree_GetEntry, 
			MUIV_NListtree_Insert_ListNode_Root,
			MUIV_NListtree_GetEntry_Position_Head,
			0);

		set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

		if (MayPasteBelow(inst, tn, tnShadow))
			{
			CopyFileTypesEntry(inst, 
				tnParent, tn,
				tnShadow,
				OBJNDX_MainListTree, OBJNDX_ShadowListTree);

			EntryHasChanged(inst, tnParent);
			}
		else if (MayPasteOnto(inst, tn, tnShadow))
			{
			CopyFileTypesEntry(inst, 
				tn, (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
				tnShadow,
				OBJNDX_MainListTree, OBJNDX_ShadowListTree);

			EntryHasChanged(inst, tn);
			}

		set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

		DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_Redraw, 
			tn,
			0);
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CopyAttrHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList], MUIM_NList_Clear);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);

	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);

		d1(kprintf("%s/%ld: Attr=%08lx\n", __FUNC__, __LINE__, Attr));
		}
	if (Attr)
		{
		d1(kprintf("%s/%ld: Attr=%08lx\n", __FUNC__, __LINE__, Attr));

		DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList],
			MUIM_NList_InsertSingle, 
			&Attr->ale_Attribute,
			MUIV_NList_Insert_Bottom);
		}

	EnablePasteAttrMenuEntry(inst);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT CutAttrHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList], MUIM_NList_Clear);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);
		}
	if (Attr)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

		DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList],
			MUIM_NList_InsertSingle, 
			&Attr->ale_Attribute,
			MUIV_NList_Insert_Bottom);

		// Remove attribute from fte's list
		RemoveAttribute(fte, Attr->ale_Attribute.fta_Type);

		DoMethod(inst->fpb_Objects[OBJNDX_AttrList], 
			MUIM_NList_Remove, 
			MUIV_NList_Remove_Active);

		EntryHasChanged(inst, tn);
		}

	EnablePasteAttrMenuEntry(inst);

	return NULL;
}

static SAVEDS(APTR) INTERRUPT PasteAttrHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		struct AttrListEntry *Attr = NULL;

		DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList],
			MUIM_NList_GetEntry, 
			0,
			&Attr);

		if (Attr)
			{
			set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

			if (MayPasteAttr(inst, fte, Attr))
				{
				struct FtAttribute *ftaNew;

				ftaNew = AddAttribute(fte, Attr->ale_Attribute.fta_Type, 
					Attr->ale_Attribute.fta_Length, Attr->ale_Attribute.fta_Data);

				if (ftaNew)
					{
					DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_NList_InsertSingle,
						ftaNew, MUIV_NList_Insert_Bottom);
					}

				UpdateMenuImage(inst, OBJNDX_MainListTree, fte);
				EntryHasChanged(inst, tn);
				}

			set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
			EnablePasteAttrMenuEntry(inst);
			}
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT AddEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct AddEntryListEntry *ael = NULL;

	DoMethod(inst->fpb_Objects[OBJNDX_AddEntryList], 
		MUIM_NList_GetEntry, 
		MUIV_NList_GetEntry_Active,
		&ael);

	d1(kprintf("%s/%ld:  inst=%08lx  ael=%08lx\n", __FUNC__, __LINE__, inst, ael));

	DoMethod(inst->fpb_Objects[OBJNDX_AddEntryPopObject], MUIM_Popstring_Close, TRUE);

	if (ael)
		{
		struct MUI_NListtree_TreeNode *tn;

		d1(kprintf("%s/%ld:  EntryType=%lu\n", __FUNC__, __LINE__, ael->ael_EntryType));

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			MUIV_NListtree_GetEntry_ListNode_Active,
			MUIV_NListtree_GetEntry_Position_Active,
			0);

		if (tn)
			{
			struct FileTypesListEntry *fte;
			struct MUI_NListtree_TreeNode *tnNew;

			d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

			set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

			tnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Insert, 
				GetNameFromEntryType(ael->ael_EntryType), NULL,
				tn,
				MUIV_NListtree_Insert_PrevNode_Tail,
				TNF_LIST | TNF_OPEN);

			fte = (struct FileTypesListEntry *) tnNew->tn_User;
			fte->ftle_EntryType = ael->ael_EntryType;

			AddDefaultAttributes(inst, fte);

			set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Redraw, 
				tn,
				0);

			UpdateAttributes(inst, (struct FileTypesListEntry *) tn->tn_User);

			// Make sure the new entry is the active one
			set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tnNew);

			EntryHasChanged(inst, tnNew);
			}
		}

	return NULL;
}


static SAVEDS(APTR) INTERRUPT AddAttributeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);

	d1(kprintf("%s/%ld:  inst=%08lx  tn=%08lx\n", __FUNC__, __LINE__, inst, tn));

	if (tn)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		struct AddAttrListEntry *aal = NULL;

		DoMethod(inst->fpb_Objects[OBJNDX_AddAttrList], 
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&aal);

		d1(kprintf("%s/%ld:  inst=%08lx  aal=%08lx\n", __FUNC__, __LINE__, inst, aal));

		if (aal)
			{
			const struct AttributeDef *atd;
			ULONG n;

			atd = GetAttributeDef(aal->aal_EntryType, fte->ftle_EntryType);

			// remove all attributes which are excluded by the new one
			for (n = 0; atd && n < atd->atd_ExcludeCount; n++)
				{
				const struct FtAttribute *fta;

				fta = FindAttribute(fte, atd->atd_Exclude[n]);

				if (fta)
					{
					ULONG Entries = 0;
					ULONG pos;

					// Remove attribute from fte's list
					RemoveAttribute(fte, fta->fta_Type);

					get(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Entries, &Entries);

					// Remove Attribute from Attribute List
					for (pos = 0; pos < Entries; pos++)
						{
						struct AttrListEntry *Attr = NULL;

						DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
							MUIM_NList_GetEntry, 
							pos,
							&Attr);
						if (Attr && Attr->ale_Attribute.fta_Type == atd->atd_Exclude[n])
							{
							DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
								MUIM_NList_Remove, 
								pos);
							}
						}
					}
				}

			AddNewAttribute(inst, fte, aal->aal_EntryType);
			SetAddableAttributes(inst, fte);

			UpdateMenuImage(inst, OBJNDX_MainListTree, fte);

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Redraw, 
				tn,
				0);

			if (ATTRTYPE_MenuDefaultAction == aal->aal_EntryType)
				BeginSetMenuDefaultAction(inst, tn);

			EntryHasChanged(inst, tn);
			}
		}

	DoMethod(inst->fpb_Objects[OBJNDX_AddAttrPopObject], MUIM_Popstring_Close, TRUE);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT RemoveEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	d1(kprintf("%s/%ld:  inst=%08lx\n", __FUNC__, __LINE__, inst));

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);

	if (tn)
		{
		EntryHasChanged(inst, tn);

		DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Remove, 
			MUIV_NListtree_Remove_ListNode_Active,
			MUIV_NListtree_Remove_TreeNode_Active,
			0);
		}

	return NULL;
}

static SAVEDS(APTR) INTERRUPT RemoveAttributeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	d1(kprintf("%s/%ld:  inst=%08lx\n", __FUNC__, __LINE__, inst));

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);

	d1(kprintf("%s/%ld:  inst=%08lx  tn=%08lx\n", __FUNC__, __LINE__, inst, tn));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);
		}

	d1(kprintf("%s/%ld: active Attr=%08lx\n", __FUNC__, __LINE__, Attr));
	if (Attr)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

		// Remove attribute from fte's list
		RemoveAttribute(fte, Attr->ale_Attribute.fta_Type);

		DoMethod(inst->fpb_Objects[OBJNDX_AttrList], 
			MUIM_NList_Remove, 
			MUIV_NList_Remove_Active);

		UpdateMenuImage(inst, OBJNDX_MainListTree, fte);

		DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_Redraw, 
			tn,
			0);

		EntryHasChanged(inst, tn);

		set(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Menu_CopyAttr], MUIA_Menuitem_Enabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Menu_CutAttr], MUIA_Menuitem_Enabled, FALSE);

		EnablePasteAttrMenuEntry(inst);
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ChangeAttributeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);
		}

	d1(kprintf("%s/%ld: active Attr=%08lx\n", __FUNC__, __LINE__, Attr));
	if (Attr)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		const struct AttributeDef *atd;

		d1(kprintf("%s/%ld: active fte=%08lx\n", __FUNC__, __LINE__, fte));

		atd = GetAttributeDef(Attr->ale_Attribute.fta_Type, fte->ftle_EntryType);
		d1(kprintf("%s/%ld: atd=%08lx\n", __FUNC__, __LINE__, atd));
		if (atd)
			{
			CONST_STRPTR ValueString = "";
			ULONG ValueULong = 0;

			switch (atd->atd_DefaultContents.avd_Type)
				{
			case ATTRDEFTYPE_ULong:
				if (atd->atd_NumberOfValues > 0)
					{
					get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue],
						MUIA_String_Contents, &ValueString);
					}
				else
					{
					get(inst->fpb_Objects[OBJNDX_String_AttributeValue],
						MUIA_String_Contents, &ValueString);
					}

				if (atd->atd_DefaultContents.avd_ConvertValueFromString)
					ValueULong = (atd->atd_DefaultContents.avd_ConvertValueFromString)(ValueString);
				else
					{
					unsigned long ulong1;
					sscanf(ValueString, "%lu", &ulong1);
					ValueULong = ulong1;
					}
				// Remove attribute from fte's list
				RemoveAttribute(fte,  atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, sizeof(ValueULong), &ValueULong);
				break;

			case ATTRDEFTYPE_FileName:
				get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFile],
					MUIA_String_Contents, &ValueString);

				// Remove attribute from fte's list
				RemoveAttribute(fte, atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, 1 + strlen(ValueString), ValueString);
				break;

			case ATTRDEFTYPE_PathName:
				get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslPath],
					MUIA_String_Contents, &ValueString);

				// Remove attribute from fte's list
				RemoveAttribute(fte, atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, 1 + strlen(ValueString), ValueString);
				break;

			case ATTRDEFTYPE_FontString:
				get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFont],
					MUIA_String_Contents, &ValueString);

				// Remove attribute from fte's list
				RemoveAttribute(fte, atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, 1 + strlen(ValueString), ValueString);
				break;

			case ATTRDEFTYPE_TTFontString:
				get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectTTFont],
					MUIA_String_Contents, &ValueString);

				// Remove attribute from fte's list
				RemoveAttribute(fte, atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, 1 + strlen(ValueString), ValueString);
				break;

			case ATTRDEFTYPE_LocString:
			case ATTRDEFTYPE_String:
			case ATTRDEFTYPE_String2:
				if (atd->atd_NumberOfValues > 0)
					{
					get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue],
						MUIA_String_Contents, &ValueString);
					}
				else
					{
					get(inst->fpb_Objects[OBJNDX_String_AttributeValue],
						MUIA_String_Contents, &ValueString);
					}
				// Remove attribute from fte's list
				RemoveAttribute(fte, atd->atd_Type);
				AddAttribute(fte, atd->atd_Type, 1 + strlen(ValueString), ValueString);
				break;
			default:
				break;
				}

			d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

			UpdateAttributes(inst, fte);
			UpdateMenuImage(inst, OBJNDX_MainListTree, fte);

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Redraw, 
				tn,
				0);

			EntryHasChanged(inst, tn);
			}
		}

	// Close "Edit Attribute" window
	set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIA_Window_Open, FALSE);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SelectAttributeValueHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct EditAttrListEntry *eal = NULL;

	DoMethod(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue], 
		MUIM_NList_GetEntry, 
		MUIV_NList_GetEntry_Active,
		&eal);

	if (eal)
		{
		set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue],
			MUIA_String_Contents, (ULONG) eal->eal_ValueString);
		}

	d1(kprintf("%s/%ld: eal=%08lx\n", __FUNC__, __LINE__, eal));

	DoMethod(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue], MUIM_Popstring_Close, TRUE);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT EditAttributePopAslFileStartHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct TagItem *TagList = (struct TagItem *) msg;
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	CONST_STRPTR ValueString;
	STRPTR lp;

	d1(KPrintF("%s/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFile], MUIA_String_Contents, &ValueString);

	d1(KPrintF("%s/%ld: ValueString=<%s>\n", __FUNC__, __LINE__, ValueString));

	while (TAG_END != TagList->ti_Tag)
		TagList++;

	stccpy(inst->fpb_FileName, FilePart(ValueString), sizeof(inst->fpb_FileName));
	stccpy(inst->fpb_Path, ValueString, sizeof(inst->fpb_Path));

	lp = PathPart(inst->fpb_Path);
	*lp = '\0';

	TagList->ti_Tag = ASLFR_DrawersOnly;
	TagList->ti_Data = FALSE;
	TagList++;

	TagList->ti_Tag = ASLFR_InitialFile;
	TagList->ti_Data = (ULONG) inst->fpb_FileName;
	TagList++;

	TagList->ti_Tag = ASLFR_InitialDrawer;
	TagList->ti_Data = (ULONG) inst->fpb_Path;
	TagList++;

	TagList->ti_Tag = TAG_END;

	d1(KPrintF("%s/%ld: ASLFR_InitialFile=<%s>  ASLFR_InitialDrawer=<%s>\n", __FUNC__, __LINE__, inst->fpb_FileName, inst->fpb_Path));

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT EditAttributePopAslPathStartHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct TagItem *TagList = (struct TagItem *) msg;
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	CONST_STRPTR ValueString;
	STRPTR lp;

	d1(kprintf("%s/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	get(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslPath], MUIA_String_Contents, &ValueString);

	while (TAG_END != TagList->ti_Tag)
		TagList++;

	stccpy(inst->fpb_FileName, FilePart(ValueString), sizeof(inst->fpb_FileName));
	stccpy(inst->fpb_Path, ValueString, sizeof(inst->fpb_Path));

	lp = PathPart(inst->fpb_Path);
	*lp = '\0';

	TagList->ti_Tag = ASLFR_DrawersOnly;
	TagList->ti_Data = TRUE;
	TagList++;

	TagList->ti_Tag = ASLFR_InitialFile;
	TagList->ti_Data = (ULONG) inst->fpb_FileName;
	TagList++;

	TagList->ti_Tag = ASLFR_InitialDrawer;
	TagList->ti_Data = (ULONG) inst->fpb_Path;
	TagList++;

	TagList->ti_Tag = TAG_END;

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT EditAttrTTFontOpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	APTR ttRequest;
	ULONG Success = 0;

	ttRequest = TT_AllocRequest();

	if (ttRequest)
		{
		struct Window *PrefsWindow;
		struct TagItem *AttrList;
		char FontName[MAX_ATTRVALUE];
		ULONG FontStyle, FontWeight, FontSize;
		STRPTR FontDesc;

		get(o, MUIA_String_Contents, &FontDesc);

		ParseTTFontFromDesc(FontDesc, &FontStyle, &FontWeight, 
			&FontSize, FontName, sizeof(FontName));

//		set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIA_Window_Sleep, TRUE);
		get(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIA_Window_Window, &PrefsWindow);

		//TT_RequestA()
		AttrList = TT_Request(ttRequest,
			TTRQ_Window, (ULONG) PrefsWindow,
			TTRQ_TitleText, (ULONG) GetLocString(MSGID_EDITATTRIBUTE_TTFONT_ASLTITLE),
			TTRQ_PositiveText, (ULONG) GetLocString(MSGID_EDITATTRIBUTE_TTFONT_ASL_OKBUTTON),
			TTRQ_NegativeText, (ULONG) GetLocString(MSGID_EDITATTRIBUTE_TTFONT_ASL_CANCELBUTTON),
			TTRQ_DoSizes, TRUE,
			TTRQ_DoStyle, TRUE,
			TTRQ_DoWeight, TRUE,
			TTRQ_Activate, TRUE,
			TTRQ_DoPreview, TRUE,
			TTRQ_InitialName, (ULONG) FontName,
			TTRQ_InitialSize, FontSize,
			TTRQ_InitialStyle, FontStyle,
			TAG_END);

		d1(kprintf("%s/%ld: AttrList=%08lx\n", __FUNC__, __LINE__, AttrList));

		if (AttrList)
			{
			char buffer[MAX_ATTRVALUE];

			BuildTTDescFromAttrList(buffer, sizeof(buffer), AttrList);

			d1(kprintf("%s/%ld: FontDesc=<%s>\n", __FUNC__, __LINE__, buffer));

			set(o, MUIA_String_Contents, (ULONG) buffer);
			set(inst->fpb_Objects[OBJNDX_TTFont_Sample], MUIA_FontSample_TTFontDesc, (ULONG) buffer);

			Success = 1;
			}

		TT_FreeRequest(ttRequest);
		}

	DoMethod(o, MUIM_Popstring_Close, Success);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT EditAttrTTFontCloseHookFunc(struct Hook *hook, Object *o, Msg msg)
{
//	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

//	set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIA_Window_Sleep, FALSE);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
//	struct MUI_NListtree_TreeNode *tn1;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));
#if 0
	tn1 = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		tn,
		MUIV_NListtree_GetEntry_Position_Head,
		0);
	d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn1, tn1 ? tn1->tn_Name : (STRPTR) ""));

	tn1 = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		tn,
		MUIV_NListtree_GetEntry_Position_Tail,
		0);
	d1(kprintf("%s/%ld: Tail tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn1, tn1 ? tn1->tn_Name : (STRPTR) ""));
#endif
	if (tn)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

		set(inst->fpb_Objects[OBJNDX_Menu_Expand], MUIA_Menuitem_Enabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Menu_Collapse], MUIA_Menuitem_Enabled, TRUE);

		switch (fte->ftle_EntryType)
			{
		case ENTRYTYPE_FileType:
		set(inst->fpb_Objects[OBJNDX_ButtonRemoveEntry], MUIA_Disabled, TRUE);
			set(inst->fpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, FALSE);
			set(inst->fpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, FALSE);
			break;
		case ENTRYTYPE_PopupMenu:
		case ENTRYTYPE_PopupMenu_SubMenu:
		case ENTRYTYPE_PopupMenu_MenuItem:
		case ENTRYTYPE_PopupMenu_InternalCmd:
		case ENTRYTYPE_PopupMenu_WbCmd:
		case ENTRYTYPE_PopupMenu_ARexxCmd:
		case ENTRYTYPE_PopupMenu_CliCmd:
		case ENTRYTYPE_PopupMenu_PluginCmd:
		case ENTRYTYPE_PopupMenu_IconWindowCmd:
		case ENTRYTYPE_PopupMenu_MenuSeparator:
		case ENTRYTYPE_ToolTip:
		case ENTRYTYPE_ToolTip_Group:
		case ENTRYTYPE_ToolTip_Member:
		case ENTRYTYPE_ToolTip_HBar:
		case ENTRYTYPE_ToolTip_String:
		case ENTRYTYPE_ToolTip_Space:
		case ENTRYTYPE_ToolTip_DtImage:
		default:
			set(inst->fpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, TRUE);
			set(inst->fpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, TRUE);
			set(inst->fpb_Objects[OBJNDX_ButtonRemoveEntry], MUIA_Disabled, FALSE);
			break;
			}

		UpdateAttributes(inst, fte);
		}
	else
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AddAttrList], MUIM_NList_Clear);
		set(inst->fpb_Objects[OBJNDX_Menu_Expand], MUIA_Menuitem_Enabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Menu_Collapse], MUIA_Menuitem_Enabled, FALSE);

		set(inst->fpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, FALSE);

		set(inst->fpb_Objects[OBJNDX_ButtonRemoveEntry], MUIA_Disabled, TRUE);
		}


	EnablePasteMenuEntry(inst);
	EnablePasteAttrMenuEntry(inst);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SelectAttributeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);
		}

	d1(kprintf("%s/%ld: active Attr=%08lx\n", __FUNC__, __LINE__, Attr));
	if (Attr)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		const struct AttributeDef *atd;

		d1(kprintf("%s/%ld: active fte=%08lx\n", __FUNC__, __LINE__, fte));

		set(inst->fpb_Objects[OBJNDX_Menu_CopyAttr], MUIA_Menuitem_Enabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Menu_CutAttr], MUIA_Menuitem_Enabled, TRUE);

		atd = GetAttributeDef(Attr->ale_Attribute.fta_Type, fte->ftle_EntryType);
		d1(kprintf("%s/%ld: atd=%08lx\n", __FUNC__, __LINE__, atd));
		if (atd)
			{
			set(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIA_Disabled, IsAttrRequired(fte, atd));

			// "Edit Attribute" button stays disabled when attribute has type ATTRDEFTYPE_None
			set(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIA_Disabled, 
				ATTRDEFTYPE_None == atd->atd_DefaultContents.avd_Type);
			}
		}
	else
		{
		set(inst->fpb_Objects[OBJNDX_Menu_CopyAttr], MUIA_Menuitem_Enabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Menu_CutAttr], MUIA_Menuitem_Enabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIA_Disabled, TRUE);
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT EditAttributeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	d1(KPrintF("%s/%ld: START\n", __FUNC__, __LINE__));

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(KPrintF("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList],
			MUIM_NList_GetEntry, 
			MUIV_NList_GetEntry_Active,
			&Attr);
		}

	d1(KPrintF("%s/%ld: active Attr=%08lx\n", __FUNC__, __LINE__, Attr));
	if (Attr)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		const struct AttributeDef *atd;

		atd = GetAttributeDef(Attr->ale_Attribute.fta_Type, fte->ftle_EntryType);
		d1(KPrintF("%s/%ld: atd=%08lx\n", __FUNC__, __LINE__, atd));
		if (atd)
			{
			char ValueString[MAX_ATTRVALUE];

			set(inst->fpb_Objects[OBJNDX_Text_AttributeName],
				MUIA_Text_Contents, (ULONG) GetAttributeName(atd->atd_Type));

			GetAttributeValueString(&Attr->ale_Attribute, ValueString, sizeof(ValueString));

			d1(KPrintF("%s/%ld: avd_Type=%ld\n", __FUNC__, __LINE__, atd->atd_DefaultContents.avd_Type));

			switch (atd->atd_DefaultContents.avd_Type)
				{
			case ATTRDEFTYPE_None:
				// nothing to edit here...
				return NULL;
				break;

			case ATTRDEFTYPE_PathName:
				set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],    		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFile],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslPath],		MUIA_ShowMe, TRUE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFont],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont],  		MUIA_ShowMe, FALSE);

				set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslPath],
					MUIA_String_Contents, (ULONG) ValueString);

				set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
					MUIA_Window_ActiveObject,
                                        inst->fpb_Objects[OBJNDX_Group_AttributeAslPath]);
				break;

			case ATTRDEFTYPE_FileName:
				set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFile],		MUIA_ShowMe, TRUE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslPath],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFont],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont],  		MUIA_ShowMe, FALSE);

				switch (atd->atd_Type)
					{
				case ATTRTYPE_DtImageName:
				case ATTRTYPE_UnselIconName:
				case ATTRTYPE_SelIconName:
					d1(KPrintF("%s/%ld: OBJNDX_DtPic_AttributeSelectAslFile=%08lx  ValueString=<%s>\n", \
						__FUNC__, __LINE__, inst->fpb_Objects[OBJNDX_DtPic_AttributeSelectAslFile], ValueString));
					set(inst->fpb_Objects[OBJNDX_DtPic_AttributeSelectAslFile], MUIA_ScaDtpic_Name, ValueString);
					break;
				default:
					set(inst->fpb_Objects[OBJNDX_DtPic_AttributeSelectAslFile], MUIA_ScaDtpic_Name, "");
					break;
					}

				set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFile],
					MUIA_String_Contents, (ULONG) ValueString);

				set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
					MUIA_Window_ActiveObject,
					inst->fpb_Objects[OBJNDX_Group_AttributeAslFile]);
				break;

			case ATTRDEFTYPE_FontString:
				set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslPath],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFile],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFont],		MUIA_ShowMe, TRUE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont],		MUIA_ShowMe, FALSE);

				set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectAslFont],
					MUIA_String_Contents, (ULONG) ValueString);
				set(inst->fpb_Objects[OBJNDX_AslFont_Sample], 
					MUIA_FontSample_StdFontDesc, (ULONG) ValueString);

				set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
					MUIA_Window_ActiveObject,
					inst->fpb_Objects[OBJNDX_Group_AttributeAslFont]);
				break;

			case ATTRDEFTYPE_TTFontString:
				set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslPath],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFile],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFont],		MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont],		MUIA_ShowMe, TRUE);

				set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectTTFont],
					MUIA_String_Contents, (ULONG) ValueString);
				set(inst->fpb_Objects[OBJNDX_TTFont_Sample],
					MUIA_FontSample_TTFontDesc, (ULONG) ValueString);

				set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
					MUIA_Window_ActiveObject,
					inst->fpb_Objects[OBJNDX_Group_AttributeTTFont]);
				break;

			case ATTRDEFTYPE_ULong:
				//... TBD
			case ATTRDEFTYPE_LocString:
			case ATTRDEFTYPE_String:
			case ATTRDEFTYPE_String2:
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFile],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslPath],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeAslFont],	MUIA_ShowMe, FALSE);
				set(inst->fpb_Objects[OBJNDX_Group_AttributeTTFont],	MUIA_ShowMe, FALSE);

				d1(KPrintF("%s/%ld: atd_NumberOfValues=%ld\n", __FUNC__, __LINE__, atd->atd_NumberOfValues));
				if (atd->atd_NumberOfValues > 0)
					{
					set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, TRUE);
					set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],		MUIA_ShowMe, FALSE);

					FillListOfAttributeValues(inst, fte, atd, Attr);

					set(inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue],
						MUIA_String_Contents, (ULONG) ValueString);

					set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
						MUIA_Window_ActiveObject,
						inst->fpb_Objects[OBJNDX_Pop_AttributeSelectValue]);
					}
				else
					{
					set(inst->fpb_Objects[OBJNDX_Group_AttributeSelectValue],	MUIA_ShowMe, FALSE);
					set(inst->fpb_Objects[OBJNDX_Group_AttributeValue],		MUIA_ShowMe, TRUE);

					set(inst->fpb_Objects[OBJNDX_String_AttributeValue],
						MUIA_String_Contents, (ULONG) ValueString);

					set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute],
						MUIA_Window_ActiveObject,
						inst->fpb_Objects[OBJNDX_String_AttributeValue]);
					}
				break;
				}

			DoMethod(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIM_Notify, MUIA_Window_Open, TRUE, 
				inst->fpb_Objects[OBJNDX_WIN_Main], 3, MUIM_Set, MUIA_Window_Sleep, TRUE);
			DoMethod(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIM_Notify, MUIA_Window_Open, FALSE, 
				inst->fpb_Objects[OBJNDX_WIN_Main], 3, MUIM_Set, MUIA_Window_Sleep, FALSE);

			d1(KPrintF("%s/%ld: atd=%08lx  OBJNDX_WIN_EditAttribute=%08lx\n", __FUNC__, __LINE__, atd, inst->fpb_Objects[OBJNDX_WIN_EditAttribute]));

			set(inst->fpb_Objects[OBJNDX_WIN_EditAttribute], MUIA_Window_Open, TRUE);

			d1(KPrintF("%s/%ld: atd=%08lx\n", __FUNC__, __LINE__, atd));
			}
		}

	d1(KPrintF("%s/%ld: END\n", __FUNC__, __LINE__));

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG MenuHookIndex = 0;
	struct Hook *MenuHook = NULL;

	d1(KPrintF("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
	d1(KPrintF("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

	get(MenuObj, MUIA_UserData, &MenuHookIndex);

	if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
		MenuHook = &inst->fpb_Hooks[MenuHookIndex];

	d1(KPrintF("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
	if (MenuHook)
		DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

	return NULL;
}

//----------------------------------------------------------------------------

static LONG ReadPrefs(struct FileTypesPrefsInst *inst,
	CONST_STRPTR LoadFileName, CONST_STRPTR DefIconPrefsName)
{
	d1(time_t now = time(NULL));

	d1(KPrintF(__FILE__ "/%s/%ld: LoadFileName=<%s>  DefIconPrefsName=<%s>\n", __FUNC__, __LINE__, LoadFileName, DefIconPrefsName));

	ReadFileTypes(inst, LoadFileName, DefIconPrefsName);

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));
	d1(kprintf(__FILE__ "/%s/%ld: ReadFileTypes took %lu s\n", __FUNC__, __LINE__, time(NULL) - now));

	return RETURN_OK;
}


static LONG WritePrefs(struct FileTypesPrefsInst *inst,
	CONST_STRPTR SaveName, CONST_STRPTR DefIconPrefsName, ULONG Flags)
{
	LONG Result;

	d1(time_t now = time(NULL));

	WriteDefIconsPrefs(inst, DefIconPrefsName);

	Result = WriteFileTypes(inst, SaveName, Flags);

	d1(kprintf(__FILE__ "/%s/%ld: WriteFileTypes took %lu s\n", __FUNC__, __LINE__, time(NULL) - now));

	if (RETURN_OK == Result)
		{
		}
	else
		{
		char Buffer[120];

		Fault(Result, "", Buffer, sizeof(Buffer) - 1);

		// MUI_RequestA()
		MUI_Request(inst->fpb_Objects[OBJNDX_APP_Main], inst->fpb_Objects[OBJNDX_WIN_Main], 0, NULL, 
			GetLocString(MSGID_ABOUTREQOK), 
			GetLocString(MSGID_REQTITLE_SAVEERROR), 
			SaveName,
			Buffer);
		}

	return Result;
}

//----------------------------------------------------------------------------

DISPATCHER(myNListTree)
{
	struct FileTypesPrefsInst *inst;
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->fpb_Hooks[MenuHookIndex];

		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, 0);

		Result = 0;
		}
		break;

	case MUIM_NList_TestPos:
		{
		struct TestPosArg
			{
			ULONG MethodID;
			LONG x, y;
			struct MUI_NList_TestPos_Result *res;
			};
		struct TestPosArg *tpa = (struct TestPosArg *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);

		if (inst->fpb_PageIsActive)
			Result = DoSuperMethodA(cl, obj, msg);
		else
			{
			Result = 0;
			tpa->res->entry = tpa->res->column = -1;
			}

		d1(kprintf("%s/%ld: MUIM_NList_TestPos  Result=%08lx  x=%ld  y=%ld  tpr=%08lx  entry=%ld  column=%ld\n", \
			__FUNC__, __LINE__, Result, tpa->x, tpa->y, tpa->res, tpa->res->entry, tpa->res->column));
		}
		break;

	case MUIM_DragQuery:
		{
		struct MUIP_DragQuery *dq = (struct MUIP_DragQuery *) msg;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf("%s/%ld: MUIM_DragQuery obj=%08lx\n", __FUNC__, __LINE__, dq->obj));

		if (inst->fpb_Objects[OBJNDX_MainListTree] == dq->obj)
			Result = MUIV_DragQuery_Accept;
		else
			Result = MUIV_DragQuery_Refuse;
		}
		break;

	case MUIM_DragDrop:
		Result = DoDragDrop(cl, obj, msg);
		break;

	case MUIM_NList_DropType:
		{
		struct MUIP_NList_DropType *dr = (struct MUIP_NList_DropType *)msg;
		struct MUI_NListtree_TreeNode *tnTo, *tnFrom;
		struct MUI_NListtree_TestPos_Result res;

		get(obj, MUIA_NList_PrivateData, &inst);

		inst->fpb_MenuTreeMayDrop = FALSE;

		Result = DoSuperMethodA(cl, obj, msg);
		d1(KPrintF("%s/%ld: DropType=%ld  pos=%ld\n", __FUNC__, __LINE__, *(dr->type), *(dr->pos)));

		DoMethod(obj, MUIM_NListtree_TestPos, dr->mousex, dr->mousey, &res);

		tnTo = res.tpr_TreeNode;

		tnFrom = MUIV_NListtree_Active_Off;
		get(obj, MUIA_NListtree_Active, &tnFrom);

		d1(KPrintF("%s/%ld: tnFrom=%08lx  tnTo=%08lx\n", __FUNC__, __LINE__, tnFrom, tnTo));

		if (tnTo && tnFrom)
			{
			switch (*(dr->type))
				{
			case MUIV_NListtree_DropType_None:
			case MUIV_NListtree_DropType_Sorted:
			default:
				break;
			case MUIV_NListtree_DropType_Above:
			case MUIV_NListtree_DropType_Below:
				if (MayPasteBelow(inst, tnTo, tnFrom))
					inst->fpb_MenuTreeMayDrop = TRUE;
				break;
			case MUIV_NListtree_DropType_Onto:
				if (MayPasteOnto(inst, tnTo, tnFrom))
					inst->fpb_MenuTreeMayDrop = TRUE;
				break;
				}
			d1(KPrintF("%s/%ld: DropType=%ld\n", __FUNC__, __LINE__, *(dr->type)));
			}
		}
		break;

	// we catch MUIM_DragReport because we want to restrict some dragging for some special objects
	case MUIM_DragReport:
		{
		get(obj, MUIA_NList_PrivateData, &inst);

		d1(KPrintF("%s/%ld: MUIM_DragReport\n", __FUNC__, __LINE__));

		Result = DoSuperMethodA(cl, obj, msg);
		if (!inst->fpb_MenuTreeMayDrop)
			Result = MUIV_DragReport_Abort;

		d1(KPrintF("%s/%ld: MUIM_DragReport\n", __FUNC__, __LINE__));
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myFileTypesNListTree)
{
	struct FileTypesPrefsInst *inst;
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->fpb_Hooks[MenuHookIndex];

		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, 0);

		Result = 0;
		}
		break;

	case MUIM_DragQuery:
		{
		struct MUIP_DragQuery *dq = (struct MUIP_DragQuery *) msg;

		d1(kprintf("%s/%ld: MUIM_DragQuery obj=%08lx\n", __FUNC__, __LINE__, dq->obj));

		// only accept objects from ourselves
		if (obj == dq->obj)
			Result = MUIV_DragQuery_Accept;
		else
			Result = MUIV_DragQuery_Refuse;
		}
		break;

	case MUIM_DragDrop:
		{
		//struct MUIP_DragDrop *dq = (struct MUIP_DragDrop *) msg;
		struct MUI_NListtree_TreeNode *ln;

		get(obj, MUIA_NList_PrivateData, &inst);
		d1(kprintf("%s/%ld: MUIP_DragDrop obj=%08lx\n", __FUNC__, __LINE__, obj));

		Result = DoSuperMethodA(cl, obj, msg);

		ln = (struct MUI_NListtree_TreeNode *) DoMethod(obj,
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_GetEntry_ListNode_Active,
			MUIV_NListtree_GetEntry_Position_Active,
			0);

		d1(kprintf("%s/%ld: ln=%08lx  tn_User=%08lx\n", __FUNC__, __LINE__, ln, ln->tn_User));

		if (ln)
			{
			SetFileTypesIcon(inst, ln);
			}
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myNList)
{
	struct FileTypesPrefsInst *inst;
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->fpb_Hooks[MenuHookIndex];

		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

DISPATCHER(myFileTypesActionsNList)
{
	struct FileTypesPrefsInst *inst;
	ULONG Result;

	d1(kprintf(__FILE__ "/%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch(msg->MethodID)
		{
	case MUIM_DragDrop:
		Result = DoSuperMethodA(cl, obj, msg);
		get(obj, MUIA_NList_PrivateData, &inst);
		CallHookPkt(&inst->fpb_Hooks[HOOKNDX_DragDropSort_FileTypesAction], obj, msg);
		break;

	case MUIM_ContextMenuChoice:
		{
		struct MUIP_ContextMenuChoice *cmc = (struct MUIP_ContextMenuChoice *) msg;
		Object *MenuObj;
		ULONG MenuHookIndex = 0;
		struct Hook *MenuHook = NULL;

		get(obj, MUIA_NList_PrivateData, &inst);

		d1(kprintf("%s/%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%ld: MenuObj=%08lx\n", __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%ld: msg=%08lx *msg=%08lx\n", __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->fpb_Hooks[MenuHookIndex];

		d1(kprintf("%s/%ld: MenuHook=%08lx\n", __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->fpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, MenuObj);

		Result = 0;
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

struct FtAttribute *AddAttribute(struct FileTypesListEntry *fte,
	enum ftAttributeType AttrType, ULONG DataLength, const void *AttrData)
{
	struct FtAttribute *fta;

	fta = malloc(sizeof(struct FtAttribute) + DataLength);
	d1(kprintf(__FILE__ "/%s/%ld: fta=%08lx  size=%lu\n", __FUNC__, __LINE__, \
		fta, sizeof(struct FtAttribute) + DataLength));

	if (fta)
		{
		AddTail(&fte->ftle_AttributesList, &fta->fta_Node);

		fta->fta_Type = AttrType;
		fta->fta_Length = DataLength;
		memcpy(fta->fta_Data, AttrData, DataLength);
		}

	return fta;
}

static void DisposeAttribute(struct FtAttribute *fta)
{
	free(fta);
}

static void CleanupAttributes(struct List *AttributesList)
{
	struct FtAttribute *fta;

	while ((fta = (struct FtAttribute *) RemHead(AttributesList)))
		{
		DisposeAttribute(fta);
		}
}

//----------------------------------------------------------------------------

CONST_STRPTR GetAttributeName(enum ftAttributeType Type)
{
	CONST_STRPTR Name;

	switch (Type)
		{
	case ATTRTYPE_FtDescription:
		Name = "DESCRIPTION";
		break;

	case ATTRTYPE_PvPluginName:
		Name = "PREVIEWPLUGIN";
		break;

	case ATTRTYPE_GroupOrientation:
		Name = "ORIENTATION";
		break;

	case ATTRTYPE_MemberHideString:
		Name = "HIDE";
		break;

	case ATTRTYPE_StringHAlign:
		Name = "HALIGN";
		break;

	case ATTRTYPE_StringVAlign:
		Name = "VALIGN";
		break;

	case ATTRTYPE_StringStyle:
		Name = "STYLE";
		break;

	case ATTRTYPE_StringFont:
		Name = "FONT";
		break;

	case ATTRTYPE_StringTTFont:
		Name = "TTFONT";
		break;

	case ATTRTYPE_StringPen:
		Name = "TEXTPEN";
		break;

	case ATTRTYPE_StringId:
		Name = "ID";
		break;

	case ATTRTYPE_StringText:
		Name = "TEXT";
		break;

	case ATTRTYPE_StringSrc:
		Name = "SRC";
		break;

	case ATTRTYPE_SpaceSize:
		Name = "SIZE";
		break;

	case ATTRTYPE_DtImageName:
		Name = "FILENAME";
		break;

	case ATTRTYPE_UnselIconName:
		Name = "UNSELECTEDICON";
		break;

	case ATTRTYPE_SelIconName:
		Name = "SELECTEDICON";
		break;

	case ATTRTYPE_MenuItemName:
		Name = "NAME";
		break;

	case ATTRTYPE_MenuCommKey:
		Name = "KEY";
		break;

	case ATTRTYPE_MenuDefaultAction:
		Name = "DEFAULTACTION";
		break;

	case ATTRTYPE_CommandName:
		Name = "NAME";
		break;

	case ATTRTYPE_CommandStacksize:
		Name = "STACK";
		break;

	case ATTRTYPE_CommandPriority:
		Name = "PRIORITY";
		break;

	case ATTRTYPE_CommandWbArgs:
		Name = "WBARGS";
		break;

	default:
		Name = "??unknown_Name??";
		break;
		}

	return Name;
}

STRPTR GetAttributeValueString(const struct FtAttribute *fta, char *Buffer, size_t BuffLen)
{
	if (NULL == fta)
		strcpy(Buffer, "");
	else
		{
		switch (fta->fta_Type)
			{
		case ATTRTYPE_FtDescription:
		case ATTRTYPE_PvPluginName:
		case ATTRTYPE_StringHAlign:
		case ATTRTYPE_StringVAlign:
		case ATTRTYPE_StringStyle:
		case ATTRTYPE_StringFont:
		case ATTRTYPE_StringTTFont:
		case ATTRTYPE_StringPen:
		case ATTRTYPE_StringId:
		case ATTRTYPE_StringText:
		case ATTRTYPE_StringSrc:
		case ATTRTYPE_DtImageName:
		case ATTRTYPE_UnselIconName:
		case ATTRTYPE_SelIconName:
		case ATTRTYPE_MenuItemName:
		case ATTRTYPE_MenuCommKey:
		case ATTRTYPE_CommandName:
		case ATTRTYPE_MemberHideString:
			stccpy(Buffer, (char *)fta->fta_Data, BuffLen);
			break;

		case ATTRTYPE_GroupOrientation:
			switch (*((enum TTLayoutMode *) fta->fta_Data))
				{
			case TTL_Horizontal:
				stccpy(Buffer, "horizontal", BuffLen);
				break;
			case TTL_Vertical:
				stccpy(Buffer, "vertical", BuffLen);
				break;
			default:
				sprintf(Buffer, "??unknown_Orientation??:%d", *((enum TTLayoutMode *) fta->fta_Data));
				break;
				}
			break;

		case ATTRTYPE_CommandStacksize:
		case ATTRTYPE_CommandPriority:
		case ATTRTYPE_SpaceSize:
			sprintf(Buffer, "%ld", *((ULONG *) fta->fta_Data));
			break;

		case ATTRTYPE_CommandWbArgs:
		case ATTRTYPE_MenuDefaultAction:
			strcpy(Buffer, "");
			break;

		default:
			stccpy(Buffer, "??unknown_AttrType??", BuffLen);
			break;
			}
		}

	return Buffer;
}


static const struct FtAttribute *FindAttribute(const struct FileTypesListEntry *fte, enum FtEntryType AttrType)
{
	struct FtAttribute *fta;

	for (fta = (struct FtAttribute *) fte->ftle_AttributesList.lh_Head;
		fta != (struct FtAttribute *) &fte->ftle_AttributesList.lh_Tail;
		fta = (struct FtAttribute *) fta->fta_Node.ln_Succ)
		{
		if (AttrType == fta->fta_Type)
			return fta;
		}

	return NULL;
}


static void RemoveAttribute(struct FileTypesListEntry *fte, enum ftAttributeType AttrType)
{
	struct FtAttribute *fta;

	for (fta = (struct FtAttribute *) fte->ftle_AttributesList.lh_Head;
		fta != (struct FtAttribute *) &fte->ftle_AttributesList.lh_Tail;
		fta = (struct FtAttribute *) fta->fta_Node.ln_Succ)
		{
		if (AttrType == fta->fta_Type)
			{
			Remove(&fta->fta_Node);
			free(fta);
			break;
			}
		}

}


static struct FileTypesListEntry *FindChildByType(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tn, enum FtEntryType RequestedType)
{
	struct FileTypesListEntry *fte = NULL;

	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);

		fte = tn ? ((struct FileTypesListEntry *) tn->tn_User) : NULL;

		while (fte && fte->ftle_EntryType != RequestedType)
			{			
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry, 
				tn,
				MUIV_NListtree_GetEntry_Position_Next,
				0);

			fte = tn ? ((struct FileTypesListEntry *) tn->tn_User) : NULL;
			}
		}

	return fte;
}


static BOOL HasChildren(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn)
{
	BOOL HasChildren = FALSE;

	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);

		if (tn)
			HasChildren = TRUE;
		}

	return HasChildren;
}

//----------------------------------------------------------------------------

CONST_STRPTR GetNameFromEntryType(enum ftAttributeType EntryType)
{
	if (EntryType >= 0 && EntryType < Sizeof(EntryTypeNames))
		return GetLocString(EntryTypeNames[EntryType]);
	else
		return "??unknown_EntryType??";
}

//----------------------------------------------------------------------------

static void AddDefaultAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte)
{
	ULONG n;

	for (n = 0; n < Sizeof(AllEntryAttributes); n++)
		{
		if (AllEntryAttributes[n].eat_EntryType == fte->ftle_EntryType)
			{
			ULONG m;

			d1(kprintf("%s/%ld:  EntryType=%lu\n", __FUNC__, __LINE__, AllEntryAttributes[n].eat_EntryType));

			for (m = 0; m < AllEntryAttributes[n].eat_AttrCount; m++)
				{
				if (IsAttrRequired(fte, &AllEntryAttributes[n].eat_AttrArray[m]))
					{
					d1(kprintf("%s/%ld:  Add AttrType=%lu\n", __FUNC__, __LINE__, \
						AllEntryAttributes[n].eat_AttrArray[m].atd_Type));

					AddNewAttribute(inst, fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type);
					}
				}
			break;
			}
		}
}

//----------------------------------------------------------------------------

static void SetAddableAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte)
{
	ULONG Added = 0;
	ULONG n;

	set(inst->fpb_Objects[OBJNDX_AddAttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(inst->fpb_Objects[OBJNDX_AddAttrList], MUIM_NList_Clear);

	for (n = 0; n < Sizeof(AllEntryAttributes); n++)
		{
		if (AllEntryAttributes[n].eat_EntryType == fte->ftle_EntryType)
			{
			ULONG m;

			for (m = 0; m < AllEntryAttributes[n].eat_AttrCount; m++)
				{
				if (NULL == FindAttribute(fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type))
					{
					Added++;
					DoMethod(inst->fpb_Objects[OBJNDX_AddAttrList], MUIM_NList_InsertSingle,
						AllEntryAttributes[n].eat_AttrArray[m].atd_Type, 
						MUIV_NList_Insert_Sorted);
					}
				}
			break;
			}
		}

	d1(kprintf("%s/%ld:  Added=%lu\n", __FUNC__, __LINE__, Added));

	set(inst->fpb_Objects[OBJNDX_AddAttrPopObject], MUIA_Disabled, 0 == Added);

	set(inst->fpb_Objects[OBJNDX_AddAttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
}

//----------------------------------------------------------------------------

static void UpdateAttributes(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte)
{
	struct FtAttribute *fta;
	struct FileTypesListEntry *ftePopupMenu;
	struct FileTypesListEntry *fteToolTip;
	const ULONG *AddEntryList;
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	DoMethod(inst->fpb_Objects[OBJNDX_AddEntryPopObject], MUIM_Popstring_Close, FALSE);
	DoMethod(inst->fpb_Objects[OBJNDX_AddAttrPopObject], MUIM_Popstring_Close, FALSE);

	set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);
	set(inst->fpb_Objects[OBJNDX_AddEntryList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_NList_Clear);
	DoMethod(inst->fpb_Objects[OBJNDX_AddEntryList], MUIM_NList_Clear);

	set(inst->fpb_Objects[OBJNDX_ButtonRemoveAttribute], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_ButtonEditAttribute], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Menu_CopyAttr], MUIA_Menuitem_Enabled, FALSE);
	set(inst->fpb_Objects[OBJNDX_Menu_CutAttr], MUIA_Menuitem_Enabled, FALSE);

	for (fta = (struct FtAttribute *) fte->ftle_AttributesList.lh_Head;
		fta != (struct FtAttribute *) &fte->ftle_AttributesList.lh_Tail;
		fta = (struct FtAttribute *) fta->fta_Node.ln_Succ)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_NList_InsertSingle,
			fta, MUIV_NList_Insert_Bottom);
		}

	switch (fte->ftle_EntryType)
		{
	case ENTRYTYPE_FileType:
		ftePopupMenu = FindChildByType(inst, tn, ENTRYTYPE_PopupMenu);
		fteToolTip = FindChildByType(inst, tn, ENTRYTYPE_ToolTip);

		if (ftePopupMenu)
			{
			if (fteToolTip)
				AddEntryList = NULL;
			else
				AddEntryList = AddFiletypeStringsTT;
			}
		else
			{
			if (fteToolTip)
				AddEntryList = AddFiletypeStringsPM;
			else
				AddEntryList = AddFiletypeStringsPMTT;
			}
		break;

	case ENTRYTYPE_PopupMenu:
		AddEntryList = AddPopupMenuStrings;
		break;

	case ENTRYTYPE_PopupMenu_SubMenu:
		AddEntryList = AddSubMenuStrings;
		break;

	case ENTRYTYPE_PopupMenu_MenuItem:
		AddEntryList = AddMenuItemStrings;
		break;

	case ENTRYTYPE_ToolTip:
		AddEntryList = AddToolTipStrings;
		break;

	case ENTRYTYPE_ToolTip_Group:
		AddEntryList = AddGroupStrings;
		break;

	case ENTRYTYPE_ToolTip_Member:
		// only 1 child allowed
		if (HasChildren(inst, tn))
			AddEntryList = NULL;
		else
			AddEntryList = AddMemberStrings;
		break;

	default:
		AddEntryList = NULL;
		break;
		}

	SetAddableAttributes(inst, fte);

	if (AddEntryList)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_AddEntryList], MUIM_NList_Insert,
			AddEntryList, -1, MUIV_NList_Insert_Sorted);
		}

	set(inst->fpb_Objects[OBJNDX_AddEntryList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
	set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	set(inst->fpb_Objects[OBJNDX_AddEntryPopObject], MUIA_Disabled, NULL == AddEntryList);
}

//----------------------------------------------------------------------------

static void AddNewAttribute(struct FileTypesPrefsInst *inst, struct FileTypesListEntry *fte, enum ftAttributeType AttrType)
{
	ULONG n;

	d1(kprintf("%s/%ld:  inst=%08lx  EntryType=%ld  AttrType=%ld\n", __FUNC__, __LINE__, inst, fte->ftle_EntryType, AttrType));

	set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	for (n = 0; n < Sizeof(AllEntryAttributes); n++)
		{
		if (AllEntryAttributes[n].eat_EntryType == fte->ftle_EntryType)
			{
			ULONG m;

			d1(kprintf("%s/%ld:  AttrCount=%lu\n", __FUNC__, __LINE__, AllEntryAttributes[n].eat_AttrCount));

			for (m = 0; m < AllEntryAttributes[n].eat_AttrCount; m++)
				{
				const struct AttrDefaultValue *avd = &AllEntryAttributes[n].eat_AttrArray[m].atd_DefaultContents;

				d1(kprintf("%s/%ld:  atd_Type=%lu\n", __FUNC__, __LINE__, AllEntryAttributes[n].eat_AttrArray[m].atd_Type));

				if (AllEntryAttributes[n].eat_AttrArray[m].atd_Type == AttrType)
					{
					struct FtAttribute *ftaNew = NULL;

					switch (avd->avd_Type)
						{
					case ATTRDEFTYPE_None:
						ftaNew = AddAttribute(fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type,
							0, NULL);
						break;
					case ATTRDEFTYPE_ULong:
						ftaNew = AddAttribute(fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type,
							sizeof(ULONG), &avd->avd_ULongValue);
						break;
					case ATTRDEFTYPE_LocString:
						ftaNew = AddAttribute(fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type,
							1 + strlen(GetLocString(avd->avd_ULongValue)), GetLocString(avd->avd_ULongValue));
						break;
					case ATTRDEFTYPE_String:
					case ATTRDEFTYPE_String2:
					case ATTRDEFTYPE_PathName:
					case ATTRDEFTYPE_FileName:
					case ATTRDEFTYPE_FontString:
					case ATTRDEFTYPE_TTFontString:
						ftaNew = AddAttribute(fte, AllEntryAttributes[n].eat_AttrArray[m].atd_Type,
							1 + strlen(avd->avd_StringValue), avd->avd_StringValue);
						break;
						}

					if (ftaNew)
						{
						DoMethod(inst->fpb_Objects[OBJNDX_AttrList], MUIM_NList_InsertSingle,
							ftaNew, MUIV_NList_Insert_Bottom);
						}
					break;
					}
				}
			}
		}

	set(inst->fpb_Objects[OBJNDX_AttrList], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	EnablePasteAttrMenuEntry(inst);
}

//----------------------------------------------------------------------------

static void BeginSetMenuDefaultAction(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tnMenuItem)
{
	struct MUI_NListtree_TreeNode *tn = tnMenuItem;
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

	// Find root of PopupMenu
	while (tn && ENTRYTYPE_PopupMenu != fte->ftle_EntryType)
		{
		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);

		if (tn)
			fte = (struct FileTypesListEntry *) tn->tn_User;
		}

	d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	// Now walk through entire Menu and remove "DefaultAction" atrribute from every MenuItem
	// except <tnMenuItem>
	SetMenuDefaultAction(inst, tn, tnMenuItem);
}


static void SetMenuDefaultAction(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnMenu, struct MUI_NListtree_TreeNode *tnNewDefault)
{
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		tnMenu,
		MUIV_NListtree_GetEntry_Position_Head,
		0);

	d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	while (tn)
		{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

		d1(kprintf("%s/%ld: tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

		switch (fte->ftle_EntryType)
			{
		case ENTRYTYPE_PopupMenu_MenuItem:
			if (tn != tnNewDefault)
				{
				const struct FtAttribute *fta = FindAttribute(fte, ATTRTYPE_MenuDefaultAction);

				d1(kprintf("%s/%ld: fta=%08lx\n", __FUNC__, __LINE__, fta));
				if (fta)
					{
					RemoveAttribute(fte, ATTRTYPE_MenuDefaultAction);

					DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
						MUIM_NListtree_Redraw, 
						tn,
						0);
					}
				}
			break;

		case ENTRYTYPE_PopupMenu_SubMenu:
			SetMenuDefaultAction(inst, tn, tnNewDefault);
			break;

		default:
			break;
			}

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Next,
			0);
		}
}

//----------------------------------------------------------------------------

static const struct AttributeDef *GetAttributeDef(enum ftAttributeType AttrType, enum FtEntryType fteEntryType)
{
	const struct EntryAttributes *eat;
	ULONG n;

	for (n = 0, eat = AllEntryAttributes; n < Sizeof(AllEntryAttributes); n++, eat++)
		{
		if (fteEntryType == eat->eat_EntryType)
			{
			const struct AttributeDef *atd = eat->eat_AttrArray;
			ULONG m;

			for (m = 0; m < eat->eat_AttrCount; m++, atd++)
				{
				if (atd->atd_Type == AttrType)
					return atd;
				}
			break;
			}
		}

	return NULL;
}

//----------------------------------------------------------------------------

static void FillListOfAttributeValues(struct FileTypesPrefsInst *inst, 
	const struct FileTypesListEntry *fte, const struct AttributeDef *atd, 
	const struct AttrListEntry *Attr)
{
	ULONG n;

	set(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);
	DoMethod(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue], MUIM_NList_Clear);

	d1(KPrintF("%s/%ld:  atd_NumberOfValues=%ld\n", __FUNC__, __LINE__, atd->atd_NumberOfValues));

	if (ATTRDEFTYPE_String2 == atd->atd_PossibleContents[0].avd_Type)
		{
		set(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue],
			MUIA_NList_Format, ",");
		}
	else
		{
		set(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue],
			MUIA_NList_Format, "");
		}

	for (n = 0; n < atd->atd_NumberOfValues; n++)
		{
		size_t len;
		struct EditAttrListEntry *eal;

		len = sizeof(struct EditAttrListEntry);

		switch (atd->atd_PossibleContents[n].avd_Type)
			{
		case ATTRDEFTYPE_ULong:
			len += sizeof(atd->atd_PossibleContents[n].avd_ULongValue);
			break;
		case ATTRDEFTYPE_LocString:
		case ATTRDEFTYPE_String:
		case ATTRDEFTYPE_String2:
		case ATTRDEFTYPE_PathName:
		case ATTRDEFTYPE_FileName:
		case ATTRDEFTYPE_FontString:
		case ATTRDEFTYPE_TTFontString:
			len += 1 + strlen(atd->atd_PossibleContents[n].avd_StringValue);
			break;
		default:
			break;
			}

		eal = malloc(len);

		if (eal)
			{
			d1(KPrintF("%s/%ld:  avd_Type=%ld\n", __FUNC__, __LINE__, atd->atd_PossibleContents[n].avd_Type));

			eal->eal_fta = Attr->ale_Attribute;
			eal->eal_fta.fta_Value2 = 0;

			switch (atd->atd_PossibleContents[n].avd_Type)
				{
			case ATTRDEFTYPE_ULong:
				memcpy(eal->eal_fta.fta_Data, &atd->atd_PossibleContents[n].avd_ULongValue, sizeof(ULONG));
				eal->eal_fta.fta_Length = sizeof(ULONG);
				break;
			case ATTRDEFTYPE_LocString:
			case ATTRDEFTYPE_String:
			case ATTRDEFTYPE_PathName:
			case ATTRDEFTYPE_FileName:
			case ATTRDEFTYPE_FontString:
			case ATTRDEFTYPE_TTFontString:
				strcpy((char *)eal->eal_fta.fta_Data, atd->atd_PossibleContents[n].avd_StringValue);
				eal->eal_fta.fta_Length = strlen(atd->atd_PossibleContents[n].avd_StringValue);
				break;
			case ATTRDEFTYPE_String2:
				eal->eal_fta.fta_Value2 = atd->atd_PossibleContents[n].avd_ULongValue;
				strcpy((char *)eal->eal_fta.fta_Data, atd->atd_PossibleContents[n].avd_StringValue);
				eal->eal_fta.fta_Length = strlen(atd->atd_PossibleContents[n].avd_StringValue);
				break;
			default:
				break;
				}

			DoMethod(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue], 
				MUIM_NList_InsertSingle,
				eal,
				MUIV_NList_Insert_Bottom);

			free(eal);
			}
		}

	set(inst->fpb_Objects[OBJNDX_List_AttributeSelectValue], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
}

//----------------------------------------------------------------------------

static ULONG ConvertGroupOrientationFromString(CONST_STRPTR DisplayString)
{
	ULONG Result = TTL_Horizontal;

	if (0 == Stricmp(DisplayString, "horizontal"))
		Result = TTL_Horizontal;
	else if (0 == Stricmp(DisplayString, "vertical"))
		Result = TTL_Vertical;

	return Result;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *CopyFileTypesEntry(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnToListNode, struct MUI_NListtree_TreeNode *tnToPrevNode, 
	struct MUI_NListtree_TreeNode *tnFrom, ULONG destList, ULONG srcList)
{
	struct FileTypesListEntry *fteFrom = (struct FileTypesListEntry *) tnFrom->tn_User;
	struct FileTypesListEntry *fteTo;
	struct MUI_NListtree_TreeNode *tnNew;
	struct MUI_NListtree_TreeNode *tnChild;
	const struct FtAttribute *fta;

	if (NULL == tnFrom)
		return NULL;

	tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[srcList],
		MUIM_NListtree_GetEntry, 
		tnFrom,
		MUIV_NListtree_GetEntry_Position_Head,
		0);

	tnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[destList],
		MUIM_NListtree_Insert, 
		tnFrom->tn_Name, NULL,
		tnToListNode,
		tnToPrevNode,
		TNF_LIST | TNF_OPEN);

	fteTo = (struct FileTypesListEntry *) tnNew->tn_User;
	fteTo->ftle_EntryType = fteFrom->ftle_EntryType;

	fteTo->fte_ImageObject = NULL;	// do not try to copy fte_ImageObject !!
	fteTo->fte_ImageIndex = 0;

	// Copy Attributes
	for (fta = (const struct FtAttribute *) fteFrom->ftle_AttributesList.lh_Head;
		fta != (const struct FtAttribute *) &fteFrom->ftle_AttributesList.lh_Tail;
		fta = (const struct FtAttribute *) fta->fta_Node.ln_Succ)
		{
		AddAttribute(fteTo, fta->fta_Type, fta->fta_Length, fta->fta_Data);
		}

	UpdateMenuImage(inst, destList, fteTo);

	// Copy Children
	while (tnChild)
		{
		d1(kprintf("%s/%ld: tnChild=%08lx <%s>\n", __FUNC__, __LINE__, tnChild, tnChild->tn_Name));

		CopyFileTypesEntry(inst, 
			tnNew, (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
			tnChild, destList, srcList);

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[srcList],
			MUIM_NListtree_GetEntry, 
			tnChild,
			MUIV_NListtree_GetEntry_Position_Next,
			0);
		}

	return tnNew;
}

//----------------------------------------------------------------------------

static BOOL MayPasteOnto(struct FileTypesPrefsInst *inst, 
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom)
{
	const struct FileTypesListEntry *fteFrom = (const struct FileTypesListEntry *) tnFrom->tn_User;
	const struct FileTypesListEntry *fteTo = (const struct FileTypesListEntry *) tnTo->tn_User;
	BOOL MayPaste = FALSE;

	d1(kprintf("%s/%ld: fteTo=%08lx <%s> %ld  fteFrom=%08lx <%s> %ld\n", __FUNC__, __LINE__, \
		fteTo, tnTo->tn_Name, fteTo->ftle_EntryType, fteFrom, tnFrom->tn_Name, fteFrom->ftle_EntryType));

	if (fteTo->ftle_EntryType >= ENTRYTYPE_MAX || fteFrom->ftle_EntryType >= ENTRYTYPE_MAX)
		return FALSE;

	d1(kprintf("%s/%ld: MayPaste=%ld\n", __FUNC__, __LINE__, \
		MayPasteIntoMatrix[fteTo->ftle_EntryType][fteFrom->ftle_EntryType]));

	if (MayPasteIntoMatrix[fteTo->ftle_EntryType][fteFrom->ftle_EntryType])
		{
		switch (fteTo->ftle_EntryType)
			{
		case ENTRYTYPE_FileType:
			// There is only one Popupmenu and one Tooltip allowed!
			switch (fteFrom->ftle_EntryType)
				{
			case ENTRYTYPE_PopupMenu:
				if (NULL == FindChildByType(inst, tnTo, ENTRYTYPE_PopupMenu))
					MayPaste = TRUE;
				break;
			case ENTRYTYPE_ToolTip:
				if (NULL == FindChildByType(inst, tnTo, ENTRYTYPE_ToolTip))
					MayPaste = TRUE;
				break;
			default:				
				break;
				}
			break;

		case ENTRYTYPE_ToolTip_Member:
			// only 1 child allowed
			if (!HasChildren(inst, tnTo))
				MayPaste = TRUE;
			break;

		default:
			MayPaste = TRUE;
			break;
			}
		}

	return MayPaste;
}


static struct MUI_NListtree_TreeNode *MayPasteBelow(struct FileTypesPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom)
{
	const struct FileTypesListEntry *fteFrom = (const struct FileTypesListEntry *) tnFrom->tn_User;
	const struct FileTypesListEntry *fteTo = (const struct FileTypesListEntry *) tnTo->tn_User;
	struct MUI_NListtree_TreeNode *tn;
	BOOL Result = FALSE;

	d1(KPrintF("%s/%ld: START fteTo=%08lx <%s> %ld  fteFrom=%08lx <%s> %ld\n", __FUNC__, __LINE__, \
		fteTo, tnTo->tn_Name, fteTo->ftle_EntryType, fteFrom, tnFrom->tn_Name, fteFrom->ftle_EntryType));

	do	{
		if (fteTo->ftle_EntryType >= ENTRYTYPE_MAX || fteFrom->ftle_EntryType >= ENTRYTYPE_MAX)
			break;

		if (tnTo->tn_Flags & TNF_OPEN)
			{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Head,
				MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

			if (tn)
				break;
			}

		do	{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Next,
				MUIV_NListtree_GetEntry_Flag_SameLevel | MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));
			if (tn)
				break;

			tnTo = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Parent,
				MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tnTo=%08lx\n", __FUNC__, __LINE__, tnTo));

			if (tnTo)
				{
				fteTo = (const struct FileTypesListEntry *) tnTo->tn_User;

				Result =  MayPasteAfterMatrix[fteTo->ftle_EntryType][fteFrom->ftle_EntryType];
				}
			} while (tnTo && !Result);

		if (NULL == tnTo)
			break;

		Result =  MayPasteAfterMatrix[fteTo->ftle_EntryType][fteFrom->ftle_EntryType];
		} while (0);

	d1(KPrintF("%s/%ld: END MayPaste=%ld\n", __FUNC__, __LINE__, Result));

	return Result ? tnTo : NULL;
}

//----------------------------------------------------------------------------

static void EnablePasteMenuEntry(struct FileTypesPrefsInst *inst)
{
	struct MUI_NListtree_TreeNode *tnMain;
	struct MUI_NListtree_TreeNode *tnShadow;

	tnMain = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);

	tnShadow = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_ShadowListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_Insert_ListNode_Root,
		MUIV_NListtree_GetEntry_Position_Head,
		0);

	if (tnMain && tnShadow &&
		( MayPasteBelow(inst, tnMain, tnShadow) || MayPasteOnto(inst, tnMain, tnShadow) ))
		{
		set(inst->fpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, TRUE);
		}
	else
		{
		set(inst->fpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, FALSE);
		}
}

//----------------------------------------------------------------------------

static BOOL MayPasteAttr(struct FileTypesPrefsInst *inst, const struct FileTypesListEntry *fte, const struct AttrListEntry *Attr)
{
	d1(kprintf("%s/%ld: \n", __FUNC__, __LINE__));

	if (NULL == Attr)
		return FALSE;

	if (NULL == FindAttribute(fte, Attr->ale_Attribute.fta_Type))
		{
		ULONG n;

		d1(kprintf("%s/%ld: Attr=%08lx  Type=%ld\n", __FUNC__, __LINE__, Attr, Attr->ale_Attribute.fta_Type));

		for (n = 0; n < Sizeof(AllEntryAttributes); n++)
			{
			if (AllEntryAttributes[n].eat_EntryType == fte->ftle_EntryType)
				{
				ULONG m;

				d1(kprintf("%s/%ld: AttrCount=%ld\n", __FUNC__, __LINE__, AllEntryAttributes[n].eat_AttrCount));

				for (m = 0; m < AllEntryAttributes[n].eat_AttrCount; m++)
					{
					if (Attr->ale_Attribute.fta_Type == AllEntryAttributes[n].eat_AttrArray[m].atd_Type)
						{
						d1(kprintf("%s/%ld: Found!\n", __FUNC__, __LINE__));
						return TRUE;
						}
					}
				break;
				}
			}
		}

	d1(kprintf("%s/%ld: May not paste!\n", __FUNC__, __LINE__));

	return FALSE;
}

//----------------------------------------------------------------------------

static void EnablePasteAttrMenuEntry(struct FileTypesPrefsInst *inst)
{
	struct MUI_NListtree_TreeNode *tn;
	struct AttrListEntry *Attr = NULL;

	set(inst->fpb_Objects[OBJNDX_Menu_PasteAttr], MUIA_Menuitem_Enabled, FALSE);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_ShadowAttrList],
			MUIM_NList_GetEntry, 
			0,
			&Attr);

		d1(kprintf("%s/%ld: Attr=%08lx\n", __FUNC__, __LINE__, Attr));

		if (Attr)
			{
			struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

			if (MayPasteAttr(inst, fte, Attr))
				set(inst->fpb_Objects[OBJNDX_Menu_PasteAttr], MUIA_Menuitem_Enabled, TRUE);
			}
		}
}

//----------------------------------------------------------------------------

static BOOL IsAttrRequired(struct FileTypesListEntry *fte, const struct AttributeDef *atd)
{
	ULONG n;

	if (!atd->atd_Required)
		return FALSE;

	for (n = 0; n < atd->atd_ExcludeCount; n++)
		{
		if (NULL != FindAttribute(fte, atd->atd_Exclude[n]))
			{
			// if one of the atd_Exclude[] members is already present 
			// in the Attribute list, this Attribute is NOT required
			return FALSE;
			}
		}

	return TRUE;
}

//----------------------------------------------------------------------------

static void EntryHasChanged(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	SetChangedFlag(inst, TRUE);
	fte->ftle_Changed = TRUE;

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Redraw, 
		tn,
		0);

	// set CHANGED flag for all parent entries up to the ENTRYTYPE_FileType
	while (tn && ENTRYTYPE_FileType != fte->ftle_EntryType)
		{
		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);
		if (tn)
			{
			fte = (struct FileTypesListEntry *) tn->tn_User;
			fte->ftle_Changed = TRUE;

			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Redraw, 
				tn,
				0);
			}
		}

	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, MUIV_NList_Quiet_None);
}

//----------------------------------------------------------------------------

static void SetChangedFlag(struct FileTypesPrefsInst *inst, BOOL changed)
{
	if (changed != inst->fpb_Changed)
		{
		set(inst->fpb_Objects[OBJNDX_Lamp_Changed], 
			MUIA_Lamp_Color, changed ? MUIV_Lamp_Color_Ok : MUIV_Lamp_Color_Off);
		inst->fpb_Changed = changed;
		}
}

//----------------------------------------------------------------------------

static void BuildTTDescFromAttrList(char *buffer, size_t length, struct TagItem *AttrList)
{
	ULONG FontStyle;
	ULONG FontWeight;
	ULONG FontSize;
	const STRPTR *FontFamilyTable;

	FontStyle = GetTagData(TT_FontStyle, TT_FontStyle_Regular, AttrList);
	FontWeight = GetTagData(TT_FontWeight, TT_FontWeight_Normal, AttrList);
	FontSize = GetTagData(TT_FontSize, 12, AttrList);
	FontFamilyTable = (const STRPTR *) GetTagData(TT_FamilyTable, (ULONG) NULL, AttrList);

	sprintf(buffer, "%ld/%ld/%ld/%s", FontStyle, FontWeight, FontSize, FontFamilyTable[0]);
}

//-----------------------------------------------------------------

static BOOL ParseTTFontFromDesc(CONST_STRPTR FontDesc, 
	ULONG *FontStyle, ULONG *FontWeight, ULONG *FontSize,
	STRPTR FontName, size_t FontNameSize)
{
	CONST_STRPTR lp;

	strcpy(FontName, "");
	*FontStyle = 0;
	*FontWeight = 0;
	*FontSize = 0;

	// Font Desc format:
	// "style/weight/size/fontname"

	if (3 != sscanf(FontDesc, "%ld/%ld/%ld", FontStyle, FontWeight, FontSize))
		return FALSE;

	lp = strchr(FontDesc, '/');	// Find "/" between style and weight
	if (NULL == lp)
		return FALSE;

	lp = strchr(lp + 1, '/');	// Find "/" between weight and size
	if (NULL == lp)
		return FALSE;

	lp = strchr(lp + 1, '/');	// Find "/" between size and name
	if (NULL == lp)
		return FALSE;

	// copy font name
	stccpy(FontName, 1 + lp, FontNameSize);

	return TRUE;
}

//-----------------------------------------------------------------

void HideEmptyNodes(struct FileTypesPrefsInst *inst)
{
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Root,
		MUIV_NListtree_GetEntry_Position_Head,
		0);
	d1(kprintf("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

	while (tn)
		{
		struct MUI_NListtree_TreeNode *tnNext;
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;

		d1(kprintf("%s/%ld: tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn->tn_Name));

		if (!HasChildren(inst, tn) && IsListEmpty(&fte->ftle_AttributesList))
			{
			struct MUI_NListtree_TreeNode *tnNew;
			struct FileTypesListEntry *fteNew;

			d1(kprintf("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));
			d1(kprintf("%s/%ld:  filetype <%s>  \n", __FUNC__, __LINE__, tn->tn_Name));

			tnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_HiddenListTree], 
				MUIM_NListtree_Insert,
				tn->tn_Name, NULL,
				MUIV_NListtree_Insert_ListNode_Root,
				MUIV_NListtree_Insert_PrevNode_Sorted,
				TNF_LIST);

			d1(kprintf("%s/%ld:  tnNew=%08lx\n", __FUNC__, __LINE__, tnNew));

			fteNew = (struct FileTypesListEntry *) tnNew->tn_User;
			fteNew->ftle_EntryType = ENTRYTYPE_FileType;
			}

		tnNext = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn, 
			MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);

		if (!HasChildren(inst, tn) && IsListEmpty(&fte->ftle_AttributesList))
			{
			DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
				MUIM_NListtree_Remove, 
				MUIV_NListtree_Remove_ListNode_Active,
				tn,
				0);
			}

		tn = tnNext;
		}
}

//-----------------------------------------------------------------

static void ShowHiddenNodes(struct FileTypesPrefsInst *inst)
{
	struct MUI_NListtree_TreeNode *tn;

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_HiddenListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Root,
		MUIV_NListtree_GetEntry_Position_Head,
		0);
	d1(kprintf("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

	while (tn)
		{
		struct MUI_NListtree_TreeNode *tnNew;
		struct FileTypesListEntry *fteNew;

		d1(kprintf("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));
		d1(kprintf("%s/%ld:  filetype <%s>  \n", __FUNC__, __LINE__, tn->tn_Name));

		tnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Insert,
			tn->tn_Name, NULL,
			MUIV_NListtree_Insert_ListNode_Root,
			MUIV_NListtree_Insert_PrevNode_Sorted,
			TNF_LIST);

		d1(kprintf("%s/%ld:  tnNew=%08lx\n", __FUNC__, __LINE__, tnNew));

		fteNew = (struct FileTypesListEntry *) tnNew->tn_User;
		fteNew->ftle_EntryType = ENTRYTYPE_FileType;

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_HiddenListTree],
			MUIM_NListtree_GetEntry, 
			tn, 
			MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);
		}

	DoMethod(inst->fpb_Objects[OBJNDX_HiddenListTree], 
		MUIM_NListtree_Remove, 
		MUIV_NListtree_Remove_ListNode_Root,
		MUIV_NListtree_Remove_TreeNode_All,
		0);

}

//-----------------------------------------------------------------

static void ParseToolTypes(struct FileTypesPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt)
{
	STRPTR tt;

	d1(kprintf("%s/%ld: start  ptt=%08lx  tooltypes=%08lx\n", __FUNC__, __LINE__, ptt, ptt->ToolTypes));
	d1(kprintf("%s/%ld: start  tooltypes=%08lx %08lx %08lx\n", __FUNC__, __LINE__, ptt, ptt->ToolTypes[0], ptt->ToolTypes[1], ptt->ToolTypes[2]));

	tt = FindToolType(ptt->ToolTypes, "HIDEEMPTYENTRIES");
	d1(kprintf("%s/%ld: tt=%08lx\n", __FUNC__, __LINE__, tt));
	if (tt)
		{
		d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

		if (MatchToolValue(tt, "YES"))
			inst->fpb_HideEmptyNodes = TRUE;
		else if (MatchToolValue(tt, "NO"))
			inst->fpb_HideEmptyNodes = FALSE;

		d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));
		}

	if (inst->fpb_Objects[OBJNDX_Menu_HideEmptyEntries])
		set(inst->fpb_Objects[OBJNDX_Menu_HideEmptyEntries], MUIA_Menuitem_Checked, inst->fpb_HideEmptyNodes);

	d1(kprintf("%s/%ld: end\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static LONG ReadScalosPrefs(struct FileTypesPrefsInst *inst, CONST_STRPTR PrefsFileName)
{
	LONG lID;
	APTR p_MyPrefsHandle;

	p_MyPrefsHandle = AllocPrefsHandle("ScalosPrefs");
	lID = ID_MAIN;

	if (p_MyPrefsHandle)
		{
		CONST_STRPTR prefDefIconPath;

		ReadPrefsHandle(p_MyPrefsHandle, PrefsFileName);

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTfAntialiasing, &inst->fpb_TTfAntialias, sizeof(inst->fpb_TTfAntialias) );
		// BYTE

		GetPreferences(p_MyPrefsHandle, lID, SCP_TTfGamma, &inst->fpb_TTfGamma, sizeof(inst->fpb_TTfGamma) );
		inst->fpb_TTfGamma = SCA_BE2WORD(inst->fpb_TTfGamma);

		prefDefIconPath = GetPrefsConfigString(p_MyPrefsHandle, SCP_PathsDefIcons, "ENV:sys");
		stccpy(inst->fpb_DefIconPath, prefDefIconPath, sizeof(inst->fpb_DefIconPath));

		FreePrefsHandle(p_MyPrefsHandle);
		}

	return RETURN_OK;
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetPrefsConfigString(APTR prefsHandle, ULONG Id, CONST_STRPTR DefaultString)
{
	struct PrefsStruct *ps = FindPreferences(prefsHandle, ID_MAIN, Id);

	if (ps)
		return (CONST_STRPTR) PS_DATA(ps);

	return DefaultString;
}


//---------------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	MajorVersion = PluginBase->pl_LibNode.lib_Version;
	MinorVersion = PluginBase->pl_LibNode.lib_Revision;

	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

	d1(kprintf("%s/%ld:   PluginBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

	if (!OpenLibraries())
		return FALSE;

#if !defined(__amigaos4__) && !defined(__AROS__)
	if (_STI_240_InitMemFunctions())
		return FALSE;
#endif

	FontSampleClass = InitFontSampleClass();
	if (NULL == FontSampleClass)
		return FALSE;

	FileTypesPrefsClass = MUI_CreateCustomClass(&PluginBase->pl_LibNode, MUIC_Group,
			NULL, sizeof(struct FileTypesPrefsInst), DISPATCHER_REF(FileTypesPrefs));

	d1(kprintf("%s/%ld:  FileTypesPrefsClass=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsClass));
	if (NULL == FileTypesPrefsClass)
		return FALSE;

	d1(kprintf("%s/%ld: FileTypesPrefsCatalog=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsCatalog));

	d1(kprintf("%s/%ld: LocaleBase=%08lx\n", __FUNC__, __LINE__, LocaleBase));

	if (LocaleBase)
		{
		d1(kprintf("%s/%ld: FileTypesPrefsLocale=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsLocale));

		if (NULL == FileTypesPrefsLocale)
			FileTypesPrefsLocale = OpenLocale(NULL);

		d1(kprintf("%s/%ld: FileTypesPrefsLocale=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsLocale));

		if (FileTypesPrefsLocale)
			{
			d1(kprintf("%s/%ld: FileTypesPrefsCatalog=%08lx\n", __FUNC__, __LINE__, FileTypesPrefsCatalog));

			if (NULL == FileTypesPrefsCatalog)
				{
				FileTypesPrefsCatalog = OpenCatalog(FileTypesPrefsLocale,
					(STRPTR) "Scalos/ScalosFileTypes.catalog", NULL);

				d1(kprintf("%s/%ld: \n", __FUNC__, __LINE__));
				}
			}
		}

	if (!StaticsTranslated)
		{
		StaticsTranslated = TRUE;

		TranslateNewMenu(ContextMenuFileTypes);
		TranslateNewMenu(ContextMenuFileTypesActions);
		TranslateNewMenu(ContextMenus);
		TranslateNewMenu(ContextMenusAttrList);
		TranslateStringArray(RegisterTitles);
		TranslateStringArray(AddFileTypeActionStrings);
		TranslateStringArray(AddFileTypeActionStringsProject);
		TranslateStringArray(AddFileTypeActionStringsDisk);
		TranslateStringArray(FileTypeActionProtectionStrings);
		}

	if (NULL == myFileTypesNListTreeClass)
		{
		myFileTypesNListTreeClass = MUI_CreateCustomClass(NULL, MUIC_NListtree,
			NULL, 0, DISPATCHER_REF(myFileTypesNListTree));
		}
	if (NULL == myFileTypesNListTreeClass)
		return FALSE;	// Failure

	if (NULL == myNListTreeClass)
		{
		myNListTreeClass = MUI_CreateCustomClass(NULL, MUIC_NListtree,
			NULL, 0, DISPATCHER_REF(myNListTree));
		}
	if (NULL == myNListTreeClass)
		return FALSE;	// Failure

	if (NULL == myFileTypesActionsNListClass)
		{
		myFileTypesActionsNListClass = MUI_CreateCustomClass(NULL, MUIC_NList,
			NULL, 0, DISPATCHER_REF(myFileTypesActionsNList));
		}
	if (NULL == myFileTypesActionsNListClass)
		return FALSE;	// Failure

	if (NULL == myNListClass)
		{
		myNListClass = MUI_CreateCustomClass(NULL, MUIC_NList,
			NULL, 0, DISPATCHER_REF(myNList));
		}
	if (NULL == myNListClass)
		return FALSE;	// Failure

	if (NULL == IconobjectClass)
		{
		IconobjectClass = InitIconobjectClass();
		}
	if (NULL == IconobjectClass)
		return FALSE;	// Failure

	if (NULL == DataTypesImageClass)
		DataTypesImageClass = InitDtpicClass();
	d1(KPrintF(__FUNC__ "/%ld: DataTypesImageClass=%08lx\n", __LINE__, DataTypesImageClass));
	if (NULL == DataTypesImageClass)
		return FALSE;	// Failure

	d1(KPrintF("%s/%ld: Success\n", __FUNC__, __LINE__));

	return TRUE;	// Success
}

//----------------------------------------------------------------------------

static ULONG DoDragDrop(Class *cl, Object *obj, Msg msg)
{
	struct FileTypesPrefsInst *inst;
	struct MUI_NListtree_TreeNode *tnTo, *tnFrom;

	get(obj, MUIA_NList_PrivateData, &inst);

	get(obj, MUIA_NListtree_DropTarget, &tnTo);

	tnFrom = MUIV_NListtree_Active_Off;
	get(obj, MUIA_NListtree_Active, &tnFrom);

	d1(kprintf(__FILE__ "/%s/%ld: tnFrom=%08lx  tnTo=%08lx\n", __FUNC__, __LINE__, tnFrom, tnTo));

	if (tnTo && tnFrom && tnTo != tnFrom && MUIV_NListtree_Active_Off != tnFrom)
		{
		struct MUI_NListtree_TreeNode *oldListNode;

		oldListNode = (struct MUI_NListtree_TreeNode *) DoMethod(obj,
			MUIM_NListtree_GetEntry,
			tnFrom,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);

		if (MayPasteOnto(inst, tnTo, tnFrom))
			{
			DoMethod(obj, MUIM_NListtree_Move,
				oldListNode,
				tnFrom,
				tnTo,
				MUIV_NListtree_Move_NewTreeNode_Tail,
				0);
			EntryHasChanged(inst, tnFrom);
			EntryHasChanged(inst, tnTo);
			}
		else
			{
			tnTo = MayPasteBelow(inst, tnTo, tnFrom);
			if (tnTo)
				{
				struct MUI_NListtree_TreeNode *newListNode;

				newListNode = (struct MUI_NListtree_TreeNode *) DoMethod(obj,
					MUIM_NListtree_GetEntry,
					tnTo,
					MUIV_NListtree_GetEntry_Position_Parent,
					0);

				DoMethod(obj, MUIM_NListtree_Move,
					oldListNode,
					tnFrom,
					newListNode,
					tnTo,
					0);
				EntryHasChanged(inst, tnFrom);
				EntryHasChanged(inst, tnTo);
				}
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	if (NULL == inst->fpb_LoadReq)
		{
		inst->fpb_LoadReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "deficons.prefs",
			ASLFR_Flags1, FRF_DOPATTERNS,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_InitialPattern, "#?.(pre|prefs)",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->fpb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}

	if (inst->fpb_LoadReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->fpb_LoadReq,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_OPEN),
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->fpb_LoadReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				InitDefIcons(inst, inst->fpb_LoadReq->fr_File);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	if (NULL == inst->fpb_SaveReq)
		{
		inst->fpb_SaveReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "deficons.prefs",
			ASLFR_DoSaveMode, TRUE,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->fpb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}

	if (inst->fpb_SaveReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->fpb_SaveReq,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_SAVEAS),
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->fpb_SaveReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				WriteDefIconsPrefs(inst, inst->fpb_SaveReq->fr_File);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == iMsg->Class)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_APP_Main], MUIM_Application_CheckRefresh);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT ShowFindGroupHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	(void) hook;
	(void) o;
	(void) msg;

	CleanupFoundNodes(inst);

	setstring(inst->fpb_Objects[OBJNDX_String_FindFileType], "");

	set(inst->fpb_Objects[OBJNDX_Menu_Find], MUIA_Menuitem_Enabled, FALSE);
	set(inst->fpb_Objects[OBJNDX_Group_FindFiletype], MUIA_ShowMe, TRUE);

	set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject,
		inst->fpb_Objects[OBJNDX_String_FindFileType]);

}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT HideFindGroupHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	(void) hook;
	(void) o;
	(void) msg;

	CleanupFoundNodes(inst);

	set(inst->fpb_Objects[OBJNDX_Menu_Find], MUIA_Menuitem_Enabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Group_FindFiletype], MUIA_ShowMe, FALSE);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT IncrementalFindFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	CONST_STRPTR FindString = NULL;

	(void) hook;
	(void) o;
	(void) msg;

	get(inst->fpb_Objects[OBJNDX_String_FindFileType], MUIA_String_Contents, &FindString);

	if (FindString && strlen(FindString) > 0)
		{
		set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled, FALSE);
                
		IncrementalSearchFileType(inst, FindString, FINDDIR_First);
		}
	else
		{
		CleanupFoundNodes(inst);
		set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Button_FindHitCount], MUIA_Text_Contents, "");
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT IncrementalFindNextFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	CONST_STRPTR FindString = NULL;

	(void) hook;
	(void) o;
	(void) msg;

	get(inst->fpb_Objects[OBJNDX_String_FindFileType], MUIA_String_Contents, &FindString);

	if (FindString && strlen(FindString) > 0)
		{
		IncrementalSearchFileType(inst, FindString, FINDDIR_Next);
		}

}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT IncrementalFindPrevFileTypeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	CONST_STRPTR FindString = NULL;

	(void) hook;
	(void) o;
	(void) msg;

	get(inst->fpb_Objects[OBJNDX_String_FindFileType], MUIA_String_Contents, &FindString);

	if (FindString && strlen(FindString) > 0)
		{
		IncrementalSearchFileType(inst, FindString, FINDDIR_Prev);
		}

}

//----------------------------------------------------------------------------

static BOOL IncrementalSearchFileType(struct FileTypesPrefsInst *inst,
	CONST_STRPTR SearchString, enum FindDir dir)
{
	BOOL Found = FALSE;
	BOOL CaseSensitive = FALSE;

	do	{
		struct MUI_NListtree_TreeNode *tn = NULL;
		Object *ActiveObject;

		if (SearchString == NULL || strlen(SearchString) < 1)
			{
			CleanupFoundNodes(inst);
			break;
			}

		get(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject, &ActiveObject);

		/*
		 * When searching for next one, fetch selected one first.
		 */

		if (FINDDIR_Next == dir)
			{
			d1(kprintf(__FILE__ "/%s/%ld:  fpb_CurrentFound=%08lx\n", __FUNC__, __LINE__, inst->fpb_CurrentFound));

			if (inst->fpb_CurrentFound != (struct FoundNode *) &inst->fpb_FoundList.lh_Tail)
				inst->fpb_CurrentFound = (struct FoundNode *) inst->fpb_CurrentFound->fdn_Node.ln_Succ;

			if (inst->fpb_CurrentFound != (struct FoundNode *) &inst->fpb_FoundList.lh_Tail)
				tn = inst->fpb_CurrentFound->fdn_TreeNode;
			else
				tn = NULL;

			d1(kprintf(__FILE__ "/%s/%ld:  fpb_CurrentFound=%08lx  tn=%08lx\n", __FUNC__, __LINE__, inst->fpb_CurrentFound, tn));
			}
		else if (FINDDIR_Prev == dir)
			{
			d1(kprintf(__FILE__ "/%s/%ld:  fpb_CurrentFound=%08lx\n", __FUNC__, __LINE__, inst->fpb_CurrentFound));

			if (inst->fpb_CurrentFound != (struct FoundNode *) &inst->fpb_FoundList.lh_Head)
				inst->fpb_CurrentFound = (struct FoundNode *) inst->fpb_CurrentFound->fdn_Node.ln_Pred;

			if (inst->fpb_CurrentFound != (struct FoundNode *) &inst->fpb_FoundList.lh_Head)
				tn = inst->fpb_CurrentFound->fdn_TreeNode;
			else
				tn = NULL;

			d1(kprintf(__FILE__ "/%s/%ld:  fpb_CurrentFound=%08lx  tn=%08lx\n", __FUNC__, __LINE__, inst->fpb_CurrentFound, tn));
			}
		else
			{
			ULONG dosearch = TRUE;

			d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

			CleanupFoundNodes(inst);

			BuildFindTree(inst,
				inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIV_NListtree_GetEntry_ListNode_Root,
				SearchString,
				MUIV_NListtree_GetEntry_Position_Head,
				&dosearch, CaseSensitive);

			if (IsListEmpty(&inst->fpb_FoundList))
				{
				d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
				tn = NULL;
				inst->fpb_CurrentFound = (struct FoundNode *) &inst->fpb_FoundList.lh_Tail;
				}
			else
				{
				d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
				inst->fpb_CurrentFound = (struct FoundNode *) inst->fpb_FoundList.lh_Head;
				tn = inst->fpb_CurrentFound->fdn_TreeNode;
				}
			d1(kprintf(__FILE__ "/%s/%ld:  fpb_CurrentFound=%08lx  tn=%08lx\n", __FUNC__, __LINE__, inst->fpb_CurrentFound, tn));

			snprintf(inst->fpb_FindHitCount, sizeof(inst->fpb_FindHitCount),
				GetLocString(MSGID_FIND_HITCOUNT_FMT), inst->fpb_FoundCount);
			set(inst->fpb_Objects[OBJNDX_Button_FindHitCount], MUIA_Text_Contents, inst->fpb_FindHitCount);
			}

		if (NULL == tn)
			{
			// nothing found
			set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled, TRUE);
			set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled, TRUE);
			set(inst->fpb_Objects[OBJNDX_Button_FindHitCount], MUIA_Text_Contents, "");
			break;
			}

		Found = TRUE;
		/*
		 * Open node if needed and select.
		 */
		DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_Open,
			MUIV_NListtree_Open_ListNode_Parent,
			tn,
			0);
		set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, tn);

		set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject, ActiveObject);

		set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled,
			inst->fpb_CurrentFound->fdn_Node.ln_Succ == (struct Node *) &inst->fpb_FoundList.lh_Tail);

		set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled,
			inst->fpb_CurrentFound->fdn_Node.ln_Pred == (struct Node *) &inst->fpb_FoundList.lh_Head);
		} while (0);

	return Found;
}

//----------------------------------------------------------------------------

static void BuildFindTree(struct FileTypesPrefsInst *inst,
	Object *ListTree,
	struct MUI_NListtree_TreeNode *list, CONST_STRPTR pattern,
	struct MUI_NListtree_TreeNode *startnode,
	ULONG *dosearch, BOOL CaseSensitive)
{
	ULONG pos;

	for (pos = 0; ; pos++)
		{
		struct MUI_NListtree_TreeNode *tn;

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
			MUIM_NListtree_GetEntry,
                        list,
			pos,
			MUIV_NListtree_GetEntry_Flag_SameLevel);

		if (NULL == tn)
			break;

		if (tn->tn_Flags & TNF_LIST)
			{
			// Current node is a list
			if (*dosearch)
				{
				// Check if list node matches
				CONST_STRPTR Found;

				Found = FindString(tn->tn_Name, pattern, CaseSensitive);

				if (Found)
					{
					struct FileTypesEntry *fte = (struct FileTypesEntry *) tn->tn_User;

					fte->fte_FindLength = strlen(pattern);
					fte->fte_FindStart = Found - tn->tn_Name;

					DoMethod(ListTree, MUIM_NListtree_Redraw, tn, 0);
					AddFoundNode(inst, ListTree, tn);
					}
				}

			// check children for matches
			BuildFindTree(inst,
				ListTree,
				tn,
				pattern,
				startnode,
				dosearch,
				CaseSensitive);
			}

		if ( tn == startnode )
			*dosearch = TRUE;
		}
}

//----------------------------------------------------------------------------

static CONST_STRPTR FindString(CONST_STRPTR string, CONST_STRPTR pattern, BOOL CaseSensitive)
{
	while (*string)
		{
		size_t i;

		for(i = 0 ; ; i++)
			{
			char c = pattern[i];

			/* End of substring? We got a match... */
			if ('\0' == c)
				{
				return string;
				}

			if (CaseSensitive)
				{
				if (c != string[i])
					break;
				}
			else
				{
				if (ToLower(c) != ToLower(string[i]))
					break;
				}
			}

		string++;
		}

	return NULL;
}

//----------------------------------------------------------------------------

static struct FoundNode *AddFoundNode(struct FileTypesPrefsInst *inst, Object *ListTree, struct MUI_NListtree_TreeNode *tn)
{
	struct FoundNode *fdn;

	d1(kprintf(__FILE__ "/%s/%ld: <%s>\n", __FUNC__, __LINE__, tn->tn_Name));

	fdn = malloc(sizeof(struct FoundNode));
	if (fdn)
		{
		fdn->fdn_ListTree = ListTree;
		fdn->fdn_TreeNode = tn;
		fdn->fdn_Index = ++inst->fpb_FoundCount;

		AddTail(&inst->fpb_FoundList, &fdn->fdn_Node);
		}

	return fdn;
}

//----------------------------------------------------------------------------

void CleanupFoundNodes(struct FileTypesPrefsInst *inst)
{
	struct FoundNode *fdn;

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	while ( (fdn = (struct FoundNode *) RemHead(&inst->fpb_FoundList)) )
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) fdn->fdn_TreeNode->tn_User;

		fte->fte_FindLength = 0;
		fte->fte_FindStart = 0;
		DoMethod(fdn->fdn_ListTree, MUIM_NListtree_Redraw, fdn->fdn_TreeNode, 0);

		free(fdn);
		}

	inst->fpb_FoundCount = 0;

	set(inst->fpb_Objects[OBJNDX_Button_FindNextFileType], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Button_FindPrevFileType], MUIA_Disabled, TRUE);
	set(inst->fpb_Objects[OBJNDX_Button_FindHitCount], MUIA_Text_Contents, "");
}

//----------------------------------------------------------------------------

void UpdateMenuImage(struct FileTypesPrefsInst *inst, ULONG ListTree, struct FileTypesListEntry *fte)
{
	char UnSelIconName[MAX_ATTRVALUE];

	if (fte->fte_ImageObject)
		{
		DoMethod(inst->fpb_Objects[ListTree],
			MUIM_NList_UseImage, NULL, fte->fte_ImageIndex, 0);

		MUI_DisposeObject(fte->fte_ImageObject);
		fte->fte_ImageObject = NULL;
		}

	GetAttributeValueString(FindAttribute(fte, ATTRTYPE_UnselIconName), UnSelIconName, sizeof(UnSelIconName));
	if (strlen(UnSelIconName) > 0)
		{
		fte->fte_ImageObject = DataTypesImageObject,
			MUIA_ScaDtpic_Name, (ULONG) UnSelIconName,
			MUIA_ScaDtpic_FailIfUnavailable, TRUE,
			End; //DataTypesMCCObject

		if (fte->fte_ImageObject)
			{
			if (0 == fte->fte_ImageIndex)
				fte->fte_ImageIndex = ++inst->fpb_MenuImageIndex;

			DoMethod(inst->fpb_Objects[ListTree],
				MUIM_NList_UseImage, fte->fte_ImageObject, fte->fte_ImageIndex, 0);
			}
		}
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && ! defined(__amigaos4__)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__) && !defined(__AROS__)
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

#endif /* !__SASC && !__amigaos4__ */

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
