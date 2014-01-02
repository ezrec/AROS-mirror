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

#ifdef __MORPHOS__
#undef USE_INLINE_STDARG
#endif

#include "builder.h"

#if __AROS__
#define MB_BE2WORD(x) AROS_BE2WORD(x)
#else
#define MB_BE2WORD(x) (x)
#endif

UWORD width, height;
int nb_images;

char ControlChar(CONST_STRPTR string)
{
    char *aux;
    char car;

    aux = strchr(string, '_');
    if (aux)
    {
        aux++;
    }
    else
        aux = "";
    car = (char) tolower((int) *aux);
    return (car);
}

char *BuildName(char *ImageNames[], int *nb_images, char *Name)
{
    int size;

    size = strlen(ImageDir) + strlen(Name) + 4;
    if (ImageNames[*nb_images] = AllocVec(size, MEMF_PUBLIC | MEMF_CLEAR))
    {
        sprintf(ImageNames[*nb_images], "5:");
        strncat(ImageNames[*nb_images], ImageDir, size);
        AddPart(ImageNames[*nb_images], Name, size);
    }
    return (ImageNames[(*nb_images)++]);
}

void WidthAndHeight(void)
{
    FILE *fh;
    char ImageName[300];
    char buffer[30];

    strcpy(ImageName, ImageDir);
    AddPart(ImageName, "Group.iff", 300);
    height = 0;
    width = 0;
    if (fh = fopen(ImageName, "r"))
    {
        if (fread(buffer, 1, 26, fh))
        {
            height = MB_BE2WORD(*((UWORD *) &buffer[22]));
            width = MB_BE2WORD(*((UWORD *) &buffer[20]));
        }
        fclose(fh);
    }
}

APTR ImagesWithText(char *ImageNames[], BOOL new)
{
    APTR result;
    APTR bt_keybutton, bt_cancel;
    APTR bt_listview, bt_string;
    APTR bt_group, bt_gauge;
    APTR bt_cycle, bt_radio;
    APTR bt_label, bt_space;
    APTR bt_scale, bt_image;
    APTR bt_check, bt_slider;
    APTR bt_text, bt_prop, bt_popobj;
    APTR bt_dirlist, bt_rect;
    APTR bt_colorfield, bt_popasl, GR_Scroll, VirtGroup;
    int i;

    // *INDENT-OFF*
    result = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ObjectChoice),
        MUIA_Window_ID, MAKE_ID('O', 'B', 'J', 'E'),
        MUIA_HelpNode, "Objects",
        WindowContents, VGroup,
            Child, GR_Scroll = ScrollgroupObject,
                MUIA_Scrollgroup_Contents, VirtGroup = VirtgroupObject,
                    MUIA_Group_Columns, 2 * config.columns,
                    Child, bt_group = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Group.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Group)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Group)),
                    Child, bt_keybutton = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Button.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_KeyButton)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_KeyButton)),
                    Child, bt_listview = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "List.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_ListView)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_ListView)),
                    Child, bt_dirlist = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "DirList.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_DirList)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_DirList)),
                    Child, bt_string = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "String.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_String)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_String)),
                    Child, bt_label = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Label.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_LabelObj)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_LabelObj)),
                    Child, bt_cycle = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Cycle.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Cycle)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Cycle)),
                    Child, bt_radio = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Radio.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Radio)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Radio)),
                    Child, bt_image = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Image.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Image)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Image)),
                    Child, bt_space = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Space.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Space)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Space)),
                    Child, bt_check = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "CheckMark.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_CheckMark)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_CheckMark)),
                    Child, bt_slider = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Slider.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Slider)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child,
                        LLabel(GetMUIBuilderString(MSG_Slider)),
                        Child, bt_gauge = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Gauge.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Gauge)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Gauge)),
                    Child, bt_scale = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Scale.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Scale)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Scale)),
                    Child, bt_text = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Text.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Text)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Text)),
                    Child, bt_prop = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Prop.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Prop)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Prop)),
                    Child, bt_rect = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Rectangle.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Rectangle)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Bar)),
                    Child, bt_colorfield = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "ColorField.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Colorfield)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        ButtonFrame,
                        MUIA_FixWidth, width,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_Colorfield)),
                    Child, bt_popasl = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "PopAsl.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_PopAsl)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width, ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_PopAsl)),
                    Child, bt_popobj = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "PopObject.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_PopObject)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, LLabel(GetMUIBuilderString(MSG_PopObject)),
                End,
            End,
            Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
        End,
    End;
    // *INDENT-ON*

    if (!result)
        return (result);

    for (i = 0;
         i <
         ((2 * config.columns) - (2 * nb_images) % (2 * config.columns));
         i++)
    {
        DoMethod(VirtGroup, OM_ADDMEMBER, HVSpace);
    }

    DoMethod(result, MUIM_Window_SetCycleChain, GR_Scroll,
             bt_group, bt_keybutton, bt_listview, bt_dirlist, bt_string,
             bt_label, bt_cycle, bt_radio, bt_image, bt_space, bt_check,
             bt_slider, bt_gauge, bt_scale, bt_text, bt_prop, bt_rect,
             bt_colorfield, bt_popasl, bt_popobj, bt_cancel, NULL);
    DoMethod(bt_dirlist, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DIRLIST);
    DoMethod(bt_text, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TEXT);
    DoMethod(bt_slider, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SLIDER);
    DoMethod(bt_image, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_IMAGE);
    DoMethod(bt_scale, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SCALE);
    DoMethod(bt_keybutton, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_KEYBUTTON);
    DoMethod(bt_listview, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_LISTVIEW);
    DoMethod(bt_check, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_CHECK);
    DoMethod(bt_string, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_STRING);
    DoMethod(bt_group, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GROUP);
    DoMethod(bt_gauge, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GAUGE);
    DoMethod(bt_prop, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_PROPG);
    DoMethod(bt_cycle, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_CYCLE);
    DoMethod(bt_radio, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_RADIO);
    DoMethod(bt_label, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_LABEL);
    DoMethod(bt_space, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SPACE);
    DoMethod(bt_rect, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_RECTANGLE);
    DoMethod(bt_colorfield, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_COLORFIELD);
    DoMethod(bt_popasl, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPASL);
    DoMethod(bt_popobj, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPOBJECT);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    if (!new)
        set(bt_group, MUIA_Disabled, TRUE);

    return (result);
}

