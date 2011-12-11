#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "b2cstr()" */
void b2cstr( BSTR b,
             STRPTR c )
{
#if defined(__amigaos4__)
    CopyStringBSTRToC(b, c, 256);
#else
    STRPTR _b = (STRPTR)BADDR(b);

    strncpy(c, &_b[1], _b[0]);
    c[_b[0]] = 0x00;
#endif
}
/* \\\ */

