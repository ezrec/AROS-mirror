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

#if defined(__AROS__)
#include <proto/dos.h>   /* Includes Printf() before defining NO_INLINE_STDARRG */
#define NO_INLINE_STDARG /* Needed for NList classes */
#endif

#include "system_headers.h"

extern struct GfxBase   *GfxBase;
extern struct ExecBase  *SysBase;
extern APTR AboutText;

STATIC APTR oldwindowptr;

STATIC CONST CONST_STRPTR scout_classes[] = { "NList.mcc", "NListtree.mcc", NULL };

BOOL GetPriority( STRPTR nodename,
                  LONG *pri )
{
    BOOL result = FALSE;
    APTR priWin;

    if ((priWin = PriorityWindowObject, End) != NULL) {
        set(priWin, MUIA_PriorityWin_ObjectName, nodename);
        set(priWin, MUIA_PriorityWin_Priority, *pri);

        if ((result = (BOOL)DoMethod(priWin, MUIM_PriorityWin_GetPriority))) {
            get(priWin, MUIA_PriorityWin_Priority, pri);
        }


        MUI_DisposeObject(priWin);
    }

    return result;
}

void NoReqOn( void )
{
    struct Process *myprocess;

    myprocess = (struct Process *)FindTask(NULL);
    oldwindowptr = myprocess->pr_WindowPtr;
    myprocess->pr_WindowPtr = (APTR) -1;
}

void NoReqOff( void )
{
    ((struct Process *)FindTask(NULL))->pr_WindowPtr = oldwindowptr;
}

struct Library *MyOpenLibrary( CONST_STRPTR libname,
                               ULONG version )
{
    struct Library *libbase;

    if (!(libbase = OpenLibrary(libname, version))) {
        Printf("Failed to open %s version %d!\n", libname, version);
    }

    return libbase;
}

void Flags2BinStr( UBYTE flags,
                   STRPTR str,
                   ULONG maxlen )
{
    _snprintf(str, maxlen, "%%%ld%ld%ld%ld%ld%ld%ld%ld", BIT_IS_SET(flags, 7) ? 1 : 0,
                                                         BIT_IS_SET(flags, 6) ? 1 : 0,
                                                         BIT_IS_SET(flags, 5) ? 1 : 0,
                                                         BIT_IS_SET(flags, 4) ? 1 : 0,
                                                         BIT_IS_SET(flags, 3) ? 1 : 0,
                                                         BIT_IS_SET(flags, 2) ? 1 : 0,
                                                         BIT_IS_SET(flags, 1) ? 1 : 0,
                                                         BIT_IS_SET(flags, 0) ? 1 : 0);
}

void VARARGS68K STDARGS MySetContents( APTR textfield,
                                       CONST_STRPTR fmt, ...)
{
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, args)) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        nnset(textfield, MUIA_Text_Contents, buf);

        FreeVec(buf);
    }

    VA_END(args);
}

void VARARGS68K STDARGS MySetContentsHealed( APTR textfield,
                                             CONST_STRPTR fmt, ...)
{
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, args)) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        healstring(buf);
        nnset(textfield, MUIA_Text_Contents, buf);

        FreeVec(buf);
    }

    VA_END(args);
}

void VARARGS68K STDARGS MySetStringContents( APTR textfield,
                                             CONST_STRPTR fmt, ... )
{
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, args)) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        nnset(textfield, MUIA_String_Contents, buf);

        FreeVec(buf);
    }

    VA_END(args);
}

void SetCountText( APTR counttext,
                   ULONG structcnt )
{
   MySetContents(counttext, MUIX_R "%04ld", structcnt);
}

STRPTR MyGetWindowTitle( CONST_STRPTR type,
                         STRPTR title,
                         ULONG maxlen )
{
    BOOL ok;

    if (type[0] != 0x00) {
        _snprintf(title, maxlen, "SCOUT " CPU ": %s", type);
    } else {
        stccpy(title, "SCOUT " CPU, maxlen);
    }

    Forbid();
    ok = (FindPort("AMITCP") || FindPort("MIAMI.1"));
    Permit();

    if (ok) {
        STRPTR host;

        host = (opts.Host) ? opts.Host : (STRPTR)"localhost";

        if (type[0] != 0x00) {
            _snprintf(title, maxlen, "SCOUT " CPU ": %s [ %s ]", type, host);
        } else {
            _snprintf(title, maxlen, "SCOUT " CPU " [ %s ]", host);
        }
    }

    //for (i = 0; i < strlen(title); i++) title[i] = ToUpper(title[i]);

    return title;
}

