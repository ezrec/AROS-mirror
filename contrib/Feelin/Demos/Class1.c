/*
    Class3 Demo

    2000-2005 © Olivier LAVIALE <gofromiel@gofromiel.com>

*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/feelin.h>

#include <intuition/intuition.h>
#include <libraries/feelin.h>

struct  FeelinBase                 *FeelinBase;
#define GfxBase                     FeelinBase -> Graphics
 
enum    {
    
        ID_DUMMY,
        ID_WINDOW

        };

// This is the instance data for our custom class.
 
struct LocalObjectData
{
    FAreaPublic                     *AreaPublic;
    int16                            x,y, sx,sy;
};

///mNew
F_METHOD(uint32,mNew)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    return F_SUPERDO();
}
//+
/// mSetup
/*
    FC_Area creates a struct FeelinEventHandler on  the  fly  depending  the
    events  modified  by the FM_ModifyHandler method. Using FM_ModifyHandler
    it's a piece of cake ot request events.
*/
F_METHOD(uint32,mSetup)
{
    F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY | FF_EVENT_BUTTON,0);

    return F_SUPERDO();
}
//+
/// mCleanup
/*
    FC_Area creates a struct FeelinEventHandler on  the  fly  depending  the
    events  modified  by the FM_ModifyHandler method. Using FM_ModifyHandler
    it's a piece of cake ot request events.
*/
F_METHOD(uint32,mCleanup)
{
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY | FF_EVENT_BUTTON);

    return F_SUPERDO();
}
//+
/// mAskMinMax
/*
    FM_AskMinMax will be called before the window is opened. We need to tell
    the FC_Window object the minimum and maximum size of our object.

    When you are the first receiving  the  method  the  fields  _minw()  and
    _minh() are set to zero and the fields _maxw() and _maxh() to FV_MAXMAX.
    We can add values to _minw() and _minh() or set _maxw() and  _maxh()  if
    we need to. Then we pass the method to our superclass.

    When the method reaches FC_Area these values will be adjusted  according
    to FA_MinXxx, FA_FixedXxx and FA_FixXxx attributes.
*/

F_METHOD(uint32,mAskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _minw += 100; _minh += 040;
    _maxw  = 500; _maxh  = 300;

    return F_SUPERDO();
}
//+
/// mDraw
/*
    Draw method is called whenever Feelin feels (obviously  ;-))  we  should
    render  our object. This usually happens after layout is finished. Note:
    You may only render within the rectangle _mx(), _my(), _mx2(), _my2().
*/
F_METHODM(void,mDraw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;

/*
    Let our superclass draw itself first, FC_Area would e.g. draw the  frame
    and clear the whole region. What it does exactly depends on flags.
*/

    F_SUPERDO();

/*
    IF FF_Draw_Object isn't set, we shouldn't  draw  anything.  Feelin  just
    wanted to update the frame or something like that.
*/

    if (Msg -> Flags & FF_Draw_Update) // called from our input method
    {
        if (LOD -> sx || LOD -> sy)
        {
            _BPen(_pens[FV_Pen_Shine]);
            ScrollRaster(rp,LOD -> sx,LOD -> sy,_ix,_iy,_ix2,_iy2);
            _BPen(_pens[FV_Pen_Dark]);
            LOD -> sx = 0;
            LOD -> sy = 0;
        }
        else
        {
            _APen(_pens[FV_Pen_Shadow]);
            _Plot(LOD -> x,LOD -> y);
        }
    }
    else if (Msg -> Flags & FF_Draw_Object)
    {
        _APen(_pens[FV_Pen_Shine]);
        _Boxf(_ix,_iy,_ix2,_iy2);
    }
}
//+
///mHandleEvent
/*
in mSetup() we said that we want get a  message  if  mousebuttons  or  keys
pressed so we have to define the input-handler

Note :

    This is really a good example, because it  shows  how  to  use  critical
    events carefully:

    FF_EVENT_MOTION is only needed when left-mousebutton is pressed,  so  we
    dont  request  this  until  we  get  a select down message and we reject
    FF_EVENT_MOTION immeditly after we get a select up message.

*/

