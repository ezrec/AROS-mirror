#include "system_headers.h"

#if defined(__AROS__)
#define MonitorButtonObject  BOOPSIOBJMACRO_START(MonitorButtonClass->mcc_Class), \
    ButtonFrame, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_PreParse, (IPTR)MUIX_C, \
    MUIA_InputMode, MUIV_InputMode_RelVerify, \
    MUIA_Background, MUII_ButtonBack, \
    MUIA_CycleChain, TRUE

#else

#define MonitorButtonObject                    NewObject(MonitorButtonClass->mcc_Class, NULL, \
    ButtonFrame, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_PreParse, (ULONG)MUIX_C, \
    MUIA_InputMode, MUIV_InputMode_RelVerify, \
    MUIA_Background, MUII_ButtonBack, \
    MUIA_CycleChain, TRUE

#endif

APTR MakeMonitorButtonClass( void );

