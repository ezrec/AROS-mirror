/* libvstring/fmtnput.c */

#include "libvstring.h"

#include <proto/exec.h>
#include <exec/rawfmt.h>


struct Mx7
{
	STRPTR ptr;
	LONG   ctr;
	LONG   max;
};


static APTR putchproc(APTR mx, UBYTE chr)
{
	struct Mx7 *mx7 = (struct Mx7*)mx;

	if (mx7->ctr < mx7->max)
	{
		*mx7->ptr = (UBYTE)chr;
		if (chr) mx7->ptr++;
		mx7->ctr++;
	}
	else if (mx7->ctr == mx7->max)
	{
		*mx7->ptr = 0x00;
		mx7->ctr++;
	}

	return mx;
}


LONG FmtNPut(STRPTR dest, STRPTR fmt, LONG maxlen, ...)
{
	va_list args;
	struct Mx7 mx7;

	if (maxlen == 0) return 0;

	mx7.ptr = dest;
	mx7.ctr = 0;
	mx7.max = maxlen - 1;
	va_start(args, maxlen);
	VNewRawDoFmt(fmt, putchproc, (STRPTR)&mx7, args);
	va_end(args);
	return mx7.ctr;
}

