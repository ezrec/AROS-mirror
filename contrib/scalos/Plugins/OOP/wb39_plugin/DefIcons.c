// DefIcons.c
// 13 Nov 2004 19:37:42


#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>

#include <clib/alib_protos.h>

#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/scalos.h>


#include <defs.h>

/* How it works:
   gets the "Global Identify Hook" from icon.library and calls this hook itself.
   If DefIcons is being started automatically, like it is default in OS3.9,
   its identify hook will be got from icon.library.
*/

//----------------------------------------------------------------------------
// Revision history :
//
// 20010405	jl	44.5
//			Forgot to set DTA_Name for generated default icons. Fixed.
//----------------------------------------------------------------------------

struct SM_GetDefIcon
	{
	ULONG MethodID;
	STRPTR filename;
	ULONG filetype;		// from FileInfoBlock fib_DirEntryType
	ULONG protectionbits;
	};


#define	d(x)	;
#define	d1(x)	;
#define	d2 (x)	x;


// aus debug.lib
extern int kprintf(const char *fmt, ...);

static BOOL Init(void);
SAVEDS(void) INTERRUPT ASM DefIconsCleanup(void);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static Object *GetDefIcon(Class *cl, Object *obj, struct SM_GetDefIcon *msg);


struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct Library *IconobjectBase;
struct Library *UtilityBase;
struct Library *IconBase;
struct Library *ScalosBase;
struct IntuitionBase *IntuitionBase;


static BOOL fInit;


