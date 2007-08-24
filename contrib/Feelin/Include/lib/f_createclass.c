#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

FClass * F_CreateClass(STRPTR Name,...)
{
    va_list msg;
    va_start(msg,Name);
    
    return F_CreateClassA(Name,(struct TagItem *) msg);
}

