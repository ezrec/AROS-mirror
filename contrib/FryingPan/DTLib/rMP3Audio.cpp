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

#include "rMP3Audio.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <LibC/LibC.h>

#define FLIP16(a) ((((a)&0xff00)>>8) | (((a)&0xff)<<8))
#define MKID(a,b,c,d) (((a&255)<<24)|((b&255)<<16)|((c&255)<<8)|(d&255))
#define ID_ID3 MKID('I','D','3','\000')

   bool rMP3Audio::sNoMpegaReported = false;

IFileReader         *rMP3Audio::openRead(const char* sFile, EDtError &rc)
{
   rMP3Audio *pSkel = 0;
   if (true == checkFile(sFile, rc))
   {
      pSkel = new rMP3Audio(sFile, rc);
   }
   return pSkel;
}

rMP3Audio::rMP3Audio(const char *sName, EDtError &rc)
   : FileReader(sName, rc)
{
   if (rc != DT_OK)
      return;

   // set pStream to null == we do not have anything yet
   pStream = 0;
   handle  = 0;
   hAccess.Initialize(this, &rMP3Audio::access);

   for (int i=0; i<MPEGA_MAX_CHANNELS; i++)
   {
      pPCM[i] = new uint16[MPEGA_PCM_SIZE];
   }

   pMPEGA = MPEGAIFace::GetInstance(MPEGA_VERSION);
   _D(Lvl_Info, "%s: MPEGA Interface at %lx", 
         (int)__PRETTY_FUNCTION__,
         (int)pMPEGA);

   if (pMPEGA == NULL)
   {
      rc = DT_RequiredResourceNotAvailable;
      return;
   }
 
   // initialize mpega_ctrl
   fillControl(&hControl);

   setBlockSize(2352);
   setLittleEndian(false);
   setType(Data_Audio);
   setDataSize(0);

   handle = DOS->Open((char*)getFileName(), MODE_OLDFILE);

   readHeaders(rc);
}

rMP3Audio::~rMP3Audio()
{
   for (int i=0; i<MPEGA_MAX_CHANNELS; i++)
   {
      delete [] pPCM[i];
   }

   if (0 != handle)
      DOS->Close(handle);

   if (NULL != pMPEGA)
   {
      pStream = 0;
      pMPEGA->FreeInstance();
      _D(Lvl_Info, "%s: MPEGA Interface disposed", (int)__PRETTY_FUNCTION__);
      pMPEGA = 0;
   }
}

void                 rMP3Audio::readHeaders(EDtError &rc)
{
   MPEGA_STREAM *pStream;

   /*
    * FIRST OF ALL determine the correct beginning and end of stream
    * since MPEGA is freaking unable to do that.
    */
   {
      uint8 x[256];
      uint32 sz;

      stream_size = 0;     // we will modify this value slightly...
      
      // 1. check for tag at the end
      DOS->Seek(handle, -128, OFFSET_END);
      DOS->Read(handle, &x, 128);
      if ((x[0]== 'T') && (x[1] == 'A') && (x[2] == 'G'))
      {
         stream_size -= 128;
         _D(Lvl_Info, "%s: found ID3v1 tag. Structure size: 128 bytes", (uint)__PRETTY_FUNCTION__);
      }
      else if ((x[118] == '3') && (x[119] == 'D') && (x[120] == 'I'))
      {
         stream_size -= 10;
         _D(Lvl_Info, "%s: found ID3v2.%ld.%ld footer. Structure size: 10 bytes", (uint)__PRETTY_FUNCTION__, x[121], x[122]);
      }

      // 2. get file length (now we can)
      sz = DOS->Seek(handle, 0, OFFSET_BEGINNING);
      stream_size += sz;

      // 3. check for id3v2
      DOS->Read(handle, &x, 10);
      if ((x[0] == 'I') && (x[1] == 'D') && (x[2] == '3'))
      {
         sz = (x[6]<<21) | (x[7]<<14) | (x[8]<<7) | (x[9]);
         stream_start = sz+10;
         stream_size -= stream_start;
         _D(Lvl_Info, "%s: found ID3v2.%ld.%ld tag. Structure size: %ld bytes", (uint)__PRETTY_FUNCTION__, x[3], x[4], sz);
         
      }

      // 4. set remaining fields
      current_location = 0;
   }

   pStream = pMPEGA->MPEGA_open(getFileName(), &hControl);
   ASSERTS(NULL != pStream, "Verify routine passed stream that does not open!\nPlease do NOT use this track.");
   if (NULL == pStream)
      return;
      
   _D(Lvl_Info, "%s: stream %08lx opened, len: %ld ms br: %ld, fr: %ld, ch: %ld", 
         (int)__PRETTY_FUNCTION__,
         (int)pStream, 
         pStream->ms_duration, 
         pStream->bitrate, 
         pStream->frequency, 
         pStream->channels);

   unsigned long lLen = 0;

   pMPEGA->MPEGA_seek(pStream, pStream->ms_duration);
   pMPEGA->MPEGA_time(pStream, &lLen);
   pMPEGA->MPEGA_seek(pStream, 0);
      
   _D(Lvl_Info, "%s: MPEGA claims this file is %ldms long. This file turns out to be %ldms", 
         (int)__PRETTY_FUNCTION__,
         pStream->ms_duration, 
         lLen);

   //lLen = pStream->ms_duration;
   lLen *= 1764;  // we need 44100Hz, *2 (stereo), *2 (16bit), that is 176400 bytes per second
   lLen /= 10;
   lLen /= getBlockSize(); //
   lLen *= getBlockSize(); // round down to nearest multiplicity of block size

   setDataSize(lLen);

   pMPEGA->MPEGA_close(pStream);

   return;
}

