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

#include "MUIToolBar.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/ConfigParser.h>
#include "MUIWindowMenu.h"
#include "../Globals.h"

/*
 * localization area
 */
enum Loc
{
   loc_Settings            =  le_ToolBar,
   loc_ShowImages,
   loc_ShowFrames,
   loc_Labels,
   loc_LabelsBelowImage,
   loc_LabelsAboveImage,
   loc_NoLabels
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_Settings,           "Settings",             "MNU_SETTINGS"        },
   {  loc_ShowImages,         "Show / Hide Images",   "MNU_SHOWIMAGES"      },
   {  loc_ShowFrames,         "Show / Hide Frames",   "MNU_SHOWFRAMES"      },
   {  loc_Labels,             "Labels",               "MNU_LABELS"          },
   {  loc_LabelsBelowImage,   "Labels Below Image",   "MNU_LABELSBELOWIMG"  },
   {  loc_LabelsAboveImage,   "Labels Above Image",   "MNU_LABELSABOVEIMG"  },
   {  loc_NoLabels,           "No Labels",            "MNU_NOLABELS"        },
   {  Localization::LocaleSet::Set_Last, 0, 0                           }
};

static const char* LocaleGroup = "ELEM_TOOLBAR";



MUIToolBar::ButtonData::ButtonData(const char* themePath, const char* themeName, const Button *def)
{
   id          = def->id;
   type        = def->type;
   sName       = def->label;
   sOrigPath1  = def->image1RelPath;
   sOrigPath2  = def->image2RelPath;
   sPath1      = themePath;
   sPath1.AddPath((char*)themeName);
   sPath2      = sPath1;
   sPath1.AddPath((char*)def->image1RelPath.Data());
   sPath2.AddPath((char*)def->image2RelPath.Data());
   label       = sName.Data();
   image1RelPath  = sPath1;
   image2RelPath  = sPath2;

}



MUIToolBar::MUIToolBar(ConfigParser *parent, Localization &loc) :
   Loc(loc)
{
   all            = 0;
   showFrames     = true;
   showImages     = true;
   spacing        = 0;
   themeName      = "default";
   themePath      = "themes";
   menu           = 0;
   active         = -1;
   labelPosition  = Label_Bottom;
   
   Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);
   pPicture = new MUICustomClassT<MUIPictureClass>(MUIC_Area);
   Config = new ConfigParser(parent, "ToolBar", 0);
   hOptions.Initialize(this, &MUIToolBar::setMenuOption);
   hCallBack.Initialize(this, &MUIToolBar::reportChange);

   showFrames     = Config->getValue("ShowFrames", showFrames);
   showImages     = Config->getValue("ShowImages", showImages);
   labelPosition  = (LabelPos)Config->getValue("ShowLabels", labelPosition);
   spacing        = Config->getValue("Spacing",    spacing);
}

MUIToolBar::~MUIToolBar()
{
   Config->setValue("ShowFrames", showFrames);
   Config->setValue("ShowImages", showImages);
   Config->setValue("ShowLabels", labelPosition);
   Config->setValue("Spacing",    spacing);

   delete Config;
   if (NULL != menu)
      delete menu;
   if (NULL != pPicture)
      delete pPicture;

   defs.ForEach(&freeDefs);
}

bool MUIToolBar::freeDefs(Button* const& id)
{
   delete id;
   return true;
}

void MUIToolBar::addButtons(const MUIToolBar::Button *definition)
{
   ASSERT(definition != 0);
   if (definition == 0)
      return;

   for (int i=0; definition[i].type != Type_End; i++)
   {
      defs << new ButtonData(themePath, themeName, &definition[i]);
   }
}

IPTR MUIToolBar::getObject()
{
   if (0 != all)
      return all;
   
   buildMenu();

   all = (IPTR)HGroup,
      MUIA_Group_Spacing,        spacing,
   End;

   rebuildGadgets();
   return all;
}

