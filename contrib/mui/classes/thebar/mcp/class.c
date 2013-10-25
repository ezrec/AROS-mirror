/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"
#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include <libraries/asl.h>
#include "version.h"
#include "private.h"

/***********************************************************************/

enum MItems { MI_GLOBALS, MI_LASTSAVED, MI_RESTORE, MI_DEFAULTS };

enum
{
    FLG_MenuCheck = 1<<0,
    FLG_Global    = 1<<1,
};

/***********************************************************************/

static IPTR
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
    {
        struct InstData *data = INST_DATA(cl,obj);
        Object          *prefs, *trans;
        const char      *t;
        static const char infotext1[] = "\033bTheBar.mcp " LIB_REV_STRING "\033n (" LIB_DATE ")\n"
                                        "Copyright (C) 2003-2005 Alfonso Ranieri\n"
                                        LIB_COPYRIGHT;
        static const char infotext2[] = "\n"
                                        "Distributed under the terms of the LGPL2.\n"
                                        "\n"
                                        "For recent versions and updates visit:\n"
                                        "http://www.sourceforge.net/projects/thebar/\n"
                                        "\n";

        if((t = tr(Msg_Info_Translator)) && *t)
        {
            trans = (Object *)VGroup,
                MUIA_InnerLeft,0,
                MUIA_InnerBottom,0,
                MUIA_InnerTop,0,
                MUIA_InnerRight,0,
                Child, TextObject,
                    MUIA_Text_PreParse, (IPTR)"\033c",
                    MUIA_Text_Contents, (IPTR)t,
                End,
                Child, ofhspace((IPTR)"a"),
            End;
        }
        else
          trans = NULL;

        if((prefs = (Object *)VGroup,

            Child, RegisterGroup(regs), // Register

                MUIA_CycleChain, TRUE,

                Child, VGroup,
                //    MUIA_Group_Columns, 2,

                    // >BarFrame
                    (lib_flags & BASEFLG_MUI4) ? TAG_IGNORE : Child,
                    (lib_flags & BASEFLG_MUI4) ? 0 : (VGroup,
                        GroupFrameT(tr(Msg_Title_BarFrame)),
                        Child, (lib_flags & BASEFLG_MUI20) ?
                            (VGroup,
                                Child, owspace(20),
                                Child, HGroup,
                                    Child, (IPTR)oflabel(Msg_Frame),
                                    Child, (IPTR)(data->frame = opopframe(Msg_Frame,Msg_Frame_Title,Msg_Frame_Help)),
                                End,
                                Child, owspace(20),
                             End) :
                            (ColGroup(2),
                                Child, (IPTR)oflabel(Msg_BarFrameShine),
                                Child, (IPTR)(data->barFrameShinePen = opoppen(Msg_BarFrameShine,Msg_BarFrameShine_Title,Msg_BarFrameShine_Help)),
                                Child, (IPTR)oflabel(Msg_BarFrameShadow),
                                Child, (IPTR)(data->barFrameShadowPen = opoppen(Msg_BarFrameShadow,Msg_BarFrameShadow_Title,Msg_BarFrameShadow_Help)),
                             End),
                    End), // <BarFrame

                    Child, ColGroup(3), // >Backrounds
                        GroupFrameT(tr(Msg_Title_Backgrounds)),

                        Child, (IPTR)oflabel(Msg_GroupBackground),
                        Child, data->groupBack = (lib_flags & BASEFLG_MUI4) ? opopfri(Msg_GroupBackground,Msg_GroupBackground_Title,Msg_GroupBackground_Help) :
                                                                              opopback(TRUE,Msg_GroupBackground,Msg_GroupBackground_Title,Msg_GroupBackground_Help),

                        Child,
                        (VGroup,
                            Child, (IPTR)VSpace(0),
                            Child, HGroup,
                                Child, (IPTR)(data->useGroupBack = ocheck(Msg_UseGroupBackground, Msg_UseGroupBackground_Help)),
                            End,
                            Child, (IPTR)VSpace(0),
                        End),
                        Child, (IPTR)oflabel(Msg_ButtonBackground),
                        Child, (IPTR)(data->buttonBack = (lib_flags & BASEFLG_MUI4) ? opopfri(Msg_ButtonBackground,Msg_ButtonBackground_Title,Msg_ButtonBackground_Help) :
                                                                                      opopback(FALSE,Msg_ButtonBackground,Msg_ButtonBackground_Title,Msg_ButtonBackground_Help)),
                        Child,
                        (VGroup,
                            Child, (IPTR)VSpace(0),
                            Child, HGroup,
                                Child, (IPTR)(data->useButtonBack = ocheck(Msg_UseButtonBackground ,Msg_UseButtonBackground_Help)),
                            End,
                            Child, (IPTR)VSpace(0),
                        End),
                    End, // <Backgrounds

                    Child, ColGroup(2), // >DragBar
                        GroupFrameT(tr(Msg_Title_DragBar)),
                        Child, (IPTR)oflabel(Msg_DragBarShine),
                        Child, HGroup,
                            Child, (IPTR)(data->dragBarShinePen = opoppen(Msg_DragBarShine,Msg_DragBarShine_Title,Msg_DragBarShine_Help)),
                            Child, (IPTR)oflabel(Msg_DragBarShadow),
                            Child, (IPTR)(data->dragBarShadowPen = opoppen(Msg_DragBarShadow,Msg_DragBarShadow_Title,Msg_DragBarShadow_Help)),
                        End,
                        Child, (IPTR)oflabel(Msg_DragBarFill),
                        Child, HGroup,
                            Child, (IPTR)(data->dragBarFillPen = opoppen(Msg_DragBarFill,Msg_DragBarFill_Title,Msg_DragBarFill_Help)),
                             Child, (IPTR)oflabel(Msg_UseDragBarFill),
                            Child, VGroup,
                                Child, (IPTR)VSpace(0),
                                Child, (IPTR)(data->useDragBarFillPen = ocheck(Msg_UseDragBarFill, Msg_UseDragBarFill_Help)),
                                Child, (IPTR)VSpace(0),
                            End,
                        End,
                    End, // <DragBar

                    Child, ColGroup(2), // >Grey
                        GroupFrameT(tr(Msg_Title_Disabled)),
                        Child, (IPTR)oflabel(Msg_DisabledBody),
                        Child, (IPTR)(data->disBodyPen = opoppen(Msg_DisabledBody,Msg_DisabledBody_Title,Msg_DisabledBody_Help)),
                        Child, (IPTR)(oflabel(Msg_DisabledShadow)),
                        Child, (IPTR)(data->disShadowPen = opoppen(Msg_DisabledShadow,Msg_DisabledShadow_Title,Msg_DisabledShadow_Help)),
                    End, // <Grey

                    Child, ColGroup(2), // >Bar spacer
                        GroupFrameT(tr(Msg_Title_BarSpacer)),
                        Child, (IPTR)oflabel(Msg_BarSpacerShinePen),
                        Child, (IPTR)(data->barSpacerShinePen = opoppen(Msg_BarSpacerShinePen,Msg_BarSpacerShinePen_Title,Msg_BarSpacerShinePen_Help)),
                        Child, (IPTR)oflabel(Msg_BarSpacerShadowPen),
                        Child, (IPTR)(data->barSpacerShadowPen = opoppen(Msg_BarSpacerShadowPen,Msg_BarSpacerShadowPen_Title,Msg_BarSpacerShadowPen_Help)),
                    End, // <Bar spacer

                    // >Buttons frame
                    (lib_flags & BASEFLG_MUI4) ? TAG_IGNORE : Child,
                    (lib_flags & BASEFLG_MUI4) ? 0 : (ColGroup(2),
                        GroupFrameT(tr(Msg_Title_Frame)),
                        Child, (IPTR)oflabel(Msg_FrameShinePen),
                        Child, HGroup,
                            Child, (IPTR)(data->frameShinePen = opoppen(Msg_FrameShinePen,Msg_FrameShinePen_Title,Msg_FrameShinePen_Help)),
                            Child, owspace(20),
                            Child, (IPTR)oflabel(Msg_FrameShadowPen),
                            Child, (IPTR)(data->frameShadowPen = opoppen(Msg_FrameShadowPen,Msg_FrameShadowPen_Title,Msg_FrameShadowPen_Help)),
                        End,
                        Child, (IPTR)olabel2(Msg_FrameStyle),
                        Child, (IPTR)(data->frameStyle = ocycle(frames,Msg_FrameStyle,Msg_FrameStyle_Help)),
                    End), // <Buttons frame

                End, // <Colors

                Child, VGroup, // >Appearance

                    Child, ColGroup(2), // >Fonts, scale
                        Child, (IPTR)olabel2(Msg_TextFont),
                        Child, PopaslObject,
                            MUIA_ShortHelp,   (IPTR)tr(Msg_TextFont_Help),
                            MUIA_Popasl_Type, ASL_FontRequest,
                            MUIA_Popstring_String, (IPTR)(data->textFont = (Object *)StringObject,
                                StringFrame,
                                MUIA_CycleChain,  TRUE,
                                MUIA_ControlChar, getKeyChar(tr(Msg_TextFont)),
                            End),
                            MUIA_Popstring_Button, (IPTR)opop(MUII_PopFont,Msg_TextFont_Help),
                        End,
                        Child, (IPTR)olabel2(Msg_TextGfxFont),
                        Child, PopaslObject,
                            MUIA_ShortHelp,   (IPTR)tr(Msg_TextGfxFont_Help),
                            MUIA_Popasl_Type, ASL_FontRequest,
                            MUIA_Popstring_String, (IPTR)(data->textGfxFont = (Object *)StringObject,
                                StringFrame,
                                MUIA_CycleChain,  TRUE,
                                MUIA_ControlChar, getKeyChar(tr(Msg_TextGfxFont)),
                            End),
                            MUIA_Popstring_Button, (IPTR)opop(MUII_PopFont,Msg_TextGfxFont_Help),
                        End,
                        Child, (IPTR)olabel2(Msg_Scale),
                        Child, (IPTR)(data->scale = oslider(Msg_Scale,Msg_Scale_Help,25,400)),
                    End, // <Fonts, scale

                    Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_Weight, 0, End,

                    Child, ColGroup(2), // >View modes, label pos
                        Child, (IPTR)olabel2(Msg_ViewMode),
                        Child, (IPTR)(data->viewMode = ocycle(viewModes,Msg_ViewMode,Msg_ViewMode_Help)),
                        Child, (IPTR)olabel2(Msg_LabelPos),
                        Child, (IPTR)(data->labelPos = ocycle(labelPoss,Msg_LabelPos,Msg_LabelPos_Help)),
                    End, // <View modes, label pos

                    Child, VGroup, // >Options
                      MUIA_Frame, MUIV_Frame_Virtual,
                      Child, ColGroup(3),
                        Child, (IPTR)(data->borderless = ocheck(Msg_Borderless,Msg_Borderless_Help)),
                        Child, (IPTR)ollabel1(Msg_Borderless),
                        Child, (IPTR)HSpace(0),

                        Child, (IPTR)(data->raised = ocheck(Msg_Raised,Msg_Raised_Help)),
                        Child, (IPTR)ollabel1(Msg_Raised),
                        Child, (IPTR)HSpace(0),

                        Child, (IPTR)(data->sunny = ocheck(Msg_Sunny,Msg_Sunny_Help)),
                        Child, (IPTR)ollabel1(Msg_Sunny),
                        Child, (IPTR)HSpace(0),

                        Child, (IPTR)(data->scaled = ocheck(Msg_Scaled,Msg_Scaled_Help)),
                        Child, (IPTR)ollabel1(Msg_Scaled),
                        Child, (IPTR)HSpace(0),

                        Child, (IPTR)(data->barSpacer = ocheck(Msg_BarSpacer,Msg_BarSpacer_Help)),
                        Child, (IPTR)ollabel1(Msg_BarSpacer),
                        Child, (IPTR)HSpace(0),

                        Child, (IPTR)(data->enableKeys = ocheck(Msg_EnableKeys,Msg_EnableKeys_Help)),
                        Child, (IPTR)ollabel1(Msg_EnableKeys),
                        Child, (IPTR)HSpace(0),
                      End,
                      Child, (IPTR)HVSpace,
                    End, // <Options

                End, // <Appearance

                Child, VGroup,  // >Spacing
                    Child, VGroup, // >Group spacing
                        GroupFrameT(tr(Msg_Title_GroupSpacing)),
                        //Child, owspace(1),
                        Child, ColGroup(2),
                            Child, (IPTR)olabel2(Msg_HorizGroupSpacing),
                            Child, HGroup,
                                Child, (IPTR)(data->horizSpacing = oslider(Msg_HorizGroupSpacing,Msg_HorizGroupSpacing_Help,0,16)),
                                Child, (IPTR)olabel2(Msg_VertGroupSpacing),
                                Child, (IPTR)(data->vertSpacing = oslider(Msg_VertGroupSpacing,Msg_VertGroupSpacing_Help,0,16)),
                            End,
                            Child, (IPTR)olabel2(Msg_BarSpacerSpacing),
                            Child, (IPTR)(data->barSpacerSpacing = oslider(Msg_BarSpacerSpacing,Msg_BarSpacerSpacing_Help,0,16)),
                        End,
                        //Child, owspace(1),
                    End, // <Group spacing

                    // >Inner spacing
                    (lib_flags & BASEFLG_MUI4) ? TAG_IGNORE : Child,
                    (lib_flags & BASEFLG_MUI4) ? 0 : (VGroup,
                        GroupFrameT(tr(Msg_Title_InnerSpacing)),
                        //Child, owspace(1),
                        Child, ColGroup(2),
                            Child, (IPTR)olabel2(Msg_HorizInnerSpacing),
                            Child, (IPTR)(data->horizInnerSpacing = oslider(Msg_HorizInnerSpacing,Msg_HorizInnerSpacing_Help,1,16)),
                            Child, (IPTR)olabel2(Msg_TopInnerSpacing),
                            Child, HGroup,
                                Child, (IPTR)(data->topInnerSpacing = oslider(Msg_TopInnerSpacing,Msg_TopInnerSpacing_Help,0,16)),
                                Child, (IPTR)olabel2(Msg_BottomInnerSpacing),
                                Child, (IPTR)(data->bottomInnerSpacing = oslider(Msg_BottomInnerSpacing,Msg_BottomInnerSpacing_Help,0,16)),
                            End,
                        End,
                        //Child, owspace(1),
                    End), // <Inner spacing

                    // >BarFrame spacing
                    (lib_flags & BASEFLG_MUI20) ? TAG_IGNORE : Child,
                    (lib_flags & BASEFLG_MUI20) ? 0 : (VGroup,
                        GroupFrameT(tr(Msg_Title_BarFrameSpacing)),
                        //Child, owspace(1),
                        Child, ColGroup(4),
                            Child, (IPTR)olabel2(Msg_LeftBarFrameSpacing),
                            Child, (IPTR)(data->leftBarFrameSpacing = oslider(Msg_LeftBarFrameSpacing,Msg_LeftBarFrameSpacing_Help,1,16)),
                            Child, (IPTR)olabel2(Msg_RightBarFrameSpacing),
                            Child, (IPTR)(data->rightBarFrameSpacing = oslider(Msg_RightBarFrameSpacing,Msg_RightBarFrameSpacing_Help,1,16)),
                            Child, (IPTR)olabel2(Msg_TopBarFrameSpacing),
                            Child, (IPTR)(data->topBarFrameSpacing = oslider(Msg_TopBarFrameSpacing,Msg_TopBarFrameSpacing_Help,1,16)),
                            Child, (IPTR)olabel2(Msg_BottomBarFrameSpacing),
                            Child, (IPTR)(data->bottomBarFrameSpacing = oslider(Msg_BottomBarFrameSpacing,Msg_BottomBarFrameSpacing_Help,1,16)),
                        End,
                        //Child, owspace(1),
                    End), // <BarFrame spacing

                    Child, VGroup, // >Text/Gfx spacing
                        GroupFrameT(tr(Msg_Title_TextGfxSpacing)),
                        //Child, owspace(1),
                        Child, ColGroup(2),
                            Child, (IPTR)olabel2(Msg_HorizTextGfxSpacing),
                            Child, (IPTR)(data->horizTexGfxSpacing = oslider(Msg_HorizTextGfxSpacing,Msg_HorizTextGfxSpacing_Help,1,16)),
                            Child, (IPTR)olabel2(Msg_VertTextGfxSpacing),
                            Child, (IPTR)(data->vertTexGfxSpacing = oslider(Msg_VertTextGfxSpacing,Msg_VertTextGfxSpacing_Help,1,16)),
                        End,
                        //Child, owspace(1),
                    End, // <Text/Gfx spacing
                    Child, (IPTR)VSpace(0),
                End, // <Spacing

                Child, VGroup, // >Settings
                    Child, ColGroup(2),
                        Child, (IPTR)olabel2(Msg_Precision),
                        Child, (IPTR)(data->precision = ocycle(precisions,Msg_Precision,Msg_Precision_Help)),
                        Child, (IPTR)olabel2(Msg_DisMode),
                        Child, (IPTR)(data->disMode = ocycle(dismodes,Msg_DisMode,Msg_DisMode_Help)),
                        Child, (IPTR)olabel2(Msg_SpacersSize),
                        Child, (IPTR)(data->spacersSize = ocycle(spacersSizes,Msg_SpacersSize,Msg_SpacersSize_Help)),
                    End,

                    Child, VGroup,
                        MUIA_Frame, MUIV_Frame_Virtual,
                        Child, ColGroup(3),
                            Child, (IPTR)(data->specialSelect = ocheck(Msg_SpecialSelect,Msg_SpecialSelect_Help)),
                            Child, (IPTR)ollabel1(Msg_SpecialSelect),
                            Child, (IPTR)HSpace(0),

                            Child, (IPTR)(data->textOverUseShine = ocheck(Msg_TextOverUseShine,Msg_TextOverUseShine_Help)),
                            Child, (IPTR)ollabel1(Msg_TextOverUseShine),
                            Child, (IPTR)HSpace(0),

                            Child, (IPTR)(data->dontMove = ocheck(Msg_DontMove,Msg_DontMove_Help)),
                            Child, (IPTR)ollabel1(Msg_DontMove),
                            Child, (IPTR)HSpace(0),

                            Child, (IPTR)(data->ignoreSel = ocheck(Msg_IgnoreSelImages,Msg_IgnoreSelImages_Help)),
                            Child, (IPTR)ollabel1(Msg_IgnoreSelImages),
                            Child, (IPTR)HSpace(0),

                            Child, (IPTR)(data->ignoreDis = ocheck(Msg_IgnoreDisImages,Msg_IgnoreDisImages_Help)),
                            Child, (IPTR)ollabel1(Msg_IgnoreDisImages),
                            Child, (IPTR)HSpace(0),

                            Child, (IPTR)(data->ntRaiseActive = ocheck(Msg_NtRaiseActive,Msg_NtRaiseActive_Help)),
                            Child, (IPTR)ollabel1(Msg_NtRaiseActive),
                            Child, (IPTR)HSpace(0),
                        End,
                        Child, (IPTR)HVSpace,
                    End,
                End, // <Settings

            End, // <Register

            Child, CrawlingObject, // >Crawling
                MUIA_FixHeightTxt, (IPTR)infotext1,
                TextFrame,
                MUIA_Background, (IPTR)"m1",

                Child, TextObject,
                    MUIA_Text_Copy,     FALSE,
                    MUIA_Text_PreParse, (IPTR)"\033c",
                    MUIA_Text_Contents, (IPTR)infotext1,

                End,

                Child, TextObject,
                    MUIA_Text_Copy,     FALSE,
                    MUIA_Text_PreParse, (IPTR)"\033c",
                    MUIA_Text_Contents, (IPTR)infotext2,
                End,

                Child, ofhspace((IPTR)"a"),

                trans ? Child : TAG_IGNORE, trans,

                Child, TextObject,
                    MUIA_Text_Copy,     FALSE,
                    MUIA_Text_PreParse, (IPTR)"\033c",
                    MUIA_Text_Contents, (IPTR)infotext1,
                End,

            End, // <Crawling

        End))
        {
            set(data->scale,MUIA_Numeric_Format,"\33c%ld %%");

            DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)prefs);

            // check for MUI 3.9+
            if(lib_flags & BASEFLG_MUI20)
            {
                const char *at;

                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->groupBack,MUICFG_TheBar_GroupBack,2,(IPTR)tr(Msg_GroupBackground),MUIA_Imagedisplay_Spec);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->groupBack,MUICFG_TheBar_GroupBack,2,(IPTR)tr(Msg_GroupBackground),MUIA_Framedisplay_Spec);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->buttonBack,MUICFG_TheBar_ButtonBack,2,(IPTR)tr(Msg_ButtonBackground),MUIA_Imagedisplay_Spec);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->buttonBack,MUICFG_TheBar_ButtonBack,2,(IPTR)tr(Msg_ButtonBackground),MUIA_Framedisplay_Spec);
                if (!((lib_flags & BASEFLG_MUI4)))
                {
                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->frameShinePen,MUICFG_TheBar_FrameShinePen,1,(IPTR)tr(Msg_FrameShinePen));
                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->frameShadowPen,MUICFG_TheBar_FrameShadowPen,1,(IPTR)tr(Msg_FrameShadowPen));
                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->frameStyle,MUICFG_TheBar_FrameStyle,1,(IPTR)tr(Msg_FrameStyle));

                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->horizInnerSpacing,MUICFG_TheBar_HorizInnerSpacing,1,(IPTR)tr(Msg_HorizInnerSpacing));
                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->topInnerSpacing,MUICFG_TheBar_TopInnerSpacing,1,(IPTR)tr(Msg_TopInnerSpacing));
                    DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->bottomInnerSpacing,MUICFG_TheBar_BottomInnerSpacing,1,(IPTR)tr(Msg_BottomInnerSpacing));
                }
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->disBodyPen,MUICFG_TheBar_DisBodyPen,1,(IPTR)tr(Msg_DisabledBody));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->disShadowPen,MUICFG_TheBar_DisShadowPen,1,(IPTR)tr(Msg_DisabledShadow));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->barSpacerShinePen,MUICFG_TheBar_BarSpacerShinePen,1,(IPTR)tr(Msg_BarSpacerShinePen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->barSpacerShadowPen,MUICFG_TheBar_BarSpacerShadowPen,1,(IPTR)tr(Msg_BarSpacerShadowPen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->dragBarShinePen,MUICFG_TheBar_DragBarShinePen,1,(IPTR)tr(Msg_DragBarShine));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->dragBarShadowPen,MUICFG_TheBar_DragBarShadowPen,1,(IPTR)tr(Msg_DragBarShadow));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->dragBarFillPen,MUICFG_TheBar_DragBarFillPen,1,(IPTR)tr(Msg_DragBarFill));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->useDragBarFillPen,MUICFG_TheBar_UseDragBarFillPen,1,(IPTR)tr(Msg_UseDragBarFill));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->textFont,MUICFG_TheBar_TextFont,1,(IPTR)tr(Msg_TextFont));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->textGfxFont,MUICFG_TheBar_TextGfxFont,1,(IPTR)tr(Msg_TextGfxFont));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->textOverUseShine,MUICFG_TheBar_TextOverUseShine,1,(IPTR)tr(Msg_TextOverUseShine));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->viewMode,MUICFG_TheBar_Appearance,1,(IPTR)(at = tr(Msg_Reg_Appearance)));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->labelPos,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->borderless,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->raised,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->sunny,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->scaled,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->barSpacer,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->enableKeys,MUICFG_TheBar_Appearance,1,(IPTR)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->horizSpacing,MUICFG_TheBar_HorizSpacing,1,(IPTR)tr(Msg_HorizGroupSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->vertSpacing,MUICFG_TheBar_VertSpacing,1,(IPTR)tr(Msg_VertGroupSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->barSpacerSpacing,MUICFG_TheBar_BarSpacerSpacing,1,(IPTR)tr(Msg_BarSpacerSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->horizTexGfxSpacing,MUICFG_TheBar_HorizTextGfxSpacing,1,(IPTR)tr(Msg_HorizTextGfxSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->vertTexGfxSpacing,MUICFG_TheBar_VertTextGfxSpacing,1,(IPTR)tr(Msg_VertTextGfxSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->precision,MUICFG_TheBar_Precision,1,(IPTR)tr(Msg_Precision));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->scale,MUICFG_TheBar_Scale,1,(IPTR)tr(Msg_Scale));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->specialSelect,MUICFG_TheBar_SpecialSelect,1,(IPTR)tr(Msg_SpecialSelect));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->ignoreSel,MUICFG_TheBar_IgnoreSelImages,1,(IPTR)tr(Msg_IgnoreSelImages));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->ignoreDis,MUICFG_TheBar_IgnoreDisImages,1,(IPTR)tr(Msg_IgnoreDisImages));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->dontMove,MUICFG_TheBar_DontMove,1,(IPTR)tr(Msg_DontMove));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->ntRaiseActive,MUICFG_TheBar_NtRaiseActive,1,(IPTR)tr(Msg_NtRaiseActive));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->disMode,MUICFG_TheBar_DisMode,1,(IPTR)tr(Msg_DisMode));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(IPTR)data->spacersSize,MUICFG_TheBar_SpacersSize,1,(IPTR)tr(Msg_SpacersSize));
            }
            else
            {
              struct NewMenu menu[] =
              {
                { NM_TITLE, (STRPTR)"TheBar",               0, 0, 0, NULL               },
                { NM_ITEM,  (STRPTR)tr(Msg_Item_Globals),   0, 0, 0, (APTR)MI_GLOBALS   },
                { NM_ITEM,  (STRPTR)tr(Msg_Item_LastSaved), 0, 0, 0, (APTR)MI_LASTSAVED },
                { NM_ITEM,  (STRPTR)tr(Msg_Item_Restore),   0, 0, 0, (APTR)MI_RESTORE   },
                { NM_ITEM,  NM_BARLABEL,                    0, 0, 0, NULL               },
                { NM_ITEM,  (STRPTR)tr(Msg_Item_Defaults),  0, 0, 0, (APTR)MI_DEFAULTS  },
                { NM_END,   NULL,                           0, 0, 0, NULL               }
              };

              if((data->menu = MUI_MakeObject(MUIO_MenustripNM, (IPTR)menu, 0)))
                superset(cl,obj,MUIA_ContextMenu,data->menu);
            }
        }
        else
        {
            CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
            obj = 0;
        }
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    switch (msg->opg_AttrID)
    {
        case MUIA_Version:  *msg->opg_Storage = LIB_VERSION; return TRUE;
        case MUIA_Revision: *msg->opg_Storage = LIB_REVISION; return TRUE;
        default:            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static IPTR
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);

  if(data->menu)
    MUI_DisposeObject(data->menu);

  return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (data->menu && !(data->flags & FLG_MenuCheck))
    {
        IPTR storage;

        get(_app(obj),MUIA_Application_Base,&storage);
        data->base = (STRPTR)storage;
        if(stricmp(data->base, "MUI") == 0)
        {
            Object *o;

            if((o = (Object *)DoMethod(data->menu, MUIM_FindUData, Msg_Item_Globals)))
            {
                DoMethod(data->menu,MUIM_Family_Remove,(IPTR)o);
                MUI_DisposeObject(o);
            }

            data->flags |= FLG_Global;
        }

        data->flags |= FLG_MenuCheck;
    }

    return TRUE;
}

