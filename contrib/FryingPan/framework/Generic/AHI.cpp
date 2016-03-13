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

#include "LibrarySpool.h"
#include "AHI.h"
#include "Thread.h"
#include <exec/io.h>
#include <libclass/exec.h>
#include <exec/ports.h>
#include <devices/ahi.h>
#include <dos/dos.h>
#include <LibC/LibC.h>

using namespace GenNS;

AHI::AHI(short channels)
{
   pCurrentMode = 0;
   pAudio.Assign(0);

   numChannels = channels;
   if (numChannels > 32)
      numChannels = 32;

   for (int i=0; i<256; i++)
   {
      smpSample[i]   = 0;
   }

   for (int i=0; i<32; i++)
   {
      smpChan[i]  = 0;
      smpWaits[i] = 0;
   }

   hHkProc.Initialize(this, &AHI::subProcess);
   hHkCmd.Initialize(this, &AHI::procCommand);
   hHkSound.Initialize(this, &AHI::procSoundMessage);
   hHkMusic.Initialize(this, &AHI::procMusicMessage);

   pThread = new Thread("AHI Process", hHkProc.GetHook(), NULL);
   pThread->SetHandler(hHkCmd.GetHook());
   // ensure everything is up and running
   pThread->DoSync(cmd_NoOp, 0);
}

AHI::~AHI()
{
   if (NULL != pThread)
   {
      delete pThread;
      pThread = 0;
   }
   
   hModes.ForEach(&freeAudioID);
}
   
bool AHI::freeAudioID(AudioID* const &id)
{
   delete id;
   return true;
}

unsigned long AHI::GetModeCount()
{
   return hModes.Count();
}

const AHI::AudioID *AHI::GetMode(unsigned long id)
{
   return hModes[id];
}

bool AHI::SetAudioMode(const AHI::AudioID *id)
{
   return (bool)pThread->DoSync(cmd_SetAudio, const_cast<AudioID*>(id));
}

AHI::AudioID *AHI::GetAudioMode()
{
   return pCurrentMode;
}

void AHI::FreeAudio()
{
   pThread->DoAsync(cmd_FreeAudio, 0);
}

unsigned short AHI::CreateDynamicSample(AHISampleType type, int freq, const Hook* func, int bufflen)
{
   ahi_CreateDynamicSample dyn = { type, freq, func, bufflen };
   return pThread->DoSync(cmd_CreateDynamicSample, &dyn);
}

unsigned short AHI::CreateStaticSample(AHISampleType type, int freq, void* buffer, int length)
{
   ahi_CreateStaticSample stat = { type, freq, buffer, length };
   return pThread->DoSync(cmd_CreateStaticSample, &stat);
}

void AHI::FreeSample(unsigned short idx)
{
   short sample = (short)idx;
   ahi_FreeSample fre = { sample };
   pThread->DoSync(cmd_FreeSample, &fre);
}

bool AHI::PlaySample(unsigned short idx)
{
   short sample = (short)idx;
   ahi_PlaySample fre = { sample };
   return pThread->DoSync(cmd_PlaySample, &fre);
}

const AHI::AudioID *AHI::FindMode(unsigned long id)
{
   unsigned long idx;
   for (idx=0; idx<GetModeCount(); idx++)
   {
      if (GetMode(idx)->id == id)
         return GetMode(idx);
   }
   return 0;
}

unsigned long AHI::subProcess(Thread *pThis, void*)
{
   pAHI = AHIIFace::GetInstance();

   if (pAHI != 0)
   {
      for (unsigned long   i  =  pAHI->AHI_NextAudioID(AHI_INVALID_ID); 
                           i !=  AHI_INVALID_ID;
                           i  =  pAHI->AHI_NextAudioID(i))
      {
         AudioID *pID = new AudioID;

         pAHI->AHI_GetAudioAttrsA(i, 0, (TagItem*)ARRAY(
                  AHIDB_Name,       (IPTR)&pID->name, 
                  AHIDB_BufferLen,  63,
                  0,                0));

         pID->id     = i;
         hModes << pID;
      }
   }


   pThis->HandleSignals(0xffffffff);

   do_FreeAudio();

   pAHI->FreeInstance();
   return 0;
}

