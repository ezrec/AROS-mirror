// ReadFtPrefs.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
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

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/locale.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>

#include <defs.h>

#include "debug.h"
#include "FileTypesPrefs.h"
#include "FileTypesPrefs_proto.h"

//----------------------------------------------------------------------------

struct CmdFunc
	{
	CONST_STRPTR cf_CommandName;	// Name of the command.
	CONST_STRPTR cf_Template;	// The command template in standard AmigaDOS syntax.

	enum FileTypeDefSections cf_validForSection;

	// Pointer to the function which implements this command.
	LONG (*cf_Function)(struct FileTypesPrefsInst *, struct MUI_NListtree_TreeNode *, LONG *);
	};

//----------------------------------------------------------------------------

static void ReadFileTypeDef(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn);
static LONG ReadFileTypeDefFile(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, CONST_STRPTR fileName);
static const struct CmdFunc *ParseFileTypeLine(CONST_STRPTR *Line);

static LONG FtBeginToolTip(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtEndToolTip(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtHBar(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtBeginGroup(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtEndGroup(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtBeginMember(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtEndMember(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtString(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtSpace(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtImage(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtBeginPopupMenu(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtEndPopupMenu(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtIncludeFile(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG GetFtDescription(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG GetFtPvPlugin(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG AddFtMenuSeparator(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG NewFtMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG EndFtMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG AddSubMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG EndSubMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtInternalCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtWbCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtARexxCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtCliCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtPluginCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);
static LONG FtIconWindowCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__AROS__)
static char *stpblk(const char *q);
#endif /* !defined(__SASC) && !defined(__MORPHOS__)  */

//----------------------------------------------------------------------------

static const struct CmdFunc CommandTable[] =
{
	{ "INCLUDE",		"NAME/A",						FTSECTION_All,		FtIncludeFile },

	{ "DESCRIPTION",	"NAME/A",						FTSECTION_All,		GetFtDescription },

	{ "PREVIEWPLUGIN",	"NAME/A",						FTSECTION_All,		GetFtPvPlugin },

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

	{ "GROUP",		"ORIENTATION/K",					FTSECTION_Tooltip,	FtBeginGroup },
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

//----------------------------------------------------------------------------

void ReadFileTypes(struct FileTypesPrefsInst *inst,
	CONST_STRPTR LoadName, CONST_STRPTR DefIconPrefsName)
{
	BPTR OldDir = (BPTR) NULL;
	BPTR fLock;
	struct MUI_NListtree_TreeNode *tn;
	struct FileInfoBlock *fib = NULL;
	BOOL SingleFile = FALSE;

	d1(kprintf(__FUNC__ "/%ld: LoadName=%08lx <%s>\n", __FUNC__, __LINE__, LoadName, LoadName ? LoadName : (CONST_STRPTR) ""));

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Clear, NULL, 0);

	if (inst->fpb_FileTypesDirLock)
		{
		UnLock(inst->fpb_FileTypesDirLock);
		inst->fpb_FileTypesDirLock = (BPTR) NULL;
		}

	do	{
		set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, TRUE);

		fLock = Lock(LoadName, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			break;

		fib = AllocDosObject(DOS_FIB, NULL);
		d1(kprintf(__FUNC__ "/%ld: fib=%08lx\n", __FUNC__, __LINE__, fib));
		if (NULL == fib)
			break;

		if (!Examine(fLock, fib))
			break;

		d1(kprintf(__FUNC__ "/%ld: fib_DirEntryType=%ld\n", __FUNC__, __LINE__, fib->fib_DirEntryType));

		if (fib->fib_DirEntryType > 0)
			{
			// "LoadName" is a directory -> use it to load filetypes descriptions
			inst->fpb_FileTypesDirLock = DupLock(fLock);
			if ((BPTR)NULL == inst->fpb_FileTypesDirLock)
				break;
			}
		else
			{
			// Single file specified, generate one entry for it
			SingleFile = TRUE;
			GenerateFileTypesNewEntry(inst, LoadName);
			}

		if (!SingleFile)
			{
			OldDir = CurrentDir(inst->fpb_FileTypesDirLock);

			d1(kprintf(__FUNC__ "/%ld: before InitDefIcons()\n", __FUNC__, __LINE__));

			InitDefIcons(inst, DefIconPrefsName);
			}

		// Now walk through all entries of OBJNDX_MainListTree
		// and try to read contents of every entry

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			MUIV_NListtree_GetEntry_ListNode_Root,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		d1(kprintf(__FUNC__ "/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

		while (tn)
			{
			d1(kprintf(__FUNC__ "/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

			ReadFileTypeDef(inst, tn);

			tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
				MUIM_NListtree_GetEntry, 
				tn, 
				MUIV_NListtree_GetEntry_Position_Next,
				MUIV_NListtree_GetEntry_Flag_SameLevel);
			} while (tn);

		} while (0);

	if (inst->fpb_HideEmptyNodes)
		HideEmptyNodes(inst);

	set(inst->fpb_Objects[OBJNDX_MainListView], MUIA_NList_Quiet, FALSE);

	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (fLock)
		UnLock(fLock);
	if (OldDir)
		CurrentDir(OldDir);
}

//----------------------------------------------------------------------------

static void ReadFileTypeDef(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn)
{
	do	{
		ULONG n;

		inst->fpb_RequestedSection = FTSECTION_All;
		inst->fpb_PopupMenu = NULL;
		inst->fpb_CurrentTTItem = NULL;
		inst->fpb_CurrentMenuItem = NULL;
		inst->fpb_ParentMenuItem = NULL;
		inst->fpb_DefaultActionSet = FALSE;

		for (n=0; n<Sizeof(inst->fpb_FileHandles); n++)
			{
			inst->fpb_FileHandles[n].ftfd_LineNumber = 0;

			d1(kprintf(__FUNC__ "/%ld: fpd_FileHandles[%ld].ftfd_FileHandle=%08lx\n", __FUNC__, __LINE__, n, inst->fpb_FileHandles[n].ftfd_FileHandle));

			if (inst->fpb_FileHandles[n].ftfd_FileHandle)
				{
				Close(inst->fpb_FileHandles[n].ftfd_FileHandle);
				inst->fpb_FileHandles[n].ftfd_FileHandle = (BPTR)NULL;
				}
			}

		inst->fpb_IncludeNesting = 0;

		ReadFileTypeDefFile(inst, tn, tn->tn_Name);
		} while (0);
}

//----------------------------------------------------------------------------

static LONG ReadFileTypeDefFile(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, CONST_STRPTR fileName)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
	LONG Result = RETURN_OK;
	char Line[8192];

	inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_LineNumber = 0;
	inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle = Open(fileName, MODE_OLDFILE);

	fte->ftle_FileFound = (BPTR)NULL != inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle;

	d1(kprintf(__FILE__ "/%s/%ld: fd=%08lx\n", __FUNC__, __LINE__, inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle));
	if ((BPTR)NULL == inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle)
		return IoErr();

	while (RETURN_OK == Result && FGets(inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle, Line, sizeof(Line)))
		{
		d1(kprintf(__FUNC__ "/%ld: Line = <%s>\n", __FUNC__, __LINE__, Line));

		inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_LineNumber++;

		if (strlen(stpblk(Line)) > 0 && '#' != *Line)
			{
			const struct CmdFunc *cmdTableEntry;
			CONST_STRPTR LinePtr = Line;
			
			cmdTableEntry = ParseFileTypeLine(&LinePtr);
			d1(kprintf(__FUNC__ "/%ld: Line = <%s>  LinePtr=<%s>  cmdTableEntry=%08lx\n", __FUNC__, __LINE__, Line, LinePtr, cmdTableEntry));
			if (cmdTableEntry)
				{
				struct RDArgs *InputRdArgs;

				d1(kprintf(__FUNC__ "/%ld: CommandName=<%s>\n", __FUNC__, __LINE__, cmdTableEntry->cf_CommandName));

				InputRdArgs = AllocDosObject(DOS_RDARGS, NULL);
				d1(kprintf(__FILE__ "/%s/%ld: rdArgs=%08lx\n", __FUNC__, __LINE__, InputRdArgs));
				if (InputRdArgs)
					{
					struct RDArgs *rdArg;
					SIPTR ArgArray[10];

					d1(kprintf(__FUNC__ "/%ld: Template = <%s>\n", __FUNC__, __LINE__, cmdTableEntry->cf_Template));

					memset(ArgArray, 0, sizeof(ArgArray));

					InputRdArgs->RDA_Source.CS_Buffer = (STRPTR) LinePtr;
					InputRdArgs->RDA_Source.CS_Length = strlen(LinePtr);
					InputRdArgs->RDA_Source.CS_CurChr = 0;
					InputRdArgs->RDA_Flags |= RDAF_NOPROMPT;

					rdArg = ReadArgs(cmdTableEntry->cf_Template, ArgArray, InputRdArgs);
					d1(kprintf(__FUNC__ "/%ld: rdArg=%08lx\n", __FUNC__, __LINE__, rdArg));
					if (rdArg)
						{
						if (cmdTableEntry->cf_Function)
							Result = (cmdTableEntry->cf_Function)(inst, tn, ArgArray);

						FreeArgs(rdArg);

						d1(kprintf(__FUNC__ "/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
						}
					else
						{
						Result = IoErr();
						d1(kprintf(__FUNC__ "/%ld: ReadArgs() returned error %ld\n", __FUNC__, __LINE__, Result));
						d1(kprintf(__FUNC__ "/%ld: LinePtr=<%s>\n", __FUNC__, __LINE__, LinePtr));
						d1(kprintf(__FUNC__ "/%ld: Template = <%s>\n", __FUNC__, __LINE__, cmdTableEntry->cf_Template));
						d1(kprintf(__FUNC__ "/%ld: CommandName=<%s>\n", __FUNC__, __LINE__, cmdTableEntry->cf_CommandName));
						break;
						}

					FreeDosObject(DOS_RDARGS, InputRdArgs);
					}
				}
			else
				{
				d1(kprintf(__FUNC__ "/%ld: unknown keyword Line=<%s>\n", __FUNC__, __LINE__, LinePtr));
				}
			}

		d1(kprintf(__FUNC__ "/%ld: Result=%ld\n", __FUNC__, __LINE__, Result));
		}

	Close(inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle);
	inst->fpb_FileHandles[inst->fpb_IncludeNesting].ftfd_FileHandle = (BPTR)NULL;

	if (inst->fpb_IncludeNesting > 0)
		inst->fpb_IncludeNesting--;	// return to previous include nesting level

	d1(kprintf(__FUNC__ "/%ld: Result=%ld  IncludeNesting=%ld\n", __FUNC__, __LINE__, Result, inst->fpb_IncludeNesting));

	return Result;
}


static const struct CmdFunc *ParseFileTypeLine(CONST_STRPTR *Line)
{
	short i;
	size_t Len;
	CONST_STRPTR Command = *Line;
	const struct CmdFunc *Table;

	/* Skip leading blanks. */

	Command = stpblk(Command);

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

	d1(kprintf(__FUNC__ "/%ld: cmd=<%s>\n", __FUNC__, __LINE__, Command);)

	for(i = 0 ; Table[i].cf_CommandName != NULL ; i++)
		{
		if((strlen(Table[i].cf_CommandName) == Len) && (Strnicmp(Command, Table[i].cf_CommandName,Len) == 0))
			{
			return &Table[i];
			}
		}

	return NULL;
}

//----------------------------------------------------------------------------

static LONG FtBeginToolTip(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;

	(void) ArgArray;

	d1(kprintf(__FILE__ "/%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

	if (inst->fpb_CurrentTTItem)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR TOOLTIP inside TOOLTIP\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	inst->fpb_CurrentTTItem = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip), NULL,
		tn,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);
	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;
	fte->ftle_EntryType = ENTRYTYPE_ToolTip;

	return RETURN_OK;
}


static LONG FtEndToolTip(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;

	(void) ArgArray;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	switch (fte->ftle_EntryType)
		{
	case ENTRYTYPE_ToolTip:
		break;
	default:
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: ENDTOOLTIP inside non-TOOLTIP\n", __FUNC__, __LINE__));
		inst->fpb_CurrentTTItem = NULL;
		return RETURN_ERROR;
		}

	inst->fpb_CurrentTTItem = NULL;

	return RETURN_OK;
}


static LONG FtHBar(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;
	struct MUI_NListtree_TreeNode *barTTd;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (ENTRYTYPE_ToolTip_Member != fte->ftle_EntryType)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: HBAR outside MEMBER\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	barTTd = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_HBar), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: barTTd=%08lx\n", __FUNC__, __LINE__, barTTd));

	if (NULL == barTTd)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) barTTd->tn_User;
	fte->ftle_EntryType = ENTRYTYPE_ToolTip_HBar;

	return RETURN_OK;
}


static LONG FtBeginGroup(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;
	struct MUI_NListtree_TreeNode *groupTTi;
	enum TTLayoutMode orientation = TTL_Vertical;
	STRPTR orientationString = (STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: orientation=<%s>\n", __FUNC__, __LINE__, orientationString));

	if (NULL == inst->fpb_CurrentTTItem)
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

	groupTTi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_Group), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: groupTTi=%08lx\n", __FUNC__, __LINE__, groupTTi));

	if (NULL == groupTTi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) groupTTi->tn_User;

	AddAttribute(fte, ATTRTYPE_GroupOrientation, sizeof(orientation), &orientation);

	fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent = inst->fpb_CurrentTTItem;
	fte->ftle_EntryType = ENTRYTYPE_ToolTip_Group;
	inst->fpb_CurrentTTItem = groupTTi;

	return RETURN_OK;
}


static LONG FtEndGroup(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (fte->ftle_EntryType != ENTRYTYPE_ToolTip_Group)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: ENDGROUP outside GROUP\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;
	if (NULL == fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent)
		return RETURN_ERROR;

	inst->fpb_CurrentTTItem = fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	return RETURN_OK;
}


static LONG FtBeginMember(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *memberTTi;
	struct FileTypesListEntry *fte;
	STRPTR hideString = (STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: hide=<%s>\n", __FUNC__, __LINE__, hideString));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (fte->ftle_EntryType != ENTRYTYPE_ToolTip_Group)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: MEMBER not inside GROUP\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	memberTTi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_Member), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: memberTTi=%08lx\n", __FUNC__, __LINE__, memberTTi));

	if (NULL == memberTTi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) memberTTi->tn_User;
	fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent = inst->fpb_CurrentTTItem;
	fte->ftle_EntryType = ENTRYTYPE_ToolTip_Member;
	inst->fpb_CurrentTTItem = memberTTi;

	if (hideString && strlen(hideString) > 0)
		AddAttribute(fte, ATTRTYPE_MemberHideString, 1 + strlen(hideString), hideString);

	return RETURN_OK;
}


static LONG FtEndMember(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (ENTRYTYPE_ToolTip_Member != fte->ftle_EntryType)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR:  ENDMEMBER outside MEMBER\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;
	if (NULL == fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent)
		return RETURN_ERROR;

	inst->fpb_CurrentTTItem = fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent;

	return RETURN_OK;
}


static LONG FtString(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;
	struct MUI_NListtree_TreeNode *stringTTd;
	STRPTR idString = (STRPTR) ArgArray[0];
	STRPTR textString = (STRPTR) ArgArray[1];
	STRPTR srcString = (STRPTR) ArgArray[2];
	STRPTR textpenString = (STRPTR) ArgArray[3];
	STRPTR halignString = (STRPTR) ArgArray[4];
	STRPTR styleString = (STRPTR) ArgArray[5];
	STRPTR fontString = (STRPTR) ArgArray[6];
	STRPTR ttFontDescString = (STRPTR) ArgArray[7];
	STRPTR valignString = (STRPTR) ArgArray[8];

	d1(kprintf(__FILE__ "/%s/%ld: id=<%s>  text=<%s>  src=<%s>\n", __FUNC__, __LINE__, idString, textString, srcString));
	d1(kprintf(__FILE__ "/%s/%ld: textpen=<%s>  halign=<%s>  style=<%s>\n", __FUNC__, __LINE__, textpenString, halignString, styleString));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (ENTRYTYPE_ToolTip_Member != fte->ftle_EntryType)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR:  STRING outside MEMBER\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	if (NULL == fte->ftle_TypeEntry.ftle_ToolTipEntry.ftti_Parent)
		return RETURN_ERROR;

	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	stringTTd = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_String), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: stringTTd=%08lx\n", __FUNC__, __LINE__, stringTTd));

	if (NULL == stringTTd)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) stringTTd->tn_User;

	if (idString && strlen(idString) > 0)
		AddAttribute(fte, ATTRTYPE_StringId, 1 + strlen(idString), idString);
	else if (textString && strlen(textString) > 0)
		AddAttribute(fte, ATTRTYPE_StringText, 1 + strlen(textString), textString);
	else if (srcString && strlen(srcString) > 0)
		AddAttribute(fte, ATTRTYPE_StringSrc, 1 + strlen(srcString), srcString);

	if (halignString && strlen(halignString) > 0)
		AddAttribute(fte, ATTRTYPE_StringHAlign, 1 + strlen(halignString), halignString);
	if (valignString && strlen(valignString) > 0)
		AddAttribute(fte, ATTRTYPE_StringVAlign, 1 + strlen(valignString), valignString);
	if (styleString && strlen(styleString) > 0)
		AddAttribute(fte, ATTRTYPE_StringStyle, 1 + strlen(styleString), styleString);

	// font specification - format: "fontname.font/size"
	if (fontString && strlen(fontString) > 0)
		AddAttribute(fte, ATTRTYPE_StringFont, 1 + strlen(fontString), fontString);

	// TrueType Font Descriptor format:
	// "style/weight/size/fontname"
	if (ttFontDescString && strlen(ttFontDescString) > 0)
		AddAttribute(fte, ATTRTYPE_StringTTFont, 1 + strlen(ttFontDescString), ttFontDescString);

	if (textpenString && strlen(textpenString) > 0)
		AddAttribute(fte, ATTRTYPE_StringPen, 1 + strlen(textpenString), textpenString);
	
	fte->ftle_EntryType = ENTRYTYPE_ToolTip_String;

	return RETURN_OK;
}


static LONG FtSpace(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	ULONG *Size = (ULONG *) ArgArray[0];
	struct MUI_NListtree_TreeNode *spaceTTd;
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld: Size=%lu\n", __FUNC__, __LINE__, Size));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (ENTRYTYPE_ToolTip_Member != fte->ftle_EntryType)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: SPACE outside MEMBER\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	spaceTTd = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_Space), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: spaceTTd=%08lx\n", __FUNC__, __LINE__, spaceTTd));

	if (NULL == spaceTTd)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) spaceTTd->tn_User;

	if (Size)
		AddAttribute(fte, ATTRTYPE_SpaceSize, sizeof(*Size), Size);

	fte->ftle_EntryType = ENTRYTYPE_ToolTip_Space;

	return RETURN_OK;
}


static LONG FtImage(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	STRPTR fileName = (STRPTR) ArgArray[0];
	struct MUI_NListtree_TreeNode *imageTTd;
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld: fileName=<%s>\n", __FUNC__, __LINE__, fileName));

	if (NULL == inst->fpb_CurrentTTItem)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) inst->fpb_CurrentTTItem->tn_User;

	if (ENTRYTYPE_ToolTip_Member != fte->ftle_EntryType)
		{
		d1(kprintf(__FILE__ "/%s/%ld: RETURN_ERROR: HBAR outside MEMBER\n", __FUNC__, __LINE__));
		return RETURN_ERROR;
		}

	imageTTd = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_ToolTip_DtImage), NULL,
		inst->fpb_CurrentTTItem,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: imageTTd=%08lx\n", __FUNC__, __LINE__, imageTTd));

	if (NULL == imageTTd)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) imageTTd->tn_User;

	AddAttribute(fte, ATTRTYPE_DtImageName, 1 + strlen(fileName), fileName);

	fte->ftle_EntryType = ENTRYTYPE_ToolTip_DtImage;

	return RETURN_OK;
}

//----------------------------------------------------------------------------

static LONG FtBeginPopupMenu(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

	inst->fpb_PopupMenu = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu), NULL,
		tn,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	fte = (struct FileTypesListEntry *) inst->fpb_PopupMenu->tn_User;
	fte->ftle_EntryType = ENTRYTYPE_PopupMenu;

	return RETURN_OK;
}


static LONG FtEndPopupMenu(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	d1(kprintf(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	inst->fpb_PopupMenu = NULL;

	return RETURN_OK;
}


static LONG FtIncludeFile(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	STRPTR fileName = (STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: fileName=<%s>\n", __FUNC__, __LINE__, fileName));

	if (inst->fpb_IncludeNesting + 1 >= MAX_INCLUDE_NESTING)
		return RETURN_ERROR;

	inst->fpb_IncludeNesting++;

	return ReadFileTypeDefFile(inst, tn, fileName);
}


static LONG GetFtDescription(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
	STRPTR descName = (STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: descName=<%s>\n", __FUNC__, __LINE__, descName));

	AddAttribute(fte, ATTRTYPE_FtDescription, 1 + strlen(descName), descName);

	return RETURN_OK;
}


static LONG GetFtPvPlugin(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
	STRPTR pluginName = (STRPTR) ArgArray[0];

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: pluginName=<%s>\n", __LINE__, pluginName));

	AddAttribute(fte, ATTRTYPE_PvPluginName, 1 + strlen(pluginName), pluginName);

	return RETURN_OK;
}


static LONG AddFtMenuSeparator(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi;
	struct FileTypesListEntry *fte;

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	if (inst->fpb_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_MenuSeparator), NULL,
		inst->fpb_PopupMenu,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;
	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_MenuSeparator;

	return RETURN_OK;
}


static LONG NewFtMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi;
	struct FileTypesListEntry *fte;
	STRPTR itemName = (STRPTR) ArgArray[0];
	STRPTR commKey = (STRPTR) ArgArray[1];
	BOOL defaultAction = (BOOL) ArgArray[2];
	STRPTR SelectedIconName = (STRPTR) ArgArray[3];
	STRPTR UnselectedIconName = (STRPTR) ArgArray[4];

	d1(kprintf(__FILE__ "/%s/%ld: itemName=<%s>  commKey=<%s>\n", __FUNC__, __LINE__, itemName, commKey));

	if (NULL == inst->fpb_PopupMenu)
		{
		d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
		return RETURN_ERROR;	// not allowed outside popup menu
		}

	if (inst->fpb_CurrentMenuItem)
		{
		d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
		return RETURN_ERROR;	// not allowed inside menu item
		}

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	if (defaultAction && inst->fpb_DefaultActionSet)
		{
		d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));
		return RETURN_ERROR;	// only one DEFAULTACTION menu item allowed
		}

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	// Start new menu tree
	inst->fpb_CurrentMenuItem = ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_MenuItem), NULL,
		inst->fpb_PopupMenu,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);
	d1(kprintf(__FILE__ "/%s/%ld: ftmi=%08lx\n", __FUNC__, __LINE__, ftmi));
	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	if (defaultAction)
		{
		AddAttribute(fte, ATTRTYPE_MenuDefaultAction, 0, 0);
		inst->fpb_DefaultActionSet = TRUE;
		}
	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_MenuItem;

	if (itemName && strlen(itemName) > 0)
		AddAttribute(fte, ATTRTYPE_MenuItemName, 1 + strlen(itemName), itemName);
	if (commKey && strlen(commKey) > 0)
		AddAttribute(fte, ATTRTYPE_MenuCommKey, 1 + strlen(commKey), commKey);

	if (UnselectedIconName)
		AddAttribute(fte, ATTRTYPE_UnselIconName, 1 + strlen(UnselectedIconName), UnselectedIconName);
	if (SelectedIconName)
		AddAttribute(fte, ATTRTYPE_SelIconName, 1 + strlen(SelectedIconName), SelectedIconName);

	UpdateMenuImage(inst, OBJNDX_MainListTree, fte);

	DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_Redraw,
		ftmi,
		0);

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	return RETURN_OK;
}


static LONG EndFtMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;

	d1(kprintf(__FILE__ "/%s/%ld: UserData=%08lx\n", __FUNC__, __LINE__, ftmi->ftmi_MenuTree));

	if (NULL == ftmi)
		return RETURN_ERROR;	// only allowed if menu item pending

	inst->fpb_CurrentMenuItem = NULL;

	return RETURN_OK;
}


static LONG AddSubMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi;
	struct FileTypesListEntry *fte;
	STRPTR itemName = (STRPTR) ArgArray[0];
	STRPTR SelectedIconName = (STRPTR) ArgArray[1];
	STRPTR UnselectedIconName = (STRPTR) ArgArray[2];

	d1(kprintf(__FILE__ "/%s/%ld: itemName=<%s>\n", __FUNC__, __LINE__, itemName));

	if (inst->fpb_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	if (inst->fpb_ParentMenuItem)
		return RETURN_ERROR;	// only 1 level of submenus is allowed

	// Start new menu tree
	inst->fpb_ParentMenuItem = inst->fpb_PopupMenu;

	inst->fpb_PopupMenu = ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_SubMenu), NULL,
		inst->fpb_PopupMenu,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	d1(kprintf(__FILE__ "/%s/%ld: ftmi=%08lx\n", __FUNC__, __LINE__, ftmi));
	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_SubMenu;

	if (itemName && strlen(itemName) > 0)
		AddAttribute(fte, ATTRTYPE_MenuItemName, 1 + strlen(itemName), itemName);

	if (UnselectedIconName)
		AddAttribute(fte, ATTRTYPE_UnselIconName, 1 + strlen(UnselectedIconName), UnselectedIconName);
	if (SelectedIconName)
		AddAttribute(fte, ATTRTYPE_SelIconName, 1 + strlen(SelectedIconName), SelectedIconName);

	UpdateMenuImage(inst, OBJNDX_MainListTree, fte);

	return RETURN_OK;
}


static LONG EndSubMenuEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_ParentMenuItem;

	d1(kprintf(__FILE__ "/%s/%ld:\n", __FUNC__, __LINE__));

	if (NULL == inst->fpb_ParentMenuItem)
		return RETURN_ERROR;	// only allowed inside submenu

	if (inst->fpb_CurrentMenuItem)
		return RETURN_ERROR;	// not allowed inside menu item

	if (NULL == ftmi)
		return RETURN_ERROR;	// no submenu pending

	inst->fpb_PopupMenu = inst->fpb_ParentMenuItem;

	inst->fpb_ParentMenuItem = NULL;

	return RETURN_OK;
}


static LONG FtInternalCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>\n", __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_InternalCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_InternalCmd;

	return RETURN_OK;
}


