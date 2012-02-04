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

#ifndef _AHIIFACE_H_ 
#define _AHIIFACE_H_ 

#include <libclass/exec.h>

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS

#include <devices/ahi.h>
#include <exec/io.h>
#include <exec/ports.h>

   BEGINSHORTDECL(AHI)

      static AHIIFace  *GetInstance()
      {
         MsgPort    *pPort    = 0;
         AHIRequest *pRequest = 0;
         Library    *LibBase  = 0;
         AHIIFace   *pFace    = 0;

         pPort = Exec->CreateMsgPort();
         if (pPort != NULL)
         {
            pRequest = (AHIRequest*)Exec->CreateIORequest(pPort, sizeof(AHIRequest));
         }

         if (pRequest != NULL)
         {
            if (0 == Exec->OpenDevice("ahi.device", AHI_NO_UNIT, (IORequest*)pRequest, 0))
            {
               LibBase = (Library*)((IOStdReq*)pRequest)->io_Device;
            }
         }
      
         if (LibBase != NULL)
         {
#ifndef __AMIGAOS4__           
            pFace = new AHIIFace;
            pFace->LibBase    = LibBase;
#else
            pFace = (AHIIFace*)Exec->GetInterface(LibBase, "main", 1, 0);
#endif
            Exec->CloseDevice((IORequest*)pRequest);
         }

         if (pRequest != NULL)
         {
            Exec->DeleteIORequest((IORequest*)pRequest);
         }

         if (pPort != NULL)
         {
            Exec->DeleteMsgPort(pPort);
         }
      
         return pFace;
      }

      void      FreeInstance()
      {
         // we could never get here if we didnt have device opened unless someone hacked.
#ifdef __AMIGAOS4__
         Exec->DropInterface((Interface*)this);
#else
         delete this;
#endif
      }

      FUNC1(AHIAudioCtrl*,          AHI_AllocAudioA,                       7, const TagItem*,         tags,          a1);
      FUNC1(AHIAudioCtrl*,          AHI_AllocAudio,                        7, const TagItem*,         tags,          a1);
      PROC1(                        AHI_FreeAudio,                         8, AHIAudioCtrl*,          audioControl,  a2);
      PROC0(                        AHI_KillAudio,                         9);
      FUNC2(long,                   AHI_ControlAudioA,                    10, AHIAudioCtrl*,          audioControl,  a2,   const TagItem*,   tags,       a1);
      FUNC2(long,                   AHI_ControlAudio,                     10, AHIAudioCtrl*,          audioControl,  a2,   const TagItem*,   tags,       a1);
      PROC5(                        AHI_SetVol,                           11, long,                   channel,       d0,   long,             volume,     d1,   long,          pan,        d2,   AHIAudioCtrl*, audioCtrl,  a2,   long,          flags,      d3);
      PROC4(                        AHI_SetFreq,                          12, long,                   channel,       d0,   long,             freq,       d1,   AHIAudioCtrl*, audioCtrl,  a2,   long,          flags,      d2);
      PROC6(                        AHI_SetSound,                         13, long,                   channel,       d0,   long,             sound,      d1,   long,          offset,     d2,   long,          length,     d3,   AHIAudioCtrl*, audioCtrl,  a2,   long,    flags,   d4);
      FUNC2(long,                   AHI_SetEffect,                        14, void*,                  effect,        a0,   AHIAudioCtrl*,    audioCtrl,  a2);
      FUNC4(long,                   AHI_LoadSound,                        15, unsigned long,          sound,         d0,   unsigned long,    type,       d1,   void*,         info,       a0,   AHIAudioCtrl*, audioCtrl,  a2);
      PROC2(                        AHI_UnloadSound,                      16, unsigned long,          sound,         d0,   AHIAudioCtrl*,    audioCtrl,  a2);
      FUNC1(long,                   AHI_NextAudioID,                      17, unsigned long,          lastAudioID,   d0);
      FUNC3(bool,                   AHI_GetAudioAttrsA,                   18, unsigned long,          audioID,       d0,   AHIAudioCtrl*,    audioCtrl,  a2,   TagItem*,      tags,       a1);
      FUNC3(bool,                   AHI_GetAudioAttrs,                    18, unsigned long,          audioID,       d0,   AHIAudioCtrl*,    audioCtrl,  a2,   TagItem*,      tags,       a1);
      FUNC1(unsigned long,          AHI_BestAudioIDA,                     19, const TagItem*,         tags,          a1);
      FUNC1(unsigned long,          AHI_BestAudioID,                      19, const TagItem*,         tags,          a1);
      FUNC1(AHIAudioModeRequester*, AHI_AllocAudioRequestA,               20, const TagItem*,         tags,          a0);
      FUNC1(AHIAudioModeRequester*, AHI_AllocAudioRequest,                20, const TagItem*,         tags,          a0);
      FUNC2(bool,                   AHI_AudioRequestA,                    21, AHIAudioModeRequester*, requester,     a0,   const TagItem*,   tags,       a1);
      FUNC2(bool,                   AHI_AudioRequest,                     21, AHIAudioModeRequester*, requester,     a0,   const TagItem*,   tags,       a1);
      PROC1(                        AHI_FreeAudioRequest,                 22, AHIAudioModeRequester*, requester,     a0);
      FUNC2(long,                   AHI_PlayA,                            23, AHIAudioCtrl*,          audioContrl,   a2,   const TagItem*,   tags,       a1);
      FUNC2(long,                   AHI_Play,                             23, AHIAudioCtrl*,          audioContrl,   a2,   const TagItem*,   tags,       a1);
      FUNC1(unsigned long,          AHI_SampleFrameSize,                  24, unsigned long,          sampleType,    d0);
      FUNC1(unsigned long,          AHI_AddAudioMode,                     25, const TagItem*,         tags,          a0);
      FUNC1(unsigned long,          AHI_RemoveAudioMode,                  26, unsigned long,          audioMode,     d0);
      FUNC1(unsigned long,          AHI_LoadModeFile,                     27, const char*,            fileName,      a0);

   ENDDECL

#endif /*_AHIIFACE_H_*/
