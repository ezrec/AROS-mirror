/* Wazp3D Beta 45.6 : Alain THELLIER - Paris - FRANCE - (November 2006 to Oct.  2009) */
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
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/cybergraphics.h>
#include <clib/Warp3D_protos.h>
#if !defined(__AROS__) && !defined(__MORPHOS__) 
typedef ULONG IPTR; 
#endif
/*==================================================================================*/
struct memory3D{
	UBYTE *pt;
	ULONG size;
	char name[40];
	void *nextME;
};
/*==================================================================================*/
#define MAXPRIM (3*1000)			/* Maximum points per primitive		*/
#define MAXPOLY	100				/* Maximum points per polygone		*/
#define MAXPOLYHACK 5				/* Maximum points per polygone in PolyHack*/
#define MAXPOLYHACK2 7				/* Maximum points per polygone in PolyHack2*/
#define MAXSCREEN  1024				/* Maximum screen size 1024x1024		*/
#define MAXTEXTURE 2048				/* Maximum tex size 2048x2048 (fast=256)  */
#define FOGSIZE 1000				/* Size for Fog values array			*/
#define MINZ 0.00000				/* min Zbuffer value				*/
#define MAXZ 0.99999				/* max Zbuffer value				*/
#define MINALPHA 5				/* Alpha range : A < MINALPHA ==> transp. */
#define MAXALPHA (255-MINALPHA)		/* Alpha range : MAXALPHA < A ==> solid	*/
#define FRAGBUFFERSIZE 10000			/* Fragments buffer (a pixels buffer)	*/
#define PSIZE sizeof(struct point3D)	/* Size for copying a point			*/

#define ZBUFF UWORD 				/* Zbuffer's format				*/
#define ZVALUE Pix->W.z				/* Z value is stored here			*/
#define ZCONVERSION Pix->L.z=0; Pix->W.z=(P->z * (float)ZRESIZE);
#define ZRESIZE 60000.0				/* Zbuffer's conversion	from float		*/

/* for a Z buffer as float 
#define ZBUFF float
#define ZVALUE Pix->L.z			
#define ZCONVERSION Pix->L.z=P->z;
#define ZRESIZE 1.0	
*/

#define DEBUGTNUM 9999				/* Activate debugger for this texture	*/
extern BOOL LibDebug;				/* Enable Library Debugger (global)		*/
struct memory3D *firstME=NULL;		/* Tracked memory-allocation	  		*/
/*==================================================================================*/
typedef const void * (*HOOKEDFUNCTION)(void *SC);
/*==================================================================================*/
#define DRIVERNAME "Wazp3D soft renderer - Alain Thellier - Paris France 2009 - Beta 45.6"
UBYTE Wazp3DPrefsName[] = {"Wazp3D Prefs.Beta 45.6.Alain Thellier 2009"};
IPTR Wazp3DPrefsNameTag=(IPTR) Wazp3DPrefsName;
/*==================================================================================*/
struct vertex3D{
	float x,y,z;
};
/*==================================================================================*/
struct point3D{
	float x,y,z;
	float u,v;
	float w;
	UBYTE RGBA[4];
};
/*==================================================================================*/
struct button3D{
	BOOL ON;
	char name[30];
	};
/*==================================================================================*/
struct WAZP3D_parameters{
char  FunctionName[89][50];
ULONG FunctionCalls[89];
ULONG FunctionCallsAll;
ULONG CrashFunctionCall;		/* begin to step the 3Dprog after this call */
ULONG NextStepFunction;			/* step the 3Dprog for this call */
char  DriverName[256];
ULONG drivertype;
W3D_Driver	driver;
W3D_Driver *DriverList[2];
W3D_ScreenMode *smode;
W3D_ScreenMode smodelist[50];
BOOL  ASLsize;
ULONG ASLminX,ASLmaxX,ASLminY,ASLmaxY;

struct button3D HardwareLie;		/* pretend to be a perfect hardware driver */
struct button3D TexFmtLie;		
struct button3D HackARGB;
struct button3D UseRatioAlpha;	/* v40: force BlendMode to 'a' if alpha-pixels < 20% */
struct button3D UseAlphaMinMax;	/* v40: ignore smallest (=transparent) and biggest (=solid) alpha values*/
struct button3D DirectBitmap;		/* v39: directly hit in the screen bitmap so no RGBA buffer & no update */
struct button3D OnlyTrueColor;	/* only use fast RGB/RGBA screens */
struct button3D SmoothTextures;
struct button3D ReloadTextures;
struct button3D DoMipMaps;		/* v44: mipmapping emulation = dont works */

struct button3D UseColorHack;		/* get background color from bitmap */
struct button3D UseClearDrawRegion;	/* let Warp3D clear his bitmap */
struct button3D UseClearImage;	/* let Wazp3D clear his RGBA buffer */
struct button3D UseMinUpdate;
struct button3D UsePolyHack;		/* convert trifan & tristrip to quad */
struct button3D UsePolyHack2;		/* v43: convert more trifan & tristrip to quad  */

struct button3D UseColoringGL;	/* v40:emulate truly decal/blend/modulate */
struct button3D UseColoring;		/*  do simple coloring */
struct button3D UseFog;
struct button3D NoPerspective;	/* v41: no perspective at all = faster */
struct button3D SimuPerspective;	/* v44: do perspective emulation (quadratic method) two times per polygon's raster */
struct button3D UseAntiImage;
struct button3D UseCullingHack;

struct button3D HackRGBA1;
struct button3D HackRGBA2;

struct button3D QuakePatch;		/* v41: Force blending if 32bits tex (if 32 bits then BlendMode is 'A' or 'a' allways) */

struct button3D DebugWazp3D;		/* global on/off for printing debug */
struct button3D DisplayFPS;

struct button3D DebugFunction;	/* usefull debug messages */
struct button3D DebugCalls;
struct button3D DebugState;
struct button3D DebugVar;
struct button3D DebugVal;
struct button3D DebugSOFT3D;
struct button3D DebugPoint;
struct button3D DebugClipper;

struct button3D DebugDoc;		/* useless debug messages*/
struct button3D DebugAdresses;
struct button3D DebugAsJSR;
struct button3D DebugDriver;
struct button3D DebugContext;
struct button3D DebugTexture;
struct button3D DebugError;
struct button3D DebugWC;
struct button3D DebugWT;
struct button3D DebugSC;
struct button3D DebugST;
struct button3D DebugTexNumber;	/* v40: print texture number in bitmap */
struct button3D DebugSepiaImage;	/* Colorize Wazp3D's RGBA buffer */
struct button3D DumpTextures;
struct button3D DumpObject;
struct button3D ResizeDumpedObject;
struct button3D StepFunction;		/* Step a Warp3D function call */
struct button3D StepSOFT3D;		/* Step a SOFT3D function call */
struct button3D StepUpdate;		/* Step at update (Wazp3D's RGBA buffer-->screen) */
struct button3D StepDrawPoly;		/* V42: Step each DrawPolyP() */
struct button3D StepFunction50;	/* V44: Step 50 Warp3D functions calls */
struct button3D DebugMemList;
struct button3D DebugMemUsage;
HOOKEDFUNCTION FunctionsFill[8];
HOOKEDFUNCTION FunctionsZtest[8*2+1];	
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

BOOL CallFlushFrame;			/* then DoUpdate inside bitmap*/
BOOL CallSetDrawRegion;
BOOL CallClearZBuffer;
BOOL CallSetBlending;
BOOL NeedNewZbuffer;
struct point3D P[MAXPRIM];
ULONG primitive;
ULONG Pnb;

struct RastPort rastport;
ULONG ModeID;

UWORD large,high,bits;
WORD Xmin,Ymin,Xmax,Ymax;					/* screen scissor */

float PointSize;
UBYTE hints[32];

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

UWORD Fps;
UWORD TimePerFrame;
ULONG LastMilliTime;

UBYTE ZMode;
UBYTE TexEnvMode;
UBYTE BlendMode;
UBYTE UseFog;
UBYTE UseGouraud;	
UBYTE UseTex;	
W3D_Context context;					/* The Warp3D's context is inside the Wazp3D's context WC */
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
	ZBUFF z;
	float w;
	LONG u;
	LONG v;
	LONG R;
	LONG G;
	LONG B;
	LONG A;
	LONG x;
	LONG y;
	LONG F;
	WORD large;
	ULONG *Image32Y;
	ZBUFF *ZbufferY;
	ZBUFF dz;
	float dw;
	LONG du;
	LONG dv;
	LONG ddu;
	LONG ddv;
	LONG dR;
	LONG dG;
	LONG dB;
	LONG dA;
	LONG dx;
	LONG dF;
	}  L;

#ifdef MOTOROLAORDER
struct pixel3DW{
	ZBUFF z;
	float w;
	UBYTE u1,u,u3,u4;
	UBYTE v1,v,v3,v4;
	UBYTE R1,R,R3,R4;
	UBYTE G1,G,G3,G4;
	UBYTE B1,B,B3,B4;
	UBYTE A1,A,A3,A4;
	WORD  x,xlow;
	WORD  y,ylow;
	WORD  F,Flow;
	WORD large;
	ULONG *Image32Y;
	ZBUFF *ZbufferY;
	ZBUFF dz;
	float dw;
	LONG du;
	LONG dv;
	LONG ddu;
	LONG ddv;
	LONG dR;
	LONG dG;
	LONG dB;
	LONG dA;
	LONG dx;
	LONG dF;
	}  W;
