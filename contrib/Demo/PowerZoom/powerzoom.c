#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCREENWIDTH  imagewidth
#define SCREENHEIGHT imageheight
#define SCREENCY (SCREENHEIGHT / 2)

/***********************************************************************************/

struct IntuitionBase 	*IntuitionBase;
struct GfxBase      	*GfxBase;
struct Library      	*CyberGfxBase;
struct Library      	*DataTypesBase;
struct Screen 	    	*scr;
struct Window 	    	*win;
struct RastPort     	*rp;
Object	    	    	*dto;
UBYTE	    	    	*imagebuffer, *imagebuffer2;
LONG	    	    	 imagewidth, imageheight, zoomwidth, zoomheight;
BOOL	    	    	 maxspeed, hiquality = TRUE;
UBYTE 	    	    	 filename[1024];
ULONG 	    	    	 cgfx_coltab[256];
WORD 	    	    	 winx = -1, winy = -1;

/***********************************************************************************/

void cleanup(char *msg)
{
    if (msg)
    {
        printf("powerzoom: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
    if (imagebuffer) FreeVec(imagebuffer);
    if (dto) DisposeDTObject(dto);
    
    if (scr) UnlockPubScreen(0, scr);
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
}

/***********************************************************************************/

#define ARG_TEMPLATE "IMAGEFILE,WINPOSX=X/N/K,WINPOSY=Y/N/K,MAXSPEED/S"
#define ARG_FILE 0
#define ARG_X 1
#define ARG_Y 2
#define ARG_MAXSPEED 3
#define NUM_ARGS 4

static IPTR args[NUM_ARGS];

static void getarguments(void)
{
    struct RDArgs *myargs;
    
    if ((myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
    {
    	if (args[ARG_FILE])
	{
    	    strncpy(filename, (char *)args[ARG_FILE], sizeof(filename));
	}
	else
	{
	    strncpy(filename, "SYS:Images/AROS.png", sizeof(filename));
	}
	if (args[ARG_X]) winx = *(IPTR *)args[ARG_X];
	if (args[ARG_Y]) winy = *(IPTR *)args[ARG_Y];
	if (args[ARG_MAXSPEED]) maxspeed = TRUE;
	
    	FreeArgs(myargs);
    }
    else
    {
    	PrintFault(IoErr(), "PowerZoom");
	cleanup(0);
    }
    
}

/***********************************************************************************/

void openlibs(void)
{
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39)))
    {
        cleanup("Can't open intuition.library V39!");
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39)))
    {
        cleanup("Can't open graphics.library V39!");
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library",0)))
    {
        cleanup("Can't open cybergraphics.library!");
    }

    if (!(DataTypesBase = OpenLibrary("datatypes.library",0)))
    {
        cleanup("Can't open datatypes.library!");
    }
}

/***********************************************************************************/

void getvisual(void)
{
    scr = LockPubScreen(NULL);
    if (!scr) cleanup("Can't lock public screen!");
    
    if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) < 15)
    {
    	cleanup("Need hicolor/truecolor screen!");
    }
}

/***********************************************************************************/

void openpic(void)
{
    dto = NewDTObject(filename,
    	    	      DTA_GroupID, GID_PICTURE,
		      PDTA_Screen, (IPTR)scr,
		      PDTA_Remap, TRUE,
		      PDTA_DestMode, PMODE_V43,
		      PDTA_UseFriendBitMap, TRUE,
		      TAG_DONE);
   
    if (!dto) cleanup("Can't open picture!");
}

/***********************************************************************************/

