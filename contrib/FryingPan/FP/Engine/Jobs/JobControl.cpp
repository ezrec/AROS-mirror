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

#include "JobControl.h"

JobControl::JobControl(unsigned long drive, DRT_Control action) :
   Job(drive)
{
   eAction = action;
}

void JobControl::execute()
{
   pOptical->OptDoMethodA(ARRAY(DRV_ControlUnit, Drive, eAction));
}

unsigned long JobControl::getProgress()
{
   return 0;
}

const char *JobControl::getActionName()
{
   switch (eAction)
   {
      case DRT_Unit_Stop:
         return "Spinning down";
        
      case DRT_Unit_Start:
         return "Spinning up";

      case DRT_Unit_Eject:
         return "Ejecting";

      case DRT_Unit_Load:
         return "Loading";

      case DRT_Unit_Idle:
         return "Holding drive";

      case DRT_Unit_Standby:
         return "Suspending drive";

      case DRT_Unit_Sleep:
         return "Drive Power Off";
   };
   return "Unknown Job";
}

bool JobControl::inhibitDOS()
{
   return false;
}

