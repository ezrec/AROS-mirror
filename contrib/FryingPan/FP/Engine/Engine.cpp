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

#include "Engine.h"
#include "Globals.h"
#include <libclass/dos.h>
#include <Generic/Thread.h>
#include <DTLib/IFileReader.h>
#include <DTLib/IFileWriter.h>
#include "Jobs/Job.h"
#include <ISOBuilder/Calls.h>
#include "Track.h"
#include "ISOTrack.h"
#include "Event.h"

#include "Jobs/JobControl.h"
#include "Jobs/JobMediumAction.h"
#include "Jobs/JobDownload.h"
#include "Jobs/JobUpload.h"
#include "Jobs/JobUploadSession.h"
#include "Jobs/JobUpdate.h"
#include "Jobs/JobCreateISO.h"
#include "Jobs/JobISOAddItem.h"
#include "Jobs/JobISORemItem.h"
#include "Jobs/JobISOMakeDir.h"
#include "Jobs/JobLayout.h"
#include <utility/tagitem.h>

#include <dos/filehandler.h>
#include <dos/dosextens.h>
#include <Generic/XMLDocument.h>

   static const char      *Cfg_DriveName        = "DriveName";
   static const char      *Cfg_Device           = "Device";
   static const char      *Cfg_Unit             = "Unit";
   static const char      *Cfg_DOSDevice        = "DOSDevice";
   static const char      *Cfg_DOSInhibit       = "DOSInhibit";

   static const char      *Cfg_ISOBuilder       = "ISOBuilder";
   static const char      *Cfg_ISODiscName      = "DiscName";
   static const char      *Cfg_ISOSystemID      = "SystemID";
   static const char      *Cfg_ISOVolSetID      = "VolumeSetID";
   static const char      *Cfg_ISOPreparerID    = "PreparerID";
   static const char      *Cfg_ISOPublisherID   = "PublisherID";
   static const char      *Cfg_ISOApplicationID = "ApplicationID";

   static const char      *Cfg_DataExport       = "DataExportHook";
   static const char      *Cfg_AudioExport      = "AudioExportHook";
   static const char      *Cfg_SessionExport    = "SessionExportHook";

Engine::Engine(ConfigParser *parent, int which)
{
   expData           = 0;
   expAudio          = 0;
   expSession        = 0;
   iso               = 0;
   pImportedSession  = 0;
   pSessImportHook   = 0;
   pSessImportReader = 0;

   bErasable   = false;
   bFormatable = false;
   bRecordable = false;
   bOverwritable=false;

   readExportHooks();

   if (pISO != NULL)
   {
      iso = pISO->Create();
   }

   if (pOptical != NULL)
   {
      pImportedSession = pOptical->OptCreateDisc();
   }

   Config = new ConfigParser(parent, "Drive", which);

   Drive.Assign(0);
   hJob.Assign(0);
   hLayoutStatus = "";
   hLayoutSize   = 0;
   
   sDriveName.FormatStr("Drive %ld", ARRAY(which));
   readSettings();

   hHkThrOperations.Initialize(this, &Engine::mainOperations);
   hHkHndOperations.Initialize(this, &Engine::hndlOperations);

   thrOperations = new Thread("Engine Ops", hHkThrOperations.GetHook(), 0);
   thrOperations->SetHandler(hHkHndOperations.GetHook());

   openDevice(sDevice, lUnit);
}

Engine::~Engine()
{
   VectorT<ITrack*> &vecTracks = tracks().ObtainWrite();

   thrOperations->Terminate();
   delete thrOperations;

   while (vecTracks.Count() > 0)
   {
      vecTracks[0]->dispose();
      vecTracks >> vecTracks[0];
   }

   tracks().Release();

   writeSettings();  // writes data from iso image. need to be before;

   if (pISO != 0)
   {
      if (iso != 0)
         iso->destroy();
      iso = 0;
   }

   if (pImportedSession != 0)
      pImportedSession->dispose();
   pImportedSession = 0;

   if (pSessImportReader != 0)
      pSessImportReader->dispose();

   closeDevice();
   delete Config;
}

