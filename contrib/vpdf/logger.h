#ifndef LOGGER_MCC_H
#define LOGGER_MCC_H

#include "mcc/classes.h"
#include "application.h"

#define	MUIM_Logger_LogMessage  		   ( MUIM_Logger_Dummy + 1 )

#if defined(__AROS__)
#define LoggerObject BOOPSIOBJMACRO_START(getLoggerClass())
#else
#define	LoggerObject   NewObject( getLoggerClass() , NULL
#endif

struct MUIP_Logger_LogMessage{ULONG MethodID; ULONG severity; STRPTR message;};

#define MUIV_Logger_LogMessage_Message MUIV_VPDF_LogMessage_Message
#define MUIV_Logger_LogMessage_Warning MUIV_VPDF_LogMessage_Warning
#define MUIV_Logger_LogMessage_Error MUIV_VPDF_LogMessage_Error

DEFCLASS(Logger);


#endif
