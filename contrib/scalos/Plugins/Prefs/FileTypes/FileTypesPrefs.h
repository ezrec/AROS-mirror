// FileTypesPrefs.h
// $Date$
// $Revision$


#ifndef SCALOS_FILETYPESPREFS_H
#define	SCALOS_FILETYPESPREFS_H

//----------------------------------------------------------------------------

#include "DefIcons.h"

//----------------------------------------------------------------------------

// Include nesting limit for filetype descriptions
#define	MAX_INCLUDE_NESTING	10

#define	MAX_ATTRNAME		50
#define	MAX_ATTRVALUE		512

#define	MAX_FTE_NAME		100

#define	MAX_FILENAME		512

//----------------------------------------------------------------------------

#define FIBF_HIDDEN		(1 << 7)

//----------------------------------------------------------------------------

// Flags for writing FileTypes
#define	FTWRITEFLAG_NONE		0x00000000
#define	FTWRITEFLAG_ONLY_SAVE_CHANGED	0x00000001
#define	FTWRITEFLAG_CLEAR_CHANGE_FLAG	0x00000002

//----------------------------------------------------------------------------

enum HookIndex
{
	HOOKNDX_Open = 1,
	HOOKNDX_SaveAs,

	HOOKNDX_TreeDisplay,
	HOOKNDX_TreeConstruct,
	HOOKNDX_TreeDestruct,

	HOOKNDX_AttrDisplay,
	HOOKNDX_AttrConstruct,
	HOOKNDX_AttrDestruct,

	HOOKNDX_AddEntryListDisplay,
	HOOKNDX_AddEntryListConstruct,
	HOOKNDX_AddEntryListDestruct,

	HOOKNDX_AddAttrListDisplay,
	HOOKNDX_AddAttrListConstruct,
	HOOKNDX_AddAttrListDestruct,

	HOOKNDX_EditAttributeListDisplay,
	HOOKNDX_EditAttributeListConstruct,
	HOOKNDX_EditAttributeListDestruct,

	HOOKNDX_FileTypesDisplay,
	HOOKNDX_FileTypesConstruct,
	HOOKNDX_FileTypesDestruct,

	HOOKNDX_FileTypesActionDisplay,
	HOOKNDX_FileTypesActionConstruct,
	HOOKNDX_FileTypesActionDestruct,

	HOOKNDX_SelectEntry,
	HOOKNDX_SelectAttribute,
	HOOKNDX_EditAttribute,

	HOOKNDX_About,
	HOOKNDX_ContextMenuTrigger,

	HOOKNDX_Collapse,
	HOOKNDX_Expand,
	HOOKNDX_CollapseAll,
	HOOKNDX_ExpandAll,

	HOOKNDX_CollapseFileTypes,
	HOOKNDX_ExpandFileTypes,
	HOOKNDX_CollapseAllFileTypes,
	HOOKNDX_ExpandAllFileTypes,

	HOOKNDX_Copy,
	HOOKNDX_Cut,
	HOOKNDX_Paste,

	HOOKNDX_CopyAttr,
	HOOKNDX_CutAttr,
	HOOKNDX_PasteAttr,

	HOOKNDX_AddEntry,
	HOOKNDX_AddAttribute,
	HOOKNDX_RemoveEntry,
	HOOKNDX_RemoveAttribute,

	HOOKNDX_ChangeAttribute,

	HOOKNDX_SelectAttributeValue,

	HOOKNDX_EditAttributePopAslFileStartHook,
	HOOKNDX_EditAttributePopAslPathStartHook,

	HOOKNDX_EditAttributeTTFontOpen,
	HOOKNDX_EditAttributeTTFontClose,

	HOOKNDX_ResetToDefaults,
	HOOKNDX_LastSaved,
	HOOKNDX_Restore,

	HOOKNDX_HideEmptyEntries,

	HOOKNDX_SelectFileTypesEntry,
	HOOKNDX_SelectFileTypesActionEntry,

