

#ifndef _MYSTIC_GLOBAL_H
#include "mystic_global.h"
#endif

//struct ExecBase	*SysBase;
#ifdef __AROS__
struct UtilityBase *UtilityBase;
#else
struct Library *UtilityBase;
#endif
struct GfxBase *GfxBase;
struct Library *GuiGFXBase;
struct Library *CyberGfxBase;
struct Library *DataTypesBase;
struct DosLibrary *DOSBase;
//struct Library *MathTransBase;
struct Library *GadToolsBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;




