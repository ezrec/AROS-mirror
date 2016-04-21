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

#include "MUIMedia.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/ConfigParser.h>
#include "Globals.h"
#include "../IEngine.h"

static const char *Cfg_PrepMethod = "PrepType";

/*
 * localization area
 */
enum Loc
{
   loc_MediumType             = lg_Media,
   loc_MediumSubType,
   loc_MediumVendor,
   loc_MediumState,
   loc_MediumContents,
   loc_MediumID,
   loc_MediumBarCode,
   loc_MediumMCN,
   loc_MediumSize,
   loc_MediumReadSpeeds,
   loc_MediumWriteSpeeds,
   loc_BtnUpdate,
   loc_BtnPrepare,
   loc_BtnErase,
   loc_BtnFormat,
   loc_BtnCloseSession,
   loc_BtnCloseDisc,
   loc_BtnCloseTracks,
   loc_BtnRepairDisc,
   loc_BtnEject,
   loc_BtnLoad,

   loc_Action_Quick           = lg_Media + ls_Group1,
   loc_Action_Complete,

   loc_TypeUnknown            = lg_Media + ls_Group2,
   loc_TypeNoDisc,
   loc_TypeCDQual1,
   loc_TypeCDQual2,
   loc_TypeCDQual3,
   loc_TypeCDQual4,
   loc_TypeCDQual5,
   loc_TypeCDQual6,
   loc_TypeCDQual7,
   loc_TypeCDQual8,
   loc_TypeCDQual9,
   loc_TypeCDRWLowSpeed,
   loc_TypeCDRWHighSpeed,
   loc_TypeCDRWUltraSpeed,
   loc_TypeDVDDualLayer,

   loc_TextContents           = lg_Media + ls_Group3,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_MediumType,         "Disc Type:",        "LBL_TYPE"           },
   {  loc_MediumSubType,      "Disc Sub Type:",    "LBL_SUBTYPE"        },
   {  loc_MediumVendor,       "Vendor:",           "LBL_VENDOR"         },
   {  loc_MediumState,        "Disc State:",       "LBL_STATE"          },
   {  loc_MediumContents,     "Contents:",         "LBL_CONTENTS"       },
   {  loc_MediumID,           "Disc ID:",          "LBL_DISC_ID"        },
   {  loc_MediumBarCode,      "Bar Code:",         "LBL_BAR_CODE"       },
   {  loc_MediumMCN,          "Catalog Number:",   "LBL_MCN"            },
   {  loc_MediumSize,         "Size:",             "LBL_SIZE"           },
   {  loc_MediumReadSpeeds,   "Read Speeds:",      "LBL_READ_SPEEDS"    },
   {  loc_MediumWriteSpeeds,  "Write Speeds:",     "LBL_WRITE_SPEEDS"   },

   {  loc_BtnUpdate,          "u&Update",          "BTN_UPDATE"         },
   {  loc_BtnPrepare,         "p&Prepare for Write","BTN_PREPARE"       },
   {  loc_BtnErase,           "a&Erase",           "BTN_ERASE"          },
   {  loc_BtnFormat,          "f&Format",          "BTN_FORMAT"         },
   {  loc_BtnCloseSession,    "c&Close Session",   "BTN_CLOSE_SESSION"  },
   {  loc_BtnCloseDisc,       "d&Close Disc",      "BTN_CLOSE_DISC"     },
   {  loc_BtnCloseTracks,     "t&Close Tracks",    "BTN_CLOSE_TRACKS"   },
   {  loc_BtnRepairDisc,      "r&Repair Disc",     "BTN_REPAIR_DISC"    },
   {  loc_BtnEject,           "e&Eject",           "BTN_EJECT"          },
   {  loc_BtnLoad,            "l&Load",            "BTN_LOAD"           },

   {  loc_Action_Quick,       "Quick",             "CCL_QUICK"          },
   {  loc_Action_Complete,    "Complete",          "CCL_COMPLETE"       },

   {  loc_TypeUnknown,        "Unknown",           "LBL_TYPE_UNKNOWN"   },
   {  loc_TypeNoDisc,         "No Disc",           "LBL_TYPE_NODISC"    },
   {  loc_TypeCDQual1,        "Best Quality",      "LBL_TYPE_QUALITY1"  },
   {  loc_TypeCDQual2,        "Fine Quality",      "LBL_TYPE_QUALITY2"  },
   {  loc_TypeCDQual3,        "Good Quality",      "LBL_TYPE_QUALITY3"  },
   {  loc_TypeCDQual4,        "Average Quality",   "LBL_TYPE_QUALITY4"  },
   {  loc_TypeCDQual5,        "Medium Quality",    "LBL_TYPE_QUALITY5"  },
   {  loc_TypeCDQual6,        "Low Quality",       "LBL_TYPE_QUALITY6"  },
   {  loc_TypeCDQual7,        "Poor Quality",      "LBL_TYPE_QUALITY7"  },
   {  loc_TypeCDQual8,        "Bad Quality",       "LBL_TYPE_QUALITY8"  },
   {  loc_TypeCDQual9,        "Worst Quality",     "LBL_TYPE_QUALITY9"  },

   {  loc_TypeCDRWLowSpeed,   "Low Speed",         "LBL_TYPE_LOWSPEED"  },
   {  loc_TypeCDRWHighSpeed,  "High Speed",        "LBL_TYPE_HIGHSPEED" },
   {  loc_TypeCDRWUltraSpeed, "Ultra Speed",       "LBL_TYPE_ULTRASPEED"},
   
   {  loc_TypeDVDDualLayer,   "Dual Layer",        "LBL_TYPE_DUALLAYER" },

   {  loc_TextContents,       "%ld Track(s) in %ld Session(s)",   "LBL_CONTENTS_TEXT"  },

   {  Localization::LocaleSet::Set_Last, 0, 0                     }
};