void readpic(void)
{
    struct BitMapHeader *bmhd = NULL;
    struct BitMap *dtobm = NULL;
    struct RastPort temprp;
    
    DoMethod(dto, DTM_PROCLAYOUT, NULL, 1);
    
    GetDTAttrs(dto, PDTA_BitMapHeader, (IPTR)&bmhd, PDTA_DestBitMap, (IPTR)&dtobm, TAG_DONE);
    if (!bmhd || !dtobm)
    {
    	cleanup("Error reading datatype image!");
    }
 
    imagewidth = bmhd->bmh_Width; imageheight = bmhd->bmh_Height;

    imagebuffer = AllocVec(imagewidth * imageheight * 4 * 2, MEMF_ANY);
    if (!imagebuffer)
    {
    	cleanup("Error allocating image buffer!");
    }
    imagebuffer2 = imagebuffer + imagewidth * imageheight * 4;
    
    InitRastPort(&temprp);
    temprp.BitMap = dtobm;
    ReadPixelArray(imagebuffer, 0, 0, imagewidth * 4,
    	    	   &temprp, 0, 0, imagewidth, imageheight, RECTFMT_ARGB);
    DeinitRastPort(&temprp);
    
    DisposeDTObject(dto); dto = NULL;
}

/***********************************************************************************/

void makewin(void)
{
    if (winx == -1) winx = (scr->Width - SCREENWIDTH - scr->WBorLeft - scr->WBorRight) / 2;
    if (winy == -1) winy = (scr->Height - SCREENHEIGHT - scr->WBorTop - scr->WBorTop - scr->Font->ta_YSize - 1) / 2;
    
    win = OpenWindowTags(NULL, WA_CustomScreen	, (IPTR)scr,
    			       WA_Left		, winx,
			       WA_Top		, winy,
    			       WA_InnerWidth	, SCREENWIDTH,
    			       WA_InnerHeight	, SCREENHEIGHT,
			       WA_AutoAdjust	, TRUE,
	    	    	       WA_Activate	, TRUE,
			       WA_IDCMP		, IDCMP_CLOSEWINDOW |
			       			  IDCMP_RAWKEY | IDCMP_VANILLAKEY,			       
    			       WA_DragBar	, TRUE,
			       WA_DepthGadget	, TRUE,
			       WA_CloseGadget	, TRUE,
			       WA_Title	    	, (IPTR)"PowerZoom: SPACE = switch quality  S = switch speed limit",
			       TAG_DONE);
			       
   if (!win) cleanup("Can't open window");

   rp = win->RPort; 

}

/***********************************************************************************/

void uglyzoom(void)
{
    LONG x, y, gx, gy, stepx, stepy;
    ULONG *src;
    ULONG *dest = (ULONG *)imagebuffer2;
    
    stepx = imagewidth  * 0x10000 / zoomwidth;
    stepy = imageheight * 0x10000 / zoomheight;
    
    for(y = 0, gy = 0; y < zoomheight; y++, gy += stepy)
    {
    	src = ((ULONG *)imagebuffer) + (gy / 0x10000) * imagewidth;
	
    	for(x = 0, gx = 0; x < zoomwidth; x++, gx += stepx)
	{
	    *dest++ = src[(gx / 0x10000)];
	}
    }
}

/***********************************************************************************/

