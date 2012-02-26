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

#ifndef _GUIMUI_GLOBALS_H_
#define _GUIMUI_GLOBALS_H_

#include <Generic/SyncProperty.h>
#include <Generic/Debug.h>
#include <Generic/PropertyT.h>
#include <Generic/Locale.h>

class IEngine;
class EngineIFace;

   /*
    * normal GLOBAL data would work only per whole library,
    * but not per instance. thus we need this class.
    */

using namespace GenNS;

enum LocaleGroups
{
   /* groups */
   lg_Global         =  1000,
   lg_Tracks         =  2000,
   lg_TracksISO      =  3000,
   lg_TracksData     =  4000,
   lg_TracksImage    =  5000,
   lg_Media          =  6000,
   lg_Contents       =  7000,
   lg_Drive          =  8000,
   lg_Settings       =  9000,
   lg_Write          =  10000,

   /* sub groups -- lg+ls */
   ls_Main           =  0,
   ls_Group1         =  200,
   ls_Group2         =  300,
   ls_Group3         =  400,
   ls_Group4         =  500,
   ls_Group5         =  600,
   ls_Group6         =  700,
   ls_Group7         =  800,
   ls_Req            =  900,

   /* elements -- to be used only by small UI modules */
   le_DeviceSelect   =  100000,
   le_PopDevice      =  100100,
   le_PopUnit        =  100200,
   le_PopAction      =  100300,
   le_PageSelect     =  100400,
   le_ToolBar        =  100500,
   le_PopISOElement  =  100600,
};

struct Globals 
{
public:
   enum eWriteSelect
   {
      Select_Tracks,
      Select_Session
   };

   enum 
   {
      loc_Req              =  0,
      loc_Info,
      loc_Warn,
      loc_Error,

      loc_OK               =  100,
      loc_Proceed,
      loc_Abort,
      loc_YesNo,
      loc_ContinueAbort,
   };

public:
   DbgHandler                *dbg;

   EngineIFace               *FryingPanEngine;
   IEngine                   *Engines[4];
   SyncProperty<IEngine*>    *CurrentEngine;
   Localization               Loc;

   /*
    * this item defines what precisely shoud be written.
    */
   PropertyT<eWriteSelect>    WriteSelection;

   Globals();
};

#endif

