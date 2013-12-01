/* Automatically generated header! Do not edit! */

/*******************************************************************************/
/* Different order of arguments for SCA_WBStart/SCA_WBStartTags manually fixed */
/*******************************************************************************/

#ifndef _INLINE_SCALOS_LIB_SFD_H
#define _INLINE_SCALOS_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOS_LIB_SFD_BASE_NAME
#define SCALOS_LIB_SFD_BASE_NAME ScalosBase
#endif /* !SCALOS_LIB_SFD_BASE_NAME */

#define SCA_WBStart(___argArray, ___tagList, ___numArgs) __SCA_WBStart_WB(SCALOS_LIB_SFD_BASE_NAME, ___argArray, ___tagList, ___numArgs)
#define __SCA_WBStart_WB(___base, ___argArray, ___tagList, ___numArgs) \
	AROS_LC3(BOOL, SCA_WBStart, \
	AROS_LCA(APTR, (___argArray), A0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	AROS_LCA(ULONG, (___numArgs), D0), \
	struct Library *, (___base), 5, Scalos_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define SCA_WBStartTags(___argArray, ___numArgs, ___firstTag, ...) __SCA_WBStartTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___argArray, ___numArgs, ___firstTag, ## __VA_ARGS__)
#define __SCA_WBStartTags_WB(___base, ___argArray, ___numArgs, ___firstTag, ...) \
	({IPTR _message[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_WBStart_WB((___base), (___argArray), (ULONG) _message, (___numArgs)); })
#endif /* !NO_INLINE_VARARGS */

#define SCA_SortNodes(___nodelist, ___sortHook, ___sortType) __SCA_SortNodes_WB(SCALOS_LIB_SFD_BASE_NAME, ___nodelist, ___sortHook, ___sortType)
#define __SCA_SortNodes_WB(___base, ___nodelist, ___sortHook, ___sortType) \
	AROS_LC3NR(VOID, SCA_SortNodes, \
	AROS_LCA(struct ScalosNodeList *, (___nodelist), A0), \
	AROS_LCA(struct Hook *, (___sortHook), A1), \
	AROS_LCA(ULONG, (___sortType), D0), \
	struct Library *, (___base), 6, Scalos_lib_sfd)

#define SCA_NewAddAppIcon(___iD, ___userData, ___iconObj, ___msgPort, ___tagList) __SCA_NewAddAppIcon_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___iconObj, ___msgPort, ___tagList)
#define __SCA_NewAddAppIcon_WB(___base, ___iD, ___userData, ___iconObj, ___msgPort, ___tagList) \
	AROS_LC5(struct AppObject *, SCA_NewAddAppIcon, \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___userData), D1), \
	AROS_LCA(Object *, (___iconObj), A0), \
	AROS_LCA(struct MsgPort *, (___msgPort), A1), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 7, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppIconTags(___iD, ___userData, ___iconObj, ___msgPort, ___firstTag, ...) __SCA_NewAddAppIconTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___iconObj, ___msgPort, ___firstTag, ## __VA_ARGS__)
#define __SCA_NewAddAppIconTags_WB(___base, ___iD, ___userData, ___iconObj, ___msgPort, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_NewAddAppIcon_WB((___base), (___iD), (___userData), (___iconObj), (___msgPort), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_RemoveAppObject(___appObj) __SCA_RemoveAppObject_WB(SCALOS_LIB_SFD_BASE_NAME, ___appObj)
#define __SCA_RemoveAppObject_WB(___base, ___appObj) \
	AROS_LC1(BOOL, SCA_RemoveAppObject, \
	AROS_LCA(struct AppObject *, (___appObj), A0), \
	struct Library *, (___base), 8, Scalos_lib_sfd)

#define SCA_NewAddAppWindow(___iD, ___userData, ___win, ___msgPort, ___tagList) __SCA_NewAddAppWindow_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___win, ___msgPort, ___tagList)
#define __SCA_NewAddAppWindow_WB(___base, ___iD, ___userData, ___win, ___msgPort, ___tagList) \
	AROS_LC5(struct AppObject *, SCA_NewAddAppWindow, \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___userData), D1), \
	AROS_LCA(struct Window *, (___win), A0), \
	AROS_LCA(struct MsgPort *, (___msgPort), A1), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 9, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppWindowTags(___iD, ___userData, ___win, ___msgPort, ___firstTag, ...) __SCA_NewAddAppWindowTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___win, ___msgPort, ___firstTag, ## __VA_ARGS__)
#define __SCA_NewAddAppWindowTags_WB(___base, ___iD, ___userData, ___win, ___msgPort, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_NewAddAppWindow_WB((___base), (___iD), (___userData), (___win), (___msgPort), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_NewAddAppMenuItem(___iD, ___userData, ___text, ___msgPort, ___tagList) __SCA_NewAddAppMenuItem_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___text, ___msgPort, ___tagList)
#define __SCA_NewAddAppMenuItem_WB(___base, ___iD, ___userData, ___text, ___msgPort, ___tagList) \
	AROS_LC5(struct AppObject *, SCA_NewAddAppMenuItem, \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___userData), D1), \
	AROS_LCA(CONST_STRPTR, (___text), A0), \
	AROS_LCA(struct MsgPort *, (___msgPort), A1), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 10, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_NewAddAppMenuItemTags(___iD, ___userData, ___text, ___msgPort, ___firstTag, ...) __SCA_NewAddAppMenuItemTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___iD, ___userData, ___text, ___msgPort, ___firstTag, ## __VA_ARGS__)
