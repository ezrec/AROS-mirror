
/*
 *  BCUBIC.C	    BEZIER CUBIC (3D) CURVED SURFACES
 *
 *  (C)Copyright 1987 by Matthew Dillon, All Rights Reserved.
 *  Permission to redistribute for non-profit only.
 *  Permission to embellish the source however you wish and redistribute.
 *
 *  Compiled with Aztec.  You MUST use the +L option (32 bit ints) or you
 *  are sunk.  You might have to specify more expression space for cc:
 *  (-E1000 should do it).   Addtionaly, the source expects all Amiga
 *  symbols to be precompiled, and MY.LIB to exist.  A good programmer
 *  would not have a problem removing the MY.LIB dependancies.
 *
 *  Usage:
 *	Prop. Gadget changes granularity.  Use the SELECT button to move
 *	control points on the X and Y axis.  Use the MENU button to move
 *	control points on the Z axis.
 *
 *	SELECT: right-left is X axis movement
 *		up-down    is Y axis movement
 *	MENU:	up-down    is Z axis movement
 *
 *	The 3D plane is a linear projection with the Z axis straight up,
 *	the X axis going to the right, and the Y axis going diagonal
 *	(lower-left to upper-right).
 *
 *  NOTE:
 *	This program has been incredibly optimized.  Do not attempt to
 *	gleam the matrix theory from the source unless you are familar
 *	with Bezier Cubic equations.
 */

/* !!!
 #include <typedefs.h>
*/ 
#include <aros/oldprograms.h>
#include <intuition/imageclass.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>

typedef struct Window WIN;
typedef struct NewWindow NW;
typedef struct IntuiMessage IMESS;
typedef struct RastPort RP;
typedef struct Gadget GADGET;
typedef struct IntuiText ITEXT;
struct GfxBase * GfxBase;
struct IntuitionBase * IntuitionBase;
/* !!!
 #include <xmisc.h>
*/


#define ONE 512
#define SSF 9
#define SSFD	(SSF*2)
#define MINGRAN 10		    /* must be at least 10  */
#define CSIZE	(ONE/MINGRAN+2)

typedef unsigned long ulong;
typedef unsigned short uword;
typedef unsigned char  ubyte;

typedef struct PropInfo XPI;
typedef struct Image	IM;

/*
extern IMESS *GetMsg();
extern char *malloc();
*/
extern GADGET Gadgets[];

#define MYGADGETS   (WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE)

NW Nw = {
    64, 64, 400, 120,
    0, 1,
    NEWSIZE|MOUSEBUTTONS|MOUSEMOVE|CLOSEWINDOW|GADGETDOWN|GADGETUP,
    MYGADGETS|REPORTMOUSE|ACTIVATE|NOCAREREFRESH|RMBTRAP,
    0, 0, (UBYTE *)"Bezier Cubic (3D curved surfaces), By Matthew Dillon", NULL, NULL,
    32, 64, -1, -1, WBENCHSCREEN
};

#define INTUITION_LIB 1
#define GRAPHICS_LIB 2
int openlibs(int dummy)
{
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
}

void closelibs(int dummy)
{
  if (GfxBase)
    CloseLibrary((struct Library *)GfxBase);
  if (IntuitionBase)
    CloseLibrary((struct Library *)IntuitionBase);
}

WIN *Win;
RP  *Rp;
short Ux, Uy, Mx, My;
short SStep = 512/16;
short TStep = 512/16;

