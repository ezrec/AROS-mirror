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

#include "JobLayout.h"
#include <Optical/IOptItem.h>
#include <utility/tagitem.h>

JobLayout::JobLayout(unsigned long drive, VectorT<ITrack*> &trks, bool master, bool closedisc, String &res, uint32 &size) :
   Job(drive),
   numblocks(size),
   operation(res)
{
   disc      = pOptical->OptCreateDisc();
   session   = disc->addChild();
   operation = "Analysing layout";

   for (int i=0; i<trks.Count(); i++)
   {
      if (trks[i]->isValid())
      {
         ITrack   *t = trks[i]->getClone();
         IOptItem *o = session->addChild();
   
         t->update();
         t->fillOptItem(o);

         tracks << t;
         items  << o;
      }
   }

   disc->setFlags((closedisc ? DIF_Disc_CloseDisc   : DIF_Disc_CloseSession) |
                  (master    ? DIF_Disc_MasterizeCD : 0));
   session->setFlags(0);
}

JobLayout::~JobLayout()
{
   while (tracks.Count() > 0)
   {
      tracks[0]->dispose();
      tracks >> tracks[0];
   }

   disc->dispose();
}

void JobLayout::execute()
 {
   EOpticalError  res = ODE_OK;

   if (tracks.Count() == 0)
   {
      operation = "Nothing to be written.";
      numblocks = 0;
      return;
   }

   res = (EOpticalError)pOptical->OptDoMethodA(ARRAY(DRV_LayoutTracks, Drive, (IPTR)disc));

   analyse(res);
}

void JobLayout::analyse(EOpticalError ret)
{
   String layout;

   {
      IPTR size = 0;
      uint32 curr = disc->getEndAddress();
      uint32 fsize=0, fcurr=0;
      uint32 ssize, scurr;
      String suff = "kB";

      pOptical->OptDoMethodA(ARRAY(DRV_GetAttrs, Drive, DRA_Disc_Size, (IPTR)&size, TAG_DONE, 0));
      numblocks = size;

      if (size == 0)
      {
         operation = "No disc";
      }
      else
      {
         int pf = 0, pi = 0;

         ssize = size;
         scurr = curr;

         size <<= 1;
         curr <<= 1;

         if (size > 2000)
         {
            pi = curr * 100 / size;
            pf = ((curr * 10000) / size) % 100;
            fsize = size;
            fcurr = curr;
            size >>= 10;
            curr >>= 10;
            suff = "MB";
         }
         if (size > 2000)
         {
            pi = curr * 100 / size;
            pf = ((curr * 10000) / size) % 100;
            fsize = size;
            fcurr = curr;
            size >>= 10;
            curr >>= 10;
            suff = "GB";
         }

         layout.FormatStr("%ld.%02ld%% (%ld.%02ld%s / %ld.%02ld%s, %ld / %ld sectors)", ARRAY(
                  pi,
                  pf,
                  curr,
                  ((fcurr * 100) >> 10) % 100,
                  (IPTR)suff.Data(),
                  size,
                  ((fsize * 100) >> 10) % 100,
                  (IPTR)suff.Data(),
                  scurr,
                  ssize,
                  ));
         numblocks = scurr;
      }
   }
 
   switch (ret)
   {
      case ODE_OK:
         operation = layout;
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
         operation = "Not enough free space on disc. "+layout;
         break;

      case ODE_DiscNotEmpty:
         operation = "Disc not empty. Please insert an empty disc.";
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
         operation = "Bad layout for this disc.";
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
         operation = "Device is not ready.";
         break;

      case ODE_BadTrackMode:
         operation = "Bad track type for this disc.";
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

   return;
}


unsigned long JobLayout::getProgress()
{
   return 65535;
}

const char *JobLayout::getActionName()
{
   return operation.Data();
}

bool JobLayout::inhibitDOS()
{
   return false;
}
