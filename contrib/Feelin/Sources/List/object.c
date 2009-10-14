#include "Private.h"

/*** Methods ***************************************************************/

///List_New
F_METHODM(APTR,List_New,TagItem)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    APTR *entries = NULL;
    BOOL readonly = FALSE;

    F_AREA_SAVE_PUBLIC;

    LOD -> PoolItemSize     = 1024;
    LOD -> PoolItemNumber   = 4;
    LOD -> ColumnCount      = 1;
    LOD -> SortMode         = FV_List_Sort_Descending;
    LOD -> Activation       = FV_List_Activation_Unknown;
    LOD -> Steps            = 1;

    LOD -> p_Spacing        = "$list-spacing";
    LOD -> p_CursorActive   = "$list-cursor-active";
    LOD -> p_Steps          = "$list-steps";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_List_ReadOnly: readonly = item.ti_Data; break;
    };

    LOD -> p_title_back   = (readonly) ? "$list-read-title-back"   : "$list-title-back";
    LOD -> p_title_frame  = (readonly) ? "$list-read-title-frame"  : "$list-title-frame";
    LOD -> p_title_scheme = (readonly) ? "$list-read-title-scheme" : "$list-title-scheme";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_List_Pool:               LOD -> Pool             = (APTR)(item.ti_Data); break;
        case FA_List_PoolItemSize:       LOD -> PoolItemSize     = item.ti_Data; break;
        case FA_List_PoolItemNumber:     LOD -> PoolItemNumber   = item.ti_Data; break;

        case FA_List_SourceArray:        entries  = (APTR *)(item.ti_Data); break;

        case FA_List_Spacing:            LOD -> p_Spacing = (STRPTR)(item.ti_Data); break;
        case FA_List_Steps:              LOD -> p_Steps = (STRPTR)(item.ti_Data); break;

        case FA_List_CursorActive:       LOD -> p_CursorActive = (STRPTR)(item.ti_Data); break;
    }

