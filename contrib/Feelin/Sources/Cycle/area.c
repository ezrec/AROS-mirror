#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///cycle_item_draw
void cycle_item_draw(FClass *Class,FObject Obj,uint32 Num,bits32 Flags)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD->PMD)
    {
        FRect r;
        FBox  b;
        uint32 f=0;
        uint16 th;
      
        r.x1 = LOD->PMD->menu_brd.l;
        r.y1 = Num * LOD->PMD->item_height + LOD->PMD->menu_brd.t;
        r.x2 = LOD->PMD->Win->Width - LOD->PMD->menu_brd.r;
        r.y2 = r.y1 + LOD->PMD->item_height - 1;
        
        b.x = r.x1;
        b.y = r.y1;
        b.w = r.x2 - r.x1 + 1;
        b.h = r.y2 - r.y1 + 1;
        
        if (Num == LOD->PMD->Active)
        {
            f = 1;

            F_Do(LOD->PMD->item_frame,FM_Frame_Draw,LOD->PMD->Render,&b,FF_Frame_Draw_Select | FF_Frame_Draw_Erase);
        }
        else
        {
            if (!Flags)
            {
                F_Do(LOD->PMD->menu_back,FM_ImageDisplay_Draw,LOD->PMD->Render,&r,0);
            }

            F_Do(LOD->PMD->item_frame,FM_Frame_Draw,LOD->PMD->Render,&b,FF_Frame_Draw_Only);
        }

        r.x1 += LOD->PMD->item_brd[f].l;
        r.y1 += LOD->PMD->item_brd[f].t;
        r.x2 -= LOD->PMD->item_brd[f].l - LOD->PMD->item_brd[f].r;
        r.y2 -= LOD->PMD->item_brd[f].t - LOD->PMD->item_brd[f].b;

        F_Set(LOD->PMD->TD,FA_TextDisplay_Contents,(uint32) LOD->strings[Num]);

        th = F_Get(LOD->PMD->TD,FA_TextDisplay_Height);

        if (th < r.y2 - r.y1 + 1)
        {
            r.y1 = (r.y2 - r.y1 + 1 - th) / 2 + r.y1;
        }
        
        F_Do(LOD->PMD->TD,FM_TextDisplay_Draw,&r);
    }
}
//+
///cycle_menu_create
struct PopMenuData * cycle_menu_create(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct PopMenuData *pmd;

    if (LOD->PMD)
    {
        return NULL;
    }

    pmd = F_New(sizeof (struct PopMenuData));
    
    if (!pmd)
    {
        return NULL;
    }
    
    if (FF_CYCLE_BELOW & LOD->flags)
    {
        pmd->Active = 0xFFFF;
    }
    else
    {
        pmd->Active = LOD->active;
    }

    /* the render object is used to draw images, frames and text on the
    popmenu window */
  
    pmd->Render = RenderObject,
    
        FA_Render_Friend, _render,
        FA_Render_Palette, F_Get(Obj,FA_ColorScheme),
       
        End;
  
    pmd->TD = TextDisplayObject,

        FA_TextDisplay_PreParse, LOD->preparse,
        FA_TextDisplay_Shortcut, FALSE,
        FA_TextDisplay_Font, _font,

        End;
    
    pmd->menu_frame = FrameObject,
    
        FA_Frame, F_Do(_app,FM_Application_Resolve,"$cycle-menu-frame",DEF_CYCLE_MENU_FRAME),
        FA_Back, F_Do(_app,FM_Application_Resolve,"$cycle-menu-back",DEF_CYCLE_MENU_BACK),
    
        End;

    pmd->item_frame = FrameObject,

        FA_Frame, F_Do(_app,FM_Application_Resolve,"$cycle-item-frame",DEF_CYCLE_ITEM_FRAME),
        FA_Back, F_Do(_app,FM_Application_Resolve,"$cycle-item-back",DEF_CYCLE_ITEM_BACK),
    
        End;

    /* check objects creation */

    if (pmd->Render && pmd->TD && pmd->menu_frame && pmd->item_frame)
    {
        FFramePublic *fp;
   
        F_Do(pmd->menu_frame,FM_Frame_Setup,pmd->Render);
        
        if ((fp = (FFramePublic *) F_Get(pmd->menu_frame,FA_Frame_PublicData)) != NULL)
        {
            pmd->menu_brd.l = fp->Border[0].l + fp->Padding[0].l;
            pmd->menu_brd.t = fp->Border[0].t + fp->Padding[0].t;
            pmd->menu_brd.r = fp->Border[0].r + fp->Padding[0].r;
            pmd->menu_brd.b = fp->Border[0].b + fp->Padding[0].b;

            pmd->menu_back = fp->Back;
        }
   
        F_Do(pmd->item_frame,FM_Frame_Setup,pmd->Render);
        
        if ((fp = (FFramePublic *) F_Get(pmd->item_frame,FA_Frame_PublicData)) != NULL)
        {
            pmd->item_brd[0].l = fp->Border[0].l + fp->Padding[0].l;
            pmd->item_brd[0].t = fp->Border[0].t + fp->Padding[0].t;
            pmd->item_brd[0].r = fp->Border[0].r + fp->Padding[0].r;
            pmd->item_brd[0].b = fp->Border[0].b + fp->Padding[0].b;
            
            pmd->item_brd[1].l = fp->Border[1].l + fp->Padding[1].l;
            pmd->item_brd[1].t = fp->Border[1].t + fp->Padding[1].t;
            pmd->item_brd[1].r = fp->Border[1].r + fp->Padding[1].r;
            pmd->item_brd[1].b = fp->Border[1].b + fp->Padding[1].b;
        }
     
        F_Set(pmd->menu_back,FA_ImageDisplay_Origin,(uint32) &pmd->menu_box);
   
        if (F_Do(pmd->TD,FM_TextDisplay_Setup,pmd->Render))
        {
            STRPTR *en;

            for (en = LOD->strings ; *en ; en++)
            {
               uint32 h;
                           
               F_Set(pmd->TD,FA_TextDisplay_Contents,(uint32) *en);
               h = F_Get(pmd->TD,FA_TextDisplay_Height);
                                          
               pmd->item_height = MAX(pmd->item_height,h);
            }
         
            pmd->item_height +=
               MAX(pmd->item_brd[0].t,pmd->item_brd[1].t) +
               MAX(pmd->item_brd[0].b,pmd->item_brd[1].b);
               
            pmd->item_width = _text_w +
               MAX(pmd->item_brd[0].l,pmd->item_brd[1].l) +
               MAX(pmd->item_brd[0].r,pmd->item_brd[1].r);

         
            pmd->WinBox.x = _text_x - pmd->menu_brd.l + F_Get(_win,FA_Left) - pmd->item_brd[1].l;

            if (FF_CYCLE_BELOW & LOD->flags)
            {
               pmd->WinBox.y = _y2 + F_Get(_win,FA_Top);
            }
            else
            {
               pmd->WinBox.y = _text_y - pmd->menu_brd.t + F_Get(_win,FA_Top) - pmd->Active * pmd->item_height - pmd->item_brd[1].t;
            }

            pmd->WinBox.w = pmd->item_width + pmd->menu_brd.l + pmd->menu_brd.r;
            pmd->WinBox.h = pmd->item_height * LOD->numstrings + pmd->menu_brd.t + pmd->menu_brd.b;
                     
            pmd->Win = OpenWindowTags(NULL,
                              
                WA_Left,          pmd->WinBox.x,
                WA_Top,           pmd->WinBox.y,
                WA_Width,         pmd->WinBox.w,
                WA_Height,        pmd->WinBox.h,
                                                   
                WA_AutoAdjust,    TRUE,
                WA_BackFill,      LAYERS_NOBACKFILL,
                WA_CustomScreen,  F_Get(_display,(uint32) "FA_Display_Screen"),

                WA_Activate,      FALSE,
                WA_Borderless,    TRUE,

                WA_DragBar,       FALSE,
                WA_DepthGadget,   FALSE,
                WA_CloseGadget,   FALSE,
                WA_SizeGadget,    FALSE,
                               
                TAG_DONE);
               
            if (pmd->Win)
            {
                FRect r;
                uint32 i=0;
                
                F_Set(pmd->Render,FA_Render_RPort,(uint32) pmd->Win->RPort);
                               
                pmd->menu_box.x = 0;
                pmd->menu_box.y = 0;
                pmd->menu_box.w = pmd->Win->Width;
                pmd->menu_box.h = pmd->Win->Height;
                
                r.x1 = pmd->menu_box.x;
                r.y1 = pmd->menu_box.y;
                r.x2 = r.x1 + pmd->menu_box.w - 1;
                r.y2 = r.y1 + pmd->menu_box.h - 1;

                F_Do(pmd->menu_back,FM_ImageDisplay_Draw,pmd->Render,&r,0);
                F_Do(pmd->menu_frame,FM_Frame_Draw,pmd->Render,&pmd->menu_box,0);
     
                LOD->PMD = pmd;

                for (en = LOD->strings ; *en ; en++)
                {
                    cycle_item_draw(Class,Obj,i++,1);
                }

                F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                            
                return LOD->PMD;
            }
                  
            F_Do(pmd->TD,FM_TextDisplay_Cleanup);
        }
        F_Do(pmd->item_frame,FM_Frame_Cleanup);
        F_Do(pmd->menu_frame,FM_Frame_Cleanup);
    }
    
    F_DisposeObj(pmd->item_frame);
    F_DisposeObj(pmd->menu_frame);
    F_DisposeObj(pmd->TD);
    F_DisposeObj(pmd->Render);
    F_Dispose(pmd);

    return NULL;
}
//+
///cycle_menu_delete
void cycle_menu_delete(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD->PMD)
    {
        struct PopMenuData *pmd = LOD->PMD;
        LOD->PMD = NULL;
        
        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);
   
        CloseWindow(pmd->Win);

        F_Do(pmd->item_frame,FM_Frame_Cleanup,pmd->Render);
        F_DisposeObj(pmd->item_frame);
        
        F_Do(pmd->menu_frame,FM_Frame_Cleanup,pmd->Render);
        F_DisposeObj(pmd->menu_frame);
        
        F_DisposeObj(pmd->TD);
        F_DisposeObj(pmd->Render);
        
        F_Dispose(pmd);
    }
}
//+
///cycle_menu_handle
uint32 cycle_menu_handle(FClass *Class,FObject Obj,FEvent *fev)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (fev->Key)
    {
        uint32 prev = LOD->PMD->Active;
        uint32 active = LOD->PMD->Active;
   
        switch (fev->Key)
        {
            case FV_KEY_UP:
            {
                if (active == 0)
                {
                    active = LOD->numstrings - 1;
                }
                else
                {
                    active--;
                }
            }
            break;
             
            case FV_KEY_DOWN:    
            {
                if (active == LOD->numstrings - 1)
                {
                    active = 0;
                }
                else
                {
                    active++;
                }
            }
            break;
             
            case FV_KEY_TOP:
            {
                active = 0;
            }
            break;
            
            case FV_KEY_BOTTOM:
            {
                active = LOD->numstrings - 1;
            }
            break;

            case FV_KEY_PRESS:
            {
                active = LOD->PMD->Active;
                
                cycle_menu_delete(Class,Obj);

                F_Set(Obj,F_IDA(FA_Cycle_Active),active);
                
                return FF_HandleEvent_Eat;
            }
            break;
         
            default: return 0;
        }
     
        if (active != prev)
        {
            LOD->PMD->Active = active;

            cycle_item_draw(Class,Obj,prev,0);
      
            ChangeWindowBox
            (
                LOD->PMD->Win,
                
                LOD->PMD->WinBox.x,
                _text_y - LOD->PMD->menu_brd.t + F_Get(_win,FA_Top) - LOD->PMD->Active * LOD->PMD->item_height - LOD->PMD->item_brd[1].t,
                LOD->PMD->WinBox.w,
                LOD->PMD->WinBox.h
            );
               
            cycle_item_draw(Class,Obj,active,0);
        }
           
        return FF_HandleEvent_Eat;
    }
    else if (fev->Class == FF_EVENT_BUTTON)
    {
        int16 msex = fev->MouseX + F_Get(_win,FA_Left) - LOD->PMD->Win->LeftEdge;
        int16 msey = fev->MouseY + F_Get(_win,FA_Top) - LOD->PMD->Win->TopEdge;

        if (msex >= 0 && msex <= LOD->PMD->Win->Width - 1 &&
            msey >= 0 && msey <= LOD->PMD->Win->Height - 1)
        {
            uint32 active = LOD->PMD->Active;
        
            cycle_menu_delete(Class,Obj);
            
            F_Set(Obj,F_IDA(FA_Cycle_Active),active);
        }
        else
        {
            cycle_menu_delete(Class,Obj);
        }

        return FF_HandleEvent_Eat;
    }
    else if (fev->Class == FF_EVENT_MOTION)
    {
        int16 msex = fev->MouseX + F_Get(_win,FA_Left) - LOD->PMD->Win->LeftEdge;
        int16 msey = fev->MouseY + F_Get(_win,FA_Top) - LOD->PMD->Win->TopEdge;
        
        int32 active = (msey - LOD->PMD->menu_brd.t) / LOD->PMD->item_height;

        if (
           active >= 0 &&
           active < LOD->numstrings &&
           
           msex >= LOD->PMD->menu_brd.l &&
           msex <= LOD->PMD->Win->Width  - LOD->PMD->menu_brd.r &&
           msey >= LOD->PMD->menu_brd.t &&
           msey <= LOD->PMD->Win->Height - LOD->PMD->menu_brd.b
           )
        {
            uint32 prev = LOD->PMD->Active;
               
            if (active != prev)
            {
                LOD->PMD->Active = active;

                if (prev != 0xFFFF)
                {
                    cycle_item_draw(Class,Obj,prev,0);
                }
                cycle_item_draw(Class,Obj,active,0);
            }
        }
        else if (LOD->PMD->Active != 0xFFFF)
        {
            active = LOD->PMD->Active;
            LOD->PMD->Active = 0xFFFF;
           
            cycle_item_draw(Class,Obj,active,0);
        }
    }
    return 0;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Cycle_Setup
