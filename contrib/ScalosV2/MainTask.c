// :ts=4 (Tabsize)
/* Scalos - MainTask */

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>
#include <exec/semaphores.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scalos_protos.h"

#include "SubRoutines.h"
#include "MainTask.h"
#include "ScalosIntern.h"
#include "WindowClass.h"
#include "ScreenClass.h"
#include "AreaClass.h"
#include "WindowTaskClass.h"
#include "Scalos.h"
#include "PlanarTestClass.h"
#include "Chunky8TestClass.h"
#include "BitmapClass.h"
#include "GroupClass.h"
#include "RectFillClass.h"
#include "PanelClass.h"
#include "PanelViewClass.h"
#include "IconPanelClass.h"
#include "GraphicClass.h"
#include "IconClass.h"
#include "TextClass.h"
#include "GFXTestClass.h"
#include "AmigaIconClass.h"
#include "DOSIconClass.h"
#include "DOSListIconClass.h"
#include "DOSDeviceIconClass.h"
#include "DOSDirIconClass.h"
#include "DOSFileIconClass.h"
#include "TableClass.h"
#include "NodeListClass.h"

#include "Debug.h"
#include "CompilerSupport.h"

#ifndef __STORM__
static const char Version[] = VERSTAG" "CPU" "__AMIGADATE__;
#else
static const char Version[] = VERSTAG" "__DATE__;
#endif

/* ------------------------ Main Ptrs ----------------------------------*/

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *GuiGFXBase = NULL;
struct Library *LayersBase = NULL;

struct MinList ScreenList;              // used in ScreenClass
struct SignalSemaphore ScreenListSem;

/* -------------------------- Lists ------------------------------------*/

extern ULONG ScalosMasterInit(void);
extern ScalosMasterExit(void);

extern struct SC_MethodData WindowMethods[];
extern struct SC_MethodData ScreenMethods[];
extern struct SC_MethodData AreaMethods[];
extern struct SC_MethodData WindowTaskMethods[];
extern struct SC_MethodData BitmapMethods[];
extern struct SC_MethodData GroupMethods[];
extern struct SC_MethodData PlanarTestMethods[];
extern struct SC_MethodData Chunky8TestMethods[];
extern struct SC_MethodData RectFillMethods[];
extern struct SC_MethodData PanelMethods[];
extern struct SC_MethodData PanelViewMethods[];
extern struct SC_MethodData IconPanelMethods[];
extern struct SC_MethodData GraphicMethods[];
extern struct SC_MethodData IconMethods[];
extern struct SC_MethodData TextMethods[];
extern struct SC_MethodData GFXTestMethods[];
extern struct SC_MethodData AmigaIconMethods[];
extern struct SC_MethodData DOSIconMethods[];
extern struct SC_MethodData DOSListIconMethods[];
extern struct SC_MethodData DOSDirIconMethods[];
extern struct SC_MethodData DOSFileIconMethods[];
extern struct SC_MethodData DOSDeviceIconMethods[];
extern struct SC_MethodData TableMethods[];
extern struct SC_MethodData NodeListMethods[];

extern ULONG MethodSenderDispatcher(struct IClass *cl, Msg msg, Object *obj);

struct LibraryList
{
        char             *LibName;
        struct Library  **LibBase;
        int               Version;
} liblist[] =
{
        { "intuition.library",(struct Library **) &IntuitionBase,0 },   /* version 0 to use EasyRequest() */
        { "graphics.library",(struct Library **) &GfxBase,39 },
        { "utility.library", &UtilityBase,39 },
        { "layers.library", &LayersBase,39 },
        { "guigfx.library", &GuiGFXBase,8 },
        { NULL,NULL,0 }
};

