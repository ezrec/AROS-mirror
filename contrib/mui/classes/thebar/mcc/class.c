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

#include <mui/muiundoc.h>

#include "SDI_hook.h"
#include "Debug.h"

#include "rev.h"

/***********************************************************************/

static Object *
makeButton(struct Button *button,Object *obj,struct InstData *data)
{
    Object                   *o;
    struct MUIS_TheBar_Brush *brush, *sbrush, *dbrush;
    ULONG                    flags = data->flags, userFlags = data->userFlags, userFlags2 = data->userFlags2, bflags = button->flags, viewMode;
    struct TagItem           attrs[64];

    if (button->img==MUIV_TheBar_BarSpacer)
    {
        return spacerObject,
            MUIA_Group_Horiz,                                                                    flags & FLG_Horiz,
            (userFlags & UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
            MUIA_TheButton_TheBar, obj,
        End;
    }
    else if(button->img==MUIV_TheBar_ButtonSpacer)
    {
      return spacerObject,
               MUIA_Group_Horiz,                                                                    flags & FLG_Horiz,
               (flags & FLG_BarSpacer) ? TAG_IGNORE : MUIA_TheButton_Spacer,                        MUIV_TheButton_Spacer_Button,
               (userFlags & UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
               MUIA_TheButton_TheBar, obj,
      End;
    }
    else if(button->img==MUIV_TheBar_ImageSpacer)
    {
      if(!(data->flags & FLG_TextOnly) && data->brushes &&((LONG)data->spacer>=0) && (data->viewMode!=MUIV_TheBar_ViewMode_Text) &&
         (o = TheButtonObject,
               MUIA_TheButton_MinVer,                                       16,
               MUIA_Group_Horiz,                                            flags & FLG_Horiz,
               MUIA_TheButton_TheBar,                                       (ULONG)obj,
               MUIA_TheButton_NoClick,                                      TRUE,
               MUIA_TheButton_Image,                                        (ULONG)data->brushes[data->spacer],
               data->sbrushes ? MUIA_TheButton_SelImage : TAG_IGNORE,       (ULONG)(data->sbrushes ? data->sbrushes[data->spacer] : NULL),
               data->dbrushes ? MUIA_TheButton_DisImage : TAG_IGNORE,       (ULONG)(data->dbrushes ? data->dbrushes[data->spacer] : NULL),
               MUIA_TheButton_ViewMode,                                     MUIV_TheButton_ViewMode_Gfx,
               MUIA_TheButton_Borderless,                                   TRUE,
               MUIA_TheButton_Raised,                                       FALSE,
               MUIA_TheButton_Sunny,                                        flags & FLG_Sunny,
               MUIA_TheButton_Scaled,                                       flags & FLG_Scaled,
               MUIA_TheButton_Spacer,                                       MUIV_TheButton_Spacer_Image,
               (flags & FLG_FreeStrip) ? MUIA_TheButton_Strip : TAG_IGNORE, (ULONG)&data->strip,
          End))
      {
        return o;
      }
      else
      {
        return spacerObject,
                 MUIA_Group_Horiz,                                                                    flags & FLG_Horiz,
                 (userFlags & UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
                 MUIA_TheButton_TheBar, obj,
               End;
      }
    }

    if (flags & FLG_TextOnly)
    {
        viewMode = MUIV_TheBar_ViewMode_Text;
        brush = sbrush = dbrush = NULL;
    }
    else
    {
        viewMode = data->viewMode;

        if (button->img<data->nbr-1)
        {
            brush = data->brushes[button->img];

            if (data->sbrushes) sbrush = data->sbrushes[button->img];
            else sbrush = NULL;

            if (data->dbrushes) dbrush = data->dbrushes[button->img];
            else dbrush = NULL;
        }
        else brush = sbrush = dbrush = NULL;
    }


    #if defined(VIRTUAL)
    attrs[0].ti_Tag   = MUIA_TheButton_InVirtgroup;
    attrs[0].ti_Data  = TRUE;
    #else
    attrs[0].ti_Tag   = TAG_IGNORE;
    #endif

    attrs[1].ti_Tag   = MUIA_Disabled;
    attrs[1].ti_Data  = bflags & BFLG_Disabled;
    attrs[2].ti_Tag   = MUIA_Selected;
    attrs[2].ti_Data  = bflags & BFLG_Selected;
    attrs[3].ti_Tag   = MUIA_Group_Horiz;
    attrs[3].ti_Data  = flags & FLG_Horiz;
    attrs[4].ti_Tag   = MUIA_TheButton_MinVer;
    attrs[4].ti_Data  = 16;
    attrs[5].ti_Tag   = MUIA_TheButton_TheBar;
    attrs[5].ti_Data  = (ULONG)obj;
    attrs[6].ti_Tag   = MUIA_TheButton_Image;
    attrs[6].ti_Data  = (ULONG)brush;
    attrs[7].ti_Tag   = MUIA_TheButton_SelImage;
    attrs[7].ti_Data  = (ULONG)sbrush;
    attrs[8].ti_Tag   = MUIA_TheButton_DisImage;
    attrs[8].ti_Data  = (ULONG)dbrush;
    attrs[9].ti_Tag   = MUIA_TheButton_Label;
    attrs[9].ti_Data  = (ULONG)button->text;
    attrs[10].ti_Tag  = MUIA_TheButton_Immediate;
    attrs[10].ti_Data = bflags & BFLG_Immediate;
    attrs[11].ti_Tag  = MUIA_TheButton_Toggle;
    attrs[11].ti_Data = bflags & BFLG_Toggle;
    attrs[12].ti_Tag  = MUIA_TheButton_Borderless;
    attrs[12].ti_Data = flags & FLG_Borderless;
    attrs[13].ti_Tag  = MUIA_TheButton_Raised;
    attrs[13].ti_Data = flags & FLG_Raised;
    attrs[14].ti_Tag  = MUIA_TheButton_Sunny;
    attrs[14].ti_Data = flags & FLG_Sunny;
    attrs[15].ti_Tag  = MUIA_TheButton_Scaled;
    attrs[15].ti_Data = flags & FLG_Scaled;
    attrs[16].ti_Tag  = MUIA_TheButton_EnableKey;
    attrs[16].ti_Data = flags & FLG_EnableKeys;
    attrs[17].ti_Tag  = MUIA_TheButton_ViewMode;
    attrs[17].ti_Data = viewMode;
    attrs[18].ti_Tag  = MUIA_TheButton_LabelPos;
    attrs[18].ti_Data = data->labelPos;
    attrs[19].ti_Tag  = MUIA_TheButton_ID;
    attrs[19].ti_Data = button->ID;

    if (button->help)
    {
        attrs[20].ti_Tag  = MUIA_ShortHelp;
        attrs[20].ti_Data = (ULONG)button->help;
    }
    else attrs[20].ti_Tag = TAG_IGNORE;

    if (flags & FLG_FreeStrip)
    {
        attrs[21].ti_Tag  = MUIA_TheButton_Strip;
        attrs[21].ti_Data = (ULONG)&data->strip;
    }
    else attrs[21].ti_Tag = TAG_IGNORE;

    if (userFlags || userFlags2)
    {
        if (userFlags & UFLG_UserHorizTextGfxSpacing)
        {
            attrs[22].ti_Tag  = MUIA_TheButton_HorizTextGfxSpacing;
            attrs[22].ti_Data = data->horizTextGfxSpacing;
        }
        else attrs[22].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserVertTextGfxSpacing)
        {
            attrs[23].ti_Tag  = MUIA_TheButton_VertTextGfxSpacing;
            attrs[23].ti_Data = data->vertTextGfxSpacing;
        }
        else attrs[23].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserHorizInnerSpacing)
        {
            attrs[24].ti_Tag  = MUIA_TheButton_HorizInnerSpacing;
            attrs[24].ti_Data = data->horizInnerSpacing;
        }
        else attrs[24].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserTopInnerSpacing)
        {
            attrs[25].ti_Tag  = MUIA_TheButton_TopInnerSpacing;
            attrs[25].ti_Data = data->topInnerSpacing;
        }
        else attrs[25].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserBottomInnerSpacing)
        {
            attrs[26].ti_Tag  = MUIA_TheButton_BottomInnerSpacing;
            attrs[26].ti_Data = data->bottomInnerSpacing;
        }
        else attrs[26].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserPrecision)
        {
            attrs[27].ti_Tag  = MUIA_TheButton_Precision;
            attrs[27].ti_Data = data->precision;
        }
        else attrs[27].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserDisMode)
        {
            attrs[28].ti_Tag  = MUIA_TheButton_DisMode;
            attrs[28].ti_Data = data->disMode;
        }
        else attrs[28].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserScale)
        {
            attrs[29].ti_Tag  = MUIA_TheButton_Scale;
            attrs[29].ti_Data = data->scale;
        }
        else attrs[29].ti_Tag = TAG_IGNORE;

        attrs[30].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserSpecialSelect)
        {
            attrs[31].ti_Tag  = MUIA_TheButton_SpecialSelect;
            attrs[31].ti_Data = userFlags & UFLG_SpecialSelect;
        }
        else attrs[31].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserTextOverUseShine)
        {
            attrs[32].ti_Tag  = MUIA_TheButton_TextOverUseShine;
            attrs[32].ti_Data = userFlags & UFLG_TextOverUseShine;
        }
        else attrs[32].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserIgnoreSelImages)
        {
            attrs[33].ti_Tag  = MUIA_TheButton_IgnoreSelImages;
            attrs[33].ti_Data = userFlags & UFLG_IgnoreSelImages;
        }
        else attrs[33].ti_Tag = TAG_IGNORE;

        if (userFlags & UFLG_UserIgnoreDisImages)
        {
            attrs[34].ti_Tag  = MUIA_TheButton_IgnoreDisImages;
            attrs[34].ti_Data = userFlags & UFLG_IgnoreDisImages;
        }
        else attrs[34].ti_Tag = TAG_IGNORE;

        if (userFlags2 & UFLG2_UserDontMove)
        {
            attrs[35].ti_Tag  = MUIA_TheButton_DontMove;
            attrs[35].ti_Data = userFlags2 & UFLG2_DontMove;
        }
        else attrs[35].ti_Tag = TAG_IGNORE;

        if (userFlags2 & UFLG2_NtRaiseActive)
        {
            attrs[36].ti_Tag  = MUIA_TheButton_NtRaiseActive;
            attrs[36].ti_Data = userFlags2 & UFLG2_NtRaiseActive;
        }
        else attrs[36].ti_Tag = TAG_IGNORE;

        attrs[37].ti_Tag = TAG_DONE;
    }
    else attrs[22].ti_Tag = TAG_DONE;

    if(button->class)
      return NewObjectA(button->class,NULL,attrs);
    else
      return MUI_NewObjectA((STRPTR)MUIC_TheButton,attrs);
}


/***********************************************************************/

