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

#include "JobCreateISO.h"

JobCreateISO::JobCreateISO(uint32 drive, IBrowser *browser, IRegHook *module, const char* name) :
   Job(drive)
{
   this->browser  = browser;
   this->name     = name;
   this->writer   = 0;
   this->hook     = module;
   hHkCallback.Initialize(this, &JobCreateISO::dataCallback);
}

JobCreateISO::~JobCreateISO()
{
}

void JobCreateISO::execute()
{
   EDtError rc = DT_OK;

   ASSERT(NULL != hook);
   if (NULL == hook)
      return;

   ASSERT(NULL != browser)
   if (NULL == browser)
      return;

   writer = hook->openWrite(name.Data(), 0, rc);

   if (NULL != writer)
   {
      numBlocks = browser->validate();
      currBlock = 0;
      writer->setBlockSize(2048);
      writer->setBlockCount(numBlocks);
      writer->setUp();
      
      browser->addDataHook(hHkCallback.GetHook());
      browser->startDataFlow();
      browser->remDataHook(hHkCallback.GetHook());

      writer->cleanUp();
      writer->dispose();
   }
}

unsigned long JobCreateISO::dataCallback(void* src, long len)
{
   if (len > 0)
   {
      writer->writeData(src, len);
      currBlock += len;
   }

   return true;
}

unsigned long JobCreateISO::getProgress()
{
   unsigned long long s1, s2;

   s2 = numBlocks;
   s1 = currBlock;
   while (s2 > 65536)
   {
      s2 >>= 1;
      s1 >>= 1;
   }

   return ((long)s1 * 65535) / (long)s2;
}

const char *JobCreateISO::getActionName()
{
   return "Creating ISO Image";
}

bool JobCreateISO::inhibitDOS()
{
   return false;
}
