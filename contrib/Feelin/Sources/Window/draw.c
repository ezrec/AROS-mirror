#include "Private.h"

//#define WIN_BUFFEREDDRAW

#ifdef WIN_BUFFEREDDRAW
///CreateRenderBuf
struct RastPort * CreateRenderBuf(struct FeelinClass *Class,APTR Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct RastPort *rp;

   if (rp = F_New(sizeof (struct RastPort)))
   {
      InitRastPort(rp);

      if (rp -> BitMap = AllocBitMap(LOD -> Win -> Width,LOD -> Win -> Height,LOD -> Render -> RPort -> BitMap -> Depth,BMF_INTERLEAVED,LOD -> Render -> RPort -> BitMap))
      {
/*
         struct FeelinRect r;

         r.x1 = 0; r.x2 = LOD -> Win -> Width - 1;
         r.y1 = 0; r.y2 = LOD -> Win -> Height - 1;

         rp -> RP_User = (APTR) F_Do(LOD -> Render,FM_Render_AddClip,&r);
*/
         return LOD -> Render -> RPort = rp;
      }
      F_Dispose(rp);
   }
}
//+
///DeleteRenderBuf
void DeleteRenderBuf(struct FeelinClass *Class,APTR Obj,struct RastPort *Handle)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Handle)
   {
      LOD -> Render -> RPort = LOD -> Win -> RPort;

      if (LOD -> Decorator)
      {
         ClipBlit(Handle,0,0,LOD -> Win -> RPort,0,0,LOD -> Win -> Width,LOD -> Win -> Height,0x0C0);
      }
      else
      {
         struct Window *win = LOD -> Win;

         ClipBlit(Handle,
                  win -> BorderLeft,win -> BorderTop,
                  win -> RPort,
                  win -> BorderLeft,win -> BorderTop,
                  win -> Width - win -> BorderLeft - win -> BorderRight,
                  win -> Height - win -> BorderTop - win -> BorderBottom,
                  0x0C0);
      }

//      F_Do(LOD -> Render,FM_Render_RemClip,Handle -> RP_User);
      FreeBitMap(Handle -> BitMap);

#ifdef __AROS__
      DeinitRastPort(Handle);
#endif
      F_Dispose(Handle);
   }
}
//+
#endif

/***************************************************************************/
/*** Private ***************************************************************/
/***************************************************************************/

#if 0
///DrRect
void DrRect(struct FeelinRender *Render,struct FeelinRect *Rect,ULONG Flags,ULONG Pen)
{
   if (FF_DrawBackground_Region & Flags)
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
#endif

/***************************************************************************/
/*** Methods ***************************************************************/
/***************************************************************************/

///Window_Draw
F_METHOD(void,Window_Draw)
{
    struct LocalObjectData     *LOD = F_LOD(Class,Obj);
    struct Window              *win = LOD -> Win;
    uint32                      x1,y1,x2,y2;
#ifdef WIN_BUFFEREDDRAW
    APTR                       buf_handle;
#endif

//    F_Log(0,"forbid (%ld) - rethink (%ld)",F_Get(LOD->Render, FA_Render_Forbid), LOD->RethinkNest);
    
    if (FF_Render_Forbid & LOD->Render->Flags)
    {
        LOD->SYSFlags |= FF_WINDOW_SYS_REFRESH_NEED;
        
        return;
    }

    LOD->SYSFlags &= ~FF_WINDOW_SYS_REFRESH_NEED;

    if (_deco_area_public)
    {
        F_Draw(LOD -> Decorator,FF_Draw_Object);
   
        x1 = _deco_il;
        x2 = win -> Width  - _deco_ir - 1;
        y1 = _deco_it;
        y2 = win -> Height - _deco_ib - 1;
    }
    else
    {
        x1 = win -> BorderLeft;
        x2 = win -> Width  - win -> BorderRight  - 1;
        y1 = win -> BorderTop;
        y2 = win -> Height - win -> BorderBottom - 1;
    }

#ifdef WIN_BUFFEREDDRAW
    buf_handle = CreateRenderBuf(Class,Obj);
#endif

    LOD -> FrameBox.x = x1; LOD -> FrameBox.w = x2 - x1 + 1;
    LOD -> FrameBox.y = y1; LOD -> FrameBox.h = y2 - y1 + 1;

    LOD -> Render -> Palette = LOD -> Scheme;
    
    if (LOD -> Root)
    {
        /* FF_Render_Complex is disabled  if  FF_WINDOW_COMPLEX_REFRESH  is
        not set, making complete window refresh faster but flashing */
   
        if (FF_WINDOW_SYS_COMPLEX & LOD -> SYSFlags)
        {
            LOD -> Render -> Flags |= FF_Render_Refreshing | FF_Render_Complex;
            F_SuperDo(Class,Obj,FM_Frame_Draw,LOD -> Render,&LOD -> FrameBox,0);
        }
        else
        {
            LOD -> Render -> Flags |= FF_Render_Refreshing;
            F_SuperDo(Class,Obj,FM_Frame_Draw,LOD -> Render,&LOD -> FrameBox,FF_Frame_Draw_Erase);
        }
                        
        if ((FF_WINDOW_SYS_REFRESH_SIMPLE & LOD->SYSFlags) ||
            ((FF_WINDOW_SYS_COMPLEX & LOD->SYSFlags) == 0))
        {
            F_Draw(LOD -> Root,FF_Draw_Object);
        }
        else
        {
            F_Draw(LOD -> Root,FF_Draw_Damaged | FF_Draw_Object);
        }

        if (FF_WINDOW_SYS_COMPLEX & LOD -> SYSFlags)
        {
            LOD -> Render -> Flags &= ~FF_Render_Refreshing;
        }
        else
        {
            LOD -> Render -> Flags &= ~(FF_Render_Refreshing | FF_Render_Complex);
        }
    }
    else
    {
        F_SuperDo(Class,Obj,FM_Frame_Draw,LOD -> Render,&LOD -> FrameBox,FF_Frame_Draw_Erase);
    }

#ifdef WIN_BUFFEREDDRAW
    DeleteRenderBuf(Class,Obj,buf_handle);
#endif
}
//+
///Window_Erase
F_METHODM(void,Window_Erase,FS_Erase)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Render && LOD -> Render -> RPort)
   {
      F_Set(LOD -> Render, FA_Render_Palette, (uint32) LOD -> Scheme);
      
      if (FF_Erase_Box & Msg -> Flags)
      {
         FRect r;

         r.x1 = ((FBox *)(Msg -> Rect)) -> x;
         r.y1 = ((FBox *)(Msg -> Rect)) -> y;
         r.x2 = ((FBox *)(Msg -> Rect)) -> x + ((FBox *)(Msg -> Rect)) -> w - 1;
         r.y2 = ((FBox *)(Msg -> Rect)) -> y + ((FBox *)(Msg -> Rect)) -> h - 1;

         F_Do(LOD -> FramePublic -> Back,FM_ImageDisplay_Draw,LOD -> Render,&r,0);
      }
      else
      {
         F_Do(LOD -> FramePublic -> Back,FM_ImageDisplay_Draw,LOD -> Render,Msg -> Rect,(FF_Erase_Region & Msg -> Flags) ? FF_ImageDisplay_Region : 0);
      }
   }
}
//+
