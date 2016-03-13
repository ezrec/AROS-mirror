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

#ifndef _GUIMUI_MUIRECORD_H_
#define _GUIMUI_MUIRECORD_H_

#include <Generic/GenericMUI.h>
#include "MUICommon.h"
#include "MUISpeedSelect.h"
#include "Globals.h"

using namespace GenNS;

class MUIRecord : 
   public MUICommon, 
   public PropertyT<Globals::eWriteSelect>::Monitor
{
protected:
   enum BtnID
   {
      ID_Masterize,
      ID_CloseDisc,
      ID_Write,
      ID_SetWriteSpeed,
      ID_BlankWrite,
      ID_PostEject
   };
protected:
   IPTR                       all;
   MUISpeedSelect            *speedselect;
   ConfigParser              *Config;

   HookT<MUIRecord, BtnID, IPTR>    hBtnHook;

protected:
   Globals                   &Glb;
   bool                       do_masterize;
   bool                       do_closedisc;
   bool                       do_blankwrite;
   bool                       do_posteject;

protected:
   IPTR                       btnClicked(BtnID button, IPTR state);

public:
   /*
    * monitor
    */
   virtual void               onPropChange(PropertyT<Globals::eWriteSelect>*, const Globals::eWriteSelect &);

public:

                              MUIRecord(ConfigParser *parent, Globals &glb);
                             ~MUIRecord();
   bool                       start();
   void                       stop();
   virtual IPTR               getObject();
   void                       update();
};

#endif

