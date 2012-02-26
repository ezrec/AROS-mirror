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

#include "rRAWAudio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>

IFileReader         *rRAWAudio::openRead(const char* sFile, EDtError &rc)
{
   rRAWAudio *pSkel = 0;
   if (true == checkFile(sFile, rc))
   {
      pSkel = new rRAWAudio(sFile, rc);
   }
   return pSkel;
}

rRAWAudio::rRAWAudio(const char *sName, EDtError &rc)
   : FileReader(sName, rc)
{
   if (rc != DT_OK)
      return;

   _D(Lvl_Info, "%s: Initializing RAW Audio reader", (int)__PRETTY_FUNCTION__);
   setBlockSize(2352);
   setLittleEndian(true);
   setType(Data_Audio);
   _D(Lvl_Info, "%s: RAW Audio reader initialized", (int)__PRETTY_FUNCTION__);
}

const char          *rRAWAudio::static_getName()
{
   return "RAW Audio Track";
}

bool                 rRAWAudio::static_isAudio()
{
   return true;
}

bool                 rRAWAudio::static_isData()
{
   return false;
}

bool                 rRAWAudio::static_isSession()
{
   return false;
}

bool                 rRAWAudio::isAudio()
{
   return static_isAudio();
}

bool                 rRAWAudio::isData()
{
   return static_isData();
}

const char          *rRAWAudio::getName()
{
   return static_getName();
}

// vim: ts=3 et
