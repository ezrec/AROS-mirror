// FileTypes.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <libraries/pm.h>
#include <workbench/workbench.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif

#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DefIcons.h"
#include "TTLayout.h"
#include <scalos/menu.h>
#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "Variables.h"

#include "MsgIdNames.h"
#include "PenNames.h"
#include "DtImageClass.h"

//----------------------------------------------------------------------------

// local data definitions

//#define	SHOWTAGLIST


// directory for filetype descriptors
#define	FILETYPES_DIR1	"ENV:Scalos/FileTypes/"
// (obsolete)
#define	FILETYPES_DIR2	"Scalos:FileTypes/"

enum FileTypeDefSections
	{
	FTSECTION_All,
	FTSECTION_Globals,
	FTSECTION_PopupMenu,
	FTSECTION_Tooltip,
	};

enum TTItemTypes
	{
	TTITYPE_Tooltip,
	TTITYPE_Group,
	TTITYPE_Member,
	};

struct FileTypeTTItem
	{
	struct Node ftti_Node;

	struct FileTypeTTItem *ftti_Parent;

	STRPTR ftti_HideHookString;

	enum TTItemTypes ftti_Type;

	struct ScalosTagList ftti_TagList;
	};

struct FileTypeMenuItem
	{
	struct Node ftmi_Node;
	struct List ftmi_SubItemList;

	struct FileTypeMenuItem *ftmi_Parent;

	STRPTR ftmi_MenuName;
	STRPTR ftmi_MenuCommKey;

	Object *ftmi_MenuIconSelected;
	Object *ftmi_MenuIconUnselected;

	struct ScalosTagList ftmi_TagList;

	struct ScalosMenuTree *ftmi_MenuTree;

	BOOL ftmi_DefaultAction;
	};

struct CmdFunc
	{
	CONST_STRPTR cf_CommandName;	// Name of the command.
	CONST_STRPTR cf_Template;	// The command template in standard AmigaDOS syntax.

	enum FileTypeDefSections cf_validForSection;

	// Pointer to the function which implements this command.
	LONG (*cf_Function)(struct FileTypeDef *, LONG *);
	};

//----------------------------------------------------------------------------

// local functions

