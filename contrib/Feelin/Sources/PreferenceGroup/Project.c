/*

$VER: 02.02 (2005/08/10)
 
    Portability update
 
$VER: 02.00 (2005/05/03)
 
    Added support for subclasses to easily use XML GUIs.
    
    Preference handling can now be fully automatic. The developer only  need
    to  provide  a FPReferenceScript array and generation / loading / saving
    and disposal is automatic ! If the user has also defined references with
    the  FA_PreferenceGroup_XMLReferences,  their  are  merged to the script
    ones, so that eveybody gets the object he wants to :-)
 
*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,PG_Load);
F_METHOD_PROTO(void,PG_Save);
F_METHOD_PROTO(void,PG_New);
F_METHOD_PROTO(void,PG_Dispose);
F_METHOD_PROTO(void,PG_Get);
//+
 
F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("XMLSource",      FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("XMLDefinitions", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("XMLReferences",  FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("XMLObject",      FV_TYPE_OBJECT),
                F_ATTRIBUTES_ADD("Script",         FV_TYPE_POINTER),
                 
                F_ARRAY_END
            };
 
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(PG_Load,          "Load"),
                F_METHODS_ADD(PG_Save,          "Save"),
                F_METHODS_ADD_STATIC(PG_New,     FM_New),
                F_METHODS_ADD_STATIC(PG_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(PG_Get,     FM_Get),
                
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
    }
    return NULL;
}

