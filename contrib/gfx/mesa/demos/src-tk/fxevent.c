/* event.c modified for 3Dfx driver */

#if defined(FX)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if defined(__WIN32__)
#include <windows.h>
#include <conio.h>
#endif

#include "gltk.h"
#include "GL/fxmesa.h"

/******************************************************************************/

void (*ExposeFunc)(int, int) = 0;
void (*ReshapeFunc)(int, int) = 0;
void (*DisplayFunc)(void) = 0;
GLenum (*KeyDownFunc)(int, GLenum) = 0;
GLenum (*MouseDownFunc)(int, int, GLenum) = 0;
GLenum (*MouseUpFunc)(int, int, GLenum) = 0;
GLenum (*MouseMoveFunc)(int, int, GLenum) = 0;
void (*IdleFunc)(void) = 0;

static int width,height;
static int startx = 0,starty = 0;
static fxMesaContext fc=NULL;
static int WindowType = 0;

/******************************************************************************/
#ifdef __linux__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

GLenum tkSetWindowLevel(GLenum level)
{
    return GL_TRUE;
}

void tkNewCursor(GLint id, GLubyte *shapeBuf, GLubyte *maskBuf, GLenum fgColor,
                 GLenum bgColor, GLint hotX, GLint hotY)
{
}

void tkSetCursor(GLint id)
{
}

void tkGetMouseLoc (int *x, int *y)
{
        *x = (int)0;
        *y = (int)0;
}

void tkGetSystem(TKenum type, void *ptr)
{
        type = (TKenum)NULL;
        ptr = NULL;
}

void tkCloseWindow() {}

void tkSetFogRamp(int density, int startIndex) {}

#define NCOLORS 8
float tkRGBMap[NCOLORS][3] = {
    {0,0,0},
    {1,0,0},
    {0,1,0},
    {1,1,0},
    {0,0,1},
    {1,0,1},
    {0,1,1},
    {1,1,1}
};

static int sg_tty_fd = -1;

int 
tty_open_nonblock (void)
{
  struct termios term;

  sg_tty_fd = open ("/dev/tty", O_RDONLY, -1, -1);
  if (sg_tty_fd < 0)
  {
    printf ("error opening /dev/tty\n");
    return (sg_tty_fd);
  }


  if (tcgetattr (sg_tty_fd, &term) == -1)
    perror ("tty_open_nonblock:tcgetattr");

  /*
   * If the user wants signals to be valid so that things like suspend
   * and interrupt (^Z & ^C) work for terminal-connected lines,
   * we need to set the ISIG flag.  We also want to post process
   * newlines in this case so they get mapped to CRNL
   */
                                                    
  term.c_lflag = ISIG;
  term.c_oflag = OPOST | ONLCR;

  term.c_iflag &= ~INPCK;
  term.c_cflag &= ~PARENB;

  term.c_iflag = 0;
  term.c_lflag &= ~ICANON;
  term.c_lflag &= ~ECHO;
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;
  term.c_cflag |= CLOCAL | CS8 | CREAD;

  if (tcsetattr (sg_tty_fd, TCSADRAIN, &term) == -1)
    perror ("tty_open_nonblock:tcflush");

  if (fcntl (sg_tty_fd, F_SETFL, O_NONBLOCK) == -1)
    perror ("tty_open_nonblock:fcntl");

  if (tcflush (sg_tty_fd, TCIFLUSH) == -1)
    perror ("tty_open_nonblock:tcflush");
  return 0;
}

int 
tty_read (int *charptr)
{
  int rc;

  rc = read (sg_tty_fd, charptr, 1);
  return (rc);
}

#elif !defined(__WIN32__)	/* DOS section */

int _tkkbhit(void )
{
	return(kbhit());
}

int _tkgetch(void )
{
	return(getch());
}

#else

static HWND		hWndMain = 0;
static HANDLE	hMainInstance = 0;
static int		fullscreen = GL_TRUE;
static int		qhead = 0;
static int		qtail = 0;
static int		queue[256] = {0};
static char		*fakeName = "WinTkMesa3DfxApplication";
static char		*argvbuf[32];
static char		cmdLineBuffer[1024];

