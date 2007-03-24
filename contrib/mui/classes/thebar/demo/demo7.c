
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
#include <string.h>
#include <stdio.h>

/***********************************************************************/

long __stack = 8192;
struct Library *MUIMasterBase;
struct IntuitionBase *IntuitionBase;

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef __SASC_60
#define SAVEDS   __saveds
#define ASM      __asm
#define REGARGS  __regargs
#define STDARGS  __stdargs
#define INLINE   __inline
#define REG(x,p) register __ ## x p
#define REGARRAY register
#else
#ifndef SAVEDS
#define SAVEDS  __saveds
#endif
#define ASM
#define REGARGS  __regargs
#define STDARGS  __attribute__((stkparm))
#define INLINE   __inline
#define REG(x,p) p __asm( #x )
#define REGARRAY
#endif /* __SASC_60 */

#define mainGroupObject NewObject(mainGroupClass->mcc_Class,NULL

#define DD_FACT 5

/***********************************************************************/

#ifndef __MORPHOS__
ULONG
DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
    return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}
#endif

/***********************************************************************/

struct data
{
    Object *bar;
    Object *list;
    ULONG  barPos;
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
    {7,  0, "_Face",  "Just a face."     },
    {14, 1, "_Mouse", "Your mouse."      },
    {17, 2, "_Tree",  "Mind takes place."},
    {MUIV_TheBar_BarSpacer, -1},
    {26, 3, "_Help",  "Read this!."      },
    {34, 4, "_ARexx", "ARexx for ever!." },
    {1,  5, "_Host",  "Your computer."   },
    {MUIV_TheBar_End},
};

#define TEXT "\
This example shows how to move the bar around.\n\n\
Of course, this is only a semplification of a real situation, where more than four positions are available and many objects are involved.\n\n\
Programmers should always avoid to Init/Exit Change and to set Horiz on groups or whatever when not strictly needed!\
"

#ifdef __MORPHOS__
#define FRAME
#else
#define FRAME MUIA_TheBar_Frame, TRUE,
#endif

ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *bar, *list;

    if (obj = (Object *)DoSuperNew(cl,obj,

            Child, (ULONG)(bar = TheBarVirtObject,
        	FRAME
                MUIA_Group_Horiz,            TRUE,
                MUIA_TheBar_Buttons,         (ULONG)buttons,
                MUIA_TheBar_PicsDrawer,      (ULONG)"PROGDIR:Pics",
                MUIA_TheBar_Strip,           (ULONG)"symbols",
                MUIA_TheBar_StripCols,       16,
                MUIA_TheBar_StripRows,       3,
                MUIA_TheBar_StripHorizSpace, 0,
                MUIA_TheBar_StripVertSpace,  0,
                MUIA_TheBar_DragBar,         TRUE,
                MUIA_TheBar_EnableKeys,      TRUE,
            End),

            Child, (ULONG)(list = ListviewObject,
                MUIA_Listview_List,  FloattextObject,
                    TextFrame,
                    MUIA_Background,     MUII_TextBack,
                    MUIA_Floattext_Text, TEXT,
                End,
            End),

            TAG_MORE, (ULONG)msg->ops_AttrList))
    {
        struct data *data = INST_DATA(cl,obj);

        data->bar    = bar;
        data->list   = list;
        data->barPos = BARPOS_TOP;
    }

    return (ULONG)obj;
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
mDragBegin(struct IClass *cl,Object *obj,struct MUIP_DragBegin *msg)
{
    return 0;
}

/***********************************************************************/

ULONG
findPos(Object *obj,ULONG obPos,ULONG mx,ULONG my)
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
            if (mx<l+w) bPos = BARPOS_LEFT;
            else if (mx>r-w) bPos = BARPOS_RIGHT;
                 else if (my<t+h) bPos = BARPOS_TOP;
                      else if (my>b-h) bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_RIGHT:
            if (mx>r-w) bPos = BARPOS_RIGHT;
            else if (mx<l+w) bPos = BARPOS_LEFT;
                 else if (my<t+h) bPos = BARPOS_TOP;
                      else if (my>b-h) bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_TOP:
            if (my<t+h) bPos = BARPOS_TOP;
            else if (mx<l+w) bPos = BARPOS_LEFT;
                 else if (mx>r-w) bPos = BARPOS_RIGHT;
                      else if (my>b-h) bPos = BARPOS_BOTTOM;
            break;

        case BARPOS_BOTTOM:
            if (my>b-h) bPos = BARPOS_BOTTOM;
            else if (mx<l+w) bPos = BARPOS_LEFT;
                 else if (mx>r-w) bPos = BARPOS_RIGHT;
                      else if (my<t+h) bPos = BARPOS_TOP;
            break;
    }

    return (bPos==obPos) ? 0 : bPos;
}

