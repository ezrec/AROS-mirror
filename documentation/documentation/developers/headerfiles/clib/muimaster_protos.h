#ifndef CLIB_MUIMASTER_PROTOS_H
#define CLIB_MUIMASTER_PROTOS_H

/*
    *** Automatically generated from 'muimaster.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif

#ifndef INTUITION_CLASSUSR_H
#   include <intuition/classusr.h>
#endif

#ifndef UTILITIY_TAGITEM_H
#   include <utility/tagitem.h>
#endif

#ifndef GRAPHICS_REGIONS_H
#   include <graphics/regions.h>
#endif

#ifndef MUIMasterBase
extern struct Library *MUIMasterBase;
#endif

/* By default, disable the variadic stuff for zune, since it's not 
   very backward compatible */
#ifndef MUIMASTER_YES_INLINE_STDARG
#undef  MUIMASTER_NO_INLINE_STDARG
#define MUIMASTER_NO_INLINE_STDARG
#endif

/* Prototypes for stubs in mui.lib */

APTR 	MUI_AllocAslRequestTags(unsigned long reqType, Tag tag1, ...) __stackparm;
BOOL 	MUI_AslRequestTags(APTR requester, Tag tag1, ...) __stackparm;
Object *MUI_MakeObject(LONG type, ...);
Object *MUI_NewObject(char *classname, Tag tag1, ...) __stackparm;
LONG    MUI_Request(APTR app, APTR win, LONG flags, char *title, char *gadgets, 
char *format, APTR param1, ...) __stackparm;

/* Predeclaration of private structures */
struct MUI_RenderInfo;
struct MUI_PenSpec;

AROS_LP2(Object*, MUI_NewObjectA,
         AROS_LPA(ClassID, classid, A0),
         AROS_LPA(struct TagItem*, tags, A1),
         LIBBASETYPEPTR, MUIMasterBase, 5, MUIMaster
);
AROS_LP1(void, MUI_DisposeObject,
         AROS_LPA(Object*, obj, A0),
         LIBBASETYPEPTR, MUIMasterBase, 6, MUIMaster
);
AROS_LP7(LONG, MUI_RequestA,
         AROS_LPA(APTR, app, D0),
         AROS_LPA(APTR, win, D1),
         AROS_LPA(LONGBITS, flags, D2),
         AROS_LPA(CONST_STRPTR, title, A0),
         AROS_LPA(CONST_STRPTR, gadgets, A1),
         AROS_LPA(CONST_STRPTR, format, A2),
         AROS_LPA(APTR, params, A3),
         LIBBASETYPEPTR, MUIMasterBase, 7, MUIMaster
);
AROS_LP2(APTR, MUI_AllocAslRequest,
         AROS_LPA(unsigned long, reqType, D0),
         AROS_LPA(struct TagItem*, tagList, A0),
         LIBBASETYPEPTR, MUIMasterBase, 8, MUIMaster
);
AROS_LP2(BOOL, MUI_AslRequest,
         AROS_LPA(APTR, requester, A0),
         AROS_LPA(struct TagItem*, tagList, A1),
         LIBBASETYPEPTR, MUIMasterBase, 9, MUIMaster
);
AROS_LP1(void, MUI_FreeAslRequest,
         AROS_LPA(APTR, requester, A0),
         LIBBASETYPEPTR, MUIMasterBase, 10, MUIMaster
);
AROS_LP0(LONG, MUI_Error,
         LIBBASETYPEPTR, MUIMasterBase, 11, MUIMaster
);
AROS_LP1(LONG, MUI_SetError,
         AROS_LPA(LONG, num, D0),
         LIBBASETYPEPTR, MUIMasterBase, 12, MUIMaster
);
AROS_LP1(struct IClass*, MUI_GetClass,
         AROS_LPA(ClassID, classid, A0),
         LIBBASETYPEPTR, MUIMasterBase, 13, MUIMaster
);
AROS_LP1(void, MUI_FreeClass,
         AROS_LPA(Class*, cl, A0),
         LIBBASETYPEPTR, MUIMasterBase, 14, MUIMaster
);
AROS_LP2(void, MUI_RequestIDCMP,
         AROS_LPA(Object*, obj, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 15, MUIMaster
);
AROS_LP2(void, MUI_RejectIDCMP,
         AROS_LPA(Object*, obj, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 16, MUIMaster
);
AROS_LP2(void, MUI_Redraw,
         AROS_LPA(Object*, obj, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 17, MUIMaster
);
AROS_LP5(struct MUI_CustomClass*, MUI_CreateCustomClass,
         AROS_LPA(struct Library*, base, A0),
         AROS_LPA(ClassID, supername, A1),
         AROS_LPA(struct MUI_CustomClass*, supermcc, A2),
         AROS_LPA(ULONG, datasize, D0),
         AROS_LPA(APTR, dispatcher, A3),
         LIBBASETYPEPTR, MUIMasterBase, 18, MUIMaster
);
AROS_LP1(BOOL, MUI_DeleteCustomClass,
         AROS_LPA(struct MUI_CustomClass*, mcc, A0),
         LIBBASETYPEPTR, MUIMasterBase, 19, MUIMaster
);
AROS_LP2(Object*, MUI_MakeObjectA,
         AROS_LPA(LONG, type, D0),
         AROS_LPA(IPTR*, params, A0),
         LIBBASETYPEPTR, MUIMasterBase, 20, MUIMaster
);
AROS_LP6(BOOL, MUI_Layout,
         AROS_LPA(Object*, obj, A0),
         AROS_LPA(LONG, left, D0),
         AROS_LPA(LONG, top, D1),
         AROS_LPA(LONG, width, D2),
         AROS_LPA(LONG, height, D3),
         AROS_LPA(ULONG, flags, D4),
         LIBBASETYPEPTR, MUIMasterBase, 21, MUIMaster
);
AROS_LP3(LONG, MUI_ObtainPen,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(struct MUI_PenSpec*, spec, A1),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 22, MUIMaster
);
AROS_LP2(void, MUI_ReleasePen,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(LONG, pen, D0),
         LIBBASETYPEPTR, MUIMasterBase, 23, MUIMaster
);
AROS_LP5(APTR, MUI_AddClipping,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(WORD, left, D0),
         AROS_LPA(WORD, top, D1),
         AROS_LPA(WORD, width, D2),
         AROS_LPA(WORD, height, D3),
         LIBBASETYPEPTR, MUIMasterBase, 24, MUIMaster
);
AROS_LP2(void, MUI_RemoveClipping,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(APTR, handle, A1),
         LIBBASETYPEPTR, MUIMasterBase, 25, MUIMaster
);
AROS_LP2(APTR, MUI_AddClipRegion,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(struct Region*, r, A1),
         LIBBASETYPEPTR, MUIMasterBase, 26, MUIMaster
);
AROS_LP2(void, MUI_RemoveClipRegion,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(APTR, handle, A1),
         LIBBASETYPEPTR, MUIMasterBase, 27, MUIMaster
);
AROS_LP2(BOOL, MUI_BeginRefresh,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 28, MUIMaster
);
AROS_LP2(void, MUI_EndRefresh,
         AROS_LPA(struct MUI_RenderInfo*, mri, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, MUIMasterBase, 29, MUIMaster
);

#endif /* CLIB_MUIMASTER_PROTOS_H */
