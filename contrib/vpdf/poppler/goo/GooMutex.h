//========================================================================
//
// GooMutex.h
//
// Portable mutex macros.
//
// Copyright 2002-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2009 Kovid Goyal <kovid@kovidgoyal.net>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
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

#ifdef _WIN32

#include <windows.h>

typedef CRITICAL_SECTION GooMutex;

#define gInitMutex(m) InitializeCriticalSection(m)
#define gDestroyMutex(m) DeleteCriticalSection(m)
#define gLockMutex(m) EnterCriticalSection(m)
#define gUnlockMutex(m) LeaveCriticalSection(m)

#else // MorphOS

#include <exec/semaphores.h>
#include <proto/exec.h>

typedef SignalSemaphore GooMutex;

#define gInitMutex(m) InitSemaphore(m)
#define gDestroyMutex(m)
#define gLockMutex(m) ObtainSemaphore(m)
#define gUnlockMutex(m) ReleaseSemaphore(m)


#endif

class MutexLocker {
public:
  MutexLocker(GooMutex *mutexA) : mutex(mutexA) { gLockMutex(mutex); }
  ~MutexLocker() { gUnlockMutex(mutex); }

private:
  GooMutex *mutex;
};


#endif