IPTR MUIToolBar::createButton(const MUIToolBar::Button *def)
{
   IPTR btn;
   IPTR label = 0;
   IPTR image = 0;
   String            s;

   if ((false == showImages) && (Label_None == labelPosition))
      showImages = true;

   if (true == showImages)
   {
      image = (IPTR)pPicture->Create(
         MUIPictureClass::MUIA_Picture_NormalImage,      def->image1RelPath.Data(),
         MUIPictureClass::MUIA_Picture_SelectedImage,    def->image2RelPath.Data(),
         MUIA_ShowSelState,                              true,
         MUIA_Selected,                                  false,
         TAG_DONE,                                       0
      );
   }

   if (Label_None != labelPosition)
   {
      label = (IPTR)TextObject,
         MUIA_Font,           MUIV_Font_Tiny,
         MUIA_Text_Contents,  def->label.Data(),
         MUIA_Text_PreParse,  "\033c",
         MUIA_Text_SetMax,    false,
         MUIA_Text_SetMin,    false,
         //MUIA_FixWidth,       40,
      End;
   }

   if ((labelPosition == Label_Left) || (labelPosition == Label_Right))
   {
      btn = (IPTR)HGroup,
         MUIA_InputMode,      MUIV_InputMode_Immediate,
         //MUIA_InputMode,      MUIV_InputMode_RelVerify,
         MUIA_UserData,       def->id,
         MUIA_Frame,          showFrames ? MUIV_Frame_ImageButton : MUIV_Frame_None,
         MUIA_Group_Spacing,  0,
         MUIA_ContextMenu,    (IPTR)menu->getObject(),
      End;
   }
   else
   {
      btn = (IPTR)VGroup,
         MUIA_InputMode,      MUIV_InputMode_Immediate,
         //MUIA_InputMode,      MUIV_InputMode_RelVerify,
         MUIA_UserData,       def->id,
         MUIA_Frame,          showFrames ? MUIV_Frame_ImageButton : MUIV_Frame_None,
         MUIA_Group_Spacing,  0,
         MUIA_ContextMenu,    menu->getObject(),
      End;
   }

   if (Label_None == labelPosition)
   {
      DoMtd((Object *)btn, ARRAY(OM_ADDMEMBER, (IPTR)image));
   }
   else if ((Label_Left == labelPosition) || (Label_Top == labelPosition))
   {
      DoMtd((Object *)btn, ARRAY(OM_ADDMEMBER, (IPTR)label));
      if (true == showImages)
      {
         DoMtd((Object *)btn, ARRAY(OM_ADDMEMBER, (IPTR)image));
      }
   }
   else
   {
      if (true == showImages)
      {
         DoMtd((Object *)btn, ARRAY(OM_ADDMEMBER, (IPTR)image));
      }
      DoMtd((Object *)btn, ARRAY(OM_ADDMEMBER, (IPTR)label));
   }

   DoMtd((Object *)btn, ARRAY(MUIM_Notify, MUIA_Selected, true, (IPTR)btn, 3, MUIM_CallHook, (IPTR)hCallBack.GetHook(), def->id));

   return btn;
}

IPTR MUIToolBar::createSeparator()
{
   return (IPTR)RectangleObject,
      MUIA_FixWidth,    10,
   End;
}

unsigned long MUIToolBar::setMenuOption(MenuOption option, MUIWindowMenu*)
{
   switch (option)
   {
      case Menu_ChangeImages:
         showImages = !showImages;
         break;

      case Menu_ChangeFrames:
         showFrames = !showFrames;
         break;

      case Menu_NoLabels:
         labelPosition = Label_None;
         break;

      case Menu_LabelsTop:
         labelPosition = Label_Top;
         break;

      case Menu_LabelsRight:
         labelPosition = Label_Right;
         break;

      case Menu_LabelsBottom:
         labelPosition = Label_Bottom;
         break;

      case Menu_LabelsLeft:
         labelPosition = Label_Left;
         break;
   }
   
   rebuildGadgets();
   return 0;
}

void MUIToolBar::buildMenu()
{
   if (NULL != menu)
      return;

   menu = new MUIWindowMenu();
   menu->addMenu(Loc[loc_Settings]);
   menu->addItem(Loc[loc_ShowImages], Menu_ChangeImages);
   menu->addItem(Loc[loc_ShowFrames], Menu_ChangeFrames);
   menu->addSeparator();
   menu->addRadio(Loc[loc_Labels]);
   menu->addRadioOption(Loc[loc_LabelsBelowImage], labelPosition == Label_Bottom,  Menu_LabelsBottom);
   menu->addRadioOption(Loc[loc_LabelsAboveImage], labelPosition == Label_Top,     Menu_LabelsTop);
   menu->addRadioOption(Loc[loc_NoLabels],         labelPosition == Label_None,    Menu_NoLabels);

   menu->setHook(hOptions.GetHook());

   return;
}

void MUIToolBar::rebuildGadgets()
{
   DoMtd((Object *)all, ARRAY(MUIM_Group_InitChange));

   for (int i=0; i<buttons.Count(); i++)
   {
      IPTR elem = buttons[i];
      DoMtd((Object *)all, ARRAY(OM_REMMEMBER, elem));
   }

   buttons.Empty();

   for (int i=0; i<defs.Count(); i++)
   {
      switch (defs[i]->type)
      {
         case Type_Button:
            defs[i]->object = createButton(defs[i]);
            break;
         case Type_Separator:
            defs[i]->object = createSeparator();
            break;
         case Type_End:
            ASSERT(false);
            break;
      }
      buttons << defs[i]->object;
   }

   for (int i=0; i<buttons.Count(); i++)
   {
      DoMtd((Object *)all, ARRAY(OM_ADDMEMBER, (IPTR)buttons[i]));
   }

   DoMtd((Object *)all, ARRAY(MUIM_Group_ExitChange));

   if (active == -1)
   {
      for (int32 i=0; i<defs.Count(); i++)
      {
         if (defs[i]->type ==Type_Button)
         {
            active = defs[i]->id;
            break;
         }
      }
   }

   setSelected(active);
}

IPTR MUIToolBar::reportChange(IPTR, IPTR id)
{
   IPTR selected = *(IPTR *)id;
   setSelected(selected);
   return callBack.Call((IPTR)this, selected);
}

void MUIToolBar::setHook(const Hook* pHook)
{
   callBack = pHook;
}

void MUIToolBar::setThemePath(const char* theme)
{
   this->themePath = theme;
}

void MUIToolBar::setSelected(int32 id)
{
   for (int32 i=0; i<defs.Count(); i++)
   {
      if (defs[i]->type == Type_Button)
      {
         Intuition->SetAttrsA((Object *)defs[i]->object, (TagItem*)ARRAY(
            MUIA_Selected,       defs[i]->id == (IPTR)id,
            TAG_DONE,            0));
      }
   }
   
   active = id;
}
