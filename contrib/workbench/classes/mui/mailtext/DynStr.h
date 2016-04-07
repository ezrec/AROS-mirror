/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 Based upon code written by Matthias Scheler <matthias@scheler.de>

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

 Header for dynamic string handling

***************************************************************************/

#ifndef DYNSTR_H
#define DYNSTR_H

#include <dos/dos.h>
#ifdef __AROS__
#define REGARGS
#else
#include <SDI_compiler.h>
#endif

typedef struct StringStruct
 {
  char *st_String;
  ULONG st_Length;
 } String;

#define STR(s) ((s).st_String)
#define INIT(s) (s).st_String=NULL

void REGARGS FreeString(String *String);
void REGARGS AllocString(String *,ULONG Length);

void REGARGS DupString(String *,char *Ptr);
void REGARGS DupStringN(String *Str,char *Ptr, ULONG Length) ;

#endif /* DYNSTR_H */

