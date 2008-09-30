#ifndef CLIB_GTLAYOUT_PROTOS_H
#define CLIB_GTLAYOUT_PROTOS_H

/*
    *** Automatically generated from 'lib.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <gtlayout.h>

struct LayoutHandle *LT_CreateHandleTags( struct Screen *screen, ... );
LONG LT_GetAttributes( struct LayoutHandle *handle, LONG id, ... );
VOID LT_SetAttributes( struct LayoutHandle *handle, LONG id, ... );
VOID LT_Add( struct LayoutHandle *handle, LONG type, STRPTR label, LONG id, ... );
VOID LT_New( struct LayoutHandle *handle, ... );
struct Window *LT_Layout( struct LayoutHandle *handle, STRPTR title, struct IBox *bounds, LONG extraWidth, LONG extraHeight, ULONG idcmp, LONG align, ... );
struct Menu *LT_LayoutMenus( struct LayoutHandle *handle, struct NewMenu *menuTemplate, ... );
struct Window *LT_Build( struct LayoutHandle *handle, ... );
BOOL LT_RebuildTags( struct LayoutHandle *handle, LONG clear, ... );
struct Menu *LT_NewMenuTags( Tag firstTag, ... );

AROS_LP8(void, LT_LevelWidth,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(STRPTR, levelFormat, A1),
         AROS_LPA(DISPFUNC, dispFunc, A2),
         AROS_LPA(LONG, min, D0),
         AROS_LPA(LONG, max, D1),
         AROS_LPA(LONG *, maxWidth, A3),
         AROS_LPA(LONG *, maxLen, A5),
         AROS_LPA(BOOL, fullCheck, D2),
         LIBBASETYPEPTR, GTLayoutBase, 5, GTLayout
);
AROS_LP1(void, LT_DeleteHandle,
         AROS_LPA(LayoutHandle *, Handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 6, GTLayout
);
AROS_LP2(LayoutHandle *, LT_CreateHandle,
         AROS_LPA(struct Screen *, Screen, A0),
         AROS_LPA(struct TextAttr *, Font, A1),
         LIBBASETYPEPTR, GTLayoutBase, 7, GTLayout
);
AROS_LP2(LayoutHandle *, LT_CreateHandleTagList,
         AROS_LPA(struct Screen *, Screen, A0),
         AROS_LPA(struct TagItem *, TagList, A1),
         LIBBASETYPEPTR, GTLayoutBase, 8, GTLayout
);
AROS_LP5(BOOL, LT_Rebuild,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(struct IBox *, bounds, A1),
         AROS_LPA(LONG, extraWidth, A2),
         AROS_LPA(LONG, extraHeight, D0),
         AROS_LPA(BOOL, clear, D1),
         LIBBASETYPEPTR, GTLayoutBase, 9, GTLayout
);
AROS_LP5(void, LT_HandleInput,
         AROS_LPA(LayoutHandle *, Handle, A0),
         AROS_LPA(ULONG, MsgQualifier, D0),
         AROS_LPA(ULONG *, MsgClass, A1),
         AROS_LPA(UWORD *, MsgCode, A2),
         AROS_LPA(struct Gadget **, MsgGadget, A3),
         LIBBASETYPEPTR, GTLayoutBase, 10, GTLayout
);
AROS_LP1(void, LT_BeginRefresh,
         AROS_LPA(LayoutHandle *, handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 11, GTLayout
);
AROS_LP2(void, LT_EndRefresh,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(BOOL, complete, D0),
         LIBBASETYPEPTR, GTLayoutBase, 12, GTLayout
);
AROS_LP3(LONG, LT_GetAttributesA,
         AROS_LPA(LayoutHandle *, Handle, A0),
         AROS_LPA(LONG, ID, D0),
         AROS_LPA(struct TagItem *, TagList, A1),
         LIBBASETYPEPTR, GTLayoutBase, 13, GTLayout
);
AROS_LP3(void, LT_SetAttributesA,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(LONG, id, D0),
         AROS_LPA(struct TagItem *, TagList, A1),
         LIBBASETYPEPTR, GTLayoutBase, 14, GTLayout
);
AROS_LP5(void, LT_AddA,
         AROS_LPA(LayoutHandle *, Handle, A0),
         AROS_LPA(LONG, Type, D0),
         AROS_LPA(STRPTR, Label, D1),
         AROS_LPA(LONG, ID, D2),
         AROS_LPA(struct TagItem *, TagList, A1),
         LIBBASETYPEPTR, GTLayoutBase, 15, GTLayout
);
AROS_LP2(void, LT_NewA,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, GTLayoutBase, 16, GTLayout
);
AROS_LP1(void, LT_EndGroup,
         AROS_LPA(LayoutHandle *, handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 17, GTLayout
);
AROS_LP8(struct Window *, LT_LayoutA,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(STRPTR, title, A1),
         AROS_LPA(struct IBox *, bounds, A2),
         AROS_LPA(LONG, extraWidth, D0),
         AROS_LPA(LONG, extraHeight, D1),
         AROS_LPA(ULONG, IDCMP, D2),
         AROS_LPA(LONG, align, D3),
         AROS_LPA(struct TagItem *, TagParams, A3),
         LIBBASETYPEPTR, GTLayoutBase, 18, GTLayout
);
AROS_LP3(struct Menu *, LT_LayoutMenusA,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(struct NewMenu *, menuTemplate, A1),
         AROS_LPA(struct TagItem *, TagParams, A2),
         LIBBASETYPEPTR, GTLayoutBase, 19, GTLayout
);
AROS_LP2(LONG, LT_LabelWidth,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(STRPTR, label, A1),
         LIBBASETYPEPTR, GTLayoutBase, 23, GTLayout
);
AROS_LP2(LONG, LT_LabelChars,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(STRPTR, label, A1),
         LIBBASETYPEPTR, GTLayoutBase, 24, GTLayout
);
AROS_LP1(void, LT_LockWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, GTLayoutBase, 25, GTLayout
);
AROS_LP1(void, LT_UnlockWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, GTLayoutBase, 26, GTLayout
);
AROS_LP1(void, LT_DeleteWindowLock,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, GTLayoutBase, 27, GTLayout
);
AROS_LP2(void, LT_ShowWindow,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(BOOL, activate, A1),
         LIBBASETYPEPTR, GTLayoutBase, 28, GTLayout
);
AROS_LP2(void, LT_Activate,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(LONG, id, D0),
         LIBBASETYPEPTR, GTLayoutBase, 29, GTLayout
);
AROS_LP2(BOOL, LT_PressButton,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(LONG, id, D0),
         LIBBASETYPEPTR, GTLayoutBase, 30, GTLayout
);
AROS_LP4(LONG, LT_GetCode,
         AROS_LPA(ULONG, MsgQualifier, D0),
         AROS_LPA(ULONG, MsgClass, D1),
         AROS_LPA(UWORD, MsgCode, D2),
         AROS_LPA(struct Gadget *, MsgGadget, A0),
         LIBBASETYPEPTR, GTLayoutBase, 31, GTLayout
);
AROS_LP1(struct IntuiMessage *, LT_GetIMsg,
         AROS_LPA(struct LayoutHandle *, Handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 32, GTLayout
);
AROS_LP1(void, LT_ReplyIMsg,
         AROS_LPA(struct IntuiMessage *, Msg, A0),
         LIBBASETYPEPTR, GTLayoutBase, 33, GTLayout
);
AROS_LP2(struct Window *, LT_BuildA,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(struct TagItem *, TagParams, A1),
         LIBBASETYPEPTR, GTLayoutBase, 34, GTLayout
);
AROS_LP3(BOOL, LT_RebuildTagList,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(BOOL, clear, D0),
         AROS_LPA(struct TagItem *, TagParals, A1),
         LIBBASETYPEPTR, GTLayoutBase, 35, GTLayout
);
AROS_LP1(void, LT_UpdateStrings,
         AROS_LPA(LayoutHandle *, Handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 36, GTLayout
);
AROS_LP1(void, LT_DisposeMenu,
         AROS_LPA(struct Menu *, menu, A0),
         LIBBASETYPEPTR, GTLayoutBase, 37, GTLayout
);
AROS_LP6(struct Menu *, LT_NewMenuTemplate,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct TextAttr *, textAttr, A1),
         AROS_LPA(struct Image *, AmigaGlyph, A2),
         AROS_LPA(struct Image *, CheckGlyph, A3),
         AROS_LPA(LONG *, ErrorPtr, D0),
         AROS_LPA(struct NewMenu *, MenuTemplate, D1),
         LIBBASETYPEPTR, GTLayoutBase, 38, GTLayout
);
AROS_LP1(struct Menu *, LT_NewMenuTagList,
         AROS_LPA(struct TagItem *, TagList, A0),
         LIBBASETYPEPTR, GTLayoutBase, 39, GTLayout
);
AROS_LP3(void, LT_MenuControlTagList,
         AROS_LPA(struct Window *, Window, A0),
         AROS_LPA(struct Menu *, IntuitionMenu, A1),
         AROS_LPA(struct TagItem *, Tags, A2),
         LIBBASETYPEPTR, GTLayoutBase, 40, GTLayout
);
AROS_LP2(struct MenuItem *, LT_GetMenuItem,
         AROS_LPA(struct Menu *, Menu, A0),
         AROS_LPA(ULONG, ID, D0),
         LIBBASETYPEPTR, GTLayoutBase, 41, GTLayout
);
AROS_LP4(struct MenuItem *, LT_FindMenuCommand,
         AROS_LPA(struct Menu *, Menu, A0),
         AROS_LPA(UWORD, MsgCode, D0),
         AROS_LPA(UWORD, MsgQualifier, D1),
         AROS_LPA(struct Gadget *, MsgGadget, A1),
         LIBBASETYPEPTR, GTLayoutBase, 42, GTLayout
);
AROS_LP8(void, LT_NewLevelWidth,
         AROS_LPA(LayoutHandle *, handle, A0),
         AROS_LPA(STRPTR, levelFormat, A1),
         AROS_LPA(DISPFUNC, dispFunc, A2),
         AROS_LPA(LONG, min, D0),
         AROS_LPA(LONG, max, D1),
         AROS_LPA(LONG *, maxWidth, A3),
         AROS_LPA(LONG *, maxLen, D3),
         AROS_LPA(BOOL, fullChecl, D2),
         LIBBASETYPEPTR, GTLayoutBase, 43, GTLayout
);
AROS_LP1(void, LT_Refresh,
         AROS_LPA(LayoutHandle *, handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 44, GTLayout
);
AROS_LP1(void, LT_CatchUpRefresh,
         AROS_LPA(LayoutHandle *, handle, A0),
         LIBBASETYPEPTR, GTLayoutBase, 45, GTLayout
);
AROS_LP2(APTR, LT_GetWindowUserData,
         AROS_LPA(struct Window *, Window, A0),
         AROS_LPA(APTR, DefaultValue, A1),
         LIBBASETYPEPTR, GTLayoutBase, 46, GTLayout
);

#endif /* CLIB_GTLAYOUT_PROTOS_H */
