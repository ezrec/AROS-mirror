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

#include "MUITracksDataAudio.h"
#include <libclass/dos.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include "../IEngine.h"
#include "Globals.h"
#include <workbench/startup.h>

/*
 * localization area
 */
enum Loc
{
   loc_Name    =  lg_TracksData,
   loc_Add,
   loc_Remove,
   loc_Play,

   loc_ColTrack   =  lg_TracksData + ls_Group1,
   loc_ColModule,
   loc_ColInfo,

   loc_SelectTracks = lg_TracksData + ls_Req,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Name,      "Data / Audio Tracks",     "LBL_DATAAUDIOTRACKS"   },
   {  loc_Add,       "a&Add",                   "BTN_ADD"               },
   {  loc_Remove,    "r&Remove",                "BTN_REMOVE"            },
   {  loc_Play,      "p&Play",                  "BTN_PLAY"              },

   {  loc_ColTrack,  "Track Name",              "COL_TRACKNAME"         },
   {  loc_ColModule, "Module Name",             "COL_MODULENAME"        },
   {  loc_ColInfo,   "Track Information",       "COL_TRACKINFO"         },

   {  loc_SelectTracks,
      "Select tracks to be added to compilation",
      "REQ_SELECTTRACKS" },
   
   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "TRACKS_DA";

   static const char*   Cfg_LastImagePath    =  "LastImagePath";

MUITracksDataAudio::MUITracksDataAudio(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all    = 0;
   addReq = 0;
   tracks = 0;
   Config = new ConfigParser(parent, "DataAudio", 0);

   hHkButton.Initialize(this, &MUITracksDataAudio::onButton);
   hConstruct.Initialize(this, &MUITracksDataAudio::onConstruct);
   hDestruct.Initialize(this, &MUITracksDataAudio::onDestruct);
   hDisplay.Initialize(this, &MUITracksDataAudio::onDisplay);
   hWBMessage.Initialize(this, &MUITracksDataAudio::onWBMessage);
   hDragSort.Initialize(this, &MUITracksDataAudio::onDragSort);

   setButtonCallBack(hHkButton.GetHook());
   Glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}
    
MUITracksDataAudio::~MUITracksDataAudio()
{
   delete Config;
}
   
DbgHandler *MUITracksDataAudio::getDebug()
{
   return Glb.dbg;
}

IPTR MUITracksDataAudio::getObject()
{
   if (0 != all)
      return all;

   tracks = new MUIList("BAR,BAR,", true);
// FIXME: nlist still does not support MUIA_DragDropSort notification.
   //tracks->setDragSortable(true);
   tracks->setConstructHook(hConstruct.GetHook());
   tracks->setDestructHook(hDestruct.GetHook());
   tracks->setDisplayHook(hDisplay.GetHook());
   tracks->setWBDropHook(hWBMessage.GetHook());
   tracks->setDragSortHook(hDragSort.GetHook());

   addReq = new FileReq(Glb.Loc[loc_SelectTracks]);
   addReq->setMultiSelect(true);

   all = (IPTR)VGroup,
      Child,                  tracks->getObject(),

      Child,                  ColGroup(3),
         Child,                  muiButton(Glb.Loc[loc_Add], Glb.Loc.Accel(loc_Add), ID_Add),
         Child,                  muiButton(Glb.Loc[loc_Remove], Glb.Loc.Accel(loc_Remove), ID_Remove),
         Child,                  muiButton(Glb.Loc[loc_Play], Glb.Loc.Accel(loc_Play), ID_Play),
      End,

   End;

   return all;
}

bool MUITracksDataAudio::start()
{
   addReq->setPath(Config->getValue(Cfg_LastImagePath, ""));
   return true;
}

void MUITracksDataAudio::stop()
{
   Config->setValue(Cfg_LastImagePath, addReq->getPath());
   if (0 != addReq)
      delete addReq;
   if (0 != tracks)
      delete tracks;
   addReq = 0;
   tracks = 0;
   all = 0;
}

const char *MUITracksDataAudio::getName()
{
   return Glb.Loc[loc_Name];
}

