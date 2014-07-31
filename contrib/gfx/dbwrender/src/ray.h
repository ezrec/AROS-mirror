/************************************************************************
 *                                                                      *
 *                  Copyright (c) 1987, David B. Wecker                 *
 *                          All Rights Reserved                         *
 *                                                                      *
 * This file is part of DBW_Render                                      *
 *                                                                      *
 * DBW_Render is distributed in the hope that it will be useful, but    *
 * WITHOUT ANY WARRANTY. No author or distributor accepts               *
 * responsibility to anyone for the consequences of using it or for     *
 * whether it serves any particular purpose or works at all, unless     *
 * he says so in writing. Refer to the DBW_Render General Public        *
 * License for full details.                                            *
 *                                                                      *
 * Everyone is granted permission to copy, modify and redistribute      *
 * DBW_Render, but only under the conditions described in the           *
 * DBW_Render General Public License. A copy of this license is         *
 * supposed to have been given to you along with DBW_Render so you      *
 * can know your rights and responsibilities. It should be in a file    *
 * named COPYING. Among other things, the copyright notice and this     *
 * notice must be preserved on all copies.                              *
 ************************************************************************
 *                                                                      *
 * Authors:                                                             *
 *      DBW - David B. Wecker                                           *
 *      jhl - John H. Lowery (IBM conversion)                           *
 *                                                                      *
 * Versions:                                                            *
 *      V1.0  870125 DBW - First released version                       *
 *      V1.01 880918 jhl - ported to IBM PC (MCGA/VGA display hardware) *
 *            890121 jhl - MAXROW & MAXCOL become variables, add        *
 *                         'D' command for Display max <hor> <vert>     *
 *                                                                      *
 ************************************************************************/

#include <math.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#ifdef IBM_PC
#include <malloc.h>
#endif

#ifdef __AROS__
#define VERSION "RAY v1.02 040102 (AROS)\n Copyright (C) 1989 J. Lowery and D. Wecker - all rights reserved\n"
#else
#define VERSION "RAY v1.02 040102 (WIN32)\n Copyright (C) 1989 J. Lowery and D. Wecker - all rights reserved\n"
#endif

#define pi 3.141592654

#define X 0
#define Y 1
#define Z 2
#define W 3
#define B 0
#define G 1
#define R 2

#define minimum( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define maximum( a, b ) ( ((a) > (b)) ? (a) : (b) )

typedef float vector[3];
typedef float matrix[4][4];

typedef struct 
{
     float xmin, xmax;
     float ymin, ymax;
}
window;

#define setwindow( xmin, ymin, xmax, ymax, w ) {\
    w.xmin = xmin;\
    w.ymin = ymin;\
    w.xmax = xmax;\
    w.ymax = ymax; }

#define copywindow( f, w ) {\
    w.xmin = f.xmin;\
    w.ymin = f.ymin;\
    w.xmax = f.xmax;\
    w.ymax = f.ymax; }

typedef struct 
{
     float xmin, xmax;
     float ymin, ymax;
     float zmin, zmax;
}
volume;

#define setvolume( xmin, ymin, zmin, xmax, ymax, zmax, v ) {\
    v.xmin = xmin;\
    v.ymin = ymin;\
    v.zmin = zmin;\
    v.xmax = xmax;\
    v.ymax = ymax;\
    v.zmax = zmax; }

#define copyvolume( f, v ) {\
    v.xmin = f.xmin;\
    v.ymin = f.ymin;\
    v.zmin = f.zmin;\
    v.xmax = f.xmax;\
    v.ymax = f.ymax;\
    v.zmax = f.zmax; }

typedef struct 
{
     vector vrp, vpn, vup, cop;
     volume vol;
}
perspective_projection;

typedef struct 
{
     int tex;
     float fuz, ref, idx;
     vector tra, amb, dif;
}
attributes;

typedef struct 
{
     vector p;
     float g;
     int s;
}
fracvert;

#define CV(x,y,z,v)     v[0]=(x); v[1]=(y); v[2]=(z)
#define VECZERO(v)      v[0] = 0.0; v[1] = 0.0; v[2] = 0.0
#define VECCOPY(frm,to) to[0] = frm[0]; to[1] = frm[1]; to[2] = frm[2]
#define VECDUMP(v,str)  printf("%s\t%5.3f %5.3f %5.3f\n",str,v[0],v[1],v[2])
#define VECSUB(v1,v2,r) r[0] = v1[0] - v2[0]; r[1] = v1[1] - v2[1];\
                        r[2] = v1[2] - v2[2]
