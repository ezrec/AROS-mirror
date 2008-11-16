/*
** debug functions
*/

#ifdef WITH_DEBUG

#include <exec/types.h>
#include <stdarg.h>

#ifdef MORPHOS
static void *myPutChar(void *data, char c)
{
    RawPutChar(c);
    return data;
}

void *vhprintf(char *FmtString, void *PutChProc, char *PutChData, va_list args);

void kprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vhprintf((char*)fmt, myPutChar, NULL, args);
	va_end(args);
}
#endif

#endif
