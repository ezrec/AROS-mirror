/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "MUIPageSelect.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include "Components/MUIToolBar.h"

/*
 * localization area
 */
enum Loc
{
   loc_Tracks              =  le_PageSelect,
   loc_Media,
   loc_Contents,           // will include player?
   loc_Drive,
   loc_Settings,
   loc_Write,

};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Tracks,    "Tracks",   "BTN_TRACKS"          },
   {  loc_Media,     "Media",    "BTN_MEDIA"           },
   {  loc_Contents,  "Contents", "BTN_CONTENTS"        },
   {  loc_Drive,     "Drive",    "BTN_DRIVE"           },
   {  loc_Settings,  "Settings", "BTN_SETTINGS"        },
   {  loc_Write,     "WRITE",    "BTN_WRITE"           },   
   {  Localization::LocaleSet::Set_Last, 0, 0      }
};

static const char* LocaleGroup = "ELEM_PAGESELECT";


MUIPageSelect::MUIPageSelect(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all      = 0;
   Config   = new ConfigParser(parent, "PageButtons", 0);
   ToolBar  = new MUIToolBar(Config, Glb.Loc);
   
   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);

   hPageSel.Initialize(this, &MUIPageSelect::pageSel);
   ToolBar->setThemePath("skins");
   ToolBar->setHook(hPageSel.GetHook());
}

MUIPageSelect::~MUIPageSelect()
{
   delete ToolBar;
   delete Config;
}

IPTR MUIPageSelect::getObject()
{
   if (0 != all)
      return all;

   MUIToolBar::Button buttons[] =
   {
      {
         MUIToolBar::Type_Button,
         1,
         0,
         Glb.Loc[loc_Tracks],
         "Tracks1.img",
         "Tracks2.img"
      },
      {
         MUIToolBar::Type_Button,
         2,
         0,
         Glb.Loc[loc_Media],
         "Media1.img",
         "Media2.img"
      },
      {
         MUIToolBar::Type_Button,
         3,
         0,
         Glb.Loc[loc_Contents], 
         "Contents1.img",
         "Contents2.img"
      },
      {
         MUIToolBar::Type_Button,
         4,
         0,
         Glb.Loc[loc_Drive], 
         "Drive1.img",
         "Drive2.img"
      },
      {
         MUIToolBar::Type_Button,
         5,
         0,
         Glb.Loc[loc_Settings], 
         "Settings1.img",
         "Settings2.img"
      },
      {
         MUIToolBar::Type_Button,
         6,
         0,
         Glb.Loc[loc_Write], 
         "Write1.img",
         "Write2.img"
      },
      {
         MUIToolBar::Type_End,
         0,
         0,
         0,
         0,
         0
      }
   };

   ToolBar->addButtons(&buttons[0]);


   all = (IPTR)HGroup,
      Child,            RectangleObject,
         MUIA_Weight,      1,
      End,
         
      Child,            (IPTR)ToolBar->getObject(),

      Child,            RectangleObject,
         MUIA_Weight,      1,
      End,
   End;

   return all;
}

bool MUIPageSelect::start()
{
   return true;
}

void MUIPageSelect::stop()
{
   return;
}

IPTR MUIPageSelect::pageSel(MUIToolBar*, uint page)
{
   return callBack(this, page);
}

void MUIPageSelect::setHook(const Hook *hook)
{
   callBack = hook;
}

void MUIPageSelect::update()
{
}

