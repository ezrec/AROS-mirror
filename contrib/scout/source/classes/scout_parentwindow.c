/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

struct ParentWinData {
    struct MinList pwd_ChildWindowList;
    struct SignalSemaphore pwd_ChildWindowListSema;
    APTR pwd_MemoryPool;
    APTR pwd_ParentWindow;
    ULONG pwd_ChildWindowCount;
    ULONG pwd_MaxChildWindowCount;
};

struct ChildWindowNode {
    struct MinNode cwn_Node;
    APTR cwn_ChildWindow;
};

struct MUIP_AddChildWindowMessage {
    STACKED ULONG MethodID;
    STACKED APTR window;
};

struct MUIP_RemChildWindowMessage {
    STACKED ULONG MethodID;
    STACKED APTR window;
};

STATIC void CloseChildWindow( struct ParentWinData *pwd,
                              struct ChildWindowNode *cwn )
{
    APTR app;

    nnset(cwn->cwn_ChildWindow, MUIA_Window_Open, FALSE);

    app = (APTR)xget(cwn->cwn_ChildWindow, MUIA_ApplicationObject);
    DoMethod(app, OM_REMMEMBER, cwn->cwn_ChildWindow);

    MUI_DisposeObject(cwn->cwn_ChildWindow);
    tbFreeVecPooled(pwd->pwd_MemoryPool, cwn);
}

STATIC void CloseChildWindows( struct ParentWinData *pwd )
{
    struct ChildWindowNode *cwn;

    ObtainSemaphore(&pwd->pwd_ChildWindowListSema);

    while ((cwn = (struct ChildWindowNode *)RemHead((struct List *)&pwd->pwd_ChildWindowList)) != NULL) {
        CloseChildWindow(pwd, cwn);
    }
    pwd->pwd_ChildWindowCount = 0;

    ReleaseSemaphore(&pwd->pwd_ChildWindowListSema);
}

STATIC void CloseSuperflousChildWindows( struct IClass *cl,
                                         Object *obj )

