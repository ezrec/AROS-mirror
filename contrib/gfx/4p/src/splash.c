/*
** splash.c
**/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "jsui.h"
#include "version.h"

// this isn't the most efficient way of doing this, but it's quick and easy...

#define USE_LLAMA (1)

unsigned char twocharacters[2][8][5] =
{
#ifdef USE_LLAMA
    { /* llama butt */
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 1 },
        { 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1 },
        { 0, 1, 0, 0, 0 },
        { 0, 1, 0, 0, 0 },
    },
    { /* llama head */
        { 0, 1, 0, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 1, 0, 0, 0 },
        { 1, 1, 0, 0, 0 },
        { 1, 1, 0, 0, 0 },
        { 0, 1, 0, 0, 0 },
        { 0, 1, 0, 0, 0 }
    }

#else

    { /* 4 */
        { 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 1 },
        { 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 1 },
    },
    { /* P */
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0 }
    }
#endif
};

unsigned char twocharacterscolors[8] = { 2,4,5,6,8,9,10,11 };

void
draw_two_characters(
        IPAGE * dst,
        PEN * pen
)
{
    int c,x,y;

    if (!dst || !pen) return;
    
#define TC_SIZE (14)
#define TC_OFFS (15)
#define TC_WIDTH (5)
    for (c=0 ; c<2 ; c++)
    {
        for (y=0 ; y<8 ; y++)
        {
            pen->Icolor = twocharacterscolors[y];
            for (x=0 ; x<5 ; x++)
            {
                if (twocharacters[c][y][x])
                {
                    primitive_rectangle_filled(dst, pen, 
                        TC_OFFS+ (x)*TC_SIZE + (c*TC_SIZE*TC_WIDTH), 
			TC_OFFS+(y*TC_SIZE),
                        TC_OFFS+ (x)*TC_SIZE + TC_SIZE + (c*TC_SIZE*TC_WIDTH) - 2, 
                        TC_OFFS+ (y*TC_SIZE) + TC_SIZE - 2
                        );
                }
            }
        }
    }
#undef TC_WIDTH
#undef TC_OFFS
#undef TC_SIZE
}

IPAGE * generate_splash(void)
{
    int myvpos;
    PEN * fgpen = NULL;
    IPAGE * backing_page = Page_Create(jsui_screen->w, jsui_screen->h, jsui_screen->ncolors);

    if (!backing_page) return NULL;
    palette_free( backing_page->palette );
    backing_page->palette = palette_generate_default( 32 );
    backing_page->pdirty = 1;

    fgpen = Pen_Create();
    fgpen->Icolor = 0;
    
    primitive_rectangle_filled(backing_page, fgpen, 0,0, backing_page->w, backing_page->h);

#define GRADSPEED (7)
    // draw some pretty bars
    fgpen->Icolor = 31;
    for (myvpos = 0 ; myvpos < 160 ; myvpos++)
    {
        if (myvpos < (12 * GRADSPEED))
        {
            fgpen->Icolor = myvpos/GRADSPEED + 20;
        } else if (myvpos > (160 - (12*GRADSPEED))) {
            fgpen->Icolor =  (160 - myvpos)/GRADSPEED + 20;
        }            
        primitive_line(backing_page, fgpen, primitive_pixel,
                    myvpos, 479,
                    myvpos+479, 0);
    }
#undef GRADSPEED

    // and now throw some text up there too..
    fgpen->Icolor = 5;
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 75,
                    "UberPaint / 4P");
                    
    fgpen->Icolor = 8;
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 60,
                    "by Scott \"Jerry\" Lawrence");
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 50,
                    "jsl.4p@umlautllama.com");
                    
    fgpen->Icolor = 2;
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 35,
                    "Version " J_VERSION);
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 25,
                    "Rev " J_REVISION);
    font_render_text_right(backing_page, fgpen, fgpen, jsui_current_font, 
                    backing_page->w - 10, backing_page->h - 15,
                    "\"" J_TPOTC "\"");
    
    /* and a cute little hidden message */
    fgpen->Icolor = 21;
    font_render_text_center(backing_page, fgpen, fgpen, jsui_current_font,
                    backing_page->w/2, (backing_page->h/2)-10,
                    "\x54\x68\x61\x6e\x6b\x73\x2c");
    font_render_text_center(backing_page, fgpen, fgpen, jsui_current_font,
                    backing_page->w/2, (backing_page->h/2),
                    "\x44\x61\x6e\x20\x53\x69\x6c\x76\x61");
    font_render_text_center(backing_page, fgpen, fgpen, jsui_current_font,
                    backing_page->w/2, (backing_page->h/2)+10,
                    "\x61\x6e\x64\x20\x45\x41");
    /* tee-hee! */
    
    draw_two_characters(backing_page, fgpen);

    Pen_Destroy(fgpen);
    Page_DR_Dirtify(backing_page, 0, 0, backing_page->w, backing_page->h);
    return (backing_page);
}