static ULONG
orderButtons(struct IClass *cl,Object *obj,struct InstData *data)
{
    struct MUIP_Group_Sort *smsg;
    struct Button          *button, *succ;
    Object                 **o;
    ULONG                  n;

    ENTER();

    for(n = 0, button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (!(button->flags & (BFLG_Sleep|BFLG_Hide))) n++;

    if (data->db) n++;

    if (n>STATICSORTSIZE) smsg = allocVecPooled(data->pool,sizeof(struct MUIP_Group_Sort)+sizeof(Object *)*(n+1));
    else smsg = (struct MUIP_Group_Sort *)&data->sortMsgID;

    if(!smsg)
    {
      RETURN(FALSE);
      return FALSE;
    }

    if (data->db)
    {
        smsg->obj[0] = data->db;
        o = smsg->obj+1;
    }
    else o = smsg->obj;

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (!(button->flags & (BFLG_Sleep|BFLG_Hide))) *o++ = button->obj;

    *o = NULL;

    DoSuperMethodA(cl,obj,(Msg)smsg);

    if (n>STATICSORTSIZE) freeVecPooled(data->pool,smsg);

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

HOOKPROTONH(LayoutFunc, ULONG, Object *obj, struct MUI_LayoutMsg *lm)
{
    struct InstData *data = INST_DATA(ThisClass->mcc_Class,obj);

    ENTER();

    switch (lm->lm_Type)
    {
        case MUILM_MINMAX:
        {
            Object *child;
            Object          *cstate;
            ULONG  horiz = data->flags & (FLG_Horiz|FLG_Table), test;
            LONG   cols = 0, rows = 0, numBut, c;
            UWORD  butMaxMinWidth, butMaxMinHeight, maxMinWidth, maxMinHeight, width, height, addSpace;

            butMaxMinWidth  = butMaxMinHeight = numBut = 0;

            for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
            {
                if (!xget(child,MUIA_ShowMe) || (xget(child,MUIA_TheButton_Spacer)!=MUIV_TheButton_Spacer_None))
                    continue;

                if ((butMaxMinWidth<MUI_MAXMAX) && (_minwidth(child)>butMaxMinWidth))
                    butMaxMinWidth = _minwidth(child);

                if ((butMaxMinHeight<MUI_MAXMAX) && (_minheight(child)>butMaxMinHeight))
                    butMaxMinHeight = _minheight(child);

                numBut++;
            }

            data->buttonWidth  = butMaxMinWidth;
            data->buttonHeight = butMaxMinHeight;

            if (data->flags & FLG_Table)
            {
                if (data->cols)
                {
                    cols = data->cols;
                    if (cols<=0 || cols>numBut) cols = numBut;
                    rows = numBut/cols;
                    if (rows==0) rows = 1;
                    else if (numBut-cols*rows>0) rows++;
                }
                else
                {
                    rows = data->rows;
                    if (rows<=0 || rows>numBut) rows = numBut;
                    cols = numBut/rows;
                    if (cols==0) cols = 1;
                    else if (numBut-cols*rows>0) cols++;
                }
            }

            //kprintf("butMaxMinWidth:%ld butMaxMinHeight:%ld - but:%ld cols:%ld rows:%ld\n",butMaxMinWidth,butMaxMinHeight,numBut,cols,rows);

            maxMinWidth = maxMinHeight = width = height = addSpace = 0;
            c = 0;
            test = FALSE;

            for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
            {
                UWORD w = 0;

                if (!xget(child,MUIA_ShowMe)) continue;

                if (horiz)
                {
                    switch (xget(child,MUIA_TheButton_Spacer))
                    {
                        case MUIV_TheButton_Spacer_None:
                            w = butMaxMinWidth;
                            addSpace++;
                            break;

                        case MUIV_TheButton_Spacer_Bar:
                            w = _minwidth(child);
                            addSpace = 0;
                            break;

                        case MUIV_TheButton_Spacer_Button:
                            w = butMaxMinWidth;
                            switch (data->spacersSize)
                            {
                                case MUIV_TheBar_SpacersSize_Quarter:
                                 w /= 4;
                                 break;

                                case MUIV_TheBar_SpacersSize_Half:
                                 w /= 2;
                                 break;

                                default:
                                    break;
                            }
                            addSpace = 0;
                            break;

                        case MUIV_TheButton_Spacer_Image:
                            w = _minwidth(child);
                            addSpace = 0;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                            addSpace = 0;
                            break;
                    }

                    if (addSpace)
                    {
                        if (test)
                        {
                            if (width>maxMinWidth) maxMinWidth = width;

                            width = 0;
                            test = FALSE;
                            c = 0;
                            addSpace = 0;
                        }

                        if (addSpace>1) width += data->horizSpacing;
                        width += w;

                        if ((data->flags & FLG_Table) && (++c>=cols)) test = TRUE;
                    }
                    else width += w;
                }
                else
                {
                    switch (xget(child,MUIA_TheButton_Spacer))
                    {
                        case MUIV_TheButton_Spacer_None:
                            height += butMaxMinHeight;
                            addSpace++;
                            break;

                        case MUIV_TheButton_Spacer_Bar:
                            height += _minheight(child);
                            addSpace = 0;
                            break;

                        case MUIV_TheButton_Spacer_Button:
                        {
                            UWORD w = butMaxMinHeight;

                            switch (data->spacersSize)
                            {
                                case MUIV_TheBar_SpacersSize_Quarter:
                                 w /= 4;
                                 break;

                                case MUIV_TheBar_SpacersSize_Half:
                                 w /= 2;
                                 break;

                                default:
                                    break;
                            }
                            height += w;
                            addSpace = 0;
                            break;
                        }

                        case MUIV_TheButton_Spacer_Image:
                            height += _minheight(child);
                            addSpace = 0;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                            addSpace = 0;
                            break;
                    }

                    if (addSpace>1) height += data->vertSpacing;
                }
            }

            if (horiz)
            {
                if (data->flags & FLG_Table)
                {
                    if (maxMinWidth==0) maxMinWidth = width;

                    lm->lm_MinMax.MinWidth  = maxMinWidth;
                    lm->lm_MinMax.MinHeight = rows*(butMaxMinHeight+data->vertSpacing)-data->vertSpacing;

                    if(data->db)
                    {
                        if(data->cols)
                          lm->lm_MinMax.MinWidth += _minwidth(data->db)+data->horizSpacing;
                        else
                          lm->lm_MinMax.MinHeight += _minheight(data->db)+data->vertSpacing;

                        #ifndef __MORPHOS__
                        lm->lm_MinMax.MinHeight += (data->flags & FLG_Framed) ? 4 : 0;
                        #endif
                    }
                }
                else
                {
                    if (data->db)
                    {
                        width += _minwidth(data->db)+data->horizSpacing;
                        #ifndef __MORPHOS__
                        width += (data->flags & FLG_Framed) ? 4 : 0;
                        #endif
                    }

                    lm->lm_MinMax.MinWidth  = width;
                    lm->lm_MinMax.MinHeight = butMaxMinHeight;
                }

		        #ifndef __MORPHOS__
                if (data->flags & FLG_Framed)
                {
                    lm->lm_MinMax.MinWidth  += data->leftBarFrameSpacing+data->rightBarFrameSpacing+2;
                    lm->lm_MinMax.MinHeight += data->topBarFrameSpacing+data->bottomBarFrameSpacing+2;
                }
        		#endif

                lm->lm_MinMax.MaxWidth  = (data->flags & FLG_FreeHoriz) ? MUI_MAXMAX : lm->lm_MinMax.MinWidth;
                lm->lm_MinMax.MaxHeight = (data->flags & FLG_FreeVert) ? MUI_MAXMAX : lm->lm_MinMax.MinHeight;
            }
            else
            {
                if (data->db)
                {
                    height += _minheight(data->db)+data->vertSpacing;
                    #ifndef __MORPHOS__
                    height += (data->flags & FLG_Framed) ? 4 : 0;
                    #endif
                }

                lm->lm_MinMax.MinWidth  = butMaxMinWidth;
                lm->lm_MinMax.MinHeight = height;

                #ifndef __MORPHOS__
                if (data->flags & FLG_Framed)
                {
                    lm->lm_MinMax.MinWidth  += data->leftBarFrameSpacing+data->rightBarFrameSpacing+2;
                    lm->lm_MinMax.MinHeight += data->topBarFrameSpacing+data->bottomBarFrameSpacing+2;
                }
                #endif

                lm->lm_MinMax.MaxWidth  = (data->flags & FLG_FreeHoriz) ? MUI_MAXMAX : lm->lm_MinMax.MinWidth;
                lm->lm_MinMax.MaxHeight = (data->flags & FLG_FreeVert) ? MUI_MAXMAX : lm->lm_MinMax.MinHeight;
            }

            lm->lm_MinMax.DefWidth  = lm->lm_MinMax.MinWidth;
            lm->lm_MinMax.DefHeight = lm->lm_MinMax.MinHeight;

            data->width  = lm->lm_MinMax.MinWidth;
            data->height = lm->lm_MinMax.MinHeight;

            #if defined(VIRTUAL)
	            #if defined(__MORPHOS__)
              data->objWidth  = lm->lm_MinMax.MinWidth;
              data->objHeight = lm->lm_MinMax.MinHeight;
	            #else
    	        if(!(data->flags & FLG_Framed))
              {
                data->objWidth  = lm->lm_MinMax.MinWidth;
    	          data->objHeight = lm->lm_MinMax.MinHeight;
    	        }
	            #endif
	          #endif

            data->lcols = cols;
            data->lrows = rows;

            //kprintf("Table:%lx - Cols:%ld LCols:%ld Rows:%ld LRows:%ld\n",data->flags & FLG_Table,data->cols,cols,data->rows,rows);

            /*kprintf("MinMax mw:%ld dw:%ld MW:%ld - mh:%ld dh:%ld MH:%ld\n",
                lm->lm_MinMax.MinWidth,
                lm->lm_MinMax.DefWidth,
                lm->lm_MinMax.MaxWidth,
                lm->lm_MinMax.MinHeight,
                lm->lm_MinMax.DefHeight,
                lm->lm_MinMax.MaxHeight);*/

            RETURN(0);
            return 0;
        }

        case MUILM_LAYOUT:
        {
            ULONG horiz = data->flags & (FLG_Horiz|FLG_Table);

            #if defined(VIRTUAL)
            lm->lm_Layout.Width  = data->objWidth;
            lm->lm_Layout.Height = data->objHeight;
            #endif

            if (horiz)
            {
                Object *child;
                Object          *cstate;
                       int x, sx, sy = 0, cols = data->lcols, c = 0, r = 0;
                ULONG  spc = data->horizSpacing;

                switch (data->barPos)
                {
                    case MUIV_TheBar_BarPos_Center:
                        #if defined(VIRTUAL)
                        x = ((ULONG)_width(obj) > data->width) ? (_width(obj)-data->width)>>1 : 0;
                        #else
                        x = (_width(obj)-_minwidth(obj))>>1;
                        #endif
                        break;

                    case MUIV_TheBar_BarPos_Right:
                        #if defined(VIRTUAL)
                        x = ((ULONG)_mwidth(obj) > data->width) ? _mwidth(obj)-data->width : 0;
                        //x = (_width(obj)>data->width) ? _width(obj)-data->width : 0;
                        #else
                        x = _mwidth(obj)-_minwidth(obj);
                        #endif
                        break;

                    default:
                        x = 0;
                        break;
                }

                #ifndef __MORPHOS__
                if (data->flags & FLG_Framed) x += data->leftBarFrameSpacing+1;
                #endif

                sx = x;

                for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
                {
                    ULONG width = 0, height = 0, y, d = 0, test = TRUE; // gcc

                    if (!xget(child,MUIA_ShowMe)) continue;

                    switch (xget(child,MUIA_TheButton_Spacer))
                    {
                        case MUIV_TheButton_Spacer_None:
                            width  = data->buttonWidth;
                            height = data->buttonHeight;
                            d = spc;
                            break;

                        case MUIV_TheButton_Spacer_Bar:
                            width  = _minwidth(child);
                            height = data->buttonHeight-2;
                            if (x) x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_Button:
                            width  = data->buttonWidth;
                            switch (data->spacersSize)
                            {
                                case MUIV_TheBar_SpacersSize_Quarter:
                                 width /= 4;
                                 break;

                                case MUIV_TheBar_SpacersSize_Half:
                                 width /= 2;
                                 break;

                                default:
                                    break;
                            }
                            height = data->buttonHeight;
                            if (x) x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_Image:
                            width  = _minwidth(child);
                            height = _minheight(child);
                            if (x) x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                            if (data->cols)
                            {
                                ULONG xx;

                                width  = _minwidth(child);
                                height = _mheight(obj);
				
                                #ifdef __MORPHOS__
                                xx = y = 0;
                                #else
                                if (data->flags & FLG_Framed)
                                {
                                    height -= 4;
                                    xx = 2;
                                    y  = 2;
                                }
                                else xx = y = 0;
                                //height -= (data->flags & FLG_Framed) ? data->topBarFrameSpacing+data->bottomBarFrameSpacing+2 : 0;
                                #endif

                                if(!MUI_Layout(child,xx,y,width,height,0))
                                {
                                  RETURN(FALSE);
                                  return FALSE;
                                }

                                sx += width+spc;
                                x += width+spc;
                                continue;
                            }
                            else
                                if (data->rows)
                                {
                                    ULONG xx;

				                    width  = _mwidth(obj);
                                    height = _minheight(child);

                                    #ifdef __MORPHOS__
                                    xx = y = 0;
                                    #else
                                    if (data->flags & FLG_Framed)
                                    {
                                        width -= 4;
                                        xx = 2;
                                        y  = 2;
                                    }
                                    else xx = y = 0;
                                    //width  -= (data->flags & FLG_Framed) ? data->leftBarFrameSpacing+data->rightBarFrameSpacing+2 : 0;
                                    #endif
				
                                    if(!MUI_Layout(child,xx,y,width,height,0))
                                    {
                                      RETURN(FALSE);
                                      return FALSE;
                                    }

                                    sy = data->vertSpacing+height;
                                    continue;
                                }
                                else
                                {
                                    ULONG xx;

                                    width  = _minwidth(child);
                                    height = _mheight(obj);
    				
                                    #ifdef __MORPHOS__
                                    xx = y = 0;
                                    #else
                                    if (data->flags & FLG_Framed)
                                    {
                                        height -= 4;
                                        xx = 2;
                                        y  = 2;
                                    }
                                    else xx = y = 0;
                                    //height -= (data->flags & FLG_Framed) ? data->topBarFrameSpacing+data->bottomBarFrameSpacing+2 : 0;
                                    #endif

                                    if(!MUI_Layout(child,xx,y,width,height,0))
                                    {
                                      RETURN(FALSE);
                                      return FALSE;
                                    }

                                    sx += width+spc;
                                    x += width+spc;
                                    continue;
                                }
                    }

                    y = sy;

                    if (height<data->buttonHeight) y += (data->buttonHeight-height)>>1;

            	    #ifndef __MORPHOS__
                    if (data->flags & FLG_Framed) y += data->topBarFrameSpacing+1;
		            #endif

                    if (data->flags & FLG_Table)
                    {
                        if (test && (c++==cols))
                        {
                            c = 1;
                            r++;
                            x = sx;
                        }

                        y += r*(data->buttonHeight+data->vertSpacing);
                    }

                    //kprintf("Layout: %lx - %ld/%ld %ld/%ld - %ld %ld %ld %ld\n",child,c,cols,r,data->rows,x,y,width,height);
                    if(!MUI_Layout(child,x,y,width,height,0))
                    {
                      RETURN(FALSE);
                      return FALSE;
                    }

                    x += width+d;
                }
            }
            else
            {
                Object *child;
                Object          *cstate;
                int    y;
                ULONG  spc = data->vertSpacing;

                switch (data->barPos)
                {
                    case MUIV_TheBar_BarPos_Center:
                        #if defined(VIRTUAL)
                        y = ((ULONG)_height(obj) > data->height) ? (_height(obj)-data->height)>>1 : 0;
                        #else
                        y = (_height(obj)-_minheight(obj))>>1;
                        #endif
                        break;

                    case MUIV_TheBar_BarPos_Right:
                        #if defined(VIRTUAL)
                        y = ((ULONG)_height(obj) > data->height) ? _height(obj)-data->height : 0;
                        #else
                        y = _height(obj)-_minheight(obj);
                        #endif
                        break;

                    default:
                        y = 0;
                        break;
                }

        		#ifndef __MORPHOS__
                if (data->flags & FLG_Framed) y += data->topBarFrameSpacing+1;
                #endif

                for (cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
                {
                    ULONG width = 0, height = 0, x, d = 0; //gcc

                    if (!xget(child,MUIA_ShowMe)) continue;

                    switch (xget(child,MUIA_TheButton_Spacer))
                    {
                        case MUIV_TheButton_Spacer_None:
                            width  = data->buttonWidth;
                            height = data->buttonHeight;
                            d = spc;
                            break;

                        case MUIV_TheButton_Spacer_Bar:
                            width  = data->buttonWidth;
                            height = _minheight(child);
                            if (y) y -= spc;
                            break;

                        case MUIV_TheButton_Spacer_Button:
                            height = data->buttonHeight;
                            switch (data->spacersSize)
                            {
                                case MUIV_TheBar_SpacersSize_Quarter:
                                 height /= 4;
                                 break;

                                case MUIV_TheBar_SpacersSize_Half:
                                 height /= 2;
                                 break;

                                default:
                                    break;
                            }
                            width  = data->buttonWidth;
                            if (y) y -= spc;
                            break;

                        case MUIV_TheButton_Spacer_Image:
                            width  = _minwidth(child);
                            height = _minheight(child);
                            if (y) y -= spc;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                        {
                            ULONG yy;

                            height = _minheight(child);
                            width  = _mwidth(obj);

                            #ifdef __MORPHOS__
                            x = yy = 0;
                            #else
                            if (data->flags & FLG_Framed)
                            {
                              width -= 4;
                              x  = 2;
                              yy = 2;
                            }
                            else x = yy = 0;
                            //width -= (data->flags & FLG_Framed) ? data->leftBarFrameSpacing+data->rightBarFrameSpacing+2 : 0;
                            #endif

                            if(!MUI_Layout(child,x,yy,width,height,0))
                            {
                              RETURN(FALSE);
                              return FALSE;
                            }

                            y += height+d;
                            continue;
                        }
                    }

                    if (width<data->buttonWidth) x = (data->buttonWidth - _minwidth(child)) >> 1;
                    else x = 0;

        		    #ifndef __MORPHOS__
                    if (data->flags & FLG_Framed) x += data->leftBarFrameSpacing+1;
		            #endif

                    if(!MUI_Layout(child,x,y,width,height,0))
                    {
                      RETURN(FALSE);
                      return FALSE;
                    }

                    y += height+d;
                }
            }

            RETURN(TRUE);
            return TRUE;
        }
    }

    RETURN(MUILM_UNKNOWN);
    return MUILM_UNKNOWN;
}
MakeStaticHook(LayoutHook, LayoutFunc);

/***********************************************************************/

static ULONG
loadDTBrush(APTR pool,struct MUIS_TheBar_Brush *brush,STRPTR file)
{
    Object *dto;
    ULONG  res = FALSE;

    ENTER();

    memset(brush,0,sizeof(struct MUIS_TheBar_Brush));

    if((dto = NewDTObject(file,DTA_GroupID,    GID_PICTURE,
                               DTA_SourceType, DTST_FILE,
                               PDTA_Remap,     FALSE,
                               PDTA_DestMode,  PMODE_V43,
                               TAG_DONE)))
    {
        struct BitMapHeader *bmh;
        ULONG               *colors, numColors;

        if (GetDTAttrs(dto,PDTA_CRegs,(ULONG)&colors,PDTA_NumColors,(ULONG)&numColors,PDTA_BitMapHeader,(ULONG)&bmh,TAG_DONE)==3)
        {
            UBYTE *chunky;
            ULONG width, height, cdepth, tw, size, csize;

            width  = bmh->bmh_Width;
            height = bmh->bmh_Height;
            cdepth = bmh->bmh_Depth>8;

            tw    = ((width+15)>>4)<<4;
            size  = tw*height;
            csize = 0;

            if (cdepth) size += size+size+size;
            else
            {
                if (colors && numColors)
                {
                    csize  = numColors*sizeof(ULONG);
                    csize += csize+csize;
                }
            }

            if((chunky = allocVecPooled(pool,size+csize)))
            {
                ULONG line8 = 0;

                if (cdepth)
                {
                  res = DoMethod(dto,PDTM_READPIXELARRAY,(ULONG)chunky,PBPAFMT_ARGB,width<<2,0,0,width,height);

                  #ifdef __MORPHOS__
                  res = TRUE;
                  #endif
        		    }
                else
                  res = DoMethod(dto,PDTM_READPIXELARRAY,(ULONG)chunky,PBPAFMT_LUT8,width,0,0,width,height);

                if (!res)
                {
                    if (DoDTMethod(dto,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE))
                    {
                        struct BitMap *bm;

                        if (GetDTAttrs(dto,PDTA_BitMap,(ULONG)&bm,TAG_DONE))
                        {
                            struct RastPort trp;

                            InitRastPort(&trp);

                            if((trp.BitMap = AllocBitMap(8*(((width+15)>>4)<<1),1,8,0,bm)))
                            {
                                struct RastPort rp;
                                UBYTE  *dest = chunky;
                                ULONG y;

                                InitRastPort(&rp);
                                rp.BitMap = bm;

                                for (y = 0; y<height; y++)
                                {
                                    ReadPixelLine8(&rp,0,y,tw,dest,&trp);
                                    dest += tw;
                                }

                                WaitBlit();
                                FreeBitMap(trp.BitMap);

                                res = line8 = TRUE;
                            }
                        }
                    }
                }
                else line8 = FALSE;

                if (res)
                {
                    brush->data = chunky;

                    brush->dataWidth  = brush->width  = width;
                    brush->dataHeight = brush->height = height;

                    if (line8)
                    {
                        brush->dataTotalWidth = tw;

                        if (colors && numColors) memcpy(brush->colors = (ULONG *)(chunky+size),colors,csize);
                        brush->numColors = numColors;

                        if (bmh->bmh_Masking==mskHasTransparentColor) brush->trColor = bmh->bmh_Transparent;
                    }
                    else
                    {
                        brush->dataTotalWidth = width;

                        if (cdepth)
                        {
                            brush->flags = BRFLG_ARGB;
                            brush->dataTotalWidth *= 4;


                            if (bmh->bmh_Masking==mskHasTransparentColor)
                            {
                                ULONG tc = bmh->bmh_Transparent;

                                brush->trColor = ((colors[tc] & 0xFF000000)>>8) | ((colors[tc+1] & 0xFF000000)>>16) | ((colors[tc+2] & 0xFF000000)>>24);
                            }
                            #ifdef __MORPHOS__
                            else brush->flags |= BRFLG_AlphaMask;
                            #else
                            {
                                APTR p = NULL;

                                if (GetDTAttrs(dto,PDTA_MaskPlane,(ULONG)&p,TAG_DONE) && p)
                                    brush->flags |= BRFLG_AlphaMask;
                            }
                            #endif

                        }
                        else
                        {
                            if (colors && numColors) memcpy(brush->colors = (ULONG *)(chunky+size),colors,csize);
                            brush->numColors = numColors;

                            if (bmh->bmh_Masking==mskHasTransparentColor) brush->trColor = bmh->bmh_Transparent;
                        }
                    }
                }
                else freeVecPooled(pool,chunky);
            }
        }

        DisposeDTObject(dto);
    }

    RETURN(res);
    return res;
}

/***********************************************************************/

static struct Button *
findButton(struct InstData *data, ULONG ID)
{
  struct MinNode *node;
  struct Button *result = NULL;

  ENTER();

  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct Button *button = (struct Button *)node;

    if(button->ID == ID)
    {
      result = button;
      break;
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static void
removeButton(struct IClass *cl, Object *obj, struct Button *button)
{
  struct InstData *data = INST_DATA(cl,obj);
  APTR pool = data->pool;

  ENTER();

  D(DBF_STARTUP, "freeing button %d: %08lx", button->ID, button);

  // remove the MUI object as well
  if(button->obj)
  {
    D(DBF_STARTUP, " freeing button's MUI object...");

    // if the button is not hided it might still be a member of
    // the thebar object hierarchy
    if(!(button->flags & BFLG_Hide))
      DoSuperMethod(cl, obj, OM_REMMEMBER, (ULONG)button->obj);

    MUI_DisposeObject(button->obj);
    button->obj = NULL;
  }

  // cleanup the notifyListClone
  if(IsMinListEmpty(&button->notifyListClone) == FALSE)
  {
    struct MinNode *notifyNode;

    D(DBF_STARTUP, " freeing notifyListClone...");

    for(notifyNode = button->notifyListClone.mlh_Head; notifyNode->mln_Succ; )
    {
      struct ButtonNotify *notify = (struct ButtonNotify *)notifyNode;

      // go on in the list in advance
      notifyNode = notifyNode->mln_Succ;

      // remove from list and free
      Remove((struct Node *)notify);
      freeVecPooled(pool, notify);
    }
  }

  // remove the button from our list
  Remove((struct Node *)button);

  // free the buttons' occupied memory
  FreePooled(pool, button, sizeof(struct Button));

  LEAVE();
}

/***********************************************************************/

static void
updateRemove(struct IClass *cl,Object *obj,struct InstData *data)
{
    struct Button *button, *succ;

    ENTER();

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        ULONG sp;

        if (button->flags & BFLG_Sleep) continue;

        sp = xget(button->obj,MUIA_TheButton_Spacer);

        if (((data->remove & MUIV_TheBar_RemoveSpacers_Bar) && (sp==MUIV_TheButton_Spacer_Bar)) ||
            ((data->remove & MUIV_TheBar_RemoveSpacers_Button) && (sp==MUIV_TheButton_Spacer_Button)) ||
            ((data->remove & MUIV_TheBar_RemoveSpacers_Image) && (sp==MUIV_TheButton_Spacer_Image)))
        {
            if (!(button->flags & BFLG_Hide))
            {
                DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)button->obj);
                button->flags |= BFLG_Hide|BFLG_TableHide;
            }
        }
        else
            if (button->flags & BFLG_TableHide)
            {
                button->flags &= ~(BFLG_Hide|BFLG_TableHide);
                DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)button->obj);

                if (!orderButtons(cl,obj,data))
                {
                    DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)button->obj);
                    button->flags |= BFLG_Hide;
                }
            }
    }

    LEAVE();
}

