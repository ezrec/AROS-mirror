// wb39proto.h
// $Date$
// $Revision$


#ifndef	WB39PROTO_H_INCLUDED
#define	WB39PROTO_H_INCLUDED

#include <defs.h>
#include "Scalos_Helper.h"


// AppWindow.c
BOOL DetachMyProcess(void);
void ShutdownMyProcess(void);
LIBFUNC_P5_PROTO(struct myAppWindowDropZone *, myAddAppWindowDropZoneA,
	A0, struct myAppWindow *, maw,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);
LIBFUNC_P3_PROTO(BOOL, myRemoveAppWindowDropZone,
	A0, struct myAppWindow *, maw,
	A1, struct myAppWindowDropZone *, dropZone,
	A6, struct Library *, WorkbenchBase, 0);
LIBFUNC_P6_PROTO(struct myAppWindow *, myAddAppWindowA,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A2, struct TagItem *, taglist,
	A6, struct Library *, WorkbenchBase, 0);
LIBFUNC_P2_PROTO(BOOL, myRemoveAppWindow,
	A0, struct myAppWindow *, appWindow,
	A6, struct Library *, WorkbenchBase, 0);
LIBFUNC_P5_PROTO(void, mySCA_DrawDrag,
	A0, APTR, draghandle,
	D0, LONG, X,
	D1, LONG, Y,
	D2, LONG, Flags,
	A6, struct Library *, ScalosBase, 0);

#ifdef __amigaos4__
LIBFUNC_P4VA_PROTO(struct AppWindowDropZone *, myAddAppWindowDropZone,
	A0, struct AppWindow *, maw,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A6, struct Library *, WorkbenchBase);
LIBFUNC_P5VA_PROTO(struct AppWindow *, myAddAppWindow,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A6, struct Library *, WorkbenchBase);
#endif

// WorkbenchControl.c
LIBFUNC_P3_PROTO(BOOL, myWorkbenchControlA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);

#ifdef __amigaos4__
LIBFUNC_P2VA_PROTO(BOOL, myWorkbenchControl,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase);
#endif

// AppIcons.c
BOOL AppIconsInit(void);
void AppIconsCleanup(void);


// Scalos_Helper.c
ULONG MenuOpenParentWindow(struct ScaWindowStruct *swi);
ULONG MenuUnselectAll(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByIcon(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByName(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeBySize(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByDate(struct ScaWindowStruct *swi);
ULONG MenuSetViewModeByType(struct ScaWindowStruct *swi);
ULONG MenuShowAllFiles(struct ScaWindowStruct *swi);
BOOL ParseWBPrefs(CONST_STRPTR filename);
BOOL ReadScalosPrefs(void);
struct ScaWindowStruct *GetIconNodeOpenWindow(BPTR dirLock, struct ScaIconNode *sIcon);
struct ScaIconNode *GetIconByName(struct ScaWindowStruct *swi, CONST_STRPTR IconName);
struct ScaWindowStruct *FindWindowByLock(BPTR xLock);
void MakeIconVisible(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon);
void MoveIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, LONG x, LONG y);
void SelectIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, BOOL Selected);
struct ScaWindowStruct *FindWindowByName(CONST_STRPTR WinName);
BOOL CloseScalosWindow(struct ScaWindowStruct *swi);
BOOL UpdateScalosWindow(struct ScaWindowStruct *swi);
struct ScaWindowStruct *GetNextWindow(struct ScaWindowStruct *swi);
struct ScaWindowStruct *GetPrevWindow(struct ScaWindowStruct *swi);
BOOL DeleteDirectory(CONST_STRPTR Path);
BOOL RemScalosIcon(BPTR Lock, CONST_STRPTR Name);
BOOL isIconSelected(const struct ScaIconNode *icon);
struct ScaRootList *GetScalosRootList(void);
ULONG MenuNewDrawer(CONST_STRPTR Name);
ULONG MenuUpdateScalosWindow(struct ScaWindowStruct *swi);
ULONG MenuCloseScalosWindow(struct ScaWindowStruct *swi);
CONST_STRPTR GetIconName(struct ScaIconNode *in);
struct ScaWindowStruct *WaitOpen(struct ScaWindowStruct *ws);


#endif
