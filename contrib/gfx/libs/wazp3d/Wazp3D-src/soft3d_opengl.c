/* Wazp3D Beta 52 : Alain THELLIER - Paris - FRANCE - (November 2006 to 2012) 	*/
/* Adaptation to AROS from Matthias Rustler							*/
/* Code clean-up and library enhancements from Gunther Nikl					*/
/* LICENSE: GNU General Public License (GNU GPL) for this file				*/

/* OpenGL-wrapper	for Win32 & AROS	(soft3d -> OpenGL )					*/

/*==================================================================*/
#if defined(_WIN32)
#include <win.h>
#include <GL/gl.h>
#endif
/*==================================================================*/
#if defined(__AROS__)
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <GL/gla.h>
#include <GL/gl.h>
struct Library * GLBase = NULL;
UBYTE  overwinname[]={"Wazp3D overlay"};
#endif
/*==================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
/*=============================================================*/
/* For OpenGL we define the Warp3D types here so it dont need any Amiga includes */
#define W3D_REPLACE             1
#define W3D_DECAL               2
#define W3D_MODULATE            3
#define W3D_BLEND               4
#define W3D_Z_NEVER             1
#define W3D_Z_LESS              2
#define W3D_Z_GEQUAL            3
#define W3D_Z_LEQUAL            4
#define W3D_Z_GREATER           5
#define W3D_Z_NOTEQUAL          6
#define W3D_Z_EQUAL             7
#define W3D_Z_ALWAYS            8
#define W3D_ZERO                1       /* source + dest */
#define W3D_ONE                 2       /* source + dest */
#define W3D_SRC_COLOR           3       /* dest only */
#define W3D_DST_COLOR           4       /* source only */
#define W3D_ONE_MINUS_SRC_COLOR 5       /* dest only */
#define W3D_ONE_MINUS_DST_COLOR 6       /* source only */
#define W3D_SRC_ALPHA           7       /* source + dest */
#define W3D_ONE_MINUS_SRC_ALPHA 8       /* source + dest */
#define W3D_DST_ALPHA           9       /* source + dest */
#define W3D_ONE_MINUS_DST_ALPHA 10      /* source + dest */
#define W3D_SRC_ALPHA_SATURATE  11      /* source only */
#define W3D_CONSTANT_COLOR      12
#define W3D_ONE_MINUS_CONSTANT_COLOR 13
#define W3D_CONSTANT_ALPHA      14
#define W3D_ONE_MINUS_CONSTANT_ALPHA 15
#define W3D_CW                  0                       /* Front face is clockwise */
#define W3D_CCW                 1                       /* Front face is counter clockwise */
#define W3D_PRIMITIVE_TRIANGLES         0
#define W3D_PRIMITIVE_TRIFAN            1
#define W3D_PRIMITIVE_TRISTRIP          2
#define W3D_PRIMITIVE_POINTS            3
#define W3D_PRIMITIVE_LINES             4
#define W3D_PRIMITIVE_LINELOOP          5
#define W3D_PRIMITIVE_LINESTRIP         6
#define W3D_CHUNKY              1
#define W3D_A1R5G5B5            2
#define W3D_R5G6B5              3
#define W3D_R8G8B8              4
#define W3D_A4R4G4B4            5
#define W3D_A8R8G8B8            6
#define W3D_A8                  7
#define W3D_L8                  8
#define W3D_L8A8                9
#define W3D_I8                 10
#define W3D_R8G8B8A8           11
/*=============================================================*/
#define W3D_NOW	 255				/* NO W-inding : for Gallium/GL wrapping	*/
#define W3D_PRIMITIVE_POLYGON	9999		/* True polygon: for Gallium/GL wrapping	*/
#define BLENDREPLACE (W3D_ONE*16+W3D_ZERO)
#define BLENDALPHA   (W3D_SRC_ALPHA*16 + W3D_ONE_MINUS_SRC_ALPHA)
#define BLENDFASTALPHA  187			/* 187 is an unused BlendMode			*/
#define BLENDNOALPHA    59			/* 59  is an unused BlendMode			*/
#define BLENDCHROMA    (W3D_SRC_COLOR*16 + W3D_DST_COLOR)       /* This is an unused BlendMode		*/
/*=============================================================*/
/* For PC's OpenGL overlay: we define how is maded an Amiga's window 	*/
/* So we know the Warp3D's window so the position & size 			*/
struct Amigawindow
{
	void *NextWindow;					/* for the linked list in a screen */
	short int  LeftEdge, TopEdge;			/* screen dimensions of window */
	short int  Width, Height;			/* screen dimensions of window */
	short int  MouseY, MouseX;			/* relative to upper-left of window */
	short int  MinWidth, MinHeight;		/* minimum sizes */
	unsigned short int  MaxWidth, MaxHeight;	/* maximum sizes */
	unsigned long int  Flags;			/* see below for defines */
	void *MenuStrip;					/* the strip of Menu headers */
	unsigned char *Title;				/* the title text for this window */
	void *FirstRequest;				/* all active Requesters */
	void *DMRequest;					/* double-click Requester */
	char pad1[2];					/* count of reqs blocking Window */
	char pad2[4];					/* this Window's Screen */
	char pad3[4];					/* this Window's very own RastPort */
	char BorderLeft, BorderTop, BorderRight, BorderBottom;
/*  [...] 					 		more Amiga stuff here not used from OpenGL side */
};
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
/*=============================================================*/
struct point3D{
	float x,y,z;
	float u,v;
	float w;
	unsigned char RGBA[4];
};
/*=============================================================*/
struct TexCoord4f{
    float s,t,r,q;
};
/*==================================================================*/
#include "soft3d_opengl.h"
/*==================================================================*/
#define PSIZE sizeof(struct point3D)
#define XLOOP(nbre) for(x=0;x<nbre;x++)
#define YLOOP(nbre) for(y=0;y<nbre;y++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define REMP        if(HC->DebugHard) Libprintf
#define HFUNC(name) REMP(#name"\n");
#define REM(name)   REMP(#name"\n");
#define VAR(var)    REMP(#var "=%ld\n",var);
#define SWAPW(a) (unsigned short int )(((unsigned short int )a>>8)+((((unsigned short int )a&0xff)<<8)))
#define SWAPL(a) (unsigned long  int )(((unsigned long int )a>>24)+(((unsigned long int )a&0xff0000)>>8)+(((unsigned long int )a&0xff00)<<8)+(((unsigned long int )a&0xff)<<24))
void LibAlert(unsigned char *text);
void Libprintf(const unsigned char *string, ...);
void Libsprintf(unsigned char *buffer,const unsigned char *string, ...);
void PrintP(struct point3D *P);
void ReorderBitmap(unsigned char  *rgba,unsigned short large,unsigned short high);
/*==================================================================*/
void *currenthc=NULL;			/* to detect if need to change context */
void *currentP =NULL;			/* to detect if need to VertexPointer  */
/*==================================================================*/
#if defined(_WIN32)
extern void *winuaewnd;				/* global UAE window */
extern unsigned long int  z3offset;		/* winuae's memory offset 	*/
/*==================================================================*/
void OS_CurrentContext(void *hc)
{
/* WIN32: set current OpenGL context */
struct HARD3D_context *HC=hc;

	if(hc!=currenthc)
		{
		wglMakeCurrent (HC->hdc,HC->hglrc);
		currenthc=hc;
		}
}
/*==================================================================*/
LRESULT CALLBACK windowFuncWin32(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
/* WIN32: this function come from QuarkTex */
		switch (message) {
		case WM_NCCREATE:
			return 1;
		case WM_NCDESTROY:
			return 1;
		case WM_CREATE:
			return 0;
		case WM_DESTROY:
			return 0;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
}
/*==================================================================*/
void OS_AdjustWindow(void *hc)
{
/* WIN32: Move the "overlay" like the original amiga window */
struct HARD3D_context *HC=hc;
struct Amigawindow *awin;
int x,y,large,high;

	if(!HC->UseOverlay)
	{
		return;
	}
	else
	{
/* HC->awin is an Amiga pointer that contain Amiga WORDs */
	awin=(struct Amigawindow *)(SWAPL(HC->awin)+z3offset);
	if(awin==NULL)	return;

	x	=SWAPW(awin->LeftEdge	) + awin->BorderLeft ;
	y	=SWAPW(awin->TopEdge	) + awin->BorderTop  ;
	large =SWAPW(awin->Width)  - awin->BorderLeft - awin->BorderRight ;
	high  =SWAPW(awin->Height) - awin->BorderTop  - awin->BorderBottom;

	MoveWindow(HC->glwnd,x,y,large,high,FALSE);
	}
}
/*==================================================================*/
int OS_StartGLoverlay(void *hc,void *bm,int x, int y,int large, int high)
{
/* WIN32: Open OpenGL with an "overlay" = a new window upside the existing WinUAE's window */
/* this function come from QuarkTex */
struct HARD3D_context *HC=hc;
WNDCLASS wc;
RECT rect;
PIXELFORMATDESCRIPTOR p;
int format;
unsigned char name[50];

	if (!HC->instance) HC->instance = GetModuleHandle(0);
	if (HC->registered) UnregisterClass("Wazp3D", HC->instance);

	GetClientRect(winuaewnd, &rect);
	x =x + rect.left +2 ;
	y =y + rect.top  +17;

	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = windowFuncWin32;
	wc.hInstance = HC->instance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = "Wazp3D";

	if (!RegisterClass(&wc)) { REMP("Warning: Could not register Window Class"); return 0; }
	HC->registered = 1;

	if (!(HC->glwnd = CreateWindowEx(0, "Wazp3D", "", WS_CHILD | WS_VISIBLE, x, y, large, high, winuaewnd, 0, 0, 0))) REMP("Warning: Could not create Window");
	if (!(HC->hdc = GetDC(HC->glwnd))) { REMP("Warning: Could not get Device Context"); return 0; }

	memset(&p, 0, sizeof(PIXELFORMATDESCRIPTOR));
	p.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	p.nVersion = 1;

	if(HC->UseDoubleBuffer)
		p.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	else
		p.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL ;

	p.dwLayerMask = PFD_MAIN_PLANE;
	p.iPixelType = PFD_TYPE_RGBA;
	p.cColorBits = 32;
	p.cDepthBits = 16;
	p.cStencilBits = 8;

	if ((format = ChoosePixelFormat(HC->hdc, &p)) == 0) { REMP("Warning: Could not choose pixel format"); return 0; }

	DescribePixelFormat(HC->hdc,format,sizeof(PIXELFORMATDESCRIPTOR), &p);
	if(p.iPixelType = PFD_TYPE_COLORINDEX)
		sprintf(name,"LUT8");
	if(p.iPixelType = PFD_TYPE_RGBA )
		sprintf(name,"R%ld<<%ld+G%ld<<%ld+B%ld<<%ld+A%ld<<%ld",p.cRedBits,p.cRedShift,p.cGreenBits,p.cGreenShift,p.cBlueBits,p.cBlueShift,p.cAlphaBits,p.cAlphaShift);
	REMP("Pixel format: %s\n",name);

	if (!SetPixelFormat(HC->hdc, format, &p)) { REMP("Warning: Could not set pixel format\n"); return 0; }

	if (!(HC->hglrc = wglCreateContext(HC->hdc))) { REMP("Warning: Could not create rendering context\n"); return 0; }
	if (!(wglMakeCurrent(HC->hdc, HC->hglrc))) { REMP("Warning: Could not activate the rendering context\n"); return 0; }

	REMP("hc %ld winuaewnd %ld HC->hdc %ld HC->hglrc %ld format%ld\n",hc,winuaewnd,HC->hdc,HC->hglrc,format);
	return 1;
}

/*==================================================================*/
int OS_StartGL(void *hc,void *bm,int x,int y,int large, int high)
{
/* WIN32: Open OpenGL with  Option "hard" = Draw in BackBuffer of the WinUAE's window */
/* this function come from WinUAE */
struct HARD3D_context *HC=hc;
PIXELFORMATDESCRIPTOR p;
int format;
int depth;
unsigned char name[50];

	if(HC->UseOverlay)
		return( OS_StartGLoverlay(hc,bm,x,y,large,high) );

	x =x + 2 ;
	y =y + 17;

	HC->glwnd=winuaewnd;
	depth=32;

/* choose and set pixelformat */
	memset (&p, 0, sizeof (p));
	p.nSize = sizeof (PIXELFORMATDESCRIPTOR);
	p.nVersion = 1;
	if(HC->UseDoubleBuffer)
		p.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
	else
		p.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_TYPE_RGBA;

	p.cColorBits = depth;
	p.iLayerType = PFD_MAIN_PLANE;
	p.cDepthBits = 16;
	p.cStencilBits = 8;

	HC->hdc = GetDC (HC->glwnd);
	format = ChoosePixelFormat (HC->hdc, &p);
	if (format == 0)
		{
		REMP("OPENGL: can't find suitable pixelformat\n");
		return 0;
		}

	DescribePixelFormat(HC->hdc,format,sizeof(PIXELFORMATDESCRIPTOR), &p);
	if(p.iPixelType = PFD_TYPE_COLORINDEX)
		sprintf(name,"LUT8");
	if(p.iPixelType = PFD_TYPE_RGBA )
		sprintf(name,"R%ld<<%ld+G%ld<<%ld+B%ld<<%ld+A%ld<<%ld",p.cRedBits,p.cRedShift,p.cGreenBits,p.cGreenShift,p.cBlueBits,p.cBlueShift,p.cAlphaBits,p.cAlphaShift);
	REMP("Pixel format: %s\n",name);

	if (!SetPixelFormat (HC->hdc, format, &p))
		{
		REMP("OPENGL: can't set pixelformat %ld\n", format);
		return 0;
		}

/* create and set GL context */
	if (!(HC->hglrc = wglCreateContext (HC->hdc)))
		{
		REMP("OPENGL: can't create gl rendering context\n");
		return 0;
		}

	if (!wglMakeCurrent (HC->hdc, HC->hglrc))
		return 0;

	REMP("hc %ld winuaewnd %ld HC->hdc %ld HC->hglrc %ld format%ld\n",hc,winuaewnd,HC->hdc,HC->hglrc,format);
	return 1;
}
/*==================================================================*/
void OS_CloseGLoverlay(void *hc)
{
/* WIN32: Close OpenGL. this function come from QuarkTex */
struct HARD3D_context *HC=hc;

	if (HC->hglrc)
		{
		wglMakeCurrent(0, 0);
		wglDeleteContext(HC->hglrc);
		HC->hglrc = 0;
		}
	if (HC->hdc)
		{
		ReleaseDC(winuaewnd,HC->hdc);
		HC->hdc = 0;
		}
	if (HC->glwnd)
		{
		DestroyWindow(HC->glwnd);
		HC->glwnd = 0;
		}
	UnregisterClass("Wazp3D",HC->instance);
	HC->registered=0;
	HC->glstarted=0;
}
/*==================================================================*/
void OS_CloseGL(void *hc)
{
/* WIN32: Close OpenGL. this function come from QuarkTex */
struct HARD3D_context *HC=hc;

	if(HC->UseOverlay)
		{OS_CloseGLoverlay(hc); return;}

	if (HC->hglrc)
		{
		wglMakeCurrent(0, 0);
		wglDeleteContext(HC->hglrc);
		HC->hglrc = 0;
		}
	if (HC->hdc)
		{
		ReleaseDC(winuaewnd,HC->hdc);
		HC->hdc = 0;
		}
	HC->registered=0;
	HC->glstarted=0;
}
/*==================================================================*/
void HARD3D_DoUpdate(void *hc)
{
/* WIN32: Update the window. The SwapBuffers() part come from QuarkTex */
struct HARD3D_context *HC=hc;
ULONG *Image32;
#define TESTBGRA ((200<<24)+(100<<16)+(10<<8)+0)

	HFUNC(HARD3D_DoUpdate)
	OS_CurrentContext(hc);

	if(HC->UseOverlay)
	{
		SwapBuffers(HC->hdc);
	}
	else					/* in this case we draw y-flipped in the back buffer */
	{

		glReadBuffer(GL_BACK);
		Image32=HC->Image8;
		Image32[0]=TESTBGRA;

		glReadPixels(0,0,HC->large,HC->high,GL_BGRA_EXT,GL_UNSIGNED_BYTE,HC->Image8);	/* read the back buffer in the bitmap */

		if(Image32[0]==TESTBGRA)		/* bitmap unchanged so GL_BGRA_EXT was not supported */
		{
		glReadPixels(0,0,HC->large,HC->high,GL_RGBA,GL_UNSIGNED_BYTE,HC->Image8);	/* read the back buffer in the bitmap */
		ReorderBitmap(HC->Image8,HC->large,HC->high);						/* WinUAE use BGRA mode */
		}
	}

}
/*==================================================================*/
#endif	/* end for win32 */
/*==================================================================*/
#if defined(__AROS__)
/*==================================================================*/
void OS_CurrentContext(void *hc)
{
/* AROS: set current OpenGL context */
struct HARD3D_context *HC=hc;

	HFUNC(OS_CurrentContext)
	if(hc!=currenthc)
		{
		glAMakeCurrent(HC->hglrc);
		currenthc=hc;
		}
}
/*==================================================================*/
void OS_AdjustWindow(void *hc)
{
/* AROS: Move the "overlay" window like the original amiga window */
struct HARD3D_context *HC=hc;
struct Window *awin;
struct Window *overwin;
struct RastPort *hackrastport;
int x,y,large,high;

	if(!HC->UseOverlay)
	{
	hackrastport=HC->hackrastport;
	Move(hackrastport,0,0);
	Move(hackrastport->Layer->rp,0,0);
	}
	else
	{
	awin		=(struct Window *)HC->awin;
	overwin	=(struct Window *)HC->overwin;
	if(awin==NULL)	return;
	if(overwin==NULL)	return;

	x	=awin->LeftEdge	 + awin->BorderLeft ;
	y	=awin->TopEdge	 + awin->BorderTop  ;
	large	=awin->Width  - awin->BorderLeft - awin->BorderRight ;
	high	=awin->Height - awin->BorderTop  - awin->BorderBottom;

	REMP("AROS: overwin <%s> at %ld %ld size %ldX%ld\n",overwin->Title,x,y,large,high);
	ChangeWindowBox(overwin,x,y,large,high);
	}
}
/*==================================================================*/
int OS_StartGLoverlay(void *hc,int x, int y,int large, int high)
{
/* AROS: Open ArosMesa & Create an "overlay" window like the original amiga window */
struct HARD3D_context *HC=hc;
struct TagItem   attributes[20];
struct Window   *mesawin;
struct Window   *awin;
int i=0;
ULONG Flags =WFLG_ACTIVATE | WFLG_REPORTMOUSE | WFLG_RMBTRAP | WFLG_SIMPLE_REFRESH | WFLG_GIMMEZEROZERO ;
ULONG IDCMPs=IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS ;

	HFUNC(OS_StartGLoverlay)
	REMP("AROS: HC %ld \n",HC);

	if(!GLBase)
		GLBase = OpenLibrary("gl.library", 20L);

	if(!GLBase)
		{ REMP("AROS: Cant open gl.library\n"); return 0; }
	awin=(struct Window *)HC->awin;

/* duplicate current Amiga window as a new "overlay" window  */

	HC->overwin= OpenWindowTags(NULL,
	WA_Activate,	FALSE,
	WA_InnerWidth,	large,
	WA_InnerHeight,	high,
	WA_Left,		awin->LeftEdge,
	WA_Top,		awin->TopEdge,
	WA_Title,		(ULONG)overwinname,
	WA_DragBar,		FALSE,
	WA_CloseGadget,	FALSE,
	WA_GimmeZeroZero,	TRUE,
	WA_Backdrop,	FALSE,
	WA_Borderless,	TRUE,
	WA_IDCMP,		IDCMPs,
	WA_Flags,		Flags,
	TAG_DONE);

	if (HC->overwin==NULL)
		{Libprintf("Cant open overlay window\n");return 0;}

/* mesa will use overwin */
	mesawin=HC->overwin;

	REMP("AROS: HC->awin %ld \n",HC->awin);
	REMP("AROS: mesawin <%s> at %ld %ld size %ldX%ld\n",mesawin->Title,mesawin->LeftEdge,mesawin->TopEdge,mesawin->Width,mesawin->Height);
	REMP("AROS: mesawin (borders %ld %ld %ld %ld )\n"  ,mesawin->BorderLeft,mesawin->BorderTop,mesawin->BorderRight,mesawin->BorderBottom);

	attributes[i].ti_Tag = GLA_Window;		attributes[i++].ti_Data = (IPTR)mesawin;
	attributes[i].ti_Tag = GLA_Left;		attributes[i++].ti_Data = x;
	attributes[i].ti_Tag = GLA_Top;		attributes[i++].ti_Data = y;
	attributes[i].ti_Tag = GLA_Width;		attributes[i++].ti_Data = large;
	attributes[i].ti_Tag = GLA_Height;		attributes[i++].ti_Data = high;
	attributes[i].ti_Tag = GLA_DoubleBuf;	attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_RGBMode;		attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_NoDepth;		attributes[i++].ti_Data = GL_FALSE;
	attributes[i].ti_Tag = GLA_NoStencil;	attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_NoAccum;		attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = TAG_DONE;

/* So Context will be created with our hackrastport->BitMap */
	HC->hglrc = glACreateContext(attributes);
	if(!HC->hglrc)
		{ REMP("AROS: Cant create Mesa context\n"); return 0; }

	glAMakeCurrent(HC->hglrc);
	REMP("AROS: HC->awin %ld HC->hglrc %ld HC->overwin %ld\n",HC->awin,HC->hglrc,HC->overwin);
	return 1;
}
/*==================================================================*/
int OS_StartGL(void *hc,void *bm,int x, int y,int large, int high)
{
/* AROS: Open ArosMesa & use original amiga window */
struct HARD3D_context *HC=hc;
struct TagItem   attributes[20];
struct Window   *mesawin;
struct RastPort *hackrastport;
struct RastPort *oldrastport;
int i=0;


	if(HC->UseOverlay)
		return( OS_StartGLoverlay(hc,x,y,large,high) );

	HFUNC(OS_StartGL)
	REMP("AROS: HC %ld \n",HC);

	if(!GLBase)
		GLBase = OpenLibrary("gl.library", 0L);

	if(!GLBase)
		{ REMP("AROS: Cant open gl.library\n"); return 0; }

/* Current Amiga window : certainly the one we want */
	mesawin=HC->awin;

	REMP("AROS: HC->awin %ld \n",HC->awin);
	REMP("AROS: mesawin <%s> at %ld %ld size %ldX%ld\n",mesawin->Title,mesawin->LeftEdge,mesawin->TopEdge,mesawin->Width,mesawin->Height);
	REMP("AROS: mesawin (borders %ld %ld %ld %ld )\n"  ,mesawin->BorderLeft,mesawin->BorderTop,mesawin->BorderRight,mesawin->BorderBottom);

	attributes[i].ti_Tag = GLA_Window;		attributes[i++].ti_Data = (IPTR)mesawin;
	attributes[i].ti_Tag = GLA_Left;		attributes[i++].ti_Data = x;
	attributes[i].ti_Tag = GLA_Top;		attributes[i++].ti_Data = y;
	attributes[i].ti_Tag = GLA_Width;		attributes[i++].ti_Data = large;
	attributes[i].ti_Tag = GLA_Height;		attributes[i++].ti_Data = high;
	attributes[i].ti_Tag = GLA_DoubleBuf;	attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_RGBMode;		attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_NoDepth;		attributes[i++].ti_Data = GL_FALSE;
	attributes[i].ti_Tag = GLA_NoStencil;	attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = GLA_NoAccum;		attributes[i++].ti_Data = GL_TRUE;
	attributes[i].ti_Tag = TAG_DONE;

/* WARNING: As Aros cant use a particular bitmap as Warp3D do but a window	*/
/* so we create a fake rastport and use this rastport in the current window 	*/
/* then this window is used to create the GL context 					*/
/* This will have this effect on the internal AROSMesaContext *amesa :		*/
/*    amesa->awin = (struct Window *)GetTagData(GLA_Window, 0, tagList);	*/
/*	amesa->visible_rp = amesa->awin->RPort;						*/
/*	Calculate rastport dimensions 							*/
/*	amesa->visible_rp_width = amesa->visible_rp->Layer->bounds.MaxX - amesa->visible_rp->Layer->bounds.MinX + 1; */
/*	amesa->visible_rp_height= amesa->visible_rp->Layer->bounds.MaxY - amesa->visible_rp->Layer->bounds.MinY + 1; */

/* Create a new rastport with our Warp3D's bitmap*/
	hackrastport=(struct RastPort *)Libmalloc(sizeof(struct RastPort));
	if(hackrastport==NULL)
		return(0);
	HC->hackrastport=hackrastport; 		/* save for later freeing the rastport*/
	memcpy(hackrastport,mesawin->RPort,sizeof(struct RastPort));	/* our rastport = same as original rastport */
	oldrastport=mesawin->RPort;			/* save original window structure */
	hackrastport->BitMap=bm;			/* use Warp3D's bitmap */
	mesawin->RPort=hackrastport;			/* use our rastport */
	Move(hackrastport,0,0);
	Move(hackrastport->Layer->rp,0,0);

/* So Context will be created with our hackrastport->BitMap */
	HC->hglrc = glACreateContext(attributes);

/* Restaure the original window structure so windowing will still works */
	mesawin->RPort=oldrastport;

	if(!HC->hglrc)
		{ REMP("AROS: Cant create Mesa context\n"); return 0; }

	glAMakeCurrent(HC->hglrc);
	HC->overwin=0;
	REMP("AROS: HC->awin %ld HC->hglrc %ld HC->overwin %ld\n",HC->awin,HC->hglrc,HC->overwin);
	return 1;
}
/*==================================================================*/
void OS_CloseGL(void *hc)
{
/* AROS: Close ArosMesa & window */
struct HARD3D_context *HC=hc;

	HFUNC(OS_CloseGL)

	if(HC->overwin!=NULL)
	      CloseWindow(HC->overwin);

	if(HC->hglrc!=NULL)
		glADestroyContext(HC->hglrc);

	if(HC->hackrastport!=NULL)
		Libfree(HC->hackrastport);

	CloseLibrary(GLBase);

	HC->hackrastport=NULL;
	HC->hglrc=NULL;
	HC->overwin=NULL;
	HC->awin=NULL;
}
/*==================================================================*/
void HARD3D_DoUpdate(void *hc)
{
/* AROS: Update the window. */
struct HARD3D_context *HC=hc;
struct RastPort *oldrastport;
struct Window   *mesawin;
short int  oldLeftEdge, oldTopEdge;

	HFUNC(HARD3D_DoUpdate)
	OS_CurrentContext(hc);

	if(HC->UseOverlay)					/* in this case we drawn in overwin */
		{
	HFUNC(glASwapBuffers)
		glASwapBuffers(HC->hglrc);
	HFUNC(glASwapBuffersOK)
		}
	else
		{							/* else we drawn in back buffer	*/
		mesawin=HC->awin;

		oldrastport=mesawin->RPort;		/* save original window's rastport	*/
		oldLeftEdge=mesawin->LeftEdge;		/* save original window's position	*/
		oldTopEdge =mesawin->TopEdge;

#ifdef WINHACK
		mesawin->LeftEdge=0;					/* so will write at 0 0 in our bitmap */
		mesawin->TopEdge =0;
		MoveWindow(mesawin,-oldLeftEdge,-oldTopEdge);
#endif
		mesawin->RPort=(struct RastPort *)HC->hackrastport;	/* use our rastport and bitmap */
		glASwapBuffers(HC->hglrc);				/* On Aros this will copy the back buffer to the current window */

#ifdef WINHACK
		mesawin->LeftEdge=oldLeftEdge;		/* restore window */
		mesawin->TopEdge =oldTopEdge;
		MoveWindow(mesawin,oldLeftEdge,oldTopEdge);
#endif

		mesawin->RPort   =oldrastport;

		}
}
/*==================================================================*/
#endif	/* end for aros */
/*==================================================================*/
void HARD3D_Start(void *hc)
{
struct HARD3D_context *HC=hc;
	HFUNC(HARD3D_Start);
	return;
}
/*==================================================================*/
void HARD3D_ClearImageBuffer(void *hc,unsigned short x,unsigned short y,unsigned short large,unsigned short high,void *rgba)
{
struct HARD3D_context *HC=hc;
unsigned char *RGBA=rgba;

	HFUNC(HARD3D_ClearImageBuffer);
	OS_CurrentContext(hc);
	glClearColor( (float)RGBA[0]/255.0,(float)RGBA[1]/255.0,(float)RGBA[2]/255.0,(float)RGBA[3]/255.0);
	glClear(GL_COLOR_BUFFER_BIT);
}
/*==================================================================*/
void startgl(void *hc,void *bm,int x, int y,int large, int high)
{
struct HARD3D_context *HC=hc;
unsigned char  RGBA[4]={0,0,0,255};

	HFUNC(startgl);
	REMP("pos: %ld %ld size: %ldX%ld\n",x,y,large,high);

	HC->UseDoubleBuffer=TRUE;
	HC->glstarted=OS_StartGL(hc,bm,x,y,large,high);
	if(!HC->glstarted)
		return;

	OS_CurrentContext(hc);
	REMP("OpenGL Vendor   <%s>\n"	,glGetString(GL_VENDOR));
	REMP("OpenGL Renderer <%s>\n"	,glGetString(GL_RENDERER));
	REMP("OpenGL Version  <%s>\n"	,glGetString(GL_VERSION));

/* use a flat mode */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glDepthRange(0.0,1.0);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	if(HC->UseAntiAlias)
	{
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	}
	else
	{
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_LINE_SMOOTH);
	}

	HARD3D_ClearImageBuffer(HC,0,0,large,high,RGBA);

}
/*==================================================================*/
void HARD3D_ClearZBuffer(void *hc,float fz)
{
struct HARD3D_context *HC=hc;
GLclampd depth=fz;

	HFUNC(HARD3D_ClearZBuffer);
	OS_CurrentContext(hc);
	glClearDepth(depth);
	glDepthMask (TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
}
/*==================================================================*/
void HARD3D_CreateTexture(void *hc,void *ht,unsigned char *pt,unsigned short large,unsigned short high,unsigned short format,unsigned char TexFlags)
{
#define AND &
struct HARD3D_context *HC=hc;
struct HARD3D_texture *HT=ht;
unsigned long size,level,glformat,bits = 0;
unsigned char UseMip=TexFlags AND 1;
unsigned char UseFiltering=TexFlags AND 2;
unsigned char *pt2;
GLuint gltex;

	HFUNC(HARD3D_CreateTexture);
	OS_CurrentContext(hc);
	glGenTextures(1,&gltex);
	glBindTexture(GL_TEXTURE_2D,gltex);
	HT->gltex=gltex;
VAR(hc)
VAR(ht)
VAR(pt)
VAR(large)
VAR(high)
VAR(bits)
VAR(UseMip)
VAR(UseFiltering)
VAR(gltex)

	if(UseFiltering)
	{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(UseMip)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}
	else
	{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	if(UseMip)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	if(format==W3D_R8G8B8)   {bits=24;glformat=GL_RGB; }
	if(format==W3D_R8G8B8A8) {bits=32;glformat=GL_RGBA;}

	VAR(glformat)
	UseMip=0;
if(UseMip)
	{
	level=0;
	pt2=pt;
	size=large * high * bits / 8;
next_mipmap:
	glTexImage2D(GL_TEXTURE_2D,level,bits/8,large,high, 0, glformat, GL_UNSIGNED_BYTE,pt2);
	pt2=&(pt2[size]);
	level++;large=large/2;high=high/2;size=size/4;
	if (high>0) goto next_mipmap;
	}
else
	glTexImage2D(GL_TEXTURE_2D, 0,bits/8,large,high, 0, glformat, GL_UNSIGNED_BYTE,pt);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
}
/*=================================================================*/
void ReorderBitmap(unsigned char  *rgba,unsigned short large,unsigned short high)
{
register unsigned char *RGBA=rgba;	/* use only registers */
register unsigned char temp;
register unsigned long size=large*high/8;
register unsigned long n;
#define SWAP(x,y) {temp=x;x=y;y=temp;}


	NLOOP(size)
			{
			SWAP(RGBA[0+0  ],RGBA[2+0  ]);	/* process 8 pixels */
			SWAP(RGBA[0+4  ],RGBA[2+4  ]);
			SWAP(RGBA[0+8  ],RGBA[2+8  ]);
			SWAP(RGBA[0+12 ],RGBA[2+12 ]);
			SWAP(RGBA[0+16 ],RGBA[2+16 ]);
			SWAP(RGBA[0+20 ],RGBA[2+20 ]);
			SWAP(RGBA[0+24 ],RGBA[2+24 ]);
			SWAP(RGBA[0+28 ],RGBA[2+28 ]);
			RGBA+=4*8;
			}
}
/*==================================================================*/
void HARD3D_DrawPrimitive(void *hc,void *p,unsigned long Pnb,unsigned long primitive)
{
struct point3D *P=p;
struct HARD3D_context *HC=hc;
unsigned char UsePersp =(HC->state->PerspMode!=0);
unsigned char UseColors=(HC->state->UseGouraud!=0);
struct TexCoord4f *V=(void*)HC->V;
unsigned int mode;
long int n;

	HFUNC(HARD3D_DrawPrimitive);
	OS_CurrentContext(hc);
VAR(P)
VAR(Pnb)

	if(Pnb>MAXPRIM)
		return;

	if(UsePersp)
	NLOOP(Pnb)
		{

		V[n].s=P[n].w*P[n].u ;
		V[n].t=P[n].w*P[n].v ;
		V[n].r=0.0;
		V[n].q=P[n].w;
		if(1.0<P[n].z) P[n].z=1.0;		/* force Z to the range 0.0 to 1.0 (GlExcess) */
 		if(P[n].z<0.0) P[n].z=0.0;
		}
	else
	NLOOP(Pnb)
		{
		if(1.0<P[n].z) P[n].z=1.0;		/* force Z to the range 0.0 to 1.0 (GlExcess) */
 		if(P[n].z<0.0) P[n].z=0.0;
		}

	NLOOP(Pnb)
		PrintP(&P[n]);

	if(primitive==W3D_PRIMITIVE_POINTS)		{mode=GL_POINTS;			goto olddraw;}
	if(primitive==W3D_PRIMITIVE_LINESTRIP)	{mode=GL_LINE_STRIP;		goto olddraw;}
	if(primitive==W3D_PRIMITIVE_LINELOOP)	{mode=GL_LINE_LOOP;		goto olddraw;}
	if(primitive==W3D_PRIMITIVE_LINES)		{mode=GL_LINES;			goto olddraw;}
	if(primitive==W3D_PRIMITIVE_TRISTRIP)	{primitive=GL_TRIANGLE_STRIP;	goto newdraw;}
	if(primitive==W3D_PRIMITIVE_TRIFAN)		{primitive=GL_TRIANGLE_FAN;	goto newdraw;}
	if(primitive==W3D_PRIMITIVE_TRIANGLES)	{primitive=GL_TRIANGLES;	goto newdraw;}
	if(primitive==W3D_PRIMITIVE_POLYGON)	{primitive=GL_POLYGON;		goto newdraw;}


newdraw:
REM(newdraw)
VAR(primitive)

	if(currentP!=P)
	{
	REM(P changed)
	currentP=P;
	glVertexPointer(3, GL_FLOAT, PSIZE,(void *)&(P->x));
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4,GL_UNSIGNED_BYTE,PSIZE,(void *)&(P->RGBA));
	}

	if(UseColors)
		glEnableClientState(GL_COLOR_ARRAY);
	else
		glDisableClientState(GL_COLOR_ARRAY);

	if(UsePersp)
		glTexCoordPointer(4, GL_FLOAT, 4*4,HC->V);
	else
		glTexCoordPointer(2, GL_FLOAT,PSIZE,(void *)&(P->u));

	glDrawArrays(primitive,0,Pnb);
	return;

olddraw:
REM(olddraw)
VAR(mode)
	glBegin(mode);
	NLOOP(Pnb)
		{
		glVertex3f(P[n].x,P[n].y,P[n].z);

		if(UsePersp)
			glTexCoord4f(V[n].s,V[n].t,V[n].r,V[n].q);
		else
			glTexCoord2f(P[n].u,P[n].v);

		if(UseColors)
			glColor4f((float)P[n].RGBA[0]/255.0,(float)P[n].RGBA[1]/255.0,(float)P[n].RGBA[2]/255.0,(float)P[n].RGBA[3]/255.0);
		else
			glColor4f( (float)HC->state->CurrentRGBA[0]/255.0,(float)HC->state->CurrentRGBA[1]/255.0,(float)HC->state->CurrentRGBA[2]/255.0,(float)HC->state->CurrentRGBA[3]/255.0);

		}
	glEnd();
}
/*==================================================================*/
void HARD3D_End(void *hc)
{
struct HARD3D_context *HC=hc;

	HFUNC(HARD3D_End);
	OS_CurrentContext(hc);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	OS_CloseGL(hc);
}
/*==================================================================*/
void HARD3D_Flush(void *hc)
{
struct HARD3D_context *HC=hc;

	HFUNC(HARD3D_Flush);
	OS_CurrentContext(hc);
/*	TODO: emulate Warp3D's direct/indirect mode with glflush/glfinish
	glFlush();
	glFinish();
*/
}
/*==================================================================*/
void ChangeHardFog(void *hc,unsigned char FogMode,float FogZmin,float FogZmax,float FogDensity,unsigned char *FogRGBA)
{
struct HARD3D_context *HC=hc;
float FogColor[4];
unsigned long FogModes[] = {0,GL_LINEAR,GL_EXP,GL_EXP2};

	HFUNC(HARD3D_Fog);
	OS_CurrentContext(hc);
	if( FogZmin	< 0.0			) FogZmin	= 0.0;
	if( 1.0	< FogZmax		) FogZmax	= 1.0;
	if( FogDensity	< 0.0		) FogDensity= 0.0;
	if( 1.0 	< FogDensity	) FogDensity= 1.0;

	if (FogMode==0)
	{
	glDisable(GL_FOG);
	return;
	}

	FogColor[0]=FogRGBA[0]/255.0;
	FogColor[1]=FogRGBA[1]/255.0;
	FogColor[2]=FogRGBA[2]/255.0;
	FogColor[3]=FogRGBA[3]/255.0;
	glFogi (GL_FOG_MODE   ,FogModes[FogMode]);
	glFogfv(GL_FOG_COLOR  ,FogColor);
	glFogf (GL_FOG_START  ,FogZmin);
	glFogf (GL_FOG_END    ,FogZmax);
	glFogf (GL_FOG_DENSITY,FogDensity);
	glEnable(GL_FOG);
}
/*==================================================================*/
void HARD3D_FreeTexture(void *hc,void *ht)
{
struct HARD3D_context *HC=hc;
struct HARD3D_texture *HT=ht;
GLuint gltex;

	HFUNC(HARD3D_FreeTexture);
	OS_CurrentContext(hc);
	gltex=HT->gltex;
	glDeleteTextures(1,&gltex);
}
/*==================================================================*/
void HARD3D_ReadZSpan(void *hc, unsigned short x, unsigned short y,unsigned long n, double *dz)
{
struct HARD3D_context *HC=hc;
float *fz=HC->fzspan;

	HFUNC(HARD3D_ReadZSpan);
	OS_CurrentContext(hc);
	glReadBuffer(GL_BACK);
	glReadPixels(x,y,n,1, GL_DEPTH_COMPONENT, GL_FLOAT,fz);
	XLOOP(n)
		dz[x]=fz[x];
}
/*==================================================================*/
void HARD3D_WriteZSpan(void *hc, unsigned short x, unsigned short y,unsigned long n, double *dz, unsigned char *mask)
{
struct HARD3D_context *HC=hc;
float *fz=HC->fzspan;

	HFUNC(HARD3D_WriteZSpan);
	OS_CurrentContext(hc);
	glReadBuffer(GL_BACK);
	glReadPixels(x,y,n,1, GL_DEPTH_COMPONENT, GL_FLOAT,fz);	/* read existing depth values */
	glRasterPos2i(x,y);
	XLOOP(n)
		if(mask[x]!=0)							/* only if (mask[x]) then change depth-values[x] */
			fz[x]=dz[x];
	glDrawPixels(n,1,GL_DEPTH_COMPONENT,GL_FLOAT,fz);		/* write existing & new depth values */
}
/*==================================================================*/
void HARD3D_AllocImageBuffer(void *hc,unsigned short large,unsigned short high)
{
struct HARD3D_context *HC=hc;
	HFUNC(HARD3D_AllocImageBuffer);
	return;		/* Writing to an ImageBuffer32 is not possible in hardware mode */
}
/*==================================================================*/
void HARD3D_SetBitmap(void *hc,void *bm,unsigned char *Image8,unsigned long format,unsigned short x,unsigned short y,unsigned short large,unsigned short high)
{
struct HARD3D_context *HC=hc;
float flarge=large;
float fhigh =high;
float xscale,yscale,zscale;
float xtrans,ytrans,ztrans;
struct RastPort *hackrastport;

	HFUNC(HARD3D_SetBitmap);
VAR(large)
VAR(high)
	if(!HC->glstarted)
		startgl(hc,bm,x,y,large,high);
	OS_CurrentContext(hc);

VAR(Image8)

	HC->Image8	=Image8;
	HC->large	=large;
	HC->high	=high;
	xscale	= 2.0f / flarge;
	yscale	=-2.0f / fhigh;
	zscale	= 2.0f / 1.0f;
	xtrans	=-(flarge / 2.0f);
	ytrans	=-(fhigh  / 2.0f);
	ztrans	=-(1.0f   / 2.0f);

#if defined(_WIN32)
	if(!HC->UseOverlay)		/* in this case we draw y-flipped in the back buffer */
		yscale=-yscale;
#endif

/* use a flat mode */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(xscale,yscale,zscale);
	glTranslatef(xtrans,ytrans,ztrans);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	OS_AdjustWindow(hc);

	glViewport(0,0,large,high);
}
/*==================================================================*/
void HARD3D_SetClipping(void *hc,unsigned short xmin,unsigned short xmax,unsigned short ymin,unsigned short ymax)
{
struct HARD3D_context *HC=hc;
int x,y,high,large;

	HFUNC(HARD3D_SetClipping);
	OS_CurrentContext(hc);

	x=xmin;
	y=(HC->high-1)-ymax;	/* GL origin is lower-left corner */
	large=xmax-xmin+1;
	high =ymax-ymin+1;

#if defined(_WIN32)
	if(!HC->UseOverlay)		/* in this case we draw y-flipped in the back buffer */
		y=ymin;
#endif

	glScissor(x,y,large,high);
	glEnable(GL_SCISSOR_TEST);
}
/*==================================================================*/
void HARD3D_SetDrawFunctions(void *hc)
{
struct HARD3D_context *HC=hc;
struct state3D *state=HC->state;
unsigned long ZUpdate,ZCompareMode,SrcFunc,DstFunc,TexEnvMode32;
float envcolor[4];
unsigned char BlendMode;
#define ZMODE(ZUpdate,ZCompareMode) (ZUpdate*8 + (ZCompareMode-1))

REM(---------)
	HFUNC(HARD3D_SetDrawFunctions);
	OS_CurrentContext(hc);
VAR(hc)
VAR(state->UseTex)
VAR(state->ST)
VAR(state->gltex)
VAR(state->TexEnvMode)
VAR(state->ZMode)
VAR(state->BlendMode)
VAR(state->UseGouraud)
VAR(state->UseFog)
VAR(state->PerspMode)


/* --- UseGouraud -------------*/
	if(state->UseGouraud)
		{
		REM(Gouraud  ON);
		glShadeModel(GL_SMOOTH);
		}
	else
		{
		REM(Gouraud OFF);
		glShadeModel(GL_FLAT);
		glColor4f( (float)state->CurrentRGBA[0]/255.0,(float)state->CurrentRGBA[1]/255.0,(float)state->CurrentRGBA[2]/255.0,(float)state->CurrentRGBA[3]/255.0);
		}

/* state->CullingMode=W3D_NOW; */

/* --- CullingMode -----------------*/
	if(state->CullingMode==W3D_NOW)			/* NO W-inding 	*/
		{glDisable(GL_CULL_FACE);}
	if(state->CullingMode==W3D_CW)
		{glEnable(GL_CULL_FACE); glFrontFace(GL_CW); }
	if(state->CullingMode==W3D_CCW)
		{glEnable(GL_CULL_FACE); glFrontFace(GL_CCW);}

#if defined(_WIN32)
	if(!HC->UseOverlay)	/* then we draw y-flipped so backfaces are now frontfaces */
	{
	if(state->CullingMode==W3D_CW)
		glFrontFace(GL_CCW);
	if(state->CullingMode==W3D_CCW)
		glFrontFace(GL_CW);
	}
#endif

/* --- PointSize -----------------*/
	glPointSize((float)state->PointSize);

/* --- FogMode -----------------*/
	if(state->UseFog)
		ChangeHardFog(hc,state->FogMode,state->FogZmin,state->FogZmax,state->FogDensity,state->FogRGBA);
	else
		glDisable(GL_FOG);

/* --- ZMode -----------------*/

	if(state->ZMode == ZMODE(0,W3D_Z_ALWAYS) )	/* default no z buffer = no test ==> always draw & dont update a zbuffer */
	{
	REM(depth OFF);
	glDisable(GL_DEPTH_TEST);	/* no test   */
	glDepthMask (0);			/* no update */
	}
	else					/* else test and/or update the z buffer */
	{
	REM(depth ON);
	ZUpdate	=(state->ZMode >>  3);
   	ZCompareMode=(state->ZMode AND 7) + 1;
	glEnable(GL_DEPTH_TEST);

	VAR(state->ZMode)
	VAR(ZUpdate)
	VAR(ZCompareMode)
	if(ZCompareMode==W3D_Z_NEVER)			ZCompareMode=GL_NEVER;
	if(ZCompareMode==W3D_Z_LESS)			ZCompareMode=GL_LESS;
	if(ZCompareMode==W3D_Z_EQUAL)			ZCompareMode=GL_EQUAL;
	if(ZCompareMode==W3D_Z_LEQUAL)		ZCompareMode=GL_LEQUAL;
	if(ZCompareMode==W3D_Z_GREATER)		ZCompareMode=GL_GREATER;
	if(ZCompareMode==W3D_Z_NOTEQUAL)		ZCompareMode=GL_NOTEQUAL;
	if(ZCompareMode==W3D_Z_GEQUAL)		ZCompareMode=GL_GEQUAL;
	if(ZCompareMode==W3D_Z_ALWAYS)		ZCompareMode=GL_ALWAYS;
	VAR(ZCompareMode)
	glDepthFunc(ZCompareMode);
	glDepthMask(ZUpdate);
	}


/* --- UseTex -----------------*/

	if(state->UseTex)
		{
	REM(UseTex);
	VAR(state->gltex)
		glBindTexture(GL_TEXTURE_2D,state->gltex);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_BLEND);
		}
	else
		{
	REM(Not UseTex);
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_BLEND);					/* so simple color can be transparent */
		}

