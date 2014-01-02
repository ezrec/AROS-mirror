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

#include "notify.h"
#include <ctype.h>

#define EOL 10

char catfile[512];
struct Library *LocaleBase;
char locale_string[82];
char catprepend[5];

void InitLocale(void)
{
    LocaleBase = OpenLibrary("locale.library", 38L);

    OpenMUIBuilderCatalog(NULL);
}

void CloseLocale(void)
{
    if (LocaleBase)
    {
        CloseMUIBuilderCatalog();
        CloseLibrary(LocaleBase);
    }
}

CONST_STRPTR GetMenuString(int MSG_Id)
{
    return (&GetMUIBuilderString(MSG_Id)[2]);
}

CONST_STRPTR GetMenuChar(int MSG_Id)
{
    return (GetMUIBuilderString(MSG_Id));
}

void CreateLocaleString(char *chaine, char car)
{
    int i;
    char *aux;
    char c;
    BOOL search = TRUE;

    c = tolower(car);
    aux = locale_string;
    if (car == '\0')
    {
        strcpy(locale_string, chaine);
        return;
    }
    for (i = 0; chaine[i] != 0; i++)
    {
        if ((c == tolower(chaine[i])) && (search))
        {
            *aux++ = '_';
            *aux++ = chaine[i];
            search = FALSE;
        }
        else
            *aux++ = chaine[i];
    }
    *aux = '\0';
}

void NotifyCatalog(APTR obj, FILE *fichier)
{
    int i, id_dest;
    event *evt;
    object *obj_aux, *obj_aux2;

    obj_aux = obj;
    for (i = 0; i < obj_aux->notify->nb_elements; i++)
    {
        evt = nth(obj_aux->notify, i);
        obj_aux2 = evt->destination;
        id_dest = obj_aux2->id;
        if (TYActions[id_dest][evt->dest_type] == TY_CONS_STRING)
        {
            fprintf(fichier,
                    ";MUIB Notification string\n%s%sNotify%d(//)\n%s\n",
                    catprepend, obj_aux->label, i, evt->argstring);
        }
    }
}

