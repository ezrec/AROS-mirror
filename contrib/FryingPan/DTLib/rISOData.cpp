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

#include "rISOData.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>

#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_CD00 MKID('C','D','0','0')


IFileReader          *rISOData::openRead(const char* sFile, EDtError &rc)
{
   rISOData *pSkel = 0;
   if (true == checkFile(sFile, rc))
   {
      pSkel = new rISOData(sFile, rc);
   }
   return pSkel;
}

rISOData::rISOData(const char *sName, EDtError &rc)
    : FileReader(sName, rc)
{
   if (rc != DT_OK)
      return;

   setBlockSize(2048);
   setLittleEndian(true); // so it is not touched
   setType(Data_Mode1);

   {
      BPTR fh;
      uint8 *ut = new uint8[4096];
      int64 sz = 0;

      fh = DOS->Open((char*)sName, MODE_OLDFILE);
      // seek to primary vol descriptor + volume space size + MSB data

      for (int i=0; i<9; i++)
         DOS->Read(fh, ut, 4096);

      sz = (ut[84] << 24) | (ut[85] << 16) | (ut[86] << 8) | ut[87];
      sz = (sz + 15) & ~15;
      
      DOS->Close(fh);
      delete [] ut;
      setDataSize(sz << 11);
   }
}

bool                 rISOData::checkFile(const char* sFileName, EDtError &rc)
{
   bool            ok = true;
   BPTR            fh;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(sFileName), MODE_OLDFILE);
   if (0 == fh)
      return false;
  
   uint8 *ut = new uint8[4096];

   for (int i=0; i<9; i++)
   {
      if (DOS->Read(fh, ut, 4096) < 4096)
         ok = false;
      if (!ok)
         break;
   }

   if (!ok)
   {
      rc = DT_FileMalformed;
   }
   else
   {
      if (ut[1] != 'C')
         ok = false;
      if (ut[2] != 'D')
         ok = false;
      if (ut[3] != '0')
         ok = false;
      if (ut[4] != '0')
         ok = false;
      if (ut[5] != '1')
         ok = false;

      if (!ok)
         rc = DT_InvalidFormat;
   }

   DOS->Close(fh);
   delete [] ut;
   return ok;
}

const char          *rISOData::static_getName()
{
   return "ISO Disc Image Reader";
}

bool                 rISOData::static_isAudio()
{
   return false;
}

bool                 rISOData::static_isData()
{
   return true;
}

bool                 rISOData::static_isSession()
{
   return false;
}

bool                 rISOData::isAudio()
{
   return static_isAudio();
}

bool                 rISOData::isData()
{
   return static_isData();
}

const char          *rISOData::getName()
{
   return static_getName();
}

// vim: ts=3 et
