#include <stdlib.h>  /* exit() function defined here */
#include <stdio.h>   /* puts(), sprintf(), printf(), etc. */
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>

#include <aros/oldprograms.h>
#include <intuition/intuition.h>

#include "rable_images.h"
#include "rable_gadgets.h"


/****************************************************************/

/* image structures */

/* image structures for gadgets */
struct Image circle1 =
{0, 0, gad_w, gad_h, gad_depth, img_circle1, 7, 0, NULL};

struct Image circle2 =
{0, 0, gad_w, gad_h, gad_depth, img_circle2, 7, 0, NULL};

struct Image square1 =
{0, 0, gad_w, gad_h, gad_depth, img_square1, 7, 0, NULL};

struct Image square2 =
{0, 0, gad_w, gad_h, gad_depth, img_square2, 7, 0, NULL};

struct Image line0 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_line, 7, 0, NULL};

struct Image oval1 =
{0, 0, gad_w, gad_h, gad_depth, img_oval1, 7, 0, NULL};

struct Image oval2 =
{0, 0, gad_w, gad_h, gad_depth, img_oval2, 7, 0, NULL};

struct Image fill1 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_fill1, 7, 0, NULL};

struct Image fill2 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_fill2, 7, 0, NULL};

struct Image fill3 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_fill3, 7, 0, NULL};

struct Image fill4 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_fill4, 7, 0, NULL};

struct Image freestyle0 =
{0, 0, gad_w * 2, gad_h, gad_depth, img_free, 7, 0, NULL};

struct Image rectangle1 =
{0, 0, gad_w, gad_h, gad_depth, img_rec1, 7, 0, NULL};

struct Image rectangle2 =
{0, 0, gad_w, gad_h, gad_depth, img_rec2, 7, 0, NULL};

struct Image triangle1 =
{0, 0, gad_w, gad_h, gad_depth, img_tri1, 7, 0, NULL};

struct Image triangle2 =
{0, 0, gad_w, gad_h, gad_depth, img_tri2, 7, 0, NULL};
	
/* image structures for palette */
struct Image pcell1 =  {0, 0, 16, 16, 1, NULL, 0, 0, NULL};
struct Image pcell2 =  {0, 0, 16, 16, 1, NULL, 0, 1, NULL};
struct Image pcell3 =  {0, 0, 16, 16, 1, NULL, 0, 2, NULL};
struct Image pcell4 =  {0, 0, 16, 16, 1, NULL, 0, 3, NULL};
struct Image pcell5 =  {0, 0, 16, 16, 1, NULL, 0, 4, NULL};
struct Image pcell6 =  {0, 0, 16, 16, 1, NULL, 0, 5, NULL};
struct Image pcell7 =  {0, 0, 16, 16, 1, NULL, 0, 6, NULL};
struct Image pcell8 =  {0, 0, 16, 16, 1, NULL, 0, 7, NULL};
struct Image pcell9 =  {0, 0, 16, 16, 1, NULL, 0, 8, NULL};
struct Image pcell10 = {0, 0, 16, 16, 1, NULL, 0, 9, NULL};
struct Image pcell11 = {0, 0, 16, 16, 1, NULL, 0, 10, NULL};
struct Image pcell12 = {0, 0, 16, 16, 1, NULL, 0, 11, NULL};
struct Image pcell13 = {0, 0, 16, 16, 1, NULL, 0, 12, NULL};
struct Image pcell14 = {0, 0, 16, 16, 1, NULL, 0, 13, NULL};
struct Image pcell15 = {0, 0, 16, 16, 1, NULL, 0, 14, NULL};
struct Image pcell16 = {0, 0, 16, 16, 1, NULL, 0, 15, NULL};
struct Image pcell17 = {0, 0, 16, 16, 1, NULL, 0, 16, NULL};
struct Image pcell18 = {0, 0, 16, 16, 1, NULL, 0, 17, NULL};
struct Image pcell19 = {0, 0, 16, 16, 1, NULL, 0, 18, NULL};
struct Image pcell20 = {0, 0, 16, 16, 1, NULL, 0, 19, NULL};
struct Image pcell21 = {0, 0, 16, 16, 1, NULL, 0, 20, NULL};
struct Image pcell22 = {0, 0, 16, 16, 1, NULL, 0, 21, NULL};
struct Image pcell23 = {0, 0, 16, 16, 1, NULL, 0, 22, NULL};
struct Image pcell24 = {0, 0, 16, 16, 1, NULL, 0, 23, NULL};
struct Image pcell25 = {0, 0, 16, 16, 1, NULL, 0, 24, NULL};
struct Image pcell26 = {0, 0, 16, 16, 1, NULL, 0, 25, NULL};
struct Image pcell27 = {0, 0, 16, 16, 1, NULL, 0, 26, NULL};
struct Image pcell28 = {0, 0, 16, 16, 1, NULL, 0, 27, NULL};
struct Image pcell29 = {0, 0, 16, 16, 1, NULL, 0, 28, NULL};
struct Image pcell30 = {0, 0, 16, 16, 1, NULL, 0, 29, NULL};
struct Image pcell31 = {0, 0, 16, 16, 1, NULL, 0, 30, NULL};
struct Image pcell32 = {0, 0, 16, 16, 1, NULL, 0, 31, NULL};