main(ac, av)
char *av[];
{
    register IMESS *mess;
    char notdone = 1;
    short mx, my, basex, basey, mm, mrmb = 0;
    short gg, gy;
    short pt = -1;
    XPI *po;

    /* !!!
     disablebreak();
    */ 
    exiterr(!init_cubic(), "");
    exiterr(!openlibs(INTUITION_LIB|GRAPHICS_LIB), "unable to open libs");
    init_gadgets(&Nw, &po);
    exiterr(!(Win = (struct Window *)OpenWindow(&Nw)), "unable to open window");
    Rp = Win->RPort;
    SetAPen(Rp, 3);
    SetDrMd(Rp, JAM2);

    uparams();
    precalculate();
    plotcurve();
    plotcontrolpts();
    while (notdone) {
	WaitPort(Win->UserPort);
	mm = 0;
	gg = 0;
	while (mess = GetMsg(Win->UserPort)) {
	    switch(mess->Class) {
	    case CLOSEWINDOW:
		notdone = 0;
		break;
	    case NEWSIZE:
		uparams();
		precalculate();
		clearwindow();
		plotcurve();
		plotcontrolpts();
		break;
	    case GADGETUP:
	    case GADGETDOWN:
		if (mess->IAddress != (APTR)&Gadgets[0]) {
		    do_help();
		    break;
		}
		gy = po->VertPot / 256;
		gg = mess->Class;
		break;
	    case MOUSEBUTTONS:
		switch(mess->Code) {
		case SELECTDOWN:
		    pt = findpoint(mess->MouseX, mess->MouseY);
		    basex = mess->MouseX;
		    basey = mess->MouseY;
		    break;
		case SELECTUP:
		    pt = -1;
		    break;
		case MENUDOWN:
		    pt = findpoint(mess->MouseX, mess->MouseY);
		    basex = mess->MouseX;
		    basey = mess->MouseY;
		    mrmb = 1;
		    break;
		case MENUUP:
		    mrmb = 0;
		    pt = -1;
		    break;
		}
		break;
	    case MOUSEMOVE:
		if (gg == GADGETDOWN) {
		    gy = po->VertPot / 256;
		    break;
		}
		if (pt >= 0) {
		    mm = 1;
		    mx = mess->MouseX;
		    my = mess->MouseY;
		}
		break;
	    default:
		break;
	    }
	    ReplyMsg(mess);
	}
	if (gg) {
	    char buf[32];
	    mm = 0;
	    if (gg == GADGETUP)
		gg = 0;
	    if (gy + 10 >= 0 && gy + 10 != SStep) {
		char buf[32];
		SStep = gy + 10;
		TStep = SStep;
		precalculate();
		clearwindow();
		plotcurve();
		plotcontrolpts();
    	    	SetDrMd(Rp, JAM1);
		Move(Rp, 32, Win->BorderTop + 1 + Rp->TxBaseline);
		sprintf (buf, "Step: %-3ld", SStep);
		Text(Rp, buf, strlen(buf));
		
	    }
	}
	if (mm && pt >= 0) {
	    if (mrmb) {
		recalculate_one(pt, 0, 0, basey - my);
		basey = my;
	    } else {
		recalculate_one(pt, mx - basex, basey - my, 0);
		basex = mx;
		basey = my;
	    }
	    clearwindow();
	    plotcurve();
	    plotcontrolpts();
	}
    }
    exiterr(1, NULL);
}

exiterr(n, str)
char *str;
{
    if (n) {
	if (str)
	    puts(str);
	if (Win)
	    CloseWindow(Win);
	
	closelibs(-1);
	exit(1);
    }
}

uparams()
{
    Ux = Win->BorderLeft;
    Uy = Win->BorderTop;
    Mx = Win->Width - Win->BorderRight;
    My = Win->Height- Win->BorderBottom;
}

long	Pmatrix[3][4][4] = {	 0,50,100,150,	      /* X    */
				 0,50,100,150,
				 0,50,100,150,
				 0,50,100,150,

				 0, 0, 0, 0,	    /* Y    */
				16,16,16,16,
				32,32,32,32,
				48,48,48,48,

				 0,16,16, 0,	    /* Z    */
				16,32,32,16,
				16,32,32,16,
				 0,16,16, 0
			    };

long	Mmatrix[4][4] = {  -1,	3, -3,	1,
			    3, -6,  3,	0,
			   -3,	3,  0,	0,
			    1,	0,  0,	0
			};

long	Mmatrix_trans[4][4];

long	Amatrix[CSIZE][4];  /* # entries used depends on step rate  */
long	Bmatrix[CSIZE][4];  /* # entries used depends on step rate  */
long	*CXarray;	    /* each contains CSIZE*CSIZE*4 bytes    */
long	*CYarray;
long	*CZarray;
short	Amax, Bmax;


init_cubic()
{
    register short i, j;

    CXarray = (long *)malloc(CSIZE*CSIZE*4);
    CYarray = (long *)malloc(CSIZE*CSIZE*4);
    CZarray = (long *)malloc(CSIZE*CSIZE*4);
    if (!CXarray || !CYarray || !CZarray) {
	printf("unable to allocate %ld bytes\n", CSIZE*CSIZE*4*3);
	return(0);
    }
    /* Transpose Mmatrix    */
    for (i = 0; i < 4; ++i) {
	for (j = 0; j < 4; ++j)
	    Mmatrix_trans[i][j] = Mmatrix[j][i];
    }
    return(1);
}


