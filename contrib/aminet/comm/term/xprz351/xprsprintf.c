#include <proto/exec.h>
#include <exec/types.h>
#include <aros/asmcall.h>

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

AROS_UFH2S(void, cpy_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(STRPTR *, strPtrPtr, A3))
{
	AROS_USERFUNC_INIT
	
	*(*strPtrPtr)++ = chr;
	
	AROS_USERFUNC_EXIT
}

void xprsprintf(STRPTR Buffer, STRPTR FormatString, ... )
{
	va_list VarArgs;
	va_start(VarArgs, FormatString);
	
	STRPTR Buffer_Ptr = Buffer;

	RawDoFmt(FormatString,(APTR)VarArgs,cpy_func,&Buffer_Ptr);
	
	va_end(VarArgs);
}