struct ClassList
{
        char    *ClassName;
        char    *SuperClassName;
        ULONG   InstSize;
        struct  SC_MethodData *methodlist;
        struct  SC_Class *Class;

}   classlist[] =
{
        {SCC_WINDOW_NAME,SCC_THREADROOT_NAME,sizeof(struct WindowInst),(struct SC_MethodData *) &WindowMethods,NULL},
        {SCC_SCREEN_NAME,SCC_ROOT_NAME,sizeof(struct ScreenInst),(struct SC_MethodData *) &ScreenMethods,NULL},
        {SCC_AREA_NAME,SCC_ROOT_NAME,sizeof(struct AreaInst),(struct SC_MethodData *) &AreaMethods,NULL},
        {SCC_THREADAREA_NAME,SCC_THREADROOT_NAME,sizeof(struct AreaInst),(struct SC_MethodData *) &AreaMethods,NULL},
        {SCC_WINDOWTASK_NAME,SCC_ROOT_NAME,sizeof(struct WindowTaskInst),(struct SC_MethodData *) &WindowTaskMethods,NULL},
        {SCC_GRAPHIC_NAME,SCC_ROOT_NAME,sizeof(struct GraphicInst),(struct SC_MethodData *) &GraphicMethods,NULL},
        {SCC_BITMAP_NAME,SCC_GRAPHIC_NAME,sizeof(struct BitmapInst),(struct SC_MethodData *) &BitmapMethods,NULL},
        {SCC_GROUP_NAME,SCC_AREA_NAME,sizeof(struct GroupInst),(struct SC_MethodData *) &GroupMethods,NULL},
        {SCC_PANEL_NAME,SCC_AREA_NAME,sizeof(struct PanelInst),(struct SC_MethodData *) &PanelMethods,NULL},
        {SCC_PANELVIEW_NAME,SCC_GROUP_NAME,sizeof(struct PanelViewInst),(struct SC_MethodData *) &PanelViewMethods,NULL},
        {SCC_ICONPANEL_NAME,SCC_PANEL_NAME,sizeof(struct IconPanelInst),(struct SC_MethodData *) &IconPanelMethods,NULL},
        {SCC_ICON_NAME,SCC_GRAPHIC_NAME,sizeof(struct IconInst),(struct SC_MethodData *) &IconMethods,NULL},
        {SCC_TEXT_NAME,SCC_GRAPHIC_NAME,sizeof(struct TextInst),(struct SC_MethodData *) &TextMethods,NULL},
        {SCC_AMIGAICON_NAME,SCC_ICON_NAME,sizeof(struct AmigaIconInst),(struct SC_MethodData *) &AmigaIconMethods,NULL},
        {SCC_DOSICON_NAME,SCC_ICON_NAME,sizeof(struct DOSIconInst),(struct SC_MethodData *) &DOSIconMethods,NULL},
        {SCC_DOSLISTICON_NAME,SCC_ICON_NAME,sizeof(struct DOSListIconInst),(struct SC_MethodData *) &DOSListIconMethods,NULL},
        {SCC_DOSDIRICON_NAME,SCC_DOSICON_NAME,sizeof(struct DOSDirIconInst),(struct SC_MethodData *) &DOSDirIconMethods,NULL},
        {SCC_DOSFILEICON_NAME,SCC_DOSICON_NAME,sizeof(struct DOSFileIconInst),(struct SC_MethodData *) &DOSFileIconMethods,NULL},
        {SCC_DOSDEVICEICON_NAME,SCC_DOSDIRICON_NAME,sizeof(struct DOSDeviceIconInst),(struct SC_MethodData *) &DOSDeviceIconMethods,NULL},
        {SCC_TABLE_NAME,SCC_ROOT_NAME,sizeof(struct TableInst),(struct SC_MethodData *) &TableMethods,NULL},
        {SCC_NODELIST_NAME,SCC_ROOT_NAME,sizeof(struct NodeListInst),(struct SC_MethodData *) &NodeListMethods,NULL},

//  {"PlanarTest.scalos",SCC_AREA_NAME,sizeof(struct PlanarTestInst),(struct SC_MethodData *) &PlanarTestMethods,NULL},
        {"PlanarTest.scalos",SCC_THREADAREA_NAME,sizeof(struct PlanarTestInst),(struct SC_MethodData *) &PlanarTestMethods,NULL},
//  {"Chunky8Test.scalos",SCC_AREA_NAME,sizeof(struct Chunky8TestInst),(struct SC_MethodData *) &Chunky8TestMethods,NULL},
        {"Chunky8Test.scalos",SCC_THREADAREA_NAME,sizeof(struct Chunky8TestInst),(struct SC_MethodData *) &Chunky8TestMethods,NULL},
//  {"RectFill.scalos",SCC_AREA_NAME,sizeof(struct RectFillInst),(struct SC_MethodData *) &RectFillMethods,NULL},
        {"RectFill.scalos",SCC_THREADAREA_NAME,sizeof(struct RectFillInst),(struct SC_MethodData *) &RectFillMethods,NULL},
        {"GFXTest.scalos",SCC_AREA_NAME,sizeof(struct GFXTestInst),(struct SC_MethodData *) &GFXTestMethods,NULL},

        {NULL,NULL,0,NULL}
};

