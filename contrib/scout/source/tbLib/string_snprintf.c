#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include "SDI_compiler.h"

/* /// "StuffChar()" */
struct FormatContext
{
    STRPTR  fc_Index;
    LONG    fc_Size;
    BOOL    fc_Overflow;
};

#if defined(__AROS__) && !defined(NO_LINEAR_VARARGS)
#include <aros/asmcall.h>

static AROS_UFH2(VOID, StuffChar,
		AROS_UFHA(UBYTE, c, D0),
		AROS_UFHA(struct FormatContext *, fc, A3)
		)
{
    AROS_USERFUNC_INIT
#else
STATIC VOID ASM StuffChar( REG(d0, UBYTE c),
                           REG(a3, struct FormatContext *fc) )
{
#endif
    /* Is there still room? */
    if (fc->fc_Size > 0) {
        (*fc->fc_Index) = c;

        fc->fc_Index++;
        fc->fc_Size--;

        /* Is there only a single character left? */
        if (fc->fc_Size == 1) {
            /* Provide null-termination. */
            (*fc->fc_Index) = '\0';

            /* Don't store any further characters. */
            fc->fc_Size = 0;
        }
    } else {
        fc->fc_Overflow = TRUE;
    }
#if defined(__AROS__) && !defined(NO_LINEAR_VARARGS)
    AROS_USERFUNC_EXIT
#endif
}
/* \\\ */

/* /// "StuffCharMOS()" */
#if defined(__MORPHOS__)
STATIC VOID StuffCharMOS(void)
{
	StuffChar(REG_D0, (struct FormatContext *)REG_A3);
}

STATIC struct EmulLibEntry StuffCharTrap = { TRAP_LIBNR, 0, &StuffCharMOS };
#define StuffChar &StuffCharTrap
#endif
/* \\\ */

/* /// "_vsnprintf()" */
BOOL _vsnprintf( STRPTR buffer,
                 LONG maxlen,
                 CONST_STRPTR fmt,
                 VA_LIST args )
{
    BOOL result = FALSE;

    /* format a text, but place only up to MaxLen
     * characters in the output buffer (including
     * the terminating NUL)
     */

    if (maxlen > 1) {
        struct FormatContext fc;

        fc.fc_Index = buffer;
        fc.fc_Size = maxlen;
        fc.fc_Overflow = FALSE;

#ifdef NO_LINEAR_VARARGS
        VNewRawDoFmt(fmt, (VOID (*)())StuffChar, (APTR)&fc, args);
#else
	RawDoFmt(fmt, VA_ARG(args, APTR), (VOID (*)())StuffChar, (APTR)&fc);
#endif

        if (!fc.fc_Overflow) {
            result = TRUE;
        }
    }

    return result;
}
/* \\\ */

/* /// "_snprintf()" */
BOOL VARARGS68K STDARGS _snprintf( STRPTR buffer,
                                   LONG maxlen,
                                   CONST_STRPTR fmt, ... )
{
    VA_LIST args;
    BOOL result;

    /* format a text, varargs version */

    VA_START(args, fmt);
    result = _vsnprintf(buffer, maxlen, fmt, args);
    VA_END(args);

    return result;
}
/* \\\ */