void MUITracksDataAudio::update()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   _d(Lvl_Info, "Updating Data/Audio tracks page");
   VectorT<ITrack*> &t = eng->tracks().ObtainRead();

   _d(Lvl_Info, "Clearing old items");
   tracks->clear();

   _d(Lvl_Info, "Adding current set of tracks.");
   for (int i=0; i<t.Count(); i++)
   {
      tracks->addItem((IPTR)t[i]);
   }

   eng->tracks().Release();

   _d(Lvl_Info, "Data/Audio tracks page updated.");

   Glb.CurrentEngine->Release();
}

IPTR MUITracksDataAudio::onConstruct(IPTR, ITrack* t)
{
   ITrackEntry *e = new ITrackEntry;
   e->track = t;
   e->information = "";

   return (IPTR)e;
}

IPTR MUITracksDataAudio::onDestruct(IPTR, ITrackEntry* e)
{
   delete e;
   return 0;
}

IPTR MUITracksDataAudio::onDisplay(const char** a, ITrackEntry* e)
{
   if (e == NULL)
   {
      a[0] = Glb.Loc[loc_ColTrack];
      a[1] = Glb.Loc[loc_ColModule];
      a[2] = Glb.Loc[loc_ColInfo];
   }
   else
   {
      uint64 s = 1ull*e->track->getBlockSize() * e->track->getBlockCount();
      const char* t = "B";
      uint32 c = (uint32)s;
      uint32 f = 0;

      if (s > 1024)
      {
         f = (long)(s & 1023) * 100 / 1024;
         s >>= 10;
         c = s;
         t = "kB";
      }
      if (s > 1024)
      {
         f = (long)(s & 1023) * 100 / 1024;
         s >>= 10;
         c = s;
         t = "MB";
      }
      if (s > 1024)
      {
         f = (long)(s & 1023) * 100 / 1024;
         s >>= 10;
         c = s;
         t = "GB";
      }

      e->information = Glb.Loc.FormatNumber(e->track->getBlockCount(), 0) + " sectors (";
      e->information += Glb.Loc.FormatNumber(c, f*10000) + t + ")";

      a[0] = e->track->getTrackName();
      a[1] = e->track->getModuleName();
      a[2] = e->information.Data();
   }
   return (IPTR)a;
}

void MUITracksDataAudio::addTracks()
{
   VectorT<const char*> &res = addReq->openReq();
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   for (int i=0; i<res.Count(); i++)
   {
      eng->addTrack(res[i]);
   }

   eng->layoutTracks(false, false);
   Glb.CurrentEngine->Release();
}

void MUITracksDataAudio::remTracks()
{
   VectorT<IPTR> &res = tracks->getSelectedItems();
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   for (int i=0; i<res.Count(); i++)
   {
      ITrackEntry *e = (ITrackEntry*)res[i];
      eng->remTrack(e->track);
   }

   eng->layoutTracks(false, false);
   Glb.CurrentEngine->Release();
}

IPTR MUITracksDataAudio::onButton(BtnID id, IPTR)
{
   switch (id)
   {
      case ID_Add:
         {
            addTracks();
         };
         break;

      case ID_Remove:
         {
            remTracks();
         }
         break;

      case ID_Play:
         break;
   }

   return 0;
}

IPTR MUITracksDataAudio::onWBMessage(AppMessage* m, IPTR)
{
   char *c = new char[1024];
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();

   for (int32 i=0; i<m->am_NumArgs; i++)
   {
      DOS->NameFromLock(m->am_ArgList[i].wa_Lock, c, 1024);
      DOS->AddPart(c, (char*)m->am_ArgList[i].wa_Name, 1024);
      pEng->addTrack(c);
   }

   delete []c;
   pEng->layoutTracks(false, false);
   Glb.CurrentEngine->Release();
   return 0;
}
   
IPTR MUITracksDataAudio::onDragSort(VectorT<ITrackEntry*>*vec, IPTR)
{
   request("Info", "Newly sorted vector has %ld items.", "Ok", ARRAY((IPTR)vec->Count()));
   return 0;
}
