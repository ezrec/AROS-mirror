#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

#define _parent_init                            FAreaPublic *parent_area_public = (FAreaPublic *) F_Get(_parent,FA_Area_PublicData)
#define _parent_x                               (parent_area_public -> Box.x)
#define _parent_y                               (parent_area_public -> Box.y)
#define _parent_w                               (parent_area_public -> Box.w)
#define _parent_h                               (parent_area_public -> Box.h)
#define _parent_flags                           parent_area_public->Flags

#define FF_COMPUTE_W                            FF_Area_Reserved1
#define FF_COMPUTE_H                            FF_Area_Reserved2

///balance_begin
void balance_begin(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FAreaNode *area,*node,*head,*tail;
    BOOL side;
    uint32 spc;

    F_Do(_parent,FM_Get,
        
        FA_Family_Head, &head,
        FA_Family_Tail, &tail,
        (FF_Balance_Vertical & LOD -> Flags) ? FA_Group_HSpacing : FA_Group_VSpacing, &spc,
        
        TAG_DONE);

    for (node = head ; node ; node = node -> Next)
    {
        if (node -> Object == Obj) break;
    }

    if (FF_Balance_Moving & LOD -> Flags)
    {
        FObject active;

        if (!(FF_Render_Complex & _render -> Flags))
        {
            LOD -> Flags |= FF_Balance_ComplexSet;

            _render -> Flags |= FF_Render_Complex;
        }
        else
        {
            LOD -> Flags &= ~FF_Balance_ComplexSet;
        }

        if (FF_Balance_QuickDraw & LOD -> Flags)
        {
            F_Set(_render,FA_Render_Forbid,TRUE);
        }

        LOD -> ActiveObj = NULL;

        if ((active = (FObject) F_Get(_win,FA_Window_ActiveObject)) != Obj)
        {
            LOD -> ActiveObj = active;

            F_Set(_win,FA_Window_ActiveObject,NULL);
        }
    }

    LOD -> BNum = 0; LOD -> BMin = 0; LOD -> BMax = 0;
    LOD -> ANum = 0; LOD -> AMin = 0; LOD -> AMax = 0;

    side = 0;

    for _each
    {
        if (area -> Object == Obj)
        {
            side = 1;
        }
        else if (side)
        {
            LOD -> ANum++;
            LOD -> AMin += (FF_Balance_Vertical & LOD -> Flags) ? _area_minw : _area_minh;
            LOD -> AMax += (FF_Balance_Vertical & LOD -> Flags) ? _area_maxw : _area_maxh;
        }
        else
        {
            LOD -> BNum++;
            LOD -> BMin += (FF_Balance_Vertical & LOD -> Flags) ? _area_minw : _area_minh;
            LOD -> BMax += (FF_Balance_Vertical & LOD -> Flags) ? _area_maxw : _area_maxh;
        }
    }

    if (node -> Prev)
    {
        FAreaNode *prev = node -> Prev;

        LOD -> BSize = (FF_Balance_Vertical & LOD -> Flags)
            ? (_prev_x2 - _head_x + 1) - ((LOD -> BNum - 1) * spc)
            : (_prev_y2 - _head_y + 1) - ((LOD -> BNum - 1) * spc);
    }
    else LOD -> BSize = 0;

    if (node -> Next)
    {
        FAreaNode *next = node -> Next;

        LOD -> ASize = (FF_Balance_Vertical & LOD -> Flags)
            ? (_tail_x2 - _next_x + 1) - ((LOD -> ANum - 1) * spc)
            : (_tail_y2 - _next_y + 1) - ((LOD -> ANum - 1) * spc);
    }
    else LOD -> BSize = 0;

//   F_DebugOut("INIT: Min %ld,%ld - Max %ld,%ld - Size %ld,%ld - Render 0x%lx\n",LOD -> BMin,LOD -> AMin,LOD -> BMax,LOD -> BMin,LOD -> BSize,LOD -> ASize,_render(Obj));
}
//+
///balance_done
void balance_done(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FAreaNode *area;

    LOD -> Flags &= ~FF_Balance_Moving;

//   balance_set_weight(Class,Obj);

    for (area = (FAreaNode *) F_Get(_parent,FA_Family_Head) ; area ; area = area -> Next)
    {
        if (area -> Object != Obj)
        {
            _area_weight = (FF_Balance_Vertical & LOD -> Flags) ? _area_w : _area_h;
        }
    }

    if (FF_Balance_ComplexSet & LOD -> Flags)
    {
        _render -> Flags &= ~FF_Balance_ComplexSet;

        LOD -> Flags &= ~FF_Balance_ComplexSet;
    }

    if (FF_Balance_QuickDraw & LOD -> Flags)
    {
        F_Set(_render,FA_Render_Forbid,FALSE);
        F_Do(_parent,FM_Layout);
        F_Draw(_parent,FF_Draw_Object);
    }

    if (LOD -> ActiveObj && LOD -> ActiveObj != Obj)
    {
        F_Set(_win,FA_Window_ActiveObject,(uint32) LOD -> ActiveObj);
    }
}
//+
///balance_modify

