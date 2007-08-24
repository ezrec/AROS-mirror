/*

$VER: 03.00 (2005/08/10)
 
   Portability update.
   
   Class rewritten for new color scheme specifications (FC_Display 04.00)
   
   Implement  the   new   FM_Adjust_Query   method,   and   use   the   new
   FM_Adust_ParseXML method.
 
$VER: 02.00 (2005/04/04)
 
   Class rewritten to use FC_Adjust.
 
$VER: 01.00 (2004/01/12)

   Cette classe permet d'ajuster un schéma de couleur. Les couleurs peuvent
   être  une  des  entrées du schéma de couleur de Feelin, une entrée de la
   table de couleur de l'écran ou enfin une couleur RVB définie par 8  bits
   pour chaque composante.

   La  classe  utilise  un  objet   FC_PreviewScheme   pour   afficher   un
   prévisualisation.  Cet  objet  est  également  utilisé  pour stocker les
   informations nécessaire à la création de la couleur.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Adjust_New);

F_METHOD_PROTO(void,Adjust_Query);
F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);
//+

F_QUERY()
{
   switch (Which)
   {
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD        (Adjust_Query,    "FM_Adjust_Query"),
            F_METHODS_ADD        (Adjust_ToString, "FM_Adjust_ToString"),
            F_METHODS_ADD        (Adjust_ToObject, "FM_Adjust_ToObject"),
            F_METHODS_ADD_STATIC (Adjust_New,       FM_New),

            F_ARRAY_END
         };
         
         STATIC F_RESOLVES_ARRAY =
         {
            F_RESOLVES_ADD("FM_Adjust_ParseXML"),
            
            F_ARRAY_END
         };
         
         STATIC F_AUTOS_ARRAY =
         {
            F_AUTOS_ADD("FA_Preview_Spec"),
            
            F_ARRAY_END
         };
         
         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Adjust),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_RESOLVES,
            F_TAGS_ADD_AUTOS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
   }
   return NULL;
}