/***********************************************************************/

struct pack
{
    struct MUIS_TheBar_Brush    **brushes;
    struct MUIS_TheBar_Brush    **sbrushes;
    struct MUIS_TheBar_Brush    **dbrushes;
    struct MUIS_TheBar_Button   *buttons;
    STRPTR                      idrawer;

    ULONG                       spacer;
    ULONG                       viewMode;
    ULONG                       flags;
    ULONG                       flags2;
    ULONG                       userFlags;
    ULONG                       userFlags2;
    ULONG                       pflags;

    ULONG                       id;

    STRPTR                      *pics;
    STRPTR                      *spics;
    STRPTR                      *dpics;
    struct MUIS_TheBar_Brush    *stripBrush;
    struct MUIS_TheBar_Brush    *sstripBrush;
    struct MUIS_TheBar_Brush    *dstripBrush;
    STRPTR                      strip;
    STRPTR                      sstrip;
    STRPTR                      dstrip;

    ULONG                       labelPos;
    ULONG                       barPos;
    ULONG                       remove;

    LONG                        cols;
    LONG                        rows;

    LONG                        stripCols;
    LONG                        stripRows;
    LONG                        stripHSpace;
    LONG                        stripVSpace;

    ULONG                       horizSpacing;
    ULONG                       vertSpacing;
    ULONG                       leftBarFrameSpacing;
    ULONG                       rightBarFrameSpacing;
    ULONG                       topBarFrameSpacing;
    ULONG                       bottomBarFrameSpacing;
    ULONG                       barSpacerSpacing;
    ULONG                       horizInnerSpacing;
    ULONG                       topInnerSpacing;
    ULONG                       bottomInnerSpacing;
    ULONG                       horizTextGfxSpacing;
    ULONG                       vertTextGfxSpacing;

    ULONG                       precision;
    ULONG                       scale;
    ULONG                       disMode;
    ULONG                       spacersSize;
    #ifdef __MORPHOS__
    ULONG                       userFrame;
    #endif
};

enum
{
    PFLG_FreeHorizExists = 1<<0,
    PFLG_FreeVertExists  = 1<<1,
    PFLG_FreeHoriz       = 1<<2,
    PFLG_FreeVert        = 1<<3,
};

