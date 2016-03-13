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

#include "MUIDrive.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/ConfigParser.h>
#include "Globals.h"
#include <Generic/Set.h>

/*
 * localization area
 */
enum Loc
{
   loc_DrivePopDevice      =  lg_Drive,
   loc_DrivePopUnit, 
   loc_DriveVendor,
   loc_DriveProduct,
   loc_DriveVersion,
   loc_DriveMechanism,
   loc_DriveReadsMedia,
   loc_DriveWritesMedia,
   loc_DriveWritesData,
   loc_DriveBurnProof,
   loc_DriveISRC,
   loc_DriveCatalogNumber,
   loc_DriveAudioStream,
   loc_DriveTestMode,
   loc_DriveMultiSession,
   loc_DriveCDText,
   loc_DriveReadSpeeds,
   loc_DriveWriteSpeeds,
   loc_DriveStatus,
   loc_DriveCDAudio,

   loc_Supported           =  lg_Drive+ls_Group1,
   loc_Unsupported,
   loc_Accurate,
   loc_Jittered,
   loc_Registered,
   loc_Unregistered,

   loc_MechCaddy           =  lg_Drive+ls_Group2,
   loc_MechTray,
   loc_MechPopup,
   loc_MechChanger,
   loc_MechMagazine,
   loc_MechUnknown,

   loc_ColDevDevices       =  lg_Drive+ls_Group3,

   loc_ColUnitNumber       =  lg_Drive+ls_Group4,
   loc_ColUnitVendor,
   loc_ColUnitDrive,

   loc_DriveNotOpened      =  lg_Drive+ls_Req,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_DrivePopDevice,        "Device:",                 "POP_DEVICE"      },
   {  loc_DrivePopUnit,          "Unit:",                   "POP_UNIT"        },
   {  loc_DriveVendor,           "Vendor:",                 "LBL_VENDOR"      },
   {  loc_DriveProduct,          "Product:",                "LBL_PRODUCT"     },
   {  loc_DriveVersion,          "Version:",                "LBL_VERSION"     },
   {  loc_DriveMechanism,        "Mechanism:",              "LBL_ECHANISM"    },
   {  loc_DriveReadsMedia,       "Reads Media:",            "LBL_READSMEDIA"  },
   {  loc_DriveWritesMedia,      "Writes Media:",           "LBL_WRITESMEDIA" },
   {  loc_DriveWritesData,       "Writes Data:",            "LBL_WRITESDATA"  },
   {  loc_DriveBurnProof,        "Burn Proof:",             "LBL_BURNPROOF"   },
   {  loc_DriveISRC,             "ISRC:",                   "LBL_ISRC"        },
   {  loc_DriveCatalogNumber,    "Media Catalog Number:",   "LBL_MCN"         },
   {  loc_DriveAudioStream,      "Audio Stream:",           "LBL_AUDIOSTREAM" },
   {  loc_DriveTestMode,         "Test Mode:",              "LBL_TESTMODE"    },
   {  loc_DriveMultiSession,     "MultiSession:",           "LBL_MULTISESSION"},
   {  loc_DriveCDText,           "CD-Text:",                "LBL_CDTEXT"      },
   {  loc_DriveReadSpeeds,       "Read Speeds:",            "LBL_READSPEEDS"  },
   {  loc_DriveWriteSpeeds,      "Write Speeds:",           "LBL_WRITESPEEDS" },
   {  loc_DriveStatus,           "Drive Status:",           "LBL_DRIVESTATUS" },
   {  loc_DriveCDAudio,          "CD-Audio Playback:",      "LBL_CDAUDIO"     },

   {  loc_Supported,             "Supported",               "LBL_SUPPORTED"   },
   {  loc_Unsupported,           "Unsupported",             "LBL_UNSUPPORTED" },
   {  loc_Accurate,              "Accurate",                "LBL_ACCURATE"    },
   {  loc_Jittered,              "Jittered",                "LBL_JITTERED"    },
   {  loc_Registered,            "Registered",              "LBL_REGISTERED"  },
   {  loc_Unregistered,          "Unregistered",            "LBL_UNREGISTERED"},

   {  loc_MechCaddy,             "Caddy",                   "LBL_MECH_CADDY"  },
   {  loc_MechTray,              "Tray",                    "LBL_MECH_TRAY"   },
   {  loc_MechPopup,             "Popup",                   "LBL_MECH_POPUP"  },
   {  loc_MechChanger,           "Changer",                 "LBL_MECH_CHANGER"},
   {  loc_MechMagazine,          "Magazine",                "LBL_MECH_MAGAZINE"},
   {  loc_MechUnknown,           "Unknown Mechanism",       "LBL_MECH_UNKNOWN"},

   {  loc_ColDevDevices,         "Devices",                 "COL_DEVICES"     },
   {  loc_ColUnitNumber,         "Unit",                    "COL_UNIT"        },
   {  loc_ColUnitVendor,         "Vendor",                  "COL_VENDOR"      },
   {  loc_ColUnitDrive,          "Drive",                   "COL_DRIVE"       },

   {  loc_DriveNotOpened,
      "Selected drive could not be opened.",
      "REQ_DRIVE_NOT_OPENED"                                                  },

   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "DRIVE";