unsigned long Engine::mainOperations(Thread* thread, void*)
{
   int            did = -1;
   int            lid = -1;
   unsigned long  drv = 0;

   do
   {
      if (NULL != pOptical)
      {
         drv = Drive.ObtainRead();
         if (drv != 0)
         {
            pOptical->OptDoMethodA(ARRAY(DRV_GetAttrs, drv, 
               DRA_Disc_CurrentDiscSeqNum,   (int32)&did,
               TAG_DONE,                     0));
            if (did != lid)
               do_update();
            lid = did;
         }
         Drive.Release();
      }
   }
   while (false == thread->HandleSignals(1000));

   return 0;
}

unsigned long Engine::hndlOperations(Command cmd, Event* ev)
{
   if (cmd == Cmd_NoOperation)
      return 0;

   switch (ev->getEventType())
   {
      case Event::Ev_Job:
         {
            Job     *job      = ev->getJob();
            String   dosdev   = sDOSDevice;
            bool     inhibit  = bDOSInhibit;

            // if user marked device for inhibition, try to obtain it
            // and remember if we succeeded.
            if (inhibit & job->inhibitDOS())
               inhibit = setInhibited(dosdev, true);

            hJob.Assign(job);
            sJobName = job->getActionName();
            notify(Eng_JobStarted);
            job->execute();
            sJobName.Assign("Ready");
            hJob.Assign(0);
            notify(Eng_JobFinished);

            if (inhibit & job->inhibitDOS())
               setInhibited(dosdev, false);
         }
         break;
      case Event::Ev_Notify:
         {
            notify(ev->getMessage());
         }
         break;
      case Event::Ev_Action:
         {
            switch (ev->getAction())
            {
               case Cmd_Update:
                  do_update();
                  break;

               case Cmd_SetWriteSpeed:
                  do_setwritespeed();
                  break;

               case Cmd_SetReadSpeed:
                  do_setreadspeed();
                  break;

               case Cmd_NoOperation:
               case Cmd_ExecuteJob:
               case Cmd_SendEvent:
                  break;

               case Cmd_UpdateLayout:
                  do_layoutupdate();
                  break;
            }
         }
         break;
   };
   delete ev;
   return 0;
}

bool Engine::setInhibited(String& device, bool state)
{
   String               s;
   DeviceNode          *dn;

   dn = (DeviceNode*)DOS->LockDosList(LDF_DEVICES | LDF_READ);

   while (NULL != (dn = (DeviceNode*)DOS->NextDosEntry((DosList*)dn, LDF_DEVICES)))
   {
      if ((IPTR)dn->dn_Startup > 1024)
      {
         s.BstrCpy(dn->dn_Name);
         if (s == device)
            break;
      }
   }

   DOS->UnLockDosList(LDF_DEVICES);

   if (dn == 0)
      return false;

   s += ":";
   DOS->Inhibit(s.Data(), state);

   return state;
}

void Engine::readSettings()
{
   ConfigParser *ciso = new ConfigParser(Config, Cfg_ISOBuilder);

   if (iso != 0)
   {
      iso->getRoot()->setVolumeID(ciso->getValue(Cfg_ISODiscName,             "CDROM"));
      iso->getRoot()->setSystemID(ciso->getValue(Cfg_ISOSystemID,             "AMIGA"));
      iso->getRoot()->setVolumeSetID(ciso->getValue(Cfg_ISOVolSetID,          ""));
      iso->getRoot()->setPreparerID(ciso->getValue(Cfg_ISOPreparerID,         ""));
      iso->getRoot()->setPublisherID(ciso->getValue(Cfg_ISOPublisherID,       ""));
      iso->getRoot()->setApplicationID(ciso->getValue(Cfg_ISOApplicationID,   "THE FRYING PAN - AMIGA CD/DVD MASTERING SOFTWARE"));
   }
   delete ciso;

   sDriveName        = Config->getValue(Cfg_DriveName,      sDriveName); 
   sDevice           = Config->getValue(Cfg_Device,         "");
   lUnit             = Config->getValue(Cfg_Unit,           (long)0);
   sDOSDevice        = Config->getValue(Cfg_DOSDevice,      "");
   bDOSInhibit       = Config->getValue(Cfg_DOSInhibit,     (long)false);

   setDataExport(Config->getValue(Cfg_DataExport,        ""));
   setAudioExport(Config->getValue(Cfg_AudioExport,      ""));
   setSessionExport(Config->getValue(Cfg_SessionExport,  ""));
}

