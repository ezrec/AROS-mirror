/***************************************************************************

 Toolbar MCP - MUI Custom Class Preferences for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

// ANSI
#include <string.h>

// Protos
#include <clib/alib_protos.h>
#include <proto/locale.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/exec.h>

// Libraries
#include <libraries/asl.h>
#define MUI_OBSOLETE
#include <libraries/mui.h>

#include "rev.h"
#include "MyLocale.h"
#include "InstanceData.h"
#include "CreatePrefs.h"

// add a replacemnet define for the standard sprintf
#define sprintf MySPrintf

#ifdef __AROS__
AROS_UFH2S(void, putchfunc, 
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(UBYTE **, data, A3))
{
    AROS_USERFUNC_INIT
    
    *(*data)++ = chr;
    
    AROS_USERFUNC_EXIT
}

int MySPrintf(char *buf, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

#if 0
	APTR raw_args[] = { &args, fmt };
	RawDoFmt("%V", raw_args, NULL, buf);
#else
#warning "FIXME: Code above is better, but doesn't work because locale.library RawDoFmt"
#warning "       replacement doesn't know %V"

    	{
    	    UBYTE *bufptr = buf;
	
	    RawDoFmt(fmt, &fmt + 1, NULL, bufptr);
    	}
#endif
	
	va_end(args);
	
	return strlen(buf);
}
#elif defined(__amigaos4__) || defined(__MORPHOS__)
static int STDARGS VARARGS68K MySPrintf(char *buf, char *fmt, ...)
{
  VA_LIST args;

  VA_START(args, fmt);
  RawDoFmt(fmt, VA_ARG(args, void *), NULL, buf);
  VA_END(args);

  return(strlen(buf));
}
#else
static int STDARGS MySPrintf(char *buf, char *fmt,...)
{
  static const UWORD PutCharProc[2] = {0x16C0,0x4E75};
  /* dirty hack to avoid assembler part :-)
    16C0: move.b d0,(a3)+
     4E75: rts */
  va_list args;

  va_start(args, fmt);
  RawDoFmt(fmt, args, (APTR)PutCharProc, buf);
  va_end(args);

  return(strlen(buf));
}
#endif

Object *CreatePrefs(struct Toolbar_DataP *data)
{
  Object **objs = data->Gadgets;
  Object *textgroup, *imagegroup;
  Object *prefsgroup;

  static STRPTR LookStrings[4];
  static STRPTR BorderTypeStrings[4];
  static STRPTR SelectionModeStrings[4];
  static STRPTR PrecisionStrings[5];
  static STRPTR GhostEffectStrings[5];
  static char AboutText[300];

  BorderTypeStrings[0] = LOCALE(MSG_BORDERTYPE_OLD, "Old");
  BorderTypeStrings[1] = LOCALE(MSG_BORDERTYPE_OFF, "Off");
  BorderTypeStrings[2] = LOCALE(MSG_BORDERTYPE_NEW,  "New");
  BorderTypeStrings[3] = NULL;

  SelectionModeStrings[0] = LOCALE(MSG_SELECTIONMODE_OLD, "Old");
  SelectionModeStrings[1] = LOCALE(MSG_SELECTIONMODE_OFF, "Off");
  SelectionModeStrings[2] = LOCALE(MSG_SELECTIONMODE_NEW,  "New");
  SelectionModeStrings[3] = NULL;

  LookStrings[0] = LOCALE(MSG_IMAGETEXT, "Image and Text");
  LookStrings[1] = LOCALE(MSG_IMAGEONLY, "Image only");
  LookStrings[2] = LOCALE(MSG_TEXTONLY,  "Text only");
  LookStrings[3] = NULL;

  PrecisionStrings[0] = LOCALE(MSG_EXACT, "Exact");
  PrecisionStrings[1] = LOCALE(MSG_IMAGE, "Image");
  PrecisionStrings[2] = LOCALE(MSG_ICON,  "Icon");
  PrecisionStrings[3] = LOCALE(MSG_GUI,   "GUI");
  PrecisionStrings[4] = NULL;

//  GhostEffectStrings[0] = LOCALE(MSG_DIMMMED,     "Dimmed"); // Temporarily disabled - remember +-1
  GhostEffectStrings[0] = LOCALE(MSG_LIGHT,       "Light Grid");
  GhostEffectStrings[1] = LOCALE(MSG_HEAVY,       "Heavy Grid");
  GhostEffectStrings[2] = LOCALE(MSG_SUPERLIGHT,  "Superlight Grid");
  GhostEffectStrings[3] = NULL;

  sprintf(AboutText, "\033cToolbar.mcc Version %s (%s)\n%s", LIB_REV_STRING, LIB_DATE, LIB_COPYRIGHT);

  prefsgroup =
    VGroup,
      Child, VGroup,
        Child, HGroup,
//        MUIA_Group_SameWidth, TRUE,
/*********************************  General  *********************************/
          Child, HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_GENERAL, "General"),
            Child, RectangleObject, End,
            Child, VGroup,
              Child, RectangleObject, End,
              Child, objs[Look] = RadioObject,
  //                MUIA_FrameTitle, "Toolbar look",
                MUIA_Radio_Entries, LookStrings,
              End,
              Child, RectangleObject, End,
            End,
  //ghosttype
            Child, RectangleObject, End,
          End,

