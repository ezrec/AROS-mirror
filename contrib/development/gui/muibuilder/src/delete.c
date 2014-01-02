/***************************************************************************

  MUIBuilder - MUI interface builder
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$$

***************************************************************************/

#include "builder.h"

void DeleteLabel(queue *file, APTR lab_obj)
{
    int i, n;

    n = file->nb_elements;
    for (i = 0; i < n; i++)
    {
        if (nth(file, i) == lab_obj)
        {
            delete_nth(file, i);
        }
    }
}

void DeleteSourceNotify(object *obj)
{
    int i, j, n, m;
    object *obj_aux;
    event *evt;

    n = obj->notifysource->nb_elements;
    for (i = 0; i < n; i++)
    {
        obj_aux = nth(obj->notifysource, 0);
        m = obj_aux->notify->nb_elements;
        for (j = 0; j < m; j++)
        {
            evt = nth(obj_aux->notify, j);
            if (evt->destination == obj)
            {
                delete_nth(obj_aux->notify, j);
                break;
            }
        }
        delete_nth(obj->notifysource, 0);
    }
    FreeVec(obj->notifysource);
}

void DeleteObjNotify(object *obj)
{
    int i, j, n, m;
    object *obj_aux;
    event *evt;

    n = obj->notify->nb_elements;
    for (i = 0; i < n; i++)
    {
        evt = nth(obj->notify, 0);
        obj_aux = evt->destination;
        m = obj_aux->notifysource->nb_elements;
        for (j = 0; j < m; j++)
        {
            if (nth(obj_aux->notifysource, j) == obj)
            {
                delete_nth(obj_aux->notifysource, j);
                break;
            }
        }
        FreeVec(evt);
        delete_nth(obj->notify, 0);
    }
    FreeVec(obj->notify);
}

void DeleteObject(APTR obj_aux)
{
    object *obj;
    window *win_aux;
    group *group_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    texte *text_aux;
    listview *list_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i, n;

    obj = (object *) obj_aux;
    if (obj->Help.nb_char > 0)
        FreeVec(obj->Help.content);
    DeleteSourceNotify(obj);
    DeleteObjNotify(obj);
    switch (obj->id)
    {
        case TY_WINDOW:
            win_aux = (window *) obj;
            delete_all(win_aux->groups);
            DeleteObject(&win_aux->root);
            if (win_aux->menu)
                DeleteObject(win_aux->menu);
            FreeVec(win_aux);
            break;
        case TY_GROUP:
            group_aux = (group *) obj;
            n = group_aux->child->nb_elements;
            for (i = 0; i < n; i++)
            {
                obj = nth(group_aux->child, i);
                DeleteObject(obj);
            }
            delete_all(group_aux->child);
            for (i = 0; i < n; i++)
            {
                FreeVec(nth(group_aux->entries, 0));
                delete_nth(group_aux->entries, 0);
            }
            FreeVec(group_aux->entries);
            FreeVec(group_aux->child);
            if (!group_aux->root)
                FreeVec(group_aux);
            break;
        case TY_CYCLE:
            cycle_aux = (cycle *) obj;
            n = cycle_aux->entries->nb_elements;
            for (i = 0; i < n; i++)
            {
                FreeVec(nth(cycle_aux->entries, 0));
                delete_nth(cycle_aux->entries, 0);
            }
            FreeVec(cycle_aux->entries);
            FreeVec(cycle_aux);
            break;
        case TY_RADIO:
            radio_aux = (radio *) obj;
            n = radio_aux->entries->nb_elements;
            for (i = 0; i < n; i++)
            {
                FreeVec(nth(radio_aux->entries, 0));
                delete_nth(radio_aux->entries, 0);
            }
            FreeVec(radio_aux->entries);
            FreeVec(radio_aux);
            break;
        case TY_TEXT:
            text_aux = (texte *) obj;
            FreeVec(text_aux);
            break;
        case TY_LISTVIEW:
            list_aux = (listview *) obj;
            FreeVec(list_aux);
            break;
        case TY_POPOBJECT:
            popobj_aux = (popobject *) obj;
            if (popobj_aux->obj)
                DeleteObject(popobj_aux->obj);
            FreeVec(obj);
            break;
        case TY_MENU:
            menu_aux = (menu *) obj;
            if ((menu_aux->id == TY_MENU) || (menu_aux->id == TY_SUBMENU))
            {
                n = menu_aux->childs->nb_elements;
                for (i = 0; i < n; i++)
                {
                    DeleteObject(nth(menu_aux->childs, 0));
                    delete_nth(menu_aux->childs, 0);
                }
            }
            FreeVec(menu_aux->childs);
            FreeVec(obj);
            break;
        case TY_SUBMENU:
            menu_aux = (menu *) obj;
            if ((menu_aux->id == TY_MENU) || (menu_aux->id == TY_SUBMENU))
            {
                n = menu_aux->childs->nb_elements;
                for (i = 0; i < n; i++)
                {
                    DeleteObject(nth(menu_aux->childs, 0));
                    delete_nth(menu_aux->childs, 0);
                }
            }
            FreeVec(menu_aux->childs);
            FreeVec(obj);
            break;
        case TY_MENUITEM:
            menu_aux = (menu *) obj;
            if ((menu_aux->id == TY_MENU) || (menu_aux->id == TY_SUBMENU))
            {
                n = menu_aux->childs->nb_elements;
                for (i = 0; i < n; i++)
                {
                    DeleteObject(nth(menu_aux->childs, 0));
                    delete_nth(menu_aux->childs, 0);
                }
            }
            FreeVec(menu_aux->childs);
            FreeVec(obj);
            break;
        default:
            FreeVec(obj);
            break;
    }
}
