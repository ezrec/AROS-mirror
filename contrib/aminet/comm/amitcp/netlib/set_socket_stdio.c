/*
 * set_socket_stdio.c - redirect stdio to/from a socket
 *
 * Author: jraja <Jarno.Rajahalme@hut.fi>
 *
 * Copyright © 1994 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      :
 * Last modified:
 */

/****** net.lib/set_socket_stdio ****************************************

    NAME
        set_socket_stdio - redirect stdio to/from a socket

    SYNOPSIS
        int set_socket_stdio(int sock);

    FUNCTION
        Redirect stdio (stdin, stdout and stderr) to/from socket 'sock'.
        This is done by dup2()'ing 'sock' to the level 1 files underneath
        the stdin, stdout and stderr.

	The original 'sock' reference is closed on successful return, so
	the socket descriptor given as argument should not be used after
        this function returns (successfully).

    RETURN VALUES
        0 if successful, -1 on error. Global variable 'errno' contains
        the specific error code set by the failing function.

    NOTES
        This module pulls in the link library stdio modules.

    AUTHOR
        Jarno Rajahalme, the AmiTCP/IP Group <amitcp-group@hut.fi>,
        Helsinki University of Technology, Finland.

    SEE ALSO
        dup2()
*****************************************************************************
*
*/

#include <stdio.h>
#include <stdlib.h>

#include <bsdsocket.h>
#include <proto/socket.h>

int
set_socket_stdio(int sock)
{
  /*
   * Replace the stdio with the sock
   */
  if (sock >= 0
      && dup2(sock, fileno(stdin)) >= 0
      && dup2(sock, fileno(stdout)) >= 0
      && dup2(sock, fileno(stderr)) >= 0)
    {
      /* 
       * line buffered mode
       */
      setvbuf(stdin, NULL, _IOLBF, 0);
      setvbuf(stdout, NULL, _IOLBF, 0);
      setvbuf(stderr, NULL, _IOLBF, 0);
      /*
       * Close the original reference to the sock if necessary
       */
      if (sock > fileno(stderr))
	CloseSocket(sock);

      return 0;
    }
  return -1;
}
