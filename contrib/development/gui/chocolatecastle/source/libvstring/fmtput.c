/* libvstring/fmtput.c */

#include "libvstring.h"

#include <proto/exec.h>
#include <exec/rawfmt.h>


VOID FmtPut(STRPTR dest, STRPTR fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	VNewRawDoFmt(fmt, RAWFMTFUNC_STRING, dest, args);
	va_end(args);
	return;
}

