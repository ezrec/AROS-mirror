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
#ifdef __amigaos4__
#define USE_OLD_ANCHORPATH
#include <dos/anchorpath.h>
#endif
#include <libraries/iffparse.h>
#include <libraries/locale.h>
#include <libraries/mui.h>
#include <utility/tagitem.h>
#include <workbench/workbench.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
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
#include "Find.h"

//----------------------------------------------------------------------------

// local data structures

#define IOBUFFERLEN		480
#define NO_TYPE_NODE            ((struct TypeNode *) -1)

#ifndef __AROS__
#define	BNULL			((BPTR) 0)
#endif

//----------------------------------------------------------------------------

// local functions

static void ReloadDefIcons(struct FileTypesPrefsInst *inst, CONST_STRPTR DefIconPrefsName);
static VOID InitTypeTree(struct FileTypesPrefsInst *inst,
	struct TypeNode *parentnode, const UBYTE **desclist);
static struct TypeNode *AllocTypeNode(const UBYTE **description);
static VOID AddSon(struct TypeNode *parent,struct TypeNode *son);
static void DeleteTypeNode(struct TypeNode *tn);
static void AddFileTypesToList(struct FileTypesPrefsInst *inst);
static void AddTypeNodeToFileTypesList(struct FileTypesPrefsInst *inst, struct TypeNode *tn,
	struct MUI_NListtree_TreeNode *parentNode);
static void AddRootTypeNodeToList(struct FileTypesPrefsInst *inst, CONST_STRPTR Name);
static struct TypeNode *DefIconsIdentifyDisk(BPTR lock);
static struct TypeNode *DefIconsIdentifyProject(CONST_STRPTR Name, struct FileInfoBlock *fib);
static STRPTR DefIconsGetDeviceName(CONST_STRPTR DosDevice, ULONG *DosType);
static LONG match(CONST_STRPTR a, CONST_STRPTR b, size_t len);

//----------------------------------------------------------------------------

