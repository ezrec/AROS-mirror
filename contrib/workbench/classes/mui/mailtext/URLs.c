/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

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

 Implementation for URL handling

***************************************************************************/

#include "URLs.h"
#include "Bitfield.h"

#include <proto/utility.h>

/*/// "BOOL IsURL(STRPTR str, const Bitfield *urlchars)" */

BOOL IsURL(STRPTR str, const Bitfield *urlchars)
{
    BOOL res ;

    switch (ToLower(str[0]))
    {
        case 'h' :
            res = ((!Strnicmp(str, "http://",      7)) && BF_IsSet(urlchars, str[7])) ;
            break ;

        case 'f' :
            res = (((!Strnicmp(str, "ftp://" ,      6)) && BF_IsSet(urlchars, str[6]))
                || ((!Strnicmp(str, "file://",      7)) && BF_IsSet(urlchars, str[7])));
            break ;

        case 'g' :
            res = ((!Strnicmp(str, "gopher://",    9)) && BF_IsSet(urlchars, str[9])) ;
            break ;

        case 'm' :
            res = ((!Strnicmp(str, "mailto:",      7)) && BF_IsSet(urlchars, str[7])) ;
            break ;

        case 'n' :
            res = (((!Strnicmp(str, "news:",        5)) && BF_IsSet(urlchars, str[5]))
                || ((!Strnicmp(str, "nntp://",      7)) && BF_IsSet(urlchars, str[7]))) ;
            break ;

        case 't' :
            res = ((!Strnicmp(str, "telnet://",    9)) && BF_IsSet(urlchars, str[9])) ;
            break ;

        case 'w' :
            res = ((!Strnicmp(str, "wais://",      7)) && BF_IsSet(urlchars, str[7])) ;
            break ;

        case 'p' :
            res = ((!Strnicmp(str, "prospero://", 11)) && BF_IsSet(urlchars, str[11])) ;
            break ;

        default:
            res = FALSE ;
            break ;
    }

    return (res) ;
}

/*\\\*/

/*/// "BOOL IsEMail(STRPTR str, const Bitfield *emailchars)" */

BOOL IsEMail(STRPTR str, const Bitfield *emailchars)
{
    BOOL foundAt = FALSE, foundDot = FALSE ;
    LONG i = 0 ;

    while (str[i] && BF_IsSet(emailchars, str[i]))
    {
        if (str[i] == '@')
        {
            if (!foundAt)
            {
                foundAt = TRUE ;
            }
            else
            {
                return FALSE ;
            }
        }

        if (foundAt && (str[i] == '.'))
        {
            foundDot = TRUE ;
        }

        i++ ;
    }

    return (BOOL) (foundDot && foundAt) ;
}

/*\\\*/

