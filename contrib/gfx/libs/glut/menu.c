/*
 * DOS/DJGPP Mesa Utility Toolkit
 * Version:  1.0
 *
 * Copyright (C) 2005  Daniel Borca   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DANIEL BORCA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "internal.h"

#include <aros/debug.h>

extern struct AROSMesaGLUT_TaskNode     *_glut_findtask(struct Task *);

GLUTmenuStatusCB _glut_menu_status_func = NULL;

#define MAX_MENUS 50

GLUTmenu *_glut_menus[MAX_MENUS]; /* OLD! */

/** New Menu Lists - use these in future instead of _glut_menus[] **/
IPTR           _glut_menus_count;
struct List    _glut_menus_list;
GLUTmenu       *_glut_menu_current;

/* DO a quick run through the menu nodes adding the no. up */
ULONG _glut_count_menuentries(GLUTmenu *count_menu)
{
  GLUTmenuentry  *menuentrynode = NULL;   
  ULONG          _menu_strip_itemcount = 0;
    
  ForeachNode(&count_menu->_gmn_Entries, menuentrynode)
  {
    if ((menuentrynode->_gme_type == GLUT_MENU_ENTRY_ITEM)) _menu_strip_itemcount ++;
    else if ((menuentrynode->_gme_type == GLUT_MENU_SUBMENU_ITEM))
    {
      /* Add one for the submenu itself */
      _menu_strip_itemcount = _menu_strip_itemcount + 1 +
                              _glut_count_menuentries((GLUTmenu *)menuentrynode->_gme_value);
    }
  }
  return _menu_strip_itemcount;
}

/* Create a MenuStrip from a list of menu(s)/node(s) */
ULONG _glut_create_newmenu(struct NewMenu *menustrip_newmenu, GLUTmenu *glut_menu, GLboolean submenu)
{
  GLUTmenuentry  *menuentrynode = NULL;
  struct NewMenu *current_newmenuentry = menustrip_newmenu;
  ULONG          _menu_strip_itemcount = 0;


  D(bug("[AMGLUT] _glut_create_newmenu: menustrip_newmenu = %x\n", menustrip_newmenu));


  if (!(submenu))
  {
    current_newmenuentry->nm_Type = NM_TITLE;
    current_newmenuentry->nm_Label = "Project ..";
    current_newmenuentry->nm_UserData = glut_menu;
    _menu_strip_itemcount ++;
  }
    
  ForeachNode(&glut_menu->_gmn_Entries, menuentrynode)
  {
    current_newmenuentry = &menustrip_newmenu[_menu_strip_itemcount];

    D(bug("[AMGLUT] _glut_create_newmenu: current_newmenu = %x\n", current_newmenuentry));


    if ((menuentrynode->_gme_type == GLUT_MENU_ENTRY_ITEM))
    {
       if (!(submenu)) current_newmenuentry->nm_Type = NM_ITEM;
       else  current_newmenuentry->nm_Type = NM_SUB;

       current_newmenuentry->nm_Label = menuentrynode->_gme_label;
       current_newmenuentry->nm_UserData = menuentrynode;
       _menu_strip_itemcount ++;
    } else if ((menuentrynode->_gme_type == GLUT_MENU_SUBMENU_ITEM))
    {
      /* Add one for the submenu itself */
       current_newmenuentry->nm_Type = NM_ITEM;
       current_newmenuentry->nm_Label = menuentrynode->_gme_label;
       current_newmenuentry->nm_UserData = menuentrynode;
       _menu_strip_itemcount ++;
      _menu_strip_itemcount = _menu_strip_itemcount +
                              _glut_create_newmenu(&menustrip_newmenu[_menu_strip_itemcount], (GLUTmenu *)menuentrynode->_gme_value, GL_TRUE);
    }
  }
  if ((!(submenu)) && (_menu_strip_itemcount > 1))
  {
    current_newmenuentry = &menustrip_newmenu[_menu_strip_itemcount];
    current_newmenuentry->nm_Type = NM_END;
  }

  return _menu_strip_itemcount;
}

