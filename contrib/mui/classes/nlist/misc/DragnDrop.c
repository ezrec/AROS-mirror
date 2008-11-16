/*
** The Settings Demo shows how to load and save object contents.
*/

/*#include "mui.h"*/

#include "demo.h"

#include <MUI/NListview_mcc.h>

#define SAVEDS
#define ASM

static const char *nase[] =
{
  "Line  1",
  "Line  2",
  "Line  3",
  "Line  4",
  "Line  5",
  "Line  6",
  "Line  7",
  "Line  8",
  "Line  9",
  "Line 10",
  "Line 11",
  "Line 12",
  "Line 13",
  "Line 14",
  "Line 15",
  "Line 16",
  "Line 17",
  "Line 18",
  "Line 19",
  "Line 20",
  "Line 21",
  "Line 22",
  "Line 23",
  "Line 24",
  "Line 25",
  "Line 26",
  "Line 27",
  "Line 28   Line 28   Line 28   Line 28   Line 28",
  "Line 29",
  "Line 30",
  "Line 31",
  "Line 32",
  "Line 33",
  "Line 34",
  "Line 35",
  "Line 36",
  "Line 37",
  "Line 38",
  "Line 39",
  "Line 40",
  "Line 41",
  "Line 42",
  "Line 43",
  "Line 44",
  "Line 45",
  "Line 46",
  "Line 47",
  "Line 48",
  "Line 49",
  "Line 50",
   NULL,
};



struct MUI_CustomClass *CL_FieldsList;
struct MUI_CustomClass *CL_ChooseFields;


ULONG __stdargs DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
  return(DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL));
}



/****************************************************************************/
/* FieldsList class                                                         */
/****************************************************************************/

/*
** FieldsList list class creates a list that accepts D&D items
** from exactly one other listview, the one which is stored in
** the objects userdata. You could also store the allowed source
** object in another private attribute, I was just too lazy to add
** the get/set methods in this case. :-)
**
** This class is designed to be used for both, the list of the
** available fields and the list of the visible fields.
**
** Note: Stop being afraid of custom classes. This one here takes
** just a few lines of code, 90% of the stuff below is comments. :-)
*/


struct FieldsList_Data
{
  LONG dummy;
};


ULONG FieldsList_DragQuery(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
  if (msg->obj==obj)
  {
    /*
    ** If somebody tried to drag ourselves onto ourselves, we let our superclass
    ** (the list class) handle the necessary actions. Depending on the state of
    ** its MUIA_NList_DragSortable attribute, it will either accept or refuse to become
    ** the destination object.
    */

    return(DoSuperMethodA(cl,obj,msg));
  }
  else if (msg->obj==(Object *)muiUserData(obj))
  {
    /*
    ** If our predefined source object wants us to become active,
    ** we politely accept it.
    */

    return(MUIV_DragQuery_Accept);
  }
  else
  {
    /*
    ** Otherwise, someone tried to feed us with something we don't like
    ** very much. Just refuse it.
    */

    return(MUIV_DragQuery_Refuse);
  }

}


