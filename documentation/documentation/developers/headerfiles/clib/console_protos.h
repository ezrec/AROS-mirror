#ifndef CLIB_CONSOLE_PROTOS_H
#define CLIB_CONSOLE_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/console/console.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <exec/io.h>
#include <devices/keymap.h>

__BEGIN_DECLS

AROS_LP2(struct InputEvent*, CDInputHandler,
         AROS_LPA(struct InputEvent*, events, A0),
         AROS_LPA(APTR, _cdihdata, A1),
         LIBBASETYPEPTR, ConsoleDevice, 7, Console
);
AROS_LP4(LONG, RawKeyConvert,
         AROS_LPA(struct InputEvent*, events, A0),
         AROS_LPA(STRPTR, buffer, A1),
         AROS_LPA(LONG, length, D1),
         AROS_LPA(struct KeyMap*, keyMap, A2),
         LIBBASETYPEPTR, ConsoleDevice, 8, Console
);

__END_DECLS

#endif /* CLIB_CONSOLE_PROTOS_H */