/* A table of clearly invalid ASCII characters (8 bits). */
static const UBYTE InvalidTab[256] =
{
	1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,
	1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//----------------------------------------------------------------------------

static struct FileTypesPrefsInst PrefsInst;

LONG InitDefIcons(void)
{
	CONST_STRPTR DefIconPrefsName = "ENV:deficons.prefs";

	CleanupDefIcons();

	PrefsInst.fpb_DefIconsInit = TRUE;

	d1(KPrintF(__FILE__ "/%s/%ld: DefIconPrefsName=<%s>\n", __FUNC__, __LINE__, DefIconPrefsName));

	ReloadDefIcons(&PrefsInst, DefIconPrefsName);

	AddFileTypesToList(&PrefsInst);

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
}


void CleanupDefIcons(void)
{
	if (PrefsInst.fpb_DefIconsInit)
		{
		if (PrefsInst.fpb_RootType)
			{
			DeleteTypeNode(PrefsInst.fpb_RootType);
			PrefsInst.fpb_RootType = NULL;
			}

		if (PrefsInst.fpb_DefIconsSegList)
			{
			UnLoadSeg(PrefsInst.fpb_DefIconsSegList);
			PrefsInst.fpb_DefIconsSegList = (BPTR)NULL;
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
        DoMethod(ListtreeFileTypes,
		MUIM_NListtree_Clear, NULL, 0);

	set(ListtreeFileTypes, MUIA_NListtree_Quiet, TRUE);

	AddTypeNodeToFileTypesList(inst, inst->fpb_RootType,
		(struct MUI_NListtree_TreeNode *) MUIV_NListtree_Insert_ListNode_Root);

	// Make sure required root entries are present
	AddRootTypeNodeToList(inst, "Project");
	AddRootTypeNodeToList(inst, "Disk");

	set(ListtreeFileTypes, MUIA_NListtree_Quiet, FALSE);

	// Activate first entry (which should be "project"
	set(ListtreeFileTypes,
		MUIA_NListtree_Active, MUIV_NListtree_Active_First);
	// For some obscure reason, the MUIA_NListtree_Active scrolls the list
	// so the first (active) entry is not visible.
	set(ListtreeFileTypes,
		MUIA_NList_First, MUIV_NList_First_Top);
}


static void AddTypeNodeToFileTypesList(struct FileTypesPrefsInst *inst, struct TypeNode *tn,
	struct MUI_NListtree_TreeNode *parentNode)
{
	while (tn)
		{
		struct MUI_NListtree_TreeNode *lnNew;

		d1(KPrintF(__FILE__ "/%s/%ld: tn=%08lx  <%s>  Next=%08lx  Son=%08lx\n", \
			__FUNC__, __LINE__, tn, tn->tn_Name, tn->tn_RightBrother, tn->tn_FirstSon));

		lnNew = (struct MUI_NListtree_TreeNode *) DoMethod(ListtreeFileTypes,
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

static void AddRootTypeNodeToList(struct FileTypesPrefsInst *inst, CONST_STRPTR Name)
{
	d1(KPrintF(__FILE__ "/%s/%ld: Name=<%s>\n", __FUNC__, __LINE__, Name));

	if (0 == DoMethod(ListtreeFileTypes,
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

/* DefIconsIdentify(BPTR dirLock, CONST_STRPTR *Name):
 *
 *	Heuristically identify the type of a file.

* IMPORTANT: this function usually returns a TypeNode *, but may also return
* one of the special values WBDISK and WBDRAWER
it returns NULL for failure
 */

struct TypeNode *DefIconsIdentify(BPTR dirLock, CONST_STRPTR Name)
{
	struct TypeNode *type = NULL;
	struct FileInfoBlock *fib = NULL;
	BPTR lock;
	BPTR oldDirLock;

	debugLock_d1(dirLock);
	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

	oldDirLock = CurrentDir(dirLock);

	if ((lock = Lock((STRPTR) Name, ACCESS_READ)) && (fib = AllocDosObject(DOS_FIB, NULL)) &&
			Examine(lock, fib))
		{
		debugLock_d1(lock);

		if (fib->fib_DirEntryType > 0)
			{
			BPTR dirlock;

			if ((dirlock = ParentDir(lock)))
				{
//				Object *IconObj;

				type = (struct TypeNode *) WBDRAWER;
				UnLock(dirlock);

/*				  // extra check for WBGARBAGE
				IconObj = NewIconObjectTags(Name,
						TAG_END);
				if (IconObj)
					{
					GetAttr(IDTA_Type, IconObj, (APTR) &type);
					DisposeIconObject(IconObj);
					}
*/
				}
			else
				{
				type = DefIconsIdentifyDisk(lock);
				}
			}
		else
			{
			type = DefIconsIdentifyProject(Name, fib);
			}
		}
	else
		{
		if (!stricmp(Name, "disk"))
			{
			BPTR curdir;

			curdir = CurrentDir((BPTR)NULL);
			CurrentDir(curdir);

			if ((lock = ParentDir(curdir)))
				UnLock(lock);
			else
				type = (struct TypeNode *)WBDISK;
			}
		}

	if (fib)
		FreeDosObject(DOS_FIB,fib);
	if (lock)
		UnLock(lock);

	CurrentDir(oldDirLock);

	d1(KPrintF("%s/%s/%ld: type=%08lx\n", __FILE__, __FUNC__, __LINE__, type));

	return(type);
}

//----------------------------------------------------------------------------

static struct TypeNode *DefIconsIdentifyDisk(BPTR lock)
{
	struct InfoData *infoData = NULL;
	struct TypeNode *type = (struct TypeNode *) WBDISK;
	STRPTR AllocatedDevName = NULL;

	debugLock_d1(lock);

	do	{
		struct TypeNode *candidate;
		struct TypeNode *tn;
		const struct DosList *VolumeNode;
		CONST_STRPTR DosDevice = "";
		CONST_STRPTR DeviceName = "";
		ULONG DosType = MAKE_ID('D','O','S',0);

		tn = PrefsInst.fpb_RootType;
		while (tn && 0 != Stricmp(tn->tn_Name, "disk"))
			tn = tn->tn_RightBrother;

		if (NULL == tn)
			break;

		infoData = malloc(sizeof(struct InfoData));
		if (NULL == infoData)
			break;

		if (!Info(lock, infoData))
			break;

		type = tn;
		d1(KPrintF("%s/%s/%ld: type=<%s>\n", __FILE__, __FUNC__, __LINE__, type->tn_Name));
		d1(KPrintF("%s/%s/%ld: id_NumBlocks=%lu  id_DiskType=%08lx  id_VolumeNode=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, \
			infoData->id_NumBlocks, infoData->id_DiskType, infoData->id_VolumeNode));

		VolumeNode = BADDR(infoData->id_VolumeNode);
		if (TypeOfMem((APTR) VolumeNode) & MEMF_PUBLIC)
			{
			d1(KPrintF("%s/%s/%ld: dol_Type=%ld  dol_Name=%08lx  dol_Task=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, VolumeNode->dol_Type, VolumeNode->dol_Name, VolumeNode->dol_Task));

			if (VolumeNode->dol_Task && (TypeOfMem(VolumeNode->dol_Task) & MEMF_PUBLIC))
				{
				struct Task *DevTask = VolumeNode->dol_Task->mp_SigTask;

				d1(KPrintF("%s/%s/%ld: dol_DiskType=%08lx\n", __FILE__, __FUNC__, __LINE__, VolumeNode->dol_misc.dol_volume.dol_DiskType));

				DosDevice = DevTask->tc_Node.ln_Name;
				d1(KPrintF("%s/%s/%ld: DevTask=<%s>\n", __FILE__, __FUNC__, __LINE__, DosDevice));

				AllocatedDevName = DefIconsGetDeviceName(DosDevice, &DosType);
				if (AllocatedDevName)
					DeviceName = AllocatedDevName;
				d1(KPrintF("%s/%s/%ld: DeviceName=<%s>\n", __FILE__, __FUNC__, __LINE__, DeviceName));
				}
			}
loop:
		candidate = type->tn_FirstSon;
loop1:
		while (candidate)
			{
			struct Magic *curr;
			BOOL matching = 1;


			curr = candidate->tn_Description;

			do
				{
				switch (curr->action)
					{
				case ACT_END:
					if (matching)
						{
						type = candidate;
						goto loop;		/* ////// */
						}
					break;

				case ACT_OR:
					if (matching)
						{
						type = candidate;
						goto loop;		/* ////// */
						}
					else
						{
						matching = 1;
						}
					break;

				case ACT_MATCH:
				case ACT_SEARCH:
				case ACT_SEARCHSKIPSPACES:
				case ACT_FILESIZE:
				case ACT_NAMEPATTERN:
				case ACT_PROTECTION:
				case ACT_ISASCII:
					matching = 0;
					break;

				case ACT_MINSIZEMB:
					if (matching)
						{
						ULONG BlocksPerMByte = (1024 * 1024) / infoData->id_BytesPerBlock;

						d1(KPrintF("%s/%s/%ld: BlocksPerMByte=%lu  id_NumBlocks=%lu\n", __FILE__, __FUNC__, __LINE__, BlocksPerMByte, infoData->id_NumBlocks));

						if ((infoData->id_NumBlocks / BlocksPerMByte) < curr->arg2)
							matching = 0;
						}
					break;

				case ACT_DOSDEVICE:
					if (matching)
						{
						UBYTE buf[50];

						matching = 0;
						if (ParsePatternNoCase(curr->str, (STRPTR) buf, sizeof(buf)) >= 0 &&
								MatchPatternNoCase((STRPTR) buf, (STRPTR) DosDevice))
							matching = 1;
						}
					break;

				case ACT_DEVICENAME:
					if (matching)
						{
						UBYTE buf[50];

						matching = 0;
						if (ParsePatternNoCase(curr->str, (STRPTR) buf, sizeof(buf)) >= 0 &&
								MatchPatternNoCase((STRPTR) buf, (STRPTR) DeviceName))
							matching = 1;
						}
					break;

				case ACT_DOSTYPE:
					if (matching)
						{
						UBYTE DosTypeString[5];

						memcpy(DosTypeString, &DosType, sizeof(DosType));
						DosTypeString[4] = '\0';

						d1(KPrintF("%s/%s/%ld: arg2=%ld  DosType=<%s>  str=<%s>\n", __FILE__, __FUNC__, __LINE__, curr->arg2, DosTypeString, curr->str));

						if (abs(curr->arg2) > sizeof(ULONG) ||
							match((CONST_STRPTR)DosTypeString, curr->str, curr->arg2))
							{
							matching = 0;
							}
						break;
						}
					break;

				case ACT_CONTENTS:
					if (matching)
						{
						LONG result;
						struct AnchorPath ap;
						BPTR OldDir = CurrentDir(lock);

						matching = 0;

						memset(&ap, 0, sizeof(ap));
						ap.ap_Flags |= APF_DOWILD;

						result = MatchFirst(curr->str, &ap);
						if (RETURN_OK == result)
							matching = 1;

						d1(KPrintF("%s/%s/%ld: str=<%s>  result=%ld\n", __FILE__, __FUNC__, __LINE__, curr->str, result));

						MatchEnd(&ap);

						CurrentDir(OldDir);
						}
					break;
					}
				} while ((curr++)->action != ACT_END);

			candidate = candidate->tn_RightBrother;
			}

		/* don't consider valid a macroclass alone */
		if (type->tn_Description->action == ACT_MACROCLASS)
			{
			candidate = type->tn_RightBrother;
			type = type->tn_Parent;
			goto loop1;		/* ////// */
			}
		} while (0);

	if (AllocatedDevName)
		free(AllocatedDevName);
	if (infoData)
		free(infoData);

	return type;
}

//----------------------------------------------------------------------------

static struct TypeNode *DefIconsIdentifyProject(CONST_STRPTR Name, struct FileInfoBlock *fib)
{
	UBYTE *Buffer = NULL;
	BPTR File = (BPTR) NULL;
	struct TypeNode *type = (struct TypeNode *) WBPROJECT;
	struct TypeNode *tn;

	tn = PrefsInst.fpb_RootType;
	while (tn && 0 != Stricmp(tn->tn_Name, "project"))
		tn = tn->tn_RightBrother;

	if (tn && (Buffer = malloc(IOBUFFERLEN)) && (File = Open((STRPTR) Name,MODE_OLDFILE)))
		{
		WORD Size;

		type = tn;

		d1(KPrintF("%s/%s/%ld: fib_FileName=<%s>  type=<%s>\n", __FILE__, __FUNC__, __LINE__, fib->fib_FileName, type->tn_Name));

		/* Read the first IOBUFFERLEN bytes. */
		if ((Size = Read(File,Buffer,IOBUFFERLEN)) >= 0)
			{
			struct TypeNode *candidate;
			const UBYTE *buf;

loop:
			candidate = type->tn_FirstSon;
loop1:
			while (candidate)
				{
				struct Magic *curr;
				BOOL matching = 1;


				curr = candidate->tn_Description;
				buf = Buffer;

				do
					{
					switch (curr->action)
						{
					case ACT_END:
						if (matching)
							{
							type = candidate;
							goto loop;		/* ////// */
							}
						break;

					case ACT_OR:
						if (matching)
							{
							type = candidate;
							goto loop;		/* ////// */
							}
						else
							{
							matching = 1;
							buf = Buffer;
							}
						break;

					case ACT_MATCH:
						if (matching)
							{
							if (Size - (buf - Buffer) < curr->arg1 + abs(curr->arg2) ||
									match((STRPTR)&buf[curr->arg1],curr->str,abs(curr->arg2)))
								matching = 0;
							}
						break;

					case ACT_SEARCH:
					case ACT_SEARCHSKIPSPACES:
						if (matching)
							{
							WORD loop;


							loop = Size - (buf - Buffer) - abs(curr->arg2);

							while (loop >= 0)
								{
								if (!match((STRPTR)buf,curr->str,curr->arg2))
									break;

								if (curr->action == ACT_SEARCHSKIPSPACES && !isspace(*buf))
									loop = -1;
								else
									{
									loop--;
									buf++;
									}
								}

							if (loop < 0) matching = 0;
							}
						break;

					case ACT_FILESIZE:
						if (matching)
							{
							if (fib->fib_Size != curr->arg2)
								matching = 0;
							}
						break;

					case ACT_NAMEPATTERN:
						if (matching)
							{
							UBYTE buf[50];

							matching = 0;
							if (ParsePatternNoCase(curr->str,(STRPTR)buf,sizeof(buf)) >= 0 &&
									MatchPatternNoCase((STRPTR)buf,fib->fib_FileName))
								matching = 1;
							}
						break;

					case ACT_PROTECTION:
						if (matching)
							{
							if ((fib->fib_Protection & curr->arg1) != curr->arg2)
								matching = 0;
							}
						break;

					case ACT_ISASCII:
						if (matching)
							{
							matching = 0;
							if (Size)
								{
								buf = &Buffer[Size];

								while (buf > Buffer && !InvalidTab[*(--buf)]);

								if (buf == Buffer) matching = 1;
								}
							}
						break;

					case ACT_MINSIZEMB:
					case ACT_DOSDEVICE:
					case ACT_DEVICENAME:
					case ACT_CONTENTS:
						matching = 0;
						break;
						}
					} while ((curr++)->action != ACT_END);

				candidate = candidate->tn_RightBrother;
				}

			/* don't consider valid a macroclass alone */
			if (type->tn_Description->action == ACT_MACROCLASS)
				{
				candidate = type->tn_RightBrother;
				type = type->tn_Parent;
				goto loop1;		/* ////// */
				}

			if (!stricmp(type->tn_Name,"iff"))
				{
				struct TypeNode *new;
				UBYTE *desc;

				if ((desc = malloc(14)))
					{
					const UBYTE *descp = desc;

					strncpy((char *)desc,(char *)&Buffer[8],4);
					desc[4] = 0;
					desc[5] = ACT_MATCH;
					desc[6] = 0;
					desc[7] = 8;
					desc[8] = 4;
					strncpy((char *)&desc[9],(char *)&Buffer[8],4);
					desc[13] = ACT_END;

					if ((new = AllocTypeNode(&descp)))
						{
						AddSon(type,new);

						type = new;
						}
					else
						free(desc);
					}
				}
			}
		}

	if (File)
		Close(File);
	if (Buffer)
		free(Buffer);
	return type;
}

//----------------------------------------------------------------------------

// DosDevice : DOS device name ("DH0"), without trailing ":"
static STRPTR DefIconsGetDeviceName(CONST_STRPTR DosDevice, ULONG *DosType)
{
	STRPTR DevName = NULL;

	do	{
		struct DosList *dl;
		struct FileSysStartupMsg *fssm;
		struct DosEnvec *env;
		CONST_STRPTR BDevName;
		size_t Length;

		dl = LockDosList(LDF_DEVICES | LDF_READ);

		dl = FindDosEntry(dl, DosDevice, LDF_DEVICES);
		if (NULL == dl)
			break;

		if (BNULL == dl->dol_misc.dol_handler.dol_Startup)
			break;

		fssm = BADDR(dl->dol_misc.dol_handler.dol_Startup);
		if (!(TypeOfMem(fssm) & MEMF_PUBLIC))
			break;

		if (BNULL == fssm->fssm_Device)
			break;

		BDevName = BADDR(fssm->fssm_Device);
		if (!(TypeOfMem((APTR) BDevName) & MEMF_PUBLIC))
			break;

		Length = BDevName[0];
		DevName = malloc(1 + Length);
		if (NULL == DevName)
			break;

		memcpy(DevName, BDevName + 1, Length);
		DevName[Length] = '\0';

		env = BADDR(fssm->fssm_Environ);
		if (NULL == env)
			break;
		if (!(TypeOfMem((APTR) env) & MEMF_PUBLIC))
			break;

		*DosType = env->de_DosType;
		} while (0);

	UnLockDosList(LDF_DEVICES | LDF_READ);

	return DevName;
}

//----------------------------------------------------------------------------

/* compare two buffers of length 'len'. These functions cannot be replaced */
/* with strn(i)cmp() because the buffers may contain NULLs */
static LONG match(CONST_STRPTR a, CONST_STRPTR b, size_t len)
{
	if (len > 0)	/* case sensitive */
		{
		while (*a == *b)
			{
			if (!(--len))
				return(0);

			a++;
			b++;
			}
		}
	else	/* ignore case */
		{
		while (toupper(*a) == toupper(*b))
			{
			if (!(++len)) return(0);

			a++;
			b++;
			}
		}

	return(*a - *b);
}

//----------------------------------------------------------------------------


