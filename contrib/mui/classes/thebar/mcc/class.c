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

#include <graphics/gfxbase.h>
#include "class.h"
#include "private.h"
#include "version.h"

/***********************************************************************/

#ifndef PDTA_AlphaChannel
/* does the image contain alpha channel data? */
#define PDTA_AlphaChannel     (DTA_Dummy + 256)
#endif

/***********************************************************************/

static Object *makeButton(struct Button *button,Object *obj,struct InstData *data)
{
    Object                   *o;
    struct MUIS_TheBar_Brush *brush, *sbrush, *dbrush;
    ULONG                    flags = data->flags, userFlags = data->userFlags, userFlags2 = data->userFlags2, bflags = button->flags, viewMode;
    struct TagItem           attrs[64], *tag;

    ENTER();

    if(button->img==MUIV_TheBar_BarSpacer)
    {
        Object *spacer;

        spacer = (Object *)spacerObject,
            MUIA_Group_Horiz, isFlagSet(flags, FLG_Horiz),
            isFlagSet(userFlags, UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
            MUIA_TheButton_TheBar, (IPTR)obj,
        End;

        RETURN(spacer);
        return(spacer);
    }
    else if(button->img==MUIV_TheBar_ButtonSpacer)
    {
      Object *spacer;

      spacer = (Object *)spacerObject,
               MUIA_Group_Horiz, isFlagSet(flags, FLG_Horiz),
               isFlagSet(flags, FLG_BarSpacer) ? TAG_IGNORE : MUIA_TheButton_Spacer, MUIV_TheButton_Spacer_Button,
               isFlagSet(userFlags, UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
               MUIA_TheButton_TheBar, (IPTR)obj,
      End;

      RETURN(spacer);
      return(spacer);
    }
    else if(button->img==MUIV_TheBar_ImageSpacer)
    {
      if(data->brushes &&((LONG)data->spacer>=0) && (!button->class) &&
         (o = (Object *)TheButtonObject,
               MUIA_TheButton_MinVer,                                       16,
               MUIA_Group_Horiz,                                            isFlagSet(flags, FLG_Horiz),
               MUIA_TheButton_TheBar,                                       (IPTR)obj,
               MUIA_TheButton_NoClick,                                      TRUE,
               MUIA_TheButton_Image,                                        (IPTR)data->brushes[data->spacer],
               data->sbrushes ? MUIA_TheButton_SelImage : TAG_IGNORE,       (IPTR)(data->sbrushes ? data->sbrushes[data->spacer] : NULL),
               data->dbrushes ? MUIA_TheButton_DisImage : TAG_IGNORE,       (IPTR)(data->dbrushes ? data->dbrushes[data->spacer] : NULL),
               MUIA_TheButton_ViewMode,                                     MUIV_TheButton_ViewMode_Gfx,
               MUIA_TheButton_Borderless,                                   TRUE,
               MUIA_TheButton_Raised,                                       FALSE,
               MUIA_TheButton_Sunny,                                        isFlagSet(flags, FLG_Sunny),
               MUIA_TheButton_Scaled,                                       isFlagSet(flags, FLG_Scaled),
               MUIA_TheButton_Spacer,                                       MUIV_TheButton_Spacer_Image,
               MUIA_TheButton_ID,                                           button->ID,
               isFlagSet(flags, FLG_FreeStrip) ? MUIA_TheButton_Strip : TAG_IGNORE, (IPTR)&data->strip,
          End))
      {
        RETURN(o);
        return o;
      }
      else if(data->brushes &&((LONG)data->spacer>=0) && (button->class) &&
         (o =  NewObject(button->class,NULL,
               MUIA_TheButton_MinVer,                                       16,
               MUIA_Group_Horiz,                                            isFlagSet(flags, FLG_Horiz),
               MUIA_TheButton_TheBar,                                       (IPTR)obj,
               MUIA_TheButton_NoClick,                                      TRUE,
               MUIA_TheButton_Image,                                        (IPTR)data->brushes[data->spacer],
               data->sbrushes ? MUIA_TheButton_SelImage : TAG_IGNORE,       (IPTR)(data->sbrushes ? data->sbrushes[data->spacer] : NULL),
               data->dbrushes ? MUIA_TheButton_DisImage : TAG_IGNORE,       (IPTR)(data->dbrushes ? data->dbrushes[data->spacer] : NULL),
               MUIA_TheButton_ViewMode,                                     MUIV_TheButton_ViewMode_Gfx,
               MUIA_TheButton_Borderless,                                   TRUE,
               MUIA_TheButton_Raised,                                       FALSE,
               MUIA_TheButton_Sunny,                                        isFlagSet(flags, FLG_Sunny),
               MUIA_TheButton_Scaled,                                       isFlagSet(flags, FLG_Scaled),
               MUIA_TheButton_Spacer,                                       MUIV_TheButton_Spacer_Image,
               MUIA_TheButton_ID,                                           button->ID,
               isFlagSet(flags, FLG_FreeStrip) ? MUIA_TheButton_Strip : TAG_IGNORE, (IPTR)&data->strip,
          TAG_DONE)))
      {
        RETURN(o);
        return o;
      }
      else
      {
        Object *spacer;

        spacer = (Object *)spacerObject,
                 MUIA_Group_Horiz, isFlagSet(flags, FLG_Horiz),
                 isFlagSet(userFlags, UFLG_UserBarSpacerSpacing) ? MUIA_TheBar_BarSpacerSpacing : TAG_IGNORE, data->barSpacerSpacing,
                 MUIA_TheButton_TheBar, (IPTR)obj,
               End;

        RETURN(spacer);
        return spacer;
      }
    }

    brush  = NULL;
    sbrush = NULL;
    dbrush = NULL;

    if (isFlagSet(flags, FLG_TextOnly))
    {
        viewMode = MUIV_TheBar_ViewMode_Text;
    }
    else
    {
        viewMode = data->viewMode;

        if (button->img<data->nbr-1)
        {
            brush = data->brushes[button->img];

            if (data->sbrushes)
                sbrush = data->sbrushes[button->img];

            if (data->dbrushes)
                dbrush = data->dbrushes[button->img];
        }
    }

    tag = attrs;

    #if defined(VIRTUAL)
    tag->ti_Tag   = MUIA_TheButton_InVirtgroup;
    tag->ti_Data    = TRUE;
    tag++;
    #endif

    tag->ti_Tag    = MUIA_Disabled;
    tag->ti_Data   = isFlagSet(bflags, BFLG_Disabled);
    tag++;

    tag->ti_Tag    = MUIA_Selected;
    tag->ti_Data   = isFlagSet(bflags, BFLG_Selected);
    tag++;

    tag->ti_Tag    = MUIA_Group_Horiz;
    tag->ti_Data   = isFlagSet(flags, FLG_Horiz);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_MinVer;
    tag->ti_Data   = 16;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_TheBar;
    tag->ti_Data   = (IPTR)obj;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Image;
    tag->ti_Data   = (IPTR)brush;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_SelImage;
    tag->ti_Data   = (IPTR)sbrush;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_DisImage;
    tag->ti_Data   = (IPTR)dbrush;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Label;
    tag->ti_Data   = (IPTR)button->text;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Immediate;
    tag->ti_Data   = isFlagSet(bflags, BFLG_Immediate);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Toggle;
    tag->ti_Data   = isFlagSet(bflags, BFLG_Toggle);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Borderless;
    tag->ti_Data   = isFlagSet(flags, FLG_Borderless);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Raised;
    tag->ti_Data   = isFlagSet(flags, FLG_Raised);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Sunny;
    tag->ti_Data   = isFlagSet(flags, FLG_Sunny);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_Scaled;
    tag->ti_Data   = isFlagSet(flags, FLG_Scaled);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_EnableKey;
    tag->ti_Data   = isFlagSet(flags, FLG_EnableKeys);
    tag++;

    tag->ti_Tag    = MUIA_TheButton_ViewMode;
    tag->ti_Data   = viewMode;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_LabelPos;
    tag->ti_Data   = data->labelPos;
    tag++;

    tag->ti_Tag    = MUIA_TheButton_ID;
    tag->ti_Data   = button->ID;
    tag++;

    if (button->help)
    {
        tag->ti_Tag    = MUIA_ShortHelp;
        tag->ti_Data   = (IPTR)button->help;
        tag++;
    }

    if (isFlagSet(flags, FLG_FreeStrip))
    {
        tag->ti_Tag    = MUIA_TheButton_Strip;
        tag->ti_Data   = (IPTR)&data->strip;
        tag++;
    }

    if (userFlags || userFlags2)
    {
        if (isFlagSet(userFlags, UFLG_UserHorizTextGfxSpacing))
        {
            tag->ti_Tag    = MUIA_TheButton_HorizTextGfxSpacing;
            tag->ti_Data   = data->horizTextGfxSpacing;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserVertTextGfxSpacing))
        {
            tag->ti_Tag    = MUIA_TheButton_VertTextGfxSpacing;
            tag->ti_Data   = data->vertTextGfxSpacing;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserHorizInnerSpacing))
        {
            tag->ti_Tag    = MUIA_TheButton_HorizInnerSpacing;
            tag->ti_Data   = data->horizInnerSpacing;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserTopInnerSpacing))
        {
            tag->ti_Tag    = MUIA_TheButton_TopInnerSpacing;
            tag->ti_Data   = data->topInnerSpacing;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserBottomInnerSpacing))
        {
            tag->ti_Tag    = MUIA_TheButton_BottomInnerSpacing;
            tag->ti_Data   = data->bottomInnerSpacing;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserPrecision))
        {
            tag->ti_Tag    = MUIA_TheButton_Precision;
            tag->ti_Data   = data->precision;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserDisMode))
        {
            tag->ti_Tag    = MUIA_TheButton_DisMode;
            tag->ti_Data   = data->disMode;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserScale))
        {
            tag->ti_Tag    = MUIA_TheButton_Scale;
            tag->ti_Data   = data->scale;
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserSpecialSelect))
        {
            tag->ti_Tag    = MUIA_TheButton_SpecialSelect;
            tag->ti_Data   = isFlagSet(userFlags, UFLG_SpecialSelect);
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserTextOverUseShine))
        {
            tag->ti_Tag    = MUIA_TheButton_TextOverUseShine;
            tag->ti_Data   = isFlagSet(userFlags, UFLG_TextOverUseShine);
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserIgnoreSelImages))
        {
            tag->ti_Tag    = MUIA_TheButton_IgnoreSelImages;
            tag->ti_Data   = isFlagSet(userFlags, UFLG_IgnoreSelImages);
            tag++;
        }

        if (isFlagSet(userFlags, UFLG_UserIgnoreDisImages))
        {
            tag->ti_Tag    = MUIA_TheButton_IgnoreDisImages;
            tag->ti_Data   = isFlagSet(userFlags, UFLG_IgnoreDisImages);
            tag++;
        }

        if (isFlagSet(userFlags2, UFLG2_UserDontMove))
        {
            tag->ti_Tag    = MUIA_TheButton_DontMove;
            tag->ti_Data   = isFlagSet(userFlags2, UFLG2_DontMove);
            tag++;
        }

        if (isFlagSet(userFlags2, UFLG2_NtRaiseActive))
        {
            tag->ti_Tag    = MUIA_TheButton_NtRaiseActive;
            tag->ti_Data   = isFlagSet(userFlags2, UFLG2_NtRaiseActive);
            tag++;
        }
    }

    tag->ti_Tag = TAG_DONE;

    // make sure we don't cause a buffer overrun
    ASSERT(((UBYTE *)tag - (UBYTE *)attrs) / sizeof(struct TagItem) <= sizeof(attrs) / sizeof(struct TagItem));

    o = button->class ? NewObjectA(button->class,NULL,attrs) :
                        MUI_NewObjectA((STRPTR)MUIC_TheButton,attrs);

    RETURN(o);
    return o;
}


/***********************************************************************/

