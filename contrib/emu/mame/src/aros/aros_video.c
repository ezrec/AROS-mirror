#include "driver.h"

#include <exec/exec.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>

#include "aros_video.h"
#include "aros_input.h"
#include "aros_main.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/************************************************************************************/

int frameskip,autoframeskip;
int scanlines, use_tweaked, video_sync, wait_vsync, use_triplebuf;
int stretch, use_mmx, use_dirty;
int vgafreq, always_synced, skiplines, skipcolumns;
float osd_gamma_correction;
int gfx_mode, gfx_width, gfx_height;
int center_x, center_y;

int brightness = 100;

struct osd_bitmap *scrbitmap;
struct Window *win;
struct Screen *scr;

/************************************************************************************/

static UBYTE Palette[256][3];
static ULONG cgfxcoltab[256];

static int FrameCounter;
static const int NoFrameSkipCount = 10;

static LONG video_left, video_top, video_width, video_height;

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

struct osd_bitmap *osd_new_bitmap(int width, int height, int depth)
{
  struct osd_bitmap *bitmap;
  unsigned char   *line;
  LONG        safety;
  LONG        i, w;

  if(Machine->orientation & ORIENTATION_SWAP_XY)
  {
    w   = width;
    width = height;
    height  = w;
  }

  if(width > 32)
    safety = 8;
  else
    safety = 0;

  if(depth != 16)
  {
    depth = 8;
    w = ((width + 2 * safety + 3) >> 2) << 2;
  }
  else
    w = ((2 * (width + 2 * safety) + 3) >> 2) << 2;

  bitmap = (struct osd_bitmap *) calloc(  sizeof(struct osd_bitmap)
                      + height*sizeof(unsigned char *)
                      + w*(height+2*safety)*sizeof(unsigned char),1);

  if(bitmap)
  {
    UBYTE fillc = 0;
    
    bitmap->width    = width;
    bitmap->height   = height;
    bitmap->depth    = depth;
    bitmap->_private = (void *) w;
    bitmap->line     = (unsigned char **) &bitmap[1]; 

    line = ((unsigned char *) &bitmap->line[height]) + safety * w;

    for(i = 0; i < height; i++)
    {
      int x;
      
      bitmap->line[i] = line;
      line += w*sizeof(unsigned char);
    }
  }
	
  return(bitmap);
}

/************************************************************************************/

void osd_free_bitmap(struct osd_bitmap *bitmap)
{
  if(bitmap)
    free(bitmap);
}

/************************************************************************************/

void osd_clearbitmap(struct osd_bitmap *bitmap)
{
  int i;

  for (i = 0;i < bitmap->height;i++)
    memset(bitmap->line[i],0,bitmap->width);

#if 0
  if(bitmap == scrbitmap)
  {
    if(DirtyLines[0])
      memset(DirtyLines[0], 1, scrbitmap->height);
  }
#endif

}

/************************************************************************************/

void osd_set_gamma(float gamma)
{
}

/************************************************************************************/

float osd_get_gamma(void)
{
  return(1.0);
}

/************************************************************************************/

void osd_set_brightness(int brightness)
{
}

/************************************************************************************/

int osd_get_brightness(void)
{
  return(100);
}

/************************************************************************************/

void osd_modify_pen(int pen, unsigned char red, unsigned char green, unsigned char blue)
{
  cgfxcoltab[pen] = (((ULONG)red) << 16) +
  		    (((ULONG)green) << 8) +
		    (((ULONG)blue));

  Palette[pen][0] = red;
  Palette[pen][1] = green;
  Palette[pen][2] = blue;


}

/************************************************************************************/

void osd_get_pen(int pen, unsigned char *r, unsigned char *g, unsigned char *b)
{
  *r  = Palette[pen][0];
  *g  = Palette[pen][1];
  *b  = Palette[pen][2];
}

/************************************************************************************/

