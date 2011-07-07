#include <intuition/intuition.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/alib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kittysprite_image.c"

static struct Screen 	*scr;
static struct Window 	*win;
static struct Region	*shape;
static Object	     	*draggad;
static BOOL 	    	 remapped, truecolor, shape_active = TRUE;
static UBYTE 	    	 chunky[KITTYSPRITE_WIDTH * KITTYSPRITE_HEIGHT];
static UBYTE 	    	 remaptable[KITTYSPRITE_COLORS];
static ULONG 	    	 cgfx_coltab[KITTYSPRITE_COLORS];

static void cleanup(char *msg)
{
    if (msg)
    {
    	printf("Kitty: %s\n", msg);
    }
    
    if (remapped)
    {
    	WORD i;
	
	for(i = 0; i < KITTYSPRITE_COLORS; i++)
	{
	    ReleasePen(scr->ViewPort.ColorMap, remaptable[i]);
	}
    }
    
    if (scr) UnlockPubScreen(NULL, scr);    
    if (draggad) DisposeObject(draggad);
    if (win) CloseWindow(win);
    
    exit(0);
}

static void getvisual(void)
{
    if (!(scr = LockPubScreen(NULL)))
    {
        cleanup("Can't lock pub screen!");
    }

    if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) > 8)
    {
    	truecolor = TRUE;
    }
}

#if KITTYSPRITE_PACKED
static UBYTE *unpack_byterun1(UBYTE *source, UBYTE *dest, LONG unpackedsize)
{
    UBYTE r;
    BYTE c;
    
    for(;;)
    {
	c = (BYTE)(*source++);
	if (c >= 0)
	{
    	    while(c-- >= 0)
	    {
		*dest++ = *source++;
		if (--unpackedsize <= 0) return source;
	    }
	}
	else if (c != -128)
	{
    	    c = -c;
	    r = *source++;

	    while(c-- >= 0)
	    {
		*dest++ = r;
		if (--unpackedsize <= 0) return source;
	    }
	}
    }
    
}
#endif

static void makegfx(void)
{
    LONG i;
        
#if KITTYSPRITE_PACKED
    unpack_byterun1(kittysprite_data, chunky, KITTYSPRITE_WIDTH * KITTYSPRITE_HEIGHT);
#else
    memcpy(chunky, kittysprite_data, KITTYSPRITE_WIDTH * KITTYSPRITE_HEIGHT);
#endif

    shape = NewRegion();
    if (shape)
    {
    	WORD x, y;
	
	for(y = 0; y < KITTYSPRITE_HEIGHT; y++)
	{
	    struct Rectangle rect = {0, y, 0, y};
	    BOOL    	     transp, transpstate = TRUE;
	    
	    for(x = 0; x < KITTYSPRITE_WIDTH; x++)
	    {
	    	transp = chunky[y * KITTYSPRITE_WIDTH + x] == 0;
		
    	    	if (transpstate)
		{
		    if (!transp)
		    {
		    	rect.MaxX = rect.MinX = x;
			transpstate = FALSE;
		    }
		}
		else
		{
		    if (transp)
		    {
		    	OrRectRegion(shape, &rect);
		    	transpstate = TRUE;
		    }
		    else
		    {
		    	rect.MaxX++;
		    }
		}
	    }
	    
	    if (!transpstate) OrRectRegion(shape, &rect);
	}
    }
    
    for(i = 0; i < KITTYSPRITE_COLORS; i++)
    {
    	if (truecolor)
	{
	    cgfx_coltab[i] = kittysprite_pal[i];
	}
	else
	{
	    ULONG r = (kittysprite_pal[i] >> 16) & 0xFF;
	    ULONG g = (kittysprite_pal[i] >> 8) & 0xFF;
	    ULONG b = (kittysprite_pal[i]) & 0xFF;
	    
	    remaptable[i] = ObtainBestPen(scr->ViewPort.ColorMap,
      	    	    	    	    	  r * 0x01010101,
				    	  g * 0x01010101,
				          b * 0x01010101,
					  OBP_Precision, PRECISION_IMAGE,
					  OBP_FailIfBad, FALSE,
					  TAG_DONE);
	}
	
    }
    
    if (!truecolor)
    {
    	remapped = TRUE;
	
	for(i = 0; i < KITTYSPRITE_WIDTH * KITTYSPRITE_HEIGHT; i++)
	{
	    chunky[i] = remaptable[chunky[i]];
	}
    }

}

static void makedraggad(void)
{
    draggad = NewObject(NULL, GADGETCLASS, GA_Left, 0,
    	    	    	    	    	   GA_Top, 0,
					   GA_RelWidth, 0,
					   GA_RelHeight, 0,
					   GA_SysGType, GTYP_WDRAGGING,
					   TAG_DONE);
					   
					   
}

static void makewin(void)
{
    win = OpenWindowTags(NULL, WA_CustomScreen, (IPTR)scr,
    	    	    	       WA_Left, 0,
			       WA_Top, 0,
			       WA_Width, KITTYSPRITE_WIDTH,
			       WA_Height, KITTYSPRITE_HEIGHT,
			       WA_Borderless, TRUE,
			       WA_Activate, TRUE,
			       WA_IDCMP, IDCMP_VANILLAKEY,
			       WA_Gadgets, (IPTR)draggad,
			       WA_ShapeRegion, (IPTR)shape,
			       TAG_DONE);

    if (!win) cleanup("Can't open window!");
}

static void paintwin(void)
{
    if (truecolor)
    {
    	WriteLUTPixelArray(chunky,
	    	    	   0,
			   0,
			   KITTYSPRITE_WIDTH,
			   win->RPort,
			   cgfx_coltab,
			   0,
			   0,
			   KITTYSPRITE_WIDTH,
			   KITTYSPRITE_HEIGHT,
			   CTABFMT_XRGB8);
    }
    else
    {
    	WriteChunkyPixels(win->RPort,
	    	    	  0,
			  0,
			  KITTYSPRITE_WIDTH - 1,
			  KITTYSPRITE_HEIGHT - 1,
			  chunky,
			  KITTYSPRITE_WIDTH);
    }
}

static void handleall(void)
{
    struct IntuiMessage *msg;
    BOOL    	    	 quitme = FALSE;
    
    while(!quitme)
    {
    	WaitPort(win->UserPort);
	while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
	{
	    switch(msg->Class)
	    {
	    	case IDCMP_VANILLAKEY:
		    switch(msg->Code)
		    {
		    	case 'Q':
			case 'q':
			case 'X':
			case 'x':
		    	case 27:
		    	    quitme = TRUE;
		    	    break;
			    
			case 'F':
			case 'f':
			case 13:
			    WindowToFront(win);
			    break;
			    
			case 'B':
			case 'b':
			case 8:
			    WindowToBack(win);
			    break;
			  
			case 'S':
			case 's':
			case 'M':
			case 'm':  
			case 32:
			    ChangeWindowShape(win, (shape_active ? NULL: shape), NULL);
			    shape_active = !shape_active;
			    break;
		    } /* switch(msg->Code) */
		    break;
		    
	    } /* switch(msg->Class) */
	    
	    ReplyMsg((struct Message *)msg);
	    
	} /* while((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) */
	
    } /* while(!quitme) */
}

int main(void)
{
    getvisual();
    makegfx();
    makedraggad();
    makewin();
    paintwin();
    handleall();
    cleanup(0);  

    return 0;
}