void balance_modify(FClass *Class,FObject Obj,LONG Diff)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FAreaNode              *area,*head,*tail;
    uint32                   hspace,vspace;
    BOOL                    side;
    uint32                   BSize,ASize,BNum,ANum,pos;
    UWORD                   loss,size;

    F_Do(_parent,FM_Get,
        
        FA_Family_Head,     &head,
        FA_Family_Tail,     &tail,
        FA_Group_HSpacing,  &hspace,
        FA_Group_VSpacing,  &vspace,
        
        TAG_DONE);

    BNum = LOD -> BNum; BSize = LOD -> BSize + Diff;
    ANum = LOD -> ANum; ASize = LOD -> ASize - Diff;

    pos = (FF_Balance_Vertical & LOD -> Flags) ? _x : _y;

    if (BSize < LOD -> BMin) { ASize += (BSize - LOD -> BMin); BSize = LOD -> BMin; }
    if (ASize < LOD -> AMin) { BSize += (ASize - LOD -> AMin); ASize = LOD -> AMin; }
    if (BSize > LOD -> BMax)   return;
    if (ASize > LOD -> AMax)   return;

/*** On s'occupe d'abord des Objets fixes **********************************/

    side = 0;

    for _each
    {
        if (area -> Object == Obj)
        {
            side = 1;
        }
        else
        {
            if (FF_Balance_Vertical & LOD -> Flags)
            {
                if (_area_minw == _area_maxw)
                {
                    if (side) { ASize -= _area_w; ANum--; }
                    else      { BSize -= _area_w; BNum--; }
                }
                else _area_flags |= FF_COMPUTE_W;
            }
            else
            {
                if (_area_minh == _area_maxh)
                {
                    if (side) { ASize -= _area_h; ANum--; }
                    else      { BSize -= _area_h; BNum--; }
                }
                else _area_flags |= FF_COMPUTE_H;
            }
        }
    }

/*** Vérification des tailles minimales et maximales ***********************/

    side = 0;

    for _each
    {
        if (area -> Object == Obj)
        {
            side = 1;
        }
        else
        {
            if (FF_Balance_Vertical & LOD -> Flags)
            {
                if (FF_COMPUTE_W & _area_flags)
                {
                    size = (side) ? ASize / ANum : BSize / BNum;

                    if (_area_minw > size)
                    {
                        if (_area_w != _area_minw)
                        {
                            _area_w = _area_minw;
                            _area_flags |= FF_Area_Damaged;
                        }
                        _area_flags &= ~FF_COMPUTE_W;
                    }
                    else if (_area_maxw < size)
                    {
                        if (_area_w != _area_maxw)
                        {
                            _area_w = _area_maxw;
                            _area_flags |= FF_Area_Damaged;
                        }
                        _area_flags &= ~FF_COMPUTE_W;
                    }
                    else continue;//goto __compute_again;

                    if (side) { ASize -= _area_w; ANum--;}
                    else      { BSize -= _area_w; BNum--;}

                    area = head ; side = 0;
                }
            }
            else
            {
                if (FF_COMPUTE_H & _area_flags)
                {
                    size = (side) ? ASize / ANum : BSize / BNum;

                    if (_area_minh > size)
                    {
                        if (_area_h != _area_minh)
                        {
                            _area_h = _area_minh;
                            _area_flags |= FF_Area_Damaged;
                        }
                        _area_flags &= ~FF_COMPUTE_H;
                    }
                    else if (_area_maxh < size)
                    {
                        if (_area_h != _area_maxh)
                        {
                            _area_h = _area_maxh;
                            _area_flags |= FF_Area_Damaged;
                        }
                        _area_flags &= ~FF_COMPUTE_H;
                    }
                    else continue;//goto __compute_again;

                    if (side) { ASize -= _area_h; ANum--;}
                    else      { BSize -= _area_h; BNum--;}

                    area = head ; side = 0;
                }
            }
        }
//__compute_again:;
    }

    loss = ASize - ((ASize / ANum) * ANum);

    if (ASize < 0) { BSize += ASize; ASize = 0; }
    if (BSize < 0) { ASize += BSize; BSize = 0; }

    side = 0;

    for _each
    {
        if (area -> Object == Obj)
        {
            side = 1;
        }
        else
        {
            if (FF_Balance_Vertical & LOD -> Flags)
            {
                if (FF_COMPUTE_W & _area_flags)
                {
                    uint16 w = (side) ? ASize / ANum : BSize / BNum;
                    
                    if (loss) { w += loss; loss = 0; }
 
                    if (_area_w != w)
                    {
                        _area_w = w;
                        _area_flags |= FF_Area_Damaged;
                    }
                    
                    _area_flags &= ~FF_COMPUTE_W;
                }
            }
            else
            {
                if (FF_COMPUTE_H & _area_flags)
                {
                    uint16 h = (side) ? ASize / ANum : BSize / BNum;

                    if (loss) { h += loss; loss = 0; }
                
                    if (_area_h != h)
                    {
                        _area_h = h;
                        _area_flags |= FF_Area_Damaged;
                    }
                    
                    _area_flags &= ~FF_COMPUTE_H;
                }
            }
        }
    }