#define __SCA_NewAddAppMenuItemTags_WB(___base, ___iD, ___userData, ___text, ___msgPort, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_NewAddAppMenuItem_WB((___base), (___iD), (___userData), (___text), (___msgPort), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_AllocStdNode(___nodeList, ___nodeType) __SCA_AllocStdNode_WB(SCALOS_LIB_SFD_BASE_NAME, ___nodeList, ___nodeType)
#define __SCA_AllocStdNode_WB(___base, ___nodeList, ___nodeType) \
	AROS_LC2(struct MinNode *, SCA_AllocStdNode, \
	AROS_LCA(struct ScalosNodeList *, (___nodeList), A0), \
	AROS_LCA(ULONG, (___nodeType), D0), \
	struct Library *, (___base), 11, Scalos_lib_sfd)

#define SCA_AllocNode(___nodeList, ___size) __SCA_AllocNode_WB(SCALOS_LIB_SFD_BASE_NAME, ___nodeList, ___size)
#define __SCA_AllocNode_WB(___base, ___nodeList, ___size) \
	AROS_LC2(struct MinNode *, SCA_AllocNode, \
	AROS_LCA(struct ScalosNodeList *, (___nodeList), A0), \
	AROS_LCA(ULONG, (___size), D0), \
	struct Library *, (___base), 12, Scalos_lib_sfd)

#define SCA_FreeNode(___nodeList, ___minNode) __SCA_FreeNode_WB(SCALOS_LIB_SFD_BASE_NAME, ___nodeList, ___minNode)
#define __SCA_FreeNode_WB(___base, ___nodeList, ___minNode) \
	AROS_LC2NR(VOID, SCA_FreeNode, \
	AROS_LCA(struct ScalosNodeList *, (___nodeList), A0), \
	AROS_LCA(struct MinNode *, (___minNode), A1), \
	struct Library *, (___base), 13, Scalos_lib_sfd)

#define SCA_FreeAllNodes(___nodeList) __SCA_FreeAllNodes_WB(SCALOS_LIB_SFD_BASE_NAME, ___nodeList)
#define __SCA_FreeAllNodes_WB(___base, ___nodeList) \
	AROS_LC1NR(VOID, SCA_FreeAllNodes, \
	AROS_LCA(struct ScalosNodeList *, (___nodeList), A0), \
	struct Library *, (___base), 14, Scalos_lib_sfd)

