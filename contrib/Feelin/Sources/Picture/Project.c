/*

$VER: 04.00 (2005/08/10)
 
    Portability update.
   
    Complete class rewritting. It's no longer an  interface  to  DataTypes.
    The class is able to make bitmap from ARGB pixels arrays; scale picture
    with several methods (nearest,  bilinear,  average);  remap  them  with
    dithering...

$VER: 03.00 (2004/12/07)
 
*/

#include "Project.h"

struct ClassUserData               *CUD;
struct Library                     *DataTypesBase;

///METHODS
F_METHOD_PROTO(void,Picture_New);
F_METHOD_PROTO(void,Picture_Dispose);
//+

///Class_New
F_METHOD(uint32,Class_New)
{
    CUD = F_LOD(Class,Obj);
    
    CUD -> arbitrer = F_NewObj(FC_Object,/*FA_Semaphore_Name, "Picture.Semaphore",*/TAG_DONE);

    if (CUD -> arbitrer)
    {
        DataTypesBase = OpenLibrary("datatypes.library",39);
    
        if (DataTypesBase)
        {
            return F_SUPERDO();
        }
        else
        {
            F_Log(FV_LOG_USER,"Unable to open datatypes.library v%ld",39);
        }
    }
 
    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    CUD = F_LOD(Class,Obj);
    
    if (DataTypesBase)
    {
        CloseLibrary(DataTypesBase); DataTypesBase = NULL;
    }

    F_DisposeObj(CUD -> arbitrer); CUD -> arbitrer = NULL;

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
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Name", FV_TYPE_STRING),

                F_ARRAY_END
            };
                              
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Picture_New, FM_New),
                F_METHODS_ADD_STATIC(Picture_Dispose, FM_Dispose),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(BitMap),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
        break;
//+
   }
   return NULL;
};