void Engine::writeSettings()
{
   ASSERT(NULL != Config);

   if (NULL == Config)
      return;

   Config->setValue(Cfg_DriveName,        sDriveName);
   Config->setValue(Cfg_Device,           sDevice);
   Config->setValue(Cfg_Unit,             lUnit);

   Config->setValue(Cfg_DOSDevice,        sDOSDevice);
   Config->setValue(Cfg_DOSInhibit,       bDOSInhibit);

   Config->setValue(Cfg_DataExport,       getDataExport());
   Config->setValue(Cfg_AudioExport,      getAudioExport());
   Config->setValue(Cfg_SessionExport,    getSessionExport());

   {
      ConfigParser *ciso = new ConfigParser(Config, Cfg_ISOBuilder);

      ciso->setValue(Cfg_ISODiscName,      iso->getRoot()->getVolumeID());
      ciso->setValue(Cfg_ISOSystemID,      iso->getRoot()->getSystemID());
      ciso->setValue(Cfg_ISOVolSetID,      iso->getRoot()->getVolumeSetID());
      ciso->setValue(Cfg_ISOPreparerID,    iso->getRoot()->getPreparerID());
      ciso->setValue(Cfg_ISOPublisherID,   iso->getRoot()->getPublisherID());
      ciso->setValue(Cfg_ISOApplicationID, iso->getRoot()->getApplicationID());

      delete ciso;
   }
}

void Engine::registerHandler(const Hook* h)
{ 
   handlers << h;
}

void Engine::unregisterHandler(const Hook* h)
{
   handlers >> h;
}

void Engine::readExportHooks()
{
   vecExpData.Empty();
   vecExpAudio.Empty();
   vecExpSession.Empty();

   for (unsigned int i=0; i<pRWHooks->GetCount(); i++)
   {
      if ((*pRWHooks)[i]->isWriter())
      {
         if ((*pRWHooks)[i]->isData())
         {
            vecExpData << (*pRWHooks)[i]->getName();
         }

         if ((*pRWHooks)[i]->isAudio())
         {
            vecExpAudio << (*pRWHooks)[i]->getName();
         }

         if ((*pRWHooks)[i]->isSession())
         {
            vecExpSession << (*pRWHooks)[i]->getName();
         }
      }
   }
}

IRegHook *Engine::findHook(const char *name, bool bWriter, HookType type)
{
   for (unsigned int i=0; i<pRWHooks->GetCount(); i++)
   {
      if (bWriter == (*pRWHooks)[i]->isWriter())
      {
         if (strcmp(name, (*pRWHooks)[i]->getName()) == 0)
         {
            switch (type)
            {
               case HType_Any:
                  return (*pRWHooks)[i];

               case HType_Data:
                  if ((*pRWHooks)[i]->isData())
                     return (*pRWHooks)[i];
                  break;

               case HType_Audio:
                  if ((*pRWHooks)[i]->isAudio())
                     return (*pRWHooks)[i];
                  break;

               case HType_Session:
                  if ((*pRWHooks)[i]->isSession())
                     return (*pRWHooks)[i];
                  break;
            }
         }
      }
   }
   return 0;
}