	HOOKNDX_RenameFileType,

	HOOKNDX_AddFileType,
	HOOKNDX_RemoveFileType,

	HOOKNDX_AddFileTypesMethod,
	HOOKNDX_RemoveFileTypeAction,

	HOOKNDX_ChangedFileTypesActionMatchMatch,
	HOOKNDX_ChangedFileTypesActionMatchCase,
	HOOKNDX_ChangedFileTypesActionMatchOffset,
	HOOKNDX_ChangedFileTypesActionSearchSearch,
	HOOKNDX_ChangedFileTypesActionSearchCase,
	HOOKNDX_ChangedFileTypesActionSearchSkipSpaces,
	HOOKNDX_ChangedFileTypesActionFilesize,
	HOOKNDX_ChangedFileTypesActionPattern,
	HOOKNDX_ChangedFileTypesActionProtection,
	HOOKNDX_ChangedFileTypesActionDosDevice,
	HOOKNDX_ChangedFileTypesActionDeviceName,
	HOOKNDX_ChangedFileTypesActionContents,
	HOOKNDX_ChangedFileTypesActionDosType,
	HOOKNDX_ChangedFileTypesActionMinSize,

	HOOKNDX_DragDropSort_FileTypesAction,

	HOOKNDX_ShowFindGroup,
	HOOKNDX_HideFindGroup,
	HOOKNDX_IncrementalFindFileType,
	HOOKNDX_IncrementalFindNextFileType,
	HOOKNDX_IncrementalFindPrevFileType,

	HOOKNDX_Learn_FileType,

	HOOKNDX_CreateNewFileTypesIcon,
	HOOKNDX_EditFileTypesIcon,

	HOOKNDX_AslIntuiMsg,

	HOOKNDX_MAX
};

enum ObjectIndex
{
	OBJNDX_APP_Main,
	OBJNDX_WIN_Main,

	OBJNDX_MainListView,
	OBJNDX_MainListTree,

	OBJNDX_HiddenListView,
	OBJNDX_HiddenListTree,

	OBJNDX_ShadowListView,
	OBJNDX_ShadowListTree,
	OBJNDX_ShadowAttrListView,
	OBJNDX_ShadowAttrList,

	OBJNDX_AttrList,
	OBJNDX_AttrListView,

	OBJNDX_ButtonAddEntry,
	OBJNDX_ButtonRemoveEntry,
	OBJNDX_ButtonAddAttribute,
	OBJNDX_ButtonRemoveAttribute,
	OBJNDX_ButtonEditAttribute,
	OBJNDX_ButtonChangeAttribute,
	OBJNDX_ButtonKeepAttribute,

	OBJNDX_AddEntryPopObject,
	OBJNDX_AddEntryList,
	OBJNDX_AddEntryListView,

	OBJNDX_AddAttrPopObject,
	OBJNDX_AddAttrListView,
	OBJNDX_AddAttrList,

	OBJNDX_ContextMenuFileTypes,
	OBJNDX_ContextMenuFileTypesActions,
	OBJNDX_ContextMenu,
	OBJNDX_ContextMenuAttrList,
	OBJNDX_Menu_Expand,
	OBJNDX_Menu_Collapse,
	OBJNDX_Menu_Copy,
	OBJNDX_Menu_Cut,
	OBJNDX_Menu_Paste,

	OBJNDX_Menu_Find,

	OBJNDX_Menu_CopyAttr,
	OBJNDX_Menu_CutAttr,
	OBJNDX_Menu_PasteAttr,

	OBJNDX_Menu_ExpandFileTypes,
	OBJNDX_Menu_CollapseFileTypes,

	OBJNDX_Menu_HideEmptyEntries,

