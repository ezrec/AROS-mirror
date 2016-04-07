#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
#include <string.h>
#include <stdio.h>
#include "SDI_compiler.h"
#include "SDI_stdarg.h"
#include "SDI_hook.h"

#if !defined(__amigaos4__)
#include <clib/alib_protos.h>
#endif

/***********************************************************************/

long __stack = 8192;
#if defined(__amigaos4__)
struct Library *IntuitionBase;
struct Library *MUIMasterBase;
struct IntuitionIFace *IIntuition;
struct MUIMasterIFace *IMUIMaster;
#else
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;
#endif

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#define mainGroupObject NewObject(mainGroupClass->mcc_Class,NULL

#define DD_FACT 5

#define MUIM_MainGroup_AdjustBar (TAG_USER+1)

/***********************************************************************/

#if defined(__amigaos4__)
#define CLOSELIB(lib, iface)              { if((iface) && (lib)) { DropInterface((APTR)(iface)); iface = NULL; CloseLibrary((struct Library *)lib); lib = NULL; } }
#define GETINTERFACE(iname, iface, base)  ((iface) = (APTR)GetInterface((struct Library *)(base), (iname), 1L, NULL))
#define DROPINTERFACE(iface)              { DropInterface((APTR)(iface)); iface = NULL; }
#else
#define CLOSELIB(lib, iface)              { if((lib)) { CloseLibrary((struct Library *)lib); lib = NULL; } }
#define GETINTERFACE(iname, iface, base)  TRUE
#define DROPINTERFACE(iface)              ((void)0)
#endif

/***********************************************************************/

#if defined(__AROS__)
Object * DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...)
{
  AROS_SLOWSTACKTAGS_PRE_AS(tag1, Object *)
  retval = (Object *)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
  AROS_SLOWSTACKTAGS_POST
}
#elif defined(__MORPHOS__)
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
  VA_END(args);

  return rc;
}
#endif

/***********************************************************************/

struct data
{
    Object *bar;
    Object *list;
    ULONG  barPos;
    ULONG  nbarPos;
    ULONG  ibarPos;

    BOOL noHoriz;
    BOOL noVert;
};

enum
{
    BARPOS_TOP = 1,
    BARPOS_BOTTOM,
    BARPOS_LEFT,
    BARPOS_RIGHT,
};

/***********************************************************************/

struct MUIS_TheBar_Button buttons[] =
{
    {7,  0, "_Face",  "Just a face.",      0, 0, NULL, NULL},
    {14, 1, "_Mouse", "Your mouse.",       0, 0, NULL, NULL},
    {17, 2, "_Tree",  "Mind takes place.", 0, 0, NULL, NULL},
    {MUIV_TheBar_BarSpacer, -1, NULL, NULL,0, 0, NULL, NULL},
    {26, 3, "_Help",  "Read this!.",       0, 0, NULL, NULL},
    {34, 4, "_ARexx", "ARexx for ever!.",  0, 0, NULL, NULL},
    {1,  5, "_Host",  "Your computer.",    0, 0, NULL, NULL},
    {MUIV_TheBar_End,-1,NULL, NULL,        0, 0, NULL, NULL}
};

#define TEXT "\
This example shows how to move the bar around.\n\n\
Of course, this is only a simplification of a real situation, where more than four positions are available and many objects are involved.\n\n\
Programmers should always avoid to Init/Exit Change and to set Horiz on groups or whatever when not strictly needed!\
"

IPTR
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *bar, *list;

    obj = (Object *)DoSuperNew(cl,obj,
            Child, bar = TheBarVirtObject,
                MUIA_Group_Horiz,            TRUE,
                MUIA_TheBar_Buttons,         buttons,
                MUIA_TheBar_PicsDrawer,      "PROGDIR:Pics",
                MUIA_TheBar_Strip,           "symbols",
                MUIA_TheBar_StripCols,       16,
                MUIA_TheBar_StripRows,       3,
                MUIA_TheBar_StripHSpace,     0,
                MUIA_TheBar_StripVSpace,     0,
                MUIA_TheBar_Frame,           TRUE,
                MUIA_TheBar_DragBar,         TRUE,
                MUIA_TheBar_EnableKeys,      TRUE,
                MUIA_TheBar_IgnoreAppearance,TRUE,
            End,

            Child, list = ListviewObject,
                MUIA_Listview_List,  FloattextObject,
                    TextFrame,
                    MUIA_Background,     MUII_TextBack,
                    MUIA_Floattext_Text, TEXT,
                End,
            End,

            TAG_MORE,msg->ops_AttrList);

    if (obj)
    {
        struct data *data = INST_DATA(cl,obj);

        data->bar    = bar;
        data->list   = list;
        data->barPos = BARPOS_TOP;
    }

    return (IPTR)obj;
}