/*** create / obtain memory pools ******************************************/

    LOD -> LinePool = F_CreatePool(1024,FA_Pool_Items,1,FA_Pool_Name,"list-lines",TAG_DONE);

    if (!LOD -> LinePool)
    {
        F_Log(FV_LOG_DEV,"Unable to create Lines Pool");
        return NULL;
    }

    if (!LOD -> Pool)
    {
        LOD -> Pool = F_CreatePool(LOD -> PoolItemSize,FA_Pool_Items,LOD -> PoolItemNumber,FA_Pool_Name,"List.Data",TAG_DONE);
        LOD -> Flags |= FF_LIST_OWNPOOL;
    }

    if (!LOD -> Pool)
    {
        F_Log(FV_LOG_DEV,"Unable to CreatePool");
        return NULL;
    }

    LOD -> TDObj = TextDisplayObject,

        FA_TextDisplay_Shortcut, FALSE,

        End;

    if (LOD->TDObj == NULL)
    {
        return NULL;
    }

    if (F_SuperDo(Class,Obj,Method,

        FA_ChainToCycle,  FALSE,
        FA_MinWidth,      20,
        FA_NoFill,        TRUE,

        FA_Back,          (readonly) ? "$list-read-back"   : "$list-back",
        FA_Font,          (readonly) ? "$list-read-font"   : "$list-font",
        FA_Frame,         (readonly) ? "$list-read-frame"  : "$list-frame",
        FA_ColorScheme,   (readonly) ? "$list-read-scheme" : "$list-scheme",

        F_IDA(FA_List_Format),  "<col />",

        TAG_MORE, Msg))
    {
        if (LOD -> TitleBar)
        {
            if (entries)
            {
                F_Do(Obj,F_IDM(FM_List_Insert),entries,ALL,FV_List_Insert_Top);
            }
            return Obj;
        }
    }
    return NULL;
}
//+
///List_Dispose
F_METHOD(void,List_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    titlebar_dispose(Class,Obj);

    if (LOD -> TDObj)
    {
        F_DisposeObj(LOD -> TDObj); LOD -> TDObj = NULL;
    }

    if (LOD -> Pool && (FF_LIST_OWNPOOL & LOD -> Flags))
    {
        F_DeletePool(LOD -> Pool); LOD -> Pool = NULL;
    }

    if (LOD -> LinePool)
    {
        F_DeletePool(LOD -> LinePool); LOD -> LinePool = NULL;
    }

    F_SUPERDO();
}
//+
///List_Get
F_METHOD(void,List_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_List_Active:       F_STORE((LOD -> Active) ? LOD -> Active -> Position : FV_List_NotVisible); break;
        case FA_List_Activation:   F_STORE(LOD -> Activation); break;
        case FA_List_Entries:      F_STORE(LOD -> LineCount); break;
        case FA_List_Visible:      F_STORE(LOD -> Visible); break;
        case FA_List_First:        F_STORE((LOD -> First) ? LOD -> First -> Position : FV_List_NotVisible); break;
        case FA_List_Last:         F_STORE((LOD -> Last) ? LOD -> Last -> Position : FV_List_NotVisible); break;
        case FA_List_Title:        F_STORE((LOD -> TitleBar && LOD -> ColumnCount == 1) ? (LOD -> TitleBar -> Strings[0]) : NULL); break;
        case FA_List_Steps:        F_STORE(LOD -> Steps); break;
        case FA_List_SortMode:     F_STORE(LOD -> SortMode); break;
    }

    F_SUPERDO();
}
//+
///List_Set
F_METHOD(void,List_Set)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,*tag,item;

   while  ((tag = F_DynamicNTI(&Tags,&item,Class)))
   switch (item.ti_Tag)
   {
      case FA_List_CompareHook:     LOD -> Hook_Compare     = (struct Hook *)(item.ti_Data); break;
      case FA_List_ConstructHook:   LOD -> Hook_Construct   = (struct Hook *)(item.ti_Data); break;
      case FA_List_DestructHook:    LOD -> Hook_Destruct    = (struct Hook *)(item.ti_Data); break;
      case FA_List_DisplayHook:     LOD -> Hook_Display     = (struct Hook *)(item.ti_Data); break;

      case FA_List_Format:
      {
         LOD -> LastFirst = NULL;

         titlebar_new(Class,Obj,(STRPTR)(item.ti_Data));
      }
      break;

      case FA_List_Title:
      {
         if (LOD -> TitleBar && LOD -> ColumnCount == 1)
         {
            LOD -> TitleBar -> Strings[0] = (STRPTR)(item.ti_Data);
         }
      }
      break;

      case FA_List_First:
      {
         FLine *line = LOD -> First;
         LONG pos;

         pos = MAX((LONG)(item.ti_Data),0);
         pos = MIN(pos,LOD -> LineCount - 1);

         list_adjust_first(Class,Obj,pos);

         if (LOD -> Quiet >= 0 && line != LOD -> First)
         {
            F_Draw(Obj,FF_Draw_Update);
         }
      }
      break;

      case FA_List_Active:
      {
         FLine *active = LOD -> Active;

         if (!(FF_LIST_ACTIVATION_INTERNAL & LOD -> Flags))
         {
            LOD -> Activation = FV_List_Activation_External;

            LOD -> Flags &= ~FF_LIST_ACTIVATION_INTERNAL;
         }

//         F_Log(0,"ACTIVE %ld - VISIBLE_FIRST 0x%08lx (%ld) - VISIBLE_LAST 0x%08lx (%ld)",item.ti_Data,LOD -> First,LOD -> First -> Position,LOD -> Last,LOD -> Last -> Position);

         if (item.ti_Data == FV_List_Active_None)
         {
            if (LOD -> Quiet >= 0)
            {
               if ((LOD -> Update = LOD -> Active))
               {
                  LOD -> Active -> Flags &= ~FF_LINE_SELECTED;

                  F_Draw(Obj,FF_Draw_Update | FF_Draw_Line);
               }
            }
            LOD -> Active = NULL;
         }
         else
         {
            LONG pos = (LOD -> Active) ? LOD -> Active -> Position : 0;

            switch (item.ti_Data)
            {
               case FV_List_Active_PageDown:
               {
                  if (LOD -> Last)
                  {
                     if (active == LOD -> Last) pos += LOD -> Visible - 1;
                     else                       pos = LOD -> Last -> Position;
                  }
                  else
                  {
                     pos += LOD -> Visible - 1;
                  }
               }
               break;

               case FV_List_Active_PageUp:
               {
                  if (LOD -> First)
                  {
                     if (active == LOD -> First)   pos -= LOD -> Visible - 1;
                     else                          pos = LOD -> First -> Position;
                  }
                  else
                  {
                     pos -= LOD -> Visible - 1;
                  }
               }
               break;

               case FV_List_Active_Down:       pos += 1; break;
               case FV_List_Active_Up:         pos -= 1; break;
               case FV_List_Active_Bottom:     pos  = LOD -> LineCount - 1; break;
               case FV_List_Active_Top:        pos  = 0; break;
               default:                        pos  = item.ti_Data;
            }

            pos = MAX(pos,0);
            pos = MIN(pos,LOD -> LineCount - 1);

            tag -> ti_Data = pos;

            for (active = (FLine *)(LOD -> LineList.Head) ; active ; active = active -> Next)
            {
               if (active -> Position == pos) break;
            }

            if (active == LOD -> Active) break;

/*** new active * clear previous one ***************************************/

            if ((LOD -> Update = LOD -> Active))
            {
               LOD -> Active -> Flags &= ~FF_LINE_SELECTED;

               if (LOD -> Quiet >= 0)
               {
                  F_Draw(Obj,FF_Draw_Update | FF_Draw_Line);
               }
            }

            LOD -> Active = active;

            if (LOD -> Active)
            {
               LOD -> Active -> Flags |= FF_LINE_SELECTED;
            }

            if (LOD -> First && LOD -> Last)
            {
               if (pos >= LOD -> First -> Position && pos <= LOD -> Last -> Position)
               {
                  if ((LOD -> Update = LOD -> Active))
                  {
                     F_Draw(Obj,FF_Draw_Update | FF_Draw_Line);
                  }
               }
               else
               {
                  if (pos > LOD -> Last -> Position)
                  {
                     pos = pos - LOD -> Visible + 1;
                  }

                  if (LOD -> Quiet >= 0)
                  {
                     F_Set(Obj,F_IDA(FA_List_First),pos);
                  }
                  else
                  {
                     F_Do(Obj,FM_Set,FA_NoNotify,TRUE,F_IDA(FA_List_First),pos,TAG_DONE);
                  }
               }
            }
         }
      }
      break;

      case FA_List_Activation:
      {
         LOD -> Activation = item.ti_Data;
      }
      break;

      case FA_List_Quiet:
      {
         if (item.ti_Data) LOD -> Quiet--;
         else              LOD -> Quiet++;

         if ((LOD -> Quiet == 0) && (FF_LIST_MODIFIED & LOD -> Flags))
         {
            list_update(Class,Obj,FALSE);

//            F_Log(0,"entries (%ld) visible (%ld) first (0x%08lx)(%ld)",LOD -> LineCount,LOD -> Visible,LOD -> First,LOD -> First -> Position);

            F_SuperDo(Class,Obj,FM_Set,
               F_IDA(FA_List_Entries), LOD -> LineCount,
               F_IDA(FA_List_Visible), LOD -> Visible,
               F_IDA(FA_List_First),   (LOD -> First) ? LOD -> First -> Position : 0,
               TAG_DONE);

            F_Draw(Obj,FF_Draw_Update);
         }
      }
      break;

      case FA_List_Steps:
      {
         LOD -> Steps = item.ti_Data;
      }
      break;
   }

   F_SUPERDO();
}
//+
///List_GetEntry
F_METHODM(APTR,List_GetEntry,FS_List_GetEntry)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   switch (Msg -> Position)
   {
      case FV_List_GetEntry_First:  return LOD -> LineList.Head;
      case FV_List_GetEntry_Last:   return LOD -> LineList.Tail;
      case FV_List_GetEntry_Prev:   return (LOD -> Active) ? LOD -> Active -> Prev : NULL;
      case FV_List_GetEntry_Next:   return (LOD -> Active) ? LOD -> Active -> Next : NULL;
      case FV_List_GetEntry_Active: return LOD -> Active;

      default:
      {
         FLine *line;

         for (line = (FLine *)(LOD -> LineList.Head) ; line ; line = line -> Next)
         {
            if (line -> Position == Msg -> Position)
            {
               break;
            }
         }

         if (line)
         {
            return line -> Entry;
         }
      }
   }
   return NULL;
}
//+