/* Update the displayed windows for the current windows (using the current menus) */
void _glut_update_window_menus (void)
{

  D(bug("[AMGLUT] In _glut_update_window_menus()\n"));

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
  if ((__glutTask->AMGLUTTN_WindowCurrent) && (__glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowGlutMenu))
  {
    /* Build a new MenuStrip */
    struct NewMenu *_menu_strip_NewMenu = NULL;
    struct Menu    *_window_menu_strip = NULL;
    ULONG          _menu_strip_itemcount = 0;
    
    _menu_strip_itemcount = _glut_count_menuentries(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowGlutMenu);

    D(bug("[AMGLUT] _glut_update_window_menus: Allocating NewMenu struct for %d items\n", _menu_strip_itemcount));


    if ((_menu_strip_NewMenu = (struct NewMenu *)AllocVec(sizeof(struct NewMenu) * (_menu_strip_itemcount + 2), MEMF_CLEAR|MEMF_PUBLIC)) == NULL) {
      return;
    }

    D(bug("[AMGLUT] _glut_update_window_menus: temp NewMenu struct  @ %x\n", _menu_strip_NewMenu));


    _glut_create_newmenu(_menu_strip_NewMenu, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowGlutMenu, GL_FALSE);
    _window_menu_strip = CreateMenusA(_menu_strip_NewMenu, NULL);

    if (_window_menu_strip)
    {
      D(bug("[AMGLUT] _glut_update_window_menus: menu strip created  @ %x\n", _window_menu_strip));

      APTR vi = GetVisualInfoA(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_Window->WScreen, NULL);
      if (vi)
      {
        /* Remove the current menu if any */
        if ((__glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip))
        {
          ClearMenuStrip(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_Window);
          __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip = NULL;
        }

        /* Add the new menustrip to the window */
        __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip = _window_menu_strip;
        if ((LayoutMenusA(_window_menu_strip, vi, NULL)))
        {
          __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
          SetMenuStrip(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_Window, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip);
          return;
        }
      }
    }
  }
}

void APIENTRY
glutMenuStateFunc (GLUTmenuStateCB func)
{
   _glut_menu_status_func = (GLUTmenuStatusCB)func;
}


void APIENTRY
glutMenuStatusFunc (GLUTmenuStatusCB func)
{
   _glut_menu_status_func = func;
}


int APIENTRY
glutCreateMenu (GLUTselectCB func)
{
  int i;

  D(bug("[AMGLUT] In glutCreateMenu()\n"));


  /* Search for an empty slot. */
  for (i = 0; i < MAX_MENUS; i++)
  {
    if (_glut_menus[i] == NULL)
    {
      GLUTmenu *menunode = NULL;      

      if ((menunode = (GLUTmenu *)AllocVec(sizeof(GLUTmenu), MEMF_CLEAR|MEMF_PUBLIC)) == NULL) {
        return 0;
      }

      _glut_menu_current = _glut_menus[i] = menunode;
      NEWLIST(&menunode->_gmn_Entries);
      menunode->_gmn_CB = func;
      menunode->_gmn_ID = ++i;
      D(bug("[AMGLUT] glutCreateMenu: GLUTMenu Node @ %x [ID %d, callback @ %x]\n", menunode, menunode->_gmn_ID, menunode->_gmn_CB));

      return i;
    }
  }
  return 0;
}


void APIENTRY
glutDestroyMenu (int menu)
{
}


int APIENTRY
glutGetMenu (void)
{
   return 0;
}


void APIENTRY
glutSetMenu (int menu)
{
}


