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

window *current_window;
char aux[256];
int version = 0;

int SearchVersion(char *text)
{
    if (strcmp(text, "BUILDER_SAVE_FILE") == 0)
        return (100);
    if (strcmp(text, "BUILDER_SAVE_FILE1.01") == 0)
        return (101);
    if (strcmp(text, "BUILDER_SAVE_FILE1.02") == 0)
        return (102);
    if (strcmp(text, "BUILDER_SAVE_FILE1.03") == 0)
        return (103);
    if (strcmp(text, "BUILDER_SAVE_FILE1.04") == 0)
        return (104);
    if (strcmp(text, "BUILDER_SAVE_FILE1.10") == 0)
        return (110);
    if (strcmp(text, "BUILDER_SAVE_FILE1.11") == 0)
        return (111);
    if (strcmp(text, "BUILDER_SAVE_FILE1.12") == 0)
        return (112);
    if (strcmp(text, "BUILDER_SAVE_FILE1.13") == 0)
        return (113);
    if (strcmp(text, "BUILDER_SAVE_FILE1.14") == 0)
        return (114);
    if (strcmp(text, "BUILDER_SAVE_FILE1.15") == 0)
        return (115);
    if (strcmp(text, "BUILDER_SAVE_FILE1.16") == 0)
        return (116);
    if (strcmp(text, "BUILDER_SAVE_FILE1.17") == 0)
        return (117);
    if (strcmp(text, "BUILDER_SAVE_FILE1.18") == 0)
        return (118);
    if (strcmp(text, "BUILDER_SAVE_FILE1.19") == 0)
        return (119);
    if (strcmp(text, "BUILDER_SAVE_FILE1.20") == 0)
        return (120);
    if (strcmp(text, "BUILDER_SAVE_FILE1.21") == 0)
        return (121);
    if (strcmp(text, "BUILDER_SAVE_FILE1.22") == 0)
        return (122);
    if (strcmp(text, "BUILDER_SAVE_FILE1.23") == 0)
        return (123);
    if (strcmp(text, "BUILDER_SAVE_FILE1.24") == 0)
        return (124);
    if (strcmp(text, "BUILDER_SAVE_FILE1.25") == 0)
        return (125);
    if (strcmp(text, "BUILDER_SAVE_FILE1.26") == 0)
        return (126);
    return (0);
}

void ReadHelp(FILE *fichier, APTR obj)
{
    int i;
    object *obj_aux;
    char *car;

    obj_aux = obj;
    strcpy(obj_aux->Help.title, LitChaine(fichier));
    obj_aux->Help.nb_char = ReadInt(fichier);
    if (version > 115)
        obj_aux->Help.generated = ReadInt(fichier);
    else
        obj_aux->Help.generated = (obj_aux->generated
                                   || (obj_aux->Help.nb_char > 0));
    if (obj_aux->Help.nb_char > 0)
    {
        car =
            AllocVec(obj_aux->Help.nb_char + 1, MEMF_PUBLIC | MEMF_CLEAR);
        obj_aux->Help.content = car;
        for (i = 0; i < obj_aux->Help.nb_char; i++)
        {
            *car = fgetc(fichier);
            car++;
        }
        *car = '\0';
    }
}

void ReadNotify(FILE *fichier, APTR obj_aux)
{
    object *obj;
    char *chaine_aux;
    event *evt;

    obj = obj_aux;
    while (strcmp(chaine_aux = LitChaine(fichier), "//END_ENTRIES//") != 0)
    {
        evt = AllocVec(sizeof(event), MEMF_PUBLIC | MEMF_CLEAR);
        strcpy(evt->label_cible, chaine_aux);
        evt->id_cible = ReadInt(fichier);
        evt->src_type = ReadInt(fichier);
        evt->dest_type = ReadInt(fichier);
        strcpy(evt->argstring, LitChaine(fichier));
        add(obj->notify, evt);
    }
}