static ULONG orderButtons(struct IClass *cl,Object *obj,struct InstData *data)
{
    struct MUIP_Group_Sort *smsg;
    struct Button          *button, *succ;
    Object                 **o;
    ULONG                  n;

    ENTER();

    for(n = 0, button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (isFlagClear(button->flags, BFLG_Sleep) && isFlagClear(button->flags, BFLG_Hide))
            n++;

    if (data->db) n++;

    if (n>STATICSORTSIZE)
        smsg = SharedAlloc(sizeof(struct MUIP_Group_Sort)+sizeof(Object *)*(n+1));
    else
        smsg = (struct MUIP_Group_Sort *)&data->sortMsgID;

    if(!smsg)
    {
      RETURN(FALSE);
      return FALSE;
    }

    // set the method ID for sorting
    smsg->MethodID = MUIM_Group_Sort;

    if (data->db)
    {
        smsg->obj[0] = data->db;
        o = smsg->obj+1;
    }
    else
        o = smsg->obj;

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (isFlagClear(button->flags, BFLG_Sleep) && isFlagClear(button->flags, BFLG_Hide))
            *o++ = button->obj;

    *o = NULL;

    DoSuperMethodA(cl,obj,(Msg)smsg);

    if (n>STATICSORTSIZE)
        SharedFree(smsg);

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

HOOKPROTONH(LayoutFunc, ULONG, Object *obj, struct MUI_LayoutMsg *lm)
{
    struct InstData *data = INST_DATA(lib_thisClass->mcc_Class,obj);

    ENTER();

    switch (lm->lm_Type)
    {
        case MUILM_MINMAX:
        {
            Object *child, *cstate;
            ULONG  horiz = isFlagSet(data->flags, FLG_Horiz) || isFlagSet(data->flags, FLG_Table);
            ULONG  test;
            LONG   cols = 0, rows = 0, numBut, c;
            UWORD  butMaxMinWidth, butMaxMinHeight, maxMinWidth, maxMinHeight, width, height, addSpace;

            butMaxMinWidth  = butMaxMinHeight = numBut = 0;

            for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
            {
                if (xget(child, MUIA_ShowMe) == FALSE || xget(child,MUIA_TheButton_Spacer) != MUIV_TheButton_Spacer_None)
                    continue;

                if ((butMaxMinWidth<MUI_MAXMAX) && (_minwidth(child)>butMaxMinWidth))
                    butMaxMinWidth = _minwidth(child);

                if ((butMaxMinHeight<MUI_MAXMAX) && (_minheight(child)>butMaxMinHeight))
                    butMaxMinHeight = _minheight(child);

                numBut++;
            }

            data->buttonWidth  = butMaxMinWidth;
            data->buttonHeight = butMaxMinHeight;

            if (isFlagSet(data->flags, FLG_Table))
            {
                if (data->cols)
                {
                    cols = data->cols;
                    if (cols<=0 || cols>numBut)
                        cols = numBut;

                    rows = numBut/cols;
                    if (rows == 0)
                        rows = 1;
                    else if (numBut-cols*rows>0)
                        rows++;
                }
                else
                {
                    rows = data->rows;
                    if (rows<=0 || rows>numBut)
                        rows = numBut;

                    cols = numBut/rows;
                    if (cols == 0)
                        cols = 1;
                    else if (numBut-cols*rows>0)
                        cols++;
                }
            }

            maxMinWidth = maxMinHeight = width = height = addSpace = 0;
            c = 0;
            test = FALSE;

            for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
            {
                UWORD w = 0;

                if (xget(child,MUIA_ShowMe) == FALSE)
                    continue;

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

                                case MUIV_TheBar_SpacersSize_None:
                                  w = 0;
                                break;

                                case MUIV_TheBar_SpacersSize_OnePoint:
                                  w = 1;
                                break;

                                case MUIV_TheBar_SpacersSize_TwoPoint:
                                  w = 2;
                                break;

                                default:
                                  if(isFlagSet(data->spacersSize, MUIV_TheBar_SpacersSize_PointsFlag))
                                    w = MUIV_TheBar_SpacersSize_GetPoints(data->spacersSize);
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
                            if (width>maxMinWidth)
                                maxMinWidth = width;

                            width = 0;
                            test = FALSE;
                            c = 0;
                            addSpace = 0;
                        }

                        if (addSpace>1)
                            width += data->horizSpacing;
                        width += w;

                        if (isFlagSet(data->flags, FLG_Table) && (++c>=cols))
                            test = TRUE;
                    }
                    else
                        width += w;
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

                                case MUIV_TheBar_SpacersSize_None:
                                  w = 0;
                                break;

                                case MUIV_TheBar_SpacersSize_OnePoint:
                                  w = 1;
                                break;

                                case MUIV_TheBar_SpacersSize_TwoPoint:
                                  w = 2;
                                break;

                                default:
                                  if(isFlagSet(data->spacersSize, MUIV_TheBar_SpacersSize_PointsFlag))
                                    w = MUIV_TheBar_SpacersSize_GetPoints(data->spacersSize);
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

                    if (addSpace>1)
                        height += data->vertSpacing;
                }
            }

            if (horiz)
            {
                if (isFlagSet(data->flags, FLG_Table))
                {
                    if (maxMinWidth==0)
                        maxMinWidth = width;

                    lm->lm_MinMax.MinWidth  = maxMinWidth;
                    lm->lm_MinMax.MinHeight = rows*(butMaxMinHeight+data->vertSpacing)-data->vertSpacing;

                    if(data->db)
                    {
                        if(data->cols)
                            lm->lm_MinMax.MinWidth += _minwidth(data->db)+data->horizSpacing;
                        else
                            lm->lm_MinMax.MinHeight += _minheight(data->db)+data->vertSpacing;

                        if(isFlagClear(lib_flags, BASEFLG_MUI4))
							lm->lm_MinMax.MinHeight += isFlagSet(data->flags, FLG_Framed) ? 4 : 0;
                    }
                }
                else
                {
                    if (data->db)
                    {
                        width += _minwidth(data->db)+data->horizSpacing;
                        if(isFlagClear(lib_flags, BASEFLG_MUI4))
							width += isFlagSet(data->flags, FLG_Framed) ? 4 : 0;
                    }

                    lm->lm_MinMax.MinWidth  = width;
                    lm->lm_MinMax.MinHeight = butMaxMinHeight;
                }

                if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                {
                    lm->lm_MinMax.MinWidth  += data->leftBarFrameSpacing+data->rightBarFrameSpacing+2;
                    lm->lm_MinMax.MinHeight += data->topBarFrameSpacing+data->bottomBarFrameSpacing+2;
                }

                lm->lm_MinMax.MaxWidth  = isFlagSet(data->flags, FLG_FreeHoriz) ? MUI_MAXMAX : lm->lm_MinMax.MinWidth;
                lm->lm_MinMax.MaxHeight = isFlagSet(data->flags, FLG_FreeVert)  ? MUI_MAXMAX : lm->lm_MinMax.MinHeight;
            }
            else
            {
                if (data->db)
                {
                    height += _minheight(data->db)+data->vertSpacing;

                    if(isFlagClear(lib_flags, BASEFLG_MUI4))
						height += isFlagSet(data->flags, FLG_Framed) ? 4 : 0;
                }

                lm->lm_MinMax.MinWidth  = butMaxMinWidth;
                lm->lm_MinMax.MinHeight = height;

                if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                {
                    lm->lm_MinMax.MinWidth  += data->leftBarFrameSpacing+data->rightBarFrameSpacing+2;
                    lm->lm_MinMax.MinHeight += data->topBarFrameSpacing+data->bottomBarFrameSpacing+2;
                }

                lm->lm_MinMax.MaxWidth  = isFlagSet(data->flags, FLG_FreeHoriz) ? MUI_MAXMAX : lm->lm_MinMax.MinWidth;
                lm->lm_MinMax.MaxHeight = isFlagSet(data->flags, FLG_FreeVert)  ? MUI_MAXMAX : lm->lm_MinMax.MinHeight;
            }

            lm->lm_MinMax.DefWidth  = lm->lm_MinMax.MinWidth;
            lm->lm_MinMax.DefHeight = lm->lm_MinMax.MinHeight;

            data->width  = lm->lm_MinMax.MinWidth;
            data->height = lm->lm_MinMax.MinHeight;

            /* Please check what was commented */
            #if defined(VIRTUAL)
              //#if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
              data->objWidth   = lm->lm_MinMax.MinWidth;
              data->objMWidth  = lm->lm_MinMax.MaxWidth;
              data->objHeight  = lm->lm_MinMax.MinHeight;
              data->objMHeight = lm->lm_MinMax.MaxHeight;
              //#else
              /*if(isFlagClear(data->flags, FLG_Framed))
              {
                data->objWidth  = lm->lm_MinMax.MinWidth;
                data->objMWidth = lm->lm_MinMax.MaxWidth;
                data->objHeight = lm->lm_MinMax.MinHeight;
                data->objMeight = lm->lm_MinMax.MaxHeight;
              }
              #endif*/
            #endif // VIRTUAL

            data->lcols = cols;
            data->lrows = rows;

            RETURN(0);
            return 0;
        }

        case MUILM_LAYOUT:
        {
            ULONG horiz = isFlagSet(data->flags, FLG_Horiz) || isFlagSet(data->flags, FLG_Table);

            #if defined(VIRTUAL)
            lm->lm_Layout.Width  = data->objWidth;
            lm->lm_Layout.Height = data->objHeight;
            #endif

            if (horiz)
            {
                Object *child, *cstate;
                int    x, sx, sy = 0, cols = data->lcols, c = 0, r = 0;
                ULONG  spc = data->horizSpacing;

                switch (data->barPos)
                {
                    case MUIV_TheBar_BarPos_Center:
                        #ifdef VIRTUAL
                        x = (_mwidth(obj)>(int)data->width) ? (_mwidth(obj)-data->width)>>1 : 0;
                        #else
                        x = (_mwidth(obj)-_minwidth(obj))>>1;
                        #endif
                        break;

                    case MUIV_TheBar_BarPos_Right:
                        #ifdef VIRTUAL
                        x = (_mwidth(obj)>(int)data->width) ? _mwidth(obj)-data->width : 0;
                        #else
                        x = _mwidth(obj)-_minwidth(obj);
                        #endif
                        break;

                    default:
                        x = 0;
                        break;
                }

                if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                    x += data->leftBarFrameSpacing+1;

                sx = x;

                for(cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
                {
                    ULONG width = 0, height = 0, y, d = 0, test = TRUE; // gcc

                    if (!xget(child,MUIA_ShowMe))
                        continue;

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
                            if (x)
                                x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_Button:
                            width = data->buttonWidth;
                            switch (data->spacersSize)
                            {
                                case MUIV_TheBar_SpacersSize_Quarter:
                                  width /= 4;
                                break;

                                case MUIV_TheBar_SpacersSize_Half:
                                  width /= 2;
                                break;

                                case MUIV_TheBar_SpacersSize_None:
                                  width = 0;
                                break;

                                case MUIV_TheBar_SpacersSize_OnePoint:
                                  width = 1;
                                break;

                                case MUIV_TheBar_SpacersSize_TwoPoint:
                                  width = 2;
                                break;

                                default:
                                  if(isFlagSet(data->spacersSize, MUIV_TheBar_SpacersSize_PointsFlag))
                                    width = MUIV_TheBar_SpacersSize_GetPoints(data->spacersSize);
                                break;
                            }
                            height = data->buttonHeight;
                            if (x)
                                x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_Image:
                            width  = _minwidth(child);
                            height = _minheight(child);
                            if (x)
                                x -= spc;
                            test = FALSE;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                            if (data->cols)
                            {
                                ULONG xx;

                                width  = _minwidth(child);
                                height = _mheight(obj);

                                #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                                xx = y = 0;
                                #else
                                if (isFlagSet(data->flags, FLG_Framed))
                                {
                                    height -= 4;
                                    xx = 2;
                                    y  = 2;
                                }
                                else
                                    xx = y = 0;
                                //height -= isFlagSet(data->flags, FLG_Framed) ? data->topBarFrameSpacing+data->bottomBarFrameSpacing+2 : 0;
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
                            {
                                if (data->rows)
                                {
                                    ULONG xx;

                                    width  = _mwidth(obj);
                                    height = _minheight(child);

                                    #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                                    xx = y = 0;
                                    #else
                                    if (isFlagSet(data->flags, FLG_Framed))
                                    {
                                        width -= 4;
                                        xx = 2;
                                        y  = 2;
                                    }
                                    else
                                        xx = y = 0;
                                    //width  -= isFlagSet(data->flags, FLG_Framed) ? data->leftBarFrameSpacing+data->rightBarFrameSpacing+2 : 0;
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

                                    #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                                    xx = y = 0;
                                    #else
                                    if (isFlagSet(data->flags, FLG_Framed))
                                    {
                                        height -= 4;
                                        xx = 2;
                                        y  = 2;
                                    }
                                    else
                                        xx = y = 0;
                                    //height -= isFlagSet(data->flags, FLG_Framed) ? data->topBarFrameSpacing+data->bottomBarFrameSpacing+2 : 0;
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
                            break;
                    }

                    y = sy;

                    if (height<data->buttonHeight)
                        y += (data->buttonHeight-height)>>1;

                    if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                        y += data->topBarFrameSpacing+1;

                    if (isFlagSet(data->flags, FLG_Table))
                    {
                        if (test && (c++==cols))
                        {
                            c = 1;
                            r++;
                            x = sx;
                        }

                        y += r*(data->buttonHeight+data->vertSpacing);
                    }

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
                Object *child, *cstate;
                int    y;
                ULONG  spc = data->vertSpacing;

                switch (data->barPos)
                {
                    case MUIV_TheBar_BarPos_Center:
                        #ifdef VIRTUAL
                        y = (_mheight(obj)>(int)data->height) ? (_mheight(obj)-data->height)>>1 : 0;
                        #else
                        y = (_height(obj)-_minheight(obj))>>1;
                        #endif
                        break;

                    case MUIV_TheBar_BarPos_Right:
                        #ifdef VIRTUAL
                        y = (_mheight(obj)>(int)data->height) ? _mheight(obj)-data->height : 0;
                        #else
                        y = _height(obj)-_minheight(obj);
                        #endif
                        break;

                    default:
                        y = 0;
                        break;
                }

                if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                    y += data->topBarFrameSpacing+1;

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
                            if (y)
                                y -= spc;
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

                                case MUIV_TheBar_SpacersSize_None:
                                  height = 0;
                                break;

                                case MUIV_TheBar_SpacersSize_OnePoint:
                                  height = 1;
                                break;

                                case MUIV_TheBar_SpacersSize_TwoPoint:
                                  height = 2;
                                break;

                                default:
                                  if(isFlagSet(data->spacersSize, MUIV_TheBar_SpacersSize_PointsFlag))
                                    height = MUIV_TheBar_SpacersSize_GetPoints(data->spacersSize);
                                break;
                            }
                            width  = data->buttonWidth;
                            if (y)
                                y -= spc;
                            break;

                        case MUIV_TheButton_Spacer_Image:
                            width  = _minwidth(child);
                            height = _minheight(child);
                            if (y)
                                y -= spc;
                            break;

                        case MUIV_TheButton_Spacer_DragBar:
                        {
                            ULONG yy;

                            height = _minheight(child);
                            width  = _mwidth(obj);

                            #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                            x = yy = 0;
                            #else
                            if (isFlagSet(data->flags, FLG_Framed))
                            {
                              width -= 4;
                              x  = 2;
                              yy = 2;
                            }
                            else
                              x = yy = 0;
                            //width -= isFlagSet(data->flags, FLG_Framed) ? data->leftBarFrameSpacing+data->rightBarFrameSpacing+2 : 0;
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

                    if (width<data->buttonWidth)
                        x = (data->buttonWidth - _minwidth(child)) >> 1;
                    else
                        x = 0;

                    if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
                        x += data->leftBarFrameSpacing+1;

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

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
#define RPL8(rp, xstart, ystart, width, array, temprp) ReadPixelLine8(rp, xstart, ystart, width, array, temprp)
#else // __amigaos4 || __MORPHOS__ || __AROS__
static LONG _ReadPixelLine8(struct RastPort *rp, UWORD xstart, UWORD ystart, UWORD width, UBYTE *array, UNUSED struct RastPort *temprp)
{
  UWORD x;
  UBYTE *a = array;

  for(x = 0; x < width; x++)
  {
    *a++ = ReadPixel(rp, x+xstart, ystart);
  }

  return (LONG)(a-array);
}

#define RPL8(rp, xstart, ystart, width, array, temprp) \
{ \
  if(GfxBase->LibNode.lib_Version > 40) \
    ReadPixelLine8(rp, xstart, ystart, width, array, temprp); \
  else \
    _ReadPixelLine8(rp, xstart, ystart, width, array, temprp); \
}
#endif // __amigaos4 || __MORPHOS__ || __AROS__

static BOOL loadDTBrush(struct MUIS_TheBar_Brush *brush,STRPTR file)
{
    Object *dto;
    BOOL res = FALSE;

    ENTER();

    memset(brush,0,sizeof(struct MUIS_TheBar_Brush));

    if((dto = NewDTObject(file,DTA_GroupID,    GID_PICTURE,
                               DTA_SourceType, DTST_FILE,
                               PDTA_Remap,     FALSE,
                               PDTA_DestMode,  PMODE_V43,
                               TAG_DONE)))
    {
        struct BitMapHeader *bmh;
        ULONG               *colors;
        IPTR                 numColors;

        if (GetDTAttrs(dto,PDTA_CRegs,(IPTR)&colors,PDTA_NumColors,(IPTR)&numColors,PDTA_BitMapHeader,(IPTR)&bmh,TAG_DONE)==3)
        {
            UBYTE *chunky;
            ULONG width, height, tw, size, csize;
            BOOL trueColor;

            width  = bmh->bmh_Width;
            height = bmh->bmh_Height;
            trueColor = bmh->bmh_Depth>8;

            tw    = ((width+15)>>4)<<4;
            size  = tw*height;
            csize = 0;

            if (trueColor)
                size *= 4;
            else
            {
                if (colors && numColors)
                {
                    csize = numColors*sizeof(ULONG)*3;
                }
            }

            if((chunky = SharedAlloc(size+csize)))
            {
                BOOL line8 = FALSE;

                if (trueColor)
                {
                  res = DoMethod(dto,PDTM_READPIXELARRAY,(IPTR)chunky,PBPAFMT_ARGB,width<<2,0,0,width,height);

                  #if !defined(__amigaos4__) && !defined(__AROS__)
                  // ignore the return code for mos broken pdt
                  if (isFlagSet(lib_flags,BASEFLG_BROKENMOSPDT))
                    res = TRUE;
                  #endif

                  #ifdef __AROS__
                  // ignore the return code for AROS
                  res = TRUE;
                  #endif
                }
                else
                {
                  // NOPE AROSMAN. YOU CAN'T IGNORE THIS ONE; JUST SKIP to ReadLine8()!
                  #ifndef __AROS__
                  res = DoMethod(dto,PDTM_READPIXELARRAY,(IPTR)chunky,PBPAFMT_LUT8,width,0,0,width,height);
                  #endif
                }

                if (!res)
                {
                    if (DoDTMethod(dto,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE))
                    {
                        struct BitMap *bm;

                        if (GetDTAttrs(dto,PDTA_BitMap,(IPTR)&bm,TAG_DONE))
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
                                    RPL8(&rp,0,y,width,dest,&trp);
                                    dest += tw;
                                }

                                WaitBlit();
                                FreeBitMap(trp.BitMap);

                                res = TRUE;
                                line8 = TRUE;
                            }
                        }
                    }
                }
                else
                    line8 = FALSE;

                if (res)
                {
                    brush->data = chunky;

                    brush->dataWidth  = brush->width  = width;
                    brush->dataHeight = brush->height = height;

                    if (line8)
                    {
                        brush->dataTotalWidth = tw;

                        if (colors && numColors)
                            memcpy(brush->colors = (ULONG *)(chunky+size),colors,csize);
                        brush->numColors = numColors;

                        if (bmh->bmh_Masking==mskHasTransparentColor)
                            brush->trColor = bmh->bmh_Transparent;
                    }
                    else
                    {
                        brush->dataTotalWidth = width;

                        if (trueColor)
                        {
                            brush->flags = BRFLG_ARGB;
                            brush->dataTotalWidth *= 4;


                            if (bmh->bmh_Masking==mskHasTransparentColor)
                            {
                                ULONG tc = bmh->bmh_Transparent;

                                brush->trColor = ((colors[tc] & 0xFF000000)>>8) | ((colors[tc+1] & 0xFF000000)>>16) | ((colors[tc+2] & 0xFF000000)>>24);
                            }
                            #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                            else
                            {
                                // MorphOS, OS4 and AROS always deliver a proper alpha channel
                                setFlag(brush->flags, BRFLG_AlphaMask);
                            }
                            #else
                            {
                                // on OS3 it depends on the used picture.datatype
                                if(bmh->bmh_Masking == mskHasAlpha)
                                {
                                    // if the masking tells us about an alpha channel
                                    // then we use this information directly
                                    setFlag(brush->flags, BRFLG_AlphaMask);
                                }
                                else
                                {
                                    // otherwise we ask the DT object about an alpha channel
                                    APTR plane = NULL;
                                    IPTR hasAlpha = 0;

                                    if (GetDTAttrs(dto, PDTA_MaskPlane, (IPTR)&plane, TAG_DONE) && plane != NULL)
                                        setFlag(brush->flags, BRFLG_AlphaMask);
                                    else if (GetDTAttrs(dto, PDTA_AlphaChannel, (IPTR)&hasAlpha, TAG_DONE) && hasAlpha != 0)
                                        setFlag(brush->flags, BRFLG_AlphaMask);
                                }
                            }
                            #endif

                        }
                        else
                        {
                            if (colors && numColors)
                                memcpy(brush->colors = (ULONG *)(chunky+size),colors,csize);
                            brush->numColors = numColors;

                            if (bmh->bmh_Masking==mskHasTransparentColor)
                                brush->trColor = bmh->bmh_Transparent;
                        }
                    }
                }
                else
                    SharedFree(chunky);
            }
        }

        DisposeDTObject(dto);
    }

    RETURN(res);
    return res;
}

/***********************************************************************/

static struct Button *findButton(struct InstData *data, ULONG ID)
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

static void removeButton(struct IClass *cl, Object *obj, struct Button *button)
{
  ENTER();

  D(DBF_STARTUP, "freeing button %d: %08lx", button->ID, button);

  // remove the MUI object as well
  if(button->obj)
  {
    D(DBF_STARTUP, " freeing button's MUI object...");

    // if the button is not hided it might still be a member of
    // the thebar object hierarchy
    if(isFlagClear(button->flags, BFLG_Hide))
      DoSuperMethod(cl, obj, OM_REMMEMBER, (IPTR)button->obj);

    MUI_DisposeObject(button->obj);
    button->obj = NULL;
  }

  // cleanup the notifyListClone
  if(IsListEmpty((struct List *)&button->notifyListClone) == FALSE)
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
      SharedFree(notify);
    }
  }

  // remove the button from our list
  Remove((struct Node *)button);

  // free the buttons' occupied memory
  SharedFree(button);

  LEAVE();
}

/***********************************************************************/

static void updateRemove(struct IClass *cl,Object *obj,struct InstData *data)
{
    struct Button *button, *succ;

    ENTER();

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        ULONG sp;

        if (isFlagSet(button->flags, BFLG_Sleep))
            continue;

        sp = xget(button->obj,MUIA_TheButton_Spacer);

        if ((isFlagSet(data->remove, MUIV_TheBar_RemoveSpacers_Bar) && (sp==MUIV_TheButton_Spacer_Bar)) ||
            (isFlagSet(data->remove, MUIV_TheBar_RemoveSpacers_Button) && (sp==MUIV_TheButton_Spacer_Button)) ||
            (isFlagSet(data->remove, MUIV_TheBar_RemoveSpacers_Image) && (sp==MUIV_TheButton_Spacer_Image)))
        {
            if (isFlagClear(button->flags, BFLG_Hide))
            {
                DoSuperMethod(cl,obj,OM_REMMEMBER,(IPTR)button->obj);
                setFlag(button->flags, BFLG_Hide|BFLG_TableHide);
            }
        }
        else
            if (isFlagSet(button->flags, BFLG_TableHide))
            {
                clearFlag(button->flags, BFLG_Hide|BFLG_TableHide);
                DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)button->obj);

                if (!orderButtons(cl,obj,data))
                {
                    DoSuperMethod(cl,obj,OM_REMMEMBER,(IPTR)button->obj);
                    setFlag(button->flags, BFLG_Hide);
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
    ULONG                       userFrame;
};

enum
{
    PFLG_FreeHorizExists = 1<<0,
    PFLG_FreeVertExists  = 1<<1,
    PFLG_FreeHoriz       = 1<<2,
    PFLG_FreeVert        = 1<<3,
};

static const ULONG ptable[] =
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
    PACK_LONGBIT(TBTAGBASE,MUIA_TheBar_Frame,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Framed),
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

    PACK_NEWOFFSET(MUIA_Frame),
    PACK_LONGBIT(MUIA_Frame,MUIA_Frame,pack,userFlags2,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG2_UserFrame),
    PACK_ENTRY(MUIA_Frame,MUIA_Frame,pack,userFrame,PKCTRL_LONG|PKCTRL_PACKONLY),

    /* Alien: group */
    PACK_NEWOFFSET(MUIA_Group_Horiz),
    PACK_LONGBIT(MUIA_Group_Horiz,MUIA_Group_Horiz,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Horiz),

    PACK_ENDTABLE
};

static void showDimensionMismatchReq(CONST_STRPTR type, CONST_STRPTR file1, LONG w1, LONG h1, CONST_STRPTR file2, LONG w2, LONG h2)
{
  struct EasyStruct es;

  memset(&es, 0, sizeof(es));
  es.es_StructSize = sizeof(es);
  #if defined(VIRTUAL)
  es.es_Title = (STRPTR)"TheBarVirt.mcc image dimension mismatch";
  #else
  es.es_Title = (STRPTR)"TheBar.mcc image dimension mismatch";
  #endif
  es.es_TextFormat = (STRPTR)"The normal image\n"
                             "  '%s'\n"
                             "has dimension %3ldx%3ld.\n"
                             "The %s image\n"
                             "  '%s'\n"
                             "has dimension %3ldx%3ld.\n"
                             "\n"
                             "Please remove this image or replace\n"
                             "it by one with matching dimensions.\n"
                             "\n"
                             "A suitable %s image will now be generated\n"
                             "from the normal image.";
  es.es_GadgetFormat = (STRPTR)"Continue";
  EasyRequest(NULL, &es, NULL, file1, w1, h1, type, file2, w2, h2, type);
}

/*
** This is needed to perform a new loading sequence:
** 1. single images on disk
** 2. one strip on disk
** 3. single strip as static brush
** 4. static images as brushes
** This makes sense in normal operations.
*/

static BOOL makePicsFun(struct pack *pt,
                        BOOL dostrip,
                        struct MUIS_TheBar_Brush *sb,
                        struct MUIS_TheBar_Brush *ssb,
                        struct MUIS_TheBar_Brush *dsb,
                        UWORD *nbrPtr)
{
    BOOL pics = FALSE;
    UWORD nbr = 0;

    ENTER();

    sb->data  = NULL;
    ssb->data = NULL;
    dsb->data = NULL;

    if (dostrip == TRUE || pt->strip || pt->pics)
    {
        struct Process *me;
        struct Window  *win;
        BPTR           idrawer, odir = (BPTR)NULL;

        me  = (struct Process *)FindTask(NULL);
        win = me->pr_WindowPtr;
        me->pr_WindowPtr = (struct Window *)-1;

        if (pt->idrawer && (idrawer = Lock(pt->idrawer,SHARED_LOCK)))
            odir = CurrentDir(idrawer);
        else
            idrawer = (BPTR)NULL;

        if (dostrip == TRUE || pt->strip)
        {
            struct MUIS_TheBar_Button *b;
            ULONG                     brpsize, brsize, totsize;

            if (dostrip == TRUE)
            {
                memcpy(sb,pt->stripBrush,sizeof(*sb));

                if (pt->sstripBrush)
                    memcpy(ssb,pt->sstripBrush,sizeof(*ssb));

                if (pt->dstripBrush)
                    memcpy(dsb,pt->dstripBrush,sizeof(*dsb));
            }
            else
            {
                if (loadDTBrush(sb,pt->strip))
                {
                    if (pt->sstrip)
                        loadDTBrush(ssb,pt->sstrip);

                    if (pt->dstrip)
                        loadDTBrush(dsb,pt->dstrip);
                }
            }

            if (sb->data)
            {
                if (ssb->data && (sb->dataWidth!=ssb->dataWidth || sb->dataHeight!=ssb->dataHeight))
                {
                    SharedFree(ssb->data);
                    ssb->data = NULL;
                }

                if (dsb->data && (sb->dataWidth!=dsb->dataWidth || sb->dataHeight!=dsb->dataHeight))
                {
                    SharedFree(dsb->data);
                    dsb->data = NULL;
                }

                if ((b = pt->buttons))
                {
                    for (nbr = 0; b->img!=MUIV_TheBar_End; b++)
                    {
                        if (b->img!=MUIV_TheBar_BarSpacer)
                            nbr++;
                    }
                }
                else
                    nbr = 0;

                if (pt->stripCols<=0)  pt->stripCols   = nbr ? nbr : 1;
                if (pt->stripRows<=0)  pt->stripRows   = 1;
                if (pt->stripHSpace<0) pt->stripHSpace = 1;
                if (pt->stripVSpace<0) pt->stripVSpace = 1;
                nbr = pt->stripCols*pt->stripRows+1;

                brpsize = sizeof(struct MUIS_TheBar_Brush *)*nbr;
                brsize  = sizeof(struct MUIS_TheBar_Brush)*nbr;

                totsize = brpsize+brsize;
                if (ssb->data)
                    totsize += brpsize+brsize;
                if (dsb->data)
                    totsize += brpsize+brsize;

                if ((pt->brushes = SharedAlloc(totsize)))
                {
                    ULONG rows, cols, horizSpace, vertSpace;
                    int   w, h;

                    rows       = pt->stripRows;
                    cols       = pt->stripCols;
                    horizSpace = pt->stripHSpace;
                    vertSpace  = pt->stripVSpace;

                    w = (sb->dataWidth -(cols-1)*horizSpace)/cols;
                    h = (sb->dataHeight-(rows-1)*vertSpace)/rows;

                    if (w && h)
                    {
                        struct MUIS_TheBar_Brush *brush, *sbrush, *dbrush;
                        int                      i, x, vofs;

                        brush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->brushes+brpsize);

                        if (ssb->data)
                        {
                            pt->sbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            sbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->sbrushes+brpsize);
                        }
                        else sbrush = NULL;

                        if (dsb->data)
                        {
                            if (ssb->data) pt->dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)sbrush+brsize);
                            else pt->dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            dbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->dbrushes+brpsize);
                        }
                        else dbrush = NULL;

                        for (x = i = vofs = 0; i<(int)rows; i++, vofs += h+vertSpace)
                        {
                            int j, hofs;

                            for (j = hofs = 0; j<(int)cols; j++, hofs += w+horizSpace)
                            {
                                memcpy(pt->brushes[x] = brush,sb,sizeof(struct MUIS_TheBar_Brush));
                                brush->left   = hofs;
                                brush->top    = vofs;
                                brush->width  = w;
                                brush->height = h;
                                brush++;

                                if (sbrush)
                                {
                                    memcpy(pt->sbrushes[x] = sbrush,ssb,sizeof(struct MUIS_TheBar_Brush));
                                    sbrush->left   = hofs;
                                    sbrush->top    = vofs;
                                    sbrush->width  = w;
                                    sbrush->height = h;
                                    sbrush++;
                                }

                                if (dbrush)
                                {
                                    memcpy(pt->dbrushes[x] = dbrush,dsb,sizeof(struct MUIS_TheBar_Brush));
                                    dbrush->left   = hofs;
                                    dbrush->top    = vofs;
                                    dbrush->width  = w;
                                    dbrush->height = h;
                                    dbrush++;
                                }

                                x++;
                            }
                        }

                        pt->brushes[x] = NULL;
                        if (sbrush) pt->sbrushes[x] = NULL;
                        if (dbrush) pt->dbrushes[x] = NULL;

                        pics = TRUE;
                    }
                }

                if (pics)
                    setFlag(pt->flags, FLG_FreeStrip);
                else
                {
                    if (pt->brushes)
                    {
                        SharedFree(pt->brushes);

                        pt->brushes = NULL;
                        if (pt->sbrushes) pt->sbrushes = NULL;
                        if (pt->dbrushes) pt->dbrushes = NULL;
                    }
                }
            }
        }

        if (!pics)
        {
            if (pt->pics)
            {
                STRPTR *p;

                for (nbr = 0, p = pt->pics; *p; nbr++, p++);

                if (nbr)
                {
                    STRPTR *sp = NULL, *dp = NULL;
                    ULONG  brpsize, brsize, totsize, num;

                    if (pt->spics)
                    {
                        for (num = 0, p = pt->spics; *p; num++, p++);
                        if (nbr!=num) pt->spics = NULL;
                    }

                    if (pt->dpics)
                    {
                        for (num = 0, p = pt->dpics; *p; num++, p++);
                        if (nbr!=num) pt->dpics = NULL;
                    }

                    nbr++;

                    brpsize = sizeof(struct MUIS_TheBar_Brush *)*nbr;
                    brsize  = sizeof(struct MUIS_TheBar_Brush)*nbr;

                    totsize = brpsize+brsize;
                    if (pt->spics) totsize += brpsize+brsize;
                    if (pt->dpics) totsize += brpsize+brsize;

                    if ((pt->brushes = SharedAlloc(totsize)))
                    {
                        struct MUIS_TheBar_Brush *brush, *sbrush = NULL, *dbrush = NULL;
                        int                      i;

                        brush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->brushes+brpsize);

                        if (pt->spics)
                        {
                            pt->sbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);
                            sbrush       = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->sbrushes+brpsize);
                        }

                        if (pt->dpics)
                        {
                            if (pt->spics) pt->dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)sbrush+brsize);
                            else pt->dbrushes = (struct MUIS_TheBar_Brush **)((UBYTE *)brush+brsize);

                            dbrush = (struct MUIS_TheBar_Brush *)((UBYTE *)pt->dbrushes+brpsize);
                        }

                        p = pt->pics;
                        if (pt->sbrushes) sp = pt->spics;
                        if (pt->dbrushes) dp = pt->dpics;

                        for (i = 0; *p; i++, p++)
                        {
                            if (!loadDTBrush(pt->brushes[i] = brush+i,*p))
                                break;

                            if (pt->sbrushes)
                            {
                                if (*sp!=MUIV_TheBar_SkipPic)
                                {
                                    if (!loadDTBrush(pt->sbrushes[i] = sbrush+i,*sp))
                                    {
                                        pt->sbrushes[i] = NULL;
                                    }
                                    else if(pt->sbrushes[i]->dataWidth != pt->brushes[i]->dataWidth || pt->sbrushes[i]->dataHeight != pt->brushes[i]->dataHeight)
                                    {
										// the selected image's dimensions do not match the normal image's dimensions
										showDimensionMismatchReq("selected", *p, pt->brushes[i]->dataWidth, pt->brushes[i]->dataHeight, *sp, pt->sbrushes[i]->dataWidth, pt->sbrushes[i]->dataHeight);
										SharedFree(pt->sbrushes[i]->data);
										pt->sbrushes[i] = NULL;
									}
                                }
                                sp++;
                            }

                            if (pt->dbrushes)
                            {
                                if (*dp!=MUIV_TheBar_SkipPic)
                                {
                                    if (!loadDTBrush(pt->dbrushes[i] = dbrush+i,*dp))
                                    {
                                        pt->dbrushes[i] = NULL;
                                    }
                                    else if(pt->dbrushes[i]->dataWidth != pt->brushes[i]->dataWidth || pt->dbrushes[i]->dataHeight != pt->brushes[i]->dataHeight)
                                    {
										// the disabled image's dimensions do not match the normal image's dimensions
										showDimensionMismatchReq("disabled", *p, pt->brushes[i]->dataWidth, pt->brushes[i]->dataHeight, *dp, pt->dbrushes[i]->dataWidth, pt->dbrushes[i]->dataHeight);
										SharedFree(pt->dbrushes[i]->data);
										pt->dbrushes[i] = NULL;
									}
                                }
                                dp++;
                            }
                        }

                        if (*p)
                        {
                            SharedFree(pt->brushes);

                            pt->brushes = NULL;
                            if (pt->sbrushes) pt->sbrushes = NULL;
                            if (pt->dbrushes) pt->dbrushes = NULL;
                        }
                        else
                        {
                            pt->brushes[i] = NULL;
                            if (pt->sbrushes) pt->sbrushes[i] = NULL;
                            if (pt->dbrushes) pt->dbrushes[i] = NULL;

                            setFlag(pt->flags, FLG_FreeBrushes);
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

    if (pics == TRUE)
        *nbrPtr = nbr;

    RETURN(pics);
    return pics;
}

