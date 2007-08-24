#include "Private.h" 
 
/*** Methods ***************************************************************/ 
 
///PH_New 
F_METHODM(ULONG,PH_New,TagItem)
{ 
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
   LOD -> p_Font     = "$pophelp-font";
   LOD -> p_Scheme   = "$pophelp-scheme";
 
   return F_SUPERDO();
} 
//+ 
///PH_Dispose
F_METHOD(void,PH_Dispose)
{
   PH_Close(Class,Obj);

   F_SUPERDO();
}
//+
///PH_Get 
F_METHOD(void,PH_Get) 
{ 
   struct LocalObjectData *LOD  = F_LOD(Class,Obj); 
   struct TagItem         *Tags = Msg,item;
 
   while  (F_DynamicNTI(&Tags,&item,Class)) 
   switch (item.ti_Tag) 
   { 
      case FA_PopHelp_Open: F_STORE(NULL != LOD -> Bubble); break;
   } 

   F_SUPERDO();
} 
//+ 
///PH_Set 
F_METHOD(void,PH_Set) 
{ 
   struct LocalObjectData *LOD  = F_LOD(Class,Obj); 
   struct TagItem         *Tags = Msg,item;
 
   while  (F_DynamicNTI(&Tags,&item,Class)) 
   switch (item.ti_Tag) 
   { 
      case FA_PopHelp_Text:
      {
         LOD -> Text = (STRPTR)(item.ti_Data);
      }
      break;

      case FA_PopHelp_Font:
      {
         LOD -> p_Font = (STRPTR)(item.ti_Data);
      }
      break;

      case FA_PopHelp_Open: 
      {
         if (item.ti_Data) PH_Open (Class,Obj); 
         else              PH_Close(Class,Obj); 
      }
      break;

      case FA_PopHelp_Window:
      {
         LOD -> Window = (FObject)(item.ti_Data);
      }
      break;

      case FA_PopHelp_Scheme:
      {
         LOD -> p_Scheme = (STRPTR)(item.ti_Data);
      }
      break;
   } 

   F_SUPERDO();
} 
//+ 
/*
STATIC FPreferenceScript Script[] =
{
   "$pophelp-font",     FV_STRING, "Contents",  NULL,0,
   "$pophelp-scheme",   FV_STRING,  "Spec",     NULL,0,
   NULL
};

F_METHOD(ULONG,Prefs_New)
{
   return F_SuperDo(Class,Obj,Method,

         FA_Group_PageTitle, "PopHelp",

         "Script",         Script,
         "XMLSource",      "feelin/preference/pophelp.xml",

   TAG_MORE,Msg);
}
*/
