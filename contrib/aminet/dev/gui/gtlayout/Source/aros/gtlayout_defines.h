#include <aros/libcall.h>
#include <exec/types.h>

#ifndef LT_Activate
#define LT_Activate(handle,id) \
	AROS_LC2(VOID ,LT_Activate, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(d0) LONG,id,D0),\
	struct GTLayoutBase *, GTLayoutBase, 29, GTLayout)
#endif

#ifndef LT_BuildA
#define LT_BuildA(handle,TagParams) \
	AROS_LC2(struct Window * ,LT_BuildA, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) struct TagItem *,TagParams,A1),\
	struct GTLayoutBase *, GTLayoutBase, 34, GTLayout)
#endif

#ifndef LT_LayoutA
#define LT_LayoutA(handle,title,bounds,extraWidth,extraHeight,IDCMP,align,TagParams) \
	AROS_LC8(struct Window * ,LT_LayoutA, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) STRPTR,title,A1),\
	AROS_LCA(REG(a2) struct IBox *,bounds,A2),\
	AROS_LCA(REG(d0) LONG,extraWidth,D0),\
	AROS_LCA(REG(d1) LONG,extraHeight,D1),\
	AROS_LCA(REG(d2) ULONG,IDCMP,D2),\
	AROS_LCA(REG(d3) LONG,align,D3),\
	AROS_LCA(REG(a3) struct TagItem *,TagParams,A3),\
	struct GTLayoutBase *, GTLayoutBase, 18, GTLayout)
#endif

#ifndef LT_GetWindowUserData
#define LT_GetWindowUserData(Window,DefaultValue) \
	AROS_LC2(APTR ,LT_GetWindowUserData, \
	AROS_LCA(REG(a0) struct Window *,Window,A0),\
	AROS_LCA(REG(a1) APTR,DefaultValue,A1),\
	struct GTLayoutBase *, GTLayoutBase, 46, GTLayout)
#endif

#ifndef LT_CreateHandle
#define LT_CreateHandle(Screen,Font) \
	AROS_LC2(LayoutHandle * ,LT_CreateHandle, \
	AROS_LCA(REG(a0) struct Screen *,Screen,A0),\
	AROS_LCA(REG(a1) struct TextAttr *,Font,A1),\
	struct GTLayoutBase *, GTLayoutBase, 7, GTLayout)
#endif

#ifndef LT_CreateHandleTagList
#define LT_CreateHandleTagList(Screen,TagList) \
	AROS_LC2(LayoutHandle * ,LT_CreateHandleTagList, \
	AROS_LCA(REG(a0) struct Screen *,Screen,A0),\
	AROS_LCA(REG(a1) struct TagItem *,TagList,A1),\
	struct GTLayoutBase *, GTLayoutBase, 8, GTLayout)
#endif

