#ifndef DEFINES_INPUT_H
#define DEFINES_INPUT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/input/input.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for input
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __PeekQualifier_WB(__InputBase) \
        AROS_LC0(UWORD, PeekQualifier, \
        struct Device *, (__InputBase), 7, Input)

#define PeekQualifier() \
    __PeekQualifier_WB(InputBase)

#define __AddNullEvent_WB(__InputBase) \
        AROS_LC0NR(void, AddNullEvent, \
        struct Device *, (__InputBase), 20, Input)

#define AddNullEvent() \
    __AddNullEvent_WB(InputBase)

__END_DECLS

#endif /* DEFINES_INPUT_H*/
