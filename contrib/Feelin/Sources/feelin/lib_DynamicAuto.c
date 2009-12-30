/*
**    Extension to the Dynamic ID system
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************
                        
$VER: 08.00 (2004/12/16)
 
*/

#include "Private.h"

///Header

struct FeelinDynamicAutoAnchor
{
    struct FeelinDynamicAutoAnchor  *Next;
    struct FeelinDynamicAutoAnchor  *Prev;

    STRPTR                           Name;       // Allocated string e.g. "Area" for "FM_Area_Setup"
    FList                            Attributes;
    FList                            Methods;
};

struct FeelinDynamicAutoEntry
{
    struct FeelinDynamicAutoEntry   *Next;
    struct FeelinDynamicAutoEntry   *Prev;

    STRPTR                           Name;       // Allocated string e.g. "Setup" for "FM_Area_Setup"
    struct FeelinDynamicAutoAnchor  *Anchor;
    FList                           *AnchorList;
    struct FeelinDynamicAutoSave    *FirstSave;
};

struct FeelinDynamicAutoSave
{
    struct FeelinDynamicAutoSave    *Next;
    ULONG                           *Save;
};

//+

/****************************************************************************
*** Debug *******************************************************************
****************************************************************************/

//#define DB_AUTO_FINDANCHOR
//#define DB_AUTO_CREATEANCHOR
//#define DB_AUTO_FINDENTRY
//#define DB_AUTO_CREATEENTRY
//#define DB_AUTO_CREATESAVE
//#define DB_AUTO_DELETESAVE
//#define DB_AUTO_ADDTABLE
//#define DB_AUTO_RESOLVE

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///f_dynamic_auto_str_cmp
STATIC LONG f_dynamic_auto_str_cmp(STRPTR Str1,STRPTR Str2)
{
    UBYTE c1, c2;

    do
    {
        c1 = *Str1++; if (c1 == '_' || c1 == '.') c1 = 0;
        c2 = *Str2++; if (c2 == '_' || c2 == '.') c2 = 0;
    }
    while (c1 == c2 && c1 && c2);

    return (LONG)(c1) - (LONG)(c2);
}
//+

///f_dynamic_auto_find_anchor
/*

    'Name' e.g. "Text_Xxx"

    When the function is called, semaphore are already locked.

*/

STATIC struct FeelinDynamicAutoAnchor * f_dynamic_auto_find_anchor(STRPTR Name,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoAnchor *anchor;

///DB_AUTO_FINDANCHOR
#ifdef DB_AUTO_FINDANCHOR
    F_Log("AutoFindAnchor - SEARCH (0x%lx) '%s'",Name,Name);
#endif
//+

    for (anchor = (APTR)(FeelinBase -> Dynamic.Head) ; anchor ; anchor = anchor -> Next)
    {
        LONG cmp = f_dynamic_auto_str_cmp(Name,anchor -> Name);

///DB_AUTO_FINDANCHOR
#ifdef DB_AUTO_FINDANCHOR
        F_Log("AutoFindAnchor - CMP '%s' & '%s' = %ld",Name,anchor -> Name,cmp);
#endif
//+

        if (cmp == 0)        break;
        else if (cmp < 0)    { anchor = NULL; break; }
    }

///DB_AUTO_FINDANCHOR
#ifdef DB_AUTO_FINDANCHOR
    if (anchor)
    {
        F_Log("AutoFindAnchor >> AutoA 0x%lx - '%s'",anchor,anchor -> Name);
    }
#endif
//+

    return anchor;
}
//+
///f_dynamic_auto_create_anchor
/*

Name e.g. "Text_Xxx"

*/

