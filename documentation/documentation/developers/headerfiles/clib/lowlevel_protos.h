#ifndef CLIB_LOWLEVEL_PROTOS_H
#define CLIB_LOWLEVEL_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/lowlevel/lowlevel.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <libraries/lowlevel.h>
#include <devices/timer.h>
#include <utility/tagitem.h>

__BEGIN_DECLS

AROS_LP1(ULONG, ReadJoyPort,
         AROS_LPA(ULONG, port, D0),
         LIBBASETYPEPTR, LowLevelBase, 5, LowLevel
);
AROS_LP0(ULONG, GetLanguageSelection,
         LIBBASETYPEPTR, LowLevelBase, 6, LowLevel
);
AROS_LP0(ULONG, GetKey,
         LIBBASETYPEPTR, LowLevelBase, 8, LowLevel
);
AROS_LP2(void, QueryKeys,
         AROS_LPA(struct KeyQuery *, queryArray, A0),
         AROS_LPA(UBYTE, arraySize, D1),
         LIBBASETYPEPTR, LowLevelBase, 9, LowLevel
);
AROS_LP2(APTR, AddKBInt,
         AROS_LPA(APTR, intRoutine, A0),
         AROS_LPA(APTR, intData, A1),
         LIBBASETYPEPTR, LowLevelBase, 10, LowLevel
);
AROS_LP1(void, RemKBInt,
         AROS_LPA(APTR, intHandle, A1),
         LIBBASETYPEPTR, LowLevelBase, 11, LowLevel
);
AROS_LP2(APTR, AddTimerInt,
         AROS_LPA(APTR, intRoutine, A0),
         AROS_LPA(APTR, intData, A1),
         LIBBASETYPEPTR, LowLevelBase, 13, LowLevel
);
AROS_LP1(void, RemTimerInt,
         AROS_LPA(APTR, intHandle, A1),
         LIBBASETYPEPTR, LowLevelBase, 14, LowLevel
);
AROS_LP1(void, StopTimerInt,
         AROS_LPA(APTR, intHandle, A1),
         LIBBASETYPEPTR, LowLevelBase, 15, LowLevel
);
AROS_LP3(void, StartTimerInt,
         AROS_LPA(APTR, intHandle, A1),
         AROS_LPA(ULONG, timeInterval, D0),
         AROS_LPA(BOOL, continuous, D1),
         LIBBASETYPEPTR, LowLevelBase, 16, LowLevel
);
AROS_LP1(ULONG, ElapsedTime,
         AROS_LPA(struct EClockVal *, context, A0),
         LIBBASETYPEPTR, LowLevelBase, 17, LowLevel
);
AROS_LP2(APTR, AddVBlankInt,
         AROS_LPA(APTR, intRoutine, A0),
         AROS_LPA(APTR, intData, A1),
         LIBBASETYPEPTR, LowLevelBase, 18, LowLevel
);
AROS_LP1(void, RemVBlankInt,
         AROS_LPA(APTR, intHandle, A1),
         LIBBASETYPEPTR, LowLevelBase, 19, LowLevel
);
AROS_LP2(BOOL, SetJoyPortAttrsA,
         AROS_LPA(ULONG, portNumber, D0),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, LowLevelBase, 22, LowLevel
);

__END_DECLS

#endif /* CLIB_LOWLEVEL_PROTOS_H */
