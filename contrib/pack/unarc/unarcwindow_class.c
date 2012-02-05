/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/xadmaster.h>
#include <proto/alib.h>

#include <libraries/mui.h>
#include <zune/customclasses.h>

#include <stdio.h>

#include "unarcwindow_class.h"
#include "locale.h"

struct UnarcWindow_DATA
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;
};

struct Listentry
{
    BOOL selected;
    struct xadFileInfo *fi;
};

static struct Hook list_display_hook, list_constr_hook, list_destr_hook;

#if 0
AROS_UFH3(ULONG, newentries,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, object, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT
    ....
    return retval;
    AROS_USERFUNC_EXIT
}
#endif

AROS_UFH3(APTR, list_constr_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(APTR, pool, A2),
    AROS_UFHA(struct Listentry *, li, A1))
{
    AROS_USERFUNC_INIT

    struct Listentry *newentry = AllocPooled(pool, sizeof(struct Listentry));
    if (newentry)
    {
        *newentry = *li;
    }
    return newentry;

    AROS_USERFUNC_EXIT
}

AROS_UFH3(void, list_destr_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(APTR, pool, A2),
    AROS_UFHA(struct Listentry *, li, A1))
{
    AROS_USERFUNC_INIT

    FreePooled(pool, li, sizeof(struct Listentry));

    AROS_USERFUNC_EXIT
}

AROS_UFH3(LONG, list_display_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(char **, array, A2),
    AROS_UFHA(struct Listentry *, li, A1))
{
    AROS_USERFUNC_INIT

    if (li->selected)
    {
        *array++ = "*";
    }
    else
    {
        *array++ = "";
    }
    *array = li->fi->xfi_FileName;

    return 0;

    AROS_USERFUNC_EXIT
}

void test(Object *list)
{
    LONG err;
    struct xadArchiveInfo *ai = xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0);
    if (ai)
    {
        err = xadGetInfo(ai, XAD_INFILENAME, "sys:test.lha", TAG_DONE);
        if (!err)
        {
            struct xadFileInfo *fi = ai->xai_FileInfo;
            while (fi)
            {
                struct Listentry newentry;
                newentry.selected = TRUE;
                newentry.fi = fi;
                DoMethod(list, MUIM_List_InsertSingle, &newentry, MUIV_List_Insert_Bottom);
                //printf("index %d name %s\n", (int)fi->xfi_EntryNumber, fi->xfi_FileName);
                fi = fi->xfi_Next;
            }
            //xadFreeInfo(ai);
        }
        else
        {
            puts("xadgetinfo failed");
        }
        //xadFreeObjectA(ai, 0);
    }
    else
    {
        puts("xadallocobject failed");
    }
}

Object *UnarcWindow__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;

    list_constr_hook.h_Entry = (HOOKFUNC)list_constr_func;
    list_destr_hook.h_Entry = (HOOKFUNC)list_destr_func;
    list_display_hook.h_Entry = (HOOKFUNC)list_display_func;

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Window_Title, _(MSG_WI_TITLE),
        MUIA_Window_ID, MAKE_ID('U', 'N', 'W', 'I'),
        WindowContents, VGroup,
            Child, ColGroup(2),
                Child, Label(_(MSG_LA_ARCHIVE)),
                Child, PopaslObject,
                    MUIA_Popasl_Type , ASL_FileRequest,
                    ASLFR_RejectIcons, TRUE,
                    MUIA_Popstring_String, str_file = StringObject,
                        StringFrame,
                    End,
                    MUIA_Popstring_Button, PopButton(MUII_PopFile),
                End,
                Child, Label(_(MSG_LA_TARGETDIR)),
                Child, PopaslObject,
                    MUIA_Popasl_Type , ASL_FileRequest,
                    ASLFR_DrawersOnly, TRUE,
                    MUIA_Popstring_String, str_targetdir = StringObject,
                        StringFrame,
                    End,
                    MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
                End,
            End,
            Child, ga_progress = GaugeObject,
                GaugeFrame,
                MUIA_FixHeight, 15,
                MUIA_Gauge_Horiz, TRUE,
                MUIA_Gauge_Max, 100,
            End,
            Child, ListviewObject,
                MUIA_Listview_Input, FALSE,
                MUIA_Listview_List, lst_content = ListObject,
                    MUIA_Frame, MUIV_Frame_InputList,
                    MUIA_List_Format, "BAR,",
                    MUIA_List_DisplayHook, &list_display_hook,
                    MUIA_List_ConstructHook, &list_constr_hook,
                    MUIA_List_DestructHook, &list_destr_hook,
                End,
            End,
            Child, HGroup,
                Child, btn_all = SimpleButton(_(MSG_BT_ALL)),
                Child, btn_none = SimpleButton(_(MSG_BT_NONE)),
                Child, btn_invert = SimpleButton(_(MSG_BT_INVERT)),
            End,
            Child, HGroup,
                Child, btn_start = SimpleButton(_(MSG_BT_START)),
                Child, HVSpace,
                Child, btn_cancel = SimpleButton(_(MSG_BT_CANCEL)),
            End,
        End,
        TAG_DONE
    );

    if (self)
    {
        struct UnarcWindow_DATA *data = INST_DATA(CLASS, self);

        data->btn_all           = btn_all;
        data->btn_none          = btn_none;
        data->btn_invert        = btn_invert;
        data->btn_start         = btn_start;
        data->btn_cancel        = btn_cancel;
        data->str_file          = str_file;
        data->str_targetdir     = str_targetdir;
        data->lst_content       = lst_content;

        DoMethod
        (
            self, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
        );

        DoMethod
        (
            data->btn_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
            MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
        );

        test(data->lst_content);
    }

    return self;
}


ZUNE_CUSTOMCLASS_1
(
    UnarcWindow, NULL, MUIC_Window, NULL,
    OM_NEW,                       struct opSet *
);