ULONG ptable[] =
{
    PACK_STARTTABLE(TBTAGBASE),

    /* Fields */
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Images,pack,brushes,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SelImages,pack,sbrushes,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_DisImages,pack,dbrushes,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Buttons,pack,buttons,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_PicsDrawer,pack,idrawer,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SpacerIndex,pack,spacer,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_ViewMode,pack,viewMode,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Pics,pack,pics,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SelPics,pack,spics,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_DisPics,pack,dpics,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Strip,pack,strip,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SelStrip,pack,sstrip,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_DisStrip,pack,dstrip,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_StripBrush,pack,stripBrush,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SelStripBrush,pack,sstripBrush,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_DisStripBrush,pack,dstripBrush,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_LabelPos,pack,labelPos,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_BarPos,pack,barPos,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Columns,pack,cols,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Rows,pack,rows,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_RemoveSpacers,pack,remove,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_StripCols,pack,stripCols,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_StripRows,pack,stripRows,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_StripHSpace,pack,stripHSpace,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_StripVSpace,pack,stripVSpace,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_MouseOver,pack,id,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_SpacersSize,pack,id,PKCTRL_LONG|PKCTRL_PACKONLY),

    /* Flags */
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Borderless,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Borderless),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Raised,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Raised),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Scaled,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Scaled),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Sunny,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Sunny),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_EnableKeys,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_EnableKeys),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_DragBar,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_DragBar),
    #ifndef __MORPHOS__
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Frame,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Framed),
    #endif
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_BarSpacer,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_BarSpacer),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_IgnoreAppearance,pack,flags2,PKCTRL_BIT|PKCTRL_PACKONLY,FLG2_IgnoreAppearance),

    /* Flags existance */
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Free,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY,(PFLG_FreeHoriz|PFLG_FreeVert)),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Free,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,(PFLG_FreeHorizExists|PFLG_FreeVertExists)),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_FreeHoriz,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY,PFLG_FreeHoriz),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_FreeHoriz,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY,PFLG_FreeHorizExists),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_FreeVert,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY,PFLG_FreeVert),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_FreeVert,pack,pflags,PKCTRL_BIT|PKCTRL_PACKONLY,PFLG_FreeVertExists),

    /* User fields */
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_HorizSpacing,pack,horizSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_VertSpacing,pack,vertSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_BarSpacerSpacing,pack,barSpacerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_HorizInnerSpacing,pack,horizInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_TopInnerSpacing,pack,topInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_BottomInnerSpacing,pack,bottomInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_LeftBarFrameSpacing,pack,leftBarFrameSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_RightBarFrameSpacing,pack,rightBarFrameSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_TopBarFrameSpacing,pack,topBarFrameSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_BottomBarFrameSpacing,pack,bottomBarFrameSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_HorizTextGfxSpacing,pack,horizTextGfxSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_VertTextGfxSpacing,pack,vertTextGfxSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Precision,pack,precision,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_Scale,pack,scale,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBTAGBASE,MUIA_TheBar_DisMode,pack,disMode,PKCTRL_LONG|PKCTRL_PACKONLY),

    /* User flags */
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_SpecialSelect,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_SpecialSelect),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_TextOverUseShine,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_TextOverUseShine),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_IgnoreSelImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_IgnoreSelImages),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_IgnoreDisImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_IgnoreDisImages),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_DontMove,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG2_DontMove),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_NtRaiseActive,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG2_NtRaiseActive),

    /* User flags existance */
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_HorizSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserHorizSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_VertSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserVertSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_BarSpacerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserBarSpacerSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_HorizInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserHorizInnerSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_TopInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserTopInnerSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_BottomInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserBottomInnerSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_LeftBarFrameSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserLeftBarFrameSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_RightBarFrameSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserRightBarFrameSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_TopBarFrameSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserTopBarFrameSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_BottomBarFrameSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserBottomBarFrameSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_HorizTextGfxSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserHorizTextGfxSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_VertTextGfxSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserVertTextGfxSpacing),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Precision,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserPrecision),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Scale,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserScale),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_SpecialSelect,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserSpecialSelect),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_TextOverUseShine,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserTextOverUseShine),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_IgnoreSelImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserIgnoreSelImages),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_IgnoreDisImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserIgnoreDisImages),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_DisMode,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserDisMode),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_DontMove,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG2_UserDontMove),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_NtRaiseActive,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG2_UserNtRaiseActive),
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_SpacersSize,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG2_UserSpacersSize),

    /* Alien: area */
    PACK_NEWOFFSET(MUIA_Background),
    PACK_LONGBIT(MUIA_Background,MUIA_Background,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Background),

    #ifdef __MORPHOS__
    PACK_NEWOFFSET(MUIA_Frame),
    PACK_LONGBIT(MUIA_Frame,MUIA_Frame,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG2_UserFrame),
    PACK_ENTRY(MUIA_Frame,MUIA_Frame,pack,userFrame,PKCTRL_LONG|PKCTRL_PACKONLY),
    #endif

    /* Alien: group */
    PACK_NEWOFFSET(MUIA_Group_Horiz),
    PACK_LONGBIT(MUIA_Group_Horiz,MUIA_Group_Horiz,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Horiz),

    PACK_ENDTABLE
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct pack                        pt;
    struct MUIS_TheBar_Button *buttons;
    APTR                      pool;
    struct TagItem            *attrs = msg->ops_AttrList;
    struct MUIS_TheBar_Brush           sb, ssb, dsb;
    ULONG                     pics;
    UWORD                     nbr = 0;

    ENTER();

    if ((GetTagData(MUIA_TheBar_MinVer,0,attrs) > LIB_VERSION) ||
        !(pool = CreatePool(MEMF_ANY,2048,1024)))
        return 0;

    memset(&pt,0,sizeof(pt));

    // set some default values before we actually see what the
    // user set during creation of that object
    pt.labelPos = MUIV_TheBar_LabelPos_Bottom;
    pt.barPos   = MUIV_TheBar_BarPos_Left;
    pt.spacer   = pt.stripCols = pt.stripRows = pt.stripHSpace = pt.stripVSpace = -1;
    pt.flags    = FLG_EnableKeys;

    PackStructureTags(&pt,ptable,attrs);

    if (pt.viewMode>=MUIV_TheBar_ViewMode_Last) pt.viewMode = MUIV_TheBar_ViewMode_Text;
    if (pt.labelPos>=MUIV_TheBar_LabelPos_Last) pt.labelPos = MUIV_TheBar_LabelPos_Bottom;

    if ((pt.pflags & PFLG_FreeHorizExists) ? (pt.flags & PFLG_FreeHoriz) : (pt.flags & FLG_Horiz))
        pt.flags |= FLG_FreeHoriz;

    if ((pt.pflags & PFLG_FreeVertExists) ? (pt.flags & PFLG_FreeVert) : (pt.flags & FLG_Horiz) == 0)
        pt.flags |= FLG_FreeVert;

    sb.data = ssb.data = dsb.data = NULL;

    if (!pt.brushes)
    {
        struct Process *me;
        struct Window  *win;
        BPTR           idrawer, odir = 0;

        pics = FALSE;

        me  = (struct Process *)FindTask(NULL);
        win = me->pr_WindowPtr;
        me->pr_WindowPtr = (struct Window *)-1;

        if(pt.idrawer != NULL && (idrawer = Lock(pt.idrawer, SHARED_LOCK)))
          odir = CurrentDir(idrawer);
        else
          idrawer = 0;

        if (pt.stripBrush || pt.strip)
        {
            struct MUIS_TheBar_Button *b;
            ULONG                     brpsize, brsize, totsize;

            if (pt.stripBrush)
            {
                sb = *pt.stripBrush;

                if (pt.sstripBrush) ssb = *pt.sstripBrush;
                if (pt.dstripBrush) dsb = *pt.dstripBrush;
            }
            else
            {
                if (loadDTBrush(pool,&sb,pt.strip))
                {
                    if (pt.sstrip) loadDTBrush(pool,&ssb,pt.sstrip);
                    if (pt.dstrip) loadDTBrush(pool,&dsb,pt.dstrip);
                }
            }

            if (sb.data)
            {
                if (ssb.data && (sb.dataWidth!=ssb.dataWidth || sb.dataHeight!=ssb.dataHeight))
                {
                    freeVecPooled(pool,ssb.data);
                    ssb.data = NULL;
                }

                if (dsb.data && (sb.dataWidth!=dsb.dataWidth || sb.dataHeight!=dsb.dataHeight))
                {
                    freeVecPooled(pool,dsb.data);
                    dsb.data = NULL;
                }

                if((b = pt.buttons))
                {
                    for (nbr = 0; b->img!=MUIV_TheBar_End; b++)
                        if (b->img!=MUIV_TheBar_BarSpacer) nbr++;
                }
                else nbr = 0;

                if (pt.stripCols<=0)  pt.stripCols   = nbr ? nbr : 1;
                if (pt.stripRows<=0)  pt.stripRows   = 1;
                if (pt.stripHSpace<0) pt.stripHSpace = 1;
                if (pt.stripVSpace<0) pt.stripVSpace = 1;
                nbr = pt.stripCols*pt.stripRows+1;

                brpsize = sizeof(struct MUIS_TheBar_Brush *)*nbr;
                brsize  = sizeof(struct MUIS_TheBar_Brush)*nbr;

                totsize = brpsize+brsize;
                if (ssb.data) totsize += brpsize+brsize;
                if (dsb.data) totsize += brpsize+brsize;

                if((pt.brushes = allocVecPooled(pool,totsize)))
                {
                    ULONG rows, cols, horizSpace, vertSpace;
                    int   w, h;

                    rows       = pt.stripRows;
                    cols       = pt.stripCols;
                    horizSpace = pt.stripHSpace;
                    vertSpace  = pt.stripVSpace;

                    w = (sb.dataWidth -(cols-1)*horizSpace)/cols;
                    h = (sb.dataHeight-(rows-1)*vertSpace)/rows;

                    if (w && h)
                    {
                        struct MUIS_TheBar_Brush *brush, *sbrush, *dbrush;
                        int x, vofs;
                        ULONG i;

                        brush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.brushes+brpsize);

                        if (ssb.data)
                        {
                            pt.sbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            sbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.sbrushes+brpsize);
                        }
                        else sbrush = NULL;

                        if (dsb.data)
                        {
                            if (ssb.data) pt.dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)sbrush+brsize);
                            else pt.dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            dbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.dbrushes+brpsize);
                        }
                        else dbrush = NULL;

                        for(x = i = vofs = 0; i<rows; i++, vofs += h+vertSpace)
                        {
                            ULONG j;
                            int hofs;

                            for (j = hofs = 0; j<cols; j++, hofs += w+horizSpace)
                            {
                                memcpy(pt.brushes[x] = brush,&sb,sizeof(struct MUIS_TheBar_Brush));
                                brush->left   = hofs;
                                brush->top    = vofs;
                                brush->width  = w;
                                brush->height = h;
                                brush++;

                                if (sbrush)
                                {
                                    memcpy(pt.sbrushes[x] = sbrush,&ssb,sizeof(struct MUIS_TheBar_Brush));
                                    sbrush->left   = hofs;
                                    sbrush->top    = vofs;
                                    sbrush->width  = w;
                                    sbrush->height = h;
                                    sbrush++;
                                }

                                if (dbrush)
                                {
                                    memcpy(pt.dbrushes[x] = dbrush,&dsb,sizeof(struct MUIS_TheBar_Brush));
                                    dbrush->left   = hofs;
                                    dbrush->top    = vofs;
                                    dbrush->width  = w;
                                    dbrush->height = h;
                                    dbrush++;
                                }

                                x++;
                            }
                        }

                        pt.brushes[x] = NULL;
                        if (sbrush) pt.sbrushes[x] = NULL;
                        if (dbrush) pt.dbrushes[x] = NULL;

                        pics = TRUE;
                    }
                }

                if (pics) pt.flags |= FLG_FreeStrip;
                else
                {
                    if (pt.brushes)
                    {
                        freeVecPooled(pool,pt.brushes);

                        pt.brushes = NULL;
                        if (pt.sbrushes) pt.sbrushes = NULL;
                        if (pt.dbrushes) pt.dbrushes = NULL;
                    }
                }
            }
        }

        if (!pics)
        {
            if (pt.pics)
            {
                STRPTR *p;

                for (nbr = 0, p = pt.pics; *p; nbr++, p++);

                if (nbr)
                {
                    STRPTR *sp = NULL, *dp = NULL;
                    ULONG  brpsize, brsize, totsize, num;

                    if (pt.spics)
                    {
                        for (num = 0, p = pt.spics; *p; num++, p++);
                        if (nbr!=num) pt.spics = NULL;
                    }

                    if (pt.dpics)
                    {
                        for (num = 0, p = pt.dpics; *p; num++, p++);
                        if (nbr!=num) pt.dpics = NULL;
                    }

                    nbr++;

                    brpsize = sizeof(struct MUIS_TheBar_Brush *)*nbr;
                    brsize  = sizeof(struct MUIS_TheBar_Brush)*nbr;

                    totsize = brpsize+brsize;
                    if (pt.spics) totsize += brpsize+brsize;
                    if (pt.dpics) totsize += brpsize+brsize;

                    if((pt.brushes = allocVecPooled(pool,totsize)))
                    {
                        struct MUIS_TheBar_Brush *brush, *sbrush = NULL, *dbrush = NULL;
                        int                      i;

                        brush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.brushes+brpsize);

                        if (pt.spics)
                        {
                            pt.sbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            sbrush      = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.sbrushes+brpsize);
                        }

                        if (pt.dpics)
                        {
                            if (pt.spics) pt.dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)sbrush+brsize);
                            else pt.dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);

                            dbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt.dbrushes+brpsize);
                        }

                        p = pt.pics;
                        if (pt.sbrushes) sp = pt.spics;
                        if (pt.dbrushes) dp = pt.dpics;

                        for(i=0; *p; i++, p++)
                        {
                            if(!loadDTBrush(pool, pt.brushes[i] = brush+i, *p))
                            {
                              E(DBF_STARTUP, "couldn't load brush '%s' (%d)", *p, i);
                              break;
                            }
                            else
                              W(DBF_STARTUP, "successfully loaded brush '%s' (%d)", *p, i);

                            if (pt.sbrushes)
                            {
                                if (*sp!=MUIV_TheBar_SkipPic) loadDTBrush(pool,pt.sbrushes[i] = sbrush+i,*sp);
                                sp++;
                            }

                            if (pt.dbrushes)
                            {
                                if (*dp!=MUIV_TheBar_SkipPic) loadDTBrush(pool,pt.dbrushes[i] = dbrush+i,*dp);

                                dp++;
                            }
                        }

                        if (*p)
                        {
                            freeVecPooled(pool,pt.brushes);

                            pt.brushes = NULL;
                            if (pt.sbrushes) pt.sbrushes = NULL;
                            if (pt.dbrushes) pt.dbrushes = NULL;
                        }
                        else
                        {
                            pt.brushes[i] = NULL;
                            if (pt.sbrushes) pt.sbrushes[i] = NULL;
                            if (pt.dbrushes) pt.dbrushes[i] = NULL;

                            pt.flags |= FLG_FreeBrushes;
                            pics = TRUE;
                        }
                    }
                }
            }
        }

        if (idrawer)
        {
            CurrentDir(odir);
            UnLock(idrawer);
        }

        me->pr_WindowPtr = win;
    }
    else
    {
        struct MUIS_TheBar_Brush **brush;

        for (brush = pt.brushes, nbr = 0; *brush; nbr++, brush++);
        nbr++;

        pics = TRUE;
    }

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Group_LayoutHook,   (ULONG)&LayoutHook,
            MUIA_Group_HorizSpacing, 0,
            MUIA_Group_VertSpacing,  0,
            (lib_flags & BASEFLG_MUI20) ? TAG_IGNORE : MUIA_CustomBackfill, TRUE,
            TAG_MORE,(ULONG)attrs)))
    {
        struct InstData *data = INST_DATA(cl,obj);

        data->brushes   = pt.brushes;
        data->sbrushes  = pt.sbrushes;
        data->dbrushes  = pt.dbrushes;
        data->viewMode  = pt.viewMode;
        data->flags     = pt.flags | (pics ? 0 : FLG_TextOnly);
        data->flags2    = pt.flags2;
        data->id        = pt.id;
        data->pool      = pool;
        data->nbr       = nbr;
        data->labelPos  = pt.labelPos;
        data->barPos    = pt.barPos;
        data->remove    = pt.remove;
        data->cols      = pt.cols;
        data->rows      = pt.rows;
        #ifdef __MORPHOS__
    	data->userFrame = pt.userFrame;
        #endif
        data->sortMsgID = MUIM_Group_Sort;

        if (data->flags & FLG_FreeStrip)
        {
            if (sb.data)  memcpy(&data->image,&sb,sizeof(data->image));
            if (ssb.data) memcpy(&data->simage,&ssb,sizeof(data->simage));
            if (dsb.data) memcpy(&data->dimage,&dsb,sizeof(data->dimage));
        }

        data->userFlags  = pt.userFlags;
        data->userFlags2 = pt.userFlags2;

        if (data->userFlags || data->userFlags2)
        {
            ULONG userFlags = data->userFlags, userFlags2 = data->userFlags2;

            if (userFlags & UFLG_UserHorizSpacing)
            {
                if (pt.horizSpacing<=256) data->horizSpacing = pt.horizSpacing;
                else userFlags &= ~UFLG_UserHorizSpacing;
            }

            if (userFlags & UFLG_UserVertSpacing)
            {
                if (pt.vertSpacing<=256) data->vertSpacing = pt.vertSpacing;
                else userFlags &= ~UFLG_UserVertSpacing;
            }

            if (userFlags & UFLG_UserLeftBarFrameSpacing)
            {
                if (pt.leftBarFrameSpacing>0 && pt.leftBarFrameSpacing<=256) data->leftBarFrameSpacing = pt.leftBarFrameSpacing;
                else userFlags &= ~UFLG_UserLeftBarFrameSpacing;
            }

            if (userFlags & UFLG_UserRightBarFrameSpacing)
            {
                if (pt.rightBarFrameSpacing>0 && pt.rightBarFrameSpacing<=256) data->rightBarFrameSpacing = pt.rightBarFrameSpacing;
                else userFlags &= ~UFLG_UserRightBarFrameSpacing;
            }

            if (userFlags & UFLG_UserTopBarFrameSpacing)
            {
                if (pt.topBarFrameSpacing>0 && pt.topBarFrameSpacing<=256) data->topBarFrameSpacing = pt.topBarFrameSpacing;
                else userFlags &= ~UFLG_UserTopBarFrameSpacing;
            }

            if (userFlags & UFLG_UserBottomBarFrameSpacing)
            {
                if (pt.bottomBarFrameSpacing>0 && pt.bottomBarFrameSpacing<=256) data->bottomBarFrameSpacing = pt.bottomBarFrameSpacing;
                else userFlags &= ~UFLG_UserBottomBarFrameSpacing;
            }

            if (userFlags & UFLG_UserBarSpacerSpacing)
            {
                if (pt.barSpacerSpacing<=256) data->barSpacerSpacing = pt.barSpacerSpacing;
                else userFlags &= ~UFLG_UserBarSpacerSpacing;
            }

            if (userFlags & UFLG_UserHorizInnerSpacing)
            {
                if (pt.horizInnerSpacing>0 && pt.horizInnerSpacing<=256) data->horizInnerSpacing = pt.horizInnerSpacing;
                else userFlags &= ~UFLG_UserHorizInnerSpacing;
            }

            if (userFlags & UFLG_UserTopInnerSpacing)
            {
                if (pt.topInnerSpacing>0 && pt.topInnerSpacing<=256) data->topInnerSpacing = pt.topInnerSpacing;
                else userFlags &= ~UFLG_UserTopInnerSpacing;
            }

            if (userFlags & UFLG_UserBottomInnerSpacing)
            {
                if (pt.bottomInnerSpacing>0 && pt.bottomInnerSpacing<=256) data->bottomInnerSpacing = pt.bottomInnerSpacing;
                else userFlags &= ~UFLG_UserBottomInnerSpacing;
            }

            if (userFlags & UFLG_UserHorizTextGfxSpacing)
            {
                if (pt.horizTextGfxSpacing>0 && pt.horizTextGfxSpacing<=256) data->horizTextGfxSpacing = pt.horizTextGfxSpacing;
                else userFlags &= ~UFLG_UserHorizTextGfxSpacing;
            }

            if (userFlags & UFLG_UserVertTextGfxSpacing)
            {
                if (pt.vertTextGfxSpacing>0 && pt.vertTextGfxSpacing<=256) data->vertTextGfxSpacing = pt.vertTextGfxSpacing;
                else userFlags &= ~UFLG_UserVertTextGfxSpacing;
            }

            if (userFlags & UFLG_UserPrecision)
            {
                if (pt.precision<MUIV_TheBar_Precision_Last) data->precision = pt.precision;
                else userFlags &= ~UFLG_UserPrecision;
            }

            if (userFlags & UFLG_UserScale)
            {
                if (pt.scale>=25 && pt.scale<=400) data->scale = pt.scale;
                else userFlags &= ~UFLG_UserScale;
            }

            if (userFlags & UFLG_UserDisMode)
            {
                if (pt.disMode<MUIV_TheBar_DisMode_Last)
                {
                    data->disMode = pt.disMode;
                    if (data->disMode==MUIV_TheBar_DisMode_Sunny) data->flags &= ~FLG_Sunny;
                }
                else userFlags &= ~UFLG_UserDisMode;
            }

            if (userFlags & UFLG_UserDisMode)
            {
                if (pt.disMode<MUIV_TheBar_DisMode_Last)
                {
                    data->disMode = pt.disMode;
                    if (data->disMode==MUIV_TheBar_DisMode_Sunny) data->flags &= ~FLG_Sunny;
                }
                else userFlags &= ~UFLG_UserDisMode;
            }

            if (userFlags2 & UFLG2_UserSpacersSize)
            {
                if (pt.spacersSize<MUIV_TheBar_SpacersSize_Last) data->spacersSize = pt.spacersSize;
                else userFlags2 &= ~UFLG2_UserSpacersSize;
            }

            data->userFlags  = userFlags;
            data->userFlags2 = userFlags2;
        }

        if (pt.spacer<data->nbr-1) data->spacer = pt.spacer;
        else data->spacer = -1;

        NewList((struct List *)&data->buttons);

        if (data->flags & FLG_DragBar)
        {
            if((data->db = dragBarObject,
                    MUIA_Group_Horiz,      data->flags & FLG_Horiz,
                    MUIA_TheButton_TheBar, obj,
                End))
            {
              DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->db);
            }
        }

        if((buttons = pt.buttons))
        {
            while (buttons->img!=MUIV_TheBar_End)
            {
                DoMethod(obj,MUIM_TheBar_AddButton,(ULONG)buttons);

                if (buttons->exclude && (buttons->flags & BFLG_Selected))
                    data->active = buttons->ID;

                buttons++;
            }
        }

        if (data->remove) updateRemove(cl,obj,data);

        if (data->cols || data->rows)
        {
            data->flags |= FLG_Table;
            if (data->flags & FLG_DragBar) set(data->db,MUIA_Group_Horiz,data->cols);
        }
    }
    else
    {
        DeletePool(pool);
    }

    RETURN((ULONG)obj);
    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;
  APTR pool = data->pool;
  ULONG res;

  ENTER();

  D(DBF_STARTUP, "disposing TheBar object: %08lx", obj);

  // delete all hided buttons and the notifyListClones of all buttons as well
  for(node = data->buttons.mlh_Head; node->mln_Succ; )
  {
    struct Button *button = (struct Button *)node;

    node = node->mln_Succ;

    removeButton(cl, obj, button);
  }

  // call the super method to let MUI clear everything else
  res = DoSuperMethodA(cl, obj, msg);

  // delete our previously allocated memory pool
  DeletePool(pool);

  RETURN(res);
  return res;
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  BOOL result = FALSE;

  ENTER();

  switch(msg->opg_AttrID)
  {
    case MUIA_TheBar_MouseOver:        *msg->opg_Storage = data->id; result=TRUE; break;
    case MUIA_TheBar_Images:           *msg->opg_Storage = (ULONG)data->brushes; result=TRUE; break;
    case MUIA_TheBar_DisImages:        *msg->opg_Storage = (ULONG)data->dbrushes; result=TRUE; break;
    case MUIA_TheBar_SelImages:        *msg->opg_Storage = (ULONG)data->sbrushes; result=TRUE; break;
    case MUIA_TheBar_ViewMode:         *msg->opg_Storage = data->viewMode; result=TRUE; break;
    case MUIA_TheBar_Borderless:       *msg->opg_Storage = (data->flags & FLG_Borderless) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Raised:           *msg->opg_Storage = (data->flags & FLG_Raised) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Sunny:            *msg->opg_Storage = (data->flags & FLG_Sunny) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Scaled:           *msg->opg_Storage = (data->flags & FLG_Scaled) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_SpacerIndex:      *msg->opg_Storage = data->spacer; result=TRUE; break;
    case MUIA_TheBar_EnableKeys:       *msg->opg_Storage = (data->flags & FLG_EnableKeys) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_TextOnly:         *msg->opg_Storage = (data->flags & FLG_TextOnly) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_LabelPos:         *msg->opg_Storage = data->labelPos; result=TRUE; break;
    case MUIA_TheBar_BarPos:           *msg->opg_Storage = data->barPos; result=TRUE; break;
    case MUIA_TheBar_Active:           *msg->opg_Storage = data->active; result=TRUE; break;
    case MUIA_TheBar_Columns:          *msg->opg_Storage = data->cols; result=TRUE; break;
    case MUIA_TheBar_Rows:             *msg->opg_Storage = data->rows; result=TRUE; break;
    case MUIA_TheBar_Free:             *msg->opg_Storage = ((data->flags & FLG_FreeHoriz) && (data->flags & FLG_FreeVert)) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_FreeHoriz:        *msg->opg_Storage = (data->flags & FLG_FreeHoriz) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_FreeVert:         *msg->opg_Storage = (data->flags & FLG_FreeVert) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_BarSpacer:        *msg->opg_Storage = (data->flags & FLG_BarSpacer) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_RemoveSpacers:    *msg->opg_Storage = data->remove; result=TRUE; break;
    #ifdef __MORPHOS__
    case MUIA_TheBar_Frame:            *msg->opg_Storage = FALSE; result=TRUE; break;
    #else
    case MUIA_TheBar_Frame:            *msg->opg_Storage = (data->flags & FLG_Framed) ? TRUE : FALSE; result=TRUE; break;
    #endif
    case MUIA_TheBar_DragBar:          *msg->opg_Storage = (data->flags & FLG_DragBar) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_IgnoreAppearance: *msg->opg_Storage = (data->flags2 & FLG2_IgnoreAppearance) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_DisMode:          *msg->opg_Storage = data->disMode; result=TRUE; break;
    case MUIA_TheBar_NtRaiseActive:    *msg->opg_Storage = (data->userFlags2 & UFLG2_NtRaiseActive) ? TRUE : FALSE; result=TRUE; break;

    case MUIA_Group_Horiz:             *msg->opg_Storage = (data->flags & FLG_Horiz) ? TRUE : FALSE; result=TRUE; break;

    case MUIA_Version:                 *msg->opg_Storage = LIB_VERSION; result=TRUE; break;
    case MUIA_Revision:                *msg->opg_Storage = LIB_REVISION; result=TRUE; break;

    default:                           result = DoSuperMethodA(cl,obj,(Msg)msg);
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

#ifndef __MORPHOS__
static void
allocateFramePens(Object *obj,struct InstData *data)
{
    APTR ptr;

    ENTER();

    if (!DoMethod(obj,MUIM_GetConfigItem,MUICFG_TheBar_BarFrameShinePen,&ptr))
        ptr = MUIDEF_TheBar_BarFrameShinePen;
    data->barFrameShinePen = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);

    if (!DoMethod(obj,MUIM_GetConfigItem,MUICFG_TheBar_BarFrameShadowPen,&ptr))
        ptr = MUIDEF_TheBar_BarFrameShadowPen;
    data->barFrameShadowPen = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);

    LEAVE();
}

