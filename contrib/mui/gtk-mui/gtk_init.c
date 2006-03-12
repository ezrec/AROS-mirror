/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id$
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifndef __MORPHOS__
#include <clib/alib_protos.h>
#endif
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
/* I need exactly one place, where proto/muimaster.h is included..? */
#include <proto/muimaster.h>
#include <mui.h>
/* cross platform hooks */
#include <SDI_hook.h>

#include "classes.h"
#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

void Close_Libs(void);

BOOL Open_Libs(void )
{
  if ((MUIMasterBase=(struct Library *)       OpenLibrary(MUIMASTER_NAME,19)) )
#ifndef __AMIGAOS4__
  if ((IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) )
  if ((GfxBase=      (struct GfxBase *)       OpenLibrary("graphics.library",0)) )
  if ((UtilityBase=  (struct UtilityBase *)   OpenLibrary("utility.library", 0)))
#else
  if ((GadToolsBase=                          OpenLibrary("gadtools.library" ,39)) )
  if ((IMUIMaster=   (struct Interface *)     GetInterface(MUIMasterBase,"main",1,NULL)) )
  if ((IGadTools=    (struct GadToolsIFace *) GetInterface(GadToolsBase, "main", 1, NULL)) )
#endif
  if (mgtk_create_rootgroup_class())
  if (mgtk_create_table_class())
  if (mgtk_create_spinbutton_class()) 
  if (mgtk_create_notebook_class()) 
  if (mgtk_create_progressbar_class()) 
  if (mgtk_create_timer_class()) 
  if (mgtk_create_fixed_class()) 
  if (mgtk_create_checkbutton_class()) 
  if (mgtk_create_togglebutton_class()) 
  if (mgtk_create_radiobutton_class()) 
  if (mgtk_create_button_class()) 
  if (mgtk_create_menu_class()) 
  if (mgtk_create_entry_class()) 
  if (mgtk_create_toolbar_class()) 
  if (mgtk_create_custom_class()) 
  {
    return (1);
  }

  Close_Libs();
  return(0);
}

void Close_Libs(void)
{
  mgtk_delete_table_class();
  mgtk_delete_spinbutton_class();
  mgtk_delete_notebook_class();
  mgtk_delete_progressbar_class();
  mgtk_delete_timer_class();
  mgtk_delete_fixed_class();
  mgtk_delete_checkbutton_class();
  mgtk_delete_togglebutton_class();
  mgtk_delete_radiobutton_class();
  mgtk_delete_button_class();
  mgtk_delete_menu_class();
  mgtk_delete_entry_class();
  mgtk_delete_toolbar_class();
  mgtk_delete_custom_class();
  mgtk_delete_rootgroup_class();

#ifndef __AMIGAOS4__
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
  CloseLibrary((struct Library *)UtilityBase);
#else
  if (IMUIMaster)   DropInterface((struct Interface *)IMUIMaster);
  if (GadToolsBase) CloseLibrary(GadToolsBase);
  if (IGadTools)    DropInterface(IGadTools);
#endif
  CloseLibrary(MUIMasterBase);
}

/*********************************
 * init_fundamentals
 *********************************/
