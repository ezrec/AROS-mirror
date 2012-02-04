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

#include "wAIFFAudio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>

#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_FORM MKID('F','O','R','M')
#define ID_COMM MKID('C','O','M','M')
#define ID_AIFF MKID('A','I','F','F')
#define ID_SSND MKID('S','S','N','D')


IFileWriter         *wAIFFAudio::openWrite(const char* sFile, const IOptItem *it, EDtError &rc)
{
   wAIFFAudio *pSkel = 0;
   pSkel = new wAIFFAudio(sFile, it, rc);
   return pSkel;
}

wAIFFAudio::wAIFFAudio(const char *sName, const IOptItem *it, EDtError &rc)
   : FileWriter(sName, rc)
{
   setLittleEndian(false);
}

const char          *wAIFFAudio::static_getName()
{
   return "AIFF Audio Track";
}

bool                 wAIFFAudio::static_isAudio()
{
   return true;
}

bool                 wAIFFAudio::static_isData()
{
   return false;
}

bool                 wAIFFAudio::static_isSession()
{
   return false;
}

bool                 wAIFFAudio::isAudio()
{
   return static_isAudio();
}

bool                 wAIFFAudio::isData()
{
   return static_isData();
}

const char          *wAIFFAudio::getName()
{
   return static_getName();
}

bool                 wAIFFAudio::setUp()
{
   BPTR fh;
   long off = 0;
   
   if (false == FileWriter::setUp())
   {
      return false;
   }

   fh = getFile();
   ASSERT(fh != 0);

   if (fh)
   {
      int   i32;
      short i16;

      i32 = L2BE(ID_FORM);
      off += DOS->Write(fh, &i32, 4);
      offTotal = off;

      i32 = L2BE(ID_AIFF);
      off += DOS->Write(fh, &i32, 4);
      off += DOS->Write(fh, &i32, 4);

      i32 = L2BE(ID_COMM);
      off += DOS->Write(fh, &i32, 4);
       
      i32 = L2BE(18); i16 = W2BE(2);
      
      off += DOS->Write(fh, &i32, 4);    // comm size 
      off += DOS->Write(fh, &i16, 2);    // num channels
      offSamples = off;
      
      off += DOS->Write(fh, &i32, 4);
      
      i16 = W2BE(16); i32 = L2BE(0x400eac44);   // hard coded frequency 

      off += DOS->Write(fh, &i16, 2);    // bits per sample
      off += DOS->Write(fh, &i32, 4);    // frequency, 1

      i32 = 0; i16 = 0;
      off += DOS->Write(fh, &i32, 4);    // frequency, 2
      off += DOS->Write(fh, &i16, 2);    // frequency, 3

      i32 = L2BE(ID_SSND);
      off += DOS->Write(fh, &i32, 4);    // SSND chunk
      offSoundChunk = off;
      off += DOS->Write(fh, &i32, 4);    // and its size. voila.
   }

   return true;
}

void                 wAIFFAudio::cleanUp()
{
   BPTR fh;
   long off = 0;
   int  i32;

   fh = getFile();
   if (fh)
   {
      off = DOS->Seek(fh, offTotal, OFFSET_BEGINNING);     // and grab the total size.
      i32 = off - offTotal - 4; 
      i32 = L2BE(i32);
      DOS->Write(fh, &i32, 4);                             // write total size.
      i32 = off - offSoundChunk - 4;
      i32 = L2BE(i32);
      DOS->Seek(fh, offSoundChunk, OFFSET_BEGINNING);
      DOS->Write(fh, &i32, 4);                             // write ssnd chunk size
      i32 >>= 2;
      i32 = L2BE(i32);
      DOS->Seek(fh, offSamples, OFFSET_BEGINNING);
      DOS->Write(fh, &i32, 4);                             // write num samples
   }

   FileWriter::cleanUp();
}

// vim: ts=3 et
