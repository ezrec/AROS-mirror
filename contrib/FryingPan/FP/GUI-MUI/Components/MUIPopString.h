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
   const void                      *value;
   Object                          *string;
   HookAttrT<void*, void*>          hCallback;
   String                           sCols;
   int                              lCols;
   VectorT<const char*>             sColNames;

protected:
   HookT<MUIPopString, int, void*>           hHkBtnHook;
   HookT<MUIPopString, void*, void*>         hHkConstruct;
   HookT<MUIPopString, void*, void*>         hHkDestruct;
   HookT<MUIPopString, const char**, void*>  hHkDisplay;

protected:
   virtual bool                     onOpen();
   virtual bool                     onClose();
   virtual unsigned long           *getPopDisplay();
   virtual unsigned long           *getPopButton();
   virtual unsigned long           *getPopObject();
   virtual unsigned long            buttonHandler(int id, void* data);

protected:
   virtual unsigned long            construct(void*, void* arg);
   virtual unsigned long            destruct(void*, void* arg);
   virtual unsigned long            display(const char** arr, void* arg);
   virtual const char              *toString(const void* arg);
public:
                                    MUIPopString(const char* name, int cols=1, const char** names=0);
   virtual                         ~MUIPopString();                                    
   virtual void                     setValue(const void* string);
   virtual unsigned long           *getObject();
   virtual const void              *getValue();
   virtual void                     clearList();
   virtual void                     addEntry(const void*);
   virtual const void              *getSelectedEntry();
};

#endif

