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

struct WindowsDetailWinData {
    TEXT wdwd_Title[WINDOW_TITLE_LENGTH];
    APTR wdwd_MainGroup;
    struct WindowEntry *wdwd_Object;
};

STATIC CONST struct LongFlag screenFlags1[] = {
    { SHOWTITLE,    "SHOWTITLE"    },
    { BEEPING,      "BEEPING"      },
    { CUSTOMBITMAP, "CUSTOMBITMAP" },
    { SCREENBEHIND, "SCREENBEHIND" },
    { SCREENQUIET,  "SCREENQUIET"  },
    { SCREENHIRES,  "SCREENHIRES"  },
    { PENSHARED,    "PENSHARED"    },
    { NS_EXTENDED,  "NS_EXTENDED"  },
    { 0x2000,       "< ??? >"      },
    { AUTOSCROLL,   "AUTOSCROLL"   },
    { 0x8000,       "< ??? >"      },
    { 0,            NULL }
};

STATIC CONST struct MaskedLongFlag screenFlags2[] = {
    { WBENCHSCREEN, SCREENTYPE, "WBENCHSCREEN" },
    { PUBLICSCREEN, SCREENTYPE, "PUBLICSCREEN" },
    { CUSTOMSCREEN, SCREENTYPE, "CUSTOMSCREEN" },
    { 0,            0,          NULL }
};

STATIC CONST struct LongFlag screenBitmapFlags[] = {
    { BMF_CLEAR,       "BMF_CLEAR"       },
    { BMF_DISPLAYABLE, "BMF_DISPLAYABLE" },
    { BMF_INTERLEAVED, "BMF_INTERLEAVED" },
    { BMF_STANDARD,    "BMF_STANDARD"    },
    { BMF_MINPLANES,   "BMF_MINPLANES"   },
#if defined(__amigaos4__)
    { BMF_HIJACKED,    "BMF_HIJACKED"    },
    { BMF_RTGTAGS,     "BMF_RTGTAGS"     },
    { BMF_RTGCHECK,    "BMF_RTGCHECK"    },
    { BMF_FRIENDISTAG, "BMF_FRIENDISTAG" },
    { BMF_INVALID,     "BMF_INVALID"     },
#elif defined(__MORPHOS__)
    { BMF_ROOTMAP,     "BMF_ROOTMAP"     },
    { BMF_SPECIALFMT,  "BMF_SPECIALFMT"  },
#else
    { BMF_SPECIALFMT,  "BMF_SPECIALFMT"  },
#endif
    { 0,               NULL }
};

STATIC CONST struct LongFlag pubScreenFlags[] = {
    { PSNF_PRIVATE, "PSNF_PRIVATE" },
    { 0x0002,       "< ??? >"      },
    { 0x0004,       "< ??? >"      },
    { 0x0008,       "< ??? >"      },
    { 0x0010,       "< ??? >"      },
    { 0x0020,       "< ??? >"      },
    { 0x0040,       "< ??? >"      },
    { 0x0080,       "< ??? >"      },
    { 0x0100,       "< ??? >"      },
    { 0x0200,       "< ??? >"      },
    { 0x0400,       "< ??? >"      },
    { 0x0800,       "< ??? >"      },
    { 0x1000,       "< ??? >"      },
    { 0x2000,       "< ??? >"      },
    { 0x4000,       "< ??? >"      },
    { 0x8000,       "< ??? >"      },
    { 0,            NULL }
};

