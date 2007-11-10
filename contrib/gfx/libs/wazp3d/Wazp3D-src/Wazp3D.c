/* Wazp3D beta 33 : Alain THELLIER - Paris - FRANCE - (November 2006 to October 2007) */
/* Adaptation to AROS from Matthias Rustler                                           */
/* LICENSE: GNU General Public License (GNU GPL) for this file                        */

#define WAZP3DDEBUG 1
#define MOTOROLAORDER 1

#if defined(__AROS__) && (AROS_BIG_ENDIAN == 0)
#undef MOTOROLAORDER
#endif

/*==================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <exec/types.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/dos.h>
#ifndef __AROS__
#include <proto/timer.h>
#endif
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>

#include <clib/Warp3D_protos.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#if !defined(__PPC__ ) && !(defined __AROS__)
#include <inline/cybergraphics.h>
#endif

#include <libraries/asl.h>
/*==================================================================================*/
struct memory3D{
	void *pt;
	LONG size;
	UBYTE name[40];
	void *nextME;
};
/*==================================================================================*/
#define MAXPNB 10000
LONG tempf;					/* Used to emulate printf("%f") */
extern BOOL LibDebug;			/* Enable Library Debugger	  */
struct memory3D *firstME=NULL;	/* Tracked memory-allocation	  */
/*==================================================================================*/
struct point3D{
	float x,y,z;
	float u,v;
	UBYTE RGBA[4];
};
/*==================================================================================*/
struct vertex3D{
	float x,y,z;
};
/*==================================================================================*/
struct MyButton{
	BOOL ON;
	UBYTE name[30];
	};
/*==================================================================================*/
struct WAZP3D_parameters{
UBYTE FunctionName[89][50];
ULONG FunctionCalls[89];
ULONG FunctionCallsAll;
ULONG CrashFunctionCall;	/* step 3Dprog after this call */
UBYTE DriverName[256];
ULONG drivertype;
W3D_Driver	driver;
W3D_Driver *DriverList[2];
W3D_ScreenMode *smode;
W3D_ScreenMode smodelist[50];
BOOL  ASLsize;
ULONG ASLminX,ASLmaxX,ASLminY,ASLmaxY;
struct MyButton HardwareLie;		/* pretend to be a perfect hardware driver */
struct MyButton OnlyRGB24;		/* only use fast RGB 24 screens */
struct MyButton UsePolyHack;
struct MyButton UseColorHack;
struct MyButton UseCullingHack;
struct MyButton UseFog;
struct MyButton UseClearDrawRegion;
struct MyButton UseMinUpdate;
struct MyButton UseAnyTexFmt;
struct MyButton HackRGBA1;
struct MyButton HackRGBA2;
struct MyButton UseAntiScreen;
struct MyButton UseClipper;
struct MyButton UsePerspective;

struct MyButton DebugWazp3D;		/* print debug on/off */
struct MyButton DebugFunction;	/* print debug what*/
struct MyButton StepFunction;
struct MyButton DebugCalls;
struct MyButton DebugState;
struct MyButton DebugPoint;
struct MyButton DebugVar;
struct MyButton DebugVal;
struct MyButton DebugDoc;
struct MyButton DebugDriver;
struct MyButton DebugContext;
struct MyButton DebugTexture;
struct MyButton DebugError;
struct MyButton DebugWC;
struct MyButton DebugWT;
struct MyButton DebugSOFT3D;
struct MyButton StepSOFT3D;
struct MyButton DebugSC;
struct MyButton DebugST;
struct MyButton DebugClipper;
struct MyButton StepCopyImage;
struct MyButton DebugMemList;
struct MyButton DebugMemUsage;
} Wazp3D;
/*==================================================================================*/
struct WAZP3D_context{
W3D_Driver *Driver;
struct WAZP3D_texture *WT;		/* binded one */
float uresize,vresize;			/* for binded tex */
struct WAZP3D_texture *firstWT;	/* list of all textures */

BOOL CallFlushFrame;	/* then CopyImageToDrawRegion inside bitmap*/
BOOL CallSetDrawRegion;
BOOL CallClearZBuffer;

struct point3D P[MAXPNB];
LONG MaxPnb;

UBYTE hints[32];

struct RastPort rastport;
ULONG ModeID;

UBYTE *Image8;
UWORD large,high;
WORD Xmin,Ymin,Xmax,Ymax;					/* screen scissor */
UBYTE *V;
ULONG Vformat;
ULONG Vstride;

UBYTE *UV;
ULONG UVformat;
ULONG UVstride;
ULONG UVoff_v;

UBYTE *RGBA;
ULONG RGBAformat;
ULONG RGBAstride;
ULONG RGBAorder;

ULONG primitive;
ULONG Pnb;
UWORD DrawMode;
float PointSize;

ULONG AlphaMode,AlphaRef;
ULONG SrcFunc,DstFunc;
ULONG LogicOp;
ULONG PenMask;

UWORD FogMode;
float FogZmin,FogZmax,FogDensity;
UBYTE FogRGBA[4];

ULONG	StencilFunc,StencilRefValue,StencilMask,StencilDpfail,StencilDppass,StencilWriteMask;
ULONG ZbufferMode;

UBYTE CurrentRGBA[4];
UBYTE	MaskRGBA[4];

W3D_Context context;
void *SC;
};
/*==================================================================================*/
struct WAZP3D_texture{
UBYTE *pt;
UWORD large,high,bits;
UWORD DrawMode;
UBYTE   WrapRGBA[4];
UBYTE TexEnvRGBA[4];
UBYTE ChromaTestMinRGBA[4];
UBYTE ChromaTestMaxRGBA[4];
ULONG ChromaTestMode;
ULONG MinFilter,MagFilter,TexEnvMode;
BOOL  MinFiltering,MagFiltering,MinMimapping;
BOOL  WrapRepeatX,WrapRepeatY;
void *nextWT;		/* next WAZP3D_texture */
void *ST;			/* link to this SOFT3D_texture*/
W3D_Texture	texture;	/* include this Warp3D texture */
};
#define ZMAX (16*1024)
#define MAXPI 500
#define ALPHAMIN 2
#define ALPHAMAX (255-ALPHAMIN)
#define BUFFERSIZE 50000
#define BUFFERMAXI  (BUFFERSIZE-1024)
#define alpha 3
/*=============================================================*/
union pixel3D {
struct pixel3DL{
	LONG z;
	LONG u;
	LONG v;
	LONG R;
	LONG G;
	LONG B;
	LONG A;
	LONG x;
	LONG y;
	WORD large;
	ULONG *Image32Y;
	WORD  *Zbuffer16Y;
	}  L;

#ifdef MOTOROLAORDER
struct pixel3DW{

	WORD  z,zlow;
	UBYTE u1,u,u3,u4;
	UBYTE v1,v,v3,v4;
	UBYTE R1,R,R3,R4;
	UBYTE G1,G,G3,G4;
	UBYTE B1,B,B3,B4;
	UBYTE A1,A,A3,A4;
	WORD  x,xlow;
	WORD  y,ylow;
	WORD large;
	ULONG *Image32Y;
	WORD  *Zbuffer16Y;
	}  W;
#else
struct pixel3DW{

	WORD  zlow,z;
	UBYTE u4,u3,u,u1;
	UBYTE v4,v3,v,v1;
	UBYTE R4,R3,R,R1;
	UBYTE G4,G3,G,G1;
	UBYTE B4,B3,B,B1;
	UBYTE A4,A3,A,A1;
	WORD  xlow,x;
	WORD  ylow,y;
	WORD large;
	ULONG *Image32Y;
	WORD  *Zbuffer16Y;
	}  W;
#endif
};
/*=============================================================*/
#ifdef MOTOROLAORDER
union blend3D {
struct blend3DW{
	ULONG Index;
	}  L;
struct blend3DB{
	UBYTE empty1,empty2,Alpha,Color;
	}  B;
};
#else
union blend3D {
struct blend3DW{
	ULONG Index;
	}  L;
struct blend3DB{
	UBYTE Color,Alpha,empty3,empty4;
	}  B;
};
#endif
/*=============================================================*/
struct buffer3D{
	UBYTE *Tex8;
	UBYTE RGBA[4];
	UBYTE *Fog8;
	UBYTE *Image8;
};
/*=============================================================*/
struct newbuffer3D{
	UBYTE *Image;
	UBYTE ImageRGBA[4];
	UBYTE ColorRGBA[4];
	UBYTE   TexRGBA[4];
	UBYTE   FogRGBA[4];
	WORD *Zbuffer;
	WORD z;
	WORD *Stencil;
	WORD s;
	LONG padding;
};
/* A=&Pixel->Tex; B=&Pixel->Image; C=&Pixel->DestRGBA; */
/* tex->A		color+A->A		 fog+A->A		 image+A->Image */
/*=============================================================*/
typedef const void * (*HOOKEDFUNCTION)(void *SC);
#define NODRAW  0
#define TEX24   1
#define TEX32   2
#define COLOR24 3
#define COLOR32 4
#define TEX24ZOFF  5
#define COLOR24ZOFF 6
#define COLOR32ZOFF 7
#define FLAT24 8
/*=============================================================*/
struct SOFT3D_drawmode{
	UBYTE ZbufferTest;
	UBYTE ZbufferUpdate;
	UBYTE Textured;
	UBYTE TextureBlended;
	UBYTE Color;
	UBYTE ColorBlended;
	UBYTE Fog;
	HOOKEDFUNCTION FillFunction;
	HOOKEDFUNCTION PixelsFunction;
};
/*=============================================================*/
struct SOFT3D_context{
	ULONG Fog32[1024];
	UBYTE Blending8[256*256];
	WORD PolyHigh;
	WORD DrawMode;
	struct SOFT3D_drawmode DrawModes[10];
	BOOL FogEnabled;
	WORD large,high;
	struct vertex3D ClipMin;			/* 3D clipper */
	struct vertex3D ClipMax;
	ULONG *Image32;
	WORD  *Zbuffer16;
	UBYTE RGBA[4];
	UBYTE FlatRGBA[4];
	struct SOFT3D_texture *ST;
	WORD Pnb;
	struct point3D *P0;
	union pixel3D *Pix;
	union pixel3D *Xdelta;
	union pixel3D  Ydelta;
	WORD PointSize;
	WORD PointLarge[1024];
	union pixel3D side1[1024];
	union pixel3D side2[1024];
	union pixel3D delta[1024];
	HOOKEDFUNCTION FillFunction;
	HOOKEDFUNCTION PixelsFunction;
	union pixel3D PIX[MAXPI];
	struct buffer3D PixBuffer[BUFFERSIZE];
	struct buffer3D *PixBufferDone;
	struct buffer3D *PixBufferMaxi;
	UWORD Culling;
	UWORD FogMode;
	float FogZmin,FogZmax,FogDensity;
	UBYTE FogRGBA[4];
	WORD PolyPnb;
	union pixel3D PolyPIX[100];
	struct point3D PolyP[100];
	struct point3D T1[100];
	struct point3D T2[100];
	void *firstST;
	UBYTE lines[4*1024*2];
	WORD Pxmin,Pxmax,Pymin,Pymax;			/* really updated region */
	WORD xUpdate,largeUpdate,yUpdate,highUpdate;	/* really updated region previous frame*/
};
/*=================================================================*/
struct SOFT3D_texture{
	UBYTE *pt;
	UBYTE *pt2;
	UWORD large,high,bits,DrawMode;
	UBYTE mipmapped,TexMode;
	void *nextST;
	LONG TindexX[256];
	LONG TindexY[256];
};
/*==================================================================================*/
#define STEP(message) Libprintf(#message"\n");
#define REMP if(Wazp3D.DebugSOFT3D.ON) Libprintf
#define REM(message) REMP(#message"\n");
#define et &&
#define ou ||
#define AND &
#define OR |
#define MLOOP(nbre) for(m=0;m<nbre;m++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define XLOOP(nbre) for(x=0;x<nbre;x++)
#define YLOOP(nbre) for(y=0;y<nbre;y++)
#define LOOP(nbre) while(0<nbre--)
#define SFUNCTION(tex) if(Wazp3D.StepSOFT3D.ON) LibAlert(#tex); if(Wazp3D.DebugSOFT3D.ON) Libprintf(#tex "\n");
#define  WINFO(var,val,doc) if(var == val) if(Wazp3D.DebugVal.ON){Libprintf(" "); if(Wazp3D.DebugVar.ON) Libprintf(#var "="); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define  SINFO(var,val) if(var == val) if(Wazp3D.DebugSOFT3D.ON){Libprintf(" " #val "\n");}
#define WINFOB(var,val,doc) if(Wazp3D.DebugVal.ON) if((var&val)!=0) {Libprintf(" "); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   VAR(var) if(Wazp3D.DebugVal.ON)  {Libprintf(" "); if(Wazp3D.DebugVar.ON) Libprintf(#var "="); Libprintf("%ld\n",var);}
#define  VARF(var) if(Wazp3D.DebugVal.ON)  {Libprintf(" "); if(Wazp3D.DebugVar.ON) Libprintf(#var "="); tempf=(LONG)(100.0*var); Libprintf("(x100)= %ld\n",tempf);}
#define   VAL(var) {if(Wazp3D.DebugVal.ON)Libprintf(" [%ld]",var);}
#define SWAP(x,y) {temp=x;x=y;y=temp;}
#define COPYRGBA(x,y)	*((ULONG *)(x))=*((ULONG *)(y));
#define ZZ LibAlert("ZZ stepping ...");
#define  ERROR(val,doc) if(error == val) {Libprintf(" "); if(Wazp3D.DebugVar.ON) Libprintf("Error="); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WRETURN(error) return(PrintError(error));
/*==================================================================================*/
#ifdef __AROS__
AROS_UFP3(BOOL, ScreenModeFilter,
AROS_UFHA(struct Hook *, h      , A0),
AROS_UFHA(APTR         , object , A2),
AROS_UFHA(APTR         , message, A1));
#else
BOOL ScreenModeFilter(struct Hook* hook __asm("a0"), APTR object __asm("a2"),APTR message __asm("a1"));
#endif
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,ULONG *Image);
UBYTE *Libstrcat(UBYTE *s1,UBYTE *s2);
UBYTE *Libstrcpy(UBYTE *s1,UBYTE *s2);
ULONG Libstrlen(UBYTE *string);
ULONG STACKReadPixelArray(APTR  destRect,UWORD  destX,UWORD  destY,UWORD  destMod,struct RastPort *  RastPort,UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat );	;
ULONG W3D_AllocStencilBuffer(W3D_Context *context);
ULONG W3D_AllocZBuffer(W3D_Context *context);
ULONG W3D_BindTexture(W3D_Context* context, ULONG tmu, W3D_Texture *texture);
ULONG W3D_CheckDriver(void);
ULONG W3D_CheckIdle(W3D_Context *context);
ULONG W3D_ClearDrawRegion(W3D_Context *context, ULONG color);
ULONG W3D_ClearStencilBuffer(W3D_Context *context, ULONG *clearvalue);
ULONG W3D_ClearZBuffer(W3D_Context *context, W3D_Double *clearvalue);
ULONG W3D_ColorPointer(W3D_Context* context, void *pointer, int stride,ULONG format, ULONG mode, ULONG flags);
ULONG W3D_DrawArray(W3D_Context* context, ULONG primitive, ULONG base, ULONG count);
ULONG W3D_DrawElements(W3D_Context* context, ULONG primitive, ULONG type, ULONG count,void *indices);
ULONG W3D_DrawLine(W3D_Context *context, W3D_Line *line);
ULONG W3D_DrawLineLoop(W3D_Context *context, W3D_Lines *lines);
ULONG W3D_DrawLineStrip(W3D_Context *context, W3D_Lines *lines);
ULONG W3D_DrawPoint(W3D_Context *context, W3D_Point *point);
ULONG W3D_DrawTriangle(W3D_Context *context, W3D_Triangle *triangle);
ULONG W3D_DrawTriangleV(W3D_Context *context, W3D_TriangleV *triangle);
ULONG W3D_DrawTriFan(W3D_Context *context, W3D_Triangles *triangles);
ULONG W3D_DrawTriFanV(W3D_Context *context, W3D_TrianglesV *triangles);
ULONG W3D_DrawTriStrip(W3D_Context *context, W3D_Triangles *triangles);
ULONG W3D_DrawTriStripV(W3D_Context *context, W3D_TrianglesV *triangles);
ULONG W3D_FillStencilBuffer(W3D_Context *context, ULONG x, ULONG y,ULONG width, ULONG height, ULONG depth,void *data);
ULONG W3D_Flush(W3D_Context *context);
ULONG W3D_FreeAllTexObj(W3D_Context *context);
ULONG W3D_FreeStencilBuffer(W3D_Context *context);
ULONG W3D_FreeZBuffer(W3D_Context *context);
ULONG W3D_GetDestFmt(void);
ULONG W3D_GetDriverState(W3D_Context *context);
ULONG W3D_GetDriverTexFmtInfo(W3D_Driver* driver, ULONG texfmt, ULONG destfmt);
ULONG W3D_GetState(W3D_Context *context, ULONG state);
ULONG W3D_GetTexFmtInfo(W3D_Context *context, ULONG texfmt, ULONG destfmt);
ULONG W3D_Hint(W3D_Context *context, ULONG mode, ULONG quality);
ULONG W3D_LockHardware(W3D_Context *context);
ULONG W3D_Query(W3D_Context *context, ULONG query, ULONG destfmt);
ULONG W3D_QueryDriver(W3D_Driver* driver, ULONG query, ULONG destfmt);
ULONG W3D_ReadStencilPixel(W3D_Context *context, ULONG x, ULONG y,ULONG *st);
ULONG W3D_ReadStencilSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, ULONG *st);
ULONG W3D_ReadZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z);
ULONG W3D_ReadZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z);
ULONG W3D_RequestMode(struct TagItem *taglist);
ULONG W3D_SetAlphaMode(W3D_Context *context, ULONG mode, W3D_Float *refval);
ULONG W3D_SetBlendMode(W3D_Context *context, ULONG srcfunc, ULONG dstfunc);
ULONG W3D_SetChromaTestBounds(W3D_Context *context, W3D_Texture *texture,ULONG rgba_lower, ULONG rgba_upper, ULONG mode);
ULONG W3D_SetColorMask(W3D_Context *context, W3D_Bool Red, W3D_Bool Green,W3D_Bool Blue, W3D_Bool Alpha);
ULONG W3D_SetCurrentColor(W3D_Context *context, W3D_Color *color);
ULONG W3D_SetCurrentPen(W3D_Context *context, ULONG pen);
ULONG W3D_SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor);
ULONG W3D_SetDrawRegionWBM(W3D_Context *context, W3D_Bitmap *bm,W3D_Scissor *scissor);
ULONG W3D_SetFilter(W3D_Context *context, W3D_Texture *texture,ULONG MinFilter, ULONG MagFilter);
ULONG W3D_SetFogParams(W3D_Context *context, W3D_Fog *fogparams,ULONG fogmode);
ULONG W3D_SetLogicOp(W3D_Context *context, ULONG operation);
ULONG W3D_SetPenMask(W3D_Context *context, ULONG pen);
ULONG W3D_SetState(W3D_Context *context, ULONG state, ULONG action);
ULONG W3D_SetStencilFunc(W3D_Context *context, ULONG func, ULONG refvalue,ULONG mask);
ULONG W3D_SetStencilOp(W3D_Context *context, ULONG sfail, ULONG dpfail,ULONG dppass);
ULONG W3D_SetTexEnv(W3D_Context *context, W3D_Texture *texture,ULONG envparam, W3D_Color *envcolor);
ULONG W3D_SetWrapMode(W3D_Context *context, W3D_Texture *texture,ULONG s_mode, ULONG t_mode, W3D_Color *bordercolor);
ULONG W3D_SetWriteMask(W3D_Context *context, ULONG mask);
ULONG W3D_SetZCompareMode(W3D_Context *context, ULONG mode);
ULONG W3D_TexCoordPointer(W3D_Context* context, void *pointer, int stride,int unit, int off_v, int off_w, ULONG flags);
ULONG W3D_UpdateTexImage(W3D_Context *context, W3D_Texture *texture,void *teximage, int level, ULONG *palette);
ULONG W3D_UpdateTexSubImage(W3D_Context *context, W3D_Texture *texture, void *teximage,ULONG level, ULONG *palette, W3D_Scissor* scissor, ULONG srcbpr);
ULONG W3D_UploadTexture(W3D_Context *context, W3D_Texture *texture);
ULONG W3D_VertexPointer(W3D_Context* context, void *pointer, int stride,ULONG mode, ULONG flags);
ULONG W3D_WriteStencilPixel(W3D_Context *context, ULONG x, ULONG y, ULONG st);
ULONG W3D_WriteStencilSpan(W3D_Context *context, ULONG x, ULONG y, ULONG n,ULONG *st, UBYTE *mask);
void *Libmalloc(ULONG size );	;
void *Libmemcpy(void *s1,void *s2,LONG n);	;
void *MYmalloc(LONG size,UBYTE *name);
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,UWORD DrawMode,BOOL GotMipmaps);
void ClipPoly(struct SOFT3D_context *SC);
void ConvertBitmap(ULONG format,UBYTE *pt1,UBYTE *pt2,UWORD high,UWORD large,ULONG offset1,ULONG offset2,UBYTE *palette);
void CopyImageToDrawRegion(W3D_Context *context);
void CreateMipmaps(struct SOFT3D_texture *ST);
void DrawLinePIX(struct SOFT3D_context *SC);
void DrawPointPIX(struct SOFT3D_context *SC);
void DrawPointSimplePIX(struct SOFT3D_context *SC,register union pixel3D *P);
void DrawPolyP(struct SOFT3D_context *SC);
void DrawPolyPIX(struct SOFT3D_context *SC);
void DrawTriLines(W3D_Context *context,WORD x1,WORD y1,WORD x2,WORD y2,WORD x3,WORD y3);
void DrawTriP(struct SOFT3D_context *SC,register struct point3D *A,register struct point3D *B,register struct point3D *C);
void DumpMem(UBYTE *pt,LONG nb);
void Fill_Zoff_Tex_Fog(struct SOFT3D_context *SC);
void Fill_Ztest_Tex(struct SOFT3D_context *SC);
void Fill_Ztest_Tex_Color_Fog(struct SOFT3D_context *SC);
void Fill_Ztest_Tex_Fog(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex_Color_Fog(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex_Fog(struct SOFT3D_context *SC);
void FillPixels_Tex32a(struct SOFT3D_context *SC);
void FillPixels_Zoff_Color24(struct SOFT3D_context *SC);
void FillPixels_Zoff_Color32(struct SOFT3D_context *SC);
void FillPixels_Zoff_Flat24(struct SOFT3D_context *SC);
void FillPixels_Zoff_Tex24(struct SOFT3D_context *SC);
void FillPixels_Ztest_Color32(struct SOFT3D_context *SC);
void FillPixels_Zwrite_Color24(struct SOFT3D_context *SC);
void FillPixels_Zwrite_Flat24(struct SOFT3D_context *SC);
void FillPixels_Zwrite_Tex24(struct SOFT3D_context *SC);
void GetPoint(struct WAZP3D_context *WC,ULONG i);
void GetVertex(struct WAZP3D_context *WC,W3D_Vertex *V);
void LibAlert(UBYTE *t);
void Libfree(void *p);
void Libprintf(UBYTE *string, ...);
void LibSettings(struct MyButton *ButtonList,WORD ButtonCount);
void mipmap_index(void *texture);
void MYfree(void *pt);
void Pixels_Color24(struct SOFT3D_context *SC);
void Pixels_Color32(struct SOFT3D_context *SC);
void Pixels_Nothing(struct SOFT3D_context *SC);
void Pixels_Tex24(struct SOFT3D_context *SC);
void Pixels_Tex24_Fog(struct SOFT3D_context *SC);
void Pixels_Tex24_Fog_Color32(struct SOFT3D_context *SC);
void Pixels_Tex32a(struct SOFT3D_context *SC);
void Pixels_Tex32A(struct SOFT3D_context *SC);
void Pixels_Tex32a_Fog(struct SOFT3D_context *SC);
void Pixels_Tex32A_Fog(struct SOFT3D_context *SC);
void Pixels_Tex32A_Fog_Color32(struct SOFT3D_context *SC);
void Pixels_Tex32B(struct SOFT3D_context *SC);
void Pixels_Tex32B_Fog(struct SOFT3D_context *SC);
void PrintContext(W3D_Context *C);
void PrintDriver(W3D_Driver *D);
void PrintME(struct memory3D *ME);
void PrintP(struct point3D *P);
void PrintPIX(union pixel3D *PIX);
void PrintPIXfull(union pixel3D *PIX);
void PrintRGBA(UBYTE *RGBA);
void PrintST(struct SOFT3D_texture *ST);
void PrintTexture(W3D_Texture *T);
void PrintWT(struct WAZP3D_texture *WT);
void ReduceBitmap(UBYTE *pt,UBYTE *pt2,WORD large,WORD high, WORD bits,WORD ratio);
void SOFT3D_ClearImage(struct SOFT3D_context *SC,WORD x,WORD y,WORD large,WORD high);
void SOFT3D_ClearZBuffer(struct SOFT3D_context *SC,WORD z16);
void SOFT3D_DrawPrimitive(struct SOFT3D_context *SC,struct point3D *P,LONG Pnb,ULONG primitive);
void SOFT3D_End(struct SOFT3D_context *SC);
void SOFT3D_Fog(struct SOFT3D_context *SC,WORD FogMode,float FogZmin,float FogZmax,float FogDensity,UBYTE *FogRGBA);
void SOFT3D_FreeTexture(struct SOFT3D_context *SC,void *texture);
void SOFT3D_SetBackground(struct SOFT3D_context *SC,UBYTE  *RGBA);
void SOFT3D_SetClipping(struct SOFT3D_context *SC,float xmin,float xmax,float ymin,float ymax,float zmin,float zmax);
void SOFT3D_SetCulling(struct SOFT3D_context *SC,UWORD Culling);
void SOFT3D_SetDrawMode(struct SOFT3D_context *SC,void *texture,WORD DrawMode);
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat );	;
void TextureAlphaToRGB(struct SOFT3D_context *SC,struct SOFT3D_texture *ST);
void TextureAlphaUsage(struct SOFT3D_texture *ST);
void UVtoRGBA(struct SOFT3D_texture *ST,float u,float v,UBYTE *RGBA);
void W3D_DestroyContext(W3D_Context *context);
void W3D_FlushFrame(W3D_Context *context);
void W3D_FlushTextures(W3D_Context *context);
void W3D_FreeScreenmodeList(W3D_ScreenMode *list);
void W3D_FreeTexObj(W3D_Context *context, W3D_Texture *texture);
void W3D_ReleaseTexture(W3D_Context *context, W3D_Texture *texture);
void W3D_SetFrontFace(W3D_Context* context, ULONG direction);
void W3D_SetScissor(W3D_Context *context, W3D_Scissor *scissor);
void W3D_UnLockHardware(W3D_Context *context);
void W3D_WaitIdle(W3D_Context *context);
void W3D_WriteZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z);
void W3D_WriteZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z, UBYTE *mask);
void WAZP3D_Settings();
void AntiAliasImage3X3(struct SOFT3D_context *SC);
BOOL SetState(W3D_Context *context,ULONG state,BOOL set);
void SOFT3D_SetPointSize(struct SOFT3D_context *SC,UWORD PointSize);
void SOFT3D_SetZbuffer(struct SOFT3D_context *SC,WORD *Zbuffer16);
/*==================================================================================*/
void PrintME(struct memory3D *ME)
{
UBYTE *wall;
	wall=ME->pt;wall=wall+ME->size;
	Libprintf("[ME %ld nextME %ld\t] pt=%ld \tsize=%ld \t<%s> \t[%ld]\n ",ME,ME->nextME,ME->pt,ME->size,ME->name,wall[0]);
}
/*==================================================================================*/
LONG ListMemoryUsage()
{
struct memory3D *ME=firstME;
LONG MemoryUsage=0;
LONG MEnb;

	if(Wazp3D.DebugMemList.ON)
		Libprintf("TRACKED=MEMORY=USAGE=================)\n");

	MemoryUsage=0;
	MEnb=0;
	while(ME!=NULL)			/* for all packages in list */
	{
	if(Wazp3D.DebugMemList.ON)
		{Libprintf("[%ld]",MEnb); PrintME(ME);}
	MemoryUsage=MemoryUsage+ME->size;
	MEnb++;
	ME=ME->nextME;
	}
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("[%ld] MemoryUsage = %ld bytes = %ld MB\n",MEnb,MemoryUsage,MemoryUsage/(1024*1024));

	return(MemoryUsage);
}
/*==================================================================================*/
void *MYmalloc(LONG size,UBYTE *name)
{
struct memory3D *ME;
UBYTE *pt;
UBYTE *wall;


	pt=NULL;
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("Will call malloc() for %ld bytes for <%s>\n",size,name);
	if(size > 20000000)
		Libprintf("BIG malloc() for %ld bytes for <%s>\n",size,name);

	pt=malloc(size+4+sizeof(struct memory3D));
	if (pt==NULL)
		{STEP(malloc fail !);}

	ME=(struct memory3D *)pt;
	ME->pt=pt+sizeof(struct memory3D);
	ME->size=size;
	strcpy(ME->name,name);
	wall=ME->pt;wall=wall+ME->size;wall[0]=111;
	ME->nextME=firstME;
	firstME=ME;
	memset(ME->pt,0,ME->size);
	ListMemoryUsage();
	REMP("MYmalloc() OK give pt: %ld (up to %ld) for <%s> \n",ME->pt,pt+sizeof(struct memory3D)+ME->size,ME->name);

	return(ME->pt);
}
/*==================================================================================*/
void MYfree(void *pt)
{
struct memory3D *ME;
struct memory3D fakeME;
struct memory3D *thisME=&fakeME;
UBYTE *Bpt=pt;

	if(pt==NULL) return;
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("Will free() memory at %ld\n",pt);
	Bpt=Bpt-sizeof(struct memory3D);
	ME =(struct memory3D *)Bpt;
	thisME->nextME=firstME;
	while(thisME!=NULL)
	{
	if(thisME->nextME==ME)
		{
		if(thisME->nextME==firstME)
			firstME=ME->nextME;
		else
			thisME->nextME=ME->nextME;
		REMP("MYfree() OK for pt: %ld was <%s>\n",ME->pt,ME->name);
		free(ME);
		ListMemoryUsage();
		return;
		}
	thisME=thisME->nextME;
	}

Libprintf("MYfree() ME %ld not found ==> not allocated pt: %ld  !!!!\n",ME,pt);
}
/*==================================================================================*/
void DumpMem(UBYTE *pt,LONG nb)
{
LONG n;

NLOOP(nb/4)
	{
	Libprintf("[%ld\t][%ld\t] %ld\t%ld\t%ld\t%ld\n",pt,4*n,pt[0],pt[1],pt[2],pt[3]);
	pt=&(pt[4]);
	}
}
/*=================================================================*/
void WAZP3D_Settings()
{
WORD ButtonCount;
#ifdef WAZP3DDEBUG
	ButtonCount=((LONG)&Wazp3D.DebugMemUsage -(LONG)&Wazp3D.HardwareLie)/sizeof(struct MyButton)+1;
#else
	ButtonCount=((LONG)&Wazp3D.DebugWazp3D  - (LONG)&Wazp3D.HardwareLie)/sizeof(struct MyButton);
#endif
	Wazp3D.DebugWazp3D.ON=LibDebug;
	LibSettings(&Wazp3D.HardwareLie,ButtonCount);
	LibDebug=Wazp3D.DebugWazp3D.ON;
}
/*=================================================================*/
void WAZP3D_Function(UBYTE n)
{
	Wazp3D.CrashFunctionCall=0;		/* to start step-by-step with this call */

	if(!Wazp3D.DebugWazp3D.ON) return;

	if(Wazp3D.FunctionCallsAll!= 0xFFFFFFFF);
		Wazp3D.FunctionCallsAll++;
	if(Wazp3D.FunctionCalls[n]!= 0xFFFFFFFF);
		Wazp3D.FunctionCalls[n]++;

	if(Wazp3D.DebugCalls.ON)
		Libprintf("[%ld][%ld]",Wazp3D.FunctionCallsAll,Wazp3D.FunctionCalls[n]);
	if(Wazp3D.DebugFunction.ON)
		Libprintf("%s\n" ,Wazp3D.FunctionName[n]);

	if(Wazp3D.FunctionCallsAll==Wazp3D.CrashFunctionCall)
		Wazp3D.StepFunction.ON=Wazp3D.StepSOFT3D.ON=TRUE;

	if(Wazp3D.StepFunction.ON)
		LibAlert(Wazp3D.FunctionName[n]);
}
/*=================================================================*/
void PrintRGBA(UBYTE *RGBA)
	{
	if (!Wazp3D.DebugVal.ON) return;
	Libprintf(" RGBA %ld %ld %ld %ld\n",RGBA[0],RGBA[1],RGBA[2],RGBA[3]);
	}
