/*
 * xprzmodem_all.h - All Includes
 */

#ifndef __AROS__
#include <proto/all.h>
#endif
#include <stdio.h>

#ifdef __AROS__
#define __a0
#define __a1
#define __a3
#define __a6
#define __d0
#define __d1
#define __asm
#define __aligned
#define __saveds
#include <sdgstd.h>
#include <aros/oldprograms.h>
#endif

#include <exec/types.h>
#include <exec/exec.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <libraries/locale.h>
#include <dos/dos.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "Xproto.h"
#include "Zmodem.h"
#include "XprZmodem.h"

#ifndef __AROS__
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/locale_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/locale_pragmas.h>

#else

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/locale.h>
//#include <proto/xpr.h>

#endif

extern struct DosLibrary *DOSBase;
extern struct ExecBase *SysBase;
extern struct LocaleInfo li;


/* end of xprzmodem_all.h source */
