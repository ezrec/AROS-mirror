#ifndef CLIB_POPUPMENU_PROTOS_H
#define CLIB_POPUPMENU_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/popupmenu/popupmenu.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <libraries/pm.h>

__BEGIN_DECLS

struct PopupMenu *PM_MakeItem(Tag tag1, ...) __stackparm;
struct PopupMenu *PM_MakeMenu(Tag tag1, ...) __stackparm;
ULONG PM_OpenPopupMenu(struct Window *prevwnd, Tag tag1, ...) __stackparm;
LONG PM_GetItemAttrs(struct PopupMenu *p, Tag tag1, ...) __stackparm;
LONG PM_SetItemAttrs(struct PopupMenu *p, Tag tag1, ...) __stackparm;
APTR PM_FilterIMsg(struct Window *w, struct PopupMenu *pm, struct IntuiMessage *im,  Tag tag1, ...) __stackparm;
struct PM_IDLst *PM_ExLst(ULONG id, ...) __stackparm;
LONG PM_InsertMenuItem(struct PopupMenu *menu, Tag tag1, ...) __stackparm;
LONG PM_LayoutMenu(struct Window *window, struct PopupMenu *pm, Tag tag1, ...) __stackparm;

__END_DECLS


__BEGIN_DECLS

AROS_LP1(struct PopupMenu *, PM_MakeMenuA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 5, Popupmenu
);
AROS_LP1(struct PopupMenu *, PM_MakeItemA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 6, Popupmenu
);
AROS_LP1(void, PM_FreePopupMenu,
         AROS_LPA(struct PopupMenu *, p, A1),
         LIBBASETYPEPTR, PopupMenuBase, 7, Popupmenu
);
AROS_LP2(ULONG, PM_OpenPopupMenuA,
         AROS_LPA(struct Window *, prevwnd, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, PopupMenuBase, 8, Popupmenu
);
AROS_LP1(struct PM_IDLst *, PM_MakeIDListA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 9, Popupmenu
);
AROS_LP2(BOOL, PM_ItemChecked,
         AROS_LPA(struct PopupMenu *, p, A1),
         AROS_LPA(ULONG, id, D1),
         LIBBASETYPEPTR, PopupMenuBase, 10, Popupmenu
);
AROS_LP2(LONG, PM_GetItemAttrsA,
         AROS_LPA(struct PopupMenu *, p, A2),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 11, Popupmenu
);
AROS_LP2(LONG, PM_SetItemAttrsA,
         AROS_LPA(struct PopupMenu *, p, A2),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 12, Popupmenu
);
AROS_LP2(struct PopupMenu *, PM_FindItem,
         AROS_LPA(struct PopupMenu *, menu, A1),
         AROS_LPA(ULONG, id, D1),
         LIBBASETYPEPTR, PopupMenuBase, 13, Popupmenu
);
AROS_LP3(void, PM_AlterState,
         AROS_LPA(struct PopupMenu *, pm, A1),
         AROS_LPA(struct PM_IDLst *, l, A2),
         AROS_LPA(UWORD, action, D1),
         LIBBASETYPEPTR, PopupMenuBase, 14, Popupmenu
);
AROS_LP4(APTR, PM_OBSOLETEFilterIMsgA,
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct PopupMenu *, pm, A2),
         AROS_LPA(struct IntuiMessage *, im, A3),
         AROS_LPA(struct TagItem *, tags, A5),
         LIBBASETYPEPTR, PopupMenuBase, 15, Popupmenu
);
AROS_LP1(struct PM_IDLst *, PM_ExLstA,
         AROS_LPA(ULONG *, id, A1),
         LIBBASETYPEPTR, PopupMenuBase, 16, Popupmenu
);
AROS_LP4(APTR, PM_FilterIMsgA,
         AROS_LPA(struct Window *, w, A0),
         AROS_LPA(struct PopupMenu *, pm, A1),
         AROS_LPA(struct IntuiMessage *, im, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, PopupMenuBase, 17, Popupmenu
);
AROS_LP2(LONG, PM_InsertMenuItemA,
         AROS_LPA(struct PopupMenu *, menu, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, PopupMenuBase, 18, Popupmenu
);
AROS_LP2(struct PopupMenu *, PM_RemoveMenuItem,
         AROS_LPA(struct PopupMenu *, menu, A0),
         AROS_LPA(struct PopupMenu *, item, A1),
         LIBBASETYPEPTR, PopupMenuBase, 19, Popupmenu
);
AROS_LP1(BOOL, PM_AbortHook,
         AROS_LPA(APTR, handle, A0),
         LIBBASETYPEPTR, PopupMenuBase, 20, Popupmenu
);
AROS_LP0(STRPTR, PM_GetVersion,
         LIBBASETYPEPTR, PopupMenuBase, 21, Popupmenu
);
AROS_LP0(void, PM_ReloadPrefs,
         LIBBASETYPEPTR, PopupMenuBase, 22, Popupmenu
);
AROS_LP3(LONG, PM_LayoutMenuA,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct PopupMenu *, pm, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, PopupMenuBase, 23, Popupmenu
);
AROS_LP1(void, PM_FreeIDList,
         AROS_LPA(struct PM_IDLst *, list, A0),
         LIBBASETYPEPTR, PopupMenuBase, 25, Popupmenu
);

__END_DECLS

#endif /* CLIB_POPUPMENU_PROTOS_H */
