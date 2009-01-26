/* Wazp3D gamma 41 : Alain THELLIER - Paris - FRANCE - (November 2006 to Jan 2009)   */
/* Adaptation to AROS from Matthias Rustler							*/
/* Code clean-up and library enhancements from Gunther Nikl					*/
/* LICENSE: GNU General Public License (GNU GPL) for this file				*/

/* define this one if you want the builtin debugger */
#define WAZP3DDEBUG 1
/* define this one if your CPU got same byteordering as Motorola */
#define MOTOROLAORDER 1

#if defined(__AROS__) && (AROS_BIG_ENDIAN == 0)
#undef MOTOROLAORDER
#endif

//PROVIDE_VARARG_FUNCTIONS

/*==================================================================================*/
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/asl.h>
#include <Warp3D/Warp3D.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/cybergraphics.h>
/*==================================================================================*/
struct memory3D{
	UBYTE *pt;
	ULONG size;
	char name[40];
	void *nextME;
};
/*==================================================================================*/
#define MAXPOINTS 10000				/* Maximum points per drawing-primitive	*/
#define MAXPOLY	100				/* Maximum points per polygone		*/
#define MAXPOLYHACK 5				/* Maximum points per polygone in PolyHack*/
#define MAXSCREEN  1024				/* Maximum scren   size 1024x1024		*/
#define MAXTEXTURE 2048				/* Maximum tex size 2048x2048 (really 256)*/
#define ZMAX 8192					/* Zbuffer resolution in 16 bits		*/
#define CONVERTUV (255.0*(256.0*256.0))	/* value to convert uv float to integer 	*/
#define CONVERTZ ((float)ZMAX)		/* value to convert  z float to integer 	*/
#define ALPHAMIN 16				/* Alpha range : A < ALPHAMIN ==> transp. */
#define ALPHAMAX (255-ALPHAMIN)		/* Alpha range : ALPHAMAX < A ==> solid	*/
#define PIXBUFFERSIZE 10000			/* Pixels buffer 					*/
extern BOOL LibDebug;				/* Enable Library Debugger (global)		*/
struct memory3D *firstME=NULL;		/* Tracked memory-allocation	  		*/
/*==================================================================================*/
#define DRIVERNAME "Wazp3D soft renderer - Alain Thellier - Paris France 2009 - gamma 41"
UBYTE Wazp3DPrefsName[] = {"Wazp3D Prefs.Gamma 41.Alain Thellier 2009"};
ULONG Wazp3DPrefsNameTag=(ULONG) Wazp3DPrefsName;
/*==================================================================================*/
struct vertex3D{
	float x,y,z;
};
/*==================================================================================*/
struct point3D{
	float x,y,z;
	float u,v;
	UBYTE RGBA[4];
};
/*==================================================================================*/
struct MyButton{
	BOOL ON;
	char name[30];
	};
/*==================================================================================*/
struct WAZP3D_parameters{
char  FunctionName[89][50];
ULONG FunctionCalls[89];
ULONG FunctionCallsAll;
ULONG CrashFunctionCall;		/* begin to step the 3Dprog after this call */
char  DriverName[256];
ULONG drivertype;
W3D_Driver	driver;
W3D_Driver *DriverList[2];
W3D_ScreenMode *smode;
W3D_ScreenMode smodelist[50];
BOOL  ASLsize;
ULONG ASLminX,ASLmaxX,ASLminY,ASLmaxY;
struct MyButton HardwareLie;		/* pretend to be a perfect hardware driver */
struct MyButton TexFmtLie;		
struct MyButton HackARGB;
struct MyButton OnlyTrueColor;	/* only use fast RGB/RGBA screens */
struct MyButton UsePolyHack;		/* convert trifan & tristrip to quad */
struct MyButton UseColorHack;		/* get background color from bitmap */
struct MyButton UseCullingHack;
struct MyButton UseFog;
struct MyButton UseColoring;		/*  do simple coloring */
struct MyButton UseColoringGL;	/* v40:emulate truly decal/blend/modulate */
struct MyButton UseClearDrawRegion;	/* let Warp3D clear his bitmap */
struct MyButton UseClearImage;	/* let Wazp3D clear his RGBA buffer */
struct MyButton UseMinUpdate;
struct MyButton UpdateAtUnlock;
struct MyButton UpdateClearZ;
struct MyButton HackRGBA1;
struct MyButton HackRGBA2;
struct MyButton UseAntiScreen;
struct MyButton SmoothTextures;
struct MyButton ReloadTextures;
struct MyButton UseClipper;
struct MyButton NoPerspective;
struct MyButton TruePerspective;
struct MyButton DirectBitmap;		/* v39: directly hit in the screen bitmap so no RGBA buffer & no update */
struct MyButton UseRatioAlpha;	/* v40: force TexMode to 'a' if alpha-pixels < 20% */
struct MyButton UseAlphaMinMax;	/* v40: ignore smallest (=transparent) and biggest (=solid) alpha values*/
struct MyButton QuakePatch;		/* v41: Force blending if 32bits tex (if 32 bits then TexMode is 'A' or 'a' allways) */

struct MyButton DebugWazp3D;		/* global on/off for printing debug */
struct MyButton DisplayFPS;
struct MyButton DebugFunction;
struct MyButton DebugCalls;
struct MyButton DebugState;
struct MyButton DebugPoint;
struct MyButton DebugVar;
struct MyButton DebugVal;
struct MyButton DebugDoc;
struct MyButton DebugAsJSR;
struct MyButton DebugDriver;
struct MyButton DebugContext;
struct MyButton DebugTexture;
struct MyButton DebugError;
struct MyButton DebugWC;
struct MyButton DebugWT;
struct MyButton DebugClipper;
struct MyButton DebugSOFT3D;
struct MyButton DebugSC;
struct MyButton DebugST;
struct MyButton DebugTexNumber;	/* v40: print texture number in bitmap */
struct MyButton DebugSepiaImage;	/* Colorize Wazp3D's RGBA buffer */
struct MyButton DumpTextures;
struct MyButton DumpObject;
struct MyButton StepFunction;		/* Step a Warp3D function call */
struct MyButton StepSOFT3D;		/* Step a SOFT3D function call */
struct MyButton StepUpdate;		/* Step at update (Wazp3D's RGBA buffer-->screen) */
struct MyButton DebugMemList;
struct MyButton DebugMemUsage;
/* not in this release */
struct MyButton DoMipMaps;
} Wazp3D;
/*==================================================================================*/
#ifdef WAZP3DDEBUG
UBYTE font8x8[14*16*8] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,8,24,24,8,0,16,8,8,16,24,0,0,0,0,8,
        0,8,24,60,28,36,40,8,16,8,24,8,0,0,0,8,
        0,8,0,24,24,8,20,0,16,8,0,28,0,24,0,24,
        0,0,0,60,12,16,40,0,16,8,0,8,0,0,0,16,
        0,8,0,24,28,36,20,0,16,8,0,0,8,0,8,16,
        0,0,0,0,8,0,0,0,8,16,0,0,8,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        28,8,28,24,4,28,28,28,28,28,0,0,0,0,0,16,
        20,24,4,4,12,16,16,4,20,20,0,0,0,0,0,8,
        20,8,28,12,20,28,28,8,28,28,8,8,8,0,16,16,
        20,8,16,4,28,4,20,8,20,4,0,0,16,24,8,0,
        28,8,28,24,4,28,28,8,28,28,8,8,8,0,16,16,
        0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        24,24,56,24,56,28,28,28,36,8,4,36,16,34,36,24,
        36,24,36,36,36,16,16,32,36,8,4,40,16,54,52,36,
        90,36,56,32,36,24,24,44,60,8,4,48,16,42,44,36,
        94,60,36,36,36,16,16,36,36,8,20,40,16,34,36,36,
        32,36,56,24,56,28,16,28,36,8,24,36,28,34,36,24,
        24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        56,24,56,28,28,36,20,34,20,20,28,24,16,24,8,0,
        36,36,36,32,8,36,20,42,20,20,4,16,16,8,20,0,
        56,36,56,24,8,36,20,42,8,8,8,16,24,8,0,0,
        32,40,36,4,8,36,8,20,20,8,16,16,8,8,0,0,
        32,20,36,56,8,24,8,20,20,8,28,16,8,8,0,0,
        0,0,0,0,0,0,0,0,0,0,0,24,0,24,0,60,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        16,0,16,0,4,0,24,0,16,8,8,16,8,0,0,0,
        8,0,16,0,4,8,16,0,16,0,0,16,8,0,0,0,
        0,12,28,28,28,28,24,28,28,8,8,20,8,54,24,28,
        0,20,20,16,20,16,16,20,20,8,8,24,8,42,20,20,
        0,12,28,28,28,28,16,28,20,8,8,20,8,42,20,28,
        0,0,0,0,0,0,0,4,0,0,8,0,0,0,0,0,
        0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,8,8,16,12,0,
        0,0,0,0,16,0,0,0,0,0,0,8,8,16,24,0,
        24,12,24,24,24,20,20,36,20,20,28,16,8,8,0,0,
        20,20,16,16,16,20,20,36,8,20,8,16,8,8,0,0,
        24,12,16,48,8,12,8,60,20,8,28,8,8,16,0,0,
        16,4,0,0,0,0,0,0,0,16,0,8,0,16,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,
        8,0,0,12,0,0,8,8,8,20,28,0,62,0,8,0,
        8,0,0,8,0,0,28,28,20,40,32,0,40,0,8,0,
        8,0,0,28,0,0,8,8,0,26,24,8,44,0,8,0,
        8,0,0,8,0,0,8,8,0,20,4,16,40,0,8,0,
        8,0,8,8,40,84,8,28,0,42,56,8,62,0,8,0,
        0,0,8,16,40,0,8,8,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,8,8,24,24,0,0,0,12,122,40,0,0,0,8,20,
        0,0,0,0,0,0,0,0,24,46,0,0,0,0,8,20,
        0,0,0,0,0,24,0,0,0,42,24,16,28,0,8,8,
        0,0,0,0,0,24,28,62,0,0,16,8,42,0,8,8,
        0,0,0,0,0,0,0,0,0,0,48,16,28,0,8,8,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,4,20,20,8,12,24,24,8,0,0,0,24,60,
        0,8,4,8,8,28,8,24,0,44,8,0,0,0,52,0,
        0,0,12,28,20,8,8,20,0,52,0,8,0,0,44,0,
        0,8,16,8,0,28,8,12,0,24,0,16,28,8,24,0,
        0,8,12,12,0,8,8,12,0,0,0,8,4,0,0,0,
        0,8,8,0,0,0,8,24,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,8,0,8,0,0,0,0,0,0,0,0,0,0,0,
        8,0,8,8,16,0,24,0,0,8,8,0,36,36,36,8,
        0,24,0,8,0,40,24,8,0,8,8,16,40,40,40,0,
        0,24,0,0,0,40,8,0,0,0,0,8,20,20,20,8,
        0,24,0,0,0,52,8,0,8,0,0,16,36,36,36,16,
        0,0,0,0,0,32,8,0,24,0,0,0,0,0,0,8,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        8,8,8,8,8,8,14,24,28,28,28,28,8,16,40,0,
        20,20,20,20,20,20,24,36,16,16,16,16,0,0,0,16,
        20,20,20,20,20,20,46,32,28,28,28,28,8,16,16,16,
        28,28,28,28,28,28,56,36,16,16,16,16,8,16,16,16,
        20,20,20,20,20,20,46,24,28,28,28,28,8,16,16,16,
        0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,24,16,8,24,24,36,0,0,16,8,24,24,8,0,0,
        24,36,24,24,24,24,24,0,28,36,36,36,36,20,16,8,
        20,52,36,36,36,36,36,20,44,36,36,36,36,20,28,20,
        52,44,36,36,36,36,36,8,52,36,36,36,36,8,20,28,
        20,36,36,36,36,36,36,20,52,36,36,36,36,8,28,20,
        24,36,24,24,24,24,24,0,56,24,24,24,24,8,16,24,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        8,4,12,12,20,8,0,0,16,8,8,20,16,8,16,0,
        4,8,0,0,0,0,0,0,8,16,20,0,8,16,40,40,
        12,12,12,12,12,12,28,12,24,24,24,24,0,0,0,0,
        20,20,20,20,20,20,42,16,20,20,20,20,8,16,16,16,
        12,12,12,12,12,12,30,12,12,12,12,12,8,16,16,16,
        0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,24,16,4,8,12,20,0,0,16,4,8,20,4,16,20,
        16,0,8,8,20,0,0,0,0,8,8,20,0,8,16,0,
        8,24,28,28,28,28,28,8,28,20,20,20,20,20,24,20,
        24,20,20,20,20,20,20,28,28,20,20,20,20,8,20,8,
        24,20,28,28,28,28,28,8,28,12,12,12,12,8,24,8,
        0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,16,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif
/*==================================================================================*/
struct WAZP3D_context{
struct WAZP3D_texture *WT;		/* binded one */
float uresize,vresize;			/* for binded tex */
struct WAZP3D_texture *firstWT;	/* list of all textures */

BOOL CallFlushFrame;	/* then DoUpdate inside bitmap*/
BOOL CallSetDrawRegion;
BOOL CallClearZBuffer;
BOOL CallSetBlending;

struct point3D P[MAXPOINTS];

UBYTE hints[32];

struct RastPort rastport;
ULONG ModeID;

UBYTE *Image8;
UWORD large,high;
WORD Xmin,Ymin,Xmax,Ymax;					/* screen scissor */

ULONG primitive;
ULONG Pnb;
float PointSize;

ULONG AlphaMode,AlphaRef;
ULONG SrcFunc,DstFunc;
ULONG LogicOp;
ULONG PenMask;

UWORD FogMode;
float FogZmin,FogZmax,FogDensity;
UBYTE FogRGBA[4];

ULONG	StencilFunc,StencilRef,StencilMask,StencilDpfail,StencilDppass,StencilWriteMask;
ULONG ZbufferMode;

UBYTE CurrentRGBA[4];
UBYTE	MaskRGBA[4];

void  *bmHandle; 			/* to directly write to bitmap */
UWORD Fps;
UWORD TimePerFrame;
ULONG LastMilliTime;

UBYTE ZMode;
UBYTE ColMode;
UBYTE TexMode;
UBYTE UseTex;
UBYTE UseFog;
UBYTE UseGouraud;	

W3D_Context context;
void *SC;
};
/*==================================================================================*/
struct WAZP3D_texture{
UBYTE *pt;
UWORD large,high,bits;
UBYTE  EnvRGBA[4];
UBYTE WrapRGBA[4];
UBYTE ChromaTestMinRGBA[4];
UBYTE ChromaTestMaxRGBA[4];
ULONG ChromaTestMode;
ULONG MinFilter,MagFilter,TexEnvMode;
BOOL  MinFiltering,MagFiltering,MinMimapping;
BOOL  WrapRepeatX,WrapRepeatY;
void *nextWT;		/* next WAZP3D_texture */
void *ST;			/* link to this SOFT3D_texture */
W3D_Texture	texture;	/* include this Warp3D texture */
};
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
	LONG ddu;
	LONG ddv;
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
	UBYTE ddu1,ddu,ddu3,ddu4;
	UBYTE ddv1,ddv,ddv3,ddv4;
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
	UBYTE ddu4,ddu3,ddu,ddu1;
	UBYTE ddv4,ddv3,ddv,ddv1;
	}  W;