/*
 *  return index of point closest to the given 2D window pixel coordinate.
 *
 *  To do this, each of the 16 control points must be transformed into their
 *  plot coordinates and the range to the specified window pixel taken.
 *  the point with the smallest range wins.
 *  (NOTE: Since we are using the ranges for comparison only, there is
 *   no need to take the square root)
 */

findpoint(wx, wy)
{
    register short i, pt;
    register ulong minrange = -1;
    ulong range;
    short x, y;

    pt = 0;
    for (i = 0; i < 16; ++i) {
	translate(Pmatrix[0][0][i], Pmatrix[1][0][i], Pmatrix[2][0][i], &x, &y);
	x -= wx;
	y -= wy;
	range = x*x + y*y;
	if (range < minrange) {
	    pt = i;
	    minrange = range;
	}
    }
    return(pt);
}

/*
 *  This need be called only when the step size changes... it calculates
 *  the A and B matrices from the following equation:
 *
 *	Overall equation:   C = SMPM.T. (Where X. means transpose of X)
 *	S = { s^3 s^2 s^1 1 }	s ranging 0 to 1
 *	T = { t^3 t^2 t^1 1 }	t ranging 0 to 1
 *
 *	Thus, a given step size will require calculation of a certain
 *	number of S and T matrices.  We might as well do the multiplication
 *	with M as well and stick the results in A and B for each step
 *
 *	A = SM
 *	B = M.T.
 *
 *	Note: As far as mmult_l() goes, any matrix with either the number
 *	of rows = 1 or number of columns = 1 can be transposed
 *	inherently (that is, it takes no work to transpose it).
 */

precalculate()
{
    long Smatrix[4], Tmatrix[4];

    register short s, t, i;

    Smatrix[3] = Tmatrix[3] = ONE;
    for (s = i = 0; s <= ONE; s += SStep) {
	Smatrix[2] = s;
	Smatrix[1] = (s * s) >> SSF;
	Smatrix[0] = (Smatrix[1] * s) >> SSF;
	mmult_l(Smatrix,Mmatrix,Amatrix[i],1,4,4);
	++i;
	if (s != ONE && s + SStep > ONE)
	    s = ONE - SStep;
    }
    Amax = i;
    for (t = i = 0; t <= ONE; t += TStep) {
	Tmatrix[2] = t;
	Tmatrix[1] = (t * t) >> SSF;
	Tmatrix[0] = (Tmatrix[1] * t) >> SSF;
	mmult_l(Mmatrix_trans,Tmatrix,Bmatrix[i],4,4,1);
	++i;
	if (t != ONE && t + TStep > ONE)
	    t = ONE - TStep;
    }
    Bmax = i;
    recalculate_all();
}

/*
 *  Recalculate all global points from the semi-compiled matrices
 *  A and B, and the P matrix (the 16 control points).
 *
 *  C = APB (matrix multiplication), for each dimension.  C is a 1x1
 *  matrix, which means that it is a simple number.
 */

recalculate_all()
{
    register short t, i, s, idx;
    register long *A, *B;
    long C[3];		/* 3D result Coordinates    */

    for (s = 0; s < Amax; ++s) {
	A = Amatrix[s];
	idx = s * Bmax;
	for (t = 0; t < Bmax; ++t, ++idx) {
	    B = Bmatrix[t];
	    for (i = 0; i < 3; ++i) {
		long T[4];
		mmult_l(A,Pmatrix[i],T,1,4,4);
		C[i] = T[0] * B[0] +
		       T[1] * B[1] +
		       T[2] * B[2] +
		       T[3] * B[3];
	    }
	    CXarray[idx] = C[0];
	    CYarray[idx] = C[1];
	    CZarray[idx] = C[2];
	}
    }
}

/*
 *  At this point, we have calculated all the global points and stuck
 *  them in a *huge* table.  If we now want to change one of the control
 *  points, we only have to recalculate one of the matrix elements.
 *  (But still have to loop through the entire grid).
 *
 *  Note that there is a possible optimization here, since this scheme
 *  allows us to determine which line segments will change when we do the
 *  actual plotting.
 */