/* --- BlendMode -----------------*/
	BlendMode=state->BlendMode;
	if(BlendMode==BLENDFASTALPHA)
		BlendMode=BLENDALPHA ;
	if(BlendMode==BLENDNOALPHA)
		BlendMode=BLENDREPLACE;
	if(BlendMode==BLENDCHROMA)
		BlendMode=(W3D_ONE*16 + W3D_ONE);		/* v51:this is not a chromatest but looks nice too */

	if(BlendMode==BLENDREPLACE)				/* default : simply write the color (even if color=tex+color) */
	{
	glDisable(GL_BLEND);
	}
	else
	{
	REM(Blend ON);
	SrcFunc=BlendMode/16; DstFunc=BlendMode-SrcFunc*16;
VAR(SrcFunc)
VAR(DstFunc)
	if(SrcFunc==W3D_ZERO)				SrcFunc=GL_ZERO;
	if(SrcFunc==W3D_ONE)				SrcFunc=GL_ONE;
	if(SrcFunc==W3D_SRC_COLOR)			SrcFunc=GL_SRC_COLOR;
	if(SrcFunc==W3D_ONE_MINUS_SRC_COLOR )	SrcFunc=GL_ONE_MINUS_SRC_COLOR;
	if(SrcFunc==W3D_DST_COLOR)			SrcFunc=GL_DST_COLOR;
	if(SrcFunc==W3D_ONE_MINUS_DST_COLOR)	SrcFunc=GL_ONE_MINUS_DST_COLOR;
	if(SrcFunc==W3D_SRC_ALPHA)			SrcFunc=GL_SRC_ALPHA;
	if(SrcFunc==W3D_ONE_MINUS_SRC_ALPHA)	SrcFunc=GL_ONE_MINUS_SRC_ALPHA;
	if(SrcFunc==W3D_DST_ALPHA)			SrcFunc=GL_DST_ALPHA;
	if(SrcFunc==W3D_ONE_MINUS_DST_ALPHA)	SrcFunc=GL_ONE_MINUS_DST_ALPHA;
   	if(SrcFunc==W3D_SRC_ALPHA_SATURATE)		SrcFunc=GL_SRC_ALPHA_SATURATE;

	if(DstFunc==W3D_ZERO)				DstFunc=GL_ZERO;
	if(DstFunc==W3D_ONE)				DstFunc=GL_ONE;
	if(DstFunc==W3D_SRC_COLOR)			DstFunc=GL_SRC_COLOR;
	if(DstFunc==W3D_ONE_MINUS_SRC_COLOR )	DstFunc=GL_ONE_MINUS_SRC_COLOR;
	if(DstFunc==W3D_DST_COLOR)			DstFunc=GL_DST_COLOR;
	if(DstFunc==W3D_ONE_MINUS_DST_COLOR)	DstFunc=GL_ONE_MINUS_DST_COLOR;
	if(DstFunc==W3D_SRC_ALPHA)			DstFunc=GL_SRC_ALPHA;
	if(DstFunc==W3D_ONE_MINUS_SRC_ALPHA)	DstFunc=GL_ONE_MINUS_SRC_ALPHA;
	if(DstFunc==W3D_DST_ALPHA)			DstFunc=GL_DST_ALPHA;
	if(DstFunc==W3D_ONE_MINUS_DST_ALPHA)	DstFunc=GL_ONE_MINUS_DST_ALPHA;
	if(DstFunc==W3D_SRC_ALPHA_SATURATE)		DstFunc=GL_SRC_ALPHA_SATURATE;
VAR(SrcFunc)
VAR(DstFunc)
	glBlendFunc(SrcFunc,DstFunc);
	glEnable(GL_BLEND);
	}

