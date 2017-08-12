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

#ifndef INCLUDE_SETTINGS_H
#define INCLUDE_SETTINGS_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_Settings;

enum SettingsID { ID_OPEN = 100, ID_BLOCK, ID_AUTOOPEN, ID_INDICATOR, ID_REQS,
  ID_ANIM, ID_BUTTONS, ID_STACK, ID_EQUALCOLOR, ID_REKOBACK,

  ID_MENU_KLONDIKE = 300, ID_MENU_FREECELL,
};

enum GameMode {GAMEMODE_KLONDIKE=0, GAMEMODE_FREECELL};

#define MUIA_Settings_Settings        (TAGBASE_KAI | 0x1301) /* [..G] */
#define MUIM_Settings_Close           (TAGBASE_KAI | 0x1302)
#define MUIM_Settings_GameMode        (TAGBASE_KAI | 0x1303) /* [IS.] */

struct MUIP_Settings_Close            {STACKED ULONG MethodID; STACKED LONG typ;};
struct MUIP_Settings_GameMode         {STACKED ULONG MethodID; STACKED enum GameMode mode;};

struct Settings
{
  enum GameMode gamemode; /* Freecell or Klondike */
  BOOL  buttons;   /* show buttons */
  BOOL  beep;      /* display beep */
  BOOL  reqs;      /* security requesters */
  BOOL  rekoback;  /* use reko back cards */
  ULONG anim;      /* animation speed */

  /* klondike options */
  ULONG open;      /* number of cards to turn */
  BOOL  block;     /* game option */
  BOOL  autoopen;  /* game option */
  BOOL  indicator; /* move indicator */
  BOOL  autoturn;  /* automatic turn */
  
  /* freecell options */
  ULONG stack;     /* number of stack start cards */
  BOOL  equalcolor;/* equal color mode */
};

BOOL Settings_Init(void);
void Settings_Exit(void);

BOOL NoneSlider_Init(void);
void NoneSlider_Exit(void);

Object* MakeNoneSlider(int min, int max, int val, int text, int help);

#endif

