// MenuPrefs.c
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
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <devices/clipboard.h>
#include <utility/utility.h>
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
#include <mui/NListtree_mcc.h>
#include <mui/Lamp_mcc.h>
#include <scalos/scalosprefsplugin.h>
#include <scalos/scalosmenuprefsplugin.h>
#include <defs.h>
#include <Year.h>

#include "MenuPrefs.h"
#include "DataTypesMCC.h"

#define	ScalosMenu_NUMBERS
#define	ScalosMenu_ARRAY
#define	ScalosMenu_CODE
#include STR(SCALOSLOCALE)

#include "plugin.h"

//----------------------------------------------------------------------------

#ifdef __AROS__
#define myNListTreeObject BOOPSIOBJMACRO_START(myNListTreeClass->mcc_Class)
//#define myNListObject BOOPSIOBJMACRO_START(myNListClass->mcc_Class)
//#define myFileTypesNListTreeObject BOOPSIOBJMACRO_START(myFileTypesNListTreeClass->mcc_Class)
#else
#define myNListTreeObject NewObject(myNListTreeClass->mcc_Class, 0
#define myNListObject NewObject(myNListClass->mcc_Class, 0
#define myFileTypesNListTreeObject NewObject(myFileTypesNListTreeClass->mcc_Class, 0
#define myFileTypesActionsNListObject NewObject(myFileTypesActionsNListClass->mcc_Class, 0
#endif

//----------------------------------------------------------------------------

// Intuition limits for various menu entries
#define	MAX_MENU	(NOMENU + 1)
#define	MAX_MENUITEM	(NOITEM + 1)
#define	MAX_SUBITEM	(NOSUB + 1)


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

// defined in DefaultMenu.c

extern const struct DefaultMenuEntry DefaultMenu[];
extern const ULONG DefaultMenuEntries;

//----------------------------------------------------------------------------

// local data structures

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_CycleChain, TRUE, \
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, (name),\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , (key),\
		MUIA_ControlChar  , (key),\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp, HelpText,\
		End


#define	Application_Return_EDIT	0
#define	Application_Return_USE	1001
#define	Application_Return_SAVE	1002

#define	ID_MENU	MAKE_ID('M','E','N','U')

#define	MAX_LINESIZE	500		// maximum length for command strings
#define	MAX_NAMESIZE	100		// maximum length for menu names
#define	MAX_COMMANDNAME	300		// maximum length for command names

struct MenuListEntry
	{
	char llist_HotKey[2];
	UBYTE llist_EntryType;
	UBYTE llist_Flags;
	UBYTE llist_CommandType;
	UBYTE llist_CommandFlags;
	BYTE  llist_Priority;
	ULONG llist_Stack;
	Object *llist_UnSelImageObj;	// Popup menu image
	ULONG llist_UnSelImageIndex;
	char llist_name[MAX_LINESIZE];
	char llist_UnselectedIconName[MAX_COMMANDNAME];
	char llist_SelectedIconName[MAX_COMMANDNAME];
	char llist_MenuItemName[MAX_NAMESIZE + 10];
	};

struct MenuInsertEntry
	{
	struct ScalosMenuTree *mie_TreeEntry;
	ULONG mie_MenuFlags;
	};

// values in llist_Flags
#define	LLISTFLGB_NotRemovable		0
#define	LLISTFLGB_NameNotSetable	1
#define LLISTFLGB_MayNotHaveChildren	2
#define LLISTFLGB_PopupMenu		3
#define	LLISTFLGF_NotRemovable		(1 << LLISTFLGB_NotRemovable)
#define	LLISTFLGF_NameNotSetable	(1 << LLISTFLGB_NameNotSetable)
#define LLISTFLGF_MayNotHaveChildren	(1 << LLISTFLGB_MayNotHaveChildren)
#define LLISTFLGF_PopupMenu		(1 << LLISTFLGB_PopupMenu)


enum ScalosMenuChunkId
	{ 
	SCMID_MainMenu = 0, 
	SCMID_Popup_Disk, 
	SCMID_Popup_Drawer, 
	SCMID_Popup_Tool, 
	SCMID_Popup_Trashcan, 
	SCMID_Popup_Window, 
	SCMID_Popup_AppIcon,
	SCMID_Popup_Desktop,
	}; 

struct ScalosMenuChunk
	{
	UWORD smch_MenuID;		// enum ScalosMenuChunkId
	UWORD smch_Entries;		// number of entries
	struct ScalosMenuTree smch_Menu[1];	// (variable) the menu entries
	};

struct CommandTableEntry
	{
	CONST_STRPTR cte_Command;
	ULONG cte_NameMsgId;
	};

enum MenuEntryType
	{
	MENUENTRY_Menu,
	MENUENTRY_MenuItem,
	MENUENTRY_SubItem,
	MENUENTRY_Invalid,
	};

//----------------------------------------------------------------------------

// aus mempools.lib
#ifndef __amigaos4__
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

// local functions

static BOOL OpenLibraries(void);
static void CloseLibraries(void);

DISPATCHER_PROTO(MenuPrefs);
static Object *CreatePrefsGroup(struct MenuPrefsInst *inst);

static SAVEDS(APTR) INTERRUPT CmdListConstructHookFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *msg);
static SAVEDS(void) INTERRUPT CmdListDestructHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *msg);
static SAVEDS(ULONG) INTERRUPT CmdListDisplayHookFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm);
static SAVEDS(LONG) INTERRUPT CmdListCompareHookFunc(struct Hook *hook, Object *obj, struct NList_CompareMessage *msg);
static SAVEDS(LONG) INTERRUPT CmdListPopupOpenHookFunc(struct Hook *hook, Object *list, Object *str);

static SAVEDS(APTR) INTERRUPT TreeConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm);
static SAVEDS(void) INTERRUPT TreeDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm);
static SAVEDS(ULONG) INTERRUPT TreeDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm);

static SAVEDS(ULONG) INTERRUPT ImagePopAslFileStartHookFunc(struct Hook *hook, Object *o, Msg msg);

static STRPTR GetLocString(ULONG MsgId);
static void TranslateStringArray(STRPTR *stringArray);
static void TranslateNewMenu(struct NewMenu *nm);

static SAVEDS(APTR) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT MergeHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ImportTDHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ImportPHookFunc(struct Hook *hook, Object *o, Msg msg);

static SAVEDS(APTR) INTERRUPT RenameEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ChangeEntry3HookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ChangeUnselectedImageHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ChangeSelectedImageHookFunc(struct Hook *hook, Object *o, Msg msg);

static SAVEDS(APTR) INTERRUPT ChangeHotkeyHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT PopButtonHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AddMenuHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AddCommandHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AddMenuItemHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT RemoveEntryHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseSelectedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandSelectedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CollapseAllHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ExpandAllHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT CmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(APTR) INTERRUPT SettingsChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT HideObsoleteHookFunc(struct Hook *hook, Object *o, Msg msg);

static SAVEDS(void) INTERRUPT MenuCopyHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT MenuCutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT MenuPasteHookFunc(struct Hook *hook, Object *o, Msg msg);

static struct MUI_NListtree_TreeNode *MoveMenuTree(struct MenuPrefsInst *inst,
	ULONG DestListtreeIndex, ULONG SrcListtreeIndex, ULONG EntryIndex);
static struct MUI_NListtree_TreeNode *CopyFileTypesEntry(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnToListNode, struct MUI_NListtree_TreeNode *tnToPrevNode,
	struct MUI_NListtree_TreeNode *tnFrom, 	ULONG destList, ULONG srcList);
static LONG CopyMenuTree(struct MenuPrefsInst *inst,
	Object *ListTreeDest, Object *ListTreeSrc,
	struct MUI_NListtree_TreeNode *Src, struct MUI_NListtree_TreeNode *DestParent);
static struct MUI_NListtree_TreeNode *CopyMenuEntry(struct MenuPrefsInst *inst,
	Object *ListTreeDest, Object *ListTreeSrc,
	struct MUI_NListtree_TreeNode *Src, struct MUI_NListtree_TreeNode *DestParent);
static LONG ReadPrefsFile(struct MenuPrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet);
static LONG WritePrefsFile(struct MenuPrefsInst *inst, CONST_STRPTR Filename);
static LONG SaveMenuNode(struct MenuPrefsInst *inst, struct IFFHandle *iff,
	const struct MenuPrefsPopMenuNode *TreeNode, enum ScalosMenuChunkId ChunkID);
static UWORD CountMenuEntries(struct MenuPrefsInst *inst, Object *ListTree,
	const struct MUI_NListtree_TreeNode *TreeNode, size_t *StringSpace);
static LONG BuildMenuTree(struct MenuPrefsInst *inst, Object *ListTree,
	const struct MUI_NListtree_TreeNode *TreeNode,
	struct ScalosMenuTree **MenuSpace, STRPTR *StringSpace,
	struct ScalosMenuTree **Parent);
static void RemoveAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *baseAddr);
static LONG SaveIcon(struct MenuPrefsInst *inst, CONST_STRPTR IconName);
static void ClearMenuList(struct MenuPrefsInst *inst);
static STRPTR AddMenuString(CONST_STRPTR MenuString, STRPTR *StringSpace);
static void AddAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *BaseAddr);
static void GenerateMenuList(struct MenuPrefsInst *inst, struct ScalosMenuTree *mTree,
	Object *ListTree, struct MUI_NListtree_TreeNode *MenuNode);
static BOOL RequestTdFile(struct MenuPrefsInst *inst, char *FileName, size_t MaxLen);
static BOOL RequestParmFile(struct MenuPrefsInst *inst, char *FileName, size_t MaxLen);
static BOOL CmpWord(CONST_STRPTR Cmd, CONST_STRPTR Line);
static void StripLF(STRPTR Line);
static STRPTR NextWord(STRPTR lp);
static void CopyWord(STRPTR dest, CONST_STRPTR src, size_t MaxLen);
static struct MUI_NListtree_TreeNode *AppMessage_Menu(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode **TreeNode, 
	struct MUI_NListtree_TreeNode **ParentNode, const struct WBArg *wbArg, 
	CONST_STRPTR Path, CONST_STRPTR FileName);
static void EnableCommandArgumentGadgets(struct MenuPrefsInst *inst, const struct MenuListEntry *mle);
static BOOL IsPopupMenu(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode *TreeNode);
static enum MenuEntryType GetMenuEntryType(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *TreeNode);
static ULONG GetMaxCountForEntry(enum MenuEntryType type);
static void SwitchPopButton(struct MenuPrefsInst *inst, UBYTE CommandType);
DISPATCHER_PROTO(myNListTree);
static Object *CreatePrefsImage(void);
static void InitHooks(struct MenuPrefsInst *inst);
static void SetChangedFlag(struct MenuPrefsInst *inst, BOOL changed);
static void AddDefaultMenuContents(struct MenuPrefsInst *inst);
static void ParseToolTypes(struct MenuPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt);
static void InsertMenuRootEntries(struct MenuPrefsInst *inst);
static Object *GetMenuEntryListtree(struct MenuPrefsInst *inst);
static struct MUI_NListtree_TreeNode *GetParentNodeForMenu(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode **PrevNode);
static struct MUI_NListtree_TreeNode *GetParentNodeForMenuItem(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode **PrevNode);
static BOOL MayPasteOnto(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom);
static struct MUI_NListtree_TreeNode *MayPasteBelow(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom);
#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__AROS__)
static char *stpblk(const char *q);
#if !defined(__amigaos4__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__amigaos4__) */
#endif /* !defined(__SASC) && !defined(__MORPHOS__) */

//----------------------------------------------------------------------------

// local data items

#ifndef __AROS__
struct Library *MUIMasterBase;
T_LOCALEBASE LocaleBase;
struct GfxBase *GfxBase;
struct Library *IconBase;
struct Library *IFFParseBase;
T_UTILITYBASE UtilityBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *DataTypesBase;

#ifdef __amigaos4__
struct Library *DOSBase;
struct DOSIFace *IDOS;
struct LocaleIFace *ILocale;
struct MUIMasterIFace *IMUIMaster;
struct GraphicsIFace *IGraphics;
struct IconIFace *IIcon;
struct IFFParseIFace *IIFFParse;
struct DataTypesIFace *IDataTypes;
struct UtilityIFace *IUtility;
struct IntuitionIFace *IIntuition;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

#ifdef __AROS__
struct DosLibrary *DOSBase;
#endif

static BOOL StaticsTranslated;

static ULONG MajorVersion, MinorVersion;

static const struct MUIP_ScalosPrefs_MCCList RequiredMccList[] =
	{
	{ MUIC_Lamp, 11, 1 },
	{ MUIC_NListtree, 18, 18 },
	{ MUIC_NListview, 18, 0 },
	{ NULL, 0, 0 }
	};

static const struct Hook MenuPrefsHooks[] =
{
	{ { NULL, NULL }, HOOKFUNC_DEF(TreeConstructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(TreeDestructFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(TreeDisplayFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListConstructHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListDestructHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListDisplayHookFunc),  },
	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListCompareHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CmdListPopupOpenHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ImagePopAslFileStartHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(ResetToDefaultsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(OpenHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(LastSavedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RestoreHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SaveAsHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(MergeHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ImportTDHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ImportPHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RenameEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangeHotkeyHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangeEntry3HookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangeUnselectedImageHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ChangeSelectedImageHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SelectEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(PopButtonHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddMenuHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddCommandHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AddMenuItemHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(RemoveEntryHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AboutHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseSelectedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandSelectedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CollapseAllHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ExpandAllHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(CmdSelectedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(ContextMenuTriggerHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AppMessageHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(SettingsChangedHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(AslIntuiMsgHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(HideObsoleteHookFunc), NULL },

	{ { NULL, NULL }, HOOKFUNC_DEF(MenuCopyHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(MenuCutHookFunc), NULL },
	{ { NULL, NULL }, HOOKFUNC_DEF(MenuPasteHookFunc), NULL },
};

static struct Locale *MenuPrefsLocale;
static struct Catalog *MenuPrefsCatalog;

struct MUI_CustomClass *MenuPrefsClass;
struct MUI_CustomClass *myNListTreeClass;
struct MUI_CustomClass *DataTypesImageClass;

static STRPTR CmdModeStrings[] =
	{
	(STRPTR) MSGID_COMMANDNAME,
	(STRPTR) MSGID_WBNAME,
	(STRPTR) MSGID_ADOSNAME,
	(STRPTR) MSGID_ICONWINNAME,
	(STRPTR) MSGID_AREXXNAME,
	(STRPTR) MSGID_PLUGINNAME,
	NULL
	};

struct CommandTableEntry CommandsTable[] =
	{
	{ "about",			MSGID_COM5NAME },
	{ "backdrop",			MSGID_COM1NAME },
	{ "cleanup",			MSGID_COM12NAME },
	{ "cleanupbydate",		MSGID_COM44NAME },
	{ "cleanupbyname",		MSGID_COM43NAME },
	{ "cleanupbysize",		MSGID_COM45NAME },
	{ "cleanupbytype",		MSGID_COM46NAME },
	{ "clearselection",		MSGID_COM36NAME },
	{ "clone",			MSGID_COM28NAME },
	{ "close",			MSGID_COM9NAME },
	{ "copy",			MSGID_COM39NAME },
	{ "copyto",			MSGID_COM49NAME },
	{ "createthumbnail",		MSGID_COM51NAME },
	{ "cut",			MSGID_COM40NAME },
	{ "delete",			MSGID_COM27NAME },
	{ "emptytrashcan",		MSGID_COM29NAME },
	{ "executecommand",		MSGID_COM2NAME },
	{ "find",			MSGID_COM_FIND },
	{ "formatdisk",			MSGID_COM33NAME },
	{ "iconify",			MSGID_COM32NAME },
	{ "iconinfo",			MSGID_COM22NAME },
	{ "iconproperties",		MSGID_COM_ICONPROPERTIES },
	{ "lastmsg",			MSGID_COM30NAME },
	{ "leaveout",			MSGID_COM25NAME },
	{ "makedir",			MSGID_COM7NAME },
	{ "moveto",			MSGID_COM50NAME },
	{ "open",			MSGID_COM19NAME },
	{ "openinbrowserwindow",        MSGID_COM_OPENBROWSERWINDOW },
	{ "openinnewwindow",    	MSGID_COM_OPENNEWWINDOW },
	{ "parent",			MSGID_COM8NAME },
	{ "paste",			MSGID_COM41NAME },
	{ "putaway",			MSGID_COM26NAME },
	{ "quit",			MSGID_COM6NAME },
	{ "redo",			MSGID_COM_REDO },
	{ "redraw",			MSGID_COM31NAME },
	{ "redrawall",			MSGID_COM3NAME },
	{ "rename",			MSGID_COM21NAME },
	{ "reset",			MSGID_COM20NAME },
	{ "selectall",			MSGID_COM11NAME },
	{ "showallfiles",		MSGID_COM16NAME },
	{ "showdefault",		MSGID_COM48NAME },
	{ "showonlyicons",		MSGID_COM15NAME },
	{ "shutdown",			MSGID_COM34NAME },
	{ "sizetofit",			MSGID_COM35NAME },
	{ "snapshot",			MSGID_COM23NAME },
	{ "snapshotall",		MSGID_COM14NAME },
	{ "snapshotwindow",		MSGID_COM13NAME },
	{ "thumbnailcachecleanup", 	MSGID_COM_THUMBNAILCACHECLEANUP },
	{ "undo",			MSGID_COM_UNDO },
	{ "unsnapshot",			MSGID_COM24NAME },
	{ "update",			MSGID_COM10NAME },
	{ "updateall",			MSGID_COM4NAME },
	{ "viewbydate",			MSGID_COM38NAME },
	{ "viewbydefault",		MSGID_COM47NAME },
	{ "viewbyicon",			MSGID_COM17NAME },
	{ "viewbysize",			MSGID_COM37NAME },
	{ "viewbytext",			MSGID_COM18NAME },
	{ "viewbytype",			MSGID_COM42NAME },
	{ "windowproperties",		MSGID_COM_WINDOWPROPERTIES },
	};

struct CommandTableEntry *CommandsArray[1 + Sizeof(CommandsTable)];

static struct NewMenu ContextMenus[] =
	{
	{ NM_TITLE, (STRPTR) MSGID_TITLENAME,			NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_OPEN, 		(STRPTR) MSGID_MENU_PROJECT_OPEN_SHORT, 	0,	0, (APTR) HOOKNDX_Open },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_MERGE, 		(STRPTR) MSGID_MENU_PROJECT_MERGE_SHORT,	0,	0, (APTR) HOOKNDX_Merge },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_SAVEAS, 	(STRPTR) MSGID_MENU_PROJECT_SAVEAS_SHORT,	0,	0, (APTR) HOOKNDX_SaveAs },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_IMPORT,		NULL, 						0,	0, NULL },
	{  NM_SUB,  (STRPTR) MSGID_MENU_PROJECT_IMPORT_TD, 	(STRPTR) MSGID_MENU_PROJECT_IMPORT_TD_SHORT, 	0,	0, (APTR) HOOKNDX_ImportTD },
	{  NM_SUB,  (STRPTR) MSGID_MENU_PROJECT_IMPORT_P, 	(STRPTR) MSGID_MENU_PROJECT_IMPORT_P_SHORT, 	0,	0, (APTR) HOOKNDX_ImportP },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSE,	 	NULL, 						NM_ITEMDISABLED,	0, (APTR) HOOKNDX_CollapseSelected },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPAND, 		NULL, 						NM_ITEMDISABLED,	0, (APTR) HOOKNDX_ExpandSelected },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,			0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COLLAPSEALL, 	NULL, 						0,	0, (APTR) HOOKNDX_CollapseAll },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_EXPANDALL, 	NULL, 						0,	0, (APTR) HOOKNDX_ExpandAll },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_COPY, 		NULL, 						0,	0, (APTR) HOOKNDX_MenuCopy },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_CUT, 		NULL, 						0,	0, (APTR) HOOKNDX_MenuCut  },
	{  NM_ITEM, (STRPTR) MSGID_MENU_EDIT_PASTE, 		NULL, 						NM_ITEMDISABLED,      0, (APTR) HOOKNDX_MenuPaste },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_SETTINGS_HIDEOBSOLETE, 	NULL, 						CHECKIT|MENUTOGGLE,	 0, (APTR) HOOKNDX_HideObsolete },
	{  NM_ITEM, NM_BARLABEL,				NULL, 						0,	0, NULL },
	{  NM_ITEM, (STRPTR) MSGID_MENU_PROJECT_ABOUT,		NULL, 						0,	0, (APTR) HOOKNDX_About },

	{   NM_END, NULL, NULL,		0, 0, 0,},
	};

static const LONG StopChunkList[] =
	{
	ID_PREF, ID_MENU,
	};

#define	ENTRYTYPE_MAX 	(1 + SCAMENUTYPE_ToolsMenu)

//                                       dest           source
static const BOOL MayPasteAfterMatrix[ENTRYTYPE_MAX][ENTRYTYPE_MAX] =
	{
	//    0      1      2      3      4
	/* 0 SCAMENUTYPE_MainMenu */
	{ FALSE, FALSE, FALSE, FALSE, FALSE, },
	/* 1 SCAMENUTYPE_Menu */
	{ FALSE,  TRUE,  TRUE, FALSE,  TRUE, },
	/* 2 SCAMENUTYPE_MenuItem */
	{ FALSE,  TRUE,  TRUE,  TRUE, FALSE, },
	/* 3 SCAMENUTYPE_Command */
	{ FALSE, FALSE, FALSE,  TRUE, FALSE, },
	/* 4 SCAMENUTYPE_ToolsMenu */
	{ FALSE,  TRUE, FALSE, FALSE, FALSE, },
	//    0      1      2      3      4
	};
//                                       dest           source
static const BOOL MayPasteIntoMatrix[ENTRYTYPE_MAX][ENTRYTYPE_MAX] =
	{
	//    0      1      2      3      4
	/* 0 SCAMENUTYPE_MainMenu */
	{ FALSE,  TRUE, FALSE, FALSE, FALSE, },
	/* 1 SCAMENUTYPE_Menu */
	{ FALSE,  TRUE,  TRUE, FALSE, FALSE, },
	/* 2 SCAMENUTYPE_MenuItem */
	{ FALSE, FALSE, FALSE,  TRUE, FALSE, },
	/* 3 SCAMENUTYPE_Command */
	{ FALSE, FALSE, FALSE,  TRUE, FALSE, },
	/* 4 SCAMENUTYPE_ToolsMenu */
	{ FALSE,  TRUE,  TRUE, FALSE, FALSE, },
	//    0      1      2      3      4
	};
//----------------------------------------------------------------------------

void closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf("%s/%s/%ld: start\n", __FILE__, __FUNC__, __LINE__));

	if (DataTypesImageClass)
		{
		CleanupDtpicClass(DataTypesImageClass);
		DataTypesImageClass = NULL;
		}
	if (myNListTreeClass)
		{
		MUI_DeleteCustomClass(myNListTreeClass);
		myNListTreeClass = NULL;
		}
	if (MenuPrefsCatalog)
		{
		CloseCatalog(MenuPrefsCatalog);
		MenuPrefsCatalog = NULL;
		}
	if (MenuPrefsLocale)
		{
		CloseLocale(MenuPrefsLocale);
		MenuPrefsLocale = NULL;
		}

	if (MenuPrefsClass)
		{
		MUI_DeleteCustomClass(MenuPrefsClass);
		MenuPrefsClass = NULL;
		}

	CloseLibraries();

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif

	d1(kprintf("%s/%s//%ld: endt\n", __FILE__, __FUNC__, __LINE__));
}