ULONG
mDragReport(struct IClass *cl,Object *obj,struct MUIP_DragReport *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       bPos, obPos, horiz, ohoriz, first, ofirst;

    if (!msg->update) return MUIV_DragReport_Refresh;

    obPos = data->barPos;
    bPos  = findPos(obj,obPos,msg->x,msg->y);

    if (bPos==0) return MUIV_DragReport_Continue;

    horiz  = (bPos==BARPOS_LEFT)  || (bPos==BARPOS_RIGHT);
    ohoriz = (obPos==BARPOS_LEFT) || (obPos==BARPOS_RIGHT);

    first  = (bPos==BARPOS_TOP)  || (bPos==BARPOS_LEFT);
    ofirst = (obPos==BARPOS_TOP) || (obPos==BARPOS_LEFT);

    set(data->bar,MUIA_TheBar_Limbo,TRUE);

    DoSuperMethod(cl,obj,MUIM_Group_InitChange);

    if (first!=ofirst)
    {
        if (first) DoSuperMethod(cl,obj,MUIM_Group_Sort,data->bar,data->list,NULL);
        else DoSuperMethod(cl,obj,MUIM_Group_Sort,data->list,data->bar,NULL);
    }

    if (horiz!=ohoriz)
    {
        SetSuperAttrs(cl,obj,MUIA_Group_Horiz,horiz,TAG_DONE);
        set(data->bar,MUIA_Group_Horiz,!horiz);
    }

    DoSuperMethod(cl,obj,MUIM_Group_ExitChange);

    set(data->bar,MUIA_TheBar_Limbo,FALSE);

    data->barPos = bPos;

    return MUIV_DragReport_Continue;
}

/***********************************************************************/

ULONG
mDragFinish(struct IClass *cl,Object *obj,struct MUIP_DragFinish *msg)
{
    return 0;
}

/***********************************************************************/

ULONG
mDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    return 0;
}

/***********************************************************************/

#ifdef __MORPHOS__
ULONG dispatcher(void)
{
    struct IClass *cl = (struct IClass *)REG_A0;
    Object        *obj = (Object *)REG_A2;
    Msg            msg  = (Msg)REG_A1;
#else
ULONG ASM SAVEDS dispatcher(REG(a0,struct IClass *cl),REG(a2,Object *obj),REG(a1,Msg msg))
{
#endif

    switch(msg->MethodID)
    {
        case OM_NEW:          return mNew(cl,obj,(APTR)msg);
        case MUIM_DragQuery:  return mDragQuery(cl,obj,(APTR)msg);
        case MUIM_DragBegin:  return mDragBegin(cl,obj,(APTR)msg);
        case MUIM_DragReport: return mDragReport(cl,obj,(APTR)msg);
        case MUIM_DragFinish: return mDragFinish(cl,obj,(APTR)msg);
        case MUIM_DragDrop:   return mDragDrop(cl,obj,(APTR)msg);
        default:              return DoSuperMethodA(cl,obj,msg);
    }
}

#ifdef __MORPHOS__
static struct EmulLibEntry dispatcherTrap = {TRAP_LIB,0,(void *)&dispatcher};
#endif

int
main(int argc,char **argv)
{
    int res;

    if (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37))
    {
        if (MUIMasterBase = OpenLibrary("muimaster.library",19))
        {
            struct MUI_CustomClass *mainGroupClass;

#ifdef __MORPHOS__
            if (mainGroupClass = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct data),(APTR)&dispatcherTrap))
#else
            if (mainGroupClass = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct data),dispatcher))
#endif
            {
                Object *app, *win;

                if (app = ApplicationObject,
                        MUIA_Application_Title,         "TheBar Demo7",
                        MUIA_Application_Version,       "$VER: TheBarDemo7 1.0 (24.6.2003)",
                        MUIA_Application_Copyright,     "Copyright 2003 by Alfonso Ranieri",
                        MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                        MUIA_Application_Description,  "TheBar example",
                        MUIA_Application_Base,         "THEBAREXAMPLE",

                        SubWindow, win = WindowObject,
                            MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                            MUIA_Window_Title,          "TheBar Demo7",
                            WindowContents, VGroup,
                                Child, mainGroupObject, End,
                            End,
                        End,
                    End)
                {
                    ULONG sigs = 0;

                    DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

                    set(win,MUIA_Window_Open,TRUE);

                    while (DoMethod(app,MUIM_Application_NewInput,&sigs)!=MUIV_Application_ReturnID_Quit)
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

            CloseLibrary(MUIMasterBase);
        }
        else
        {
            printf("%s: Can't open muimaster.library ver 19 or higher\n",argv[0]);
            res = RETURN_ERROR;
        }

        CloseLibrary((struct Library *)IntuitionBase);
    }
    else
    {
        printf("%s: Can't open intuition.library ver 37 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }


    return res;
}

/***********************************************************************/