/*------------------------ Functions -----------------------------------*/


BOOL OpenLibs(void)
{
        int i;
        struct  EasyStruct easystruct =
        {
                sizeof (struct EasyStruct),
                NULL,
                "Scalos Error",
                "Error opening '%s' version %ld !",
                "Ok"
        };

        for (i=0;(liblist[i].LibName);i++)
        {
                if (!(*(liblist[i].LibBase) = (struct Library *) OpenLibrary(liblist[i].LibName,(ULONG) liblist[i].Version)))
                {
                        if (IntuitionBase) EasyRequest(NULL,&easystruct,NULL,liblist[i].LibName,(ULONG) liblist[i].Version);
                        return FALSE;
                }
        }
        return TRUE;
}

void CloseLibs(void)
{
        int i;

        for (i=0;liblist[i].LibName;i++)
        {
                if (*liblist[i].LibBase)
                        CloseLibrary(*liblist[i].LibBase);
        }
}

BOOL InitClasses(void)
{
        int i;
        struct  EasyStruct easystruct =
        {
                sizeof (struct EasyStruct),
                NULL,
                "Scalos Error",
                "Error making '%s' class!",
                "Ok"
        };
        
        for (i=0;classlist[i].ClassName;i++)
        {
                if(!(classlist[i].Class = SC_MakeSimpleClass( classlist[i].ClassName,classlist[i].SuperClassName,NULL,classlist[i].methodlist,classlist[i].InstSize,0 )))
                {
                        EasyRequest(NULL,&easystruct,NULL,classlist[i].ClassName);
                        return FALSE;
                }
        }

        return TRUE;
}

void RemClasses(void)
{
        int i;

        for (i=0;classlist[i].ClassName;i++);

        while (i-- != 0)
        {
                if (classlist[i].Class)
                {
                        if (SC_FreeClass(classlist[i].Class))
                                classlist[i].Class = NULL;
                }
        }
}


BOOL Init(void)
{
        if(OpenLibs() && ScalosMasterInit())
        {

                if(InitClasses())
                {
                        NewList((struct List *) &ScreenList);
                        memset(&ScreenListSem,0,sizeof(struct SignalSemaphore));
                        InitSemaphore(&ScreenListSem);
                        return TRUE;
                }
                CloseLibs();
        }
        return FALSE;
}


void fail(Object *app, char *errortxt)
{
        if(errortxt) puts(errortxt);
        SC_DisposeObject( app );
        RemClasses();
        CloseLibs();
        ScalosMasterExit();
        if(errortxt) exit(EXIT_FAILURE);
        else exit(EXIT_SUCCESS);
}
        

/* -------------------------- Main ----------------------------------- */