unsigned long AHI::procCommand(AHICommand aCmd, void* pData)
{
   switch (aCmd)
   {
      case cmd_NoOp:
         return true;

      case cmd_SetAudio:
         return do_SetAudio((AudioID*)pData);

      case cmd_FreeAudio:
         do_FreeAudio();
         break;

      case cmd_CreateDynamicSample:
         return do_CreateDynamicSample((ahi_CreateDynamicSample*)pData);

      case cmd_CreateStaticSample:
         return do_CreateStaticSample((ahi_CreateStaticSample*)pData);

      case cmd_FreeSample:
         do_FreeSample(((ahi_FreeSample*)pData)->sample);
         break;

      case cmd_PlaySample:
         return do_PlaySample((ahi_PlaySample*)pData);

      case cmd_SoundStop:
         do_SoundStop((ahi_SoundInfo*)pData);
         break;

      case cmd_SoundLoop:
         do_SoundLoop((ahi_SoundInfo*)pData);
         break;
   };
   return 0;
}

unsigned long AHI::procSoundMessage(AHIAudioCtrl* pCtl, AHISoundMessage*msg)
{
   ExtAHISampleInfo *pExt = smpChan[msg->ahism_Channel];

   // if this is not true we started nosound
   if (0 != pExt)
   {
      if (pExt->dynamic == true)
      {
         if (pExt->fndsample->ahisi_Length)
         {
            pThread->DoAsync(cmd_SoundLoop, new ahi_SoundInfo(msg->ahism_Channel));
         }
         else
         {
            pThread->DoAsync(cmd_SoundStop, new ahi_SoundInfo(msg->ahism_Channel));
         }
      }
      else
      {
         pThread->DoAsync(cmd_SoundStop, new ahi_SoundInfo(msg->ahism_Channel));
      }
   }
   return 0;
}
   
unsigned long AHI::procMusicMessage(long cmd, ExtAHISampleInfo* pExt)
{
   switch (cmd)
   {
      case cmd_SoundLoop:
         pExt->ahisi_Length = pExt->hData(pExt->ahisi_Address, pExt->maxlen);// / pExt->bps;
         //Generic::MessageBox("Info", "Passing data\n%ld bytes requested,\n%ld bytes received.", "Ok", ARRAY(pExt->maxlen, pExt->ahisi_Length));
         break;

   }
   return 0;
}

bool AHI::do_SetAudio(AudioID* newID)
{
   AHIAudioCtrl *pCtl = 0;
   int           err;

   if ((pCurrentMode !=0) && (pCurrentMode->id == newID->id))
      return true;
   do_FreeAudio();
   if (0 == pAHI)
      return false;
   if (0 == newID)
      return true;

   pCtl = pAHI->AHI_AllocAudioA((TagItem*)ARRAY(
      AHIA_AudioID,        newID->id,
      AHIA_MixFreq,        44100,
      AHIA_Channels,       static_cast<IPTR>(numChannels),
      AHIA_Sounds,         256,
      AHIA_SoundFunc,      (IPTR)hHkSound.GetHook(),
      TAG_DONE,            0
   ));

   if (0 != pCtl)
   {
      pAudio.Assign(pCtl);
      pCurrentMode = newID;
      err = pAHI->AHI_ControlAudioA(pCtl, (TagItem*)ARRAY(AHIC_Play, true, 0, 0));
      if (0 != err)
      {
         request("Error", "Unable to start audio device.\nError code %ld", "Proceed", ARRAY(static_cast<IPTR>(err)));
      }
      return true;
   }
   return false;
}

void AHI::do_FreeAudio()
{
   AHIAudioCtrl     *pCtl = 0;

   if (pCurrentMode == 0)
      return;
   if (0 == pAHI)
      return;

   pCtl = pAudio.Assign(0);
   if (0 != pCtl)
   {
      for (int i=0; i<256; i++)
      {
         if (0 != smpSample[i])
         {
            do_FreeSample(i);
         }
      }
      pAHI->AHI_FreeAudio(pCtl);
   }
   pCurrentMode = 0;
}

