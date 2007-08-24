/*
   ColorCube Demo

   2001-2004 © Olivier LAVIALE <gofromiel@gofromiel.com>

   This example illustrate color management with a FC_Display object.
*/

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
//#include <proto/utility.h>
#include <proto/feelin.h>

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FPalette                       *Palette;
    uint8                           Segments;
};

#define  FA_Cube_Segments                       (FCCA_BASE + 0)

struct FeelinBase                  *FeelinBase;
#define GfxBase                     FeelinBase->Graphics
//#define UtilityBase                 FeelinBase->Utility

/*
#ifdef __VBCC__
struct GfxBase                     *GfxBase;
struct Library                     *UtilityBase;
#else
#define GfxBase                     FeelinBase -> Graphics
#define UtilityBase                 FeelinBase -> Utility
#endif
*/

/*** Private ***************************************************************/

///Cube_Create
FPalette * Cube_Create(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
       
    if (_render)
    {
        uint32 *orgb,x,y,r,g,b,a,step,seg=LOD -> Segments;

        if (LOD -> Palette)
        {
            F_Do(_display,FM_RemPalette,LOD -> Palette);
            LOD -> Palette = NULL;
        }

        if (orgb = F_New(seg * seg * sizeof (uint32)))
        {
            step = 256 / (seg - 1);

            for (y = 0 ; y < seg ; y++)
            {
                for (x = 0 ; x < seg ; x ++)
                {
                    if ((x == (seg - 1)) && (y == 0))
                    {
                        r = 0xFF;
                    }
                    else
                    {
                        a = step * (seg - 1 - y);
                        r = x * (a / (seg - 1));
                    }

                    if ((y ==(seg - 1)) && (x ==0))
                    {
                        b = 0xFF;
                    }
                    else
                    {
                        a = step * (seg - 1 - x);
                        b = y * (a / (seg - 1));
                    }

                    if ((y == (seg - 1)) && (x == (seg - 1)))
                    {
                        g = 0xFF;
                    }
                    else
                    {
                        g = y * ((step * x) / (seg - 1));
                    }

                    orgb[x+(y*seg)] = (r << 16) | (g << 8) | b;
                }
            }

            LOD -> Palette = (FPalette *) F_Do(_display,FM_AddPalette,seg * seg,orgb);

            F_Dispose(orgb);

            return LOD -> Palette;
        }
    }
    return NULL;
}
//+

/*** Methods ***************************************************************/

///Cube_New
F_METHOD(uint32,Cube_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;
 
    LOD -> Segments = 5;

    return F_SuperDo(Class,Obj,Method,
   
        FA_NoFill, TRUE,
      
        TAG_MORE, Msg);
}
//+
///Cube_Set
F_METHOD(void,Cube_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Cube_Segments:
        {
            if (item.ti_Data > 1 && item.ti_Data < 17)
            {
                LOD -> Segments = item.ti_Data;
                
                Cube_Create(Class,Obj);
                
                F_Draw(Obj,FF_Draw_Update);
            }
            else
            {
                F_Log(FV_LOG_DEV,"Value %ld out of range (2-16)",item.ti_Data);
            }
        }
        break;
    }
    F_SUPERDO();
}
//+
///Cube_Show
F_METHOD(uint32,Cube_Show)
{
    if (F_SUPERDO())
    {
        if (Cube_Create(Class,Obj)) return TRUE;
    }
    return FALSE;
}
//+
///Cube_Hide
F_METHOD(uint32,Cube_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        F_Do(_display,FM_RemPalette,LOD -> Palette);

        LOD -> Palette = NULL;
    }

    return F_SUPERDO();
}
//+
///Cube_Draw
F_METHODM(void,Cube_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint32 x,y,a,b,p=0;
    uint32 w,h,bonusw,bonush,dw,dh,seg=LOD -> Segments;
    struct RastPort *rp = _rp;
        
    F_SUPERDO();

    y = _iy;
    w = _iw / seg; bonusw = _iw - (w * seg);
    h = _ih / seg; bonush = _ih - (h * seg);

    dh = h;

    for (a = 0 ; a < seg ; a++)
    {
        dw = w;
        x = _ix;

        if (a == (seg - 1)) dh = h + bonush;

        for (b = 0 ; b < seg ; b++)
        {
            _APen(LOD -> Palette -> Pens[p]);

            if (b == (seg - 1)) dw = w + bonusw;

            _Boxf(x,y,dw + x - 1,dh + y - 1);

            x += w;
            p++;
        }
        y += h;
    }
}
//+

STATIC F_METHODS_ARRAY =
{
    F_METHODS_ADD_STATIC(Cube_New,   FM_New),
    F_METHODS_ADD_STATIC(Cube_Set,   FM_Set),
    F_METHODS_ADD_STATIC(Cube_Show,  FM_Show),
    F_METHODS_ADD_STATIC(Cube_Hide,  FM_Hide),
    F_METHODS_ADD_STATIC(Cube_Draw,  FM_Draw),

    F_ARRAY_END
};

/*** main ******************************************************************/

int main()
{
    if (F_FEELIN_OPEN)
    {
        FClass  *cl;
        FObject  app,win,cub,sld;

/*
      #ifdef __VBCC__
      GfxBase = FeelinBase -> Graphics;
      UtilityBase = FeelinBase -> Utility;
      #endif
*/
        cl = F_CreateClass(NULL,
            
            FA_Class_LODSize,  sizeof (struct LocalObjectData),
            FA_Class_Super,    FC_Area,
            FA_Class_Methods,  F_METHODS,
            
            TAG_DONE);
            
        if (cl)
        {
            app = AppObject,
                FA_Application_Title,        "demo_Cube",
                FA_Application_Version,      "$VER: demo_Cube 2.0 (2003/08/04)",
                FA_Application_Copyright,    "© 2000-2004 Olivier LAVIALE",
                FA_Application_Author,       "Olivier LAVIALE - gofromiel@gofromiel.com",
                FA_Application_Description,  "Tutorial on color managing",
                FA_Application_Base,         "demo_Cude",

                Child, win = WindowObject,
                    FA_ID,               MAKE_ID('M','A','I','N'),
                    FA_Window_Title,     "Feelin : Cube",
                    FA_Window_Open,      TRUE,

                    Child, VGroup,
                        Child, cub = F_NewObj(cl -> Name,
                            
                            FA_Frame,       "$gauge-frame",
                            FA_MinWidth,    50,
                            FA_MinHeight,   50,
                            
                            End,
                        
                        Child, HGroup,
                            Child, TextObject, FA_Text, "Segments :", FA_SetMax,TRUE,DontChain, End,
                            Child, sld = F_MakeObj(FV_MakeObj_Slider, TRUE,2,16,5,
                                FA_ID,            MAKE_ID('C','U','B','W'),
                                FA_SetMax,        FV_SetHeight,
                                FA_ContextHelp,   "Adjust the number of color segment per corner.",
                              
                                End,
                        End,
                    End,
                End,
            End;

            if (app)
            {
                F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
                F_Do(sld,FM_Notify,"FA_Numeric_Value",FV_Notify_Always,cub,FM_Set,2,FA_Cube_Segments,FV_Notify_Value);
               
                F_Do(app,FM_Application_Run);
                F_DisposeObj(app);
            }
            F_DeleteClass(cl);
        }
        F_FEELIN_CLOSE;
    }
    else
    {
       Printf("Unable to open feelin.library v%ld\n",FV_FEELIN_VERSION);
    }
    return 0;
}