LIBFUNC_P2(ULONG, LIBSCAGetPrefsInfo,
	D0, ULONG, which,
	A6, struct PluginBase *, PluginBase, 5);
{
	ULONG result;

	d1(kprintf("%s/%s/%ld: which=%ld\n", __FILE__, __FUNC__, __LINE__, which));

	(void) PluginBase;

	switch(which)
		{
	case SCAPREFSINFO_GetClass:
		result = (ULONG) MenuPrefsClass;
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

	d1(kprintf("%s/%s/%ld: result=%lu\n", __FILE__, __FUNC__, __LINE__, result));

	return result;
}
LIBFUNC_END

//----------------------------------------------------------------------------

DISPATCHER(MenuPrefs)
{
	struct MenuPrefsInst *inst;
	ULONG result = 0;

	switch(msg->MethodID)
		{
	case OM_NEW:
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		d1(kprintf("%s/%s/%ld: OM_NEW obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		if (obj)
			{
			Object *prefsobject;
			struct opSet *ops = (struct opSet *) msg;

			inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);

			memset(inst, 0, sizeof(struct MenuPrefsInst));

			inst->mpb_TreeImageIndex = 0;
			inst->mpb_Changed = FALSE;
			inst->mpb_WBScreen = LockPubScreen("Workbench");

			InitHooks(inst);

			inst->mpb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, TRUE, ops->ops_AttrList);
			inst->mpb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) "", ops->ops_AttrList);
			inst->mpb_HideObsoletePopupMenus = GetTagData(MUIM_ScalosPrefs_MenuPrefs_HideObsolete, TRUE, ops->ops_AttrList);
			inst->mpb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, (ULONG) NULL, ops->ops_AttrList);
			inst->mpb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, (ULONG) NULL, ops->ops_AttrList);

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
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf("%s/%s/%ld: OM_DISPOSE obj=%08lx\n", __FILE__, __FUNC__, __LINE__, obj));
		if (inst->mpb_WBScreen)
			{
			UnlockPubScreen(NULL, inst->mpb_WBScreen);
			inst->mpb_WBScreen = NULL;
			}
		if (inst->mpb_Objects[OBJNDX_ContextMenu])
			{
			MUI_DisposeObject(inst->mpb_Objects[OBJNDX_ContextMenu]);
			inst->mpb_Objects[OBJNDX_ContextMenu] = NULL;
			}
		if (inst->mpb_SaveReq)
			{
			MUI_FreeAslRequest(inst->mpb_SaveReq);
			inst->mpb_SaveReq = NULL;
			}
		if (inst->mpb_LoadReq)
			{
			MUI_FreeAslRequest(inst->mpb_LoadReq);
			inst->mpb_LoadReq = NULL;
			}
		return DoSuperMethodA(cl, obj, msg);
		break;

	case OM_SET:
		{
		struct opSet *ops = (struct opSet *) msg;

		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);

		if (FindTagItem(MUIM_ScalosPrefs_MenuPrefs_HideObsolete, ops->ops_AttrList))
			{
			inst->mpb_HideObsoletePopupMenus = GetTagData(MUIM_ScalosPrefs_MenuPrefs_HideObsolete, inst->mpb_HideObsoletePopupMenus, ops->ops_AttrList);
			set(inst->mpb_Objects[OBJNDX_Menu_HideObsolete], MUIA_Menuitem_Checked, inst->mpb_HideObsoletePopupMenus);
			CallHookPkt(&inst->mpb_Hooks[HOOKNDX_HideObsolete],
				inst->mpb_Objects[OBJNDX_APP_Main], 0);
			}
		inst->mpb_fCreateIcons = GetTagData(MUIA_ScalosPrefs_CreateIcons, inst->mpb_fCreateIcons, ops->ops_AttrList);
		inst->mpb_ProgramName = (CONST_STRPTR) GetTagData(MUIA_ScalosPrefs_ProgramName, (ULONG) inst->mpb_ProgramName, ops->ops_AttrList);
		inst->mpb_Objects[OBJNDX_WIN_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_MainWindow, 
			(ULONG) inst->mpb_Objects[OBJNDX_WIN_Main], ops->ops_AttrList);
		inst->mpb_Objects[OBJNDX_APP_Main] = (APTR) GetTagData(MUIA_ScalosPrefs_Application, 
			(ULONG) inst->mpb_Objects[OBJNDX_APP_Main], ops->ops_AttrList);

		return DoSuperMethodA(cl, obj, msg);
		}
		break;

	case OM_GET:
		{
		struct opGet *opg = (struct opGet *) msg;

		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		switch (opg->opg_AttrID)
			{
		case MUIA_ScalosPrefs_CreateIcons:
			*opg->opg_Storage = inst->mpb_fCreateIcons;
			result = 0;
			break;
		default:
			result = DoSuperMethodA(cl, obj, msg);
			}
		}
		break;

	case MUIM_ScalosPrefs_ParseToolTypes:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		d1(kprintf(__FILE__ "/%s/%ld: before ParseToolTypes\n", __FUNC__, __LINE__));
		ParseToolTypes(inst, (struct MUIP_ScalosPrefs_ParseToolTypes *) msg);
		d1(kprintf(__FILE__ "/%s/%ld: after ParseToolTypes\n", __FUNC__, __LINE__));
		 break;

	case MUIM_ScalosPrefs_LoadConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		if (RETURN_OK != ReadPrefsFile(inst, "ENV:Scalos/Menu13.prefs", TRUE))
			{
			ClearMenuList(inst);
			ReadPrefsFile(inst, "ENV:Scalos/Menu.prefs", FALSE);
			}
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENV:Scalos/Menu13.prefs");
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_UseConfigIfChanged:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		if (inst->mpb_Changed)
			{
			WritePrefsFile(inst, "ENV:Scalos/Menu13.prefs");
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_SaveConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		WritePrefsFile(inst, "ENVARC:Scalos/Menu13.prefs");
		WritePrefsFile(inst, "ENV:Scalos/Menu13.prefs");
		SetChangedFlag(inst, FALSE);
		break;

	case MUIM_ScalosPrefs_SaveConfigIfChanged:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		if (inst->mpb_Changed)
			{
			WritePrefsFile(inst, "ENVARC:Scalos/Menu13.prefs");
			WritePrefsFile(inst, "ENV:Scalos/Menu13.prefs");
			SetChangedFlag(inst, FALSE);
			}
		break;

	case MUIM_ScalosPrefs_RestoreConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_Restore], 0);
		break;

	case MUIM_ScalosPrefs_ResetToDefaults:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ResetToDefaults], 0);
		break;

	case MUIM_ScalosPrefs_LastSavedConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_LastSaved], 0);
		break;

	case MUIM_ScalosPrefs_PageActive:
		{
		struct MUIP_ScalosPrefs_PageActive *spa = (struct MUIP_ScalosPrefs_PageActive *) msg;

		d1(kprintf("%s/%s/%ld: MUIM_ScalosPrefs_PageActive  isActive=%08lx\n", __FILE__, __FUNC__, __LINE__, spa->isActive));
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		inst->mpb_PageIsActive = spa->isActive;
		}
		break;

	case MUIM_ScalosPrefs_OpenConfig:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_Open], 0);
		break;

	case MUIM_ScalosPrefs_SaveConfigAs:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_SaveAs], 0);
		break;

	case MUIM_ScalosPrefs_About:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_About], 0);
		break;

	case MUIM_ScalosPrefs_LoadNamedConfig:
		{
		struct MUIP_ScalosPrefs_LoadNamedConfig *lnc = (struct MUIP_ScalosPrefs_LoadNamedConfig *) msg;

		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		ClearMenuList(inst);
		ReadPrefsFile(inst, lnc->ConfigFileName, FALSE);
		}
		break;

	case MUIM_ScalosPrefs_MenuPrefs_ImportTD:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ImportTD], 0);
		break;

	case MUIM_ScalosPrefs_MenuPrefs_ImportP:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ImportP], 0);
		break;

	case MUIM_ScalosPrefs_MenuPrefs_Merge:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_Merge], 0);
		break;

	case MUIM_ScalosPrefs_MenuPrefs_CollapseAll:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_CollapseAll], 0);
		break;

	case MUIM_ScalosPrefs_MenuPrefs_ExpandAll:
		inst = (struct MenuPrefsInst *) INST_DATA(cl, obj);
		DoMethod(obj, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ExpandAll], 0);
		break;

	case MUIM_ScalosPrefs_CreateSubWindows:
		result = (ULONG) NULL;
		break;

	case MUIM_ScalosPrefs_GetListOfMCCs:
		result = (ULONG) RequiredMccList;
		break;

	default:
		d1(kprintf("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));
		result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static Object *CreatePrefsGroup(struct MenuPrefsInst *inst)
{
	d1(kprintf("%s/%s//%ld:  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, inst));

	inst->mpb_Objects[OBJNDX_ContextMenu] = MUI_MakeObject(MUIO_MenustripNM, ContextMenus, 0);

	inst->mpb_Objects[OBJNDX_Menu_Copy] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_MenuCopy);
	inst->mpb_Objects[OBJNDX_Menu_Cut] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_MenuCut);
	inst->mpb_Objects[OBJNDX_Menu_Paste] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_MenuPaste);
	inst->mpb_Objects[OBJNDX_Menu_HideObsolete] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_HideObsolete);
	inst->mpb_Objects[OBJNDX_Menu_CollapseSelected] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_CollapseSelected);
	inst->mpb_Objects[OBJNDX_Menu_ExpandSelected] = (Object *) DoMethod(inst->mpb_Objects[OBJNDX_ContextMenu],
			MUIM_FindUData, HOOKNDX_ExpandSelected);

	inst->mpb_Objects[OBJNDX_Group_Main] = VGroup,
		MUIA_Background, MUII_PageBack,

		Child, inst->mpb_Objects[OBJNDX_Group_List] = VGroup,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, GetLocString(MSGID_FRAMETITLE),

			Child, HGroup,
				Child, VGroup,
					Child, inst->mpb_Objects[OBJNDX_MainListView] = NListviewObject,
						MUIA_NListview_NList, inst->mpb_Objects[OBJNDX_MainListTree] = myNListTreeObject,
							MUIA_CycleChain, TRUE,
							MUIA_ContextMenu, inst->mpb_Objects[OBJNDX_ContextMenu],
							MUIA_NList_PrivateData, inst,
							MUIA_NList_Format, ",",
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NListtree_DisplayHook, &inst->mpb_Hooks[HOOKNDX_TreeDisplay],
							MUIA_NListtree_ConstructHook, &inst->mpb_Hooks[HOOKNDX_TreeConstruct],
							MUIA_NListtree_DestructHook, &inst->mpb_Hooks[HOOKNDX_TreeDestruct],
							MUIA_NListtree_DragDropSort, TRUE,
							MUIA_NList_ShowDropMarks, TRUE,
							MUIA_NListtree_AutoVisible, MUIV_NListtree_AutoVisible_Expand,
							End,
						MUIA_ObjectID, MAKE_ID('M','T','R','E'),
						MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
						End,

					Child, NListviewObject,
						MUIA_ShowMe, FALSE,
						MUIA_NListview_NList, inst->mpb_Objects[OBJNDX_HiddenListTree] = myNListTreeObject,
							MUIA_CycleChain, TRUE,
							MUIA_NList_PrivateData, inst,
							MUIA_NList_Format, ",",
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NListtree_DisplayHook, &inst->mpb_Hooks[HOOKNDX_TreeDisplay],
							MUIA_NListtree_ConstructHook, &inst->mpb_Hooks[HOOKNDX_TreeConstruct],
							MUIA_NListtree_DestructHook, &inst->mpb_Hooks[HOOKNDX_TreeDestruct],
							End,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
						End,

					Child, NListviewObject,
						MUIA_ShowMe, FALSE,
						MUIA_NListview_NList, inst->mpb_Objects[OBJNDX_ListTreeClipboard] = myNListTreeObject,
							MUIA_CycleChain, TRUE,
							MUIA_NList_PrivateData, inst,
							MUIA_NList_Format, ",",
							InputListFrame,
							MUIA_Background, MUII_ListBack,
							MUIA_NListtree_DisplayHook, &inst->mpb_Hooks[HOOKNDX_TreeDisplay],
							MUIA_NListtree_ConstructHook, &inst->mpb_Hooks[HOOKNDX_TreeConstruct],
							MUIA_NListtree_DestructHook, &inst->mpb_Hooks[HOOKNDX_TreeDestruct],
							End,
						MUIA_Listview_DragType, MUIV_Listview_DragType_None,
						End,

					End, //VGroup

				Child, BalanceObject,
					End, //Balance

				Child, VGroup,

					Child, inst->mpb_Objects[OBJNDX_Group_Name] = VGroup,
						Child, VGroup,
							Child, LLabel(GetLocString(MSGID_ENTRY_STRING_NAME)),
							Child, inst->mpb_Objects[OBJNDX_NameString] = StringObject,
								MUIA_CycleChain, TRUE,
								MUIA_Frame, MUIV_Frame_String,
								MUIA_String_MaxLen, MAX_NAMESIZE,
								End,
							End, //VGroup
						Child, HGroup,
							MUIA_Weight, 0,
							Child, Label(GetLocString(MSGID_HOTKEYNAME)),
							Child, inst->mpb_Objects[OBJNDX_StringHotkey] = StringObject,
								MUIA_CycleChain, TRUE,
								MUIA_String_MaxLen, 2,
								MUIA_Weight, 10,
								MUIA_Frame, MUIV_Frame_String,
								End,
							End,
						Child, HVSpace,
						End, //VGroup

					Child, HVSpace,

					Child, ColGroup(2),
						MUIA_FrameTitle, GetLocString(MSGID_ENTRY_GROUP_UNSELECTEDIMAGE),
						MUIA_Background, MUII_GroupBack,
						GroupFrame,

						Child, inst->mpb_Objects[OBJNDX_PopAsl_UnselectedImage] = PopaslObject,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, GetLocString(MSGID_POPASL_UNSELECTEDIMAGE_SHORTHELP),
							MUIA_Popasl_Type, ASL_FileRequest,
							MUIA_Popasl_StartHook, &inst->mpb_Hooks[HOOKNDX_ImagePopAslFileStart],
							MUIA_Popstring_String, MUI_MakeObject(MUIO_String, NULL, MAX_COMMANDNAME),
							MUIA_Popstring_Button, PopButton(MUII_PopFile),
							End, //PopaslObject

						Child, inst->mpb_Objects[OBJNDX_DtImage_UnselectedImage] = DataTypesImageObject,
							MUIA_ShortHelp, GetLocString(MSGID_SAMPLE_UNSELECTEDIMAGE_SHORTHELP),
							MUIA_ScaDtpic_Name,  (ULONG) "",
							End, //DataTypesMCCObject
						End, //ColGroup

					Child, ColGroup(2),
						MUIA_FrameTitle, GetLocString(MSGID_ENTRY_GROUP_SELECTEDIMAGE),
						MUIA_Background, MUII_GroupBack,
						GroupFrame,

						Child, inst->mpb_Objects[OBJNDX_PopAsl_SelectedImage] = PopaslObject,
							MUIA_CycleChain, TRUE,
							MUIA_ShortHelp, GetLocString(MSGID_POPASL_SELECTEDIMAGE_SHORTHELP),
							MUIA_Popasl_Type, ASL_FileRequest,
							MUIA_Popasl_StartHook, &inst->mpb_Hooks[HOOKNDX_ImagePopAslFileStart],
							MUIA_Popstring_String, MUI_MakeObject(MUIO_String, NULL, MAX_COMMANDNAME),
							MUIA_Popstring_Button, PopButton(MUII_PopFile),
							End, //PopaslObject

						Child, inst->mpb_Objects[OBJNDX_DtImage_SelectedImage] = DataTypesImageObject,
							MUIA_ShortHelp, GetLocString(MSGID_SAMPLE_SELECTEDIMAGE_SHORTHELP),
							MUIA_ScaDtpic_Name,  (ULONG) "",
							End, //DataTypesMCCObject

						End, //ColGroup

					Child, HVSpace,

					Child, inst->mpb_Objects[OBJNDX_Group_CmdProperties] = VGroup,
						MUIA_FrameTitle, GetLocString(MSGID_MENU_GROUP_COMMAND_PROPERTIES),
						MUIA_Background, MUII_GroupBack,
						GroupFrame,

						Child, inst->mpb_Objects[OBJNDX_Group_String] = VGroup,
							Child, inst->mpb_Objects[OBJNDX_CycleGad] = CycleObject,
								MUIA_Cycle_Entries, CmdModeStrings,
								MUIA_Weight, 5,
								MUIA_CycleChain, TRUE,
								End,

							Child, HGroup,
								Child, inst->mpb_Objects[OBJNDX_PopObject] = PopobjectObject,
									MUIA_ShowMe, FALSE,
									MUIA_CycleChain, TRUE,
									MUIA_Popstring_Button, PopButton(MUII_PopUp),
									MUIA_Popobject_StrObjHook, &inst->mpb_Hooks[HOOKNDX_CmdListPopupOpen],
									MUIA_Popobject_Object, inst->mpb_Objects[OBJNDX_CmdListView] = NListviewObject,
										MUIA_NListview_NList, NListObject, 
											InputListFrame,
											MUIA_Background, MUII_ListBack,
											MUIA_NList_Format, ",",
											MUIA_NList_ConstructHook2, &inst->mpb_Hooks[HOOKNDX_CmdListConstruct],
											MUIA_NList_DestructHook2, &inst->mpb_Hooks[HOOKNDX_CmdListDestruct],
											MUIA_NList_DisplayHook2, &inst->mpb_Hooks[HOOKNDX_CmdListDisplay],
											MUIA_NList_CompareHook2, &inst->mpb_Hooks[HOOKNDX_CmdListCompare],
											MUIA_NList_AdjustWidth, TRUE,
											MUIA_NList_SortType, 1,
											MUIA_NList_TitleMark, MUIV_NList_TitleMark_Down | 1,
											MUIA_NList_SourceArray, CommandsArray,
										End, //NListObject
									End, //NListviewObject
								End, //PopobjectObject

								Child, inst->mpb_Objects[OBJNDX_Popbutton] = PopButton(MUII_PopUp),

								Child, inst->mpb_Objects[OBJNDX_StringCmd] = StringObject,
									MUIA_CycleChain, TRUE,
									MUIA_String_MaxLen, MAX_LINESIZE,
									MUIA_Frame, MUIV_Frame_String,
									End,
								End, //HGroup
							End, //VGroup

						Child, HVSpace,

						Child, ColGroup(4),
							Child, HVSpace,

							Child, Label(GetLocString(MSGID_ARGSNAME)),
							Child, inst->mpb_Objects[OBJNDX_CheckArgs] = CheckMark(FALSE),

							Child, HVSpace,
							End, //ColGroup

						Child, ColGroup(2),
							Child, Label(GetLocString(MSGID_STACKNAME)),
							Child, inst->mpb_Objects[OBJNDX_StringStack] = StringObject,
								MUIA_CycleChain, TRUE,
								MUIA_Frame, MUIV_Frame_String,
								MUIA_Weight, 30,
								MUIA_String_Accept, "0123456789",
								MUIA_String_MaxLen, 10,
								MUIA_String_Integer, DEFAULT_STACKSIZE,
								End,

							Child, Label(GetLocString(MSGID_PRIORITY_SLIDER_NAME)),
							Child, inst->mpb_Objects[OBJNDX_SliderPriority] = SliderObject,
								MUIA_CycleChain, TRUE,
								MUIA_Group_Horiz, TRUE,
								MUIA_Numeric_Min, -128,
								MUIA_Numeric_Max, 127,
								MUIA_Numeric_Value, 0,
								MUIA_ShortHelp, GetLocString(MSGID_PRIORITY_SLIDER_BUBBLE),
								End,
							End, //ColGroup
						End, //VGroup
					Child, HVSpace,
					End, //VGroup
				End, //HGroup

				Child, HGroup,
					Child, inst->mpb_Objects[OBJNDX_Lamp_Changed] = LampObject,
						MUIA_Lamp_Type, MUIV_Lamp_Type_Huge,
						MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
						MUIA_ShortHelp, GetLocString(MSGID_SHORTHELP_LAMP_CHANGED),
						End, //LampObject

					Child, inst->mpb_Objects[OBJNDX_Group_Buttons1] = HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, inst->mpb_Objects[OBJNDX_NewMenuButton] = KeyButtonHelp(GetLocString(MSGID_NEWNAME),
								'm', GetLocString(MSGID_SHORTHELP_NEWMENUBUTTON)),
						Child, inst->mpb_Objects[OBJNDX_NewItemButton] = KeyButtonHelp(GetLocString(MSGID_NEWINAME),
								'i', GetLocString(MSGID_SHORTHELP_NEWITEMBUTTON)),
						Child, inst->mpb_Objects[OBJNDX_NewCommandButton] = KeyButtonHelp(GetLocString(MSGID_NEW2NAME),
								'c', GetLocString(MSGID_SHORTHELP_NEWCOMMANDBUTTON)),
						Child, inst->mpb_Objects[OBJNDX_DelButton] = KeyButtonHelp(GetLocString(MSGID_DELNAME),
								'd', GetLocString(MSGID_SHORTHELP_DELBUTTON)),
						End, //HGroup
					End, //HGroup
			End, //VGroup

		MUIA_ContextMenu, inst->mpb_Objects[OBJNDX_ContextMenu],
	End;

	if (NULL == inst->mpb_Objects[OBJNDX_Group_Main])
		return NULL;

	d1(kprintf("%s/%s//%ld:  ContextMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->mpb_Objects[OBJNDX_ContextMenu]));

	set(inst->mpb_Objects[OBJNDX_Menu_HideObsolete], MUIA_Menuitem_Checked, inst->mpb_HideObsoletePopupMenus);

	DoMethod(inst->mpb_Objects[OBJNDX_CmdListView], MUIM_NList_Sort);

	DoMethod(inst->mpb_Objects[OBJNDX_CmdListView], MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		inst->mpb_Objects[OBJNDX_CmdListView], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_CmdSelected]);
	DoMethod(inst->mpb_Objects[OBJNDX_StringHotkey], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeHotkey], MUIV_TriggerValue);

	DoMethod(inst->mpb_Objects[OBJNDX_PopAsl_UnselectedImage], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeUnselectedImage], MUIV_TriggerValue);

	DoMethod(inst->mpb_Objects[OBJNDX_PopAsl_SelectedImage], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeSelectedImage], MUIV_TriggerValue);

	DoMethod(inst->mpb_Objects[OBJNDX_NameString], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_RenameEntry], MUIV_TriggerValue);

	DoMethod(inst->mpb_Objects[OBJNDX_SliderPriority], MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeEntry3]);

	DoMethod(inst->mpb_Objects[OBJNDX_StringStack], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeEntry3]);

	DoMethod(inst->mpb_Objects[OBJNDX_StringCmd], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeEntry3]);

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime, 
		inst->mpb_Objects[OBJNDX_MainListTree], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_SelectEntry], MUIV_TriggerValue);

	DoMethod(inst->mpb_Objects[OBJNDX_CycleGad], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeEntry3]);

	DoMethod(inst->mpb_Objects[OBJNDX_Popbutton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_PopButton]);

	DoMethod(inst->mpb_Objects[OBJNDX_NewMenuButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_AddMenu]);

	DoMethod(inst->mpb_Objects[OBJNDX_NewItemButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_AddMenuItem]);

	DoMethod(inst->mpb_Objects[OBJNDX_NewCommandButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_AddCommand]);

	DoMethod(inst->mpb_Objects[OBJNDX_DelButton], MUIM_Notify, MUIA_Pressed, FALSE, 
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_RemoveEntry]);


	DoMethod(inst->mpb_Objects[OBJNDX_CheckArgs], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_MainListTree], 2, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ChangeEntry3]);

	DoMethod(inst->mpb_Objects[OBJNDX_Group_Main], MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_Group_Main], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_ContextMenuTrigger], MUIV_TriggerValue );

	DoMethod(inst->mpb_Objects[OBJNDX_Group_Main], MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
		inst->mpb_Objects[OBJNDX_Group_Main], 3, MUIM_CallHook, &inst->mpb_Hooks[HOOKNDX_AppMessage], MUIV_TriggerValue);

	d1(KPrintF("%s/%s//%ld: mpb_HideObsoletePopupMenus=%ld\n", __FILE__, __FUNC__, __LINE__, inst->mpb_HideObsoletePopupMenus));

	InsertMenuRootEntries(inst);

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s//%ld:  LocaleBase=%08lx  IFFParseBase=%08lx\n", __FILE__, __FUNC__, __LINE__, LocaleBase, IFFParseBase));

	return inst->mpb_Objects[OBJNDX_Group_Main];
}