/***********************************************************************/

ULONG
mDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    struct data *data = INST_DATA(cl,obj);

    return (ULONG)((msg->obj==data->bar) ? MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse);
}

/***********************************************************************/

ULONG
mDragBegin(struct IClass *cl,Object *obj,UNUSED struct MUIP_DragBegin *msg)
{
    struct data *data = INST_DATA(cl,obj);

    data->ibarPos = 0;

    return 0;
}

/***********************************************************************/

ULONG
findPos(Object *obj,ULONG obPos,LONG mx,LONG my)
{
    ULONG bPos;
    LONG  l, t, r, b, w, h;

    l = _mleft(obj);
    t = _mtop(obj);
    r = _mright(obj);
    b = _mbottom(obj);
    w = _mwidth(obj)/DD_FACT;
    h = _mheight(obj)/DD_FACT;

    bPos = 0;

    switch (obPos)
    {
        case BARPOS_LEFT:
            if (mx<l+w)
                bPos = BARPOS_LEFT;
            else if (mx>r-w)
                bPos = BARPOS_RIGHT;
            else if (my<t+h)
                bPos = BARPOS_TOP;
            else if (my>b-h)
                bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_RIGHT:
            if (mx>r-w)
                bPos = BARPOS_RIGHT;
            else if (mx<l+w)
                bPos = BARPOS_LEFT;
            else if (my<t+h)
                bPos = BARPOS_TOP;
            else if (my>b-h)
                bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_TOP:
            if (my<t+h)
                bPos = BARPOS_TOP;
            else if (mx<l+w)
                bPos = BARPOS_LEFT;
            else if (mx>r-w)
                bPos = BARPOS_RIGHT;
            else if (my>b-h)
                bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_BOTTOM:
            if (my>b-h)
                bPos = BARPOS_BOTTOM;
            else if (mx<l+w)
                bPos = BARPOS_LEFT;
            else if (mx>r-w)
                bPos = BARPOS_RIGHT;
            else if (my<t+h)
                bPos = BARPOS_TOP;
            break;
    }

    return (bPos==obPos) ? 0 : bPos;
}

ULONG
mDragReport(struct IClass *cl,Object *obj,struct MUIP_DragReport *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       bPos, obPos;

    if (!msg->update)
        return MUIV_DragReport_Refresh;

    obPos = data->barPos;
    bPos  = findPos(obj,obPos,msg->x,msg->y);

    if (bPos==0)
        return MUIV_DragReport_Continue;

    switch (obPos)
    {
        case BARPOS_TOP:
        case BARPOS_BOTTOM:
            if ((bPos==BARPOS_LEFT || bPos==BARPOS_RIGHT) && data->noVert)
            {
                data->ibarPos = bPos;
                return MUIV_DragReport_Continue;
            }
            break;

        case BARPOS_LEFT:
        case BARPOS_RIGHT:
            if ((bPos==BARPOS_TOP || bPos==BARPOS_BOTTOM) && data->noHoriz)
            {
                data->ibarPos = bPos;
                return MUIV_DragReport_Continue;
            }
            break;
    }

    data->nbarPos = bPos;
    data->ibarPos = 0;
    DoMethod(obj,MUIM_MainGroup_AdjustBar);

    return MUIV_DragReport_Continue;
}

/***********************************************************************/

ULONG
mDragFinish(struct IClass *cl,Object *obj,UNUSED struct MUIP_DragFinish *msg)
{
    struct data *data = INST_DATA(cl,obj);

    return data->ibarPos ? MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse;
}

/***********************************************************************/

ULONG
mDragDrop(struct IClass *cl,Object *obj,UNUSED struct MUIP_DragDrop *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->ibarPos)
    {
        data->nbarPos = data->ibarPos;
        DoMethod(obj,MUIM_MainGroup_AdjustBar);
    }

    return 0;
}

/***********************************************************************/

ULONG
mAdjustBar(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       bPos, obPos, horiz, ohoriz, first, ofirst;

    obPos = data->barPos;
    bPos  = data->nbarPos;

    horiz  = (bPos==BARPOS_LEFT)  || (bPos==BARPOS_RIGHT);
    ohoriz = (obPos==BARPOS_LEFT) || (obPos==BARPOS_RIGHT);

    first  = (bPos==BARPOS_TOP)  || (bPos==BARPOS_LEFT);
    ofirst = (obPos==BARPOS_TOP) || (obPos==BARPOS_LEFT);

    set(data->bar,MUIA_TheBar_Limbo,TRUE);

    DoSuperMethod(cl,obj,MUIM_Group_InitChange);

    if (first!=ofirst)
    {
        if (first)
            DoSuperMethod(cl,obj,MUIM_Group_Sort,data->bar,data->list,NULL);
        else
            DoSuperMethod(cl,obj,MUIM_Group_Sort,data->list,data->bar,NULL);
    }

    if (horiz!=ohoriz)
    {
        SetSuperAttrs(cl,obj,MUIA_Group_Horiz,horiz,TAG_DONE);
        set(data->bar,MUIA_Group_Horiz,!horiz);
    }

    DoSuperMethod(cl,obj,MUIM_Group_ExitChange);

    set(data->bar,MUIA_TheBar_Limbo,FALSE);

    data->barPos = bPos;

    return 0;
}