ULONG FieldsList_DragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
  if (msg->obj==obj)
  {
    /*
    ** user wants to move entries within our object, these kinds of actions
    ** can get quite complicated, but fortunately, everything is automatically
    ** handled by list class. We just need to pass through the method.
    */

    return(DoSuperMethodA(cl,obj,msg));
  }
  else
  {
    /*
    ** we can be sure that msg->obj is our predefined source object
    ** since we wouldnt have accepted the MUIM_DragQuery and wouldnt
    ** have become an active destination object otherwise.
    */

    APTR entry;
    LONG dropmark;
    LONG sortable;

get(obj,MUIA_NList_DropMark,&dropmark);
kprintf("%lx|MUIA_NList_DropMark=%ld\n",obj,dropmark);

    /*
    ** get the current entry from the source object, remove it there, and insert
    ** it to ourselves. You would have to do a little more action if the source
    ** listview would support multi select, but this one doesnt, so things get
    ** quite easy. Note that this direct removing/adding of list entries is only
    ** possible if both contain lists simple pointers to static items. If they would
    ** feature custom construct/destruct hooks, we'd need to create a copy of
    ** the entries instead of simply moving pointers.
    */

    /* get source entry */
    DoMethod(msg->obj,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active,&entry);
    /* remove source entry */
    DoMethod(msg->obj,MUIM_NList_Remove,MUIV_NList_Remove_Active);
    get(obj,MUIA_NList_DragSortable,&sortable);
    if (sortable)
    {
      /*
      ** if we are in a sortable list (in our case the visible fields),
      ** we need to make sure to insert the new entry at the correct
      ** position. The MUIA_NList_DropMark attribute is maintained
      ** by list class and shows us where we shall go after a drop.
      */

      get(obj,MUIA_NList_DropMark,&dropmark);
      DoMethod(obj,MUIM_NList_InsertSingle,entry,dropmark);
    }
    else
    {
      /*
      ** we are about to return something to the available fields
      ** listview which is always sorted.
      */

      DoMethod(obj,MUIM_NList_InsertSingle,entry,MUIV_NList_Insert_Sorted);
    }

    /*
    ** make the insterted object the active and make the source listviews
    ** active object inactive to give some more visual feedback to the user.
    */

    get(obj,MUIA_NList_InsertPosition,&dropmark);
/*    set(obj,MUIA_NList_Active,dropmark);*/
/*    set(msg->obj,MUIA_NList_Active,MUIV_NList_Active_Off);*/

    return(0);
  }
}


/*SAVEDS ASM ULONG FieldsList_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
ULONG FieldsList_Dispatcher(LONG pad)
{
  register struct IClass *a0 __asm("a0");
  struct IClass *cl = a0;
  register Object *a2 __asm("a2");
  Object *obj = a2;
  register Msg a1 __asm("a1");
  Msg msg = a1;

  switch (msg->MethodID)
  {
    case MUIM_DragQuery: return(FieldsList_DragQuery(cl,obj,(APTR)msg));
    case MUIM_DragDrop : return(FieldsList_DragDrop (cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}



/****************************************************************************/
/* ChooseFields class                                                       */
/****************************************************************************/

/*
** This class creates two listviews, one contains all available fields,
** the other one contains the visible fields. You can control this
** stuff completely with D&D. This thing could e.g. be useful to
** configure the display of an address utility.
*/

static const char *fields[] =
{
  "Age",
  "Birthday",
  "c/o",
  "City",
  "Comment",
  "Country",
  "EMail",
  "Fax",
  "First name",
  "Job   Line 28   Line 28   Line 28   Line 28",
  "Name",
  "Phone",
  "Projects",
  "Salutation",
  "State",
  "Street",
  "ZIP",
  NULL
};

struct ChooseFields_Data
{
  LONG dummy;
};

ULONG ChooseFields_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  Object *available, *visible;

  obj = (Object *)DoSuperNew(cl,obj,
    MUIA_Group_Columns, 2,
    MUIA_Group_VertSpacing, 1,
    Child, TextObject, MUIA_Text_Contents, "\33cAvailable Fields\n(alpha sorted)", End,
    Child, TextObject, MUIA_Text_Contents, "\33cVisible Fields\n(sortable)", End,
    Child, NListviewObject,
      MUIA_NListview_NList, available = NewObject(CL_FieldsList->mcc_Class,NULL,
        InputListFrame,
/*MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,*/
        MUIA_NList_DragType, MUIV_NList_DragType_Default,
        MUIA_NList_SourceArray, (char **)fields,
        MUIA_NList_ShowDropMarks, FALSE,
              MUIA_Dropable, TRUE,
        TAG_DONE),
      End,
    Child, NListviewObject,
      MUIA_NListview_NList, visible = NewObject(CL_FieldsList->mcc_Class,NULL,
        MUIA_NList_DragType, MUIV_NList_DragType_Default,
        InputListFrame,
        MUIA_NList_DragSortable, TRUE,
              MUIA_Dropable, TRUE,
        TAG_DONE),
      End,
    End;

  if (obj)
  {
    /*
    ** tell available object to accept items from visible object.
    ** the use of MUIA_UserData is just to make the FieldsList
    ** subclass more simple.
    */

    set(available,MUIA_UserData,visible);

    /*
    ** the other way round...
    */

    set(visible,MUIA_UserData,available);
  }

  return((ULONG)obj);
}

