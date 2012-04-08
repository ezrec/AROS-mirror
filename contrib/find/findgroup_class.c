/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/alib.h>

//#define DEBUG 1
#include <aros/debug.h>

#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <zune/customclasses.h>

#include <stdio.h>

#include "findgroup_class.h"
#include "locale.h"

#define PATHNAMESIZE (1024)


struct FindGroup_DATA
{
    Object *btn_start, *btn_stop;
    Object *str_pattern, *str_contents, *lst_paths, *lst_found;
    Object *txt_status;
    struct Hook start_hook, change_selection_hook, read_file_hook, expand_dest_hook;
    ULONG found_cnt;
};


struct Listentry
{
    //BOOL selected;
    //struct xadFileInfo *fi;
};

static struct Hook list_display_hook, list_constr_hook, list_destr_hook;


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
#if 0
        static TEXT protbuf[8], sizebuf[20], crsizebuf[20], percentbuf[8];
        static TEXT datebuf[10], timebuf[10];
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
#endif
    }
    else
    {
        *array++ = (STRPTR)_(MSG_LBL_FULLPATH);
        *array++ = (STRPTR)_(MSG_LBL_SIZE);
        *array++ = (STRPTR)_(MSG_LBL_PROTECTION);
        *array++ = (STRPTR)_(MSG_LBL_DATE);
        *array++ = (STRPTR)_(MSG_LBL_TIME);
        *array++ = (STRPTR)_(MSG_LBL_COMMENT);
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

    //struct FindGroup_DATA *data = h->h_Data;
    //ULONG status = *(ULONG *)msg;
    //struct Listentry *oldentry, newentry;
    //LONG i;
    //TEXT buffer[50];

    D(bug("[change_selection_func] status %u\n", status));

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, start_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[start_func] called\n"));

    //struct FindGroup_DATA *data = h->h_Data;
    //struct Listentry *entry;
    //LONG i, result;

    AROS_USERFUNC_EXIT
}


AROS_UFH3S(void, read_file_func,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    D(bug("[read_file_func] called\n"));

    //struct FindGroup_DATA *data = h->h_Data;
    //LONG result;
    //TEXT buffer[50];


    AROS_USERFUNC_EXIT
}


Object *FindGroup__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    Object *btn_start, *btn_stop;
    Object *str_pattern, *str_contents, *lst_paths, *lst_found;
    Object *txt_status;

#if 0
    struct TagItem  *tstate = message->ops_AttrList;
    struct TagItem  *tag    = NULL;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_FindGroup_Archive:
                archive = (STRPTR)tag->ti_Data;
                break;

            case MUIA_FindGroup_Destination:
                destination = (STRPTR)tag->ti_Data;
                break;
        }
    }
#endif

    list_constr_hook.h_Entry = (HOOKFUNC)list_constr_func;
    list_destr_hook.h_Entry = (HOOKFUNC)list_destr_func;
    list_display_hook.h_Entry = (HOOKFUNC)list_display_func;

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Group_Horiz, TRUE,
        Child, NListviewObject,
            MUIA_NListview_NList, lst_paths = NListObject,
            End,
        End,
        Child, VGroup,
            Child, ColGroup(2),
                Child, Label(_(MSG_LBL_PATTERN)),
                Child, str_pattern = StringObject,
                    StringFrame,
                End,
                Child, Label(_(MSG_LBL_CONTENTS)),
                Child, str_contents = StringObject,
                    StringFrame,
                End,
            End,
            Child, NListviewObject,
                    MUIA_NListview_NList, lst_found = NListObject,
                    MUIA_Frame, MUIV_Frame_InputList,
                    MUIA_NList_Input, FALSE,
                    MUIA_NList_Format, "BAR,BAR P=\33r,BAR P=\33r,BAR P=\33r,BAR P=\33r,",
                    MUIA_NList_Title, TRUE,
                    MUIA_NList_DisplayHook, &list_display_hook,
                    MUIA_NList_ConstructHook, &list_constr_hook,
                    MUIA_NList_DestructHook, &list_destr_hook,
                End,
            End,
            Child, txt_status = TextObject,
                TextFrame,
            End,
            Child, HGroup,
                Child, btn_start = SimpleButton(_(MSG_BT_START)),
                Child, btn_stop = SimpleButton(_(MSG_BT_STOP)),
            End,
        End,
        TAG_MORE, (IPTR)message->ops_AttrList,
        TAG_DONE
    );

    if (self)
    {
        struct FindGroup_DATA *data = INST_DATA(CLASS, self);

        data->btn_start         = btn_start;
        data->btn_stop          = btn_stop;
        data->str_pattern       = str_pattern;
        data->str_contents      = str_contents;
        data->lst_paths         = lst_paths;
        data->lst_found         = lst_found;
        data->txt_status        = txt_status;

        data->start_hook.h_Entry = (HOOKFUNC)start_func;
        data->start_hook.h_Data = data;

        SET(data->btn_stop, MUIA_Disabled, TRUE);

        DoMethod
        (
            data->btn_start, MUIM_Notify, MUIA_Pressed, FALSE,
            self, 2, MUIM_CallHook, &data->start_hook
        );

#if 0
        DoMethod
        (
            data->lst_content, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime,
            data->lst_content, 3, MUIM_CallHook, &data->change_selection_hook, INVERTSINGLE
        );
#endif
    }
    return self;
}


IPTR FindGroup__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    //struct FindGroup_DATA *data = INST_DATA(CLASS, self);

    return DoSuperMethodA(CLASS, self, message);
}


ZUNE_CUSTOMCLASS_2
(
    FindGroup, NULL, MUIC_Group, NULL,
    OM_NEW,             struct opSet *,
    OM_DISPOSE,         Msg
);