/***********************************************************************/

ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    Object *parent = NULL;

    if (!DoSuperMethodA(cl,obj,msg))
        return FALSE;

    // get the parent group, containing the list and the bar
    get(obj, MUIA_Parent, &parent);

    // These are rough estimations assuming that the single buttons have square dimensions.
    // Horizontal placement is forbidden if the group's width is less than the bar's height.
    data->noHoriz = _width(parent) < _minheight(data->bar);
    // Vertical placement is forbidden if the group's height is less than the bar's width.
    data->noVert = _height(parent) < _minwidth(data->bar);

    return TRUE;
}

/***********************************************************************/

DISPATCHER(_dispatcher)
{
    switch(msg->MethodID)
    {
        case OM_NEW:                   return mNew(cl,obj,(APTR)msg);

        case MUIM_Show:                return mShow(cl,obj,(APTR)msg);
        case MUIM_DragQuery:           return mDragQuery(cl,obj,(APTR)msg);
        case MUIM_DragBegin:           return mDragBegin(cl,obj,(APTR)msg);
        case MUIM_DragReport:          return mDragReport(cl,obj,(APTR)msg);
        case MUIM_DragFinish:          return mDragFinish(cl,obj,(APTR)msg);
        case MUIM_DragDrop:            return mDragDrop(cl,obj,(APTR)msg);

        case MUIM_MainGroup_AdjustBar: return mAdjustBar(cl,obj,(APTR)msg);

        default:                       return DoSuperMethodA(cl,obj,msg);
    }
}

const char *usedClasses[] = {"TheBar.mcc",NULL};
    
int
main(UNUSED int argc,char **argv)
{
    int res;

    if ((IntuitionBase = (APTR)OpenLibrary("intuition.library",39)) != NULL &&
        GETINTERFACE("main", IIntuition, IntuitionBase))
    {
        if ((MUIMasterBase = OpenLibrary("muimaster.library",19)) != NULL &&
            GETINTERFACE("main", IMUIMaster, MUIMasterBase))
        {
            struct MUI_CustomClass *mainGroupClass;

            if ((mainGroupClass = MUI_CreateCustomClass(NULL,(STRPTR)MUIC_Group,NULL,sizeof(struct data),ENTRY(_dispatcher))) != NULL)
            {
                Object *app, *win;

                if ((app = ApplicationObject,
                        MUIA_Application_Title,         "TheBar Demo7",
                        MUIA_Application_Version,       "$VER: TheBarDemo7 1.1 (29.2.2008)",
                        MUIA_Application_Copyright,     "Copyright 2008 by Alfonso Ranieri",
                        MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                        MUIA_Application_Description,   "TheBar example",
                        MUIA_Application_Base,          "THEBAREXAMPLE",
                        MUIA_Application_UsedClasses,   usedClasses,

                        SubWindow, win = WindowObject,
                            MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                            MUIA_Window_Title,          "TheBar Demo7",
                            WindowContents, VGroup,
                                Child, mainGroupObject, End,
                            End,
                        End,
                    End) != NULL)
                {
                    ULONG sigs = 0;

                    DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

                    set(win,MUIA_Window_Open,TRUE);

                    while ((LONG)DoMethod(app,MUIM_Application_NewInput,&sigs)!=MUIV_Application_ReturnID_Quit)
                    {
                        if (sigs)
                        {
                            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                            if (sigs & SIGBREAKF_CTRL_C) break;
                        }
                    }

                    MUI_DisposeObject(app);

                    res = RETURN_OK;
                }
                else
                {
                    printf("%s: can't create application\n",argv[0]);
                    res = RETURN_FAIL;
                }

                MUI_DeleteCustomClass(mainGroupClass);
            }
            else
            {
                printf("%s: can't create custom class\n",argv[0]);
                res = RETURN_FAIL;
            }

            CLOSELIB(MUIMasterBase, IMUIMaster);
        }
        else
        {
            printf("%s: Can't open muimaster.library ver 19 or higher\n",argv[0]);
            res = RETURN_ERROR;
        }

        CLOSELIB(IntuitionBase, IIntuition);
    }
    else
    {
        printf("%s: Can't open intuition.library ver 37 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }


    return res;
}

/***********************************************************************/