char **commandLineToArgv(LPSTR lpCmdLine,int *pArgc);

/*
 * MainWndproc
 *
 * Callback for all Windows messages
 */
long FAR PASCAL MainWndproc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT	ps;
	HDC			hdc;

	switch(message)
	{
		case WM_SETCURSOR:
			if(!fullscreen)
			{
				SetCursor(NULL);
				return(0);
			}
			break;

		case WM_CREATE:
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd,&ps);
			EndPaint(hWnd,&ps);
			return(1);
		
		case WM_CLOSE:
			tkQuit();
			break;

		case WM_DESTROY:
			break;

		case WM_MOVE:
			if(!grSstControl(GR_CONTROL_MOVE))
			{
				tkQuit();
				return(0);
			}
			break;

		case WM_DISPLAYCHANGE:
		case WM_SIZE:
			{
				RECT	rect;

				GetClientRect(hWndMain,&rect);
				if(ReshapeFunc)
					(*ReshapeFunc)(rect.right,rect.bottom);
			}
			if(!grSstControl(GR_CONTROL_RESIZE))
			{
				tkQuit();
				return(0);
			}
			break;

		case WM_ACTIVATE:
			{
				WORD	fActive = LOWORD(wParam);
				BOOL	fMinimized = (BOOL) HIWORD(wParam);

				if((fActive == WA_INACTIVE) || fMinimized)
					grSstControl(GR_CONTROL_DEACTIVATE);
				else
					grSstControl(GR_CONTROL_ACTIVATE);
			}
			break;

		case WM_KEYDOWN: 
			switch (wParam) { 
			case VK_LEFT:
				queue[qhead++] = 0;
				qhead &= 255;
				queue[qhead++] = 'K';
				qhead &= 255;
				break;
			case VK_RIGHT:
				queue[qhead++] = 0;
				qhead &= 255;
				queue[qhead++] = 'M';
				qhead &= 255;
				break;
			case VK_UP:
				queue[qhead++] = 0;
				qhead &= 255;
				queue[qhead++] = 'H';
				qhead &= 255;
				break;
   		case VK_DOWN:
				queue[qhead++] = 0;
				qhead &= 255;
				queue[qhead++] = 'P';
				qhead &= 255;
				break;
			}
			break;

		case WM_CHAR:
			if(!isascii(wParam))
				break;
			queue[qhead++] = wParam;
			qhead &= 255;
			break;

		default:
			break;
	}
	return(DefWindowProc(hWnd,message,wParam,lParam));
} /* MainWndproc */

/*
 * initApplication
 *
 * Do that Windows initialization stuff...
 */
static BOOL initApplication(HANDLE hInstance,int nCmdShow)
{
	WNDCLASS	wc;
	BOOL		rc;

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = MainWndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);    /* generic icon */
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = "WinTkMesa3DfxClass";
	rc = RegisterClass(&wc);
	if(!rc)
		return(FALSE);
	hMainInstance = hInstance;
	return(TRUE);
} /* initApplication */

/*
 * WinMain
 */
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,int nCmdShow)
{
	int			argc;
	char		**argv;
	extern int	main(int argc,char **argv);

	if(!initApplication(hInstance,nCmdShow))
		return(FALSE);
	argv = commandLineToArgv(lpCmdLine,&argc);
	main(argc,argv);
	if(hWndMain)
	{
		DestroyWindow(hWndMain);
		hWndMain = 0;
	}
    return(FALSE);
} /* WinMain */

/*
 * Converts lpCmdLine to WinMain into argc, argv
 */