static const char* LocaleGroup = "MEDIA";


MUIMedia::MUIMedia(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all = 0;

   Config = new ConfigParser(parent, "PageMedia", 0);

   preptype = Config->getValue(Cfg_PrepMethod, (long)0);

   hBtnHook.Initialize(this, &MUIMedia::buttonClicked);
   setButtonCallBack(hBtnHook.GetHook());
   
   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);
}

MUIMedia::~MUIMedia()
{
   Config->setValue(Cfg_PrepMethod, preptype);
   delete Config;
}

bool MUIMedia::start()
{
   return true;
}

void MUIMedia::stop()
{
}

IPTR MUIMedia::getObject()
{
   if (0 != all)
      return all;

   static const char* preptypes[3];
   // previously we initialized static array with dynamic values. obviously that wouldn't work.
   preptypes[0] = Glb.Loc[loc_Action_Quick].Data();
   preptypes[1] = Glb.Loc[loc_Action_Complete].Data();
   preptypes[2] = 0;

   all = (IPTR)VGroup,
      GroupFrame,
      Child,                  RectangleObject,
         MUIA_Weight,            1,
      End,

      Child,                  ColGroup(2),
         MUIA_Group_SameSize,    true,
         Child,                  muiLabel(Glb.Loc[loc_MediumType], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_DiscType, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumSubType], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_SubType, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumVendor], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_Vendor, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumState], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_State, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumContents], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_Contents, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumID], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_DiscID, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumBarCode], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_BarCode, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumMCN], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_Catalog, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumSize], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_DiscSize, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumReadSpeeds], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_ReadSpeeds, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_MediumWriteSpeeds], 0, ID_Default, Align_Right),
         Child,                  muiLabel("---", 0, ID_WriteSpeeds, Align_Left),
      End,
      
      Child,                  RectangleObject,
         MUIA_Weight,            1,
      End,

      Child,                  muiButton(Glb.Loc[loc_BtnUpdate], Glb.Loc.Accel(loc_BtnUpdate), ID_Refresh),

      Child,                  ColGroup(3),
         Child,                  muiCycle(preptypes, 0, ID_SetPrepType, preptype),
         Child,                  muiButton(Glb.Loc[loc_BtnErase],  Glb.Loc.Accel(loc_BtnErase), ID_EraseDisc),
         Child,                  muiButton(Glb.Loc[loc_BtnFormat], Glb.Loc.Accel(loc_BtnFormat), ID_FormatDisc),
      End,

      Child,                  muiButton(Glb.Loc[loc_BtnPrepare], Glb.Loc.Accel(loc_BtnPrepare), ID_PrepareDisc),

      Child,                  ColGroup(4),
         Child,                  muiButton(Glb.Loc[loc_BtnCloseSession], Glb.Loc.Accel(loc_BtnCloseSession), ID_CloseSession),
         Child,                  muiButton(Glb.Loc[loc_BtnCloseDisc], Glb.Loc.Accel(loc_BtnCloseDisc), ID_CloseDisc),
         Child,                  muiButton(Glb.Loc[loc_BtnCloseTracks], Glb.Loc.Accel(loc_BtnCloseTracks), ID_CloseTracks),
         Child,                  muiButton(Glb.Loc[loc_BtnRepairDisc], Glb.Loc.Accel(loc_BtnRepairDisc), ID_Repair),
      End,

      Child,                  ColGroup(2),
         Child,                  muiButton(Glb.Loc[loc_BtnEject], Glb.Loc.Accel(loc_BtnEject), ID_Eject),
         Child,                  muiButton(Glb.Loc[loc_BtnLoad], Glb.Loc.Accel(loc_BtnLoad), ID_Load),
      End,

   End;

   return all;
}

