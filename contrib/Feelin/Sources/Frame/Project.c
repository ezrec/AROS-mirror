/*

$VER: 03.00 (2005/08/10)
 
   Portability update.
   
   Frames are now defined in XML format.
 
$VER: 02.00 (2005/05/06)
 
   The FA_Back attribute has been moved from FC_Area to FC_Frame. The class
   does  now  handles  the  background image as well. The rendering is thus
   improved since the class does really know which part is to clear.
   
   The  FA_InLeft,  FA_InRight,  FA_InTop,  FA_InBottom,   FA_InWidth   and
   FA_InHeight attributes are now handled by FC_Frame and have been renamed
   as    FA_Frame_InnerLeft,    FA_Frame_InnerRight,     FA_Frame_InnerTop,
   FA_Frame_InnerBottom, FA_Frame_InnerWidth and FA_Frame_InnerHeight.
   
   Renamed FF_Frame_Select as FF_Frame_Draw_Select.
   
   Added 4 new frames for FC_Window.
 
   FV_Adjust_Separator is used as frame specifications separator.

$VER: 01.00 (2004/12/18)

   FC_FrameDisplay is deprecated, this new class replaces it. This class is
   a subclass of FC_Object, and the superclass of FC_Area.

*/

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,Frame_New);
F_METHOD_PROTO(void,Frame_Dispose);
F_METHOD_PROTO(void,Frame_Get);
F_METHOD_PROTO(void,Frame_Set);
F_METHOD_PROTO(void,Frame_Setup);
F_METHOD_PROTO(void,Frame_Cleanup);
F_METHOD_PROTO(void,Frame_Draw);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
    
   if ((CUD -> XMLDocument = XMLDocumentObject, End))
   {
      STATIC F_RESOLVES_ARRAY =
      {
         F_RESOLVES_ADD("FA_Document_Source"),
         F_RESOLVES_ADD("FA_Document_SourceType"),
         F_RESOLVES_ADD("FM_Document_AddIDs"),
         F_RESOLVES_ADD("FM_Document_Resolve"),
         F_RESOLVES_ADD("FA_XMLDocument_Markups"),

         F_ARRAY_END
      };

      F_DynamicResolveTable(F_RESOLVES);

      CUD -> XMLIDs = F_RESOLVES;

      return (FObject) F_SUPERDO();
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);

   F_DisposeObj(CUD -> XMLDocument); CUD -> XMLDocument = NULL;
   
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
                F_METHODS_ADD_STATIC(Class_New, FM_New),
                F_METHODS_ADD_STATIC(Class_Dispose, FM_Dispose),
                
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
        break;
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Position) =
            {
                F_VALUES_ADD("UpLeft",      FV_Frame_UpLeft),
                F_VALUES_ADD("UpRight",     FV_Frame_UpRight),
                F_VALUES_ADD("UpCenter",    FV_Frame_UpCenter),
                F_VALUES_ADD("DownLeft",    FV_Frame_DownLeft),
                F_VALUES_ADD("DownRight",   FV_Frame_DownRight),
                F_VALUES_ADD("DownCenter",  FV_Frame_DownCenter),

                F_ARRAY_END
            };
            
            STATIC F_VALUES_ARRAY(Frame) =
            {
                F_VALUES_ADD("None",        0),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_BOTH("Data", FV_TYPE_POINTER, FA_Frame_PublicData),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("Frame", FV_TYPE_STRING,  FA_Frame,               Frame),
                F_ATTRIBUTES_ADD_BOTH("Font", FV_TYPE_STRING, FA_Frame_Font),
                F_ATTRIBUTES_ADD_BOTH("Title", FV_TYPE_STRING, FA_Frame_Title),
                F_ATTRIBUTES_ADD_BOTH("PreParse", FV_TYPE_STRING, FA_Frame_PreParse),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("Position", FV_TYPE_STRING, FA_Frame_Position,      Position),
                F_ATTRIBUTES_ADD_BOTH("InnerLeft", FV_TYPE_INTEGER, FA_Frame_InnerLeft),
                F_ATTRIBUTES_ADD_BOTH("InnerTop", FV_TYPE_INTEGER, FA_Frame_InnerTop),
                F_ATTRIBUTES_ADD_BOTH("InnerRight", FV_TYPE_INTEGER, FA_Frame_InnerRight),
                F_ATTRIBUTES_ADD_BOTH("InnerBottom", FV_TYPE_INTEGER, FA_Frame_InnerBottom),
                F_ATTRIBUTES_ADD_BOTH("Back", FV_TYPE_STRING, FA_Back),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_BOTH(Frame_Draw, "Draw", FM_Frame_Draw),
                F_METHODS_ADD_BOTH(Frame_Setup, "Setup", FM_Frame_Setup),
                F_METHODS_ADD_BOTH(Frame_Cleanup, "Cleanup", FM_Frame_Cleanup),

                F_METHODS_ADD_STATIC(Frame_New, FM_New),
                F_METHODS_ADD_STATIC(Frame_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(Frame_Get, FM_Get),
                F_METHODS_ADD_STATIC(Frame_Set, FM_Set),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Object),
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
};
