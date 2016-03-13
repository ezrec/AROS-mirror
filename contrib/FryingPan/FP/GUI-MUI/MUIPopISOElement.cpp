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

#include "MUIPopISOElement.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/LibrarySpool.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include "Globals.h"

#undef String

/*
 * localization area
 */
enum Loc
{
   loc_PageCurrentElement  =  le_PopISOElement,
   loc_PageFileSystem,
   loc_ISOLevel1,
   loc_ISOLevel2,
   loc_ISOLevel3,
   loc_FileSystemSettings,
   loc_ISOLevel,
   loc_AddRockRidge,
   loc_AddJoliet,
   loc_RelaxedISO,
   loc_FileSystemInfo,
   loc_DiscName,
   loc_DiscSetName,
   loc_System,
   loc_Publisher,
   loc_Preparer,
   loc_Application,
   loc_FileDetails,
   loc_OrigName,
   loc_ISOName,
   loc_RRName,
   loc_JolietName,
   loc_Comment,
   loc_FilePresence,
   loc_ShowInISO,
   loc_ShowInRR,
   loc_ShowInJoliet,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_PageCurrentElement, "Current Element",            "PGE_CURRELEM"          },
   {  loc_PageFileSystem,     "File System",                "PGE_FILESYSTEM"        },
   {  loc_ISOLevel1,          "Level 1 (8+3 chars)",        "CCL_LEVEL1"            },
   {  loc_ISOLevel2,          "Level 2 (31 chars)",         "CCL_LEVEL2"            },
   {  loc_ISOLevel3,          "Level 3 (31 chars, fragmentation)", "CCL_LEVEL3"     },
   {  loc_FileSystemSettings, "Filesystem Settings",        "LBL_FILESYSSETTINGS"   },
   {  loc_ISOLevel,           "i&ISO Level",                "LBL_ISOLEVEL"          },
   {  loc_AddRockRidge,       "r&Add RockRidge Extension",  "LBL_ADDROCKRIDGE"      },
   {  loc_AddJoliet,          "j&Add Joliet Extension",     "LBL_ADDJOLIET"         },
   {  loc_RelaxedISO,         "x&Relaxed ISO Names",        "LBL_RELAXEDISO"        },
   {  loc_FileSystemInfo,     "Filesystem Information",     "LBL_FILESYSINFO"       },
   {  loc_DiscName,           "n&Disc Name:",               "LBL_DISCNAME"          },
   {  loc_DiscSetName,        "d&Disc Set Name:",           "LBL_DISCSETNAME"       },
   {  loc_System,             "s&System:",                  "LBL_SYSTEM"            },
   {  loc_Publisher,          "p&Publisher:",               "LBL_PUBLISHER"         },
   {  loc_Preparer,           "e&Preparer:",                "LBL_PREPARER"          },
   {  loc_Application,        "a&Application:",             "LBL_APPLICATION"       },
   {  loc_FileDetails,        "Item Details",               "LBL_FILEDETAILS"       },
   {  loc_OrigName,           "Original Name:",             "LBL_ORIGNAME"          },
   {  loc_ISOName,            "ISO Name:",                  "LBL_ISONAME"           },
   {  loc_RRName,             "r&RockRidge Name:",          "LBL_RRNAME"            },
   {  loc_JolietName,         "j&Joliet Name:",             "LBL_JOLIETNAME"        },
   {  loc_Comment,            "c&Comment:",                 "LBL_COMMENT"           },
   {  loc_FilePresence,       "Item Presence",              "LBL_ITEMPRESENCE"      },
   {  loc_ShowInISO,          "i&Show in ISO Tree:",        "LBL_SHOWINISO"         },
   {  loc_ShowInRR,           "t&Show in RR Tree:",         "LBL_SHOWINRR"          },
   {  loc_ShowInJoliet,       "o&Show in Joliet Tree:",     "LBL_SHOWINJOLIET"      },

   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "ELEM_POPISO";



MUIPopISOElement::MUIPopISOElement(Globals &glb) :
   MUIPopup(0),
   Glb(glb)
{
   hHkBtnHook.Initialize(this, &MUIPopISOElement::buttonHandler);
   setButtonCallBack(hHkBtnHook.GetHook());
   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);

   page = 0;
   elem = 0;
   browser = 0;
}

MUIPopISOElement::~MUIPopISOElement()
{
}

bool MUIPopISOElement::onOpen()
{
   setElement(elem);
   setBrowser(browser);
   return true;
}

bool MUIPopISOElement::onClose()
{
   return true;
}

IPTR MUIPopISOElement::getPopDisplay()
{
   string = muiString("", 0, ID_String); 
   return string;
  
}

