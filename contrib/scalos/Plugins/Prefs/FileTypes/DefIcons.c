// DefIcons.c
// $Date$
// $Revision$


#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/filehandler.h>
#include <dos/doshunks.h>
#include <dos/stdio.h>
#include <libraries/iffparse.h>
#include <libraries/locale.h>
#include <libraries/mui.h>
#include <utility/tagitem.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <defs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "debug.h"
#include "FileTypesPrefs.h"
#include "FileTypesPrefs_proto.h"

//----------------------------------------------------------------------------

// local data structures

#define IOBUFFERLEN		480
#define NO_TYPE_NODE            ((struct TypeNode *) -1)

#define	DEFICON_THEME_PREFIX	"THEME:DefIcons/def_"
#define	DEFICON_PATH_PREFIX	"ENV:Sys/def_"

//----------------------------------------------------------------------------

// local functions

static void ReloadDefIcons(struct FileTypesPrefsInst *inst, CONST_STRPTR DefIconPrefsName);
static VOID InitTypeTree(struct FileTypesPrefsInst *inst,
	struct TypeNode *parentnode, const UBYTE **desclist);
static struct TypeNode *AllocTypeNode(const UBYTE **description);
static VOID AddSon(struct TypeNode *parent,struct TypeNode *son);
static void DeleteTypeNode(struct TypeNode *tn);
static void AddFileTypesToList(struct FileTypesPrefsInst *inst);
static void AddTypeNodeToList(struct FileTypesPrefsInst *inst, struct TypeNode *tn);
static void AddTypeNodeToFileTypesList(struct FileTypesPrefsInst *inst, struct TypeNode *tn,
	struct MUI_NListtree_TreeNode *parentNode);
static BOOL WriteDefIconsNodes(struct FileTypesPrefsInst *inst, BPTR fh,
	struct MUI_NListtree_TreeNode *ln, ULONG *Length);
static BOOL WriteDefIconsActions(BPTR fh, const struct FileTypesEntry *fte, ULONG *Length);
static void AddRootTypeNodeToList(struct FileTypesPrefsInst *inst, CONST_STRPTR Name);

//----------------------------------------------------------------------------

// defined in DefaultDefIconsPrefs.c
extern const UBYTE DefaultDeficonsPrefs[];

//----------------------------------------------------------------------------

LONG InitDefIcons(struct FileTypesPrefsInst *inst, CONST_STRPTR DefIconPrefsName)
{
	CleanupDefIcons(inst);

	inst->fpb_DefIconsInit = TRUE;

	d1(KPrintF(__FILE__ "/%s/%ld: DefIconPrefsName=<%s>\n", __FUNC__, __LINE__, DefIconPrefsName));

	ReloadDefIcons(inst, DefIconPrefsName);

	AddFileTypesToList(inst);

	return RETURN_OK;
}


static void ReloadDefIcons(struct FileTypesPrefsInst *inst, CONST_STRPTR DefIconPrefsName)
{
	if (DefIconPrefsName)
		{
		inst->fpb_DefIconsSegList = LoadSeg(DefIconPrefsName);

		d1(KPrintF(__FILE__ "/%s/%ld: SegList=%08lx\n", __FUNC__, __LINE__, inst->fpb_DefIconsSegList));

		if (inst->fpb_DefIconsSegList)
			{
			const UBYTE *inittable;

			inittable = ((UBYTE *)BADDR(inst->fpb_DefIconsSegList))+4;
			InitTypeTree(inst, NO_TYPE_NODE, &inittable);

			d1(KPrintF(__FILE__ "/%s/%ld: RootType=%08lx\n", __FUNC__, __LINE__, inst->fpb_RootType));
			}
		}
	else
		{
		// use predefined prefs
		const UBYTE *inittable = DefaultDeficonsPrefs;

		InitTypeTree(inst, NO_TYPE_NODE, &inittable);
		}
}


void CleanupDefIcons(struct FileTypesPrefsInst *inst)
{
	if (inst->fpb_DefIconsInit)
		{
		if (inst->fpb_RootType)
			{
			DeleteTypeNode(inst->fpb_RootType);
			inst->fpb_RootType = NULL;
			}

		if (inst->fpb_DefIconsSegList)
			{
			UnLoadSeg(inst->fpb_DefIconsSegList);
			inst->fpb_DefIconsSegList = (BPTR)NULL;
			}
		}
}


