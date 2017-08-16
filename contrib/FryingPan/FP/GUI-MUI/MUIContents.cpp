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

#include "MUIContents.h"
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
   loc_TargetDir           =  lg_Contents,
   loc_Update,
   loc_DownloadTracks,

   loc_TreeItemNumber      =  lg_Contents+ls_Group1,
   loc_TreeItemName,
   loc_TreeItemInfo,

   loc_TreeTypeDisc        =  lg_Contents+ls_Group2,
   loc_TreeTypeSession,
   loc_TreeTypeAudioTrack,
   loc_TreeTypeDataTrack,
   loc_TreeTypeEmptyTrack,
   loc_TreeTypeIndex,
   loc_TreeTypeIncomplete,

   loc_SelectTargetDirFirst   =  lg_Contents+ls_Req,
   loc_SelectTracksToDownload,
   loc_SelectedItemWrongType,
   loc_SelectItemExportModule,
   loc_AudioExport,
   loc_DataExport,
   loc_SessionExport,

};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_TargetDir,             "Target Directory:",       "BTN_TARGETDIR"   },
   {  loc_Update,                "u&Update",                "BTN_UPDATE"      },
   {  loc_DownloadTracks,        "t&Download Tracks",       "BTN_DLTRACKS"    },
   {  loc_TreeItemNumber,        "Number",                  "COL_TREENUMBER"  },
   {  loc_TreeItemName,          "Name",                    "COL_TREENAME"    },
   {  loc_TreeItemInfo,          "Information",             "COL_TREEINFO"    },
   {  loc_TreeTypeDisc,          "Disc",                    "TYPE_DISC"       },
   {  loc_TreeTypeSession,       "Session",                 "TYPE_SESSION"    },
   {  loc_TreeTypeAudioTrack,    "Audio Track",             "TYPE_AUDIOTRACK" },
   {  loc_TreeTypeDataTrack,     "Data Track",              "TYPE_DATATRACK"  },
   {  loc_TreeTypeEmptyTrack,    "Empty Track",             "TYPE_EMPTYTRACK" },
   {  loc_TreeTypeIndex,         "Index",                   "TYPE_INDEX"      },
   {  loc_TreeTypeIncomplete,    "Incomplete",              "TYPE_INCOMPLETE" },

   {  loc_SelectTargetDirFirst,
      "Please select target directory first",
      "REQ_SELECT_TARGET_FIRST"                                               },
   {  loc_SelectTracksToDownload,
      "Please select tracks to download",
      "REQ_SELECT_TRACKS_FIRST",                                              },
   {  loc_SelectedItemWrongType,
      "One of selected items is neither a SESSION or TRACK.\n"
         "This item will not be downloaded",
      "REQ_SELECTED_ITEM_WRONG_TYPE"                                          },
   {  loc_SelectItemExportModule,
      "No module has been configured for %s export.\n"
         "Please configure program first",
      "REQ_SELECT_ITEM_EXPORTS_FIRST"                                         },
   {  loc_AudioExport,           "Audio",                   "REQ_SELECT_AUDIO"      },
   {  loc_DataExport,            "Data",                    "REQ_SELECT_DATA"       },
   {  loc_SessionExport,         "Session",                 "REQ_SELECT_SESSION"    },
   
   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "CONTENTS";

/*
 * configuration elements
 */
static const char* Cfg_TargetDir = "TargetDir";

MUIContents::MUIContents(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   _dx(Lvl_Info, "Creating Contents page");
   all    = 0;

   _dx(Lvl_Info, "Setting up hooks");
   hHkDisplay.Initialize(this, &MUIContents::display);
   hHkDestruct.Initialize(this, &MUIContents::destruct);
   hHkConstruct.Initialize(this, &MUIContents::construct);
   hHkButton.Initialize(this, &MUIContents::button);
   setButtonCallBack(hHkButton.GetHook());

   _dx(Lvl_Info, "Accessing configuration element");
   Config = new ConfigParser(parent, "PageContents", 0);
   tracks = 0;
   target = 0;

   _dx(Lvl_Info, "Updating localization map");
   glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}

MUIContents::~MUIContents()
{
   _dx(Lvl_Info, "Disposing configuration element");
   delete Config;
}
   
DbgHandler *MUIContents::getDebug()
{
   return Glb.dbg;
}