static void
freeFramePens(Object *obj,struct InstData *data)
{
    ENTER();

    MUI_ReleasePen(muiRenderInfo(obj),data->barFrameShadowPen);
    MUI_ReleasePen(muiRenderInfo(obj),data->barFrameShinePen);

    LEAVE();
}
#endif

/***********************************************************************/

enum
{
    SFLG_Rebuild       = 1<<0,
    SFLG_Horiz         = 1<<1,
    SFLG_Table         = 1<<2,
    SFLG_Remove        = 1<<3,
    SFLG_Change        = 1<<4,

    SFLG_ViewMode      = 1<<5,
    SFLG_Scaled        = 1<<6,
    SFLG_Sunny         = 1<<7,
    SFLG_Raised        = 1<<8,
    SFLG_LabelPos      = 1<<9,
    SFLG_EnableKeys    = 1<<10,


    SFLG_Frame         = 1<<11,
    SFLG_DragBar       = 1<<12,

    SFLG_NtRaiseActive = 1<<13,

    SFLG_ButtonAttrs   = SFLG_ViewMode|SFLG_Scaled|SFLG_Sunny|SFLG_Raised|SFLG_LabelPos|SFLG_EnableKeys|SFLG_NtRaiseActive,
};

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem          *tstate;
    ULONG          flags = 0, res;

    ENTER();

    for(tstate = msg->ops_AttrList; (tag = NextTagItem(&tstate)); )
    {
        ULONG tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_TheBar_Active:
                if (tidata==data->active) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    struct Button *b, *button, *succ;

                    data->active = tidata;

                    if (!(b = findButton(data,tidata)))
                    {
                        tag->ti_Tag = TAG_IGNORE;
                        break;
                    }

                    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                    {
                        if (b==button) continue;

                        if (b->exclude & (1<<button->ID))
                        {
                            if (button->flags & BFLG_Sleep) b->flags &= ~MUIV_TheBar_ButtonFlag_Selected;
                            else set(button->obj,MUIA_Selected,FALSE);
                        }
                    }

                    if (b->flags & BFLG_Sleep) b->flags |= MUIV_TheBar_ButtonFlag_Selected;
                    else set(b->obj,MUIA_Selected,TRUE);
                }
                break;

            case MUIA_TheBar_MouseOver:
                if (data->id==tidata) tag->ti_Tag = TAG_IGNORE;
                else data->id =tidata;
                break;


            case MUIA_Group_Horiz:
                if (!BOOLSAME(tidata,data->flags & FLG_Horiz))
                {
                    if (tidata)
                    {
                        data->flags |= FLG_Horiz|FLG_FreeHoriz;
                        data->flags &= ~FLG_FreeVert;
                    }
                    else
                    {
                        data->flags |= FLG_FreeVert;
                        data->flags &= ~(FLG_Horiz|FLG_FreeHoriz);
                    }
                    flags |= SFLG_Horiz;
                }
                break;

	        #ifndef __MORPHOS__
            case MUIA_TheBar_Frame:
                if (!BOOLSAME(tidata,data->flags & FLG_Framed))
                {
                    if (tidata)
                    {
                        if (data->flags & FLG_Setup) allocateFramePens(obj,data);

                        data->flags |= FLG_Framed;
                    }
                    else
                    {

                        if (data->flags & FLG_Setup) freeFramePens(obj,data);

                        data->flags &= ~FLG_Framed;
                    }

                    flags |= SFLG_Frame;
                }
                break;
    	    #endif

            case MUIA_TheBar_DragBar:
                if (!BOOLSAME(tidata,data->flags & FLG_DragBar))
                {
                    if (tidata)
                    {
                        data->flags |= FLG_DragBar;

                        if((data->db = dragBarObject,
                            	MUIA_Group_Horiz,      data->flags & FLG_Horiz,
                            	MUIA_TheButton_TheBar, obj,
                            End))
                        {
                            DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->db);
                            flags |= SFLG_Rebuild;
                        }
                    }
                    else
                    {
                        data->flags &= ~FLG_DragBar;

                        DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)data->db);
                        MUI_DisposeObject(data->db);
                        data->db = NULL;
                    }

                    flags |= SFLG_DragBar;
                }
                break;

            case MUIA_TheBar_Borderless:
                if (BOOLSAME(tidata,data->flags & FLG_Borderless)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_Borderless;
                    else data->flags &= ~FLG_Borderless;

                    flags |= SFLG_Rebuild;
                }
                break;

            case MUIA_TheBar_ViewMode:
                if (tidata>=MUIV_TheBar_ViewMode_Last) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    ULONG old = data->viewMode;

                    if ((old==tidata) || (data->flags & FLG_TextOnly)) tag->ti_Tag = TAG_IGNORE;
                    else
                    {
                        if ((data->flags & FLG_Setup) && ((old==MUIV_TheBar_ViewMode_Text) || (tidata==MUIV_TheBar_ViewMode_Text))) flags |= SFLG_Rebuild;
                        else flags |= SFLG_ViewMode;

                        data->viewMode = tidata;
                    }
                }
                break;

            case MUIA_TheBar_Scaled:
                if (BOOLSAME(tidata,data->flags & FLG_Scaled)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_Scaled;
                    else data->flags &= ~FLG_Scaled;

                    if (data->flags & FLG_Setup) flags |= SFLG_Rebuild;
                    else flags |= SFLG_Scaled;
                }
                break;

            case MUIA_TheBar_Sunny:
                if (BOOLSAME(tidata,data->flags & FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_Sunny;
                    else data->flags &= ~FLG_Sunny;

                    if (data->flags & FLG_Setup)
                    {
                        flags |= SFLG_Rebuild;
                        if (data->flags & FLG_FreeStrip) data->flags |= FLG_RebuildbitMaps;
                    }
                    else flags |= SFLG_Sunny;
                }
                break;

            case MUIA_TheBar_Raised:
                if (BOOLSAME(tidata,data->flags & FLG_Raised)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_Raised;
                    else data->flags &= ~FLG_Raised;

                    if (data->flags & FLG_Setup) flags |= SFLG_Rebuild;
                    else flags |= SFLG_Raised;
                }
                break;

            case MUIA_TheBar_NtRaiseActive:
                if (BOOLSAME(tidata,data->userFlags2 & UFLG2_NtRaiseActive)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->userFlags2 |= UFLG2_NtRaiseActive;
                    else data->userFlags2 &= ~UFLG2_NtRaiseActive;
                    flags |= SFLG_NtRaiseActive;
                }
                break;

            case MUIA_TheBar_LabelPos:
                if ((tidata>=MUIV_TheBar_LabelPos_Last) || (tidata==data->labelPos)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->labelPos = tidata;
                    flags |= SFLG_LabelPos;
                }
                break;

            case MUIA_TheBar_EnableKeys:
                if (BOOLSAME(tidata,data->flags & FLG_EnableKeys)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_EnableKeys;
                    else data->flags &= ~FLG_EnableKeys;
                    flags |= SFLG_EnableKeys;
                }
                break;

            case MUIA_TheBar_BarPos:
                if ((tidata>=MUIV_TheBar_BarPos_Last) || (data->barPos==tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->barPos = tidata;
                    flags |= SFLG_Change;
                }
                break;

            case MUIA_Background:
                data->flags |= FLG_Background;
                break;

            case MUIA_TheBar_Columns:
                if (data->cols==(int)tidata) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->cols = (int)tidata;
                    flags |= SFLG_Table;
                }
                break;

            case MUIA_TheBar_Rows:
                if (data->rows==(int)tidata) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->rows = (int)tidata;
                    flags |= SFLG_Table;
                }
                break;

            case MUIA_TheBar_Free:
                if (BOOLSAME(tidata,(data->flags & FLG_FreeHoriz) && (data->flags & FLG_FreeVert))) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_FreeHoriz|FLG_FreeVert;
                    else data->flags &= ~(FLG_FreeHoriz|FLG_FreeVert);

                    flags |= SFLG_Change;
                }
                break;

            case MUIA_TheBar_FreeHoriz:
                if (BOOLSAME(tidata,data->flags & FLG_FreeHoriz)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_FreeHoriz;
                    else data->flags &= ~FLG_FreeHoriz;

                    flags |= SFLG_Change;
                }
                break;

            case MUIA_TheBar_FreeVert:
                if (BOOLSAME(tidata,data->flags & FLG_FreeVert)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_FreeVert;
                    else data->flags &= ~FLG_FreeVert;

                    flags |= SFLG_Change;
                }
                break;

            case MUIA_TheBar_BarSpacer:
                if (BOOLSAME(tidata,data->flags & FLG_BarSpacer)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_BarSpacer;
                    else data->flags &= ~FLG_BarSpacer;

                    flags |= SFLG_Rebuild;
                }
                break;

            case MUIA_TheBar_SpacerIndex:
                if (data->spacer==tidata) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->spacer = tidata;

                    flags |= SFLG_Rebuild;
                }
                break;

            case MUIA_TheBar_IgnoreAppearance:
                if (BOOLSAME(tidata,data->flags2 & FLG2_IgnoreAppearance)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags2 |= FLG2_IgnoreAppearance;
                    else data->flags2 &= ~FLG2_IgnoreAppearance;
                }
                break;

            case MUIA_TheBar_RemoveSpacers:
                if (data->remove==tidata) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->remove = tidata;
                    flags |= SFLG_Remove;
                }
                break;
        }
    }

    if ((data->flags & FLG_Setup) && flags) DoMethod(obj,MUIM_Group_InitChange);

    if (flags & SFLG_Remove) updateRemove(cl,obj,data);

    if (flags & SFLG_Table)
    {
        if (data->cols || data->rows)
        {
            if (data->flags & FLG_DragBar) set(data->db,MUIA_Group_Horiz,data->cols);

            if (!(data->flags & FLG_Table))
            {
                struct Button *button, *succ;

                data->flags |= FLG_Table;

                for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                {
                    if (button->flags & BFLG_Sleep) continue;
                    set(button->obj,MUIA_Group_Horiz,TRUE);
                }
            }
        }
        else
        {
            if (data->flags & FLG_Table)
            {
                struct Button *button, *succ;

                data->flags &= ~FLG_Table;

                for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                {
                    if (button->flags & BFLG_Sleep) continue;
                    set(button->obj,MUIA_Group_Horiz,data->flags & FLG_Horiz);
                }

                if (data->flags & FLG_DragBar) set(data->db,MUIA_Group_Horiz,data->flags & FLG_Horiz);
            }
        }
    }

    if (flags & SFLG_Horiz)
    {
        ULONG horiz = data->flags & FLG_Horiz;

        if (!(flags & SFLG_Rebuild))
        {
            struct Button *button, *succ;

            for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
            {
                if (button->flags & BFLG_Sleep) continue;
                set(button->obj,MUIA_Group_Horiz,horiz);
            }
        }

        if (!(data->flags & FLG_Table) && data->db) set(data->db,MUIA_Group_Horiz,horiz);
    }

    if (flags & SFLG_Rebuild) DoMethod(obj,MUIM_TheBar_Rebuild);
    else
        if (flags & SFLG_ButtonAttrs)
        {
            struct Button *button;
            struct Button *succ;
            struct TagItem attrs[8] = { { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_IGNORE, 0 },
                                        { TAG_DONE,   0 } };

            if (flags & SFLG_ViewMode)
            {
                attrs[0].ti_Tag  = MUIA_TheButton_ViewMode;
                attrs[0].ti_Data = data->viewMode;
            }

            if (flags & SFLG_Raised)
            {
                attrs[1].ti_Tag  = MUIA_TheButton_Raised;
                attrs[1].ti_Data = (data->flags & FLG_Raised) ? TRUE : FALSE;
            }

            if (flags & SFLG_Scaled)
            {
                attrs[2].ti_Tag  = MUIA_TheButton_Scaled;
                attrs[2].ti_Data = (data->flags & FLG_Scaled) ? TRUE : FALSE;
            }

            if (flags & SFLG_Sunny)
            {
                attrs[3].ti_Tag  = MUIA_TheButton_Sunny;
                attrs[3].ti_Data = (data->flags & FLG_Sunny) ? TRUE : FALSE;
            }

            if (flags & SFLG_LabelPos)
            {
                attrs[4].ti_Tag  = MUIA_TheButton_LabelPos;
                attrs[4].ti_Data = data->labelPos;
            }

            if (flags & SFLG_EnableKeys)
            {
                attrs[5].ti_Tag  = MUIA_TheButton_EnableKey;
                attrs[5].ti_Data = (data->flags & FLG_EnableKeys) ? TRUE : FALSE;
            }

            if (flags & SFLG_NtRaiseActive)
            {
                attrs[6].ti_Tag  = MUIA_TheButton_NtRaiseActive;
                attrs[6].ti_Data = (data->userFlags2 & UFLG2_NtRaiseActive) ? TRUE : FALSE;
            }

            DoSuperMethod(cl,obj,OM_SET,(ULONG)attrs,NULL);

            for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                if (!(button->flags & BFLG_Sleep)) DoMethod(button->obj,OM_SET,(ULONG)attrs,NULL);
        }

    res = DoSuperMethodA(cl,obj,(Msg)msg);

    if ((data->flags & FLG_Setup) && flags) DoMethod(obj,MUIM_Group_ExitChange);

    RETURN(res);
    return res;
}

