#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

FObject F_MakeObj(uint32 Type,...)
{
    va_list msg;
    va_start(msg,Type);
    
    return F_MakeObjA(Type, (uint32 *) msg);
}

