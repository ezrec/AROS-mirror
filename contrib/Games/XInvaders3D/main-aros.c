/*------------------------------------------------------------------
  main-x11.c:

    XINVADERS 3D - 3d Shoot'em up
    Copyright (C) 2000 Don Llopis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

------------------------------------------------------------------*/

#include "game.h"

#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <devices/inputevent.h>
#include <proto/arossupport.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <string.h>

/*================================================================*/

/* Cohan-Sutherland clipping algorithm */

#define CLIPLEFT  1
#define CLIPRIGHT 2
#define CLIPLOWER 4
#define CLIPUPPER 8

int clipline(int *px1, int *py1, int *px2, int *py2);

/*================================================================*/

struct GfxBase 		*GfxBase;
struct IntuitionBase 	*IntuitionBase;
struct Screen 		*scr;
struct Window 		*win;
struct BitMap		*draw_bm;
struct RastPort 	*win_rp, *draw_rp;

ULONG 			coltable[256 * 3 + 2];

int 			window_width, window_height;
int                     XMax, YMax, XMin, YMin;

/*------------------------------------------------------------------
 * main
 *  
 *
------------------------------------------------------------------*/
int main ( int argc, char **argv )
{
   int i;

   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39);
   
   if (!IntuitionBase || !GfxBase)
   {
      fprintf ( stderr, "Error: could not open libraries!\n" );
      goto error;
   }
   
   if ( !Graphics_init ( WIN_WIDTH, WIN_HEIGHT ) )
   {
      fprintf ( stderr, "Error: could not initialize graphics!\n" );
      goto error;
   }

   if ( !Game_init ( WIN_WIDTH, WIN_HEIGHT ) )
   {
      fprintf ( stderr, "Error: could not initialize game data!\n" );
      goto error;
   }

   Game_main ();

error:
   Graphics_shutdown ();
   
   if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
   if (GfxBase) CloseLibrary((struct Library *)GfxBase);
   
   /* print contact information */
   i = 0;
   while ( game_about_info[i] )
   {
      fprintf ( stderr, "%s\n", game_about_info[i] );
      i++;
   }
         
   return 0;
}

/*================================================================*/

#if defined(__AROS__)
static inline ULONG makecolor(UBYTE val)
{
    return (val << 24) | (val << 16) | (val << 8) | (val);
}
#else
#define makecolor(val)     ((val) * 0x01010101)
#endif

int Graphics_init ( unsigned int win_width, unsigned int win_height )
{
   ULONG newcol;
   int i, j;

   window_width  = win_width;
   window_height = win_height;

   XMin = 0;
   YMin = 0;
   XMax = win_width - 1;
   YMax = win_height - 1;

   scr = OpenScreenTags(0, SA_Width	, window_width	,
   			   SA_Height	, window_height	,
			   SA_Depth	, 8		,
			   SA_Quiet	, TRUE		,
			   TAG_DONE);
   if (!scr)
   {
      fprintf ( stderr, "Error: could not open screen!\n" );
      return FALSE;
   }
   
   if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) < 8)
   {
      fprintf ( stderr, "Error: need at least a 256 color screen!\n" );
      return FALSE;
   }
   
   draw_bm = AllocBitMap(window_width, window_height, 0, BMF_MINPLANES, scr->RastPort.BitMap);
   if (!draw_bm)
   {
      fprintf ( stderr, "Error: can't alloc draw bitmap!\n" );
      return FALSE;
   }
   
   draw_rp = CreateRastPort();
   if (!draw_rp)
   {
      fprintf ( stderr, "Error: can't alloc draw rastport!\n" );
      return FALSE;
   }
   
   draw_rp->BitMap = draw_bm;

   win = OpenWindowTags(0, WA_CustomScreen	, (IPTR) scr	,
   			   WA_Left		, 0		,
   			   WA_Top		, 0		,
			   WA_Width		, window_width	,
			   WA_Height		, window_height	,
			   WA_Activate		, TRUE		,
			   WA_Borderless	, TRUE		,
			   WA_IDCMP		, IDCMP_RAWKEY	,
			   TAG_DONE);
   
   if (!win)
   {
      fprintf ( stderr, "Error: can't open window!\n" );
      return FALSE;
   }
   
   win_rp = win->RPort;
   
   /* load default color scheme */

   coltable[0] = 256 << 16L;
   
   /* red */
   for ( i=0, j=0; i<64; i++, j++ )
   {
      coltable[1 + i * 3]     = makecolor(j * 4);
      coltable[1 + i * 3 + 1] = 0;
      coltable[1 + i * 3 + 2] = 0;
   }

   /* green */
   for ( i=64, j=0; i<128; i++, j++ )
   {
      coltable[1 + i * 3]     = 0;
      coltable[1 + i * 3 + 1] = makecolor(j * 4);
      coltable[1 + i * 3 + 2] = 0;
   }
   
   /* blue */
   for ( i=128, j=0; i<192; i++, j++ )
   {
      coltable[1 + i * 3]     = 0;
      coltable[1 + i * 3 + 1] = 0;
      coltable[1 + i * 3 + 2] = makecolor(j * 4);
   }

   /* white */
   for ( i=192, j=0; i<256; i++, j++ )
   {
      newcol = makecolor(j * 4);
      coltable[1 + i * 3]     = newcol;
      coltable[1 + i * 3 + 1] = newcol;
      coltable[1 + i * 3 + 2] = newcol;
   }

   /* yellow */
   newcol = makecolor(255UL);
   coltable[1 + 192 * 3]     = newcol;
   coltable[1 + 192 * 3 + 1] = newcol;
   coltable[1 + 192 * 3 + 2] = makecolor(128UL);

   LoadRGB32(&scr->ViewPort, coltable);

   SetRast(win_rp, 0);
   SetRast(draw_rp, 0);
   
   return TRUE;
}

