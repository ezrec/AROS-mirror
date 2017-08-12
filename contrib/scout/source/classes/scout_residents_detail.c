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

struct ResidentsDetailWinData {
    TEXT rdwd_Title[WINDOW_TITLE_LENGTH];
    APTR rdwd_Texts[10];
    struct ResidentEntry *rdwd_Resident;
};

STATIC CONST struct LongFlag residentFlags[] = {
    { RTF_COLDSTART,  "RTF_COLDSTART"  },
    { RTF_SINGLETASK, "RTF_SINGLETASK" },
    { RTF_AFTERDOS,   "RTF_AFTERDOS"   },
#if defined(__MORPHOS__)
    { RTF_PPC,        "RTF_PPC"        },
#else
    { 0x08,           "< ??? >"        },
#endif
    { 0x10,           "< ??? >"        },
#if defined(__amigaos4__)
    { RTF_NATIVE,     "RTF_NATIVE"     },
    { 0x40,           "< ??? >"        },
#elif defined(__MORPHOS__)
    { 0x20,           "< ??? >"        },
    { RTF_EXTENDED,   "RTF_EXTENDED"   },
#else
    { 0x20,           "< ??? >"        },
    { 0x40,           "< ??? >"        },
#endif
    { RTF_AUTOINIT,   "RTF_AUTOINIT"   },
    { 0,              NULL }
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct ResidentsDetailWinData *rdwd = INST_DATA(cl, obj);
    struct ResidentEntry *re = rdwd->rdwd_Resident;
    struct Resident *rt = (struct Resident *)re->re_Addr;

    MySetContentsHealed(rdwd->rdwd_Texts[0], "%s", re->re_Name);
    MySetContents(rdwd->rdwd_Texts[1], ADDRESS_FORMAT, rt);
    MySetContentsHealed(rdwd->rdwd_Texts[2], "%s", nonetest(rt->rt_IdString));
    MySetContents(rdwd->rdwd_Texts[3], ADDRESS_FORMAT, rt->rt_MatchTag);
    MySetContents(rdwd->rdwd_Texts[4], "%s", GetNodeType(rt->rt_Type));
    MySetContents(rdwd->rdwd_Texts[5], "%ld", rt->rt_Pri);
    MySetContents(rdwd->rdwd_Texts[6], "%ld", rt->rt_Version);
    MySetContents(rdwd->rdwd_Texts[7], ADDRESS_FORMAT, rt->rt_EndSkip);
    set(rdwd->rdwd_Texts[8], MUIA_DisassemblerButton_Address, rt->rt_Init);
    set(rdwd->rdwd_Texts[9], MUIA_FlagsButton_Flags, rt->rt_Flags);

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtResidentsDetailTitle, re->re_Name, rdwd->rdwd_Title, sizeof(rdwd->rdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[10], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Residents",
        MUIA_Window_ID, MakeID('.','R','E','S'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtResidentName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentIdString2),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentMatchTag),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentType),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentPri2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentVersion),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentEndSkip),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtResidentInit),
                            Child, (IPTR)(texts[ 8] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtResidentFlags2),
                            Child, (IPTR)(texts[ 9] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtResidentFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)residentFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtResidentFlagsTitle,
                            End),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ResidentsDetailWinData *rdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, rdwd->rdwd_Texts, sizeof(rdwd->rdwd_Texts));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct ResidentsDetailWinData *rdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_ResidentsDetailWin_Resident:
                rdwd->rdwd_Resident =  (struct ResidentEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(ResidentsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeResidentsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ResidentsDetailWinData), ENTRY(ResidentsDetailWinDispatcher));
}

