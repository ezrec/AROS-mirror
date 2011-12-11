#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "b2cstrn()" */
void b2cstrn( BSTR b,
              STRPTR c,
              ULONG maxlen )
{
#if defined(__amigaos4__)
    CopyStringBSTRToC(b, c, maxlen);
#else
    STRPTR _b = (STRPTR)BADDR(b);
    ULONG len;

    len = _b[0];
    if (len >= maxlen) len = maxlen - 1;

    strncpy(c, &_b[1], len);
    c[len] = 0x00;
#endif
}
/* \\\ */