#define SCA_MoveNode(___srcNodeList, ___destNodeList, ___minNode) __SCA_MoveNode_WB(SCALOS_LIB_SFD_BASE_NAME, ___srcNodeList, ___destNodeList, ___minNode)
#define __SCA_MoveNode_WB(___base, ___srcNodeList, ___destNodeList, ___minNode) \
	AROS_LC3NR(VOID, SCA_MoveNode, \
	AROS_LCA(struct ScalosNodeList *, (___srcNodeList), A0), \
	AROS_LCA(struct ScalosNodeList *, (___destNodeList), A1), \
	AROS_LCA(struct MinNode *, (___minNode), D0), \
	struct Library *, (___base), 15, Scalos_lib_sfd)

#define SCA_SwapNodes(___minNode1, ___minNode2, ___nodeList) __SCA_SwapNodes_WB(SCALOS_LIB_SFD_BASE_NAME, ___minNode1, ___minNode2, ___nodeList)
#define __SCA_SwapNodes_WB(___base, ___minNode1, ___minNode2, ___nodeList) \
	AROS_LC3NR(VOID, SCA_SwapNodes, \
	AROS_LCA(struct MinNode *, (___minNode1), A0), \
	AROS_LCA(struct MinNode *, (___minNode2), A1), \
	AROS_LCA(struct ScalosNodeList *, (___nodeList), A2), \
	struct Library *, (___base), 16, Scalos_lib_sfd)

