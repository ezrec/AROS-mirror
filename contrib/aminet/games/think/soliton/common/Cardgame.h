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

#ifndef INCLUDE_CARDGAME_H
#define INCLUDE_CARDGAME_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_Cardgame;

/*** Methods ***/

#define MUIM_Cardgame_DragStart       0x818f0001    // overridable
#define MUIM_Cardgame_DragAccept      0x818f0002    // overridable
#define MUIM_Cardgame_DragDone        0x818f0003    // overridable
#define MUIM_Cardgame_ClickCard       0x818f0004    // overridable

#define MUIM_Cardgame_SetGraphic      0x818f0011
#define MUIM_Cardgame_SetCards        0x818f0012
#define MUIM_Cardgame_GetCards        0x818f0013
#define MUIM_Cardgame_MoveCards       0x818f0014
#define MUIM_Cardgame_SetEmptyImage   0x818f0015    // private

#define MUIM_Cardgame_TimerTick       0x818f0021    // overridable
#define MUIM_Cardgame_TimerReset      0x818f0022
#define MUIM_Cardgame_TimerEvent      0x818f0023    // private

/*** Method structs ***/

struct MUIP_Cardgame_DragStart      {STACKED ULONG ID; STACKED LONG source; STACKED LONG* size;};
struct MUIP_Cardgame_DragAccept     {STACKED ULONG ID; STACKED LONG source; STACKED LONG dest; STACKED LONG size; STACKED int* cards;};
struct MUIP_Cardgame_DragDone       {STACKED ULONG ID; STACKED LONG source; STACKED LONG dest; STACKED LONG size;};
struct MUIP_Cardgame_ClickCard      {STACKED ULONG ID; STACKED LONG pile;   STACKED LONG nr;   STACKED LONG dblclck;};
struct MUIP_Cardgame_TimerTick      {STACKED ULONG ID; STACKED LONG sec;};

struct MUIP_Cardgame_SetGraphic     {STACKED ULONG ID; STACKED char* cardset; STACKED char* pattern;};
struct MUIP_Cardgame_SetCards       {STACKED ULONG ID; STACKED LONG pile;   STACKED int* cards; STACKED LONG size;};
struct MUIP_Cardgame_GetCards       {STACKED ULONG ID; STACKED LONG pile;   STACKED int* cards; STACKED LONG* size;};
struct MUIP_Cardgame_MoveCards      {STACKED ULONG ID; STACKED LONG source; STACKED LONG dest;  STACKED LONG size;};
struct MUIP_Cardgame_SetEmptyImage  {STACKED ULONG ID; STACKED LONG pile;   STACKED LONG img;};

/*** Attributes ***/

#define MUIA_Cardgame_Piles         0x818f0041    // [IS.]
#define MUIA_Cardgame_TimerRunning  0x818f0042    // [ISG]
#define MUIA_Cardgame_MoveSpeed     0x818f0043    // [ISG]
#define MUIA_Cardgame_RasterX       0x818f0044    // [IS.]
#define MUIA_Cardgame_RasterY       0x818f0045    // [IS.]
#define MUIA_Cardgame_NoREKOBack    0x818f0046    // [IS.]

BOOL Cardgame_Init(void);
void Cardgame_Exit(void);

#endif
