#include "scrndef.h"
/*
 * Menu description for selecting color mapping
 */

/*
 * define mutual exclusion flags
 */
#define MUBIT1 1
#define MUBIT2 2
#define MUBIT3 4
#define MUBIT4 8
#define MUBIT5 16
#define MULO 32
#define MUHI 64
#define MUHAM 128
#define MUOVER 256

static struct IntuiText scrntext[] = {
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"2 color",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"4 color ",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"8 color", NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"16 color",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"32 color",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"lores",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"hires",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"ham",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"overscan",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"interlace",  NULL },
};

static struct MenuItem scrnitems[] = {
  { &scrnitems[1], /* next item */
    10, 0, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS ,
    MUBIT2|MUBIT3|MUBIT4|MUBIT5, /* mutual exclude bits */
    (APTR) &scrntext[0],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[2], /* next item */
    10, 10, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    MUBIT1|MUBIT3|MUBIT4|MUBIT5, /* mutual exclude bits */
    (APTR) &scrntext[1],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[3], /* next item */
    10, 20, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    MUBIT1|MUBIT2|MUBIT4|MUBIT5, /* mutual exclude bits */
    (APTR) &scrntext[2],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[4], /* next item */
    10, 30, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    MUBIT1|MUBIT2|MUBIT3|MUBIT5, /* mutual exclude bits */
    (APTR) &scrntext[3],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[5], /* next item */
    10, 40, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    MUBIT1|MUBIT2|MUBIT3|MUBIT4|MUHI, /* mutual exclude bits */
    (APTR) &scrntext[4],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[6], /* next item */
    10, 50, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    MUHI|MUHAM, /* mutual exclude bits */
    (APTR) &scrntext[5],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[7], /* next item */
    10, 60, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    MULO|MUHAM|MUBIT5, /* mutual exclude bits */
    (APTR) &scrntext[6],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[8], /* next item */
    10, 70, 90 , 10, /* x,y,w,h */
    (COLMEMFLAGS) & ~ITEMENABLED,
    MULO|MUHI, /* mutual exclude bits */
    (APTR) &scrntext[7],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &scrnitems[9], /* next item */
    10, 80, 90 , 10, /* x,y,w,h */
    ( COLMEMFLAGS|MENUTOGGLE)& ~ITEMENABLED ,
    0, /* mutual exclude bits */
    (APTR) &scrntext[8],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    10, 90, 90 , 10, /* x,y,w,h */
    COLMEMFLAGS|MENUTOGGLE,
    0, /* mutual exclude bits */
    (APTR) &scrntext[9],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};

extern void CloseSurf();
extern void OpenSurf();

void MenuSetScrn()
{
    CloseSurf();

    /*
     * overscan
     */
    SurfScrnDef.Width = 320;
    SurfScrnDef.Height = 200+ButHeight;
    SurfScrnDef.LeftEdge = SurfScrnDef.TopEdge = 0;

    if( Selected(scrnitems[8])) {
        SurfScrnDef.Width = 352;
        SurfScrnDef.Height = 220;
    }

    if( Selected(scrnitems[7])) { /* ham mode */
        SurfScrnDef.Depth = 6;
        SurfScrnDef.ViewModes = HAM;
    }
    else {
        int i;
        SurfScrnDef.Depth = 3; /* incase non of the flags are set */

        for( i = 0; i < 5; i++ ) {
            if( Selected( scrnitems[i])) {
                SurfScrnDef.Depth = i+1;
            }
        }

        if( Selected( scrnitems[5] ) ) { /* lores */
            SurfScrnDef.ViewModes = 0;
        }
        else {
            SurfScrnDef.ViewModes = HIRES;
            SurfScrnDef.Width <<= 1;
            SurfScrnDef.LeftEdge *= 2;
            if( SurfScrnDef.Depth > 4 ) {
                SurfScrnDef.Depth = 4;
            }
        }
    }

    if( Selected( scrnitems[9] )) {   /* interlace */
        SurfScrnDef.Height <<= 1;
        SurfScrnDef.Height -= ButHeight;
        SurfScrnDef.TopEdge *= 2;
        SurfScrnDef.ViewModes |= LACE;
    }


    OpenSurf();
}