MUIDrive::MUIDrive(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   _dx(Lvl_Info, "Creating Drive page");
   all = 0;
   popDevice = 0;
   popUnit = 0;

   _dx(Lvl_Info, "Updating localization map");
   Glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);

   _dx(Lvl_Info, "Creating config element");
   Config = new ConfigParser(parent, "PageDrive", 0);

   _dx(Lvl_Info, "Initializing hooks");
   hHkButtonHook.Initialize(this, &MUIDrive::buttonHook);
}

MUIDrive::~MUIDrive()
{
   _dx(Lvl_Info, "Disposing configuration element");
   delete Config;
}

IPTR MUIDrive::getObject()
{
   _dx(Lvl_Info, "Retrieving page content (%08lx)", (IPTR)all);
   if (NULL != all)
      return all;

   _dx(Lvl_Info, "Page not created - building");

   if (popDevice == 0)
   {
      _dx(Lvl_Info, "Setting up PopDevice");
      popDevice = new MUIPopDevice(Glb.Loc[loc_DrivePopDevice], (const char**)ARRAY((IPTR)Glb.Loc[loc_ColDevDevices].Data(), 0));
      popDevice->setID(ID_DeviceSelect);
      popDevice->setCallbackHook(hHkButtonHook.GetHook());
   }

   if (popUnit == 0)
   {
      _dx(Lvl_Info, "Setting up PopUnit");
      popUnit   = new MUIPopUnit(Glb, Glb.Loc[loc_DrivePopUnit], (const char**)ARRAY(
               (IPTR)Glb.Loc[loc_ColUnitNumber].Data(),
               (IPTR)Glb.Loc[loc_ColUnitVendor].Data(),
               (IPTR)Glb.Loc[loc_ColUnitDrive].Data(),
               0));
      popUnit->setID(ID_UnitSelect);
      popUnit->setCallbackHook(hHkButtonHook.GetHook());
   }

   _dx(Lvl_Info, "Building main page");
   all = (IPTR)VGroup,
      GroupFrame,
      Child,                  RectangleObject,
         MUIA_Weight,            1,
      End,

      Child,                  HGroup,
         Child,                  popDevice->getObject(),
         Child,                  popUnit->getObject(),
      End,

      Child,                  HGroup,
         TextFrame,
         Child,                  HGroup,
            Child,                  muiLabel(Glb.Loc[loc_DriveVendor], 0, ID_Default, Align_Right),
            Child,                  vendor = muiLabel("---", 0, ID_DriveVendor, Align_Left),
            MUIA_Weight,            50,
         End,

         Child,                  HGroup,
            Child,                  muiLabel(Glb.Loc[loc_DriveProduct], 0, ID_Default, Align_Right),
            Child,                  product = muiLabel("---", 0, ID_DriveProduct, Align_Left),
            MUIA_Weight,            100,
         End,

         Child,                  HGroup,
            Child,                  muiLabel(Glb.Loc[loc_DriveVersion], 0, ID_Default, Align_Right),
            Child,                  version = muiLabel("---", 0, ID_DriveVersion, Align_Left),
            MUIA_Weight,            50,
         End,
      End,

      Child,                  RectangleObject,
         MUIA_Rectangle_HBar,    true,
         MUIA_FixHeight,         10,
      End,
         

      Child,                  ColGroup(2),
         MUIA_Group_SameSize,    true,
         Child,                  muiLabel(Glb.Loc[loc_DriveMechanism], 0, ID_Default, Align_Right),
         Child,                  mechanism = muiLabel("---", 0, ID_MechanismType, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveReadsMedia], 0, ID_Default, Align_Right),
         Child,                  readmedia = muiLabel("---", 0, ID_ReadableMedia, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveWritesMedia], 0, ID_Default, Align_Right),
         Child,                  writemedia = muiLabel("---", 0, ID_WritableMedia, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveWritesData], 0, ID_Default, Align_Right),
         Child,                  writedata = muiLabel("---", 0, ID_SupportedData, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveBurnProof], 0, ID_Default, Align_Right),
         Child,                  burnproof = muiLabel("---", 0, ID_Burnproof, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveISRC], 0, ID_Default, Align_Right),
         Child,                  isrc = muiLabel("---", 0, ID_ISRC, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveCatalogNumber], 0, ID_Default, Align_Right),
         Child,                  mcn = muiLabel("---", 0, ID_MCN, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveAudioStream], 0, ID_Default, Align_Right),
         Child,                  audiostream = muiLabel("---", 0, ID_AudioStream, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveCDAudio], 0, ID_Default, Align_Right),
         Child,                  audioplayback = muiLabel("---", 0, ID_AudioPlayback, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveTestMode], 0, ID_Default, Align_Right),
         Child,                  testwrite = muiLabel("---", 0, ID_TestMode, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveMultiSession], 0, ID_Default, Align_Right),
         Child,                  multisession = muiLabel("---", 0, ID_Multisession, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveCDText], 0, ID_Default, Align_Right),
         Child,                  cdtext = muiLabel("---", 0, ID_CDText, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveReadSpeeds], 0, ID_Default, Align_Right),
         Child,                  readspeeds = muiLabel("---", 0, ID_ReadSpeeds, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveWriteSpeeds], 0, ID_Default, Align_Right),
         Child,                  writespeeds = muiLabel("---", 0, ID_WriteSpeeds, Align_Left),
         Child,                  muiLabel(Glb.Loc[loc_DriveStatus], 0, ID_Default, Align_Right),
         Child,                  state = muiLabel("---", 0, ID_DriveStatus, Align_Left),
      End,
      
      Child,                  RectangleObject,
         MUIA_Weight,            1,
      End,

   End;

   _dx(Lvl_Info, "Page created (%08lx)", (IPTR)all);
   return all;
}

