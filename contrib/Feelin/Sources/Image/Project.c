/*

$VER: 03.12 (2005/08/10)
 
    Portability update
 
$VER: 03.10 (2005/04/08)
 
    The class is no longer use as preview object, because of the  FC_Preview
    and    FC_PreviewImage    classes.    The    class    uses    the    new
    FA_ImageDisplay_Origin attribute.
 
$VER: 03.00 (2004/07/18)

    Follows FC_ImageDisplay v3.0 new image handling.

    If image spec is NULL, a cross is drawn instead.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Image_New);
F_METHOD_PROTO(void,Image_Dispose);
F_METHOD_PROTO(void,Image_Set);
F_METHOD_PROTO(void,Image_Get);
F_METHOD_PROTO(void,Image_Setup);
F_METHOD_PROTO(void,Image_Cleanup);
F_METHOD_PROTO(void,Image_AskMinMax);
F_METHOD_PROTO(void,Image_Draw);
F_METHOD_PROTO(void,Prefs_New);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Spec", FV_TYPE_STRING),
            
                F_ARRAY_END
            };
                                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Image_New,        FM_New),
                F_METHODS_ADD_STATIC(Image_Dispose,    FM_Dispose),
                F_METHODS_ADD_STATIC(Image_Set,        FM_Set),
                F_METHODS_ADD_STATIC(Image_Get,        FM_Get),
                F_METHODS_ADD_STATIC(Image_Setup,      FM_Setup),
                F_METHODS_ADD_STATIC(Image_Cleanup,    FM_Cleanup),
                F_METHODS_ADD_STATIC(Image_AskMinMax,  FM_AskMinMax),
                F_METHODS_ADD_STATIC(Image_Draw,       FM_Draw),
                
                F_ARRAY_END
            };
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                
                F_ARRAY_END
            };
 
            return F_TAGS;
        }
//+
///Prefs
        case FV_Query_PrefsTags:
        {
            STATIC F_METHODS_ARRAY =
            {
               F_METHODS_ADD_STATIC(Prefs_New, FM_New),
            
                F_ARRAY_END
            };
 
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD(Super,      FC_PreferenceGroup),
            F_TAGS_ADD(Methods,    F_METHODS),
                
                F_ARRAY_END
            };
 
            return F_TAGS;
        }
//+
    }
    return NULL;
}

