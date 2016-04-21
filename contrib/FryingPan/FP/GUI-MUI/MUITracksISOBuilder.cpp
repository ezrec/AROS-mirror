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

#include "MUITracksISOBuilder.h"
#include <libclass/dos.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include "Components/MUIList.h"
#include "Components/MUITree.h"
#include <ISOBuilder/IBrowser.h>
#include <FP/IEngine.h>
#include "Globals.h"
#include <workbench/startup.h>

/*
 * localization area
 */
enum Loc
{
   loc_Name    =  lg_TracksISO,
   loc_Add,
   loc_Remove,
   loc_MakeDir,
   loc_BuildISO,

   loc_ColDirTree = lg_TracksISO+ls_Group1,

   loc_ColFileName = lg_TracksISO + ls_Group2,
   loc_ColFileSize,

   loc_SelectFiles = lg_TracksISO + ls_Req,
   loc_SelectTarget
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Name,         "ISO Builder",             "LBL_ISOBUILDER"        },
   {  loc_Add,          "a&Add",                   "BTN_ADD"               },
   {  loc_Remove,       "r&Remove",                "BTN_REMOVE"            },
   {  loc_MakeDir,      "c&Create Dir",            "BTN_CREATEDIR"         },
   {  loc_BuildISO,     "b&Build Image",           "BTN_BUILDIMAGE"        },
   {  loc_ColDirTree,   "Dir Tree",                "COL_DIRTREE"           },
   {  loc_ColFileName,  "Item Name",               "COL_ITEMNAME"          },
   {  loc_ColFileSize,  "Item Size",               "COL_ITEMSIZE"          },

   {  loc_SelectFiles,
      "Select files to be added",
      "REQ_SELECTFILES" },
   {  loc_SelectTarget,
      "Select target image file",
      "REQ_SELECTTARGET" },
   
   {  Localization::LocaleSet::Set_Last, 0, 0                              }
};

static const char* LocaleGroup = "TRACKS_ISO";

   static const char*   Cfg_LastImagePath    =  "LastImagePath";
   static const char*   Cfg_LastSourcePath   =  "LastSourcePath";

MUITracksISOBuilder::MUITracksISOBuilder(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all    = 0;
   files  = 0;
   dirs   = 0;
   popelem= 0;

   hHkTreeDisplayHook.Initialize(this, &MUITracksISOBuilder::treeDisplayHook);
   hHkButton.Initialize(this, &MUITracksISOBuilder::button);
   hHkTreeSelect.Initialize(this, &MUITracksISOBuilder::treeSelect);
   hHkElemSelect.Initialize(this, &MUITracksISOBuilder::elemSelect);
   hHkWBMessage.Initialize(this, &MUITracksISOBuilder::onWBMessage);

   hHkFilesConstruct.Initialize(this, &MUITracksISOBuilder::filesConstruct);
   hHkFilesDestruct.Initialize(this, &MUITracksISOBuilder::filesDestruct);
   hHkFilesDisplay.Initialize(this, &MUITracksISOBuilder::filesDisplay);

   Config = new ConfigParser(parent, "ISOBuilder", 0);

   setButtonCallBack(hHkButton.GetHook());
   Glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}
   
MUITracksISOBuilder::~MUITracksISOBuilder()
{
   delete Config;
}

