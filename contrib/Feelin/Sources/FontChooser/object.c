#include "Private.h"

/*** Methods ***************************************************************/

///FontChooser_New
F_METHOD(FObject,FontChooser_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct FeelinAvailFontNode *font_node;

   F_AREA_SAVE_PUBLIC;

   /* create object */

   LOD -> name_list = ListObject,
      FA_MinWidth,               100,
      FA_MinHeight,              50,
     "FA_List_DisplayHook",     &CUD -> name_display_hook,
   End;

   LOD -> size_list = ListObject,
      FA_MinWidth,               30,
      FA_MinHeight,              50,
     "FA_List_ConstructHook",   &CUD -> size_construct_hook,
     "FA_List_DestructHook",    &CUD -> size_destruct_hook,
     "FA_List_DisplayHook",     &CUD -> size_display_hook,
   End;

   if (!LOD -> name_list && !LOD -> size_list)
   {
      goto __error;
   }

   LOD -> name_string = StringObject, "FA_String_AttachedList",LOD -> name_list, End;
   LOD -> size_string = StringObject, "FA_String_AttachedList",LOD -> size_list, End;

   if (!LOD -> name_string || !LOD -> size_string)
   {
      goto __error;
   }

/*** add font names to name_list *******************************************/

   for (font_node = (struct FeelinAvailFontNode *)(CUD -> AvailFontHeader -> FontList.Head) ; font_node ; font_node = font_node -> Next)
   {
      F_Do(LOD -> name_list,F_IDO(FM_List_InsertSingle),font_node,FV_List_Insert_Bottom);
   }

/*** create notifications **************************************************/

   F_Do(LOD -> name_list,FM_Notify,"FA_List_Active",FV_Notify_Always,
        Obj,F_IDM(FM_FontChooser_ActivateName),1,FV_Notify_Value);

   F_Do(LOD -> name_list,FM_Notify,"FA_List_Activation",FV_List_Activation_DoubleClick,
        Obj,FM_Set,2,F_IDA(FA_FontChooser_Choosed),TRUE);

   F_Do(LOD -> size_list,FM_Notify,"FA_List_Active",FV_Notify_Always,
        Obj,F_IDM(FM_FontChooser_ActivateSize),1,FV_Notify_Value);

   F_Do(LOD -> size_list,FM_Notify,"FA_List_Activation",FV_List_Activation_DoubleClick,
        Obj,FM_Set,2,F_IDA(FA_FontChooser_Choosed),TRUE);

   F_Do(LOD -> name_string,FM_Notify,"FA_String_Contents",FV_Notify_Always,
        Obj,F_IDM(FM_FontChooser_ActivateBoth),0);

   F_Do(LOD -> size_string,FM_Notify,"FA_String_Contents",FV_Notify_Always,
        Obj,F_IDM(FM_FontChooser_ActivateBoth),0);

/*** initialise lists ******************************************************/

   F_Set(LOD -> name_list,(uint32) "FA_List_Active",0);

/*** create group **********************************************************/

   if (F_SuperDo(Class,Obj,Method,

      FA_ChainToCycle,  FALSE,
      FA_Horizontal,    FALSE,

      Child, HGroup,
         Child, VGroup, FA_ChainToCycle,FALSE, FA_Weight,80,
            Child, ListviewObject, FA_ChainToCycle,FALSE, "FA_Listview_List",LOD -> name_list, End,
            Child, LOD -> name_string,
         End,

         Child, VGroup, FA_ChainToCycle,FALSE, FA_Weight,20,
            Child, ListviewObject, FA_ChainToCycle,FALSE, "FA_Listview_List",LOD -> size_list, End,
            Child, LOD -> size_string,
         End,
      End,

      Child, LOD -> preview = TextObject,
         FA_Frame,         "$text-frame",
         FA_Back,          "$text-back",
         FA_Font,          LOD -> spec,
         FA_Weight,        5,
         FA_MinHeight,     20,
         FA_Text,          "Preview",
         FA_Text_PreParse, "<align=center><pens shadow=halfdark text=shine>",
         FA_Text_VCenter,  TRUE,
         FA_ChainToCycle,  FALSE,
         FA_Draggable,     TRUE,
         End,

      TAG_MORE,Msg))
   {
      return Obj;
   }
   return NULL;

__error:

   F_DisposeObj(LOD -> name_list); LOD -> name_list = NULL;
   F_DisposeObj(LOD -> size_list); LOD -> size_list = NULL;
   F_DisposeObj(LOD -> name_string); LOD -> name_string = NULL;
   F_DisposeObj(LOD -> size_string); LOD -> size_string = NULL;

   return NULL;
}
//+
///FontChooser_Dispose
F_METHOD(void,FontChooser_Dispose)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);

   F_Dispose(LOD -> spec); LOD -> spec = NULL;

   F_SUPERDO();
}
//+
///FontChooser_Set
F_METHOD(void,FontChooser_Set)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_FontChooser_Spec:
      {
         uint32 active_name=0;
         uint32 active_size=0;

         F_Dispose(LOD -> spec);
         LOD -> spec = F_StrNew(NULL,"%s",item.ti_Data);

         if (LOD -> spec)
         {
            struct FeelinAvailFontNode *font_node;

            for (font_node = (struct FeelinAvailFontNode *)(CUD -> AvailFontHeader -> FontList.Head) ; font_node ; font_node = font_node -> Next)
            {
               uint32 len = F_StrLen(font_node -> Name);

               if (LOD -> spec[len] == '/')
               {
                  if (Strnicmp(font_node -> Name,LOD -> spec,len) == 0)
                  {
                     struct FeelinAvailSizeNode *size_node;
                     uint32 size = atol(FilePart(LOD -> spec));

                     for (size_node = (struct FeelinAvailSizeNode *)(font_node -> SizeList.Head) ; size_node ; size_node = size_node -> Next)
                     {
                        if (size_node -> Size == size) break;

                        active_size++;
                     }
                     break;
                  }
               }
               active_name++;
            }
         }

         F_Set(LOD -> name_list,(uint32) "FA_List_Active",active_name);
         F_Set(LOD -> size_list,(uint32) "FA_List_Active",active_size);

         F_Set(LOD -> preview,FA_Font,(uint32)(LOD -> spec));
      }
      break;
   }

   F_SUPERDO();
}
//+
///FontChooser_Get
F_METHOD(void,FontChooser_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   BOOL up=FALSE;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_FontChooser_Spec:
      {
         F_STORE(LOD -> spec);
      }
      break;

      default: up = TRUE;
   }

   if (up) F_SUPERDO();
}
//+

