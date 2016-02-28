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

#include "MUIWindowMenu.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/LibrarySpool.h> 

MUIWindowMenu::MUIWindowMenu()
{
   pMain = MenustripObject,
      MUIA_Menustrip_Enabled, true,
   End;

   hLocal.Initialize(this, &MUIWindowMenu::callBack);
   hCallback = 0;
   pRadio    = 0;
}

unsigned long MUIWindowMenu::callBack(long* arg1, long *arg2)
{
   return hCallback.Call(arg2[0], this);
}

MUIWindowMenu::~MUIWindowMenu()
{
}

Object *MUIWindowMenu::getObject()
{
   return pMain;
}

void MUIWindowMenu::addItem(const char *tTitle, long lParam, const char* tShort)
{
   Object *pObj = MenuitemObject,
                     MUIA_Menuitem_Title,    tTitle,
                     MUIA_UserData,          lParam,
                     ((tShort != 0) && (tShort[0] != 0)) ? MUIA_Menuitem_Shortcut : TAG_IGNORE, tShort,
                  End;

   DoMtd(pObj, ARRAY(MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)pObj, 3, MUIM_CallHook, (IPTR)hLocal.GetHook(), lParam));
   DoMtd(pMenu, ARRAY(MUIM_Family_AddTail, (IPTR)pObj)); 
}

void MUIWindowMenu::addSeparator()
{
   Object *pObj = MenuitemObject,
                     MUIA_Menuitem_Title,    0xffffffff,
                  End;

   DoMtd(pMenu, ARRAY(MUIM_Family_AddTail, (IPTR)pObj)); 
}

void MUIWindowMenu::addChoice(const char *tTitle, bool bSelected, long lParam, const char* tShort)
{
   Object *pObj = MenuitemObject,
                     MUIA_Menuitem_Title,    tTitle,
                     MUIA_Menuitem_Checked,  bSelected,
                     MUIA_Menuitem_Checkit,  true,
                     ((tShort != 0) && (tShort[0] != 0)) ? MUIA_Menuitem_Shortcut : TAG_IGNORE, tShort,
                     MUIA_UserData,          lParam,
                  End;

   DoMtd(pObj, ARRAY(MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)pObj, 3, MUIM_CallHook, (IPTR)hLocal.GetHook(), lParam));

   DoMtd(pMenu, ARRAY(MUIM_Family_AddTail, (IPTR)pObj)); 
}

void MUIWindowMenu::addRadio(const char *tTitle)
{
   Object *pObj = MenuitemObject,
                     MUIA_Menuitem_Title, tTitle,
                     MUIA_UserData,       0xffffffff,
                  End;

   DoMtd(pMenu, ARRAY(MUIM_Family_AddTail, (IPTR)pObj)); 
   pRadio = pObj;
}

void MUIWindowMenu::addRadioOption(const char *tTitle, bool bSelected, long lParam, const char* tShort)
{
   ASSERT(pRadio != NULL);
   if (pRadio == NULL)
      return;
   Object *pObj = MenuitemObject,
                     MUIA_Menuitem_Title,    tTitle,
                     MUIA_Menuitem_Checked,  bSelected,
                     MUIA_Menuitem_Checkit,  true,
                     MUIA_UserData,          lParam,
                     ((tShort != 0) && (tShort[0] != 0)) ? MUIA_Menuitem_Shortcut : TAG_IGNORE, tShort,
                     MUIA_Menuitem_Exclude,  0xffffffff,
                  End;

   DoMtd(pObj, ARRAY(MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)pObj, 3, MUIM_CallHook, (IPTR)hLocal.GetHook(), lParam));

   DoMtd(pRadio, ARRAY(MUIM_Family_AddTail, (IPTR)pObj)); 
}

void MUIWindowMenu::setHook(const Hook *pNotify)
{
   hCallback = pNotify;
}

void MUIWindowMenu::addMenu(const char *tTitle)
{
   pMenu = MenuitemObject,
      MUIA_Menuitem_Title,     tTitle,
      MUIA_Menuitem_Enabled,   true,
   End,

   DoMtd(pMain, ARRAY(MUIM_Family_AddTail, (IPTR)pMenu)); 
}
