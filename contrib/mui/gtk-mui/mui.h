/*****************************************************************************
 *
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 * 
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id$
 *
 *****************************************************************************/

#ifndef MGTK_MUI_H
#define MGTK_MUI_H

#if defined (__AROS__) || defined (__AMIGAOS4__) || defined (__MORPHOS__)
  #ifndef LIBRARIES_MUI_H
    #include <libraries/mui.h>
  #endif
  #include <proto/muimaster.h>
#else
  /* AmigaOS 3 */
  #include <clib/muimaster_protos.h>
#endif

#ifndef MUIA_Group_Forward
#define MUIA_Group_Forward                  0x80421422 /* V11 .s. BOOL              */
#endif

#ifndef MUII_ImageButtonBack
#define MUII_ImageButtonBack MUII_ButtonBack	/* MUII_ImageButtonBack not present in mui 3.8 */
#endif

#ifndef TEXTINPUT_MCC_H
#  define MCC_TI_TAGBASE  ((TAG_USER)|((1307<<16)+0x712))
#  define MCC_TI_ID(x)    (MCC_TI_TAGBASE+(x))
#  define MUIA_Textinput_MarkStart  MCC_TI_ID(133)             /* V13 isg ULONG */
#  define MUIA_Textinput_MarkEnd    MCC_TI_ID(134)             /* V13 isg ULONG */
#endif

#ifndef _parent
#define _parent(_o) ((Object *)xget(_o, MUIA_Parent))
#endif

#endif /* MGTK_MUI_H */