#endif
};
/*=============================================================*/
#ifdef MOTOROLAORDER
union oper3D {
struct oper3DW{
	ULONG Index;
	}  L;
struct oper3DB{
	UBYTE empty1,empty2,a,b;
	}  B;
};
#else
union oper3D {
struct oper3DW{
	ULONG Index;
	}  L;
struct oper3DB{
	UBYTE b,a,empty3,empty4;
	}  B;
};
#endif
/*=============================================================*/
struct pixbuffer3D{
	UBYTE *Image8;
	UBYTE *Tex8;
	UBYTE *Color8;		/* ptr on ColorRGBA */
	UBYTE *Buffer8;		/* ptr on BufferRGBA*/
	UBYTE *Fog8;		/* ptr on FogRGBA*/
	UBYTE  ColorRGBA[4];
	UBYTE BufferRGBA[4];
	UBYTE    FogRGBA[4];
};
/*=============================================================*/
struct rgbabuffer3D{		/* same thing as pixbuffer3D but access to only one of the RGBA values */
	UBYTE *RGBA;
	UBYTE *unused1;
	UBYTE *unused2;
	UBYTE *unused3;
	UBYTE *unused4;
	UBYTE  unused5[4];
	UBYTE  unused6[4];
	UBYTE  unused7[4];
};
/*=============================================================*/
typedef const void * (*HOOKEDFUNCTION)(void *SC);
/*=============================================================*/
struct SOFT3D_context{
	UBYTE FogA[ZMAX];
	UBYTE Mul8[256*256];
	UBYTE Add8[256*256];
	WORD PolyHigh;
	WORD large,high;
	struct vertex3D ClipMin;			/* 3D clipper */
	struct vertex3D ClipMax;
	ULONG *Image32;
	WORD  *Zbuffer16;
	UBYTE RGBA[4];
	UBYTE CurrentRGBA[4];
	UBYTE FlatRGBA[4];
	struct SOFT3D_texture *ST;
	WORD Pnb;
	struct point3D *P0;
	union pixel3D *Pix;
	union pixel3D *Xdelta;
	union pixel3D  Ydelta;
	WORD PointSize;
	WORD PointLarge[MAXSCREEN];
	union pixel3D side1[MAXSCREEN];
	union pixel3D side2[MAXSCREEN];
	union pixel3D Xdeltas[MAXSCREEN];
	HOOKEDFUNCTION FillFunction;
	HOOKEDFUNCTION PixelsFunctionIn;
	HOOKEDFUNCTION PixelsFunctionTex;
	HOOKEDFUNCTION PixelsFunctionCol;
	HOOKEDFUNCTION PixelsFunctionFog;
	HOOKEDFUNCTION PixelsFunctionOut;
	struct pixbuffer3D PixBuffer[PIXBUFFERSIZE];
	struct pixbuffer3D *PixBufferDone;
	struct pixbuffer3D *PixBufferMaxi;
	UWORD Culling;
	UWORD FogMode;
	float FogZmin,FogZmax,FogDensity;
	UBYTE FogRGBA[4];
	WORD PolyPnb;
	union pixel3D PolyPIX[MAXPOLY];
	struct point3D PolyP[MAXPOLY];
	struct point3D T1[MAXPOLY];
	struct point3D T2[MAXPOLY];
	void *firstST;
	UBYTE AliasedLines[4*MAXSCREEN*2];
	WORD Pxmin,Pxmax,Pymin,Pymax;				/* really updated region */
	WORD xUpdate,largeUpdate,yUpdate,highUpdate;	/* really updated region previous frame*/
	UWORD Tnb,DumpStage;
	ULONG DumpFnum,DumpPnum,DumpFnb,DumpPnb;		/* Faces & Points count. */
	struct  face3D *DumpF;
	struct point3D *DumpP;
	UBYTE ZMode;
	UBYTE ColMode;
	UBYTE TexMode;
	UBYTE UseTex;
	UBYTE UseFog;
	UBYTE UseGouraud;	
	UBYTE ColorChange;			/* V41: Soft3D can smartly desactivate the gouraud if all points got the same color */
	struct rgbabuffer3D *Src;
	struct rgbabuffer3D *Dst;
	struct rgbabuffer3D *End;
	UBYTE EnvRGBA[4];
	W3D_Context *context;					/*v39: hack to allow access to the context within SOFT3D */
};
/*=================================================================*/
struct SOFT3D_mipmap{
	UBYTE *Tex8V[256];	/* adresse of the texture at this line */
	LONG   Tex8U[256];
};
/*=================================================================*/
struct SOFT3D_texture{
	struct SOFT3D_mipmap MMs[10];
	UBYTE *pt;			/* original  data from 3Dprog */
	UBYTE *pt2;			/* converted data as RGB or RGBA */
	UBYTE *ptmm;		/* mipmaps as RGB or RGBA */
	UWORD large,high,bits;
	UBYTE MipMapped,TexMode;
	UBYTE name[40];
	UWORD Tnum;
	void *nextST;
};
/*==================================================================================*/
struct face3D{
	ULONG Pnum;				/*indice on first point3D */
	UWORD Pnb;				/*point3D count*/
	struct SOFT3D_texture *ST;	/*Face's texture */
};
/*==================================================================================*/
#define et &&
#define ou ||
#define AND &
#define OR |
#define FLOOP(nbre) for(f=0;f<nbre;f++)
#define PLOOP(nbre) for(p=0;p<nbre;p++)
#define MLOOP(nbre) for(m=0;m<nbre;m++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define XLOOP(nbre) for(x=0;x<nbre;x++)
#define YLOOP(nbre) for(y=0;y<nbre;y++)
#define SWAP(x,y) {temp=x;x=y;y=temp;}
#define COPYRGBA(x,y)	*((ULONG *)(x))=*((ULONG *)(y));
/* simpler W3D_GetState used internally */
#define StateON(s) ((context->state & s)!=0)
/*==================================================================================*/
#ifdef WAZP3DDEBUG
#define REM(message) Libprintf(#message"\n");
#define SREM(message) if(Wazp3D.DebugSOFT3D.ON) Libprintf(#message"\n");
#define SFUNCTION(tex) if(Wazp3D.StepSOFT3D.ON) LibAlert(#tex); if(Wazp3D.DebugSOFT3D.ON) Libprintf(#tex "\n");
#define  WINFO(var,val,doc) if(var == val) if(Wazp3D.DebugVal.ON){if(Wazp3D.DebugVar.ON) Libprintf(" " #var "="); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WTAG(val,doc) if(tag == val) if(Wazp3D.DebugVal.ON){Libprintf(" " #val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf(",%ld,\n",data);}
#define  SINFO(var,val) if(var == val) if(Wazp3D.DebugSOFT3D.ON){Libprintf(" " #val "\n");}
#define WINFOB(var,val,doc) if(Wazp3D.DebugVal.ON) if((var&val)!=0) {Libprintf(" " #val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   VAR(var) if(Wazp3D.DebugVal.ON)  {if(Wazp3D.DebugVar.ON) Libprintf(" " #var "="); Libprintf("%ld\n",var);}
#define  VARF(var) if(Wazp3D.DebugVal.ON)  {if(Wazp3D.DebugVar.ON) Libprintf(" " #var "="); pf(var);}
#define   VAL(var) {if(Wazp3D.DebugVal.ON) Libprintf(" [%ld]",var);}
#define ZZ LibAlert("ZZ stepping ...");
#define  ERROR(val,doc) if(error == val) if(Wazp3D.DebugVal.ON) {if(Wazp3D.DebugVar.ON) Libprintf(" Error="); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WRETURN(error) return(PrintError(error));
#define QUERY(q,doc,s) if(query==q) if(Wazp3D.DebugVal.ON) {sup=s;  Libprintf(" " #q); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
/*==================================================================================*/
#else
#define REM(message) ;
#define SREM(message)  ;
#define SFUNCTION(tex) ;
#define  WINFO(var,val,doc) ;
#define   WTAG(val,doc) ;
#define  SINFO(var,val);
#define WINFOB(var,val,doc) ;
#define   VAR(var) ;
#define  VARF(var) ;
#define   VAL(var) ;
#define ZZ ;
#define  ERROR(val,doc) ;
#define   WRETURN(error) return(error);
#define QUERY(q,doc,s) if(query==q) sup=s;
#endif
/*==================================================================================*/
#ifdef __AROS__
AROS_UFP3(BOOL, ScreenModeFilter,
AROS_UFHA(struct Hook *, h	  , A0),
AROS_UFHA(APTR		 , object , A2),
AROS_UFHA(APTR		 , message, A1));
#else
BOOL ScreenModeFilter(struct Hook* hook __asm("a0"), APTR object __asm("a2"),APTR message __asm("a1"));
#endif
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,ULONG *Image);
UBYTE *Libstrcat(char *s1,const char *s2);
UBYTE *Libstrcpy(char *s1,const char *s2);
ULONG Libstrlen(const char *string);
ULONG STACKReadPixelArray(APTR  destRect,UWORD  destX,UWORD  destY,UWORD  destMod,struct RastPort *  RastPort,UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat );
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
void W3D_Settings(void);
void *Libmalloc(ULONG size );
void *Libmemcpy(void *s1,void *s2,LONG n);
void *MYmalloc(ULONG size,char *name);
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,UWORD TexMode,BOOL MipMapped);
void ClipPoly(struct SOFT3D_context *SC);
void ConvertBitmap(ULONG format,UBYTE *pt1,UBYTE *pt2,UWORD high,UWORD large,ULONG offset1,ULONG offset2,UBYTE *palette);
void DoUpdate(W3D_Context *context);
void CreateMipmaps(struct SOFT3D_texture *ST);
void DrawLinePIX(struct SOFT3D_context *SC);
void DrawPointPIX(struct SOFT3D_context *SC);
void DrawPointSimplePIX(struct SOFT3D_context *SC,register union pixel3D *P);
void DrawPolyP(struct SOFT3D_context *SC);
void DrawPolyPIX(struct SOFT3D_context *SC);
void DrawTriP(struct SOFT3D_context *SC,register struct point3D *A,register struct point3D *B,register struct point3D *C);
void DumpMem(UBYTE *pt,LONG nb);
void Fill_Zoff_Tex(struct SOFT3D_context *SC);
void Fill_Zoff_Tex_Gouraud(struct SOFT3D_context *SC);
void Fill_Zoff_Gouraud(struct SOFT3D_context *SC);
void Fill_Zoff_Tex_Gouraud_Fog(struct SOFT3D_context *SC);
void Fill_Ztest_Tex(struct SOFT3D_context *SC);
void Fill_Ztest_Tex_Gouraud(struct SOFT3D_context *SC);
void Fill_Ztest_Gouraud(struct SOFT3D_context *SC);
void Fill_Ztest_Tex_Gouraud_Fog(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex_Gouraud(struct SOFT3D_context *SC);
void Fill_Zwrite_Gouraud(struct SOFT3D_context *SC);
void Fill_Zwrite_Tex_Gouraud_Fog(struct SOFT3D_context *SC);
void GetPoint(W3D_Context *context,ULONG i);
void GetVertex(struct WAZP3D_context *WC,W3D_Vertex *V);
void LibAlert(const char *t);
void Libfree(void *p);
void Libprintf(const char *string, ...);
void LibSettings(struct MyButton *ButtonList,WORD ButtonCount);
void MYfree(void *pt);
void Pixels24R (struct SOFT3D_context *SC);
void Pixels24m (struct SOFT3D_context *SC);
void Pixels32R (struct SOFT3D_context *SC);
void Pixels32m (struct SOFT3D_context *SC);
void Pixels32AA(struct SOFT3D_context *SC);
void Pixels32A (struct SOFT3D_context *SC);
void Pixels32a (struct SOFT3D_context *SC);
void PixelsARGB(struct SOFT3D_context *SC);
void PixelsBGRA(struct SOFT3D_context *SC);
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
void SOFT3D_SetDrawFunctions(struct SOFT3D_context *SC,struct SOFT3D_texture *ST,UBYTE TexMode,UBYTE ZMode,UBYTE UseFog,UBYTE ColMode,UBYTE UseGouraud);
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat );
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
void AntiAliasImage(void *image,UWORD large,UWORD high,UBYTE *AliasedLines);
void SmoothTexture(void *image,UWORD large,UWORD high,UBYTE *AliasedLines,UBYTE bits);
BOOL SetState(W3D_Context *context,ULONG state,BOOL set);
void SOFT3D_SetPointSize(struct SOFT3D_context *SC,UWORD PointSize);
void SOFT3D_SetZbuffer(struct SOFT3D_context *SC,WORD *Zbuffer16);
void Libsprintf(UBYTE *buffer,const char *string, ...);
void Libsavefile(UBYTE *filename,void *pt,ULONG size);
void Libloadfile(UBYTE *filename,void *pt,ULONG size);
ULONG LibMilliTimer(void);
void SOFT3D_DrawPixels(struct SOFT3D_context *SC);
BOOL   LockBM(W3D_Context *context);
void UnLockBM(W3D_Context *context);
void SetBitmap(W3D_Context *context, struct BitMap *bm);
void	 DrawText(W3D_Context *context,UBYTE *text,WORD x,WORD y);
void BitmapPattern(void *image,UWORD large,UWORD high,UWORD bits,UBYTE color);
void Pixels24M(struct SOFT3D_context *SC);
void Pixels32M(struct SOFT3D_context *SC);
void Pixels24D(struct SOFT3D_context *SC);
void Pixels32D(struct SOFT3D_context *SC);
void Pixels24B(struct SOFT3D_context *SC);
void Pixels32B(struct SOFT3D_context *SC);
void SOFT3D_SetEnvColor(struct SOFT3D_context *SC,UBYTE  *RGBA);
/*==========================================================================*/
void pf(float x)		/* emulate printf() from a float */
{
LONG high,low;

	high=(LONG)(x);
	x=(x-(float)high);
	if(x<0.0) x=-x;
	low =(LONG)(1000000.0*x);
	Libprintf("%ld.%ld ",high,low);

}
/*==================================================================================*/
#ifdef WAZP3DDEBUG
void PrintME(struct memory3D *ME)
{
UBYTE *wall;
	wall=ME->pt;wall=wall+ME->size;
	Libprintf("[ME %ld nextME %ld\t] pt=%ld \tsize=%ld \t<%s> \t[%ld]\n ",ME,ME->nextME,ME->pt,ME->size,ME->name,wall[0]);
}
#else
#define PrintME()
#endif
/*==================================================================================*/
LONG ListMemoryUsage()
{
LONG MemoryUsage=0;
#ifdef WAZP3DDEBUG
struct memory3D *ME=firstME;
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
#endif
	return(MemoryUsage);
}
/*==================================================================================*/
void *MYmalloc(ULONG size,char *name)
{
struct memory3D *ME;
UBYTE *pt;
UBYTE *wall;

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("Will call malloc() for %ld bytes for <%s>\n",size,name);
#endif
	pt=Libmalloc(size+4+sizeof(struct memory3D));
	if (pt==NULL)
		{REM(malloc fail !);}

	ME=(struct memory3D *)pt;
	ME->pt=&pt[sizeof(struct memory3D)];
	ME->size=size;
	Libstrcpy(ME->name,name);
	ME->nextME=firstME;
	firstME=ME;
	(wall=&ME->pt[ME->size])[0]=111;
	memset(ME->pt,0,ME->size);
#ifdef WAZP3DDEBUG
	ListMemoryUsage();
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("MYmalloc() OK give pt: %ld (up to %ld) for <%s> \n",ME->pt,&ME->pt[ME->size],ME->name);
#endif
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
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugMemUsage.ON)
		Libprintf("Will free() memory at %ld\n",pt);
#endif
	ME =(struct memory3D *)&Bpt[-sizeof(struct memory3D)];
	thisME->nextME=firstME;
	while(thisME!=NULL)
	{
	if(thisME->nextME==ME)
		{
		if(thisME->nextME==firstME)
			firstME=ME->nextME;
		else
			thisME->nextME=ME->nextME;
#ifdef WAZP3DDEBUG
		if(Wazp3D.DebugMemUsage.ON) Libprintf("MYfree() OK for pt: %ld was <%s>\n",ME->pt,ME->name);
		Libfree(ME);
		ListMemoryUsage();
#else
		Libfree(ME);
#endif
		return;
		}
	thisME=thisME->nextME;
	}
#ifdef WAZP3DDEBUG
		if(Wazp3D.DebugMemUsage.ON) Libprintf("MYfree() ME %ld not found ==> not allocated pt: %ld  !!!!\n",ME,pt);
#endif
}
/*==================================================================================*/
void DumpMem(UBYTE *pt,LONG nb)
{
#ifdef WAZP3DDEBUG
LONG n;
NLOOP(nb/4)
	{
	Libprintf("[%ld\t][%ld\t] %ld\t%ld\t%ld\t%ld\n",pt,4*n,pt[0],pt[1],pt[2],pt[3]);
	pt=&(pt[4]);
	}
#endif
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
	Wazp3D.CrashFunctionCall=0;		/*to start the step-by-step with this call */

	if(!Wazp3D.DebugWazp3D.ON) return;

	if(Wazp3D.FunctionCallsAll!= 0xFFFFFFFF)
		Wazp3D.FunctionCallsAll++;
	if(Wazp3D.FunctionCalls[n]!= 0xFFFFFFFF)
		Wazp3D.FunctionCalls[n]++;
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugCalls.ON)
		Libprintf("[%ld][%ld]",Wazp3D.FunctionCallsAll,Wazp3D.FunctionCalls[n]);
	if(Wazp3D.DebugFunction.ON)
		Libprintf("%s\n" ,Wazp3D.FunctionName[n]);
	if(Wazp3D.DebugAsJSR.ON)
		Libprintf("                    JSR            -$%ld(A6)\n" ,30+6*n);	/* should better display it as hexa */