static BOOL OpenLibraries(void)
{
	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
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

	d1(kprintf("%s/%s//%ld:  LocaleBase=%08lx  IFFParseBase=%08lx\n", __FILE__, __FUNC__, __LINE__, LocaleBase, IFFParseBase));

	return TRUE;
}


static void CloseLibraries(void)
{
	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

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

		ltdm->strings[0] = GetLocString(cte->cte_NameMsgId);
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

static SAVEDS(LONG) INTERRUPT CmdListPopupOpenHookFunc(struct Hook *hook, Object *list, Object *str)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	ULONG entries = 0;
	ULONG n;
	CONST_STRPTR string = "";
	size_t length;

	get(list, MUIA_NList_Entries, &entries);
	get(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, &string);

	length = strlen(string);

	d1(KPrintF(__FILE__ "/%s/%ld: string=<%s>  entries=%lu  length=%lu\n", __FUNC__, __LINE__, string, entries, length));

	for (n = 0; n < entries; n++)
		{
		struct CommandTableEntry *cte = NULL;

		DoMethod(list, MUIM_NList_GetEntry, n, &cte);
		if (cte)
			{
			if (0 == Strnicmp(cte->cte_Command, string, length))
				{
				d1(KPrintF(__FILE__ "/%s/%ld: found!\n", __FUNC__, __LINE__, cte));
				set(list, MUIA_NList_Active, n);
				DoMethod(list, MUIM_NList_Jump, n);
				break;
				}
			}
		}

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT TreeConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm)
{
	struct MenuListEntry *mle = AllocPooled(ltcm->MemPool, sizeof(struct MenuListEntry));

	d1(kprintf("%s/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, obj, ltcm, ltcm->MemPool, ltcm->UserData));
	d1(kprintf("%s/%s/%ld: mle=%08lx\n", __FILE__, __FUNC__, __LINE__, mle));

	if (mle)
		{
		const struct MenuInsertEntry *mie = (const struct MenuInsertEntry *) ltcm->UserData;
		struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

		if (ltcm->Name)
			stccpy(mle->llist_name, ltcm->Name, sizeof(mle->llist_name));
		else
			strcpy(mle->llist_name, "");

		d1(KPrintF("%s/%s/%ld: llist_name=<%s>  mtre_type=%ld\n", __FILE__, __FUNC__, __LINE__, mle->llist_name, mie->mie_TreeEntry->mtre_type));

		d1(KPrintF("%s/%s/%ld: mtre_iconnames=%08lx\n", __FILE__, __FUNC__, __LINE__, mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_iconnames));
		if ((mie->mie_TreeEntry->mtre_flags & MTREFLGF_IconNames) &&
			mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_iconnames)
			{
			stccpy(mle->llist_UnselectedIconName, mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_iconnames,
				sizeof(mle->llist_UnselectedIconName));
			stccpy(mle->llist_SelectedIconName,
				mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_iconnames + 1 + strlen(mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_iconnames),
				sizeof(mle->llist_SelectedIconName));

			d1(KPrintF("%s/%s/%ld: llist_UnselectedIconName=<%s>  llist_SelectedIconName=<%s>\n", __FILE__, __FUNC__, __LINE__, mle->llist_UnselectedIconName, mle->llist_SelectedIconName));

			mle->llist_UnSelImageIndex = ++inst->mpb_TreeImageIndex;

			mle->llist_UnSelImageObj = DataTypesImageObject,
				MUIA_ScaDtpic_Name,  (ULONG) mle->llist_UnselectedIconName,
				End;

			d1(KPrintF("%s/%s/%ld: llist_UnSelImageObj=%08lx\n", __FILE__, __FUNC__, __LINE__, mle->llist_UnSelImageObj));

			DoMethod(obj, MUIM_NList_UseImage, mle->llist_UnSelImageObj, mle->llist_UnSelImageIndex, 0);
			}
		else
			{
			mle->llist_UnSelImageIndex = 0;
			strcpy(mle->llist_UnselectedIconName, "");
			strcpy(mle->llist_SelectedIconName, "");
			}

		ltcm->UserData = mle;
		ltcm->Name = mle->llist_name;
		mle->llist_EntryType = mie->mie_TreeEntry->mtre_type;
		mle->llist_Flags = mie->mie_MenuFlags;

		d1(kprintf("%s/%s/%ld: EntryName=<%s> mie_MenuFlags=%02lx  EntryType=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ltcm->Name, mie->mie_MenuFlags, mie->mie_TreeEntry->mtre_type));

		d1(kprintf("%s/%s/%ld: llist_HotKey=%08lx  mtre_hotkey=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, mle->llist_HotKey, mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_hotkey));

		stccpy(mle->llist_HotKey, mie->mie_TreeEntry->MenuCombo.MenuTree.mtre_hotkey, sizeof(mle->llist_HotKey));

		if (SCAMENUTYPE_Command == mle->llist_EntryType)
			{
			d1(kprintf("%s/%s/%ld: llist_name=%08lx  mcom_name=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, mle->llist_name, mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_name));

			stccpy(mle->llist_name,
				mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_name ? mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_name : (STRPTR) "",
				sizeof(mle->llist_name));

			mle->llist_Priority = mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_pri;
			mle->llist_CommandType = mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_type;
			mle->llist_CommandFlags = mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_flags;
			mle->llist_Stack = mie->mie_TreeEntry->MenuCombo.MenuCommand.mcom_stack;
			}
		}

	return mle;
}

static SAVEDS(void) INTERRUPT TreeDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm)
{
	struct MenuListEntry *mle = (struct MenuListEntry *) ltdm->UserData;

	d1(kprintf("%s/%s/%ld: mle=%08lx\n", __FILE__, __FUNC__, __LINE__, mle));

	if (mle)
		{
		if (mle->llist_UnSelImageObj)
			{
			DoMethod(obj, MUIM_NList_UseImage, NULL, mle->llist_UnSelImageIndex, 0);

			MUI_DisposeObject(mle->llist_UnSelImageObj);
			mle->llist_UnSelImageObj = NULL;
			}
		FreePooled(ltdm->MemPool, mle, sizeof(struct MenuListEntry));
		}

	ltdm->UserData = NULL;
}

static SAVEDS(ULONG) INTERRUPT TreeDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm)
{
	static CONST_STRPTR EntryTypeNames[] =
		{
		"\33b\33u",	// MainMenu
		"\33b",		// Menu
		"",		// MenuItem
		"\33i",		// Command
		"\33b\338"	// ToolsMenu
		};
	struct MenuListEntry *mle = (struct MenuListEntry *) ltdm->TreeNode->tn_User;

	d1(kprintf("%s/%s/%ld: mle=%08lx\n", __FILE__, __FUNC__, __LINE__, mle));

	if (mle)
		{
		ltdm->Preparse[0] = (STRPTR) EntryTypeNames[mle->llist_EntryType];

		d1(kprintf("%s/%s/%ld: EntryName=<%s>  commandType=%ld\n", \
			__FILE__, __FUNC__, __LINE__, mle->llist_name, mle->llist_CommandType));

		switch (mle->llist_EntryType)
			{
		case SCAMENUTYPE_Command:
			ltdm->Array[0] = mle->llist_name;
			ltdm->Array[1] = "";
			break;
		case SCAMENUTYPE_MenuItem:
			if (0 == strlen(ltdm->TreeNode->tn_Name))
				ltdm->Array[0] = GetLocString(MSGID_BARNAME);
			else
				{
				if (mle->llist_UnSelImageObj)
					{
					snprintf(mle->llist_MenuItemName, sizeof(mle->llist_MenuItemName),
						"\33o[%ld]%s",
						(long)mle->llist_UnSelImageIndex, ltdm->TreeNode->tn_Name);
					ltdm->Array[0] = mle->llist_MenuItemName;
					}
				else
					{
					ltdm->Array[0] = ltdm->TreeNode->tn_Name;
					}
				}
			ltdm->Array[1] = mle->llist_HotKey;
			break;
		default:
			ltdm->Array[0] = ltdm->TreeNode->tn_Name;
			ltdm->Array[1] = "";
			break;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT ImagePopAslFileStartHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct TagItem *TagList = (struct TagItem *) msg;
//	  struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	d1(KPrintF("%s/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	while (TAG_END != TagList->ti_Tag)
		TagList++;

	TagList->ti_Tag = ASLFR_DrawersOnly;
	TagList->ti_Data = FALSE;
	TagList++;

//	  TagList->ti_Tag = ASLFR_InitialFile;
//	  TagList->ti_Data = (ULONG) inst->fpb_FileName;
//	  TagList++;

	TagList->ti_Tag = ASLFR_InitialDrawer;
	TagList->ti_Data = (ULONG) "THEME:FileTypes/";
	TagList++;

	TagList->ti_Tag = TAG_END;

	return TRUE;
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct ScalosMenu_LocaleInfo li;

	li.li_Catalog = MenuPrefsCatalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR) GetScalosMenuString(&li, MsgId);
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

static SAVEDS(APTR) INTERRUPT ResetToDefaultsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	d1(KPrintF("%s/%s//%ld: START\n", __FILE__, __FUNC__, __LINE__));

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);
	set(inst->mpb_Objects[OBJNDX_HiddenListTree], MUIA_NListtree_Quiet, TRUE);
	d1(KPrintF("%s/%s//%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Remove main menu contents
	ClearMenuList(inst);
	d1(KPrintF("%s/%s//%ld: \n", __FILE__, __FUNC__, __LINE__));

	set(inst->mpb_MainMenuNode.mpn_Listtree,
		MUIA_NListtree_Active, (ULONG) inst->mpb_MainMenuNode.mpn_ListNode);

	d1(KPrintF("%s/%s//%ld: \n", __FILE__, __FUNC__, __LINE__));

	AddDefaultMenuContents(inst);

	d1(KPrintF("%s/%s//%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0);

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);
	set(inst->mpb_Objects[OBJNDX_HiddenListTree], MUIA_NListtree_Quiet, FALSE);

	d1(KPrintF("%s/%s//%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	if (NULL == inst->mpb_LoadReq)
		{
		inst->mpb_LoadReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Menu.pre",
			ASLFR_Flags1, FRF_DOPATTERNS,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_InitialPattern, "#?.(pre|prefs)",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->mpb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}
	if (inst->mpb_LoadReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->mpb_LoadReq,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_OPEN),
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->mpb_LoadReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				ClearMenuList(inst);
				ReadPrefsFile(inst, inst->mpb_LoadReq->fr_File, FALSE);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT MergeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	if (NULL == inst->mpb_LoadReq)
		{
		inst->mpb_LoadReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, "Menu.pre",
			ASLFR_Flags1, FRF_DOPATTERNS,
			ASLFR_InitialDrawer, "SYS:Prefs/presets",
			ASLFR_InitialPattern, "#?.(pre|prefs)",
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->mpb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}
	if (inst->mpb_LoadReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->mpb_LoadReq,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_MERGE_ASLTITLE),
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->mpb_LoadReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				ReadPrefsFile(inst, inst->mpb_LoadReq->fr_File, FALSE);

				SetChangedFlag(inst, TRUE);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}

static SAVEDS(APTR) INTERRUPT LastSavedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	ClearMenuList(inst);
	ReadPrefsFile(inst, "ENVARC:Scalos/Menu13.prefs", FALSE);
	SetChangedFlag(inst, TRUE);

	return 0;
}

static SAVEDS(APTR) INTERRUPT RestoreHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	ClearMenuList(inst);
	ReadPrefsFile(inst, "ENV:Scalos/Menu13.prefs", FALSE);
	SetChangedFlag(inst, TRUE);

	return 0;
}

static SAVEDS(APTR) INTERRUPT SaveAsHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	if (NULL == inst->mpb_SaveReq)
		{
		CONST_STRPTR InitialDrawer;
		CONST_STRPTR InitialFile;

		if (inst->mpb_LoadReq)
			{
			InitialDrawer = inst->mpb_LoadReq->fr_Drawer;
			InitialFile  = inst->mpb_LoadReq->fr_File;
			}
		else
			{
			InitialDrawer = "SYS:Prefs/presets";
			InitialFile = "Menu.pre";
			}

		inst->mpb_SaveReq = MUI_AllocAslRequestTags(ASL_FileRequest,
			ASLFR_InitialFile, InitialFile,
			ASLFR_DoSaveMode, TRUE,
			ASLFR_InitialDrawer, InitialDrawer,
			ASLFR_UserData, inst,
			ASLFR_IntuiMsgFunc, &inst->mpb_Hooks[HOOKNDX_AslIntuiMsg],
			TAG_END);
		}
	if (inst->mpb_SaveReq)
		{
		BOOL Result;
		struct Window *win = NULL;

		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		//AslRequest(
		Result = MUI_AslRequestTags(inst->mpb_SaveReq,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_SAVEAS),
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			TAG_END);

		if (Result)
			{
			BPTR dirLock = Lock(inst->mpb_SaveReq->fr_Drawer, ACCESS_READ);

			if (dirLock)
				{
				BPTR oldDir = CurrentDir(dirLock);

				WritePrefsFile(inst, inst->mpb_SaveReq->fr_File);

				CurrentDir(oldDir);
				UnLock(dirLock);
				}
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT AboutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	MUI_Request(inst->mpb_Objects[OBJNDX_APP_Main], inst->mpb_Objects[OBJNDX_WIN_Main], 0, NULL, 
		GetLocString(MSGID_ABOUTREQOK), 
		GetLocString(MSGID_ABOUTREQFORMAT), 
		MajorVersion, MinorVersion, COMPILER_STRING, CURRENTYEAR);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT RenameEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *TreeNode;

	if (inst->mpb_QuietFlag)
		return 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;
		STRPTR newName = "";
		BOOL MayAddCommand, MayHaveHotkey;

		SetChangedFlag(inst, TRUE);

		get(inst->mpb_Objects[OBJNDX_NameString], MUIA_String_Contents, &newName);

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);

		stccpy(mle->llist_name, newName, sizeof(mle->llist_name));

		if (0 == strlen(newName))
			{
			// Remove all children from the new "=== Bar ==="
			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
				MUIM_NListtree_Remove,
				TreeNode,
				MUIV_NListtree_Remove_TreeNode_All);

			// BAR may not have a hotkey
			strcpy(mle->llist_HotKey, "");

			TreeNode->tn_Flags |= TNF_NOSIGN;
			}
		else
			{
			TreeNode->tn_Flags &= ~TNF_NOSIGN;
			}

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Rename,
			TreeNode,
			newName,
			0);

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);

		MayAddCommand = (strlen(mle->llist_name) > 0) &&
			((SCAMENUTYPE_MenuItem == mle->llist_EntryType)
			 || (SCAMENUTYPE_Command == mle->llist_EntryType));

		MayHaveHotkey = (strlen(mle->llist_name) > 0)
			&& SCAMENUTYPE_MenuItem == mle->llist_EntryType;

		set(inst->mpb_Objects[OBJNDX_StringHotkey], MUIA_Disabled, !MayHaveHotkey);
		set(inst->mpb_Objects[OBJNDX_NewCommandButton], MUIA_Disabled, !MayAddCommand);
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT ChangeHotkeyHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	STRPTR newHotKey = *(STRPTR *) msg;
	struct MUI_NListtree_TreeNode *TreeNode;

	if (inst->mpb_QuietFlag)
		return 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		SetChangedFlag(inst, TRUE);

		mle->llist_HotKey[0] = *newHotKey;
		mle->llist_HotKey[1] = '\0';

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Redraw, 
			TreeNode,
			0);
		}

	return 0;
}

static SAVEDS(APTR) INTERRUPT ChangeEntry3HookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *TreeNode;

	if (inst->mpb_QuietFlag)
		return 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		if (SCAMENUTYPE_Command == mle->llist_EntryType)
			{
			STRPTR string = NULL;
			ULONG Value = 0;
			LONG sValue = 0;

			SetChangedFlag(inst, TRUE);

			get(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, &string);
			stccpy(mle->llist_name, string, sizeof(mle->llist_name));

			get(inst->mpb_Objects[OBJNDX_CycleGad], MUIA_Cycle_Active, &Value);
			mle->llist_CommandType = Value;

			SwitchPopButton(inst, Value);

			get(inst->mpb_Objects[OBJNDX_CheckArgs], MUIA_Selected, &Value);
			if (Value)
				mle->llist_CommandFlags |= MCOMFLGF_Args;
			else
				mle->llist_CommandFlags &= ~MCOMFLGF_Args;

			EnableCommandArgumentGadgets(inst, mle);

			get(inst->mpb_Objects[OBJNDX_StringStack], MUIA_String_Integer, &mle->llist_Stack);

			get(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Numeric_Value, &sValue);
			mle->llist_Priority = sValue;

			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NList_Redraw, MUIV_NList_Redraw_Active);
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT ChangeUnselectedImageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	STRPTR NewImage = *(STRPTR *) msg;
	struct MUI_NListtree_TreeNode *TreeNode;

	if (inst->mpb_QuietFlag)
		return 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(o,
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		SetChangedFlag(inst, TRUE);

		stccpy(mle->llist_UnselectedIconName, NewImage, sizeof(mle->llist_UnselectedIconName));
		set(inst->mpb_Objects[OBJNDX_DtImage_UnselectedImage], MUIA_ScaDtpic_Name, (ULONG) mle->llist_UnselectedIconName);

		if (mle->llist_UnSelImageObj)
			{
			MUI_DisposeObject(mle->llist_UnSelImageObj);
			mle->llist_UnSelImageObj = NULL;

			DoMethod(o, MUIM_NList_UseImage, mle->llist_UnSelImageObj, mle->llist_UnSelImageIndex, 0);
			}
		if (0 == mle->llist_UnSelImageIndex)
			mle->llist_UnSelImageIndex = ++inst->mpb_TreeImageIndex;

		if (strlen(mle->llist_UnselectedIconName) > 0)
			{
			mle->llist_UnSelImageObj = DataTypesImageObject,
				MUIA_ScaDtpic_Name,  (ULONG) mle->llist_UnselectedIconName,
				End;
			DoMethod(o, MUIM_NList_UseImage, mle->llist_UnSelImageObj, mle->llist_UnSelImageIndex, 0);
			}

		DoMethod(o,
			MUIM_NListtree_Redraw,
			TreeNode,
			0);
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT ChangeSelectedImageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	STRPTR NewImage = *(STRPTR *) msg;
	struct MUI_NListtree_TreeNode *TreeNode;

	if (inst->mpb_QuietFlag)
		return 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(o,
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		SetChangedFlag(inst, TRUE);

		stccpy(mle->llist_SelectedIconName, NewImage, sizeof(mle->llist_SelectedIconName));
		set(inst->mpb_Objects[OBJNDX_DtImage_SelectedImage], MUIA_ScaDtpic_Name, (ULONG) mle->llist_SelectedIconName);

		DoMethod(o,
			MUIM_NListtree_Redraw,
			TreeNode,
			0);
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT SelectEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *TreeNode = *(struct MUI_NListtree_TreeNode **) msg;

	if (inst->mpb_QuietFlag)
		return 0;

	inst->mpb_QuietFlag = TRUE;

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		if (mle)
			{
			struct MUI_NListtree_TreeNode *tnShadow;
			BOOL MayAddNewMenu, MayAddNewItem, MayAddCommand;
			BOOL MayChangeName, MayDelete, MayHaveHotkey;
			BOOL MayHaveImage;
			enum MenuEntryType menuType;
			ULONG Count, MaxCount;

			Count = DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_GetNr, 
				TreeNode, MUIV_NListtree_GetNr_Flag_CountLevel);

			menuType = GetMenuEntryType(inst, TreeNode);
			MaxCount = GetMaxCountForEntry(menuType);

			MayAddNewMenu = (SCAMENUTYPE_Command != mle->llist_EntryType) &&
				Count < MaxCount &&
				menuType < MENUENTRY_SubItem;

			d1(kprintf("%s/%s/%ld: menuType=%lu  EntryCount=%lu  MaxCount=%lu\n", \
				__FILE__, __FUNC__, __LINE__, menuType, Count, MaxCount));

			MayAddNewItem = ((SCAMENUTYPE_MainMenu != mle->llist_EntryType) || (mle->llist_Flags & LLISTFLGF_PopupMenu)) &&
				Count < MaxCount &&
				(!(mle->llist_Flags & LLISTFLGF_MayNotHaveChildren) || (mle->llist_Flags & LLISTFLGF_PopupMenu));

			MayAddCommand = (mle->llist_name && strlen(mle->llist_name) > 0) &&
				((SCAMENUTYPE_MenuItem == mle->llist_EntryType)
				 || (SCAMENUTYPE_Command == mle->llist_EntryType));

			MayChangeName = (SCAMENUTYPE_Command != mle->llist_EntryType)
				&& (SCAMENUTYPE_MainMenu != mle->llist_EntryType)
				&& !(mle->llist_Flags & LLISTFLGF_NameNotSetable);

			MayDelete = !(mle->llist_Flags & LLISTFLGF_NotRemovable);

			MayHaveHotkey = (strlen(mle->llist_name) > 0)
				&& SCAMENUTYPE_MenuItem == mle->llist_EntryType;

			MayHaveImage = (mle->llist_name && strlen(mle->llist_name) > 0) &&
				IsPopupMenu(inst, TreeNode) &&
				(SCAMENUTYPE_Menu == mle->llist_EntryType
				|| SCAMENUTYPE_MenuItem	== mle->llist_EntryType);

			if (SCAMENUTYPE_Command == mle->llist_EntryType)
				{
				TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Parent, 0);
				}

			// Cannot cut or copy entire main/tool/popup menus
			if ((SCAMENUTYPE_MainMenu == mle->llist_EntryType)
				|| (SCAMENUTYPE_ToolsMenu == mle->llist_EntryType))
				{
				set(inst->mpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, FALSE);
				set(inst->mpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, FALSE);
				}
			else
				{
				set(inst->mpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, TRUE);
				set(inst->mpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, TRUE);
				}

			d1(kprintf("%s/%s/%ld: tn_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, TreeNode->tn_Name));

			set(inst->mpb_Objects[OBJNDX_NameString], MUIA_String_Contents, (ULONG) TreeNode->tn_Name);
			set(inst->mpb_Objects[OBJNDX_StringHotkey], MUIA_String_Contents, (ULONG) mle->llist_HotKey);
			set(inst->mpb_Objects[OBJNDX_PopAsl_UnselectedImage], MUIA_String_Contents, (ULONG) mle->llist_UnselectedIconName);
			set(inst->mpb_Objects[OBJNDX_PopAsl_SelectedImage], MUIA_String_Contents, (ULONG) mle->llist_SelectedIconName);
			set(inst->mpb_Objects[OBJNDX_DtImage_UnselectedImage], MUIA_ScaDtpic_Name, (ULONG) mle->llist_UnselectedIconName);
			set(inst->mpb_Objects[OBJNDX_DtImage_SelectedImage], MUIA_ScaDtpic_Name, (ULONG) mle->llist_SelectedIconName);

			set(inst->mpb_Objects[OBJNDX_DelButton], MUIA_Disabled, !MayDelete);
			set(inst->mpb_Objects[OBJNDX_NameString], MUIA_Disabled, !MayChangeName);
			set(inst->mpb_Objects[OBJNDX_NewItemButton], MUIA_Disabled, !MayAddNewItem);
			set(inst->mpb_Objects[OBJNDX_NewCommandButton], MUIA_Disabled, !MayAddCommand);
			set(inst->mpb_Objects[OBJNDX_NewMenuButton], MUIA_Disabled, !MayAddNewMenu);
			set(inst->mpb_Objects[OBJNDX_PopAsl_UnselectedImage], MUIA_Disabled, !MayHaveImage);
			set(inst->mpb_Objects[OBJNDX_PopAsl_SelectedImage], MUIA_Disabled, !MayHaveImage);

			set(inst->mpb_Objects[OBJNDX_Menu_CollapseSelected], MUIA_Menuitem_Enabled, TRUE);
			set(inst->mpb_Objects[OBJNDX_Menu_ExpandSelected], MUIA_Menuitem_Enabled, TRUE);

			set(inst->mpb_Objects[OBJNDX_StringHotkey], MUIA_Disabled, !MayHaveHotkey);
			set(inst->mpb_Objects[OBJNDX_Group_String], MUIA_Disabled, (SCAMENUTYPE_Command != mle->llist_EntryType));
			set(inst->mpb_Objects[OBJNDX_Group_CmdProperties], MUIA_Disabled, (SCAMENUTYPE_Command != mle->llist_EntryType));

			tnShadow = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_ListTreeClipboard],
				MUIM_NListtree_GetEntry,
				MUIV_NListtree_Insert_ListNode_Root,
				MUIV_NListtree_GetEntry_Position_Head,
				0);

			if (tnShadow)
				{
				if (MayPasteBelow(inst, TreeNode, tnShadow))
					{
					set(inst->mpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, TRUE);
					}
				else if (MayPasteOnto(inst, TreeNode, tnShadow))
					{
					set(inst->mpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, TRUE);
					}
				else
					{
					set(inst->mpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, FALSE);
					}
				}
			else
				{
				set(inst->mpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, FALSE);
				}

			if (SCAMENUTYPE_Command == mle->llist_EntryType)
				{
				SwitchPopButton(inst, mle->llist_CommandType);

				set(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, (ULONG) mle->llist_name);
				set(inst->mpb_Objects[OBJNDX_StringStack], MUIA_String_Integer, (ULONG) mle->llist_Stack);
				set(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Numeric_Value, (LONG) mle->llist_Priority);
				set(inst->mpb_Objects[OBJNDX_CycleGad], MUIA_Cycle_Active, (ULONG) mle->llist_CommandType);

				}
			else
				{
				set(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, (ULONG) "");
				}

			EnableCommandArgumentGadgets(inst, mle);
			}
		}
	else
		{
		set(inst->mpb_Objects[OBJNDX_Menu_Copy], MUIA_Menuitem_Enabled, FALSE);
		set(inst->mpb_Objects[OBJNDX_Menu_Cut], MUIA_Menuitem_Enabled, FALSE);
		set(inst->mpb_Objects[OBJNDX_Menu_CollapseSelected], MUIA_Menuitem_Enabled, FALSE);
		set(inst->mpb_Objects[OBJNDX_Menu_ExpandSelected], MUIA_Menuitem_Enabled, FALSE);

		set(inst->mpb_Objects[OBJNDX_NameString], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_StringHotkey], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_DelButton], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_Group_String], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_Group_CmdProperties], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_PopAsl_UnselectedImage], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_PopAsl_SelectedImage], MUIA_Disabled, TRUE);
