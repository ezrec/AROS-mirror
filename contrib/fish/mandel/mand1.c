/*
                   MAND1.C - Graphics routines
             Mandelbrot Self-Squared Dragon Generator
                    For the Commodore Amiga
                         Version 1.00

                      Accompanies MAND.C

             Copyright (C) 1985, Robert S. French
                  Placed in the Public Domain

     Assorted Goodies and Intuition-stuff by =RJ Mical=  1985
      Hope you appreciate them.  I especially like the zoom.

This program may be distributed free of charge as long as the above
notice is retained.

*/


#include "mand.h"

/*----------------------*/
/* Graphics definitions */

extern struct   GfxBase       *GfxBase;
extern struct   IntuitionBase *IntuitionBase;
 
extern struct   RastPort      *rp,*rp2;
extern struct   ViewPort      *vp;
 
extern struct   Window        *w,*w2;
extern struct   Screen        *screen;
extern struct   IntuiMessage  *message;
 
extern long last_color;

extern BOOL SettingCenter, SettingBoxSize;

/*----------------------------------*/
/* Miscellaneous Global Definitions */

extern union kludge {
   float f;
   KLUDGE_INT i;
} start_r,end_r,start_i,end_i;  /* Block bounds for set */
extern int max_x,max_y;  /* Graphics window size */
extern int max_count,color_inc,color_offset,color_set,color_mode,color_div;
extern int color_inset,func_num;

extern int v_starty,max_mem;
extern long v_offset;
extern UWORD *color_table,*v_mand_store;

extern int modified,want_read;

extern FILE *console,*v_fp,*redir_fp;
 
extern SHORT ZoomCenterX, ZoomCenterY, ZoomBoxSizeX, ZoomBoxSizeY;
extern SHORT ZoomBoxStartX, ZoomBoxStartY;

void loc_abort(char *s);

/*----------------*/
/* Color routines */

init_colors()
{
   switch (color_set) {
      case 0: init_cincr(); break;
      case 1: init_c7rot(); break;
   }
}

init_cincr()
/* this routine initializes the color table with sequential values from
 * 0 to 4095
 */
{
   UWORD i;

   for (i=0;i<4096;i++)
      *(color_table + i) = i;
}


init_c7rot()
/* this routine initializes the color table with this pattern:
 * 0 = 0
 * 1-15 = unit steps of blue
 * 16-30 = unit steps of green
 * 31-45 = unit steps of red
 * 46-60 = unit steps of sky sky blue (blue and green)
 * 61-75 = unit steps of purple (blue and red)
 * 76-90 = unit steps of yellow (green and red)
 * 91-115 = unit steps of white (all colors)
 */
{
   UWORD i,j,*base;

   base = color_table;
   *(base++) = 0;

   for (j = 0; j < 39; j++) {
      for (i = 1; i < 16; i++)
         *(base++) = i; /* gimme the blues */
      for (i = 1; i < 16; i++)
         *(base++) = i << 4; /* these are the greens */
      for (i = 1; i < 16; i++)
         *(base++) = i << 8; /* the reds */
      for (i = 1; i < 16; i++)
         *(base++) = i | (i << 4); /* blue and green */
      for (i = 1; i < 16; i++)
         *(base++) = i | (i << 8); /* blue and red */
      for (i = 1; i < 16; i++)
         *(base++) = (i << 4) | (i << 8); /* green and red */
      for (i = 1; i < 16; i++)
         *(base++) = i | (i << 4) | (i << 8); /* all three */
   }
}