/****************************************************************/


/* gadget structures to be attached to a window */

struct Gadget cgad1 =
{
NULL, 0, 0, gad_w, gad_h,    /* this one starts out selected */
GFLG_GADGHCOMP | GFLG_GADGIMAGE | GFLG_SELECTED,
GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &circle1, NULL, NULL, 0, NULL, circle_n, NULL
};


struct Gadget cgad2 =
{
&cgad1, gad_w + 1, 0, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &circle2, NULL, NULL, 0, NULL, circle_f, NULL
};


struct Gadget sgad1 =
{
&cgad2, gad_w * 2 + 1, 0, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &square1, NULL, NULL, 0, NULL, square_n, NULL
};

struct Gadget sgad2 =
{
&sgad1, gad_w * 3 + 1, 0, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &square2, NULL, NULL, 0, NULL, square_f, NULL
};

struct Gadget ogad1 =
{
&sgad2, 0, gad_h + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &oval1, NULL, NULL, 0, NULL, oval_n, NULL
};

struct Gadget ogad2 =
{
&ogad1, gad_w + 1, gad_h + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &oval2, NULL, NULL, 0, NULL, oval_f, NULL
};

struct Gadget lgad0 =
{
&ogad2, gad_w * 2 + 1, gad_h * 3 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &line0, NULL, NULL, 0, NULL, line, NULL
};

struct Gadget frsgad0 =
{
&lgad0, 0, gad_h * 3 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &freestyle0, NULL, NULL, 0, NULL, freestyle, NULL
};

struct Gadget recgad1 =
{
&frsgad0, gad_w * 2 + 1, gad_h + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &rectangle1, NULL, NULL, 0, NULL, rectangle_n, NULL
};

struct Gadget recgad2 =
{
&recgad1, gad_w * 3 + 1, gad_h + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &rectangle2, NULL, NULL, 0, NULL, rectangle_f, NULL
};

struct Gadget trigad1 =
{
&recgad2, 0, gad_h * 2 + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &triangle1, NULL, NULL, 0, NULL, triangle_n, NULL
};

struct Gadget trigad2 =
{
&trigad1, gad_w + 1, gad_h * 2 + 1, gad_w, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &triangle2, NULL, NULL, 0, NULL, triangle_f, NULL
};

/* structures for fill gadgets */

struct Gadget fgad1 =
{
&trigad2, 0, gad_h * 4 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &fill1, NULL, NULL, 0, NULL, fill_1, NULL
};

struct Gadget fgad2 =
{
&fgad1, gad_w * 2 + 1, gad_h * 4 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &fill2, NULL, NULL, 0, NULL, fill_2, NULL
};

struct Gadget fgad3 =
{
&fgad2, 0, gad_h * 5 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &fill3, NULL, NULL, 0, NULL, fill_3, NULL
};

struct Gadget fgad4 =
{
&fgad3, gad_w * 2 + 1, gad_h * 5 + 1, gad_w * 2, gad_h,
GFLG_GADGHCOMP | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &fill4, NULL, NULL, 0, NULL, fill_4, NULL
};