/***********************************************************************/

static struct MUIS_TheBar_Appearance staticAp = { MUIDEF_TheBar_Appearance_ViewMode,
                                                  MUIDEF_TheBar_Appearance_Flags,
                                                  MUIDEF_TheBar_Appearance_LabelPos,
                                                  { 0, 0 }
                                                };

static ULONG
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    #if !defined(VIRTUAL)
    Object               *parent;
    #endif
    STRPTR               ptr;
    ULONG                *val;

    ENTER();

    /* Appearance */
    if (!(data->flags2 & FLG2_IgnoreAppearance))
    {
        struct MUIS_TheBar_Appearance *ap;

        if (!getconfigitem(cl,obj,MUICFG_TheBar_Appearance,&ap))
            ap = &staticAp;

        SetAttrs(obj,MUIA_TheBar_ViewMode,     ap->viewMode,
                     MUIA_TheBar_LabelPos,     ap->labelPos,
                     MUIA_TheBar_Borderless,   (ULONG)(ap->flags & MUIV_TheBar_Appearance_Borderless),
                     MUIA_TheBar_Raised,       (ULONG)(ap->flags & MUIV_TheBar_Appearance_Raised),
                     MUIA_TheBar_Sunny,        (ULONG)(ap->flags & MUIV_TheBar_Appearance_Sunny),
                     MUIA_TheBar_Scaled,       (ULONG)(ap->flags & MUIV_TheBar_Appearance_Scaled),
                     MUIA_TheBar_BarSpacer,    (ULONG)(ap->flags & MUIV_TheBar_Appearance_BarSpacer),
                     MUIA_TheBar_EnableKeys,   (ULONG)(ap->flags & MUIV_TheBar_Appearance_EnableKeys),
                     TAG_DONE);
    }

    if (!(data->flags & FLG_Background))
    {
        ULONG done = FALSE;

        if (lib_flags & BASEFLG_MUI4)
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_GroupBack,&ptr))
            {
                SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,(ULONG)ptr,TAG_DONE);
                done = TRUE;
            }
        }
        else
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_UseGroupBack,&val) ? *val : MUIDEF_TheBar_UseGroupBack)
            {
                if (!(lib_flags & BASEFLG_MUI20) && getconfigitem(cl,obj,MUICFG_TheBar_Gradient,&ptr))
                {
                    memcpy(&data->grad,ptr,sizeof(data->grad));
                    SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,NULL,TAG_DONE);
                    data->flags2 |= FLG2_Gradient;
                    done = TRUE;
                }
                else
                {
                    if (getconfigitem(cl,obj,MUICFG_TheBar_GroupBack,&ptr))
                    {
                        SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,(ULONG)ptr,TAG_DONE);
                        done = TRUE;
                    }
                }
            }
        }

        if (!done)
        {
            DoSuperMethod(cl,obj,OM_GET,MUIA_Parent,(ULONG)&ptr);
            SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,ptr ? (ULONG)_backspec(ptr) : MUII_WindowBack,TAG_DONE);
        }
    }

    #ifdef __MORPHOS__
    if (getconfigitem(cl,obj,MUICFG_TheBar_Frame,&ptr))
    {
        memcpy(&data->frameSpec,ptr,sizeof(data->frameSpec));
        SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,(ULONG)&data->frameSpec,TAG_DONE);
    }
    else
        if (data->userFlags2 & UFLG2_UserFrame) SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,data->userFrame,TAG_DONE);
    	else SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,MUIV_Frame_None,TAG_DONE);
    #endif

    if(!DoSuperMethodA(cl,obj,msg))
    {
      RETURN(FALSE);
      return FALSE;
    }

    if (!(data->userFlags & UFLG_UserHorizSpacing))
    {
        data->horizSpacing = getconfigitem(cl,obj,MUICFG_TheBar_HorizSpacing,&val) ?
            *val : MUIDEF_TheBar_HorizSpacing;
    }

    if (!(data->userFlags & UFLG_UserVertSpacing))
    {
        data->vertSpacing = getconfigitem(cl,obj,MUICFG_TheBar_VertSpacing,&val) ?
            *val : MUIDEF_TheBar_VertSpacing;
    }

    if (!(data->userFlags & UFLG_UserLeftBarFrameSpacing))
    {
        data->leftBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_LeftBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_LeftBarFrameSpacing;
    }

    if (!(data->userFlags & UFLG_UserRightBarFrameSpacing))
    {
        data->rightBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_RightBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_RightBarFrameSpacing;
    }

    if (!(data->userFlags & UFLG_UserTopBarFrameSpacing))
    {
        data->topBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_TopBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_TopBarFrameSpacing;
    }

    if (!(data->userFlags & UFLG_UserBottomBarFrameSpacing))
    {
        data->bottomBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_BottomBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_BottomBarFrameSpacing;
    }

    /* IgnoreSelImages */
    if (!(data->userFlags & UFLG_UserIgnoreSelImages))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreSelImages,&val) ? *val : MUIDEF_TheBar_IgnoreSelImages)
            data->userFlags |= UFLG_IgnoreSelImages;
        else data->userFlags &= ~UFLG_IgnoreSelImages;
   }

    /* IgnoreDisImages */
    if (!(data->userFlags & UFLG_UserIgnoreDisImages))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreDisImages,&val) ? *val : MUIDEF_TheBar_IgnoreDisImages)
            data->userFlags |= UFLG_IgnoreDisImages;
        else data->userFlags &= ~UFLG_IgnoreDisImages;
    }

    /* DontMove */
    if (!(data->userFlags2 & UFLG2_UserDontMove))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_DontMove,&val) ? *val : MUIDEF_TheBar_DontMove)
            data->userFlags2 |= UFLG2_DontMove;
        else data->userFlags2 &= ~UFLG2_DontMove;
    }

    /* NtRaiseActive */
    if (!(data->userFlags2 & UFLG2_UserNtRaiseActive))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_NtRaiseActive,&val) ? *val : MUIDEF_TheBar_NtRaiseActive)
            data->userFlags2 |= UFLG2_NtRaiseActive;
        else data->userFlags2 &= ~UFLG2_NtRaiseActive;
    }

    #ifndef __MORPHOS__
    /* Frame pens */
    if (data->flags & FLG_Framed) allocateFramePens(obj,data);
    #endif

    /* DisMode */
    if (!(data->userFlags & UFLG_UserDisMode))
    {
        data->disMode = getconfigitem(cl,obj,MUICFG_TheBar_DisMode,&val) ?
            *val : MUIDEF_TheBar_DisMode;

        if (data->disMode==MUIV_TheBar_DisMode_Sunny) data->flags &= ~FLG_Sunny;
    }

    /* SpacersSize */
    if (!(data->userFlags2 & UFLG2_UserSpacersSize))
    {
        data->spacersSize = getconfigitem(cl,obj,MUICFG_TheBar_SpacersSize,&val) ?
            *val : MUIDEF_TheBar_SpacersSize;
    }

    /* Derive GFX env info */
    data->screen = _screen(obj);
    data->screenDepth = GetBitMapAttr(data->screen->RastPort.BitMap,BMA_DEPTH);
    if (CyberGfxBase && IsCyberModeID(GetVPModeID(&data->screen->ViewPort)))
    {
        data->flags |= FLG_CyberMap;
        if (data->screenDepth>8) data->flags |= FLG_CyberDeep;
    }

    if (data->flags & FLG_FreeStrip)
    {
        struct Button *button, *succ;

        build(data);

        for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
            if (!(button->flags & BFLG_Sleep)) DoMethod(button->obj,MUIM_TheButton_Build);
    }

    memset(&data->eh,0,sizeof(data->eh));
    data->eh.ehn_Class  = cl;
    data->eh.ehn_Object = obj;
    data->eh.ehn_Events = IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,(ULONG)&data->eh);

    #if defined(VIRTUAL)
    data->flags |= FLG_IsInVirtgroup;
    #else
    for (parent = obj; ;)
    {
        if((parent = (Object *)xget(parent, MUIA_Parent)) == NULL)
          break;

        if((ptr = (STRPTR)xget(parent, MUIA_Virtgroup_Top)))
        {
            data->flags |= FLG_IsInVirtgroup;
            break;
        }
    }
    #endif

    data->flags |= FLG_Setup;

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    ULONG result = 0;

    ENTER();

    #ifndef __MORPHOS__
    if (data->flags & FLG_Framed) freeFramePens(obj,data);
    #endif

    DoMethod(_win(obj),MUIM_Window_RemEventHandler,(ULONG)&data->eh);

    if (data->flags & FLG_FreeStrip) freeBitMaps(data);

    data->flags  &= ~(FLG_Setup|FLG_CyberMap|FLG_CyberDeep|FLG_IsInVirtgroup);
    data->flags2 &= ~(FLG2_Gradient);

    result = DoSuperMethodA(cl,obj,msg);

    RETURN(result);
    return result;
}

/***********************************************************************/

static ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    ENTER();

    if(!DoSuperMethodA(cl,obj,msg))
    {
      RETURN(FALSE);
      return FALSE;
    }

    if ((data->flags & FLG_CyberDeep) && (data->flags2 & FLG2_Gradient))
    {
        UWORD w = _width(obj), h = _height(obj);

        if((data->gradbm = AllocBitMap(w,h,24,BMF_MINPLANES,_rp(obj)->BitMap)))
        {
            struct RastPort trp;
            ULONG  rs, gs, bs, from, to, horiz;
            int    i, x, d, step, rdif, gdif, bdif, r1, g1, b1, r2, g2, b2, r, g, b, rd = 0, gd = 0, bd = 0, ridx = 0, gidx = 0, bidx = 0;

            InitRastPort(&trp);
            trp.BitMap = data->gradbm;

            from  = data->grad.from;
            to    = data->grad.to;
            horiz = data->grad.flags & MUIV_TheBar_Gradient_Horiz;
            if (!(data->flags & FLG_Horiz)) horiz = !horiz;

            /* Compute diffs */
            r1 = (from & 0x00FF0000)>>16;
            g1 = (from & 0x0000FF00)>>8;
            b1 = (from & 0x000000FF);

            r2 = (to & 0x00FF0000)>>16;
            g2 = (to & 0x0000FF00)>>8;
            b2 = (to & 0x000000FF);

            if (r1>r2)
            {
                rdif = r1-r2;
                rs = FALSE;
            }
            else
            {
                rdif = r2-r1;
                rs = TRUE;
            }

            if (g1>g2)
            {
                gdif = g1-g2;
                gs = FALSE;
            }
            else
            {
                gdif = g2-g1;
                gs = TRUE;
            }

            if (b1>b2)
            {
                bdif = b1-b2;
                bs = FALSE;
            }
            else
            {
                bdif = b2-b1;
                bs = TRUE;
            }

            /* Find out max diff */
            step = (rdif>=gdif) ? rdif : gdif;
            if (bdif>step) step = bdif;

            /* Find out best step to minimize rest */
            if (horiz)
            {
                if (step>w) step = w;

                if (step==0) step = 1;
                else
                {
                    x = w/step;
                    if (w!=x*step)
                    {
                        step = w/(x+1);
                        if (step>w) step = w;
                    }
                }
            }
            else
            {
                if (step>h) step = h;

                if (step==0) step = 1;
                else
                {
                    x = h/step;
                    if (h!=x*step)
                    {
                        step = h/(x+1);
                        if (step>h) step = h;
                    }
                }
            }

            if (step>1)
            {
                /* Compute color components deltas */
                rd = rdif/step;
                if (!rd) rd = 1;

                gd = gdif/step;
                if (!gd) gd = 1;

                bd = bdif/step;
                if (!bd) bd = 1;

                /* Compute rests */
                ridx = (rdif>step) ? step/((rdif-step)/rd) : 0;
                gidx = (gdif>step) ? step/((gdif-step)/gd) : 0;
                bidx = (bdif>step) ? step/((bdif-step)/bd) : 0;
            }

            /* Here we go */

            d = (horiz) ? w/step : h/step;
            x = 0;
            r = r1;
            g = g1;
            b = b1;
            //printf("width:%ld steps:%ld delta:%ld rest:%ld rdif:%ld gdif:%ld bdif:%ld ridx:%ld gidx:%ld bidx:%ld\n",w,step,d,w-step*d,rdif,gdif,bdif,ridx,gidx,bidx);

            for (i = 0; i<step; i++)
            {
                ULONG col = (r<<16) | (g<<8) | b;

                /* Fill; if last step fill all rest */
                if (horiz)
                    if (i==step-1)
                    {
                        FillPixelArray(&trp,x,0,w-x,h,col);
                        break;
                    }
                    else FillPixelArray(&trp,x,0,d,h,col);
                else
                    if (i==step-1)
                    {
                        FillPixelArray(&trp,0,x,w,h-x,col);
                        break;
                    }
                    else FillPixelArray(&trp,0,x,w,d,col);

                x += d;

                /* Add delta to each color component */
                if (rs)
                {
                    r += rd;

                    /* Time to add more one more delta? */
                    if (i && ridx && !(i%ridx)) r += rd;
                    if (r>r2) r = r2;
                }
                else
                {
                    r -= rd;

                    if (i && ridx && !(i%ridx)) r -= rd;
                    if (r<r2) r = r2;
                }

                if (gs)
                {
                    g += gd;

                    if (i && gidx && !(i%gidx)) g += gd;
                    if (g>g2) g = g2;
                }
                else
                {
                    g -= gd;

                    if (i && gidx && !(i%gidx)) g -= gd;
                    if (g<g2) g = g2;
                }

                if (bs)
                {
                    b += bd;

                    if (i && bidx && !(i%bidx)) b += bd;
                    if (b>b2) b = b2;
                }
                else
                {
                    b -= bd;

                    if (i && bidx && !(i%bidx)) b -= bd;
                    if (b<b2) b = b2;
                }
            }
        }
    }

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static ULONG
mHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    ULONG result = 0;

    if (data->gradbm)
    {
        FreeBitMap(data->gradbm);
        data->gradbm = NULL;
    }

    result = DoSuperMethodA(cl,obj,msg);

    RETURN(result);
    return result;
}

/***********************************************************************/

#ifndef __MORPHOS__
static ULONG
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    ENTER();

    DoSuperMethodA(cl,obj,(Msg)msg);

    #if defined(VIRTUAL)
    if (data->flags & FLG_Framed)
    #else
    if ((data->flags & FLG_Framed) && (msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT)))
    #endif
    {
        struct RastPort rp;

        memcpy(&rp,_rp(obj),sizeof(rp));

        SetAPen(&rp,MUIPEN(data->barFrameShinePen));
        Move(&rp,_left(obj),_bottom(obj));
        Draw(&rp,_left(obj),_top(obj));
        Draw(&rp,_right(obj),_top(obj));

        SetAPen(&rp,MUIPEN(data->barFrameShadowPen));
        Draw(&rp,_right(obj),_bottom(obj));
        Draw(&rp,_left(obj)+1,_bottom(obj));
    }

    RETURN(0);
    return 0;
}
#endif

/***********************************************************************/

static ULONG
mCustomBackfill(struct IClass *cl,Object *obj,struct MUIP_CustomBackfill *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result = 0;

  ENTER();

  if(lib_flags & BASEFLG_MUI20)
  {
    result = DoSuperMethodA(cl,obj,(Msg)msg);
  }
  else
  {
    //Printf("BackFill %lx %ld %ld %ld %ld %ld %ld %ld\n",lib_flags & BASEFLG_MUI20,msg->left,msg->top,msg->right,msg->bottom,msg->xoffset,msg->yoffset);

    if(data->gradbm)
      BltBitMapRastPort(data->gradbm,msg->left-_left(obj),msg->top-_top(obj),_rp(obj),msg->left,msg->top,msg->right-msg->left+1,msg->bottom-msg->top+1,0xc0);
    else
      DoSuperMethod(cl,obj,MUIM_DrawBackground,msg->left,msg->top,msg->right-msg->left+1,msg->bottom-msg->top+1,msg->xoffset,msg->yoffset,0);
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mCreateDragImage(struct IClass *cl,Object *obj,struct MUIP_CreateDragImage *msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    ENTER();

    data->dm = (struct MUI_DragImage *)DoSuperMethodA(cl,obj,(Msg)msg);

    RETURN((ULONG)data->dm);
    return (ULONG)data->dm;
}

/***********************************************************************/

static ULONG
mDeleteDragImage(struct IClass *cl,Object *obj,struct MUIP_DeleteDragImage *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    ULONG result;

    ENTER();

    data->dm = NULL;

    result = DoSuperMethodA(cl,obj,(Msg)msg);

    RETURN(result);
    return result;
}

/***********************************************************************/

static ULONG
mInitChange(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result;

  ENTER();

  result = data->changes++ ? 0 : DoSuperMethodA(cl,obj,msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mExitChange(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result;

  ENTER();

  result = (data->changes && !(--data->changes)) ? DoSuperMethodA(cl,obj,msg) : 0;

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mRebuild(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;

  ENTER();

  D(DBF_STARTUP, "Rebuild: %08lx", obj);

  if(data->flags & FLG_Setup)
    DoMethod(obj,MUIM_Group_InitChange);

  // cleanup all buttons in our list
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct MinList *notifyList;
    struct Button *button = (struct Button *)node;

    // if the button is flagged as sleeping
    // we can continue
    if(button->flags & BFLG_Sleep)
      continue;

    // clear some flags
    button->flags &= ~(BFLG_Disabled|BFLG_Selected);

    // save some variables
    if(xget(button->obj, MUIA_Disabled))
      button->flags |= BFLG_Disabled;

    if(xget(button->obj, MUIA_Selected))
      button->flags |= BFLG_Selected;

    // remove object from group if necessary
    if(!(button->flags & BFLG_Hide))
      DoSuperMethod(cl, obj, OM_REMMEMBER, (ULONG)button->obj);

    // before we finally dispose the button object we have to create a clone
    // of the notifyList it currently maintains.
    if((notifyList = (struct MinList *)xget(button->obj, MUIA_TheButton_NotifyList)))
    {
      struct MinNode *node;

      // now we cleanup the old clone list
      NewList((struct List *)&button->notifyListClone);

      // walk through the notifyList of the button and clone it
      // as we walk by
      for(node = notifyList->mlh_Head; node->mln_Succ; node = node->mln_Succ)
      {
        struct ButtonNotify *notify = (struct ButtonNotify *)node;
        struct ButtonNotify *clone;
        ULONG size;

        size = sizeof(struct ButtonNotify)+sizeof(ULONG)*notify->msg.FollowParams;

        // clone
        if((clone = allocVecPooled(data->pool, size)))
        {
          // copy the data of the notify
          memcpy(clone, notify, size);

          // add it to our clone list
          AddTail((struct List *)&button->notifyListClone, (struct Node *)clone);
        }
        else
          break;
      }
    }

    // dispose button MUI object
    MUI_DisposeObject(button->obj);
    button->obj = NULL;
  }

  // for a MUI redraw
  if((data->flags & FLG_Setup) && !(data->flags & FLG_CyberDeep))
    MUI_Redraw(obj, MADF_DRAWOBJECT);

  // rebuild all bitmaps in case required
  if(data->flags & FLG_RebuildbitMaps)
  {
    freeBitMaps(data);
    build(data);

    data->flags &= ~FLG_RebuildbitMaps;
  }

  // now we go and rebuild all buttons again
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct Button *button = (struct Button *)node;

    // if the button is flagged as sleeping
    // we can continue
    if(button->flags & BFLG_Sleep)
      continue;

    // generate a new button object
    if((button->obj = makeButton(button, obj, data)))
    {
      struct MinNode *node;

      // add the button object to our group
      // if it isn't flagged as being hided
      if(!(button->flags & BFLG_Hide))
        DoSuperMethod(cl, obj, OM_ADDMEMBER, (ULONG)button->obj);

      // now we put the notifies from our clone list
      // back active for the new button
      for(node = button->notifyListClone.mlh_Head; node->mln_Succ; )
      {
        struct ButtonNotify *notify = (struct ButtonNotify *)node;

        node = node->mln_Succ;

        // now we set the notify as we have identifed the button
        DoMethodA(button->obj, (Msg)&notify->msg);

        // remove the clone notify from the list
        Remove((struct Node *)notify);

        // free the clone notify
        freeVecPooled(data->pool, notify);
      }

      if((data->flags & FLG_Setup) && (data->flags & FLG_FreeStrip))
        DoMethod(button->obj, MUIM_TheButton_Build);
    }
  }

  if(data->flags & FLG_Setup)
    DoMethod(obj, MUIM_Group_ExitChange);

  RETURN(0);
  return 0;
}

/***********************************************************************/

static BOOL
mNotify(struct IClass *cl, Object *obj, struct MUIP_TheBar_Notify *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;
  BOOL result = FALSE;

  ENTER();

  // now we first find the button object with its ID
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct Button *button = (struct Button *)node;

    // if the ID matches we found the correct toolbar
    // button to which we add the notify.
    if(button->ID == msg->ID)
    {
      struct MUIP_Notify *notify;

      // lets allocate a temporary buffer for sending
      // the notify method to the button correctly.
      if((notify = allocVecPooled(data->pool, sizeof(struct MUIP_Notify)+(sizeof(ULONG)*msg->followParams))))
      {
        // now we fill the notify structure
        notify->MethodID      = MUIM_Notify;
        notify->TrigAttr      = msg->attr;
        notify->TrigVal       = msg->value;
        notify->DestObj       = msg->dest;

        // fill the rest with memcpy
        memcpy(&notify->FollowParams, &msg->followParams, sizeof(ULONG)*(msg->followParams+1));

        // now we set the notify as we have identifed the button
        result = DoMethodA(button->obj, (Msg)notify);

        // free the temporary buffer again
        freeVecPooled(data->pool, notify);
      }

      break;
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mKillNotify(struct IClass *cl, Object *obj, struct MUIP_TheBar_KillNotify *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;
  BOOL result = FALSE;

  ENTER();

  // now we first find the button object with its ID
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct Button *button = (struct Button *)node;

    // if the ID matches we found the correct toolbar
    // button to which we add the notify.
    if(button->ID == msg->ID)
    {
      // now we kill the notify as we have identifed the button
      if(msg->dest != NULL)
        DoMethod(button->obj, MUIM_KillNotifyObj, msg->attr, msg->dest);
      else
        DoMethod(button->obj, MUIM_KillNotify, msg->attr);

      break;
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mAddButton(struct IClass *cl,Object *obj,struct MUIP_TheBar_AddButton *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct Button *button;

    ENTER();

    if((button = AllocPooled(data->pool, sizeof(struct Button))))
    {
        button->img     = msg->button->img;
        button->ID      = msg->button->ID;
        button->text    = msg->button->text;
        button->help    = msg->button->help;
        button->exclude = msg->button->exclude;
        button->class   = msg->button->class;
        button->obj     = NULL;
        button->flags   = 0;

        NewList((struct List *)&button->notifyListClone);

        if (msg->button->flags & MUIV_TheBar_ButtonFlag_NoClick)   button->flags |= BFLG_NoClick;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Immediate) button->flags |= BFLG_Immediate;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Toggle)    button->flags |= BFLG_Toggle ;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Disabled)  button->flags |= BFLG_Disabled;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Selected)  button->flags |= BFLG_Selected;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Sleep)     button->flags |= BFLG_Sleep;
        if (msg->button->flags & MUIV_TheBar_ButtonFlag_Hide)      button->flags |= BFLG_Hide;

        if (button->exclude)
        {
            button->flags &= ~(MUIV_TheBar_ButtonFlag_NoClick|MUIV_TheBar_ButtonFlag_Toggle);
            button->flags |= MUIV_TheBar_ButtonFlag_Immediate;
        }

        if ((button->flags & BFLG_Sleep) || (button->obj = makeButton(button,obj,data)))
        {
            AddTail((struct List *)&data->buttons, (struct Node *)button);

            if (!(button->flags & BFLG_Sleep))
            {
                set(button->obj,MUIA_TheButton_TheBar,obj);

                if (!(button->flags & BFLG_Hide))
                {
                    if (data->flags & FLG_Setup) DoMethod(obj,MUIM_Group_InitChange);
                    DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)button->obj);
                    if (data->flags & FLG_Setup)
                    {
                        if (data->flags & FLG_FreeStrip) DoMethod(button->obj,MUIM_TheButton_Build);
                        DoMethod(obj,MUIM_Group_ExitChange);
                    }
                }

                if (button->exclude) DoMethod(button->obj,MUIM_Notify,MUIA_Selected,TRUE,(ULONG)obj,3,MUIM_Set,MUIA_TheBar_Active,button->ID);

                msg->button->obj = button->obj;
            }
        }
        else
        {
            FreePooled(data->pool,button,sizeof(struct Button));
            button = NULL;
        }
    }

    RETURN((ULONG)button);
    return (ULONG)button;
}

/***********************************************************************/