#else
struct pixel3DW{
	ZBUFF z;
	float w;
	UBYTE u4,u3,u,u1;
	UBYTE v4,v3,v,v1;
	UBYTE R4,R3,R,R1;
	UBYTE G4,G3,G,G1;
	UBYTE B4,B3,B,B1;
	UBYTE A4,A3,A,A1;
	WORD  xlow,x;
	WORD  ylow,y;
	WORD  Flow,F;
	WORD large;
	ULONG *Image32Y;
	ZBUFF *ZbufferY;
	ZBUFF dz;
	float dw;
	LONG du;
	LONG dv;
	LONG ddu;
	LONG ddv;
	LONG dR;
	LONG dG;
	LONG dB;
	LONG dA;
	LONG dx;
	LONG dF;
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
struct fragbuffer3D{
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
struct rgbabuffer3D{		/* same thing as fragbuffer3D but access to only one of the RGBA values */
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
struct SOFT3D_context{
	UBYTE Mul8[256*256];			/* table for multiplying two UBYTE */
	UBYTE Add8[256*256];			/* table for adding two UBYTE with overflow */
	ULONG Image0ToColor[256];		/* tables for converting 8/15/16 bits to RGBA */
	ULONG Image1ToColor[256];
	UBYTE Color0ToImage0[256];		
	UBYTE Color1ToImage0[256];
	UBYTE Color2ToImage0[256];
	UBYTE Color3ToImage0[256];
	UBYTE Color0ToImage1[256];
	UBYTE Color1ToImage1[256];
	UBYTE Color2ToImage1[256];
	UBYTE Color3ToImage1[256];
	WORD PolyHigh;
	WORD large,high;
	struct vertex3D ClipMin;			/* 3D clipper */
	struct vertex3D ClipMax;
	ULONG *Image32;
	ZBUFF *Zbuffer;
	UBYTE BackRGBA[4];
	UBYTE NoopRGBA[4];
	UBYTE CurrentRGBA[4];
	UBYTE FlatRGBA[4];
	struct SOFT3D_texture *ST;
	struct SOFT3D_mipmap  *MM;
	WORD Pnb;
	union pixel3D *Pix;
	WORD PointSize;
	WORD PointLarge[MAXSCREEN];
	union pixel3D edge1[MAXSCREEN];
	union pixel3D edge2[MAXSCREEN];
	union pixel3D edgeM[MAXSCREEN];
	UBYTE Ztest[MAXSCREEN];
	HOOKEDFUNCTION FunctionFill;				/* functions used in pipeline */
	HOOKEDFUNCTION FunctionZtest;	
	HOOKEDFUNCTION FunctionIn;
	HOOKEDFUNCTION FunctionBlend;
	HOOKEDFUNCTION FunctionTexEnv;
	HOOKEDFUNCTION FunctionFog;
	HOOKEDFUNCTION FunctionSepia;
	HOOKEDFUNCTION FunctionOut;
	struct fragbuffer3D FragBuffer[FRAGBUFFERSIZE];
	struct fragbuffer3D *FragBufferDone;		/* last pixel written in FragBuffer */
	struct fragbuffer3D *FragBufferMaxi;		/* dont fill FragBuffer beyond this limit ==> flush it*/
	UWORD Culling;
	UWORD FogMode;
	float FogZmin,FogZmax,FogDensity;
	UBYTE FogRGBA[4];
	ULONG FogRGBAs[FOGSIZE];
	WORD PolyPnb;
	union pixel3D PolyPix[MAXPOLY];
	struct point3D PolyP[MAXPOLY];
	struct point3D T1[MAXPOLY];
	struct point3D T2[MAXPOLY];
	void *firstST;
	UBYTE AliasedLines[4*MAXSCREEN*2];
	WORD Pxmin,Pxmax,Pymin,Pymax,Pzmin,Pzmax;		/* really updated region */
	WORD xUpdate,largeUpdate,yUpdate,highUpdate;	/* really updated region previous frame*/
	UWORD Tnb,DumpStage;
	ULONG DumpFnum,DumpPnum,DumpFnb,DumpPnb;		/* Faces & Points count. */
	struct  face3D *DumpF;
	struct point3D *DumpP;
	UBYTE ZMode;
	UBYTE BlendMode;
	UBYTE TexEnvMode;
	UBYTE UseGouraud;	
	UBYTE UseFog;
	UBYTE UseTex;
	UBYTE UseFunctionIn;
	UBYTE ColorChange;			/* V41: Soft3D can smartly desactivate the gouraud if all points got the same color */
	UBYTE ColorTransp;			/* V43: Soft3D detect if points got alpha value */
	struct rgbabuffer3D *Src;
	struct rgbabuffer3D *Dst;
	struct rgbabuffer3D *End;
	UBYTE EnvRGBA[4];
	LONG dmin; 
	struct BitMap *bm;
	void  *bmHandle; 				/* to directly write to bitmap */
	UWORD yoffset;
	struct RastPort rastport;
};
/*=================================================================*/
struct SOFT3D_mipmap{
	UBYTE *Tex8V[256];	/* adresse of the texture at this line */
	ULONG  Tex8U[256];
	ULONG  Tex8Vlow[256];
	ULONG  Tex8Ulow[256];
};
/*=================================================================*/
struct SOFT3D_texture{
	struct SOFT3D_mipmap MMs[12];
	UBYTE *pt;			/* original  data from 3Dprog */
	UBYTE *pt2;			/* converted data as RGB or RGBA */
	UBYTE *ptmm;		/* mipmaps as RGB or RGBA */
	UWORD large,high,bits;
	UBYTE MipMapped,BlendMode;
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
#define et  &&
#define ou  ||
#define AND &
#define OR  |
#define FLOOP(nbre) for(f=0;f<nbre;f++)
#define PLOOP(nbre) for(p=0;p<nbre;p++)
#define MLOOP(nbre) for(m=0;m<nbre;m++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define XLOOP(nbre) for(x=0;x<nbre;x++)
#define YLOOP(nbre) for(y=0;y<nbre;y++)
#define SWAP(x,y) {temp=x;x=y;y=temp;}
#define COPYRGBA(x,y)	*((ULONG *)(x))=*((ULONG *)(y));
#define NOTSAMERGBA(x,y)	(*((ULONG *)(x))!=*((ULONG *)(y)))
/* simpler W3D_GetState used internally */
#define StateON(s) ((context->state & s)!=0)
#define FREEPTR(ptr) {MYfree(ptr);ptr=NULL;}
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
#define  VARF(var) if(Wazp3D.DebugVal.ON)  {if(Wazp3D.DebugVar.ON) Libprintf(" " #var "="); pf(var); Libprintf("\n"); }
#define   VAL(var) {if(Wazp3D.DebugVal.ON) Libprintf(" [%ld]",var);}
#define ZZ LibAlert("ZZ stepping ...");
#define  ERROR(val,doc) if(error == val) if(Wazp3D.DebugVal.ON) {if(Wazp3D.DebugVar.ON) Libprintf(" Error="); Libprintf(#val); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WRETURN(error) return(PrintError(error));
#define QUERY(q,doc,s) if(query==q) if(Wazp3D.DebugVal.ON) {sup=s;  Libprintf(" " #q); if(Wazp3D.DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define WAZP3DFUNCTION(n) WAZP3D_Function(n);
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
#define WAZP3DFUNCTION(n) ;
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
/*==================================================================================*/
/* internal private functions */
void AntiAliasImage(void *image,UWORD large,UWORD high,UBYTE *AliasedLines);    
void BitmapPattern(void *image,UWORD large,UWORD high,UWORD bits,UBYTE color);    
void ClipPoly(struct SOFT3D_context *SC);    
void ConvertBitmap(ULONG format,UBYTE *pt1,UBYTE *pt2,UWORD high,UWORD large,ULONG offset1,ULONG offset2,UBYTE *palette);    
void CreateMipmaps(struct SOFT3D_texture *ST);    
void DoUpdate(W3D_Context *context);    
void DrawFragBuffer(struct SOFT3D_context *SC);    
void DrawLinePix(struct SOFT3D_context *SC);    
void DrawPointPix(struct SOFT3D_context *SC);    
void DrawSimplePix(struct SOFT3D_context *SC,register union pixel3D *P);    
void DrawPolyP(struct SOFT3D_context *SC);    
void DrawPolyPix(struct SOFT3D_context *SC);    
void DrawText(W3D_Context *context,UBYTE *text,WORD x,WORD y);    
void DrawTriP(struct SOFT3D_context *SC,register struct point3D *A,register struct point3D *B,register struct point3D *C);    
void DumpMem(UBYTE *pt,LONG nb);    
void Fill_BigTexPersp_Gouraud_Fog(struct SOFT3D_context *SC); 
void Fill_BigTexPersp_Fog(struct SOFT3D_context *SC);
void Fill_Flat(struct SOFT3D_context *SC);    
void Fill_Fog(struct SOFT3D_context *SC);    
void Fill_Gouraud(struct SOFT3D_context *SC);    
void Fill_Gouraud_Fog(struct SOFT3D_context *SC);    
void Fill_Tex(struct SOFT3D_context *SC);    
void Fill_Tex_Fog(struct SOFT3D_context *SC);    
void Fill_Tex_Gouraud(struct SOFT3D_context *SC);    
void Fill_Tex_Gouraud_Fog(struct SOFT3D_context *SC);    
void GetPoint(W3D_Context *context,ULONG i);    
void GetVertex(struct WAZP3D_context *WC,W3D_Vertex *V);    
void ImagePlot(struct SOFT3D_context *SC,UWORD x,UWORD y);    
void *Libmalloc(ULONG size );    
void *Libmemcpy(void *s1,void *s2,LONG n);    
UBYTE *Libstrcat(char *s1,const char *s2);    
UBYTE *Libstrcpy(char *s1,const char *s2);
void LibAlert(const char *t);    
void Libfree(void *p);    
void Libloadfile(UBYTE *filename,void *pt,ULONG size);    
ULONG LibMilliTimer(void);    
void Libprintf(const char *string, ...);    
void Libsavefile(UBYTE *filename,void *pt,ULONG size);    
void LibSettings(struct button3D *ButtonList,WORD ButtonCount);    
void Libsprintf(UBYTE *buffer,const char *string, ...);    
ULONG Libstrlen(const char *string);    
BOOL LockBM(struct SOFT3D_context *SC);    
void *MYmalloc(ULONG size,char *name);    
void MYfree(void *pt);    
void PixelsARGB(struct SOFT3D_context *SC);    
void PixelsBlend24(struct SOFT3D_context *SC);    
void PixelsBlend32(struct SOFT3D_context *SC);    
void PixelsDecal24(struct SOFT3D_context *SC);    
void PixelsDecal32(struct SOFT3D_context *SC);    
void PixelsInOutBGRA(struct SOFT3D_context *SC);    
void PixelsMedian24(struct SOFT3D_context *SC);    
void PixelsMedian32(struct SOFT3D_context *SC);    
void PixelsModulate24(struct SOFT3D_context *SC);    
void PixelsModulate32(struct SOFT3D_context *SC);    
void PixelsOne_One24(struct SOFT3D_context *SC);
void PixelsReplace24(struct SOFT3D_context *SC);    
void PixelsReplace32(struct SOFT3D_context *SC);    
void PixelsSrcAlpha_One32(struct SOFT3D_context *SC);    
void PixelsSrcAlpha_OneMinusSrcAlpha32(struct SOFT3D_context *SC);    
void PixelsSrcAlpha_OneMinusSrcAlpha32fast(struct SOFT3D_context *SC);    
void PixelsSrcAlpha_OneMinusSrcAlpha32perfect(struct SOFT3D_context *SC);    
void PrintAllFunctionsAdresses(void);    
void PrintContext(W3D_Context *C);    
void PrintDriver(W3D_Driver *D);    
void PrintME(struct memory3D *ME);    
void PrintP(struct point3D *P);    
void PrintPix(union pixel3D *Pix);    
void PrintRGBA(UBYTE *RGBA);    
void PrintST(struct SOFT3D_texture *ST);    
void PrintTexture(W3D_Texture *T);    
void PrintWT(struct WAZP3D_texture *WT);    
void ReduceBitmap(UBYTE *pt,UBYTE *pt2,WORD large,WORD high, WORD bits,WORD ratio);    
void SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor);
BOOL SetState(W3D_Context *context,ULONG state,BOOL set);    
void SetTexture(W3D_Context* context,W3D_Texture *texture);    
void SmoothTexture(void *image,UWORD large,UWORD high,UBYTE *AliasedLines,UBYTE bits);    
ULONG STACKReadPixelArray(APTR  destRect,UWORD  destX,UWORD  destY,UWORD  destMod,struct RastPort *  RastPort,UWORD  SrcX, UWORD  SrcY, UWORD  SizeX, UWORD  SizeY, UBYTE  DestFormat );    
void STACKWritePixelArray(APTR image,UWORD  SrcX, UWORD  SrcY, UWORD  SrcMod, struct RastPort *  RastPort, UWORD  DestX, UWORD  DestY, UWORD  SizeX, UWORD  SizeY, UBYTE  SrcFormat );    
void TextureAlphaUsage(struct SOFT3D_texture *ST);    
void UnLockBM(struct SOFT3D_context *SC);    
void UVtoRGBA(struct SOFT3D_texture *ST,float u,float v,UBYTE *RGBA);    
void Ztest_zalways(struct SOFT3D_context *SC);    
void Ztest_zalways_update(struct SOFT3D_context *SC);    
void Ztest_zequal(struct SOFT3D_context *SC);    
void Ztest_zequal_update(struct SOFT3D_context *SC);    
void Ztest_zgequal(struct SOFT3D_context *SC);    
void Ztest_zgequal_update(struct SOFT3D_context *SC);    
void Ztest_zgreater(struct SOFT3D_context *SC);    
void Ztest_zgreater_update(struct SOFT3D_context *SC);    
void Ztest_zlequal(struct SOFT3D_context *SC);    
void Ztest_zlequal_update(struct SOFT3D_context *SC);    
void Ztest_zless(struct SOFT3D_context *SC);    
void Ztest_zless_update(struct SOFT3D_context *SC);    
void Ztest_znequal(struct SOFT3D_context *SC);    
void Ztest_znequal_update(struct SOFT3D_context *SC);    
void Ztest_znever_update(struct SOFT3D_context *SC);    
void Ztest_znotequal(struct SOFT3D_context *SC);    
void Ztest_znotequal_update(struct SOFT3D_context *SC); 
void ZbufferCheck(W3D_Context *context);
/*==========================================================================*/
/* semi-public functions : SOFT3D is the rasterizer*/
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,void *bm);  
void  SOFT3D_ClearImage(struct SOFT3D_context *SC,WORD x,WORD y,WORD large,WORD high);    
void  SOFT3D_ClearZBuffer(struct SOFT3D_context *SC,float fz);    
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,BOOL MipMapped);  
BOOL  SOFT3D_DoUpdate(struct SOFT3D_context *SC);	
void  SOFT3D_DrawPrimitive(struct SOFT3D_context *SC,struct point3D *P,LONG Pnb,ULONG primitive);    
void  SOFT3D_End(struct SOFT3D_context *SC);    
void  SOFT3D_Fog(struct SOFT3D_context *SC,WORD FogMode,float FogZmin,float FogZmax,float FogDensity,UBYTE *FogRGBA);    
void  SOFT3D_FreeTexture(struct SOFT3D_context *SC,void  *texture);
void  SOFT3D_ReadZSpan(struct SOFT3D_context *SC, ULONG x, ULONG y,ULONG n, W3D_Double *z);
void  SOFT3D_ReallocImage(struct SOFT3D_context *SC,WORD large,WORD high);
void  SOFT3D_SetBackColor(struct SOFT3D_context *SC,UBYTE  *RGBA);    
void  SOFT3D_SetBitmap(struct SOFT3D_context *SC,void  *bm,WORD x,WORD y,WORD large,WORD high);    
void  SOFT3D_SetClipping(struct SOFT3D_context *SC,float xmin,float xmax,float ymin,float ymax,float zmin,float zmax);    
void  SOFT3D_SetCulling(struct SOFT3D_context *SC,UWORD Culling);    
void  SOFT3D_SetCurrentColor(struct SOFT3D_context *SC,UBYTE  *RGBA);    
void  SOFT3D_SetDrawFunctions(struct SOFT3D_context *SC);    
void  SOFT3D_SetDrawStates(struct SOFT3D_context *SC,struct SOFT3D_texture *ST,UBYTE ZMode,UBYTE BlendMode,UBYTE TexEnvMode,UBYTE UseGouraud,UBYTE UseFog);    
void  SOFT3D_SetEnvColor(struct SOFT3D_context *SC,UBYTE  *RGBA);    
void  SOFT3D_SetImage(struct SOFT3D_context *SC,WORD large,WORD high,ULONG *Image32);
void  SOFT3D_SetPointSize(struct SOFT3D_context *SC,UWORD PointSize);    
void  SOFT3D_SetZbuffer(struct SOFT3D_context *SC,ZBUFF *Zbuffer);    
void  SOFT3D_WriteZSpan(struct SOFT3D_context *SC, ULONG x, ULONG y,ULONG n, W3D_Double *zD, UBYTE *mask);    
void  WAZP3D_Settings();    
/*==========================================================================*/
void pf(float x)		/* emulate printf() from a float %f */
{
LONG high,low;

	high=(LONG)(x);
	x=(x-(float)high);
	if(x<0.0) x=-x;
	low =(LONG)(1000000.0*x);
	Libprintf("%ld.%ld ",high,low);

}
/*==========================================================================*/
void ph(ULONG x)		/* emulate printf() as hexa */
{
LONG high,low;
WORD n;
UBYTE name[9];
UBYTE hexa[16] = {"0123456789ABCDEF"};

	name[8]=0;
	NLOOP(8)
	{
	high=x>>4;
	low =x-(high<<4);
	x=high;
	name[7-n]=hexa[low];
	}
	NLOOP(7)
	{
	if(name[n]!='0')
		{Libprintf("%s",&name[n]);return;}
	}
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
	ButtonCount=((LONG)&Wazp3D.DebugMemUsage -(LONG)&Wazp3D.HardwareLie)/sizeof(struct button3D)+1;
#else
	ButtonCount=((LONG)&Wazp3D.DebugWazp3D  - (LONG)&Wazp3D.HardwareLie)/sizeof(struct button3D);
#endif
	Wazp3D.DebugWazp3D.ON=LibDebug;
	LibSettings(&Wazp3D.HardwareLie,ButtonCount);
	LibDebug=Wazp3D.DebugWazp3D.ON;
}
/*=================================================================*/
void WAZP3D_Function(UBYTE n)
{
	if(Wazp3D.FunctionCallsAll!= 0xFFFFFFFF)
		Wazp3D.FunctionCallsAll++;
	if(Wazp3D.FunctionCalls[n]!= 0xFFFFFFFF)
		Wazp3D.FunctionCalls[n]++;

#ifdef WAZP3DDEBUG
	if(!Wazp3D.DebugWazp3D.ON) return;
	if(Wazp3D.DebugCalls.ON)
		Libprintf("[%ld][%ld]",Wazp3D.FunctionCallsAll,Wazp3D.FunctionCalls[n]);
	if(Wazp3D.DebugFunction.ON)
		{
		Libprintf("%s\n" ,Wazp3D.FunctionName[n]);
	if(Wazp3D.DebugAsJSR.ON)
		{
		Libprintf("                    JSR            -$");
		ph(30+6*n);
		Libprintf("(A6)\n");
		}
		}

	if(Wazp3D.NextStepFunction < Wazp3D.FunctionCallsAll )
	if(Wazp3D.StepFunction50.ON)
		{
		LibAlert("Step 50 functions");
		Wazp3D.NextStepFunction+=50;
		}
	if(Wazp3D.FunctionCallsAll==Wazp3D.CrashFunctionCall)
		Wazp3D.StepFunction.ON=Wazp3D.StepSOFT3D.ON=TRUE;

	if(Wazp3D.StepFunction.ON)
		LibAlert(Wazp3D.FunctionName[n]);

#endif
}
/*=================================================================*/
void CopyPix(union pixel3D *P1,union pixel3D *P2)
{
	P1->L.x=P2->L.x; 
	P1->L.y=P2->L.y; 
	P1->L.z=P2->L.z; 
	P1->L.w=P2->L.w; 
	P1->L.u=P2->L.u; 
	P1->L.v=P2->L.v; 
	P1->L.R=P2->L.R; 
	P1->L.G=P2->L.G; 
	P1->L.B=P2->L.B; 
	P1->L.A=P2->L.A; 
	P1->L.F=P2->L.F; 
}
/*=================================================================*/
void CopyP(struct point3D *P1,struct point3D *P2)
{
	P1->x=P2->x; 
	P1->y=P2->y; 
	P1->z=P2->z; 
	P1->w=P2->w; 
	P1->u=P2->u; 
	P1->v=P2->v; 
	COPYRGBA(P1->RGBA,P2->RGBA);
}
/*==========================================================================*/
void ColorToRGBA(UBYTE *RGBA,float r,float g,float b,float a)
{
	RGBA[0]=(UBYTE)(r*256.0); 
	if(1.0<=r)	RGBA[0]=255;
	if(r<=0.0)	RGBA[0]=0;
	RGBA[1]=(UBYTE)(g*256.0); 
	if(1.0<=g)	RGBA[1]=255;
	if(g<=0.0)	RGBA[1]=0;
	RGBA[2]=(UBYTE)(b*256.0); 
	if(1.0<=b)	RGBA[2]=255;
	if(b<=0.0)	RGBA[2]=0;
	RGBA[3]=(UBYTE)(a*256.0); 
	if(1.0<=a)	RGBA[3]=255;
	if(a<=0.0)	RGBA[3]=0;
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
/*	Libprintf(" P XYZW, %ld, %ld, %ld, %ld, UV, %ld, %ld,",(WORD)P->x,(WORD)P->y,(WORD)(1000.0*P->z),(WORD)(1000.0*P->w),(WORD)(P->u),(WORD)(P->v));*/

	Libprintf(" P XYZ W UV ");
	pf(P->x);
	pf(P->y);
	pf(P->z);
	pf(P->w);
	pf(P->u);
	pf(P->v);
	if (Wazp3D.DebugVal.ON)
		PrintRGBA((UBYTE *)&P->RGBA);
	else
		Libprintf("\n");
}
/*=================================================================*/
void PrintP2(struct point3D *P)
{
	Libprintf("ClipXYZW; %ld; %ld; %ld; %ld; UV; %ld; %ld\n",(WORD)P->x,(WORD)P->y,(WORD)(1000.0*P->z),(WORD)(1000.0*P->w),(WORD)(P->u),(WORD)(P->v));
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
	if (!Wazp3D.DebugST.ON) return;
	Libprintf("SOFT3D_texture(%ld) %s  pt %ld pt2 %ld NextST(%ld) MipMapped %ld BlendMode:%lc \n",ST,ST->name,ST->pt,ST->pt2,ST->nextST,ST->MipMapped,ST->BlendMode);
}
/*=================================================================*/
void PrintPix(union pixel3D *Pix)
{
	if (!Wazp3D.DebugPoint.ON)  return;
	if (!Wazp3D.DebugSOFT3D.ON) return;
	Libprintf(" Pix XY %ld %ld \tUV %ld %ld ",Pix->W.x,Pix->W.y,Pix->W.u,Pix->W.v);
	Libprintf("RGBA %ld %ld %ld %ld large %ld",Pix->W.R,Pix->W.G,Pix->W.B,Pix->W.A,Pix->W.large);
	Libprintf(" ZWF ");  pf(Pix->L.z); pf(Pix->L.w); pf(Pix->W.F);Libprintf("\n"); 

}
/*==========================================================================*/
void PrintAllT(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;
WORD Ntexture;

	if (!Wazp3D.DebugWT.ON) return;
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
#define PrintP2(x) ;
#define PrintWT(x) ;
#define PrintST(x) ;
#define PrintPix(x) ;
#define PrintAllT(x) ;
#endif

/*=============================================================*/
void SOFT3D_SetImage(struct SOFT3D_context *SC,WORD large,WORD high,ULONG *Image32)
{
WORD y;

	if(SC==NULL) return;
	if(Image32==NULL) return;
SFUNCTION(SOFT3D_SetImage)

	SC->large  =large;
	SC->high   =high;
	SC->Image32=Image32;

	SC->Pxmin=0;
	SC->Pymin=0;
	SC->Pxmax=large-1;
	SC->Pymax=high -1;

	YLOOP(high)
		{
		SC->edge1[y].L.Image32Y   =SC->edge2[y].L.Image32Y  =SC->edgeM[y].L.Image32Y   =Image32;
		SC->edge1[y].L.y=0;SC->edge1[y].W.y=y;
		SC->edge2[y].L.y=0;SC->edge2[y].W.y=y;
		SC->edgeM[y].L.y=0;SC->edgeM[y].W.y=y;
		Image32+=large;
		}

}
/*=============================================================*/
void SOFT3D_ReallocImage(struct SOFT3D_context *SC,WORD large,WORD high)
{
	FREEPTR(SC->Image32);
	SC->Image32=MYmalloc(SC->large*SC->high*32/8,"SOFT3D_Image");
	SOFT3D_SetImage(SC,large,high,SC->Image32);
}
/*=============================================================*/
struct SOFT3D_context *SOFT3D_Start(WORD large,WORD high,void *bm)
{
register union oper3D Oper;
struct SOFT3D_context *SC;
UWORD x,y;
ULONG n;
UBYTE  FogRGBA[]={255,255,255,255}; /* default white fog */
UBYTE BackRGBA[]={0,0,0,0};		/* default black background */
UBYTE ZMode,FillMode;
float m=0.0;	/*for debug: set a margin for clipper so show if 3Dprog never setted scissor */

SFUNCTION(SOFT3D_Start)
	SC=MYmalloc(sizeof(struct SOFT3D_context),"SOFT3D_context");
	if(SC==NULL) return(NULL);
	InitRastPort(&SC->rastport);

	if(!Wazp3D.DirectBitmap.ON)	/* then need an RGBA buffer the "Image" else use the bitmap*/
	{
	SC->Image32=MYmalloc(large*high*32/8,"SOFT3D_Image");
	if(SC->Image32==NULL) 
			{MYfree(SC); return(NULL);}
	}

	SOFT3D_SetBitmap(SC,bm,0,0,large,high);

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

	SC->FragBufferDone=SC->FragBuffer;
	SC->FragBufferMaxi=SC->FragBuffer + (FRAGBUFFERSIZE-MAXSCREEN);
	NLOOP(FRAGBUFFERSIZE)
	{
	SC->FragBuffer[n].Color8 =(UBYTE *)&SC->FragBuffer[n].ColorRGBA[0];
	SC->FragBuffer[n].Buffer8=(UBYTE *)&SC->FragBuffer[n].BufferRGBA[0];
	SC->FragBuffer[n].Fog8   =(UBYTE *)&SC->FragBuffer[n].FogRGBA[0];
	}

	ZMode=W3D_Z_NEVER;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_znever_update;  /* same as Ztest_znever */
	ZMode=W3D_Z_LESS;	 	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zless;  
	ZMode=W3D_Z_GEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zgequal;  
	ZMode=W3D_Z_LEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zlequal;  
	ZMode=W3D_Z_GREATER;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zgreater;  
	ZMode=W3D_Z_NOTEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_znotequal;  
	ZMode=W3D_Z_EQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zequal;  
	ZMode=W3D_Z_ALWAYS;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zalways;
   
	ZMode=8+W3D_Z_NEVER;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_znever_update;  
	ZMode=8+W3D_Z_LESS;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zless_update;  
	ZMode=8+W3D_Z_GEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zgequal_update;  
	ZMode=8+W3D_Z_LEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zlequal_update;  
	ZMode=8+W3D_Z_GREATER;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zgreater_update;  
	ZMode=8+W3D_Z_NOTEQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_znotequal_update;  
	ZMode=8+W3D_Z_EQUAL;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zequal_update;  
	ZMode=8+W3D_Z_ALWAYS;	Wazp3D.FunctionsZtest[ZMode]=(HOOKEDFUNCTION)Ztest_zalways_update;  

	FillMode=(FALSE*4 + TRUE*2  + TRUE*1  );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Gouraud_Fog;
	FillMode=(FALSE*4 + TRUE*2  + FALSE*1 );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Gouraud;
	FillMode=(FALSE*4 + FALSE*2 + TRUE*1  );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Fog;
	FillMode=(FALSE*4 + FALSE*2 + FALSE*1 );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Flat;

	FillMode=(TRUE*4 + TRUE*2  + TRUE*1  );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Tex_Gouraud_Fog;
	FillMode=(TRUE*4 + TRUE*2  + FALSE*1 );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Tex_Gouraud;
	FillMode=(TRUE*4 + FALSE*2 + TRUE*1  );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Tex_Fog;
	FillMode=(TRUE*4 + FALSE*2 + FALSE*1 );	Wazp3D.FunctionsFill[FillMode]=(HOOKEDFUNCTION)Fill_Tex;

	SOFT3D_SetClipping(SC,m,(float)(large-1)-m,m,(float)(high-1)-m,MINZ,MAXZ);
	SOFT3D_Fog(SC,0,MINZ,MAXZ,0.0,FogRGBA);					/* default no fog */
	SOFT3D_SetDrawStates(SC,NULL,W3D_Z_ALWAYS,'R','0',FALSE,FALSE);	/* default notex nofog nozbuffer just color */
	SOFT3D_SetPointSize(SC,1);							/* default simpler point = 1 pixel */
	SOFT3D_SetBackColor(SC,BackRGBA);

	SC->Tnb=0;			/* texture number */
	SC->DumpStage=0;
	return(SC);
}
/*=============================================================*/
void SOFT3D_SetZbuffer(struct SOFT3D_context *SC,ZBUFF *Zbuffer)
{
WORD y;

SFUNCTION(SOFT3D_SetZbuffer)
	SC->Zbuffer=Zbuffer;
VAR(Zbuffer)
VAR(SC->large)
VAR(SC->high)

	if(Zbuffer!=NULL)
	YLOOP(SC->high)
		{ SC->edge1[y].L.ZbufferY = SC->edge2[y].L.ZbufferY =SC->edgeM[y].L.ZbufferY =Zbuffer;Zbuffer+=SC->large;}
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
void SOFT3D_SetBackColor(struct SOFT3D_context *SC,UBYTE  *RGBA)
{
SREM(SOFT3D_SetBackColor)
	PrintRGBA(RGBA);
	COPYRGBA(SC->BackRGBA,RGBA);
}
/*=============================================================*/
void SOFT3D_SetEnvColor(struct SOFT3D_context *SC,UBYTE  *RGBA)
{
SREM(SOFT3D_SetEnvColor)
	PrintRGBA(RGBA);
	COPYRGBA(SC->EnvRGBA,RGBA);
}
/*=============================================================*/
void SOFT3D_SetCurrentColor(struct SOFT3D_context *SC,UBYTE  *RGBA)
{
SREM(SOFT3D_SetCurrentColor)
	PrintRGBA(RGBA);
	COPYRGBA(SC->CurrentRGBA,RGBA);
}
/*=============================================================*/
void SOFT3D_ClearImage(struct SOFT3D_context *SC,WORD x,WORD y,WORD large,WORD high)
{
ULONG *ptRGBA32=(ULONG *)SC->BackRGBA;
register ULONG RGBA32=ptRGBA32[0];
register ULONG *Image32;
register LONG  size;
register LONG  n;

SFUNCTION(SOFT3D_ClearImage)
	if(SC->Image32==NULL) return;
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
void SOFT3D_ClearZBuffer(struct SOFT3D_context *SC,float fz)
{
register ZBUFF *Zbuffer=SC->Zbuffer;
register ULONG size=SC->high*SC->large/8;
register ULONG n;
register ZBUFF z;
register float zresize=ZRESIZE;

SFUNCTION(SOFT3D_ClearZBuffer)
	if(Zbuffer==NULL) return;
	if (z < MINZ)	z=MINZ;
	if (MAXZ < z)	z=MAXZ;
	z=fz*zresize;
	NLOOP(size)
	{
	Zbuffer[0]=z;	Zbuffer[1]=z;	Zbuffer[2]=z; Zbuffer[3]=z;
	Zbuffer[4]=z;	Zbuffer[5]=z;	Zbuffer[6]=z; Zbuffer[7]=z;
	Zbuffer+=8;
	}
}
/*==========================================================================*/
void SOFT3D_ReadZSpan(struct SOFT3D_context *SC, ULONG x, ULONG y,ULONG n, W3D_Double *z)
{
register ZBUFF *Zbuffer;
register float fz;
register float zresize=ZRESIZE;

SFUNCTION(SOFT3D_ReadZSpan)
	Zbuffer=SC->edge1[y].L.ZbufferY + x;
	XLOOP(n)
		{
		fz=(float)Zbuffer[x];
		fz=fz/zresize;
		z[x]=(W3D_Double)fz;
		}

}
/*==========================================================================*/
void SOFT3D_WriteZSpan(struct SOFT3D_context *SC, ULONG x, ULONG y,ULONG n, W3D_Double *zD, UBYTE *mask)
{
register ZBUFF *Zbuffer;
register float fz;
register float zresize=ZRESIZE;

SFUNCTION(SOFT3D_WriteZSpan)
	Zbuffer=SC->edge1[y].L.ZbufferY + x;
	XLOOP(n)
		if(mask[x]==1)
			{
			fz=zD[x];
			if (fz < MINZ)	fz=MINZ;
			if (MAXZ < fz)	fz=MAXZ;
			Zbuffer[x]=fz*zresize;
			}

}
/*=============================================================*/
void SOFT3D_End(struct SOFT3D_context *SC)
{
SFUNCTION(SOFT3D_End)
	if(SC==NULL) return;
	if(!Wazp3D.DirectBitmap.ON)
		FREEPTR(SC->Image32);
}
/*=============================================================*/
#define ADD8(a1,b1,dst)              Oper1.B.b=b1;  Oper1.B.a=a1;  dst=Add8[Oper1.L.Index];
#define MUL8(a1,b1,dst)              Oper1.B.b=b1;  Oper1.B.a=a1;  dst=Mul8[Oper1.L.Index];
#define NEXTADDMUL8(b1,b2,dst)       Oper1.B.b=b1;  Oper2.B.b=b2;  dst=Mul8[Oper1.L.Index]+Mul8[Oper2.L.Index];
#define FULLADDMUL8(b1,a1,b2,a2,dst) Oper1.B.a=a1;  Oper2.B.a=a2;  NEXTADDMUL8(b1,b2,dst)
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
#define CT1R Frag[0].Tex8[0]
#define CT1G Frag[0].Tex8[1]
#define CT1B Frag[0].Tex8[2]
#define AT1  Frag[0].Tex8[3]
#define CF1R Frag[0].ColorRGBA[0]
#define CF1G Frag[0].ColorRGBA[1]
#define CF1B Frag[0].ColorRGBA[2]
#define AF1  Frag[0].ColorRGBA[3]

#define CT2R Frag[1].Tex8[0]
#define CT2G Frag[1].Tex8[1]
#define CT2B Frag[1].Tex8[2]
#define AT2  Frag[1].Tex8[3]
#define CF2R Frag[1].ColorRGBA[0]
#define CF2G Frag[1].ColorRGBA[1]
#define CF2B Frag[1].ColorRGBA[2]
#define AF2  Frag[1].ColorRGBA[3]

#define CCR SC->EnvRGBA[0]
#define CCG SC->EnvRGBA[1]
#define CCB SC->EnvRGBA[2]

#define AT Ct[3]
#define AF Cf[3]
#define ONE 255
/*=============================================================*/
void PixelsTex32ToImage(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsTex32ToImage)
	while(0<size--)
	{
	COPYRGBA(Frag[0].Image8,Frag[0].Tex8);
	COPYRGBA(Frag[1].Image8,Frag[1].Tex8);
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsTex24ToImage(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsTex24ToImage)
	while(0<size--)
	{
	Frag[0].Image8[0]=Frag[0].Tex8[0];
	Frag[0].Image8[1]=Frag[0].Tex8[1];
	Frag[0].Image8[2]=Frag[0].Tex8[2];
	Frag[0].Image8[3]=ONE;

	Frag[1].Image8[0]=Frag[1].Tex8[0];
	Frag[1].Image8[1]=Frag[1].Tex8[1];
	Frag[1].Image8[2]=Frag[1].Tex8[2];
	Frag[1].Image8[3]=ONE;
	Frag+=2;
	}
}

/*=============================================================*/
void PixelsColorToImage(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsColorToImage)
	while(0<size--)
	{
	COPYRGBA(Frag[0].Image8,Frag[0].ColorRGBA);
	COPYRGBA(Frag[1].Image8,Frag[1].ColorRGBA);
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsTex32ToBuffer(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsTex32ToBuffer)
	while(0<size--)
	{
	COPYRGBA(Frag[0].Buffer8,Frag[0].Tex8);
	COPYRGBA(Frag[1].Buffer8,Frag[1].Tex8);
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsTex24ToBuffer(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsTex24ToBuffer)
	while(0<size--)
	{
	Frag[0].Buffer8[0]=Frag[0].Tex8[0];
	Frag[0].Buffer8[1]=Frag[0].Tex8[1];
	Frag[0].Buffer8[2]=Frag[0].Tex8[2];
	Frag[0].Buffer8[3]=ONE;

	Frag[1].Buffer8[0]=Frag[1].Tex8[0];
	Frag[1].Buffer8[1]=Frag[1].Tex8[1];
	Frag[1].Buffer8[2]=Frag[1].Tex8[2];
	Frag[1].Buffer8[3]=ONE;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsColorToBuffer(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsColorToBuffer)
	while(0<size--)
	{
	COPYRGBA(Frag[0].Buffer8,Frag[0].ColorRGBA);
	COPYRGBA(Frag[1].Buffer8,Frag[1].ColorRGBA);
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsReplace24(struct SOFT3D_context *SC)
{
/* replace color=tex  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsReplace24)
	while(0<size--)
	{
	Frag[0].ColorRGBA[0]=Frag[0].Tex8[0];
	Frag[0].ColorRGBA[1]=Frag[0].Tex8[1];
	Frag[0].ColorRGBA[2]=Frag[0].Tex8[2];
	Frag[0].ColorRGBA[3]=ONE;

	Frag[1].ColorRGBA[0]=Frag[1].Tex8[0];
	Frag[1].ColorRGBA[1]=Frag[1].Tex8[1];
	Frag[1].ColorRGBA[2]=Frag[1].Tex8[2];
	Frag[1].ColorRGBA[3]=ONE;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsReplace32(struct SOFT3D_context *SC)
{
/* replace color=tex  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(PixelsReplace32)
	while(0<size--)
	{
	COPYRGBA(Frag[0].ColorRGBA,Frag[0].Tex8);
	COPYRGBA(Frag[1].ColorRGBA,Frag[1].Tex8);
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsModulate24(struct SOFT3D_context *SC)
{
/* v40: modulate src & dst  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsModulate24)
	Oper1.L.Index=0;
	while(0<size--)
	{
	MUL8(CT1R,CF1R,CF1R)
	MUL8(CT1G,CF1G,CF1G)
	MUL8(CT1B,CF1B,CF1B)

	MUL8(CT2R,CF2R,CF2R)
	MUL8(CT2G,CF2G,CF2G)
	MUL8(CT2B,CF2B,CF2B)
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsModulate32(struct SOFT3D_context *SC)
{
/* v40: modulate src & dst  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsModulate32)
	Oper1.L.Index=0;
	while(0<size--)
	{
	MUL8(CT1R,CF1R,CF1R)
	MUL8(CT1G,CF1G,CF1G)
	MUL8(CT1B,CF1B,CF1B)
	MUL8(AT1,AF1,AF1)

	MUL8(CT2R,CF2R,CF2R)
	MUL8(CT2G,CF2G,CF2G)
	MUL8(CT2B,CF2B,CF2B)
	MUL8(AT2,AF2,AF2)
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsDecal24(struct SOFT3D_context *SC)
{
/* v40: decal src & dst  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

SREM(Pixels24DEC)
	while(0<size--)
	{
	CF1R=CT1R;
	CF1G=CT1G;
	CF1B=CT1B;
	AF1 =ONE;

	CF2R=CT2R;
	CF2G=CT2G;
	CF2B=CT2B;
	AF2 =ONE;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsDecal32(struct SOFT3D_context *SC)
{
/* v40: decal src & dst  (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;

SREM(PixelsDecal32)
	Oper1.L.Index=Oper2.L.Index=0;
	while(0<size--)
	{
	FULLADDMUL8(CT1R,AT1,CF1R,ONE-AT1,CF1R)
	NEXTADDMUL8(CT1G,CF1G,CF1G)
	NEXTADDMUL8(CT1B,CF1B,CF1B)
/*	AF1=AF1; */		/* alpha from color */

	FULLADDMUL8(CT2R,AT2,CF2R,ONE-AT2,CF2R)
	NEXTADDMUL8(CT2G,CF2G,CF2G)
	NEXTADDMUL8(CT2B,CF2B,CF2B)
/*	AF2=AF2; */		/* alpha from color */
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsBlend24(struct SOFT3D_context *SC)
{
/* v40: blend src & dst  & env (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;

SREM(PixelsBlend24)
	Oper1.L.Index=Oper2.L.Index=0;
	while(0<size--)
	{
	FULLADDMUL8(CF1R,ONE-CT1R,CCR,CT1R,CF1R)
	FULLADDMUL8(CF1G,ONE-CT1G,CCG,CT1G,CF1G)
	FULLADDMUL8(CF1B,ONE-CT1B,CCB,CT1B,CF1B)

	FULLADDMUL8(CF2R,ONE-CT2R,CCR,CT2R,CF2R)
	FULLADDMUL8(CF2G,ONE-CT2G,CCG,CT2G,CF2G)
	FULLADDMUL8(CF2B,ONE-CT2B,CCB,CT2B,CF2B)
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsBlend32(struct SOFT3D_context *SC)
{
/* v40: blend src & dst  & env (always) */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;
register union oper3D Oper2;

SREM(PixelsBlend32)
	Oper1.L.Index=Oper2.L.Index=0;
	while(0<size--)
	{
	FULLADDMUL8(CF1R,ONE-CT1R,CCR,CT1R,CF1R)
	FULLADDMUL8(CF1G,ONE-CT1G,CCG,CT1G,CF1G)
	FULLADDMUL8(CF1B,ONE-CT1B,CCB,CT1B,CF1B)
	MUL8(AF1,AT1,AF1)

	FULLADDMUL8(CF2R,ONE-CT2R,CCR,CT2R,CF2R)
	FULLADDMUL8(CF2G,ONE-CT2G,CCG,CT2G,CF2G)
	FULLADDMUL8(CF2B,ONE-CT2B,CCR,CT2B,CF2B)
	MUL8(AF2,AT2,AF2)
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsMedian24(struct SOFT3D_context *SC)
{
/* make median value with src & dst  (always)*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register ULONG  one=1;

SREM(PixelsMedian24)
	while(0<size--)
	{
	CF1R= ( (ULONG) CT1R + (ULONG)CF1R )>>one;
	CF1G= ( (ULONG) CT1G + (ULONG)CF1G )>>one;
	CF1B= ( (ULONG) CT1B + (ULONG)CF1B )>>one;

	CF2R= ( (ULONG) CT2R + (ULONG)CF2R )>>one;
	CF2G= ( (ULONG) CT2G + (ULONG)CF2G )>>one;
	CF2B= ( (ULONG) CT2B + (ULONG)CF2B )>>one;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsMedian32(struct SOFT3D_context *SC)
{
/* make median value with src & dst  (always)*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register ULONG one=1;

SREM(PixelsMedian32)
	while(0<size--)
	{
	CF1R= ( (ULONG) CT1R + (ULONG)CF1R )>>one;
	CF1G= ( (ULONG) CT1G + (ULONG)CF1G )>>one;
	CF1B= ( (ULONG) CT1B + (ULONG)CF1B )>>one;
	AF1 = ( (ULONG) AT1  + (ULONG)AF1  )>>one;
	if(AT1 < MINALPHA) AF1=0;

	CF2R= ( (ULONG) CT2R + (ULONG)CF2R )>>one;
	CF2G= ( (ULONG) CT2G + (ULONG)CF2G )>>one;
	CF2B= ( (ULONG) CT2B + (ULONG)CF2B )>>one;
	AF2 = ( (ULONG) AT2  + (ULONG)AF2  )>>one;
	if(AT2 < MINALPHA) AF2=0;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsFogToImage(struct SOFT3D_context *SC)
{
/* blend source & dest (always) with a fog color already weigthed to fog A	*/
/* FogRGBA[3] is set to background transparency against fog	level			*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsFogToImage)
	Oper1.L.Index=0;
	while(0<size--)
	{
	Oper1.B.a= Frag[0].FogRGBA[3];	
	Oper1.B.b= Frag[0].Image8[0];	Frag[0].Image8[0]	=Frag[0].FogRGBA[0]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[0].Image8[1];	Frag[0].Image8[1]	=Frag[0].FogRGBA[1]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[0].Image8[2];	Frag[0].Image8[2]	=Frag[0].FogRGBA[2]+Mul8[Oper1.L.Index];

	Oper1.B.a= Frag[1].FogRGBA[3];	
	Oper1.B.b= Frag[1].Image8[0];	Frag[1].Image8[0]	=Frag[1].FogRGBA[0]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[1].Image8[1];	Frag[1].Image8[1]	=Frag[1].FogRGBA[1]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[1].Image8[2];	Frag[1].Image8[2]	=Frag[1].FogRGBA[2]+Mul8[Oper1.L.Index];
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsFogToBuffer(struct SOFT3D_context *SC)
{
/* blend source & dest (always) with a fog color already weigthed to fog A	*/
/* FogRGBA[3] is set to background transparency against fog	level			*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsFogToBuffer)
	Oper1.L.Index=0;
	while(0<size--)
	{
	Oper1.B.a= Frag[0].FogRGBA[3];	
	Oper1.B.b= Frag[0].BufferRGBA[0];	Frag[0].BufferRGBA[0]=Frag[0].FogRGBA[0]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[0].BufferRGBA[1];	Frag[0].BufferRGBA[1]=Frag[0].FogRGBA[1]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[0].BufferRGBA[2];	Frag[0].BufferRGBA[2]=Frag[0].FogRGBA[2]+Mul8[Oper1.L.Index];

	Oper1.B.a= Frag[1].FogRGBA[3];	
	Oper1.B.b= Frag[1].BufferRGBA[0];	Frag[1].BufferRGBA[0]=Frag[1].FogRGBA[0]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[1].BufferRGBA[1];	Frag[1].BufferRGBA[1]=Frag[1].FogRGBA[1]+Mul8[Oper1.L.Index];
	Oper1.B.b= Frag[1].BufferRGBA[2];	Frag[1].BufferRGBA[2]=Frag[1].FogRGBA[2]+Mul8[Oper1.L.Index];
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsSepiaToImage(struct SOFT3D_context *SC)
{
/* colorize in red the pixels 	*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UWORD r;

SREM(PixelsSepiaToImage)
	while(0<size--)
	{
	r=Frag[0].Image8[0]+Frag[0].Image8[1]+Frag[0].Image8[2];
	Frag[0].Image8[0]=255;
	if(r<=255*2)
		Frag[0].Image8[0]=r/2;
	Frag[0].Image8[1]=r/4;
	Frag[0].Image8[2]=r/6;

	r=Frag[1].Image8[0]+Frag[1].Image8[1]+Frag[1].Image8[2];
	Frag[1].Image8[0]=255;
	if(r<=255*2)
		Frag[1].Image8[0]=r/2;
	Frag[1].Image8[1]=r/4;
	Frag[1].Image8[2]=r/6;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsSepiaToBuffer(struct SOFT3D_context *SC)
{
/* colorize in red the pixels 	*/
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;
register UWORD r;

SREM(PixelsSepiaToBuffer)
	while(0<size--)
	{
	r=Frag[0].BufferRGBA[0]+Frag[0].BufferRGBA[1]+Frag[0].BufferRGBA[2];
	Frag[0].BufferRGBA[0]=255;
	if(r<=255*2)
		Frag[0].BufferRGBA[0]=r/2;
	Frag[0].BufferRGBA[1]=r/4;
	Frag[0].BufferRGBA[2]=r/6;

	r=Frag[1].BufferRGBA[0]+Frag[1].BufferRGBA[1]+Frag[1].BufferRGBA[2];
	Frag[1].BufferRGBA[0]=255;
	if(r<=255*2)
		Frag[1].BufferRGBA[0]=r/2;
	Frag[1].BufferRGBA[1]=r/4;
	Frag[1].BufferRGBA[2]=r/6;
	Frag+=2;
	}
}
/*=============================================================*/
void PixelsSrcAlpha_OneMinusSrcAlpha32perfect(struct SOFT3D_context *SC)
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

SREM(PixelsSrcAlpha_OneMinusSrcAlpha32perfect)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Ct[0],AT,Cf[0],ONE-AT,Cf[0])
	NEXTADDMUL8(Ct[1],Cf[1],Cf[1])
	NEXTADDMUL8(Ct[2],Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void PixelsSrcAlpha_OneMinusSrcAlpha32(struct SOFT3D_context *SC)
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
		{ PixelsSrcAlpha_OneMinusSrcAlpha32perfect(SC); return; }

SREM(PixelsSrcAlpha_OneMinusSrcAlpha32)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA;  Ct=Src->RGBA;
	if (AT > MINALPHA)				/* if source visible ? */
	{
		if(AT < MAXALPHA)				/* if source not solid ? */
		{
		FULLADDMUL8(Ct[0],AT,Cf[0],ONE-AT,Cf[0])
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
void PixelsSrcAlpha_OneMinusSrcAlpha32fast(struct SOFT3D_context *SC)
{
/* copy source to dest (if source not transparent)*/
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;

SREM(PixelsSrcAlpha_OneMinusSrcAlpha32fast)
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	if (AT > MINALPHA)					/* if source visible ? */
		COPYRGBA(Cf,Ct);
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void PixelsSrcAlpha_One32(struct SOFT3D_context *SC)
{
/* v45: for glxcess :-) */
register struct rgbabuffer3D* Src=SC->Src;
register struct rgbabuffer3D* Dst=SC->Dst;
register UBYTE *Mul8=SC->Mul8;
register UBYTE *Add8=SC->Add8;
register UBYTE *Cf;
register UBYTE *Ct;
register union oper3D Oper1;
register union oper3D Oper2;
register struct rgbabuffer3D* End=SC->End;
register UBYTE tmp;

SREM(PixelsSrcAlpha_One32)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA;  Ct=Src->RGBA;
	if (AT > MINALPHA)				/* if source visible ? */
	{
		if(AT < MAXALPHA)				/* if source not solid ? */
		{
		MUL8(Ct[0],AT,tmp) ADD8(tmp,Cf[0],Cf[0])
		MUL8(Ct[1],AT,tmp) ADD8(tmp,Cf[1],Cf[1])
		MUL8(Ct[2],AT,tmp) ADD8(tmp,Cf[2],Cf[2])
		}
		else
		{
		ADD8(Ct[0],Cf[0],Cf[0])
		ADD8(Ct[1],Cf[1],Cf[1])
		ADD8(Ct[2],Cf[2],Cf[2])
		}

	}
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;

}
/*=============================================================*/
void PixelsOne_One24(struct SOFT3D_context *SC)
{
/* v41: add saturate src & dst  (allways) */
/*
BlendingFunctions used in BlitzQuake:
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	---> PixelsSrcAlpha_OneMinusSrcAlpha32
glBlendFunc (GL_ONE, GL_ONE);					---> PixelsOne_One24 (v41)
glBlendFunc (GL_ZERO, GL_SRC_ALPHA);			---> PixelsZero_SrcAlpha32 (v41)
glBlendFunc (GL_ZERO, GL_SRC_COLOR));			---> PixelsZero_SrcColor24 (v45)

*/
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Add8=SC->Add8;
register union oper3D Oper1;

SREM(PixelsOne_One24)
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
void PixelsZero_SrcAlpha32(struct SOFT3D_context *SC)
{
/* v41: alpha-modulate dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsZero_SrcAlpha32)
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
void PixelsZero_SrcColor24(struct SOFT3D_context *SC)
{
/* v45: color-modulate dst  (allways) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsZero_SrcColor24)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	MUL8(Ct[0],Cf[0],Cf[0])
	MUL8(Ct[1],Cf[1],Cf[1])
	MUL8(Ct[2],Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void PixelsZero_OneMinusSrcColor24(struct SOFT3D_context *SC)
{
/* v45: for glxcess :-) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

SREM(PixelsZero_OneMinusSrcColor24)
	Oper1.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	MUL8(ONE-Ct[0],Cf[0],Cf[0])
	MUL8(ONE-Ct[1],Cf[1],Cf[1])
	MUL8(ONE-Ct[2],Cf[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void PixelsSrcAlpha_OneMinusSrcColor32(struct SOFT3D_context *SC)
{
/* v45: for glxcess :-) */
register struct rgbabuffer3D* Src=SC->Src;	/* tex */
register struct rgbabuffer3D* Dst=SC->Dst;	/* color  */
register struct rgbabuffer3D* End=SC->End;
register UBYTE *Cf;
register UBYTE *Ct;
register UBYTE *Mul8=SC->Mul8;
register UBYTE *Add8=SC->Add8;
register union oper3D Oper1;
register union oper3D Oper2;
register UBYTE *tmp;

SREM(PixelsSrcAlpha_OneMinusSrcColor32)
	Oper1.L.Index=Oper2.L.Index=0;
Pixels:
	Cf=Dst->RGBA; Ct=Src->RGBA;
	FULLADDMUL8(Ct[0],AT,Cf[0],ONE-Ct[0],Cf[0])
	FULLADDMUL8(Ct[1],AT,Cf[1],ONE-Ct[1],Cf[1])
	FULLADDMUL8(Ct[2],AT,Cf[2],ONE-Ct[2],Cf[2])
	Dst++;
	Src++;
	if(Dst!=End)	 goto Pixels;
}
/*=============================================================*/
void PixelsDstColor_Zero24(struct SOFT3D_context *SC)
{
SREM(PixelsDstColor_Zero24)
	PixelsZero_SrcColor24(SC);
}
/*=============================================================*/
void PixelsInBGRA(struct SOFT3D_context *SC)
{
/* Convert BGRA -> buffer */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	COPYRGBA(Frag[0].BufferRGBA,Frag[0].Image8);
	Frag[0].BufferRGBA[0]=Frag[0].Image8[2];
	Frag[0].BufferRGBA[2]=Frag[0].Image8[0];

	COPYRGBA(Frag[1].BufferRGBA,Frag[1].Image8);
	Frag[1].BufferRGBA[0]=Frag[1].Image8[2];
	Frag[1].BufferRGBA[2]=Frag[1].Image8[0];
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsOutBGRA(struct SOFT3D_context *SC)
{
/* Convert buffer -> BGRA */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	COPYRGBA(Frag[0].Image8,Frag[0].BufferRGBA);
	Frag[0].Image8[2]=Frag[0].BufferRGBA[0];
	Frag[0].Image8[0]=Frag[0].BufferRGBA[2];

	COPYRGBA(Frag[1].Image8,Frag[1].BufferRGBA);
	Frag[1].Image8[2]=Frag[1].BufferRGBA[0];
	Frag[1].Image8[0]=Frag[1].BufferRGBA[2];
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsInARGB(struct SOFT3D_context *SC)
{
/* Convert ARGB -> buffer */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	Frag[0].BufferRGBA[0]=Frag[0].Image8[1];
	Frag[0].BufferRGBA[1]=Frag[0].Image8[2];
	Frag[0].BufferRGBA[2]=Frag[0].Image8[3];
	Frag[0].BufferRGBA[3]=Frag[0].Image8[0];

	Frag[1].BufferRGBA[0]=Frag[1].Image8[1];
	Frag[1].BufferRGBA[1]=Frag[1].Image8[2];
	Frag[1].BufferRGBA[2]=Frag[1].Image8[3];
	Frag[1].BufferRGBA[3]=Frag[0].Image8[0];
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsOutARGB(struct SOFT3D_context *SC)
{
/* Convert buffer -> ARGB */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	Frag[0].Image8[1]=Frag[0].BufferRGBA[0];
	Frag[0].Image8[2]=Frag[0].BufferRGBA[1];
	Frag[0].Image8[3]=Frag[0].BufferRGBA[2];
	Frag[0].Image8[0]=Frag[0].BufferRGBA[3];

	Frag[1].Image8[1]=Frag[1].BufferRGBA[0];
	Frag[1].Image8[2]=Frag[1].BufferRGBA[1];
	Frag[1].Image8[3]=Frag[1].BufferRGBA[2];
	Frag[1].Image8[0]=Frag[0].BufferRGBA[3];
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsInRGB(struct SOFT3D_context *SC)
{
/* Convert RGB -> buffer */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	Frag[0].BufferRGBA[0]=Frag[0].Image8[0];
	Frag[0].BufferRGBA[1]=Frag[0].Image8[1];
	Frag[0].BufferRGBA[2]=Frag[0].Image8[2];
	Frag[0].BufferRGBA[3]=ONE;

	Frag[1].BufferRGBA[0]=Frag[1].Image8[0];
	Frag[1].BufferRGBA[1]=Frag[1].Image8[1];
	Frag[1].BufferRGBA[2]=Frag[1].Image8[2];
	Frag[1].BufferRGBA[3]=ONE;
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsOutRGB(struct SOFT3D_context *SC)
{
/* Convert buffer -> RGB */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	Frag[0].Image8[0]=Frag[0].BufferRGBA[0];
	Frag[0].Image8[1]=Frag[0].BufferRGBA[1];
	Frag[0].Image8[2]=Frag[0].BufferRGBA[2];

	Frag[1].Image8[0]=Frag[1].BufferRGBA[0];
	Frag[1].Image8[1]=Frag[1].BufferRGBA[1];
	Frag[1].Image8[2]=Frag[1].BufferRGBA[2];
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsInRGBA(struct SOFT3D_context *SC)
{
/* Convert RGBA -> buffer */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	COPYRGBA(Frag[0].BufferRGBA,Frag[0].Image8);
	COPYRGBA(Frag[1].BufferRGBA,Frag[1].Image8);
	Frag+=2;
	}

}
/*=============================================================*/
void PixelsOutRGBA(struct SOFT3D_context *SC)
{
/* Convert buffer -> RGBA */
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=(SC->FragBufferDone - SC->FragBuffer + 1)/2;

	while(0<size--)
	{
	COPYRGBA(Frag[0].Image8,Frag[0].BufferRGBA);
	COPYRGBA(Frag[1].Image8,Frag[1].BufferRGBA);
	Frag+=2;
	}

}
/*=============================================================*/
void SelectMipMap(struct SOFT3D_context *SC)
{
LONG dmin;
UBYTE mm;

	if(SC->ST==NULL) return;
	SC->MM=&SC->ST->MMs[0];

	if(!Wazp3D.DoMipMaps.ON) return;

SREM(SelectMipMap)
	dmin=SC->dmin>>16;

 	mm=3;
	if(dmin>=2) 	mm=4;
	if(dmin>=4) 	mm=5;
	if(dmin>=8) 	mm=6;
	if(dmin>=16) 	mm=7;
	if(dmin>=32) 	mm=8;
	if(dmin>=64) 	mm=9;
	if(dmin>=128) 	mm=10;
	if(dmin>=256) 	mm=11;

	if(dmin==1024)
	 	mm=3;

	VAR(dmin)
	VAR(mm)
	SC->MM=&SC->ST->MMs[mm];
}
/*=============================================================*/
void Fill_BigTexPersp_Gouraud_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_BigTexPersp_Gouraud_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->Tex8+=MM->Tex8Ulow[Pix->W.u3]+MM->Tex8Vlow[Pix->W.v3];
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_BigTexPersp_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_BigTexPersp_Gouraud_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->Tex8+=MM->Tex8Ulow[Pix->W.u3]+MM->Tex8Vlow[Pix->W.v3];
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_TexPersp_Gouraud_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_TexPersp_Gouraud_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Tex_Gouraud_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

	if(Wazp3D.SimuPerspective.ON)
		{Fill_TexPersp_Gouraud_Fog(SC); return;}
SREM(Fill_Tex_Gouraud_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Gouraud_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Gouraud_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Gouraud(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Gouraud)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		Frag++;
		}
	Image32++;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_TexPersp_Gouraud(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_TexPersp_Gouraud)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Tex_Gouraud(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

	if(Wazp3D.SimuPerspective.ON)
		{Fill_TexPersp_Gouraud(SC); return;}
SREM(Fill_Tex_Gouraud)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		Frag->ColorRGBA[0]=Pix->W.R;
		Frag->ColorRGBA[1]=Pix->W.G;
		Frag->ColorRGBA[2]=Pix->W.B;
		Frag->ColorRGBA[3]=Pix->W.A;
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.R +=Pix->L.dR;
	Pix->L.G +=Pix->L.dG;
	Pix->L.B +=Pix->L.dB;
	Pix->L.A +=Pix->L.dA;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_TexPersp_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_TexPersp_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Tex_Fog(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

	if(Wazp3D.SimuPerspective.ON)
		{Fill_TexPersp_Fog(SC); return;}
SREM(Fill_Tex_Fog)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		COPYRGBA(Frag->FogRGBA,(&SC->FogRGBAs[Pix->W.F]));
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.F +=Pix->L.dF;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_TexPersp(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_TexPersp)
	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	Pix->L.du+=Pix->L.ddu;
	Pix->L.dv+=Pix->L.ddv;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Tex(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

	if(Wazp3D.SimuPerspective.ON)
		{Fill_TexPersp(SC); return;}
SREM(Fill_Tex)
	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		Frag->Tex8 =MM->Tex8U   [Pix->W.u ]+MM->Tex8V   [Pix->W.v ];
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		Frag++;
		}
	Image32++;
	Pix->L.u +=Pix->L.du;
	Pix->L.v +=Pix->L.dv;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Fill_Flat(struct SOFT3D_context *SC)
{
register ULONG *Image32;
register UBYTE *Ztest;
register union pixel3D *Pix=SC->Pix;
register struct SOFT3D_mipmap *MM=SC->MM;
register struct fragbuffer3D *Frag=SC->FragBufferDone;
register WORD high  =SC->PolyHigh;
register WORD large;

SREM(Fill_Flat)

	while(0<high--)
	{
	Image32  =&Pix->L.Image32Y  [Pix->W.x];
	large	   = Pix->W.large;

	SC->Pix=Pix; SC->FunctionZtest(SC); Ztest=SC->Ztest;
	while(0<large--)
	{
	if(*Ztest++)	
		{
		Frag->Image8=(UBYTE *)Image32;
		COPYRGBA(Frag->ColorRGBA,SC->FlatRGBA);
		Frag++;
		}
	Image32++;
	}

	if(Frag > SC->FragBufferMaxi)
		{SC->FragBufferDone=Frag;  DrawFragBuffer(SC); Frag=SC->FragBuffer;}
	Pix++; 
	}
SC->FragBufferDone=Frag;
}
/*=============================================================*/
void Ztest_znever_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(FALSE);  Ztest++; }
}
/*=============================================================*/
void Ztest_zless_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE < *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_zgequal_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE >= *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_zlequal_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE <= *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_zgreater_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE > *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_znotequal_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE != *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_zequal_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE == *Zbuffer ); if(*Ztest) *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
void Ztest_zalways_update(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(TRUE); *Zbuffer=ZVALUE; Ztest++; Zbuffer++; Pix->L.z+=dz;}
}
/*=============================================================*/
/* void Ztest_znever(struct SOFT3D_context *SC) same as Ztest_znever_update */
/*=============================================================*/
void Ztest_zless(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE < *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_zgequal(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE >= *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_zlequal(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE <= *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_zgreater(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE > *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_znotequal(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE != *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_zequal(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register ZBUFF  *Zbuffer= &(Pix->L.ZbufferY[Pix->W.x]);
register LONG dz=Pix->L.dz;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(ZVALUE == *Zbuffer ); Ztest++; Zbuffer++; Pix->L.z+=dz; }
}
/*=============================================================*/
void Ztest_zalways(struct SOFT3D_context *SC)
{
register UBYTE *Ztest=SC->Ztest;
register union pixel3D *Pix=SC->Pix;
register WORD large=Pix->W.large;

	while(0<large--)
		{ *Ztest=(TRUE);  Ztest++; }
}
/*=============================================================*/
void SetTexture(W3D_Context* context,W3D_Texture *texture)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT=NULL;
void *ST=NULL;
BOOL blended;
ULONG TexEnvMode;
UBYTE EnvRGBA[4];

SREM(SetTexture)
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
		WC->uresize=256.0/(float)(WT->large);
		WC->vresize=256.0/(float)(WT->high );
		PrintWT(WT);
		}

	TexEnvMode=context->globaltexenvmode;  		/* global texture-env-mode  */
	ColorToRGBA(EnvRGBA,context->globaltexenvcolor[0],context->globaltexenvcolor[1],context->globaltexenvcolor[2],context->globaltexenvcolor[3]); /* global texture-env-color */
	if(WT!=NULL)
	if(!StateON(W3D_GLOBALTEXENV))
	{
	TexEnvMode=WT->TexEnvMode;		/* texture's texture-env-mode  */
	COPYRGBA(EnvRGBA,WT->EnvRGBA);	/* texture's texture-env-color */
	}

	WC->UseFog  =  (StateON(W3D_FOGGING)) et (WC->FogMode!=0);

	WC->ZMode=W3D_Z_ALWAYS;		/* default no z buffer = no test ==> always draw & dont update a zbuffer */

	if(context->zbuffer!=NULL)
	if(StateON(W3D_ZBUFFER)) 
		WC->ZMode   =8*(StateON(W3D_ZBUFFERUPDATE)) + WC->ZbufferMode;

	WC->UseTex  =  (StateON(W3D_TEXMAPPING)) et (texture!=NULL);
	blended     =  (StateON(W3D_BLENDING));

/* step 1: define TexEnvMode */
	WC->TexEnvMode='0';				/* if no tex ==> no TexEnv effect */	
	if(WC->UseTex)
		{
		WC->TexEnvMode='R';		/* default W3D_REPLACE  : simply R-eplace the color with tex = copy tex */

		if(Wazp3D.UseColoring.ON)		/* If  use color for lighting*/
		if(TexEnvMode!=W3D_REPLACE)
			WC->TexEnvMode='m';		/* then do a simple m-edian (color + tex)/2 */

		if(Wazp3D.UseColoringGL.ON)		/* If  use color (new functions)  for lighting*/
			{
			if(TexEnvMode==W3D_DECAL)
				WC->TexEnvMode='D';		/* then D-ecal color & tex */
			if(TexEnvMode==W3D_MODULATE)
				WC->TexEnvMode='M';		/* then M-odulate color & tex */
			if(TexEnvMode==W3D_BLEND)
				WC->TexEnvMode='B';		/* then B-lend color & tex & envcolor*/
			}

		}

/* step 2: define BlendMode */
		WC->BlendMode='R';				/* default : simply W-rite the color (even if color=tex+color) */
		if(blended)
			{
			WC->BlendMode='0';
			if(WC->SrcFunc==W3D_SRC_ALPHA)
			if(WC->DstFunc==W3D_ONE_MINUS_SRC_ALPHA)
				WC->BlendMode='A';		/* classic A-lpha transparency */
			if(WC->SrcFunc==W3D_SRC_ALPHA)
			if(WC->DstFunc==W3D_ONE)
				WC->BlendMode='G';		/* for demo G-lxcess transparency */
			if(WC->SrcFunc==W3D_ONE)
			if(WC->DstFunc==W3D_ONE)
				WC->BlendMode='S';		/* S-aturate = Quake's glow effect */
			if(WC->SrcFunc==W3D_ZERO)
			if(WC->DstFunc==W3D_SRC_ALPHA)
				WC->BlendMode='L';		/* L-ightmap effect */
			if(WC->SrcFunc==W3D_ZERO)
			if(WC->DstFunc==W3D_SRC_COLOR)
				WC->BlendMode='M';		/* M-odulate effect */
			if(WC->SrcFunc==W3D_DST_COLOR)
			if(WC->DstFunc==W3D_ZERO)
				WC->BlendMode='m';		/* m-odulate effect = same as M */
			if(WC->SrcFunc==W3D_ZERO)
			if(WC->DstFunc==W3D_ONE_MINUS_SRC_COLOR)
				WC->BlendMode='C';		/* C-oloring for demo Glxcess */
			if(WC->SrcFunc==W3D_SRC_ALPHA)
			if(WC->DstFunc==W3D_ONE_MINUS_SRC_COLOR)
				WC->BlendMode='c';		/* c-oloring for demo Glxcess */

			if(WC->BlendMode=='0')
			if(Wazp3D.DebugTexNumber.ON)
			{
			LibDebug=Wazp3D.DebugVal.ON=Wazp3D.DebugVar.ON=TRUE;
			PrintST(ST);
			W3D_SetBlendMode(context,WC->SrcFunc,WC->DstFunc);	/* then print it */
			LibDebug=Wazp3D.DebugVal.ON=Wazp3D.DebugVar.ON=FALSE;
			}

			if(WC->BlendMode=='0')
				WC->BlendMode='A';			/* default: classic A-lpha transparency */
			}

	WC->UseGouraud=StateON(W3D_GOURAUD);

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf(" WC: ZMode%ld BlendMode%lc TexEnvMode%lc UseFog%ld UseTex%ld Blended%ld UseGouraud%ld\n",WC->ZMode,WC->BlendMode,WC->TexEnvMode,WC->UseFog,WC->UseTex,blended,WC->UseGouraud);
#endif
	SOFT3D_SetEnvColor(WC->SC,EnvRGBA);
	SOFT3D_SetDrawStates(WC->SC,ST,WC->ZMode,WC->BlendMode,WC->TexEnvMode,WC->UseGouraud,WC->UseFog);
}
/*=============================================================*/
void SOFT3D_SetDrawStates(struct SOFT3D_context *SC,struct SOFT3D_texture *ST,UBYTE ZMode,UBYTE BlendMode,UBYTE TexEnvMode,UBYTE UseGouraud,UBYTE UseFog)
{

SREM(SOFT3D_SetDrawStates)
	if(ST!=NULL)
		PrintST(ST);

	SC->ST		=ST;
	SC->ZMode		=ZMode;
	SC->BlendMode	=BlendMode;
	SC->TexEnvMode	=TexEnvMode;
	SC->UseGouraud	=UseGouraud;
	SC->UseFog		=UseFog;

	if(Wazp3D.DebugSOFT3D.ON) 
		Libprintf(" SC: ZMode%ld BlendMode%lc TexEnvMode%lc  UseGouraud%ld UseFog%ld\n",SC->ZMode,SC->BlendMode,SC->TexEnvMode,SC->UseGouraud,SC->UseFog);

}
/*=============================================================*/
void SOFT3D_SetDrawFunctions(struct SOFT3D_context *SC)
{
struct SOFT3D_texture *ST;
UBYTE FillMode,ZMode,BlendMode,TexEnvMode,UseFog,UseGouraud,UseTex,BigTex;
BOOL tex24;

SREM(SOFT3D_SetDrawFunctions)

SREM(flush remaining pixels)
	DrawFragBuffer(SC);	

	ST		=SC->ST;
	ZMode		=SC->ZMode;	   
	TexEnvMode	=SC->TexEnvMode;	   
	BlendMode	=SC->BlendMode;	   
	UseFog	=SC->UseFog;	   
	UseGouraud	=SC->UseGouraud;	 

	if(Wazp3D.DebugSOFT3D.ON) 
		Libprintf(" SC: ZMode%ld BlendMode%lc TexEnvMode%lc UseGouraud%ld ColorChange%ld UseFog%ld ColorTransp%ld\n",SC->ZMode,SC->BlendMode,SC->TexEnvMode,SC->UseGouraud,SC->ColorChange,SC->UseFog,SC->ColorTransp);

	SREM(FlatRGBA);		PrintRGBA(SC->FlatRGBA);
	SREM(CurrentRGBA);	PrintRGBA(SC->CurrentRGBA);


/* If it's debug-texture then activate debugger */
	if(ST!=NULL)
	if(Wazp3D.DebugTexNumber.ON)
	{
	if(ST->Tnum==DEBUGTNUM)
		{
		LibDebug=TRUE;
		Libprintf("Using DebugTexture %s\n",ST->name);
		}
	else
		{
		LibDebug=FALSE;
		}
	}



/* change some flags if needed*/
	if(ST!=NULL)
	{
	if(Wazp3D.HackRGBA1.ON)
		{TexEnvMode='0';	BlendMode='R'; UseFog=FALSE;	UseGouraud=SC->ColorChange=TRUE; }
	if(Wazp3D.HackRGBA2.ON)
		{TexEnvMode='0';	BlendMode='R'; UseFog=FALSE;	UseGouraud=SC->ColorChange=FALSE;}
	}
	

	if(UseGouraud==FALSE)					/* if truly want flat color*/
		{
		SREM(True Flat)
		if(!Wazp3D.HackRGBA2.ON) 			/* in this case flatcolor is already polygon's averagecolor */
		COPYRGBA(SC->FlatRGBA,SC->CurrentRGBA);	/* if true no-gouraud mode then flat-color come from current-color  */
		}

	if(!SC->ColorChange)					/* then dont truly need gouraud shading */
		{
		SREM(Gouraud to Flat)
		UseGouraud=FALSE;					/* then flat color come from point-color NOT from current-color */
		}


	if(ST!=NULL)
		PrintST(ST);

	UseTex=(TexEnvMode!='0');	/* if use tex */
	tex24=FALSE;			/* default 32 bits color */
	BigTex=FALSE;
	if(UseTex)
		{
		if(256<ST->large) BigTex=TRUE;
		if(256<ST->high ) BigTex=TRUE;
		tex24=(ST->bits==24);	/* but the tex can be only 24 bits */
		}


	if(UseTex)
	if(BlendMode=='A')			/* if want alpha  */  
		{
		SREM(can optim A to a)
		BlendMode=ST->BlendMode;		/* then use best BlendMode (A or a) */

		if(TexEnvMode!='R')			/* if got a coloring function */
		if(SC->ColorTransp)			/* that colorize in alpha (=make texture transparent)*/
			{
			SREM(ColorTransp so really use A)
			BlendMode='A';			/* then use the tex as a true alpha one (A) */
			}
		}

	if(TexEnvMode=='R')			/* if tex and no coloring function */
	if(BlendMode=='R')			/* and write flat */
	if(Wazp3D.QuakePatch.ON)
		{
		SREM(Quake use own BlendMode )
		BlendMode=ST->BlendMode;	/* then use current tex BlendMode (R or A or a) */
		}



/* if color is white dont do a stupid flat-color modulate on a texture = do nothing */
	if(UseTex)
	if(TexEnvMode!='R')			/* with a coloring function */
	if(!SC->ColorChange)		
	if(SC->FlatRGBA[0]==255)
	if(SC->FlatRGBA[1]==255)
	if(SC->FlatRGBA[2]==255)
	if(SC->FlatRGBA[3]==255)
		{
		SREM(White so no Coloring)
		TexEnvMode='R';
		}


	FillMode=UseTex*4  +  UseGouraud*2 + UseFog*1;
	SC->FunctionFill	=Wazp3D.FunctionsFill[FillMode];
	if(BigTex)
		{
		SC->FunctionFill=(HOOKEDFUNCTION)Fill_BigTexPersp_Gouraud_Fog;
		if(!UseGouraud)
			SC->FunctionFill=(HOOKEDFUNCTION)Fill_BigTexPersp_Fog;
		}

	SC->FunctionZtest	=Wazp3D.FunctionsZtest [ZMode];
	SINFO(SC->FunctionZtest,(void *)Ztest_znever_update)
	SINFO(SC->FunctionZtest,(void *)Ztest_zless)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zgequal)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zlequal)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zgreater)  
	SINFO(SC->FunctionZtest,(void *)Ztest_znotequal)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zequal)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zalways)
   
	SINFO(SC->FunctionZtest,(void *)Ztest_znever_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zless_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zgequal_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zlequal_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zgreater_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_znotequal_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zequal_update)  
	SINFO(SC->FunctionZtest,(void *)Ztest_zalways_update) 


	SC->FunctionTexEnv=NULL;
	SC->FunctionBlend	=NULL;
	SC->FunctionFog	=NULL;
	SC->FunctionSepia	=NULL;

/* FunctionTexEnv */

	if(TexEnvMode=='R')
	{
			SC->FunctionTexEnv=NULL;	/* do nothing : let the FunctionBlend do the copy */
			TexEnvMode='0';
	}

	if(TexEnvMode=='m')
	{
	if(tex24)
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsMedian24;
	else
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsMedian32;
	}

	if(TexEnvMode=='M')
	{
	if(tex24)
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsModulate24;
	else
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsModulate32;
	}

	if(TexEnvMode=='D')
	{
	if(tex24)
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsDecal24;
	else
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsDecal32;
	}

	if(TexEnvMode=='B')
	{
	if(tex24)
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsBlend24;
	else
			SC->FunctionTexEnv=(HOOKEDFUNCTION)PixelsBlend32;
	}


/* FunctionBlend */
	if(BlendMode=='A')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsSrcAlpha_OneMinusSrcAlpha32;
	if(BlendMode=='a')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsSrcAlpha_OneMinusSrcAlpha32fast;
	if(BlendMode=='G')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsSrcAlpha_One32;
	if(BlendMode=='S')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsOne_One24;
	if(BlendMode=='L')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsZero_SrcAlpha32;
	if(BlendMode=='M')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsZero_SrcColor24;
	if(BlendMode=='m')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsDstColor_Zero24;
	if(BlendMode=='C')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsZero_OneMinusSrcColor24;
	if(BlendMode=='c')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsSrcAlpha_OneMinusSrcColor32;

	if(tex24)
	if(SC->FunctionTexEnv==NULL)		/* if no function to convert a 24bits tex to 32bits color*/
	{
	if(BlendMode=='A')	BlendMode='R';		/* then dont use the 32 bits functions that use SrcAlpha(=ONE) */
	if(BlendMode=='a')	BlendMode='R';
	if(BlendMode=='G')	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsOne_One24;
	if(BlendMode=='L')	BlendMode='0';		/* then do Zero_SrcOne = let the destination unchanged =do nothing*/ 
	}
	
	SC->UseFunctionIn=(SC->FunctionIn!=NULL);
	if(BlendMode=='R')
	{
	SC->FunctionBlend=(HOOKEDFUNCTION)PixelsColorToImage;
	if(SC->FunctionIn!=NULL)
		SC->FunctionBlend=(HOOKEDFUNCTION)PixelsColorToBuffer;
	if(UseTex)
	if(SC->FunctionTexEnv==NULL)		/* if no function to convert a 24bits tex to 32bits color*/
		{
		if(tex24)
			{
			SC->FunctionBlend=(HOOKEDFUNCTION)PixelsTex24ToImage;
			if(SC->FunctionIn!=NULL)
				SC->FunctionBlend=(HOOKEDFUNCTION)PixelsTex24ToBuffer;
			}
		else
			{
			SC->FunctionBlend=(HOOKEDFUNCTION)PixelsTex32ToImage;
			if(SC->FunctionIn!=NULL)
				SC->FunctionBlend=(HOOKEDFUNCTION)PixelsTex32ToBuffer;
			}
		SC->UseFunctionIn=FALSE;		/* not need to read Image's pixels = : they are all replaced */
		}
	}

	if(Wazp3D.DebugSOFT3D.ON) 
		Libprintf("Functions set with TexEnvMode%lc BlendMode%lc\n",TexEnvMode,BlendMode);

/* FunctionFog */
	if(UseFog)
		{
		SC->FunctionFog=(HOOKEDFUNCTION)PixelsFogToImage;
		if(SC->FunctionIn!=NULL)
			SC->FunctionFog=(HOOKEDFUNCTION)PixelsFogToBuffer;
		}

/* FunctionSepia */
	if(Wazp3D.DebugSepiaImage.ON)
	{
		SC->FunctionSepia=(HOOKEDFUNCTION)PixelsSepiaToImage;
		if(SC->FunctionIn!=NULL)
			SC->FunctionSepia=(HOOKEDFUNCTION)PixelsSepiaToBuffer;
	}

	SC->UseTex	=UseTex;
}
/*=============================================================*/
void DrawFragBuffer(struct SOFT3D_context *SC)
{
register struct fragbuffer3D *Frag=SC->FragBuffer;
register ULONG size=SC->FragBufferDone - SC->FragBuffer;
struct rgbabuffer3D *CurrentImage8;

SREM(DrawFragBuffer)
	if(SC->Image32==NULL) return;
	if(size==0) {SREM( ==> nothing to Draw);goto DrawFragBufferEnd;}
	VAR(size)

/* add one (noop) fragment to FragBuffer so can do size=(size + 1)/2 and so can process 2 Frags inside a loop   */
	SC->FragBufferDone->Tex8	=SC->NoopRGBA;	/* read the noop pixel as texture */
	SC->FragBufferDone->Image8	=SC->NoopRGBA;	/* write it in noop pixel = do nothing with Image */

/* default: read & write directly to Image8 (=Wazp3D's RGBAbuffer ) */
	CurrentImage8=(struct rgbabuffer3D *)&Frag->Image8;

/* cant print inside lockbitmap so print the functionIn/Out name here */
	SINFO(SC->FunctionIn,(void *)NULL)
	SINFO(SC->FunctionIn,(void *)PixelsInBGRA)
	SINFO(SC->FunctionIn,(void *)PixelsInRGBA)
	SINFO(SC->FunctionIn,(void *)PixelsInARGB)
	SINFO(SC->FunctionIn,(void *)PixelsInRGB)
	VAR(SC->UseFunctionIn)

	SINFO(SC->FunctionOut,(void *)NULL)
	SINFO(SC->FunctionOut,(void *)PixelsOutBGRA)
	SINFO(SC->FunctionOut,(void *)PixelsOutRGBA)
	SINFO(SC->FunctionOut,(void *)PixelsOutARGB)
	SINFO(SC->FunctionOut,(void *)PixelsOutRGB)

/* Pass 1: Read/convert pixel from bitmap */
	if(SC->FunctionIn!=NULL)
	if(SC->UseFunctionIn)
	{
SREM(FunctionIn)
	if(!LockBM(SC))
		goto DrawFragBufferEnd;		/* if cant lock ==> panic ==> exit */
	SC->FunctionIn(SC);			/* convert readed pixels to RGBA format. TODO: manage 8/15/16 bits mode too */
	UnLockBM(SC);
	CurrentImage8=(struct rgbabuffer3D *)&Frag->Buffer8;		/* now read & write to buffer for bitmap pixel  */
	}

/* Pass 2: apply color (=light) */
	if(SC->FunctionTexEnv!=NULL)
	{
SREM(FunctionTexEnv)
	SC->FunctionTexEnv(SC);
	}

/* Pass 3: apply texture */
	if(SC->FunctionBlend!=NULL)
	{
SREM(FunctionBlend)
	SC->Src=(struct rgbabuffer3D *)&Frag->Color8;		/* default use color(can be color=color+tex) */
	if(SC->UseTex)							/* if use tex */
	if(SC->FunctionTexEnv==NULL)					/* and no coloring */
		{
		SREM(use tex only)
		SC->Src=(struct rgbabuffer3D *)&Frag->Tex8;	/* then use tex only   */
		}
	SC->Dst=CurrentImage8;
	SC->End=SC->Dst+size;
	SC->FunctionBlend(SC);
	}

/* Pass 4: apply fog */
	if(SC->FunctionFog!=NULL)
	{
SREM(FunctionFog)
	SC->FunctionFog(SC);
	}

/* Pass 5: apply sepia (debug effect) */
	if(SC->FunctionSepia!=NULL)
	{
SREM(FunctionSepia)
	SC->FunctionSepia(SC);
	}

/* Pass 6: Write/convert pixel to bitmap */
	if(SC->FunctionOut!=NULL)
	{
SREM(FunctionOut)
	if(LockBM(SC))
	{
	SC->FunctionOut(SC);
	UnLockBM(SC);
	}
	}
DrawFragBufferEnd:
	SC->FragBufferDone =SC->FragBuffer;
}
/*=============================================================*/
void DrawSimplePix(struct SOFT3D_context *SC,register union pixel3D *P)
{

SFUNCTION(DrawPointSimplePIX)
	if ( (P->W.x < 0) ou (P->W.y < 0) ou (SC->large <= P->W.x) ou (SC->high <= P->W.y))
		return;

	SC->Pix  =&(SC->edge1[P->W.y]);
	CopyPix(SC->Pix,P);
	SC->Pix->W.large=1;
	SC->PolyHigh=1;

	if(SC->ST!=NULL)
		SC->MM=&SC->ST->MMs[0];	/* default to biggest mipmap <=> never call FunctionFill with SC->MM=NULL*/

	SC->FunctionFill(SC);
}
/*=============================================================*/
void DrawPointPix(struct SOFT3D_context *SC)
{
register union pixel3D *P=SC->PolyPix;
register WORD *PointLarge=SC->PointLarge;
register WORD xmin,xmax,ymin,ymax,y;
register WORD R;

SFUNCTION(DrawPointPIX)
	SC->ColorChange=FALSE;
	SOFT3D_SetDrawStates(SC,NULL,SC->ZMode,'R','0',FALSE,FALSE);		/* default notex nofog nogouraud just color */
	SOFT3D_SetDrawFunctions(SC);

	COPYRGBA(SC->FlatRGBA,SC->PolyP->RGBA);

	if(SC->PointSize==1)
		{DrawSimplePix(SC,P);return;}

	R=SC->PointSize/2+1;
	xmin=0+R;
	ymin=0+R;
	xmax=SC->large-R;
	ymax=SC->high -R;
	if ( (P->W.x<xmin) ou (P->W.y<ymin) ou (xmax<=P->W.x) ou (ymax<=P->W.y))
		return;

	P->W.y	=P->W.y - SC->PointSize/2;
	SC->Pix	=&(SC->edge1[P->W.y]);
	SC->PolyHigh=SC->PointSize;

	YLOOP(SC->PolyHigh)
		{
		CopyPix(&SC->Pix[y],P);
		SC->Pix[y].W.large =PointLarge[y];
		SC->Pix[y].W.x	 =SC->Pix[y].W.x - PointLarge[y]/2;
		}

	if(SC->ST!=NULL)
		SC->MM=&SC->ST->MMs[0];	/* default to biggest mipmap <=> never call FunctionZtest with SC->MM=NULL*/
	SC->FunctionFill(SC);
}
/*=============================================================*/
void DoDeltasUV(union pixel3D *P1,union pixel3D *P2)
{
/* V44: Compute a quadratic approximation of texture to emulate perspective */
double a,b,z0,z2,rz4,n,dt,rz4dt;
double u0,u1,u2,du,ddu;
double v0,v1,v2,dv,ddv;

/*
	if(P1->L.w==P1->L.w)
	{
		P1->L.du=(P2->L.u - P1->L.u)/P1->W.large;
		P1->L.dv=(P2->L.v - P1->L.v)/P1->W.large;
		P1->L.ddu=P1->L.ddv=0;
		return;
	}
*/

	n=P1->W.large;
	z0=1.0/P1->L.w;	
	z2=1.0/P2->L.w;
	u0=P1->L.u;
	u2=P2->L.u;
	v0=P1->L.v;
	v2=P2->L.v;

	dt  = u2-u0;					
	rz4dt = (4.0*dt*z0) / (z0 + z2) ;		
	a  = ( -rz4dt +dt +dt ) / (n*n) ;
	b  = ( +rz4dt -dt )  	/  (n) ;
	 du = a + b ;
	ddu = a + a ;

	dt  = v2-v0;
	rz4dt = (4.0*dt*z0) / (z0 + z2) ;
	a  = ( -rz4dt +dt +dt ) / (n*n) ;
	b  = ( +rz4dt -dt )  	/  (n) ;
	 dv = a + b ;
	ddv = a + a ;

/* store ddu ddv du dv as LONG */
	P1->L.du	= du  ;
	P1->L.dv	= dv  ;
	P1->L.ddu	= ddu ;
	P1->L.ddv	= ddv ;

}
/*=============================================================*/
void EdgeMinDeltas(struct SOFT3D_context *SC,union pixel3D *P1,union pixel3D *P2)
{
/* for selecting mipmap find the minimum uv linear delta */ 
register WORD size;
register LONG u,v,du,dv;

	if(SC->BlendMode!='0')		/* then dont bother with corect texturing*/
		return;
SREM(EdgeMinDeltas)
	PrintPix(P1);
	PrintPix(P2);

	size=P2->W.y - P1->W.y;
	VAR(size)
	if(size==0)
		size=P2->W.x - P1->W.x;
	VAR(size)
	if(size==0)
		return;

	u=(P2->L.u - P1->L.u);
	v=(P2->L.v - P1->L.v);

	VAR(u>>16)
	VAR(v>>16)
	du=u/size;
	dv=v/size;

	if(du<0)	du=-du;	/* absolute value */
	if(dv<0)	dv=-dv;

	if(du!=0)			
	if(du<SC->dmin)			/* find minimal delta = texture step */
		SC->dmin=du;

	if(dv!=0)			
	if(dv<SC->dmin)			/* find minimal delta = texture step */
		SC->dmin=dv;

VAR(du>>16)
VAR(dv>>16)
VAR(SC->dmin>>16)
}
/*=============================================================*/
void DoEdgeTruePersp(struct SOFT3D_context *SC,union pixel3D *P1,union pixel3D *P2)
{
register WORD high,n;
double u,v,u2,v2,du,dv,w;
union pixel3D DeltaY;

SFUNCTION(DoEdgeTruePersp)
	high=P2->W.y - P1->W.y;
	if(high<2)	return;

	DeltaY.L.dx=(P2->L.x - P1->L.x)/high;
	DeltaY.L.dz=(P2->L.z - P1->L.z)/high;
	DeltaY.L.dw=(P2->L.w - P1->L.w)/high;
	DeltaY.L.dR=(P2->L.R - P1->L.R)/high;
	DeltaY.L.dG=(P2->L.G - P1->L.G)/high;
	DeltaY.L.dB=(P2->L.B - P1->L.B)/high;
	DeltaY.L.dA=(P2->L.A - P1->L.A)/high;
	DeltaY.L.dF=(P2->L.F - P1->L.F)/high;

	u =(double)P1->L.u * P1->L.w;
	u2=(double)P2->L.u * P2->L.w;
	v =(double)P1->L.v * P1->L.w;
	v2=(double)P2->L.v * P2->L.w;

	du=(u2 - u)/high;
	dv=(v2 - v)/high;

	high--;						/* ne pas recalculer les points extremites */
	NLOOP(high)
	{
		P1[1].L.x=P1->L.x+DeltaY.L.dx;
		P1[1].L.z=P1->L.z+DeltaY.L.dz;
		P1[1].L.w=P1->L.w+DeltaY.L.dw;
		P1[1].L.R=P1->L.R+DeltaY.L.dR;
		P1[1].L.G=P1->L.G+DeltaY.L.dG;
		P1[1].L.B=P1->L.B+DeltaY.L.dB;
		P1[1].L.A=P1->L.A+DeltaY.L.dA;
		P1[1].L.F=P1->L.F+DeltaY.L.dF;
		w=P1[1].L.w;
		u=u+du;
		v=v+dv;
		P1[1].L.u=u/w;
		P1[1].L.v=v/w;
		P1++;
	}
}
/*=============================================================*/
void DoEdge(struct SOFT3D_context *SC,union pixel3D *P1,union pixel3D *P2)
{
register WORD high,n;
union pixel3D DeltaY;

	if(SC->BlendMode!='0')
	{
	if(Wazp3D.DoMipMaps.ON)
		EdgeMinDeltas(SC,P1,P2);
	if(!Wazp3D.NoPerspective.ON) 
		{DoEdgeTruePersp(SC,P1,P2);return;}
	}

	high=P2->W.y - P1->W.y;
	if(high<2)	return;

SFUNCTION(DoEdge)
	DeltaY.L.dx=(P2->L.x - P1->L.x)/high;
	DeltaY.L.dz=(P2->L.z - P1->L.z)/high;
	DeltaY.L.du=(P2->L.u - P1->L.u)/high;
	DeltaY.L.dv=(P2->L.v - P1->L.v)/high;
	DeltaY.L.dR=(P2->L.R - P1->L.R)/high;
	DeltaY.L.dG=(P2->L.G - P1->L.G)/high;
	DeltaY.L.dB=(P2->L.B - P1->L.B)/high;
	DeltaY.L.dA=(P2->L.A - P1->L.A)/high;
	DeltaY.L.dF=(P2->L.F - P1->L.F)/high;

	high--;						/* ne pas recalculer les points extremites */
	NLOOP(high)
	{
		P1[1].L.x=P1->L.x+DeltaY.L.dx;
		P1[1].L.z=P1->L.z+DeltaY.L.dz;
		P1[1].L.u=P1->L.u+DeltaY.L.du;
		P1[1].L.v=P1->L.v+DeltaY.L.dv;
		P1[1].L.R=P1->L.R+DeltaY.L.dR;
		P1[1].L.G=P1->L.G+DeltaY.L.dG;
		P1[1].L.B=P1->L.B+DeltaY.L.dB;
		P1[1].L.A=P1->L.A+DeltaY.L.dA;
		P1[1].L.F=P1->L.F+DeltaY.L.dF;
		P1++;
	}

}
/*=============================================================*/
void FillPolyNoPersp(struct SOFT3D_context *SC,union pixel3D *edge1,union pixel3D *edge2,WORD high)
{
register WORD n,large,maxlarge,maxn;
register union pixel3D *P1;
register union pixel3D *P2;
union pixel3D DeltaX;

SFUNCTION(FillPolyNoPersp)
	P1=edge1;
	P2=edge2;
	maxn=maxlarge=0;
	NLOOP(high)			/* trouver le large max du polygone = meilleur delta */
		{
		large = P2->W.x - P1->W.x;
		P1->W.large = large; 	
		if(maxlarge < large)
			{maxn=n; maxlarge=large;}	
		P1++;P2++;
		}

/* compute best delta */
	P1=&edge1[maxn];
	P2=&edge2[maxn];
	if(maxlarge!=0)
	{
	DeltaX.L.dz=(P2->L.z - P1->L.z)/maxlarge;
	DeltaX.L.du=(P2->L.u - P1->L.u)/maxlarge;
	DeltaX.L.dv=(P2->L.v - P1->L.v)/maxlarge;
	DeltaX.L.dR=(P2->L.R - P1->L.R)/maxlarge;
	DeltaX.L.dG=(P2->L.G - P1->L.G)/maxlarge;
	DeltaX.L.dB=(P2->L.B - P1->L.B)/maxlarge;
	DeltaX.L.dA=(P2->L.A - P1->L.A)/maxlarge;
	DeltaX.L.dF=(P2->L.F - P1->L.F)/maxlarge;
	DeltaX.L.ddu=0;
	DeltaX.L.ddv=0;
	}

/* and copy same delta for all segments */ 
	P1=edge1;
	NLOOP(high)
	{
	P1->L.dz	=DeltaX.L.dz;
	P1->L.du	=DeltaX.L.du;
	P1->L.dv	=DeltaX.L.dv;
	P1->L.dR	=DeltaX.L.dR;
	P1->L.dG	=DeltaX.L.dG;
	P1->L.dB	=DeltaX.L.dB;
	P1->L.dA	=DeltaX.L.dA;
	P1->L.dF	=DeltaX.L.dF;
	P1->L.ddu	=0;
	P1->L.ddv	=0;
	P1++;
	}

	SC->Pix=edge1;
	SC->PolyHigh=high;
	SelectMipMap(SC);
	SC->FunctionFill(SC);
}
/*=============================================================*/
void FillPoly(struct SOFT3D_context *SC,union pixel3D *edge1,union pixel3D *edge2,WORD high)
{
register union pixel3D *P1;
register union pixel3D *P2;
register WORD n,large;

	if(Wazp3D.NoPerspective.ON)
		{FillPolyNoPersp(SC,edge1,edge2,high);	return;}

SFUNCTION(FillPoly)
	P1=edge1;
	P2=edge2;

/* compute a delta per segment */ 
	if(Wazp3D.SimuPerspective.ON)
	NLOOP(high)
	{
	large=P2->W.x - P1->W.x;	
	P1->W.large=large;
		if(large!=0)
		{
		P1->L.dz=(P2->L.z - P1->L.z)/large;
		P1->L.dR=(P2->L.R - P1->L.R)/large;
		P1->L.dG=(P2->L.G - P1->L.G)/large;
		P1->L.dB=(P2->L.B - P1->L.B)/large;
		P1->L.dA=(P2->L.A - P1->L.A)/large;
		P1->L.dF=(P2->L.F - P1->L.F)/large;
		DoDeltasUV(P1,P2);
		}
	P1++,P2++;
	}
	else
	NLOOP(high)
	{
	large=P2->W.x - P1->W.x;	
	P1->W.large=large;
		if(large!=0)
		{
		P1->L.dz=(P2->L.z - P1->L.z)/large;
		P1->L.du=(P2->L.u - P1->L.u)/large;
		P1->L.dv=(P2->L.v - P1->L.v)/large;
		P1->L.dR=(P2->L.R - P1->L.R)/large;
		P1->L.dG=(P2->L.G - P1->L.G)/large;
		P1->L.dB=(P2->L.B - P1->L.B)/large;
		P1->L.dA=(P2->L.A - P1->L.A)/large;
		P1->L.dA=(P2->L.F - P1->L.F)/large;
		P1->L.ddu=0;
		P1->L.ddv=0;
		}
	P1++,P2++;
	}


	SC->Pix=edge1;
	SC->PolyHigh=high;
	SelectMipMap(SC);
	SC->FunctionFill(SC);

}
/*=============================================================*/
void FillPolyDouble(struct SOFT3D_context *SC,union pixel3D *edge1,union pixel3D *edge2,union pixel3D *edgeM,WORD high)
{
/* V44: convert polygon to 2 polygons for better perspective emulation */
register union pixel3D *P1;
register union pixel3D *P2;
register union pixel3D *PM;
register WORD n;
float u,v;

SFUNCTION(FillPolyDouble)
	P1=edge1;
	PM=edgeM;
	P2=edge2;

/* compute center point PM */ 
	NLOOP(high)
	{
		PM->L.x=(P1->L.x + P2->L.x)/2;
		PM->L.y=(P1->L.y + P2->L.y)/2;
		PM->L.z=(P1->L.z + P2->L.z)/2;
		PM->L.w=(P1->L.w + P2->L.w)/2.0;
		PM->L.R=(P1->L.R + P2->L.R)/2;
		PM->L.G=(P1->L.G + P2->L.G)/2;
		PM->L.B=(P1->L.B + P2->L.B)/2;
		PM->L.A=(P1->L.A + P2->L.A)/2;
		PM->L.F=(P1->L.F + P2->L.F)/2;

		if(PM->L.w==0.0) PM->L.w=0.00001; 			
		u= ( (P1->L.u*P1->L.w) + (P2->L.u*P2->L.w) ) /2.0 ;
		PM->L.u=(u/ PM->L.w);

		v= ( (P1->L.v*P1->L.w) + (P2->L.v*P2->L.w) ) /2.0 ;
		PM->L.v=(v/ PM->L.w);

		PM->W.large=P1->W.large/2;
		P1->W.large=P1->W.large - PM->W.large;

		P1++,P2++;PM++;
	}

	FillPoly(SC,edge1,edgeM,high);
	FillPoly(SC,edgeM,edge2,high);
}
/*=============================================================*/
void DrawSegmentPix(struct SOFT3D_context *SC,union pixel3D *P1,union pixel3D *P2)
{
/* draw a simple horizontal line */
register union pixel3D *edge1=SC->edge1;
register union pixel3D *edge2=SC->edge2;
register union pixel3D *temp;

	SFUNCTION(DrawSegmentPIX)
/* Trouver point P1 et point P2 des deux points de cette ligne */
	if (P1->W.x > P2->W.x)
		SWAP(P1,P2);

	if ( (P1->W.x<0) ou (P1->W.y<0) ou (SC->large<=P1->W.x) ou (SC->high<=P1->W.y) ou (P2->W.x<0) ou (P2->W.y<0) ou (SC->large<=P2->W.x) ou (SC->high<=P2->W.y))
		return;

	CopyPix(&(edge1[P1->W.y]),P1);
	CopyPix(&(edge2[P2->W.y]),P2);
	P1=&(edge1[P1->W.y]);
	P2=&(edge2[P2->W.y]);

	FillPoly(SC,P1,P2,1);
}
/*=============================================================*/
void DrawLinePix(struct SOFT3D_context *SC)
{
register union pixel3D *P1=&SC->PolyPix[0];
register union pixel3D *P2=&SC->PolyPix[1];
register union pixel3D *edge1=SC->edge1;
register union pixel3D *edge2=SC->edge2;
register union pixel3D *temp;
register WORD LineHigh,LineLarge;
register WORD n;
BOOL EdgesOrdered;
WORD adjustx;

	SFUNCTION(DrawLinePIX)
/* Trouver point min et point max des deux points P1&P2 de cette ligne */
	if (P1->W.y > P2->W.y)
		SWAP(P1,P2);

	if ( (P1->W.x<0) ou (P1->W.y<0) ou (SC->large<=P1->W.x) ou (SC->high<=P1->W.y) ou (P2->W.x<0) ou (P2->W.y<0) ou (SC->large<=P2->W.x) ou (SC->high<=P2->W.y))
		return;

	LineLarge= P2->W.x - P1->W.x ;
	LineHigh = P2->W.y - P1->W.y ;
	if(LineLarge<0) LineLarge=-LineLarge;
	SC->PolyHigh = LineHigh+1;

	VAR(LineLarge)
	VAR(LineHigh)

	if(LineLarge+LineHigh==0)
		{DrawSimplePix(SC,P1);return;}
	if(LineLarge+LineHigh<=2)
		{DrawSimplePix(SC,P1);DrawSimplePix(SC,P2);return;}
	if(LineHigh==0)
		{DrawSegmentPix(SC,P1,P2);return;}

	PrintPix(P1);
	PrintPix(P2);

	EdgesOrdered=(P1->W.x < P2->W.x);

/* Store the 2 points in an edge (edge1) */
	CopyPix(&(edge1[P1->W.y]),P1);
	CopyPix(&(edge1[P2->W.y]),P2);

	SC->dmin=1024<<16;
	DoEdge(SC,&edge1[P1->W.y],&edge1[P2->W.y]);

	P1=edge1=&edge1[P1->W.y];
	P2=edge2=&edge2[P1->W.y];

	
	if(EdgesOrdered)
		adjustx = +1;
	else
		adjustx = -1;

	NLOOP(LineHigh)
		{
		CopyPix(P2,&P1[1]);
		P2->W.x += adjustx ;	/* avoid to have a segment with large=0 */
		P1++;
		P2++;
		}
	CopyPix(P2,P1);

	if(EdgesOrdered)
		{P1=edge1;P2=edge2;}
	else
		{P1=edge2;P2=edge1;}

	FillPoly(SC,P1,P2,SC->PolyHigh);
}
/*=============================================================*/
void DrawPolyPix(struct SOFT3D_context *SC)
{
register union pixel3D *Pix=SC->PolyPix;
register union pixel3D *edge1;
register union pixel3D *edge2;
register union pixel3D *Pmin=NULL;
register union pixel3D *Pmax=NULL;
register union pixel3D *temp;
register WORD Pnb=SC->PolyPnb;
register WORD n;
register WORD PolyHigh;
register WORD PolyLarge;
register WORD PolyYmin;
register WORD PolyYmax;

	SFUNCTION(DrawPolyPix)
	VAR(SC->PolyPnb)
	PolyYmin=PolyYmax=Pix[0].W.y;


/* Polygon loop : draw all edges */
	SC->dmin=1024<<16;
NLOOP(Pnb)
{
	PrintPix(&Pix[n]);
	Pmin =&(Pix[n]);
	Pmax =&(Pix[n+1]);
	if (n+1==Pnb)
		Pmax =&(Pix[0  ]);

/* Trouver point Pmin et point Pmax des deux points de cette ligne */
	if (Pmin->W.y != Pmax->W.y)  /* skip all horizontal lines */
	{

	if (Pmin->W.y < Pmax->W.y)
		{edge1=SC->edge1;}
	else
		{edge1=SC->edge2; SWAP(Pmin,Pmax);}

	if (Pmin->W.y < PolyYmin   )	PolyYmin=Pmin->W.y;
	if (PolyYmax  < Pmax->W.y  )	PolyYmax=Pmax->W.y;

/* Stocker les deux points dans edge1 */
	CopyPix(&(edge1[Pmin->W.y]),Pmin);
	Pmin=&(edge1[Pmin->W.y]);
	CopyPix(&(edge1[Pmax->W.y]),Pmax);
	Pmax=&(edge1[Pmax->W.y]);

/* draw this edge */
	DoEdge(SC,Pmin,Pmax);
	}
}
/* all edges drawn */

/* Sort edges */
	n=(PolyYmin+PolyYmax)/2;
	edge1=SC->edge1;
	edge2=SC->edge2;
	if (SC->edge1[n].L.x < SC->edge2[n].L.x)
		{edge1=SC->edge1;edge2=SC->edge2;}
	else
		{edge1=SC->edge2;edge2=SC->edge1;}

/* compute PolyHigh */
	PolyHigh= PolyYmax - PolyYmin ; 
	if(PolyYmax==(SC->high-1))	/* patch: for TheVague if last line of screen then dont crop it */
		PolyHigh++;			

/* if poly is only an horizontal line */
	if(PolyHigh==0)
	{
	Pmin = Pmax =Pix;
	NLOOP(Pnb)
		{
		if(Pix->W.x  < Pmin->W.x)
			Pmin=Pix;
		if(Pmax->W.x <  Pix->W.x)
			Pmax=Pix;
		Pix++;
		}
	DrawSegmentPix(SC,Pmin,Pmax);
	return;
	}

/*
	if(SC->UseTex)
*/

	if(Wazp3D.SimuPerspective.ON)
		{
		FillPolyDouble(SC ,&(edge1[PolyYmin]) ,&(edge2[PolyYmin]), &(SC->edgeM[PolyYmin]) ,PolyHigh);
		return;
		}

	FillPoly(SC,&(edge1[PolyYmin]),&(edge2[PolyYmin]),PolyHigh);
	SFUNCTION(-----------)
}
/*==================================================================================*/
void SOFT3D_Fog(struct SOFT3D_context *SC,WORD FogMode,float FogZmin,float FogZmax,float FogDensity,UBYTE *FogRGBA)
{
register float d,z,f;
register UWORD n;
UBYTE A;
UBYTE *RGBA;
register UBYTE *Mul8=SC->Mul8;
register union oper3D Oper1;

#define CLAMP(X,MIN,MAX) ((X)<(MIN)?(MIN):((X)>(MAX)?(MAX):(X)))

SFUNCTION(SOFT3D_Fog)
	if(Wazp3D.DebugSOFT3D.ON)
	{
	VAR(FogMode)
	VARF(FogZmin)
	VARF(FogZmax)
	VARF(FogDensity)
	}

	if(FogMode<0)
		return;
	if(3<FogMode)
		return;

	if(FogZmin 	<  MINZ   )	FogZmin =MINZ;
	if(MAXZ     <= FogZmax)	FogZmax =MAXZ;

	SC->FogMode		=FogMode;
	SC->FogZmin		=FogZmin;
	SC->FogZmax		=FogZmax;
	SC->FogDensity	=FogDensity;
	COPYRGBA(SC->FogRGBA,FogRGBA);

	if(FogMode==0)
		return;

	Oper1.L.Index=0;

	NLOOP(FOGSIZE)
	{
	z=(((float)n)/((float)FOGSIZE));
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
	case 3:
		d = -(FogDensity*FogDensity);
		f= exp( d * z*z );
		break;
	default:
		f=0.0;
		break;
	}

	RGBA=(UBYTE *) &SC->FogRGBAs[n];
	A = 255.0*(1.0 - CLAMP( f, 0.0F, 1.0F ));
	MUL8(A,FogRGBA[0],RGBA[0])
	MUL8(A,FogRGBA[1],RGBA[1])
	MUL8(A,FogRGBA[2],RGBA[2])
	RGBA[3]=ONE-A;

	if(Wazp3D.DebugSOFT3D.ON) VARF(f);
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
	Libmemcpy(&(SC->DumpP[SC->DumpPnum]),&(SC->PolyP[0]),PSIZE*SC->PolyPnb);
	SC->DumpFnum+=1;
	SC->DumpPnum+=SC->PolyPnb;
	}
#endif
}
/*================================================================*/
void DrawPolyP(struct SOFT3D_context *SC)
{
union   pixel3D *Pix=SC->PolyPix;
struct point3D *P=SC->PolyP;
float PolyZmin,PolyZmax;
WORD Pnb,n;
BOOL BackFace=FALSE;

#ifdef WAZP3DDEBUG
if(Wazp3D.DebugSOFT3D.ON) Libprintf("DrawPolyP Pnb %ld  \n",SC->PolyPnb);
#endif

	if(SC->PolyPnb > MAXPOLY )	{SREM(Poly has too much Points!); return;}
	P=SC->PolyP;

	if(SC->PolyPnb > 1)
		ClipPoly(SC);

	Pnb=SC->PolyPnb;
	if(Pnb>=3)
		BackFace=! (0.0 > (P[1].x - P[0].x)*(P[2].y - P[0].y)-(P[2].x - P[0].x)*(P[1].y - P[0].y));
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON) Libprintf(" Backface %ld Culling %ld\n",BackFace,SC->Culling);
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

	COPYRGBA(SC->FlatRGBA,SC->PolyP->RGBA); 		/* default: flat-color come from current points */
	if(Wazp3D.HackRGBA2.ON)					/* in this case use face center as flat color */
	if(Pnb>=3)
		UVtoRGBA(SC->ST,(P[0].u+P[1].u+P[2].u)/3.0,(P[0].v+P[1].v+P[2].v)/3.0,SC->FlatRGBA);

	PolyZmin=MAXZ; PolyZmax=MINZ;
	SC->ColorChange=FALSE;
	SC->ColorTransp=FALSE;


	NLOOP(Pnb)
	{
	if (P->z < PolyZmin)	PolyZmin=P->z;
	if (PolyZmax < P->z)	PolyZmax=P->z;

	if(Wazp3D.HackRGBA1.ON)
		UVtoRGBA(SC->ST,P->u,P->v,P->RGBA);

	Pix->L.x	= 0;	/* erase low parts */
	Pix->L.y	= 0;
	Pix->W.x	= P->x;
	Pix->W.y	= P->y;
	ZCONVERSION
	Pix->L.w	= P->w;

	Pix->L.F	= 0;
	Pix->W.F	= (P->z * (float)FOGSIZE);
	if(P->z<MINZ) Pix->W.F=0;
	if(MAXZ<P->z) Pix->W.F=FOGSIZE-1;


	Pix->L.u	=(P->u+0.5)*2.0;	/* add 0.5 to texture coordinate for better rounding then mul by 2 to keep an integer */
	Pix->L.v	=(P->v+0.5)*2.0;
	Pix->L.u	=Pix->L.u<<15;	/* already multiplied per 2 so only shift per 15 */ 
	Pix->L.v	=Pix->L.v<<15;

	Pix->W.R	= P->RGBA[0];
	Pix->W.G	= P->RGBA[1];
	Pix->W.B	= P->RGBA[2];
	Pix->W.A	= P->RGBA[3];

/* new update-area */
	if(Pix->W.x < SC->Pxmin) SC->Pxmin=Pix->W.x;
	if(SC->Pxmax < Pix->W.x) SC->Pxmax=Pix->W.x;
	if(Pix->W.y < SC->Pymin) SC->Pymin=Pix->W.y;
	if(SC->Pymax < Pix->W.y) SC->Pymax=Pix->W.y;
	if(ZVALUE < SC->Pzmin) SC->Pzmin=ZVALUE;
	if(SC->Pzmax < ZVALUE) SC->Pzmax=ZVALUE;

	if(NOTSAMERGBA(SC->PolyP->RGBA,P->RGBA))	/* if color truly change ==> do gouraud */ 
		{SREM(ColorChange);SC->ColorChange=TRUE;}

	if(SC->PolyP->RGBA[3]!=P->RGBA[3])		/* if alpha change ==> do alpha */ 
		SC->ColorTransp=TRUE;

	Pix++;
	P++;
	}

	if(SC->PolyP->RGBA[3]!=255)		/* if not solid ==> do alpha */ 
		SC->ColorTransp=TRUE;

/* This face is inside Fog ? */
	if(SC->FogMode==0)
		SC->UseFog=FALSE;
	if(SC->FogMode==1)
	{
	if(PolyZmax	   < SC->FogZmin) SC->UseFog=FALSE;
	if(SC->FogZmax <	PolyZmin  ) SC->UseFog=FALSE;	  /* end of fogging area */
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
	{
	Libprintf("LinearFog:UseFog%ld. FogZmin FogZmax PolyZmin PolyZmax:",SC->UseFog);
	pf(SC->FogZmin);pf(SC->FogZmax);pf(PolyZmin);pf(PolyZmax);
	Libprintf("\n");
	}
#endif
	}
	if(!Wazp3D.UseFog.ON) SC->UseFog=FALSE;

	SOFT3D_SetDrawFunctions(SC);		/* draw functions still can change now if ColorChange ColorTransp UseFog just changed */

	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DumpObject.ON)
		DumpPoly(SC);

	switch(Pnb)
	{
	case 1:  DrawPointPix(SC); break;
	case 2:  DrawLinePix(SC);  break;
	default: DrawPolyPix(SC);  break;
	}

#ifdef WAZP3DDEBUG
	if(Wazp3D.StepDrawPoly.ON) 
		{
		DrawFragBuffer(SC);	/* flush buffer = finish this poly */
		NLOOP(Pnb)
			ImagePlot(SC,SC->PolyPix[n].W.x,SC->PolyPix[n].W.y);
		LibAlert("DrawPolyP() done !!");
		}
#endif

}
/*=============================================================*/
void DrawTriP(struct SOFT3D_context *SC,register struct point3D *A,register struct point3D *B,register struct point3D *C)
{
	CopyP(&(SC->PolyP[0]),A);
	CopyP(&(SC->PolyP[1]),B);
	CopyP(&(SC->PolyP[2]),C);
	SC->PolyPnb=3;
	DrawPolyP(SC);
}
/*=============================================================*/
void UVtoRGBA(struct SOFT3D_texture *ST,float u,float v,UBYTE *RGBA)
{
register UBYTE *Tex8;
UBYTE u8,v8;

	SREM(UVtoRGBA)
	if(ST==NULL) return;
	PrintST(ST);

	u8=u;
	v8=v;
	Tex8=ST->MMs[0].Tex8U[u8]+ST->MMs[0].Tex8V[v8];

	RGBA[0]=Tex8[0];
	RGBA[1]=Tex8[1];
	RGBA[2]=Tex8[2];
	RGBA[3]=255;
	if (ST->bits==32)
		RGBA[3]=Tex8[3];
	if(Wazp3D.DebugSOFT3D.ON) PrintRGBA((UBYTE *)RGBA);
}
/*=============================================================*/
#ifdef WAZP3DDEBUG
void ImagePlot(struct SOFT3D_context *SC,UWORD x,UWORD y)
{
UBYTE *RGB;
ULONG offset;

	if(Wazp3D.DebugSOFT3D.ON) Libprintf("ImagePlot x y %ld %ld \n",x,y);
	offset=(SC->large*y + x);
	RGB=(UBYTE *)&(SC->Image32[offset]);
	RGB[0]=10;
	RGB[1]=255;
	RGB[2]=10;
}
/*=============================================================*/
void TexturePlot(struct SOFT3D_texture *ST,UWORD x,UWORD y,UBYTE *ColorRGBA)
{
UBYTE *RGB;
ULONG offset;

	if(x<ST->large)
	if(y<ST->high )
	{
	offset=(ST->large*y + x)  * ST->bits / 8;
	RGB=&(ST->pt[offset]);
	RGB[0]=ColorRGBA[0];
	RGB[1]=ColorRGBA[1];
	RGB[2]=ColorRGBA[2];
	if(ST->bits==32)
		RGB[3]=ColorRGBA[3];
	}
}
/*=============================================================*/
void TexturePrint(struct SOFT3D_texture *ST,WORD x,WORD y,UBYTE *texte)
{
UBYTE *F;
UBYTE RGBA[4];
UWORD m,n,c;
UBYTE Bit[] = {128,64,32,16,8,4,2,1};
#define FONTSIZE 8
#define FONTLARGE (128/8)

	RGBA[3]=255;
	while(*texte!=0)
	{
	c=*texte++;
	if(32<c) c=c-32; else c=0; 
	F=&(font8x8[(c AND 15) + (c/16)*FONTLARGE*FONTSIZE]);
		MLOOP(FONTSIZE)
		{
		NLOOP(FONTSIZE)
			{
			RGBA[0]=RGBA[1]=RGBA[2]=0;
			if (F[0] AND Bit[n]) 
				RGBA[0]=RGBA[1]=RGBA[2]=255;
			TexturePlot(ST,x+n,y+m,RGBA);
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
WORD MaxPolyHack;
#define W3D_PRIMITIVE_POLYGON	9999

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
	{
	Libprintf("SOFT3D_DrawPrimitive Pnb %ld (%ld)\n",Pnb,primitive);
	if(SC->ST!=NULL) Libprintf(" %s\n",SC->ST->name);
	}
#endif

	if(SC->Image32==NULL) 
		{ REM(Image is lost or freed: cant draw !!); return;}		/* happen if SOFT3D_ReallocImage() fail */

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

	MaxPolyHack=0;
	if(Wazp3D.UsePolyHack.ON)
		MaxPolyHack=MAXPOLYHACK;
	if(Wazp3D.UsePolyHack2.ON)
		MaxPolyHack=MAXPOLYHACK2;

	if(Pnb<=MaxPolyHack)				/* a simple quad after a clipping in x y z can have now more than 4 points */
	{
	if(primitive==W3D_PRIMITIVE_TRIFAN)
		primitive=W3D_PRIMITIVE_POLYGON;

	if(!Wazp3D.UsePolyHack2.ON)
	if(primitive==W3D_PRIMITIVE_TRISTRIP)
		{
		if(Pnb==3)
		{
		CopyP(&(SC->PolyP[0]),&P[0]);
		CopyP(&(SC->PolyP[1]),&P[2]);
		CopyP(&(SC->PolyP[2]),&P[1]);
		}

		if(Pnb==4)
		{
		CopyP(&(SC->PolyP[0]),&P[0]);
		CopyP(&(SC->PolyP[1]),&P[2]);
		CopyP(&(SC->PolyP[2]),&P[3]);
		CopyP(&(SC->PolyP[3]),&P[1]);
		}

		if(Pnb==5)
		{
		CopyP(&(SC->PolyP[0]),&P[0]);
		CopyP(&(SC->PolyP[1]),&P[2]);
		CopyP(&(SC->PolyP[2]),&P[4]);
		CopyP(&(SC->PolyP[3]),&P[3]);
		CopyP(&(SC->PolyP[4]),&P[1]);
		}

		if(Pnb==6)
		{
		CopyP(&(SC->PolyP[0]),&P[0]);
		CopyP(&(SC->PolyP[1]),&P[2]);
		CopyP(&(SC->PolyP[2]),&P[4]);
		CopyP(&(SC->PolyP[3]),&P[5]);
		CopyP(&(SC->PolyP[4]),&P[3]);
		CopyP(&(SC->PolyP[5]),&P[1]);
		}

		if(Pnb==7)
		{
		CopyP(&(SC->PolyP[0]),&P[0]);
		CopyP(&(SC->PolyP[1]),&P[2]);
		CopyP(&(SC->PolyP[2]),&P[4]);
		CopyP(&(SC->PolyP[3]),&P[5]);
		CopyP(&(SC->PolyP[4]),&P[6]);
		CopyP(&(SC->PolyP[5]),&P[3]);
		CopyP(&(SC->PolyP[6]),&P[1]);
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
		CopyP(&(SC->PolyP[0]),&P[n]);
		SC->PolyPnb=1;
		DrawPolyP(SC);
		return;
		}


	if(primitive==W3D_PRIMITIVE_LINES)
	{
	nb=Pnb/2;
	NLOOP(nb)
		{
		CopyP(&(SC->PolyP[0]),&P[2*n]);
		CopyP(&(SC->PolyP[1]),&P[2*n+1]);
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
		CopyP(&(SC->PolyP[0]),&P[n]);
		CopyP(&(SC->PolyP[1]),&P[n+1]);
		SC->PolyPnb=2;
		DrawPolyP(SC);
		}

	CopyP(&(SC->PolyP[0]),&P[nb]);
	CopyP(&(SC->PolyP[1]),&P[0 ]);
	SC->PolyPnb=2;
	DrawPolyP(SC);
	return;
	}

	if(primitive==W3D_PRIMITIVE_LINESTRIP)
	{
	nb=Pnb-1;
	NLOOP(nb)
		{
		CopyP(&(SC->PolyP[0]),&P[n]);
		CopyP(&(SC->PolyP[1]),&P[n+1]);
		SC->PolyPnb=2;
		DrawPolyP(SC);
		}
	return;
	}

	if(primitive==W3D_PRIMITIVE_POLYGON	)
	{
	NLOOP(Pnb)
		{
		CopyP(&(SC->PolyP[n]),&P[n]);
		}
	SC->PolyPnb=Pnb;
	DrawPolyP(SC);
	}
}
/*=============================================================*/
void SOFT3D_SetPointSize(struct SOFT3D_context *SC,UWORD PointSize)
{
register WORD *PointLarge=SC->PointLarge;
register WORD x,y,D;

SFUNCTION(SOFT3D_SetPointSize)
	VAR(PointSize);
	if(PointSize!=SC->PointSize)
	{
	D=PointSize;
	YLOOP(PointSize)
	XLOOP(D)
		if( ( (2*y-D)*(2*y-D) + 2*2*x*x) < D*D ) PointLarge[y]=x*2;
	SC->PointSize=PointSize;
	}
}
/*=================================================================*/
void TextureAlphaUsage(struct SOFT3D_texture *ST)
{
ULONG size,n,AllAnb,RatioA;
ULONG Anb[256];
UBYTE *RGBA;
UBYTE A;
#define RATIOALPHA 20

SFUNCTION(TextureAlphaUsage)
	if(ST->bits==32)
	{
	ST->BlendMode='A';	/* alpha texture with blending */

		if(Wazp3D.UseRatioAlpha.ON)	/* count alpha-pixels  */
		{
		NLOOP(256)
			Anb[n]=0;
		AllAnb=0;
		size=ST->large*ST->high;
		RGBA=ST->pt;
		NLOOP(size)
			{
			A=RGBA[3];
			Anb[A]++; 
			if(MINALPHA < A)				/* if source visible   ? */
			if(A < MAXALPHA)				/* if source not solid ? */
				AllAnb++;
			RGBA+=4;
			}

		if(!Wazp3D.UseAlphaMinMax.ON)			/* then all A values are transparents, except 0 & 255 */ 
			AllAnb=size-Anb[0]-Anb[255];

		if(AllAnb!=0) RatioA=(100*AllAnb)/size; else RatioA=0;

		if(RatioA<RATIOALPHA)
			ST->BlendMode='a';

		if(Anb[255] == size) ST->BlendMode='R';	/* all pixels solid ?*/
		}
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("Tex: %ld pixels A=0:%ld A=255:%ld A=All:%ld RatioA=%ld %\n",size,Anb[0],Anb[255],AllAnb,RatioA);
#endif
	}

}
/*==================================================================*/
void *SOFT3D_CreateTexture(struct SOFT3D_context *SC,UBYTE *pt,UWORD large,UWORD high,UWORD bits,BOOL MipMapped)
{
struct SOFT3D_texture *ST;
UBYTE *Tex8V;
UBYTE *ptmm;
float TexelsPerU,TexelsPerV,nf;
UWORD BytesPerTexel,BytesPerLine,n,m;
UBYTE BitmapName[40];
LONG Tsize,MMsize;

SFUNCTION(SOFT3D_CreateTexture)
	ST=MYmalloc(sizeof(struct SOFT3D_texture),"SOFT3D_texture");
	if(ST==NULL) return(NULL);

	ST->pt	=pt;
	ST->large	=large;
	ST->high	=high;
	ST->bits	=bits;
	ST->BlendMode='R';	/* default */
/* add to linkage */
	ST->nextST =SC->firstST;
	SC->firstST=ST;

	TextureAlphaUsage(ST);

/* Create texture index */
	BytesPerTexel=bits/8;
	BytesPerLine =ST->large*BytesPerTexel;

	TexelsPerU=((float)ST->large)/256.0;
	TexelsPerV=((float)ST->high )/256.0;
	Tex8V=ST->pt;

	Tsize=ST->large*ST->high*ST->bits/8;

	if(MipMapped)
		CreateMipmaps(ST);

	ptmm=ST->ptmm;
	MMsize=MAXTEXTURE*MAXTEXTURE*ST->bits/8;
	MLOOP(10)
	{
		NLOOP(256)
		{
		nf=(float)n;
		ST->MMs[m].Tex8U[n]=       (ULONG)(	BytesPerTexel * (UWORD)(TexelsPerU*nf));
		ST->MMs[m].Tex8V[n]=Tex8V +(ULONG)( BytesPerLine  * (UWORD)(TexelsPerV*nf));
		ST->MMs[m].Tex8Ulow[n]=    (ULONG)(	BytesPerTexel * (UWORD)(TexelsPerU*nf/256.0));
		ST->MMs[m].Tex8Vlow[n]=    (ULONG)( BytesPerLine  * (UWORD)(TexelsPerV*nf/256.0));
		if(TexelsPerU<=1.0)
				ST->MMs[m].Tex8Ulow[n]=0;
		if(TexelsPerV<=1.0)
				ST->MMs[m].Tex8Vlow[n]=0;
		}

		if(ST->MipMapped)
		{
		if(Tsize > MMsize) 	/* for smaller texture-models use mipmaps */ 
		{
		Tex8V=ptmm;
		TexelsPerU=TexelsPerU/2.0;
		TexelsPerV=TexelsPerV/4.0;
		ptmm=ptmm+MMsize;
		}
		MMsize=MMsize/4;
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
		Libsavefile(ST->name,ST->pt,ST->large*ST->high*BytesPerTexel);

	Libsprintf(ST->name,"Texture%ld_%ldX%ldX%ld.RAW",ST->Tnum,ST->large,ST->high,ST->bits);

	Libsprintf(BitmapName,"Tex%ld %lc",ST->Tnum,ST->BlendMode);

	if(Wazp3D.ReloadTextures.ON)
		Libloadfile(ST->name,ST->pt,ST->large*ST->high*BytesPerTexel);

	if(Wazp3D.SmoothTextures.ON)
		SmoothTexture(ST->pt,ST->large,ST->high,SC->AliasedLines,ST->bits);

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugTexNumber.ON)
		TexturePrint(ST,0,ST->high/2,BitmapName);
	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("%s is done, BlendMode(%lc)\n",ST->name,ST->BlendMode);
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
		FREEPTR(ST->pt2);
		FREEPTR(ST->ptmm);
		break;
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
struct button3D *ButtonsList=(struct button3D *)&Wazp3D.HardwareLie;

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
	Wazp3D.NextStepFunction=50;
	Wazp3D.CrashFunctionCall=0;		/*to start the step-by-step with this call */

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

	Libstrcpy(Wazp3D.HardwareLie.name,"+HardwareDriver Lie");
	Libstrcpy(Wazp3D.TexFmtLie.name,"+TexFmt Lie");
	Libstrcpy(Wazp3D.HackARGB.name,"+Hack ARGB texs");
	Libstrcpy(Wazp3D.UseRatioAlpha.name,"+Use RatioAlpha(20%)");
	Libstrcpy(Wazp3D.UseAlphaMinMax.name,"+Use AlphaMin&Max");
	Libstrcpy(Wazp3D.DirectBitmap.name,"+Directly draw in Bitmap");
	Libstrcpy(Wazp3D.OnlyTrueColor.name,"+Only TrueColor 24&32");
	Libstrcpy(Wazp3D.SmoothTextures.name,"+Smooth Textures");
	Libstrcpy(Wazp3D.DoMipMaps.name,"+Do MipMaps");
	Libstrcpy(Wazp3D.ReloadTextures.name,"+Reload new Textures");

	Libstrcpy(Wazp3D.UseColoringGL.name,"Use Coloring GL");
	Libstrcpy(Wazp3D.UseFog.name,"Use Fog");
	Libstrcpy(Wazp3D.UseColoring.name,"Use Coloring");
	Libstrcpy(Wazp3D.NoPerspective.name," No   Perspective");
	Libstrcpy(Wazp3D.SimuPerspective.name,"Simu. Perspective");
	Libstrcpy(Wazp3D.UsePolyHack.name,"Use Poly Hack");
	Libstrcpy(Wazp3D.UsePolyHack2.name,"Use Poly Hack more");
	Libstrcpy(Wazp3D.UseColorHack.name,"Use BGcolor Hack");
	Libstrcpy(Wazp3D.UseCullingHack.name,"Use Culling Hack");
	Libstrcpy(Wazp3D.UseClearDrawRegion.name,"Use ClearDrawRegion");
	Libstrcpy(Wazp3D.UseClearImage.name,"Use Clear Image");
	Libstrcpy(Wazp3D.UseMinUpdate.name,"Use Min. Update");
	Libstrcpy(Wazp3D.HackRGBA1.name,"Tex as RGBA1 Hack");
	Libstrcpy(Wazp3D.HackRGBA2.name,"Tex as RGBA2 Hack");
	Libstrcpy(Wazp3D.UseAntiImage.name,"AntiAlias Image");

	Libstrcpy(Wazp3D.QuakePatch.name,"Quake Patch");

	Libstrcpy(Wazp3D.DebugWazp3D.name,">>> Enable Debugger >>>");
	Libstrcpy(Wazp3D.DisplayFPS.name,"Display FPS");
	Libstrcpy(Wazp3D.DebugFunction.name,"Debug Function");
	Libstrcpy(Wazp3D.StepFunction.name, "[Step]  Function");
	Libstrcpy(Wazp3D.DebugCalls.name,"Debug Calls");
	Libstrcpy(Wazp3D.DebugAdresses.name,"Debug Adresses");
	Libstrcpy(Wazp3D.DebugAsJSR.name,"Debug as JSR");
	Libstrcpy(Wazp3D.DebugVar.name,"Debug Var name ");
	Libstrcpy(Wazp3D.DebugVal.name,"Debug Var value");
	Libstrcpy(Wazp3D.DebugDoc.name,"Debug Var doc  ");
	Libstrcpy(Wazp3D.DebugState.name,"Debug State");
	Libstrcpy(Wazp3D.DebugDriver.name,"Debug Driver");
	Libstrcpy(Wazp3D.DebugContext.name,"Debug Context");
	Libstrcpy(Wazp3D.DebugTexture.name,"Debug Texture");
	Libstrcpy(Wazp3D.DebugTexNumber.name,"Debug Tex number");
	Libstrcpy(Wazp3D.DebugPoint.name,"Debug Point");
	Libstrcpy(Wazp3D.DebugError.name,"Debug Error");
	Libstrcpy(Wazp3D.DebugWC.name,"Debug WC");
	Libstrcpy(Wazp3D.DebugWT.name,"Debug WT");
	Libstrcpy(Wazp3D.DebugSOFT3D.name,"Debug SOFT3D");
	Libstrcpy(Wazp3D.StepUpdate.name,"[Step] Update");
	Libstrcpy(Wazp3D.StepDrawPoly.name,"[Step] DrawPoly");
	Libstrcpy(Wazp3D.StepSOFT3D.name, "[Step]  SOFT3D");
	Libstrcpy(Wazp3D.StepFunction50.name, "[Step] 50 Functions");
	Libstrcpy(Wazp3D.DebugSC.name,"Debug SC");
	Libstrcpy(Wazp3D.DebugST.name,"Debug ST");
	Libstrcpy(Wazp3D.DebugSepiaImage.name,"Debug Sepia Image");
	Libstrcpy(Wazp3D.DebugClipper.name,"Debug Clipper");
	Libstrcpy(Wazp3D.DumpTextures.name,"Dump Textures");
	Libstrcpy(Wazp3D.DumpObject.name,"Dump Object");
	Libstrcpy(Wazp3D.ResizeDumpedObject.name,"Resize Dumped Object");
	Libstrcpy(Wazp3D.DebugMemList.name,"Debug MemList");
	Libstrcpy(Wazp3D.DebugMemUsage.name,"Debug MemUsage");

	ButtonCount=((LONG)&Wazp3D.DebugMemUsage -(LONG)&Wazp3D.HardwareLie)/sizeof(struct button3D)+1;
	NLOOP(ButtonCount)
		ButtonsList[n].ON=FALSE;

	Wazp3D.HardwareLie.ON=TRUE;
	Wazp3D.TexFmtLie.ON=TRUE;
	Wazp3D.HackARGB.ON=TRUE;
	Wazp3D.UsePolyHack.ON=TRUE;
	Wazp3D.UseColorHack.ON=TRUE;
	Wazp3D.UseClearDrawRegion.ON=TRUE;
	Wazp3D.UseClearImage.ON=TRUE;
	Wazp3D.UseMinUpdate.ON=TRUE;
	Wazp3D.UseRatioAlpha.ON=TRUE;
	Wazp3D.UseAlphaMinMax.ON=TRUE;


#if defined(__AROS__)
	Wazp3D.UseColoringGL.ON=TRUE;		/* we assume that AROS' cpu is fast enough to enable nice features */
	Wazp3D.SimuPerspective.ON=TRUE;
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
	if(Wazp3D.DebugCalls.ON)
		LibAlert("List of calls");

	NLOOP(88)
		if(Wazp3D.DebugCalls.ON)
			{
			if (n==40)	LibAlert("More calls...");
			Libprintf("[%ld] %ld\tcalls to %s \n",1+n,Wazp3D.FunctionCalls[1+n],Wazp3D.FunctionName[1+n]);
			}
	if(Wazp3D.DebugCalls.ON)
		LibAlert("All calls listed");

	if(Wazp3D.DebugAdresses.ON)
		 PrintAllFunctionsAdresses();
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
UWORD RGBA16;
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

/* A1=one bit alpha. If this alpha is one, the pixel is fully opaque. If the alpha is zero, the pixel is invisible/fully transparent. */            
if(format==W3D_A1R5G5B5)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA16=color8[0]*256+color8[1];
		a=(RGBA16 >> 15) *   255 ;
		r=(RGBA16 >> 10) << (8-5);
		g=(RGBA16 >>  5) << (8-5);
		b=(RGBA16 >>  0) << (8-5);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA[3]=a;
		RGBA+=4;
		color8+=2;
		}
	RGBA16+=offset1;
	RGBA   +=offset2;
	}

if(format==W3D_R5G6B5)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA16=color8[0]*256+color8[1];
		r=(RGBA16 >> 11) << (8-5);
		g=(RGBA16 >>  5) << (8-6);
		b=(RGBA16 >>  0) << (8-5);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA+=3;
		color8+=2;
		}
	RGBA16+=offset1;
	RGBA   +=offset2;
	}

if(format==W3D_A4R4G4B4)
	YLOOP(high)
	{
		XLOOP(large)
		{
		RGBA16=color8[0]*256+color8[1];
		a=(RGBA16 >> 12) << (8-4);
		r=(RGBA16 >>  8) << (8-4);
		g=(RGBA16 >>  4) << (8-4);
		b=(RGBA16 >>  0) << (8-4);
		RGBA[0]=r;
		RGBA[1]=g;
		RGBA[2]=b;
		RGBA[3]=a;
		RGBA+=4;
		color8+=2;
		}
	RGBA16+=offset1;
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
register float c0,c1;
register float u0w,v0w,u1w,v1w;

	P->x= PN[0].x + t * (PN[1].x - PN[0].x) ;
	P->y= PN[0].y + t * (PN[1].y - PN[0].y) ;
	P->z= PN[0].z + t * (PN[1].z - PN[0].z) ;
	P->w= PN[0].w + t * (PN[1].w - PN[0].w) ;

/* compute the new U V with a true-perspective */
	if(PN[0].w!=PN[1].w)
	{
	if(P->w==0.0) P->w=0.00001; 			
	u0w=PN[0].u*PN[0].w;
	v0w=PN[0].v*PN[0].w;
	u1w=PN[1].u*PN[1].w;
	v1w=PN[1].v*PN[1].w;
	P->u= (u0w + t * (u1w - u0w) )/P->w;
	P->v= (v0w + t * (v1w - v0w) )/P->w;
	}
	else
	{
	P->u= PN[0].u + t * (PN[1].u - PN[0].u) ;
	P->v= PN[0].v + t * (PN[1].v - PN[0].v) ;
	}

	c0=PN[0].RGBA[0]; c1=PN[1].RGBA[0]; P->RGBA[0]=c0 + t * (c1 - c0); 
	c0=PN[0].RGBA[1]; c1=PN[1].RGBA[1]; P->RGBA[1]=c0 + t * (c1 - c0); 
	c0=PN[0].RGBA[2]; c1=PN[1].RGBA[2]; P->RGBA[2]=c0 + t * (c1 - c0); 
	c0=PN[0].RGBA[3]; c1=PN[1].RGBA[3]; P->RGBA[3]=c0 + t * (c1 - c0); 
}
/*=================================================================*/
#define NewPointClipX(xlim) {ClipPoint(PN,P,(xlim - PN[0].x) / (PN[1].x - PN[0].x)); P->x=xlim; P++; NewPnb++; }
#define NewPointClipY(ylim) {ClipPoint(PN,P,(ylim - PN[0].y) / (PN[1].y - PN[0].y)); P->y=ylim; P++; NewPnb++; }
#define NewPointClipZ(zlim) {ClipPoint(PN,P,(zlim - PN[0].z) / (PN[1].z - PN[0].z)); P->z=zlim; P++; NewPnb++; }
#define CopyPoint(A)  {CopyP(P,A); P++; NewPnb++; }
#define SwapBuffers useT1=!useT1; if(useT1) {PN=T1; P=T2;} else	{PN=T2; P=T1;}
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
	Libmemcpy(PN,SC->PolyP,Pnb*PSIZE);

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
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; CopyP(&(PN[Pnb]),PN);
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
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; CopyP(&(PN[Pnb]),PN);
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
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; CopyP(&(PN[Pnb]),PN);
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
FaceClipped=TRUE; IsInside[Pnb]=IsInside[0]; CopyP(&(PN[Pnb]),PN);
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

	if(FaceClipped==FALSE) return;

/* if clipped get the new PolyPnb & PolyP */

	SC->PolyPnb=Pnb;
	if(useT1) PN=T1; else PN=T2;

	if(Wazp3D.DebugClipper.ON)
	{
	VARF(SC->ClipMin.x);
	VARF(SC->ClipMax.x);
	VARF(SC->ClipMin.y);
	VARF(SC->ClipMax.y);
	VARF(SC->ClipMin.z);
	VARF(SC->ClipMax.z);


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
	Libmemcpy(SC->PolyP,PN,SC->PolyPnb*PSIZE);
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

		if(Wazp3D.DebugFunction.ON)
			RGB2[1]=255/ratio;

		if(bits==32)
			RGB2[3]=a;
		RGB2=&(RGB2[P]);
		}
	}

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
	ST->ptmm=MYmalloc(size/3,"mipmaps"); 		/* size is mathematically false but allways fit */
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

	if(MAXPRIM<WC->Pnb) return;
	P=&(WC->P[WC->Pnb]);
	WC->Pnb++;
	P->x=V->x;
	P->y=V->y;
	P->z=(float)V->z;
	P->w=V->w;
	P->u=V->u * WC->uresize;
	P->v=V->v * WC->vresize;
	ColorToRGBA(P->RGBA,V->color.r,V->color.g,V->color.b,V->color.a);

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
ULONG  UVoffsetw	=context->TPWOffs[unit];

UBYTE *C		=context->ColorPointer;
ULONG  Cformat	=context->CPMode;
ULONG  Cstride	=context->CPStride;

UBYTE *pt;
struct VertexFFF *fff;
struct VertexFFD *ffd;
struct VertexDDD *ddd;
float *u;
float *v;
float *w;
float *rgbaF;
UBYTE *rgbaB;
UBYTE  RGBA[4];
struct point3D *P;

	if(MAXPRIM<WC->Pnb) return;
	P=&(WC->P[WC->Pnb]);
	P->x=P->y=P->z=P->u=P->v=0.0;
	COPYRGBA(P->RGBA,WC->CurrentRGBA);	/* default: if a point dont have a color value then it use CurrentColor */
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

	P->w=1.0/P->z;
	}


/* recover UV values */
	if(V!=NULL)
	{
	pt=&(UV[i*UVstride]);
	u=(float *)pt;
	v=(float *)&pt[UVoffsetv];
	w=(float *)&pt[UVoffsetw];

	if(UVformat==W3D_TEXCOORD_NORMALIZED)
	{
	P->u=u[0] * 256.0;
	P->v=v[0] * 256.0;
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
		ColorToRGBA(RGBA,rgbaF[0],rgbaF[1],rgbaF[2],rgbaF[3]);
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
	SREM(SaveMTL)
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

	SREM(SaveOBJ)
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

	if(Wazp3D.ResizeDumpedObject.ON)
	PLOOP(Pnb)
		{ P[p].x=(2.0*P[p].x/SC->large)-1.0; P[p].y=(2.0*P[p].y/SC->high)-1.0; P[p].z=(2.0*P[p].z)-1.0; }

	PLOOP(Pnb)
		{ Libprintf("v  ");  pf(P[p].x); pf(P[p].y); pf(P[p].z); Libprintf("\n"); }
	PLOOP(Pnb)
		{ Libprintf("vt  "); pf(P[p].u); pf(P[p].v); Libprintf("\n"); }
	PLOOP(Pnb)
		{ Libprintf("vn  "); pf(1.0); pf(1.0); pf(1.0); Libprintf("\n"); }		/* dont compute vertices' normals = simpler */

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

	if(SC->DumpStage==0)	
		{
		Libprintf("DumpObject(0/3): reset\n");
		SC->DumpFnum=SC->DumpPnum=SC->DumpFnb=SC->DumpPnb=0;
		SC->DumpF=NULL;
		SC->DumpP=NULL;
		SC->DumpStage=1;return;
		}

	if(SC->DumpStage==1)	
		{
		Libprintf("DumpObject(1/3): count&alloc\n");
		VAR(SC->DumpFnb)
		VAR(SC->DumpPnb)
		if(SC->DumpFnb==0) {SC->DumpStage=0;return;}		/* wait a frame that drawn something */
		SC->DumpPnb=SC->DumpPnb+SC->DumpPnb/2;			/* alloc 150% the previous size */
		SC->DumpFnb=SC->DumpFnb+SC->DumpFnb/2;
		SC->DumpP=MYmalloc(SC->DumpPnb*PSIZE,"DumpP");
		SC->DumpF=MYmalloc(SC->DumpFnb*sizeof(struct  face3D),"DumpF");
		if(SC->DumpP!=NULL) 	
		if(SC->DumpF!=NULL) 
			{SC->DumpStage=2; return;}	/* buffers ok ? then continue */
		SC->DumpStage=3;				/* else free all */
		}

	if(SC->DumpStage==2)	
		{
		if(SC->DumpFnb==0) {SC->DumpStage=2;return;}		/* wait a frame that drawn something */
		Libprintf("DumpObject(1/3): dump&save\n");
		SaveOBJ(SC,"DumpWazp3D.obj");
		SC->DumpStage=3; return;
		}

	if(SC->DumpStage==3)	
		{
		Libprintf("DumpObject(1/3): free all\n");
		FREEPTR(SC->DumpP);
		FREEPTR(SC->DumpF);
		Wazp3D.DumpObject.ON=FALSE;
		SC->DumpStage=0; return;
		}

#endif
}
/*==========================================================================*/
void	 DrawText(W3D_Context *context,UBYTE *text,WORD x,WORD y)
{
struct WAZP3D_context *WC=context->driver;
struct RastPort *rp=&WC->rastport;

SREM(DrawText)
	SetDrMd(rp, JAM1);
	SetAPen(rp, 0) ; 
	RectFill(rp,x-3,y-9,x+6*strlen(text)+3,y+2); 

	SetAPen(rp, 2);
	Move(rp,x-2,y-2+context->yoffset);
	Text(rp,text, strlen(text));

	SetAPen(rp, 2);
	Move(rp,x,y+context->yoffset);
	Text(rp,text, strlen(text));

	SetAPen(rp, 1);
	Move(rp,x-1,y-1+context->yoffset);
	Text(rp,text, strlen(text));

	SetAPen(rp, 1);
}
/*==========================================================================*/
BOOL SOFT3D_DoUpdate(struct SOFT3D_context *SC)
{
UBYTE BackRGBA[4];

	if(SC->Image32==NULL) return(FALSE);
	if(SC->Pxmax==0) return(FALSE);		/* nothing to update ? */
	if(SC->Pymax==0) return(FALSE);

SREM(SOFT3D_DoUpdate)
	DrawFragBuffer(SC);
	if(Wazp3D.StepUpdate.ON) 
		LibAlert("Update will occurs now !!");
	if(Wazp3D.DirectBitmap.ON)	
		goto UpdateDone;

SREM(MinUpdate)
	if(Wazp3D.UseMinUpdate.ON)
	{
	SC->xUpdate		=SC->Pxmin;		/* should also be used to clear the previous drawing */
	SC->yUpdate		=SC->Pymin;
	SC->largeUpdate	=SC->Pxmax-SC->Pxmin+1;
	SC->highUpdate	=SC->Pymax-SC->Pymin+1;
	}
	else
	{
	SC->xUpdate		=0;
	SC->yUpdate		=0;
	SC->largeUpdate	=SC->large;
	SC->highUpdate	=SC->high;
	}

#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugSOFT3D.ON)
		Libprintf("Updated Region from %ld %ld (%ld X %ld pixels) \n",SC->xUpdate,SC->yUpdate,SC->largeUpdate,SC->highUpdate);
#endif
	VARF(SC->Pzmin)
	VARF(SC->Pzmax)

SREM(UseColorHack)
	if(Wazp3D.UseColorHack.ON)
		{
		STACKReadPixelArray(BackRGBA,0,0,1*(32/8),&SC->rastport,0,0,1,1,RECTFMT_RGBA);
		SOFT3D_SetBackColor(SC,BackRGBA);
		}

SREM(UseAntiImage)
	if(Wazp3D.UseAntiImage.ON)
		AntiAliasImage(SC->Image32,SC->large,SC->high,SC->AliasedLines);

SREM(STACKWritePixelArray)
	if(0<SC->largeUpdate)
	if(SC->largeUpdate<=SC->large)
	if(0< SC->highUpdate)
	if(SC->highUpdate <=SC->high)
	{
	VAR(SC->xUpdate)
	VAR(SC->yUpdate)
	VAR(SC->largeUpdate)
	VAR(SC->highUpdate)
	STACKWritePixelArray(SC->Image32,SC->xUpdate,SC->yUpdate,SC->large*(32/8),&SC->rastport,SC->xUpdate,SC->yUpdate+SC->yoffset,SC->largeUpdate,SC->highUpdate,RECTFMT_RGBA);
	if(Wazp3D.UseClearImage.ON)
		SOFT3D_ClearImage(SC,0,0,SC->large,SC->high);
	}

UpdateDone:
	SC->Pxmin=SC->large-1;
	SC->Pymin=SC->high -1;
	SC->Pzmin= 1000.0;
	SC->Pxmax=0;
	SC->Pymax=0;
	SC->Pzmax=-1000.0;
	return(TRUE);

}
/*==========================================================================*/
void DoUpdate(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
UBYTE name[40];
ULONG MilliTime;

	if(!SOFT3D_DoUpdate(WC->SC))
		return;
#ifdef WAZP3DDEBUG
	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DumpObject.ON)
		DumpObject(WC->SC);

	if(Wazp3D.DebugWazp3D.ON)
	if(Wazp3D.DisplayFPS.ON)
	{
SREM(DisplayFPS)
	MilliTime=LibMilliTimer();
	WC->TimePerFrame= MilliTime - WC->LastMilliTime;
	WC->LastMilliTime=MilliTime;
	if(WC->TimePerFrame!=0)
		WC->Fps=(((ULONG)1000)/WC->TimePerFrame);
	Libsprintf(name,"Wazp3D FPS:%ld  ",WC->Fps);
	DrawText(context,name,4,10);
	}
SREM(DoUpdate OK)
#endif
}
/*==========================================================================*/
W3D_Context	*W3D_CreateContext(ULONG *error, struct TagItem *taglist)
{
W3D_Context	*context;
struct WAZP3D_context *WC;
struct SOFT3D_context *SC;
ULONG tag,data;
UWORD n,bits;
ULONG EnableMask,DisableMask,supportedfmt;
ULONG AllStates=~0;
ULONG UnsupportedStates=  W3D_ANTI_POINT | W3D_ANTI_LINE | W3D_ANTI_POLYGON | W3D_DITHERING | W3D_LOGICOP | W3D_STENCILBUFFER | W3D_ALPHATEST | W3D_SPECULAR | W3D_TEXMAPPING3D | W3D_CHROMATEST;
ULONG	envsupmask=W3D_REPLACE | W3D_DECAL | W3D_MODULATE  | W3D_BLEND;	/* v40: full implementation */
BOOL stateOK=TRUE;
ULONG noerror;

#if defined(__AROS__)
	UnsupportedStates= UnsupportedStates | W3D_DOUBLEHEIGHT;
#endif

	WAZP3DFUNCTION(1);
	VAR(error)
	VAR(taglist)

	if(error==NULL) error=&noerror; /* StormMesa patch: can be NULL if you don`t want an error code returned */
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
	context->fog.fog_start		=MINZ;
	context->fog.fog_end		=MAXZ;
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
	if(tag==W3D_CC_W3DBM)		context->w3dbitmap	=data;	/* flag */

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

	InitRastPort(&WC->rastport);

	SetDrawRegion(context,context->drawregion,context->yoffset,NULL);
 
/*V39: if the bitmap (ie context->drawregion) is 32 bits then try to use it as the Wazp3D RGBA buffer */
	if(WC->bits!=32)
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


	WC->firstWT=NULL;
	WC->CallFlushFrame=WC->CallSetDrawRegion=WC->CallClearZBuffer=FALSE;
	WC->PointSize=1.0;
	WC->CallSetBlending=FALSE;

	if(Wazp3D.DirectBitmap.ON)
		{
		Wazp3D.UseColorHack.ON=FALSE;		/* Wazp3D RGBA buffer's options : uncompatible with DirectBitmap */
		Wazp3D.UseClearImage.ON=FALSE;
		Wazp3D.UseMinUpdate.ON=FALSE;
		Wazp3D.UseAntiImage.ON=FALSE;
		}

	WC->SC=SOFT3D_Start(WC->large,WC->high,context->drawregion);
	if(WC->SC==NULL) return(NULL);

	Wazp3D.UseAntiImage.ON=StateON(W3D_ANTI_FULLSCREEN);

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

	WAZP3DFUNCTION(2);
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

	WAZP3DFUNCTION(3);
	W3D_FreeAllTexObj(context);
	SOFT3D_End(WC->SC);
	VAR(WC->CallFlushFrame)
	VAR(WC->CallSetDrawRegion)
	VAR(WC->CallClearZBuffer)

	FREEPTR(context->driver);		/* context->driver=WC that also include the W3D_Context */

	if (Wazp3D.DebugContext.ON)
		LibAlert("DestroyContextOK");
}
/*==========================================================================*/
ULONG W3D_GetState(W3D_Context *context, ULONG state)
{
ULONG action;

	WAZP3DFUNCTION(4);
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
void W3D_SetFrontFace(W3D_Context* context, ULONG direction);
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(5);
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

	WAZP3DFUNCTION(6);

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
	WAZP3DFUNCTION(7);
	VAR(Wazp3D.drivertype);
	return(Wazp3D.drivertype);
}
/*==========================================================================*/
void  SOFT3D_SetBitmap(struct SOFT3D_context *SC,void  *bm,WORD x,WORD y,WORD large,WORD high)    
{
UBYTE *Image8;						/* = bitmap memory  */
ULONG format;

	if(SC==NULL) return;
SREM(SOFT3D_SetBitmap)
	SC->bm=bm;
	SC->rastport.BitMap=bm;
	SC->yoffset=y;			/* only stored for WritePixelArray() */

	if(Wazp3D.DirectBitmap.ON)
	{
	SC->bmHandle=LockBitMapTags((APTR)SC->bm,LBMI_BASEADDRESS,(ULONG)&Image8, TAG_DONE);
	format = GetCyberMapAttr(bm,CYBRMATTR_PIXFMT);
	UnLockBitMap(SC->bmHandle);

	if(format==PIXFMT_BGRA32)
		{
		SC->FunctionIn =(HOOKEDFUNCTION)PixelsInBGRA;
		SC->FunctionOut=(HOOKEDFUNCTION)PixelsOutBGRA;
		}
	if(format==PIXFMT_RGBA32)
		{
		SC->FunctionIn =(HOOKEDFUNCTION)PixelsInRGBA;
		SC->FunctionOut=(HOOKEDFUNCTION)PixelsOutRGBA;
		}
	if(format==PIXFMT_ARGB32)
		{
		SC->FunctionIn =(HOOKEDFUNCTION)PixelsInARGB;
		SC->FunctionOut=(HOOKEDFUNCTION)PixelsOutARGB;
		}
/*
not yet implemented
	if(format==PIXFMT_RGB24)
		{
		SC->FunctionIn =(HOOKEDFUNCTION)PixelsInRGB;
		SC->FunctionOut=(HOOKEDFUNCTION)PixelsOutRGB;
		}
*/
	Image8=Image8 + y*SC->large*32/8 + x*32/8;  /* Dont use yoffset in SOFT3D : use real pointer to Image */
	SC->Image32=(ULONG *)Image8;
	}

	SOFT3D_SetImage(SC,large,high,SC->Image32);
}
/*==========================================================================*/
BOOL LockBM(struct SOFT3D_context *SC)
{
UBYTE *Image8;						/* = bitmap memory  */

	SC->bmHandle=LockBitMapTags((APTR)SC->bm,LBMI_BASEADDRESS,(ULONG)&Image8, TAG_DONE);
	return(SC->bmHandle!=NULL);
}
/*==========================================================================*/
void UnLockBM(struct SOFT3D_context *SC)
{
	if(SC->bmHandle!=NULL)
		UnLockBitMap(SC->bmHandle);
}
/*==========================================================================*/
ULONG W3D_LockHardware(W3D_Context *context)
{
	WAZP3DFUNCTION(8);
	context->HWlocked=TRUE;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void W3D_UnLockHardware(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(9);

	if(!WC->CallFlushFrame)
	if(!WC->CallSetDrawRegion)
	if(!WC->CallClearZBuffer)
		DoUpdate(context);	/* draw the image if the usual update-functions are never called  */

	context->HWlocked=FALSE;
}
/*==========================================================================*/
void W3D_WaitIdle(W3D_Context *context)
{
	WAZP3DFUNCTION(10);
}
/*==========================================================================*/
ULONG W3D_CheckIdle(W3D_Context *context)
{
	WAZP3DFUNCTION(11);
	WRETURN(W3D_SUCCESS);		/* else W3D_BUSY */
}
/*==========================================================================*/
ULONG QueryDriver(W3D_Driver* driver, ULONG query, ULONG destfmt)
{
ULONG support=W3D_NOT_SUPPORTED;
UBYTE sup='N';

	if(driver!=NULL)				/*patch: StormMesa can send driver=NULL */
	if(driver!=&Wazp3D.driver) return(support);

	VAR(destfmt)
	VAR(query)

	if( destfmt != 0)			/*patch: StormMesa can send this undocumented destfmt=0 meaning all dest_formats */
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
	WAZP3DFUNCTION(12);
	return(QueryDriver(&Wazp3D.driver,query,destfmt));	/* only one driver */
}
/*==========================================================================*/
ULONG W3D_GetTexFmtInfo(W3D_Context *context, ULONG texfmt, ULONG destfmt)
{
	WAZP3DFUNCTION(13);
	return(W3D_GetDriverTexFmtInfo(&Wazp3D.driver,texfmt,destfmt));
}
/*==========================================================================*/
ULONG W3D_GetDriverState(W3D_Context *context)
{
	WAZP3DFUNCTION(14);
	WRETURN(W3D_SUCCESS);		/* else W3D_NOTVISIBLE */
}
/*==========================================================================*/
ULONG W3D_GetDestFmt(void)
{
	WAZP3DFUNCTION(15);
	return(Wazp3D.driver.formats);
}
/*==========================================================================*/
W3D_Driver **   W3D_GetDrivers(void)
{
	WAZP3DFUNCTION(16);
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

	WAZP3DFUNCTION(18);
	if(driver!=NULL)				/*patch: StormMesa can send driver=NULL */
	if(driver!=&Wazp3D.driver) return(support);

	if(destfmt==0)				/*patch: StormMesa can send this undocumented destfmt=0 meaning all dest_formats */
		support=supported;

	if(texfmt==W3D_R8G8B8)
		support=supported;

	if(texfmt==W3D_R8G8B8A8)
		support=supported;

	if(Wazp3D.TexFmtLie.ON)
		support=supported;

	if(destfmt !=0)				/*patch: StormMesa can send this undocumented destfmt=0 meaning all dest_formats */
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

	WAZP3DFUNCTION(19);
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
		ModeID=INVALID_ID;
		if(AslRequest(requester, NULL))
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

	WAZP3DFUNCTION(20);
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

	WAZP3DFUNCTION(21);
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
UWORD bits=0;
ULONG mask=1;
ULONG noerror;

	WAZP3DFUNCTION(22);
	if(error==NULL) error=&noerror; /* patch: StormMesa send error as NULL cause it don`t want an error code returned */

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
		{FREEPTR(texture->driver); *error=W3D_NOMEMORY; return(NULL); }			 /* texture->driver=WT */

	if(!Wazp3D.TexFmtLie.ON)
	if(!texture->matchfmt)
		{FREEPTR(texture->driver); *error=W3D_UNSUPPORTEDTEXFMT; return(NULL); }	 /* texture->driver=WT */

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

	WT->ST=SOFT3D_CreateTexture(WC->SC,WT->pt,WT->large,WT->high,WT->bits,Wazp3D.DoMipMaps.ON);

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

	WAZP3DFUNCTION(23);
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
struct WAZP3D_texture *WT;
struct WAZP3D_texture fakeWT;
struct WAZP3D_texture *thisWT=&fakeWT;
WORD Ntexture=0;


	WAZP3DFUNCTION(24);
	VAR(texture)
	if(texture==NULL) return; 	/* patch: StormMesa use this undocumented NULL value for texture */
	WT=texture->driver;
	PrintWT(WT);

	thisWT->nextWT=WC->firstWT;
	while(thisWT!=NULL)
	{
	VAL(Ntexture)
	if(thisWT->nextWT==WT)
		{
		SREM(is texture found)
		if(thisWT->nextWT==WC->firstWT)
			WC->firstWT=WT->nextWT;
		else
			thisWT->nextWT=WT->nextWT;
		W3D_ReleaseTexture(context,texture);
		SOFT3D_FreeTexture(WC->SC,WT->ST);
		FREEPTR(texture->driver);	 		/* texture->driver=WT */
		break;
		}
	thisWT=thisWT->nextWT;
	Ntexture++;
	}

	PrintAllT(context);
}
/*==========================================================================*/
void W3D_ReleaseTexture(W3D_Context *context, W3D_Texture *texture)
{

	WAZP3DFUNCTION(25);
	VAR(texture)
	if(texture==NULL) return; 	/* patch: StormMesa use this undocumented NULL value for texture */
	if(texture->texdata==NULL) SREM(texture got no converted texdata);
	FREEPTR(texture->texdata);
}
/*==========================================================================*/
void W3D_FlushTextures(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;

	WAZP3DFUNCTION(26);
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

	WAZP3DFUNCTION(27);
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
struct WAZP3D_texture emptyWT;
struct WAZP3D_texture *WT;


	WAZP3DFUNCTION(28);
	if(texture!=NULL) WT=texture->driver; else WT=&emptyWT; 	/* patch: StormMesa use this undocumented NULL value for texture */

	if(envcolor!=NULL)
		ColorToRGBA(WT->EnvRGBA,envcolor->r,envcolor->g,envcolor->b,envcolor->a);

	WT->TexEnvMode=envparam;

	if(StateON(W3D_GLOBALTEXENV))
	{
	context->globaltexenvmode=envparam;			/* Global texture environment mode */
	if(envcolor!=NULL)					/* envcolor is only specified when envparam == W3D_BLEND so can be NULL */
	{
	context->globaltexenvcolor[0]=envcolor->r;	/* global texture env color */
	context->globaltexenvcolor[1]=envcolor->g;
	context->globaltexenvcolor[2]=envcolor->b;
	context->globaltexenvcolor[3]=envcolor->a;
	}
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

	WAZP3DFUNCTION(29);
	WT->WrapRepeatX=(s_mode==W3D_REPEAT);
	WT->WrapRepeatY=(t_mode==W3D_REPEAT);
	if(bordercolor!=NULL)
		ColorToRGBA(WT->WrapRGBA,bordercolor->r,bordercolor->g,bordercolor->b,bordercolor->a);

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

	WAZP3DFUNCTION(30);
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


	WAZP3DFUNCTION(31);
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
	WAZP3DFUNCTION(32);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FreeAllTexObj(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
struct WAZP3D_texture *WT;
struct WAZP3D_texture *nextWT;
WORD Ntexture=0;

	WAZP3DFUNCTION(33);

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

	WAZP3DFUNCTION(34);
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

	WAZP3DFUNCTION(35);
	WC->Pnb=0;
	SetTexture(context,line->tex);
	GetVertex(WC,&line->v1);
	GetVertex(WC,&line->v2);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawPoint(W3D_Context *context, W3D_Point *point)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(36);
	WC->PointSize=point->pointsize;
	 if(WC->PointSize<1.0)	WC->PointSize=1.0;	/* PATCH: skulpt dont set pointsize*/
	WC->Pnb=0;
	SetTexture(context,point->tex);		/* ??? tex not used */
	GetVertex(WC,&point->v1);

	SOFT3D_SetPointSize(WC->SC,(UWORD)WC->PointSize);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,1,W3D_PRIMITIVE_POINTS);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriangle(W3D_Context *context, W3D_Triangle *triangle)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(37);
	WC->Pnb=0;
	SetTexture(context,triangle->tex);
	GetVertex(WC,&triangle->v1);
	GetVertex(WC,&triangle->v2);
	GetVertex(WC,&triangle->v3);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIANGLES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriFan(W3D_Context *context, W3D_Triangles *triangles)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
LONG n;;

	WAZP3DFUNCTION(38);
	v=triangles->v;
	WC->Pnb=0;
	SetTexture(context,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,v++);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIFAN);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriStrip(W3D_Context *context, W3D_Triangles *triangles)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
LONG n;;

	WAZP3DFUNCTION(39);
	v=triangles->v;
	WC->Pnb=0;
	SetTexture(context,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,v++);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRISTRIP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_Flush(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(40);
	DrawFragBuffer(WC->SC);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLineStrip(W3D_Context *context, W3D_Lines *lines)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
LONG n;;

	WAZP3DFUNCTION(41);
	v=lines->v;
	WC->Pnb=0;
	SetTexture(context,lines->tex);
NLOOP(lines->vertexcount)
	GetVertex(WC,v++);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINESTRIP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawLineLoop(W3D_Context *context, W3D_Lines *lines)
{
struct WAZP3D_context *WC=context->driver;
W3D_Vertex *v;
LONG n;;

	WAZP3DFUNCTION(42);
	v=lines->v;
	WC->Pnb=0;
	SetTexture(context,lines->tex);
NLOOP(lines->vertexcount)
	GetVertex(WC,v++);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_LINELOOP);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ClearDrawRegion(W3D_Context *context, ULONG color)
{
struct WAZP3D_context *WC=context->driver;
ULONG ARGB32=color;
UBYTE *ARGB=(UBYTE *)&ARGB32;
UBYTE RGBA[4];

	WAZP3DFUNCTION(43);
	RGBA[0]=ARGB[1];
	RGBA[1]=ARGB[2];
	RGBA[2]=ARGB[3];
	RGBA[3]=ARGB[0];

	if(!Wazp3D.UseColorHack.ON)
		SOFT3D_SetBackColor(WC->SC,RGBA);

	if(Wazp3D.UseClearDrawRegion.ON)
		FillPixelArray(&WC->rastport,0,0+context->yoffset,WC->large,WC->high,color);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetAlphaMode(W3D_Context *context, ULONG mode, W3D_Float *refval)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(44);
	WC->AlphaMode=mode;
	WC->AlphaRef=(UBYTE)(*refval*256.0); 
	if(1.0<=*refval)	WC->AlphaRef=255;
	if(*refval<=0.0)	WC->AlphaRef=0;	

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

	WAZP3DFUNCTION(45);
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

/* patch: BlitzQuake/MiniGL use SetBlendMode but forget to activate with SetState() the blending */
	if(!WC->CallSetBlending)
		SetState(context,W3D_BLENDING,TRUE);
	
/* backdoor: For "The Vague" DiskMag */
	if(WC->SrcFunc==W3D_ZERO)
	if(WC->DstFunc==W3D_ZERO)
		{
		Wazp3D.UseClearImage.ON=FALSE;	/* allways draw on the buffer  without clearing*/
		Wazp3D.UseColoringGL.ON=TRUE;		/* allways do true coloring */
		}

	WRETURN(W3D_SUCCESS);
}

/*==========================================================================*/
void ZbufferCheck(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	if(WC->NeedNewZbuffer)
	{
		SREM(Reallocating a new Zbuffer)
		W3D_FreeZBuffer(context);
		W3D_AllocZBuffer(context);
		VAR(context->zbuffer)
	} 
}
/*==========================================================================*/
void SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor)
{
struct WAZP3D_context *WC=context->driver;
W3D_Bitmap  *w3dbm;
BOOL SameSize;

SREM(SetDrawRegion)
/* change bitmap */
	if(bm==NULL) return;
SREM(setting the bitmap)
	if(context->w3dbitmap)
	{
	w3dbm=(W3D_Bitmap  *)bm;
	context->width  = w3dbm->width;				/* bitmap width  */
	context->height = w3dbm->height;				/* bitmap height */
	context->depth  = GetBitMapAttr(bm, BMA_DEPTH  );	/* bitmap depth  */
	context->bprow  = w3dbm->bprow;				/* bytes per row */
	context->format = w3dbm->format;				/* bitmap format (see below) */
	context->drawmem= w3dbm->dest;				/* base address for drawing operations */
	}
	else
	{
	context->width  = GetBitMapAttr( bm, BMA_WIDTH  );	/* bitmap width  */
	context->height = GetBitMapAttr( bm, BMA_HEIGHT );	/* bitmap height */
	context->depth  = GetBitMapAttr( bm, BMA_DEPTH  );	/* bitmap depth  */
	context->bprow  = bm->BytesPerRow;
	if(context->depth==24)	 context->format=W3D_FMT_R8G8B8;	/* is it allways correct ??? */
	if(context->depth==32)	 context->format=W3D_FMT_A8R8G8B8;
	context->format = 0;
	context->drawmem=NULL;
	}

	WC->bits=context->bprow/context->width*8;
	context->drawregion=bm;
	WC->rastport.BitMap=bm;

/* change yoffset */
	if(StateON(W3D_DOUBLEHEIGHT))
		context->height=context->height/2;	/* if doubleheight use only the half height for the Wazp3D RGBA buffer*/
	else
		yoffset=0;					/* if cant do double-height (ie AROS) then let this to 0 */
	context->yoffset=yoffset;

/* store new (?) size */
	SameSize=((WC->large==context->width) et (WC->high==context->height));
	WC->large=context->width;
	WC->high =context->height;
	VAR(WC->large)
	VAR(WC->high)

/* change scissor */
	if(scissor==NULL)
	{
SREM(no scissor yet )
	context->scissor.left	=0;
	context->scissor.top	=0;
	context->scissor.width	=context->width;
	context->scissor.height	=context->height;
	}
	else
	{
SREM(with scissor )
	Libmemcpy(&context->scissor,scissor,sizeof(W3D_Scissor));
	}
	VAR(context->scissor.left)
	VAR(context->scissor.top)
	VAR(context->scissor.width)
	VAR(context->scissor.height)

	WC->Xmin=context->scissor.left;
	WC->Ymin=context->scissor.top;
	WC->Xmax=WC->Xmin+context->scissor.width-1;
	WC->Ymax=WC->Ymin+context->scissor.height-1;

	if(WC->Xmin<0)		WC->Xmin=0;
	if(WC->Ymin<0)		WC->Ymin=0;
	if(WC->large<=WC->Xmax)	WC->Xmax=WC->large-1;
/*	if(WC->high <=WC->Ymax)	WC->Ymax=WC->high-1;	*/
	/* patch: for thevague */
	if(WC->high < WC->Ymax)	WC->Ymax=WC->high;	

	if(WC->SC==NULL) return;			/* exit if SOFT3D not yet started */

/* SOFT3D: change bitmap  & scissor */
	SOFT3D_SetBitmap(WC->SC,bm,0,yoffset,WC->large,WC->high);
	SOFT3D_SetClipping(WC->SC,(float)WC->Xmin,(float)WC->Xmax,(float)WC->Ymin,(float)WC->Ymax,MINZ,MAXZ);

/* if same size = nothing more to do */
	if(SameSize)
		return;

/* if not same size = change Image & query for a  new Zbuffer */
	SREM(Bitmap size changed !!!!)
	if(!Wazp3D.DirectBitmap.ON)			/* then use an Image (Wazp3D's RGBA buffer) */
	{
		SREM(Not DirectBitmap so need new Image)
		SOFT3D_ReallocImage(WC->SC,WC->large,WC->high);

	} 

	if(context->zbuffer!=NULL)			/* if use a Zbuffer */
	{
		SREM(Will need a new Zbuffer)
		WC->NeedNewZbuffer=TRUE;
	} 

}
/*==========================================================================*/
ULONG W3D_SetDrawRegion(W3D_Context *context, struct BitMap *bm,int yoffset, W3D_Scissor *scissor)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(46);
	VAR(bm)
	VAR(yoffset)
	VAR(scissor->left)
	VAR(scissor->top)
	VAR(scissor->width)
	VAR(scissor->height)

	WC->CallSetDrawRegion=TRUE;
/* draw the image if needed */
	if(!WC->CallFlushFrame)		/* W3D_FlushFrame is the better time to do the Update */
		DoUpdate(context);

/* do nothing if the prog always call this function with same parameters */
	if(context->drawregion		==bm			)
	if(context->yoffset		==yoffset		)
	if(context->scissor.left	==scissor->left	)
	if(context->scissor.top		==scissor->top	)
	if(context->scissor.width	==scissor->width	)
	if(context->scissor.height	==scissor->height	)
		WRETURN(W3D_SUCCESS);		/* nothing to do ===> return */

	SetDrawRegion(context,bm,yoffset,scissor);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetDrawRegionWBM(W3D_Context *context, W3D_Bitmap *bm,W3D_Scissor *scissor)
{
ULONG error;

	WAZP3DFUNCTION(47);
	context->w3dbitmap=TRUE;
	error=W3D_SetDrawRegion(context,(struct BitMap *)bm,context->yoffset,scissor);
	WRETURN(error);
}
/*==========================================================================*/
ULONG W3D_SetFogParams(W3D_Context *context, W3D_Fog *fogparams,ULONG fogmode)
{
/* 
The fields fog_start and fog_end must be given if linear fog is used.
These values are in 'w-space', meaning 1.0 is the front plane, and 0.0 is the back plane.
The density field is only used in exponential fog modes
*/
struct WAZP3D_context *WC=context->driver;


	WAZP3DFUNCTION(48);
	WC->FogMode=0;
	if(fogmode==W3D_FOG_LINEAR)	WC->FogMode=1;
	if(fogmode==W3D_FOG_EXP)	WC->FogMode=2;
	if(fogmode==W3D_FOG_EXP_2)	WC->FogMode=3;

	ColorToRGBA(WC->FogRGBA,fogparams->fog_color.r,fogparams->fog_color.g,fogparams->fog_color.b,1.0);

	if(0.0!=fogparams->fog_start)
	WC->FogZmin		=1.0/fogparams->fog_start;

	if(0.0!=fogparams->fog_end)
	WC->FogZmax		=1.0/fogparams->fog_end;

	if(fogparams->fog_start==1.0)
		WC->FogZmin=MINZ;
	if(fogparams->fog_end  ==0.0)
		WC->FogZmax=MAXZ;

	if(WC->FogZmin<MINZ)
		WC->FogZmin=MINZ;
	if(MAXZ<WC->FogZmax)
		WC->FogZmax=MAXZ;

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

	WAZP3DFUNCTION(49);
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

	WAZP3DFUNCTION(50);
	VAR(Red)
	VAR(Green)
	VAR(Blue)
	VAR(Alpha)
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

	WAZP3DFUNCTION(51);
	VAR(pen)
	WC->PenMask=pen;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetCurrentColor(W3D_Context *context, W3D_Color *color)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(52);
	if(color!=NULL)
		ColorToRGBA(WC->CurrentRGBA,color->r,color->g,color->b,color->a);

	SOFT3D_SetCurrentColor(WC->SC,WC->CurrentRGBA);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetCurrentPen(W3D_Context *context, ULONG pen)
{
struct WAZP3D_context *WC=context->driver;
UBYTE RGBA[4];

	WAZP3DFUNCTION(53);
/* horrible hack to recover the pen as RGBA TODO: find a better method*/
	SetAPen (&WC->rastport,pen);
	RectFill(&WC->rastport,0,0,1,1);
	STACKReadPixelArray(RGBA,0,0,1*(32/8),&WC->rastport,0,0,1,1,RECTFMT_RGBA);

	COPYRGBA(WC->CurrentRGBA,RGBA);
	SOFT3D_SetCurrentColor(WC->SC,WC->CurrentRGBA);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void W3D_SetScissor(W3D_Context *context, W3D_Scissor *scissor)
{
	WAZP3DFUNCTION(54);
	if(context->scissor.left	==scissor->left	)
	if(context->scissor.top		==scissor->top	)
	if(context->scissor.width	==scissor->width	)
	if(context->scissor.height	==scissor->height	)
		return;			/* nothing to do ===> return */

	SetDrawRegion(context,context->drawregion,context->yoffset,scissor);
}
/*==========================================================================*/
void W3D_FlushFrame(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(55);
	WC->CallFlushFrame=TRUE;
	DoUpdate(context);
}
/*==========================================================================*/
ULONG W3D_AllocZBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(56);
	if(context->zbufferalloc==TRUE)
		WRETURN(W3D_SUCCESS);	/* already allocated ? */

	context->zbuffer=MYmalloc(WC->large*WC->high*sizeof(ZBUFF),"Zbuffer");
	if(context->zbuffer==NULL)
 		WRETURN(W3D_NOGFXMEM);

	context->zbufferalloc=TRUE;
	context->zbufferlost =FALSE;		 /* Is it TRUE if just allocated ?!? */

	WC->NeedNewZbuffer=FALSE;
	SOFT3D_SetZbuffer(WC->SC,context->zbuffer);
	SOFT3D_ClearZBuffer(WC->SC,1.0);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_FreeZBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(57);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	context->zbufferalloc=FALSE;
	context->zbufferlost =TRUE;	 /* Is it TRUE if just freed ?!? */
	FREEPTR(context->zbuffer);
	SOFT3D_SetZbuffer(WC->SC,NULL);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ClearZBuffer(W3D_Context *context, W3D_Double *clearvalue)
{
struct WAZP3D_context *WC=context->driver;
float z=1.0;

	WAZP3DFUNCTION(58);
	ZbufferCheck(context);
	WC->CallClearZBuffer=TRUE;
	if(clearvalue!=NULL)
		z=*clearvalue;
	VARF(z);
	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER);
	SOFT3D_ClearZBuffer(WC->SC,z);
	DoUpdate(context);			/*draw the image if any */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ReadZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *z)
{
	WAZP3DFUNCTION(59);
	VAR(x);
	VAR(y);
	VARF((float)*z);
	return(W3D_ReadZSpan(context,x,y,1,z));
}
/*==========================================================================*/
ULONG W3D_ReadZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(60);
	ZbufferCheck(context);
	VAR(x);
	VAR(y);
	VAR(n);
	VAR(z);
	if(context->zbuffer==NULL)
		WRETURN(W3D_NOZBUFFER);

	SOFT3D_ReadZSpan(WC->SC,x,y,n,z);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_SetZCompareMode(W3D_Context *context, ULONG mode)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(61);
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

	WAZP3DFUNCTION(62);
	VAR(x);
	VAR(y);
	VARF((float)*z);
/*	return(W3D_WriteZSpan(context,x,y,1,z,&mask)); curiously W3D_WriteZSpan dont return an ULONG */
	W3D_WriteZSpan(context,x,y,1,z,&mask);
}
/*==========================================================================*/
void W3D_WriteZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *zD, UBYTE *mask)
{
struct WAZP3D_context *WC=context->driver;

	WAZP3DFUNCTION(63);
	ZbufferCheck(context);
/*	if(context->zbuffer==NULL) WRETURN(W3D_NOZBUFFER); */
	if(context->zbuffer==NULL) return;

	VAR(x);
	VAR(y);
	VAR(n);
	VAR(mask)

	SOFT3D_WriteZSpan(WC->SC,x,y,n,zD,mask);

/*	WRETURN(W3D_SUCCESS);*/
}
/*==========================================================================*/
ULONG W3D_AllocStencilBuffer(W3D_Context *context)
{
struct WAZP3D_context *WC=context->driver;
ULONG clearvalue[1];

	WAZP3DFUNCTION(64);
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

	WAZP3DFUNCTION(65);
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

	WAZP3DFUNCTION(66);
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

	WAZP3DFUNCTION(67);
	if(context->stencilbuffer==NULL) WRETURN(W3D_NOSTENCILBUFFER);
	FREEPTR(context->stencilbuffer);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_ReadStencilPixel(W3D_Context *context, ULONG x, ULONG y,ULONG *st)
{
	WAZP3DFUNCTION(68);
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

	WAZP3DFUNCTION(69);
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

	WAZP3DFUNCTION(70);
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

	WAZP3DFUNCTION(71);
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

	WAZP3DFUNCTION(72);
	VAR(mask)
	WC->StencilWriteMask=mask;
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_WriteStencilPixel(W3D_Context *context, ULONG x, ULONG y, ULONG st)
{
UBYTE mask=1;
ULONG stencilvalue=st;

	WAZP3DFUNCTION(73);
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

	WAZP3DFUNCTION(74);
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

	WAZP3DFUNCTION(75);
	WC->Pnb=0;
	SetTexture(context,triangle->tex);
	GetVertex(WC,triangle->v1);
	GetVertex(WC,triangle->v2);
	GetVertex(WC,triangle->v3);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIANGLES);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriFanV(W3D_Context *context, W3D_TrianglesV *triangles)
{
struct WAZP3D_context *WC=context->driver;
LONG n;

	WAZP3DFUNCTION(76);
	WC->Pnb=0;
	SetTexture(context,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,triangles->v[n]);

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,W3D_PRIMITIVE_TRIFAN);
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawTriStripV(W3D_Context *context, W3D_TrianglesV *triangles)
{
struct WAZP3D_context *WC=context->driver;
LONG n;

	WAZP3DFUNCTION(77);
	WC->Pnb=0;
	SetTexture(context,triangles->tex);
NLOOP(triangles->vertexcount)
	GetVertex(WC,triangles->v[n]);

	ZbufferCheck(context);
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

	WAZP3DFUNCTION(78);
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
	WAZP3DFUNCTION(79);
	/* list is included in WC so cant be freed */
}
/*==========================================================================*/
ULONG		 W3D_BestModeID(struct TagItem *taglist)
{
ULONG tag,data;
ULONG driver,ModeID;
ULONG large,high,bits;

	WAZP3DFUNCTION(80);
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

	WAZP3DFUNCTION(81);
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

	WAZP3DFUNCTION(82);
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

	WAZP3DFUNCTION(83);
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

	WAZP3DFUNCTION(84);
	VAR(pointer)
	VAR(stride)
	WINFO(format,W3D_COLOR_FLOAT," ")
	WINFO(format,W3D_COLOR_UBYTE," ")
	WINFO(mode,W3D_CMODE_RGB," ")
	WINFO(mode,W3D_CMODE_BGR," ")
	WINFO(mode,W3D_CMODE_RGBA," ")
	WINFO(mode,W3D_CMODE_ARGB," ")
	WINFO(mode,W3D_CMODE_BGRA," ")
	VAR(format)
	VAR(mode)
	VAR(flags)

	context->ColorPointer=pointer;	/* Pointer to the color array */
	context->CPStride=stride;		/* Color pointer stride */
	context->CPMode= mode | format;	/* Mode + color format */
	context->CPFlags=flags;			/* not yet used=0 */
	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_BindTexture(W3D_Context* context, ULONG tmu, W3D_Texture *texture)
{
	WAZP3DFUNCTION(85);
	VAR(tmu)
	VAR(texture)
	PrintTexture(texture);

	if(W3D_MAX_TMU <= tmu)
		WRETURN(W3D_ILLEGALINPUT);
	context->CurrentTex[tmu]=texture;

	SetTexture(context,texture);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
ULONG W3D_DrawArray(W3D_Context* context, ULONG primitive, ULONG base, ULONG count)
{
struct WAZP3D_context *WC=context->driver;
ULONG i=base;
ULONG n;

	WAZP3DFUNCTION(86);
	WC->primitive=primitive;
	WC->Pnb=0;
/* MiniGL bug = it change context->CurrentTex without using W3D_BindTexture */
		if(context->CurrentTex[0]!=NULL) 	SetTexture(context,context->CurrentTex[0]);
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

	ZbufferCheck(context);
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

	WAZP3DFUNCTION(87);
	WC->primitive=primitive;
	WC->Pnb=0;
/* MiniGL bug = it change context->CurrentTex without using W3D_BindTexture */
		if(context->CurrentTex[0]!=NULL) 	SetTexture(context,context->CurrentTex[0]);

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

	ZbufferCheck(context);
	SOFT3D_DrawPrimitive(WC->SC,WC->P,WC->Pnb,WC->primitive);

	WRETURN(W3D_SUCCESS);
}
/*==========================================================================*/
void		 W3D_SetFrontFace(W3D_Context* context, ULONG direction)
{
struct WAZP3D_context *WC=context->driver;
UWORD Culling=0;

	WAZP3DFUNCTION(88);
	WINFO(direction,W3D_CW ,"Front face is clockwise");
	WINFO(direction,W3D_CCW,"Front face is counter clockwise");
	context->FrontFaceOrder=direction;

	if(context->FrontFaceOrder==W3D_CW)	Culling=1;
	if(context->FrontFaceOrder==W3D_CCW)   Culling=2;
	if(!StateON(W3D_CULLFACE))		Culling=0;
	SOFT3D_SetCulling(WC->SC,Culling);
}
/*==========================================================================*/
void		 PrintAllFunctionsAdresses(void)
{
#ifdef WAZP3DDEBUG
#define  VARH(var) { Libprintf(" " #var " Adresse="); ph((ULONG)var); Libprintf("\n"); }

		
		VARH(W3D_CreateContext)	   
		VARH(W3D_DestroyContext)	   
		VARH(W3D_GetState)	   
		VARH(W3D_SetState)	   
		VARH(W3D_Hint)	   
		VARH(W3D_CheckDriver)	   
		VARH(W3D_LockHardware)	   
		VARH(W3D_UnLockHardware)	   
		VARH(W3D_WaitIdle)	   
		VARH(W3D_CheckIdle)	   
		VARH(W3D_Query)	   
		VARH(W3D_GetTexFmtInfo)	   
		VARH(W3D_GetDriverState)	   
		VARH(W3D_GetDestFmt)	   
		VARH(W3D_GetDrivers)	   
		VARH(W3D_QueryDriver)	   
		VARH(W3D_GetDriverTexFmtInfo)	   
		VARH(W3D_RequestMode)	   
		VARH(W3D_TestMode)	   
		VARH(W3D_AllocTexObj)	   
		VARH(W3D_FreeTexObj)	   
		VARH(W3D_ReleaseTexture)	   
		VARH(W3D_FlushTextures)	   
		VARH(W3D_SetFilter)	   
		VARH(W3D_SetTexEnv)	   
		VARH(W3D_SetWrapMode)	   
		VARH(W3D_UpdateTexImage)	   
		VARH(W3D_UpdateTexSubImage)	   
		VARH(W3D_UploadTexture)	   
		VARH(W3D_FreeAllTexObj)	   
		VARH(W3D_SetChromaTestBounds)	   
		VARH(W3D_DrawLine)	   
		VARH(W3D_DrawPoint)	   
		VARH(W3D_DrawTriangle)	   
		VARH(W3D_DrawTriFan)	   
		VARH(W3D_DrawTriStrip)	   
		VARH(W3D_Flush)	   
		VARH(W3D_DrawLineStrip)	   
		VARH(W3D_DrawLineLoop)	   
		VARH(W3D_ClearDrawRegion)
		VARH(W3D_SetAlphaMode)	   
		VARH(W3D_SetBlendMode)	   
		VARH(W3D_SetDrawRegion)	   
		VARH(W3D_SetDrawRegionWBM)	   
		VARH(W3D_SetFogParams)	   
		VARH(W3D_SetLogicOp)	   
		VARH(W3D_SetColorMask)	   
		VARH(W3D_SetPenMask)	   
		VARH(W3D_SetCurrentColor)	   
		VARH(W3D_SetCurrentPen)	   
		VARH(W3D_SetScissor)	   
		VARH(W3D_FlushFrame)	   
		VARH(W3D_AllocZBuffer)	   
		VARH(W3D_FreeZBuffer)	   
		VARH(W3D_ClearZBuffer)	   
		VARH(W3D_ReadZPixel)	   
		VARH(W3D_ReadZSpan)	   
		VARH(W3D_SetZCompareMode)	   
		VARH(W3D_WriteZPixel)	   
		VARH(W3D_WriteZSpan)	   
		VARH(W3D_AllocStencilBuffer)	   
		VARH(W3D_ClearStencilBuffer)	   
		VARH(W3D_FillStencilBuffer)	   
		VARH(W3D_FreeStencilBuffer)	   
		VARH(W3D_ReadStencilPixel)	   
		VARH(W3D_ReadStencilSpan)	   
		VARH(W3D_SetStencilFunc)	   
		VARH(W3D_SetStencilOp)	   
		VARH(W3D_SetWriteMask)	   
		VARH(W3D_WriteStencilPixel)	   
		VARH(W3D_WriteStencilSpan)	   
		VARH(W3D_DrawTriangleV)	   
		VARH(W3D_DrawTriFanV)	   
		VARH(W3D_DrawTriStripV)	   
		VARH(W3D_GetScreenmodeList)	   
		VARH(W3D_FreeScreenmodeList)	   
		VARH(W3D_BestModeID)	   
		VARH(W3D_VertexPointer)	   
		VARH(W3D_TexCoordPointer)	   
		VARH(W3D_ColorPointer)	   
		VARH(W3D_BindTexture)	   
		VARH(W3D_DrawArray)	   
		VARH(W3D_DrawElements)   
		VARH(W3D_SetFrontFace)

		LibAlert("All adresses listed");
#endif
}
/*==========================================================================*/