void Engine::do_update()
{
   unsigned long     ret;
   unsigned long     drv;

   drv = Drive.ObtainRead();

   sDriveVendor   = "";
   sDriveProduct  = "";
   sDriveVersion  = "";
   pCurrentDisc = 0;

   //MessageBox("Info", "reading drive info", "ok", 0);
   if (drv != 0)
   {
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_IsDiscInserted));
      bDiscPresent   = ret ? true : false;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_Vendor));
      sDriveVendor   = (const char*)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_Product));
      sDriveProduct  = (const char*)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_Version));
      sDriveVersion  = (const char*)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_MechanismType));
      eMechanismType = (DRT_Mechanism)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_ReadsMedia));
      eReadableMedia = ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_WritesMedia));
      eWritableMedia = ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_Capabilities));
      eCapabilities  = ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_CurrentProfile));
      eCurrentProfile = (DRT_Profile)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_IsRegistered));
      bRegistered = ret ? true : false;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Disc_SubType));
      eDiscSubType = (DRT_SubType)ret;
      ret = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Disc_Vendor));
      sVendor = (char*)ret;

      readSpeeds  = (DiscSpeed*)pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_ReadSpeeds));
      writeSpeeds = (DiscSpeed*)pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_WriteSpeeds));
      readSpeed = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_CurrentReadSpeed));
      writeSpeed = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_CurrentWriteSpeed));
   }
   Drive.Release();

   //MessageBox("Info", "reading drive contents", "ok", 0);
   readContents();

   //MessageBox("Info", "setting contents", "ok", 0);
   notify(Eng_Update);

   //MessageBox("Info", "writing settings", "ok", 0);
   writeSettings();

   //MessageBox("Info", "all done", "ok", 0);
}
   
void Engine::do_layoutupdate()
{
   VectorT<ITrack*> v;
   VectorT<ITrack*> &vecTracks = tracks().ObtainRead();
   String res;

   hLayoutSize = 0;
   hLayoutStatus = "Nothing to write.";

   if (false == iso->isEmpty())
      v << new ISOTrack(iso);

   for (int32 i=0; i<vecTracks.Count(); i++)
   {
      v << vecTracks[i];
   }

   tracks().Release();

   if (v.Count() != 0)
   {
      unsigned long drive = Drive.ObtainRead();
      if (0 != drive)
      {
         // we do not want to run this job in background.
         JobLayout *job = new JobLayout(drive, v, false, false, hLayoutStatus, hLayoutSize);

         /*
          * append operations to the end of the list ;)
          * this way we're sure all events modifying the iso image or things are complete.
          */        
         thrOperations->DoAsync(Cmd_ExecuteJob, new Event(job));
      }
      Drive.Release();
   }

   thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_UpdateLayout));
}

void Engine::readContents()
{
   unsigned long     drv;
   const IOptItem   *items = pCurrentDisc;

   drv = Drive.ObtainRead();
   if (drv == 0)
   {
      Drive.Release();
      return;
   }

   pCurrentDisc = (const IOptItem*)pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Disc_Contents));

   {
      int32 rec, era, fmt, ovw;

      pOptical->OptDoMethodA(ARRAY(DRV_GetAttrs, drv, 
            DRA_Disc_IsWritable,    (IPTR)&rec, 
            DRA_Disc_IsErasable,    (IPTR)&era, 
            DRA_Disc_IsFormatable,  (IPTR)&fmt,
            DRA_Disc_IsOverwritable,(IPTR)&ovw,
            DRA_Disc_Size,          (IPTR)&lDiscSize,
            TAG_DONE,               0));

      bRecordable    = rec ? true : false;
      bErasable      = era ? true : false;
      bFormatable    = fmt ? true : false;
      bOverwritable  = ovw ? true : false;
   }

   if (pCurrentDisc != 0)
      pCurrentDisc->claim();
   if (items != 0)
      items->dispose();
   Drive.Release();
   return;
}

void Engine::notify(EngineMessage msg)
{
   handlers.Send(msg, this);
}
   
const char *Engine::getName()
{
   return sDriveName.Data();
}

void Engine::setName(const char* name)
{
   sDriveName = name;
}

VectorT<const char*> &Engine::getDataExports()
{
   return vecExpData;
}

