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

BOOL createwin;

/* effectue la copie de la partie commune a tous les objets */
BOOL CopyObject(object *objsrc, object *objdest, int taille, APTR father)
{
    APTR helptext;
    BOOL result = TRUE;

    memcpy(objdest, objsrc, taille);
    strcat(objdest->label, "C");
    objdest->father = father;
    /* on ne copie pas les notifications */
    objdest->notify = create();
    objdest->notifysource = create();
    /* on duplique le texte d'aide */
    if (objsrc->Help.nb_char > 0)
    {
        helptext =
            AllocVec(objsrc->Help.nb_char + 1, MEMF_PUBLIC | MEMF_CLEAR);
        if (helptext)
        {
            memcpy(helptext, objsrc->Help.content, objsrc->Help.nb_char);
            objdest->Help.content = helptext;
        }
        else
        {
            result = FALSE;
            ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
        }
    }
    return (result);
}

/* duplique un objet */
APTR Duplicate(APTR src, APTR father)
{
    object *objsrc, *objdest;
    window *win1_aux, *win2_aux;
    group *group1_aux, *group2_aux;
    cycle *cycle1_aux, *cycle2_aux;
    radio *radio1_aux, *radio2_aux;
    popobject *popobj1, *popobj2;
    menu *menu1, *menu2;
    int i;
    char *chaine_aux;
    APTR result;
    BOOL copyok;

    objsrc = src;

    switch (objsrc->id)
    {
        case TY_WINDOW:
            win1_aux = src;
            win2_aux = AllocVec(sizeof(window), MEMF_PUBLIC | MEMF_CLEAR);
            if (!win2_aux)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject((object *) win1_aux, (object *) win2_aux,
                           sizeof(window), father);
            if (!copyok)
                return (NULL);

            /* la liste des groupes est initialisee a vide */
            win2_aux->groups = create();

            /* on copie le groupe 'root' */
            copyok =
                CopyObject((object *) &win1_aux->root,
                           (object *) &win2_aux->root, sizeof(group),
                           father);
            if (!copyok)
                return (NULL);
            win2_aux->root.father = win2_aux;
            win2_aux->root.child = create();
            win2_aux->root.entries = create();
            win2_aux->menu = Duplicate(win1_aux->menu, win2_aux);
            for (i = 0; i < win1_aux->root.child->nb_elements; i++)
            {
                add(win2_aux->root.child,
                    Duplicate(nth(win1_aux->root.child, i),
                              &win2_aux->root));
            }
            for (i = 0; i < win1_aux->root.entries->nb_elements; i++)
            {
                chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
                if (!chaine_aux)
                {
                    ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                    return (NULL);
                }
                strncpy(chaine_aux, nth(win1_aux->root.entries, i), 80);
                win2_aux->root.registertitles[i] = chaine_aux;
                add(win2_aux->root.entries, chaine_aux);
            }
            result = win2_aux;
            break;
        case TY_GROUP:
            group1_aux = src;
            group2_aux = AllocVec(sizeof(group), MEMF_PUBLIC | MEMF_CLEAR);
            if (!group2_aux)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject((object *) group1_aux, (object *) group2_aux,
                           sizeof(group), father);
            if (!copyok)
                return (NULL);
            group2_aux->child = create();
            group2_aux->entries = create();
            group2_aux->root = FALSE;
            for (i = 0; i < group1_aux->child->nb_elements; i++)
            {
                add(group2_aux->child,
                    Duplicate(nth(group1_aux->child, i), group2_aux));
            }
            for (i = 0; i < group1_aux->entries->nb_elements; i++)
            {
                chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
                if (!chaine_aux)
                {
                    ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                    break;
                }
                strncpy(chaine_aux, nth(group1_aux->entries, i), 80);
                group2_aux->registertitles[i] = chaine_aux;
                add(group2_aux->entries, chaine_aux);
            }
            result = group2_aux;
            break;
        case TY_KEYBUTTON:
            objdest = AllocVec(sizeof(bouton), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(bouton), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_STRING:
            objdest = AllocVec(sizeof(chaine), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(chaine), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_LISTVIEW:
            objdest = AllocVec(sizeof(listview), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(listview), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_GAUGE:
            objdest = AllocVec(sizeof(gauge), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(gauge), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_CYCLE:
            objdest = AllocVec(sizeof(cycle), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(cycle), father);
            if (!copyok)
                return (NULL);
            cycle1_aux = src;
            cycle2_aux = (cycle *) objdest;
            cycle2_aux->entries = create();
            for (i = 0; i < cycle1_aux->entries->nb_elements; i++)
            {
                chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
                if (!chaine_aux)
                {
                    ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                    break;
                }
                strncpy(chaine_aux, nth(cycle1_aux->entries, i), 80);
                add(cycle2_aux->entries, chaine_aux);
                cycle2_aux->STRA_cycle[i] = chaine_aux;
            }
            result = objdest;
            break;
        case TY_RADIO:
            objdest = AllocVec(sizeof(radio), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(radio), father);
            if (!copyok)
                return (NULL);
            radio1_aux = src;
            radio2_aux = (radio *) objdest;
            radio2_aux->entries = create();
            for (i = 0; i < radio1_aux->entries->nb_elements; i++)
            {
                chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
                if (!chaine_aux)
                {
                    ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                    break;
                }
                strncpy(chaine_aux, nth(radio1_aux->entries, i), 80);
                add(radio2_aux->entries, chaine_aux);
                radio2_aux->STRA_radio[i] = chaine_aux;
            }
            result = objdest;
            break;
        case TY_LABEL:
            objdest = AllocVec(sizeof(label), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(label), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_SPACE:
            objdest = AllocVec(sizeof(space), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(space), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_CHECK:
            objdest = AllocVec(sizeof(check), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(check), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_SCALE:
            objdest = AllocVec(sizeof(scale), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(scale), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_IMAGE:
            objdest = AllocVec(sizeof(image), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(image), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_SLIDER:
            objdest = AllocVec(sizeof(slider), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(slider), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_TEXT:
            objdest = AllocVec(sizeof(texte), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(texte), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_PROP:
            objdest = AllocVec(sizeof(prop), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(prop), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_DIRLIST:
            objdest = AllocVec(sizeof(dirlist), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(dirlist), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_RECTANGLE:
            objdest =
                AllocVec(sizeof(rectangle), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject(objsrc, objdest, sizeof(rectangle), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_POPASL:
            objdest =
                AllocVec(sizeof(popaslobj), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject(objsrc, objdest, sizeof(popaslobj), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_COLORFIELD:
            objdest =
                AllocVec(sizeof(colorfield), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject(objsrc, objdest, sizeof(colorfield), father);
            if (!copyok)
                return (NULL);
            result = objdest;
            break;
        case TY_POPOBJECT:
            objdest =
                AllocVec(sizeof(popobject), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok =
                CopyObject(objsrc, objdest, sizeof(popobject), father);
            if (!copyok)
                return (NULL);
            popobj1 = (popobject *) objdest;
            popobj2 = (popobject *) objsrc;
            popobj1->obj = Duplicate(popobj2->obj, popobj1);
            result = objdest;
            break;
        case TY_MENU:
        case TY_SUBMENU:
        case TY_MENUITEM:
            objdest = AllocVec(sizeof(menu), MEMF_PUBLIC | MEMF_CLEAR);
            if (!objdest)
            {
                ErrorMessage(GetMUIBuilderString(MSG_NotEnoughMemory));
                return (NULL);
            }
            copyok = CopyObject(objsrc, objdest, sizeof(menu), father);
            if (!copyok)
                return (NULL);
            menu1 = (menu *) objsrc;
            menu2 = (menu *) objdest;
            menu2->childs = create();
            for (i = 0; i < menu1->childs->nb_elements; i++)
            {
                add(menu2->childs,
                    Duplicate(nth(menu1->childs, i), menu2));
            }
            result = objdest;
            break;
        default:
            ErrorMessage(GetMUIBuilderString(MSG_CopyNotImplemented));
    }
    return (result);
}