void nicezoom(void)
{
    LONG x, y, gx, gy, stepx, stepy, sx, sy, nx, ny, sample_w, sample_h, sample_size;
    LONG pix_w, pix_h, pix_size;
    int  iter = 0;
    
    ULONG *src;
    ULONG *dest = (ULONG *)imagebuffer2;
    
    stepx = imagewidth  * 0x10000 / zoomwidth;
    stepy = imageheight * 0x10000 / zoomheight;

    for(y = 0, gy = 0; y < zoomheight; y++, gy += stepy)
    {
    	src = ((ULONG *)imagebuffer) + (gy / 0x10000) * imagewidth;

#   	define PRINT(d) // if (y == 1 && x == 2) d
	
    	for(x = 0, gx = 0; x < zoomwidth; x++, gx += stepx)
	{
	    ULONG r = 0, g = 0, b = 0;
	    LONG gx2 = gx + stepx;
	    LONG gy2 = gy + stepy;
	    
	    sample_w = gx2 - gx;
	    sample_h = gy2 - gy;
	    sample_size = (sample_w >> 8) * (sample_h >> 8);
    	    sample_size += (sample_size >> 8);

    	    if (!sample_size)
	    {
	    	dest++;
		continue;
	    }
	    	    
	    for(sy = gy; sy < gy2; sy = ny)
	    {
		ny = (sy + 0x10000) & 0xFFFF0000;
		if (ny > gy2) ny = gy2;

		for(sx = gx; sx < gx2; sx = nx)
		{
		    ULONG src, pix_r, pix_g, pix_b;
		    
		    nx = (sx + 0x10000) & 0xFFFF0000;
		    if (nx > gx2) nx = gx2;
		    
		    pix_w = nx - sx;
		    pix_h = ny - sy;
		    pix_size = ((pix_w >> 8) * (pix_h >> 8));
    	    	    //pix_size += (pix_size >> 8);
		    
    	    	    PRINT(kprintf("pix_size[%x,%x] %x\n",sx,sy,pix_size));

		    src = ((ULONG *)imagebuffer)[(sy / 0x10000) * imagewidth + sx / 0x10000];
    	
		#if AROS_BIG_ENDIAN
		    pix_r = (src >> 16) & 0xFF;
		    pix_g = (src >> 8) & 0xFF;
		    pix_b = (src) & 0xFF;
		#else
		    pix_r = (src >> 8) & 0xFF;
		    pix_g = (src >> 16) & 0xFF;
		    pix_b = (src >> 24) & 0xFF;
		#endif
		
		    r += pix_r * pix_size;
		    g += pix_g * pix_size;
		    b += pix_b * pix_size;
		    
		    iter++;
		}
		
    	    }
	    
	    r /= sample_size;
	    g /= sample_size;
	    b /= sample_size;
	    
    	#if AROS_BIG_ENDIAN
	    *dest++ = (r << 16) | (g << 8) | b;
	#else	    
	    *dest++ = (r << 8) | (g << 16) | (b << 24);
	#endif
	}
    }

    //kprintf("iterations: %d\n", iter);
}

/***********************************************************************************/

void action(void)
{
    static int direction = -1;

    if (zoomwidth && zoomheight)
    {
	if (hiquality)
	{
    	    nicezoom();
	}
	else
	{
    	    uglyzoom();
	}

	WritePixelArray(imagebuffer2,
    	    		0,
			0,
			zoomwidth * 4,
    	    		rp,
			win->BorderLeft,
			win->BorderTop,
			(zoomwidth < win->GZZWidth ? zoomwidth : win->GZZWidth),
			(zoomheight < win->GZZHeight ? zoomheight : win->GZZHeight),
			RECTFMT_ARGB);
    }

#if 1    
    zoomwidth += direction;
    if ((zoomwidth == imagewidth) || (zoomwidth < 3)) direction = -direction;
    zoomheight = zoomwidth * imageheight / imagewidth;
#else
    zoomwidth = imagewidth * 2 / 3;
    zoomheight = imageheight;
#endif
    
    
}

/***********************************************************************************/

void handleall(void)
{
    struct IntuiMessage *msg;
    BOOL    	    	 quitme = FALSE;
    
    zoomwidth = imagewidth;
    zoomheight = imageheight;
    
    while(!quitme)
    {
    	if (!maxspeed) WaitTOF();
	action();
	while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
	{
            switch(msg->Class)
	    {
		case IDCMP_CLOSEWINDOW:
	            quitme = TRUE;
		    break;

		case IDCMP_VANILLAKEY:
	    	    switch(msg->Code)
		    {
			case 27:
		    	    quitme = TRUE;
			    break;
			    
			case ' ':
			    hiquality = !hiquality;
			    break;
			    
			case 'S':
			case 's':
			    maxspeed = !maxspeed;
			    break;
		    }
		    break;

	    }
            ReplyMsg((struct Message *)msg);
	}
    }

}

/***********************************************************************************/

main(int argc, char *argv[])
{
    getarguments();
    openlibs();
    getvisual();
    openpic();
    readpic();
    makewin();
    handleall();
    cleanup(0);
}

/***********************************************************************************/