bool MUIDrive::start()
{
   _dx(Lvl_Info, "All done.");
   return true;
}

void MUIDrive::stop()
{
   _dx(Lvl_Info, "Disposing PopDevice/PopUnit elements");
   if (popDevice != 0)
      delete popDevice;
   popDevice = 0;

   if (popUnit != 0)
      delete popUnit;
   popUnit = 0;

   all = 0;
   _dx(Lvl_Info, "All done.");
}

void MUIDrive::update()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();
   sDevice  = eng->getDevice();
   lUnit    = eng->getUnit();

   _dx(Lvl_Info, "Updating device and unit popups");
   popDevice->setValue((IPTR)sDevice.Data());
   popUnit->setDevice(sDevice.Data());
   popUnit->setValue(lUnit);

   if (eng->isOpened())
   {
      _dx(Lvl_Info, "Unit opened -> updating display");
      unsigned long caps = eng->getCapabilities();
      const char  *a = Glb.Loc[loc_Supported], 
                  *b = Glb.Loc[loc_Unsupported];

      muiSetText(ID_DriveVendor,    eng->getDriveVendor());
      muiSetText(ID_DriveProduct,   eng->getDriveProduct());
      muiSetText(ID_DriveVersion,   eng->getDriveVersion());
      muiSetText(ID_MechanismType,  mechanismToString(eng->getMechanismType()));
      muiSetText(ID_ReadableMedia,  mediaToString(eng->getReadableMedia()));
      muiSetText(ID_WritableMedia,  mediaToString(eng->getWritableMedia()));
      muiSetText(ID_SupportedData,  dataToString(caps));
      muiSetText(ID_Burnproof,      caps & DRT_Can_Do_BurnProof         ?  a :  b);
      muiSetText(ID_ISRC,           caps & DRT_Can_Read_ISRC            ?  a :  b);
      muiSetText(ID_MCN,            caps & DRT_Can_Read_MCN             ?  a :  b);
      muiSetText(ID_TestMode,       caps & DRT_Can_Do_TestMode          ?  a :  b);
      muiSetText(ID_Multisession,   caps & DRT_Can_Read_MultiSession    ?  a :  b);
      muiSetText(ID_CDText,         caps & DRT_Can_Read_CDText          ?  a :  b);
      muiSetText(ID_AudioPlayback,  caps & DRT_Can_Play_Audio           ?  a :  b);
      muiSetText(ID_AudioStream,    caps & DRT_Can_Read_CDAudioAccurate ?  Glb.Loc[loc_Accurate]   : Glb.Loc[loc_Jittered]);
      muiSetText(ID_ReadSpeeds,     speedsToString(eng->getReadSpeeds()));
      muiSetText(ID_WriteSpeeds,    speedsToString(eng->getWriteSpeeds()));
      muiSetText(ID_DriveStatus,    eng->isRegistered()                 ?  Glb.Loc[loc_Registered] : Glb.Loc[loc_Unregistered]);
   }
   else
   {
      _dx(Lvl_Info, "Unit not opened - setting up blanks");
      muiSetText(ID_DriveVendor,    "---");
      muiSetText(ID_DriveProduct,   "---");
      muiSetText(ID_DriveVersion,   "---");
      muiSetText(ID_MechanismType,  "---");
      muiSetText(ID_ReadableMedia,  "---");
      muiSetText(ID_WritableMedia,  "---");
      muiSetText(ID_SupportedData,  "---");
      muiSetText(ID_Burnproof,      "---");
      muiSetText(ID_ISRC,           "---");
      muiSetText(ID_MCN,            "---");
      muiSetText(ID_AudioStream,    "---");
      muiSetText(ID_AudioPlayback,  "---");
      muiSetText(ID_TestMode,       "---");
      muiSetText(ID_Multisession,   "---");
      muiSetText(ID_CDText,         "---");
      muiSetText(ID_ReadSpeeds,     "---");
      muiSetText(ID_WriteSpeeds,    "---");
      muiSetText(ID_DriveStatus,    "---");
   }
   Glb.CurrentEngine->Release();
   _dx(Lvl_Info, "All done.");
}

