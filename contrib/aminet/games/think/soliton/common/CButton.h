/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef INCLUDE_CBUTTON_H
#define INCLUDE_CBUTTON_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_CButton;

#define MUIM_CButton_Active   (TAGBASE_KAI | 0x3531)
#define MUIA_CButton_Text     (TAGBASE_KAI | 0x3532)
#define MUIA_CButton_Visible  (TAGBASE_KAI | 0x3533)
#define MUIA_CButton_Img1     (TAGBASE_KAI | 0x3534)
#define MUIA_CButton_Img2     (TAGBASE_KAI | 0x3535)

BOOL CButton_Init(void);
void CButton_Exit(void);

struct InternalImage
{
  const UBYTE * data;
  LONG          w;
  LONG          h;
  LONG          d;
  LONG          compression;
  LONG          mask;
  const ULONG * colors;
};

Object* CButtonObject(char* text, char* help, struct InternalImage *img1, struct InternalImage *img2);

#endif
