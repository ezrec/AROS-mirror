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

#ifndef _GUIMUI_COMPONENTS_MUILIST_H_
#define _GUIMUI_COMPONENTS_MUILIST_H_

#include <Generic/GenericMUI.h>
#include <Generic/String.h>
#include <Generic/VectorT.h>
#include <Generic/HookT.h>
#include <Generic/HookAttrT.h>
#include <workbench/workbench.h>

class Hook;

using namespace GenNS;

class MUIList : public virtual GenericMUI
{
protected:
   Object                 *listview;
   Object                 *list;
   bool                    nlist;
   bool                    multiSelect;
   bool                    dragSortable;
   String                  format;
   VectorT<IPTR>           selected;
   HookAttrT<IPTR, IPTR>          hOnSelect;
   HookAttrT<IPTR, AppMessage*>   hOnWBDrop;
   HookAttrT<IPTR, IPTR>          hOnSort;

protected:
   HookT<MUIList, IPTR, IPTR>          hHkOnSelect;         //
   HookT<MUIList, IPTR, AppMessage**>  hHkOnWBDrop;         //
   HookT<MUIList, IPTR, IPTR>          hHkOnDragSort;       // will call with new vector as param.

protected:
   IPTR                    onSelect(IPTR, IPTR);
   IPTR                    onWBDrop(IPTR, AppMessage**);
   IPTR                    onDragSort(IPTR, IPTR);

public:
                           MUIList(const char *format, bool bMultiSelect=false);
   virtual                ~MUIList(); 
   virtual IPTR            getObject();
   virtual void            setConstructHook(const Hook* hook);
   virtual void            setDestructHook(const Hook* hook);
   virtual void            setDisplayHook(const Hook* hook);
   virtual void            setSelectionHook(const Hook* hook);
   virtual void            setWBDropHook(const Hook* hook);
   virtual void            setDragSortHook(const Hook* hook);

   virtual void            addItem(IPTR item);
   virtual void            clear();
   virtual VectorT<IPTR>  &getSelectedItems();

   virtual void            setEnabled(bool enabled);
   virtual void            setDragSortable(bool sortable);
};

#endif

