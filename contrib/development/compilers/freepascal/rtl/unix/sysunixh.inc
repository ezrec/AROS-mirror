{
    $Id$
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by Michael Van Canneyt,
    member of the Free Pascal development team.

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This is the core of the system unit *nix systems (now FreeBSD
     and Unix).

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}


{$ifdef m68k}
{ used for single computations }
const
  BIAS4 = $7f-1;
{$endif}

{$define newsignal}

{$I systemh.inc}
{$I heaph.inc}

const
  UnusedHandle    = -1;
  StdInputHandle  = 0;
  StdOutputHandle = 1;
  StdErrorHandle  = 2;

var
  argc : longint;
  argv : ppchar;
  envp : ppchar;


{
   $Log$
   Revision 1.1  2002/02/19 08:26:21  sasu
   Initial revision

   Revision 1.1.2.1  2000/09/16 11:01:17  marco
    *  Split syslinux into sysunix.inc and sysunixh.inc


}