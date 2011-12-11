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

#ifdef __AROS__
#include <exec/rawfmt.h>
#endif

	/* StuffChar(REG(a3) struct FormatContext *Context,REG(d0) UBYTE Char):
	 *
	 *	Stuffs a character into the buffer, unless there is not
	 *	enough room left.
	 */

#ifdef __AROS__
struct FormatContext *
#else
VOID ASM
#endif
StuffChar(REG(a3) struct FormatContext *Context,REG(d0) UBYTE Char)
{
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
        return Context;
#endif
}

	/* LimitedVSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,...):
	 *
	 *	Like VSPrintf, but won't write more than the given number
	 *	of bytes.
	 */

VOID
LimitedVSPrintf(LONG Size,STRPTR Buffer,CONST_STRPTR FormatString,va_list VarArgs)
{
	struct FormatContext Context;

	Context.Index	= Buffer;
	Context.Size	= Size;

#ifdef __AROS__
        VNewRawDoFmt(FormatString,(VOID_FUNC)StuffChar,(APTR)&Context,VarArgs);
#else
#ifdef USE_GLUE
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)StuffCharGlue,(APTR)&Context);
#else
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)StuffChar,(APTR)&Context);
#endif	/* USE_GLUE */
#endif
}

	/* LimitedSPrintf(LONG Size,STRPTR Buffer,CONST_STRPTR FormatString,...):
	 *
	 *	Like SPrintf, but won't write more than the given number
	 *	of bytes.
	 */

VOID
LimitedSPrintf(LONG Size,STRPTR Buffer,CONST_STRPTR FormatString,...)
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


VOID
VSPrintf(STRPTR Buffer,CONST_STRPTR FormatString,va_list VarArgs)
{
#ifdef __AROS__
	VNewRawDoFmt(FormatString,RAWFMTFUNC_STRING,&Buffer,VarArgs);
#else
	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)"\x16\xC0\x4E\x75",(APTR)Buffer);
#endif
}

	/* SPrintf(STRPTR Buffer,STRPTR FormatString,...):
	 *
	 *	Just like sprintf(), but using the ROM routines.
	 */

VOID
SPrintf(STRPTR Buffer,CONST_STRPTR FormatString,...)
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

#ifdef __AROS__
ULONG *CountChar(ULONG *Count, UBYTE ch)
{
    (*Count)++;
    return Count;
}
#else
VOID ASM
CountChar(REG(a3) ULONG *Count)
{
	*Count = *Count + 1;
}
#endif

	/* GetFormatLength(STRPTR FormatString,va_list VarArgs):
	 *
	 *	Before calling VSPrintf or the like, count the number of
	 *	characters that would go into the format buffer (including
	 *	the trailing NUL character).
	 */

ULONG
GetFormatLength(CONST_STRPTR FormatString,va_list VarArgs)
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
GetFormatLengthArgs(CONST_STRPTR FormatString,...)
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
LimitedStrcat(LONG Size,STRPTR To,CONST_STRPTR From)
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

	/* LimitedStrcpy(LONG Size,STRPTR To,CONST_STRPTR From):
	 *
	 *	Like strcpy, but limited by the size of the
	 *	destination buffer.
	 */

VOID
LimitedStrcpy(LONG Size,STRPTR To,CONST_STRPTR From)
{
	while(*From && Size > 0)
	{
		*To++ = *From++;

		Size--;
	}

	*To = 0;
}

	/* LimitedStrncpy(LONG Size,STRPTR To,CONST_STRPTR From):
	 *
	 *	Like strncpy, but limited by the size of the
	 *	destination buffer.
	 */

VOID
LimitedStrncpy(LONG Size,STRPTR To,CONST_STRPTR From,LONG FromSize)
{
	while(*From && Size > 0 && FromSize > 0)
	{
		*To++ = *From++;

		Size--;
		FromSize--;
	}

	*To = 0;
}
