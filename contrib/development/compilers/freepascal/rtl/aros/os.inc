{
    $Id$
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by the Free Pascal development team.

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}
{$define aros}
{$undef amiga}
{$undef go32v2}
{$undef os2}
{$undef linux}
{$undef win32}
{$undef macos}
{$undef atari}

{
  $Log$
  Revision 1.2  2002/02/27 08:45:39  dgs
  Work in progress.

  Revision 1.1.1.1  2002/02/19 08:24:54  sasu
  Initial import. See readme.aros in freepascal/rtl/aros.

  Compiler have target AROS now. Left to do is hundreds of m68k asm parts of system includes, mainly libcall wrappers.
  To find places where to make AROS specifig version, seek [AROS VERSION HERE] string and write i386 version of function or
  procedure.

  Revision 1.1  2000/07/13 06:30:28  michael
  + Initial import

  Revision 1.4  2000/02/09 16:59:28  peter
    * truncated log

  Revision 1.3  2000/01/07 16:41:29  daniel
    * copyright 2000

  Revision 1.2  2000/01/07 16:32:22  daniel
    * copyright 2000 added

}
