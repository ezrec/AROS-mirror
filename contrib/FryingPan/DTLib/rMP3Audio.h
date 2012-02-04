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

#ifndef _R_MP3AUDIO_H_ 
#define _R_MP3AUDIO_H_ 


#include "FileReader.h"
#include <Generic/DynList.h>
#include <Generic/HookT.h>


#include <libclass/mpega.h>
#include <libraries/mpega.h>

/*
 * can't say i like that - this is just freaking nuts
 * mp3 files that come with id3v2 tags are read with noise at the beginning (just use seek())
 * mp3 files that come with id3v1 tags will come with some screwy noise too (seek again)
 * anyone knows how the hell this seek should work or what!
 */

class rMP3Audio : public FileReader
{ 
protected:
   MPEGAIFace                   *pMPEGA;
   MPEGA_CTRL                    hControl;
   MPEGA_STREAM                 *pStream;
   uint16                       *pPCM[MPEGA_MAX_CHANNELS];
   short                         lPCMOffset;
   short                         lSamples;
   unsigned short                lErrors;
   BPTR                          handle;
   uint32                        stream_start;
   uint32                        stream_size;
   uint32                        current_location;

protected:
   static bool                   sNoMpegaReported;
   HookT<rMP3Audio, void*, MPEGA_ACCESS*> hAccess;

protected:
   virtual void                  readHeaders(EDtError &rc); 
   virtual void                  fillControl(MPEGA_CTRL *pCtrl);
                                 rMP3Audio(const char* sFileName, EDtError &rc);
   virtual                      ~rMP3Audio();
   static bool                   checkMP3(BPTR fh, EDtError &rc);
   static long                   verifyFrame(unsigned char* pFrame, EDtError &rc);

   uint                          access(void* handle, MPEGA_ACCESS *acc);
public:
   static IFileReader           *openRead(const char* sFile, EDtError &rc);
   static bool                   checkFile(const char* sFileName, EDtError &rc);
   virtual void                  dispose();
  
   virtual bool                  setUp();
   virtual void                  cleanUp();
   virtual bool                  readData(const IOptItem* item, void* pBuff, int len);

   virtual const char           *getName();
   virtual bool                  isAudio();
   virtual bool                  isData();

   static bool                   static_isAudio();
   static bool                   static_isData();
   static bool                   static_isSession();
   static const char            *static_getName();
};

#endif /*_MP3AUDIO_H_*/
