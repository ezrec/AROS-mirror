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

#include "MUIPopDevice.h"
#include <Generic/DynList.h>
#include <exec/execbase.h>
#include <Generic/LibrarySpool.h>

using namespace GenNS;
extern "C" Library *SysBase;

MUIPopDevice::MUIPopDevice(const char* ttl, const char** cols) :
   MUIPopString(ttl, 1, cols)
{
}

MUIPopDevice::~MUIPopDevice()
{
}

bool MUIPopDevice::onOpen()
{
   EList list((MinList*)&((ExecBase*)SysBase)->DeviceList);
   clearList();
   
   for (unsigned int i=0; i<list.GetCount(); i++)
   {
      addEntry((IPTR)list.GetNode(i)->ln_Name);
   }

   return true;
}

bool MUIPopDevice::onClose()
{
   return MUIPopString::onClose();
}

