#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "c2bstr()" */
void c2bstr( STRPTR c,
             BSTR b )
{
#if defined(__amigaos4__)
    CopyStringCToBSTR(c, b, 256);
#else
    STRPTR _b = (STRPTR)BADDR(b);

    if (c) {
        _b[0] = strlen(c);
        strncpy(_b + 1, c, _b[0]);
    } else {
        _b[0] = 0;
    }
    if (_b[0] < 255) {
        _b[_b[0] + 1] = 0;
    }
#endif
}
/* \\\ */

