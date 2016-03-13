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

#ifndef _GUIMUI_MUISETTINGS_H_
#define _GUIMUI_MUISETTINGS_H_

#include <Generic/GenericMUI.h>
#include "MUICommon.h"
#include "Components/MUIPopText.h"
#include "MUIPopDOSDevice.h"
#include "Globals.h"
#include <Generic/ConfigParser.h>

using namespace GenNS;

class MUISettings : public MUICommon
{
protected:
   enum BtnID
   {
      ID_DataExport        =  1,
      ID_AudioExport,
      ID_SessionExport,

      ID_DOSInhibit        =  100,
      ID_DOSDevice,
      ID_FindDOSDevice
   };

protected:
   IPTR                       all;
   ConfigParser              *Config;
   

protected:
   Globals                   &Glb;
   MUIPopText                *popDataExport;
   MUIPopText                *popAudioExport;
   MUIPopText                *popSessionExport;
   MUIPopDOSDevice           *popDOSDevice;

protected:
   HookT<MUISettings, BtnID, IPTR>    hkButton;

protected:
   virtual IPTR               button(BtnID id, IPTR data);

public:

                              MUISettings(ConfigParser *parent, Globals &glb);
   virtual                   ~MUISettings();
   virtual bool               start();
   virtual void               stop();
   virtual IPTR               getObject();
   virtual void               update();
};

#endif

