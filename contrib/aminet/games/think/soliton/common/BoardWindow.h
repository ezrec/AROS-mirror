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

#ifndef INCLUDE_BOARDWIN_H
#define INCLUDE_BOARDWIN_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_BoardWindow;

#define MUIM_BoardWindow_Quit           (TAGBASE_KAI | 0x1101)
#define MUIM_BoardWindow_NewSettings    (TAGBASE_KAI | 0x1102)
#define MUIM_BoardWindow_NewGame        (TAGBASE_KAI | 0x1103)

#define MUIA_BoardWindow_ProfileManager (TAGBASE_KAI | 0x1111)  /* [I..} */
#define MUIA_BoardWindow_Statistics     (TAGBASE_KAI | 0x1112)  /* [I..} */
#define MUIA_BoardWindow_Settings       (TAGBASE_KAI | 0x1113)  /* [I..} */
#define MUIA_BoardWindow_About          (TAGBASE_KAI | 0x1114)  /* [I..} */

BOOL BoardWindow_Init(void);
void BoardWindow_Exit(void);

#endif