bool                 rMP3Audio::checkFile(const char* sFileName, EDtError &rc)
{
   Library        *pBase;
   bool            bRes;
   BPTR            fh;

   // 1. verify we have the library.
//   DPrintfA("Accessing mpega...\n");

   rc = DT_RequiredResourceNotAvailable;

   pBase = Exec->OpenLibrary("mpega.library", MPEGA_VERSION);

   if ((false == sNoMpegaReported) && (NULL == pBase))
   {
      request("Information", 
         "MPEGA library seems to be missing.\nAs a result, your MP3 files will not work.\n\nThis message will not pop up any more.",
         "Ok", 0);

      sNoMpegaReported = true;
      return false;
   }

   ASSERT(pBase != NULL);
   Exec->CloseLibrary(pBase);
   pBase = 0;

   // 2. verify the file is EXACTLY what we are looking for.
//   DPrintfA("accessing file...\n");

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(sFileName), MODE_OLDFILE);
   if (0 == fh)
      return false;

//   DPrintfA("checking file...\n");
   bRes = checkMP3(fh, rc);
   DOS->Close(fh);
   return bRes;
}

void                 rMP3Audio::dispose()
{
   delete this;
}

const char          *rMP3Audio::static_getName()
{
   return "MP3 Audio Track";
}

bool                 rMP3Audio::static_isAudio()
{
   return true;
}

bool                 rMP3Audio::static_isSession()
{
   return false;
}

bool                 rMP3Audio::static_isData()
{
   return false;
}

bool                 rMP3Audio::isAudio()
{
   return static_isAudio();
}

bool                 rMP3Audio::isData()
{
   return static_isData();
}

const char          *rMP3Audio::getName()
{
   return static_getName();
}

void                 rMP3Audio::fillControl(MPEGA_CTRL *pCtrl)
{
   pCtrl->bs_access                  = const_cast<Hook*>(hAccess.GetHook());
   pCtrl->check_mpeg                 = false;
   pCtrl->stream_buffer_size         = 0;
   pCtrl->layer_3.force_mono         = false;
   pCtrl->layer_3.stereo.freq_div    = 1;
   pCtrl->layer_3.stereo.freq_max    = 44100;
   pCtrl->layer_3.stereo.quality     = 2;
   pCtrl->layer_3.mono.freq_div      = 1;
   pCtrl->layer_3.mono.freq_max      = 44100;
   pCtrl->layer_3.mono.quality       = 2;
   pCtrl->layer_1_2.force_mono       = false;
   pCtrl->layer_1_2.stereo.freq_div  = 1;
   pCtrl->layer_1_2.stereo.freq_max  = 44100;
   pCtrl->layer_1_2.stereo.quality   = 2;
   pCtrl->layer_1_2.mono.freq_div    = 1;
   pCtrl->layer_1_2.mono.freq_max    = 44100;
   pCtrl->layer_1_2.mono.quality     = 2;
}

