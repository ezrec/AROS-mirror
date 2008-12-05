/***************************************************************************

 NListview.mcc - New Listview MUI Custom Class
 Registered MUI class, Serial Number: 1d51 (0x9d510020 to 0x9d51002F)

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

#define LIB_VERSION    19
#define LIB_REVISION   68

#define LIB_REV_STRING "19.68"
#define LIB_DATE       "01.07.2004"

#if defined(__PPC__)
  #if defined(__MORPHOS__)
    #define CPU " [MOS/PPC]"
  #else
    #define CPU " [OS4/PPC]"
  #endif
#elif defined(_M68060) || defined(__M68060) || defined(__mc68060)
  #define CPU " [060]"
#elif defined(_M68040) || defined(__M68040) || defined(__mc68040)
  #define CPU " [040]"
#elif defined(_M68030) || defined(__M68030) || defined(__mc68030)
  #define CPU " [030]"
#elif defined(_M68020) || defined(__M68020) || defined(__mc68020)
  #define CPU " [020]"
#else
  #define CPU ""
#endif

#define LIB_COPYRIGHT  "Copyright (c) 1996-2004 G.Masson, C.Scholling, P.Gruchala, S.Bauer and J.Langner"
