#include "gadgetdef.h"

static struct IntuiText filetext[] = {
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"save as", NULL },
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"save first", NULL },
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"save next", NULL },
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"open map", NULL },
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"close map", NULL },
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"write data", NULL }
};

static struct IntuiText greytext[] = {
   { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"Grey model",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Average",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Lumin",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"Distance",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"R only",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"G only",   NULL },
   { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"B only",   NULL }
};

static struct IntuiText packtext[] = {
    { 0, 1, JAM2, 2, 0, NULL, (UBYTE *)"compression", NULL },
    { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"run length", NULL },
    { 0, 1, JAM2, 20, 0, NULL, (UBYTE *)"none", NULL }
};



#define FILEFLAGS  ( ITEMTEXT | HIGHCOMP | ITEMENABLED )

static struct MenuItem packitems[] = {
  { &packitems[1], /* next item */
    90, 0, 110 , 10, /* x,y,w,h */
    COLMEMFLAGS| CHECKED,
    2, /* mutual exclude bits */
    (APTR) &packtext[1],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    90, 10, 110 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    1, /* mutual exclude bits */
    (APTR) &packtext[2],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};

#define GREYMUTUAL(pos) (077 ^ (1<<(pos)))
static struct MenuItem greyitems[] = {
  { &greyitems[1], /* next item */
    90, 0, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS|CHECKED,
    GREYMUTUAL(0), /* mutual exclude bits */
    (APTR) &greytext[1],  /* average */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &greyitems[2], /* next item */
    90, 10, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    GREYMUTUAL(1), /* mutual exclude bits */
    (APTR) &greytext[2],  /* lumin */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &greyitems[3], /* next item */
    90, 20, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    GREYMUTUAL(2), /* mutual exclude bits */
    (APTR) &greytext[3],  /* dist */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &greyitems[4], /* next item */
    90, 30, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    GREYMUTUAL(3), /* mutual exclude bits */
    (APTR) &greytext[4],  /* dist */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &greyitems[5], /* next item */
    90, 40, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    GREYMUTUAL(4), /* mutual exclude bits */
    (APTR) &greytext[5],  /* dist */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    90, 50, 80 , 10, /* x,y,w,h */
    COLMEMFLAGS,
    GREYMUTUAL(5), /* mutual exclude bits */
    (APTR) &greytext[6],  /* dist */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};




static struct MenuItem fileitems[] = {
  { &fileitems[1], /* next item */
    10, 0, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[0],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[2], /* next item */
    10, 10, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[1],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[3], /* next item */
    10, 20, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[2],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[4], /* next item */
    10, 30, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &packtext[0],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    packitems, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[5], /* next item */
    10, 40, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[3],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[6], /* next item */
    10, 50, IMAGE_HITWIDTH , 10, /* x,y,w,h */
    COLMEMFLAGS,
    1, /* mutual exclude bits */
    (APTR) &greytext[0],     /* red */
    NULL, /* highlight image */
    'h', /* command byte ? */
    greyitems, /* submenu item */
    0 /* next select for select dragging */
    },
  { &fileitems[7], /* next item */
    10, 60, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[4],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    },
  { NULL, /* next item */
    10, 70, 90 , 10, /* x,y,w,h */
    FILEFLAGS,
    0, /* mutual exclude bits */
    (APTR) &filetext[5],  /* grey */
    NULL, /* highlight image */
    'h', /* command byte ? */
    NULL, /* submenu item */
    0 /* next select for select dragging */
    }
};

extern char *GetOutFile();
extern char *GetInFile();
extern void SetHourGlass();
extern void ClearHourGlass();
extern void WriteIlbm(char *, struct NewWindow *, struct NewScreen *, bool);
extern void ReadIlbm(char *);
extern void WriteData(char *);

void MenuDoFile(item)
int item;
{
    static int filecnt = 0;
    char tempbuff[80];
    char *filename;
    bool packflag;
    int i;

    packflag = Selected( packitems[0] )?1:0;


    switch (item ) {
    case 0:
        if( (filename = GetOutFile())) {
            SetHourGlass();
            WriteIlbm(filename, &SurfWinDef, &SurfScrnDef, packflag);
        }
        break;
    case 1:
        filecnt = 0; /* deliberate fall into case 2 */
    case 2:
        if( (filename = GetOutFile()) != NULL) {
            SetHourGlass();
            sprintf(tempbuff, "%s.%d", filename, filecnt++ );
            WriteIlbm(tempbuff, &SurfWinDef, &SurfScrnDef, packflag);
        }
        break;
    case 4:
        if( NULL != (filename = GetInFile())){
            SetHourGlass();
            ReadIlbm( filename);
        }
        break;
    case 5:
        for( i = 0; i < (sizeof(greyitems)/sizeof(greyitems[0])); i++ ) {
            if( Selected(greyitems[i])) {
                SetGreyModel(i);
            }
        }
        break;
    case 6:
        CloseImgPix();
        break;
    case 7:
        if( NULL != (filename = GetOutFile())) {
            SetHourGlass();
            WriteData(filename);
        }
        break;

    default:
        break;
    }

    ClearHourGlass();
}
