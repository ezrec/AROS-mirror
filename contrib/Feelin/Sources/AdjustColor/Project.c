/*

$VER: 03.00 (2005/08/10)
 
    Portability update.
    
    FM_Adjust_Query support.
    
    Uses the new FC_AdjustSchemeEntry class instead of FC_AdjustRaster.
 
$VER: 02.00 (2005/03/24)
 
    Réécriture complète du système d'adjustement.

$VER: 01.00 (2004/01/11)

    Cette classe permet d'ajuster une couleur. Cette couleur peut  être  une
    des  entrées  du  schéma de couleur de Feelin, un pinceau de la table de
    couleur de l'écran ou enfin une couleur RVB  définie  par  8  bits  pour
    chaque composante.

    La  classe  utilise  un   objet   FC_PreviewColor   pour   afficher   un
    prévisualisation.  Cet  objet  est  également  utilisé  pour stocker les
    informations nécessaire à la création de la couleur.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Adjust_New);
F_METHOD_PROTO(void,Adjust_Dispose);
F_METHOD_PROTO(void,Adjust_Get);
F_METHOD_PROTO(void,Adjust_Set);

F_METHOD_PROTO(void,Adjust_Query);
//F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);
//+

///F_QUERY
F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("AddScheme", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("AddPen", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("AddRGB", FV_TYPE_BOOLEAN),
                
                F_ARRAY_END
            };
 
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD        (Adjust_Query,    "FM_Adjust_Query"),
                F_METHODS_ADD        (Adjust_ToObject, "FM_Adjust_ToObject"),
                F_METHODS_ADD_STATIC (Adjust_New,       FM_New),

                F_ARRAY_END
            };
            
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FM_Adjust_ToString"),
            
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Adjust),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_RESOLVES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
    }
    return NULL;
}
//+
