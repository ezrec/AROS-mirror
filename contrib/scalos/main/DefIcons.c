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
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/filehandler.h>
#ifdef __amigaos4__
#include <dos/anchorpath.h>
#endif
#include <libraries/iffparse.h>
#include <libraries/locale.h>
#include <intuition/intuitionbase.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <utility/tagitem.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/wb.h>
#include <proto/icon.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

#define IOBUFFERLEN		480
#define NO_TYPE_NODE            ((struct TypeNode *) -1)

#define	DEFICON_THEME_PREFIX	"THEME:DefIcons/def_"
#define	DEFICON_PATH_PREFIX	"def_"

//----------------------------------------------------------------------------

// local functions

static void ReloadDefIcons(void);

static VOID InitTypeTree(struct TypeNode *parentnode,STRPTR *desclist);
static struct TypeNode *AllocTypeNode(STRPTR *description);
static VOID AddSon(struct TypeNode *parent,struct TypeNode *son);

static LONG match(CONST_STRPTR a, CONST_STRPTR b, size_t len);
static struct TypeNode *DefIconsIdentifyDisk(BPTR lock);
static struct TypeNode *DefIconsIdentifyProject(CONST_STRPTR Name, T_ExamineData *fib);
static void DeleteTypeNode(struct TypeNode *tn);
static Object *ReadDefIconObjectForName(ULONG IconType, CONST_STRPTR TypeName,
	BPTR dirLock, CONST_STRPTR OriginalName, struct TagItem *TagList);
static Object *ReadDefIconObjectForNameFallback(ULONG IconType, CONST_STRPTR TypeName,
	BPTR dirLock, CONST_STRPTR OriginalName, struct TagItem *TagList);
static Object *CloneDefIconObject(Object *IconObj, BPTR dirLock, 
	CONST_STRPTR OriginalName, struct TagItem *TagList);
static STRPTR DefIconsGetDeviceName(CONST_STRPTR DosDevice, ULONG *DosType);

//----------------------------------------------------------------------------

// local data items

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

static struct TypeNode *RootType = NULL;
static BPTR seglist = (BPTR)NULL;
static BOOL DefIconsInit = FALSE;
static ULONG DefIconsPrefsCRC = 0;

static Object *CachedDefIconObjects[1 + WBAPPICON];

//----------------------------------------------------------------------------

// public data items
SCALOSSEMAPHORE DefIconsSemaphore;
SCALOSSEMAPHORE DefIconsCacheSemaphore;

//----------------------------------------------------------------------------

LONG InitDefIcons(void)
{
	ScalosInitSemaphore(&DefIconsSemaphore);
	ScalosInitSemaphore(&DefIconsCacheSemaphore);

	DefIconsInit = TRUE;

	ReloadDefIcons();

	return RETURN_OK;
}


static void ReloadDefIcons(void)
{
	static CONST_STRPTR DefIconsPrefsFileName = "ENV:deficons.prefs";

	CleanupDefIcons();

	ScalosObtainSemaphore(&DefIconsSemaphore);

	DefIconsPrefsCRC = GetPrefsCRCFromName(DefIconsPrefsFileName);
	seglist = LoadSeg((STRPTR) DefIconsPrefsFileName);

	d1(kprintf("%s/%s/%ld: SegList=%08lx\n", __FILE__, __FUNC__, __LINE__, seglist));

	if (seglist)
		{
		STRPTR inittable;

		inittable = ((STRPTR)BADDR(seglist))+4;
		InitTypeTree(NO_TYPE_NODE, &inittable);

		d1(kprintf("%s/%s/%ld: RootType=%08lx\n", __FILE__, __FUNC__, __LINE__, RootType));
		}

	ScalosReleaseSemaphore(&DefIconsSemaphore);
}


