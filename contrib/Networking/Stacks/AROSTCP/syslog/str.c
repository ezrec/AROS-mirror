#define __NOLIBBASE__

#include <proto/exec.h>
//#include <exec/rawfmt.h>

#include <stdarg.h>

extern struct Library *SysBase;


STRPTR strnew(APTR pool, STRPTR original)
{
	ULONG l = 1;
	STRPTR copy, p, s = original;

	if (original) {
		while (*original++) l++;

		if (copy = AllocVecPooled(pool, l))
		{
			p = copy;
			p--;
			s--;

			while (*++p = *++s);
		}
	} else {
		copy = AllocVecPooled(pool, 1);
		if (copy)
			*copy = '\0';
	}
	return copy;
}


STRPTR vfmtnew(APTR pool, STRPTR fmt, va_list args)
{
	ULONG l = 0;
	STRPTR s;
	va_list copy;

	__va_copy(copy, args);
//	VNewRawDoFmt(fmt, (APTR(*)(APTR, UBYTE))RAWFMTFUNC_COUNT, (STRPTR)&l, args);

	if (s = AllocVecPooled(pool, l + 1))
	{
//		VNewRawDoFmt(fmt, RAWFMTFUNC_STRING, s, copy);
	}
	return s;
}


STRPTR fmtnew(APTR pool, STRPTR fmt, ...)
{
	STRPTR s;
	va_list args;

	va_start(args, fmt);
	s = vfmtnew(pool, fmt, args);
	va_end(args);
	return s;
}



