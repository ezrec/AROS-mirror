

#ifndef _MYSTIC_GLOBAL_H
#include "mystic_global.h"
#endif

/* On AROS use autoopening of libraries */
#ifndef __AROS__
//struct ExecBase	*SysBase;
struct Library *UtilityBase;
struct GfxBase *GfxBase;
struct Library *GuiGFXBase;
struct Library *CyberGfxBase;
struct Library *DataTypesBase;
struct DosLibrary *DOSBase;
//struct Library *MathTransBase;
struct Library *GadToolsBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;
#endif