gen_mand()
{
   void write_out();

   int x_coord,y_coord,count;
   union kludge x_gap,y_gap,z_r,z_i,u_r,u_i,temp,temp2,temp3,const0;
   union kludge const1,const2,const12;
   ULONG class;
   USHORT code;

    /* if there was a display open, close it now */
   CloseDisplay();
   /* reset the display variables: */
   SettingCenter = SettingBoxSize = FALSE;

   if (v_fp) {
      fclose(v_fp);
      v_fp = NULL;
   }

   v_fp = fopen("Mandelbrot.temp.file","w+");
   if (v_fp == NULL) {
      loc_abort("Can't open temporary file!");
   }

   v_starty = 1;
   v_offset = 0L;
   modified = FALSE;
   want_read = FALSE;
   ZoomCenterX = (max_x >> 1);
   ZoomCenterY = (max_y >> 1);
   ZoomBoxSizeX = max_x;
   ZoomBoxSizeY = max_y;

   if (open_winds())
      return (1);

   x_gap.i = SPDiv(SPFlt(max_x),SPSub(start_r.i,end_r.i));
   y_gap.i = SPDiv(SPFlt(max_y),SPSub(start_i.i,end_i.i));

   const0.i = SPFlt(0);
   const1.i = SPFlt(1);
   const2.i = SPFlt(2);
   const12.i = SPDiv(SPFlt(2),SPFlt(1));

   for (y_coord = 0; y_coord < max_y; y_coord++)
      {
      last_color = 0xfff;
      v_pos_line(y_coord);
      modified = TRUE;
      for (x_coord = 0; x_coord < max_x; x_coord++)
         {
         while (message = (struct IntuiMessage *)GetMsg(w->UserPort))
            {
            class = message->Class;
            code  = message->Code;
            ReplyMsg((struct Message *)message);
            if (class == MENUPICK)
               {
               switch MENUNUM(code)
                  {
                  case MENU_OPTIONS:
                     switch ITEMNUM(code)
                        {
                        case OPTIONS_STOP:
                           return (0);
                        case OPTIONS_CLOSE:
                           CloseDisplay();
                           fclose(v_fp);
                           v_fp = NULL;
                           return (1);
                        }
                  }
               }
            }

         if (func_num == 0)
            z_r.i = const0.i;
         else
            z_r.i = const12.i;

         z_i.i = const0.i;
         u_r.i = SPAdd(start_r.i,SPMul(SPFlt(x_coord),x_gap.i));
         u_i.i = SPAdd(start_i.i,SPMul(SPFlt(max_y-y_coord-1),y_gap.i));

#if 0
kprintf("%d %d %d %d\n", (LONG)(start_r.f * 1000.0),
    	    	    	 (LONG)(x_gap.f * 1000.0),
    	    	    	 (LONG)(start_i.f * 1000.0),
    	    	    	 (LONG)(y_gap.f * 1000.0));
			 
u_r.f = -2.0  + (((float)x_coord) * 4.0 / 320.0);
u_i.f = -2.0  + (((float)y_coord) * 4.0 / 200.0);
#endif

         count = 0;

         for (count = 0;
              SPFix( SPAdd( SPMul(z_r.i, z_r.i), SPMul(z_i.i,z_i.i))) < 4
                 && (count < max_count);
              count++) {
            if (func_num == 0) {  /* z = z^2-u */
               temp.i = SPSub(u_r.i,SPSub(SPMul(z_i.i,z_i.i),SPMul(z_r.i,z_r.i)));
               z_i.i = SPSub(u_i.i,SPMul(SPMul(const2.i,z_r.i),z_i.i));
               z_r = temp;
            }
            else if (func_num == 1) {  /* z = uz(1-z) */
               temp2.i = SPSub(z_r.i,const1.i);
               temp3.i = SPNeg(z_i.i);
               temp.i = SPSub(SPMul(temp3.i,z_i.i),SPMul(temp2.i,z_r.i));
               z_i.i = SPAdd(SPMul(temp2.i,z_i.i),SPMul(temp3.i,z_r.i));
               z_r = temp;
               temp.i = SPSub(SPMul(z_i.i,u_i.i),SPMul(z_r.i,u_r.i));
               z_i.i = SPAdd(SPMul(z_r.i,u_i.i),SPMul(z_i.i,u_r.i));
               z_r = temp;
            }
         }
         if (count >= max_count)
            count = 0;
         *(v_mand_store+(y_coord-v_starty)*max_x+x_coord) = count;
         write_out(count,x_coord,y_coord);
         }

   }
   DisplayBeep(NULL); /* hey, we're done! */
   return (0);
}
 
