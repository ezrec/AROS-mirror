/* KDP Elate/Ami port
   wrecK/KnP YhdZ/KnP

   since KDP was intended for full screen 320x200 8 bit operation,
   some things dont work as they should (esp. the mouse function)

   also, the LONG and ULONG typedefs are 32 bit, NOT 64 !!
*/

#include <dos/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <devices/inputevent.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "kdp.h"

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

unsigned int mousex_old=-1234, mousey_old=-1234, mousex=0, mousey=0, buttons=0, dmousex=0, dmousey=0;

/* KdP data */
unsigned char vbuf[64000];
unsigned char buffer[64000*4];
unsigned char kdppalette[1024];

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *CyberGfxBase;
struct Screen *scr;
struct Window *win;
struct RastPort *rp;

ULONG cgfx_coltab[256];
ULONG loadrgb_coltab[256 * 3 + 2];

BYTE Keys[256];

BOOL wbscreen;

void  KDPwaitVr()
{
  //sync stuff goes here...
   WaitTOF();
}


/* handles events :)
   returns when there are no more events */
int handle_events()
{
    struct IntuiMessage *msg;
  
    while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
        switch(msg->Class)
	{
	    case IDCMP_MOUSEMOVE:	        
	        mousex_old = mousex;
		mousey_old = mousey;
		
		if (mousex_old == -1234) mousex_old = win->GZZMouseX;
		if (mousey_old == -1234) mousey_old = win->GZZMouseY;
		
		mousex = win->GZZMouseX;
		mousey = win->GZZMouseY;
		
		dmousex += mousex - mousex_old;
		dmousey += mousey - mousey_old;
		/* fall through */
	
	    case IDCMP_MOUSEBUTTONS:
	        buttons = 0;
		if (msg->Qualifier & IEQUALIFIER_LEFTBUTTON) buttons |= 1;
		if (msg->Qualifier & IEQUALIFIER_RBUTTON) buttons |= 2;
		if (msg->Qualifier & IEQUALIFIER_MIDBUTTON) buttons |= 4;
		break;
	
	    case IDCMP_CLOSEWINDOW:
	        buttons = 3;
		break;
			
	} /* switch(msg->Class) */
	
	ReplyMsg((struct Message *)msg);
	
    } /* while((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) */
 
    return 0;
}

//open screen and setup a greyscale palette
int KDPopen(KDPscreen *screen)
{
    int i, wx, wy;
    
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39)))
    {
        printf("Can't open intuition.library V39!");
	return 0;
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 40)))
    {
        printf("Can't open graphics.library V40!");
	return 0;
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library",0)))
    {
        printf("Can't open cybergraphics.library!");
	return 0;
    }

    if ((scr = LockPubScreen(NULL)))
    {    
	if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) >= 15)
	{
	    wx = scr->MouseX - (SCREENWIDTH + scr->WBorLeft + scr->WBorRight) / 2;
	    wy = scr->MouseY - (SCREENHEIGHT + scr->WBorTop + scr->WBorBottom + scr->Font->ta_YSize + 1) / 2;
            wbscreen = TRUE;
	} else {
	    UnlockPubScreen(NULL, scr);
	    scr = NULL;
	}
    }

    if (!wbscreen)
    {
        scr = OpenScreenTags(NULL, SA_Width	, SCREENWIDTH	,
				   SA_Height	, SCREENHEIGHT	,
				   SA_Depth	, 8		,
				   TAG_DONE);
	if (!scr)
	{
	    printf("Can't open screen!\n");
	    return 0;
	}
	loadrgb_coltab[0] = (256 << 16) + 0;
    }		  
    
    {
    	char s[10];
	
        if (GetVar("WINPOSX", s, sizeof(s), GVF_LOCAL_ONLY | LV_VAR) > 0)
	{
	    wx = strtol(s, 0, 0);
	}
        if (GetVar("WINPOSY", s, sizeof(s), GVF_LOCAL_ONLY | LV_VAR) > 0)
	{
	    wy = strtol(s, 0, 0);
	}

    }
    
    win = OpenWindowTags(NULL,
    		WA_CustomScreen				, (IPTR)scr		,
    		WA_Left					, wx			,
		WA_Top					, wy			, 
    		WA_InnerWidth				, SCREENWIDTH		,
    		WA_InnerHeight				, SCREENHEIGHT		,
		WA_AutoAdjust				, TRUE			,
		WA_IDCMP				, IDCMP_CLOSEWINDOW  |
			       	  			  IDCMP_MOUSEBUTTONS |
				   			  IDCMP_MOUSEMOVE	,
		WA_Activate				, TRUE			,
		WA_ReportMouse				, TRUE			,
		WA_RMBTrap				, TRUE			,
		wbscreen ? TAG_IGNORE : WA_Borderless	, TRUE			,
		wbscreen ? TAG_IGNORE : TAG_DONE	, 0			,
		WA_Title				, (IPTR)"Knp Window: Try pressing LMB or RMB"	,
		WA_DragBar				, TRUE			,
		WA_DepthGadget				, TRUE			,
		WA_CloseGadget				, TRUE			,
		TAG_DONE);
			       
    if (!win)
    {
	printf("Can't open window");
	return 0;
    }
    
    screen->vmem=vbuf;
    //create default greyscale palette
    for (i=0; i<256; i++)
      KDPsetColor(i, i, i, i);
      
    return 1;
}