VectorT<const char*> &Engine::getAudioExports()
{
   return vecExpAudio;
}

VectorT<const char*> &Engine::getSessionExports()
{
   return vecExpSession;
}

const char* Engine::getDataExport()
{
   if (NULL != expData)
   {
      return expData->getName();
   }
   return 0;
}

const char* Engine::getAudioExport()
{
   if (NULL != expAudio)
   {
      return expAudio->getName();
   }
   return 0;
}

const char* Engine::getSessionExport()
{
   if (NULL != expSession)
   {
      return expSession->getName();
   }
   return 0;
}

void Engine::setDataExport(const char* name)
{
   expData = findHook(name, true, HType_Data);
}

void Engine::setAudioExport(const char* name)
{
   expAudio = findHook(name, true, HType_Audio);
}

void Engine::setSessionExport(const char* name)
{
   expSession = findHook(name, true, HType_Session);
}

IBrowser *Engine::getISOBrowser()
{
   return iso;
}

const char *Engine::getDevice()
{
   return sDevice.Data();
}

int Engine::getUnit()
{
   return lUnit;
}

bool Engine::openDevice(const char* name, int lun)
{
   unsigned long pdrv = 0;

   ASSERT(NULL != pOptical);

   if (pOptical == NULL)
      return false;

   pdrv = Drive.ObtainRead();
   Drive.Release();


   // 
   // only if we have changed device or unit OR if we dont have the drive yet
   // like, if the previous try failed;
   //
   if ((lUnit   != lun)  ||
       (sDevice != name) ||
       (pdrv    == 0))
   {
      closeDevice();

      if (strlen(name) == 0)
         return false;

      pdrv = pOptical->OptDoMethodA(ARRAY(DRV_NewDrive, (IPTR)name, lun));

      Drive.Assign(pdrv);
   }

   if (pdrv != 0)
   {
      sDevice  = name;
      lUnit    = lun;
      update();
      return true;
   }
   else
   {
      sDevice  = "";
      lUnit    = 0;
      return false;
   }
}

void Engine::closeDevice()
{
   unsigned long pdrv;
   ASSERT(NULL != pOptical);

   if (pOptical == NULL)
      return;

   pdrv = Drive.Assign(0);
   
   if (pdrv != 0)
   {
      sDevice  = "";
      lUnit    = 0;
      pOptical->OptDoMethodA(ARRAY(DRV_EndDrive, pdrv));
   }
}
   
bool Engine::isOpened()
{
   unsigned long drv;
   drv = Drive.ObtainRead();
   Drive.Release();
   return (drv != 0) ? true : false;
}

const char *Engine::getDriveVendor()
{
   if (isOpened())
      return sDriveVendor.Data();
   return "";
}

const char *Engine::getDriveProduct()
{
   if (isOpened())
      return sDriveProduct.Data();
   return "";
}

const char *Engine::getDriveVersion()
{
   if (isOpened())
      return sDriveVersion.Data();
   return "";
}

DRT_Mechanism Engine::getMechanismType()
{
   if (isOpened())
      return eMechanismType;
   return DRT_Mechanism_Unknown;
}

unsigned long Engine::getReadableMedia()
{
   if (isOpened())
      return eReadableMedia;
   return 0;
}

unsigned long Engine::getWritableMedia()
{
   if (isOpened())
      return eWritableMedia;
   return 0;
}

unsigned long Engine::getCapabilities()
{ 
   if (isOpened())
      return eCapabilities;
   return 0;
}

DiscSpeed *Engine::getReadSpeeds()
{
   if (isOpened())
      return readSpeeds;
   return 0;
}

DiscSpeed *Engine::getWriteSpeeds()
{
   if (isOpened())
      return writeSpeeds; 
   return 0;
}

uint16 Engine::getWriteSpeed()
{
   return writeSpeed;
}

uint16 Engine::getReadSpeed()
{
   return readSpeed;
}