static inline short makecol(int r, int g, int b)
{
/*  switch(PixelArray[0]->PixelFormat)
  {

    case PIXFMT_RGB15:
      return((r&0xf8)<<7)|((g&0xf8)<<2)|(b>>3);
      break;

    case PIXFMT_BGR15:
      return((b&0xf8)<<7)|((g&0xf8)<<2)|(r>>3);
      break;

    case PIXFMT_RGB15PC:
      return((r&0xf8)>>1)|(g>>6)|((g&0x38)<<10)|((b&0xf8)<<5);
      break;

    case PIXFMT_BGR15PC:
      return((b&0xf8)>>1)|(g>>6)|((g&0x38)<<10)|((r&0xf8)<<5);
      break;

    case PIXFMT_BGR16:
      return((b&0xf8)<<8)|((g&0xfc)<<3)|(r>>3);
      break;

    case PIXFMT_RGB16PC:
      return(r&0xf8)|(g>>5)|((g&0x1c)<<11)|((b&0xf8)<<5);
      break;

    case PIXFMT_BGR16PC:
      return(b&0xf8)|(g>>5)|((g&0x1c)<<11)|((r&0xf8)<<5);
      break;

    case PIXFMT_RGB16:
    
    default:
      return((r&0xf8)<<8)|((g&0xfc)<<3)|(b>>3);
      break;
  }
  */

  return(r&0xf8)|(g>>5)|((g&0x1c)<<11)|((b&0xf8)<<5);

}

/************************************************************************************/

int osd_allocate_colors(unsigned int totalcolors,const unsigned char *palette,unsigned short *pens,int modifiable)
{
    int i;
    int startcolor = 0;
    
    if(scrbitmap->depth != 8)
    {
	int r,g,b;

	for (i = 0;i < totalcolors;i++)
	{
	    r = 255 * brightness * pow(palette[3*i+0] / 255.0, 1 / osd_gamma_correction) / 100;
	    g = 255 * brightness * pow(palette[3*i+1] / 255.0, 1 / osd_gamma_correction) / 100;
	    b = 255 * brightness * pow(palette[3*i+2] / 255.0, 1 / osd_gamma_correction) / 100;
	    *pens++ = makecol(r,g,b);	
	}

	Machine->uifont->colortable[0] = makecol(0x00,0x00,0x00);
	Machine->uifont->colortable[1] = makecol(0xff,0xff,0xff);
	Machine->uifont->colortable[2] = makecol(0xff,0xff,0xff);
	Machine->uifont->colortable[3] = makecol(0x00,0x00,0x00);
    }
    else
    {
        if (totalcolors >= 255)
	{
	    int best_black;
	    int best_white;
	    int best_black_score;
	    int best_white_score;

	    best_black       = 0;
	    best_white       = 0;
	    best_black_score = 3*255*255;
	    best_white_score = 0;

	    for(i = 0; i < totalcolors; i++)
	    {
		int r,g,b,score;

		pens[i] = i;

		r = palette[3*i];
		g = palette[3*i+1];
		b = palette[3*i+2];

		score = r*r + g*g + b*b;

		if(score < best_black_score)
		{
                    best_black       = i;
                    best_black_score = score;
		}

		if(score > best_white_score)
		{
                    best_white       = i;
                    best_white_score = score;
		}
	    }

	    Machine->uifont->colortable[0] = pens[best_black];
	    Machine->uifont->colortable[1] = pens[best_white];
	    Machine->uifont->colortable[2] = pens[best_white];
	    Machine->uifont->colortable[3] = pens[best_black];

	} else {
	     /* reserve color 1 for the user interface text */
	     cgfxcoltab[1] = 0xFFFFFF;
	     
	     Machine->uifont->colortable[0] = 0;
	     Machine->uifont->colortable[1] = 1;
	     Machine->uifont->colortable[2] = 1;
	     Machine->uifont->colortable[3] = 0;

	     /* fill the palette starting from the end, so we mess up badly written */
	     /* drivers which don't go through Machine->pens[] */
	     for (i = 0;i < totalcolors;i++)
		     pens[i] = (256-1)-i;
	}

        for(i = 0; i < totalcolors; i++)
        {
 	    ULONG r = palette[3*i];
	    ULONG g = palette[3*i+1];
	    ULONG b = palette[3*i+2];

	    Palette[pens[i]][0] = r;
	    Palette[pens[i]][1] = g;
	    Palette[pens[i]][2] = b;

	    cgfxcoltab[pens[i]] = (r << 16) + (g << 8) + (b);
	}
	
    }
    
    return 0;
}


/************************************************************************************/

void osd_led_w(int led,int on)
{
}

/************************************************************************************/

int osd_skip_this_frame(void)
{
    if(FrameCounter >= NoFrameSkipCount)
    {
      if(FrameCounter < (NoFrameSkipCount + frameskip))
	return(1);
    }

    return(0);
}

/************************************************************************************/

struct osd_bitmap *osd_create_display(int width, int height,int depth, int attributes)
{
    unsigned char *line;