/***********************************************************************/

static struct MUIS_TheBar_Appearance staticAp = {MUIDEF_TheBar_Appearance_ViewMode,
                                                 MUIDEF_TheBar_Appearance_Flags,
                                                 0,
                                                 {0,0}};

static IPTR
mConfigToGadgets(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_ConfigToGadgets *msg)
{
    struct InstData               *data = INST_DATA(cl, obj );
    Object                        *cfg = msg->configdata;
    APTR                          ptr;
    struct MUIS_TheBar_Appearance *ap;
    ULONG                         v;
    ULONG                         *val;

    /* These are MUI version dependent */

    /* Group back */
    if(!(lib_flags & BASEFLG_MUI4) && !(lib_flags & BASEFLG_MUI20) && (ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Gradient)))
    {
        nnset(data->groupBack,MUIA_Popbackground_Grad,ptr);
    }
    else
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_GroupBack)))
            ptr = MUIDEF_TheBar_GroupBack;

        nnset(data->groupBack,MUIA_Imagedisplay_Spec,ptr);
    }

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseGroupBack)) ?
        *val : MUIDEF_TheBar_UseGroupBack;
    nnset(data->useGroupBack,MUIA_Selected,v);

    /* Group frame */
    if (lib_flags & BASEFLG_MUI20)
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Frame)))
            ptr = MUIDEF_TheBar_Frame;

        if (lib_flags & BASEFLG_MUI4)
		      nnset(data->groupBack,MUIA_Framedisplay_Spec,ptr);
        else
		      nnset(data->frame,MUIA_Framedisplay_Spec,ptr);
    }
    else
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarFrameShinePen)))
            ptr = MUIDEF_TheBar_BarFrameShinePen;
        nnset(data->barFrameShinePen,MUIA_Pendisplay_Spec,ptr);

        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarFrameShadowPen)))
            ptr = MUIDEF_TheBar_BarFrameShadowPen;
        nnset(data->barFrameShadowPen,MUIA_Pendisplay_Spec,ptr);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_LeftBarFrameSpacing)) ?
            *val : MUIDEF_TheBar_LeftBarFrameSpacing;
        nnset(data->leftBarFrameSpacing,MUIA_Numeric_Value,v);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_RightBarFrameSpacing)) ?
            *val : MUIDEF_TheBar_RightBarFrameSpacing;
        nnset(data->rightBarFrameSpacing,MUIA_Numeric_Value,v);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TopBarFrameSpacing)) ?
            *val : MUIDEF_TheBar_TopBarFrameSpacing;
        nnset(data->topBarFrameSpacing,MUIA_Numeric_Value,v);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BottomBarFrameSpacing)) ?
            *val : MUIDEF_TheBar_BottomBarFrameSpacing;
        nnset(data->bottomBarFrameSpacing,MUIA_Numeric_Value,v);
    }

    /* Button back */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_ButtonBack)))
        ptr = MUIDEF_TheBar_ButtonBack;
    nnset(data->buttonBack,MUIA_Imagedisplay_Spec,ptr);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseButtonBack)) ?
        *val : MUIDEF_TheBar_UseButtonBack;
    nnset(data->useButtonBack,MUIA_Selected,v);

    /* Button frame */
    if (lib_flags & BASEFLG_MUI4)
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_ButtonFrame)))
            ptr = MUIDEF_TheBar_ButtonFrame;
        nnset(data->buttonBack,MUIA_Framedisplay_Spec,ptr);
    }
    else
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameShinePen)))
            ptr = MUIDEF_TheBar_FrameShinePen;
        nnset(data->frameShinePen,MUIA_Pendisplay_Spec,ptr);

        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameShadowPen)))
            ptr = MUIDEF_TheBar_FrameShadowPen;
        nnset(data->frameShadowPen,MUIA_Pendisplay_Spec,ptr);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameStyle)) ?
            *val : MUIDEF_TheBar_FrameStyle;
        nnset(data->frameStyle,MUIA_Cycle_Active,v);
    }

    /* Buttons inner spacing */
    if (!(lib_flags & BASEFLG_MUI4))
    {
        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizInnerSpacing)) ?
            *val : MUIDEF_TheBar_HorizInnerSpacing;
        nnset(data->horizInnerSpacing,MUIA_Numeric_Value,v);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TopInnerSpacing)) ?
            *val : MUIDEF_TheBar_TopInnerSpacing;
        nnset(data->topInnerSpacing,MUIA_Numeric_Value,v);

        v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BottomInnerSpacing)) ?
            *val : MUIDEF_TheBar_BottomInnerSpacing;
        nnset(data->bottomInnerSpacing,MUIA_Numeric_Value,v);
    }

    /* Rest is MUI version indipendent */

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisBodyPen)))
        ptr = MUIDEF_TheBar_DisBodyPen;
    nnset(data->disBodyPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisShadowPen)))
        ptr = MUIDEF_TheBar_DisShadowPen;
    nnset(data->disShadowPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerShinePen)))
        ptr = MUIDEF_TheBar_BarSpacerShinePen;
    nnset(data->barSpacerShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerShadowPen)))
        ptr = MUIDEF_TheBar_BarSpacerShadowPen;
    nnset(data->barSpacerShadowPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarShinePen)))
        ptr = MUIDEF_TheBar_DragBarShinePen;
    nnset(data->dragBarShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarShadowPen)))
        ptr = MUIDEF_TheBar_DragBarShadowPen;
    nnset(data->dragBarShadowPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarFillPen)))
        ptr = MUIDEF_TheBar_DragBarFillPen;
    nnset(data->dragBarFillPen,MUIA_Pendisplay_Spec,ptr);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseDragBarFillPen)) ?
        *val : MUIDEF_TheBar_UseDragBarFillPen;
    nnset(data->useDragBarFillPen,MUIA_Selected,v);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextFont)))
        ptr = MUIDEF_TheBar_TextFont;
    nnset(data->textFont,MUIA_String_Contents,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextGfxFont)))
        ptr = MUIDEF_TheBar_TextGfxFont;
    nnset(data->textGfxFont,MUIA_String_Contents,ptr);

    if (!(ap = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Appearance)))
        ap = &staticAp;

    nnset(data->viewMode,MUIA_Cycle_Active,ap->viewMode);
    nnset(data->labelPos,MUIA_Cycle_Active,ap->labelPos);
    nnset(data->borderless,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Borderless);
    nnset(data->raised,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Raised);
    nnset(data->sunny,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Sunny);
    nnset(data->scaled,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Scaled);
    nnset(data->barSpacer,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_BarSpacer);
    nnset(data->enableKeys,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_EnableKeys);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizSpacing)) ?
        *val : MUIDEF_TheBar_HorizSpacing;
    nnset(data->horizSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_VertSpacing)) ?
        *val : MUIDEF_TheBar_VertSpacing;
    nnset(data->vertSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerSpacing)) ?
        *val : MUIDEF_TheBar_BarSpacerSpacing;
    nnset(data->barSpacerSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizTextGfxSpacing)) ?
        *val : MUIDEF_TheBar_HorizTextGfxSpacing;
    nnset(data->horizTexGfxSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_VertTextGfxSpacing)) ?
        *val : MUIDEF_TheBar_VertTextGfxSpacing;
    nnset(data->vertTexGfxSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Precision)) ?
        *val : MUIDEF_TheBar_Precision;
    nnset(data->precision,MUIA_Cycle_Active,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisMode)) ?
        *val : MUIDEF_TheBar_DisMode;
    nnset(data->disMode,MUIA_Cycle_Active,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Scale)) ?
        *val : MUIDEF_TheBar_Scale;
    nnset(data->scale,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_SpecialSelect)) ?
        *val : MUIDEF_TheBar_SpecialSelect;
    nnset(data->specialSelect,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextOverUseShine)) ?
        *val : MUIDEF_TheBar_TextOverUseShine;
    nnset(data->textOverUseShine,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_IgnoreSelImages)) ?
        *val : MUIDEF_TheBar_IgnoreSelImages;
    nnset(data->ignoreSel,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_IgnoreDisImages)) ?
        *val : MUIDEF_TheBar_IgnoreDisImages;
    nnset(data->ignoreDis,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DontMove)) ?
        *val : MUIDEF_TheBar_DontMove;
    nnset(data->dontMove,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_NtRaiseActive)) ?
        *val : MUIDEF_TheBar_NtRaiseActive;
    nnset(data->ntRaiseActive,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_SpacersSize)) ?
        *val : MUIDEF_TheBar_SpacersSize;
    nnset(data->spacersSize,MUIA_Cycle_Active,v);

    return 0;
}

