/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

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
#include "private.h"

#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include <libraries/asl.h>

#include <mui/muiundoc.h>

#include "locale.h"
#include "rev.h"

#include <stdio.h>

#include "SDI_hook.h"

/***********************************************************************/
/*
** alfie's prefs
*/
/*
#define ALFIE_ButtonBack            ((APTR)"0:135")
#define ALFIE_UseButtonBack         TRUE
#define ALFIE_FrameShinePen         ((APTR)"r08640864,9CEE9CEE,FFFFFFFF")
#define ALFIE_FrameShadowPen        ALFIE_FrameShinePen
#define ALFIE_DisBodyPen            ((APTR)"r78787878,78787878,78787878")
#define ALFIE_DisShadowPen          ((APTR)"rd0d0d0d0,d0d0d0d0,d0d0d0d0")
#define ALFIE_BarSpacerShinePen     ((APTR)"rd0d0d0d0,d0d0d0d0,d0d0d0d0")
#define ALFIE_BarSpacerShadowPen    ((APTR)"r50505050,50505050,50505050")
#define ALFIE_HorizSpacing          1
#define ALFIE_VertSpacing           1
#define ALFIE_HorizInnerSpacing     1
#define ALFIE_TopInnerSpacing       1
#define ALFIE_BottomInnerSpacing    1
#define ALFIE_HorizTextGfxSpacing   2
#define ALFIE_VertTextGfxSpacing    2
#define ALFIE_DisMode               MUIV_TheBar_DisMode_Grid
*/

/***********************************************************************/

enum MItems { MI_GLOBALS, MI_LASTSAVED, MI_RESTORE, MI_DEFAULTS };

