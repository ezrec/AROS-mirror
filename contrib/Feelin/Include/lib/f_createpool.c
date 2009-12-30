#define NO_TAG_CALL

#include <stdarg.h>
#include <proto/feelin.h>

APTR F_CreatePool(uint32 ItemSize, ...)
{
    va_list msg;
    va_start(msg, ItemSize); 

    return F_CreatePoolA(ItemSize, (struct TagItem *) msg);
}

