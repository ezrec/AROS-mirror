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

#ifndef _GUIMUI_COMPONENTS_MUITOOLBAR_H_
#define _GUIMUI_COMPONENTS_MUITOOLBAR_H_

#include <Generic/LibrarySpool.h>
#include <Generic/GenericMUI.h>
#include <Generic/MUICustomClassT.h>
#include <Generic/ConfigParser.h>
#include <Generic/VectorT.h>
#include <Generic/HookT.h>
#include <Generic/HookAttrT.h>
#include "MUIPictureClass.h"
#include <Generic/Locale.h>

class MUIWindowMenu;
class CustomObject;
class CustomClass;
using namespace GenNS;

class MUIToolBar : public GenericMUI
{
public:
   enum ButtonType
   {
      Type_End       =  0,
      Type_Button,
      Type_Separator
   };

   struct Button
   {
      ButtonType     type;
      IPTR           id;
      IPTR           object;
      String         label;
      String         image1RelPath;
      String         image2RelPath;
   };

protected:
   enum LabelPos
   {
      Label_None     =  0,
      Label_Top,
      Label_Right,
      Label_Bottom,
      Label_Left
   };

   struct ButtonData : public Button
   {
      String   sName;
      String   sPath1;
      String   sPath2;
      String   sOrigPath1;
      String   sOrigPath2;
      ButtonData(const char* themePath, const char* themeName, const Button *def);
   };

   enum MenuOption
   {
      Menu_NoLabels    =  1,
      Menu_LabelsTop,
      Menu_LabelsRight,
      Menu_LabelsBottom,
      Menu_LabelsLeft,
      Menu_ChangeImages,
      Menu_ChangeFrames
   };

protected:
   IPTR                          all;
   VectorT<IPTR>                 buttons;
   VectorT<Button*>              defs;
   bool                          showImages;
   bool                          showFrames;
   LabelPos                      labelPosition;
   ConfigParser                 *Config;
   int                           spacing;
   String                        themeName;
   String                        themePath;
   MUIWindowMenu                *menu;
   HookAttrT<IPTR, IPTR>         callBack;
   int32                         active;
   Localization                 &Loc;

   HookT<MUIToolBar, MenuOption, MUIWindowMenu*>   hOptions;
   HookT<MUIToolBar, IPTR, IPTR>                   hCallBack;
   
protected:
   MUICustomClassT<MUIPictureClass>               *pPicture;

protected:
   static bool                   freeDefs(Button* const& id);

protected:
   virtual IPTR                  createButton(const MUIToolBar::Button *definition);
   virtual IPTR                  createSeparator();
   virtual IPTR                  setMenuOption(MenuOption, MUIWindowMenu*);
   virtual void                  buildMenu();
   virtual void                  rebuildGadgets();
   virtual IPTR                  reportChange(IPTR, IPTR);
public:
                                 MUIToolBar(ConfigParser *parent, Localization &loc);
   virtual                      ~MUIToolBar();
   virtual void                  addButtons(const MUIToolBar::Button *definition);    // title / image array
   virtual IPTR                  getObject();
   virtual void                  setHook(const Hook* pHook);
   virtual void                  setThemePath(const char* theme);
   virtual void                  setSelected(int32 i);

};

#endif