/*================================================================*/

void Graphics_shutdown ( void )
{
   if (win) CloseWindow(win);
   if (draw_rp) FreeRastPort(draw_rp);
   if (draw_bm)
   {
      WaitBlit();
      FreeBitMap(draw_bm);
   }
   if (scr) CloseScreen(scr);
}

/*================================================================*/

int Update_display ( void )
{
   BltBitMapRastPort(draw_bm, 0, 0,
   		     win_rp, 0, 0, window_width, window_height, 192);
   SetRast(draw_rp, 0);
   
   return TRUE;
}

/*================================================================*/

int Handle_events ( void )
{
   struct IntuiMessage *imsg;
   int success = TRUE;
   
   while((imsg = (struct IntuiMessage *)GetMsg(win->UserPort)))
   {
      switch(imsg->Class)
      {
         case IDCMP_RAWKEY:
	    switch(imsg->Code)
	    {
	        case 0x40: /* SPACE */
		case 0x63: /* CTRL */
		   gv->key_FIRE = TRUE;
		   break;

		case 0x40 | IECODE_UP_PREFIX:
		case 0x63 | IECODE_UP_PREFIX:
		   gv->key_FIRE = FALSE;
		   break;
		
		case 0x4C: /* CURSOR UP */
		case 0x3E: /* NUM 8 */
		   gv->key_UP = TRUE;
		   break;
		   
		case 0x4C | IECODE_UP_PREFIX:
		case 0x3E | IECODE_UP_PREFIX:
		   gv->key_UP = FALSE;
		   break;

		case 0x4D: /* CURSOR DOWN */
		case 0x1E: /* NUM 2 */
		   gv->key_DOWN = TRUE;
		   break;
		   
		case 0x4D | IECODE_UP_PREFIX:
		case 0x1E | IECODE_UP_PREFIX:
		   gv->key_DOWN = FALSE;
		   break;

		case 0x4F: /* CURSOR LEFT */
		case 0x2D: /* NUM 4 */
		   gv->key_LEFT = TRUE;
		   break;
		   
		case 0x4F | IECODE_UP_PREFIX:
		case 0x2D | IECODE_UP_PREFIX:
		   gv->key_LEFT = FALSE;
		   break;

		case 0x4E: /* CURSOR RIGHT */
		case 0x2F: /* NUM 2 */
		   gv->key_RIGHT = TRUE;
		   break;
		   
		case 0x4E | IECODE_UP_PREFIX:
		case 0x2F | IECODE_UP_PREFIX:
		   gv->key_RIGHT = FALSE;
		   break;
		
		case 0x23: /* F */
		   gv->display_fps ^= TRUE;
		   break;
		   
		case 0x19: /* P */
		   Game_paused_toggle ();
		   break;
		
		case 0x10: /* Q */
		   Game_reset ();
		   break;
		
		case 0x45: /* ESC */
		   /* quit! */
		   success = FALSE;
		   break;
		   
	    } /* switch(imsg->Code) */
	      
	    break;
	      
      } /* switch(imsg->Class) */
      
      ReplyMsg((struct Message *)imsg);
      
   } /* while((imsg = (struct IntuiMessage *)GetMsg(win->UserPort))) */
   
   return success;
}

/*================================================================*/

void Draw_line ( int x0, int y0, int x1, int y1, unsigned int color )
{
   if (clipline(&x0, &y0, &x1, &y1))
   {
      SetAPen(draw_rp, color);
      Move(draw_rp, x0, y0);
      Draw(draw_rp, x1, y1);
   }
}

