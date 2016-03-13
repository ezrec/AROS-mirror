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

#ifndef _GUIMUI_MUIDRIVESELECT_H_
#define _GUIMUI_MUIDRIVESELECT_H_

#include "MUICommon.h"
#include <Generic/ConfigParser.h>
#include "Components/MUIWindowMenu.h"
#include <Generic/HookT.h>
#include "Globals.h"

class MUIDriveSelect : public MUICommon
{
protected:
   Globals                   &Glb;
   const char                *names[5]; 
   ConfigParser              *Config;
   IPTR                       cycle;
   IPTR                       string;
   MUIWindowMenu             *menu;
   IPTR                       pager;
   IPTR                       cyclegroup;
   IPTR                       currentdrive;

protected:
   HookT<MUIDriveSelect, IPTR, IPTR>    hHkBtnHook;

protected:
   enum
   {
      ID_SwitchDrive    =  1,
      ID_DriveName,     
      ID_EditDriveName,
   };

protected:
   virtual IPTR               btnHook(IPTR id, IPTR data);
   virtual void               rebuildCycle();

public:
                              MUIDriveSelect(ConfigParser* parent, Globals &glb);
   virtual                   ~MUIDriveSelect();
   virtual bool               start();
   virtual void               stop();
   virtual IPTR               getObject();
   virtual void               update();
};

#endif

