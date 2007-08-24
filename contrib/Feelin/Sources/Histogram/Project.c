/*

$VER: 01.00 (2005/08/10)

*/

#include "Project.h"

struct Library                     *MathIeeeDoubBasBase;
struct Library                     *MathIeeeDoubTransBase;

///METHODS
F_METHOD_PROTO(void,Histogram_AddRGB);
F_METHOD_PROTO(void,Histogram_AddPixels);
F_METHOD_PROTO(void,Histogram_CreateArray);
F_METHOD_PROTO(void,Histogram_Sort);
F_METHOD_PROTO(void,Histogram_Extract);

F_METHOD_PROTO(void,Histogram_New);
F_METHOD_PROTO(void,Histogram_Dispose);
F_METHOD_PROTO(void,Histogram_Get);
//+

///Class_New
F_METHOD(uint32,Class_New)
{
    if ((MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library",34)))
    {
        if ((MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library",34)))
        {
            return F_SUPERDO();
        }
    }
    return NULL;
}
//+
///Class_Dispose
F_METHOD(void,Class_Dispose)
{
    if (MathIeeeDoubBasBase)
    {
        if (MathIeeeDoubTransBase)
        {
            CloseLibrary(MathIeeeDoubTransBase); MathIeeeDoubTransBase = NULL;
        }
        
        CloseLibrary(MathIeeeDoubBasBase); MathIeeeDoubBasBase = NULL;
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
//                F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
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
                F_ATTRIBUTES_ADD("Resolution", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Pool", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("NumColors", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("NumPixels", FV_TYPE_INTEGER),
                
                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Histogram_AddRGB, "AddRGB"),
                F_METHODS_ADD(Histogram_AddPixels, "AddPixels"),
                F_METHODS_ADD(Histogram_CreateArray, "CreateArray"),
                F_METHODS_ADD(Histogram_Sort, "Sort"),
                F_METHODS_ADD(Histogram_Extract, "Extract"),
                
                F_METHODS_ADD_STATIC(Histogram_New, FM_New),
                F_METHODS_ADD_STATIC(Histogram_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(Histogram_Get, FM_Get),
                
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
}