disp_mand()
{
   void write_out();

   int x_coord,y_coord,count;

   if (open_winds())
      return (1);

   want_read = TRUE;

   for (y_coord=0;y_coord<max_y;y_coord++) {
      last_color = 0xfff;
      v_pos_line(y_coord);
      for (x_coord=0;x_coord<max_x;x_coord++) {
         count = *(v_mand_store+(y_coord-v_starty)*max_x+x_coord);
         write_out(count,x_coord,y_coord);
      }
   }

   return (0);
}

void write_out(count,x,y)
int count,x,y;
{
   void ham_write();
   SHORT modulus;
   int color;

   y += STARTY;

   if (!(color_mode & NOT_HOLDANDMODIFY)) {
      if (count == 0)
         color = color_inset;
      else
         color = count * color_inc + color_offset;
      ham_write(*(color_table+color),x,y);
   }
   else /* we're not hold-and-modify, so ... */ {
      if (color_mode & HIRES_MODE) modulus = 13;
      else modulus = 29;

      if (count == 0) color = 2;
      else color = (((count - 1) / color_div) % modulus) + 3;
//kprintf("writepixel: color = %d count = %d\n", color, count);
      SetAPen(rp, color);
      WritePixel(rp, x, y);
   }
}

void ham_write(color, x, y)
int color;
int x, y;
{
   if ((color & 0xf00) != (last_color & 0xf00)) {
      SetAPen(rp,((color & 0xf00) >> 8) + 0x20);
      WritePixel(rp,x,y);
      last_color = (last_color & 0xff) | (color & 0xf00);
      return;
   }

   if ((color & 0xf0) != (last_color & 0xf0)) {
      SetAPen(rp,((color & 0xf0) >> 4) + 0x30);
      WritePixel(rp,x,y);
      last_color = (last_color & 0xf0f) | (color & 0xf0);
      return;
   }

   SetAPen(rp,(color & 0xf) + 0x10);
   WritePixel(rp,x,y);
   last_color = (last_color & 0xff0) | (color & 0xf);
   return;
}


DrawZoomCenter()
{
   SetDrMd(w->RPort, COMPLEMENT);
   Move(w->RPort, 0, ZoomCenterY);
   Draw(w->RPort, max_x, ZoomCenterY);
   Move(w->RPort, ZoomCenterX, 0);
   Draw(w->RPort, ZoomCenterX, max_y);
}


RecalcZoomBox()
{
   ZoomBoxSizeX = (abs(w->MouseX - ZoomCenterX) << 1) + 1;
   ZoomBoxSizeY = (abs(w->MouseY - ZoomCenterY) << 1) + 1;
   if (w->MouseX < ZoomCenterX) ZoomBoxStartX = w->MouseX;
   else ZoomBoxStartX = ZoomCenterX - ((ZoomBoxSizeX - 1) >> 1);
   if (w->MouseY < ZoomCenterY) ZoomBoxStartY = w->MouseY;
   else ZoomBoxStartY = ZoomCenterY - ((ZoomBoxSizeY - 1) >> 1);
}


DrawZoomBox()
{
   SHORT endx, endy;

   endx = ZoomBoxStartX + ZoomBoxSizeX - 1;
   endy = ZoomBoxStartY + ZoomBoxSizeY - 1;

   SetDrMd(w->RPort, COMPLEMENT);
   Move(w->RPort, ZoomBoxStartX, ZoomBoxStartY);
   Draw(w->RPort, ZoomBoxStartX, endy);
   Draw(w->RPort, endx, endy);
   Draw(w->RPort, endx, ZoomBoxStartY);
   Draw(w->RPort, ZoomBoxStartX, ZoomBoxStartY);
}


CloseDisplay()
{
   if (w) CloseWindow(w);
   if (screen) CloseScreen(screen);
   w = NULL;
   screen = NULL;
}
