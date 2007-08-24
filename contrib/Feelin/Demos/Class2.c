/*
   2000-2004 © Olivier LAVIALE <gofromiel@gofromiel.com>

   This example illustrate how to writte a custom class using an  array  of
   methods instead of a dispatcher.
*/

#include <stdlib.h>

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;

#ifdef __VBCC__
#undef GfxBase
struct GfxBase *GfxBase;
#endif

/* Here is the beginning of our simple new class... */

struct LocalObjectData
{
   FAreaPublic                     *AreaPublic;
   struct FeelinSignalHandler       SignalHandler;
};

enum  {

      FM_Class2_Strobo

      };

/// mNew
F_METHOD(uint32,mNew)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;

   LOD -> SignalHandler.Object      = Obj;
   LOD -> SignalHandler.Method      = F_IDM(FM_Class2_Strobo);
   LOD -> SignalHandler.Flags       = FF_SignalHandler_Timer;
   LOD -> SignalHandler.fsh_Secs    = 0;
   LOD -> SignalHandler.fsh_Micros  = 30000;

   return F_SUPERDO();
}
//+
/// mShow
F_METHOD(LONG,mShow)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      F_Do(_app,FM_Application_AddSignalHandler,&LOD -> SignalHandler);

      return TRUE;
   }
   return FALSE;
}
//+
/// mHide
F_METHOD(void,mHide)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (FF_Area_CanShow & _flags)
   {
      F_Do(_app,FM_Application_RemSignalHandler,&LOD -> SignalHandler);
   }
   F_SUPERDO();
}
//+
/// mAskMinMax
/*
AskMinMax method will be called before the  window  is  opened  and  before
layout  takes place. We need to tell Feelin the minimum and maximum size of
our object. Note that we indeed need to *add* these values,  not  just  set
them !
*/
F_METHOD(uint32,mAskMinMax)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   _minw += 30;
   _minh += 30;

/*
Now call our superclass. FC_Area will handle everything,  taking  care  of
FA_FixedXxx, FA_MinXxx and FA_MaxXxx.
*/

   return F_SUPERDO();
}
//+
/// mDraw
/*
Draw method is called whenever  Feelin  feels  (obviously  ;-))  we  should
render our object. This usually happens after layout is finished. Note: You
may  only  render  within  the  rectangle  _mx(Obj),  _my(Obj),  _mx2(Obj),
_my2(Obj).
*/
F_METHOD(void,mDraw)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct RastPort *rp = _rp;

/*
let our superclass draw itself first, Area class would e.g. draw the  frame
and clear the whole region. What it does exactly depends on flags.
*/

   F_SUPERDO();

   _APen(rand() % (1 << rp -> BitMap -> Depth));
   _Boxf(_ix,_iy,_ix2,_iy2);
}
//+
///mStrobo
F_METHOD(LONG,mStrobo)
{
   F_Draw(Obj,FF_Draw_Update);
   return TRUE; // If we return FALSE the timer event won't be requested again
}
//+

///Main
int32 main(void)
{
    FClass *cc;
    FObject app,win;

 /*
    Methods handled by the class.
 */

    STATIC F_METHODS_ARRAY =
    {
        F_METHODS_ADD(mStrobo, "Stobo"),

        F_METHODS_ADD_STATIC(mNew,       FM_New),
        F_METHODS_ADD_STATIC(mShow,      FM_Show),
        F_METHODS_ADD_STATIC(mHide,      FM_Hide),
        F_METHODS_ADD_STATIC(mAskMinMax, FM_AskMinMax),
        F_METHODS_ADD_STATIC(mDraw,      FM_Draw),

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
        #ifdef __VBCC__
        GfxBase = FeelinBase -> Graphics;
        #endif

        /* Create the new custom class with a call to F_CreateClassA().

        This function returns a struct FeelinClass. You must use cc  ->  Name
        to create instance of your custom class. This Name is unique and made
        by F_CreateClassA(). */

        if ((cc = F_CreateClassA(NULL, F_TAGS)) != NULL)
        {
            app = AppObject,
               FA_Application_Title,        "demo_Class2",
               FA_Application_Version,      "$VER: Class2 1.2 (2004/03/13)",
               FA_Application_Copyright,    "© 2000-2003 Olivier LAVIALE",
               FA_Application_Author,       "Olivier LAVIALE <HaploLaMain@aol.com>",
               FA_Application_Description,  "Tutorial on Client.AddSignalHandler()",
               FA_Application_Base,         "CLASS2",

               Child, win = WindowObject,
                  FA_ID,           MAKE_ID('M','A','I','N'),
                  FA_Window_Title, "Crazy colors using Application.AddSignalHandler()",
                  FA_Window_Open,   TRUE,
                  
                  Child, VGroup, FA_Group_Rows, 2,
                     Child, F_NewObj(cc -> Name,GaugeFrame, DontChain, TAG_DONE),
                     Child, F_NewObj(cc -> Name,GaugeFrame, DontChain, TAG_DONE),
                     Child, F_NewObj(cc -> Name,GaugeFrame, DontChain, TAG_DONE),
                     Child, F_NewObj(cc -> Name,GaugeFrame, DontChain, TAG_DONE),
                  End,
               End,
            End;

            if (app)
            {
               F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);
               F_Do(app,FM_Application_Run);
               F_DisposeObj(app);
            }

            F_DeleteClass(cc);
        }
        else
        {
            Printf("Unable to create custom class.\n");
        }
        
        F_FEELIN_CLOSE;
    }
    else
    {
        Printf("Failed to open feelin.library.\n");
    }
    return 0;
}
//+