static int init_fundamentals() {
  mgtk->default_style=mgtk_get_default_style(mgtk->MuiRoot);

  if (mgtk->default_style)
  {
    mgtk_g_type_register_fundamental(IS_GROUP,"hbox",&mgtk_hbox_new_noparams);
    mgtk_g_type_register_fundamental(IS_CUSTOM,"mgtkcustom",&mgtk_custom_new);

    g_signal_new("toggled", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("clicked", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("pressed", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("released", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("destroy", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("changed", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("value_changed", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("realized", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("activate", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("child-detached", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    g_signal_new("child-attached", 0, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL); 
    return 1;
  }
  return 0;
}
/*********************************
 * gtk_init
 *
 * create MuiApp and init mgtk
 *
 *********************************/
void gtk_init(int *argc, char ***argv)
{
  char *tname;
  char *i;
  char *name;

#if defined ENABLE_RT
  RT_Init();
#endif

  if ( ! Open_Libs() )
  {
    printf("GTK: fatal error: Cannot open libs\n");
    exit(1);
  }

  mgtk=g_new(struct mgtk_ObjApp,1);

  if(!mgtk) {
    exit(1);
  }

  /* get program name */
  tname=*argv[0];
  i=*argv[0];

  while(i[0]) {
    if((i[0]=='/') || (i[0]==':')) {
      tname=i+1;
    }
    i++;
  }

  /* TODO: This could (should) be handled in the application class subclass  -itix
   */

  if(tname[0]) {
    name=g_strdup_printf("GTK %s",tname);
  }
  else {
    name=g_strdup("GTK Application");
  }

  if (!init_fundamentals())
  {
    printf("GTK: out of memory\n");
    exit(1);
  }

  mgtk->MuiApp = (APTR) ApplicationObject,
    MUIA_Application_Title  , name,
    MUIA_Application_Version ,   "$VER: 0.0.1 (6/13/05)",
    MUIA_Application_Copyright , "GTK-MUI: LGPL",
    MUIA_Application_Author  ,   "GTK by Oliver Brunner",
    MUIA_Application_Description,"A program built with GTK wrapper",
#ifndef __AROS__
    MUIA_Application_Base  ,     "GTK wrapper",
#endif
    SubWindow, mgtk->MuiRoot = (APTR) WindowObject,
      MUIA_Window_Menustrip, mgtk->MuiRootStrip = (APTR) MenustripObject,
       	MUIA_Menustrip_Enabled, (LONG) FALSE,
      End,
      MUIA_Window_Title, "First empty Window",
#ifndef __AROS__
      MUIA_Window_ID , MAKE_ID('M','G','T','K'),
#endif
      WindowContents, mgtk->Group = (APTR) NewObject(CL_RootGroup->mcc_Class, NULL,TAG_DONE),
    End,
  End;

  g_free(name);

  /* MuiApp fails if MuiRootStrip failed */

  if (!mgtk->MuiApp) {
    printf("gtk_window_new: Cannot create application. (%ld,%ld)\n",(long int) mgtk->MuiApp,(long int) mgtk->MuiRootStrip);
    exit(1);
  }

  DoMethod(mgtk->MuiRoot,MUIA_Window_Width,150);
  DoMethod(mgtk->MuiRoot,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    mgtk->MuiApp,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  DebOut("mgtk->MuiApp: %lx\n",mgtk->MuiApp);
  DebOut("mgtk->MuiRoot: %lx\n",mgtk->MuiRoot);
  DebOut("mgtk->MuiRootStrip: %lx\n",mgtk->MuiRootStrip);
  DebOut("mgtk->MuiGroup: %lx\n",mgtk->Group);

/*
  mgtk->black_pen=ObtainBestPenA( ViewAddress()->ViewPort->ColorMap, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, NULL ); 
  */

  #if USE_PENS
  mgtk->white_black_optained=0;
  if ((mgtk->screen = LockPubScreen(0))) {
    if ((mgtk->dri = GetScreenDrawInfo(mgtk->screen))) {
      if ((mgtk->visualinfo = GetVisualInfoA(mgtk->screen, NULL))) {
        mgtk->white_pen=ObtainBestPen( mgtk->screen->ViewPort.ColorMap, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, OBP_FailIfBad, FALSE, TAG_DONE ); 
        mgtk->black_pen=ObtainBestPen( mgtk->screen->ViewPort.ColorMap, 0, 0, 0, OBP_FailIfBad, FALSE, TAG_DONE ); 
        mgtk->white_black_optained=1;
      }
      else {
        DebOut("WARNING: Init: Unable to GetVisualInfoA\n");
      }
    }
    else {
        DebOut("WARNING: Init: Unable to GetScreenDrawInfo\n");
    }
  }
  else {
    DebOut("WARNING: Init: Unable to LockPubScreen\n");
  }

  /* if anything above failed */
  if(!mgtk->white_black_optained) {
    mgtk->white_pen=MGTK_PEN_WHITE;
    mgtk->black_pen=MGTK_PEN_BLACK; /* naja.. */
  }
  #endif

  return;
}

