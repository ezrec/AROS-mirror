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

#include <KernelKit.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gltk.h"
#ifdef FX
#include "GL/fxmesa.h"
#else /* FX */
#include "GL/osmesa.h"
#endif /* FX */

static struct _WINDOWINFO {
    int x, y;
    int width, height;
    GLenum type;
    GLenum dmPolicy;
    int ipfd;
    bool bDefPos;
    char *title;
} windInfo = {
    50, 30, 320, 200, (GLenum)(TK_RGB | TK_SINGLE), (GLenum)0, 0, TRUE, "TKView"
};

static void (*ExposeFunc)(int, int)              = NULL;
static void (*ReshapeFunc)(GLsizei, GLsizei)     = NULL;
static void (*DisplayFunc)(void)                 = NULL;
static GLenum (*KeyDownFunc)(int, GLenum)        = NULL;
static GLenum (*MouseDownFunc)(int, int, GLenum) = NULL;
static GLenum (*MouseUpFunc)(int, int, GLenum)   = NULL;
static GLenum (*MouseMoveFunc)(int, int, GLenum) = NULL;
static void (*IdleFunc)(void)                    = NULL;

float tkRGBMap[8][3] = {
    {
	0, 0, 0
    },
    {
	1, 0, 0
    },
    {
	0, 1, 0
    },
    {
	1, 1, 0
    },
    {
	0, 0, 1
    },
    {
	1, 0, 1
    },
    {
	0, 1, 1
    },
    {
	1, 1, 1
    }
};

static uchar idxlist[256];
#ifndef FX
static uchar *tmpout;
static uchar *bits;
#endif /* not FX */

/***************************************************************
 *                                                             *
 *  BeOS stuff                                                 *
 *                                                             *
 ***************************************************************/

const 	ulong 			APP_SIGNATURE = '????';
const 	ulong			MSG_REDRAW = 1;

class 					MesaWindow;
class 					MesaView;
class					MesaApp;

// Lets make our life easy and make them global:

#ifndef FX
OSMesaContext 			ctx;	
BBitmap 				*the_bitmap;
#else /* FX */
fxMesaContext			ctx;
#endif /* FX */
MesaView				*the_view;	
MesaWindow 			    *the_window;
MesaApp 				*the_app;

class MesaView : public BView
{
	BRect Bound;

	public:
	
	MesaView(BRect frame):BView(frame,"Mesa View",B_FOLLOW_NONE,B_WILL_DRAW)
	{
		Bound=Bounds();
	};
	
	void MouseDown(BPoint pos)
	{
		ulong buttons;
		ulong mask;

		Window()->Lock();
		GetMouse(&pos,&buttons);
		Window()->Unlock();

		if(MouseDownFunc)
		{

        	mask = 0;
        	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
            	mask |= TK_LEFTBUTTON;
        	}
        	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
            	mask |= TK_MIDDLEBUTTON;
        	}
        	if (buttons & B_TERTIARY_MOUSE_BUTTON) {
            	mask |= TK_RIGHTBUTTON;
        	}

			(*MouseDownFunc)(pos.x,pos.y,(GLenum)mask);
			if( DisplayFunc)
    		{
    			(*DisplayFunc)();
	    		Window()->Lock();
   	 			Draw(Bound);
   	 			Window()->Unlock();
    		}
		}

		while(buttons)
		{
			Window()->Lock();
			GetMouse(&pos,&buttons);
			Window()->Unlock();

			if(MouseMoveFunc)
			{
			
            	mask = 0;
            	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
                	mask |= TK_LEFTBUTTON;
            	}
            	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
                	mask |= TK_MIDDLEBUTTON;
            	}
            	if (buttons & B_TERTIARY_MOUSE_BUTTON) {
                	mask |= TK_RIGHTBUTTON;
            	}
				(*MouseUpFunc)(pos.x,pos.y,(GLenum)mask);
				if( DisplayFunc)
    			{
    				(*DisplayFunc)();
	    			Window()->Lock();
   	 				Draw(Bound);
   	 				Window()->Unlock();
    			}
			}
		}

		if(MouseUpFunc)
		{
        	mask = 0;
        	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
            	mask |= TK_LEFTBUTTON;
        	}
        	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
            	mask |= TK_MIDDLEBUTTON;
        	}
        	if (buttons & B_TERTIARY_MOUSE_BUTTON) {
            	mask |= TK_RIGHTBUTTON;
        	}

			(*MouseUpFunc)(pos.x,pos.y,(GLenum)mask);
			if( DisplayFunc)
    		{
    			(*DisplayFunc)();
	    		Window()->Lock();
   	 			Draw(Bound);
   	 			Window()->Unlock();
    		}
		}
	};
	
	void KeyDown(const char *bytes, int32 numBytes)
	{
		int key;
		GLenum mask=(GLenum)0;
		
		if(numBytes == 1)
		{
			switch(bytes[0])
		{
			case B_SPACE:
					key = TK_SPACE;
					break;
			case B_ESCAPE:
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
			case B_LEFT_ARROW:
					key = TK_LEFT;
					break;
			case B_RIGHT_ARROW:
					key = TK_RIGHT;
					break;
			case B_UP_ARROW:
					key = TK_UP;
					break;
			case B_DOWN_ARROW:
					key = TK_DOWN;
					break;
			default:
						inherited::KeyDown(bytes, numBytes);
						return;
			}
		}
		else
		{
			inherited::KeyDown(bytes, numBytes);
					return;
		}
		if(KeyDownFunc)
		{
			(*KeyDownFunc)(key,mask);
		}
		if(DisplayFunc)
    	{
    		(*DisplayFunc)();
#ifndef FX
	    	Window()->Lock();
   	 		Draw(Bound);
   	 		Window()->Unlock();
#endif /* not FX */
    	}
	};
	
	void Draw(BRect frame)
	{
#ifndef FX
		DrawBitmap(the_bitmap,BPoint(0,0));
#endif /* not FX */
	};					
};

