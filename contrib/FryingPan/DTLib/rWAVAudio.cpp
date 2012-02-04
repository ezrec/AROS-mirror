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

#include "rWAVAudio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>

#define FLIP16(a) ((((a)&0xff00)>>8) | (((a)&0xff)<<8))

#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_RIFF MKID('R','I','F','F')
#define ID_WAVE MKID('W','A','V','E')
#define ID_fmt  MKID('f','m','t',' ')
#define ID_data MKID('d','a','t','a')

struct WaveHdr
{
   unsigned short    format;
   unsigned short    channels;
   unsigned long     frequency;
   unsigned long     bytesPerSecond;
   unsigned short    bytesPerSample;
   unsigned short    resolution;
};

IFileReader         *rWAVAudio::openRead(const char* sFile, EDtError &rc)
{
   rWAVAudio *pSkel = 0;
   if (true == rWAVAudio::checkFile(sFile, rc))
   {
      pSkel = new rWAVAudio(sFile, rc);
   }
   return pSkel;
}

rWAVAudio::rWAVAudio(const char *sName, EDtError &rc)
   : FileReader(sName, rc)
{
   if (rc != DT_OK)
      return;

   readHeaders();
   setBlockSize(2352);
   setLittleEndian(true);
   setType(Data_Audio);
}

const char          *rWAVAudio::static_getName()
{
   return "WAV Audio Track";
}

bool                 rWAVAudio::static_isAudio()
{
   return true;
}

bool                 rWAVAudio::static_isData()
{
   return false;
}

bool                 rWAVAudio::static_isSession()
{
   return false;
}

bool                 rWAVAudio::isAudio()
{
   return static_isAudio();
}

bool                 rWAVAudio::isData()
{
   return static_isData();
}

const char          *rWAVAudio::getName()
{
   return static_getName();
}

void                 rWAVAudio::readHeaders()
{
   unsigned int     buff;
   int              c=0;

   _D(Lvl_Info, "%s: Reading headers", (int)__PRETTY_FUNCTION__);
   DOS->Seek(getFile(), 0, OFFSET_BEGINNING);

   if (DOS->Read(getFile(), &buff, 4)!=4) 
   {
      _D(Lvl_Info, "%s: File seems to be too small.. reading failed.", (int)__PRETTY_FUNCTION__);
      return;
   }
   buff = L2BE(buff);
   if (buff != ID_RIFF) 
   {
      _D(Lvl_Info, "%s: RIFF Wave file header not present (got: %08lx)", (int)__PRETTY_FUNCTION__, buff);
      return;
   }

   DOS->Seek(getFile(), 4, OFFSET_CURRENT);

   if (DOS->Read(getFile(), &buff, 4)!=4) 
   {
      _D(Lvl_Info, "%s: File truncated..?", (int)__PRETTY_FUNCTION__);
      return;
   }
   buff = L2BE(buff);
   if (buff != ID_WAVE) 
   {
      _D(Lvl_Info, "%s: This RIFF is not Wave... (%08lx)", (int)__PRETTY_FUNCTION__, buff);
      return;
   }

   for (c=0; c<2;) 
   {
      if (DOS->Read(getFile(), &buff, 4)!=4) 
      {
         _D(Lvl_Info, "%s: File truncated...", (int)__PRETTY_FUNCTION__);
         return;
      }

      _D(Lvl_Info, "%s: Read tag: %08lx", (int)__PRETTY_FUNCTION__, buff);

      buff = L2BE(buff);

      if (buff == ID_fmt) 
      {
         _D(Lvl_Info, "%s: format tag located.", (int)__PRETTY_FUNCTION__);
         c++;
      }
      else if (buff == ID_data) 
      {
         _D(Lvl_Info, "%s: data tag located.", (int)__PRETTY_FUNCTION__);
         c++;
         if (DOS->Read(getFile(), &buff, 4)!=4) 
         {
            _D(Lvl_Info, "%s: file truncated.", (int)__PRETTY_FUNCTION__);
            return;
         }

         setDataOffset(DOS->Seek(getFile(), 0, OFFSET_CURRENT));
         buff = L2LE(buff);
         setDataSize(buff);
         DOS->Seek(getFile(), -4, OFFSET_CURRENT);
      }
      if (DOS->Read(getFile(), &buff, 4)!=4) 
      {
         _D(Lvl_Info, "%s: Failed to read chunk size. File truncated.", (int)__PRETTY_FUNCTION__);
         return;
      }
      buff = L2LE(buff);
      _D(Lvl_Info,"%s: chunk size = %ld bytes.", (int)__PRETTY_FUNCTION__, buff);
      DOS->Seek(getFile(), buff, OFFSET_CURRENT);
   }

   return;
}

bool                 rWAVAudio::checkHeader(WaveHdr* pHdr, EDtError &rc)
{
   if (W2LE(1)     != pHdr->format)
   {
      rc = DT_FileFormatNotSupported;
      return false;
   }
   if (W2LE(2)     != pHdr->channels)
   {
      rc = DT_WrongChannelCount;
      return false;
   }
   if (L2LE(44100) != pHdr->frequency)
   {
      rc = DT_WrongFrequency;
      return false;
   }
   if (W2LE(16)    != pHdr->resolution)
   {
      rc = DT_WrongResolution;
      return false;
   }
   rc = DT_OK;
   return true;
}

bool                 rWAVAudio::checkFile(const char* sFileName, EDtError &rc)
{
   unsigned int    buff;
   int             c=0, len;
   BPTR            fh;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(sFileName), MODE_OLDFILE);
   if (0 == fh)
      return false;
        
   DOS->Seek(fh, 0, OFFSET_BEGINNING);

   if (DOS->Read(fh, &buff, 4)!=4) 
   {
      DOS->Close(fh);
      rc = DT_FileMalformed;
      return false;
   }
   buff = L2BE(buff);
   if (buff != ID_RIFF) 
   {
      DOS->Close(fh);
      rc = DT_InvalidFormat;
      return false;
   }

   DOS->Seek(fh, 4, OFFSET_CURRENT);

   if (DOS->Read(fh, &buff, 4)!=4) 
   {
      DOS->Close(fh);
      rc = DT_FileMalformed;
      return false;
   }
   buff = L2BE(buff);
   if (buff != ID_WAVE) 
   {
      DOS->Close(fh);
      rc = DT_InvalidFormat;
      return false;
   }

   for (c=0;c<2;) 
   {
      if (DOS->Read(fh, &buff, 4)!=4) 
      {
         rc = DT_FileMalformed;
         DOS->Close(fh);
         return false;
      }

      buff = L2BE(buff);
      if (buff == ID_fmt) 
      {
         WaveHdr *wave_hdr = 0;
         bool     bCorrect = false;

         buff = 0;
         DOS->Read(fh, &buff, 4);
         len = L2LE(buff);
         wave_hdr = (WaveHdr*)new char[len];

         if ((NULL != wave_hdr) && (len == DOS->Read(fh, wave_hdr, len)))
         {
            bCorrect = checkHeader(wave_hdr, rc);
            delete [] ((char*)wave_hdr);
         }
         c++;

         if (false == bCorrect)
         {
            DOS->Close(fh);
            return false;
         }
         continue;
      }
      else if (buff == ID_data) 
      {
         c++;
      }
      
      if (DOS->Read(fh, &buff, 4)!=4) 
      {
         rc = DT_FileMalformed;
         DOS->Close(fh);
         return false;
      }

      buff = L2LE(buff);
      DOS->Seek(fh, buff, OFFSET_CURRENT);
   }

   DOS->Close(fh);
   return true;
}


