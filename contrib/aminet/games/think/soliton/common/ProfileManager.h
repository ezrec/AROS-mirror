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

#ifndef INCLUDE_PROFILEMANAGER_H
#define INCLUDE_PROFILEMANAGER_H

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#include "MUITools.h"

extern struct MUI_CustomClass *CL_ProfileManager;

#define MUIA_ProfileManager_Cardgame        (TAGBASE_KAI | 0x1302)  // [.S.]
#define MUIA_ProfileManager_ProfileMenu     (TAGBASE_KAI | 0x1303)  // [.S.]

#define MUIM_ProfileManager_LoadProfiles    (TAGBASE_KAI | 0x1311)

#define MUIM_ProfileManager_Open            (TAGBASE_KAI | 0x1313)  // privat
#define MUIM_ProfileManager_Close           (TAGBASE_KAI | 0x1314)  // privat
#define MUIM_ProfileManager_Redefine        (TAGBASE_KAI | 0x1315)  // privat
#define MUIM_ProfileManager_Add             (TAGBASE_KAI | 0x1316)  // privat
#define MUIM_ProfileManager_Rem             (TAGBASE_KAI | 0x1317)  // privat
#define MUIM_ProfileManager_Select          (TAGBASE_KAI | 0x1318)  // privat

BOOL getlin(BPTR f, char * buf, ULONG size);

BOOL ProfileManager_Init(void);
void ProfileManager_Exit(void);


#endif