#endif

	if(Wazp3D.FunctionCallsAll==Wazp3D.CrashFunctionCall)
		Wazp3D.StepFunction.ON=Wazp3D.StepSOFT3D.ON=TRUE;

	if(Wazp3D.StepFunction.ON)
		LibAlert(Wazp3D.FunctionName[n]);
}
/*=================================================================*/
#ifdef WAZP3DDEBUG
void PrintRGBA(UBYTE *RGBA)
{
	if (!Wazp3D.DebugVal.ON) return;
	Libprintf(" RGBA %ld %ld %ld %ld\n",RGBA[0],RGBA[1],RGBA[2],RGBA[3]);
}
/*=================================================================*/
void PrintP(struct point3D *P)
{
	if (!Wazp3D.DebugPoint.ON) return;
	Libprintf(" P XYZ, %ld, %ld, %ld, UV, %ld, %ld, ",(WORD)P->x,(WORD)P->y,(WORD)(1024.0*P->z),(WORD)(256.0*P->u),(WORD)(256.0*P->v));
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
	Libprintf("SOFT3D_texture(%ld) %ldX%ldX%ld  pt %ld pt2 %ld NextST(%ld) MipMapped %ld TexMode:%ld \n",ST,ST->large,ST->high,ST->bits,ST->pt,ST->pt2,ST->nextST,ST->MipMapped,ST->TexMode);
}
/*=================================================================*/
void PrintPIXfull(union pixel3D *PIX)
{
	if (!Wazp3D.DebugPoint.ON)  return;
	if (!Wazp3D.DebugSOFT3D.ON) return;
	Libprintf(" PIX XYZ %ld.%ld %ld.%ld %ld.%ld \tUV %ld.%ld %ld.%ld ",PIX->W.x,PIX->W.xlow,PIX->W.y,PIX->W.ylow,PIX->W.z,PIX->W.zlow,PIX->W.u,PIX->W.u3,PIX->W.v,PIX->W.v3);
	Libprintf("RGBA %ld.%ld %ld.%ld %ld.%ld %ld.%ld large %ld\n",PIX->W.R,PIX->W.R3,PIX->W.G,PIX->W.G3,PIX->W.B,PIX->W.B3,PIX->W.A,PIX->W.A3,PIX->W.large);
}
/*=================================================================*/
void PrintPIX(union pixel3D *PIX)
{
	if (!Wazp3D.DebugPoint.ON)  return;
	if (!Wazp3D.DebugSOFT3D.ON) return;
	Libprintf(" PIX XYZ %ld %ld %ld \tUV %ld %ld ",PIX->W.x,PIX->W.y,PIX->W.z,PIX->W.u,PIX->W.v);
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
#else
#define PrintRGBA(x) ;
#define PrintP(x) ;
#define PrintWT(x) ;
#define PrintST(x) ;
#define PrintPIXfull(x) ;
#define PrintPIX(x) ;
#define PrintAllT(x) ;
#endif

/*=============================================================*/
void SOFT3D_SetImage(struct SOFT3D_context *SC,WORD large,WORD high,ULONG *Image)
{
WORD y;

SFUNCTION(SOFT3D_SetImage)
	SC->large=large;
	SC->high =high;
	SC->Pxmin=0;
	SC->Pymin=0;
	SC->Pxmax=large-1;
	SC->Pymax=high -1;
	SC->Image32=Image;

	YLOOP(high)
		{
		SC->side1[y].L.Image32Y   =SC->side2[y].L.Image32Y  =(ULONG *) &SC->Image32  [y*large];
		SC->side1[y].L.y=0;SC->side1[y].W.y=y;
		}

}
/*=============================================================*/
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,ULONG *Image)
{
register union oper3D Oper;
struct SOFT3D_context *SC;
UWORD x,y;
ULONG n;
UBYTE FogRGBA[4];
float m=5.0;	/* set a margin for clipper so show if 3Dprog never setted scissor */

SFUNCTION(SOFT3D_Start)
	SC=MYmalloc(sizeof(struct SOFT3D_context),"SOFT3D_context");
	if(SC==NULL) return(NULL);
	SC->Tnb=0;			/* texture number */
	SC->DumpStage=0;

	if(Image==NULL)
		Image=MYmalloc(large*high*32/8,"SOFT3D_ImageBuffer auto");

	SOFT3D_SetImage(SC,large,high,Image);

	Oper.L.Index=0;
	XLOOP(256)
	YLOOP(256)
	{
	Oper.B.a=x;
	Oper.B.b=y;
	SC->Mul8[Oper.L.Index]=((x*y)/255);
	if((x+y)<256)
		SC->Add8[Oper.L.Index]=x+y;
	else
		SC->Add8[Oper.L.Index]=255;
	}

	SC->PixBufferDone=SC->PixBuffer;
	SC->PixBufferMaxi=SC->PixBuffer + (PIXBUFFERSIZE-large);
	NLOOP(PIXBUFFERSIZE)
	{
	SC->PixBuffer[n].Color8 =(UBYTE *)&SC->PixBuffer[n].ColorRGBA[0];
	SC->PixBuffer[n].Buffer8=(UBYTE *)&SC->PixBuffer[n].BufferRGBA[0];
	SC->PixBuffer[n].Fog8   =(UBYTE *)&SC->PixBuffer[n].FogRGBA[0];
	}

	SOFT3D_SetClipping(SC,m,(float)(large-1)-m,m,(float)(high-1)-m,0.000,0.999);
	FogRGBA[0]=FogRGBA[1]=FogRGBA[2]=FogRGBA[3]=255;	/* default white */
	SOFT3D_Fog(SC,0,0.0,0.999,0.1,FogRGBA);			/* default no fog */
	SOFT3D_SetDrawFunctions(SC,NULL,'0',1,FALSE,'R',FALSE);	/* default notex nofog nozbuffer just color */
	SOFT3D_SetPointSize(SC,1);					/* default simpler point = 1 pixel */

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
SREM(SOFT3D_SetBackground)
	PrintRGBA(RGBA);
	COPYRGBA(SC->RGBA,RGBA);
}
/*=============================================================*/
void SOFT3D_SetEnvColor(struct SOFT3D_context *SC,UBYTE  *RGBA)
{
SREM(SOFT3D_SetEnvColor)
	PrintRGBA(RGBA);
	COPYRGBA(SC->EnvRGBA,RGBA);
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

	if(RGBA32==0)
		{
		size=SC->large*high*32/8;
		memset(Image32,0,size);
		return;
		}

	size=SC->large*high/8;
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
	if(!Wazp3D.DirectBitmap.ON)
		MYfree(SC->Image32);
	MYfree(SC);
}
/*=============================================================*/
#define ADD8(a1,b1,dst)              Oper1.B.b=b1;  Oper1.B.a=a1;  dst=Add8[Oper1.L.Index];
#define MUL8(a1,b1,dst)              Oper1.B.b=b1;  Oper1.B.a=a1;  dst=Mul8[Oper1.L.Index];
#define NEXTADDMUL8(b1,b2,dst)       Oper1.B.b=b1;  Oper2.B.b=b2;  dst=Mul8[Oper1.L.Index]+Mul8[Oper2.L.Index];
#define FULLADDMUL8(b1,a1,b2,a2,dst) Oper1.B.a=a1;  Oper2.B.a=a2;  NEXTADDMUL8(b1,b2,dst)
#define AT Ct[3]
#define AF Cf[3]
/*=============================================================*/
/* since v34:new pixels-functions to do multi-pass blending	*/
/* see OpenGL doc Ct=texture Cf=color Cc=env*/
/*
GL_TEXTURE_ENV_MODE:
	GL_REPLACE 	C = Ct 			A = At
	GL_MODULATE	C = Ct * Cf 		A = At * Af
	GL_BLEND	C = Cf*(1-Ct) + Cc*Ct 	A = Af * At
	GL_DECAL	C = Cf*(1-At) + Ct*At 	A = Af
f = fragment, t = texture, c = GL_TEXTURE_ENV_COLOR
glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,param);
glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,param);
*/
/*=============================================================*/
void Pixels24R(struct SOFT3D_context *SC)
{
/* copy source to dest (allways)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;

SREM(Pixels24R)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	Cf[0]=Ct[0];
	Cf[1]=Ct[1];
	Cf[2]=Ct[2];
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels32R(struct SOFT3D_context *SC)
{
/* copy source to dest (allways)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;

SREM(Pixels32R)
Pixels:
	COPYRGBA(Dst->RGBA,Src->RGBA);
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels24M(struct SOFT3D_context *SC)
{
/* v40: modulate src & dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(Pixels24M)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	MUL8(Ct[0],Cf[0],Cf[0])
	MUL8(Ct[1],Cf[1],Cf[1])
	MUL8(Ct[2],Cf[2],Cf[2])
/*	AF=AF; */		/* alpha from color */
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels32M(struct SOFT3D_context *SC)
{
/* v40: modulate src & dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;


SREM(Pixels32M)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	MUL8(Ct[0],Cf[0],Cf[0])
	MUL8(Ct[1],Cf[1],Cf[1])
	MUL8(Ct[2],Cf[2],Cf[2])
	MUL8(AT,AF,AF)
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels24D(struct SOFT3D_context *SC)
{
/* v40: decal src & dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;

SREM(Pixels24DEC)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	Cf[0]=Ct[0];
	Cf[1]=Ct[1];
	Cf[2]=Ct[2];
/*	AF=AF; */		/* alpha from color */
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels32D(struct SOFT3D_context *SC)
{
/* v40: decal src & dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;

SREM(Pixels32D)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Ct[0],AT,Cf[0],255-AT,Cf[0])
	NEXTADDMUL8(Ct[1],Cf[1],Cf[1])
	NEXTADDMUL8(Ct[2],Cf[2],Cf[2])
/*	AF=AF; */		/* alpha from color */
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels24B(struct SOFT3D_context *SC)
{
/* v40: blend src & dst  & env (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;
register UBYTE *Cc=SC->EnvRGBA;

SREM(Pixels24B)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Cf[0],255-Ct[0],Cc[0],Ct[0],Cf[0])
	NEXTADDMUL8(Cf[1],Cc[1],Cf[1])
	NEXTADDMUL8(Cf[2],Cc[2],Cf[2])
/*	AF=AF; */		/* alpha from color */

	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels32B(struct SOFT3D_context *SC)
{
/* v40: blend src & dst  & env (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;
register UBYTE *Cc=SC->EnvRGBA;


SREM(Pixels32B)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Cf[0],255-Ct[0],Cc[0],Ct[0],Cf[0])
	NEXTADDMUL8(Cf[1],Cc[1],Cf[1])
	NEXTADDMUL8(Cf[2],Cc[2],Cf[2])
	MUL8(AF,AT,AF)
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels24m(struct SOFT3D_context *SC)
{
/* make median value with src & dst  (allways)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register ULONG  one=1;

SREM(Pixels24m)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	Cf[0]= ( (ULONG) Ct[0] + (ULONG)Cf[0] )>>one;
	Cf[1]= ( (ULONG) Ct[1] + (ULONG)Cf[1] )>>one;
	Cf[2]= ( (ULONG) Ct[2] + (ULONG)Cf[2] )>>one;
/*	AF=AF; */		/* alpha from color */
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels32m(struct SOFT3D_context *SC)
{
/* make median value with src & dst  (allways)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register ULONG one=1;

SREM(Pixels32m)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	Cf[0]= ( (ULONG) Ct[0] + (ULONG)Cf[0] )>>one;
	Cf[1]= ( (ULONG) Ct[1] + (ULONG)Cf[1] )>>one;
	Cf[2]= ( (ULONG) Ct[2] + (ULONG)Cf[2] )>>one;
	Cf[3]= ( (ULONG) AT    + (ULONG)AF    )>>one;
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels32AA(struct SOFT3D_context *SC)
{
/* blend source & dest (allways)*/
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;
register UBYTE *Mul8=SC->Mul8;
register UBYTE *Cf;
register UBYTE *Ct;
register union oper3D Oper1;
register union oper3D Oper2;
register struct rgbabuffer3D* End=SC->End;

SREM(Pixels32AA)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Ct[0],AT,Cf[0],255-AT,Cf[0])
	NEXTADDMUL8(Ct[1],Cf[1],Cf[1])
	NEXTADDMUL8(Ct[2],Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void Pixels32A(struct SOFT3D_context *SC)
{
/* blend source & dest (if source not solid nor transparent)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register UBYTE *Mul8=SC->Mul8;
register UBYTE *Cf;
register UBYTE *Ct;
register union oper3D Oper1;
register union oper3D Oper2;
register struct rgbabuffer3D* End=SC->End;


	if(!Wazp3D.UseAlphaMinMax.ON)
		{ Pixels32AA(SC); return; }

SREM(Pixels32A)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA;  Ct=Src->RGBA;
	if (AT > ALPHAMIN)				/* if source visible ? */
	{
		if(AT < ALPHAMAX)				/* if source not solid ? */
		{
		FULLADDMUL8(Ct[0],AT,Cf[0],255-AT,Cf[0])
		NEXTADDMUL8(Ct[1],Cf[1],Cf[1])
		NEXTADDMUL8(Ct[2],Cf[2],Cf[2])
		}
		else
		{
		COPYRGBA(Cf,Ct);
		}

	}
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels32a(struct SOFT3D_context *SC)
{
/* copy source to dest (if source not transparent)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;

SREM(Pixels32a)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	if (AT > ALPHAMIN)					/* if source visible ? */
		COPYRGBA(Cf,Ct);
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels24S(struct SOFT3D_context *SC)
{
/* v41: add saturate src & dst  (allways) */
/*
BlendingFunctions used in BlitzQuake:
glBlendFunc (GL_ZERO, GL_SRC_ALPHA);			---> Pixels24L (v41)
glBlendFunc (GL_ZERO, GL_SRC_COLOR));			---> Pixels24M (v41)
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	---> Pixels32A
glBlendFunc (GL_ONE, GL_ONE);					---> Pixels24S (v41)
*/
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Add8=SC->Add8;
register union oper3D Oper1;

SREM(Pixels24S)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	ADD8(Ct[0],Cf[0],Cf[0])
	ADD8(Ct[1],Cf[1],Cf[1])
	ADD8(Ct[2],Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void Pixels24L(struct SOFT3D_context *SC)
{
/* v41: alpha-modulate dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(Pixels24L)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	MUL8(AT,Cf[0],Cf[0])
	MUL8(AT,Cf[1],Cf[1])
	MUL8(AT,Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void PixelsBGRA(struct SOFT3D_context *SC)
{
/* Convert BGRA <-> RGBA */
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;

SREM(PixelsBGRA)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	Cf[0]= Ct[2];
	Cf[1]= Ct[1];
	Cf[2]= Ct[0];
	AF= AT;
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}

/*=============================================================*/
void Fill_Zwrite(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zwrite)
	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		PixBuffer->Image8=Image8;
		COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Ztest)
	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		PixBuffer->Image8=Image8;
		COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zoff)
	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	PixBuffer->Image8=Image8;
	COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
	PixBuffer++;
	Image8+=4;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex_Gouraud_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zwrite_Tex_Gouraud_Fog)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(PixBuffer->FogRGBA,SC->FogRGBA);
		PixBuffer->FogRGBA[3]=SC->FogA[Pix->W.z];
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex_Gouraud_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Ztest_Tex_Gouraud_Fog)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(PixBuffer->FogRGBA,SC->FogRGBA);
		PixBuffer->FogRGBA[3]=SC->FogA[Pix->W.z];
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff_Tex_Gouraud_Fog(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zoff_Tex_Gouraud_Fog)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	 while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	PixBuffer->Image8=Image8;
	PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
	PixBuffer->ColorRGBA[0]=Pix->W.R;
	PixBuffer->ColorRGBA[1]=Pix->W.G;
	PixBuffer->ColorRGBA[2]=Pix->W.B;
	PixBuffer->ColorRGBA[3]=Pix->W.A;
	COPYRGBA(PixBuffer->FogRGBA,SC->FogRGBA);
	PixBuffer->FogRGBA[3]=SC->FogA[Pix->W.z];
	PixBuffer++;
	Image8+=4;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zwrite_Tex_Gouraud)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Ztest_Tex_Gouraud)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff_Tex_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zoff_Tex_Gouraud)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	PixBuffer->Image8=Image8;
	PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
	PixBuffer->ColorRGBA[0]=Pix->W.R;
	PixBuffer->ColorRGBA[1]=Pix->W.G;
	PixBuffer->ColorRGBA[2]=Pix->W.B;
	PixBuffer->ColorRGBA[3]=Pix->W.A;
	PixBuffer++;
	Image8+=4;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zwrite_Gouraud)
	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		PixBuffer->Image8=Image8;
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Ztest_Gouraud)
	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		PixBuffer->Image8=Image8;
		PixBuffer->ColorRGBA[0]=Pix->W.R;
		PixBuffer->ColorRGBA[1]=Pix->W.G;
		PixBuffer->ColorRGBA[2]=Pix->W.B;
		PixBuffer->ColorRGBA[3]=Pix->W.A;
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff_Gouraud(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zoff_Gouraud)
	 while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	PixBuffer->Image8=Image8;
	PixBuffer->ColorRGBA[0]=Pix->W.R;
	PixBuffer->ColorRGBA[1]=Pix->W.G;
	PixBuffer->ColorRGBA[2]=Pix->W.B;
	PixBuffer->ColorRGBA[3]=Pix->W.A;
	PixBuffer++;
	Image8+=4;
	Pix->L.R+=Xdelta->L.R;
	Pix->L.G+=Xdelta->L.G;
	Pix->L.B+=Xdelta->L.B;
	Pix->L.A+=Xdelta->L.A;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zwrite_Tex(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zwrite_Tex)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		Zbuffer16[0]=Pix->W.z;
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Ztest_Tex(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register WORD  *Zbuffer16;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Ztest_Tex)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	Zbuffer16=(WORD  *)(& Pix->L.Zbuffer16Y[Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	if(Zbuffer16[0] > Pix->W.z)
		{
		PixBuffer->Image8=Image8;
		PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
		COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
		PixBuffer++;
		}
	Image8+=4;
	Zbuffer16++;
	Pix->L.z+=Xdelta->L.z;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Zoff_Tex(struct SOFT3D_context *SC)
{
register UBYTE *Image8;
register union pixel3D *Pix=SC->Pix;
register union pixel3D *Xdelta=SC->Xdelta;
register struct SOFT3D_mipmap *MM;
register struct pixbuffer3D *PixBuffer=SC->PixBufferDone;

register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Zoff_Tex)
	MM=&SC->ST->MMs[0];		/* TODO: manage mipmaps level*/

	while(0<high--)
	{
	Image8   =(UBYTE *)(& Pix->L.Image32Y  [Pix->W.x]);
	large	   =Pix->W.large;

	while(0<large--)
	{
	PixBuffer->Image8=Image8;
	PixBuffer->Tex8=MM->Tex8U[Pix->W.u]+MM->Tex8V[Pix->W.v];
	COPYRGBA(PixBuffer->ColorRGBA,SC->FlatRGBA);
	PixBuffer++;
	Image8+=4;
	Pix->L.u+=Xdelta->L.u;
	Pix->L.v+=Xdelta->L.v;		Xdelta->L.u+=Xdelta->L.ddu;	Xdelta->L.v+=Xdelta->L.ddv;
	}
	if(PixBuffer > SC->PixBufferMaxi)
		{SC->PixBufferDone=PixBuffer;  SOFT3D_DrawPixels(SC); PixBuffer=SC->PixBuffer;}
	Pix++; Xdelta++;
	}
SC->PixBufferDone=PixBuffer;
}
/*=============================================================*/
void Fill_Nothing(struct SOFT3D_context *SC)
{
SREM(Fill_Nothing)
	return;
}
/*=============================================================*/
void SOFT3D_SetDrawFunctions(struct SOFT3D_context *SC,struct SOFT3D_texture *ST,UBYTE TexMode,UBYTE ZMode,UBYTE UseFog,UBYTE ColMode,UBYTE UseGouraud)
{
UBYTE FillMode;
BOOL tex24;

	if(Wazp3D.HackRGBA1.ON)
		{ColMode=TexMode;		UseFog=FALSE;	TexMode='0'; UseGouraud=TRUE;}
	if(Wazp3D.HackRGBA2.ON)
		{ColMode='R';		UseFog=FALSE;	TexMode='0'; UseGouraud=FALSE;}

	if(ST!=NULL)
		PrintST(ST);

	if(ST==NULL)
		{TexMode='0';}

	if(ST!=NULL)	
	if(TexMode!='0')			/* if textured */
	if(TexMode!='R')			/* if blended now*/
		TexMode=ST->TexMode;	/* then use current tex blendmode (A/a) */

	if(ST!=NULL)	
	if(TexMode!='0')			/* if textured */
	if(Wazp3D.QuakePatch.ON)
		TexMode=ST->TexMode;	/* then use current tex blendmode (A/a) */


/* if nothing change ==> return */
	if(SC->ST	  	==ST)
	if(SC->ZMode  	==ZMode)
	if(SC->ColMode	==ColMode)
	if(SC->TexMode	==TexMode)
	if(SC->UseFog 	==UseFog)
	if(SC->UseGouraud	==UseGouraud)
		return;

SREM(SOFT3D_SetDrawFunctions)
SREM(==> flush remaining pixels)
	SOFT3D_DrawPixels(SC);		/* flush remaining pixels*/

/* else update */
	SC->ST		=ST;
	SC->ZMode		=ZMode;
	SC->ColMode		=ColMode;
	SC->TexMode		=TexMode;
	SC->UseFog		=UseFog;
	SC->UseGouraud	=UseGouraud;
	SC->UseTex		=0;
	if(TexMode!='0')
		SC->UseTex=1;

	if(UseGouraud==TRUE)
	if(!SC->ColorChange)		/* = dont truly need gouraud shading */
		{
		UseGouraud=0;
		COPYRGBA(SC->FlatRGBA,SC->PolyP->RGBA); /* flat color from current points */
		}

	FillMode=SC->ZMode*4+SC->UseTex*2+SC->UseGouraud*1;
	if(UseFog)	FillMode=16+SC->ZMode;

VAR(SC->ZMode)
VAR(SC->ColMode)
VAR(SC->TexMode)
VAR(SC->UseTex)
VAR(SC->ColorChange)
VAR(SC->UseGouraud)
VAR(SC->UseFog)
VAR(FillMode)

	SC->FillFunction=(HOOKEDFUNCTION)Fill_Nothing;
	SC->PixelsFunctionTex= NULL;
	SC->PixelsFunctionCol= NULL;
	SC->PixelsFunctionFog= NULL;

	if(FillMode==0 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff;
	if(FillMode==1 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Gouraud;
	if(FillMode==2 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex;
	if(FillMode==3 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex_Gouraud;

	if(FillMode==4 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Ztest;
	if(FillMode==5 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Ztest_Gouraud;
	if(FillMode==6 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Ztest_Tex;
	if(FillMode==7 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Ztest_Tex_Gouraud;

	if(FillMode==8 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff;
	if(FillMode==9 )	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Gouraud;
	if(FillMode==10)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex;
	if(FillMode==11)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex_Gouraud;

	if(FillMode==12)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zwrite;
	if(FillMode==13)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zwrite_Gouraud;
	if(FillMode==14)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zwrite_Tex;
	if(FillMode==15)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zwrite_Tex_Gouraud;

	if(FillMode==16)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex_Gouraud_Fog;
	if(FillMode==17)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Ztest_Tex_Gouraud_Fog;
	if(FillMode==18)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zoff_Tex_Gouraud_Fog;
	if(FillMode==19)	SC->FillFunction=(HOOKEDFUNCTION)Fill_Zwrite_Tex_Gouraud_Fog;


	tex24=FALSE;
	if(SC->ST!=NULL)
	tex24=(SC->ST->bits==24);

/* FunctionTex */
	if(TexMode=='R')
	{
	if(tex24)
		SC->PixelsFunctionTex=(HOOKEDFUNCTION)Pixels24R;
	else
		SC->PixelsFunctionTex=(HOOKEDFUNCTION)Pixels32R;
	}
	if(TexMode=='A')	SC->PixelsFunctionTex=(HOOKEDFUNCTION)Pixels32A;
	if(TexMode=='a')	SC->PixelsFunctionTex=(HOOKEDFUNCTION)Pixels32a;

/* FunctionCol */
	if(ColMode=='R')	SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32R;
	if(ColMode=='A')	SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32A;

	if(ColMode=='S')	SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24S;
	if(ColMode=='L')	SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24L;

/* dont do a stupid flat modulate with white = do nothing */
	if(!SC->ColorChange)		
	if(SC->FlatRGBA[0]==255)
	if(SC->FlatRGBA[1]==255)
	if(SC->FlatRGBA[2]==255)
	if(SC->FlatRGBA[3]==255)
		{
		SREM(No white shading)
		ColMode='0';
		}

	if(ColMode=='m')
	{
	if(tex24)
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24m;
	else
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32m;
	}

	if(ColMode=='M')
	{
	if(tex24)
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24M;
	else
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32M;
	}

	if(ColMode=='D')
	{
	if(tex24)
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24D;
	else
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32D;
	}

	if(ColMode=='B')
	{
	if(tex24)
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels24B;
	else
			SC->PixelsFunctionCol=(HOOKEDFUNCTION)Pixels32B;
	}

/* FunctionFog */
	if(UseFog)
		SC->PixelsFunctionFog=(HOOKEDFUNCTION)Pixels32A;

	if(Wazp3D.DirectBitmap.ON)
		{
		SC->PixelsFunctionIn =(HOOKEDFUNCTION)PixelsBGRA;
		SC->PixelsFunctionOut=(HOOKEDFUNCTION)PixelsBGRA;
		}
}
/*=============================================================*/
void SOFT3D_DrawPixels(struct SOFT3D_context *SC)
{
register struct pixbuffer3D *PixBuffer=SC->PixBuffer;
register ULONG PixBufferSize;
struct rgbabuffer3D *CurrentImage8=(struct rgbabuffer3D *)&PixBuffer->Image8;

SREM(SOFT3D_DrawPixels)
	if(SC->PixBuffer == SC->PixBufferDone) {SREM(Nothing to Draw);return;}
	PixBufferSize=SC->PixBufferDone - SC->PixBuffer;
	VAR(PixBufferSize)

	


/* default: read & write to Image8 (=Wazp3D's RGBAbuffer ) */
	CurrentImage8=(struct rgbabuffer3D *)&PixBuffer->Image8;

/* Pass 1: Read/convert pixel from bitmap */
	if(SC->PixelsFunctionIn!=NULL)
	{
	if(!LockBM(SC->context))		/* TODO: dont use the context within SOFT3D */
		goto DrawPixelsDone;	/* if cant lock ==> panic ==> exit */
SREM(PixelsFunctionIn)
	SC->Src=(struct rgbabuffer3D *)&PixBuffer->Image8;
	SC->Dst=(struct rgbabuffer3D *)&PixBuffer->Buffer8;
	SC->End=SC->Dst+PixBufferSize;
	SC->PixelsFunctionIn(SC);	/* convert readed pixels to RGBA format. TODO: manage ARGB,RGBA too */
	CurrentImage8=SC->Dst;		/* now read & write to buffer for bitmap pixel  */
	UnLockBM(SC->context);		/* TODO: dont use the context within SOFT3D */
	}

/* Pass 2: apply color (=light) */
	if(SC->PixelsFunctionCol!=NULL)
	{
SREM(PixelsFunctionCol)
	SC->Src=(struct rgbabuffer3D *)&PixBuffer->Tex8;		/* if tex do tex+color -> color */
	SC->Dst=(struct rgbabuffer3D *)&PixBuffer->Color8;
	if(SC->PixelsFunctionTex==NULL)					/* else just do color -> image */
		{
		SC->Src=(struct rgbabuffer3D *)&PixBuffer->Color8;
		SC->Dst=CurrentImage8;
		}
	SC->End=SC->Dst+PixBufferSize;
	SC->PixelsFunctionCol(SC);
	}

/* Pass 3: apply texture */
	if(SC->PixelsFunctionTex!=NULL)
	{
SREM(PixelsFunctionTex)
	SC->Src=(struct rgbabuffer3D *)&PixBuffer->Tex8;	/* default use tex only */
	if(SC->PixelsFunctionCol!=NULL)				/* if colorization use color(=color+tex) */
		SC->Src=(struct rgbabuffer3D *)&PixBuffer->Color8;
	SC->Dst=CurrentImage8;
	SC->End=SC->Dst+PixBufferSize;
	SC->PixelsFunctionTex(SC);
	}

/* Pass 4: apply fog */
	if(SC->PixelsFunctionFog!=NULL)
	{
SREM(PixelsFunctionFog)
	SC->Src=(struct rgbabuffer3D *)&PixBuffer->Fog8;
	SC->Dst=CurrentImage8;
	SC->End=SC->Dst+PixBufferSize;
	SC->PixelsFunctionFog(SC);
	}

/* Pass 5: Write/convert pixel to bitmap */
	if(SC->PixelsFunctionOut!=NULL)
	if(LockBM(SC->context))
	{
SREM(PixelsFunctionOut)
	SC->Src=CurrentImage8;
	SC->Dst=(struct rgbabuffer3D *)&PixBuffer->Image8;
	SC->End=SC->Dst+PixBufferSize;
	SC->PixelsFunctionOut(SC);
	UnLockBM(SC->context);
	}

DrawPixelsDone:
	SC->PixBufferDone=SC->PixBuffer;
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
	SOFT3D_SetDrawFunctions(SC,NULL,'0',SC->ZMode,FALSE,'0',FALSE); /* no tex & no color=flat */

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
	SC->Pix	=&(SC->side1[P->W.y]);
	SC->PolyHigh=SC->PointSize;

	YLOOP(SC->PolyHigh)
		{
		Libmemcpy(&SC->Pix[y],P,9*4);
		SC->Pix[y].W.large =PointLarge[y];
		SC->Pix[y].W.x	 =SC->Pix[y].W.x - PointLarge[y]/2;
		}

	SC->FillFunction(SC);
}
/*=============================================================*/
void DoDeltas(struct SOFT3D_context *SC,union pixel3D *P0,union pixel3D *P2,union pixel3D *Delta,WORD size)
{
/* V42: Compute a quadratic approximation of texture to emulate perspective */
float a,b,z0,rz,z2,n,td;
float u0,u1,u2,du,ddu;
float v0,v1,v2,dv,ddv;

	if(size==0)	return;
	Delta->L.z=(P2->L.z - P0->L.z)/size;

/*	if(SC->UseGouraud) */
	{
	Delta->L.R=(P2->L.R - P0->L.R)/size;
	Delta->L.G=(P2->L.G - P0->L.G)/size;
	Delta->L.B=(P2->L.B - P0->L.B)/size;
	Delta->L.A=(P2->L.A - P0->L.A)/size;
	}

	if(!SC->UseTex) return; 

	if( (!Wazp3D.TruePerspective.ON) ou (P0->L.z==P2->L.z) )
	{
	Delta->L.u=(P2->L.u - P0->L.u)/size;
	Delta->L.v=(P2->L.v - P0->L.v)/size;
	Delta->L.ddu=0;
	Delta->L.ddv=0;
	return;
	}

	n=size;
	z0=P0->L.z;	
	u0=P0->L.u;
	v0=P0->L.v;
	z2=P2->L.z;
	u2=P2->L.u;
	v2=P2->L.v;

	rz=(z2-z0)/(z0+z2);  
	td=u2-u0; 

	a=td*rz; 
	b=a/n; 
	du=td/n-b; 
	ddu=2*b/(n-1); 

	td=v2-v0; 

	a=td*rz; 
	b=a/n; 
	dv=td/n-b; 
	ddv=2*b/(n-1); 

	Delta->L.u		= du  ;
	Delta->L.v		= dv  ;
	Delta->L.ddu	= ddu ;
	Delta->L.ddv	= ddv ;

}
/*=============================================================*/
void DoLine(struct SOFT3D_context *SC,union pixel3D *P,union pixel3D *P2,WORD high)
{
WORD n;
union pixel3D Ydelta;

	if (high < 2) return;

	Ydelta.L.x=(P2->L.x - P->L.x)/high;
	DoDeltas(SC,P,P2,&Ydelta,high);
	high--;						/* ne pas recalculer les points extremites */
	NLOOP(high)
	{
		P[1].L.x=P[0].L.x+Ydelta.L.x;
		P[1].L.z=P[0].L.z+Ydelta.L.z;
		P[1].L.u=P[0].L.u+Ydelta.L.u;
		P[1].L.v=P[0].L.v+Ydelta.L.v;
		P[1].L.R=P[0].L.R+Ydelta.L.R;
		P[1].L.G=P[0].L.G+Ydelta.L.G;
		P[1].L.B=P[0].L.B+Ydelta.L.B;
		P[1].L.A=P[0].L.A+Ydelta.L.A;
		Ydelta.L.u=Ydelta.L.u+Ydelta.L.ddu;
		Ydelta.L.v=Ydelta.L.v+Ydelta.L.ddv;
		P++;
	}

}
/*=============================================================*/
void DrawSegmentPIX(struct SOFT3D_context *SC)
{
/* draw a simple horizontal line */
register union pixel3D *Pmin=&SC->PolyPIX[0];
register union pixel3D *Pmax=&SC->PolyPIX[1];
register union pixel3D *side=SC->side1;
register union pixel3D *temp;
register WORD LineLarge;

	SFUNCTION(DrawSegmentPIX)
/* Trouver point Pmin et point Pmax des deux points de cette ligne */
	if (Pmin->W.x > Pmax->W.x)
		SWAP(Pmin,Pmax);

	if ( (Pmin->W.x<0) ou (Pmin->W.y<0) ou (SC->large<=Pmin->W.x) ou (SC->high<=Pmin->W.y) ou (Pmax->W.x<0) ou (Pmax->W.y<0) ou (SC->large<=Pmax->W.x) ou (SC->high<=Pmax->W.y))
		return;

	Libmemcpy(&(side[Pmin->W.y]),Pmin,9*4);
	SC->Pix=&(side[Pmin->W.y]);
	LineLarge= Pmax->W.x - Pmin->W.x ;
	side[Pmin->W.y].W.large= LineLarge +1;
	SC->PolyHigh=1;

	SC->Xdelta=&(SC->Xdeltas[Pmin->W.y]);
	DoDeltas(SC,Pmin,Pmax,SC->Xdelta,LineLarge);

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

	LineLarge= Pmax->W.x - Pmin->W.x ;
	LineHigh = Pmax->W.y - Pmin->W.y ;
	if(LineLarge<0) LineLarge=-LineLarge;
	SC->PolyHigh = LineHigh+1;

	VAR(LineLarge)
	VAR(LineHigh)

	if(LineLarge+LineHigh==0)
		{DrawPointSimplePIX(SC,Pmin);return;}
	if(LineLarge+LineHigh<=2)
		{DrawPointSimplePIX(SC,Pmin);DrawPointSimplePIX(SC,Pmax);return;}
	if(LineHigh==0)
		{DrawSegmentPIX(SC);return;}

	PrintPIX(Pmin);
	PrintPIX(Pmax);
/* Stocker les deux points dans le side */
	Libmemcpy(&(side[Pmin->W.y]),Pmin,9*4);
	Pmin=&(side[Pmin->W.y]);
	Libmemcpy(&(side[Pmax->W.y]),Pmax,9*4);
	Pmax=&(side[Pmax->W.y]);
	SC->Pix=Pmin;
	DoLine(SC,Pmin,Pmax,LineHigh);

	side=Pmin;
	Pmin->W.large=Pmax->W.large=1;
	if(Pmin->W.x < Pmax->W.x)
	NLOOP(LineHigh)
	{
		side[0].W.large= 1 + side[1].W.x - side[0].W.x ;
		side++;
	}
	else
	NLOOP(LineHigh)
	{
		side[1].W.large= 1 + side[0].W.x - side[1].W.x ;
		side++;
	}

	if(LineLarge==0)
		{SC->FillFunction(SC);return;}	/* so dont need the Xdeltas */

	SC->Xdelta=&(SC->Xdeltas[Pmin->W.y]);
	DoDeltas(SC,Pmin,Pmax,SC->Xdelta,LineLarge);

	NLOOP(SC->PolyHigh)
	{
	SC->Xdelta[n].L.z		=SC->Xdelta[0].L.z;
	SC->Xdelta[n].L.u		=SC->Xdelta[0].L.u;
	SC->Xdelta[n].L.v		=SC->Xdelta[0].L.v;
	SC->Xdelta[n].L.R		=SC->Xdelta[0].L.R;
	SC->Xdelta[n].L.G		=SC->Xdelta[0].L.G;
	SC->Xdelta[n].L.B		=SC->Xdelta[0].L.B;
	SC->Xdelta[n].L.A		=SC->Xdelta[0].L.A;
	SC->Xdelta[n].L.ddu	=SC->Xdelta[0].L.ddu;
	SC->Xdelta[n].L.ddv	=SC->Xdelta[0].L.ddv;
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
	Pmax =&(PIX[n+1]);
	if (n+1==Pnb)
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
	DoLine(SC,Pmin,Pmax,size);

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
	SC->Xdelta=&(SC->Xdeltas[PolyYmin]);
	SC->PolyHigh=PolyYmax-PolyYmin; 
	if(SC->TexMode=='R')	SC->PolyHigh++;	/* v41: will awoid to redraw same edges for transp tristrips */


	if ((SC->PolyHigh  < 2   ) ou (SC->PolyHigh  > MAXSCREEN))
		return;

	NLOOP(SC->PolyHigh)
		{
		size= 1 + side2->W.x - side1->W.x ;
		side1->W.large = size - 1; 	/* v41: will awoid to redraw same edges for transp tristrips */
		if(SC->TexMode=='R')	side1->W.large++;
	
		if(PolyLarge < size)
			{Pmin=side1; Pmax=side2; PolyLarge=size;}

		if(!Wazp3D.NoPerspective.ON)
			DoDeltas(SC,side1,side2,&SC->Xdelta[n],size);

		side1++;side2++;
		}

	if(Wazp3D.NoPerspective.ON)
	{
	DoDeltas(SC,Pmin,Pmax,&SC->Xdelta[0],PolyLarge);
	NLOOP(SC->PolyHigh)
	{
	SC->Xdelta[n].L.z=SC->Xdelta[0].L.z;
	SC->Xdelta[n].L.u=SC->Xdelta[0].L.u;
	SC->Xdelta[n].L.v=SC->Xdelta[0].L.v;
	SC->Xdelta[n].L.R=SC->Xdelta[0].L.R;
	SC->Xdelta[n].L.G=SC->Xdelta[0].L.G;
	SC->Xdelta[n].L.B=SC->Xdelta[0].L.B;
	SC->Xdelta[n].L.A=SC->Xdelta[0].L.A;
	SC->Xdelta[n].L.ddu=SC->Xdelta[0].L.ddu;
	SC->Xdelta[n].L.ddv=SC->Xdelta[0].L.ddv;
	}
	}
	SFUNCTION(DrawPolyPIX-OK!)

	if(PolyLarge > 1   )
	if(PolyLarge < MAXSCREEN)
	SC->FillFunction(SC);

}
/*==================================================================================*/
void SOFT3D_Fog(struct SOFT3D_context *SC,WORD FogMode,float FogZmin,float FogZmax,float FogDensity,UBYTE *FogRGBA)
{
register float d,z,f;
register UWORD n;

#define CLAMP(X,MIN,MAX) ((X)<(MIN)?(MIN):((X)>(MAX)?(MAX):(X)))

SFUNCTION(SOFT3D_Fog)
	if(Wazp3D.DebugSOFT3D.ON)
	{
	SREM(SOFT3D_Fog)
	VAR(FogMode)
	VARF(FogZmin)
	VARF(FogZmax)
	VARF(FogDensity)
	}

	switch(FogMode)
	{
	case 0:  SREM(NO_FOG)	break;
	case 1:  SREM(GL_LINEAR)	break;
	case 2:  SREM(GL_EXP)	break;
	case 3:  SREM(GL_EXP2)	break;
	case 4:  SREM(FOG_INTERPOLATED)	break;
	default: SREM(BadFogMode) return;
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

	NLOOP(ZMAX)
	{
	z=(((float)n)/((float)ZMAX));
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
	default:
		d = -(FogDensity*FogDensity);
		f= exp( d * z*z );
		break;
	}

	f = CLAMP( f, 0.0F, 1.0F );
	SC->FogA[n]= (UBYTE) ( 255.0 * (1.0 - f ));			/* A transparency = 1 - fog value */
	if(Wazp3D.DebugSOFT3D.ON) VAR(SC->FogA[n])
	}

}
/*================================================================*/
void DumpPoly(struct SOFT3D_context *SC)
{
#ifdef WAZP3DDEBUG
struct face3D *F;


SFUNCTION(DumpPoly)
	if(SC->DumpStage==1)		/* 1: count faces & points */
	{
	SC->DumpFnb+=1;
	SC->DumpPnb+=SC->PolyPnb;
	return;
	}

	if(SC->DumpStage==2)		/* 2: dump faces & points */
	{
	if(SC->DumpFnum+1 >= SC->DumpFnb)
		{SC->DumpStage=3; Libprintf("Dump Fail too much faces\n"); return;}

	if(SC->DumpPnum+SC->PolyPnb >= SC->DumpPnb)
		{SC->DumpStage=3; Libprintf("Dump Fail too much points\n"); return;}

	F=&SC->DumpF[SC->DumpFnum];
	F->Pnum=SC->DumpPnum;
	F->Pnb =SC->PolyPnb;
	F->ST  =SC->ST;
	Libmemcpy(&(SC->DumpP[SC->DumpPnum]),&(SC->PolyP[0]),24*SC->PolyPnb);
	SC->DumpFnum+=1;
	SC->DumpPnum+=SC->PolyPnb;
	}
#endif
}
/*================================================================*/
void DrawPolyP(struct SOFT3D_context *SC)
{
union   pixel3D *PIX=SC->PolyPIX;
struct point3D *P=SC->PolyP;
float PolyZmin,PolyZmax;
WORD Pnb,n;
BOOL UseFog;
BOOL BackFace=FALSE;
#define NOTSAMERGBA(x,y)	(*((ULONG *)(x))!=*((ULONG *)(y)))


#ifdef WAZP3DDEBUG
if(Wazp3D.DebugSOFT3D.ON) Libprintf("DrawPolyP Pnb %ld  \n",SC->PolyPnb);
#endif

	if(SC->PolyPnb > MAXPOLY )	{SREM(Poly has too much Points!); return;}
	P=SC->PolyP;

	if(Wazp3D.UseClipper.ON)
	if(SC->PolyPnb > 1)
		ClipPoly(SC);

	Pnb=SC->PolyPnb;
	if(Pnb>=3)
		BackFace=! (0.0 > (P[1].x - P[0].x)*(P[2].y - P[0].y)-(P[2].x - P[0].x)*(P[1].y - P[0].y));
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("Backface %ld Culling %ld\n",BackFace,SC->Culling);
#endif

	if(Wazp3D.UseCullingHack.ON)
	if(BackFace==TRUE)
		return;

	if(SC->Culling==1)
	if(BackFace==TRUE)
		return;

	if(SC->Culling==2)
	if(BackFace==FALSE)
		return;

	memset(PIX,0,Pnb*sizeof(union pixel3D));

	if(Wazp3D.HackRGBA2.ON)		/* use face center as color */
	if(Pnb>=3)
		UVtoRGBA(SC->ST,(P[0].u+P[1].u+P[2].u)/3.0,(P[0].v+P[1].v+P[2].v)/3.0,SC->FlatRGBA);


	PolyZmin=1.0; PolyZmax=0.0;
	SC->ColorChange=FALSE;
	NLOOP(Pnb)
	{
	if (P->z < PolyZmin)	PolyZmin=P->z;
	if (PolyZmax < P->z)	PolyZmax=P->z;

	if(Wazp3D.HackRGBA1.ON)
		UVtoRGBA(SC->ST,P->u,P->v,P->RGBA);

	PrintP(P);
	PIX->W.x	= P->x;
	PIX->W.y	= P->y;
	PIX->W.z	= P->z *  CONVERTZ;
	PIX->L.u	= P->u *  CONVERTUV;
	PIX->L.v	= P->v *  CONVERTUV;
	PIX->W.R	= P->RGBA[0];
	PIX->W.G	= P->RGBA[1];
	PIX->W.B	= P->RGBA[2];
	PIX->W.A	= P->RGBA[3];
	PrintPIX(PIX);

/* new update-area */
	if(PIX->W.x < SC->Pxmin) SC->Pxmin=PIX->W.x;
	if(SC->Pxmax < PIX->W.x) SC->Pxmax=PIX->W.x;
	if(PIX->W.y < SC->Pymin) SC->Pymin=PIX->W.y;
	if(SC->Pymax < PIX->W.y) SC->Pymax=PIX->W.y;

	if(NOTSAMERGBA(SC->PolyP->RGBA,P->RGBA))	/* if color truly change ==> do gouraud */ 
		{SREM(ColorChange);SC->ColorChange=TRUE;}

	PIX++;
	P++;
	}

	if(Wazp3D.HackRGBA2.ON)
		SC->ColorChange=FALSE;


/* This face is inside Fog ? */
	UseFog=TRUE;
	if(SC->FogMode==0)
		UseFog=FALSE;
	if(SC->FogMode==1)
	{
	if(PolyZmax	   < SC->FogZmin) UseFog=FALSE;
	if(SC->FogZmax <	PolyZmin  ) UseFog=FALSE;	  /* end of fogging area */
	}

	if(!Wazp3D.UseFog.ON) UseFog=FALSE;

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("FogMode %ld FogZmin %ld FogZmax %ld  PolyZmin %ld PolyZmax %ld --> UseFog %ld \n",SC->FogMode,(WORD)(1024.0*SC->FogZmin),(WORD)(1024.0*SC->FogZmax),(WORD)(1024.0*PolyZmin),(WORD)(1024.0*PolyZmax),UseFog);
#endif

	if(SC->UseFog!=UseFog)	/* if fog disabled for this face change DrawFunctions */
		SOFT3D_SetDrawFunctions(SC,SC->ST,SC->TexMode,SC->ZMode,UseFog,SC->ColMode,SC->UseGouraud);

	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DumpObject.ON)
		DumpPoly(SC);

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
/*=============================================================*/
void UVtoRGBA(struct SOFT3D_texture *ST,float u,float v,UBYTE *RGBA)
{
register UBYTE *pt;
register ULONG offset;
register WORD x,y;

	RGBA[0]=RGBA[1]=RGBA[2]=RGBA[3]=0;
	if(ST==NULL) return;
	PrintST(ST);
	if(0.999 < u) u=0.999;
	if(0.999 < v) v=0.999;
	if(u  < 0.0 ) u=0.0;
	if(v  < 0.0 ) v=0.0;
	VARF(u)
	VARF(v)
	x=(WORD)(((float)ST->large) * u);
	y=(WORD)(((float)ST->high ) * v);
	offset=(ST->large*y + x)  * ST->bits / 8;
	pt=&(ST->pt[offset]);
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("UVtoRGBA x y %ld %ld --> %ld",x,y,offset);
#endif
	RGBA[0]=pt[0];
	RGBA[1]=pt[1];
	RGBA[2]=pt[2];
	RGBA[3]=255; if (ST->bits==32) RGBA[3]=pt[3];
	if(Wazp3D.DebugSOFT3D.ON) PrintRGBA((UBYTE *)RGBA);
}
/*=============================================================*/
#ifdef WAZP3DDEBUG
void TexturePlot(struct SOFT3D_texture *ST,UWORD x,UWORD y,UBYTE *ColorRGB)
{
UBYTE *RGB;
ULONG offset;

	if(x<ST->large)
	if(y<ST->high )
	{
	offset=(ST->large*y + x)  * ST->bits / 8;
	RGB=&(ST->pt[offset]);
	RGB[0]=ColorRGB[0];
	RGB[1]=ColorRGB[1];
	RGB[2]=ColorRGB[2];
	}
}
/*=============================================================*/
void TexturePrint(struct SOFT3D_texture *ST,WORD x,WORD y,UBYTE *texte)
{
UBYTE *F;
UBYTE RGB[3];
UWORD m,n,c;
UBYTE Bit[] = {128,64,32,16,8,4,2,1};
#define FONTSIZE 8
#define FONTLARGE (128/8)

	while(*texte!=0)
	{
	c=*texte++;
	if(32<c) c=c-32; else c=0; 
	F=&(font8x8[(c AND 15) + (c/16)*FONTLARGE*FONTSIZE]);
		MLOOP(FONTSIZE)
		{
		NLOOP(FONTSIZE)
			{
			RGB[0]=RGB[1]=RGB[2]=0;
			if (F[0] AND Bit[n]) 
				RGB[0]=RGB[1]=RGB[2]=255;
			TexturePlot(ST,x+n,y+m,RGB);
			}
		F+=FONTLARGE;
		}
	x+=FONTSIZE;
	}

}
#endif
/*================================================================*/
void SOFT3D_DrawPrimitive(struct SOFT3D_context *SC,struct point3D *P,LONG Pnb,ULONG primitive)
{
WORD n,nb;
#define W3D_PRIMITIVE_POLYGON	9999

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
	{
	Libprintf("SOFT3D_DrawPrimitive Pnb %ld (%ld)\n",Pnb,primitive);
	if(SC->ST!=NULL) Libprintf("%s\n",SC->ST->name);
	}
#endif

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
	if(Pnb<=MAXPOLYHACK)				/* a simple quad after a clipping in x y z can have now more than 4 points */
	{
	if(primitive==W3D_PRIMITIVE_TRIFAN)
		primitive=W3D_PRIMITIVE_POLYGON;

	if(primitive==W3D_PRIMITIVE_TRISTRIP)
		{

		if(Pnb==4)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[0],24);
		Libmemcpy(&(SC->PolyP[1]),&P[2],24);
		Libmemcpy(&(SC->PolyP[2]),&P[3],24);
		Libmemcpy(&(SC->PolyP[3]),&P[1],24);
		}

		if(Pnb==5)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[0],24);
		Libmemcpy(&(SC->PolyP[1]),&P[2],24);
		Libmemcpy(&(SC->PolyP[2]),&P[4],24);
		Libmemcpy(&(SC->PolyP[3]),&P[3],24);
		Libmemcpy(&(SC->PolyP[4]),&P[1],24);
		}

		if(Pnb==6)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[0],24);
		Libmemcpy(&(SC->PolyP[1]),&P[2],24);
		Libmemcpy(&(SC->PolyP[2]),&P[4],24);
		Libmemcpy(&(SC->PolyP[3]),&P[5],24);
		Libmemcpy(&(SC->PolyP[4]),&P[3],24);
		Libmemcpy(&(SC->PolyP[5]),&P[1],24);
		}

		if(Pnb==6)
		{
		Libmemcpy(&(SC->PolyP[0]),&P[0],24);
		Libmemcpy(&(SC->PolyP[1]),&P[2],24);
		Libmemcpy(&(SC->PolyP[2]),&P[4],24);
		Libmemcpy(&(SC->PolyP[3]),&P[5],24);
		Libmemcpy(&(SC->PolyP[4]),&P[6],24);
		Libmemcpy(&(SC->PolyP[5]),&P[3],24);
		Libmemcpy(&(SC->PolyP[6]),&P[1],24);
		}

		SC->PolyPnb=Pnb;
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
/*=================================================================*/
void TextureAlphaUsage(struct SOFT3D_texture *ST)
{
ULONG size,n,AllAnb,RatioA;
ULONG Anb[256];
UBYTE *RGBA;
#define RATIOALPHA 20

SFUNCTION(TextureAlphaUsage)
	if(ST->TexMode!='R')	/* already setted */
		return;

	if(ST->bits==32)
	{
	ST->TexMode='A';	/* alpha texture with blending */

		if(Wazp3D.UseRatioAlpha.ON)	/* count alpha-pixels  */
		{
		NLOOP(256)
			Anb[n]=0;

		size=ST->large*ST->high;
		RGBA=ST->pt;
		NLOOP(size)
			{Anb[RGBA[3]]++; RGBA+=4;}

		AllAnb=size-Anb[0]-Anb[255];
		if(AllAnb!=0) RatioA=(100*AllAnb)/size; else RatioA=0;

		if(RatioA<RATIOALPHA)
			ST->TexMode='a';

		if(Anb[0]   == size) ST->TexMode='R';
		if(Anb[255] == size) ST->TexMode='R';
		}
	}
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("T [%ld] %ld bytes(A0:%ld A255:%ld) AllAnb %ld RatioA %ld %\n",ST->TexMode,size,Anb[0],Anb[255],AllAnb,RatioA);
#endif
}
/*==================================================================*/
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,UWORD TexMode,BOOL MipMapped)
{
struct SOFT3D_texture *ST;
LONG   Tex8U;
UBYTE *Tex8V;
float Xratio,Yratio,nf;
UWORD Nbytes,n,m;
UBYTE BitmapName[40];
LONG Tsize;

SFUNCTION(SOFT3D_CreateTexture)
	ST=MYmalloc(sizeof(struct SOFT3D_texture),"SOFT3D_texture");
	if(ST==NULL) return(NULL);

	ST->pt	=pt;
	ST->large	=large;
	ST->high	=high;
	ST->bits	=bits;
	ST->TexMode='R';	/* default */
/* add to linkage */
	ST->nextST =SC->firstST;
	SC->firstST=ST;

	TextureAlphaUsage(ST);

SREM(Create tex index)
	Nbytes=bits/8;
	Xratio=((float)ST->large)/256.0;
	Yratio=((float)ST->high )/256.0;
	Tex8U=0;
	Tex8V=ST->pt;

	Tsize=ST->large*ST->high*ST->bits/8;

	if(MipMapped)
		CreateMipmaps(ST);

	MLOOP(10)
	{
	NLOOP(256)
		{
		nf=(float)n;
		ST->MMs[m].Tex8U[n]=Tex8U +(ULONG)(		    Nbytes*(UWORD)(Xratio*nf));
		ST->MMs[m].Tex8V[n]=Tex8V +(ULONG)( ST->large*Nbytes*(UWORD)(Yratio*nf));
		}

	if(ST->MipMapped)
	{
	Tsize/4;
	Tex8V=Tex8V+Tsize;
	Xratio=Xratio/2.0;
	Yratio=Yratio/4.0;
	if(m==0) 
		Tex8V=ST->ptmm;
	}

	}

	if (Wazp3D.DebugST.ON)
		{
		PrintST(ST);
		LibAlert("Texture done");
		}

	SC->Tnb++;
	ST->Tnum=SC->Tnb;

	Libsprintf(ST->name,"T:Texture%ld_%ldX%ldX%ld.RAW",ST->Tnum,ST->large,ST->high,ST->bits);
	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DumpTextures.ON)
		Libsavefile(ST->name,ST->pt,ST->large*ST->high*Nbytes);

	Libsprintf(ST->name,"Texture%ld_%ldX%ldX%ld.RAW",ST->Tnum,ST->large,ST->high,ST->bits);
	Libsprintf(BitmapName,"Tex%ld",ST->Tnum);

	if(Wazp3D.ReloadTextures.ON)
		Libloadfile(ST->name,ST->pt,ST->large*ST->high*Nbytes);

	if(Wazp3D.SmoothTextures.ON)
		SmoothTexture(ST->pt,ST->large,ST->high,SC->AliasedLines,ST->bits);

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugTexNumber.ON)
		TexturePrint(ST,0,ST->high/2,BitmapName);
#endif

	return( (void *) ST);
}
/*==================================================================*/
void SOFT3D_FreeTexture(struct SOFT3D_context *SC,void *texture)
{
struct SOFT3D_texture *ST=texture;
struct SOFT3D_texture fakeST;
struct SOFT3D_texture *thisST=&fakeST;
WORD Ntexture=0;

SFUNCTION(SOFT3D_FreeTexture)
	PrintST(ST);

	thisST->nextST=SC->firstST;
	while(thisST!=NULL)
	{
	VAL(Ntexture)
	if(thisST->nextST==ST)
		{
		SREM(is ST found)
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
#ifdef WAZP3DDEBUG
/*=================================================================*/
ULONG PrintError(LONG error)
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
char *drivertype="";

	if(!Wazp3D.DebugDriver.ON) return;
	if(D->swdriver==W3D_TRUE)	drivertype="Software";
	if(D->swdriver==W3D_FALSE)   drivertype="Hardware";

	WINFO(D->ChipID,W3D_CHIP_UNKNOWN," ");
	WINFO(D->ChipID,W3D_CHIP_VIRGE," ");
	WINFO(D->ChipID,W3D_CHIP_PERMEDIA2," ");
	WINFO(D->ChipID,W3D_CHIP_VOODOO1," ");
	WINFO(D->ChipID,W3D_CHIP_AVENGER_LE,"Voodoo 3 LE= Little endian mode")
	WINFO(D->ChipID,W3D_CHIP_AVENGER_BE,"Voodoo 3 BE= Big endian mode ")
	WINFO(D->ChipID,W3D_CHIP_PERMEDIA3," ");
	WINFO(D->ChipID,W3D_CHIP_RADEON," ");
	WINFO(D->ChipID,W3D_CHIP_RADEON2," ");

	SREM(Support formats:)
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
	VAR(&C->restex)
	VAR(&C->tex)
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
	VAR(&T->link)
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
/*=================================================================*/
#else
#define PrintError(error) error
#define PrintDriver(x)  ;
#define PrintContext(x) ;
#define PrintTexture(x) ;
#endif
/*==================================================================================*/
void WAZP3D_Init()
{
WORD ButtonCount,n;
struct MyButton *ButtonsList=(struct MyButton *)&Wazp3D.HardwareLie;

#define TRUECOLORFORMATS (W3D_FMT_R8G8B8|W3D_FMT_B8G8R8|W3D_FMT_A8R8G8B8|W3D_FMT_A8B8G8R8|W3D_FMT_R8G8B8A8|W3D_FMT_B8G8R8A8)
#define HIGHCOLORFORMATS (W3D_FMT_R5G5B5|W3D_FMT_B5G5R5|W3D_FMT_R5G5B5PC|W3D_FMT_B5G5R5PC|W3D_FMT_R5G6B5|W3D_FMT_B5G6R5|W3D_FMT_R5G6B5PC|W3D_FMT_B5G6R5PC|W3D_FMT_R8G8B8|W3D_FMT_B8G8R8|W3D_FMT_A8R8G8B8|W3D_FMT_A8B8G8R8|W3D_FMT_R8G8B8A8|W3D_FMT_B8G8R8A8)

	SREM(WAZP3D_Init)
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
	Libstrcpy(Wazp3D.TexFmtLie.name,"TexFmt Lie");
	Libstrcpy(Wazp3D.HackARGB.name,"Hack ARGB texs");
	Libstrcpy(Wazp3D.OnlyTrueColor.name,"Only TrueColor 24&32");
	Libstrcpy(Wazp3D.UsePolyHack.name,"Use Poly Hack");
	Libstrcpy(Wazp3D.UseColorHack.name,"Use BGcolor Hack");
	Libstrcpy(Wazp3D.UseCullingHack.name,"Use Culling Hack");
	Libstrcpy(Wazp3D.UseFog.name,"Use Fog");
	Libstrcpy(Wazp3D.UseColoring.name,"Use Coloring");
	Libstrcpy(Wazp3D.UseColoringGL.name,"Use Coloring GL");
	Libstrcpy(Wazp3D.UseClearDrawRegion.name,"Use ClearDrawRegion");
	Libstrcpy(Wazp3D.UseClearImage.name,"Use ClearImage");
	Libstrcpy(Wazp3D.UseMinUpdate.name,"Use Min. Update");
	Libstrcpy(Wazp3D.UpdateAtUnlock.name,"Update At UnlockHardware");
	Libstrcpy(Wazp3D.UpdateClearZ.name,"ClearZbuffer at Update");
	Libstrcpy(Wazp3D.HackRGBA1.name,"Tex as RGBA1 Hack");
	Libstrcpy(Wazp3D.HackRGBA2.name,"Tex as RGBA2 Hack");
	Libstrcpy(Wazp3D.UseAntiScreen.name,"AntiAlias Screen");
	Libstrcpy(Wazp3D.SmoothTextures.name,"Smooth Textures");
	Libstrcpy(Wazp3D.ReloadTextures.name,"Reload Textures");
	Libstrcpy(Wazp3D.UseClipper.name,"Use Clipper");
	Libstrcpy(Wazp3D.NoPerspective.name,"No Perspective");
	Libstrcpy(Wazp3D.TruePerspective.name,"True Perspective");
	Libstrcpy(Wazp3D.DirectBitmap.name,"Directly draw in Bitmap");
	Libstrcpy(Wazp3D.UseRatioAlpha.name,"Use RatioAlpha(20%)");
	Libstrcpy(Wazp3D.UseAlphaMinMax.name,"Use AlphaMin&Max");
	Libstrcpy(Wazp3D.DebugTexNumber.name,"Debug Tex number");
	Libstrcpy(Wazp3D.QuakePatch.name,"Quake Patch");
	Libstrcpy(Wazp3D.DoMipMaps.name,"Do MipMaps");

	Libstrcpy(Wazp3D.DebugWazp3D.name,">>> Enable Debugger >>>");
	Libstrcpy(Wazp3D.DisplayFPS.name,"Display FPS");
	Libstrcpy(Wazp3D.DebugFunction.name,"Debug Function");
	Libstrcpy(Wazp3D.StepFunction.name, "[Step]  Function");
	Libstrcpy(Wazp3D.DebugCalls.name,"Debug Calls");
	Libstrcpy(Wazp3D.DebugAsJSR.name,"Debug as JSR");
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
	Libstrcpy(Wazp3D.StepUpdate.name,"[Step] Update");
	Libstrcpy(Wazp3D.StepSOFT3D.name, "[Step]  SOFT3D");
	Libstrcpy(Wazp3D.DebugSC.name,"Debug SC");
	Libstrcpy(Wazp3D.DebugST.name,"Debug ST");
	Libstrcpy(Wazp3D.DebugSepiaImage.name,"Debug Sepia Image");
	Libstrcpy(Wazp3D.DebugClipper.name,"Debug Clipper");
	Libstrcpy(Wazp3D.DumpTextures.name,"Dump Textures");
	Libstrcpy(Wazp3D.DumpObject.name,"Dump Object");
	Libstrcpy(Wazp3D.DebugMemList.name,"Debug MemList");
	Libstrcpy(Wazp3D.DebugMemUsage.name,"Debug MemUsage");

	ButtonCount=((LONG)&Wazp3D.DebugMemUsage -(LONG)&Wazp3D.HardwareLie)/sizeof(struct MyButton)+1;
	NLOOP(ButtonCount)
		ButtonsList[n].ON=FALSE;

	Wazp3D.HardwareLie.ON=TRUE;
	Wazp3D.TexFmtLie.ON=TRUE;
	Wazp3D.HackARGB.ON=TRUE;
	Wazp3D.UsePolyHack.ON=TRUE;
	Wazp3D.UseColorHack.ON=TRUE;
	Wazp3D.UseClearDrawRegion.ON=TRUE;
	Wazp3D.UseClearImage.ON=TRUE;
	Wazp3D.UseClipper.ON=TRUE;
	Wazp3D.UseMinUpdate.ON=TRUE;
	Wazp3D.UseRatioAlpha.ON=TRUE;
	Wazp3D.UseAlphaMinMax.ON=TRUE;

#if defined(__AROS__)
	Wazp3D.UseColoringGL.ON=TRUE;		/* we assume that AROS' cpu is fast enough to enable those nice features */
	Wazp3D.TruePerspective.ON=TRUE;
#endif

	LibDebug=FALSE;

	Wazp3D.smode=(W3D_ScreenMode *)&Wazp3D.smodelist;
	Wazp3D.drivertype=W3D_DRIVER_CPU;
	Wazp3D.DriverList[0]=&Wazp3D.driver;
	Wazp3D.DriverList[1]=NULL;

	Libstrcpy(Wazp3D.DriverName,DRIVERNAME);
	Wazp3D.driver.name	=Wazp3D.DriverName;
	Wazp3D.driver.ChipID	=W3D_CHIP_UNKNOWN;
	Wazp3D.driver.formats	=HIGHCOLORFORMATS;
	Wazp3D.driver.swdriver	=W3D_TRUE;

	if(Wazp3D.OnlyTrueColor.ON)
		Wazp3D.driver.formats=TRUECOLORFORMATS;

	if(Wazp3D.HardwareLie.ON)
		{
		Wazp3D.drivertype=W3D_DRIVER_3DHW;
		Wazp3D.driver.swdriver=W3D_FALSE;
		}

}
/*==================================================================================*/
void WAZP3D_Close()
{
#ifdef WAZP3DDEBUG
WORD n;

	SREM(WAZP3D_Close)
	NLOOP(88)
		if(Wazp3D.DebugCalls.ON)
			{
			if (n==40)	LibAlert("More calls...");
			Libprintf("[%ld] %ld\tcalls to %s \n",1+n,Wazp3D.FunctionCalls[1+n],Wazp3D.FunctionName[1+n]);
			}
	if(Wazp3D.DebugCalls.ON)
		LibAlert("All calls listed");
#endif
}
/*==================================================================================*/
ULONG BytesPerPix1(ULONG format)
{
ULONG bpp=0;

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
ULONG BytesPerPix2(ULONG format)
{
/* after conversion to RGB24 or RGB32 */
ULONG bpp2;

	bpp2=32/8;
	if(format==W3D_R8G8B8)	bpp2=24/8;
	if(format==W3D_R5G6B5)	bpp2=24/8;
	if(format==W3D_L8)	bpp2=24/8;
	return(bpp2);
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

if(pt1==NULL) return;
if(pt2==NULL) return;

if(format==W3D_CHUNKY)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA[0]=palette[(*color8*4)+0];
		RGBA[1]=palette[(*color8*4)+1];
		RGBA[2]=palette[(*color8*4)+2];
		RGBA[3]=palette[(*color8*4)+3];
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

SREM(ConvertBitmap Done)
}
/*==================================================================================*/
ULONG MakeNewTexdata(W3D_Texture *texture)
{
UWORD high,large,bpp2;
ULONG format=texture->texfmtsrc;

SFUNCTION(MakeNewTexdata)

	high =texture->texheight;
	large=texture->texwidth;
	bpp2=BytesPerPix2(format);

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
void ClipPoint(struct point3D *PN,struct point3D *P,float t)
{
register float c,c0,c1;

	P->x= PN[0].x + t * (PN[1].x - PN[0].x) ;
	P->y= PN[0].y + t * (PN[1].y - PN[0].y) ;
	P->z= PN[0].z + t * (PN[1].z - PN[0].z) ;
	P->u= PN[0].u + t * (PN[1].u - PN[0].u) ;
	P->v= PN[0].v + t * (PN[1].v - PN[0].v) ;

	c0=PN[0].RGBA[0]; c1=PN[1].RGBA[0]; c=c0 + t * (c1 - c0); P->RGBA[0]=c;
	c0=PN[0].RGBA[1]; c1=PN[1].RGBA[1]; c=c0 + t * (c1 - c0); P->RGBA[1]=c;
	c0=PN[0].RGBA[2]; c1=PN[1].RGBA[2]; c=c0 + t * (c1 - c0); P->RGBA[2]=c;
	c0=PN[0].RGBA[3]; c1=PN[1].RGBA[3]; c=c0 + t * (c1 - c0); P->RGBA[3]=c;
}
/*=================================================================*/
#define NewPointClipX(xlim) {ClipPoint(PN,P,(xlim - PN[0].x) / (PN[1].x - PN[0].x)); P++; NewPnb++; }
#define NewPointClipY(ylim) {ClipPoint(PN,P,(ylim - PN[0].y) / (PN[1].y - PN[0].y)); P++; NewPnb++; }
#define NewPointClipZ(zlim) {ClipPoint(PN,P,(zlim - PN[0].z) / (PN[1].z - PN[0].z)); P++; NewPnb++; }
#define CopyPoint(A)  {Libmemcpy(P,A,sizeof(struct point3D)); P++; NewPnb++; }
#define SwapBuffers useT1=!useT1; if(useT1) {PN=T1; P=T2;} else	{PN=T2; P=T1;}
/*=================================================================*/
void PrintP2(struct point3D *P)
{
	Libprintf("ClipXYZ; %ld; %ld; %ld; UV; %ld; %ld\n",(WORD)P->x,(WORD)P->y,(WORD)(1024.0*P->z),(WORD)(256.0*P->u),(WORD)(256.0*P->v));
}
/*=================================================================*/
void ClipPoly(struct SOFT3D_context *SC)
{
struct point3D *P;
struct point3D *PN;
struct point3D *T1=(struct point3D *)&SC->T1;
struct point3D *T2=(struct point3D *)&SC->T2;
BOOL useT1=TRUE;
UBYTE  IsInside[MAXPOLY];
UBYTE InsidePnb;
LONG	Pnb=SC->PolyPnb;
LONG n,NewPnb;
BOOL FaceClipped;


if(Wazp3D.DebugClipper.ON) REM(ClipPoly)
	if(Pnb>MAXPOLY) return;
	FaceClipped=FALSE;
	NewPnb=0;

	useT1=TRUE;
	PN=T1; P=T2;
	Libmemcpy(PN,SC->PolyP,Pnb*sizeof(struct point3D));

NLOOP(Pnb)
{
PN[n].x=floor(PN[n].x);
PN[n].y=floor(PN[n].y);
}

NewPnb=InsidePnb=0;
NLOOP(Pnb)
{
IsInside[n]=FALSE;
if(PN[n].x <= SC->ClipMax.x)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Max.x) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Max.x)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipX(SC->ClipMax.x);

PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;
NLOOP(Pnb)
{

IsInside[n]=FALSE;
if(SC->ClipMin.x <= PN[n].x)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Min.x) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Min.x)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipX(SC->ClipMin.x);
PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;
NLOOP(Pnb)
{

IsInside[n]=FALSE;
if(PN[n].y <= SC->ClipMax.y)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Max.y) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Max.y)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipY(SC->ClipMax.y);
PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;
NLOOP(Pnb)
{

IsInside[n]=FALSE;
if(SC->ClipMin.y <= PN[n].y)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Min.y) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Min.y)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipY(SC->ClipMin.y);
PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
if(SC->ZMode==0)	goto DontClipZ;

NewPnb=InsidePnb=0;
NLOOP(Pnb)
{
IsInside[n]=FALSE;
if(PN[n].z <= SC->ClipMax.z)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}

if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Max.z) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Max.z)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipZ(SC->ClipMax.z);
PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
NewPnb=InsidePnb=0;
NLOOP(Pnb)
{
IsInside[n]=FALSE;
if(SC->ClipMin.z <= PN[n].z)
	{ IsInside[n]=TRUE;	InsidePnb++; }
}
if(InsidePnb==0) {if(Wazp3D.DebugClipper.ON) REM(Outside Min.z) goto HideFace;}

if(InsidePnb!=Pnb)
{
if(Wazp3D.DebugClipper.ON) REM(Clip Min.z)
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; Libmemcpy(&(PN[Pnb]),PN,sizeof(struct point3D));
NLOOP(Pnb)
{
if(IsInside[n]==TRUE)
		CopyPoint(PN);
if(IsInside[n]!=IsInside[n+1])
		NewPointClipZ((SC->ClipMin.z+0.001));
PN++;
}
Pnb=NewPnb;
SwapBuffers
}
/*=================================*/
DontClipZ:
	if(FaceClipped==FALSE) return;

/* if clipped get the new PolyPnb & PolyP */

	SC->PolyPnb=Pnb;
	if(useT1) PN=T1; else PN=T2;

	if(Wazp3D.DebugClipper.ON)
	{
		Wazp3D.DebugPoint.ON=TRUE;
		REM(CLIPPER-)
		NLOOP(Pnb)
			PrintP(&SC->PolyP[n]);
		REM(--------)
		NLOOP(Pnb)
			PrintP2(&PN[n]);
		REM(========)
		Wazp3D.DebugPoint.ON=FALSE;
	}
	Libmemcpy(SC->PolyP,PN,SC->PolyPnb*sizeof(struct point3D));
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

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("ReduceBitmap/%ld %ldX%ld %ldbits >from %ld to %ld\n",ratio,large,high,bits,pt,pt2);
#endif
	if(pt ==NULL) return;
	if(pt2==NULL) return;
	L=large * bits /8;
	P=bits /8;
	large=large/ratio;
	high =high /ratio;
	ratio2=ratio*ratio;

	RGB1=pt;
	RGB2=pt2;
SREM(Reducing ...)
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
SREM(ReduceBitmap OK)
}
/*==================================================================================*/
void CreateMipmaps(struct SOFT3D_texture *ST)
{
UBYTE *ptmm;
UWORD large,high,level,reduction;
ULONG size;


	large=ST->large;
	high =ST->high;
	size =ST->large * ST->high * ST->bits / 8;
	ST->ptmm=MYmalloc(size/3,"SOFT3D_mipmap"); 		/* size is mathematically false but allways fit */
	if(ST->ptmm==NULL) return;
	ptmm=ST->ptmm;

	level=0;
	reduction=2;
next_mipmap:
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("MipMap %ldX%ld = %ld (%ld)\n",large,high,size,ptmm);
#endif
	ReduceBitmap(ST->pt,ptmm,ST->large,ST->high,ST->bits,reduction);
	level++;
	large=large/2;
	high =high /2;
	size =size /4;
	reduction=reduction*2;
	ptmm=ptmm+size;
	if (high>0) goto next_mipmap;
	ST->MipMapped=TRUE;
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

	if(MAXPOINTS<WC->Pnb) return;
	P=&(WC->P[WC->Pnb]);
	WC->Pnb++;
	P->x=V->x;
	P->y=V->y;
	P->z=(float)V->z;

	P->u=V->u * WC->uresize;
	P->v=V->v * WC->vresize;
	P->RGBA[0]=V->color.r * 255.0;
	P->RGBA[1]=V->color.g * 255.0;
	P->RGBA[2]=V->color.b * 255.0;
	P->RGBA[3]=V->color.a * 255.0;

	PrintP(P);
}
/*==========================================================================*/
void GetPoint(W3D_Context *context,ULONG i)
{
/* MiniGL bug = it dont use W3D_VertexPointer & W3D_TexCoordPointer & W3D_ColorPointer  */
struct WAZP3D_context *WC=context->driver;
UBYTE *V		=context->VertexPointer;
ULONG  Vformat	=context->VPMode;
ULONG  Vstride	=context->VPStride;

WORD   unit		=0;
UBYTE *UV		=context->TexCoordPointer[unit];
ULONG  UVformat	=context->TPFlags[unit];
ULONG  UVstride	=context->TPStride[unit];
ULONG  UVoffsetv	=context->TPVOffs[unit];

UBYTE *C		=context->ColorPointer;
ULONG  Cformat	=context->CPMode;
ULONG  Cstride	=context->CPStride;

UBYTE *pt;
struct VertexFFF *fff;
struct VertexFFD *ffd;
struct VertexDDD *ddd;
float *u;
float *v;
float *rgbaF;
UBYTE *rgbaB;
UBYTE  RGBA[4];
struct point3D *P;

	if(MAXPOINTS<WC->Pnb) return;
	P=&(WC->P[WC->Pnb]);
	P->x=P->y=P->z=P->u=P->v=0.0;
	COPYRGBA(P->RGBA,WC->CurrentRGBA);	/* by default the point use CurrentColor */
	WC->Pnb++;

/* recover XYZ values */
	if(V!=NULL)
	{
	pt=&(V[i*Vstride]);
	if(Vformat==W3D_VERTEX_F_F_F)
	{
	fff=(struct VertexFFF *)pt;
	P->x=fff->x;
	P->y=fff->y;
	P->z=fff->z;
	}
	if(Vformat==W3D_VERTEX_F_F_D)
	{
	ffd=(struct VertexFFD *)pt;
	P->x=ffd->x;
	P->y=ffd->y;
	P->z=(float)ffd->z;
	}
	if(Vformat==W3D_VERTEX_D_D_D)
	{
	ddd=(struct VertexDDD *)pt;
	P->x=(float)ddd->x;
	P->y=(float)ddd->y;
	P->z=(float)ddd->z;
	}
	}


/* recover UV values */
	if(V!=NULL)
	{
	pt=&(UV[i*UVstride]);
	u=(float *)pt;
	v=(float *)&pt[UVoffsetv];

	if(UVformat==W3D_TEXCOORD_NORMALIZED)
	{
	P->u=u[0];
	P->v=v[0];
	}
	else
	{
	P->u=u[0] * WC->uresize;
	P->v=v[0] * WC->vresize;
	}
	}

/* recover Color RGBA values */
	if(C!=NULL)
	{
	pt=&(C[i*Cstride]);
	if(Cformat AND W3D_COLOR_FLOAT)
		{
		rgbaF=(float *)pt;
		RGBA[0]=rgbaF[0]*255.0;
		RGBA[1]=rgbaF[1]*255.0;
		RGBA[2]=rgbaF[2]*255.0;
		RGBA[3]=rgbaF[3]*255.0;
		}
	if(Cformat AND W3D_COLOR_UBYTE)
		{
		rgbaB=(UBYTE *)pt;
		RGBA[0]=rgbaB[0];
		RGBA[1]=rgbaB[1];
		RGBA[2]=rgbaB[2];
		RGBA[3]=rgbaB[3];
		}
	if(Cformat AND W3D_CMODE_RGB)		P->RGBA[0]=RGBA[0]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[2]; P->RGBA[3]=255;
	if(Cformat AND W3D_CMODE_BGR)		P->RGBA[0]=RGBA[2]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[0]; P->RGBA[3]=255;
	if(Cformat AND W3D_CMODE_RGBA)	P->RGBA[0]=RGBA[0]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[2]; P->RGBA[3]=RGBA[3];
	if(Cformat AND W3D_CMODE_ARGB)	P->RGBA[0]=RGBA[1]; P->RGBA[1]=RGBA[2]; P->RGBA[2]=RGBA[3]; P->RGBA[3]=RGBA[0];
	if(Cformat AND W3D_CMODE_BGRA)	P->RGBA[0]=RGBA[2]; P->RGBA[1]=RGBA[1]; P->RGBA[2]=RGBA[0]; P->RGBA[3]=RGBA[3];
	}
	PrintP(P);
}
/*==========================================================================*/
struct pixels9 {
UBYTE *P00;
UBYTE *P01;
UBYTE *P02;
UBYTE *P10;
UBYTE *P11;
UBYTE *P12;
UBYTE *P20;
UBYTE *P21;
UBYTE *P22;
UBYTE *NewP;
};
/*==========================================================================*/
void SmoothPixel(struct pixels9 *R)
{
register UWORD x;
register ULONG four=4;

		x=R->P11[0]; x=x+x; x=x+x; x=x+x;	/* x8 */
		x=x+R->P00[0]+R->P01[0]+R->P02[0]+R->P10[0]+R->P12[0]+R->P20[0]+R->P21[0]+R->P22[0];
		R->NewP[0]=x>>four;			/* /16 */

		x=R->P11[1]; x=x+x; x=x+x; x=x+x;
		x=x+R->P00[1]+R->P01[1]+R->P02[1]+R->P10[1]+R->P12[1]+R->P20[1]+R->P21[1]+R->P22[1];
		R->NewP[1]=x>>four;

		x=R->P11[2]; x=x+x; x=x+x; x=x+x;
		x=x+R->P00[2]+R->P01[2]+R->P02[2]+R->P10[2]+R->P12[2]+R->P20[2]+R->P21[2]+R->P22[2];
		R->NewP[2]=x>>four;

}
/*==========================================================================*/
void SmoothTexture(void *image,UWORD large,UWORD high,UBYTE *AliasedLines,UBYTE bits)
{
register UWORD x,y;
register UBYTE *Pdone;
register UBYTE *P=(UBYTE *)image;
register UBYTE *linedone;
register UBYTE *line;
register ULONG px=bits/8;
register ULONG py=large*px;
UBYTE *temp;
struct pixels9 R;

	if (large>MAXSCREEN) return;

	YLOOP(high)
	{
	linedone=AliasedLines;
	line    =AliasedLines+py;
	if(y&1)
		SWAP(line,linedone)

		XLOOP(large)
		{
		R.NewP=line;

		R.P10=P-px;
		R.P11=P;
		R.P12=P+px;
		R.P00=R.P10-py;
		R.P01=R.P11-py;
		R.P02=R.P12-py;
 		R.P20=R.P10+py;
		R.P21=R.P11+py;
		R.P22=R.P12+py;

		if(x==0)		{ R.P00+=px; R.P10+=px; R.P20+=px; } 
		if(x==large)	{ R.P02-=px; R.P12-=px; R.P22-=px; } 
		if(y==0)		{ R.P00+=py; R.P01+=py; R.P02+=py; } 
		if(y==high)		{ R.P20-=py; R.P21-=py; R.P22-=py; } 

		SmoothPixel(&R);

		line+=px;
		P+=px;
		}

	/* copy previous line from previous buffer */
		Pdone=P-py-py;
		if(y!=0)
		XLOOP(large)
		{
		Pdone[0]=linedone[0];
		Pdone[1]=linedone[1];
		Pdone[2]=linedone[2];
		Pdone+=px;
		linedone+=px;
		}
	}

	/* copy last line from last buffer */
	Pdone   =P-py;
	linedone=line-py;
	XLOOP(large)
	{
	Pdone[0]=linedone[0];
	Pdone[1]=linedone[1];
	Pdone[2]=linedone[2];
	Pdone+=px;
	linedone+=px;
	}
}
/*==========================================================================*/
void AntiAliasImage(void *image,UWORD large,UWORD high,UBYTE *AliasedLines)
{
register UWORD r,g,b,x,y;
register UBYTE *L0=(UBYTE *)image;
register UBYTE *L1;
register UBYTE *L2;
register UBYTE *line0;
register UBYTE *line1;
UBYTE *temp;
#define B32 4

	if (large>MAXSCREEN) return;
	line0=(UBYTE *) &AliasedLines[B32*MAXSCREEN*0];
	line1=(UBYTE *) &AliasedLines[B32*MAXSCREEN*1];
	L1=L0+large*B32;
	L2=L1+large*B32;
	large=large-2;
	high =high -2;

	XLOOP(large)
	{
	line0[0+B32]=L0[0+B32];
	line0[1+B32]=L0[1+B32];
	line0[2+B32]=L0[2+B32];
	line0+=B32;
	L0+=B32;
	}
	line1=(UBYTE *)&AliasedLines[B32*MAXSCREEN*1];
	L0=(UBYTE *)image;

	YLOOP(high)
	{
	line0=(UBYTE *) &AliasedLines[B32*MAXSCREEN*0];
	line1=(UBYTE *) &AliasedLines[B32*MAXSCREEN*1];
	if(y&1)
		SWAP(line0,line1)
		XLOOP(large)
		{
		r=L1[0+B32]; r=r+r; r=r+r; r=r+r;
		g=L1[1+B32]; g=g+g; g=g+g; g=g+g;
		b=L1[2+B32]; b=b+b; b=b+b; b=b+b;
		r=r+L0[0+0]+L0[0+B32]+L0[0+B32*2]+L1[0+0]+L1[0+B32*2]+L2[0+0]+L2[0+B32]+L2[0+B32*2];
		g=g+L0[1+0]+L0[1+B32]+L0[1+B32*2]+L1[1+0]+L1[1+B32*2]+L2[1+0]+L2[1+B32]+L2[1+B32*2];
		b=b+L0[2+0]+L0[2+B32]+L0[2+B32*2]+L1[2+0]+L1[2+B32*2]+L2[2+0]+L2[2+B32]+L2[2+B32*2];

		line1[0+B32]=r>>4;
		line1[1+B32]=g>>4;
		line1[2+B32]=b>>4;

		L0[0+B32]=line0[0+B32];
		L0[1+B32]=line0[1+B32];
		L0[2+B32]=line0[2+B32];

		line0+=B32;
		line1+=B32;
		L0+=B32;
		L1+=B32;
		L2+=B32;
		}

	L0+=B32*2;
	L1+=B32*2;
	L2+=B32*2;
	}
}
/*==========================================================================*/
void SepiaImage(void *image,ULONG size)
{
register UBYTE *RGBA0=(UBYTE *)image;
register UWORD r;
register ULONG n;

	NLOOP(size)
	{
	r=RGBA0[0]+RGBA0[1]+RGBA0[2];
	RGBA0[0]=255;
	if(r<=255*2) RGBA0[0]=r/2;
	RGBA0[1]=r/4;
	RGBA0[2]=r/6;
	RGBA0+=B32;
	}
}
/*==========================================================================*/
void SaveMTL(struct SOFT3D_context *SC,UBYTE *filenameMTL)
{
#ifdef WAZP3DDEBUG
struct SOFT3D_texture *ST;
struct  face3D *F=SC->DumpF;
LONG Fnb=SC->DumpFnum;
BOOL ThisTextureIsUsed;
ULONG f;
WORD t;

	if(SC->firstST==NULL) return;
	Libprintf("%s ====================\n",filenameMTL);

/* mat for untextured faces */
	Libprintf("newmtl MatFlat\n");
	Libprintf("illum 4\n");
	Libprintf("Ni 1.00\nKd 0.00 0.00 0.00\nKa 0.00 0.00 0.00\nTf 1.00 1.00 1.00\n");

/* mats for textured faces */
	t=0;
	ST=SC->firstST;
	while(ST!=NULL)
	{
		ThisTextureIsUsed=FALSE;
		FLOOP(Fnb)
			if(F[f].ST==ST)
				ThisTextureIsUsed=TRUE;

		if(ThisTextureIsUsed==TRUE)
		{
		Libprintf("newmtl MatTex%ld\n",t);
		Libprintf("illum 4\n");
		Libprintf("map_Kd %s\n",ST->name);
		Libprintf("Ni 1.00\nKd 0.00 0.00 0.00\nKa 0.00 0.00 0.00\nTf 1.00 1.00 1.00\n");
		}
	t++;
	ST=ST->nextST;
	}
LibAlert("Dump .MTL done :-)");
#endif
}
/*==========================================================================*/
void SaveOBJ(struct SOFT3D_context *SC,UBYTE *filename)
{
#ifdef WAZP3DDEBUG
struct SOFT3D_texture *ST;
struct point3D *P=SC->DumpP;
struct  face3D *F=SC->DumpF;
LONG Pnb=SC->DumpPnum;
LONG Fnb=SC->DumpFnum;
WORD Tnb=SC->Tnb;
UBYTE filenameOBJ[256];
UBYTE filenameMTL[256];
UBYTE name[40];
BOOL ThisTextureIsUsed;
ULONG f,p,t,n,i;

	VAR(SC->DumpP)
	VAR(SC->DumpPnb)
	VAR(SC->DumpPnum)
	VAR(SC->DumpF)
	VAR(SC->DumpFnb)
	VAR(SC->DumpFnum)

	n=strlen(filename);
	strcpy(filenameMTL,filename);
	filenameMTL[n-4]=0;
	strcat(filenameMTL,".mtl");
	SaveMTL(SC,filenameMTL);


	strcpy(filenameOBJ,filename);
	filenameOBJ[n-4]=0;
	strcat(filenameOBJ,".obj");

	Libprintf("%s ====================\n",filenameOBJ);

	if(SC->firstST!=NULL)
	Libprintf("mtllib %s\n",filenameMTL);

	Libprintf("g default\n");


	PLOOP(Pnb)
		{ Libprintf("v  ");  pf(P[p].x); pf(P[p].y); pf(P[p].z); Libprintf("\n"); }
	PLOOP(Pnb)
		{ Libprintf("vt  "); pf(P[p].u); pf(P[p].v); Libprintf("\n"); }
	PLOOP(Pnb)
		{ Libprintf("vn  "); pf(1.0); pf(1.0); pf(1.0); Libprintf("\n"); }		/* dont compute normals = simpler */

	strcpy(name,"DumpWazp3D");
	Libprintf("g %s\n",name);

/* 1: untextured faces */
	Libprintf("usemtl MatFlat\n");
	FLOOP(Fnb)
		if(F[f].ST==NULL)
		{
		Libprintf("f");
		PLOOP(F[f].Pnb)
			{
			i=F[f].Pnum+p+1;				/* The numbering starting with 1 */
			Libprintf(" %ld/%ld/%ld",i,i,i);	/* use same indice for Vi UVi Ni */
			}
		Libprintf("\n");
		}

/* 2: textured faces */
	t=0;
	ST=SC->firstST;
	while(ST!=NULL)
	{
		ThisTextureIsUsed=FALSE;
		FLOOP(Fnb)
			if(F[f].ST==ST)
				ThisTextureIsUsed=TRUE;

		if(ThisTextureIsUsed==TRUE)
		{
		Libprintf("usemtl MatTex%ld\n",t);
		FLOOP(Fnb)
			if(F[f].ST==ST)
			{
			Libprintf("f");
			PLOOP(F[f].Pnb)
				{
				i=F[f].Pnum+p+1;				/* The numbering starting with 1 */
				Libprintf(" %ld/%ld/%ld",i,i,i);	/* use same indice for Vi UVi Ni */
				}
			Libprintf("\n");
			}
		}
	t++;
	ST=ST->nextST;
	}

LibAlert("Dump .OBJ done :-)");

#endif
}
/*================================================================*/
void DumpObject(struct SOFT3D_context *SC)
{
#ifdef WAZP3DDEBUG

	Libprintf("DumpObject(%ld/3)\n",SC->DumpStage);

	if(SC->DumpStage==0)	/* 0=reset */
		{
		SC->DumpFnum=SC->DumpPnum=SC->DumpFnb=SC->DumpPnb=0;
		SC->DumpF=NULL;
		SC->DumpP=NULL;
		}

	if(SC->DumpStage==1)	/* 1=count&alloc*/
		{
		VAR(SC->DumpFnb)
		VAR(SC->DumpPnb)
		SC->DumpPnb=SC->DumpPnb+SC->DumpPnb/2;	/* alloc 150% the previous size */
		SC->DumpFnb=SC->DumpFnb+SC->DumpFnb/2;
		SC->DumpP=MYmalloc(SC->DumpPnb*sizeof(struct point3D),"DumpP");
		SC->DumpF=MYmalloc(SC->DumpFnb*sizeof(struct  face3D),"DumpF");
		if(SC->DumpP==NULL) SC->DumpStage=3;	/* then free all */
		if(SC->DumpF==NULL) SC->DumpStage=3;
		}

	if(SC->DumpStage==2)	/* 2=dump&save */
		{
		SaveOBJ(SC,"DumpWazp3D.obj");
		}

	if(SC->DumpStage==3)	/* 3=clear */
		{
		MYfree(SC->DumpP);
		MYfree(SC->DumpF);
		Wazp3D.DumpObject.ON=FALSE;
		}

	if(SC->DumpStage<3)
		SC->DumpStage++;
	else
		SC->DumpStage=0;

#endif
}
/*==========================================================================*/
void	 DrawText(W3D_Context *context,UBYTE *text,WORD x,WORD y)
{
struct WAZP3D_context *WC=context->driver;
struct RastPort *rp=&WC->rastport;

	SetAPen(rp, 2);
	SetDrMd(rp, JAM1);
	Move(rp,x,y+context->yoffset);
	Text(rp,text, strlen(text));
	SetAPen(rp, 1);
}
/*==========================================================================*/
void DoUpdate(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct SOFT3D_context *SC=WC->SC;
UBYTE RGBA[4];
UBYTE name[40];
ULONG MilliTime;

SREM(DoUpdate-------------------------)
	if(WC->Image8==NULL) return;
	if(SC->Pxmax==0) return;
	if(SC->Pymax==0) return;
	W3D_Flush(context);
	if(Wazp3D.DirectBitmap.ON)	goto ClearZBuffer;

	if(Wazp3D.StepUpdate.ON) LibAlert("Update will occurs now !!");


SREM(MinUpdate)
	if(Wazp3D.UseMinUpdate.ON)
	{
	SC->xUpdate	=SC->Pxmin;	/* should also be used to clear the previous drawing */
	SC->yUpdate	=SC->Pymin;
	SC->largeUpdate=SC->Pxmax-SC->Pxmin+1;
	SC->highUpdate =SC->Pymax-SC->Pymin+1;
	}
	else
	{
	SC->xUpdate	=0;
	SC->yUpdate	=0;
	SC->largeUpdate=WC->large;
	SC->highUpdate =WC->high;
	}

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("Updated Region from %ld %ld (%ld X %ld pixels) \n",SC->xUpdate,SC->yUpdate,SC->largeUpdate,SC->highUpdate);
#endif

SREM(UseColorHack)
	if(Wazp3D.UseColorHack.ON)
		{
		STACKReadPixelArray(RGBA,0,0,1*(32/8),&WC->rastport,0,0,1,1,RECTFMT_RGBA);
		SOFT3D_SetBackground(WC->SC,RGBA);
		PrintRGBA((UBYTE *)RGBA);
		}

SREM(UseAntiScreen)
	if(Wazp3D.UseAntiScreen.ON)
		AntiAliasImage(SC->Image32,SC->large,SC->high,SC->AliasedLines);
	if(Wazp3D.DebugSepiaImage.ON)
		SepiaImage(SC->Image32,SC->large*SC->high);

SREM(STACKWritePixelArray)
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
	if(Wazp3D.UseClearImage.ON)
		SOFT3D_ClearImage(WC->SC,0,0,SC->large,SC->high);
	}

SREM(Px Py)
	SC->Pxmin=WC->large-1;
	SC->Pymin=WC->high -1;
	SC->Pxmax=0;
	SC->Pymax=0;

ClearZBuffer:
SREM(ClearZBuffer)
	if(Wazp3D.UpdateClearZ.ON)
		SOFT3D_ClearZBuffer(SC,ZMAX);

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DumpObject.ON)
		DumpObject(SC);

	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DisplayFPS.ON)
	{
	MilliTime=LibMilliTimer();
	WC->TimePerFrame= MilliTime - WC->LastMilliTime;
	WC->LastMilliTime=MilliTime;
	if(WC->TimePerFrame!=0)
		WC->Fps=(((ULONG)1000)/WC->TimePerFrame);
	Libsprintf(name,"Wazp3D FPS:%ld",WC->Fps);
	DrawText(context,name,4,10);
	}
#endif

SREM(DoUpdateOK)
}
/*==========================================================================*/
W3D_Context	*W3D_CreateContext(ULONG *error, struct TagItem *taglist)
{
ULONG tag,data;
UWORD n,bits;
W3D_Context	*context;
W3D_Bitmap  *w3dbm;
struct WAZP3D_context *WC;
struct SOFT3D_context *SC;
ULONG AllStates=~0;
ULONG UnsupportedStates= W3D_ANTI_POINT & W3D_ANTI_LINE & W3D_ANTI_POLYGON & W3D_DITHERING & W3D_LOGICOP & W3D_STENCILBUFFER & W3D_ALPHATEST & W3D_SPECULAR & W3D_TEXMAPPING3D & W3D_CHROMATEST;
ULONG EnableMask,DisableMask,supportedfmt,envsupmask;
BOOL stateOK=TRUE;

#if defined(__AROS__)
	UnsupportedStates= UnsupportedStates & W3D_DOUBLEHEIGHT;
#endif

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
	envsupmask=W3D_REPLACE & W3D_DECAL & W3D_MODULATE & W3D_BLEND;	/* v40: full implementation */

	WC->ModeID=INVALID_ID;

	context->driver=WC;				 /* insert driver specific data here */
	context->gfxdriver=NULL;			/* usable by the GFXdriver */	/* TODO: find if it is the pointer to the driver ? */
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

	context->maxtexwidth  =MAXTEXTURE;		/* -HJF- replaced these for possible */
	context->maxtexheight =MAXTEXTURE;		/* support of non-square textures */
	context->maxtexwidthp =MAXTEXTURE;		/* -HJF- For hardware that has different */
	context->maxtexheightp=MAXTEXTURE;		/* constaints in perspective mode */
							 /* scissor region */
	context->scissor.left	=0;
	context->scissor.top	=0;
	context->scissor.width	=0;
	context->scissor.height	=0;
							 /* fogging parameters */
	context->fog.fog_color.r	=1.0;
	context->fog.fog_color.g	=1.0;
	context->fog.fog_color.b	=1.0;
	context->fog.fog_start		=0.999;
	context->fog.fog_end		=0.000;
	context->fog.fog_density	=0.1;

	context->envsupmask=envsupmask;	 /* Mask of supported envmodes */
	context->queue=NULL;			 /* queue to buffer drawings */
	context->drawmem=NULL;			 /* base address for drawing operations */

	context->globaltexenvmode=W3D_MODULATE;		/* Global texture env mode V41: More OpenGL compatible*/

	context->globaltexenvcolor[0]=1.0;			/* Global texture env color */
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
	context->TPStride[n]=0;			/* Stride of TexCoordPointers */
	context->TPVOffs[n]=0;			/* Offset to V coordinate */
	context->TPWOffs[n]=0;			/* Offset to W coordinate */
	context->TPFlags[n]=0;			/* Flags */
	}

	context->ColorPointer=NULL;		 /* Pointer to the color array */
	context->CPStride=0;			 /* Color pointer stride */
	context->CPMode=0;			 /* Mode and color format */
	context->CPFlags=0;			 /* not yet used */

	context->FrontFaceOrder=W3D_CW;	/* Winding order of front facing triangles */
	context->specialbuffer=NULL;		/* Special buffer for chip-dependant use (like command */

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

	while (taglist->ti_Tag != TAG_DONE)
	{
	if (taglist->ti_Tag == TAG_MORE) {
	  taglist = (struct TagItem *)taglist->ti_Data;
	  continue;
	}

	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;

	if(tag==W3D_CC_MODEID )		WC->ModeID			=data;
	if(tag==W3D_CC_BITMAP )		context->drawregion	=(struct BitMap *)data;
	if(tag==W3D_CC_YOFFSET)		context->yoffset		=data;
	if(tag==W3D_CC_DRIVERTYPE)	context->drivertype	=data;
	if(tag==W3D_CC_W3DBM)		context->w3dbitmap	=data;

	if(tag==W3D_CC_INDIRECT)		stateOK=SetState(context,W3D_INDIRECT	,data);
	if(tag==W3D_CC_GLOBALTEXENV )		stateOK=SetState(context,W3D_GLOBALTEXENV	,data);
	if(tag==W3D_CC_DOUBLEHEIGHT )		stateOK=SetState(context,W3D_DOUBLEHEIGHT	,data);
	if(tag==W3D_CC_FAST )			stateOK=SetState(context,W3D_FAST		,data);
	if(!stateOK)
		*error=W3D_UNSUPPORTEDSTATE;

	if(!Wazp3D.HardwareLie.ON)
	if(context->drivertype == W3D_DRIVER_3DHW)
		*error=W3D_ILLEGALINPUT;

	WTAG(W3D_CC_TAGS,"Tags")
	WTAG(W3D_CC_BITMAP,"destination bitmap")
	WTAG(W3D_CC_YOFFSET,"y-Offset ")
	WTAG(W3D_CC_DRIVERTYPE,"Driver type ")
	WTAG(W3D_CC_W3DBM,"Use W3D_Bitmap")
	WTAG(W3D_CC_INDIRECT,"Indirect drawing ")
	WTAG(W3D_CC_GLOBALTEXENV,"SetTexEnv is global")
	WTAG(W3D_CC_DOUBLEHEIGHT,"Drawing area has double height")
	WTAG(W3D_CC_FAST,"Allow Warp3D to modify passed structures ")
	WTAG(W3D_CC_MODEID,"Specify modeID to use")
	PrintError(*error);

	if(*error!=W3D_SUCCESS)
		{W3D_DestroyContext(context);context=NULL;return(context);}
	}
	WINFO(context->drivertype,W3D_DRIVER_UNAVAILABLE,"driver unavailable ")
	WINFO(context->drivertype,W3D_DRIVER_BEST,"use best mode ")
	WINFO(context->drivertype,W3D_DRIVER_3DHW,"use 3D-HW ")
	WINFO(context->drivertype,W3D_DRIVER_CPU,"use CPU ")


	SetBitmap(context,context->drawregion);
	context->scissor.left	=0;
	context->scissor.top	=0;
	context->scissor.width	=context->width;
	context->scissor.height	=context->height;

	if(StateON(W3D_DOUBLEHEIGHT))
		WC->high=WC->high/2;	/* if doubleheight use only the half height for the Wazp3D RGBA buffer*/
	else
		context->yoffset=0;	/* AROS cant do doubleheight */

/*beta39: if the bitmap (ie context->drawregion) is 32 bits then try to use it as the Wazp3D RGBA buffer */


	bits=context->bprow/context->width*8;
	if(bits!=32)
		Wazp3D.DirectBitmap.ON=FALSE;

VAR(context->width)
VAR(context->height)
VAR(context->depth)
VAR(context->bprow)
VAR(context->format)
VAR(bits)
VAR(context->drawregion->BytesPerRow)
VAR(context->drawregion->Rows)
VAR(context->drawregion->Flags)
VAR(context->drawregion->Depth)
VAR(context->drawregion->pad)

	if(Wazp3D.DirectBitmap.ON)
	if(LockBM(context))
		{
		UnLockBM(context);
		Wazp3D.UseColorHack.ON=FALSE;		/* Wazp3D RGBA buffer's options : uncompatible with DirectBitmap */
		Wazp3D.UseClearImage.ON=FALSE;
		Wazp3D.UseMinUpdate.ON=FALSE;
		Wazp3D.UpdateAtUnlock.ON=FALSE;
		Wazp3D.UseAntiScreen.ON=FALSE;
		Wazp3D.DebugSepiaImage.ON=FALSE;
		}

/* else alloc the Wazp3D RGBA buffer */
	if(WC->Image8==NULL)
		{
		Wazp3D.DirectBitmap.ON=FALSE;
		WC->Image8=MYmalloc(WC->large*WC->high*32/8,"SOFT3D_ImageBuffer");
		}

#if defined(__AROS__)
	/* WarpTest doesn't draw the triangles without this */
	Wazp3D.UpdateAtUnlock.ON=TRUE;
#endif

	InitRastPort(&WC->rastport);
	WC->rastport.BitMap=context->drawregion;
	WC->firstWT=NULL;
	WC->CallFlushFrame=WC->CallSetDrawRegion=WC->CallClearZBuffer=FALSE;
	WC->PointSize=1.0;

	WC->CallSetBlending=FALSE;

	WC->SC=SOFT3D_Start(WC->large,WC->high,(ULONG *)WC->Image8);
	SC=WC->SC;
	SC->context=context;
	Wazp3D.UseAntiScreen.ON=StateON(W3D_ANTI_FULLSCREEN);

	DoUpdate(context);	/* clear Image buffer */

	return(context);
}
/*==========================================================================*/
#if PROVIDE_VARARG_FUNCTIONS
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
#endif // PROVIDE_VARARG_FUNCTIONS
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

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugVal.ON)
	if(Wazp3D.DebugState.ON)
		{if(action==W3D_DISABLED) {Libprintf(" [ ] ");} else {Libprintf(" [X] ");}}
