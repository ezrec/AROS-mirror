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

#ifndef INCLUDE_ABOUT_H
#define INCLUDE_ABOUT_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_About;

#define MUIM_About_AboutMUI (TAGBASE_KAI | 0x1402)

BOOL About_Init(void);
void About_Exit(void);

extern char* VERSION_STRING;
extern char* VERSION_NUMBER;
extern char* VERSION_DATE;
extern char* APPNAME;
extern char* APPCOPYRIGHT;

#endif
