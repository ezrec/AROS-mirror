#include <cybergraphx/cybergraphics.h>        
#ifdef __AROS__
#include <proto/cybergraphics.h>
#else
#include <inline/cybergraphics.h>        
#endif

/* FIXME: do we really need this file for AROS? */

/* stack functions calls only for GCC */
   
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat )        
{(void)WritePixelArray(image,SrcX,SrcY,SrcMod,RastPort,DestX,DestY,SizeX,SizeY,SrcFormat);} 

ULONG STACKReadPixelArray(APTR  destRect, UWORD  destX,UWORD  destY, UWORD  destMod, struct RastPort *  RastPort, UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat )
{return(ReadPixelArray(destRect,destX,destY,destMod,RastPort,SrcX,SrcY,SizeX,SizeY,DestFormat));}
