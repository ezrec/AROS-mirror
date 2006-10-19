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

 Header for the attribute engine

***************************************************************************/
#ifndef ATTRIBENGINE_H
#define ATTRIBENGINE_H

#include <exec/types.h>
#include "Bitfield.h"
#include "DynStr.h"

/* Maximum linesize */

#define AE_LINESIZE 4096

struct AEAttrData
{
    BOOL    active ;        /* Attribute currently active? Initialize with FALSE! */
    char    set[16],        /* String that sets the attribute */
            unset[16] ;     /* String that unsets the attribute */
    UWORD   dist,           /* Maximum distance between introducing and terminating chars */
            setSize,        /* Size of set-String */
            unsetSize ;     /* Size of unset-String */
} ;

struct AEParams
{
    BOOL               showAttrs,           /* Show attributechars if the attribute has been set? */
                       underlineURL,        /* underline URLs? */
                       separateSig ;        /* replace signature separators by a given string? */
    String             dpattern,            /* disable parsing if a line matches this pattern */
                       epattern,            /* re-enable parsing if a line matches this pattern */
                       spattern ;           /* pattern for recognition of signature separators */
    Bitfield           scharsBF,            /* Non-Introducing Chars */
                       tcharsBF,            /* Terminating Chars */
                       wscharsBF,           /* Whitespace Chars */
                       attsBF,              /* Attributechars, should always be "_*#/" */
                       qcharsBF,            /* Quote Chars */
                       urlBF ;              /* legal URL Chars */
    char               qchars[8] ;          /* Quote Chars as a string */
    char               sigSeparator[8] ;    /* String to be inserted in place of a signature separator (if separateSig == TRUE) */
    struct AEAttrData  bdata,               /* Parameters for Bold */
                       idata,               /* Parameters for Italics */
                       udata,               /* Parameters for Underline */
                       qcolors[5],          /* Parameters for the colors of five quotelevels, (dist unused) */
                       tcolor,              /* Parameters for highlighted text */
                       ucolor ;             /* Parameters for for URLs (dist unused) */
    LONG               qdist;               /* Quote distance */
};

BOOL AE_SetAttributes(const STRPTR src, STRPTR dst, long dstsize, struct AEParams *p) ;

#endif /* ATTRIBENGINE_H */

