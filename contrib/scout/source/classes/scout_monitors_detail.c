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
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#define ECS_SPECIFIC

#include "system_headers.h"

struct MonitorsDetailWinData {
    TEXT mdwd_Title[WINDOW_TITLE_LENGTH];
    APTR mdwd_Texts[29];
    struct MonitorSpec *mdwd_Monitor;
};

STATIC CONST struct LongFlag monitorSpecFlags[] = {
    { MSF_REQUEST_NTSC,      "MSF_REQUEST_NTSC"          },
    { MSF_REQUEST_PAL,       "MSF_REQUEST_PAL"           },
    { MSF_REQUEST_SPECIAL,   "MSF_REQUEST_SPECIAL"       },
    { MSF_REQUEST_A2024,     "MSF_REQUEST_A2024"         },
    { MSF_DOUBLE_SPRITES,    "MSF_DOUBLE_SPRITES"        },
    { 0x0020,                "< ??? >"                   },
    { 0x0040,                "< ??? >"                   },
    { 0x0080,                "< ??? >"                   },
    { 0x0100,                "< ??? >"                   },
    { 0x0200,                "< ??? >"                   },
    { 0x0400,                "< ??? >"                   },
    { 0x0800,                "< ??? >"                   },
    { 0x1000,                "< ??? >"                   },
    { 0x2000,                "< ??? >"                   },
    { 0x4000,                "< ??? >"                   },
    { 0x8000,                "< ??? >"                   },
    { 0,                     NULL                        }
};

STATIC CONST struct LongFlag beamcon0Flags[] = {
    { HSYNCTRUE,   "HSYNCTRUE"       },
    { VSYNCTRUE,   "VSYNCTRUE"       },
    { CSYNCTRUE,   "CSYNCTRUE"       },
    { CSBLANK,     "CSBLANK"         },
    { VARCSYNC,    "VARCSYNC"        },
    { DISPLAYPAL,  "DISPLAYPAL"      },
    { DISPLAYDUAL, "DISPLAYDUAL"     },
    { VARBEAM,     "VARBEAM"         },
    { VARHSYNC,    "VARHSYNC"        },
    { VARVSYNC,    "VARVSYNC"        },
    { CSCBLANKEN,  "CSCBLANKEN"      },
    { LOLDIS,      "LOLDIS"          },
    { VARVBLANK,   "VARVBLANK"       },
    { 0x2000,      "LPENDIS"         },
    { 0x4000,      "HARDDIS"         },
    { 0x8000,      "< ??? >"         },
    { 0,           NULL              }
};


STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct MonitorsDetailWinData *mdwd = INST_DATA(cl, obj);
    struct MonitorSpec *Mspc = mdwd->mdwd_Monitor;

    MySetContents(mdwd->mdwd_Texts[ 0], "%s", Mspc->ms_Node.xln_Name);
    MySetContents(mdwd->mdwd_Texts[ 1], ADDRESS_FORMAT, Mspc);
    set(mdwd->mdwd_Texts[ 2], MUIA_FlagsButton_Flags, Mspc->ms_Flags);
    MySetContents(mdwd->mdwd_Texts[ 3], "%lD x %lD", Mspc->ratioh, Mspc->ratiov);
    MySetContents(mdwd->mdwd_Texts[ 4], "%lD", Mspc->total_rows);
    MySetContents(mdwd->mdwd_Texts[ 5], "%lD", Mspc->total_colorclocks);
    MySetContents(mdwd->mdwd_Texts[ 6], "%lDµs", (ULONG)Mspc->total_colorclocks * 280);
    MySetContents(mdwd->mdwd_Texts[ 7], "%lD", Mspc->DeniseMaxDisplayColumn);
    set(mdwd->mdwd_Texts[ 8], MUIA_FlagsButton_Flags, Mspc->BeamCon0);
    MySetContents(mdwd->mdwd_Texts[ 9], "%lD", Mspc->min_row);
    MySetContents(mdwd->mdwd_Texts[10], "%lD", Mspc->ms_OpenCount);
    set(mdwd->mdwd_Texts[11], MUIA_DisassemblerButton_Address, Mspc->ms_transform);
    set(mdwd->mdwd_Texts[12], MUIA_DisassemblerButton_Address, Mspc->ms_translate);
    set(mdwd->mdwd_Texts[13], MUIA_DisassemblerButton_Address, Mspc->ms_scale);
    MySetContents(mdwd->mdwd_Texts[14], "%lD x %lD", Mspc->ms_xoffset, Mspc->ms_yoffset);
    MySetContents(mdwd->mdwd_Texts[15], "%lD x %lD - %lD x %lD", Mspc->ms_LegalView.MinX, Mspc->ms_LegalView.MinY, Mspc->ms_LegalView.MaxX, Mspc->ms_LegalView.MaxY);
    set(mdwd->mdwd_Texts[16], MUIA_DisassemblerButton_Address, Mspc->ms_maxoscan);
    set(mdwd->mdwd_Texts[17], MUIA_DisassemblerButton_Address, Mspc->ms_videoscan);
    MySetContents(mdwd->mdwd_Texts[18], "%lD", Mspc->DeniseMaxDisplayColumn);
    MySetContents(mdwd->mdwd_Texts[19], "$%08lx", Mspc->DisplayCompatible); // ULONG
    set(mdwd->mdwd_Texts[20], MUIA_DisassemblerButton_Address, Mspc->ms_MrgCop);
    set(mdwd->mdwd_Texts[21], MUIA_DisassemblerButton_Address, Mspc->ms_LoadView);
    if (GfxBase->LibNode.lib_Version > 38)
        set(mdwd->mdwd_Texts[22], MUIA_DisassemblerButton_Address, Mspc->ms_KillView);

    if (Mspc->ms_Special) {
        MySetContents(mdwd->mdwd_Texts[23], "$%04lx", Mspc->ms_Special->spm_Flags);
	set(mdwd->mdwd_Texts[24], MUIA_DisassemblerButton_Address, Mspc->ms_Special->do_monitor);
	MySetContents(mdwd->mdwd_Texts[25], "%lD - %lD", Mspc->ms_Special->hblank.asi_Start, Mspc->ms_Special->hblank.asi_Stop);
	MySetContents(mdwd->mdwd_Texts[26], "%lD - %lD", Mspc->ms_Special->vblank.asi_Start, Mspc->ms_Special->vblank.asi_Stop);
	MySetContents(mdwd->mdwd_Texts[27], "%lD - %lD", Mspc->ms_Special->hsync.asi_Start, Mspc->ms_Special->hsync.asi_Stop);
	MySetContents(mdwd->mdwd_Texts[28], "%lD - %lD", Mspc->ms_Special->vsync.asi_Start, Mspc->ms_Special->vsync.asi_Stop);
    } else {
        MySetContents(mdwd->mdwd_Texts[23], txtNotAvailable);
	set(mdwd->mdwd_Texts[24], MUIA_DisassemblerButton_Address, NULL);
	MySetContents(mdwd->mdwd_Texts[25], txtNotAvailable);
	MySetContents(mdwd->mdwd_Texts[26], txtNotAvailable);
	MySetContents(mdwd->mdwd_Texts[27], txtNotAvailable);
	MySetContents(mdwd->mdwd_Texts[28], txtNotAvailable);
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtMonitorsDetailTitle, Mspc->ms_Node.xln_Name, mdwd->mdwd_Title, sizeof(mdwd->mdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[36], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "ScreenMode",
        MUIA_Window_ID, MakeID('M','O','N','D'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorFlags2),
                            Child, (IPTR)(texts[ 2] = (Object *)FlagsButtonObject,
                                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                                    MUIA_FlagsButton_Title, (IPTR)txtMonitorFlags,
                                    MUIA_FlagsButton_BitArray, (IPTR)monitorSpecFlags,
                                    MUIA_FlagsButton_WindowTitle, (IPTR)txtMonitorFlagsTitle,
                                End),
                            Child, (IPTR)MyLabel2(txtMonitorRatio),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorTotalRows),
			    Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorTotalColorClocks),
                            Child, (IPTR)HGroup,
                                    Child, (IPTR)(texts[5] = MyTextObject6()),
                                    Child, (IPTR)MyLabel2(" = "),
                                    Child, (IPTR)(texts[6] = MyTextObject6()),
                            End,
                            Child, (IPTR)MyLabel2(txtMonitorDeniseMaxDisplayColumn),
                            Child, (IPTR)(texts[7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorBeamCon0_2),
                            Child, (IPTR)(texts[8] = (Object *)FlagsButtonObject,
                                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                                    MUIA_FlagsButton_Title, (IPTR)txtMonitorBeamCon0,
                                    MUIA_FlagsButton_BitArray, (IPTR)beamcon0Flags,
                                    MUIA_FlagsButton_WindowTitle, (IPTR)txtMonitorBeamCon0Title,
                                End),
                            Child, (IPTR)MyLabel2(txtMonitorMinRow),
                            Child, (IPTR)(texts[9] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtScreenModeOpenCount),
			    Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorTransform),
                            Child, (IPTR)(texts[11] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtMonitorTranslate),
                            Child, (IPTR)(texts[12] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtMonitorScale),
                            Child, (IPTR)(texts[13] = (Object *)DisassemblerButtonObject, End),
			    Child, (IPTR)MyLabel2(txtMonitorOffset),
                            Child, (IPTR)(texts[14] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorLegalView),
                            Child, (IPTR)(texts[15] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMonitorMaxoscan),
                            Child, (IPTR)(texts[16] = (Object *)DisassemblerButtonObject, End),
			    Child, (IPTR)MyLabel2(txtMonitorVideoscan),
                            Child, (IPTR)(texts[17] = (Object *)DisassemblerButtonObject, End),
			    Child, (IPTR)MyLabel2(txtMonitorDeniseMinDisplayColumn),
                            Child, (IPTR)(texts[18] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorDisplayCompatible),
                            Child, (IPTR)(texts[19] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorMrgCop),
			    Child, (IPTR)(texts[20] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtMonitorLoadView),
                            Child, (IPTR)(texts[21] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtMonitorKillView),
                            Child, (IPTR)(texts[22] = (Object *)DisassemblerButtonObject, End),
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtMonitorSpecial),
                            Child, (IPTR)ColGroup(2),
                                Child, (IPTR)MyLabel2(txtSpecialMonitorFlags),
				Child, (IPTR)(texts[23] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtSpecialMonitorDoMonitor),
                                Child, (IPTR)(texts[24] = (Object *)DisassemblerButtonObject, End),
				Child, (IPTR)MyLabel2(txtSpecialMonitorHBlank),
				Child, (IPTR)(texts[25] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtSpecialMonitorVBlank),
				Child, (IPTR)(texts[26] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtSpecialMonitorHSync),
				Child, (IPTR)(texts[27] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtSpecialMonitorVSync),
				Child, (IPTR)(texts[28] = MyTextObject6()),
                            End,
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MonitorsDetailWinData *mdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, mdwd->mdwd_Texts, sizeof(mdwd->mdwd_Texts));

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
    struct MonitorsDetailWinData *mdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_MonitorsDetailWin_MonitorSpec:
                mdwd->mdwd_Monitor = (struct MonitorSpec *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(MonitorsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMonitorsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MonitorsDetailWinData), ENTRY(MonitorsDetailWinDispatcher));
}