static VOID InitTypeTree(struct FileTypesPrefsInst *inst,
	struct TypeNode *parentnode, const UBYTE **desclist)
{
	struct TypeNode *newTn = NULL;

	while (1)
		{
		switch(**desclist)
			{
		case TYPE_DOWN_LEVEL:
			(*desclist)++;
			InitTypeTree(inst, newTn, desclist);
			break;

		case TYPE_UP_LEVEL:
			(*desclist)++;
			/* fall thru next case */
		case TYPE_END:
			return;
			break;

		default:
			if ((newTn = AllocTypeNode(desclist)))	  /* desclist will be incremented by */
								/* AllocTypeNode */
				{
				if (parentnode != NO_TYPE_NODE)
					{
					AddSon(parentnode, newTn);
					}
				else
					{
					if (inst->fpb_RootType)
						inst->fpb_RootType->tn_RightBrother = newTn;
					else
						inst->fpb_RootType = newTn;
					}
				}
			break;
			}
		}
}


static struct TypeNode *AllocTypeNode(const UBYTE **description)
{
	struct TypeNode *nd;
	ULONG j;
	const UBYTE *arg;
	const UBYTE *name;

	j = 0;
	name = *description;
	arg = name + strlen((char *)name) + 1;
	d1(KPrintF(__FILE__ "/%s/%ld: name=<%s>\n", __FUNC__, __LINE__, name));
	while (*arg != ACT_END)
	{
		j++;
		switch(*(arg++))
		{
		case ACT_MATCH:
			arg = arg + 3 + abs((BYTE)arg[2]);
			break;
		case ACT_SEARCH:
		case ACT_SEARCHSKIPSPACES:
			arg = arg + 1 + abs((BYTE)arg[0]);
			break;
		case ACT_PROTECTION:
			arg += 8;
			break;
		case ACT_FILESIZE:
		case ACT_MINSIZEMB:
			arg += 4;
			break;
		case ACT_NAMEPATTERN:
		case ACT_DOSDEVICE:
		case ACT_DEVICENAME:
		case ACT_CONTENTS:
			arg = arg + strlen((char *)arg) + 1;
			break;
		case ACT_DOSTYPE:
			arg = arg + 1 + arg[0];
			break;
		}
	}

	if ((nd = calloc(sizeof(struct TypeNode) + j * sizeof(struct Magic), 1)))
		{
		nd->tn_Name = (STRPTR) name;

		j = 0;
		arg = name + strlen((char *)name) + 1;
		while (*arg != ACT_END)
			{
			switch(nd->tn_Description[j].action = *(arg++))
				{
			case ACT_MATCH:
				nd->tn_Description[j].arg1 = *(arg++);
				nd->tn_Description[j].arg1 <<= 8;
				nd->tn_Description[j].arg1 |= *(arg++);
				nd->tn_Description[j].arg2 = (BYTE)*(arg++);
				nd->tn_Description[j].str = (STRPTR) arg;
				arg += abs(nd->tn_Description[j].arg2);
				break;
			case ACT_SEARCH:
			case ACT_SEARCHSKIPSPACES:
				nd->tn_Description[j].arg2 = (BYTE)*(arg++);
				nd->tn_Description[j].str = (STRPTR) arg;
				arg += abs(nd->tn_Description[j].arg2);
				break;
			case ACT_FILESIZE:
			case ACT_MINSIZEMB:
				nd->tn_Description[j].arg2 = *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				break;
			case ACT_DOSTYPE:
				nd->tn_Description[j].arg2 = (BYTE)*(arg++);
				nd->tn_Description[j].str = (STRPTR) arg;
				arg += nd->tn_Description[j].arg2;
				d1(KPrintF(__FILE__ "/%s/%ld: arg2=%ld  str=<%s>\n", __FUNC__, __LINE__, nd->tn_Description[j].arg2, nd->tn_Description[j].str));
				break;
			case ACT_NAMEPATTERN:
			case ACT_DOSDEVICE:
			case ACT_DEVICENAME:
			case ACT_CONTENTS:
				nd->tn_Description[j].str = (STRPTR) arg;
				arg += strlen((char *)arg) + 1;
				break;
			case ACT_PROTECTION:
				nd->tn_Description[j].arg1 = *(arg++);
				nd->tn_Description[j].arg1 <<= 8;
				nd->tn_Description[j].arg1 |= *(arg++);
				nd->tn_Description[j].arg1 <<= 8;
				nd->tn_Description[j].arg1 |= *(arg++);
				nd->tn_Description[j].arg1 <<= 8;
				nd->tn_Description[j].arg1 |= *(arg++);
				nd->tn_Description[j].arg2 = *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				nd->tn_Description[j].arg2 <<= 8;
				nd->tn_Description[j].arg2 |= *(arg++);
				break;
			}

			j++;
			}
		nd->tn_Description[j].action = ACT_END;
		}