#if 0
		set(inst->mpb_Objects[OBJNDX_Menu_Paste], MUIA_Menuitem_Enabled, FALSE);
#endif
		}

	inst->mpb_QuietFlag = FALSE;

	return 0;
}

static SAVEDS(APTR) INTERRUPT PopButtonHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MenuListEntry *mle;
	struct FileRequester *Req;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG CommandType = 0;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);
	if (NULL == TreeNode)
		return 0;

	get(inst->mpb_Objects[OBJNDX_CycleGad], MUIA_Cycle_Active, &CommandType);

	mle = (struct MenuListEntry *) TreeNode->tn_User;
	Req = MUI_AllocAslRequest(ASL_FileRequest, NULL);

	if (Req)
		{
		char FileName[512];
		struct Window *win = NULL;
		BOOL Result;
		STRPTR p, path = NULL;
		BOOL drawersOnly = FALSE;
		ULONG AslTitle = MSGID_IMPORTNAME;

		get(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, &path);
		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		switch (CommandType)
			{
		case SCAMENUCMDTYPE_AmigaDOS:
			AslTitle = MSGID_POPUP_ASLTITLE_AMIGADOS;
			break;
		case SCAMENUCMDTYPE_Workbench:
			AslTitle = MSGID_POPUP_ASLTITLE_WORKBENCH;
			break;
		case SCAMENUCMDTYPE_IconWindow:
			drawersOnly = TRUE;
			AslTitle = MSGID_POPUP_ASLTITLE_ICONWINDOW;
			break;
		case SCAMENUCMDTYPE_ARexx:
			AslTitle = MSGID_POPUP_ASLTITLE_AREXX;
			break;
		case SCAMENUCMDTYPE_Plugin:
			AslTitle = MSGID_POPUP_ASLTITLE_MENUPLUGIN;
			if (0 == strlen(path))
				strcpy(path, "scalos:Plugins/Menu/");
			break;
		default:
			break;
			}

		if (drawersOnly)
			strcpy(FileName, "");
		else
			stccpy(FileName, FilePart(path), sizeof(FileName));

		p = PathPart(path);
		*p = '\0';

		d1(kprintf("%s/%s//%ld:  InitialFile=<%s>  InitialDrawer=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName, path));
		d1(kprintf("%s/%s//%ld:  DrawersOnly=%ld\n", __FILE__, __FUNC__, __LINE__, drawersOnly));

		Result = MUI_AslRequestTags(Req,
			ASLFR_TitleText, GetLocString(MSGID_MENU_PROJECT_SAVEAS),
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(AslTitle),
			ASLFR_Flags1, FRF_DOPATTERNS,
			ASLFR_InitialFile, FileName,
			ASLFR_InitialDrawer, path,
			ASLFR_DrawersOnly, drawersOnly,
			TAG_END);

		if (Result)
			{
			struct DiskObject *icon;

			SetChangedFlag(inst, TRUE);

			d1(kprintf("%s/%s//%ld:  fr_Drawer=<%s>  fr_File=<%s>\n", __FILE__, __FUNC__, __LINE__, Req->fr_Drawer, Req->fr_File));

			stccpy(FileName, Req->fr_Drawer, sizeof(FileName));

			if (!drawersOnly)
				AddPart(FileName, Req->fr_File, sizeof(FileName));

			d1(kprintf("%s/%s//%ld:  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

			set(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, (ULONG) FileName);
			stccpy(mle->llist_name, FileName, sizeof(mle->llist_name));

			icon = GetDiskObject(FileName);
			if (icon)
				{
				STRPTR ToolString;
				ULONG ToolPri = 0;

				get(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Numeric_Value, &ToolPri);
				set(inst->mpb_Objects[OBJNDX_StringStack], MUIA_String_Integer, icon->do_StackSize);

				ToolString = FindToolType(icon->do_ToolTypes, "TOOLPRI");
				if (ToolString)
					{
					long long1;
					sscanf(ToolString, "%ld", &long1);
					ToolPri = long1;
					}

				set(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Numeric_Value, (LONG) ToolPri);

				FreeDiskObject(icon);
				}

			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NList_Redraw, MUIV_NList_Redraw_Active);
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT AddMenuHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ParentNode;
	struct MUI_NListtree_TreeNode *PrevNode;
	struct MenuInsertEntry mie;
	struct ScalosMenuTree mTree;

	memset(&mTree, 0, sizeof(mTree));
	mTree.mtre_type = SCAMENUTYPE_Menu;
	mie.mie_TreeEntry = &mTree;
	mie.mie_MenuFlags = 0;

	SetChangedFlag(inst, TRUE);

	ParentNode = GetParentNodeForMenu(inst, &PrevNode);

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert, GetLocString(MSGID_NEW_MENU),
		&mie,
		ParentNode, PrevNode, MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

	// try to activate menu name string gadget
	set(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject,
		(ULONG) inst->mpb_Objects[OBJNDX_NameString]);

	return 0;
}


static SAVEDS(APTR) INTERRUPT AddCommandHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ParentNode;
	struct MUI_NListtree_TreeNode *TreeNode;

	SetChangedFlag(inst, TRUE);

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_GetEntry, 0, MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;
		struct MenuInsertEntry mie;
		struct ScalosMenuTree mTree;

		memset(&mTree, 0, sizeof(mTree));
		mTree.mtre_type = SCAMENUTYPE_Command;
		mie.mie_TreeEntry = &mTree;
		mie.mie_MenuFlags = 0;

		switch (mle->llist_EntryType)
			{
		case SCAMENUTYPE_MainMenu:
		case SCAMENUTYPE_Menu:
		case SCAMENUTYPE_ToolsMenu:
			break;

		case SCAMENUTYPE_MenuItem:
			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
				MUIM_NListtree_Insert, "", 
				&mie,
				TreeNode, MUIV_NListtree_Insert_PrevNode_Tail, 
				MUIV_NListtree_Insert_Flag_Active);

			// try to activate command name string gadget
			set(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject,
				(ULONG) inst->mpb_Objects[OBJNDX_StringCmd]);
			break;

		case SCAMENUTYPE_Command:
			ParentNode = TreeNode;

			TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry, TreeNode,
				MUIV_NListtree_GetEntry_Position_Parent, 0);

			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
				MUIM_NListtree_Insert, "", 
				&mie,
				TreeNode, ParentNode, 
				MUIV_NListtree_Insert_Flag_Active);

			// try to activate command name string gadget
			set(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject,
				(ULONG) inst->mpb_Objects[OBJNDX_StringCmd]);
			break;
			}
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT AddMenuItemHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ParentNode;
	struct MUI_NListtree_TreeNode *PrevNode;

	SetChangedFlag(inst, TRUE);

	ParentNode = GetParentNodeForMenuItem(inst, &PrevNode);

	if (ParentNode)
		{
		struct MUI_NListtree_TreeNode *newNode;
		struct MenuInsertEntry mie;
		struct ScalosMenuTree mTree;

		memset(&mTree, 0, sizeof(mTree));
		mTree.mtre_type = SCAMENUTYPE_MenuItem;
		mie.mie_TreeEntry = &mTree;
		mie.mie_MenuFlags = 0;

		newNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_Insert, GetLocString(MSGID_NEW_ITEM),
			&mie,
			ParentNode, PrevNode, 
			MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_GetNr, 
			newNode, MUIV_NListtree_GetNr_Flag_CountLevel);

		// try to activate menu name string gadget
		set(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject,
			(ULONG) inst->mpb_Objects[OBJNDX_NameString]);

		d1(kprintf("%s/%s/%ld: Node Count=%ld\n", __FILE__, __FUNC__, __LINE__, Count));
		}

	return 0;
}


