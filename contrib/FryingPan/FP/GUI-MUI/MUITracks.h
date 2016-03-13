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

#ifndef _FP_GUIMUI_MUITRACKS_H_
#define _FP_GUIMUI_MUITRACKS_H_

#include "MUICommon.h"
#include "Globals.h"
#include <Generic/HookT.h>
#include <Generic/ConfigParser.h>

using namespace GenNS;

class MUITracksISOBuilder;
class MUITracksDataAudio;
class MUITracksSession;
class Globals;

class MUITracks : 
   public MUICommon,
   public PropertyT<Globals::eWriteSelect>::Monitor
{
protected:
   enum BtnId
   {
      Btn_InfoGauge  =  100,
      Btn_CompositionType
   };

protected:
   IPTR                       all;
   Object                    *page;
   ConfigParser              *Config;
   MUITracksISOBuilder       *ISOBuilder;
   MUITracksDataAudio        *DataAudio;
   MUITracksSession          *Session;
   Object                    *infogauge;
   Globals                   &Glb;
   
protected:
   HookT<MUITracks, BtnId, IPTR>             hHkButton;
   
protected:
   IPTR                       onButton(BtnId id, IPTR);

public:
   /*
    * monitor
    */
   virtual void               onPropChange(PropertyT<Globals::eWriteSelect>*, const Globals::eWriteSelect &);

public:

                              MUITracks(ConfigParser *parent, Globals &glb);
   virtual                   ~MUITracks();
   virtual bool               start();
   virtual void               stop();
   virtual IPTR               getObject();
   virtual void               update();
   virtual void               layoutUpdate();

   virtual void               enableISO();
   virtual void               disableISO();
};

#endif

