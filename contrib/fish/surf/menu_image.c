#include "menuexp.h"


/*
 * Menu description for selecting color mapping
 */
static struct IntuiText Specular = {
     0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Specular",  NULL
};

static struct IntuiText revtext[] = {
   { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"Rev. Axis",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"X",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Y",   NULL }
};

static struct IntuiText dithertext[] = {
   { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"Dither",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"none",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"2",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"4",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"8",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"16",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"32",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"64",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"128",   NULL }
};


static struct IntuiText MiscText[] = {
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Abort Draw",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Debug",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Flip XY Map", NULL }
};

struct MenuItem revitems[] = {
  { &revitems[1], /* next item */
    90, 0, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    2, /* mutual exclude bits */
    (APTR) &revtext[1],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    90, 10, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    1, /* mutual exclude bits */
    (APTR) &revtext[2],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};

#define DitherExMask(X) (((1<<8)-1) ^ (1<<X))

struct MenuItem ditheritems[] = {
  { &ditheritems[1], /* next item */
    90, 0, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(0), /* mutual exclude bits */
    (APTR) &dithertext[1],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[2], /* next item */
    90, 10, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(1), /* mutual exclude bits */
    (APTR) &dithertext[2],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[3], /* next item */
    90, 20, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    DitherExMask(2), /* mutual exclude bits */
    (APTR) &dithertext[3],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[4], /* next item */
    90, 30, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(3), /* mutual exclude bits */
    (APTR) &dithertext[4],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[5], /* next item */
    90, 40, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(4), /* mutual exclude bits */
    (APTR) &dithertext[5],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[6], /* next item */
    90, 50, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(5), /* mutual exclude bits */
    (APTR) &dithertext[6],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &ditheritems[7], /* next item */
    90, 60, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(6), /* mutual exclude bits */
    (APTR) &dithertext[7],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    90, 70, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    DitherExMask(7), /* mutual exclude bits */
    (APTR) &dithertext[8],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};


#define IMAGE_HITWIDTH 96

struct MenuItem imageitems[] = {
  { &imageitems[1], /* next item */
    1, 0, IMAGE_HITWIDTH , 10, /* x,y,w,h */
    COLMEMFLAGS,
    2, /* mutual exclude bits */
    (APTR) &revtext[0],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    revitems, /* submenu item */
    0 /* next select for select dragging */
    },
  { &imageitems[2], /* next item */
    1, 10, IMAGE_HITWIDTH , 10, /* x,y,w,h */
    COLMEMFLAGS,
    1, /* mutual exclude bits */
    (APTR) &dithertext[0],     /* red */
    NULL, /* highlight image */
    'h', /* command byte ? */
    ditheritems, /* submenu item */
    0 /* next select for select dragging */
    },
  { &imageitems[3], /* next item */
    1, 20, IMAGE_HITWIDTH , 10, /* x,y,w,h */
    COLMEMFLAGS|MENUTOGGLE,
    0, /* mutual exclude bits */
    (APTR) &Specular,
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &imageitems[4], /* next item */
    1, 30, IMAGE_HITWIDTH , 10, /* x,y,w,h */
    COLMEMFLAGS|MENUTOGGLE,
    0, /* mutual exclude bits */
    (APTR) &MiscText[2], /* Flip XY */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &imageitems[5], /* next item */
    1, 40, 100 , 10, /* x,y,w,h */
    COLMEMFLAGS| MENUTOGGLE,
    0, /* mutual exclude bits */
    (APTR) &MiscText[0], /* AbortDraw */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    1, 50, IMAGE_HITWIDTH, 10, /* x,y,w,h */
    COLMEMFLAGS|MENUTOGGLE,
    0, /* mutual exclude bits */
    (APTR) &MiscText[1],   /* debug */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};

USHORT *AbortDrawPtr = &imageitems[4].Flags;
USHORT *DebugOnPtr = &imageitems[5].Flags;


static void MenuSetImage()
{
    int i;
    if( Selected(revitems[0])) {
        SetRevAxis(0); /* Xaxis */
    }
    else {
        SetRevAxis(1); /* Yaxis */
    }

    for( i = 0; i < sizeof(ditheritems)/sizeof(ditheritems[0]); i++ ) {
        if( Selected(ditheritems[i] )) {
            if( i != DitherPower ) {
                FreeOldDither();
            }
            DitherPower = i;
        }
    }

    SpecOn = Selected( imageitems[2])?true:false;
    FlipImgPix( Selected(imageitems[3])?true:false);
}