class MesaWindow : public BWindow 
{
	public:
	
	MesaWindow(int width, int height):BWindow(BRect(0,0,width-1,height-1),"MesaView",B_TITLED_WINDOW,B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
	{
#ifndef FX
		MoveTo(windInfo.x,windInfo.y);
#else /* FX */
		BRect scr;
		{
		BScreen s; // local so it is destroyed quickly
		scr = s.Frame();
		}
		MoveTo(0, 0);
		ResizeTo(width = scr.bottom, height = scr.right);
#endif /* FX */
		Lock();
		AddChild(the_view = new MesaView(BRect(0, 0, width, height)));
		the_view->MakeFocus();
		Unlock();
	};
	
	void MessageReceived(BMessage *msg)
	{
		switch(msg->what)
		{	
			case	MSG_REDRAW:
#ifndef FX
					Lock();
					the_view->DrawBitmap(the_bitmap,BPoint(0,0));
					Unlock();
#endif /* not FX */
			       	if (IdleFunc)
			        {
            			(*IdleFunc)();
						PostMessage(MSG_REDRAW);
					}
					break;
			default:
					BWindow::MessageReceived(msg);
					break;
		}
	};
	
	bool QuitRequested(void)
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return TRUE;
	};
};

class MesaApp : public BApplication 
{
	public:
	
	MesaApp():BApplication(APP_SIGNATURE)
	{
		the_window = NULL;
#ifndef FX
		the_bitmap = NULL;
#endif /* not FX */
	};
			
	void ReadyToRun(void)
	{
		the_window = new MesaWindow(windInfo.width,windInfo.height);
		the_window->Show();
		if(ExposeFunc)
		{
			(*ExposeFunc)(windInfo.width,windInfo.height);
		}		
		if(ReshapeFunc)
		{
			(*ReshapeFunc)(windInfo.width,windInfo.height);
		}
		if( DisplayFunc)
    	{
    		(*DisplayFunc)();
    	}
		the_window->PostMessage(MSG_REDRAW);
	};

	bool QuitRequested(void)
	{
		if (BApplication::QuitRequested()) 
		{
#ifndef FX
			if (the_bitmap)	delete the_bitmap;
			if (ctx) OSMesaDestroyContext( ctx );
#else /* FX */
			if (ctx) fxMesaDestroyContext( ctx );
#endif /* FX */
			return TRUE;
		}
		return FALSE;
	};
			
	void AboutRequested(void)
	{
		char str[256];
		sprintf(str, "MesaDemos, ported by Tinic Urou\n<5uro@informatik.uni-hamburg.de>\nFreely distributable.");
		BAlert *the_alert = new BAlert("", str, "OK");
		the_alert->Go();
	};
};


/***************************************************************
 *                                                             *
 *  Exported Functions go here                                 *
 *                                                             *
 ***************************************************************/

void tkErrorPopups(GLboolean bEnable)
{
}

void tkCloseWindow(void)
{
}

