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

struct CatalogsWinData {
    TEXT cwd_Title[WINDOW_TITLE_LENGTH];
    APTR cwd_CatalogList;
    APTR cwd_CatalogText;
    APTR cwd_CatalogCount;
};

struct CatalogsCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(catlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct CatalogEntry));
}
MakeStaticHook(catlist_con2hook, catlist_con2func);

HOOKPROTONHNO(catlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(catlist_des2hook, catlist_des2func);

HOOKPROTONHNO(catlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct CatalogEntry *ce = (struct CatalogEntry *)msg->entry;

    if (ce) {
        msg->strings[0] = ce->ce_Address;
        msg->strings[1] = ce->ce_Name;
        msg->strings[2] = ce->ce_Version;
        msg->strings[3] = ce->ce_Language;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtCatalogName;
        msg->strings[2] = txtCatalogVersion;
        msg->strings[3] = txtCatalogLanguage;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(catlist_dsp2hook, catlist_dsp2func);

STATIC LONG catlist_cmp2colfunc( struct CatalogEntry *ce1,
                                 struct CatalogEntry *ce2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(ce1->ce_Address, ce2->ce_Address);
        case 1: return stricmp(ce1->ce_Name, ce2->ce_Name);
        case 2: return VersionCompare(ce1->ce_Version, ce2->ce_Version);
        case 3: return stricmp(ce1->ce_Language, ce2->ce_Language);
    }
}

STATIC LONG catlist_cmpfunc( const struct Node *n1,
                             const struct Node *n2 )
{
    return stricmp(((struct CatalogEntry *)n1)->ce_Name, ((struct CatalogEntry *)n2)->ce_Name);
}

HOOKPROTONHNO(catlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct CatalogEntry *ce1, *ce2;
    ULONG col1, col2;

    ce1 = (struct CatalogEntry *)msg->entry1;
    ce2 = (struct CatalogEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = catlist_cmp2colfunc(ce2, ce1, col1);
    } else {
        cmp = catlist_cmp2colfunc(ce1, ce2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = catlist_cmp2colfunc(ce2, ce1, col2);
    } else {
        cmp = catlist_cmp2colfunc(ce1, ce2, col2);
    }

    return cmp;
}
MakeStaticHook(catlist_cmp2hook, catlist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct CatalogEntry *ce, void *userData ),
                         void *userData )
{
    struct CatalogEntry *ce;

    if ((ce = tbAllocVecPooled(globalPool, sizeof(struct CatalogEntry))) != NULL) {
        if (SendDaemon("GetCatalogList")) {
            while (ReceiveDecodedEntry(ce, sizeof(struct CatalogEntry))) {
                callback(ce, userData);
            }
        }

        tbFreeVecPooled(globalPool, ce);
    }
}

STATIC void IterateList( void (* callback)( struct CatalogEntry *ce, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct Library *LocaleBase;

    if ((LocaleBase = OpenLibrary("locale.library", 38)) != NULL) {
        struct Catalog *cCat = NULL;

        // we try to open a catalog which should exist on all systems
        // as the catalog is linked within a list we can traverse this list to find any other open catalog
        if (!amigaOS4 || ((cCat = OpenCatalog(NULL, "sys/c.catalog", OC_BuiltInLanguage, "english",
                                                                    TAG_DONE)) != NULL)) {
            struct MinList tmplist;
            struct List *catList;
            struct CatalogEntry *ce;
            struct Catalog *cat;

            NewList((struct List *)&tmplist);

            Forbid();

            if (amigaOS4) {
                catList = FindListOfNode(&cCat->cat_Link);
            } else {
                catList = (struct List *)(&((struct IntLocaleBase *)LocaleBase)->lb_CatalogList); // AmigaOS3 and MorphOS store the catalog list here
            }

            ITERATE_LIST(catList, struct Catalog *, cat) {
                if ((ce = AllocVec(sizeof(struct CatalogEntry), MEMF_CLEAR)) != NULL) {
                    ce->ce_Addr = cat;
                    _snprintf(ce->ce_Address, sizeof(ce->ce_Address), ADDRESS_FORMAT, cat);
                    stccpy(ce->ce_Name, nonetest(cat->cat_Link.ln_Name), sizeof(ce->ce_Name));
                    _snprintf(ce->ce_Version, sizeof(ce->ce_Version), "%ld.%ld", cat->cat_Version, cat->cat_Revision);
                    stccpy(ce->ce_Language, nonetest(cat->cat_Language), sizeof(ce->ce_Language));

                    AddTail((struct List *)&tmplist, (struct Node *)ce);
                }
            }

            Permit();

            if (sort) SortLinkedList((struct List *)&tmplist, catlist_cmpfunc);

            ITERATE_LIST(&tmplist, struct CatalogEntry *, ce) {
                callback(ce, userData);
            }
            FreeLinkedList((struct List *)&tmplist);

            if (amigaOS4) {
	            CloseCatalog(cCat);
            }
        }

        CloseLibrary(LocaleBase);
    }
}

STATIC void UpdateCallback( struct CatalogEntry *ce,
                            void *userData )
{
    struct CatalogsCallbackUserData *ud = (struct CatalogsCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, ce);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct CatalogEntry *ce,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtCatalogPrintLine, ce->ce_Address, ce->ce_Version, ce->ce_Language, ce->ce_Name);
}

STATIC void SendCallback( struct CatalogEntry *ce,
                          UNUSED void *userData )
{
    SendEncodedEntry(ce, sizeof(struct CatalogEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR catlist, cattext, catcount, updateButton, printButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Catalogs",
        MUIA_Window_ID, MakeID('C','A','T','A'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&catlist, MakeID('C','A','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR", &catlist_con2hook, &catlist_des2hook, &catlist_dsp2hook, &catlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&cattext, &catcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct CatalogsWinData *cwd = INST_DATA(cl, obj);
        APTR parent;

        cwd->cwd_CatalogList = catlist;
        cwd->cwd_CatalogText = cattext;
        cwd->cwd_CatalogCount = catcount;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtCatalogsTitle, cwd->cwd_Title, sizeof(cwd->cwd_Title)));
        set(obj, MUIA_Window_DefaultObject, catlist);

        DoMethod(parent,          MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,             MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(catlist,         MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_CatalogsWin_ListChange);
        DoMethod(updateButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CatalogsWin_Update);
        DoMethod(printButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CatalogsWin_Print);
        DoMethod(exitButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(catlist,         MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct CatalogsWinData *cwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cwd->cwd_CatalogList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct CatalogsWinData *cwd = INST_DATA(cl, obj);
    struct CatalogsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(cwd->cwd_CatalogList, MUIA_NList_Quiet, TRUE);
    DoMethod(cwd->cwd_CatalogList, MUIM_NList_Clear);

    ud.ud_List = cwd->cwd_CatalogList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(cwd->cwd_CatalogCount, ud.ud_Count);
    MySetContents(cwd->cwd_CatalogText, "");

    set(cwd->cwd_CatalogList, MUIA_NList_Quiet, FALSE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintCatalogs(NULL);

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct CatalogsWinData *cwd = INST_DATA(cl, obj);
    struct CatalogEntry *ce;

    if ((ce = (struct CatalogEntry *)GetActiveEntry(cwd->cwd_CatalogList)) != NULL) {
        MySetContents(cwd->cwd_CatalogText, "%s \"%s\"", ce->ce_Address, ce->ce_Name);
    }

    return 0;
}

DISPATCHER(CatalogsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                      return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                  return (mDispose(cl, obj, (APTR)msg));
        case MUIM_CatalogsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_CatalogsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_CatalogsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintCatalogs( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtCatalogPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendCatalogList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeCatalogsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct CatalogsWinData), ENTRY(CatalogsWinDispatcher));
}

