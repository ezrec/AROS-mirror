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

#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include <Generic/LibrarySpool.h>
#include "../IEngine.h"
#include "Jobs/Job.h"
#include <DTLib/IRegHook.h>
#include <Generic/Generic.h>
#include <Generic/DistributorT.h>
#include <Generic/ConfigParser.h>
#include <Generic/String.h>
#include <Generic/RWSyncT.h>
#include <Generic/Thread.h>
#include <DTLib/Calls.h>
#include <ISOBuilder/Calls.h>
#include <DTLib/IRegHook.h>
#include <Generic/DynList.h>
#include <Optical/IOptItem.h>
#include "../ITrack.h"

#include <Generic/Property.h>

class Event;

class Engine : public IEngine
{
public:
   enum Command
   {
      Cmd_NoOperation   =  0,
      Cmd_ExecuteJob,
      Cmd_SendEvent,
      Cmd_Update,
      Cmd_SetWriteSpeed,
      Cmd_SetReadSpeed,
      Cmd_UpdateLayout
   };

protected:
   enum HookType
   {
      HType_Any,
      HType_Audio,
      HType_Data,
      HType_Session
   };

protected:
   DistributorT<EngineMessage, IEngine*>  handlers;

   String                     sDriveName;
   ConfigParser              *Config;

   String                     sDevice;
   int                        lUnit;
   RWSyncT<unsigned long>     Drive;

   String                     sDOSDevice;
   bool                       bDOSInhibit;
 
   String                     sDriveVendor;
   String                     sDriveProduct;
   String                     sDriveVersion;
   
   DRT_Mechanism              eMechanismType;
   DRT_Profile                eCurrentProfile;
   DRT_SubType                eDiscSubType;
   String                     sVendor;
   unsigned long              eReadableMedia;
   unsigned long              eWritableMedia;
   unsigned long              eCapabilities;
   bool                       bRegistered;
   bool                       bDiscPresent;
   bool                       bRecordable;
   bool                       bErasable;
   bool                       bFormatable;
   bool                       bOverwritable;
   DiscSpeed                 *readSpeeds;
   DiscSpeed                 *writeSpeeds;
   uint16                     readSpeed;
   uint16                     writeSpeed;
   IPTR                     lDiscSize;

   RWSyncT<Job*>              hJob;
   String                     sJobName;
   const IOptItem            *pCurrentDisc;
   IOptItem                  *pImportedSession;    // in fact this holds the complete structure (disc and below)
   IRegHook                  *pSessImportHook;     // this is not really the way we want it to be :P
   IFileReader               *pSessImportReader;   // :P
   /*
    * DTLib :)
    */

   VectorT<const char*>       vecExpData;
   VectorT<const char*>       vecExpAudio;
   VectorT<const char*>       vecExpSession;
   IRegHook                  *expData;
   IRegHook                  *expAudio;
   IRegHook                  *expSession;

   RWSyncT< VectorT<ITrack*> >   rwTracks;
   String                        hLayoutStatus;
   uint32                        hLayoutSize;
   
   /*
    * ISOBuilder
    */

   ClISO                     *iso;

protected:
   Thread                    *thrOperations;

protected:
   HookT<Engine, Thread*, void*>       hHkThrOperations;
   HookT<Engine, Command, Event*>      hHkHndOperations;

protected:
   virtual unsigned long      mainOperations(Thread*, void*);
   virtual unsigned long      hndlOperations(Command, Event*);

protected:
   virtual void               readContents();
   virtual void               readExportHooks();
   virtual IRegHook          *findHook(const char *name, bool bWriter, HookType type);
   virtual bool               setInhibited(String& device, bool state);
   virtual void               do_update();
   void                       do_layoutupdate();
   virtual void               do_setwritespeed();
   virtual void               do_setreadspeed();

public:
                              Engine(ConfigParser *parent, int which);
   virtual                   ~Engine();                           
   
   virtual void               readSettings();
   virtual void               writeSettings();

   virtual void               registerHandler(const Hook*);                // IEngine*, Message*
   virtual void               unregisterHandler(const Hook*);              // IEngine*, Message*

   virtual void               update();
   virtual void               notify(EngineMessage msg);

   virtual const char        *getName();
   virtual void               setName(const char* name);

   /*
    * exports
    */

   virtual VectorT<const char*>    &getDataExports();
   virtual VectorT<const char*>    &getAudioExports();
   virtual VectorT<const char*>    &getSessionExports();

   virtual const char*              getDataExport();
   virtual const char*              getAudioExport();
   virtual const char*              getSessionExport();
   virtual void                     setDataExport(const char* name);
   virtual void                     setAudioExport(const char* name);
   virtual void                     setSessionExport(const char* name);

   virtual IBrowser*                getISOBrowser();
   virtual void                     addISOItem(const char* path);
   virtual void                     remISOItem(ClElement* elem);
   virtual void                     makeISODir(const char* name);

   /*
    * device info & stuff
    */

   virtual bool               openDevice(const char* name, int lun);
   virtual const char        *getDevice();
   virtual int                getUnit();
   virtual void               closeDevice();
   virtual bool               isOpened();

   virtual const char        *getDriveVendor();
   virtual const char        *getDriveProduct();
   virtual const char        *getDriveVersion();

   virtual DRT_Mechanism      getMechanismType();
   virtual unsigned long      getReadableMedia();
   virtual unsigned long      getWritableMedia();
   
   virtual unsigned long      getCapabilities();

   virtual DiscSpeed         *getReadSpeeds();
   virtual DiscSpeed         *getWriteSpeeds();
   virtual uint16             getWriteSpeed();
   virtual uint16             getReadSpeed();
   virtual void               setWriteSpeed(uint16 speed);
   virtual void               setReadSpeed(uint16 speed);

   virtual bool               isRegistered();

   virtual bool               isDiscInserted();
   virtual bool               isDVD();
   virtual bool               isCD();
   virtual bool               isBD();
   virtual DRT_Profile        getDiscType();
   virtual DRT_SubType        getDiscSubType();
   virtual const char        *getDiscVendor();
   virtual bool               isRecordable();
   virtual bool               isErasable();
   virtual bool               isFormatable();
   virtual bool               isOverwritable();
   virtual uint32             getDiscSize();

   virtual const char        *getActionName();
   virtual const char        *getLayoutInfo();
   virtual uint32             getLayoutSize();
   virtual unsigned short     getActionProgress();
   virtual void               loadTray();
   virtual void               ejectTray();
   virtual void               closeSession();
   virtual void               closeDisc();
   virtual void               closeTracks();
   virtual void               repairDisc();
   virtual void               quickErase();
   virtual void               completeErase();
   virtual void               quickFormat();
   virtual void               completeFormat();
   virtual void               structureDisc();
   virtual void               updateAll();

   virtual const IOptItem    *getContents();
   virtual void               layoutTracks(bool masterize, bool close);
   virtual void               downloadTrack(const IOptItem*, const char*);
   virtual void               createImage(const char*);
   virtual void               addTrack(const char*);
   virtual void               remTrack(ITrack *track);

   virtual RWSyncT< VectorT<ITrack*> > &tracks();
   
   virtual bool               importSession(const char*);
   virtual const IOptItem    *getSessionContents();
   
   virtual void               recordDisc(bool masterize, bool closedisc);
   virtual void               recordSession();

   virtual void               setDOSDevice(const char*);
   virtual const char        *getDOSDevice();
   virtual void               setDOSInhibit(bool flag);
   virtual bool               getDOSInhibit();
   virtual const char        *findMatchingDOSDevice();
};

#endif
