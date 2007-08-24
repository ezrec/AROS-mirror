/*

$VER: 05.00 (2005/08/10)
 
   Portability update.
   
   Metaclass support.
   
$VER: 04.00 (2005/05/09)
 
   Added a title above preference group, and a FC_PopScheme object with the
   FC_PopFrame and FC_PopImage objects.
   
   The class is now a subclass of FC_Dialog.
 
$VER: 03.00 (2004/11/01)

   The preference update system have  been  totaly  rewritten  and  is  now
   extremely  light  and  easy.  The class do not handle application update
   itself anymore. The new attribute FA_Preference_Update is there for this
   purpose.  Applications  put  a notification on this attribute and update
   themselves when their FC_Preference  object  (or  their  inherited  one)
   changes.

   The update of the FC_PreferenceGroup object is easier too since FC_Group
   (09.00)  is  smarter  then  before  when  updating  its layout, clearing
   trashed regions and redrawing modified objects.

   TODO:  ajouter  un  drapeau   FF_PE_TESTED   qui   sera   posé   lorsque
   l'utilisateur  teste  la  configuration.  Le  drapeaux  sera  utilisé si
   l'utilisateur annule les modifications. Si le drapeau n'est pas présent,
   pas besoin de restaurer le fichier de configuration.

 */

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,PE_New);
F_METHOD_PROTO(void,PE_Dispose);
F_METHOD_PROTO(void,PE_Connect);
F_METHOD_PROTO(void,PE_Disconnect);
F_METHOD_PROTO(void,PE_Setup);
F_METHOD_PROTO(void,PE_Load);
F_METHOD_PROTO(void,PE_Save);
F_METHOD_PROTO(void,PE_Use);
F_METHOD_PROTO(void,PE_Test);
F_METHOD_PROTO(void,PE_Cancel);
F_METHOD_PROTO(void,PE_Change);
//+

F_QUERY()
{
   switch (Which)
   {
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD        (PE_Load,        "Load"),
            F_METHODS_ADD        (PE_Save,        "Save"),
            F_METHODS_ADD        (PE_Use,         "Use"),
            F_METHODS_ADD        (PE_Test,        "Test"),
            F_METHODS_ADD        (PE_Cancel,      "Cancel"),
            F_METHODS_ADD        (PE_Change,      "Change"),
            F_METHODS_ADD_STATIC (PE_New,          FM_New),
            F_METHODS_ADD_STATIC (PE_Dispose,      FM_Dispose),
            F_METHODS_ADD_STATIC (PE_Connect,      FM_Connect),
            F_METHODS_ADD_STATIC (PE_Disconnect,   FM_Disconnect),
            F_METHODS_ADD_STATIC (PE_Setup,        FM_Window_Setup),
            
            F_ARRAY_END
         };

         STATIC F_AUTOS_ARRAY =
         {
            F_AUTOS_ADD("FM_List_GetEntry"),
            F_AUTOS_ADD("FM_Preference_Add"),
            F_AUTOS_ADD("FM_Preference_AddLong"),
            F_AUTOS_ADD("FM_Preference_AddString"),
            F_AUTOS_ADD("FM_Preference_Resolve"),
            F_AUTOS_ADD("FM_Preference_ResolveInt"),
            F_AUTOS_ADD("FM_Preference_Read"),
            F_AUTOS_ADD("FM_Preference_Write"),
            F_AUTOS_ADD("FM_Preference_Clear"),
            F_AUTOS_ADD("FM_PreferenceGroup_Load"),
            F_AUTOS_ADD("FM_PreferenceGroup_Save"),
            
            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Dialog),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_AUTOS,
            F_TAGS_ADD_CATALOG,
            
            F_ARRAY_END
         };
 
         return F_TAGS;
      }
//+
   }
   return NULL;
}