/*** Mise en place des Objets **********************************************/

    for _each
    {
        if (_area_prev)
        {
            FAreaNode *prev = _area_prev;

            if (FF_Balance_Vertical & LOD -> Flags)
            {
                int16 x = _prev_x2 + hspace + 1;
 
                if (_area_x != x)
                {
                    _area_x = x;
                    _area_flags |= FF_Area_Damaged;
                }
            }
            else
            {
                int16 y = _prev_y2 + vspace + 1;
                
                if (_area_y != y)
                {
                    _area_y = y;
                    _area_flags |= FF_Area_Damaged;
                }
            }
        }
    }

    if (tail && loss)
    {
        if (FF_Balance_Vertical & LOD -> Flags)
        {
            _tail_x += loss;
            _tail_flags |= FF_Area_Damaged;
        }
        else
        {
            _tail_y += loss;
            _tail_flags |= FF_Area_Damaged;
        }
    }

/*** Dessin des Objets si modification *************************************/

    if (pos != ((FF_Balance_Vertical & LOD -> Flags) ? _x : _y))
    {
        _parent_init;
        _parent_flags |= FF_Area_Damaged;
        _flags |= FF_Area_Damaged;

/*** Mise à jour des sous groupes ******************************************/
/*
        if (!(FF_Balance_QuickDraw & LOD -> Flags))
        {
            for _each
            {
                if (area -> Object != Obj)
                {
                    F_Do(area -> Object,FM_Layout);
                }
            }
        }
*/
        if (FF_Balance_QuickDraw & LOD -> Flags)
        {
            if (FF_Balance_Vertical & LOD -> Flags)
            {
                F_Erase(_parent,_parent_x,_parent_y,_x - 1,_parent_y + _parent_h - 1,0);
                F_Erase(_parent,_x2 + 1,_parent_y,_parent_x + _parent_w - 1,_parent_y + _parent_h - 1,0);
            }
            else
            {
                F_Erase(_parent,_parent_x,_parent_y,_parent_x + _parent_w - 1,_y - 1,0);
                F_Erase(_parent,_parent_x,_y2 + 1,_parent_x + _parent_w - 1,_parent_y + _parent_h - 1,0);
            }

            for _each
            {
                if (area -> Object == Obj)
                {
                    F_Do(Obj, FM_Draw, FF_Draw_Object);
                }
                else //if (FF_Area_Damaged & _area_flags)
                {
                    struct RastPort *rp = _rp;

                    int16 x1 = _area_x,x2 = x1 + _area_w - 1;
                    int16 y1 = _area_y,y2 = y1 + _area_h - 1;

                    _FPen(FV_Pen_Shine);
                    _Move(x1,y1); _Draw(x2,y2);
                    _Draw(x1,y2); _Draw(x2,y1); _Draw(x1,y1);
                }
            }
        }
        else
        {
            for _each
            {
                if ((area -> Object != Obj) && (FF_Area_Damaged & _area_flags))
                {
                    F_Do(area->Object,FM_Layout);
                }
            }

            F_Draw(_parent, FF_Draw_Damaged | FF_Draw_Object);
        }
    }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Balance_HandleEvent