/*=================================================================*/
void PrintP(struct point3D *P)
	{
	if (!Wazp3D.DebugPoint.ON) return;
	Libprintf(" XYZ %ld %ld %ld UV %ld %ld ",(WORD)P->x,(WORD)P->y,(WORD)(1024.0*P->z),(WORD)(256.0*P->u),(WORD)(256.0*P->v));
/*	Libprintf(" XYZ %2.2f %2.2f %2.2f UV %2.2f %2.2f",P->x,P->y,P->z,P->u,P->v);*/
	if (Wazp3D.DebugVal.ON)
		PrintRGBA((UBYTE *)&P->RGBA);
	else
		Libprintf("\n");
	}
/*=================================================================*/
void PrintWT(struct WAZP3D_texture *WT)
	{
	if (!Wazp3D.DebugWT.ON) return;
	Libprintf("WAZP3D_texture(%ld) %ldX%ldX%ld  pt%ld ST<%ld> NextWT<%ld> \n",WT,WT->large,WT->high,WT->bits,WT->pt,WT->ST,WT->nextWT);
	}
/*=================================================================*/
void PrintST(struct SOFT3D_texture *ST)
{
	if (!Wazp3D.DebugPoint.ON) return;
	if (!Wazp3D.DebugST.ON) return;
	Libprintf("SOFT3D_texture(%ld) %ldX%ldX%ld  pt %ld pt2 %ld NextST(%ld) Mip %ld DrawM:%ld TexM:%ld IndexXY %ld %ld\n",ST,ST->large,ST->high,ST->bits,ST->pt,ST->pt2,ST->nextST,ST->mipmapped,ST->DrawMode,ST->TexMode,ST->TindexX,ST->TindexY);
/*	Libsavefile("Texture.raw",ST->pt,ST->large*ST->high*ST->bits/8); */
}
/*=================================================================*/
void PrintPIXfull(union pixel3D *PIX)
{
	if (!Wazp3D.DebugPoint.ON)  return;
	if (!Wazp3D.DebugSOFT3D.ON) return;
	Libprintf(" XYZ %ld.%ld %ld.%ld %ld.%ld \tTexUV %ld.%ld %ld.%ld ",PIX->W.x,PIX->W.xlow,PIX->W.y,PIX->W.ylow,PIX->W.z,PIX->W.zlow,PIX->W.u,PIX->W.u3,PIX->W.v,PIX->W.v3);
	Libprintf("RGBA %ld.%ld %ld.%ld %ld.%ld %ld.%ld large %ld\n",PIX->W.R,PIX->W.R3,PIX->W.G,PIX->W.G3,PIX->W.B,PIX->W.B3,PIX->W.A,PIX->W.A3,PIX->W.large);
}
/*=================================================================*/
void PrintPIX(union pixel3D *PIX)
{
	if (!Wazp3D.DebugPoint.ON)  return;
	if (!Wazp3D.DebugSOFT3D.ON) return;
	Libprintf("XYZ %ld %ld %ld \tTexUV %ld %ld ",PIX->W.x,PIX->W.y,PIX->W.z,PIX->W.u,PIX->W.v);
	Libprintf("RGBA %ld %ld %ld %ld large %ld\n",PIX->W.R,PIX->W.G,PIX->W.B,PIX->W.A,PIX->W.large);
}
/*==========================================================================*/
void PrintAllT(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;
WORD Ntexture;

	SFUNCTION(PrintAllT)
	if(WC->firstWT==NULL)	return;
	Ntexture=0;
	WT=WC->firstWT;

	while(WT!=NULL)
	 {
	VAL(Ntexture)
	PrintWT(WT);
	WT=WT->nextWT;	/* my own linkage */
	Ntexture++;
	}
}
/*=============================================================*/
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,ULONG *Image)
{
register union blend3D Blend;
struct SOFT3D_context *SC;
WORD x,y;
float m=5.0;	/* margin for clipper = show if 3Dprog never setted scissor */

SFUNCTION(SOFT3D_Start)
	SC=MYmalloc(sizeof(struct SOFT3D_context),"SOFT3D_context");
	if(SC==NULL) return(NULL);
	SC->large=large;
	SC->high =high;

	if(Image==NULL)
		SC->Image32=MYmalloc(large*high*32/8,"SOFT3D_ImageBuffer auto");
	else
		SC->Image32=Image;

	YLOOP(high)
		{
		SC->side1[y].L.Image32Y   =SC->side2[y].L.Image32Y  =(ULONG *) &SC->Image32  [y*large];
		SC->side1[y].L.y=0;SC->side1[y].W.y=y;
		}

	Blend.L.Index=0;
	SC->PixBufferDone= SC->PixBuffer;
	SC->PixBufferMaxi=&SC->PixBuffer[BUFFERMAXI];
	XLOOP(256)
	YLOOP(256)
	{
	Blend.B.Alpha=x;
	Blend.B.Color=y;
	SC->Blending8[Blend.L.Index]=((x*y)/255);
	}

	SC->FlatRGBA[0]=100;SC->FlatRGBA[1]=200;SC->FlatRGBA[2]=255;SC->FlatRGBA[3]=100;
	SOFT3D_SetDrawMode(SC,NULL,NODRAW);
	SOFT3D_SetClipping(SC,m,(float)(large-1)-m,m,(float)(high-1)-m,0.0,0.999);
	SOFT3D_SetPointSize(SC,1);
	SC->Pxmin=0;
	SC->Pymin=0;
	SC->Pxmax=large-1;
	SC->Pymax=high -1;
	return(SC);
}
/*=============================================================*/
void SOFT3D_SetZbuffer(struct SOFT3D_context *SC,WORD *Zbuffer16)
{
WORD y;

SFUNCTION(SOFT3D_SetZbuffer)
	SC->Zbuffer16=Zbuffer16;
	if(Zbuffer16!=NULL)
	YLOOP(SC->high)
		{ SC->side1[y].L.Zbuffer16Y = SC->side2[y].L.Zbuffer16Y =Zbuffer16;Zbuffer16+=SC->large;}
}
/*=================================================================*/
void SOFT3D_SetCulling(struct SOFT3D_context *SC,UWORD Culling)
{
	SC->Culling=Culling;
}
/*=================================================================*/
void SOFT3D_SetClipping(struct SOFT3D_context *SC,float xmin,float xmax,float ymin,float ymax,float zmin,float zmax)
{
	SC->ClipMin.x=xmin;
	SC->ClipMax.x=xmax;
	SC->ClipMin.y=ymin;
	SC->ClipMax.y=ymax;
	SC->ClipMin.z=zmin;
	SC->ClipMax.z=zmax;
}
/*=================================================================*/
void SOFT3D_SetBackground(struct SOFT3D_context *SC,UBYTE  *RGBA)
{
		COPYRGBA(SC->RGBA,RGBA);
}
/*=============================================================*/
void SOFT3D_ClearImage(struct SOFT3D_context *SC,WORD x,WORD y,WORD large,WORD high)
{
ULONG *ptRGBA32=(ULONG *)SC->RGBA;
register ULONG RGBA32=ptRGBA32[0];
register ULONG *Image32;
register LONG  size;
register LONG  n;

SFUNCTION(SOFT3D_ClearImage)
	Image32=&SC->Image32[y*SC->large];
	size=SC->large*high/8;

	if (Wazp3D.DebugSOFT3D.ON)
		SC->RGBA[0]=SC->RGBA[1]=SC->RGBA[2]=200;

	if(RGBA32==0)
		memset(Image32,0,(32/8*SC->large*high));
	else

		NLOOP(size)
		{
		Image32[0]=RGBA32;Image32[1]=RGBA32;Image32[2]=RGBA32;Image32[3]=RGBA32;
		Image32[4]=RGBA32;Image32[5]=RGBA32;Image32[6]=RGBA32;Image32[7]=RGBA32;
		Image32+=8;
		}

}
/*=============================================================*/
void SOFT3D_ClearZBuffer(struct SOFT3D_context *SC,WORD z16)
{
register WORD *Zbuffer16=SC->Zbuffer16;
register LONG size=SC->large*SC->high/8;
register LONG n;

SFUNCTION(SOFT3D_ClearZBuffer)
	if(Zbuffer16==NULL) return;
z16=ZMAX; 			/* force to be cleaned */
	NLOOP(size)
	{
	Zbuffer16[0]=z16;	Zbuffer16[1]=z16;	Zbuffer16[2]=z16; Zbuffer16[3]=z16;
	Zbuffer16[4]=z16;	Zbuffer16[5]=z16;	Zbuffer16[6]=z16; Zbuffer16[7]=z16;
	Zbuffer16+=8;
	}
}
/*=============================================================*/
void SOFT3D_End(struct SOFT3D_context *SC)
{
SFUNCTION(SOFT3D_End)
	if(SC==NULL) return;
	MYfree(SC->Image32);
	MYfree(SC->Zbuffer16);
	MYfree(SC);
}
/*=============================================================*/
void Pixels_Nothing(struct SOFT3D_context *SC)
{
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Color24(struct SOFT3D_context *SC)
{
register struct buffer3D *PixBuffer=SC->PixBufferDone;

Pixels:
	PixBuffer--;
	COPYRGBA(PixBuffer->Image8,PixBuffer->RGBA);
	PixBuffer->Image8[3]= 255;

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Color32(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register UBYTE *Image8;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register union blend3D Blend;

	Blend.L.Index=0;
Pixels:
	PixBuffer--;
	Image8=PixBuffer->Image8;
	RGBA=PixBuffer->RGBA;
	if (RGBA[3] > ALPHAMIN)				/* if color visible ? */
	{
	Blend.B.Alpha=255-RGBA[3];			/* image buffer += color */
	Blend.B.Color=Image8[0] ;
	Image8[0] =RGBA[0]+Blending8[Blend.L.Index];
	Blend.B.Color=Image8[1] ;
	Image8[1] =RGBA[1]+Blending8[Blend.L.Index];
	Blend.B.Color=Image8[2] ;
	Image8[2] =RGBA[2]+Blending8[Blend.L.Index];
	Image8[3]= 255;
	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex24(struct SOFT3D_context *SC)
{
register struct buffer3D *PixBuffer=SC->PixBufferDone;

Pixels:
	PixBuffer--;
	COPYRGBA(PixBuffer->Image8,PixBuffer->Tex8);
	PixBuffer->Image8[3]= 255;

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32a(struct SOFT3D_context *SC)
{
register struct buffer3D *PixBuffer=SC->PixBufferDone;

Pixels:
	PixBuffer--;
	if (PixBuffer->Tex8[alpha] > ALPHAMIN)					/* if texture visible ? */
	{
	COPYRGBA(PixBuffer->Image8,PixBuffer->Tex8);
	PixBuffer->Image8[3]= 255;
	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32B(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
		RGBA=PixBuffer->Image8;

		if( A < ALPHAMAX)				/* if texture not solid ? */
		{
/*-------------- add to image-------*/
		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/


		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32A(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;

REM(Pixels_Tex32A)
	Blend.L.Index=0;
Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
		RGBA=PixBuffer->Image8;
		if( A < ALPHAMAX)				/* if texture not solid ? */
		{
/*-------------- add to image-------*/
		Blend.B.Alpha=A;				/* Alpha blend Texture */
		Blend.B.Color=R;
		R=Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=Blending8[Blend.L.Index];

		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex24_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register UBYTE *Fog8;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register union blend3D Blend;


REM(Pixels_Tex24_Fog)
	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	Image8=PixBuffer->Image8;
	Tex8  =PixBuffer->Tex8;
	Fog8	=PixBuffer->Fog8;
	if (Fog8[3] < ALPHAMAX)			 /* if fog visible ? */
	{
		Blend.B.Alpha=Fog8[3];			/* image = texture + fog */
		Blend.B.Color=Tex8[0];
		Image8[0]=Fog8[0]+Blending8[Blend.L.Index];
		Blend.B.Color=Tex8[1];
		Image8[1]=Fog8[1]+Blending8[Blend.L.Index];
		Blend.B.Color=Tex8[2];
		Image8[2]=Fog8[2]+Blending8[Blend.L.Index];
	}
	else
	{							/* if no fog => image = texture */
	Image8[0]= Tex8[0];
	Image8[1]= Tex8[1];
	Image8[2]= Tex8[2];
	}
	Image8[3]= 255;

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32a_Fog(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
/*------------- fog -----------------*/
		RGBA=PixBuffer->Fog8;
		if (RGBA[3] < ALPHAMAX)			 /* if fog visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += fog */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA=PixBuffer->Image8;
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32B_Fog(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
/*------------- fog -----------------*/
		RGBA=PixBuffer->Fog8;
		if (RGBA[3] < ALPHAMAX)			 /* if fog visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += fog */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}

		RGBA=PixBuffer->Image8;
		if( A < ALPHAMAX)				/* if texture not solid ? */
		{
/*-------------- add to image-------*/
		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}


	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32A_Fog(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
/*------------- fog -----------------*/
		RGBA=PixBuffer->Fog8;
		if (RGBA[3] < ALPHAMAX)			 /* if fog visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += fog */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}

		RGBA=PixBuffer->Image8;
		if( A < ALPHAMAX)				/* if texture not solid ? */
		{
/*-------------- add to image-------*/
		Blend.B.Alpha=A;				/* Alpha blend Texture */
		Blend.B.Color=R;
		R=Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=Blending8[Blend.L.Index];

		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;
	}


	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex24_Fog_Color32(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	R= RGBA[0];				/* color buffer = texture */
	G= RGBA[1];
	B= RGBA[2];
/*------------- fog -----------------*/
		RGBA=PixBuffer->Fog8;
		if (RGBA[3] < ALPHAMAX)			 /* if fog visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += fog */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}

/*-------------- color --------------*/
		RGBA=PixBuffer->RGBA;
		if (RGBA[3] < ALPHAMAX)			 /* if color visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += color */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}

/*-------------- add to image-------*/
	RGBA=PixBuffer->Image8;
	RGBA[0]=R;
	RGBA[1]=G;
	RGBA[2]=B;
	RGBA[3]=255;

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Pixels_Tex32A_Fog_Color32(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register struct buffer3D *PixBuffer=SC->PixBufferDone;
register UBYTE *Blending8=SC->Blending8;
register UBYTE R;
register UBYTE G;
register UBYTE B;
register UBYTE A;
register union blend3D Blend;


	Blend.L.Index=0;

Pixels:
	PixBuffer--;
	RGBA=PixBuffer->Tex8;
	A= RGBA[alpha];
	if (A > ALPHAMIN)					/* if texture visible ? */
	{
		R= RGBA[0];				/* color buffer = texture */
		G= RGBA[1];
		B= RGBA[2];
/*------------- fog -----------------*/
		RGBA=PixBuffer->Fog8;
		if (RGBA[3] < ALPHAMAX)			 /* if fog visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += fog */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}

		RGBA=PixBuffer->Image8;
		if( A < ALPHAMAX)				/* if texture not solid ? */
		{
/*-------------- add to image-------*/
		Blend.B.Alpha=A;				/* Alpha blend Texture */
		Blend.B.Color=R;
		R=Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=Blending8[Blend.L.Index];

		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- color --------------*/
		RGBA=PixBuffer->RGBA;
		if (RGBA[3] < ALPHAMAX)			 /* if color visible ? */
		{
		Blend.B.Alpha=RGBA[3];			/* color buffer += color */
		Blend.B.Color=R;
		R=RGBA[0]+Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=RGBA[1]+Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=RGBA[2]+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
	RGBA=PixBuffer->Image8;
	RGBA[0]=R;
	RGBA[1]=G;
	RGBA[2]=B;
	RGBA[3]=255;
	}

	if(PixBuffer != SC->PixBuffer) goto Pixels;
	SC->PixBufferDone= SC->PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}

	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}

SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(Fill_Ztest_Tex)

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;


REM(Fill_Zwrite_Tex_Fog)
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Fog8=(UBYTE*)&(SC->Fog32[Pix->W.z/16] );
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}

SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Fog8=(UBYTE*)&(SC->Fog32[Pix->W.z/16] );
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}

SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff_Tex_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;


REM(Fill_Zoff_Tex_Fog)
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	large	=Pix->W.large;
	while(0<large--)
	{
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Fog8=(UBYTE*)&(SC->Fog32[Pix->W.z/16] );
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer++;
		Image8+=4;
		Pix->L.z+=Xdelta->L.z;
		Pix->L.u+=Xdelta->L.u;
		Pix->L.v+=Xdelta->L.v;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex_Color_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

	 while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Fog8=(UBYTE*)&(SC->Fog32[Pix->W.z/16] );
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer->RGBA[0]=Pix->W.R;
		PixBuffer->RGBA[1]=Pix->W.G;
		PixBuffer->RGBA[2]=Pix->W.B;
		PixBuffer->RGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}

SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex_Color_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;
register struct buffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(Fill_Ztest_Tex_Color_Fog)
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		PixBuffer->Fog8=(UBYTE*)&(SC->Fog32[Pix->W.z/16] );
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=Tex8;
		PixBuffer->RGBA[0]=Pix->W.R;
		PixBuffer->RGBA[1]=Pix->W.G;
		PixBuffer->RGBA[2]=Pix->W.B;
		PixBuffer->RGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi) {SC->PixBufferDone=PixBuffer;  SC->PixelsFunction(SC);}
	Pix++; Xdelta++;
	}

SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void FillPixels_Zoff_Flat24(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register UBYTE *RGBA=SC->FlatRGBA;

register WORD high  =SC->PolyHigh;
register WORD large;


REM(FillPixels_Zoff_Flat24)
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	large	=Pix->W.large;
	while(0<large--)
	{
	COPYRGBA(Image8,RGBA);
	Image8+=4;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zwrite_Flat24(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register UBYTE *RGBA=SC->FlatRGBA;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(FillPixels_Zwrite_Color24)

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		COPYRGBA(Image8,RGBA);
		}
	Image8+=4;
	Zbuffer16++;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zoff_Color24(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(FillPixels_Zoff_Color24)

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	Image8[0]= Pix->W.R;
	Image8[1]= Pix->W.G;
	Image8[2]= Pix->W.B;
	Image8[3]= 255;

	Image8+=4;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zwrite_Color24(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(FillPixels_Zwrite_Color24)

	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		Image8[0]= Pix->W.R;
		Image8[1]= Pix->W.G;
		Image8[2]= Pix->W.B;
		Image8[3] =Pix->W.A;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zoff_Color32(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register UBYTE *Blending8=SC->Blending8;
register union blend3D Blend;

register WORD high  =SC->PolyHigh;
register WORD large;
UBYTE R,G,B,A;

REM(FillPixels_Zoff_Color32)
	Blend.L.Index=0;

	while(0<high--)
	{
	RGBA  =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	large	=Pix->W.large;
	large--;	/* dont rewrite same edge for 2 triangles */

	while(0<large--)
	{
	A= Pix->W.A;
	if (A > ALPHAMIN)					/* if color32 visible ? */
	{
		R= Pix->W.R;				/* color buffer = color32 */
		G= Pix->W.G;
		B= Pix->W.B;
		if( A < ALPHAMAX)				/* if color32 not solid ? */
		{
/*-------------- add to image-------*/
		Blend.B.Alpha=A;				/* Alpha blend color32 */
		Blend.B.Color=R;
		R=Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=Blending8[Blend.L.Index];

		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}

	RGBA+=4;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Ztest_Color32(struct SOFT3D_context *SC)
{
register UBYTE *RGBA;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register UBYTE *Blending8=SC->Blending8;
register union blend3D Blend;

register WORD high  =SC->PolyHigh;
register WORD large;
UBYTE R,G,B,A;

REM(FillPixels_Ztest_Color32)
	Blend.L.Index=0;

	while(0<high--)
	{
	RGBA	 =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
	A= Pix->W.A;
	if (A > ALPHAMIN)					/* if color32 visible ? */
	{
		R= Pix->W.R;					/* color buffer = color32 */
		G= Pix->W.G;
		B= Pix->W.B;
		if( A < ALPHAMAX)				/* if color32 not solid ? */
		{
/*-------------- add to image-------*/
		Blend.B.Alpha=A;				/* Alpha blend color32 */
		Blend.B.Color=R;
		R=Blending8[Blend.L.Index];
		Blend.B.Color=G;
		G=Blending8[Blend.L.Index];
		Blend.B.Color=B;
		B=Blending8[Blend.L.Index];

		A=255-A;
		Blend.B.Alpha=A;			/* color buffer += 1-Alpha blend Image */
		Blend.B.Color=RGBA[0];
		R=R+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[1];
		G=G+Blending8[Blend.L.Index];
		Blend.B.Color=RGBA[2];
		B=B+Blending8[Blend.L.Index];
		}
/*-------------- copy to image-------*/
		RGBA[0]=R;
		RGBA[1]=G;
		RGBA[2]=B;
		RGBA[3]=255;

	}

	}
	RGBA+=4;
	Zbuffer16++;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zwrite_Tex24(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(FillPixels_Zwrite_Tex24)
	while(0<high--)
	{
	Image8=(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16= (& Pix->L.Zbuffer16Y[Pix->W.x]);
	large=Pix->W.large;
	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
		Image8[0]= Tex8[0];
		Image8[1]= Tex8[1];
		Image8[2]= Tex8[2];
		Image8[3]= 255;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Zoff_Tex24(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;

register WORD high  =SC->PolyHigh;
register WORD large;

REM(FillPixels_Zoff_Tex24)
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	large	   =Pix->W.large;
	while(0<large--)
	{
	Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
	Image8[0]= Tex8[0];
	Image8[1]= Tex8[1];
	Image8[2]= Tex8[2];
	Image8[3]= 255;
	Image8+=4;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void FillPixels_Tex32a(struct SOFT3D_context *SC)
{
register UBYTE *Tex8;
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register ULONG *TindexXY=SC->ST->TindexX;


register WORD high  =SC->PolyHigh;
register WORD large;
register union blend3D Blend;

REM(FillPixels_Tex32a)
	Blend.L.Index=0;
	while(0<high--)
	{
	Image8   =(UBYTE *) (& Pix->L.Image32Y[Pix->W.x]);
	Zbuffer16=	(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	=Pix->W.large;

	while(0<large--)
	{
	if(*Zbuffer16++ > Pix->W.z)
	 {
	Blend.B.Color=Pix->W.u;
	Blend.B.Alpha=Pix->W.v;
	Tex8=(UBYTE *) (TindexXY[Pix->W.u ]+TindexXY[Pix->W.v+256]);
	if (Tex8[alpha] > ALPHAMIN)					/* if texture visible ? */
		{
		Image8[0]= Tex8[0];
		Image8[1]= Tex8[1];
		Image8[2]= Tex8[2];
		Image8[3]= 255;
		}
	}
	Image8+=4;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;
	}
	Pix++; Xdelta++;
	}
}
/*=============================================================*/
void DrawPointSimplePIX(struct SOFT3D_context *SC,register union pixel3D *P)
{

SFUNCTION(DrawPointSimplePIX)
	if ( (P->W.x < 0) ou (P->W.y < 0) ou (SC->large <= P->W.x) ou (SC->high <= P->W.y))
		return;

	SC->Pix  =&(SC->side1[P->W.y]);
	Libmemcpy(SC->Pix,P,9*4);
	SC->Pix->W.large=1;
	SC->PolyHigh	=1;

/* WARNING: flat coloring so Xdelta Ydelta are not computed  */
	SC->FillFunction(SC);
}
/*=============================================================*/
void DrawPointPIX(struct SOFT3D_context *SC)
{
register union pixel3D *P=SC->PolyPIX;
register WORD *PointLarge=SC->PointLarge;
register WORD xmin,xmax,ymin,ymax,y;
register WORD R;

SFUNCTION(DrawPointPIX)
	if(SC->PointSize==1)
		{DrawPointSimplePIX(SC,P);return;}

	R=SC->PointSize/2+1;
	xmin=0+R;
	ymin=0+R;
	xmax=SC->large-R;
	ymax=SC->high -R;
	if ( (P->W.x<xmin) ou (P->W.y<ymin) ou (xmax<=P->W.x) ou (ymax<=P->W.y))
		return;

	P->W.y	=P->W.y - SC->PointSize/2;
	SC->Pix	  =&(SC->side1[P->W.y]);
	SC->PolyHigh=SC->PointSize;

	YLOOP(SC->PolyHigh)
		{
		Libmemcpy(&SC->Pix[y],P,9*4);
		SC->Pix[y].W.large =PointLarge[y];
		SC->Pix[y].W.x	 =SC->Pix[y].W.x - PointLarge[y]/2;
		}

/* WARNING: flat coloring so Xdelta Ydelta are not computed  */
	SC->FillFunction(SC);
}
/*=============================================================*/
void DrawLinePIX(struct SOFT3D_context *SC)
{
register union pixel3D *Pmin=&SC->PolyPIX[0];
register union pixel3D *Pmax=&SC->PolyPIX[1];
register union pixel3D *side=SC->side1;
register union pixel3D *temp;
register WORD LineHigh,LineLarge;
register WORD n;

	SFUNCTION(DrawLinePIX)
/* Trouver point Pmin et point Pmax des deux points de cette ligne */
	if (Pmin->W.y > Pmax->W.y)
		SWAP(Pmin,Pmax);

	if ( (Pmin->W.x<0) ou (Pmin->W.y<0) ou (SC->large<=Pmin->W.x) ou (SC->high<=Pmin->W.y) ou (Pmax->W.x<0) ou (Pmax->W.y<0) ou (SC->large<=Pmax->W.x) ou (SC->high<=Pmax->W.y))
		return;

	LineLarge= Pmax->W.x - Pmin->W.x + 1;
	LineHigh = Pmax->W.y - Pmin->W.y ;
	if(LineLarge<0) LineLarge=-LineLarge;
	LineLarge=LineLarge+1;
	SC->PolyHigh = LineHigh;

	VAR(LineLarge)
	VAR(LineHigh)

	if(LineLarge+LineHigh==2)
		{DrawPointSimplePIX(SC,Pmin);return;}
	if(LineLarge+LineHigh <5)
		{DrawPointSimplePIX(SC,Pmin);DrawPointSimplePIX(SC,Pmax);return;}

	PrintPIX(Pmin);
	PrintPIX(Pmax);
/* Stocker les deux points dans le side */
	Libmemcpy(&(side[Pmin->W.y]),Pmin,9*4);
	Pmin=&(side[Pmin->W.y]);
	Libmemcpy(&(side[Pmax->W.y]),Pmax,9*4);
	Pmax=&(side[Pmax->W.y]);

	if(LineHigh!=0)
	{
	SC->Ydelta.L.x=(Pmax->L.x-Pmin->L.x)/LineHigh;
	SC->Ydelta.L.z=(Pmax->L.z-Pmin->L.z)/LineHigh;
	SC->Ydelta.L.u=(Pmax->L.u-Pmin->L.u)/LineHigh;
	SC->Ydelta.L.v=(Pmax->L.v-Pmin->L.v)/LineHigh;
	SC->Ydelta.L.R=(Pmax->L.R-Pmin->L.R)/LineHigh;
	SC->Ydelta.L.G=(Pmax->L.G-Pmin->L.G)/LineHigh;
	SC->Ydelta.L.B=(Pmax->L.B-Pmin->L.B)/LineHigh;
	SC->Ydelta.L.A=(Pmax->L.A-Pmin->L.A)/LineHigh;
	}

	SC->Pix=side=Pmin;
	LineHigh--;				/* ne pas recalculer les points extrmits */
	NLOOP(LineHigh)
	{
		side[1].L.x=side[0].L.x+SC->Ydelta.L.x;
		side[1].L.z=side[0].L.z+SC->Ydelta.L.z;
		side[1].L.u=side[0].L.u+SC->Ydelta.L.u;
		side[1].L.v=side[0].L.v+SC->Ydelta.L.v;
		side[1].L.R=side[0].L.R+SC->Ydelta.L.R;
		side[1].L.G=side[0].L.G+SC->Ydelta.L.G;
		side[1].L.B=side[0].L.B+SC->Ydelta.L.B;
		side[1].L.A=side[0].L.A+SC->Ydelta.L.A;
		side++;
	}

	side=Pmin;
	LineHigh=SC->PolyHigh;
	if ((SC->PolyHigh  < 2   ) ou (SC->PolyHigh  > 1024))
		return;
	LineHigh--;				/* ne pas recalculer les points extrmits */
	NLOOP(LineHigh)
	{
		if(side[1].W.x == side[0].W.x)
			side[0].W.large= 1;
		if(side[1].W.x > side[0].W.x)
			side[0].W.large= side[1].W.x - side[0].W.x;
		if(side[1].W.x < side[0].W.x)
			side[1].W.large= side[0].W.x - side[1].W.x;
		side++;
	}
	Pmin->W.large=Pmax->W.large=1;

	SC->Xdelta=&(SC->delta[Pmin->W.y]);
	SC->Xdelta[0].L.z=(Pmax->L.z-Pmin->L.z)/LineLarge;
	SC->Xdelta[0].L.u=(Pmax->L.u-Pmin->L.u)/LineLarge;
	SC->Xdelta[0].L.v=(Pmax->L.v-Pmin->L.v)/LineLarge;
	SC->Xdelta[0].L.R=(Pmax->L.R-Pmin->L.R)/LineLarge;
	SC->Xdelta[0].L.G=(Pmax->L.G-Pmin->L.G)/LineLarge;
	SC->Xdelta[0].L.B=(Pmax->L.B-Pmin->L.B)/LineLarge;
	SC->Xdelta[0].L.A=(Pmax->L.A-Pmin->L.A)/LineLarge;

	NLOOP(SC->PolyHigh)
	{
	SC->Xdelta[n].L.z=SC->Xdelta[0].L.z;
	SC->Xdelta[n].L.u=SC->Xdelta[0].L.u;
	SC->Xdelta[n].L.v=SC->Xdelta[0].L.v;
	SC->Xdelta[n].L.R=SC->Xdelta[0].L.R;
	SC->Xdelta[n].L.G=SC->Xdelta[0].L.G;
	SC->Xdelta[n].L.B=SC->Xdelta[0].L.B;
	SC->Xdelta[n].L.A=SC->Xdelta[0].L.A;
	}

	SC->FillFunction(SC);
}
/*=============================================================*/
void DrawPolyPIX(struct SOFT3D_context *SC)
{
register union pixel3D *PIX=SC->PolyPIX;
register union pixel3D *side1;
register union pixel3D *side2;
register union pixel3D *Pmin=NULL;
register union pixel3D *Pmax=NULL;
register union pixel3D *temp;
register WORD Pnb=SC->PolyPnb;
register WORD n;
register WORD size;
register WORD PolyLarge;
register WORD PolyYmin;
register WORD PolyYmax;


	SFUNCTION(DrawPolyPIX)
	VAR(SC->PolyPnb)
	PolyYmin=PolyYmax=PIX[0].W.y;

/* Loop du Polygone */
NLOOP(Pnb)
{
	PrintPIXfull(&PIX[n]);
	Pmin =&(PIX[n]);
	if (n+1!=Pnb)
		Pmax =&(PIX[n+1]);
	else
		Pmax =&(PIX[0  ]);

	if(Pmin->W.y==Pmax->W.y)
		goto tracer_suivante;

/* Trouver point Pmin et point Pmax des deux points de cette ligne */
	if (Pmin->W.y < Pmax->W.y)
		{side1=SC->side1;}
	else
		{side1=SC->side2; SWAP(Pmin,Pmax);}

	if (Pmin->W.y < PolyYmin   )	PolyYmin=Pmin->W.y;
	if (PolyYmax   < Pmax->W.y )	PolyYmax=Pmax->W.y;


/* Stocker les deux points dans un side1 */
	Libmemcpy(&(side1[Pmin->W.y]),Pmin,9*4);
	Pmin=&(side1[Pmin->W.y]);
	Libmemcpy(&(side1[Pmax->W.y]),Pmax,9*4);
	Pmax=&(side1[Pmax->W.y]);

size = Pmax->W.y - Pmin->W.y;
if (size > 1 )
	{
	SC->Ydelta.L.x=(Pmax->L.x-Pmin->L.x)/size;
	SC->Ydelta.L.z=(Pmax->L.z-Pmin->L.z)/size;
	SC->Ydelta.L.u=(Pmax->L.u-Pmin->L.u)/size;
	SC->Ydelta.L.v=(Pmax->L.v-Pmin->L.v)/size;
	SC->Ydelta.L.R=(Pmax->L.R-Pmin->L.R)/size;
	SC->Ydelta.L.G=(Pmax->L.G-Pmin->L.G)/size;
	SC->Ydelta.L.B=(Pmax->L.B-Pmin->L.B)/size;
	SC->Ydelta.L.A=(Pmax->L.A-Pmin->L.A)/size;

	size--;				/* ne pas recalculer les points extrmits */
	side1=Pmin;
	DrawLine:
		side1[1].L.x=side1[0].L.x+SC->Ydelta.L.x;
		side1[1].L.z=side1[0].L.z+SC->Ydelta.L.z;
		side1[1].L.u=side1[0].L.u+SC->Ydelta.L.u;
		side1[1].L.v=side1[0].L.v+SC->Ydelta.L.v;
		side1[1].L.R=side1[0].L.R+SC->Ydelta.L.R;
		side1[1].L.G=side1[0].L.G+SC->Ydelta.L.G;
		side1[1].L.B=side1[0].L.B+SC->Ydelta.L.B;
		side1[1].L.A=side1[0].L.A+SC->Ydelta.L.A;
		side1++;
	if(size-- != 0) goto DrawLine;
	}
tracer_suivante:
 ;
}

/* Tracage des lignes fini ! */

/* Trier sides */
	n=(PolyYmin+PolyYmax)/2;
	if (SC->side1[n].L.x < SC->side2[n].L.x)
		{side1=SC->side1;side2=SC->side2;}
	else
		{side1=SC->side2;side2=SC->side1;}


/* trouver la largeur max */
	PolyLarge=0;
	side1=&(side1[PolyYmin]);
	side2=&(side2[PolyYmin]);
	SC->Pix=side1;
	SC->Xdelta=&(SC->delta[PolyYmin]);
	SC->PolyHigh=PolyYmax-PolyYmin+1;

	if ((SC->PolyHigh  < 2   ) ou (SC->PolyHigh  > 1024))
		return;

	NLOOP(SC->PolyHigh)
		{
		size= side1->W.large = 1 + side2->W.x - side1->W.x ;
		if(PolyLarge < size)
			{Pmin=side1; Pmax=side2; PolyLarge=size;}

		if(Wazp3D.UsePerspective.ON)
		if(size!=0)
		{
		SC->Xdelta[n].L.z=(side2->L.z-side1->L.z)/size;
		SC->Xdelta[n].L.u=(side2->L.u-side1->L.u)/size;
		SC->Xdelta[n].L.v=(side2->L.v-side1->L.v)/size;
		SC->Xdelta[n].L.R=(side2->L.R-side1->L.R)/size;
		SC->Xdelta[n].L.G=(side2->L.G-side1->L.G)/size;
		SC->Xdelta[n].L.B=(side2->L.B-side1->L.B)/size;
		SC->Xdelta[n].L.A=(side2->L.A-side1->L.A)/size;
		}
		side1++;side2++;
		}

	if(!Wazp3D.UsePerspective.ON)
	{
	SC->Xdelta[0].L.z=(Pmax->L.z-Pmin->L.z)/PolyLarge;
	SC->Xdelta[0].L.u=(Pmax->L.u-Pmin->L.u)/PolyLarge;
	SC->Xdelta[0].L.v=(Pmax->L.v-Pmin->L.v)/PolyLarge;
	SC->Xdelta[0].L.R=(Pmax->L.R-Pmin->L.R)/PolyLarge;
	SC->Xdelta[0].L.G=(Pmax->L.G-Pmin->L.G)/PolyLarge;
	SC->Xdelta[0].L.B=(Pmax->L.B-Pmin->L.B)/PolyLarge;
	SC->Xdelta[0].L.A=(Pmax->L.A-Pmin->L.A)/PolyLarge;

	NLOOP(SC->PolyHigh)
	{
	SC->Xdelta[n].L.z=SC->Xdelta[0].L.z;
	SC->Xdelta[n].L.u=SC->Xdelta[0].L.u;
	SC->Xdelta[n].L.v=SC->Xdelta[0].L.v;
	SC->Xdelta[n].L.R=SC->Xdelta[0].L.R;
	SC->Xdelta[n].L.G=SC->Xdelta[0].L.G;
	SC->Xdelta[n].L.B=SC->Xdelta[0].L.B;
	SC->Xdelta[n].L.A=SC->Xdelta[0].L.A;
	}
	}
	SFUNCTION(DrawPolyPIX-OK!)

	if(PolyLarge > 1   )
	if(PolyLarge < 1024)
	SC->FillFunction(SC);
}
/*==================================================================================*/
void SOFT3D_Fog(struct SOFT3D_context *SC,WORD FogMode,float FogZmin,float FogZmax,float FogDensity,UBYTE *FogRGBA)
{
register UBYTE *ThisFogRGBA;
register float d,z,f;
register union blend3D Blend;
register UBYTE *Blending8=SC->Blending8;
register UWORD n;

#define CLAMP(X,MIN,MAX) ((X)<(MIN)?(MIN):((X)>(MAX)?(MAX):(X)))

SFUNCTION(SOFT3D_Fog)
	Blend.L.Index=0;
	if(Wazp3D.DebugSOFT3D.ON)
	{
	Libprintf("SOFT3D_Fog\n");
	VAR(FogMode)
	VARF(FogZmin)
	VARF(FogZmax)
	VARF(FogDensity)
	}

	switch(FogMode)
	{
	case 0:  REM(NO_FOG)	break;
	case 1:  REM(GL_LINEAR)	break;
	case 2:  REM(GL_EXP)	break;
	case 3:  REM(GL_EXP2)	break;
	default: REM(BadFogMode) return;
	}

	if(FogZmin	<  0.0)	FogZmin =0.0;
	if(1.0  <= FogZmax)	FogZmax =0.9999;

	SC->FogMode		=FogMode;
	SC->FogZmin		=FogZmin;
	SC->FogZmax		=FogZmax;
	SC->FogDensity	=FogDensity;
	COPYRGBA(SC->FogRGBA,FogRGBA);

	if(FogMode==0)
		return;

	NLOOP(1024)
	{
	z=(((float)n)/1024.0);
	ThisFogRGBA=(UBYTE *)&SC->Fog32[n];
	switch (FogMode)
	{
	case 1:
		d = 1.0F / (FogZmax - FogZmin);
		f= (FogZmax - z) * d;
		break;
	case 2:
		d = -FogDensity;
		f= exp( d * z);
		break;
	default:	/*=3*/
		d = -(FogDensity*FogDensity);
		f= exp( d * z*z );
		break;
	}

	f = CLAMP( f, 0.0F, 1.0F );
	ThisFogRGBA[3] =(UBYTE) (255.0 * f);
	Blend.B.Alpha=ThisFogRGBA[3];
	Blend.B.Color=FogRGBA[0];
	ThisFogRGBA[0]=Blending8[Blend.L.Index];
	Blend.B.Color=FogRGBA[1];
	ThisFogRGBA[1]=Blending8[Blend.L.Index];
	Blend.B.Color=FogRGBA[2];
	ThisFogRGBA[2]=Blending8[Blend.L.Index];
	ThisFogRGBA[3]=255-ThisFogRGBA[3];

	if(Wazp3D.UseFog.ON)
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("[%ld] %ld %ld %ld %ld\n",n,ThisFogRGBA[0],ThisFogRGBA[1],ThisFogRGBA[2],ThisFogRGBA[3]);
	}

}
/*================================================================*/
void DrawPolyP(struct SOFT3D_context *SC)
{
union   pixel3D *PIX=SC->PolyPIX;
struct point3D *P=SC->PolyP;
register union blend3D Blend;
register UBYTE *Blending8=SC->Blending8;
float PolyZmin,PolyZmax;
float resizeuv=254.0;
float resizez=ZMAX;
BOOL BlendColor=FALSE;
WORD Pnb,n;
BOOL FogThisFace;
BOOL BackFace;

SFUNCTION(DrawPolyP)
if(Wazp3D.DebugSOFT3D.ON) Libprintf("DrawPolyP Pnb %ld  \n",SC->PolyPnb);

	if(SC->PolyPnb > 64 )	{REM(Poly has too much Points!); return;}

if(Wazp3D.UseClipper.ON)
if(SC->PolyPnb > 1)
	ClipPoly(SC);

	Blend.L.Index=0;

	Pnb=SC->PolyPnb;
	P  =SC->PolyP;

	if(Pnb>=3)
		BackFace=! (0.0 > (P[1].x - P[0].x)*(P[2].y - P[0].y)-(P[2].x - P[0].x)*(P[1].y - P[0].y));

	if(Wazp3D.DebugSOFT3D.ON) Libprintf("Backface %ld Culling %ld\n",BackFace,SC->Culling);

	if(Wazp3D.UseCullingHack.ON)
	if(BackFace==TRUE)
		{if(Wazp3D.DebugSOFT3D.ON) Libprintf("Back face removed Pnb %ld\n",Pnb); return;}

	if(SC->Culling==1)
	if(BackFace==TRUE)
		{if(Wazp3D.DebugSOFT3D.ON) Libprintf("Back face removed Pnb %ld\n",Pnb); return;}

	if(SC->Culling==2)
	if(BackFace==FALSE)
		{if(Wazp3D.DebugSOFT3D.ON) Libprintf("Front face removed Pnb %ld\n",Pnb); return;}


	memset(PIX,0,Pnb*sizeof(union pixel3D));

/*
	if(SC->DrawMode==COLOR32ZOFF)
		BlendColor=TRUE;
	if(SC->DrawMode==COLOR32)
		BlendColor=TRUE;
*/


	if((Wazp3D.HackRGBA1.ON) ou (Wazp3D.HackRGBA2.ON))
	{
	if(SC->ST->bits==24)
		SOFT3D_SetDrawMode(SC,SC->ST,COLOR24);
	if(SC->ST->bits==32)
		SOFT3D_SetDrawMode(SC,SC->ST,COLOR32);
	}


if(Wazp3D.HackRGBA2.ON)		/* todo: here should use FLAT24 */
if(Pnb>=3)
		UVtoRGBA(SC->ST,(P[0].u+P[1].u+P[2].u)/3.0,(P[0].v+P[1].v+P[2].v)/3.0,SC->FlatRGBA);

	PolyZmin=1.0; PolyZmax=0.0;

	NLOOP(Pnb)
	{
	if(0.999 < P->u) P->u=0.999;
	if(0.999 < P->v) P->v=0.999;
	if(P->u  < 0.0 ) P->u=0.0;
	if(P->v  < 0.0 ) P->v=0.0;

	if (P->z < 0.0)	P->z=0.0;
	if (0.999 < P->z)	P->z=0.999;
	if (P->z < PolyZmin)	PolyZmin=P->z;
	if (PolyZmax < P->z)	PolyZmax=P->z;

	if(Wazp3D.HackRGBA1.ON)
		UVtoRGBA(SC->ST,P->u,P->v,P->RGBA);

	if(Wazp3D.HackRGBA2.ON)
		COPYRGBA(P->RGBA,SC->FlatRGBA);

	PIX->W.x	= P->x;
	PIX->W.y	= P->y;
	PIX->W.z	= P->z *  resizez;
	PIX->W.u	= P->u *  resizeuv;
	PIX->W.v	= P->v *  resizeuv;
	PIX->W.R	= P->RGBA[0];
	PIX->W.G	= P->RGBA[1];
	PIX->W.B	= P->RGBA[2];
	PIX->W.A	= P->RGBA[3];

/* update drawed area */
	if(PIX->W.x < SC->Pxmin) SC->Pxmin=PIX->W.x;
	if(SC->Pxmax < PIX->W.x) SC->Pxmax=PIX->W.x;
	if(PIX->W.y < SC->Pymin) SC->Pymin=PIX->W.y;
	if(SC->Pymax < PIX->W.y) SC->Pymax=PIX->W.y;

	if(PIX->W.x==SC->large) P->x--;
	if(PIX->W.y==SC->high ) P->y--;

	if(BlendColor)
	{
	Blend.B.Alpha=PIX->W.A;
	Blend.B.Color=PIX->W.R;
	PIX->W.R=Blending8[Blend.L.Index];
	Blend.B.Color=PIX->W.G;
	PIX->W.G=Blending8[Blend.L.Index];
	Blend.B.Color=PIX->W.B;
	PIX->W.B=Blending8[Blend.L.Index];
	PIX->W.A=255-PIX->W.A;
	}

	PIX++;
	P++;
	}

/* This face is inside Fog ? */
	FogThisFace=TRUE;
	if(SC->FogMode==0)	FogThisFace=FALSE;
	if(SC->FogMode==1)
	{
	if(PolyZmax < SC->FogZmin) FogThisFace=FALSE;
	if(SC->FogZmax   < PolyZmin) FogThisFace=FALSE;	  /* end of fogging area */
	}

	if(!Wazp3D.UseFog.ON) FogThisFace=FALSE;

REMP("FogMode %ld FogZmin %ld FogZmax %ld  PolyZmin %ld PolyZmax %ld --> FogThisFace %ld \n",SC->FogMode,(WORD)(1024.0*SC->FogZmin),(WORD)(1024.0*SC->FogZmax),(WORD)(1024.0*PolyZmin),(WORD)(1024.0*PolyZmax),FogThisFace);

	if(SC->FogEnabled!=FogThisFace)	/* if fog disabled for this face change drawmode */
	{
	SC->FogEnabled=FogThisFace;
	SOFT3D_SetDrawMode(SC,SC->ST, SC->DrawMode);
	}

	switch(Pnb)
	{
	case 1:  DrawPointPIX(SC); break;
	case 2:  DrawLinePIX(SC);  break;
	default: DrawPolyPIX(SC);  break;
	}

}
/*=============================================================*/
void DrawTriP(struct SOFT3D_context *SC,register struct point3D *A,register struct point3D *B,register struct point3D *C)
{
	Libmemcpy(&(SC->PolyP[0]),A,24);
	Libmemcpy(&(SC->PolyP[1]),B,24);
	Libmemcpy(&(SC->PolyP[2]),C,24);
	SC->PolyPnb=3;
	DrawPolyP(SC);
}
/*==========================================================================*/
void UVtoRGBA(struct SOFT3D_texture *ST,float u,float v,UBYTE *RGBA)
{
register UBYTE *pt;
register ULONG offset;
register WORD x,y;

	RGBA[0]=RGBA[1]=RGBA[2]=RGBA[3]=0;
	if(ST==NULL) return;
PrintST(ST);
VARF(u)
VARF(v)
	x=(WORD)(((float)ST->large) * u);
	y=(WORD)(((float)ST->high ) * v);
	offset=(ST->large*y + x)  * ST->bits / 8;
	pt=&(ST->pt[offset]);
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("UVtoRGBA x y %ld %ld --> %ld",x,y,offset);
	RGBA[0]=pt[0];
	RGBA[1]=pt[1];
	RGBA[2]=pt[2];
	RGBA[3]=255; if (ST->bits==32) RGBA[3]=pt[3];
	if(Wazp3D.DebugSOFT3D.ON) PrintRGBA((UBYTE *)RGBA);
}
/*================================================================*/
void SOFT3D_DrawPrimitive(struct SOFT3D_context *SC,struct point3D *P,LONG Pnb,ULONG primitive)
{
WORD n,nb;
#define W3D_PRIMITIVE_POLYGON	9999

	SFUNCTION(SOFT3D_DrawPrimitive)
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("SOFT3D_DrawPrimitive Pnb %ld (%ld)\n",Pnb,primitive);

	SINFO(primitive,W3D_PRIMITIVE_TRIANGLES)
	SINFO(primitive,W3D_PRIMITIVE_TRIFAN)
	SINFO(primitive,W3D_PRIMITIVE_TRISTRIP)
	SINFO(primitive,W3D_PRIMITIVE_POINTS)
	SINFO(primitive,W3D_PRIMITIVE_LINES)
	SINFO(primitive,W3D_PRIMITIVE_LINELOOP)
	SINFO(primitive,W3D_PRIMITIVE_LINESTRIP)
	SINFO(primitive,W3D_PRIMITIVE_POLYGON)

	if(primitive==W3D_PRIMITIVE_TRIANGLES)
	{
	nb=Pnb/3;
	NLOOP(nb)
		DrawTriP(SC,&P[3*n+0],&P[3*n+1],&P[3*n+2]);
	}

	if(Wazp3D.UsePolyHack.ON)
	if(Pnb==4)
	{
	if(primitive==W3D_PRIMITIVE_TRIFAN)
		primitive=W3D_PRIMITIVE_POLYGON;

	if(primitive==W3D_PRIMITIVE_TRISTRIP)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[0],24);
		Libmemcpy(&(SC->PolyP[1]),&P[2],24);
		Libmemcpy(&(SC->PolyP[2]),&P[3],24);
		Libmemcpy(&(SC->PolyP[3]),&P[1],24);
		SC->PolyPnb=4;
		DrawPolyP(SC);
		return;
		}
	}

	if(primitive==W3D_PRIMITIVE_TRIFAN)
	{
	for (n=2;n<Pnb;n++)
		DrawTriP(SC,&P[0],&P[n-1],&P[n]);
	return;
	}

	if(primitive==W3D_PRIMITIVE_TRISTRIP)
	{
	for (n=2;n<Pnb;n++)
	if (n&1)	  /* reverse vertex order */
		DrawTriP(SC,&P[n-1],&P[n-2],&P[n-0]);
	else
		DrawTriP(SC,&P[n-2],&P[n-1],&P[n-0]);
	return;
	}

	if(primitive==W3D_PRIMITIVE_POINTS)
	NLOOP(Pnb)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[n],24);
		SC->PolyPnb=1;
		DrawPolyP(SC);
		return;
		}


	if(primitive==W3D_PRIMITIVE_LINES)
	{
	nb=Pnb/2;
	NLOOP(nb)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[2*n],24*2);
		SC->PolyPnb=2;
		DrawPolyP(SC);
		}
	return;
	}

	if(primitive==W3D_PRIMITIVE_LINELOOP)
	{
	nb=Pnb-1;
	NLOOP(nb)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[n],24*2);
		SC->PolyPnb=2;
		DrawPolyP(SC);
		}

	Libmemcpy(&(SC->PolyP[0]),&P[nb],24);
	Libmemcpy(&(SC->PolyP[1]),&P[0 ],24);
	SC->PolyPnb=2;
	DrawPolyP(SC);
	return;
	}

	if(primitive==W3D_PRIMITIVE_LINESTRIP)
	{
	nb=Pnb-1;
	NLOOP(nb)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[n],24*2);
		SC->PolyPnb=2;
		DrawPolyP(SC);
		}
	return;
	}

	if(primitive==W3D_PRIMITIVE_POLYGON	)
	{
	SC->PolyPnb=Pnb;
	Libmemcpy(SC->PolyP,P,Pnb*24);
	DrawPolyP(SC);
	}
}
/*=============================================================*/
void SOFT3D_SetPointSize(struct SOFT3D_context *SC,UWORD PointSize)
{
register WORD *PointLarge=SC->PointLarge;
register WORD x,y,R;

SFUNCTION(SOFT3D_SetPointSize)
	if(PointSize!=SC->PointSize)
	{
	VAR(PointSize);
	R=PointSize/2;
	YLOOP(PointSize)
	XLOOP(R)
	if( ( (y-R)*(y-R) + x*x) < R*R ) PointLarge[y]=x*2;
	SC->PointSize=PointSize;
	}
}
/*=============================================================*/
void SOFT3D_SetDrawMode(struct SOFT3D_context *SC,void *texture,WORD DrawMode)
{
struct SOFT3D_texture *ST=texture;

SFUNCTION(SOFT3D_SetDrawMode)

/*
	if(SC->ST==ST)
	if(SC->DrawMode==DrawMode)
		return;
*/

	if(DrawMode!=NODRAW)
	if(ST==NULL)
		DrawMode=COLOR24;

	if(ST!=NULL)  PrintST(ST);

	if(SC->PixBuffer < SC->PixBufferDone)  SC->PixelsFunction(SC);

	SC->DrawMode=DrawMode;
	SC->ST=ST;

REMP("DrawMode%ld FogEnabled%ld TexMode%ld -> ",SC->DrawMode,SC->FogEnabled,SC->ST->TexMode,SC->ST->TexMode);
	if(DrawMode==NODRAW)
		{
		REM(NODRAW)
		SC->FillFunction=  (HOOKEDFUNCTION)Pixels_Nothing;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}

	if(DrawMode==FLAT24)
		{
		REM(FLAT24)
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zwrite_Flat24;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}

	if(DrawMode==COLOR24ZOFF)
		{
		REM(COLOR24ZOFF)
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zoff_Color24;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}

	if(DrawMode==COLOR24)
		{
		REM(COLOR24)
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zwrite_Color24;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}

	if(DrawMode==COLOR32ZOFF)
		{
		REM(COLOR32ZOFF)
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zoff_Color32;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}

	if(DrawMode==COLOR32)
		{
		REM(COLOR32)
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Ztest_Color32;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		return;
		}



	if(DrawMode==TEX32)
	{
	REM(TEX32)
	if(SC->FogEnabled==FALSE)
		{
		if(SC->ST->TexMode=='A')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)Fill_Ztest_Tex;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex32A;
			}
		if(SC->ST->TexMode=='B')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)Fill_Ztest_Tex;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex32B;
			}
		if(SC->ST->TexMode=='a')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Tex32a;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
			}

		}
	else		/* if fog */
		{
		if(SC->ST->TexMode=='A')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)Fill_Ztest_Tex_Fog;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex32A_Fog;
			}
		if(SC->ST->TexMode=='B')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)Fill_Ztest_Tex_Fog;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex32B_Fog;
			}
		if(SC->ST->TexMode=='a')
			{
			SC->FillFunction=  (HOOKEDFUNCTION)Fill_Ztest_Tex_Fog;
			SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex32a_Fog;
			}
		}
	}


	if(DrawMode==TEX24ZOFF)
	{
	REM(TEX24ZOFF)
	if(SC->FogEnabled==FALSE)
		{
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zoff_Tex24;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		}
	else	/* if fog */
		{
		SC->FillFunction=  (HOOKEDFUNCTION)Fill_Zoff_Tex_Fog;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex24_Fog;
		}
	}


	if(DrawMode==TEX24)
	{
	REM(TEX24)
	if(SC->FogEnabled==FALSE)
		{
		SC->FillFunction=  (HOOKEDFUNCTION)FillPixels_Zwrite_Tex24;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Nothing;
		}
	else	/* if fog */
		{
		SC->FillFunction=  (HOOKEDFUNCTION)Fill_Zwrite_Tex_Fog;
		SC->PixelsFunction=(HOOKEDFUNCTION)Pixels_Tex24_Fog;
		}
	}
}
/*=================================================================*/
void TextureAlphaToRGB(struct SOFT3D_context *SC,struct SOFT3D_texture *ST)
{
ULONG x,y;
UBYTE *RGBA;
UBYTE *RGBA2;
register UBYTE *Blending8=SC->Blending8;
register union blend3D Blend;

SFUNCTION(TextureAlphaToRGB)
	if(ST->bits!=32) return;
	if(ST->TexMode!='A') return;

	ST->pt2   =MYmalloc(ST->large*ST->high*32/8,"Texture converted");
	if(ST->pt2==NULL) return;

	ST->TexMode='B';
	RGBA = ST->pt;
	RGBA2= ST->pt2;
	Blend.L.Index=0;

	YLOOP(ST->high)
	XLOOP(ST->large)
	{
	Blend.B.Alpha=RGBA[alpha];

	RGBA2[alpha]=RGBA[alpha];

	Blend.B.Color=RGBA[0];
	RGBA2[0]=Blending8[Blend.L.Index];

	Blend.B.Color=RGBA[1];
	RGBA2[1]=Blending8[Blend.L.Index];

	Blend.B.Color=RGBA[2];
	RGBA2[2]=Blending8[Blend.L.Index];

	RGBA +=4;
	RGBA2+=4;
	}
}
/*=================================================================*/
void TextureAlphaUsage(struct SOFT3D_texture *ST)
{
ULONG size,n,AllAnb,RatioA;
ULONG Anb[256];
UBYTE *RGBA;

SFUNCTION(TextureAlphaUsage)
	if(ST->bits!=32) {ST->TexMode='T';return;}

	NLOOP(256)
		Anb[n]=0;

	size=ST->large*ST->high;
	RGBA=ST->pt;
	NLOOP(size)
		{Anb[RGBA[alpha]]++; RGBA+=4;}

	AllAnb=size-Anb[0]-Anb[255];
	if(AllAnb!=0) RatioA=(100*AllAnb)/size; else RatioA=0;

	if(RatioA<25)
		ST->TexMode='a';
	else
		ST->TexMode='A';

	if(Anb[0]   == size) ST->TexMode='T';
	if(Anb[255] == size) ST->TexMode='T';

	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("T [%ld] %ld bytes(A0:%ld A255:%ld) AllAnb %ld RatioA %ld %\n",ST->TexMode,size,Anb[0],Anb[255],AllAnb,RatioA);
}
/*=============================================================*/
void mipmap_index(void *texture)
{
struct SOFT3D_texture *ST=texture;
ULONG *TindexX=ST->TindexX;
ULONG *TindexY=ST->TindexY;
WORD rx,ry,m,n,xbytes,ybytes;
ULONG xval,yval;
UBYTE *pt;
ULONG size;
UWORD large;

SFUNCTION(mipmap_index)

	if(ST->large <= 256)
		{rx=256/ST->large;	xbytes=ST->bits/8;}
	else
		{rx=1;	xbytes=ST->large/256 * ST->bits/8;}

	if(ST->high <= 256)
		{ry=256/ST->high;	ybytes=ST->large*ST->bits/8;}
	else
		{ry=1;	ybytes=ST->high/256  * ST->large*ST->bits/8;}

	pt=ST->pt;
	size=ST->large * ST->high * ST->bits / 8;

next_mipmap:
	xval=(ULONG)0 ;
	yval=(ULONG)pt;

	MLOOP(256)
		{
		NLOOP(rx)
			*TindexX++ = xval;
		xval+=xbytes;
		}

	MLOOP(256)
		{
		NLOOP(ry)
			*TindexY++ = yval;
		yval+=ybytes;
		}

	rx=rx*2;
	ry=ry*2;
	ybytes=ybytes/2;
	large=large/2;
	size=size/4;
	pt=&(pt[size]);

	if (ST->mipmapped==1)
	if (large>0) goto next_mipmap;

}
/*==================================================================*/
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,UWORD DrawMode,BOOL GotMipmaps)
{
struct SOFT3D_texture *ST;
ULONG *TindexX;
ULONG *TindexY;
WORD rx,ry,m,n,xbytes,ybytes;
ULONG xval,yval;
WORD x,y;
UBYTE *RGB;
LONG grey;

SFUNCTION(SOFT3D_CreateTexture)
	ST=MYmalloc(sizeof(struct SOFT3D_texture),"SOFT3D_texture");
	if(ST==NULL) return(NULL);

	ST->pt	=pt;
	ST->large	=large;
	ST->high	=high;
	ST->bits	=bits;
	ST->DrawMode=DrawMode;
	ST->TexMode	='?';
/* add to linkage */
	ST->nextST =SC->firstST;
	SC->firstST=ST;

	TindexX=(ULONG *)&ST->TindexX;
	TindexY=(ULONG *)&ST->TindexY;

	if(ST->TexMode=='?')	 TextureAlphaUsage(ST);
/*	if(ST->TexMode=='A')	TextureAlphaToRGB(SC,ST); */

REM(Create tex index)
	if(ST->large <= 256)
		{rx=256/ST->large;	xbytes=ST->bits/8;}
	else
		{rx=1;	xbytes=ST->large/256 * ST->bits/8;}

	if(ST->high <= 256)
		{ry=256/ST->high;	ybytes=ST->large*ST->bits/8;}
	else
		{ry=1;	ybytes=ST->high/256  * ST->large*ST->bits/8;}

	xval=0;
	yval=(ULONG) ST->pt;

	MLOOP(256/rx)
		{
		NLOOP(rx)
			*TindexX++ = xval;
		xval+=xbytes;
		}

	MLOOP(256/ry)
		{
		NLOOP(ry)
			*TindexY++ = yval;
		yval+=ybytes;
		}


	RGB=ST->pt;
	if (Wazp3D.DebugST.ON)
	YLOOP(32)
	{
	XLOOP(ST->large)
		{
		grey= RGB[0] + RGB[1] + RGB[2];
		Libprintf("%ld",grey/(3*256/8));
		RGB+=ST->bits/8;
		}
	Libprintf("|\n");
	}
	PrintST(ST);

	if (Wazp3D.DebugST.ON)
		LibAlert("Texture done");
	return( (void *) ST);
}
/*==================================================================*/
void SOFT3D_FreeTexture(struct SOFT3D_context *SC,void *texture)
{
struct SOFT3D_texture *ST=texture;
struct SOFT3D_texture fakeST;
struct SOFT3D_texture *thisST=&fakeST;
WORD Ntexture=0;

	WAZP3D_Function(24);
	PrintST(ST);

	thisST->nextST=SC->firstST;
	while(thisST!=NULL)
	{
	VAL(Ntexture)
	if(thisST->nextST==ST)
		{
		REM(is Texture found)
		if(thisST->nextST==SC->firstST)
			SC->firstST=ST->nextST;
		else
			thisST->nextST=ST->nextST;
		MYfree(ST->pt2);
		MYfree(ST);
		}
	thisST=thisST->nextST;
	Ntexture++;
	}
}
/*=================================================================*/
ULONG PrintError(ULONG error)
{
	if (!Wazp3D.DebugError.ON) return(error);
	ERROR(W3D_SUCCESS,"success ")
	ERROR(W3D_BUSY,"graphics hardware is busy ")
	ERROR(W3D_ILLEGALINPUT,"failure,illegal input ")
	ERROR(W3D_NOMEMORY,"no memory available ")
	ERROR(W3D_NODRIVER,"no hardware driver available ")
	ERROR(W3D_NOTEXTURE,"texture is missing ")
	ERROR(W3D_TEXNOTRESIDENT,"texture not resident ")
	ERROR(W3D_NOMIPMAPS,"mipmaps are not supported by this texture object")
	ERROR(W3D_NOGFXMEM,"no graphics memory available ")
	ERROR(W3D_NOTVISIBLE,"drawing area not or bitmap swapped out")
	ERROR(W3D_UNSUPPORTEDFILTER,"unsupported filter ")
	ERROR(W3D_UNSUPPORTEDTEXENV,"unsupported filter ")
	ERROR(W3D_UNSUPPORTEDWRAPMODE,"unsupported wrap mode ")
	ERROR(W3D_UNSUPPORTEDZCMP,"unsupported Z compare mode ")
	ERROR(W3D_UNSUPPORTEDATEST,"unsupported alpha test ")
	ERROR(W3D_UNSUPPORTEDBLEND,"unsupported blending function ")
	ERROR(W3D_UNSUPPORTEDFOG,"unsupported fog mode ")
	ERROR(W3D_UNSUPPORTEDSTATE,"can't enable/disable state ")
	ERROR(W3D_UNSUPPORTEDFMT,"unsupported bitmap format ")
	ERROR(W3D_UNSUPPORTEDTEXSIZE,"unsupported texture border size ")
	ERROR(W3D_UNSUPPORTEDLOGICOP,"unsupported logical operation ")
	ERROR(W3D_UNSUPPORTEDSTTEST,"unsupported stencil test ")
	ERROR(W3D_ILLEGALBITMAP,"illegal bitmap ")
	ERROR(W3D_NOZBUFFER,"Zbuffer is missing or unavailable ")
	ERROR(W3D_NOPALETTE,"Palette missing for chunky textures")
	ERROR(W3D_MASKNOTSUPPORTED,"color/index masking not supported ")
	ERROR(W3D_NOSTENCILBUFFER,"Stencil buffer is missing or unavailable ")
	ERROR(W3D_QUEUEFAILED,"The request can not be queued ")
	ERROR(W3D_UNSUPPORTEDTEXFMT,"Texformat unsupported ")
	ERROR(W3D_WARNING,"Partial success ")
	ERROR(W3D_UNSUPPORTED,"Requested feature is unsupported ")
	ERROR(W3D_H_TEXMAPPING,"Quality of general texture mapping")
	ERROR(W3D_H_MIPMAPPING,"Quality of mipmapping ")
	ERROR(W3D_H_BILINEARFILTER,"Quality of bilinear filtering ")
	ERROR(W3D_H_MMFILTER,"Quality of depth filter ")
	ERROR(W3D_H_PERSPECTIVE,"Quality of perspective correction ")
	ERROR(W3D_H_BLENDING,"Quality of alpha blending ")
	ERROR(W3D_H_FOGGING,"Quality of fogging ")
	ERROR(W3D_H_ANTIALIASING,"Quality of antialiasing ")
	ERROR(W3D_H_DITHERING,"Quality of dithering ")
	ERROR(W3D_H_ZBUFFER,"Quality of zbuffering ")
	ERROR(W3D_H_POINTDRAW," ")
	return(error);
}
/*==========================================================================*/
void PrintDriver(W3D_Driver *D)
{
UBYTE drivertype[10];

	if(!Wazp3D.DebugDriver.ON) return;
	if(D->swdriver==W3D_TRUE)	Libstrcpy(drivertype,"Software");
	if(D->swdriver==W3D_FALSE)	Libstrcpy(drivertype,"Hardware");

	Libprintf("%s [%s]\n",D->name,drivertype);
	WINFO(D->ChipID,W3D_CHIP_UNKNOWN," ");
	WINFO(D->ChipID,W3D_CHIP_VIRGE," ");
	WINFO(D->ChipID,W3D_CHIP_PERMEDIA2," ");
	WINFO(D->ChipID,W3D_CHIP_VOODOO1," ");
	WINFO(D->ChipID,W3D_CHIP_AVENGER_LE,"Voodoo 3 LE= Little endian mode")
	WINFO(D->ChipID,W3D_CHIP_AVENGER_BE,"Voodoo 3 BE= Big endian mode ")
	WINFO(D->ChipID,W3D_CHIP_PERMEDIA3," ");
	WINFO(D->ChipID,W3D_CHIP_RADEON," ");
	WINFO(D->ChipID,W3D_CHIP_RADEON2," ");

	Libprintf("support formats:\n");
	WINFOB(D->formats,W3D_FMT_CLUT,"chunky ")
	WINFOB(D->formats,W3D_FMT_R5G5B5,"0 rrrrr ggggg bbbbb ")
	WINFOB(D->formats,W3D_FMT_B5G5R5,"0 bbbbb ggggg rrrrr ")
	WINFOB(D->formats,W3D_FMT_R5G5B5PC,"ggg bbbbb 0 rrrrr gg ")
	WINFOB(D->formats,W3D_FMT_B5G5R5PC,"ggg rrrrr 0 bbbbb gg ")
	WINFOB(D->formats,W3D_FMT_R5G6B5,"rrrrr gggggg bbbbb ")
	WINFOB(D->formats,W3D_FMT_B5G6R5,"bbbbb gggggg rrrrr ")
	WINFOB(D->formats,W3D_FMT_R5G6B5PC,"ggg bbbbb rrrrr ggg ")
	WINFOB(D->formats,W3D_FMT_B5G6R5PC,"ggg rrrrr bbbbb ggg ")
	WINFOB(D->formats,W3D_FMT_R8G8B8,"rrrrrrrr gggggggg bbbbbbbb ")
	WINFOB(D->formats,W3D_FMT_B8G8R8,"bbbbbbbb gggggggg rrrrrrrr ")
	WINFOB(D->formats,W3D_FMT_A8R8G8B8,"aaaaaaaa rrrrrrrr gggggggg bbbbbbbb ")
	WINFOB(D->formats,W3D_FMT_A8B8G8R8,"aaaaaaaa bbbbbbbb gggggggg rrrrrrrr ")
	WINFOB(D->formats,W3D_FMT_R8G8B8A8,"rrrrrrrr gggggggg bbbbbbbb aaaaaaaa ")
	WINFOB(D->formats,W3D_FMT_B8G8R8A8,"bbbbbbbb gggggggg rrrrrrrr aaaaaaaa ")
}
/*==========================================================================*/
void PrintContext(W3D_Context *C)
{
WORD n;

	if(!Wazp3D.DebugContext.ON) return;
	VAR(C->driver)
	VAR(C->gfxdriver)
	VAR(C->drivertype)
	VAR(C->regbase)
	VAR(C->vmembase)
	VAR(C->zbuffer)
	VAR(C->stencilbuffer)
	VAR(C->state)
	VAR(C->drawregion)
	VAR(C->supportedfmt)
	VAR(C->format)
	VAR(C->yoffset)
	VAR(C->bprow)
	VAR(C->width)
	VAR(C->height)
	VAR(C->depth)
	VAR(C->chunky)
	VAR(C->destalpha)
	VAR(C->zbufferalloc)
	VAR(C->stbufferalloc)
	VAR(C->HWlocked)
	VAR(C->w3dbitmap)
	VAR(C->zbufferlost)
	VAR(C->reserved3)
	VAR(C->restex)
	VAR(C->tex)
	VAR(C->maxtexwidth)
	VAR(C->maxtexheight)
	VAR(C->maxtexwidthp)
	VAR(C->maxtexheightp)
	VAR(C->scissor.left)
	VAR(C->scissor.top)
	VAR(C->scissor.width)
	VAR(C->scissor.height)

	VARF(C->fog.fog_start)
	VARF(C->fog.fog_end)
	VARF(C->fog.fog_density)
	VARF(C->fog.fog_color.r)
	VARF(C->fog.fog_color.g)
	VARF(C->fog.fog_color.b)
	VAR(C->envsupmask)
	VAR(C->queue)
	VAR(C->drawmem)
	VAR(C->globaltexenvmode)
	VARF(C->globaltexenvcolor[0])
	VARF(C->globaltexenvcolor[1])
	VARF(C->globaltexenvcolor[2])
	VARF(C->globaltexenvcolor[3])
	VAR(C->DriverBase)
	VAR(C->EnableMask)
	VAR(C->DisableMask)
	VAR(C->CurrentChip)
	VAR(C->DriverVersion)

	VAR(C->VertexPointer)
	VAR(C->VPStride)
	VAR(C->VPMode)
	VAR(C->VPFlags)
	NLOOP(W3D_MAX_TMU)
	{
	VAR(C->TexCoordPointer[n])
	VAR(C->TPStride[n])
	VAR(C->CurrentTex[n])
	VAR(C->TPVOffs[n])
	VAR(C->TPWOffs[n])
	VAR(C->TPFlags[n])
	}
	VAR(C->ColorPointer)
	VAR(C->CPStride)
	VAR(C->CPMode)
	VAR(C->CPFlags)
	VAR(C->FrontFaceOrder)
	VAR(C->specialbuffer)
}
/*==========================================================================*/
void PrintTexture(W3D_Texture *T)
{
WORD n;

	if(!Wazp3D.DebugTexture.ON) return;
	VAR(T->link)
	VAR(T->resident)
	VAR(T->mipmap)
	VAR(T->dirty)
	VAR(T->matchfmt)
	VAR(T->reserved1)
	VAR(T->reserved2)
	VAR(T->mipmapmask)
	VAR(T->texsource)
	NLOOP(16)
		VAR(T->mipmaps[n])
	VAR(T->texfmtsrc)
	VAR(T->palette)
	VAR(T->texdata)
	VAR(T->texdest)
	VAR(T->texdestsize)
	VAR(T->texwidth)
	VAR(T->texwidthexp)
	VAR(T->texheight)
	VAR(T->texheightexp)
	VAR(T->bytesperpix)
	VAR(T->bytesperrow)
	VAR(T->driver)
}
/*==================================================================================*/
void WAZP3D_Init()
{
WORD n;
#define TRUECOLORFORMATS W3D_FMT_R5G5B5+W3D_FMT_B5G5R5+W3D_FMT_R5G5B5PC+W3D_FMT_B5G5R5PC+W3D_FMT_R5G6B5+W3D_FMT_B5G6R5+W3D_FMT_R5G6B5PC+W3D_FMT_B5G6R5PC+W3D_FMT_R8G8B8+W3D_FMT_B8G8R8+W3D_FMT_A8R8G8B8+W3D_FMT_A8B8G8R8+W3D_FMT_R8G8B8A8+W3D_FMT_B8G8R8A8

	REM(WAZP3D_Init)
	firstME=NULL;	/* Tracked memory-allocation	*/

	NLOOP(88)
		{
		Libstrcpy(Wazp3D.FunctionName[1+n],"W3D_");
		Wazp3D.FunctionCalls[1+n]=0;
		}
	Wazp3D.FunctionCallsAll=0;

	Libstrcat(Wazp3D.FunctionName[ 1],"CreateContext");
	Libstrcat(Wazp3D.FunctionName[ 2],"CreateContextTags");
	Libstrcat(Wazp3D.FunctionName[ 3],"DestroyContext");
	Libstrcat(Wazp3D.FunctionName[ 4],"GetState");
	Libstrcat(Wazp3D.FunctionName[ 5],"SetState");
	Libstrcat(Wazp3D.FunctionName[ 6],"Hint");
	Libstrcat(Wazp3D.FunctionName[ 7],"CheckDriver");
	Libstrcat(Wazp3D.FunctionName[ 8],"LockHardware");
	Libstrcat(Wazp3D.FunctionName[ 9],"UnLockHardware");
	Libstrcat(Wazp3D.FunctionName[10],"WaitIdle");
	Libstrcat(Wazp3D.FunctionName[11],"CheckIdle");
	Libstrcat(Wazp3D.FunctionName[12],"Query");
	Libstrcat(Wazp3D.FunctionName[13],"GetTexFmtInfo");
	Libstrcat(Wazp3D.FunctionName[14],"GetDriverState");
	Libstrcat(Wazp3D.FunctionName[15],"GetDestFmt");
	Libstrcat(Wazp3D.FunctionName[16],"GetDrivers");
	Libstrcat(Wazp3D.FunctionName[17],"QueryDriver");
	Libstrcat(Wazp3D.FunctionName[18],"GetDriverTexFmtInfo");
	Libstrcat(Wazp3D.FunctionName[19],"RequestMode");
	Libstrcat(Wazp3D.FunctionName[20],"RequestModeTags");
	Libstrcat(Wazp3D.FunctionName[21],"TestMode");
	Libstrcat(Wazp3D.FunctionName[22],"AllocTexObj");
	Libstrcat(Wazp3D.FunctionName[23],"AllocTexObjTags");
	Libstrcat(Wazp3D.FunctionName[24],"FreeTexObj");
	Libstrcat(Wazp3D.FunctionName[25],"ReleaseTexture");
	Libstrcat(Wazp3D.FunctionName[26],"FlushTextures");
	Libstrcat(Wazp3D.FunctionName[27],"SetFilter");
	Libstrcat(Wazp3D.FunctionName[28],"SetTexEnv");
	Libstrcat(Wazp3D.FunctionName[29],"SetWrapMode");
	Libstrcat(Wazp3D.FunctionName[30],"UpdateTexImage");
	Libstrcat(Wazp3D.FunctionName[31],"UpdateTexSubImage");
	Libstrcat(Wazp3D.FunctionName[32],"UploadTexture");
	Libstrcat(Wazp3D.FunctionName[33],"FreeAllTexObj");
	Libstrcat(Wazp3D.FunctionName[34],"SetChromaTestBounds");
	Libstrcat(Wazp3D.FunctionName[35],"DrawLine");
	Libstrcat(Wazp3D.FunctionName[36],"DrawPoint");
	Libstrcat(Wazp3D.FunctionName[37],"DrawTriangle");
	Libstrcat(Wazp3D.FunctionName[38],"DrawTriFan");
	Libstrcat(Wazp3D.FunctionName[39],"DrawTriStrip");
	Libstrcat(Wazp3D.FunctionName[40],"Flush");
	Libstrcat(Wazp3D.FunctionName[41],"DrawLineStrip");
	Libstrcat(Wazp3D.FunctionName[42],"DrawLineLoop");
	Libstrcat(Wazp3D.FunctionName[43],"ClearDrawRegion");
	Libstrcat(Wazp3D.FunctionName[44],"SetAlphaMode");
	Libstrcat(Wazp3D.FunctionName[45],"SetBlendMode");
	Libstrcat(Wazp3D.FunctionName[46],"SetDrawRegion");
	Libstrcat(Wazp3D.FunctionName[47],"SetDrawRegionWBM");
	Libstrcat(Wazp3D.FunctionName[48],"SetFogParams");
	Libstrcat(Wazp3D.FunctionName[49],"SetLogicOp");
	Libstrcat(Wazp3D.FunctionName[50],"SetColorMask");
	Libstrcat(Wazp3D.FunctionName[51],"SetPenMask");
	Libstrcat(Wazp3D.FunctionName[52],"SetCurrentColor");
	Libstrcat(Wazp3D.FunctionName[53],"SetCurrentPen");
	Libstrcat(Wazp3D.FunctionName[54],"SetScissor");
	Libstrcat(Wazp3D.FunctionName[55],"FlushFrame");
	Libstrcat(Wazp3D.FunctionName[56],"AllocZBuffer");
	Libstrcat(Wazp3D.FunctionName[57],"FreeZBuffer");
	Libstrcat(Wazp3D.FunctionName[58],"ClearZBuffer");
	Libstrcat(Wazp3D.FunctionName[59],"ReadZPixel");
	Libstrcat(Wazp3D.FunctionName[60],"ReadZSpan");
	Libstrcat(Wazp3D.FunctionName[61],"SetZCompareMode");
	Libstrcat(Wazp3D.FunctionName[62],"WriteZPixel");
	Libstrcat(Wazp3D.FunctionName[63],"WriteZSpan");
	Libstrcat(Wazp3D.FunctionName[64],"AllocStencilBuffer");
	Libstrcat(Wazp3D.FunctionName[65],"ClearStencilBuffer");
	Libstrcat(Wazp3D.FunctionName[66],"FillStencilBuffer");
	Libstrcat(Wazp3D.FunctionName[67],"FreeStencilBuffer");
	Libstrcat(Wazp3D.FunctionName[68],"ReadStencilPixel");
	Libstrcat(Wazp3D.FunctionName[69],"ReadStencilSpan");
	Libstrcat(Wazp3D.FunctionName[70],"SetStencilFunc");
	Libstrcat(Wazp3D.FunctionName[71],"SetStencilOp");
	Libstrcat(Wazp3D.FunctionName[72],"SetWriteMask");
	Libstrcat(Wazp3D.FunctionName[73],"WriteStencilPixel");
	Libstrcat(Wazp3D.FunctionName[74],"WriteStencilSpan");
	Libstrcat(Wazp3D.FunctionName[75],"DrawTriangleV");
	Libstrcat(Wazp3D.FunctionName[76],"DrawTriFanV");
	Libstrcat(Wazp3D.FunctionName[77],"DrawTriStripV");
	Libstrcat(Wazp3D.FunctionName[78],"GetScreenmodeList");
	Libstrcat(Wazp3D.FunctionName[79],"FreeScreenmodeList");
	Libstrcat(Wazp3D.FunctionName[80],"BestModeID");
	Libstrcat(Wazp3D.FunctionName[81],"BestModeIDTags");
	Libstrcat(Wazp3D.FunctionName[82],"VertexPointer");
	Libstrcat(Wazp3D.FunctionName[83],"TexCoordPointer");
	Libstrcat(Wazp3D.FunctionName[84],"ColorPointer");
	Libstrcat(Wazp3D.FunctionName[85],"BindTexture");
	Libstrcat(Wazp3D.FunctionName[86],"DrawArray");
	Libstrcat(Wazp3D.FunctionName[87],"DrawElements"),
	Libstrcat(Wazp3D.FunctionName[88],"SetFrontFace");

	Libstrcpy(Wazp3D.HardwareLie.name,"HardwareDriver Lie");
	Libstrcpy(Wazp3D.OnlyRGB24.name,"Only Fast RGB24");
	Libstrcpy(Wazp3D.UsePolyHack.name,   "Use	Poly Hack");
	Libstrcpy(Wazp3D.UseColorHack.name,  "Use BGcolor Hack");
	Libstrcpy(Wazp3D.UseCullingHack.name,"Use Culling Hack");
	Libstrcpy(Wazp3D.UseFog.name,"Use Fog");
	Libstrcpy(Wazp3D.UseClearDrawRegion.name,"Use ClearDrawRegion");
	Libstrcpy(Wazp3D.UseMinUpdate.name,"Use Min. Update");

	Libstrcpy(Wazp3D.UseAnyTexFmt.name,"Use any texFmt");
	Libstrcpy(Wazp3D.HackRGBA1.name,"Tex as RGBA1 Hack");
	Libstrcpy(Wazp3D.HackRGBA2.name,"Tex as RGBA2 Hack");
	Libstrcpy(Wazp3D.UseAntiScreen.name,"AntiAlias Screen");
	Libstrcpy(Wazp3D.UseClipper.name,"Use Clipper");
	Libstrcpy(Wazp3D.UsePerspective.name,"Use Persp.Correct");

	Libstrcpy(Wazp3D.DebugWazp3D.name,">>> Enable Debugger >>>");
	Libstrcpy(Wazp3D.DebugFunction.name,"Debug Function");
	Libstrcpy(Wazp3D.StepFunction.name, "Step  Function");
	Libstrcpy(Wazp3D.DebugCalls.name,"Debug Calls");
	Libstrcpy(Wazp3D.DebugVar.name,"Debug Var name ");
	Libstrcpy(Wazp3D.DebugVal.name,"Debug Var value");
	Libstrcpy(Wazp3D.DebugDoc.name,"Debug Var doc  ");
	Libstrcpy(Wazp3D.DebugState.name,"Debug State");
	Libstrcpy(Wazp3D.DebugDriver.name,"Debug Driver");
	Libstrcpy(Wazp3D.DebugContext.name,"Debug Context");
	Libstrcpy(Wazp3D.DebugTexture.name,"Debug Texture");
	Libstrcpy(Wazp3D.DebugPoint.name,"Debug Point");
	Libstrcpy(Wazp3D.DebugError.name,"Debug Error");
	Libstrcpy(Wazp3D.DebugWC.name,"Debug WC");
	Libstrcpy(Wazp3D.DebugWT.name,"Debug WT");
	Libstrcpy(Wazp3D.DebugSOFT3D.name,"Debug SOFT3D");
	Libstrcpy(Wazp3D.StepSOFT3D.name, "Step  SOFT3D");
	Libstrcpy(Wazp3D.DebugSC.name,"Debug SC");
	Libstrcpy(Wazp3D.DebugST.name,"Debug ST");
	Libstrcpy(Wazp3D.DebugClipper.name,"Debug Clipper");
	Libstrcpy(Wazp3D.StepCopyImage.name,"Step CopyImage");
	Libstrcpy(Wazp3D.DebugMemList.name,"Debug MemList");
	Libstrcpy(Wazp3D.DebugMemUsage.name,"Debug MemUsage");

	Wazp3D.HardwareLie.ON=TRUE;		/* pretend to be a perfect hardware driver */
	Wazp3D.OnlyRGB24.ON=FALSE;		/* only use fast RGB screens */
	Wazp3D.UseAnyTexFmt.ON=TRUE;
	Wazp3D.UsePolyHack.ON=TRUE;		/* convert trifan & tristrip to quad */
	Wazp3D.UseColorHack.ON=TRUE;		/* get background color from bitmap */
	Wazp3D.UseClearDrawRegion.ON=TRUE;
	Wazp3D.UseClipper.ON=TRUE;
	Wazp3D.UseMinUpdate.ON=TRUE;
	Wazp3D.UseCullingHack.ON=FALSE;
	Wazp3D.HackRGBA1.ON=FALSE;
	Wazp3D.HackRGBA2.ON=FALSE;
	Wazp3D.UseFog.ON=FALSE;
	Wazp3D.UseAntiScreen.ON=FALSE;
	Wazp3D.UsePerspective.ON=FALSE;

	Wazp3D.DebugFunction.ON=FALSE;		/* print Debug texts */
	Wazp3D.DebugVal.ON=FALSE;
	Wazp3D.DebugPoint.ON=FALSE;
	Wazp3D.DebugCalls.ON=FALSE;
	Wazp3D.DebugDriver.ON=FALSE;
	Wazp3D.DebugContext.ON=FALSE;
	Wazp3D.DebugTexture.ON=FALSE;
	Wazp3D.DebugError.ON=FALSE;
	Wazp3D.DebugWC.ON=FALSE;
	Wazp3D.DebugWT.ON=FALSE;
	Wazp3D.DebugSOFT3D.ON=FALSE;
	Wazp3D.DebugSC.ON=FALSE;
	Wazp3D.DebugST.ON=FALSE;
	Wazp3D.DebugClipper.ON=FALSE;
	Wazp3D.StepCopyImage.ON=FALSE;
	Wazp3D.DebugMemList.ON=FALSE;
	Wazp3D.DebugMemUsage.ON=FALSE;
	Wazp3D.DebugDoc.ON=FALSE;
	Wazp3D.DebugVar.ON=FALSE;
	Wazp3D.DebugState.ON=FALSE;

	Wazp3D.StepFunction.ON=FALSE;		/* Step at functions call */
	Wazp3D.StepSOFT3D.ON=FALSE;
	LibDebug=FALSE;

	Wazp3D.smode=(W3D_ScreenMode *)&Wazp3D.smodelist;
	Wazp3D.drivertype=W3D_DRIVER_CPU;
	Wazp3D.DriverList[0]=&Wazp3D.driver;
	Wazp3D.DriverList[1]=NULL;

	Libstrcpy(Wazp3D.DriverName,"Wazp3D soft renderer - Alain Thellier - Paris France 2007 - Beta 33");
	Wazp3D.driver.name	=Wazp3D.DriverName;
	Wazp3D.driver.ChipID	=W3D_CHIP_UNKNOWN;
	Wazp3D.driver.formats	=TRUECOLORFORMATS;
	Wazp3D.driver.swdriver	=W3D_TRUE;

	if(Wazp3D.OnlyRGB24.ON)
		Wazp3D.driver.formats=W3D_FMT_R8G8B8;

	if(Wazp3D.HardwareLie.ON)
		{
		Wazp3D.drivertype=W3D_DRIVER_3DHW;
		Wazp3D.driver.swdriver=W3D_FALSE;
		}
}
/*==================================================================================*/
void WAZP3D_Close()
{
WORD n;

	REM(WAZP3D_Close)
	NLOOP(88)
		if(Wazp3D.DebugCalls.ON)
			{
			if (n==40)	LibAlert("More calls...");
			Libprintf("[%ld] %ld\tcalls to %s \n",1+n,Wazp3D.FunctionCalls[1+n],Wazp3D.FunctionName[1+n]);
			}
	if(Wazp3D.DebugCalls.ON)
		LibAlert("All calls showed");
}
/*==================================================================================*/
ULONG BytesPerPix(ULONG format)
{
ULONG bpp;

	if(format==W3D_CHUNKY)	 bpp= 8/8;
	if(format==W3D_A8)	 bpp= 8/8;
	if(format==W3D_L8)	 bpp= 8/8;
	if(format==W3D_I8)	 bpp= 8/8;
	if(format==W3D_A4R4G4B4) bpp=16/8;
	if(format==W3D_A1R5G5B5) bpp=16/8;
	if(format==W3D_R5G6B5)	 bpp=16/8;
	if(format==W3D_L8A8)	 bpp=16/8;
	if(format==W3D_R8G8B8)	 bpp=24/8;
	if(format==W3D_A8R8G8B8) bpp=32/8;
	if(format==W3D_R8G8B8A8) bpp=32/8;
	return(bpp);
}
/*==================================================================================*/
void ConvertBitmap(ULONG format,UBYTE *pt1,UBYTE *pt2,UWORD high,UWORD large,ULONG offset1,ULONG offset2,UBYTE *palette)
{
UBYTE *color8=pt1;
UWORD *color16=(UWORD *)pt1;
UBYTE *RGBA=pt2;
UWORD a,r,g,b,x,y;

SFUNCTION(ConvertBitmap)
VAR(high)
VAR(large)
VAR(format)
VAR(pt1)
VAR(pt2)
VAR(offset1)
VAR(offset2)
VAR(palette)

if(format==W3D_CHUNKY)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=palette[*color8*4+1];
		RGBA[1]=palette[*color8*4+2];
		RGBA[2]=palette[*color8*4+3];
		RGBA[3]=palette[*color8*4+4];
		RGBA+=4;
		color8++;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_A1R5G5B5)
	YLOOP(high)
	{
		XLOOP(large)
		{
		a=(*color16 >> 15) << (8-1);
		r=(*color16 >> 10) << (8-5);
		g=(*color16 >>  5) << (8-5);
		b=(*color16 >>  0) << (8-5);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA[3]=a;
		RGBA+=4;
		color16++;
		}
	color16+=offset1;
	RGBA   +=offset2;
	}

if(format==W3D_R5G6B5)
	YLOOP(high)
	{
		XLOOP(large)
		{
		r=(*color16 >> 11) << (8-5);
		g=(*color16 >>  5) << (8-6);
		b=(*color16 >>  0) << (8-5);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA+=3;
		color16++;
		}
	color16+=offset1;
	RGBA   +=offset2;
	}

if(format==W3D_A4R4G4B4)
	YLOOP(high)
	{
		XLOOP(large)
		{
		a=(*color16 >> 12) << (8-4);
		r=(*color16 >>  8) << (8-4);
		g=(*color16 >>  4) << (8-4);
		b=(*color16 >>  0) << (8-4);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA[3]=a;
		RGBA+=4;
		color16++;
		}
	color16+=offset1;
	RGBA   +=offset2;
	}

if(format==W3D_R8G8B8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=color8[0];
		RGBA[1]=color8[1];
		RGBA[2]=color8[2];
		RGBA+=3;
		color8+=3;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_A8R8G8B8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=color8[1];
		RGBA[1]=color8[2];
		RGBA[2]=color8[3];
		RGBA[3]=color8[0];
		RGBA+=4;
		color8+=4;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_R8G8B8A8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=color8[0];
		RGBA[1]=color8[1];
		RGBA[2]=color8[2];
		RGBA[3]=color8[3];
		RGBA+=4;
		color8+=4;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_A8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=RGBA[1]=RGBA[2]=0;
		RGBA[3]=color8[0];
		RGBA+=4;
		color8+=1;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_L8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=RGBA[1]=RGBA[2]=color8[0];
		RGBA+=3;
		color8+=1;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_L8A8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=RGBA[1]=RGBA[2]=color8[0];
		RGBA[3]=color8[1];
		RGBA+=4;
		color8+=2;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}

if(format==W3D_I8)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=RGBA[1]=RGBA[2]=RGBA[3]=color8[0];
		RGBA+=4;
		color8+=1;
		}
	color8+=offset1;
	RGBA  +=offset2;
	}



}
/*==================================================================================*/
ULONG ConvertFullTexture(W3D_Texture *texture)
{
UWORD high,large,bpp2;
ULONG format=texture->texfmtsrc;

SFUNCTION(ConvertFullTexture)

	bpp2=32/8;
	if(format==W3D_R8G8B8)	bpp2=24/8;
	if(format==W3D_R5G6B5)	bpp2=24/8;
	if(format==W3D_L8)	bpp2=24/8;



	high =texture->texheight;
	large=texture->texwidth;
VAR(bpp2)
VAR(high)
VAR(large)
	texture->texdata=MYmalloc(high*large*bpp2,"texdata converted");
	if(texture->texdata==NULL)
		return(0);

	ConvertBitmap(format,texture->texsource,texture->texdata,high,large,0,0,(UBYTE *)texture->palette);

	return(bpp2*8);
}
/*=================================================================*/
#define CUT(A,B,z,x,lim) P->z= A->z + (lim - A->x) * (B->z - A->z) / (B->x - A->x);