APTR ImagesWithoutText(char *ImageNames[], BOOL new)
{
    APTR result;
    APTR bt_keybutton, bt_cancel;
    APTR bt_listview, bt_string;
    APTR bt_group, bt_gauge;
    APTR bt_cycle, bt_radio;
    APTR bt_label, bt_space;
    APTR bt_scale, bt_image;
    APTR bt_check, bt_slider;
    APTR bt_text, bt_prop, bt_popobj;
    APTR bt_dirlist, bt_rect;
    APTR bt_colorfield, bt_popasl, GR_Scroll, VirtGroup;
    int i;

    // *INDENT-OFF*
    result = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ObjectChoice),
        MUIA_Window_ID, MAKE_ID('O', 'B', 'J', 'E'),
        MUIA_HelpNode, "Objects",
        WindowContents, VGroup,
            Child, GR_Scroll = ScrollgroupObject,
                MUIA_Scrollgroup_Contents, VirtGroup = VirtgroupObject,
                    MUIA_Group_Columns, config.columns,
                    Child, bt_group = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Group.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Group)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_keybutton = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Button.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_KeyButton)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_listview = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "List.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_ListView)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_dirlist = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "DirList.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_DirList)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_string = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "String.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_String)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_label = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Label.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_LabelObj)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_cycle = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Cycle.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Cycle)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_radio = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Radio.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Radio)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_image = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Image.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Image)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_space = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Space.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Space)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_check = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "CheckMark.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_CheckMark)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_slider = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Slider.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Slider)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_gauge = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Gauge.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Gauge)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_scale = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Scale.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Scale)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_text = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Text.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Text)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_prop = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Prop.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Prop)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_rect = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "Rectangle.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Rectangle)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_colorfield = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "ColorField.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_Colorfield)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        ButtonFrame,
                        MUIA_FixWidth, width,
                    End,
                    Child, bt_popasl = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "PopAsl.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_PopAsl)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                    Child, bt_popobj = ImageObject,
                        MUIA_Image_Spec, BuildName(ImageNames, &nb_images, "PopObject.iff"),
                        MUIA_ControlChar, ControlChar(GetMUIBuilderString(MSG_PopObject)),
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Image_FreeHoriz, TRUE,
                        MUIA_FixHeight, height,
                        MUIA_FixWidth, width,
                        ButtonFrame,
                    End,
                End,
            End,
            Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
        End,
    End;
    // *INDENT-ON*

    if (!result)
        return (result);

    for (i = 0; i < (config.columns - nb_images % config.columns); i++)
    {
        DoMethod(VirtGroup, OM_ADDMEMBER, HVSpace);
    }

    DoMethod(result, MUIM_Window_SetCycleChain, GR_Scroll,
             bt_group, bt_keybutton, bt_listview, bt_dirlist, bt_string,
             bt_label, bt_cycle, bt_radio, bt_image, bt_space, bt_check,
             bt_slider, bt_gauge, bt_scale, bt_text, bt_prop, bt_rect,
             bt_colorfield, bt_popasl, bt_popobj, bt_cancel, NULL);
    DoMethod(bt_dirlist, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DIRLIST);
    DoMethod(bt_text, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TEXT);
    DoMethod(bt_slider, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SLIDER);
    DoMethod(bt_image, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_IMAGE);
    DoMethod(bt_scale, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SCALE);
    DoMethod(bt_keybutton, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_KEYBUTTON);
    DoMethod(bt_listview, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_LISTVIEW);
    DoMethod(bt_check, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_CHECK);
    DoMethod(bt_string, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_STRING);
    DoMethod(bt_group, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GROUP);
    DoMethod(bt_gauge, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GAUGE);
    DoMethod(bt_prop, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_PROPG);
    DoMethod(bt_cycle, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_CYCLE);
    DoMethod(bt_radio, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_RADIO);
    DoMethod(bt_label, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_LABEL);
    DoMethod(bt_space, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SPACE);
    DoMethod(bt_rect, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_RECTANGLE);
    DoMethod(bt_colorfield, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_COLORFIELD);
    DoMethod(bt_popasl, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPASL);
    DoMethod(bt_popobj, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPOBJECT);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    if (!new)
        set(bt_group, MUIA_Disabled, TRUE);

    return (result);
}