SAVEDS(ULONG) INTERRUPT ASM myHookFunc(REG(a0, Class *cl),
		REG(a2, Object *obj),
		REG(a1, Msg msg))
{
	ULONG Result;

	d(kprintf(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case SCCM_IconWin_GetDefIcon:
		Result = (ULONG) GetDefIcon(cl, obj, (struct SM_GetDefIcon *) msg);

		d(kprintf(__FUNC__ "/%ld: RootList=%08lx  InternInfos=%08lx\n", __LINE__, \
			rList, rList->rl_internInfos));

		break;

	default:
		d(kprintf(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}


SAVEDS(ULONG) INTERRUPT ASM DefIconsInit(void)
{
	d(kprintf(__FUNC__ "/%ld: \n", __LINE__));

	if (!fInit)
		{
		BOOL Success = fInit = Init();

		if (!Success)
			DefIconsCleanup();

		return Success;
		}

	return TRUE;
}


static BOOL Init(void)
{
	if (!OpenLibraries())
		return FALSE;

	d(kprintf(__FUNC__ "/%ld: IconobjectBase=%08lx UtilityBase=%08lx\n", \
		__LINE__, IconobjectBase, UtilityBase));

	return TRUE;
}


SAVEDS(void) INTERRUPT ASM DefIconsCleanup(void)
{
	CloseLibraries();

	fInit = FALSE;
}


static BOOL OpenLibraries(void)
{
#ifndef __AROS__
	SysBase = *( (struct ExecBase **) 4l);
#endif

	DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
	if (NULL == DOSBase)
		return FALSE;

	IconobjectBase = OpenLibrary("iconobject.library", 39);
	if (NULL == IconobjectBase)
		return FALSE;

	UtilityBase = OpenLibrary("utility.library", 40);
	if (NULL == UtilityBase)
		return FALSE;

	IconBase = OpenLibrary("icon.library", 44);
	if (NULL == IconBase)
		return FALSE;

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;

	ScalosBase = OpenLibrary(SCALOSNAME, 41);
	if (NULL == ScalosBase)
		return FALSE;

	return TRUE;
}


static void CloseLibraries(void)
{
	if (ScalosBase)
		{
		CloseLibrary(ScalosBase);
		ScalosBase = NULL;
		}
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
		}
	if (UtilityBase)
		{
		CloseLibrary(UtilityBase);
		UtilityBase = NULL;
		}
}


void _XCEXIT(long x)
{
}


static Object *GetDefIcon(Class *cl, Object *obj, struct SM_GetDefIcon *msg)
{
	Object *IconObj = NULL;
	struct DiskObject *diskObj = NULL;
	struct Hook *pHook = NULL;

	d1(kprintf(__FUNC__ "/%ld: filename=<%s>  type=%08lx  protection=%08lx\n", __LINE__, \
		msg->filename, msg->filetype, msg->protectionbits));

	// IconControlA()
	IconControl(NULL,
		ICONCTRLA_GetGlobalIdentifyHook, &pHook,
		TAG_END);

	d1(kprintf(__FUNC__ "/%ld: pHook=%08lx\n", __LINE__, pHook));
	if (pHook)
		{
		struct TagItem EmptyTagList = { TAG_END, 0 };
		BPTR fLock;
		struct FileInfoBlock *fib;

		fLock = Lock(msg->filename, ACCESS_READ);
		d1(kprintf(__FUNC__ "/%ld: fLock=%08lx\n", __LINE__, fLock));
		if (fLock)
			{
			BPTR fh;
			BPTR parentLock = ParentDir(fLock);

			fh = Open(msg->filename, MODE_OLDFILE);

			fib = AllocDosObject(DOS_FIB, TAG_END);
			d1(kprintf(__FUNC__ "/%ld: fib=%08lx  fh=%08lx\n", __LINE__, fib, fh));
			if (fib)
				{
				struct IconIdentifyMsg iim;

				Examine(fLock, fib);

				iim.iim_SysBase = (struct Library *) SysBase;
				iim.iim_DOSBase = (struct Library *) DOSBase;
				iim.iim_UtilityBase = UtilityBase;
				iim.iim_IconBase = IconBase;

				iim.iim_FileLock = fLock;
				iim.iim_ParentLock = parentLock;
				iim.iim_FIB = fib;
				iim.iim_FileHandle = fh;

				iim.iim_Tags = &EmptyTagList;

				diskObj = (struct DiskObject *) CallHookPkt(pHook, NULL, &iim);

				d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx\n", __LINE__, diskObj));

				FreeDosObject(DOS_FIB, fib);
				}

			if (parentLock)
				UnLock(parentLock);
			if (fh)
				Close(fh);

			UnLock(fLock);
			}
		}

	d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx  FileType=%ld\n", __LINE__, diskObj, msg->filetype));

	if (NULL == diskObj && IconBase->lib_Version >= 44)
		{
		ULONG iconType;

		switch (msg->filetype)
			{
		case ST_ROOT:
			iconType = WBDISK;
			break;

		case ST_USERDIR:
		case ST_LINKDIR:
			iconType = WBDRAWER;
			break;

		default:
			iconType = WBPROJECT;
			break;
			}

		// GetIconTagList
		diskObj = GetIconTags(msg->filename,
			ICONGETA_GetDefaultType, iconType,
			ICONGETA_FailIfUnavailable, FALSE,
			TAG_END);
		d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx\n", __LINE__, diskObj));
		}

	if (NULL == diskObj)
		{
		diskObj = GetDiskObjectNew(msg->filename);
		d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx\n", __LINE__, diskObj));
		}

	d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx\n", __LINE__, diskObj));

	if (diskObj)
		{
		ULONG SupportedIconTypes = ~0;

		if (ScalosBase->lib_Version >= 40)
			{
			SCA_ScalosControl(NULL,
				SCALOSCTRLA_GetSupportedIconTypes, &SupportedIconTypes,
				TAG_END);
			}

		IconObj = Convert2IconObjectTags(diskObj,
			IDTA_SupportedIconTypes, SupportedIconTypes,
			TAG_END);

		if (IconObj)
			{
			SetAttrs(IconObj, 
				DTA_Name, msg->filename,
				IDTA_DoFreeDiskObject, TRUE,
				TAG_END);
			}
		else
			FreeDiskObject(diskObj);
		}

	d1(if (diskObj && diskObj->do_DrawerData)\
		kprintf(__FUNC__ "/%ld: dd_Flags=%08lx\n", __LINE__, diskObj->do_DrawerData->dd_Flags));

	d1(kprintf(__FUNC__ "/%ld: IconObj=%08lx\n", __LINE__, diskObj, IconObj));

	return IconObj;
}