static BOOL makePics(struct pack *pt,
                     struct MUIS_TheBar_Brush *sb,
                     struct MUIS_TheBar_Brush *ssb,
                     struct MUIS_TheBar_Brush *dsb,
                     UWORD *nbrPtr)
{
    BOOL pics;

    ENTER();

    pics = makePicsFun(pt,FALSE,sb,ssb,dsb,nbrPtr);
    if (pics == FALSE && pt->stripBrush)
        pics = makePicsFun(pt,TRUE,sb,ssb,dsb,nbrPtr);

    if (pics == FALSE && pt->brushes)
    {
        struct MUIS_TheBar_Brush **brush;
        UWORD                    nbr;

        for (brush = pt->brushes, nbr = 0; *brush; nbr++, brush++);
        nbr++;

        *nbrPtr = nbr;
        pics = TRUE;
    }

    RETURN(pics);
    return pics;
}

static IPTR mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct pack               pt;
    struct MUIS_TheBar_Button *buttons;
    struct TagItem            *attrs = msg->ops_AttrList;
    struct MUIS_TheBar_Brush  sb, ssb, dsb;
    BOOL                      pics;
    UWORD                     nbr = 0;

    ENTER();

    if(GetTagData(MUIA_TheBar_MinVer, 0 ,attrs) > LIB_VERSION)
    {
      RETURN(0);
      return 0;
    }

    memset(&pt,0,sizeof(pt));

    // set some default values before we actually see what the
    // user set during creation of that object
    pt.labelPos = MUIV_TheBar_LabelPos_Bottom;
    pt.barPos   = MUIV_TheBar_BarPos_Left;
    pt.spacer   = pt.stripCols = pt.stripRows = pt.stripHSpace = pt.stripVSpace = -1;
    pt.flags    = FLG_EnableKeys;

    PackStructureTags(&pt,ptable,attrs);

    if (pt.viewMode>=MUIV_TheBar_ViewMode_Last)
        pt.viewMode = MUIV_TheBar_ViewMode_Text;
    if (pt.labelPos>=MUIV_TheBar_LabelPos_Last)
        pt.labelPos = MUIV_TheBar_LabelPos_Bottom;

    if (isFlagSet(pt.pflags, PFLG_FreeHorizExists) ? isFlagSet(pt.flags, PFLG_FreeHoriz) : isFlagSet(pt.flags, FLG_Horiz))
        setFlag(pt.flags, FLG_FreeHoriz);

    if (isFlagSet(pt.pflags, PFLG_FreeVertExists) ? isFlagSet(pt.flags, PFLG_FreeVert) : isFlagSet(pt.flags, FLG_Horiz) == 0)
        setFlag(pt.flags, FLG_FreeVert);

    pics = makePics(&pt,&sb,&ssb,&dsb,&nbr);

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Group_LayoutHook,   (IPTR)&LayoutHook,
            MUIA_Group_HorizSpacing, 0,
            MUIA_Group_VertSpacing,  0,
            isFlagSet(lib_flags, BASEFLG_MUI20) ? TAG_IGNORE : MUIA_CustomBackfill, TRUE,
            TAG_MORE,(IPTR)attrs)))
    {
        struct InstData *data = INST_DATA(cl, obj);

        data->brushes   = pt.brushes;
        data->sbrushes  = pt.sbrushes;
        data->dbrushes  = pt.dbrushes;
        data->viewMode  = pt.viewMode;
        data->flags     = pt.flags | (pics ? 0 : FLG_TextOnly);
        data->flags2    = pt.flags2;
        data->id        = pt.id;
        data->nbr       = nbr;
        data->labelPos  = pt.labelPos;
        data->barPos    = pt.barPos;
        data->remove    = pt.remove;
        data->cols      = pt.cols;
        data->rows      = pt.rows;
        #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
        data->userFrame = pt.userFrame;
        #endif

        if (isFlagSet(data->flags, FLG_FreeStrip))
        {
            if (sb.data)
                memcpy(&data->image,&sb,sizeof(data->image));
            if (ssb.data)
                memcpy(&data->simage,&ssb,sizeof(data->simage));
            if (dsb.data)
                memcpy(&data->dimage,&dsb,sizeof(data->dimage));
        }

        data->userFlags  = pt.userFlags;
        data->userFlags2 = pt.userFlags2;

        if (data->userFlags || data->userFlags2)
        {
            ULONG userFlags = data->userFlags, userFlags2 = data->userFlags2;

            if (isFlagSet(userFlags, UFLG_UserHorizSpacing))
            {
                if (pt.horizSpacing<=256)
                    data->horizSpacing = pt.horizSpacing;
                else
                    clearFlag(userFlags, UFLG_UserHorizSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserVertSpacing))
            {
                if (pt.vertSpacing<=256)
                    data->vertSpacing = pt.vertSpacing;
                else
                    clearFlag(userFlags, UFLG_UserVertSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserLeftBarFrameSpacing))
            {
                if (pt.leftBarFrameSpacing>0 && pt.leftBarFrameSpacing<=256)
                    data->leftBarFrameSpacing = pt.leftBarFrameSpacing;
                else
                    clearFlag(userFlags, UFLG_UserLeftBarFrameSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserRightBarFrameSpacing))
            {
                if (pt.rightBarFrameSpacing>0 && pt.rightBarFrameSpacing<=256)
                    data->rightBarFrameSpacing = pt.rightBarFrameSpacing;
                else
                    clearFlag(userFlags, UFLG_UserRightBarFrameSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserTopBarFrameSpacing))
            {
                if (pt.topBarFrameSpacing>0 && pt.topBarFrameSpacing<=256)
                    data->topBarFrameSpacing = pt.topBarFrameSpacing;
                else
                    clearFlag(userFlags, UFLG_UserTopBarFrameSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserBottomBarFrameSpacing))
            {
                if (pt.bottomBarFrameSpacing>0 && pt.bottomBarFrameSpacing<=256)
                    data->bottomBarFrameSpacing = pt.bottomBarFrameSpacing;
                else
                    clearFlag(userFlags, UFLG_UserBottomBarFrameSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserBarSpacerSpacing))
            {
                if (pt.barSpacerSpacing<=256)
                    data->barSpacerSpacing = pt.barSpacerSpacing;
                else
                    clearFlag(userFlags, UFLG_UserBarSpacerSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserHorizInnerSpacing))
            {
                if (pt.horizInnerSpacing>0 && pt.horizInnerSpacing<=256)
                    data->horizInnerSpacing = pt.horizInnerSpacing;
                else
                    clearFlag(userFlags, UFLG_UserHorizInnerSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserTopInnerSpacing))
            {
                if (pt.topInnerSpacing>0 && pt.topInnerSpacing<=256)
                    data->topInnerSpacing = pt.topInnerSpacing;
                else
                    clearFlag(userFlags, UFLG_UserTopInnerSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserBottomInnerSpacing))
            {
                if (pt.bottomInnerSpacing>0 && pt.bottomInnerSpacing<=256)
                    data->bottomInnerSpacing = pt.bottomInnerSpacing;
                else
                    clearFlag(userFlags, UFLG_UserBottomInnerSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserHorizTextGfxSpacing))
            {
                if (pt.horizTextGfxSpacing>0 && pt.horizTextGfxSpacing<=256)
                    data->horizTextGfxSpacing = pt.horizTextGfxSpacing;
                else
                    clearFlag(userFlags, UFLG_UserHorizTextGfxSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserVertTextGfxSpacing))
            {
                if (pt.vertTextGfxSpacing>0 && pt.vertTextGfxSpacing<=256)
                    data->vertTextGfxSpacing = pt.vertTextGfxSpacing;
                else
                    clearFlag(userFlags, UFLG_UserVertTextGfxSpacing);
            }

            if (isFlagSet(userFlags, UFLG_UserPrecision))
            {
                if (pt.precision<MUIV_TheBar_Precision_Last)
                    data->precision = pt.precision;
                else
                    clearFlag(userFlags, UFLG_UserPrecision);
            }

            if (isFlagSet(userFlags, UFLG_UserScale))
            {
                if (pt.scale>=25 && pt.scale<=400)
                    data->scale = pt.scale;
                else
                    clearFlag(userFlags, UFLG_UserScale);
            }

            if (isFlagSet(userFlags, UFLG_UserDisMode))
            {
                if (pt.disMode<MUIV_TheBar_DisMode_Last)
                {
                    data->disMode = pt.disMode;
                    if (data->disMode==MUIV_TheBar_DisMode_Sunny)
                        clearFlag(data->flags, FLG_Sunny);
                }
                else
                    clearFlag(userFlags, UFLG_UserDisMode);
            }

            if (isFlagSet(userFlags2, UFLG2_UserSpacersSize))
            {
                /*
                if (pt.spacersSize<MUIV_TheBar_SpacersSize_Last)
                    data->spacersSize = pt.spacersSize;
                else
                    clearFlag(userFlags2, UFLG2_UserSpacersSize);
                */
                if (MUIV_TheBar_SpacersSize_IsValid(pt.spacersSize))
                    data->spacersSize = pt.spacersSize;
                else
                    clearFlag(userFlags2, UFLG2_UserSpacersSize);
            }

            data->userFlags  = userFlags;
            data->userFlags2 = userFlags2;
        }

        if (pt.spacer<data->nbr-1)
            data->spacer = pt.spacer;
        else
            data->spacer = -1;

        NewList((struct List *)&data->buttons);

        if (isFlagSet(data->flags, FLG_DragBar))
        {
            if((data->db = (Object *)dragBarObject,
                    MUIA_Group_Horiz,      isFlagSet(data->flags, FLG_Horiz),
                    MUIA_TheButton_TheBar, (IPTR)obj,
                End))
            {
              DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)data->db);
            }
        }

        if((buttons = pt.buttons))
        {
            while (buttons->img!=MUIV_TheBar_End)
            {
                DoMethod(obj,MUIM_TheBar_AddButton,(IPTR)buttons);

                if (buttons->exclude && isFlagSet(buttons->flags, BFLG_Selected))
                    data->active = buttons->ID;

                buttons++;
            }
        }

        if (data->remove)
            updateRemove(cl,obj,data);

        if (data->cols || data->rows)
        {
            setFlag(data->flags, FLG_Table);
            if (isFlagSet(data->flags, FLG_DragBar))
                set(data->db,MUIA_Group_Horiz,data->cols);
        }

        #if defined(__amigaos3__)
        // cgx/WritePixelArrayAlpha is available in AfA only
        if(CyberGfxBase != NULL && CyberGfxBase->lib_Version >= 45 &&
           PictureDTBase != NULL && PictureDTBase->lib_Version >= 46)
          data->allowAlphaChannel = TRUE;
        #endif
    }

    RETURN((IPTR)obj);
    return (IPTR)obj;
}

