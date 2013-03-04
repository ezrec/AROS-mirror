#include <exec/types.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <string.h>
#include <stdarg.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "ErrorMsg()" */
void VARARGS68K STDARGS ErrorMsg( CONST_STRPTR title,
                                  CONST_STRPTR msg, ... )
{
    VA_LIST args;
    struct EasyStruct es;

    es.es_StructSize = sizeof(struct EasyStruct);
    es.es_Flags = 0;
    es.es_Title = title;
    es.es_TextFormat = msg;
    es.es_GadgetFormat = (STRPTR)"Ok";

    VA_START(args, msg);
#if defined(__AROS__) && defined(__ARM_ARCH__)
    #warning "TODO: fix va_arg usage for ARM"
#else
    EasyRequestArgs(NULL, &es, NULL, VA_ARG(args, APTR));
#endif
    VA_END(args);
}
/* \\\ */