///FontChooser_ActivateName
F_METHODM(void,FontChooser_ActivateName,FS_FontChooser_ActivateName)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct FeelinAvailFontNode *font = (struct FeelinAvailFontNode *) F_Do(LOD -> name_list,F_IDO(FM_List_GetEntry),Msg -> Pos);

   if (font)
   {
      struct FeelinAvailSizeNode *size;
      uint32 current_size,match_entry=0;

      /* stegerg: CHECKME/FIXME size_string my be NULL. Therefore I added some checks */
      {
      	 STRPTR size_string = (STRPTR)F_Get(LOD -> size_string,(uint32) "FA_String_Contents");

	 if (size_string)
	 {
            current_size = atol(size_string);
	 }
	 else
	 {
            current_size = 0;
	 }
      }

//      F_Log(0,"active %ld 0x%08lx '%s'",Msg -> Pos,font,font -> Name);

      F_Set(LOD -> size_list,F_IDO(FA_List_Quiet),TRUE);

      F_Do(LOD -> size_list,F_IDO(FM_List_Remove),FV_List_Remove_All);

      for (size = (struct FeelinAvailSizeNode *) font -> SizeList.Head ; size ; size = size -> Next)
      {
         F_Do(LOD -> size_list,F_IDO(FM_List_InsertSingle),size -> Size,FV_List_Insert_Bottom);

         if (size -> Size < current_size)
         {
            match_entry++;
         }
      }

      /* do not notify here, because 'size_string' will  also  trigger  the
      notify */

      F_Do(LOD -> name_string,FM_Set,

         FA_NoNotify,         TRUE,
        "FA_String_Contents", font -> Name,

         TAG_DONE);

      F_Do(LOD -> size_list,FM_Set,

        "FA_List_Active", match_entry,
        "FA_List_Quiet",  FALSE,

         TAG_DONE);
   }
}
//+
///FontChooser_ActivateSize
F_METHODM(void,FontChooser_ActivateSize,FS_FontChooser_ActivateSize)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   uint32 size = F_Do(LOD -> size_list,F_IDO(FM_List_GetEntry),Msg -> Pos);

   if (size)
   {
      F_Set(LOD -> size_string,(uint32) "FA_String_Contents",size);
   }
}
//+
///FontChooser_ActivateBoth
F_METHOD(void,FontChooser_ActivateBoth)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Dispose(LOD -> spec);

   LOD -> spec = F_StrNew(NULL,"%s/%s",F_Get(LOD -> name_string,(uint32) "FA_String_Contents"),F_Get(LOD -> size_string,(uint32) "FA_String_Contents"));

   if (LOD -> spec)
   {
      F_Set(LOD -> preview,FA_Font,(uint32)(LOD -> spec));
   }
}
//+

