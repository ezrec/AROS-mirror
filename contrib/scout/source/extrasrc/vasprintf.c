#include <SDI/SDI_compiler.h>
#include <SDI/SDI_stdarg.h>
#include <proto/exec.h>

#if defined(__MORPHOS__) || defined(__AROS__)
#include <exec/rawfmt.h>

STRPTR VASPrintf( CONST_STRPTR fmt,
                  VA_LIST args )
{
	STRPTR buf = NULL;
	ULONG len = 0;

#ifdef NO_LINEAR_VARARGS
	va_list args2;

	__va_copy(args2, args);
	VNewRawDoFmt(fmt, (APTR)RAWFMTFUNC_COUNT, (APTR)&len, args2);
#else
	RawDoFmt(fmt, (APTR)args, (APTR)RAWFMTFUNC_COUNT, (APTR)&len);
#endif

	if (len > 0) {
        if ((buf = AllocVec(len, MEMF_ANY)) != NULL) {
#ifdef NO_LINEAR_VARARGS
		VNewRawDoFmt(fmt, (APTR)RAWFMTFUNC_STRING, (APTR)buf, args);
#else
	    	RawDoFmt(fmt, (APTR)args, (APTR)RAWFMTFUNC_STRING, (APTR)buf);
#endif
        }
	}

	return buf;
}

#else

struct FormatContext {
    ULONG fc_Length;
    STRPTR fc_Buffer;
};

STATIC VOID ASM CountChar( REG(d0, UNUSED UBYTE c),
                           REG(a3, struct FormatContext *fc) )
{
    fc->fc_Length++;
}

STATIC VOID ASM StuffChar( REG(d0, UBYTE c),
                           REG(a3, struct FormatContext *fc) )
{
    (*fc->fc_Buffer) = c;
    fc->fc_Buffer++;
}

STRPTR VASPrintf( CONST_STRPTR fmt,
                  VA_LIST args )
{
    struct FormatContext fc;
    STRPTR buf = NULL;

    fc.fc_Length = 0;
    RawDoFmt(fmt, (APTR)args, (VOID (*)())CountChar, (APTR)&fc);

	if (fc.fc_Length > 0) {
        if ((fc.fc_Buffer = AllocVec(fc.fc_Length, MEMF_CLEAR)) != NULL) {
            buf = fc.fc_Buffer;
            RawDoFmt(fmt, (APTR)args, (VOID (*)())StuffChar, (APTR)&fc);
        }
	}

	return buf;
}
#endif

STRPTR VARARGS68K STDARGS ASPrintf( CONST_STRPTR fmt, ... )
{
    VA_LIST args;
    STRPTR result;

    VA_START(args, fmt);
    result = VASPrintf(fmt, args);
    VA_END(args);

    return result;
}