STATIC APTR f_dynamic_auto_create_anchor(STRPTR Name,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoAnchor *anchor;
    struct FeelinDynamicAutoAnchor *prev;
    ULONG len=0;

    F_OPool(FeelinBase -> DynamicPool);

    while (Name[len] != '_') len++;

    if ((anchor = F_NewP(FeelinBase -> DynamicPool,sizeof (struct FeelinDynamicAutoAnchor) + len + 1)) != NULL)
    {
        anchor -> Name = (APTR)((ULONG)(anchor) + sizeof (struct FeelinDynamicAutoAnchor));

        CopyMem(Name,anchor -> Name,len);

        for (prev = (APTR)(FeelinBase -> Dynamic.Tail) ; prev ; prev = prev -> Prev)
        {
            if (F_StrCmp(anchor -> Name,prev -> Name,ALL) > 0) break;
        }
        F_LinkInsert(&FeelinBase -> Dynamic,(FNode *) anchor,(FNode *) prev);

#ifdef DB_AUTO_CREATEANCHOR
        F_Log("AutoCreateAnchor - Anchor 0x%lx (0x%08.lx - 0x%08.lx) - '%s'",anchor,anchor -> Prev,anchor -> Next,anchor -> Name);

        {
            APTR next = FeelinBase -> Dynamic.Head;
            struct FeelinDynamicAutoAnchor *node;

            while (node = F_NextNode(&next))
            {
                F_DebugOut("             Anchor 0x%lx - '%s'\n",node,node -> Name);
                if (node == next)
                {
                    F_DebugOut("BAAAAAAAAAD\n");
                    break;
                }
            }
        }
#endif
    }

    F_RPool(FeelinBase -> DynamicPool);

    return anchor;
}
//+
///f_dynamic_auto_delete_anchor
STATIC void f_dynamic_auto_delete_anchor(struct FeelinDynamicAutoAnchor *AutoA,struct in_FeelinBase *FeelinBase)
{
    if (AutoA)
    {
        F_OPool(FeelinBase -> DynamicPool);

///DB_AUTO_DELETEANCHOR
#ifdef DB_AUTO_DELETEANCHOR
        F_DebugOut("AutoDeleteAnchor - AutoA 0x%lx - '%s'\n",AutoA,AutoA -> Name);
#endif
//+

        if (!AutoA -> Attributes.Head && !AutoA -> Methods.Head)
        {
            F_LinkRemove(&FeelinBase -> Dynamic,(FNode *) AutoA);
            F_Dispose(AutoA);
        }

        F_RPool(FeelinBase -> DynamicPool);
    }
}
//+