void main(void)
{
        Object  *app;
        ULONG   signal = 0;

        if (Init())
        {

                        /*
                Object *o1;
                Object *o2;
                Object *o3;

                o1 = SC_NewObject(NULL,SCC_DOSDIRICON_NAME,SCCA_Icon_Name,"dh0:WBStartup",TAG_DONE);


                SC_DoMethod(o1,SCCM_Icon_GetIcon,SCCV_Icon_GetIcon_Disk);

                o1 = SC_NewObject(NULL,SCC_DOSLISTICON_NAME,TAG_DONE);

                SC_DoMethod(o1,SCCM_Icon_Entry,SCCV_Icon_Entry_First);
                o3 = (Object *) SC_DoMethod(o1,SCCM_Icon_GetObject,TAG_DONE);

                do
                {
                        char *str;
                        if (o2 = (Object *) SC_DoMethod(o1,SCCM_Icon_GetObject,TAG_DONE))
                        {
                                printf("%s - %s  Type: %s\n",get(o2,SCCA_Icon_Name),get(o2,SCCA_DOSDeviceIcon_DeviceName),str = (char *) SC_DoMethod(o2,SCCM_Icon_GetString,SCCA_DOSDeviceIcon_AssignType));
                                FreeVec(str);
                                SC_DisposeObject(o2);
                        }
                        else
                                printf("Object error\n");
                }
                while (SC_DoMethod(o1,SCCM_Icon_Entry,SCCV_Icon_Entry_Next));

                printf("List end\n\n");

                SC_DoMethod(o3,SCCM_Icon_Entry,SCCV_Icon_Entry_First);
                do
                {
                        if (o2 = (Object *) SC_DoMethod(o3,SCCM_Icon_GetObject,TAG_DONE))
                        {
                                printf("%s\n",get(o2,SCCA_Icon_Name));
                                SC_DisposeObject(o2);
                        }
                        else
                                printf("Object error\n");
                }
                while (SC_DoMethod(o3,SCCM_Icon_Entry,SCCV_Icon_Entry_Next));

                SC_DoMethod(o3,SCCM_Icon_Entry,SCCV_Icon_Entry_None);
                SC_DoMethod(o1,SCCM_Icon_Entry,SCCV_Icon_Entry_None);

                SC_DisposeObject(o3);
                SC_DisposeObject(o1);

                        */
/*********************************************************************************************************************/
                        /*
                Object  *winobject1;
                Object  *rectobj;
                Object  *screenobj;

                rectobj = SC_NewObject(NULL,"RectFill.scalos",TAG_DONE);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);

                winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,rectobj,SCCA_Window_ScreenObj,screenobj,TAG_DONE);
                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject1,TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");

                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
                        */
/*********************************************************************************************************************/
/*
                Object  *winobject1;
                Object  *btmobj1;
                Object  *screenobj;

                btmobj1 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);

                winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,btmobj1,SCCA_Window_ScreenObj,screenobj,TAG_DONE);
                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject1,TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");

                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
*/
/*********************************************************************************************************************/
/*
                Object  *btmobj1,*btmobj2;
                Object  *rectobj;
                Object  *screenobj;
                Object  *winobject1,*winobject2,*winobject3;

                btmobj1 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj2 = SC_NewObject(NULL,"PlanarTest.scalos",TAG_DONE);
                rectobj = SC_NewObject(NULL,"RectFill.scalos",TAG_DONE);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);


                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window,
                                winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,btmobj1,
                                                         SCCA_Window_ScreenObj,screenobj,TAG_DONE),
                        SCCA_WindowTask_Window, winobject3 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,rectobj,
                                                                                                                          SCCA_Window_ScreenObj,screenobj,TAG_DONE),
                        SCCA_WindowTask_Window, winobject2 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,btmobj2,
                                                                                                                          SCCA_Window_ScreenObj,screenobj,TAG_DONE),
                        TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject3,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #3.\n");

                if(!(SC_DoMethod(winobject2,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #2.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");


                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
                SC_DoMethod(winobject2,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
                SC_DoMethod(winobject3,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);

*/
/*********************************************************************************************************************/
/*
                Object  *btmobj1,*btmobj2,*btmobj3,*btmobj4;
                Object  *panelobj;
                Object  *grobj2,*grobj1;
                Object  *screenobj;
                Object  *winobject4;

                btmobj1 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj2 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj3 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj4 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);

                panelobj = SC_NewObject(NULL,SCC_PANELVIEW_NAME,SCCA_PanelView_Panel,
                                                                SC_NewObject(NULL,SCC_ICONPANEL_NAME,TAG_DONE),
                                                                TAG_DONE);

                grobj2   = SC_NewObject(NULL,SCC_GROUP_NAME,
                                                           SCCA_Group_Child,btmobj1,
                                                           SCCA_Group_Child,panelobj,
                                                           SCCA_Group_Child,btmobj2,
                                                           SCCA_Group_Type,SCCV_Group_VGroup,
                                                           TAG_DONE);

                grobj1   = SC_NewObject(NULL,SCC_GROUP_NAME,
                                                                SCCA_Group_Child,btmobj3,
                                                                SCCA_Group_Child,grobj2,
                                                                SCCA_Group_Child,btmobj4,
                                                                SCCA_Group_Type,SCCV_Group_HGroup,
                                                                TAG_DONE
                                                           );

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);


                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject4 = SC_NewObject(NULL,SCC_WINDOW_NAME,
                                                                                                                                                                         SCCA_Window_Root,grobj1,
                                                                                                                                                                         SCCA_Window_ScreenObj,screenobj,
                                                                                                                                                                         TAG_DONE),
                                                   TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject4,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #4.\n");

                SC_DoMethod(winobject4,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
*/
/*********************************************************************************************************************/
/*
                Object  *btmobj1,*btmobj2,*btmobj3,*btmobj4,*btmobj5;
                Object  *rectobj1,*rectobj2,*rectobj3;
                Object  *grobj1,*grobj2,*grobj3;
                Object  *screenobj;
                Object  *winobject1;

                btmobj1 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj2 = SC_NewObject(NULL,"PlanarTest.scalos",TAG_DONE);
                btmobj3 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                btmobj4 = SC_NewObject(NULL,"PlanarTest.scalos",TAG_DONE);
                btmobj5 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                rectobj1 = SC_NewObject(NULL,"RectFill.scalos",TAG_DONE);
                rectobj2 = SC_NewObject(NULL,"RectFill.scalos",TAG_DONE);
                rectobj3 = SC_NewObject(NULL,"RectFill.scalos",TAG_DONE);
                grobj3   = SC_NewObject(NULL,SCC_GROUP_NAME,
                                                                SCCA_Group_Child,rectobj3,
                                                                SCCA_Group_Child,btmobj5,
                                                                SCCA_Group_Child,btmobj2,
                                                                SCCA_Group_Type,SCCV_Group_VGroup,
                                                                TAG_DONE
                                                           );
                grobj2   = SC_NewObject(NULL,SCC_GROUP_NAME,
                                                                SCCA_Group_Child,btmobj3,
                                                                SCCA_Group_Child,btmobj4,
                                                                SCCA_Group_Child,grobj3,
                                                                SCCA_Group_Child,rectobj1,
                                                                SCCA_Group_Type,SCCV_Group_HGroup,
                                                                TAG_DONE
                                                           );
                grobj1   = SC_NewObject(NULL,SCC_GROUP_NAME,
                                                                SCCA_Group_Child,btmobj1,
                                                                SCCA_Group_Child,grobj2,
                                                                SCCA_Group_Child,rectobj2,
                                                                SCCA_Group_Type,SCCV_Group_VGroup,
                                                                TAG_DONE
                                                           );

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);


                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,
                                                   SCCA_WindowTask_Window,winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,
                                                                                                                                                        SCCA_Window_Root,grobj1,
                                                                                                                                                        SCCA_Window_ScreenObj,screenobj,
                                                                                                                                                        TAG_DONE
                                                                                                                                                   ),
                                                   TAG_DONE
                                                  );

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");

                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
*/
/*********************************************************************************************************************/
/*
                Object  *btmobj1;
                Object  *grobj;
                Object  *screenobj;
                Object  *winobject4;

                btmobj1 = SC_NewObject(NULL,"Chunky8Test.scalos",TAG_DONE);
                grobj   = SC_NewObject(NULL,SCC_GROUP_NAME,SCCA_Group_Child,btmobj1,TAG_DONE);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);


                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject4 = SC_NewObject(NULL,SCC_WINDOW_NAME,
                                                                                                                                                                         SCCA_Window_Root,grobj,
                                                                                                                                                                         SCCA_Window_ScreenObj,screenobj,
                                                                                                                                                                         TAG_DONE),
                                                                          TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject4,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #4.\n");

                SC_DoMethod(winobject4,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
*/
/*********************************************************************************************************************/

extern Object *chunky_8_makebitmap(void);

                Object  *winobject1;
                Object  *panelobj;
                Object  *iconpanelobj;
                Object  *screenobj;
                Object  *bitmap;
                Object  *icon;
                Object  *icon2;

                bitmap = chunky_8_makebitmap();

                icon = SC_NewObject(NULL,SCC_DOSDIRICON_NAME,SCCA_Icon_Path,"sys:Prefs",TAG_DONE);
                SC_DoMethod(icon,SCCM_Icon_GetIcon,SCCV_Icon_GetIcon_Disk);

                icon2 = SC_NewObject(NULL,SCC_DOSDIRICON_NAME,SCCA_Icon_Path,"sys:Devs",TAG_DONE);
                SC_DoMethod(icon2,SCCM_Icon_GetIcon,SCCV_Icon_GetIcon_Disk);

                panelobj = SC_NewObject(NULL,SCC_PANELVIEW_NAME,SCCA_PanelView_Panel,
                                                                iconpanelobj = SC_NewObject(NULL,SCC_ICONPANEL_NAME,SCCA_Panel_Background,bitmap,TAG_DONE),
                                                                TAG_DONE);


                SC_DoMethod(iconpanelobj,SCCM_IconPanel_Add,icon);
                SC_DoMethod(iconpanelobj,SCCM_IconPanel_Add,icon2);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);

                winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,panelobj,SCCA_Window_ScreenObj,screenobj,TAG_DONE);
                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject1,TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");

                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);