static SAVEDS(APTR) INTERRUPT RemoveEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Active,
		MUIV_NListtree_Remove_TreeNode_Active, 0);

	SetChangedFlag(inst, TRUE);

	return 0;
}

//----------------------------------------------------------------------------

static LONG ReadPrefsFile(struct MenuPrefsInst *inst, CONST_STRPTR Filename, BOOL Quiet)
{
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;
	struct ScalosMenuChunk *menuChunk = NULL;

	set(inst->mpb_Objects[OBJNDX_MainListView], MUIA_NListtree_Quiet, TRUE);

	d1(kprintf("%s/%s//%ld:  LocaleBase=%08lx  IFFParseBase=%08lx\n", __FILE__, __FUNC__, __LINE__, LocaleBase, IFFParseBase));

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

		while (1)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);

			d1(kprintf("%s/%s//%ld:  cn=%08lx\n", __FILE__, __FUNC__, __LINE__, cn));
			if (NULL == cn)
				break;

			if (ID_MENU == cn->cn_ID)
				{
				LONG Actual;

				menuChunk = malloc(cn->cn_Size);

				d1(kprintf("%s/%s//%ld:  menuChunk=%08lx\n", __FILE__, __FUNC__, __LINE__, menuChunk));
				if (NULL == menuChunk)
					break;

				Actual = ReadChunkBytes(iff, menuChunk, cn->cn_Size);
				if (Actual != cn->cn_Size)
					break;

				menuChunk->smch_MenuID = SCA_BE2WORD(menuChunk->smch_MenuID);
				menuChunk->smch_Entries = SCA_BE2WORD(menuChunk->smch_Entries);

				d1(kprintf("%s/%s/%ld: MenuID=%ld  Entries=%ld\n", \
					__FILE__, __FUNC__, __LINE__, menuChunk->smch_MenuID, menuChunk->smch_Entries));

				AddAddresses(menuChunk->smch_Menu, (UBYTE *) menuChunk);

				switch (menuChunk->smch_MenuID)
					{
				case SCMID_MainMenu:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_MainMenuNode.mpn_Listtree,
						inst->mpb_MainMenuNode.mpn_ListNode);
					break;
				case SCMID_Popup_Disk:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_ListNode);
					break;
				case SCMID_Popup_Drawer:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_ListNode);
					break;
				case SCMID_Popup_Tool:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_ListNode);
					break;
				case SCMID_Popup_Trashcan:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_ListNode);
					break;
				case SCMID_Popup_Window:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_ListNode);
					break;
				case SCMID_Popup_AppIcon:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_ListNode);
					break;
				case SCMID_Popup_Desktop:
					GenerateMenuList(inst, menuChunk->smch_Menu,
						inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_Listtree,
						inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_ListNode);
					break;
					}

				free(menuChunk);
				menuChunk = NULL;
				}
			}

		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (IFFERR_EOF == Result)
			Result = RETURN_OK;
		} while (0);

	set(inst->mpb_Objects[OBJNDX_MainListView], MUIA_NListtree_Quiet, FALSE);

	if (menuChunk)
		free(menuChunk);

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
		MUI_Request(inst->mpb_Objects[OBJNDX_APP_Main], inst->mpb_Objects[OBJNDX_WIN_Main], 0, NULL, 
			GetLocString(MSGID_ABOUTREQOK), 
			GetLocString(MSGID_REQTITLE_READERROR), 
			Filename,
			Buffer);
		}

//	DoMethod(ScalosPenList, MUIM_NList_Redraw, MUIV_NList_Redraw_All);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG WritePrefsFile(struct MenuPrefsInst *inst, CONST_STRPTR Filename)
{
	struct IFFHandle *iff;
	LONG Result;
	BOOL IffOpen = FALSE;

	do	{
		static const struct PrefHeader prefHeader = { 1, 0, 0L };

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

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_MainMenuNode, SCMID_MainMenu);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_Disk], SCMID_Popup_Disk);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_Drawer], SCMID_Popup_Drawer);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject], SCMID_Popup_Tool);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan], SCMID_Popup_Trashcan);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_Window], SCMID_Popup_Window);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon], SCMID_Popup_AppIcon);
		if (RETURN_OK != Result)
			break;

		Result = SaveMenuNode(inst,
			iff, &inst->mpb_PopMenuNode[POPMENUINDEX_Desktop], SCMID_Popup_Desktop);
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

	if (RETURN_OK == Result)
		{
		if (inst->mpb_fCreateIcons)
			SaveIcon(inst, Filename);
		}
	else
		{
		char Buffer[120];

		Fault(Result, "", Buffer, sizeof(Buffer) - 1);

		// MUI_RequestA()
		MUI_Request(inst->mpb_Objects[OBJNDX_APP_Main], inst->mpb_Objects[OBJNDX_WIN_Main], 0, NULL, 
			GetLocString(MSGID_ABOUTREQOK), 
			GetLocString(MSGID_REQTITLE_SAVEERROR), 
			Filename,
			Buffer);
		}

	return Result;
}


static LONG SaveMenuNode(struct MenuPrefsInst *inst, struct IFFHandle *iff,
	const struct MenuPrefsPopMenuNode *TreeNode, enum ScalosMenuChunkId ChunkID)
{
	LONG Result;
	struct ScalosMenuChunk *mChunk = NULL;

	do	{
		const struct MUI_NListtree_TreeNode *SubNode;
		size_t StringSpaceSize = 0;
		STRPTR StringSpace;
		UWORD nodeCount;
		size_t Length;
		struct ScalosMenuTree *MenuTreeSpace;

		SubNode = (const struct MUI_NListtree_TreeNode *) DoMethod(TreeNode->mpn_Listtree,
			MUIM_NListtree_GetEntry, TreeNode->mpn_ListNode,
                        MUIV_NListtree_GetEntry_Position_Head,
			0);
		if(NULL == SubNode)
			{
			Result = RETURN_OK;
			break;
			}

		nodeCount = CountMenuEntries(inst, TreeNode->mpn_Listtree, SubNode, &StringSpaceSize);

		Length = sizeof(struct ScalosMenuChunk) + (nodeCount * sizeof(struct ScalosMenuChunk)) + StringSpaceSize;

		mChunk = calloc(1, Length);
		if (NULL == mChunk)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		d1(kprintf("%s/%s/%ld: mChunk=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, mChunk, Length));

		StringSpace = (STRPTR) &mChunk->smch_Menu[nodeCount];

		mChunk->smch_MenuID = SCA_WORD2BE(ChunkID);
		mChunk->smch_Entries = SCA_WORD2BE(nodeCount);

		d1(kprintf("%s/%s/%ld: ChunkID=%ld  Count=%ld  StringSpaceSize=%lu\n", \
			__FILE__, __FUNC__, __LINE__, mChunk->smch_MenuID, mChunk->smch_Entries, StringSpaceSize));

		MenuTreeSpace = mChunk->smch_Menu;

		Result = BuildMenuTree(inst, TreeNode->mpn_Listtree, SubNode, &MenuTreeSpace, &StringSpace, NULL);
		if (RETURN_OK != Result)
			break;

		RemoveAddresses(mChunk->smch_Menu, (UBYTE *) mChunk);

		Result = PushChunk(iff, 0, ID_MENU, IFFSIZE_UNKNOWN);
		if (RETURN_OK != Result)
			break;

		if (Length != WriteChunkBytes(iff, (APTR) mChunk, Length))
			{
			Result = IoErr();
			break;
			}

		Result = PopChunk(iff);
		if (RETURN_OK != Result)
			break;
		} while (0);

	if (mChunk)
		free(mChunk);

	return Result;
}


static UWORD CountMenuEntries(struct MenuPrefsInst *inst, Object *ListTree,
	const struct MUI_NListtree_TreeNode *TreeNode, size_t *StringSpace)
{
	UWORD Count = 0;

	while (TreeNode)
		{
		const struct MUI_NListtree_TreeNode *SubNode;
		const struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;

		d1(kprintf("%s/%s/%ld: TreeNode=%08lx  <%s>\n", \
			__FILE__, __FUNC__, __LINE__, TreeNode, TreeNode->tn_Name));

		Count++;

		*StringSpace += 1 + strlen(mle->llist_name);

		if (strlen(mle->llist_UnselectedIconName) > 0
			|| strlen(mle->llist_SelectedIconName) > 0)
			{
			*StringSpace += 1 + strlen(mle->llist_UnselectedIconName)
				+ 1 + strlen(mle->llist_SelectedIconName);
			}

		SubNode = (const struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
				MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Head,
				0);

		if (SubNode)
			Count += CountMenuEntries(inst, ListTree, SubNode, StringSpace);

		TreeNode = (const struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
			MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);
		}

	return Count;
}


static LONG BuildMenuTree(struct MenuPrefsInst *inst, Object *ListTree,
	const struct MUI_NListtree_TreeNode *TreeNode,
	struct ScalosMenuTree **MenuSpace, STRPTR *StringSpace,
	struct ScalosMenuTree **Parent)
{
	LONG Result = RETURN_OK;

	while (TreeNode)
		{
		struct ScalosMenuTree *mTree = *(struct ScalosMenuTree **) MenuSpace;
		const struct MUI_NListtree_TreeNode *SubNode;
		const struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;
		STRPTR MenuName;

		d1(KPrintF("%s/%s/%ld: TreeNode=%08lx  <%s>  mTree=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, TreeNode, TreeNode->tn_Name ? TreeNode->tn_Name : (STRPTR) "===BAR===", mTree));

		if (Parent)
			*Parent = mTree;

		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->mtre_type = mle->llist_EntryType;
		mTree->mtre_flags = mle->llist_Flags;

		if (strlen(mle->llist_name) > 0)
			{
			MenuName = AddMenuString(mle->llist_name, StringSpace);
			}
		else
			{
			MenuName = NULL;
			}

		*MenuSpace = mTree + 1;

		if (SCAMENUTYPE_Command == mTree->mtre_type)
			{
			mTree->MenuCombo.MenuCommand.mcom_flags = mle->llist_CommandFlags;
			mTree->MenuCombo.MenuCommand.mcom_type = mle->llist_CommandType;
			mTree->MenuCombo.MenuCommand.mcom_stack = mle->llist_Stack;
			mTree->MenuCombo.MenuCommand.mcom_pri = mle->llist_Priority;
			mTree->MenuCombo.MenuCommand.mcom_name = MenuName;
			}
		else
			{
			if (strlen(mle->llist_UnselectedIconName) > 0
				|| strlen(mle->llist_SelectedIconName) > 0)
				{
				STRPTR IconName;

				IconName = AddMenuString(mle->llist_UnselectedIconName, StringSpace);
				AddMenuString(mle->llist_SelectedIconName, StringSpace);
				mTree->MenuCombo.MenuTree.mtre_iconnames = IconName;

				mTree->mtre_flags |= MTREFLGF_IconNames;
				}
			else
				{
				mTree->MenuCombo.MenuTree.mtre_iconnames = NULL;
				mTree->mtre_flags &= ~MTREFLGF_IconNames;
				}

			d1(KPrintF("%s/%s/%ld: mtre_name=<%s>  mtre_flags=%02lx  mtre_iconnames=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuTree.mtre_name, \
				mTree->mtre_flags, mTree->MenuCombo.MenuTree.mtre_iconnames));

			stccpy(mTree->MenuCombo.MenuTree.mtre_hotkey, mle->llist_HotKey, 
				sizeof(mTree->MenuCombo.MenuTree.mtre_hotkey));
			mTree->MenuCombo.MenuTree.mtre_name = MenuName;
			}

		SubNode = (const struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
				MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Head,
				0);

		if (SubNode)
			{
			Result = BuildMenuTree(inst, ListTree, SubNode, MenuSpace, StringSpace, &mTree->mtre_tree);
			if (RETURN_OK != Result)
				break;
			}

		TreeNode = (const struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
			MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);

		if (TreeNode)
			Parent = &mTree->mtre_Next;
		}

	return Result;
}


static STRPTR AddMenuString(CONST_STRPTR MenuString, STRPTR *StringSpace)
{
	STRPTR MenuName = *StringSpace;

	strcpy(MenuName, MenuString);
	*StringSpace = (APTR) (MenuName + 1 + strlen(MenuName));

	return MenuName;
}


static void RemoveAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *BaseAddr)
{
	while (MenuTree)
		{
		struct ScalosMenuTree *MenuTreeNext = MenuTree->mtre_Next;

		if (SCAMENUTYPE_Command == MenuTree->mtre_type)
			{
			if (MenuTree->MenuCombo.MenuCommand.mcom_name)
				{
				MenuTree->MenuCombo.MenuCommand.mcom_name -= (ULONG) BaseAddr;
				MenuTree->MenuCombo.MenuCommand.mcom_name = (APTR) SCA_LONG2BE(MenuTree->MenuCombo.MenuCommand.mcom_name);
				}
			MenuTree->MenuCombo.MenuCommand.mcom_stack = SCA_LONG2BE(MenuTree->MenuCombo.MenuCommand.mcom_stack);
			}
		else
			{
			if (MenuTree->MenuCombo.MenuTree.mtre_name)
				{
				MenuTree->MenuCombo.MenuTree.mtre_name -= (ULONG) BaseAddr;
				MenuTree->MenuCombo.MenuTree.mtre_name = (APTR) SCA_LONG2BE(MenuTree->MenuCombo.MenuTree.mtre_name);
				}

			if ((MenuTree->mtre_flags & MTREFLGF_IconNames) && MenuTree->MenuCombo.MenuTree.mtre_iconnames)
				{
				MenuTree->MenuCombo.MenuTree.mtre_iconnames -= (ULONG) BaseAddr;
				MenuTree->MenuCombo.MenuTree.mtre_iconnames = (APTR) SCA_LONG2BE(MenuTree->MenuCombo.MenuTree.mtre_iconnames);
				}

			d1(KPrintF("%s/%s/%ld: mtre_name=<%s>  mtre_flags=%02lx  mtre_iconnames=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, MenuTree->MenuCombo.MenuTree.mtre_name, \
                                MenuTree->mtre_flags, MenuTree->MenuCombo.MenuTree.mtre_iconnames));
			}
		if (MenuTree->mtre_tree)
			{
			RemoveAddresses(MenuTree->mtre_tree, BaseAddr);
			MenuTree->mtre_tree = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_tree) - (ULONG) BaseAddr);
			MenuTree->mtre_tree = (APTR)SCA_LONG2BE(MenuTree->mtre_tree);
			}
		if (MenuTree->mtre_Next)
			{
			MenuTree->mtre_Next = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_Next) - (ULONG) BaseAddr);
			MenuTree->mtre_Next = (APTR)SCA_LONG2BE(MenuTree->mtre_Next);
			}

		MenuTree = MenuTreeNext;
		}
}


static LONG SaveIcon(struct MenuPrefsInst *inst, CONST_STRPTR IconName)
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

	icon = allocIcon = GetDiskObject("ENV:sys/def_ScaMenuPrefs");
	if (NULL == icon)
		icon = allocIcon = GetDiskObject("ENV:sys/def_pref");
	if (NULL == icon)
		icon = &DefaultIcon;

	if (icon)
		{
		STRPTR oldDefaultTool = icon->do_DefaultTool;

		icon->do_DefaultTool = (STRPTR) inst->mpb_ProgramName;

		PutDiskObject((STRPTR) IconName, icon);

		icon->do_DefaultTool = oldDefaultTool;

		if (allocIcon)
			FreeDiskObject(allocIcon);
		}

	return 0;
}


static void ClearMenuList(struct MenuPrefsInst *inst)
{
	short n;

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);

	DoMethod(inst->mpb_MainMenuNode.mpn_Listtree, MUIM_NListtree_Remove,
		inst->mpb_MainMenuNode.mpn_ListNode, MUIV_NListtree_Remove_TreeNode_All, 0);

	for (n=0; n<Sizeof(inst->mpb_PopMenuNode); n++)
		{
		DoMethod(inst->mpb_PopMenuNode[n].mpn_Listtree, MUIM_NListtree_Remove,
			inst->mpb_PopMenuNode[n].mpn_ListNode,
			MUIV_NListtree_Remove_TreeNode_All, 0);
		}

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);
}


static void AddAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *BaseAddr)
{
	while (MenuTree)
		{
		if (SCAMENUTYPE_Command == MenuTree->mtre_type)
			{
			MenuTree->MenuCombo.MenuCommand.mcom_name = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuCommand.mcom_name);
			if (MenuTree->MenuCombo.MenuCommand.mcom_name)
				MenuTree->MenuCombo.MenuCommand.mcom_name += (ULONG) BaseAddr;
			MenuTree->MenuCombo.MenuCommand.mcom_stack = SCA_BE2LONG(MenuTree->MenuCombo.MenuCommand.mcom_stack);
			}
		else
			{
			MenuTree->MenuCombo.MenuTree.mtre_name = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuTree.mtre_name);
			if (MenuTree->MenuCombo.MenuTree.mtre_name)
				MenuTree->MenuCombo.MenuTree.mtre_name += (ULONG) BaseAddr;

			if (MenuTree->mtre_flags & MTREFLGF_IconNames)
				{
				MenuTree->MenuCombo.MenuTree.mtre_iconnames = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuTree.mtre_iconnames);
				if (MenuTree->MenuCombo.MenuTree.mtre_iconnames)
					{
					MenuTree->MenuCombo.MenuTree.mtre_iconnames += (ULONG) BaseAddr;
					}
					else
					{
					MenuTree->MenuCombo.MenuTree.mtre_iconnames = NULL;
					}
				}
			else
				{
				MenuTree->MenuCombo.MenuTree.mtre_iconnames = NULL;
				}

			d1(KPrintF("%s/%s/%ld: mtre_name=<%s>  mtre_flags=%02lx  mtre_iconnames=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, MenuTree->MenuCombo.MenuTree.mtre_name, \
				MenuTree->mtre_flags, MenuTree->MenuCombo.MenuTree.mtre_iconnames));
			d1(KPrintF("%s/%s/%ld: mtre_iconnames=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, MenuTree->MenuCombo.MenuTree.mtre_iconnames \
				? MenuTree->MenuCombo.MenuTree.mtre_iconnames : (STRPTR) ""));
			}
		MenuTree->mtre_tree = (APTR) SCA_BE2LONG(MenuTree->mtre_tree);
		if (MenuTree->mtre_tree)
			{
			MenuTree->mtre_tree = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_tree) + (ULONG) BaseAddr);
			AddAddresses(MenuTree->mtre_tree, BaseAddr);
			}
		MenuTree->mtre_Next = SCA_BE2LONG(MenuTree->mtre_Next);
		if (MenuTree->mtre_Next)
			{
			MenuTree->mtre_Next = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_Next) + (ULONG) BaseAddr);
			}
		MenuTree = MenuTree->mtre_Next;
		}
}


