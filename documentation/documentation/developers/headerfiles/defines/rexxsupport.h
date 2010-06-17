#ifndef DEFINES_REXXSUPPORT_H
#define DEFINES_REXXSUPPORT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/rexxsupport/rexxsupport.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for rexxsupport
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __ArexxDispatch_WB(__RexxSupportBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ArexxDispatch, \
                  AROS_LCA(struct RexxMsg *,(__arg1),A0), \
                  AROS_LCA(STRPTR *,(__arg2),A1), \
        struct Library *, (__RexxSupportBase), 5, RexxSupport)

#define ArexxDispatch(arg1, arg2) \
    __ArexxDispatch_WB(RexxSupportBase, (arg1), (arg2))

#define __ClosePortRsrc_WB(__RexxSupportBase, __arg1) \
        AROS_LC1NR(void, ClosePortRsrc, \
                  AROS_LCA(struct RexxRsrc *,(__arg1),A0), \
        struct Library *, (__RexxSupportBase), 6, RexxSupport)

#define ClosePortRsrc(arg1) \
    __ClosePortRsrc_WB(RexxSupportBase, (arg1))

__END_DECLS

#endif /* DEFINES_REXXSUPPORT_H*/
