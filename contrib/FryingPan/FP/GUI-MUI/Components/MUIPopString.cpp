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

#include "MUIPopString.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/LibrarySpool.h>
#include <proto/intuition.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <LibC/LibC.h>

#undef String

MUIPopString::MUIPopString(const char* name, int cols, const char** names) :
   MUIPopup(name)
{
   sCols = "";
   lCols = cols;
   while (--cols)
   {
      sCols += "BAR,";
      if (names != 0)
      {
         if (*names)
            sColNames << *names++;
         else
            sColNames << "";
      }
   }

   /*
    * fill in last column
    */
   if (names != 0)
   {
      if (*names)
         sColNames << *names++;
      else
         sColNames << "";
   }

   value = 0;
   hHkBtnHook.Initialize(this, &MUIPopString::buttonHandler);
   hHkConstruct.Initialize(this, &MUIPopString::construct);
   hHkDestruct.Initialize(this, &MUIPopString::destruct);
   hHkDisplay.Initialize(this, &MUIPopString::display);
   setButtonCallBack(hHkBtnHook.GetHook());
}

MUIPopString::~MUIPopString()
{
}

bool MUIPopString::onOpen()
{
   return true;
}

bool MUIPopString::onClose()
{
   value = getSelectedEntry();
   muiSetText(ID_String, toString(value));
   return true;
}

unsigned long *MUIPopString::getPopDisplay()
{
   string = (Object *)muiString("", 0, ID_String); 
   return (unsigned long *)string;
}

unsigned long *MUIPopString::getPopButton()
{
   return (unsigned long *)PopButton(MUII_PopUp);
}

unsigned long *MUIPopString::getPopObject()
{
   listview = NListviewObject,
      MUIA_NListview_NList,         list = NListObject,
         InputListFrame,
         MUIA_NList_Title,          sColNames.Count() != 0,
         MUIA_NList_ConstructHook,  (long)hHkConstruct.GetHook(),
         MUIA_NList_DestructHook,   (long)hHkDestruct.GetHook(),
         MUIA_NList_DisplayHook,    (long)hHkDisplay.GetHook(),
         MUIA_NList_Format,         (long)sCols.Data(),
      End,
   End;

   return (unsigned long *)listview;
}

void MUIPopString::setValue(const void* string)
{
   muiSetText(ID_String, (const char*)string);
}

const void *MUIPopString::getValue()
{
   return value;
}

unsigned long MUIPopString::buttonHandler(int id, void* data)
{
   switch (id)
   {
      case ID_String:
         {
            if (((value != 0) && (strcmp(toString(value), (const char*)data))) ||
                 (value == 0))
            {
               value = (const char*)data;
               update();
            }
         }
         break;
   }
   return 0;
}

unsigned long MUIPopString::construct(void*, void* arg)
{
   return (unsigned long)arg;
}

unsigned long MUIPopString::destruct(void*, void* arg)
{
   return 0;
}

unsigned long MUIPopString::display(const char** arr, void* arg)
{
   if (arg != NULL)
   {
      arr[0] = (const char*)arg;
   }
   else
   {
      for (int i=0; i<lCols; i++)
         arr[i] = sColNames[i];
   }
   return 0;
}

void MUIPopString::clearList()
{
   if (all == 0)
      return; 
   DoMtd((Object *)list, ARRAY(MUIM_NList_Clear));
}

void MUIPopString::addEntry(const void* entry)
{
   if (all == 0)
      return;
   DoMtd((Object *)list, ARRAY(MUIM_NList_InsertSingle, (long)entry, (unsigned)MUIV_NList_Insert_Bottom));
}

const void *MUIPopString::getSelectedEntry()
{
   void *item;

   if (all == 0)
      return 0;

   DoMtd((Object *)list, ARRAY(MUIM_NList_GetEntry, (unsigned)MUIV_NList_GetEntry_Active, (long)&item));

   return item;
}

unsigned long *MUIPopString::getObject()
{
   MUIPopup::getObject();
   DoMtd((Object *)list, ARRAY(MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime, (long)popup, 2, MUIM_Popstring_Close, true));

//   if (NULL != string)
//      DoMtd(string, ARRAY(MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, (long)popup, 2, MUIM_Popstring_Close, true));
   return (unsigned long *)all;
}

const char *MUIPopString::toString(const void* arg)
{
   return (const char*) arg;
}