IPTR MUIPopISOElement::getPopButton()
{
   return (IPTR)PopButton(MUII_PopUp);
}

IPTR MUIPopISOElement::getPopObject()
{
   static const char* titles[3] = 
   {
      0 
   };
   titles[0] = Glb.Loc[loc_PageCurrentElement];
   titles[1] = Glb.Loc[loc_PageFileSystem];


   if (page != 0)
      return page;

   page = (IPTR)RegisterGroup(titles),
      Child,         getElemSettings(),
      Child,         getISOSettings(),
   End;

   return page;
}

void MUIPopISOElement::setValue(IPTR val)
{
   muiSetText(ID_String, (const char*)val);
}

IPTR MUIPopISOElement::getValue()
{
   return (IPTR)value.Data();
}

IPTR MUIPopISOElement::buttonHandler(IPTR id, IPTR data)
{
   switch ((BtnID)id)
   {
      case ID_String:
         {
            if (value != (const char*)data)
            {
               value = (const char*)data;
               if (elem != 0)
                  elem->setNormalName(value);
            }
         }
         break;

      case ID_VolumeName:
         {
            browser->getRoot()->setVolumeID((char*)data);
         }
         break;

      case ID_VolumeSetName:
         {
            browser->getRoot()->setVolumeSetID((char*)data);
         }
         break;

      case ID_Publisher:
         {
            browser->getRoot()->setPublisherID((char*)data);
         }
         break;

      case ID_Preparer:
         {
            browser->getRoot()->setPreparerID((char*)data);
         }
         break;
 
      case ID_Application:
         {
            browser->getRoot()->setApplicationID((char*)data);
         }
         break;

      case ID_SystemType:
         {
            browser->getRoot()->setSystemID((char*)data);
         }
         break;

      case ID_ISOLevel:
         {
            if (browser == 0)
               break;
            switch ((IPTR)data)
            {
               case 0:
                  browser->getRoot()->setISOLevel(ClName::ISOLevel_1);
                  break;

               case 1:
                  browser->getRoot()->setISOLevel(ClName::ISOLevel_2);
                  break;

               case 2:
                  browser->getRoot()->setISOLevel(ClName::ISOLevel_3);
                  break;

            }
         }
         break;

      case ID_RelaxedISO:
         {
            if (browser != 0)
               browser->getRoot()->setISORelaxed((bool)data);
         }
         break;

      case ID_AddJoliet:
         {
            if (browser != 0)
               browser->getRoot()->setJolietEntry((bool)data);
         }
         break;

      case ID_AddRockRidge:
         {
            if (browser != 0)
               browser->getRoot()->setRockRidgeEntry((bool)data);
         }
         break;

      case ID_NormalName:
         {
            if (elem != NULL)
               elem->setNormalName((char*)data);
         }
         break;

      case ID_JolietName:
         {
            if (elem != NULL)
               elem->setJolietName((char*)data);
         }
         break;

      case ID_RRName:
         {
            if (elem != NULL)
               elem->setNormalName((char*)data);
         }
         break;

      case ID_AddISOEntry:
         {
            if (elem != NULL)
               elem->setISOEntry((bool)data);
         }
         break;

      case ID_AddRREntry:
         {
            if (elem != NULL)
               elem->setRockRidgeEntry((bool)data);
         }
         break;

      case ID_AddJolietEntry:
         {
            if (elem != NULL)
               elem->setJolietEntry((bool)data);
         }
         break;

      case ID_Comment:
         {
            if (elem != NULL)
               elem->setComment((char*)data);
         }
         break;

      case ID_ISOName:
         {
            // ???
         }
         break;
   }
   return 0;
}

IPTR MUIPopISOElement::getObject()
{
   MUIPopup::getObject();
   return (IPTR)all;
}

