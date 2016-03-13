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

#ifndef _GUIMUI_MUITRACKSDATAAUDIO_H_
#define _GUIMUI_MUITRACKSDATAAUDIO_H_

#include "MUICommon.h"
#include <Generic/ConfigParser.h>
#include "../ITrack.h"
#include "Components/MUIList.h"
#include "Components/FileReq.h"
#include "Globals.h"

class MUITracksDataAudio : public MUICommon
{
protected:
   enum BtnID
   {
      ID_Add,
      ID_Remove,
      ID_Play
   };

protected:
   struct ITrackEntry
   {
      ITrack*     track;
      String      information;
   };

protected:
   Globals                   &Glb;
   IPTR                       all;

   MUIList                   *tracks;
   FileReq                   *addReq;
   ConfigParser              *Config;
   

protected:
   DbgHandler                *getDebug();

protected:
   HookT<MUITracksDataAudio, BtnID, IPTR>                   hHkButton;
   HookT<MUITracksDataAudio, IPTR, ITrack*>                 hConstruct;
   HookT<MUITracksDataAudio, IPTR, ITrackEntry*>            hDestruct;
   HookT<MUITracksDataAudio, const char**, ITrackEntry*>    hDisplay;
   HookT<MUITracksDataAudio, AppMessage*, IPTR>             hWBMessage;
   HookT<MUITracksDataAudio, VectorT<ITrackEntry*>*, IPTR>  hDragSort;

protected:
   IPTR                       onButton(BtnID, IPTR);
   IPTR                       onConstruct(IPTR, ITrack*);
   IPTR                       onDestruct(IPTR, ITrackEntry*);
   IPTR                       onDisplay(const char**, ITrackEntry*);
   IPTR                       onWBMessage(AppMessage* m, IPTR);
   IPTR                       onDragSort(VectorT<ITrackEntry*>*, IPTR);

protected:
   void                       addTracks();
   void                       remTracks();
public:
                              MUITracksDataAudio(ConfigParser *parent, Globals &glb);
                             ~MUITracksDataAudio();
   virtual IPTR               getObject();
   bool                       start();
   void                       stop();
   const char                *getName();
   void                       update();
};

#endif