#define VECSUM(v1,v2,r) r[0] = v1[0] + v2[0]; r[1] = v1[1] + v2[1];\
                        r[2] = v1[2] + v2[2]
#define VECMUL(v1,v2,r) r[0] = v1[0] * v2[0]; r[1] = v1[1] * v2[1];\
                        r[2] = v1[2] * v2[2]
#define VECDIV(v1,v2,r) r[0] = v1[0] / v2[0]; r[1] = v1[1] / v2[1];\
                        r[2] = v1[2] / v2[2]
#define VECSCALE(s,v,r) r[0] = (s)*v[0]; r[1] = (s)*v[1]; r[2] = (s)*v[2]
#define NORM(v)         ((float)sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2])))
#define DOT(v1,v2)      ((v1[0]*v2[0])+(v1[1]*v2[1])+(v1[2]*v2[2]))
#define CROSS(v1,v2,r)  r[0] = (v1[1] * v2[2]) - (v2[1] * v1[2]);\
                        r[1] = (v1[2] * v2[0]) - (v2[2] * v1[0]);\
                        r[2] = (v1[0] * v2[1]) - (v2[0] * v1[1])
#define DIRECTION(f,t,d)        VECSUB(t,f,d); normalize(d)
#define SPHERENORMAL(cent,p,n)  DIRECTION(cent,p,n);
#define PLANENORMAL(ve,vp,n)    CROSS(vp,ve,n); normalize(n);

#define ERROR( str ) { fprintf( stderr, "%s\n", str ); exit(0); }

#ifndef IBM_PC
extern void *malloc();
#endif

extern long time();

#define CHECK_ALLOC( ptr, typ ) {\
    if (!(ptr = (typ *) malloc( sizeof( typ )))) {\
        ERROR( "MALLOC - out of memory" ); \
        exit( 0 ); \
        } \
    }

#define CHECK_ALLOCA( ptr, typ, num ) {\
    if (!(ptr = (typ *) malloc( sizeof( typ ) * num ))) {\
        ERROR( "MALLOC - out of memory" ); \
        exit( 0 ); \
        } \
    }

#define sptr( p ) ((sphere *) p)
#define tptr( p ) ((triangle *) p)
#define qptr( p ) ((quad *) p)
#define rptr( p ) ((ring *) p)
#define eptr( p ) ((extent *) p)
#define cptr( p ) ((cylinder *) p)

#define EXTENT 0
#define SPHERE 1
#define TRIANGLE 2
#define QUAD 3
#define RING 4
#define CYLINDER 5

typedef float hvector[4];

typedef struct NODE 
{
     struct NODE *next;
     int kind;
     attributes attr;
}
node;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector position, ve, vp;
}
quad;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector position, ve, vp;
}
triangle;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector position, ve, vp;
     float minrad, maxrad;
}
ring;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector center;
     float radius;
}
sphere;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector bottom, top;
     float a, b, c;
}
cylinder;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector base, apex;
     float baserad;
}
cone;

typedef struct 
{
     node *next;
     int kind;
     attributes attr;
     vector center;
     float radius;
     node *sub;
}
extent;

typedef struct 
{
     vector intensity;
     vector direction;
     float distscale, radius;
     int kind;
}
lighttype;

typedef struct 
{
     vector center;
     float wavelength, amplitude, drag, propagate;
}
wavetype;

typedef struct 
{
     vector color;
     float distscale;
}
hazetype;

typedef struct 
{
     vector color;
     float start, scale;
}
blendtype;

typedef struct 
{
     float start, altscale, altfactor, threshhold;
}
snowtype;

typedef struct 
{
     float scale, zoom;
}
pebbletype;

typedef struct 
{
     int level;
     float xscale, yscale, zscale;
     int texture;
}
fractaltype;

typedef struct 
{
     vector color;
     float x, y, z, bevel, angle;
     int beveltype;
}
checkertype;

typedef struct 
{
     vector othercolor;
     float thickscale, ringspacing, turbscale;
     int squeeze;
}
woodtype;

typedef struct 
{
     vector veincolor;
     float xscale, turbscale;
     int squeeze;
}
marbletype;

typedef struct 
{
     vector min, max;
}
rextent;