IPTR MUIMedia::buttonClicked(BtnID id, IPTR state)
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   switch (id)
   {
      case ID_Refresh:
         {
            if (pEng != NULL)
               pEng->updateAll();
         }
         break;

      case ID_SetPrepType:
         {
            preptype = state;
         }
         break;

      case ID_EraseDisc:
         {
            if (preptype == 0)
               pEng->quickErase();
            else
               pEng->completeErase();
         }
         break;

      case ID_FormatDisc:
         {
            if (preptype == 0)
               pEng->quickFormat();
            else
               pEng->completeFormat();
         }
         break;


      case ID_PrepareDisc:
         {
            pEng->structureDisc();
         }
         break;

      case ID_CloseSession:
         {
            pEng->closeSession();
         }
         break;

      case ID_CloseDisc:
         {
            pEng->closeDisc();
         }
         break;

      case ID_CloseTracks:
         {
            pEng->closeTracks();
         }
         break;

      case ID_Repair:
         {
            pEng->repairDisc();
         }
         break;

      case ID_Load:
         {
            pEng->loadTray();
         };
         break;

      case ID_Eject:
         {
            pEng->ejectTray();
         };
         break;

      default:
         break;
   };

   Glb.CurrentEngine->Release();
   return 0;
}

void MUIMedia::update()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();

   if (pEng->isOpened() && pEng->isDiscInserted())
   {
      muiSetText(ID_DiscType,          profileToString());
      muiSetText(ID_SubType,           subTypeToString());
      muiSetText(ID_ReadSpeeds,        speedsToString(pEng->getReadSpeeds()));
      muiSetText(ID_WriteSpeeds,       speedsToString(pEng->getWriteSpeeds()));
      muiSetText(ID_State,             stateToString());
      muiSetText(ID_Contents,          contentsToString());
      muiSetText(ID_DiscSize,          sizeToString());
      muiSetText(ID_Vendor,            pEng->getDiscVendor());
   }
   else
   {
      muiSetText(ID_DiscType,          "---");
      muiSetText(ID_SubType,           "---");
      muiSetText(ID_ReadSpeeds,        "---");
      muiSetText(ID_WriteSpeeds,       "---");
      muiSetText(ID_State,             "---");
      muiSetText(ID_Contents,          "---");
      muiSetText(ID_Vendor,            "---");
      muiSetText(ID_DiscSize,          "---");
   }

   Glb.CurrentEngine->Release();

}

String MUIMedia::stateToString()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   String res = "";

   if (pEng->isRecordable())
      res += "Recordable ";
   if (pEng->isErasable())
      res += "Erasable ";
   if (pEng->isFormatable())
      res += "Formatable ";
   if (pEng->isOverwritable())
      res += "Overwritable ";
   if (res.Length() == 0)
      res += "Read only";

   Glb.CurrentEngine->Release();
   return res;
}