static LONG FtWbCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>  StackSize=%08lx  wbArgs=%ld\n", __FUNC__, __LINE__, cmdName, StackSize, wbArgs));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_WbCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);
	if (StackSize)
		AddAttribute(fte, ATTRTYPE_CommandStacksize, sizeof(*StackSize), StackSize);
	if (Priority)
		AddAttribute(fte, ATTRTYPE_CommandPriority, sizeof(*Priority), Priority);
	AddAttribute(fte, ATTRTYPE_CommandWbArgs, sizeof(wbArgs), &wbArgs);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_WbCmd;

	return RETURN_OK;
}


static LONG FtARexxCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>  StackSize=%08lx  wbArgs=%ld\n", __FUNC__, __LINE__, cmdName, StackSize, wbArgs));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_ARexxCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);
	if (StackSize)
		AddAttribute(fte, ATTRTYPE_CommandStacksize, sizeof(*StackSize), StackSize);
	if (Priority)
		AddAttribute(fte, ATTRTYPE_CommandPriority, sizeof(*Priority), Priority);
	AddAttribute(fte, ATTRTYPE_CommandWbArgs, sizeof(wbArgs), &wbArgs);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_ARexxCmd;

	return RETURN_OK;
}


static LONG FtCliCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];
	ULONG *StackSize = (ULONG *) ArgArray[1];
	ULONG wbArgs = ArgArray[2];
	ULONG *Priority = (ULONG *) ArgArray[3];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>  StackSize=%08lx\n", __FUNC__, __LINE__, cmdName, StackSize));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_CliCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);
	if (StackSize)
		AddAttribute(fte, ATTRTYPE_CommandStacksize, sizeof(*StackSize), StackSize);
	if (Priority)
		AddAttribute(fte, ATTRTYPE_CommandPriority, sizeof(*Priority), Priority);
	AddAttribute(fte, ATTRTYPE_CommandWbArgs, sizeof(wbArgs), &wbArgs);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_CliCmd;

	return RETURN_OK;
}