static void GenerateMenuList(struct MenuPrefsInst *inst, struct ScalosMenuTree *mTree,
	Object *ListTree, struct MUI_NListtree_TreeNode *MenuNode)
{
	while (mTree)
		{
		struct MenuInsertEntry mie;
		struct MUI_NListtree_TreeNode *TreeNode;
		ULONG MenuType = mTree->mtre_type;
		ULONG Flags = 0;
		ULONG pos;

		mie.mie_TreeEntry = mTree;
		mie.mie_MenuFlags = 0;

		d1(KPrintF("%s/%s/%ld: mTree=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, MenuType));

		switch (MenuType)
			{
		case SCAMENUTYPE_Command:
			d1(kprintf("%s/%s/%ld: Command name <%s>  ListNode=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuCommand.mcom_name, MenuNode->tn_Name));

			DoMethod(ListTree,
				MUIM_NListtree_Insert, "", 
				&mie,
				MenuNode, MUIV_NListtree_Insert_PrevNode_Tail, 0);
			break;

		default:
			d1(KPrintF("%s/%s/%ld: Menu name <%s>  ListNode=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuTree.mtre_name, MenuNode->tn_Name));

			if (SCAMENUTYPE_ToolsMenu == mTree->mtre_type)
				mie.mie_MenuFlags |= LLISTFLGF_NotRemovable;

			Flags |= TNF_LIST;

			//.menu
			if (NULL == mTree->MenuCombo.MenuTree.mtre_name || 0 == strlen(mTree->MenuCombo.MenuTree.mtre_name))
				Flags |= TNF_NOSIGN;

			if (SCAMENUTYPE_ToolsMenu != MenuType)
				{
				TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
					MUIM_NListtree_Insert, mTree->MenuCombo.MenuTree.mtre_name ? mTree->MenuCombo.MenuTree.mtre_name : (STRPTR) "", 
					&mie,
					MenuNode, MUIV_NListtree_Insert_PrevNode_Tail, Flags);
				}
			else
				{
				TreeNode = NULL;

				for (pos=0; ; pos++)
					{
					struct MenuListEntry *mle;

					TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
						MUIM_NListtree_GetEntry, NULL, pos, 0);

					if (NULL == TreeNode)
						break;

					mle = (struct MenuListEntry *) TreeNode->tn_User;

					if (SCAMENUTYPE_ToolsMenu == mle->llist_EntryType)
						break;
					}

				if (NULL == TreeNode)
					{
					TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(ListTree,
						MUIM_NListtree_Insert, mTree->MenuCombo.MenuTree.mtre_name ? mTree->MenuCombo.MenuTree.mtre_name : (STRPTR) "", 
						&mie,
						MenuNode, MUIV_NListtree_Insert_PrevNode_Tail, Flags);
					}
				}

			if (mTree->mtre_tree)
				GenerateMenuList(inst, mTree->mtre_tree, ListTree, TreeNode);
			break;
			}

		mTree = mTree->mtre_Next;
		}
}


static SAVEDS(APTR) INTERRUPT ImportTDHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	char FileName[1000];
	BPTR fd;

	if (!RequestTdFile(inst, FileName, sizeof(FileName)))
		return NULL;

	fd = Open(FileName, MODE_OLDFILE);
	if (fd)
		{
		char Line[512];
		struct MUI_NListtree_TreeNode *MenuNode = NULL;
		struct MUI_NListtree_TreeNode *ItemNode = NULL;
		struct MUI_NListtree_TreeNode *SubNode = NULL;
		struct MUI_NListtree_TreeNode *CmdNode = NULL;
		struct MenuInsertEntry mie;
		struct ScalosMenuTree mTree;

		memset(&mTree, 0, sizeof(mTree));
		mie.mie_TreeEntry = &mTree;
		mie.mie_MenuFlags = 0;

		ClearMenuList(inst);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Close, 
			MUIV_NListtree_Close_ListNode_Root, 
			MUIV_NListtree_Close_TreeNode_All, 
			0);

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);
		inst->mpb_QuietFlag = TRUE;

		while (NULL != FGets(fd, Line, sizeof(Line) - 1))
			{
			STRPTR lp;

			StripLF(Line);
			lp = stpblk(Line);

			if (CmpWord("TITLE", lp))
				{
				lp = NextWord(lp);

				CmdNode = SubNode = ItemNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_Menu;

				MenuNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, lp,
					&mie,
					&inst->mpb_MainMenuNode, 
					MenuNode ? MenuNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

				continue;
				}
			if (CmpWord("ITEM", lp))
				{
				lp = NextWord(lp);

				if (NULL == MenuNode)
					break;

				CmdNode = SubNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_MenuItem;

				ItemNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, lp,
					&mie,
					MenuNode, 
					ItemNode ? ItemNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

				continue;
				}
			if (CmpWord("ITEMBAR", lp))
				{
				if (NULL == MenuNode)
					break;

				CmdNode = SubNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_MenuItem;

				ItemNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, "",
					&mie,
					MenuNode, 
					ItemNode ? ItemNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST | TNF_NOSIGN);

				continue;
				}
			if (CmpWord("(CLI)", lp))
				{
				struct MUI_NListtree_TreeNode *ParentNode;
				struct MenuListEntry *mle;
				ULONG StackSize;

				if (SubNode)
					ParentNode = SubNode;
				else
					ParentNode = ItemNode;

				if (NULL == ParentNode)
					break;

				lp = NextWord(lp);

				StackSize = atoi(lp);

				lp = NextWord(lp);

				mTree.mtre_type = SCAMENUTYPE_Command;

				CmdNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, lp, 
					&mie,
					ParentNode, 
					CmdNode ? CmdNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active);

				mle = (struct MenuListEntry *) CmdNode->tn_User;

				mle->llist_Stack = StackSize;
				mle->llist_CommandType = SCAMENUCMDTYPE_AmigaDOS;

				continue;
				}
			if (CmpWord("(WB)", lp))
				{
				struct MUI_NListtree_TreeNode *ParentNode;
				struct MenuListEntry *mle;

				if (SubNode)
					ParentNode = SubNode;
				else
					ParentNode = ItemNode;

				if (NULL == ParentNode)
					break;

				lp = NextWord(lp);

				mTree.mtre_type = SCAMENUTYPE_Command;

				CmdNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, lp, 
					&mie,
					ParentNode, 
					CmdNode ? CmdNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active);

				mle = (struct MenuListEntry *) CmdNode->tn_User;

				mle->llist_CommandType = SCAMENUCMDTYPE_Workbench;
				mle->llist_Stack = DEFAULT_STACKSIZE;

				continue;
				}
			if (CmpWord("HOTKEY", lp))
				{
				struct MUI_NListtree_TreeNode *ParentNode;
				struct MenuListEntry *mle;

				if (SubNode)
					ParentNode = SubNode;
				else
					ParentNode = ItemNode;

				if (NULL == ParentNode)
					break;

				lp = NextWord(lp);

				mle = (struct MenuListEntry *) ParentNode->tn_User;

				stccpy(mle->llist_HotKey, lp, sizeof(mle->llist_HotKey));

				continue;
				}
			if (CmpWord("SUB", lp))
				{
				lp = NextWord(lp);

				if (NULL == ItemNode)
					break;

				CmdNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_Menu;

				SubNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, lp,
					&mie,
					ItemNode, 
					SubNode ? SubNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

				continue;
				}
			}

		Close(fd);

		inst->mpb_QuietFlag = FALSE;
		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],  MUIM_NListtree_Open, 
			MUIV_NListtree_Open_ListNode_Root, 
			MUIV_NListtree_Open_TreeNode_All, 
			0);
		}

	SetChangedFlag(inst, TRUE);

	return NULL;
}


static BOOL RequestTdFile(struct MenuPrefsInst *inst, char *FileName, size_t MaxLen)
{
	struct FileRequester *Req = MUI_AllocAslRequest(ASL_FileRequest, NULL);
	BOOL Result = FALSE;

	if (Req)
		{
		struct Window *win = NULL;

		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		// MUI_AslRequest()
		Result = MUI_AslRequestTags(Req,
			ASLFR_TitleText, GetLocString(MSGID_TITLE_IMPORTTD),
			ASLFR_InitialFile, "ToolsDaemon.menu",
			ASLFR_InitialDrawer, "S:",
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_Flags1, FRF_DOPATTERNS,
			TAG_END);

		if (Result)
			{
			stccpy(FileName, Req->fr_Drawer, MaxLen);
			AddPart(FileName, Req->fr_File, MaxLen);
			}
		}

	return Result;
}


static BOOL RequestParmFile(struct MenuPrefsInst *inst, char *FileName, size_t MaxLen)
{
	struct FileRequester *Req = MUI_AllocAslRequest(ASL_FileRequest, NULL);
	BOOL Result = FALSE;

	if (Req)
		{
		struct Window *win = NULL;

		get(inst->mpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);

		// MUI_AslRequest()
		Result = MUI_AslRequestTags(Req,
			ASLFR_TitleText, GetLocString(MSGID_TITLE_IMPORTP),
			ASLFR_InitialFile, "ParM.cfg",
			ASLFR_InitialDrawer, "",
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_Flags1, FRF_DOPATTERNS,
			TAG_END);

		if (Result)
			{
			stccpy(FileName, Req->fr_Drawer, MaxLen);
			AddPart(FileName, Req->fr_File, MaxLen);
			}
		}

	return Result;
}


static BOOL CmpWord(CONST_STRPTR Cmd, CONST_STRPTR Line)
{
	while (*Line && (tolower(*Cmd) == tolower(*Line)))
		{
		Cmd++;
		Line++;
		}

	return (BOOL) ('\0' == *Cmd && ('\0' == *Line || isspace(*Line)));
}


static void StripLF(STRPTR Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if ('\n' == *Line)
		*Line = '\0';
}


static STRPTR NextWord(STRPTR lp)
{
	if ('"' == *lp)
		{
		lp++;		// skip leading '"'
		while (*lp && '"' != *lp)
			lp++;
		if ('"' == *lp)
			lp++;	// skip trailing '"'
		}
	else
		{
		while (*lp && !isspace(*lp))
			lp++;
		}

	return stpblk(lp);
}


static void CopyWord(STRPTR dest, CONST_STRPTR src, size_t MaxLen)
{
	if ('"' == *src)
		{
		src++;		// skip leading '"'
		while (*src && '"' != *src && MaxLen > 1)
			{
			*dest++ = *src++;
			MaxLen--;
			}
		}
	else
		{
		while (*src && !isspace(*src) && MaxLen > 1)
			{
			*dest++ = *src++;
			MaxLen--;
			}
		}

	*dest = '\0';
}


static SAVEDS(APTR) INTERRUPT ImportPHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	char FileName[1000];
	BPTR fd;

	if (!RequestParmFile(inst, FileName, sizeof(FileName)))
		return NULL;

	fd = Open(FileName, MODE_OLDFILE);
//	fd = Open("ParM.cfg", MODE_OLDFILE);
	if (fd)
		{
		struct MUI_NListtree_TreeNode *MenuNode = NULL;
		struct MUI_NListtree_TreeNode *ItemNode = NULL;
		struct MUI_NListtree_TreeNode *SubNode = NULL;
		struct MUI_NListtree_TreeNode *CmdNode;
		char Line[512];
		struct MenuInsertEntry mie;
		struct ScalosMenuTree mTree;

		memset(&mTree, 0, sizeof(mTree));
		mie.mie_TreeEntry = &mTree;
		mie.mie_MenuFlags = 0;

		ClearMenuList(inst);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Close, 
			MUIV_NListtree_Close_ListNode_Root, 
			MUIV_NListtree_Close_TreeNode_All, 
			0);

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);
		inst->mpb_QuietFlag = TRUE;

		while (NULL != FGets(fd, Line, sizeof(Line) - 1))
			{
			char ItemName[100];
			STRPTR lp;

			StripLF(Line);
			lp = stpblk(Line);

			if ('#' == *lp || 0 == strlen(lp))
				continue;

			if (CmpWord("MENU", lp))
				{
				lp = NextWord(lp);

				CopyWord(ItemName, lp, sizeof(ItemName));

				d1(kprintf("%s/%s/%ld: ITEM <%s> Menu=%08lx Item=%08lx Sub=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, ItemName, MenuNode, ItemNode, SubNode));

				SubNode = ItemNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_Menu;

				MenuNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, ItemName,
					&mie,
					&inst->mpb_MainMenuNode, 
					MenuNode ? MenuNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Head, 
					TNF_OPEN | TNF_LIST);

				continue;
				}
			if (CmpWord("ITEM", lp))
				{
				struct MenuListEntry *mle;
				char HotKey[2];

				lp = NextWord(lp);

				if (NULL == MenuNode)
					break;

				CmdNode = NULL;
				strcpy(HotKey, "");

				if ('{' == *lp)
					{
					HotKey[0] = lp[1];
					lp = NextWord(lp);
					}

				CopyWord(ItemName, lp, sizeof(ItemName));

				d1(kprintf("%s/%s/%ld: ITEM <%s> Menu=%08lx Item=%08lx Sub=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, ItemName, MenuNode, ItemNode, SubNode));

				mTree.mtre_type = SCAMENUTYPE_MenuItem;

				ItemNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, ItemName,
					&mie,
					SubNode ? SubNode : MenuNode, 
					ItemNode ? ItemNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					TNF_OPEN | TNF_LIST);

				mle = (struct MenuListEntry *) ItemNode->tn_User;

				stccpy(mle->llist_HotKey, HotKey, sizeof(mle->llist_HotKey));

				lp = NextWord(stpblk(lp));

				if (CmpWord("WB", lp))
					{
					lp = NextWord(lp);
					mTree.mtre_type = SCAMENUTYPE_Command;

					CmdNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
						MUIM_NListtree_Insert, lp, 
						&mie,
						ItemNode, 
						CmdNode ? CmdNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
						0);

					mle = (struct MenuListEntry *) CmdNode->tn_User;

					mle->llist_CommandType = SCAMENUCMDTYPE_Workbench;
					mle->llist_Stack = DEFAULT_STACKSIZE;
					}
				else if (CmpWord("RUN", lp) || CmpWord("SHELL", lp))
					{
					lp = NextWord(lp);
					mTree.mtre_type = SCAMENUTYPE_Command;

					CmdNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
						MUIM_NListtree_Insert, lp, 
						&mie,
						ItemNode, 
						CmdNode ? CmdNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
						0);

					mle = (struct MenuListEntry *) CmdNode->tn_User;

					mle->llist_Stack = DEFAULT_STACKSIZE;
					mle->llist_CommandType = SCAMENUCMDTYPE_AmigaDOS;
					}

				continue;
				}
			if (CmpWord("BARLABEL", lp))
				{
				if (NULL == MenuNode)
					break;

				SubNode = NULL;
				mTree.mtre_type = SCAMENUTYPE_MenuItem;

				ItemNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, "",
					&mie,
					MenuNode, 
					ItemNode ? ItemNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					TNF_LIST | TNF_NOSIGN);

				continue;
				}
			if (CmpWord("SUBMENU", lp))
				{
				lp = NextWord(lp);

				if (SubNode)
					break;

				CopyWord(ItemName, lp, sizeof(ItemName));

				d1(kprintf("%s/%s/%ld: SUBMENU <%s> Menu=%08lx Item=%08lx Sub=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, ItemName, MenuNode, ItemNode, SubNode));

				mTree.mtre_type = SCAMENUTYPE_Menu;

				SubNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
					MUIM_NListtree_Insert, ItemName,
					&mie,
					MenuNode, 
					ItemNode ? ItemNode : (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail, 
					TNF_OPEN | TNF_LIST);

				continue;
				}
			if (CmpWord("ENDSUBMENU", lp))
				{
				d1(kprintf("%s/%s/%ld: ENDSUBMENU Menu=%08lx Item=%08lx Sub=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, MenuNode, ItemNode, SubNode));

				if (NULL == SubNode)
					break;

				ItemNode = SubNode;
				SubNode = NULL;

				continue;
				}
			}

		Close(fd);

		inst->mpb_QuietFlag = FALSE;

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],  MUIM_NListtree_Open, 
			MUIV_NListtree_Open_ListNode_Root, 
			MUIV_NListtree_Open_TreeNode_All, 
			0);
		}

	SetChangedFlag(inst, TRUE);

	return NULL;
}


//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CollapseSelectedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Close,
				MUIV_NListtree_Close_ListNode_Active,
				MUIV_NListtree_Close_TreeNode_All,
				0);
			}

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tn);
		}

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Close,
		MUIV_NListtree_Close_ListNode_Active,
		tn,
		0);

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ExpandSelectedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Open,
		MUIV_NListtree_Open_ListNode_Active,
		MUIV_NListtree_Open_TreeNode_Active,
		0);

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnChild;

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf("%s/%ld: Head tn=%08lx  <%s>\n", __FUNC__, __LINE__, tnChild, tnChild ? tnChild->tn_Name : (STRPTR) ""));

		if (tnChild)
			{
			set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tnChild);

			DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_Open,
				MUIV_NListtree_Open_ListNode_Active,
				MUIV_NListtree_Open_TreeNode_All,
				0);
			}

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, (ULONG) tn);
		}

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CollapseAllHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Close, 
		MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ExpandAllHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Open, 
		MUIV_NListtree_Open_ListNode_Root, MUIV_NListtree_Open_TreeNode_All, 0);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT CmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct CommandTableEntry *cte = NULL;

	DoMethod(inst->mpb_Objects[OBJNDX_CmdListView], MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &cte);
	if (cte)
		{
		set(inst->mpb_Objects[OBJNDX_StringCmd], MUIA_String_Contents, (ULONG) cte->cte_Command);

		DoMethod(inst->mpb_Objects[OBJNDX_PopObject], MUIM_Popstring_Close, 0);

		ChangeEntry3HookFunc(hook, o, msg);
		}

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT ContextMenuTriggerHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	Object *MenuObj = *((Object **) msg);
	ULONG MenuHookIndex = 0;
	struct Hook *MenuHook = NULL;

	d1(kprintf("%s/%s//%ld: MenuObj=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuObj));
	d1(kprintf("%s/%s//%ld: msg=%08lx *msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, *((ULONG *) msg)));

	get(MenuObj, MUIA_UserData, &MenuHookIndex);

	if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
		MenuHook = &inst->mpb_Hooks[MenuHookIndex];

	d1(kprintf("%s/%s//%ld: MenuHook=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuHook));
	if (MenuHook)
		DoMethod(inst->mpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, 0);

	return NULL;
}


static SAVEDS(APTR) INTERRUPT AppMessageHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct AppMessage *AppMsg = *(struct AppMessage **) msg;
	struct MUI_NListtree_TreeNode *TreeNode = NULL;

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, TRUE);

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		struct MUI_NListtree_TreeNode *ParentNode = (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail;
		LONG n;

		for (n=0; n<AppMsg->am_NumArgs; )
			{
			struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;
			char Buffer[1000];
			STRPTR FileName;

			if (!NameFromLock(AppMsg->am_ArgList[n].wa_Lock, Buffer, sizeof(Buffer)))
				break;
			if (AppMsg->am_ArgList[n].wa_Name && strlen(AppMsg->am_ArgList[n].wa_Name) > 0)
				AddPart(Buffer, AppMsg->am_ArgList[n].wa_Name, sizeof(Buffer));

			FileName = FilePart(Buffer);

			switch (mle->llist_EntryType)
				{
			case SCAMENUTYPE_MainMenu:
				d1(kprintf("%s/%s/%ld: insert at SCAMENUTYPE_MainMenu\n", __FILE__, __FUNC__, __LINE__));
				if (mle->llist_Flags & LLISTFLGF_MayNotHaveChildren)
					{
					AppMessage_Menu(inst, &TreeNode, &ParentNode, &AppMsg->am_ArgList[n], Buffer, FileName);
					n++;
					}
				else
					{
					struct MenuInsertEntry mie;
					struct ScalosMenuTree mTree;

					memset(&mTree, 0, sizeof(mTree));
					mie.mie_TreeEntry = &mTree;
					mie.mie_MenuFlags = 0;
					mTree.mtre_type = SCAMENUTYPE_Menu;

					TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
						MUIM_NListtree_Insert, "DragIn Menu", 
						&mie,
						TreeNode, ParentNode, TNF_OPEN | TNF_LIST);
					}
				break;

			case SCAMENUTYPE_Menu:
			case SCAMENUTYPE_ToolsMenu:
				d1(kprintf("%s/%s/%ld: insert at SCAMENUTYPE_Menu\n", __FILE__, __FUNC__, __LINE__));
				AppMessage_Menu(inst, &TreeNode, &ParentNode, &AppMsg->am_ArgList[n], Buffer, FileName);
				n++;
				break;

			case SCAMENUTYPE_MenuItem:
				d1(kprintf("%s/%s/%ld: insert at SCAMENUTYPE_MenuItem\n", __FILE__, __FUNC__, __LINE__));
				ParentNode = TreeNode;
				TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
					MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Parent, 0);

				AppMessage_Menu(inst, &TreeNode, &ParentNode, &AppMsg->am_ArgList[n], Buffer, FileName);
				n++;
				break;

			case SCAMENUTYPE_Command:
				d1(kprintf("%s/%s/%ld: insert at SCAMENUTYPE_Command\n", __FILE__, __FUNC__, __LINE__));
				TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
					MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Parent, 0);
				break;

			default:
				break;
				}
			}
		}

	set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Quiet, FALSE);

	return 0;
}


