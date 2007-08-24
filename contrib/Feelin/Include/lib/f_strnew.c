#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

STRPTR F_StrNew(uint32 *Length,STRPTR Fmt,...)
{
    va_list msg;
    va_start(msg,Fmt);

    return F_StrNewA(Length, Fmt, (int32 *) msg);
}

