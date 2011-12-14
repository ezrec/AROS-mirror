/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>               /* sprintf */

#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "About.h"
#include "Locales.h"

#define USE_IMG_ABOUT_COLORS
#include "IMG_About.c"

/****************************************************************************************
  AboutMUI
****************************************************************************************/

static IPTR About_AboutMUI(/*struct IClass* cl,*/ Object* obj/*, Msg msg*/)
{
#if !defined(USE_ZUNE) && !defined(__AROS__)
  Object* app = (Object*)xget(obj, MUIA_ApplicationObject);
  Object* aboutwin = AboutmuiObject,
                       MUIA_Window_RefWindow    , obj,
                       MUIA_Aboutmui_Application, app,
                       End;
  if(aboutwin)
    setatt(aboutwin, MUIA_Window_Open, TRUE);
  return 0;
#else
  return (IPTR)WindowObject, End;
#endif
}

/****************************************************************************************
  New
****************************************************************************************/
static char version_text[50];

static IPTR About_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  Object *BT_Ok, *BT_MUI;
#if !defined(USE_ZUNE) && !defined(__AROS__)
  char *text;

  sprintf(version_text, GetStr(MSG_ABOUT_VERSION), VERSION_NUMBER, VERSION_DATE);
  text = GetStr(MSG_ABOUT);
#endif

  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_HelpNode          , "COPYRIGHT",
    MUIA_Window_Title      , APPNAME,
    MUIA_Window_LeftEdge   , MUIV_Window_LeftEdge_Centered,
    MUIA_Window_TopEdge    , MUIV_Window_TopEdge_Centered,
//    MUIA_Window_CloseGadget, FALSE,
//    MUIA_Window_DepthGadget, FALSE,
//    MUIA_Window_SizeGadget , FALSE,
//    MUIA_Window_DragBar    , FALSE,
//    MUIA_Window_Borderless , TRUE,

    WindowContents, 

      HGroup, ButtonFrame,
        MUIA_Background, MUII_GroupBack,

        /* left side */

        Child, VGroup,
          MUIA_Weight        , 50,

          Child, HVSpace,

          Child, MakeImage(IMG_About_body, 
                           IMG_ABOUT_WIDTH, IMG_ABOUT_HEIGHT, 
                           IMG_ABOUT_DEPTH, IMG_ABOUT_COMPRESSION, 
                           IMG_ABOUT_MASKING, IMG_About_colors),

          Child, TextObject,
            MUIA_Text_PreParse, "\033c",
            MUIA_Text_Contents, APPNAME,
            End,

          Child, TextObject,
            MUIA_Text_PreParse, "\033c",
            MUIA_Text_Contents, version_text,
            MUIA_Font         , MUIV_Font_Tiny,
            End,

          Child, HVSpace,

          Child, HGroup,
            Child, BT_Ok     = MakeButton(MSG_ABOUT_OK , 0),
            Child, BT_MUI    = MakeButton(MSG_ABOUT_MUI, 0),
            End,

          End,  /* left side */


        /* right side */

        Child, VBarObject,

#if !defined(USE_ZUNE) && !defined(__AROS__)
        Child, ListviewObject,
          MUIA_Listview_Input, FALSE,
          MUIA_Weight        , 250,
          MUIA_FixWidthTxt   , text,
          MUIA_Listview_List , FloattextObject,
            MUIA_Background, MUII_GroupBack,
            MUIA_Floattext_Text, text,
            End,
          End,
#endif

        End,

    TAG_MORE, msg->ops_AttrList);

  if (obj)
  {
    DoMethod(obj   , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , obj, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod(BT_Ok , MUIM_Notify, MUIA_Pressed            , FALSE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod(BT_MUI, MUIM_Notify, MUIA_Pressed            , FALSE, obj, 1, MUIM_About_AboutMUI);

    return (IPTR)obj;
  }
  return 0;
}


/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(About_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW             : return About_New     (cl, obj, (struct opSet *)msg);
    case MUIM_About_AboutMUI: return About_AboutMUI(/*cl,*/ obj/*, msg*/);
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_About = NULL;

BOOL About_Init(void)
{
  if(!(CL_About = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, 0, About_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_ABOUTCLASS);
    return FALSE;
  }
  return TRUE;
}

void About_Exit(void)
{
  if(CL_About)
    MUI_DeleteCustomClass(CL_About);
}

