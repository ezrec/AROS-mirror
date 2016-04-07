#include <stdio.h>
#include <stdlib.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include <libraries/mui.h>
#undef SysBase
#undef GfxBase
#undef UtilityBase
#undef IntuitionBase

#include "libs.h"

struct IntuitionBase *IntuitionBase=NULL;
struct GfxBase  *GfxBase=NULL;
struct Library  *MUIMasterBase=NULL;
struct Library  *UtilityBase=NULL;

#ifndef __AROS__

/* On AmigaOS we build a fake library base, because it's not compiled as sharedlibrary yet */
#include "muimaster_intern.h"

int openmuimaster(void)
{
    static struct MUIMasterBase_intern MUIMasterBase_instance;
    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.dosbase = (void*)OpenLibrary("dos.library",37);
    MUIMasterBase_instance.utilitybase = (void*)OpenLibrary("utility.library",37);
    MUIMasterBase_instance.aslbase = OpenLibrary("asl.library",37);
    MUIMasterBase_instance.gfxbase = (void*)OpenLibrary("graphics.library",37);
    MUIMasterBase_instance.layersbase = OpenLibrary("layers.library",37);
    MUIMasterBase_instance.intuibase = (void*)OpenLibrary("intuition.library",37);
    MUIMasterBase_instance.cxbase = OpenLibrary("commodities.library",37);
    MUIMasterBase_instance.keymapbase = OpenLibrary("keymap.library",37);
    MUIMasterBase_instance.gadtoolsbase = OpenLibrary("gadtools.library",37);
    __zune_prefs_init(&__zprefs);

    return 1;
}

void closemuimaster(void)
{
}

#undef SysBase
#undef IntuitionBase
#undef GfxBase
#undef LayersBase
#undef UtilityBase

#else

int openmuimaster(void)
{
    if ((MUIMasterBase = OpenLibrary("muimaster.library", 0))) return 1;
    return 0;
}

void closemuimaster(void)
{
    if (MUIMasterBase) CloseLibrary(MUIMasterBase);
}

#endif



int Ouvrir_Libs(void )
{
  if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary((UBYTE *)"intuition.library",39)) )
    return(0);

  if ( !(GfxBase=(struct GfxBase *) OpenLibrary((UBYTE *)"graphics.library",0)) )
  {
    Fermer_Libs();
    return(0);
  }


  if ( !openmuimaster())
  {
    Fermer_Libs();
    return(0);
  }

  if ( !(UtilityBase=OpenLibrary("utility.library",0)) )
  {
    Fermer_Libs();
    return(0);
  }


  return(1);
}

void Fermer_Libs(void )
{
  if ( IntuitionBase )
    CloseLibrary((struct Library *)IntuitionBase);

  if ( GfxBase )
    CloseLibrary((struct Library *)GfxBase);

  closemuimaster();

  if ( UtilityBase )
    CloseLibrary((struct Library *)UtilityBase);

}