void ReadArea(FILE *fichier, area *Area)
{
    char chaine_aux[3];

    Area->Hide = ReadInt(fichier);
    Area->Disable = ReadInt(fichier);
    Area->InputMode = ReadInt(fichier);
    Area->Phantom = ReadInt(fichier);
    Area->Weight = ReadInt(fichier);
    Area->Background = ReadInt(fichier);
    Area->Frame = ReadInt(fichier);
    strcpy(chaine_aux, LitChaine(fichier));
    Area->key = chaine_aux[0];
    strcpy(Area->TitleFrame, LitChaine(fichier));
}

void ReadApplicationList(FILE *fichier, queue *list)
{
    char *chaine_aux;
    char *chaine_aux2;

    while (strcmp(chaine_aux = LitChaine(fichier), "//END_ENTRIES//") != 0)
    {
        chaine_aux2 = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
        strncpy(chaine_aux2, chaine_aux, 80);
        add(list, chaine_aux2);
    }
}

void ReadFunctionHook(FILE *fichier, char **name)
{
    char *chaine_aux;

    /* if (chaine_aux = AllocVec(80, MEMF_PUBLIC|MEMF_CLEAR))
       {
       sprintf(chaine_aux, "%s", LitChaine(fichier));
       if (strlen(chaine_aux)>0) AddInList(chaine_aux, application.Functions);
       else
       {
       FreeVec(chaine_aux);
       chaine_aux = NULL;
       }
       } */
    if (chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR))
    {
        sprintf(chaine_aux, "%s", (char *) LitChaine(fichier));
        *name = PointerOnString(chaine_aux, application.Functions);
        FreeVec(chaine_aux);
    }
}

APTR LitChaine(FILE *fichier)
{
    char *aux2;

    fgets(aux, 256, fichier);
    aux2 = strchr(aux, '\n');
    if (aux2)
    {
        *aux2 = '\0';
    }
    else
    {
        aux[0] = '\0';
    }
    return (aux);
}

void NextLine(FILE *fichier)
{
    char chaine_aux[80];

    fgets(chaine_aux, 80, fichier);
}

int ReadInt(FILE *fichier)
{
    int i;

    if (fscanf(fichier, "%d", &i) == EOF)
        i = -1;
    NextLine(fichier);
    return (i);
}

void LoadApplication()
{
    char file[256];
    CONST_STRPTR title;

    title = GetMUIBuilderString(MSG_FileName);

    if (LoadRequester(title, savedir, savefile))
    {
        strncpy(file, savedir, 255);
        AddPart(file, savefile, 255);
        LoadFile(file);
    }
}

