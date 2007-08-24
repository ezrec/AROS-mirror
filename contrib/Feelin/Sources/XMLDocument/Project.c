/*

$VER: 03.00 (2005/08/10)
 
    Portability update.
    
    Implement FM_Document_Clear
 
$VER: 02.00 (2005/04/30)
 
    The class is now a subclass of FC_Document, a lot of  things  have  been
    moved  from  FC_XMLDocument  to  FC_Document  so  that every FC_Document
    subclasses can enjoy them.
    
    A table of strings is used to efficiently store strings, reducing memory
    usage and increasing performances.
    
    the <?xml ?> header is now optional.
    
    rename FM_XMLDocument_Resolve as FM_XMLDocument_AddIDs
    rename FXMLResolve as FXMLID
    
    changed the type of the field 'ID' of the FXMLName  structure,  it's  no
    longer  a  pointer  to a FXMLID struct (former FXMLResolve) but a simple
    ULONG storing the ID. Since names are saved in a FHashTable  instead  of
    using  the  resolve one, the pointer is useless. Further more, ID is far
    more easy to check now.
    
    The method FM_XMLDocument_Resolve as been added to the class. It can  be
    used  to  resolve  simple data type (object and definition resolving are
    still handled by FC_XMLObject).
    
    Added &tab; and &nl; which are replaced  respectively  by  a  tabulation
    (\t) and newline character (\n).
 
*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,XMLDocument_Get);
F_METHOD_PROTO(void,XMLDocument_Parse);
F_METHOD_PROTO(void,XMLDocument_Clear);
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
                F_ATTRIBUTES_ADD("Markups", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Attributes", FV_TYPE_POINTER),
                
                F_ARRAY_END
            };
                                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD       (XMLDocument_Parse,    "FM_Document_Parse"),
                F_METHODS_ADD       (XMLDocument_Clear,    "FM_Document_Clear"),
                F_METHODS_ADD_STATIC(XMLDocument_Get,       FM_Get),
                
                F_ARRAY_END
            };
            
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FA_Document_Pool"),
                F_RESOLVES_ADD("FA_Document_Source"),
                F_RESOLVES_ADD("FA_Document_SourceType"),
                F_RESOLVES_ADD("FA_Document_Version"),
                F_RESOLVES_ADD("FA_Document_Revision"),
                F_RESOLVES_ADD("FM_Document_Read"),
                F_RESOLVES_ADD("FM_Document_AddIDs"),
                F_RESOLVES_ADD("FM_Document_Resolve"),
                F_RESOLVES_ADD("FM_Document_ObtainName"),
                F_RESOLVES_ADD("FM_Document_Log"),
                
                F_ARRAY_END
            };
             
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Document),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_RESOLVES,
                
                F_ARRAY_END
            };
 
            return F_TAGS;
        }
//+
    }
    return NULL;
}
