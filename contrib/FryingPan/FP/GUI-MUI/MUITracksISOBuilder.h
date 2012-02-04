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

#ifndef _GUIMUI_MUITRACKSISOBUILDER_H_
#define _GUIMUI_MUITRACKSISOBUILDER_H_

#include "MUICommon.h"    
#include "MUIPopISOElement.h"
#include <Generic/ConfigParser.h>
#include <Generic/HookT.h>
#include "Components/FileReq.h"
#include <workbench/workbench.h>

class MUIList;
class MUITree;
class IEngine;
class ClElement;
class Globals;

class MUITracksISOBuilder : public MUICommon
{
protected:
   enum BtnID
   {
      ID_Add,
      ID_Remove,
      ID_MakeDir,
      ID_BuildImage,
      ID_Name
   };

   struct Entry 
   {
      ClElement     *elem;
      String         s1;
      String         s2;
      String         s3;
   };

protected:
   Globals                   &Glb;
   MUITree                   *dirs;
   MUIList                   *files;
   
   FileReq                   *req;
   FileReq                   *buildimg;
   MUIPopISOElement          *popelem;

   uint32                    *all;

   ConfigParser              *Config;
   HookT<MUITracksISOBuilder, const char**, class ClDirectory*>   hHkTreeDisplayHook;
   HookT<MUITracksISOBuilder, BtnID, void*>                       hHkButton;
   HookT<MUITracksISOBuilder, ClDirectory*, void*>                hHkTreeSelect;
   HookT<MUITracksISOBuilder, Entry*, void*>                      hHkElemSelect;
   HookT<MUITracksISOBuilder, AppMessage*, void*>                 hHkWBMessage;
   
   HookT<MUITracksISOBuilder, void*, class ClElement*>            hHkFilesConstruct;
   HookT<MUITracksISOBuilder, void*, class Entry*>                hHkFilesDestruct;
   HookT<MUITracksISOBuilder, const char**, class Entry*>         hHkFilesDisplay;

protected:
   void                       showTree(class IBrowser*);
   void                       showContents(class IBrowser*);
   void                       addTreeEntries(uint32 parent, ClDirectory *dir);

   void                       addFiles(IEngine*, IBrowser*);
   void                       removeFiles(IEngine*, IBrowser*);
   void                       buildImage(IEngine*, IBrowser*);

protected:
   uint32                     treeDisplayHook(const char**, ClDirectory*);
   uint32                     button(BtnID, void*);
   uint32                     treeSelect(ClDirectory*, void*);
   uint32                     elemSelect(Entry*, void*);
   uint32                     onWBMessage(AppMessage*, void*);

   uint32                     filesConstruct(void*, ClElement*);
   uint32                     filesDestruct(void*, Entry*);
   uint32                     filesDisplay(const char**, Entry*);

public:
                              MUITracksISOBuilder(ConfigParser *parent, Globals &glb);
                             ~MUITracksISOBuilder();
   uint32                    *getObject();
   bool                       start();
   void                       stop();
   const char                *getName();
   void                       update();

   void                       disableISO();
   void                       enableISO();
};

#endif