recalculate_one(pt, dx, dy, dz)
{
    register short t, i, idx, col;
    register long  temp;
    long     arow;
    short    s, row;

    col = pt & 3;
    row = pt >> 2;

    Pmatrix[0][0][pt] += dx;	/* not required until a recalc_all()	*/
    Pmatrix[1][0][pt] += dy;
    Pmatrix[2][0][pt] += dz;

    for (s = 0; s < Amax; ++s) {
	arow = Amatrix[s][row];
	idx = s * Bmax;
	for (t = 0; t < Bmax; ++t, ++idx) {
	    temp = Bmatrix[t][col] * arow;
	    CXarray[idx] += temp * dx;
	    CYarray[idx] += temp * dy;
	    CZarray[idx] += temp * dz;
	}
    }
}


plotcurve()
{
    register short i, s, t, idx;
    uword x, y;
    static short Corr[CSIZE][2];

    for (s = 0; s < Amax; ++s) {
	idx = s * Bmax;
	translate(CXarray[idx]>>SSFD, CYarray[idx]>>SSFD, CZarray[idx]>>SSFD, &x, &y);
	Move(Rp, x, y);
	for (i = t = 0; t < Bmax; ++t, ++idx, ++i)
	    translate(CXarray[idx]>>SSFD, CYarray[idx]>>SSFD, CZarray[idx]>>SSFD, &Corr[i][0], &Corr[i][1]);
	PolyDraw(Rp, i, Corr);
    }
    for (t = 0; t < Bmax; ++t) {
	idx = t;
	translate(CXarray[idx]>>SSFD, CYarray[idx]>>SSFD, CZarray[idx]>>SSFD, &x, &y);
	Move(Rp, x, y);
	for (s = i = 0; s < Amax; ++s, idx += Bmax, ++i)
	    translate(CXarray[idx]>>SSFD, CYarray[idx]>>SSFD, CZarray[idx]>>SSFD, &Corr[i][0], &Corr[i][1]);
	PolyDraw(Rp, i, Corr);
    }
}


plotcontrolpts()
{
    register short i;
    short x, y;

    SetAPen(Rp, 1);
    for (i = 0; i < 16; ++i) {
	translate(Pmatrix[0][0][i], Pmatrix[1][0][i], Pmatrix[2][0][i], &x, &y);
	Move(Rp, x - 2, y + 0);
	Draw(Rp, x + 2, y + 0);
	Move(Rp, x + 0, y - 2);
	Draw(Rp, x + 0, y + 2);
    }
    SetAPen(Rp, 3);
}

translate(x, y, z, wx, wy)
uword *wx, *wy;
{
    *wx = Ux + 50 + x + y/2;
    *wy = My - 50 - (z + y/2);
}

/*
 *  MATRIX ROUTINES
 *
 *  mmult_l(a,b,d,n1,n2,n3)
 *
 *  a	n1 x n2
 *  b	n2 x n3
 *  d	n1 x n3
 *
 *  NOTE: This isn't the most efficient way to handle matrix multiplication.
 */

mmult_l(a,b,d,n1,n2,n3)
long *a, *b, *d;
{
    register short i, j, k;
    register long *an2 = a;
    register long *bn3;
    register long *dn3 = d;
    register long sum;

    for (i = 0; i < n1; ++i, dn3 += n3, an2 += n2) {
	for (j = 0; j < n3; ++j) {
	    sum = 0;
	    for (k = 0, bn3 = b; k < n2; ++k, bn3 += n3)
		sum += *(an2+k) * *(bn3+j);
	    *(dn3+j) = sum;
	}
    }
}


/*
 *  MISC
 */

clearwindow()
{
    SetAPen(Rp, 0);
    RectFill(Rp, Ux, Uy, Mx - 1, My - 1);
    SetAPen(Rp, 3);
}

/*
 *  GADGET ROUTINES!	------------------------------------------------
 */

#define NG(nn)	&Gadgets[nn+1]
#define G_YGLOB 1
#define G_XGLOB 2

XPI Props[] = {
    { AUTOKNOB|FREEVERT|PROPNEWLOOK|PROPBORDERLESS , 0, 0, 0x1FFF, 0x1FFF }
};

short pairs[] = { 0,0,10,0,10,10,0,10 };

struct Border Border[] = {
    { 0,0,1,1,JAM2,4,pairs,NULL }
};