#define unionvector( re, v, nre ) {\
    int i; \
    for (i = 0; i < 3; i++) {\
        (nre)->min[i] = minimum( (re)->min[i], (v)[i] ); \
        (nre)->max[i] = maximum( (re)->max[i], (v)[i] ); \
        } \
    }

#define unionrextent( re1, re2, nre ) {\
    int i; \
    for (i = 0; i < 3; i++) {\
        (nre)->min[i] = minimum( (re1)->min[i], (re2)->min[i] ); \
        (nre)->max[i] = maximum( (re1)->max[i], (re2)->max[i] ); \
        } \
    }

#define MAXX     1600           /* absolute maximum pixel columns    */
#define MAXY     1200           /* absolute maximum pixel rows       */

#define BPP         8           /* Bits resolution per primary color */

#define DISPWID     11          /* Width of display surface          */
#define DISPHI      8           /* Height of display surface         */

#define MAXGRAY     (1 << 7) /* (1 << BPP) */
#define PPW         (sizeof( int ) * 8 / BPP)
#define WPSL        (MAXX / PPW)
#define MAXLIT      5
#define MAXPEBBLE   2
#define MAXSNOW     2
#define MAXBLEND    2
#define MAXWAVE     10
#define MAXHAZE     2
#define MAXCHECKER  2
#define MAXFRACTAL  2
#define MAXWOOD     2
#define MAXMARBLE   2
#define SMALL       0.001
#define MAXOBJ      50  /* maximum object intersections for one ray */

#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

typedef int         scanlinetype[ WPSL ];
typedef float       matrix3[3][3];

#ifdef MODULE_RAY
node                *root,
*g_objpairs[ MAXOBJ ];
FILE                *df,*fp,*fpout;
char                fname[256], outname[256], str[256];
scanlinetype        blueline,
greenline,
redline;
lighttype           light[ MAXLIT ];
marbletype          marble[ MAXMARBLE ];
woodtype            wood[ MAXWOOD ];
fractaltype         fractal[ MAXFRACTAL ];
checkertype         checker[ MAXCHECKER ];
pebbletype          pebble[ MAXPEBBLE ];
snowtype            snow[ MAXSNOW ];
blendtype           blend[ MAXBLEND ];
hazetype            haze[ MAXHAZE ];
wavetype            wave[ MAXWAVE ];
int                 sline,
  curr_runs,
  max_runs,
  allopaque = 1,
  numlits = 0,
  numwaves = 0,
  numcheckers = 0,
  numfractals = 0,
  numwoods = 0,
  nummarble = 0,
  numblends = 0,
  numsnows = 0,
  numhazes = 0,
  numpebbles = 0,
  ambientlight = 0,
  amblitnum = 1,
  amblitdenom = 2,
  antialias = 1,
  dopseudo = 0,
  histogram = 0,
  startrow = 0,
  endrow = MAXY,
  g_objcounter;
long                sort_size = 0L,
  sorts = 0L,
  total_runs = 0L,
  max_intersects = 0L,
  stacktop,
  stackbot;
float               maxhours = 12.0,
  brickheight = 3.0,
  brickwidth = 8.0,
  brickdepth = 4.0, 
  brickmortar = 0.5,
  idxref = 1.0,
  aperture = 0.0,
  focus = 100,
  ambscale = -1.0,
  variance = 0.00097,
  brickmortarwidth,
  brickmortarheight,
  brickmortardepth,
  sqrt3,
  sqrt3times2,
  d_maxgray,
  g_distances[ MAXOBJ ];
vector              mortardiffuse = 
{
     0.8, 0.85, 0.99 
}
,
eye,
vrp,
vu,
vr,
backgroundval,
g_points[ MAXOBJ ],
xaxis = 
{
     1.0, 0.0, 0.0 
}
,
yaxis = 
{
     0.0, 1.0, 0.0 
}
,
zaxis = 
{
     0.0, 0.0, 1.0 
}
;
matrix              woodorient;
jmp_buf             env;
int                 MAXROW = 600;  /* default */
int                 MAXCOL = 800;  /* default */

#else

extern node         *root,
  *g_objpairs[];