	*description = arg+1;	/* increment *description for recursion */

	return(nd);
}



static VOID AddSon(struct TypeNode *parent,struct TypeNode *son)
{
	struct TypeNode *nd;

	if (!parent || !son)
		return;

	if ((nd = parent->tn_FirstSon))
		{
		while (nd->tn_RightBrother)
			nd = nd->tn_RightBrother;

		nd->tn_RightBrother = son;
		}
	else
		parent->tn_FirstSon = son;

	son->tn_Parent = parent;
}


static void DeleteTypeNode(struct TypeNode *tn)
{
	while (tn)
		{
		struct TypeNode *tnNext = tn->tn_RightBrother;

		DeleteTypeNode(tn->tn_FirstSon);

		free(tn);

		tn = tnNext;
		}
}


static void AddFileTypesToList(struct FileTypesPrefsInst *inst)
{
        DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_Clear, NULL, 0);

	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Quiet, TRUE);

	AddTypeNodeToFileTypesList(inst, inst->fpb_RootType,
		(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root);

	// Make sure required root entries are present
	AddRootTypeNodeToList(inst, "Project");
	AddRootTypeNodeToList(inst, "Disk");

	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Quiet, FALSE);

	// Activate first entry (which should be "project"
	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIA_NListtree_Active, MUIV_NListtree_Active_First);
	// For some obscure reason, the MUIA_NListtree_Active scrolls the list
	// so the first (active) entry is not visible.
	set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIA_NList_First, MUIV_NList_First_Top);

	if (inst->fpb_FileTypesDirLock)
		{
		BPTR oldDir;

		oldDir = CurrentDir(inst->fpb_FileTypesDirLock);

		GenerateFileTypesNewEntry(inst, "disk");
		GenerateFileTypesNewEntry(inst, "drawer");
		GenerateFileTypesNewEntry(inst, "trashcan");
		GenerateFileTypesNewEntry(inst, "project");
		GenerateFileTypesNewEntry(inst, "tool");
		GenerateFileTypesNewEntry(inst, "appicon");

		AddTypeNodeToList(inst, inst->fpb_RootType);

		CurrentDir(oldDir);
		}
}


static void AddTypeNodeToList(struct FileTypesPrefsInst *inst, struct TypeNode *tn)
{
	while (tn)
		{
		// avoid duplicate entries
		if (0 == DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_FindName,
			MUIV_NListtree_FindName_ListNode_Root,
			tn->tn_Name,
			0))
			{
			GenerateFileTypesNewEntry(inst, (STRPTR) tn->tn_Name);
			}

		AddTypeNodeToList(inst, tn->tn_FirstSon);

		tn = tn->tn_RightBrother;
		}
}


static void AddTypeNodeToFileTypesList(struct FileTypesPrefsInst *inst, struct TypeNode *tn,
	struct MUI_NListtree_TreeNode *parentNode)
{
	while (tn)
		{
		struct MUI_NListtree_TreeNode *lnNew;

		d1(KPrintF(__FILE__ "/%s/%ld: tn=%08lx  <%s>  Next=%08lx  Son=%08lx\n", \
			__FUNC__, __LINE__, tn, tn->tn_Name, tn->tn_RightBrother, tn->tn_FirstSon));

		lnNew = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_Insert,
			tn->tn_Name,
			tn,
			parentNode,
			MUIV_NListtree_Insert_PrevNode_Tail,
			TNF_LIST | TNF_OPEN);

		d1(KPrintF(__FILE__ "/%s/%ld: parentNode=%08lx  lnNew=%08lx\n", __FUNC__, __LINE__, parentNode, lnNew));

		if (tn->tn_FirstSon)
			AddTypeNodeToFileTypesList(inst, tn->tn_FirstSon, lnNew);