///f_dynamic_auto_find_entry
/*
    'Name' e.g. "FA_Text_Width"
*/
STATIC struct FeelinDynamicAutoEntry * f_dynamic_auto_find_entry(STRPTR Name,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoAnchor *anchor;
    struct FeelinDynamicAutoEntry  *autoe = NULL;

    if ((anchor = f_dynamic_auto_find_anchor(Name+3,FeelinBase)) != NULL)
    {
        APTR first = (Name[1] == 'A') ? anchor -> Attributes.Head : anchor -> Methods.Head;

        Name += 3; while (*Name != '_') Name++; Name++;

///DB_AUTO_FINDENTRY
#ifdef DB_AUTO_FINDENTRY
        F_DebugOut("AutoFindEntry - Anchor '%s' - Item '%s'\n",anchor -> Name,Name);
#endif
//+

        for (autoe = first ; autoe ; autoe = autoe -> Next)
        {
            LONG cmp = F_StrCmp(Name,autoe -> Name,ALL);

            if (cmp == 0)     break;
            else if (cmp < 0) { autoe = NULL; break; }
        }
///DB_AUTO_FINDENTRY
#ifdef DB_AUTO_FINDENTRY
        if (autoe)
        {
            F_DebugOut("AutoFindEntry - Entry 0x%lx - Name '%s'\n",autoe,autoe -> Name);
        }
        else
        {
            F_DebugOut("AutoFindEntry - NOT FOUND\n");
        }
#endif
//+
    }

    return autoe;
}
//+
///f_dynamic_auto_create_entry
/*///DOC

    'Name' e.g. "FM_Text_Draw"

    This function create a new struct FeelinDynamicAutoEntry  that  will  be
    linked to the approriate list of its appropriate anchor.

    Before calling this function you must be sure that no  other  Auto  Entry
    with  the  same name already exists, and the name passed as argument is a
    valid one.

    Only     the     function     DynamicAutoCreateSave     should      call
    DynamicAutoCreateEntry.  The  Dynamic  semaphore  must  be locked before
    calling the function because the function will not lock it to save time.

*///+
STATIC struct FeelinDynamicAutoEntry * f_dynamic_auto_create_entry(STRPTR Name,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoAnchor  *anchor;
    struct FeelinDynamicAutoEntry   *autoe = NULL;

    anchor = f_dynamic_auto_find_anchor(Name+3,FeelinBase);
    if (!anchor) anchor = f_dynamic_auto_create_anchor(Name+3,FeelinBase);

    if (anchor)
    {
        struct FeelinList *list = (Name[1] == 'A') ? &anchor -> Attributes : &anchor -> Methods;
        struct FeelinDynamicAutoEntry *prev;
        ULONG len;

        Name += 3 ; while (*Name != '_') Name++; Name++; len = F_StrLen(Name);
            
        if ((autoe = F_NewP(FeelinBase -> DynamicPool,sizeof (struct FeelinDynamicAutoEntry) + len + 1)) != NULL)
        {
            autoe -> Anchor      = anchor;
            autoe -> AnchorList  = list;
            autoe -> Name        = (APTR)((ULONG)(autoe) + sizeof (struct FeelinDynamicAutoEntry));

            CopyMem(Name,autoe -> Name,len);

            for (prev = (APTR)(list -> Tail) ; prev ; prev = prev -> Prev)
            {
                if (F_StrCmp(autoe -> Name,prev -> Name,ALL) > 0) break;
            }
            F_LinkInsert(list,(FNode *) autoe,(FNode *) prev);

///DB_AUTO_CREATEENTRY
#ifdef DB_AUTO_CREATEENTRY
            F_DebugOut("AutoCreateEntry - Anchor 0x%lx '%s' - AutoE 0x%lx - '%s'\n",anchor,anchor -> Name,autoe,autoe -> Name);
#endif
//+
        }
    }

    return autoe;
}
//+
///f_dynamic_auto_delete_entry
STATIC void f_dynamic_auto_delete_entry(struct FeelinDynamicAutoEntry *AutoE,struct in_FeelinBase *FeelinBase)
{
    if (AutoE)
    {
        struct FeelinDynamicAutoAnchor *anchor = AutoE -> Anchor;

///DB_AUTO_DELETEENTRY
#ifdef DB_AUTO_DELETEENTRY
        F_DebugOut("AutoDeleteEntry - AutoE 0x%lx - AutoA 0x%lx (0x%lx) '%s'\n",AutoE,AutoE -> Anchor,AutoE -> AnchorList);
#endif
//+

        F_LinkRemove(AutoE -> AnchorList,(FNode *) AutoE);
        F_Dispose(AutoE);

        if (!anchor -> Attributes.Head && !anchor -> Methods.Head)
        {
            f_dynamic_auto_delete_anchor(anchor,FeelinBase);
        }
    }
}
//+

