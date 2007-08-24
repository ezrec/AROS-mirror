/*

$VER: 01.00 (2005/08/10)

*/

#include "Project.h"

struct Library                     *CyberGfxBase;

///METHODS
F_METHOD_PROTO(void,BitMap_Blit);
F_METHOD_PROTO(void,BitMap_Render);

F_METHOD_PROTO(void,BitMap_New);
F_METHOD_PROTO(void,BitMap_Dispose);
F_METHOD_PROTO(void,BitMap_Get);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFX_INCLUDE_VERSION);
   
    return (FObject) F_SUPERDO();
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
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
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(BlitMode) =
            {
                F_VALUES_ADD("Copy", FV_BitMap_BlitMode_Copy),
                F_VALUES_ADD("Tile", FV_BitMap_BlitMode_Tile),
                F_VALUES_ADD("Scale", FV_BitMap_BlitMode_Scale),
                F_VALUES_ADD("Frame", FV_BitMap_BlitMode_Frame),
                
                F_ARRAY_END
            };
            
            STATIC F_VALUES_ARRAY(ScaleFilter) =
            {
                F_VALUES_ADD("Nearest", FV_BitMap_ScaleFilter_Nearest),
                F_VALUES_ADD("Bilinear", FV_BitMap_ScaleFilter_Bilinear),
                F_VALUES_ADD("Average", FV_BitMap_ScaleFilter_Average),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Width", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Height", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Depth", FV_TYPE_INTEGER),
                
                F_ATTRIBUTES_ADD("PixelSize", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("PixelArray", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("PixelArrayMod", FV_TYPE_POINTER),

                F_ATTRIBUTES_ADD("ColorType", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("ColorCount", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("ColorArray", FV_TYPE_INTEGER),
                
                F_ATTRIBUTES_ADD("Pens", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("NumPens", FV_TYPE_INTEGER),
                
                F_ATTRIBUTES_ADD("Rendered", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("RenderedType", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("RenderedWidth", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("RenderedHeight", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("TargetWidth", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("TargetHeight", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("TargetScreen", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("TargetMaxColors", FV_TYPE_INTEGER),

                F_ATTRIBUTES_ADD_WITH_VALUES("BlitMode", FV_TYPE_INTEGER, BlitMode),
                F_ATTRIBUTES_ADD_WITH_VALUES("ScaleFilter", FV_TYPE_INTEGER, ScaleFilter),
 
                F_ATTRIBUTES_ADD("Margins", FV_TYPE_POINTER),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(BitMap_Blit,"Blit"),
                F_METHODS_ADD(BitMap_Render,"Render"),
                
                F_METHODS_ADD_STATIC(BitMap_New, FM_New),
                F_METHODS_ADD_STATIC(BitMap_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(BitMap_Get, FM_Get),
                
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
