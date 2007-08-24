#include "Private.h"

//#define DB_RESIZABLE
//#define DB_MINMAX
//#define DB_RETHINK_LAYOUT

/*** Private ***************************************************************/

struct FS_HOOK_RETHINKREQUEST                   { FClass *Class; int32 Forbid; };
 
///code_rethinkrequest
F_HOOKM(void,code_rethinkrequest,FS_HOOK_RETHINKREQUEST)
{
    struct FeelinClass *Class = Msg->Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    BOOL resize=FALSE;

//    F_Log(0,"process rethink requests (%ld) forbid (%ld)",LOD->RethinkNest,F_Get(LOD->Render,FA_Render_Forbid));
 
    while (LOD->RethinkNest)
    {
        uint16 prev_w = LOD->Box.w;
        uint16 prev_h = LOD->Box.h;
   
        LOD->SYSFlags &= ~FF_WINDOW_SYS_KNOWNMINMAX;

        window_resizable(Class,Obj);
        window_minmax(Class,Obj);
        
     //   F_Log(0,"Box %03ldx%03ld %03ldx%03ld MIN %03ldx%03ld MAX %03ldx%03ld",prev_w,prev_h,LOD->Box.w,LOD->Box.h,LOD->MinMax.MinW,LOD->MinMax.MinH,LOD->MinMax.MaxW,LOD->MinMax.MaxH);

        if (LOD->MinMax.MinW == LOD->MinMax.MaxW)
        {
            if (LOD->Box.w != prev_w) resize = TRUE;
        }

        if (LOD->MinMax.MinH == LOD->MinMax.MaxH)
        {
            if (LOD->Box.h != prev_h) resize = TRUE;
        }

        if (prev_w < LOD->Box.w || prev_h < LOD->Box.h)
        {
            resize = TRUE;
        }

        LOD->RethinkNest--;
    }

/**/

    if (LOD->Win)
    {
        /* WindowLimits() doen't work correctly. MinMax values need  to
        be set directly before calling the function. */

        LOD->Win->MinWidth  = LOD->MinMax.MinW;
        LOD->Win->MinHeight = LOD->MinMax.MinH;
        LOD->Win->MaxWidth  = LOD->MinMax.MaxW;
        LOD->Win->MaxHeight = LOD->MinMax.MaxH;

        if (resize)
        {
            F_Set(LOD->Render, FA_Render_Forbid, FALSE);

            ChangeWindowBox(LOD->Win,
               (LOD->Box.w > LOD->Win->Width) ? (LOD->Box.x - (LOD->Box.w - LOD->Win->Width) / 2) : (LOD->Box.x + (LOD->Win->Width - LOD->Box.w) / 2),
               (LOD->Box.h > LOD->Win->Height) ? (LOD->Box.y - (LOD->Box.h - LOD->Win->Height) / 2) : (LOD->Box.y + (LOD->Win->Height - LOD->Box.h) / 2),
                LOD->Box.w,
                LOD->Box.h);
        }

        WindowLimits(LOD->Win,
            LOD->MinMax.MinW,LOD->MinMax.MinH,
            LOD->MinMax.MaxW,LOD->MinMax.MaxH);

        if (resize)
        {
            return;
        }
    }

    if (!resize)
    {
        F_Do(Obj, FM_Window_Layout);
    }
 
/**/

    F_Set(LOD->Render,FA_Render_Forbid,FALSE);
    
    if (FF_WINDOW_SYS_REFRESH_NEED & LOD->SYSFlags)
    {
//        F_Log(0,"redraw window");
        
        F_Do(Obj, FM_Window_Draw);
    }
    else if (!resize)
    {
//        F_Log(0,"redraw root");
        
        if (FF_WINDOW_SYS_COMPLEX & LOD->SYSFlags)
        {
            F_Draw(LOD->Root, FF_Draw_Damaged | FF_Draw_Object);
        }
        else
        {
            F_Draw(LOD->Root, FF_Draw_Object);
        }
    }
}
//+

///window_resizable

/* This function is used to know if the window is resizable. If the  window
is  resizable the flag FF_WINDOW_WIN_RESIZABLE is set to 'WINFlags'. If the
minmax   of   the   root    object    has    not    been    computed    yet
(FF_WINDOW_SYS_KNOWNMINMAX  cleared), the FM_AskMinMax method is invoked on
the  root  object  and  the  flags  FF_WINDOW_SYS_KNOWNMINMAX  is  set   to
'SYSFlags' */
 
