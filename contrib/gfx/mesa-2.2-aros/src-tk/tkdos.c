/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

// Mesa Tweaking by: Mark E. Peterson (markp@ic.mankato.mn.us)
// Adapted from windows version for dos by: Charlie Wallace (cwallace@dreamworks.com)


// Force this on, i`m always forgetting, and it must be on for this
// file anyway, cw..

#ifndef DOSVGA
#define DOSVGA 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <conio.h>

#ifdef DJGPP
#include <pc.h>
#endif

// Ah the wonders of TRUE and FALSE, heaven help us if they`re enums
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#include "gltk.h"
#include "gl\dosmesa.h"

#define static

#if defined(__cplusplus) || defined(c_plusplus)
#define class c_class
#endif

#if DBG
#define TKASSERT(x)                                     \
if ( !(x) ) {                                           \
    PrintMessage("%s(%d) Assertion failed %s\n",        \
        __FILE__, __LINE__, #x);                        \
}
#else
#define TKASSERT(x)
#endif  /* DBG */

/******************************************************************************/

static struct _WINDOWINFO {
    int x, y;
    int width, height;
    GLenum type;
    GLenum dmPolicy;
    int ipfd;
    BOOL bDefPos;
} windInfo = {
    0, 0, 320, 200, TK_INDEX | TK_SINGLE, TK_MINIMUM_CRITERIA, 0, TRUE
};


static HWND     tkhwnd     = NULL;
static HDC      tkhdc      = NULL;
static HPALETTE tkhpalette = NULL;
GLboolean tkPopupEnable = TRUE;

// Fixed palette support.

#define BLACK   PALETTERGB(0,0,0)
#define WHITE   PALETTERGB(255,255,255)
#define NUM_STATIC_COLORS   (COLOR_BTNHIGHLIGHT - COLOR_SCROLLBAR + 1)

static void (*ExposeFunc)(int, int)              = NULL;
static void (*ReshapeFunc)(GLsizei, GLsizei)     = NULL;
static void (*DisplayFunc)(void)                 = NULL;
static GLenum (*KeyDownFunc)(int, GLenum)        = NULL;
static GLenum (*MouseDownFunc)(int, int, GLenum) = NULL;
static GLenum (*MouseUpFunc)(int, int, GLenum)   = NULL;
static GLenum (*MouseMoveFunc)(int, int, GLenum) = NULL;
static void (*IdleFunc)(void)                    = NULL;

static char     *lpszClassName = "tkLibWClass";
static WCHAR    *lpszClassNameW = "tkLibWClass";

long tkWndProc(HWND hWnd, UINT message, DWORD wParam, LONG lParam);
static unsigned char ComponentFromIndex(int i, int nbits, int shift );
static void PrintMessage( const char *Format, ... );
//static PALETTEENTRY *FillRgbPaletteEntries( PIXELFORMATDESCRIPTOR *Pfd, PALETTEENTRY *Entries, UINT Count );
static HPALETTE CreateCIPalette( HDC Dc );
static HPALETTE CreateRGBPalette( HDC hdc );
static void DestroyThisWindow( HWND Window );
static void CleanUp( void );
static void DelayPaletteRealization( void );
static long RealizePaletteNow( HDC Dc, HPALETTE Palette);
static void ForceRedraw( HWND Window );
static void *AllocateMemory( size_t Size );
static void *AllocateZeroedMemory( size_t Size );
static void FreeMemory( void *Chunk );

/*
 *  Prototypes for the debugging functions go here
 */

#define DBGFUNC 0
#if DBGFUNC

static void DbgPrintf( const char *Format, ... );
static void pwi( void );
static void pwr(RECT *pr);
//static void ShowPixelFormat(HDC hdc);

#endif
#define NCOLORS 17
float tkRGBMap[NCOLORS][3] = {
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {1,0,0},
    {0,1,0},
    {1,1,0},
    {0,0,1},
    {1,0,1},
    {0,1,1},
    {1,1,1}
};


/***************************************************************
 *                                                             *
 *  Exported Functions go here                                 *
 *                                                             *
 ***************************************************************/

void tkErrorPopups(GLboolean bEnable)
{
    tkPopupEnable = bEnable;
}

void tkCloseWindow(void)
{
#if 0
    DestroyThisWindow(tkhwnd);
	/* if (w.cMain) {
	   XMesaDestroyContext(w.cMain);
	}*/
#endif
	DOSMesaDestroyContext(NULL);
}


void tkExec(void)
{
	int key;
	char ch;
	GLenum mask=0;

    //MSG Message;

    /*
     *  WM_SIZE gets delivered before we get here!
     */

    if (ReshapeFunc)
    {
    	(*ReshapeFunc)(windInfo.width, windInfo.height);
    }

    while (ch!=27)  //GL_TRUE
    {
        /*
         *  Process all pending messages
         */
		  #if 0
        while (PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
            if (GetMessage(&Message, NULL, 0, 0) )
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            else
            {
                /*
                 *  Nothing else to do here, just return
                 */

                return;
            }
        }
        #endif

        /*
         *  If an idle function was defined, call it
         */

		  	if(kbhit()) {
		 		ch = getch();

				if(KeyDownFunc) {
					switch(ch) {
						case ' ':
							key = TK_SPACE;
							break;
						case 27:
							key = TK_ESCAPE;
							break;
						case '1':
							key = TK_1;
							break;
						case '2':
							key = TK_2;
							break;
						case '3':
							key = TK_3;
							break;
						case '4':
							key = TK_4;
							break;
						case '5':
							key = TK_5;
							break;
						case '6':
							key = TK_6;
							break;
						case '7':
							key = TK_7;
							break;
						case '8':
							key = TK_8;
							break;
						case '9':
							key = TK_9;
							break;
						case '0':
							key = TK_0;
							break;
						case 'a':
							key = TK_a;
							break;
						case 'b':
							key = TK_b;
							break;
						case 'c':
							key = TK_c;
							break;
						case 'd':
							key = TK_d;
							break;
						case 'e':
							key = TK_e;
							break;
						case 'f':
							key = TK_f;
							break;
						case 'g':
							key = TK_g;
							break;
						case 'h':
							key = TK_h;
							break;
						case 'i':
							key = TK_i;
							break;
						case 'j':
							key = TK_j;
							break;
						case 'k':
							key = TK_k;
							break;
						case 'l':
							key = TK_l;
							break;
						case 'm':
							key = TK_m;
							break;
						case 'n':
							key = TK_n;
							break;
						case 'o':
							key = TK_o;
							break;
						case 'p':
							key = TK_p;
							break;
						case 'q':
							key = TK_q;
							break;
						case 'r':
							key = TK_r;
							break;
						case 's':
							key = TK_s;
							break;
						case 't':
							key = TK_t;
							break;
						case 'u':
							key = TK_u;
							break;
						case 'v':
							key = TK_v;
							break;
						case 'w':
							key = TK_w;
							break;
						case 'x':
							key = TK_x;
							break;
						case 'y':
							key = TK_y;
							break;
						case 'z':
							key = TK_z;
							break;
						case 'A':
							key = TK_A;
							break;
						case 'B':
							key = TK_B;
							break;
						case 'C':
							key = TK_C;
							break;
						case 'D':
							key = TK_D;
							break;
						case 'E':
							key = TK_E;
							break;
						case 'F':
							key = TK_F;
							break;
						case 'G':
							key = TK_G;
							break;
						case 'H':
							key = TK_H;
							break;
						case 'I':
							key = TK_I;
							break;
						case 'J':
							key = TK_J;
							break;
						case 'K':
							key = TK_K;
							break;
						case 'L':
							key = TK_L;
							break;
						case 'M':
							key = TK_M;
							break;
						case 'N':
							key = TK_N;
							break;
						case 'O':
							key = TK_O;
							break;
						case 'P':
							key = TK_P;
							break;
						case 'Q':
							key = TK_Q;
							break;
						case 'R':
							key = TK_R;
							break;
						case 'S':
							key = TK_S;
							break;
						case 'T':
							key = TK_T;
							break;
						case 'U':
							key = TK_U;
							break;
						case 'V':
							key = TK_V;
							break;
						case 'W':
							key = TK_W;
							break;
						case 'X':
							key = TK_X;
							break;
						case 'Y':
							key = TK_Y;
							break;
						case 'Z':
							key = TK_Z;
							break;
						case 0:
							if( kbhit() ) {
								ch = getch();
								switch( ch ) {
									case 'P':
										key = TK_UP;
										break;
									case 'M':
										key = TK_LEFT;
										break;
									case 'K':
										key = TK_RIGHT;
										break;
									case 'H':
										key = TK_DOWN;
										break;
								}
							}
							break;
						}
						(*KeyDownFunc)(key,mask);
						key = 0;
					}
				}

       	if (IdleFunc)
        {
            (*IdleFunc)();
        }
        if( DisplayFunc)
        {
        	(*DisplayFunc)();
        }
    }
}

void tkExposeFunc(void (*Func)(int, int))
{
    ExposeFunc = Func;
}

void tkReshapeFunc(void (*Func)(GLsizei, GLsizei))
{
    ReshapeFunc = Func;
}

void tkDisplayFunc(void (*Func)(void))
{
    DisplayFunc = Func;
}

void tkKeyDownFunc(GLenum (*Func)(int, GLenum))
{
    KeyDownFunc = Func;
}

void tkMouseDownFunc(GLenum (*Func)(int, int, GLenum))
{
    MouseDownFunc = Func;
}

void tkMouseUpFunc(GLenum (*Func)(int, int, GLenum))
{
    MouseUpFunc = Func;
}

void tkMouseMoveFunc(GLenum (*Func)(int, int, GLenum))
{
    MouseMoveFunc = Func;
}

void tkIdleFunc(void (*Func)(void))
{
    IdleFunc = Func;

}

#ifdef DOSVGA
extern int vga_getxdim(void);
extern int vga_getydim(void);
#endif

void tkInitPosition(int x, int y, int width, int height)
{
#ifdef DOSVGA
	if(width>vga_getxdim()) width = vga_getxdim();
	if(height>vga_getydim()) height = vga_getydim();
#endif

   windInfo.bDefPos = FALSE;
   windInfo.x = x ;
   windInfo.y = y ;
   windInfo.width = width;
   windInfo.height = height;
}

void tkInitDisplayMode(GLenum type)
{
    windInfo.type = type;
}

void tkInitDisplayModePolicy(GLenum type)
{
    windInfo.dmPolicy = type;
}

GLenum tkInitDisplayModeID(GLint ipfd)
{
    windInfo.ipfd = ipfd;
    return GL_TRUE;
}

extern unsigned short vga_cindex ;


extern void restore_video_mode(void); /* see dosmesa.c */
extern void set_video_mode(unsigned short,unsigned short,char); /* see dosmesa.c */

void resetcrt(void)
{
	restore_video_mode();
}
GLenum tkInitWindowAW(char *title, BOOL bUnicode)
{
    DOSMesaContext Cur;
    GLenum   Result = GL_FALSE,RGB_Flag=GL_TRUE,DB_Flag=GL_FALSE;
	if( windInfo.type & TK_INDEX )
		set_video_mode(windInfo.x,windInfo.y,FALSE);
	 else
	 	set_video_mode(windInfo.x,windInfo.y,TRUE);

	atexit( resetcrt );

#if 0
    WNDCLASS wndclass;
    RECT     WinRect;
    HANDLE   hInstance;
    ATOM     aRegister;

    hInstance = GetModuleHandle(NULL);

    // Must not define CS_CS_PARENTDC style.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = (WNDPROC)tkWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;

    if (bUnicode)
        wndclass.lpszClassName = (LPCSTR)lpszClassNameW;
    else
        wndclass.lpszClassName = (LPCSTR)lpszClassName;

    if (bUnicode)
    {
        aRegister = RegisterClassW((CONST WNDCLASSW *)&wndclass);
    }
    else
    {
        aRegister = RegisterClass(&wndclass);
    }


    /*
     *  If the window failed to register, then there's no
     *  need to continue further.
     */

    if(0 == aRegister)
    {
        PrintMessage("Failed to register window class\n");
        return(Result);
    }


    /*
     *  Make window large enough to hold a client area as large as windInfo
     */

    WinRect.left   = windInfo.x;
    WinRect.right  = windInfo.x + windInfo.width;
    WinRect.top    = windInfo.y;
    WinRect.bottom = windInfo.y + windInfo.height;

    AdjustWindowRect(&WinRect, WS_OVERLAPPEDWINDOW, FALSE);

    /*
     *  Must use WS_CLIPCHILDREN and WS_CLIPSIBLINGS styles.
     */

    if (bUnicode)
    {
        tkhwnd = CreateWindowW(
                    (LPCWSTR)lpszClassNameW,
                    (LPCWSTR)title,
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                    (windInfo.bDefPos) ? CW_USEDEFAULT : WinRect.left,
                    (windInfo.bDefPos) ? CW_USEDEFAULT : WinRect.top,
                    WinRect.right - WinRect.left,
                    WinRect.bottom - WinRect.top,
                    NULL,
                    NULL,
                    hInstance,
                    NULL);
    }
    else
    {
        tkhwnd = CreateWindow(
                    lpszClassName,
                    title,
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                    (windInfo.bDefPos) ? CW_USEDEFAULT : WinRect.left,
                    (windInfo.bDefPos) ? CW_USEDEFAULT : WinRect.top,
                    WinRect.right - WinRect.left,
                    WinRect.bottom - WinRect.top,
                    NULL,
                    NULL,
                    hInstance,
                    NULL);
    }

    if ( NULL != tkhwnd )
    {
        // If default window positioning used, find out window position and fix
        // up the windInfo position info.

        if (windInfo.bDefPos)
        {
            GetWindowRect(tkhwnd, &WinRect);
            windInfo.x = WinRect.left + GetSystemMetrics(SM_CXFRAME);
            windInfo.y = WinRect.top  + GetSystemMetrics(SM_CYCAPTION)
                         - GetSystemMetrics(SM_CYBORDER)
                         + GetSystemMetrics(SM_CYFRAME);
        }

        tkhdc = GetDC(tkhwnd);

        if ( NULL != tkhdc )
            ShowWindow(tkhwnd, SW_SHOWDEFAULT);
        else
            PrintMessage("Could not get an HDC for window 0x%08lX\n", tkhwnd );
    }
    else
        PrintMessage("create window failed\n");
#endif


    if (windInfo.type & TK_INDEX)
    {
    	vga_cindex = 256 ;
      RGB_Flag=GL_FALSE;
      tkSetRGBMap(NCOLORS,(float *) tkRGBMap);
    }
    if (windInfo.type & TK_DOUBLE)
      DB_Flag=GL_TRUE;

    Cur=DOSMesaCreateContext();
    DOSMesaMakeCurrent(Cur);
    return GL_TRUE;
}

// Initialize a window, create a rendering context for that window
GLenum tkInitWindow(char *title)
{
    TKASSERT( NULL==tkhwnd      );
    TKASSERT( NULL==tkhdc       );
    TKASSERT( NULL==tkhrc       );
    TKASSERT( NULL==tkhpalette  );

    return tkInitWindowAW(title, FALSE);
}


/******************************************************************************/

/*
 * You cannot just call DestroyWindow() here.  The programs do not expect
 * tkQuit() to return;  DestroyWindow() just sends a WM_DESTROY message
 */

void tkQuit(void)
{
	exit(1);
#if 0
    DestroyThisWindow(tkhwnd);
    ExitProcess(0);
#endif
}

/******************************************************************************/

#if defined( __WATCOMC__)
void outpb(unsigned char byte);
#pragma aux outpb = \
	"mov dx,03c9h" \
	"out dx,al" \
	parm [ax] \
	modify exact [dx];
#endif

static unsigned char Rr,Gg,Bb,Cindex;
void  set_onecolor(int cindex,int R,int G,int B)
{
#if defined( __WATCOMC__ )
	outp(0x3c8,cindex);
	outpb(R);
	outpb(G);
	outpb(B);
#endif

#if defined( DJGPP )
	Rr=R;Gg=G;Bb=B;
	Cindex = cindex;

	asm ("
		movw	$0x3c8,%dx
		movw	_Cindex,%ax
		outb	%al,%dx
		inc	%dx
		movb	_Rr,%al
		outb	%al,%dx
		movb	_Gg,%al
		outb	%al,%dx
		movb	_Bb,%al
		outb	%al,%dx
	");
#endif
}

void tkSetOneColor(int index, float r, float g, float b)
{
	unsigned char R=r*63.0,G=g*63,B=b*63;

	set_onecolor(index,R,G,B);

#if 0
    PALETTEENTRY PalEntry;
    HPALETTE Palette;
    if ( NULL != (Palette = CreateCIPalette( tkhdc )) )
    {
        PalEntry.peRed   = (BYTE)(r*(float)255.0 + (float)0.5);
        PalEntry.peGreen = (BYTE)(g*(float)255.0 + (float)0.5);
        PalEntry.peBlue  = (BYTE)(b*(float)255.0 + (float)0.5);
        PalEntry.peFlags = 0;
        SetPaletteEntries( Palette, index, 1, &PalEntry);
        DelayPaletteRealization();
    }
#endif
}

void tkSetFogRamp(int density, int startIndex)
{
	UINT n, i, j, k, intensity, fogValues, colorValues;

	fogValues = 1 << density ;
	colorValues = 1 << startIndex ;
	for( i = 0 ; i < colorValues; i++ ) {
		for( j = 0 ; j < fogValues; j++ ) {
			k = i * fogValues + j;

			intensity = i * fogValues + j * colorValues;

			if( intensity > 0xff )
				intensity = 0xff;


			intensity/=4;

			set_onecolor(k,intensity,intensity,intensity);
		}
	}
}

void tkSetGreyRamp(void)
{
	UINT Count,i;
	float intensity;

	Count = tkGetColorMapSize();

	for( i = 0 ; i < Count ; i++ )
	{
		intensity = (float)(((double)i / (double)(Count-1)) * (double)255.0 + (double)0.5);
		intensity/=4;
		set_onecolor(i,intensity,intensity,intensity);
	}

#if 0
    HPALETTE CurrentPal;
    PALETTEENTRY *Entries;
    UINT Count, i;
    float intensity;

    if ( NULL != (CurrentPal = CreateCIPalette( tkhdc )) )
    {
        Count   = GetPaletteEntries( CurrentPal, 0, 0, NULL );
        Entries = AllocateMemory( Count * sizeof(PALETTEENTRY) );

        if ( NULL != Entries )
        {
            for (i = 0; i < Count; i++)
            {
                intensity = (float)(((double)i / (double)(Count-1)) * (double)255.0 + (double)0.5);
                Entries[i].peRed =
                Entries[i].peGreen =
                Entries[i].peBlue = (BYTE) intensity;
                Entries[i].peFlags = 0;
            }
            SetPaletteEntries( CurrentPal, 0, Count, Entries );
            FreeMemory( Entries );

            DelayPaletteRealization();
        }
    }
#endif
}

void tkSetRGBMap( int Size, float *Values )
{
    HPALETTE CurrentPal;
    int i;
    if ( NULL != (CurrentPal = CreateCIPalette( tkhdc )) )
    {
      for (i=0; i<Size; i++)
        tkSetOneColor(i,Values[i*3],Values[i*3+1],Values[i*3+2]);
    }
}

/******************************************************************************/

void tkSwapBuffers(void)
{
  DOSMesaSwapBuffers();
}

/******************************************************************************/

GLint tkGetColorMapSize(void)
{
#if 0
    CreateCIPalette( tkhdc );

    if ( NULL == tkhpalette )
        return( 0 );

    return( GetPaletteEntries( tkhpalette, 0, 0, NULL ) );
#endif
	return 256;

}

void tkGetMouseLoc(int *x, int *y)
{
#if 0
    POINT Point;

    *x = 0;
    *y = 0;

    GetCursorPos(&Point);

    /*
     *  GetCursorPos returns screen coordinates,
     *  we want window coordinates
     */

    *x = Point.x - windInfo.x;
    *y = Point.y - windInfo.y;
#endif
}

HWND tkGetHWND(void)
{
    return tkhwnd;
}

HDC tkGetHDC(void)
{
    return tkhdc;
}
GLenum tkGetDisplayModePolicy(void)
{
    return windInfo.dmPolicy;
}

GLint tkGetDisplayModeID(void)
{
    return windInfo.ipfd;
}

GLenum tkGetDisplayMode(void)
{
    return windInfo.type;
}


/***********************************************************************
 *                                                                     *
 *  The Following functions are for our own use only. (ie static)      *
 *                                                                     *
 ***********************************************************************/

long tkWndProc(HWND hWnd, UINT message, DWORD wParam, LONG lParam)
{
#if 0
    int key;
    PAINTSTRUCT paint;
    HDC hdc;

    switch (message)
    {

    case WM_USER:

        if ( RealizePaletteNow( tkhdc, tkhpalette) > 0 )
            ForceRedraw( hWnd );
        return(0);

    case WM_SIZE:
        windInfo.width  = LOWORD(lParam);
        windInfo.height = HIWORD(lParam);

        if (ReshapeFunc)
        {
            (*ReshapeFunc)(windInfo.width, windInfo.height);

            ForceRedraw( hWnd );
        }
        return (0);

    case WM_MOVE:
        windInfo.x = LOWORD(lParam);
        windInfo.y = HIWORD(lParam);
        return (0);

    case WM_PAINT:

        /*
         *  Validate the region even if there are no DisplayFunc.
         *  Otherwise, USER will not stop sending WM_PAINT messages.
         */

        hdc = BeginPaint(tkhwnd, &paint);

        if (DisplayFunc)
        {
            (*DisplayFunc)();
        }

        EndPaint(tkhwnd, &paint);
        return (0);

    case WM_PALETTECHANGED:
        if ( hWnd != (HWND) wParam )
          RealizePaletteNow(tkhdc,tkhpalette);
        return (0);
    case WM_QUERYNEWPALETTE:

    // In the foreground!  Let RealizePaletteNow do the work--
    // if management of the static system color usage is needed,
    // RealizePaletteNow will take care of it.

        if ( NULL != tkhpalette )
        {
            if ( RealizePaletteNow(tkhdc, tkhpalette) > 0 )
                ForceRedraw( hWnd );

            return (1);
        }

        return (0);

    case WM_ACTIVATE:

    // If the window is going inactive, the palette must be realized to
    // the background.  Cannot depend on WM_PALETTECHANGED to be sent since
    // the window that comes to the foreground may or may not be palette
    // managed.

        if ( LOWORD(wParam) == WA_INACTIVE )
        {
            if ( NULL != tkhpalette )
            {
            // Realize as a background palette.  Need to call
            // RealizePaletteNow rather than RealizePalette directly to
            // because it may be necessary to release usage of the static
            // system colors.

                if ( RealizePaletteNow( tkhdc, tkhpalette) > 0 )
                    ForceRedraw( hWnd );
            }
        }

    // Allow DefWindowProc() to finish the default processing (which includes
    // changing the keyboard focus).

        break;

    case WM_MOUSEMOVE:

        if (MouseMoveFunc)
        {
            GLenum mask;

            mask = 0;
            if (wParam & MK_LBUTTON) {
                mask |= TK_LEFTBUTTON;
            }
            if (wParam & MK_MBUTTON) {
                mask |= TK_MIDDLEBUTTON;
            }
            if (wParam & MK_RBUTTON) {
                mask |= TK_RIGHTBUTTON;
            }

            if ((*MouseMoveFunc)( LOWORD(lParam), HIWORD(lParam), mask ))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_LBUTTONDOWN:

        SetCapture(hWnd);

        if (MouseDownFunc)
        {
            if ( (*MouseDownFunc)(LOWORD(lParam), HIWORD(lParam),
                 TK_LEFTBUTTON) )
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_LBUTTONUP:

        ReleaseCapture();

        if (MouseUpFunc)
        {
            if ((*MouseUpFunc)(LOWORD(lParam), HIWORD(lParam), TK_LEFTBUTTON))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_MBUTTONDOWN:

        SetCapture(hWnd);

        if (MouseDownFunc)
        {
            if ((*MouseDownFunc)(LOWORD(lParam), HIWORD(lParam),
                    TK_MIDDLEBUTTON))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_MBUTTONUP:

        ReleaseCapture();

        if (MouseUpFunc)
        {
            if ((*MouseUpFunc)(LOWORD(lParam), HIWORD(lParam),
                TK_MIDDLEBUTTON))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_RBUTTONDOWN:

        SetCapture(hWnd);

        if (MouseDownFunc)
        {
            if ((*MouseDownFunc)(LOWORD(lParam), HIWORD(lParam),
                TK_RIGHTBUTTON))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_RBUTTONUP:

        ReleaseCapture();

        if (MouseUpFunc)
        {
            if ((*MouseUpFunc)(LOWORD(lParam), HIWORD(lParam),
                TK_RIGHTBUTTON))
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_SPACE:          key = TK_SPACE;         break;
        case VK_RETURN:         key = TK_RETURN;        break;
        case VK_ESCAPE:         key = TK_ESCAPE;        break;
        case VK_LEFT:           key = TK_LEFT;          break;
        case VK_UP:             key = TK_UP;            break;
        case VK_RIGHT:          key = TK_RIGHT;         break;
        case VK_DOWN:           key = TK_DOWN;          break;
        default:                key = GL_FALSE;         break;
        }

        if (key && KeyDownFunc)
        {
            GLenum mask;

            mask = 0;
            if (GetKeyState(VK_CONTROL)) {
                mask |= TK_CONTROL;
            }

            if (GetKeyState(VK_SHIFT)) {

                mask |= TK_SHIFT;
            }

            if ( (*KeyDownFunc)(key, mask) )
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_CHAR:
        if (('0' <= wParam && wParam <= '9') ||
            ('a' <= wParam && wParam <= 'z') ||
            ('A' <= wParam && wParam <= 'Z')) {

            key = wParam;
        } else {
            key = GL_FALSE;
        }

        if (key && KeyDownFunc) {
            GLenum mask;

            mask = 0;

            if (GetKeyState(VK_CONTROL)) {
                mask |= TK_CONTROL;
            }

            if (GetKeyState(VK_SHIFT)) {
                mask |= TK_SHIFT;
            }

            if ( (*KeyDownFunc)(key, mask) )
            {
                ForceRedraw( hWnd );
            }
        }
        return (0);

    case WM_CLOSE:
        DestroyWindow(tkhwnd);
        return(0);

    case WM_DESTROY:
        CleanUp();
        PostQuitMessage(TRUE);
        return 0;
    }
    return(DefWindowProc( hWnd, message, wParam, lParam));
#endif
	return 1;
}

static HPALETTE CreateCIPalette( HDC Dc )
{
#if 0
    LOGPALETTE *LogicalPalette;
    HPALETTE StockPalette;
    UINT PaletteSize, StockPaletteSize, EntriesToCopy;

    if ( (Dc != NULL) && (NULL == tkhpalette) )
    {
                PaletteSize = 256; //(Pfd.cColorBits >= 8) ? 256 : (1 << Pfd.cColorBits);

                LogicalPalette = AllocateZeroedMemory( sizeof(LOGPALETTE) +
                                        (PaletteSize * sizeof(PALETTEENTRY)) );

                if ( NULL != LogicalPalette )
                {
                    LogicalPalette->palVersion    = 0x300;
                    LogicalPalette->palNumEntries = PaletteSize;

                    StockPalette     = GetStockObject(DEFAULT_PALETTE);
                    StockPaletteSize = GetPaletteEntries( StockPalette, 0, 0, NULL );

                    /*
                     *  start by copying default palette into new one
                     */

                    EntriesToCopy = StockPaletteSize < PaletteSize ?
                                        StockPaletteSize : PaletteSize;

                    GetPaletteEntries( StockPalette, 0, EntriesToCopy,
                                        LogicalPalette->palPalEntry );

                    /*
                     *  If we are taking possession of the system colors,
                     *  must guarantee that 0 and 255 are black and white
                     *  (respectively).
                     */

                    tkhpalette = CreatePalette(LogicalPalette);

                    FreeMemory(LogicalPalette);

                    RealizePaletteNow( Dc, tkhpalette);
                }
            }
    return( tkhpalette );
#endif
}
static void
PrintMessage( const char *Format, ... )
{
    va_list ArgList;
    char Buffer[256];

    va_start(ArgList, Format);
    vsprintf(Buffer, Format, ArgList);
    va_end(ArgList);
}

static void DelayPaletteRealization( void )
{
#if 0
    MSG Message;

    TKASSERT(NULL!=tkhwnd);

    /*
     *  Add a WM_USER message to the queue, if there isn't one there already.
     */

    if (!PeekMessage(&Message, tkhwnd, WM_USER, WM_USER, PM_NOREMOVE) )
    {
        PostMessage( tkhwnd, WM_USER, 0, 0);
    }
#endif
}

/******************************Public*Routine******************************\
* RealizePaletteNow
*
* Select the given palette in background or foreground mode (as specified
* by the bForceBackground flag), and realize the palette.
*
* If static system color usage is set, the system colors are replaced.
*
* History:
*  26-Apr-1994 -by- Gilman Wong [gilmanw]
* Wrote it.
\**************************************************************************/

static long RealizePaletteNow( HDC Dc, HPALETTE Palette)
{
#if 0
    long Result = -1;
    TKASSERT( NULL!=Dc      );
    TKASSERT( NULL!=Palette );
    if ( NULL != SelectPalette( Dc, Palette, FALSE ) )
    {
      Result = RealizePalette( Dc );
      DOSMesaPaletteChange(Palette);
    }
    return( Result );

#endif
}

static void ForceRedraw( HWND Window )
{
#if 0
    MSG Message;

    if (!PeekMessage(&Message, Window, WM_PAINT, WM_PAINT, PM_NOREMOVE) )
    {
        InvalidateRect( Window, NULL, FALSE );
    }
#endif
}
static void DestroyThisWindow( HWND Window )
{
#if 0
    if ( NULL != Window )
    {
        DestroyWindow( Window );
    }
#endif
}

/*
 *  This Should be called in response to a WM_DESTROY message
 */

static void CleanUp( void )
{

// Be really nice and reset global values.
    tkhwnd        = NULL;
    tkhdc         = NULL;
    tkhpalette    = NULL;

    ExposeFunc    = NULL;
    ReshapeFunc   = NULL;
    IdleFunc      = NULL;
    DisplayFunc   = NULL;
    KeyDownFunc   = NULL;
    MouseDownFunc = NULL;
    MouseUpFunc   = NULL;
    MouseMoveFunc = NULL;
}

static void *AllocateMemory( size_t Size )
{
    return( malloc( Size ) );
}

static void *AllocateZeroedMemory( size_t Size )
{
	void *temp = malloc(Size);
	memset(temp,0,Size);
	return( temp );
}


static void FreeMemory( void *Chunk )
{
    TKASSERT( NULL!=Chunk );

    free( Chunk );
}
