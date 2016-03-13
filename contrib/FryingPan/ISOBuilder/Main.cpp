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

#include <Generic/LibrarySpool.h>
#include "Main.h"
#include "ClISO.h"
#include "ISOStructures.h"
#include <libclass/dos.h>
#include <dos/dos.h>

Main::Main()
{
   fh = DOS->Open("Output.img", MODE_NEWFILE);
   hook.Initialize(this, &Main::fWrite);
}

Main::~Main()
{
   DOS->Close(fh);
}

unsigned long Main::fWrite(void *pMem, long lLen)
{
   //DPrintfA("Writing %ld bytes\n", lLen << 11);
   DOS->Write(fh, pMem, lLen << 11);
   return true;
}

const struct Hook *Main::getHook()
{
   return hook.GetHook();
}


struct Args
{
   char       *dir;

   static const char* Template()
   {
      return "DIR/A";
   }
};


int main()
{
   ClISO    *pISO;
   Main     *pMain;
   ISOPrimaryVolumeDescriptor *pDesc = new ISOPrimaryVolumeDescriptor();
   Args      args; 
   RDArgs   *rda;

   LibrarySpool::Init();

   pDesc->setApplicationID("The Frying Pan - Amiga CD and DVD recording software");
   DOS->VPrintf("ISO ID: %32s\n\n", ARRAY((IPTR)pDesc->getApplicationID()));

   DOS->VPrintf("731 / 721 size: %ld / %ld\n", ARRAY(sizeof(val731), sizeof(val721)));   
   DOS->VPrintf("733 / 723 size: %ld / %ld\n", ARRAY(sizeof(val733), sizeof(val723)));  
   DOS->VPrintf("CString / WString size: %ld / %ld\n", ARRAY(sizeof(ISOString<4, 0>), sizeof(ISOWString<4, 0>)));
   DOS->VPrintf("ISO Dir Record size: %ld bytes\n", ARRAY(sizeof(ISODirRecord)));
   DOS->VPrintf("ISO Basic Descriptor size: %ld bytes\n", ARRAY(sizeof(ISOBasicDescriptor)));
   DOS->VPrintf("ISO Boot Descriptor size: %ld bytes\n", ARRAY(sizeof(ISOBootRecordDescriptor)));
   DOS->VPrintf("ISO Primary Descriptor size: %ld bytes\n", ARRAY(sizeof(ISOPrimaryVolumeDescriptor)));
   DOS->VPrintf("ISO Supplementary Descriptor size: %ld bytes\n", ARRAY(sizeof(ISOSupplementaryVolumeDescriptor)));
   DOS->VPrintf("ISO Volume Partition Descriptor size: %ld bytes\n", ARRAY(sizeof(ISOVolumePartitionDescriptor)));
  
   delete pDesc;

   rda = DOS->ReadArgs(Args::Template(), (IPTR *)&args, 0);

   if (NULL != rda)
   {
      pMain = new Main();
      pISO  = new ClISO();

      pISO->addDataHook(pMain->getHook());
      
      pISO->getCurrDir()->scanDirectory(args.dir);
      DOS->VPrintf("Scan complete. Starting data flow...\n", 0);
      pISO->startDataFlow();

      delete pISO;
      delete pMain;

      DOS->FreeArgs(rda);
   }

   LibrarySpool::Exit();
   return 0;
}