void ObjectCatalog(APTR obj, FILE *fichier)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    bouton *button_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    check *check_aux;
    slider *slider_aux;
    chaine *string_aux;
    label *label_aux;
    texte *text_aux;
    prop *prop_aux;
    rectangle *rect_aux;
    colorfield *field_aux;
    dirlist *dir_aux;
    popaslobj *popasl_aux;
    listview *Listview_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    gauge *gauge_aux;

    int i;

    NotifyCatalog(obj, fichier);
    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_WINDOW:
            win_aux = obj;
            if (strlen(win_aux->title) > 0)
            {
                fprintf(fichier, ";MUIB window Title\n%s%s (//)\n%s\n",
                        catprepend, win_aux->label, win_aux->title);
            }
            ObjectCatalog(win_aux->menu, fichier);
            ObjectCatalog(&win_aux->root, fichier);
            break;
        case TY_GROUP:
            group_aux = obj;
            if (strlen(group_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier, ";MUIB group\n%s%sTitle (//)\n%s\n",
                        catprepend, group_aux->label,
                        group_aux->Area.TitleFrame);
            }
            if (group_aux->registermode)
            {
                if (group_aux->entries->nb_elements != 0)
                    for (i = 0; i < group_aux->entries->nb_elements; i++)
                    {
                        fprintf(fichier,
                                ";MUIB Register Entry\n%s%s%d (//)\n%s\n",
                                catprepend, group_aux->label, i,
                                (char *) nth(group_aux->entries, i));
                    }
                else
                    fprintf(fichier,
                            ";MUIB Register Entry\n%s%s0 (//)\nError\n",
                            catprepend, group_aux->label);
            }
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                ObjectCatalog(nth(group_aux->child, i), fichier);
            }
            break;
        case TY_KEYBUTTON:
            button_aux = obj;
            fprintf(fichier, ";MUIB button text\n");
            if ((button_aux->Area.Weight == 100) &&
                (!button_aux->Area.Hide) &&
                (!button_aux->Area.Disable) &&
                (!button_aux->Area.Phantom) &&
                (strchr(button_aux->title, tolower(button_aux->Area.key))
                 || strchr(button_aux->title,
                           toupper(button_aux->Area.key))))
            {
                CreateLocaleString(button_aux->title,
                                   button_aux->Area.key);
                fprintf(fichier, "%s%s (//)\n%s\n", catprepend,
                        button_aux->label, locale_string);
            }
            else
            {
                if (button_aux->Area.key != '\0')
                {
                    fprintf(fichier, "%s%s (//)\n%c\\00%s\n", catprepend,
                            button_aux->label, button_aux->Area.key,
                            button_aux->title);
                }
                else
                    fprintf(fichier, "%s%s (//)\n%s\n", catprepend,
                            button_aux->label, button_aux->title);
            }
            break;
        case TY_TEXT:
            text_aux = obj;
            if (strlen(text_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB text content\n%s%sTitle (//)\n%s\n",
                        catprepend, text_aux->label,
                        text_aux->Area.TitleFrame);
            }
            if ((strlen(text_aux->content) > 0)
                || (text_aux->Area.key != '\0'))
            {
                fprintf(fichier, ";MUIB text content\n%s%s (//)\n",
                        catprepend, text_aux->label);
                if (text_aux->Area.key != '\0')
                    fprintf(fichier, "%c\\00", text_aux->Area.key);
                if (strlen(text_aux->content) > 0)
                    fprintf(fichier, "%s", text_aux->content);
                fprintf(fichier, "\n");
            }
            break;
        case TY_GAUGE:
            gauge_aux = obj;
            if (strlen(gauge_aux->infotext) > 0)
            {
                fprintf(fichier, ";MUIB gauge infotext\n%s%s (//)\n",
                        catprepend, gauge_aux->label);
                fprintf(fichier, "%s\n", gauge_aux->infotext);
            }
            break;
        case TY_CYCLE:
            cycle_aux = obj;
            if (cycle_aux->Area.key != '\0')
                fprintf(fichier, ";MUIB Cycle\n%s%s (//)\n%c\\00\n",
                        catprepend, cycle_aux->label, cycle_aux->Area.key);
            for (i = 0; i < cycle_aux->entries->nb_elements; i++)
            {
                fprintf(fichier, ";MUIB Cycle Entry number %d\n", i);
                fprintf(fichier, "%s%s%d (//)\n%s\n", catprepend,
                        cycle_aux->label, i,
                        (char *) nth(cycle_aux->entries, i));
            }
            break;
        case TY_RADIO:
            radio_aux = obj;
            if (strlen(radio_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Radio TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, radio_aux->label,
                        radio_aux->Area.TitleFrame);
            }
            if (radio_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB Radio Title\n%s%s (//)\n",
                        catprepend, radio_aux->label);
                fprintf(fichier, "%c\\00\n", radio_aux->Area.key);
            }
            for (i = 0; i < radio_aux->entries->nb_elements; i++)
            {
                fprintf(fichier, ";MUIB Radio Entry number %d\n", i);
                fprintf(fichier, "%s%s%d (//)\n%s\n", catprepend,
                        radio_aux->label, i,
                        (char *) nth(radio_aux->entries, i));
            }
            break;
        case TY_CHECK:
            check_aux = obj;
            if ((check_aux->title_exist) || (check_aux->Area.key != '\0'))
            {
                fprintf(fichier, ";MUIB CheckMark\n");
                fprintf(fichier, "%s%s (//)\n", catprepend,
                        check_aux->label);
                if (check_aux->Area.key != '\0')
                    fprintf(fichier, "%c\\00", check_aux->Area.key);
                if (check_aux->title_exist)
                    fprintf(fichier, "%s", check_aux->title);
                fprintf(fichier, "\n");
            }
            break;
        case TY_SLIDER:
            slider_aux = obj;
            if (strlen(slider_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Slider TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, slider_aux->label,
                        slider_aux->Area.TitleFrame);
            }
            if ((slider_aux->title_exist)
                || (slider_aux->Area.key != '\0'))
            {
                fprintf(fichier, ";MUIB Slider\n");
                fprintf(fichier, "%s%s (//)\n", catprepend,
                        slider_aux->label);
                if (slider_aux->Area.key != '\0')
                    fprintf(fichier, "%c\\00", slider_aux->Area.key);
                if (slider_aux->title_exist)
                    fprintf(fichier, "%s", slider_aux->title);
                fprintf(fichier, "\n");
            }
            break;
        case TY_STRING:
            string_aux = obj;
            if (strlen(string_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Slider TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, string_aux->label,
                        string_aux->Area.TitleFrame);
            }
            if ((string_aux->title_exist)
                || (string_aux->Area.key != '\0'))
            {
                fprintf(fichier, ";MUIB String Gadget\n");
                fprintf(fichier, "%s%s (//)\n", catprepend,
                        string_aux->label);
                if (string_aux->Area.key != '\0')
                    fprintf(fichier, "%c\\00", string_aux->Area.key);
                if (string_aux->title_exist)
                    fprintf(fichier, "%s", string_aux->title);
                fprintf(fichier, "\n");
            }
            break;
        case TY_LABEL:
            label_aux = obj;
            if ((label_aux->Area.Weight == 0) ||
                (!label_aux->Area.Hide) ||
                (!label_aux->Area.Disable) || (!label_aux->Area.Phantom))
            {
                CreateLocaleString(label_aux->title, label_aux->Area.key);
                fprintf(fichier, ";MUIB Label\n%s%s (//)\n%s\n",
                        catprepend, label_aux->label, locale_string);
            }
            else
            {
                if (label_aux->Area.key != '\0')
                {
                    fprintf(fichier, ";MUIB Label\n%s%s (//)\n%c\\00%s\n",
                            catprepend, label_aux->label,
                            label_aux->Area.key, label_aux->title);
                }
                else
                    fprintf(fichier, ";MUIB Label\n%s%s (//)\n%s\n",
                            catprepend, label_aux->label,
                            label_aux->title);
            }
            break;
        case TY_PROP:
            prop_aux = obj;
            if (strlen(prop_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Prop TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, prop_aux->label,
                        prop_aux->Area.TitleFrame);
            }
            if (prop_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB Prop HotKey\n%s%s (//)\n",
                        catprepend, prop_aux->label);
                fprintf(fichier, "%c\\00\n", prop_aux->Area.key);
            }
            break;
        case TY_RECTANGLE:
            rect_aux = obj;
            if (strlen(rect_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Rectangle TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, rect_aux->label,
                        rect_aux->Area.TitleFrame);
            }
            if (rect_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB Rectangle HotKey\n%s%s (//)\n",
                        catprepend, rect_aux->label);
                fprintf(fichier, "%c\\00\n", rect_aux->Area.key);
            }
            break;
        case TY_COLORFIELD:
            field_aux = obj;
            if (strlen(field_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB ColorField TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, field_aux->label,
                        field_aux->Area.TitleFrame);
            }
            if (field_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB ColorField HotKey\n%s%s (//)\n",
                        catprepend, field_aux->label);
                fprintf(fichier, "%c\\00\n", field_aux->Area.key);
            }
            break;
        case TY_DIRLIST:
            dir_aux = obj;
            if (strlen(dir_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB DirList TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, dir_aux->label,
                        dir_aux->Area.TitleFrame);
            }
            if (dir_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB DirList Hotkey\n%s%s (//)\n",
                        catprepend, dir_aux->label);
                fprintf(fichier, "%c\\00\n", dir_aux->Area.key);
            }
            break;
        case TY_LISTVIEW:
            Listview_aux = obj;
            if (strlen(Listview_aux->Area.TitleFrame) > 0)
            {
                fprintf(fichier,
                        ";MUIB Listview TitleFrame\n%s%sTitle (//)\n%s\n",
                        catprepend, Listview_aux->label,
                        Listview_aux->Area.TitleFrame);
            }
            if ((strlen(Listview_aux->title) > 0)
                || (Listview_aux->Area.key != '\0'))
            {
                fprintf(fichier, ";MUIB Listview Gadget\n");
                fprintf(fichier, "%s%s (//)\n", catprepend,
                        Listview_aux->label);
                if (Listview_aux->Area.key != '\0')
                    fprintf(fichier, "%c\\00", Listview_aux->Area.key);
                if (strlen(Listview_aux->title) > 0)
                    fprintf(fichier, "%s", Listview_aux->title);
                fprintf(fichier, "\n");
            }
            if ((Listview_aux->type == 1)
                && (strlen(Listview_aux->content) > 0))
            {
                fprintf(fichier,
                        ";MUIB Listview Gadget Initial Content\n");
                fprintf(fichier, "%s%sContent (//)\n%s\n", catprepend,
                        Listview_aux->label, Listview_aux->content);
            }
            break;
        case TY_POPASL:
            popasl_aux = obj;
            if (popasl_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB PopAsl HotKey\n%s%s (//)\n",
                        catprepend, popasl_aux->label);
                fprintf(fichier, "%c\\00\n", popasl_aux->Area.key);
            }
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            if (popobj_aux->Area.key != '\0')
            {
                fprintf(fichier, ";MUIB Pobject HotKey\n%s%s (//)\n",
                        catprepend, popobj_aux->label);
                fprintf(fichier, "%c\\00\n", popobj_aux->Area.key);
            }
            ObjectCatalog(popobj_aux->obj, fichier);
            break;
        case TY_MENU:
            menu_aux = obj;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                ObjectCatalog(nth(menu_aux->childs, i), fichier);
            }
            break;
        case TY_SUBMENU:
            menu_aux = obj;
            if (strlen(menu_aux->label) > 0)
            {
                fprintf(fichier, ";MUIB Menu Title\n%s%s (//)\n",
                        catprepend, menu_aux->label);
                fprintf(fichier, "%s\n", menu_aux->name);
            }
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                ObjectCatalog(nth(menu_aux->childs, i), fichier);
            }
            break;
        case TY_MENUITEM:
            menu_aux = obj;
            if (strncmp(menu_aux->name, "BarLabel", 8) == 0)
                break;
            if (strlen(menu_aux->name) > 0)
            {
                fprintf(fichier, ";MUIB Menu Title\n%s%s (//)\n",
                        catprepend, menu_aux->label);
                fprintf(fichier, "%s\n", menu_aux->name);
            }
            if (menu_aux->key)
            {
                fprintf(fichier, ";MUIB Menu ShortCut\n%s%sChar (//)\n",
                        catprepend, menu_aux->label);
                fprintf(fichier, "%c_\n", menu_aux->key);
            }
            break;
    }
}