enum
{
    FLG_MenuCheck = 1<<0,
    FLG_Global    = 1<<1,
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
    {
        struct InstData *data = INST_DATA(cl,obj);
        const char *t;
        char buf[128];
        Object *prefs;
        Object *trans;
        Object *groups[16];
        Object *hidden[16];

        static const char *regs[5];
        static const char *frames[3];
        static const char *precisions[5];
        static const char *dismodes[5];
        static const char *spacersSizes[4];
        static const char *viewModes[4];
        static const char *labelPoss[5];

        regs[0] = tr(Msg_Reg_Colors);
        regs[1] = tr(Msg_Reg_Appearance);
        regs[2] = tr(Msg_Reg_Spacing);
        regs[3] = tr(Msg_Reg_Options);
        regs[4] = NULL;

        frames[0] = tr(Msg_FrameStyle_Recessed);
        frames[1] = tr(Msg_FrameStyle_Normal);
        frames[2] = NULL;

        precisions[0] = tr(Msg_Precision_Gui);
        precisions[1] = tr(Msg_Precision_Icon);
        precisions[2] = tr(Msg_Precision_Image);
        precisions[3] = tr(Msg_Precision_Exact);
        precisions[4] = NULL;

        dismodes[0] = tr(Msg_DisMode_Shape);
        dismodes[1] = tr(Msg_DisMode_Grid);
        dismodes[2] = tr(Msg_DisMode_FullGrid);
        dismodes[3] = tr(Msg_DisMode_Sunny);
        dismodes[4] = NULL;

        spacersSizes[0] = tr(Msg_SpacersSize_Quarter);
        spacersSizes[1] = tr(Msg_SpacersSize_Half);
        spacersSizes[2] = tr(Msg_SpacersSize_One);
        spacersSizes[3] = NULL;

        viewModes[0] = tr(Msg_TextGfx);
        viewModes[1] = tr(Msg_Gfx);
        viewModes[2] = tr(Msg_Text);
        viewModes[3] = NULL;

        labelPoss[0] = tr(Msg_LabelPos_Bottom);
        labelPoss[1] = tr(Msg_LabelPos_Top);
        labelPoss[2] = tr(Msg_LabelPos_Right);
        labelPoss[3] = tr(Msg_LabelPos_Left);
    		labelPoss[4] = NULL;

        snprintf(buf, sizeof(buf), tr(Msg_Info_First), "\33bTheBar " LIB_REV_STRING "\33n (" LIB_DATE ")\33n");

        if((t = tr(Msg_Info_Translator)) && *t)
        {
            trans = VGroup,
                MUIA_InnerLeft,0,
                MUIA_InnerBottom,0,
                MUIA_InnerTop,0,
                MUIA_InnerRight,0,
                Child, TextObject,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, t,
                End,
                Child, ofhspace("a"),
            End;
        }
        else
          trans = NULL;

        if((prefs = VGroup,

            Child, RegisterGroup(regs), // Register

                MUIA_CycleChain, TRUE,

                Child, groups[0] = VGroup,
                    MUIA_Group_Columns, 2,

                    Child, VGroup, // >Backrounds
                        GroupFrameT(tr(Msg_Title_Backgrounds)),
                        Child, owspace(1),
                        Child, groups[1] = ColGroup(3),
                            Child, oflabel(Msg_GroupBackground),
                            Child, data->groupBack = (lib_flags & BASEFLG_MUI4) ? opopfri(Msg_GroupBackground,Msg_GroupBackground_Title,Msg_GroupBackground_Help) :
                                                                                  opopback(TRUE,Msg_GroupBackground,Msg_GroupBackground_Title,Msg_GroupBackground_Help),

                            Child, hidden[0] = VGroup,
                                Child, VSpace(0),
                                Child, HGroup,
                                    //Child, olabel1(Msg_UseGroupBackground),
                                    Child, data->useGroupBack = ocheck(Msg_UseGroupBackground, Msg_UseGroupBackground_Help),
                                End,
                                Child, VSpace(0),
                            End,
                            Child, oflabel(Msg_ButtonBackground),
                            Child, data->buttonBack = opopback(FALSE,Msg_ButtonBackground,Msg_ButtonBackground_Title,Msg_ButtonBackground_Help),
                            Child, hidden[1] = VGroup,
                                Child, VSpace(0),
                                Child, HGroup,
                                    //Child, olabel1(Msg_UseButtonBackground),
                                    Child, data->useButtonBack = ocheck(Msg_UseButtonBackground ,Msg_UseButtonBackground_Help),
                                End,
                                Child, VSpace(0),
                            End,
                        End,
                        Child, owspace(1),
                    End, // <Backgrounds

                    Child, VGroup, // >Buttons frame
                        GroupFrameT(tr(Msg_Title_Frame)),
                        Child, owspace(1),
                        Child, ColGroup(2),
                            Child, oflabel(Msg_FrameShinePen),
                            Child, HGroup,
                                Child, data->frameShinePen = opoppen(Msg_FrameShinePen,Msg_FrameShinePen_Title,Msg_FrameShinePen_Help),
                                Child, owspace(20),
                                Child, oflabel(Msg_FrameShadowPen),
                                Child, data->frameShadowPen = opoppen(Msg_FrameShadowPen,Msg_FrameShadowPen_Title,Msg_FrameShadowPen_Help),
                            End,
                            Child, olabel2(Msg_FrameStyle),
                            Child, data->frameStyle = ocycle(frames,Msg_FrameStyle,Msg_FrameStyle_Help),
                        End,
                        Child, owspace(1),
                    End, // <Buttons frame

                    Child, hidden[2] = VGroup, // >BarFrame
                        GroupFrameT(tr(Msg_Title_BarFrame)),
                        MUIA_Weight, 60,
                        Child, owspace(1),
                        Child, HGroup,
                            #if defined(__MORPHOS__) || defined(__amigaos4__)
                            Child, oflabel(Msg_Frame),
	                          Child, data->frame = opopframe(Msg_Frame,Msg_Frame_Title,Msg_Frame_Help),
                            #else
                            Child, oflabel(Msg_BarFrameShine),
                            Child, data->barFrameShinePen = opoppen(Msg_BarFrameShine,Msg_BarFrameShine_Title,Msg_BarFrameShine_Help),
                            Child, owspace(20),
                            Child, oflabel(Msg_BarFrameShadow),
                            Child, data->barFrameShadowPen = opoppen(Msg_BarFrameShadow,Msg_BarFrameShadow_Title,Msg_BarFrameShadow_Help),
                            #endif
                        End,
                        Child, owspace(1),
                    End, // <BarFrame

                    Child, VGroup, // >Grey
                        GroupFrameT(tr(Msg_Title_Disabled)),
                        MUIA_Weight, 60,
                        Child, owspace(1),
                        Child, HGroup,
                            Child, oflabel(Msg_DisabledBody),
                            Child, data->disBodyPen = opoppen(Msg_DisabledBody,Msg_DisabledBody_Title,Msg_DisabledBody_Help),
                            Child, owspace(20),
                            Child, oflabel(Msg_DisabledShadow),
                            Child, data->disShadowPen = opoppen(Msg_DisabledShadow,Msg_DisabledShadow_Title,Msg_DisabledShadow_Help),
                        End,
                        Child, owspace(1),
                    End, // <Grey

                    Child, VGroup, // >Bar spacer
                        GroupFrameT(tr(Msg_Title_BarSpacer)),
                        MUIA_Weight, 60,
                        Child, owspace(1),
                        Child, HGroup,
                            Child, oflabel(Msg_BarSpacerShinePen),
                            Child, data->barSpacerShinePen = opoppen(Msg_BarSpacerShinePen,Msg_BarSpacerShinePen_Title,Msg_BarSpacerShinePen_Help),
                            Child, owspace(20),
                            Child, oflabel(Msg_BarSpacerShadowPen),
                            Child, data->barSpacerShadowPen = opoppen(Msg_BarSpacerShadowPen,Msg_BarSpacerShadowPen_Title,Msg_BarSpacerShadowPen_Help),
                        End,
                        Child, owspace(1),
                    End, // <Bar spacer

                    Child, VGroup, // >DragBar
                        GroupFrameT(tr(Msg_Title_DragBar)),
                        MUIA_Weight, 60,
                        Child, owspace(1),
                        Child, HGroup,
                            Child, oflabel(Msg_DragBarShine),
                            Child, data->dragBarShinePen = opoppen(Msg_DragBarShine,Msg_DragBarShine_Title,Msg_DragBarShine_Help),
                            Child, owspace(20),
                            Child, oflabel(Msg_DragBarShadow),
                            Child, data->dragBarShadowPen = opoppen(Msg_DragBarShadow,Msg_DragBarShadow_Title,Msg_DragBarShadow_Help),
                        End,
                        Child, owspace(1),
                        Child, HGroup,
                            MUIA_Weight, 80,
                            Child, oflabel(Msg_DragBarFill),
                            Child, data->dragBarFillPen = opoppen(Msg_DragBarFill,Msg_DragBarFill_Title,Msg_DragBarFill_Help),
                            Child, VGroup,
                                Child, VSpace(0),
                                Child, HGroup,
                                    Child, olabel1(Msg_UseDragBarFill),
                                    Child, data->useDragBarFillPen = ocheck(Msg_UseDragBarFill, Msg_UseDragBarFill_Help),
                                End,
                                Child, VSpace(0),
                            End,
                        End,
                    End, // <DragBar

                End, // <Colors

                Child, VGroup, // >Appearance
                    Child, ColGroup(2),
                        Child, olabel2(Msg_TextFont),
                        Child, PopaslObject,
                            MUIA_ShortHelp,   tr(Msg_TextFont_Help),
                            MUIA_Popasl_Type, ASL_FontRequest,
                            MUIA_Popstring_String, data->textFont = StringObject,
                                StringFrame,
                                MUIA_CycleChain,  TRUE,
                                MUIA_ControlChar, GetKeyChar(tr(Msg_TextFont)),
                            End,
                            MUIA_Popstring_Button, opop(MUII_PopFont,Msg_TextFont_Help),
                        End,
                        Child, olabel2(Msg_TextGfxFont),
                        Child, PopaslObject,
                            MUIA_ShortHelp,   tr(Msg_TextGfxFont_Help),
                            MUIA_Popasl_Type, ASL_FontRequest,
                            MUIA_Popstring_String, data->textGfxFont = StringObject,
                                StringFrame,
                                MUIA_CycleChain,  TRUE,
                                MUIA_ControlChar, GetKeyChar(tr(Msg_TextGfxFont)),
                            End,
                            MUIA_Popstring_Button, opop(MUII_PopFont,Msg_TextGfxFont_Help),
                        End,
                        Child, olabel2(Msg_Scale),
                        Child, data->scale = oslider(Msg_Scale,Msg_Scale_Help,25,400),
                    End,

                    Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_Weight, 0, End,

                    /*Child, TextObject,
                    	//MUIA_Weight, 0,
                        TextFrame,
                    	MUIA_Background,    MUII_TextBack,
                    	MUIA_Text_Contents, tr(Msg_Appearance_Text),
                    End,*/

                    Child, HGroup,
                        Child, olabel2(Msg_ViewMode),
                        Child, data->viewMode = ocycle(viewModes,Msg_ViewMode,Msg_ViewMode_Help),
                        Child, olabel2(Msg_LabelPos),
                        Child, data->labelPos = ocycle(labelPoss,Msg_LabelPos,Msg_LabelPos_Help),
                    End,
                    Child, ScrollgroupObject,
                        MUIA_Scrollgroup_FreeHoriz, FALSE,
                        MUIA_Scrollgroup_Contents, VirtgroupObject,
                            MUIA_Frame, MUIV_Frame_Virtual,
                            Child, HGroup,
                                Child, ColGroup(2),
                                    Child, data->borderless = ocheck(Msg_Borderless,Msg_Borderless_Help),
                                    Child, ollabel1(Msg_Borderless),
                                    Child, data->raised = ocheck(Msg_Raised,Msg_Raised_Help),
                                    Child, ollabel1(Msg_Raised),
                                    Child, data->sunny = ocheck(Msg_Sunny,Msg_Sunny_Help),
                                    Child, ollabel1(Msg_Sunny),
                                    Child, data->scaled = ocheck(Msg_Scaled,Msg_Scaled_Help),
                                    Child, ollabel1(Msg_Scaled),
                                    Child, data->barSpacer = ocheck(Msg_BarSpacer,Msg_BarSpacer_Help),
                                    Child, ollabel1(Msg_BarSpacer),
                                    Child, data->enableKeys = ocheck(Msg_EnableKeys,Msg_EnableKeys_Help),
                                    Child, ollabel1(Msg_EnableKeys),
                                End,
                                Child, HSpace(0),
                            End,
                            Child, owspace(1),
                        End,
                    End,
                End, // <Appearance

                Child, ColGroup(2),  // >Spacing
                    Child, VGroup, // >Group spacing
                        GroupFrameT(tr(Msg_Title_GroupSpacing)),
                        Child, owspace(1),
                        Child, ColGroup(2),
                            Child, olabel2(Msg_HorizGroupSpacing),
                            Child, HGroup,
                                Child, data->horizSpacing = oslider(Msg_HorizGroupSpacing,Msg_HorizGroupSpacing_Help,0,16),
                                Child, olabel2(Msg_VertGroupSpacing),
                                Child, data->vertSpacing = oslider(Msg_VertGroupSpacing,Msg_VertGroupSpacing_Help,0,16),
                            End,
                            Child, olabel2(Msg_BarSpacerSpacing),
                            Child, data->barSpacerSpacing = oslider(Msg_BarSpacerSpacing,Msg_BarSpacerSpacing_Help,0,16),
                        End,
                        Child, owspace(1),
                    End, // <Group spacing

                    Child, VGroup, // >Inner spacing
                        GroupFrameT(tr(Msg_Title_InnerSpacing)),
                        Child, owspace(1),
                        Child, ColGroup(2),
                            Child, olabel2(Msg_HorizInnerSpacing),
                            Child, data->horizInnerSpacing = oslider(Msg_HorizInnerSpacing,Msg_HorizInnerSpacing_Help,1,16),
                            Child, olabel2(Msg_TopInnerSpacing),
                            Child, HGroup,
                                Child, data->topInnerSpacing = oslider(Msg_TopInnerSpacing,Msg_TopInnerSpacing_Help,1,16),
                                Child, olabel2(Msg_BottomInnerSpacing),
                                Child, data->bottomInnerSpacing = oslider(Msg_BottomInnerSpacing,Msg_BottomInnerSpacing_Help,1,16),
                            End,
                        End,
                        Child, owspace(1),
                    End, // <Inner spacing

                    #if !defined(__MORPHOS__) && !defined(__amigaos4__)
                    Child, VGroup, // >BarFrame spacing
                        GroupFrameT(tr(Msg_Title_BarFrameSpacing)),
                        Child, owspace(1),
                        Child, ColGroup(4),
                            Child, olabel2(Msg_LeftBarFrameSpacing),
                            Child, data->leftBarFrameSpacing = oslider(Msg_LeftBarFrameSpacing,Msg_LeftBarFrameSpacing_Help,1,16),
                            Child, olabel2(Msg_RightBarFrameSpacing),
                            Child, data->rightBarFrameSpacing = oslider(Msg_RightBarFrameSpacing,Msg_RightBarFrameSpacing_Help,1,16),
                            Child, olabel2(Msg_TopBarFrameSpacing),
                            Child, data->topBarFrameSpacing = oslider(Msg_TopBarFrameSpacing,Msg_TopBarFrameSpacing_Help,1,16),
                            Child, olabel2(Msg_BottomBarFrameSpacing),
                            Child, data->bottomBarFrameSpacing = oslider(Msg_BottomBarFrameSpacing,Msg_BottomBarFrameSpacing_Help,1,16),
                        End,
                        Child, owspace(1),
                    End, // <BarFrame spacing
                    #endif

                    Child, VGroup, // >Text/Gfx spacing
                        GroupFrameT(tr(Msg_Title_TextGfxSpacing)),
                        Child, owspace(1),
                        Child, ColGroup(2),
                            Child, olabel2(Msg_HorizTextGfxSpacing),
                            Child, data->horizTexGfxSpacing = oslider(Msg_HorizTextGfxSpacing,Msg_HorizTextGfxSpacing_Help,1,16),
                            Child, olabel2(Msg_VertTextGfxSpacing),
                            Child, data->vertTexGfxSpacing = oslider(Msg_VertTextGfxSpacing,Msg_VertTextGfxSpacing_Help,1,16),
                        End,
                        Child, owspace(1),
                    End, // <Text/Gfx spacing

                    #if defined(__MORPHOS__) || defined(__amigaos4__)
                    Child, HVSpace,
                    #endif

                End,  // <Spacing

                Child, VGroup, // >Settings
                    Child, ColGroup(4),
                        Child, olabel2(Msg_Precision),
                        Child, data->precision = ocycle(precisions,Msg_Precision,Msg_Precision_Help),
                        Child, olabel2(Msg_DisMode),
                        Child, data->disMode = ocycle(dismodes,Msg_DisMode,Msg_DisMode_Help),
                        Child, olabel2(Msg_SpacersSize),
                        Child, data->spacersSize = ocycle(spacersSizes,Msg_SpacersSize,Msg_SpacersSize_Help),
                    End,
                    Child, ScrollgroupObject,
                        MUIA_Scrollgroup_FreeHoriz, FALSE,
                        MUIA_Scrollgroup_Contents, VirtgroupObject,
                            MUIA_Frame, MUIV_Frame_Virtual,
                            Child, HGroup,
                                Child, ColGroup(2),
                                    Child, data->specialSelect = ocheck(Msg_SpecialSelect,Msg_SpecialSelect_Help),
                                    Child, ollabel1(Msg_SpecialSelect),
                                    Child, data->textOverUseShine = ocheck(Msg_TextOverUseShine,Msg_TextOverUseShine_Help),
                                    Child, ollabel1(Msg_TextOverUseShine),
                                    Child, data->dontMove = ocheck(Msg_DontMove,Msg_DontMove_Help),
                                    Child, ollabel1(Msg_DontMove),
                                    Child, data->ignoreSel = ocheck(Msg_IgnoreSelImages,Msg_IgnoreSelImages_Help),
                                    Child, ollabel1(Msg_IgnoreSelImages),
                                    Child, data->ignoreDis = ocheck(Msg_IgnoreDisImages,Msg_IgnoreDisImages_Help),
                                    Child, ollabel1(Msg_IgnoreDisImages),
                                    Child, data->ntRaiseActive = ocheck(Msg_NtRaiseActive,Msg_NtRaiseActive_Help),
                                    Child, ollabel1(Msg_NtRaiseActive),
                                End,
                                Child, HSpace(0),
                            End,
                            Child, owspace(1),
                        End,
                    End,
                End, // <Settings

            End, // <Register

            Child, CrawlingObject, // >Crawling
                MUIA_FixHeightTxt, "\n\n",
                TextFrame,
                MUIA_Background, "m1",

                Child, TextObject,
                	MUIA_Text_Copy,     TRUE,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, buf,

                End,

                Child, TextObject,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, tr(Msg_Info_Reserved),
                End,

                Child, ofhspace("a"),

                Child, TextObject,
                	MUIA_Text_Copy,     TRUE,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, tr(Msg_Info_Rest),
                End,

                Child, ofhspace("a"),

                trans ? Child : TAG_IGNORE, trans,

                Child, TextObject,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, buf,
                End,

            End, // <Crawling

        End))
        {
            set(data->scale,MUIA_Numeric_Format,"\33c%ld %%");

            DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)prefs);

