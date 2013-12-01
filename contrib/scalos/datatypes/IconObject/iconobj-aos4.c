// iconobj-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/exectags.h>
#include <exec/resident.h>

#include <proto/exec.h>
#include <interfaces/iconobject.h>

#include <stdarg.h>
#include <limits.h>

#include "iconobj.h"

int _start(void)
{
	return -1;
}

static struct Library *Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec);
static BPTR Expungelib(struct LibraryManagerInterface *Self);
static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version);
static BPTR Closelib(struct LibraryManagerInterface *Self);
static ULONG Obtainlib(struct LibraryManagerInterface *Self);
static ULONG Releaselib(struct LibraryManagerInterface *Self);

extern struct ExecBase *SysBase;

/* OS4.0 Library */

/* ------------------- OS4 Manager Interface ------------------------ */
static const APTR managerfunctable[] =
	{
	(APTR)Obtainlib,
	(APTR)Releaselib,
	(APTR)NULL,
	(APTR)NULL,
	(APTR)Openlib,
	(APTR)Closelib,
	(APTR)Expungelib,
	(APTR)NULL,
	(APTR)-1
	};

static const struct TagItem managertags[] =
	{
	{MIT_Name,             (ULONG)"__library"},
	{MIT_VectorTable,      (ULONG)managerfunctable},
	{MIT_Version,          1},
	{TAG_DONE,             0}
	};

/* ---------------------- OS4 Main Interface ------------------------ */
static APTR functable[] =
	{
	Obtainlib,
	Releaselib,
	NULL,
	NULL,
	ObtainInfoEngine,
	(APTR) -1
	};

static const struct TagItem maintags[] =
	{
	{MIT_Name,             (ULONG)"main"},
	{MIT_VectorTable,      (ULONG)functable},
	{MIT_Version,          1},
	{TAG_DONE,             0}
	};

/* Init table used in library initialization. */
static const ULONG interfaces[] =
	{
	(ULONG)managertags,
	(ULONG)maintags,
	(ULONG)0
	};

static const struct TagItem inittab[] =
	{
	{CLT_DataSize, (ULONG)sizeof(struct IconObjectDtLibBase)},
	{CLT_Interfaces, (ULONG) interfaces},
//	  {CLT_Vector68K,  (ULONG)VecTable68K},
	{CLT_InitFunc, (ULONG) Initlib},
	{TAG_DONE, 0}
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
	RTF_NATIVE | RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	(struct TagItem *)inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct IconObjectDtLibBase *IconObjLibBase = (struct IconObjectDtLibBase *) libbase;
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = SCHAR_MIN;
	IconObjLibBase->nib_SegList = (struct SegList *)seglist;

	if (!InitDatatype(IconObjLibBase))
		{
		Expungelib(Self);
		IconObjLibBase = NULL;
		}

	return (struct Library *)IconObjLibBase;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct IconObjectDtLibBase *IconObjLibBase = (struct IconObjectDtLibBase *) Self->Data.LibBase;

	if (0 == IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		struct SegList *libseglist = IconObjLibBase->nib_SegList;

		Remove((struct Node *) IconObjLibBase);
		CloseDatatype(IconObjLibBase);
		DeleteLibrary((struct Library *)IconObjLibBase);

		return (BPTR)libseglist;
		}

	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct IconObjectDtLibBase *IconObjLibBase = (struct IconObjectDtLibBase *) Self->Data.LibBase;

	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(IconObjLibBase))
		{
		Closelib(Self);
		return NULL;
		}

	return (struct LibraryHeader *)IconObjLibBase;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct IconObjectDtLibBase *IconObjLibBase = (struct IconObjectDtLibBase *) Self->Data.LibBase;

	IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;
/*
	if (0 == IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (IconObjLibBase->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(Self);
			}
		}
*/
	return (BPTR)NULL;
}

static ULONG Obtainlib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount++);
}

static ULONG Releaselib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount--);
}