void LoadFile(char *file)
{
    FILE *fichier;
    int i;
    int n;
    char *title;

    if (strlen(file) > 0)
    {
        if (fichier = fopen(file, "r"))
        {
            if ((version = SearchVersion(LitChaine(fichier))) != 0)
            {
                DeleteApplication();
                InitAppli();
                if (application.Help.nb_char > 0)
                    FreeVec(application.Help.content);
                if (version > 102)
                {
                    strcpy(genfile, LitChaine(fichier));
                    strcpy(catfile, LitChaine(fichier));
                }
                else
                {
                    genfile[0] = '\0';
                    catfile[0] = '\0';
                }
                nb_window = ReadInt(fichier);
                nb_listview = ReadInt(fichier);
                nb_group = ReadInt(fichier);
                nb_button = ReadInt(fichier);
                nb_string = ReadInt(fichier);
                nb_gauge = ReadInt(fichier);
                nb_cycle = ReadInt(fichier);
                nb_radio = ReadInt(fichier);
                nb_check = ReadInt(fichier);
                nb_image = ReadInt(fichier);
                nb_slider = ReadInt(fichier);
                nb_text = ReadInt(fichier);
                nb_prop = ReadInt(fichier);
                if (version > 110)
                {
                    nb_rectangle = ReadInt(fichier);;
                    nb_colorfield = ReadInt(fichier);
                    nb_popasl = ReadInt(fichier);
                    nb_popobject = ReadInt(fichier);
                }
                else
                {
                    nb_rectangle = 0;
                    nb_colorfield = 0;
                    nb_popasl = 0;
                    nb_popobject = 0;
                }
                if (version > 114)
                {
                    nb_menu = ReadInt(fichier);
                    nb_space = ReadInt(fichier);
                    nb_scale = ReadInt(fichier);
                }
                else
                {
                    nb_menu = 0;
                    nb_space = 0;
                    nb_scale = 0;
                }
                if (version > 124)
                    nb_barlabel = ReadInt(fichier);
                if (version > 120)
                    nb_label = ReadInt(fichier);
                else
                    nb_label = 0;
                if (version > 121)
                {
                    code = ReadInt(fichier);
                    env = ReadInt(fichier);
                    declarations = ReadInt(fichier);
                    local = ReadInt(fichier);
                    notifications = ReadInt(fichier);
                }
                if (version > 122)
                {
                    generate_all = ReadInt(fichier);
                }
                InitAppli();
                strcpy(application.base, LitChaine(fichier));
                strcpy(application.author, LitChaine(fichier));
                strcpy(application.title, LitChaine(fichier));
                strcpy(application.version, LitChaine(fichier));
                strcpy(application.copyright, LitChaine(fichier));
                strcpy(application.description, LitChaine(fichier));
                strcpy(application.helpfile, LitChaine(fichier));
                if (version > 103)
                {
                    strcpy(real_getstring, LitChaine(fichier));
                }
                else
                    real_getstring[0] = '\0';
                if (version > 115)
                    strcpy(catprepend, LitChaine(fichier));
                else
                    strcpy(catprepend, "MSG_");
                i = ReadInt(fichier);
                application.Help.nb_char = i;
                if (version > 115)
                    application.Help.generated = ReadInt(fichier);
                if (i > 0)
                {
                    title = AllocVec(i + 1, MEMF_PUBLIC | MEMF_CLEAR);
                    application.Help.content = title;
                    for (i = 0; i < application.Help.nb_char; i++)
                    {
                        *title = fgetc(fichier);
                        title++;
                    }
                    *title = '\0';
                }
                if (version > 111)
                {
                    ReadApplicationList(fichier, application.Idents);
                    ReadApplicationList(fichier, application.Functions);
                    ReadApplicationList(fichier, application.Variables);
                }
                if (version > 104)
                    ReadNotify(fichier, &application);
                if (version >= 117)
                {
                    i = ReadInt(fichier);
                    application.appmenu =
                        LoadObject(fichier, &application, i);
                }
                n = windows->nb_elements;
                for (i = 0; i < n; i++)
                {
                    DeleteObject(nth(windows, 0));
                    delete_nth(windows, 0);
                }

                do
                {
                    i = ReadInt(fichier);
                    if (i != -1)
                    {
                        add(windows, LoadObject(fichier, NULL, i));
                    }
                } while (i != -1);

                delete_all(labels);
                CreateLabels(&application, NULL);
                LinkNotify(0);
            }
            else
                ErrorMessage(GetMUIBuilderString(MSG_NotBuilderSaveFile));
            fclose(fichier);
        }
    }
}