F_METHODM(uint32,Balance_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;

    switch (fev -> Class)
    {
///FF_EVENT_KEY
        case FF_EVENT_KEY:
            if (FF_EVENT_KEY_UP & fev -> Flags)
            {
                if (FF_Balance_Moving & LOD -> Flags)
                {
                    balance_done(Class,Obj);
                }
            }
            else if (fev -> Key)
            {
                if (fev -> Key == FV_KEY_PRESS)
                {
                    return FF_HandleEvent_Eat;
                }
                else
                {
                    WORD v;

                    if (FF_Balance_Vertical & LOD -> Flags)
                    {
                        switch (fev -> Key)
                        {
                            case FV_KEY_LEFT:       v = -5     ; break;
                            case FV_KEY_RIGHT:      v =  5     ; break;
                            case FV_KEY_STEPLEFT:   v = -20    ; break;
                            case FV_KEY_STEPRIGHT:  v =  20    ; break;
                            case FV_KEY_FIRST:      v = -10000 ; break;
                            case FV_KEY_LAST:       v =  10000 ; break;
                            default:                             return 0;
                        }
                    }
                    else
                    {
                        switch (fev -> Key)
                        {
                            case FV_KEY_UP:         v = -5    ; break;
                            case FV_KEY_DOWN:       v =  5    ; break;
                            case FV_KEY_STEPUP:     v = -20   ; break;
                            case FV_KEY_STEPDOWN:   v =  20   ; break;
                            case FV_KEY_TOP:        v = -10000; break;
                            case FV_KEY_BOTTOM:     v =  10000; break;
                            default:                            return 0;
                        }
                    }

                    if (!(FF_Balance_Moving & LOD -> Flags))
                    {
                        LOD -> Flags |= FF_Balance_Moving;
                        LOD -> Mouse  = 0;

                        balance_begin(Class,Obj);
                    }

                    LOD -> Mouse += v;

                    if (LOD -> Mouse >  10000) LOD -> Mouse =  10000;
                    if (LOD -> Mouse < -10000) LOD -> Mouse = -10000;

                    balance_modify(Class,Obj,LOD -> Mouse);

                    return FF_HandleEvent_Eat;
                }
            }
        break;
//+
///FF_EVENT_BUTTON
        case FF_EVENT_BUTTON:
        {
            if (fev -> Code == FV_EVENT_BUTTON_SELECT)
            {
                if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                {
                    if (fev -> MouseX >= _x && fev -> MouseX <= _x2 &&
                         fev -> MouseY >= _y && fev -> MouseY <= _y2)
                    {
                        LOD -> Offset  = (LOD -> Flags & FF_Balance_Vertical) ? fev -> MouseX - _x + 1 : fev -> MouseY - _y + 1;
                        LOD -> Mouse   = (LOD -> Flags & FF_Balance_Vertical) ? fev -> MouseX : fev -> MouseY;
                        LOD -> Flags  |= FF_Balance_Moving;

                        if ((IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT) & fev -> Qualifier)
                        {
                            F_Log(FV_LOG_DEV,"Couple");
                        }

                        F_Set(Obj,FA_Selected,TRUE);
                        
                        balance_begin(Class,Obj);
                        
                        F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                        
                        return FF_HandleEvent_Eat;
                    }
                }
                else
                {
                    if (FF_Balance_Moving & LOD -> Flags)
                    {
                        F_Set(Obj,FA_Selected,FALSE);
                        
                        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);
                        
                        balance_done(Class,Obj);

                        return FF_HandleEvent_Eat;
                    }
                }
            }
        }
        break;
//+
///FF_EVENT_MOTION
        case FF_EVENT_MOTION:
        {
            if (FF_Balance_Moving & LOD -> Flags)
            {
                if (FF_Balance_Vertical & LOD -> Flags)
                {
                    if (fev -> MouseX - LOD -> Offset + 1 == _x)
                    {
                       return 0;
                    }
                }
                else
                {
                    if (fev -> MouseY - LOD -> Offset + 1 == _y)
                    {
                        return 0;
                    }
                }

                balance_modify(Class,Obj,(FF_Balance_Vertical & LOD -> Flags) ? fev -> MouseX - LOD -> Mouse : fev -> MouseY - LOD -> Mouse);

                return FF_HandleEvent_Eat;
            }
        }
        break;
//+
    }
    return 0;
}
//+
