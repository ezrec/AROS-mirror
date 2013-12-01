#ifndef _INLINE_SCALOS_H
#define _INLINE_SCALOS_H

#ifndef CLIB_SCALOS_PROTOS_H
#define CLIB_SCALOS_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef  WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif
#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif

#ifndef SCALOS_BASE_NAME
#define SCALOS_BASE_NAME ScalosBase
#endif

#define SCA_WBStart(argArray, tagList, numArgs) \
	LP3(0x1e, BOOL, SCA_WBStart, APTR, argArray, a0, CONST struct TagItem *, tagList, a1, ULONG, numArgs, d0, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_WBStartTags(argArray, numArgs, tags...) \
       ({ULONG _tags[] = {tags}; SCA_WBStart((argArray), (struct TagItem *) _tags, (numArgs));})
#endif

#define SCA_SortNodes(nodelist, sortHook, sortType) \
	LP3NR(0x24, SCA_SortNodes, struct ScalosNodeList *, nodelist, a0, struct Hook *, sortHook, a1, ULONG, sortType, d0, \
	, SCALOS_BASE_NAME)

#define SCA_NewAddAppIcon(iD, userData, iconObj, msgPort, tagList) \
	LP5(0x2a, struct AppObject *, SCA_NewAddAppIcon, ULONG, iD, d0, ULONG, userData, d1, Object *, iconObj, a0, struct MsgPort *, msgPort, a1, CONST struct TagItem *, tagList, a2, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppIconTags(iD, userData, iconObj, msgPort, tags...) \
	({ULONG _tags[] = {tags}; SCA_NewAddAppIcon((iD), (userData), (iconObj), (msgPort), (CONST struct TagItem *) _tags);})
#endif

#define SCA_RemoveAppObject(appObj) \
	LP1(0x30, BOOL, SCA_RemoveAppObject, struct AppObject *, appObj, a0, \
	, SCALOS_BASE_NAME)

#define SCA_NewAddAppWindow(iD, userData, win, msgPort, tagList) \
	LP5(0x36, struct AppObject *, SCA_NewAddAppWindow, ULONG, iD, d0, ULONG, userData, d1, struct Window *, win, a0, struct MsgPort *, msgPort, a1, CONST struct TagItem *, tagList, a2, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppWindowTags(iD, userData, win, msgPort, tags...) \
	({ULONG _tags[] = {tags}; SCA_NewAddAppWindow((iD), (userData), (win), (msgPort), (CONST struct TagItem *) _tags);})
#endif

#define SCA_NewAddAppMenuItem(iD, userData, text, msgPort, tagList) \
	LP5(0x3c, struct AppObject *, SCA_NewAddAppMenuItem, ULONG, iD, d0, ULONG, userData, d1, CONST_STRPTR, text, a0, struct MsgPort *, msgPort, a1, CONST struct TagItem *, tagList, a2, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppMenuItemTags(iD, userData, text, msgPort, tags...) \
	({ULONG _tags[] = {tags}; SCA_NewAddAppMenuItem((iD), (userData), (text), (msgPort), (CONST struct TagItem *) _tags);})
#endif

#define SCA_AllocStdNode(nodeList, nodeType) \
	LP2(0x42, struct MinNode *, SCA_AllocStdNode, struct ScalosNodeList *, nodeList, a0, ULONG, nodeType, d0, \
	, SCALOS_BASE_NAME)

#define SCA_AllocNode(nodeList, size) \
	LP2(0x48, struct MinNode *, SCA_AllocNode, struct ScalosNodeList *, nodeList, a0, ULONG, size, d0, \
	, SCALOS_BASE_NAME)

#define SCA_FreeNode(nodeList, minNode) \
	LP2NR(0x4e, SCA_FreeNode, struct ScalosNodeList *, nodeList, a0, struct MinNode *, minNode, a1, \
	, SCALOS_BASE_NAME)

#define SCA_FreeAllNodes(nodeList) \
	LP1NR(0x54, SCA_FreeAllNodes, struct ScalosNodeList *, nodeList, a0, \
	, SCALOS_BASE_NAME)

#define SCA_MoveNode(srcNodeList, destNodeList, minNode) \
	LP3NR(0x5a, SCA_MoveNode, struct ScalosNodeList *, srcNodeList, a0, struct ScalosNodeList *, destNodeList, a1, struct MinNode *, minNode, d0, \
	, SCALOS_BASE_NAME)

#define SCA_SwapNodes(minNode1, minNode2, nodeList) \
	LP3NR(0x60, SCA_SwapNodes, struct MinNode *, minNode1, a0, struct MinNode *, minNode2, a1, struct ScalosNodeList *, nodeList, a2, \
	, SCALOS_BASE_NAME)

#define SCA_OpenIconWindow(tagList) \
	LP1(0x66, BOOL, SCA_OpenIconWindow, CONST struct TagItem *, tagList, a0, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_OpenIconWindowTags(tags...) \
	({ULONG _tags[] = {tags}; SCA_OpenIconWindow((CONST struct TagItem *) _tags);})
#endif

#define SCA_LockWindowList(accessmode) \
	LP1(0x6c, struct ScaWindowList *, SCA_LockWindowList, LONG, accessmode, d0, \
	, SCALOS_BASE_NAME)

#define SCA_UnLockWindowList() \
	LP0NR(0x72, SCA_UnLockWindowList, \
	, SCALOS_BASE_NAME)

#define SCA_AllocMessage(messagetype, additional_size) \
	LP2(0x78, struct ScalosMessage *, SCA_AllocMessage, ULONG, messagetype, d0, ULONG, additional_size, d1, \
	, SCALOS_BASE_NAME)

#define SCA_FreeMessage(message) \
	LP1NR(0x7e, SCA_FreeMessage, struct ScalosMessage *, message, a1, \
	, SCALOS_BASE_NAME)

#define SCA_InitDrag(screen) \
	LP1(0x84, struct DragHandle *, SCA_InitDrag, struct Screen *, screen, a0, \
	, SCALOS_BASE_NAME)

#define SCA_EndDrag(dragHandle) \
	LP1NR(0x8a, SCA_EndDrag, struct DragHandle *, dragHandle, a0, \
	, SCALOS_BASE_NAME)

#define SCA_AddBob(dragHandle, bm, mask, width, height, xOffset, yOffset) \
	LP7(0x90, BOOL, SCA_AddBob, struct DragHandle *, dragHandle, a0, struct BitMap *, bm, a1, APTR, mask, a2, ULONG, width, d0, ULONG, height, d1, LONG, xOffset, d2, LONG, yOffset, d3, \
	, SCALOS_BASE_NAME)

#define SCA_DrawDrag(dragHandle, x, y, flags) \
	LP4NR(0x96, SCA_DrawDrag, struct DragHandle *, dragHandle, a0, LONG, x, d0, LONG, y, d1, ULONG, flags, d2, \
	, SCALOS_BASE_NAME)

#define SCA_UpdateIcon(windowType, updateIconStruct, ui_SIZE) \
	LP3NR(0x9c, SCA_UpdateIcon, ULONG, windowType, d0, APTR, updateIconStruct, a0, ULONG, ui_SIZE, d1, \
	, SCALOS_BASE_NAME)

#define SCA_MakeWBArgs(buffer, in, argsSize) \
	LP3(0xa2, ULONG, SCA_MakeWBArgs, struct WBArg *, buffer, a0, struct ScaIconNode *, in, a1, ULONG, argsSize, d0, \
	, SCALOS_BASE_NAME)

#define SCA_FreeWBArgs(buffer, number, flags) \
	LP3NR(0xa8, SCA_FreeWBArgs, struct WBArg *, buffer, a0, ULONG, number, d0, ULONG, flags, d1, \
	, SCALOS_BASE_NAME)

#define SCA_ScreenTitleMsg(format, args) \
	LP2NR(0xb4, SCA_ScreenTitleMsg, CONST_STRPTR, format, a0, APTR, args, a1, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_ScreenTitleMsgArgs(format, tags...) \
	({ULONG _tags[] = {tags}; SCA_ScreenTitleMsg((format), (APTR) _tags);})
#endif

#define SCA_MakeScalosClass(className, superClassName, instSize, dispFunc) \
	LP4(0xba, struct ScalosClass *, SCA_MakeScalosClass, CONST_STRPTR, className, a0, CONST_STRPTR, superClassName, a1, ULONG, instSize, d0, APTR, dispFunc, a2, \
	, SCALOS_BASE_NAME)

#define SCA_FreeScalosClass(scalosClass) \
	LP1(0xc0, BOOL, SCA_FreeScalosClass, struct ScalosClass *, scalosClass, a0, \
	, SCALOS_BASE_NAME)

#define SCA_NewScalosObject(className, tagList) \
	LP2(0xc6, Object *, SCA_NewScalosObject, CONST_STRPTR, className, a0, CONST struct TagItem *, tagList, a1, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_NewScalosObjectTags(className, tags...) \
	({ULONG _tags[] = {tags}; SCA_NewScalosObject((className), (CONST struct TagItem *) _tags);})
#endif

#define SCA_DisposeScalosObject(obj) \
	LP1NR(0xcc, SCA_DisposeScalosObject, Object *, obj, a0, \
	, SCALOS_BASE_NAME)

#define SCA_ScalosControlA(name, tagList) \
	LP2(0xd2, ULONG, SCA_ScalosControlA, CONST_STRPTR, name, a0, CONST struct TagItem *, tagList, a1, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_ScalosControl(name, tags...) \
	({ULONG _tags[] = {tags}; SCA_ScalosControlA((name), (CONST struct TagItem *) _tags);})
#endif

#ifndef NO_INLINE_STDARG
#define SCA_ScalosControlTags SCA_ScalosControl
#endif

#define SCA_GetDefIconObject(dirLock, name) \
	LP2(0xd8, Object *, SCA_GetDefIconObject, BPTR, dirLock, a0, CONST_STRPTR, name, a1, \
	, SCALOS_BASE_NAME)

#define SCA_OpenDrawerByName(path, tagList) \
	LP2(0xde, struct ScaWindowStruct *, SCA_OpenDrawerByName, CONST_STRPTR, path, a0, struct TagItem *, tagList, a1, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_OpenDrawerByNameTags(path, tags...) \
	({ULONG _tags[] = {tags}; SCA_OpenDrawerByName((path), (struct TagItem *) _tags);})
#endif

#define SCA_CountWBArgs(in) \
	LP1(0xe4, ULONG, SCA_CountWBArgs, struct ScaIconNode *, in, a0, \
	, SCALOS_BASE_NAME)

#define SCA_GetDefIconObjectA(dirLock, name, tagList) \
	LP3(0xea, Object *, SCA_GetDefIconObjectA, BPTR, dirLock, a0, CONST_STRPTR, name, a1, struct TagItem *, tagList, a2, \
	, SCALOS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCA_GetDefIconObjectTags(dirLock, name, tags...) \
	({ULONG _tags[] = {tags}; SCA_GetDefIconObjectA((dirLock), (name), (struct TagItem *) _tags);})
#endif

#define SCA_LockDrag(dragHandle) \
	LP1(0xf0, ULONG, SCA_LockDrag, struct DragHandle *, dragHandle, a0, \
	, SCALOS_BASE_NAME)

#define SCA_UnlockDrag(dragHandle) \
	LP1(0xf6, ULONG, SCA_UnlockDrag, struct DragHandle *, dragHandle, a0, \
	, SCALOS_BASE_NAME)

#endif /*  _INLINE_SCALOS_H  */
