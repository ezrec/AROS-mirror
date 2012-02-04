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

#ifndef _ENGINE_JOBS_JOBISOREMITEMS_H_
#define _ENGINE_JOBS_JOBISOREMITEMS_H_

#include "Job.h"
#include <Generic/String.h>


class JobISORemItem : public Job
{
protected:
   class IBrowser         *iso;     // iso name
   class ClElement        *elem;    // item to be removed
   class ClDirectory      *dir;     // parent directory that will include files.
   String                  action;
public:
                           JobISORemItem(unsigned long drv, IBrowser* b, ClElement *elem);
   virtual                ~JobISORemItem();
   virtual void            execute();
   virtual unsigned long   getProgress();
   virtual const char     *getActionName();
   virtual bool            inhibitDOS();
};

#endif