void CleanupDefIcons(void)
{
	ULONG n;

	if (DefIconsInit)
		{
		ScalosObtainSemaphore(&DefIconsSemaphore);

		DeleteTypeNode(RootType);
		RootType = NULL;

		if (seglist)
			{
			UnLoadSeg(seglist);
			seglist = (BPTR)NULL;
			}

		ScalosReleaseSemaphore(&DefIconsSemaphore);
		}

	// Cleanup cached DefIcons
	ScalosObtainSemaphore(&DefIconsCacheSemaphore);
	for (n = 0; n < Sizeof(CachedDefIconObjects); n++)
		{
		if (CachedDefIconObjects[n])
			{
			DisposeIconObject(CachedDefIconObjects[n]);
			CachedDefIconObjects[n] = NULL;
			}
		}
	ScalosReleaseSemaphore(&DefIconsCacheSemaphore);
}


void NewDefIconsPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_DEFICONSPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_DEFICONSPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}
}


BOOL ChangedDefIconsPrefs(void)
{
	ULONG NewDefIconsPrefsCRC;
	BOOL Changed = FALSE;

	NewDefIconsPrefsCRC = GetPrefsCRCFromName("ENV:deficons.prefs");
	d1(kprintf("%s/%s/%ld: NewDefIconsPrefsCRC=%08lx  DefIconsPrefsCRC=%08lx\n",
		__FILE__, __FUNC__, __LINE__, NewDefIconsPrefsCRC, DefIconsPrefsCRC));

	if (NewDefIconsPrefsCRC != DefIconsPrefsCRC)
		{
		struct ScaWindowStruct *ws;

		Changed = TRUE;

		// Remove all references to DefIcons structure members
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_IconWin_ClearIconFileTypes);
			}
		SCA_UnLockWindowList();

		FileTypeFlush(FALSE);
		ReloadDefIcons();
		}

	return Changed;
}


static VOID InitTypeTree(struct TypeNode *parentnode,STRPTR *desclist)
{
	struct TypeNode *newTn = NULL;

	while (1)
		{
		switch(**desclist)
			{
		case TYPE_DOWN_LEVEL:
			(*desclist)++;
			InitTypeTree(newTn, desclist);
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
					if (RootType)
						RootType->tn_RightBrother = newTn;
					else
						RootType = newTn;
					}
				}
			break;
			}
		}
}



static struct TypeNode *AllocTypeNode(STRPTR *description)
{
	struct TypeNode *nd;
	ULONG j;
	STRPTR arg;
	STRPTR name;

