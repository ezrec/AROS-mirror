#ifndef __AROS__
static char *RCS_ID_C="$Id$";
#endif
/*
 * autoinit.c --- SAS C auto initialization functions
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 * Modified: kjk <klei0001@uni-trier.de>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sat Mar 20 03:31:29 1993 ppessi
 * Last modified: Wed May 25 17:51:41 1994 kjk
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/libraries.h>

#include <intuition/intuition.h>

#include <clib/socket_protos.h>
#include <proto/socket.h>
#include <proto/exec.h>
#include <proto/intuition.h>


struct Library *SocketBase = NULL;

int errno = 0;                  /* global errno variable */

CONST_STRPTR  SOCKETNAME = (CONST_STRPTR) "bsdsocket.library";

#define SOCKETVERSION 2         /* minimum bsdsocket version to use */

extern STRPTR _ProgramName;     /* SAS startup module defines this :-) */

/****** net.lib/autoinit *********************************************
*
*   NAME   
*       autoinit - SAS C Autoinitialization Functions 
*
*   SYNOPSIS
*       _STIopenSockets()
*
*       void _STIopenSockets(void)
*
*       _STDcloseSockets()
*
*       void _STDcloseSockets(void)
*
*   FUNCTION
*       These functions open and close the bsdsocket.library at the
*       startup and exit of the program, respectively. For a
*       program to use these functions, it must be linked with
*       netlib:net.lib.
*
*       If the library can be opened, the _STIopenSockets() calls
*       bsdsocket.library function SetErrnoPtr() to tell the
*       library the address and the size of the errno variable of
*       the calling program. 
*
*   NOTES
*       _STIopenSockets() also checks that the system version is at
*       least 37. It puts up a requester if the bsdsocket.library
*       is not found or is of wrong version.
*
*       The autoinitialization and autotermination functions are
*       features specific to the SAS C6. However, these functions
*       can be used with other (ANSI) C compilers, too. Example
*       follows: 
*
*       \* at start of main() *\
*
*       atexit(_STDcloseSockets);
*       _STDopenSockets();
*
*   BUGS
*
*   SEE ALSO
*       bsdsocket.library/SetErrnoPtr(),
*       SAS/C 6 User's Guide p. 145 for details of
*       autoinitialization and autotermination functions.  
*****************************************************************************
*
*/

/*
 * Using __stdargs prevents creation of register arguments entry point.
 * If both stack args and reg. args entry points are created, this
 * function is called _twice_, which is not wanted.
 */
int openSockets(void)
{

  /*
   * Check OS version
   */
#if 0
  if ((*(struct Library **)4)->lib_Version < 37)
    return(-1); /* well, in case of failure, anything but 0 may be returned */
#endif

  /*
   * Open bsdsocket.library
   */
  if ((SocketBase = OpenLibrary(SOCKETNAME, SOCKETVERSION)) != NULL) {
    /*
     * Succesfull. Now tell bsdsocket.library the address of our errno
     */
    SetErrnoPtr(&errno, sizeof(errno));
    
    return 1;
  }

#ifndef __AROS__
  STRPTR errorStr;
  errorStr = (STRPTR) "AmiTCP/IP version 2 or later must be started first.";
  
  if (IntuitionBase != NULL) {
    struct EasyStruct libraryES;
    
    libraryES.es_StructSize = sizeof(libraryES);
    libraryES.es_Flags = 0;
    libraryES.es_Title = _ProgramName;
    libraryES.es_TextFormat = errorStr;
    libraryES.es_GadgetFormat = (STRPTR) "Exit %s";
    
    EasyRequestArgs(NULL, &libraryES, NULL, (APTR)&_ProgramName);

  }
#else
  fprintf(stderr, "ERROR: could not open %s\n", SOCKETNAME);
  fprintf(stderr, "No network stack available!\n");
#endif
  return 0;
}

void 
closeSockets(void)
{
  if (SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = NULL;
  }
}

ADD2EXIT(closeSockets, 0);
