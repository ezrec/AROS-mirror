#ifndef CLIB_INTUITION_PROTOS_H
#define CLIB_INTUITION_PROTOS_H

/*
    *** Automatically generated from 'intuition.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#ifndef INTUITION_CDEF_H
#define INTUITION_CDEF_H
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <intuition/classes.h>
#include <intuition/windecorclass.h>
#include <intuition/scrdecorclass.h>
#include <intuition/menudecorclass.h>
#include <utility/hooks.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

/* Prototypes for stubs in amiga.lib */
IPTR DoGadgetMethod (struct Gadget * gad, struct Window * win,  
                    struct Requester * req, ULONG MethodID, ...);
IPTR SetGadgetAttrs (struct Gadget * gad, struct Window * win,
                    struct Requester * req, Tag tag1, ...);

#if !defined(DEFINES_INTUITION_PROTOS_H) /* Prevent name clashes */
IPTR SetAttrs (APTR obj, Tag tag1, ...) __stackparm;
IPTR SetSuperAttrs (Class * cl, Object * obj, Tag tag1, ...) __stackparm;
APTR NewObject (Class * classPtr, UBYTE * classID, Tag tag1, ...) __stackparm;
#ifndef USE_BOOPSI_STUBS
IPTR DoMethodA (Object * obj, Msg message);
IPTR DoMethod (Object * obj, STACKULONG MethodID, ...) __stackparm;
IPTR DoSuperMethodA (Class * cl, Object * obj, Msg message);
IPTR DoSuperMethod (Class * cl, Object * obj, STACKULONG MethodID, ...) __stackparm;
IPTR CoerceMethodA (Class * cl, Object * obj, Msg message);
IPTR CoerceMethod (Class * cl, Object * obj, STACKULONG MethodID, ...) __stackparm;
#endif /* !USE_BOOPSI_STUBS */
#endif

IPTR DoSuperNewTagList(Class *CLASS, Object *object, struct GadgetInfo *gadgetInfo, struct TagItem *tags);
IPTR DoSuperNewTags(Class *CLASS, Object *object, struct GadgetInfo *gadgetInfo, Tag tag1, ...) __stackparm;

#if !defined(INTUITION_NO_INLINE_STDARG) && !defined(NO_INLINE_STDARG)
#   ifndef CLIB_INTUITION_PROTOS_H /* prevent name clashes */
#       define SetSuperAttrsA(class, object, attrs)          \
        ({                                                   \
            struct opSet __ops;                              \
                                                             \
            __ops.MethodID     = OM_SET;                     \
            __ops.ops_AttrList = (attrs);                    \
            __ops.ops_GInfo    = NULL;                       \
                                                             \
            DoSuperMethodA((class), (object), (Msg) &__ops.MethodID); \
        })
#       define SetSuperAttrs(class, object, args...)                      \
        ({                                                                \
            IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(args) };         \
            SetSuperAttrsA((class), (object), (struct TagItem *) __args); \
        })
    
#       ifndef USE_BOOPSI_STUBS /* prevent name clashes */
#           define DoMethodA(object, message)                                 \
            ({                                                                \
                (object) != NULL ?                                            \
                ({                                                            \
                    CALLHOOKPKT                                               \
                    (                                                         \
                        (struct Hook *) OCLASS((object)), (object), (message) \
                    );                                                        \
                })                                                            \
                :                                                             \
                    0                                                         \
                ;                                                             \
            })
#           define DoMethod(object, methodid, args...)                        \
            ({                                                                \
                IPTR __args[] = {methodid, AROS_PP_VARIADIC_CAST2IPTR(args)}; \
                DoMethodA((object), __args);                                  \
            })
            
#           define DoSuperMethodA(class, object, message)                     \
            ({                                                                \
                ((class) != NULL && (object) != NULL) ?                       \
                    CALLHOOKPKT                                               \
                    (                                                         \
                        (struct Hook *) ((Class *) (class))->cl_Super,        \
                        (object), (message)                                   \
                    )                                                         \
                :                                                             \
                    0                                                         \
                ;                                                             \
            })
#           define DoSuperMethod(class, object, methodid, args...)            \
            ({                                                                \
                IPTR __args[] = {methodid, AROS_PP_VARIADIC_CAST2IPTR(args)}; \
                DoSuperMethodA((class), (object), __args);                    \
            })
            
#           define CoerceMethodA(class, object, message)                      \
            ({                                                                \
                ((class) != NULL && (object) != NULL) ?                       \
                    CALLHOOKPKT((struct Hook *) (class), (object), (message)) \
                :                                                             \
                    0                                                         \
                ;                                                             \
            })