String MUIDrive::mechanismToString(DRT_Mechanism t)
{
   String s;
   switch (t)
   {
      case DRT_Mechanism_Caddy:     s = Glb.Loc[loc_MechCaddy];      break;
      case DRT_Mechanism_Tray:      s = Glb.Loc[loc_MechTray];       break;
      case DRT_Mechanism_Popup:     s = Glb.Loc[loc_MechPopup];      break;
      case DRT_Mechanism_Changer:   s = Glb.Loc[loc_MechChanger];    break;
      case DRT_Mechanism_Magazine:  s = Glb.Loc[loc_MechMagazine];   break;
      case DRT_Mechanism_Unknown:   s = Glb.Loc[loc_MechUnknown];    break;
      default:
         _dx(Lvl_Error, "Whoa, now that's funny. Unknown mechanism type %ld", t);
         s = "Unknown?";
         break;
   }
   return s;
}

String MUIDrive::mediaToString(unsigned long val)
{
   Set      t(val);
   String   s = "";
   bool e = false;

   _dx(Lvl_Info, "Building up media string from %lx", val);

   if (t & (DRT_Media_CD_ROM | DRT_Media_CD_R | DRT_Media_CD_RW))
   {
      bool f = false;
      
      s += "CD (";

      if (t & DRT_Media_CD_ROM)
         s += "ROM", f = true;

      if (t & DRT_Media_CD_R)       
         s += f ? ", " : "", s += "R", f = true;

      if (t & DRT_Media_CD_RW)      
         s += f ? ", " : "", s += "RW";

      s += ")";
      e = true;
   }
   
   if (t & (DRT_Media_DVD_ROM | DRT_Media_DVD_MinusR | DRT_Media_DVD_PlusR | DRT_Media_DVD_MinusRW | DRT_Media_DVD_PlusRW | DRT_Media_DVD_RAM))
   {
      bool f = false;

      if (e) 
         s += ", ";
      s += "DVD (";

      if (t & DRT_Media_DVD_ROM)
         s += "ROM", f = true;
      if (t & DRT_Media_DVD_RAM)
         s += f ? ", " : "", s += "RAM", f = true;

      if (t.ContainsAll(DRT_Media_DVD_PlusR | DRT_Media_DVD_MinusR))
         s += f ? ", " : "", s += "±R", f = true;
      else if (t & DRT_Media_DVD_PlusR)
         s += f ? ", " : "", s += "+R", f = true;
      else if (t & DRT_Media_DVD_MinusR)
         s += f ? ", " : "", s += "-R", f = true;

      if (t.ContainsAll(DRT_Media_DVD_PlusRW | DRT_Media_DVD_MinusRW))
         s += f ? ", " : "", s += "±RW", f = true;
      else if (t & DRT_Media_DVD_PlusRW)
         s += f ? ", " : "", s += "+RW", f = true;
      else if (t & DRT_Media_DVD_MinusRW)
         s += f ? ", " : "", s += "-RW", f = true;

      s += ")";
      e = true;
   }

   if (t & (DRT_Media_BD_ROM | DRT_Media_BD_R | DRT_Media_BD_RW))
   {
      bool f = false;

      if (e)
         s += ", ";
      s += "Blu Ray (";

      if (t & DRT_Media_BD_ROM)
         s += "ROM", f = true;
      if (t & DRT_Media_BD_R)
         s += f ? ", " : "", s += "R", f = true;
      if (t & DRT_Media_BD_RW)
         s += f ? ", " : "", s += "RW", f = true;

      s += ")";
      e = true;
   }

   _dx(Lvl_Info, "Media string: %s", (IPTR)s.Data());
   return s;
}

