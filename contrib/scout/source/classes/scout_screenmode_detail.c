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

struct ScreenModesDetailWinData {
    TEXT smdwd_Title[WINDOW_TITLE_LENGTH];
    APTR smdwd_Texts[36];
    struct ScreenModeEntry *smdwd_ScreenMode;
};

STATIC CONST struct LongFlag propertyFlags[] = {
    { DIPF_IS_LACE,              "DIPF_IS_LACE"              },
    { DIPF_IS_DUALPF,            "DIPF_IS_DUALPF"            },
    { DIPF_IS_PF2PRI,            "DIPF_IS_PF2PRI"            },
    { DIPF_IS_HAM,               "DIPF_IS_HAM"               },
    { DIPF_IS_ECS,               "DIPF_IS_ECS"               },
    { DIPF_IS_PAL,               "DIPF_IS_PAL"               },
    { DIPF_IS_SPRITES,           "DIPF_IS_SPRITES"           },
    { DIPF_IS_GENLOCK,           "DIPF_IS_GENLOCK"           },
    { DIPF_IS_WB,                "DIPF_IS_WB"                },
    { DIPF_IS_DRAGGABLE,         "DIPF_IS_DRAGGABLE"         },
    { DIPF_IS_PANELLED,          "DIPF_IS_PANELLED"          },
    { DIPF_IS_BEAMSYNC,          "DIPF_IS_BEAMSYNC"          },
    { DIPF_IS_EXTRAHALFBRITE,    "DIPF_IS_EXTRAHALFBRITE"    },
    { DIPF_IS_SPRITES_ATT,       "DIPF_IS_SPRITES_ATT"       },
    { DIPF_IS_SPRITES_CHNG_RES,  "DIPF_IS_SPRITES_CHNG_RES"  },
    { DIPF_IS_SPRITES_BORDER,    "DIPF_IS_SPRITES_BORDER"    },
    { DIPF_IS_AA,                "DIPF_IS_AA"                },
    { DIPF_IS_SCANDBL,           "DIPF_IS_SCANDBL"           },
    { DIPF_IS_SPRITES_CHNG_BASE, "DIPF_IS_SPRITES_CHNG_BASE" },
    { DIPF_IS_SPRITES_CHNG_PRI,  "DIPF_IS_SPRITES_CHNG_PRI"  },
    { DIPF_IS_DBUFFER,           "DIPF_IS_DBUFFER"           },
    { DIPF_IS_PROGBEAM,          "DIPF_IS_PROGBEAM"          },
    { 0x00400000,                "< ??? >"                   },
    { 0x00800000,                "< ??? >"                   },
    { 0x01000000,                "< ??? >"                   },
    { 0x02000000,                "< ??? >"                   },
    { 0x04000000,                "< ??? >"                   },
    { 0x08000000,                "< ??? >"                   },
    { 0x10000000,                "< ??? >"                   },
    { 0x20000000,                "< ??? >"                   },
    { 0x40000000,                "< ??? >"                   },
    { DIPF_IS_FOREIGN,           "DIPF_IS_FOREIGN"           },
    { 0,                         NULL }
};

