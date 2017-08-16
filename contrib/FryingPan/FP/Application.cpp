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

#include "Application.h"
#include <Generic/Debug.h>
#include <LibC/LibC.h>

uint32 StartupFlags = 0;

Application::Application()
{
   _createDebug(true, "Application");
   Interface = 0;

   _d(Lvl_Info, "%s: Acquiring interface..", (IPTR)__PRETTY_FUNCTION__);
   Module = GUIIFace::GetInstance(0);
   _d(Lvl_Info, "%s: Module caught at %08lx", (IPTR)__PRETTY_FUNCTION__, (IPTR)Module);

   if (NULL != Module)
   {
      _d(Lvl_Info, "%s: Acquiring module interface...", (IPTR)__PRETTY_FUNCTION__);
      Interface = Module->getInterface();
      _d(Lvl_Info, "%s: Obtained interface: %08lx (%s)", (IPTR)__PRETTY_FUNCTION__, (IPTR)Interface, (IPTR)(Interface == 0 ? "Bad" : "Good"));
      ASSERT(NULL != Interface);

      if (NULL != Interface)
      {
         _d(Lvl_Info, "%s: Starting the interface", (IPTR)__PRETTY_FUNCTION__);
         Interface->start();
         _d(Lvl_Info, "%s: Stoping the interface", (IPTR)__PRETTY_FUNCTION__);
         Interface->stop();
         _d(Lvl_Info, "%s: Disposing the interface", (IPTR)__PRETTY_FUNCTION__);
         Interface->dispose();
      }
      Interface = 0;
      _d(Lvl_Info, "%s: Freeing module...", (IPTR)__PRETTY_FUNCTION__);
      Module->FreeInstance();
   }
   else
   {
      request("Information", "The Frying Pan was unable to open its libraries.\nPlease reinstall the software and make sure that\nInstaller adds the Assign to your system.", "Ok", 0);
   }
   Module = 0;
}

Application::~Application()
{
   _d(Lvl_Info, "%s: Shutting down", (IPTR)__PRETTY_FUNCTION__);
   _destroyDebug();
}

void Application::setDebug(DbgHandler* d)
{
   dbg = d;
}

DbgHandler* Application::getDebug()
{
   return dbg;
}
