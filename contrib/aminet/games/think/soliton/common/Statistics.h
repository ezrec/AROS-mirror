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

#ifndef INCLUDE_STATISTICS_H
#define INCLUDE_STATISTICS_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_Statistics;

#define MUIM_Statistics_ClearScores (TAGBASE_KAI | 0x2525)
#define MUIA_Statistics_Winner      (TAGBASE_KAI | 0x2526)
#define MUIM_Statistics_Enter       (TAGBASE_KAI | 0x2527)

#define MUIV_Statistics_Close       (TAGBASE_KAI | 0x2523)

BOOL Statistics_Init(void);
void Statistics_Exit(void);

#endif
