// Patches.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>
#include <dos/dostags.h>
#include <guigfx/guigfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/guigfx.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------
// Revision history :
//
// 20011203	jl	initial history
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

// global functions

LIBFUNC_P3_PROTO(struct Window *, sca_OpenWindowTagList,
	A0, struct NewWindow *, newWin,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase, 0);

LIBFUNC_P1_PROTO(LONG, sca_CloseWorkBench,
	A6, struct IntuitionBase *, iBase, 0);
LIBFUNC_P1_PROTO(LONG, sca_OpenWorkBench,
	A6, struct IntuitionBase *, iBase, 0);
LIBFUNC_P8_PROTO(struct AppIcon *, sca_AddAppIconA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, IconText,
	A1, struct MsgPort *, iconPort,
	A2, BPTR, lock,
	A3, struct DiskObject *, DiskObj,
	A4, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P2_PROTO(ULONG, sca_RemoveAppIcon,
	A0, struct AppIcon *, appIcon,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P6_PROTO(struct AppWindow *, sca_AddAppWindowA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, struct Window *, window,
	A1, struct MsgPort *, iconPort,
	A2, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P2_PROTO(BOOL, sca_RemoveAppWindow,
	A0, struct AppWindow *, aw,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P6_PROTO(struct AppMenuItem *, sca_AddAppMenuItemA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, text,
	A1, struct MsgPort *, iconPort,
	A2, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P2_PROTO(BOOL, sca_RemoveAppMenuItem,
	A0, struct AppMenuItem *, ami,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P3_PROTO(struct Screen *, sca_OpenScreenTagList,
	A0, struct NewScreen *, newScr,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase, 0);
LIBFUNC_P4_PROTO(void, sca_UpdateWorkbench,
	A0, CONST_STRPTR, Name,
	A1, BPTR, ParentLock,
	D0, LONG, Action,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P1_PROTO(LONG, sca_SetBackFill,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P4_PROTO(ULONG, sca_WBInfo,
	A0, BPTR, lock,
	A1, STRPTR, name,
	A2, struct Screen *, screen,
	A6, struct Library *, wbBase, 0);
LIBFUNC_P3_PROTO(BOOL, sca_PutDiskObject,
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A6, struct Library *, IconBase, 0);
LIBFUNC_P2_PROTO(BOOL, sca_DeleteDiskObject,
	A0, CONST_STRPTR, Name,
	A6, struct Library *, IconBase, 0);
LIBFUNC_P4_PROTO(BOOL, sca_PutIconTagList,
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A2, struct TagItem *, tags,
	A6, struct Library *, IconBase, 0);

LIBFUNC_P2_PROTO(ULONG, sca_DeleteFile,
	D1, CONST_STRPTR, Name,
	A6, struct DosLibrary *, DOSBase, 0);
LIBFUNC_P3_PROTO(ULONG, sca_Rename,
	D1, CONST_STRPTR, oldName,
	D2, CONST_STRPTR, newName,
	A6, struct DosLibrary *, DOSBase, 0);
LIBFUNC_P3_PROTO(BPTR, sca_Open,
	D1, CONST_STRPTR, name,
	D2, LONG, accessMode,
	A6, struct DosLibrary *, DOSBase, 0);
LIBFUNC_P2_PROTO(ULONG, sca_Close,
	D1, BPTR, file,
	A6, struct DosLibrary *, DOSBase, 0);
LIBFUNC_P2_PROTO(BPTR, sca_CreateDir,
	D1, CONST_STRPTR, name,
	A6, struct DosLibrary *, DOSBase, 0);

//LIBFUNC_P2_PROTO(CONST_STRPTR, sca_GetLocString,
//	D0, ULONG, StringID,
//	A6, struct Library *, wbBase);

//----------------------------------------------------------------------------

// public data items

#ifdef __AROS__

#ifdef TEST_OPENWINDOWTAGLIST
extern struct Window * (*OldOpenWindowTagList) ();
#endif /* TEST_OPENWINDOWTAGLIST */
extern ULONG (*OldCloseWB) ();
extern ULONG (*OldOpenWB) ();
extern ULONG (*OldRemoveAppIcon) ();
extern BOOL (*OldRemoveAppWindow) ();
extern BOOL (*OldRemoveAppMenuItem) ();
extern struct Screen * (*OldOpenScreenTagList) ();
extern void (*OldUpdateWorkbench) ();
extern APTR OldAddAppIconA;
extern APTR OldAddAppWindowA;
extern APTR OldAppAppMenuItemA;
extern APTR OldSetBackFill;
extern APTR OldWBInfo;
extern BOOL (*OldPutDiskObject) ();
extern BOOL (*OldDeleteDiskObject) ();
extern BOOL (*OldPutIconTagList) ();
extern ULONG (*OldDeleteFile) ();
extern ULONG (*OldRename) ();
extern BPTR (*OldOpen) ();
extern ULONG (*OldClose) ();
extern BPTR (*OldCreateDir) ();

#else

#ifdef TEST_OPENWINDOWTAGLIST
extern LIBFUNC_P3_DPROTO(struct Window *, (*OldOpenWindowTagList),
	A0, struct NewWindow *, newWin,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase);
#endif /* TEST_OPENWINDOWTAGLIST */
extern LIBFUNC_P1_DPROTO(ULONG, (*OldCloseWB),
	A6, struct IntuitionBase *, iBase);
extern LIBFUNC_P1_DPROTO(ULONG, (*OldOpenWB),
	A6, struct IntuitionBase *, iBase);
extern LIBFUNC_P2_DPROTO(ULONG, (*OldRemoveAppIcon),
	A0, struct AppIcon *, appIcon,
	A6, struct Library *, qq);
extern LIBFUNC_P2_DPROTO(BOOL, (*OldRemoveAppWindow),
	A0, struct AppWindow *, aw,
	A6, struct Library *, wbBase);
extern LIBFUNC_P2_DPROTO(BOOL, (*OldRemoveAppMenuItem),
	A0, struct AppMenuItem *, ami,
	A6, struct Library *, wbBase);
extern LIBFUNC_P3_DPROTO(struct Screen *, (*OldOpenScreenTagList),
	A0, struct NewScreen *, newScr,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase);
extern LIBFUNC_P4_DPROTO(void, (*OldUpdateWorkbench),
	A0, CONST_STRPTR, Name,
	A1, BPTR, ParentLock,
	D0, LONG, Action,
	A6, struct Library *, wbBase);
extern APTR OldAddAppIconA;
extern APTR OldAddAppWindowA;
extern APTR OldAppAppMenuItemA;
extern APTR OldSetBackFill;
extern APTR OldWBInfo;
extern LIBFUNC_P3_DPROTO(BOOL, (*OldPutDiskObject),
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A6, struct Library *, IconBase);
extern LIBFUNC_P2_DPROTO(BOOL, (*OldDeleteDiskObject),
	A0, CONST_STRPTR, Name,
	A6, struct Library *, IconBase);
extern LIBFUNC_P4_DPROTO(BOOL, (*OldPutIconTagList),
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A2, struct TagItem *, tags,
	A6, struct Library *, IconBase);

extern LIBFUNC_P2_DPROTO(ULONG, (*OldDeleteFile),
	D1, CONST_STRPTR, Name,
	A6, struct DosLibrary *, DOSBase);
extern LIBFUNC_P3_DPROTO(ULONG, (*OldRename),
	D1, CONST_STRPTR, oldName,
	D2, CONST_STRPTR, newName,
	A6, struct DosLibrary *, DOSBase);
extern LIBFUNC_P3_DPROTO(BPTR, (*OldOpen),
	D1, CONST_STRPTR, name,
	D2, LONG, accessMode,
	A6, struct DosLibrary *, DOSBase);
extern LIBFUNC_P2_DPROTO(ULONG, (*OldClose),
	D1, BPTR, file,
	A6, struct DosLibrary *, DOSBase);
extern LIBFUNC_P2_DPROTO(BPTR, (*OldCreateDir),
	D1, CONST_STRPTR, name,
	A6, struct DosLibrary *, DOSBase);

//extern LIBFUNC_P1_DPROTO(CONST_STRPTR, (*OldwbPrivate2),
//	D0, ULONG, StringID);

#endif

//----------------------------------------------------------------------------

