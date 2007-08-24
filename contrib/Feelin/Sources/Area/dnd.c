#include "Private.h"

struct myDnDDisplay
{
    ULONG                            Pen;
    BYTE                             Way;
};

STATIC struct myDnDDisplay          DnD;

/*** Methods ***************************************************************/

///Area_DnDDo
F_METHODM(void,Area_DnDDo,FS_DnDDo)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FObject appserver;

    if ((appserver = F_SharedOpen("AppServer")) != NULL)
    {
        F_Do(appserver,F_DynamicFindID("FM_AppServer_DnDHandle"),Obj,&_box,_render);

        F_SharedClose(appserver);
    }
}
//+
///Area_DnDQuery
F_METHODM(FObject,Area_DnDQuery,FS_DnDQuery)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
//   F_Log(0,"%s %s",(FF_AREA_DROPABLE & LOD -> Flags) ? "dropable" : "not dropable",(FF_Area_CanDraw & _flags) ? "can draw" : "cannot draw");
 
    if ((FF_AREA_DROPABLE & LOD -> Flags) &&
         (!(FF_Area_Disabled & _flags)) &&
         (FF_Area_CanDraw  & _flags) && (Obj != Msg -> Source))
    {
        if (Msg -> MouseX >= _x && Msg -> MouseX <= _x2 &&
             Msg -> MouseY >= _y && Msg -> MouseY <= _y2)
        {
//         F_Log(0,"in box");
 
            Msg -> Box -> x = _x; Msg -> Box -> w = _w;
            Msg -> Box -> y = _y; Msg -> Box -> h = _h;

            return Obj;
        }
//      F_Log(0,"not in box");
    }
    return NULL;
}
//+
///Area_DnDBegin
F_METHODM(void,Area_DnDBegin,FS_DnDBegin)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    DnD.Pen = FV_Pen_Shine;
    DnD.Way = 1;

    if (_render)
    {
        struct RastPort *rp = _render -> RPort;
            
        //F_Log(0,"palette 0x%08lx pens 0x%08lx",_palette, _pens);
 
        _APen(_pens[DnD.Pen]);
        _Move(_x - 1,_y - 1);
        _Draw(_x2 + 1,_y - 1);
        _Draw(_x2 + 1,_y2 + 1);
        _Draw(_x - 1,_y2 + 1);
        _Draw(_x - 1,_y - 1);
    }
}
//+
///Area_DnDFinish
F_METHODM(void,Area_DnDFinish,FS_DnDFinish)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        struct Region *region;
        
        ULONG x1 = _x - 1, x2 = x1 + _w + 1;
        ULONG y1 = _y - 1, y2 = y1 + _h + 1;

        if ((region = NewRegion()) != NULL)
        {
            FRect rect;

            rect.x1 = x1; rect.x2 = x2;
            rect.y1 = y1; rect.y2 = y2; OrRectRegion(region,(struct Rectangle *) &rect);

            rect.x1 += 1; rect.x2 -= 1;
            rect.y1 += 1; rect.y2 -= 1; ClearRectRegion(region,(struct Rectangle *) &rect);

            F_Do(_parent,FM_Erase,region,FF_Erase_Region);

            DisposeRegion(region);
        }
        else
        {
            F_Erase(_parent,x1,y1,x2,y1,0);
            F_Erase(_parent,x2,y1,x2,y2,0);
            F_Erase(_parent,x1,y2,x2,y2,0);
            F_Erase(_parent,x1,y1,x1,y2,0);
        }
    }
}
//+
///Area_DnDReport
F_METHODM(void,Area_DnDReport,FS_DnDReport)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Update == FV_DnDReport_Timer)
    {
        if (_render)
        {
            struct RastPort     *rp = _rp;
            struct Region       *region;
            APTR                 clip=NULL;
            WORD                 x1 = _x, x2 = x1 + _w - 1,
                                        y1 = _y, y2 = y1 + _h - 1;

            DnD.Pen += DnD.Way;

            /* Rendering are clipped to preserve the drag bob */

            if ((region = NewRegion()) != NULL)
            {
                FRect r;

                r.x1 = x1 - 1; r.x2 = x2 + 1;
                r.y1 = y1 - 1; r.y2 = y2 + 1;

                OrRectRegion(region,(struct Rectangle *) &r);

                r.x1 = Msg -> DragBox -> x ; r.x2 = r.x1 + Msg -> DragBox -> w - 1;
                r.y1 = Msg -> DragBox -> y ; r.y2 = r.y1 + Msg -> DragBox -> h - 1;
                
                XorRectRegion(region,(struct Rectangle *) &r);

                clip = (APTR) F_Do(_render,FM_Render_AddClipRegion,region);

                DisposeRegion(region);
            }
                
            //F_Log(0,"palette 0x%08lx pens 0x%08lx",_palette, _pens);

            _APen(_pens[DnD.Pen]);
            _Move(x1 - 1,y1 - 1);
            _Draw(x2 + 1,y1 - 1);
            _Draw(x2 + 1,y2 + 1);
            _Draw(x1 - 1,y2 + 1);
            _Draw(x1 - 1,y1 - 1);

            if (clip)
            {
                F_Do(_render,FM_Render_RemClip,clip);
            }

            if (DnD.Pen == FV_Pen_Shine)
            {
                DnD.Way = 1;
            }
            else if (DnD.Pen == FV_Pen_Dark)
            {
                DnD.Way = -1;
            }
        }
    }
}
//+
