/****************************************************************************
*									    *
* Init.h							     970319 *
*									    *
* Open Libs and Screen							    *
*									    *
****************************************************************************/

#ifndef INIT_H
#define INIT_H

#include "MyMath.h"
#include "Moveing.h"

#define MaxWidth  2048
#define MaxHeight 2048

extern struct RastPort * MyRastPort,* RastPort1,* RastPort2,* InfoRastPort,
		       * MyTmpRast,TmpInfoRast,Tmp1Rast,Tmp2Rast;
extern struct GfxBase  * GfxBase;
extern uint8	       * ColMap;

extern fix ColLight;

extern int16 InitLibs(void);
extern int16 InitScreen(void);
extern void  CleanupScreen(void);
extern void  CleanupLibs(void);

#endif