static struct MUI_NListtree_TreeNode *AppMessage_Menu(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode **TreeNode, 
	struct MUI_NListtree_TreeNode **ParentNode, const struct WBArg *wbArg, 
	CONST_STRPTR Path, CONST_STRPTR FileName)
{
	struct MUI_NListtree_TreeNode *NewTreeNode;
	struct MenuListEntry *mle;
	struct MenuInsertEntry mie;
	struct ScalosMenuTree mTree;

	memset(&mTree, 0, sizeof(mTree));
	mie.mie_TreeEntry = &mTree;
	mie.mie_MenuFlags = 0;

	d1(kprintf("%s/%s/%ld: listnode=%08lx <%s>  prevtreenode=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, *TreeNode, (*TreeNode)->tn_Name, *ParentNode));

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetChangedFlag(inst, TRUE);

	mTree.mtre_type = SCAMENUTYPE_MenuItem;

	*TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Insert, FileName, 
		&mie,
		*TreeNode, *ParentNode, TNF_OPEN | TNF_LIST);

	d1(kprintf("%s/%s/%ld: listnode=%08lx <%s>  prevtreenode=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, *TreeNode, (*TreeNode)->tn_Name, *ParentNode));

	mTree.mtre_type = SCAMENUTYPE_Command;

	NewTreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert, "", 
		&mie,
		*TreeNode, MUIV_NListtree_Insert_PrevNode_Tail, 0);
	if (NULL == NewTreeNode)
		return NULL;

	mle = (struct MenuListEntry *) NewTreeNode->tn_User;

	stccpy(mle->llist_name, Path, sizeof(mle->llist_name));

	mle->llist_Stack = DEFAULT_STACKSIZE;
	mle->llist_Priority = 0;

	if (FileName && strlen(FileName) > 0)
		mle->llist_CommandType = SCAMENUCMDTYPE_Workbench;
	else
		mle->llist_CommandType = SCAMENUCMDTYPE_IconWindow;

	return NewTreeNode;
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT SettingsChangedHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetChangedFlag(inst, TRUE);

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

	if (IDCMP_REFRESHWINDOW == iMsg->Class)
		{
		DoMethod(inst->mpb_Objects[OBJNDX_APP_Main], MUIM_Application_CheckRefresh);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT HideObsoleteHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	ULONG HideObsolete = 0;

	//TODO
	get(inst->mpb_Objects[OBJNDX_Menu_HideObsolete], MUIA_Menuitem_Checked, &HideObsolete);
	inst->mpb_HideObsoletePopupMenus = HideObsolete;

	d1(KPrintF("%s/%s/%ld: mpb_HideObsoletePopupMenus=%ld\n", __FILE__, __FUNC__, __LINE__, inst->mpb_HideObsoletePopupMenus));

	set(inst->mpb_Objects[OBJNDX_MainListView], MUIA_NListtree_Quiet, TRUE);
	set(inst->mpb_Objects[OBJNDX_HiddenListTree], MUIA_NListtree_Quiet, TRUE);

	if (inst->mpb_HideObsoletePopupMenus)
		{
		// Move obsolete entries from OBJNDX_MainListTree to OBJNDX_HiddenListTree
		MoveMenuTree(inst, OBJNDX_HiddenListTree, OBJNDX_MainListTree, POPMENUINDEX_Disk);
		MoveMenuTree(inst, OBJNDX_HiddenListTree, OBJNDX_MainListTree, POPMENUINDEX_Drawer);
		MoveMenuTree(inst, OBJNDX_HiddenListTree, OBJNDX_MainListTree, POPMENUINDEX_ToolProject);
		MoveMenuTree(inst, OBJNDX_HiddenListTree, OBJNDX_MainListTree, POPMENUINDEX_Trashcan);
		MoveMenuTree(inst, OBJNDX_HiddenListTree, OBJNDX_MainListTree, POPMENUINDEX_AppIcon);
		}
	else
		{
		// Move obsolete entries from OBJNDX_HiddenListTree to OBJNDX_MainListTree
		MoveMenuTree(inst, OBJNDX_MainListTree, OBJNDX_HiddenListTree, POPMENUINDEX_Disk);
		MoveMenuTree(inst, OBJNDX_MainListTree, OBJNDX_HiddenListTree, POPMENUINDEX_Drawer);
		MoveMenuTree(inst, OBJNDX_MainListTree, OBJNDX_HiddenListTree, POPMENUINDEX_ToolProject);
		MoveMenuTree(inst, OBJNDX_MainListTree, OBJNDX_HiddenListTree, POPMENUINDEX_Trashcan);
		MoveMenuTree(inst, OBJNDX_MainListTree, OBJNDX_HiddenListTree, POPMENUINDEX_AppIcon);
		}

	set(inst->mpb_Objects[OBJNDX_MainListView], MUIA_NListtree_Quiet, FALSE);
	set(inst->mpb_Objects[OBJNDX_HiddenListTree], MUIA_NListtree_Quiet, FALSE);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT MenuCopyHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *TreeNode;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (inst->mpb_QuietFlag)
		return;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		DoMethod(inst->mpb_Objects[OBJNDX_ListTreeClipboard], MUIM_NListtree_Clear);

		CopyFileTypesEntry(inst,
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root,
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
			TreeNode,
			OBJNDX_ListTreeClipboard,
			OBJNDX_MainListTree);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT MenuCutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *TreeNode;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (inst->mpb_QuietFlag)
		return;

	TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active, 0);

	if (TreeNode)
		{
		DoMethod(inst->mpb_Objects[OBJNDX_ListTreeClipboard], MUIM_NListtree_Clear);

		CopyFileTypesEntry(inst,
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root,
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
			TreeNode,
			OBJNDX_ListTreeClipboard,
			OBJNDX_MainListTree);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], MUIM_NListtree_Remove,
			MUIV_NListtree_Remove_ListNode_Active,
			MUIV_NListtree_Remove_TreeNode_Active, 0);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT MenuPasteHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct MenuPrefsInst *inst = (struct MenuPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *tn;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry,
		MUIV_NListtree_GetEntry_ListNode_Active,
		MUIV_NListtree_GetEntry_Position_Active,
		0);
	d1(kprintf("%s/%ld: active tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn ? tn->tn_Name : (STRPTR) ""));

	if (tn)
		{
		struct MUI_NListtree_TreeNode *tnParent;
		struct MUI_NListtree_TreeNode *tnShadow;

		tnParent = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry,
			tn,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);

		tnShadow = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_ListTreeClipboard],
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_Insert_ListNode_Root,
			MUIV_NListtree_GetEntry_Position_Head,
			0);

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_Full);

		if (MayPasteBelow(inst, tn, tnShadow))
			{
			CopyFileTypesEntry(inst,
				tnParent, tn,
				tnShadow,
				OBJNDX_MainListTree, OBJNDX_ListTreeClipboard);

//			  EntryHasChanged(inst, tnParent);
			}
		else if (MayPasteOnto(inst, tn, tnShadow))
			{
			CopyFileTypesEntry(inst,
				tn, (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
				tnShadow,
				OBJNDX_MainListTree, OBJNDX_ListTreeClipboard);

//			  EntryHasChanged(inst, tn);
			}

		set(inst->mpb_Objects[OBJNDX_MainListTree], MUIA_NList_Quiet, MUIV_NList_Quiet_None);

		DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_Redraw,
			tn,
			0);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *MoveMenuTree(struct MenuPrefsInst *inst,
	ULONG DestListtreeIndex, ULONG SrcListtreeIndex, ULONG EntryIndex)
{
	struct MUI_NListtree_TreeNode *NewNode = NULL;

	if (inst->mpb_PopMenuNode[EntryIndex].mpn_ListNode &&
		inst->mpb_PopMenuNode[EntryIndex].mpn_Listtree == inst->mpb_Objects[SrcListtreeIndex])
		{
		// Create new entry in dest listtree
		NewNode = CopyMenuEntry(inst,
			inst->mpb_Objects[DestListtreeIndex],
			inst->mpb_Objects[SrcListtreeIndex],
			inst->mpb_PopMenuNode[EntryIndex].mpn_ListNode,
			MUIV_NListtree_Insert_ListNode_Root);
		if (NewNode)
			{
			// remove old entry (w/ subentries) from src listtree
			DoMethod(inst->mpb_PopMenuNode[EntryIndex].mpn_Listtree,
				MUIM_NListtree_Remove,
				MUIV_NListtree_Remove_ListNode_Root,
				inst->mpb_PopMenuNode[EntryIndex].mpn_ListNode,
				0);

			// remember new entry
			inst->mpb_PopMenuNode[EntryIndex].mpn_Listtree = inst->mpb_Objects[DestListtreeIndex];
			inst->mpb_PopMenuNode[EntryIndex].mpn_ListNode = NewNode;
			}
		}

	return NewNode;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *CopyFileTypesEntry(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnToListNode, struct MUI_NListtree_TreeNode *tnToPrevNode,
	struct MUI_NListtree_TreeNode *tnFrom, 	ULONG destList, ULONG srcList)
{
	struct MenuListEntry *mleFrom = (struct MenuListEntry *) tnFrom->tn_User;
	struct MenuListEntry *mleTo;
	struct MUI_NListtree_TreeNode *tnNew;
	struct MUI_NListtree_TreeNode *tnChild;
	struct MenuInsertEntry mie;
	struct ScalosMenuTree mTree;
	STRPTR IconNames = NULL;

	memset(&mTree, 0, sizeof(mTree));
	mie.mie_TreeEntry = &mTree;

	if (NULL == tnFrom)
		return NULL;

	tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[srcList],
		MUIM_NListtree_GetEntry,
		tnFrom,
		MUIV_NListtree_GetEntry_Position_Head,
		0);

	mTree.mtre_type = mleFrom->llist_EntryType;
	mie.mie_MenuFlags = mleFrom->llist_Flags;

	switch (mleFrom->llist_EntryType)
		{
	case SCAMENUTYPE_Menu:
	case SCAMENUTYPE_ToolsMenu:
	case SCAMENUTYPE_MenuItem:
		stccpy(mTree.MenuCombo.MenuTree.mtre_hotkey, mleFrom->llist_HotKey,
			sizeof(mTree.MenuCombo.MenuTree.mtre_hotkey));

		if (strlen(mleFrom->llist_UnselectedIconName) > 0
			|| strlen(mleFrom->llist_SelectedIconName) > 0)
			{
			size_t len = 2 + strlen(mleFrom->llist_UnselectedIconName) + strlen(mleFrom->llist_SelectedIconName);

			IconNames = malloc(len);
			if (IconNames)
				{
				strcpy(IconNames, mleFrom->llist_UnselectedIconName);
				strcpy(IconNames + 1 + strlen(IconNames), mleFrom->llist_SelectedIconName);

				mTree.mtre_flags |= MTREFLGF_IconNames;
				}
			}
		break;
	case SCAMENUTYPE_Command:
		mTree.MenuCombo.MenuCommand.mcom_type = mleFrom->llist_CommandType;
		mTree.MenuCombo.MenuCommand.mcom_flags = mleFrom->llist_CommandFlags;
		mTree.MenuCombo.MenuCommand.mcom_pri = mleFrom->llist_Priority;
		mTree.MenuCombo.MenuCommand.mcom_stack = mleFrom->llist_Stack;
		break;
	default:
		break;
		}

	tnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[destList],
		MUIM_NListtree_Insert,
		tnFrom->tn_Name,
		&mie,
		tnToListNode,
		tnToPrevNode,
		tnFrom->tn_Flags);

	if (IconNames)
		free(IconNames);

	mleTo = (struct MenuListEntry *) tnNew->tn_User;

	*mleTo = *mleFrom;

	// Copy Children
	while (tnChild)
		{
		d1(kprintf("%s/%ld: tnChild=%08lx <%s>\n", __FUNC__, __LINE__, tnChild, tnChild->tn_Name));

		CopyFileTypesEntry(inst,
			tnNew, (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail,
			tnChild, destList, srcList);

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[srcList],
			MUIM_NListtree_GetEntry,
			tnChild,
			MUIV_NListtree_GetEntry_Position_Next,
			0);
		}

	return tnNew;
}

//----------------------------------------------------------------------------

static LONG CopyMenuTree(struct MenuPrefsInst *inst,
	Object *ListTreeDest, Object *ListTreeSrc,
	struct MUI_NListtree_TreeNode *Src, struct MUI_NListtree_TreeNode *DestParent)
{
	LONG Result = RETURN_OK;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	while (Src)
		{
		CopyMenuEntry(inst, ListTreeDest, ListTreeSrc, Src, DestParent);

		Src = (struct MUI_NListtree_TreeNode *) DoMethod(ListTreeSrc,
			MUIM_NListtree_GetEntry,
			Src,
			MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);
		d1(KPrintF("%s/%s/%ld: Src=%08lx\n", __FILE__, __FUNC__, __LINE__, Src));
		}

