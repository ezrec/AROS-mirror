#define FRAMES (4)

#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/execbase.h>
#ifndef USE_ZUNE
#include <libraries/mui.h>
#else
#include <mui.h>
#endif
#include <math.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BoingTransferClData
{
  APTR window;
  BOOL LeftDirection,TopDirection,AutoMove;
  double Radius;
  long Position,RedPen,WhitePen,HorizPos,VertPos;
  PLANEPTR Bitplane;
  struct AreaInfo AInfo;
  struct BitMap *BitMap[FRAMES],*Mask,*Back,*Temp;
  struct RastPort RP[FRAMES],MaskRP,TempRP,BackRP;
  struct TmpRas TmpRaster;
  struct Window *win;
  void *Buffer[50];
  WORD BitMapSize;
  struct Process *AutoMoveProc;
  struct MsgPort *ReplyPort;
};

#define MUIM_BoingTransfer_Move     0x80420000
#define MUIM_BoingTransfer_Reset    0x80420001
#define MUIA_BoingTransfer_AutoMove 0x80420002