{
    struct ParentWinData *pwd = INST_DATA(cl, obj);

    if (pwd->pwd_MaxChildWindowCount > 0) {
        ULONG cnt;
        struct ChildWindowNode *cwn;

        ObtainSemaphore(&pwd->pwd_ChildWindowListSema);

        cnt = pwd->pwd_ChildWindowCount;

        ITERATE_LIST(&pwd->pwd_ChildWindowList, struct ChildWindowNode *, cwn) {
            if (cnt > pwd->pwd_MaxChildWindowCount) {
                // just notify the window to close, everything is is done later by
                // MUIM_Window_RemChildWindow which will automatically be called
                set(cwn->cwn_ChildWindow, MUIA_Window_CloseRequest, TRUE);
                cnt--;
            } else {
                set(cwn->cwn_ChildWindow, MUIA_Window_MaxChildWindowCount, pwd->pwd_MaxChildWindowCount);
            }
        }

        ReleaseSemaphore(&pwd->pwd_ChildWindowListSema);
    } else {
        struct ChildWindowNode *cwn;

        ObtainSemaphore(&pwd->pwd_ChildWindowListSema);

        ITERATE_LIST(&pwd->pwd_ChildWindowList, struct ChildWindowNode *, cwn) {
            set(cwn->cwn_ChildWindow, MUIA_Window_MaxChildWindowCount, pwd->pwd_MaxChildWindowCount);
        }

        ReleaseSemaphore(&pwd->pwd_ChildWindowListSema);
    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    if ((obj = (Object *)DoSuperNew(cl, obj, TAG_MORE, msg->ops_AttrList)) != NULL) {
        struct ParentWinData *pwd = INST_DATA(cl, obj);

        NewList((struct List *)&pwd->pwd_ChildWindowList);
        InitSemaphore(&pwd->pwd_ChildWindowListSema);

        pwd->pwd_ParentWindow = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);
        pwd->pwd_MaxChildWindowCount = GetTagData(MUIA_Window_MaxChildWindowCount, 0, msg->ops_AttrList);

        if ((pwd->pwd_MemoryPool = tbCreatePool(MEMF_CLEAR, 4096, 4096)) != NULL) {
            DoMethod(obj, MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Self, 1, MUIM_Window_CloseChildWindows);
        } else {
            CoerceMethod(cl, obj, OM_DISPOSE);
            obj = NULL;
        }
    }

    return (IPTR)obj;
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct ParentWinData *pwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                pwd->pwd_ParentWindow = (APTR)tag->ti_Data;
                break;

            case MUIA_Window_MaxChildWindowCount:
                pwd->pwd_MaxChildWindowCount = tag->ti_Data;
                CloseSuperflousChildWindows(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

STATIC IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct ParentWinData *pwd = INST_DATA(cl, obj);

    switch (msg->opg_AttrID) {
        case MUIA_Window_ParentWindow:
            *msg->opg_Storage = (IPTR)pwd->pwd_ParentWindow;
            return TRUE;
            break;

        case MUIA_Window_MaxChildWindowCount:
            *msg->opg_Storage = pwd->pwd_MaxChildWindowCount;
            return TRUE;
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ParentWinData *pwd = INST_DATA(cl, obj);

    CloseChildWindows(pwd);
    if (pwd->pwd_MemoryPool) tbDeletePool(pwd->pwd_MemoryPool);

    nnset(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mAddChildWindow( struct IClass *cl,
                              Object *obj,
                              struct MUIP_AddChildWindowMessage *msg )
{
    IPTR result = 0;
    struct ParentWinData *pwd = INST_DATA(cl, obj);
    struct ChildWindowNode *cwn;

    if ((cwn = tbAllocVecPooled(pwd->pwd_MemoryPool, sizeof(struct ChildWindowNode))) != NULL) {
        cwn->cwn_ChildWindow = msg->window;

        ObtainSemaphore(&pwd->pwd_ChildWindowListSema);

        pwd->pwd_ChildWindowCount++;
        AddTail((struct List *)&pwd->pwd_ChildWindowList, (struct Node *)cwn);

        ReleaseSemaphore(&pwd->pwd_ChildWindowListSema);

        CloseSuperflousChildWindows(cl, obj);

        DoMethod(_app(obj), OM_ADDMEMBER, msg->window);

        result = 1;
    }

    return result;
}

STATIC IPTR mRemChildWindow( struct IClass *cl,
                              Object *obj,
                              struct MUIP_RemChildWindowMessage *msg )
{
    IPTR result = 0;
    struct ParentWinData *pwd = INST_DATA(cl, obj);
    struct ChildWindowNode *cwn;

    ObtainSemaphore(&pwd->pwd_ChildWindowListSema);

    ITERATE_LIST(&pwd->pwd_ChildWindowList, struct ChildWindowNode *, cwn) {
        if (cwn->cwn_ChildWindow == msg->window) {
            Remove((struct Node *)cwn);
            CloseChildWindow(pwd, cwn);
            pwd->pwd_ChildWindowCount--;

            result = 1;

            break;
        }
    }

    ReleaseSemaphore(&pwd->pwd_ChildWindowListSema);

    return result;
}

STATIC IPTR mCloseChildWindows( struct IClass *cl,
                                 Object *obj,
                                 UNUSED Msg msg )
{
    struct ParentWinData *pwd = INST_DATA(cl, obj);

    CloseChildWindows(pwd);

    return 0;
}

DISPATCHER(ParentWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                        return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                        return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                        return (mGet(cl, obj, (APTR)msg));
        case OM_DISPOSE:                    return (mDispose(cl, obj, (APTR)msg));
        case MUIM_Window_AddChildWindow:    return (mAddChildWindow(cl, obj, (APTR)msg));
        case MUIM_Window_RemChildWindow:    return (mRemChildWindow(cl, obj, (APTR)msg));
        case MUIM_Window_CloseChildWindows: return (mCloseChildWindows(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeParentWinClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Window,  NULL, sizeof(struct ParentWinData), ENTRY(ParentWinDispatcher));
}

