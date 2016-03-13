/* Wazp3D Beta 52 : Alain THELLIER - Paris - FRANCE - (November 2006 to 2012) 	*/
/* Adaptation to AROS from Matthias Rustler							*/
/* Code clean-up and library enhancements from Gunther Nikl					*/
/* LICENSE: GNU General Public License (GNU GPL) for this file				*/

/* This file contain the Warp3D & sof3d common definitions					*/

#ifndef __WAZP3D_H
#define __WAZP3D_H

#ifdef  __AROS__
#define AMIGA 1
#define PROVIDE_VARARG_FUNCTIONS defined(__AROS__)
#endif

/*==================================================================================*/
#define DRIVERNAME "Wazp3D - Alain Thellier - Paris France 2012 - Beta 52"
#define PREFSNAME  "Wazp3D-Prefs Beta 52 Alain Thellier 2012"
/*==================================================================================*/
#if defined(_WIN32)

/* define this one if you want the builtin debugger */
#define WAZP3DDEBUG 1

/* define this one if your CPU got same byteordering as Motorola */
/* #define MOTOROLAORDER 1 */

/* define this one if you build Wazp3D.library that use a separate soft3d.library  */
#define SOFT3DLIB 1

/* define this one if you want a soft3d.library that use the native sof3d.dll */
#define SOFT3DDLL 1 

/* define this one if you build a soft3d.dll that use an hardware opengl32.dll */
#define USEOPENGL 1 

#endif

#ifdef  __AROS__
#define WAZP3DDEBUG 1
#define MOTOROLAORDER 1 /* for Aros 68k/ppc case */
/* define this one if you build a Wazp3D/soft3d that use an hardware mesa.library */
#define USEOPENGL 1 
#endif

#if defined(__AROS__) && (AROS_BIG_ENDIAN == 0)
#undef MOTOROLAORDER
#endif
/*======================================================================================*/
/* fields names that changed from Warp3D V4 to V5 */
#define reserved1  pinned
#define DriverBase DriverIFace

#ifdef __amigaos4__

#define __USE_INLINE__
#define __USE_BASETYPE__
#define __USE_OLD_TIMEVAL__

/* Chip constants (5,6,7,8,9)  that changed (!!!) from Warp3D V4 to V5 */
#define	W3D_CHIP_AVENGER_LE	5
#define	W3D_CHIP_AVENGER_BE	6
#define	W3D_CHIP_PERMEDIA3	7
/* #define	W3D_CHIP_RADEON		8*/
#define	W3D_CHIP_RADEON2		9

#define	W3D_CHIP_OBSOLETE1	5
#define	W3D_CHIP_OBSOLETE2	6
#define	W3D_CHIP_RADEON		7
#define	W3D_CHIP_AVENGER		8
#define	W3D_CHIP_NAPALM		9

#endif
/*==================================================================================*/
#ifdef AMIGA 

#ifdef  __AROS__
#include <aros/symbolsets.h>
#include <proto/arossupport.h>
#endif

#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <dos/dos.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <libraries/asl.h>
#include <devices/timer.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/gadtools.h>
#include <proto/timer.h>

#ifdef  __AROS__
#include "Warp3D.h"
#else
#include <Warp3D/Warp3D.h>
#endif

#include "Warp3D_protos.h"

#ifdef W3D_MULTITEXTURE
#define WARP3DV5
#endif

#ifdef __amigaos4__
#include "cybergraphics.h"
#else
#include <cybergraphx/cybergraphics.h>
#endif

#include <proto/cybergraphics.h>

/*======================================================================================*/
#if !defined(__AROS__) && !defined(__MORPHOS__)
typedef ULONG IPTR;
#endif
/*======================================================================================*/
#ifdef SOFT3DLIB 
extern struct Library *		Soft3DBase;
#endif

#if !defined(__AROS__) 
extern struct ExecBase*		SysBase;
extern struct IntuitionBase*	IntuitionBase;
extern struct DosLibrary*	DOSBase;
extern struct GfxBase*		GfxBase;
extern struct Library*		CyberGfxBase;
extern struct Library*		UtilityBase;
extern struct Library*		GadToolsBase;
#endif

extern struct Device *		TimerBase;
extern struct timerequest       tr;

/*======================================================================================*/
#ifdef __amigaos4__
extern struct Library*          NewlibBase;
extern struct Interface*        INewlib;

extern struct Library*          P96Base;

extern struct ExecIFace*        IExec;
extern struct DOSIFace*         IDOS;
extern struct GraphicsIFace*    IGraphics;
extern struct IntuitionIFace*   IIntuition;
extern struct GadToolsIFace*    IGadTools;
extern struct CyberGfxIFace*    ICyberGfx;
extern struct TimerIFace*       ITimer;
extern struct UtilityIFace*     IUtility;
extern struct P96IFace*         IP96;

