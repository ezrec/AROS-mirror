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

#include <dos/dostags.h>
#include "builder.h"

char savefile[256];
char savedir[256];

#define SAVE_VERSION "BUILDER_SAVE_FILE1.26\n"

void SaveNotify(FILE *fichier, object *obj)
{
    int i;
    event *evt;

    for (i = 0; i < obj->notify->nb_elements; i++)
    {
        evt = nth(obj->notify, i);
        fprintf(fichier, "%s\n", evt->destination->label);
        fprintf(fichier, "%d\n", evt->id_cible);
        fprintf(fichier, "%d\n", evt->src_type);
        fprintf(fichier, "%d\n", evt->dest_type);
        fprintf(fichier, "%s\n", evt->argstring);
    }
    fprintf(fichier, "//END_ENTRIES//\n");
}

void SaveArea(FILE *fichier, area *Area)
{
    fprintf(fichier, "%d\n", Area->Hide);
    fprintf(fichier, "%d\n", Area->Disable);
    fprintf(fichier, "%d\n", Area->InputMode);
    fprintf(fichier, "%d\n", Area->Phantom);
    fprintf(fichier, "%d\n", Area->Weight);
    fprintf(fichier, "%d\n", Area->Background);
    fprintf(fichier, "%d\n", Area->Frame);
    fprintf(fichier, "%c\n", Area->key);
    fprintf(fichier, "%s\n", Area->TitleFrame);
}

void SaveApplicationList(FILE *fichier, queue *list)
{
    int i;

    for (i = 0; i < list->nb_elements; i++)
    {
        fprintf(fichier, "%s\n", (char *) nth(list, i));
    }
    fprintf(fichier, "//END_ENTRIES//\n");
}

void SaveApplication(BOOL SaveAs)
{
    int i;
    FILE *fichier;
    char file[256];
    CONST_STRPTR title;
    struct DiskObject *icon, *icon2;

    title = GetMUIBuilderString(MSG_FileName);

    if (SaveAs || (strlen(savefile) == 0))
    {
        if (SaveRequester(title, savedir, savefile))
        {
            if (strlen(savefile) > 0)
                add_extend(savefile, ".MUIB");
            else
                return;
        }
        else
            return;
    }

    strncpy(file, savedir, 255);
    AddPart(file, savefile, 255);
    if (fichier = fopen(file, "w+"))
    {
        fprintf(fichier, SAVE_VERSION);
        fprintf(fichier, "%s\n", genfile);
        fprintf(fichier, "%s\n", catfile);
        fprintf(fichier, "%d\n", nb_window);
        fprintf(fichier, "%d\n", nb_listview);
        fprintf(fichier, "%d\n", nb_group);
        fprintf(fichier, "%d\n", nb_button);
        fprintf(fichier, "%d\n", nb_string);
        fprintf(fichier, "%d\n", nb_gauge);
        fprintf(fichier, "%d\n", nb_cycle);
        fprintf(fichier, "%d\n", nb_radio);
        fprintf(fichier, "%d\n", nb_check);
        fprintf(fichier, "%d\n", nb_image);
        fprintf(fichier, "%d\n", nb_slider);
        fprintf(fichier, "%d\n", nb_text);
        fprintf(fichier, "%d\n", nb_prop);
        fprintf(fichier, "%d\n", nb_rectangle);
        fprintf(fichier, "%d\n", nb_colorfield);
        fprintf(fichier, "%d\n", nb_popasl);
        fprintf(fichier, "%d\n", nb_popobject);
        fprintf(fichier, "%d\n", nb_menu);
        fprintf(fichier, "%d\n", nb_space);
        fprintf(fichier, "%d\n", nb_scale);
        fprintf(fichier, "%d\n", nb_barlabel);
        fprintf(fichier, "%d\n", nb_label);
        fprintf(fichier, "%d\n", code);
        fprintf(fichier, "%d\n", env);
        fprintf(fichier, "%d\n", declarations);
        fprintf(fichier, "%d\n", local);
        fprintf(fichier, "%d\n", notifications);
        fprintf(fichier, "%d\n", generate_all);
        fprintf(fichier, "%s\n", application.base);
        fprintf(fichier, "%s\n", application.author);
        fprintf(fichier, "%s\n", application.title);
        fprintf(fichier, "%s\n", application.version);
        fprintf(fichier, "%s\n", application.copyright);
        fprintf(fichier, "%s\n", application.description);
        fprintf(fichier, "%s\n", application.helpfile);
        fprintf(fichier, "%s\n", real_getstring);
        fprintf(fichier, "%s\n", catprepend);
        fprintf(fichier, "%d\n", application.Help.nb_char);
        fprintf(fichier, "%d\n", application.Help.generated);
        if (application.Help.nb_char > 0)
        {
            title = application.Help.content;
            for (i = 0; i < application.Help.nb_char; i++)
            {
                fprintf(fichier, "%c", *title);
                title++;
            }
        }
        SaveApplicationList(fichier, application.Idents);
        SaveApplicationList(fichier, application.Functions);
        SaveApplicationList(fichier, application.Variables);
        SaveNotify(fichier, (object *) &application);
        SaveObject(application.appmenu, fichier);
        for (i = 0; i < windows->nb_elements; i++)
        {
            SaveObject(nth(windows, i), fichier);
        }
        fprintf(fichier, "%d\n", -1);
        fclose(fichier);
        if (config.icons)
        {
            icon = GetDiskObject("ENV:sys/def_MUIBuilder");
            if (icon)
            {
                icon->do_CurrentX = NO_ICON_POSITION;
                icon->do_CurrentY = NO_ICON_POSITION;
                icon2 = GetDiskObject(file);
                if (!icon2)
                {
                    PutDiskObject(file, icon);
                }
                else
                    FreeDiskObject(icon2);
                FreeDiskObject(icon);
            }
        }
    }
}