STATIC CONST struct LongFlag windowFlags1[] = {
    { WFLG_SIZEGADGET,    "WFLG_SIZEGADGET"    },
    { WFLG_DRAGBAR,       "WFLG_DRAGBAR"       },
    { WFLG_DEPTHGADGET,   "WFLG_DEPTHGADGET"   },
    { WFLG_CLOSEGADGET,   "WFLG_CLOSEGADGET"   },
    { WFLG_SIZEBRIGHT,    "WFLG_SIZEBRIGHT"    },
    { WFLG_SIZEBBOTTOM,   "WFLG_SIZEBBOTTOM"   },
    { WFLG_BACKDROP,      "WFLG_BACKDROP"      },
    { WFLG_REPORTMOUSE,   "WFLG_REPORTMOUSE"   },
    { WFLG_GIMMEZEROZERO, "WFLG_GIMMEZEROZERO" },
    { WFLG_BORDERLESS,    "WFLG_BORDERLESS"    },
    { WFLG_ACTIVATE,      "WFLG_ACTIVATE"      },
    { WFLG_WINDOWACTIVE,  "WFLG_WINDOWACTIVE"  },
    { WFLG_INREQUEST,     "WFLG_INREQUEST"     },
    { WFLG_MENUSTATE,     "WFLG_MENUSTATE"     },
    { WFLG_RMBTRAP,       "WFLG_RMBTRAP"       },
    { WFLG_NOCAREREFRESH, "WFLG_NOCAREREFRESH" },
    { WFLG_NEWLOOKMENUS,  "WFLG_NEWLOOKMENUS"  },
    { WFLG_NW_EXTENDED,   "WFLG_NW_EXTENDED"   },
    { WFLG_WINDOWREFRESH, "WFLG_WINDOWREFRESH" },
    { WFLG_WBENCHWINDOW,  "WFLG_WBENCHWINDOW"  },
    { WFLG_WINDOWTICKED,  "WFLG_WINDOWTICKED"  },
    { WFLG_VISITOR,       "WFLG_VISITOR"       },
    { WFLG_ZOOMED,        "WFLG_ZOOMED"        },
    { WFLG_HASZOOM,       "WFLG_HASZOOM"       },
    { 0x40000000,         "< ??? >"            },
    { 0x80000000,         "< ??? >"            },
    { 0,                  NULL }
};

STATIC CONST struct MaskedLongFlag windowFlags2[] = {
   { WFLG_SMART_REFRESH,  WFLG_REFRESHBITS, "WFLG_SMART_REFRESH"  },
   { WFLG_SIMPLE_REFRESH, WFLG_REFRESHBITS, "WFLG_SIMPLE_REFRESH" },
   { WFLG_SUPER_BITMAP,   WFLG_REFRESHBITS, "WFLG_SUPER_BITMAP"   },
   { WFLG_OTHER_REFRESH,  WFLG_REFRESHBITS, "WFLG_OTHER_REFRESH"  },
   { 0,                   0,                NULL }
};

