/*RCS_ID_C="$Id$";*/
/*
 * strerror.c --- network errno support for AmiTCP/IP
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sat Mar 20 02:03:45 1993 ppessi
 * Last modified: Sat Feb 26 19:53:12 1994 jraja
 *
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 3.5  1994/02/26  17:57:21  jraja
 * Fixed includes.
 *
 * Revision 3.4  1994/02/15  20:58:54  jraja
 * A little more efficient version.
 *
 * Revision 3.3  1994/02/03  19:23:54  ppessi
 * Fixed documentation and prototypes
 *
 * Revision 3.2  1994/01/23  22:58:35  jraja
 * Added autodoc.
 *
 */

#include <errno.h>
#include <bsdsocket.h>
#include <proto/socket.h>
#include <amitcp/socketbasetags.h>

/****** net.lib/strerror *****************************************************

    NAME
        strerror -- return the text for given error number

    SYNOPSIS
        string = strerror(error);

        char * strerror(int);

    FUNCTION
        This function returns pointer to the (English) string describing the
        error code given as argument. The error strings are defined for the
        error codes defined in <sys/errno.h>.

    NOTES
        The string pointed to by the return value should not be modified by
        the program, but may be overwritten by a subsequent call to this
        function.

    BUGS
        The strerror() prototype should be 
	const char *strerror(unsigned int); 
	However, the SAS C includes define it differently.

    SEE ALSO
        <netinclude:sys/errno.h>, perror(), PrintNetFault()
*****************************************************************************
*/

#ifdef notyet
const char *
strerror(unsigned int error)
#else
char *
strerror(int error)
#endif
{
  ULONG taglist[3];

  taglist[0] = SBTM_GETVAL(SBTC_ERRNOSTRPTR);
  taglist[1] = error;
  taglist[2] = TAG_END;

  SocketBaseTagList((struct TagItem *)taglist);
  return (char *)taglist[1];
}
