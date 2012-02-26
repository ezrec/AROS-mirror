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

#include "wWAVAudio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>

#define FLIP32(a) ((((a)&0xff000000)>>24) | (((a)&0xff0000)>>8) | (((a)&0xff00)<<8) | (((a)&0xff)<<24))
#define FLIP16(a) ((((a)&0xff00)>>8) | (((a)&0xff)<<8))

#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_RIFF MKID('R','I','F','F')
#define ID_WAVE MKID('W','A','V','E')
#define ID_fmt  MKID('f','m','t',' ')
#define ID_data MKID('d','a','t','a')


IFileWriter         *wWAVAudio::openWrite(const char* sFile, const IOptItem *di, EDtError &rc)
{
   wWAVAudio *pSkel = 0;
   pSkel = new wWAVAudio(sFile, di, rc);
   return pSkel;
}

wWAVAudio::wWAVAudio(const char *sName, const IOptItem *di, EDtError &rc)
   : FileWriter(sName, rc)
{
   setLittleEndian(true);
}

const char          *wWAVAudio::static_getName()
{
   return "WAV Audio Track";
}

bool                 wWAVAudio::static_isAudio()
{
   return true;
}

bool                 wWAVAudio::static_isData()
{
   return false;
}

bool                 wWAVAudio::static_isSession()
{
   return false;
}

bool                 wWAVAudio::isAudio()
{
   return static_isAudio();
}

bool                 wWAVAudio::isData()
{
   return static_isData();
}

const char          *wWAVAudio::getName()
{
   return static_getName();
}

bool                 wWAVAudio::setUp()
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

      i32 = ID_RIFF;
      i32 = L2BE(i32);
      off += DOS->Write(fh, &i32, 4);
      offTotal = off;
      off += DOS->Write(fh, &i32, 4);
      
      i32 = ID_WAVE;
      i32 = L2BE(i32);
      off += DOS->Write(fh, &i32, 4);    // WAVE

      i32 = ID_fmt;
      i32 = L2BE(i32);
      off += DOS->Write(fh, &i32, 4);    // fmt
      
      i32 = L2LE(16);
      off += DOS->Write(fh, &i32, 4);    // fmt size
      
      i16 = W2LE(1);              
      off += DOS->Write(fh, &i16, 2);    // uncompressed format

      i16 = W2LE(2);           
      off += DOS->Write(fh, &i16, 2);    // 2 channels

      i32 = L2LE(44100);
      off += DOS->Write(fh, &i32, 4);    // frequency (44kHz)

      i32 = L2LE(44100*2*2);      
      off += DOS->Write(fh, &i32, 4);    // bytes per second

      i16 = W2LE(2*2);   
      off += DOS->Write(fh, &i16, 2);    // sample size * channels

      i16 = W2LE(16);
      off += DOS->Write(fh, &i16, 2);    // sample resolution

      i32 = ID_data;
      i32 = L2BE(i32);
      off += DOS->Write(fh, &i32, 4);    // data
      offSoundChunk = off;
      off += DOS->Write(fh, &i32, 4);    // data length
   }

   return true;
}

void                 wWAVAudio::cleanUp()
{
   BPTR fh;
   long off = 0;
   int  i32;

   fh = getFile();
   if (fh)
   {
      off = DOS->Seek(fh, offTotal, OFFSET_BEGINNING);     // and grab the total size.
      i32 = L2LE(off - offTotal - 4); 
      DOS->Write(fh, &i32, 4);                             // write total size.
      i32 = L2LE(off - offSoundChunk - 4);
      DOS->Seek(fh, offSoundChunk, OFFSET_BEGINNING);
      DOS->Write(fh, &i32, 4);                             // write ssnd chunk size
   }

   FileWriter::cleanUp();
}

// vim: ts=3 et