            if (lib_flags & BASEFLG_MUI4)
            {
                const char *at;

                DoMethod(groups[1],OM_REMMEMBER,(ULONG)hidden[0]);
                DoMethod(groups[1],OM_REMMEMBER,(ULONG)hidden[1]);
                set(groups[1],MUIA_Group_Columns,2);

                DoMethod(groups[0],OM_REMMEMBER,(ULONG)hidden[2]);

                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->groupBack,MUICFG_TheBar_GroupBack,2,(ULONG)tr(Msg_GroupBackground),MUIA_Imagedisplay_Spec);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->groupBack,MUICFG_TheBar_GroupBack,2,(ULONG)tr(Msg_GroupBackground),MUIA_Framedisplay_Spec);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->buttonBack,MUICFG_TheBar_ButtonBack,1,(ULONG)tr(Msg_ButtonBackground));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->frameShinePen,MUICFG_TheBar_FrameShinePen,1,(ULONG)tr(Msg_FrameShinePen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->frameShadowPen,MUICFG_TheBar_FrameShadowPen,1,(ULONG)tr(Msg_FrameShadowPen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->frameStyle,MUICFG_TheBar_FrameStyle,1,(ULONG)tr(Msg_FrameStyle));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->disBodyPen,MUICFG_TheBar_DisBodyPen,1,(ULONG)tr(Msg_DisabledBody));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->disShadowPen,MUICFG_TheBar_DisShadowPen,1,(ULONG)tr(Msg_DisabledShadow));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->barSpacerShinePen,MUICFG_TheBar_BarSpacerShinePen,1,(ULONG)tr(Msg_BarSpacerShinePen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->barSpacerShadowPen,MUICFG_TheBar_BarSpacerShadowPen,1,(ULONG)tr(Msg_BarSpacerShadowPen));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->dragBarShinePen,MUICFG_TheBar_DragBarShinePen,1,(ULONG)tr(Msg_DragBarShine));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->dragBarShadowPen,MUICFG_TheBar_DragBarShadowPen,1,(ULONG)tr(Msg_DragBarShadow));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->dragBarFillPen,MUICFG_TheBar_DragBarFillPen,1,(ULONG)tr(Msg_DragBarFill));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->useDragBarFillPen,MUICFG_TheBar_UseDragBarFillPen,1,(ULONG)tr(Msg_UseDragBarFill));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->textFont,MUICFG_TheBar_TextFont,1,(ULONG)tr(Msg_TextFont));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->textGfxFont,MUICFG_TheBar_TextGfxFont,1,(ULONG)tr(Msg_TextGfxFont));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->textOverUseShine,MUICFG_TheBar_TextOverUseShine,1,(ULONG)tr(Msg_TextOverUseShine));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->viewMode,MUICFG_TheBar_Appearance,1,(ULONG)(at = tr(Msg_Reg_Appearance)));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->labelPos,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->borderless,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->raised,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->sunny,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->scaled,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->barSpacer,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->enableKeys,MUICFG_TheBar_Appearance,1,(ULONG)at);
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->horizSpacing,MUICFG_TheBar_HorizSpacing,1,(ULONG)tr(Msg_HorizGroupSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->vertSpacing,MUICFG_TheBar_VertSpacing,1,(ULONG)tr(Msg_VertGroupSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->barSpacerSpacing,MUICFG_TheBar_BarSpacerSpacing,1,(ULONG)tr(Msg_BarSpacerSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->horizInnerSpacing,MUICFG_TheBar_HorizInnerSpacing,1,(ULONG)tr(Msg_HorizInnerSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->topInnerSpacing,MUICFG_TheBar_TopInnerSpacing,1,(ULONG)tr(Msg_TopInnerSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->bottomInnerSpacing,MUICFG_TheBar_BottomInnerSpacing,1,(ULONG)tr(Msg_BottomInnerSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->horizTexGfxSpacing,MUICFG_TheBar_HorizTextGfxSpacing,1,(ULONG)tr(Msg_HorizTextGfxSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->vertTexGfxSpacing,MUICFG_TheBar_VertTextGfxSpacing,1,(ULONG)tr(Msg_VertTextGfxSpacing));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->precision,MUICFG_TheBar_Precision,1,(ULONG)tr(Msg_Precision));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->scale,MUICFG_TheBar_Scale,1,(ULONG)tr(Msg_Scale));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->specialSelect,MUICFG_TheBar_SpecialSelect,1,(ULONG)tr(Msg_SpecialSelect));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->ignoreSel,MUICFG_TheBar_IgnoreSelImages,1,(ULONG)tr(Msg_IgnoreSelImages));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->ignoreDis,MUICFG_TheBar_IgnoreDisImages,1,(ULONG)tr(Msg_IgnoreDisImages));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->dontMove,MUICFG_TheBar_DontMove,1,(ULONG)tr(Msg_DontMove));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->ntRaiseActive,MUICFG_TheBar_NtRaiseActive,1,(ULONG)tr(Msg_NtRaiseActive));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->disMode,MUICFG_TheBar_DisMode,1,(ULONG)tr(Msg_DisMode));
                DoSuperMethod(cl,obj,MUIM_Mccprefs_RegisterGadget,(ULONG)data->spacersSize,MUICFG_TheBar_SpacersSize,1,(ULONG)tr(Msg_SpacersSize));
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

              if((data->menu = MUI_MakeObject(MUIO_MenustripNM, (ULONG)menu, 0)))
                superset(cl,obj,MUIA_ContextMenu,data->menu);
            }
        }
        else
        {
            CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
            obj = 0;
        }
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
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

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    if (data->menu) MUI_DisposeObject(data->menu);
    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (data->menu && !(data->flags & FLG_MenuCheck))
    {
        get(_app(obj),MUIA_Application_Base,&data->base);
        if (!stricmp(data->base,"MUI"))
        {
            Object *o;

            if((o = (Object *)DoMethod(data->menu, MUIM_FindUData, Msg_Item_Globals)))
            {
                DoMethod(data->menu,MUIM_Family_Remove,(ULONG)o);
                MUI_DisposeObject(o);
            }

            data->flags |= FLG_Global;
        }

        data->flags |= FLG_MenuCheck;
    }

    return TRUE;
}

