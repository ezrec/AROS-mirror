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
   muiSetText(ID_String, toString((const void*)value));
   return true;
}

IPTR MUIPopString::getPopDisplay()
{
   string = (Object *)muiString("", 0, ID_String); 
   return (IPTR)string;
}

IPTR MUIPopString::getPopButton()
{
   return (IPTR)PopButton(MUII_PopUp);
}

IPTR MUIPopString::getPopObject()
{
   listview = NListviewObject,
      MUIA_NListview_NList,         list = NListObject,
         InputListFrame,
         MUIA_NList_Title,          sColNames.Count() != 0,
         MUIA_NList_ConstructHook,  (IPTR)hHkConstruct.GetHook(),
         MUIA_NList_DestructHook,   (IPTR)hHkDestruct.GetHook(),
         MUIA_NList_DisplayHook,    (IPTR)hHkDisplay.GetHook(),
         MUIA_NList_Format,         (IPTR)sCols.Data(),
      End,
   End;

   return (IPTR)listview;
}

void MUIPopString::setValue(IPTR val)
{
   muiSetText(ID_String, (const char*)val);
}

IPTR MUIPopString::getValue()
{
   return value;
}

IPTR MUIPopString::buttonHandler(IPTR id, IPTR data)
{
   switch (id)
   {
      case ID_String:
         {
            if (((value != 0) && (strcmp(toString((const void*)value), (const char*)data))) ||
                 (value == 0))
            {
               value = data;
               update();
            }
         }
         break;
   }
   return 0;
}

IPTR MUIPopString::construct(IPTR, IPTR arg)
{
   return arg;
}

IPTR MUIPopString::destruct(IPTR, IPTR arg)
{
   return 0;
}

IPTR MUIPopString::display(const char** arr, IPTR arg)
{
   if (arg != (IPTR)NULL)
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

void MUIPopString::addEntry(IPTR entry)
{
   if (all == 0)
      return;

   DoMtd((Object *)list, ARRAY(MUIM_NList_InsertSingle, entry, (IPTR)MUIV_NList_Insert_Bottom));
}

const IPTR MUIPopString::getSelectedEntry()
{
   IPTR item=0;

   if (all == 0)
      return 0;

   DoMtd((Object *)list, ARRAY(MUIM_NList_GetEntry, (IPTR)MUIV_NList_GetEntry_Active, (IPTR)&item));

   return item;
}

IPTR MUIPopString::getObject()
{
   MUIPopup::getObject();
   DoMtd((Object *)list, ARRAY(MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime, (IPTR)popup, 2, MUIM_Popstring_Close, true));

//   if (NULL != string)
//      DoMtd(string, ARRAY(MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, (long)popup, 2, MUIM_Popstring_Close, true));
   return (IPTR)all;
}

const char *MUIPopString::toString(const void* arg)
{
   return (const char*) arg;
}
