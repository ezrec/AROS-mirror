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

#include "MUISpeedSelect.h"
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include "Globals.h"
#include "../IEngine.h"
#include <libraries/mui.h>
#include <LibC/LibC.h>

MUISpeedSelect::MUISpeedSelect(Globals &glb) :
   Glb(glb)
{
   hHkBtnHook.Initialize(this, &MUISpeedSelect::btnHook);
   
   menu = new MUIWindowMenu();
   menu->addMenu("Options");

   menu->setHook(hHkBtnHook.GetHook());
   setButtonCallBack(hHkBtnHook.GetHook());
}

bool MUISpeedSelect::vecDeleteName(const char* const& name)
{
   free(const_cast<char*>(name));
   return true;
}

unsigned long MUISpeedSelect::btnHook(long id, void* data)
{
   switch (id)
   {
      case ID_SwitchSpeed:
         {
            long which = (long)data;
            if (speeds.Count() == 0)
               return 0;
            hook(this->id, speeds[which]);
         }
         break;
   }
   return 0;
}
   
const char* MUISpeedSelect::dupstr(const char* str)
{
   char* dst = (char*)malloc(strlen(str)+1);
   strcpy(dst, str);
   return dst;
}

void MUISpeedSelect::rebuildCycle(DiscSpeed *speeds, uint16 currspd)
{
   DoMtd((Object *)cyclegroup, ARRAY(MUIM_Group_InitChange));
   DoMtd((Object *)cyclegroup, ARRAY(OM_REMMEMBER, (IPTR)cycle));

   this->names.ForEach(&vecDeleteName).Empty();
   this->speeds.Empty();

   int32 which = 0; 
   String s;

   if (speeds != 0)
   {
      for (int i=0; speeds[i].kbps != 0; i++)
      {
         s = Glb.Loc.FormatNumber(speeds[i].i, speeds[i].f * 100000) + "x (";
         s += Glb.Loc.FormatNumber(speeds[i].kbps) + " kBps)";
         names << dupstr(s.Data());
         this->speeds << speeds[i].kbps;
         if (currspd >= speeds[i].kbps)
         {
            which = i;
            currspd = 0;
         }
      }
   }
   
   if (names.Count() == 0)
   {
      names << dupstr("---");
   }

   names << 0;
   
   cycle = muiCycle(const_cast<const char**>(names.GetArray()), 0, ID_SwitchSpeed, which);

   Intuition->SetAttrsA(cycle, (TagItem*)ARRAY(
      MUIA_ContextMenu,    (long)menu->getObject(),
      TAG_DONE,            0
   ));

   DoMtd((Object *)cyclegroup, ARRAY(OM_ADDMEMBER, (IPTR)cycle));
   DoMtd((Object *)cyclegroup, ARRAY(MUIM_Group_ExitChange));
}

MUISpeedSelect::~MUISpeedSelect()
{
   names.ForEach(&vecDeleteName);
   names.Empty();
   delete menu;
}

bool MUISpeedSelect::start()
{
   return true;
}

void MUISpeedSelect::stop()
{
}

unsigned long *MUISpeedSelect::getObject()
{
   cycle = 0;
   pager = (long unsigned int *)PageGroup,
      Child,               cyclegroup = (long unsigned int *)VGroup,
      End,
   End;

   rebuildCycle(0, 0);

   return pager;
}

void MUISpeedSelect::setSpeeds(DiscSpeed *ds, uint16 sp)
{
   rebuildCycle(ds, sp);
}

void MUISpeedSelect::update()
{
}

void MUISpeedSelect::setCallbackHook(const Hook *hk)
{
   hook = hk;
}

void MUISpeedSelect::setID(int32 i)
{
   id = i;
}