///f_dynamic_auto_create_save
STATIC struct FeelinDynamicAutoSave * f_dynamic_auto_create_save(FDynamicEntry *entry,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoSave  *autos = NULL;
    struct FeelinDynamicAutoEntry *autoe;
    STRPTR                         name = entry -> Name;

    if (!name || name[0] != 'F' || (name[1] != 'A' && name[1] != 'M') || name[2] != '_')
    {
        F_Log(FV_LOG_CLASS,"F_DynamicCreateAutoSave() - Incorrect name '%s' (0x%08.lx)",name,name);
        return NULL;
    }

///DB_AUTO_CREATESAVE
#ifdef DB_AUTO_CREATESAVE
    F_DebugOut("[33mAutoCreateSave[0m - [1mEntry[0m 0x%lx - [1mStore[0m 0x%lx - '%s'\n",entry,&entry -> ID,entry -> Name);
#endif
//+

    autoe = f_dynamic_auto_find_entry(name,FeelinBase);
    if (!autoe) autoe = f_dynamic_auto_create_entry(name,FeelinBase);

    if (autoe)
    {
        if ((autos = F_NewP(FeelinBase -> DynamicPool,sizeof (struct FeelinDynamicAutoSave))) != NULL)
        {
            autos -> Save = &entry -> ID;

            /*** link ***/

            if (autoe -> FirstSave)
            {
                struct FeelinDynamicAutoSave *prev = autoe -> FirstSave;

                while (prev -> Next) prev = prev -> Next;
                prev -> Next = autos;
            }
            else
            {
                autoe -> FirstSave = autos;
            }

/* if the correct class is loaded we can already resolve some IDs */

            if (f_dynamic_find_name(name+3,FeelinBase))
            {
                *autos -> Save = F_DynamicFindID(name);
            }
        }
///DB_AUTO_CREATESAVE
#ifdef DB_AUTO_CREATESAVE
        F_DebugOut("AutoCreateSave - AutoS 0x%lx - Store 0x%lx - AutoE 0x%lx '%s' - AutoA 0x%lx '%s'\n",autos,autos -> Save,autoe,autoe -> Name,autoe -> Anchor,autoe -> Anchor -> Name);
#endif
//+
    }

    return autos;
}
//+
///f_dynamic_auto_delete_save
STATIC void f_dynamic_auto_delete_save(struct FeelinDynamicEntry *entry,struct in_FeelinBase *FeelinBase)
{
    struct FeelinDynamicAutoSave  *autos;
    struct FeelinDynamicAutoEntry *autoe;
    STRPTR                         name = entry -> Name;

    if (!name || name[0] != 'F' || (name[1] != 'A' && name[1] != 'M') || name[2] != '_')
    {
        F_Log(FV_LOG_CLASS,"F_DynamicCreateAutoSave() - Incorrect name '%s' (0x%08.lx)",name,name);
        return;
    }

///DB_AUTO_DELETESAVE
#ifdef DB_AUTO_DELETESAVE
    F_DebugOut("Auto_DeleteSave - Entry 0x%lx - IDAddr 0x%lx - '%s'\n",entry,&entry -> ID,entry -> Name);
#endif
//+

    if ((autoe = f_dynamic_auto_find_entry(name,FeelinBase)) != NULL)
    {
        struct FeelinDynamicAutoSave *prev = NULL;

///DB_AUTO_DELETESAVE
#ifdef DB_AUTO_DELETESAVE
        F_DebugOut("Auto_DeleteSave - AutoE 0x%lx '%s'\n",autoe,autoe -> Name);
#endif
//+

        for (autos = autoe -> FirstSave ; autos ; autos = autos -> Next)
        {
            if (autos -> Save == &entry -> ID)
            {
///DB_AUTO_DELETESAVE
#ifdef DB_AUTO_DELETESAVE
                F_DebugOut("Auto_DeleteSave - FOUND - Save 0x%lx\n",autos);
#endif
//+
                if (prev)   prev -> Next = autos -> Next;
                else        autoe -> FirstSave = autos -> Next;

                *autos -> Save = 0;

                F_Dispose(autos);

                break;
            }
            prev = autos;
        }

        if (!autoe -> FirstSave)
        {
            f_dynamic_auto_delete_entry(autoe,FeelinBase);
        }
    }
}
//+

/****************************************************************************
*** Shared ******************************************************************
****************************************************************************/

