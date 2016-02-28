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

#include "MUIDriveSelect.h"
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include "Globals.h"
#include "../IEngine.h"
#include <libraries/mui.h>

/*
 * localization area
 */
enum Loc
{
   loc_Drive                  = le_DeviceSelect,
   loc_Options,
   loc_EditName,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Drive,              "Drive %s",       "BTN_DRIVE"       },
   {  loc_Options,            "Options",        "MNU_OPTIONS"     },
   {  loc_EditName,           "Edit Name",      "MNU_EDITNAME"    },

   {  Localization::LocaleSet::Set_Last, 0, 0                     }
};

static const char* LocaleGroup = "ELEM_DEVSELECT";
/*
 * that's it :)
 */

MUIDriveSelect::MUIDriveSelect(ConfigParser* parent, Globals &glb) :
   Glb(glb)
{
   Config   = new ConfigParser(parent, "DriveSelector", 0);
   currentdrive = 0;
   menu = 0;

   hHkBtnHook.Initialize(this, &MUIDriveSelect::btnHook);
   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);
   currentdrive = Config->getValue("CurrentDrive", currentdrive);
}

uint32 MUIDriveSelect::btnHook(int32 id, void* data)
{
   switch (id)
   {
      case ID_DriveName:
         {
            
            IEngine *eng = Glb.CurrentEngine->ObtainRead();
            eng->setName((char*)data);
            Glb.CurrentEngine->Release();

            // causes SwitchDrive()!
            rebuildCycle();

            Intuition->SetAttrsA(pager, (TagItem*)ARRAY(
               MUIA_Group_ActivePage,     0,
               TAG_DONE,                  0
            ));
         }
         break;

      case ID_SwitchDrive:
         {
            long which = (long)data;
            Glb.CurrentEngine->Assign(Glb.Engines[which]);
            currentdrive = which;
         }
         break;
 
      case ID_EditDriveName:
         {
            IEngine *eng = Glb.CurrentEngine->ObtainRead();
            muiSetText(ID_DriveName, eng->getName());
            Intuition->SetAttrsA(pager, (TagItem*)ARRAY(
               MUIA_Group_ActivePage,     1,
               TAG_DONE,                  0
            ));
            Glb.CurrentEngine->Release();
         }
         break;
   }
   return 0;
}

void MUIDriveSelect::rebuildCycle()
{
   DoMtd((Object *)cyclegroup, ARRAY(MUIM_Group_InitChange));
   DoMtd((Object *)cyclegroup, ARRAY(OM_REMMEMBER, (IPTR)cycle));
   for (int i=0; i<4; i++)
   {
      names[i] = Glb.Engines[i]->getName();
   }
   names[4] = 0;
   cycle = muiCycle(names, ' ', ID_SwitchDrive, currentdrive);

   Intuition->SetAttrsA(cycle, (TagItem*)ARRAY(
      MUIA_ContextMenu,    (long)menu->getObject(),
      TAG_DONE,            0
   ));

   DoMtd((Object *)cyclegroup, ARRAY(OM_ADDMEMBER, (IPTR)cycle));
   DoMtd((Object *)cyclegroup, ARRAY(MUIM_Group_ExitChange));
}

MUIDriveSelect::~MUIDriveSelect()
{
   Config->setValue("CurrentDrive", currentdrive);
   delete Config;
   delete menu;
   Config = 0;
}

bool MUIDriveSelect::start()
{
   return true;
}

void MUIDriveSelect::stop()
{
   if (menu)
      delete menu;
   menu = 0;
}

uint32 *MUIDriveSelect::getObject()
{
   if (menu == 0)
   {
      menu = new MUIWindowMenu();
      menu->addMenu(Glb.Loc[loc_Options]);
      menu->addItem(Glb.Loc[loc_EditName], ID_EditDriveName);

      menu->setHook(hHkBtnHook.GetHook());
      setButtonCallBack(hHkBtnHook.GetHook());
   }

   cycle = 0;
   pager = (uint32 *)PageGroup,
      Child,               cyclegroup = (uint32 *)VGroup,
      End,
      Child,               VGroup,
         Child,               string = muiString("", 0, ID_DriveName),
      End,
   End;

   rebuildCycle();

   return pager;
}

void MUIDriveSelect::update()
{
}