String MUIMedia::profileToString()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   String   s;

   switch (pEng->getDiscType())
   {
      case DRT_Profile_Unknown:
         s = Glb.Loc[loc_TypeUnknown];
         break;

      case DRT_Profile_NoDisc:
         s = Glb.Loc[loc_TypeNoDisc];
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
         s = "DVD-MRW";
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
         s = "DDCD-RW";
         break;

      case DRT_Profile_BD_ROM:
         s = "BD-ROM";
         break;

      case DRT_Profile_BD_R_Sequential:
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

String MUIMedia::subTypeToString()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   String   s;

   s = "Unknown";

   switch (pEng->getDiscType())
   {
      case DRT_Profile_Unknown:
      case DRT_Profile_NoDisc:
         break;

      case DRT_Profile_CD_ROM:
      case DRT_Profile_CD_R:
      case DRT_Profile_DDCD_ROM:
      case DRT_Profile_DDCD_R:
         switch (pEng->getDiscSubType())
         {
            case DRT_SubType_Unknown:
               s = Glb.Loc[loc_TypeUnknown];
               break;

            case DRT_SubType_CD_R_Quality1:
               s = Glb.Loc[loc_TypeCDQual1];
               break;

            case DRT_SubType_CD_R_Quality2:
               s = Glb.Loc[loc_TypeCDQual2];
               break;

            case DRT_SubType_CD_R_Quality3:
               s = Glb.Loc[loc_TypeCDQual3];
               break;

            case DRT_SubType_CD_R_Quality4:
               s = Glb.Loc[loc_TypeCDQual4];
               break;

            case DRT_SubType_CD_R_Quality5:
               s = Glb.Loc[loc_TypeCDQual5];
               break;

            case DRT_SubType_CD_R_Quality6:
               s = Glb.Loc[loc_TypeCDQual6];
               break;

            case DRT_SubType_CD_R_Quality7:
               s = Glb.Loc[loc_TypeCDQual7];
               break;

            case DRT_SubType_CD_R_Quality8:
               s = Glb.Loc[loc_TypeCDQual8];
               break;

            case DRT_SubType_CD_R_Quality9:
               s = Glb.Loc[loc_TypeCDQual9];
               break;
            
            default:
               break;
         }
         break;

      case DRT_Profile_CD_RW:
      case DRT_Profile_DDCD_RW:
      case DRT_Profile_CD_MRW:
         switch (pEng->getDiscSubType())
         {
            case DRT_SubType_Unknown:
               s = Glb.Loc[loc_TypeUnknown];
               break;

            case DRT_SubType_CD_RW_LowSpeed:
               s = Glb.Loc[loc_TypeCDRWLowSpeed];
               break;

            case DRT_SubType_CD_RW_HighSpeed:
               s = Glb.Loc[loc_TypeCDRWHighSpeed];
               break;

            case DRT_SubType_CD_RW_UltraSpeed:
               s = Glb.Loc[loc_TypeCDRWUltraSpeed];
               break;

            default:
               break;
         }
         break;

      case DRT_Profile_DVD_ROM:
      case DRT_Profile_DVD_MinusR:
      case DRT_Profile_DVD_PlusR:
      case DRT_Profile_DVD_MinusRW_Sequential:
      case DRT_Profile_DVD_MinusRW_Restricted:
      case DRT_Profile_DVD_PlusRW:
      case DRT_Profile_DVD_RAM:
         switch (pEng->getDiscSubType())
         {
            case DRT_SubType_Unknown:
               s = Glb.Loc[loc_TypeUnknown];
               break;

            case DRT_SubType_DVD_DualLayer:
               s = Glb.Loc[loc_TypeDVDDualLayer];
               break;

            default:
               break;
         }
         break;


      case DRT_Profile_BD_ROM:
      case DRT_Profile_BD_R_Sequential:
      case DRT_Profile_BD_R_RandomWrite:
      case DRT_Profile_BD_RW:
         s = "Hey, is this really a BD? Cool!";
         break;

   }

   Glb.CurrentEngine->Release();
   return s;
}

String MUIMedia::contentsToString()
{
   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   const IOptItem *oi = pEng->getContents();
   int32 trks = 0;
   int32 sess = 0;

   String   s;

   if (oi == 0)
   {
      s = Glb.Loc[loc_TypeNoDisc];
   }
   else if (oi->getChildCount() > 0)
   {
      oi = oi->getChild(oi->getChildCount()-1);
      sess = oi->getItemNumber();
      trks = oi->getChild(oi->getChildCount()-1)->getItemNumber();
      s.FormatStr(Glb.Loc[loc_TextContents], ARRAY(trks, sess));
   }

   Glb.CurrentEngine->Release();
   return s;
}

String MUIMedia::sizeToString()
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
         fsize = size;
         size >>= 10;
      }
      if (size > 2000)
      {
         pfx = "GB";
         fsize = size;
         size >>= 10;
      }


      s.FormatStr("%ld.%02ld%s", ARRAY(
               size,
               (IPTR)((fsize * 100) >> 10) % 100,
               (IPTR)pfx.Data()
      ));
   }

   Glb.CurrentEngine->Release();
   return s;
}

String MUIMedia::speedsToString(DiscSpeed *speeds)
{
   String   s     = "";
   String   t;
   int      idx;

   if (speeds == 0)
      return s;

   for (idx=0; speeds[idx].kbps != 0; idx++)
   {
      t = Glb.Loc.FormatNumber(speeds[idx].i, speeds[idx].f * 100000) + "x";
      if (s.Length() > 0)
         s += ", ";
      s += t;
   }
   
   return s;
}



