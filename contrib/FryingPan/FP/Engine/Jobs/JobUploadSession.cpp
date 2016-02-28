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

#include "JobUploadSession.h"
#include <Optical/IOptItem.h>
#include <Optical/Optical.h>

JobUploadSession::JobUploadSession(unsigned long drive, const IOptItem *src, IFileReader *rdr) :
   Job(drive)
{
   disc     = src;
   reader   = rdr;
   
   operation = "Recording Disc Image";
   disc->claim();
}

JobUploadSession::~JobUploadSession()
{
   disc->dispose();
}

void JobUploadSession::execute()
{
   unsigned long res = 0;
   
   /*
    * in general, we do not want to play layouts. it won't be here..
    * reason: if someone wants to record a disc image, this image already carries the layout details.
    */

   res = pOptical->OptDoMethodA(ARRAY(DRV_UploadLayout, Drive, (IPTR)disc));

   numBlocks = 0;
   currBlock = 0;

   for (int i=0; i<disc->getChild(0)->getChildCount(); i++)
      numBlocks += disc->getChild(0)->getChild(i)->getDataBlockCount();


   if (res == ODE_OK)
   {
      char    *memblk   = new char[(3072 << 4)];

      numBlocks = disc->getDataBlockCount();
      currBlock = 0;

      pOptical->OptDoMethodA(ARRAY(DRV_LockDrive, Drive, DRT_LockDrive_Write));

      reader->setUp();

      for (int i=0; i<disc->getChild(0)->getChildCount(); i++)
      {
         uint32   size     = disc->getChild(0)->getChild(i)->getDataBlockCount();

         while (size > 0)
         {
            uint32 trns = 16 < size ? 16 : size;
            reader->readData(disc, memblk, trns);
            res = (EOpticalError)pOptical->OptDoMethodA(ARRAY(DRV_WriteSequential, Drive, (IPTR)memblk, trns));
            currBlock += trns;
            size -= trns;
            
            if (res)
               break;
         }


         if (res)
            break;
      }

      delete [] memblk;

      reader->cleanUp();

      if (disc->getFlags() & DIF_Disc_CloseDisc)
         pOptical->OptDoMethodA(ARRAY(DRV_CloseDisc, Drive, DRT_Close_Finalize));
      else
         pOptical->OptDoMethodA(ARRAY(DRV_CloseDisc, Drive, DRT_Close_Session));

      pOptical->OptDoMethodA(ARRAY(DRV_LockDrive, Drive, DRT_LockDrive_Unlock));

      if (res != 0)
      {
         request("Error", "Error during write process. Operation aborted.", "Proceed", 0);
      }
   }
   else
   {
      request("Information", "Track upload failed (%ld). Disc will not be written to.", "Proceed", ARRAY(res));
   }
}

uint32 JobUploadSession::onData(void* mem, int sectors)
{
   EOpticalError ret = ODE_OK;

   if (sectors == 0)
   {
      // we're done with iso image
      return ODE_OK;
   }

   currBlock += sectors;
   ret = (EOpticalError)pOptical->OptDoMethodA(ARRAY(DRV_WriteSequential, Drive, (IPTR)mem, sectors));

   switch (ret)
   {
      case ODE_OK:
         break;

      case ODE_NoMemory:           
         operation = "Not enough memory to complete.";
         break;

      case ODE_NoHandler:
         operation = "No device handler.";
         break;

      case ODE_NoDevice:
         operation = "Device not opened.";
         break;

      case ODE_InitError:
         operation = "Initialization failure.";
         break;

      case ODE_BadDriveType:
         operation = "Illegal drive type.";
         break;

      case ODE_MediumError:
         operation = "Medium failure, disc cannot be written to.";
         break;
         
      case ODE_IllegalType:
         operation = "Illegal disc type.";
         break;

      case ODE_NoDevOpened:
         operation = "No device opened.";
         break;

      case ODE_CommandError:
         operation = "Command failed. Disk cannot be written to.";
         break;

      case ODE_NoFormatDescriptors:
         operation = "No format descriptors found. Disc cannot be formatted.";
         break;

      case ODE_NoModePage:
         operation = "Unable to set device page, device refused to cooperate.";
         break;

      case ODE_DeviceBusy:
         operation = "Device is busy.";
         break;

      case ODE_IllegalCommand:
         operation = "Illegal command. Please try again.";
         break;

      case ODE_TooManyTracks:
         operation = "Too many tracks.";
         break;

      case ODE_TooManySessions:
         operation = "Too many sessions.";
         break;

      case ODE_IllegalParameter:
         operation = "Illegal parameter, device refused to accept settings.";
         break;

      case ODE_NoDisc:
         operation = "Disc has been replaced.";
         break;

      case ODE_NotEnoughSpace:
         operation = "Not enough free space.";
         break;

      case ODE_DiscNotEmpty:
         operation = "Disc not empty.";
         break;

      case ODE_BadSessionNumber:
         operation = "Bad session number.";
         break;

      case ODE_BadTrackNumber:
         operation = "Bad track number.";
         break;

      case ODE_BadIndexNumber:
         operation = "Bad index number.";
         break;

      case ODE_BadLayout:
         operation = "Bad layout.";
         break;

      case ODE_DiscFull:
         operation = "Disc is full.";
         break;

      case ODE_DiscReserved:
         operation = "Disc has been reserved.";
         break;

      case ODE_DiscNotFormatted:
         operation = "Disc is not formatted.";
         break;

      case ODE_DeviceNotReady:
         operation = "Disc is not ready.";
         break;

      case ODE_BadTrackMode:
         operation = "Bad track type.";
         break;

      case ODE_BadBlockNumber:
         operation = "Bad block number.";
         break;

      case ODE_Exiting:
         operation = "Device rejected command because it is now cleaning up.";
         break;

      case ODE_DriveLockedForWrite:
         operation = "Device locked for write.";
         break;
         
      case ODE_DriveLockedForRead:
         operation = "Device locked for read.";
         break;

      case ODE_BadTransferSize:
         operation = "Bad transfer size.";
         break;

      case ODE_NoDataReturned:
         operation = "No data returned by drive.";
         break;
    }

   if (ret != ODE_OK)
   {
      operation += "\nwhile transferring\n%ld sectors from %08lx";
      request("Error during write", operation.Data(), "Abort", ARRAY(sectors, (IPTR)mem));
   }

   return (ret == ODE_OK);
}


unsigned long JobUploadSession::getProgress()
{
   unsigned long long s1=0, s2=0;

   s2 = numBlocks;
   s1 = currBlock;

   if (s2 == 0)
      return 0;

   while (s2 >= 16384)
   {
      s2 >>= 1;
      s1 >>= 1;
   }

   return ((long)s1 * 65535) / (long)s2;
}

const char *JobUploadSession::getActionName()
{
   return operation.Data();
}

bool JobUploadSession::inhibitDOS()
{
   return true;
}