STATIC CONST struct LongFlag notAvailFlags[] = {
    { DI_AVAIL_NOCHIPS,        "DI_AVAIL_NOCHIPS"        },
    { DI_AVAIL_NOMONITOR,      "DI_AVAIL_NOMONITOR"      },
    { DI_AVAIL_NOTWITHGENLOCK, "DI_AVAIL_NOTWITHGENLOCK" },
    { 0x0008,                  "< ??? >"                 },
    { 0x0010,                  "< ??? >"                 },
    { 0x0020,                  "< ??? >"                 },
    { 0x0040,                  "< ??? >"                 },
    { 0x0080,                  "< ??? >"                 },
    { 0x0100,                  "< ??? >"                 },
    { 0x0200,                  "< ??? >"                 },
    { 0x0400,                  "< ??? >"                 },
    { 0x0800,                  "< ??? >"                 },
    { 0x1000,                  "< ??? >"                 },
    { 0x2000,                  "< ??? >"                 },
    { 0x4000,                  "< ??? >"                 },
    { 0x8000,                  "< ??? >"                 },
    { 0,                       NULL }
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct ScreenModesDetailWinData *smdwd = INST_DATA(cl, obj);
    struct ScreenModeEntry *sme = smdwd->smdwd_ScreenMode;
    struct ScreenModeData *smd;

    if ((smd = tbAllocVecPooled(globalPool, sizeof(*smd))) != NULL) {
        STRPTR tmp;
        ULONG fh = 0, fv = 0;
        TEXT colors[32];

        GetDisplayInfoData(NULL, (UBYTE *)&smd->display, sizeof(struct DisplayInfo)  , DTAG_DISP, sme->sme_ModeID);
        GetDisplayInfoData(NULL, (UBYTE *)&smd->dimension, sizeof(struct DimensionInfo), DTAG_DIMS, sme->sme_ModeID);
        GetDisplayInfoData(NULL, (UBYTE *)&smd->monitor, sizeof(struct MonitorInfo)  , DTAG_MNTR, sme->sme_ModeID);

        if (GetDisplayInfoData(NULL, (UBYTE *)&smd->name, sizeof(struct NameInfo), DTAG_NAME, sme->sme_ModeID)) {
            MySetContents(smdwd->smdwd_Texts[ 0], "%s", smd->name.Name);
        } else {
            MySetContents(smdwd->smdwd_Texts[ 0], MUIX_PH "%s" MUIX_PT, sme->sme_Name);
        }
        MySetContents(smdwd->smdwd_Texts[ 1], "$%08lx", sme->sme_ModeID); // ULONG

	set(smdwd->smdwd_Texts[ 2], MUIA_MonitorButton_MonitorSpec, smd->monitor.Mspc);
        MySetContents(smdwd->smdwd_Texts[ 5], "%lD x %lD", smd->monitor.ViewPosition.x, smd->monitor.ViewPosition.y);
        MySetContents(smdwd->smdwd_Texts[ 6], "%lD x %lD", smd->monitor.ViewResolution.x, smd->monitor.ViewResolution.y);
        MySetContents(smdwd->smdwd_Texts[ 7], "%lD x %lD - %lD x %lD", smd->monitor.ViewPositionRange.MinX, smd->monitor.ViewPositionRange.MinY, smd->monitor.ViewPositionRange.MaxX, smd->monitor.ViewPositionRange.MaxY);
        MySetContents(smdwd->smdwd_Texts[ 8], "%lD", smd->monitor.TotalRows);
        MySetContents(smdwd->smdwd_Texts[ 9], "%lD", smd->monitor.TotalColorClocks);
        MySetContents(smdwd->smdwd_Texts[10], "%lDµs", (ULONG)smd->monitor.TotalColorClocks * 280);

        if (smd->monitor.TotalColorClocks && smd->monitor.TotalRows)
        {
            fh = 1000000L / (smd->monitor.TotalColorClocks * 28);
            fv = (fh * 100) / smd->monitor.TotalRows;
        }
        MySetContents(smdwd->smdwd_Texts[11], "%lDkHz", fh / 10);
        MySetContents(smdwd->smdwd_Texts[12], "%lDHz", fv);
        MySetContents(smdwd->smdwd_Texts[13], "%lD", smd->monitor.MinRow);

        switch(smd->monitor.Compatibility) {
            case MCOMPAT_MIXED: tmp = msgScreenModeCompatMixed; break;
            case MCOMPAT_SELF: tmp = msgScreenModeCompatSelf; break;
            case MCOMPAT_NOBODY: tmp = msgScreenModeCompatNobody; break;
            default: tmp = (STRPTR)MUIX_PH "< ??? >" MUIX_PT;
        }
        MySetContents(smdwd->smdwd_Texts[14], " %lD ", smd->monitor.Compatibility);
        MySetContents(smdwd->smdwd_Texts[15], "%s", tmp);
        MySetContents(smdwd->smdwd_Texts[16], "%lD x %lD", smd->monitor.MouseTicks.x, smd->monitor.MouseTicks.y);
        MySetContents(smdwd->smdwd_Texts[17], "%lD x %lD", smd->monitor.DefaultViewPosition.x, smd->monitor.DefaultViewPosition.y);
        MySetContents(smdwd->smdwd_Texts[18], "$%08lx", smd->monitor.PreferredModeID); // ULONG

        if (FLAG_IS_SET(smd->display.PropertyFlags, DIPF_IS_DUALPF)) {
           _snprintf(colors, sizeof(colors), "2x%ld",1 << (smd->dimension.MaxDepth >> 1));
        } else if (FLAG_IS_SET(smd->display.PropertyFlags, DIPF_IS_HAM)) {
           _snprintf(colors, sizeof(colors), "[%ld]",1 << (3 * (smd->dimension.MaxDepth - 2)));
        } else if (smd->dimension.MaxDepth > 24) {
           stccpy(colors, "> 16.8M", sizeof(colors));
        } else {
           _snprintf(colors, sizeof(colors), "%lD", 1 << smd->dimension.MaxDepth);
        }
        MySetContents(smdwd->smdwd_Texts[19], " %lD ", smd->dimension.MaxDepth);
        MySetContents(smdwd->smdwd_Texts[20], txtScreenModeColorsFormat, colors);
        MySetContents(smdwd->smdwd_Texts[21], "%lD x %lD", smd->dimension.MinRasterWidth, smd->dimension.MinRasterHeight);
        MySetContents(smdwd->smdwd_Texts[22], "%lD x %lD", smd->dimension.MaxRasterWidth, smd->dimension.MaxRasterHeight);
        MySetContents(smdwd->smdwd_Texts[23], "%lD x %lD (%lD x %lD)", smd->dimension.Nominal.MaxX - smd->dimension.Nominal.MinX + 1, smd->dimension.Nominal.MaxY - smd->dimension.Nominal.MinY + 1, smd->dimension.Nominal.MinX, smd->dimension.Nominal.MinY);
        MySetContents(smdwd->smdwd_Texts[24], "%lD x %lD (%lD x %lD)", smd->dimension.MaxOScan.MaxX - smd->dimension.MaxOScan.MinX + 1, smd->dimension.MaxOScan.MaxY - smd->dimension.MaxOScan.MinY + 1, smd->dimension.MaxOScan.MinX, smd->dimension.MaxOScan.MinY);
        MySetContents(smdwd->smdwd_Texts[25], "%lD x %lD (%lD x %lD)", smd->dimension.VideoOScan.MaxX - smd->dimension.VideoOScan.MinX + 1, smd->dimension.VideoOScan.MaxY - smd->dimension.VideoOScan.MinY + 1, smd->dimension.VideoOScan.MinX, smd->dimension.VideoOScan.MinY);
        MySetContents(smdwd->smdwd_Texts[26], "%lD x %lD (%lD x %lD)", smd->dimension.TxtOScan.MaxX - smd->dimension.TxtOScan.MinX + 1, smd->dimension.TxtOScan.MaxY - smd->dimension.TxtOScan.MinY + 1, smd->dimension.TxtOScan.MinX, smd->dimension.TxtOScan.MinY);
        MySetContents(smdwd->smdwd_Texts[27], "%lD x %lD (%lD x %lD)", smd->dimension.StdOScan.MaxX - smd->dimension.StdOScan.MinX + 1, smd->dimension.StdOScan.MaxY - smd->dimension.StdOScan.MinY + 1, smd->dimension.StdOScan.MinX, smd->dimension.StdOScan.MinY);

        set(smdwd->smdwd_Texts[28], MUIA_FlagsButton_Flags, smd->display.NotAvailable);
        set(smdwd->smdwd_Texts[29], MUIA_FlagsButton_Flags, smd->display.PropertyFlags);
        MySetContents(smdwd->smdwd_Texts[30], "%lD x %lD", smd->display.Resolution.x, smd->display.Resolution.y);
        MySetContents(smdwd->smdwd_Texts[31], "%lDns", smd->display.PixelSpeed);
        MySetContents(smdwd->smdwd_Texts[32], "%lD", smd->display.NumStdSprites);
        MySetContents(smdwd->smdwd_Texts[33], "%lD", smd->display.PaletteRange);
        MySetContents(smdwd->smdwd_Texts[34], "%lD x %lD", smd->display.SpriteResolution.x, smd->display.SpriteResolution.y);
        MySetContents(smdwd->smdwd_Texts[35], "%lD:%lD:%lD", smd->display.RedBits, smd->display.GreenBits, smd->display.BlueBits);

        tbFreeVecPooled(globalPool, smd);
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtScreenModesDetailTitle, sme->sme_Name, smdwd->smdwd_Title, sizeof(smdwd->smdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[36], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "ScreenMode",
        MUIA_Window_ID, MakeID('.','S','M','D'),
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
                            Child, (IPTR)MyLabel2(txtScreenModeID2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtScreenModeMonitorInfo),
                            Child, (IPTR)ColGroup(2),
                                Child, (IPTR)MyLabel2(txtScreenModeMonitorSpec),
                                Child, (IPTR)(texts[ 2] = (Object *)MonitorButtonObject, End),
                                Child, (IPTR)MyLabel2(txtScreenModeViewPosition),
                                Child, (IPTR)(texts[ 5] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeViewResolution),
                                Child, (IPTR)(texts[ 6] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeViewPositionRange),
                                Child, (IPTR)(texts[ 7] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeTotalRows),
                                Child, (IPTR)(texts[ 8] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeTotalClockColors),
                                Child, (IPTR)HGroup,
                                    Child, (IPTR)(texts[ 9] = MyTextObject6()),
                                    Child, (IPTR)MyLabel2(" = "),
                                    Child, (IPTR)(texts[10] = MyTextObject6()),
                                End,
                                Child, (IPTR)MyLabel2(txtScreenModeHFrequency),
                                Child, (IPTR)(texts[11] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeVFrequency),
                                Child, (IPTR)(texts[12] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeMinRow),
                                Child, (IPTR)(texts[13] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeCompatibility),
                                Child, (IPTR)HGroup,
                                    Child, (IPTR)(texts[14] = MyTextObject2()),
                                    Child, (IPTR)MyLabel2(" = "),
                                    Child, (IPTR)(texts[15] = MyTextObject6()),
                                End,
                                Child, (IPTR)MyLabel2(txtScreenModeMouseTicks),
                                Child, (IPTR)(texts[16] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeDefaultViewPosition),
                                Child, (IPTR)(texts[17] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModePreferredModeID),
                                Child, (IPTR)(texts[18] = MyTextObject6()),
                            End,
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtScreenModeDimensionInfo),
                            Child, (IPTR)ColGroup(2),
                                Child, (IPTR)MyLabel2(txtScreenModeMaxDepth),
                                Child, (IPTR)HGroup,
                                    Child, (IPTR)(texts[19] = MyTextObject2()),
                                    Child, (IPTR)MyLabel2(" = "),
                                    Child, (IPTR)(texts[20] = MyTextObject6()),
                                End,
                                Child, (IPTR)MyLabel2(txtScreenModeMinRaster),
                                Child, (IPTR)(texts[21] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeMaxRaster),
                                Child, (IPTR)(texts[22] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeNominal),
                                Child, (IPTR)(texts[23] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeMaxOScan),
                                Child, (IPTR)(texts[24] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeVideoOScan),
                                Child, (IPTR)(texts[25] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeTxtOScan),
                                Child, (IPTR)(texts[26] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeStdOScan),
                                Child, (IPTR)(texts[27] = MyTextObject6()),
                            End,
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtScreenModeDisplayInfo),
                            Child, (IPTR)ColGroup(2),
                                Child, (IPTR)MyLabel2(txtScreenModeNotAvailable2),
                                Child, (IPTR)(texts[28] = (Object *)FlagsButtonObject,
                                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                                    MUIA_FlagsButton_Title, (IPTR)txtScreenModeNotAvailable,
                                    MUIA_FlagsButton_BitArray, (IPTR)notAvailFlags,
                                    MUIA_FlagsButton_WindowTitle, (IPTR)txtScreenModeNotAvailableTitle,
                                End),
                                Child, (IPTR)MyLabel2(txtScreenModePropertyFlags2),
                                Child, (IPTR)(texts[29] = (Object *)FlagsButtonObject,
                                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                                    MUIA_FlagsButton_Title, (IPTR)txtScreenModePropertyFlags,
                                    MUIA_FlagsButton_BitArray, (IPTR)propertyFlags,
                                    MUIA_FlagsButton_WindowTitle, (IPTR)txtScreenModePropertyFlagsTitle,
                                End),
                                Child, (IPTR)MyLabel2(txtScreenModeResolution),
                                Child, (IPTR)(texts[30] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModePixelSpeed),
                                Child, (IPTR)(texts[31] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeNumStdSprites),
                                Child, (IPTR)(texts[32] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModePaletteRange),
                                Child, (IPTR)(texts[33] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeSpriteResolution),
                                Child, (IPTR)(texts[34] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(txtScreenModeRGBBits),
                                Child, (IPTR)(texts[35] = MyTextObject6()),
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
        struct ScreenModesDetailWinData *smdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, smdwd->smdwd_Texts, sizeof(smdwd->smdwd_Texts));

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
    struct ScreenModesDetailWinData *smdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_ScreenModesDetailWin_ScreenMode:
                smdwd->smdwd_ScreenMode = (struct ScreenModeEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(ScreenModesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeScreenModesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ScreenModesDetailWinData), ENTRY(ScreenModesDetailWinDispatcher));
}