/***********************************************************************/

static struct MUIS_TheBar_Appearance staticAp = { MUIDEF_TheBar_Appearance_ViewMode,
                                                  MUIDEF_TheBar_Appearance_Flags,
                                                  0,
                                                  { 0, 0 }
                                                };

static ULONG
mConfigToGadgets(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_ConfigToGadgets *msg)
{
    struct InstData                   *data = INST_DATA(cl, obj );
    Object                        *cfg = msg->configdata;
    APTR                                   ptr;
    struct MUIS_TheBar_Appearance *ap;
    ULONG                         v;
    ULONG                                  *val;

    if (!(lib_flags & BASEFLG_MUI20) && (ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Gradient)))
        set(data->groupBack,MUIA_Popbackground_Grad,ptr);
    else
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_GroupBack)))
            ptr = MUIDEF_TheBar_GroupBack;
        set(data->groupBack,MUIA_Imagedisplay_Spec,ptr);
    }

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseGroupBack)) ?
        *val : MUIDEF_TheBar_UseGroupBack;
    set(data->useGroupBack,MUIA_Selected,v);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_ButtonBack)))
        ptr = MUIDEF_TheBar_ButtonBack;
    set(data->buttonBack,MUIA_Imagedisplay_Spec,ptr);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseButtonBack)) ?
        *val : MUIDEF_TheBar_UseButtonBack;
    set(data->useButtonBack,MUIA_Selected,v);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameShinePen)))
        ptr = MUIDEF_TheBar_FrameShinePen;
    set(data->frameShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameShadowPen)))
        ptr = MUIDEF_TheBar_FrameShadowPen;
    set(data->frameShadowPen,MUIA_Pendisplay_Spec,ptr);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_FrameStyle)) ?
        *val : MUIDEF_TheBar_FrameStyle;
    set(data->frameStyle,MUIA_Cycle_Active,v);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisBodyPen)))
        ptr = MUIDEF_TheBar_DisBodyPen;
    set(data->disBodyPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisShadowPen)))
        ptr = MUIDEF_TheBar_DisShadowPen;
    set(data->disShadowPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerShinePen)))
        ptr = MUIDEF_TheBar_BarSpacerShinePen;
    set(data->barSpacerShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerShadowPen)))
        ptr = MUIDEF_TheBar_BarSpacerShadowPen;
    set(data->barSpacerShadowPen,MUIA_Pendisplay_Spec,ptr);

    #if defined(__MORPHOS__) || defined(__amigaos4__)
    if (lib_flags & BASEFLG_MUI4)
    {
        if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Frame)))
            ptr = MUIDEF_TheBar_Frame;
        set(data->groupBack,MUIA_Framedisplay_Spec,ptr);
    }
    else set(data->frame,MUIA_Framedisplay_Spec,ptr);
    #else
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarFrameShinePen)))
        ptr = MUIDEF_TheBar_BarFrameShinePen;
    set(data->barFrameShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarFrameShadowPen)))
        ptr = MUIDEF_TheBar_BarFrameShadowPen;
    set(data->barFrameShadowPen,MUIA_Pendisplay_Spec,ptr);
    #endif

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarShinePen)))
        ptr = MUIDEF_TheBar_DragBarShinePen;
    set(data->dragBarShinePen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarShadowPen)))
        ptr = MUIDEF_TheBar_DragBarShadowPen;
    set(data->dragBarShadowPen,MUIA_Pendisplay_Spec,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DragBarFillPen)))
        ptr = MUIDEF_TheBar_DragBarFillPen;
    set(data->dragBarFillPen,MUIA_Pendisplay_Spec,ptr);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_UseDragBarFillPen)) ?
        *val : MUIDEF_TheBar_UseDragBarFillPen;
    set(data->useDragBarFillPen,MUIA_Selected,v);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextFont)))
        ptr = MUIDEF_TheBar_TextFont;
    set(data->textFont,MUIA_String_Contents,ptr);

    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextGfxFont)))
        ptr = MUIDEF_TheBar_TextGfxFont;
    set(data->textGfxFont,MUIA_String_Contents,ptr);

    if (!(ap = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Appearance)))
        ap = &staticAp;

    set(data->viewMode,MUIA_Cycle_Active,ap->viewMode);
    set(data->labelPos,MUIA_Cycle_Active,ap->labelPos);
    set(data->borderless,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Borderless);
    set(data->raised,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Raised);
    set(data->sunny,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Sunny);
    set(data->scaled,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_Scaled);
    set(data->barSpacer,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_BarSpacer);
    set(data->enableKeys,MUIA_Selected,ap->flags & MUIV_TheBar_Appearance_EnableKeys);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizSpacing)) ?
        *val : MUIDEF_TheBar_HorizSpacing;
    set(data->horizSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_VertSpacing)) ?
        *val : MUIDEF_TheBar_VertSpacing;
    set(data->vertSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BarSpacerSpacing)) ?
        *val : MUIDEF_TheBar_BarSpacerSpacing;
    set(data->barSpacerSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizInnerSpacing)) ?
        *val : MUIDEF_TheBar_HorizInnerSpacing;
    set(data->horizInnerSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TopInnerSpacing)) ?
        *val : MUIDEF_TheBar_TopInnerSpacing;
    set(data->topInnerSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BottomInnerSpacing)) ?
        *val : MUIDEF_TheBar_BottomInnerSpacing;
    set(data->bottomInnerSpacing,MUIA_Numeric_Value,v);

    #if !defined(__MORPHOS__) && !defined(__amigaos4__)
    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_LeftBarFrameSpacing)) ?
        *val : MUIDEF_TheBar_LeftBarFrameSpacing;
    set(data->leftBarFrameSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_RightBarFrameSpacing)) ?
        *val : MUIDEF_TheBar_RightBarFrameSpacing;
    set(data->rightBarFrameSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TopBarFrameSpacing)) ?
        *val : MUIDEF_TheBar_TopBarFrameSpacing;
    set(data->topBarFrameSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_BottomBarFrameSpacing)) ?
        *val : MUIDEF_TheBar_BottomBarFrameSpacing;
    set(data->bottomBarFrameSpacing,MUIA_Numeric_Value,v);
    #endif

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_HorizTextGfxSpacing)) ?
        *val : MUIDEF_TheBar_HorizTextGfxSpacing;
    set(data->horizTexGfxSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_VertTextGfxSpacing)) ?
        *val : MUIDEF_TheBar_VertTextGfxSpacing;
    set(data->vertTexGfxSpacing,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Precision)) ?
        *val : MUIDEF_TheBar_Precision;
    set(data->precision,MUIA_Cycle_Active,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DisMode)) ?
        *val : MUIDEF_TheBar_DisMode;
    set(data->disMode,MUIA_Cycle_Active,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_Scale)) ?
        *val : MUIDEF_TheBar_Scale;
    set(data->scale,MUIA_Numeric_Value,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_SpecialSelect)) ?
        *val : MUIDEF_TheBar_SpecialSelect;
    set(data->specialSelect,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_TextOverUseShine)) ?
        *val : MUIDEF_TheBar_TextOverUseShine;
    set(data->textOverUseShine,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_IgnoreSelImages)) ?
        *val : MUIDEF_TheBar_IgnoreSelImages;
    set(data->ignoreSel,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_IgnoreDisImages)) ?
        *val : MUIDEF_TheBar_IgnoreDisImages;
    set(data->ignoreDis,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_DontMove)) ?
        *val : MUIDEF_TheBar_DontMove;
    set(data->dontMove,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_NtRaiseActive)) ?
        *val : MUIDEF_TheBar_NtRaiseActive;
    set(data->ntRaiseActive,MUIA_Selected,v);

    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_TheBar_SpacersSize)) ?
        *val : MUIDEF_TheBar_SpacersSize;
    set(data->spacersSize,MUIA_Cycle_Active,v);

    return 0;
}