/***********************************************************************/

static IPTR
mGadgetsToConfig(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_GadgetsToConfig *msg)
{
    struct InstData                *data = INST_DATA(cl,obj);
    Object                         *cfg = msg->configdata;
    struct MUIS_TheBar_Appearance  ap;
    STRPTR                         ptr;
    ULONG                          val;

    /* These are MUI version dependent */

    /* Group back */
    if (lib_flags & BASEFLG_MUI20)
    {
        get(data->groupBack,MUIA_Imagedisplay_Spec,&ptr);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_GroupBack);
    }
    else
    {
        if ((ptr = (STRPTR)xget(data->groupBack,MUIA_Popbackground_Grad)))
        {
            DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_GroupBack);
            addconfigitem(cfg,ptr,sizeof(struct MUIS_TheBar_Gradient),MUICFG_TheBar_Gradient);
        }
        else
        {
            DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_Gradient);
            ptr = (STRPTR)xget(data->groupBack,MUIA_Imagedisplay_Spec);
            addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_GroupBack);
        }
    }

    val = xget(data->useGroupBack, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseGroupBack);

    /* Group frame */
    if (lib_flags & BASEFLG_MUI20)
    {
        if (lib_flags & BASEFLG_MUI4) ptr = (STRPTR)xget(data->groupBack,MUIA_Framedisplay_Spec);
        else ptr = (STRPTR)xget(data->frame,MUIA_Framedisplay_Spec);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_Frame);
    }
    else
    {
        ptr = (STRPTR)xget(data->barFrameShinePen, MUIA_Pendisplay_Spec);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_BarFrameShinePen);

        ptr = (STRPTR)xget(data->barFrameShadowPen, MUIA_Pendisplay_Spec);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_BarFrameShadowPen);

        val = xget(data->frameStyle, MUIA_Cycle_Active);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_FrameStyle);

        val = xget(data->leftBarFrameSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_LeftBarFrameSpacing);

        val = xget(data->rightBarFrameSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_RightBarFrameSpacing);

        val = xget(data->topBarFrameSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_TopBarFrameSpacing);

        val = xget(data->bottomBarFrameSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_BottomBarFrameSpacing);
    }

    /* Button back */
    get(data->buttonBack,MUIA_Imagedisplay_Spec,&ptr);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_ButtonBack);

    val = xget(data->useButtonBack,MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseButtonBack);

    /* Button frame */
    if (lib_flags & BASEFLG_MUI4)
    {
        get(data->buttonBack,MUIA_Framedisplay_Spec,&ptr);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_ButtonFrame);
    }
    else
    {
        ptr = (STRPTR)xget(data->frameShinePen, MUIA_Pendisplay_Spec);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_FrameShinePen);

        ptr = (STRPTR)xget(data->frameShadowPen, MUIA_Pendisplay_Spec);
        addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_FrameShadowPen);

        val = xget(data->frameStyle, MUIA_Cycle_Active);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_FrameStyle);
    }

    /* Buttons inner spacing */
    if (!(lib_flags & BASEFLG_MUI4))
    {
        val = xget(data->horizInnerSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_HorizInnerSpacing);

        val = xget(data->topInnerSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_TopInnerSpacing);

        val = xget(data->bottomInnerSpacing, MUIA_Numeric_Value);
        addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_BottomInnerSpacing);
    }

    /* Rest is MUI version indipendent */

    ptr = (STRPTR)xget(data->disBodyPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_DisBodyPen);

    ptr = (STRPTR)xget(data->disShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_DisShadowPen);

    ptr = (STRPTR)xget(data->barSpacerShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_BarSpacerShinePen);

    ptr = (STRPTR)xget(data->barSpacerShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_BarSpacerShadowPen);

    ptr = (STRPTR)xget(data->dragBarShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_DragBarShinePen);

    ptr = (STRPTR)xget(data->dragBarShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_DragBarShadowPen);

    ptr = (STRPTR)xget(data->dragBarFillPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_DragBarFillPen);

    val = xget(data->useDragBarFillPen, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseDragBarFillPen);

    ptr = (STRPTR)xget(data->textFont, MUIA_String_Contents);
    if(*((STRPTR)ptr)) addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_TextFont);
    else DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_TextFont);

    ptr = (STRPTR)xget(data->textGfxFont, MUIA_String_Contents);
    if(*((STRPTR)ptr)) addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_TextGfxFont);
    else DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_TextGfxFont);

    ap.viewMode = xget(data->viewMode, MUIA_Cycle_Active);
    ap.labelPos = xget(data->labelPos, MUIA_Cycle_Active);
    ap.flags = 0;
    if(xget(data->borderless,MUIA_Selected)) ap.flags |= MUIV_TheBar_Appearance_Borderless;
    if(xget(data->raised,MUIA_Selected))     ap.flags |= MUIV_TheBar_Appearance_Raised;
    if(xget(data->sunny,MUIA_Selected))      ap.flags |= MUIV_TheBar_Appearance_Sunny;
    if(xget(data->scaled,MUIA_Selected))     ap.flags |= MUIV_TheBar_Appearance_Scaled;
    if(xget(data->barSpacer,MUIA_Selected))  ap.flags |= MUIV_TheBar_Appearance_BarSpacer;
    if(xget(data->enableKeys,MUIA_Selected)) ap.flags |= MUIV_TheBar_Appearance_EnableKeys;
    addconfigitem(cfg,&ap,sizeof(ap),MUICFG_TheBar_Appearance);

    val = xget(data->horizSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_HorizSpacing);

    val = xget(data->vertSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_VertSpacing);

    val = xget(data->barSpacerSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_BarSpacerSpacing);

    val = xget(data->horizTexGfxSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_HorizTextGfxSpacing);

    val = xget(data->vertTexGfxSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_VertTextGfxSpacing);

    val = xget(data->precision, MUIA_Cycle_Active);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_Precision);

    val = xget(data->disMode, MUIA_Cycle_Active);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_DisMode);

    val = xget(data->scale, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_Scale);

    val = xget(data->specialSelect, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_SpecialSelect);

    val = xget(data->textOverUseShine, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_TextOverUseShine);

    val = xget(data->ignoreSel, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_IgnoreSelImages);

    val = xget(data->ignoreDis, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_IgnoreDisImages);

    val = xget(data->dontMove, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_DontMove);

    val = xget(data->ntRaiseActive, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_NtRaiseActive);

    val = xget(data->spacersSize, MUIA_Cycle_Active);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_SpacersSize);

    return 0;
}

