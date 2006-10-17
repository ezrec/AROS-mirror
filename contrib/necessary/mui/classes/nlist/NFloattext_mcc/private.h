#ifndef MUI_NFloattext_priv_MCC_H
#define MUI_NFloattext_priv_MCC_H

/***************************************************************************

 NFloattext.mcc - New Floattext MUI Custom Class
 Registered MUI class, Serial Number: 1d51 (0x9d5100a1 to 0x9d5100aF)

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#ifndef MUI_MUI_H
#include "mui.h"
#endif

#if defined(__AROS__)
  #include "NFloattext_mcc.h"
#elif defined(__PPC__)
  #pragma pack(2)
  #include "NFloattext_mcc.h"
  #pragma pack()
#else
  #include "NFloattext_mcc.h"
#endif

#include <mcc_common.h>
#include <mcc_debug.h>

#define ALIGN_MASK      (0x0700)

struct NFTData
{
  char *NFloattext_Text;
  LONG NFloattext_Align;
  LONG NFloattext_entry_len;
  char *NFloattext_entry;
  BOOL NFloattext_Justify;
  BOOL NFloattext_Copied;
};

#endif /* MUI_NFloattext_priv_MCC_H */

