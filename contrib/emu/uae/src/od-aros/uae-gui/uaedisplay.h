#ifndef UAEDISPLAY_H
#define UAEDISPLAY_H

#include <proto/muimaster.h>
#include <libraries/mui.h>

enum
{
    MUIA_UAEDisplay_Width = TAG_USER | 0x01000000, /* (ISG) */
    MUIA_UAEDisplay_Height,                        /* (ISG) */
    MUIA_UAEDisplay_EventHandler,                  /* (ISG) */
    MUIA_UAEDisplay_Memory,                        /* (..G) */
    MUIA_UAEDisplay_BytesPerPix,                   /* (..G) */
    MUIA_UAEDisplay_BytesPerRow,                   /* (..G) */
    MUIA_UAEDisplay_BitDepth,                      /* (..G) */
    MUIA_UAEDisplay_MaxWidth,                      /* (..G) */
    MUIA_UAEDisplay_MaxHeight,                     /* (..G) */
};

enum
{
    MUIM_UAEDisplay_Update = TAG_USER | 0x01000000,
    __MUIM_UAEDisplay_ReallocMemory /* Private */
};

struct MUIP_UAEDisplay_Update
{
    IPTR MethodID;
    SIPTR top;
    SIPTR bottom;
};


extern struct MUI_CustomClass *UAEDisplay_CLASS;
#define UAEDisplayObject BOOPSIOBJMACRO_START(UAEDisplay_CLASS->mcc_Class)

#endif /* !AROS_GUI_H */