///f_dynamic_auto_add_class
void f_dynamic_auto_add_class
(
    struct FeelinClass *Class,
    struct in_FeelinBase *FeelinBase
)
{
    struct FeelinDynamicAutoAnchor *autoa;
    struct FeelinDynamicAutoEntry  *autoe;
    struct FeelinDynamicAutoSave   *autos;

    if (!Class) return;

    F_OPool(FeelinBase -> ClassesPool);
    F_OPool(FeelinBase -> DynamicPool);

    if ((autoa = f_dynamic_auto_find_anchor(Class -> Name,FeelinBase)) != NULL)
    {
        if (autoa -> Attributes.Head)
        {
            if (Class -> Attributes)
            {
                for (autoe = (struct FeelinDynamicAutoEntry *)(autoa -> Attributes.Head) ; autoe ; autoe = autoe -> Next)
                {
                    FClassAttribute *entry;
                    BOOL found = FALSE;

                    for (entry = Class -> Attributes ; entry -> Name ; entry++)
                    {
                        if (F_StrCmp(autoe -> Name,entry -> Name,ALL) == 0)
                        {
                            found = TRUE;

                            for (autos = autoe -> FirstSave ; autos ; autos = autos -> Next)
                            {
                                *autos -> Save = entry -> ID;
///DB_AUTO_RESOLVE
#ifdef DB_AUTO_RESOLVE
                    F_DebugOut("AutoResolve - F%lc_%s_%s 0x%lx\n",(autoe -> AnchorList == &autoe -> Anchor -> Attributes) ? 'A' : 'M',autoe -> Anchor -> Name,autoe -> Name,*autos -> Save);
#endif
//+
                            }
                        }
                    }

                    if (found == FALSE)
                    {
                        F_Log(FV_LOG_CLASS,"F_AutoDynamic() - 'F%lc_%s_%s' not defined !!",(autoe -> AnchorList == &autoe -> Anchor -> Attributes) ? 'A' : 'M',autoe -> Anchor -> Name,autoe -> Name);
                    }
                }
            }
            else
            {
                struct FeelinDynamicAutoEntry *autoe;

                F_Log(FV_LOG_CLASS,"%s'%s' does not define any %s","F_DynamicAuto() - ",Class -> Name,"attribute");

                for (autoe = (struct FeelinDynamicAutoEntry *)(autoa -> Attributes.Head) ; autoe ; autoe = autoe -> Next)
                {
                    F_Log(FV_LOG_CLASS,"%sUnable to resolve 'F%lc_%s_%s'","F_DynamicAuto() - ",'A',autoa -> Name,autoe -> Name);
                }
            }
        }

        if (autoa -> Methods.Head)
        {
            if (Class -> Methods)
            {
                for (autoe = (struct FeelinDynamicAutoEntry *)(autoa -> Methods.Head) ; autoe ; autoe = autoe -> Next)
                {
                    FClassMethod *entry;
                    BOOL found = FALSE;

                    for (entry = Class -> Methods ; entry -> Function ; entry++)
                    {
                        if (entry -> Name)
                        {
                            if (F_StrCmp(autoe -> Name,entry -> Name,ALL) == 0)
                            {
                                found = TRUE;

                                for (autos = autoe -> FirstSave ; autos ; autos = autos -> Next)
                                {
                                    *autos -> Save = entry -> ID;
///DB_AUTO_RESOLVE
#ifdef DB_AUTO_RESOLVE
                                    F_DebugOut("AutoResolve - F%lc_%s_%s 0x%lx\n",(autoe -> AnchorList == &autoe -> Anchor -> Attributes) ? 'A' : 'M',autoe -> Anchor -> Name,autoe -> Name,*autos -> Save);
#endif
//+
                                }
                            }
                        }
                    }

                    if (found == FALSE)
                    {
                        F_Log(FV_LOG_CLASS,"F_AutoDynamic() - 'F%lc_%s_%s' not defined !!",(autoe -> AnchorList == &autoe -> Anchor -> Attributes) ? 'A' : 'M',autoe -> Anchor -> Name,autoe -> Name);
                    }
                }
            }
            else
            {
                struct FeelinDynamicAutoEntry *autoe;

                F_Log(FV_LOG_CLASS,"%s'%s' does not define any %s","F_DynamicAuto() - ",Class -> Name,"methods");

                for (autoe = (struct FeelinDynamicAutoEntry *)(autoa -> Methods.Head) ; autoe ; autoe = autoe -> Next)
                {
                    F_Log(FV_LOG_CLASS,"%sUnable to resolve 'F%lc_%s_%s'","F_DynamicAuto() - ",'M',autoa -> Name,autoe -> Name);
                }
            }
        }
    }

    F_RPool(FeelinBase -> DynamicPool);
    F_RPool(FeelinBase -> ClassesPool);
}
//+