/***********************************************************************/

static ULONG
loadPrefs(Object *obj, const char *fileName)
{
    Object *cfg;
    ULONG  res = FALSE;

    if((cfg = MUI_NewObjectA((STRPTR)MUIC_Dataspace,NULL)))
    {
        struct IFFHandle *iffh;

        if((iffh = AllocIFF()))
        {
            if((iffh->iff_Stream = (IPTR)Open(fileName,MODE_OLDFILE)))
            {
                InitIFFasDOS(iffh);

                if (!OpenIFF(iffh,IFFF_READ))
                {
                    if (!StopChunk(iffh,ID_PREF,ID_PRHD) &&
                        !StopChunk(iffh,ID_PREF,MAKE_ID('M','U','I','C')) &&
                        !ParseIFF(iffh,IFFPARSE_SCAN))
                    {
                        struct ContextNode *cn;

                        if ((cn = CurrentChunk(iffh)) && (cn->cn_Type==ID_PREF) && (cn->cn_ID==ID_PRHD) && (cn->cn_Size==sizeof(struct PrefHeader)))
                        {
                            struct PrefHeader prhd;

                            if (ReadChunkBytes(iffh,&prhd,cn->cn_Size)==cn->cn_Size &&
                                !ParseIFF(iffh,IFFPARSE_SCAN) &&
                                !DoMethod(cfg,MUIM_Dataspace_ReadIFF,(IPTR)iffh))
                            {
                                DoMethod(obj,MUIM_Settingsgroup_ConfigToGadgets,(IPTR)cfg);
                                res = TRUE;
                            }
                        }
                    }

                    CloseIFF(iffh);
                }

                Close((BPTR)iffh->iff_Stream);
            }

            FreeIFF(iffh);
        }

        MUI_DisposeObject(cfg);
    }

    return res;
}