String MUIDrive::dataToString(unsigned long val)
{
   String s = "";

   _dx(Lvl_Info, "Building up Data string for %lx", val);

   if (val & DRT_Can_Read_Mode2)          s += "Mode2, ";
   if (val & (DRT_Can_Read_Mode2Form1 | DRT_Can_Read_Mode2Form2))
      s += "XA, ";
   if (val & DRT_Can_Read_CDAudio)
      s += "CD-Audio, ";

   _dx(Lvl_Info, "Data string: %s", (IPTR)s.Data());

   return s;
}

String MUIDrive::speedsToString(DiscSpeed *speeds)
{
   String   s     = "";
   String   t;
   int      idx;

   _dx(Lvl_Info, "Constructing speeds string");

   if (speeds == 0)
      return s;

   for (idx=0; speeds[idx].kbps != 0; idx++)
   {
      t = Glb.Loc.FormatNumber(speeds[idx].i, speeds[idx].f * 100000) + "x";
      if (s.Length() > 0)
         s += ", ";
      s += t;
   }
   
   _dx(Lvl_Info, "String: %s", (IPTR)s.Data());
   return s;
}

IPTR MUIDrive::buttonHook(IPTR id, IPTR data)
{
   switch (id)
   {
      case ID_DeviceSelect:
         {
            sDevice = (const char*)data;
            popUnit->setDevice(sDevice);
         }
         break;

      case ID_UnitSelect:
         {
            bool  res;

            lUnit = (IPTR)data;
            IEngine *peng = Glb.CurrentEngine->ObtainRead();
            res = peng->openDevice(sDevice, lUnit);
            Glb.CurrentEngine->Release();
            if (false == res)
            {
               request(Glb.Loc[Globals::loc_Error], Glb.Loc[loc_DriveNotOpened], Glb.Loc[Globals::loc_Proceed], 0);
            }
         }
         break;

   };
   return 0;
}
   
DbgHandler *MUIDrive::getDebug()
{
   return Glb.dbg;
}

// vim: encoding=iso-8859-1:ts=3:et:fileencoding=iso-8859-1