#ifndef LT_DeleteHandle
#define LT_DeleteHandle(Handle) \
	AROS_LC1(VOID ,LT_DeleteHandle, \
	AROS_LCA(REG(a0) LayoutHandle *,Handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 6, GTLayout)
#endif

#ifndef LT_DisposeMenu
#define LT_DisposeMenu(menu) \
	AROS_LC1(VOID ,LT_DisposeMenu, \
	AROS_LCA(REG(a0) struct Menu *,menu,A0),\
	struct GTLayoutBase *, GTLayoutBase, 37, GTLayout)
#endif

#ifndef LT_FindMenuCommand
#define LT_FindMenuCommand(Menu,MsgCode,MsgQualifier,MsgGadget) \
	AROS_LC4(struct MenuItem * ,LT_FindMenuCommand, \
	AROS_LCA(REG(a0) struct Menu *,Menu,A0),\
	AROS_LCA(REG(d0) UWORD,MsgCode,D0),\
	AROS_LCA(REG(d1) UWORD,MsgQualifier,D1),\
	AROS_LCA(REG(a1) struct Gadget *,MsgGadget,A1),\
	struct GTLayoutBase *, GTLayoutBase, 42, GTLayout)
#endif

#ifndef LT_GetAttributesA
#define LT_GetAttributesA(Handle,ID,TagList) \
	AROS_LC3(LONG ,LT_GetAttributesA, \
	AROS_LCA(REG(a0) LayoutHandle *,Handle,A0),\
	AROS_LCA(REG(d0) LONG,ID,D0),\
	AROS_LCA(REG(a1) struct TagItem *,TagList,A1),\
	struct GTLayoutBase *, GTLayoutBase, 13, GTLayout)
#endif

#ifndef LT_GetCode
#define LT_GetCode(MsgQualifier,MsgClass,MsgCode,MsgGadget) \
	AROS_LC4(LONG ,LT_GetCode, \
	AROS_LCA(REG(d0) ULONG,MsgQualifier,D0),\
	AROS_LCA(REG(d1) ULONG,MsgClass,D1),\
	AROS_LCA(REG(d2) UWORD,MsgCode,D2),\
	AROS_LCA(REG(a0) struct Gadget *,MsgGadget,A0),\
	struct GTLayoutBase *, GTLayoutBase, 31, GTLayout)
#endif

#ifndef LT_GetMenuItem
#define LT_GetMenuItem(Menu,ID) \
	AROS_LC2(struct MenuItem * ,LT_GetMenuItem, \
	AROS_LCA(REG(a0) struct Menu *,Menu,A0),\
	AROS_LCA(REG(d0) ULONG,ID,D0),\
	struct GTLayoutBase *, GTLayoutBase, 41, GTLayout)
#endif

#ifndef LT_HandleInput
#define LT_HandleInput(Handle,MsgQualifier,MsgClass,MsgCode,MsgGadget) \
	AROS_LC5(VOID ,LT_HandleInput, \
	AROS_LCA(REG(a0) LayoutHandle *,Handle,A0),\
	AROS_LCA(REG(d0) ULONG,MsgQualifier,D0),\
	AROS_LCA(REG(a1) ULONG *,MsgClass,A1),\
	AROS_LCA(REG(a2) UWORD *,MsgCode,A2),\
	AROS_LCA(REG(a3) struct Gadget **,MsgGadget,A3),\
	struct GTLayoutBase *, GTLayoutBase, 10, GTLayout)
#endif

#ifndef LT_GetIMsg
#define LT_GetIMsg(Handle) \
	AROS_LC1(struct IntuiMessage * ,LT_GetIMsg, \
	AROS_LCA(REG(a0) struct LayoutHandle *,Handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 32, GTLayout)
#endif

#ifndef LT_ReplyIMsg
#define LT_ReplyIMsg(Msg) \
	AROS_LC1(VOID ,LT_ReplyIMsg, \
	AROS_LCA(REG(a0) struct IntuiMessage *,Msg,A0),\
	struct GTLayoutBase *, GTLayoutBase, 33, GTLayout)
#endif

#ifndef LT_LabelWidth
#define LT_LabelWidth(handle,label) \
	AROS_LC2(LONG ,LT_LabelWidth, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) STRPTR,label,A1),\
	struct GTLayoutBase *, GTLayoutBase, 23, GTLayout)
#endif

#ifndef LT_LabelChars
#define LT_LabelChars(handle,label) \
	AROS_LC2(LONG ,LT_LabelChars, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) STRPTR,label,A1),\
	struct GTLayoutBase *, GTLayoutBase, 24, GTLayout)
#endif

#ifndef LT_LayoutMenusA
#define LT_LayoutMenusA(handle,menuTemplate,TagParams) \
	AROS_LC3(struct Menu * ,LT_LayoutMenusA, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) struct NewMenu *,menuTemplate,A1),\
	AROS_LCA(REG(a2) struct TagItem *,TagParams,A2),\
	struct GTLayoutBase *, GTLayoutBase, 19, GTLayout)
#endif

