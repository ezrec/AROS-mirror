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

#include "MUIPopUnit.h"
#include <Generic/DynList.h>
#include <exec/execbase.h>
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include "Globals.h"
#include "../IEngine.h"
#include <Optical/Optical.h>
#include <mui/NList_mcc.h>

MUIPopUnit::MUIPopUnit(Globals &glb, const char* ttl, const char** cols) :
   MUIPopText(ttl, 3, cols),
   Glb(glb)
{
   sDevice = "";
}

MUIPopUnit::~MUIPopUnit()
{
}

bool MUIPopUnit::onOpen()
{
   clearList();

   if (0 == all)
      return false;

   if (0 == sDevice.Length())
      return false;

   if (NULL == Glb.FryingPanEngine)
      return false;

   const ScanData *d, *t;

   d = Glb.FryingPanEngine->scanDevice(sDevice.Data());

   t = d;

   while (NULL != t)
   {
      addEntry(t);
      t = t->sd_Next;
   }

   Glb.FryingPanEngine->freeScanData(d);

   return true;
}

bool MUIPopUnit::onClose()
{
   Entry *e = (Entry*)getSelectedEntry();
   unit = e->lun;
   return MUIPopString::onClose();
}

void MUIPopUnit::setDevice(const char *device)
{
   sDevice = device;
   onOpen();
}

unsigned long MUIPopUnit::construct(void*, void* arg)
{
   ASSERT(arg != NULL);
   if (NULL == arg)
      return 0;

   ScanData   *d = (ScanData*)arg;
   Entry      *e = new Entry();

   e->unit     = d->sd_Unit;
   e->vendor   = d->sd_Vendor;
   e->product  = d->sd_DeviceName;
   e->lun      = d->sd_Unit;

   e->name.FormatStr("%ld: %s - %s", ARRAY(e->lun, (IPTR)e->vendor.Data(), (IPTR)e->product.Data()));

   return (unsigned long)e;
}

unsigned long MUIPopUnit::destruct(void*, void* arg)
{
   delete (Entry*)arg;
   return 0;
}

unsigned long MUIPopUnit::display(const char** arr, void* arg)
{
   if (NULL == arg)
   {
      return MUIPopText::display(arr, arg);
   }
   else
   {
      Entry *e = (Entry*)arg;

      arr[0]   =  e->unit.Data();
      arr[1]   =  e->vendor.Data();
      arr[2]   =  e->product.Data();
   }

   return 0;
}

const char *MUIPopUnit::toString(const void* arg)
{
   return ((Entry*)arg)->name.Data();
}

const void* MUIPopUnit::getValue()
{
   return (void*)unit;
}
   
void MUIPopUnit::setValue(const void* val)
{
   int      i=0;
   Entry   *e;

   while (true)
   {
      DoMtd((Object *)list, ARRAY(MUIM_NList_GetEntry, i, (IPTR)&e));
      if (NULL   == e)
         break;

      if (e->lun == (IPTR)val)
         break;
      i++;
   }

   if (NULL != e)
   {
      MUIPopText::setValue(e->name.Data());
   }
   else
   {
      MUIPopText::setValue("");
   }
}

