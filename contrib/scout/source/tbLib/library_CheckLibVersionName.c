#include <exec/libraries.h>
#include <proto/exec.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "CheckLibVersionName()" */
ULONG CheckLibVersionName( CONST_STRPTR libname,
                           UWORD version,
                           UWORD revision )
{
    ULONG result;
    struct Library *lib;

    lib = OpenLibrary(libname, 0);
    result = CheckLibVersion(lib, version, revision);
    if (lib) CloseLibrary(lib);

    return result;
}
/* \\\ */

