/*

$VER: 02.00 (2005/08/10)

    Portability update.
    
    Better requester layout and ergonomy.

$VER: 01.00 (2005/04/14)

*/

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,FileChooser_Update);
F_METHOD_PROTO(void,FileChooser_Activate);

F_METHOD_PROTO(void,FileChooser_New);
F_METHOD_PROTO(void,FileChooser_Get);
F_METHOD_PROTO(void,FileChooser_Set);

F_METHOD_PROTO(void,FileChooser_Setup);
//+

/*

FA_FileChooser_Action [I.G]
                                
    Set the type of operation that the chooser is performing. The layout is
    adapted according to the action.
 
    FV_FileChooser_Action_Open, FV_FileChooser_Action_Save
    
    Show a "create folder" option in "Save" mode.
    
FA_FileChooser_Multiple [I.G]

    Multiple file selection, anly available in "OPEN" mode.
    
FA_FileChooser_PreviewObject [ISG]

    Set developer supplied preview object used to display a preview of  the
    currently selected file.
    
FA_FileChooser_Shortcuts [ISG]

    Custom directory shortcuts to add to the volume list.

*/

///f_str_newpA
STATIC STRPTR f_str_newpA
(
   APTR Pool,
   uint32 * LenPtr,
   STRPTR Fmt,
   uint32 *Params,
   struct FeelinBase *FeelinBase
)
{
   static const uint32 len_func = 0x52934E75; /* ADDQ.L  #1,(A3) ; RTS */
   static const uint32 put_func = 0x16C04E75; /* MOVE.B d0,(a3)+ ; RTS */

   STRPTR string = NULL;
   uint32 len = 0;

   RawDoFmt(Fmt,Params,(void *)(&len_func),&len);

   if (len > 1)
   {
      if (string = F_NewP(Pool,len))
      {
         RawDoFmt(Fmt,Params,(void *)(&put_func),string);
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
STATIC STRPTR f_str_newp(struct FeelinBase *FeelinBase,APTR Pool,uint32 * LenPtr,STRPTR Fmt,...)
{
   va_list params;
   va_start(params,Fmt);

   return f_str_newpA(Pool,LenPtr,Fmt,(uint32 *)(params),FeelinBase);
}
//+

///code_list_volumes_construct
F_HOOKM(APTR,code_list_volumes_construct,FS_List_Construct)
{
    struct ListEntryRaw *ler = Msg -> Entry;
    struct ListEntry *le=NULL;

    if (ler -> Type == FV_ENTRY_VOLUME || ler -> Type == FV_ENTRY_ASSIGN)
    {
        if ((le = F_NewP(Msg -> Pool,sizeof (struct ListEntry))))
        {
            STRPTR name = BADDR(((struct DosList *)(ler -> Data)) -> dol_Name);

            le -> Type = ler -> Type;
            
            if (le -> Type == FV_ENTRY_VOLUME)
            {
                le -> Name = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s (%s)",name + 1,((struct Task *)(((struct DosList *)(ler -> Data)) -> dol_Task -> mp_SigTask)) -> tc_Node.ln_Name);
            }
            else
            {
                le -> Name = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s",name + 1);
            }
 
/*
            le -> Name = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s:",name + 1);

//            F_Log(0,"NAME (%s)(%ld)",le -> Name,le -> Type);

            if (ler -> Type == FV_ENTRY_VOLUME)
            {
                BPTR lock;

                le -> DevName = f_str_newp(FeelinBase,Msg -> Pool,NULL,"%s:",((struct Task *)(((struct DosList *)(ler -> Data)) -> dol_Task -> mp_SigTask)) -> tc_Node.ln_Name);

                if ((lock = Lock(le -> DevName,SHARED_LOCK)))
                {
                  struct InfoData id;

                  if (Info(lock, &id) == DOSTRUE)
                  {
                     le -> Size = id.id_NumBlocksUsed * 100 /  id.id_NumBlocks;

   //F_Log(0,"size %ld%%",le -> Size);
   //                  F_Log(0,"nb(%ld) bpb(%ld) nbu(%ld)",id.id_NumBlocks,id.id_BytesPerBlock,id.id_NumBlocksUsed);
                  }
               }
            }
            else
            {
               le -> Lock = ((struct DosList *)(ler -> Data)) -> dol_Lock;
   //            F_Log(0,"assign lock 0x%08lx",le -> Lock);
            }
*/
        }
    }
    else if (ler -> Type == FV_ENTRY_VOLUME_SEPARATOR || ler -> Type == FV_ENTRY_DIRECTORY_SEPARATOR)
    {
       if ((le = F_NewP(Msg -> Pool,sizeof (struct ListEntry))))
       {
          le -> Type = ler -> Type;
       }
    }
    else if (ler -> Type == FV_ENTRY_DIRECTORY || ler -> Type == FV_ENTRY_FILE)
    {
       if ((le = F_NewP(Msg -> Pool,sizeof (struct ListEntry))))
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
///code_list_volumes_display
F_HOOKM(FListDisplay *,code_list_volumes_display,FS_List_Display)
{
    struct ListEntry *le = Msg -> Entry;

    if (le)
    {
        switch (le -> Type)
        {
            case FV_ENTRY_VOLUME:
            {
//                F_StrFmt(Hook -> h_Data,"%ld%% used",le -> Size);

                Msg -> Strings[0] = le -> Name;
//                Msg -> Strings[1] = le -> DevName;
//                Msg -> Strings[2] = Hook -> h_Data;

//                Msg -> PreParses[0] = "<image type='picture' src='Feelin:Images/FileChooser/back.png' />";
 
//                Msg -> PreParses[1] = "<pens style=ghost>";
//                Msg -> PreParses[2] = "<align=right>";
            }
            break;

            case FV_ENTRY_VOLUME_SEPARATOR:
            {
                Msg -> Strings[0] = "<hr compact=true>";
//                Msg -> Strings[1] = "<hr compact=true>";
//                Msg -> Strings[2] = "<hr compact=true>";
            }
            break;

            case FV_ENTRY_ASSIGN:
            {
                Msg -> Strings[0] = le -> Name;
//                Msg -> Strings[2] = "(assign)";

                Msg -> PreParses[0] = "<pens text=highlight>";
//                Msg -> PreParses[2] = "<align=right><pens style=ghost>";
            }
            break;
        }
    }
    return NULL;
}
//+

///code_list_destruct
F_HOOKM(void,code_list_destruct,FS_List_Destruct)
{
   struct ListEntry *le=Msg -> Entry;

   if (le)
   {
      F_DisposeP(Msg -> Pool,le -> Name);
      F_DisposeP(Msg -> Pool,le -> DevName);
      F_DisposeP(Msg -> Pool,le);
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

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
   
   CUD -> hook_destruct.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(code_list_destruct);
   CUD -> hook_compare.h_Entry   = (HOOKFUNC) F_FUNCTION_GATE(code_list_compare);

   CUD -> hook_volumes_construct.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_list_volumes_construct);
   CUD -> hook_volumes_display.h_Entry   = (HOOKFUNC) F_FUNCTION_GATE(code_list_volumes_display);
   CUD -> hook_volumes_display.h_Data    = &CUD -> buf;

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
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Path", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("File", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("FilterIcons", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("FilterFiles", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Pattern", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Choosed", FV_TYPE_BOOLEAN),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(FileChooser_Update, "Private1"),
                F_METHODS_ADD(FileChooser_Activate, "Private2"),
                
                F_METHODS_ADD_STATIC(FileChooser_New, FM_New),
//                F_METHODS_ADD_STATIC(FileChooser_Get, FM_Get),
//                F_METHODS_ADD_STATIC(FileChooser_Set, FM_Set),
                
                F_METHODS_ADD_STATIC(FileChooser_Setup, FM_Setup),

                F_ARRAY_END
            };
             
            STATIC F_AUTOS_ARRAY =
            {
               F_AUTOS_ADD("FA_List_Quiet"),
               F_AUTOS_ADD("FA_List_Format"),
               F_AUTOS_ADD("FA_List_Activation"),
               F_AUTOS_ADD("FM_List_InsertSingle"),
               F_AUTOS_ADD("FM_List_Remove"),
               F_AUTOS_ADD("FM_List_GetEntry"),

               F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_AUTOS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
