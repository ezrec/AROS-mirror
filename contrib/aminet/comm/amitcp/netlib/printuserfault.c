/* 
 * printuserfault.c --- Print a usergroup error message (DOS)
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sat Mar 20 02:10:14 1993 ppessi
 * Last modified: Mon Mar 28 02:26:31 1994 ppessi
 */

/****** net.lib/PrintUserFault ************************************************

    NAME
        PrintUserFault - socket error messages

    SYNOPSIS
        PrintUserFault(code, banner)
        void PrintUserFault(LONG, const UBYTE *)

    FUNCTION
        The PrintUserFault() function finds the error message corresponding to
        the code and writes it, followed by a newline, to the standard error
        or Output() filehandle. If the argument string is non-NULL it is
        preappended to the message string and separated from it by a colon and
        space (`: '). If string is NULL only the error message string is
        printed.

    NOTES
        The PrintUserFault() function used the DOS io functions.  It is
        recommended to use PrintUserFault() when the standard C IO functions
        are not otherwise in use.

    SEE ALSO
        strerror(), perror(), <netinclude:sys/errno.h>

******************************************************************************
*/

#include <errno.h>
#include <sys/my-errno.h>
#include <exec/execbase.h>
extern struct ExecBase *SysBase;
#include <exec/types.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#if __SASC
#include <proto/dos.h>
#include <proto/usergroup.h>
#elif __GNUC__
#include <proto/dos.h>
#include <proto/usergroup.h>
#endif

void PrintUserFault(LONG code, const UBYTE *banner)
{
  struct Process *p = (struct Process *)SysBase->ThisTask;
  BPTR Stderr = p->pr_CES ? p->pr_CES : p->pr_COS;

  if (banner != NULL) {
    FPuts(Stderr, banner);
    FPuts(Stderr, ": ");
  }
  FPuts(Stderr, ug_StrError(code));
  FPuts(Stderr, "\n");
  Flush(Stderr);
}

