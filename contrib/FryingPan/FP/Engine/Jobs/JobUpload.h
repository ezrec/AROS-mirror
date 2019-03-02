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

#ifndef _ENGINE_JOBS_JOBUPLOAD_H_
#define _ENGINE_JOBS_JOBUPLOAD_H_

#include "Job.h"
#include <DTLib/IFileWriter.h>
#include <DTLib/IRegHook.h>
#include <Optical/IOptItem.h>
#include <Generic/VectorT.h>
#include <FP/ITrack.h>
#include <ISOBuilder/IBrowser.h>
#include <Generic/HookT.h>

class JobUpload : public Job
{
   IOptItem               *disc;
   IOptItem               *session;
   VectorT<ITrack*>        tracks;
   VectorT<IOptItem*>      items;
   unsigned long long      numBlocks;
   unsigned long long      currBlock;
   unsigned short          currTrack;
   bool                    finalize;
   bool                    masterize;
   String                  operation;

protected:
   HookT<JobUpload, void*, int>  hHkData;

protected:
   virtual uint          onData(void* mem, int sectors);
public:
                           JobUpload(unsigned long drive, VectorT<ITrack*> &tracks, bool masterize, bool closedisc);
   virtual                ~JobUpload();                              
   virtual void            execute();
   virtual unsigned long   getProgress();
   virtual const char     *getActionName();
   virtual bool            inhibitDOS();
};

#endif

