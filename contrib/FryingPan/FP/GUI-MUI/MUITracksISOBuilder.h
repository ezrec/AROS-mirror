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

   IPTR                       all;

   ConfigParser              *Config;
   HookT<MUITracksISOBuilder, const char**, class ClDirectory*>   hHkTreeDisplayHook;
   HookT<MUITracksISOBuilder, BtnID, IPTR>                        hHkButton;
   HookT<MUITracksISOBuilder, ClDirectory*, IPTR>                 hHkTreeSelect;
   HookT<MUITracksISOBuilder, Entry*, IPTR>                       hHkElemSelect;
   HookT<MUITracksISOBuilder, AppMessage*, IPTR>                  hHkWBMessage;
   
   HookT<MUITracksISOBuilder, IPTR, class ClElement*>             hHkFilesConstruct;
   HookT<MUITracksISOBuilder, IPTR, class Entry*>                 hHkFilesDestruct;
   HookT<MUITracksISOBuilder, const char**, class Entry*>         hHkFilesDisplay;

protected:
   void                       showTree(class IBrowser*);
   void                       showContents(class IBrowser*);
   void                       addTreeEntries(IPTR parent, ClDirectory *dir);

   void                       addFiles(IEngine*, IBrowser*);
   void                       removeFiles(IEngine*, IBrowser*);
   void                       buildImage(IEngine*, IBrowser*);

protected:
   IPTR                       treeDisplayHook(const char**, ClDirectory*);
   IPTR                       button(BtnID, IPTR);
   IPTR                       treeSelect(ClDirectory*, IPTR);
   IPTR                       elemSelect(Entry*, IPTR);
   IPTR                       onWBMessage(AppMessage*, IPTR);

   IPTR                       filesConstruct(IPTR, ClElement*);
   IPTR                       filesDestruct(IPTR, Entry*);
   IPTR                       filesDisplay(const char**, Entry*);

public:
                              MUITracksISOBuilder(ConfigParser *parent, Globals &glb);
                             ~MUITracksISOBuilder();
   virtual IPTR               getObject();
   bool                       start();
   void                       stop();
   const char                *getName();
   void                       update();

   void                       disableISO();
   void                       enableISO();
};

#endif