char **commandLineToArgv(LPSTR lpCmdLine,int *pArgc)
{
	char	*p,*pEnd;
	int		argc = 0;

	argvbuf[argc++] = fakeName;
	if(lpCmdLine == NULL)
	{
		*pArgc = argc;
		return(argvbuf);
	}
	strcpy(cmdLineBuffer,lpCmdLine);
	p = cmdLineBuffer;
	pEnd = p + strlen(cmdLineBuffer);
	if(pEnd >= &cmdLineBuffer[1022])
		pEnd = &cmdLineBuffer[1022];
	while(1)
	{
		/* skip over white space */
		while(*p == ' ')
			p++;
		if(p >= pEnd)
			break;
		argvbuf[argc++] = p;
		if(argc >= 32)
			break;
		/* skip till there's a 0 or a white space */
		while(*p && (*p != ' '))
			p++;
		if(*p == ' ')
			*p++ = 0;
	}
	*pArgc = argc;
	return(argvbuf);
}

int _tkkbhit(void)
{
	MSG		msg;

	if(qhead != qtail)
		return(1);
    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);      /* this might change qhead */
		if(qhead != qtail)
			return(1);
	}
	return(0);
}

char _tkgetch(void)
{
	MSG		msg;
	char	rv;

	if(qtail != qhead)
	{
		rv = queue[qtail++];
		qtail &= 255;
		return(rv);
	}
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(qtail != qhead)
		{
			rv = queue[qtail++];
			qtail &= 255;
			return(rv);
		}
	}
	/* Should never get here!! */
	/* printf("Bad exit..\n"); */
	/* fflush(stdout); */
}

#endif

