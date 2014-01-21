/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(unsigned long, inet_addr,

/*  SYNOPSIS */
        AROS_LHA(const char *, cp, A0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 30, BSDSocket)

/*  FUNCTION
        The routines inet_addr() and inet_network()  each  interpret
        character  strings  representing  numbers  expressed  in the
        Internet standard `.' notation, returning  numbers  suitable
        for  use as Internet addresses and Internet network numbers,
        respectively.  The routine inet_makeaddr() takes an Internet
        network number and a local network address and constructs an
        Internet address from it.   The  routines  inet_netof()  and
        inet_lnaof()  break apart Internet host addresses, returning
        the network number and local network address  part,  respec-
        tively.

        The routine inet_ntoa() returns a pointer to a string in the
        base 256 notation ``d.d.d.d'' described below.

        All Internet address are returned in  network  order  (bytes
        ordered  from left to right).  All network numbers and local
        address parts are returned as machine format integer values.

        Values specified using the `.'  notation  take  one  of  the
        following forms:

             a.b.c.d
             a.b.c
             a.b
             a

        When four parts are specified, each is interpreted as a byte
        of data and assigned, from left to right, to  the four bytes
        of  an Internet address.  Note: when  an Internet address is
        viewed  as  a  32-bit  integer  quantity  on  little  endian
        systems,  the  bytes referred to  above appear  as  d.c.b.a.
        bytes are ordered from right to left.

        When a three part address is specified,  the  last  part  is
        interpreted  as  a  16-bit  quantity and placed in the right
        most two bytes of the network address.  This makes the three
        part  address  format convenient for specifying Class B net-
        work addresses as "128.net.host".

        When a two part address is supplied, the last part is inter-
        preted  as  a  24-bit  quantity and placed in the right most
        three bytes of the network address.  This makes the two part
        address  format  convenient  for  specifying Class A network
        addresses as "net.host".
         
        When only one part is given, the value is stored directly in
        the network address without any byte rearrangement.
        All numbers supplied as ``parts'' in a `.' notation  may  be
        decimal,  octal,  or  hexadecimal,  as  specified  in  the C
        language (that is, a leading 0x or 0X  implies  hexadecimal;
        otherwise,  a leading 0 implies octal; otherwise, the number
        is interpreted as decimal).

    INPUTS

        cp  - Pointer to string to convert

    RESULT

        The value -1 is returned by inet_addr()  and  inet_network()
        for malformed requests.

    NOTES

        Return  value  of  Inet_MakeAddr()  and  argument  types  of
        Inet_LnaOf(), Inet_NetOf() and Inet_NtoA() are ULONGs instead
        of  struct  in_addr.  The original behaviour  is achieved by
        using  included  stub  functions (lower case function names)
        which handle structure arguments.

    EXAMPLE

    BUGS

        The problem of host byte ordering versus network byte ordering
        is  confusing.  A simple way to specify Class C network addresses
        in a manner similar to that for Class B and Class A is needed.

        The return value from inet_ntoa() points to static buffer
        which  is  overwritten  in  each inet_ntoa() call.

    SEE ALSO

        Inet_LnaOf(), inet_lnaof(), inet_MakeAddr(), inet_makeaddr(),
        Inet_NetOf(), inet_netof(), inet_network(), Inet_NtoA(),
        inet_ntoa(), 

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT

} /* inet_addr */
