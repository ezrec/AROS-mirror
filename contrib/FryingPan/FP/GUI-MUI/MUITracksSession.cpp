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

#include "MUITracksSession.h"
#include <Generic/ConfigParser.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include "Globals.h"
#include "../IEngine.h"

/*
 * localization area
 */
enum Loc
{
   loc_Name    =  lg_TracksImage,
   loc_SessionFile,

   loc_ColNumber = lg_TracksImage + ls_Group1,
   loc_ColName,
   loc_ColInfo,

   loc_TypeDisc,
   loc_TypeSession,
   loc_TypeDataTrack,
   loc_TypeAudioTrack,
   loc_TypeIndex,

};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Name,            "Session Import", "LBL_SESSIONIMPORT"  },
   {  loc_SessionFile,     "Session File:",  "LBL_SESSIONFILE"    },
   {  loc_ColNumber,       "Number",         "COL_NUMBER"         },
   {  loc_ColName,         "Name",           "COL_TYPE"           },
   {  loc_ColInfo,         "Information",    "COL_INFO"           },

   {  loc_TypeDisc,        "Disc",           "TYPE_DISC"          },
   {  loc_TypeSession,     "Session",        "TYPE_SESSION"       },
   {  loc_TypeDataTrack,   "Data Track",     "TYPE_DATATRACK"     },
   {  loc_TypeAudioTrack,  "Audio Track",    "TYPE_AUDIOTRACK"    },
   {  loc_TypeIndex,       "Index",          "TYPE_INDEX"         },

   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "TRACKS_IMG";


MUITracksSession::MUITracksSession(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all    = 0;
   tracks = 0;
   target = 0;
   hHkDisplay.Initialize(this, &MUITracksSession::display);
   hHkDestruct.Initialize(this, &MUITracksSession::destruct);
   hHkConstruct.Initialize(this, &MUITracksSession::construct);
   hHkButton.Initialize(this, &MUITracksSession::button);

   Config = new ConfigParser(parent, "PageContents", 0);
   setButtonCallBack(hHkButton.GetHook());
   Glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}

MUITracksSession::~MUITracksSession()
{
   delete Config;
}
   
DbgHandler *MUITracksSession::getDebug()
{
   return Glb.dbg;
}

IPTR MUITracksSession::getObject()
{
   if (0 != all)
      return all;

   tracks = new MUITree("BAR,BAR,", true);
   tracks->setDisplayHook(hHkDisplay.GetHook());
   tracks->setDestructHook(hHkDestruct.GetHook());
   tracks->setConstructHook(hHkConstruct.GetHook());

   target = new MUIPopAsl(Glb.Loc[loc_SessionFile], MUIPopAsl::Pop_File);
   target->setID(Btn_SetSession);
   target->setCallbackHook(hHkButton.GetHook());


   all = (IPTR)VGroup,
      GroupFrame,
      Child,                     tracks->getObject(),
      Child,                     target->getObject(),
   End;

   return all;
}

bool MUITracksSession::start()
{
//   target = Config->getValue(Cfg_TargetDir, "");
   update();
   return true;
}

void MUITracksSession::stop()
{
   if (0 != tracks)
      delete tracks;
   if (0 != target)
      delete target;
   tracks = 0;
   target = 0;
   all    = 0;
//   Config->setValue(Cfg_TargetDir, (char*)target->getValue());
}

IPTR MUITracksSession::construct(IPTR, const IOptItem* item)
{
   Entry *e = new Entry;
   e->item = item;
   e->item->claim();
   return (IPTR)e;
}

IPTR MUITracksSession::destruct(IPTR, Entry* e)
{
   e->item->dispose();
   delete e;
   return 0;
}

IPTR MUITracksSession::display(const char** arr, Entry* e)
{
   if (NULL == e)
   {
      arr[0] = Glb.Loc[loc_ColNumber];
      arr[1] = Glb.Loc[loc_ColName];
      arr[2] = Glb.Loc[loc_ColInfo];
   }
   else
   {
      const char          *suffix;
      unsigned long long   len = 0;
      int                  fraction = 0;

      switch (e->item->getItemType())
      {
         case Item_Disc:
            {
               e->number = "-";
               e->name   = Glb.Loc[loc_TypeDisc];
            }
            break;

         case Item_Session:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               e->name  = Glb.Loc[loc_TypeSession];
            }
            break;

         case Item_Track:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               if (e->item->getDataType() == Data_Audio)
               {
                  e->name  = Glb.Loc[loc_TypeAudioTrack];
               }
               else
               {
                  e->name  = Glb.Loc[loc_TypeDataTrack];
               }
            }
            break;

         case Item_Index:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               e->name  = Glb.Loc[loc_TypeIndex];
            }
            break;
      }

      if (e->item->hasCDText())
      {
         e->name  = e->item->getCDTArtist();
         e->name += " - ";
         e->name += e->item->getCDTTitle();
      }

      len      = (unsigned long long)e->item->getBlockCount() * e->item->getSectorSize();
      suffix   = "B";
      if (len > 1024)
      {
         fraction = (10*(len - ((len>>10)<<10)))>>10;
         len   >>= 10;
         suffix  = "kB";
      }
      if (len > 1024)
      {
         fraction = (10*(len - ((len>>10)<<10)))>>10;
         len   >>= 10;
         suffix  = "MB";
      }
      if (len > 1024)
      {
         fraction = (10*(len - ((len>>10)<<10)))>>10;
         len   >>= 10;
         suffix  = "GB";
      }
      
      if (len != 0)
         e->info = Glb.Loc.FormatNumber(len, fraction * 100000) + suffix + "   ";
      else
         e->info = "";
      e->info += "(" + Glb.Loc.FormatNumber(e->item->getStartAddress()) + " - " + Glb.Loc.FormatNumber(e->item->getEndAddress()) + ")";

      arr[0] = e->number.Data();
      arr[1] = e->name.Data();
      arr[2] = e->info.Data();
   }
   return true;
}

void MUITracksSession::update()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   _d(Lvl_Info, "Updating Sessions page");

   if (NULL != eng)
   {
      const IOptItem * item = eng->getSessionContents();
      _d(Lvl_Info, "Clearing old items");
      tracks->clear();
      _d(Lvl_Info, "Recursively adding new items (if any). Disc structure at %08lx", (IPTR)item);
      addRecurse(0, item);
   }
   else
   {
      _d(Lvl_Error, "%s: No engine! Do we have a leak?", (IPTR)__PRETTY_FUNCTION__);
   }

   _d(Lvl_Info, "Sessions page updated.");

   Glb.CurrentEngine->Release();
}

void MUITracksSession::addRecurse(IPTR parent, const IOptItem* data)
{
   IPTR item;

   if (data == 0)
      return;

   _d(Lvl_Info, "Adding item %08lx of type %ld to tree", (IPTR)data, data->getItemType());
   item = tracks->addEntry(parent, (IPTR)data, true);

   _d(Lvl_Info, "Item branch: %08lx", item);
   _d(Lvl_Info, "This item has %ld children, that will be added now.", (IPTR)data->getChildCount());

   for (int i=0; i<data->getChildCount(); i++)
   {
      addRecurse(item, data->getChild(i));
   }

   _d(Lvl_Info, "All children have been added");
}

IPTR MUITracksSession::button(BtnID id, IPTR data)
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   switch (id)
   {
      case Btn_SetSession:
         {
            eng->importSession((char*)data);
            update();
         }
         break;
   }

   Glb.CurrentEngine->Release();  
   return 0;
}

const char *MUITracksSession::getName()
{
   return Glb.Loc[loc_Name];
}
