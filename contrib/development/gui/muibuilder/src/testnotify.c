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

#include "testnotify.h"

int GetEvent(int EVT[], int id)
{
    return (EVT[id]);
}

int GetAction(int ACT[], int id)
{
    return (ACT[id]);
}

BOOL IsApplicationMenu(menu *menu_aux)
{
    if (menu_aux == application.appmenu)
        return (TRUE);
    if ((menu_aux->id == TY_MENU) ||
        (menu_aux->id == TY_SUBMENU) || (menu_aux->id == TY_MENUITEM))
        return (IsMenuParent(application.appmenu, menu_aux));
    else
        return (FALSE);
}

void ObjectNotify(object *obj)
{
    int i, id_src, id_dest;
    object *obj_aux, *obj_aux2;
    event *evt;

    id_src = obj->id;
    for (i = 0; i < obj->notify->nb_elements; i++)
    {
        evt = nth(obj->notify, i);
        obj_aux = evt->destination;
        id_dest = obj_aux->id;
        if (id_dest > 0)
            switch (TYActions[id_dest][evt->dest_type])
            {
                case TY_NOTHING:
                    DoMethod(obj->muiobj,
                             MUIM_Notify,
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 0),
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 1),
                             obj_aux->muiobj, GetAction(NActions[id_dest],
                                                        4 *
                                                        evt->dest_type +
                                                        0),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 1),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 2),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 3));
                    break;
                case TY_WINOBJ:
                    obj_aux2 = evt->arguments;
                    DoMethod(obj->muiobj,
                             MUIM_Notify,
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 0),
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 1),
                             obj_aux2->muiobj, GetAction(NActions[id_dest],
                                                         4 *
                                                         evt->dest_type +
                                                         0),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 1),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 2),
                             obj_aux->muiobj);
                    break;
                case TY_CONS_INT:
                    if (NActions[id_dest][4 * evt->dest_type + 0] > 2)
                    {
                        DoMethod(obj->muiobj,
                                 MUIM_Notify,
                                 GetEvent(NEvenements[id_src],
                                          2 * evt->src_type + 0),
                                 GetEvent(NEvenements[id_src],
                                          2 * evt->src_type + 1),
                                 obj_aux->muiobj,
                                 GetAction(NActions[id_dest],
                                           4 * evt->dest_type + 0),
                                 GetAction(NActions[id_dest],
                                           4 * evt->dest_type + 1),
                                 GetAction(NActions[id_dest],
                                           4 * evt->dest_type + 2),
                                 atoi(evt->argstring));
                    }
                    else
                    {
                        DoMethod(obj->muiobj,
                                 MUIM_Notify,
                                 GetEvent(NEvenements[id_src],
                                          2 * evt->src_type + 0),
                                 GetEvent(NEvenements[id_src],
                                          2 * evt->src_type + 1),
                                 obj_aux->muiobj,
                                 GetAction(NActions[id_dest],
                                           4 * evt->dest_type + 0),
                                 GetAction(NActions[id_dest],
                                           4 * evt->dest_type + 1),
                                 atoi(evt->argstring));
                    }
                    break;
                case TY_CONS_CHAR:
                    DoMethod(obj->muiobj,
                             MUIM_Notify,
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 0),
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 1),
                             obj_aux->muiobj, GetAction(NActions[id_dest],
                                                        4 *
                                                        evt->dest_type +
                                                        0),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 1),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 2),
                             evt->argstring[0]);
                    break;
                case TY_CONS_STRING:
                    DoMethod(obj->muiobj,
                             MUIM_Notify,
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 0),
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 1),
                             obj_aux->muiobj, GetAction(NActions[id_dest],
                                                        4 *
                                                        evt->dest_type +
                                                        0),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 1),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 2),
                             evt->argstring);
                    break;
                case TY_CONS_BOOL:
                    DoMethod(obj->muiobj,
                             MUIM_Notify,
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 0),
                             GetEvent(NEvenements[id_src],
                                      2 * evt->src_type + 1),
                             obj_aux->muiobj, GetAction(NActions[id_dest],
                                                        4 *
                                                        evt->dest_type +
                                                        0),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 1),
                             GetAction(NActions[id_dest],
                                       4 * evt->dest_type + 2),
                             (evt->argstring[0] == '1'));
                    break;
                default:
                    break;
            }
    }
}

void TestNotify(object *obj)
{
    window *win_aux;
    group *group_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i;

    switch (obj->id)
    {
        case TY_APPLI:
            TestNotify((object *) application.appmenu);
            for (i = 0; i < windows->nb_elements; i++)
            {
                TestNotify(nth(windows, i));
            }
            break;
        case TY_WINDOW:
            win_aux = (window *) obj;
            ObjectNotify(obj);
            TestNotify((object *) win_aux->menu);
            for (i = 0; i < win_aux->root.child->nb_elements; i++)
            {
                TestNotify(nth(win_aux->root.child, i));
            }
            break;
        case TY_GROUP:
            group_aux = (group *) obj;
            ObjectNotify(obj);
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                TestNotify(nth(group_aux->child, i));
            }
            break;
        case TY_POPOBJECT:
            popobj_aux = (popobject *) obj;
            ObjectNotify(obj);
            TestNotify(popobj_aux->obj);
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = (menu *) obj;
            ObjectNotify(obj);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                TestNotify(nth(menu_aux->childs, i));
            }
            break;
        default:
            ObjectNotify(obj);
            break;
    }
}