STRPTR MyGetChildWindowTitle( CONST_STRPTR part1,
                              CONST_STRPTR part2,
                              STRPTR title,
                              ULONG maxlen )
{
    STRPTR esc;

    _snprintf(title, maxlen, "%s: %s", part1, part2);
    if ((esc = strchr(title, '\033')) != NULL) *esc = 0x00;

    return title;
}

BOOL CheckMCCVersion( CONST_STRPTR mcc,
                      ULONG minver,
                      ULONG minrev,
                      BOOL errorReq )
{
    Object *obj;
    BOOL result;
    ULONG clv = CLV_NOT_OPEN, ver = 0, rev = 0;

    if ((obj = MUI_NewObject(mcc, TAG_DONE)) != NULL) {
        get(obj, MUIA_Version, &ver);
        get(obj, MUIA_Revision, &rev);
        MUI_DisposeObject(obj);

        if (ver > minver || (ver == minver && rev >= minrev)) {
            clv = CLV_NEWER_OR_SAME;
        } else {
            clv = CLV_OLDER;
        }
    }

    if (clv != CLV_NEWER_OR_SAME) {
        TEXT libname[256];
        struct Library *lib;

        stccpy(libname, mcc, sizeof(libname));
        lib = OpenLibrary(libname, 0);
        if (!lib) {
            _snprintf(libname, sizeof(libname), "PROGDIR:mui/%s", mcc);
            lib = OpenLibrary(libname, 0);
        }
        if (!lib) {
            _snprintf(libname, sizeof(libname), "PROGDIR:libs/%s", mcc);
            lib = OpenLibrary(libname, 0);
        }
        if (!lib) {
            _snprintf(libname, sizeof(libname), "PROGDIR:%s", mcc);
            lib = OpenLibrary(libname, 0);
        }
        if (lib) {
            ver = lib->lib_Version;
            rev = lib->lib_Revision;

            clv = CheckLibVersion(lib, minver, minrev);

            CloseLibrary(lib);
        } else {
            clv = CLV_NOT_OPEN;
        }
    }

    switch (clv) {
        default:
        case CLV_NOT_OPEN:
            if (errorReq) {
                ErrorMsg(vers, msgMCCNotAvailable, mcc, mcc, minver, minrev);
            }
            result = FALSE;
            break;

        case CLV_OLDER:
            if (errorReq) {
                ErrorMsg(vers, msgMCCTooOld, mcc, mcc, minver, minrev, ver, rev);
            }
            result = FALSE;
            break;

        case CLV_NEWER_OR_SAME:
            result = TRUE;
            break;
    }

    return result;
}

void ApplicationSleep( BOOL sleep )
{
    set(AP_Scout, MUIA_Application_Sleep, sleep);
}

void RedrawActiveEntry( APTR list )
{
    DoMethod(list, MUIM_NList_Redraw, MUIV_NList_Redraw_Active);
}

void RemoveActiveEntry( APTR list )
{
    DoMethod(list, MUIM_NList_Remove, MUIV_NList_Remove_Active);
}

void InsertBottomEntry( APTR list,
                        APTR entry )
{
    DoMethod(list, MUIM_NList_InsertSingle, entry, MUIV_List_Insert_Bottom);
}

void InsertSortedEntry( APTR list,
                        APTR entry )
{
    DoMethod(list, MUIM_NList_InsertSingle, entry, MUIV_List_Insert_Sorted);
}

APTR GetActiveEntry( APTR list )
{
    APTR result = NULL;

    if (list) DoMethod(list, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &result);

    return result;
}

struct MUI_NListtree_TreeNode *GetActiveTreeNode( APTR tree )
{
    struct MUI_NListtree_TreeNode *result = NULL;

