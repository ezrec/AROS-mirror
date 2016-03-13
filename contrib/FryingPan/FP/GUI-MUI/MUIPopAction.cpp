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

#include "MUIPopAction.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/LibrarySpool.h>
#include <proto/intuition.h>
#include <libraries/asl.h>
#include <proto/muimaster.h>
#include <Generic/String.h>
#include <Optical/Optical.h>
#include <FP/IEngine.h>

#undef String

/*
 * localization area
 */
enum Loc
{
   loc_QuickAction         =  le_PopAction,
   loc_MediaInfo,
   loc_DiscType,
   loc_DiscSize,
   loc_Actions,
   loc_Erase,
   loc_Eject,
   loc_Load
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_QuickAction,     "Quick Action",      "LBL_QACTION"   },
   {  loc_MediaInfo,       "Disc Info",         "LBL_MEDIAINFO" },
   {  loc_DiscType,        "Disc Type",         "LBL_DISCTYPE"  },
   {  loc_DiscSize,        "Disc Size",         "LBL_DISCSIZE"  },
   {  loc_Actions,         "Actions",           "LBL_ACTIONS"   },
   {  loc_Erase,           "Erase",             "BTN_ERASE"     },
   {  loc_Eject,           "Eject",             "BTN_EJECT"     },
   {  loc_Load,            "Load",              "BTN_LOAD"      },
   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "ELEM_POPACTION";


MUIPopAction::MUIPopAction(Globals &glb) :
   MUIPopup(""),
   Glb(glb)
{
   Glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
   hHkBtnHook.Initialize(this, &MUIPopAction::buttonHandler);
   setButtonCallBack(hHkBtnHook.GetHook());
}

MUIPopAction::~MUIPopAction()
{
}

bool MUIPopAction::onOpen()
{
   update();
   return true;
}

bool MUIPopAction::onClose()
{
   return true;
}

IPTR MUIPopAction::getPopDisplay()
{
   return (IPTR)RectangleObject,
      MUIA_FixWidth,  0,
      MUIA_FixHeight, 0,
      MUIA_Weight,    0,
   End;
}

IPTR MUIPopAction::getPopButton()
{
   return (IPTR)PopButton(MUII_PopUp);
}

IPTR MUIPopAction::getPopObject()
{
   return (IPTR)VGroup,
      GroupFrameT(Glb.Loc[loc_QuickAction].Data()),
      Child,            ColGroup(2),
         GroupFrameT(Glb.Loc[loc_MediaInfo].Data()),
         Child,            muiLabel(Glb.Loc[loc_DiscType], 0, ID_Default, Align_Right),
         Child,            muiLabel("", 0, ID_MediaType, Align_Left),
         Child,            muiLabel(Glb.Loc[loc_DiscSize], 0, ID_Default, Align_Right),
         Child,            muiLabel("", 0, ID_DiscSize, Align_Left),
      End,

      Child,            VGroup,
         GroupFrameT(Glb.Loc[loc_Actions].Data()),
         Child,            muiButton(Glb.Loc[loc_Erase], 0, ID_PrepareWrite),
         Child,            HGroup,
            Child,            muiButton(Glb.Loc[loc_Eject], 0, ID_Eject),
            Child,            muiButton(Glb.Loc[loc_Load], 0, ID_Load),
         End,
      End,
   End;
}

void MUIPopAction::setValue(IPTR)
{
}

IPTR MUIPopAction::getValue()
{
   return 0;
}

IPTR MUIPopAction::buttonHandler(IPTR id, IPTR data)
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   switch (id)
   {
      case ID_PrepareWrite:
         pEng->structureDisc();
         break;

      case ID_Eject:
         pEng->ejectTray();
         break;

      case ID_Load:
         pEng->loadTray();
         break;
   }

   Glb.CurrentEngine->Release();
   return 0;
}

String MUIPopAction::discType()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   String   s;

   switch (pEng->getDiscType())
   {
      case DRT_Profile_Unknown:
         s = "---";
         break;

      case DRT_Profile_NoDisc:
         s = "---";
         break;

      case DRT_Profile_CD_ROM:
         s = "CD-ROM";
         break;

      case DRT_Profile_CD_R:
         s = "CD-R";
         break;

      case DRT_Profile_CD_RW:
         s = "CD-RW";
         break;

      case DRT_Profile_CD_MRW:
         s = "CD-MRW";
         break;

      case DRT_Profile_DVD_ROM:
         s = "DVD-ROM";
         break;

      case DRT_Profile_DVD_MinusR:
         s = "DVD-R";
         break;

      case DRT_Profile_DVD_PlusR:
         s = "DVD+R";
         break;

      case DRT_Profile_DVD_MinusRW_Sequential:
         s = "DVD-RW";
         break;

      case DRT_Profile_DVD_MinusRW_Restricted:
         s = "DVD-RW";
         break;

      case DRT_Profile_DVD_PlusRW:
         s = "DVD+RW";
         break;

      case DRT_Profile_DVD_RAM:
         s = "DVD-RAM";
         break;

      case DRT_Profile_DDCD_ROM:
         s = "DDCD-ROM";
         break;

      case DRT_Profile_DDCD_R:
         s = "DDCD-R";
         break;

      case DRT_Profile_DDCD_RW:
         s = "DD CD-RW";
         break;

      case DRT_Profile_BD_ROM:
         s = "BD-ROM";
         break;

      case DRT_Profile_BD_R_Sequential:
         s = "BD-R";
         break;

      case DRT_Profile_BD_R_RandomWrite:
         s = "BD-R";
         break;

      case DRT_Profile_BD_RW:
         s = "BD-RW";
         break;

   }

   Glb.CurrentEngine->Release();
   return s;
}
   
String MUIPopAction::discSize()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   int32 size = 0;
   int32 fsize = 0;
   String pfx = "kB";

   String   s = "-";

   if (pEng != NULL)
   {
      size = pEng->getDiscSize();

      size <<= 1;

      if (size > 2000)
      {
         pfx = "MB";
         fsize = (100 * (size & 1023)) >> 10;
         size >>= 10;
      }
      if (size > 2000)
      {
         pfx = "GB";
         fsize = (100 * (size & 1023)) >> 10;
         size >>= 10;
      }

      s = Glb.Loc.FormatNumber(size, fsize*10000) + pfx;
   }

   Glb.CurrentEngine->Release();
   return s;
}

void MUIPopAction::update()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();

   muiSetText(ID_MediaType, discType());
   muiSetText(ID_DiscSize, discSize());

   if (pEng->isErasable() || pEng->isFormatable())
      muiSetEnabled(ID_PrepareWrite, true);
   else
      muiSetEnabled(ID_PrepareWrite, false);

   Glb.CurrentEngine->Release();
}