IPTR MUIPopISOElement::getISOSettings()
{
   static const char* levels[4] =
   {
      0
   };
   levels[0] = Glb.Loc[loc_ISOLevel1];
   levels[1] = Glb.Loc[loc_ISOLevel2];
   levels[2] = Glb.Loc[loc_ISOLevel3];

   return (IPTR)VGroup,
      Child,         ColGroup(2),
         MUIA_Group_SameSize, true,

         Child,         ColGroup(2),
            GroupFrameT(Glb.Loc[loc_FileSystemSettings].Data()),
         
            Child,         muiSpace(),
            Child,         muiSpace(),

            Child,         muiLabel(Glb.Loc[loc_ISOLevel], Glb.Loc.Accel(loc_ISOLevel), ID_Default, Align_Right),
            Child,         muiCycle(levels, Glb.Loc.Accel(loc_ISOLevel), ID_ISOLevel),

            Child,         muiLabel(Glb.Loc[loc_AddRockRidge], Glb.Loc.Accel(loc_AddRockRidge), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_AddRockRidge), ID_AddRockRidge),

            Child,         muiLabel(Glb.Loc[loc_AddJoliet], Glb.Loc.Accel(loc_AddJoliet), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_AddJoliet), ID_AddJoliet),

            Child,         muiLabel(Glb.Loc[loc_RelaxedISO], Glb.Loc.Accel(loc_RelaxedISO), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_RelaxedISO), ID_RelaxedISO),

            Child,         muiSpace(),
            Child,         muiSpace(),
         End,
         
         Child,         ColGroup(2),
            GroupFrameT(Glb.Loc[loc_FileSystemInfo].Data()),

            Child,         muiSpace(),
            Child,         muiSpace(),

            // we could add these fancy checkbox-to-string items here

            Child,         muiLabel(Glb.Loc[loc_DiscName], Glb.Loc.Accel(loc_DiscName), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_DiscName), ID_VolumeName),

            Child,         muiLabel(Glb.Loc[loc_DiscSetName], Glb.Loc.Accel(loc_DiscSetName), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_DiscSetName), ID_VolumeSetName),

            Child,         muiLabel(Glb.Loc[loc_System], Glb.Loc.Accel(loc_System), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_System), ID_SystemType),

            Child,         muiLabel(Glb.Loc[loc_Publisher], Glb.Loc.Accel(loc_Publisher), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_Publisher), ID_Publisher),

            Child,         muiLabel(Glb.Loc[loc_Preparer], Glb.Loc.Accel(loc_Preparer), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_Preparer), ID_Preparer),

            Child,         muiLabel(Glb.Loc[loc_Application], Glb.Loc.Accel(loc_Application), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_Application), ID_Application),

            Child,         muiSpace(),
            Child,         muiSpace(),

         End,
      End,
   End;
}

void MUIPopISOElement::setBrowser(IBrowser* pBser)
{
   browser = pBser;
   if (browser != NULL)
   {
      ClRoot *pRoot = browser->getRoot();

      muiSetEnabled(ID_VolumeName,     true);
      muiSetEnabled(ID_VolumeSetName,  true);
      muiSetEnabled(ID_Publisher,      true);
      muiSetEnabled(ID_Preparer,       true);
      muiSetEnabled(ID_Application,    true);
      muiSetEnabled(ID_ISOLevel,       true);
      muiSetEnabled(ID_AddRockRidge,   true);
      muiSetEnabled(ID_AddJoliet,      true);
      muiSetEnabled(ID_RelaxedISO,     true);

      String s;

      s = pRoot->getVolumeID();
      s.TrimChars(" ");
      muiSetText(ID_VolumeName,     s);

      s = pRoot->getVolumeSetID();
      s.TrimChars(" ");
      muiSetText(ID_VolumeSetName,  s);

      s = pRoot->getSystemID();
      s.TrimChars(" ");
      muiSetText(ID_SystemType,  s);

      s = pRoot->getPublisherID();
      s.TrimChars(" ");
      muiSetText(ID_Publisher,      s);

      s = pRoot->getPreparerID();
      s.TrimChars(" ");
      muiSetText(ID_Preparer,       s);

      s = pRoot->getApplicationID();
      s.TrimChars(" ");
      muiSetText(ID_Application,    s);

      switch (pRoot->getISOLevel())
      {
         case ClRoot::ISOLevel_1:
            muiSetSelected(ID_ISOLevel, 0);
            break;

         case ClRoot::ISOLevel_2:
            muiSetSelected(ID_ISOLevel, 1);
            break;

         case ClRoot::ISOLevel_3:
            muiSetSelected(ID_ISOLevel, 2);
            break;
      }

      muiSetSelected(ID_AddRockRidge,  pRoot->isRockRidgeEntry());
      muiSetSelected(ID_AddJoliet,     pRoot->isJolietEntry());
      muiSetSelected(ID_RelaxedISO,    pRoot->isISORelaxed());
   }
   else
   {
      muiSetEnabled(ID_VolumeName,     false);
      muiSetEnabled(ID_VolumeSetName,  false);
      muiSetEnabled(ID_Publisher,      false);
      muiSetEnabled(ID_Preparer,       false);
      muiSetEnabled(ID_Application,    false);
      muiSetEnabled(ID_ISOLevel,       false);
      muiSetEnabled(ID_AddRockRidge,   false);
      muiSetEnabled(ID_AddJoliet,      false);
      muiSetEnabled(ID_RelaxedISO,     false);
   }
}