    if (tree) result = (struct MUI_NListtree_TreeNode *)DoMethod(tree, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

    return result;
}

APTR MyNListviewObject( APTR *list,
                        ULONG id,
                        CONST_STRPTR format,
                        struct Hook *conhook,
                        struct Hook *deshook,
                        struct Hook *dsphook,
                        struct Hook *cmphook,
                        BOOL input )
{
    APTR listview;

    if ((listview = NListviewObject,
        MUIA_CycleChain, TRUE,
        //MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        //MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
        MUIA_NListview_NList, *list = NListObject,
            MUIA_NList_Input, input,
            MUIA_NList_Format, format,
            MUIA_NList_Title, TRUE,
            MUIA_NList_TitleSeparator, TRUE,
            (conhook) ? MUIA_NList_ConstructHook2 : TAG_IGNORE, conhook,
            (deshook) ? MUIA_NList_DestructHook2 : TAG_IGNORE, deshook,
            (dsphook) ? MUIA_NList_DisplayHook2 : TAG_IGNORE, dsphook,
            (cmphook) ? MUIA_NList_CompareHook2 : TAG_IGNORE, cmphook,
            MUIA_NList_Exports, MUIV_NList_Exports_ColWidth | MUIV_NList_Exports_ColOrder,
            MUIA_NList_Imports, MUIV_NList_Imports_ColWidth | MUIV_NList_Imports_ColOrder,
            MUIA_NList_MinColSortable, 0,
            MUIA_NList_Pool, globalPool,
            MUIA_ContextMenu, NULL,
            MUIA_ObjectID, id,
        End,
    End) != NULL) {
        DoMethod(*list, MUIM_Notify, MUIA_NList_SortType,    MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_Set, MUIA_NList_TitleMark, MUIV_TriggerValue);
        DoMethod(*list, MUIM_Notify, MUIA_NList_SortType2,   MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_Set, MUIA_NList_TitleMark2, MUIV_TriggerValue);
        DoMethod(*list, MUIM_Notify, MUIA_NList_TitleClick,  MUIV_EveryTime, MUIV_Notify_Self, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
        DoMethod(*list, MUIM_Notify, MUIA_NList_TitleClick2, MUIV_EveryTime, MUIV_Notify_Self, 4, MUIM_NList_Sort3, MUIV_TriggerValue, MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
    }

    return listview;
}

APTR MyNListtreeObject( APTR *tree,
                        CONST_STRPTR format,
                        struct Hook *conhook,
                        struct Hook *deshook,
                        struct Hook *dsphook,
                        struct Hook *cmphook,
                        struct Hook *findhook,
                        ULONG column,
			BOOL emptynodes)
{
    APTR listview;

    listview = NListviewObject,
        MUIA_CycleChain, TRUE,
        //MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        //MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
        MUIA_NListview_NList, *tree = NListtreeObject,
            InputListFrame,
            MUIA_NListtree_DragDropSort, FALSE,
            MUIA_NListtree_Format, format,
            MUIA_NListtree_Title, TRUE,
            (conhook) ? MUIA_NListtree_ConstructHook : TAG_IGNORE, conhook,
            (deshook) ? MUIA_NListtree_DestructHook : TAG_IGNORE, deshook,
            (dsphook) ? MUIA_NListtree_DisplayHook : TAG_IGNORE, dsphook,
            MUIA_NListtree_CompareHook, (cmphook) ? cmphook : (struct Hook *)MUIV_NListtree_CompareHook_Tail,
            (findhook) ? MUIA_NListtree_FindUserDataHook : TAG_IGNORE, findhook,
            MUIA_NListtree_TreeColumn, column,
	    MUIA_NListtree_EmptyNodes, emptynodes,
            MUIA_NListtree_MultiSelect, MUIV_NListtree_MultiSelect_None,
            MUIA_ContextMenu, NULL,
            MUIA_NList_Pool, globalPool,
        End,
    End;

    return listview;
}

APTR MyBelowListview( APTR *text,
                      APTR *count )
{
    APTR group;

    group = HGroup,
        Child, *text = MyTextObject5(SPACE40),
        Child, (IPTR)HGroup, MUIA_HorizWeight, 0,
            Child, (IPTR)MyLabel(txtCount),
            Child, *count = MyTextObject4("0000"),
        End,
    End;

    return group;
}

APTR MyTextObject( void )
{
    return (TextObject, MyTextFrame, End);
}

APTR MyTextObject2( void )
{
    return (TextObject, MyTextFrame, MUIA_Text_SetMax, TRUE, End);
}

APTR MyTextObject3( CONST_STRPTR text )
{
    return (TextObject, MyTextFrame, MUIA_Text_Contents, text, End);
}

APTR MyTextObject4( CONST_STRPTR text )
{
    return (TextObject, MyTextFrame, MUIA_Text_SetMax, TRUE, MUIA_Text_Contents, text, End);
}

APTR MyTextObject5( CONST_STRPTR text )
{
    return (TextObject, MyTextFrame, MUIA_Text_SetMin, TRUE, MUIA_Text_Contents, text, End);
}

APTR MyTextObject6( void )
{
    return (TextObject, MyTextFrame, MUIA_Text_SetMin, FALSE, MUIA_Text_PreParse, MUIX_C, End);
}

APTR MyTextObject7( CONST_STRPTR text )
{
    return (TextObject, MyTextFrame, MUIA_Text_SetMin, FALSE, MUIA_Text_PreParse, MUIX_C, MUIA_Text_Contents, text, End);
}

APTR MyCheckmarkImage( void )
{
    return (ImageObject,
        ImageButtonFrame,
        MUIA_InputMode,      MUIV_InputMode_None,
        MUIA_Image_Spec,     MUII_CheckMark,
        MUIA_Image_Spec,     MUII_CheckMark,
        MUIA_Image_FreeVert, TRUE,
        MUIA_Background,     MUII_ButtonBack,
        MUIA_ShowSelState,   FALSE,
    End);
}

BOOL isValidPointer( APTR ptr )
{
    // anything below a certain address is considered invalid
    if ((IPTR)ptr <= 0x200 || (IPTR)ptr == ~0) {
        return FALSE;
    }

#if defined(__amigaos4__)
	{
	    struct MMUIFace *IMMU;

	    if ((IMMU = (struct MMUIFace *)GetInterface((struct Library *)SysBase, "mmu", 1, NULL)) != NULL) {
	        APTR sysStack;
	        ULONG flags;

	        sysStack = SuperState();
	        flags = IMMU->GetMemoryAttrs(ptr, 0);
	        UserState(sysStack);

	        DropInterface((struct Interface *)IMMU);

	        if (FLAG_IS_SET(flags, MEMATTRF_NOT_MAPPED)) {
	            return FALSE;
	        } else {
	            return TRUE;
	        }
	    }
	}
#elif defined(__MORPHOS__)
    {
        ULONG RomStart, RomEnd;

    	if (SysBase->LibNode.lib_Version <= 50)
	    {
		    RomStart = 0x1000000;
    		RomEnd   = 0x2000000;
	    }
    	else
	    {
		    NewGetSystemAttrs(&RomStart, sizeof(RomStart), SYSTEMINFOTYPE_MODULE_START, TAG_DONE);
    		NewGetSystemAttrs(&RomEnd, sizeof(RomEnd), SYSTEMINFOTYPE_MODULE_SIZE, TAG_DONE);
	    	RomEnd += RomStart;
    	}

        if ((IPTR)ptr >= RomStart && (IPTR)ptr < RomEnd) {
            return TRUE;
        }
    }
#elif !defined(__AROS__)
    if (MMUBase) {
        ULONG flags;

        // invalid addresses are marked with MAPP_INVALID
        flags = GetProperties(NULL, (ULONG)ptr, TAG_DONE);
        if (FLAG_IS_SET(flags, MAPP_INVALID)) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
#endif

    return (BOOL)((TypeOfMem(ptr)) ? TRUE : FALSE);
}

void healstring( STRPTR s )
{
    ULONG i = 0;

    while (s[i++] != 0x00) {
        if (s[i] > 0xfe && s[i] <= 0x07) {
            s[i] = 0x00;
        } else if (s[i] > 0x07 && s[i] <= 0x11) {
            s[i] = ' ';
        }
    }
}

CONST_STRPTR nonetest( CONST_STRPTR s )
{
    CONST_STRPTR t;
    TEXT c;

    // very simple test for sensible names, may cause Enforcer-Hits for wrong >pointers
    if ((IPTR)s <= 0x200 || (s != NULL && s[0] == 0)) {
        return txtNone;
    }

    t = s;
    while ((c = *t) != '\0') {
        if (currentLocale != NULL) {
            if (!IsPrint(currentLocale, c) && c != '\x0a' && c != '\x0d') {
                return txtNone;
            }
        }
        if (!((c > 8 && c < 128) || (c > 159 && c != 215 && c != 247 && c != 255))) {
            return txtNone;
        }
        t++;
    }

    return s;
}

BOOL points2ram( APTR addr )
{
    if (isValidPointer(addr)) {
#if defined(__amigaos4__)
        ULONG type;

        type = TypeOfMem(addr);
        if ((type & (MEMF_PRIVATE | MEMF_SHARED)) == (MEMF_PRIVATE | MEMF_SHARED)) {
            // this address can be either private or shared, hence it must be RAM, not ROM
            return TRUE;
        }
#else
        return TRUE;
#endif
    }

    return FALSE;
}

STRPTR StripMUIFormatting( STRPTR str )
{
    STATIC TEXT buffer[TEXT_LENGTH];
    TEXT c, *p = buffer;
    ULONG i;

    i = 0;
    while ((c = *str++) != '\0' && i < sizeof(buffer) - 1) {
        if (c == '\033') {
            str++;
        } else {
            *p++ = c;
            i++;
        }
    }
    *p = '\0';

    return buffer;
}

BOOL IsReal( CONST_STRPTR text )
{
    ULONG punkte = 0;
    TEXT c;

    while ((c = *text++) != '\0') {
        if (c == '.') {
            if (punkte) {
                return FALSE;
            }
            punkte++;
        } else if (!isdigit(c)) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL IsHex( CONST_STRPTR text,
            SIPTR *result )
{
    int i;

    *result = 0;

    if (text) {
        if (text[0] == '$')
            i = 1;
        else if (!strnicmp("0x", text, 2))
            i = 2;
        else
            return FALSE;

        if (strlen(text + i) <= sizeof(*result) * 2) {
            *result = strtol(text + i, NULL, 16);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IsUHex( CONST_STRPTR text,
             IPTR *result)
{
    ULONG i;

    *result = 0;

    if (text) {
        if (text[0] == '$')
            i = 1;
        else if (!strnicmp("0x", text, 2))
            i = 2;
        else
            return FALSE;

        if (strlen(text + i) <= sizeof(*result) * 2) {
            *result = strtoul(text + i, NULL, 16);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IsDec( CONST_STRPTR text,
            SIPTR *result)
{
    while (*text != '\0' && isspace(*text)) {
        text++;
    }

    return (BOOL)(stcd_l(text, result) > 0);
}

LONG HexCompare( CONST_STRPTR hex1str,
                 CONST_STRPTR hex2str )
{
    SIPTR hex1, hex2;

    IsHex(hex1str, &hex1);
    IsHex(hex2str, &hex2);

    return hex1 - hex2;
}

LONG IntegerCompare( CONST_STRPTR int1str,
                     CONST_STRPTR int2str )
{
    SIPTR int1, int2;

    IsDec(int1str, &int1);
    IsDec(int2str, &int2);

    return int1 - int2;
}

LONG VersionCompare( CONST_STRPTR ver1str,
                     CONST_STRPTR ver2str )
{
    STRPTR copy1, copy2;
    SIPTR ver1, ver2, rev1, rev2;
    LONG result = 0;

    copy1 = _strdup_pool(ver1str, globalPool);
    copy2 = _strdup_pool(ver2str, globalPool);

    if (copy1 != NULL && copy2 != NULL) {
        STRPTR p;

        p = strchr(copy1, '.');
        *p++ = 0x00;
        IsDec(copy1, &ver1);
        IsDec(p, &rev1);
        p = strchr(copy2, '.');
        *p++ = 0x00;
        IsDec(copy2, &ver2);
        IsDec(p, &rev2);

        result = ver1 - ver2;
        if (result == 0) result = rev1 - rev2;
    }

    tbFreeVecPooled(globalPool, copy1);
    tbFreeVecPooled(globalPool, copy2);

    return result;
}

LONG PriorityCompare( CONST_STRPTR pri1str,
                      CONST_STRPTR pri2str )
{
    SIPTR pri1, pri2;

    IsDec(pri1str, &pri1);
    IsDec(pri2str, &pri2);

    return pri2 - pri1;
}

void SortLinkedList( struct List *lh,
                     LONG (* compare)( const struct Node *, const struct Node *) )
{
    struct List list[2], *from, *to;
    struct Node *ln;
    LONG insize;

    from = &list[0];
    to = &list[1];
    NewList(from);
    NewList(to);

    while ((ln = RemHead(lh)) != NULL) {
        AddTail(from, ln);
    }

    insize = 1;

    while (TRUE) {
        LONG nmerges, psize, qsize, i;
        struct Node *p, *q, *e;

        NewList(to);
        p = from->lh_Head;
        nmerges = 0;

        while (p->ln_Succ != NULL) {
            nmerges++;
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                if (q->ln_Succ != NULL && q->ln_Succ->ln_Succ == NULL) break;
                q = q->ln_Succ;
                psize++;
            }

            qsize = insize;

            while (psize > 0 || (qsize > 0 && q->ln_Succ != NULL)) {
                if (psize == 0) {
                    e = q;
                    q = q->ln_Succ;
                    qsize--;
                } else if (qsize == 0 || q->ln_Succ == NULL) {
                    e = p;
                    p = p->ln_Succ;
                    psize--;
                } else if (compare(p, q) <= 0) {
                    e = p;
                    p = p->ln_Succ;
                    psize--;
                } else {
                    e = q;
                    q = q->ln_Succ;
                    qsize--;
                }

                Remove(e);
                AddTail(to, e);
            }

            p = q;
        }

        if (nmerges <= 1) {
            break;
        } else {
            struct List *tmp;

            tmp = from;
            from = to;
            to = tmp;

            NewList(to);

            insize *= 2;
        }
    }

    NewList(lh);
    while ((ln = RemHead(to)) != NULL) {
        AddTail(lh, ln);
    }
}

void FreeLinkedList( struct List *lh )
{
    struct Node *ln;

    while ((ln = RemHead(lh)) != NULL) {
        FreeVec(ln);
    }
}

void GetRamPointerCount( struct Library *lib,
                         LONG *count,
                         LONG *total )
{
    LONG max;
    LONG cnt, offset;

    max = lib->lib_NegSize;

    cnt = 0;
    for (offset = LIB_VECTSIZE; offset <= max; offset += LIB_VECTSIZE) {
        struct JumpEntry *je;

        je = (struct JumpEntry *)((UBYTE *)lib - offset);

        if (je->je_JMPInstruction == 0x0000) {
            // this cannot (or better: should not) happen
            cnt = -1;
            break;
        } else if (je->je_JMPInstruction == 0x4ef9) {
            if (points2ram(je->je_JumpAddress) || je->je_JumpAddress == NULL) cnt++;
        }
    }

    *count = cnt;
    *total = lib->lib_NegSize / LIB_VECTSIZE;
}

struct List *FindListOfNode( struct Node *ln )
{
    struct List *lh = NULL;

    if (ln) {
        while (ln->ln_Pred) ln = ln->ln_Pred;
        lh = (struct List *)ln;
    }

    return lh;
}

IPTR AllocListEntry( APTR pool,
                     APTR source,
                     ULONG size )
{
    void *result;

    if ((result = tbAllocVecPooled(pool, size)) != NULL) {
        CopyMem(source, result, size);
    }

    return (IPTR)result;
}

void FreeListEntry( APTR pool,
                    APTR *entry )
{
    if (*entry) {
        tbFreeVecPooled(pool, *entry);
        *entry = NULL;
    }
}

void NameCopy( STRPTR dst,
               CONST_STRPTR src)
{
    for (;;) {
        UBYTE c;

        c = *src++;

        if (c == '\0' || c == '.') {
            break;
        }

        *dst++ = ToUpper(c);
    }

    *dst++ = ':';
    *dst = '\0';
}

STATIC ASM SAVEDS void singlewindows_callfunc( void )
{
    opts.SingleWindows = !opts.SingleWindows;
    set(WI_Main, MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0);
}

MakeStaticHook(singlewindows_callhook, singlewindows_callfunc);

STATIC ASM SAVEDS void flushdevs_callfunc( void )
{
    FlushDevices();
}

MakeStaticHook(flushdevs_callhook, flushdevs_callfunc);

STATIC ASM SAVEDS void flushfonts_callfunc( void )
{
    FlushFonts();
}

MakeStaticHook(flushfonts_callhook, flushfonts_callfunc);

STATIC ASM SAVEDS void flushlibs_callfunc( void )
{
    FlushLibraries();
}

MakeStaticHook(flushlibs_callhook, flushlibs_callfunc);

STATIC ASM SAVEDS void flushall_callfunc( void )
{
    FlushDevices();
    FlushFonts();
    FlushLibraries();
}

MakeStaticHook(flushall_callhook, flushall_callfunc);

STATIC ASM SAVEDS void clearvect_callfunc( void )
{
    ClearResetVectors();
}

MakeStaticHook(clearvect_callhook, clearvect_callfunc);

enum menu_ids {
    MENU_PROJECT = 1,
        MENU_ABOUT, MENU_ABOUT_MUI, MENU_QUIT,

    MENU_LIST = 10,
       MENU_ALLOCATIONS, MENU_ASSIGNS, MENU_CLASSES, MENU_COMMODITIES, MENU_DEVICES, MENU_EXPANSIONS, MENU_FONTS,
       MENU_INPUTHANDLERS, MENU_INTERRUPTS, MENU_LIBRARIES, MENU_LOCKS, MENU_LOWMEMORY, MENU_MEMORY, MENU_MOUNTS,
       MENU_PORTS, MENU_RESIDENTS, MENU_COMMANDS, MENU_RESOURCES, MENU_SEMAPHORES, MENU_SCREENMODES, MENU_SYSTEM,
       MENU_TASKS, MENU_TIMERS, MENU_VECTORS, MENU_WINDOWS, MENU_PATCHES, MENU_CATALOGS, MENU_AUDIOMODES,
       MENU_RESETHANDLERS,

    MENU_OTHER = 50,
        MENU_FLUSHDEVS, MENU_FLUSHFONTS, MENU_FLUSHLIBS, MENU_FLUSHALL, MENU_CLEARVECTORS,

    MENU_OPTIONS = 70,
        MENU_SINGLE_WINDOWS
};

static Object *MakeMenuitem( CONST_STRPTR str,
                             ULONG ud )
{
    Object *result;

    if (str == NULL) {
        result = MenuitemObject,
            MUIA_Menuitem_Title, NM_BARLABEL,
        End;
    } else if (str[1] == '\0') {
        result = MenuitemObject,
            MUIA_Menuitem_Title, str + 2,
            MUIA_Menuitem_Shortcut, str,
            MUIA_UserData, ud,
        End;
    } else {
        result = MenuitemObject,
            MUIA_Menuitem_Title, str,
            MUIA_UserData, ud,
        End;
    }

    return result;
}

APTR GetApplication( void )
{
    APTR menustrip;
    APTR app, win;

    menustrip = MenustripObject,
        MUIA_Family_Child, (IPTR)MenuObject,
            MUIA_Menu_Title, txtMenuProject,
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuAbout,             MENU_ABOUT),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuAboutMUI,          MENU_ABOUT_MUI),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuQuit,              MENU_QUIT),
        End,
        MUIA_Family_Child, (IPTR)MenuObject,
            MUIA_Menu_Title, txtMenuList,
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuAllocations,       MENU_ALLOCATIONS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuAssigns,           MENU_ASSIGNS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuClasses,           MENU_CLASSES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuCommodities,       MENU_COMMODITIES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuDevices,           MENU_DEVICES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuExpansions,        MENU_EXPANSIONS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuFonts,             MENU_FONTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuInputHandlers,     MENU_INPUTHANDLERS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuInterrupts,        MENU_INTERRUPTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuLibraries,         MENU_LIBRARIES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuLocks,             MENU_LOCKS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuLowMemory,         MENU_LOWMEMORY),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuMemory,            MENU_MEMORY),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuMount,             MENU_MOUNTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuPorts,             MENU_PORTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuResidents,         MENU_RESIDENTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuResCmds,           MENU_COMMANDS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuResources,         MENU_RESOURCES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuScreenModes,       MENU_SCREENMODES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuSemaphores,        MENU_SEMAPHORES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuSystem,            MENU_SYSTEM),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuTasks,             MENU_TASKS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuTimer,             MENU_TIMERS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuVectors,           MENU_VECTORS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuWindows,           MENU_WINDOWS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(NULL,                     0),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuInstalledPatches,  MENU_PATCHES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuLocaleCatalogs,    MENU_CATALOGS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuAHIAudioModes,     MENU_AUDIOMODES),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuResetHandlers,     MENU_RESETHANDLERS),
        End,
        MUIA_Family_Child, (IPTR)MenuObject,
            MUIA_Menu_Title, txtMenuOther,
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuFlushDevices,      MENU_FLUSHDEVS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuFlushFonts,        MENU_FLUSHFONTS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuFlushLibraries,    MENU_FLUSHLIBS),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuFlushAll,          MENU_FLUSHALL),
            MUIA_Family_Child, (IPTR)MakeMenuitem(NULL,                     0),
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuClearResetVectors, MENU_CLEARVECTORS),
        End,
        MUIA_Family_Child, (IPTR)MenuObject,
            MUIA_Menu_Title, txtMenuOptions,
            MUIA_Family_Child, (IPTR)MakeMenuitem(txtMenuSingleWindows,     MENU_SINGLE_WINDOWS),
        End,
    End;

