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

#ifndef _GUIMUI_POPISOELEMENT_H_
#define _GUIMUI_POPISOELEMENT_H_ 

#include "Components/MUIPopup.h"
#include "Components/MUI.h"
#include <Generic/HookT.h>
#include <Generic/HookAttrT.h>
#include <Generic/String.h>
#include "Globals.h"

#include <ISOBuilder/IBrowser.h>

class MUIPopISOElement : public MUIPopup
{
protected:
   enum BtnID
   {
      ID_String      =  100,
      
      ID_NormalName  =  1000,
      ID_ISOName,
      ID_RRName,
      ID_JolietName,
      ID_Comment,

      ID_AddISOEntry =  1100,
      ID_AddRREntry,
      ID_AddJolietEntry,

      ID_ISOLevel    =  2000,
      ID_AddRockRidge,
      ID_AddJoliet,
      ID_RelaxedISO,

      ID_VolumeName  =  2100,
      ID_SystemType,
      ID_VolumeSetName,
      ID_Publisher,
      ID_Preparer,
      ID_Application
   };

protected:
   String                           value;
   IPTR                             string;

   IPTR                             page;

   IBrowser                        *browser;
   ClElement                       *elem;
   Globals                         &Glb;

   HookAttrT<IPTR, IPTR>           hCallback;

protected:
   HookT<MUIPopISOElement, IPTR, IPTR>           hHkBtnHook;

protected:
   virtual bool                     onOpen();
   virtual bool                     onClose();
   virtual IPTR                     getPopDisplay();
   virtual IPTR                     getPopButton();
   virtual IPTR                     getPopObject();
   virtual IPTR                     buttonHandler(IPTR id, IPTR data);

   virtual IPTR                     getElemSettings();
   virtual IPTR                     getISOSettings();
public:
                                    MUIPopISOElement(Globals &glb);
   virtual                         ~MUIPopISOElement();
   virtual void                     setValue(IPTR val);
   virtual IPTR                     getValue();
   virtual IPTR                     getObject();

   virtual void                     setBrowser(IBrowser *pBser);
   virtual void                     setElement(ClElement *pElem);

   virtual void                     setEnabled(bool enabled);
};

#endif

