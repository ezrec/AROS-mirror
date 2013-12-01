#ifndef CLIB_SCALOS_PROTOS_H
#define CLIB_SCALOS_PROTOS_H


/*
**	$VER: scalos_protos.h 21.42 (08.01.2008)
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2008 ©1999-2002 The Scalos Team
**	All Rights Reserved
*/

#ifndef  INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef  WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif
#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif

BOOL SCA_WBStart(APTR argArray, CONST struct TagItem * tagList, ULONG numArgs);
BOOL SCA_WBStartTags(APTR argArray, ULONG numArgs, ULONG firstTag, ...);
VOID SCA_SortNodes(struct ScalosNodeList * nodelist, struct Hook * sortHook,
	ULONG sortType);
struct AppObject * SCA_NewAddAppIcon(ULONG iD, ULONG userData, Object * iconObj,
	struct MsgPort * msgPort, CONST struct TagItem * tagList);
struct AppObject * SCA_NewAddAppIconTags(ULONG iD, ULONG userData, Object * iconObj,
	struct MsgPort * msgPort, ULONG tagList, ...);
BOOL SCA_RemoveAppObject(struct AppObject * appObj);
struct AppObject * SCA_NewAddAppWindow(ULONG iD, ULONG userData, struct Window * win,
	struct MsgPort * msgPort, CONST struct TagItem * tagList);
struct AppObject * SCA_NewAddAppWindowTags(ULONG iD, ULONG userData, struct Window * win,
	struct MsgPort * msgPort, ULONG tagList, ...);
struct AppObject * SCA_NewAddAppMenuItem(ULONG iD, ULONG userData, CONST_STRPTR text,
	struct MsgPort * msgPort, CONST struct TagItem * tagList);
struct AppObject * SCA_NewAddAppMenuItemTags(ULONG iD, ULONG userData, CONST_STRPTR text,
	struct MsgPort * msgPort, ULONG tagList, ...);
struct MinNode * SCA_AllocStdNode(struct ScalosNodeList * nodeList, ULONG nodeType);
struct MinNode * SCA_AllocNode(struct ScalosNodeList * nodeList, ULONG size);
VOID SCA_FreeNode(struct ScalosNodeList * nodeList, struct MinNode * minNode);
VOID SCA_FreeAllNodes(struct ScalosNodeList * nodeList);
VOID SCA_MoveNode(struct ScalosNodeList * srcNodeList,
	struct ScalosNodeList * destNodeList, struct MinNode * minNode);
VOID SCA_SwapNodes(struct MinNode * minNode1, struct MinNode * minNode2,
	struct ScalosNodeList * nodeList);
BOOL SCA_OpenIconWindow(CONST struct TagItem * tagList);
BOOL SCA_OpenIconWindowTags(ULONG tagList, ...);
struct ScaWindowList * SCA_LockWindowList(LONG accessmode);
VOID SCA_UnLockWindowList(void);
struct ScalosMessage * SCA_AllocMessage(ULONG messagetype, ULONG additional_size);
VOID SCA_FreeMessage(struct ScalosMessage * message);
struct DragHandle * SCA_InitDrag(struct Screen * screen);
VOID SCA_EndDrag(struct DragHandle * dragHandle);
BOOL SCA_AddBob(struct DragHandle * dragHandle, struct BitMap * bm, APTR mask,
	ULONG width, ULONG height, LONG xOffset, LONG yOffset);
VOID SCA_DrawDrag(struct DragHandle * dragHandle, LONG x, LONG y, ULONG flags);
VOID SCA_UpdateIcon(ULONG windowType, APTR updateIconStruct, ULONG ui_SIZE);
ULONG SCA_MakeWBArgs(struct WBArg * buffer, struct ScaIconNode * in, ULONG argsSize);
VOID SCA_FreeWBArgs(struct WBArg * buffer, ULONG number, ULONG flags);
VOID SCA_ScreenTitleMsg(CONST_STRPTR format, APTR args);
VOID SCA_ScreenTitleMsgArgs(CONST_STRPTR format, ULONG args, ...);
struct ScalosClass * SCA_MakeScalosClass(CONST_STRPTR className, CONST_STRPTR superClassName,
	ULONG instSize, APTR dispFunc);
BOOL SCA_FreeScalosClass(struct ScalosClass * scalosClass);
Object * SCA_NewScalosObject(CONST_STRPTR className, CONST struct TagItem * tagList);
Object * SCA_NewScalosObjectTags(CONST_STRPTR className, ULONG tagList, ...);
VOID SCA_DisposeScalosObject(Object * obj);
ULONG SCA_ScalosControlA(CONST_STRPTR name, CONST struct TagItem * tagList);
ULONG SCA_ScalosControl(CONST_STRPTR name, ULONG tagList, ...);
ULONG SCA_ScalosControlTags(CONST_STRPTR name, ULONG tagList, ...);
Object * SCA_GetDefIconObject(BPTR dirLock, CONST_STRPTR name);
struct ScaWindowStruct * SCA_OpenDrawerByName(CONST_STRPTR path, struct TagItem * tagList);
struct ScaWindowStruct * SCA_OpenDrawerByNameTags(CONST_STRPTR path, ULONG tagList, ...);
ULONG SCA_CountWBArgs(struct ScaIconNode * in);
Object * SCA_GetDefIconObjectA(BPTR dirLock, CONST_STRPTR name, struct TagItem * tagList);
Object * SCA_GetDefIconObjectTags(BPTR dirLock, CONST_STRPTR name, ULONG tagList, ...);
ULONG SCA_LockDrag(struct DragHandle * dragHandle);
ULONG SCA_UnlockDrag(struct DragHandle * dragHandle);

#endif	/*  CLIB_SCALOS_PROTOS_H  */
