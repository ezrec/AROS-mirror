
/*
 * Menu description for selecting color mapping
 */
static struct IntuiText colortext[] = {
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"grey",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"red",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"green", NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"blue",  NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"rainbow",NULL }
};

#define ColNum (sizeof(colortext)/sizeof(struct IntuiText))
#define ColXMask ((1<<ColNum)-1)
#define ColorExclude(entry) (ColXMask^( 1<<entry))

#define COLMEMFLAGS  ( CHECKIT | ITEMTEXT | HIGHCOMP | ITEMENABLED )

extern void SetHourGlassCol();
extern void SetRainbow();


struct MenuItem coloritems[] = {
  { &coloritems[1], /* next item */
    10, 0, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    ColorExclude(0), /* mutual exclude bits */
    (APTR) &colortext[0],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &coloritems[2], /* next item */
    10, 10, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    ColorExclude(1), /* mutual exclude bits */
    (APTR) &colortext[1],     /* red */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &coloritems[3], /* next item */
    10, 20, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    ColorExclude(2), /* mutual exclude bits */
    (APTR) &colortext[2],
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &coloritems[4], /* next item */
    10, 30, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    ColorExclude(3), /* mutual exclude bits */
    (APTR) &colortext[3],
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    10, 40, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    ColorExclude(4), /* mutual exclude bits */
    (APTR) &colortext[4],
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};



void MenuSetColMap()
{
    int which;

    for( which = 0; which < ColNum; which++ ) {
        if( Selected(coloritems[which]))
            break;
    }

    SetHourGlassCol();

    switch( which ) {
    case 0:
        SetMono( 0xf, 0xf, 0xf );
        break;
    case 1:
        SetMono( 0xf, 0, 0 );
        break;
    case 2:
        SetMono( 0, 0xf, 0 );
        break;
    case 3:
        SetMono( 0x0, 0x0, 0xf );
        break;
    case 4:
        SetRainbow();
        break;
    default:
        break;
    }
}
