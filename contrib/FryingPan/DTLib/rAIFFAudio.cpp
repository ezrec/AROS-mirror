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

#include "rAIFFAudio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>

#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_FORM MKID('F','O','R','M')
#define ID_COMM MKID('C','O','M','M')
#define ID_AIFF MKID('A','I','F','F')
#define ID_SSND MKID('S','S','N','D')


IFileReader         *rAIFFAudio::openRead(const char* sFile, EDtError &rc)
{
   rAIFFAudio *pSkel = 0;
   if (true == checkFile(sFile, rc))
   {
      pSkel = new rAIFFAudio(sFile, rc);
   }
   return pSkel;
}

rAIFFAudio::rAIFFAudio(const char *sName, EDtError &rc)
   : FileReader(sName, rc)
{
   if (rc != DT_OK)
      return;

   readHeaders();
   setBlockSize(2352);
   setLittleEndian(false);
   setType(Data_Audio);
}

void                 rAIFFAudio::readHeaders()
{
   unsigned int     buff;
   int              c=0;

   DOS->Seek(getFile(), 0, OFFSET_BEGINNING);

   if (DOS->Read(getFile(), &buff, 4)!=4) 
      return;
   buff = L2BE(buff);
   if (buff != ID_FORM) 
      return;

   DOS->Seek(getFile(), 4, OFFSET_CURRENT);
   if (DOS->Read(getFile(), &buff, 4)!=4) 
      return;
   buff = L2BE(buff);
   if (buff != ID_AIFF) 
      return;

   for (c=0; c<2;) 
   {
      if (DOS->Read(getFile(), &buff, 4)!=4) 
         return;

      if (buff == ID_COMM) 
      {
         c++;
      }
      else if (buff == ID_SSND) 
      {
         c++;
         if (DOS->Read(getFile(), &buff, 4)!=4) 
            return;
         setDataOffset(DOS->Seek(getFile(), 0, OFFSET_CURRENT));
         setDataSize(L2BE(buff));
         DOS->Seek(getFile(), -4, OFFSET_CURRENT);

      }

      if (DOS->Read(getFile(), &buff, 4)!=4) 
         return;
      buff = L2BE(buff);
      DOS->Seek(getFile(), buff, OFFSET_CURRENT);
   }

   return;
}

bool                 rAIFFAudio::checkFile(const char* sFileName, EDtError &rc)
{
   unsigned int    buff;
   int             c=0;
   BPTR            fh;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(sFileName), MODE_OLDFILE);
   if (0 == fh)
      return false;
        
   /*
    * open file at its beginning
    */
   DOS->Seek(fh, 0, OFFSET_BEGINNING);
   if (DOS->Read(fh, &buff, 4)!=4) 
   {
      rc = DT_FileMalformed;
      DOS->Close(fh);
      return false;
   }

   /*
    * check if we have a FORM header
    */
   buff = L2BE(buff);
   if (buff != ID_FORM) 
   {
      rc = DT_InvalidFormat;
      DOS->Close(fh);
      return false;
   }

   /*
    * if so - see if it is an AIFF file
    */
   DOS->Seek(fh, 4, OFFSET_CURRENT);
   if (DOS->Read(fh, &buff, 4)!=4) 
   {
      rc = DT_FileMalformed;
      DOS->Close(fh);
      return false;
   }
   
   buff = L2BE(buff);
   if (buff != ID_AIFF) 
   {
      rc = DT_InvalidFormat;
      DOS->Close(fh);
      return false;
   }

   /*
    * alright! read what you can.
    */
   for (c=0;c<2;) 
   {
      if (DOS->Read(fh, &buff, 4)!=4) 
      {
         DOS->Close(fh);
         return false;
      }

      buff = L2BE(buff);

      if (buff == ID_COMM) 
      {
         c++;
      }
      else if (buff == ID_SSND) 
      {
         c++;
      }

      if (DOS->Read(fh, &buff, 4)!=4) 
      {
         rc = DT_FileMalformed;
         DOS->Close(fh);
         return false;
      }

      buff = L2BE(buff);
      DOS->Seek(fh, buff, OFFSET_CURRENT);
   }

   rc = DT_OK;
   DOS->Close(fh);
   return true;
}

const char          *rAIFFAudio::static_getName()
{
   return "AIFF Audio Track";
}

bool                 rAIFFAudio::static_isAudio()
{
   return true;
}

bool                 rAIFFAudio::static_isData()
{
   return false;
}

bool                 rAIFFAudio::static_isSession()
{
   return false;
}

bool                 rAIFFAudio::isAudio()
{
   return static_isAudio();
}

bool                 rAIFFAudio::isData()
{
   return static_isData();
}

const char          *rAIFFAudio::getName()
{
   return static_getName();
}

// vim: ts=3 et
