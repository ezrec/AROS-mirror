#include "demo.h"


/* Instance Data */

struct MyData
{
  struct MUI_InputHandlerNode ihnode;

  LONG index;
};


/* Attributes and methods for the custom class */

#define MUISERIALNR_STUNTZI 1
#define TAGBASE_STUNTZI (TAG_USER | ( MUISERIALNR_STUNTZI << 16))
#define MUIM_Class5_Trigger (TAGBASE_STUNTZI | 0x0001)



/* Some strings to display */

static const char *LifeOfBrian[] =
{
  "Cheer up, Brian. You know what they say.",
  "Some things in life are bad,",
  "They can really make you mad.",
  "Other things just make you swear and curse.",
  "When you're chewing on life's grissle,",
  "Don't grumble, give a whistle.",
  "And this'll help things turn out for the best,",
  "And...",
  "Always look on the bright side of life",
  "Always look on the light side of life",
  "If life seems jolly rotten,",
  "There's something you've forgotten,",
  "And that's to laugh, and smile, and dance, and sing.",
  "When you're feeling in the dumps,",
  "Don't be silly chumps,",
  "Just purse your lips and whistle, that's the thing.",
  "And...",
  "Always look on the bright side of life, come on!",
  "Always look on the right side of life",
  "For life is quite absurd,",
  "And death's the final word.",
  "You must always face the curtain with a bow.",
  "Forget about your sin,",
  "Give the audience a grin.",
  "Enjoy it, it's your last chance anyhow,",
  "So...",
  "Always look on the bright side of death",
  "Just before you draw your terminal breath.",
  "Life's a piece of shit,",
  "When you look at it.",
  "Life's a laugh, and death's a joke, it's true.",
  "You'll see it's all a show,",
  "Keep 'em laughing as you go,",
  "Just remember that the last laugh is on you.",
  "And...",
  "Always look on the bright side of life !",
  "...",
  "*** THE END ***",
  "",
  NULL,
};



/***************************************************************************/
/* Here is the beginning of our new class...                               */
/***************************************************************************/


ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
  struct MyData *data;

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,msg)))
    return(0);

  data = INST_DATA(cl,obj);


  data->ihnode.ihn_Object  = obj;
  data->ihnode.ihn_Millis  = 1000;
  data->ihnode.ihn_Method  = MUIM_Class5_Trigger;
  data->ihnode.ihn_Flags   = MUIIHNF_TIMER;

  data->index = 0;

  return((ULONG)obj);
}


ULONG mDispose(struct IClass *cl,Object *obj,Msg msg)
{


  return(DoSuperMethodA(cl,obj,msg));
}


ULONG mSetup(struct IClass *cl,Object *obj,Msg msg)
{
  struct MyData *data = INST_DATA(cl,obj);

  if (!DoSuperMethodA(cl,obj,msg))
    return(FALSE);

  DoMethod(_app(obj),MUIM_Application_AddInputHandler,&data->ihnode);

  return(TRUE);
}


ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
  struct MyData *data = INST_DATA(cl,obj);

  DoMethod(_app(obj),MUIM_Application_RemInputHandler,&data->ihnode);

  return(DoSuperMethodA(cl,obj,msg));
}


ULONG mTrigger(struct IClass *cl,Object *obj,Msg msg)
{
  struct MyData *data = INST_DATA(cl,obj);

    set(obj,MUIA_Text_Contents,LifeOfBrian[data->index]);

    if (!LifeOfBrian[++data->index])
      data->index = 0;
kprintf("it works !\n");
  return(FALSE);
}


/*
** Here comes the dispatcher for our custom class.
*/

/*SAVEDS ASM ULONG            MyDispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
ULONG MyDispatcher(LONG pad)
{
  register struct IClass *a0 __asm("a0");
  struct IClass *cl = a0;
  register Object *a2 __asm("a2");
  Object *obj = a2;
  register Msg a1 __asm("a1");
  Msg msg = a1;

  switch (msg->MethodID)
  {
    case OM_NEW             : return(mNew    (cl,obj,(APTR)msg));
    case OM_DISPOSE         : return(mDispose(cl,obj,(APTR)msg));
    case MUIM_Setup         : return(mSetup  (cl,obj,(APTR)msg));
    case MUIM_Cleanup       : return(mCleanup(cl,obj,(APTR)msg));
    case MUIM_Class5_Trigger: return(mTrigger(cl,obj,(APTR)msg));
  }

  return(DoSuperMethodA(cl,obj,msg));
}


/***************************************************************************/
/* Thats all there is about it. Now lets see how things are used...        */
/***************************************************************************/

int main(int argc,char *argv[])
{
  Object *app,*window,*MyObj;
  struct MUI_CustomClass *mcc;

  init();

  /* Create the new custom class with a call to MUI_CreateCustomClass(). */
  /* Caution: This function returns not a struct IClass, but a           */
  /* struct MUI_CustomClass which contains a struct IClass to be         */
  /* used with NewObject() calls.                                        */
  /* Note well: MUI creates the dispatcher hook for you, you may         */
  /* *not* use its h_Data field! If you need custom data, use the        */
  /* cl_UserData of the IClass structure!                                */

  if (!(mcc = MUI_CreateCustomClass(NULL,MUIC_Text,NULL,sizeof(struct MyData),MyDispatcher)))
    fail(NULL,"Could not create custom class.");

  app = ApplicationObject,
    MUIA_Application_Title      , "Class5",
    MUIA_Application_Version    , "$VER: Class5 19.5 (12.02.97)",
    MUIA_Application_Copyright  , "©1993, Stefan Stuntz",
    MUIA_Application_Author     , "Stefan Stuntz",
    MUIA_Application_Description, "Demonstrate the use of custom classes.",
    MUIA_Application_Base       , "Class5",

    SubWindow, window = WindowObject,
      MUIA_Window_Title, "Input Handler Class",
      MUIA_Window_ID   , MAKE_ID('C','L','S','5'),
      WindowContents, VGroup,

        Child, TextObject,
          TextFrame,
          MUIA_Background, MUII_TextBack,
          MUIA_Text_Contents, "\33cDemonstration of a class that reacts on\nevents (here: timer signals) automatically.",
          End,

        Child, MyObj = NewObject(mcc->mcc_Class,NULL,
          TextFrame,
          MUIA_Background, MUII_BACKGROUND,
          MUIA_Text_PreParse, "\33c",
          TAG_DONE),

        End,

      End,
    End;

  if (!app)
    fail(app,"Failed to create Application.");

  DoMethod(window,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

/*
** This is the ideal input loop for an object oriented MUI application.
** Everything is encapsulated in classes, no return ids need to be used,
** we just check if the program shall terminate.
** Note that MUIM_Application_NewInput expects sigs to contain the result
** from Wait() (or 0). This makes the input loop significantly faster.
*/

  set(window,MUIA_Window_Open,TRUE);

  {
    ULONG sigs = 0;

    while (DoMethod(app,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit)
    {
      if (sigs)
      {
        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
        if (sigs & SIGBREAKF_CTRL_C) break;
      }
    }
  }

  set(window,MUIA_Window_Open,FALSE);


/*
** Shut down...
*/

  MUI_DisposeObject(app);     /* dispose all objects. */
  MUI_DeleteCustomClass(mcc); /* delete the custom class. */
  fail(NULL,NULL);            /* exit, app is already disposed. */
}
