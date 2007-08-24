/*

$VER: 02.00 (2005/07/22)
                         
   Your first class.
   
   Adapted by Olivier LAVIALE (gofromiel@gofromiel.com) from the MUI C
   example 'Class1.c').

*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

struct FeelinBase                  *FeelinBase;
#define GfxBase                     FeelinBase -> Graphics

enum    { // object ids

        ID_DUMMY,
        ID_WINDOW

        };
        
#define FM_Trigger                              (FCCM_BASE + 0)
 
/* In order to maximize evolutivity and independency, all Feelin classes
are opaque. For convenience, some root classes, like FC_Area, allow you to
access a little public part of their local object data.

Using FC_Area public data is done through macros to save a lot of typing,
but need some workaround. Because these macros access area data
transparently, they need the field 'AreaData' to be defined in the 'LOD'
variable : */

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FSignalHandler                  sh;
    uint16                          index;
    int16                           add;
};

///mNew

/* You should use the F_AREA_SAVE_PUBLIC macro to save the FC_Area public
part pointer in the 'AreaData' member. You should use this macro during
your object creation time e.g. FM_New. */
 
F_METHOD(uint32,mNew)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    LOD->sh.Object = Obj;
    LOD->sh.Method = FM_Trigger;
    LOD->sh.Flags = FF_SignalHandler_Timer;
    LOD->sh.fsh_Secs = 0;
    LOD->sh.fsh_Micros = 100000;

    LOD->index = FV_Pen_Shine;
    LOD->add = 1;
 
    return F_SUPERDO();
}
//+
///mSetup
F_METHOD(uint32,mSetup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
     F_Do(_app,FM_Application_AddSignalHandler,&LOD -> sh);

     return TRUE;
   }
   return FALSE;
}
//+
///mCleanup
F_METHOD(uint32,mCleanup)
{
  struct LocalObjectData *LOD = F_LOD(Class,Obj);

  F_Do(_app,FM_Application_RemSignalHandler,&LOD -> sh);

  return F_SUPERDO();
}
//+
///mAskMinMax

/* Accessing FC_Area public datas is then really easy. Have a look at
FC_Area documentation to learn about available macros. */

F_METHOD(uint32,mAskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    /* FM_AskMinMax will be called before the window is opened and before
    layout takes place. We need to tell Feelin the minimum and maximum size
    of our object. */

    _minw += 100 ; _maxw = 500;
    _minh +=  40 ; _maxh = 300;

    return F_SUPERDO();
}
//+
///mDraw
F_METHODM(void,mDraw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    struct RastPort *rp = _rp;

    int16 x1 = _ix;
    int16 x2 = _ix2;
    int16 y1 = _iy;
    int16 y2 = _iy2;

    uint32 i;

    /* let our superclass draw itself first. FC_Area would e.g. draw the
    frame and clear the whole region. What it does exactly depends on
    flags. */

    F_SUPERDO();

    /* Ok, everything ready to render... */

    for (i = x1 ; i <= x2 ; i += 5)
    {
       _FPen(LOD->index);
       _Move(x1,y2); _Draw(i,y1);
       _Move(x2,y2); _Draw(i,y1);

        LOD->index += LOD->add;

        if (LOD->index == FV_Pen_Shine)
        {
            LOD->add = 1;
        }
        else if (LOD->index == FV_Pen_Dark)
        {
            LOD->add = -1;
        }
    }
}
//+
///mTrigger
F_METHOD(uint32,mTrigger)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
                                        
    LOD->index += LOD->add;
    
    if (LOD->index == FV_Pen_Shine)
    {
        LOD->add = 1;
    }
    else if (LOD->index == FV_Pen_Dark)
    {
        LOD->add = -1;
    }

    F_Draw(Obj,FF_Draw_Update);

    return TRUE;
}
//+

/// Main
uint32 main(void)
{
    STATIC F_METHODS_ARRAY =
    {
        F_METHODS_ADD_STATIC(mTrigger, FM_Trigger),

        F_METHODS_ADD_STATIC(mDraw, FM_Draw),
        F_METHODS_ADD_STATIC(mAskMinMax, FM_AskMinMax),
        F_METHODS_ADD_STATIC(mCleanup, FM_Cleanup),
        F_METHODS_ADD_STATIC(mSetup, FM_Setup),

        F_METHODS_ADD_STATIC(mNew, FM_New),
        
        F_ARRAY_END
    };
    
    STATIC F_TAGS_ARRAY =
    {
        F_TAGS_ADD_SUPER(Area),
        F_TAGS_ADD_LOD,
        F_TAGS_ADD_METHODS,
        
        F_ARRAY_END
    };

    if (F_FEELIN_OPEN)
    {
        FClass *cc = F_CreateClassA(NULL,F_TAGS);
        
        if (cc)
        {
            FObject app,win;
           
            app = AppObject,
                FA_Application_Title,       "demo_First_02",
                FA_Application_Version,     "$VER: First02 01.00 (2005/07/22)",
                FA_Application_Copyright,   "©2001-2005 Olivier LAVIALE",
                FA_Application_Author,      "Olivier LAVIALE (gofromiel@gofromiel.com)",
                FA_Application_Description, "My first class",
                FA_Application_Base,        "FIRST02",

                Child, win = WindowObject,
                    FA_ID, ID_WINDOW,
                    FA_Window_Title, "My first class",
                    FA_Window_Open, TRUE,

                    Child, F_NewObj(cc -> Name,
                        FA_Frame, "<frame id='40' padding='4' />",
                        FA_Back, "halfshadow",
                    End,
                End,
            End;

            if (app)
            {
                F_Do(win,FM_Notify, FA_Window_CloseRequest,TRUE,
                    app,FM_Application_Shutdown,0);
               
                F_Do(app,FM_Application_Run);
               
                F_DisposeObj(app);
            }
            
            F_DeleteClass(cc);
        }
        else
        {
            Printf("Unable to create custom class\n");
        }

        F_FEELIN_CLOSE;
    }
    else
    {
        Printf("Unable to open feelin.library\n");
    }

    return 0;
}
//+
