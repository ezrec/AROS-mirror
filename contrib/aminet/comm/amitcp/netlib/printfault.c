/* 
 * printfault.c --- Print a socket error message (DOS)
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sat Mar 20 02:10:14 1993 ppessi
 * Last modified: Thu Feb  3 12:56:20 1994 ppessi
 */

/****** net.lib/PrintNetFault ************************************************

    NAME
        PrintNetFault - socket error messages

    SYNOPSIS
        PrintNetFault(code, banner)
        void PrintNetFault(LONG, const UBYTE *)

    FUNCTION
        The PrintNetFault() function finds the error message corresponding
        to the code and writes it, followed by a newline, to the standard
        error or Output() filehandle. If the argument string is non-NULL it
        is preappended to the message string and separated from it by a
        colon and space (`: '). If string is NULL only the error message
        string is printed.

    NOTES
        The PrintNetFault() function uses the DOS IO functions.

    SEE ALSO
        strerror(), perror(), <netinclude:sys/errno.h>

******************************************************************************
*/

#include <errno.h>
#include <sys/my-errno.h>
#include <clib/netlib_protos.h>

#include <exec/execbase.h>
extern struct ExecBase *SysBase;
#include <exec/types.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#if __SASC
#include <proto/dos.h>
#elif __GNUC__
#include <proto/dos.h>
#endif

void 
PrintNetFault(LONG code, const UBYTE *banner)
{
  struct Process *p = (struct Process *)SysBase->ThisTask;
  BPTR Stderr = p->pr_CES ? p->pr_CES : p->pr_COS;
  const UBYTE *err = strerror(errno);

  if (banner != NULL) {
    FPuts(Stderr, banner);
    FPuts(Stderr, ": ");
  }
  FPuts(Stderr, err);
  FPuts(Stderr, "\n");
  Flush(Stderr);
}