void tkExec(void)
{
	the_app->Run();
	delete the_app;
	exit(0);
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

void tkInitPosition(int x, int y, int width, int height)
{
   windInfo.bDefPos = FALSE;
   windInfo.x = x;
   windInfo.y = y;
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

GLenum tkInitWindowAW(char *title, bool bUnicode)
{
#ifdef FX
#define NUM_RES 3

   static GrScreenResolution_t res[NUM_RES+1] = {
      GR_RESOLUTION_512x384,
      GR_RESOLUTION_640x480,
      GR_RESOLUTION_800x600,
      GR_RESOLUTION_NONE};
   static int xres[NUM_RES]={512,640,800};
   static int yres[NUM_RES]={384,480,600};
   int i;
   GLuint window;
   GLint attribs[100];
#else	/* FX */
	int realw;
	color_space type;
#endif /* FX */
	
 	windInfo.title=title;
	
	// What a hack!
	the_app = new MesaApp();

#ifndef FX
	switch(windInfo.type)
	{
		case	TK_RGB:
				type=B_RGB_32_BIT;
				break;
		case	TK_INDEX:
				type=B_COLOR_8_BIT;
				break;
	}

	// other mode does not work now
	type=B_RGB_32_BIT;
	windInfo.type=(GLenum)TK_RGB;

	the_bitmap = new BBitmap(BRect(0, 0, windInfo.width,windInfo.height),type);
	bits = (uchar *)the_bitmap->Bits();

	switch(type)
	{
		case	B_RGB_32_BIT:
				realw=the_bitmap->BytesPerRow()/sizeof(ulong);
				ctx = OSMesaCreateContext( GL_RGBA, NULL );
				tmpout=0;
				memset(bits,0,the_bitmap->BytesPerRow()*windInfo.height);
				OSMesaMakeCurrent( ctx, bits, GL_UNSIGNED_BYTE,realw,windInfo.height);
				break;
		case	B_COLOR_8_BIT:
				realw=the_bitmap->BytesPerRow();
				ctx = OSMesaCreateContext( GL_COLOR_INDEX, NULL );
				tmpout=new uchar[the_bitmap->BytesPerRow()*windInfo.height];
				memset(tmpout,0,the_bitmap->BytesPerRow()*windInfo.height);
				OSMesaMakeCurrent( ctx, tmpout, GL_UNSIGNED_BYTE,realw,windInfo.height);
				break;
	}
	OSMesaPixelStore( OSMESA_Y_UP, 0 );
#else /* FX */
   /* Build fxMesa attribute list */
   i = 0;
   if (TK_IS_DOUBLE(windInfo.type)) {
      attribs[i] = FXMESA_DOUBLEBUFFER;
      i++;
   }
   if (TK_HAS_DEPTH(windInfo.type)) {
      attribs[i] = FXMESA_DEPTH_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_ALPHA(windInfo.type)) {
      attribs[i] = FXMESA_ALPHA_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_ACCUM(windInfo.type)) {
      attribs[i] = FXMESA_ACCUM_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_STENCIL(windInfo.type)) {
      attribs[i] = FXMESA_STENCIL_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   attribs[i] = FXMESA_NONE;  /* end of list */

   /* find smallest screen size >= requested size */
   for(i=0;i<NUM_RES;i++)
      if((windInfo.width<=xres[i]) && (windInfo.height<=yres[i]))
         break;

   if(i == NUM_RES)
      i = NUM_RES-1;  /* largest possible */
   window = 0;

   windInfo.width = xres[i];
   windInfo.height = yres[i];

   ctx = fxMesaCreateContext(window, res[i], GR_REFRESH_75Hz, attribs);
   if(!ctx) {
      return(GL_FALSE);
   }
   fxMesaMakeCurrent(ctx);
#endif /* FX */

    return GL_TRUE;
}

GLenum tkInitWindow(char *title)
{
    return tkInitWindowAW(title, FALSE);
}


/******************************************************************************/

/*
 * You cannot just call DestroyWindow() here.  The programs do not expect
 * tkQuit() to return;  DestroyWindow() just sends a WM_DESTROY message
 */

void tkQuit(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
}

/******************************************************************************/

void set_onecolor(int k, uchar r, uchar g, uchar b)
{
	BScreen s;
	idxlist[(k&0xFF)]=s.IndexForColor(r,g,b);
}

void tkSetOneColor(int index, float r, float g, float b)
{
	unsigned char R=r*255.0,G=g*255.0,B=b*255.0;
	set_onecolor(index,R,G,B);
}

void tkSetFogRamp(int density, int startIndex)
{
	int n, i, j, k, intensity, fogValues, colorValues;

	fogValues = 1 << density ;
	colorValues = 1 << startIndex ;
	for( i = 0 ; i < colorValues; i++ ) {
		for( j = 0 ; j < fogValues; j++ ) {
			k = i * fogValues + j;

			intensity = i * fogValues + j * colorValues;

			if( intensity > 0xff )
				intensity = 0xff;

			set_onecolor(k,intensity,intensity,intensity);
		}
	}
}

void tkSetGreyRamp(void)
{
	int Count,i;
	float intensity;
	Count = tkGetColorMapSize();
	for( i = 0 ; i < Count ; i++ )
	{
		intensity = (float)(((double)i / (double)(Count-1)) * (double)255.0 + (double)0.5);
		set_onecolor(i,intensity,intensity,intensity);
	}
}

void tkSetRGBMap( int Size, float *Values )
{
	for (int i=0; i<Size; i++) tkSetOneColor(i,Values[i*3],Values[i*3+1],Values[i*3+2]);
}

void tkSwapBuffers(void)
{
#ifdef FX
	fxMesaSwapBuffers();
#endif /* FX */
}

GLint tkGetColorMapSize(void)
{
	return 256;
}

void tkGetMouseLoc(int *x, int *y)
{
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
