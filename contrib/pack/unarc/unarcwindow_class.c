/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/xadmaster.h>
#include <proto/alib.h>

#define DEBUG 1
#include <aros/debug.h>

#include <libraries/mui.h>
#include <zune/customclasses.h>

#include "unarcwindow_class.h"
#include "locale.h"

#define PATHNAMESIZE (1024)


struct UnarcWindow_DATA
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;
    struct Hook start_hook, change_selection_hook, read_file_hook;
    struct xadArchiveInfo *ai;
    ULONG entry_cnt;
    STRPTR targetpathname;
};


struct Listentry
{
    BOOL selected;
    struct xadFileInfo *fi;
};

static struct Hook list_display_hook, list_constr_hook, list_destr_hook;

enum
{
    INVERTSINGLE,
    INVERTALL,
    CLEARALL,
    SETALL
};


static BOOL is_file(CONST_STRPTR filename)
{
    if (filename == NULL)
    {
        return FALSE;
    }

    BOOL retval = FALSE;

    BPTR lock = Lock(filename, ACCESS_READ);
    if (lock)
    {
        struct FileInfoBlock *fib = AllocDosObject(DOS_FIB, NULL);
        if (fib)
        {
            BOOL ex = Examine(lock, fib);
            if (ex)
            {
                D(bug("[is_file] file %s direntry_type %d\n", filename, fib->fib_DirEntryType));
                if (fib->fib_DirEntryType < 0) // File
                {
                    retval = TRUE;
                }
            }
            FreeDosObject(DOS_FIB, fib);
        }
        UnLock(lock);
    }
    return retval;
}


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