/****************************************************************************
*** Functions ***************************************************************
****************************************************************************/

///f_dynamic_add_auto_table
F_LIB_DYNAMIC_ADD_AUTO_TABLE
{
    BOOL ok = TRUE;

    F_OPool(FeelinBase -> DynamicPool);

    if (Table)
    {
        struct FeelinDynamicEntry *entry = Table;

        while (entry -> Name)
        {
            if (f_dynamic_auto_create_save(entry,FeelinBase))
            {
                entry++;
            }
            else
            {
                ok = FALSE; break;
            }
        }

        if (ok == FALSE)
        {
            struct FeelinDynamicEntry *stop = entry;

            entry = Table;

            while (entry -> Name)
            {
                if (entry == stop) break;

                f_dynamic_auto_delete_save(entry++,FeelinBase);
            }
        }
///DB_AUTO_ADDTABLE
#ifdef DB_AUTO_ADDTABLE
        else
        {
            struct FeelinDynamicAutoAnchor *autoa;
            APTR nexta = FeelinBase -> Dynamic.Head;

            while (autoa = F_NextNode(&nexta))
            {
                struct FeelinDynamicAutoEntry *autoe;
                APTR nexte;

                F_Log("FC_%-26.s (M: 0x%lx - A: 0x%lx)",autoa -> Name,autoa -> Attributes.Head,autoa -> Methods.Head);

                if (nexte = autoa -> Attributes.Head)
                {
                    if (nexta == autoa)
                    {
                        F_Log("BAAAAAAAAAAAAAAAAAAAAAAAD_AUTOA"); break;
                    }

                    while (autoe = F_NextNode(&nexte))
                    {
                        struct FeelinDynamicAutoSave *autos;

                        F_Log("   FA_%-23.s (0x%lx)",autoe -> Name,autoe);

                        if (nexte == autoe)
                        {
                            F_Log("BAAAAAAAAAAAAAAAAAAAAAAAD_AUTOE_1"); break;
                        }

                        for (autos = autoe -> FirstSave ; autos ; autos = autos -> Next)
                        {
                            F_Log("      AutoS 0x%lx - Store 0x%lx",autos,autos -> Save);
                        }
                    }
                }

                if (nexte = autoa -> Methods.Head)
                {
                    while (autoe = F_NextNode(&nexte))
                    {
                        struct FeelinDynamicAutoSave *autos;

                        F_Log("   FM_%-23.s (0x%lx)",autoe -> Name,autoe);

                        if (nexte == autoe)
                        {
                            F_Log("BAAAAAAAAAAAAAAAAAAAAAAAD_AUTOE_2"); break;
                        }

                        for (autos = autoe -> FirstSave ; autos ; autos = autos -> Next)
                        {
                            F_Log("      AutoS 0x%lx - Store 0x%lx",autos,autos -> Save);
                        }
                    }
                }
            }
        }
#endif
//+
    }

    F_RPool(FeelinBase -> DynamicPool);

    return ok;
}
F_LIB_END
//+
///f_dynamic_rem_auto_table
F_LIB_DYNAMIC_REM_AUTO_TABLE
{
    if (Table)
    {
        F_OPool(FeelinBase -> DynamicPool);

        while (Table -> Name)
        {
            f_dynamic_auto_delete_save(Table++,FeelinBase);
        }

        F_RPool(FeelinBase -> DynamicPool);
    }
}
F_LIB_END
//+

