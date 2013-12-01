// plugin-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/exectags.h>
#include <exec/resident.h>

#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <interfaces/scalosplugin.h>

#include <scalos/scalos.h>
#include <defs.h>

#include <stdarg.h>

#include "plugin.h"

#if PLUGIN_TYPE == PREVIEW
#include <interfaces/scalospreviewplugin.h>
#endif

#ifndef d
#define d(x)
#endif
#ifndef d1
#define d1(x)
#endif
#ifndef d2
#define d2(x) x
#endif

// from debug.lib
extern int kprintf(const char *fmt, ...);

int _start(void)
{
	return -1;
}

static struct Library *Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec);
static BPTR Expungelib(struct LibraryManagerInterface *Self);
static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version);
static BPTR Closelib(struct LibraryManagerInterface *Self);
static ULONG Obtainlib(struct Interface *Self);
static ULONG Releaselib(struct Interface *Self);

#if PLUGIN_TYPE == PREVIEW
static LIBFUNC_P4VA_PROTO(ULONG, LIBSCAPreviewGenerateTags,
	A0, struct ScaWindowTask *, wt,
	A1, BPTR, dirLock,
	A2, CONST_STRPTR, iconName,
	A6, struct PluginBase *, PluginBase);
#endif

extern CONST_APTR VecTable68K[];

struct ExecBase *SysBase;
struct ExecIFace *IExec;

//----------------------------------------------------------------------------

#include "plugin-common.c"

//----------------------------------------------------------------------------

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
#if PLUGIN_TYPE == OOP
	GetClassInfo,
#endif
#if (PLUGIN_TYPE == FILETYPE)
	FT_INFOSTRING,
#endif
#if PLUGIN_TYPE == PREVIEW
	PV_GENERATE,
	LIBSCAPreviewGenerateTags,
#endif
#if (PLUGIN_TYPE == PREFS)
	LIBSCAGetPrefsInfo,
#endif
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
	{CLT_DataSize, (ULONG)sizeof(LIB_BASETYPE)},
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
#ifdef LIB_IDSTRING
	libIdString,
#else
	&libVerString[6],
#endif
	(APTR)inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct PluginBase *PluginBase = (struct PluginBase *) libbase;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;

	(void)libVerString;

	PluginBase->pl_Initialized = FALSE;

	PluginBase->pl_LibNode.lib_Revision = LIB_REVISION;
	PluginBase->pl_SegList = seglist;

#if PLUGIN_TYPE == OOP
	PluginBase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif
	d1(kprintf(__FILE__ "/%s/%ld: pl_PlugID=%08lx\n", __FUNC__, __LINE__, PluginBase->pl_PlugID));

	return (struct Library *)PluginBase;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct PluginBase *PluginBase = (struct PluginBase *) Self->Data.LibBase;

	if (0 == PluginBase->pl_LibNode.lib_OpenCnt)
		{
		BPTR libseglist = PluginBase->pl_SegList;

		Remove((struct Node *) PluginBase);
#ifndef NOCLOSEPLUGIN
		closePlugin(PluginBase);
#endif
		DeleteLibrary((struct Library *)PluginBase);

		return libseglist;
		}

	PluginBase->pl_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct PluginBase *PluginBase = (struct PluginBase *) Self->Data.LibBase;

	PluginBase->pl_LibNode.lib_OpenCnt++;
	PluginBase->pl_LibNode.lib_Flags &= ~LIBF_DELEXP;

#ifndef NOINITPLUGIN
	if (!PluginBase->pl_Initialized)
		{
		if (!initPlugin(PluginBase))
			{
			Closelib(Self);
			return NULL;
			}
		PluginBase->pl_Initialized = TRUE;
		}
#endif

	return (struct LibraryHeader *)PluginBase;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct PluginBase *PluginBase = (struct PluginBase *) Self->Data.LibBase;

	PluginBase->pl_LibNode.lib_OpenCnt--;
/*
	if (0 == PluginBase->pl_LibNode.lib_OpenCnt)
		{
		if (PluginBase->pl_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(Self);
			}
		}
*/
	return (BPTR)NULL;
}

static ULONG Obtainlib(struct Interface *Self)
{
  return(Self->Data.RefCount++);
}

static ULONG Releaselib(struct Interface *Self)
{
  return(Self->Data.RefCount--);
}

#if PLUGIN_TYPE == PREVIEW
static LIBFUNC_P4VA(ULONG, LIBSCAPreviewGenerateTags,
	A0, struct ScaWindowTask *, wt,
	A1, BPTR, dirLock,
	A2, CONST_STRPTR, iconName,
	A6, struct PluginBase *, PluginBase)
{
	BOOL ret;
  	va_list args;
	va_startlinear(args, iconName);

	(void) PluginBase;

	ret = ((struct ScalosPreviewPluginIFace *)self)->SCAPreviewGenerate(wt, dirLock, iconName, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END
#endif

