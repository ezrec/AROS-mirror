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

#ifndef _FP_GUIMUI_GUI_H_
#define _FP_GUIMUI_GUI_H_

#include "../GUI/GenericUI.h"
#include "Components/MUI.h"
#include <Generic/GenericMUI.h>
#include <Generic/HookT.h>
#include "Globals.h"
#include <Generic/Timer.h>
#include <Generic/Port.h>
#include "../IEngine.h"
#include <libclass/icon.h>
#include <Generic/Configuration.h>

class MUIPageSelect;
class MUITracks;
class MUIMedia;
class MUIContents;
class MUIDrive;
class MUISettings;
class MUIRecord;
class MUIDriveSelect;
class MUIPopAction;
class MUIWindowMenu;

using namespace GenNS;

class GUI : public GenericUI, public MUI
{
protected:
   enum 
   {
      ID_MainProgress      =  1,
   };

   enum ActionID
   {
      Action_AboutMUI,
      Action_MUISettings,
      Action_Translate,
      Action_Quit
   };
protected:
   Configuration       *Config;
   ConfigParser        *Cfg;

   Globals              Glb;
   Object              *pApp;
   Object              *pWin;
   MUIPageSelect       *Select;
   MUIDriveSelect      *DriveSelect;
   MUITracks           *Tracks;
   MUIMedia            *Media;
   MUIContents         *Contents;
   MUIDrive            *Drive;
   MUISettings         *Settings;
   MUIRecord           *Record;

   MUIPopAction        *Action;
   MUIWindowMenu       *Menu;

   Object              *elements;
   Object              *pages;
   Object              *switchview;
   Object              *gauge;

   bool                 bCompact;

   Port                *pMsgPort;
   IconIFace           *iicon;
   DiskObject          *appobj;

protected:
   HookT<GUI, IPTR, IPTR>                 hShowHide;
   HookT<GUI, IPTR, long>                 hPager;
   HookT<GUI, ActionID, IPTR>             hAction;
   HookT<GUI, EngineMessage, IEngine*>    hEngineCmd;       // as received from engine
   HookT<GUI, EngineMessage, IEngine*>    hEngineMsg;       // as received via dosync()
   HookT<GUI, IEngine*, IEngine*>         hEngineChanged;

   Timer                                 *pTimer;
protected:
   IPTR                       doShowHide(IPTR, IPTR);
   IPTR                       doChangePage(IPTR, long page);
   IPTR                       doUserAction(ActionID, IPTR);
   IPTR                       doEngineMessage(EngineMessage, IEngine*);
   IPTR                       doEngineInternalMessage(EngineMessage, IEngine*);
   IPTR                       doEngineChanged(IEngine* old, IEngine* current);

   void                       setDebug(DbgHandler *h);
   DbgHandler                *getDebug();

public:
                              GUI();
                             ~GUI();
   bool                       start();
   void                       stop();
   void                       dispose();
   void                       update();
   void                       periodicUpdate();
   void                       layoutUpdate();
};

#endif