	j = 0;
	name = *description;
	arg = name + strlen(name) + 1;
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
			arg = arg + strlen(arg) + 1;
			break;
		case ACT_DOSTYPE:
			arg = arg + 1 + arg[0];
			break;
		}
	}

	if ((nd = ScalosAlloc(sizeof(struct TypeNode) + j * sizeof(struct Magic))))
		{
		nd->tn_RightBrother = nd->tn_FirstSon = nd->tn_Parent = NULL;
		nd->tn_IconObject = NULL;
		nd->tn_Name = name;

		j = 0;
		arg = name + strlen(name) + 1;
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
				nd->tn_Description[j].str = arg;
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
				break;
			case ACT_NAMEPATTERN:
			case ACT_DOSDEVICE:
			case ACT_DEVICENAME:
			case ACT_CONTENTS:
				nd->tn_Description[j].str = arg;
				arg += strlen(arg) + 1;
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



/* DefIconsIdentify(BPTR dirLock, CONST_STRPTR *Name, ULONG IconType):
 *
 *	Heuristically identify the type of a file.

* IMPORTANT: this function usually returns a TypeNode *, but may also return
* one of the special values WBDISK and WBDRAWER
it returns NULL for failure
 */

struct TypeNode *DefIconsIdentify(BPTR dirLock, CONST_STRPTR Name, ULONG IconType)
{
	struct TypeNode *type = NULL;
	T_ExamineData *fib = NULL;
	BPTR lock;
	BPTR oldDirLock;

	debugLock_d1(dirLock);
	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

	oldDirLock = CurrentDir(dirLock);

	if ((lock = Lock((STRPTR) Name, ACCESS_READ)) && ScalosExamineBegin(&fib) &&
			ScalosExamineLock(lock, &fib))
		{
		debugLock_d1(lock);

		if (ScalosExamineIsDrawer(fib))
			{
			BPTR dirlock;

			if ((dirlock = ParentDir(lock)))
				{
				Object *IconObj;

				type = (struct TypeNode *) WBDRAWER;
				UnLock(dirlock);

				// extra check for WBGARBAGE
				IconObj = NewIconObjectTags(Name,
						TAG_END);
				if (IconObj)
					{
					GetAttr(IDTA_Type, IconObj, (APTR) &type);
					DisposeIconObject(IconObj);
					}
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
		d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

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
		else if (ICONTYPE_NONE != IconType)
			{
			type = (struct TypeNode *) IconType;
			}
		}

	ScalosExamineEnd(&fib);
	if (lock)
		UnLock(lock);

	CurrentDir(oldDirLock);

	d1(KPrintF("%s/%s/%ld: type=%08lx\n", __FILE__, __FUNC__, __LINE__, type));

	return(type);
}


static struct TypeNode *DefIconsIdentifyDisk(BPTR lock)
{
	struct InfoData *infoData = NULL;
	struct TypeNode *type = (struct TypeNode *) WBDISK;
	STRPTR AllocatedDevName = NULL;

	debugLock_d1(lock);

	ScalosObtainSemaphoreShared(&DefIconsSemaphore);

	do	{
		struct TypeNode *candidate;
		struct TypeNode *tn;
		const struct DosList *VolumeNode;
		CONST_STRPTR DosDevice = "";
		CONST_STRPTR DeviceName = "";
		ULONG DosType = MAKE_ID('D','O','S',0);

		tn = RootType;
		while (tn && 0 != Stricmp(tn->tn_Name, "disk"))
			tn = tn->tn_RightBrother;

		if (NULL == tn)
			break;

		infoData = ScalosAllocInfoData();
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
						UBYTE buf[150];

						matching = 0;
						if (ParsePatternNoCase(curr->str, (STRPTR) buf, sizeof(buf)) >= 0 &&
								MatchPatternNoCase((STRPTR) buf, (STRPTR) DosDevice))
							matching = 1;
						}
					break;

				case ACT_DEVICENAME:
					if (matching)
						{
						UBYTE buf[150];

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
						struct AnchorPath *ap;
						BPTR OldDir = CurrentDir(lock);

						matching = 0;

						ap = ScalosAllocAnchorPath(APF_DOWILD, 0);
						if (ap)
							{
							result = MatchFirst(curr->str, ap);
							if (RETURN_OK == result)
								matching = 1;

							d1(KPrintF("%s/%s/%ld: str=<%s>  result=%ld\n", __FILE__, __FUNC__, __LINE__, curr->str, result));

							MatchEnd(ap);
							ScalosFreeAnchorPath(ap);
							}

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

	ScalosReleaseSemaphore(&DefIconsSemaphore);

	if (AllocatedDevName)
		ScalosFree(AllocatedDevName);
	ScalosFreeInfoData(&infoData);

	return type;
}


static struct TypeNode *DefIconsIdentifyProject(CONST_STRPTR Name, T_ExamineData *fib)
{
	UBYTE *Buffer = NULL;
	BPTR File = (BPTR) NULL;
	struct TypeNode *type = (struct TypeNode *) WBPROJECT;
	struct TypeNode *tn;

	ScalosObtainSemaphoreShared(&DefIconsSemaphore);

	tn = RootType;
	while (tn && 0 != Stricmp(tn->tn_Name, "project"))
		tn = tn->tn_RightBrother;

	if (tn && (Buffer = ScalosAlloc(IOBUFFERLEN)) && (File = Open((STRPTR) Name,MODE_OLDFILE)))
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
							if (0 != Cmp64(ScalosExamineGetSize(fib), MakeU64(curr->arg2)))
							//if (fib->fib_Size != curr->arg2)
								matching = 0;
							}
						break;

					case ACT_NAMEPATTERN:
						if (matching)
							{
							UBYTE buf[150];

							matching = 0;
							if (ParsePatternNoCase(curr->str,(STRPTR)buf,sizeof(buf)) >= 0 &&
									MatchPatternNoCase((STRPTR)buf, (STRPTR)ScalosExamineGetName(fib)))
								matching = 1;
							}
						break;

					case ACT_PROTECTION:
						if (matching)
							{
							if ((ScalosExamineGetProtection(fib) & curr->arg1) != curr->arg2)
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
				STRPTR desc;


				if ((desc = ScalosAlloc(14)))
					{
					STRPTR descp = desc;

					strncpy(desc,(char *)&Buffer[8],4);
					desc[4] = 0;
					desc[5] = ACT_MATCH;
					desc[6] = 0;
					desc[7] = 8;
					desc[8] = 4;
					strncpy(&desc[9],(char *)&Buffer[8],4);
					desc[13] = ACT_END;

					if ((new = AllocTypeNode(&descp)))
						{
						AddSon(type,new);

						type = new;
						}
					else
						ScalosFree(desc);
					}
				}
			}
		}

	ScalosReleaseSemaphore(&DefIconsSemaphore);

	if (File)
		Close(File);
	if (Buffer)
		ScalosFree(Buffer);
	return type;
}


static void DeleteTypeNode(struct TypeNode *tn)
{
	while (tn)
		{
		struct TypeNode *tnNext = tn->tn_RightBrother;

		DeleteTypeNode(tn->tn_FirstSon);

		if (tn->tn_IconObject)
			{
			DisposeIconObject(tn->tn_IconObject);
			tn->tn_IconObject = NULL;
			}
		ScalosFree(tn);

		tn = tnNext;
		}
}


Object *ReturnDefIconObjTags(BPTR dirLock, CONST_STRPTR Name, ULONG FirstTag, ...)
{
	Object *o = NULL;
	struct TagItem *TagList;
	va_list args;

	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);

	if (TagList)
		{
		o = ReturnDefIconObj(dirLock, Name, TagList);
		FreeTagItems(TagList);
		}

	va_end(args);

	return o;
}

