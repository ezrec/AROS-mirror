/*
    Copyright © 2012-2017, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/xadmaster.h>
#include <proto/alib.h>

//#define DEBUG 1
#include <aros/debug.h>

#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <zune/customclasses.h>

#include <stdio.h>

#include "unarcgroup_class.h"
#include "locale.h"

#define PATHNAMESIZE (1024)


struct UnarcGroup_DATA
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;
    Object *txt_status_total, *txt_status_select;
    struct Hook start_hook, change_selection_hook, read_file_hook, expand_dest_hook;
    struct xadArchiveInfo *ai;
    ULONG total_entry_cnt;
    ULONG total_size;
    ULONG select_entry_cnt;
    ULONG select_size;
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


static STRPTR alloc_name_from_lock(CONST_STRPTR filename)
{
    STRPTR result = NULL;
    STRPTR buffer = AllocVec(PATHNAMESIZE, MEMF_ANY);
    if (buffer)
    {
        BPTR lock = Lock(filename, ACCESS_READ);
        if (lock)
        {
            if (NameFromLock(lock, buffer, PATHNAMESIZE))
            {
                result = buffer;
            }
            UnLock(lock);
        }
    }
    return result;
}


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

    if (li)
    {
        static TEXT protbuf[8], sizebuf[20], crsizebuf[20], percentbuf[8];
        static TEXT datebuf[20], timebuf[20];
        struct DateTime dt;

        // Protection bits
        protbuf[0] = li->fi->xfi_Protection & FIBF_SCRIPT  ? 's' : '-';
        protbuf[1] = li->fi->xfi_Protection & FIBF_PURE    ? 'p' : '-';
        protbuf[2] = li->fi->xfi_Protection & FIBF_ARCHIVE ? 'a' : '-';

        // The following bits are high-active!
        protbuf[3] = li->fi->xfi_Protection & FIBF_READ    ? '-' : 'r';
        protbuf[4] = li->fi->xfi_Protection & FIBF_WRITE   ? '-' : 'w';
        protbuf[5] = li->fi->xfi_Protection & FIBF_EXECUTE ? '-' : 'e';
        protbuf[6] = li->fi->xfi_Protection & FIBF_DELETE  ? '-' : 'd';
        protbuf[7] = '\0';

        // Sizes
        snprintf(sizebuf, sizeof sizebuf, "%lu", (long unsigned int)li->fi->xfi_Size);
        snprintf(crsizebuf, sizeof crsizebuf, "%lu", (long unsigned int)li->fi->xfi_CrunchSize);
        if (li->fi->xfi_Size)
        {
            snprintf(percentbuf, sizeof percentbuf, "%u %%", (unsigned int)(100 - li->fi->xfi_CrunchSize * 100 / li->fi->xfi_Size));
        }
        else
        {
            strlcpy(percentbuf, "100 %", sizeof percentbuf);
        }

        // Date
        datebuf[0] = '\0';
        timebuf[0] = '\0';
        dt.dat_Format = FORMAT_DEF;
        dt.dat_Flags = 0;
        dt.dat_StrDay = NULL;
        dt.dat_StrDate = datebuf;
        dt.dat_StrTime = timebuf;
        if (xadConvertDates(XAD_DATEXADDATE, &li->fi->xfi_Date, XAD_GETDATEDATESTAMP, &dt, TAG_DONE) == 0)
        {
            DateToStr(&dt);
        }

        if (li->selected)
        {
            *array++ = "\033I[5:PROGDIR:Images/selected]";
        }
        else
        {
            *array++ = "\033I[5:PROGDIR:Images/unselected]";
        }
        *array++ = li->fi->xfi_FileName;
        *array++ = protbuf;
        *array++ = sizebuf;
        *array++ = crsizebuf;
        *array++ = percentbuf;
        *array++ = datebuf;
        *array++ = timebuf;
    }
    else
    {
        *array++ = "";
        *array++ = (STRPTR)_(MSG_LBL_NAME);
        *array++ = (STRPTR)_(MSG_LBL_PROTECTION);
        *array++ = (STRPTR)_(MSG_LBL_SIZE);
        *array++ = (STRPTR)_(MSG_LBL_PACKSIZE);
        *array++ = (STRPTR)_(MSG_LBL_RATE);
        *array++ = (STRPTR)_(MSG_LBL_DATE);
        *array++ = (STRPTR)_(MSG_LBL_TIME);
    }

    return 0;

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, change_selection_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    struct UnarcGroup_DATA *data = h->h_Data;
    ULONG status = *(ULONG *)msg;
    struct Listentry *oldentry, newentry;
    LONG i;
    TEXT buffer[50];

    D(bug("[change_selection_func] status %u\n", status));

    SET(data->lst_content, MUIA_List_Quiet, TRUE);

    switch(status)
    {
        case SETALL:
            data->select_size = data->total_size;
            data->select_entry_cnt = data->total_entry_cnt;
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_NList_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                if (!oldentry->selected)
                {
                    newentry.selected = TRUE;
                    newentry.fi = oldentry->fi;
                    DoMethod(data->lst_content, MUIM_NList_Remove, i);
                    DoMethod(data->lst_content, MUIM_NList_InsertSingle, &newentry, i);
                }
            }
            break;

        case CLEARALL:
            data->select_size = 0;
            data->select_entry_cnt = 0;
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_NList_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                if (oldentry->selected)
                {
                    newentry.selected = FALSE;
                    newentry.fi = oldentry->fi;
                    DoMethod(data->lst_content, MUIM_NList_Remove, i);
                    DoMethod(data->lst_content, MUIM_NList_InsertSingle, &newentry, i);
                }
            }
            break;

        case INVERTALL:
            data->select_size = 0;
            data->select_entry_cnt = 0;
            for (i = 0; ; i++)
            {
                DoMethod(data->lst_content, MUIM_NList_GetEntry, i, &oldentry);
                if (!oldentry)
                    break;
                newentry.selected = oldentry->selected ? FALSE : TRUE;
                newentry.fi = oldentry->fi;
                DoMethod(data->lst_content, MUIM_NList_Remove, i);
                DoMethod(data->lst_content, MUIM_NList_InsertSingle, &newentry, i);
                if (newentry.selected)
                {
                    data->select_size += newentry.fi->xfi_Size;
                    data->select_entry_cnt++;            
                }
            }
            break;

        case INVERTSINGLE:
            i = XGET(data->lst_content, MUIA_NList_EntryClick);
            if (i >= 0)
            {
                DoMethod(data->lst_content, MUIM_NList_GetEntry, i, &oldentry);
                newentry.selected = oldentry->selected ? FALSE : TRUE;
                newentry.fi = oldentry->fi;
                DoMethod(data->lst_content, MUIM_NList_Remove, i);
                DoMethod(data->lst_content, MUIM_NList_InsertSingle, &newentry, i);
                if (newentry.selected)
                {
                    data->select_size += newentry.fi->xfi_Size;
                    data->select_entry_cnt++;            
                }
                else
                {
                    data->select_size -= newentry.fi->xfi_Size;
                    data->select_entry_cnt--;            
                }
            }
            break;
    }

    SET(data->lst_content, MUIA_List_Quiet, FALSE);
    snprintf(buffer, sizeof buffer, _(MSG_TXT_STATUS_SEL), data->select_size, data->select_entry_cnt);
    SET(data->txt_status_select, MUIA_Text_Contents, buffer);

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, start_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[start_func] called\n"));

    struct UnarcGroup_DATA *data = h->h_Data;
    struct Listentry *entry;
    LONG i, result;

    STRPTR destination = (STRPTR)XGET(data->str_targetdir, MUIA_String_Contents);
    if (destination[0] == '\0')
    {
        // FIXME: what if user selects "" as destination when starting from CLI?
        MUI_Request
        (
            _app(obj), _win(obj), 0, _(MSG_ERR),
            _(MSG_OK), _(MSG_ERR_NODEST)
        );
        return;
    }

    for (i = 0; ; i++)
    {
        DoMethod(obj, MUIM_NList_GetEntry, i, &entry);
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
            if (entry->fi->xfi_Flags & XADFIF_DIRECTORY)
            {
                // FIXME: create directory (needed if empty)
            }
            else
            {
            result = xadFileUnArc
            (
                data->ai,
                XAD_ENTRYNUMBER, i + 1,
                XAD_OUTFILENAME, data->targetpathname,
                XAD_MAKEDIRECTORY, TRUE,
                XAD_OVERWRITE, TRUE,
                TAG_DONE
            );
            D(bug("[start_func] xadFileUnArc result %d\n", result));
            if (result !=0 && result != XADERR_BADPARAMS) // FIXME: why do I have to catch that error?
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

    struct UnarcGroup_DATA *data = h->h_Data;
    LONG result;
    TEXT buffer[50];

    STRPTR filename = (STRPTR)XGET(data->str_file, MUIA_String_Contents);
    STRPTR newfilename = alloc_name_from_lock(filename); // expand to full path
    if (newfilename)
    {
        NNSET(data->str_file, MUIA_String_Contents, newfilename);
        filename = (STRPTR)XGET(data->str_file, MUIA_String_Contents);
        FreeVec(newfilename);
    }

    DoMethod(data->lst_content, MUIM_NList_Clear);

    xadFreeInfo(data->ai);
    data->total_entry_cnt = 0;
    data->total_size = 0;
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
            DoMethod(data->lst_content, MUIM_NList_InsertSingle, &newentry, MUIV_List_Insert_Bottom);
            data->total_entry_cnt++;
            data->total_size += fi->xfi_Size;
            fi = fi->xfi_Next;
        }
        SET(data->ga_progress, MUIA_Gauge_Max, data->total_entry_cnt - 1);
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
    snprintf(buffer, sizeof buffer, _(MSG_TXT_STATUS_ALL), data->total_size, data->total_entry_cnt);
    SET(data->txt_status_total, MUIA_Text_Contents, buffer);

    data->select_entry_cnt = data->total_entry_cnt;
    data->select_size = data->total_size;
    snprintf(buffer, sizeof buffer, _(MSG_TXT_STATUS_SEL), data->select_size, data->select_entry_cnt);
    SET(data->txt_status_select, MUIA_Text_Contents, buffer);

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, expand_dest_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[expand_dest_func] called\n"));

    struct UnarcGroup_DATA *data = h->h_Data;

    STRPTR targetdir = (STRPTR)XGET(data->str_targetdir, MUIA_String_Contents);
    STRPTR newtargetdir = alloc_name_from_lock(targetdir); // expand to full path
    if (newtargetdir)
    {
        NNSET(data->str_targetdir, MUIA_String_Contents, newtargetdir);
        FreeVec(newtargetdir);
    }

    AROS_USERFUNC_EXIT
}


Object *UnarcGroup__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    Object *btn_all, *btn_none, *btn_invert, *btn_start, *btn_cancel;
    Object *str_file, *str_targetdir, *lst_content, *ga_progress;
    Object *txt_status_total, *txt_status_select;
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
            case MUIA_UnarcGroup_Archive:
                archive = (STRPTR)tag->ti_Data;
                break;

            case MUIA_UnarcGroup_Destination:
                destination = (STRPTR)tag->ti_Data;
                break;
        }
    }

    if (archive == NULL) archive = "SYS:";
    if (destination == NULL) destination = "RAM:";

    list_constr_hook.h_Entry = (HOOKFUNC)list_constr_func;
    list_destr_hook.h_Entry = (HOOKFUNC)list_destr_func;
    list_display_hook.h_Entry = (HOOKFUNC)list_display_func;

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
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
        Child, NListviewObject,
            MUIA_NListview_NList, lst_content = NListObject,
                MUIA_Frame, MUIV_Frame_InputList,
                MUIA_NList_Input, FALSE,
                MUIA_NList_Format, "BAR,BAR,P=\33r BAR,P=\33r BAR,P=\33r BAR,P=\33r BAR,BAR,",
                MUIA_NList_Title, TRUE,
                MUIA_NList_DisplayHook, &list_display_hook,
                MUIA_NList_ConstructHook, &list_constr_hook,
                MUIA_NList_DestructHook, &list_destr_hook,
            End,
        End,
        Child, HGroup,
            Child, txt_status_total = TextObject,
                TextFrame,
            End,
            Child, txt_status_select = TextObject,
                TextFrame,
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
        TAG_MORE, (IPTR)message->ops_AttrList,
        TAG_DONE
    );

    if (self)
    {
        struct UnarcGroup_DATA *data = INST_DATA(CLASS, self);

        data->btn_all           = btn_all;
        data->btn_none          = btn_none;
        data->btn_invert        = btn_invert;
        data->btn_start         = btn_start;
        data->btn_cancel        = btn_cancel;
        data->str_file          = str_file;
        data->str_targetdir     = str_targetdir;
        data->lst_content       = lst_content;
        data->ga_progress       = ga_progress;
        data->txt_status_total  = txt_status_total;
        data->txt_status_select = txt_status_select;
        data->ai                = ai;

        data->start_hook.h_Entry = (HOOKFUNC)start_func;
        data->start_hook.h_Data = data;
        data->read_file_hook.h_Entry = (HOOKFUNC)read_file_func;
        data->read_file_hook.h_Data = data;
        data->change_selection_hook.h_Entry = (HOOKFUNC)change_selection_func;
        data->change_selection_hook.h_Data = data;
        data->expand_dest_hook.h_Entry = (HOOKFUNC)expand_dest_func;
        data->expand_dest_hook.h_Data = data;

        data->targetpathname = targetpathname;

        SET(data->btn_all, MUIA_Disabled, TRUE);
        SET(data->btn_none, MUIA_Disabled, TRUE);
        SET(data->btn_invert, MUIA_Disabled, TRUE);
        SET(data->btn_start, MUIA_Disabled, TRUE);

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
            data->lst_content, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime,
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

        DoMethod
        (
            data->str_targetdir, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            data->str_targetdir, 2, MUIM_CallHook, &data->expand_dest_hook
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


IPTR UnarcGroup__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct UnarcGroup_DATA *data = INST_DATA(CLASS, self);

    if (data->ai)
    {
        xadFreeInfo(data->ai);
        xadFreeObjectA(data->ai, 0);
    }
    FreeVec(data->targetpathname);

    return DoSuperMethodA(CLASS, self, message);
}


ZUNE_CUSTOMCLASS_2
(
    UnarcGroup, NULL, MUIC_Group, NULL,
    OM_NEW,             struct opSet *,
    OM_DISPOSE,         Msg
);