/*********************************************************************************************************************/
/*
                Object  *winobject1;
                Object  *rectobj;
                Object  *screenobj;

                rectobj = SC_NewObject(NULL,"GFXTest.scalos",TAG_DONE);

                screenobj = SC_NewObject(NULL,SCC_SCREEN_NAME,SCCA_Screen_PubName,"Workbench",TAG_DONE);

                winobject1 = SC_NewObject(NULL,SCC_WINDOW_NAME,SCCA_Window_Root,rectobj,SCCA_Window_ScreenObj,screenobj,TAG_DONE);
                app = SC_NewObject(NULL,SCC_WINDOWTASK_NAME,SCCA_WindowTask_Window, winobject1,TAG_DONE);

                if(!app) fail(app, "Error: Failed to create application.\n");

                if(!(SC_DoMethod(winobject1,SCCM_Window_Open,TRUE)))
                        fail(app, "Error: Failed to open window #1.\n");

                SC_DoMethod(winobject1,SCCM_Notify,SCCA_Window_CloseRequest,TRUE,app,1,SCCM_WindowTask_Return);
*/
/*********************************************************************************************************************/


                while (SC_DoMethod(app, SCCM_WindowTask_Input, &signal) == FALSE)
                {
                        signal = Wait(signal);
                }

                SC_DisposeObject( app );

                SC_DisposeObject(screenobj);



                RemClasses();
                ScalosMasterExit();
                CloseLibs();
        }
}
        
/* -------------------------- Inits ------------------------------------*/