AROS_UFH3S(LONG, list_display_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(char **, array, A2),
    AROS_UFHA(struct Listentry *, li, A1))
{
    AROS_USERFUNC_INIT

    if (li->selected)
    {
        *array++ = "\033I[6:16]"; // FIXME: that should be a checkmark image
    }
    else
    {
        *array++ = " ";
    }
    *array = li->fi->xfi_FileName;

    return 0;

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, change_selection_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    struct UnarcWindow_DATA *data = h->h_Data;
    ULONG status = *(ULONG *)msg;
    struct Listentry *oldentry, newentry;
    LONG i;

    D(bug("[change_selection_func] status %u\n", status));

    SET(data->lst_content, MUIA_List_Quiet, TRUE);

    switch(status)
    {
        case SETALL:
            NNSET(data->lst_content, MUIA_List_Active, MUIV_List_Active_Off);
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_List_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                if (!oldentry->selected)
                {
                    newentry.selected = TRUE;
                    newentry.fi = oldentry->fi;
                    DoMethod(data->lst_content, MUIM_List_Remove, i);
                    DoMethod(data->lst_content, MUIM_List_InsertSingle, &newentry, i);
                }
            }
            break;
        case CLEARALL:
            NNSET(data->lst_content, MUIA_List_Active, MUIV_List_Active_Off);
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_List_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                if (oldentry->selected)
                {
                    newentry.selected = FALSE;
                    newentry.fi = oldentry->fi;
                    DoMethod(data->lst_content, MUIM_List_Remove, i);
                    DoMethod(data->lst_content, MUIM_List_InsertSingle, &newentry, i);
                }
            }
            break;
        case INVERTALL:
            NNSET(data->lst_content, MUIA_List_Active, MUIV_List_Active_Off);
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_List_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                newentry.selected = oldentry->selected ? FALSE : TRUE;
                newentry.fi = oldentry->fi;
                DoMethod(data->lst_content, MUIM_List_Remove, i);
                DoMethod(data->lst_content, MUIM_List_InsertSingle, &newentry, i);
            }
            break;
        case INVERTSINGLE:
            i = XGET(data->lst_content, MUIA_List_Active);
            if (i != -1)
            {
                NNSET(data->lst_content, MUIA_List_Active, MUIV_List_Active_Off);
                DoMethod(data->lst_content, MUIM_List_GetEntry, i, &oldentry);
                newentry.selected = oldentry->selected ? FALSE : TRUE;
                newentry.fi = oldentry->fi;
                DoMethod(data->lst_content, MUIM_List_Remove, i);
                DoMethod(data->lst_content, MUIM_List_InsertSingle, &newentry, i);
            }
            break;
    }

    SET(data->lst_content, MUIA_List_Quiet, FALSE);

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, start_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[start_func] called\n"));

    struct UnarcWindow_DATA *data = h->h_Data;
    struct Listentry *entry;
    LONG i, result;

    STRPTR destination = (STRPTR)XGET(data->str_targetdir, MUIA_String_Contents);
    for (i = 0; ; i++)
    {
        DoMethod(obj, MUIM_List_GetEntry, i, &entry);
        if (!entry)
            break;
        if (entry->selected)
        {
            strlcpy(data->targetpathname, destination, PATHNAMESIZE);
            result = AddPart(data->targetpathname, entry->fi->xfi_FileName, PATHNAMESIZE);
            if (!result)
            {
               MUI_Request
                (
                    _app(obj), _win(obj), 0, _(MSG_ERR),
                    _(MSG_OK), _(MSG_ERR_ADDPART), data->targetpathname, result
                );
                return;
            }
            D(bug("[start_func] filename %s\n", data->targetpathname));
            result = xadFileUnArc
            (
                data->ai,
                XAD_ENTRYNUMBER, i + 1,
                XAD_OUTFILENAME, data->targetpathname,
                XAD_MAKEDIRECTORY, TRUE,
                XAD_OVERWRITE, TRUE,
                TAG_DONE
            );
            if (result)
            {
                if
                (
                    MUI_Request
                    (
                        _app(obj), _win(obj), 0, _(MSG_ERR),
                        _(MSG_SKIP_CANCEL), _(MSG_ERR_CANT_UNPACK), data->targetpathname, xadGetErrorText(result)
                    ) == 0
                )
                {
                    return;
                }
            }
        }
        SET(data->ga_progress, MUIA_Gauge_Current, i);
    }

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, read_file_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[read_file_func] called\n"));

    struct UnarcWindow_DATA *data = h->h_Data;
    LONG result;

    STRPTR filename = (STRPTR)XGET(data->str_file, MUIA_String_Contents);

    DoMethod(data->lst_content, MUIM_List_Clear);

    xadFreeInfo(data->ai);
    data->entry_cnt = 0;
    SET(data->ga_progress, MUIA_Gauge_Current, 0);
    SET(data->btn_all, MUIA_Disabled, TRUE);
    SET(data->btn_none, MUIA_Disabled, TRUE);
    SET(data->btn_invert, MUIA_Disabled, TRUE);
    SET(data->btn_start, MUIA_Disabled, TRUE);

    result = xadGetInfo(data->ai, XAD_INFILENAME, filename, TAG_DONE);
    if (!result)
    {
        struct xadFileInfo *fi = data->ai->xai_FileInfo;
        while (fi)
        {
            struct Listentry newentry;
            newentry.selected = TRUE;
            newentry.fi = fi;
            DoMethod(data->lst_content, MUIM_List_InsertSingle, &newentry, MUIV_List_Insert_Bottom);
            data->entry_cnt++;
            fi = fi->xfi_Next;
        }
        SET(data->ga_progress, MUIA_Gauge_Max, data->entry_cnt - 1);
        SET(data->btn_all, MUIA_Disabled, FALSE);
        SET(data->btn_none, MUIA_Disabled, FALSE);
        SET(data->btn_invert, MUIA_Disabled, FALSE);
        SET(data->btn_start, MUIA_Disabled, FALSE);
    }
    else
    {
        MUI_RequestA
        (
            NULL, NULL, 0, _(MSG_ERR),
            _(MSG_OK), _(MSG_ERR_NO_ARC), NULL
        );
    }

    AROS_USERFUNC_EXIT
}