int32 window_resizable(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_root_area_public)
    {
        if (!(FF_WINDOW_SYS_KNOWNMINMAX & LOD->SYSFlags))
        {
            _root_minw = 0;
            _root_minh = 0;
            _root_maxw = FV_MAXMAX;
            _root_maxh = FV_MAXMAX;

            F_Do(LOD->Root,FM_AskMinMax);

            #ifdef DB_RESIZABLE
            F_Log(0,"MIN %ld x %ld - MAX %ld x %ld",_root_minw,_root_minh,_root_maxw,_root_maxh);
            #endif

            LOD->SYSFlags |= FF_WINDOW_SYS_KNOWNMINMAX;
        }

        /* FF_WINDOW_GUI_RESIZABLE is made public by the  FA_Window_Resizable
        attribute.  This attribute is used by decorators to know if they need
        to create zoom and size gadgets. */

        if (_root_minw == _root_maxw && _root_minh == _root_maxh)
        {
            LOD->WINFlags &= ~FF_WINDOW_WIN_RESIZABLE;
        }
        else
        {
            LOD->WINFlags |= FF_WINDOW_WIN_RESIZABLE;
        }
    }
    else
    {
        LOD->WINFlags |= FF_WINDOW_WIN_RESIZABLE;
    }

    if (FF_WINDOW_WIN_BORDERLESS & LOD->WINFlags)
    {
        LOD->WINFlags &= ~FF_WINDOW_WIN_RESIZABLE;
    }

    return (int32)(0 != (LOD->WINFlags & FF_WINDOW_WIN_RESIZABLE));
}
//+
///window_minmax

#define FF_CENTER_W                             (1 << 0)
#define FF_CENTER_H                             (1 << 1)
 
void window_minmax(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Screen *scr = LOD->Screen;
    uint16 minw,minh,maxw,maxh;
    uint8 center = (LOD->Box.w) ? 0 : (FF_CENTER_W | FF_CENTER_H);

    if (!scr) return;
                
    F_Do(LOD->Decorator, FM_AskMinMax);
 
    if (FF_WINDOW_WIN_BORDERLESS & LOD->WINFlags)
    {
        LOD->Borders.l = 0; LOD->Borders.r = 0;
        LOD->Borders.t = 0; LOD->Borders.t = 0;
    }
    else
    {
        LOD->Borders.l = LOD->FramePublic->Border[0].l + LOD->FramePublic->Padding[0].l;
        LOD->Borders.r = LOD->FramePublic->Border[0].r + LOD->FramePublic->Padding[0].r;
        LOD->Borders.t = LOD->FramePublic->Border[0].t + LOD->FramePublic->Padding[0].t;
        LOD->Borders.b = LOD->FramePublic->Border[0].b + LOD->FramePublic->Padding[0].b;
   
        if (LOD->Decorator)
        {
            LOD->Borders.l += _deco_il;
            LOD->Borders.r += _deco_ir;
            LOD->Borders.t += _deco_it;
            LOD->Borders.b += _deco_ib;
        }
        else
        {
            LOD->Borders.l += scr->WBorLeft;
            LOD->Borders.r += scr->WBorRight;
            LOD->Borders.t += scr->WBorTop + scr->RastPort.Font->tf_YSize + 1;
            LOD->Borders.b += scr->BarHeight - 3; /* FIXME: BarHeight should be good, but its not !? */
        }
    }

    minw = LOD->Borders.l + LOD->Borders.r; maxw = scr->Width;
    minh = LOD->Borders.t + LOD->Borders.b; maxh = scr->Height;

    if (_root_area_public)
    {
        FMinMax *mm = &_root_minmax;

        if (minw + mm->MinW < maxw) maxw = minw + mm->MaxW; minw += mm->MinW;
        if (minh + mm->MaxH < maxh) maxh = minh + mm->MaxH; minh += mm->MinH;
    }

    window_resizable(Class,Obj);

    #ifdef DB_MINMAX
    F_Log(0,"Dim %4ld,%4ld - Min %4ld,%4ld - Max %4ld,%4ld",LOD->Box.w,LOD->Box.h,minw,minh,maxw,maxh);
    #endif

    if (!LOD->Box.w && !LOD->Box.h)
    {
       if (LOD->UserBox.x)
       {
          if (FF_WINDOW_BOX_XPERCENT & LOD->BOXFlags)
          {
             LOD->Box.x = scr->Width * LOD->UserBox.x / 100; center &= ~FF_CENTER_W;
          }
          else
          {
             LOD->Box.x = LOD->UserBox.x;
          }
       }

       if (LOD->UserBox.y)
       {
          if (FF_WINDOW_BOX_YPERCENT & LOD->BOXFlags)
          {
             LOD->Box.y = scr->Height * LOD->UserBox.y / 100; center &= ~FF_CENTER_H;
          }
          else
          {
             LOD->Box.y = LOD->UserBox.y;
          }
       }

       if (LOD->UserBox.w)
       {
          if (FF_WINDOW_BOX_WPERCENT & LOD->BOXFlags)
          {
             LOD->Box.w = scr->Width * LOD->UserBox.w / 100;
          }
          else
          {
             LOD->Box.w = LOD->UserBox.w;
          }
 //         F_Log(0,"w %ld",LOD->Box.w);
       }
     
       if (LOD->UserBox.h)
       {
          if (FF_WINDOW_BOX_HPERCENT & LOD->BOXFlags)
          {
             LOD->Box.h = scr->Height * LOD->UserBox.h / 100;
          }
          else
          {
             LOD->Box.h = LOD->UserBox.h;
          }
 //         F_Log(0,"h %ld",LOD->Box.h);
       }
    }

    #ifdef DB_MINMAX
    F_Log(0,"Dim %4ld,%4ld - Min %4ld,%4ld - Max %4ld,%4ld",LOD->Box.w,LOD->Box.h,minw,minh,maxw,maxh);
    #endif

    if (LOD->Box.w > maxw) LOD->Box.w = maxw;
    if (LOD->Box.h > maxh) LOD->Box.h = maxh;
    if (LOD->Box.w < minw) LOD->Box.w = minw;
    if (LOD->Box.h < minh) LOD->Box.h = minh;

    if (!LOD->Win && center)
    {
        LOD->Box.x = (scr->Width - LOD->Box.w) / 2;
        LOD->Box.y = (scr->Height - LOD->Box.h) / 2;
    }

    LOD->MinMax.MaxW = MIN(maxw,scr->Width);
    LOD->MinMax.MaxH = MIN(maxh,scr->Height);
    LOD->MinMax.MinW = minw;
    LOD->MinMax.MinH = minh;

    #ifdef DB_MINMAX
    F_Log(0,">>> DIM %4ld,%4ld - MAX %4ld x %4ld",LOD->Box.w,LOD->Box.h,LOD->MinMax.MaxW,LOD->MinMax.MaxH);
    #endif
}
//+