void APIENTRY
glutAddMenuEntry (const char *label, int value)
{

  D(bug("[AMGLUT] In glutAddMenuEntry('%s')\n", label));

  if (_glut_menu_current)
  {
    GLUTmenuentry *menuentrynode = NULL;      

    if ((menuentrynode = (GLUTmenuentry *)AllocVec(sizeof(GLUTmenuentry), MEMF_CLEAR|MEMF_PUBLIC)) == NULL) {
      return;
    }

    menuentrynode->_gme_type = GLUT_MENU_ENTRY_ITEM;
    menuentrynode->_gme_label = AllocVec(strlen(label) + 1, MEMF_CLEAR|MEMF_PUBLIC);
    strcpy(menuentrynode->_gme_label, label);
    menuentrynode->_gme_value = value;    


    D(bug("[AMGLUT] glutAddMenuEntry: GLUTmenuentry Node @ %x ['%s' value %d]\n", menuentrynode, menuentrynode->_gme_label, menuentrynode->_gme_value));


    _glut_menu_current->_gmn_Entry_Count ++;
    AddTail(&_glut_menu_current->_gmn_Entries, &menuentrynode->_gme_Node);
  }
}


void APIENTRY
glutAddSubMenu (const char *label, int submenu)
{
  D(bug("[AMGLUT] In glutAddSubMenu('%s')\n", label));

  if (_glut_menu_current)
  {
    GLUTmenuentry *menuentrynode = NULL;      

    if ((menuentrynode = (GLUTmenuentry *)AllocVec(sizeof(GLUTmenuentry), MEMF_CLEAR|MEMF_PUBLIC)) == NULL) {
      return;
    }

    menuentrynode->_gme_type = GLUT_MENU_SUBMENU_ITEM;
    menuentrynode->_gme_label = AllocVec(strlen(label), MEMF_CLEAR|MEMF_PUBLIC);
    strcpy(menuentrynode->_gme_label, label);
    menuentrynode->_gme_value = (SIPTR)_glut_menus[submenu-1];
    
    D(bug("[AMGLUT] glutAddSubMenu: GLUTmenuentry Node @ %x ['%s' SubMenu @ %x]\n", menuentrynode, menuentrynode->_gme_label, menuentrynode->_gme_value));

    _glut_menu_current->_gmn_Entry_Count ++;
    AddTail(&_glut_menu_current->_gmn_Entries, &menuentrynode->_gme_Node);
  }
}


void APIENTRY
glutChangeToMenuEntry (int item, const char *label, int value)
{
}


void APIENTRY
glutChangeToSubMenu (int item, const char *label, int submenu)
{
}


void APIENTRY
glutRemoveMenuItem (int item)
{
}


void APIENTRY
glutAttachMenu (int button)
{
  D(bug("[AMGLUT] In glutAttachMenu()\n"));

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
  if ((__glutTask->AMGLUTTN_WindowCurrent) && (_glut_menu_current))
  {
    if ((button == GLUT_LEFT_BUTTON) || (button == GLUT_MIDDLE_BUTTON))
    {
      D(bug("[AMGLUT] glutAttachMenu: WARNING: Unsupported menu button\n"));
    }
    else if ((button == GLUT_RIGHT_BUTTON))
    {
      D(bug("[AMGLUT] glutAttachMenu: Updating menu for right mouse button\n"));
      __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowGlutMenu = _glut_menu_current;
      _glut_update_window_menus();
    }
  }
}


void APIENTRY
glutDetachMenu (int button)
{
  D(bug("[AMGLUT] In glutDetachMenu()\n"));

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
  if ((__glutTask->AMGLUTTN_WindowCurrent))
  {
    if ((button == GLUT_LEFT_BUTTON) || (button == GLUT_MIDDLE_BUTTON))
    {
      D(bug("[AMGLUT] glutDetachMenu: WARNING: Unsupported menu button\n"));
    }
    else if ((button == GLUT_RIGHT_BUTTON))
    {
      if ((__glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip))
      {
        D(bug("[AMGLUT] glutDetachMenu: Detaching menu for right mouse button\n"));

        ClearMenuStrip(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_Window);
        __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowGlutMenu = NULL;
        __glutTask->AMGLUTTN_WindowCurrent->amglutwin_WindowMenuStrip = NULL;
      }
    }
  }
}


void APIENTRY
glutMenuDestroyFunc ( void (* callback)( void ) )
{
}


void * APIENTRY
glutGetMenuData (void)
{
   return NULL;
}


void APIENTRY
glutSetMenuData (void *data)
{
}
