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

#ifndef _GUIMUI_COMPONENTS_MUITREE_H_
#define _GUIMUI_COMPONENTS_MUITREE_H_

#include <Generic/GenericMUI.h>
#include <Generic/String.h>
#include <Generic/HookAttrT.h>
#include <Generic/HookT.h>
#include <Generic/VectorT.h>

struct MUIP_NListtree_DisplayMessage;
struct MUIP_NListtree_ConstructMessage;
struct MUIP_NListtree_DestructMessage;

class Hook;
using namespace GenNS;

class MUITree : public virtual GenericMUI
{
protected:
   Object                 *listview;
   Object                 *list;
   bool                    multiselect;
   String                  format;
   HookAttrT<IPTR, IPTR>   hConstruct;
   HookAttrT<IPTR, IPTR>   hDestruct;
   HookAttrT<IPTR, IPTR>   hDisplay;
   HookAttrT<IPTR, IPTR>   hSelect;
   VectorT<IPTR>           hSelected;

protected:
   HookT<MUITree, IPTR, MUIP_NListtree_ConstructMessage*>     hHkConstruct;
   HookT<MUITree, IPTR, MUIP_NListtree_DestructMessage*>      hHkDestruct;
   HookT<MUITree, IPTR, MUIP_NListtree_DisplayMessage*>       hHkDisplay;
   HookT<MUITree, IPTR, IPTR>                                 hHkSelect;

protected:
   virtual IPTR            doConstruct(IPTR, MUIP_NListtree_ConstructMessage*);
   virtual IPTR            doDestruct(IPTR, MUIP_NListtree_DestructMessage*);
   virtual IPTR            doDisplay(IPTR, MUIP_NListtree_DisplayMessage*);
   virtual IPTR            doSelect(IPTR, IPTR);

public:
                           MUITree(const char *format, bool multiselect=false);
   virtual                ~MUITree(); 
   virtual IPTR            getObject();
   virtual void            setConstructHook(const Hook* hook);       // void*, <source>
   virtual void            setDestructHook(const Hook* hook);        // void*, <element>
   virtual void            setDisplayHook(const Hook* hook);         // const char**, <element>
   virtual void            setSelectionHook(const Hook* hook);       // <element> active, void*
   virtual void            setWeight(int weight);

   virtual IPTR            addEntry(IPTR parent, IPTR data, bool opened=false);
   virtual void            showObject(IPTR data, bool expand);
   virtual void            clear();
   virtual VectorT<IPTR>  &getSelectedObjects();

   virtual void            setEnabled(bool enabled);
};

#endif

