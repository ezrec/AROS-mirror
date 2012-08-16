/* libvstring/fmtnew.c */

#include "libvstring.h"


STRPTR FmtNew(STRPTR fmt, ...)
{
	STRPTR s;
	va_list args;

	va_start(args, fmt);
	s = VFmtNew(fmt, args);
	va_end(args);
	return s;
}

