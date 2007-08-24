/*

$VER: 01.02 (2005/08/10)
 
    Portability update

$VER: 01.00 (2005/04/14)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,FileChooser_New);
F_METHOD_PROTO(void,FileChooser_Get);
F_METHOD_PROTO(void,FileChooser_Set);
F_METHOD_PROTO(void,FileChooser_Update);
F_METHOD_PROTO(void,FileChooser_Activate);
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
                F_METHODS_ADD        (FileChooser_Update,   "Private1"),
                F_METHODS_ADD        (FileChooser_Activate, "Private2"),
                F_METHODS_ADD_STATIC (FileChooser_New,       FM_New),
                F_METHODS_ADD_STATIC (FileChooser_Get,       FM_Get),
                F_METHODS_ADD_STATIC (FileChooser_Set,       FM_Set),

               F_ARRAY_END
            };
             
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
