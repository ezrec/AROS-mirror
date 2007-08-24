/*

$VER: 02.00 (2005/08/10)
 
   Portability update.
   
   Metaclass support.

$VER: 01.00 (2005/04/14)

   This subclass of FC_List is used  to  travel  through  volumes,  assign,
   directories and files with a lovely list.

*/

#include "Project.h"

struct ClassUserData               *CUD;
 
///METHODS
F_METHOD(void,DOSList_ChangePath);

F_METHOD(void,DOSList_New);
F_METHOD(void,DOSList_Dispose);
F_METHOD(void,DOSList_Get);
F_METHOD(void,DOSList_Set);
F_METHOD(void,DOSList_HandleEvent);
//+

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

#ifdef __AROS__
#include <aros/asmcall.h>

AROS_UFH2S(void, put_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(STRPTR *, strPtrPtr, A3))
{
    AROS_USERFUNC_INIT
    
    *(*strPtrPtr)++ = chr;
    
    AROS_USERFUNC_EXIT
}

AROS_UFH2S(void, len_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(ULONG *, lenPtr, A3))
{
    AROS_USERFUNC_INIT
    
    (*lenPtr)++;
    
    AROS_USERFUNC_EXIT
}
#endif

///f_str_newpA
STATIC STRPTR f_str_newpA
(
   APTR Pool,
   ULONG * LenPtr,
   STRPTR Fmt,
   ULONG *Params,
   struct FeelinBase *FeelinBase
)
{
#ifndef __AROS__
   static const uint32 len_func = 0x52934E75; /* ADDQ.L  #1,(A3) ; RTS */
   static const uint32 put_func = 0x16C04E75; /* MOVE.B d0,(a3)+ ; RTS */
#endif

   STRPTR string = NULL;
   ULONG len = 0;

#ifdef __AROS__
   RawDoFmt(Fmt,Params,(VOID_FUNC)AROS_ASMSYMNAME(len_func),&len);
#else
   RawDoFmt(Fmt,Params,(void *)(&len_func),&len);
#endif

   if (len > 1)
   {
      if (string = F_NewP(Pool,len))
      {
      #ifdef __AROS__
      	 STRPTR stringptr = string;
	 
         RawDoFmt(Fmt,Params,(VOID_FUNC)AROS_ASMSYMNAME(put_func),&stringptr);
      #else
         RawDoFmt(Fmt,Params,(void *)(&put_func),string);
      #endif
      }
   }

   if (LenPtr)
   {
      *LenPtr = len - 1;
   }

   return string;
}
//+
///f_str_newp
STATIC STRPTR f_str_newp(struct FeelinBase *FeelinBase,APTR Pool,ULONG * LenPtr,STRPTR Fmt,...)
{
   va_list params;
   va_start(params,Fmt);

   return f_str_newpA(Pool,LenPtr,Fmt,(ULONG *)(params),FeelinBase);
}
//+

