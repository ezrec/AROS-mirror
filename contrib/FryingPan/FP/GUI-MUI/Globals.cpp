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

#include "Globals.h"
#include <Generic/Locale.h>

using namespace GenNS;

class MUIMasterIFace *MUIMaster;
class DatatypesIFace *Datatypes;

static class Localization::LocaleSet LocaleSets[] =
{
   {  Globals::loc_Req,             "Request",        "REQUEST"      },
   {  Globals::loc_Info,            "Information",    "INFORMATION"  },
   {  Globals::loc_Warn,            "Warning",        "WARNING"      },
   {  Globals::loc_Error,           "Error",          "ERROR"        },

   {  Globals::loc_OK,              "OK",             "OK"           },
   {  Globals::loc_Proceed,         "Proceed",        "PROCEED"      },
   {  Globals::loc_Abort,           "Abort",          "ABORT"        },
   {  Globals::loc_YesNo,           "Yes|No",         "YESNO"        },
   {  Globals::loc_ContinueAbort,   "Continue|Abort", "CONTINUEABORT"},
   {  Localization::LocaleSet::Set_Last, 0, 0                        }
};

static const char* LocaleGroup = "GLOBAL";

Globals::Globals()
{
   Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}