#endif

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
	if(context->EnableMask  & state)
		{context->state = context->state |  state; return(TRUE);}

	if(!set)
	if(context->DisableMask & state)
		{context->state = context->state & ~state; return(TRUE);}

	return(FALSE);
}
/*==========================================================================*/
ULONG W3D_SetState(W3D_Context *context, ULONG state, ULONG action)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(5);
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugVal.ON)
	if(Wazp3D.DebugState.ON)
		{if(action==W3D_DISABLED)  {Libprintf(" [ ] ");} else {Libprintf(" [X] ");}}
#endif

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

	if(state==W3D_BLENDING)
		WC->CallSetBlending=TRUE;

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
#ifdef WAZP3DDEBUG
		if(quality==W3D_H_FAST)		Libprintf(" [FAST] ");
		if(quality==W3D_H_AVERAGE)	Libprintf(" [AVER] ");
		if(quality==W3D_H_NICE)		Libprintf(" [NICE] ");
#endif
		;
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
	VAR(Wazp3D.drivertype);
	return(Wazp3D.drivertype);
}
/*==========================================================================*/
BOOL LockBM(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct BitMap *bm=context->drawregion;
UBYTE *drawmem;	/* = bitmap memory ptr */

	WC->bmHandle=LockBitMapTags((APTR)bm,LBMI_BASEADDRESS,(ULONG)&drawmem, TAG_DONE);
	drawmem=drawmem + context->yoffset*context->drawregion->BytesPerRow;  /* not needed ? */
	WC->Image8=drawmem;		/* Wazp3D RGBA buffer is now the bitmap memory*/
	if(context->drawmem!=drawmem)		/* update SOFT3D to make it use this bitmap as buffer */
		{
		SOFT3D_SetImage(WC->SC,WC->large,WC->high,(ULONG *)WC->Image8);
		SOFT3D_SetZbuffer(WC->SC,context->zbuffer);
		}
	context->drawmem	=drawmem;		/* store it so can detect if bitmap memory changed */
	return(WC->bmHandle!=NULL);
}
/*==========================================================================*/
void UnLockBM(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	if(WC->bmHandle!=NULL)
		UnLockBitMap(WC->bmHandle);
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
	if(Wazp3D.UpdateAtUnlock.ON)
		DoUpdate(context);	/*draw the image if any */
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
ULONG QueryDriver(W3D_Driver* driver, ULONG query, ULONG destfmt)
{
ULONG support=W3D_NOT_SUPPORTED;
UBYTE sup='N';

	if(driver!=&Wazp3D.driver) return(support);

	VAR(destfmt)
	VAR(query)

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
	QUERY(W3D_Q_MIPMAPPING,"mipmapping ",'N');
	QUERY(W3D_Q_BILINEARFILTER,"bilinear filter ",'N');
	QUERY(W3D_Q_MMFILTER,"mipmap filter ",'N');
	QUERY(W3D_Q_LINEAR_REPEAT,"W3D_REPEAT for linear texmapping ",'Y');
	QUERY(W3D_Q_LINEAR_CLAMP,"W3D_CLAMP for linear texmapping ",'N');
	QUERY(W3D_Q_PERSPECTIVE,"perspective correction ",'Y');
	QUERY(W3D_Q_PERSP_REPEAT,"W3D_REPEAT for persp. texmapping ",'Y');
	QUERY(W3D_Q_PERSP_CLAMP,"W3D_CLAMP for persp. texmapping ",'N');
	QUERY(W3D_Q_ENV_REPLACE,"texenv REPLACE ",'Y');
	QUERY(W3D_Q_ENV_DECAL,"texenv DECAL ",'Y');
	QUERY(W3D_Q_ENV_MODULATE,"texenv MODULATE ",'Y');
	QUERY(W3D_Q_ENV_BLEND,"texenv BLEND ",'Y');
	QUERY(W3D_Q_FLATSHADING,"flat shading ",'Y');
	QUERY(W3D_Q_GOURAUDSHADING,"gouraud shading ",'Y');
	QUERY(W3D_Q_ZBUFFER,"Z buffer in general ",'Y');
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
	QUERY(W3D_Q_ANTIALIASING,"antialiasing in general ",'y');
	QUERY(W3D_Q_ANTI_POINT,"point antialiasing ",'N');
	QUERY(W3D_Q_ANTI_LINE,"line antialiasing ",'N');
	QUERY(W3D_Q_ANTI_POLYGON,"polygon antialiasing ",'N');
	QUERY(W3D_Q_ANTI_FULLSCREEN,"fullscreen antialiasing ",'Y');
	QUERY(W3D_Q_DITHERING,"dithering ",'N');
	QUERY(W3D_Q_SCISSOR,"scissor test ",'Y');
	QUERY(W3D_Q_MAXTEXWIDTH,"max. texture width ",'Y');
	QUERY(W3D_Q_MAXTEXHEIGHT,"max. texture height ",'Y');
	QUERY(W3D_Q_MAXTEXWIDTH_P,"max. texture width persp",'Y');
	QUERY(W3D_Q_MAXTEXHEIGHT_P,"max. texture height persp",'Y');
	QUERY(W3D_Q_RECTTEXTURES,"rectangular textures ",'Y');
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
		sup='Y';

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

	if(query==W3D_Q_MAXTEXWIDTH)
		support=MAXTEXTURE;
	if(query==W3D_Q_MAXTEXHEIGHT)
		support=MAXTEXTURE;
	if(query==W3D_Q_MAXTEXWIDTH_P)
		support=MAXTEXTURE;
	if(query==W3D_Q_MAXTEXHEIGHT_P)
		support=MAXTEXTURE;

	return(support);
}
/*==========================================================================*/
ULONG W3D_Query(W3D_Context *context, ULONG query, ULONG destfmt)
{
	WAZP3D_Function(12);
	return(QueryDriver(&Wazp3D.driver,query,destfmt));	/* only one driver */
}
/*==========================================================================*/
ULONG W3D_GetTexFmtInfo(W3D_Context *context, ULONG texfmt, ULONG destfmt)
{
	WAZP3D_Function(13);
	return(W3D_GetDriverTexFmtInfo(&Wazp3D.driver,texfmt,destfmt));
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
	return(QueryDriver(driver,query,destfmt));
}
/*==========================================================================*/
ULONG W3D_GetDriverTexFmtInfo(W3D_Driver* driver, ULONG texfmt, ULONG destfmt)
{
ULONG support  =W3D_TEXFMT_UNSUPPORTED;
ULONG supported=W3D_TEXFMT_SUPPORTED+W3D_TEXFMT_FAST+W3D_TEXFMT_ARGBFAST;

	WAZP3D_Function(18);
	if(driver!=&Wazp3D.driver) return(support);

	if(destfmt==0)				/*patch: to make StormMesa happy*/
		support=supported;

	if(texfmt==W3D_R8G8B8)
		support=supported;

	if(texfmt==W3D_R8G8B8A8)
		support=supported;

	if(Wazp3D.TexFmtLie.ON)
		support=supported;

	if(destfmt !=0)				/*patch: StormMesa use this undocumented 0 value */
	if((destfmt & Wazp3D.driver.formats)==0)
		support=W3D_TEXFMT_UNSUPPORTED;

	VAR(texfmt)
	VAR(destfmt)
	WINFOB(support,W3D_TEXFMT_SUPPORTED,"format is supported,although it may be converted")
	WINFOB(support,W3D_TEXFMT_FAST,"format directly supported ")
	WINFOB(support,W3D_TEXFMT_CLUTFAST,"format is directly supported on LUT8 screens")
	WINFOB(support,W3D_TEXFMT_ARGBFAST,"format is directly supported on 16/24 bit screens")
	WINFOB(support,W3D_TEXFMT_UNSUPPORTED,"this format is unsupported,and can't be simulated")
	return(support);
}
/*==========================================================================*/
#ifdef __AROS__
AROS_UFH3(BOOL, ScreenModeFilter,
AROS_UFHA(struct Hook *, h	  , A0),
AROS_UFHA(APTR		 , object , A2),
AROS_UFHA(APTR		 , message, A1))
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

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf("ScreenModeFilter[%ld]%ld X %ld X %ld\n",ID,large,high,bits);
#endif

	if( MAXSCREEN < large) return FALSE;
	if( MAXSCREEN < high ) return FALSE;

	if(Wazp3D.ASLsize)
	{
	if( large < Wazp3D.ASLminX ) return FALSE;
	if( large > Wazp3D.ASLmaxX ) return FALSE;
	if( high  < Wazp3D.ASLminY ) return FALSE;
	if( high  > Wazp3D.ASLmaxY ) return FALSE;
	}

	if(bits < 15) return FALSE;

	if(Wazp3D.OnlyTrueColor.ON)
		if(bits < 24) return FALSE;

	return TRUE;
	}
	return FALSE;