		tn = tn->tn_RightBrother;
		}
}

//----------------------------------------------------------------------------

BOOL WriteDefIconsPrefs(struct FileTypesPrefsInst *inst, CONST_STRPTR FileName)
{
	static ULONG DefIconHeader[8];
	static ULONG DefIconTrailer[1];
	BPTR fh;
	BOOL Success = FALSE;

	DefIconHeader[0] = SCA_LONG2BE(HUNK_HEADER);
	DefIconHeader[1] = 0;
	DefIconHeader[2] = SCA_LONG2BE(1);
	DefIconHeader[3] = 0;
	DefIconHeader[4] = 0;
	DefIconHeader[5] = 0;		// HunkLength must be written here
	DefIconHeader[6] = SCA_LONG2BE(HUNK_DATA);
	DefIconHeader[7] = 0;		// HunkLength must be written here

	DefIconTrailer[0] = SCA_LONG2BE(HUNK_END);

	do	{
		ULONG HunkLength = 0;
		ULONG HunkLengthBE = 0; // Hunk length in big endian
		struct MUI_NListtree_TreeNode *ln;

		fh = Open(FileName, MODE_NEWFILE);
		if ((BPTR)NULL == fh)
			break;

		if (0 != SetVBuf(fh, NULL, BUF_FULL, 2048))
			break;

		// write HUNK_HEADER and HUNK_DATA
		if (1 != FWrite(fh, (APTR) DefIconHeader, sizeof(DefIconHeader), 1))
			break;

		// get the very first root level node
		ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_GetEntry_ListNode_Root,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		if (NULL == ln)
			break;

		do	{
			if (!WriteDefIconsNodes(inst, fh, ln, &HunkLength))
				break;

			// get next root level node
			ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIM_NListtree_GetEntry,
				MUIV_NListtree_GetEntry_ListNode_Root,
				MUIV_NListtree_GetEntry_Position_Next,
				MUIV_NListtree_GetEntry_Flag_SameLevel);
			} while (ln);

		FPutC(fh, TYPE_END);
		HunkLength++;

		// pad with 0x00 until length is multiple of sizeof(ULONG)
		while (HunkLength & 0x03)
			{
			d1(KPrintF(__FILE__ "/%s/%ld: HunkLength=%lu\n", __FUNC__, __LINE__, HunkLength));
			FPutC(fh, 0);
			HunkLength++;
			}

		// write HUNK_END
		if (1 != FWrite(fh, (APTR) DefIconTrailer, sizeof(DefIconTrailer), 1))
			break;

		HunkLength /= sizeof(ULONG);

		// Write hunk length in H_HEADER
		Seek(fh, 5 * sizeof(ULONG), OFFSET_BEGINNING);

		HunkLengthBE = SCA_LONG2BE(HunkLength);
		if (1 != FWrite(fh, &HunkLengthBE, sizeof(HunkLengthBE), 1))
			break;

		// Write hunk length in H_DATA
		Seek(fh, 7 * sizeof(ULONG), OFFSET_BEGINNING);

		HunkLengthBE = SCA_LONG2BE(HunkLength);
		if (1 != FWrite(fh, &HunkLengthBE, sizeof(HunkLengthBE), 1))
			break;

		Success = TRUE;
		} while (0);

	if (fh)
		Close(fh);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL WriteDefIconsNodes(struct FileTypesPrefsInst *inst, BPTR fh,
	struct MUI_NListtree_TreeNode *ln, ULONG *Length)
{
	BOOL Success = TRUE;

	while (ln && Success)
		{
		const struct FileTypesEntry *fte = (const struct FileTypesEntry *) ln->tn_User;
		struct MUI_NListtree_TreeNode *lnSub;
		ULONG len;

		// write TypeNode name, including trailing "\0"
		len = 1 + strlen((char *)fte->fte_TypeNode.tn_Name);
		if (1 != FWrite(fh, fte->fte_TypeNode.tn_Name, len, 1))
			{
			Success = FALSE;
			break;
			}
		*Length += len;

		if (!WriteDefIconsActions(fh, fte, Length))
			{
			Success = FALSE;
			break;
			}

		lnSub = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			ln,
			MUIV_NListtree_GetEntry_Position_Head,
			0);
		if (lnSub)
			{
			FPutC(fh, TYPE_DOWN_LEVEL);
			(*Length)++;

			if (!WriteDefIconsNodes(inst, fh, lnSub, Length))
				{
				Success = FALSE;
				break;
				}

			FPutC(fh, TYPE_UP_LEVEL);
			(*Length)++;
			}