/* gadget structures for palette top row */
struct Gadget pgad1 =
{
NULL, 0, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell1, NULL, NULL, 0, NULL, 101, NULL
};
struct Gadget pgad2 = /* this one's initially selected */
{
&pgad1, 16, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE| GFLG_SELECTED,
GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell2, NULL, NULL, 0, NULL, 102, NULL
};
struct Gadget pgad3 =
{
&pgad2, 32, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell3, NULL, NULL, 0, NULL, 103, NULL
};
struct Gadget pgad4 =
{
&pgad3, 48, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell4, NULL, NULL, 0, NULL, 104, NULL
};
struct Gadget pgad5 =
{
&pgad4, 64, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell5, NULL, NULL, 0, NULL, 105, NULL
};
struct Gadget pgad6 =
{
&pgad5, 80, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell6, NULL, NULL, 0, NULL, 106, NULL
};
struct Gadget pgad7 =
{
&pgad6, 96, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell7, NULL, NULL, 0, NULL, 107, NULL
};
struct Gadget pgad8 =
{
&pgad7, 112, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell8, NULL, NULL, 0, NULL, 108, NULL
};
struct Gadget pgad9 =
{
&pgad8, 128, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell9, NULL, NULL, 0, NULL, 109, NULL
};
struct Gadget pgad10 =
{
&pgad9, 144, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell10, NULL, NULL, 0, NULL, 110, NULL
};
struct Gadget pgad11 =
{
&pgad10, 160, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell11, NULL, NULL, 0, NULL, 111, NULL
};
struct Gadget pgad12 =
{
&pgad11, 176, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell12, NULL, NULL, 0, NULL, 112, NULL
};
struct Gadget pgad13 =
{
&pgad12, 192, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell13, NULL, NULL, 0, NULL, 113, NULL
};
struct Gadget pgad14 =
{
&pgad13, 208, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell14, NULL, NULL, 0, NULL, 114, NULL
};
struct Gadget pgad15 =
{
&pgad14, 224, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell15, NULL, NULL, 0, NULL, 115, NULL
};
struct Gadget pgad16 =
{
&pgad15, 240, 0, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell16, NULL, NULL, 0, NULL, 116, NULL
};


/* gadget structures for palette bottom row */
struct Gadget pgad17 =
{
&pgad16, 0, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell17, NULL, NULL, 0, NULL, 117, NULL
};
struct Gadget pgad18 =
{
&pgad17, 16, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell18, NULL, NULL, 0, NULL, 118, NULL
};
struct Gadget pgad19 =
{
&pgad18, 32, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell19, NULL, NULL, 0, NULL, 119, NULL
};
struct Gadget pgad20 =
{
&pgad19, 48, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell20, NULL, NULL, 0, NULL, 120, NULL
};
struct Gadget pgad21 =
{
&pgad20, 64, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell21, NULL, NULL, 0, NULL, 121, NULL
};
struct Gadget pgad22 =
{
&pgad21, 80, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell22, NULL, NULL, 0, NULL, 122, NULL
};
struct Gadget pgad23 =
{
&pgad22, 96, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell23, NULL, NULL, 0, NULL, 123, NULL
};
struct Gadget pgad24 =
{
&pgad23, 112, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell24, NULL, NULL, 0, NULL, 124, NULL
};
struct Gadget pgad25 =
{
&pgad24, 128, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell25, NULL, NULL, 0, NULL, 125, NULL
};
struct Gadget pgad26 =
{
&pgad25, 144, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell26, NULL, NULL, 0, NULL, 126, NULL
};
struct Gadget pgad27 =
{
&pgad26, 160, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell27, NULL, NULL, 0, NULL, 127, NULL
};
struct Gadget pgad28 =
{
&pgad27, 176, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell28, NULL, NULL, 0, NULL, 128, NULL
};
struct Gadget pgad29 =
{
&pgad28, 192, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell29, NULL, NULL, 0, NULL, 129, NULL
};
struct Gadget pgad30 =
{
&pgad29, 208, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell30, NULL, NULL, 0, NULL, 130, NULL
};
struct Gadget pgad31 =
{
&pgad30, 224, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell31, NULL, NULL, 0, NULL, 131, NULL
};
struct Gadget pgad32 =
{
&pgad31, 240, 16, 16, 16,
GFLG_GADGHBOX | GFLG_GADGIMAGE, GACT_IMMEDIATE | GACT_TOGGLESELECT,
GTYP_BOOLGADGET, &pcell32, NULL, NULL, 0, NULL, 132, NULL
};


/* This routine gives the starting address of the gadget chain */
/* It must be updated by hand if the first gadget is changed */
struct Gadget *get_gadgets(void)
{
return (&fgad4);
}


/************** structures for canvas size requester **************/

UBYTE buff1[10];
UBYTE buff2[10];

struct StringInfo si1={buff1,NULL,0,10,0,0,0,0,0,0,NULL,NULL,NULL};
struct StringInfo si2={buff2,NULL,0,10,0,0,0,0,0,0,NULL,NULL,NULL};