/***********************************************************************/

static IPTR mDispose(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct MinNode *node;
  IPTR res;

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

  RETURN(res);
  return res;
}

/***********************************************************************/

static IPTR mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  BOOL result = FALSE;

  ENTER();

  switch(msg->opg_AttrID)
  {
    case MUIA_TheBar_MouseOver:        *msg->opg_Storage = data->id; result=TRUE; break;
    case MUIA_TheBar_Images:           *msg->opg_Storage = (IPTR)data->brushes; result=TRUE; break;
    case MUIA_TheBar_DisImages:        *msg->opg_Storage = (IPTR)data->dbrushes; result=TRUE; break;
    case MUIA_TheBar_SelImages:        *msg->opg_Storage = (IPTR)data->sbrushes; result=TRUE; break;
    case MUIA_TheBar_ViewMode:         *msg->opg_Storage = data->viewMode; result=TRUE; break;
    case MUIA_TheBar_Borderless:       *msg->opg_Storage = isFlagSet(data->flags, FLG_Borderless) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Raised:           *msg->opg_Storage = isFlagSet(data->flags, FLG_Raised) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Sunny:            *msg->opg_Storage = isFlagSet(data->flags, FLG_Sunny) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_Scaled:           *msg->opg_Storage = isFlagSet(data->flags, FLG_Scaled) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_SpacerIndex:      *msg->opg_Storage = data->spacer; result=TRUE; break;
    case MUIA_TheBar_EnableKeys:       *msg->opg_Storage = isFlagSet(data->flags, FLG_EnableKeys) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_TextOnly:         *msg->opg_Storage = isFlagSet(data->flags, FLG_TextOnly) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_LabelPos:         *msg->opg_Storage = data->labelPos; result=TRUE; break;
    case MUIA_TheBar_BarPos:           *msg->opg_Storage = data->barPos; result=TRUE; break;
    case MUIA_TheBar_Active:           *msg->opg_Storage = data->active; result=TRUE; break;
    case MUIA_TheBar_Columns:          *msg->opg_Storage = data->cols; result=TRUE; break;
    case MUIA_TheBar_Rows:             *msg->opg_Storage = data->rows; result=TRUE; break;
    case MUIA_TheBar_Free:             *msg->opg_Storage = (isFlagSet(data->flags, FLG_FreeHoriz) && isFlagSet(data->flags, FLG_FreeVert)) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_FreeHoriz:        *msg->opg_Storage = isFlagSet(data->flags, FLG_FreeHoriz) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_FreeVert:         *msg->opg_Storage = isFlagSet(data->flags, FLG_FreeVert) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_BarSpacer:        *msg->opg_Storage = isFlagSet(data->flags, FLG_BarSpacer) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_RemoveSpacers:    *msg->opg_Storage = data->remove; result=TRUE; break;
    case MUIA_TheBar_Frame:            *msg->opg_Storage = isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_DragBar:          *msg->opg_Storage = isFlagSet(data->flags, FLG_DragBar) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_IgnoreAppearance: *msg->opg_Storage = isFlagSet(data->flags2, FLG2_IgnoreAppearance) ? TRUE : FALSE; result=TRUE; break;
    case MUIA_TheBar_DisMode:          *msg->opg_Storage = data->disMode; result=TRUE; break;
    case MUIA_TheBar_NtRaiseActive:    *msg->opg_Storage = isFlagSet(data->userFlags2, UFLG2_NtRaiseActive) ? TRUE : FALSE; result=TRUE; break;

    case MUIA_Group_Horiz:             *msg->opg_Storage = isFlagSet(data->flags, FLG_Horiz) ? TRUE : FALSE; result=TRUE; break;

    case MUIA_Version:                 *msg->opg_Storage = LIB_VERSION; result=TRUE; break;
    case MUIA_Revision:                *msg->opg_Storage = LIB_REVISION; result=TRUE; break;

    default:                           result = DoSuperMethodA(cl,obj,(Msg)msg);
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static void allocateFramePens(Object *obj,struct InstData *data)
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

static void freeFramePens(Object *obj,struct InstData *data)
{
    ENTER();

    MUI_ReleasePen(muiRenderInfo(obj),data->barFrameShadowPen);
    MUI_ReleasePen(muiRenderInfo(obj),data->barFrameShinePen);

    LEAVE();
}

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

static IPTR mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    ULONG flags = 0, res;

    ENTER();

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

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
                        if (b==button)
                            continue;

                        if (isFlagSet(b->exclude,  (1U<<button->ID)))
                        {
                            if isFlagSet(button->flags, BFLG_Sleep)
                                clearFlag(b->flags, MUIV_TheBar_ButtonFlag_Selected);
                            else
                                set(button->obj,MUIA_Selected,FALSE);
                        }
                    }

                    if isFlagSet(b->flags, BFLG_Sleep)
                        setFlag(b->flags, MUIV_TheBar_ButtonFlag_Selected);
                    else
                        set(b->obj,MUIA_Selected,TRUE);
                }
                break;

            case MUIA_TheBar_MouseOver:
                if (data->id==tidata)
                    tag->ti_Tag = TAG_IGNORE;
                else
                    data->id =tidata;
                break;


            case MUIA_Group_Horiz:
                if (!BOOLSAME(tidata, isFlagSet(data->flags, FLG_Horiz)))
                {
                    if (tidata)
                    {
                        setFlag(data->flags, FLG_Horiz|FLG_FreeHoriz);
                        clearFlag(data->flags, FLG_FreeVert);
                    }
                    else
                    {
                        setFlag(data->flags, FLG_FreeVert);
                        clearFlag(data->flags, FLG_Horiz|FLG_FreeHoriz);
                    }

                    setFlag(flags, SFLG_Horiz);
                }
                break;

            case MUIA_TheBar_Frame:
                if(isFlagClear(lib_flags,BASEFLG_MUI4) && !BOOLSAME(tidata, isFlagSet(data->flags, FLG_Framed)))
                {
                    if (tidata)
                    {
                        if (isFlagSet(data->flags, FLG_Setup))
                            allocateFramePens(obj,data);

                        setFlag(data->flags, FLG_Framed);
                    }
                    else
                    {
                        if (isFlagSet(data->flags, FLG_Setup))
                            freeFramePens(obj,data);

                        clearFlag(data->flags, FLG_Framed);
                    }

                    setFlag(flags, SFLG_Frame);
                }
                break;

            case MUIA_TheBar_DragBar:
                if (!BOOLSAME(tidata, isFlagSet(data->flags, FLG_DragBar)))
                {
                    if (tidata)
                    {
                        setFlag(data->flags, FLG_DragBar);

                        if((data->db = (Object *)dragBarObject,
                                MUIA_Group_Horiz,      isFlagSet(data->flags, FLG_Horiz),
                                MUIA_TheButton_TheBar, (IPTR)obj,
                            End))
                        {
                            DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)data->db);
                            setFlag(flags, SFLG_Rebuild);
                        }
                    }
                    else
                    {
                        clearFlag(data->flags, FLG_DragBar);

                        DoSuperMethod(cl,obj,OM_REMMEMBER,(IPTR)data->db);
                        MUI_DisposeObject(data->db);
                        data->db = NULL;
                    }

                    setFlag(flags, SFLG_DragBar);
                }
                break;

            case MUIA_TheBar_Borderless:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_Borderless)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_Borderless);
                    else
                        clearFlag(data->flags, FLG_Borderless);

                    setFlag(flags, SFLG_Rebuild);
                }
                break;

            case MUIA_TheBar_ViewMode:
                if (tidata>=MUIV_TheBar_ViewMode_Last)
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    ULONG old = data->viewMode;

                    if ((old==tidata) || isFlagSet(data->flags, FLG_TextOnly))
                        tag->ti_Tag = TAG_IGNORE;
                    else
                    {
                        if (isFlagSet(data->flags, FLG_Setup) && ((old==MUIV_TheBar_ViewMode_Text) || (tidata==MUIV_TheBar_ViewMode_Text)))
                            setFlag(flags, SFLG_Rebuild);
                        else
                            setFlag(flags, SFLG_ViewMode);

                        data->viewMode = tidata;
                    }
                }
                break;

            case MUIA_TheBar_Scaled:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_Scaled)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_Scaled);
                    else
                        clearFlag(data->flags, FLG_Scaled);

                    if (isFlagSet(data->flags, FLG_Setup))
                        setFlag(flags, SFLG_Rebuild);
                    else
                        setFlag(flags, SFLG_Scaled);
                }
                break;

            case MUIA_TheBar_Sunny:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_Sunny)) || (data->disMode==MUIV_TheBar_DisMode_Sunny))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_Sunny);
                    else
                        clearFlag(data->flags, FLG_Sunny);

                    if (isFlagSet(data->flags, FLG_Setup))
                    {
                        setFlag(flags, SFLG_Rebuild);
                        if (isFlagSet(data->flags, FLG_FreeStrip))
                            setFlag(data->flags, FLG_RebuildbitMaps);
                    }
                    else
                        setFlag(flags, SFLG_Sunny);
                }
                break;

            case MUIA_TheBar_Raised:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_Raised)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_Raised);
                    else
                        clearFlag(data->flags, FLG_Raised);

                    if (isFlagSet(data->flags, FLG_Setup))
                        setFlag(flags, SFLG_Rebuild);
                    else
                        setFlag(flags, SFLG_Raised);
                }
                break;

            case MUIA_TheBar_NtRaiseActive:
                if (BOOLSAME(tidata, isFlagSet(data->userFlags2, UFLG2_NtRaiseActive)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->userFlags2, UFLG2_NtRaiseActive);
                    else
                        clearFlag(data->userFlags2, UFLG2_NtRaiseActive);

                    setFlag(flags, SFLG_NtRaiseActive);
                }
                break;

            case MUIA_TheBar_LabelPos:
                if ((tidata>=MUIV_TheBar_LabelPos_Last) || (tidata==data->labelPos))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->labelPos = tidata;
                    setFlag(flags, SFLG_LabelPos);
                }
                break;

            case MUIA_TheBar_EnableKeys:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_EnableKeys)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_EnableKeys);
                    else
                        clearFlag(data->flags, FLG_EnableKeys);

                    setFlag(flags, SFLG_EnableKeys);
                }
                break;

            case MUIA_TheBar_BarPos:
                if ((tidata>=MUIV_TheBar_BarPos_Last) || (data->barPos==tidata))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->barPos = tidata;
                    setFlag(flags, SFLG_Change);
                }
                break;

            case MUIA_Background:
                setFlag(data->flags, FLG_Background);
                break;

            case MUIA_TheBar_Columns:
                if (data->cols==(int)tidata)
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->cols = (int)tidata;
                    setFlag(flags, SFLG_Table);
                }
                break;

            case MUIA_TheBar_Rows:
                if (data->rows==(int)tidata)
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->rows = (int)tidata;
                    setFlag(flags, SFLG_Table);
                }
                break;

            case MUIA_TheBar_Free:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_FreeHoriz)) && isFlagSet(data->flags, FLG_FreeVert))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_FreeHoriz|FLG_FreeVert);
                    else
                        clearFlag(data->flags, FLG_FreeHoriz|FLG_FreeVert);

                    setFlag(flags, SFLG_Change);
                }
                break;

            case MUIA_TheBar_FreeHoriz:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_FreeHoriz)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_FreeHoriz);
                    else
                        clearFlag(data->flags, FLG_FreeHoriz);

                    setFlag(flags, SFLG_Change);
                }
                break;

            case MUIA_TheBar_FreeVert:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_FreeVert)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_FreeVert);
                    else
                        clearFlag(data->flags, FLG_FreeVert);

                    setFlag(flags, SFLG_Change);
                }
                break;

            case MUIA_TheBar_BarSpacer:
                if (BOOLSAME(tidata, isFlagSet(data->flags, FLG_BarSpacer)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags, FLG_BarSpacer);
                    else
                        clearFlag(data->flags, FLG_BarSpacer);

                    setFlag(flags, SFLG_Rebuild);
                }
                break;

            case MUIA_TheBar_SpacerIndex:
                if (data->spacer==tidata)
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->spacer = tidata;

                    setFlag(flags, SFLG_Rebuild);
                }
                break;

            case MUIA_TheBar_IgnoreAppearance:
                if (BOOLSAME(tidata, isFlagSet(data->flags2, FLG2_IgnoreAppearance)))
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                        setFlag(data->flags2, FLG2_IgnoreAppearance);
                    else
                        clearFlag(data->flags2, FLG2_IgnoreAppearance);
                }
                break;

            case MUIA_TheBar_RemoveSpacers:
                if (data->remove==tidata)
                    tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->remove = tidata;
                    setFlag(flags, SFLG_Remove);
                }
                break;
        }
    }

    if (isFlagSet(data->flags, FLG_Setup) && flags)
        DoMethod(obj,MUIM_Group_InitChange);

    if (isFlagSet(flags, SFLG_Remove))
        updateRemove(cl,obj,data);

    if (isFlagSet(flags, SFLG_Table))
    {
        if (data->cols || data->rows)
        {
            if (isFlagSet(data->flags, FLG_DragBar))
                set(data->db,MUIA_Group_Horiz,data->cols);

            if (isFlagClear(data->flags, FLG_Table))
            {
                struct Button *button, *succ;

                setFlag(data->flags, FLG_Table);

                for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                {
                    if (isFlagSet(button->flags, BFLG_Sleep))
                        continue;
                    set(button->obj,MUIA_Group_Horiz,TRUE);
                }
            }
        }
        else
        {
            if (isFlagSet(data->flags, FLG_Table))
            {
                struct Button *button, *succ;

                clearFlag(data->flags, FLG_Table);

                for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                {
                    if (isFlagSet(button->flags, BFLG_Sleep))
                        continue;

                    set(button->obj, MUIA_Group_Horiz,isFlagSet(data->flags, FLG_Horiz));
                }

                if (isFlagSet(data->flags, FLG_DragBar))
                    set(data->db, MUIA_Group_Horiz, isFlagSet(data->flags, FLG_Horiz));
            }
        }
    }

    if (isFlagSet(flags, SFLG_Horiz))
    {
        ULONG horiz = isFlagSet(data->flags, FLG_Horiz);

        if (isFlagClear(flags, SFLG_Rebuild))
        {
            struct Button *button, *succ;

            for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
            {
                if (isFlagSet(button->flags, BFLG_Sleep))
                    continue;

                set(button->obj,MUIA_Group_Horiz,horiz);
            }
        }

        if (isFlagClear(data->flags, FLG_Table) && data->db)
            set(data->db,MUIA_Group_Horiz,horiz);
    }

    if (isFlagSet(flags, SFLG_Rebuild))
        DoMethod(obj,MUIM_TheBar_Rebuild);
    else
        if (isAnyFlagSet(flags, SFLG_ButtonAttrs))
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

            if (isFlagSet(flags, SFLG_ViewMode))
            {
                attrs[0].ti_Tag  = MUIA_TheButton_ViewMode;
                attrs[0].ti_Data = data->viewMode;
            }

            if (isFlagSet(flags, SFLG_Raised))
            {
                attrs[1].ti_Tag  = MUIA_TheButton_Raised;
                attrs[1].ti_Data = isFlagSet(data->flags, FLG_Raised) ? TRUE : FALSE;
            }

            if (isFlagSet(flags, SFLG_Scaled))
            {
                attrs[2].ti_Tag  = MUIA_TheButton_Scaled;
                attrs[2].ti_Data = isFlagSet(data->flags, FLG_Scaled) ? TRUE : FALSE;
            }

            if (isFlagSet(flags, SFLG_Sunny))
            {
                attrs[3].ti_Tag  = MUIA_TheButton_Sunny;
                attrs[3].ti_Data = isFlagSet(data->flags, FLG_Sunny) ? TRUE : FALSE;
            }

            if (isFlagSet(flags, SFLG_LabelPos))
            {
                attrs[4].ti_Tag  = MUIA_TheButton_LabelPos;
                attrs[4].ti_Data = data->labelPos;
            }

            if (isFlagSet(flags, SFLG_EnableKeys))
            {
                attrs[5].ti_Tag  = MUIA_TheButton_EnableKey;
                attrs[5].ti_Data = isFlagSet(data->flags, FLG_EnableKeys) ? TRUE : FALSE;
            }

            if (isFlagSet(flags, SFLG_NtRaiseActive))
            {
                attrs[6].ti_Tag  = MUIA_TheButton_NtRaiseActive;
                attrs[6].ti_Data = isFlagSet(data->userFlags2, UFLG2_NtRaiseActive) ? TRUE : FALSE;
            }

            DoSuperMethod(cl,obj,OM_SET,(IPTR)attrs,NULL);

            for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
                if (isFlagClear(button->flags, BFLG_Sleep))
                    DoMethod(button->obj,OM_SET,(IPTR)attrs,NULL);
        }

    res = DoSuperMethodA(cl,obj,(Msg)msg);

    if (isFlagSet(data->flags, FLG_Setup) && flags)
        DoMethod(obj,MUIM_Group_ExitChange);

    RETURN(res);
    return res;
}

