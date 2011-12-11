#include <exec/libraries.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "CheckLibVersion()" */
ULONG CheckLibVersion( struct Library *lib,
                       UWORD version,
                       UWORD revision )
{
    ULONG result = CLV_NOT_OPEN;

    if (lib) {
        if ((lib->lib_Version > version) || ((lib->lib_Version == version) && (lib->lib_Revision >= revision))) {
            result = CLV_NEWER_OR_SAME;
        } else {
            result = CLV_OLDER;
        }
    }

    return result;
}
/* \\\ */