	d1(KPrintF("%s/%s/%ld: END  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *CopyMenuEntry(struct MenuPrefsInst *inst,
	Object *ListTreeDest, Object *ListTreeSrc,
	struct MUI_NListtree_TreeNode *Src, struct MUI_NListtree_TreeNode *DestParent)
{
	const struct MenuListEntry *mleSrc = (struct MenuListEntry *) Src->tn_User;
	struct MUI_NListtree_TreeNode *NewNode;

	d1(KPrintF("%s/%s/%ld: TreeNode=%08lx  <%s>  DestParent=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, Src, Src->tn_Name ? Src->tn_Name : (STRPTR) "===BAR===", DestParent));

	NewNode	= (struct MUI_NListtree_TreeNode *) DoMethod(ListTreeDest,
		MUIM_NListtree_Insert, Src->tn_Name,
		0,
		DestParent,
                MUIV_NListtree_Insert_PrevNode_Tail,
		Src->tn_Flags);
	if (NewNode)
		{
		struct MenuListEntry *mleDest = (struct MenuListEntry *) NewNode->tn_User;
		struct MUI_NListtree_TreeNode *SubNode;

		// Clone entry
		*mleDest = *mleSrc;

		SubNode = (struct MUI_NListtree_TreeNode *) DoMethod(ListTreeSrc,
				MUIM_NListtree_GetEntry,
				Src,
                                MUIV_NListtree_GetEntry_Position_Head,
				0);

		d1(KPrintF("%s/%s/%ld: SubNode=%08lx\n", __FILE__, __FUNC__, __LINE__, SubNode));
		if (SubNode)
			{
			CopyMenuTree(inst,
                                ListTreeDest,
				ListTreeSrc,
				SubNode,
				NewNode);
			}
		}

	return NewNode;
}

//----------------------------------------------------------------------------

static void EnableCommandArgumentGadgets(struct MenuPrefsInst *inst, const struct MenuListEntry *mle)
{
	switch (mle->llist_CommandType)
		{
	case SCAMENUCMDTYPE_AmigaDOS:
	case SCAMENUCMDTYPE_Workbench:
		set(inst->mpb_Objects[OBJNDX_CheckArgs], MUIA_Disabled, FALSE);
		set(inst->mpb_Objects[OBJNDX_CheckArgs], MUIA_Selected, (mle->llist_CommandFlags & MCOMFLGF_Args));
		set(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Disabled, FALSE);
		set(inst->mpb_Objects[OBJNDX_StringStack], MUIA_Disabled, FALSE);
		break;

	case SCAMENUCMDTYPE_Command:
	case SCAMENUCMDTYPE_IconWindow:
	case SCAMENUCMDTYPE_ARexx:
	case SCAMENUCMDTYPE_Plugin:
		set(inst->mpb_Objects[OBJNDX_CheckArgs], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_CheckArgs], MUIA_Selected, FALSE);
		set(inst->mpb_Objects[OBJNDX_SliderPriority], MUIA_Disabled, TRUE);
		set(inst->mpb_Objects[OBJNDX_StringStack], MUIA_Disabled, TRUE);
		break;
		}
}

//----------------------------------------------------------------------------

static BOOL IsPopupMenu(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode *TreeNode)
{
	struct MUI_NListtree_TreeNode *ParentNode;
	BOOL IsPopupMenu = FALSE;

	// find root of this TreeNode
	ParentNode = TreeNode;
	while (ParentNode)
		{
		if (inst->mpb_MainMenuNode.mpn_ListNode != ParentNode)
			{
			short n;

			for (n = 0; n < Sizeof(inst->mpb_PopMenuNode); n++)
				{
				if (inst->mpb_PopMenuNode[n].mpn_ListNode == ParentNode)
					{
					IsPopupMenu = TRUE;
					break;
					}
				}
			}

		ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, ParentNode, MUIV_NListtree_GetEntry_Position_Parent, 0);

		d1(kprintf("%s/%s/%ld: Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, ParentNode));
		}

	return IsPopupMenu;
}

//----------------------------------------------------------------------------

static enum MenuEntryType GetMenuEntryType(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *TreeNode)
{
	struct MenuListEntry *mle = (struct MenuListEntry *) TreeNode->tn_User;
	struct MUI_NListtree_TreeNode *ParentNode;
	BOOL IsMainMenu = FALSE;
	BOOL IsPopupMenu = FALSE;
	ULONG Level = 0;

	if (NULL == mle)
		return MENUENTRY_Invalid;

	if (SCAMENUTYPE_Command == mle->llist_EntryType)
		{
		// Get parent menu entry for command
		TreeNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_GetEntry, TreeNode, MUIV_NListtree_GetEntry_Position_Parent, 0);

		mle = (struct MenuListEntry *) TreeNode->tn_User;
		}

	// find root of this TreeNode
	ParentNode = TreeNode;
	while (ParentNode)
		{
		if (inst->mpb_MainMenuNode.mpn_ListNode == ParentNode)
			IsMainMenu = TRUE;
		else
			{
			short n;

			for (n = 0; n < Sizeof(inst->mpb_PopMenuNode); n++)
				{
				if (inst->mpb_PopMenuNode[n].mpn_ListNode == ParentNode)
					{
					IsPopupMenu = TRUE;
					break;
					}
				}
			}

		Level++;

		ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree], 
			MUIM_NListtree_GetEntry, ParentNode, MUIV_NListtree_GetEntry_Position_Parent, 0);

		d1(kprintf("%s/%s/%ld: Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, ParentNode));
		}

	if (IsMainMenu)
		{
		Level -= 2;
		// Level 0 = MainMenu
		// Level 1 = Menu entry
		// Level 2 = MenuItem
		// Level 3 = SubItem
		}
	else if (IsPopupMenu)
		{
		Level--;
		// Level 1 = Popup Menu
		// Level 2 = MenuItem
		// Level 3 = SubItem
		}
	else
		return MENUENTRY_Invalid;

	d1(kprintf("%s/%s/%ld: IsMainMenu=%ld IsPopupMenu=%ld Level=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IsMainMenu, IsPopupMenu, Level));

	switch (mle->llist_EntryType)
		{
	case SCAMENUTYPE_Menu:
	case SCAMENUTYPE_MainMenu:
	case SCAMENUTYPE_ToolsMenu:
		Level++;	// Check applies to Menu children
		break;
	default:
		break;
		}

	switch (Level)
		{
	case 1:		// Menu
		return MENUENTRY_Menu;
		break;
	case 2:		// MenuItem
		return MENUENTRY_MenuItem;
		break;
	case 3:		// SubItem
		return MENUENTRY_SubItem;
		break;
	default:	// should never occur
		return MENUENTRY_Invalid;
		break;
		}
}

//----------------------------------------------------------------------------

static ULONG GetMaxCountForEntry(enum MenuEntryType type)
{
	switch (type)
		{
	case MENUENTRY_Menu:
		return MAX_MENU;
		break;
	case MENUENTRY_MenuItem:
		return MAX_MENUITEM;
		break;
	case MENUENTRY_SubItem:
		return MAX_SUBITEM;
		break;
	default:
		return 0;
		break;
		}
}

//----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

static void SwitchPopButton(struct MenuPrefsInst *inst, UBYTE CommandType)
{
	if (SCAMENUCMDTYPE_Command == CommandType)
		{
		set(inst->mpb_Objects[OBJNDX_PopObject], MUIA_ShowMe, TRUE);
		set(inst->mpb_Objects[OBJNDX_Popbutton], MUIA_ShowMe, FALSE);
		}
	else
		{
		set(inst->mpb_Objects[OBJNDX_PopObject], MUIA_ShowMe, FALSE);
		set(inst->mpb_Objects[OBJNDX_Popbutton], MUIA_ShowMe, TRUE);
		}

}

//----------------------------------------------------------------------------

DISPATCHER(myNListTree)
{
	struct MenuPrefsInst *inst = NULL;
	ULONG Result;

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

		d1(kprintf("%s/%s//%ld:  MUIM_ContextMenuChoice  item=%08lx\n", __FILE__, __FUNC__, __LINE__, cmc->item));

		MenuObj = cmc->item;

		d1(kprintf("%s/%s//%ld: MenuObj=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuObj));
		d1(kprintf("%s/%s//%ld: msg=%08lx *msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, *((ULONG *) msg)));

		get(MenuObj, MUIA_UserData, &MenuHookIndex);

		if (MenuHookIndex > 0 && MenuHookIndex < HOOKNDX_MAX)
			MenuHook = &inst->mpb_Hooks[MenuHookIndex];

		d1(kprintf("%s/%s//%ld: MenuHook=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuHook));
		if (MenuHook)
			DoMethod(inst->mpb_Objects[OBJNDX_Group_Main], MUIM_CallHook, MenuHook, 0);

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

		if (inst->mpb_PageIsActive)
			Result = DoSuperMethodA(cl, obj, msg);
		else
			{
			Result = 0;
			tpa->res->entry = tpa->res->column = -1;
			}

		d1(kprintf("%s/%s//%ld: MUIM_NList_TestPos  Result=%08lx  x=%ld  y=%ld  tpr=%08lx  entry=%ld  column=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Result, tpa->x, tpa->y, tpa->res, tpa->res->entry, tpa->res->column));
		}
		break;

	case MUIM_DragDrop:
		get(obj, MUIA_NList_PrivateData, &inst);
		SetChangedFlag(inst, TRUE);
		Result = DoSuperMethodA(cl, obj, msg);
		break;

	case MUIM_NList_DropType:
		{
		struct MUIP_NList_DropType *dr = (struct MUIP_NList_DropType *)msg;
		struct MUI_NListtree_TreeNode *tnTo, *tnFrom;
		struct MUI_NListtree_TestPos_Result res;

		get(obj, MUIA_NList_PrivateData, &inst);

		inst->mpb_MenuTreeMayDrop = FALSE;

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
					inst->mpb_MenuTreeMayDrop = TRUE;
				break;
			case MUIV_NListtree_DropType_Onto:
				if (MayPasteOnto(inst, tnTo, tnFrom))
					inst->mpb_MenuTreeMayDrop = TRUE;
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
		if (!inst->mpb_MenuTreeMayDrop)
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


static Object *CreatePrefsImage(void)
{
#include "MenuPrefsImage.h"
	Object	*img;

	// First try to load datatypes image from THEME: tree
	img = DataTypesImageObject,
		MUIA_ScaDtpic_Name, (ULONG) "THEME:prefs/plugins/menu",
		MUIA_ScaDtpic_FailIfUnavailable, TRUE,
		End; //DataTypesMCCObject

	if (NULL == img)
		img = IMG(MenuPrefs, MENUPREFS);	// use built-in fallback image

	return img;
}


static void InitHooks(struct MenuPrefsInst *inst)
{
	ULONG n;

	for (n=0; n<HOOKNDX_MAX; n++)
		{
		inst->mpb_Hooks[n] = MenuPrefsHooks[n];
		inst->mpb_Hooks[n].h_Data = inst;
		}
}


static void SetChangedFlag(struct MenuPrefsInst *inst, BOOL changed)
{
	if (changed != inst->mpb_Changed)
		{
		set(inst->mpb_Objects[OBJNDX_Lamp_Changed], 
			MUIA_Lamp_Color, changed ? MUIV_Lamp_Color_Ok : MUIV_Lamp_Color_Off);
		inst->mpb_Changed = changed;
		}
}


static void AddDefaultMenuContents(struct MenuPrefsInst *inst)
{
	ULONG n;
	struct MUI_NListtree_TreeNode *LastNode[4];
	Object *Listtree = NULL;
	struct MenuInsertEntry mie;
	struct ScalosMenuTree mTree;

	mie.mie_TreeEntry = &mTree;

	inst->mpb_QuietFlag = TRUE;

	for (n = 0; n < DefaultMenuEntries; n++)
		{
		struct MUI_NListtree_TreeNode *ParentNode;
		CONST_STRPTR MenuName;
		struct MUI_NListtree_TreeNode *NewNode;
		struct MenuListEntry *mle;
		ULONG InsertFlags;

		memset(&mTree, 0, sizeof(mTree));

		switch (DefaultMenu[n].dme_ParentNode)
			{
		case DEFAULTPARENTNODE_MainMenu:
			Listtree = inst->mpb_MainMenuNode.mpn_Listtree;
			ParentNode = inst->mpb_MainMenuNode.mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_MainMenu Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_Disk:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld:DEFAULTPARENTNODE_Popup_Disk Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_Drawer:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_Drawer Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_ToolProject:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_ToolProject Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_Trashcan:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_Trashcan Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_Window:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_Window Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_AppIcon:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_AppIcon Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_Popup_Desktop:
			Listtree = inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_Listtree;
			ParentNode = inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_ListNode;
			d1(KPrintF("%s/%s//%ld: DEFAULTPARENTNODE_Popup_Desktop Listtree=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree));
			break;
		case DEFAULTPARENTNODE_LastInsertedMenu:
		case DEFAULTPARENTNODE_LastInsertedMenuItem:
			ParentNode = LastNode[DefaultMenu[n].dme_Level - 1];
			break;
		case DEFAULTPARENTNODE_LastInserted:
		default:
			ParentNode = (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_LastInserted;
			break;
			}

		InsertFlags = MUIV_NListtree_Insert_Flag_Active;

		if (SCAMENUTYPE_Command == DefaultMenu[n].dme_EntryType)
			{
			MenuName = (CONST_STRPTR) DefaultMenu[n].dme_NameId;
			mTree.MenuCombo.MenuCommand.mcom_name = (STRPTR) MenuName;
			}
		else
			{
			if (DefaultMenu[n].dme_NameId)
				MenuName = GetLocString(DefaultMenu[n].dme_NameId);
			else
				MenuName = "";
			InsertFlags |= TNF_OPEN | TNF_LIST;
			}

		d1(KPrintF("%s/%s//%ld: Listtree=%08lx  ParentNode=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree, ParentNode));

		mTree.mtre_type = DefaultMenu[n].dme_EntryType;
		mie.mie_MenuFlags = 0;

		switch (DefaultMenu[n].dme_EntryType)
			{
		case SCAMENUTYPE_Menu:
		case SCAMENUTYPE_ToolsMenu:
		case SCAMENUTYPE_MenuItem:
			stccpy(mTree.MenuCombo.MenuTree.mtre_hotkey, GetLocString(DefaultMenu[n].dme_MenuShortId), sizeof(mle->llist_HotKey));
			break;
		case SCAMENUTYPE_Command:
			mTree.MenuCombo.MenuCommand.mcom_type = DefaultMenu[n].dme_CmdType;
			mTree.MenuCombo.MenuCommand.mcom_flags = DefaultMenu[n].dme_CmdFlags;
			mTree.MenuCombo.MenuCommand.mcom_pri = DefaultMenu[n].dme_CmdPri;
			mTree.MenuCombo.MenuCommand.mcom_stack = DefaultMenu[n].dme_CmdStackSize;
			break;
		default:
			break;
			}

		DoMethod(Listtree,
			MUIM_NListtree_Insert, MenuName,
			&mie,
			ParentNode,
			MUIV_NListtree_Insert_PrevNode_Tail,
			InsertFlags);

		d1(KPrintF("%s/%s//%ld: Listtree=%08lx  NewNode=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree, NewNode));
		NewNode = (struct MUI_NListtree_TreeNode *) DoMethod(Listtree,
				MUIM_NListtree_GetEntry,
				ParentNode,
				MUIV_NListtree_GetEntry_Position_Tail, 0);
		d1(KPrintF("%s/%s//%ld: Listtree=%08lx  NewNode=%08lx\n", __FILE__, __FUNC__, __LINE__, Listtree, NewNode));
		if (NewNode)
			mle = (struct MenuListEntry *) NewNode->tn_User;
		else
			mle = NULL;

		if (mle)
			{
			switch (DefaultMenu[n].dme_EntryType)
				{
			case SCAMENUTYPE_Menu:
			case SCAMENUTYPE_ToolsMenu:
			case SCAMENUTYPE_MenuItem:
				LastNode[DefaultMenu[n].dme_Level] = NewNode;
				break;
			default:
				break;
				}
			}
		}

	inst->mpb_QuietFlag = FALSE;
}

//-----------------------------------------------------------------

static void ParseToolTypes(struct MenuPrefsInst *inst, struct MUIP_ScalosPrefs_ParseToolTypes *ptt)
{
	STRPTR tt;

	d1(kprintf("%s/%ld: start  ptt=%08lx  tooltypes=%08lx\n", __FUNC__, __LINE__, ptt, ptt->ToolTypes));
	d1(kprintf("%s/%ld: start  tooltypes=%08lx %08lx %08lx\n", __FUNC__, __LINE__, ptt, ptt->ToolTypes[0], ptt->ToolTypes[1], ptt->ToolTypes[2]));

	tt = FindToolType(ptt->ToolTypes, "HIDEOBSOLETE");
	d1(kprintf("%s/%ld: tt=%08lx\n", __FUNC__, __LINE__, tt));
	if (tt)
		{
		d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

		if (MatchToolValue(tt, "YES"))
			inst->mpb_HideObsoletePopupMenus = TRUE;
		else if (MatchToolValue(tt, "NO"))
			inst->mpb_HideObsoletePopupMenus = FALSE;

		d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));
		}

	if (inst->mpb_Objects[OBJNDX_Menu_HideObsolete])
		set(inst->mpb_Objects[OBJNDX_Menu_HideObsolete], MUIA_Menuitem_Checked, inst->mpb_HideObsoletePopupMenus);

	CallHookPkt(&inst->mpb_Hooks[HOOKNDX_HideObsolete],
		inst->mpb_Objects[OBJNDX_APP_Main], 0);

	d1(kprintf("%s/%ld: end\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	ULONG n;

	MajorVersion = PluginBase->pl_LibNode.lib_Version;
	MinorVersion = PluginBase->pl_LibNode.lib_Revision;

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

	d1(kprintf("%s/%s//%ld:   MenuPrefsLibBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (!OpenLibraries())
		return FALSE;

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

#if !defined(__amigaos4__) && !defined(__AROS__)
	if (_STI_240_InitMemFunctions())
		return FALSE;
#endif

	d1(kprintf("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

	MenuPrefsClass = MUI_CreateCustomClass(&PluginBase->pl_LibNode, MUIC_Group,
			NULL, sizeof(struct MenuPrefsInst), DISPATCHER_REF(MenuPrefs));

	d1(kprintf("%s/%s//%ld:  MenuPrefsClass=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuPrefsClass));
	if (NULL == MenuPrefsClass)
		return FALSE;

	for (n=0; n<Sizeof(CommandsTable); n++)
		CommandsArray[n] = &CommandsTable[n];
	CommandsArray[Sizeof(CommandsTable)] = NULL;

	d1(KPrintF("%s/%s//%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (LocaleBase)
		{
		if (NULL == MenuPrefsLocale)
			MenuPrefsLocale = OpenLocale(NULL);

		if (MenuPrefsLocale)
			{
			if (NULL == MenuPrefsCatalog)
				{
				MenuPrefsCatalog = OpenCatalog(MenuPrefsLocale,
					(STRPTR) "Scalos/ScalosMenu.catalog", NULL);
				}
			}
		}

	if (!StaticsTranslated)
		{
		StaticsTranslated = TRUE;

		TranslateStringArray(CmdModeStrings);
		TranslateNewMenu(ContextMenus);
		}

	if (NULL == myNListTreeClass)
		{
		myNListTreeClass = MUI_CreateCustomClass(NULL, MUIC_NListtree,
			NULL, 0, DISPATCHER_REF(myNListTree));
		}

	d1(KPrintF("%s/%s//%ld: myNListTreeClass=%08lx\n", __FILE__, __FUNC__, __LINE__, myNListTreeClass));

	if (NULL == myNListTreeClass)
		return FALSE;	// Failure

	if (NULL == DataTypesImageClass)
		DataTypesImageClass = InitDtpicClass();

	d1(KPrintF("%s/%s//%ld: DataTypesImageClass=%08lx\n", __FILE__, __FUNC__, __LINE__, DataTypesImageClass));

	if (NULL == DataTypesImageClass)
		return FALSE;	// Failure


	d1(KPrintF("%s/%s//%ld: success\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;	// Success
}

//----------------------------------------------------------------------------

static void InsertMenuRootEntries(struct MenuPrefsInst *inst)
{
	struct MenuInsertEntry mie;
	struct ScalosMenuTree mTree;

	memset(&mTree, 0, sizeof(mTree));
	mie.mie_TreeEntry = &mTree;
	mTree.mtre_type = SCAMENUTYPE_MainMenu;

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable;
	inst->mpb_MainMenuNode.mpn_Listtree = inst->mpb_Objects[OBJNDX_MainListTree];
	inst->mpb_MainMenuNode.mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_MainMenuNode.mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_MAINMENUNAME),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		MUIV_NListtree_Insert_Flag_Active | TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_Listtree = GetMenuEntryListtree(inst);
	inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_Disk].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME1),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_Listtree = GetMenuEntryListtree(inst);
	inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_Drawer].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME2),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_Listtree = GetMenuEntryListtree(inst);
	inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_ToolProject].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME3),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_Listtree = GetMenuEntryListtree(inst);
	inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_Trashcan].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME4),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_Listtree = inst->mpb_Objects[OBJNDX_MainListTree];
	inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_Window].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME5),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_Listtree = GetMenuEntryListtree(inst);
	inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_AppIcon].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME6),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);

	mie.mie_MenuFlags = LLISTFLGF_NotRemovable | LLISTFLGF_NameNotSetable | LLISTFLGF_MayNotHaveChildren | LLISTFLGF_PopupMenu;
	inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_Listtree = inst->mpb_Objects[OBJNDX_MainListTree];
	inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_ListNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_PopMenuNode[POPMENUINDEX_Desktop].mpn_Listtree,
		MUIM_NListtree_Insert, GetLocString(MSGID_POPMENUNAME7),
		&mie,
		MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_OPEN | TNF_LIST);
}

//----------------------------------------------------------------------------

static Object *GetMenuEntryListtree(struct MenuPrefsInst *inst)
{
	return inst->mpb_Objects[inst->mpb_HideObsoletePopupMenus ? OBJNDX_HiddenListTree : OBJNDX_MainListTree];
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *GetParentNodeForMenu(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode **PrevNode)
{
	struct MUI_NListtree_TreeNode *ParentNode;
	BOOL Finished = FALSE;

	*PrevNode = (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail;

	ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 0, MUIV_NListtree_GetEntry_Position_Active, 0);

	while (ParentNode && !Finished)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) ParentNode->tn_User;

		switch (mle->llist_EntryType)
			{
		case SCAMENUTYPE_MainMenu:
		case SCAMENUTYPE_Menu:
		case SCAMENUTYPE_ToolsMenu:
			Finished = TRUE;
			break;

		case SCAMENUTYPE_MenuItem:
		case SCAMENUTYPE_Command:
			*PrevNode = ParentNode;
			ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry, ParentNode,
				MUIV_NListtree_GetEntry_Position_Parent, 0);
			break;
			}
		}

	return ParentNode;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *GetParentNodeForMenuItem(struct MenuPrefsInst *inst, struct MUI_NListtree_TreeNode **PrevNode)
{
	struct MUI_NListtree_TreeNode *ParentNode;

	*PrevNode = (struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_PrevNode_Tail;

	ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 0, MUIV_NListtree_GetEntry_Position_Active, 0);

	while (ParentNode)
		{
		struct MenuListEntry *mle = (struct MenuListEntry *) ParentNode->tn_User;

		if ((SCAMENUTYPE_MainMenu == mle->llist_EntryType)
			&& (mle->llist_Flags & LLISTFLGF_MayNotHaveChildren))
			break;

		if (SCAMENUTYPE_Menu == mle->llist_EntryType)
			break;
		if (SCAMENUTYPE_ToolsMenu == mle->llist_EntryType)
			break;

		*PrevNode = ParentNode;
		ParentNode = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, ParentNode,
			MUIV_NListtree_GetEntry_Position_Parent, 0);
		}

	return ParentNode;
}

//----------------------------------------------------------------------------

static BOOL MayPasteOnto(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom)
{
	const struct MenuListEntry *fteFrom = (const struct MenuListEntry *) tnFrom->tn_User;
	const struct MenuListEntry *fteTo = (const struct MenuListEntry *) tnTo->tn_User;
	BOOL MayPaste = FALSE;

	d1(kprintf("%s/%ld: fteTo=%08lx <%s> %ld  fteFrom=%08lx <%s> %ld\n", __FUNC__, __LINE__, \
		fteTo, tnTo->tn_Name, fteTo->llist_EntryType, fteFrom, tnFrom->tn_Name, fteFrom->llist_EntryType));

	if (fteTo->llist_EntryType >= ENTRYTYPE_MAX || fteFrom->llist_EntryType >= ENTRYTYPE_MAX)
		return FALSE;

	d1(kprintf("%s/%ld: MayPaste=%ld\n", __FUNC__, __LINE__, \
		MayPasteIntoMatrix[fteTo->llist_EntryType][fteFrom->llist_EntryType]));

	if (MayPasteIntoMatrix[fteTo->llist_EntryType][fteFrom->llist_EntryType])
		{
		MayPaste = TRUE;

		if ((SCAMENUTYPE_MainMenu == fteTo->llist_EntryType)
			&& (SCAMENUTYPE_Menu != fteFrom->llist_EntryType)
			&& !IsPopupMenu(inst, tnTo))
			{
			// Only menus may be dropped onto Main menu
			MayPaste = FALSE;
			}
		if ( 0 == strlen(tnTo->tn_Name) )
			{
			// Nothing may be dropped onto a separator
			MayPaste = FALSE;
			}
		}

	return MayPaste;
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *MayPasteBelow(struct MenuPrefsInst *inst,
	struct MUI_NListtree_TreeNode *tnTo, struct MUI_NListtree_TreeNode *tnFrom)
{
	const struct MenuListEntry *fteFrom = (const struct MenuListEntry *) tnFrom->tn_User;
	const struct MenuListEntry *fteTo = (const struct MenuListEntry *) tnTo->tn_User;
	struct MUI_NListtree_TreeNode *tn;
	BOOL Result = FALSE;

	d1(KPrintF("%s/%ld: START fteTo=%08lx <%s> %ld  fteFrom=%08lx <%s> %ld\n", __FUNC__, __LINE__, \
		fteTo, tnTo->tn_Name, fteTo->llist_EntryType, fteFrom, tnFrom->tn_Name, fteFrom->llist_EntryType));

	do	{
		if (fteTo->llist_EntryType >= ENTRYTYPE_MAX || fteFrom->llist_EntryType >= ENTRYTYPE_MAX)
			break;

		if (tnTo->tn_Flags & TNF_OPEN)
			{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Head,
				MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

			if (tn)
				break;
			}

		do	{
			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Next,
				MUIV_NListtree_GetEntry_Flag_SameLevel | MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));
			if (tn)
				break;

			tnTo = (struct MUI_NListtree_TreeNode *) DoMethod(inst->mpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry,
				tnTo,
				MUIV_NListtree_GetEntry_Position_Parent,
				MUIV_NListtree_GetEntry_Flag_Visible);
			d1(KPrintF("%s/%ld: tnTo=%08lx\n", __FUNC__, __LINE__, tnTo));

			if (tnTo)
				{
				fteTo = (const struct MenuListEntry *) tnTo->tn_User;

				Result =  MayPasteAfterMatrix[fteTo->llist_EntryType][fteFrom->llist_EntryType];
				}
			} while (tnTo && !Result);

		if (NULL == tnTo)
			break;

		Result =  MayPasteAfterMatrix[fteTo->llist_EntryType][fteFrom->llist_EntryType];
		} while (0);

	d1(KPrintF("%s/%ld: END MayPaste=%ld\n", __FUNC__, __LINE__, Result));

	return Result ? tnTo : NULL;
}

//----------------------------------------------------------------------------

#ifdef __amigaos4__

static char *stpblk(const char *q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}

#elif !defined(__SASC) && !defined(__AROS__)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__)

static char *stpblk(const char *q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}

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
#endif /*__MORPHOS__*/

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