/***********************************************************************/

static struct MUIS_TheBar_Appearance staticAp = { MUIDEF_TheBar_Appearance_ViewMode,
                                                  MUIDEF_TheBar_Appearance_Flags,
                                                  MUIDEF_TheBar_Appearance_LabelPos,
                                                  { 0, 0 }
                                                };

static IPTR mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    #if !defined(VIRTUAL)
    Object               *parent;
    #endif
    STRPTR               ptr;
    ULONG                *val;

    ENTER();

    /* Appearance */
    if (isFlagClear(data->flags2, FLG2_IgnoreAppearance))
    {
        struct MUIS_TheBar_Appearance *ap;

        if (!getconfigitem(cl,obj,MUICFG_TheBar_Appearance,&ap))
            ap = &staticAp;

        SetAttrs(obj, MUIA_TheBar_ViewMode,     ap->viewMode,
                      MUIA_TheBar_LabelPos,     ap->labelPos,
                      MUIA_TheBar_Borderless,   (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_Borderless),
                      MUIA_TheBar_Raised,       (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_Raised),
                      MUIA_TheBar_Sunny,        (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_Sunny),
                      MUIA_TheBar_Scaled,       (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_Scaled),
                      MUIA_TheBar_BarSpacer,    (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_BarSpacer),
                      MUIA_TheBar_EnableKeys,   (IPTR)isFlagSet(ap->flags, MUIV_TheBar_Appearance_EnableKeys),
                      TAG_DONE);
    }

    if (isFlagClear(data->flags, FLG_Background))
    {
        ULONG done = FALSE;

        if(getconfigitem(cl,obj,MUICFG_TheBar_UseGroupBack,&val) ? *val : MUIDEF_TheBar_UseGroupBack)
        {
          if(isFlagClear(lib_flags, BASEFLG_MUI4) && isFlagClear(lib_flags, BASEFLG_MUI20) &&
             getconfigitem(cl,obj,MUICFG_TheBar_Gradient,&ptr))
          {
            memcpy(&data->grad,ptr,sizeof(data->grad));
            SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,NULL,TAG_DONE);
            setFlag(data->flags2, FLG2_Gradient);
            done = TRUE;
          }
          else
          {
            if(getconfigitem(cl,obj,MUICFG_TheBar_GroupBack,&ptr))
            {
              SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,(IPTR)ptr,TAG_DONE);
              done = TRUE;
            }
          }
        }

        if (!done)
        {
            DoSuperMethod(cl,obj,OM_GET,MUIA_Parent,(IPTR)&ptr);
            SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,(IPTR)(ptr ? _backspec(ptr) : MUII_WindowBack),TAG_DONE);
        }
    }

    if (isAnyFlagSet(lib_flags,BASEFLG_MUI20|BASEFLG_MUI4))
    {
      if (getconfigitem(cl,obj,MUICFG_TheBar_Frame,&ptr))
      {
        strlcpy(data->frameSpec,ptr,sizeof(data->frameSpec));
        SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,(IPTR)data->frameSpec,TAG_DONE);
      }
    }
    else
    {
      if (isFlagSet(data->userFlags2, UFLG2_UserFrame))
      {
        SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,data->userFrame,TAG_DONE);
      }
      else
      {
        SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Frame,MUIV_Frame_None,TAG_DONE);
      }
    }

    if(!DoSuperMethodA(cl,obj,msg))
    {
      RETURN(FALSE);
      return FALSE;
    }

    if (isFlagClear(data->userFlags, UFLG_UserHorizSpacing))
    {
        data->horizSpacing = getconfigitem(cl,obj,MUICFG_TheBar_HorizSpacing,&val) ?
            *val : MUIDEF_TheBar_HorizSpacing;
    }

    if (isFlagClear(data->userFlags, UFLG_UserVertSpacing))
    {
        data->vertSpacing = getconfigitem(cl,obj,MUICFG_TheBar_VertSpacing,&val) ?
            *val : MUIDEF_TheBar_VertSpacing;
    }

    if (isFlagClear(data->userFlags, UFLG_UserLeftBarFrameSpacing))
    {
        data->leftBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_LeftBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_LeftBarFrameSpacing;
    }

    if (isFlagClear(data->userFlags, UFLG_UserRightBarFrameSpacing))
    {
        data->rightBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_RightBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_RightBarFrameSpacing;
    }

    if (isFlagClear(data->userFlags, UFLG_UserTopBarFrameSpacing))
    {
        data->topBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_TopBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_TopBarFrameSpacing;
    }

    if (isFlagClear(data->userFlags, UFLG_UserBottomBarFrameSpacing))
    {
        data->bottomBarFrameSpacing = getconfigitem(cl,obj,MUICFG_TheBar_BottomBarFrameSpacing,&val) ?
            *val : MUIDEF_TheBar_BottomBarFrameSpacing;
    }

    /* IgnoreSelImages */
    if (isFlagClear(data->userFlags, UFLG_UserIgnoreSelImages))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreSelImages,&val) ? *val : MUIDEF_TheBar_IgnoreSelImages)
            setFlag(data->userFlags, UFLG_IgnoreSelImages);
        else
            clearFlag(data->userFlags, UFLG_IgnoreSelImages);
   }

    /* IgnoreDisImages */
    if (isFlagClear(data->userFlags, UFLG_UserIgnoreDisImages))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreDisImages,&val) ? *val : MUIDEF_TheBar_IgnoreDisImages)
            setFlag(data->userFlags, UFLG_IgnoreDisImages);
        else
            clearFlag(data->userFlags, UFLG_IgnoreDisImages);
    }

    /* DontMove */
    if (isFlagClear(data->userFlags2, UFLG2_UserDontMove))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_DontMove,&val) ? *val : MUIDEF_TheBar_DontMove)
            setFlag(data->userFlags2, UFLG2_DontMove);
        else
            clearFlag(data->userFlags2, UFLG2_DontMove);
    }

    /* NtRaiseActive */
    if (isFlagClear(data->userFlags2, UFLG2_UserNtRaiseActive))
    {
        if (getconfigitem(cl,obj,MUICFG_TheBar_NtRaiseActive,&val) ? *val : MUIDEF_TheBar_NtRaiseActive)
            setFlag(data->userFlags2, UFLG2_NtRaiseActive);
        else
            clearFlag(data->userFlags2, UFLG2_NtRaiseActive);
    }

    /* Frame pens */
    if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
        allocateFramePens(obj,data);

    /* DisMode */
    if (isFlagClear(data->userFlags, UFLG_UserDisMode))
    {
        data->disMode = getconfigitem(cl,obj,MUICFG_TheBar_DisMode,&val) ?
            *val : MUIDEF_TheBar_DisMode;

        if (data->disMode==MUIV_TheBar_DisMode_Sunny)
            clearFlag(data->flags, FLG_Sunny);
    }

    /* SpacersSize */
    if (isFlagClear(data->userFlags2, UFLG2_UserSpacersSize))
    {
        data->spacersSize = getconfigitem(cl,obj,MUICFG_TheBar_SpacersSize,&val) ?
            *val : MUIDEF_TheBar_SpacersSize;
    }

    /* Derive GFX env info */
    data->screen = _screen(obj);
    data->screenDepth = GetBitMapAttr(data->screen->RastPort.BitMap,BMA_DEPTH);
    if (CyberGfxBase && IsCyberModeID(GetVPModeID(&data->screen->ViewPort)))
    {
        setFlag(data->flags, FLG_CyberMap);
        if (data->screenDepth>8)
            setFlag(data->flags, FLG_CyberDeep);
    }

    if (isFlagSet(data->flags, FLG_FreeStrip))
    {
        struct Button *button, *succ;

        build(data);

        for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
            if (isFlagClear(button->flags, BFLG_Sleep))
                DoMethod(button->obj,MUIM_TheButton_Build);
    }

    if (isFlagClear(lib_flags,BASEFLG_MUI20))
    {
        memset(&data->eh,0,sizeof(data->eh));
        data->eh.ehn_Class  = cl;
        data->eh.ehn_Object = obj;
        data->eh.ehn_Events = IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW;
        DoMethod(_win(obj),MUIM_Window_AddEventHandler,(IPTR)&data->eh);
    }

    #if defined(VIRTUAL)
    setFlag(data->flags, FLG_IsInVirtgroup);
    #else
    for (parent = obj; ;)
    {
        if((parent = (Object *)xget(parent, MUIA_Parent)) == NULL)
          break;

        if((ptr = (STRPTR)xget(parent, MUIA_Virtgroup_Top)))
        {
            setFlag(data->flags, FLG_IsInVirtgroup);
            break;
        }
    }
    #endif

    setFlag(data->flags, FLG_Setup);

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static IPTR
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result = 0;

  ENTER();

  if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
    freeFramePens(obj,data);

  if (isFlagClear(lib_flags,BASEFLG_MUI20))
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,(IPTR)&data->eh);

  if (isFlagSet(data->flags, FLG_FreeStrip))
    freeBitMaps(data);

  clearFlag(data->flags, FLG_Setup|FLG_CyberMap|FLG_CyberDeep|FLG_IsInVirtgroup);
  clearFlag(data->flags2, FLG2_Gradient);

  result = DoSuperMethodA(cl,obj,msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

/*
** Don't ask neither change.
** This is such a strange thing, I can't explain,
** In some rare condition without "forcing" the
** already computed dimensions of the object in
** AskMinMax, the group gets wrong height and
** strange things happens. Very rare, but with
** few lines of code, we simply force the right
** dimensions and all is fine. Just for MUI<4.
*/

#ifdef VIRTUAL
static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    ULONG res = DoSuperMethodA(cl,obj,(Msg)msg);

    if (isFlagClear(lib_flags,BASEFLG_MUI4))
    {
        struct InstData *data = INST_DATA(cl, obj);

        msg->MinMaxInfo->MinWidth  = _subwidth(obj)+data->objWidth;
        msg->MinMaxInfo->DefWidth  = _subwidth(obj)+data->objWidth;
        msg->MinMaxInfo->MaxWidth  = _subwidth(obj)+data->objMWidth;
        if (msg->MinMaxInfo->MaxWidth>MUI_MAXMAX)
            msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;

        msg->MinMaxInfo->MinHeight = _subheight(obj)+data->objHeight;
        msg->MinMaxInfo->DefHeight = _subheight(obj)+data->objHeight;
        msg->MinMaxInfo->MaxHeight = _subheight(obj)+data->objMHeight;
        if (msg->MinMaxInfo->MaxHeight>MUI_MAXMAX)
            msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;
    }

    return res;
}
#endif

