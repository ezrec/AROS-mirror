#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

uint32 F_Do(FObject Obj,uint32 Method,...)
{
    va_list msg;
    va_start(msg, Method);
    
    return F_DoA(Obj, Method, (APTR) msg);
}

