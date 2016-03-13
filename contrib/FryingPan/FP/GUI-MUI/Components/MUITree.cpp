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

#include "MUITree.h"
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>

MUITree::MUITree(const char *format, bool multiselect)
{
   listview = 0;
   list     = 0;
   this->multiselect = multiselect;
   this->format      = format;

   hHkConstruct.Initialize(this, &MUITree::doConstruct);
   hHkDestruct.Initialize(this, &MUITree::doDestruct);
   hHkDisplay.Initialize(this, &MUITree::doDisplay);
   hHkSelect.Initialize(this, &MUITree::doSelect);
}

MUITree::~MUITree()
{
}

IPTR MUITree::getObject()
{
   if (0 != listview)
      return (IPTR)listview;

   list = NListtreeObject,
      InputListFrame,
      MUIA_NListtree_Format,        (IPTR)format.Data(),    
      MUIA_NListtree_Title,         true,
      MUIA_NListtree_MultiSelect,   multiselect,
      MUIA_NListtree_ConstructHook, (IPTR)hHkConstruct.GetHook(),
      MUIA_NListtree_DestructHook,  (IPTR)hHkDestruct.GetHook(),
      MUIA_NListtree_DisplayHook,   (IPTR)hHkDisplay.GetHook(),
      MUIA_NListtree_DragDropSort,  false,
   End;

   if (list != 0)
   {
      listview = NListviewObject,
         MUIA_NListview_NList,      (IPTR)list,
      End;
   
      DoMtd(list, ARRAY(MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime, (IPTR)list, 2, MUIM_CallHook, (IPTR)hHkSelect.GetHook()));
   }
   else
   {
      ASSERTS(false, "NListTree could not be found! Memory will leak a little..");
      return (IPTR)TextObject, 
         MUIA_Text_Contents,     "NListTree could not be found!",
      End;
   }


   return (IPTR)listview;
}

void MUITree::setConstructHook(const Hook* hook)
{
   hConstruct = hook;
}

void MUITree::setDestructHook(const Hook* hook)
{
   hDestruct = hook;
}

void MUITree::setDisplayHook(const Hook* hook)
{
   hDisplay = hook;
}

void MUITree::setSelectionHook(const Hook* hook)
{
   hSelect = hook;
}

void MUITree::setWeight(int weight)
{
   Object *obj = (Object *)getObject();

   if (0 == obj)
      return;

   Intuition->SetAttrsA(obj, (TagItem*)ARRAY(
      MUIA_Weight,                  weight,
      TAG_DONE,                     0
   ));

}

IPTR MUITree::doConstruct(IPTR, MUIP_NListtree_ConstructMessage* m)
{
   if (hConstruct.IsValid())
   {
      return hConstruct.Call((IPTR)this, (IPTR)m->UserData);
   }
   else
   {
      return (IPTR)m->UserData;
   }
}

IPTR MUITree::doDestruct(IPTR, MUIP_NListtree_DestructMessage* m)
{
   if (hDestruct.IsValid())
   {
      return hDestruct.Call((IPTR)this, (IPTR)m->UserData);
   }
   else
   {
      return 0;
   }
}

IPTR MUITree::doDisplay(IPTR, MUIP_NListtree_DisplayMessage* m)
{
   if (hDisplay.IsValid())
   {
      if (m->EntryPos >= 0)
         return hDisplay.Call((IPTR)m->Array, (IPTR)m->TreeNode->tn_User);
      else
         return hDisplay.Call((IPTR)m->Array, (IPTR)NULL);
   }
   else
   {
      /*
       * due to the fact that we put just dummy data as node name, 
       * we need to pass the UserData as the name here :)
       */
      if (m->EntryPos >= 0)
         m->Array[0] = (char *)m->TreeNode->tn_User;
      else
         m->Array[0] = (char *)"Tree";
      return 0;
   }
}

IPTR MUITree::doSelect(IPTR, IPTR)
{
   IPTR data = 0;
   MUI_NListtree_TreeNode *tn = 0;
   tn = (MUI_NListtree_TreeNode*)DoMtd((Object *)list, ARRAY(MUIM_NListtree_GetEntry, 
                                                   (IPTR)MUIV_NListtree_GetEntry_ListNode_Active,
                                                   (IPTR)MUIV_NListtree_GetEntry_Position_Active,
                                                   0));
   if (tn != 0)
      data = (IPTR)tn->tn_User;

   return hSelect.Call(data, data);
}

IPTR MUITree::addEntry(IPTR parent, IPTR entry, bool opened)
{
   if (0 == list)
      getObject();

   if (0 == list)
      return 0;

   if (0 == parent)
      parent = MUIV_NListtree_Insert_ListNode_Root;

   return DoMtd((Object *)list, ARRAY(MUIM_NListtree_Insert, (IPTR)"*", (IPTR)entry, parent, (IPTR)MUIV_NListtree_Insert_PrevNode_Tail, (IPTR)TNF_LIST | (opened ? TNF_OPEN : 0)));
}

void MUITree::clear()
{
   hSelected.Empty();

   if (0 == list)
      getObject();

   if (0 == list)
      return;

   DoMtd((Object *)list, ARRAY(MUIM_NListtree_Clear, 0, 0));  
}

VectorT<IPTR> &MUITree::getSelectedObjects()
{
   MUI_NListtree_TreeNode *sel = (MUI_NListtree_TreeNode*)MUIV_NListtree_NextSelected_Start;

   hSelected.Empty();

   while (true)
   {
      DoMtd((Object *)list, ARRAY(MUIM_NListtree_NextSelected, (IPTR)&sel));
      if (sel == (MUI_NListtree_TreeNode*)MUIV_NListtree_NextSelected_End)
         break;
      hSelected << (IPTR)sel->tn_User;
   }
   
   return hSelected;
}

void MUITree::showObject(IPTR data, bool expand)
{
   MUI_NListtree_TreeNode *tn;

   tn = (MUI_NListtree_TreeNode*)DoMtd((Object *)list, ARRAY(MUIM_NListtree_FindUserData, 
                                                   MUIV_NListtree_FindUserData_ListNode_Root, 
                                                   data, 
                                                   MUIV_NListtree_FindUserData_Flag_Activate));
   (void)tn; // Unused
   if (expand)
   {
      DoMtd((Object *)list, ARRAY(MUIM_NListtree_Open, 
                        (IPTR)MUIV_NListtree_Open_ListNode_Parent, 
                        (IPTR)MUIV_NListtree_Open_TreeNode_Active));
   }
}

void MUITree::setEnabled(bool enabled)
{
   Intuition->SetAttrsA(listview, (TagItem*)ARRAY(MUIA_Disabled, !enabled, TAG_DONE, 0));
}


