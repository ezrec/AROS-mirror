/*
 * xprzmodem_all.h - All Includes
 */

#include <proto/all.h>
#include <stdio.h>

#include <exec/types.h>
#include <exec/exec.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <libraries/locale.h>
#include <dos/dos.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "xproto.h"
#include "zmodem.h"
#include "xprzmodem.h"

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/locale_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/locale_pragmas.h>

extern struct DosLibrary *DOSBase;
extern struct ExecBase *SysBase;
extern struct LocaleInfo li;

/* end of xprzmodem_all.h source */