STATIC CONST struct LongFlag idcmpFlags[] = {
   { IDCMP_SIZEVERIFY,       "IDCMP_SIZEVERIFY"       },
   { IDCMP_NEWSIZE,          "IDCMP_NEWSIZE"          },
   { IDCMP_REFRESHWINDOW,    "IDCMP_REFRESHWINDOW"    },
   { IDCMP_MOUSEBUTTONS,     "IDCMP_MOUSEBUTTONS"     },
   { IDCMP_MOUSEMOVE,        "IDCMP_MOUSEMOVE"        },
   { IDCMP_GADGETDOWN,       "IDCMP_GADGETDOWN"       },
   { IDCMP_GADGETUP,         "IDCMP_GADGETUP"         },
   { IDCMP_REQSET,           "IDCMP_REQSET"           },
   { IDCMP_MENUPICK,         "IDCMP_MENUPICK"         },
   { IDCMP_CLOSEWINDOW,      "IDCMP_CLOSEWINDOW"      },
   { IDCMP_RAWKEY,           "IDCMP_RAWKEY"           },
   { IDCMP_REQVERIFY,        "IDCMP_REQVERIFY"        },
   { IDCMP_REQCLEAR,         "IDCMP_REQCLEAR"         },
   { IDCMP_MENUVERIFY,       "IDCMP_MENUVERIFY"       },
   { IDCMP_NEWPREFS,         "IDCMP_NEWPREFS"         },
   { IDCMP_DISKINSERTED,     "IDCMP_DISKINSERTED"     },
   { IDCMP_DISKREMOVED,      "IDCMP_DISKREMOVED"      },
   { IDCMP_WBENCHMESSAGE,    "IDCMP_WBENCHMESSAGE"    },
   { IDCMP_ACTIVEWINDOW,     "IDCMP_ACTIVEWINDOW"     },
   { IDCMP_INACTIVEWINDOW,   "IDCMP_INACTIVEWINDOW"   },
   { IDCMP_DELTAMOVE,        "IDCMP_DELTAMOVE"        },
   { IDCMP_VANILLAKEY,       "IDCMP_VANILLAKEY"       },
   { IDCMP_INTUITICKS,       "IDCMP_INTUITICKS"       },
   { IDCMP_IDCMPUPDATE,      "IDCMP_IDCMPUPDATE"      },
   { IDCMP_MENUHELP,         "IDCMP_MENUHELP"         },
   { IDCMP_CHANGEWINDOW,     "IDCMP_CHANGEWINDOW"     },
   { IDCMP_GADGETHELP,       "IDCMP_GADGETHELP"       },
#if defined(__amigaos4__)
   { IDCMP_EXTENDEDMOUSE,    "IDCMP_EXTENDEDMOUSE"    },
   { IDCMP_EXTENDEDKEYBOARD, "IDCMD_EXTENDEDKEYBOARD" },
   { IDCMP_RESERVED1,        "IDCMP_RESERVED1"        },
   { IDCMP_RESERVED2,        "IDCMP_RESERVED2"        },
#elif defined(__MORPHOS__)
   { IDCMP_MOUSEHOVER,       "IDCMP_MOUSEHOVER"       },
#endif
   { IDCMP_LONELYMESSAGE,    "IDCMP_LONELYMESSAGE"    },
   { 0,                      NULL }
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct WindowsDetailWinData *wdwd = INST_DATA(cl, obj);
    struct WindowEntry *we = wdwd->wdwd_Object;

    if (stricmp(we->we_Type, "SCREEN") == 0) {
        APTR texts[41], subgroup;
        struct Screen *scr = (struct Screen *)we->we_Addr;
        ULONG id, depth;
        ULONG penCount, *pens;
        struct List *publist;

        subgroup = (Object *)ColGroup(2),
            Child, (IPTR)MyLabel2(txtWindowTitle2),
            Child, (IPTR)(texts[ 0] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtAddress2),
            Child, (IPTR)(texts[ 1] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowNextScreen),
            Child, (IPTR)(texts[ 2] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFirstWindow),
            Child, (IPTR)(texts[ 3] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowLeftEdge),
            Child, (IPTR)(texts[ 4] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowTopEdge),
            Child, (IPTR)(texts[ 5] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWidth),
            Child, (IPTR)(texts[ 6] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowHeight),
            Child, (IPTR)(texts[ 7] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMouseY),
            Child, (IPTR)(texts[ 8] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMouseX),
            Child, (IPTR)(texts[ 9] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFlags2),
            Child, (IPTR)(texts[10] = (Object *)FlagsButtonObject,
                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                MUIA_FlagsButton_Title, (IPTR)txtWindowFlags,
                MUIA_FlagsButton_BitArray, (IPTR)screenFlags1,
                MUIA_FlagsButton_MaskArray, (IPTR)screenFlags2,
                MUIA_FlagsButton_WindowTitle, (IPTR)txtWindowScreenFlagsTitle,
            End),
            Child, (IPTR)MyLabel2(txtWindowDefaultTitle),
            Child, (IPTR)(texts[11] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBarHeight),
            Child, (IPTR)(texts[12] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBarVBorder),
            Child, (IPTR)(texts[13] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBarHBorder),
            Child, (IPTR)(texts[14] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMenuVBorder),
            Child, (IPTR)(texts[15] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMenuHBorder),
            Child, (IPTR)(texts[16] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWBorTop),
            Child, (IPTR)(texts[17] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWBorLeft),
            Child, (IPTR)(texts[18] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWBorRight),
            Child, (IPTR)(texts[19] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWBorBottom),
            Child, (IPTR)(texts[20] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowScreenMode),
            Child, (IPTR)(texts[21] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowDepth),
            Child, (IPTR)HGroup,
                Child, (IPTR)(texts[22] = MyTextObject2()),
                Child, (IPTR)MyLabel2(" = "),
                Child, (IPTR)(texts[23] = MyTextObject6()),
            End,
            Child, (IPTR)MyLabel2(txtWindowFont),
            Child, (IPTR)(texts[24] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowViewPort),
            Child, (IPTR)(texts[25] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowRastPort),
            Child, (IPTR)(texts[26] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBitmap),
            Child, (IPTR)(texts[27] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBitmapWidth),
            Child, (IPTR)(texts[28] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBitmapHeight),
            Child, (IPTR)(texts[29] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBitmapDepth),
            Child, (IPTR)(texts[30] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBitmapFlags2),
            Child, (IPTR)(texts[31] = (Object *)FlagsButtonObject,
                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                MUIA_FlagsButton_Title, (IPTR)txtWindowBitmapFlags,
                MUIA_FlagsButton_BitArray, (IPTR)screenBitmapFlags,
                MUIA_FlagsButton_WindowTitle, (IPTR)txtWindowScreenBitmapFlagsTitle,
            End),
            Child, (IPTR)MyLabel2(txtWindowLayerInfo),
            Child, (IPTR)(texts[32] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFirstGadget),
            Child, (IPTR)(texts[33] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowDetailPen),
            Child, (IPTR)(texts[34] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBlockPen),
            Child, (IPTR)(texts[35] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowSaveColor0),
            Child, (IPTR)(texts[36] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBarLayer),
            Child, (IPTR)(texts[37] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowExtData),
            Child, (IPTR)(texts[38] = (Object *)DisassemblerButtonObject,
                MUIA_DisassemblerButton_ForceHexDump, TRUE,
            End),
            Child, (IPTR)MyLabel2(txtWindowUserData),
            Child, (IPTR)(texts[39] = (Object *)DisassemblerButtonObject,
                MUIA_DisassemblerButton_ForceHexDump, TRUE,
            End),
            Child, (IPTR)MyLabel2(txtWindowPens),
            Child, (IPTR)(texts[40] = MyTextObject6()),
        End;

        DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(wdwd->wdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_ExitChange);

        MySetContents(texts[ 0], "%s", nonetest((STRPTR)scr->Title));
        MySetContents(texts[ 1], "$%08lx", scr);
        MySetContents(texts[ 2], "$%08lx", scr->NextScreen);
        MySetContents(texts[ 3], "$%08lx", scr->FirstWindow);
        MySetContents(texts[ 4], "%lD", scr->LeftEdge);
        MySetContents(texts[ 5], "%lD", scr->TopEdge);
        MySetContents(texts[ 6], "%lD", scr->Width);
        MySetContents(texts[ 7], "%lD", scr->Height);
        MySetContents(texts[ 8], "%lD", scr->MouseY);
        MySetContents(texts[ 9], "%lD", scr->MouseX);
        set(texts[10], MUIA_FlagsButton_Flags, scr->Flags);
        MySetContents(texts[11], "%s", nonetest((STRPTR)scr->DefaultTitle));
        MySetContents(texts[12], "%lD", scr->BarHeight);
        MySetContents(texts[13], "%lD", scr->BarVBorder);
        MySetContents(texts[14], "%lD", scr->BarHBorder);
        MySetContents(texts[15], "%lD", scr->MenuVBorder);
        MySetContents(texts[16], "%lD", scr->MenuHBorder);
        MySetContents(texts[17], "%lD", scr->WBorTop);
        MySetContents(texts[18], "%lD", scr->WBorLeft);
        MySetContents(texts[19], "%lD", scr->WBorRight);
        MySetContents(texts[20], "%lD", scr->WBorBottom);

        depth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);
        penCount = MIN(256, (1L << depth));

        if ((id = GetVPModeID(&scr->ViewPort)) != 0) {
            STRPTR tmp;

            if ((tmp = tbAllocVecPooled(globalPool, PATH_LENGTH)) != NULL) {
                APTR handle;

                if ((handle = FindDisplayInfo(id)) != NULL) {
                    if (GetDisplayInfoData(handle, tmp, sizeof(struct NameInfo), DTAG_NAME, 0)) {
                        MySetContents(texts[21], "%s ($%08lx)", ((struct NameInfo *)tmp)->Name, id);
                    } else {
                        struct MonitorSpec *mspc;

                        if ((mspc = OpenMonitor(NULL, id)) != NULL) {
                            ULONG i = 0;
                            struct DimensionInfo diminfo;

                            NameCopy(tmp, mspc->ms_Node.xln_Name);

                            GetDisplayInfoData(handle, (UBYTE *)&diminfo, sizeof(struct DimensionInfo), DTAG_DIMS, 0);
                            _snprintf(&tmp[strlen(tmp)], PATH_LENGTH - i, "%ld x %ld ($%08lx)", diminfo.Nominal.MaxX - diminfo.Nominal.MinX + 1, diminfo.Nominal.MaxY - diminfo.Nominal.MinY + 1, id);
                            MySetContents(texts[21], "%s", tmp);
                            CloseMonitor(mspc);
                        }
                    }
                }

                tbFreeVecPooled(globalPool, tmp);
            }
        }

        MySetContents(texts[22], " %2lD ", depth);
        MySetContents(texts[23], txtWindowColors, (1L << (depth == 32 ? 24 : depth)));
        MySetContents(texts[24], "%s/%ld", scr->Font->ta_Name, scr->Font->ta_YSize);
        MySetContents(texts[25], "$%08lx", &scr->ViewPort);
        MySetContents(texts[26], "$%08lx", &scr->RastPort);
        MySetContents(texts[27], "$%08lx", scr->RastPort.BitMap);
        MySetContents(texts[28], "%lU", GetBitMapAttr(scr->RastPort.BitMap, BMA_WIDTH));
        MySetContents(texts[29], "%lU", GetBitMapAttr(scr->RastPort.BitMap, BMA_HEIGHT));
        MySetContents(texts[30], "%lU", GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH));
        set(texts[31], MUIA_FlagsButton_Flags, GetBitMapAttr(scr->RastPort.BitMap, BMA_FLAGS));
        MySetContents(texts[32], "$%08lx", &scr->LayerInfo);
        MySetContents(texts[33], "$%08lx", scr->FirstGadget);
        MySetContents(texts[34], "%lD", scr->DetailPen);
        MySetContents(texts[35], "%lD", scr->BlockPen);
        MySetContents(texts[36], "%lD", scr->SaveColor0);
        MySetContents(texts[37], "$%08lx", scr->BarLayer);
        set(texts[38], MUIA_DisassemblerButton_Address, scr->ExtData);
        set(texts[39], MUIA_DisassemblerButton_Address, scr->UserData);

        if (FLAG_IS_SET(scr->Flags, PENSHARED)) {
            if ((pens = tbAllocVecPooled(globalPool, penCount * sizeof(ULONG))) != NULL) {
                ULONG pen;
                ULONG cnt = 0;

                // forbid multitasking, so we don't stop anybody else from obtaining free pens
                Forbid();

                for (pen = 0; pen < penCount; pen++) {
                    pens[pen] = ObtainPen(scr->ViewPort.ColorMap, -1, pen, pen, pen, PEN_EXCLUSIVE | PEN_NO_SETCOLOR);
                    if (pens[pen] == -1) break;
                    cnt++;
                }
                for (pen = 0; pen < cnt; pen++) {
                    ReleasePen(scr->ViewPort.ColorMap, pens[pen]);
                }

                Permit();

                MySetContents(texts[40], txtWindowSharedPens, penCount, penCount - cnt, cnt);

                tbFreeVecPooled(globalPool, pens);
            }
        } else {
            MySetContents(texts[40], txtWindowSharedPens, 0, penCount, 0);
        }

        if ((publist = LockPubScreenList()) != NULL) {
            struct PubScreenNode *psn;

            ITERATE_LIST(publist, struct PubScreenNode *, psn) {
                if (psn->psn_Screen == scr) {
                    subgroup = (Object *)VGroup,
                        GroupFrameT(txtWindowPubScreenSpecific),
                        Child, (IPTR)(subgroup = (Object *)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtWindowPubScreenFlags2),
                            Child, (IPTR)(texts[1] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                                MUIA_FlagsButton_Title, (IPTR)txtWindowPubScreenFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)pubScreenFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtWindowPubScreenFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtWindowPubScreenVisitorCount),
                            Child, (IPTR)(texts[2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtWindowPubScreenSigBit),
                            Child, (IPTR)(texts[3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtWindowPubScreenSigTask),
                            Child, (IPTR)(texts[4] = (Object *)TaskButtonObject,
                            End),
                        End),
                    End;

                    DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_InitChange);
                    DoMethod(wdwd->wdwd_MainGroup, OM_ADDMEMBER, subgroup);
                    DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_ExitChange);

                    MySetContentsHealed(texts[0], "%s", psn->psn_Node.ln_Name);
                    set(texts[1], MUIA_FlagsButton_Flags, psn->psn_Flags);
                    MySetContents(texts[2], "%lD", psn->psn_VisitorCount);
                    if (psn->psn_SigBit <= 31) {
                        MySetContents(texts[3], "%s", GetSigBitName(psn->psn_SigBit));
                    } else {
                        MySetContents(texts[3], "---");
                    }
                    set(texts[4], MUIA_TaskButton_Task, psn->psn_SigTask);

                    break;
                }
            }

            UnlockPubScreenList();
        }

        set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtWindowsDetailTitleScreen, we->we_Title, wdwd->wdwd_Title, sizeof(wdwd->wdwd_Title)));
    } else if (stricmp(we->we_Type, "WINDOW") == 0) {
        APTR texts[50], subgroup;
        struct Window *win = (struct Window *)we->we_Addr;

        subgroup = (Object *)ColGroup(2),
            Child, (IPTR)MyLabel2(txtWindowTitle2),
            Child, (IPTR)(texts[ 0] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtAddress2),
            Child, (IPTR)(texts[ 1] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowNextWindow),
            Child, (IPTR)(texts[ 2] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowLeftEdge),
            Child, (IPTR)(texts[ 3] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowTopEdge),
            Child, (IPTR)(texts[ 4] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWidth),
            Child, (IPTR)(texts[ 5] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowHeight),
            Child, (IPTR)(texts[ 6] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMouseY),
            Child, (IPTR)(texts[ 7] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMouseX),
            Child, (IPTR)(texts[ 8] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMinWidth),
            Child, (IPTR)(texts[ 9] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMinHeight),
            Child, (IPTR)(texts[10] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMaxWidth),
            Child, (IPTR)(texts[11] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMaxHeight),
            Child, (IPTR)(texts[12] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFlags2),
            Child, (IPTR)(texts[13] = (Object *)FlagsButtonObject,
                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                MUIA_FlagsButton_Title, (IPTR)txtWindowFlags,
                MUIA_FlagsButton_BitArray, (IPTR)windowFlags1,
                MUIA_FlagsButton_MaskArray, (IPTR)windowFlags2,
                MUIA_FlagsButton_WindowTitle, (IPTR)txtWindowWindowFlagsTitle,
            End),
            Child, (IPTR)MyLabel2(txtWindowMenuStrip),
            Child, (IPTR)(texts[14] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFirstRequest),
            Child, (IPTR)(texts[15] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowDMRequest),
            Child, (IPTR)(texts[16] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowReqCount),
            Child, (IPTR)(texts[17] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowWScreen),
            Child, (IPTR)(texts[18] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowRPort),
            Child, (IPTR)(texts[19] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBorderLeft),
            Child, (IPTR)(texts[20] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBorderTop),
            Child, (IPTR)(texts[21] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBorderRight),
            Child, (IPTR)(texts[22] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBorderBottom),
            Child, (IPTR)(texts[23] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBorderRPort),
            Child, (IPTR)(texts[24] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowFirstGadget),
            Child, (IPTR)(texts[25] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowParent),
            Child, (IPTR)(texts[26] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowDescendant),
            Child, (IPTR)(texts[27] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowPointer),
            Child, (IPTR)(texts[28] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowPtrHeight),
            Child, (IPTR)(texts[29] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowPtrWidth),
            Child, (IPTR)(texts[30] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowXOffset),
            Child, (IPTR)(texts[31] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowYOffset),
            Child, (IPTR)(texts[32] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowIDCMPFlags2),
            Child, (IPTR)(texts[33] = (Object *)FlagsButtonObject,
                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                MUIA_FlagsButton_Title, (IPTR)txtWindowIDCMPFlags,
                MUIA_FlagsButton_BitArray, (IPTR)idcmpFlags,
                MUIA_FlagsButton_WindowTitle, (IPTR)txtWindowIDCMPFlagsTitle,
            End),
            Child, (IPTR)MyLabel2(txtWindowUserPort),
            Child, (IPTR)(texts[34] = (Object *)PortButtonObject,
            End),
            Child, (IPTR)MyLabel2(txtWindowWindowPort),
            Child, (IPTR)(texts[35] = (Object *)PortButtonObject,
            End),
            Child, (IPTR)MyLabel2(txtWindowMessageKey),
            Child, (IPTR)(texts[36] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowDetailPen),
            Child, (IPTR)(texts[37] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowBlockPen),
            Child, (IPTR)(texts[38] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowCheckMark),
            Child, (IPTR)(texts[39] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowScreenTitle),
            Child, (IPTR)(texts[40] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowGZZMouseX),
            Child, (IPTR)(texts[41] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowGZZMouseY),
            Child, (IPTR)(texts[42] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowGZZWidth),
            Child, (IPTR)(texts[43] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowGZZHeight),
            Child, (IPTR)(texts[44] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowExtData),
            Child, (IPTR)(texts[45] = (Object *)DisassemblerButtonObject,
                MUIA_DisassemblerButton_ForceHexDump, TRUE,
            End),
            Child, (IPTR)MyLabel2(txtWindowUserData),
            Child, (IPTR)(texts[46] = (Object *)DisassemblerButtonObject,
                MUIA_DisassemblerButton_ForceHexDump, TRUE,
            End),
            Child, (IPTR)MyLabel2(txtWindowWLayer),
            Child, (IPTR)(texts[47] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowIFont),
            Child, (IPTR)(texts[48] = MyTextObject6()),
            Child, (IPTR)MyLabel2(txtWindowMoreFlags),
            Child, (IPTR)(texts[49] = MyTextObject6()),
        End;

        DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(wdwd->wdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(wdwd->wdwd_MainGroup, MUIM_Group_ExitChange);

        MySetContents(texts[ 0], "%s", nonetest((STRPTR)win->Title));
        MySetContents(texts[ 1], "$%08lx", win);
        MySetContents(texts[ 2], "$%08lx", win->NextWindow);
        MySetContents(texts[ 3], "%lD", win->LeftEdge);
        MySetContents(texts[ 4], "%lD", win->TopEdge);
        MySetContents(texts[ 5], "%lD", win->Width);
        MySetContents(texts[ 6], "%lD", win->Height);
        MySetContents(texts[ 7], "%lD", win->MouseY);
        MySetContents(texts[ 8], "%lD", win->MouseX);
        MySetContents(texts[ 9], "%lD", win->MinWidth);
        MySetContents(texts[10], "%lD", win->MinHeight);
        MySetContents(texts[11], "%lD", win->MaxWidth);
        MySetContents(texts[12], "%lD", win->MaxHeight);
        set(texts[13], MUIA_FlagsButton_Flags, win->Flags);
        MySetContents(texts[14], "$%08lx", win->MenuStrip);
        MySetContents(texts[15], "$%08lx", win->FirstRequest);
        MySetContents(texts[16], "$%08lx", win->DMRequest);
        MySetContents(texts[17], "%lD", win->ReqCount);
        MySetContents(texts[18], "$%08lx", win->WScreen);
        MySetContents(texts[19], "$%08lx", win->RPort);
        MySetContents(texts[20], "%lD", win->BorderLeft);
        MySetContents(texts[21], "%lD", win->BorderTop);
        MySetContents(texts[22], "%lD", win->BorderRight);
        MySetContents(texts[23], "%lD", win->BorderBottom);
        MySetContents(texts[24], "$%08lx", win->BorderRPort);
        MySetContents(texts[25], "$%08lx", win->FirstGadget);
        MySetContents(texts[26], "$%08lx", win->Parent);
        MySetContents(texts[27], "$%08lx", win->Descendant);
        MySetContents(texts[28], "$%08lx", win->Pointer);
        MySetContents(texts[29], "%lD", win->PtrHeight);
        MySetContents(texts[30], "%lD", win->PtrWidth);
        MySetContents(texts[31], "%lD", win->XOffset);
        MySetContents(texts[32], "%lD", win->YOffset);
        set(texts[33], MUIA_FlagsButton_Flags, win->IDCMPFlags);
        set(texts[34], MUIA_PortButton_Port, win->UserPort);
        set(texts[35], MUIA_PortButton_Port, win->WindowPort);
        MySetContents(texts[36], "$%08lx", win->MessageKey);
        MySetContents(texts[37], "%lD", win->DetailPen);
        MySetContents(texts[38], "%lD", win->BlockPen);
        MySetContents(texts[39], "$%08lx", win->CheckMark);
        MySetContents(texts[40], "%s", win->ScreenTitle);
        MySetContents(texts[41], "%lD", win->GZZMouseX);
        MySetContents(texts[42], "%lD", win->GZZMouseY);
        MySetContents(texts[43], "%lD", win->GZZWidth);
        MySetContents(texts[44], "%lD", win->GZZHeight);
        set(texts[45], MUIA_DisassemblerButton_Address, win->ExtData);
        set(texts[46], MUIA_DisassemblerButton_Address, win->UserData);
        MySetContents(texts[47], "$%08lx", win->WLayer);
        MySetContents(texts[48], "%s/%ld", win->IFont->tf_Message.mn_Node.ln_Name, win->IFont->tf_YSize);
        MySetContents(texts[49], "$%08lx", win->MoreFlags);

        set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtWindowsDetailTitleWindow, we->we_Title, wdwd->wdwd_Title, sizeof(wdwd->wdwd_Title)));
    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, exitButton, maingroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Windows",
        MUIA_Window_ID, MakeID('.','W','I','N'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(maingroup = (Object *)VGroup,
                        GroupFrame,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,

        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct WindowsDetailWinData *wdwd = INST_DATA(cl, obj);
        APTR parent;

        wdwd->wdwd_MainGroup = maingroup;

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
    struct WindowsDetailWinData *wdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_WindowsDetailWin_Object:
                wdwd->wdwd_Object = (struct WindowEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(WindowsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeWindowsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct WindowsDetailWinData), ENTRY(WindowsDetailWinDispatcher));
}