/*SAVEDS ASM ULONG ChooseFields_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
ULONG ChooseFields_Dispatcher(LONG pad)
{
  register struct IClass *a0 __asm("a0");
  struct IClass *cl = a0;
  register Object *a2 __asm("a2");
  Object *obj = a2;
  register Msg a1 __asm("a1");
  Msg msg = a1;

  switch (msg->MethodID)
  {
    case OM_NEW: return(ChooseFields_New(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}



/****************************************************************************/
/* Main Program                                                             */
/****************************************************************************/


VOID ExitClasses(VOID)
{
  if (CL_FieldsList  ) MUI_DeleteCustomClass(CL_FieldsList  );
  if (CL_ChooseFields) MUI_DeleteCustomClass(CL_ChooseFields);
}


BOOL InitClasses(VOID)
{
  CL_FieldsList    = MUI_CreateCustomClass(NULL,MUIC_NList ,NULL,sizeof(struct FieldsList_Data),FieldsList_Dispatcher);
  CL_ChooseFields  = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct ChooseFields_Data ),ChooseFields_Dispatcher );

  if (CL_FieldsList && CL_ChooseFields)
    return(TRUE);

  ExitClasses();
  return(FALSE);
}


int main(int argc,char *argv[])
{
  Object *app,*window;

  init();

  if (!InitClasses())
    fail(NULL,"failed to init classes.");

  app = ApplicationObject,
    MUIA_Application_Title      , "DragnDrop",
    MUIA_Application_Version    , "$VER: DragnDrop 17.6 (18.08.96)",
    MUIA_Application_Copyright  , "©1992-95, Stefan Stuntz",
    MUIA_Application_Author     , "Stefan Stuntz",
    MUIA_Application_Description, "Demonstrate Drag & Drop capabilities",
    MUIA_Application_Base       , "DRAGNDROP",

    SubWindow, window = WindowObject,
      MUIA_Window_Title, "Drag&Drop Demo",
      MUIA_Window_ID   , MAKE_ID('D','R','A','G'),
      WindowContents, VGroup,

        Child, NewObject(CL_ChooseFields->mcc_Class,NULL,TAG_DONE),

        Child, ColGroup(2),
          Child, TextObject, TextFrame, MUIA_Background, MUII_TextBack, MUIA_Text_Contents, "\33cNListview without\nmultiple selection.", End,
          Child, TextObject, TextFrame, MUIA_Background, MUII_TextBack, MUIA_Text_Contents, "\33cNListview with\nmultiple selection.", End,
          Child, NListviewObject,
            MUIA_NListview_NList, NListObject,
              MUIA_NList_DragType, MUIV_NList_DragType_Default,
              MUIA_Dropable, FALSE,
              InputListFrame,
              MUIA_NList_SourceArray, nase,
              MUIA_NList_DragSortable, TRUE,
              End,
            End,
          Child, NListviewObject,
            MUIA_NListview_NList, NListObject,
              MUIA_Dropable, TRUE,
              MUIA_NList_DragType, MUIV_NList_DragType_Default,
              MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,
              InputListFrame,
              MUIA_NList_SourceArray, nase,
              MUIA_NList_DragSortable, TRUE,
              End,
            End,
          End,
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

  MUI_DisposeObject(app); /* note that you must first dispose all objects */
  ExitClasses();          /* *before* deleting the classes! */
  fail(NULL,NULL);
}