///Preferences
STATIC FPreferenceScript Script[] =
{
    { "$list-frame",             FV_TYPE_STRING,  "Spec",     "<frame id='23' padding-width='2' padding-height='1' />",0 },
    { "$list-back",              FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-scheme",            FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-title-back",        FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-title-frame",       FV_TYPE_STRING,  "Spec",     DEF_CELL_FRAME,0 },
    { "$list-title-scheme",      FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-read-frame",        FV_TYPE_STRING,  "Spec",     "<frame id='24' padding-width='2' padding-height='1' />",0 },
    { "$list-read-back",         FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-read-scheme",       FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-read-title-back",   FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-read-title-frame",  FV_TYPE_STRING,  "Spec",     DEF_CELL_FRAME,0 },
    { "$list-read-title-scheme", FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-cursor-active",     FV_TYPE_STRING,  "Spec",     "highlight",0   },
    { "$list-cursor-selected",   FV_TYPE_STRING,  "Spec",     NULL,0          },
    { "$list-font-prop",         FV_TYPE_STRING,  "Contents", NULL,0          },
    { "$list-font-fixed",        FV_TYPE_STRING,  "Contents", NULL,0          },
    { "$list-spacing",           FV_TYPE_INTEGER, "Value",    (STRPTR) 1,0    },
    { "$list-steps",             FV_TYPE_INTEGER, "Value",    (STRPTR) 3,0    },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Listview",

        "Script", Script,
        "XMLSource", "feelin/preference/listview.xml",

    TAG_MORE,Msg);
}
//+

