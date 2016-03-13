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

#include <GenericLib/Library.h>
#include <libclass/dos.h>
#include <LibC/LibC.h>
#include <Generic/LibrarySpool.h>
#include <Generic/Configuration.h>
#include <Optical/Optical.h>
#include <ISOBuilder/Calls.h>
#include <DTLib/Calls.h>

#include "Engine.h"
#include "Globals.h"

   // TARGET:
   // both fryingpan and coffe work on common engine
   // this CAN BE DONE!

   LIBRARY("fryingpan.engine", "$VER: fryingpan.engine 1.2 Tomasz Wiszkowski", 1);
   GATE1(IEngine*,         getEngine,     int,              d0);
   GATE1(const ScanData*,  scanDevice,    const char*,      a0);
   GATE1(void,             freeScanData,  const ScanData*,  a0);

   LIB_FT_Begin
      LIB_FT_Function(getEngine)
      LIB_FT_Function(scanDevice)
      LIB_FT_Function(freeScanData)
   LIB_FT_End

   Configuration    *pConfig;
   VectorT<Engine*> *pEngines;
   uint32            StartupFlags = 0;

bool Lib_SetUp()
{
   __setup();

   LibrarySpool::Init();
   pRWHooks = new TList<IRegHook*>();

   pOptical = OpticalIFace::GetInstance(0);
   if (NULL == pOptical)
   {
      request("Warning", "Optical.drive seems to be missing.\nPlease check your installation, verify that\nadequate LIBS: assign is present and\nlibraries are both readable and executable", "Continue", 0);
   }

   pISO = ISOBuilderIFace::GetInstance(0);
   if (NULL == pISO)
   {
      request("Information", "I was not able to locate isobuilder.module\nISO construction will not work.", "Proceed", 0);
   }

   pDT  = DTLibIOIFace::GetInstance(0);
   if (NULL == pDT)
   {
      request("Information", "I was unable to find the dtlibio.module\nData import and export will not work.", "Proceed", 0);
   }
   else
   {
      pDT->Register(pRWHooks->GetList());
      pRWHooks->Update();
   }

   //
   // make sure config dirs exist
   //
   {
      BPTR lock;
      lock = DOS->CreateDir("ENVARC:FryingPan");
      if (lock)
         DOS->UnLock(lock);
      lock = DOS->CreateDir("ENV:FryingPan");
      if (lock)
         DOS->UnLock(lock);
   }

   // 
   // now read the configuration
   //
   {
      pConfig  = new Configuration("Engine");
      ASSERT(NULL != pConfig);
      if (NULL != pConfig)
      {
         pConfig->readFile("ENV:FryingPan/Engine.prefs");
      }
   }

   // 
   // initialize engines
   //
   {
      pEngines = new VectorT<Engine*>();
      (*pEngines) << new Engine(pConfig, 1);
      (*pEngines) << new Engine(pConfig, 2);
      (*pEngines) << new Engine(pConfig, 3);
      (*pEngines) << new Engine(pConfig, 4);
   }

   //
   // we're done with initialization
   //
   return true;
}

bool freeEngines(Engine* const& eng)
{
   delete eng;
   return true;
}

void Lib_CleanUp()
{
   if (pEngines != NULL)
   {
      pEngines->ForEach(&freeEngines);
      delete pEngines;
   }
   pEngines = 0;

   if (NULL != pDT)
   {
      pDT->Unregister(pRWHooks->GetList());
      pRWHooks->Update();
      pDT->FreeInstance();
   }

   if (NULL != pOptical)
   {
      pOptical->FreeInstance();
   }

   if (NULL != pISO)
   {
      pISO->FreeInstance();
      pISO = 0;
   }
   //
   // this way we have valid config pointer and valid settings at the end.
   //
   if (NULL != pConfig)
   {
      pConfig->writeFile("ENVARC:FryingPan/Engine.prefs");
      pConfig->writeFile("ENV:FryingPan/Engine.prefs");
      delete pConfig;
      pConfig = 0;
   }

   delete pRWHooks;
   LibrarySpool::Exit();
   __cleanup();
}

bool Lib_Acquire()
{
   return true;
}

void Lib_Release()
{

   for (int i=0; i<pEngines->Count(); i++)
   {
      (*pEngines)[i]->writeSettings();
   }
   

   if (NULL != pConfig)
   {
      pConfig->writeFile("ENVARC:FryingPan/Engine.prefs");
      pConfig->writeFile("ENV:FryingPan/Engine.prefs");
   }

 }

int main()
{
    request("Information", "This module carries FryingPan Main Engine.\nTo use it you need to launch executable.", "Ok", 0);
    return -1;
}

IEngine *getEngine(int which)
{
   ASSERT((0 <= which) && (3 >= which));
   return (*pEngines)[which];
}

const ScanData* scanDevice(const char* device)
{
   if (NULL == pOptical)
      return 0;
   return (ScanData*)pOptical->OptDoMethodA(ARRAY(DRV_ScanDevice, (IPTR)device));
}

void freeScanData(const ScanData* data)
{
   if (NULL == pOptical)
      return;

   pOptical->OptDoMethodA(ARRAY(DRV_FreeScanResults, (IPTR)data));
}

// vim: ts=3 et
