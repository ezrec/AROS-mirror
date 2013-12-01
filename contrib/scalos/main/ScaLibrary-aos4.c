// ScaLibrary-aos4.c
// $Date$
// $Revision$


#include <proto/exec.h>
#undef __USE_INLINE__
#include <proto/scalos.h>

#include <exec/emulation.h>
#include <exec/libraries.h>
#include <scalos/scalos.h>

#include <stdarg.h>

#include "defs.h"
#include "functions.h"
#include "ScaLibrary.h"
#include "Scalos_rev.h"
#include "Variables.h"

struct ScalosIFace *IScalos;
extern const CONST_APTR VecTable68K[];

/* ------------------- OS4 Vararg functions ------------------------- */
static LIBFUNC_P5VA_PROTO(struct AppObject *, sca_NewAddAppIconTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, Object *, iconObj,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P2VA_PROTO(ULONG, sca_ScalosControlTags,
	A0, CONST_STRPTR, name,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P5VA_PROTO(struct AppObject *, sca_NewAddAppMenuItemTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, MenuText,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P3VA_PROTO(BOOL, sca_WBStartTags,
	A0, struct WBArg *, ArgArray,
	D0, ULONG, NumArgs,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P2VA_PROTO(Object *, sca_NewScalosObjectTags,
	A0, CONST_STRPTR, ClassName,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P2VA_PROTO(void, sca_ScreenTitleMsgArgs,
	A0, CONST_STRPTR, Format,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P5VA_PROTO(struct AppObject *, sca_NewAddAppWindowTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, struct Window *, win,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase);
static LIBFUNC_P2VA_PROTO(struct ScaWindowStruct *, sca_OpenDrawerByNameTags,
	A0, CONST_STRPTR, path,
	A6, struct ScalosBase *, ScalosBase);

LIBFUNC_P5VA(struct AppObject *, sca_NewAddAppIconTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, Object *, iconObj,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase)
{
	struct AppObject *ret;
  	va_list args;
	va_startlinear(args, msgPort);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_NewAddAppIcon(ID, UserData, iconObj, msgPort, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(ULONG, sca_ScalosControlTags,
	A0, CONST_STRPTR, name,
	A6, struct ScalosBase *, ScalosBase)
{
	ULONG ret;
  	va_list args;
	va_startlinear(args, name);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_ScalosControlA(name, va_getlinearva(args, APTR));

	va_end(args);

	return ret;
}
LIBFUNC_END

LIBFUNC_P5VA(struct AppObject *, sca_NewAddAppMenuItemTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, MenuText,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase)
{
	struct AppObject *ret;
  	va_list args;
	va_startlinear(args, msgPort);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_NewAddAppMenuItem(ID,UserData, MenuText, msgPort, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P3VA(BOOL, sca_WBStartTags,
	A0, struct WBArg *, ArgArray,
	D0, ULONG, NumArgs,
	A6, struct ScalosBase *, ScalosBase)
{
	BOOL ret;
  	va_list args;
	va_startlinear(args, NumArgs);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_WBStart(ArgArray, va_getlinearva(args, const struct TagItem *), NumArgs);

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(Object *, sca_NewScalosObjectTags,
	A0, CONST_STRPTR, ClassName,
	A6, struct ScalosBase *, ScalosBase)
{
	Object *ret;
  	va_list args;
	va_startlinear(args, ClassName);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_NewScalosObject(ClassName, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return ret;
}
LIBFUNC_END

LIBFUNC_P2VA(void, sca_ScreenTitleMsgArgs,
	A0, CONST_STRPTR, Format,
	A6, struct ScalosBase *, ScalosBase)
{
  	va_list args;
	va_startlinear(args, Format);

	(void)ScalosBase;

	((struct ScalosIFace *)self)->SCA_ScreenTitleMsg(Format, va_getlinearva(args, APTR));

	va_end(args);
}
LIBFUNC_END

LIBFUNC_P5VA(struct AppObject *, sca_NewAddAppWindowTags,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, struct Window *, win,
	A1, struct MsgPort *, msgPort,
	A6, struct ScalosBase *, ScalosBase)
{
	struct AppObject *ret;
  	va_list args;
	va_startlinear(args, msgPort);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_NewAddAppWindow(ID, UserData, win, msgPort, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P2VA(struct ScaWindowStruct *, sca_OpenDrawerByNameTags,
	A0, CONST_STRPTR, path,
	A6, struct ScalosBase *, ScalosBase)
{
	struct ScaWindowStruct *ret;
  	va_list args;
	va_startlinear(args, path);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_OpenDrawerByName(path, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P1VA(BOOL, sca_OpenIconWindowTags,
	A6, struct ScalosBase *, ScalosBase)
{
	BOOL ret;
  	va_list args;
	va_startlinear(args, self); // self is always the first argument

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_OpenIconWindow(va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P3VA(Object *, sca_GetDefIconObjectTags,
	A0, BPTR, dirLock,
	A1, CONST_STRPTR, name,
	A6, struct ScalosBase *, ScalosBase)
{
	Object *ret;
  	va_list args;
	va_startlinear(args, name);

	(void)ScalosBase;

	ret = ((struct ScalosIFace *)self)->SCA_GetDefIconObjectA(dirLock, name, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

/* ------------------- OS4 Manager Interface ------------------------ */
static ULONG sca_LibObtain(struct Interface *Self)
{
	d1(KPrintF("%s/%s/%ld: Self=%08lx  RefCount=%ld\n", __FILE__, __FUNC__, __LINE__, Self, Self->Data.RefCount));
	return(Self->Data.RefCount++);
}

static ULONG sca_LibRelease(struct Interface *Self)
{
	d1(KPrintF("%s/%s/%ld: Self=%08lx  RefCount=%ld\n", __FILE__, __FUNC__, __LINE__, Self, Self->Data.RefCount));
	return(Self->Data.RefCount--);
}

static BPTR sca_LibExpunge(struct LibraryManagerInterface *Self)
{
	struct Library *libBase = Self->Data.LibBase;
	d1(KPrintF("%s/%s/%ld: libBase=%08lx  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase, libBase->lib_OpenCnt));

	if (0 == libBase->lib_OpenCnt)
		{
		Remove(&libBase->lib_Node);
		DeleteLibrary(libBase);
		}

	return (BPTR)NULL;
}

static struct LibraryHeader *sca_LibOpen(struct LibraryManagerInterface *Self, ULONG version)
{
	struct Library *libBase = (struct Library *)Self->Data.LibBase;

	libBase->lib_OpenCnt++;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount++;

	return (struct LibraryHeader *)libBase;
}

static BPTR sca_LibClose(struct LibraryManagerInterface *Self)
{
	struct Library *libBase = (struct Library *)Self->Data.LibBase;

	libBase->lib_OpenCnt--;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount--;

	return (BPTR)NULL;
}

static const APTR LibManagerVectors[] =
{
	(APTR)sca_LibObtain,
	(APTR)sca_LibRelease,
	(APTR)NULL,
	(APTR)NULL,
	(APTR)sca_LibOpen,
	(APTR)sca_LibClose,
	(APTR)sca_LibExpunge,
	(APTR)NULL,
	(APTR)-1
};

static const struct TagItem sca_LibManagerTags[] =
{
	{MIT_Name,             (ULONG)"__library"},
	{MIT_VectorTable,      (ULONG)LibManagerVectors},
	{MIT_Version,          1},
	{TAG_DONE,             0}
};

/* ---------------------- OS4 Main Interface ------------------------ */
APTR ScalosLibFunctions[] =
{
	sca_LibObtain,
	sca_LibRelease,
	NULL,
	NULL,

	sca_WBStart,
	sca_WBStartTags,
	sca_SortNodes,
	sca_NewAddAppIcon,
	sca_NewAddAppIconTags,
	sca_RemoveAppObject,
	sca_NewAddAppWindow,
	sca_NewAddAppWindowTags,
	sca_NewAddAppMenuItem,
	sca_NewAddAppMenuItemTags,
	sca_AllocStdNode,
	sca_AllocNode,
	sca_FreeNode,
	sca_FreeAllNodes,
	sca_MoveNode,
	sca_SwapNodes,
	sca_OpenIconWindow,
	sca_OpenIconWindowTags,
	sca_LockWindowList,
	sca_UnLockWindowList,
	sca_AllocMessage,
	sca_FreeMessage,
	sca_InitDrag,
	sca_EndDrag,
	sca_AddBob,
	sca_DrawDrag,
	sca_UpdateIcon,
	sca_MakeWBArgs,
	sca_FreeWBArgs,
	sca_RemapBitmap,
	sca_ScreenTitleMsg,
	sca_ScreenTitleMsgArgs,
	sca_MakeScalosClass,
	sca_FreeScalosClass,
	sca_NewScalosObject,
	sca_NewScalosObjectTags,
	sca_DisposeScalosObject,
	sca_ScalosControl,
	sca_ScalosControlTags,
	sca_ScalosControlTags,
	sca_GetDefIconObject,
	sca_OpenDrawerByName,
	sca_OpenDrawerByNameTags,
	sca_CountWBArgs,
	sca_GetDefIconObjectA,
	sca_GetDefIconObjectTags,
	sca_LockDrag,
	sca_UnlockDrag,

	(APTR) ~0
};

static const struct TagItem sca_MainTags[] =
{
	{MIT_Name,             (ULONG)"main"},
	{MIT_VectorTable,      (ULONG)ScalosLibFunctions},
	{MIT_Version,          1},
	{TAG_DONE,             0}
};


/* --------------------- OS4 Library Creation ----------------------- */
static const ULONG ScalosInterfaces[] =
{
	(ULONG)sca_LibManagerTags,
	(ULONG)sca_MainTags,
	(ULONG)0
};


struct ScalosBase *sca_MakeLibrary(void)
{
	struct Library *libbase;
	struct ExtendedLibrary *extlib;

	libbase = CreateLibraryTags(
		sizeof(struct ScalosBase),
		CLT_Interfaces, &ScalosInterfaces,
		CLT_Vector68K, &VecTable68K,
		TAG_DONE, 0);
	extlib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);

	libbase->lib_Node.ln_Type = NT_LIBRARY;
	libbase->lib_Node.ln_Name = (STRPTR)scalosLibName;
	libbase->lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
	libbase->lib_Version = VERSION;
	libbase->lib_Revision = REVISION;
	libbase->lib_IdString = (STRPTR)libIdString;
	((struct ScalosBase *)libbase)->scb_Revision = ScalosRevision;

	IScalos = (struct ScalosIFace *)extlib->MainIFace;

	return (struct ScalosBase *)libbase;
}

