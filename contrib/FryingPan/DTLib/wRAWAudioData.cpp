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

#include "wRAWAudioData.h"
#include <exec/lists.h>
#include <Generic/HookT.h>

IFileWriter         *wRAWAudioData::openWrite(const char* sFile, const IOptItem *di, EDtError &rc)
{
   wRAWAudioData *pSkel = 0;
   pSkel = new wRAWAudioData(sFile, di, rc);
   return pSkel;
}

wRAWAudioData::wRAWAudioData(const char *sName, const IOptItem *it, EDtError &rc)
   : FileWriter(sName, rc)
{
   setLittleEndian(true);
}

const char          *wRAWAudioData::static_getName()
{
   return "RAW Audio/Data Track";
}

bool                 wRAWAudioData::static_isAudio()
{
   return true;
}

bool                 wRAWAudioData::static_isData()
{
   return true;
}

bool                 wRAWAudioData::isAudio()
{
   return static_isAudio();
}

bool                 wRAWAudioData::isData()
{
   return static_isData();
}

const char          *wRAWAudioData::getName()
{
   return static_getName();
}

bool                 wRAWAudioData::static_isSession()
{
   return false;
}

// vim: ts=3 et