bool                 rMP3Audio::checkMP3(BPTR file, EDtError &rc)
{
   unsigned char *buf       = 0;
   int            len;
   bool           res       = false;
   int            offset    = 0;
   int            sync      = 0;
   int            framelen  = 0;

   rc = DT_OutOfMemory;
   buf = new unsigned char[131072];
   if (NULL == buf)
      return false;

   len = DOS->Read(file, buf, 131072);
   if (len != 131072)
   {
      delete [] buf;
      rc = DT_FileMalformed;
      return false;
   }

   // if we havent found anything within first 8k, there is no valid stream.
   // we search only first 8kB for starting frame, but
   // then we look up the whole 128kB for frame headers.

   rc = DT_OK;

   while (offset < 8192)
   {
      framelen = verifyFrame(&buf[offset], rc);
      rc = DT_OK;

      if (framelen > 0)
      {
         sync = offset;
         while (sync < 131000)         // do not increase!
         {
            framelen = verifyFrame(&buf[sync], rc);
            sync += framelen;
            if (framelen == 0) 
               break;
            if (rc != DT_OK)
               break;
         }

         if (rc != DT_OK)
            break;

         if (framelen == 0)
         {
            sync = 0;
            offset++;
         }
         else
         {
            res = true;
            break;
         }
      }
      else
      {
         offset++;
      }
   }
   delete [] buf;
   DOS->Seek(file, 0, OFFSET_BEGINNING);
   return res;
}

long                 rMP3Audio::verifyFrame(unsigned char* frame, EDtError &rc)
{
   int bitrate = 0;
   int pad     = 0;

   if (frame[0] != 0xff)            // check sync
   {
      rc = DT_OK;
      return 0;
   }
   if ((frame[1] & 0xfa) != 0xfa)   // check valid type (mpeg 1 layer 3) 
   {
      rc = DT_FileFormatNotSupported;
      return 0;
   }
   if ((frame[2] & 0xf0) == 0xf0)   // check valid bit rate
   {
      return 0;
   }
   if ((frame[2] & 0xc) != 0x0)     // check valid freq
   {
      rc = DT_WrongFrequency;
      return 0;
   }
   if ((frame[3] & 0xc0) == 0xc0)   // check valid mode (stereo) 
   {
      rc = DT_WrongChannelCount;
      return 0;
   }

   switch ((frame[2]&0xf0) >> 4)
   {
      case 1:     bitrate = 32;     break;
      case 2:     bitrate = 40;     break;
      case 3:     bitrate = 48;     break;
      case 4:     bitrate = 56;     break;
      case 5:     bitrate = 64;     break;
      case 6:     bitrate = 80;     break;
      case 7:     bitrate = 96;     break;
      case 8:     bitrate = 112;    break;
      case 9:     bitrate = 128;    break;
      case 10:    bitrate = 160;    break;
      case 11:    bitrate = 192;    break;
      case 12:    bitrate = 224;    break;
      case 13:    bitrate = 256;    break;
      case 14:    bitrate = 320;    break;
   }
   pad      = (frame[2]&02) ? 1 : 0;
   
   //DPrintfA("I think I found sync. Frame len: %ld bytes\n", ((1440*bitrate)/441)+pad);
   
   return ((1440*bitrate)/441)+pad; 
}

bool                 rMP3Audio::setUp()
{
   int err;

   pStream = pMPEGA->MPEGA_open(getFileName(), &hControl);

   ASSERT(NULL != pStream);
   if (pStream == NULL)
      return false;

   lPCMOffset = 0;
   lSamples   = 0;
   lErrors    = 0;

   err = pMPEGA->MPEGA_seek(pStream, 1);
   (void)err; // Unused if not debugging
   _D(Lvl_Info, "%s: Initialized MPEGA stream (%08lx): error code %ld",
         (int)__PRETTY_FUNCTION__,
         (int)pStream,
         err);
   return true;
}

void                  rMP3Audio::cleanUp()
{
   if (pStream != 0)
      pMPEGA->MPEGA_close(pStream);
   pStream = 0;
   _D(Lvl_Info, "%s: MPEGA stream closed.",
         (int)__PRETTY_FUNCTION__);
}

