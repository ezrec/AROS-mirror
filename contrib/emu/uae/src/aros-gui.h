#ifndef AROS_GUI_H
#define AROS_GUI_H

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <libraries/mui.h>

enum
{
    MUIA_UAEDisplay_Width = MUIB_MUI, /* (ISG) */
    MUIA_UAEDisplay_Height,           /* (ISG) */
    MUIA_UAEDisplay_EventHandler,     /* (ISG) */
    MUIA_UAEDisplay_Memory,           /* (..G) */
    MUIA_UAEDisplay_BytesPerPix,      /* (..G) */
    MUIA_UAEDisplay_BytesPerRow,      /* (..G) */
    MUIA_UAEDisplay_BitDepth,         /* (..G) */
    MUIA_UAEDisplay_MaxWidth,         /* (..G) */
    MUIA_UAEDisplay_MaxHeight,        /* (..G) */
};

enum
{
    MUIM_UAEDisplay_Update = MUIB_MUI,
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

extern Object *uaedisplay;

#endif /* !AROS_GUI_H */
