#ifndef _GUIGFX_DATA_H
#define _GUIGFX_DATA_H 1

//extern struct ExecBase		*SysBase;
#ifdef __AROS__
extern struct UtilityBase	*UtilityBase;
#else
extern struct Library		*UtilityBase;
#endif
extern struct GfxBase		*GfxBase;
extern struct Library		*RenderBase;
extern struct Library		*CyberGfxBase;
extern struct Library		*DataTypesBase;
extern struct DosLibrary	*DOSBase;
//extern struct Library		*MathTransBase;
//extern struct Library 		*TowerBase;
extern struct IntuitionBase	*IntuitionBase;
extern struct Library           *aroscbase;

extern APTR MemHandler;

extern BOOL gfx40;
extern BOOL cgfx41;

extern BOOL env_usescalepixelarray;
extern BOOL env_usewpa8;
extern int env_autoditherthreshold;

extern ULONG defaultpalette[256];

#endif