void MergeFile(void)
{
    FILE *fichier;
    static char file[512];
    static char dir_aux[256];
    static char file_aux[256];
    CONST_STRPTR title;
    int i, n;
    window *win_aux;
    object *obj_aux;

    title = GetMUIBuilderString(MSG_MergeFile);

    strncpy(dir_aux, savedir, 255);
    strncpy(file_aux, file_aux, 255);
    if (LoadRequester(title, dir_aux, file_aux))
    {
        strncpy(file, dir_aux, 255);
        AddPart(file, file_aux, 255);
        if (strlen(file) > 0)
        {
            if (fichier = fopen(file, "r"))
            {
                if ((version = SearchVersion(LitChaine(fichier))) != 0)
                {
                    if (application.Help.nb_char > 0)
                        FreeVec(application.Help.content);
                    if (version > 102)
                    {
                        LitChaine(fichier);
                        LitChaine(fichier);
                    }
                    nb_window += ReadInt(fichier);
                    nb_listview += ReadInt(fichier);
                    nb_group += ReadInt(fichier);
                    nb_button += ReadInt(fichier);
                    nb_string += ReadInt(fichier);
                    nb_gauge += ReadInt(fichier);
                    nb_cycle += ReadInt(fichier);
                    nb_radio += ReadInt(fichier);
                    nb_check += ReadInt(fichier);
                    nb_image += ReadInt(fichier);
                    nb_slider += ReadInt(fichier);
                    nb_text += ReadInt(fichier);
                    nb_prop += ReadInt(fichier);
                    if (version > 110)
                    {
                        nb_rectangle += ReadInt(fichier);
                        nb_colorfield += ReadInt(fichier);
                        nb_popasl += ReadInt(fichier);
                        nb_popobject += ReadInt(fichier);
                    }
                    if (version > 114)
                    {
                        nb_menu += ReadInt(fichier);
                        nb_space += ReadInt(fichier);
                        nb_scale += ReadInt(fichier);
                    }
                    if (version > 124)
                        nb_barlabel = nb_barlabel + ReadInt(fichier);
                    if (version > 120)
                        nb_label = nb_label + ReadInt(fichier);
                    if (version > 121)
                    {
                        code = ReadInt(fichier);
                        env = ReadInt(fichier);
                        declarations = ReadInt(fichier);
                        local = ReadInt(fichier);
                        notifications = ReadInt(fichier);
                    }
                    if (version > 122)
                    {
                        generate_all = ReadInt(fichier);
                    }
                    /* toutes les chaines de l'appli */
                    LitChaine(fichier);
                    LitChaine(fichier);
                    LitChaine(fichier);
                    LitChaine(fichier);
                    LitChaine(fichier);
                    LitChaine(fichier);
                    LitChaine(fichier);
                    if (version > 103)
                    {
                        LitChaine(fichier);
                    }
                    if (version > 115)
                        LitChaine(fichier);
                    i = ReadInt(fichier);
                    application.Help.nb_char = i;
                    if (version > 115)
                        ReadInt(fichier);
                    if (i > 0)
                    {
                        for (i = 0; i < application.Help.nb_char; i++)
                        {
                            fgetc(fichier);
                        }
                    }
                    if (version > 111)
                    {
                        ReadApplicationList(fichier, application.Idents);
                        ReadApplicationList(fichier,
                                            application.Functions);
                        ReadApplicationList(fichier,
                                            application.Variables);
                    }
                    if (version > 104)
                        ReadNotify(fichier, &application);
                    if (version >= 117)
                    {
                        i = ReadInt(fichier);
                        obj_aux = LoadObject(fichier, &application, i);
                        DeleteObject(obj_aux);
                    }
                    n = windows->nb_elements;

                    do
                    {
                        fscanf(fichier, "%d", &i);
                        NextLine(fichier);
                        if (i != -1)
                        {
                            add(windows, LoadObject(fichier, NULL, i));
                        }
                    } while (i != -1);

                    delete_all(labels);
                    CreateLabels(&application, NULL);
                    win_aux = nth(windows, n);
                    for (i = 0; i < labels->nb_elements; i++)
                    {
                        if (nth(labels, i) == win_aux)
                        {
                            LinkNotify(i);
                            break;
                        }
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_NotBuilderSaveFile));
                fclose(fichier);
            }
        }
    }
}

