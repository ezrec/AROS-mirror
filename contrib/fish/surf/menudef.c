#include <aros/oldprograms.h>
#include <stdio.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#ifdef MANX
#include <functions.h>
#endif
#include "scrnio.h"
#include "menudef.h"

#include "scrndef.h"
#include "poly.h"
#include "readilbm.h"

#define Selected(ax) ((ax).Flags & CHECKED)

#include "menu_color.c"
#include "menu_scrn.c"
#include "menu_image.c"
#include "menu_files.c"

struct Menu menu[] ={
    { &menu[1], /* next menu */
    8, 0, /* x, y */
    6*8, 10, /* w,h */
    MENUENABLED,
    (BYTE *)"File",
    fileitems,
    0,0,0,0
    },
    { &menu[2], /* next menu */
    7*8, 0, /* x, y */
    6*8, 10, /* w,h */
    MENUENABLED,
    (BYTE *)"Color",
    coloritems,
    0,0,0,0
    },
    { &menu[3], /* next menu */
    14*8,0, /* x, y */
    7*8, 10, /* w,h */
    MENUENABLED,
    (BYTE *)"Screen",
    scrnitems,
    0,0,0,0
    },
    { NULL, /* next menu */
    22*8,0, /* x, y */
    6*8, 10, /* w,h */
    MENUENABLED,
    (BYTE *)"Image",
    imageitems,
    0,0,0,0
    }
};



void MenuHandler(code)
USHORT code;
{
    if( code == 0xffff ) { /* invalid menu pick */
        return;
    }

    switch( MENUNUM(code)) {
        case 0: /* write ilbm */
            MenuDoFile(ITEMNUM(code));
            break;
        case 1: /* set screen color */
            MenuSetColMap();
            break;
        case 2: /* set screen type */
            MenuSetScrn();
            break;
        case 3: /* set image stuff */
            MenuSetImage();
            break;
        default:
            break;
    }
}