static LONG FtBeginToolTip(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtEndToolTip(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtHBar(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtBeginGroup(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtEndGroup(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtBeginMember(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtEndMember(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtString(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtSpace(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtImage(struct FileTypeDef *ftd, LONG *ArgArray);

static LONG FtBeginPopupMenu(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtEndPopupMenu(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtIncludeFile(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG GetFtDescription(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG GetFtPvPlugin(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG AddFtMenuSeparator(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG NewFtMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG EndFtMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG AddSubMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG EndSubMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtInternalCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtWbCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtARexxCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtCliCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtPluginCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static LONG FtIconWindowCmd(struct FileTypeDef *ftd, LONG *ArgArray);
static const struct CmdFunc *ParseFileTypeLine(CONST_STRPTR *Line);
static struct ttDef *ReadFileTypeGlobals(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName);
static struct ttDef *ReadFileTypeToolTipDef(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName);
static struct PopupMenu *ReadFileTypePopupMenu(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName);
static LONG ReadFileTypeDef(struct FileTypeDef *ftd, CONST_STRPTR fileName);
static struct FileTypeDef *CreateFileTypeDef(void);
static void DisposeFileTypeDef(struct FileTypeDef *ftd);
static void DisposeFileTypeDefTemp(struct FileTypeDef *ftd);
static struct FileTypeMenuItem *NewFileTypeMenuItem(void);
static void DisposeFileTypeMenuItem(const struct FileTypeDef *ParentFtd, struct FileTypeMenuItem *ftmi);
static struct FileTypeTTItem *NewFileTypeTTItem(enum TTItemTypes type);
static void DisposeFileTypeTTItem(struct FileTypeTTItem *ftti);
static void FreeMenuTree(const struct FileTypeDef *ParentFtd, struct ScalosMenuTree *mTree);
static struct FileTypeDef *GetFileTypeDefForName(CONST_STRPTR Name);
static void AddFileTypeDef(struct FileTypeDef *ftd, CONST_STRPTR Name);
static void FileTypeFlushObsolete(void);
struct FileTypeFileInfo *NewFileTypeFileInfo(struct FileTypeDef *ftd, BPTR fh, CONST_STRPTR Name);
static void DisposeFileTypeFileInfo(struct FileTypeFileInfo *ffi);
static struct FileTypeDef *ReadFileTypeDefForName(CONST_STRPTR Name);
static void InvalidateFileType(struct FileTypeDef *ftd);
static void FileTypeDefChanged(struct internalScaWindowTask *iwt, struct NotifyMessage *msg);

LONG ConvertMsgNameToID(CONST_STRPTR MsgName);
LONG ConvertPenNameToPen(CONST_STRPTR PenName);

#ifdef SHOWTAGLIST
static void ShowTagList(struct TagItem *tagList);
#endif /* SHOWTAGLIST */

//----------------------------------------------------------------------------

// public data items :
SCALOSSEMAPHORE FileTypeListSema;

//----------------------------------------------------------------------------

// local data items :

struct CmdFunc CommandTable[] =
{
	{ "INCLUDE",		"NAME/A",						FTSECTION_All,		FtIncludeFile },

	{ "DESCRIPTION",	"NAME/A",						FTSECTION_Globals,	GetFtDescription },

	{ "PREVIEWPLUGIN",	"NAME/A",						FTSECTION_Globals,	GetFtPvPlugin },

	{ "POPUPMENU",		"",							FTSECTION_PopupMenu,	FtBeginPopupMenu },
	{ "ENDPOPUPMENU",	"",							FTSECTION_PopupMenu,	FtEndPopupMenu },

	{ "SUBMENU",		"NAME/A,SELECTEDICON/K,UNSELECTEDICON/K",		FTSECTION_PopupMenu,	AddSubMenuEntry },
	{ "ENDSUBMENU",		"",							FTSECTION_PopupMenu,	EndSubMenuEntry },

	{ "MENUENTRY",		"NAME/A,KEY/K,DEFAULTACTION/S,SELECTEDICON/K,UNSELECTEDICON/K",	FTSECTION_PopupMenu,	NewFtMenuEntry },
	{ "ENDMENUENTRY",	"",							FTSECTION_PopupMenu,	EndFtMenuEntry },

	{ "INTERNALCMD",	"NAME/A",						FTSECTION_PopupMenu,	FtInternalCmd },
	{ "WBCMD",		"NAME/A,STACK/K/N,WBARGS/S,PRI=PRIORITY/K/N",		FTSECTION_PopupMenu,	FtWbCmd },
	{ "AREXXCMD",		"NAME/A,STACK/K/N,WBARGS/S,P=PRIORITY/K/N",		FTSECTION_PopupMenu,	FtARexxCmd },
	{ "CLICMD",		"NAME/A,STACK/K/N,WBARGS/S,P=PRIORITY/K/N",		FTSECTION_PopupMenu,	FtCliCmd },
	{ "ICONWINDOWCMD",	"NAME/A",						FTSECTION_PopupMenu,	FtIconWindowCmd },
	{ "PLUGINCMD",	 	"NAME/A",						FTSECTION_PopupMenu,	FtPluginCmd },

	{ "MENUSEPARATOR",	"",							FTSECTION_PopupMenu,	AddFtMenuSeparator },

	{ "TOOLTIP",		"",							FTSECTION_Tooltip,	FtBeginToolTip },
	{ "ENDTOOLTIP",		"",							FTSECTION_Tooltip,	FtEndToolTip },

	{ "GROUP",		"ORIENTATION/A/K",					FTSECTION_Tooltip,	FtBeginGroup },
	{ "ENDGROUP",		"",							FTSECTION_Tooltip,	FtEndGroup },

	{ "MEMBER",		"HIDE/K",						FTSECTION_Tooltip,	FtBeginMember },
	{ "ENDMEMBER",		"",							FTSECTION_Tooltip,	FtEndMember },

	{ "HBAR",		"",							FTSECTION_Tooltip,	FtHBar },
	{ "STRING",		"ID/K,TEXT/K,SRC/K,TEXTPEN/K,HALIGN/K,STYLE/K,FONT/K,TTFONT/K,VALIGN/K",
											FTSECTION_Tooltip,	FtString },
	{ "SPACE",		"SIZE/N/A",						FTSECTION_Tooltip,	FtSpace },
	{ "DTIMAGE",		"FILENAME/A",						FTSECTION_Tooltip,	FtImage },

	{ NULL,			NULL,							FTSECTION_All,		NULL },
};

static ULONG Default_Priority = 0;	// default priority for running WB/CLI/ARexx programs

static BOOL FilesTypesInitialized = FALSE;

static struct List FileTypeList;
static struct List ObsoleteFileTypeList;

//----------------------------------------------------------------------------

BOOL FileTypeInit(void)
{
	d1(kprintf("%s/%s/%ld: FilesTypesInitialized=%ld\n", __FILE__, __FUNC__, __LINE__, FilesTypesInitialized));

	if (!FilesTypesInitialized)
		{
		NewList(&FileTypeList);
		NewList(&ObsoleteFileTypeList);

		ScalosInitSemaphore(&FileTypeListSema);

		FilesTypesInitialized = TRUE;
		}

	return TRUE;
}

void FileTypeCleanup(void)
{
	d1(KPrintF("%s/%s/%ld: FilesTypesInitialized=%ld\n", __FILE__, __FUNC__, __LINE__, FilesTypesInitialized));

	if (FilesTypesInitialized)
		{
		FileTypeFlush(TRUE);
		FilesTypesInitialized = FALSE;
		}
}


// FileTypes directory has changed
void NewFileTypesPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: START smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_FILETYPESPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_FILETYPESPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


BOOL ChangedFileTypesPrefs(struct MainTask *mt)
{
	(void) mt;

	return FALSE;
}

void FileTypeFlush(BOOL Final)
{
	struct FileTypeDef *ftd;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&FileTypeListSema);

	while ((ftd = (struct FileTypeDef *) RemTail(&FileTypeList)))
		{
		if (ScalosAttemptSemaphore(&ftd->ftd_Sema))
			{
			d1(KPrintF("%s/%s/%ld: delete ftd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));
			Forbid();
			ScalosReleaseSemaphore(&ftd->ftd_Sema);
			DisposeFileTypeDef(ftd);
			Permit();
			}
		else if (Final)
			{
			// Final program cleanup
			DisposeFileTypeDef(ftd);
			}
		else
			{
			// cannot free ATM, move to ObsoleteFileTypeList
			d1(KPrintF("%s/%s/%ld: move to ObsoleteFileTypeList ftd=%08lx <%s>\n", \
				__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));
			Remove(&ftd->ftd_Node);
			AddTail(&ObsoleteFileTypeList, &ftd->ftd_Node);
			}
		}

	ScalosReleaseSemaphore(&FileTypeListSema);

	FileTypeFlushObsolete();
}

//----------------------------------------------------------------------------

static LONG FtBeginToolTip(struct FileTypeDef *ftd, LONG *ArgArray)
{
	(void) ArgArray;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (ftd->ftd_CurrentTTItem)
		{
		d1(kprintf("%s/%s/%ld: RETURN_ERROR TOOLTIP inside TOOLTIP\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	ftd->ftd_CurrentTTItem = NewFileTypeTTItem(TTITYPE_Tooltip);
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));
	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	return RETURN_OK;
}


static LONG FtEndToolTip(struct FileTypeDef *ftd, LONG *ArgArray)
{
	(void) ArgArray;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Tooltip:
		break;
	case TTITYPE_Member:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: ENDTOOLTIP inside non-TOOLTIP\n", __FILE__, __FUNC__, __LINE__));
		DisposeFileTypeTTItem(ftd->ftd_CurrentTTItem);
		ftd->ftd_CurrentTTItem = NULL;
		return RETURN_ERROR;
		}

	ScalosTagListEnd(&ftd->ftd_CurrentTTItem->ftti_TagList);

#ifdef SHOWTAGLIST
	ShowTagList(ftd->ftd_CurrentTTItem->ftti_TagList.stl_TagList);
#endif /* SHOWTAGLIST */
	ftd->ftd_ToolTipDef = TT_CreateItemA(ftd->ftd_CurrentTTItem->ftti_TagList.stl_TagList);

	DisposeFileTypeTTItem(ftd->ftd_CurrentTTItem);
	ftd->ftd_CurrentTTItem = NULL;

	if (NULL == ftd->ftd_ToolTipDef)
		return RETURN_ERROR;

	return RETURN_OK;
}


static LONG FtHBar(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct ttDef *barTTd;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Member:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: HBAR outside MEMBER\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	barTTd = TT_CreateItem(TT_WideTitleBar, NULL, 
		ftd->ftd_CurrentTTItem->ftti_HideHookString ? TT_HiddenHook : TAG_IGNORE, ftd->ftd_CurrentTTItem->ftti_HideHookString,
		TAG_END);

	d1(kprintf("%s/%s/%ld: barTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, barTTd));

	if (NULL == barTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, 
			TT_Item, (ULONG) barTTd))
		{
		TTDisposeItem(barTTd);
		return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtBeginGroup(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeTTItem *groupTTi;
	enum TTLayoutMode orientation = TTL_Vertical;
	STRPTR orientationString = (STRPTR) ArgArray[0];

	d1(kprintf("%s/%s/%ld: orientation=<%s>\n", __FILE__, __FUNC__, __LINE__, orientationString));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	if (orientationString && strlen(orientationString) > 0)
		{
		if (0 == Stricmp(orientationString, "horizontal"))
			orientation = TTL_Horizontal;
		else if (0 == Stricmp(orientationString, "vertical"))
			orientation = TTL_Vertical;
		else
			return RETURN_ERROR;
		}

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, TT_LayoutMode, orientation))
		return RETURN_ERROR;

	groupTTi = NewFileTypeTTItem(TTITYPE_Group);
	d1(kprintf("%s/%s/%ld: groupTTi=%08lx\n", __FILE__, __FUNC__, __LINE__, groupTTi));

	if (NULL == groupTTi)
		return RETURN_ERROR;

	groupTTi->ftti_Parent = ftd->ftd_CurrentTTItem;
	ftd->ftd_CurrentTTItem = groupTTi;
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));

	return RETURN_OK;
}


static LONG FtEndGroup(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeTTItem *groupTTi;
	struct ttDef *groupTTd;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Group:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Member:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: ENDGROUP outside GROUP\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	groupTTi = ftd->ftd_CurrentTTItem;
	if (NULL == groupTTi->ftti_Parent)
		return RETURN_ERROR;

	ScalosTagListEnd(&groupTTi->ftti_TagList);

#ifdef SHOWTAGLIST
	ShowTagList(groupTTi->ftti_TagList.stl_TagList);
#endif /* SHOWTAGLIST */
	ftd->ftd_CurrentTTItem = groupTTi->ftti_Parent;
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));

	groupTTd = TT_CreateItemA(groupTTi->ftti_TagList.stl_TagList);
	d1(kprintf("%s/%s/%ld: groupTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, groupTTd));

	DisposeFileTypeTTItem(groupTTi);

	if (NULL == groupTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, TT_Members, (ULONG) groupTTd))
		{
		TTDisposeItem(groupTTd);
		return RETURN_ERROR;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

#ifdef SHOWTAGLIST
	ShowTagList(ftd->ftd_CurrentTTItem->ftti_TagList.stl_TagList);
#endif /* SHOWTAGLIST */

	return RETURN_OK;
}


static LONG FtBeginMember(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeTTItem *memberTTi;
	STRPTR hideString = (STRPTR) ArgArray[0];

	d1(kprintf("%s/%s/%ld: hide=<%s>\n", __FILE__, __FUNC__, __LINE__, hideString));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Group:
		break;
	case TTITYPE_Member:
	case TTITYPE_Tooltip:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: MEMBER not inside GROUP\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	memberTTi = NewFileTypeTTItem(TTITYPE_Member);
	d1(kprintf("%s/%s/%ld: memberTTi=%08lx\n", __FILE__, __FUNC__, __LINE__, memberTTi));

	if (NULL == memberTTi)
		return RETURN_ERROR;

	memberTTi->ftti_Parent = ftd->ftd_CurrentTTItem;
	ftd->ftd_CurrentTTItem = memberTTi;
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));

	if (hideString && strlen(hideString) > 0)
		{
		memberTTi->ftti_HideHookString = ScalosAlloc(1 + strlen(hideString));

		if (NULL == memberTTi->ftti_HideHookString)
			{
			DisposeFileTypeTTItem(memberTTi);
			return RETURN_ERROR;
			}

		strcpy(memberTTi->ftti_HideHookString, hideString);
		}

	return RETURN_OK;
}


static LONG FtEndMember(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeTTItem *memberTTi;
	struct ttDef *memberTTd;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Member:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR:  ENDMEMBER outside MEMBER\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	memberTTi = ftd->ftd_CurrentTTItem;
	if (NULL == memberTTi->ftti_Parent)
		return RETURN_ERROR;

	ScalosTagListEnd(&memberTTi->ftti_TagList);

	ftd->ftd_CurrentTTItem = memberTTi->ftti_Parent;
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));

#ifdef SHOWTAGLIST
	ShowTagList(memberTTi->ftti_TagList.stl_TagList);
#endif /* SHOWTAGLIST */

	memberTTd = TT_CreateItemA(memberTTi->ftti_TagList.stl_TagList);
	d1(kprintf("%s/%s/%ld: memberTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, memberTTd));

	DisposeFileTypeTTItem(memberTTi);

	if (NULL == memberTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, TT_Item, (ULONG) memberTTd))
		{
		TTDisposeItem(memberTTd);
		return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtString(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct ttDef *stringTTd;
	STRPTR idString = (STRPTR) ArgArray[0];
	STRPTR textString = (STRPTR) ArgArray[1];
	STRPTR srcString = (STRPTR) ArgArray[2];
	STRPTR textpenString = (STRPTR) ArgArray[3];
	STRPTR halignString = (STRPTR) ArgArray[4];
	STRPTR styleString = (STRPTR) ArgArray[5];
	STRPTR fontString = (STRPTR) ArgArray[6];
	STRPTR ttFontString = (STRPTR) ArgArray[7];
	STRPTR valignString = (STRPTR) ArgArray[8];
	ULONG hAlign = GTJ_LEFT;
	enum TTVertAlign vAlign = TTVERTALIGN_Top;
	ULONG Style = FS_NORMAL;
	ULONG TextPen = iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[TEXTPEN];
	ULONG TitleTag, TitleData;
	char FontName[128];
	char *TTFontDescriptor = NULL;
	struct TextAttr ftTextAttr;

	d1(kprintf("%s/%s/%ld: id=<%s>  text=<%s>  src=<%s>\n", __FILE__, __FUNC__, __LINE__, idString, textString, srcString));
	d1(kprintf("%s/%s/%ld: textpen=<%s>  halign=<%s>  style=<%s>\n", __FILE__, __FUNC__, __LINE__, textpenString, halignString, styleString));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Member:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR:  STRING outside MEMBER\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	if (NULL == ftd->ftd_CurrentTTItem->ftti_Parent)
		return RETURN_ERROR;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (halignString && strlen(halignString) > 0)
		{
		if (0 == Stricmp(halignString, "left"))
			hAlign = GTJ_LEFT;
		else if (0 == Stricmp(halignString, "center"))
			hAlign = GTJ_CENTER;
		else if (0 == Stricmp(halignString, "right"))
			hAlign = GTJ_RIGHT;
		else
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  unknown value <%s> for STRING HALIGN\n", \
				__FILE__, __FUNC__, __LINE__, halignString));
			return RETURN_ERROR;
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (styleString && strlen(styleString) > 0)
		{
		if (0 == Stricmp(styleString, "normal"))
			Style = FS_NORMAL;
		else if (0 == Stricmp(styleString, "bold"))
			Style = FSF_BOLD;
		else if (0 == Stricmp(styleString, "italic"))
			Style = FSF_ITALIC;
		else if (0 == Stricmp(styleString, "bolditalic"))
			Style = FSF_BOLD | FSF_ITALIC;
		else
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  unknown value <%s> for STRING STYLE\n", \
				__FILE__, __FUNC__, __LINE__, styleString));
			return RETURN_ERROR;
			}
		}

	if (valignString && strlen(valignString))
		{
		if (0 == Stricmp(valignString, "top"))
			vAlign = TTVERTALIGN_Top;
		else if (0 == Stricmp(valignString, "center"))
			vAlign = TTVERTALIGN_Center;
		else if (0 == Stricmp(valignString, "bottom"))
			vAlign = TTVERTALIGN_Bottom;
		else
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  unknown value <%s> for STRING VALIGN\n", \
				__FILE__, __FUNC__, __LINE__, valignString));
			return RETURN_ERROR;
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (textpenString && strlen(textpenString) > 0)
		{
		LONG PenIndex;

		PenIndex = ConvertPenNameToPen(textpenString);

		if (UNKNOWN_PEN != PenIndex)
			TextPen = PalettePrefs.pal_PensList[PenIndex];
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	memset(&ftTextAttr, 0, sizeof(ftTextAttr));

	// font specification - format: "fontname.font/size"
	//  or				"fontname/size"
	if (fontString && strlen(fontString) > 0)
		{
		CONST_STRPTR lp;
		STRPTR dlp;
		size_t Len;
		LONG YSize;

		ftTextAttr.ta_Name = FontName;
		ftTextAttr.ta_Style = FS_NORMAL;

		for (lp=fontString, dlp=FontName, Len=sizeof(FontName); 
			*lp && '/' != *lp && Len > 1; )
			{
			*dlp++ = *lp++;
			Len--;
			}
		*dlp = '\0';

		if ('/' != *lp)
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  invalid font spec <%s> for STRING FONT\n", \
				__FILE__, __FUNC__, __LINE__, fontString));
			return RETURN_ERROR;
			}

		// if it is missing, append ".font" to font name
		if (strlen(FontName) < 5 || 0 != stricmp(FontName + strlen(FontName) - 5, ".font"))
			{
			SafeStrCat(FontName, ".font", sizeof(FontName));
			}

		lp++;		// skip "/"

		if (StrToLong(lp, &YSize) <= 0)
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  invalid font size <%s> for STRING FONT\n", \
				__FILE__, __FUNC__, __LINE__, lp));
			return RETURN_ERROR;
			}
		ftTextAttr.ta_YSize = YSize;

		// enforce YSize is within 2..127
		if (ftTextAttr.ta_YSize < 2 || ftTextAttr.ta_YSize > 127)
			{
			d1(kprintf("%s/%s/%ld: RETURN_ERROR:  invalid font size %ld for STRING FONT\n", \
				__FILE__, __FUNC__, __LINE__, ftTextAttr.ta_YSize));
			return RETURN_ERROR;
			}
		}

	if (ttFontString && strlen(ttFontString) > 0)
		{
		// TrueType Font Descriptor format:
		// "style/weight/size/fontname"
		TTFontDescriptor = ttFontString;
		d1(kprintf("%s/%s/%ld: TTFontDescriptor=<%s>\n", __FILE__, __FUNC__, __LINE__, TTFontDescriptor));
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (idString && strlen(idString) > 0)
		{
		TitleTag = TT_TitleID;
		TitleData = ConvertMsgNameToID(idString);
		}
	else if (textString && strlen(textString) > 0)
		{
		TitleTag = TT_Title;
		TitleData = (ULONG) textString;
		}
	else if (srcString && strlen(srcString) > 0)
		{
		TitleTag = TT_TitleHook;
		TitleData = (ULONG) srcString;
		}
	else
		return RETURN_ERROR;

	stringTTd = TT_CreateItem(TitleTag, TitleData,
				TT_HAlign, hAlign,
				TT_VAlign, vAlign,
				TT_TextStyle, Style,
				TT_TextPen, TextPen,
				ftTextAttr.ta_Name ? TT_Font : TAG_IGNORE, &ftTextAttr,
				TTFontDescriptor ? TT_TTFont : TAG_IGNORE, TTFontDescriptor,
				ftd->ftd_CurrentTTItem->ftti_HideHookString ? TT_HiddenHook : TAG_IGNORE, (ULONG) ftd->ftd_CurrentTTItem->ftti_HideHookString,
				TAG_END);

	d1(kprintf("%s/%s/%ld: stringTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, stringTTd));
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem));
	d1(kprintf("%s/%s/%ld: CurrentTTItem=%08lx  ftti_TagList=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, ftd->ftd_CurrentTTItem, ftd->ftd_CurrentTTItem->ftti_TagList));

	if (NULL == stringTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, TT_Item, (ULONG) stringTTd))
		{
		TTDisposeItem(stringTTd);
		return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtSpace(struct FileTypeDef *ftd, LONG *ArgArray)
{
	ULONG *Size = (ULONG *) ArgArray[0];
	struct ttDef *spaceTTd;

	d1(kprintf("%s/%s/%ld: Size=%lu\n", __FILE__, __FUNC__, __LINE__, Size));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Member:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: SPACE outside MEMBER\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	spaceTTd = TT_CreateItem(TT_Space, *Size, 
		ftd->ftd_CurrentTTItem->ftti_HideHookString ? TT_HiddenHook : TAG_IGNORE, ftd->ftd_CurrentTTItem->ftti_HideHookString,
		TAG_END);

	d1(kprintf("%s/%s/%ld: spaceTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, spaceTTd));

	if (NULL == spaceTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, 
			TT_Item, (ULONG) spaceTTd))
		{
		TTDisposeItem(spaceTTd);
		return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtImage(struct FileTypeDef *ftd, LONG *ArgArray)
{
	STRPTR fileName = (STRPTR) ArgArray[0];
	struct ttDef *imageTTd;

	d1(kprintf("%s/%s/%ld: fileName=<%s>\n", __FILE__, __FUNC__, __LINE__, fileName));

	if (NULL == ftd->ftd_CurrentTTItem)
		return RETURN_ERROR;

	switch (ftd->ftd_CurrentTTItem->ftti_Type)
		{
	case TTITYPE_Member:
		break;
	case TTITYPE_Tooltip:
	case TTITYPE_Group:
	default:
		d1(kprintf("%s/%s/%ld: RETURN_ERROR: HBAR outside MEMBER\n", __FILE__, __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	imageTTd = TT_CreateItem(TT_DTImage, fileName, 
		ftd->ftd_CurrentTTItem->ftti_HideHookString ? TT_HiddenHook : TAG_IGNORE, ftd->ftd_CurrentTTItem->ftti_HideHookString,
		TAG_END);

	d1(kprintf("%s/%s/%ld: imageTTd=%08lx\n", __FILE__, __FUNC__, __LINE__, imageTTd));

	if (NULL == imageTTd)
		return RETURN_ERROR;

	if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_CurrentTTItem->ftti_TagList, 
			TT_Item, (ULONG) imageTTd))
		{
		TTDisposeItem(imageTTd);
		return RETURN_ERROR;
		}

	return RETURN_OK;
}

//----------------------------------------------------------------------------

static LONG FtBeginPopupMenu(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	while ((ftmi = (struct FileTypeMenuItem *) RemTail(&ftd->ftd_MenuList)))
		{
		d1(kprintf("%s/%s/%ld: ftmi=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi));
		DisposeFileTypeMenuItem(ftd, ftmi);
		}
	if (ftd->ftd_PopupMenu && PopupMenuBase)
		{
		PM_FreePopupMenu(ftd->ftd_PopupMenu);
		ftd->ftd_PopupMenu = NULL;
		}

	if (ftd->ftd_Description)
		{
		if (PopupMenuBase)
			{
			if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_TagList, PM_Item,
					(ULONG) PM_MakeItem(PM_ID, PM_TITLE_ID,
						PM_Title, (ULONG) ftd->ftd_Description,
						PM_NoSelect, TRUE,
						PM_ShinePen, TRUE,
						PM_Shadowed, FALSE,
						PM_Centre, TRUE,
						TAG_END)))
				return RETURN_ERROR;

			if (RETURN_OK != ScalosTagListNewEntry(&ftd->ftd_TagList, 
						PM_Item, (ULONG) PM_MakeItem(PM_WideTitleBar, TRUE,
						TAG_END)))
				return RETURN_ERROR;
			}
		}

	return RETURN_OK;
}


static LONG FtEndPopupMenu(struct FileTypeDef *ftd, LONG *ArgArray)
{
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosTagListEnd(&ftd->ftd_TagList);

	if (PopupMenuBase)
		ftd->ftd_PopupMenu = PM_MakeMenuA(ftd->ftd_TagList.stl_TagList);

	return RETURN_OK;
}


static LONG FtIncludeFile(struct FileTypeDef *ftd, LONG *ArgArray)
{
	STRPTR fileName = (STRPTR) ArgArray[0];

	d1(kprintf("%s/%s/%ld: fileName=<%s>\n", __FILE__, __FUNC__, __LINE__, fileName));

	if (ftd->ftd_IncludeNesting + 1 >= MAX_INCLUDE_NESTING)
		return RETURN_ERROR;

	ftd->ftd_IncludeNesting++;

	return ReadFileTypeDef(ftd, fileName);
}


static LONG GetFtDescription(struct FileTypeDef *ftd, LONG *ArgArray)
{
	STRPTR descName = (STRPTR) ArgArray[0];

	d1(kprintf("%s/%s/%ld: descName=<%s>\n", __FILE__, __FUNC__, __LINE__, descName));

	if (NULL == ftd->ftd_Description)
		{
		ftd->ftd_Description = AllocCopyString(descName);
		if (NULL == ftd->ftd_Description)
			return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG GetFtPvPlugin(struct FileTypeDef *ftd, LONG *ArgArray)
{
	STRPTR pluginName = (STRPTR) ArgArray[0];

	d1(KPrintF("%s/%s/%ld: pluginName=<%s>\n", __FILE__, __FUNC__, __LINE__, pluginName));

	if (NULL == ftd->ftd_PreviewPluginName)
		{
		ftd->ftd_PreviewPluginName = AllocCopyString(pluginName);
		if (NULL == ftd->ftd_PreviewPluginName)
			return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG AddFtMenuSeparator(struct FileTypeDef *ftd, LONG *ArgArray)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (ftd->ftd_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(ftd->ftd_CurrentTagList, PM_Item,
				(ULONG) PM_MakeItem(PM_UserData, NULL,
					PM_TitleBar, TRUE,
					TAG_END)))
			return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG NewFtMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi;
	STRPTR itemName = (STRPTR) ArgArray[0];
	STRPTR commKey = (STRPTR) ArgArray[1];
	BOOL defaultAction = (BOOL) ArgArray[2];
	STRPTR SelectedIconName = (STRPTR) ArgArray[3];
	STRPTR UnselectedIconName = (STRPTR) ArgArray[4];

	d1(kprintf("%s/%s/%ld: itemName=<%s>  commKey=<%s>\n", __FILE__, __FUNC__, __LINE__, itemName, commKey));

	if (ftd->ftd_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (defaultAction && ftd->ftd_DefaultAction)
		return RETURN_ERROR;	// only one DEFAULTACTION menu item allowed

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	// Start new menu tree
	ftd->ftd_CurrentMenuItem = ftmi = NewFileTypeMenuItem();
	d1(kprintf("%s/%s/%ld: ftmi=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi));
	if (NULL == ftmi)
		return RETURN_ERROR;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	AddTail(ftd->ftd_CurrentMenuList, &ftmi->ftmi_Node);

	ftmi->ftmi_DefaultAction = defaultAction;

	ftmi->ftmi_MenuName = AllocCopyString(itemName);
	if (NULL == ftmi->ftmi_MenuName)
		return RETURN_ERROR;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	ftmi->ftmi_MenuCommKey = AllocCopyString(commKey);
	if (NULL == ftmi->ftmi_MenuCommKey)
		return RETURN_ERROR;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	ftmi->ftmi_MenuIconSelected = ftmi->ftmi_MenuIconUnselected  = NULL;
	if (SelectedIconName)
		{
		ftmi->ftmi_MenuIconSelected = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) SelectedIconName,
			TAG_END);
		}
	if (UnselectedIconName)
		{
		ftmi->ftmi_MenuIconUnselected = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) UnselectedIconName,
			TAG_END);
		}
	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_Title, (ULONG) ftmi->ftmi_MenuName))
			return RETURN_ERROR;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		if (RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_CommKey, (ULONG) ftmi->ftmi_MenuCommKey))
			return RETURN_ERROR;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		if (RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_Bold, (ULONG) defaultAction))
			return RETURN_ERROR;

		if (ftmi->ftmi_MenuIconSelected && RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_IconSelected, (ULONG) ftmi->ftmi_MenuIconSelected))
			return RETURN_ERROR;
		if (ftmi->ftmi_MenuIconUnselected && RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_IconUnselected, (ULONG) ftmi->ftmi_MenuIconUnselected))
			return RETURN_ERROR;
		}
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	return RETURN_OK;
}


static LONG EndFtMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;

	d1(KPrintF("%s/%s/%ld: UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi->ftmi_MenuTree));

	if (NULL == ftmi)
		return RETURN_ERROR;	// only allowed if menu item pending

	if (ftmi->ftmi_DefaultAction)
		ftd->ftd_DefaultAction = ftmi->ftmi_MenuTree;

	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_UserData, (ULONG) ftmi->ftmi_MenuTree))
			return RETURN_ERROR;
		}

	ScalosTagListEnd(&ftmi->ftmi_TagList);

#ifdef SHOWTAGLIST
	d1(ShowTagList(ftmi->ftmi_TagList));
#endif /* SHOWTAGLIST */

	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(ftd->ftd_CurrentTagList, 
				PM_Item, (ULONG) PM_MakeItemA(ftmi->ftmi_TagList.stl_TagList)))
			return RETURN_ERROR;
		}

	ftd->ftd_CurrentMenuItem = NULL;

	return RETURN_OK;
}


static LONG AddSubMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi;
	STRPTR itemName = (STRPTR) ArgArray[0];
	STRPTR SelectedIconName = (STRPTR) ArgArray[1];
	STRPTR UnselectedIconName = (STRPTR) ArgArray[2];

	d1(kprintf("%s/%s/%ld: itemName=<%s>\n", __FILE__, __FUNC__, __LINE__, itemName));

	if (ftd->ftd_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	if (ftd->ftd_ParentMenuItem)
		return RETURN_ERROR;	// only 1 level of submenus is allowed

	// Start new menu tree
	ftd->ftd_ParentMenuItem = ftmi = NewFileTypeMenuItem();
	d1(kprintf("%s/%s/%ld: ftmi=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi));
	if (NULL == ftmi)
		return RETURN_ERROR;

	AddTail(ftd->ftd_CurrentMenuList, &ftmi->ftmi_Node);

	ftmi->ftmi_MenuName = AllocCopyString(itemName);
	if (NULL == ftmi->ftmi_MenuName)
		return RETURN_ERROR;

	ftmi->ftmi_MenuIconSelected = ftmi->ftmi_MenuIconUnselected  = NULL;
	if (SelectedIconName)
		{
		ftmi->ftmi_MenuIconSelected = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) SelectedIconName,
			TAG_END);
		}
	if (UnselectedIconName)
		{
		ftmi->ftmi_MenuIconUnselected = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) UnselectedIconName,
			TAG_END);
		}

	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_Title, (ULONG) ftmi->ftmi_MenuName))
			return RETURN_ERROR;

		if (ftmi->ftmi_MenuIconSelected && RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_IconSelected, (ULONG) ftmi->ftmi_MenuIconSelected))
			return RETURN_ERROR;
		if (ftmi->ftmi_MenuIconUnselected && RETURN_OK != ScalosTagListNewEntry(&ftmi->ftmi_TagList, PM_IconUnselected, (ULONG) ftmi->ftmi_MenuIconUnselected))
			return RETURN_ERROR;
		}

	ftd->ftd_CurrentMenuList = &ftmi->ftmi_SubItemList;
	ftd->ftd_CurrentTagList = &ftmi->ftmi_TagList;

	return RETURN_OK;
}


static LONG EndSubMenuEntry(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_ParentMenuItem;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (ftd->ftd_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	if (NULL == ftmi)
		return RETURN_ERROR;	// no submenu pending

	ScalosTagListEnd(&ftmi->ftmi_TagList);

	ftd->ftd_ParentMenuItem = ftmi->ftmi_Parent;

	if (ftd->ftd_ParentMenuItem)
		{
		ftd->ftd_CurrentMenuList = &ftd->ftd_ParentMenuItem->ftmi_SubItemList;
		ftd->ftd_CurrentTagList = &ftd->ftd_ParentMenuItem->ftmi_TagList;
		}
	else
		{
		ftd->ftd_CurrentMenuList = &ftd->ftd_MenuList;
		ftd->ftd_CurrentTagList = &ftd->ftd_TagList;
		}

	if (PopupMenuBase)
		{
		if (RETURN_OK != ScalosTagListNewEntry(ftd->ftd_CurrentTagList, 
				PM_Item, (ULONG) PM_MakeItem(PM_Sub, (ULONG) PM_MakeMenuA(ftmi->ftmi_TagList.stl_TagList),
						PM_Title, (ULONG) ftmi->ftmi_MenuName,
						ftmi->ftmi_MenuIconSelected   ? PM_IconSelected   : TAG_IGNORE, (ULONG) ftmi->ftmi_MenuIconSelected,
						ftmi->ftmi_MenuIconUnselected ? PM_IconUnselected : TAG_IGNORE, (ULONG) ftmi->ftmi_MenuIconUnselected,
						TAG_END)))
			return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtInternalCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	struct ScalosMenuTree *mTree;

	d1(KPrintF("%s/%s/%ld: cmdName=<%s>\n", __FILE__, __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		d1(KPrintF("%s/%s/%ld: ftmi_MenuTree=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi->ftmi_MenuTree));

		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_Command;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);

		d1(KPrintF("%s/%s/%ld: mTree=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, mTree, cmdName));
		d1(KPrintF("%s/%s/%ld: ftmi_MenuTree=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi->ftmi_MenuTree));

		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;
		}

	return RETURN_OK;
}


static LONG FtWbCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];
	struct ScalosMenuTree *mTree;

	d1(kprintf("%s/%s/%ld: cmdName=<%s>  StackSize=%08lx  wbArgs=%ld\n", __FILE__, __FUNC__, __LINE__, cmdName, StackSize, wbArgs));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_Workbench;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);
		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;

		if (NULL == StackSize)
			StackSize = &CurrentPrefs.pref_DefaultStackSize;
		if (NULL == Priority)
			Priority = &Default_Priority;

		mTree->MenuCombo.MenuCommand.mcom_stack = *StackSize;
		mTree->MenuCombo.MenuCommand.mcom_pri = (BYTE) *Priority;

		mTree->MenuCombo.MenuCommand.mcom_flags = MCOMFLGF_Priority;
		if (wbArgs)
			mTree->MenuCombo.MenuCommand.mcom_flags |= MCOMFLGF_Args;
		}

	return RETURN_OK;
}


static LONG FtARexxCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];
	struct ScalosMenuTree *mTree;

	d1(kprintf("%s/%s/%ld: cmdName=<%s>  StackSize=%08lx  wbArgs=%ld\n", __FILE__, __FUNC__, __LINE__, cmdName, StackSize, wbArgs));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_ARexx;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);
		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;

		if (NULL == StackSize)
			StackSize = &CurrentPrefs.pref_DefaultStackSize;
		if (NULL == Priority)
			Priority = &Default_Priority;

		mTree->MenuCombo.MenuCommand.mcom_stack = *StackSize;
		mTree->MenuCombo.MenuCommand.mcom_pri = (BYTE) *Priority;

		mTree->MenuCombo.MenuCommand.mcom_flags = MCOMFLGF_Priority;
		if (wbArgs)
			mTree->MenuCombo.MenuCommand.mcom_flags |= MCOMFLGF_Args;
		}

	return RETURN_OK;
}


static LONG FtCliCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];
	struct ScalosMenuTree *mTree;

	d1(kprintf("%s/%s/%ld: cmdName=<%s>  StackSize=%08lx\n", __FILE__, __FUNC__, __LINE__, cmdName, StackSize));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_AmigaDOS;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);
		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;

		if (NULL == StackSize)
			StackSize = &CurrentPrefs.pref_DefaultStackSize;
		if (NULL == Priority)
			Priority = &Default_Priority;

		mTree->MenuCombo.MenuCommand.mcom_stack = *StackSize;
		mTree->MenuCombo.MenuCommand.mcom_pri = (BYTE) *Priority;

		mTree->MenuCombo.MenuCommand.mcom_flags = MCOMFLGF_Priority;
		if (wbArgs)
			mTree->MenuCombo.MenuCommand.mcom_flags |= MCOMFLGF_Args;
		}

	return RETURN_OK;
}


static LONG FtPluginCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	struct ScalosMenuTree *mTree;

	d1(kprintf("%s/%s/%ld: cmdName=<%s>\n", __FILE__, __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_Plugin;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);
		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;

		mTree->MenuCombo.MenuCommand.mcom_stack = 0;
		mTree->MenuCombo.MenuCommand.mcom_flags = 0;
		}

	return RETURN_OK;
}


static LONG FtIconWindowCmd(struct FileTypeDef *ftd, LONG *ArgArray)
{
	struct FileTypeMenuItem *ftmi = ftd->ftd_CurrentMenuItem;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	struct ScalosMenuTree *mTree;

	d1(kprintf("%s/%s/%ld: cmdName=<%s>\n", __FILE__, __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	mTree = ScalosAlloc(sizeof(struct ScalosMenuTree));
	if (mTree)
		{
		AppendToMenuTree(&ftmi->ftmi_MenuTree, mTree);

		mTree->mtre_type = SCAMENUTYPE_Command;
		mTree->mtre_Next = NULL;
		mTree->mtre_tree = NULL;

		mTree->MenuCombo.MenuCommand.mcom_type = SCAMENUCMDTYPE_IconWindow;
		mTree->MenuCombo.MenuCommand.mcom_name = AllocCopyString(cmdName);
		if (NULL == mTree->MenuCombo.MenuCommand.mcom_name)
			return RETURN_ERROR;

		mTree->MenuCombo.MenuCommand.mcom_stack = 0;
		mTree->MenuCombo.MenuCommand.mcom_flags = 0;
		}

	return RETURN_OK;
}


static const struct CmdFunc *ParseFileTypeLine(CONST_STRPTR *Line)
{
	short i;
	size_t Len;
	CONST_STRPTR Command = *Line;
	const struct CmdFunc *Table;

	/* Skip leading blanks. */

	while(*Command == ' ' || *Command == '\t' || '\n' == *Command)
		Command++;

	/* Now determine the length of the command. */

	Len = 0;

	while (Command[Len])
		{
		if ('\n' == Command[Len] || Command[Len] == '\t' || Command[Len] == ' ')
			break;
		else
			Len++;
		}

	*Line = Command + Len;

	/* Now we need to check if the command we received
	 * can be handled by one of the routines in the
	 * command table.
	 */

	Table = CommandTable;

	d1(KPrintF("%s/%s/%ld: cmd=<%s>\n", __FILE__, __FUNC__, __LINE__, Command);)

	for(i = 0 ; Table[i].cf_CommandName != NULL ; i++)
		{
		if((strlen(Table[i].cf_CommandName) == Len) 
				&& (Strnicmp((STRPTR) Command, (STRPTR) Table[i].cf_CommandName,Len) == 0))
			{
			return &Table[i];
			}
		}

	return NULL;
}


struct FileTypeDef *FindFileType(struct internalScaWindowTask *iwt, const struct ScaIconNode *in)
{
	struct FileTypeDef *ftd;
	const struct TypeNode *tNode;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  in=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, GetIconName(in)));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_GetIconFileType, in);

	tNode = in->in_FileType;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  tNode=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, tNode));

	ftd = FindFileTypeForTypeNode(iwt, tNode);

	DisposeFileTypeDefTemp(ftd);

	d1(KPrintF("%s/%s/%ld: ftd=%08lx  ttd=%08lx  pm=%08lx  defAction=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_ToolTipDef : NULL,\
		ftd ? ftd->ftd_PopupMenu : NULL, ftd->ftd_DefaultAction));

	return ftd;
}


struct FileTypeDef *FindFileTypeForTypeNode(struct internalScaWindowTask *iwt, const struct TypeNode *tNode)
{
	struct FileTypeDef *ftd = NULL;

	do	{
		d1(kprintf("%s/%s/%ld: iwt=%08lx  tNode=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, tNode));

		switch ((ULONG) tNode)
			{
		case WBDISK:
			d1(kprintf("%s/%s/%ld: WBDISK start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = GetFileTypeDefForName("disk");
			if (ftd)
				{
				ScalosObtainSemaphore(&ftd->ftd_ReadSema);
				d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
				if (!ftd->ftd_ReadFinished)
					{
					ReadFileTypeGlobals(ftd, ftd->ftd_Node.ln_Name);
					ReadFileTypePopupMenu(ftd, ftd->ftd_Node.ln_Name);
					ReadFileTypeToolTipDef(ftd, ftd->ftd_Node.ln_Name);
					ftd->ftd_ReadFinished = TRUE;
					}
				ScalosReleaseSemaphore(&ftd->ftd_ReadSema);
				}
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		case WBDRAWER:
			d1(kprintf("%s/%s/%ld: WBDRAWER start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = ReadFileTypeDefForName("drawer");
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		case WBGARBAGE:
			d1(kprintf("%s/%s/%ld: WBGARBAGE start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = ReadFileTypeDefForName("trashcan");
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		case WBPROJECT:
			d1(kprintf("%s/%s/%ld: WBPROJECT start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = ReadFileTypeDefForName("project");
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		case WBTOOL:
			d1(kprintf("%s/%s/%ld: WBTOOL start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = ReadFileTypeDefForName("tool");
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		case WBAPPICON:
			d1(kprintf("%s/%s/%ld: WBAPPICON start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ftd = ReadFileTypeDefForName("appicon");
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;

		default:
			d1(kprintf("%s/%s/%ld: default start  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			if (IS_TYPENODE(tNode))
				{
				ftd = GetFileTypeDefForName(tNode->tn_Name);
				d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
				if (ftd && !ftd->ftd_ReadFinished)
					{
					const struct TypeNode *tNode2;

					ScalosObtainSemaphore(&ftd->ftd_ReadSema);
					d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore()  ftd=%08lx  ReadFinished=%ld\n", \
						__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
					if (ftd->ftd_ReadFinished)
						break;		// No need to read ftd again!

					for (tNode2=tNode; tNode2; tNode2 = tNode2->tn_Parent)
						{
						d1(kprintf("%s/%s/%ld: tn=%08lx  <%s>   Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, \
							tNode2, tNode2->tn_Name, tNode2->tn_Parent));

						if (ReadFileTypeGlobals(ftd, tNode2->tn_Name))
							break;
						}
					for (tNode2=tNode; tNode2; tNode2 = tNode2->tn_Parent)
						{
						d1(kprintf("%s/%s/%ld: tn=%08lx  <%s>   Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, \
							tNode2, tNode2->tn_Name, tNode2->tn_Parent));

						if (ReadFileTypePopupMenu(ftd, tNode2->tn_Name))
							break;
						}
					for (tNode2=tNode; tNode2; tNode2 = tNode2->tn_Parent)
						{
						d1(kprintf("%s/%s/%ld: tn=%08lx  <%s>   Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, \
							tNode2, tNode2->tn_Name, tNode2->tn_Parent));

						if (ReadFileTypeToolTipDef(ftd, tNode2->tn_Name))
							break;
						}
					ftd->ftd_ReadFinished = TRUE;
					ScalosReleaseSemaphore(&ftd->ftd_ReadSema);
					}
				}
			d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
			break;
			}
		} while (0);

	DisposeFileTypeDefTemp(ftd);

	d1(kprintf("%s/%s/%ld: ftd=%08lx  ttd=%08lx  pm=%08lx  defAction=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_ToolTipDef : NULL,\
		ftd ? ftd->ftd_PopupMenu : NULL, ftd->ftd_DefaultAction));

	return ftd;
}


void ReleaseFileType(struct FileTypeDef *ftd)
{
	d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

	if (ftd)
		{
		ScalosReleaseSemaphore(&ftd->ftd_Sema);
		}
}


static LONG ReadFileTypeDef(struct FileTypeDef *ftd, CONST_STRPTR fileName)
{
	LONG Result = RETURN_OK;
	char Line[256];
	struct FileTypeFileDesc *ftfd = &ftd->ftd_FileHandles[ftd->ftd_IncludeNesting];

	ftfd->ftfd_LineNumber = 0;
	ftfd->ftfd_FileHandle = Open((STRPTR) fileName, MODE_OLDFILE);

	NewFileTypeFileInfo(ftd, ftfd->ftfd_FileHandle, fileName);

	d1(kprintf("%s/%s/%ld: fd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftfd->ftfd_FileHandle));
	if ((BPTR)NULL == ftfd->ftfd_FileHandle)
		return IoErr();

	while (RETURN_OK == Result && FGets(ftfd->ftfd_FileHandle, Line, sizeof(Line)))
		{
		d1(KPrintF("%s/%s/%ld: Line = <%s>\n", __FILE__, __FUNC__, __LINE__, Line));

		ftfd->ftfd_LineNumber++;

		if (strlen(stpblk(Line)) > 0 && '#' != *Line)
			{
			const struct CmdFunc *cmdTableEntry;
			CONST_STRPTR LinePtr = Line;
			
			cmdTableEntry = ParseFileTypeLine(&LinePtr);
			d1(KPrintF("%s/%s/%ld: Line = <%s>  LinePtr=<%s>  cmdTableEntry=%08lx\n", __FILE__, __FUNC__, __LINE__, Line, LinePtr, cmdTableEntry));
			if (cmdTableEntry)
				{
				d1(KPrintF("%s/%s/%ld: CommandName=<%s>\n", __FILE__, __FUNC__, __LINE__, cmdTableEntry->cf_CommandName));

				if (FTSECTION_All == cmdTableEntry->cf_validForSection 
					|| ftd->ftd_RequestedSection == cmdTableEntry->cf_validForSection)
					{
					struct RDArgs *InputRdArgs;

					InputRdArgs = AllocDosObject(DOS_RDARGS, NULL);
					d1(kprintf("%s/%s/%ld: rdArgs=%08lx\n", __FILE__, __FUNC__, __LINE__, InputRdArgs));
					if (InputRdArgs)
						{
						struct RDArgs *rdArg;
						LONG ArgArray[10];

						d1(KPrintF("%s/%s/%ld: Template = <%s>\n", __FILE__, __FUNC__, __LINE__, cmdTableEntry->cf_Template));

						memset(ArgArray, 0, sizeof(ArgArray));

						InputRdArgs->RDA_Source.CS_Buffer = (STRPTR) LinePtr;
						InputRdArgs->RDA_Source.CS_Length = strlen(LinePtr);
						InputRdArgs->RDA_Source.CS_CurChr = 0;
						InputRdArgs->RDA_Flags |= RDAF_NOPROMPT;

						rdArg = ReadArgs((STRPTR) cmdTableEntry->cf_Template, ArgArray, InputRdArgs);
						d1(KPrintF("%s/%s/%ld: rdArg=%08lx\n", __FILE__, __FUNC__, __LINE__, rdArg));
						if (rdArg)
							{
							if (cmdTableEntry->cf_Function)
								Result = (cmdTableEntry->cf_Function)(ftd, ArgArray);

							FreeArgs(rdArg);

							d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
							}
						else
							{
							Result = IoErr();
							d1(kprintf("%s/%s/%ld: ReadArgs() returned error %ld\n", __FILE__, __FUNC__, __LINE__, Result));
							d1(kprintf("%s/%s/%ld: LinePtr=<%s>\n", __FILE__, __FUNC__, __LINE__, LinePtr));
							d1(kprintf("%s/%s/%ld: Template = <%s>\n", __FILE__, __FUNC__, __LINE__, cmdTableEntry->cf_Template));
							d1(kprintf("%s/%s/%ld: CommandName=<%s>\n", __FILE__, __FUNC__, __LINE__, cmdTableEntry->cf_CommandName));
							break;
							}

						FreeDosObject(DOS_RDARGS, InputRdArgs);
						}
					}
				}
			else
				{
				d1(kprintf("%s/%s/%ld: unknown keyword Line=<%s>\n", __FILE__, __FUNC__, __LINE__, LinePtr));
				}
			}
		}

	Close(ftfd->ftfd_FileHandle);
	ftfd->ftfd_FileHandle = (BPTR)NULL;

	if (ftd->ftd_IncludeNesting > 0)
		ftd->ftd_IncludeNesting--;	// return to previous include nesting level

	return Result;
}


static struct PopupMenu *ReadFileTypePopupMenu(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName)
{
	BPTR oldDir = NOT_A_LOCK;

	d1(kprintf("%s/%s/%ld: fileTypeName=<%s>\n", __FILE__, __FUNC__, __LINE__, fileTypeName));

	do	{
		ScalosObtainSemaphore(&ftd->ftd_ReadSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore()  ftd=%08lx  ReadFinished=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
		if (ftd->ftd_ReadFinished)
			break;		// No need to read ftd again!

		oldDir = CurrentDir(ftd->ftd_FileTypesDirLock);

		ftd->ftd_RequestedSection = FTSECTION_PopupMenu;

		if (RETURN_OK != ReadFileTypeDef(ftd, fileTypeName))
			break;

		ScalosTagListEnd(&ftd->ftd_TagList);

#ifdef SHOWTAGLIST
		d1(ShowTagList(ftd->ftd_TagList.stl_TagList));
#endif /* SHOWTAGLIST */

		d1(kprintf("%s/%s/%ld: ftd_PopupMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PopupMenu));
		if (NULL == ftd->ftd_PopupMenu)
			break;
		} while (0);

	ScalosReleaseSemaphore(&ftd->ftd_ReadSema);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: ftd_PopupMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PopupMenu));

	return ftd->ftd_PopupMenu;
}


static struct ttDef *ReadFileTypeGlobals(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName)
{
	BPTR oldDir = NOT_A_LOCK;

	d1(kprintf("%s/%s/%ld: fileTypeName=<%s>\n", __FILE__, __FUNC__, __LINE__, fileTypeName));

	do	{
		ScalosObtainSemaphore(&ftd->ftd_ReadSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore()  ftd=%08lx  ReadFinished=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
		if (ftd->ftd_ReadFinished)
			break;		// No need to read ftd again!

		oldDir = CurrentDir(ftd->ftd_FileTypesDirLock);

		ftd->ftd_RequestedSection = FTSECTION_Globals;
		d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		if (RETURN_OK != ReadFileTypeDef(ftd, fileTypeName))
			break;

		d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		ScalosTagListEnd(&ftd->ftd_TagList);

#ifdef SHOWTAGLIST
		d1(ShowTagList(ftd->ftd_TagList.stl_TagList));
#endif /* SHOWTAGLIST */

		d1(kprintf("%s/%s/%ld: ftd_ToolTipDef=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_ToolTipDef));
		if (NULL == ftd->ftd_ToolTipDef)
			break;
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	ScalosReleaseSemaphore(&ftd->ftd_ReadSema);

	d1(kprintf("%s/%s/%ld: ftd_ToolTipDef=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_ToolTipDef));

	return ftd->ftd_ToolTipDef;
}


static struct ttDef *ReadFileTypeToolTipDef(struct FileTypeDef *ftd, CONST_STRPTR fileTypeName)
{
	BPTR oldDir = NOT_A_LOCK;

	d1(kprintf("%s/%s/%ld: fileTypeName=<%s>\n", __FILE__, __FUNC__, __LINE__, fileTypeName));

	do	{
		ScalosObtainSemaphore(&ftd->ftd_ReadSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore()  ftd=%08lx  ReadFinished=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));
		if (ftd->ftd_ReadFinished)
			break;		// No need to read ftd again!

		oldDir = CurrentDir(ftd->ftd_FileTypesDirLock);

		ftd->ftd_RequestedSection = FTSECTION_Tooltip;
		d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		if (RETURN_OK != ReadFileTypeDef(ftd, fileTypeName))
			break;

		d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		ScalosTagListEnd(&ftd->ftd_TagList);

#ifdef SHOWTAGLIST
		d1(ShowTagList(ftd->ftd_TagList.stl_TagList));
#endif /* SHOWTAGLIST */

		d1(kprintf("%s/%s/%ld: ftd_ToolTipDef=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_ToolTipDef));
		if (NULL == ftd->ftd_ToolTipDef)
			break;
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	ScalosReleaseSemaphore(&ftd->ftd_ReadSema);

	d1(kprintf("%s/%s/%ld: ftd_ToolTipDef=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_ToolTipDef));

	return ftd->ftd_ToolTipDef;
}


static struct FileTypeDef *CreateFileTypeDef(void)
{
	struct FileTypeDef *ftd;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		ftd = ScalosAlloc(sizeof(struct FileTypeDef));
		if (NULL == ftd)
			break;

		memset(ftd, 0, sizeof(struct FileTypeDef));

		ftd->ftd_CurrentMenuList = &ftd->ftd_MenuList;
		NewList(ftd->ftd_CurrentMenuList);

		NewList(&ftd->ftd_FileInfoList);

		ftd->ftd_CurrentTagList = &ftd->ftd_TagList;
		ftd->ftd_CurrentMenuItem = NULL;
		ftd->ftd_CurrentTTItem = NULL;

		ftd->ftd_DefaultAction = NULL;

		ftd->ftd_Name = NULL;

		ftd->ftd_PreviewPluginName = NULL;
		ftd->ftd_PreviewPluginBase = NULL;

		ScalosInitSemaphore(&ftd->ftd_Sema);
		ScalosInitSemaphore(&ftd->ftd_ReadSema);
		ScalosInitSemaphore(&ftd->ftd_DisposeSema);
		ScalosInitSemaphore(&ftd->ftd_FileInfoSema);

		ftd->ftd_ReadFinished = FALSE;

		memset(ftd->ftd_FileHandles, 0, sizeof(ftd->ftd_FileHandles));
		ftd->ftd_IncludeNesting = 0;

		if (RETURN_OK != ScalosTagListInit(&ftd->ftd_TagList))
			break;

		ftd->ftd_FileTypesDirLock = Lock(FILETYPES_DIR1, ACCESS_READ);
		if ((BPTR)NULL == ftd->ftd_FileTypesDirLock)
			ftd->ftd_FileTypesDirLock = Lock(FILETYPES_DIR2, ACCESS_READ);
		if ((BPTR)NULL == ftd->ftd_FileTypesDirLock)
			break;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		DisposeFileTypeDef(ftd);
		ftd = NULL;
		}

	d1(kprintf("%s/%s/%ld: CREATE ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

	return ftd;
}


static void DisposeFileTypeDef(struct FileTypeDef *ftd)
{
	d1(kprintf("%s/%s/%ld: DISPOSE ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

	if (ftd)
		{
		struct FileTypeMenuItem *ftmi;
		struct FileTypeFileInfo *ffi;

		ScalosObtainSemaphore(&ftd->ftd_DisposeSema);
		Forbid();
		ScalosReleaseSemaphore(&ftd->ftd_DisposeSema);

		DisposeFileTypeDefTemp(ftd);

		if (ftd->ftd_PreviewPluginBase)
			{
#ifdef __amigaos4__
			if (ftd->ftd_IPreviewPlugin)
				{
				DropInterface((struct Interface *)ftd->ftd_IPreviewPlugin);
				ftd->ftd_IPreviewPlugin = NULL;
				}
#endif
			CloseLibrary(ftd->ftd_PreviewPluginBase);
			ftd->ftd_PreviewPluginBase = NULL;
			}
		while ((ftmi = (struct FileTypeMenuItem *) RemTail(&ftd->ftd_MenuList)))
			{
			d1(kprintf("%s/%s/%ld: ftmi=%08lx\n", __FILE__, __FUNC__, __LINE__, ftmi));
			DisposeFileTypeMenuItem(ftd, ftmi);
			}

		ScalosObtainSemaphore(&ftd->ftd_FileInfoSema);
		while ((ffi = (struct FileTypeFileInfo *) RemTail(&ftd->ftd_FileInfoList)))
			{
			d1(kprintf("%s/%s/%ld: ffi=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi));
			DisposeFileTypeFileInfo(ffi);
			}
		ScalosReleaseSemaphore(&ftd->ftd_FileInfoSema);

		if (ftd->ftd_DefaultAction)
			{
			FreeMenuTree(NULL, ftd->ftd_DefaultAction);
			ftd->ftd_DefaultAction = NULL;
			}

		d1(KPrintF("%s/%s/%ld: ftd_PreviewPluginName=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PreviewPluginName));
		if (ftd->ftd_PreviewPluginName)
			{
			FreeCopyString(ftd->ftd_PreviewPluginName);
			ftd->ftd_PreviewPluginName = NULL;
			}

		d1(kprintf("%s/%s/%ld: ftd_Description=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_Description));
		if (ftd->ftd_Description)
			{
			FreeCopyString(ftd->ftd_Description);
			ftd->ftd_Description = NULL;
			}

		d1(kprintf("%s/%s/%ld: ftd_PopupMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PopupMenu));
		if (ftd->ftd_PopupMenu && PopupMenuBase)
			{
			PM_FreePopupMenu(ftd->ftd_PopupMenu);
			ftd->ftd_PopupMenu = NULL;
			}
		if (ftd->ftd_ToolTipDef)
			{
			TTDisposeItem(ftd->ftd_ToolTipDef);
			ftd->ftd_ToolTipDef = NULL;
			}
		if (ftd->ftd_Name)
			{
			FreeCopyString(ftd->ftd_Name);
			ftd->ftd_Name = NULL;
			}

		ScalosFree(ftd);

		Permit();
		}
}


static void DisposeFileTypeDefTemp(struct FileTypeDef *ftd)
{
	d1(kprintf("%s/%s/%ld: DISPOSE ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

	if (ftd)
		{
		ULONG n;

		ScalosObtainSemaphore(&ftd->ftd_DisposeSema);

		for (n=0; n<sizeof(ftd->ftd_FileHandles)/sizeof(ftd->ftd_FileHandles[0]); n++)
			{
			if (ftd->ftd_FileHandles[n].ftfd_FileHandle)
				{
				Close(ftd->ftd_FileHandles[n].ftfd_FileHandle);
				ftd->ftd_FileHandles[n].ftfd_FileHandle = (BPTR)NULL;
				}
			}

		d1(kprintf("%s/%s/%ld: ftd_TagList.stl_TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_TagList.stl_TagList));
		ScalosTagListCleanup(&ftd->ftd_TagList);

		if (ftd->ftd_FileTypesDirLock)
			{
			UnLock(ftd->ftd_FileTypesDirLock);
			ftd->ftd_FileTypesDirLock = (BPTR)NULL;
			}

		if (ftd->ftd_CurrentTTItem)
			{
			DisposeFileTypeTTItem(ftd->ftd_CurrentTTItem);
			ftd->ftd_CurrentTTItem = NULL;
			}


		ScalosReleaseSemaphore(&ftd->ftd_DisposeSema);
		}
}


static struct FileTypeMenuItem *NewFileTypeMenuItem(void)
{
	struct FileTypeMenuItem *newFtmi;

	newFtmi = ScalosAlloc(sizeof(struct FileTypeMenuItem));
	if (NULL == newFtmi)
		return NULL;

	NewList(&newFtmi->ftmi_SubItemList);
	newFtmi->ftmi_MenuTree = NULL;
	newFtmi->ftmi_Parent = NULL;

	if (RETURN_OK != ScalosTagListInit(&newFtmi->ftmi_TagList))
		{
		ScalosFree(newFtmi);
		return NULL;
		}
	newFtmi->ftmi_MenuName = NULL;
	newFtmi->ftmi_MenuCommKey = NULL;
	newFtmi->ftmi_DefaultAction = FALSE;
	newFtmi->ftmi_MenuIconSelected = newFtmi->ftmi_MenuIconUnselected  = NULL;

	return newFtmi;
}


static void DisposeFileTypeMenuItem(const struct FileTypeDef *ParentFtd, struct FileTypeMenuItem *ftmi)
{
	if (ftmi)
		{
		struct FileTypeMenuItem *ftmiSub;

		while ((ftmiSub = (struct FileTypeMenuItem *) RemTail(&ftmi->ftmi_SubItemList)))
			DisposeFileTypeMenuItem(ParentFtd, ftmiSub);

		FreeMenuTree(ParentFtd, ftmi->ftmi_MenuTree);
		ftmi->ftmi_MenuTree = NULL;

		if (ftmi->ftmi_MenuIconSelected)
			DisposeObject(ftmi->ftmi_MenuIconSelected);
		if (ftmi->ftmi_MenuIconUnselected)
			DisposeObject(ftmi->ftmi_MenuIconUnselected);
		if (ftmi->ftmi_MenuName)
			FreeCopyString(ftmi->ftmi_MenuName);
		if (ftmi->ftmi_MenuCommKey)
			FreeCopyString(ftmi->ftmi_MenuCommKey);

		ScalosTagListCleanup(&ftmi->ftmi_TagList);

		ScalosFree(ftmi);
		}
}


static struct FileTypeTTItem *NewFileTypeTTItem(enum TTItemTypes type)
{
	struct FileTypeTTItem *newTTi;

	newTTi = ScalosAlloc(sizeof(struct FileTypeTTItem));
	if (NULL == newTTi)
		return NULL;

	newTTi->ftti_Parent = NULL;
	newTTi->ftti_HideHookString = NULL;
	newTTi->ftti_Type = type;

	if (RETURN_OK != ScalosTagListInit(&newTTi->ftti_TagList))
		{
		ScalosFree(newTTi);
		return NULL;
		}

	d1(kprintf("%s/%s/%ld: NEW tti=%08lx\n", __FILE__, __FUNC__, __LINE__, newTTi));

	return newTTi;
}


static void DisposeFileTypeTTItem(struct FileTypeTTItem *ftti)
{
	d1(kprintf("%s/%s/%ld: DISPOSE tti=%08lx\n", __FILE__, __FUNC__, __LINE__, ftti));

	if (ftti)
		{
		ScalosTagListCleanup(&ftti->ftti_TagList);

		if (ftti->ftti_HideHookString)
			{
			ScalosFree(ftti->ftti_HideHookString);
			ftti->ftti_HideHookString = NULL;
			}

		ScalosFree(ftti);
		}
}


LONG ConvertMsgNameToID(CONST_STRPTR MsgName)
{
	const struct MsgIdNameDef *mnd;

	for (mnd=MsgIdNames; mnd->mnd_MsgName; mnd++)
		{
		if (0 == Stricmp((STRPTR) MsgName, (STRPTR) mnd->mnd_MsgName))
			return (LONG) mnd->mnd_MsgId;
		}

	return UNKNOWN_MSG;
}


LONG ConvertPenNameToPen(CONST_STRPTR PenName)
{
	const struct PenNameDef *pnd;

	for (pnd=PenNames; pnd->pnd_PenName; pnd++)
		{
		if (0 == Stricmp((STRPTR) PenName, (STRPTR) pnd->pnd_PenName))
			return (LONG) pnd->pnd_PenNumber;
		}

	return UNKNOWN_PEN;
}


static void FreeMenuTree(const struct FileTypeDef *ParentFtd, struct ScalosMenuTree *mTree)
{
	d1(kprintf("%s/%s/%ld: ParentFtd=%08lx, mTree=%08lx\n", __FILE__, __FUNC__, __LINE__, ParentFtd, mTree));

	while (mTree)
		{
		struct ScalosMenuTree *mTreeNext = mTree->mtre_Next;

		// Do NOT free parent's ftd_DefaultAction here!
		if (NULL == ParentFtd || mTree != ParentFtd->ftd_DefaultAction)
			{
			FreeMenuTree(ParentFtd, mTree->mtre_tree);

			if (mTree->MenuCombo.MenuCommand.mcom_name)
				FreeCopyString(mTree->MenuCombo.MenuCommand.mcom_name);

			ScalosFree(mTree);
			}

		mTree = mTreeNext;
		}
}


static struct FileTypeDef *GetFileTypeDefForName(CONST_STRPTR Name)
{
	struct FileTypeDef *ftd;

	d1(kprintf("%s/%s/%ld: START Name=<%s> \n", __FILE__, __FUNC__, __LINE__, Name));

	ScalosObtainSemaphoreShared(&FileTypeListSema);
	ftd = (struct FileTypeDef *) FindName(&FileTypeList, (STRPTR) Name);
	ScalosReleaseSemaphore(&FileTypeListSema);

	d1(kprintf("%s/%s/%ld: Name=<%s> found ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, Name, ftd));

	if (NULL == ftd)
		{
		FileTypeFlushObsolete();

		ftd = CreateFileTypeDef();

		d1(kprintf("%s/%s/%ld: created ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		if (ftd)
			{
			AddFileTypeDef(ftd, Name);
			}
		}

	if (ftd)
		ScalosObtainSemaphoreShared(&ftd->ftd_Sema);

	d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

	return ftd;
}


static void AddFileTypeDef(struct FileTypeDef *ftd, CONST_STRPTR Name)
{
	d1(kprintf("%s/%s/%ld: START  ftd=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ftd, Name));

	if (ftd->ftd_Name)
		FreeCopyString(ftd->ftd_Name);

	ftd->ftd_Name = AllocCopyString(Name);
	if (ftd->ftd_Name)
		{
		ftd->ftd_Node.ln_Name = ftd->ftd_Name;

		FileTypeFlushObsolete();

		ScalosObtainSemaphore(&FileTypeListSema);
		AddTail(&FileTypeList, &ftd->ftd_Node);
		ScalosReleaseSemaphore(&FileTypeListSema);
		}
	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void FileTypeFlushObsolete(void)
{
	struct FileTypeDef *ftd;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&FileTypeListSema);

	// Walk through ObsoleteFileTypeList and try to free entries

	for (ftd = (struct FileTypeDef *) ObsoleteFileTypeList.lh_Head;
		ftd != (struct FileTypeDef *) &ObsoleteFileTypeList.lh_Tail;
		)
		{
		struct FileTypeDef *ftdNext = (struct FileTypeDef *) ftd->ftd_Node.ln_Succ;

		d1(kprintf("%s/%s/%ld: ftd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));

		if (ScalosAttemptSemaphore(&ftd->ftd_Sema))
			{
			d1(kprintf("%s/%s/%ld: delete from ObsoleteFileTypeList ftd=%08lx <%s>\n", \
				__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));
			Remove(&ftd->ftd_Node);
			Forbid();
			ScalosReleaseSemaphore(&ftd->ftd_Sema);
			DisposeFileTypeDef(ftd);
			Permit();
			}

		ftd = ftdNext;
		}

	ScalosReleaseSemaphore(&FileTypeListSema);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


struct FileTypeFileInfo *NewFileTypeFileInfo(struct FileTypeDef *ftd, BPTR fh, CONST_STRPTR Name)
{
	struct FileTypeFileInfo *ffi;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: ftd=%08lx <%s>  fh=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name, fh, Name));

	do	{
		ScalosObtainSemaphore(&ftd->ftd_FileInfoSema);

		ffi = (struct FileTypeFileInfo *) FindName(&ftd->ftd_FileInfoList, (STRPTR) Name);
		d1(kprintf("%s/%s/%ld:  ffi=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi));
		if (ffi)
			{
			Success = TRUE;
			break;
			}

		ffi = ScalosAlloc(sizeof(struct FileTypeFileInfo));
		d1(kprintf("%s/%s/%ld:  ffi=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi));
		if (NULL == ffi)
			break;

		ffi->ffi_NotifyNode = NULL;

		ffi->ffi_Name = AllocCopyString(Name);
		if (NULL == ffi->ffi_Name)
			break;

		ffi->ffi_Node.ln_Name = ffi->ffi_Name;

		ffi->ffi_Path = AllocPathBuffer();

		d1(kprintf("%s/%s/%ld:  ffi->Path=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi->ffi_Path));
		if (NULL == ffi->ffi_Path)
			break;

		if (fh)
			{
			if (!NameFromFH(fh, ffi->ffi_Path, MAX_FileName-1))
				break;

			d1(kprintf("%s/%s/%ld:  ffi->Path=<%s>\n", __FILE__, __FUNC__, __LINE__, ffi->ffi_Path));
			}
		else
			{
			BPTR dirLock;

			dirLock = CurrentDir((BPTR)NULL);
			if (!NameFromLock(dirLock, ffi->ffi_Path, MAX_FileName-1))
				{
				CurrentDir(dirLock);
				break;
				}

			d1(kprintf("%s/%s/%ld:  ffi->Path=<%s>\n", __FILE__, __FUNC__, __LINE__, ffi->ffi_Path));

			CurrentDir(dirLock);
			AddPart(ffi->ffi_Path, (STRPTR) Name, MAX_FileName-1);
			d1(kprintf("%s/%s/%ld:  ffi->Path=<%s>\n", __FILE__, __FUNC__, __LINE__, ffi->ffi_Path));
			}

		ffi->ffi_NotifyTab.nft_FileName = ffi->ffi_Path;
		ffi->ffi_NotifyTab.nft_Entry = FileTypeDefChanged;

		ffi->ffi_NotifyNode = AddToMainNotifyList(&ffi->ffi_NotifyTab, 0);
		d1(KPrintF("%s/%s/%ld:  ffi_NotifyNode=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi->ffi_NotifyNode));
		if (NULL == ffi->ffi_NotifyNode)
			break;

		AddTail(&ftd->ftd_FileInfoList, &ffi->ffi_Node);
		Success = TRUE;
		} while (0);

	ScalosReleaseSemaphore(&ftd->ftd_FileInfoSema);

	if (!Success)
		{
		DisposeFileTypeFileInfo(ffi);
		ffi = NULL;
		}

	return ffi;
}


static void DisposeFileTypeFileInfo(struct FileTypeFileInfo *ffi)
{
	d1(kprintf("%s/%s/%ld:  ffi=%08lx\n", __FILE__, __FUNC__, __LINE__, ffi));

	if (ffi->ffi_NotifyNode)
		{
		RemFromMainNotifyList(ffi->ffi_NotifyNode);
		ffi->ffi_NotifyNode = NULL;
		}
	if (ffi->ffi_Path)
		{
		FreePathBuffer(ffi->ffi_Path);
		ffi->ffi_Path = NULL;
		}
	if (ffi->ffi_Name)
		{
		FreeCopyString(ffi->ffi_Name);
		ffi->ffi_Name = NULL;
		}

	ScalosFree(ffi);
}


static struct FileTypeDef *ReadFileTypeDefForName(CONST_STRPTR Name)
{
	struct FileTypeDef *ftd;

	d1(kprintf("%s/%s/%ld: <%s> start\n", __FILE__, __FUNC__, __LINE__, Name));

	ftd = GetFileTypeDefForName(Name);
	if (ftd)
		{
		d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_ReadFinished));

		ScalosObtainSemaphore(&ftd->ftd_ReadSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore()  ftd=%08lx  ReadFinished=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));

		if (!ftd->ftd_ReadFinished)
			{
			ReadFileTypeGlobals(ftd, ftd->ftd_Node.ln_Name);
			ReadFileTypePopupMenu(ftd, ftd->ftd_Node.ln_Name);
			ReadFileTypeToolTipDef(ftd, ftd->ftd_Node.ln_Name);
			ftd->ftd_ReadFinished = TRUE;
			}
		ScalosReleaseSemaphore(&ftd->ftd_ReadSema);
		}

	d1(kprintf("%s/%s/%ld: ftd=%08lx  ReadFinished=%ld\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_ReadFinished));

	return ftd;
}


static void InvalidateFileType(struct FileTypeDef *ftd)
{
	d1(KPrintF("%s/%s/%ld: START ftd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));

	ScalosObtainSemaphore(&FileTypeListSema);

	Remove(&ftd->ftd_Node);

	if (ScalosAttemptSemaphore(&ftd->ftd_Sema))
		{
		d1(KPrintF("%s/%s/%ld: delete ftd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));
		Forbid();
		ScalosReleaseSemaphore(&ftd->ftd_Sema);
		DisposeFileTypeDef(ftd);
		Permit();
		}
	else
		{
		// cannot free ATM, move to ObsoleteFileTypeList
		d1(KPrintF("%s/%s/%ld: move to ObsoleteFileTypeList ftd=%08lx <%s>\n", \
			__FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));
		AddTail(&ObsoleteFileTypeList, &ftd->ftd_Node);

		if (ftd->ftd_PreviewPluginBase)
			{
			struct Library *PluginBase = ftd->ftd_PreviewPluginBase;

			ftd->ftd_PreviewPluginBase = NULL;
			d1(KPrintF("%s/%s/%ld:  CloseLibrary PluginBase=%08lx \n", __FILE__, __FUNC__, __LINE__, PluginBase));
#ifdef __amigaos4__
			if (ftd->ftd_IPreviewPlugin)
				{
				DropInterface((struct Interface *)ftd->ftd_IPreviewPlugin);
				ftd->ftd_IPreviewPlugin = NULL;
				}
#endif
			CloseLibrary(PluginBase);
			}
		}

	ScalosReleaseSemaphore(&FileTypeListSema);

	FileTypeFlushObsolete();

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void FileTypeDefChanged(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct FileTypeDef *ftdInvalidate = NULL;
	struct FileTypeDef *ftd;

	if (msg->nm_NReq && msg->nm_NReq->nr_Name)
		{
		d1(kprintf("%s/%s/%ld: START msg=%08lx  NotifyReq=%08lx  <%s>\n", \
			__FILE__, __FUNC__, __LINE__, msg, msg->nm_NReq, msg->nm_NReq->nr_Name));

		ScalosObtainSemaphore(&FileTypeListSema);

		for (ftd = (struct FileTypeDef *) FileTypeList.lh_Head;
			!ftdInvalidate && ftd != (struct FileTypeDef *) &FileTypeList.lh_Tail;
				)
			{
			struct FileTypeDef *ftdNext = (struct FileTypeDef *) ftd->ftd_Node.ln_Succ;
			struct FileTypeFileInfo *ffi;

			d1(kprintf("%s/%s/%ld: checking ftd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ftd, ftd->ftd_Name));

			ScalosObtainSemaphoreShared(&ftd->ftd_FileInfoSema);

			for (ffi = (struct FileTypeFileInfo *) ftd->ftd_FileInfoList.lh_Head;
				!ftdInvalidate && ffi != (struct FileTypeFileInfo *) &ftd->ftd_FileInfoList.lh_Tail;
				ffi = (struct FileTypeFileInfo *) ffi->ffi_Node.ln_Succ)
				{
				d1(kprintf("%s/%s/%ld: checking ffi=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, ffi, ffi->ffi_Path));

				if (&ffi->ffi_NotifyNode->non_NotifyRequest == msg->nm_NReq)
					ftdInvalidate = ftd;
				}

			ScalosReleaseSemaphore(&ftd->ftd_FileInfoSema);

			d1(kprintf("%s/%s/%ld: ftdInvalidate=%08lx\n", __FILE__, __FUNC__, __LINE__, ftdInvalidate));

			if (ftdInvalidate)
				InvalidateFileType(ftdInvalidate);

			ftd = ftdNext;
			}
		ScalosReleaseSemaphore(&FileTypeListSema);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


#ifdef SHOWTAGLIST
static void ShowTagList(struct TagItem *tagList)
{
	struct TagItem *tag;

	while (tag = NextTagItem(&tagList))
		{
		BOOL Found = FALSE;
		const struct TagItem *ti;
		const struct TagItem TagNames[] =
			{
			{ TT_Title,		(ULONG) "TT_Title"		},
			{ TT_TitleID,		(ULONG) "TT_TitleID"		},
			{ TT_HorizGroup,	(ULONG) "TT_HorizGroup"		},
			{ TT_VertGroup,		(ULONG) "TT_VertGroup"		},
			{ TT_TextStyle,		(ULONG) "TT_TextStyle"		},
			{ TT_HAlign,		(ULONG) "TT_HAlign"		},
			{ TT_Item,		(ULONG) "TT_Item"		},	
			{ TT_Members,		(ULONG) "TT_Members"		},
			{ TT_LayoutMode,	(ULONG) "TT_LayoutMode"		},
			{ TT_Hidden,		(ULONG) "TT_Hidden"		},
			{ TT_WideTitleBar,	(ULONG) "TT_WideTitleBar"	},
			{ TT_TextPen,		(ULONG) "TT_TextPen"		},
			{ TT_Image,		(ULONG) "TT_Image"		},
			{ TT_Space,		(ULONG) "TT_Space"		},
			{ TT_Font,		(ULONG) "TT_Font"		},	
			{ TT_DrawFrame,		(ULONG) "TT_DrawFrame"		},
			{ TT_TitleHook,		(ULONG) "TT_TitleHook"		},
			{ TT_HiddenHook,	(ULONG) "TT_HiddenHook"		},
			{ TT_SpaceLeft,		(ULONG) "TT_SpaceLeft"		},
			{ TT_SpaceRight,	(ULONG) "TT_SpaceRight"		},
			{ TT_SpaceTop,		(ULONG) "TT_SpaceTop"		},
			{ TT_SpaceBottom,	(ULONG) "TT_SpaceBottom"	},
			{ TT_DTImage,		(ULONG) "TT_DTImage"		},
			{ TAG_END						}
			};

		for (ti=TagNames; TAG_END != ti->ti_Tag; ti++)
			{
			if (ti->ti_Tag == tag->ti_Tag)
				{
				kprintf("%s/%s/%ld:      tag=<%s>  data=%08lx\n",
					__FILE__, __FUNC__, __LINE__, ti->ti_Data, tag->ti_Data);
				Found = TRUE;
				break;
				}
			}
		if (!Found)
			{
			kprintf("%s/%s/%ld:      tag=%08lx  data=%08lx\n", __FILE__, __FUNC__, __LINE__,
				tag->ti_Tag, tag->ti_Data);
			}
		}
}
#endif


