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

 Implementation for dynamic string handling

***************************************************************************/

#include "DynStr.h"
#include <string.h>
#include <proto/exec.h>
#include <exec/memory.h>

#define MAX_WASTE 64L

/*/// "void __regargs AllocString(String *Str,ULONG Length)" */

void REGARGS AllocString(String *Str,ULONG Length)
{
    Str->st_String=(char *)AllocVec(++Length, MEMF_CLEAR);
    Str->st_Length=Length;
}

/*\\\*/

/*/// "void REGARGS FreeString(String *Str)" */

void REGARGS FreeString(String *Str)
{
    if (Str->st_String)
    {
        FreeVec(Str->st_String) ;
        Str->st_String=NULL;
    }
}

/*\\\*/

/*/// "void __regargs DupString(String *Str,char *Ptr)" */

void REGARGS DupString(String *Str,char *Ptr)
{
    LONG Length;

    Length=strlen(Ptr);

    if ((Str->st_String == NULL) || (Length >= Str->st_Length) || ((Str->st_Length-Length) > MAX_WASTE))
    {
        FreeString(Str);

        AllocString(Str,Length);
    }

    (void)strcpy(Str->st_String,Ptr);
}

/*\\\*/

/*/// "void REGARGS DupStringN(String *Str,char *Ptr, ULONG Length)" */

void REGARGS DupStringN(String *Str,char *Ptr, ULONG Length)
{
    if ((Str->st_String == NULL) || (Length >= Str->st_Length) || ((Str->st_Length-Length) > MAX_WASTE))
    {
        FreeString(Str);

        AllocString(Str,Length);
    }

    (void)strncpy(Str->st_String,Ptr,Length);
    Str->st_String[Length] = '\0' ;
}

/*\\\*/



