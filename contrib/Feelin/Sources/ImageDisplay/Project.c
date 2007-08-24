/*

$VER: 05.00 (2005/08/10)
 
    New Feelin design. Added a shared FC_XMLDocument object used to  decode
    spec.
   
    All  specification  had  been   changed.   Except   for   colors,   all
    specifications  are  now  in  XML  format.  Note  that colors spec have
    changed as well.
   
    Metaclass support.
   
    Added a middle color for gradients.
    
    Support the new FC_Picture and FC_BitMap classes
 
$VER: 04.00 (2005/04/06)
 
    Class rewritten. The class uses Hi-Color  function  whenever  possible.
    Added simple gradient support.
   
    FA_ImageDisplay_Origin (FBox *)
   
        If this atytribute is defined, the coordinates within the FBox  are
        used  as  origins.  This  helps  in  drawing  gradients  correctly.
        Pictures were drawn using 0:0 as origin, now  they  also  use  this
        attribute.  Thus,  objects  using  picture  gets  the right aspect,
        because to origin is relative to their inner borders.
   
    FA_ImageDisplay_Mask (BOOL)
   
        should be used to know is you  have  to  erase  the  region  before
        drawing  the  image.  Very  usefull to avoid unecesary erasings and
        flickerings.

$VER: 03.10 (2004/10/27)

    Because  FC_ImageDisplay  objects  can  be   inherited,   'Render'   in
    FS_ImageDisplay_Draw  must  be  preserved  and  used for rendering e.g.
    FC_Slider can create its own FC_Render object for its know, thus if the
    knob inherit its background which was created by FC_Window object (with
    window's FC_Render object), rendering will use  the  correct  FC_Render
    object.



   TODO ______________________________________________________________ TODO

   In Low-Color contexts a  median  color  should  be  used  instead  of  a
   gradient.

   The IMAGE_HOOK should handles three functions :  SETUP,  CLEANUP,  DRAW.
   userdata ?..
   
*/

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,ID_New);
F_METHOD_PROTO(void,ID_Dispose);
F_METHOD_PROTO(void,ID_Get);
F_METHOD_PROTO(void,ID_Set);
F_METHOD_PROTO(void,ID_Setup);
F_METHOD_PROTO(void,ID_Cleanup);
F_METHOD_PROTO(void,ID_Draw);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFX_INCLUDE_VERSION);
 
   if (!(DataTypesBase = OpenLibrary("datatypes.library",FV_DTVERSION)))
   {
      F_Log(FV_LOG_USER,"Unable to open datatypes.library v%ld",FV_DTVERSION);
   }

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

      CUD -> IDs = F_RESOLVES;
      
      if ((CUD -> Pool = F_CreatePool(1024,FA_Pool_Items,1,FA_Pool_Name,"ImageDisplay.Image",TAG_DONE)))
      {
         return (FObject) F_SUPERDO();
      }
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DeletePool(CUD -> Pool); CUD -> Pool = NULL;
   F_DisposeObj(CUD -> XMLDocument); CUD -> XMLDocument = NULL;
   
   if (DataTypesBase)
   {
      CloseLibrary(DataTypesBase); DataTypesBase = NULL;
   }

   if (CyberGfxBase)
   {
      CloseLibrary(CyberGfxBase); CyberGfxBase = NULL;
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
                F_ATTRIBUTES_ADD_BOTH("Spec",   FV_TYPE_STRING,  FA_ImageDisplay_Spec),
                F_ATTRIBUTES_ADD_BOTH("State",  FV_TYPE_INTEGER, FA_ImageDisplay_State),
                F_ATTRIBUTES_ADD_BOTH("Width",  FV_TYPE_INTEGER, FA_ImageDisplay_Width),
                F_ATTRIBUTES_ADD_BOTH("Height", FV_TYPE_INTEGER, FA_ImageDisplay_Height),
                F_ATTRIBUTES_ADD_BOTH("Mask",   FV_TYPE_POINTER, FA_ImageDisplay_Mask),
                F_ATTRIBUTES_ADD_BOTH("Origin", FV_TYPE_POINTER, FA_ImageDisplay_Origin),
             
                F_ARRAY_END
            };
    
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_BOTH(ID_Cleanup, "Cleanup", FM_ImageDisplay_Cleanup),
                F_METHODS_ADD_BOTH(ID_Setup, "Setup", FM_ImageDisplay_Setup),
                F_METHODS_ADD_BOTH(ID_Draw, "Draw", FM_ImageDisplay_Draw),
                
                F_METHODS_ADD_STATIC(ID_New, FM_New),
                F_METHODS_ADD_STATIC(ID_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(ID_Get, FM_Get),
                F_METHODS_ADD_STATIC(ID_Set, FM_Set),
                
                F_ARRAY_END
            };
            
            STATIC F_AUTOS_ARRAY =
            {
                F_AUTOS_ADD("FM_BitMap_Render"),
                F_AUTOS_ADD("FM_BitMap_Blit"),

                F_AUTOS_ADD("FA_BitMap_TargetScreen"),
                F_AUTOS_ADD("FA_BitMap_TargetWidth"),
                F_AUTOS_ADD("FA_BitMap_TargetHeight"),
                F_AUTOS_ADD("FA_BitMap_RenderedWidth"),
                F_AUTOS_ADD("FA_BitMap_RenderedHeight"),

                F_AUTOS_ADD("FA_Display_Screen"),
                
                F_ARRAY_END
            };
    
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Object),
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
};
