#ifndef SCRNIO_H
#define SCRNIO_H 1

#ifndef MYTYPES_H_FILE
#include "mytypes.h"
#endif
extern void InitWindow();
extern void CloseDisplay();
extern void SetMono();
extern void ClearWindow();
extern void DrawPoly();

extern short DitherPower, DitherLevels;
extern int WinHOrig, WinVOrig;
extern int XOR, WRITE;
extern short DitherMask;

extern void FreeOldDither();
extern bool AllocDither();

#define CntrX(XVAL) ((XVAL) - WinHOrig)
#define CntrY(YVAL) (WinVOrig - (YVAL)) /* fudge for the mouse */

#define UCntrX(XVAL) ((XVAL) + WinHOrig)
#define UCntrY(YVAL) (WinVOrig - (YVAL))

#endif /* SCRNIO_H */