unsigned short AHI::do_CreateDynamicSample(ahi_CreateDynamicSample *dat)
{
   ExtAHISampleInfo *pExt1 = do_AllocSample(true, dat->type, 0, dat->buflen);
   ExtAHISampleInfo *pExt2 = do_AllocSample(true, dat->type, 0, dat->buflen);

   if ((0 == pExt1) ||
       (0 == pExt2))
   {
      if (0 != pExt1)
         do_FreeSample(pExt1->id);
      if (0 != pExt2)
         do_FreeSample(pExt2->id);

      return AHI_NOSOUND;
   }

   pExt1->fndsample = pExt2;
   pExt2->fndsample = pExt1;

   pExt1->freq      = dat->freq;
   pExt2->freq      = dat->freq;

   pExt1->update    = new Thread("AHI Music");
   pExt2->update    = pExt1->update;

   pExt1->update->SetHandler(hHkMusic.GetHook());

   pExt1->hData     = dat->func;
   pExt2->hData     = dat->func;

   return pExt1->id;
}

unsigned short AHI::do_CreateStaticSample(ahi_CreateStaticSample *dat)
{
   ExtAHISampleInfo *pExt = do_AllocSample(false, dat->type, dat->buffer, dat->buflen);
   if (pExt != NULL)
   {
      pExt->freq     = dat->freq;
      return pExt->id;
   }
   return AHI_NOSOUND;
}

AHI::ExtAHISampleInfo *AHI::do_AllocSample(bool dynamic, AHISampleType type, void* buffer, int buflen)
{
   if (NULL == pAHI)
      return 0;

   smpSync.Acquire();

   ExtAHISampleInfo *pInfo = 0;
   int               i;
   AHIAudioCtrl     *pCtl  = 0;

   for (i=0; i<256; i++)
   {
      if (smpSample[i] == 0)
         break;
   }

   if (i < 256)
   {
      pInfo = new ExtAHISampleInfo;
      
      if (dynamic)
      {
         buffer = new char[buflen];
      }

      switch (type)
      {
         case AHI_Mono8:
            pInfo->bps  =  1;
            break;
         case AHI_Mono16:
            pInfo->bps  =  2;
            break;
         case AHI_Mono32:
            pInfo->bps  =  4;
            break;
         case AHI_Stereo8:
            pInfo->bps  =  2;
            break;
         case AHI_Stereo16:
            pInfo->bps  =  4;
            break;
         case AHI_Stereo32:
            pInfo->bps  =  8;
            break;
         default:
            pInfo->bps  =  1;
      }
      pInfo->ahisi_Address    = buffer;
      pInfo->ahisi_Length     = buflen / pInfo->bps;
      pInfo->ahisi_Type       = type;
      pInfo->id               = i;
      pInfo->freq             = 0;
      pInfo->dynamic          = dynamic;
      pInfo->fndsample        = 0;
      pInfo->update           = 0;
      pInfo->maxlen           = buflen;

      pCtl = pAudio.ObtainRead();
      if (NULL != pCtl)
      {
         pAHI->AHI_LoadSound(i, (dynamic ? AHIST_DYNAMICSAMPLE : AHIST_SAMPLE), pInfo, pCtl);
      }
      pAudio.Release();

      smpSample[i] = pInfo;
   }

   smpSync.Release();

   return pInfo;
}

void AHI::do_FreeSample(short idx)
{
   if (idx >= 256)
      return;

   if (0 == pAHI)
      return;

   smpSync.Acquire();
   if (0 != smpSample[idx])
   {
      ExtAHISampleInfo *pExt1 = smpSample[idx];
      ExtAHISampleInfo *pExt2 = pExt1->fndsample;
      
      // 1 CLEAR SAMPLE AND ITS FRIEND
      smpSample[idx] = 0;
      if (NULL != pExt2)
         smpSample[pExt2->id] = 0;

      delete pExt1->update;
      pExt1->update = 0;
      pExt2->update = 0;

      // 2 UNLOAD SAMPLE AND ITS FRIEND
      AHIAudioCtrl *pCtl = pAudio.ObtainRead();
      if (pCtl)
      {
         pAHI->AHI_UnloadSound(idx, pCtl);
         if (NULL != pExt2)
            pAHI->AHI_UnloadSound(pExt2->id, pCtl);
      }
      pAudio.Release();

      // 3 FREE MEMORY BUFFER FOR SAMPLE AND ITS FRIEND
      if (pExt1->dynamic == true)
         delete [] ((char*)pExt1->ahisi_Address);
      if ((NULL != pExt2) &&
          (pExt2->dynamic == true))
         delete [] ((char*)pExt2->ahisi_Address);

      // 4 FREE SAMPLE AND ITS FRIEND
      delete pExt1;
      if (NULL != pExt2)
         delete pExt2;
   }
   smpSync.Release();
}