Object *ReturnDefIconObj(BPTR dirLock, CONST_STRPTR Name, struct TagItem *TagList)
{
	struct TypeNode *tn;
	Object *IconObj = NULL;

	if (Name && strlen(Name) > 0)
		tn = DefIconsIdentify(dirLock, Name, GetTagData(IDTA_Type, ICONTYPE_NONE, TagList));
	else
		tn = DefIconsIdentifyDisk(dirLock);

	d1(KPrintF("%s/%s/%ld: tn=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, tn, Name));
	debugLock_d1(dirLock);

	switch ((ULONG) tn)
		{
	case WBDISK:
		IconObj = ReadDefIconObjectForNameFallback(WBDISK,
			"disk", dirLock, Name, TagList);
		d1(KPrintF("%s/%s/%ld: WBDISK IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		break;

	case WBDRAWER:
		IconObj = ReadDefIconObjectForNameFallback(WBDRAWER,
			"drawer", dirLock, Name, TagList);
		d1(KPrintF("%s/%s/%ld: WBDRAWER IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		break;

	case WBAPPICON:
		IconObj = ReadDefIconObjectForNameFallback(WBAPPICON,
			"appicon", dirLock, Name, TagList);
		d1(KPrintF("%s/%s/%ld: WBAPPICON IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		if (NULL == IconObj)
			{
			IconObj = ReadDefIconObjectForNameFallback(WBPROJECT,
				"project", dirLock, Name, TagList);
			}
		d1(KPrintF("%s/%s/%ld: WBAPPICON IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		break;

	case ICONTYPE_NONE:
		IconObj = ReadDefIconObjectForNameFallback(WBPROJECT,
			"project", dirLock, Name, TagList);
		d1(KPrintF("%s/%s/%ld: ??? IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		break;

	default:
		if (IS_TYPENODE(tn))
			{
			d1(KPrintF("%s/%s/%ld: tn=%08lx  tn_IconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, tn, tn->tn_IconObject));
			if (tn->tn_IconObject)
				{
				// We have a cached iconObject, try to clone it now
				IconObj = CloneDefIconObject(tn->tn_IconObject, dirLock, Name, TagList);
				d1(KPrintF("%s/%s/%ld: CloneDefIconObject returned IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
				}

			d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
			if (NULL == IconObj)
				{
				struct TypeNode *tn2 = tn;

				while (tn2 && NULL == IconObj)
					{
					IconObj = ReadDefIconObjectForName(WBPROJECT,
						tn2->tn_Name, dirLock, Name, TagList);

					d1(KPrintF("%s/%s/%ld: IconObj=%08lx  tn2=%08lx <%s>  Parent=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, IconObj, tn2, tn2->tn_Name, tn2->tn_Parent));
					tn2 = tn2->tn_Parent;
					}

				d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
				tn->tn_IconObject = IconObj;
				if (tn->tn_IconObject)
					{
					IconObj = CloneDefIconObject(tn->tn_IconObject, dirLock, Name, TagList);
					d1(KPrintF("%s/%s/%ld: CloneDefIconObject returned IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
					if (NULL == IconObj)
						{
						// Cloning failed
						IconObj = tn->tn_IconObject;
						tn->tn_IconObject = NULL;
						}
					}
				}
			}
		if (NULL == IconObj)
			{
			IconObj = GetDefIconObjectTags(WBPROJECT,
				IDTA_Text, (ULONG) Name,
				TAG_MORE, (ULONG) TagList,
				TAG_END);
			}
		break;
		}

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	return IconObj;
}


static Object *ReadDefIconObjectForName(ULONG IconType, CONST_STRPTR TypeName,
	BPTR dirLock, CONST_STRPTR OriginalName, struct TagItem *TagList)
{
	STRPTR DefIconPath;
	Object *IconObj = NULL;

	DefIconPath = AllocPathBuffer();
	d1(KPrintF("%s/%s/%ld: DefIconPath=%08lx  TypeName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		DefIconPath, TypeName));
	if (DefIconPath)
		{
		struct WBArg OriginalLocation;

		strcpy(DefIconPath, DEFICON_THEME_PREFIX);
		SafeStrCat(DefIconPath, TypeName, Max_PathLen);

		OriginalLocation.wa_Lock = dirLock;
		OriginalLocation.wa_Name = (STRPTR) OriginalName;

		d1(KPrintF("%s/%s/%ld: DefIconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, DefIconPath));

		IconObj = (Object *) NewIconObjectTags(DefIconPath,
			IDTA_IconLocation, (ULONG) &OriginalLocation,
			IDTA_SupportedIconTypes, GetTagData(IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes, TagList),
			IDTA_SizeConstraints, GetTagData(IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints, TagList),
			IDTA_ScalePercentage, GetTagData(IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor, TagList),
			TagList ? TAG_MORE : TAG_IGNORE, (ULONG) TagList,
			TAG_END);

		d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

		if (NULL == IconObj)
			{
			stccpy(DefIconPath, CurrentPrefs.pref_DefIconPath, Max_PathLen);
			AddPart(DefIconPath, DEFICON_PATH_PREFIX, Max_PathLen);
			SafeStrCat(DefIconPath, TypeName, Max_PathLen);

			d1(KPrintF("%s/%s/%ld: DefIconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, DefIconPath));

			IconObj = (Object *) NewIconObjectTags(DefIconPath,
				IDTA_IconLocation, (ULONG) &OriginalLocation,
				IDTA_SupportedIconTypes, GetTagData(IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes, TagList),
				IDTA_SizeConstraints, GetTagData(IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints, TagList),
				IDTA_ScalePercentage, GetTagData(IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor, TagList),
				TagList ? TAG_MORE : TAG_IGNORE, (ULONG) TagList,
				TAG_END);

			d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
			}

		FreePathBuffer(DefIconPath);
		}

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	return IconObj;
}


static Object *ReadDefIconObjectForNameFallback(ULONG IconType, CONST_STRPTR TypeName,
	BPTR dirLock, CONST_STRPTR OriginalName, struct TagItem *TagList)
{
	Object *IconObj = NULL;

	ScalosObtainSemaphoreShared(&DefIconsCacheSemaphore);
	if (IconType >= WBDISK && IconType <= WBAPPICON && CachedDefIconObjects[IconType])
		{
		// We have a cached iconObject, try to clone it now
		IconObj = CloneDefIconObject(CachedDefIconObjects[IconType], dirLock, OriginalName, TagList);
		d1(KPrintF("%s/%s/%ld: CloneDefIconObject returned IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		}
	ScalosReleaseSemaphore(&DefIconsCacheSemaphore);

	if (NULL == IconObj)
		{
		IconObj = ReadDefIconObjectForName(IconType, TypeName,
			dirLock, OriginalName, TagList);

		if (IconType >= WBDISK && IconType <= WBAPPICON && IconObj)
			{
			ScalosObtainSemaphore(&DefIconsCacheSemaphore);

			if (CachedDefIconObjects[IconType])
				DisposeIconObject(CachedDefIconObjects[IconType]);

			CachedDefIconObjects[IconType] = IconObj;
			IconObj = CloneDefIconObject(CachedDefIconObjects[IconType], dirLock, OriginalName, TagList);
			d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
			if (NULL == IconObj)
				{
				// Cloning failed
				IconObj = CachedDefIconObjects[IconType];
				CachedDefIconObjects[IconType] = NULL;
				}

			ScalosReleaseSemaphore(&DefIconsCacheSemaphore);
			}
		else
			{
			IconObj = GetDefIconObjectTags(IconType,
				IDTA_Text, (ULONG) OriginalName,
				TAG_MORE, (ULONG) TagList,
				TAG_END);
			}
		}

	return IconObj;
}


static Object *CloneDefIconObject(Object *IconObj, BPTR dirLock, 
	CONST_STRPTR OriginalName, struct TagItem *TagList)
{
	struct WBArg OriginalLocation;
	struct TagItem CloneTags[6];

	if (NULL == IconObj)
		return NULL;

	OriginalLocation.wa_Lock = dirLock;
	OriginalLocation.wa_Name = (STRPTR) OriginalName;

	d1(KPrintF("%s/%s/%ld: DefIconPath=<%s>\n", __FILE__, __FUNC__, __LINE__, DefIconPath));
	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	CloneTags[0].ti_Tag = IDTA_IconLocation;
	CloneTags[0].ti_Data = (ULONG) &OriginalLocation;

	CloneTags[1].ti_Tag = IDTA_SupportedIconTypes;
	CloneTags[1].ti_Data = GetTagData(IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes, TagList);

	CloneTags[2].ti_Tag = IDTA_SizeConstraints;
	CloneTags[2].ti_Data = GetTagData(IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints, TagList);

	CloneTags[3].ti_Tag = IDTA_ScalePercentage;
	CloneTags[3].ti_Data = GetTagData(IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor, TagList);

	CloneTags[4].ti_Tag = TagList ? TAG_MORE : TAG_IGNORE;
	CloneTags[4].ti_Data = (ULONG) TagList;

	CloneTags[5].ti_Tag = TAG_END;
	CloneTags[5].ti_Data = 0;

	IconObj = (Object *) DoMethod(IconObj,
		IDTM_CloneIconObject, CloneTags);

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	return IconObj;
}


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
		DevName = ScalosAlloc(1 + Length);
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

