/* Automatically generated SDI stubs! Do not edit! */

#include <exec/types.h>
#include <utility/tagitem.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <aros/libcall.h>
#include <SDI_lib.h>

#include "lib_protos.h"

AROS_LH0(BOOL, MB_Open,
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC_NP(MB_Open);
    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, MB_Close,
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC_NP(MB_Close);
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, MB_GetA,
    AROS_LHA(struct TagItem *, ___TagList, A1),
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC(MB_GetA, ___TagList);
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MB_GetVarInfoA,
    AROS_LHA(ULONG, ___varnb, D0),
    AROS_LHA(struct TagItem *, ___TagList, A1),
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC(MB_GetVarInfoA, ___varnb, ___TagList);
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MB_GetNextCode,
    AROS_LHA(ULONG *, ___type, A0),
    AROS_LHA(char **, ___code, A1),
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC(MB_GetNextCode, ___type, ___code);
    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, MB_GetNextNotify,
    AROS_LHA(ULONG *, ___type, A0),
    AROS_LHA(char **, ___code, A1),
    struct LibraryHeader *, __BASE_OR_IFACE_VAR, 0, LIBSTUB
)
{
    AROS_LIBFUNC_INIT
    return CALL_LFUNC(MB_GetNextNotify, ___type, ___code);
    AROS_LIBFUNC_EXIT
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