void MUIPopISOElement::setElement(ClElement *pElem)
{
   elem = pElem;
   if (NULL != elem)
   {
      muiSetEnabled(ID_String,      true);
      muiSetEnabled(ID_NormalName,  true);
      muiSetEnabled(ID_ISOName,     true);
      muiSetEnabled(ID_JolietName,  true);
      muiSetEnabled(ID_Comment,     true);
      muiSetEnabled(ID_RRName,      true);
      muiSetEnabled(ID_AddISOEntry, true);
      muiSetEnabled(ID_AddRREntry,  true);
      muiSetEnabled(ID_AddJolietEntry, true);

      muiSetText(ID_String,      elem->getNormalName());
      muiSetText(ID_NormalName,  elem->getNormalName());
      muiSetText(ID_ISOName,     elem->getISOName());
      muiSetText(ID_JolietName,  elem->getJolietName());
      muiSetText(ID_Comment,     elem->getComment());
      muiSetText(ID_RRName,      elem->getNormalName());

      muiSetSelected(ID_AddISOEntry,    elem->isISOEntry());
      muiSetSelected(ID_AddJolietEntry, elem->isJolietEntry());
      muiSetSelected(ID_AddRREntry,     elem->isRockRidgeEntry());
   }
   else
   {
      muiSetEnabled(ID_String,      false);
      muiSetEnabled(ID_NormalName,  false);
      muiSetEnabled(ID_ISOName,     false);
      muiSetEnabled(ID_JolietName,  false);
      muiSetEnabled(ID_Comment,     false);
      muiSetEnabled(ID_RRName,      false);
      muiSetEnabled(ID_AddISOEntry, false);
      muiSetEnabled(ID_AddRREntry,  false);
      muiSetEnabled(ID_AddJolietEntry, false);
 
      muiSetText(ID_String,      "");
      muiSetText(ID_NormalName,  "");
      muiSetText(ID_ISOName,     "");
      muiSetText(ID_JolietName,  "");
      muiSetText(ID_Comment,     "");
      muiSetText(ID_RRName,      "");

      muiSetSelected(ID_AddISOEntry,    false);
      muiSetSelected(ID_AddJolietEntry, false);
      muiSetSelected(ID_AddRREntry,     false);
   }
}

IPTR MUIPopISOElement::getElemSettings()
{
   return (IPTR)VGroup,
      Child,         ColGroup(2),
         MUIA_Group_SameSize, true,

         Child,         ColGroup(2),
            GroupFrameT(Glb.Loc[loc_FileDetails].Data()),
         
            Child,         muiSpace(),
            Child,         muiSpace(),

            Child,         muiLabel(Glb.Loc[loc_OrigName], Glb.Loc.Accel(loc_OrigName), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_OrigName), ID_NormalName),

            Child,         muiLabel(Glb.Loc[loc_ISOName], Glb.Loc.Accel(loc_ISOName), ID_Default, Align_Right),
            Child,         muiLabel("---", 0, ID_ISOName, Align_Left),

            Child,         muiLabel(Glb.Loc[loc_RRName], Glb.Loc.Accel(loc_RRName), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_RRName), ID_RRName),

            Child,         muiLabel(Glb.Loc[loc_JolietName], Glb.Loc.Accel(loc_JolietName), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_JolietName), ID_JolietName),

            Child,         muiLabel(Glb.Loc[loc_Comment], Glb.Loc.Accel(loc_Comment), ID_Default, Align_Right),
            Child,         muiString(0, Glb.Loc.Accel(loc_Comment), ID_Comment),

            Child,         muiSpace(),
            Child,         muiSpace(),
         End,

         Child,         ColGroup(2),
            GroupFrameT(Glb.Loc[loc_FilePresence].Data()),

            Child,         muiSpace(),
            Child,         muiSpace(),

            Child,         muiLabel(Glb.Loc[loc_ShowInISO], Glb.Loc.Accel(loc_ShowInISO), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_ShowInISO), ID_AddISOEntry),

            Child,         muiLabel(Glb.Loc[loc_ShowInRR], Glb.Loc.Accel(loc_ShowInRR), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_ShowInRR), ID_AddRREntry),

            Child,         muiLabel(Glb.Loc[loc_ShowInJoliet], Glb.Loc.Accel(loc_ShowInJoliet), ID_Default, Align_Right),
            Child,         muiCheckBox(0, Glb.Loc.Accel(loc_ShowInJoliet), ID_AddJolietEntry),

            Child,         muiSpace(),
            Child,         muiSpace(),

         End,
      End,
   End;
}

void MUIPopISOElement::setEnabled(bool enabled)
{
   Intuition->SetAttrsA(all, (TagItem*)ARRAY(MUIA_Disabled, !enabled, TAG_DONE, 0));
}