#ifndef LT_LevelWidth
#define LT_LevelWidth(handle,levelFormat,dispFunc,min,max,maxWidth,maxLen,fullCheck) \
	AROS_LC8(VOID ,LT_LevelWidth, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) STRPTR,levelFormat,A1),\
	AROS_LCA(REG(a2) DISPFUNC,dispFunc,A2),\
	AROS_LCA(REG(d0) LONG,min,D0),\
	AROS_LCA(REG(d1) LONG,max,D1),\
	AROS_LCA(REG(a3) LONG *,maxWidth,A3),\
	AROS_LCA(REG(a5) LONG *,maxLen,A5),\
	AROS_LCA(REG(d2) BOOL,fullCheck,D2),\
	struct GTLayoutBase *, GTLayoutBase, 5, GTLayout)
#endif

#ifndef LT_NewLevelWidth
#define LT_NewLevelWidth(handle,levelFormat,dispFunc,min,max,maxWidth,maxLen,fullCheck) \
	AROS_LC8(VOID ,LT_NewLevelWidth, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) STRPTR,levelFormat,A1),\
	AROS_LCA(REG(a2) DISPFUNC,dispFunc,A2),\
	AROS_LCA(REG(d0) LONG,min,D0),\
	AROS_LCA(REG(d1) LONG,max,D1),\
	AROS_LCA(REG(a3) LONG *,maxWidth,A3),\
	AROS_LCA(REG(d3) LONG *,maxLen,D3),\
	AROS_LCA(REG(d2) BOOL,fullCheck,D2),\
	struct GTLayoutBase *, GTLayoutBase, 43, GTLayout)
#endif

#ifndef LT_LockWindow
#define LT_LockWindow(window) \
	AROS_LC1(VOID ,LT_LockWindow, \
	AROS_LCA(REG(a0) struct Window *,window,A0),\
	struct GTLayoutBase *, GTLayoutBase, 25, GTLayout)
#endif

#ifndef LT_UnlockWindow
#define LT_UnlockWindow(window) \
	AROS_LC1(VOID ,LT_UnlockWindow, \
	AROS_LCA(REG(a0) struct Window *,window,A0),\
	struct GTLayoutBase *, GTLayoutBase, 26, GTLayout)
#endif

#ifndef LT_DeleteWindowLock
#define LT_DeleteWindowLock(window) \
	AROS_LC1(VOID ,LT_DeleteWindowLock, \
	AROS_LCA(REG(a0) struct Window *,window,A0),\
	struct GTLayoutBase *, GTLayoutBase, 27, GTLayout)
#endif

#ifndef LT_MenuControlTagList
#define LT_MenuControlTagList(Window,IntuitionMenu,Tags) \
	AROS_LC3(VOID ,LT_MenuControlTagList, \
	AROS_LCA(REG(a0) struct Window *,Window,A0),\
	AROS_LCA(REG(a1) struct Menu *,IntuitionMenu,A1),\
	AROS_LCA(REG(a2) struct TagItem *,Tags,A2),\
	struct GTLayoutBase *, GTLayoutBase, 40, GTLayout)
#endif

#ifndef LT_AddA
#define LT_AddA(Handle,Type,Label,ID,TagList) \
	AROS_LC5(VOID ,LT_AddA, \
	AROS_LCA(REG(a0) LayoutHandle *,Handle,A0),\
	AROS_LCA(REG(d0) LONG,Type,D0),\
	AROS_LCA(REG(d1) STRPTR,Label,D1),\
	AROS_LCA(REG(d2) LONG,ID,D2),\
	AROS_LCA(REG(a1) struct TagItem *,TagList,A1),\
	struct GTLayoutBase *, GTLayoutBase, 15, GTLayout)
#endif

#ifndef LT_NewA
#define LT_NewA(handle,tagList) \
	AROS_LC2(VOID ,LT_NewA, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) struct TagItem *,tagList,A1),\
	struct GTLayoutBase *, GTLayoutBase, 16, GTLayout)
#endif

#ifndef LT_EndGroup
#define LT_EndGroup(handle) \
	AROS_LC1(VOID ,LT_EndGroup, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 17, GTLayout)
#endif

#ifndef LT_NewMenuTagList
#define LT_NewMenuTagList(TagList) \
	AROS_LC1(struct Menu * ,LT_NewMenuTagList, \
	AROS_LCA(REG(a0) struct TagItem *,TagList,A0),\
	struct GTLayoutBase *, GTLayoutBase, 39, GTLayout)