		// get next node
		ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			ln,
			MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);
		}

	return Success;
}

//----------------------------------------------------------------------------

static BOOL WriteDefIconsActions(BPTR fh, const struct FileTypesEntry *fte, ULONG *Length)
{
	BOOL Success = TRUE;
	struct FileTypesActionListEntry *ftale;

	for (ftale = (struct FileTypesActionListEntry *) fte->fte_MagicList.lh_Head;
		Success && ftale != (struct FileTypesActionListEntry *) &fte->fte_MagicList.lh_Tail;
		ftale = (struct FileTypesActionListEntry *) ftale->ftale_Node.ln_Succ)
		{
		ULONG len;

		d1(KPrintF(__FILE__ "/%s/%ld: ftale=%08lx  action=%ld\n", \
			__FUNC__, __LINE__, ftale, ftale->ftale_Magic.action));

		FPutC(fh, ftale->ftale_Magic.action);
		(*Length)++;

		switch (ftale->ftale_Magic.action)
			{
		case ACT_SEARCH:
		case ACT_SEARCHSKIPSPACES:
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg2);
			(*Length)++;
			len = abs(ftale->ftale_Magic.arg2);
			if (1 != FWrite(fh, ftale->ftale_Magic.str, len, 1))
				{
				Success = FALSE;
				break;
				}
			*Length += len;
			break;
		case ACT_MATCH:
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg1 >> 8));
			(*Length)++;
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg1);
			(*Length)++;
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg2);
			(*Length)++;

			len = abs(ftale->ftale_Magic.arg2);
			if (1 != FWrite(fh, ftale->ftale_Magic.str, len, 1))
				{
				Success = FALSE;
				break;
				}
			*Length += len;
			break;
		case ACT_NAMEPATTERN:
		case ACT_DOSDEVICE:
		case ACT_DEVICENAME:
		case ACT_CONTENTS:
			len = 1 + strlen(ftale->ftale_Magic.str);
			if (1 != FWrite(fh, ftale->ftale_Magic.str, len, 1))
				{
				Success = FALSE;
				break;
				}
			*Length += len;
			break;
		case ACT_PROTECTION:
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg1 >> 24));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg1 >> 16));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg1 >> 8));
			(*Length)++;
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg1);
			(*Length)++;

			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 24));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 16));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 8));
			(*Length)++;
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg2);
			(*Length)++;
			break;
		case ACT_FILESIZE:
		case ACT_MINSIZEMB:
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 24));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 16));
			(*Length)++;
			FPutC(fh, (UBYTE) (ftale->ftale_Magic.arg2 >> 8));
			(*Length)++;
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg2);
			(*Length)++;
			break;
		case ACT_DOSTYPE:
			FPutC(fh, (UBYTE) ftale->ftale_Magic.arg2);
			(*Length)++;

			len = ftale->ftale_Magic.arg2;
			if (1 != FWrite(fh, ftale->ftale_Magic.str, len, 1))
				{
				Success = FALSE;
				break;
				}
			*Length += len;
			break;
		default:
		case ACT_END:
		case ACT_ISASCII:
		case ACT_OR:
		case ACT_MACROCLASS:
			// no additional parameters here
			break;
			}
		}

	FPutC(fh, ACT_END);
	(*Length)++;

	return Success;
}

//----------------------------------------------------------------------------

static void AddRootTypeNodeToList(struct FileTypesPrefsInst *inst, CONST_STRPTR Name)
{
	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>\n", __FUNC__, __LINE__, Name));

	if (0 == DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_FindName,
		MUIV_NListtree_FindName_ListNode_Root,
		Name,
		MUIV_NListtree_FindName_Flag_SameLevel))
		{
		struct TypeNode tn;

		d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s> not found, creating new\n", __FUNC__, __LINE__, Name));

		tn.tn_RightBrother = tn.tn_FirstSon = tn.tn_Parent = NULL;
		tn.tn_IconObject = NULL;
		tn.tn_Name = (STRPTR) Name;
		tn.tn_Description[0].action = ACT_END;

		AddTypeNodeToFileTypesList(inst, &tn,
			(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root);
		}
}

//----------------------------------------------------------------------------


