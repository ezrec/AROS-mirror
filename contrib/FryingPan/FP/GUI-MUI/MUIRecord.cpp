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

#include "MUIRecord.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/ConfigParser.h>
#include <FP/IEngine.h>
#include "Globals.h"

/*
 * localization area
 */
enum Loc
{
   loc_WriteOptions     = lg_Write,
   loc_MasterizeCD,
   loc_CloseDisc,
   loc_BlankBeforeWrite,
   loc_EjectAfterWrite,
   loc_WriteSpeed,
   loc_Operation,
   loc_Write
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_WriteOptions,    "Write Options",        "LBL_WRITEOPTS"      },
   {  loc_MasterizeCD,     "m&Masterize CD",       "LBL_MASTERIZECD"    },
   {  loc_CloseDisc,       "f&Finalize Disc",      "LBL_FINALIZEDISC"   },
   {  loc_BlankBeforeWrite,"b&Blank before Write", "LBL_BLANKBEFORE"    },
   {  loc_EjectAfterWrite, "e&Eject after Write",  "LBL_EJECTAFTER"     },
   {  loc_WriteSpeed,      "s&Write Speed:",       "LBL_WRITESPEED"     },
   {  loc_Operation,       "Operation",            "LBL_OPERATION"      },
   {  loc_Write,           "w&Write",              "BTN_WRITE"          },

   {  Localization::LocaleSet::Set_Last, 0, 0                     }
};

static const char* LocaleGroup = "WRITE";



MUIRecord::MUIRecord(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all = 0;
   speedselect = 0;

   Config = new ConfigParser(parent, "PageRecord", 0);

   hBtnHook.Initialize(this, &MUIRecord::btnClicked);
   setButtonCallBack(hBtnHook.GetHook());

   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);
}

MUIRecord::~MUIRecord()
{
   Config->setValue("Masterize", do_masterize);
   Config->setValue("CloseDisc", do_closedisc);
   Config->setValue("BlankWrite", do_blankwrite);
   Config->setValue("PostEject", do_posteject);
   delete Config;
}

bool MUIRecord::start()
{
   do_masterize = Config->getValue("Masterize", (long)false);
   do_closedisc = Config->getValue("CloseDisc", (long)false);
   do_blankwrite = Config->getValue("BlankWrite", (long)false);
   do_posteject = Config->getValue("PostEject", (long)true);

   muiSetSelected(ID_Masterize, do_masterize);
   muiSetSelected(ID_CloseDisc, do_closedisc);
   muiSetSelected(ID_BlankWrite, do_blankwrite);
   muiSetSelected(ID_PostEject, do_posteject);
   
   Glb.WriteSelection << this;

   return true;
}

void MUIRecord::stop()
{
   Glb.WriteSelection >> this;

   delete speedselect;
   speedselect = 0;
   all = 0;
}

IPTR MUIRecord::getObject()
{
   if (0 != all)
      return all;

   speedselect = new MUISpeedSelect(Glb);
   speedselect->setCallbackHook(hBtnHook.GetHook());
   speedselect->setID(ID_SetWriteSpeed);

   all = (IPTR)VGroup,
      GroupFrame,
      Child,                  muiSpace(),

      Child,                  VGroup,
         GroupFrameT(Glb.Loc[loc_WriteOptions].Data()),

         Child,                  ColGroup(2),
            Child,                  muiCheckBox(Glb.Loc[loc_MasterizeCD], Glb.Loc.Accel(loc_MasterizeCD), ID_Masterize, Align_Right,  false),
            Child,                  muiCheckBox(Glb.Loc[loc_CloseDisc], Glb.Loc.Accel(loc_CloseDisc), ID_CloseDisc, Align_Left,   false),
         End,

         Child,                  ColGroup(2),
            Child,                  muiCheckBox(Glb.Loc[loc_BlankBeforeWrite], Glb.Loc.Accel(loc_BlankBeforeWrite), ID_BlankWrite, Align_Right, false),
            Child,                  muiCheckBox(Glb.Loc[loc_EjectAfterWrite], Glb.Loc.Accel(loc_EjectAfterWrite), ID_PostEject,  Align_Left,  false),
         End,

         Child,                  muiBar(),

         Child,                  HGroup,
            Child,                  muiLabel(Glb.Loc[loc_WriteSpeed], 0, ID_Default, Align_Right),
            Child,                  speedselect->getObject(),
         End,
      End,

      Child,                  VGroup,
         GroupFrameT(Glb.Loc[loc_Operation].Data()),

//         Child,                  muiGauge("Current track: No Op"),
//         Child,                  muiGauge("Current disc: No Op"),
//         Child,                  muiBar(),
         Child,                  muiButton(Glb.Loc[loc_Write], Glb.Loc.Accel(loc_Write), ID_Write),
      End,

      Child,                  muiSpace(),

   End;

   return all;
}

IPTR MUIRecord::btnClicked(BtnID button, IPTR state)
{
   switch (button)
   {
      case ID_Masterize:
         do_masterize = state;
         muiSetEnabled(ID_CloseDisc, !state);
         muiSetSelected(ID_CloseDisc, true);
         do_closedisc = true;
         break;

      case ID_CloseDisc:
         do_closedisc = state;
         break;

      case ID_Write:
         {
            IEngine * eng = Glb.CurrentEngine->ObtainRead();
            
            if (do_blankwrite && eng->isErasable())
               eng->structureDisc();

            if (Glb.WriteSelection == Globals::Select_Tracks)
               eng->recordDisc(eng->isDVD() ? false : do_masterize, eng->isDVD() ? false : do_closedisc);

            else if (Glb.WriteSelection == Globals::Select_Session)
               eng->recordSession();

            if (do_posteject)
               eng->ejectTray();

            Glb.CurrentEngine->Release();
         }
         break;

      case ID_SetWriteSpeed:
         {
            IEngine * eng = Glb.CurrentEngine->ObtainRead();
            eng->setWriteSpeed(state);
            Glb.CurrentEngine->Release();
         }
         break;

      case ID_BlankWrite:
         {
            do_blankwrite = state;
         }
         break;

      case ID_PostEject:
         {
            do_posteject = state;
         }
         break;
   }

   return 0;
}

void MUIRecord::onPropChange(PropertyT<Globals::eWriteSelect>* p, const Globals::eWriteSelect &s)
{
   if (p == &Glb.WriteSelection)
   {
      if (s == Globals::Select_Tracks)
      {
         muiSetEnabled(ID_Masterize, true);
         muiSetEnabled(ID_CloseDisc, true);
      }
      else
      {
         muiSetEnabled(ID_Masterize, false);
         muiSetEnabled(ID_CloseDisc, false);
      }
   }
}

void MUIRecord::update()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();

   /*
    * do not alter the state of the buttons here, instead - filter options out before write
    */
   if (pEng->isDVD())
   {
      muiSetEnabled(ID_Masterize, false);
      muiSetEnabled(ID_CloseDisc, false);
   }
   else
   {
      muiSetEnabled(ID_Masterize, true);
      muiSetEnabled(ID_CloseDisc, true);
   }

   if (pEng != 0)
   {
      DiscSpeed *speeds = pEng->getWriteSpeeds();
      int32 speed = pEng->getWriteSpeed();

      speedselect->setSpeeds(speeds, speed);
   }
   else
      speedselect->setSpeeds(0, 0);

   Glb.CurrentEngine->Release();
}


