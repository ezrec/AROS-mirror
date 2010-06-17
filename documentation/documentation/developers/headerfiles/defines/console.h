#ifndef DEFINES_CONSOLE_H
#define DEFINES_CONSOLE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/console/console.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for console
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __CDInputHandler_WB(__ConsoleDevice, __arg1, __arg2) \
        AROS_LC2(struct InputEvent*, CDInputHandler, \
                  AROS_LCA(struct InputEvent*,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
        struct Device *, (__ConsoleDevice), 7, Console)

#define CDInputHandler(arg1, arg2) \
    __CDInputHandler_WB(ConsoleDevice, (arg1), (arg2))

#define __RawKeyConvert_WB(__ConsoleDevice, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, RawKeyConvert, \
                  AROS_LCA(struct InputEvent*,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(struct KeyMap*,(__arg4),A2), \
        struct Device *, (__ConsoleDevice), 8, Console)

#define RawKeyConvert(arg1, arg2, arg3, arg4) \
    __RawKeyConvert_WB(ConsoleDevice, (arg1), (arg2), (arg3), (arg4))

__END_DECLS

#endif /* DEFINES_CONSOLE_H*/