#           define CoerceMethod(class, object, methodid, args...)             \
            ({                                                                \
                IPTR __args[] = {methodid, AROS_PP_VARIADIC_CAST2IPTR(args)}; \
                CoerceMethodA((class), (object), __args);                     \
            })
#       endif /* !USE_BOOPSI_STUBS */
#   endif /* CLIB_INTUITION_PROTOS_H */   

#   define DoSuperNewTagList(class, object, gadgetinfo, tags)              \
    ({                                                                     \
        struct opSet __ops;                                                \
                                                                           \
        __ops.MethodID     = OM_NEW;                                       \
        __ops.ops_AttrList = (tags);                                       \
        __ops.ops_GInfo    = (gadgetinfo);                                 \
                                                                           \
        (class) != NULL && (object) != NULL ?                              \
            DoSuperMethodA((class), (object), (Msg)&__ops.MethodID)                 \
        :                                                                  \
            0                                                              \
        ;                                                                  \
    })
#   define DoSuperNewTags(class, object, gadgetinfo, args...)              \
    ({                                                                     \
        IPTR __args[] = {AROS_PP_VARIADIC_CAST2IPTR(args)};                                            \
        DoSuperNewTagList                                                  \
        (                                                                  \
            (class), (object), (gadgetinfo), (struct TagItem *) __args     \
        );                                                                 \
    })
#endif /* !INTUITION_NO_INLINE_STDARG && !NO_INLINE_STDARG */

#ifndef CLIB_BOOPSI_PROTOS_H /* Prevent name clashes */
IPTR CallHookA (struct Hook * hook, APTR obj, APTR param);
IPTR CallHook (struct Hook * hook, APTR obj, ...);
#endif

struct Window * OpenWindowTags (struct NewWindow * newWindow, Tag tag1, ...);
struct Screen * OpenScreenTags (struct NewScreen * newScreen, Tag tag1, ...);

LONG EasyRequest (struct Window * window, struct EasyStruct * easyStruct, ULONG * idcmpPtr, ...);

struct ICData;