/*** Methods ***************************************************************/

///Window_Layout
F_METHOD(void,Window_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    struct Window *win = LOD->Win;

    if (win)
    {
        LOD->Box.x = win->LeftEdge;
        LOD->Box.y = win->TopEdge;
        LOD->Box.w = win->Width;
        LOD->Box.h = win->Height;
    }

    F_Set(LOD->Render, FA_Render_Forbid, TRUE);
  
    if (LOD->Decorator)
    {
        F_Layout(LOD->Decorator,
            0,
            0,
            LOD->Box.w,
            LOD->Box.h,
            0);
    }

    F_Layout(LOD->Root,
        LOD->Borders.l,
        LOD->Borders.t,
        LOD->Box.w - LOD->Borders.l - LOD->Borders.r,
        LOD->Box.h - LOD->Borders.t - LOD->Borders.b,
        0);

    F_Set(LOD->Render, FA_Render_Forbid, FALSE);
}
//+
///Window_RequestRethink
F_METHODM(void,Window_RequestRethink,FS_Window_RequestRethink)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//    F_Log(0,"source %s{%08lx} - nest (%ld)",_classname(Msg->Source), Msg->Source, LOD->RethinkNest);
 
    if (LOD->Application && LOD->Render)
    {
        int32 forbid=FALSE;
 
        LOD->RethinkNest++;

//        F_Log(0,"RETHINKNEST (%ld)",LOD->RethinkNest);
 
        if (LOD->RethinkNest == 1)
        {
//            F_Log(0,"FORBID RENDERINGS, WIN 0x%08lx",LOD->Win);
            
            F_Set(LOD->Render, FA_Render_Forbid, TRUE);
             
            F_Do(LOD->Application, FM_Application_PushMethod, Obj, FM_CallHookEntry,3, code_rethinkrequest,Class,forbid);
        }
    }
}
//+