bool                  rMP3Audio::readData(const IOptItem* item, void* buff, int lBlocks)
{
   uint16 *pBuff = (uint16*)buff;
   uint32  lLen = lBlocks * getBlockSize();
   
   ASSERT(NULL != pStream);
   if (pStream == NULL)
      return false;

   while (lLen > 0)
   {
      if (lPCMOffset >= lSamples)
         lPCMOffset = 0;
      if (0 == lPCMOffset)
      {
         // if anything fails, do not return garbage, ok?
         lSamples = pMPEGA->MPEGA_decode_frame(pStream, pPCM);
         _D(Lvl_Info, "%s: Decoding MPEGA frames (stream %lx, buffer %lx): %ld",
               (IPTR)__PRETTY_FUNCTION__,
               (int)pStream,
               (int)pPCM,
               lSamples);
         if (lSamples < 0)
         {
            if (lSamples != MPEGA_ERR_EOF)
            {
               lErrors++;
               if (lErrors == 64)
               {
                  request("Info", "Its been 64 'faulty' frames now and most likely it is going to increase\nI doubt this is the problem with MP3 file (but who knows?)\nIf problem persists, mail the brilliant author of your mpega.library\nI keep getting %ld.", "Proceed", ARRAY((IPTR)lSamples));
               }
            }
            lSamples = MPEGA_PCM_SIZE;
            memset(pPCM[0], 0, MPEGA_PCM_SIZE);
            memset(pPCM[1], 0, MPEGA_PCM_SIZE);

            // do nothing. things will not get copied anyways
         }
      }
      // clone data to target buffer. remember to reverse byte order.
      while (lPCMOffset<lSamples)
      {
         // clone reversed samples.
         *pBuff = FLIP16(pPCM[0][lPCMOffset]);
         pBuff++;
         *pBuff = FLIP16(pPCM[1][lPCMOffset]);
         pBuff++;
         lLen -= 4;
         lPCMOffset++;
         // this loop will never begin with lLen = 0. see outer while loop.
         if (lLen < 4)
            break;
      }
   }

   return true;
}
   
uint                  rMP3Audio::access(void* , MPEGA_ACCESS *acc)
{
   register uint32 d;

   switch (acc->func)
   {
      case MPEGA_BSFUNC_OPEN:
         /*
          * since our stream is already open, we won't re-open it again.
          */
         acc->data.open.stream_size = stream_size;
         DOS->Seek(handle, stream_start, OFFSET_BEGINNING);
         current_location = 0;
         _D(Lvl_Info, "%s: MPEGA_OPEN - Opening virtual stream. Current location: %ld", (uint)__PRETTY_FUNCTION__, current_location);
         return (uint)this;

      case MPEGA_BSFUNC_CLOSE:
         /*
          * do nothing. we close everything on dispose()
          */
         _D(Lvl_Info, "%s: MPEGA_CLOSE - Disposing virtual stream.", (uint)__PRETTY_FUNCTION__);
         return 0;

      case MPEGA_BSFUNC_READ:
         /*
          * that is fairly easy. just read()
          */
         if (current_location >= stream_size)
         {
            _D(Lvl_Info, "%s: MPEGA_READ - no more data", (uint)__PRETTY_FUNCTION__);
            return 0;
         }

         d = DOS->Read(handle, acc->data.read.buffer, 
             (unsigned)acc->data.read.num_bytes < (stream_size - current_location) ? (unsigned)acc->data.read.num_bytes  : (stream_size - current_location)); 
         current_location += d;
         _D(Lvl_Info, "%s: MPEGA_READ - Read %ld bytes. Advancing position to %ld.", (uint)__PRETTY_FUNCTION__, d, current_location);
         return d;

      case MPEGA_BSFUNC_SEEK:
         current_location = ((unsigned)acc->data.seek.abs_byte_seek_pos < stream_size ? (unsigned)acc->data.seek.abs_byte_seek_pos : stream_size);
         _D(Lvl_Info, "%s: MPEGA_SEEK - Seeking to relative location %ld.", (uint)__PRETTY_FUNCTION__, current_location);
         DOS->Seek(handle, current_location + stream_start, OFFSET_BEGINNING);
         return 0;
   }
   
   _D(Lvl_Warning, "%s: MPEGA_xxx - Your MPEGA library just asked me to do something I never heard of (%ld)", (uint)__PRETTY_FUNCTION__, acc->func);
   ASSERTS(false, "Your mpega.library has just asked me\nto do something funny.");
   return 0;
}