#endif
AROS_LP3(UWORD, AddGadget,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Gadget *, gadget, A1),
         AROS_LPA(ULONG, position, D0),
         LIBBASETYPEPTR, IntuitionBase, 7, Intuition
);
AROS_LP1(BOOL, ClearDMRequest,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 8, Intuition
);
AROS_LP1(void, ClearMenuStrip,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 9, Intuition
);
AROS_LP1(void, ClearPointer,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 10, Intuition
);
AROS_LP1(BOOL, CloseScreen,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 11, Intuition
);
AROS_LP1(void, CloseWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 12, Intuition
);
AROS_LP0(LONG, CloseWorkBench,
         LIBBASETYPEPTR, IntuitionBase, 13, Intuition
);
AROS_LP2(void, CurrentTime,
         AROS_LPA(ULONG *, seconds, A0),
         AROS_LPA(ULONG *, micros, A1),
         LIBBASETYPEPTR, IntuitionBase, 14, Intuition
);
AROS_LP3(BOOL, DisplayAlert,
         AROS_LPA(ULONG, alertnumber, D0),
         AROS_LPA(UBYTE *, string, A0),
         AROS_LPA(UWORD, height, D1),
         LIBBASETYPEPTR, IntuitionBase, 15, Intuition
);
AROS_LP1(void, DisplayBeep,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 16, Intuition
);
AROS_LP4(BOOL, DoubleClick,
         AROS_LPA(ULONG, sSeconds, D0),
         AROS_LPA(ULONG, sMicros, D1),
         AROS_LPA(ULONG, cSeconds, D2),
         AROS_LPA(ULONG, cMicros, D3),
         LIBBASETYPEPTR, IntuitionBase, 17, Intuition
);
AROS_LP4(void, DrawBorder,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct Border *, border, A1),
         AROS_LPA(LONG, leftOffset, D0),
         AROS_LPA(LONG, topOffset, D1),
         LIBBASETYPEPTR, IntuitionBase, 18, Intuition
);
AROS_LP4(void, DrawImage,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct Image *, image, A1),
         AROS_LPA(LONG, leftOffset, D0),
         AROS_LPA(LONG, topOffset, D1),
         LIBBASETYPEPTR, IntuitionBase, 19, Intuition
);
AROS_LP2(void, EndRequest,
         AROS_LPA(struct Requester *, requester, A0),
         AROS_LPA(struct Window *, window, A1),
         LIBBASETYPEPTR, IntuitionBase, 20, Intuition
);
AROS_LP2(struct Preferences *, GetDefPrefs,
         AROS_LPA(struct Preferences *, prefbuffer, A0),
         AROS_LPA(WORD, size, D0),
         LIBBASETYPEPTR, IntuitionBase, 21, Intuition
);
AROS_LP2(struct Preferences *, GetPrefs,
         AROS_LPA(struct Preferences *, prefbuffer, A0),
         AROS_LPA(WORD, size, D0),
         LIBBASETYPEPTR, IntuitionBase, 22, Intuition
);
AROS_LP1(void, InitRequester,
         AROS_LPA(struct Requester *, requester, A0),
         LIBBASETYPEPTR, IntuitionBase, 23, Intuition
);
AROS_LP2(struct MenuItem *, ItemAddress,
         AROS_LPA(struct Menu *, menustrip, A0),
         AROS_LPA(UWORD, menunumber, D0),
         LIBBASETYPEPTR, IntuitionBase, 24, Intuition
);
AROS_LP2(BOOL, ModifyIDCMP,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, IntuitionBase, 25, Intuition
);
AROS_LP8(void, ModifyProp,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         AROS_LPA(ULONG, flags, D0),
         AROS_LPA(ULONG, horizPot, D1),
         AROS_LPA(ULONG, vertPot, D2),
         AROS_LPA(ULONG, horizBody, D3),
         AROS_LPA(ULONG, vertBody, D4),
         LIBBASETYPEPTR, IntuitionBase, 26, Intuition
);
AROS_LP3(void, MoveScreen,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(LONG, dx, D0),
         AROS_LPA(LONG, dy, D1),
         LIBBASETYPEPTR, IntuitionBase, 27, Intuition
);
AROS_LP3(void, MoveWindow,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(LONG, dx, D0),
         AROS_LPA(LONG, dy, D1),
         LIBBASETYPEPTR, IntuitionBase, 28, Intuition
);
AROS_LP3(void, OffGadget,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         LIBBASETYPEPTR, IntuitionBase, 29, Intuition
);
AROS_LP2(void, OffMenu,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(UWORD, menunumber, D0),
         LIBBASETYPEPTR, IntuitionBase, 30, Intuition
);
AROS_LP3(void, OnGadget,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         LIBBASETYPEPTR, IntuitionBase, 31, Intuition
);
AROS_LP2(void, OnMenu,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(UWORD, menunumber, D0),
         LIBBASETYPEPTR, IntuitionBase, 32, Intuition
);
AROS_LP1(struct Screen *, OpenScreen,
         AROS_LPA(struct NewScreen *, newScreen, A0),
         LIBBASETYPEPTR, IntuitionBase, 33, Intuition
);
AROS_LP1(struct Window *, OpenWindow,
         AROS_LPA(struct NewWindow *, newWindow, A0),
         LIBBASETYPEPTR, IntuitionBase, 34, Intuition
);
AROS_LP0(IPTR, OpenWorkBench,
         LIBBASETYPEPTR, IntuitionBase, 35, Intuition
);
AROS_LP4(void, PrintIText,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct IntuiText *, iText, A1),
         AROS_LPA(LONG, leftOffset, D0),
         AROS_LPA(LONG, topOffset, D1),
         LIBBASETYPEPTR, IntuitionBase, 36, Intuition
);
AROS_LP3(void, RefreshGadgets,
         AROS_LPA(struct Gadget *, gadgets, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         LIBBASETYPEPTR, IntuitionBase, 37, Intuition
);
AROS_LP2(UWORD, RemoveGadget,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Gadget *, gadget, A1),
         LIBBASETYPEPTR, IntuitionBase, 38, Intuition
);
AROS_LP2(void, ReportMouse,
         AROS_LPA(LONG, flag, D0),
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 39, Intuition
);
AROS_LP2(BOOL, Request,
         AROS_LPA(struct Requester *, requester, A0),
         AROS_LPA(struct Window *, window, A1),
         LIBBASETYPEPTR, IntuitionBase, 40, Intuition
);
AROS_LP1(void, ScreenToBack,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 41, Intuition
);
AROS_LP1(void, ScreenToFront,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 42, Intuition
);
AROS_LP2(BOOL, SetDMRequest,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Requester *, dmrequest, A1),
         LIBBASETYPEPTR, IntuitionBase, 43, Intuition
);
AROS_LP2(BOOL, SetMenuStrip,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Menu *, menu, A1),
         LIBBASETYPEPTR, IntuitionBase, 44, Intuition
);
AROS_LP6(void, SetPointer,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(UWORD *, pointer, A1),
         AROS_LPA(LONG, height, D0),
         AROS_LPA(LONG, width, D1),
         AROS_LPA(LONG, xOffset, D2),
         AROS_LPA(LONG, yOffset, D3),
         LIBBASETYPEPTR, IntuitionBase, 45, Intuition
);
AROS_LP3(void, SetWindowTitles,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(CONST_STRPTR, windowTitle, A1),
         AROS_LPA(CONST_STRPTR, screenTitle, A2),
         LIBBASETYPEPTR, IntuitionBase, 46, Intuition
);
AROS_LP2(void, ShowTitle,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(BOOL, ShowIt, D0),
         LIBBASETYPEPTR, IntuitionBase, 47, Intuition
);
AROS_LP3(void, SizeWindow,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(LONG, dx, D0),
         AROS_LPA(LONG, dy, D1),
         LIBBASETYPEPTR, IntuitionBase, 48, Intuition
);
AROS_LP0(struct View *, ViewAddress,
         LIBBASETYPEPTR, IntuitionBase, 49, Intuition
);
AROS_LP1(struct ViewPort *, ViewPortAddress,
         AROS_LPA(struct Window *, Window, A0),
         LIBBASETYPEPTR, IntuitionBase, 50, Intuition
);
AROS_LP1(void, WindowToBack,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 51, Intuition
);
AROS_LP1(void, WindowToFront,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 52, Intuition
);
AROS_LP5(BOOL, WindowLimits,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(WORD, MinWidth, D0),
         AROS_LPA(WORD, MinHeight, D1),
         AROS_LPA(UWORD, MaxWidth, D2),
         AROS_LPA(UWORD, MaxHeight, D3),
         LIBBASETYPEPTR, IntuitionBase, 53, Intuition
);
AROS_LP3(struct Preferences *, SetPrefs,
         AROS_LPA(struct Preferences *, prefbuffer, A0),
         AROS_LPA(LONG, size, D0),
         AROS_LPA(BOOL, inform, D1),
         LIBBASETYPEPTR, IntuitionBase, 54, Intuition
);
AROS_LP1(LONG, IntuiTextLength,
         AROS_LPA(struct IntuiText *, iText, A0),
         LIBBASETYPEPTR, IntuitionBase, 55, Intuition
);
AROS_LP0(BOOL, WBenchToBack,
         LIBBASETYPEPTR, IntuitionBase, 56, Intuition
);
AROS_LP0(BOOL, WBenchToFront,
         LIBBASETYPEPTR, IntuitionBase, 57, Intuition
);
AROS_LP8(BOOL, AutoRequest,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct IntuiText *, body, A1),
         AROS_LPA(struct IntuiText *, posText, A2),
         AROS_LPA(struct IntuiText *, negText, A3),
         AROS_LPA(ULONG, pFlag, D0),
         AROS_LPA(ULONG, nFlag, D1),
         AROS_LPA(ULONG, width, D2),
         AROS_LPA(ULONG, height, D3),
         LIBBASETYPEPTR, IntuitionBase, 58, Intuition
);
AROS_LP1(void, BeginRefresh,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 59, Intuition
);
AROS_LP7(struct Window *, BuildSysRequest,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct IntuiText *, bodytext, A1),
         AROS_LPA(struct IntuiText *, postext, A2),
         AROS_LPA(struct IntuiText *, negtext, A3),
         AROS_LPA(ULONG, IDCMPFlags, D0),
         AROS_LPA(WORD, width, D2),
         AROS_LPA(WORD, height, D3),
         LIBBASETYPEPTR, IntuitionBase, 60, Intuition
);
AROS_LP2(void, EndRefresh,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(BOOL, complete, D0),
         LIBBASETYPEPTR, IntuitionBase, 61, Intuition
);
AROS_LP1(void, FreeSysRequest,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 62, Intuition
);
AROS_LP1(LONG, MakeScreen,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 63, Intuition
);
AROS_LP0(LONG, RemakeDisplay,
         LIBBASETYPEPTR, IntuitionBase, 64, Intuition
);
AROS_LP0(LONG, RethinkDisplay,
         LIBBASETYPEPTR, IntuitionBase, 65, Intuition
);
AROS_LP3(APTR, AllocRemember,
         AROS_LPA(struct Remember **, rememberKey, A0),
         AROS_LPA(ULONG, size, D0),
         AROS_LPA(ULONG, flags, D1),
         LIBBASETYPEPTR, IntuitionBase, 66, Intuition
);
AROS_LP1(void, AlohaWorkbench,
         AROS_LPA(struct MsgPort *, wbmsgport, A0),
         LIBBASETYPEPTR, IntuitionBase, 67, Intuition
);
AROS_LP2(void, FreeRemember,
         AROS_LPA(struct Remember **, rememberKey, A0),
         AROS_LPA(LONG, reallyForget, D0),
         LIBBASETYPEPTR, IntuitionBase, 68, Intuition
);
AROS_LP1(ULONG, LockIBase,
         AROS_LPA(ULONG, What, D0),
         LIBBASETYPEPTR, IntuitionBase, 69, Intuition
);
AROS_LP1(void, UnlockIBase,
         AROS_LPA(ULONG, ibLock, A0),
         LIBBASETYPEPTR, IntuitionBase, 70, Intuition
);
AROS_LP4(LONG, GetScreenData,
         AROS_LPA(APTR, buffer, A0),
         AROS_LPA(ULONG, size, D0),
         AROS_LPA(ULONG, type, D1),
         AROS_LPA(struct Screen *, screen, A1),
         LIBBASETYPEPTR, IntuitionBase, 71, Intuition
);
AROS_LP4(void, RefreshGList,
         AROS_LPA(struct Gadget *, gadgets, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         AROS_LPA(LONG, numGad, D0),
         LIBBASETYPEPTR, IntuitionBase, 72, Intuition
);
AROS_LP5(UWORD, AddGList,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Gadget *, gadget, A1),
         AROS_LPA(ULONG, position, D0),
         AROS_LPA(LONG, numGad, D1),
         AROS_LPA(struct Requester *, requester, A2),
         LIBBASETYPEPTR, IntuitionBase, 73, Intuition
);
AROS_LP3(UWORD, RemoveGList,
         AROS_LPA(struct Window *, remPtr, A0),
         AROS_LPA(struct Gadget *, gadget, A1),
         AROS_LPA(LONG, numGad, D0),
         LIBBASETYPEPTR, IntuitionBase, 74, Intuition
);
AROS_LP1(void, ActivateWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 75, Intuition
);
AROS_LP1(void, RefreshWindowFrame,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 76, Intuition
);
AROS_LP3(BOOL, ActivateGadget,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         LIBBASETYPEPTR, IntuitionBase, 77, Intuition
);
AROS_LP9(void, NewModifyProp,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         AROS_LPA(ULONG, flags, D0),
         AROS_LPA(ULONG, horizPot, D1),
         AROS_LPA(ULONG, vertPot, D2),
         AROS_LPA(ULONG, horizBody, D3),
         AROS_LPA(ULONG, vertBody, D4),
         AROS_LPA(LONG, numGad, D5),
         LIBBASETYPEPTR, IntuitionBase, 78, Intuition
);
AROS_LP3(LONG, QueryOverscan,
         AROS_LPA(ULONG, displayid, A0),
         AROS_LPA(struct Rectangle *, rect, A1),
         AROS_LPA(WORD, oscantype, D0),
         LIBBASETYPEPTR, IntuitionBase, 79, Intuition
);
AROS_LP2(void, MoveWindowInFrontOf,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Window *, behindwindow, A1),
         LIBBASETYPEPTR, IntuitionBase, 80, Intuition
);
AROS_LP5(void, ChangeWindowBox,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(LONG, left, D0),
         AROS_LPA(LONG, top, D1),
         AROS_LPA(LONG, width, D2),
         AROS_LPA(LONG, height, D3),
         LIBBASETYPEPTR, IntuitionBase, 81, Intuition
);
AROS_LP1(struct Hook *, SetEditHook,
         AROS_LPA(struct Hook *, hook, A0),
         LIBBASETYPEPTR, IntuitionBase, 82, Intuition
);
AROS_LP2(LONG, SetMouseQueue,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(UWORD, queuelength, D0),
         LIBBASETYPEPTR, IntuitionBase, 83, Intuition
);
AROS_LP1(void, ZipWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 84, Intuition
);
AROS_LP1(struct Screen *, LockPubScreen,
         AROS_LPA(CONST_STRPTR, name, A0),
         LIBBASETYPEPTR, IntuitionBase, 85, Intuition
);
AROS_LP2(void, UnlockPubScreen,
         AROS_LPA(UBYTE *, name, A0),
         AROS_LPA(struct Screen *, screen, A1),
         LIBBASETYPEPTR, IntuitionBase, 86, Intuition
);
AROS_LP0(struct List *, LockPubScreenList,
         LIBBASETYPEPTR, IntuitionBase, 87, Intuition
);
AROS_LP0(void, UnlockPubScreenList,
         LIBBASETYPEPTR, IntuitionBase, 88, Intuition
);
AROS_LP2(UBYTE *, NextPubScreen,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(UBYTE *, namebuff, A1),
         LIBBASETYPEPTR, IntuitionBase, 89, Intuition
);
AROS_LP1(void, SetDefaultPubScreen,
         AROS_LPA(UBYTE *, name, A0),
         LIBBASETYPEPTR, IntuitionBase, 90, Intuition
);
AROS_LP1(UWORD, SetPubScreenModes,
         AROS_LPA(UWORD, modes, D0),
         LIBBASETYPEPTR, IntuitionBase, 91, Intuition
);
AROS_LP2(UWORD, PubScreenStatus,
         AROS_LPA(struct Screen *, Scr, A0),
         AROS_LPA(UWORD, StatusFlags, D0),
         LIBBASETYPEPTR, IntuitionBase, 92, Intuition
);
AROS_LP1(struct RastPort *, ObtainGIRPort,
         AROS_LPA(struct GadgetInfo *, gInfo, A0),
         LIBBASETYPEPTR, IntuitionBase, 93, Intuition
);
AROS_LP1(void, ReleaseGIRPort,
         AROS_LPA(struct RastPort *, rp, A0),
         LIBBASETYPEPTR, IntuitionBase, 94, Intuition
);
AROS_LP3(void, GadgetMouse,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct GadgetInfo *, ginfo, A1),
         AROS_LPA(WORD *, mousepoint, A2),
         LIBBASETYPEPTR, IntuitionBase, 95, Intuition
);
AROS_LP3(ULONG, SetIPrefs,
         AROS_LPA(APTR, data, A0),
         AROS_LPA(ULONG, length, D0),
         AROS_LPA(ULONG, type, D1),
         LIBBASETYPEPTR, IntuitionBase, 96, Intuition
);
AROS_LP1(struct Screen *, GetDefaultPubScreen,
         AROS_LPA(UBYTE *, nameBuffer, A0),
         LIBBASETYPEPTR, IntuitionBase, 97, Intuition
);
AROS_LP4(LONG, EasyRequestArgs,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct EasyStruct *, easyStruct, A1),
         AROS_LPA(ULONG *, IDCMP_ptr, A2),
         AROS_LPA(APTR, argList, A3),
         LIBBASETYPEPTR, IntuitionBase, 98, Intuition
);
AROS_LP4(struct Window *, BuildEasyRequestArgs,
         AROS_LPA(struct Window *, RefWindow, A0),
         AROS_LPA(struct EasyStruct *, easyStruct, A1),
         AROS_LPA(ULONG, IDCMP, D0),
         AROS_LPA(APTR, Args, A3),
         LIBBASETYPEPTR, IntuitionBase, 99, Intuition
);
AROS_LP3(LONG, SysReqHandler,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(ULONG *, IDCMPFlagsPtr, A1),
         AROS_LPA(BOOL, WaitInput, D0),
         LIBBASETYPEPTR, IntuitionBase, 100, Intuition
);
AROS_LP2(struct Window *, OpenWindowTagList,
         AROS_LPA(struct NewWindow *, newWindow, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, IntuitionBase, 101, Intuition
);
AROS_LP2(struct Screen *, OpenScreenTagList,
         AROS_LPA(struct NewScreen *, newScreen, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, IntuitionBase, 102, Intuition
);
AROS_LP6(void, DrawImageState,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct Image *, image, A1),
         AROS_LPA(LONG, leftOffset, D0),
         AROS_LPA(LONG, topOffset, D1),
         AROS_LPA(ULONG, state, D2),
         AROS_LPA(struct DrawInfo *, drawInfo, A2),
         LIBBASETYPEPTR, IntuitionBase, 103, Intuition
);
AROS_LP2(BOOL, PointInImage,
         AROS_LPA(ULONG, point, D0),
         AROS_LPA(struct Image *, image, A0),
         LIBBASETYPEPTR, IntuitionBase, 104, Intuition
);
AROS_LP4(void, EraseImage,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct Image *, image, A1),
         AROS_LPA(LONG, leftOffset, D0),
         AROS_LPA(LONG, topOffset, D1),
         LIBBASETYPEPTR, IntuitionBase, 105, Intuition
);
AROS_LP3(APTR, NewObjectA,
         AROS_LPA(struct IClass *, classPtr, A0),
         AROS_LPA(UBYTE *, classID, A1),
         AROS_LPA(struct TagItem *, tagList, A2),
         LIBBASETYPEPTR, IntuitionBase, 106, Intuition
);
AROS_LP1(void, DisposeObject,
         AROS_LPA(APTR, object, A0),
         LIBBASETYPEPTR, IntuitionBase, 107, Intuition
);
AROS_LP2(IPTR, SetAttrsA,
         AROS_LPA(APTR, object, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, IntuitionBase, 108, Intuition
);
AROS_LP3(ULONG, GetAttr,
         AROS_LPA(ULONG, attrID, D0),
         AROS_LPA(Object *, object, A0),
         AROS_LPA(IPTR *, storagePtr, A1),
         LIBBASETYPEPTR, IntuitionBase, 109, Intuition
);
AROS_LP4(IPTR, SetGadgetAttrsA,
         AROS_LPA(struct Gadget *, gadget, A0),
         AROS_LPA(struct Window *, window, A1),
         AROS_LPA(struct Requester *, requester, A2),
         AROS_LPA(struct TagItem *, tagList, A3),
         LIBBASETYPEPTR, IntuitionBase, 110, Intuition
);
AROS_LP1(APTR, NextObject,
         AROS_LPA(APTR, objectPtrPtr, A0),
         LIBBASETYPEPTR, IntuitionBase, 111, Intuition
);
AROS_LP1(struct IClass *, FindClass,
         AROS_LPA(ClassID, classID, A0),
         LIBBASETYPEPTR, IntuitionBase, 112, Intuition
);
AROS_LP5(struct IClass *, MakeClass,
         AROS_LPA(ClassID, classID, A0),
         AROS_LPA(ClassID, superClassID, A1),
         AROS_LPA(struct IClass *, superClassPtr, A2),
         AROS_LPA(ULONG, instanceSize, D0),
         AROS_LPA(ULONG, flags, D1),
         LIBBASETYPEPTR, IntuitionBase, 113, Intuition
);
AROS_LP1(void, AddClass,
         AROS_LPA(struct IClass *, classPtr, A0),
         LIBBASETYPEPTR, IntuitionBase, 114, Intuition
);
AROS_LP1(struct DrawInfo *, GetScreenDrawInfo,
         AROS_LPA(struct Screen *, screen, A0),
         LIBBASETYPEPTR, IntuitionBase, 115, Intuition
);
AROS_LP2(void, FreeScreenDrawInfo,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct DrawInfo *, drawInfo, A1),
         LIBBASETYPEPTR, IntuitionBase, 116, Intuition
);
AROS_LP2(BOOL, ResetMenuStrip,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Menu *, menu, A1),
         LIBBASETYPEPTR, IntuitionBase, 117, Intuition
);
AROS_LP1(void, RemoveClass,
         AROS_LPA(struct IClass *, classPtr, A0),
         LIBBASETYPEPTR, IntuitionBase, 118, Intuition
);
AROS_LP1(BOOL, FreeClass,
         AROS_LPA(struct IClass *, iclass, A0),
         LIBBASETYPEPTR, IntuitionBase, 119, Intuition
);
AROS_LP3(struct ScreenBuffer *, AllocScreenBuffer,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct BitMap *, bitmap, A1),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, IntuitionBase, 128, Intuition
);
AROS_LP2(void, FreeScreenBuffer,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct ScreenBuffer *, screenbuffer, A1),
         LIBBASETYPEPTR, IntuitionBase, 129, Intuition
);
AROS_LP2(ULONG, ChangeScreenBuffer,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct ScreenBuffer *, screenbuffer, A1),
         LIBBASETYPEPTR, IntuitionBase, 130, Intuition
);
AROS_LP3(void, ScreenDepth,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(ULONG, flags, D0),
         AROS_LPA(APTR, reserved, A1),
         LIBBASETYPEPTR, IntuitionBase, 131, Intuition
);
AROS_LP6(void, ScreenPosition,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(ULONG, flags, D0),
         AROS_LPA(LONG, x1, D1),
         AROS_LPA(LONG, y1, D2),
         AROS_LPA(LONG, x2, D3),
         AROS_LPA(LONG, y2, D4),
         LIBBASETYPEPTR, IntuitionBase, 132, Intuition
);
AROS_LP7(void, ScrollWindowRaster,
         AROS_LPA(struct Window *, win, A1),
         AROS_LPA(WORD, dx, D0),
         AROS_LPA(WORD, dy, D1),
         AROS_LPA(WORD, xmin, D2),
         AROS_LPA(WORD, ymin, D3),
         AROS_LPA(WORD, xmax, D4),
         AROS_LPA(WORD, ymax, D5),
         LIBBASETYPEPTR, IntuitionBase, 133, Intuition
);
AROS_LP2(void, LendMenus,
         AROS_LPA(struct Window *, fromwindow, A0),
         AROS_LPA(struct Window *, towindow, A1),
         LIBBASETYPEPTR, IntuitionBase, 134, Intuition
);
AROS_LP4(IPTR, DoGadgetMethodA,
         AROS_LPA(struct Gadget *, gad, A0),
         AROS_LPA(struct Window *, win, A1),
         AROS_LPA(struct Requester *, req, A2),
         AROS_LPA(Msg, msg, A3),
         LIBBASETYPEPTR, IntuitionBase, 135, Intuition
);
AROS_LP2(void, SetWindowPointerA,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, IntuitionBase, 136, Intuition
);
AROS_LP4(BOOL, TimedDisplayAlert,
         AROS_LPA(ULONG, alertnumber, D0),
         AROS_LPA(UBYTE *, string, A0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG, time, A1),
         LIBBASETYPEPTR, IntuitionBase, 137, Intuition
);
AROS_LP2(void, HelpControl,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, IntuitionBase, 138, Intuition
);
AROS_LP1(LONG, IsWindowVisible,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 139, Intuition
);
AROS_LP1(void, ShowWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 140, Intuition
);
AROS_LP1(void, HideWindow,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 141, Intuition
);
AROS_LP3(struct Region *, ChangeWindowShape,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct Region *, newshape, A1),
         AROS_LPA(struct Hook *, callback, A2),
         LIBBASETYPEPTR, IntuitionBase, 143, Intuition
);
AROS_LP1(void, SetDefaultScreenFont,
         AROS_LPA(struct TextFont *, textfont, A0),
         LIBBASETYPEPTR, IntuitionBase, 144, Intuition
);
AROS_LP4(IPTR, DoNotify,
         AROS_LPA(Class *, cl, A0),
         AROS_LPA(Object *, o, A1),
         AROS_LPA(struct ICData *, ic, A2),
         AROS_LPA(struct opUpdate *, msg, A3),
         LIBBASETYPEPTR, IntuitionBase, 145, Intuition
);
AROS_LP1(void, FreeICData,
         AROS_LPA(struct ICData *, icdata, A0),
         LIBBASETYPEPTR, IntuitionBase, 146, Intuition
);
AROS_LP1(struct IntuiMessage *, AllocIntuiMessage,
         AROS_LPA(struct Window *, window, A0),
         LIBBASETYPEPTR, IntuitionBase, 148, Intuition
);
AROS_LP1(void, FreeIntuiMessage,
         AROS_LPA(struct IntuiMessage *, imsg, A0),
         LIBBASETYPEPTR, IntuitionBase, 149, Intuition
);
AROS_LP1(BOOL, LateIntuiInit,
         AROS_LPA(APTR, data, A0),
         LIBBASETYPEPTR, IntuitionBase, 150, Intuition
);
AROS_LP2(void, SendIntuiMessage,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(struct IntuiMessage *, imsg, A1),
         LIBBASETYPEPTR, IntuitionBase, 151, Intuition
);
AROS_LP2(void, ChangeDecoration,
         AROS_LPA(ULONG, ID, D0),
         AROS_LPA(struct NewDecorator *, decor, A0),
         LIBBASETYPEPTR, IntuitionBase, 153, Intuition
);
AROS_LP3(void, WindowAction,
         AROS_LPA(struct Window *, window, A0),
         AROS_LPA(ULONG, action, D0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, IntuitionBase, 157, Intuition
);
AROS_LP7(void, ScrollWindowRasterNoFill,
         AROS_LPA(struct Window *, win, A1),
         AROS_LPA(WORD, dx, D0),
         AROS_LPA(WORD, dy, D1),
         AROS_LPA(WORD, xmin, D2),
         AROS_LPA(WORD, ymin, D3),
         AROS_LPA(WORD, xmax, D4),
         AROS_LPA(WORD, ymax, D5),
         LIBBASETYPEPTR, IntuitionBase, 159, Intuition
);
AROS_LP4(ULONG, SetPointerBounds,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct Rectangle *, rect, A1),
         AROS_LPA(ULONG, reserved, D0),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, IntuitionBase, 160, Intuition
);
AROS_LP1(IPTR, StartScreenNotifyTagList,
         AROS_LPA(struct TagItem *, tags, A0),
         LIBBASETYPEPTR, IntuitionBase, 161, Intuition
);
AROS_LP1(BOOL, EndScreenNotify,
         AROS_LPA(IPTR, notify, A0),
         LIBBASETYPEPTR, IntuitionBase, 162, Intuition
);

#endif /* CLIB_INTUITION_PROTOS_H */