void ReadLine(FILE *tmp)
{
    int aux;

    do
    {
        aux = fgetc(tmp);
    }
    while ((aux != EOL) && (aux != EOF));
}

void CopyLines(FILE *tmp, FILE *fichier, int nb)
{
    int i;
    int aux = 0;

    if (!feof(tmp))
        for (i = 0; (i < nb) && (aux != EOF); i++)
        {
            do
            {
                aux = fgetc(tmp);
                fputc(aux, fichier);
            }
            while ((aux != EOL) && (aux != EOF));
        }
}

void CopyStrings(FILE *tmp, FILE *fichier)
{
    char string[6];

    while (!feof(tmp))
    {
        fgets(string, 6, tmp);
        if (!feof(tmp))
        {
            if (strncmp(string, ";MUIB", 5) == 0)
            {
                if (strlen(string) >= 5)
                    ReadLine(tmp);
                ReadLine(tmp);
                ReadLine(tmp);
            }
            else
            {
                fprintf(fichier, "%s", string);
                if (strlen(string) >= 5)
                    CopyLines(tmp, fichier, 1);
            }
        }
    }
}

void CreateCatalog(void)
{
    FILE *fichier, *tmp = NULL;
    int i;
    BPTR lock;
    char command[529];

    add_extend(catfile, ".cd");
    if (lock = Lock(catfile, ACCESS_READ))
    {
        UnLock(lock);
        sprintf(command, "copy \"%s\" T:MB.tmp", catfile);
        Execute(command, BNULL, BNULL);
        DeleteFile(catfile);
        tmp = fopen("T:MB.tmp", "r");
    }
    if (fichier = fopen(catfile, "w+"))
    {
        fprintf(fichier,
                ";MUIB\n;MUIB\n;MUIB This catalog description file was generated by MUIBuilder.\n");
        fprintf(fichier, ";MUIB\n");
        fprintf(fichier,
                ";MUIB Please don't modify the lines generated by MUIBuilder,\n;MUIB but you can add your own entries.\n");
        fprintf(fichier, ";MUIB\n%sAppDescription (//)\n", catprepend);
        fprintf(fichier, "%s\n;MUIB\n", application.description);
        fprintf(fichier, "%sAppCopyright (//)\n%s\n", catprepend,
                application.copyright);
        if (application.appmenu->childs->nb_elements > 0)
            ObjectCatalog(application.appmenu, fichier);
        for (i = 0; i < windows->nb_elements; i++)
        {
            ObjectCatalog(nth(windows, i), fichier);
        }
        if (tmp)
        {
            CopyStrings(tmp, fichier);
            fclose(tmp);
        }
    }
    fclose(fichier);
    DeleteFile("T:MB.tmp");
}
