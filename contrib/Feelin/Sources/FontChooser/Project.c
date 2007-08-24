/*

$VER: 02.00 (2005/08/10)
 
   Portability update.
   
   Metaclass support.
 
$VER: 01.00 (2005/04/28)
 
   AdjustFont renamed as FontChooser

*/

#include "Project.h"

struct Library                     *DiskfontBase;
struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,FontChooser_New);
F_METHOD_PROTO(void,FontChooser_Dispose);
F_METHOD_PROTO(void,FontChooser_Get);
F_METHOD_PROTO(void,FontChooser_Set);
F_METHOD_PROTO(void,FontChooser_ActivateName);
F_METHOD_PROTO(void,FontChooser_ActivateSize);
F_METHOD_PROTO(void,FontChooser_ActivateBoth);
//+

///entry_name_display
STATIC F_HOOKM(APTR,entry_name_display,FS_List_Display)
{
   struct FeelinAvailFontNode *font_node = Msg -> Entry;

   if (font_node)
   {
      Msg -> Strings[0] = font_node -> Name;
   }
   return NULL;
}
//+
///entry_size_construct
STATIC F_HOOKM(STRPTR,entry_size_construct,FS_List_Construct)
{
   STRPTR str = F_NewP(Msg -> Pool,16);

   if (str)
   {
      stcl_d(str,(LONG)(Msg -> Entry));
   }

   return str;
}
//+
///entry_size_destruct
STATIC F_HOOKM(void,entry_size_destruct,FS_List_Destruct)
{
   F_Dispose(Msg -> Entry);
}
//+
///entry_size_display
STATIC F_HOOKM(APTR,entry_size_display,FS_List_Display)
{
   Msg -> Strings[0]   = Msg -> Entry;
   Msg -> PreParses[0] = "<align=right>";

   return NULL;
}
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   CUD -> name_display_hook.h_Entry    = (HOOKFUNC) F_FUNCTION_GATE(entry_name_display);
   CUD -> size_construct_hook.h_Entry  = (HOOKFUNC) F_FUNCTION_GATE(entry_size_construct);
   CUD -> size_destruct_hook.h_Entry   = (HOOKFUNC) F_FUNCTION_GATE(entry_size_destruct);
   CUD -> size_display_hook.h_Entry    = (HOOKFUNC) F_FUNCTION_GATE(entry_size_display);

   if ((DiskfontBase = OpenLibrary("diskfont.library",39)))
   {
      if ((CUD -> AvailFontHeader = f_fontlist_create(0)))
      {
         return (FObject) F_SUPERDO();
      }
   }
   else
   {
      F_Log(FV_LOG_USER,"Unable to open %s v%ld","diskfont.library",39);
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   if (DiskfontBase)
   {
      if (CUD -> AvailFontHeader)
      {
         f_fontlist_delete(CUD -> AvailFontHeader); CUD -> AvailFontHeader = NULL;
      }

      CloseLibrary(DiskfontBase); DiskfontBase = NULL;
   }

   F_SUPERDO();
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
            F_METHODS_ADD_STATIC(Class_New,      FM_New),
            F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

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
            F_ATTRIBUTES_ADD("Spec", FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("Choosed", FV_TYPE_BOOLEAN),
             
            F_ARRAY_END
         };
         
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD(FontChooser_ActivateName,    "Private1"),
            F_METHODS_ADD(FontChooser_ActivateSize,    "Private2"),
            F_METHODS_ADD(FontChooser_ActivateBoth,    "Private3"),
            
            F_METHODS_ADD_STATIC(FontChooser_New,       FM_New),
            F_METHODS_ADD_STATIC(FontChooser_Dispose,   FM_Dispose),
            F_METHODS_ADD_STATIC(FontChooser_Get,       FM_Get),
            F_METHODS_ADD_STATIC(FontChooser_Set,       FM_Set),

            F_ARRAY_END
         };

         STATIC F_AUTOS_ARRAY =
         {
            F_AUTOS_ADD("FM_List_Remove"),
            F_AUTOS_ADD("FM_List_GetEntry"),
            F_AUTOS_ADD("FM_List_InsertSingle"),
            F_AUTOS_ADD("FA_List_Quiet"),
            
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
