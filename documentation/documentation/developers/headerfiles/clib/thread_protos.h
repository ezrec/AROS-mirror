#ifndef CLIB_THREAD_PROTOS_H
#define CLIB_THREAD_PROTOS_H

/*
    *** Automatically generated from 'thread.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <libraries/thread.h>
#include <stdint.h>
AROS_LP2(uint32_t, CreateThread,
         AROS_LPA(ThreadEntryFunction, entry, A0),
         AROS_LPA(void *, data, A1),
         LIBBASETYPEPTR, ThreadBase, 5, Thread
);
AROS_LP2(BOOL, WaitThread,
         AROS_LPA(uint32_t, thread_id, D0),
         AROS_LPA(void **, result, A0),
         LIBBASETYPEPTR, ThreadBase, 6, Thread
);
AROS_LP0(void, WaitAllThreads,
         LIBBASETYPEPTR, ThreadBase, 7, Thread
);
AROS_LP1(BOOL, DetachThread,
         AROS_LPA(uint32_t, thread_id, D0),
         LIBBASETYPEPTR, ThreadBase, 8, Thread
);
AROS_LP0(uint32_t, CurrentThread,
         LIBBASETYPEPTR, ThreadBase, 9, Thread
);
AROS_LP0(void *, CreateMutex,
         LIBBASETYPEPTR, ThreadBase, 10, Thread
);
AROS_LP1(BOOL, DestroyMutex,
         AROS_LPA(void *, mutex, A0),
         LIBBASETYPEPTR, ThreadBase, 11, Thread
);
AROS_LP1(void, LockMutex,
         AROS_LPA(void *, mutex, A0),
         LIBBASETYPEPTR, ThreadBase, 12, Thread
);
AROS_LP1(BOOL, TryLockMutex,
         AROS_LPA(void *, mutex, A0),
         LIBBASETYPEPTR, ThreadBase, 13, Thread
);
AROS_LP1(void, UnlockMutex,
         AROS_LPA(void *, mutex, A0),
         LIBBASETYPEPTR, ThreadBase, 14, Thread
);
AROS_LP0(void *, CreateCondition,
         LIBBASETYPEPTR, ThreadBase, 15, Thread
);
AROS_LP1(BOOL, DestroyCondition,
         AROS_LPA(void *, cond, A0),
         LIBBASETYPEPTR, ThreadBase, 16, Thread
);
AROS_LP2(BOOL, WaitCondition,
         AROS_LPA(void *, cond, A0),
         AROS_LPA(void *, mutex, A1),
         LIBBASETYPEPTR, ThreadBase, 17, Thread
);
AROS_LP1(void, SignalCondition,
         AROS_LPA(void *, cond, A0),
         LIBBASETYPEPTR, ThreadBase, 18, Thread
);
AROS_LP1(void, BroadcastCondition,
         AROS_LPA(void *, cond, A0),
         LIBBASETYPEPTR, ThreadBase, 19, Thread
);

#endif /* CLIB_THREAD_PROTOS_H */