static LONG FtPluginCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>\n", __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_PluginCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_PluginCmd;

	return RETURN_OK;
}


static LONG FtIconWindowCmd(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, LONG *ArgArray)
{
	struct MUI_NListtree_TreeNode *ftmi = inst->fpb_CurrentMenuItem;
	struct FileTypesListEntry *fte;
	CONST_STRPTR cmdName = (CONST_STRPTR) ArgArray[0];

	d1(kprintf(__FILE__ "/%s/%ld: cmdName=<%s>\n", __FUNC__, __LINE__, cmdName));

	if (NULL == ftmi)
		return RETURN_ERROR;

	ftmi = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		GetNameFromEntryType(ENTRYTYPE_PopupMenu_IconWindowCmd), NULL,
		ftmi,
		MUIV_NListtree_Insert_PrevNode_Tail,
		TNF_LIST);

	if (NULL == ftmi)
		return RETURN_ERROR;

	fte = (struct FileTypesListEntry *) ftmi->tn_User;

	if (cmdName && strlen(cmdName) > 0)
		AddAttribute(fte, ATTRTYPE_CommandName, 1 + strlen(cmdName), cmdName);

	fte->ftle_EntryType = ENTRYTYPE_PopupMenu_IconWindowCmd;

	return RETURN_OK;
}


void GenerateFileTypesNewEntry(struct FileTypesPrefsInst *inst, CONST_STRPTR Name)
{
	struct MUI_NListtree_TreeNode *ltn;
	struct FileTypesListEntry *fte;


	d1(kprintf(__FUNC__ "/%ld:  filetype <%s>  \n", __FUNC__, __LINE__, Name));

	ltn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree], 
		MUIM_NListtree_Insert,
		Name, NULL,
		MUIV_NListtree_Insert_ListNode_Root,
		MUIV_NListtree_Insert_PrevNode_Sorted,
		TNF_LIST);

	d1(kprintf(__FUNC__ "/%ld:  ltn=%08lx\n", __FUNC__, __LINE__, ltn));

	fte = (struct FileTypesListEntry *) ltn->tn_User;
	fte->ftle_EntryType = ENTRYTYPE_FileType;
}


#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static char *stpblk(const char *q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}

#endif /* !defined(__SASC) && !defined(__MORPHOS__)  */