void SaveObject(APTR obj, FILE *fichier)
{
    object *obj_aux;
    bouton *button_aux;
    gauge *gauge_aux;
    chaine *string_aux;
    label *label_aux;
    check *check_aux;
    scale *scale_aux;
    slider *slider_aux;
    prop *prop_aux;
    rectangle *rect_aux;
    colorfield *field_aux;
    popobject *popobj_aux;
    space *space_aux;
    int i;
    char *aux;

    obj_aux = obj;
    fprintf(fichier, "%d\n", obj_aux->id);
    fprintf(fichier, "%s\n", obj_aux->label);
    fprintf(fichier, "%d\n", obj_aux->generated);
    fprintf(fichier, "%s\n", obj_aux->Help.title);
    fprintf(fichier, "%d\n", obj_aux->Help.nb_char);
    fprintf(fichier, "%d\n", obj_aux->Help.generated);
    if (obj_aux->Help.nb_char > 0)
    {
        aux = obj_aux->Help.content;
        for (i = 0; i < obj_aux->Help.nb_char; i++)
        {
            fprintf(fichier, "%c", *aux);
            aux++;
        }
    }
    SaveNotify(fichier, obj_aux);
    switch (obj_aux->id)
    {
        case TY_WINDOW:
            SaveWindow(fichier, obj);
            break;
        case TY_GROUP:
            SaveGroup(fichier, obj);
            break;
        case TY_KEYBUTTON:
            button_aux = obj;
            fprintf(fichier, "%s\n", button_aux->title);
            SaveArea(fichier, &button_aux->Area);
            break;
        case TY_TEXT:
            SaveText(fichier, obj);
            break;
        case TY_SCALE:
            scale_aux = obj;
            SaveArea(fichier, &scale_aux->Area);
            fprintf(fichier, "%d\n", scale_aux->horiz);
            break;
        case TY_SLIDER:
            slider_aux = obj;
            SaveArea(fichier, &slider_aux->Area);
            fprintf(fichier, "%s\n", slider_aux->title);
            fprintf(fichier, "%d\n", slider_aux->title_exist);
            fprintf(fichier, "%d\n", slider_aux->max);
            fprintf(fichier, "%d\n", slider_aux->min);
            fprintf(fichier, "%d\n", slider_aux->quiet);
            fprintf(fichier, "%d\n", slider_aux->init);
            fprintf(fichier, "%d\n", slider_aux->reverse);
            fprintf(fichier, "%d\n", slider_aux->horizontal);
            break;
        case TY_GAUGE:
            gauge_aux = obj;
            SaveArea(fichier, &gauge_aux->Area);
            fprintf(fichier, "%d\n", gauge_aux->divide);
            fprintf(fichier, "%d\n", gauge_aux->horizontal);
            fprintf(fichier, "%d\n", gauge_aux->max);
            fprintf(fichier, "%d\n", gauge_aux->fixheight);
            fprintf(fichier, "%d\n", gauge_aux->fixwidth);
            fprintf(fichier, "%d\n", gauge_aux->height);
            fprintf(fichier, "%d\n", gauge_aux->width);
            fprintf(fichier, "%s\n", gauge_aux->infotext);
            break;
        case TY_PROP:
            prop_aux = obj;
            SaveArea(fichier, &prop_aux->Area);
            fprintf(fichier, "%d\n", prop_aux->horizontal);
            fprintf(fichier, "%d\n", prop_aux->entries);
            fprintf(fichier, "%d\n", prop_aux->first);
            fprintf(fichier, "%d\n", prop_aux->visible);
            fprintf(fichier, "%d\n", prop_aux->fixheight);
            fprintf(fichier, "%d\n", prop_aux->fixwidth);
            fprintf(fichier, "%d\n", prop_aux->height);
            fprintf(fichier, "%d\n", prop_aux->width);
            break;
        case TY_LISTVIEW:
            SaveListview(fichier, obj);
            break;
        case TY_DIRLIST:
            SaveDirList(fichier, obj);
            break;
        case TY_CYCLE:
            SaveCycle(fichier, obj);
            break;
        case TY_RADIO:
            SaveRadio(fichier, obj);
            break;
        case TY_STRING:
            string_aux = obj;
            SaveArea(fichier, &string_aux->Area);
            fprintf(fichier, "%s\n", string_aux->title);
            fprintf(fichier, "%s\n", string_aux->content);
            fprintf(fichier, "%d\n", string_aux->title_exist);
            fprintf(fichier, "%s\n", string_aux->accept);
            fprintf(fichier, "%s\n", string_aux->reject);
            fprintf(fichier, "%d\n", string_aux->format);
            fprintf(fichier, "%d\n", string_aux->integer);
            fprintf(fichier, "%d\n", string_aux->maxlen);
            fprintf(fichier, "%d\n", string_aux->secret);
            break;
        case TY_CHECK:
            check_aux = obj;
            SaveArea(fichier, &check_aux->Area);
            fprintf(fichier, "%s\n", check_aux->title);
            fprintf(fichier, "%d\n", check_aux->title_exist);
            fprintf(fichier, "%d\n", check_aux->init_state);
            break;
        case TY_LABEL:
            label_aux = obj;
            SaveArea(fichier, &label_aux->Area);
            fprintf(fichier, "%s\n", label_aux->title);
            break;
        case TY_IMAGE:
            SaveImage(fichier, obj);
            break;
        case TY_RECTANGLE:
            rect_aux = obj;
            SaveArea(fichier, &rect_aux->Area);
            fprintf(fichier, "%d\n", rect_aux->type);
            fprintf(fichier, "%d\n", rect_aux->fixheight);
            fprintf(fichier, "%d\n", rect_aux->fixwidth);
            fprintf(fichier, "%d\n", rect_aux->height);
            fprintf(fichier, "%d\n", rect_aux->width);
            break;
        case TY_COLORFIELD:
            field_aux = obj;
            SaveArea(fichier, &field_aux->Area);
            fprintf(fichier, "%d\n", field_aux->fixheight);
            fprintf(fichier, "%d\n", field_aux->fixwidth);
            fprintf(fichier, "%d\n", field_aux->height);
            fprintf(fichier, "%d\n", field_aux->width);
            fprintf(fichier, "%u\n", (unsigned int)field_aux->red);
            fprintf(fichier, "%u\n", (unsigned int)field_aux->green);
            fprintf(fichier, "%u\n", (unsigned int)field_aux->blue);
            break;
        case TY_POPASL:
            SavePopAsl(fichier, obj);
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            SaveArea(fichier, &popobj_aux->Area);
            fprintf(fichier, "%s\n", popobj_aux->openhook);
            fprintf(fichier, "%s\n", popobj_aux->closehook);
            SaveObject(popobj_aux->obj, fichier);
            fprintf(fichier, "%d\n", popobj_aux->image);
            fprintf(fichier, "%d\n", popobj_aux->follow);
            fprintf(fichier, "%d\n", popobj_aux->light);
            fprintf(fichier, "%d\n", popobj_aux->Volatile);
            break;
        case TY_MENU:
        case TY_SUBMENU:
        case TY_MENUITEM:
            SaveMenu(fichier, obj);
            break;
        case TY_SPACE:
            space_aux = obj;
            fprintf(fichier, "%d\n", space_aux->type);
            fprintf(fichier, "%d\n", space_aux->spacing);
            break;
    }
}
