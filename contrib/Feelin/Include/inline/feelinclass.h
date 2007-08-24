#ifndef _INLINE_FEELINCLASS_H
#define _INLINE_FEELINCLASS_H

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef FEELINCLASS_BASE_NAME
#define FEELINCLASS_BASE_NAME FeelinClassBase
#endif /* !FEELINCLASS_BASE_NAME */

#undef F_Query
#define F_Query(__p0, __p1) \
    LP2(30, struct TagItem *, F_Query, \
        ULONG , __p0, d0, \
        struct FeelinBase *, __p1, a0, \
        , FEELINCLASS_BASE_NAME)

#endif /* !_PPCINLINE_FEELINCLASS_H */
