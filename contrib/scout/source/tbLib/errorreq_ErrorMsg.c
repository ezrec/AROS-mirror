#include <exec/types.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <string.h>
#include <stdarg.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

#define STRLEN(s) \
({ \
    CONST_STRPTR _s = s; \
    while (*_s++ != '\0'); \
    _s - s - 1; \
})

static inline STRPTR CreateFormatStringFromEasyStruct(struct EasyStruct *easyStruct)
{
    STRPTR format = NULL;
    LONG lentext = 0, lengadget = 0;

    if (easyStruct->es_TextFormat) lentext = STRLEN(easyStruct->es_TextFormat);
    if (easyStruct->es_GadgetFormat) lengadget = STRLEN(easyStruct->es_GadgetFormat);

    format = AllocVec(lentext + lengadget + 1, MEMF_PUBLIC);
    CopyMem(easyStruct->es_TextFormat, format, lentext);
    CopyMem(easyStruct->es_GadgetFormat, format + lentext, lengadget);
    format[lentext + lengadget] = '\0';

    return format;
}

static inline void FreeFormatString(STRPTR format)
{
    FreeVec(format);
}

/* /// "ErrorMsg()" */
void VARARGS68K STDARGS ErrorMsg( CONST_STRPTR title,
                                  CONST_STRPTR msg, ... )
{
    struct EasyStruct es;

    es.es_StructSize = sizeof(struct EasyStruct);
    es.es_Flags = 0;
    es.es_Title = title;
    es.es_TextFormat = msg;
    es.es_GadgetFormat = (STRPTR)"Ok";

    STRPTR format = CreateFormatStringFromEasyStruct(&es);

    AROS_SLOWSTACKFORMAT_PRE_USING(msg, format);
    EasyRequestArgs(NULL, &es, NULL, AROS_SLOWSTACKFORMAT_ARG(format));
    AROS_SLOWSTACKFORMAT_POST(format);

    FreeFormatString(format);
}
/* \\\ */