struct IntuiText itext1={30,31,JAM1,-15,0,NULL,"W",     NULL};
struct IntuiText itext2={30,31,JAM1,-15,0,NULL,"H",     NULL};
struct IntuiText itext3={30,31,JAM1, 10,4,NULL,"OKAY",  NULL};
struct IntuiText itext4={30,31,JAM1,  4,4,NULL,"CANCEL",NULL};
struct IntuiText itext5={30,31,JAM1, 15,8,NULL,"Canvas Size:",NULL};

/* button border */
SHORT border1_points[]=
{0,0,  55,0,  55,15,  0,15,  0,0};
struct Border border1={0,0,30,31,JAM1,5,border1_points,NULL};

/* main border */
SHORT border2_points[]=
{2,2,  137,2,  137,77,  2,77,  2,2};
struct Border border2={0,0,30,31,JAM1,5,border2_points,NULL};

/* number entry border */
SHORT border3_points[]=
{0,0,  61,0,  61,13,  0,13,  0,0};
struct Border border3={-3,-3,30,31,JAM1,5,border3_points,NULL};


/* the requester's gadgets */
struct Gadget reqgad1={NULL,40,25,60,20,GFLG_GADGHCOMP|GFLG_TABCYCLE,
GACT_LONGINT,GTYP_STRGADGET|GTYP_REQGADGET,(APTR)&border3,NULL,&itext1,
NULL,(APTR)&si1,501,NULL};

struct Gadget reqgad2={&reqgad1,40,40,60,20,GFLG_GADGHCOMP|GFLG_TABCYCLE,
GACT_LONGINT,GTYP_STRGADGET|GTYP_REQGADGET,(APTR)&border3,NULL,&itext2,
NULL,(APTR)&si2,502,NULL};

struct Gadget reqgad3={&reqgad2,10,55,55,15,GFLG_GADGHCOMP, /* OKAY */
GACT_IMMEDIATE|GACT_RELVERIFY|GACT_ENDGADGET,
GTYP_BOOLGADGET|GTYP_REQGADGET,(APTR)&border1,NULL,&itext3,
NULL,NULL,503,NULL};

struct Gadget reqgad4={&reqgad3,74,55,55,15,GFLG_GADGHCOMP, /* CANCEL */
GACT_IMMEDIATE|GACT_RELVERIFY|GACT_ENDGADGET,
GTYP_BOOLGADGET|GTYP_REQGADGET,(APTR)&border1,NULL,&itext4,
NULL,NULL,504,NULL};

/************** structures for palette sliders **************/

struct IntuiText itext6={30,31,JAM1,-15,2,NULL,"R",NULL};
struct IntuiText itext7={30,31,JAM1,-15,2,NULL,"G",NULL};
struct IntuiText itext8={30,31,JAM1,-15,2,NULL,"B",NULL};

struct Image no_img1; /* dummy image */
struct Image no_img2;
struct Image no_img3;

struct PropInfo slider_info1={FREEHORIZ|AUTOKNOB,0,0,MAXBODY*1/16,
                              0,0,0,0,0,0,0,};
struct PropInfo slider_info2={FREEHORIZ|AUTOKNOB,0,0,MAXBODY*1/16,
                              0,0,0,0,0,0,0,};
struct PropInfo slider_info3={FREEHORIZ|AUTOKNOB,0,0,MAXBODY*1/16,
                              0,0,0,0,0,0,0,};

struct Gadget rslider={&pgad32,20,35,50,12,GFLG_GADGHCOMP,
GACT_IMMEDIATE|GACT_RELVERIFY,GTYP_PROPGADGET,(APTR)&no_img1,NULL,&itext6,
NULL,(APTR)&slider_info1,601,NULL};

struct Gadget gslider={&rslider,100,35,50,12,GFLG_GADGHCOMP,
GACT_IMMEDIATE|GACT_RELVERIFY,GTYP_PROPGADGET,(APTR)&no_img2,NULL,&itext7,
NULL,(APTR)&slider_info2,602,NULL};

struct Gadget bslider={&gslider,180,35,50,12,GFLG_GADGHCOMP,
GACT_IMMEDIATE|GACT_RELVERIFY,GTYP_PROPGADGET,(APTR)&no_img3,NULL,&itext8,
NULL,(APTR)&slider_info3,603,NULL};


struct Gadget *get_pal(void)
{
return (&bslider);
}

