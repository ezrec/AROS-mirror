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

#ifndef _IENGINE_H_
#define _IENGINE_H_

#include "Engine/Calls.h"
#include <Optical/IOptItem.h>
#include "ITrack.h"
#include <Generic/VectorT.h>
#include <Generic/RWSyncT.h>

class Hook;
class IBrowser;
class ClElement;

enum EngineMessage
{
   Eng_Update              =  0x1ff00000,
   Eng_JobStarted,
   Eng_JobFinished,
   Eng_DisableISO,
   Eng_EnableISO,
   Eng_UpdateLayout,
};

class IEngine 
{
public:
   virtual void                           registerHandler(const Hook*)              = 0;        // IEngine*, Message*
   virtual void                           unregisterHandler(const Hook*)            = 0;        //

   virtual const char                    *getName()                                 = 0;
   virtual void                           setName(const char* name)                 = 0;

   virtual VectorT<const char*>          &getDataExports()                          = 0;
   virtual VectorT<const char*>          &getAudioExports()                         = 0;
   virtual VectorT<const char*>          &getSessionExports()                       = 0;

   virtual const char*                    getDataExport()                           = 0;
   virtual const char*                    getAudioExport()                          = 0;
   virtual const char*                    getSessionExport()                        = 0;
   virtual void                           setDataExport(const char* name)           = 0;
   virtual void                           setAudioExport(const char* name)          = 0;
   virtual void                           setSessionExport(const char* name)        = 0;

   virtual IBrowser*                      getISOBrowser()                           = 0;
   virtual void                           addISOItem(const char* path)              = 0;
   virtual void                           remISOItem(ClElement* elem)               = 0;
   virtual void                           makeISODir(const char* name)              = 0;
   /* 
    * Open & Close device
    */

   virtual bool                           openDevice(const char* name, int unit)    = 0;
   virtual const char                    *getDevice()                               = 0;
   virtual int                            getUnit()                                 = 0;
   virtual void                           closeDevice()                             = 0;
   virtual bool                           isOpened()                                = 0;

   /* 
    * drive information
    */

   virtual const char                    *getDriveVendor()                          = 0;
   virtual const char                    *getDriveProduct()                         = 0;
   virtual const char                    *getDriveVersion()                         = 0;

   virtual DRT_Mechanism                  getMechanismType()                        = 0;
   virtual unsigned long                  getReadableMedia()                        = 0;
   virtual unsigned long                  getWritableMedia()                        = 0;
   virtual unsigned long                  getCapabilities()                         = 0;
   virtual DiscSpeed                     *getReadSpeeds()                           = 0;
   virtual DiscSpeed                     *getWriteSpeeds()                          = 0;
   virtual uint16                         getWriteSpeed()                           = 0;
   virtual uint16                         getReadSpeed()                            = 0;
   virtual void                           setWriteSpeed(uint16)                     = 0;
   virtual void                           setReadSpeed(uint16)                      = 0;
   virtual bool                           isRegistered()                            = 0;

   /*
    * media information
    */
   virtual bool                           isDiscInserted()                          = 0;
   virtual bool                           isDVD()                                   = 0;
   virtual bool                           isCD()                                    = 0;
   virtual bool                           isBD()                                    = 0;
   virtual DRT_Profile                    getDiscType()                             = 0;
   virtual DRT_SubType                    getDiscSubType()                          = 0;
   virtual const char                    *getDiscVendor()                           = 0;
   virtual bool                           isRecordable()                            = 0;
   virtual bool                           isErasable()                              = 0;
   virtual bool                           isFormatable()                            = 0;
   virtual bool                           isOverwritable()                          = 0;
   virtual uint32                         getDiscSize()                             = 0;

   /*
    * actions
    */

   virtual void                           updateAll()                               = 0;
   virtual const char                    *getActionName()                           = 0;
   virtual const char                    *getLayoutInfo()                           = 0;
   virtual uint32                         getLayoutSize()                           = 0;
   virtual unsigned short                 getActionProgress()                       = 0;
   virtual void                           loadTray()                                = 0;
   virtual void                           ejectTray()                               = 0;
   virtual void                           closeSession()                            = 0;
   virtual void                           closeDisc()                               = 0;
   virtual void                           closeTracks()                             = 0;
   virtual void                           repairDisc()                              = 0;
   virtual void                           quickErase()                              = 0;
   virtual void                           completeErase()                           = 0;
   virtual void                           quickFormat()                             = 0;
   virtual void                           completeFormat()                          = 0;
   virtual void                           structureDisc()                           = 0;

   virtual const IOptItem                *getContents()                             = 0;
   virtual void                           layoutTracks(bool, bool)                  = 0;

   virtual void                           downloadTrack(const IOptItem*, const char*)  = 0;
   virtual void                           createImage(const char*)                  = 0;
   virtual void                           addTrack(const char*)                     = 0;
   virtual void                           remTrack(ITrack *track)                   = 0;
   virtual RWSyncT< VectorT<ITrack*> >   &tracks()                                  = 0;

   virtual bool                           importSession(const char*)                = 0;
   virtual const IOptItem                *getSessionContents()                      = 0;

   virtual void                           recordDisc(bool masterize, bool closedisc)= 0;
   virtual void                           recordSession()                           = 0;     // upload only the session

   virtual void                           setDOSDevice(const char*)                 = 0;
   virtual const char                    *getDOSDevice()                            = 0;
   virtual void                           setDOSInhibit(bool flag)                  = 0;
   virtual bool                           getDOSInhibit()                           = 0;
   virtual const char                    *findMatchingDOSDevice()                   = 0;
};

#endif
