#include "Private.h"

///DrRect
STATIC void DrRect(FRender *Render,FRect *Rect,ULONG Flags)
{
    ULONG Pen = (Rect -> x1 + Rect -> y1) % 128;
 
    if (FF_Erase_Region & Flags)
    {
        struct RegionRectangle *rr;

        for (rr = ((struct Region *)(Rect)) -> RegionRectangle ; rr ; rr = rr -> Next)
        {
            if (rr -> bounds.MinX <= rr -> bounds.MaxX && rr -> bounds.MinY <= rr -> bounds.MaxY)
            {
                SetAPen(Render -> RPort,Pen);
                RectFill(Render -> RPort,((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MinX,
                                                 ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MinY,
                                                 ((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MaxX,
                                                 ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MaxY);
            }
        }
    }
    else
    {
        SetAPen(Render -> RPort,Pen);
        RectFill(Render -> RPort,Rect -> x1,Rect -> y1,Rect -> x2,Rect -> y2);
    }
}
//+

/*** Private ***************************************************************/

///draw_stnd
STATIC F_METHODM(void,draw_stnd,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _sub_declare_all;

    if ((FF_Area_Damaged & _flags) && (LOD->Region != NULL))
    {
        DisposeRegion(LOD->Region); LOD->Region = NULL;
    }

    if ((FF_Render_Complex & _render -> Flags) ||
        (FF_GROUP_RETHINKING & LOD -> Flags))
    {
        struct Region *region;
        
//      F_Log(0,"complex renrering (0x%08lx)(0x%08lx)",FF_Render_Complex & _render -> Flags,FF_GROUP_RETHINKING & LOD -> Flags);

        F_Set(Obj, FA_NoFill, TRUE);

        if (LOD -> Region)
        {
            F_Do(Obj,FM_Erase,LOD -> Region,FF_Erase_Region);
        }
        else if ((region = NewRegion()) != NULL)
        {
            FRect r;
            
            r.x1 = _ix; r.x2 = _ix2;
            r.y1 = _iy; r.y2 = _iy2;

            OrRectRegion(region,(struct Rectangle *) &r);

            for _each
            {
                r.x1 = _sub_x; r.x2 = r.x1 + _sub_w - 1;
                r.y1 = _sub_y; r.y2 = r.y1 + _sub_h - 1;

                ClearRectRegion(region,(struct Rectangle *) &r);
            }

            if (region -> RegionRectangle)
            {
                F_Do(Obj, FM_Erase, region, FF_Erase_Region);
                //DrRect(_render, region, FF_Erase_Region);
            }

            LOD -> Region = region;
        }

        //F_SuperDo(Class,Obj,Method,Msg -> Flags);
        F_SUPERDO();
    }
    else
    {
        F_Set(Obj, FA_NoFill, FALSE);

        F_SUPERDO();
    }
}
//+
///draw_page
STATIC F_METHODM(void,draw_page,FS_Draw)
{
    struct LocalObjectData    *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;
    uint32 *pens;
    int16 x1 = _x;
    int16 x2 = _x2;
    int16 y1 = _y;
    int16 y2 = _y2;
    int16 y;

    FAreaNode *node;

    F_SUPERDO();
    
    pens = _pens;
    
    y = y1 + 5 + LOD -> PageData -> Font -> tf_YSize;

    /* sides and bottom lines of the page */

    _APen(pens[FV_Pen_Dark]);     _Move(x1,y+2);    _Draw(x1,y2);     _Draw(x2,y2); _Draw(x2,y+2);
    _APen(pens[FV_Pen_Shine]);    _Move(x1+1,y+2);  _Draw(x1+1,y2-2);
    _APen(pens[FV_Pen_Shadow]);   _Move(x1+2,y2-1); _Draw(x2-1,y2-1); _Draw(x2-1,y+3);
    _APen(pens[FV_Pen_Fill]);     _Plot(x1+1,y2-1); _Plot(x2-1,y+2);

    /* erase the page contents. if render mode is complex, we only erase the
    borders of the contents */

    if (FF_Render_Complex & _render -> Flags)
    {
        struct Region *region;

        if ((region = NewRegion()) != NULL)
        {
            FRect r;

            node = LOD -> PageData -> ActiveNode;

            r.x1 = x1 + 2; r.x2 = x2 - 2;
            r.y1 = y + 3;  r.y2 = y2 - 2; OrRectRegion(region,(struct Rectangle *) &r);

            r.x1 = _sub_x; r.x2 = r.x1 + _sub_w - 1;
            r.y1 = _sub_y; r.y2 = r.y1 + _sub_h - 1; ClearRectRegion(region,(struct Rectangle *) &r);

//         DrRect(_render,region,FF_Erase_Region,3);
            F_Do(Obj,FM_Erase,region,FF_Erase_Region);

            DisposeRegion(region);
        }
    }
    else
    {
        FRect r;

        r.x1 = x1 + 2; r.x2 = x2 - 2;
        r.y1 = y + 3;  r.y2 = y2 - 2;

//      DrRect(_render,&r,0,4);
        F_Do(Obj,FM_Erase,&r,0);
    }

    {
        UWORD w,x=0,back_x2,back_x1,back_y1;
        ULONG i = 0;
        FRect td_Rect;

        for _each i++;

        w = (x2 - x1 - i) / i;

        for _each
        {
            if ((FF_Draw_Update & Msg -> Flags &&
                 (node == LOD -> PageData -> UpdateNode || node == LOD -> PageData -> ActiveNode)) ||
                  FF_Draw_Object & Msg -> Flags)
            {
                back_x1 = x1;
                back_x2 = x2;
                back_y1 = y1;

                if (node == LOD -> PageData -> ActiveNode)
                {
                    x  = x1 + w - 1;
                    x1 = x1 + 2;
                    x2 = x1 + w - 5; td_Rect.y1 = y1 + 5;

                    F_Erase(_parent,x1,y1,x1 + 3,y1 + 3,0);
                    F_Erase(_parent,x2 - 3,y1,x2,y1+3,0);

                    /* erase the inner space */

                    F_Erase(Obj,x1+4,y1+2,x2-3,y1+4,0);
                    F_Erase(Obj,x1+3,y1+5,x2-3,y,0);

                    _APen(pens[FV_Pen_Dark]);
                    _Move(x1,y)    ; _Draw(x1,y1+5) ; _Plot(x1+1,y1+4) ; _Plot(x1+4,y1+1) ; _Move(x1+1,y1+3) ; _Draw(x1+3,y1+1);
                    _Move(x2,y)    ; _Draw(x2,y1+5) ; _Plot(x2-1,y1+4) ; _Plot(x2-4,y1+1) ; _Move(x2-1,y1+3) ; _Draw(x2-3,y1+1);
                    _Move(x1+5,y1) ; _Draw(x2-5,y1);

                    _APen(pens[FV_Pen_Shine]);
                    _Move(x1+1,y)    ; _Draw(x1+1,y1+5);
                    _Move(x1+2,y1+4) ; _Draw(x1+2,y1+3);
                    _Move(x1+3,y1+2) ; _Draw(x1+4,y1+2);
                    _Move(x1+5,y1+1) ; _Draw(x2-5,y1+1);

                    _APen(pens[FV_Pen_Shadow])     ; _Plot(x2,y+1)    ; _Plot(x1,y1+4); _Plot(x1+4,y1); _Plot(x2-4,y1); _Plot(x2,y1+4); _Move(x2-1,y) ; _Draw(x2-1,y1+5) ; _Plot(x2-3,y1+2) ; _Plot(x2-2,y1+3);
                    _APen(pens[FV_Pen_HalfShine])  ; _Plot(x2-4,y1+1) ; _Move(x1,y+2) ; _Draw(x1+2,y) ; _Draw(x1+2,y1+5) ; _Draw(x1+5,y1+2) ; _Plot(x1+3,y1+3);
                    _APen(pens[FV_Pen_HalfShadow]) ; _Move(x2,y+2)    ; _Draw(x2-2,y) ; _Draw(x2-2,y1+4) ; _Draw(x2-4,y1+2);
                }
                else
                {
                    /* erase the above the tab */

                    F_Erase(_parent,x1,y1,(node -> Next) ? x1 + w : x2,y1 + 1,0);

                    x1 = x1 + 4;
                    y1 = y1 + 2;
                    x2 = x1 + w - 9 ; td_Rect.y1 = y1 + 3;

                    F_Erase(_parent,x1,y1,x1+2,y1+2,0);
                    F_Erase(_parent,x2-2,y1,x2,y1+2,0);

                    _APen(pens[FV_Pen_HalfShadow]);
                    _Boxf(x1+2,y1+1,x2-2,y1+3);
                    _Boxf(x1+1,y1+4,x2-1,y);

                    _APen(pens[FV_Pen_Shine]);
                    _Move(x1,y)      ; _Draw(x1,y1+4);
                    _Move(x1+1,y1+3) ; _Draw(x1+1,y1+2);
                    _Move(x1+2,y1+1) ; _Draw(x1+3,y1+1);
                    _Move(x1+4,y1)   ; _Draw(x2-4,y1);

                    _APen(pens[FV_Pen_Dark])      ; _Move(x2,y)      ; _Draw(x2,y1+4)   ; _Plot(x2-1,y1+2);
                    _APen(pens[FV_Pen_HalfDark])  ; _Plot(x2-2,y1+1) ; _Plot(x2,y1+3);
                    _APen(pens[FV_Pen_Shadow])    ; _Move(x2-1,y)    ; _Draw(x2-1,y1+3) ; _Draw(x2-3,y1+1);
                    _APen(pens[FV_Pen_Fill])      ; _Move(x1+1,y1+4) ; _Draw(x1+4,y1+1) ; _Plot(x1+2,y1+2);
                    _APen(pens[FV_Pen_HalfShine]) ; _Plot(x2-3,y1)   ; _Plot(x1,y1+3)   ; _Plot(x1+3,y1);
                }

                /*** clear space to the left and space to the right ***/

                F_Erase(_parent,back_x1,y1,x1 - 1,y,0);
                F_Erase(_parent,x2 + 1,y1,(node -> Next) ? back_x1 + w : back_x2,y,0);

                td_Rect.x1 = x1+5; td_Rect.x2 = x2-5;
                td_Rect.y2 = td_Rect.y1 + LOD -> PageData -> Font -> tf_YSize;

                x2 = back_x2;
                y1 = back_y1;
                x1 = back_x1;

                F_Do(LOD -> PageData -> TD,FM_Set,

                    FA_TextDisplay_PreParse, (node == LOD -> PageData -> ActiveNode) ? LOD -> PageData -> APrep : LOD -> PageData -> IPrep,
                    FA_TextDisplay_Contents, F_Get(node -> Object,FA_Group_PageTitle),

                TAG_DONE);

                F_Do(LOD -> PageData -> TD,FM_TextDisplay_Draw,&td_Rect);
            }

            x1 += w;
        }

        back_x2 = x2;

        x1 = x - w + 3;
        x2 = x - 2;

        /* erase the line separating the active tab and the group */

        F_Erase(Obj,x1+1,y+1,x2-1,y+2,0);

        _APen(pens[FV_Pen_Shine]);       _Plot(x1,y+1);
        _APen(pens[FV_Pen_HalfShine]);   _Plot(x1+1,y+1);  _Plot(x1,y+2);
        _APen(pens[FV_Pen_HalfShadow]);  _Plot(x2-1,y+1);  _Plot(x2,y+2);
        _APen(pens[FV_Pen_Shadow]);      _Plot(x2,y+1);

        x2 = back_x2;

        if (LOD -> PageData -> ActiveNode == (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head))
        {
            _APen(pens[FV_Pen_Dark])  ; _Plot(_x,y+2)   ; _Plot(_x+1,y+1);
            _APen(pens[FV_Pen_Shine]) ; _Plot(_x+1,y+2);
            _APen(pens[FV_Pen_Fill])  ; _Plot(_x,y+1);
        }
        else
        {
            _APen(pens[FV_Pen_Dark])  ; _Move(_x,y+1)   ; _Draw(x-w-1+3,y+1) ; _Plot(_x,y+2);
            _APen(pens[FV_Pen_Shine]) ; _Move(_x+1,y+2) ; _Draw(x-w-1+3,y+2);
        }

        _APen(pens[FV_Pen_Dark])  ; _Move(x-1,y+1) ; _Draw(x2,y+1)   ; _Plot(x2,y+2);
        _APen(pens[FV_Pen_Shine]) ; _Move(x-1,y+2) ; _Draw(x2-2,y+2);
    }
/*
    if (LOD -> PageData -> ActiveNode)
    {
        F_Draw(LOD -> PageData -> ActiveNode -> Object,FF_Draw_Object);
    }
*/
}
//+

/*** Methods ***************************************************************/

///Group_Draw
F_METHODM(void, Group_Draw, FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (FF_GROUP_CHANGING & LOD -> Flags)
    {
        return;
    }

    #if 0
    if (FF_Draw_Damaged & Msg->Flags)
    {
        F_Log(0,"overlord damaged only");
    }
    #endif

 
    if (LOD->PageData)
    {
        if (FF_Area_Damaged & _flags)
        {
            draw_page(Class, Obj, Method, Msg, FeelinBase);
        }
    }
    else
    {
        draw_stnd(Class, Obj, Method, Msg, FeelinBase);
    }
 
    #if 0
    if (((FF_Draw_Damaged & Msg->Flags) && (FF_Area_Damaged & _flags)) ||
        ((FF_Draw_Damaged & Msg->Flags) == 0))
    {
    }
    #endif
    
    if (LOD->PageData)
    {
        FAreaNode *node = LOD->PageData->ActiveNode;
 
        if (node)
        {
            #if 1
            if (FF_Draw_Damaged & Msg->Flags)
            {
                if ((FF_Area_Group & _sub_flags) == 0)
                {
                    if ((FF_Area_Damaged & _sub_flags) == 0)
                    {
                        return;
                    }
                }

                F_Draw(node->Object, Msg->Flags);
            }
            else
            #endif
            {
                F_Draw(node->Object, FF_Draw_Object);
            }
        }
    }
    else
    {
        FAreaNode *node;

        #if 1
        if (FF_Draw_Damaged & Msg->Flags)
        {
            for _each
            {
                if ((FF_Area_Group & _sub_flags) == 0)
                {
                    if ((FF_Area_Damaged & _sub_flags) == 0)
                    {
                        continue;
                    }
                }

                F_Draw(node->Object, Msg->Flags);
            }
        }
        else
        #endif
        {
            for _each
            {
                F_Draw(node->Object, Msg->Flags);
            }
        }
    }
 
}
//+