    if ((AP_Scout = app = ApplicationObject,
        MUIA_Application_Title      , PROGNAME,
        MUIA_Application_Version    , version_date,
        MUIA_Application_Copyright  , COPYRIGHT,
        MUIA_Application_Author     , AUTHOR,
        MUIA_Application_Description, DESCRIPTION,
        MUIA_Application_Base       , PROGNAME,
        MUIA_Application_Commands   , arexx_list,
        MUIA_Application_Menustrip  , menustrip,
        MUIA_Application_DiskObject , GetDiskObject("PROGDIR:Scout"),
        MUIA_Application_HelpFile   , "scout.guide",
        MUIA_Application_UsedClasses, scout_classes,
        SubWindow, WI_Main = win = MainWindowObject,
            MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
        End,
    End) != NULL) {
        struct PatchPort *pp;
        struct SetManPort *sp;
        struct Library *pc;
        APTR menu;

        DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_ABOUT,          win,                     1, MUIM_MainWin_About);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_ABOUT_MUI,      MUIV_Notify_Application, 2, MUIM_Application_AboutMUI, win);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_QUIT,           MUIV_Notify_Self,        2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_ALLOCATIONS,    win,                     1, MUIM_MainWin_ShowAllocations);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_ASSIGNS,        win,                     1, MUIM_MainWin_ShowAssigns);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_CLASSES,        win,                     1, MUIM_MainWin_ShowClasses);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_COMMODITIES,    win,                     1, MUIM_MainWin_ShowCommodities);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_DEVICES,        win,                     1, MUIM_MainWin_ShowDevices);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_EXPANSIONS,     win,                     1, MUIM_MainWin_ShowExpansions);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_FONTS,          win,                     1, MUIM_MainWin_ShowFonts);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_INPUTHANDLERS,  win,                     1, MUIM_MainWin_ShowInputHandlers);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_INTERRUPTS,     win,                     1, MUIM_MainWin_ShowInterrupts);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_LIBRARIES,      win,                     1, MUIM_MainWin_ShowLibraries);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_LOCKS,          win,                     1, MUIM_MainWin_ShowLocks);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_LOWMEMORY,      win,                     1, MUIM_MainWin_ShowLowMemory);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_MEMORY,         win,                     1, MUIM_MainWin_ShowMemory);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_MOUNTS,         win,                     1, MUIM_MainWin_ShowMounts);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_PORTS,          win,                     1, MUIM_MainWin_ShowPorts);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_RESIDENTS,      win,                     1, MUIM_MainWin_ShowResidents);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_COMMANDS,       win,                     1, MUIM_MainWin_ShowCommands);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_RESOURCES,      win,                     1, MUIM_MainWin_ShowResources);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_SCREENMODES,    win,                     1, MUIM_MainWin_ShowScreenModes);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_SEMAPHORES,     win,                     1, MUIM_MainWin_ShowSemaphores);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_SYSTEM,         win,                     1, MUIM_MainWin_ShowSystem);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_TASKS,          win,                     1, MUIM_MainWin_ShowTasks);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_TIMERS,         win,                     1, MUIM_MainWin_ShowTimers);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_VECTORS,        win,                     1, MUIM_MainWin_ShowVectors);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_WINDOWS,        win,                     1, MUIM_MainWin_ShowWindows);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_PATCHES,        win,                     1, MUIM_MainWin_ShowPatches);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_CATALOGS,       win,                     1, MUIM_MainWin_ShowCatalogs);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_AUDIOMODES,     win,                     1, MUIM_MainWin_ShowAudioModes);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_RESETHANDLERS,  win,                     1, MUIM_MainWin_ShowResetHandlers);

        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_FLUSHDEVS,      MUIV_Notify_Application, 2, MUIM_CallHook, &flushdevs_callhook);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_FLUSHFONTS,     MUIV_Notify_Application, 2, MUIM_CallHook, &flushfonts_callhook);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_FLUSHLIBS,      MUIV_Notify_Application, 2, MUIM_CallHook, &flushlibs_callhook);
        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_FLUSHALL,       MUIV_Notify_Application, 2, MUIM_CallHook, &flushall_callhook);

        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_CLEARVECTORS,   MUIV_Notify_Application, 2, MUIM_CallHook, &clearvect_callhook);

        DoMethod(app, MUIM_Notify, MUIA_Application_MenuAction, MENU_SINGLE_WINDOWS, MUIV_Notify_Self,        2, MUIM_CallHook, &singlewindows_callhook);

        DoMethod(app, MUIM_Notify, MUIA_Application_Iconified,  FALSE,               win,                     1, MUIM_MainWin_UpdateAll);

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
        Forbid();
        pp = (struct PatchPort *)FindPort(PATCHPORT_NAME);
        sp = (struct SetManPort *)FindPort(SETMANPORT_NAME);
        pc = (struct Library *)OpenResource(PATCHRES_NAME);
        Permit();
#else
        pp = NULL;
        sp = NULL;
        pc = NULL;
#endif

        menu = (APTR)DoMethod(menustrip, MUIM_FindUData, MENU_PATCHES);
        nnset(menu, MUIA_Menuitem_Enabled, (pp != NULL || sp != NULL || pc != NULL));

        menu = (APTR)DoMethod(menustrip, MUIM_FindUData, MENU_SINGLE_WINDOWS);
        SetAttrs(menu, MUIA_NoNotify, TRUE,
                       MUIA_Menuitem_Checkit, TRUE,
                       MUIA_Menuitem_Toggle, TRUE,
                       MUIA_Menuitem_Checked, opts.SingleWindows,
                       TAG_DONE);
    }

    return app;
}