/*================================================================*/

void Draw_point ( int x0, int y0, unsigned int color )
{
   if ( x0 > 2 && y0 > -2 && x0 < XMax && y0 < YMax - 5 )
   {
      SetAPen(draw_rp, color);
      RectFill(draw_rp, x0 - 3, y0 + 3, x0 - 3 + 3 - 1, y0 + 3 + 3 - 1);
   }
}

/*================================================================*/

void Draw_text ( char *message, int x0, int y0, unsigned int color )
{
   SetAPen(draw_rp, color);
   SetDrMd(draw_rp, JAM1);
   Move(draw_rp, x0, y0);
   Text(draw_rp, message, strlen(message));
} 

/*================================================================*/

/*------------------------------------------------------------------
 *
 * System msec & sec Timer functions
 *
------------------------------------------------------------------*/

void Timer_init ( TIMER *t )
{
   t->init_time_stamp = time ( NULL );

   gettimeofday ( &(t->t0), NULL );
   gettimeofday ( &(t->t1), NULL );
}

/*================================================================*/

CLOCK_T Timer_ticks ( void )
{
   return clock ();
}

/*================================================================*/

double Timer_sec ( TIMER *t )
{
   return difftime ( time(NULL), t->init_time_stamp );
}

/*================================================================*/

long Timer_msec ( TIMER *t )
{
   long msec;
  
   if ( gettimeofday ( &(t->t1), NULL ) < 0 ) return -1;

   msec = ((t->t1.tv_sec-t->t0.tv_sec)*1000L)+
      ((t->t1.tv_usec-t->t0.tv_usec)/1000L);

   t->t0.tv_sec = t->t1.tv_sec;
   t->t0.tv_usec = t->t1.tv_usec;

   return msec;
}

/*================================================================*/

int clipline(int *px1, int *py1, int *px2, int *py2)
{
   int x1, y1, x2, y2;
   int K1=0,K2=0;
   int dx,dy;

   x1 = *px1; y1 = *py1; x2 = *px2; y2 = *py2;

   dx=x2-x1;
   dy=y2-y1;

   if(y1<YMin) K1 =CLIPLOWER;
   if(y1>YMax) K1 =CLIPUPPER;
   if(x1<XMin) K1|=CLIPLEFT;
   if(x1>XMax) K1|=CLIPRIGHT;

   if(y2<YMin) K2 =CLIPLOWER;
   if(y2>YMax) K2 =CLIPUPPER;
   if(x2<XMin) K2|=CLIPLEFT;
   if(x2>XMax) K2|=CLIPRIGHT;

   while(K1 || K2)
   {

      if(K1 & K2)
	 return FALSE;

      if(K1)
      {
	 if(K1 & CLIPLEFT)
	 {
	    y1+=(XMin-x1)*dy/dx;
	    x1=XMin;
         }
	 else if(K1 & CLIPRIGHT)
	 {
            y1+=(XMax-x1)*dy/dx;
	    x1=XMax;
         }

         if(K1 & CLIPLOWER)
         {
	    x1+=(YMin-y1)*dx/dy;
            y1=YMin;
	 }
         else if(K1 & CLIPUPPER)
         {
            x1+=(YMax-y1)*dx/dy;
            y1=YMax;
	 }
         K1 = 0;

         if(y1<YMin) K1 =CLIPLOWER;
         if(y1>YMax) K1 =CLIPUPPER;
         if(x1<XMin) K1|=CLIPLEFT;
         if(x1>XMax) K1|=CLIPRIGHT;
      }

      if(K1 & K2)
         return FALSE;

      if(K2)
      {
	 if(K2 & CLIPLEFT)
    	 {
            y2+=(XMin-x2)*dy/dx;
            x2=XMin;
         }
         else if(K2 & CLIPRIGHT)
         {
            y2+=(XMax-x2)*dy/dx;
            x2=XMax;
         }
	 
	 if(K2 & CLIPLOWER)
         {
            x2+=(YMin-y2)*dx/dy;
	    y2=YMin;
         }
         else if(K2 & CLIPUPPER)
         {
            x2+=(YMax-y2)*dx/dy;
            y2=YMax;
         }

         K2 = 0;
         if(y2<YMin) K2 =CLIPLOWER;
         if(y2>YMax) K2 =CLIPUPPER;
         if(x2<XMin) K2|=CLIPLEFT;
         if(x2>XMax) K2|=CLIPRIGHT;
      }
   }

   *px1 = x1; *py1 = y1; *px2 = x2; *py2 = y2;
   return TRUE;
}

/*================================================================*/
