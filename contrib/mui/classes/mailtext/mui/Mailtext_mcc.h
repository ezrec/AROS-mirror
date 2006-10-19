/****************************************************************

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

 Mailtext_mcc.h

****************************************************************/

/*** Include stuff ***/

#ifndef MAILTEXT_MCC_H
#define MAILTEXT_MCC_H

#ifndef LIBRARIES_MUI_H
#include "libraries/mui.h"
#endif

/*** MUI Defines ***/

#define MUIC_Mailtext  "Mailtext.mcc"

#define MailtextObject MUI_NewObject(MUIC_Mailtext

#define MUIM_Mailtext_CopyToClip            0x8057013c
#define MUIA_Mailtext_IncPercent            0x80570103  /* v10 [ISG] */
#define MUIA_Mailtext_Text                  0x80570105  /* v10 [ISG] */
#define MUIA_Mailtext_QuoteChars            0x80570107  /* v10 [ISG] */
#define MUIA_Mailtext_ForbidContextMenu     0x80570136  /* v18 [I..] */
#define MUIA_Mailtext_ActionEMail           0x8057013a  /* v19 [..G] */
#define MUIA_Mailtext_ActionURL             0x8057013b  /* v18 [..G] */
#define MUIA_Mailtext_DisplayRaw            0x80570139  /* v18 [.SG] */
#define MUIA_Mailtext_Wordwrap              0x8057013d  /* v18 [.SG] */

#endif /* MAILTEXT_MCC_H */