IPTR MUIContents::getObject()
{
   _dx(Lvl_Info, "Retrieving page content (%lx)", (IPTR)all);
   if (0 != all)
      return all;

   _dx(Lvl_Info, "Page not created yet - building");

   if (tracks == 0)
   {
      _dx(Lvl_Info, "Setting up Tracks tree");
      tracks = new MUITree("BAR,BAR,", true);
      tracks->setDisplayHook(hHkDisplay.GetHook());
      tracks->setDestructHook(hHkDestruct.GetHook());
      tracks->setConstructHook(hHkConstruct.GetHook());
   }

   if (target == 0)
   {
      _dx(Lvl_Info, "Setting up PopASL");
      target = new MUIPopAsl(Glb.Loc[loc_TargetDir], MUIPopAsl::Pop_Dir);
      target->setValue((IPTR)Config->getValue(Cfg_TargetDir, ""));
   }

   all = (IPTR)VGroup,
      GroupFrame,
      Child,                     tracks->getObject(),
      Child,                     target->getObject(),
      Child,                     muiButton(Glb.Loc[loc_Update], Glb.Loc.Accel(loc_Update), ID_Refresh),
      Child,                     muiButton(Glb.Loc[loc_DownloadTracks], Glb.Loc.Accel(loc_DownloadTracks), ID_DownloadTracks),
   End;

   _dx(Lvl_Info, "Page content created (%08lx)", (IPTR)all);
   return all;
}

bool MUIContents::start()
{
   _dx(Lvl_Info, "All done");
   return true;
}

void MUIContents::stop()
{
   _dx(Lvl_Info, "Applying target dir configuration");
   Config->setValue(Cfg_TargetDir, (char*)target->getValue());

   _dx(Lvl_Info, "Disposing tree (%08lx) and popasl (%08lx) objects", (uint)target, (uint)tracks);
   delete target;
   target = 0;
   delete tracks;
   tracks = 0;
   all = 0;
   _dx(Lvl_Info, "All done");
}

IPTR MUIContents::construct(IPTR, const IOptItem* item)
{
   Entry *e = new Entry;
   _dx(Lvl_Info, "Creating new Tree element %08lx (-> %08lx)", (IPTR)e, (IPTR)item);
   e->item = item;
   _dx(Lvl_Info, "Locking element");
   e->item->claim();
   _dx(Lvl_Info, "Element set up.");
   return (IPTR)e;
}

IPTR MUIContents::destruct(IPTR, Entry* e)
{
   _dx(Lvl_Info, "Disposing Tree element %08lx (-> %08lx)", (IPTR)e, (IPTR)e->item);
   e->item->dispose();
   delete e;
   _dx(Lvl_Info, "Done.");
   return 0;
}

IPTR MUIContents::display(const char** arr, Entry* e)
{
   if (NULL == e)
   {
      _dx(Lvl_Info, "Filling up Tree column information");
      arr[0] = Glb.Loc[loc_TreeItemNumber].Data();
      arr[1] = Glb.Loc[loc_TreeItemName].Data();
      arr[2] = Glb.Loc[loc_TreeItemInfo].Data();
   }
   else
   {
      String               s1, s2, s3;
      const char          *suffix;
      unsigned long long   len = 0;
      int                  fraction = 0;

      _dx(Lvl_Info, "Constructing display for Tree element %08lx (-> %08lx)", (IPTR)e, (IPTR)e->item);

      switch (e->item->getItemType())
      {
         case Item_Disc:
            {
               e->number = "-";
               e->name   = Glb.Loc[loc_TreeTypeDisc];
            }
            break;

         case Item_Session:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               e->name  = Glb.Loc[loc_TreeTypeSession];
            }
            break;

         case Item_Track:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               if (e->item->getDataType() == Data_Audio)
               {
                  e->name  = Glb.Loc[loc_TreeTypeAudioTrack];
               }
               else
               {
                  if (e->item->isBlank())
                  {
                     e->name = Glb.Loc[loc_TreeTypeEmptyTrack];
                  }
                  else
                  {
                     e->name  = Glb.Loc[loc_TreeTypeDataTrack];
                  }
               }
            }
            break;

         case Item_Index:
            {
               e->number.FormatStr("%ld", ARRAY(e->item->getItemNumber()));
               e->name  = Glb.Loc[loc_TreeTypeIndex];
            }
            break;
      }

      if (e->item->hasCDText())
      {
         e->name  = e->item->getCDTArtist();
         e->name += " - ";
         e->name += e->item->getCDTTitle();
      }

      if (!e->item->isComplete())
      {
         e->name += " (";
         e->name += Glb.Loc[loc_TreeTypeIncomplete];
         e->name += ")";
      }

      len      = (unsigned long long)e->item->getBlockCount() * e->item->getSectorSize();
      suffix   = "B";
      if (len > 1024)
      {
         fraction = (10*(len&1023))>>10;
         len   >>= 10;
         suffix  = "kB";
      }
      if (len > 1024)
      {
         fraction = (10*(len&1023))>>10;
         len   >>= 10;
         suffix  = "MB";
      }
      if (len > 1024)
      {
         fraction = (10*(len&1023))>>10;
         len   >>= 10;
         suffix  = "GB";
      }

      s1 = Glb.Loc.FormatNumber(len, fraction * 100000);
      s2 = Glb.Loc.FormatNumber(e->item->getStartAddress());
      s3 = Glb.Loc.FormatNumber(e->item->getEndAddress());
      e->info.FormatStr("%s %s  (%s - %s)", ARRAY((IPTR)s1.Data(), (IPTR)suffix, (IPTR)s2.Data(), (IPTR)s3.Data()));

      arr[0] = e->number.Data();
      arr[1] = e->name.Data();
      arr[2] = e->info.Data();
   }
   
   _dx(Lvl_Info, "Displaying information %s | %s | %s", (IPTR)arr[0], (IPTR)arr[1], (IPTR)arr[2]);
   _dx(Lvl_Info, "Done.");
   return true;
}

