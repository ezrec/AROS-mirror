


#ifndef _MYSTIC_DATA_H
#define _MYSTIC_DATA_H 1

//extern struct ExecBase		*SysBase;
#ifdef __AROS__
extern struct UtilityBase	*UtilityBase;
#else
extern struct Library		*UtilityBase;
#endif
extern struct GfxBase		*GfxBase;
extern struct Library		*GuiGFXBase;
extern struct Library		*CyberGfxBase;
extern struct Library		*DataTypesBase;
extern struct DosLibrary	*DOSBase;
//extern struct Library		*MathTransBase;
extern struct IntuitionBase	*IntuitionBase;
extern struct Library		*GadToolsBase;
extern struct Library		*LayersBase;


#endif
