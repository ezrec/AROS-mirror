/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _AHI_H_
#define _AHI_H_

#include "HookT.h"
#include "HookAttrT.h"
#include <libclass/ahi.h>
#include <devices/ahi.h>
#include "VectorT.h"
#include "RWSyncT.h"
#include "Synchronizer.h"


namespace GenNS
{
   class Thread;
   class AHI 
   {
   public:
      struct AudioID
      {
         unsigned long  id;
         char     name[64];
      };

      enum AHISampleType
      {
         AHI_Mono8      =  AHIST_M8S,
         AHI_Stereo8    =  AHIST_S8S,
         AHI_Mono16     =  AHIST_M16S,
         AHI_Stereo16   =  AHIST_S16S,
         AHI_Mono32     =  AHIST_M32S,
         AHI_Stereo32   =  AHIST_S32S
      };


   private:
      // the following lines define the message interface

      enum AHICommand
      {
         cmd_NoOp       =  1000, // for initialization :]
         cmd_SetAudio,           // AudioID*
         cmd_FreeAudio,
         cmd_CreateStaticSample,
         cmd_CreateDynamicSample,
         cmd_FreeSample,
         cmd_PlaySample,
         cmd_SoundLoop,          // SoundInfo
         cmd_SoundStop           // SoundInfo
      };

      struct ahi_CreateDynamicSample
      {
         AHISampleType     type;
         int               freq;
         const Hook       *func;
         int               buflen;
      };

      struct ahi_CreateStaticSample
      {
         AHISampleType     type;
         int               freq;
         void             *buffer;
         int               buflen;
      };

      struct ahi_FreeSample
      {
         short             sample;
      };

      struct ahi_PlaySample
      {
         short             sample;
      };

      struct ahi_SoundInfo
      {
         short    channel;

         ahi_SoundInfo(short ch) : channel(ch) 
         { 
         }
      };

      struct ExtAHISampleInfo : public AHISampleInfo
      {
         unsigned char     id;
         unsigned char     bps;
         long              freq;
         long              maxlen;
         bool              dynamic; 
         ExtAHISampleInfo *fndsample;
         HookAttrT<void*, uint> hData;
         Thread           *update;
      };

   protected:
      HookT<AHI, Thread*, void*>                   hHkProc;
      HookT<AHI, AHICommand, void*>                hHkCmd;
      HookT<AHI, AHIAudioCtrl*, AHISoundMessage*>  hHkSound;
      HookT<AHI, long, ExtAHISampleInfo*>          hHkMusic;

      Thread                          *pThread;
      AHIIFace                        *pAHI;
      VectorT<AudioID*>                hModes;
      short                            numChannels;

      RWSyncT<AHIAudioCtrl*>           pAudio;

      Synchronizer                     smpSync;

      ExtAHISampleInfo                *smpSample[256];
      ExtAHISampleInfo                *smpChan[32];
      Task                            *smpWaits[32];     // we will send SIGB_ABORT when it is ready :)
      unsigned char                    smpSignals[32];


      AudioID                         *pCurrentMode;
   protected:
      static bool                      freeAudioID(AudioID* const &id);
      virtual unsigned long            subProcess(Thread*, void*);
      virtual unsigned long            procCommand(AHICommand, void*);   
      virtual unsigned long            procSoundMessage(AHIAudioCtrl*, AHISoundMessage*);
      virtual unsigned long            procMusicMessage(long cmd, ExtAHISampleInfo* pExt);

      virtual bool                     do_SetAudio(AudioID*);
      virtual void                     do_FreeAudio();
      virtual unsigned short           do_CreateDynamicSample(ahi_CreateDynamicSample*);
      virtual unsigned short           do_CreateStaticSample(ahi_CreateStaticSample*);
      virtual void                     do_FreeSample(short);
      virtual bool                     do_PlaySample(ahi_PlaySample*);
      virtual void                     do_SoundStop(ahi_SoundInfo*);
      virtual void                     do_SoundLoop(ahi_SoundInfo*);
      ExtAHISampleInfo                *do_AllocSample(bool dynamic, AHISampleType type, void* buffer, int buflen);
   public:
      AHI(short channels);
      virtual                         ~AHI();

      virtual unsigned long            GetModeCount();
      virtual const AudioID           *GetMode(unsigned long index);
      virtual const AudioID           *FindMode(unsigned long id);

      virtual bool                     SetAudioMode(const AudioID *id);
      virtual void                     FreeAudio();
      virtual AudioID                 *GetAudioMode();
      virtual unsigned short           CreateDynamicSample(AHISampleType type, int freq, const Hook* func, int bufflen);
      virtual unsigned short           CreateStaticSample(AHISampleType type, int freq, void* buffer, int length);
      virtual void                     FreeSample(unsigned short);
      virtual bool                     PlaySample(unsigned short);
      virtual void                     WaitSample(unsigned short);
   };
};

#endif