/* --- TexEnvMode -----------------*/
#define GL_REPLACE 0x1E01
#define GL_BLEND   0x0BE2
	if(state->UseTex)
	{

	REM(state->TexEnvModes);
	TexEnvMode32=GL_REPLACE;		/* default */
	if(state->TexEnvMode==W3D_REPLACE)		TexEnvMode32=GL_REPLACE;
	if(state->TexEnvMode==W3D_MODULATE)		TexEnvMode32=GL_MODULATE;
	if(state->TexEnvMode==W3D_DECAL)		TexEnvMode32=GL_DECAL;
	if(state->TexEnvMode==W3D_BLEND)		TexEnvMode32=GL_BLEND;

	if(state->TexEnvMode==W3D_BLEND)		/* also need the envcolor */
	{
	envcolor[0]=(float)state->EnvRGBA[0]/255.0;
	envcolor[1]=(float)state->EnvRGBA[1]/255.0;
	envcolor[2]=(float)state->EnvRGBA[2]/255.0;
	envcolor[3]=(float)state->EnvRGBA[3]/255.0;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,envcolor);
	}

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,TexEnvMode32);
	VAR(TexEnvMode32)
	}

/* --- PerspMode -----------------*/
	if(state->PerspMode==0)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	else
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

REM(----------------------)
}
/*==================================================================*/
void HARD3D_AllocZbuffer(void *hc,unsigned short large,unsigned short high)
{
struct HARD3D_context *HC=hc;
	HFUNC(HARD3D_AllocZbuffer);
	OS_CurrentContext(hc);
	if(large!=0)
	if(high !=0)
		{
	 	glEnable(GL_DEPTH_TEST);
		glDepthFunc(W3D_Z_LESS);
		glDepthMask(TRUE);
		return;
		}
	glDisable(GL_DEPTH_TEST);
}
/*==================================================================*/
void HARD3D_UpdateTexture(void *hc,void *ht,unsigned char *pt,unsigned short large,unsigned short high,unsigned char bits)
{
struct HARD3D_context *HC=hc;
struct HARD3D_texture *HT=ht;
unsigned long format,x,y,level;
GLuint gltex;

	HFUNC(HARD3D_UpdateTexture);
	OS_CurrentContext(hc);
	gltex=HT->gltex;
	glBindTexture(GL_TEXTURE_2D,gltex);
	VAR(gltex)
	x=y=level=0;
	if(bits==32)
		format=GL_RGBA; else format=GL_RGB;
	glTexSubImage2D(GL_TEXTURE_2D,level,x,y,large,high,format,GL_UNSIGNED_BYTE,pt);
}
/*==================================================================*/