IPTR MUITracksISOBuilder::getObject()
{
   if (0 != all)
      return all;

   files  = new MUIList("BAR,BAR", true); 
   dirs   = new MUITree("");
   req    = new FileReq(Glb.Loc[loc_SelectFiles]);
   req->setMultiSelect(true);
   buildimg = new FileReq(Glb.Loc[loc_SelectTarget]);
   buildimg->setPath(Config->getValue(Cfg_LastImagePath, ""));

   dirs->setDisplayHook(hHkTreeDisplayHook.GetHook());
   dirs->setSelectionHook(hHkTreeSelect.GetHook());

   files->setConstructHook(hHkFilesConstruct.GetHook());
   files->setDestructHook(hHkFilesDestruct.GetHook());
   files->setDisplayHook(hHkFilesDisplay.GetHook());
   files->setSelectionHook(hHkElemSelect.GetHook());
   files->setWBDropHook(hHkWBMessage.GetHook());

   popelem = new MUIPopISOElement(Glb); 
   dirs->setWeight(40);

   req->setPath(Config->getValue(Cfg_LastSourcePath, ""));


   all = (IPTR)VGroup,
      Child,                  HGroup,
         Child,                  dirs->getObject(),

         Child,                  BalanceObject,
         End,

         Child,                  files->getObject(),
      End,

      Child,                  ColGroup(4),
         Child,                  muiButton(Glb.Loc[loc_Add], Glb.Loc.Accel(loc_Add), ID_Add),
         Child,                  muiButton(Glb.Loc[loc_Remove], Glb.Loc.Accel(loc_Remove), ID_Remove),
         Child,                  muiButton(Glb.Loc[loc_MakeDir], Glb.Loc.Accel(loc_MakeDir), ID_MakeDir),
         Child,                  muiButton(Glb.Loc[loc_BuildISO], Glb.Loc.Accel(loc_BuildISO), ID_BuildImage),
      End,

      Child,                  popelem->getObject(),
   End;

   return all;
}

bool MUITracksISOBuilder::start()
{
   return true;
}

void MUITracksISOBuilder::stop()
{
   Config->setValue(Cfg_LastSourcePath, req->getPath());
   Config->setValue(Cfg_LastImagePath, buildimg->getPath());
   if (0 != dirs)
      delete dirs;
   if (0 != files)
      delete files;
   if (0 != popelem)
      delete popelem;
   if (0 != req)
      delete req;
   if (0 != buildimg)
      delete buildimg;

   dirs = 0;
   files = 0;
   popelem = 0;
   req = 0;
   buildimg = 0;
   all = 0;
}

const char *MUITracksISOBuilder::getName()
{
   return Glb.Loc[loc_Name];
}

void MUITracksISOBuilder::update()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   IBrowser *b = pEng->getISOBrowser();
   
   showTree(b);
   showContents(b);

   popelem->setBrowser(b);

   Glb.CurrentEngine->Release();
}

void MUITracksISOBuilder::showTree(IBrowser *b)
{
   dirs->clear();

   addTreeEntries(0, b->getRoot());
   dirs->showObject((IPTR)b->getCurrDir(), true);
}

void MUITracksISOBuilder::showContents(IBrowser *b)
{
   ClDirectory *d = b->getCurrDir();

   files->clear();

   for (int32 i=0; i<d->getChildrenCount(); i++)
   {
      files->addItem((IPTR)d->getChild(i));
   }
}

IPTR MUITracksISOBuilder::treeDisplayHook(const char** arr, ClDirectory* elem)
{
   if (elem != 0)
   {
      arr[0] = elem->getNormalName();
   }
   else
   {
      arr[0] = Glb.Loc[loc_ColDirTree];
   }
   return 0;
}

void MUITracksISOBuilder::addTreeEntries(IPTR parent, ClDirectory* elem)
{
   parent = dirs->addEntry(parent, (IPTR)elem);
   for (int32 i=0; i<elem->getChildrenCount(); i++)
   {
      if (elem->getChild(i)->isDirectory())
      {
         addTreeEntries(parent, static_cast<ClDirectory*>(elem->getChild(i)));
      }
   }
}

IPTR MUITracksISOBuilder::button(BtnID id, IPTR data)
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();
   IBrowser *b = eng->getISOBrowser();
   
   switch (id)
   {
      case ID_MakeDir:
         {
            b->makeDir();
            update();
         }
         break;

      case ID_Remove:
         {
            removeFiles(eng, b);
         }
         break;

      case ID_Add:
         {
            addFiles(eng, b);
         }
         break;

      case ID_BuildImage:
         {
            buildImage(eng, b);
         }
         break;

      default:
         break;
   }

   Glb.CurrentEngine->Release();
   return 0;
}

