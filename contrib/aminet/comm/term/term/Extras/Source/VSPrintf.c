/*
**	VSPrintf.c
**
**	Formatted output to memory
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* StuffChar(REG(a3) struct FormatContext *Context,REG(d0) UBYTE Char):
	 *
	 *	Stuffs a character into the buffer, unless there is not
	 *	enough room left.
	 */

#ifndef __AROS__
VOID ASM
StuffChar(REG(a3) struct FormatContext *Context,REG(d0) UBYTE Char)
#else
AROS_UFH2(VOID, StuffChar,
 AROS_UFHA(UBYTE                 , Char   , D0),
 AROS_UFHA(struct FormatContext *, Context, A3))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	if(Context->Size > 0)
	{
		*Context->Index++ = Char;

		if(--Context->Size == 1)
		{
			*Context->Index = 0;
			Context->Size = 0;
		}
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

	/* LimitedVSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,...):
	 *
	 *	Like VSPrintf, but won't write more than the given number
	 *	of bytes.
	 */

VOID
LimitedVSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,va_list VarArgs)
{
	struct FormatContext Context;

	Context.Index	= Buffer;
	Context.Size	= Size;

#ifdef USE_GLUE
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)StuffCharGlue,(APTR)&Context);
#else
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)StuffChar,(APTR)&Context);
#endif	/* USE_GLUE */
}

	/* LimitedSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,...):
	 *
	 *	Like SPrintf, but won't write more than the given number
	 *	of bytes.
	 */

VOID
LimitedSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,...)
{
	va_list VarArgs;

	va_start(VarArgs,FormatString);
	LimitedVSPrintf(Size,Buffer,FormatString,VarArgs);
	va_end(VarArgs);
}

	/* VSPrintf(STRPTR Buffer,STRPTR FormatString,va_list VarArgs):
	 *
	 *	Just like vsprintf(), but using the ROM routines.
	 */


#ifdef __AROS__
AROS_UFH2S(void, cpy_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(STRPTR *, strPtrPtr, A3))
{
    AROS_USERFUNC_INIT
    
    *(*strPtrPtr)++ = chr;
    
    AROS_USERFUNC_EXIT
}
#endif

VOID
VSPrintf(STRPTR Buffer,STRPTR FormatString,va_list VarArgs)
{
#ifdef __AROS__
	STRPTR Buffer_Ptr = Buffer;
#endif
#ifdef __AROS__
	RawDoFmt(FormatString,(APTR)VarArgs,cpy_func,&Buffer_Ptr);
#else
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)"\x16\xC0\x4E\x75",(APTR)Buffer);
#endif
}

	/* SPrintf(STRPTR Buffer,STRPTR FormatString,...):
	 *
	 *	Just like sprintf(), but using the ROM routines.
	 */

VOID
SPrintf(STRPTR Buffer,STRPTR FormatString,...)
{
	va_list VarArgs;

	va_start(VarArgs,FormatString);
	VSPrintf(Buffer,FormatString,VarArgs);
	va_end(VarArgs);
}

	/* CountChar(REG(a3) ULONG *Count):
	 *
	 *	Counts the number of bytes to stuff into the output buffer.
	 */

#ifndef __AROS__
VOID ASM
CountChar(REG(a3) ULONG *Count)
#else
AROS_UFH2(VOID, CountChar,
 AROS_UFHA(UBYTE          , dummy, D0),
 AROS_UFHA(ULONG *        , Count, A3))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	*Count = *Count + 1;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

	/* GetFormatLength(STRPTR FormatString,va_list VarArgs):
	 *
	 *	Before calling VSPrintf or the like, count the number of
	 *	characters that would go into the format buffer (including
	 *	the trailing NUL character).
	 */

ULONG
GetFormatLength(STRPTR FormatString,va_list VarArgs)
{
	ULONG Count;

	Count = 0;

#ifdef USE_GLUE
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CountCharGlue,(APTR)&Count);
#else
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)CountChar,(APTR)&Count);
#endif	/* USE_GLUE */

	return(Count);
}

	/* GetFormatLengthArgs(STRPTR FormatString,...):
	 *
	 *	VarArgs version of GetFormatLength.
	 */

ULONG
GetFormatLengthArgs(STRPTR FormatString,...)
{
	va_list VarArgs;
	ULONG Count;

	va_start(VarArgs,FormatString);
	Count = GetFormatLength(FormatString,VarArgs);
	va_end(VarArgs);

	return(Count);
}

	/* LimitedStrcat(LONG Size,STRPTR To,STRPTR From):
	 *
	 *	Like strcat, but limited by the size of the
	 *	destination buffer.
	 */

VOID
LimitedStrcat(LONG Size,STRPTR To,STRPTR From)
{
	while(*To)
	{
		To++;
		Size--;
	}

	while(*From && Size > 0)
	{
		*To++ = *From++;

		Size--;
	}

	*To = 0;
}

	/* LimitedStrcpy(LONG Size,STRPTR To,STRPTR From):
	 *
	 *	Like strcpy, but limited by the size of the
	 *	destination buffer.
	 */

VOID
LimitedStrcpy(LONG Size,STRPTR To,STRPTR From)
{
	while(*From && Size > 0)
	{
		*To++ = *From++;

		Size--;
	}

	*To = 0;
}

	/* LimitedStrncpy(LONG Size,STRPTR To,STRPTR From):
	 *
	 *	Like strncpy, but limited by the size of the
	 *	destination buffer.
	 */

VOID
LimitedStrncpy(LONG Size,STRPTR To,STRPTR From,LONG FromSize)
{
	while(*From && Size > 0 && FromSize > 0)
	{
		*To++ = *From++;

		Size--;
		FromSize--;
	}

	*To = 0;
}