#endif
/*======================================================================================*/
#else				
#include <math.h>				/* For the DLL we include nothing */
#include <string.h>
#include <stdarg.h>
#include "soft3d_amiga_defines.h"	/* but we still need AmigaOS definitions  */
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
#define MAXPOLY	32				/* Maximum points per polygon			*/
#define MAXPOLYHACK 5				/* Maximum points per polygon in PolyHack */
#define MAXPOLYHACK2 7				/* Maximum points per polygon in PolyHack2*/
#define MAXSCREEN  2048				/* Maximum screen size 2048x2048		*/
#define MAXTEXTURE 2048				/* Maximum tex size 2048x2048 (fast=256)  */
#define MAXSTAGE 2				/* V5: multitexturing blending stages (up to 16)	*/
#define FOGSIZE 4096				/* Size for Fog values array			*/
#define MINZ 0.00000				/* min Zbuffer value				*/
#define MAXZ 0.99999				/* max Zbuffer value				*/
#define MINALPHA 8				/* Alpha range : A < MINALPHA ==> transp. */
#define MAXALPHA (255-MINALPHA)		/* Alpha range : MAXALPHA < A ==> solid	*/

#ifdef AMIGA
#define FRAGBUFFERSIZE (128*128)		/* Assume Amiga/UAE/ PC got big cache */

#ifdef __amigaos4__
#undef FRAGBUFFERSIZE
#define FRAGBUFFERSIZE (64*64)		/* Assume Amiga PPC got small cache 	*/
#endif

#ifdef  __AROS__
#undef FRAGBUFFERSIZE
#define FRAGBUFFERSIZE (256*256)		/* Assume AROS on PC got very big cache	*/
#endif

/* if not AMIGA */
#else							/* Make it use less memory on AmigaOS	*/
#define FRAGBUFFERSIZE (256*256)		/* Assume PC is fast enough to draw in	*/
#endif						/* one call a big square 256x256 pixels	*/

#define PSIZE sizeof(struct point3D)		/* Size for copying a point				*/
#define BLENDREPLACE (W3D_ONE	*16 + W3D_ZERO)
#define BLENDALPHA      (W3D_SRC_ALPHA*16 + W3D_ONE_MINUS_SRC_ALPHA)
#define BLENDFASTALPHA  187			/* 187 is an unused BlendMode			*/
#define BLENDNOALPHA    59			/* 59  is an unused BlendMode			*/

#define BLENDCHROMA    (W3D_SRC_COLOR*16 + W3D_DST_COLOR)       /* This is an unused BlendMode		*/


#define W3D_NOW	 255					/* NO W-inding : for Gallium/GL wrapping	*/
#define W3D_PRIMITIVE_POLYGON	9999			/* True polygon: for Gallium/GL wrapping	*/
#define W3D_Q_SETTINGS 9999				/* start Wazp3D-Prefs	*/

#define FLOATZBUFFER 1

#ifdef FLOATZBUFFER

#define ZBUFF float
#define ZVALUE Pix->L.z
#define ZCONVERSION Pix->L.z=P->z;
#define ZRESIZE 1.0				/* Zbuffer's conversion	from float		*/

#else

#define ZBUFF UWORD				 /* Zbuffer's format				*/
#define ZVALUE Pix->L.z				/* Z value is stored here			*/
#define ZCONVERSION Pix->L.z=FFLOOR((P->z * (float)ZRESIZE));
#define ZRESIZE 60000.0

#endif

#define DEBUGTNUM 9999				/* Activate debugger for this texture	*/