IPTR MUITracksISOBuilder::treeSelect(ClDirectory* dir, IPTR)
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();
   IBrowser *b = eng->getISOBrowser();

   if (dir != 0)
   {
      b->setCurrDir(dir);
      showContents(b);
   }

   Glb.CurrentEngine->Release();
   return 0;
}

IPTR MUITracksISOBuilder::elemSelect(Entry* elem, IPTR)
{
   if (elem != NULL)
   {
      popelem->setElement(elem->elem);
   }
   else
   {
      popelem->setElement(0);
   }
   return 0;
}

IPTR MUITracksISOBuilder::filesConstruct(IPTR, ClElement* clelem)
{
   Entry *e = new Entry;
   e->elem = clelem;

   return (IPTR)e;
}

IPTR MUITracksISOBuilder::filesDestruct(IPTR, Entry* e)
{
   delete e;
   return 0;
}

IPTR MUITracksISOBuilder::filesDisplay(const char** arr, Entry* e)
{
   if (e == 0)
   {
      arr[0] = Glb.Loc[loc_ColFileName];
      arr[1] = Glb.Loc[loc_ColFileSize];
   }
   else
   {
      if (e->elem->isDirectory())
      {
         e->s1.FormatStr("\0333%s", ARRAY((IPTR)e->elem->getNormalName()));
         e->s2 = "\0333<DIR>";
      }
      else
      {
         e->s1.FormatStr("%s", ARRAY((IPTR)e->elem->getNormalName()));
         e->s2 = Glb.Loc.FormatNumber(e->elem->getISOSize(), 0);
      }

      arr[0] = e->s1.Data();
      arr[1] = e->s2.Data();
   }
   return 0;
}

void MUITracksISOBuilder::addFiles(IEngine *e, IBrowser* b)
{
   VectorT<const char*> &v = req->openReq();

   for (int32 i=0; i<v.Count(); i++)
   {
      e->addISOItem(v[i]);
      //b->getCurrDir()->addChild(v[i]);
   }

   e->layoutTracks(false, false);
   update();
}

void MUITracksISOBuilder::buildImage(IEngine *e, IBrowser* b)
{
   String name = buildimg->saveReq();

   e->createImage(name.Data());
}

void MUITracksISOBuilder::removeFiles(IEngine *e, IBrowser* b)
{
   VectorT<Entry*> &v = (VectorT<Entry*>&)files->getSelectedItems();

   for (int32 i=0; i<v.Count(); i++)
   {
      e->remISOItem(v[i]->elem);
   }

   e->layoutTracks(false, false);
   update();
   
}

void MUITracksISOBuilder::disableISO()
{
   files->setEnabled(false);
   dirs->setEnabled(false);
   popelem->setEnabled(false);
   muiSetEnabled(ID_Add, false);
   muiSetEnabled(ID_Remove, false);
   muiSetEnabled(ID_MakeDir, false);
   muiSetEnabled(ID_BuildImage, false);
   popelem->close();
}

void MUITracksISOBuilder::enableISO()
{
   update();
   files->setEnabled(true);
   dirs->setEnabled(true);
   popelem->setEnabled(true);
   muiSetEnabled(ID_Add, true);
   muiSetEnabled(ID_Remove, true);
   muiSetEnabled(ID_MakeDir, true);
   muiSetEnabled(ID_BuildImage, true);
}
   
IPTR MUITracksISOBuilder::onWBMessage(AppMessage* m, IPTR)
{
   char *c = new char[1024];
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();

   for (int32 i=0; i<m->am_NumArgs; i++)
   {
      DOS->NameFromLock(m->am_ArgList[i].wa_Lock, c, 1024);
      DOS->AddPart(c, (char*)m->am_ArgList[i].wa_Name, 1024);
      pEng->addISOItem(c);
   }

   delete []c;
   pEng->layoutTracks(false, false);
   Glb.CurrentEngine->Release();
   return 0;
}