extern FILE         *df,*fp,*fpout;
extern char         fname[],  outname[], str[];
extern scanlinetype blueline, greenline, redline;
extern lighttype    light[];
extern marbletype   marble[];
extern woodtype     wood[];
extern fractaltype  fractal[];
extern checkertype  checker[];
extern pebbletype   pebble[];
extern snowtype     snow[];
extern blendtype    blend[];
extern hazetype     haze[];
extern wavetype     wave[];
extern int          sline,
  curr_runs,
  max_runs,
  allopaque,
  numlits,
  numwaves,
  numcheckers,
  numfractals,
  numwoods,
  nummarble,
  numblends,
  numsnows,
  numhazes,
  numpebbles,
  ambientlight,
  amblitnum,
  amblitdenom,
  antialias,
  dopseudo,
  histogram,
  startrow,
  endrow,
  g_objcounter;
extern long         sort_size,
  sorts,
  total_runs,
  max_intersects,
  stacktop,
  stackbot;
extern float        maxhours,
  brickheight,
  brickwidth,
  brickdepth,
  brickmortar,
  idxref,
  aperture,
  focus,
  ambscale,
  variance,
  brickmortarwidth,
  brickmortarheight,
  brickmortardepth,
  sqrt3,
  sqrt3times2,
  d_maxgray,
  g_distances[];
extern vector       mortardiffuse,
  eye,
  vrp,
  vu,
  vr,
  backgroundval,
  g_points[],
  xaxis,
  yaxis,
  zaxis;
extern matrix       woodorient;
extern jmp_buf      env;

extern int          MAXROW;
extern int          MAXCOL;

#endif

#ifndef MODULE_CALC
extern void         spherenormal();     /* center to dir                */
extern void         planenormal();      /* ve vp n                      */
extern void         calcripple();       /* point w ripple               */
extern void         noise3();           /* point total                  */
extern float        noise();            /* point                        */
extern float        turbulence();       /* point                        */
extern void         dodirection();      /* val eye2 d2 atten amblits    */
#endif

#ifndef MODULE_EXTENT
extern void         setextent();        /* ep re                        */
extern void         getextent();        /* np re                        */
#endif

#ifndef MODULE_HIT
extern void         findnormal();       /* np p n                       */
extern int          hitcylinder();      /* top bottom a b c eye d p t   */
extern int          hitsphere();        /* center radius eye d p t      */
extern int          hitplane();         /* p ve vp eye d sfs inter      */
extern int          hittriangle();      /* tp eye d p t                 */
extern int          hitquad();          /* qp eye d p t                 */
extern int          hitring();          /* rp eye d p t                 */
extern void         shell();            /* v v1 v2 n                    */
#endif

#ifndef MODULE_FILEIO
extern void         dumpnode();         /* n                            */
extern void         copyattr();         /* oa na                        */
extern void         read_vec();         /* v                            */
extern void         read_attr();        /* attr                         */
extern void         dofractal();        /* level a b c attr             */
extern void         readimagefile();    /* opp                          */
extern void         getinput();         /* argc argv                    */
extern void         write_scanline();   /*                              */
extern void         getoutput();        /* argc argv                    */
#endif

#ifndef MODULE_INTER
extern node *get_next_intersection();   /* which best_p best_t          */
extern void         add_intersection(); /* np p t attenuating           */
extern void       calc_intersections(); /* np eye d attenuating         */
extern void         all_intersects();   /* eye d attenuating            */
extern int          bounce_lighting();  /* pintens plitdir bouncep lit  */
extern void         blendcolor();       /* orig target scale result     */
extern void         getatten();         /* atten p d lit pointdist      */
#endif

#ifndef MODULE_MATH
extern void         veczero();          /* v                            */
extern void         veccopy();          /* from to                      */
extern void         vecdump();          /* v str                        */
extern float        hlsvalue();         /* n1 n2 hue                    */
extern void         cv();               /* x y z v                      */
extern void         hls();              /* h l s v                      */
extern void         vecsub();           /* v1 v2 r                      */
extern void         vecsum();           /* v1 v2 r                      */
extern void         vecmul();           /* v1 v2 r                      */
extern void         vecdiv();           /* v1 v2 r                      */
extern void         vecscale();         /* s v r                        */
extern float        norm();             /* v                            */
extern void         normalize();        /* v                            */
extern float        dot();              /* v1 v2                        */
extern void         cross();            /* v1 v2 r                      */
extern void         direction();        /* f t d                        */
extern long         curstack();         /*                              */
#endif

#ifndef MODULE_RND
extern float    rnd();
#endif

#ifndef MODULE_TEXTURE
extern void         gettex();           /* diffuse np p n               */
#endif

#ifndef MODULE_VAL
extern void         getval();           /* val np p d atten ambientlit  */
#endif

