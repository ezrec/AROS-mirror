#include <proto/exec.h>
#include <exec/types.h>
#include <aros/asmcall.h>

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

STRPTR *cpy_func(STRPTR *strPtrPtr, UBYTE chr)
{
	*(*strPtrPtr)++ = chr;

	return strPtrPtr;
}

void xprsprintf(STRPTR Buffer, STRPTR FormatString, ... )
{
	STRPTR Buffer_Ptr = Buffer;
	va_list VarArgs;
	va_start(VarArgs, FormatString);

	VNewRawDoFmt(FormatString,(VOID_FUNC)cpy_func,&Buffer_Ptr,VarArgs);
	
	va_end(VarArgs);
}