/**************************************************************************/

static IPTR
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    if (!data->menu) return 0;

    switch (muiUserData(msg->item))
    {
        case MI_DEFAULTS:
            set(data->groupBack,MUIA_Imagedisplay_Spec,MUIDEF_TheBar_GroupBack);
            set(data->useGroupBack,MUIA_Selected,MUIDEF_TheBar_UseGroupBack);
            set(data->buttonBack,MUIA_Imagedisplay_Spec,MUIDEF_TheBar_ButtonBack);
            set(data->useButtonBack,MUIA_Selected,MUIDEF_TheBar_UseButtonBack);
            set(data->frameShinePen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_FrameShinePen);
            set(data->frameShadowPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_FrameShadowPen);
            set(data->frameStyle,MUIA_Cycle_Active,MUIDEF_TheBar_FrameStyle);
            set(data->disBodyPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_DisBodyPen);
            set(data->disShadowPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_DisShadowPen);
            set(data->barSpacerShinePen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_BarSpacerShinePen);
            set(data->barSpacerShadowPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_BarSpacerShadowPen);
            #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
            set(data->barFrameShinePen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_BarFrameShinePen);
            set(data->barFrameShadowPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_BarFrameShadowPen);
            #endif
            set(data->dragBarShinePen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_DragBarShinePen);
            set(data->dragBarShadowPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_DragBarShadowPen);
            set(data->dragBarFillPen,MUIA_Pendisplay_Spec,MUIDEF_TheBar_DragBarFillPen);
            set(data->useDragBarFillPen,MUIA_Imagedisplay_Spec,MUIDEF_TheBar_UseDragBarFillPen);

            set(data->textFont,MUIA_String_Contents,MUIDEF_TheBar_TextFont);
            set(data->textGfxFont,MUIA_String_Contents,MUIDEF_TheBar_TextGfxFont);
            set(data->viewMode,MUIA_Cycle_Active,MUIDEF_TheBar_Appearance_ViewMode);
            set(data->labelPos,MUIA_Cycle_Active,MUIDEF_TheBar_Appearance_LabelPos);
            set(data->scale,MUIA_Numeric_Value,MUIDEF_TheBar_Scale);
            set(data->borderless,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_Borderless);
            set(data->raised,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_Raised);
            set(data->sunny,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_Sunny);
            set(data->scaled,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_Scaled);
            set(data->barSpacer,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_BarSpacer);
            set(data->enableKeys,MUIA_Selected,MUIDEF_TheBar_Appearance_Flags & MUIV_TheBar_Appearance_EnableKeys);

            set(data->horizSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_HorizSpacing);
            set(data->vertSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_VertSpacing);
            set(data->barSpacerSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_BarSpacerSpacing);
            set(data->horizInnerSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_HorizInnerSpacing);
            set(data->topInnerSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_TopInnerSpacing);
            set(data->bottomInnerSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_BottomInnerSpacing);
            #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
            set(data->leftBarFrameSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_LeftBarFrameSpacing);
            set(data->rightBarFrameSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_RightBarFrameSpacing);
            set(data->topBarFrameSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_TopBarFrameSpacing);
            set(data->bottomBarFrameSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_BottomBarFrameSpacing);
            #endif
            set(data->horizTexGfxSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_HorizTextGfxSpacing);
            set(data->vertTexGfxSpacing,MUIA_Numeric_Value,MUIDEF_TheBar_VertTextGfxSpacing);

            set(data->precision,MUIA_Cycle_Active,MUIDEF_TheBar_Precision);
            set(data->disMode,MUIA_Cycle_Active,MUIDEF_TheBar_DisMode);
            set(data->specialSelect,MUIA_Selected,MUIDEF_TheBar_SpecialSelect);
            set(data->textOverUseShine,MUIA_Selected,MUIDEF_TheBar_TextOverUseShine);

            set(data->ignoreSel,MUIA_Selected,MUIDEF_TheBar_IgnoreSelImages);
            set(data->ignoreDis,MUIA_Selected,MUIDEF_TheBar_IgnoreDisImages);
            set(data->dontMove,MUIA_Selected,MUIDEF_TheBar_DontMove);
            set(data->ntRaiseActive,MUIA_Selected,MUIDEF_TheBar_NtRaiseActive);

            set(data->spacersSize,MUIA_Cycle_Active,MUIDEF_TheBar_SpacersSize);
            break;

        case MI_GLOBALS:
            loadPrefs(obj,"ENVARC:MUI/«Global».prefs");
            break;

        case MI_LASTSAVED:
            if (data->flags & FLG_Global) loadPrefs(obj,"ENVARC:MUI/«Global».prefs");
            else
            {
                char buf[256];

                strcpy(buf,"ENVARC:MUI");
                AddPart(buf,data->base,sizeof(buf)-6);
                strcat(buf,".prefs");
                loadPrefs(obj,buf);
            }
            break;

        case MI_RESTORE:
            if (data->flags & FLG_Global) loadPrefs(obj,"ENV:MUI/«Global».prefs");
            else
            {
                char buf[256];

                strcpy(buf,"ENV:MUI");
                AddPart(buf,data->base,sizeof(buf)-6);
                strcat(buf,".prefs");
                loadPrefs(obj,buf);
            }
            break;
    }

    return 0;
}

/***********************************************************************/

DISPATCHER(_DispatcherP)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                                return mNew(cl,obj,(APTR)msg);
    case OM_GET:                                return mGet(cl,obj,(APTR)msg);
    case OM_DISPOSE:                            return mDispose(cl,obj,(APTR)msg);
    case MUIM_Setup:                            return mSetup(cl,obj,(APTR)msg);
    case MUIM_Settingsgroup_ConfigToGadgets:    return mConfigToGadgets(cl,obj,(APTR)msg);
    case MUIM_Settingsgroup_GadgetsToConfig:    return mGadgetsToConfig(cl,obj,(APTR)msg);
    case MUIM_ContextMenuChoice:                return mContextMenuChoice(cl,obj,(APTR)msg);

    default:                                    return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/
