/*
 * $Id$
 * 
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.20  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.19  1993/06/03  20:51:30  ppessi
 * Added RCS ID
 *
 * Revision 1.18  93/04/24  22:06:35  22:06:35  jraja (Jarno Tapio Rajahalme)
 * Removed inclusion of machine/ansi.h. Added #define _SIZE_T_ instead.
 * 
 * Revision 1.17  93/04/13  22:26:48  22:26:48  jraja (Jarno Tapio Rajahalme)
 * Removed replacing exec pragmas since it just doesn't work :-(
 * 
 * Revision 1.16  93/04/05  14:58:33  14:58:33  jraja (Jarno Tapio Rajahalme)
 * Added inline Forbid.
 * 
 * Revision 1.15  93/03/22  04:17:42  04:17:42  ppessi (Pekka Pessi)
 * Fixed include headaches, size_t kludge is hopefully dead and buried.
 * However, if you try to compile AmiTCP/IP with SAC SHORTINTS ....
 * 
 * Revision 1.14  93/03/20  18:43:25  18:43:25  ppessi (Pekka Pessi)
 * Added ExecBase
 * and SysBase
 * 
 * Revision 1.12  93/03/17  12:10:42  12:10:42  jraja (Jarno Tapio Rajahalme)
 * Added inline versions of AbortIO() and Remove().
 * 
 * Revision 1.11  93/03/17  09:58:16  09:58:16  jraja (Jarno Tapio Rajahalme)
 * Added inline AbortIO().
 * Changed SAS pragmas to use ioRequest->io_Device directly.
 * 
 * Revision 1.10  93/03/05  12:25:50  12:25:50  jraja (Jarno Tapio Rajahalme)
 * Defined TimerBase as struct Library for GCC, too.
 * 
 * Revision 1.9  93/03/05  10:56:04  10:56:04  jraja (Jarno Tapio Rajahalme)
 * Some changes made by Pekka.
 * 
 * Revision 1.8  93/03/05  03:25:26  03:25:26  ppessi (Pekka Pessi)
 * Compiles with SASC. Initial test version.
 * 
 * Revision 1.7  93/03/04  09:42:22  09:42:22  jraja (Jarno Tapio Rajahalme)
 * Fixed includes.
 * 
 * Revision 1.6  93/03/02  16:54:51  16:54:51  ppessi (Pekka Pessi)
 * Added dos/dos.h (break bit defines)
 * 
 * Revision 1.5  93/02/26  13:22:38  13:22:38  too (Tomi Ollila)
 * code checked w/ too, ppessi and jraja
 * 
 * Revision 1.4  93/02/04  18:33:18  18:33:18  jraja (Jarno Tapio Rajahalme)
 * Added needed include files.
 * 
 * Revision 1.3  93/01/06  19:05:12  19:05:12  jraja (Jarno Tapio Rajahalme)
 * added timer prototypes.
 * 
 * Revision 1.2  92/12/22  00:51:30  00:51:30  jraja (Jarno Tapio Rajahalme)
 * removed inline-keyword hassle (handled in sys/cdefs.h).
 * 
 * Revision 1.1  92/12/22  00:02:08  00:02:08  jraja (Jarno Tapio Rajahalme)
 * Initial revision
 * 
 */
#ifndef AMIGA_INCLUDES_H
#define AMIGA_INCLUDES_H

/*
 * Standard Amiga includes
 */
#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef EXEC_DEVICES_H
#include <exec/devices.h>
#endif

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif

#ifndef EXEC_IO_H
#include <exec/io.h>
#endif

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif

#ifndef PROTO_ALIB_H
#include <proto/alib.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef _CDEFS_H_
#include <sys/cdefs.h>
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _SYS_TIME_H_
#include <sys/time.h>
#endif

#if 0

/*
 * for built in functions in SASC
 */
#if __SASC
#define USE_BUILTIN_MATH
#ifndef _STRING_H
#include <string.h>
#endif
#elif __GNUC__
/* There is also built in functions in GNUC
 * -- however, we do not use them just now.
 *   Actually, we'll give 'em a shot. LW
 */
/*
#define _SIZE_T_        unsigned int            / sizeof() / (comment)

#ifdef _SIZE_T_
typedef _SIZE_T_ size_t;
#undef _SIZE_T_
#endif
*/
#ifndef NULL
#define NULL 0
#endif

#endif
/*
extern struct ExecBase *SysBase;
*/
/*
 * Amiga shared library prototypes
 */

#if __GNUC__

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif

#ifndef PROTO_TIMER_H
/*
 * predefine TimerBase to Library to follow SASC convention.
 */
/*
#define BASE_EXT_DECL extern struct Library * TimerBase;
#define BASE_NAME (struct Device *)TimerBase
*/
#include <inline/timer.h>
#endif

static inline VOID  
BeginIO(register struct IORequest *ioRequest)
{
  register struct IORequest *a1 __asm("a1") = ioRequest;
  register struct Device *a6 __asm("a6") = ioRequest->io_Device;
  __asm __volatile ("jsr a6@(-0x1e)"
  : /* no output */
  : "r" (a6), "r" (a1)
  : "a0","a1","d0","d1", "memory");
}

#elif __SASC
/*
#ifndef CLIB_EXEC_PROTOS_H
#include <clib/exec_protos.h>
#endif
#include <pragmas/exec_sysbase_pragmas.h>
#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif
*/
#pragma msg 93 ignore push

#if 0

extern VOID pragmaed_AbortIO(struct IORequest *);
#pragma libcall DeviceBase pragmaed_AbortIO 24 901

static inline __asm VOID 
AbortIO(register __a1 struct IORequest *ioRequest)
{
#define DeviceBase ioRequest->io_Device
  pragmaed_AbortIO(ioRequest);
#undef DeviceBase
}

#endif

extern VOID pragmaed_BeginIO(struct IORequest *);
#pragma libcall DeviceBase pragmaed_BeginIO 1E 901

static inline __asm VOID 
BeginIO(register __a1 struct IORequest *ioRequest)
{
#define DeviceBase ioRequest->io_Device
  pragmaed_BeginIO(ioRequest);
#undef DeviceBase
}

#pragma msg 93 pop

#endif


/*
 * common inlines for both compilers
 */

static inline VOID
NewList(register struct List *list)
{
  list->lh_Head = (struct Node *)&list->lh_Tail;
  list->lh_Tail = NULL;
  list->lh_TailPred = (struct Node *)list;
}

#endif /* 0 */




/*
 * undef math log, because it conflicts with log() used for logging.
 */
#undef log

#undef BASE_EXT_DECL
#undef BASE_NAME

#endif /* !AMIGA_INCLUDES_H */

