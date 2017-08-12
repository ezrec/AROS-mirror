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

struct CommoditiesDetailWinData {
    TEXT cdwd_Title[WINDOW_TITLE_LENGTH];
    APTR cdwd_Texts[9];
    APTR cdwd_SubList;
    struct CxEntry *cdwd_Commodity;
};

extern STRPTR GetCxNodeType( UBYTE type );

STATIC CONST struct LongFlag brokerFlags[] = {
    { 0x0001,        "< ??? >"       },
    { COF_ACTIVE,    "COF_ACTIVE"    },
    { COF_SHOW_HIDE, "COF_SHOW_HIDE" },
    { 0x0008,        "< ??? >"       },
    { 0x0010,        "< ??? >"       },
    { 0x0020,        "< ??? >"       },
    { 0x0040,        "< ??? >"       },
    { 0x0080,        "< ??? >"       },
    { 0x0100,        "< ??? >"       },
    { 0x0200,        "< ??? >"       },
    { 0x0400,        "< ??? >"       },
    { 0x0800,        "< ??? >"       },
    { 0x1000,        "< ??? >"       },
    { 0x2000,        "< ??? >"       },
    { 0x4000,        "< ??? >"       },
    { 0x8000,        "< ??? >"       },
    { 0,             NULL }
};

struct myCxFilter {
    struct Node cf_Node;
    UWORD cf_pad0;
    APTR cf_pad1[3];
    struct InputXpression *cf_Hotkey;
};

HOOKPROTONHNO(cxsublist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct CxSubEntry));
}
MakeStaticHook(cxsublist_con2hook, cxsublist_con2func);