	OBJNDX_Text_AttributeName,
	OBJNDX_Pop_AttributeSelectValue,
	OBJNDX_Listview_AttributeSelectValue,
	OBJNDX_Group_AttributeSelectValue,
	OBJNDX_List_AttributeSelectValue,
	OBJNDX_String_AttributeValue,
	OBJNDX_Group_AttributeValue,
	OBJNDX_Pop_AttributeSelectAslFile,
	OBJNDX_DtPic_AttributeSelectAslFile,
	OBJNDX_Group_AttributeAslFile,
	OBJNDX_Pop_AttributeSelectAslPath,
	OBJNDX_Group_AttributeAslPath,
	OBJNDX_Group_AttributeAslFont,
	OBJNDX_Pop_AttributeSelectAslFont,
	OBJNDX_AslFont_Sample,
	OBJNDX_Pop_AttributeSelectTTFont,
	OBJNDX_Group_AttributeTTFont,
	OBJNDX_TTFont_Sample,

	OBJNDX_Lamp_Changed,

	OBJNDX_Group_Main,

	// make sure ALL OBJNDX_Group_Filetypes_Actions_** entries are contiguous
	OBJNDX_Group_Filetypes_Actions_Protection,
	OBJNDX_Group_Filetypes_Actions_Pattern,
	OBJNDX_Group_Filetypes_Actions_Filesize,
	OBJNDX_Group_Filetypes_Actions_Search,
	OBJNDX_Group_Filetypes_Actions_Match,
	OBJNDX_Group_Filetypes_Actions_Empty,
	OBJNDX_Group_FileTypes_Action_MinSizeMB,
	OBJNDX_Group_Filetypes_Actions_DosDevice,
	OBJNDX_Group_Filetypes_Actions_DeviceName,
	OBJNDX_Group_Filetypes_Actions_Contents,
	OBJNDX_Group_Filetypes_Actions_DosType,

	OBJNDX_Cycle_Filetypes_Protection_R,
	OBJNDX_Cycle_Filetypes_Protection_W,
	OBJNDX_Cycle_Filetypes_Protection_E,
	OBJNDX_Cycle_Filetypes_Protection_D,
	OBJNDX_Cycle_Filetypes_Protection_S,
	OBJNDX_Cycle_Filetypes_Protection_P,
	OBJNDX_Cycle_Filetypes_Protection_A,
	OBJNDX_Cycle_Filetypes_Protection_H,

	OBJNDX_Text_FileTypes_FileTypesName,
	OBJNDX_IconObjectMCC_FileTypes_Icon,
	OBJNDX_Text_FileTypes_DefIconName,
	OBJNDX_Button_FileTypes_CreateIcon,
	OBJNDX_Button_FileTypes_EditIcon,

	OBJNDX_NListview_FileTypes,
	OBJNDX_NListtree_FileTypes,
	OBJNDX_String_FileTypes_Name,
	OBJNDX_Button_FileTypes_Add,
	OBJNDX_Button_FileTypes_Remove,

	OBJNDX_NListview_FileTypes_Methods,
	OBJNDX_NList_FileTypes_Actions,

	OBJNDX_Popobject_FileTypes_Methods_Add,
	OBJNDX_NListview_FileTypes_Methods_Add,
	OBJNDX_Nlist_FileTypes_Methods_Add,

	OBJNDX_Button_FileTypes_Actions_Remove,
	OBJNDX_Button_FileTypes_Actions_Learn,

	OBJNDX_String_Filetypes_Actions_Match_Offset,
	OBJNDX_Checkmark_FileTypes_Action_Match_Case,
	OBJNDX_String_Filetypes_Actions_Match_Match,

	OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces,
	OBJNDX_Checkmark_FileTypes_Action_Search_Case,
	OBJNDX_String_FileTypes_Action_Search_Search,

	OBJNDX_String_FileTypes_Action_Filesize_Filesize,

	OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize,

	OBJNDX_String_FileTypes_Action_Pattern_Pattern,

	OBJNDX_String_FileTypes_Action_DosDevice_Pattern,