F_METHODM(uint32,mHandleEvent,FS_HandleEvent)
{
    #define _between(a,x,b) ((x) >= (a) && (x) <= (b))
    #define _isinobject(x,y) (_between(_ix,(x),_ix2) && _between(_iy,(y),_iy2))

/*
Note on Arrows handling :

If you don't handle arrows return NULL, this will allow  Window  object  to
cycle   through  its  chain  using  arrows  instead  of  tabulations  (more
confortable hu ?), else return FF_HandleEvent_Eat as usual.
*/

    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;

    if (fev -> Key)
    {
        switch (fev -> Key)
        {
            case FV_KEY_LEFT:  LOD -> sx = -1; break;
            case FV_KEY_RIGHT: LOD -> sx =  1; break;
            case FV_KEY_UP:    LOD -> sy = -1; break;
            case FV_KEY_DOWN:  LOD -> sy =  1; break;
            default:           return NULL;
        }

        F_Draw(Obj,FF_Draw_Update);

        return FF_HandleEvent_Eat; // Forbid arrow cycling, because *WE* handle key events.
    }
    else if (fev -> Class == FF_EVENT_BUTTON)
    {
        if (fev -> Code == FV_EVENT_BUTTON_SELECT && FF_EVENT_BUTTON_DOWN & fev -> Flags)
        {
            if (_isinobject(fev -> MouseX,fev -> MouseY))
            {
                LOD -> x = fev -> MouseX;
                LOD -> y = fev -> MouseY;
                F_Draw(Obj,FF_Draw_Update);

                // Only request FF_EVENT_MOTION if we realy need it
                
                F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);

                return FF_HandleEvent_Eat;
            }
        }
        else
        {
            // Reject FF_EVENT_MOTION because lmb is no longer pressed

            F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);
        }
    }
    else if (fev -> Class == FF_EVENT_MOTION)
    {
        if (_isinobject(fev -> MouseX,fev -> MouseY))
        {
            LOD -> x = fev -> MouseX;
            LOD -> y = fev -> MouseY;

            F_Draw(Obj,FF_Draw_Update);

            return FF_HandleEvent_Eat;
        }
    }
    return NULL;
}
//+

/// Main
int32 main(void)
{
    STATIC F_METHODS_ARRAY =
    {
        F_METHODS_ADD_STATIC(mNew,         FM_New),
        F_METHODS_ADD_STATIC(mSetup,       FM_Setup),
        F_METHODS_ADD_STATIC(mCleanup,     FM_Cleanup),
        F_METHODS_ADD_STATIC(mAskMinMax,   FM_AskMinMax),
        F_METHODS_ADD_STATIC(mDraw,        FM_Draw),
        F_METHODS_ADD_STATIC(mHandleEvent, FM_HandleEvent),

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
        /* Create the new custom class with a call to F_CreateClassA().

        This  function  returns  a  feelinClass  structure.  You  must  use
        Class->Name  to  create instance of your custom class. This Name is
        unique and made by F_CreateClassA() when FA_Class_Name is NULL. */

        FClass *cc = F_CreateClassA(NULL,F_TAGS);

        if (cc)
        {
            FObject app,win;

            app = AppObject,
                FA_Application_Title, "demo_Class1",
                FA_Application_Version, "$VER: Class1 02.00 (2005/07/22)",
                FA_Application_Copyright, "© 2000-2005 Olivier LAVIALE",
                FA_Application_Author, "Olivier LAVIALE - gofromiel@gofromiel.com",
                FA_Application_Description, "My first subclass",
                FA_Application_Base, "DEMOCLASS1",

                Child, win = WindowObject,
                    FA_ID, ID_WINDOW,
                    FA_Window_Title, "My first subclass",
                    FA_Window_Open, TRUE,

                    Child, VGroup,
                        Child, TextObject, FA_SetMax,FV_SetHeight, FA_ChainToCycle,FALSE, FA_Frame,"$text-frame", FA_Back,"$text-back", FA_Text, "<align=center><i>Paint</i> with <b>mouse</b>,<br><i>Scroll</i> with <b>cursor keys</b>.", End,
                        Child, F_NewObj(cc -> Name, FA_Frame,"<frame id='24' />", TAG_DONE),
                    End,
                End,
            End;

            if (app)
            {
                F_Do(win,FM_Notify, FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);
                F_Do(app,FM_Application_Run);
                F_DisposeObj(app);
            }

            F_DeleteClass(cc);
        }
        else
        {
            Printf("Could not create custom class.\n");
        }
        
        F_FEELIN_CLOSE;
    }
    else
    {
        Printf("Failed to open feelin.library\n");
    }
    return 0;
}
//+