APTR LoadObject(FILE *fichier, APTR father, int obj)
{
    APTR result = NULL;
    bouton *button_aux;
    gauge *gauge_aux;
    chaine *string_aux;
    label *label_aux;
    space *space_aux;
    check *check_aux;
    scale *scale_aux;
    slider *slider_aux;
    prop *prop_aux;
    rectangle *rect_aux;
    colorfield *field_aux;
    popobject *popobj_aux;
    static char chaine2_aux[80];
    int i;

    APTR PointerOnString(char *chaine, queue *list);

    switch (obj)
    {
        case TY_WINDOW:
            result = LoadWindow(fichier, version);
            break;
        case TY_GROUP:
            result = LoadGroup(fichier, father, version);
            break;
        case TY_KEYBUTTON:
            button_aux =
                AllocVec(sizeof(bouton), MEMF_PUBLIC | MEMF_CLEAR);
            InitButton(button_aux);
            button_aux->id = obj;
            button_aux->father = father;
            strcpy(button_aux->label, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            button_aux->generated = i;
            NextLine(fichier);
            ReadHelp(fichier, button_aux);
            if (version > 104)
                ReadNotify(fichier, button_aux);
            strcpy(button_aux->title, LitChaine(fichier));
            if (version < 113)
            {
                strcpy(chaine2_aux, LitChaine(fichier));
                button_aux->Area.key = *chaine2_aux;
                fscanf(fichier, "%d", &i);
                button_aux->Area.Weight = i;
                NextLine(fichier);
            }
            else
                ReadArea(fichier, &button_aux->Area);
            result = button_aux;
            break;
        case TY_GAUGE:
            gauge_aux = AllocVec(sizeof(gauge), MEMF_PUBLIC | MEMF_CLEAR);
            InitGauge(gauge_aux);
            gauge_aux->id = obj;
            gauge_aux->father = father;
            strcpy(gauge_aux->label, LitChaine(fichier));
            gauge_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, gauge_aux);
            if (version > 104)
                ReadNotify(fichier, gauge_aux);
            if (version >= 113)
                ReadArea(fichier, &gauge_aux->Area);
            gauge_aux->divide = ReadInt(fichier);
            gauge_aux->horizontal = ReadInt(fichier);
            gauge_aux->max = ReadInt(fichier);
            gauge_aux->fixheight = ReadInt(fichier);
            gauge_aux->fixwidth = ReadInt(fichier);
            gauge_aux->height = ReadInt(fichier);
            gauge_aux->width = ReadInt(fichier);
            if (version > 114)
                strcpy(gauge_aux->infotext, LitChaine(fichier));
            result = gauge_aux;
            break;
        case TY_IMAGE:
            result = LoadImage(fichier, father, version);
            break;
        case TY_LISTVIEW:
            result = LoadListview(fichier, father, version);
            break;
        case TY_DIRLIST:
            result = LoadDirList(fichier, father, version);
            break;
        case TY_CHECK:
            check_aux = AllocVec(sizeof(check), MEMF_PUBLIC | MEMF_CLEAR);
            InitCheckMark(check_aux);
            check_aux->id = obj;
            check_aux->father = father;
            strcpy(check_aux->label, LitChaine(fichier));
            check_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, check_aux);
            if (version > 104)
                ReadNotify(fichier, check_aux);
            if (version >= 113)
                ReadArea(fichier, &check_aux->Area);
            strcpy(check_aux->title, LitChaine(fichier));
            check_aux->title_exist = ReadInt(fichier);
            if (version > 117)
                check_aux->init_state = ReadInt(fichier);
            result = check_aux;
            break;
        case TY_SCALE:
            scale_aux = AllocVec(sizeof(scale), MEMF_PUBLIC | MEMF_CLEAR);
            InitScale(scale_aux);
            scale_aux->id = obj;
            scale_aux->father = father;
            strcpy(scale_aux->label, LitChaine(fichier));
            if (version <= 114)
                sprintf(scale_aux->label, "Scale_%d", nb_scale++);
            scale_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, scale_aux);
            if (version > 104)
                ReadNotify(fichier, scale_aux);
            if (version >= 113)
                ReadArea(fichier, &scale_aux->Area);
            scale_aux->horiz = ReadInt(fichier);
            result = scale_aux;
            break;
        case TY_PROP:
            prop_aux = AllocVec(sizeof(prop), MEMF_PUBLIC | MEMF_CLEAR);
            InitProp(prop_aux);
            prop_aux->id = obj;
            prop_aux->father = father;
            strcpy(prop_aux->label, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            prop_aux->generated = i;
            NextLine(fichier);
            ReadHelp(fichier, prop_aux);
            if (version > 104)
                ReadNotify(fichier, prop_aux);
            if (version >= 113)
                ReadArea(fichier, &prop_aux->Area);
            fscanf(fichier, "%d", &i);
            prop_aux->horizontal = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->entries = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->first = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->visible = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->fixheight = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->fixwidth = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->height = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            prop_aux->width = i;
            NextLine(fichier);
            result = prop_aux;
            break;
        case TY_TEXT:
            result = LoadText(fichier, father, version);
            break;
        case TY_SLIDER:
            slider_aux =
                AllocVec(sizeof(slider), MEMF_PUBLIC | MEMF_CLEAR);
            InitSlider(slider_aux);
            slider_aux->id = obj;
            slider_aux->father = father;
            strcpy(slider_aux->label, LitChaine(fichier));
            slider_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, slider_aux);
            if (version > 104)
                ReadNotify(fichier, slider_aux);
            if (version >= 113)
                ReadArea(fichier, &slider_aux->Area);
            strcpy(slider_aux->title, LitChaine(fichier));
            slider_aux->title_exist = ReadInt(fichier);
            slider_aux->max = ReadInt(fichier);
            slider_aux->min = ReadInt(fichier);
            slider_aux->quiet = ReadInt(fichier);
            slider_aux->init = ReadInt(fichier);
            if (version > 101)
            {
                slider_aux->reverse = ReadInt(fichier);
            }
            if (version > 117)
            {
                slider_aux->horizontal = ReadInt(fichier);
            }
            result = slider_aux;
            break;
        case TY_CYCLE:
            result = LoadCycle(fichier, father, version);
            break;
        case TY_RADIO:
            result = LoadRadio(fichier, father, version);
            break;
        case TY_STRING:
            string_aux =
                AllocVec(sizeof(chaine), MEMF_PUBLIC | MEMF_CLEAR);
            InitString(string_aux);
            string_aux->id = obj;
            string_aux->father = father;
            strcpy(string_aux->label, LitChaine(fichier)),
                fscanf(fichier, "%d", &i);
            string_aux->generated = i;
            NextLine(fichier);
            ReadHelp(fichier, string_aux);
            if (version > 104)
                ReadNotify(fichier, string_aux);
            if (version >= 113)
                ReadArea(fichier, &string_aux->Area);
            strcpy(string_aux->title, LitChaine(fichier));
            strcpy(string_aux->content, LitChaine(fichier));
            if (version < 113)
            {
                fscanf(fichier, "%d", &i);
                string_aux->Area.Weight = i;
                NextLine(fichier);
            }
            fscanf(fichier, "%d", &i);
            string_aux->title_exist = i;
            NextLine(fichier);
            strcpy(string_aux->accept, LitChaine(fichier));
            strcpy(string_aux->reject, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            string_aux->format = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            string_aux->integer = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            string_aux->maxlen = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            string_aux->secret = i;
            NextLine(fichier);
            result = string_aux;
            break;
        case TY_LABEL:
            label_aux = AllocVec(sizeof(label), MEMF_PUBLIC | MEMF_CLEAR);
            InitLabel(label_aux);
            label_aux->id = obj;
            label_aux->father = father;
            strcpy(label_aux->label, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            label_aux->generated = i;
            NextLine(fichier);
            ReadHelp(fichier, label_aux);
            if (version > 104)
                ReadNotify(fichier, label_aux);
            if (version >= 113)
                ReadArea(fichier, &label_aux->Area);
            strcpy(label_aux->title, LitChaine(fichier));
            if (version < 113)
            {
                fscanf(fichier, "%d", &i);
                label_aux->Area.Weight = i;
                NextLine(fichier);
            }
            result = label_aux;
            break;
        case TY_SPACE:
            space_aux = AllocVec(sizeof(space), MEMF_PUBLIC | MEMF_CLEAR);
            InitSpace(space_aux);
            space_aux->id = obj;
            space_aux->father = father;
            strcpy(space_aux->label, LitChaine(fichier));
            if (version <= 114)
                sprintf(space_aux->label, "Space_%d", nb_space++);
            space_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, space_aux);
            if (version > 104)
                ReadNotify(fichier, space_aux);
            if (version > 118)
            {
                space_aux->type = ReadInt(fichier);
                space_aux->spacing = ReadInt(fichier);
            }
            result = space_aux;
            break;
        case TY_RECTANGLE:
            rect_aux =
                AllocVec(sizeof(rectangle), MEMF_PUBLIC | MEMF_CLEAR);
            InitRectangle(rect_aux);
            rect_aux->id = obj;
            rect_aux->father = father;
            strcpy(rect_aux->label, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            rect_aux->generated = i;
            NextLine(fichier);
            ReadHelp(fichier, rect_aux);
            ReadNotify(fichier, rect_aux);
            if (version >= 113)
                ReadArea(fichier, &rect_aux->Area);
            fscanf(fichier, "%d", &i);
            if (version < 113)
                i++;
            rect_aux->type = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            rect_aux->fixheight = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            rect_aux->fixwidth = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            rect_aux->height = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            rect_aux->width = i;
            NextLine(fichier);
            if (version < 113)
            {
                fscanf(fichier, "%d", &i);
                rect_aux->Area.Weight = i;
                NextLine(fichier);
            }
            result = rect_aux;
            break;
        case TY_COLORFIELD:
            field_aux =
                AllocVec(sizeof(colorfield), MEMF_PUBLIC | MEMF_CLEAR);
            InitColorField(field_aux);
            field_aux->id = obj;
            field_aux->father = father;
            strcpy(field_aux->label, LitChaine(fichier));
            field_aux->generated = ReadInt(fichier);
            ReadHelp(fichier, field_aux);
            ReadNotify(fichier, field_aux);
            if (version >= 113)
                ReadArea(fichier, &field_aux->Area);
            if (version < 113)
            {
                field_aux->Area.InputMode = ReadInt(fichier);
            }
            field_aux->fixheight = ReadInt(fichier);
            field_aux->fixwidth = ReadInt(fichier);
            field_aux->height = ReadInt(fichier);
            field_aux->width = ReadInt(fichier);
            if (version > 118)
            {
                field_aux->red = ReadInt(fichier);
                field_aux->green = ReadInt(fichier);
                field_aux->blue = ReadInt(fichier);
            }
            result = field_aux;
            break;
        case TY_POPASL:
            result = LoadPopAsl(fichier, father, version);
            break;
        case TY_POPOBJECT:
            popobj_aux =
                AllocVec(sizeof(popobject), MEMF_PUBLIC | MEMF_CLEAR);
            InitPopObject(popobj_aux);
            popobj_aux->id = obj;
            strcpy(popobj_aux->label, LitChaine(fichier));
            fscanf(fichier, "%d", &i);
            popobj_aux->generated = i;
            popobj_aux->father = father;
            NextLine(fichier);
            ReadHelp(fichier, popobj_aux);
            ReadNotify(fichier, popobj_aux);
            if (version >= 113)
                ReadArea(fichier, &popobj_aux->Area);
            ReadFunctionHook(fichier, &popobj_aux->openhook);
            ReadFunctionHook(fichier, &popobj_aux->closehook);
            fscanf(fichier, "%d", &i);
            NextLine(fichier);
            popobj_aux->obj = LoadObject(fichier, popobj_aux, i);
            fscanf(fichier, "%d", &i);
            popobj_aux->image = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            popobj_aux->follow = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            popobj_aux->light = i;
            NextLine(fichier);
            fscanf(fichier, "%d", &i);
            popobj_aux->Volatile = i;
            NextLine(fichier);
            result = popobj_aux;
            break;
        case TY_MENU:
        case TY_SUBMENU:
        case TY_MENUITEM:
            result = LoadMenu(fichier, father, version, obj);
            break;
        default:
            ErrorMessage(GetMUIBuilderString(MSG_WrongVersion));
            EXIT_PRG();
    }
    return (result);
}