	OBJNDX_String_FileTypes_Action_DeviceName_Pattern,

	OBJNDX_String_FileTypes_Action_Contents_Pattern,

	OBJNDX_String_FileTypes_Action_DosType_Pattern,

	OBJNDX_Group_FindFiletype,
	OBJNDX_Button_HideFind,
	OBJNDX_String_FindFileType,
	OBJNDX_Button_FindNextFileType,
	OBJNDX_Button_FindPrevFileType,
	OBJNDX_Button_FindHitCount,

	OBJNDX_WIN_EditAttribute,

	OBJNDX_MAX
};

enum FileTypeDefSections
	{
	FTSECTION_All,
	FTSECTION_PopupMenu,
	FTSECTION_Tooltip,
	};

struct FileTypeFileDesc
	{
	ULONG ftfd_LineNumber;
	BPTR ftfd_FileHandle;
	};

enum ftAttributeType
	{
	ATTRTYPE_FtDescription = 1,	// char[]
	ATTRTYPE_PvPluginName,		// char[]

	ATTRTYPE_GroupOrientation,	// enum TTLayoutMode

	ATTRTYPE_MemberHideString,	// char[]

	ATTRTYPE_StringHAlign,		// char[]
	ATTRTYPE_StringVAlign,		// char[]
	ATTRTYPE_StringStyle,		// char[]
	ATTRTYPE_StringFont,		// char[]
	ATTRTYPE_StringTTFont,		// char[]
	ATTRTYPE_StringPen,		// char[]
	ATTRTYPE_StringId,		// char[]
	ATTRTYPE_StringText,		// char[]
	ATTRTYPE_StringSrc,		// char[]

	ATTRTYPE_SpaceSize,		// ULONG

	ATTRTYPE_DtImageName,		// char[]
	ATTRTYPE_UnselIconName,		// char[]
	ATTRTYPE_SelIconName,		// char[]

	ATTRTYPE_MenuItemName,		// char[]
	ATTRTYPE_MenuCommKey,		// char[]
	ATTRTYPE_MenuDefaultAction,	// ./.

	ATTRTYPE_CommandName,		// char[]
	ATTRTYPE_CommandStacksize,	// ULONG
	ATTRTYPE_CommandPriority,	// ULONG
	ATTRTYPE_CommandWbArgs,		// ./.
	};

struct FtAttribute
	{
	struct Node fta_Node;

	enum ftAttributeType fta_Type;
	ULONG fta_Value2;
	ULONG fta_Length;
	BYTE fta_Data[1];
	};

struct AttrListEntry
	{
	char ale_ValueString[MAX_ATTRVALUE];
	struct FtAttribute ale_Attribute;
	};


enum FtEntryType
	{
	ENTRYTYPE_FileType,

	ENTRYTYPE_PopupMenu,
	ENTRYTYPE_PopupMenu_SubMenu,
	ENTRYTYPE_PopupMenu_MenuItem,
	ENTRYTYPE_PopupMenu_InternalCmd,
	ENTRYTYPE_PopupMenu_WbCmd,
	ENTRYTYPE_PopupMenu_ARexxCmd,
	ENTRYTYPE_PopupMenu_CliCmd,
	ENTRYTYPE_PopupMenu_PluginCmd,
	ENTRYTYPE_PopupMenu_IconWindowCmd,
	ENTRYTYPE_PopupMenu_MenuSeparator,

	ENTRYTYPE_ToolTip,
	ENTRYTYPE_ToolTip_Group,
	ENTRYTYPE_ToolTip_Member,
	ENTRYTYPE_ToolTip_HBar,
	ENTRYTYPE_ToolTip_String,
	ENTRYTYPE_ToolTip_Space,
	ENTRYTYPE_ToolTip_DtImage,

	// MUST be last entry !!!
	ENTRYTYPE_MAX
	};

enum TTLayoutMode 
	{ 
	TTL_Horizontal, 
	TTL_Vertical 
	};