void MUIContents::update()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();

   _dx(Lvl_Info, "Updating Contents page");

   if (NULL != eng)
   {
      const IOptItem * item = eng->getContents();
      _dx(Lvl_Info, "Clearing old items");
      tracks->clear();
      _dx(Lvl_Info, "Recursively adding new items (if any). Disc structure at %08lx", (IPTR)item);
      addRecurse(0, item);
   }

   _dx(Lvl_Info, "Contents page updated.");

   Glb.CurrentEngine->Release();
}

void MUIContents::addRecurse(IPTR parent, const IOptItem* data)
{
   IPTR item;

   if (data == 0)
      return;

   _dx(Lvl_Info, "Adding item %08lx of type %ld to tree", (IPTR)data, data->getItemType());
   item = tracks->addEntry(parent, (IPTR)data, true);

   _dx(Lvl_Info, "Item branch: %08lx", item);
   _dx(Lvl_Info, "This item has %ld children, that will be added now.", (IPTR)data->getChildCount());

   for (int i=0; i<data->getChildCount(); i++)
   {
      addRecurse(item, data->getChild(i));
   }

   _dx(Lvl_Info, "All children have been added");
}

IPTR MUIContents::button(BtnID id, IPTR)
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();


   switch (id)
   {
      case ID_Refresh:
         {
            if (NULL != eng)
            {
               eng->updateAll();
            }
         }
         break;

      case ID_DownloadTracks:
         {
            String dest = (char*)target->getValue();
            if (dest.Length() == 0)
            {
               request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectTargetDirFirst], Glb.Loc[Globals::loc_OK], 0);
               break;
            }
            VectorT<IPTR> &vec = tracks->getSelectedObjects();

            if (vec.Count() == 0)
            {
               request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectTracksToDownload], Glb.Loc[Globals::loc_OK], 0);
               break;
            }

            if (eng->getDataExport() == 0)
            {
               request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectItemExportModule], Glb.Loc[Globals::loc_OK], ARRAY((IPTR)Glb.Loc[loc_DataExport].Data()));
               break;
            }

            if (eng->getAudioExport() == 0)
            {
               request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectItemExportModule], Glb.Loc[Globals::loc_OK], ARRAY((IPTR)Glb.Loc[loc_AudioExport].Data()));
               break;
            }

            if (eng->getSessionExport() == 0)
            {
               request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectItemExportModule], Glb.Loc[Globals::loc_OK], ARRAY((IPTR)Glb.Loc[loc_SessionExport].Data()));
               break;
            }

            for (int i=0; i<vec.Count(); i++)
            {
               String      pname = dest;
               String      fname;
               Entry      *item = (Entry*)vec[i];

               if ((item->item->getItemType() == Item_Disc) || (item->item->getItemType() == Item_Index))
               {
                  request(Glb.Loc[Globals::loc_Req], Glb.Loc[loc_SelectedItemWrongType], Glb.Loc[Globals::loc_OK], 0);
                  continue;
               }

               fname.FormatStr("%02ld.Track.img", ARRAY(item->item->getItemNumber()));
               pname.AddPath(fname);
               eng->downloadTrack(item->item, pname);
            }
         }
         break;

   }

   Glb.CurrentEngine->Release();  
   return 0;
}