/***********************************************************************/

static IPTR mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl, obj);

    ENTER();

    if(!DoSuperMethodA(cl,obj,msg))
    {
      RETURN(FALSE);
      return FALSE;
    }

    if (isFlagSet(data->flags, FLG_CyberDeep) && isFlagSet(data->flags2, FLG2_Gradient))
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
            horiz = isFlagSet(data->grad.flags, MUIV_TheBar_Gradient_Horiz);
            if (isFlagClear(data->flags, FLG_Horiz))
                horiz = !horiz;

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
            if (bdif>step)
                step = bdif;

            /* Find out best step to minimize rest */
            if (horiz)
            {
                if (step>w)
                    step = w;

                if (step==0)
                    step = 1;
                else
                {
                    x = w/step;
                    if (w!=x*step)
                    {
                        step = w/(x+1);
                        if (step>w)
                            step = w;
                    }
                }
            }
            else
            {
                if (step>h)
                    step = h;

                if (step==0)
                    step = 1;
                else
                {
                    x = h/step;
                    if (h!=x*step)
                    {
                        step = h/(x+1);
                        if (step>h)
                            step = h;
                    }
                }
            }

            if (step>1)
            {
                /* Compute color components deltas */
                rd = rdif/step;
                if (!rd)
                    rd = 1;

                gd = gdif/step;
                if (!gd)
                    gd = 1;

                bd = bdif/step;
                if (!bd)
                    bd = 1;

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
                    else
                        FillPixelArray(&trp,x,0,d,h,col);
                else
                    if (i==step-1)
                    {
                        FillPixelArray(&trp,0,x,w,h-x,col);
                        break;
                    }
                    else
                        FillPixelArray(&trp,0,x,w,d,col);

                x += d;

                /* Add delta to each color component */
                if (rs)
                {
                    r += rd;

                    /* Time to add more one more delta? */
                    if (i && ridx && !(i%ridx))
                        r += rd;
                    if (r>r2)
                        r = r2;
                }
                else
                {
                    r -= rd;

                    if (i && ridx && !(i%ridx))
                        r -= rd;
                    if (r<r2)
                        r = r2;
                }

                if (gs)
                {
                    g += gd;

                    if (i && gidx && !(i%gidx))
                        g += gd;
                    if (g>g2)
                        g = g2;
                }
                else
                {
                    g -= gd;

                    if (i && gidx && !(i%gidx))
                        g -= gd;
                    if (g<g2)
                        g = g2;
                }

                if (bs)
                {
                    b += bd;

                    if (i && bidx && !(i%bidx))
                        b += bd;
                    if (b>b2)
                        b = b2;
                }
                else
                {
                    b -= bd;

                    if (i && bidx && !(i%bidx))
                        b -= bd;
                    if (b<b2)
                        b = b2;
                }
            }
        }
    }

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static IPTR mHide(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result = 0;

  ENTER();

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

static IPTR mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR rc;

  ENTER();

  rc = DoSuperMethodA(cl,obj,(Msg)msg);

  if(isFlagClear(lib_flags, BASEFLG_MUI4))
  {
    #if defined(VIRTUAL)
    if (isFlagSet(data->flags, FLG_Framed))
    #else
    if (isFlagSet(data->flags, FLG_Framed) && (isFlagSet(msg->flags, MADF_DRAWUPDATE) || isFlagSet(msg->flags, MADF_DRAWOBJECT)))
    #endif
    {
      SetAPen(_rp(obj),MUIPEN(data->barFrameShinePen));
      Move(_rp(obj),_left(obj),_bottom(obj));
      Draw(_rp(obj),_left(obj),_top(obj));
      Draw(_rp(obj),_right(obj),_top(obj));

      SetAPen(_rp(obj),MUIPEN(data->barFrameShadowPen));
      Draw(_rp(obj),_right(obj),_bottom(obj));
      Draw(_rp(obj),_left(obj)+1,_bottom(obj));
    }
  }

  RETURN(rc);
  return rc;
}