/*********************************  Image  *********************************/
          Child, imagegroup = HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_IMAGE, "Image"),
            Child, RectangleObject, End,
            Child, VGroup,
              Child, RectangleObject, End,
              Child, HGroup,
                Child, RectangleObject, End,
                Child, VGroup,
                  Child, Label(LOCALE(MSG_USEIMAGES1, "\33cUse ghosted/selected images")),
//                  Child, MUI_MakeObject(MUIO_Label, LOCALE(MSG_USEIMAGES2, "\33c(if they are available)"), MUIO_Label_Tiny),
                  Child, TextObject,
                    MUIA_Text_Contents, LOCALE(MSG_USEIMAGES2, "\33c(if they are available)"),
                    MUIA_Font, MUIV_Font_Tiny,
                  End,
                End,
                Child, objs[UseImages] = CheckMark(TRUE),
//                Child, RectangleObject, End,
              End,
              Child, ColGroup(2),
//              Child, HGroup,
                Child, Label(LOCALE(MSG_REMAP, "Remap Precision")),
                Child, objs[Precision] = Cycle(PrecisionStrings),
//              End,
//              Child, HGroup,
                Child, Label(LOCALE(MSG_GHOSTEFFECT, "Ghosting effect")),
                Child, objs[GhostEffect] = Cycle(GhostEffectStrings),
//              End,
              End,

              Child, RectangleObject, End,
            End,
            Child, RectangleObject, End,
          End,
        End,

/*********************************  Spacing  *********************************/
        Child, HGroup,
          Child, HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_SPACING, "Spacing"),
            Child, VGroup,
              Child, RectangleObject, End,
              Child, ColGroup(2),
                Child, Label(LOCALE(MSG_GROUPSPACE, "Group Space")),
                Child, objs[GroupSpace] = SliderObject,
                  MUIA_Slider_Min,   -1,
                  MUIA_Slider_Max,   50,
                End,
                Child, Label(LOCALE(MSG_TOOLSPACE, "Tool Space")),
                Child, objs[ToolSpace] = SliderObject,
                  MUIA_Slider_Min,   -1,
                  MUIA_Slider_Max,   10,
                End,
                Child, VGroup,
                  Child, Label(LOCALE(MSG_INNERSPACE, "\33cInner Space")),
//                  Child, MUI_MakeObject(MUIO_Label,LOCALE(MSG_INNERSPACE_WITH, "\33c(with tooltext)"), MUIO_Label_Tiny),
                  Child, TextObject,
                    MUIA_Text_Contents, LOCALE(MSG_INNERSPACE_WITH, "\33c(with tooltext)"),
                    MUIA_Font, MUIV_Font_Tiny,
                  End,
                End,
                Child, objs[InnerSpace_Text] = SliderObject,
                  MUIA_Slider_Min,   0,
                  MUIA_Slider_Max,   10,
                End,
                Child, VGroup,
                  Child, Label(LOCALE(MSG_INNERSPACE, "\33cInner Space")),
//                  Child, MUI_MakeObject(MUIO_Label,LOCALE(MSG_INNERSPACE_WITHOUT, "\33c(without tooltext)"), MUIO_Label_Tiny),
                  Child, TextObject,
                    MUIA_Text_Contents, LOCALE(MSG_INNERSPACE_WITHOUT, "\33c(without tooltext)"),
                    MUIA_Font, MUIV_Font_Tiny,
                  End,
                End,
                Child, objs[InnerSpace_NoText] = SliderObject,
                  MUIA_Slider_Min,   0,
                  MUIA_Slider_Max,   10,
                  MUIA_Disabled, TRUE,
                End,
  // some kind of relative mode
  // width: fixed/floating
              End,
              Child, RectangleObject, End,
            End,
          End,

  /*********************************  Text  *********************************/
          Child, textgroup = HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_TEXT, "Text"),
            Child, VGroup,
              Child, RectangleObject, End,
