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

#ifndef INCLUDE_SOLITON_H
#define INCLUDE_SOLITON_H

#include "MUITools.h"

#define MUIA_Soliton_Settings         (TAGBASE_KAI | 0x1001)
#define MUIA_Soliton_Stats            (TAGBASE_KAI | 0x1002)
#define MUIA_Soliton_Profile          (TAGBASE_KAI | 0x1003)

#define MUIM_Soliton_EditSettings     (TAGBASE_KAI | 0x1004)
#define MUIM_Soliton_About            (TAGBASE_KAI | 0x1005)
#define MUIM_Soliton_Statistics       (TAGBASE_KAI | 0x1006)

struct MUIP_Soliton_Statistics    {STACKED ULONG MethodID; STACKED LONG winner;};

extern struct MUI_CustomClass *CL_Soliton;

BOOL Soliton_Init(void);
void Soliton_Exit(void);

#endif