APTR NewChild(UNUSED APTR obj, BOOL new)
{
    APTR WI_Objects;

    APTR result = NULL;
    BOOL running = TRUE;
    ULONG signal;
    bouton *keybutton_aux;
    listview *list_aux;
    chaine *string_aux;
    group *group_aux;
    gauge *gauge_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    label *label_aux;
    space *space_aux;
    check *check_aux;
    scale *scale_aux;
    image *image_aux;
    slider *slider_aux;
    texte *text_aux;
    prop *prop_aux;
    dirlist *dirlist_aux;
    rectangle *rectangle_aux;
    popaslobj *popasl;
    colorfield *field_aux;
    popobject *popobj_aux;
    int i;
    char *ImageNames[50];

    nb_images = 0;

    WidthAndHeight();
    if ((width == 0) && (height == 0))
    {
        ErrorMessage(GetMUIBuilderString(MSG_CantFindImages));
        return (NULL);
    }

    if (config.labels)
        WI_Objects = ImagesWithText(ImageNames, new);
    else
        WI_Objects = ImagesWithoutText(ImageNames, new);

    WI_current = WI_Objects;


    DoMethod(WI_Objects, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_END);



    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_Objects);
    set(WI_Objects, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_KEYBUTTON:
                keybutton_aux =
                    AllocVec(sizeof(bouton), MEMF_PUBLIC | MEMF_CLEAR);
                if (keybutton_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                keybutton_aux->id = TY_KEYBUTTON;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewButton(keybutton_aux, TRUE))
                {
                    keybutton_aux->generated = TRUE;
                    keybutton_aux->Help.nb_char = 0;
                    keybutton_aux->Help.generated = TRUE;
                    strcpy(keybutton_aux->Help.title,
                           keybutton_aux->title);
                    result = keybutton_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(keybutton_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_COLORFIELD:
                field_aux =
                    AllocVec(sizeof(colorfield), MEMF_PUBLIC | MEMF_CLEAR);
                if (field_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                field_aux->id = TY_COLORFIELD;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewColorField(field_aux, TRUE))
                {
                    field_aux->generated = TRUE;
                    field_aux->Help.generated = TRUE;
                    field_aux->Help.nb_char = 0;
                    strcpy(field_aux->Help.title, "Color Field");
                    result = field_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(field_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_RECTANGLE:
                rectangle_aux =
                    AllocVec(sizeof(rectangle), MEMF_PUBLIC | MEMF_CLEAR);
                if (rectangle_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                rectangle_aux->id = TY_RECTANGLE;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewRectangle(rectangle_aux, TRUE))
                {
                    rectangle_aux->generated = FALSE;
                    rectangle_aux->Help.generated = FALSE;
                    rectangle_aux->Help.nb_char = 0;
                    strcpy(rectangle_aux->Help.title, "Rectangle");
                    result = rectangle_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(rectangle_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_LISTVIEW:
                list_aux =
                    AllocVec(sizeof(listview), MEMF_PUBLIC | MEMF_CLEAR);
                if (list_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                list_aux->id = TY_LISTVIEW;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewListView(list_aux, TRUE))
                {
                    list_aux->generated = TRUE;
                    list_aux->Help.generated = TRUE;
                    list_aux->Help.nb_char = 0;
                    strcpy(list_aux->Help.title, "List Gadget");
                    result = list_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(list_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_DIRLIST:
                dirlist_aux =
                    AllocVec(sizeof(dirlist), MEMF_PUBLIC | MEMF_CLEAR);
                if (dirlist_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                dirlist_aux->id = TY_DIRLIST;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewDirList(dirlist_aux, TRUE))
                {
                    dirlist_aux->generated = TRUE;
                    dirlist_aux->Help.generated = TRUE;
                    dirlist_aux->Help.nb_char = 0;
                    strcpy(dirlist_aux->Help.title, "Directory List");
                    result = dirlist_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(dirlist_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_CHECK:
                check_aux =
                    AllocVec(sizeof(check), MEMF_PUBLIC | MEMF_CLEAR);
                if (check_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                check_aux->id = TY_CHECK;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewCheckMark(check_aux, TRUE))
                {
                    check_aux->generated = TRUE;
                    check_aux->Help.generated = TRUE;
                    check_aux->Help.nb_char = 0;
                    if (check_aux->title_exist)
                        strcpy(check_aux->Help.title, check_aux->title);
                    else
                        strcpy(check_aux->Help.title, "CheckMark Gadget");
                    result = check_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(check_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_IMAGE:
                image_aux =
                    AllocVec(sizeof(image), MEMF_PUBLIC | MEMF_CLEAR);
                if (image_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                image_aux->id = TY_IMAGE;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewImage(image_aux, TRUE))
                {
                    if (image_aux->Area.InputMode)
                        image_aux->generated = TRUE;
                    else
                        image_aux->generated = FALSE;
                    image_aux->Help.generated = FALSE;
                    image_aux->Help.nb_char = 0;
                    strcpy(image_aux->Help.title, "Image Gadget");
                    result = image_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(image_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_TEXT:
                text_aux =
                    AllocVec(sizeof(texte), MEMF_PUBLIC | MEMF_CLEAR);
                if (text_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                text_aux->id = TY_TEXT;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewText(text_aux, TRUE))
                {
                    text_aux->generated = TRUE;
                    text_aux->Help.generated = FALSE;
                    text_aux->Help.nb_char = 0;
                    strcpy(text_aux->Help.title, "Text");
                    result = text_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(text_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_SCALE:
                scale_aux =
                    AllocVec(sizeof(scale), MEMF_PUBLIC | MEMF_CLEAR);
                if (scale_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                scale_aux->id = TY_SCALE;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewScale(scale_aux, TRUE))
                {
                    scale_aux->generated = FALSE;
                    scale_aux->Help.generated = FALSE;
                    scale_aux->Help.nb_char = 0;
                    strcpy(scale_aux->Help.title, "Scale Gadget");
                    result = scale_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(scale_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_SLIDER:
                slider_aux =
                    AllocVec(sizeof(slider), MEMF_PUBLIC | MEMF_CLEAR);
                if (slider_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                slider_aux->id = TY_SLIDER;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewSlider(slider_aux, TRUE))
                {
                    slider_aux->generated = TRUE;
                    slider_aux->Help.generated = TRUE;
                    slider_aux->Help.nb_char = 0;
                    strcpy(slider_aux->Help.title, "Slider Gadget");
                    result = slider_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(slider_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_STRING:
                string_aux =
                    AllocVec(sizeof(chaine), MEMF_PUBLIC | MEMF_CLEAR);
                if (string_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                string_aux->id = TY_STRING;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewString(string_aux, TRUE))
                {
                    string_aux->generated = TRUE;
                    string_aux->Help.generated = TRUE;
                    string_aux->Help.nb_char = 0;
                    if (string_aux->title_exist)
                        strcpy(string_aux->Help.title, string_aux->title);
                    else
                        strcpy(string_aux->Help.title, "String Gadget");
                    result = string_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(string_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_LABEL:
                label_aux =
                    AllocVec(sizeof(label), MEMF_PUBLIC | MEMF_CLEAR);
                if (label_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                label_aux->id = TY_LABEL;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewLabel(label_aux, TRUE))
                {
                    label_aux->generated = FALSE;
                    label_aux->Help.generated = FALSE;
                    label_aux->Help.nb_char = 0;
                    strcpy(label_aux->Help.title, "Label");
                    result = label_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(label_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_SPACE:
                space_aux =
                    AllocVec(sizeof(space), MEMF_PUBLIC | MEMF_CLEAR);
                if (space_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                space_aux->id = TY_SPACE;
                if (NewSpace(space_aux, TRUE))
                {
                    space_aux->generated = FALSE;
                    space_aux->Help.generated = FALSE;
                    space_aux->Help.nb_char = 0;
                    strcpy(space_aux->Help.title, "Space");
                    result = space_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(space_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_GROUP:
                group_aux =
                    AllocVec(sizeof(group), MEMF_PUBLIC | MEMF_CLEAR);
                if (group_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                group_aux->id = TY_GROUP;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewGroup(group_aux, GR_NOROOT, TRUE))
                {
                    group_aux->generated =
                        (group_aux->registermode == TRUE);
                    group_aux->Help.generated = TRUE;
                    group_aux->Help.nb_char = 0;
                    if (strlen(group_aux->Area.TitleFrame) > 0)
                        strcpy(group_aux->Help.title,
                               group_aux->Area.TitleFrame);
                    else
                        strcpy(group_aux->Help.title, "Group");
                    result = group_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(group_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_GAUGE:
                gauge_aux =
                    AllocVec(sizeof(gauge), MEMF_PUBLIC | MEMF_CLEAR);
                if (gauge_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                gauge_aux->id = TY_GAUGE;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewGauge(gauge_aux, TRUE))
                {
                    gauge_aux->generated = TRUE;
                    gauge_aux->Help.generated = TRUE;
                    gauge_aux->Help.nb_char = 0;
                    strcpy(gauge_aux->Help.title, "Gauge Gadget");
                    result = gauge_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(gauge_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_PROPG:
                prop_aux =
                    AllocVec(sizeof(prop), MEMF_PUBLIC | MEMF_CLEAR);
                if (prop_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                prop_aux->id = TY_PROP;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewProp(prop_aux, TRUE))
                {
                    prop_aux->generated = TRUE;
                    prop_aux->Help.generated = TRUE;
                    prop_aux->Help.nb_char = 0;
                    strcpy(prop_aux->Help.title, "Proportionnal Gadget");
                    result = prop_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(prop_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_POPASL:
                popasl =
                    AllocVec(sizeof(popaslobj), MEMF_PUBLIC | MEMF_CLEAR);
                if (popasl == NULL)
                {
                    result = NULL;
                    break;
                }
                popasl->id = TY_POPASL;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewPopAsl(popasl, TRUE))
                {
                    popasl->generated = TRUE;
                    popasl->Help.generated = TRUE;
                    popasl->Help.nb_char = 0;
                    strcpy(popasl->Help.title, "PopAsl Object");
                    result = popasl;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(popasl);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_POPOBJECT:
                popobj_aux =
                    AllocVec(sizeof(popobject), MEMF_PUBLIC | MEMF_CLEAR);
                if (popobj_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                popobj_aux->id = TY_POPOBJECT;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewPopObject(popobj_aux, TRUE))
                {
                    popobj_aux->generated = TRUE;
                    popobj_aux->Help.generated = TRUE;
                    popobj_aux->Help.nb_char = 0;
                    strcpy(popobj_aux->Help.title, "PopObject");
                    result = popobj_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(popobj_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_CYCLE:
                cycle_aux =
                    AllocVec(sizeof(cycle), MEMF_PUBLIC | MEMF_CLEAR);
                if (cycle_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                cycle_aux->id = TY_CYCLE;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewCycle(cycle_aux, TRUE))
                {
                    cycle_aux->generated = TRUE;
                    cycle_aux->Help.generated = TRUE;
                    cycle_aux->Help.nb_char = 0;
                    strcpy(cycle_aux->Help.title, "Cycle Gadget");
                    result = cycle_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(cycle_aux);
                    if (config.phantom)
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                    WI_current = WI_Objects;
                }
                break;
            case ID_RADIO:
                radio_aux =
                    AllocVec(sizeof(radio), MEMF_PUBLIC | MEMF_CLEAR);
                if (radio_aux == NULL)
                {
                    result = NULL;
                    break;
                }
                radio_aux->id = TY_RADIO;
                if (config.phantom)
                    set(WI_Objects, MUIA_Window_Open, FALSE);
                if (NewRadio(radio_aux, TRUE))
                {
                    radio_aux->generated = TRUE;
                    radio_aux->Help.generated = TRUE;
                    radio_aux->Help.nb_char = 0;
                    strcpy(radio_aux->Help.title, "Radio Gadget");
                    result = radio_aux;
                    running = FALSE;
                }
                else
                {
                    DeleteObject(radio_aux);
                    if (config.phantom)
                    {
                        set(WI_Objects, MUIA_Window_Open, TRUE);
                        WI_current = WI_Objects;
                    }
                }
                break;
            case ID_END:
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_Objects, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_Objects);
    MUI_DisposeObject(WI_Objects);
    set(app, MUIA_Application_Sleep, FALSE);
    for (i = 0; i < nb_images; i++)
    {
        FreeVec(ImageNames[i]);
    }
    return (result);
}

BOOL EditChild(object *child_aux)
{
    BOOL bool_aux;
    group *group_aux;

    if (child_aux)
    {
        bool_aux = FALSE;
        switch (child_aux->id)
        {
            case TY_GROUP:
                group_aux = (group *) child_aux;
                bool_aux =
                    NewGroup((group *) child_aux, group_aux->root, FALSE);
                break;
            case TY_KEYBUTTON:
                bool_aux = NewButton((bouton *) child_aux, FALSE);
                break;
            case TY_POPASL:
                bool_aux = NewPopAsl((popaslobj *) child_aux, FALSE);
                break;
            case TY_POPOBJECT:
                bool_aux = NewPopObject((popobject *) child_aux, FALSE);
                break;
            case TY_COLORFIELD:
                bool_aux = NewColorField((colorfield *) child_aux, FALSE);
                break;
            case TY_RECTANGLE:
                bool_aux = NewRectangle((rectangle *) child_aux, FALSE);
                break;
            case TY_STRING:
                bool_aux = NewString((chaine *) child_aux, FALSE);
                break;
            case TY_LISTVIEW:
                bool_aux = NewListView((listview *) child_aux, FALSE);
                break;
            case TY_DIRLIST:
                bool_aux = NewDirList((dirlist *) child_aux, FALSE);
                break;
            case TY_CHECK:
                bool_aux = NewCheckMark((check *) child_aux, FALSE);
                break;
            case TY_SCALE:
                bool_aux = NewScale((scale *) child_aux, FALSE);
                break;
            case TY_SPACE:
                bool_aux = NewSpace((space *) child_aux, FALSE);
                break;
            case TY_IMAGE:
                bool_aux = NewImage((image *) child_aux, FALSE);
                break;
            case TY_SLIDER:
                bool_aux = NewSlider((slider *) child_aux, FALSE);
                break;
            case TY_GAUGE:
                bool_aux = NewGauge((gauge *) child_aux, FALSE);
                break;
            case TY_PROP:
                bool_aux = NewProp((prop *) child_aux, FALSE);
                break;
            case TY_CYCLE:
                bool_aux = NewCycle((cycle *) child_aux, FALSE);
                break;
            case TY_RADIO:
                bool_aux = NewRadio((radio *) child_aux, FALSE);
                break;
            case TY_LABEL:
                bool_aux = NewLabel((label *) child_aux, FALSE);
                break;
            case TY_TEXT:
                bool_aux = NewText((texte *) child_aux, FALSE);
                break;
        }
    }
    return (bool_aux);
}