/***********************************************************************/

#define SIZEOF(ptr,spec) ((lib_flags & BASEFLG_MUI20) ? sizeof(struct spec) : strlen((STRPTR)ptr)+1)

static ULONG
mGadgetsToConfig(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_GadgetsToConfig *msg)
{
    struct InstData           *data = INST_DATA(cl,obj);
    Object                *cfg = msg->configdata;
    struct MUIS_TheBar_Appearance      ap;
    STRPTR                         ptr;
    ULONG                          val;

    if(!(lib_flags & BASEFLG_MUI20) && (ptr = (STRPTR)xget(data->groupBack, MUIA_Popbackground_Grad)))
    {
        DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_GroupBack);
        addconfigitem(cfg,ptr,sizeof(struct MUIS_TheBar_Gradient),MUICFG_TheBar_Gradient);
    }
    else
    {
        DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_Gradient);
        ptr = (STRPTR)xget(data->groupBack, MUIA_Imagedisplay_Spec);
        addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_ImageSpec),MUICFG_TheBar_GroupBack);
    }

    val = xget(data->useGroupBack, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseGroupBack);

    ptr = (STRPTR)xget(data->buttonBack, MUIA_Imagedisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_ImageSpec),MUICFG_TheBar_ButtonBack);

    val = xget(data->useButtonBack, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseButtonBack);

    ptr = (STRPTR)xget(data->frameShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_FrameShinePen);

    ptr = (STRPTR)xget(data->frameShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_FrameShadowPen);

    val = xget(data->frameStyle, MUIA_Cycle_Active);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_FrameStyle);

    ptr = (STRPTR)xget(data->disBodyPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_DisBodyPen);

    ptr = (STRPTR)xget(data->disShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_DisShadowPen);

    ptr = (STRPTR)xget(data->barSpacerShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_BarSpacerShinePen);

    ptr = (STRPTR)xget(data->barSpacerShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_BarSpacerShadowPen);

    #if defined(__MORPHOS__) || defined(__amigaos4__)
    if(lib_flags & BASEFLG_MUI4)
      ptr = (STRPTR)xget(data->groupBack, MUIA_Framedisplay_Spec);
    else
      ptr = (STRPTR)xget(data->frame, MUIA_Framedisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_FrameSpec),MUICFG_TheBar_Frame);
    #else

    ptr = (STRPTR)xget(data->barFrameShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_BarFrameShinePen);

    ptr = (STRPTR)xget(data->barFrameShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_BarFrameShadowPen);
    #endif

    ptr = (STRPTR)xget(data->dragBarShinePen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_DragBarShinePen);

    ptr = (STRPTR)xget(data->dragBarShadowPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_DragBarShadowPen);

    ptr = (STRPTR)xget(data->dragBarFillPen, MUIA_Pendisplay_Spec);
    addconfigitem(cfg,ptr,SIZEOF(ptr,MUI_PenSpec),MUICFG_TheBar_DragBarFillPen);

    val = xget(data->useDragBarFillPen, MUIA_Selected);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_UseDragBarFillPen);

    ptr = (STRPTR)xget(data->textFont, MUIA_String_Contents);
    if(*((STRPTR)ptr))
      addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_TextFont);
    else
      DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_TextFont);

    ptr = (STRPTR)xget(data->textGfxFont, MUIA_String_Contents);
    if(*((STRPTR)ptr))
      addconfigitem(cfg,ptr,strlen((STRPTR)ptr)+1,MUICFG_TheBar_TextGfxFont);
    else
      DoMethod(cfg,MUIM_Dataspace_Remove,MUICFG_TheBar_TextGfxFont);

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

    val = xget(data->horizInnerSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_HorizInnerSpacing);

    val = xget(data->topInnerSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_TopInnerSpacing);

    val = xget(data->bottomInnerSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_BottomInnerSpacing);

    val = xget(data->horizTexGfxSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_HorizTextGfxSpacing);

    val = xget(data->vertTexGfxSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_VertTextGfxSpacing);

    #if !defined(__MORPHOS__) && !defined(__amigaos4__)
    val = xget(data->leftBarFrameSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_LeftBarFrameSpacing);

    val = xget(data->rightBarFrameSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_RightBarFrameSpacing);

    val = xget(data->topBarFrameSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_TopBarFrameSpacing);

    val = xget(data->bottomBarFrameSpacing, MUIA_Numeric_Value);
    addconfigitem(cfg,&val,sizeof(val),MUICFG_TheBar_BottomBarFrameSpacing);
    #endif

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
            if((iffh->iff_Stream = Open(fileName,MODE_OLDFILE)))
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
                                !DoMethod(cfg,MUIM_Dataspace_ReadIFF,(ULONG)iffh))
                            {
                                DoMethod(obj,MUIM_Settingsgroup_ConfigToGadgets,(ULONG)cfg);
                                res = TRUE;
                            }
                        }
                    }

                    CloseIFF(iffh);
                }

                Close(iffh->iff_Stream);
            }

            FreeIFF(iffh);
        }

        MUI_DisposeObject(cfg);
    }

    return res;
}

/**************************************************************************/

static ULONG
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
            #if !defined(__MORPHOS__) && !defined(__amigaos4__)
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
            #if !defined(__MORPHOS__) && !defined(__amigaos4__)
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