///code_list_construct
F_HOOKM(APTR,code_list_construct,FS_List_Construct)
{
    struct ListEntryRaw *ler = Msg -> Entry;
    struct ListEntry *le=NULL;

    if (ler -> Type == FV_ENTRY_VOLUME || ler -> Type == FV_ENTRY_ASSIGN)
    {
        le = F_NewP(Msg -> Pool,sizeof (struct ListEntry));
 
        if (le)
        {
//	#ifdef __AROS__
//            STRPTR name = BADDR(((struct DosList *)(ler -> Data)) -> dol_DevName - 1);
//	#else
            STRPTR name = BADDR(((struct DosList *)(ler -> Data)) -> dol_Name);
//    	#endif
            le -> Type = ler -> Type;
            le -> Name = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s:",name + 1);

            if (ler -> Type == FV_ENTRY_VOLUME)
            {
//	    #ifdef __AROS__
//                le -> DevName = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s:", "AROS_FIXME");
//	    #else
                le -> DevName = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s:",((struct Task *)(((struct DosList *)(ler -> Data)) -> dol_Task -> mp_SigTask)) -> tc_Node.ln_Name);
//            #endif               
//                F_Log(0,"devname (0x%08lx)(%s)",le->DevName);
 
                if (le->DevName)
                {
                    BPTR lock = Lock(le -> DevName,SHARED_LOCK);
 
                    if (lock)
                    {
                        struct InfoData id;

                        if (Info(lock, &id) == DOSTRUE)
                        {
                            le -> Size = id.id_NumBlocksUsed * 100 /  id.id_NumBlocks;
                        }
                    }
                }
            }
            else
            {
                le -> Lock = ((struct DosList *)(ler -> Data)) -> dol_Lock;
            }
        }
    }
    else if (ler -> Type == FV_ENTRY_VOLUME_SEPARATOR || ler -> Type == FV_ENTRY_DIRECTORY_SEPARATOR)
    {
        le = F_NewP(Msg -> Pool,sizeof (struct ListEntry));

        if (le)
        {
            le -> Type = ler -> Type;
        }
    }
    else if (ler -> Type == FV_ENTRY_DIRECTORY || ler -> Type == FV_ENTRY_FILE)
    {
        le = F_NewP(Msg -> Pool,sizeof (struct ListEntry));

        if (le)
        {
            STRPTR name = (STRPTR)(&((struct FileInfoBlock *)(ler -> Data)) -> fib_FileName);

            le -> Type = ler -> Type;
            le -> Name = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s",name);
            le -> Size = ((struct FileInfoBlock *)(ler -> Data)) -> fib_Size;
        }
    }

    return le;
}
//+
///code_list_destruct
F_HOOKM(void,code_list_destruct,FS_List_Destruct)
{
    struct ListEntry *le = Msg -> Entry;

    if (le)
    {
        F_Dispose(le -> Name);
        F_Dispose(le -> DevName);
        F_Dispose(le);
    }
}
//+
///code_list_compare
F_HOOKM(LONG,code_list_compare,FS_List_Compare)
{
    struct ListEntry *e1 = Msg -> Entry;
    struct ListEntry *e2 = Msg -> Other;

    if (e1 -> Type == e2 -> Type)
    {
        return Stricmp(e1 -> Name,e2 -> Name);
    }
    else if (e1 -> Type < e2 -> Type)
    {
        return -1;
    }
    return 1;
}
//+
///code_list_display
F_HOOKM(FListDisplay *,code_list_display,FS_List_Display)
{
    struct ListEntry *le = Msg -> Entry;

    if (le)
    {
        switch (le -> Type)
        {
            case FV_ENTRY_VOLUME:
            {
                F_StrFmt(Hook -> h_Data,"%ld%% used",le -> Size);

                Msg -> Strings[0] = le -> Name;
                Msg -> Strings[1] = le -> DevName;
                Msg -> Strings[2] = Hook -> h_Data;

                Msg -> PreParses[1] = "<pens style=ghost>";
                Msg -> PreParses[2] = "<align=right>";
            }
            break;

            case FV_ENTRY_VOLUME_SEPARATOR:
            {
                Msg -> Strings[0] = "<hr compact=true>";
                Msg -> Strings[1] = "<hr compact=true>";
                Msg -> Strings[2] = "<hr compact=true>";
            }
            break;

            case FV_ENTRY_ASSIGN:
            {
                Msg -> Strings[0] = le -> Name;
                Msg -> Strings[2] = "(assign)";

                Msg -> PreParses[0] = "<pens text=highlight>";
                Msg -> PreParses[2] = "<align=right><pens style=ghost>";
            }
            break;

            case FV_ENTRY_DIRECTORY:
            {
                Msg -> Strings[0] = le -> Name;
                Msg -> Strings[1] = "(dir)";

                Msg -> PreParses[0] = "<pens text=highlight>";
                Msg -> PreParses[1] = "<align=right><pens style=ghost>";
            }
            break;

            case FV_ENTRY_DIRECTORY_SEPARATOR:
            {
                Msg -> Strings[0] = "<hr compact=true>";
                Msg -> Strings[1] = "<hr compact=true>";
            }
            break;

            case FV_ENTRY_FILE:
            {
                F_StrFmt(Hook -> h_Data,"%ld",le -> Size);

                Msg -> Strings[0] = le -> Name;
                Msg -> Strings[1] = Hook -> h_Data;

                Msg -> PreParses[1] = "<align=right>";
            }
            break;
        }
    }
    return NULL;
}
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    CUD = F_LOD(Class,Obj);
  
    CUD -> construct_hook.h_Entry = (HOOKFUNC) code_list_construct;
    CUD -> destruct_hook.h_Entry  = (HOOKFUNC) code_list_destruct;
    CUD -> compare_hook.h_Entry   = (HOOKFUNC) code_list_compare;
    CUD -> display_hook.h_Entry   = (HOOKFUNC) code_list_display;
    CUD -> display_hook.h_Data    = &CUD -> buf;

    return (FObject) F_SUPERDO();
}
//+

F_QUERY()
{
   switch (Which)
   {
///Meta
      case FV_Query_MetaClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Class_New, FM_New),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Class),
            F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_VALUES_ARRAY(Path) =
         {
            F_VALUES_ADD("Volumes", FV_DOSList_Path_Volumes),
            F_VALUES_ADD("Parent",  FV_DOSList_Path_Parent),
            F_VALUES_ADD("Prog",    FV_DOSList_Path_Prog),
            F_VALUES_ADD("Work",    FV_DOSList_Path_Work),

            F_ARRAY_END
         };

         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD_WITH_VALUES("Path", FV_TYPE_STRING,Path),
            F_ATTRIBUTES_ADD("File", FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("FilterIcons", FV_TYPE_BOOLEAN),
            F_ATTRIBUTES_ADD("FilterFiles", FV_TYPE_BOOLEAN),
            F_ATTRIBUTES_ADD("FilterHook", FV_TYPE_POINTER),
            F_ATTRIBUTES_ADD("Pattern", FV_TYPE_STRING),

            F_ARRAY_END
         };

         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD(DOSList_ChangePath,"ChangePath"),
            F_METHODS_ADD_STATIC(DOSList_New,FM_New),
            F_METHODS_ADD_STATIC(DOSList_Dispose,FM_Dispose),
            F_METHODS_ADD_STATIC(DOSList_Get,FM_Get),
            F_METHODS_ADD_STATIC(DOSList_Set,FM_Set),
            F_METHODS_ADD_STATIC(DOSList_HandleEvent,FM_HandleEvent),

            F_ARRAY_END
         };

         STATIC F_RESOLVES_ARRAY =
         {
            F_RESOLVES_ADD("FA_List_Quiet"),
            F_RESOLVES_ADD("FA_List_Format"),
            F_RESOLVES_ADD("FM_List_InsertSingle"),
            F_RESOLVES_ADD("FM_List_Remove"),
            F_RESOLVES_ADD("FM_List_GetEntry"),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(List),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_ATTRIBUTES,
            F_TAGS_ADD_RESOLVES,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
   }
   return NULL;
}