HOOKPROTONHNO(cxsublist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(cxsublist_des2hook, cxsublist_des2func);

HOOKPROTONHNO(cxsublist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct CxSubEntry *cxse = (struct CxSubEntry *)msg->entry;

    if (cxse) {
        msg->strings[0] = cxse->cxse_Address;
        msg->strings[1] = cxse->cxse_Type;
        msg->strings[2] = cxse->cxse_Pri;
        msg->strings[3] = cxse->cxse_Hotkey;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeType;
        msg->strings[2] = txtNodePri;
        msg->strings[3] = txtCommodityHotkey;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(cxsublist_dsp2hook, cxsublist_dsp2func);

STATIC void ExtractHotkey( struct Node *node,
                           STRPTR buffer,
                           LONG maxlen )
{
    struct myCxFilter *cf = (struct myCxFilter *)node;
    struct InputXpression*ix = cf->cf_Hotkey;

    buffer[0] = 0x00;
    if (ix) {
        UWORD q = ix->ix_Qualifier;

        //kprintf("version %02lx, class %02lx, code %04lx, codemask %04lx, qual %04lx, qualmask %04lx, qualsame %04lx\n", ix->ix_Version, ix->ix_Class, ix->ix_Code, ix->ix_CodeMask, ix->ix_Qualifier, ix->ix_QualMask, ix->ix_QualSame);

        switch (ix->ix_Class) {
            case IECLASS_NULL: _strcatn(buffer, "null ", maxlen); break;
            case IECLASS_RAWKEY: _strcatn(buffer, "rawkey ", maxlen); break;
            case IECLASS_RAWMOUSE: _strcatn(buffer, "rawmouse ", maxlen); break;
            case IECLASS_EVENT: _strcatn(buffer, "event ", maxlen); break;
            case IECLASS_POINTERPOS: _strcatn(buffer, "pointerpos ", maxlen); break;
            case IECLASS_TIMER: _strcatn(buffer, "timer ", maxlen); break;
            case IECLASS_NEWPREFS: _strcatn(buffer, "newprefs ", maxlen); break;
            case IECLASS_DISKREMOVED: _strcatn(buffer, "diskremoved ", maxlen); break;
            case IECLASS_DISKINSERTED: _strcatn(buffer, "diskinserted ", maxlen); break;
            case IECLASS_ACTIVEWINDOW: _strcatn(buffer, "activewindow ", maxlen); break;
            case IECLASS_INACTIVEWINDOW: _strcatn(buffer, "inactivewindow ", maxlen); break;
            case IECLASS_NEWPOINTERPOS: _strcatn(buffer, "newpointerpos ", maxlen); break;
            case IECLASS_CHANGEWINDOW: _strcatn(buffer, "changewindow ", maxlen); break;
            case IECLASS_NEWMOUSE: _strcatn(buffer, "newmouse ", maxlen); break;
        #if defined(__amigaos4__)
            case IECLASS_MOUSEWHEEL: _strcatn(buffer, "mousewheel ", maxlen); break;
            case IECLASS_EXTENDEDRAWKEY: _strcatn(buffer, "extrawkey ", maxlen); break;
        #elif defined(__MORPHOS__)
            case IECLASS_EXTRAWKEY: _strcatn(buffer, "extrawkey ", maxlen); break;
        #endif
            default: _snprintf(buffer, maxlen, "?? (class=$%02lx) ", ix->ix_Class); break;
        }

        if (FLAG_IS_SET(q, IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) {
            _strcatn(buffer, "shift ", maxlen);
        } else if (FLAG_IS_SET(q, IEQUALIFIER_LSHIFT)) {
            _strcatn(buffer, "lshift ", maxlen);
        } else if (FLAG_IS_SET(q, IEQUALIFIER_RSHIFT)) {
            _strcatn(buffer, "rshift ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_CAPSLOCK)) {
            _strcatn(buffer, "capslock ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_CONTROL)) {
            _strcatn(buffer, "control ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_LALT | IEQUALIFIER_RALT)) {
            _strcatn(buffer, "alt ", maxlen);
        } else if (FLAG_IS_SET(q, IEQUALIFIER_LALT)) {
            _strcatn(buffer, "lalt ", maxlen);
        } else if (FLAG_IS_SET(q, IEQUALIFIER_RALT)) {
            _strcatn(buffer, "ralt ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_LCOMMAND)) {
            _strcatn(buffer, "lamiga ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_RCOMMAND)) {
            _strcatn(buffer, "ramiga ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_NUMERICPAD)) {
            _strcatn(buffer, "numericpad ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_REPEAT)) {
            _strcatn(buffer, "repeat ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_MIDBUTTON)) {
            _strcatn(buffer, "midbutton ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_RBUTTON)) {
            _strcatn(buffer, "rbutton ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_LEFTBUTTON)) {
            _strcatn(buffer, "leftbutton ", maxlen);
        }
        if (FLAG_IS_SET(q, IEQUALIFIER_RELATIVEMOUSE)) {
            _strcatn(buffer, "relativemouse ", maxlen);
        }

        if (ix->ix_Class == IECLASS_RAWKEY /* && ix->ix_Version == IX_VERSION */) {
            UWORD c = ix->ix_Code;
            CONST_STRPTR key;

            CLEAR_FLAG(c, IECODE_UP_PREFIX);
            switch (c) {
                case 0x0000: key = ""; break;
                case 0x0040: key = "space "; break;
                case 0x0041: key = "backspace "; break;
                case 0x0042: key = "tab "; break;
                case 0x0043: key = "enter "; break;
                case 0x0044: key = "return "; break;
                case 0x0045: key = "escape "; break;
                case 0x0046: key = "delete "; break;
                case 0x0047: key = "insert "; break;
                case 0x0048: key = "page_up "; break;
                case 0x0049: key = "page_down "; break;
                case 0x004b: key = "f11 "; break;
                case 0x004c: key = "cursor_up "; break;
                case 0x004d: key = "cursor_down "; break;
                case 0x004e: key = "cursor_right "; break;
                case 0x004f: key = "cursor_left "; break;
                case 0x0050: key = "f1 "; break;
                case 0x0051: key = "f2 "; break;
                case 0x0052: key = "f3 "; break;
                case 0x0053: key = "f4 "; break;
                case 0x0054: key = "f5 "; break;
                case 0x0055: key = "f6 "; break;
                case 0x0056: key = "f7 "; break;
                case 0x0057: key = "f8 "; break;
                case 0x0058: key = "f9 "; break;
                case 0x0059: key = "f10 "; break;
                case 0x005f: key = "help "; break;
                case 0x0060: key = "lshift "; break;
                case 0x0061: key = "rshift "; break;
                case 0x0062: key = "capslock "; break;
                case 0x0063: key = "control "; break;
                case 0x0064: key = "lalt "; break;
                case 0x0065: key = "ralt "; break;
                case 0x0066: key = "lamiga "; break;
                case 0x0067: key = "ramiga "; break;
                case 0x006d: key = "printscreen "; break;
                case 0x006e: key = "pause "; break;
                case 0x006f: key = "f12 "; break;
                case 0x0070: key = "home "; break;
                case 0x0071: key = "end "; break;
                case 0x0072: key = "media_stop "; break;
                case 0x0073: key = "media_play "; break;
                case 0x0074: key = "media_prev "; break;
                case 0x0075: key = "media_next "; break;
                case 0x0076: key = "media_shuffle "; break;
                case 0x0077: key = "media_repeat "; break;
                case NM_WHEEL_UP: key = "nm_wheel_up "; break;          // 0x7a
                case NM_WHEEL_DOWN: key = "nm_wheel_down "; break;      // 0x7b
                case NM_WHEEL_LEFT: key = "nm_wheel_left "; break;      // 0x7c
                case NM_WHEEL_RIGHT: key = "nm_wheel_right "; break;    // 0x7d
                case NM_BUTTON_FOURTH: key = "nm_wheel_button "; break; // 0x7e
                case 0xffff: key = "any "; break; // yes, the any-key really exists! :)
                default: key = NULL; break;
            }

            if (key) {
                _strcatn(buffer, key, maxlen);
                if (FLAG_IS_SET(ix->ix_Code, IECODE_UP_PREFIX)) {
                    _strcatn(buffer, "upstroke", maxlen);
                }
            } else {
                struct Library *KeymapBase;

                if ((KeymapBase = OpenLibrary("keymap.library", 37)) != NULL) {
                #if defined(__amigaos4__)
                    struct KeymapIFace *IKeymap;
                #endif

                    if (GETINTERFACE(IKeymap, KeymapBase)) {
                        struct InputEvent ie;
                        TEXT tmp[16];
                        LONG act;

                        memset(&ie, 0x00, sizeof(ie));
                        ie.ie_Class = IECLASS_RAWKEY;
                        ie.ie_Code = ix->ix_Code;
                        ie.ie_Qualifier = 0;

                        act = MapRawKey(&ie, tmp, sizeof(tmp), NULL);
                        if (act != -1 && isprint(buffer[0])) {
                            tmp[act] = 0x00;
                            _strcatn(buffer, tmp, maxlen);
                        } else {
                            _snprintf(&buffer[strlen(buffer)], maxlen - strlen(buffer), "?? (code=$%04lx)", ix->ix_Code);
                        }

                        DROPINTERFACE(IKeymap);
                    }

                    CloseLibrary(KeymapBase);
                }
            }
        } else if (ix->ix_Class == IECLASS_RAWMOUSE) {
            CONST_STRPTR key;

            switch (ix->ix_Code) {
                case 0x00: key = ""; break;
                case IECODE_UP_PREFIX: key = "mouse_up"; break;
                case IECODE_LBUTTON: key = "mouse_leftpress "; break;
                case IECODE_MBUTTON: key = "mouse_middlepress "; break;
                case IECODE_RBUTTON: key = "mouse_rightpress "; break;
            #if defined(__amigaos4__)
                case IECODE_4TH_BUTTON: key = "mouse_4thpress "; break;
                case IECODE_5TH_BUTTON: key = "mouse_5thpress "; break;
            #endif
                case IECODE_NOBUTTON: key = "nobutton"; break;
                default: key = NULL; break;
            }

            if (key) {
                _strcatn(buffer, key, maxlen);
            } else {
                _snprintf(&buffer[strlen(buffer)], maxlen - strlen(buffer), "?? (code=$%04lx)", ix->ix_Code);
            }
        } else if (ix->ix_Class == IECLASS_NEWMOUSE) {
            CONST_STRPTR key;

            switch (ix->ix_Code) {
                case 0x00: key = ""; break;
                case NM_WHEEL_UP: key = "nm_wheel_up"; break;
                case NM_WHEEL_DOWN: key = "nm_wheel_down"; break;
                case NM_WHEEL_LEFT: key = "nm_wheel_left"; break;
                case NM_WHEEL_RIGHT: key = "nm_wheel_right"; break;
                case NM_BUTTON_FOURTH: key = "nm_wheel_button"; break;
                default: key = NULL; break;
            }

            if (key) {
                _strcatn(buffer, key, maxlen);
            } else {
                _snprintf(&buffer[strlen(buffer)], maxlen - strlen(buffer), "?? (code=$%04lx)", ix->ix_Code);
            }
    /*
        } else if (ix->ix_Version != IX_VERSION) {
            _snprintf(&buffer[strlen(buffer)], maxlen - strlen(buffer), "?? (version=$%02lx)", ix->ix_Version);
    */
        }
    }
}

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct Library *CxBase;

    if ((CxBase = OpenLibrary("commodities.library", 37)) != NULL) {
    #if defined(__amigaos4__)
        struct CommoditiesIFace *ICommodities;
    #endif

        if (GETINTERFACE(ICommodities, CxBase)) {
            struct CommoditiesDetailWinData *cdwd = INST_DATA(cl, obj);
            struct CxEntry *cxe = cdwd->cdwd_Commodity;
            struct PrivateCxObj *mco = cxe->cxe_Addr;
            struct Node *node;
            struct MinList tmplist;
            struct CxSubEntry *cxse;

            MySetContents(cdwd->cdwd_Texts[0], "%s", mco->mco_Name);
            MySetContents(cdwd->cdwd_Texts[1], ADDRESS_FORMAT, mco);
            MySetContents(cdwd->cdwd_Texts[2], "%s", mco->mco_Title);
            MySetContents(cdwd->cdwd_Texts[3], "%s", mco->mco_Descr);
            MySetContents(cdwd->cdwd_Texts[4], "%s", GetCxNodeType(CxObjType((CxObj *)mco)));
            MySetContents(cdwd->cdwd_Texts[5], "%ld", mco->mco_Node.ln_Pri);
            set(cdwd->cdwd_Texts[6], MUIA_PortButton_Port, mco->mco_Port);
            set(cdwd->cdwd_Texts[7], MUIA_TaskButton_Task, mco->mco_Task);
            set(cdwd->cdwd_Texts[8], MUIA_FlagsButton_Flags, mco->mco_Flags);

            NewList((struct List *)&tmplist);

            ApplicationSleep(TRUE);

            set(cdwd->cdwd_SubList, MUIA_NList_Quiet, TRUE);
            DoMethod(cdwd->cdwd_SubList, MUIM_NList_Clear);

            Forbid();

            ITERATE_LIST(&mco->mco_SubList, struct Node *, node) {
                if ((cxse = AllocVec(sizeof(struct CxSubEntry), MEMF_CLEAR)) != NULL) {
                    _snprintf(cxse->cxse_Address, sizeof(cxse->cxse_Address), ADDRESS_FORMAT, node);
                    stccpy(cxse->cxse_Type, GetCxNodeType(CxObjType((CxObj *)node)), sizeof(cxse->cxse_Type));
                    _snprintf(cxse->cxse_Pri, sizeof(cxse->cxse_Pri), "%4ld", node->ln_Pri);
                    if (node->ln_Type == CX_FILTER) {
                        ExtractHotkey(node, cxse->cxse_Hotkey, sizeof(cxse->cxse_Hotkey));
                    } else {
                        stccpy(cxse->cxse_Hotkey, txtNone, sizeof(cxse->cxse_Hotkey));
                    }

                    AddTail((struct List *)&tmplist, (struct Node *)cxse);
                }
            }

            Permit();

            ITERATE_LIST(&tmplist, struct CxSubEntry *, cxse) {
                InsertBottomEntry(cdwd->cdwd_SubList, cxse);
            }
            FreeLinkedList((struct List *)&tmplist);

            set(cdwd->cdwd_SubList, MUIA_NList_Quiet, FALSE);

            ApplicationSleep(FALSE);

            set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtCommoditiesDetailTitle, cxe->cxe_Name, cdwd->cdwd_Title, sizeof(cdwd->cdwd_Title)));

            DROPINTERFACE(ICommodities);
        }

        CloseLibrary(CxBase);
    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[9], sublist, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Commodities",
        MUIA_Window_ID, MakeID('.','C','X','X'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtCommodityName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtCommodityTitle),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtCommodityDescription),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtCommodityPort2),
                            Child, (IPTR)(texts[ 6] = (Object *)PortButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtCommodityTask),
                            Child, (IPTR)(texts[ 7] = (Object *)TaskButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtCommodityFlags2),
                            Child, (IPTR)(texts[ 8] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtCommodityFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)brokerFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtCommodityBrokerFlagsTitle,
                            End),
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtCommoditySubObjects),
                            Child, (IPTR)MyNListviewObject(&sublist, MakeID('.','C','L','V'), "BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR", &cxsublist_con2hook, &cxsublist_des2hook, &cxsublist_dsp2hook, NULL, FALSE),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct CommoditiesDetailWinData *cdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, cdwd->cdwd_Texts, sizeof(cdwd->cdwd_Texts));
        cdwd->cdwd_SubList = sublist;

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
    struct CommoditiesDetailWinData *cdwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cdwd->cdwd_SubList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct CommoditiesDetailWinData *cdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_CommoditiesDetailWin_Commodity:
                cdwd->cdwd_Commodity = (struct CxEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(CommoditiesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeCommoditiesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct CommoditiesDetailWinData), ENTRY(CommoditiesDetailWinDispatcher));
}