//close all KDP stuff
void KDPclose(KDPscreen *screen)
{
    if (win) CloseWindow(win);
    
    if (scr)
    {
        if (wbscreen)
	    UnlockPubScreen(0, scr);
	else
	    CloseScreen(scr);
    }
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);

}

//set color (num) to (r,g,b)
void KDPsetColor(UBYTE num, UBYTE r, UBYTE g, UBYTE b)
{
  if (wbscreen)
  {
    cgfx_coltab[num] = (((ULONG)r) << 16) + (((ULONG)g) << 8) + ((ULONG)b);
  } else {
    SetRGB32(&scr->ViewPort, num,
    	     ((ULONG) r) * 0x01010101,
	     ((ULONG) g) * 0x01010101,
	     ((ULONG) b) * 0x01010101);
  }
}

//set all color registers to (pal)
//(pal is a 256*3 byte array)
//(r,g,b,r,g,b,r,g,b,......)
void KDPsetPal(UBYTE *pal)
{
  unsigned int i, p=0;
  ULONG r,g,b;
  
  if (wbscreen)
  {
    for (i=0;i<256;i++)
      {
	r = *pal++;
	g = *pal++;
	b = *pal++;

	cgfx_coltab[i] = (r << 16) + (g << 8) + b;
      }
  } else {
    for (i=0;i<256;i++)
      {
	r = *pal++;
	g = *pal++;
	b = *pal++;

	loadrgb_coltab[1 + i * 3] = r * 0x01010101;
	loadrgb_coltab[2 + i * 3] = g * 0x01010101;
	loadrgb_coltab[3 + i * 3] = b * 0x01010101;
      }
    LoadRGB32(&scr->ViewPort, loadrgb_coltab);
  }
}

//display (vmem)
void KDPshow(UBYTE *vmem)
{
    handle_events( );

    if (wbscreen)
        WriteLUTPixelArray(vmem,
			   0,
			   0,
			   SCREENWIDTH,
			   win->RPort,
			   cgfx_coltab,
			   win->BorderLeft,
			   win->BorderTop,
			   SCREENWIDTH,
			   SCREENHEIGHT,
			   CTABFMT_XRGB8);
   else
        WriteChunkyPixels(win->RPort,
			  0,
			  0,
			  SCREENWIDTH - 1,
			  SCREENHEIGHT - 1,
			  vmem,
			  SCREENWIDTH);
}

void KDPgetMouse(KDPmouse *mouse)
{
   handle_events();
   mouse->xspeed=dmousex;
   mouse->yspeed=dmousey;
   mouse->button=buttons;
   dmousex=0;
   dmousey=0;
}

//load a file into memory,
//the pointer returned should be "free"'d
void *KDPloadFile(char *filename)
{
  FILE *fp;
  int len;
  void *mem=0;
  if (fp=fopen(filename,"rb"))
    {
    fseek(fp,0,SEEK_END);
    len=ftell(fp);
    fseek(fp,0,SEEK_SET);
    mem=(void *)malloc(len);
    fread(mem,len,1,fp);
    fclose(fp);
    }
  return mem;
}

//these functions read words/longs/float from memory in Motorola or Intel byteorder
WORD KDPgetMword(UBYTE *wrd)
{
  return (wrd[0]<<8)+wrd[1];
}

LONG KDPgetMlong(UBYTE *lng)
{
  return (lng[0]<<24)+(lng[1]<<16)+(lng[2]<<8)+lng[3];
}

WORD KDPgetIword(UBYTE *wrd)
{
  return (wrd[1]<<8)+wrd[0];
}

LONG KDPgetIlong(UBYTE *lng)
{
  return (lng[3]<<24)+(lng[2]<<16)+(lng[1]<<8)+lng[0];
}

float KDPgetMfloat(UBYTE *b)
  {
  LONG l;
  float f;
  l=KDPgetMlong(b);
  memcpy(&f,&l,4);
  return f;
  }
  
float KDPgetIfloat(UBYTE *b)
  {
  LONG l;
  float f;
  l=KDPgetIlong(b);
  memcpy(&f,&l,4);
  return f;
  }

//write Motorola/Intel byteordered words/longs/floats to a file
int KDPwriteMword(WORD word,FILE *file)
  {
  UBYTE *wrd=(UBYTE *)&word;
  if(fputc(wrd[0],file)==EOF) return EOF;
  if(fputc(wrd[1],file)==EOF) return EOF;
  return !EOF;
  }

int KDPwriteMlong(LONG llng,FILE *file)
  {
  UBYTE *lng=(UBYTE *)&llng;
  if(fputc(lng[0],file)==EOF) return EOF;
  if(fputc(lng[1],file)==EOF) return EOF;
  if(fputc(lng[2],file)==EOF) return EOF;
  if(fputc(lng[3],file)==EOF) return EOF;
  return !EOF;
  }

int KDPwriteIword(WORD word,FILE *file)
  {
  UBYTE *wrd=(UBYTE *)&word;
  if(fputc(wrd[1],file)==EOF) return EOF;
  if(fputc(wrd[0],file)==EOF) return EOF;
  return !EOF;
  }

int KDPwriteIlong(LONG llng,FILE *file)
  {
  UBYTE *lng=(UBYTE *)&llng; 
  if(fputc(lng[3],file)==EOF) return EOF;
  if(fputc(lng[2],file)==EOF) return EOF;
  if(fputc(lng[1],file)==EOF) return EOF;
  if(fputc(lng[0],file)==EOF) return EOF;
  return !EOF;
  }