    LONG left, top, right, bottom;
    LONG i, t;

    printf("*** AROS: osd_createdisplay %i x %i x %i. attr = %i ****\n", width, height, depth, attributes);

    if(Machine->orientation & ORIENTATION_SWAP_XY)
    {
	t      = width;
	width  = height;
	height = t;

	left   = Machine->drv->visible_area.min_y;
	top    = Machine->drv->visible_area.min_x;
	right  = Machine->drv->visible_area.max_y;
	bottom = Machine->drv->visible_area.max_x;
    }
    else
    {
	left   = Machine->drv->visible_area.min_x;
	top    = Machine->drv->visible_area.min_y;
	right  = Machine->drv->visible_area.max_x;
	bottom = Machine->drv->visible_area.max_y;
    }

    if(attributes & VIDEO_TYPE_VECTOR)
    {
    /*
	if(Config[CFG_WIDTH] > width)
	  width = Config[CFG_WIDTH];

	if(Config[CFG_HEIGHT] > height)
	  height = Config[CFG_HEIGHT];
    */      
	left   = 0;
	top    = 0;
	right  = width - 1;
	bottom = height - 1;
    }
    else
    {
	if(Machine->orientation & ORIENTATION_FLIP_X)
	{
	    t     = width - left - 1;
	    left  = width - right - 1;
	    right = t;
	}

	if(Machine->orientation & ORIENTATION_FLIP_Y)
	{
	    t      = height - top - 1;
	    top    = height - bottom - 1;
	    bottom = t;
	}
    }

    video_left = left;
    video_top = top;
    video_width = right - left + 1;
    video_height = bottom - top + 1;

    Machine->uiwidth  = right - left + 1;
    Machine->uiheight = bottom - top + 1;
    Machine->uixmin   = left;
    Machine->uiymin   = top;

    if ((scr = LockPubScreen(NULL)))
    {
	WORD winx = (scr->Width - video_width - scr->WBorLeft - scr->WBorRight) / 2;
	WORD winy = (scr->Height - video_height - scr->WBorTop - scr->WBorBottom - scr->Font->ta_YSize - 1) / 2;

	win = OpenWindowTags(0, WA_PubScreen, (IPTR)scr,
      				WA_Title, (IPTR)"AROS Mame",
  				WA_Left, winx,
				WA_Top, winy,
				WA_InnerWidth, video_width,
				WA_InnerHeight, video_height,
				WA_CloseGadget, TRUE,
				WA_DragBar, TRUE,
				WA_DepthGadget, TRUE,
				WA_Activate, TRUE,
				WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
				TAG_DONE);

	if (win)
	{
	    WORD w = width, h = height;

	    if(Machine->orientation & ORIENTATION_SWAP_XY)
	    {
        	w ^= h;
		h ^= w;
		w ^= h;
	    }

	    scrbitmap = osd_new_bitmap(w, h, depth);

	} /* if (win) */

    } /* if ((scr = LockPubScreen(NULL))) */

    if (!scrbitmap)
    {
	if (win) CloseWindow(win);win = NULL;
	if (scr) UnlockPubScreen(NULL, scr);scr = NULL;
    }

    return scrbitmap;
}

/************************************************************************************/

void osd_close_display(void)
{
    LONG i;

    #if 0
    if(DirtyLines[0])
    {
	for(i = 1; i < (2 * (Config[CFG_BUFFERING] + 1)); i++)
	{
	    if(DirtyLines[0] > DirtyLines[i])
	      DirtyLines[0] = DirtyLines[i];
	}

	free(DirtyLines[0]);
    }
    #endif

    if(scrbitmap) free(scrbitmap);

    if (win) CloseWindow(win);win = NULL;
    if (scr) UnlockPubScreen(NULL, scr);scr = NULL;

    win = NULL;  

}

/************************************************************************************/

void osd_mark_dirty(int xmin, int ymin, int xmax, int ymax, int ui)
{
}

/************************************************************************************/

void osd_update_video_and_audio(void)
{
    WriteLUTPixelArray(scrbitmap->line[0],
		       video_left,
		       video_top,
		       scrbitmap->_private,
		       win->RPort,
		       cgfxcoltab,
		       win->BorderLeft,
		       win->BorderTop,
		       video_width,
		       video_height,
		       CTABFMT_XRGB8);

    AROS_Input_Update();
}

/************************************************************************************/
