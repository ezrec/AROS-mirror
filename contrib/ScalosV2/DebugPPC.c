
#include <exec/types.h>

void KPrintF(char *format, ...);

void KPrintFPPC(char *format, ULONG arg1, ULONG arg2, ULONG arg3, ULONG arg4, ULONG arg5, ULONG arg6, ULONG arg7, ULONG arg8)
{
	KPrintF(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

