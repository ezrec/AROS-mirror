#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

void F_Log(uint32 Level,STRPTR Fmt,...)
{
    va_list msg;
    va_start(msg, Fmt);
    
    F_LogA(Level, Fmt, (int32 *) msg);
}

