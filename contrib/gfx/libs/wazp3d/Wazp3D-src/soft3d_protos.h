#ifndef __CLIB_SOFT3D_PROTOS_H
#define __CLIB_SOFT3D_PROTOS_H

UBYTE SOFT3D_DoUpdate(APTR sc);
void  SOFT3D_End(APTR sc);
void  SOFT3D_ClearZBuffer(APTR sc,float fz);
void  SOFT3D_FreeTexture(APTR sc,APTR st);
void  SOFT3D_Flush(APTR sc);
void  SOFT3D_SetBackColor(APTR sc,APTR RGBA);
void  SOFT3D_SetCulling(APTR sc,UBYTE CullingMode);
void  SOFT3D_SetCurrentColor(APTR sc,APTR RGBA);
void  SOFT3D_SetEnvColor(APTR sc,APTR RGBA);
void  SOFT3D_SetPointSize(APTR sc,UWORD PointSize);
APTR  SOFT3D_AllocZbuffer(APTR sc,UWORD large,UWORD high);
APTR  SOFT3D_Start(APTR PrefsWazp3D);
void  SOFT3D_AllocImageBuffer(APTR sc,UWORD large,UWORD high);
void  SOFT3D_DrawPrimitive(APTR sc,APTR p,ULONG Pnb,ULONG primitive);
void  SOFT3D_ReadZSpan(APTR sc,UWORD x,UWORD y,ULONG n,APTR dz);
void  SOFT3D_ClearImageBuffer(APTR sc,UWORD x,UWORD y,UWORD large,UWORD high);
APTR  SOFT3D_CreateTexture(APTR sc,APTR pt,UWORD large,UWORD high,UWORD bits,UBYTE TexMode);
void  SOFT3D_Fog(APTR sc,UBYTE FogMode,float FogZmin,float FogZmax,float FogDensity,APTR FogRGBA);
void  SOFT3D_WriteZSpan(APTR sc,UWORD x,UWORD y,ULONG n,APTR dz,APTR mask);
void  SOFT3D_SetClipping(APTR sc,UWORD xmin,UWORD xmax,UWORD ymin,UWORD ymax);
void  SOFT3D_SetBitmap(APTR sc,APTR bm,APTR bmdata,ULONG bmformat,UWORD x,UWORD y,UWORD large,UWORD high);
void  SOFT3D_SetDrawStates(APTR sc,APTR st,UBYTE ZMode,UBYTE BlendMode,UBYTE TexEnvMode,UBYTE UseGouraud,UBYTE UseFog,UBYTE PerspMode);
void  SOFT3D_UpdateTexture(APTR sc,APTR st,APTR pt);	
#endif


