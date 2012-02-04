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

#include "JobISOAddItem.h"
#include <ISOBuilder/IBrowser.h>

JobISOAddItem::JobISOAddItem(unsigned long drv, IBrowser* b, String item) :
   Job(drv)
{
   iso = b;
   name = item;
   dir = b->getCurrDir();

   action = "Adding "+name;
}

JobISOAddItem::~JobISOAddItem()
{
}

void JobISOAddItem::execute()
{
   dir->addChild(name);
}

unsigned long JobISOAddItem::getProgress()
{
   return 0;
}

const char *JobISOAddItem::getActionName()
{
   return action.Data();
}

bool JobISOAddItem::inhibitDOS()
{
   return false;
}