#define TRUECOLORFORMATS (W3D_FMT_R8G8B8|W3D_FMT_B8G8R8|W3D_FMT_A8R8G8B8|W3D_FMT_A8B8G8R8|W3D_FMT_R8G8B8A8|W3D_FMT_B8G8R8A8)
#define HIGHCOLORFORMATS (W3D_FMT_R5G5B5|W3D_FMT_B5G5R5|W3D_FMT_R5G5B5PC|W3D_FMT_B5G5R5PC|W3D_FMT_R5G6B5|W3D_FMT_B5G6R5|W3D_FMT_R5G6B5PC|W3D_FMT_B5G6R5PC|W3D_FMT_R8G8B8|W3D_FMT_B8G8R8|W3D_FMT_A8R8G8B8|W3D_FMT_A8B8G8R8|W3D_FMT_R8G8B8A8|W3D_FMT_B8G8R8A8)
#define ALLCOLORFORMATS  (W3D_FMT_R5G5B5|W3D_FMT_B5G5R5|W3D_FMT_R5G5B5PC|W3D_FMT_B5G5R5PC|W3D_FMT_R5G6B5|W3D_FMT_B5G6R5|W3D_FMT_R5G6B5PC|W3D_FMT_B5G6R5PC|W3D_FMT_R8G8B8|W3D_FMT_B8G8R8|W3D_FMT_A8R8G8B8|W3D_FMT_A8B8G8R8|W3D_FMT_R8G8B8A8|W3D_FMT_B8G8R8A8|W3D_FMT_CLUT)
#define ALLSTENCILMODES 0xFFFF
typedef void (*HOOKEDFUNCTION)(void *c);
#define ZMODE(ZUpdate,ZCompareMode) (ZUpdate*8 + (ZCompareMode-1))
/*==================================================================*/
struct state3D	/* v52: now all is described in a drawing state */
{
	unsigned char Changed;
	unsigned char ZMode;
	unsigned char BlendMode;
	unsigned char TexEnvMode;

	unsigned char PerspMode;
	unsigned char CullingMode;
	unsigned char FogMode;
	unsigned char UseGouraud;

	unsigned char UseTex;
	unsigned char UseFog;
	unsigned char pad3;
	unsigned char pad4;

	unsigned char CurrentRGBA[4];
	unsigned char EnvRGBA[4];
	unsigned char FogRGBA[4];
	unsigned char BackRGBA[4];

	unsigned long PointSize;
	unsigned long LineSize;
	float FogZmin;
	float FogZmax;
	float FogDensity;
	void* ST;
	unsigned long gltex;	/* GL texture */
};
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
struct face3D{
	ULONG Pnum;				/*indice on first point3D */
	UWORD Pnb;				/*point3D count*/
	void *tex;				/*Face's texture */
};
/*==================================================================================*/
struct button3D{
	UBYTE CycleNb,ON,padding1,padding2;
	char name[100];
	char *cyclenames[8];
	};