bool AHI::do_PlaySample(ahi_PlaySample *dat)
{
   int            idx = dat->sample;
   AHIAudioCtrl  *pCtl;
   bool           res = false;
   int            err;
   int            chan;

   if (idx > 255)
      return false;
   if (idx < 0)
      return false;

   if (0 == smpSample[idx])
      return false;

   pCtl = pAudio.ObtainRead();
   if (pCtl != 0)
   {
      for (chan=0; chan<numChannels; chan++)
      {
         if (smpChan[chan] == 0)
            break;
      }
      
      if (chan != numChannels)
      {
         smpChan[chan] = smpSample[idx];
   
         if (smpSample[idx]->dynamic != false)
         {
            memset(smpSample[idx]->ahisi_Address, 0, smpSample[idx]->maxlen);
            memset(smpSample[idx]->fndsample->ahisi_Address, 0, smpSample[idx]->maxlen);
         }

         err = pAHI->AHI_PlayA(pCtl, (TagItem*)ARRAY(
                  AHIP_BeginChannel,   static_cast<IPTR>(chan),
                  AHIP_Freq,           static_cast<IPTR>(smpSample[idx]->freq),
                  AHIP_Vol,            0x10000,
                  AHIP_Pan,            0x8000,
                  AHIP_Sound,          static_cast<IPTR>(idx),
                  AHIP_EndChannel,     0,
                  TAG_DONE,            0));

         if (err != 0)
         {
            smpChan[chan] = 0;
            res = false;
         }
         else
         {
            res = true;
         }
      }
   }
   pAudio.Release();

   return res;
}

void AHI::do_SoundStop(ahi_SoundInfo* dat)
{
   int            chan = dat->channel;
   AHIAudioCtrl  *pCtl;

   delete dat;

   if (chan > numChannels)
      return;
   if (chan < 0)
      return;

   smpSync.Acquire();
   pCtl = pAudio.ObtainRead();

   if (pCtl != NULL)
   { 
      smpChan[chan] = 0;
      pAHI->AHI_PlayA(pCtl, (TagItem*)ARRAY(
               AHIP_BeginChannel,   static_cast<IPTR>(chan),
               AHIP_Sound,          AHI_NOSOUND,
               AHIP_Vol,            65536,
               AHIP_Pan,            32768,
               AHIP_LoopSound,      0,
               AHIP_EndChannel,     0,
               TAG_DONE,            0));
      pAHI->AHI_SetSound(chan, AHI_NOSOUND, 0, 0, pCtl, 0);

      if (smpWaits[chan] != NULL)
      {
         Exec->Signal(smpWaits[chan], 1 << (smpSignals[chan]));
         smpWaits[chan] = 0;
      }
   }
   pAudio.Release();
   smpSync.Release();
}

void AHI::WaitSample(unsigned short sample)
{
   bool bWait = false;
   int sig = Exec->AllocSignal(-1);

   smpSync.Acquire();

   for (int i=0; i<numChannels; i++)
   {
      if ((smpChan[i] != 0) &&
          (smpChan[i]->id == sample))
      {
         smpWaits[i] = Exec->FindTask(0);
         smpSignals[i] = sig;
         bWait = true;
      }
   }

   smpSync.Release();

   if (true == bWait)
   {
      Exec->Wait(1 << sig);
   }
   Exec->FreeSignal(sig);
}

void AHI::do_SoundLoop(ahi_SoundInfo* info)
{
   ExtAHISampleInfo *pExt = smpChan[info->channel];
   AHIAudioCtrl     *pCtl = pAudio.ObtainRead();

   smpChan[info->channel] = pExt->fndsample;
   pExt->update->DoAsync(cmd_SoundLoop, pExt->fndsample);
   pAHI->AHI_SetSound(info->channel, pExt->fndsample->id, 0, 0, pCtl, 0);
   pAudio.Release();
   return;
}