/***********************************************************************/

static IPTR mBackfill(struct IClass *cl,Object *obj,struct MUIP_Backfill *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result = 0;

  ENTER();

  if (isFlagSet(lib_flags,BASEFLG_MUI20))
    return DoSuperMethodA(cl,obj,(Msg)msg);

  if(data->gradbm)
    BltBitMapRastPort(data->gradbm,msg->left-_left(obj),msg->top-_top(obj),_rp(obj),msg->left,msg->top,msg->right-msg->left+1,msg->bottom-msg->top+1,0xc0);
  else
    DoSuperMethod(cl,obj,MUIM_DrawBackground,msg->left,msg->top,msg->right-msg->left+1,msg->bottom-msg->top+1,msg->xoffset,msg->yoffset,0);

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mCreateDragImage(struct IClass *cl,Object *obj,struct MUIP_CreateDragImage *msg)
{
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  data->dm = (struct MUI_DragImage *)DoSuperMethodA(cl,obj,(Msg)msg);

  RETURN((IPTR)data->dm);
  return (IPTR)data->dm;
}

/***********************************************************************/

static IPTR mDeleteDragImage(struct IClass *cl,Object *obj,struct MUIP_DeleteDragImage *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result;

  ENTER();

  data->dm = NULL;

  result = DoSuperMethodA(cl,obj,(Msg)msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mInitChange(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result;

  ENTER();

  result = data->changes++ ? 0 : DoSuperMethodA(cl,obj,msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mExitChange(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  IPTR result;

  ENTER();

  result = (data->changes && !(--data->changes)) ? DoSuperMethodA(cl,obj,msg) : 0;

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mRebuild(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct MinNode *node;

  ENTER();

  D(DBF_STARTUP, "Rebuild: %08lx", obj);

  if(isFlagSet(data->flags, FLG_Setup))
    DoMethod(obj,MUIM_Group_InitChange);

  // cleanup all buttons in our list
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct MinList *notifyList;
    struct Button *button = (struct Button *)node;

    // if the button is flagged as sleeping
    // we can continue
    if(isFlagSet(button->flags, BFLG_Sleep))
      continue;

    // clear some flags
    clearFlag(button->flags, BFLG_Disabled|BFLG_Selected);

    // save some variables
    if(xget(button->obj, MUIA_Disabled))
      setFlag(button->flags, BFLG_Disabled);

    if(xget(button->obj, MUIA_Selected))
      setFlag(button->flags, BFLG_Selected);

    // remove object from group if necessary
    if(isFlagClear(button->flags, BFLG_Hide))
      DoSuperMethod(cl, obj, OM_REMMEMBER, (IPTR)button->obj);

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

        size = sizeof(struct ButtonNotify)+sizeof(IPTR)*notify->msg.FollowParams;

        // clone
        if((clone = SharedAlloc(size)))
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
  if(isFlagSet(data->flags, FLG_Setup) && isFlagClear(data->flags, FLG_CyberDeep))
    MUI_Redraw(obj, MADF_DRAWOBJECT);

  // rebuild all bitmaps in case required
  if(isFlagSet(data->flags, FLG_RebuildbitMaps))
  {
    freeBitMaps(data);
    build(data);

    clearFlag(data->flags, FLG_RebuildbitMaps);
  }

  // now we go and rebuild all buttons again
  for(node = data->buttons.mlh_Head; node->mln_Succ; node = node->mln_Succ)
  {
    struct Button *button = (struct Button *)node;

    // if the button is flagged as sleeping
    // we can continue
    if(isFlagSet(button->flags, BFLG_Sleep))
      continue;

    // generate a new button object
    if((button->obj = makeButton(button, obj, data)))
    {
      struct MinNode *node;

      // add the button object to our group
      // if it isn't flagged as being hidden
      if(isFlagClear(button->flags, BFLG_Hide))
        DoSuperMethod(cl, obj, OM_ADDMEMBER, (IPTR)button->obj);

      // now we put the notifies from our clone list
      // back active for the new button
      for(node = button->notifyListClone.mlh_Head; node->mln_Succ; )
      {
        struct ButtonNotify *notify = (struct ButtonNotify *)node;

        node = node->mln_Succ;

        // now we set the notify as we have identifed the button
        DoMethodA(button->obj, (Msg)(void *)&notify->msg);

        // remove the clone notify from the list
        Remove((struct Node *)notify);

        // free the clone notify
        SharedFree(notify);
      }

      if(isFlagSet(data->flags, FLG_Setup) && isFlagSet(data->flags, FLG_FreeStrip))
        DoMethod(button->obj, MUIM_TheButton_Build);
    }
  }

  if(isFlagSet(data->flags, FLG_Setup))
    DoMethod(obj, MUIM_Group_ExitChange);

  RETURN(0);
  return 0;
}

/***********************************************************************/

static IPTR mNotify(struct IClass *cl, Object *obj, struct MUIP_TheBar_Notify *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  BOOL result = FALSE;
  struct Button *button;

  ENTER();

  // now we first find the button object with its ID
  if((button = findButton(data, msg->ID)) != NULL)
  {
    struct MUIP_Notify *notify;

    // lets allocate a temporary buffer for sending
    // the notify method to the button correctly.
    if((notify = SharedAlloc(sizeof(struct MUIP_Notify)+(sizeof(IPTR)*msg->followParams))) != NULL)
    {
      // now we fill the notify structure
      notify->MethodID      = MUIM_Notify;
      notify->TrigAttr      = msg->attr;
      notify->TrigVal       = msg->value;
      notify->DestObj       = msg->dest;

      // fill the rest with memcpy
      memcpy(&notify->FollowParams, &msg->followParams, sizeof(msg->followParams)+sizeof(IPTR)*msg->followParams);

      // now we set the notify as we have identifed the button
      result = DoMethodA(button->obj, (Msg)notify);

      SharedFree(notify);
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mKillNotify(struct IClass *cl, Object *obj, struct MUIP_TheBar_KillNotify *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  BOOL result = FALSE;

  ENTER();

  if((button = findButton(data, msg->ID)) != NULL)
  {
    // now we kill the notify as we have identifed the button
    if(msg->dest != NULL)
      DoMethod(button->obj, MUIM_KillNotifyObj, (IPTR)msg->attr, (IPTR)msg->dest);
    else
      DoMethod(button->obj, MUIM_KillNotify, msg->attr);
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mAddButton(struct IClass *cl,Object *obj,struct MUIP_TheBar_AddButton *msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    struct Button *button;

    ENTER();

    if((button = SharedAlloc(sizeof(struct Button))))
    {
        button->img     = msg->button->img;
        button->ID      = msg->button->ID;
        button->text    = msg->button->text;
        button->help    = msg->button->help;
        button->exclude = msg->button->exclude;
        button->class  =  msg->button->_class;
        button->obj     = NULL;
        button->flags   = 0;

        NewList((struct List *)&button->notifyListClone);

        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_NoClick))
            setFlag(button->flags, BFLG_NoClick);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Immediate))
            setFlag(button->flags, BFLG_Immediate);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Toggle))
            setFlag(button->flags, BFLG_Toggle);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Disabled))
            setFlag(button->flags, BFLG_Disabled);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Selected))
            setFlag(button->flags, BFLG_Selected);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Sleep))
            setFlag(button->flags, BFLG_Sleep);
        if (isFlagSet(msg->button->flags, MUIV_TheBar_ButtonFlag_Hide))
            setFlag(button->flags, BFLG_Hide);

        if (button->exclude)
        {
            clearFlag(button->flags, MUIV_TheBar_ButtonFlag_NoClick|MUIV_TheBar_ButtonFlag_Toggle);
            setFlag(button->flags, MUIV_TheBar_ButtonFlag_Immediate);
        }

        if (isFlagSet(button->flags, BFLG_Sleep) || (button->obj = makeButton(button,obj,data)))
        {
            AddTail((struct List *)&data->buttons, (struct Node *)button);

            if (isFlagClear(button->flags, BFLG_Sleep))
            {
                set(button->obj,MUIA_TheButton_TheBar,obj);

                if (isFlagClear(button->flags, BFLG_Hide))
                {
                    if (isFlagSet(data->flags, FLG_Setup))
                        DoMethod(obj,MUIM_Group_InitChange);

                    DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)button->obj);

                    if (isFlagSet(data->flags, FLG_Setup))
                    {
                        if (isFlagSet(data->flags, FLG_FreeStrip))
                            DoMethod(button->obj,MUIM_TheButton_Build);

                        DoMethod(obj,MUIM_Group_ExitChange);
                    }
                }

                if (button->exclude)
                    DoMethod(button->obj,MUIM_Notify,MUIA_Selected,TRUE,(IPTR)obj,3,MUIM_Set,MUIA_TheBar_Active,button->ID);

                msg->button->obj = button->obj;
            }
        }
        else
        {
            SharedFree(button);
            button = NULL;
        }
    }

    RETURN((IPTR)button);
    return (IPTR)button;
}

/***********************************************************************/

