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

#ifndef _GUIMUI_COMPONENTS_POPSTRING_H_
#define _GUIMUI_COMPONENTS_POPSTRING_H_

#include "MUIPopup.h"
#include "MUI.h"
#include <Generic/HookT.h>
#include <Generic/HookAttrT.h>
#include <Generic/String.h>

class MUIPopString : public MUIPopup
{
protected:
   enum
   {
      ID_String      =  100
   };

protected:
   Object                          *listview;
   Object                          *list;
   IPTR                             value;
   Object                          *string;
   HookAttrT<IPTR, IPTR>            hCallback;
   String                           sCols;
   int                              lCols;
   VectorT<const char*>             sColNames;

protected:
   HookT<MUIPopString, IPTR, IPTR>          hHkBtnHook;
   HookT<MUIPopString, IPTR, IPTR>          hHkConstruct;
   HookT<MUIPopString, IPTR, IPTR>          hHkDestruct;
   HookT<MUIPopString, const char**, IPTR>  hHkDisplay;

protected:
   virtual bool                     onOpen();
   virtual bool                     onClose();
   virtual IPTR                     getPopDisplay();
   virtual IPTR                     getPopButton();
   virtual IPTR                     getPopObject();
   virtual IPTR                     buttonHandler(IPTR id, IPTR data);

protected:
   virtual IPTR                     construct(IPTR, IPTR arg);
   virtual IPTR                     destruct(IPTR, IPTR arg);
   virtual IPTR                     display(const char** arr, IPTR arg);
   virtual const char              *toString(const void* arg);
public:
                                    MUIPopString(const char* name, int cols=1, const char** names=0);
   virtual                         ~MUIPopString();                                    
   virtual void                     setValue(IPTR val);
   virtual IPTR                     getObject();
   virtual IPTR                     getValue();
   virtual void                     clearList();
   virtual void                     addEntry(const IPTR);
   virtual const IPTR               getSelectedEntry();
};

#endif

