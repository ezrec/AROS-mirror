#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

STRPTR F_StrFmt(STRPTR Buf,STRPTR Fmt,...)
{
    va_list msg;
    va_start(msg,Fmt);

    return F_StrFmtA(Buf,Fmt,(int32 *) msg);
}

