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

struct FlagsWinData {
    APTR fwd_FlagsText;
    APTR fwd_FlagsList;
    ULONG fwd_FlagsValue;
    ULONG fwd_FlagsType;
};

struct MUIP_FlagsWin_ShowFlagsMessage {
    STACKED ULONG MethodID;
    STACKED ULONG value;
    STACKED ULONG type;
    STACKED void *bitarray;
    STACKED void *maskarray;
    STACKED UBYTE *title;
};

struct FlagsEntry {
    TEXT fe_Name[64];
    TEXT fe_Value[32];
};

HOOKPROTONHNO(flaglist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct FlagsEntry));
}
MakeStaticHook(flaglist_con2hook, flaglist_con2func);

HOOKPROTONHNO(flaglist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(flaglist_des2hook, flaglist_des2func);

HOOKPROTONHNO(flaglist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct FlagsEntry *fe = (struct FlagsEntry *)msg->entry;

    if (fe) {
        msg->strings[0] = fe->fe_Name;
        msg->strings[1] = fe->fe_Value;
    } else {
        msg->strings[0] = txtFlagsName;
        msg->strings[1] = txtFlagsValue;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(flaglist_dsp2hook, flaglist_dsp2func);

HOOKPROTONHNO(flaglist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    struct FlagsEntry *fe1, *fe2;

    fe1 = (struct FlagsEntry *)msg->entry1;
    fe2 = (struct FlagsEntry *)msg->entry2;

    return stricmp(fe1->fe_Value, fe2->fe_Value);
}
MakeStaticHook(flaglist_cmp2hook, flaglist_cmp2func);

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR flagtext, flaglist;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_Window_ID, MakeID('F','L','A','G'),
        WindowContents, VGroup,
            Child, (IPTR)HGroup,
                Child, (IPTR)HSpace(0),
                Child, (IPTR)(flagtext = (Object *)MyLabel2("")),
                Child, (IPTR)HSpace(0),
            End,
            Child, (IPTR)MyNListviewObject(&flaglist, MakeID('F','L','L','V'), "BAR,BAR", &flaglist_con2hook, &flaglist_des2hook, &flaglist_dsp2hook, &flaglist_cmp2hook, FALSE),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct FlagsWinData *fwd = INST_DATA(cl, obj);
        APTR parent;

        fwd->fwd_FlagsText = flagtext;
        fwd->fwd_FlagsList = flaglist;

        set(obj, MUIA_Window_DefaultObject, flaglist);

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        DoMethod(parent,   MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,      MUIM_Notify,              MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct FlagsWinData *fwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(fwd->fwd_FlagsList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mShowFlags( struct IClass *cl,
                         Object *obj,
                         Msg msg )
{
    struct FlagsEntry *fe;

    if ((fe = tbAllocVecPooled(globalPool, sizeof(struct FlagsEntry))) != NULL) {
        struct FlagsWinData *fwd = INST_DATA(cl, obj);
        struct MUIP_FlagsWin_ShowFlagsMessage *sfm = (struct MUIP_FlagsWin_ShowFlagsMessage *)msg;
        TEXT binflags[16];

        switch (sfm->type) {
            case MUIV_FlagsWin_Type_Byte:
                Flags2BinStr(sfm->value, binflags, sizeof(binflags));
                MySetContents(fwd->fwd_FlagsText, "%s = %s", sfm->title, binflags);
                break;

            case MUIV_FlagsWin_Type_Word:
                MySetContents(fwd->fwd_FlagsText, "%s = $%04lx", sfm->title, sfm->value);
                break;

            case MUIV_FlagsWin_Type_Long:
                MySetContents(fwd->fwd_FlagsText, "%s = $%08lx", sfm->title, sfm->value);
                break;
        }

        if (sfm->bitarray) {
            struct LongFlag *lf = (struct LongFlag *)sfm->bitarray;
            ULONG i;

            i = 0;
            while (lf[i].lf_Name) {
                if (FLAG_IS_SET(sfm->value, lf[i].lf_Value)) {
                    stccpy(fe->fe_Name, lf[i].lf_Name, sizeof(fe->fe_Name));
                    switch (sfm->type) {
                        case MUIV_FlagsWin_Type_Byte:
                            Flags2BinStr(lf[i].lf_Value, fe->fe_Value, sizeof(fe->fe_Value));
                            break;

                        case MUIV_FlagsWin_Type_Word:
                            _snprintf(fe->fe_Value, sizeof(fe->fe_Value), "$%04lx", lf[i].lf_Value);
                            break;

                        case MUIV_FlagsWin_Type_Long:
                            _snprintf(fe->fe_Value, sizeof(fe->fe_Value), "$%08lx", lf[i].lf_Value);
                            break;
                    }

                    InsertSortedEntry(fwd->fwd_FlagsList, fe);
                }
                i++;
            }
        }
        if (sfm->maskarray) {
            struct MaskedLongFlag *mlf = (struct MaskedLongFlag *)sfm->maskarray;
            ULONG i;

            i = 0;
            while (mlf[i].mlf_Name) {
                if ((sfm->value & mlf[i].mlf_Mask) == mlf[i].mlf_Value) {
                    stccpy(fe->fe_Name, mlf[i].mlf_Name, sizeof(fe->fe_Name));
                    switch (sfm->type) {
                        case MUIV_FlagsWin_Type_Byte:
                            Flags2BinStr(mlf[i].mlf_Value, fe->fe_Value, sizeof(fe->fe_Value));
                            break;

                        case MUIV_FlagsWin_Type_Word:
                            _snprintf(fe->fe_Value, sizeof(fe->fe_Value), "$%04lx", mlf[i].mlf_Value);
                            break;

                        case MUIV_FlagsWin_Type_Long:
                            _snprintf(fe->fe_Value, sizeof(fe->fe_Value), "$%08lx", mlf[i].mlf_Value);
                            break;
                    }

                    InsertSortedEntry(fwd->fwd_FlagsList, fe);
                }
                i++;
            }
        }

        tbFreeVecPooled(globalPool, fe);
    }

    return 0;
}

DISPATCHER(FlagsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                  return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:              return (mDispose(cl, obj, (APTR)msg));
        case MUIM_FlagsWin_ShowFlags: return (mShowFlags(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeFlagsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct FlagsWinData), ENTRY(FlagsWinDispatcher));
}