#define NewPointClipX(A,B,lim) {CUT(A,B,y,x,lim) CUT(A,B,z,x,lim) CUT(A,B,u,x,lim) CUT(A,B,v,x,lim) P->x=lim;COPYRGBA(P->RGBA,A->RGBA);P++;NewPnb++;}
#define NewPointClipY(A,B,lim) {CUT(A,B,x,y,lim) CUT(A,B,z,y,lim) CUT(A,B,u,y,lim) CUT(A,B,v,y,lim) P->y=lim;COPYRGBA(P->RGBA,A->RGBA);P++;NewPnb++;}
#define NewPointClipZ(A,B,lim) {CUT(A,B,x,z,lim) CUT(A,B,y,z,lim) CUT(A,B,u,z,lim) CUT(A,B,v,z,lim) P->z=lim;COPYRGBA(P->RGBA,A->RGBA);P++;NewPnb++;}
#define CopyPoint(A)  {Libmemcpy(P,A,sizeof(struct point3D)); P++; NewPnb++; }
#define SwapBuffers useT1=!useT1; if(useT1) {P1=&(T1[0]); P2=&(T1[1]);P=&(T2[0]);} else	{P1=&(T2[0]); P2=&(T2[1]);P=&(T1[0]);}
/*=================================================================*/
void ClipPoly(struct SOFT3D_context *SC)
{
struct point3D *P;
struct point3D *P1;
struct point3D *P2;
struct point3D *T1=(struct point3D *)&SC->T1;
struct point3D *T2=(struct point3D *)&SC->T2;
BOOL useT1=TRUE;
UBYTE  IsInside[100];
UBYTE InsidePnb;
LONG	Pnb=SC->PolyPnb;
LONG n,NewPnb;
BOOL FaceClipped;

if(Wazp3D.DebugClipper.ON)
{
REM(===================================================)
	VARF(SC->ClipMin.x)
	VARF(SC->ClipMax.x)
	VARF(SC->ClipMin.y)
	VARF(SC->ClipMax.y)
	VARF(SC->ClipMin.z)
	VARF(SC->ClipMax.z)
}
	if(Pnb>100) return;
	FaceClipped=FALSE;
	NewPnb=0;

	useT1=TRUE;
	P1=&(T1[0]);P2=&(T1[1]);P=&(T2[0]);
	Libmemcpy(P1,SC->PolyP,Pnb*sizeof(struct point3D));

	/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMax x)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(P1[n].x <= SC->ClipMax.x)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMax.x) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipX(P1,P2,SC->ClipMax.x)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipX(P2,P1,SC->ClipMax.x); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMin x)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(SC->ClipMin.x <= P1[n].x)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMin.x) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipX(P2,P1,SC->ClipMin.x)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipX(P1,P2,SC->ClipMin.x); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMax y)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(P1[n].y <= SC->ClipMax.y)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMax.y) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipY(P1,P2,SC->ClipMax.y)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipY(P2,P1,SC->ClipMax.y); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMin y)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(SC->ClipMin.y <= P1[n].y)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMin.y) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipY(P2,P1,SC->ClipMin.y)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipY(P1,P2,SC->ClipMin.y); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMax z)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(P1[n].z <= SC->ClipMax.z)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMax.z) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipZ(P1,P2,SC->ClipMax.z)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipZ(P2,P1,SC->ClipMax.z); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;if(Wazp3D.DebugClipper.ON) REM(-------)if(Wazp3D.DebugClipper.ON) REM(ClipTest SC->ClipMin z)