void Engine::setWriteSpeed(uint16 speed)
{
   writeSpeed = speed;
   Event *e = new Event(Cmd_SetWriteSpeed);
   thrOperations->DoAsync(Cmd_Update, e);
}

void Engine::setReadSpeed(uint16 speed)
{
   readSpeed = speed;
   Event *e = new Event(Cmd_SetReadSpeed);
   thrOperations->DoAsync(Cmd_Update, e);
}

void Engine::do_setwritespeed()
{
   uint32 drv = Drive.ObtainRead();

   if (drv != 0)
   {
      pOptical->OptDoMethodA(ARRAY(DRV_SetAttr, drv, DRA_Drive_CurrentWriteSpeed, writeSpeed, 0));
      writeSpeed = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_CurrentWriteSpeed));
   }

   Drive.Release();
}

void Engine::do_setreadspeed()
{
   uint32 drv = Drive.ObtainRead();

   if (drv != 0)
   {
      pOptical->OptDoMethodA(ARRAY(DRV_SetAttr, drv, DRA_Drive_CurrentReadSpeed, readSpeed, 0));
      readSpeed = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_CurrentReadSpeed));
   }

   Drive.Release();
}

bool Engine::isDiscInserted()
{
   return bDiscPresent;
}

bool Engine::isDVD()
{
   if (!isOpened())
      return false; 

   switch (getDiscType())
   {
      case DRT_Profile_DVD_ROM:
      case DRT_Profile_DVD_MinusR:
      case DRT_Profile_DVD_PlusR:
      case DRT_Profile_DVD_MinusRW_Sequential:
      case DRT_Profile_DVD_MinusRW_Restricted:
      case DRT_Profile_DVD_PlusRW:
      case DRT_Profile_DVD_RAM:
         return true;
      default:
         return false;
   }
}

bool Engine::isCD()
{
   if (!isOpened())
      return false;

   switch (getDiscType())
   {
      case DRT_Profile_CD_ROM:
      case DRT_Profile_CD_R:
      case DRT_Profile_CD_RW:
      case DRT_Profile_CD_MRW:
      case DRT_Profile_DDCD_ROM:
      case DRT_Profile_DDCD_R:
      case DRT_Profile_DDCD_RW:
         return true;
      default:
         return false;
   }
}

bool Engine::isBD()
{
   if (!isOpened())
      return false;

   switch (getDiscType())
   {
      case DRT_Profile_BD_ROM:
      case DRT_Profile_BD_R_Sequential:
      case DRT_Profile_BD_R_RandomWrite:
      case DRT_Profile_BD_RW:
         return true;
      default:
         return false;
   }
}

bool Engine::isRecordable()
{
   return bRecordable;
}

bool Engine::isErasable()
{
   return bErasable;
}

bool Engine::isFormatable()
{
   return bFormatable;
}

bool Engine::isOverwritable()
{
   return bOverwritable;
}

DRT_Profile Engine::getDiscType()
{
   if (isOpened())
      return eCurrentProfile;
   return DRT_Profile_NoDisc;
}

bool Engine::isRegistered()
{
   if (isOpened())
      return bRegistered;
   return false;              
}
   
const IOptItem *Engine::getContents()
{
   if (isOpened())
      return pCurrentDisc;
   return 0;
}

void Engine::layoutTracks(bool masterize, bool closedisc)
{
   thrOperations->DoAsync(Cmd_UpdateLayout, new Event(Cmd_UpdateLayout));
}

DRT_SubType Engine::getDiscSubType()
{
   return eDiscSubType;
}

const char *Engine::getDiscVendor()
{
   return sVendor.Data();
}

void Engine::setDOSDevice(const char* dev)
{
   sDOSDevice = dev;
}

const char *Engine::getDOSDevice()
{
   return sDOSDevice.Data();
}

void Engine::setDOSInhibit(bool flag)
{
   bDOSInhibit = flag;
}

bool Engine::getDOSInhibit()
{
   return bDOSInhibit;
}