ITEXT Itext[] = {
    { 0,1,JAM2,3,2,NULL,(ubyte *)"?",NULL }
};

IM Images[] = {
    { 0,0,2,1,1, NULL, 1, 0, NULL },
};

GADGET Gadgets[] = {
    {
	&Gadgets[1], -15, 22, 15, -19, GADGIMAGE|GADGHCOMP|GRELRIGHT|GRELHEIGHT,
	GADGIMMEDIATE|RIGHTBORDER|RELVERIFY,PROPGADGET,
	(APTR)&Images[0],NULL,NULL,0,(APTR)&Props[0], G_YGLOB, 0
    },
    {
	NULL,	     -15, 11, 15, 11,  GRELRIGHT,
	RELVERIFY|RIGHTBORDER, BOOLGADGET,
	(APTR)&Border[0],NULL,&Itext[0],0,0,0,0
    }
};

GADGET *Gc;
long GUx, GUy;

init_gadgets(nw, ppo)
NW *nw;
XPI **ppo;
{
    struct Screen *scr;
    struct DrawInfo *dri;    
    Object *sizeim;
    WORD sizewidth  = 14;
    WORD sizeheight = 14;
    
    scr = LockPubScreen(NULL);
    if (scr)
    {
    	dri = GetScreenDrawInfo(scr);
	if (dri)
	{
	    sizeim = NewObject(NULL, SYSICLASS, SYSIA_DrawInfo, (IPTR)dri,
	    	    	    	    	        SYSIA_Which, SIZEIMAGE,
						TAG_DONE);
	
	    if (sizeim)
	    {
	    	sizewidth = ((struct Image *)sizeim)->Width;
	    	sizeheight = ((struct Image *)sizeim)->Height;
	    	DisposeObject(sizeim);
	    }
	    
	    Itext[0].DrawMode = JAM1;
	    Itext[0].FrontPen = dri->dri_Pens[SHINEPEN];
	    	    
	    Itext[0].LeftEdge = (sizewidth - GfxBase->DefaultFont->tf_XSize) / 2;
	    
	    Gadgets[1].LeftEdge = -sizewidth + 1;
	    Gadgets[1].TopEdge = scr->WBorTop + scr->Font->ta_YSize + 1;
	    Gadgets[1].Width = sizewidth;
	    Gadgets[1].Height = GfxBase->DefaultFont->tf_YSize + 2;
    	    Gadgets[1].GadgetRender = NULL; /* no border */
	    
	    Gadgets[0].LeftEdge = -sizewidth + 1 + 3;			
            Gadgets[0].TopEdge = scr->WBorTop + scr->Font->ta_YSize + 1 + Gadgets[1].Height + 2;
	    Gadgets[0].Width  = sizewidth - 6;
	    Gadgets[0].Height = -Gadgets[0].TopEdge - sizeheight - 2;
	    	    
	    FreeScreenDrawInfo(scr, dri);
    	}
	UnlockPubScreen(NULL, scr);
    }
    
    nw->FirstGadget = &Gadgets[0];
    *ppo = &Props[0];
}


do_help()
{
    long fh;
    static char *help[] = {
"",
"(C)Copyright 1987 by Matthew Dillon, All Rights Reserved",
"Freely distributable for non-profit only",
"",
"Bezier Cubic Surfaces.  Allows you to fool around with the Bezier Cubic",
"equation for 3D curved surface generation.  The Bezier form uses 16",
"control points to define the surface.",
"",
"To move a control point along the Z axis, position the mouse over the",
"control point in question and move it UP or DOWN with the MENU BUTTON",
"held down.  To move a control point along the X or Y axis, position the",
"mouse over the control point in question and move it LEFT/RIGHT or",
"UP/DOWN with the SELECT BUTTON held down.  The Y axis is on the diagonal,",
"in a lower-left to upper-right going direction.",
"",
"The prop. Gadget is used to define the granualarity of the surface.",
"",
"(Return to continue)",
	NULL
    };

    fh = Open("con:0/0/640/200/HELP", 1006);
    if (fh) {
	register short i;
	char c;
	for (i = 0; help[i]; ++i) {
	    Write(fh, help[i], strlen(help[i]));
	    Write(fh, "\n", 1);
	}
	Read(fh, &c, 1);
	Close(fh);
    }
}