static ULONG
mGetAttr(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetAttr *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct Button *bt;
  BOOL result = FALSE;

  ENTER();

  if((bt = findButton(data,msg->ID)))
  {
    switch (msg->attr)
    {
      case MUIA_TheBar_Attr_Hide:
      {
        *msg->storage = bt->flags & BFLG_Hide;
        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Sleep:
      {
        *msg->storage = bt->flags & BFLG_Sleep;
        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Disabled:
      {
        if(bt->obj)
          *msg->storage = xget(bt->obj, MUIA_Disabled);
        else
          *msg->storage = bt->flags & BFLG_Disabled;

        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Selected:
      {
        if(bt->obj)
          *msg->storage = xget(bt->obj, MUIA_Selected);
        else
          *msg->storage = bt->flags & BFLG_Selected;

        result = TRUE;
      }
      break;
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
hideButton(struct IClass *cl,Object *obj,struct InstData *data,struct Button *bt,ULONG value)
{
    ULONG res = FALSE;

    ENTER();

    if (!BOOLSAME(value,bt->flags & BFLG_Hide))
    {
        if (data->flags & FLG_Setup) DoMethod(obj,MUIM_Group_InitChange);

        if (value)
        {
            DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)bt->obj);
            bt->flags |= BFLG_Hide;
            res = TRUE;
        }
        else
        {
            bt->flags &= ~BFLG_Hide;
            DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)bt->obj);

            if (orderButtons(cl,obj,data)) res = TRUE;
            else
            {
                DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)bt->obj);
                bt->flags |= BFLG_Hide;
            }

        }

        if (data->flags & FLG_Setup)
        {
            if (data->flags & FLG_FreeStrip) DoMethod(obj,MUIM_TheButton_Build);
            DoMethod(obj,MUIM_Group_ExitChange);
        }
    }
    else res = TRUE;

    RETURN(res);
    return res;
}

/***********************************************************************/

static ULONG
sleepButton(struct IClass *cl, Object *obj, struct InstData *data, struct Button *bt, ULONG value)
{
  ULONG res = FALSE;

  ENTER();

  D(DBF_STARTUP, "sleepButton: %d", value);

  if(!BOOLSAME(value,bt->flags & BFLG_Sleep))
  {
    // should the button put to sleep or awakend
    if(value)
    {
      struct MinList *notifyList;

      // remove the button object from the group
      if(!(bt->flags & BFLG_Hide))
      {
        if(data->flags & FLG_Setup)
          DoMethod(obj, MUIM_Group_InitChange);

        DoSuperMethod(cl, obj, OM_REMMEMBER, (ULONG)bt->obj);

        if(data->flags & FLG_Setup)
          DoMethod(obj, MUIM_Group_ExitChange);
      }

      bt->flags &= ~(BFLG_Disabled|BFLG_Selected);

      if(xget(bt->obj, MUIA_Disabled))
        bt->flags |= BFLG_Disabled;

      if(xget(bt->obj, MUIA_Selected))
        bt->flags |= BFLG_Selected;

      // before we finally put the button to sleep we have to create a clone
      // of the notifyList it currently maintains.
      if((notifyList = (struct MinList *)xget(bt->obj, MUIA_TheButton_NotifyList)))
      {
        struct MinNode *node;

        // now we cleanup the old clone list
        NewList((struct List *)&bt->notifyListClone);

        // walk through the notifyList of the button and clone it
        // as we walk by
        for(node = notifyList->mlh_Head; node->mln_Succ; node = node->mln_Succ)
        {
          struct ButtonNotify *notify = (struct ButtonNotify *)node;
          struct ButtonNotify *clone;
          ULONG size;

          size = sizeof(struct ButtonNotify)+sizeof(ULONG)*notify->msg.FollowParams;

          // clone
          if((clone = allocVecPooled(data->pool, size)))
          {
            // copy the data of the notify
            memcpy(clone, notify, size);

            // add it to our clone list
            AddTail((struct List *)&bt->notifyListClone, (struct Node *)clone);
          }
          else
            break;
        }
      }

      MUI_DisposeObject(bt->obj);
      bt->obj = NULL;

      bt->flags |= BFLG_Sleep;

      res = TRUE;
    }
    else // bring the button back from sleep
    {
      if((bt->obj = makeButton(bt, obj, data)))
      {
        SetAttrs(bt->obj, MUIA_TheButton_TheBar, (ULONG)obj,
                          MUIA_Group_Horiz,      data->flags & FLG_Horiz,
                          TAG_DONE);

        if(bt->exclude)
          DoMethod(bt->obj, MUIM_Notify, MUIA_Selected, TRUE, (ULONG)obj, 3, MUIM_Set, MUIA_TheBar_Active, bt->ID);

        if(bt->flags & BFLG_Hide)
          res = TRUE;
        else
        {
          if(data->flags & FLG_Setup)
            DoMethod(obj, MUIM_Group_InitChange);

          DoSuperMethod(cl, obj, OM_ADDMEMBER, (ULONG)bt->obj);

          // put the buttons in order
          if(orderButtons(cl, obj, data))
            res = TRUE;
          else
          {
            // otherwise remove it again
            DoSuperMethod(cl, obj, OM_REMMEMBER,(ULONG)bt->obj);
            MUI_DisposeObject(bt->obj);
            bt->obj = NULL;
          }

          if(data->flags & FLG_Setup)
          {
            if(data->flags & FLG_FreeStrip)
              DoMethod(obj,MUIM_TheButton_Build);

            DoMethod(obj,MUIM_Group_ExitChange);
          }

          // make the button the active one
          if(res && bt->exclude && (bt->flags & BFLG_Selected))
            set(obj, MUIA_TheBar_Active, bt->ID);
        }

        if(res)
        {
          struct MinNode *node;

          // now we put the notifies from our clone list
          // back active for the new button
          for(node = bt->notifyListClone.mlh_Head; node->mln_Succ; )
          {
            struct ButtonNotify *notify = (struct ButtonNotify *)node;

            node = node->mln_Succ;

            // now we set the notify as we have identifed the button
            DoMethodA(bt->obj, (Msg)&notify->msg);

            // remove the clone notify from the list
            Remove((struct Node *)notify);

            // free the clone notify
            freeVecPooled(data->pool, notify);
          }

          // remove the sleep flag
          bt->flags &= ~BFLG_Sleep;
        }
      }
    }
  }
  else
    res = TRUE;

  RETURN(res);
  return res;
}

/***********************************************************************/

static ULONG
mSetAttr(struct IClass *cl,Object *obj,struct MUIP_TheBar_SetAttr *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct Button *bt;
    ULONG         res = FALSE;

    ENTER();

    if((bt = findButton(data,msg->ID)))
    {
        ULONG value = msg->value;

        switch (msg->attr)
        {
            case MUIA_TheBar_Attr_Hide:
                res = hideButton(cl,obj,data,bt,value);
                break;

            case MUIA_TheBar_Attr_Sleep:
                res = sleepButton(cl,obj,data,bt,value);
                break;

            case MUIA_TheBar_Attr_Disabled:
                if (bt->obj) set(bt->obj,MUIA_Disabled,value);
                if (value) bt->flags |= BFLG_Disabled;
                else bt->flags &= ~BFLG_Disabled;
                res = TRUE;
                break;

            case MUIA_TheBar_Attr_Selected:
                if (bt->exclude) set(obj,MUIA_TheBar_Active,bt->ID);
                else if (bt->obj) set(bt->obj,MUIA_Selected,value);
                if (value) bt->flags |= BFLG_Selected;
                else bt->flags &= ~BFLG_Selected;
                res = TRUE;
                break;
        }
    }

    RETURN(res);
    return res;
}

/***********************************************************************/

static ULONG
mRemove(struct IClass *cl, Object *obj, struct MUIP_TheBar_Remove *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct Button *button;
  BOOL result = FALSE;

  ENTER();

  if((button = findButton(data, msg->ID)))
  {
    if(button->obj)
    {
      if(data->flags & FLG_Setup)
        DoMethod(obj, MUIM_Group_InitChange);

      removeButton(cl, obj, button);

      if(data->flags & FLG_Setup)
        DoMethod(obj, MUIM_Group_ExitChange);
    }
    else
      removeButton(cl, obj, button);

    result = TRUE;
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mGetObject(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetObject *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct Button *button;
  Object *result;

  ENTER();

  result = (button = findButton(data,msg->ID)) ? button->obj : NULL;

  RETURN((ULONG)result);
  return (ULONG)result;
}

/***********************************************************************/

static ULONG
mDoOnButton(struct IClass *cl,Object *obj,struct MUIP_TheBar_DoOnButton *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct Button *button;
  ULONG result;

  ENTER();

  result = ((button = findButton(data,msg->ID)) && !(button->flags & BFLG_Sleep)) ? DoMethodA(button->obj,(Msg)&msg->method) : 0;

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mClear(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;

  ENTER();

  if(data->flags & FLG_Setup)
    DoMethod(obj,MUIM_Group_InitChange);

  for(node = data->buttons.mlh_Head; node->mln_Succ; )
  {
    struct Button *button = (struct Button *)node;

    // advance further
    node = node->mln_Succ;

    removeButton(cl, obj, button);
  }

  if(data->flags & FLG_Setup)
    DoMethod(obj,MUIM_Group_ExitChange);


  RETURN(0);
  return 0;
}

/***********************************************************************/

static ULONG
mDeActivate(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct Button *button, *succ;

    ENTER();

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        if (button->flags & BFLG_Sleep) continue;
        set(button->obj,MUIA_TheButton_MouseOver,FALSE);
    }

    RETURN(0);
    return 0;
}

/***********************************************************************/

static ULONG
mSort(struct IClass *cl,Object *obj,struct MUIP_TheBar_Sort *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct MinList                  temp;
    struct MUIP_Group_Sort *smsg;
    struct Button          *button, *succ;
    Object                 **o;
    ULONG                  n;
    LONG                   *id;

    ENTER();

    /* Counts buttons */
    for(n = 0, button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (!(button->flags & (BFLG_Sleep|BFLG_Hide))) n++;

    if (data->db) n++;

    /* Alloc sort msg */
    if (n>STATICSORTSIZE) smsg = allocVecPooled(data->pool,sizeof(struct MUIP_Group_Sort)+sizeof(Object *)*(n+1));
    else smsg = (struct MUIP_Group_Sort *)&data->sortMsgID;
    if (!smsg)
    {
      RETURN(FALSE);
      return FALSE;
    }

    /* Insert DragBar */
    if (data->db)
    {
        smsg->obj[0] = data->db;
        o = smsg->obj+1;
    }
    else o = smsg->obj;

    NewList((struct List *)&temp);

    /* Insert message buttons */
    for (id = msg->obj; *id>=0; id++, o++)
    {
        if (!(button = findButton(data,*id)))
        {
            if(n>STATICSORTSIZE)
              freeVecPooled(data->pool,smsg);

            RETURN(FALSE);
            return FALSE;
        }

        Remove((struct Node *)button);
        AddHead((struct List *)&temp, (struct Node *)button);

        if (!(button->flags & (BFLG_Sleep|BFLG_Hide))) *o = button->obj;
    }

    /* Insert missing */
    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ, o++)
    {
        Remove((struct Node *)button);
        AddHead((struct List *)&temp, (struct Node *)button);
        if (!(button->flags & (BFLG_Sleep|BFLG_Hide))) *o = button->obj;
    }

    *o = NULL;

    /* Re-insert buttons */
    for(button = (struct Button *)(temp.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        Remove((struct Node *)button);
        AddHead((struct List *)&data->buttons, (struct Node *)button);
    }

    if (data->flags & FLG_Setup) DoMethod(obj,MUIM_Group_InitChange);
    DoSuperMethodA(cl,obj,(Msg)smsg);
    if (data->flags & FLG_Setup) DoMethod(obj,MUIM_Group_ExitChange);

    if (n>STATICSORTSIZE) freeVecPooled(data->pool,smsg);

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static ULONG
mGetDragImage(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetDragImage *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    ULONG result = 0;

    ENTER();

    if (data->dm)
    {
        if (BOOLSAME(data->flags & FLG_Horiz,msg->horiz))
        {
            data->di.width  = data->dm->width;
            data->di.height = data->dm->height;
        }
        else
        {
	        #ifdef __MORPHOS__
            data->di.width  = data->buttonWidth;
            data->di.height = data->buttonHeight;
            #else
            data->di.width  = data->buttonWidth+((data->flags & FLG_Framed) ? data->leftBarFrameSpacing+data->rightBarFrameSpacing+2 : 0);
            data->di.height = data->buttonHeight+((data->flags & FLG_Framed) ? data->topBarFrameSpacing+data->bottomBarFrameSpacing+2 : 0);
            #endif
        }

        data->di.di = data->dm;


        result = (ULONG)&data->di;
    }

    RETURN(result);
    return result;
}

/***********************************************************************/

static ULONG
mHandleEvent(struct IClass *cl, Object *obj, UNUSED struct MUIP_HandleEvent *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    struct Button *button, *succ;

    ENTER();

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        if (button->flags & BFLG_Sleep) continue;
        set(button->obj,MUIA_TheButton_MouseOver,FALSE);
    }

    RETURN(0);
    return 0;
}

/***********************************************************************/

DISPATCHER(_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW:                        return mNew(cl,obj,(APTR)msg);
    case OM_DISPOSE:                    return mDispose(cl,obj,(APTR)msg);
    case OM_GET:                        return mGet(cl,obj,(APTR)msg);
    case OM_SET:                        return mSets(cl,obj,(APTR)msg);

    case MUIM_Setup:                    return mSetup(cl,obj,(APTR)msg);
    case MUIM_Cleanup:                  return mCleanup(cl,obj,(APTR)msg);
    case MUIM_Show:                     return mShow(cl,obj,(APTR)msg);
    case MUIM_Hide:                     return mHide(cl,obj,(APTR)msg);
    #ifndef __MORPHOS__
    case MUIM_Draw:                     return mDraw(cl,obj,(APTR)msg);
    #endif
    case MUIM_CustomBackfill:           return mCustomBackfill(cl,obj,(APTR)msg);
    case MUIM_CreateDragImage:          return mCreateDragImage(cl,obj,(APTR)msg);
    case MUIM_DeleteDragImage:          return mDeleteDragImage(cl,obj,(APTR)msg);
    case MUIM_HandleEvent:              return mHandleEvent(cl,obj,(APTR)msg);

    case MUIM_Group_InitChange:         return mInitChange(cl,obj,(APTR)msg);
    case MUIM_Group_ExitChange:         return mExitChange(cl,obj,(APTR)msg);

    case MUIM_TheBar_Rebuild:           return mRebuild(cl,obj,(APTR)msg);
    case MUIM_TheBar_AddButton:         return mAddButton(cl,obj,(APTR)msg);
    case MUIM_TheBar_GetAttr:           return mGetAttr(cl,obj,(APTR)msg);
    case MUIM_TheBar_SetAttr:           return mSetAttr(cl,obj,(APTR)msg);
    case MUIM_TheBar_Remove:            return mRemove(cl,obj,(APTR)msg);
    case MUIM_TheBar_GetObject:         return mGetObject(cl,obj,(APTR)msg);
    case MUIM_TheBar_DoOnButton:        return mDoOnButton(cl,obj,(APTR)msg);
    case MUIM_TheBar_Clear:             return mClear(cl,obj,(APTR)msg);
    case MUIM_TheBar_DeActivate:        return mDeActivate(cl,obj,(APTR)msg);
    case MUIM_TheBar_Sort:              return mSort(cl,obj,(APTR)msg);
    case MUIM_TheBar_GetDragImage:      return mGetDragImage(cl,obj,(APTR)msg);

    // notify methods
    case MUIM_TheBar_Notify:            return mNotify(cl, obj, (APTR)msg);
    case MUIM_TheBar_KillNotify:        return mKillNotify(cl, obj, (APTR)msg);

    default:                            return DoSuperMethodA(cl,obj,msg);
  }
}

/**********************************************************************/
