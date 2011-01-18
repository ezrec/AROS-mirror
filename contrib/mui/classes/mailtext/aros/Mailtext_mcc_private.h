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

 Mailtext OpenSource project : http://sourceforge.net/projects/mailtext/

 $Id$

 Sourcecode for Mailtext.mcc

 MCC_Mailtext uses the following tags:

    0x805701xx

 Highest defined tagvalue:

 define MUICFG_Mailtext_Font         0x80570141

***************************************************************************/

/*
#define BETARELEASE "7"
*/
                                                                               /***/
/*** Include stuff ***/                                                        /***/
                                                                               /***/
#ifndef MAILTEXT_MCC_PRIVATE_H                                                 /***/
#define MAILTEXT_MCC_PRIVATE_H                                                 /***/
                                                                               /***/
#ifndef LIBRARIES_MUI_H                                                        /***/
#include "libraries/mui.h"                                                     /***/
#endif                                                                         /***/
                                                                               /***/
#include "DynStr.h"
#include "AttributesEngine.h"
#include "loc/Mailtext_mcc.h"
#include "URLs.h"

struct Data
{
    STRPTR          ptext ;

    UWORD           percent ;

    char            urlmagic[16] ;

    LONG            urlmagiclen,
                    actionstack ;

    BOOL            qcOverride,
                    actionBeep,
                    cmForbid,
                    displayRaw,
                    wrap ;

    String          text,
                    urlaction,
                    url,
                    email,
                    fontName,
                    wschars ;

    LONG            tpen,
                    upen,
                    qpens[5] ;

    Object          *menu,
                    *miWrap,
                    *miRaw ;

    Bitfield        mailBF ;

    struct AEParams params ;
    struct TextFont *font ;     // Font, der in den Prefs eingestellt wurde.
    LONG            setFont ;   // Font, der per [IS.] gesetzt wurde.


    struct Catalog  *catalog ;
};

#endif