/*==================================================================================*/
union rgba3D {
struct rgba3DW{
	ULONG RGBA32;
	}  L;
struct rgba3DB{
	UBYTE RGBA[4];
	}  B;
};
/*==================================================================================*/
#ifdef WARP3DV5
#define MAXFUNCTIONS 97				/* 97 functions in V5  */
#else
#define MAXFUNCTIONS 88				/* 88 functions in V4  */
#endif
/*==================================================================================*/
BOOL LibDebug;				/* Enable Library Debugger (global)	*/
/*==================================================================================*/
struct WAZP3D_parameters{
struct button3D HardwareLie;		/* pretend to be a perfect hardware driver */
struct button3D TexFmtLie;
struct button3D HackTexs;
struct button3D UseRatioAlpha;	/* v40: force BlendMode to BLENDFAST if alpha-pixels < 20% */
struct button3D UseAlphaMinMax;	/* v40: ignore smallest (=transparent) and biggest (=solid) alpha values*/
struct button3D OnlyTrueColor;	/* only use fast RGB/RGBA screens */
struct button3D SmoothTextures;
struct button3D ReloadTextures;
struct button3D DoMipMaps;		/* v44: mipmapping emulation = dont works */
struct button3D Renderer;		/* v48: use soft3d or hardware OpenGL */

struct button3D UseColorHack;		/* get background color from bitmap */
struct button3D UseClearDrawRegion;	/* let Warp3D clear his bitmap */
struct button3D UseClearImage;	/* let Wazp3D clear his RGBA buffer */
struct button3D UseMinUpdate;

/*v47: cycle gadgets */
struct button3D PolyMode;		/* convert trifan & tristrip to quad */
struct button3D PerspMode;		/* no perspective at all = faster /do perspective emulation (quadratic method) two times per polygon's raster */
struct button3D TexMode;		/* emulate truly decal/blend/modulate */

struct button3D UseFog;
struct button3D UseAntiImage;
struct button3D UseFiltering;

struct button3D QuakeMatrixPatch;	/* v41: Force blending if 32bits tex (if 32 bits then BlendMode is BLENDALPHA or BLENDFAST allways) */
struct button3D IndirectMode;		/* v50: Else flush after each poly drawn */
struct button3D UseStateTracker;	/* v47: detect is states truly changes */

struct button3D DebugWazp3D;		/* global on/off for printing debug */
struct button3D DisplayFPS;

struct button3D DebugFunction;	/* usefull debug messages */
struct button3D DebugCalls;
struct button3D DebugState;
struct button3D DebugVar;
struct button3D DebugVal;
struct button3D DebugSOFT3D;
struct button3D DebugError;		/* if no error in Wazp3D then the crash come from elsewhere */
struct button3D DebugPoint;
struct button3D DebugClipper;

struct button3D DebugDoc;		/* useless debug messages*/
struct button3D DebugAdresses;
struct button3D DebugAsJSR;
struct button3D DebugDriver;
struct button3D DebugContext;
struct button3D DebugTexture;
struct button3D DebugWC;
struct button3D DebugWT;
struct button3D DebugSC;
struct button3D DebugST;
struct button3D DebugTexNumber;	/* v40: print texture number in bitmap */
struct button3D DebugTexColor;	/* v47: show texture number/blendmode/envmode as color */
struct button3D DebugBlendFunction;	/* v46: track the slow BlendFunctionAll() */
struct button3D DebugSepiaImage;	/* Colorize Wazp3D's RGBA buffer */
struct button3D DumpTextures;
struct button3D DumpObject;
struct button3D StepFunction;		/* Step a Warp3D function call */
struct button3D StepSOFT3D;		/* Step a SOFT3D function call */
struct button3D StepUpdate;		/* Step at update (Wazp3D's RGBA buffer-->screen) */
struct button3D StepDrawPoly;		/* V42: Step each DrawPolyP() */
struct button3D StepFunction50;	/* V44: Step 50 Warp3D functions calls */
struct button3D DebugMemList;
struct button3D DebugMemUsage;

void *window;				/* current Amiga-window at startup */ 
UBYTE PrefsIsOpened,UseDLL;		/* V47: If use the soft3d DLL for WinUAE */
#ifdef AMIGA

char  FunctionName[MAXFUNCTIONS+1][50];
ULONG FunctionCalls[MAXFUNCTIONS+1];
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

#endif

};
/*==================================================================================*/
#define et  &&
#define ou  ||
#define AND &
#define OR  |
#define FLOOP(nbre) for(f=0;f<nbre;f++)
#define ILOOP(nbre) for(i=0;i<nbre;i++)
#define JLOOP(nbre) for(j=0;j<nbre;j++)
#define PLOOP(nbre) for(p=0;p<nbre;p++)
#define MLOOP(nbre) for(m=0;m<nbre;m++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define XLOOP(nbre) for(x=0;x<nbre;x++)
#define YLOOP(nbre) for(y=0;y<nbre;y++)
#define SWAP(x,y) {temp=x;x=y;y=temp;}
static inline void COPYRGBA(APTR dst, APTR src)
{
    ULONG *_dst = (ULONG *)dst;
    ULONG *_src = (ULONG *)src;
    *_dst=*_src;
}
static inline BOOL NOTSAMERGBA(APTR dst, APTR src)
{
    BOOL retval = FALSE;
    ULONG *_dst = (ULONG *)dst;
    ULONG *_src = (ULONG *)src;
    if ((*_dst)!=(*_src))
        retval = TRUE;
    return retval;
}
static inline BOOL SAMERGBA(APTR dst, APTR src)
{
    BOOL retval = FALSE;
    ULONG *_dst = (ULONG *)dst;
    ULONG *_src = (ULONG *)src;
    if ((*_dst)==(*_src))
        retval = TRUE;
    return retval;
}
/* simpler W3D_GetState used internally */
#define StateON(s) ((context->state & s)!=0)
#define FREEPTR(ptr) {MYfree(ptr);ptr=NULL;}
#define FEXP(x)   ( (float)  exp( (double)(x) ));
#define FFLOOR(x) ( (float)floor( (double)(x) ));
/*==================================================================================*/
#ifdef WAZP3DDEBUG
#define REM(message) if(Wazp3D->DebugVal.ON) Libprintf(#message"\n");
#define STEP(tex) LibAlert(#tex);
#define SREM(message) if(Wazp3D->DebugSOFT3D.ON) Libprintf(#message"\n");
#define SFUNCTION(tex) if(Wazp3D->StepSOFT3D.ON) LibAlert(#tex); if(Wazp3D->DebugSOFT3D.ON) Libprintf(#tex "\n");
#define  WINFO(var,val,doc) if(var == val) if(Wazp3D->DebugVal.ON){if(Wazp3D->DebugVar.ON) Libprintf(" " #var "="); Libprintf(#val); if(Wazp3D->DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WTAG(val,doc) if(tag == val) if(Wazp3D->DebugVal.ON){Libprintf(" " #val); if(Wazp3D->DebugDoc.ON) Libprintf(", " #doc); Libprintf(",%ld,\n",((ULONG)data));}
#define  SINFO(var,val) if(var == val) if(Wazp3D->DebugSOFT3D.ON){Libprintf(" " #val "\n");}
#define WINFOB(var,val,doc) if(Wazp3D->DebugVal.ON) if((var&val)!=0) {Libprintf(" " #val); if(Wazp3D->DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   VAR(var) if(Wazp3D->DebugVal.ON)  {if(Wazp3D->DebugVar.ON) Libprintf(" " #var "="); Libprintf("%ld\n",((IPTR)var));}
#define  SVAR(var) if(Wazp3D->DebugSOFT3D.ON) if(Wazp3D->DebugVal.ON)  {if(Wazp3D->DebugVar.ON) Libprintf(" " #var "="); Libprintf("%ld\n",((IPTR)var));}
#define  VARF(var) if(Wazp3D->DebugVal.ON)  {if(Wazp3D->DebugVar.ON) Libprintf(" " #var "="); pf(var); Libprintf("\n"); }
#define  SVARF(var) if(Wazp3D->DebugSOFT3D.ON) if(Wazp3D->DebugVal.ON)  {if(Wazp3D->DebugVar.ON) Libprintf(" " #var "="); pf(var); Libprintf("\n"); }
#define   VAL(var) {if(Wazp3D->DebugVal.ON) Libprintf(" [%ld]",((ULONG)var));}
#define ZZ  LibAlert("ZZ stepping...");
#define XX Libprintf("XX stepping...\n");
#define  ERROR(val,doc) if(error == val) if(Wazp3D->DebugVal.ON) {if(Wazp3D->DebugVar.ON) Libprintf(" Error="); Libprintf(#val); if(Wazp3D->DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define   WRETURN(error) return(PrintError(error));
#define QUERY(q,doc,s) if(query==q) if(Wazp3D->DebugVal.ON) {sup=s;  Libprintf(" " #q); if(Wazp3D->DebugDoc.ON) Libprintf(", " #doc); Libprintf("\n");}
#define WAZP3DFUNCTION(n) WAZP3D_Function(n);
#define YYY Wazp3D->DebugWazp3D.ON=	Wazp3D->DisplayFPS.ON= Wazp3D->DebugFunction.ON= Wazp3D->DebugCalls.ON= Wazp3D->DebugState.ON= Wazp3D->DebugVar.ON= Wazp3D->DebugVal.ON= Wazp3D->DebugSOFT3D.ON= Wazp3D->DebugError.ON=	Wazp3D->DebugPoint.ON=TRUE;
#define NNN Wazp3D->DebugWazp3D.ON=	Wazp3D->DisplayFPS.ON= Wazp3D->DebugFunction.ON= Wazp3D->DebugCalls.ON= Wazp3D->DebugState.ON= Wazp3D->DebugVar.ON= Wazp3D->DebugVal.ON= Wazp3D->DebugSOFT3D.ON= Wazp3D->DebugError.ON=	Wazp3D->DebugPoint.ON=FALSE;
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
#define COPYPIX(a,b) { (a)->L.x=(b)->L.x; (a)->L.y=(b)->L.y; (a)->L.z=(b)->L.z; (a)->L.w=(b)->L.w; (a)->L.u=(b)->L.u; (a)->L.v=(b)->L.v; (a)->L.R=(b)->L.R; (a)->L.G=(b)->L.G; (a)->L.B=(b)->L.B; (a)->L.A=(b)->L.A; (a)->L.F=(b)->L.F; }
/*==================================================================================*/
#define COPYP(a,b)   { (a)->x=(b)->x; (a)->y=(b)->y; (a)->z=(b)->z; (a)->w=(b)->w; (a)->u=(b)->u; (a)->v=(b)->v; COPYRGBA((a)->RGBA,(b)->RGBA); }
/*==================================================================================*/
/* memory/debug usefull functions */
void  *MYmalloc(ULONG size,char *name);
void   MYfree(void *pt);
void PrintME(struct memory3D *ME);
void PrintP(struct point3D *P);
void PrintP2(struct point3D *P);
void PrintRGBA(UBYTE *RGBA);
void DumpMem(UBYTE *pt,LONG nb);
void pf(float x);
void *Libmalloc(ULONG size );
void Libfree(void *p);
void Libmemcpy(void *s1,void *s2,LONG n);
void Libstrcpy(void *p1,void *p2);
void Libstrcat(void *p1,void *p2);
ULONG Libstrlen(void *p);
void LibAlert(void *p1);
void Libsavefile(void *filename,void *pt,ULONG size);
void Libloadfile(void *filename,void *pt,ULONG size);
ULONG LibMilliTimer(void);
/*==================================================================================*/

#endif




