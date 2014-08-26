#ifndef ARROWSTRING_H
#define ARROWSTRING_H 1

#include "mcc/classes.h"

#if defined(__AROS__)
#define ArrowStringObject BOOPSIOBJMACRO_START(getArrowStringClass())
#else
#define ArrowStringObject NewObject(getArrowStringClass(), NULL
#endif

#define	MM_ArrowString_Dummy            ( OM_Dummy + 0x10000 )
#define MA_ArrowString_TagBase          ( TAG_USER + 0x10000 )

#define MM_ArrowString_Increase         ( MM_ArrowString_Dummy + 1 )
#define MM_ArrowString_Decrease         ( MM_ArrowString_Dummy + 2 )

#define MA_ArrowString_Changed          ( MA_ArrowString_TagBase + 1 )
#define MA_ArrowString_Step             ( MA_ArrowString_TagBase + 2 )
#define MA_ArrowString_Min              ( MA_ArrowString_TagBase + 3 )
#define MA_ArrowString_Max              ( MA_ArrowString_TagBase + 4 )

DEFCLASS(ArrowString);

#endif