/*              Child, HGroup,
                Child, RectangleObject, End,
                Child, Label(LOCALE(MSG_PLACEMENT, "Placement")),
                Child, data->CY_Placement = Cycle(CY_textplacement),
//                Child, RectangleObject, End,
              End,
*/              Child, ColGroup(2),
//                Child, VGroup,
                  Child, Label(LOCALE(MSG_COLOR, "Text Color")),
//                End,
//                Child, VGroup,
                  Child, objs[ToolPen] = PoppenObject,
                    MUIA_Window_Title, LOCALE(MSG_ADJUST, "Adjust Text Color"),
                    MUIA_Draggable, TRUE,
                    MUIA_ShortHelp, LOCALE(MSG_COLOR_HELP, "Color of the tool texts."),
                  End,
                  Child, Label2(LOCALE(MSG_FONT, "Text Font")),
                  Child, data->FontASL = PopaslObject,
                    MUIA_Popstring_String, objs[ToolFont] = String(0,35),
                    MUIA_Popstring_Button, PopButton(MUII_PopUp),
                    MUIA_Popasl_Type , ASL_FontRequest,
                    ASLFO_TitleText  , LOCALE(MSG_SELECT, "Please select a text font..."),
                  End,
//                End,
              End,
              Child, RectangleObject, End,
            End,
          End,
        End,
// Sløringseffekt: Normal, 3D, ...
  End,
/*
      Child, HGroup,
        Child, HGroup,
          MUIA_Background, MUII_GroupBack,
          MUIA_Frame     , MUIV_Frame_Group,
          MUIA_FrameTitle, LOCALE(MSG_BORDERTYPE, "Border Type"),
          InnerSpacing(12, 12),
          Child, objs[BorderType] = Cycle(BorderTypeStrings),
        End,
        Child, VGroup,
          MUIA_Background, MUII_GroupBack,
          MUIA_Frame     , MUIV_Frame_Group,
          MUIA_FrameTitle, LOCALE(MSG_SELECTIONMODE, "Selection Border"),
          InnerSpacing(12, 12),
          Child, objs[SelectionMode] = Cycle(SelectionModeStrings),
          Child, HGroup,
            Child, Label2(LOCALE(MSG_AUTOACTIVE, "Show Active on Mousehi")),
            Child, objs[AutoActive] = CheckMark(TRUE),
          End,
        End,
      End,
*/



        Child, HGroup,


          Child, HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_BORDERTYPE, "Border Type"),
            Child, RectangleObject, End,
            Child, VGroup,
              Child, RectangleObject, End,
              Child, objs[BorderType] = Cycle(BorderTypeStrings),
              Child, RectangleObject, End,
            End,
            Child, RectangleObject, End,
          End,

          Child, HGroup,
            MUIA_Background, MUII_GroupBack,
            MUIA_Frame     , MUIV_Frame_Group,
            MUIA_FrameTitle, LOCALE(MSG_SELECTIONMODE, "Selection Border"),
            Child, RectangleObject, End,
            Child, VGroup,
              Child, RectangleObject, End,
              MUIA_Weight, 200,
              Child, objs[SelectionMode] = Cycle(SelectionModeStrings),
              Child, HGroup,
                Child, Label2(LOCALE(MSG_AUTOACTIVE, "Show Active on Mousehit")),
                Child, objs[AutoActive] = CheckMark(TRUE),
              End,            
              Child, RectangleObject, End,
            End,
            Child, RectangleObject, End,
          End,


        End,


      Child, TextObject,
        TextFrame,
        MUIA_Background, MUII_TextBack,
        MUIA_FixHeightTxt, "\n\n",
        MUIA_Text_Contents, AboutText,
      End,
    End;

  if(prefsgroup)
  {
    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 0, textgroup, 7, MUIM_MultiSet, MUIA_Disabled, FALSE, textgroup, imagegroup, objs[InnerSpace_Text], NULL);
    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 1, textgroup, 6, MUIM_MultiSet, MUIA_Disabled, TRUE, textgroup, objs[InnerSpace_Text], NULL);
    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 2, textgroup, 6, MUIM_MultiSet, MUIA_Disabled, FALSE, textgroup, objs[InnerSpace_Text], NULL);

    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 0, objs[InnerSpace_NoText], 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 1, imagegroup, 6, MUIM_MultiSet, MUIA_Disabled, FALSE, imagegroup, objs[InnerSpace_NoText], NULL);
    DoMethod(objs[Look], MUIM_Notify, MUIA_Radio_Active, 2, imagegroup, 6, MUIM_MultiSet, MUIA_Disabled, TRUE, imagegroup, objs[InnerSpace_NoText], NULL);
  }

  return prefsgroup;
}