enum TTItemTypes
	{
	TTITYPE_Tooltip,
	TTITYPE_Group,
	TTITYPE_Member,
	};

struct FileTypesToolTipEntry
	{
	struct MUI_NListtree_TreeNode *ftti_Parent;
	};

struct FileTypesActionListEntry
	{
	struct Node ftale_Node;
	struct Magic ftale_Magic;
	STRPTR ftale_AllocatedString;
	};

struct FileTypesEntry
	{
	ULONG fte_FindStart;		// start position of find marker (0=name start)
	ULONG fte_FindLength;		// length of find marker (0=no marker)

	STRPTR fte_AllocatedName2;	// allocated space for find marker in tn_Name
	STRPTR fte_AllocatedName;	// allocated space for tn_Name
	struct TypeNode fte_TypeNode;

	struct List fte_MagicList;	// List of FileTypesActionListEntry
	};

struct FileTypesActionEntry
	{
	struct FileTypesActionListEntry *fae_ftale;
	char fae_ActionAttrs[80];
	};


struct FileTypesListEntry
	{
	char ftle_NameBuffer[MAX_FTE_NAME + MAX_ATTRVALUE];

	enum FtEntryType ftle_EntryType; // Type of this node

	BOOL ftle_FileFound;
	BOOL ftle_Changed;

	ULONG fte_ImageIndex;
	Object *fte_ImageObject;

	struct List ftle_AttributesList;

	union 
		{
		struct FileTypesToolTipEntry ftle_ToolTipEntry;
		} ftle_TypeEntry;
	};

struct FileTypesPrefsInst
{
	ULONG	fpb_fCreateIcons;
	ULONG	fpb_PageIsActive;
	BOOL	fpb_Changed;

	BPTR	fpb_DefIconsSegList;
	struct TypeNode *fpb_RootType;
	BOOL   fpb_DefIconsInit;

	struct Screen *fpb_WBScreen;

	CONST_STRPTR	fpb_ProgramName;

	struct	Hook fpb_Hooks[HOOKNDX_MAX];
	Object 	*fpb_Objects[OBJNDX_MAX];

	char fpb_FileName[MAX_FILENAME];
	char fpb_Path[MAX_FILENAME];

	char fpb_DefIconPath[MAX_FILENAME];

	char fpb_FindHitCount[80];

	struct FileTypeFileDesc fpb_FileHandles[MAX_INCLUDE_NESTING];
	ULONG fpb_IncludeNesting;	// index into fpb_FileHandles[]

	enum FileTypeDefSections fpb_RequestedSection;

	ULONG fpb_SaveFlags;

	BOOL fpb_HideEmptyNodes;
	BOOL fpb_DefaultActionSet;

	BYTE fpb_TTfAntialias;
	WORD fpb_TTfGamma;

	BOOL fpb_MenuTreeMayDrop;

	BPTR fpb_FileTypesDirLock;

	Object *fpb_IconObject;

	ULONG fpb_MenuImageIndex;

	struct List fpb_FoundList;
	struct FoundNode *fpb_CurrentFound;
	ULONG fpb_FoundCount;		// number of entries in fpb_FoundList

	struct MUI_NListtree_TreeNode *fpb_PopupMenu;
	struct MUI_NListtree_TreeNode *fpb_CurrentTTItem;
	struct MUI_NListtree_TreeNode *fpb_CurrentMenuItem;
	struct MUI_NListtree_TreeNode *fpb_ParentMenuItem;

	struct FileRequester *fpb_LearnReq;
	struct FileRequester *fpb_LoadReq;
	struct FileRequester *fpb_SaveReq;

	Object *fpb_SubWindows[2];
};


#define	Sizeof(array)	(sizeof(array) / sizeof((array)[0]))

//----------------------------------------------------------------------------

struct ScalosFileTypes_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

//----------------------------------------------------------------------------

#endif /* SCALOS_FILETYPESPREFS_H */