NLOOP(Pnb)
{
if(Wazp3D.DebugClipper.ON) PrintP(&(P1[n]));
IsInside[n]=FALSE;
if(SC->ClipMin.z <= P1[n].z)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}
if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Fully ouside SC->ClipMin.z) goto HideFace;}

if(InsidePnb!=Pnb)
{
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(P1[Pnb]),P1,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
	{
	if(IsInside[n+1]==TRUE)
		CopyPoint(P2)
	else
		NewPointClipZ(P2,P1,SC->ClipMin.z)
	}
else
	{
	if(IsInside[n+1]==TRUE)
		{NewPointClipZ(P1,P2,SC->ClipMin.z); CopyPoint(P2)}
	}
P1++;	P2++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
if(Wazp3D.DebugClipper.ON) VAR(NewPnb)
if(Wazp3D.DebugClipper.ON) VAR(Pnb)
if(Wazp3D.DebugClipper.ON) VAR(FaceClipped)
	if(FaceClipped==FALSE)	{if(Wazp3D.DebugClipper.ON) if(Wazp3D.DebugClipper.ON) REM(Face Not Clipped) return;}

	SC->PolyPnb=Pnb;

	if(useT1) P1=T1; else P1=T2;
	Libmemcpy(SC->PolyP,P1,SC->PolyPnb*sizeof(struct point3D));
if(Wazp3D.DebugClipper.ON) REM(===================================================)
	return;
HideFace:
	SC->PolyPnb=0;
	return;
}
/*=================================================================*/
void ReduceBitmap(UBYTE *pt,UBYTE *pt2,WORD large,WORD high, WORD bits,WORD ratio)
{
UBYTE *RGB2;
UBYTE *RGB1;
LONG P,L;		  /* bytesperpixel, bytesperline */
LONG x,y,m,n;
LONG r,g,b,a;
LONG ratio2;
LONG offset;

if(Wazp3D.DebugSOFT3D.ON) Libprintf("ReduceBitmap/%ld %ldX%ld %ldbits >>from %ld to %ld\n",ratio,large,high,bits,pt,pt2);
	if(pt ==NULL) return;
	if(pt2==NULL) return;
	L=large * bits /8;
	P=bits /8;
	large=large/ratio;
	high =high /ratio;
	ratio2=ratio*ratio;

	RGB1=pt;
	RGB2=pt2;
REM(Reducing ...)
	YLOOP(high)
	{
	XLOOP(large)
		{
		r=g=b=a=0;
		MLOOP(ratio)
		NLOOP(ratio)
			{
			offset=L*m+P*n+L*ratio*y+P*ratio*x;
			RGB1=&(pt[offset]);
			r=r+(LONG)RGB1[0];
			g=g+(LONG)RGB1[1];
			b=b+(LONG)RGB1[2];
			if(bits==32)
				a=a+(LONG)RGB1[3];
			}
		r=r/ratio2;
		g=g/ratio2;
		b=b/ratio2;
		a=a/ratio2;
		RGB2[0]=r;
		RGB2[1]=g;
		RGB2[2]=b;
		if(bits==32)
			RGB2[3]=a;
		RGB2=&(RGB2[P]);
		}
	}
REM(ReduceBitmap OK)
}
/*==================================================================================*/
void CreateMipmaps(struct SOFT3D_texture *ST)
{
UBYTE *pt;
UWORD large,high,level,reduction;
ULONG size;

	level=0;
	pt=ST->pt;
	large=ST->large;
	high=ST->high;
	size=ST->large * ST->high * ST->bits / 8;
	reduction=2;
next_mipmap:
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("MipMap %ldX%ld = %ld (%ld)\n",large,high,size,pt);
	pt=&(pt[size]);
	ReduceBitmap(ST->pt,pt,ST->large,ST->high,ST->bits,reduction);
	level++;large=large/2;high=high/2;size=size/4;reduction=reduction*2;
	if (high>0) goto next_mipmap;
}
/*==================================================================================*/
struct VertexFFF{
	float x,y,z;
};
/*==================================================================================*/
struct VertexFFD{
	float x,y;
	double z;
};
/*==================================================================================*/
struct VertexDDD{
	double x,y,z;
};
/*==========================================================================*/
void GetVertex(struct WAZP3D_context *WC,W3D_Vertex *V)
{
struct point3D *P;

SFUNCTION(GetVertex)
	P=&(WC->P[WC->Pnb]);
	WC->Pnb++;
	P->x=V->x;
	P->y=V->y;
	P->z=(float)V->z;
	P->u=V->u/WC->uresize;
	P->v=V->v/WC->vresize;
	P->RGBA[0]=V->color.r * 255.0;
	P->RGBA[1]=V->color.g * 255.0;
	P->RGBA[2]=V->color.b * 255.0;
	P->RGBA[3]=V->color.a * 255.0;
	PrintP(P);
}
/*==========================================================================*/
void GetPoint(struct WAZP3D_context *WC,ULONG i)
{
struct point3D *P;
UBYTE *pt;
float *rgba;
UBYTE TmpRGBA[4];
UBYTE *RGBA=(UBYTE *)TmpRGBA;
float *u;
float *v;
struct VertexFFF *fff;
struct VertexFFD *ffd;
struct VertexDDD *ddd;

	P=&(WC->P[WC->Pnb]);
	P->x=P->y=P->z=P->u=P->v=0.0;
	COPYRGBA(P->RGBA,WC->CurrentRGBA);
	WC->Pnb++;

/* recover XYZ values */
	if(WC->V!=NULL)
	{
	pt=&(WC->V[i*WC->Vstride]);
	if(WC->Vformat==W3D_VERTEX_F_F_F)
	{
	fff=(struct VertexFFF *)pt;
	P->x=fff->x;
	P->y=fff->y;
	P->z=fff->z;
	}
	if(WC->Vformat==W3D_VERTEX_F_F_D)
	{
	ffd=(struct VertexFFD *)pt;
	P->x=ffd->x;
	P->y=ffd->y;
	P->z=(float)ffd->z;
	}
	if(WC->Vformat==W3D_VERTEX_D_D_D)
	{
	ddd=(struct VertexDDD *)pt;
	P->x=(float)ddd->x;
	P->y=(float)ddd->y;
	P->z=(float)ddd->z;
	}
	}


/* recover UV values */
	if(WC->V!=NULL)
	{
	pt=&(WC->UV[i*WC->UVstride]);
	u=(float *)pt;
	v=(float *)&pt[WC->UVoff_v];

	if(WC->UVformat==W3D_TEXCOORD_NORMALIZED)
	{
	REM(W3D_TEXCOORD_NORMALIZED)
	P->u=u[0];
	P->v=v[0];
	}
	else
	{
	REM(W3D_TEXCOORD_NOT_NORMALIZED)
	P->u=u[0]/WC->uresize;
	P->v=v[0]/WC->vresize;
	}
	}

/* recover RGBA values */
	if(WC->RGBA!=NULL)
	{
	pt=&(WC->RGBA[i*WC->RGBAstride]);
	if(WC->RGBAformat==W3D_COLOR_FLOAT)
	{
	rgba=(float *)pt;
	RGBA[0]=rgba[0]*255.0;RGBA[1]=rgba[1]*255.0;RGBA[2]=rgba[2]*255.0;RGBA[3]=rgba[3]*255.0;
	}
	if(WC->RGBAformat==W3D_COLOR_UBYTE)
	{
	RGBA=pt;
	}
	if(WC->RGBAorder==W3D_CMODE_RGB)	P->RGBA[0]=RGBA[0]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[2]; P->RGBA[3]=255;
	if(WC->RGBAorder==W3D_CMODE_BGR)	P->RGBA[0]=RGBA[2]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[0]; P->RGBA[3]=255;
	if(WC->RGBAorder==W3D_CMODE_RGBA)	P->RGBA[0]=RGBA[0]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[2]; P->RGBA[3]=RGBA[3];
	if(WC->RGBAorder==W3D_CMODE_ARGB)	P->RGBA[0]=RGBA[1]; P->RGBA[1]=RGBA[2]; P->RGBA[2]=RGBA[3]; P->RGBA[3]=RGBA[0];
	if(WC->RGBAorder==W3D_CMODE_BGRA)	P->RGBA[0]=RGBA[2]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[0]; P->RGBA[3]=RGBA[3];
	}

	PrintP(P);
}
/*==========================================================================*/
void DrawTriLines(W3D_Context *context,WORD x1,WORD y1,WORD x2,WORD y2,WORD x3,WORD y3)
{
struct WAZP3D_context *WC=context->driver;
WORD y;
#define LINE(x1,y1,x2,y2)  {Move(&WC->rastport,(WORD)x1,(WORD)y1+y); Draw(&WC->rastport,(WORD)x2,(WORD)y2+y );}

	if(context==NULL) return;
	y=(WORD)context->yoffset;
	REMP ("DrawTriLines %ld %ld , %ld %ld , %ld %ld \n",x1,y1,x2,y2,x3,y3);
	 LINE(x1,y1,x2,y2);
	 LINE(x2,y2,x3,y3);
	 LINE(x3,y3,x1,y1);
}
/*==========================================================================*/
void AntiAliasImage2X2(struct SOFT3D_context *SC)
{
register UBYTE *RGBA0=(UBYTE *)SC->Image32;
register UBYTE *RGBA1;
UBYTE TmpDiv7[7*256];
register UBYTE *div7=(UBYTE *)TmpDiv7;
register UWORD r,g,b,x,y;
UWORD large=SC->large;
UWORD high =SC->high;

	XLOOP(7*256)
		div7[x]=x/7;

	RGBA1=RGBA0+large*4;
	large=large-1;
	high =high -1;

	YLOOP(high)
	{
	XLOOP(large)
	{
	r=RGBA0[0]; r=r+r; r=r+r;
	g=RGBA0[1]; g=g+g; g=g+g;
	b=RGBA0[2]; b=b+b; b=b+b;
	r=r+RGBA0[0+4]+RGBA1[0]+RGBA1[0+4];RGBA0[0]=div7[r];
	g=g+RGBA0[1+4]+RGBA1[1]+RGBA1[1+4];RGBA0[1]=div7[g];
	b=b+RGBA0[2+4]+RGBA1[2]+RGBA1[2+4];RGBA0[2]=div7[b];
	RGBA0+=4;
	RGBA1+=4;
	}
	RGBA0+=4;
	RGBA1+=4;
	}

}
/*==========================================================================*/
void AntiAliasImage3X3(struct SOFT3D_context *SC)
{
register UWORD r,g,b,x,y;
register UBYTE *RGBA0=(UBYTE *)SC->Image32;
register UBYTE *RGBA1;
register UBYTE *RGBA2;
register UBYTE *line0;
register UBYTE *line1;
UWORD large=SC->large;
UWORD high =SC->high;
UBYTE *temp;

	line0=(UBYTE *) &SC->lines[4*1024*0];
	line1=(UBYTE *) &SC->lines[4*1024*1];
	RGBA1=RGBA0+large*4;
	RGBA2=RGBA1+large*4;
	large=large-2;
	high =high -2;

	XLOOP(large)
	{
	line0[0+4]=RGBA0[0+4];
	line0[1+4]=RGBA0[1+4];
	line0[2+4]=RGBA0[2+4];
	line0+=4;
	RGBA0+=4;
	}
	line1=(UBYTE *)&SC->lines[4*1024*1];
	RGBA0=(UBYTE *) SC->Image32;

	YLOOP(high)
	{
	line0=(UBYTE *) &SC->lines[4*1024*0];
	line1=(UBYTE *) &SC->lines[4*1024*1];
	if(y&1)
		SWAP(line0,line1)
		XLOOP(large)
		{
		r=RGBA1[0+4]; r=r+r; r=r+r; r=r+r;
		g=RGBA1[1+4]; g=g+g; g=g+g; g=g+g;
		b=RGBA1[2+4]; b=b+b; b=b+b; b=b+b;
		r=r+RGBA0[0+0]+RGBA0[0+4]+RGBA0[0+8]+RGBA1[0+0]+RGBA1[0+8]+RGBA2[0+0]+RGBA2[0+4]+RGBA2[0+8];
		g=g+RGBA0[1+0]+RGBA0[1+4]+RGBA0[1+8]+RGBA1[1+0]+RGBA1[1+8]+RGBA2[1+0]+RGBA2[1+4]+RGBA2[1+8];
		b=b+RGBA0[2+0]+RGBA0[2+4]+RGBA0[2+8]+RGBA1[2+0]+RGBA1[2+8]+RGBA2[2+0]+RGBA2[2+4]+RGBA2[2+8];

		line1[0+4]=r>>4;
		line1[1+4]=g>>4;
		line1[2+4]=b>>4;

		RGBA0[0+4]=line0[0+4];
		RGBA0[1+4]=line0[1+4];
		RGBA0[2+4]=line0[2+4];

		line0+=4;
		line1+=4;
		RGBA0+=4;
		RGBA1+=4;
		RGBA2+=4;
		}

	RGBA0+=4*2;
	RGBA1+=4*2;
	RGBA2+=4*2;
	}
}
/*==========================================================================*/
void CopyImageToDrawRegion(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct SOFT3D_context *SC=WC->SC;
UBYTE RGBA[4];

REM(CopyImageToDrawRegion-------------------------)
	if(WC->Image8==NULL) return;
	if(SC->Pxmax==0) return;
	if(SC->Pymax==0) return;

	if(Wazp3D.StepCopyImage.ON) LibAlert("CopyImageToDrawRegion() occurs now !!");

	W3D_Flush(context);

	if(Wazp3D.UseMinUpdate.ON)
	{
	SC->xUpdate    =SC->Pxmin;	/* should also be used to clear the previous drawing */
	SC->yUpdate    =SC->Pymin;
	SC->largeUpdate=SC->Pxmax-SC->Pxmin+1;
	SC->highUpdate =SC->Pymax-SC->Pymin+1;
	 }
	else
	{
	SC->xUpdate    =0;
	SC->yUpdate    =0;
	SC->largeUpdate=WC->large;
	SC->highUpdate =WC->high;
	}

	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("Updated Region from %ld %ld (%ld X %ld pixels) \n",SC->xUpdate,SC->yUpdate,SC->largeUpdate,SC->highUpdate);

	if(Wazp3D.UseColorHack.ON)
		{
		STACKReadPixelArray(RGBA,0,0,1*(32/8),&WC->rastport,0,0,1,1,RECTFMT_RGBA);
		SOFT3D_SetBackground(WC->SC,RGBA);
		PrintRGBA((UBYTE *)RGBA);
		}

	if(Wazp3D.UseAntiScreen.ON)
		AntiAliasImage3X3(WC->SC);

	if(0<SC->largeUpdate)
	if(SC->largeUpdate<=WC->large)
	if(0< SC->highUpdate)
	if(SC->highUpdate <=WC->high)
	{
	VAR(context->yoffset)
	VAR(SC->xUpdate)
	VAR(SC->yUpdate)
	VAR(SC->largeUpdate)
	VAR(SC->highUpdate)
	STACKWritePixelArray(WC->Image8,SC->xUpdate,SC->yUpdate,WC->large*(32/8),&WC->rastport,SC->xUpdate,SC->yUpdate+context->yoffset,SC->largeUpdate,SC->highUpdate,RECTFMT_RGBA);
	SOFT3D_ClearImage(WC->SC,SC->xUpdate,SC->yUpdate,SC->largeUpdate,SC->highUpdate);
	if(!WC->CallClearZBuffer)
		SOFT3D_ClearZBuffer(WC->SC,ZMAX);		/*if 3D prog dont manage a zbuffer , clear it ourself */
	}

	SC->Pxmin=WC->large-1;
	SC->Pymin=WC->high -1;
	SC->Pxmax=0;
	SC->Pymax=0;

REM(CopyImageToDrawRegionOK)
	if(Wazp3D.DebugContext.ON) LibAlert("OK");
}
/*==========================================================================*/
/* simpler W3D_GetState used internally */
#define StateON(s) ((context->state & s)!=0)
/*==========================================================================*/
W3D_Context	*W3D_CreateContext(ULONG *error, struct TagItem *taglist)
{
ULONG tag,data;
UWORD n;
W3D_Context	*context;
W3D_Bitmap  *w3dbm;
struct WAZP3D_context *WC;
ULONG action[2];
ULONG AllStates=~0;
ULONG UnsupportedStates= W3D_PERSPECTIVE & W3D_ANTI_POINT & W3D_ANTI_LINE & W3D_ANTI_POLYGON & W3D_ANTI_FULLSCREEN & W3D_DITHERING & W3D_LOGICOP & W3D_STENCILBUFFER & W3D_ALPHATEST & W3D_SPECULAR & W3D_TEXMAPPING3D & W3D_CHROMATEST;
ULONG EnableMask,DisableMask,supportedfmt,envsupmask;


	WAZP3D_Function(1);
	VAR(error)
	VAR(taglist)

	*error=W3D_SUCCESS;

	WC=MYmalloc(sizeof(struct WAZP3D_context),"WAZP3D_context");
	if(WC==NULL)
		{ *error=W3D_NOMEMORY;PrintError(*error);return(NULL);}
	context=&WC->context;

	DisableMask=AllStates;
	if(Wazp3D.HardwareLie.ON)
		EnableMask=AllStates;
	else
		EnableMask=AllStates & (~UnsupportedStates) ;

	supportedfmt=Wazp3D.driver.formats;
	envsupmask=W3D_REPLACE & W3D_DECAL;
	if(Wazp3D.HardwareLie.ON)
		envsupmask=W3D_REPLACE & W3D_DECAL & W3D_MODULATE & W3D_BLEND;

	WC->ModeID=INVALID_ID;
	WC->MaxPnb=MAXPNB;

REM( context values )
	context->driver=WC;			 /* insert driver specific data here */
	context->gfxdriver=NULL;		/* usable by the GFXdriver */
	context->drivertype=Wazp3D.drivertype;	/* driver type (3DHW / CPU) */
	context->regbase=NULL;			 /* register base */
	context->vmembase=NULL;			 /* video memory base */
	context->zbuffer=NULL;			 /* Pointer to the Z buffer */
	context->stencilbuffer=NULL;		/* Pointer to the stencil buffer */
	context->state=0;				 /* hardware state (see below) */
	context->drawregion=NULL;		/* destination bitmap */
	context->supportedfmt=supportedfmt;	/* bitmask with all supported dest fmt */
	context->format=0;			 /* bitmap format (see below) */
	context->yoffset=0;			 /* Y-Offset (for ScrollVPort-Multibuf.) */
	context->bprow=0;				 /* bytes per row */
	context->width=0;				 /* bitmap width */
	context->height=0;			 /* bitmap height */
	context->depth=0;				 /* bitmap depth */
	context->chunky=FALSE;			 /* TRUE, if palettized screen mode */
	context->destalpha=FALSE;		 /* TRUE, if dest alpha channel available */
	context->zbufferalloc=FALSE;		/* TRUE, is Z buffer is allocated */
	context->stbufferalloc=FALSE;		/* TRUE, is stencil buffer is allocated */
	context->HWlocked=FALSE;		 /* TRUE, if 3D HW was locked */
	context->w3dbitmap=FALSE;		/* TRUE, if drawregion points to a W3D_Bitmap */
	context->zbufferlost=FALSE;		 /* TRUE, if zbuffer not reallocatable */
	context->reserved3=0;

/*	context->restex=NULL;	*/		/* A list of all resident textures */
/*	context->tex=NULL;	*/		/* A list of all textures which are not on card */

	context->maxtexwidth  =1024;		/* -HJF- replaced these for possible */
	context->maxtexheight =1024;		/* support of non-square textures */
	context->maxtexwidthp =1024;		/* -HJF- For hardware that has different */
	context->maxtexheightp=1024;		/* constaints in perspective mode */
							 /* scissor region */
	context->scissor.left	=0;
	context->scissor.top	=0;
	context->scissor.width	=0;
	context->scissor.height	=0;
							 /* fogging parameters */
	context->fog.fog_color.r	=1.0;
	context->fog.fog_color.g	=1.0;
	context->fog.fog_color.b	=1.0;
	context->fog.fog_start	 	=0.999;
	context->fog.fog_end		=0.000;
	context->fog.fog_density	=0.1;

	context->envsupmask=envsupmask;	 /* Mask of supported envmodes */
	context->queue=NULL;			 /* queue to buffer drawings */
	context->drawmem=NULL;			 /* base address for drawing operations */

	context->globaltexenvmode=0;		/* Global texture environment mode */
	context->globaltexenvcolor[0]=1.0;	/* global texture env color */
	context->globaltexenvcolor[1]=1.0;
	context->globaltexenvcolor[2]=1.0;
	context->globaltexenvcolor[3]=1.0;

	context->DriverBase=NULL;		/* Library base of the active driver */

	context->DisableMask=DisableMask;	/* Mask for disable-able states */
	context->EnableMask =EnableMask;	/* Mask for enable-able states */

	context->CurrentChip=W3D_CHIP_UNKNOWN;	/* Chip constant */
	context->DriverVersion=1;			/* Internal driver version */

	context->VertexPointer=NULL;		/* Pointer to the vertex buffer array */
	context->VPStride=0;			 /* Stride of vertex array */
	context->VPMode=0;			 /* Vertex buffer format */
	context->VPFlags=0;			 /* not yet used */

	NLOOP(W3D_MAX_TMU)
	{
	context->CurrentTex[n]=NULL;
	context->TexCoordPointer[n]=NULL;
	context->TPStride[n]=0;		 /* Stride of TexCoordPointers */
	context->TPVOffs[n]=0;			/* Offset to V coordinate */
	context->TPWOffs[n]=0;			/* Offset to W coordinate */
	context->TPFlags[n]=0;			/* Flags */
	}

	context->ColorPointer=NULL;		/* Pointer to the color array */
	context->CPStride=0;			 /* Color pointer stride */
	context->CPMode=0;			 /* Mode and color format */
	context->CPFlags=0;			 /* not yet used */

	context->FrontFaceOrder=W3D_CW;	/* Winding order of front facing triangles */
	context->specialbuffer=NULL;		/* Special buffer for chip-dependant use (like command */

REM( Warp3Ds default context state values )
	SetState(context,W3D_AUTOTEXMANAGEMENT,TRUE);
	SetState(context,W3D_SYNCHRON ,	FALSE);
	SetState(context,W3D_INDIRECT ,	FALSE);
	SetState(context,W3D_TEXMAPPING ,	TRUE);
	SetState(context,W3D_PERSPECTIVE ,	FALSE);
	SetState(context,W3D_FAST ,		FALSE);
	SetState(context,W3D_GOURAUD ,	TRUE);
	SetState(context,W3D_ZBUFFER ,	FALSE);
	SetState(context,W3D_ZBUFFERUPDATE ,FALSE);
	SetState(context,W3D_BLENDING ,	FALSE);
	SetState(context,W3D_FOGGING ,	FALSE);
	SetState(context,W3D_ANTI_POINT ,	FALSE);
	SetState(context,W3D_ANTI_LINE ,	FALSE);
	SetState(context,W3D_ANTI_POLYGON ,	FALSE);
	SetState(context,W3D_ANTI_FULLSCREEN,FALSE);
	SetState(context,W3D_DITHERING ,	FALSE);
	SetState(context,W3D_LOGICOP ,	FALSE);
	SetState(context,W3D_STENCILBUFFER ,FALSE);
	SetState(context,W3D_DOUBLEHEIGHT ,	FALSE);
	SetState(context,W3D_ALPHATEST ,	FALSE);
	SetState(context,W3D_SPECULAR ,	FALSE);
	SetState(context,W3D_TEXMAPPING3D ,	FALSE);
	SetState(context,W3D_CHROMATEST ,	FALSE);
	SetState(context,W3D_GLOBALTEXENV ,	FALSE);


	action[TRUE] =W3D_ENABLE;
	action[FALSE]=W3D_DISABLE;
	while (taglist->ti_Tag != TAG_DONE)
	{
	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;

	if(tag==W3D_CC_MODEID )		WC->ModeID			=data;
	if(tag==W3D_CC_BITMAP )		context->drawregion	=(struct BitMap *)data;
	if(tag==W3D_CC_YOFFSET)		context->yoffset		=data;
	if(tag==W3D_CC_DRIVERTYPE)	context->drivertype	=data;
	if(tag==W3D_CC_W3DBM)		context->w3dbitmap	=data;

	if(tag==W3D_CC_INDIRECT)		*error=W3D_SetState(context,W3D_INDIRECT		,action[data]);
	if(tag==W3D_CC_GLOBALTEXENV )		*error=W3D_SetState(context,W3D_GLOBALTEXENV	,action[data]);
	if(tag==W3D_CC_DOUBLEHEIGHT )		*error=W3D_SetState(context,W3D_DOUBLEHEIGHT	,action[data]);
	if(tag==W3D_CC_FAST )			*error=W3D_SetState(context,W3D_FAST		,action[data]);

	if(context->drivertype==W3D_DRIVER_3DHW)
	if(!Wazp3D.HardwareLie.ON)
		*error=W3D_ILLEGALINPUT;

	WINFO(tag,W3D_CC_TAGS,"Tags")
	WINFO(tag,W3D_CC_BITMAP,"destination bitmap")
	WINFO(tag,W3D_CC_YOFFSET,"y-Offset ")
	WINFO(tag,W3D_CC_DRIVERTYPE,"Driver type ")
	WINFO(tag,W3D_CC_W3DBM,"Use W3D_Bitmap")
	WINFO(tag,W3D_CC_INDIRECT,"Indirect drawing ")
	WINFO(tag,W3D_CC_GLOBALTEXENV,"SetTexEnv is global")
	WINFO(tag,W3D_CC_DOUBLEHEIGHT,"Drawing area has double height")
	WINFO(tag,W3D_CC_FAST,"Allow Warp3D to modify passed structures ")
	WINFO(tag,W3D_CC_MODEID,"Specify modeID to use")
	VAR(data)
	PrintError(*error);

	if(*error!=W3D_SUCCESS)
		{W3D_DestroyContext(context);context=NULL;return(context);}
	}
	WINFO(context->drivertype,W3D_DRIVER_UNAVAILABLE,"driver unavailable ")
	WINFO(context->drivertype,W3D_DRIVER_BEST,"use best mode ")
	WINFO(context->drivertype,W3D_DRIVER_3DHW,"use 3D-HW ")
	WINFO(context->drivertype,W3D_DRIVER_CPU,"use CPU ")

	context->scissor.width	=context->width;
	context->scissor.height	=context->height;

	if(context->w3dbitmap)
	{
	w3dbm=(W3D_Bitmap  *)context->drawregion;
	context->bprow  = w3dbm->bprow;	/* bytes per row */
	context->width  = w3dbm->width;	/* bitmap width */
	context->height = w3dbm->height;	/* bitmap height */
	context->depth  = GetBitMapAttr( context->drawregion, BMA_DEPTH  );	/* bitmap depth */
	context->format = w3dbm->format;	/* bitmap format (see below) */
	context->drawmem= w3dbm->dest;	/* base address for drawing operations */
	}
	else
	{
	context->width  = GetBitMapAttr( context->drawregion, BMA_WIDTH  );	/* bitmap width */
	context->height = GetBitMapAttr( context->drawregion, BMA_HEIGHT );	/* bitmap height */
	context->depth  = GetBitMapAttr( context->drawregion, BMA_DEPTH  );	/* bitmap depth */
	context->bprow  = context->width * context->depth / 8;
	context->format = 0;
	if(context->depth==24)	 context->format=W3D_FMT_R8G8B8;
	if(context->depth==32)	 context->format=W3D_FMT_A8R8G8B8;
	context->drawmem=NULL;
	}

	WC->Driver=&Wazp3D.driver;
	WC->large=context->width ;
	WC->high =context->height;
	if(StateON(W3D_DOUBLEHEIGHT)) WC->high=WC->high/2;

	WC->Image8=MYmalloc(WC->large*WC->high*32/8,"SOFT3D_ImageBuffer");

	InitRastPort(&WC->rastport);
	WC->rastport.BitMap=context->drawregion;
	WC->firstWT=NULL;
	WC->CallFlushFrame=WC->CallSetDrawRegion=WC->CallClearZBuffer=FALSE;

	WC->SC=SOFT3D_Start(WC->large,WC->high,(ULONG *)WC->Image8);
	Wazp3D.UseAntiScreen.ON=StateON(W3D_ANTI_FULLSCREEN);

	W3D_AllocZBuffer(context);		/* SOFT3D !NEED! a Zbuffer */
	CopyImageToDrawRegion(context);	/* clear Image buffer */

	return(context);
}
/*==========================================================================*/
W3D_Context *W3D_CreateContextTags(ULONG *error, Tag tag1, ...)
{
static ULONG tag[30];
va_list va;
WORD n=0;

	WAZP3D_Function(2);
	tag[n] = tag1;
	VAR(tag[n])
	va_start (va, tag1);
	do	 {
		n++;	tag[n]= va_arg(va, ULONG);	VAR(tag[n])
		if(n&2) if (tag[n] == TAG_DONE) break;
		}
	while (n<30);
	va_end(va);

	return (W3D_CreateContext(error,(struct TagItem *)tag) );
}
/*==========================================================================*/
void W3D_DestroyContext(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(3);
	W3D_FreeAllTexObj(context);
	SOFT3D_End(WC->SC);
	VAR(WC->CallFlushFrame)
	VAR(WC->CallSetDrawRegion)
	VAR(WC->CallClearZBuffer)

	MYfree(context->driver);		/* context->driver=WC that also include the W3D_Context */

	if (Wazp3D.DebugContext.ON)
		LibAlert("DestroyContextOK");
}
/*==========================================================================*/
ULONG W3D_GetState(W3D_Context *context, ULONG state)
{
ULONG action;

	WAZP3D_Function(4);
	if(context->state & state)
		action=W3D_ENABLED;
	else
		action=W3D_DISABLED;

	if(Wazp3D.DebugVal.ON)
	if(Wazp3D.DebugState.ON)
		if(action==W3D_DISABLED) {Libprintf("[ ] ");} else {Libprintf("[X] ");}

	WINFO(state,W3D_AUTOTEXMANAGEMENT,"automatic texture management ")
	WINFO(state,W3D_SYNCHRON,"wait,until HW is idle ")
	WINFO(state,W3D_INDIRECT,"drawing is queued until flushed ")
	WINFO(state,W3D_GLOBALTEXENV,"SetTexEnv is global ")
	WINFO(state,W3D_DOUBLEHEIGHT,"Drawing area is double height ")
	WINFO(state,W3D_FAST,"Allow Warp3D to modify passed structures")
	WINFO(state,W3D_AUTOCLIP,"clip to screen region ")
	WINFO(state,W3D_TEXMAPPING,"texmapping state ")
	WINFO(state,W3D_PERSPECTIVE,"perspective correction state ")
	WINFO(state,W3D_GOURAUD,"gouraud/flat shading ")
	WINFO(state,W3D_ZBUFFER,"Z-Buffer state ")
	WINFO(state,W3D_ZBUFFERUPDATE,"Z-Buffer update state ")
	WINFO(state,W3D_BLENDING,"Alpha blending state ")
	WINFO(state,W3D_FOGGING,"Fogging state ")
	WINFO(state,W3D_ANTI_POINT,"Point antialiasing state ")
	WINFO(state,W3D_ANTI_LINE,"Line antialiasing state ")
	WINFO(state,W3D_ANTI_POLYGON,"Polygon antialiasing state")
	WINFO(state,W3D_ANTI_FULLSCREEN,"Fullscreen antialiasing")
	WINFO(state,W3D_DITHERING,"dithering state ")
	WINFO(state,W3D_LOGICOP,"logic operations ")
	WINFO(state,W3D_STENCILBUFFER,"stencil buffer/stencil");
	WINFO(state,W3D_ALPHATEST,"alpha test ")
	WINFO(state,W3D_SPECULAR,"Specular lighting ")
	WINFO(state,W3D_TEXMAPPING3D,"3d textures ")
	WINFO(state,W3D_SCISSOR,"Scissor test enable ")
	WINFO(state,W3D_CHROMATEST,"Chroma test enable ")
	WINFO(state,W3D_CULLFACE,"Backface culling enable ")

	if(!Wazp3D.DebugState.ON)
	{
	WINFO(action,W3D_ENABLED,"mode is enabled ")
	WINFO(action,W3D_DISABLED,"mode is disabled ")
	}

	return(action);
}
/*==========================================================================*/
BOOL SetState(W3D_Context *context,ULONG state,BOOL set)
{
/* simpler W3D_SetState used internally */

	if(set)
	{
	if(context->EnableMask & state)
		context->state = context->state | state;
	else
		WRETURN(FALSE);
	}
	else
	{
	if(context->DisableMask & state)
		context->state = context->state & ~state;
	else
		WRETURN(FALSE);
	}
	WRETURN(TRUE);
}
/*==========================================================================*/
ULONG W3D_SetState(W3D_Context *context, ULONG state, ULONG action)
{

	WAZP3D_Function(5);

	if(Wazp3D.DebugVal.ON)
	if(Wazp3D.DebugState.ON)
		if(action==W3D_DISABLED)  {Libprintf("[ ] ");} else {Libprintf("[X] ");}

	WINFO(state,W3D_AUTOTEXMANAGEMENT,"automatic texture management ")
	WINFO(state,W3D_SYNCHRON,"wait,until HW is idle ")
	WINFO(state,W3D_INDIRECT,"drawing is queued until flushed ")
	WINFO(state,W3D_GLOBALTEXENV,"SetTexEnv is global ")
	WINFO(state,W3D_DOUBLEHEIGHT,"Drawing area is double height ")
	WINFO(state,W3D_FAST,"Allow Warp3D to modify passed structures")
	WINFO(state,W3D_AUTOCLIP,"clip to screen region ")
	WINFO(state,W3D_TEXMAPPING,"texmapping state ")
	WINFO(state,W3D_PERSPECTIVE,"perspective correction state ")
	WINFO(state,W3D_GOURAUD,"gouraud/flat shading ")
	WINFO(state,W3D_ZBUFFER,"Z-Buffer state ")
	WINFO(state,W3D_ZBUFFERUPDATE,"Z-Buffer update state ")
	WINFO(state,W3D_BLENDING,"Alpha blending state ")
	WINFO(state,W3D_FOGGING,"Fogging state ")
	WINFO(state,W3D_ANTI_POINT,"Point antialiasing state ")
	WINFO(state,W3D_ANTI_LINE,"Line antialiasing state ")
	WINFO(state,W3D_ANTI_POLYGON,"Polygon antialiasing state")
	WINFO(state,W3D_ANTI_FULLSCREEN,"Fullscreen antialiasing")
	WINFO(state,W3D_DITHERING,"dithering state ")
	WINFO(state,W3D_LOGICOP,"logic operations ")
	WINFO(state,W3D_STENCILBUFFER,"stencil buffer/stencil");
	WINFO(state,W3D_ALPHATEST,"alpha test ")
	WINFO(state,W3D_SPECULAR,"Specular lighting ")
	WINFO(state,W3D_TEXMAPPING3D,"3d textures ")
	WINFO(state,W3D_SCISSOR,"Scissor test enable ")
	WINFO(state,W3D_CHROMATEST,"Chroma test enable ")
	WINFO(state,W3D_CULLFACE,"Backface culling enable ")

	if(!Wazp3D.DebugState.ON)
	{
	WINFO(action,W3D_ENABLE,"enable mode ")
	WINFO(action,W3D_DISABLE,"disable mode ")
	}

	if(SetState(context,state,(action==W3D_ENABLE)) == FALSE)
		WRETURN(W3D_UNSUPPORTEDSTATE);

	if(state==W3D_CULLFACE)
		W3D_SetFrontFace(context,context->FrontFaceOrder);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_Hint(W3D_Context *context, ULONG mode, ULONG quality)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(6);

	if(Wazp3D.DebugState.ON)
		{
		if(quality==W3D_H_FAST)		Libprintf("[FAST] ");
		if(quality==W3D_H_AVERAGE)	Libprintf("[AVER] ");
		if(quality==W3D_H_NICE)		Libprintf("[NICE] ");
		}
	else
		{
		WINFO(quality,W3D_H_FAST,"Low quality,fast rendering ");
		WINFO(quality,W3D_H_AVERAGE,"Medium quality and speed ");
		WINFO(quality,W3D_H_NICE,"Best quality,low speed ");
		}

	WINFO(mode,W3D_H_TEXMAPPING,"quality of general texmapping");
	WINFO(mode,W3D_H_MIPMAPPING,"quality of mipmapping ");
	WINFO(mode,W3D_H_BILINEARFILTER,"quality of bilinear filtering");
	WINFO(mode,W3D_H_MMFILTER,"quality of depth filter");
	WINFO(mode,W3D_H_PERSPECTIVE,"quality of perspective correction");
	WINFO(mode,W3D_H_BLENDING,"quality of alpha blending");
	WINFO(mode,W3D_H_FOGGING,"quality of fogging ");
	WINFO(mode,W3D_H_ANTIALIASING,"quality of antialiasing ");
	WINFO(mode,W3D_H_DITHERING,"quality of dithering ");
	WINFO(mode,W3D_H_ZBUFFER,"quality of ZBuffering ");

	WC->hints[mode]=quality;

/* else W3D_ILLEGALINPUT */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_CheckDriver(void)
{
	WAZP3D_Function(7);
	return(Wazp3D.drivertype);
}
/*==========================================================================*/
ULONG W3D_LockHardware(W3D_Context *context)
{
	WAZP3D_Function(8);
	context->HWlocked=TRUE;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void W3D_UnLockHardware(W3D_Context *context)
{
	WAZP3D_Function(9);
	context->HWlocked=FALSE;
}
/*==========================================================================*/
void W3D_WaitIdle(W3D_Context *context)
{
	WAZP3D_Function(10);
}
/*==========================================================================*/
ULONG W3D_CheckIdle(W3D_Context *context)
{
	WAZP3D_Function(11);
	WRETURN(W3D_SUCCESS);		/* else W3D_BUSY */
}
/*==========================================================================*/
ULONG W3D_Query(W3D_Context *context, ULONG query, ULONG destfmt)
{
	WAZP3D_Function(12);
	WRETURN(W3D_QueryDriver(context->gfxdriver,query,destfmt));
}
/*==========================================================================*/
ULONG W3D_GetTexFmtInfo(W3D_Context *context, ULONG texfmt, ULONG destfmt)
{
	WAZP3D_Function(13);
	WRETURN(W3D_GetDriverTexFmtInfo(&Wazp3D.driver,texfmt,destfmt));
}
/*==========================================================================*/
ULONG W3D_GetDriverState(W3D_Context *context)
{
	WAZP3D_Function(14);
	WRETURN(W3D_SUCCESS);		/* else W3D_NOTVISIBLE */
}
/*==========================================================================*/
ULONG W3D_GetDestFmt(void)
{
	WAZP3D_Function(15);
	return(Wazp3D.driver.formats);
}
/*==========================================================================*/
W3D_Driver **   W3D_GetDrivers(void)
{
	WAZP3D_Function(16);
	return((W3D_Driver **)&Wazp3D.DriverList);
}
/*==========================================================================*/
ULONG W3D_QueryDriver(W3D_Driver* driver, ULONG query, ULONG destfmt)
{
#define QUERY(q,doc,s) if(query==q) {sup=s; if(Wazp3D.DebugVal.ON) Libprintf(#q); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); if(Wazp3D.DebugVal.ON) Libprintf(":" #q);Libprintf("\n");}
ULONG support;
UBYTE sup;

	WAZP3D_Function(17);
	if(Wazp3D.DebugVal.ON) Libprintf("Driver<%s>\n ",Wazp3D.DriverName);
	sup='N';
	support=W3D_NOT_SUPPORTED;
	VAR(query)
	VAR(destfmt)

	if( destfmt != 0)			/* Undocumented value = StormMesa call W3D_Query() with destfmt=0 */
	if((destfmt & Wazp3D.driver.formats)==0)
		goto unsupported;

	QUERY(W3D_Q_DRAW_POINT,"point drawing ",'Y');
	QUERY(W3D_Q_DRAW_LINE,"line drawing ",'Y');
	QUERY(W3D_Q_DRAW_TRIANGLE,"triangle drawing ",'Y');
	QUERY(W3D_Q_DRAW_POINT_X,"points with size != 1 supported ",'Y');
	QUERY(W3D_Q_DRAW_LINE_X,"lines with width != 1 supported ",'N');
	QUERY(W3D_Q_DRAW_LINE_ST,"line stippling supported ",'N');
	QUERY(W3D_Q_DRAW_POLY_ST,"polygon stippling supported ",'N');
	QUERY(W3D_Q_TEXMAPPING,"texmapping in general ",'Y');
	QUERY(W3D_Q_MIPMAPPING,"mipmapping ",'Y');
	QUERY(W3D_Q_BILINEARFILTER,"bilinear filter ",'N');
	QUERY(W3D_Q_MMFILTER,"mipmap filter ",'N');
	QUERY(W3D_Q_LINEAR_REPEAT,"W3D_REPEAT for linear texmapping ",'N');
	QUERY(W3D_Q_LINEAR_CLAMP,"W3D_CLAMP for linear texmapping ",'N');
	QUERY(W3D_Q_PERSPECTIVE,"perspective correction ",'N');
	QUERY(W3D_Q_PERSP_REPEAT,"W3D_REPEAT for persp. texmapping ",'N');
	QUERY(W3D_Q_PERSP_CLAMP,"W3D_CLAMP for persp. texmapping ",'N');
	QUERY(W3D_Q_ENV_REPLACE,"texenv REPLACE ",'Y');
	QUERY(W3D_Q_ENV_DECAL,"texenv DECAL ",'Y');
	QUERY(W3D_Q_ENV_MODULATE,"texenv MODULATE ",'N');
	QUERY(W3D_Q_ENV_BLEND,"texenv BLEND ",'N');
	QUERY(W3D_Q_FLATSHADING,"flat shading ",'Y');
	QUERY(W3D_Q_GOURAUDSHADING,"gouraud shading ",'Y');
	QUERY(W3D_Q_ZBUFFER,"Z buffer in general ",'y');
	QUERY(W3D_Q_ZBUFFERUPDATE,"Z buffer update ",'y');
	QUERY(W3D_Q_ZCOMPAREMODES,"Z buffer compare modes ",'y');
	QUERY(W3D_Q_ALPHATEST,"alpha test in general ",'N');
	QUERY(W3D_Q_ALPHATESTMODES,"alpha test modes ",'N');
	QUERY(W3D_Q_BLENDING,"alpha blending ",'Y');
	QUERY(W3D_Q_SRCFACTORS,"source factors ",'y');
	QUERY(W3D_Q_DESTFACTORS,"destination factors ",'y');
	QUERY(W3D_Q_FOGGING,"fogging in general ",'Y');
	QUERY(W3D_Q_LINEAR,"linear fogging ",'Y');
	QUERY(W3D_Q_EXPONENTIAL,"exponential fogging ",'Y');
	QUERY(W3D_Q_S_EXPONENTIAL,"square exponential fogging ",'Y');
	QUERY(W3D_Q_ANTIALIASING,"antialiasing in general ",'N');
	QUERY(W3D_Q_ANTI_POINT,"point antialiasing ",'N');
	QUERY(W3D_Q_ANTI_LINE,"line antialiasing ",'N');
	QUERY(W3D_Q_ANTI_POLYGON,"polygon antialiasing ",'N');
	QUERY(W3D_Q_ANTI_FULLSCREEN,"fullscreen antialiasing ",'N');
	QUERY(W3D_Q_DITHERING,"dithering ",'N');
	QUERY(W3D_Q_SCISSOR,"scissor test ",'Y');
	QUERY(W3D_Q_MAXTEXWIDTH,"max. texture width ",'N');
	QUERY(W3D_Q_MAXTEXHEIGHT,"max. texture height ",'N');
	QUERY(W3D_Q_RECTTEXTURES,"rectangular textures ",'N');
	QUERY(W3D_Q_LOGICOP,"logical operations ",'N');
	QUERY(W3D_Q_MASKING,"color/index masking ",'N');
	QUERY(W3D_Q_STENCILBUFFER,"stencil buffer in general ",'N');
	QUERY(W3D_Q_STENCIL_MASK,"mask value ",'N');
	QUERY(W3D_Q_STENCIL_FUNC,"stencil functions ",'N');
	QUERY(W3D_Q_STENCIL_SFAIL,"stencil operation SFAIL ",'N');
	QUERY(W3D_Q_STENCIL_DPFAIL,"stencil operation DPFAIL ",'N');
	QUERY(W3D_Q_STENCIL_DPPASS,"stencil operation DPPASS ",'N');
	QUERY(W3D_Q_STENCIL_WRMASK,"stencil buffer supports write masking ",'N');
	QUERY(W3D_Q_PALETTECONV,"driver can use texture with a palette other than the screen palette on 8 bit screens ",'N');
	QUERY(W3D_Q_DRAW_POINT_FX,"driver supports point fx (fog, zbuffer) ",'Y');
	QUERY(W3D_Q_DRAW_POINT_TEX,"driver supports points textured ",'Y');
	QUERY(W3D_Q_DRAW_LINE_FX,"driver supports line fx ",'Y');
	QUERY(W3D_Q_DRAW_LINE_TEX,"driver supports textured lines ",'Y');
	QUERY(W3D_Q_SPECULAR,"driver supports specular reflection ",'N');
	QUERY(W3D_Q_CULLFACE,"driver supports culling of faces ",'Y');

	if(Wazp3D.HardwareLie.ON)
		support='Y';

	if(sup=='Y')
		support=W3D_FULLY_SUPPORTED;
	if(sup=='y')
		support=W3D_PARTIALLY_SUPPORTED;
	if(sup=='N')
		support=W3D_NOT_SUPPORTED;

unsupported:
	WINFO(support,W3D_FULLY_SUPPORTED,"completely supported")
	WINFO(support,W3D_PARTIALLY_SUPPORTED,"partially supported")
	WINFO(support,W3D_NOT_SUPPORTED,"not supported")

	return(support);
}
/*==========================================================================*/
ULONG W3D_GetDriverTexFmtInfo(W3D_Driver* driver, ULONG texfmt, ULONG destfmt)
{
ULONG support;

	WAZP3D_Function(18);
	support=W3D_TEXFMT_UNSUPPORTED;

	if((destfmt & Wazp3D.driver.formats)!=0)
	{
	if(texfmt==W3D_R8G8B8)
		support=W3D_TEXFMT_FAST +W3D_TEXFMT_ARGBFAST+W3D_TEXFMT_SUPPORTED;
	if(texfmt==W3D_R8G8B8A8)
		support=W3D_TEXFMT_FAST +W3D_TEXFMT_ARGBFAST+W3D_TEXFMT_SUPPORTED;
	}

	WINFO(support,W3D_TEXFMT_SUPPORTED,"format is supported,although it may be converted")
	WINFO(support,W3D_TEXFMT_FAST,"format directly supported ")
	WINFO(support,W3D_TEXFMT_CLUTFAST,"format is directly supported on LUT8 screens")
	WINFO(support,W3D_TEXFMT_ARGBFAST,"format is directly supported on 16/24 bit screens")
	WINFO(support,W3D_TEXFMT_UNSUPPORTED,"this format is unsupported,and can't be simulated")

	return(support);
}
/*==========================================================================*/
#ifdef __AROS__
AROS_UFH3(BOOL, ScreenModeFilter,
AROS_UFHA(struct Hook *, h      , A0),
AROS_UFHA(APTR         , object , A2),
AROS_UFHA(APTR         , message, A1))
{
    AROS_USERFUNC_INIT
#else
BOOL ScreenModeFilter(struct Hook* hook __asm("a0"), APTR object __asm("a2"),APTR message __asm("a1"))
{
#endif
ULONG ID=(ULONG)message;
struct DimensionInfo dims;
UWORD large,high,bits;
ULONG ok;


	ok=GetDisplayInfoData(NULL, (UBYTE*)&dims, sizeof(struct  DimensionInfo), DTAG_DIMS, ID);
	if(ok)
	{
	large=dims.Nominal.MaxX+1;
	high =dims.Nominal.MaxY+1;
	bits =dims.MaxDepth;

REMP("ScreenModeFilter[%ld]%ld X %ld X %ld\n",ID,large,high,bits);

	if( 1024 < large) return FALSE;
	if( 1024 < high ) return FALSE;

	if(Wazp3D.ASLsize)
	{
	if( large < Wazp3D.ASLminX ) return FALSE;
	if( large > Wazp3D.ASLmaxX ) return FALSE;
	if( high  < Wazp3D.ASLminY ) return FALSE;
	if( high  > Wazp3D.ASLmaxY ) return FALSE;
	}

	if(bits < 15) return FALSE;

	if(Wazp3D.OnlyRGB24.ON)
		if(bits != 24) return FALSE;

	return TRUE;
	}
	return FALSE;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
/*==========================================================================*/
ULONG W3D_RequestMode(struct TagItem *taglist)
{
W3D_Driver *driver;
ULONG tag,data;
ULONG size,format,drivertype,ModeID;
struct Library *AslBase;
struct ScreenModeRequester *requester;

#include <utility/hooks.h>
struct Hook filter;
filter.h_Entry = (HOOKFUNC)ScreenModeFilter;


	WAZP3D_Function(19);
	size=TRUE;
	driver=&Wazp3D.driver;
	Wazp3D.ASLminX=200;
	Wazp3D.ASLmaxX=1024;
	Wazp3D.ASLminY=200;
	Wazp3D.ASLmaxY=1024;
	Wazp3D.ASLsize=FALSE;

	drivertype=Wazp3D.drivertype;
	format=Wazp3D.driver.formats;

	while (taglist->ti_Tag != TAG_DONE)
	{
	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;
	if(tag==W3D_SMR_SIZEFILTER)	Wazp3D.ASLsize	=TRUE;
	if(tag==W3D_SMR_DRIVER )	driver		=(W3D_Driver *)data;

	if(tag==W3D_SMR_DESTFMT)	format		=data;
	if(tag==W3D_SMR_TYPE)		drivertype		=data;

	if(tag==ASLSM_MinWidth)		Wazp3D.ASLminX	=data;
	if(tag==ASLSM_MaxWidth)		Wazp3D.ASLmaxX	=data;
	if(tag==ASLSM_MinHeight)	Wazp3D.ASLminY	=data;
	if(tag==ASLSM_MaxHeight)	Wazp3D.ASLmaxY	=data;

	WINFO(tag,W3D_SMR_TAGS,"taglist ")
	WINFO(tag,W3D_SMR_DRIVER,"Driver to filter ")
	WINFO(tag,W3D_SMR_DESTFMT,"Dest Format to filter ")
	WINFO(tag,W3D_SMR_TYPE,"Type to filter ")
	WINFO(tag,W3D_SMR_SIZEFILTER,"Also filter size ")

	WINFO(tag,W3D_SMR_MODEMASK,"AND-Mask for modes ")
	WINFO(tag,ASLSM_MinWidth," ")
	WINFO(tag,ASLSM_MaxWidth," ")
	WINFO(tag,ASLSM_MinHeight," ")
	WINFO(tag,ASLSM_MaxHeight," ")
	VAR(data)
	}

	if ((AslBase = OpenLibrary("asl.library", 39L)))
	{
	if((requester = (struct ScreenModeRequester *)AllocAslRequestTags(
	ASL_ScreenModeRequest,
	ASLSM_TitleText,(ULONG) "Wazp3D Screen Modes ",
	ASLSM_FilterFunc,(ULONG)&filter,
	TAG_DONE )))
		{
		AslRequest(requester, NULL);
		ModeID =requester->sm_DisplayID;
		FreeAslRequest(requester);
		}
	CloseLibrary(AslBase);
	}

	VAR(ModeID)
	return(ModeID);
}
/*==========================================================================*/
ULONG		 W3D_RequestModeTags(Tag tag1, ...)
{
static ULONG tag[30];
va_list va;
WORD n=0;

	WAZP3D_Function(20);
	tag[n] = tag1;
	VAR(tag[n])
	va_start (va, tag1);
	do	 {
		n++;	tag[n]= va_arg(va, ULONG);	VAR(tag[n])
		if(n&2) if (tag[n] == TAG_DONE) break;
		}
	while (n<30);
	va_end(va);

	return (W3D_RequestMode((struct TagItem *)tag));
}
/*==========================================================================*/
W3D_Driver *	W3D_TestMode(ULONG ModeID)
{
ULONG format,bits,bytesperpixel;
W3D_Driver *driver;

	WAZP3D_Function(21);
	driver=&Wazp3D.driver;

	if( ModeID==INVALID_ID)
		driver=NULL;
	if(!IsCyberModeID(ModeID ))
		driver=NULL;

	format		=GetCyberIDAttr(CYBRIDATTR_PIXFMT,ModeID);
	bits			=GetCyberIDAttr(CYBRIDATTR_DEPTH ,ModeID);
	bytesperpixel	=GetCyberIDAttr(CYBRIDATTR_BPPIX ,ModeID);

	if(bits<15)
		driver=NULL;
	if(bytesperpixel<2)
		driver=NULL;

	if(Wazp3D.OnlyRGB24.ON)
	if(bytesperpixel!=3)
	if(bits!=24)
		driver=NULL;

	VAR(driver);
	return(driver);
}
/*==========================================================================*/
W3D_Texture	*W3D_AllocTexObj(W3D_Context *context, ULONG *error,struct TagItem *taglist)
{
W3D_Texture *texture=NULL;
struct WAZP3D_context *WC;
struct WAZP3D_texture *WT;
ULONG tag,data;
ULONG n;
APTR *MipPt;
ULONG size;
UWORD DrawMode,bits;
BOOL GotMipmaps=FALSE;
ULONG mask=1;

	WAZP3D_Function(22);
	WT=MYmalloc(sizeof(struct WAZP3D_texture),"WAZP3D_texture");
	if(WT==NULL)
		{ *error=W3D_NOMEMORY;PrintError(*error);return(NULL);}
	texture=&WT->texture;
	texture->driver=WT;

	while (taglist->ti_Tag != TAG_DONE)
	{
	tag =taglist->ti_Tag  ;		data=taglist->ti_Data ; taglist++;

	if(tag==W3D_ATO_IMAGE )		texture->texsource =(void *)data;
	if(tag==W3D_ATO_WIDTH )		texture->texwidth  =data;		 /* texture width in pixels */
	if(tag==W3D_ATO_HEIGHT)		texture->texheight =data;		 /* texture width in pixels */
	if(tag==W3D_ATO_FORMAT)		texture->texfmtsrc =data;		 /* texture format (from W3D_ATO_FORMAT) */
	if(tag==W3D_ATO_MIPMAP)
				{
						texture->mipmap=TRUE;			 /* TRUE, if mipmaps are supported */
						texture->mipmapmask=data;		 /* which mipmaps have to be generated */
				}
	if(tag==W3D_ATO_MIPMAPPTRS)	MipPt		   =(void *)data;
	if(tag==W3D_ATO_PALETTE)	texture->palette   =(void *)data;	   /* texture palette for chunky textures */

	WINFO(tag,W3D_ATO_TAGS,"tags");
	WINFO(tag,W3D_ATO_IMAGE,"texture image ")
	WINFO(tag,W3D_ATO_FORMAT,"source format ")
	WINFO(tag,W3D_ATO_WIDTH,"border width ")
	WINFO(tag,W3D_ATO_HEIGHT,"border height ")
	WINFO(tag,W3D_ATO_MIPMAP,"mipmap mask ")
	WINFO(tag,W3D_ATO_PALETTE,"texture palette ")
	WINFO(tag,W3D_ATO_MIPMAPPTRS,"array of mipmap")
	VAR(data)
	}


	WT->large=texture->texwidth ;
	WT->high =texture->texheight;

	if(Wazp3D.HardwareLie.ON)
		texture->resident=TRUE;		/* TRUE, if texture is on card */
	else
		texture->resident=FALSE;

	texture->bytesperpix=BytesPerPix(texture->texfmtsrc);

	texture->matchfmt=FALSE;		/* TRUE, if srcfmt = destfmt */
	if(texture->texfmtsrc==W3D_R8G8B8)
		{bits=24;texture->matchfmt=TRUE;}
	if(texture->texfmtsrc==W3D_R8G8B8A8)
		{bits=32;texture->matchfmt=TRUE;}


	if(Wazp3D.UseAnyTexFmt.ON)
	if(!texture->matchfmt)
		bits=ConvertFullTexture(texture);

	if(!Wazp3D.UseAnyTexFmt.ON)
	if(!texture->matchfmt)
		{MYfree(WT); *error=W3D_UNSUPPORTEDTEXFMT; return(NULL); }

	texture->dirty=FALSE;			 /* TRUE, if texture image was changed */
	texture->reserved1=FALSE;
	texture->reserved2=FALSE;

	if(texture->mipmap)
	NLOOP(16)
		{
		if(texture->mipmapmask & mask )
			texture->mipmaps[n]=NULL;
		else
			texture->mipmaps[n]=*MipPt++;		 /* mipmap images are given*/
		mask=mask*2;
		}

	texture->texdest=NULL;						 /* texture location on card */

	texture->bytesperrow=texture->texwidth*texture->bytesperpix;	/* bytes per row */
	texture->texdestsize=texture->bytesperrow*texture->texheight;	/* size of VRAM allocation */
	if(texture->mipmap)
		texture->texdestsize=texture->texdestsize+texture->texdestsize/3;

	size=65536;
	NLOOP(16)
	{
	if(size >= texture->texwidth)
		texture->texwidthexp =n;				 /* texture width exponent */
	if(size >= texture->texheight)
		texture->texheightexp=n;				 /* texture width exponent */
	size=size/2;
	}

	WC=context->driver;			/* insert new texture in textures list */
	WT->nextWT =WC->firstWT;
	WC->firstWT=WT;

	if(texture->matchfmt)
		WT->pt	=texture->texsource;
	else
		WT->pt	 =texture->texdata;

	WT->large	=texture->texwidth ;
	WT->high	=texture->texheight;
	WT->bits	=bits;
	PrintTexture(texture);

	WINFO(texture->texfmtsrc,W3D_CHUNKY,"palettized ")
	WINFO(texture->texfmtsrc,W3D_A1R5G5B5,"a rrrrr ggggg bbbbb ")
	WINFO(texture->texfmtsrc,W3D_R5G6B5,"rrrrr gggggg bbbbb ")
	WINFO(texture->texfmtsrc,W3D_R8G8B8,"rrrrrrrr gggggggg bbbbbbbb ")
	WINFO(texture->texfmtsrc,W3D_A4R4G4B4,"aaaa rrrr gggg bbbb ")
	WINFO(texture->texfmtsrc,W3D_A8R8G8B8,"aaaaaaaa rrrrrrrr gggggggg  ")
	WINFO(texture->texfmtsrc,W3D_A8,"aaaaaaaa ")
	WINFO(texture->texfmtsrc,W3D_L8,"llllllll ")
	WINFO(texture->texfmtsrc,W3D_L8A8,"llllllll aaaaaaaa ")
	WINFO(texture->texfmtsrc,W3D_I8,"iiiiiiii ")
	WINFO(texture->texfmtsrc,W3D_R8G8B8A8,"rrrrrrrr gggggggg bbbbbbbb")

	if(WT->bits==24) DrawMode=TEX24; else DrawMode=TEX32;
	WT->ST=SOFT3D_CreateTexture(WC->SC,WT->pt,WT->large,WT->high,WT->bits,DrawMode,GotMipmaps);

	PrintWT(WT);

	*error=W3D_SUCCESS;
	PrintError(*error);
	return(texture);
}
/*==========================================================================*/
W3D_Texture	*W3D_AllocTexObjTags(W3D_Context *context, ULONG *error,Tag tag1, ...)
{
static ULONG tag[30];
va_list va;
WORD n=0;

	WAZP3D_Function(23);
	tag[n] = tag1;
	VAR(tag[n])
	va_start (va, tag1);
	do	 {
		n++;	tag[n]= va_arg(va, ULONG);	VAR(tag[n])
		if(n&2) if (tag[n] == TAG_DONE) break;
		}
	while (n<30);
	va_end(va);
	return W3D_AllocTexObj(context,error,(struct TagItem *)tag);
}
/*==========================================================================*/
void W3D_FreeTexObj(W3D_Context *context, W3D_Texture *texture)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT=texture->driver;
struct WAZP3D_texture fakeWT;
struct WAZP3D_texture *thisWT=&fakeWT;
WORD Ntexture=0;


	WAZP3D_Function(24);
	PrintWT(WT);

	thisWT->nextWT=WC->firstWT;
	while(thisWT!=NULL)
	{
	VAL(Ntexture)
	if(thisWT->nextWT==WT)
		{
		REM(is Texture found)
		if(thisWT->nextWT==WC->firstWT)
			WC->firstWT=WT->nextWT;
		else
			thisWT->nextWT=WT->nextWT;
		W3D_ReleaseTexture(context,texture);
		SOFT3D_FreeTexture(WC->SC,WT->ST);
		MYfree(WT);
		}
	thisWT=thisWT->nextWT;
	Ntexture++;
	}

	PrintAllT(context);
}
/*==========================================================================*/
void W3D_ReleaseTexture(W3D_Context *context, W3D_Texture *texture)
{

	WAZP3D_Function(25);
	if(texture->texdata==NULL) REM(texture got no converted texdata);
	MYfree(texture->texdata);
}
/*==========================================================================*/
void W3D_FlushTextures(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;

	WAZP3D_Function(26);
	WT=WC->firstWT;
	while(WT!=NULL)
	 {
	W3D_ReleaseTexture(context,&WT->texture);
	WT=WT->nextWT;	/* my own linkage */
	}
}
/*==========================================================================*/
ULONG W3D_SetFilter(W3D_Context *context, W3D_Texture *texture,ULONG MinFilter, ULONG MagFilter)
{
struct WAZP3D_texture *WT=texture->driver;

	WAZP3D_Function(27);
	if(MinFilter==W3D_NEAREST)			{WT->MinFiltering=FALSE;WT->MinMimapping=FALSE;}
	if(MinFilter==W3D_LINEAR)			{WT->MinFiltering=TRUE; WT->MinMimapping=FALSE;}
	if(MinFilter==W3D_NEAREST_MIP_NEAREST)	{WT->MinFiltering=FALSE;WT->MinMimapping=TRUE;}
	if(MinFilter==W3D_LINEAR_MIP_NEAREST)	{WT->MinFiltering=TRUE; WT->MinMimapping=TRUE;}
	if(MinFilter==W3D_NEAREST_MIP_LINEAR)	{WT->MinFiltering=FALSE;WT->MinMimapping=TRUE;}
	if(MinFilter==W3D_LINEAR_MIP_LINEAR)	{WT->MinFiltering=TRUE; WT->MinMimapping=TRUE;}
	if(MagFilter==W3D_NEAREST)			{WT->MagFiltering=FALSE;}
	if(MagFilter==W3D_LINEAR)			{WT->MagFiltering=TRUE;}
	WT->MinFilter=MinFilter;
	WT->MagFilter=MagFilter;

	WINFO(MinFilter,W3D_NEAREST,"no mipmapping,no filtering ")
	WINFO(MinFilter,W3D_LINEAR,"no mipmapping,bilinear filtering ")
	WINFO(MinFilter,W3D_NEAREST_MIP_NEAREST,"mipmapping,no filtering ")
	WINFO(MinFilter,W3D_NEAREST_MIP_LINEAR,"mipmapping,bilinear filtering ")
	WINFO(MinFilter,W3D_LINEAR_MIP_NEAREST,"filtered mipmapping,no filtering ")
	WINFO(MinFilter,W3D_LINEAR_MIP_LINEAR,"mipmapping,trilinear filtering ")

	WINFO(MagFilter,W3D_NEAREST,"no mipmapping,no filtering ")
	WINFO(MagFilter,W3D_LINEAR,"no mipmapping,bilinear filtering ")
	WINFO(MagFilter,W3D_NEAREST_MIP_NEAREST,"mipmapping,no filtering ")
	WINFO(MagFilter,W3D_NEAREST_MIP_LINEAR,"mipmapping,bilinear filtering ")
	WINFO(MagFilter,W3D_LINEAR_MIP_NEAREST,"filtered mipmapping,no filtering ")
	WINFO(MagFilter,W3D_LINEAR_MIP_LINEAR,"mipmapping,trilinear filtering ")

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetTexEnv(W3D_Context *context, W3D_Texture *texture,ULONG envparam, W3D_Color *envcolor)
{
struct WAZP3D_texture *WT=texture->driver;

	WAZP3D_Function(28);
	if(envcolor!=NULL)
	{
	WT->TexEnvRGBA[0]=envcolor->r * 255.0;
	WT->TexEnvRGBA[1]=envcolor->g * 255.0;
	WT->TexEnvRGBA[2]=envcolor->b * 255.0;
	WT->TexEnvRGBA[3]=envcolor->a * 255.0;
	}
	WT->TexEnvMode=envparam;
	PrintRGBA((UBYTE *)& WT->TexEnvRGBA );

	if(StateON(W3D_GLOBALTEXENV))
	{
	context->globaltexenvmode=envparam;		/* Global texture environment mode */
	context->globaltexenvcolor[0]=envcolor->r;	/* global texture env color */
	context->globaltexenvcolor[1]=envcolor->g;
	context->globaltexenvcolor[2]=envcolor->b;
	context->globaltexenvcolor[3]=envcolor->a;
	}

	WINFO(envparam,W3D_REPLACE,"unlit texturing ")
	WINFO(envparam,W3D_DECAL,"RGB: same as W3D_REPLACE RGBA: use alpha to blend texture with primitive =lit-texturing")
	WINFO(envparam,W3D_MODULATE,"lit-texturing by modulation ")
	WINFO(envparam,W3D_BLEND,"blend with environment color ")
	PrintRGBA((UBYTE *)&WT->TexEnvRGBA);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetWrapMode(W3D_Context *context, W3D_Texture *texture,ULONG s_mode, ULONG t_mode, W3D_Color *bordercolor)
{
struct WAZP3D_texture *WT=texture->driver;

	WAZP3D_Function(29);
	WT->WrapRepeatX=(s_mode==W3D_REPEAT);
	WT->WrapRepeatY=(t_mode==W3D_REPEAT);
	if(bordercolor!=NULL)
	{
	WT->WrapRGBA[0]=bordercolor->r * 255.0;
	WT->WrapRGBA[1]=bordercolor->g * 255.0;
	WT->WrapRGBA[2]=bordercolor->b * 255.0;
	WT->WrapRGBA[3]=bordercolor->a * 255.0;
	}

	WINFO(s_mode,W3D_REPEAT,"texture is repeated ")
	WINFO(s_mode,W3D_CLAMP,"texture is clamped")

	WINFO(t_mode,W3D_REPEAT,"texture is repeated ")
	WINFO(t_mode,W3D_CLAMP,"texture is clamped ")
	PrintRGBA((UBYTE *)& WT->WrapRGBA );
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_UpdateTexImage(W3D_Context *context, W3D_Texture *texture,void *teximage, int level, ULONG *palette)
{
W3D_Scissor scissor;
ULONG srcbpr;

	WAZP3D_Function(30);
	scissor.left=0;
	scissor.top =0;
	scissor.width =texture->texwidth ;
	scissor.height=texture->texheight;
	srcbpr=texture->bytesperrow;
	WRETURN(W3D_UpdateTexSubImage(context,texture,teximage,level,palette,&scissor,srcbpr));
}
/*==========================================================================*/
ULONG W3D_UpdateTexSubImage(W3D_Context *context, W3D_Texture *texture, void *teximage,ULONG level, ULONG *palette, W3D_Scissor* scissor, ULONG srcbpr)
{
ULONG offset1,offset2;
UBYTE *RGB1;
UBYTE *RGB2;
UWORD x,y,high,large,Thigh,Tlarge,bpp1,bpp2;
ULONG format=texture->texfmtsrc;


	WAZP3D_Function(31);
	bpp1=texture->bytesperpix;
	bpp2=32/8;
	if(format==W3D_R8G8B8)	bpp2=24/8;
	if(format==W3D_R5G6B5)	bpp2=24/8;
	if(format==W3D_L8)	bpp2=24/8;

	x	=scissor->left;
	y	=scissor->top;
	high	=scissor->height;
	large	=scissor->width;
	Thigh =texture->texheight;
	Tlarge=texture->texwidth;

	offset1=texture->bytesperrow-large*bpp1;
	offset2=srcbpr-large*bpp1;

VAR(x)
VAR(y)
VAR(high )
VAR(large)
VAR(offset1)
VAR(offset2)

	RGB1=(UBYTE *)texture->texsource;
	RGB2=(UBYTE *)teximage;
	RGB1=(UBYTE *)&RGB1[(y*Tlarge + x)*bpp1];

VAR(RGB1)
VAR(RGB2)

	if(offset1==0)
	if(offset2==0)
		Libmemcpy(RGB1,RGB2,high*large);

	YLOOP(high)
	{
	XLOOP(large*bpp1)
		*RGB1++=*RGB2++;
	RGB1+=offset1;
	RGB2+=offset2;
	}

	RGB1=(UBYTE *)texture->texsource;
	RGB2=(UBYTE *)texture->texdata;
	RGB1=(UBYTE *)&RGB1[(y*Tlarge + x)*bpp1];
	RGB2=(UBYTE *)&RGB2[(y*Tlarge + x)*bpp2];
	offset2=(Tlarge-large)*bpp2;
VAR(offset2)
ConvertFullTexture(texture);
/*	ConvertBitmap(format,RGB1,RGB2,high,large,offset1,offset2,texture->palette); */

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_UploadTexture(W3D_Context *context, W3D_Texture *texture)
{
	WAZP3D_Function(32);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FreeAllTexObj(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;
struct WAZP3D_texture *nextWT;
WORD Ntexture=0;

	WAZP3D_Function(33);

	if(WC->firstWT==NULL)
		WRETURN(W3D_SUCCESS);

	WT=WC->firstWT;
	while(WT!=NULL)
	 {
VAL(Ntexture)
PrintWT(WT);
	nextWT=WT->nextWT;	/* my own linkage */
	W3D_FreeTexObj(context,&WT->texture);
	WT=nextWT;
	Ntexture++;
	}
	WC->firstWT=NULL;

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetChromaTestBounds(W3D_Context *context, W3D_Texture *texture,ULONG rgba_lower, ULONG rgba_upper, ULONG mode)
{
struct WAZP3D_texture *WT=texture->driver;
ULONG *color32;

	WAZP3D_Function(34);
	color32=(ULONG *)WT->ChromaTestMinRGBA; *color32=rgba_lower;
	color32=(ULONG *)WT->ChromaTestMaxRGBA; *color32=rgba_upper;
	WT->ChromaTestMode=mode;

	PrintRGBA((UBYTE *)&WT->ChromaTestMinRGBA);
	PrintRGBA((UBYTE *)&WT->ChromaTestMaxRGBA);
	WINFO(mode,W3D_CHROMATEST_NONE,"No chroma test ");
	WINFO(mode,W3D_CHROMATEST_INCLUSIVE,"texels in the range pass the test ");
	WINFO(mode,W3D_CHROMATEST_EXCLUSIVE,"texels in the range are rejected ");

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLine(W3D_Context *context, W3D_Line *line)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(35);
	WC->Pnb=0;
	W3D_BindTexture(context,0,line->tex);
	GetVertex(WC,&line->v1);
	GetVertex(WC,&line->v2);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawPoint(W3D_Context *context, W3D_Point *point)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(36);
	WC->PointSize=point->pointsize;
	WC->Pnb=0;
	W3D_BindTexture(context,0,point->tex);
	GetVertex(WC,&point->v1);

	SOFT3D_SetPointSize(WC->SC,(UWORD)WC->PointSize);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,1,W3D_PRIMITIVE_POINTS);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriangle(W3D_Context *context, W3D_Triangle *triangle)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(37);
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangle->tex);
	GetVertex(WC,&triangle->v1);
	GetVertex(WC,&triangle->v2);
	GetVertex(WC,&triangle->v3);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIANGLES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriFan(W3D_Context *context, W3D_Triangles *triangles)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
ULONG n;

	WAZP3D_Function(38);
	v=triangles->v;
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,v++);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIFAN);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriStrip(W3D_Context *context, W3D_Triangles *triangles)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
ULONG n;

	WAZP3D_Function(39);
	v=triangles->v;
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,v++);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRISTRIP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_Flush(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(40);
	SOFT3D_SetDrawMode(WC->SC,NULL,NODRAW);		 /* changing of current DrawMode force a flush */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLineStrip(W3D_Context *context, W3D_Lines *lines)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
ULONG n;

	WAZP3D_Function(41);
	v=lines->v;
	WC->Pnb=0;
	W3D_BindTexture(context,0,lines->tex);
NLOOP(lines->vertexcount)
	GetVertex(WC,v++);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINESTRIP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLineLoop(W3D_Context *context, W3D_Lines *lines)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
ULONG n;

	WAZP3D_Function(42);
	v=lines->v;
	WC->Pnb=0;
	W3D_BindTexture(context,0,lines->tex);
NLOOP(lines->vertexcount)
	GetVertex(WC,v++);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINELOOP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ClearDrawRegion(W3D_Context *context, ULONG color)
{
struct WAZP3D_context *WC=context->driver;
struct SOFT3D_context *SC=WC->SC;
UBYTE *ARGB=(UBYTE *)color;
UBYTE RGBA[4];

	WAZP3D_Function(43);
	RGBA[0]=ARGB[1];
	RGBA[1]=ARGB[2];
	RGBA[2]=ARGB[3];
	RGBA[3]=ARGB[0];

	if(!Wazp3D.UseColorHack.ON)
		SOFT3D_SetBackground(WC->SC,RGBA);

/*	if(!Wazp3D.UseMinUpdate.ON)		Clearing only updated region cant work with double buffering */
	{
	SC->xUpdate    =0;
	SC->yUpdate    =0;
	SC->largeUpdate=WC->large;
	SC->highUpdate =WC->high;
	}


	if(Wazp3D.UseClearDrawRegion.ON)
		FillPixelArray(&WC->rastport,SC->xUpdate,SC->yUpdate+context->yoffset,SC->largeUpdate,SC->highUpdate,color);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetAlphaMode(W3D_Context *context, ULONG mode, W3D_Float *refval)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(44);
	WC->AlphaMode=mode;
	WC->AlphaRef=(UBYTE)(*refval*255.0);

	WINFO(mode,W3D_A_NEVER,"discard incoming pixel ")
	WINFO(mode,W3D_A_LESS,"draw,if A < refvalue ")
	WINFO(mode,W3D_A_GEQUAL,"draw,if A >= refvalue ")
	WINFO(mode,W3D_A_LEQUAL,"draw,if A <= refvalue ")
	WINFO(mode,W3D_A_GREATER,"draw,if A > refvalue ")
	WINFO(mode,W3D_A_NOTEQUAL,"draw,if A != refvalue ")
	WINFO(mode,W3D_A_EQUAL,"draw,if A == refvalue ")
	WINFO(mode,W3D_A_ALWAYS,"always draw ")
	VARF(*refval)
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetBlendMode(W3D_Context *context, ULONG srcfunc, ULONG dstfunc)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(45);
	WC->SrcFunc=srcfunc;
	WC->DstFunc=dstfunc;

	WINFO(srcfunc,W3D_ZERO,"source + dest ")
	WINFO(srcfunc,W3D_ONE,"source + dest ")
	WINFO(srcfunc,W3D_SRC_COLOR,"dest only ")
	WINFO(srcfunc,W3D_DST_COLOR,"source only ")
	WINFO(srcfunc,W3D_ONE_MINUS_SRC_COLOR,"dest only ")
	WINFO(srcfunc,W3D_ONE_MINUS_DST_COLOR,"source only ")
	WINFO(srcfunc,W3D_SRC_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_ONE_MINUS_SRC_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_DST_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_ONE_MINUS_DST_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_SRC_ALPHA_SATURATE,"source only ")
	WINFO(srcfunc,W3D_CONSTANT_COLOR," ");
	WINFO(srcfunc,W3D_ONE_MINUS_CONSTANT_COLOR," ");
	WINFO(srcfunc,W3D_CONSTANT_ALPHA," ");
	WINFO(srcfunc,W3D_ONE_MINUS_CONSTANT_ALPHA," ");

	WINFO(dstfunc,W3D_ZERO,"source + dest ")
	WINFO(dstfunc,W3D_ONE,"source + dest ")
	WINFO(dstfunc,W3D_SRC_COLOR,"dest only ")
	WINFO(dstfunc,W3D_DST_COLOR,"source only ")
	WINFO(dstfunc,W3D_ONE_MINUS_SRC_COLOR,"dest only ")
	WINFO(dstfunc,W3D_ONE_MINUS_DST_COLOR,"source only ")
	WINFO(dstfunc,W3D_SRC_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_ONE_MINUS_SRC_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_DST_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_ONE_MINUS_DST_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_SRC_ALPHA_SATURATE,"source only ")
	WINFO(dstfunc,W3D_CONSTANT_COLOR," ");
	WINFO(dstfunc,W3D_ONE_MINUS_CONSTANT_COLOR," ");
	WINFO(dstfunc,W3D_CONSTANT_ALPHA," ");
	WINFO(dstfunc,W3D_ONE_MINUS_CONSTANT_ALPHA," ");

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(46);
	VAR(yoffset)

	WC->CallSetDrawRegion=TRUE;
	CopyImageToDrawRegion(context);	/*draw the image if any */

	context->yoffset=yoffset;
	Libmemcpy(&context->scissor,scissor,sizeof(W3D_Scissor));
	if(bm!=NULL)
		context->drawregion=bm;
	WC->rastport.BitMap=context->drawregion;

	WC->Xmin=scissor->left;
	WC->Ymin=scissor->top;
	WC->Xmax=WC->Xmin+scissor->width-1;
	WC->Ymax=WC->Ymin+scissor->height-1;
	if(WC->Xmin<0) WC->Xmin=0;
	if(WC->Ymin<0) WC->Ymin=0;
	if(WC->large<=WC->Xmax) WC->Xmax=WC->large-1;
	if(WC->high <=WC->Ymax) WC->Ymax=WC->high -1;

VAR(WC->Xmin)
VAR(WC->Ymin)
VAR(WC->Xmax)
VAR(WC->Ymax)
	SOFT3D_SetClipping(WC->SC,(float)WC->Xmin,(float)WC->Xmax,(float)WC->Ymin,(float)WC->Ymax,0.0,0.999);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetDrawRegionWBM(W3D_Context *context, W3D_Bitmap *bm,W3D_Scissor *scissor)
{
ULONG error;

	WAZP3D_Function(47);
	context->w3dbitmap=TRUE;
	error=W3D_SetDrawRegion(context,(struct BitMap *)bm,context->yoffset,scissor);
	WRETURN(error);
}
/*==========================================================================*/
ULONG W3D_SetFogParams(W3D_Context *context, W3D_Fog *fogparams,ULONG fogmode)
{
struct WAZP3D_context *WC=context->driver;


	WAZP3D_Function(48);
	WC->FogMode=0;
	if(fogmode==W3D_FOG_LINEAR)	WC->FogMode=1;
	if(fogmode==W3D_FOG_EXP)	WC->FogMode=2;
	if(fogmode==W3D_FOG_EXP_2)	WC->FogMode=3;

	WC->FogRGBA[0]	=fogparams->fog_color.r*255.0;
	WC->FogRGBA[1]	=fogparams->fog_color.g*255.0;
	WC->FogRGBA[2]	=fogparams->fog_color.b*255.0;
	WC->FogRGBA[3]	=255;
	WC->FogZmin		=fogparams->fog_end;		/* end is zmin !!! */
	WC->FogZmax		=fogparams->fog_start;
	WC->FogDensity	=fogparams->fog_density;
	Libmemcpy(&context->fog,fogparams,sizeof(W3D_Fog));

	WINFO(fogmode,W3D_FOG_LINEAR,"linear fogging ")
	WINFO(fogmode,W3D_FOG_EXP,"exponential fogging ")
	WINFO(fogmode,W3D_FOG_EXP_2,"square exponential fogging ")
	WINFO(fogmode,W3D_FOG_INTERPOLATED,"interpolated fogging ")
	PrintRGBA((UBYTE *)&WC->FogRGBA);
	VARF(fogparams->fog_start)
	VARF(fogparams->fog_end)
	VARF(fogparams->fog_density)

	SOFT3D_Fog(WC->SC,WC->FogMode,WC->FogZmin,WC->FogZmax,WC->FogDensity,WC->FogRGBA);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetLogicOp(W3D_Context *context, ULONG operation)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(49);
	WC->LogicOp=operation;
	WINFO(operation,W3D_LO_CLEAR,"dest = 0 ");
	WINFO(operation,W3D_LO_AND,"dest = source AND dest ");
	WINFO(operation,W3D_LO_AND_REVERSE,"dest = source AND NOT dest ");
	WINFO(operation,W3D_LO_COPY,"dest = source ");
	WINFO(operation,W3D_LO_AND_INVERTED,"dest = NOT source AND dest ");
	WINFO(operation,W3D_LO_NOOP,"dest = dest ");
	WINFO(operation,W3D_LO_XOR,"dest = source XOR dest ");
	WINFO(operation,W3D_LO_OR,"dest = source OR dest ");
	WINFO(operation,W3D_LO_NOR,"dest = NOT (source OR dest) ");
	WINFO(operation,W3D_LO_EQUIV,"dest = NOT (source XOR dest) ");
	WINFO(operation,W3D_LO_INVERT,"dest = NOT  dest ");
	WINFO(operation,W3D_LO_OR_REVERSE,"dest = source OR NOT dest ");
	WINFO(operation,W3D_LO_COPY_INVERTED,"dest = NOT source ");
	WINFO(operation,W3D_LO_OR_INVERTED,"dest = NOT source OR dest ");
	WINFO(operation,W3D_LO_NAND,"dest = NOT (source AND dest) ");
	WINFO(operation,W3D_LO_SET,"dest = 1 ");
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetColorMask(W3D_Context *context, W3D_Bool Red, W3D_Bool Green,W3D_Bool Blue, W3D_Bool Alpha)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(50);
	if(Red)	WC->MaskRGBA[0]=255; else WC->MaskRGBA[0]=0;
	if(Green)	WC->MaskRGBA[1]=255; else WC->MaskRGBA[1]=0;
	if(Blue)	WC->MaskRGBA[2]=255; else WC->MaskRGBA[2]=0;
	if(Alpha)	WC->MaskRGBA[3]=255; else WC->MaskRGBA[3]=0;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetPenMask(W3D_Context *context, ULONG pen)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(51);
	VAR(pen)
	WC->PenMask=pen;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetCurrentColor(W3D_Context *context, W3D_Color *color)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(52);
	if(color!=NULL)
	{
	WC->CurrentRGBA[0]=color->r * 255.0;
	WC->CurrentRGBA[1]=color->g * 255.0;
	WC->CurrentRGBA[2]=color->b * 255.0;
	WC->CurrentRGBA[3]=color->a * 255.0;
	}
	PrintRGBA((UBYTE *) WC->CurrentRGBA);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetCurrentPen(W3D_Context *context, ULONG pen)
{
struct WAZP3D_context *WC=context->driver;
UBYTE RGBA[4];

	WAZP3D_Function(53);
/* horrible hack to recover pen RGBA values . Todo: find another way*/
	SetAPen (&WC->rastport,pen);
	RectFill(&WC->rastport,0,0,1,1);
	STACKReadPixelArray(RGBA,0,0,1*(32/8),&WC->rastport,0,0,1,1,RECTFMT_RGBA);

	COPYRGBA(WC->CurrentRGBA,RGBA);
	PrintRGBA((UBYTE *) WC->CurrentRGBA);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void W3D_SetScissor(W3D_Context *context, W3D_Scissor *scissor)
{
	WAZP3D_Function(54);
	W3D_SetDrawRegion(context,context->drawregion,0,scissor);
}
/*==========================================================================*/
void W3D_FlushFrame(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(55);
	WC->CallFlushFrame=TRUE;
	CopyImageToDrawRegion(context);
}
/*==========================================================================*/
ULONG W3D_AllocZBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(56);
	if(context->zbufferalloc==TRUE)	 WRETURN(W3D_SUCCESS);	/* already allocated ? */

	context->zbuffer=MYmalloc(WC->large*WC->high*16/8,"Zbuffer16");
	if(context->zbuffer==NULL) WRETURN(W3D_NOGFXMEM);
	context->zbufferalloc=TRUE;
	context->zbufferlost =FALSE;		 /* Is it TRUE if just allocated ?!? */

	SOFT3D_SetZbuffer(WC->SC,context->zbuffer);
	SOFT3D_ClearZBuffer(WC->SC,ZMAX);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FreeZBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(57);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	MYfree(context->zbuffer);

	SOFT3D_SetZbuffer(WC->SC,NULL);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ClearZBuffer(W3D_Context *context, W3D_Double *clearvalue)
{
struct WAZP3D_context *WC=context->driver;
register WORD z16;
float z=*clearvalue;
float resizez=ZMAX;

	WAZP3D_Function(58);
	WC->CallClearZBuffer=TRUE;
	VARF(*clearvalue);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	if (z < 0.000)	z=0.000;
	if (0.999 < z)	z=0.999;
	z16=resizez*z;
	VAR(z16);
	SOFT3D_ClearZBuffer(WC->SC,z16);
	CopyImageToDrawRegion(context);    /*draw the image if any */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ReadZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z)
{
	WAZP3D_Function(59);
	VAR(x);
	VAR(y);
	VAR(z);
	WRETURN(W3D_ReadZSpan(context,x,y,1,z));
}
/*==========================================================================*/
ULONG W3D_ReadZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z)
{
struct WAZP3D_context *WC=context->driver;
register WORD *Zbuffer16=context->zbuffer;
register ULONG offset;
W3D_Double resizez=ZMAX;

	WAZP3D_Function(60);
	VAR(x);
	VAR(y);
	VAR(n);
	VAR(z);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	offset=x+y*WC->large;
	Zbuffer16=&(Zbuffer16[offset]);
	XLOOP(n)
		z[x]=((W3D_Double)Zbuffer16[x])/resizez;

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetZCompareMode(W3D_Context *context, ULONG mode)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(61);
	WC->ZbufferMode=mode;

	WINFO(mode,W3D_Z_NEVER,"discard incoming pixel ")
	WINFO(mode,W3D_Z_LESS,"draw,if Z < Zbuffer ")
	WINFO(mode,W3D_Z_GEQUAL,"draw,if Z >= Zbuffer ")
	WINFO(mode,W3D_Z_LEQUAL,"draw,if Z <= Zbuffer ")
	WINFO(mode,W3D_Z_GREATER,"draw,if Z > Zbuffer ")
	WINFO(mode,W3D_Z_NOTEQUAL,"draw,if Z != Zbuffer ")
	WINFO(mode,W3D_Z_EQUAL,"draw,if Z == Zbuffer ")
	WINFO(mode,W3D_Z_ALWAYS,"always draw ")
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void W3D_WriteZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z)
{
UBYTE mask=1;

	WAZP3D_Function(62);
	VAR(x);
	VAR(y);
	VARF(*z);
/*	WRETURN(W3D_WriteZSpan(context,x,y,1,z,&mask)); */
	W3D_WriteZSpan(context,x,y,1,z,&mask);
}
/*==========================================================================*/
void W3D_WriteZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *zD, UBYTE *mask)
{
struct WAZP3D_context *WC=context->driver;
register WORD *Zbuffer16 =context->zbuffer;
register ULONG offset;
register float z;
float resizez=ZMAX;

	WAZP3D_Function(63);
/*	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER); */
	if(context->zbuffer==NULL) return;

	offset=x+y*WC->large;
	Zbuffer16=&(Zbuffer16[offset]);
	VAR(x);
	VAR(y);
	VAR(n);
	VAR(mask)
	XLOOP(n)
		if(mask[x]==1)
			{
			z=zD[x];
			if (z < 0.000)	z=0.000;
			if (0.999 < z)	z=0.999;
			VAR(z);
			Zbuffer16[x]=resizez*z;
			}
/*	WRETURN(W3D_SUCCESS);*/
}
/*==========================================================================*/
ULONG W3D_AllocStencilBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
ULONG clearvalue[1];

	WAZP3D_Function(64);
	if(context->stbufferalloc==TRUE)	 WRETURN(W3D_SUCCESS);

	context->stencilbuffer=MYmalloc(WC->large*WC->high*8/8,"Stencil8");
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOGFXMEM);
	context->stbufferalloc=TRUE;
	clearvalue[0]=0;
	W3D_ClearStencilBuffer(context,(ULONG *)clearvalue);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ClearStencilBuffer(W3D_Context *context, ULONG *clearvalue)
{
struct WAZP3D_context *WC=context->driver;
UBYTE s8;

	WAZP3D_Function(65);
	VAR(*clearvalue)
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	s8=*clearvalue;  /* todo: enhance this conversion */
	memset(context->stencilbuffer,s8,WC->large*WC->high*8/8);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FillStencilBuffer(W3D_Context *context, ULONG x, ULONG y,ULONG width, ULONG height, ULONG depth,void *data)
{
struct WAZP3D_context *WC=context->driver;
register UBYTE *Stencil8=context->stencilbuffer;
register ULONG offset;
UBYTE *data8 =data;
UWORD *data16=data;
ULONG *data32=data;

	WAZP3D_Function(66);
	VAR(x);
	VAR(y);
	VAR(width);
	VAR(height);
	VAR(depth);
	VAR(data);
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	offset=x+y*WC->large;
	Stencil8+=offset;
	offset=WC->large-width;
	if(depth==8)
		YLOOP(height)
			{
			XLOOP(width)
				*Stencil8++=*data8++;
			Stencil8+=offset;
			}
	if(depth==16)
		YLOOP(height)
			{
			XLOOP(width)
				*Stencil8++=*data16++;
			Stencil8+=offset;
			}
	if(depth==32)
		YLOOP(height)
			{
			XLOOP(width)
				*Stencil8++=*data32++;
			Stencil8+=offset;
			}
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FreeStencilBuffer(W3D_Context *context)
{

	WAZP3D_Function(67);
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	MYfree(context->stencilbuffer);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ReadStencilPixel(W3D_Context *context, ULONG x, ULONG y,ULONG *st)
{
	WAZP3D_Function(68);
	VAR(x);
	VAR(y);
	VAR(st);
	WRETURN(W3D_ReadStencilSpan(context,x,y,1,st));
}
/*==========================================================================*/
ULONG W3D_ReadStencilSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, ULONG *st)
{
struct WAZP3D_context *WC=context->driver;
register UBYTE *Stencil8=context->stencilbuffer;
register ULONG offset;

	WAZP3D_Function(69);
	VAR(x);
	VAR(y);
	VAR(n);
	VAR(st);
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	offset=x+y*WC->large;
	Stencil8+=offset;
	XLOOP(n)
		st[x]=Stencil8[x];
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetStencilFunc(W3D_Context *context, ULONG func, ULONG refvalue,ULONG mask)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(70);
	WC->StencilFunc=func;
	WC->StencilRefValue=refvalue;
	WC->StencilMask=mask;

	WINFO(func,W3D_ST_NEVER,"don't draw pixel ")
	WINFO(func,W3D_ST_ALWAYS,"draw always ")
	WINFO(func,W3D_ST_LESS,"draw,if refvalue < ST ")
	WINFO(func,W3D_ST_LEQUAL,"draw,if refvalue <= ST ")
	WINFO(func,W3D_ST_EQUAL,"draw,if refvalue == ST ")
	WINFO(func,W3D_ST_GEQUAL,"draw,if refvalue >= ST ")
	WINFO(func,W3D_ST_GREATER,"draw,if refvalue > ST ")
	WINFO(func,W3D_ST_NOTEQUAL,"draw,if refvalue != ST ")
	VAR(refvalue)
	VAR(mask)
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetStencilOp(W3D_Context *context, ULONG sfail, ULONG dpfail,ULONG dppass)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(71);
	WC->StencilDpfail=dpfail;
	WC->StencilDppass=dppass;

	WINFO(sfail,W3D_ST_KEEP,"keep stencil buffer value ")
	WINFO(sfail,W3D_ST_ZERO,"clear stencil buffer value ")
	WINFO(sfail,W3D_ST_REPLACE,"replace by reference value ")
	WINFO(sfail,W3D_ST_INCR,"increment ")
	WINFO(sfail,W3D_ST_DECR,"decrement ")
	WINFO(sfail,W3D_ST_INVERT,"invert bitwise ")

	WINFO(dpfail,W3D_ST_KEEP,"keep stencil buffer value ")
	WINFO(dpfail,W3D_ST_ZERO,"clear stencil buffer value ")
	WINFO(dpfail,W3D_ST_REPLACE,"replace by reference value ")
	WINFO(dpfail,W3D_ST_INCR,"increment ")
	WINFO(dpfail,W3D_ST_DECR,"decrement ")
	WINFO(dpfail,W3D_ST_INVERT,"invert bitwise ")

	WINFO(dppass,W3D_ST_KEEP,"keep stencil buffer value ")
	WINFO(dppass,W3D_ST_ZERO,"clear stencil buffer value ")
	WINFO(dppass,W3D_ST_REPLACE,"replace by reference value ")
	WINFO(dppass,W3D_ST_INCR,"increment ")
	WINFO(dppass,W3D_ST_DECR,"decrement ")
	WINFO(dppass,W3D_ST_INVERT,"invert bitwise ")
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetWriteMask(W3D_Context *context, ULONG mask)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(72);
	VAR(mask)
	WC->StencilWriteMask=mask;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_WriteStencilPixel(W3D_Context *context, ULONG x, ULONG y, ULONG st)
{
UBYTE mask=1;
ULONG stencilvalue=st;

	WAZP3D_Function(73);
	VAR(x)
	VAR(y)
	VAR(st)
	WRETURN(W3D_WriteStencilSpan(context,x,y,1,&stencilvalue,&mask));
}
/*==========================================================================*/
ULONG W3D_WriteStencilSpan(W3D_Context *context, ULONG x, ULONG y, ULONG n,ULONG *st, UBYTE *mask)
{
struct WAZP3D_context *WC=context->driver;
register UBYTE *Stencil8=context->stencilbuffer;
register ULONG offset;

	WAZP3D_Function(74);
	VAR(x)
	VAR(y)
	VAR(n)
	VAR(st)
	VAR(mask)

	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	offset=x+y*WC->large;
	Stencil8+=offset;
	XLOOP(n)
		if(mask[x]==1)
			Stencil8[x]=st[x];
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriangleV(W3D_Context *context, W3D_TriangleV *triangle)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(75);
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangle->tex);
	GetVertex(WC,triangle->v1);
	GetVertex(WC,triangle->v2);
	GetVertex(WC,triangle->v3);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIANGLES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriFanV(W3D_Context *context, W3D_TrianglesV *triangles)
{
struct WAZP3D_context *WC=context->driver;
ULONG n;

	WAZP3D_Function(76);
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,triangles->v[n]);

	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIFAN);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriStripV(W3D_Context *context, W3D_TrianglesV *triangles)
{
struct WAZP3D_context *WC=context->driver;
ULONG n;

	WAZP3D_Function(77);
	WC->Pnb=0;
	W3D_BindTexture(context,0,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,triangles->v[n]);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRISTRIP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
W3D_ScreenMode *W3D_GetScreenmodeList(void)
{
struct CyberModeNode *cmode;
struct CyberModeNode *cmodelist;
W3D_ScreenMode *smode=(W3D_ScreenMode *)&Wazp3D.smodelist;
WORD n=0;

	WAZP3D_Function(78);
	cmodelist=(struct CyberModeNode *)AllocCModeListTags(CYBRMREQ_MinDepth,16,CYBRMREQ_MaxDepth,32,TAG_DONE);

	if (cmodelist)
	{
	cmode=cmodelist;
	while ( (cmode=(struct CyberModeNode *)cmode->Node.ln_Succ) ->Node.ln_Succ )
	if(n<50)
		{

			smode->ModeID	=cmode->DisplayID;
			smode->Width	=cmode->Width;
			smode->Height	=cmode->Height;
			smode->Depth	=cmode->Depth;
			Libstrcpy(smode->DisplayName,cmode->ModeText);
			smode->Driver	=W3D_TestMode(smode->ModeID);
			smode->Next		=&(smode[1]);
			smode++;
			n++;
		}
	smode->Next=NULL;
	}
	FreeCModeList((struct List *)cmodelist);
	return(NULL);
}
/*==========================================================================*/
void W3D_FreeScreenmodeList(W3D_ScreenMode *list)
{
	WAZP3D_Function(79);
	/* list is included in WC so cant be freed */
}
/*==========================================================================*/
ULONG		 W3D_BestModeID(struct TagItem *taglist)
{
ULONG tag,data;
ULONG driver,ModeID;
ULONG large,high,bits;

	WAZP3D_Function(80);
	driver=(ULONG)&Wazp3D.driver;
	large	=320;
	high	=240;
	bits  =32;

	while (taglist->ti_Tag != TAG_DONE)
	{
	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;
	if(tag==W3D_BMI_DRIVER)	driver=data;
	if(tag==W3D_BMI_WIDTH)	large	=data;
	if(tag==W3D_BMI_HEIGHT)	high	=data;
	if(tag==W3D_BMI_DEPTH)	bits	=data;
	WINFO(tag,W3D_BMI_TAGS," ")
	WINFO(tag,W3D_BMI_DRIVER," ")
	WINFO(tag,W3D_BMI_WIDTH," ")
	WINFO(tag,W3D_BMI_HEIGHT," ")
	WINFO(tag,W3D_BMI_DEPTH," ")
	VAR(data)
	}

	 if(Wazp3D.OnlyRGB24.ON)
		bits=24;

	ModeID = BestCModeIDTags(
		CYBRBIDTG_Depth		,bits,
		CYBRBIDTG_NominalWidth	,large,
		CYBRBIDTG_NominalHeight	,high,
		TAG_DONE);

	if( W3D_TestMode(ModeID) == NULL)
		return(INVALID_ID);
	else
		return(ModeID);
}
/*==========================================================================*/
ULONG		 W3D_BestModeIDTags(Tag tag1, ...)
{
static ULONG tag[30];
va_list va;
WORD n=0;

	WAZP3D_Function(81);
	tag[n] = tag1;
	VAR(tag[n])
	va_start (va, tag1);
	do	 {
		n++;	tag[n]= va_arg(va, ULONG);	VAR(tag[n])
		if(n&2) if (tag[n] == TAG_DONE) break;
		}
	while (n<30);
	va_end(va);

	return (W3D_BestModeID((struct TagItem *)tag));
}
/*==========================================================================*/
ULONG W3D_VertexPointer(W3D_Context* context, void *pointer, int stride,ULONG mode, ULONG flags)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(82);
	WC->V=pointer;
	WC->Vstride=stride;
	WC->Vformat=mode;

	context->VertexPointer=pointer;		/* Pointer to the vertex buffer array */
	context->VPStride=stride;			 /* Stride of vertex array */
	context->VPMode=mode;				 /* Vertex buffer format */
	context->VPFlags=flags;				 /* not yet used */

	VAR(pointer)
	VAR(stride)
	WINFO(mode,W3D_VERTEX_F_F_F," ")
	WINFO(mode,W3D_VERTEX_F_F_D," ")
	WINFO(mode,W3D_VERTEX_D_D_D," ")
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_TexCoordPointer(W3D_Context* context, void *pointer, int stride,int unit, int off_v, int off_w, ULONG flags)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(83);
	VAR(pointer)
	VAR(stride)
	VAR(unit)
	VAR(off_v)
	VAR(off_w)
	WINFO(flags,W3D_TEXCOORD_NORMALIZED,"Texture coordinates are normalized ")
	if(W3D_MAX_TMU <= unit)
		WRETURN(W3D_ILLEGALINPUT);

	WC->UV=pointer;
	WC->UVstride=stride;
	WC->UVformat=flags;
	WC->UVoff_v=off_v;

	context->TexCoordPointer[unit]=pointer;
	context->TPStride[unit]=stride;		 /* Stride of TexCoordPointers */
	context->TPVOffs[unit]=off_v;			/* Offset to V coordinate */
	context->TPWOffs[unit]=off_w;			/* Offset to W coordinate */
	context->TPFlags[unit]=flags;			/* Flags */

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ColorPointer(W3D_Context* context, void *pointer, int stride,ULONG format, ULONG mode, ULONG flags)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(84);
	WC->RGBA=pointer;
	WC->RGBAstride=stride;
	WC->RGBAformat=format;
	WC->RGBAorder=mode;

	context->ColorPointer=pointer;	/* Pointer to the color array */
	context->CPStride=stride;		 /* Color pointer stride */
	context->CPMode=mode & format;	/* Mode and color format */
	context->CPFlags=flags;			 /* not yet used */


	VAR(pointer)
	VAR(stride)
	WINFO(format,W3D_COLOR_FLOAT," ")
	WINFO(format,W3D_COLOR_UBYTE," ")
	WINFO(mode,W3D_CMODE_RGB," ")
	WINFO(mode,W3D_CMODE_BGR," ")
	WINFO(mode,W3D_CMODE_RGBA," ")
	WINFO(mode,W3D_CMODE_ARGB," ")
	WINFO(mode,W3D_CMODE_BGRA," ")
	VAR(flags)

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_BindTexture(W3D_Context* context, ULONG tmu, W3D_Texture *texture)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;
WORD DrawMode=0;
BOOL fog,color,blend,tex,zbuf;

	WAZP3D_Function(85);
	VAR(tmu)
	VAR(texture)
	PrintTexture(texture);

	if(W3D_MAX_TMU <= tmu)
		WRETURN(W3D_ILLEGALINPUT);
	context->CurrentTex[tmu]=texture;

	fog  =(StateON(W3D_FOGGING));
	color=(StateON(W3D_GOURAUD));
	tex  =(StateON(W3D_TEXMAPPING));
	zbuf =(StateON(W3D_ZBUFFER));
	blend=(StateON(W3D_BLENDING));


	if(!fog)
		SOFT3D_Fog(WC->SC,0,WC->FogZmin,WC->FogZmax,WC->FogDensity,WC->FogRGBA);

	if(texture==NULL)
		{
REM(TEXTURE_NULL)
		WC->uresize=1.0;
		WC->vresize=1.0;
		tex  = FALSE;
		}
	else
		{
REM(TEXTURE_NOT_NULL)
		WT=texture->driver;
		WC->WT=WT;
		WC->uresize=(float)(WT->high );
		WC->vresize=(float)(WT->large);
		PrintWT(WT);
		}


	if(tex)
	if(zbuf)
		DrawMode=TEX24;

	if(tex)
	if(!zbuf)
		DrawMode=TEX24ZOFF;

	if(tex)
	if(zbuf)
	if(blend)
	if(WT->bits==32)
		DrawMode=TEX32;

	if(!tex)
	if(!zbuf)
	if(!blend)
		DrawMode=COLOR24ZOFF;

	if(!tex)
	if(zbuf)
	if(!blend)
		DrawMode=COLOR24;

	if(!tex)
	if(!zbuf)
	if(blend)
		DrawMode=COLOR32ZOFF;

	if(!tex)
	if(zbuf)
	if(blend)
		DrawMode=COLOR32;

REMP("fog%ld color%ld tex%ld zbuf%ld blend%ld -> DrawMode%ld \n",fog,color,tex,zbuf,blend,DrawMode);

	SOFT3D_SetDrawMode(WC->SC,WT->ST,DrawMode);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawArray(W3D_Context* context, ULONG primitive, ULONG base, ULONG count)
{
struct WAZP3D_context *WC=context->driver;
ULONG i=base;
ULONG n;

	WAZP3D_Function(86);
	WC->primitive=primitive;
	WC->Pnb=0;
	NLOOP(count)
		GetPoint(WC,i++);

	WINFO(primitive,W3D_PRIMITIVE_TRIANGLES," ")
	WINFO(primitive,W3D_PRIMITIVE_TRIFAN," ")
	WINFO(primitive,W3D_PRIMITIVE_TRISTRIP," ")
	WINFO(primitive,W3D_PRIMITIVE_POINTS," ")
	WINFO(primitive,W3D_PRIMITIVE_LINES," ")
	WINFO(primitive,W3D_PRIMITIVE_LINELOOP," ")
	WINFO(primitive,W3D_PRIMITIVE_LINESTRIP," ")
	VAR(base)
	VAR(count)
	if(primitive==W3D_PRIMITIVE_POINTS) SOFT3D_SetPointSize(WC->SC,1);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,WC->primitive);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawElements(W3D_Context* context, ULONG primitive, ULONG type, ULONG count,void *indices)
{
struct WAZP3D_context *WC=context->driver;
UBYTE *I8 =indices;
UWORD *I16=indices;
ULONG *I32=indices;
ULONG n;

	WAZP3D_Function(87);
	WC->primitive=primitive;
	WC->Pnb=0;

	if(type==W3D_INDEX_UBYTE)
	NLOOP(count)
		GetPoint(WC,*I8++);

	if(type==W3D_INDEX_UWORD)
	NLOOP(count)
		GetPoint(WC,*I16++);

	if(type==W3D_INDEX_ULONG)
	NLOOP(count)
		GetPoint(WC,*I32++);

	WINFO(primitive,W3D_PRIMITIVE_TRIANGLES," ")
	WINFO(primitive,W3D_PRIMITIVE_TRIFAN," ")
	WINFO(primitive,W3D_PRIMITIVE_TRISTRIP," ")
	WINFO(primitive,W3D_PRIMITIVE_POINTS," ")
	WINFO(primitive,W3D_PRIMITIVE_LINES," ")
	WINFO(primitive,W3D_PRIMITIVE_LINELOOP," ")
	WINFO(primitive,W3D_PRIMITIVE_LINESTRIP," ")
	WINFO(type,W3D_INDEX_UBYTE," ")
	WINFO(type,W3D_INDEX_UWORD," ")
	WINFO(type,W3D_INDEX_ULONG," ")
	VAR(count)
	VAR(indices)

	if(primitive==W3D_PRIMITIVE_POINTS) SOFT3D_SetPointSize(WC->SC,1);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,WC->primitive);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void		 W3D_SetFrontFace(W3D_Context* context, ULONG direction)
{
struct WAZP3D_context *WC=context->driver;
UWORD Culling;

	WAZP3D_Function(88);
	WINFO(direction,W3D_CW ,"Front face is clockwise");
	WINFO(direction,W3D_CCW,"Front face is counter clockwise");
	context->FrontFaceOrder=direction;

	if(context->FrontFaceOrder==W3D_CW)	Culling=1;
	if(context->FrontFaceOrder==W3D_CCW)	Culling=2;
	if(!StateON(W3D_CULLFACE))		Culling=0;
	SOFT3D_SetCulling(WC->SC,Culling);
}
/*==========================================================================*/
#ifdef __AROS__
void W3D_Settings(void)
{
    WAZP3D_Settings();
}
#endif
