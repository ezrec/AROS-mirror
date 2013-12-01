/*
** 2004 May 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This header file defines OS-specific features for Amiga-type OS
** AmigaOS3.x AmigaOS4 MorphOS
*/
#ifndef _SQLITE_OS_OTHER_H_
#define _SQLITE_OS_OTHER_H_

#include <exec/types.h>
#include <exec/semaphores.h>
#include <dos/dos.h>

#define DOS_SHARED_LOCK     	SHARED_LOCK
#define DOS_EXCLUSIVE_LOCK  	EXCLUSIVE_LOCK

#undef  SHARED_LOCK
#undef  EXCLUSIVE_LOCK


#if SQLITE_OS_OTHER

#define sqlite3DebugPrintf	    KPrintF
#endif


/*
** Maximum number of characters in a temporary file name
*/
#define SQLITE_TEMPNAME_SIZE 200


#endif /* _SQLITE_OS_OTHER_H_ */
