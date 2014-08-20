//========================================================================
//
// GooMutex.h
//
// Portable mutex macros.
//
// Copyright 2002-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GMUTEX_H
#define GMUTEX_H

// Usage:
//
// GooMutex m;
// gInitMutex(&m);
// ...
// gLockMutex(&m);
//   ... critical section ...
// gUnlockMutex(&m);
// ...
// gDestroyMutex(&m);

#ifdef WIN32

#include <windows.h>

typedef CRITICAL_SECTION GooMutex;

#define gInitMutex(m) InitializeCriticalSection(m)
#define gDestroyMutex(m) DeleteCriticalSection(m)
#define gLockMutex(m) EnterCriticalSection(m)
#define gUnlockMutex(m) LeaveCriticalSection(m)

#else // assume pthreads

//#include <pthread.h>
#include <exec/semaphores.h>
#include <proto/exec.h>

typedef SignalSemaphore GooMutex;

#define gInitMutex(m) InitSemaphore(m)
#define gDestroyMutex(m)
#define gLockMutex(m) ObtainSemaphore(m)
#define gUnlockMutex(m) ReleaseSemaphore(m)

#endif

#endif
