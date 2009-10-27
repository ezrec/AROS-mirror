#include "system_headers.h"

#if defined(__AROS__)

#define FontDisplayObject  BOOPSIOBJMACRO_START(FontDisplayClass->mcc_Class), \
    TextFrame, \
    MUIA_Background, MUII_BACKGROUND \

#else

#define FontDisplayObject  NewObject(FontDisplayClass->mcc_Class, NULL, \
    TextFrame, \
    MUIA_Background, MUII_BACKGROUND \

#endif

APTR MakeFontDisplayClass( void );