static IPTR mGetAttr(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetAttr *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *bt;
  BOOL result = FALSE;

  ENTER();

  if((bt = findButton(data,msg->ID)) !=NULL)
  {
    switch (msg->attr)
    {
      case MUIA_TheBar_Attr_Hide:
      {
        *msg->storage = isFlagSet(bt->flags, BFLG_Hide);
        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Sleep:
      {
        *msg->storage = isFlagSet(bt->flags, BFLG_Sleep);
        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Disabled:
      {
        if(bt->obj)
          *msg->storage = xget(bt->obj, MUIA_Disabled);
        else
          *msg->storage = isFlagSet(bt->flags, BFLG_Disabled);

        result = TRUE;
      }
      break;

      case MUIA_TheBar_Attr_Selected:
      {
        if(bt->obj)
          *msg->storage = xget(bt->obj, MUIA_Selected);
        else
          *msg->storage = isFlagSet(bt->flags, BFLG_Selected);

        result = TRUE;
      }
      break;
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG hideButton(struct IClass *cl,Object *obj,struct InstData *data,struct Button *bt,ULONG value)
{
    ULONG res = FALSE;

    ENTER();

    if (!BOOLSAME(value, isFlagSet(bt->flags, BFLG_Hide)))
    {
        if (isFlagSet(data->flags, FLG_Setup))
            DoMethod(obj,MUIM_Group_InitChange);

        if (value)
        {
            DoSuperMethod(cl,obj,OM_REMMEMBER,(IPTR)bt->obj);
            setFlag(bt->flags, BFLG_Hide);
            res = TRUE;
        }
        else
        {
            clearFlag(bt->flags, BFLG_Hide);
            DoSuperMethod(cl,obj,OM_ADDMEMBER,(IPTR)bt->obj);

            if (orderButtons(cl,obj,data))
                res = TRUE;
            else
            {
                DoSuperMethod(cl,obj,OM_REMMEMBER,(IPTR)bt->obj);
                setFlag(bt->flags, BFLG_Hide);
            }

        }

        if (isFlagSet(data->flags, FLG_Setup))
        {
            if (isFlagSet(data->flags, FLG_FreeStrip))
                DoMethod(bt->obj,MUIM_TheButton_Build);
            DoMethod(obj,MUIM_Group_ExitChange);
        }
    }
    else
        res = TRUE;

    RETURN(res);
    return res;
}

/***********************************************************************/

static ULONG sleepButton(struct IClass *cl, Object *obj, struct InstData *data, struct Button *bt, ULONG value)
{
  ULONG res = FALSE;

  ENTER();

  D(DBF_STARTUP, "sleepButton: %d", value);

  if(!BOOLSAME(value, isFlagSet(bt->flags, BFLG_Sleep)))
  {
    // should the button put to sleep or awakend
    if(value)
    {
      struct MinList *notifyList;

      // remove the button object from the group
      if(isFlagClear(bt->flags, BFLG_Hide))
      {
        if(isFlagSet(data->flags, FLG_Setup))
          DoMethod(obj, MUIM_Group_InitChange);

        DoSuperMethod(cl, obj, OM_REMMEMBER, (IPTR)bt->obj);

        if(isFlagSet(data->flags, FLG_Setup))
          DoMethod(obj, MUIM_Group_ExitChange);
      }

      clearFlag(bt->flags, BFLG_Disabled|BFLG_Selected);

      if(xget(bt->obj, MUIA_Disabled))
        setFlag(bt->flags, BFLG_Disabled);

      if(xget(bt->obj, MUIA_Selected))
        setFlag(bt->flags, BFLG_Selected);

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
          if((clone = SharedAlloc(size)))
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

      setFlag(bt->flags, BFLG_Sleep);

      res = TRUE;
    }
    else // bring the button back from sleep
    {
      if((bt->obj = makeButton(bt, obj, data)))
      {
        SetAttrs(bt->obj, MUIA_TheButton_TheBar, (IPTR)obj,
                          MUIA_Group_Horiz,      isFlagSet(data->flags, FLG_Horiz),
                          TAG_DONE);

        if(bt->exclude)
          DoMethod(bt->obj, MUIM_Notify, MUIA_Selected, TRUE, (IPTR)obj, 3, MUIM_Set, MUIA_TheBar_Active, bt->ID);

        if(isFlagSet(bt->flags, BFLG_Hide))
          res = TRUE;
        else
        {
          if(isFlagSet(data->flags, FLG_Setup))
            DoMethod(obj, MUIM_Group_InitChange);

          DoSuperMethod(cl, obj, OM_ADDMEMBER, (IPTR)bt->obj);

          // put the buttons in order
          if(orderButtons(cl, obj, data))
            res = TRUE;
          else
          {
            // otherwise remove it again
            DoSuperMethod(cl, obj, OM_REMMEMBER,(IPTR)bt->obj);
            MUI_DisposeObject(bt->obj);
            bt->obj = NULL;
          }

          if(isFlagSet(data->flags, FLG_Setup))
          {
            if(isFlagSet(data->flags, FLG_FreeStrip) && bt->obj)
              DoMethod(bt->obj,MUIM_TheButton_Build);

            DoMethod(obj,MUIM_Group_ExitChange);
          }

          // make the button the active one
          if(res && bt->exclude && isFlagSet(bt->flags, BFLG_Selected))
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
            DoMethodA(bt->obj, (Msg)(void *)&notify->msg);

            // remove the clone notify from the list
            Remove((struct Node *)notify);

            // free the clone notify
            SharedFree(notify);
          }

          // remove the sleep flag
          clearFlag(bt->flags, BFLG_Sleep);
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

static IPTR privateSetAttr(Object *obj, struct Button *button, IPTR attr, IPTR value, BOOL noNotify)
{
  IPTR res = FALSE;

  ENTER();

  switch(attr)
  {
    case MUIA_TheBar_Attr_Disabled:
    {
      if(button->obj != NULL)
        SetAttrs(button->obj, MUIA_NoNotify, noNotify, MUIA_Disabled, value, TAG_DONE);

      if(value != 0)
        setFlag(button->flags, BFLG_Disabled);
      else
        clearFlag(button->flags, BFLG_Disabled);
      res = TRUE;
    }
    break;

    case MUIA_TheBar_Attr_Selected:
    {
      if(button->exclude)
        SetAttrs(obj, MUIA_NoNotify, noNotify, MUIA_TheBar_Active, button->ID, TAG_DONE);
      else if(button->obj != NULL)
        SetAttrs(button->obj, MUIA_NoNotify, noNotify, MUIA_Selected, value, TAG_DONE);

      if(value != 0)
        setFlag(button->flags, BFLG_Selected);
      else
        clearFlag(button->flags, BFLG_Selected);
      res = TRUE;
    }
    break;
  }

  RETURN(res);
  return res;
}

/***********************************************************************/

static IPTR mNoNotifySetAttr(struct IClass *cl,Object *obj,struct MUIP_TheBar_SetAttr *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  IPTR res = FALSE;

  ENTER();

  D(DBF_ALWAYS, "nnset attr %08lx of button %2d to value %08lx", msg->attr, msg->ID, msg->value);
  if((button = findButton(data, msg->ID)) != NULL)
  {
    switch(msg->attr)
    {
      case MUIA_TheBar_Attr_Hide:
        res = hideButton(cl, obj, data, button, msg->value);
      break;

      case MUIA_TheBar_Attr_Sleep:
        res = sleepButton(cl, obj, data, button, msg->value);
      break;

      case MUIA_TheBar_Attr_Disabled:
      case MUIA_TheBar_Attr_Selected:
        res = privateSetAttr(obj, button, msg->attr, msg->value, TRUE);
      break;
    }
  }

  RETURN(res);
  return res;
}

/***********************************************************************/

static IPTR mSetAttr(struct IClass *cl,Object *obj,struct MUIP_TheBar_SetAttr *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  IPTR res = FALSE;

  ENTER();

  D(DBF_ALWAYS, "set attr %08lx of button %2d to value %08lx", msg->attr, msg->ID, msg->value);
  if((button = findButton(data, msg->ID)) != NULL)
  {
    switch(msg->attr)
    {
      case MUIA_TheBar_Attr_Hide:
        res = hideButton(cl, obj, data, button, msg->value);
      break;

      case MUIA_TheBar_Attr_Sleep:
        res = sleepButton(cl, obj, data, button, msg->value);
      break;

      case MUIA_TheBar_Attr_Disabled:
      case MUIA_TheBar_Attr_Selected:
        res = privateSetAttr(obj, button, msg->attr, msg->value, FALSE);
      break;
    }
  }

  RETURN(res);
  return res;
}

/***********************************************************************/

static IPTR mRemove(struct IClass *cl, Object *obj, struct MUIP_TheBar_Remove *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  BOOL result = FALSE;

  ENTER();

  if((button = findButton(data, msg->ID)))
  {
    if(button->obj)
    {
      if(isFlagSet(data->flags, FLG_Setup))
        DoMethod(obj, MUIM_Group_InitChange);

      removeButton(cl, obj, button);

      if(isFlagSet(data->flags, FLG_Setup))
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

static IPTR mGetObject(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetObject *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  Object *result;

  ENTER();

  result = (button = findButton(data,msg->ID)) ? button->obj : NULL;

  RETURN((IPTR)result);
  return (IPTR)result;
}

/***********************************************************************/

static IPTR mDoOnButton(struct IClass *cl,Object *obj,struct MUIP_TheBar_DoOnButton *msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct Button *button;
  IPTR result;

  ENTER();

  result = ((button = findButton(data,msg->ID)) && isFlagClear(button->flags, BFLG_Sleep)) ? DoMethodA(button->obj,(Msg)&msg->method) : 0;

  RETURN(result);
  return result;
}

/***********************************************************************/

static IPTR mClear(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  struct MinNode *node;

  ENTER();

  if(isFlagSet(data->flags, FLG_Setup))
    DoMethod(obj,MUIM_Group_InitChange);

  for(node = data->buttons.mlh_Head; node->mln_Succ; )
  {
    struct Button *button = (struct Button *)node;

    // advance further
    node = node->mln_Succ;

    removeButton(cl, obj, button);
  }

  if(isFlagSet(data->flags, FLG_Setup))
    DoMethod(obj,MUIM_Group_ExitChange);


  RETURN(0);
  return 0;
}

/***********************************************************************/

static IPTR mDeActivate(struct IClass *cl, Object *obj, UNUSED Msg msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    struct Button *button, *succ;

    ENTER();

    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        if (isFlagSet(button->flags, BFLG_Sleep))
            continue;

        set(button->obj,MUIA_TheButton_MouseOver,FALSE);
    }

    RETURN(0);
    return 0;
}

/***********************************************************************/

static IPTR mSort(struct IClass *cl,Object *obj,struct MUIP_TheBar_Sort *msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    struct MinList                  temp;
    struct MUIP_Group_Sort *smsg;
    struct Button          *button, *succ;
    Object                 **o;
    ULONG                  n;
    LONG                   *id;

    ENTER();

    /* Counts buttons */
    for(n = 0, button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        if (isFlagClear(button->flags, BFLG_Sleep) && isFlagClear(button->flags, BFLG_Hide))
            n++;

    if (data->db)
        n++;

    /* Alloc sort msg */
    if (n>STATICSORTSIZE)
        smsg = SharedAlloc(sizeof(struct MUIP_Group_Sort)+sizeof(Object *)*(n+1));
    else
        smsg = (struct MUIP_Group_Sort *)&data->sortMsgID;
    if (!smsg)
    {
      RETURN(FALSE);
      return FALSE;
    }

    // set the method ID for sorting
    smsg->MethodID = MUIM_Group_Sort;

    /* Insert DragBar */
    if (data->db)
    {
        smsg->obj[0] = data->db;
        o = smsg->obj+1;
    }
    else
        o = smsg->obj;

    NewList((struct List *)&temp);

    /* Insert message buttons */
    for (id = msg->obj; *id>=0; id++, o++)
    {
        if (!(button = findButton(data,*id)))
        {
            if(n>STATICSORTSIZE)
              SharedFree(smsg);

            RETURN(FALSE);
            return FALSE;
        }

        Remove((struct Node *)button);
        AddHead((struct List *)&temp, (struct Node *)button);

        if (isFlagClear(button->flags, BFLG_Sleep) && isFlagClear(button->flags, BFLG_Hide))
            *o = button->obj;
    }

    /* Insert missing */
    for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ, o++)
    {
        Remove((struct Node *)button);
        AddHead((struct List *)&temp, (struct Node *)button);
        if (isFlagClear(button->flags, BFLG_Sleep) && isFlagClear(button->flags, BFLG_Hide))
            *o = button->obj;
    }

    *o = NULL;

    /* Re-insert buttons */
    for(button = (struct Button *)(temp.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
    {
        Remove((struct Node *)button);
        AddHead((struct List *)&data->buttons, (struct Node *)button);
    }

    if (isFlagSet(data->flags, FLG_Setup))
        DoMethod(obj,MUIM_Group_InitChange);

    DoSuperMethodA(cl,obj,(Msg)smsg);

    if (isFlagSet(data->flags, FLG_Setup))
        DoMethod(obj,MUIM_Group_ExitChange);

    if (n>STATICSORTSIZE)
        SharedFree(smsg);

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static IPTR mGetDragImage(struct IClass *cl,Object *obj,struct MUIP_TheBar_GetDragImage *msg)
{
    struct InstData *data = INST_DATA(cl, obj);
    IPTR result = 0;

    ENTER();

    if (data->dm)
    {
        if (BOOLSAME(isFlagSet(data->flags, FLG_Horiz), msg->horiz))
        {
            data->di.width  = data->dm->width;
            data->di.height = data->dm->height;
        }
        else
        {
            data->di.width  = data->buttonWidth;
            data->di.height = data->buttonHeight;
            if(isFlagClear(lib_flags,BASEFLG_MUI4) && isFlagSet(data->flags, FLG_Framed))
            {
              data->di.width  += data->leftBarFrameSpacing+data->rightBarFrameSpacing+2;
              data->di.height += data->topBarFrameSpacing+data->bottomBarFrameSpacing+2;
            }
        }

        data->di.di = data->dm;

        result = (IPTR)&data->di;
    }

    RETURN(result);
    return result;
}

/***********************************************************************/


static IPTR mHandleEvent(struct IClass *cl, Object *obj, UNUSED struct MUIP_HandleEvent *msg)
{
    if (isFlagClear(lib_flags,BASEFLG_MUI20))
    {
        struct InstData *data = INST_DATA(cl, obj);
        struct Button *button, *succ;

        ENTER();

        for(button = (struct Button *)(data->buttons.mlh_Head); (succ = (struct Button *)(button->node.mln_Succ)); button = succ)
        {
            if (isFlagSet(button->flags, BFLG_Sleep))
                continue;

            set(button->obj,MUIA_TheButton_MouseOver,FALSE);
        }

        RETURN(0);
        return 0;
    }
    else
    {
        ULONG res = DoSuperMethodA(cl,obj,(Msg)msg);

        RETURN(res);
        return res;
    }
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
    case MUIM_Draw:                     return mDraw(cl,obj,(APTR)msg);
    case MUIM_Backfill:                 return mBackfill(cl,obj,(APTR)msg);

    #ifdef VIRTUAL
    case MUIM_AskMinMax:                return mAskMinMax(cl,obj,(APTR)msg);
    #endif
    case MUIM_CreateDragImage:          return mCreateDragImage(cl,obj,(APTR)msg);
    case MUIM_DeleteDragImage:          return mDeleteDragImage(cl,obj,(APTR)msg);
    case MUIM_HandleEvent:              return mHandleEvent(cl,obj,(APTR)msg);

    case MUIM_Group_InitChange:         return mInitChange(cl,obj,(APTR)msg);
    case MUIM_Group_ExitChange:         return mExitChange(cl,obj,(APTR)msg);

    case MUIM_TheBar_Rebuild:           return mRebuild(cl,obj,(APTR)msg);
    case MUIM_TheBar_AddButton:         return mAddButton(cl,obj,(APTR)msg);
    case MUIM_TheBar_GetAttr:           return mGetAttr(cl,obj,(APTR)msg);
    case MUIM_TheBar_SetAttr:           return mSetAttr(cl,obj,(APTR)msg);
    case MUIM_TheBar_NoNotifySetAttr:   return mNoNotifySetAttr(cl,obj,(APTR)msg);
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