#ifdef __AROS__
	AROS_USERFUNC_EXIT
#endif
}
/*==========================================================================*/
#include <utility/hooks.h>
ULONG W3D_RequestMode(struct TagItem *taglist)
{
W3D_Driver *driver;
ULONG tag,data;
ULONG size,format,drivertype,ModeID=INVALID_ID;
struct Library *AslBase;
struct ScreenModeRequester *requester;

struct Hook filter;
filter.h_Entry = (HOOKFUNC)ScreenModeFilter;

	WAZP3D_Function(19);
	size=TRUE;
	driver=&Wazp3D.driver;
	Wazp3D.ASLminX=0;
	Wazp3D.ASLmaxX=MAXSCREEN;
	Wazp3D.ASLminY=0;
	Wazp3D.ASLmaxY=MAXSCREEN;
	Wazp3D.ASLsize=FALSE;

	drivertype=Wazp3D.drivertype;
	format=Wazp3D.driver.formats;

	while (taglist->ti_Tag != TAG_DONE)
	{
	if (taglist->ti_Tag == TAG_MORE) {
	  taglist = (struct TagItem *)taglist->ti_Data;
	  continue;
	}
	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;
	if(tag==W3D_SMR_SIZEFILTER)	Wazp3D.ASLsize	=TRUE;
	if(tag==W3D_SMR_DRIVER )	driver		=(W3D_Driver *)data;

	if(tag==W3D_SMR_DESTFMT)	format		=data;
	if(tag==W3D_SMR_TYPE)		drivertype		=data;

	if(tag==ASLSM_MinWidth)		Wazp3D.ASLminX	=data;
	if(tag==ASLSM_MaxWidth)		Wazp3D.ASLmaxX	=data;
	if(tag==ASLSM_MinHeight)	Wazp3D.ASLminY	=data;
	if(tag==ASLSM_MaxHeight)	Wazp3D.ASLmaxY	=data;

	WTAG(W3D_SMR_TAGS,"taglist ")
	WTAG(W3D_SMR_DRIVER,"Driver to filter ")
	WTAG(W3D_SMR_DESTFMT,"Dest Format to filter ")
	WTAG(W3D_SMR_TYPE,"Type to filter ")
	WTAG(W3D_SMR_SIZEFILTER,"Also filter size ")

	WTAG(W3D_SMR_MODEMASK,"AND-Mask for modes ")
	WTAG(ASLSM_MinWidth," ")
	WTAG(ASLSM_MaxWidth," ")
	WTAG(ASLSM_MinHeight," ")
	WTAG(ASLSM_MaxHeight," ")
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
#if PROVIDE_VARARG_FUNCTIONS
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
#endif // PROVIDE_VARARG_FUNCTIONS
/*==========================================================================*/
W3D_Driver *	W3D_TestMode(ULONG ModeID)
{
ULONG format,bits,bytesperpixel;
W3D_Driver *driver;

	WAZP3D_Function(21);
	VAR(ModeID);
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

	if(Wazp3D.OnlyTrueColor.ON)
	if(bytesperpixel<3)
	if(bits<24)
		driver=NULL;

	VAR(driver);
	return(driver);
}
/*==========================================================================*/
void ARGBtoRGBA(UBYTE *RGBA,ULONG size)
{
register UBYTE a,r,g,b;
register ULONG n;

	NLOOP(size)
	{
		a=RGBA[0];
		r=RGBA[1];
		g=RGBA[2];
		b=RGBA[3];
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA[3]=a;
		RGBA+=4;
	}
}
/*==========================================================================*/
W3D_Texture	*W3D_AllocTexObj(W3D_Context *context, ULONG *error,struct TagItem *taglist)
{
W3D_Texture *texture=NULL;
struct WAZP3D_context *WC;
struct WAZP3D_texture *WT;
ULONG tag,data;
ULONG n;
APTR *MipPt=NULL;
ULONG size;
UWORD TexMode;
UWORD bits=0;
ULONG mask=1;

	WAZP3D_Function(22);
	WT=MYmalloc(sizeof(struct WAZP3D_texture),"WAZP3D_texture");
	if(WT==NULL)
		{ *error=W3D_NOMEMORY;PrintError(*error);return(NULL);}
	texture=&WT->texture;
	texture->driver=WT;

	while (taglist->ti_Tag != TAG_DONE)
	{
	if (taglist->ti_Tag == TAG_MORE) {
	  taglist = (struct TagItem *)taglist->ti_Data;
	  continue;
	}
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

	WTAG(W3D_ATO_TAGS,"tags");
	WTAG(W3D_ATO_IMAGE,"texture image ")
	WTAG(W3D_ATO_FORMAT,"source format ")
	WTAG(W3D_ATO_WIDTH,"border width ")
	WTAG(W3D_ATO_HEIGHT,"border height ")
	WTAG(W3D_ATO_MIPMAP,"mipmap mask ")
	WTAG(W3D_ATO_PALETTE,"texture palette ")
	WTAG(W3D_ATO_MIPMAPPTRS,"array of mipmap")
	}

	WT->large=texture->texwidth ;
	WT->high =texture->texheight;

	if(Wazp3D.HardwareLie.ON)
		texture->resident=TRUE;		/* TRUE, if texture is on card */
	else
		texture->resident=FALSE;

	texture->bytesperpix=BytesPerPix1(texture->texfmtsrc);
	texture->texdata=NULL;

	if(Wazp3D.TexFmtLie.ON)
	if(texture->texfmtsrc==W3D_A8R8G8B8)
	if(Wazp3D.HackARGB.ON)
		{
/*directly convert original texture data to RGBA (texsource) */
		ARGBtoRGBA(texture->texsource,texture->texheight*texture->texwidth);
		texture->texfmtsrc=W3D_R8G8B8A8;
		}

	texture->matchfmt=FALSE;		/* TRUE, if srcfmt = destfmt */
	if(texture->texfmtsrc==W3D_R8G8B8)
		{bits=24;texture->matchfmt=TRUE;}
	if(texture->texfmtsrc==W3D_R8G8B8A8)
		{bits=32;texture->matchfmt=TRUE;}

	if(Wazp3D.TexFmtLie.ON)
	if(!texture->matchfmt)
		bits=MakeNewTexdata(texture);
	if(bits==0)
		{MYfree(WT); *error=W3D_NOMEMORY; return(NULL); }


	if(!Wazp3D.TexFmtLie.ON)
	if(!texture->matchfmt)
		{MYfree(WT); *error=W3D_UNSUPPORTEDTEXFMT; return(NULL); }

	texture->dirty=FALSE;			 /* TRUE, if texture image was changed */
	texture->reserved1=FALSE;
	texture->reserved2=FALSE;

	if(MipPt!=NULL)
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

	WT->large	 =texture->texwidth ;
	WT->high	 =texture->texheight;
	WT->bits	 =bits;

	WT->TexEnvMode=W3D_MODULATE;		/* Global texture env mode */

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

	if(WT->bits==24) TexMode='R'; else TexMode='A';
	WT->ST=SOFT3D_CreateTexture(WC->SC,WT->pt,WT->large,WT->high,WT->bits,TexMode,Wazp3D.DoMipMaps.ON);

	PrintWT(WT);

	*error=W3D_SUCCESS;
	PrintError(*error);
	return(texture);
}
/*==========================================================================*/
#if PROVIDE_VARARG_FUNCTIONS
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
#endif // PROVIDE_VARARG_FUNCTIONS
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
		SREM(is Texture found)
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
	if(texture->texdata==NULL) SREM(texture got no converted texdata);
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
	WT->EnvRGBA[0]=envcolor->r * 255.0;
	WT->EnvRGBA[1]=envcolor->g * 255.0;
	WT->EnvRGBA[2]=envcolor->b * 255.0;
	WT->EnvRGBA[3]=envcolor->a * 255.0;
	}
	WT->TexEnvMode=envparam;
	PrintRGBA((UBYTE *)& WT->EnvRGBA );

	if(StateON(W3D_GLOBALTEXENV))
	{
	context->globaltexenvmode=envparam;			/* Global texture environment mode */
	context->globaltexenvcolor[0]=envcolor->r;	/* global texture env color */
	context->globaltexenvcolor[1]=envcolor->g;
	context->globaltexenvcolor[2]=envcolor->b;
	context->globaltexenvcolor[3]=envcolor->a;
	}

	WINFO(envparam,W3D_REPLACE,"unlit texturing ")
	WINFO(envparam,W3D_DECAL,"same as W3D_REPLACE use alpha to blend texture with primitive =lit-texturing")
	WINFO(envparam,W3D_MODULATE,"lit-texturing by modulation ")
	WINFO(envparam,W3D_BLEND,"blend with environment color ")
	PrintRGBA((UBYTE *)&WT->EnvRGBA);

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
VAR(level)
	if(level!=0)
		WRETURN(W3D_SUCCESS);	/* 0 is the tex !=0 are the mipmaps */

	scissor.left=0;
	scissor.top =0;
	scissor.width =texture->texwidth ;
	scissor.height=texture->texheight;
	srcbpr=texture->bytesperrow;
	return(W3D_UpdateTexSubImage(context,texture,teximage,level,palette,&scissor,srcbpr));
}
/*==========================================================================*/
ULONG W3D_UpdateTexSubImage(W3D_Context *context, W3D_Texture *texture, void *teximage,ULONG level, ULONG *palette, W3D_Scissor* scissor, ULONG srcbpr)
{
struct WAZP3D_texture *WT=texture->driver;
ULONG offset1,offset2;
UBYTE *RGB1;
UBYTE *RGB2;
UWORD x,y,high,large,Tlarge,bpp1,bpp2;
ULONG format,sizelarge;


	WAZP3D_Function(31);
VAR(level)
	if(level!=0)
		WRETURN(W3D_SUCCESS);	/* 0 is the tex !=0 are the mipmaps */

	x	=scissor->left;
	y	=scissor->top;
	high	=scissor->height;
	large	=scissor->width;
	Tlarge=texture->texwidth;
	format=texture->texfmtsrc;

	bpp1=texture->bytesperpix;				/* 1=new image */
	offset1=srcbpr - large*bpp1;
	RGB1=(UBYTE *)teximage;

	bpp2=texture->bytesperpix;
	offset2=texture->bytesperrow - large*bpp2;	/*2=original texture */
	RGB2=(UBYTE *)texture->texsource;
	RGB2=(UBYTE *)&RGB2[(y*Tlarge + x)*bpp2];


VAR(x)
VAR(y)
VAR(high )
VAR(large)
VAR(offset1)
VAR(offset2)
VAR(RGB1)
VAR(RGB2)
VAR(texture->texdata)
VAR(high*large*bpp1)

	if(offset1==0)
	if(offset2==0)
	{
SREM(memcpy)
		Libmemcpy(RGB2,RGB1,high*large*bpp1);	/*update all*/
	}
	else
	{
SREM(loops)
		sizelarge=large*bpp1;
		YLOOP(high)
		{
		XLOOP(sizelarge)
			*RGB1++=*RGB2++;			/*update the area*/
		RGB1+=offset1;
		RGB2+=offset2;
		}
	}

	if(texture->texdata==NULL)
		WRETURN(W3D_SUCCESS);

/* if got texsource->texdata re-convert the updated part */
	bpp1=texture->bytesperpix;				/*1=original  texture */
	RGB1=(UBYTE *)texture->texsource;
	RGB1=(UBYTE *)&RGB1[(y*Tlarge + x)*bpp1];
	offset1=(Tlarge-large)*bpp1;

	bpp2=BytesPerPix2(format);				/*2=converted texture */
	RGB2=(UBYTE *)texture->texdata;
	RGB2=(UBYTE *)&RGB2[(y*Tlarge + x)*bpp2];
	offset2=(Tlarge-large)*bpp2;
VAR(bpp1)
VAR(bpp2)
VAR(offset1)
VAR(offset2)
VAR(RGB1)
VAR(RGB2)
	ConvertBitmap(format,RGB1,RGB2,high,large,offset1,offset2,(UBYTE *)texture->palette);
	TextureAlphaUsage(WT->ST); /* Test if tex become transparent */
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
	 if(WC->PointSize<1.0)	WC->PointSize=1.0;	/* PATCH: skulpt dont set pointsize*/
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
LONG n;;

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
LONG n;;

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
	SOFT3D_DrawPixels(WC->SC);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLineStrip(W3D_Context *context, W3D_Lines *lines)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
LONG n;;

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
LONG n;;

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

/* PATCH: BlitzQuake/MiniGL use SetBlendMode but forget to activate(SetState) the blending */
	if(!WC->CallSetBlending)
		SetState(context,W3D_BLENDING,TRUE);
	
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void SetBitmap(W3D_Context *context, struct BitMap *bm)
{
struct WAZP3D_context *WC=context->driver;
W3D_Bitmap  *w3dbm;

	if(bm==NULL) return;

	if(context->w3dbitmap)
	{
	w3dbm=(W3D_Bitmap  *)context->drawregion;
	context->width  = w3dbm->width;	/* bitmap width */
	context->height = w3dbm->height;	/* bitmap height */
	context->depth  = GetBitMapAttr( context->drawregion, BMA_DEPTH  );	/* bitmap depth */
	context->bprow  = w3dbm->bprow;	/* bytes per row */
	context->format = w3dbm->format;	/* bitmap format (see below) */
	context->drawmem= w3dbm->dest;	/* base address for drawing operations */
	}
	else
	{
	context->width  = GetBitMapAttr( context->drawregion, BMA_WIDTH  );	/* bitmap width */
	context->height = GetBitMapAttr( context->drawregion, BMA_HEIGHT );	/* bitmap height */
	context->depth  = GetBitMapAttr( context->drawregion, BMA_DEPTH  );	/* bitmap depth */
	context->bprow  = context->drawregion->BytesPerRow;
	if(context->depth==24)	 context->format=W3D_FMT_R8G8B8;	/* is it allways correct ??? */
	if(context->depth==32)	 context->format=W3D_FMT_A8R8G8B8;
	context->format = 0;
	context->drawmem=NULL;
	}

	context->drawregion=bm;
	WC->rastport.BitMap=context->drawregion;
	WC->large=context->width ;
	WC->high =context->height;
}
/*==========================================================================*/
ULONG W3D_SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(46);
	VAR(bm)
	VAR(yoffset)
	WC->CallSetDrawRegion=TRUE;

	if(context->drawregion		==bm			)
	if(context->yoffset		==yoffset		)
	if(context->scissor.left	==scissor->left	)
	if(context->scissor.top		==scissor->top	)
	if(context->scissor.width	==scissor->width	)
	if(context->scissor.height	==scissor->height	)
		WRETURN(W3D_SUCCESS);		/* nothing to do ===> return */

/* draw the image if needed */
	DoUpdate(context);

	if(!StateON(W3D_DOUBLEHEIGHT))
		yoffset=0;			/* if cant do double-height (ie AROS) then let this to 0 */

/* change bitmap */
	SetBitmap(context,bm);

/* change yoffset */
	context->yoffset=yoffset;

/* change scissor */
	Libmemcpy(&context->scissor,scissor,sizeof(W3D_Scissor));
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
	SOFT3D_SetClipping(WC->SC,(float)WC->Xmin,(float)WC->Xmax,(float)WC->Ymin,(float)WC->Ymax,0.000,0.999);

	if(Wazp3D.DirectBitmap.ON)		/* if bitmap changed ==> update bitmap memory pointer */
	if(LockBM(context))
		UnLockBM(context);

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
	WC->FogZmin	   =fogparams->fog_end;		/* end is zmin !!! */
	WC->FogZmax	   =fogparams->fog_start;
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
/* horrible hack to recover the pen as RGBA TODO: find a better method*/
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
	DoUpdate(context);
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

	WAZP3D_Function(58);
	WC->CallClearZBuffer=TRUE;
	VARF(*clearvalue);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	if (z < 0.000)	z=0.000;
	if (0.999 < z)	z=0.999;
	z16=CONVERTZ*z;
	VAR(z16);
	SOFT3D_ClearZBuffer(WC->SC,z16);
	DoUpdate(context);	/*draw the image if any */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ReadZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z)
{
	WAZP3D_Function(59);
	VAR(x);
	VAR(y);
	VAR(z);
	return(W3D_ReadZSpan(context,x,y,1,z));
}
/*==========================================================================*/
ULONG W3D_ReadZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z)
{
struct WAZP3D_context *WC=context->driver;
register WORD *Zbuffer16=context->zbuffer;
register ULONG offset;

	WAZP3D_Function(60);
	VAR(x);
	VAR(y);
	VAR(n);
	VAR(z);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	offset=x+y*WC->large;
	Zbuffer16=&(Zbuffer16[offset]);
	XLOOP(n)
		z[x]=((W3D_Double)Zbuffer16[x])/CONVERTZ;

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
/*	return(W3D_WriteZSpan(context,x,y,1,z,&mask)); curiously W3D_WriteZSpan dont return an ULONG */
	W3D_WriteZSpan(context,x,y,1,z,&mask);
}
/*==========================================================================*/
void W3D_WriteZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *zD, UBYTE *mask)
{
struct WAZP3D_context *WC=context->driver;
register WORD *Zbuffer16 =context->zbuffer;
register ULONG offset;
register float z;

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
			Zbuffer16[x]=CONVERTZ*z;
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
	return(W3D_ReadStencilSpan(context,x,y,1,st));
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
	WC->StencilRef=refvalue;
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
	return(W3D_WriteStencilSpan(context,x,y,1,&stencilvalue,&mask));
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
LONG n;

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
LONG n;

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
	if (taglist->ti_Tag == TAG_MORE) {
	  taglist = (struct TagItem *)taglist->ti_Data;
	  continue;
	}
	tag =taglist->ti_Tag  ;	data=taglist->ti_Data ; taglist++;
	if(tag==W3D_BMI_DRIVER)	driver=data;
	if(tag==W3D_BMI_WIDTH)	large	=data;
	if(tag==W3D_BMI_HEIGHT)	high	=data;
	if(tag==W3D_BMI_DEPTH)	bits	=data;
	WTAG(W3D_BMI_TAGS," ")
	WTAG(W3D_BMI_DRIVER," ")
	WTAG(W3D_BMI_WIDTH," ")
	WTAG(W3D_BMI_HEIGHT," ")
	WTAG(W3D_BMI_DEPTH," ")
	}

	 if(Wazp3D.OnlyTrueColor.ON)
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
#if PROVIDE_VARARG_FUNCTIONS
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
#endif // PROVIDE_VARARG_FUNCTIONS
/*==========================================================================*/
ULONG W3D_VertexPointer(W3D_Context* context, void *pointer, int stride,ULONG mode, ULONG flags)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3D_Function(82);
	VAR(pointer)
	VAR(stride)
	WINFO(mode,W3D_VERTEX_F_F_F," ")
	WINFO(mode,W3D_VERTEX_F_F_D," ")
	WINFO(mode,W3D_VERTEX_D_D_D," ")

	context->VertexPointer=pointer;		/* Pointer to the vertex buffer array */
	context->VPStride=stride;			 /* Stride of vertex array */
	context->VPMode=mode;				 /* Vertex buffer format */
	context->VPFlags=flags;				 /* not yet used */
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

	context->ColorPointer=pointer;	/* Pointer to the color array */
	context->CPStride=stride;		/* Color pointer stride */
	context->CPMode=mode & format;	/* Mode and color format */
	context->CPFlags=flags;			/* not yet used=0 */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_BindTexture(W3D_Context* context, ULONG tmu, W3D_Texture *texture)
{
struct WAZP3D_context *WC=context->driver;
struct SOFT3D_context *SC=WC->SC;
struct WAZP3D_texture *WT=NULL;
void *ST=NULL;
BOOL blended;
ULONG TexEnvMode;
UBYTE EnvRGBA[4];
UBYTE TexModeS[2];
UBYTE ColModeS[2];

	WAZP3D_Function(85);
	VAR(tmu)
	VAR(texture)
	PrintTexture(texture);

	if(W3D_MAX_TMU <= tmu)
		WRETURN(W3D_ILLEGALINPUT);
	context->CurrentTex[tmu]=texture;

	if(texture==NULL)
		{
SREM(TEXTURE_NULL)
		WT=NULL;
		WC->WT=WT;
		ST=NULL;
		WC->uresize=1.0;
		WC->vresize=1.0;
		}
	else
		{
SREM(TEXTURE_NOT_NULL)
		WT=texture->driver;
		WC->WT=WT;
		ST=WT->ST;
		WC->uresize=1.0/(float)(WT->large - 1);
		WC->vresize=1.0/(float)(WT->high  - 1);
		PrintWT(WT);
		}

	TexEnvMode=context->globaltexenvmode;  		/* global texture-env-mode  */
	EnvRGBA[0]=255.0*context->globaltexenvcolor[0];	/* global texture-env-color */
	EnvRGBA[1]=255.0*context->globaltexenvcolor[1];	
	EnvRGBA[2]=255.0*context->globaltexenvcolor[2];	
	EnvRGBA[3]=255.0*context->globaltexenvcolor[3];	
	if(WT!=NULL)
	if(!StateON(W3D_GLOBALTEXENV))
	{
	TexEnvMode=WT->TexEnvMode;		/* texture's texture-env-mode  */
	COPYRGBA(EnvRGBA,WT->EnvRGBA);	/* texture's texture-env-color */
	}

	WC->TexMode='0';
	WC->ColMode='0';
	WC->UseFog  =  (StateON(W3D_FOGGING)) et (WC->FogMode!=0);
	WC->ZMode   =2*(StateON(W3D_ZBUFFERUPDATE)) + (StateON(W3D_ZBUFFER));
	WC->UseTex  =  (StateON(W3D_TEXMAPPING)) et (texture!=NULL);
	blended     =  (StateON(W3D_BLENDING));

	if(WC->UseTex)
		{
/* define WC->TexMode */
		WC->TexMode='R';			/* default = simple tex */
		if(blended)
			WC->TexMode='A';		/* alpha blended tex */
/* define WC->ColMode */
		if(TexEnvMode==W3D_REPLACE)
			WC->ColMode='0';		/* do only the tex function */

		if(Wazp3D.UseColoring.ON)	/* If  use color for lighting*/
		if(TexEnvMode!=W3D_REPLACE)
			WC->ColMode='m';		/* then do the simple Mix color & tex */

		if(Wazp3D.UseColoringGL.ON)	/* If  use color (new functions)  for lighting*/
			{
			if(TexEnvMode==W3D_DECAL)
				WC->ColMode='D';		/* then Mix color & tex */
			if(TexEnvMode==W3D_MODULATE)
				WC->ColMode='M';		/* then Mix color & tex */
			if(TexEnvMode==W3D_BLEND)
				WC->ColMode='B';		/* then Mix color & tex */
			}

		}
	else
		{
		WC->ColMode='R';
			if(blended)
			{
			if(WC->SrcFunc==W3D_SRC_ALPHA)
			if(WC->DstFunc==W3D_ONE_MINUS_SRC_ALPHA)
				WC->ColMode='A';
			if(WC->SrcFunc==W3D_ONE)
			if(WC->DstFunc==W3D_ONE)
				WC->ColMode='S';
			if(WC->SrcFunc==W3D_ZERO)
			if(WC->DstFunc==W3D_SRC_ALPHA)
				WC->ColMode='L';
			if(WC->SrcFunc==W3D_ZERO)
			if(WC->DstFunc==W3D_SRC_COLOR)
				WC->ColMode='M';
			}
		}

	WC->UseGouraud=FALSE;
	if(WC->ColMode!='0')
	if(StateON(W3D_GOURAUD))
		WC->UseGouraud=TRUE;

	if(WC->UseGouraud==FALSE)		/* flat color from current color */
		COPYRGBA(SC->FlatRGBA,WC->CurrentRGBA);

#ifdef WAZP3DDEBUG
	TexModeS[0]=WC->TexMode;
	ColModeS[0]=WC->ColMode;
	TexModeS[1]=ColModeS[1]=0;
	if(Wazp3D.DebugSOFT3D.ON) Libprintf(" ZMode%ld TexMode%s ColMode%s UseFog%ld UseTex%ld blended%ld UseGouraud%d\n",WC->ZMode,TexModeS,ColModeS,WC->UseFog,WC->UseTex,blended,WC->UseGouraud);
#endif

	SOFT3D_SetDrawFunctions(WC->SC,ST,WC->TexMode,WC->ZMode,WC->UseFog,WC->ColMode,WC->UseGouraud);
	SOFT3D_SetEnvColor(WC->SC,EnvRGBA);
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
		GetPoint(context,i++);

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
/* MiniGL bug = it change context->CurrentTex without using W3D_BindTexture */
		if(context->CurrentTex[0]!=NULL) W3D_BindTexture(context,0,context->CurrentTex[0]);
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
		GetPoint(context,*I8++);

	if(type==W3D_INDEX_UWORD)
	NLOOP(count)
		GetPoint(context,*I16++);

	if(type==W3D_INDEX_ULONG)
	NLOOP(count)
		GetPoint(context,*I32++);

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
/* MiniGL bug = it change context->CurrentTex without using W3D_BindTexture */
		if(context->CurrentTex[0]!=NULL) W3D_BindTexture(context,0,context->CurrentTex[0]);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,WC->primitive);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void		 W3D_SetFrontFace(W3D_Context* context, ULONG direction)
{
struct WAZP3D_context *WC=context->driver;
UWORD Culling=0;

	WAZP3D_Function(88);
	WINFO(direction,W3D_CW ,"Front face is clockwise");
	WINFO(direction,W3D_CCW,"Front face is counter clockwise");
	context->FrontFaceOrder=direction;

	if(context->FrontFaceOrder==W3D_CW)	Culling=1;
	if(context->FrontFaceOrder==W3D_CCW)   Culling=2;
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