void tkExec(void)
{
	static int firsttime=1;
	int key;
	int ch;
	GLenum mask=0;

#ifdef __linux__
    if (firsttime) {
        if (ReshapeFunc)
            (*ReshapeFunc)(width, height);

        tty_open_nonblock ();
        firsttime = 0;
        ch = 0;
    }

    while (ch!=0x1b) {
       /* TODO: block on tty_read() if there's no idle loop! */
       key = tty_read ( &ch );
                  
       if ( key >= 0 ) {
          ch &= 0xFF;
#else
    if(ReshapeFunc && firsttime) {
       (*ReshapeFunc)(width,height);
       firsttime=0;
       ch=0;
    }

    while (ch!=27) {
       if(_tkkbhit()) {
          ch = _tkgetch();

          if(KeyDownFunc) {
#endif
				switch(ch) {
					case ' ':
						key = TK_SPACE;
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
#ifdef __linux__
                                        case 0x1b:
                                                key = tty_read ( &ch );
                                                if (key < 0)
                                                {
                                                    key = TK_ESCAPE;
                                                    break;
                                                }

                                                key = tty_read ( &ch );
                                                if (key < 0)
                                                    break;

                                                ch &= 0xFF;
                                                switch ( ch ) {
                                                        case 'A':
                                                                key = TK_UP;
                                                                break;
                                                        case 'B':
                                                                key = TK_DOWN;
                                                                break;
                                                        case 'C':
                                                                key = TK_RIGHT;
                                                                break;
                                                        case 'D':
                                                                key = TK_LEFT;
                                                                break;
                                                }
                                                break;
#else
					case 27:
						key = TK_ESCAPE;
						break;
					case 0:
					case 0xe0:
						ch = _tkgetch();
						switch( ch ) {
							case 'P':
								key = TK_DOWN;
								break;
							case 'M':
								key = TK_RIGHT;
								break;
							case 'K':
								key = TK_LEFT;
								break;
							case 'H':
								key = TK_UP;
								break;
						}
						break;
#endif
					/*default:
						printf("\n====%x %c %d\n",ch,ch,ch);
						break;*/
					}
					(*KeyDownFunc)(key,mask);
					key = 0;
				}
#ifndef __linux__
			}
#endif

			if (IdleFunc)
				(*IdleFunc)();

			if(DisplayFunc)
				(*DisplayFunc)();
        }

}



/******************************************************************************/

void tkExposeFunc(void (*Func)(int, int))
{

    ExposeFunc = Func;
}

/******************************************************************************/

void tkReshapeFunc(void (*Func)(int, int))
{

    ReshapeFunc = Func;
}

/******************************************************************************/

void tkDisplayFunc(void (*Func)(void))
{

    DisplayFunc = Func;
}

/******************************************************************************/

void tkKeyDownFunc(GLenum (*Func)(int, GLenum))
{

    KeyDownFunc = Func;
}

/******************************************************************************/

void tkMouseDownFunc(GLenum (*Func)(int, int, GLenum))
{

    MouseDownFunc = Func;
}

/******************************************************************************/

void tkMouseUpFunc(GLenum (*Func)(int, int, GLenum))
{

    MouseUpFunc = Func;
}

/******************************************************************************/

void tkMouseMoveFunc(GLenum (*Func)(int, int, GLenum))
{

    MouseMoveFunc = Func;
}

/******************************************************************************/

void tkIdleFunc(void (*Func)(void))
{

    IdleFunc = Func;
}

/******************************************************************************/

void tkInitDisplayMode(GLenum type)
{
#ifdef WIN32
	if(!hMainInstance)
		initApplication(0,FALSE);
#endif
        WindowType = type;
}

void tkInitPosition(int x, int y, int w, int h)
{
   startx = x;
   starty = y;

#ifdef __linux__
   /* larger sizes don't seem to work */
   if (w>640)
      w = 640;
   if (h>480)
      h = 480;
#endif
   width=w;
   height=h;
}

void tkSwapBuffers(void)
{
	fxMesaSwapBuffers();
}

void tkQuit(void)
{
	fxMesaDestroyContext(fc);
#ifdef WIN32
	if(hWndMain)
		DestroyWindow(hWndMain);
	hWndMain = 0;
#endif
	exit(0);
}

GLenum tkInitWindow(char *title)
{
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

   /* Build fxMesa attribute list */
   i = 0;
   if (TK_IS_DOUBLE(WindowType)) {
      attribs[i] = FXMESA_DOUBLEBUFFER;
      i++;
   }
   if (TK_HAS_DEPTH(WindowType)) {
      attribs[i] = FXMESA_DEPTH_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_ALPHA(WindowType)) {
      attribs[i] = FXMESA_ALPHA_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_ACCUM(WindowType)) {
      attribs[i] = FXMESA_ACCUM_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   if (TK_HAS_STENCIL(WindowType)) {
      attribs[i] = FXMESA_STENCIL_SIZE;
      i++;
      attribs[i] = 1;
      i++;
   }
   attribs[i] = FXMESA_NONE;  /* end of list */

   /* find smallest screen size >= requested size */
   for(i=0;i<NUM_RES;i++)
      if((width<=xres[i]) && (height<=yres[i]))
         break;

#ifdef __WIN32__
   if(i == NUM_RES)
      fullscreen = GL_FALSE;
   hWndMain = CreateWindowEx(WS_EX_APPWINDOW,"WinTkMesa3DfxClass",title,
                             WS_OVERLAPPED | WS_CAPTION  | WS_THICKFRAME |
                             WS_MAXIMIZEBOX | WS_MINIMIZEBOX | 
                             WS_VISIBLE |    /* so we don't have to call ShowWindow */
                             WS_POPUP |      /* non-app window */
                             WS_SYSMENU,     /* so we get an icon in the tray */
                             startx,starty,
                             fullscreen ? 200 : width,
                             fullscreen ? 200 : height,
                             NULL,NULL,hMainInstance,NULL);
   if(!hWndMain)
      return(GL_FALSE);
   ShowWindow(hWndMain,SW_NORMAL);
   UpdateWindow(hWndMain);
   window = (GLuint)(fullscreen ? 0 : hWndMain);
#else
   /* linux */
   if(i == NUM_RES)
      i = NUM_RES-1;  /* largest possible */
   window = 0;
#endif
   width = xres[i];
   height = yres[i];
   fc = fxMesaCreateContext(window, res[i], GR_REFRESH_75Hz, attribs);
   if(!fc) {
      printf("Error creating fxMesa context\n");
      return(GL_FALSE);
   }
   fxMesaMakeCurrent(fc);
   return(GL_TRUE);
}

void tkSetOneColor(int index, float r, float g, float b)
{
}

void tkSetGreyRamp(void)
{
}

void tkSetRGBMap( int Size, float *Values )
{
}

GLint tkGetColorMapSize(void)
{
	return 256;
}

#else

void tk_dummy_func(void)
{
}

#endif /*defined(FX)*/
