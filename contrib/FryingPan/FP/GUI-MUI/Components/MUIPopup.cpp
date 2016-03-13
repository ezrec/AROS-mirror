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

#include "MUIPopup.h"
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include <Generic/LibrarySpool.h>
#include <proto/intuition.h>

MUIPopup::MUIPopup(const char *name)
{
   all = 0;

   this->name = name;
   hHkOpen.Initialize(this, &MUIPopup::doOpen);
   hHkClose.Initialize(this, &MUIPopup::doClose);
}

MUIPopup::~MUIPopup()
{
}

void MUIPopup::update()
{
   hNotify.Call(id, (IPTR)getValue());
}

void MUIPopup::setID(IPTR id)
{
   this->id = id;
}

void MUIPopup::setCallbackHook(const Hook* hook)
{
   hNotify = hook;
}

IPTR MUIPopup::getObject()
{
   if (NULL != all)
      return (IPTR)all;

   all = HGroup,
    End;

   if (name.Length() != 0)
   {
      DoMtd(all, ARRAY(OM_ADDMEMBER, (IPTR)(label = (Object *)muiLabel(name, 0, 0, Align_Right))));
   }

   DoMtd(all, ARRAY(OM_ADDMEMBER, (IPTR)(
            popup = PopobjectObject,
               MUIA_Popstring_String,              display = (Object *)getPopDisplay(),
               MUIA_Popobject_Object,              object  = (Object *)getPopObject(),
               MUIA_Popstring_Button,              button  = (Object *)getPopButton(),
               MUIA_Popobject_ObjStrHook,          hHkClose.GetHook(),
               MUIA_Popobject_StrObjHook,          hHkOpen.GetHook(),
            End)));

   return (IPTR)all;
}

IPTR MUIPopup::doOpen(IPTR a, IPTR x)
{
   return onOpen();
}

IPTR MUIPopup::doClose(IPTR, IPTR)
{
   bool bResult;
   bResult = onClose();
   update();
   return bResult;
}

void MUIPopup::open()
{
   DoMtd((Object *)all, ARRAY(MUIM_Popstring_Open));
}

void MUIPopup::close()
{
   DoMtd((Object *)all, ARRAY(MUIM_Popstring_Close, true));
}