const char* Engine::findMatchingDOSDevice()
{
   String               s;
   String               d;
   DeviceNode          *dn;
   FileSysStartupMsg   *fs;

   s.AllocBuf(128);

   dn = (DeviceNode*)DOS->LockDosList(LDF_DEVICES | LDF_READ);


   while (NULL != (dn = (DeviceNode*)DOS->NextDosEntry((DosList*)dn, LDF_DEVICES)))
   {
      if ((IPTR)dn->dn_Startup > 1024)
      {
         fs = (FileSysStartupMsg*)((IPTR)dn->dn_Startup << 2);
         if ((fs != 0) && (fs->fssm_Device))
         {
            s.BstrCpy(dn->dn_Name);
            d.BstrCpy(fs->fssm_Device);
            if (d == sDevice)
            {
               if ((int32)fs->fssm_Unit == lUnit)
               {
                  setDOSDevice(s.Data());
                  break;
               }
            }
         }
      }
   }

   DOS->UnLockDosList(LDF_DEVICES);

   return getDOSDevice();
}

   /*
    * device commands go here
    */

void Engine::update()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(Cmd_Update);
      thrOperations->DoAsync(Cmd_Update, e);
   }

   Drive.Release();
}

void Engine::ejectTray()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobControl(drv, DRT_Unit_Eject));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}

void Engine::loadTray()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobControl(drv, DRT_Unit_Load));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}

const char *Engine::getActionName()
{
   return sJobName.Data();
}

const char *Engine::getLayoutInfo()
{
   return hLayoutStatus.Data();
}

void Engine::closeSession()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_CloseSession));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}

void Engine::closeDisc()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_CloseDisc));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}
   
void Engine::closeTracks()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_CloseTracks));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}

void Engine::repairDisc()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_RepairDisc));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   Drive.Release();
}

void Engine::quickErase()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_QuickErase));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   layoutTracks(false, false);
   Drive.Release();
}

void Engine::completeErase()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_CompleteErase));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   layoutTracks(false, false);
   Drive.Release();
}

void Engine::quickFormat()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_QuickFormat));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   layoutTracks(false, false);
   Drive.Release();
}

void Engine::completeFormat()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_CompleteFormat));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   layoutTracks(false, false);
   Drive.Release();
}

void Engine::structureDisc()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      Event *e = new Event(new JobMediumAction(drv, JobMediumAction::Act_Prepare));
      thrOperations->DoAsync(Cmd_ExecuteJob, e);
   }

   layoutTracks(false, false);
   Drive.Release();
}

void Engine::downloadTrack(const IOptItem *track, const char* name)
{
   if (NULL == expData)
   {
      request("Information", "The ISO Export module has not been selected", "Ok", 0);
      return;
   }
   
   if (NULL == expAudio)
   {
      request("Information", "The Audio Export module has not been selected", "Ok", 0);
      return;
   }

   unsigned long drv = Drive.ObtainRead();
   track->claim();

   if (drv != 0)
   {
      JobDownload *dl;
      if (track->getItemType() == Item_Track)
      {
         if (track->getDataType() == Data_Audio)
         {
            dl = new JobDownload(drv, track, expAudio, name);
         }
         else
         {
            dl = new JobDownload(drv, track, expData, name);
         }
      }
      else
      {
         dl = new JobDownload(drv, track, expSession, name);
      }
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(dl));
   }

   Drive.Release();
}

unsigned short Engine::getActionProgress()
{
   unsigned short progress = 0;

   Job *pjob = hJob.ObtainRead();
   
   if (NULL != pjob)
   {
      progress = pjob->getProgress();
   }

   if (progress == 0)
   {
      uint32 drv = Drive.ObtainRead();
      if (drv != 0)
      {
         progress = pOptical->OptDoMethodA(ARRAY(DRV_GetAttr, drv, DRA_Drive_OperationProgress));
      }
      Drive.Release();
   }

   hJob.Release();

   return progress;
}

