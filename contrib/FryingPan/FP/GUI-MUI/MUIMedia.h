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

#ifndef _GUIMUI_MUIMEDIA_H_
#define _GUIMUI_MUIMEDIA_H_

#include "MUICommon.h"
#include <Generic/String.h>
#include "Globals.h"
#include <Generic/ConfigParser.h>

class DiscSpeed;

using namespace GenNS;

class MUIMedia : public MUICommon
{
protected:
   enum BtnID
   {
      ID_Refresh        =  1,
      ID_SetPrepType,
      ID_EraseDisc,
      ID_FormatDisc,
      ID_PrepareDisc,
      ID_CloseSession,
      ID_CloseDisc,
      ID_CloseTracks,
      ID_Repair,
      ID_Load,
      ID_Eject,

      ID_DiscType       =  100,
      ID_SubType,
      ID_Vendor,
      ID_State,
      ID_Contents,
      ID_DiscID,
      ID_BarCode,
      ID_Catalog,
      ID_DiscSize,
      ID_ReadSpeeds,
      ID_WriteSpeeds
   };

protected:
   Globals                   &Glb;
   IPTR                       all;
   ConfigParser              *Config;

   HookT<MUIMedia, BtnID, IPTR> hBtnHook;

protected:
   uint16                     preptype;

protected:
   IPTR                       buttonClicked(BtnID id, IPTR state);
   String                     speedsToString(DiscSpeed*);
   String                     profileToString();
   String                     subTypeToString();
   String                     stateToString();
   String                     contentsToString();
   String                     sizeToString();

public:

                              MUIMedia(ConfigParser *parent, Globals &glb);
                             ~MUIMedia();
   bool                       start();
   void                       stop();
   IPTR                       getObject();
   void                       update();
};

#endif