Object *UnarcWindow__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;
    STRPTR archive = NULL, destination = NULL;

    struct xadArchiveInfo *ai = xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0);
    STRPTR targetpathname = AllocVec(PATHNAMESIZE, MEMF_ANY);

    if (ai == NULL || targetpathname == NULL)
    {
        // FIXME: I get memory corruption alert if I leave function here
        MUI_RequestA
        (
            NULL, NULL, 0, _(MSG_ERR),
            _(MSG_OK), _(MSG_ERR_NO_MEM), NULL
        );
        return NULL;
    }

    struct TagItem  *tstate = message->ops_AttrList;
    struct TagItem  *tag    = NULL;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_UnarcWindow_Archive:
                archive = (STRPTR)tag->ti_Data;
                break;

            case MUIA_UnarcWindow_Destination:
                destination = (STRPTR)tag->ti_Data;
                break;
        }
    }

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
                    ASLFR_TitleText, _(MSG_FREQ_ARCHIVE_TITLE),
                    ASLFR_RejectIcons, TRUE,
                    MUIA_Popstring_String, str_file = StringObject,
                        StringFrame,
                        MUIA_String_Contents, archive,
                    End,
                    MUIA_Popstring_Button, PopButton(MUII_PopFile),
                End,
                Child, Label(_(MSG_LA_DESTINATION)),
                Child, PopaslObject,
                    MUIA_Popasl_Type , ASL_FileRequest,
                    ASLFR_TitleText, _(MSG_FREQ_DESTINATION_TITLE),
                    ASLFR_DrawersOnly, TRUE,
                    MUIA_Popstring_String, str_targetdir = StringObject,
                        StringFrame,
                        MUIA_String_Contents, destination,
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
                    MUIA_List_Format, ",",
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
            Child, (IPTR) RectangleObject, 
                MUIA_Rectangle_HBar, TRUE, 
                MUIA_FixHeight,      2, 
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
        data->ga_progress       = ga_progress;
        data->ai                = ai;

        data->start_hook.h_Entry = (HOOKFUNC)start_func;
        data->start_hook.h_Data = data;
        data->read_file_hook.h_Entry = (HOOKFUNC)read_file_func;
        data->read_file_hook.h_Data = data;
        data->change_selection_hook.h_Entry = (HOOKFUNC)change_selection_func;
        data->change_selection_hook.h_Data = data;

        data->targetpathname = targetpathname;

        SET(data->btn_all, MUIA_Disabled, TRUE);
        SET(data->btn_none, MUIA_Disabled, TRUE);
        SET(data->btn_invert, MUIA_Disabled, TRUE);
        SET(data->btn_start, MUIA_Disabled, TRUE);

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

        DoMethod
        (
            data->btn_all, MUIM_Notify, MUIA_Pressed, FALSE,
            data->lst_content, 3, MUIM_CallHook, &data->change_selection_hook, SETALL
        );

        DoMethod
        (
            data->btn_none, MUIM_Notify, MUIA_Pressed, FALSE,
            data->lst_content, 3, MUIM_CallHook, &data->change_selection_hook, CLEARALL
        );

        DoMethod
        (
            data->btn_invert, MUIM_Notify, MUIA_Pressed, FALSE,
            data->lst_content, 3, MUIM_CallHook, &data->change_selection_hook, INVERTALL
        );

        DoMethod
        (
            data->lst_content, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
            data->lst_content, 3, MUIM_CallHook, &data->change_selection_hook, INVERTSINGLE
        );

        DoMethod
        (
            data->btn_start, MUIM_Notify, MUIA_Pressed, FALSE,
            data->lst_content, 2, MUIM_CallHook, &data->start_hook
        );

        DoMethod
        (
            data->str_file, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            data->lst_content, 2, MUIM_CallHook, &data->read_file_hook
        );

        if (is_file(archive))
        {
            // fill the list if we have an existing file
            DoMethod
            (
                data->lst_content, MUIM_CallHook, &data->read_file_hook
            );
        }
    }
    return self;
}


IPTR UnarcWindow__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct UnarcWindow_DATA *data = INST_DATA(CLASS, self);

    if (data->ai)
        xadFreeObjectA(data->ai, 0);
    FreeVec(data->targetpathname);

    return DoSuperMethodA(CLASS, self, message);
}


ZUNE_CUSTOMCLASS_2
(
    UnarcWindow, NULL, MUIC_Window, NULL,
    OM_NEW,             struct opSet *,
    OM_DISPOSE,         Msg
);