void Engine::addTrack(const char* name)
{
   VectorT<ITrack*> &vecTracks = tracks().ObtainWrite();

   Track *t = new Track(name);
   if (t->isValid())
      vecTracks << t;
   else
      t->dispose();
   thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_Update));

   tracks().Release();
}

void Engine::remTrack(ITrack* track)
{
   VectorT<ITrack*> &vecTracks = tracks().ObtainWrite();
   
   vecTracks >> track;
   thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_Update));
   
   tracks().Release();
}

RWSyncT< VectorT<ITrack*> > &Engine::tracks()
{
   return rwTracks;
}

void Engine::recordDisc(bool masterize, bool closedisc)
{
   VectorT<ITrack*> v;
   VectorT<ITrack*> &vecTracks = tracks().ObtainRead();

   if (iso->isEmpty() == false)
      v << new ISOTrack(iso);

   for (int32 i=0; i<vecTracks.Count(); i++)
   {
      v << vecTracks[i];
   }

   tracks().Release();

   if (v.Count() == 0)
      return;

   unsigned long drive = Drive.ObtainRead();
   if (0 != drive)
   {
      JobUpload *job = new JobUpload(drive, v, masterize, closedisc);
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(job));
   }
   Drive.Release();
}

void Engine::recordSession()
{
   unsigned long drive = Drive.ObtainRead();
   if (0 != drive)
   {
      JobUploadSession *job = new JobUploadSession(drive, pImportedSession, pSessImportReader);
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(job));
   }
   Drive.Release();
}

void Engine::updateAll()
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      JobUpdate *job = new JobUpdate(drv);
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(job));
   }

   Drive.Release();
}

void Engine::createImage(const char* name)
{
   if (NULL == expData)
   {
      request("Information", "The ISO Export module has not been selected", "Ok", 0);
      return;
   }

   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      JobCreateISO *dl;

      dl = new JobCreateISO(drv, iso, expData, name);
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(dl));
   }

   Drive.Release();
}

void Engine::addISOItem(const char* path)
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      JobISOAddItem *dl;

      dl = new JobISOAddItem(drv, iso, path);
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_DisableISO));
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(dl));
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_EnableISO));
   }

   Drive.Release();
}

void Engine::remISOItem(ClElement* elem)
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      JobISORemItem *dl;

      dl = new JobISORemItem(drv, iso, elem);
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_DisableISO));
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(dl));
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_EnableISO));
   }

   Drive.Release();
}

void Engine::makeISODir(const char* path)
{
   unsigned long drv = Drive.ObtainRead();

   if (drv != 0)
   {
      JobISOMakeDir *dl;

      dl = new JobISOMakeDir(drv, iso, path);
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_DisableISO));
      thrOperations->DoAsync(Cmd_ExecuteJob, new Event(dl));
      thrOperations->DoAsync(Cmd_SendEvent, new Event(Eng_EnableISO));
   }

   Drive.Release();
}

uint32 Engine::getLayoutSize()
{
   return hLayoutSize;
}
   
uint32 Engine::getDiscSize()
{
   return lDiscSize;
}
   
bool Engine::importSession(const char* name)
{
   EDtError rc = DT_OK;

   if (pSessImportReader != 0)
      pSessImportReader->dispose();
   pSessImportReader = 0;

   if (pImportedSession != 0)
      pImportedSession->dispose();
   pImportedSession = pOptical->OptCreateDisc();

   for (unsigned int i=0; i<pRWHooks->GetCount(); i++)
   {
      if (((*pRWHooks)[i]->isReader()) && (*pRWHooks)[i]->isSession())
      {
         if ((*pRWHooks)[i]->checkFile(name, rc))
         {
            pSessImportHook   = (*pRWHooks)[i];
            pSessImportReader = pSessImportHook->openRead(name, rc);

            if (rc == DT_OK)
            {
               pSessImportReader->fillOptItem(pImportedSession);
               return true;
            }

            if (pSessImportReader)
               pSessImportReader->dispose();
         }
      }
   }

   return false;
}

const IOptItem *Engine::getSessionContents()
{
   return pImportedSession;
}