#define SCA_OpenIconWindow(___tagList) __SCA_OpenIconWindow_WB(SCALOS_LIB_SFD_BASE_NAME, ___tagList)
#define __SCA_OpenIconWindow_WB(___base, ___tagList) \
	AROS_LC1(BOOL, SCA_OpenIconWindow, \
	AROS_LCA(CONST struct TagItem *, (___tagList), A0), \
	struct Library *, (___base), 17, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_OpenIconWindowTags(___firstTag, ...) __SCA_OpenIconWindowTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___firstTag, ## __VA_ARGS__)
#define __SCA_OpenIconWindowTags_WB(___base, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_OpenIconWindow_WB((___base), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_LockWindowList(___accessmode) __SCA_LockWindowList_WB(SCALOS_LIB_SFD_BASE_NAME, ___accessmode)
#define __SCA_LockWindowList_WB(___base, ___accessmode) \
	AROS_LC1(struct ScaWindowList *, SCA_LockWindowList, \
	AROS_LCA(LONG, (___accessmode), D0), \
	struct Library *, (___base), 18, Scalos_lib_sfd)

#define SCA_UnLockWindowList() __SCA_UnLockWindowList_WB(SCALOS_LIB_SFD_BASE_NAME)
#define __SCA_UnLockWindowList_WB(___base) \
	AROS_LC0NR(VOID, SCA_UnLockWindowList, \
	struct Library *, (___base), 19, Scalos_lib_sfd)

#define SCA_AllocMessage(___messagetype, ___additional_size) __SCA_AllocMessage_WB(SCALOS_LIB_SFD_BASE_NAME, ___messagetype, ___additional_size)
#define __SCA_AllocMessage_WB(___base, ___messagetype, ___additional_size) \
	AROS_LC2(struct ScalosMessage *, SCA_AllocMessage, \
	AROS_LCA(ULONG, (___messagetype), D0), \
	AROS_LCA(UWORD, (___additional_size), D1), \
	struct Library *, (___base), 20, Scalos_lib_sfd)

#define SCA_FreeMessage(___message) __SCA_FreeMessage_WB(SCALOS_LIB_SFD_BASE_NAME, ___message)
#define __SCA_FreeMessage_WB(___base, ___message) \
	AROS_LC1NR(VOID, SCA_FreeMessage, \
	AROS_LCA(struct ScalosMessage *, (___message), A1), \
	struct Library *, (___base), 21, Scalos_lib_sfd)

#define SCA_InitDrag(___screen) __SCA_InitDrag_WB(SCALOS_LIB_SFD_BASE_NAME, ___screen)
#define __SCA_InitDrag_WB(___base, ___screen) \
	AROS_LC1(struct DragHandle *, SCA_InitDrag, \
	AROS_LCA(struct Screen *, (___screen), A0), \
	struct Library *, (___base), 22, Scalos_lib_sfd)

#define SCA_EndDrag(___dragHandle) __SCA_EndDrag_WB(SCALOS_LIB_SFD_BASE_NAME, ___dragHandle)
#define __SCA_EndDrag_WB(___base, ___dragHandle) \
	AROS_LC1NR(VOID, SCA_EndDrag, \
	AROS_LCA(struct DragHandle *, (___dragHandle), A0), \
	struct Library *, (___base), 23, Scalos_lib_sfd)

#define SCA_AddBob(___dragHandle, ___bm, ___mask, ___width, ___height, ___xOffset, ___yOffset) __SCA_AddBob_WB(SCALOS_LIB_SFD_BASE_NAME, ___dragHandle, ___bm, ___mask, ___width, ___height, ___xOffset, ___yOffset)
#define __SCA_AddBob_WB(___base, ___dragHandle, ___bm, ___mask, ___width, ___height, ___xOffset, ___yOffset) \
	AROS_LC7(BOOL, SCA_AddBob, \
	AROS_LCA(struct DragHandle *, (___dragHandle), A0), \
	AROS_LCA(struct BitMap *, (___bm), A1), \
	AROS_LCA(APTR, (___mask), A2), \
	AROS_LCA(ULONG, (___width), D0), \
	AROS_LCA(ULONG, (___height), D1), \
	AROS_LCA(LONG, (___xOffset), D2), \
	AROS_LCA(LONG, (___yOffset), D3), \
	struct Library *, (___base), 24, Scalos_lib_sfd)

#define SCA_DrawDrag(___dragHandle, ___x, ___y, ___flags) __SCA_DrawDrag_WB(SCALOS_LIB_SFD_BASE_NAME, ___dragHandle, ___x, ___y, ___flags)
#define __SCA_DrawDrag_WB(___base, ___dragHandle, ___x, ___y, ___flags) \
	AROS_LC4NR(VOID, SCA_DrawDrag, \
	AROS_LCA(struct DragHandle *, (___dragHandle), A0), \
	AROS_LCA(LONG, (___x), D0), \
	AROS_LCA(LONG, (___y), D1), \
	AROS_LCA(ULONG, (___flags), D2), \
	struct Library *, (___base), 25, Scalos_lib_sfd)

#define SCA_UpdateIcon(___windowType, ___updateIconStruct, ___ui_SIZE) __SCA_UpdateIcon_WB(SCALOS_LIB_SFD_BASE_NAME, ___windowType, ___updateIconStruct, ___ui_SIZE)
#define __SCA_UpdateIcon_WB(___base, ___windowType, ___updateIconStruct, ___ui_SIZE) \
	AROS_LC3NR(VOID, SCA_UpdateIcon, \
	AROS_LCA(UBYTE, (___windowType), D0), \
	AROS_LCA(APTR, (___updateIconStruct), A0), \
	AROS_LCA(ULONG, (___ui_SIZE), D1), \
	struct Library *, (___base), 26, Scalos_lib_sfd)

#define SCA_MakeWBArgs(___buffer, ___in, ___argsSize) __SCA_MakeWBArgs_WB(SCALOS_LIB_SFD_BASE_NAME, ___buffer, ___in, ___argsSize)
#define __SCA_MakeWBArgs_WB(___base, ___buffer, ___in, ___argsSize) \
	AROS_LC3(ULONG, SCA_MakeWBArgs, \
	AROS_LCA(struct WBArg *, (___buffer), A0), \
	AROS_LCA(struct ScaIconNode *, (___in), A1), \
	AROS_LCA(ULONG, (___argsSize), D0), \
	struct Library *, (___base), 27, Scalos_lib_sfd)

#define SCA_FreeWBArgs(___buffer, ___number, ___flags) __SCA_FreeWBArgs_WB(SCALOS_LIB_SFD_BASE_NAME, ___buffer, ___number, ___flags)
#define __SCA_FreeWBArgs_WB(___base, ___buffer, ___number, ___flags) \
	AROS_LC3NR(VOID, SCA_FreeWBArgs, \
	AROS_LCA(struct WBArg *, (___buffer), A0), \
	AROS_LCA(ULONG, (___number), D0), \
	AROS_LCA(ULONG, (___flags), D1), \
	struct Library *, (___base), 28, Scalos_lib_sfd)

#define SCA_ScreenTitleMsg(___format, ___args) __SCA_ScreenTitleMsg_WB(SCALOS_LIB_SFD_BASE_NAME, ___format, ___args)
#define __SCA_ScreenTitleMsg_WB(___base, ___format, ___args) \
	AROS_LC2NR(VOID, SCA_ScreenTitleMsg, \
	AROS_LCA(CONST_STRPTR, (___format), A0), \
	AROS_LCA(APTR, (___args), A1), \
	struct Library *, (___base), 30, Scalos_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define SCA_ScreenTitleMsgArgs(___format, ___firstTag, ...) __SCA_ScreenTitleMsgArgs_WB(SCALOS_LIB_SFD_BASE_NAME, ___format, ___firstTag, ## __VA_ARGS__)
#define __SCA_ScreenTitleMsgArgs_WB(___base, ___format, ___firstTag, ...) \
	({IPTR _message[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_ScreenTitleMsg_WB((___base), (___format), (APTR) _message); })
#endif /* !NO_INLINE_VARARGS */

#define SCA_MakeScalosClass(___className, ___superClassName, ___instSize, ___dispFunc) __SCA_MakeScalosClass_WB(SCALOS_LIB_SFD_BASE_NAME, ___className, ___superClassName, ___instSize, ___dispFunc)
#define __SCA_MakeScalosClass_WB(___base, ___className, ___superClassName, ___instSize, ___dispFunc) \
	AROS_LC4(struct ScalosClass *, SCA_MakeScalosClass, \
	AROS_LCA(CONST_STRPTR, (___className), A0), \
	AROS_LCA(CONST_STRPTR, (___superClassName), A1), \
	AROS_LCA(UWORD, (___instSize), D0), \
	AROS_LCA(APTR, (___dispFunc), A2), \
	struct Library *, (___base), 31, Scalos_lib_sfd)

#define SCA_FreeScalosClass(___scalosClass) __SCA_FreeScalosClass_WB(SCALOS_LIB_SFD_BASE_NAME, ___scalosClass)
#define __SCA_FreeScalosClass_WB(___base, ___scalosClass) \
	AROS_LC1(BOOL, SCA_FreeScalosClass, \
	AROS_LCA(struct ScalosClass *, (___scalosClass), A0), \
	struct Library *, (___base), 32, Scalos_lib_sfd)

#define SCA_NewScalosObject(___className, ___tagList) __SCA_NewScalosObject_WB(SCALOS_LIB_SFD_BASE_NAME, ___className, ___tagList)
#define __SCA_NewScalosObject_WB(___base, ___className, ___tagList) \
	AROS_LC2(Object *, SCA_NewScalosObject, \
	AROS_LCA(CONST_STRPTR, (___className), A0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 33, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_NewScalosObjectTags(___className, ___firstTag, ...) __SCA_NewScalosObjectTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___className, ___firstTag, ## __VA_ARGS__)
#define __SCA_NewScalosObjectTags_WB(___base, ___className, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_NewScalosObject_WB((___base), (___className), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_DisposeScalosObject(___obj) __SCA_DisposeScalosObject_WB(SCALOS_LIB_SFD_BASE_NAME, ___obj)
#define __SCA_DisposeScalosObject_WB(___base, ___obj) \
	AROS_LC1NR(VOID, SCA_DisposeScalosObject, \
	AROS_LCA(Object *, (___obj), A0), \
	struct Library *, (___base), 34, Scalos_lib_sfd)

#define SCA_ScalosControlA(___name, ___tagList) __SCA_ScalosControlA_WB(SCALOS_LIB_SFD_BASE_NAME, ___name, ___tagList)
#define __SCA_ScalosControlA_WB(___base, ___name, ___tagList) \
	AROS_LC2(ULONG, SCA_ScalosControlA, \
	AROS_LCA(CONST_STRPTR, (___name), A0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 35, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_ScalosControl(___name, ___firstTag, ...) __SCA_ScalosControl_WB(SCALOS_LIB_SFD_BASE_NAME, ___name, ___firstTag, ## __VA_ARGS__)
#define __SCA_ScalosControl_WB(___base, ___name, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_ScalosControlA_WB((___base), (___name), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#ifndef NO_INLINE_STDARG
#define SCA_ScalosControlTags(___name, ___firstTag, ...) __SCA_ScalosControlTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___name, ___firstTag, ## __VA_ARGS__)
#define __SCA_ScalosControlTags_WB(___base, ___name, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_ScalosControlA_WB((___base), (___name), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_GetDefIconObject(___dirLock, ___name) __SCA_GetDefIconObject_WB(SCALOS_LIB_SFD_BASE_NAME, ___dirLock, ___name)
#define __SCA_GetDefIconObject_WB(___base, ___dirLock, ___name) \
	AROS_LC2(Object *, SCA_GetDefIconObject, \
	AROS_LCA(BPTR, (___dirLock), A0), \
	AROS_LCA(CONST_STRPTR, (___name), A1), \
	struct Library *, (___base), 36, Scalos_lib_sfd)

#define SCA_OpenDrawerByName(___path, ___tagList) __SCA_OpenDrawerByName_WB(SCALOS_LIB_SFD_BASE_NAME, ___path, ___tagList)
#define __SCA_OpenDrawerByName_WB(___base, ___path, ___tagList) \
	AROS_LC2(struct ScaWindowStruct *, SCA_OpenDrawerByName, \
	AROS_LCA(CONST_STRPTR, (___path), A0), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 37, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_OpenDrawerByNameTags(___path, ___firstTag, ...) __SCA_OpenDrawerByNameTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___path, ___firstTag, ## __VA_ARGS__)
#define __SCA_OpenDrawerByNameTags_WB(___base, ___path, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_OpenDrawerByName_WB((___base), (___path), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_CountWBArgs(___in) __SCA_CountWBArgs_WB(SCALOS_LIB_SFD_BASE_NAME, ___in)
#define __SCA_CountWBArgs_WB(___base, ___in) \
	AROS_LC1(ULONG, SCA_CountWBArgs, \
	AROS_LCA(struct ScaIconNode *, (___in), A0), \
	struct Library *, (___base), 38, Scalos_lib_sfd)

#define SCA_GetDefIconObjectA(___dirLock, ___name, ___tagList) __SCA_GetDefIconObjectA_WB(SCALOS_LIB_SFD_BASE_NAME, ___dirLock, ___name, ___tagList)
#define __SCA_GetDefIconObjectA_WB(___base, ___dirLock, ___name, ___tagList) \
	AROS_LC3(Object *, SCA_GetDefIconObjectA, \
	AROS_LCA(BPTR, (___dirLock), A0), \
	AROS_LCA(CONST_STRPTR, (___name), A1), \
	AROS_LCA(struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 39, Scalos_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCA_GetDefIconObjectTags(___dirLock, ___name, ___firstTag, ...) __SCA_GetDefIconObjectTags_WB(SCALOS_LIB_SFD_BASE_NAME, ___dirLock, ___name, ___firstTag, ## __VA_ARGS__)
#define __SCA_GetDefIconObjectTags_WB(___base, ___dirLock, ___name, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCA_GetDefIconObjectA_WB((___base), (___dirLock), (___name), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define SCA_LockDrag(___dragHandle) __SCA_LockDrag_WB(SCALOS_LIB_SFD_BASE_NAME, ___dragHandle)
#define __SCA_LockDrag_WB(___base, ___dragHandle) \
	AROS_LC1(ULONG, SCA_LockDrag, \
	AROS_LCA(struct DragHandle *, (___dragHandle), A0), \
	struct Library *, (___base), 40, Scalos_lib_sfd)

#define SCA_UnlockDrag(___dragHandle) __SCA_UnlockDrag_WB(SCALOS_LIB_SFD_BASE_NAME, ___dragHandle)
#define __SCA_UnlockDrag_WB(___base, ___dragHandle) \
	AROS_LC1(ULONG, SCA_UnlockDrag, \
	AROS_LCA(struct DragHandle *, (___dragHandle), A0), \
	struct Library *, (___base), 41, Scalos_lib_sfd)

#endif /* !_INLINE_SCALOS_LIB_SFD_H */
