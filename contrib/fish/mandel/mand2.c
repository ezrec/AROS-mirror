/*
                       MAND2.C - Menus
             Mandelbrot Self-Squared Dragon Generator
                    For the Commodore Amiga
                         Version 1.00

                      Accompanies MAND.C

             Copyright (C) 1985, Robert S. French
                  Placed in the Public Domain

     Assorted Goodies and Intuition-stuff by =RJ Mical=  1985

This program may be distributed free of charge as long as the above
notice is retained.

*/


#include "mand.h"


struct IntuiText OptionsText[OPTIONS_COUNT] =
   {
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Stop",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "1/4 size display",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Full-size display",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Redisplay",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Close display",
      NULL,       /* next */
      }
    };

struct IntuiText ZoomText[ZOOM_COUNT] =
   {
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Set Zoom Frame Center",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Set Zoom Frame Size",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Zoom to Current Frame",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Zoom In x 10",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Zoom Out x 2",
      NULL,       /* next */
      },
      {
      0, 1, JAM2, /* frontpen, backpen, drawmode */
      1, 1,       /* left, top */
      NULL,       /* font */
      "Zoom Out x 10",
      NULL,       /* next */
      },
    };

struct MenuItem OptionsItems[OPTIONS_COUNT] =
   {
      {
      &OptionsItems[1],                /* next menuitem */
      0, ITEM_HEIGHT * 0,              /* select box left, top */
      OPTIONS_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | COMMSEQ | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &OptionsText[0],                 /* text */
      0,                               /* select */
      'S',                             /* command */
      0,                               /* next item */
      0,                               /* next select */
      },
      {
      &OptionsItems[2],                /* next menuitem */
      0, ITEM_HEIGHT * 1,              /* select box left, top */
      OPTIONS_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | COMMSEQ | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &OptionsText[1],                 /* text */
      0,                               /* select */
      'Q',                             /* command */
      0,                               /* next item */
      0,                               /* next select */
      },
      {
      &OptionsItems[3],                /* next menuitem */
      0, ITEM_HEIGHT * 2,              /* select box left, top */
      OPTIONS_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | COMMSEQ | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &OptionsText[2],                 /* text */
      0,                               /* select */
      'F',                             /* command */
      0,                               /* next item */
      0,                               /* next select */
      },
      {
      &OptionsItems[4],                /* next menuitem */
      0, ITEM_HEIGHT * 3,              /* select box left, top */
      OPTIONS_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | COMMSEQ | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &OptionsText[3],                 /* text */
      0,                               /* select */
      'R',                             /* command */
      0,                               /* next item */
      0,                               /* next select */
      },
      {
      NULL,                            /* next menuitem */
      0, ITEM_HEIGHT * 4,              /* select box left, top */
      OPTIONS_WIDTH, ITEM_HEIGHT,       /* select box width, height */
      ITEMTEXT | COMMSEQ | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &OptionsText[4],                 /* text */
      0,                               /* select */
      'C',                             /* command */
      0,                               /* next item */
      0,                               /* next select */
      },
   };


struct MenuItem ZoomItems[ZOOM_COUNT] =
   {
      {
      &ZoomItems[1],                /* next menuitem */
      0, ITEM_HEIGHT * 0,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[0],                 /* text */
      },
      {
      &ZoomItems[2],                /* next menuitem */
      0, ITEM_HEIGHT * 1,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[1],                 /* text */
      },
      {
      &ZoomItems[3],                /* next menuitem */
      0, ITEM_HEIGHT * 2,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[2],                 /* text */
      },
      {
      &ZoomItems[4],                /* next menuitem */
      0, ITEM_HEIGHT * 3,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[3],                 /* text */
      },
      {
      &ZoomItems[5],                /* next menuitem */
      0, ITEM_HEIGHT * 4,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,      /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[4],                 /* text */
      },
      {
      NULL,                            /* next menuitem */
      0, ITEM_HEIGHT * 5,              /* select box left, top */
      ZOOM_WIDTH, ITEM_HEIGHT,       /* select box width, height */
      ITEMTEXT | ITEMENABLED | HIGHCOMP, /* flags */
      0,                               /* mutual exclude */
      &ZoomText[5],                 /* text */
      },
   };


struct Menu MainMenu[MENU_COUNT] =
   {
      {
      &MainMenu[1],                    /* next menu */
      5, 0,                            /* select box left, top */
      70, 8,                           /* select box width, height */
      MENUENABLED,                     /* flags */
      "Display",                       /* text */
      &OptionsItems[0],                /* first item */
      0, 0, 0, 0,                      /* for the mystery variables */
      },
      {
      NULL,                            /* next menu */
      70 + 5, 0,                            /* select box left, top */
      70, 8,                           /* select box width, height */
      MENUENABLED,                     /* flags */
      "Zoom",                       /* text */
      &ZoomItems[0],                /* first item */
      0, 0, 0, 0,                      /* for the mystery variables */
      },
   };
