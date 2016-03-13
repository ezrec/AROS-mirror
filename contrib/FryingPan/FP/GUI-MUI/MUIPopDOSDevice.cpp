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

#include "MUIPopDOSDevice.h"
#include <libclass/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <Generic/DynList.h>
#include <exec/execbase.h>
#include <Generic/LibrarySpool.h>
#include <dos/filehandler.h>

MUIPopDOSDevice::MUIPopDOSDevice() :
   MUIPopString(0)
{
}

MUIPopDOSDevice::~MUIPopDOSDevice()
{
   data.ForEach(&freeString);
}
   
bool MUIPopDOSDevice::freeString(String* const& s)
{
   delete s;
   return true;
}

bool MUIPopDOSDevice::onOpen()
{
   String              *s;
   DeviceNode          *dn;

   data.Empty();

   clearList();

   dn = (DeviceNode*)DOS->LockDosList(LDF_DEVICES | LDF_READ);

   while (NULL != (dn = (DeviceNode*)DOS->NextDosEntry((DosList*)dn, LDF_DEVICES)))
   {
      if ((IPTR)dn->dn_Startup > 1024)
      {
         s = new String();
         s->BstrCpy(dn->dn_Name);

         addEntry((IPTR)s->Data());
         data << s;
      }
   }

   DOS->UnLockDosList(LDF_DEVICES);

   return true;
}

const char* MUIPopDOSDevice::toString(const void* d)
{
   if (d == 0)
      return "-";
   return (const char*)d;
}

bool MUIPopDOSDevice::onClose()
{
   return MUIPopString::onClose();
}

