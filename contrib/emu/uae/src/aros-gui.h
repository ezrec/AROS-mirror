#ifndef AROS_GUI_H
#define AROS_GUI_H

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <libraries/mui.h>

#define MUIA_UAEDisplay_Width        (MUIB_MUI | 1)
#define MUIA_UAEDisplay_Height       (MUIB_MUI | 2)
#define MUIA_UAEDisplay_Memory       (MUIB_MUI | 3)
#define MUIA_UAEDisplay_BytesPerPix  (MUIB_MUI | 4)
#define MUIA_UAEDisplay_BytesPerRow  (MUIB_MUI | 5)
#define MUIA_UAEDisplay_BitDepth     (MUIB_MUI | 6)
#define MUIA_UAEDisplay_EventHandler (MUIB_MUI | 7)

#define MUIM_UAEDisplay_Update (MUIB_MUI | 1)
struct MUIP_UAEDisplay_Update
{
    IPTR MethodID;
    SIPTR top;
    SIPTR bottom;
};

/* Private */
#define __MUIM_UAEDisplay_ReallocMemory (MUIB_MUI | 2)

extern struct MUI_CustomClass *UAEDisplay_CLASS;
#define UAEDisplayObject BOOPSIOBJMACRO_START(UAEDisplay_CLASS->mcc_Class)

extern Object *uaedisplay;

#endif /* !AROS_GUI_H */