F_METHODM(uint32,Cycle_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
     
    uint32 layout = F_Do(Msg->Render->Application,FM_Application_ResolveInt,LOD->p_Layout,FV_Cycle_Layout_Right);

    if (layout == FV_Cycle_Layout_Right)
    {
        F_Do(Obj,FM_AddMember,LOD->image,FV_AddMember_Tail);
        F_Do(Obj,FM_AddMember,LOD->text,FV_AddMember_Tail);
    }
    else
    {
        F_Do(Obj,FM_AddMember,LOD->text,FV_AddMember_Tail);
        F_Do(Obj,FM_AddMember,LOD->image,FV_AddMember_Tail);
    }

    if (F_SUPERDO())
    {
        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);
                 
        LOD->level = F_Do(_app, FM_Application_ResolveInt, LOD->p_level, DEF_CYCLE_LEVEL);
        LOD->preparse = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD->p_PreParse,NULL);
        
        F_Set(LOD->text,FA_Text_PreParse,(uint32) LOD->preparse);
        
        if (F_Do(_app,FM_Application_ResolveInt,LOD->p_Position,DEF_CYCLE_POSITION))
        {
            LOD->flags |= FF_CYCLE_BELOW;
        }
        else
        {
            LOD->flags &= ~FF_CYCLE_BELOW;
        }
   
        return TRUE;
    }
    return FALSE;
}
//+
///Cycle_Cleanup
F_METHOD(void,Cycle_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);
    
    if (_render)
    {
        cycle_menu_delete(Class,Obj);

        F_Do(Obj,FM_RemMember,LOD->image);
        F_Do(Obj,FM_RemMember,LOD->text);
     
        F_SUPERDO();
    }
}
//+
///Cycle_HandleEvent
F_METHODM(uint32,Cycle_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    FEvent *fev = Msg->Event;
    
    if (LOD->PMD)
    {
        return cycle_menu_handle(Class,Obj,fev);
    }
  
    if (fev->Key)
    {
        uint32 active;

        switch (fev->Key)
        {
            case FV_KEY_UP:      active = FV_Cycle_Active_Prev;    break;
            case FV_KEY_DOWN:    active = FV_Cycle_Active_Next;    break;
            case FV_KEY_TOP:     active = FV_Cycle_Active_First;   break;
            case FV_KEY_BOTTOM:  active = FV_Cycle_Active_Last;    break;
            case FV_KEY_PRESS:   active = FV_Cycle_Active_Next;
            {
                if (LOD->numstrings >= LOD->level)
                {
                    if (cycle_menu_create(Class,Obj))
                    {
                        return FF_HandleEvent_Eat;
                    }
                }
            }
            break;
            
            default:
            {
                return 0;
            }
        }

        F_Set(Obj,F_IDA(FA_Cycle_Active),active);

        return FF_HandleEvent_Eat;
    }
    else if (fev->Class == FF_EVENT_BUTTON)
    {
        if (fev->Code == FV_EVENT_BUTTON_SELECT)
        {
            if (FF_EVENT_BUTTON_DOWN & fev->Flags)
            {
                if (_inside(fev->MouseX,_x,_x2) &&
                    _inside(fev->MouseY,_y,_y2))
                {
                    if (_inside(fev->MouseX,_text_x,_text_x + _text_w - 1) &&
                        LOD->numstrings >= LOD->level)
                    {
                       if (cycle_menu_create(Class,Obj))
                       {
                          return FF_HandleEvent_Eat;
                       }
                    }

                    F_Do(Obj,FM_Set, FA_Selected,TRUE, FA_Pressed,TRUE, TAG_DONE);

                    F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION | FF_EVENT_TICK,0);

                    return FF_HandleEvent_Eat;
                }
            }
            else if (FF_Area_Pressed & _flags)
            {
                if (FF_Area_Selected & _flags)
                {
                    F_Set(Obj,FA_Selected,FALSE);
                }

                if (_inside(fev->MouseX,_x,_x2) &&
                    _inside(fev->MouseY,_y,_y2))
                {
                    F_Do(Obj,FM_Set,
                        
                        F_IDA(FA_Cycle_Active), ((IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT) & fev->Qualifier) ? FV_Cycle_Active_Prev : FV_Cycle_Active_Next,
                        FA_Pressed, FALSE,
                        
                        TAG_DONE);
                }
                else
                {
                    _flags &= ~FF_Area_Pressed;
                }

                F_Do(Obj,FM_ModifyHandler,NULL,FF_EVENT_MOTION | FF_EVENT_TICK);

                return FF_HandleEvent_Eat;
            }
        }
        else if (fev->Code == FV_EVENT_BUTTON_WHEEL && _inside(fev->MouseX,_x,_x2) && _inside(fev->MouseY,_y,_y2))
        {
           F_Set(Obj,F_IDA(FA_Cycle_Active),(FF_EVENT_BUTTON_DOWN & fev->Flags) ? FV_Cycle_Active_Next : FV_Cycle_Active_Prev);
        }
    }
    else if (fev->Class == FF_EVENT_MOTION)
    {
        if (FF_Area_Pressed & _flags)
        {
            if (_inside(fev->MouseX,_x,_x2) &&
                _inside(fev->MouseY,_y,_y2))
            {
                if (!(FF_Area_Selected & _flags)) F_Set(Obj,FA_Selected,TRUE);
            }
            else
            {
                if (FF_Area_Selected & _flags) F_Set(Obj,FA_Selected,FALSE);
            }
        }
    }
    return 0;
}
//+
///Cycle_GoActive
F_METHOD(void,Cycle_GoActive)
{
   F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);

   F_SUPERDO();
}
//+
///Cycle_GoInactive
F_METHOD(void,Cycle_GoInactive)
{
   F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);

   F_SUPERDO();
}
//+

