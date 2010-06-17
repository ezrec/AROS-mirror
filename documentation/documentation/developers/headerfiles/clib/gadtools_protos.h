#ifndef CLIB_GADTOOLS_PROTOS_H
#define CLIB_GADTOOLS_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/gadtools/gadtools.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <utility/tagitem.h>
#include <libraries/gadtools.h>

__BEGIN_DECLS

APTR           GetVisualInfo(struct Screen *, Tag, ...);
struct Gadget *CreateGadget(ULONG, struct Gadget *, struct NewGadget *, Tag, ...);
void           DrawBevelBox(struct RastPort *, WORD, WORD, WORD, WORD, Tag, ...);
void           GT_SetGadgetAttrs(struct Gadget *, struct Window *, struct Requester *, Tag, ...);
LONG           GT_GetGadgetAttrs(struct Gadget *, struct Window *, struct Requester *, Tag, ...);
struct Menu *  CreateMenus(struct NewMenu *, Tag, ...);
BOOL           LayoutMenus(struct Menu *, APTR, Tag, ...);

__END_DECLS


__BEGIN_DECLS

AROS_LP4(struct Gadget *, CreateGadgetA,
         AROS_LPA(ULONG, kind, D0),
         AROS_LPA(struct Gadget *, previous, A0),
         AROS_LPA(struct NewGadget *, ng, A1),
         AROS_LPA(struct TagItem *, taglist, A2),
         LIBBASETYPEPTR, GadToolsBase, 5, GadTools
);
AROS_LP1(void, FreeGadgets,
         AROS_LPA(struct Gadget *, glist, A0),
         LIBBASETYPEPTR, GadToolsBase, 6, GadTools
);
AROS_LP4(void, GT_SetGadgetAttrsA,
         AROS_LPA(struct Gadget *, gad, A0),
         AROS_LPA(struct Window *, win, A1),
         AROS_LPA(struct Requester *, req, A2),
         AROS_LPA(struct TagItem *, tagList, A3),
         LIBBASETYPEPTR, GadToolsBase, 7, GadTools
);
AROS_LP2(struct Menu *, CreateMenusA,
         AROS_LPA(struct NewMenu *, newmenu, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, GadToolsBase, 8, GadTools
);
AROS_LP1(void, FreeMenus,
         AROS_LPA(struct Menu *, menu, A0),
         LIBBASETYPEPTR, GadToolsBase, 9, GadTools
);
AROS_LP3(BOOL, LayoutMenuItemsA,
         AROS_LPA(struct MenuItem *, menuitem, A0),
         AROS_LPA(APTR, vi, A1),
         AROS_LPA(struct TagItem *, tagList, A2),
         LIBBASETYPEPTR, GadToolsBase, 10, GadTools
);
AROS_LP3(BOOL, LayoutMenusA,
         AROS_LPA(struct Menu *, menu, A0),
         AROS_LPA(APTR, vi, A1),
         AROS_LPA(struct TagItem *, tagList, A2),
         LIBBASETYPEPTR, GadToolsBase, 11, GadTools
);
AROS_LP1(struct IntuiMessage *, GT_GetIMsg,
         AROS_LPA(struct MsgPort *, intuiport, A0),
         LIBBASETYPEPTR, GadToolsBase, 12, GadTools
);
AROS_LP1(void, GT_ReplyIMsg,
         AROS_LPA(struct IntuiMessage *, imsg, A1),
         LIBBASETYPEPTR, GadToolsBase, 13, GadTools
);
AROS_LP2(void, GT_RefreshWindow,
         AROS_LPA(struct Window *, win, A0),
         AROS_LPA(struct Requester *, req, A1),
         LIBBASETYPEPTR, GadToolsBase, 14, GadTools
);
AROS_LP1(void, GT_BeginRefresh,
         AROS_LPA(struct Window *, win, A0),
         LIBBASETYPEPTR, GadToolsBase, 15, GadTools
);
AROS_LP2(void, GT_EndRefresh,
         AROS_LPA(struct Window *, win, A0),
         AROS_LPA(BOOL, complete, D0),
         LIBBASETYPEPTR, GadToolsBase, 16, GadTools
);
AROS_LP1(struct IntuiMessage *, GT_FilterIMsg,
         AROS_LPA(struct IntuiMessage *, imsg, A1),
         LIBBASETYPEPTR, GadToolsBase, 17, GadTools
);
AROS_LP1(struct IntuiMessage *, GT_PostFilterIMsg,
         AROS_LPA(struct IntuiMessage *, modimsg, A1),
         LIBBASETYPEPTR, GadToolsBase, 18, GadTools
);
AROS_LP1(struct Gadget *, CreateContext,
         AROS_LPA(struct Gadget **, glistpointer, A0),
         LIBBASETYPEPTR, GadToolsBase, 19, GadTools
);
AROS_LP6(void, DrawBevelBoxA,
         AROS_LPA(struct RastPort *, rport, A0),
         AROS_LPA(WORD, left, D0),
         AROS_LPA(WORD, top, D1),
         AROS_LPA(WORD, width, D2),
         AROS_LPA(WORD, height, D3),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, GadToolsBase, 20, GadTools
);
AROS_LP2(APTR, GetVisualInfoA,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, GadToolsBase, 21, GadTools
);
AROS_LP1(void, FreeVisualInfo,
         AROS_LPA(APTR, vi, A0),
         LIBBASETYPEPTR, GadToolsBase, 22, GadTools
);
AROS_LP4(LONG, GT_GetGadgetAttrsA,
         AROS_LPA(struct Gadget *, gad, A0),
         AROS_LPA(struct Window *, win, A1),
         AROS_LPA(struct Requester *, req, A2),
         AROS_LPA(struct TagItem *, taglist, A3),
         LIBBASETYPEPTR, GadToolsBase, 29, GadTools
);

__END_DECLS

#endif /* CLIB_GADTOOLS_PROTOS_H */
