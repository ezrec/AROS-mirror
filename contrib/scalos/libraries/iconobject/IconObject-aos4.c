// IconObject-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/emulation.h>
#include <exec/exectags.h>
#include <exec/resident.h>

#include <proto/exec.h>
#include <interfaces/iconobject.h>

#include <stdarg.h>

#include "IconObject.h"

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

LIBFUNC_P2VA_PROTO(Object *, LIBNewIconObjectTags,
	A0, APTR, Name,
	A6, struct IconObjectBase *, IconObjectBase);
LIBFUNC_P2VA_PROTO(Object *, LIBGetDefIconObjectTags,
	D0, ULONG, IconType,
	A6, struct IconObjectBase *, IconObjectBase);
LIBFUNC_P3VA_PROTO(LONG, LIBPutIconObjectTags,
	A0, Object *, iconobject,
	A1, APTR, path,
	A6, struct IconObjectBase *, IconObjectBase);
LIBFUNC_P2VA_PROTO(Object *, LIBConvert2IconObjectTags,
	A0, struct DiskObject *, diskobject,
	A6, struct IconObjectBase *, IconObjectBase);

extern const CONST_APTR VecTable68K[];

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
	LIBNewIconObject,
	LIBNewIconObjectTags,
	LIBDisposeIconObject,
	LIBGetDefIconObject,
	LIBGetDefIconObjectTags,
	LIBPutIconObject,
	LIBPutIconObjectTags,
	LIBIsIconName,
	LIBConvert2IconObject,
	LIBConvert2IconObjectA,
	LIBConvert2IconObjectTags,
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
	{CLT_DataSize, (ULONG)sizeof(struct IconObjectBase)},
	{CLT_Interfaces, (ULONG) interfaces},
	{CLT_Vector68K,  (ULONG)VecTable68K},
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
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	IconObjLibBase->iob_LibNode.lib_Revision = LIB_REVISION;
	IconObjLibBase->iob_SegList = (struct SegList *)seglist;

	if (!IconObjectInit(IconObjLibBase))
		{
		Expungelib(Self);
		IconObjLibBase = NULL;
		}

	return IconObjLibBase ? &IconObjLibBase->iob_LibNode : NULL;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) Self->Data.LibBase;

	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		struct SegList *libseglist = IconObjLibBase->iob_SegList;

		Remove((struct Node *) IconObjLibBase);
		IconObjectCleanup(IconObjLibBase);
		DeleteLibrary((struct Library *)IconObjLibBase);

		return (BPTR)libseglist;
		}

	IconObjLibBase->iob_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) Self->Data.LibBase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt++;
	IconObjLibBase->iob_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!IconObjectOpen(IconObjLibBase))
		{
		Closelib(Self);
		return NULL;
		}

	return (struct LibraryHeader *)&IconObjLibBase->iob_LibNode;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) Self->Data.LibBase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt--;
/*
	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		if (IconObjLibBase->iob_LibNode.lib_Flags & LIBF_DELEXP)
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

/* ------------------- OS4 Varargs Functions ------------------------ */
LIBFUNC_P2VA(Object *, LIBNewIconObjectTags,
	A0, APTR, Name,
	A6, struct IconObjectBase *, IconObjectBase)
{
	Object *ret;
  	va_list args;
	va_startlinear(args, Name);

	(void)IconObjectBase;

	ret = ((struct IconobjectIFace *)self)->NewIconObject(Name, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(Object *, LIBGetDefIconObjectTags,
	D0, ULONG, IconType,
	A6, struct IconObjectBase *, IconObjectBase)
{
	Object *ret;
  	va_list args;
	va_startlinear(args, IconType);

	(void)IconObjectBase;

	ret = ((struct IconobjectIFace *)self)->GetDefIconObject(IconType, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P3VA(LONG, LIBPutIconObjectTags,
	A0, Object *, iconobject,
	A1, APTR, path,
	A6, struct IconObjectBase *, IconObjectBase)
{
  	va_list args;
	LONG ret;
	va_startlinear(args, path);

	(void)IconObjectBase;

	ret = ((struct IconobjectIFace *)self)->PutIconObject(iconobject, path, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return ret;
}
LIBFUNC_END

LIBFUNC_P2VA(Object *, LIBConvert2IconObjectTags,
	A0, struct DiskObject *, diskobject,
	A6, struct IconObjectBase *, IconObjectBase)
{
	Object *ret;
  	va_list args;
	va_startlinear(args, diskobject);

	(void)IconObjectBase;

	ret = ((struct IconobjectIFace *)self)->Convert2IconObjectA(diskobject, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END