#endif

#ifndef LT_NewMenuTemplate
#define LT_NewMenuTemplate(screen,textAttr,AmigaGlyph,CheckGlyph,ErrorPtr,MenuTemplate) \
	AROS_LC6(struct Menu * ,LT_NewMenuTemplate, \
	AROS_LCA(REG(a0) struct Screen *,screen,A0),\
	AROS_LCA(REG(a1) struct TextAttr *,textAttr,A1),\
	AROS_LCA(REG(a2) struct Image *,AmigaGlyph,A2),\
	AROS_LCA(REG(a3) struct Image *,CheckGlyph,A3),\
	AROS_LCA(REG(d0) LONG *,ErrorPtr,D0),\
	AROS_LCA(REG(d1) struct NewMenu *,MenuTemplate,D1),\
	struct GTLayoutBase *, GTLayoutBase, 38, GTLayout)
#endif

#ifndef LT_PressButton
#define LT_PressButton(handle,id) \
	AROS_LC2(BOOL ,LT_PressButton, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(d0) LONG,id,D0),\
	struct GTLayoutBase *, GTLayoutBase, 30, GTLayout)
#endif

#ifndef LT_RebuildTagList
#define LT_RebuildTagList(handle,clear,TagParams) \
	AROS_LC3(BOOL ,LT_RebuildTagList, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(d0) BOOL,clear,D0),\
	AROS_LCA(REG(a1) struct TagItem *,TagParams,A1),\
	struct GTLayoutBase *, GTLayoutBase, 35, GTLayout)
#endif

#ifndef LT_Rebuild
#define LT_Rebuild(handle,bounds,extraWidth,extraHeight,clear) \
	AROS_LC5(BOOL ,LT_Rebuild, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) struct IBox *,bounds,A1),\
	AROS_LCA(REG(a2) LONG,extraWidth,A2),\
	AROS_LCA(REG(d0) LONG,extraHeight,D0),\
	AROS_LCA(REG(d1) BOOL,clear,D1),\
	struct GTLayoutBase *, GTLayoutBase, 9, GTLayout)
#endif

#ifndef LT_BeginRefresh
#define LT_BeginRefresh(handle) \
	AROS_LC1(VOID ,LT_BeginRefresh, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 11, GTLayout)
#endif

#ifndef LT_EndRefresh
#define LT_EndRefresh(handle,complete) \
	AROS_LC2(VOID ,LT_EndRefresh, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(d0) BOOL,complete,D0),\
	struct GTLayoutBase *, GTLayoutBase, 12, GTLayout)
#endif

#ifndef LT_Refresh
#define LT_Refresh(handle) \
	AROS_LC1(VOID ,LT_Refresh, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 44, GTLayout)
#endif

#ifndef LT_CatchUpRefresh
#define LT_CatchUpRefresh(handle) \
	AROS_LC1(VOID ,LT_CatchUpRefresh, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 45, GTLayout)
#endif

#ifndef LT_SetAttributesA
#define LT_SetAttributesA(handle,id,TagList) \
	AROS_LC3(VOID ,LT_SetAttributesA, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(d0) LONG,id,D0),\
	AROS_LCA(REG(a1) struct TagItem *,TagList,A1),\
	struct GTLayoutBase *, GTLayoutBase, 14, GTLayout)
#endif

#ifndef LT_ShowWindow
#define LT_ShowWindow(handle,activate) \
	AROS_LC2(VOID ,LT_ShowWindow, \
	AROS_LCA(REG(a0) LayoutHandle *,handle,A0),\
	AROS_LCA(REG(a1) BOOL,activate,A1),\
	struct GTLayoutBase *, GTLayoutBase, 28, GTLayout)
#endif

#ifndef LT_UpdateStrings
#define LT_UpdateStrings(Handle) \
	AROS_LC1(VOID ,LT_UpdateStrings, \
	AROS_LCA(REG(a0) LayoutHandle *,Handle,A0),\
	struct GTLayoutBase *, GTLayoutBase, 36, GTLayout)
#endif

