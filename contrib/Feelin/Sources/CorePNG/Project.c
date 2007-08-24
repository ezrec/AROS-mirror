/*

$VER: 01.00 (2005/08/17)

*/

#include "Project.h"

#if 0
struct Library                     *CyberGfxBase;
struct Library                     *MathIeeeDoubBasBase;
struct Library                     *MathIeeeDoubTransBase;
#endif

//FIXME: Remove CyberGfx, it's there for testing only !

///METHODS
F_METHOD_PROTO(void, CorePNG_New);
F_METHOD_PROTO(void, CorePNG_Dispose);
F_METHOD_PROTO(void, CorePNG_Get);
//+

#if 0
///Class_New
F_METHOD(uint32, Class_New)
{
    if ((MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library",34)))
    {
        if ((MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library",34)))
        {

            CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFX_INCLUDE_VERSION);

            if (CyberGfxBase)
            {
                return F_SUPERDO();
            }
        }
    }
    return 0;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    if (MathIeeeDoubBasBase)
    {
        if (MathIeeeDoubTransBase)
        {
            CloseLibrary(MathIeeeDoubTransBase); MathIeeeDoubTransBase = NULL;
        }

        CloseLibrary(MathIeeeDoubBasBase); MathIeeeDoubBasBase = NULL;
    }

    if (CyberGfxBase)
    {
        CloseLibrary(CyberGfxBase); CyberGfxBase = NULL;
    }

   F_SUPERDO();
}
//+
#endif

F_QUERY()
{
    switch (Which)
    {
#if 0
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
#endif
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(SourceType) =
            {
                F_VALUES_ADD("None",FV_CoreMedia_SourceType_None),
                F_VALUES_ADD("File",FV_CoreMedia_SourceType_File),
                F_VALUES_ADD("Handle",FV_CoreMedia_SourceType_Handle),
           
                F_ARRAY_END
            };
   
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Source", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD_WITH_VALUES("SourceType", FV_TYPE_INTEGER, SourceType),
                F_ATTRIBUTES_ADD("Public", FV_TYPE_POINTER),

                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(CorePNG_New, FM_New),
                F_METHODS_ADD_STATIC(CorePNG_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(CorePNG_Get, FM_Get),
                
                F_ARRAY_END
            };
            
            #if 0
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("<name>"),

                F_ARRAY_END
            };
            #endif
            
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Object),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                //F_TAGS_ADD_RESOLVES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
